"""
UART Service
-------------

An example showing how to write a simple program using the Nordic Semiconductor
(nRF) UART service.

"""

import asyncio
import sys
import numpy as np
from itertools import count, takewhile
from typing import Iterator

from bleak import BleakClient, BleakScanner
from bleak.backends.characteristic import BleakGATTCharacteristic
from bleak.backends.device import BLEDevice
from bleak.backends.scanner import AdvertisementData

UART_SERVICE_UUID = "6E400001-B5A3-F393-E0A9-E50E24DCCA9E"
UART_RX_CHAR_UUID = "6E400002-B5A3-F393-E0A9-E50E24DCCA9E"
UART_TX_CHAR_UUID = "6E400003-B5A3-F393-E0A9-E50E24DCCA9E"


# TIP: you can get this function and more from the ``more-itertools`` package.
def sliced(data: bytes, n: int) -> Iterator[bytes]:
    """
    Slices *data* into chunks of size *n*. The last slice may be smaller than
    *n*.
    """
    return takewhile(len, (data[i : i + n] for i in count(0, n)))


#define SCALE_Q(n) (1.0f / (1 << n))

#const float scaleRadToDeg = 180.0 / 3.14159265358;
conversion = (180.0 / 3.14159265358)*(1.0 / (1<<14))
async def uart_terminal():
    """This is a simple "terminal" program that uses the Nordic Semiconductor
    (nRF) UART service. It reads from stdin and sends each line of data to the
    remote device. Any data received from the device is printed to stdout.
    """

    def match_nus_uuid(device: BLEDevice, adv: AdvertisementData):
        # This assumes that the device includes the UART service UUID in the
        # advertising data. This test may need to be adjusted depending on the
        # actual advertising data supplied by the device.
        #print(adv.service_uuids)
        print("\t\t Found Device: ", adv.local_name)
        if( adv.local_name == "Zephyr" ):
            print("\t\t\t Got and advertising ESP32C3 Zephyr device.")
            print("\t\t\t\t uuid Data: ", adv.service_uuids)
            return True
        if UART_SERVICE_UUID.lower() in adv.service_uuids:
            return True

        return False

    device = await BleakScanner.find_device_by_filter(match_nus_uuid)

    if device is None:
        print("no matching device found, you may need to edit match_nus_uuid().")
        sys.exit(1)

    def handle_disconnect(_: BleakClient):
        print("Device was disconnected, goodbye.")
        # cancelling all tasks effectively ends the program
        for task in asyncio.all_tasks():
            task.cancel()

    def handle_rx(_: BleakGATTCharacteristic, data: bytearray):
        #print("Received:", data)
        message = bytes(12)
        roll = 0
        pitch = 0
        yaw = 0
        message = data
        print("Message[0]= \t",message[0],"\t","\tMessage[1]= \t",message[1],"\t")
        print("Message[2]= \t",message[2],"\t","\tMessage[3]= \t",message[3])
        print("Message[4]= \t",message[4],"\t","\tMessage[5]= \t",message[5])
        # Check if message!=bytes("main", 'utf-8') or message!=bytes("coop", 'utf-8'):
            
        if len(message)>5:
            sign_roll = -1 if(message[0]&0x80) else 1
            sign_pitch = -1 if(message[2]&0x80) else 1
            sign_yaw = -1 if(message[4]&0x80) else 1
            roll = conversion * sign_roll * round(float(int( message[1]&0x00ff)+ int(((message[0]&0x7f )<<8))) ,ndigits=2)
            pitch = conversion * sign_pitch * round(float(int( message[3]&0x00ff)+ int(((message[2]&0x7f )<<8))) ,ndigits=2)
            yaw = conversion * sign_yaw * round(float(int( message[5]&0x00ff)+ int(((message[4]&0x7f )<<8))) ,ndigits=2)
            print("\t ","\t", np.array([roll,pitch,yaw]))
                
        #print("\troll,pitch,yaw =",roll,pitch,yaw)
    async with BleakClient(device, disconnected_callback=handle_disconnect) as client:
        await client.start_notify(UART_TX_CHAR_UUID, handle_rx)

        print("Connected, start typing and press ENTER...")

        loop = asyncio.get_running_loop()
        nus = client.services.get_service(UART_SERVICE_UUID)
        rx_char = nus.get_characteristic(UART_RX_CHAR_UUID)

        while True:
            # This waits until you type a line and press ENTER.
            # A real terminal program might put stdin in raw mode so that things
            # like CTRL+C get passed to the remote device.
            data = await loop.run_in_executor(None, sys.stdin.buffer.readline)

            # data will be empty on EOF (e.g. CTRL+D on *nix)
            if not data:
                break

            # some devices, like devices running MicroPython, expect Windows
            # line endings (uncomment line below if needed)
            # data = data.replace(b"\n", b"\r\n")

            # Writing without response requires that the data can fit in a
            # single BLE packet. We can use the max_write_without_response_size
            # property to split the data into chunks that will fit.

            for s in sliced(data, rx_char.max_write_without_response_size):
                await client.write_gatt_char(rx_char, s, response=False)

            print("sent:", data)


if __name__ == "__main__":
    try:
        asyncio.run(uart_terminal())
    except asyncio.CancelledError:
        # task is cancelled on disconnect, so we ignore this error
        pass