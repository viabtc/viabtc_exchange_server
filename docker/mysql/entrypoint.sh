#!/bin/sh
MYSQL_ROOT_PASSWORD=root
MYSQL_DATABASE_HISTORY=trade_history
MYSQL_DATABASE_LOG=trade_log
MYSQL_USER=exchange
MYSQL_PASSWORD=exchange
SOCKET=/var/run/mysqld/mysqld.sock

_check_config() {
	toRun=`"$@" --verbose --help`
	if ! errors="`"${toRun[@]}" 2>&1 >/dev/null`"; then
		cat >&2 <<-EOM
			ERROR: mysqld failed while attempting to check config
			command was: "${toRun[*]}"
			$errors
		EOM
		exit 1
	fi
}

_get_config() {
	local conf="$1"; shift
	"$@" --verbose --help --log-bin-index="`mktemp -u`" 2>/dev/null | awk '$1 == "'"$conf"'" { print $2; exit }'
}

if [ "$1" = 'mysqld' -a "`id -u`" = '0' ]; then
	# _check_config "$@"

	DATADIR="`_get_config 'datadir' "$@"`"
	mkdir -p "$DATADIR"
	chown -R mysql:mysql "$DATADIR"
	mkdir -p `dirname $SOCKET`
	chown -R mysql:mysql `dirname $SOCKET`

	if [ ! -d "$DATADIR/mysql" ]; then
		echo "Initializing database in $DATADIR/mysql"
		mysql_install_db --verbose --user=mysql
		echo 'Database initialized'

		echo 'Start mysql to change root password'
		su -c "$@ --skip-networking --socket=${SOCKET}&" mysql -s /bin/sh

		mysql="mysql --protocol=socket -uroot -hlocalhost --socket=${SOCKET}"

		i=0
		while [ $i -lt 30 ]; do
			if echo 'SELECT 1' | /bin/sh -c "$mysql" &> /dev/null; then
				break
			fi
			echo 'MySQL init process in progress...'
			sleep 1
			i=`echo $((i + 1))`
		done

		if [ $i -eq 30  ]; then
			echo >&2 'MySQL init process failed.'
			exit 1
		fi

		echo 'SET PASSWORD'

		/bin/sh -c "${mysql}" <<-EOSQL
			-- What's done in this file shouldn't be replicated
			--  or products like mysql-fabric won't work
			SET @@SESSION.SQL_LOG_BIN=0;
			DELETE FROM mysql.user WHERE user NOT IN ('mysql.session', 'mysql.sys', 'root') OR host NOT IN ('localhost') ;
			SET PASSWORD FOR 'root'@'localhost'=PASSWORD('${MYSQL_ROOT_PASSWORD}') ;
			GRANT ALL ON *.* TO 'root'@'localhost' WITH GRANT OPTION ;
			DROP DATABASE IF EXISTS test ;
			FLUSH PRIVILEGES ;
			CREATE USER '$MYSQL_USER'@'%' IDENTIFIED BY '$MYSQL_PASSWORD';
			CREATE DATABASE $MYSQL_DATABASE_HISTORY;
			CREATE DATABASE $MYSQL_DATABASE_LOG;
			GRANT ALL ON $MYSQL_DATABASE_HISTORY.* TO '$MYSQL_USER'@'%' ;
			GRANT ALL ON $MYSQL_DATABASE_LOG.* TO '$MYSQL_USER'@'%' ;
			FLUSH PRIVILEGES ;
		EOSQL

		mysql="mysql --protocol=socket -u${MYSQL_USER} -p${MYSQL_PASSWORD} -hlocalhost --socket=${SOCKET} $MYSQL_DATABASE_HISTORY"
		echo "${mysql} < /docker-entrypoint-initdb.d/create_trade_history.sql";
		/bin/sh -c "${mysql}" < "/docker-entrypoint-initdb.d/create_trade_history.sql";

		mysql="mysql --protocol=socket -u${MYSQL_USER} -p${MYSQL_PASSWORD} -hlocalhost --socket=${SOCKET} $MYSQL_DATABASE_LOG"
		echo "${mysql} < /docker-entrypoint-initdb.d/create_trade_log.sql";
		/bin/sh -c "${mysql}" < "/docker-entrypoint-initdb.d/create_trade_log.sql";

		sed -i.bak "s/MYSQL_USER=.*$/MYSQL_USER=\"${MYSQL_USER}\"/" /docker-entrypoint-initdb.d/init_trade_history.sh
		sed -i.bak "s/MYSQL_PASS=.*$/MYSQL_PASS=\"${MYSQL_PASSWORD}\"/" /docker-entrypoint-initdb.d/init_trade_history.sh
		sed -i.bak "s/MYSQL_DB=.*$/MYSQL_DB=\"${MYSQL_DATABASE_HISTORY}\"/" /docker-entrypoint-initdb.d/init_trade_history.sh
		/docker-entrypoint-initdb.d/init_trade_history.sh

	  PIDFILE="`_get_config 'pid-file' "$@"`"
		pid=`cat $PIDFILE`

		if ! kill -s TERM "$pid" ; then
			echo >&2 'MySQL kill process failed.'
			if ! wait "$pid"; then
				echo >&2 'MySQL wait process failed.'
				exit 1
			fi
		fi

		echo
		echo 'MySQL init process done. Ready for start up.'
		echo
	fi
fi
exec su -c "$*" mysql -s /bin/sh
