sudo systemctl stop WebServer
sudo systemctl disable WebServer
sudo rm /etc/systemd/system/WebServer.service
sudo systemctl daemon-reload
sudo systemctl reset-failed
