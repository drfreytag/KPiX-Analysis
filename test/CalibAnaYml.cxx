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

#include "KpixCalibData.h" /*'ChannelData' class from calibrationFitter.cxx*/
#include "YmlVariables.h"

using namespace std;
using namespace lycoris;

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

struct CalibData {
  uint kpix;
  uint channel;
  uint bucket;
  uint range; // not often used
  lycoris::KpixCalibData data;
};
//////
struct find_dataIndex {
  uint kpix;
  find_dataIndex(uint kpix) : kpix(kpix) {}
  bool operator () ( CalibData const& c) const {
    return c.kpix == kpix;
  }
};

//////

// Process the data
int main ( int argc, char **argv ) {
  
  // generic:
  bool                   debug=true;
  
  // Data container init:
  std::vector <CalibData> v_ChanData;

  // kpix data read
  DataRead               dataRead;
  off_t                  fileSize, filePos;
  uint                   lastPct = 100;
  uint                   currPct = 0;

  bool                   isYml=true;
  string                 calState;
  uint                   calChannel, calDac;
  uint                   injectTime[5];

  // kpix events 
  KpixEvent              event;
  uint                   eventCount;
  
  // kpix samples
  KpixSample             *sample;
  //uint                   range, value, kpix, channel, bucket, tstamp;
  uint                   value, tstamp;
  string                 serial;
  KpixSample::SampleType type; 
  uint                   badTimes;

  // string/stream init
  ofstream               logfile;
  stringstream           tmp;
  string                 outRoot;

  // Root Histos init
  TH1F                   *hist;
     
  // Data file is the first and only arg
  if ( argc != 2 ) {
    cout << "\nUsage: \ncalibrationFitter data_file \n\n";
    return(1);
  }
  
  // Open data file
  if ( ! dataRead.open(argv[1]) ) {
    cout << "Error opening data file " << argv[1] << endl;
    return(1);
  }
  
  // debug log:
  if (debug){
    cout << "open a debug log file"<< endl;
    logfile.open("debug.log", std::ofstream::out);
  }
  // Create output names
  tmp.str("");
  tmp << argv[1] << ".calib.root";
  outRoot = tmp.str();
  
  //////////////////////////////////////////
  // Read Data
  //////////////////////////////////////////
  cout << "Opened data file: " << argv[1] << endl;
  fileSize = dataRead.size();
  filePos  = dataRead.pos();
  
  // Init
  cout << "\rReading File: 0 %" << flush;
  
  // Process each event
  while ( dataRead.next(&event) ) {

    // Get Config - injection times from yml in bin
    if ( eventCount == 0 ) {
      injectTime[0] = dataRead.getYmlConfigInt("KpixDaqCore:KpixAsicArray:KpixAsic[24]:Cal0Delay");
      injectTime[1] = dataRead.getYmlConfigInt("KpixDaqCore:KpixAsicArray:KpixAsic[24]:Cal1Delay") + injectTime[0] + 4;
      injectTime[2] = dataRead.getYmlConfigInt("KpixDaqCore:KpixAsicArray:KpixAsic[24]:Cal2Delay") + injectTime[1] + 4;
      injectTime[3] = dataRead.getYmlConfigInt("KpixDaqCore:KpixAsicArray:KpixAsic[24]:Cal3Delay") + injectTime[2] + 4;
      injectTime[4] = 8192;

      logfile << "CalDelay : [" << dec
	      << injectTime[0] << ", "	
	      << injectTime[1] << ", "
	      << injectTime[2] << ", "
	      << injectTime[3] << ", "
	      << injectTime[4] << "]" << endl;
    }
    
    // Get calibration state
    calState   = dataRead.getYmlStatus("CalState");
    calChannel = dataRead.getYmlStatusInt("CalChannel");
    calDac     = dataRead.getYmlStatusInt("CalDac");

    bool foundCalChannel = false;
    
    logfile << "CalState : "  << calState << "\n"
	    << "CalChannel : "<< calChannel << "\n"
	    << "CalDac : "    << calDac << "\n";

    // debug: verify the data structure @ Nov 1, 2018
    uint ts = event.timestamp();
    uint nevt = event.eventNumber();
    logfile << "event number : " << nevt
	    << ", timestamp : "  << ts   << "\n";
    
    // get each sample
    for (uint x=0; x < event.count(); x++) {
      // Get sample
      sample  = event.sample(x);
      type    = sample->getSampleType();

      CalibData cdata;
      cdata.kpix    = sample->getKpixAddress();
      cdata.channel = sample->getKpixChannel();
      cdata.bucket  = sample->getKpixBucket();
      cdata.range   = sample->getSampleRange();
      
      value   = sample->getSampleValue();
      tstamp  = sample->getSampleTime();

      // debug: verify the data structure @ Nov 1, 2018
      logfile << " Sample data:" << "\n" 
	      << "  - kpix:   " << sample->getKpixAddress() << "\n"
	      << "  - evtNum: " << sample->getEventNum() << "\n"
	      << "  - tstamp: " << sample->getSampleTime() << "\n"
	      << "  - channel:" << sample->getKpixChannel() << "\n"
	      << "  - bucket: " << sample->getKpixBucket() << "\n"
	      << "  - trgType:" << sample->getTrigType() << "\n";


            
      // Only process real Data samples
      if ( type == KpixSample::Data ) {

	// add data entry
	KpixCalibData data;

	// Non calibration based run. Fill mean, ignore times
	if ( calState == "Idle"){
	  cout << "I have an idle event!"<< endl;
	  data.addBasePoint(value);
	}
	
	// // Filter for time
	else if ( tstamp > injectTime[bucket] && tstamp < injectTime[bucket+1] ) {
	  //else if(true){
	  // Baseline
	  if ( calState == "Baseline" )
	    data.addBasePoint(value);
	  // Injection
	  else if ( calState == "Inject"/* && calDac != minDac */) {

	    printf(" [debug] channel : %d ? calChannel : %d", cdata.channel, calChannel);
	    
	    if ( cdata.channel == calChannel ) {
	      //cout<< "[dev] it is the calChannel !" << endl;
	      data.addCalibPoint(calDac, value);
	    }
	    //else if ( data != NULL )
	    else {
	      auto it = std::find_if( v_ChanData.begin(), v_ChanData.end(),
	    			 find_dataIndex(kpix) );
	      if ( it! = v_ChanData.end() ){
	    	printf( "[dev] It is neighborhit! of kpix %d : channel %d :bucket %d : range %d\n",
	    		kpix, cdata.channel, bucket, range);
	    	data.addNeighborPoint(cdata.channel, calDac, value);
	      }
	      // cout<< "[dev] it is not neighbor hit!" << endl;
	    }
	  }
	  
	  else badTimes++;
	
	  // push to data map
	  cdata.data = data;
	  //v_ChanData.push_back(cdata);
	  
	}
      }
    }

    /*// begin - Show progress
    filePos  = dataRead.pos();
    currPct = (uint)(((double)filePos / (double)fileSize) * 100.0);
    if ( currPct != lastPct ) {
      cout << "\rReading File: " << currPct << " %      " << flush;
      lastPct = currPct;
    }
    // end - Show read process
    */
    eventCount++;

    //if (eventCount>0) break;
      
  } 
  cout << "\rReading File: Done.               " << endl;
  dataRead.close();

  logfile.close();
  return (0);
}

