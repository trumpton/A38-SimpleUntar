//
// SimpleUntar.h
//
// (c) Steve Clarke, Vizier Design 2023
//
// This class provides an untar function to unpack a tar file
// into a given folder on a LittleFS filesystem.
//
// CHANGELOG
// =========
//
// 20231017     0.1   Initial Version
//
//
// PRE-REQUISITES
// ==============
//
// Arduino ESP8266 or ESP32
// LittleFS
//
//
// EXAMPLE
// =======
//
//  SimpleUntar engine ;
//  if (!engine.unpack("/test.tar", "/")) {
//    Serial.printf("Untar Error: %s\n", engine.errorMessage()) ;
//  } else {
//    Serial.printf("Untar, unpacked %d files\n", engine.numFiles()) ;
//  }
//
//
// LICENSE
// =======
//
// Released under the GNU AFFERO GENERAL PUBLIC LICENSE
// See LICENSE file
//

#ifndef SIMPLEUNTAR_DEFINED
#define SIMPLEUNTAR_DEFINED

#include <Arduino.h>

class SimpleUntar {

private:
  int numfiles ;
  String fnParse(String path1, String path2) ;

public:
    enum error {
        ErrorOk = 0,
        ErrorOutputFolderCreation,
        ErrorInputFileNotPresent,
        ErrorUnexpectedEndOfFile,
        ErrorUnexpectedRecordInFile,
        ErrorWritingToDisk
    } unpackErrorCode ;

public:
    SimpleUntar() ;
    int unpack(String tarfile, String destination) ;
    const char * errorMessage() ;
    error errorCode() ;
    int numFiles() ;

} ;

#endif