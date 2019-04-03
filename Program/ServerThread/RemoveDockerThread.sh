############################################################
# Technological Institute of Costa Rica                    #
# Project Operating Systems                                #
# Teacher: Diego Vargas                                    #
# Students: Raul Arias, Bryan Abarca, Rony Paniagua        #
# Remove script of the Thread web server                   #
############################################################

#Stop container
sudo docker stop $(sudo docker ps -q --filter ancestor=web_server_thread )
#Delete image
sudo docker rmi --force $(sudo docker images --format '{{.Repository}}:{{.Tag}}' | grep 'web_server_thread')
