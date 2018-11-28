//-----------------------------------------------------------------------------
// File          : analysis.cpp
// Author        : Uwe Kraemer (orig. Ryan Herbst) <uwe.kraemer@desy.de>
// Created       : 06/28/2017
// Project       : KPiX Analysis
//-----------------------------------------------------------------------------
// Description :
// General analysis of KPiX data.
//-----------------------------------------------------------------------------
// Copyright (c) 2012 by SLAC. All rights reserved.
// Proprietary and confidential to SLAC.
//-----------------------------------------------------------------------------
// Modification history :
// 05/30/2012: created
// 06/28/2017: large scale rewrite of original calibrationFitter.cpp
// 22/03/2018: clean up ecal plots and add strip plots by <mengqing.wu@desy.de>
//-----------------------------------------------------------------------------

#include <iostream>
#include <iomanip>
#include <TFile.h>
#include <TH1F.h>
#include <TH2F.h>
#include <TF1.h>
#include <TTree.h>
#include <TROOT.h>
#include <TPolyLine3D.h>
#include <TCanvas.h>
#include <TMultiGraph.h>
#include <TApplication.h>
#include <TGraphErrors.h>
#include <TGraph.h>
#include <TStyle.h>
#include <TVector.h>
#include <TKey.h>
#include <TClass.h>
#include <stdarg.h>
#include <KpixEvent.h>
#include <KpixSample.h>
#include <Data.h>
#include <DataRead.h>
#include <math.h>
#include <fstream>
#include <XmlVariables.h>
#include <string.h>
#include <algorithm>
#include <stdlib.h>

#include "kpixmap.h"
#include "kpix_left_and_right.h"
#include "cluster.h"
using namespace std;


//////////////////////////////////////////
// Global Variables
//////////////////////////////////////////
vector<TGraphErrors*> calib_graphs; //needed for current loopdir	
vector<TH1F*> pedestal_hists;
//////////////////////////////////////////
// Functions
//////////////////////////////////////////



void loopdir(TDirectory* dir, string histname)
{
	
	TDirectory *dirsav = gDirectory;
	TIter keys_iter(dir->GetListOfKeys());
	TKey* key;
	
	while ((key = (TKey*)keys_iter()))
	{
		if (key->IsFolder())
		{
			dir->cd(key->GetName());
			TDirectory *subdir = gDirectory;
			//subfolder->cd();
			loopdir(subdir, histname);
			dirsav->cd();
			continue;
		}
		else
		{
			string keyname = key->GetName();
			string keytype = key->GetClassName();
			//cout << histname << endl;
			//cout << found_calibpedestal << endl;
			//cout << keyname << endl;
			//cout << found_calibpedestal << endl;
			
			//cout << int(histname.find("pedestal") != -1) << endl;
			//cout << int(histname.find("calib") != -1) << endl;
			
			
			if (int(histname.find("pedestal") != -1))
			{
				int found2= keyname.find("_b");
				//cout << keyname << endl;
				//cout << found1 << endl;
				//cout << found2 << endl;
				//cout << "A " << string::npos() << endl;
				if (int(keytype.find("TH1") != -1))
				{
					if (int( keyname.find("hist_fc_") != -1) && found2 != -1)
					{
						//cout << keyname << endl;
						TH1F *pedestal_hist = (TH1F*)key->ReadObj();
						pedestal_hist->SetName(key->GetName());
						pedestal_hists.push_back(pedestal_hist);
					}
				}
			}
			else if ( int(histname.find("calib") != -1))
			{
				//cout << found_object << endl;
				//cout << keyname << endl;
				if (int(keytype.find("TH1") == -1))
				{
					
					if (int(keyname.find("calib") != -1)) 
					{
						//cout << keyname << endl;
						TGraphErrors *calib_graph = (TGraphErrors*)key->ReadObj();
						calib_graph->SetName(key->GetName());
						calib_graphs.push_back(calib_graph);
					}
				}
				//else
				//{
					//if (found_object == 0) 
					//{
						//TH1F *pedestal_hist = (TH1F*)key->ReadObj();
						//pedestal_hist->SetName(key->GetName());
						//pedestal_hists.push_back(pedestal_hist);
					//}
				//}
			
			}
		}
	}
}




// Coincidence function void coincidence(int* time_list1, int* time_list2, int* time_list3, int* channel_list1, int* channel_list2, int* channel_list3)


double smallest_time_diff( vector<double> ext_list, int int_value)
{
	double trigger_diff = 8200.0;
	for (uint k = 0; k<ext_list.size(); ++k)
	{
		double delta_t = int_value-ext_list[k];
		if (fabs(trigger_diff) > fabs(delta_t) && delta_t > 0) 
		{
			trigger_diff = delta_t;
		}
	}
	return trigger_diff;
}



