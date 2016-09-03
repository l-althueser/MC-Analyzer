/******************************************************************
 * plot_LM.C
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
#include <string>
#include <numeric>
#include <math.h> 
#include <cmath>
using namespace std;

/*=================================================================*/

void LM_xy(){
	cout << "Please insert a datafile as parameter." << endl;
}

/*=================================================================*/

void LM_xy(string datafile){
	LM_xy(datafile, 40, 10, 40);
}

/*=================================================================*/

void LM_xy(string datafile, const int NbRBin, const int NbTBin, const int NbZBin) {
	LM_xy(datafile, "events", NbRBin, NbTBin, NbZBin);
}

/*=================================================================*/

void LM_xy(string datafile, string zname, const int NbRBin, const int NbTBin, const int NbZBin) {
	size_t found=datafile.find_last_of("/\\");
	string workingdirectory = datafile.substr(0,found);
	string datafilename = datafile.substr(found+1);
	TString Tdatafile = datafile;

	//////////////////////////////////////////////////////////////////

	// recommended values: NbRBin = 40; NbTBin = 10; NbZBin = 40;
	const int NbVolumes = NbRBin*NbTBin*NbZBin; 
	const float DetMax = -169.;//mm
	const float DetMin = -2.;//mm
	const float DetHeight = -(DetMax - DetMin);//mm 
	const float DetRadius = 40.;//mm

	cout << "R is divided in " << NbRBin << " bins" << endl;
	cout << "Theta is divided in " << NbTBin << " bins" << endl;
	cout << "Z is divided in " << NbZBin << " bins" << endl;
	cout << "The Detector is divided into " << NbVolumes << " volumes" << endl;
	
	found=datafilename.find_last_of(".");
	string outputfilename = datafilename.substr(0,found);
	const char rootfileout[10000];
	sprintf(rootfileout,"%s/%s_plot.root", workingdirectory.c_str(), outputfilename.c_str());
	TFile *f_plot = TFile::Open(rootfileout,"UPDATE");
	if (!f_plot) { return; }

	//////////////////////////////////////////////////////////////////

	char filename[10000];
	sprintf(filename,"%s/%s_LM_ZSlice_R%d-T%d-Z%d.dat", workingdirectory.c_str(), datafilename.c_str(), NbRBin, NbTBin, NbZBin);

	ifstream txtfile;
	txtfile.open(filename);
 
	int zi=0;
	int ti=0;
	int ri=0;
	int counter[NbZBin][NbTBin][NbRBin]={0};

	float d=0;
	char dum[1000];
	txtfile.getline(dum,1000);

	if (zname == "pmthits") {
		for (int i=0; i<=NbVolumes; i++){
		txtfile >> zi >> ti >> ri >> d >> d >> d >> d >> counter[zi][ti][ri] >> d >> d;
		}
	}
	else if (zname == "energy") {
		for (int i=0; i<=NbVolumes; i++){
		txtfile >> zi >> ti >> ri >> d >> d >> d >> d >> d >> d >> counter[zi][ti][ri];
		}
	}
	else if ((zname == "events") || (zname == "2pos") || (zname == "3pos") || (zname == "4pos")) {
		for (int i=0; i<=NbVolumes; i++){
		txtfile >> zi >> ti >> ri >> d >> d >> d >> d >> d >> counter[zi][ti][ri] >> d;
		}
	}
	else if (zname == "ly") {
		for (int i=0; i<=NbVolumes; i++){
		txtfile >> zi >> ti >> ri >> d >> d >> d >> counter[zi][ti][ri] >> d >> d >> d;
		}
	}
	else {
		for (int i=0; i<=NbVolumes; i++){
		txtfile >> zi >> ti >> ri >> d >> d >> d >> d >> d >> counter[zi][ti][ri] >> d;
		}
		zname = "events";
	}
	
	TStyle *myStyle = new TStyle("myStyle","my style");
	myStyle->SetPalette(1,0);
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

	TH2F *hLYMap;
	TH2F *dumh;
	char title_stat[1000];
	char title[1000];
	string smalltitle = "";
	string filetag = "";
	
	gStyle->SetPadLeftMargin(0.105);
	gStyle->SetPadRightMargin(0.13);
  gStyle->SetPadTopMargin(0.06);
	gStyle->SetPadBottomMargin(0.11);

	TCanvas *zslices = new TCanvas("LM ZSlice","LM ZSlice");
	zslices.SetCanvasSize(800,700);
		
	sprintf(title_stat,"LM rtMap");
	sprintf(title,"%s_LM_ZSlice_R%d-T%d-Z%d", datafilename.c_str(), NbRBin, NbTBin, NbZBin);
	hLMMap = new TH2F(title_stat,title,NbTBin,0,2*TMath::Pi(),NbRBin,0,DetRadius);
	dumh = new TH2F(title_stat,title,0,-DetRadius,DetRadius,0,-DetRadius,DetRadius);

	double sum = 0;

	for (int t=0; t<NbTBin; t++){	
		for (int r=0; r<NbRBin; r++){
			double countertr = 0;
			for (int z=0; z<NbZBin; z++){
				countertr += counter[z][t][r];
				if (zname == "2pos") {
					int t2 = t + NbTBin/2;
					if ( t2 >= NbTBin ) t2 = t2 - NbTBin;
					countertr += counter[z][t2][r];
				}
				else if (zname == "3pos") {
					int t2 = t + NbTBin/3 +1;
					if ( t2 >= NbTBin ) t2 = t2 - NbTBin;
					countertr += counter[z][t2][r];
					int t3 = t + 2*NbTBin/3;
					if ( t3 >= NbTBin ) t3 = t3 - NbTBin;
					countertr += counter[z][t3][r];
				}
				else if (zname == "4pos") {
					int t2 = t + NbTBin/4;
					if ( t2 >= NbTBin ) t2 = t2 - NbTBin;
					countertr += counter[z][t2][r];
					int t3 = t + 2*NbTBin/4;
					if ( t3 >= NbTBin ) t3 = t3 - NbTBin;
					countertr += counter[z][t3][r];
					int t4 = t + 3*NbTBin/4;
					if ( t4 >= NbTBin ) t4 = t4 - NbTBin;
					countertr += counter[z][t4][r];
				}
			}
			hLMMap->SetBinContent(t+1,r+1,countertr);
			sum += countertr;
		}
	}
	smalltitle="(1 source)";
	
	if (zname == "2pos") {
		zname = "events";
		smalltitle="(2 sources)";
		filetag = "_2pos";
	}
	else if (zname == "3pos") {
		zname = "events";
		smalltitle="(3 sources)";
		filetag = "_3pos";
	}
	else if (zname == "4pos") {
		zname = "events";
		smalltitle="(4 sources)";
		filetag = "_4pos";
	}
					
	Float_t *bins = hLMMap->GetArray();
	int binmax = hLMMap->GetBinContent(hLMMap->GetMaximumBin());
	if (binmax == 0) {
 		cout << "===========================================" << endl;
 		cout << "Warning: no " << zname << " accessible ..." << endl;
 		cout << "===========================================" << endl;
 		return 0;
 	}
  //int binmid = (hLMMap->GetBinContent(NbYBin,NbXBin) + hLMMap->GetBinContent(NbYBin,NbXBin+1) + hLMMap->GetBinContent(NbYBin+1,NbXBin) + hLMMap->GetBinContent(NbYBin+1,NbXBin+1))/4; 

	dumh->Draw();
	hLMMap->Draw("POL COLZ same");
	sprintf(dum, "%s", zname.c_str()); 
	//hLMMap->GetZaxis()->SetTitle(dum);
 	//hLMMap->GetZaxis()->SetTitleOffset(1.45);
	dumh->GetXaxis()->SetTitle("x [mm]");
	dumh->GetYaxis()->SetTitle("y [mm]");
	dumh->GetXaxis()->SetLabelSize(0.05);
  dumh->GetYaxis()->SetLabelSize(0.05);
  dumh->GetXaxis()->SetTitleSize(0.05);
  dumh->GetYaxis()->SetTitleSize(0.05);
  
	dumh->Draw();
	hLMMap->Draw("POL COLZ same");
	
	hLMMap->GetZaxis()->SetLabelSize(0.05);
	hLMMap->GetZaxis()->SetTitleSize(0.05);
	
	// hide title and use the free space
  gStyle->SetOptTitle(0);

	TText *ZTitle = new TText();
 	ZTitle->SetNDC();
 	ZTitle->SetTextSize(0.04);
 	ZTitle->DrawText(0.78, 0.955, dum);
 			
 	TText *sTitle = new TText();
 	sTitle->SetNDC();
 	sTitle->SetTextSize(0.04);
 	sTitle->DrawText(0.415, 0.955, smalltitle.c_str());

	char val[30];
	sprintf(val,"sum of %s: %d", zname.c_str(), sum);
  TLatex caption(0.09,0.01,val); 
  caption.SetNDC(kTRUE);
  caption.Draw();

	gStyle->SetOptStat(0000);
	gStyle->SetOptFit(0000);

	//sprintf(filename,"%s/%s_LM_ZSlice_R%d-T%d-Z%d_%s%s_abs.pdf", workingdirectory.c_str(), datafilename.c_str(), NbRBin, NbTBin, NbZBin, zname.c_str(), filetag.c_str());
	//zslices->SaveAs(filename);
	//zslices->Write();

	gPad->SetLogz();
	sprintf(filename,"%s/%s_LM_ZSlice_R%d-T%d-Z%d_%s%s_abs_logz.pdf", workingdirectory.c_str(), datafilename.c_str(), NbRBin, NbTBin, NbZBin, zname.c_str(), filetag.c_str());
	//zslices->SaveAs(filename);
	zslices->Write();
	gPad->SetLogz(0);

	for (Int_t i=0;i<hLMMap.GetSize();i++) bins[i] = bins[i]/sum*100;
	sprintf(dum, "%s [pct]", zname.c_str());
	//hLMMap->GetZaxis()->SetTitle(dum);
	ZTitle->DrawText(0.78, 0.955, dum);

	sprintf(filename,"%s/%s_LM_ZSlice_R%d-T%d-Z%d_%s%s_pct.pdf", workingdirectory.c_str(), datafilename.c_str(), NbRBin, NbTBin, NbZBin, zname.c_str(), filetag.c_str());
	//zslices->SaveAs(filename);
	zslices->Write();

	gPad->SetLogz();
	sprintf(filename,"%s/%s_LM_ZSlice_R%d-T%d-Z%d_%s%s_pct_logz.pdf", workingdirectory.c_str(), datafilename.c_str(), NbRBin, NbTBin, NbZBin, zname.c_str(), filetag.c_str());
	//zslices->SaveAs(filename);
	zslices->Write();
	gPad->SetLogz(0);
	
	delete f_plot;
}

