############################################################
# Technological Institute of Costa Rica                    #
# Project Operating Systems                                #
# Teacher: Diego Vargas                                    #
# Students: Raul Arias, Bryan Abarca, Rony Paniagua        #
# Installation script of the fifo web server               #
############################################################

#Install docker and dependencies
sudo apt-get update
sudo apt-get install -y apt-transport-https ca-certificates
curl -fsSL https://download.docker.com/linux/ubuntu/gpg | sudo apt-key add -
sudo add-apt-repository "deb [arch=amd64] https://download.docker.com/linux/ubuntu bionic stable"
sudo apt update
sudo apt install -y docker-ce

#Build and run docker
sudo docker build -t web_server_fifo .
sudo docker run -p 8081:8081 -it --rm --name running_web_server_fifo web_server_fifo
