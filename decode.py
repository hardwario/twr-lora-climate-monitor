#!/usr/bin/env python3
import sys
import __future__

HEADER_BOOT =  0x00
HEADER_UPDATE = 0x01

header_lut = {HEADER_BOOT: 'BOOT', HEADER_UPDATE: 'UPDATE'}


def decode(data):
    if len(data) != 20:
        raise Exception("Bad data length, 20 characters expected")

    header = int(data[0:2], 16)

    temperature = int(data[6:10], 16) if data[6:10] != 'ffff' else None

    if temperature:
        if temperature > 32768:
            temperature -= 65536
        temperature /= 10.0

    return {
        "header": header_lut[header],
        "voltage": int(data[2:4], 16) / 10.0 if data[2:4] != 'ff' else None,
        "orientation": int(data[4:6], 16),
        "temperature": temperature,
        "humidity": int(data[10:12], 16) / 2.0 if data[10:12] != 'ff' else None,
        "illuminance": int(data[12:16], 16) if data[12:16] != 'ffff' else None,
        "pressure": int(data[16:20], 16) * 2 if data[16:20] != 'ffff' else None,
    }


def pprint(data):
    print('Header :', data['header'])
    print('Voltage :', data['voltage'])
    print('Orientation :', data['orientation'])
    print('Temperature :', data['temperature'])
    print('Humidity :', data['humidity'])
    print('Illuminance :', data['illuminance'])
    print('Pressure :', data['pressure'])

if __name__ == '__main__':
    if len(sys.argv) != 2 or sys.argv[1] in ('help', '-h', '--help'):
        print("usage: python3 decode.py [data]")
        print("example: python3 decode.py 011b0100f5600024c313")
        exit(1)

    data = decode(sys.argv[1])
    pprint(data)
