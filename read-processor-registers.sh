#!/bin/sh
#
# Read I2C controller registers using devmem
#

# Base addresses em hex direto
get_base_addr() {
    case "$1" in
        0) echo 0x02000000 ;;
        1) echo 0x02010000 ;;
        2) echo 0x02020000 ;;
        3) echo 0x02030000 ;;
        4) echo 0x02040000 ;;
        5) echo 0x02050000 ;;
        6) echo 0x02060000 ;;
        7) echo 0x02070000 ;;
        *) echo "Invalid I2C index"; exit 1 ;;
    esac
}

read_i2c() {
    idx=$1
    base=$(get_base_addr "$idx")

    echo "I2C$idx (base: $base)"

    for reg in 0x00 0x01 0x02 0x03 0x04 0x05 0x06; do
        case "$reg" in
            0x00) name="IBAD" ;;
            0x01) name="IBFD" ;;
            0x02) name="IBCR" ;;
            0x03) name="IBSR" ;;
            0x04) name="IBDR" ;;
            0x05) name="IBIC" ;;
            0x06) name="IBDBG" ;;
        esac
        addr=$((base + reg))
        value=$(devmem $addr 8)
        printf " %-5s (0x%02X): 0x%02X\n" "$name" "$reg" "$value"
    done
    echo ""
}

[ $(which devmem) ] || {
    echo "devmem not found. Please install it."
    exit 1
}

if [ "$1" = "i2c" ]; then
    if [ "$2" = "all" ]; then
        for i in 0 1 2 3 4 5 6 7; do
            read_i2c "$i"
        done
    elif [ "$2" -ge 0 ] 2>/dev/null && [ "$2" -le 7 ]; then
        read_i2c "$2"
    else
        echo "Usage: $0 i2c [0-7|all]"
        exit 1
    fi
else
    echo "Usage: $0 i2c [0-7|all]"
    exit 1
fi
