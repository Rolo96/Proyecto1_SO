############################################################
# Technological Institute of Costa Rica                    #
# Project Operating Systems                                #
# Teacher: Diego Vargas                                    #
# Students: Raul Arias, Bryan Abarca, Rony Paniagua        #
# Makefile of the fifo web server                          #
############################################################

#Variables
DIR = /etc/init.d
DIR_CONF = /etc/WebServerFifo
DIR_SERVER = ./WebServerFifoDir/
SERVICE = WebServerFifo
CONF_FILE = config.conf
SERVER = WebServerFifo
SRC = main.c
BIN = main

#Copy config file and script daemon, run and install daemon
all: $(BIN)	
		mkdir -p $(DIR_CONF)
		cp $(CONF_FILE) $(DIR_CONF)
		cp $(SERVICE) $(DIR)
		update-rc.d WebServerFifo defaults
		/etc/init.d/WebServerFifo start

#Compile C web server
$(BIN): 
		gcc $(SRC) -o $(DIR_SERVER)WebServerFifo


