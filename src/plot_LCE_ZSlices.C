/******************************************************************
 * plot_LCE_ZSlices.C
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

void LCE_MapZSlices(){
	cout << "Please insert a datafile as parameter." << endl;
}

/*=================================================================*/

void LCE_MapZSlices(string datafile){
	LCE_MapZSlices(datafile, 10, 10, 10);
}

/*=================================================================*/

void LCE_MapZSlices(string datafile, const int NbRBin, const int NbTBin, const int NbZBin) {
	size_t found=datafile.find_last_of("/\\");
	string workingdirectory = datafile.substr(0,found);
	string datafilename = datafile.substr(found+1);
	TString Tdatafile = datafile;

	//////////////////////////////////////////////////////////////////

	// recommended values: NbRBin = 40; NbTBin = 10; NbZBin = 40;
	const int NbVolumes = NbRBin*NbTBin*NbZBin; 
	const float DetMax = 		169.;//mm
	const float DetMin = 			2.;//mm
	const float DetHeight = DetMax - DetMin;//mm 
	const float DetRadius = 40.;//mm
	
	cout << "R is divided in " << NbRBin << " bins" << endl;
	cout << "Theta is divided in " << NbTBin << " bins" << endl;
	cout << "Z is divided in " << NbZBin << " bins" << endl;
	cout << "The Detector is divided into " << NbVolumes << " volumes" << endl;

	float ly[NbZBin][NbTBin][NbRBin]={0};
	float LCE[NbZBin][NbTBin][NbRBin]={0};

	// read txtfile
	char filename[10000];
	sprintf(filename,"%s/%s_LCE-LY_R%d-T%d-Z%d.dat", workingdirectory.c_str(), datafilename.c_str(), NbRBin, NbTBin, NbZBin);

	ifstream txtfile;
	txtfile.open(filename);

	int zi=0;
	int ti=0;
	int ri=0;
	float d=0;

	char dum[100];
	txtfile.getline(dum,100);

	for (int i=0; i<=NbVolumes; i++){
		txtfile >> zi >> ti >> ri >> d >> d >> d >> ly[zi][ti][ri] >> LCE[zi][ti][ri] >> d >> d;
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

	TH2F *hLYMap[NbZBin];
	TH2F *dumh[NbZBin];
	char title_stat[1000];
	char title[1000];
	float Tlow,Thigh,Rlow,Rhigh,Zlow,Zhigh;

	gStyle->SetPadLeftMargin(0.09);
	gStyle->SetPadRightMargin(0.185);
  gStyle->SetPadTopMargin(0.1);
	gStyle->SetPadBottomMargin(0.1);

	int z_x = 5;
	int z_y = NbZBin/z_x + 0.5;
	TCanvas *zslices = new TCanvas("LY ZSlices","LY ZSlices",400*z_x,400*z_y);
	zslices->Divide(z_x,z_y,0.0025,0.01);

	float binmax = ly[0][0][0];
	float binmin = ly[0][0][0];
	float avly = 0;

	for (int z=0; z<NbZBin; z++){
		Zhigh = -z*DetHeight/NbZBin-DetMin;
		Zlow = (-1-z)*DetHeight/NbZBin-DetMin;

		zslices->cd(z+1);
		
		sprintf(title_stat,"LY rtMapZ%d",z);
		sprintf(title,"Light Yield rtMap for %.2f<z<%.2f",Zhigh,Zlow);
		hLYMap[z] = new TH2F(title_stat,title,NbTBin,0,2*TMath::Pi(),NbRBin,0,DetRadius);
		dumh[z] = new TH2F(title_stat,title,0,-DetRadius,DetRadius,0,-DetRadius,DetRadius);

		for (int t=0; t<NbTBin; t++){	
			for (int r=0; r<NbRBin ; r++){
				hLYMap[z]->SetBinContent(t+1,r+1,ly[z][t][r]);
				if (binmax < ly[z][t][r]) binmax = ly[z][t][r];
				if (binmin > ly[z][t][r]) binmin = ly[z][t][r];
				avly += ly[z][t][r]/(NbZBin*NbTBin*NbRBin);
			}
		}
		dumh[z]->Draw();
		hLYMap[z]->Draw("POL COLZ same");
		hLYMap[z]->GetZaxis()->SetTitle("ly [pe/keV]");
 		hLYMap[z]->GetZaxis()->SetTitleOffset(1.4);
		dumh[z]->GetXaxis()->SetTitle("x[mm]");
		dumh[z]->GetYaxis()->SetTitle("y[mm]");
		dumh[z]->GetXaxis()->SetLabelSize(0.05);
  	dumh[z]->GetYaxis()->SetLabelSize(0.05);
  	dumh[z]->GetXaxis()->SetTitleSize(0.05);
  	dumh[z]->GetYaxis()->SetTitleSize(0.05);
  	hLYMap[z]->GetZaxis()->SetLabelSize(0.05);
		hLYMap[z]->GetZaxis()->SetTitleSize(0.05);
		
		gStyle->SetOptStat(0000);
		gStyle->SetOptFit(0000);
	}
	
	for (int z=0; z<NbZBin; z++){
		hLYMap[z]->SetMaximum(binmax);
    hLYMap[z]->SetMinimum(binmin);
	}
	
	sprintf(filename,"%s/%s_LY_ZSlices_R%d-T%d-Z%d.pdf", workingdirectory.c_str(), datafilename.c_str(), NbRBin, NbTBin, NbZBin);
	zslices->SaveAs(filename);
	
	//for (int z=0; z<NbZBin; z++){
	//	Float_t *bins = hLYMap[z]->GetArray();
	//	for (Int_t i=0;i<hLYMap[z]->GetSize();i++) bins[i] = bins[i]/binmax;
	//	hLYMap[z]->GetZaxis()->SetTitle("ly [pct]");
	//	hLYMap[z]->SetMaximum(1);
  //  hLYMap[z]->SetMinimum(binmin/binmax);
	//}

	//sprintf(filename,"%s/%s_LY_ZSlices_R%d-T%d-Z%d_pct.pdf", workingdirectory.c_str(), datafilename.c_str(), NbRBin, NbTBin, NbZBin);
	//zslices->SaveAs(filename);
	
	cout << endl << "average ly: " << avly << endl;
}

/*=================================================================*/
