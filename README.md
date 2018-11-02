<a href="https://www.bigclown.com/"><img src="https://bigclown.sirv.com/logo.png" width="200" alt="BigClown Logo" align="right"></a>

# Firmware for BigClown Lora Climate Monitor

[![Travis](https://img.shields.io/travis/bigclownlabs/bcf-lora-climate-monitor/master.svg)](https://travis-ci.org/bigclownlabs/bcf-lora-climate-monitor)
[![Release](https://img.shields.io/github/release/bigclownlabs/bcf-lora-climate-monitor.svg)](https://github.com/bigclownlabs/bcf-lora-climate-monitor/releases)
[![License](https://img.shields.io/github/license/bigclownlabs/bcf-lora-climate-monitor.svg)](https://github.com/bigclownlabs/bcf-lora-climate-monitor/blob/master/LICENSE)
[![Twitter](https://img.shields.io/twitter/follow/BigClownLabs.svg?style=social&label=Follow)](https://twitter.com/BigClownLabs)

## Description

Unit measure temperature, relative humidity, illuminance and atmospheric pressure.
Values is sent every 15 minutes over LoRaWAN. Values are the arithmetic mean of the measured values since the last send.

Measure interval is 30s for temperature, relative humidity, illuminance, orientation. And 5minutes for atmospheric pressure.
The battery is measured during transmission.

## Buffer
big endian

| Byte    | Name        | Type   | multiple | unit
| ------: | ----------- | ------ | -------- | -------
|       0 | HEADER      | uint8  |          |
|       1 | BATTERY     | uint8  | 10       | V
|       2 | ORIENTATION | uint8  |          |
|  3 -  4 | TEMPERATURE | int16  | 10       | °C
|       5 | HUMIDITY    | uint8  | 2        | %
|  6 -  7 | ILLUMINANCE | uint16 |          | lux
|  8 -  9 | PRESSURE    | uint16 | 0.5      | Pa

### Header

* 0 - bool
* 1 - update
* 2 - button click
* 3 - button hold

## AT

```sh
picocom -b 115200 --omap crcrlf  --echo /dev/ttyUSB0
```

## License

This project is licensed under the [MIT License](https://opensource.org/licenses/MIT/) - see the [LICENSE](LICENSE) file for details.

---

Made with &#x2764;&nbsp; by [**HARDWARIO s.r.o.**](https://www.hardwario.com/) in the heart of Europe.
