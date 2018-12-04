//-----------------------------------------------------------------------------
// File          : KpixEvent.h
// Author        : Ryan Herbst  <rherbst@slac.stanford.edu>
// Created       : 05/29/2012
// Project       : KPIX DAQ Software
//-----------------------------------------------------------------------------
// Description :
// Event Data consists of the following: Z[xx:xx] = Zeros
//    Header = 8 x 32-bits
//       Header[0] = EventNumber[31:0]
//       Header[1] = Timestamp[31:00]
//       Header[2] = Zeros[31:0]
//       Header[3] = Zeros[31:0]
//       Header[4] = Zeros[31:0]
//       Header[5] = Zeros[31:0]
//       Header[6] = Zeros[31:0]
//       Header[7] = Zeros[31:0]
//
//    Samples = 2 x 32-bits
//
//    Tail = 1 x 32-bits
//       Tail[0] = Zeros
//       Tail[1] = Zeros
//-----------------------------------------------------------------------------
// Copyright (c) 2012 by SLAC. All rights reserved.
// Proprietary and confidential to SLAC.
//-----------------------------------------------------------------------------
// Modification history :
// 05/29/2012: created
//----------------------------------------------------------------------------
// Modified by Mengqing Wu for Lycoris Project @ 28/09/2018
// -- 
//----------------------------------------------------------------------------
#include <iostream>
#include <string>
#include <string.h>
#include <unistd.h>
#include <math.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "KpixEvent.h"

#include  <iomanip>

using namespace std;

// Constructor
KpixEvent::KpixEvent () : Data() { }

// Deconstructor
KpixEvent::~KpixEvent () { }

// Get event number
uint KpixEvent::eventNumber ( ) {
   return(data_[0]);
}

// Get timestamp
uint KpixEvent::timestamp ( ) {
   return(data_[1]);
}

// Get sample count
uint KpixEvent::count ( ) {
   uint rem = 0;
   if ( size_ <= (headSize_ + tailSize_)) return(0);

   rem = (size_-(headSize_ + tailSize_));

   if ( (rem % sampleSize_) != 0 ) return(0);

   return(rem/sampleSize_);
}

// Get sample at index
KpixSample *KpixEvent::sample (uint index) {
   if ( index >= count() ) return(NULL);
   else {
     //cout << "[evt debug] sample data : 0x"<< hex << setw(8) << setfill('0')<< (data_[headSize_+(index*sampleSize_)]) << endl;
      sample_.setData(&(data_[headSize_+(index*sampleSize_)]),eventNumber());
      return(&sample_);
   }
}

// Get sample at index
KpixSample *KpixEvent::sampleCopy (uint index) {
   KpixSample *tmp;

   if ( index >= count() ) return(NULL);
   else {
      tmp = new KpixSample (&(data_[headSize_+(index*sampleSize_)]),eventNumber());
      return(tmp);
   }
}

