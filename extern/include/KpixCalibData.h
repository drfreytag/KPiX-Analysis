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

// Channel data
class KpixCalibData {
 public:

  // Baseline Data
  uint         baseData[8192];
  uint         baseMin;
  uint         baseMax;
  double       baseCount;
  double       baseMean;
  double       baseSum;
  double       baseRms;
  
  // Baseline fit data
  double       baseFitMean;
  double       baseFitSigma;
  double       baseFitMeanErr;
  double       baseFitSigmaErr;
  double       baseFitChisquare;
  
  // Calib Data
  double       calibCount[256];
  double       calibMean[256];
  double       calibSum[256];
  double       calibRms[256];
  double       calibError[256];
  double       calibOtherValue[1024];
  double       calibOtherDac[1024];
  
  KpixCalibData() ;
  ~KpixCalibData(){};
  
  void addBasePoint(uint data) ;
  
  void addCalibPoint(uint x, uint y);
  
  void addNeighborPoint(uint chan, uint x, uint y) ;
  void computeBase () ;
    
  void computeCalib(double chargeError);
  

};
