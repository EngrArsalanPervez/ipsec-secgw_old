# ssh -f -N -L 3000:192.168.0.73:3000 network@172.25.90.231
# ssh -f -N -L 5000:192.168.0.73:5000 network@172.25.90.231
# sudo ip link set dev enp2s0f0 mtu 1400

1. UDP stats IKEv2
3. Key Update
4. Established Tunnel Status
4. Netstat Time and Ports

sudo systemctl stop strongswan
sudo pkill charon
sudo pkill charon-systemd
sudo netstat -ulnp | grep -E '500|4500'

sudo systemctl start strongswan
sudo systemctl status strongswan

systemctl disable strongswan
systemctl disable strongswan-starter


sudo ln -s /home/network/GUI/NodeServer/conf_files/IPE.cfg ./IPE.cfg
