############################################################
# Technological Institute of Costa Rica                    #
# Project Operating Systems                                #
# Teacher: Diego Vargas                                    #
# Students: Raul Arias, Bryan Abarca, Rony Paniagua        #
# Remove fork web server                                   #
############################################################

#Stop web server and remove it
update-rc.d -f WebServerFork remove
/etc/init.d/WebServerFork stop
rm /etc/init.d/WebServerFork
