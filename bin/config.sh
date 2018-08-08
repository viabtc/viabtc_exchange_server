#!/bin/bash
# Exports functions parsing the services configs

# Internal utils
servicesdir="`dirname ${0}`/.."

function get () {
	filter=$1
	service=$2
  config="${servicesdir}/$service/config.json"
	ret=`jq "$filter[0]" < $config`
	test -z $ret || test $ret = null && $ret=''
	if [ -z $ret ]; then
	  ret=`jq $filter[0] < $config`
		test -z ret && return 1
	fi
	echo $ret
}

function trim_quotes () {
	echo $1|sed -e's/^"//'|sed -e's/"$//'
}

function trim_port() {
	echo $1|sed -e's/^[^@]*@//'
}

function trim_quotes_port() {
	echo $1|sed -e's/^"//'|sed -e's/"$//'|sed -e's/^[^@]*@//'
}

# Exported functions
function accesshttp_bind () {
	if [ -z $REMOTE ]; then
		# get .svr.bind accesshttp
		ip=$( get .svr.bind accesshttp )
		# echo "ip=$ip"
		trim_quotes_port $ip
	else
		echo $REMOTE
  fi
}