//////////////////////////////////////////
// Begin of main analysis
//////////////////////////////////////////
int main ( int argc, char **argv )
{
	
	TH1::SetDefaultSumw2();
	
	//////////////////////////////////////////
	// Class declaration and histogram initialization
	//////////////////////////////////////////
	
	DataRead               dataRead;  //kpix event classes used for analysis of binary date
	off_t                  fileSize;  //
	off_t                  filePos;   //
	KpixEvent              event;    //
	KpixSample             *sample;   //
	
	// cycles to skip in front:
	long int                   skip_cycles_front;
	FILE*                  f_skipped_cycles;
	string                 outtxt;
	
	string                 calState;
	uint                   lastPct;
	uint                   currPct;
	bool                   kpixFound[32]; // variable that gives true if a kpix at the index n (0<=n<32) was found
	bool                   chanFound[32][1024];
	uint                   x;
	uint                   value;
	uint                   kpix;
	uint                   channel;
	uint                   bucket;
	double                  tstamp;
	string                 serial;
	KpixSample::SampleType type;	

	TH1F				*fc_response[32][5];
	
	vector<TH1F*> 		fc_response_cycle;
	vector<TH1F*> 		fc_response_cycle_mean_subtracted;
	vector<TH1F*> 		fc_response_cycle_median_subtracted;
	vector<TH1F*> 		fc_response_cycle_gauss_subtracted;
	
	TH1F				*fc_response_mean_subtracted[32][5];
	TH1F				*fc_response_CM_subtracted[32][5];
	TH1F				*fc_response_CMmedian_subtracted[32][5];
	TH1F				*fc_response_CMgauss_subtracted[32][5];
	TH1F				*fc_response_median_subtracted[32][5];
	TH1F				*fc_response_gauss_subtracted[32][5];
	TH1F				*fc_response_channel[32][1024];
	TH1F				*fc_response_mean_subtracted_channel[32][1024];
	TH1F				*fc_response_median_subtracted_channel[32][1024];
	
	
	TH1F				*mean_charge[32][5];
	TH1F				*median_charge[32][5];
	
	
	TH1F				*common_mode_hist[32];
	
	//TH1F				*fc_response_subgroup[32][5];
	//TH1F				*fc_response_subtracted_subgroup[32][5];
	
	TH2F 				*position_vs_charge_corrected[32][5];
	TH2F 				*kpix_position_vs_charge_corrected[32][5];
	TH2F 				*position_vs_charge[32][5];
	
	//TH1F				*fc_response_timed[32][5];
	//TH1F				*fc_response_subtracted_timed[32][5];
	TH1F				*slopes[32][5];
	
	TH2F				*slopes_vs_strip[32][5];
	
	TH1F				*pedestals_mean[32][5];
	TH1F				*pedestals_median[32][5];
	TH1F				*pedestals_gauss[32][5];
	
	// Stringstream initialization for histogram naming
	stringstream           tmp;
	stringstream           tmp_units;
	
	// Stringstream initialization for folder naming
	
	stringstream			FolderName;
	
	ofstream               xml;
	ofstream               csv;
	uint                   acqCount; // acquisitionCount
	uint                   acqProcessed;
	string                 outRoot;
	TFile                  *rFile;
	stringstream           crossString;
	stringstream           crossStringCsv;
	XmlVariables           config;
	ofstream               debug;
	
	// Calibration slope, is filled when 
	double					calib_slope[32][1024] = {1}; //ADD buckets later.
	double					pedestal_mean[32][1024][4] = {0};
	double					pedestal_median[32][1024][4] = {0};
	double					pedestal_gauss[32][1024][4] = {0};
	int						calibration_check = 0;
	//int						pedestal_check = 0;
	
	unordered_map<uint, uint> kpix2strip_left;
	unordered_map<uint, uint> kpix2strip_right;
	kpix2strip_left = kpix_left();
	kpix2strip_right = kpix_right();
	
	pixel					pixel_kpix[1024];
	pixel_mapping(pixel_kpix);
	
	
	
	//////////////////////////////////////////
	// Skip cycle/Read calibration file
	//////////////////////////////////////////
	
	// Data file is the first and only arg
	if (argc!= 4) {
	cout << "Usage: ./analysis data_file [skip_cycles_front (int)]||[read calibration input file (char)] \n";
	return(1);
	}
	
	char* end;
	
	
	if ( argc == 4 ) {
		//cout << "Even more debug " << strtol(argv[2], &end, 10) << endl;
		if (strtol(argv[2], &end, 10) != 0 )
		{
			skip_cycles_front = strtol(argv[2], &end, 10);
			cout<< " -- I am skipping first events: " << skip_cycles_front << endl;
			tmp.str("");
			tmp << argv[1] << ".printSkipped.txt";
			outtxt = tmp.str();
			f_skipped_cycles = fopen(outtxt.c_str(), "w");
		}
		else 
		{
			cout << " -- Reading " << argv[2] << " as calibration input file." << endl;
			skip_cycles_front = 0;
			TFile *calibration_file = TFile::Open(argv[2]);
			calibration_check = 1;
			loopdir(calibration_file, "calib");
			
			cout << "Number of calibration slopes = " << calib_graphs.size() << endl;
			
			for (unsigned int i = 0; i<calib_graphs.size(); ++i)
			{
				//cout << "Current key1 = " << cal_key->GetClassName() << endl;
				
				string calib_name         = calib_graphs[i]->GetName();
				//cout << calib_name << endl;
				size_t kpix_num_start     = calib_name.find("_k")+2;
				size_t channel_num_start  = calib_name.find("_c")+2;
				size_t kpix_num_length       = calib_name.length() - kpix_num_start;
				size_t channel_num_length    = calib_name.find("_b") - channel_num_start;
				
				//cout << kpix_num_start << endl;
				//cout << kpix_num_length << endl;
				//
				//cout << channel_num_start << endl;
				//cout << channel_num_length << endl;
				
			    string channel_string = calib_name.substr(channel_num_start, channel_num_length);
			    string kpix_string = calib_name.substr(kpix_num_start, kpix_num_length);
			    
			   //cout << "Channel Number = " <<  channel_string << endl;
			   //cout << "KPiX Number = " << kpix_string << endl;
			    
			    
			    int kpix_num = stoi(kpix_string);
			    int channel_num = stoi(channel_string);
				
				//cout << "KPiX Number = " << kpix_num << endl;
				//cout << "Channel Number = " << channel_num << endl;
				
				calib_slope[kpix_num][channel_num] = calib_graphs[i]->GetFunction("pol1")->GetParameter(1);
				//cout << "Slope of KPiX " << kpix_num << " and channel " << channel_num << " is " <<  calib_slope[kpix_num][channel_num] << endl;
				
			}
			cout << " -- Reading " << argv[3] << " as pedestal subtraction input file." << endl;
			skip_cycles_front = 0;
			TFile *pedestal_file = TFile::Open(argv[3]);
			//pedestal_check = 1;
			loopdir(pedestal_file, "pedestal");
			
			cout << "Number of pedestal histograms = " << pedestal_hists.size() << endl;
			for (unsigned int i = 0; i<pedestal_hists.size(); ++i)
			{
				//cout << "Current key1 = " << cal_key->GetClassName() << endl;
				
				string pedestal_name         = pedestal_hists[i]->GetName();
				
				//cout << pedestal_name << endl;
				
				size_t kpix_num_start     = pedestal_name.find("_k")+2;
				size_t channel_num_start  = pedestal_name.find("_c")+2;
				size_t kpix_num_length       = pedestal_name.length() - kpix_num_start;
				size_t channel_num_length    = pedestal_name.find("_b") - channel_num_start;
				
				size_t bucket_num_start 		= pedestal_name.find("_b")+2;
				size_t bucket_num_length      	= pedestal_name.length();
				
			    string channel_string = pedestal_name.substr(channel_num_start, channel_num_length);
			    string kpix_string = pedestal_name.substr(kpix_num_start, kpix_num_length);
			    string bucket_string = pedestal_name.substr(bucket_num_start, bucket_num_length);
			    
			    //cout << "Channel Number = " <<  channel_string << endl;
			    //cout << "KPiX Number = " << kpix_string << endl;
			    
			    int kpix_num = stoi(kpix_string);
			    int channel_num = stoi(channel_string);
			    int bucket_num = stoi(bucket_string);
			    
				
				pedestal_mean[kpix_num][channel_num][bucket_num] = pedestal_hists[i]->GetMean();
				//cout << "Pedestal of KPiX " << kpix << " and channel " << channel << " is " <<  pedestal[kpix_num][channel_num] << endl;
				
				
				//cout << "Mean " << pedestal_hists[i]->GetMean() << endl;
				if ( pedestal_mean[kpix_num][channel_num][bucket_num] != 0 && pedestal_hists[i]->GetFunction("gaus") != 0  )
				{
					//pedestal_hists[i]->Fit("gaus","q");
					//double test = pedestal_hists[i]->GetFunction("gaus")->GetParameter(1);
					//cout << "Parameter 0 " << pedestal_hists[i]->GetFunction("gaus")->GetParameter(0) << endl;
					//cout << "Parameter 1 " << pedestal_hists[i]->GetFunction("gaus")->GetParameter(1) << endl;
					//cout << "Parameter 2 " << pedestal_hists[i]->GetFunction("gaus")->GetParameter(2) << endl;
					pedestal_gauss[kpix_num][channel_num][bucket_num] = pedestal_hists[i]->GetFunction("gaus")->GetParameter(1);
					
					Double_t k,q;
					q = 0.5;
					pedestal_hists[i]->GetQuantiles(1, &k, &q);
					//cout << "Median " << k << endl;
					pedestal_median[kpix_num][channel_num][bucket_num] = k;
					
					
				}
				
				
				
				//cout << "KPiX Number = " << kpix_num << "    Channel Number = " << channel_num << "     Bucket Number = " << bucket_num << endl;
				//cout << "Mean " << pedestal_mean[kpix_num][channel_num][bucket_num] << endl;
				//cout << "Median " << pedestal_median[kpix_num][channel_num][bucket_num] << endl;
				//cout << "Gauss " << pedestal_gauss[kpix_num][channel_num][bucket_num] << endl;
				//cout << endl;
				
				
			}
			gROOT->GetListOfFiles()->Remove(calibration_file);
			calibration_file->Close();
			gROOT->GetListOfFiles()->Remove(pedestal_file);
			pedestal_file->Close();
			
		}
		
	
	}
	
	
	
	//////////////////////////////////////////
	// Open Data file
	//////////////////////////////////////////
	
	
	
	if ( ! dataRead.open(argv[1])  ) {
		cout << "Error opening data file " << argv[1] << endl;
		return(1);
	}
	
	// Create output names
	tmp.str("");
	tmp << argv[1] << "_external.root";
	outRoot = tmp.str();
	
	
	//////////////////////////////////////////
	// Read Data
	//////////////////////////////////////////
	cout << "Opened data file: " << argv[1] << endl;
	fileSize = dataRead.size();
	filePos  = dataRead.pos();
	
	// Init
	currPct          	= 0;
	lastPct          	= 100;
	
	cout << "\rReading File: 0 %" << flush;  // Printout of progress bar
	//goodTimes       	= 0;
	
	
	// Open root file
	rFile = new TFile(outRoot.c_str(),"recreate"); // produce root file
	rFile->cd(); // move into root folder base
	
	
	vector<double> common_modes[32];
	vector<double> common_modes2[32];
	vector<double> common_modes3[32];
	
	while ( dataRead.next(&event) ) // event read to check for filled channels and kpix to reduce number of empty histograms.
	{
		acqCount++;		
		int cycle_time_local[32][8192] = {0}; //list for each kpix that is part of improved monster finder

		if (acqCount > skip_cycles_front)
		{
			acqProcessed++;
			double CM[32] = {0};
			double CM2[32] = {0};
			double CM3[32] = {0};
			int hitcounter[32] = {0};
			for (x=0; x < event.count(); x++)
			{
		
				//// Get sample
				sample  = event.sample(x);
				kpix    = sample->getKpixAddress();
				tstamp  = sample->getSampleTime();
				channel = sample->getKpixChannel();
				bucket  = sample->getKpixBucket();
				value   = sample->getSampleValue();
				type    = sample->getSampleType();
				
				//cout << type <<endl;
				
				if ( type == KpixSample::Data )
				{
					kpixFound[kpix]          = true;
					chanFound[kpix][channel] = true;
					cycle_time_local[kpix][int(tstamp)]+=1;
					
					if (bucket == 0)
					{
						//cout << "Test" << endl;
						if (calibration_check == 1)
						{
							double charge_value = double(value)/calib_slope[kpix][channel]*pow(10,15);
							double corrected_charge_value_mean = charge_value - pedestal_mean[kpix][channel][0];
							double corrected_charge_value_median = charge_value - pedestal_median[kpix][channel][0];
							double corrected_charge_value_gauss = charge_value - pedestal_gauss[kpix][channel][0];
							
							if (calib_slope[kpix][channel]/pow(10,15) > 1 && calib_slope[kpix][channel]/pow(10,15) < 30)
							{
								CM[kpix] = CM[kpix] + corrected_charge_value_mean;
								CM2[kpix] = CM2[kpix] + corrected_charge_value_median;
								CM3[kpix] = CM3[kpix] + corrected_charge_value_gauss;
								hitcounter[kpix] = hitcounter[kpix] + 1;
								
								//cout << "cycle " << acqProcessed << endl;
								//cout << "kpix " << kpix << endl;
								//cout << "bucket " << bucket << endl;
								//cout << "ADC value " << value << endl;
								//cout << "charge value " << corrected_charge_value_mean << endl;
								//if (CM2[kpix]/hitcounter[kpix] > 2)
								//{
									//cout << "KPiX Number = " << kpix << "        cycle number = " << acqProcessed << "             channel = " << channel << "                  charge value = " << charge_value << "                 pedestal_gauss = " << pedestal_gauss[kpix][channel][0] <<  "            common mode = " <<  CM2[kpix]/hitcounter[kpix] << endl;
								//}
								
								
							}
						}
					}
					
					
				}
				//cout << "KPIX: " << kpix << endl;
				//cout << "Channel: " << channel << endl;
				kpixFound[0] = false; // for some reason the system finds a kpix in slot 0
			}
			for (int k = 0; k < 32 ; k++)
			{
				//for (int b = 0; b < 4; b++)
				//{
				//if ( k == 26 || k ==  30)
				//{
					//cout << "KPiX Number = " << k << "     Cycle Number = " << acqCount << endl;
					//cout << "CM Mean " 		<< 	CM[k]	<< endl;
					//cout << "CM Median "	<< 	CM2[k] 	<< endl;
					//cout << "CM Gauss " 	<< 	CM3[k]	<< endl;
					//cout << endl;
				//}
				common_modes[k].push_back(CM[k]/hitcounter[k]);
				common_modes2[k].push_back(CM2[k]/hitcounter[k]);
				common_modes3[k].push_back(CM3[k]/hitcounter[k]);
				//}
			}
		}
		else 
		{
			auto byte = event.count();
			auto train = event.eventNumber();
			if (f_skipped_cycles!=NULL)
			fprintf(f_skipped_cycles, " index = %d , byte = %6d, train = %6d \n ", acqCount, byte, train);
		
		}
		
	}
	
	if (f_skipped_cycles!=NULL)  {
		fclose( f_skipped_cycles);
		cout << endl;
		cout << "Wrote skipped cycles to " << outtxt << endl;
		cout << endl;
	}
	
	
	//cout << tstamp << endl;
	dataRead.close();
	double weight = 1.0/acqProcessed;
	
	//////////////////////////////////////////
	// New histogram generation within subfolder structure
	//////////////////////////////////////////
	
	

	for (kpix = 0; kpix < 32; kpix++) //looping through all possible kpix
	{
		//
		//cout << "DEBUG test " << kpixFound[kpix] << endl;
		if (kpixFound[kpix]) //checking if kpix exists
		{
			rFile->cd(); //producing subfolder for kpix same as above for the event subfolder structure
			FolderName.str("");
			FolderName << "KPiX_" << kpix;
			rFile->mkdir(FolderName.str().c_str());
			TDirectory *kpix_folder = rFile->GetDirectory(FolderName.str().c_str());
			kpix_folder->cd();
			
			
			
			tmp.str("");
			tmp << "mean_charge_k" << kpix << "_total";
			mean_charge[kpix][4] = new TH1F(tmp.str().c_str(), "mean_charge; Charge (fC);   #channels", 800,-50.5, 199.5);
			
			tmp.str("");
			tmp << "median_charge_k" << kpix << "_total";
			median_charge[kpix][4] = new TH1F(tmp.str().c_str(), "median_charge; Charge (fC);   #channels", 800,-50.5, 199.5);
			
			tmp.str("");
			tmp << "position_vs_charge_corrected_k" << kpix << "_total";
			position_vs_charge_corrected[kpix][4] = new TH2F(tmp.str().c_str(), "position vs corrected_charge; Strip position;  Charge (fC)", 230,-0.5,919.5, 20,-0.5,19.5);
			
			tmp.str("");
			tmp << "kpix_position_vs_charge_corrected_k" << kpix << "_total";
			kpix_position_vs_charge_corrected[kpix][4] = new TH2F(tmp.str().c_str(), "kpix position vs corrected_charge; KPiX position;  Charge (fC)", 256,-0.5,1023.5, 20,-0.5,19.5);
			
			tmp.str("");
			tmp << "position_vs_charge_k" << kpix << "_total";
			position_vs_charge[kpix][4] = new TH2F(tmp.str().c_str(), "position vs charge; Strip position;  Charge (fC)", 230,-0.5,919.5, 200,-0.5,199.5);
			
			tmp.str("");
			tmp << "fc_response_k" << kpix << "_total";
			fc_response[kpix][4] = new TH1F(tmp.str().c_str(), "fc_response; Charge (fC); #entries/#acq.cycles", 800,-0.5, 399.5);
			
			tmp.str("");
			tmp << "fc_response_mean_subtracted_k" << kpix << "_total";
			fc_response_mean_subtracted[kpix][4] = new TH1F(tmp.str().c_str(), "fc_response; Charge (fC); #entries/#acq.cycles", 1500,-10.5, 199.5);
			
			tmp.str("");
			tmp << "fc_response_CMmean_subtracted_k" << kpix << "_total";
			fc_response_CM_subtracted[kpix][4] = new TH1F(tmp.str().c_str(), "fc_response; Charge (fC); #entries/#acq.cycles", 1500,-10.5, 199.5);
			tmp.str("");
			tmp << "fc_response_CMmedian_subtracted_k" << kpix << "_total";
			fc_response_CMmedian_subtracted[kpix][4] = new TH1F(tmp.str().c_str(), "fc_response; Charge (fC); #entries/#acq.cycles", 1500,-10.5, 199.5);
			tmp.str("");
			tmp << "fc_response_CMgauss_subtracted_k" << kpix << "_total";
			fc_response_CMgauss_subtracted[kpix][4] = new TH1F(tmp.str().c_str(), "fc_response; Charge (fC); #entries/#acq.cycles", 1500,-10.5, 199.5);
			
			tmp.str("");
			tmp << "fc_response_median_subtracted_k" << kpix << "_total";
			fc_response_median_subtracted[kpix][4] = new TH1F(tmp.str().c_str(), "fc_response; Charge (fC); #entries/#acq.cycles", 1500,-10.5, 199.5);
			
			tmp.str("");
			tmp << "fc_response_gauss_subtracted_k" << kpix << "_total";
			fc_response_gauss_subtracted[kpix][4] = new TH1F(tmp.str().c_str(), "fc_response; Charge (fC); #entries/#acq.cycles", 1500,-10.5, 199.5);
			
			
			//tmp.str("");
			//tmp << "fc_response_timed_" << kpix << "_total";
			//fc_response_timed[kpix][4] = new TH1F(tmp.str().c_str(), "fc_response_timed; Charge (fC); #entries/#acq.cycles", 800,-0.5, 399.5);
			
			//tmp.str("");
			//tmp << "fc_response_subtracted_timed_" << kpix << "_total";
			//fc_response_subtracted_timed[kpix][4] = new TH1F(tmp.str().c_str(), "fc_response_timed; Charge (fC); #entries/#acq.cycles", 700,-50.5, 299.5);
			
			//tmp.str("");
			//tmp << "fc_response_subgroup_k" << kpix << "_total";
			//fc_response_subgroup[kpix][4] = new TH1F(tmp.str().c_str(), "fc_response; Charge (fC); #entries/#acq.cycles", 800,-0.5, 399.5);
			
			//tmp.str("");
			//tmp << "fc_response_subtracted_subgroup_k" << kpix << "_total";
			//fc_response_subtracted_subgroup[kpix][4] = new TH1F(tmp.str().c_str(), "fc_response; Charge (fC); #entries/#acq.cycles", 1500,-50.5, 199.5);
			
			
			tmp.str("");
			tmp << "common_mode_hist_k" << kpix << "_b0";
			common_mode_hist[kpix] = new TH1F(tmp.str().c_str(), "common_mode; Charge (fC);   #entries/#acq.cycles", 28,-2.5, 11.5);
			
			
			tmp.str("");
			tmp << "pedestals_mean_" << kpix << "_total";
			pedestals_mean[kpix][4] = new TH1F(tmp.str().c_str(), "pedestals; Charge (fC); #entries/#acq.cycles", 800,-0.5, 399.5);
			
			tmp.str("");
			tmp << "pedestals_median_" << kpix << "_total";
			pedestals_median[kpix][4] = new TH1F(tmp.str().c_str(), "pedestals; Charge (fC); #entries/#acq.cycles", 800,-0.5, 399.5);
			
			tmp.str("");
			tmp << "pedestals_gauss_" << kpix << "_total";
			pedestals_gauss[kpix][4] = new TH1F(tmp.str().c_str(), "pedestals; Charge (fC); #entries/#acq.cycles", 800,-0.5, 399.5);
			
			tmp.str("");
			tmp << "slopes_" << kpix << "_total";
			slopes[kpix][4] = new TH1F(tmp.str().c_str(), "slopes; Charge (fC); #entries/#acq.cycles", 250,-0.5, 50);
			
			tmp.str("");
			tmp << "slopes_vs_strip_" << kpix << "_total";
			slopes_vs_strip[kpix][4] = new TH2F(tmp.str().c_str(), "slopes_vs_strip; Strip Position; Charge (fC)",  230,-0.5,919.5, 250,-0.5, 50);
			
			FolderName.str("");
			FolderName << "cycles";
			kpix_folder->mkdir(FolderName.str().c_str());
			TDirectory *cycle_folder = kpix_folder->GetDirectory(FolderName.str().c_str());
			rFile->cd(cycle_folder->GetPath());
			for (int cycles = 0; cycles < acqProcessed; cycles++)
			{
				FolderName.str("");
				FolderName << "cycle_" << cycles;
				cycle_folder->mkdir(FolderName.str().c_str());
				TDirectory *cycles_folder = cycle_folder->GetDirectory(FolderName.str().c_str());
				cycles_folder->cd();
				
				tmp.str("");
				tmp << "fc_response_k" << kpix << "_b0_" << "cycle_" << cycles;
				fc_response_cycle.push_back(new TH1F(tmp.str().c_str(), "fc_response; Charge (fC); #entries/#acq.cycles", 800,-0.5, 399.5));
				
				tmp.str("");
				tmp << "fc_response_mean_subtracted_k" << kpix << "_b0_" << "cycle_" << cycles;
				fc_response_cycle_mean_subtracted.push_back(new TH1F(tmp.str().c_str(), "fc_response; Charge (fC); #entries/#acq.cycles", 500,-50.5, 199.5));
				
				tmp.str("");
				tmp << "fc_response_median_subtracted_k" << kpix << "_b0_" << "cycle_" << cycles;
				fc_response_cycle_median_subtracted.push_back(new TH1F(tmp.str().c_str(), "fc_response; Charge (fC); #entries/#acq.cycles", 500,-50.5, 199.5));
				
				tmp.str("");
				tmp << "fc_response_gauss_subtracted_k" << kpix << "_b0_" << "cycle_" << cycles;
				fc_response_cycle_gauss_subtracted.push_back(new TH1F(tmp.str().c_str(), "fc_response; Charge (fC); #entries/#acq.cycles", 500,-50.5, 199.5));
				
			}
			
			FolderName.str("");
			FolderName << "Buckets";
			kpix_folder->mkdir(FolderName.str().c_str());
			TDirectory *buckets_folder = kpix_folder->GetDirectory(FolderName.str().c_str());
			rFile->cd(buckets_folder->GetPath());
			
			for (bucket = 0; bucket < 4; bucket++)
			{
				FolderName.str("");
				FolderName << "Bucket " << bucket;
				buckets_folder->mkdir(FolderName.str().c_str());
				TDirectory *bucket_folder = buckets_folder->GetDirectory(FolderName.str().c_str());
				rFile->cd(bucket_folder->GetPath());
				
				tmp.str("");
				tmp << "mean_charge_k" << kpix << "_b" << bucket;
				mean_charge[kpix][bucket] = new TH1F(tmp.str().c_str(), "mean_charge; Charge (fC);   #channels", 800,-50.5, 199.5);
				
				tmp.str("");
				tmp << "median_charge_k" << kpix << "_b" << bucket;
				median_charge[kpix][bucket] = new TH1F(tmp.str().c_str(), "median_charge; Charge (fC);   #channels", 800,-50.5, 199.5);
				
				tmp.str("");
				tmp << "position_vs_charge_corrected_k" << kpix << "_b" << bucket;
				position_vs_charge_corrected[kpix][bucket] = new TH2F(tmp.str().c_str(), "position vs corrected_charge; Strip position;  Charge (fC)", 230,-0.5,919.5, 20,-0.5,19.5);
				
				tmp.str("");
				tmp << "kpix_position_vs_charge_corrected_k" << kpix << "_b" << bucket;
				kpix_position_vs_charge_corrected[kpix][bucket] = new TH2F(tmp.str().c_str(), "kpix position vs corrected_charge; KPiX position;  Charge (fC)", 256,-0.5,1023.5, 20,-0.5,19.5);
				
				tmp.str("");
				tmp << "position_vs_charge_k" << kpix << "_b" << bucket;
				position_vs_charge[kpix][bucket] = new TH2F(tmp.str().c_str(), "position vs charge; Strip position;  Charge (fC)", 230,-0.5,919.5, 200,-0.5,199.5);
				
				tmp.str("");
				tmp << "fc_response_k" << kpix << "_b" << bucket;
				fc_response[kpix][bucket] = new TH1F(tmp.str().c_str(), "fc_response; Charge (fC); #entries/#acq.cycles", 800,-0.5, 399.5);
				
				tmp.str("");
				tmp << "fc_response_mean_subtracted_k" << kpix << "_b" << bucket;
				fc_response_mean_subtracted[kpix][bucket] = new TH1F(tmp.str().c_str(), "fc_response; Charge (fC); #entries/#acq.cycles", 1500,-10.5, 199.5);
				
				tmp.str("");
				tmp << "fc_response_CM_subtracted_k" << kpix << "_b" << bucket;
				fc_response_CM_subtracted[kpix][bucket] = new TH1F(tmp.str().c_str(), "fc_response; Charge (fC); #entries/#acq.cycles", 1500,-10.5, 199.5);
				tmp.str("");
				tmp << "fc_response_CMmedian_subtracted_k" << kpix << "_b" << bucket;
				fc_response_CMmedian_subtracted[kpix][bucket] = new TH1F(tmp.str().c_str(), "fc_response; Charge (fC); #entries/#acq.cycles", 1500,-10.5, 199.5);
				tmp.str("");
				tmp << "fc_response_CMgauss_subtracted_k" << kpix << "_b" << bucket;
				fc_response_CMgauss_subtracted[kpix][bucket] = new TH1F(tmp.str().c_str(), "fc_response; Charge (fC); #entries/#acq.cycles", 1500,-10.5, 199.5);
				
				tmp.str("");
				tmp << "fc_response_median_subtracted_k" << kpix << "_b" << bucket;
				fc_response_median_subtracted[kpix][bucket] = new TH1F(tmp.str().c_str(), "fc_response; Charge (fC); #entries/#acq.cycles", 1500,-10.5, 199.5);
				
				tmp.str("");
				tmp << "fc_response_gauss_subtracted_k" << kpix << "_b" << bucket;
				fc_response_gauss_subtracted[kpix][bucket] = new TH1F(tmp.str().c_str(), "fc_response; Charge (fC); #entries/#acq.cycles", 1500,-10.5, 199.5);
				
				//tmp.str("");
				//tmp << "fc_response_timed_" << kpix << "_b" << bucket;
				//fc_response_timed[kpix][bucket] = new TH1F(tmp.str().c_str(), "fc_response_timed; Charge (fC); #entries/#acq.cycles", 800,-0.5, 399.5);
				
				//tmp.str("");
				//tmp << "fc_response_subtracted_timed_" << kpix << "_b" << bucket;
				//fc_response_subtracted_timed[kpix][bucket] = new TH1F(tmp.str().c_str(), "fc_response_timed; Charge (fC); #entries/#acq.cycles", 700,-50.5, 299.5);
				
				//tmp.str("");
				//tmp << "fc_response_subgroup_k" << kpix << "_b" << bucket;
				//fc_response_subgroup[kpix][bucket] = new TH1F(tmp.str().c_str(), "fc_response; Charge (fC); #entries/#acq.cycles", 800,-0.5, 399.5);
				
				//tmp.str("");
				//tmp << "fc_response_subtracted_subgroup_k" << kpix << "_b" << bucket;
				//fc_response_subtracted_subgroup[kpix][bucket] = new TH1F(tmp.str().c_str(), "fc_response; Charge (fC); #entries/#acq.cycles", 1500,-50.5, 199.5);
				
				
				tmp.str("");
				tmp << "pedestals_mean_" << kpix << "_b" << bucket;
				pedestals_mean[kpix][bucket] = new TH1F(tmp.str().c_str(), "pedestals; Charge (fC); #entries/#acq.cycles", 800,-0.5, 399.5);
				
				tmp.str("");
				tmp << "pedestals_median_" << kpix << "_b" << bucket;
				pedestals_median[kpix][bucket] = new TH1F(tmp.str().c_str(), "pedestals; Charge (fC); #entries/#acq.cycles", 800,-0.5, 399.5);
				
				tmp.str("");
				tmp << "pedestals_gauss_" << kpix << "_b" << bucket;
				pedestals_gauss[kpix][bucket] = new TH1F(tmp.str().c_str(), "pedestals; Charge (fC); #entries/#acq.cycles", 800,-0.5, 399.5);
				
				tmp.str("");
				tmp << "slopes_" << kpix << "_b" << bucket;
				slopes[kpix][bucket] = new TH1F(tmp.str().c_str(), "slopes; Charge (fC); #entries/#acq.cycles", 250,-0.5, 50);
				
				tmp.str("");
				tmp << "slopes_vs_strip_" << kpix << "_b" << bucket;
				slopes_vs_strip[kpix][bucket] = new TH2F(tmp.str().c_str(), "slopes_vs_strip; Strip Position; Charge (fC)",  230,-0.5,919.5, 250,-0.5, 50);
			}
			
			FolderName.str("");
			FolderName << "Strips_and_Channels";
			kpix_folder->mkdir(FolderName.str().c_str());
			TDirectory *channels_folder = kpix_folder->GetDirectory(FolderName.str().c_str());
			rFile->cd(channels_folder->GetPath());
			
			for (channel = 0; channel < 1024; channel++) //looping through all possible kpix
			{
				if (chanFound[kpix]) //checking if kpix exists
				{
					FolderName.str("");
					FolderName << "strip_" << kpix2strip_left.at(channel) << "_channel_" << channel;
					channels_folder->mkdir(FolderName.str().c_str());
					TDirectory *channel_folder = channels_folder->GetDirectory(FolderName.str().c_str());
					rFile->cd(channel_folder->GetPath());
					
					
					tmp.str("");
					tmp << "fc_response_c" << channel << setw(4) << "_k" << kpix << "_b0";
					fc_response_channel[kpix][channel] = new TH1F(tmp.str().c_str(), "fc_response; Charge (fC); #entries/#acq.cycles", 800,-0.5, 399.5);
					
					tmp.str("");
					tmp << "fc_response_mean_subtracted_c" << channel << setw(4) << "_k" << kpix << "_b0";
					fc_response_mean_subtracted_channel[kpix][channel] = new TH1F(tmp.str().c_str(), "fc_response; Charge (fC); #entries/#acq.cycles", 500,-50.5, 199.5);
					
					tmp.str("");
					tmp << "fc_response_median_subtracted_c" << channel << setw(4) << "_k" << kpix << "_b0";
					fc_response_median_subtracted_channel[kpix][channel] = new TH1F(tmp.str().c_str(), "fc_response; Charge (fC); #entries/#acq.cycles", 500,-50.5, 199.5);
					
				}
			}
		}
	}
	//////////////////////////////////////////
	// Data read for all events for detailed look
	//////////////////////////////////////////

	dataRead.open(argv[1]); //open file again to start from the beginning
	
	
	int cycle_num = 0;
	int datacounter = 0;
	
	
	cout << common_modes[26].size() << endl;
	cout << common_modes[30].size() << endl;
	while ( dataRead.next(&event) )
	{
		cycle_num++;
		
		if ( cycle_num > skip_cycles_front)
		{
			
			
			std::vector<double> time_ext;
			std::map<int, vector<pair<int, double>>> timed_spacecharge;
			
			std::vector<double> corrected_charge_vec[32][5];
			//cout << "Beginning a new EVENT" << endl;
			//cout << " NEW EVENT " << endl;
			for (x=0; x < event.count(); x++)
			{
				//cout << "DEBUG: EVENT COUNT " << event.count() << endl;
				//// Get sample
				sample  = event.sample(x);
				kpix    = sample->getKpixAddress();
				channel = sample->getKpixChannel();
				bucket  = sample->getKpixBucket();
				value   = sample->getSampleValue();
				type    = sample->getSampleType();
				tstamp  = sample->getSampleTime();
				
				
				
		
				if (type == 2)// If event is of type external timestamp
				{
					double time = tstamp + double(value * 0.125);
					time_ext.push_back(time);
					//cout << "DEBUG: channel in timestmap = " << channel << endl;
					//cout << "DEBUG: bucket in timestmap = " << bucket << endl;
				}
		
		
				if ( type == KpixSample::Data ) // If event is of type KPiX data
				{
		
					
						
					
					//cout << tstamp << endl;
					if (bucket == 0)
					{
						//cout << "Test" << endl;
						if (calibration_check == 1)
						{
							
							//cout << calib_slope[kpix][channel]/pow(10,15) << endl;
							//cout << "Test2" << endl;
							if (calib_slope[kpix][channel]/pow(10,15) > 1 && calib_slope[kpix][channel]/pow(10,15) < 15 && !(kpix == 30 && (channel == 500 || channel == 501 || channel == 490 || channel == 491 || channel == 522 || channel == 523 || channel == 532 || channel == 533 )))
							{
							//cout << "Slope " << calib_slope[kpix][channel]/pow(10,15) << endl;
							//if (calib_slope[kpix][channel]/pow(10,15) > 1 && calib_slope[kpix][channel]/pow(10,15) < 15 && kpix2strip_left.at(channel) != 9999 && pedestal[kpix][channel] < 1000 && pedestal[kpix][channel] > 100) // Filter out channels with horrible calibration and non connected channels.
							//if (calib_slope[kpix][channel]/pow(10,15) > 1 && calib_slope[kpix][channel]/pow(10,15) < 15)
							//{
								
								double charge_value = double(value)/calib_slope[kpix][channel]*pow(10,15);
								double corrected_charge_value_mean = charge_value - pedestal_mean[kpix][channel][bucket];
								double corrected_charge_value_median = charge_value - pedestal_median[kpix][channel][bucket];
								double corrected_charge_value_gauss = charge_value - pedestal_gauss[kpix][channel][bucket];
								
								double CM_corrected_charge = corrected_charge_value_mean - common_modes[kpix][datacounter];
								double CM_corrected_charge2 = corrected_charge_value_median - common_modes2[kpix][datacounter];
								double CM_corrected_charge3 = corrected_charge_value_gauss - common_modes3[kpix][datacounter];
								
								
								//cout << "Common Mode Value of cycle " << datacounter << " and KPIX " << kpix << " = " << common_modes[kpix][datacounter] << endl;
								
								
								
								common_mode_hist[kpix]->Fill(common_modes[kpix][datacounter], weight);
								
								
								//cyclical
								
								if (kpix == 26)
								{
									fc_response_cycle[datacounter]->Fill(charge_value, weight);
									fc_response_cycle_mean_subtracted[datacounter]->Fill(corrected_charge_value_mean, weight);
									fc_response_cycle_median_subtracted[datacounter]->Fill(corrected_charge_value_median, weight);
									fc_response_cycle_gauss_subtracted[datacounter]->Fill(corrected_charge_value_gauss, weight);
								}
								if (kpix == 30)
								{
									fc_response_cycle[datacounter+acqProcessed]->Fill(charge_value, weight);
									fc_response_cycle_mean_subtracted[datacounter+acqProcessed]->Fill(corrected_charge_value_mean, weight);
									fc_response_cycle_median_subtracted[datacounter+acqProcessed]->Fill(corrected_charge_value_median, weight);
									fc_response_cycle_gauss_subtracted[datacounter+acqProcessed]->Fill(corrected_charge_value_gauss, weight);
								}
								
								//total
								
								fc_response_CM_subtracted[kpix][4]->Fill(CM_corrected_charge, weight);
								fc_response[kpix][4]->Fill(charge_value, weight);
								
								fc_response_mean_subtracted[kpix][4]->Fill(corrected_charge_value_mean, weight);
								fc_response_median_subtracted[kpix][4]->Fill(corrected_charge_value_median, weight);
								fc_response_gauss_subtracted[kpix][4]->Fill(corrected_charge_value_gauss, weight);
								
								fc_response_CM_subtracted[kpix][4]->Fill(CM_corrected_charge, weight);
								fc_response_CMmedian_subtracted[kpix][4]->Fill(CM_corrected_charge2, weight);
								fc_response_CMgauss_subtracted[kpix][4]->Fill(CM_corrected_charge3, weight);
								
								
								position_vs_charge[kpix][4]->Fill(kpix2strip_left.at(channel), charge_value, 1.0);
								position_vs_charge_corrected[kpix][4]->Fill(kpix2strip_left.at(channel), corrected_charge_value_median, 1.0);
								kpix_position_vs_charge_corrected[kpix][4]->Fill(channel, corrected_charge_value_median, 1.0);
								
								
								//bucket seperated
								
								fc_response[kpix][bucket]->Fill(charge_value, weight);
								fc_response_mean_subtracted[kpix][bucket]->Fill(corrected_charge_value_mean, weight);\
								fc_response_median_subtracted[kpix][bucket]->Fill(corrected_charge_value_median, weight);
								fc_response_gauss_subtracted[kpix][bucket]->Fill(corrected_charge_value_gauss, weight);
								
								fc_response_CM_subtracted[kpix][bucket]->Fill(CM_corrected_charge, weight);
								fc_response_CMmedian_subtracted[kpix][bucket]->Fill(CM_corrected_charge2, weight);
								fc_response_CMgauss_subtracted[kpix][bucket]->Fill(CM_corrected_charge3, weight);
								
								
								//channel seperated
								fc_response_channel[kpix][channel]->Fill(charge_value, weight);
								fc_response_mean_subtracted_channel[kpix][channel]->Fill(corrected_charge_value_mean, weight);
								fc_response_median_subtracted_channel[kpix][channel]->Fill(corrected_charge_value_median, weight);
								
								
								position_vs_charge[kpix][bucket]->Fill(kpix2strip_left.at(channel), charge_value, 1.0);
								position_vs_charge_corrected[kpix][bucket]->Fill(kpix2strip_left.at(channel), corrected_charge_value_median, 1.0);
								kpix_position_vs_charge_corrected[kpix][bucket]->Fill(channel, corrected_charge_value_median, 1.0);
								
								//if (kpix2strip_left.at(channel) < 700 && kpix2strip_left.at(channel) > 400)
								//{
									//fc_response_subgroup[kpix][4]->Fill(charge_value, weight);
									//fc_response_subtracted_subgroup[kpix][4]->Fill(corrected_charge_value_median, weight);
									
									//fc_response_subgroup[kpix][bucket]->Fill(charge_value, weight);
									//fc_response_subtracted_subgroup[kpix][bucket]->Fill(corrected_charge_value_median, weight);
								//}
								
								//cout << "Charge Value of Channel " << channel << " = " << charge_value << endl;
								//cout << "Pedestal of Channel " << channel << " = " << pedestal[kpix][channel] << endl;
								//cout << "ADC Value of Channel " << channel << " = " << value << endl;
							//}
							}
							
							
						}
					}

					
					//double trig_diff = smallest_time_diff(time_ext, tstamp); //Calculation of minimal difference is done in a function for cleanup
					
					
				}
				

			}	
			//cout << corrected_charge_vec.size() << endl;
			//for (int k = 0; k < 32; ++k)
			//{
				//for (int i =0; i < corrected_charge_vec[k].size(); ++i) //common mode subtraction
				//{
					//double CM_corrected_charge = corrected_charge_vec[k].at(i) - (common_mode[k]/hitcounter[k]);
					////cout << "CM_corrected_charge = " << CM_corrected_charge << endl;
					////cout << "corrected_charge = " << corrected_charge_vec[k].at(i) << endl;
					////cout << "common mode = " << common_mode[k] << endl;
					////cout << "hitcounter = " << (common_mode[k]/hitcounter[k]) << endl;
					//fc_response_CM_subtracted[k]->Fill(CM_corrected_charge, weight);
				//}
			//}
			datacounter++;
			
		}	
		////   Show progress
		filePos  = dataRead.pos();
		currPct = (uint)(((double)filePos / (double)fileSize) * 100.0);
		if ( currPct != lastPct ) 
		{
			cout << "\rReading File: " << currPct << " %      " << flush;
			lastPct = currPct;
		}
		
		
	}
	for (int kpix = 0 ; kpix < 32; kpix++)
	{
		if (kpixFound[kpix] == true)
		{
			for (int channel = 0; channel < 1024 ; channel++)
			{
				pedestals_mean[kpix][bucket]->Fill(pedestal_mean[kpix][channel][bucket]);
				pedestals_median[kpix][bucket]->Fill(pedestal_median[kpix][channel][bucket]);
				pedestals_gauss[kpix][bucket]->Fill(pedestal_gauss[kpix][channel][bucket]);
				
				slopes[kpix][bucket]->Fill(calib_slope[kpix][channel]/pow(10,15));
				slopes_vs_strip[kpix][bucket]->Fill(kpix2strip_left.at(channel), calib_slope[kpix][channel]/pow(10,15), 1.0);
				
				slopes[kpix][4]->Fill(calib_slope[kpix][channel]/pow(10,15));
				slopes_vs_strip[kpix][4]->Fill(kpix2strip_left.at(channel), calib_slope[kpix][channel]/pow(10,15), 1.0);
				
				
				double mean = fc_response_median_subtracted_channel[kpix][channel]->GetMean();
				Double_t median,q;
				q = 0.5;
				fc_response_median_subtracted_channel[kpix][channel]->GetQuantiles(1, &median, &q);
				mean_charge[kpix][bucket]->Fill(mean);
				median_charge[kpix][bucket]->Fill(median);
				
			}
		}
	}
	
	//cout <<  endl << "Full coincidence of sensors with external trigger: " << full_coincidence_channel_entries->GetEntries() << endl;
	//cout << "Three coincidence of sensors: " << three_coincidence << endl;
	//cout << "Two coincidence of sensors: " << two_coincidence << endl;
	
	//cout << endl <<  "An event is currently classified as a monster if the amount of triggers one acquisition clock within a cycle is above " << monster_finder_limit << endl;
	//cout << "_______________________________________________________________" << endl;
	

	
	cout << endl;
	cout << "Writing root plots to " << outRoot << endl;
	cout << endl;
	
	rFile->Write();
	gROOT->GetListOfFiles()->Remove(rFile); //delete cross links that make production of subfolder structure take forever
	rFile->Close();
	
	
	
	dataRead.close();
	return(0);
}
