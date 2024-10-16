#!/bin/bash
#
# ===============================================================
# Script Name: vscode-remote-connector.sh
#
# Author: Klaus Becker
#
# Description:
# This script connects to an already running Visual Studio Code 
# (VSCode) window on the host machine, using the remote Command 
# Line Interface (CLI). It is executed from the remote machine 
# (where this script runs, connected via SSH), and attempts to 
# connect to the most recent VSCode instance up to a maximum 
# number of retry attempts.
#
# Prerequisites:
# - Visual Studio Code must be installed and running on the **host** 
#   machine (the machine you are connecting to via SSH).
# - The VSCode Remote - SSH extension must be installed and 
#   connected to the remote machine where this script is executed.
# - This script should be run from the **remote** machine, where 
#   you are connected via SSH to the host.
#
# The script also supports a debug mode that provides additional 
# logging for troubleshooting.
#
# Usage: ./vscode-remote-connector.sh [--debug] [args]
#
# ===============================================================

# Maximum number of retry attempts
MAX_RETRY=10

# Flag to enable debug mode
DEBUG=false

log_debug() {
    if [[ "$DEBUG" == true ]]; then
        echo "DEBUG: $1"
    fi
}

# Function to log errors
log_error() {
    echo "ERROR: $1" >&2
}

# Check for debug flag in the arguments
for arg in "$@"; do
    if [[ "$arg" == "--debug" ]]; then
        DEBUG=true
        log_debug "Debug mode enabled."
        break
    fi
done

# Iterate through possible VSCode instances up to the maximum retry count
for i in $(seq 1 $MAX_RETRY); do
    log_debug "Attempt $i to connect to VSCode instance."

    # Fetch the VSCode remote command line script based on the most recent installation
    script=$(ls ~/.vscode-server/bin/*/bin/remote-cli/code -t 2>/dev/null | tail -1)

    if [[ -z ${script} ]]; then
        log_error "VSCode remote script not found. Ensure VSCode is installed and configured correctly."
        exit 1
    else
        log_debug "Using VSCode remote script: $script"
    fi

    # Find the most recent VSCode IPC socket file
    socket=$(ls /run/user/$UID/vscode-ipc-* -t 2>/dev/null | head -n$i | tail -1)

    if [[ -z ${socket} ]]; then
        log_error "VSCode IPC socket not found. Is VSCode running on this system?"
        exit 1
    else
        log_debug "Using VSCode IPC socket: $socket"
    fi

    # Export the IPC socket environment variable
    export VSCODE_IPC_HOOK_CLI=${socket}

    # Execute the VSCode remote script with the provided arguments
    if [[ "$DEBUG" == true ]]; then
        ${script} "$@" 
    else
        ${script} "$@" 2>/dev/null
    fi

    # Check if the script executed successfully
    if [ "$?" -eq 0 ]; then
        log_debug "Successfully connected to VSCode instance."
        exit 0
    fi
done

# If all attempts fail, log a final error message
log_error "Failed to connect to VSCode instance after $MAX_RETRY attempts."
exit 1
