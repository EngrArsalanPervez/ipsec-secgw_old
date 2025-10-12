import argparse
import subprocess
import time
import os
from datetime import datetime
from pymongo import MongoClient


MAC = ""


# --- MongoDB setup ---
client = MongoClient("mongodb://localhost:27017/")
db = client["IPSec"]
collection = db["policies"]
interface = "vEth0_0"

# --- ANSI colors ---
GREEN = "\033[92m"
RED = "\033[91m"
YELLOW = "\033[93m"
CYAN = "\033[96m"
RESET = "\033[0m"
BOLD = "\033[1m"


def log(level, message, color=RESET):
    """Formatted timestamped log output."""
    timestamp = datetime.now().strftime("%Y-%m-%d %H:%M:%S")
    print(f"{color}[{timestamp}] [{level}] {message}{RESET}")


def clear_screen():
    os.system("cls" if os.name == "nt" else "clear")


def run_command(cmd, show=True):
    """Run a system command and return output safely."""
    try:
        result = subprocess.run(cmd, capture_output=True, text=True, check=False)
        if show:
            if result.returncode == 0:
                if result.stdout.strip():
                    log("INFO", f"Command output:\n{result.stdout.strip()}", CYAN)
                else:
                    log("INFO", "Command executed successfully (no output)", GREEN)
            else:
                log("ERROR", result.stderr.strip(), RED)
        return result
    except Exception as e:
        log("ERROR", f"Command execution failed: {e}", RED)
        return None


def is_ipsec_secgw_running():
    """Check if ipsec-secgw process is running."""
    try:
        result = subprocess.run(["ps", "aux"], capture_output=True, text=True)
        if "ipsec-secgw" in result.stdout and "grep" not in result.stdout:
            return True
        return False
    except Exception as e:
        log("ERROR", f"Failed to check ipsec-secgw process: {e}", RED)
        return False


def interface_is_up(iface):
    """Check if interface is up."""
    try:
        result = subprocess.run(
            ["ip", "link", "show", iface], capture_output=True, text=True
        )
        return "state UP" in result.stdout
    except Exception as e:
        log("ERROR", f"Failed to check interface {iface}: {e}", RED)
        return False


def ensure_interface_up(iface):
    """Ensure the interface is up; bring it up silently if not."""
    try:
        if interface_is_up(iface):
            log("INFO", f"Interface {iface} is already up", GREEN)
        else:
            log("INFO", f"Interface {iface} is down — bringing it up...", YELLOW)
            subprocess.run(
                ["sudo", "ip", "link", "set", "dev", iface, "up"],
                capture_output=True,
                text=True,
            )
            log("INFO", f"Interface {iface} is now up", GREEN)
    except Exception as e:
        log("ERROR", f"Failed to bring up interface {iface}: {e}", RED)


def ip_exists(src_ip, iface):
    """Check if IP already exists on interface."""
    try:
        result = subprocess.run(
            ["ip", "addr", "show", "dev", iface], capture_output=True, text=True
        )
        return src_ip.split("/")[0] in result.stdout
    except Exception as e:
        log("ERROR", f"Failed to check IP {src_ip} on {iface}: {e}", RED)
        return False


def arp_exists(dst_ip):
    """Check if ARP entry exists."""
    try:
        result = subprocess.run(["ip", "neigh", "show"], capture_output=True, text=True)
        return dst_ip in result.stdout
    except Exception as e:
        log("ERROR", f"Failed to check ARP entry for {dst_ip}: {e}", RED)
        return False


def apply_network_settings():
    """Fetch IP/ARP entries from DB and apply."""
    try:
        fields_to_fetch = {"_id": 0, "dstIPSAOUT": 1, "srcIPSAOUT": 1}
        documents = collection.find({}, fields_to_fetch)

        for doc in documents:
            src_ip = doc.get("dstIPSAOUT")
            dst_ip = doc.get("srcIPSAOUT")

            if src_ip == "" or dst_ip == "":
                log("ERROR", f"Skipping invalid document: {doc}", RED)
                continue

            # --- Ensure /24 mask is appended if missing ---
            if "/" not in src_ip:
                src_ip += "/24"

            print(f"\n{BOLD}=========================================={RESET}")
            log(
                "INFO",
                f"Processing Document:\n  ➤ dstIPNWSPOUT: {src_ip}\n  ➤ dstIPSAOUT: {src_ip}",
                CYAN,
            )
            print(f"{BOLD}=========================================={RESET}")

            ensure_interface_up(interface)

            # --- Apply IP to interface ---
            if not ip_exists(src_ip, interface):
                run_command(
                    ["sudo", "ip", "addr", "add", src_ip, "dev", interface], show=False
                )
                log("INFO", f"Added IP {src_ip} to {interface}", GREEN)
            else:
                log("INFO", f"IP {src_ip} already exists on {interface}", YELLOW)

            # --- Add ARP entry for dst IP ---
            if not arp_exists(dst_ip):
                print(MAC)
                run_command(["sudo", "arp", "-s", dst_ip, MAC], show=False)
                log("INFO", f"Added ARP entry for {dst_ip}", GREEN)
            else:
                log("INFO", f"ARP entry for {dst_ip} already exists", YELLOW)

    except Exception as e:
        log("ERROR", f"Unexpected error in apply_network_settings(): {e}", RED)


def main():
    log("INFO", "Starting continuous IPsec monitor...", CYAN)
    parser = argparse.ArgumentParser(description="Continuous IPsec monitor")
    parser.add_argument(
        "--device",
        choices=["HCLOS", "LCLOS"],
        required=True,
        help="Specify the device type (HCLOS or LCLOS)"
    )
    args = parser.parse_args()
    ThisDevice = args.device

    if ThisDevice == "HCLOS":
        MAC = "02:00:00:00:00:02"
    elif ThisDevice == "LCLOS":
        MAC = "02:00:00:00:00:01"

    print(MAC)

    print("Starting continuous IPsec monitor...")
    print(f"Device Type: {ThisDevice}")
    print(f"MAC Address: {MAC}")

    try:
        while True:
            if is_ipsec_secgw_running():
                log(
                    "INFO",
                    "ipsec-secgw detected — waiting 10 seconds before monitoring...",
                    GREEN,
                )
                time.sleep(10)
                clear_screen()
                apply_network_settings()
            else:
                log(
                    "INFO",
                    "ipsec-secgw is not running — waiting before retry...",
                    YELLOW,
                )
            time.sleep(5)
    except KeyboardInterrupt:
        log("INFO", "Monitoring stopped by user.", RED)
    except Exception as e:
        log("ERROR", f"Unexpected error in main loop: {e}", RED)
    finally:
        client.close()
        log("INFO", "MongoDB connection closed.", CYAN)


if __name__ == "__main__":
    main()
