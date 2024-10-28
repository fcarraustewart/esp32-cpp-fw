import asyncio
from bleak import BleakClient
import struct
import matplotlib.pyplot as plt
from matplotlib.animation import FuncAnimation
import numpy as np

# Define your ESP32 BLE peripheral information
#
#define BT_UUID_NUS_SRV_VAL \
#	BT_UUID_128_ENCODE(0x6e400001, 0xb5a3, 0xf393, 0xe0a9, 0xe50e24dcca9e)
#define BT_UUID_NUS_RX_CHAR_VAL \
#	BT_UUID_128_ENCODE(0x6e400002, 0xb5a3, 0xf393, 0xe0a9, 0xe50e24dcca9e)
#define BT_UUID_NUS_TX_CHAR_VAL \
#	BT_UUID_128_ENCODE(0x6e400003, 0xb5a3, 0xf393, 0xe0a9, 0xe50e24dcca9e)
#0x "6e400001-b5a3-f393-e0a9-e50e24dcca9e"
#
BLE_DEVICE_NAME =   "Zephyr"  # Replace with your device name or address
NUS_SERVICE_UUID =  "6e400001-b5a3-f393-e0a9-e50e24dcca9e"      # Replace with your NUS service UUID
NUS_CHAR_UUID =     "6e400002-b5a3-f393-e0a9-e50e24dcca9e"      # Replace with your NUS characteristic UUID

# Buffer size for data
BUFFER_SIZE = 100

# Arrays to store incoming IMU data for plotting
roll_data = np.zeros(BUFFER_SIZE)
pitch_data = np.zeros(BUFFER_SIZE)
yaw_data = np.zeros(BUFFER_SIZE)

# Function to handle incoming notifications from ESP32
def notification_handler(sender, data):
    global roll_data, pitch_data, yaw_data
    
    # Assuming the ESP32 sends the IMU data in a structured binary format (float32 for roll, pitch, yaw)
    roll, pitch, yaw = struct.unpack('fff', data)

    # Shift data and append new values
    roll_data = np.roll(roll_data, -1)
    roll_data[-1] = roll
    pitch_data = np.roll(pitch_data, -1)
    pitch_data[-1] = pitch
    yaw_data = np.roll(yaw_data, -1)
    yaw_data[-1] = yaw

    print(f"Roll: {roll:.2f}, Pitch: {pitch:.2f}, Yaw: {yaw:.2f}")

# Function to update the plot
def update_plot(frame):
    plt.cla()  # Clear the current axes
    plt.plot(roll_data, label="Roll")
    plt.plot(pitch_data, label="Pitch")
    plt.plot(yaw_data, label="Yaw")
    plt.ylim(-180, 180)  # Assuming roll, pitch, yaw are in degrees
    plt.title('IMU Data Streaming (Roll, Pitch, Yaw)')
    plt.xlabel('Time (samples)')
    plt.ylabel('Degrees')
    plt.legend(loc="upper left")
    plt.grid(True)

# Main function to connect to the ESP32 and start receiving data
async def main():
    # Scan for devices
    print("Scanning for devices...")

    # Connect to the ESP32 BLE device
    async with BleakClient(BLE_DEVICE_NAME, timeout = 20) as client:
        print(f"Connected to {BLE_DEVICE_NAME}")
        
        # Start receiving notifications from the NUS service characteristic
        await client.start_notify(NUS_CHAR_UUID, notification_handler)
        
        # Plot the IMU data in real-time
        fig = plt.figure()
        ani = FuncAnimation(fig, update_plot, interval=10)  # 100Hz = 10ms interval
        plt.show()

        # Run until user interrupts
        try:
            while True:
                await asyncio.sleep(0.1)
        except KeyboardInterrupt:
            print("Stopping notifications...")

        # Stop receiving notifications
        await client.stop_notify(NUS_CHAR_UUID)
        print(f"Disconnected from {BLE_DEVICE_NAME}")

# Entry point
if __name__ == "__main__":
    asyncio.run(main())
