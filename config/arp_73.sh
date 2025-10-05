sudo ip link set dev vEth0_0 up
ip addr add 30.30.30.2/24 dev vEth0_0
arp -s 30.30.30.1 00:90:0b:d8:6e:aa
