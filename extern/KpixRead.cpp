//-----------------------------------------------------------------------------
// File          : DataRead.cpp
// Author        : Ryan Herbst  <rherbst@slac.stanford.edu>
// Created       : 04/12/2011
// Project       : General Purpose
//-----------------------------------------------------------------------------
// Description :
// Read data & configuration from disk
//-----------------------------------------------------------------------------
// This file is part of 'SLAC Generic DAQ Software'.
// It is subject to the license terms in the LICENSE.txt file found in the 
// top-level directory of this distribution and at: 
//    https://confluence.slac.stanford.edu/display/ppareg/LICENSE.html. 
// No part of 'SLAC Generic DAQ Software', including this file, 
// may be copied, modified, propagated, or distributed except according to 
// the terms contained in the LICENSE.txt file.
// Proprietary and confidential to SLAC.
//-----------------------------------------------------------------------------
// Modification history :
// 04/12/2011: created
//-----------------------------------------------------------------------------
// Modified by Mengqing Wu <mengqing.wu@desy.de>
// Modified : 21/08/2018
// Project  : Lycoris
// Target   : Make it lighter, and use C/C++ std lib instead of linux file descriptor for reading bin file.
//
//-----------------------------------------------------------------------------


#include "KpixRead.h"

#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <iostream>
#include <iomanip>
#include <stdint.h>
using namespace std;

#ifdef RTEMS
#define O_LARGEFILE 0
#endif

// Constructor
KpixRead::KpixRead ( ) {
   fd_          = -1;
   size_        = 0;
   sawRunStart_ = false;
   sawRunStop_  = false;
   sawRunTime_  = false;
   rdAddr_      = 0;
   rdCount_     = 0;
   smem_        = NULL;
}

// Deconstructor
KpixRead::~KpixRead ( ) { }

// Process xml
void KpixRead::xmlParse ( uint32_t size, char *data ) {
   char         *buff;
   uint32_t      mySize;
   uint32_t      myType;

// #ifdef USE_BZLIB
//    int32_t          bzerror;
// #endif

   // Decode size
   myType = (size >> 28) & 0xF;
   mySize = (size & 0x0FFFFFFF);

   //cout << "Found Marker: Type=" << dec << myType << ", Size=" << dec << mySize << endl;

   // Read file
   buff = (char *) malloc(mySize+1);
   if ( data != NULL ) memcpy(buff,data,mySize);
   else if ( bzEnable_ ) {
// #ifdef USE_BZLIB
//       if ( BZ2_bzRead ( &bzerror,bzFile_,buff, mySize ) != (int32_t)mySize )  {
//          cout << "KpixRead::xmlParse -> Read error!" << endl;
//          return;
//       }
// #endif
   }

   else if ( ::read(fd_, buff, mySize) != (int32_t)mySize) {
      cout << "KpixRead::xmlParse -> Read error!" << endl;
      return;
   }
   buff[mySize-1] = 0;

   if ( myType == Data::XmlConfig   ) config_.parse("config",buff);
   if ( myType == Data::XmlStatus   ) status_.parse("status",buff);
   if ( myType == Data::XmlRunStart ) {
      //cout << "-----------XML Start---------------" << endl;
      //cout << buff << endl;
      //cout << "-----------------------------------" << endl;
      start_.parse("runStart",buff);
   }
   if ( myType == Data::XmlRunStop  ) {
      //cout << "-----------XML Stop----------------" << endl;
      //cout << buff << endl;
      //cout << "-----------------------------------" << endl;
      stop_.parse("runStop",buff);
   }
   if ( myType == Data::XmlRunTime  ) {
      //cout << "-----------XML Time----------------" << endl;
      //cout << buff << endl;
      //cout << "-----------------------------------" << endl;
      time_.parse("runTime",buff);
   }

   //std::cout << buff << std::endl;
   free(buff);
}

