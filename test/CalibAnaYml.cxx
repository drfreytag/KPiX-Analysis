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
// 15/11/2018: TBD - naive copy of old calibrationFitter, problems to solve for inject plots!
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
  lycoris::KpixCalibData *data;
};

// Process the data
int main ( int argc, char **argv ) {
  
  // generic:
  bool                   debug=true;
  
  // Data container init:
  //std::vector <CalibData> v_ChanData;
  bool                   kpixFound[24] = {false};
  bool                   chanFound[24][1024] = {false};
  KpixCalibData          *chanData[24][1024][4][2] = {nullptr};  

  // kpix data read
  DataRead               dataRead;
  off_t                  fileSize, filePos;
  uint                   lastPct = 100;
  uint                   currPct = 0;

  //bool                   isYml=true;
  string                 calState;
  uint                   calChannel, calDac;
  uint                   injectTime[5];
  uint                   minDac;
  uint                   minChan;
  uint                   maxChan;
  bool                   positive = true; // TBA!
  bool                   b0CalibHigh = false; // TBA!
  
  // kpix events 
  KpixEvent              event;
  uint                   eventCount = 0;
  
  // kpix samples
  KpixSample             *sample;
  uint                   kpix, channel, bucket, range;
  uint                   value, tstamp;
  string                 serial;
  KpixSample::SampleType type; 
  uint                   badTimes = 0;

  // string/stream init
  ofstream               logfile;
  stringstream           tmp;
  string                 outRoot;

  // Root Histos init
  TH1F                   *hist;
  TGraphErrors           *grCalib;
  TGraph                 *grResid;

  // calibration calculation container
  uint                   grCount;
  double                 grX[256] = {0};
  double                 grY[256] = {0};
  double                 grYErr[256] = {0};
  double                 grXErr[256] = {0};
  double                 grRes[256] = {0};
  double                 chargeError[2] = {0}; // not verified w/ old, not work!
  double                 fitMin[2] = {0}; // not verified w/ old, not work!
  double                 fitMax[2] = {0}; // not verified w/ old, not work!
  
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

  TFile* rFile = new TFile(outRoot.c_str(),"recreate");
    
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

      minDac        = dataRead.getYmlStatusInt("CalDacMin");
      minChan       = dataRead.getYmlStatusInt("CalChanMin");
      maxChan       = dataRead.getYmlStatusInt("CalChanMax");

      logfile << "CalDacMin : " << minDac <<endl;
      logfile << "CalChanMin, Max : " << minChan << ", " << maxChan << endl;
      
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
    
    logfile << "CalState :" << calState << "\n"
	    << "CalChannel : " << calChannel << "\n"
	    << "CalDac :     " << calDac << "\n";

    // debug: verify the data structure @ Nov 1, 2018
    uint ts = event.timestamp();
    uint nevt = event.eventNumber();
    //logfile << "event number : " << nevt
    //<< ", timestamp : "  << ts   << "\n";
    
    // get each sample
    for (uint x=0; x < event.count(); x++) {
      // Get sample
      sample  = event.sample(x);
      type    = sample->getSampleType();

      kpix    = sample->getKpixAddress();
      channel = sample->getKpixChannel();
      bucket  = sample->getKpixBucket();
      range   = sample->getSampleRange();
      
      value   = sample->getSampleValue();
      tstamp  = sample->getSampleTime();

      // debug: verify the data structure @ Nov 1, 2018
      /*logfile << " Sample data:" << "\n" 
	      << "  - kpix:   " << kpix << "\n"
	      << "  - evtNum: " << sample->getEventNum() << "\n"
	      << "  - tstamp: " << sample->getSampleTime() << "\n"
	      << "  - channel:" << channel << "\n"
	      << "  - bucket: " << bucket << "\n"
	      << "  - trgType:" << sample->getTrigType() << "\n"
	      << "  - value:  " << value << "\n" ;
      */
      // Only process real Data samples
      if ( type == KpixSample::Data ) {

	// debug: select only bucket 0
	if (bucket!=0) continue;
	
	// new a data entry
	kpixFound[kpix]          = true;
	chanFound[kpix][channel] = true;

	if ( chanData[kpix][channel][bucket][range] == nullptr ) chanData[kpix][channel][bucket][range] = new KpixCalibData;

	//cout << "[debug] I am calState -> " << calState << endl;
	
	// Non calibration based run. Fill mean, ignore times
	if ( calState == "Idle")
	  //cout << "I have an idle event!"<< endl;
	  chanData[kpix][channel][bucket][range]->addBasePoint(value);
	
	
	// Filter for time
	//else if ( tstamp > injectTime[bucket] && tstamp < injectTime[bucket+1] ) {
	else if (true){
	  // Baseline
	  if ( calState == "Baseline" ){
	    //cout << "I am baseline!" << endl;
	    chanData[kpix][channel][bucket][range]->addBasePoint(value);
	  }
	  
	  // Injection
	  else if ( calState == "Inject"/* && calDac != minDac */) {

	    //printf(" [debug] channel : %d ? calChannel : %d\n", channel, calChannel);
	    
	    if ( channel == calChannel ) {
	      //cout<< "[dev] it is the calChannel !" << endl;
	      chanData[kpix][channel][bucket][range]->addCalibPoint(calDac, value);
	     }

	    else {
	      if (  chanData[kpix][calChannel][bucket][range] != nullptr ){
	    	//printf( "[dev] It is neighborhit! of kpix %d : channel %d :bucket %d : range %d\n",kpix, channel, bucket, range);
		chanData[kpix][calChannel][bucket][range]->addNeighborPoint(channel, calDac, value);
	      }
	      else /*cout<< "[dev] it is not neighbor hit!" << endl*/;
	    }
	  }
	}
	
	else badTimes++;

      }
    }

    // begin - Show progress
    filePos  = dataRead.pos();
    currPct = (uint)(((double)filePos / (double)fileSize) * 100.0);
    if ( currPct != lastPct ) {
      cout << "\rReading File: " << currPct << " %      " << flush;
      lastPct = currPct;
    }
    // end - Show read process
    
    eventCount++;

    //if (eventCount>0) break;
      
  } 
  cout << "\rReading File: Done.               " << endl;
  dataRead.close();

  //  cout << "[debug] how many base point count for k0_c0_b0_r0 : " <<  chanData[0][0][0][0]->baseCount << endl;
    
  //////////////////////////////////////////
  // Process Baselines 
  //////////////////////////////////////////

  logfile << "Process Baselines" << endl;
  
  // Process each kpix device
  for( kpix = 0; kpix<24; kpix++){
    // kpix is valid
    if ( !kpixFound[kpix] ) continue;

    //logfile << "Process kpix : " << kpix << endl;

    // Process each calib-channel
    for( channel=minChan; channel <= maxChan; channel++) {
      
      // Show process
      cout << "\rProcessing baseline kpix " << dec << kpix << " / 24" 
	   << ", Channel " << channel << " / " << dec << maxChan
	   << "                 " << flush;
      
      // channel is valid
      if ( !chanFound[kpix][channel] ) continue;

      //logfile << "\t Process channel : " << channel << endl;
      // loop over bucket
      for ( bucket=0; bucket<4; bucket++){
	// bucket is valid
	if ( chanData[kpix][channel][bucket][0] == nullptr && chanData[kpix][channel][bucket][1] == nullptr ) continue;

	//logfile << "\t\t Process bucket : " << bucket << endl;
	// loop over range: low gain is range==1, which is not applicable for tracker, so always ==0;
	for( range = 0; range<2; range++){
	  if ( chanData[kpix][channel][bucket][range] == nullptr ) continue;
	  
	  chanData[kpix][channel][bucket][range]->computeBase();

	  // Create histogram
	  tmp.str("");
	  tmp << "hist_k" << kpix << "_c" << dec << setw(4) << setfill('0') << channel;
	  tmp << "_b" << dec << bucket;
	  tmp << "_r" << dec << range;

	  //logfile << tmp.str() << endl ;
	  
	  hist = new TH1F(tmp.str().c_str(),tmp.str().c_str(),8192,0,8192);
	  // Fill histogram
	  for ( int adc=0; adc<8192; adc++){
	    hist->SetBinContent( adc+1, chanData[kpix][channel][bucket][range]->baseData[adc] );
	    //logfile << "\t(adc, value): " << adc+1 << ", "<< chanData[kpix][channel][bucket][range]->baseData[adc] << endl;
	  }

	  // Style histogram
	  hist->GetXaxis()->SetRangeUser(chanData[kpix][channel][bucket][range]->baseMin,
					 chanData[kpix][channel][bucket][range]->baseMax);
	  if ( hist->GetSumOfWeights() != 0 )
	    hist->Fit("gaus","q");
	  hist->Write();
	  
	}// loop over range
	
      }// loop over bucket
      
    }// loop over channel
  }// loop over kpix

  cout << endl;



  //////////////////////////////////////////
  // Process Calibration
  //////////////////////////////////////////
  
  for (kpix = 0; kpix<24; kpix++){
    if (!kpixFound[kpix]) continue;
    
    for( channel=minChan; channel<=maxChan; channel++) {
      
    // Show process
      cout << "\rProcessing calibration kpix " << dec << kpix << " / 24" 
	   << ", Channel " << channel << " / " << dec << maxChan
	   << "                 " << flush;

      if ( !chanFound[kpix][channel] ) continue;

      for( bucket = 0; bucket < 4; bucket++) {
	// bucket is valid
	if ( chanData[kpix][channel][bucket][0] == nullptr && chanData[kpix][channel][bucket][1] == nullptr ) continue;

	for( range=0; range<2; range++){
	  if ( chanData[kpix][channel][bucket][range] == nullptr ) continue;
	  
	  chanData[kpix][channel][bucket][range]->computeCalib(chargeError[range]);

	  // Create calibration graph
	  grCount = 0;

	  for (int dac =0; dac<256; dac++) {
	    
	    // Calibration point is valid
	    if ( chanData[kpix][channel][bucket][range]->calibCount[dac] > 0 ) {
	      
	      grX[grCount]    = calibCharge ( dac, positive, ((bucket==0)?b0CalibHigh:false) );
	      grY[grCount]    = chanData[kpix][channel][bucket][range]->calibMean[dac];
	      grYErr[grCount] = chanData[kpix][channel][bucket][range]->calibError[dac];
	      grXErr[grCount] = 0;
	      
	      /*
	      logfile << "Kpix=" << dec << kpix << " Channel=" << dec << channel << " Bucket=" << dec << bucket
		    << " Range=" << dec << range
		    << " Adding point x=" << grX[grCount] 
		    << " Rms=" << chanData[kpix][channel][bucket][range]->calibRms[x]
		    << " Error=" << chanData[kpix][channel][bucket][range]->calibError[x] << endl;
	      */
	      grCount++;
	    }//--- working point

	  }// loop over DAC value

	  // Create graph
	  if( grCount > 0 ) {
	    grCalib = new TGraphErrors(grCount,grX,grY,grXErr,grYErr);
	    grCalib->Draw("Ap");
	    grCalib->GetXaxis()->SetTitle("Charge [C]");
	    grCalib->GetYaxis()->SetTitle("ADC");
	    grCalib->Fit("pol1","eq","",fitMin[range],fitMax[range]);
	    grCalib->GetFunction("pol1")->SetLineWidth(1);
	    
	    // Create name and write
	    tmp.str("");
	    tmp << "calib_k" << kpix << "_c" << dec << setw(4) << setfill('0') << channel;
	    tmp << "_b" << dec << bucket;
	    tmp << "_r" << dec << range;
	    grCalib->SetTitle(tmp.str().c_str());
	    grCalib->Write(tmp.str().c_str());
	    
	    // TBD: add residual plot
	    
	  }
	    

	}// loop over range
      }// loop over bucket
    }// loop over channel
  } // loop over kpix
  


  //-- debug output:
  cout << " We have kpix: ";
  for (kpix=0; kpix<24; kpix++){
    if (kpixFound[kpix]) 
      cout << " " << kpix << ", ";
  }
  cout << endl;

  rFile->Close();
  delete rFile;
  
  logfile.close();
  return (0);
}

