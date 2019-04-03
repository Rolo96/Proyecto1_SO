############################################################
# Technological Institute of Costa Rica                    #
# Project Operating Systems                                #
# Teacher: Diego Vargas                                    #
# Students: Raul Arias, Bryan Abarca, Rony Paniagua        #
# Remove Thread web server                                 #
############################################################

#Stop web server and remove it
update-rc.d -f WebServerThread remove
/etc/init.d/WebServerThread stop
rm /etc/init.d/WebServerThread
