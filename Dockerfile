FROM java:jre

RUN apt-get update
RUN apt-get install -y cmake nodejs
RUN apt-get install -y python2.7
RUN apt-get install -y git build-essential

RUN ln -s /usr/bin/nodejs /usr/bin/node && \
  ln -s /usr/bin/python2.7 /usr/bin/python

RUN curl https://s3.amazonaws.com/mozilla-games/emscripten/releases/emsdk-portable.tar.gz > /root/emsdk-portable.tar.gz && \
  cd /root && tar -zxvf emsdk-portable.tar.gz && \
  cd emsdk_portable && \
  ./emsdk update && \
  ./emsdk install latest

RUN cd /root/emsdk_portable && \
  ./emsdk activate latest && \
  ln -s /usr/bin/python /usr/bin/python2

COPY entrypoint.sh /root/

ENTRYPOINT ["/bin/bash", "/root/entrypoint.sh"]
