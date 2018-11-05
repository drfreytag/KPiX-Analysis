//-----------------------------------------------------------------------------
// File          : CalibAnaYml.cpp
// Author        : Mengqing Wu <mengqing.wu@desy.de>
// Created       : 25/10/2018
// Project       : KPiX Calibration Data RO
//-----------------------------------------------------------------------------
// Comment       : originally from Ryan T. Herbst @SLAC
//-----------------------------------------------------------------------------

#include <iostream>
#include <iomanip>
#include <stdarg.h>
#include <math.h>
#include <cmath> /*std::isnan*/
#include <fstream>

#include "KpixCalibData.h"

using namespace std;

// Old class name: ChannelData
using namespace lycoris;

KpixCalibData::KpixCalibData() {
  uint x;
  
  for (x=0; x < 8192; x++) baseData[x] = 0;
  baseMin          = 8192;
  baseMax          = 0;
  baseCount        = 0;
  baseMean         = 0;
  baseSum          = 0;
  baseRms          = 0;
  baseFitMean      = 0;
  baseFitSigma     = 0;
  baseFitMeanErr   = 0;
  baseFitSigmaErr  = 0;
  baseFitChisquare = 0;
  
  for (x=0; x < 256; x++) {
    calibCount[x]  = 0;
    calibMean[x]   = 0;
    calibSum[x]    = 0;
    calibRms[x]    = 0;
    calibError[x]  = 0;
  }
  for (x=0; x < 1024; x++) {
    calibOtherValue[x] = 0;
    calibOtherDac[x] = 0;
  }
}

void KpixCalibData::addBasePoint(uint data) {
  baseData[data]++;
  if ( data < baseMin ) baseMin = data;
  if ( data > baseMax ) baseMax = data;
  baseCount++;
  
  double tmpM = baseMean;
  double value = data;
  
  baseMean += (value - tmpM) / baseCount;
  baseSum  += (value - tmpM) * (value - baseMean);
}

void KpixCalibData::addCalibPoint(uint x, uint y) {
	calibCount[x]++;
	
	double tmpM = calibMean[x];
	double value = y;
	
	calibMean[x] += (value - tmpM) / calibCount[x];
	calibSum[x]  += (value - tmpM) * (value - calibMean[x]);
      }

void KpixCalibData::addNeighborPoint(uint chan, uint x, uint y) {
         if ( y > calibOtherValue[chan] ) {
            calibOtherValue[chan] = y;
            calibOtherDac[chan] = x;
         }
      }

void KpixCalibData::computeBase () {
         if ( baseCount > 0 ) baseRms = sqrt(baseSum / baseCount);
      }

void KpixCalibData::computeCalib(double chargeError) {
  uint   x;
  double tmp;
  
  for (x=0; x < 256; x++) {
    if ( calibCount[x] > 0 ) {
      calibRms[x] = sqrt(calibSum[x] / calibCount[x]);
      tmp = calibRms[x] / sqrt(calibCount[x]);
      calibError[x] = sqrt((tmp * tmp) + (chargeError * chargeError));
    }
  }
}
