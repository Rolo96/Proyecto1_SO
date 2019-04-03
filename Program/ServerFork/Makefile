############################################################
# Technological Institute of Costa Rica                    #
# Project Operating Systems                                #
# Teacher: Diego Vargas                                    #
# Students: Raul Arias, Bryan Abarca, Rony Paniagua        #
# Makefile of the fork web server                          #
############################################################

#Variables
DIR = /etc/init.d
DIR_CONF = /etc/WebServerFork
DIR_SERVER = ./WebServerForkDir/
SERVICE = WebServerFork
CONF_FILE = config.conf
SERVER = WebServerFork
SRC = main.c
BIN = main

#Copy config file and script daemon, run and install daemon
all: $(BIN)	
		mkdir -p $(DIR_CONF)
		cp $(CONF_FILE) $(DIR_CONF)
		cp $(SERVICE) $(DIR)
		update-rc.d WebServerFork defaults
		/etc/init.d/WebServerFork start

#Compile C web server
$(BIN): 
		gcc $(SRC) -o $(DIR_SERVER)WebServerFork