/*=================================================================*/

void LM_zx(){
	cout << "Please insert a datafile as parameter." << endl;
}

/*=================================================================*/

void LM_zx(string datafile){
	LM_zx(datafile, 40, 40);
}

/*=================================================================*/

void LM_zx(string datafile, const int NbZBin, const int NbXBin) {
	LM_zx(datafile, "events", NbZBin, NbXBin);
}

/*=================================================================*/

void LM_zx(string datafile, string zname, const int NbZBin, const int NbXBin) {
	size_t found=datafile.find_last_of("/\\");
	string workingdirectory = datafile.substr(0,found);
	string datafilename = datafile.substr(found+1);
	TString Tdatafile = datafile;

	//////////////////////////////////////////////////////////////////

	// recommended values: NbZBin = 40; NbXBin = 40;
	const int NbVolumes = NbZBin*NbXBin; 
	const float DetMax = -169.;//mm
	const float DetMin = 			2.;//mm
	const float DetHeight = -(DetMax - DetMin);//mm 
	const float DetRadius = 40.;//mm

	cout << "Z is divided in " << NbZBin << " bins" << endl;
	cout << "X is divided in " << NbXBin << " bins" << endl;
	cout << "The Detector is divided into " << NbVolumes << " volumes." << endl;

	//////////////////////////////////////////////////////////////////
	
	found=datafilename.find_last_of(".");
	string outputfilename = datafilename.substr(0,found);
	const char rootfileout[10000];
	sprintf(rootfileout,"%s/%s_plot.root", workingdirectory.c_str(), outputfilename.c_str());
	TFile *f_plot = TFile::Open(rootfileout,"UPDATE");
	if (!f_plot) { return; }

	char filename[10000];
	sprintf(filename,"%s/%s_LM_Z%d-X%d.dat", workingdirectory.c_str(), datafilename.c_str(), NbZBin, NbXBin);

	ifstream txtfile;
	txtfile.open(filename);
 
	int zi=0;
	int xi=0;
	int counter[NbZBin][NbXBin]={0};

	float d=0;
	char dum[1000];
	txtfile.getline(dum,1000);

	if (zname == "pmthits") {
		for (int i=0; i<=NbVolumes; i++){
			txtfile >> zi >> xi >> d >> d >> d >> counter[zi][xi] >> d >> d;
		}
	}
	else if (zname == "energy") {
		for (int i=0; i<=NbVolumes; i++){
			txtfile >> zi >> xi >> d >> d >> d >> d >> d >> counter[zi][xi];
		}
	}
	else if (zname == "events") {
		for (int i=0; i<=NbVolumes; i++){
			txtfile >> zi >> xi >> d >> d >> d >> d >> counter[zi][xi] >> d;
		}
	}
	else {
		for (int i=0; i<=NbVolumes; i++){
			txtfile >> zi >> xi >> d >> d >> d >> d >> counter[zi][xi] >> d;
		}
		zname = "events";
	}
	
	gStyle->SetPadLeftMargin(0.105);
	gStyle->SetPadRightMargin(0.165);
  gStyle->SetPadTopMargin(0.02);
	gStyle->SetPadBottomMargin(0.11);
	
	sprintf(filename,"%s_LM_Z%d-X%d", datafilename.c_str(), NbZBin, NbXBin);
	TCanvas *c1 = new TCanvas(filename,filename);
	TStyle *myStyle = new TStyle("myStyle","my style");
	myStyle->SetPalette(1,0);
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
	
	double sum = 0;
	
	TH2F *h1 = new TH2F("LM",filename,NbXBin,-DetRadius,DetRadius,NbZBin,DetMax,DetMin);
	for (int z=0; z<(NbZBin); z++){
		for (int x=0; x<NbXBin; x++){
			h1->SetBinContent(x+1,NbZBin-z,counter[z][x]);
			sum += counter[z][x];
		}
 	}

 Float_t *bins = h1->GetArray();
 int binmax = h1->GetBinContent(h1->GetMaximumBin());
 if (binmax == 0) {
 	cout << "===========================================" << endl;
 	cout << "Warning: no " << zname << " accessible ..." << endl;
 	cout << "===========================================" << endl;
 	return 0;
 }
 int binmid = (h1->GetBinContent(NbXBin,NbZBin/2) + h1->GetBinContent(NbXBin,(NbZBin/2)+1) + h1->GetBinContent(NbXBin+1,NbZBin/2) + h1->GetBinContent(NbXBin+1,(NbZBin/2)+1))/4; 

	sprintf(dum, "%s", zname.c_str());
  h1->GetZaxis()->SetTitle(dum);
  h1->GetZaxis()->SetTitleOffset(1.2);
  h1->GetYaxis()->SetTitle("z [mm]");
  h1->GetXaxis()->SetTitle("y [mm]");
  h1->Draw("COLZ");
 
  h1->GetXaxis()->SetLabelSize(0.05);
  h1->GetYaxis()->SetLabelSize(0.05);
  h1->GetZaxis()->SetLabelSize(0.05);
  h1->GetXaxis()->SetTitleSize(0.05);
  h1->GetYaxis()->SetTitleSize(0.05);
  h1->GetZaxis()->SetTitleSize(0.05);
 
  // hide title and use the free space
  gStyle->SetOptTitle(0);

	char val[30];
	sprintf(val,"sum of %s: %d", zname.c_str(), sum);
  TLatex caption(0.09,0.01,val); 
  caption.SetNDC(kTRUE);
  caption.Draw();
 
	// you can hide the stats
	gStyle->SetOptStat(0);
	gStyle->SetStatY(0.9);
	gStyle->SetStatX(0.875);
	gStyle->SetStatW(0.125);
	gStyle->SetStatH(0.125); 

	//gStyle->SetOptStat("");
	//sprintf(filename,"%s/%s_LM_Z%d-X%d_%s_abs.pdf", workingdirectory.c_str(), datafilename.c_str(), NbZBin, NbXBin, zname.c_str());
	//zslices->SaveAs(filename);
	//zslices->Write();

	//gStyle->SetOptStat("neim");
	//sprintf(filename,"%s/%s_LM_Z%d-X%d_%s_abs_stat.pdf", workingdirectory.c_str(), datafilename.c_str(), NbZBin, NbXBin, zname.c_str());
	//zslices->SaveAs(filename);
	//zslices->Write();

	for (Int_t i=0;i<h1->GetSize();i++) bins[i] = bins[i]/sum*100;
	sprintf(dum, "%s [pct]", zname.c_str());
 	h1->GetZaxis()->SetTitle(dum);

	gStyle->SetOptStat("");
	sprintf(filename,"%s/%s_LM_Z%d-X%d_%s_pct.pdf", workingdirectory.c_str(), datafilename.c_str(), NbZBin, NbXBin, zname.c_str());
	//zslices->SaveAs(filename);
	zslices->Write();

	//gStyle->SetOptStat("neim");
	//sprintf(filename,"%s/%s_LM_Z%d-X%d_%s_pct_stat.pdf", workingdirectory.c_str(), datafilename.c_str(), NbZBin, NbXBin, zname.c_str());
	//zslices->SaveAs(filename);
	//zslices->Write();
	
	delete f_plot;
}

