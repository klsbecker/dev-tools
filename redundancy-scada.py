#!/usr/bin/env python

"""
This script implements a SCADA (Supervisory Control and Data Acquisition) system
using Flask for the web interface and Modbus TCP for communication with PLCs (Programmable Logic Controllers).

Key Features:
- Flask Web Server: Provides a web interface for monitoring and configuring the system.
- Modbus TCP Client: Communicates with multiple PLCs to read and update register values.
- Redundancy Support: Handles multiple PLC IPs for redundancy and failover.
- Data Registers: Monitors specific Modbus registers for system status and synchronization metrics.
- PLC Identification: Maps PLC IDs to human-readable names for better visualization.

Components:
1. `MODBUS_IPS`: List of PLC IPs for redundancy.
2. `MODBUS_PORT`: Port used for Modbus TCP communication.
3. `REGISTERS`: Dictionary mapping register names to their Modbus addresses.
4. `PLC_ID`: Dictionary mapping PLC IDs to descriptive names.

The script is designed to run continuously, polling the PLCs for data and updating the web interface in real-time.

The web interface displays the current values of the monitored registers and allows users to reset counters via a button click.

Author: Klaus Becker
"""

from flask import Flask, jsonify, render_template_string, request, make_response
from pymodbus.client.sync import ModbusTcpClient
import threading
import time

app = Flask(__name__)

MODBUS_IPS = ["192.168.0.1"]
MODBUS_PORT = 504

REGISTERS = {
    "PLC ID": 0,
    "RedundancyState": 1,
    "SuccessExchDgCmdSync": 2,
    "FailedExchDgCmdSync": 3,
    "SuccessRedDataSync": 4,
    "FailedRedDataSync": 5,
    "SuccessRedForceSync": 6,
    "FailedRedForceSync": 7,
}

PLC_ID = {
    1: "NONE",
    2: "PLC A",
    3: "PLC B",
}

REDUNDANCY_STATE = {
    0: "NOT CONFIGURED",
    1: "INITIALIZING",
    2: "STARTING",
    3: "STANDBY",
    4: "ACTIVE",
    5: "INACTIVE",
}

data = {}

def get_cookie_ips():
    cookie = request.cookies.get("selected_ips")
    if cookie:
        return [ip.strip() for ip in cookie.split(",") if ip]
    return MODBUS_IPS


def update_loop():
    while True:
        for ip in MODBUS_IPS:
            if ip not in data:
                data[ip] = {name: 0 for name in REGISTERS}
            client = ModbusTcpClient(ip, port=MODBUS_PORT)
            if client.connect():
                rr = client.read_holding_registers(0, len(REGISTERS))
                if not rr.isError():
                    for name, addr in REGISTERS.items():
                        data[ip][name] = rr.registers[addr]
            client.close()
        time.sleep(0.5)

@app.route("/")
def index():
    global MODBUS_IPS
    cookie_ips = get_cookie_ips()
    MODBUS_IPS = cookie_ips
    for ip in cookie_ips:
        if ip not in data:
            data[ip] = {name: 0 for name in REGISTERS}
    html = """
    <!DOCTYPE html>
    <html>
    <head>
        <title>Redundancy Sync SCADA</title>
        <style>
            body { font-family: sans-serif; padding: 30px; }
            table { border-collapse: collapse; table-layout: auto; width: auto; }
            th, td { padding: 10px; border: 1px solid #ccc; text-align: left; }
            button { padding: 8px 16px; border: none; background-color: #007BFF; color: white; border-radius: 6px; cursor: pointer; }
            input { padding: 8px 12px; border: 1px solid #ccc; border-radius: 6px; width: 320px; }
        </style>
    </head>
    <body>
        <h2>Redundancy Sync SCADA</h2>
        <table id="reg-table">
            <tr><th>Parameter</th>{% for ip in modbus_ips %}<th>{{ ip }}</th>{% endfor %}</tr>
            {% for key in registers %}
            <tr><td>{{ key }}</td>
                {% for ip in modbus_ips %}
                <td id="cell-{{ ip }}-{{ key }}">{{ data[ip][key] }}</td>
                {% endfor %}
            </tr>
            {% endfor %}
        </table>
        <button onclick="resetRegisters()">Reset Counters</button>
        <form onsubmit="setIPs(); return false;" style="margin-top: 20px; display: flex; align-items: center; gap: 10px;">
            <label for="ipInput" style="font-weight: bold;">IPs:</label>
            <input type="text" id="ipInput" placeholder="192.168.20.101,192.168.18.65">
            <button type="submit">Update</button>
        </form>
        <script>
            function setIPs() {
                const ips = document.getElementById("ipInput").value.replace(/\s/g, "");
                document.cookie = `selected_ips=${ips};path=/`;
                location.reload();
            }

            function updateTable() {
                fetch('/values')
                    .then(res => res.json())
                    .then(data => {
                        for (let ip in data) {
                            for (let key in data[ip]) {
                                const cell = document.getElementById(`cell-${ip}-${key}`);
                                if (cell) {
                                    cell.innerText = data[ip][key];
                                }
                            }
                        }
                    });
            }

            function resetRegisters() {
                fetch('/reset', { method: 'POST' });
            }

            setInterval(updateTable, 500);  // Update table every 500ms
        </script>
    </body>
    </html>
    """
    resp = make_response(render_template_string(html, modbus_ips=cookie_ips, registers=REGISTERS.keys(), data=data))
    return resp

@app.route("/values")
def get_values():
    formatted_data = {}
    for ip in data:
        formatted_data[ip] = {}
        for key, value in data[ip].items():
            if key == "PLC ID":
                formatted_data[ip][key] = PLC_ID.get(value, f"ID {value}")
            elif key == "RedundancyState":
                formatted_data[ip][key] = REDUNDANCY_STATE.get(value, f"State {value}")
            else:
                formatted_data[ip][key] = value
    return jsonify(formatted_data)


@app.route("/reset", methods=["POST"])
def reset():
    for ip in user_ips():
        client = ModbusTcpClient(ip, port=MODBUS_PORT)
        if client.connect():
            client.write_coil(2, True)
            client.close()
    return '', 204

if __name__ == "__main__":
    threading.Thread(target=update_loop, daemon=True).start()
    app.run(host="0.0.0.0", port=5001)
