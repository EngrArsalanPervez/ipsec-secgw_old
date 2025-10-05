sudo ip link set dev vEth0_0 up
ip addr add 10.10.10.2/24 dev vEth0_0
arp -s 10.10.10.1 02:00:00:00:00:01
