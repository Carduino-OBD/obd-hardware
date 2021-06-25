# Carduino drive format
All drives are stored in a custom binary format to save space. The header will always contain the version number in the first 2 bytes for compatibility purposes. All numbers are little endian.

## Version 1:
## Packet format
```
|------Header------|------n drive frames------|------final drive frames (all bits set to 1)------|------Footer------|
      20 bytes          n frames * 67 bytes                         67 bytes  							1 byte
```

### Header:
```
|------Version------|------VIN------|------Fuel tank level------|
      uint16_t          int8_t * 17             uint8_t
```
- Version: 16 bit unsigned integer to allow clients to correctly parse the data
- VIN: A 17 character ascii string representing the vehicle VIN
- Fuel tank level: 8 bit unsigned integer representing the vehicle fuel tank level at the start of the drive, a percentage between 0 and 100 (inclusive)

### Drive frame:
Each frame represents the vehicle state at a particular point in time. The final drive frame will have all bits set to 1 and is considered a stop frame and is therefore invalid.
```
|-----GPS Time-----|-----GPS Speed-----|-----Latitutde-----|-----Longitude-----|-----Heading-----|-----Altitude-----|-----AccelX-----|-----AccelY-----|-----AccelZ-----|-----Vehicle Speed-----|
      uint64_t		  float (4 byte)      float (4 byte)       float (4 byte)       uint16_t       float (4 byte)    float (4 byte)    float (4 byte)   float (4 byte)        uint8_t
```
- GPS Time: a 64 bit unsigned integer representing the unix time of the data frame
- GPS Speed: a 64 bit float representing the GPS speed in km/h
- GPS latitude: a 64 bit float representing the GPS latitude
- GPS longtitude: a 64 bit float representing the GPS longitude
- GPS heading: a 16 bit unsigned integer representing the GPS heading
- GPS altitude: a 64 bit float representing the GPS heading
- AccelX: a 64 bit float representing the acceleration of the device in the x axis
- AccelY: a 64 bit float representing the acceleration of the device in the y axis
- AccelZ: a 64 bit float representing the acceleration of the device in the z axis
- Vehicle Speed: an 8 bit unsigned integer representing the vehicle speed as reported by OBD


## Footer:
```
|------Fuel tank level------|
           uint8_t
```
- Fuel tank level: 8 bit unsigned integer representing the vehicle fuel tank level at the end of the drive, a percentage between 0 and 100 (inclusive)
