sudo ip link set dev vEth0_0 up
ip addr add 20.20.20.2/24 dev vEth0_0
arp -s 20.20.20.1 02:00:00:00:00:01
