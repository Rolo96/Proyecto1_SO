############################################################
# Technological Institute of Costa Rica                    #
# Project Operating Systems                                #
# Teacher: Diego Vargas                                    #
# Students: Raul Arias, Bryan Abarca, Rony Paniagua        #
# Remove fifo web server                                   #
############################################################

#Stop web server and remove it
update-rc.d -f WebServerFifo remove
/etc/init.d/WebServerFifo stop
rm /etc/init.d/WebServerFifo
