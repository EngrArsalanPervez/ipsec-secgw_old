cd ipsec-secgw
git pull
make
rm IPE.cfg; ln -s /home/network/GUI/NodeServer/conf_files/IPE.cfg ./IPE.cfg
cp -r scripts /home/network/Desktop/bootstartup/
cp -r config/dpdk_hclos.sh /home/network/Desktop/bootstartup/dpdk.sh
cp -r config/tmux.sh /home/network/Desktop/bootstartup/tmux.sh
nano /home/network/Desktop/bootstartup/dpdk.sh # check Interfaces, HCLOS/LCLOS
nano /home/network/Desktop/bootstartup/tmux.sh # python3 vEth0_0.py --device HCLOS
pkill ipsec-secgw

cd /home/network/Desktop/bootstartup/
tmux kill-session -t eup
./tmux.sh






# Delete all local changes
git reset --hard && git clean -fd

# Delete everything local. Sync with remote
git fetch origin
git reset --hard origin/$(git rev-parse --abbrev-ref HEAD)
git clean -fd
git clean -fdx