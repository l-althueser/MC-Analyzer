/******************************************************************
 * plot_LCE_2D.C
 * 
 * @author	Lutz Althueser
 * @date   	2015-04-01
 * @updated	2016-02-27
 *
 * @comment 
 *
 * @changes	 
 *
 ******************************************************************/
 
#include <iostream>
#include <stdio.h>
#include <string>
using namespace std;

/*=================================================================*/

void LCE_Map(){
	cout << "Please insert a datafile as parameter." << endl;
}

/*=================================================================*/

void LCE_Map(string datafile){
	LCE_Map(datafile, 40, 10, 40);
}

/*=================================================================*/

void LCE_Map(string datafile, const int NbRBin, const int NbTBin, const int NbZBin) {
	size_t found=datafile.find_last_of("/\\");
	string workingdirectory = datafile.substr(0,found);
	string datafilename = datafile.substr(found+1);
	TString Tdatafile = datafile;
	
	//////////////////////////////////////////////////////////////////

	// recommended values: NbRBin = 40; NbTBin = 10; NbZBin = 40;
	const int NbVolumes = NbRBin*NbTBin*NbZBin; 

	cout << "R is divided in " << NbRBin << " bins" << endl;
	cout << "Theta is divided in " << NbTBin << " bins" << endl;
	cout << "Z is divided in " << NbZBin << " bins" << endl;
	cout << "The Detector is divided into " << NbVolumes << " volumes" << endl;

	float ly[NbZBin][NbTBin][NbRBin]={0};
	float LCE[NbZBin][NbTBin][NbRBin]={0};
	float LCEabs[NbZBin][NbTBin][NbRBin]={0};
	float LCEzr[NbZBin][NbRBin]={0};
	float LCEabszr[NbZBin][NbRBin]={0};
	float lyzr[NbZBin][NbRBin]={0};

	//////////////////////////////////////////////////////////////////

	TFile *rootfile = new TFile(Tdatafile);
	TTree *T1 = (TTree*)rootfile->Get("t1");
	
	if ( rootfile->GetListOfKeys()->Contains("nbeventstosimulate") == 1 ) {
		TParameter<int> *NbEventsToSimulateParameter = rootfile->Get("nbeventstosimulate");
		const int NbSimuInitial = NbEventsToSimulateParameter->GetVal();
	}
	else {
		const int NbSimuInitial = 100000000;
	}
	long nbentries = T1->GetEntries();
	
	delete rootfile;
	
	found=datafilename.find_last_of(".");
	string outputfilename = datafilename.substr(0,found);
	const char rootfileout[10000];
	sprintf(rootfileout,"%s/%s_plot.root", workingdirectory.c_str(), outputfilename.c_str());
	TFile *f_plot = TFile::Open(rootfileout,"UPDATE");
	if (!f_plot) { return; }
	
	// read txtfile
	char filename[10000];
	sprintf(filename,"%s/%s_LCE-LY_R%d-T%d-Z%d.dat", workingdirectory.c_str(), datafilename.c_str(), NbRBin, NbTBin, NbZBin);

	ifstream txtfile;
	txtfile.open(filename);
 
	int zi=0;
	int ti=0;
	int ri=0;

	float av=0;
	float avlyop=0;
	float avlykr=0;

	float d=0;

	char dum[100];
	txtfile.getline(dum,100);

	for (int i=0; i<=NbVolumes; i++){
		txtfile >> zi >> ti >> ri >> d >> d >> d >> ly[zi][ti][ri] >> LCEabs[zi][ti][ri] >> LCE[zi][ti][ri] >> d;
	}

	//////////////////////////////////////////////////////////////////

	for (int z=0; z<NbZBin; z++){	
		for (int r=0; r<NbRBin ; r++){
			for (int t=0; t<NbTBin; t++){
				LCEzr[z][r]+=LCE[z][t][r];
				LCEabszr[z][r]+=LCEabs[z][t][r];
				lyzr[z][r]+=ly[z][t][r];	
			}
			LCEzr[z][r]=LCEzr[z][r]/NbTBin;
			lyzr[z][r]=lyzr[z][r]/NbTBin;
			av += LCEzr[z][r]/(NbZBin*NbRBin);
			avlyop += lyzr[z][r]/(NbZBin*NbRBin);
			avlykr += ((LCEabszr[z][r]/(41.55*(NbSimuInitial/(NbRBin*NbZBin))))*0.3)/(NbZBin*NbRBin);
		}
	}

	gStyle->SetPadLeftMargin(0.105);
	gStyle->SetPadRightMargin(0.16);
  gStyle->SetPadTopMargin(0.06);
	gStyle->SetPadBottomMargin(0.11);

	sprintf(filename,"%s_LCE_R%d-T%d-Z%d", datafilename.c_str(), NbRBin, NbTBin, NbZBin);
	TCanvas *c1 = new TCanvas(filename,filename);

	TStyle *myStyle = new TStyle("myStyle","my style");
	myStyle->SetCanvasColor(10);
	myStyle->SetTitleFillColor(0);
	myStyle->cd();

  const Int_t NRGBs = 5;
  const Int_t NCont = 20;

  Double_t stops[NRGBs] = { 0.00, 0.34, 0.61, 0.84, 1.00 };
  Double_t red[NRGBs]   = { 0.00, 0.00, 0.87, 1.00, 0.51 };
  Double_t green[NRGBs] = { 0.00, 0.81, 1.00, 0.20, 0.00 };
  Double_t blue[NRGBs]  = { 0.51, 1.00, 0.12, 0.00, 0.00 };

	TColor::CreateGradientColorTable(NRGBs, stops, red, green, blue, NCont);
  gStyle->SetNumberContours(NCont);

	TH2F *h1 = new TH2F("LCE",filename,NbRBin,0,1600,NbZBin,-169,-2);
	for (int z=0; z<(NbZBin); z++){
		for (int r=0; r<NbRBin ; r++){
				h1->SetBinContent(r+1,NbZBin-z,LCEabszr[z][r]);
		}
	}

	Float_t *bins = h1->GetArray();
	int binmax = h1->GetBinContent(h1->GetMaximumBin());
	
	h1->Draw("COLZ");
  h1->GetZaxis()->SetTitle("LCE [pmthits]");
  h1->GetZaxis()->SetTitleOffset(1.15);
	h1->GetYaxis()->SetTitle("z [mm]");
	h1->GetXaxis()->SetTitle("r^{2} [mm^{2}]");
	
	h1->GetXaxis()->SetLabelSize(0.05);
  h1->GetYaxis()->SetLabelSize(0.05);
  h1->GetZaxis()->SetLabelSize(0.05);
  h1->GetXaxis()->SetTitleSize(0.05);
  h1->GetYaxis()->SetTitleSize(0.05);
  h1->GetZaxis()->SetTitleSize(0.05);
 
  // hide title and use the free space
  gStyle->SetOptTitle(0);

	//h1->SetMaximum(1);
  //h1->SetMinimum(0.85);

	// you can hide the stats
	gStyle->SetOptStat(0);
	gStyle->SetStatY(0.9);
	gStyle->SetStatX(0.9);
	gStyle->SetStatW(0.125);
	gStyle->SetStatH(0.125); 
	
	sprintf(filename,"%s/%s_LCE_R%d-T%d-Z%d_abs.pdf", workingdirectory.c_str(), datafilename.c_str(), NbRBin, NbTBin, NbZBin);
	//c1->SaveAs(filename);
	//c1->Write();

	gStyle->SetOptStat("neim");
	sprintf(filename,"%s/%s_LCE_R%d-T%d-Z%d_abs_stat.pdf", workingdirectory.c_str(), datafilename.c_str(), NbRBin, NbTBin, NbZBin);
	//c1->SaveAs(filename);
	//c1->Write();
	
	if (datafilename.find("83mKr") != std::string::npos) {
		//ly for Kr-83m
		gStyle->SetOptStat(0);
		for (int z=0; z<(NbZBin); z++){
			for (int r=0; r<NbRBin ; r++){
				h1->SetBinContent(r+1,NbZBin-z,(LCEabszr[z][r]/(41.55*(NbSimuInitial/(NbRBin*NbZBin))))*0.3);
			}
		}
		h1->GetZaxis()->SetTitle("LY [pe/keV]");
	
		sprintf(filename,"%s/%s_LCE_R%d-T%d-Z%d_ly-83mKr.pdf", workingdirectory.c_str(), datafilename.c_str(), NbRBin, NbTBin, NbZBin);
		//c1->SaveAs(filename);
		c1->Write();
		
		cout << "Average LYKr: " << avlykr << endl;
	}
	else {
		//ly for op
		gStyle->SetOptStat(0);
		for (int z=0; z<(NbZBin); z++){
			for (int r=0; r<NbRBin ; r++){
				h1->SetBinContent(r+1,NbZBin-z,lyzr[z][r]);
			}
		}
		h1->GetZaxis()->SetTitle("LY [pe/keV]");
	
		sprintf(filename,"%s/%s_LCE_R%d-T%d-Z%d_ly.pdf", workingdirectory.c_str(), datafilename.c_str(), NbRBin, NbTBin, NbZBin);
		//c1->SaveAs(filename);
		c1->Write();
		
		cout << "Average LYOP: " << avlyop << endl;
	}

	gStyle->SetOptStat(0);
	for (Int_t i=0;i<h1->GetSize();i++) if (bins[i] != 0) bins[i] = bins[i]/binmax*100;
	h1->GetZaxis()->SetTitle("LCE [pmthits pct]");
	
	sprintf(filename,"%s/%s_LCE_R%d-T%d-Z%d_pct.pdf", workingdirectory.c_str(), datafilename.c_str(), NbRBin, NbTBin, NbZBin);
	//c1->SaveAs(filename);
	//c1->Write();
	
	gStyle->SetOptStat("neim");
	sprintf(filename,"%s/%s_LCE_R%d-T%d-Z%d_pct_stat.pdf", workingdirectory.c_str(), datafilename.c_str(), NbRBin, NbTBin, NbZBin);
	//c1->SaveAs(filename);
	//c1->Write();
	
	gStyle->SetOptStat(0);
	for (int z=0; z<(NbZBin); z++){
		for (int r=0; r<NbRBin ; r++){
			h1->SetBinContent(r+1,NbZBin-z,LCEzr[z][r]*100);
		}
	}
	h1->GetZaxis()->SetTitle("LCE [pct]");
	
	sprintf(filename,"%s/%s_LCE_R%d-T%d-Z%d_initial_pct.pdf", workingdirectory.c_str(), datafilename.c_str(), NbRBin, NbTBin, NbZBin);
	//c1->SaveAs(filename);
	c1->Write();
	
	gStyle->SetOptStat("nem");
	sprintf(filename,"%s/%s_LCE_R%d-T%d-Z%d_initial_pct_stat.pdf", workingdirectory.c_str(), datafilename.c_str(), NbRBin, NbTBin, NbZBin);
	//c1->SaveAs(filename);
	//c1->Write();

	cout << "Average LCE: " << av << endl;
	
	delete f_plot;
}

/*=================================================================*/
