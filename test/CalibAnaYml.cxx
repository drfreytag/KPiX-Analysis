//-----------------------------------------------------------------------------
// File          : CalibAnaYml.cpp
// Author        : Mengqing Wu <mengqing.wu@desy.de>
// Created       : 25/10/2018
// Project       : Lycoris Analysis Package
//-----------------------------------------------------------------------------
// Description :
// Application to process and fit kpix calibration data
// Code derivate from Ryan T. Herbst @SLAC
//-----------------------------------------------------------------------------
// Modification history :
// 25/10/2018: Modified - Naive reader, do not need any conf yaml input
//-----------------------------------------------------------------------------
#include <iostream>
#include <iomanip>
#include <stdarg.h>
#include <math.h>
#include <cmath> /*std::isnan*/
#include <fstream>

#include <TFile.h>
#include <TH1F.h>
#include <TH2F.h>
#include <TCanvas.h>
#include <TMultiGraph.h>
#include <TApplication.h>
#include <TGraphErrors.h>
#include <TGraph.h>
#include <TStyle.h>

#include "KpixEvent.h"
#include "KpixSample.h"
#include "Data.h"
#include "DataRead.h"

#include "KpixCalibData.h"
#include "YmlVariables.h"

using namespace std;

// Function to compute calibration charge
double calibCharge ( uint dac, bool positive, bool highCalib ) {
   double volt;
   double charge;

   if ( dac >= 0xf6 ) volt = 2.5 - ((double)(0xff-dac))*50.0*0.0001;
   else volt =(double)dac * 100.0 * 0.0001;

   if ( positive ) charge = (2.5 - volt) * 200e-15;
   else charge = volt * 200e-15;

   if ( highCalib ) charge *= 22.0;

   return(charge);
}


// Process the data
int main ( int argc, char **argv ) {
  DataRead               dataRead;
     

  return (0);
}

