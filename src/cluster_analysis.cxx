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

		

struct spacecharge
{
	vector<int> position;
	vector<double> charge;
};


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
			size_t found_object = keyname.find(histname);
			size_t found_type = keytype.find("TH1");
			if (found_type != 0)
			{
				
				if (found_object == 0) 
				{
					TGraphErrors *calib_graph = (TGraphErrors*)key->ReadObj();
					calib_graph->SetName(key->GetName());
					calib_graphs.push_back(calib_graph);
				}
			}
			else
			{
				if (found_object == 0) 
				{
					TH1F *pedestal_hist = (TH1F*)key->ReadObj();
					pedestal_hist->SetName(key->GetName());
					pedestal_hists.push_back(pedestal_hist);
				}
			}
		}
	}
}



// Coincidence function void coincidence(int* time_list1, int* time_list2, int* time_list3, int* channel_list1, int* channel_list2, int* channel_list3)


void addDoubleToXml ( ofstream *xml, uint indent, string variable, Double_t value ) { //xml output function
	uint x;
	
	if ( !(value != value) ) {
		for (x=0; x < indent; x++) *xml << " ";
		*xml << "<" << variable << ">";
		*xml << value;
		*xml << "</" << variable << ">";
		*xml << endl;
	}
}

void addStringToXml ( ofstream *xml, uint indent, string variable, string value ) { //xml output function
	uint x;
	
	for (x=0; x < indent; x++) *xml << " ";
	*xml << "<" << variable << ">";
	*xml << value;
	*xml << "</" << variable << ">";
	*xml << endl;
}

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
	
	cluster chargecluster;
	
	int cluster_size = 4;
	int clusters = 1024/cluster_size;
	
	TH1F 					*left_strip_entries[32];
	TH1F                   	*hist[32][1024];
	TH1F                   	*hist_cluster_position; 
	TH1F                   	*hist_cluster_charge; 
	TH1F 					*cluster_entries[32];
	
	TH1F 					*left_strip_entries_map;
	
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
	double					pedestal[32][1024] = {1};
	int						calibration_check = 0;
	
	
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
	if ( argc != 2 && argc!= 3) {
	cout << "Usage: ./analysis data_file [skip_cycles_front (int)]||[read calibration input file (char)] \n";
	return(1);
	}
	
	char* end;
	// skip first few cycles or read in calibration data file:
	if ( argc == 3 ) {
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
			loopdir(calibration_file, "calib_");
			loopdir(calibration_file, "hist_");
			for (unsigned int i = 0; i<calib_graphs.size(); ++i)
			{
				//cout << "Current key1 = " << cal_key->GetClassName() << endl;
				
				string calib_name         = calib_graphs[i]->GetName();
				
				size_t kpix_num_start     = calib_name.find("_k")+2;
				size_t channel_num_start  = calib_name.find("_c")+2;
				size_t kpix_num_length       = calib_name.length() - kpix_num_start;
				size_t channel_num_length    = calib_name.find("_b") - channel_num_start;
				
			    string channel_string = calib_name.substr(channel_num_start, channel_num_length);
			    string kpix_string = calib_name.substr(kpix_num_start, kpix_num_length);
			    
			    int kpix_num = stoi(kpix_string);
			    int channel_num = stoi(channel_string);
				
				//cout << "KPiX Number = " << kpix << endl;
				//cout << "Channel Number = " << channel << endl;
				
				calib_slope[kpix_num][channel_num] = calib_graphs[i]->GetFunction("pol1")->GetParameter(1);
				//cout << "Slope of KPiX " << kpix << " and channel " << channel << " is " <<  calib_slope[kpix][channel] << endl;
				
			}
			
			for (unsigned int i = 0; i<pedestal_hists.size(); ++i)
			{
				//cout << "Current key1 = " << cal_key->GetClassName() << endl;
				
				string pedestal_name         = pedestal_hists[i]->GetName();
				
				size_t kpix_num_start     = pedestal_name.find("_k")+2;
				size_t channel_num_start  = pedestal_name.find("_c")+2;
				size_t kpix_num_length       = pedestal_name.length() - kpix_num_start;
				size_t channel_num_length    = pedestal_name.find("_b") - channel_num_start;
				
			    string channel_string = pedestal_name.substr(channel_num_start, channel_num_length);
			    string kpix_string = pedestal_name.substr(kpix_num_start, kpix_num_length);
			    
			    //cout << "Channel Number = " <<  channel_string << endl;
			   // cout << "KPiX Number = " << kpix_string << endl;
			    
			    int kpix_num = stoi(kpix_string);
			    int channel_num = stoi(channel_string);
				
				//cout << "KPiX Number = " << kpix_num << endl;
				//cout << "Channel Number = " << channel_num << endl;
				//cout << "Parameter 0 " << pedestal_hists[i]->GetFunction("gaus")->GetParameter(0) << endl;
				//cout << "Parameter 1 " << pedestal_hists[i]->GetFunction("gaus")->GetParameter(1) << endl;
				//cout << "Parameter 2 " << pedestal_hists[i]->GetFunction("gaus")->GetParameter(2) << endl;
				pedestal[kpix_num][channel_num] = pedestal_hists[i]->GetFunction("gaus")->GetParameter(1);
				//cout << "Slope of KPiX " << kpix << " and channel " << channel << " is " <<  calib_slope[kpix][channel] << endl;
				
			}
			
			
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
	tmp << argv[1] << "_cluster.root";
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
	
	
	//////////////////////////////////////////
	// Single unique histogram generation
	//////////////////////////////////////////
	
	TH2F *cluster_position_vs_charge = new TH2F ("cluster position vs charge", "Cluster position vs charge; Strip position;  Charge (fC)", 410,-0.5,919.5, 20,-0.5,299.5);
	TH2F *position_vs_charge = new TH2F ("position vs charge", "position vs charge; Strip position;  Charge (fC)", 230,-0.5,919.5, 160,-0.5,159.5);
	TH2F *position_vs_corrcharge = new TH2F ("position vs corrected charge", "position vs corrected charge; Strip position;  Charge (fC)", 230,-0.5,919.5, 40,-0.5,39.5);
	TH1F *charge_sum_position = new TH1F ("charge_sum_position" , "charge_sum_position; Strip position; Charge Sum (fC)", 410, -0.5, 919.5);
	
	while ( dataRead.next(&event) ) // event read to check for filled channels and kpix to reduce number of empty histograms.
	{
		acqCount++;		
		int cycle_time_local[32][8192] = {0}; //list for each kpix that is part of improved monster finder

		if (acqCount > skip_cycles_front)
		{
			acqProcessed++;
			for (x=0; x < event.count(); x++)
			{
		
				//// Get sample
				sample  = event.sample(x);
				kpix    = sample->getKpixAddress();
				tstamp  = sample->getSampleTime();
				channel = sample->getKpixChannel();
				bucket  = sample->getKpixBucket();
				type    = sample->getSampleType();
				if ( type == KpixSample::Data )
				{
					kpixFound[kpix]          = true;
					chanFound[kpix][channel] = true;
					cycle_time_local[kpix][int(tstamp)]+=1;
					
				}
				//cout << "KPIX: " << kpix << endl;
				//cout << "Channel: " << channel << endl;
				kpixFound[0] = false; // for some reason the system finds a kpix in slot 0
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
	
	
	dataRead.close();
	double weight = 1.0;//acqProcessed;
	
	//////////////////////////////////////////
	// New histogram generation within subfolder structure
	//////////////////////////////////////////

	tmp.str("");
	tmp << "Left_Strip_entries_map_k_total";
	left_strip_entries_map = new TH1F(tmp.str().c_str(), "Strip_Entries; Strip_address; #entries/#acq.cycles", 920,-0.5, 919.5);

	hist_cluster_charge = new TH1F("hist_fc_cluster","hist_fc_cluster; Clustered Charge (fC); #entries/#acq.cycles",2730, -0.5,545.5);
	hist_cluster_position = new TH1F("hist_cluster","hist_cluster; Cluster Position (Strip Address); #entries/#acq.cycles",920, -0.5,919.5);
	
	

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
			tmp << "Left_Strip_entries_k_" << kpix << "_total";
			left_strip_entries[kpix] = new TH1F(tmp.str().c_str(), "Strip_Entries; Strip_address; #entries/#acq.cycles", 920,-0.5, 919.5);
			
			
			
			tmp.str("");
			tmp << "Cluster_entries_k_" << kpix << "_total";
			cluster_entries[kpix] = new TH1F(tmp.str().c_str(), "Cluster_Entries; Cluster_address; #entries/#acq.cycles", 256,-0.5, 255);
			
			FolderName.str("");
			FolderName << "Strips_and_Channels";
			kpix_folder->mkdir(FolderName.str().c_str());
			TDirectory *channels_folder = kpix_folder->GetDirectory(FolderName.str().c_str());
			rFile->cd(channels_folder->GetPath());
			
			for (channel = 0; channel < 1024; channel++)
			{
				if (chanFound[kpix][channel])
				{
					
					
					tmp.str("");  //set stringstream tmp to an empty string	
					tmp << "hist_fc" << "_s" << dec <<  kpix2strip_left.at(channel);
					tmp << "_c" << dec << setw(4) << setfill('0') << channel;
					tmp << "_k" << dec << kpix; // add _k$kpix to stringstream
					
					tmp_units.str(""); //set stringstream decribing histogram units to an empty string
					tmp_units << "hist_fc" << "_s" << dec <<  kpix2strip_left.at(channel);
					tmp_units << "_c" << dec << setw(4) << setfill('0') << channel;
					tmp_units << "_k" << dec << kpix; // add _k$kpix to stringstream
					tmp_units << "; Charge (fC); #entries/#acq.cycles"; // add title: x label, y label to stringstream
					hist[kpix][channel] = new TH1F(tmp.str().c_str(),tmp_units.str().c_str(),2730, -0.5,545.5);	
					
					
				}
			}
			//FolderName.str("");
			//FolderName << "Clusters";
			//kpix_folder->mkdir(FolderName.str().c_str());
			//TDirectory *cluster_folder = kpix_folder->GetDirectory(FolderName.str().c_str());
			//rFile->cd(cluster_folder->GetPath());

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
			//float time_corr_clustered_charge[32][256];
			//std::map<int, map<int, double>> correlated_charge;
			std::map<int, spacecharge> correlated_charge;
			//std::vector<int> Assignment_number;
			
			
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
		
					
						
					int cluster = kpix2strip_left.at(channel)/cluster_size;
					//cout << tstamp << endl;
					if (bucket == 0)
					{
						left_strip_entries[kpix]->Fill(kpix2strip_left.at(channel), weight);
						cluster_entries[kpix]->Fill(cluster, weight);
						
						int timestamp = tstamp;
						
						if (calibration_check == 1)
						{
							
							//cout << calib_slope[kpix][channel]/pow(10,15) << endl;
							
							
							
							if (calib_slope[kpix][channel]/pow(10,15) > 1 && calib_slope[kpix][channel]/pow(10,15) < 15 && kpix2strip_left.at(channel) != 9999 && pedestal[kpix][channel] < 1000 && pedestal[kpix][channel] > 100) // Filter out channels with horrible calibration and non connected channels.
							{
								double charge_value = double(value)/calib_slope[kpix][channel]*pow(10,15);
								double corrected_charge_value = charge_value - ( pedestal[kpix][channel] /calib_slope[kpix][channel]*pow(10,15) );
								//cout << "Charge value : " << charge_value << endl;
								//cout << "Corrected charge value : " << corrected_charge_value << endl;
								//cout << "Pedestal : " << pedestal[kpix][channel]/(calib_slope[kpix][channel]/pow(10,15)) << endl;
								
								hist[kpix][channel]->Fill(charge_value, weight);
								
								if (kpix == 26)
								{
									//correlated_charge[timestamp][kpix2strip_left.at(channel)] = charge_value;
									correlated_charge[timestamp].position.push_back(kpix2strip_left.at(channel));
									correlated_charge[timestamp].charge.push_back(charge_value);
									position_vs_charge->Fill(kpix2strip_left.at(channel), charge_value, weight);
									position_vs_corrcharge->Fill(kpix2strip_left.at(channel), corrected_charge_value, weight);
									
									charge_sum_position->Fill(kpix2strip_left.at(channel), charge_value);
									
								}
							}
							
							
							
						}
					}

					
					//double trig_diff = smallest_time_diff(time_ext, tstamp); //Calculation of minimal difference is done in a function for cleanup
					
					
				}

			}
			//cout << " Next Event " << endl;
			for (auto const& cor_charge : correlated_charge )
			{
				
				if (cor_charge.second.position.size() > 1 )
				{
					//cout << "Time Coordinate " << cor_charge.first << endl; 
					for ( int i = 0 ; i < cor_charge.second.position.size(); i++)
					{
						//cout << "Spatial Coordinate " << cor_charge.second.position.at(i) << endl;
						//cout << "Charge " << cor_charge.second.charge.at(i) << endl;
						
					}
					
					
				}
				chargecluster.construct(cor_charge.second.position, cor_charge.second.charge);
				
				hist_cluster_charge->Fill(chargecluster.charge, weight);
				hist_cluster_position->Fill(chargecluster.position, weight);
				cluster_position_vs_charge->Fill(chargecluster.position, chargecluster.charge, weight);
				
				//cout << "Cluster Position " << chargecluster.position << endl;
				//cout << "Cluster Charge " << chargecluster.charge << endl;
				
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
