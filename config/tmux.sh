#!/bin/bash

# Name of the tmux session
SESSION_NAME="eup"

# Check if the session already exists
tmux has-session -t $SESSION_NAME 2>/dev/null

# If the session doesn't exist, create it
if [ $? != 0 ]; then
    # Create the :0 window (tab)
    tmux new-session -d -s $SESSION_NAME -n "DPDK"
    tmux send-keys -t $SESSION_NAME:0 "cd /home/network/Desktop/bootstartup" Enter
    tmux send-keys -t $SESSION_NAME:0 "./dpdk.sh" Enter

    # Create the :1 window (tab)
    tmux new-window -t $SESSION_NAME:1 -n "ipsec-secgw"
    tmux send-keys -t $SESSION_NAME:1 "cd /home/network/Desktop/dpdk/examples/ipsec-secgw" Enter

    # Create the :2 window (tab)
    tmux new-window -t $SESSION_NAME:2 -n "htop"
    tmux send-keys -t $SESSION_NAME:2 "htop" Enter

    # Create the :3 window (tab)
    tmux new-window -t $SESSION_NAME:3 -n "bash"
    tmux send-keys -t $SESSION_NAME:3 "bash" Enter

    # --- Tab 4: Node ---
    tmux new-window -t $SESSION_NAME:4 -n "Node"
    tmux send-keys -t $SESSION_NAME:4 "cd /home/network/Desktop/bootstartup" Enter

    # --- Tab 5: IPSec ---
    tmux new-window -t $SESSION_NAME:5 -n "IPSec"
    tmux send-keys -t $SESSION_NAME:5 "cd /home/network/Desktop/dpdk/examples/ipsec-secgw/scripts" Enter
    tmux send-keys -t $SESSION_NAME:5 "python3 IPSec.py" Enter

    # --- Tab 6: vEth ---
    tmux new-window -t $SESSION_NAME:6 -n "vEth"
    tmux send-keys -t $SESSION_NAME:6 "cd /home/network/Desktop/dpdk/examples/ipsec-secgw/scripts" Enter
    tmux send-keys -t $SESSION_NAME:6 "python3 vETh.py" Enter
    # Select the 0 window
    tmux select-window -t $SESSION_NAME:0

    echo "New tmux session '$SESSION_NAME' created with 11 tabs running different scripts."
else
    echo "Session '$SESSION_NAME' already exists. Attaching to it..."
fi

# Attach to the session
tmux attach-session -t $SESSION_NAME