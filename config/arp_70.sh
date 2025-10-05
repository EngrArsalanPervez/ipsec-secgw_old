sudo ip link set dev vEth0_0 up
ip addr add 10.10.10.1/24 dev vEth0_0
ip addr add 20.20.20.1/24 dev vEth0_0
ip addr add 30.30.30.1/24 dev vEth0_0
arp -s 10.10.10.2 00:90:0b:d8:6d:9a
arp -s 20.20.20.2 00:90:0b:d8:6e:12
arp -s 30.30.30.2 00:90:0b:c0:f7:24
