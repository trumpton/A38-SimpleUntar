# Introduction

SimpleUntar is a class object which can be used to unpack tar files onto a
LittleFS filesystem on an Arduino ESP32 or ESP8266 platform.

# Author

SimpleUntar is created by Steve Clarke
(c) Steve Clarke, Vizier Design 2023

# Source Code

SimpleUntar sourcecode is available at: https://www.github.com/trumpton/A38-SimpleUntar

# License

Released under the GNU AFFERO GENERAL PUBLIC LICENSE
See LICENSE file

# Example

The following code unpacks the test.tar file into the root folder

```
SimpleUntar engine ;
if (!engine.unpack("/test.tar", "/")) {
  Serial.printf("Untar Error: %s\n", engine.errorMessage()) ;
} else {
  Serial.printf("Untar, unpacked %d files\n", engine.numFiles()) ;
}
```
