#!/bin/bash

# Name of the tmux session
SESSION_NAME="eup"

# Check if the session already exists
tmux has-session -t $SESSION_NAME 2>/dev/null

# If the session doesn't exist, create it
if [ $? != 0 ]; then
  # Create the :0 window (tab)
  tmux new-session -d -s $SESSION_NAME -n "DPDK"
  tmux send-keys -t $SESSION_NAME:0 "cd /home/network/Desktop/bootstartup/" Enter
  tmux send-keys -t $SESSION_NAME:0 "./dpdk.sh" Enter

  # Create the :1 window (tab)
  tmux new-window -t $SESSION_NAME:1 -n "ipsec-secgw"
  tmux send-keys -t $SESSION_NAME:1 "cd /home/network/Desktop/dpdk/examples/ipsec-secgw" Enter

  # Create the :2 window (tab)
  tmux new-window -t $SESSION_NAME:2 -n "config"
  tmux send-keys -t $SESSION_NAME:2 "cd /home/network/Desktop/dpdk/examples/ipsec-secgw/config" Enter

  # Create the :3 window (tab)
  tmux new-window -t $SESSION_NAME:3 -n "dpdk.sh"
  tmux send-keys -t $SESSION_NAME:3 "nvim /home/network/Desktop/bootstartup/dpdk.sh" Enter

  # Create the :4 window (tab)
  tmux new-window -t $SESSION_NAME:4 -n "IPE.cfg"
  tmux send-keys -t $SESSION_NAME:4 "cat /home/network/Desktop/dpdk/examples/ipsec-secgw/IPE.cfg" Enter

  # Create the :5 window (tab)
  tmux new-window -t $SESSION_NAME:5 -n "ipsec.conf"
  tmux send-keys -t $SESSION_NAME:5 "cat /etc/ipsec.conf" Enter

  # Create the :6 window (tab)
  tmux new-window -t $SESSION_NAME:6 -n "ipsec.secrets"
  tmux send-keys -t $SESSION_NAME:6 "cat /etc/ipsec.secrets" Enter

  # Create the :7 window (tab)
  tmux new-window -t $SESSION_NAME:7 -n "DB"
  tmux send-keys -t $SESSION_NAME:7 "mongosh" Enter

  # Create the :8 window (tab)
  tmux new-window -t $SESSION_NAME:8 -n "ipsec.py"
  tmux send-keys -t $SESSION_NAME:8 "cd /home/network/Desktop/bootstartup/scripts" Enter
  tmux send-keys -t $SESSION_NAME:8 "python3 ipsec_status.py" Enter

  # Create the :9 window (tab)
  tmux new-window -t $SESSION_NAME:9 -n "vEth"
  tmux send-keys -t $SESSION_NAME:9 "cd /home/network/Desktop/bootstartup/scripts" Enter
  tmux send-keys -t $SESSION_NAME:9 "python3 vEth0_0.py" Enter

  # Create the :2 window (tab)
  tmux new-window -t $SESSION_NAME:9 -n "htop"
  tmux send-keys -t $SESSION_NAME:9 "htop" Enter

  # Select the 0 window
  tmux select-window -t $SESSION_NAME:0

  echo "New tmux session '$SESSION_NAME' created with 11 tabs running different scripts."
else
  echo "Session '$SESSION_NAME' already exists. Attaching to it..."
fi

# Attach to the session
tmux attach-session -t $SESSION_NAME
