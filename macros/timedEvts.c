
void timedEvts(){

  TH1::AddDirectory(kFALSE);
  //gStyle->SetOptStat(0);
  //gROOT->ForceStyle();

  TString fn  = "/scratch/data/tracker_test/2018_08_17_16_07_45.bin_0.root";
  TString gdir= "KPiX_26/Strips_and_Channels/strip_490_channel_395/bucket_0/";
  
  TString hnall   = gdir + "hist_s490_c0395_b0_k26";
  TString hntimed = gdir + "hist_timed_s490_c0395_b0_k26_time_cut";
  
  TFile *file = TFile::Open(fn.Data());

  TH1F* hall = nullptr;
  TH1F* htimed = nullptr;
  
  file->GetObject(hnall, hall);
  file->GetObject(hntimed, htimed);

  if (hall==nullptr || htimed==nullptr) return;
  
  TH1F* hped = (TH1F*)hall->Clone();
  hped->Add(htimed, -1);
  hped->SetMarkerColor(kBlue);
  hped->SetLineColor(kBlue);

  // fit crystal ball:
  /* TF1 *fit = new TF1("fit","crystalball" ); */
  /* fit->SetParameters(1,2,1,1,0); // arbitrary initial -> must not work.. otherwise lucky! */
  /* hped->Fit("fit", "q", "", 140, 200); */

    
  hall -> SetMarkerColor(kBlack);
  hall -> SetLineColor(kBlack);

  htimed -> SetMarkerColor(kRed);
  htimed -> SetLineColor(kRed);

  //hall->Draw("hpe");
  //htimed->Draw("hepsame");
  hped->Draw("hep");
  
  hped->GetXaxis()->SetRangeUser(140, 240);
  //hall->GetYaxis()->SetRangeUser();
  //hall->GetYaxis()->SetTitle("");
  
}
