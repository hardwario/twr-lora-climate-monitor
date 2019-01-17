/*

Decoder for The Things Network

*/


function Decoder(bytes, port) {
    // Decode an uplink message from a buffer
    var header = bytes[0];
    var voltage = bytes[1] / 10.0;
    var orientation = bytes[2];
    var temperature = ((bytes[3] << 8) | bytes[4]) / 10.0;
    var humidity = bytes[5] / 2;
    var illuminance = ((bytes[6] << 8) | bytes[7]);
    var pressure = ((bytes[8] << 8) | bytes[9]) * 2.0;

    // (array) of bytes to an object of fields.
    var decoded = {
      header: header,
      voltage: voltage,
      orientation: orientation,
      temperature: temperature,
      humidity: humidity,
      illuminance: illuminance,
      pressure: pressure

    };

    return decoded;
  }
