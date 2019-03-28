############################################################
# Technological Institute of Costa Rica                    #
# Project Operating Systems                                #
# Teacher: Diego Vargas                                    #
# Students: Raul Arias, Bryan Abarca, Rony Paniagua        #
# Installation and use instructions of the fifo web server #
############################################################

Requirements:

	-Work with Ubuntu (tested on a azure VM with Ubuntu 18.04)

Creation and execution of the docker:

	-To create the docker it is necessary to execute the command: sh InstallDockerFifo.sh in the path where the delivered files are located, this is responsible for installing the docker in the 		machine, do the construction of the docker and execute it. (It is necessary to add nano and curl to the docker so the installation commands are added in the docker but when executing them there 		are some messages in red of some things that are not found but those messages do not interfere in the correct functioning of the docker).

Docker execution test:

	-When executing the previous command, the docker remains running, by default it is ready to run on port 8081, so to do tests it would only be necessary to go to a browser and make requests, 		currently there are several test files in the directory of the web server (/usr/src/WebServerFifo/WebServerFifoDir), so the possible requests would be:

		--http://server:8081/test.png
		--http://server:8081/test.pdf
		--http://server:8081/test.html
		--http://server:8081/test.bin
		--http://server:8081/test.mp4

Modify web server config and operations with the daemon:

	-To perform the requested operations (start, stop, restart, view status) of the web server, as well as modify the configuration file, the following commands must be executed:

	- To enter the running docker you must open another terminal and execute the command: sudo docker exec -it running_web_server_fifo bash	

		--To perform actions on the daemon
		--Start daemon: /etc/init.d/WebServerFifo start
		--Stop daemon: /etc/init.d/WebServerFifo stop
		--Restart daemon: /etc/init.d/WebServerFifo restart
		--View daemon status: /etc/init.d/WebServerFifo status

		--To modify the configuration file
		--nano /etc/WebServerFifo/config.conf

Uninstall docker

	-To uninstall the docker it is necessary to execute the command: sh RemoveDockerFifo.sh in the path where the delivered files are located


