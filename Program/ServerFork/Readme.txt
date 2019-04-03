############################################################
# Technological Institute of Costa Rica                    #
# Project Operating Systems                                #
# Teacher: Diego Vargas                                    #
# Students: Raul Arias, Bryan Abarca, Rony Paniagua        #
# Installation and use instructions of the fork web server #
############################################################

Requirements:

	-Work with Ubuntu (tested on a azure VM with Ubuntu 18.04)

Creation and execution of the docker:

	-To create the docker it is necessary to execute the command: sh InstallDockerFork.sh in the path where the delivered files are located, this is responsible for installing the docker in the 		machine, do the construction of the docker and execute it. (It is necessary to add nano and curl to the docker so the installation commands are added in the docker but when executing them there 		are some messages in red of some things that are not found but those messages do not interfere in the correct functioning of the docker).

Docker execution test:

	-When executing the previous command, the docker remains running, by default it is ready to run on port 8083, so to do tests it would only be necessary to go to a browser and make requests, 		currently there are several test files in the directory of the web server (/usr/src/WebServerFork/WebServerForkDir), so the possible requests would be:

		--http://server:8083/test.png
		--http://server:8083/test.pdf
		--http://server:8083/test.html
		--http://server:8083/test.bin
		--http://server:8083/test.mp4

Modify web server config and operations with the daemon:

	-To perform the requested operations (start, stop, restart, view status) of the web server, as well as modify the configuration file, the following commands must be executed:

	- To enter the running docker you must open another terminal and execute the command: sudo docker exec -it running_web_server_fork bash	

		--To perform actions on the daemon
		--Start daemon: /etc/init.d/WebServerFork start
		--Stop daemon: /etc/init.d/WebServerFork stop
		--Restart daemon: /etc/init.d/WebServerFork restart
		--View daemon status: /etc/init.d/WebServerFork status

		--To modify the configuration file
		--nano /etc/WebServerFork/config.conf

Uninstall docker

	-To uninstall the docker it is necessary to execute the command: sh RemoveDockerFork.sh in the path where the delivered files are located


