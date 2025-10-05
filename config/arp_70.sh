sudo ip link set dev vEth0_0 up
ip addr add 10.10.10.1/24 dev vEth0_0
arp -s 10.10.10.2 00:90:0b:d8:6d:9a
