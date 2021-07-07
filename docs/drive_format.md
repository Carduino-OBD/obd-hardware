# Carduino drive format
All drives are stored in a custom binary format to save space. The header will always contain the version number in the first 2 bytes for compatibility purposes. All numbers are little endian.

## Version 1:
## Packet format
```
|------Header------|------Polyline------|------drive frames------|------Footer------|
      28 bytes 	  int8_t*Polyline Size  n frames * 14 bytes         1 byte
```

### Header:
```
|------Version------|------VIN------|------Fuel tank level------|------Polyline Size------|------Drive Frame Size------|
      uint16_t          int8_t * 17             uint8_t		      uint32_t			        uint32_t
```
- Version: 16 bit unsigned integer to allow clients to correctly parse the data
- VIN: A 17 character ascii string representing the vehicle VIN. **Not** null terminated
- Fuel tank level: 8 bit unsigned integer representing the vehicle fuel tank level at the start of the drive, a percentage between 0 and 100 (inclusive)
- Polyline Size: The length of the polyline string. Each element of the polyline string is int8_t
- Drive Frame Size: The size of the drive frame buffer. Each frame is divided into 14 bytes, so n_frames = drive frame size / 14


### Drive frame:
Each frame represents the vehicle state at a particular point in time.
```
|-----GPS Time-----|-----GPS Speed-----|-----Heading-----|-----Altitude-----|-----Vehicle Speed-----|
      uint64_t		    uint8_t              int16_t          int16_t              uint8_t
```
- GPS Time: a 64 bit unsigned integer representing the unix time of the data frame
- GPS Speed: an 8 bit unsigned integer representing the GPS speed in km/h
- GPS heading: a 16 bit signed integer representing the GPS heading
- GPS altitude: a 16 bit signed representing the GPS heading
- Vehicle Speed: an 8 bit unsigned integer representing the vehicle speed as reported by OBD in km/h


### Polyline data:
To save space, all longitude and latitude data is compressed with the polyline encoding format as a base64 string. Every drive frame will have a corresponding lat/long.
The polyline string is **not** null terminated and the size must be determined with the polyline size information.

## Footer:
```
|------Fuel tank level------|
           uint8_t
```
- Fuel tank level: 8 bit unsigned integer representing the vehicle fuel tank level at the end of the drive, a percentage between 0 and 100 (inclusive)
