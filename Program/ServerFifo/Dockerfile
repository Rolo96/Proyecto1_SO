############################################################
# Technological Institute of Costa Rica                    #
# Project Operating Systems                                #
# Teacher: Diego Vargas                                    #
# Students: Raul Arias, Bryan Abarca, Rony Paniagua        #
# Docker file fifo web server                              #
############################################################

# Setting docker base
FROM gcc:4.9
COPY . /usr/src/WebServerFifo
WORKDIR /usr/src/WebServerFifo

#Initial port expose
EXPOSE 8081

#Install dependencies (nano, curl) 
RUN apt-get update; exit 0
RUN apt-get install -y nano
RUN apt-get install -y curl
RUN export TERM=xterm

#Command to run web server
CMD make && tail -f /dev/null
