//
// SimpleUntar.cpp
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

// Enable Debugging
//#define SIMPLEUNTAR_DEBUG 1

#include "SimpleUntar.h"
#include <Arduino.h>
#include <LittleFS.h>

#ifdef SIMPLEUNTAR_DEBUG
#define DEBUG(fmt, args...) Serial.printf(fmt, ##args)
#else
#define DEBUG(fmt, args...)
#endif

struct GnuTarHeader {
  char name[100];
  char mode[8];
  char uid[8];
  char gid[8];
  unsigned char size[12];
  char mtime[12];
  char chksum[8];
  char typeflag;
  char linkname[100];
  char magic[6];
  char version[2];
  char uname[32];
  char gname[32];
  char devmajor[8];
  char devminor[8];
  char prefix[155];
  char pad[12];
};


// CONSTRUCTOR
SimpleUntar::SimpleUntar() {
  unpackErrorCode = ErrorOk;
  numfiles = 0 ;
}

// UNPACK
int SimpleUntar::unpack(String tarfile, String destination) {

  numfiles = 0;

  DEBUG("Tarfile Checking: %s\n", tarfile.c_str());
  if (!LittleFS.exists(tarfile)) {
    unpackErrorCode = ErrorInputFileNotPresent;
    return unpackErrorCode;
  }

  DEBUG("Tarfile Creating: %s\n", destination.c_str());
  LittleFS.mkdir(destination);
  if (!LittleFS.exists(destination)) {
    unpackErrorCode = ErrorOutputFolderCreation;
    return unpackErrorCode;
  }

  struct GnuTarHeader rh;
  File th;
  bool eof = false;
  bool firstrecord = true;
  String filename = "";

  DEBUG("Tarfile Opening %s\n", tarfile.c_str());
  th = LittleFS.open(tarfile, "r");

  do {

    char *b = (char *)&rh;
    for (int i=0; i < 512; i++) b[i] = '\0';

    int l = th.readBytes((char *)&rh, sizeof(rh));

    if (l < 0) {      
      DEBUG("Tarfile End of File or Read Error\n") ;
      unpackErrorCode = ErrorUnexpectedEndOfFile;
      eof = true;
    }

    if (!eof) {

      DEBUG("Tarfile Reading Record: %d - %d\n", l, (int)rh.typeflag);

      if (strncmp(rh.magic, "ustar", 5) != 0) {

        DEBUG("Tarfile Magic Number not found\n");

        if (firstrecord) {
          unpackErrorCode = ErrorUnexpectedRecordInFile;
        } else {
          unpackErrorCode = ErrorOk;
        }
        eof = true;

      } else {

        DEBUG("Processing request\n") ;

        switch (rh.typeflag) {

          // Capture long filename / path
          case 'L':
            {

              uint8_t buf[512 + 1];
              long int size = strtol((char *)rh.size, NULL, 8);
              filename = "";

              while (size > 0) {
                int len = th.readBytes((char*)buf, sizeof(buf) - 1);
                buf[len] = '\0';  // Append null character
                filename = filename + String((char*)buf);
                size = size - 512;
              }
            }
            break;

          // Create directory
          case '5':
            {
              // Use long filename in preference to rh.name if supplied
              if (filename.length() == 0) {
                rh.mode[0] = '\0';
                filename = rh.name;
              }
              DEBUG("Tarfile Mkdir: %s\n", fnParse(destination, filename).c_str());
              LittleFS.mkdir(fnParse(destination, filename).c_str());
              filename = "";
            }
            break;

          // Save file
          case '0':
            {

              uint8_t buf[512];
              long int size = strtol((char *)rh.size, NULL, 8);

              // Use long filename in preference to rh.name if supplied
              if (filename.length() == 0) {
                rh.mode[0] = '\0';
                filename = rh.name;
              }

              DEBUG("Tarfile Unpacking: %s\n", fnParse(destination, filename).c_str());

              File of = LittleFS.open(fnParse(destination, filename).c_str(), "w");
              while (!eof && size > 0) {
                int len = th.readBytes((char *)buf, sizeof(buf));
                int writesize = (size < 512 ? size : 512);
                if (of.write(buf, writesize) != writesize) {
                  unpackErrorCode = ErrorWritingToDisk;
                  eof = true;
                }
                size = size - 512;
              }
              of.close();

              numfiles++;

              filename = "";
            }
            break;

          // Skip unrecognised records
          default:
            {
              uint8_t buf[512];
              long int size = atoi((char *)rh.size);
              while (size > 0) {
                th.readBytes((char *)buf, sizeof(buf));
                size = size - 512;
              }
              filename = "";
            }
            break;
        }

        firstrecord = false;
      }
    }

  } while (!eof);

  th.close();

  if (unpackErrorCode != ErrorOk) {
    numfiles=0 ;
  }

  return unpackErrorCode ;
}

String SimpleUntar::fnParse(String path1, String path2) {
  if (path1.endsWith("/")) return String(path1 + path2);
  else return String(path1 + "/" + path2);
}

int SimpleUntar::numFiles() {
  return numfiles ;
}

SimpleUntar::error SimpleUntar::errorCode() {
  return unpackErrorCode ;
}

const char * SimpleUntar::errorMessage() {
  String msg ;
  switch (unpackErrorCode) {
    case ErrorOk:
      msg = F("OK") ;
      break ;
    case ErrorOutputFolderCreation:
      msg = F("Unable to create output destination folder") ;
      break ;
    case ErrorInputFileNotPresent:
      msg = F("Unable to access input tarfile") ;
      break ;
    case ErrorUnexpectedEndOfFile:
      msg = F("Unexpected End of File") ;
      break ;
    case ErrorUnexpectedRecordInFile:
      msg = F("Unexpected record in file - is this a tar file?") ;
      break ;
    case ErrorWritingToDisk:
      msg = F("Error saving to disk") ;
      break ;
    default:
      msg = F("Unknown error") ;
      break ;
  }
  return msg.c_str();
}
