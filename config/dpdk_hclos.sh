cd /home/network/Desktop/dpdk
modprobe uio
insmod ./dpdk-kmods/linux/igb_uio/igb_uio.ko
insmod ./build/kernel/linux/kni/rte_kni.ko carrier=on
./usertools/dpdk-devbind.py --bind=igb_uio enp4s0f0
./usertools/dpdk-devbind.py --bind=igb_uio enp4s0f2
./usertools/dpdk-devbind.py --bind=igb_uio enp5s0f0
./usertools/dpdk-devbind.py --bind=igb_uio enp5s0f2
echo 1024 >/sys/devices/system/node/node0/hugepages/hugepages-2048kB/nr_hugepages
cd examples/ipsec-secgw/
./build/ipsec-secgw -c 0x3f -n 4 --socket-mem 1024,0 --vdev "crypto_openssl" -- -p 0x3f -P -u 0xE --config="(0,0,0),(1,0,1),(2,0,2),(3,0,3)" -d HCLOS -f ./config/HCLOS.cfg --transfer-mode poll -t 1
until ipsec-secgw; do
  echo "Server 'ipsec-secgw' crashed with exit code $?.  Respawning.." >&2
  sleep 2
  ./build/ipsec-secgw -c 0x3f -n 4 --socket-mem 1024,0 --vdev "crypto_openssl" -- -p 0x3f -P -u 0xE --config="(0,0,0),(1,0,1),(2,0,2),(3,0,3)" -d HCLOS -f ./config/HCLOS.cfg --transfer-mode poll -t 1
done
