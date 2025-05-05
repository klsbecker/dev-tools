#!/bin/bash
#
# Scipt to copy a file to multiple IPs

TARGET_IPS="$1"
FILE="$2"
TARGET_PATH="$3"

if [ -z "$TARGET_IPS" ] || [ -z "$FILE" ] || [ -z "$TARGET_PATH" ]; then
    echo "Usage: $0 <target_ips> <file> <target_path>"
    echo " target_ips: Comma-separated list of IPs"
    echo " file: Path to the file to copy"
    echo " target_path: Path on the target machine"
    exit 1
fi

for ip in $(echo $TARGET_IPS | tr ',' ' '); do
    scp "$FILE" root@$ip:"$TARGET_PATH"
done
