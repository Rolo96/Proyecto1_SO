#########################################################
# Instituto Tecnologico de Costa Rica                   #
# Proyecto Sistemas operativos                          #
# Profesor: Diego Vargas                                #
# Estudiantes: Raul Arias, Bryan Abarca, Rony Paniagua  #
# Install docker web server fifo script                 #
#########################################################

#Install docker and dependencies
sudo apt-get update
sudo apt-get install apt-transport-https ca-certificates
curl -fsSL https://download.docker.com/linux/ubuntu/gpg | sudo apt-key add -
sudo add-apt-repository "deb [arch=amd64] https://download.docker.com/linux/ubuntu bionic stable"
sudo apt update
sudo apt install docker-ce

#Build and run docker
sudo docker build -t web_server_fifo .
sudo docker run -p 8080:8080 -it --rm --name running_web_server_fifo web_server_fifo