// Open file
bool KpixRead::open ( string file, bool compressed ) {

// #ifdef USE_BZLIB
//    bzEnable_ = compressed;
//    int32_t    bzerror;
//    FILE * f;
// #else
   bzEnable_ = false;
// #endif

   size_ = 0;
   status_.clear();
   config_.clear();

   // Attempt to open compressed file
//    if ( bzEnable_ ) {

// #ifdef USE_BZLIB

//       // Open file
//       f = fopen ( file.c_str(), "r" );

//       // Attempt to compress file
//       if ( f ) {
//          bzFile_ = BZ2_bzReadOpen(&bzerror,f,0,0,NULL,0);
//          if ( bzerror != BZ_OK ) bzEnable_ = false;
//       }
//       else bzEnable_ = false;

//       if ( !bzEnable_ ) {
//          cout << "KpixRead::open -> Failed to open compressed file: " << file << endl;
//          return(false);
//       }
//       cout << "Opened compressed file" << endl;
// #endif

//    }

   // Attempt to open file
   //   else {
      if ( (fd_ = ::open (file.c_str(),O_RDONLY | O_LARGEFILE)) < 0 ) {
         cout << "KpixRead::open -> Failed to open file: " << file << endl;
         return(false);
      }
      //   }
   return(true);
}

// Open file
void KpixRead::close () {
// #ifdef USE_BZLIB
//    int32_t bzerror;
// #endif

   if ( bzEnable_ ) {

// #ifdef USE_BZLIB
//       BZ2_bzReadClose(&bzerror,bzFile_);
//       bzEnable_ = false;
// #endif

   } else {
      ::close(fd_);
      fd_ = -1;
   }
}

//! Return file size in bytes
off_t KpixRead::size ( ) {
   off_t curr;

   if ( fd_ < 0 ) return(0);
   if ( size_ == 0 ) {
      curr  = lseek(fd_, 0, SEEK_CUR);
      size_ = lseek(fd_, 0, SEEK_END);
      lseek(fd_, curr, SEEK_SET);
   }
   return(size_);
}

//! Return file position in bytes
off_t KpixRead::pos ( ) {
   if ( fd_ < 0 ) return(0);
   return(lseek(fd_, 0, SEEK_CUR));
}

// Get next data record
bool KpixRead::next (Data *data) {
   uint32_t size;
   char *shBuff;
   bool found = false;

// #ifdef USE_BZLIB
//    int32_t  bzerror;
// #endif

   if ( fd_ < 0 && smem_ == NULL && !bzEnable_ ) return(false);

   // Read until we get data
   do { 

      // First read frame size from data file
      if ( smem_ != NULL ) {
         if ( dataSharedRead((DataSharedMemory *)smem_,&rdAddr_,&rdCount_, &size, (uint8_t **)(&shBuff) ) == 0 ) {
            return(false);
         }
      } 
      else if ( bzEnable_ ) {

// #ifdef USE_BZLIB

//          cout << "Reading size field" << endl;
//          if ( BZ2_bzRead ( &bzerror,bzFile_,&size,4 ) != 4 ) {
//             cout << "Size field read fail" << endl;
//             return(false);
//          }
//          shBuff = NULL;
// #endif

      }
      else {
	if ( read(fd_,&size,4) != 4 ) return(false);
         shBuff = NULL;
      }

      if ( size == 0 ) continue;

      //cout << "Size field = 0x" << hex << size << endl; // wmq
      //cout << "size >> 28 = " << (size >> 28) <<endl; // wmq
      // Frame type
      switch ( (size >> 28) & 0xF ) {
         
         // Data
         case Data::RawData : found = true; break;

         // Configuration
         case Data::XmlConfig : xmlParse(size,shBuff); break;

         // Status
         case Data::XmlStatus : xmlParse(size,shBuff); break;

         // Start
         case Data::XmlRunStart : sawRunStart_ = true; xmlParse(size,shBuff); break;

         // Stop
         case Data::XmlRunStop : sawRunStop_ = true; xmlParse(size,shBuff); break;

         // Time
         case Data::XmlRunTime : sawRunTime_ = true; xmlParse(size,shBuff); break;

         // Unknown
         default: 
            cout << "KpixRead::next -> Unknown data type 0x" 
                 << hex << setw(8) << setfill('0') << ((size >> 28) & 0xF) << " skipping." << endl;
            if ( smem_ != NULL ) return(false);   
            else return(lseek(fd_, (size & 0x0FFFFFFF), SEEK_CUR));
            break;
      }
   } while ( ! found );

   // Read data
   if ( smem_ != NULL ) {
      data->copy ( (uint32_t *)shBuff,size );
      cout<<"Read Data: I am copying!\n"; // wmq
      return(true);
   }
   else {
      if (bzEnable_) return(data->read(bzFile_,size));
      else return(data->read(fd_,size));
   }
}

