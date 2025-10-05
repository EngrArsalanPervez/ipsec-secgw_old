sudo ip link set dev vEth0_0 up
ip addr add 10.10.10.1/24 dev vEth0_0
ip addr add 20.20.20.1/24 dev vEth0_0
ip addr add 30.30.30.1/24 dev vEth0_0
arp -s 10.10.10.2 a1:b1:c1:d1:e1:f1
arp -s 20.20.20.2 a1:b1:c1:d1:e1:f1
arp -s 30.30.30.2 a1:b1:c1:d1:e1:f1
