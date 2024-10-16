# Dev Tools

This repository contains a collection of utility scripts designed to assist with various development tasks. These scripts were initially created for my personal day-to-day workflow, but they are open for anyone to use, modify, and improve. Contributions are welcome, and I hope these tools help others streamline their development processes as well.

## Available Scripts

### [vscode-remote-connector.sh](./vscode-remote-connector.sh)

This script enables users to open a Visual Studio Code (VSCode) instance on their local machine from an SSH terminal connected to a remote server. By leveraging the Remote - SSH extension in VSCode, it allows seamless access to edit files on the remote machine using the familiar VSCode interface.

The script automatically searches for the necessary IPC socket and the remote CLI script to establish a valid connection to the most recent instance of VSCode. It includes a debug mode to provide additional logging for troubleshooting, making it easier to diagnose connection issues.

## Contributions

These scripts are a reflection of tools I’ve developed for my personal use, but they are by no means finished or exclusive to my workflow. If you find them helpful, feel free to adapt them to your own needs or contribute improvements. Pull requests are welcome!
