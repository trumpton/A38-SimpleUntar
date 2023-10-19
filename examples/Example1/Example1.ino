//
// SimpleUntar Example 1
//

#include <SimpleUntar.h>
#include <LittleFS.h>

void setup() {

  // Initialise Serial Interface
  delay(1000);
  Serial.begin(115200);
  Serial.println("\n\nBEGIN\n");
  
  // Initialise LittleFS
  LittleFS.begin() ;

  // Instantiate untar engine
  SimpleUntar engine ;

  // Unpack File
  if (!engine.unpack("/test.tar", "/")) {

    // Unpack Failed - report error message
    Serial.printf("Untar Error: %s\n", engine.errorMessage()) ;

  } else {

    // Unpack Success - report number of files extracted
    Serial.printf("Untar, unpacked %d files\n", engine.numFiles()) ;

  }

}

void loop() {
}

