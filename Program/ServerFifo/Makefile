DIR = /etc/systemd/system
DIR_CONF = /etc/webserver
DIR_SERVER = ./WebServerDir/
SERVICE = WebServer.service
CONF_FILE = config.conf
SERVER = WebServer
SRC = main.c
BIN = main
TOPDIR=$(shell pwd)

all: $(BIN)	
		sed -i 's:path:'$(TOPDIR)':' WebServer.service
		sudo  mkdir -p $(DIR_CONF)
		sudo cp $(CONF_FILE) $(DIR_CONF)
		sudo cp $(SERVICE) $(DIR)
		sudo systemctl daemon-reload
		sudo systemctl enable $(SERVER)
		sudo systemctl start $(SERVER)
		sudo chmod -R -f 775 /var/log
$(BIN): 
		gcc $(SRC) -o $(DIR_SERVER)WebServer


