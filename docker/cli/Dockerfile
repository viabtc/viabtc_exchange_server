FROM ubuntu:16.04
MAINTAINER Sergey Dolin <sergey@s4y.solutions>

RUN apt-get update && \
    apt-get upgrade -y && \
    apt-get install -y \
      mysql-client \
      inetutils-ping \
      net-tools \
      lsof \
      telnet && \
    rm -rf /var/lib/apt/lists/* 

# Run docker indefinetly
RUN echo 'while true;do sleep 3600;done' > /wait
RUN chmod +x /wait
CMD /wait
