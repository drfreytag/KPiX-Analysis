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
vector<TH1F*> TH1_histograms;
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
			TH1F *TH1_hist = (TH1F*)key->ReadObj();
			TH1_hist->SetName(key->GetName());
			TH1_histograms.push_back(calib_graph);
		}
	}
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
	
	
	TH1F				*mean_charge[32];

	
	// Stringstream initialization for histogram naming
	stringstream           tmp;
	stringstream           tmp_units;
	
	// Stringstream initialization for folder naming
	
	stringstream			FolderName;
	
	
	string                 outRoot;
	TFile                  *rFile;
	
	
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
	if ( argc != 2) {
	cout << "Usage: ./analysis data_file \n";
	return(1);
	}
	
	char* end;
	
	//////////////////////////////////////////
	// Open Data file
	//////////////////////////////////////////
	
	
	
	if ( ! dataRead.open(argv[1])  ) {
		cout << "Error opening data file " << argv[1] << endl;
		return(1);
	}
	
	// Create output names
	tmp.str("");
	tmp << argv[1];
	size_t extension     = tmp.find(".root");
	size_t filename_len       = tmp.length() - extension;
	tmp = tmp.substr(extension, filename_len);
	tmp << "_step2.root";
	outRoot = tmp.str();
	
	
	//////////////////////////////////////////
	// Read Data
	//////////////////////////////////////////
	cout << "Opened data file: " << argv[1] << endl;
	
	
	// Open root file
	rFile = new TFile(outRoot.c_str(),"recreate"); // produce root file
	rFile->cd(); // move into root folder base
	
	
	//////////////////////////////////////////
	// Single unique histogram generation
	//////////////////////////////////////////
	
	
	
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
			tmp << "mean_charge_k" << kpix << "_b0";
			mean_charge[kpix] = new TH2F(tmp.str().c_str(), "mean_charge; Charge (fC);   #entries/#acq.cycles", 460,-0.5,919.5, 20,-0.5,19.5);
			
			tmp.str("");
			tmp << "position_vs_charge_k" << kpix << "_b0";
			position_vs_charge[kpix] = new TH2F(tmp.str().c_str(), "position vs charge; Strip position;  Charge (fC)", 460,-0.5,919.5, 200,-0.5,199.5);
			
			tmp.str("");
			tmp << "fc_response_k" << kpix << "_b0";
			fc_response[kpix] = new TH1F(tmp.str().c_str(), "fc_response; Charge (fC); #entries/#acq.cycles", 800,-0.5, 399.5);
			
			tmp.str("");
			tmp << "fc_response_subtracted_k" << kpix << "_b0";
			fc_response_subtracted[kpix] = new TH1F(tmp.str().c_str(), "fc_response; Charge (fC); #entries/#acq.cycles", 1500,-50.5, 199.5);
			
			
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
					tmp << "fc_response_subtracted_c" << channel << setw(4) << "_k" << kpix << "_b0";
					fc_response_subtracted_channel[kpix][channel] = new TH1F(tmp.str().c_str(), "fc_response; Charge (fC); #entries/#acq.cycles", 1500,-50.5, 199.5);
				}
			}
		}
	}
	
	//////////////////////////////////////////
	// Data read for all events for detailed look into single event structure
	//////////////////////////////////////////

	dataRead.open(argv[1]); //open file again to start from the beginning
	
	
	int cycle_num = 0;
	
	
	while ( dataRead.next(&event) )
	{
		cycle_num++;
		if ( cycle_num > skip_cycles_front)
		{
			
			
			std::vector<double> time_ext;
			std::map<int, vector<pair<int, double>>> timed_spacecharge;
			
			
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
							
							//cout << "Slope " << calib_slope[kpix][channel]/pow(10,15) << endl;
							//if (calib_slope[kpix][channel]/pow(10,15) > 1 && calib_slope[kpix][channel]/pow(10,15) < 15 && kpix2strip_left.at(channel) != 9999 && pedestal[kpix][channel] < 1000 && pedestal[kpix][channel] > 100) // Filter out channels with horrible calibration and non connected channels.
							//if (calib_slope[kpix][channel]/pow(10,15) > 1 && calib_slope[kpix][channel]/pow(10,15) < 15)
							//{
								double charge_value = double(value)/calib_slope[kpix][channel]*pow(10,15);
								double corrected_charge_value = charge_value - pedestal[kpix][channel];
								
								fc_response[kpix]->Fill(charge_value, weight);
								fc_response_subtracted[kpix]->Fill(corrected_charge_value, weight);
								
								fc_response_channel[kpix][channel]->Fill(charge_value, weight);
								fc_response_subtracted_channel[kpix][channel]->Fill(corrected_charge_value, weight);
								
								position_vs_charge[kpix]->Fill(kpix2strip_left.at(channel), charge_value, 1.0);
								position_vs_charge_corrected[kpix]->Fill(kpix2strip_left.at(channel), corrected_charge_value, 1.0);
								
								if (kpix2strip_left.at(channel) < 700 && kpix2strip_left.at(channel) > 400)
								{
									fc_response_subgroup[kpix]->Fill(charge_value, weight);
									fc_response_subtracted_subgroup[kpix]->Fill(corrected_charge_value, weight);
								}
								
								//cout << "Charge Value of Channel " << channel << " = " << charge_value << endl;
								//cout << "Pedestal of Channel " << channel << " = " << pedestal[kpix][channel] << endl;
								//cout << "ADC Value of Channel " << channel << " = " << value << endl;
							//}
							
							
							
						}
					}

					
					//double trig_diff = smallest_time_diff(time_ext, tstamp); //Calculation of minimal difference is done in a function for cleanup
					
					
				}

			}	
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
				pedestals[kpix]->Fill(pedestal[kpix][channel], weight);
				slopes[kpix]->Fill(calib_slope[kpix][channel]/pow(10,15), weight);
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
