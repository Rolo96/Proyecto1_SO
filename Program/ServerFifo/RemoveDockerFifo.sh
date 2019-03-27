#########################################################
# Instituto Tecnologico de Costa Rica                   #
# Proyecto Sistemas operativos                          #
# Profesor: Diego Vargas                                #
# Estudiantes: Raul Arias, Bryan Abarca, Rony Paniagua  #
# Remove docker web server fifo script                  #
#########################################################

#Stop container
sudo docker stop $(sudo docker ps -q --filter ancestor=web_server_fifo )
#Delete image
sudo docker rmi --force $(sudo docker images --format '{{.Repository}}:{{.Tag}}' | grep 'web_server_fifo')
