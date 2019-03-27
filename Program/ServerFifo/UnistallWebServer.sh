#########################################################
# Instituto Tecnologico de Costa Rica                   #
# Proyecto Sistemas operativos                          #
# Profesor: Diego Vargas                                #
# Estudiantes: Raul Arias, Bryan Abarca, Rony Paniagua  #
# Unistall web server fifo script                       #
#########################################################

#Stop web server and remove it
update-rc.d -f WebServerFifo remove
/etc/init.d/WebServerFifo stop
rm /etc/init.d/WebServerFifo