// Get next data record
Data *KpixRead::next ( ) {
  Data *tmp = new Data;
   if ( next(tmp) ) return(tmp);
   else {
      delete tmp;
      return(NULL);
   }
}

// Get a config value
string KpixRead::getConfig ( string var ) {
   return(config_.get(var));
}

// Get a status value
string KpixRead::getStatus ( string var ) {
   return(status_.get(var));
}

// Get a config value
uint32_t KpixRead::getConfigInt ( string var ) {
   return(config_.getInt(var));
}

// Get a status value
uint32_t KpixRead::getStatusInt ( string var ) {
   return(status_.getInt(var));
}

// Dump config
void KpixRead::dumpConfig ( ) {
   cout << "Dumping current config variables:" << endl;
   cout << config_.getList("   Config: ");
}

// Dump status
void KpixRead::dumpStatus ( ) {
   cout << "Dumping current status variables:" << endl;
   cout << status_.getList("   Status: ");
}

//! Get config as XML
string KpixRead::getConfigXml ( ) {
   string ret;
   ret = "";
   ret.append("<system>\n");
   ret.append("   <config>\n");
   ret.append(config_.getXml());
   ret.append("   </config>\n");
   ret.append("</system>\n");
   return(ret);
}

//! Dump status
string KpixRead::getStatusXml ( ) {
   string ret;
   ret = "";
   ret.append("<system>\n");
   ret.append("   <status>\n");
   ret.append(status_.getXml());
   ret.append("   </status>\n");
   ret.append("</system>\n");
   return(ret);
}

// Get a start value
string KpixRead::getRunStart ( string var ) {
   return(start_.get(var));
}

// Get a stop value
string KpixRead::getRunStop ( string var ) {
   return(stop_.get(var));
}

// Get a time value
string KpixRead::getRunTime ( string var ) {
   return(time_.get(var));
}

// Dump start
void KpixRead::dumpRunStart ( ) {
   cout << "Dumping run start variables:" << endl;
   cout << start_.getList("   RunStart: ");
}

// Dump stop
void KpixRead::dumpRunStop ( ) {
   cout << "Dumping run stop variables:" << endl;
   cout << stop_.getList("    RunStop: ");
}

// Dump time
void KpixRead::dumpRunTime ( ) {
   cout << "Dumping run time variables:" << endl;
   cout << time_.getList("    RunTime: ");
}

// Return true if we saw start marker, self clearing
bool  KpixRead::sawRunStart ( ) {
   bool ret = sawRunStart_;
   sawRunStart_ = false;
   return(ret);
}

// Return true if we saw stop marker, self clearing
bool  KpixRead::sawRunStop ( ) {
   bool ret = sawRunStop_;
   sawRunStop_ = false;
   return(ret);
}

// Return true if we saw time marker, self clearing
bool  KpixRead::sawRunTime ( ) {
   bool ret = sawRunTime_;
   sawRunTime_ = false;
   return(ret);
}

// Enable shared 
// void KpixRead::openShared ( string system, uint32_t id, int32_t uid ) {

//    // Attempt to open and init shared memory
//    if ( (smemFd_ = dataSharedOpenAndMap ( (DataSharedMemory **)(&smem_) , system.c_str(), id, uid )) < 0 ) {
//       smem_ = NULL;
//       throw string("CommLink::enabledSharedMemory -> Failed to open shared memory");
//    }
//    rdAddr_  = 0;
//    rdCount_ = 0;
// }

