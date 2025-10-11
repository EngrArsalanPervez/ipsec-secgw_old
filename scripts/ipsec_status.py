import subprocess
import re
import os
import time
from datetime import datetime
from pymongo import MongoClient, errors


try:
    client = MongoClient("mongodb://localhost:27017/", serverSelectionTimeoutMS=2000)
    db = client["IPSec"]
    collection = db["policies"]
    client.admin.command("ping")
except errors.ConnectionFailure:
    print("❌ MongoDB connection failed — ensure MongoDB is running.")
    collection = None
except Exception as e:
    print(f"❌ Unexpected MongoDB error: {e}")
    collection = None


def log(msg, level="INFO"):
    """Helper function to print timestamped log messages."""
    print(f"[{datetime.now().strftime('%Y-%m-%d %H:%M:%S')}] [{level}] {msg}")


def is_ipsec_secgw_running():
    """Check if ipsec-secgw process is running."""
    try:
        result = subprocess.run(
            ["ps", "aux"], capture_output=True, text=True, check=True
        )
        for line in result.stdout.splitlines():
            if "ipsec-secgw" in line and "grep" not in line:
                return True
        return False
    except subprocess.CalledProcessError:
        log("Error running ps command.", "ERROR")
        return False
    except FileNotFoundError:
        log("Error: 'ps' command not found.", "ERROR")
        return False
    except Exception as e:
        log(f"Unexpected error checking ipsec-secgw: {e}", "ERROR")
        return False


def updateConn():
    """Mark all policies as Not Connected."""
    if collection is None:
        log("MongoDB not available — cannot update connectivity.", "ERROR")
        return
    try:
        log("Marking all connectivity fields as 'Not Connected'...")
        result = collection.update_many({}, {"$set": {"connectivity": "Not Connected"}})
        log(
            f"Updated {result.modified_count} document(s) — connectivity = 'Not Connected'"
        )
    except errors.PyMongoError as e:
        log(f"MongoDB update failed: {e}", "ERROR")
    except Exception as e:
        log(f"Unexpected MongoDB update error: {e}", "ERROR")


def parse_ipsec_status_and_update_mongo():
    """Parse IPsec tunnel status and update MongoDB."""
    try:
        result = subprocess.run(
            ["ipsec", "status"], capture_output=True, text=True, check=True
        )
        output = result.stdout.strip()

        os.system("clear")
        log("=============================================================")
        log("IPsec Tunnel Status", "INFO")
        log("=============================================================")

        if not output:
            log("No IPsec output found.")
            updateConn()
            return

        established_matches = re.findall(
            r"ESTABLISHED\s+([\d\w\s]+ago),\s+([\d\.]+)\[.*?\]\.\.\.([\d\.]+)\[.*?\]",
            output,
        )
        connecting_matches = re.findall(
            r"CONNECTING.*?([\d\.]+)\[.*?\]\.\.\.([\d\.]+)\[.*?\]", output
        )

        if not established_matches and not connecting_matches:
            log("No established or connecting tunnels found.")
            updateConn()
            return
        for established_time, src_ip, dst_ip in established_matches:
            status_string = f"ESTABLISHED {established_time.strip()}"
            log(f"Tunnel: {src_ip} → {dst_ip} | {status_string}")
            if collection is not None:
                try:
                    result = collection.update_many(
                        {"dstIPSAOUT": src_ip},
                        {"$set": {"connectivity": status_string}},
                    )
                    if result.modified_count > 0:
                        log(f"Updated {result.modified_count} document(s) for {src_ip}")
                    else:
                        log(f"No documents found for {src_ip}")
                except errors.PyMongoError as e:
                    log(f"MongoDB update error: {e}", "ERROR")

        for src_ip, dst_ip in connecting_matches:
            log(f"Tunnel: {src_ip} → {dst_ip} | CONNECTING")
            if collection is not None:
                try:
                    result = collection.update_many(
                        {"dstIPSAIN": src_ip}, {"$set": {"connectivity": "CONNECTING"}}
                    )
                    if result.modified_count > 0:
                        log(f"Updated {result.modified_count} document(s) for {src_ip}")
                    else:
                        log(f"No documents found for {src_ip}")
                except errors.PyMongoError as e:
                    log(f"MongoDB update error: {e}", "ERROR")

    except subprocess.CalledProcessError:
        log("Failed to get IPsec status — marking as Not Connected.", "ERROR")
        updateConn()
    except FileNotFoundError:
        log("Error: 'ipsec' command not found — marking as Not Connected.", "ERROR")
        updateConn()
    except Exception as e:
        log(f"Unexpected error while parsing IPsec status: {e}", "ERROR")
        updateConn()


def continuous_monitor(interval=5):
    """Continuously check if ipsec-secgw is running and monitor IPsec when active."""
    app_running = False
    log("Starting continuous IPsec monitor...")

    try:
        while True:
            try:
                running = is_ipsec_secgw_running()
                if running:
                    if not app_running:
                        log(
                            "ipsec-secgw detected — waiting 10 seconds before monitoring..."
                        )
                        time.sleep(10)
                        app_running = True
                    parse_ipsec_status_and_update_mongo()
                else:
                    if app_running:
                        log("ipsec-secgw stopped — marking Not Connected.")
                        updateConn()
                        app_running = False
                    else:
                        log("App is not running.")
                time.sleep(interval)

            except Exception as loop_error:
                log(f"Loop-level error: {loop_error}", "ERROR")
                updateConn()
                time.sleep(interval)

    except KeyboardInterrupt:
        log("Monitoring stopped by user (Ctrl+C).", "WARN")
    except Exception as e:
        log(f"Fatal error in monitor: {e}", "ERROR")
    finally:
        log("Exiting monitoring loop — cleaning up.")


if __name__ == "__main__":
    continuous_monitor(interval=5)
