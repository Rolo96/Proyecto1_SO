############################################################
# Technological Institute of Costa Rica                    #
# Project Operating Systems                                #
# Teacher: Diego Vargas                                    #
# Students: Raul Arias, Bryan Abarca, Rony Paniagua        #
# Makefile of the Thread web server                        #
############################################################

#Variables
DIR = /etc/init.d
DIR_CONF = /etc/WebServerThread
DIR_SERVER = ./WebServerThreadDir/
SERVICE = WebServerThread
CONF_FILE = config.conf
SERVER = WebServerThread
SRC = main.c
BIN = main

#Copy config file and script daemon, run and install daemon
all: $(BIN)	
		mkdir -p $(DIR_CONF)
		cp $(CONF_FILE) $(DIR_CONF)
		cp $(SERVICE) $(DIR)
		update-rc.d WebServerThread defaults
		/etc/init.d/WebServerThread start

#Compile C web server
$(BIN): 
		gcc $(SRC) -o $(DIR_SERVER)WebServerThread -lpthread