/*=================================================================*/

void LM_zy(){
	cout << "Please insert a datafile as parameter." << endl;
}

/*=================================================================*/

void LM_zy(string datafile){
	LM_zy(datafile, 40, 40);
}

/*=================================================================*/

void LM_zy(string datafile, const int NbZBin, const int NbYBin) {
	LM_zy(datafile, "events", NbZBin, NbYBin);
}

/*=================================================================*/

void LM_zy(string datafile, string zname, const int NbZBin, const int NbYBin) {
	size_t found=datafile.find_last_of("/\\");
	string workingdirectory = datafile.substr(0,found);
	string datafilename = datafile.substr(found+1);
	TString Tdatafile = datafile;

	//////////////////////////////////////////////////////////////////

	// recommended values: NbZBin = 40; NbYBin = 40;
	const int NbVolumes = NbZBin*NbYBin; 
	const float DetMax = -169.;//mm
	const float DetMin = -2.;//mm
	const float DetHeight = -(DetMax - DetMin);//mm 
	const float DetRadius = 40.;//mm

	cout << "Z is divided in " << NbZBin << " bins" << endl;
	cout << "Y is divided in " << NbYBin << " bins" << endl;
	cout << "The Detector is divided into " << NbVolumes << " volumes." << endl;

	//////////////////////////////////////////////////////////////////

	found=datafilename.find_last_of(".");
	string outputfilename = datafilename.substr(0,found);
	const char rootfileout[10000];
	sprintf(rootfileout,"%s/%s_plot.root", workingdirectory.c_str(), outputfilename.c_str());
	TFile *f_plot = TFile::Open(rootfileout,"UPDATE");
	if (!f_plot) { return; }
	
	char filename[10000];
	sprintf(filename,"%s/%s_LM_Z%d-Y%d.dat", workingdirectory.c_str(), datafilename.c_str(), NbZBin, NbYBin);

	ifstream txtfile;
	txtfile.open(filename);
 
	int zi=0;
	int yi=0;
	int counter[NbZBin][NbYBin]={0};

	float d=0;
	char dum[1000];
	txtfile.getline(dum,1000);
	
	if (zname == "pmthits") {
		for (int i=0; i<=NbVolumes; i++){
			txtfile >> zi >> yi >> d >> d >> d >> counter[zi][yi] >> d >> d;
		}
	}
	else if (zname == "energy") {
		for (int i=0; i<=NbVolumes; i++){
			txtfile >> zi >> yi >> d >> d >> d >> d >> d >> counter[zi][yi];
		}
	}
	else if (zname == "events") {
		for (int i=0; i<=NbVolumes; i++){
			txtfile >> zi >> yi >> d >> d >> d >> d >> counter[zi][yi] >> d;
		}
	}
	else {
		for (int i=0; i<=NbVolumes; i++){
			txtfile >> zi >> yi >> d >> d >> d >> d >> counter[zi][yi] >> d;
		}
		zname == "events";
	}
	
	gStyle->SetPadLeftMargin(0.105);
	gStyle->SetPadRightMargin(0.165);
  gStyle->SetPadTopMargin(0.02);
	gStyle->SetPadBottomMargin(0.11);
	
	sprintf(filename,"%s_LM_Z%d-Y%d", datafilename.c_str(), NbZBin, NbYBin);
	TCanvas *c1 = new TCanvas(filename,filename);
	TStyle *myStyle = new TStyle("myStyle","my style");
	myStyle->SetPalette(1,0);
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

	double sum = 0;
	
	TH2F *h1 = new TH2F("LM",filename,NbYBin,-DetRadius,DetRadius,NbZBin,DetMax,DetMin);
	for (int z=0; z<(NbZBin); z++){
		for (int y=0; y<NbYBin; y++){
			h1->SetBinContent(y+1,NbZBin-z,counter[z][y]);
			sum += counter[z][y];
		}
 	}

 Float_t *bins = h1->GetArray();
 double binmax = h1->GetBinContent(h1->GetMaximumBin()); 
 if (binmax == 0) {
 	cout << "===========================================" << endl;
 	cout << "Warning: no " << zname << " accessible ..." << endl;
 	cout << "===========================================" << endl;
 	return 0;
 }
 int binmid = (h1->GetBinContent(NbYBin,NbZBin/2) + h1->GetBinContent(NbYBin,(NbZBin/2)+1) + h1->GetBinContent(NbYBin+1,NbZBin/2) + h1->GetBinContent(NbYBin+1,(NbZBin/2)+1))/4; 

 sprintf(dum, "%s", zname.c_str());
 h1->GetZaxis()->SetTitle(dum);
 h1->GetZaxis()->SetTitleOffset(1.2);
 h1->GetYaxis()->SetTitle("z [mm]");
 h1->GetXaxis()->SetTitle("y [mm]");
 h1->Draw("COLZ");
 
 h1->GetXaxis()->SetLabelSize(0.05);
 h1->GetYaxis()->SetLabelSize(0.05);
 h1->GetZaxis()->SetLabelSize(0.05);
 h1->GetXaxis()->SetTitleSize(0.05);
 h1->GetYaxis()->SetTitleSize(0.05);
 h1->GetZaxis()->SetTitleSize(0.05);
 
 // hide title and use the free space
 gStyle->SetOptTitle(0);
 
	char val[30];
	sprintf(val,"sum of %s: %d", zname.c_str(), sum);
  TLatex caption(0.09,0.01,val); 
  caption.SetNDC(kTRUE);
  caption.Draw();
 
	// you can hide the stats
	gStyle->SetOptStat(0);
	gStyle->SetStatY(0.9);
	gStyle->SetStatX(0.875);
	gStyle->SetStatW(0.125);
	gStyle->SetStatH(0.125); 

	//gStyle->SetOptStat("");
	//sprintf(filename,"%s/%s_LM_Z%d-Y%d_%s_abs.pdf", workingdirectory.c_str(), datafilename.c_str(), NbZBin, NbYBin, zname.c_str());
	//zslices->SaveAs(filename);
	//zslices->Write();
	
	//gStyle->SetOptStat("neim");
	//sprintf(filename,"%s/%s_LM_Z%d-Y%d_%s_abs_stat.pdf", workingdirectory.c_str(), datafilename.c_str(), NbZBin, NbYBin, zname.c_str());
	//zslices->SaveAs(filename);
	//zslices->Write();

	for (Int_t i=0;i<h1->GetSize();i++) bins[i] = bins[i]/sum*100;
	sprintf(dum, "%s [pct]", zname.c_str());
 	h1->GetZaxis()->SetTitle(dum);
 
	gStyle->SetOptStat("");
	sprintf(filename,"%s/%s_LM_Z%d-Y%d_%s_pct.pdf", workingdirectory.c_str(), datafilename.c_str(), NbZBin, NbYBin, zname.c_str());
	//zslices->SaveAs(filename);
	zslices->Write();
	
	//gStyle->SetOptStat("neim");
	//sprintf(filename,"%s/%s_LM_Z%d-Y%d_%s_pct_stat.pdf", workingdirectory.c_str(), datafilename.c_str(), NbZBin, NbYBin, zname.c_str());
	//zslices->SaveAs(filename);
	//zslices->Write();
	
	delete f_plot;
}

/*=================================================================*/
