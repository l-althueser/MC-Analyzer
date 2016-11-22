/******************************************************************
 * MC plot LCE
 * 
 * @author	Lutz Althueser
 *
 ******************************************************************/
 // include own functions
#include "fileoperations.cc"
#include "TPC_Definition.cc"

// include C++ libs
#include <vector>
#include <iostream>
#include <stdio.h>
#include <string>
#include <sstream>
#include <algorithm>

#include <stdlib.h>

// include ROOT
#include "TROOT.h"
#include "TChain.h"
#include "TApplication.h"
#include "TSystemDirectory.h"
#include "TSystem.h"
#include "TCanvas.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TLegend.h"
#include "TStyle.h"
#include "TFile.h"
#include "TLine.h"
#include "TColor.h"
#include "TEntryList.h"
#include "TGaxis.h"

using namespace std;

/*=================================================================*/

void OpPhot_Xe1T_data(string datafile, string export_format, string suffix) {
	
	// read in datafilename and get working directory
	size_t found=datafile.find_last_of("/\\");
	string workingdirectory = datafile.substr(0,found);
	string datafilename = datafile.substr(found+1);
	
	// Read raw file and determine data structure
	ifstream raw;
	string linebuffer;
	
	const char* DELIMITER = " ";
	char* token[100] = {}; // initialize to 0
	
	// store LCE data
	TPC_Definition TPC;
	TPC.Set_Bins(9,4,4);
	string bin_format = "big";
	float ly[1000][1000]={0};
	float lyZ[1000]={0};
	float lyareatop[1000][1000]={0};
	float lyareatopZ[1000]={0};
	float nbinst[4] = {4.,6.,8.,12.};
	
	// open datafile
	raw.open(datafile.c_str());
	// check if the file could be opened successfully 
	if(!raw) {cout << "-> Error opening LCE map data file." << endl; gApplication->Terminate();}
		getline(raw, linebuffer);
		// read the data file line by line
		while (getline(raw, linebuffer)) {
			// parse the line
			char* buf = strdup(linebuffer.c_str());
			token[0] = strtok(buf, DELIMITER); // first token
			for (int n = 1; n < 100; n++) {
				token[n] = strtok(0, DELIMITER); // subsequent tokens
				if (!token[n]) break; // no more tokens
			}
			
			ly[atoi(token[0])][atoi(token[2])] += atof(token[6])/nbinst[atoi(token[2])];
			lyZ[atoi(token[0])] += atof(token[6])/(nbinst[atoi(token[2])]*TPC.Get_nbinsR());
			lyareatop[atoi(token[0])][atoi(token[2])] += atof(token[10])/nbinst[atoi(token[2])];
			lyareatopZ[atoi(token[0])] += atof(token[10])/(nbinst[atoi(token[2])]*TPC.Get_nbinsR());
		}
	raw.close();
	
	TFile *file_outplot;  
	char file_outplotname[10000];
	sprintf(file_outplotname,"%s/%s_plot_LCE.root", workingdirectory.c_str(),suffix.c_str());
	file_outplot = new TFile(file_outplotname,"RECREATE");
	
	const Int_t canvas_x = 650;
	const Int_t canvas_y = 800;
	char canvasfile[10000];
	
	TGaxis::SetMaxDigits(3);
	//TGaxis::SetExponentOffset(-0.01, 0.01, "y"); // X and Y offset for Y axis
	TGaxis::SetExponentOffset(0.01, -0.0325, "x"); // Y and Y offset for X axis
	
	const Int_t NRGBs = 5;
	const Int_t NCont = 255;
	static Int_t ColPalette[255];
	Double_t stops[NRGBs] = { 0.00, 0.34, 0.61, 0.84, 1.00 };
	Double_t red[NRGBs]   = { 0.00, 0.00, 0.87, 1.00, 0.51 };
	Double_t green[NRGBs] = { 0.00, 0.81, 1.00, 0.20, 0.00 };
	Double_t blue[NRGBs]  = { 0.51, 1.00, 0.12, 0.00, 0.00 };
	
	Int_t FI = TColor::CreateGradientColorTable(NRGBs,stops,red,green,blue,NCont);
    for (int i=0; i<NCont; i++) ColPalette[i] = FI+i;
		
	TStyle *style_1D = new TStyle("1D","1D");
	style_1D->SetCanvasColor(10);
	style_1D->SetTitleFillColor(0);
	style_1D->SetOptStat(0);
	style_1D->SetPadLeftMargin(0.105);
	style_1D->SetPadRightMargin(0.09);
	style_1D->SetPadTopMargin(0.075);
	style_1D->SetPadBottomMargin(0.075);
	
	style_1D->SetTitleOffset(1.,"X");
	style_1D->SetTitleOffset(1.45,"Y");
	style_1D->SetTitleOffset(1.35,"Z");
	
	style_1D->SetPalette(NCont,ColPalette);
	style_1D->SetNumberContours(NCont);
	style_1D->cd();
	
	TStyle *style_2D = new TStyle("2D","2D");
	style_2D->SetCanvasColor(10);
	style_2D->SetTitleFillColor(0);
	style_2D->SetOptStat(0);
	style_2D->SetPadLeftMargin(0.105);
	style_2D->SetPadRightMargin(0.15);
	style_2D->SetPadTopMargin(0.075);
	style_2D->SetPadBottomMargin(0.075);
	
	style_2D->SetTitleOffset(1.,"X");
	style_2D->SetTitleOffset(1.45,"Y");
	style_2D->SetTitleOffset(1.35,"Z");
	
	style_2D->SetPalette(NCont,ColPalette);
	style_2D->SetNumberContours(NCont);
	style_2D->cd();
	
	TStyle *style_3D = new TStyle("3D","3D");
	style_3D->SetCanvasColor(10);
	style_3D->SetTitleFillColor(0);
	style_3D->SetOptStat(0);
	style_3D->SetPadLeftMargin(0.15);
	style_3D->SetPadRightMargin(0.05);
	style_3D->SetPadTopMargin(0.075);
	style_3D->SetPadBottomMargin(0.075);
	
	style_3D->SetTitleOffset(1.75,"X");
	style_3D->SetTitleOffset(1.75,"Y");
	style_3D->SetTitleOffset(2.,"Z");
	
	style_3D->SetPalette(NCont,ColPalette);
	style_3D->SetNumberContours(99);
	style_3D->cd();
	
	gStyle->SetPalette(NCont,ColPalette);
		
	/*=================================================================*/
	gROOT->SetBatch(kFALSE);
	/*=================================================================*/
	/*=================================================================*/
	// generate ly map
	/*=================================================================*/
	style_2D->cd();
	TCanvas *c_ly_map = new TCanvas("ly_map","ly_map",canvas_x,canvas_y);
	TH2F* h_ly_map = new TH2F("ly_map", "ly map of ^{83m}Kr data", TPC.Get_nbinsR(), TPC.Get_LXe_minRR(), TPC.Get_LXe_maxRR(), TPC.Get_nbinsZ(), TPC.Get_LXe_minZ(), TPC.Get_LXe_maxZ());
	h_ly_map->SetXTitle("R^{2} [cm^{2}]");
	h_ly_map->GetXaxis()->CenterTitle();
	h_ly_map->SetYTitle("Z [cm]");
	h_ly_map->GetYaxis()->CenterTitle();
	h_ly_map->SetZTitle("ly [pe/keV]");
	h_ly_map->GetZaxis()->CenterTitle();
	for (int z=0; z<(TPC.Get_nbinsZ()); z++){
		for (int r=0; r<TPC.Get_nbinsR() ; r++){
			h_ly_map->SetBinContent(r+1,TPC.Get_nbinsZ()-z,ly[z][r]);
		}
	}
	h_ly_map->Draw("colz");
	if (file_outplot) c_ly_map->Write();
	sprintf(canvasfile,"%s/%s_ly_map.%s", workingdirectory.c_str(),suffix.c_str(),export_format.c_str());
	if (!(export_format=="")) c_ly_map->SaveAs(canvasfile);

	/*=================================================================*/
	gROOT->SetBatch(kTRUE);
	/*=================================================================*/
	/*=================================================================*/
	// generate relative LCE map
	/*=================================================================*/
	style_2D->cd();
	TCanvas *c_rLCE_map = new TCanvas("rLCE_map","rLCE_map",canvas_x,canvas_y);
	TH2F* h_rLCE_map = new TH2F("rLCE_map", "relative LCE map of ^{83m}Kr data", TPC.Get_nbinsR(), TPC.Get_LXe_minRR(), TPC.Get_LXe_maxRR(), TPC.Get_nbinsZ(), TPC.Get_LXe_minZ(), TPC.Get_LXe_maxZ());
	h_rLCE_map->SetXTitle("R^{2} [cm^{2}]");
	h_rLCE_map->GetXaxis()->CenterTitle();
	h_rLCE_map->SetYTitle("Z [cm]");
	h_rLCE_map->GetYaxis()->CenterTitle();
	h_rLCE_map->SetZTitle("relative LCE");
	h_rLCE_map->GetZaxis()->CenterTitle();
	for (int z=0; z<(TPC.Get_nbinsZ()); z++){
		for (int r=0; r<TPC.Get_nbinsR() ; r++){
			h_rLCE_map->SetBinContent(r+1,TPC.Get_nbinsZ()-z,ly[z][r]);
		}
	}
	double h_rLCE_map_mean = 0;
	for (int z=0; z<(TPC.Get_nbinsZ()); z++){
		for (int r=0; r<TPC.Get_nbinsR(); r++){
				h_rLCE_map_mean += h_rLCE_map->GetBinContent(r+1,TPC.Get_nbinsZ()-z)/(TPC.Get_nbinsZ()*TPC.Get_nbinsR());
		}
	}
	cout << "Mean ly: " << h_rLCE_map_mean << endl;
	h_rLCE_map->Scale(1./h_rLCE_map_mean);
	h_rLCE_map->Draw("colz");
	if (file_outplot) c_rLCE_map->Write();
	sprintf(canvasfile,"%s/%s_rLCE_map.%s", workingdirectory.c_str(),suffix.c_str(),export_format.c_str());
	if (!(export_format=="")) c_rLCE_map->SaveAs(canvasfile);
	
	/*=================================================================*/
	// generate relative LCE map 3D
	/*=================================================================*/
	style_3D->cd();
	TCanvas *c_rLCE_map_3D = new TCanvas("rLCE_map_3D","rLCE_map_3D",canvas_x,canvas_y);
	TH2F* h_rLCE_map_3D = new TH2F("rLCE_map_3D", "relative LCE map of ^{83m}Kr data", TPC.Get_nbinsZ(), TPC.Get_LXe_minZ(), TPC.Get_LXe_maxZ(), TPC.Get_nbinsR(), TPC.Get_LXe_minRR(), TPC.Get_LXe_maxRR());
	h_rLCE_map_3D->SetYTitle("R^{2} [cm^{2}]");
	h_rLCE_map_3D->GetYaxis()->CenterTitle();
	h_rLCE_map_3D->SetXTitle("Z [cm]");
	h_rLCE_map_3D->GetXaxis()->CenterTitle();
	h_rLCE_map_3D->SetZTitle("relative LCE");
	h_rLCE_map_3D->GetZaxis()->CenterTitle();
	for (int z=0; z<TPC.Get_nbinsZ(); z++){
		for (int r=0; r<TPC.Get_nbinsR(); r++){
				h_rLCE_map_3D->SetBinContent(z+1,r+1,h_rLCE_map->GetBinContent(r+1,z+1));
		}
	}
	h_rLCE_map_3D->Draw("surf1");
	c_rLCE_map_3D->SetTheta(20.);
	c_rLCE_map_3D->SetPhi(220.);
	if (file_outplot) c_rLCE_map_3D->Write();
	sprintf(canvasfile,"%s/%s_rLCE_map_3D.%s", workingdirectory.c_str(),suffix.c_str(),export_format.c_str());
	if (!(export_format=="")) c_rLCE_map_3D->SaveAs(canvasfile);

	/*=================================================================*/
	// generate relative LCE map TOP PMTs
	/*=================================================================*/
	style_2D->cd();
	TCanvas *c_rLCE_map_top = new TCanvas("rLCE_map_top","rLCE_map_top",canvas_x,canvas_y);
	TH2F* h_rLCE_map_top = new TH2F("rLCE_map_top", "relative LCE map of ^{83m}Kr data (TOP PMTs)", TPC.Get_nbinsR(), TPC.Get_LXe_minRR(), TPC.Get_LXe_maxRR(), TPC.Get_nbinsZ(), TPC.Get_LXe_minZ(), TPC.Get_LXe_maxZ());
	h_rLCE_map_top->SetXTitle("R^{2} [cm^{2}]");
	h_rLCE_map_top->GetXaxis()->CenterTitle();
	h_rLCE_map_top->SetYTitle("Z [cm]");
	h_rLCE_map_top->GetYaxis()->CenterTitle();
	h_rLCE_map_top->SetZTitle("relative LCE");
	h_rLCE_map_top->GetZaxis()->CenterTitle();
	for (int z=0; z<(TPC.Get_nbinsZ()); z++){
		for (int r=0; r<TPC.Get_nbinsR() ; r++){
			h_rLCE_map_top->SetBinContent(r+1,TPC.Get_nbinsZ()-z,lyareatop[z][r]);
		}
	}
	h_rLCE_map_top->Scale(1./h_rLCE_map_mean);
	h_rLCE_map_top->Draw("colz");
	if (file_outplot) c_rLCE_map_top->Write();
	sprintf(canvasfile,"%s/%s_rLCE_map_top.%s", workingdirectory.c_str(),suffix.c_str(),export_format.c_str());
	if (!(export_format=="")) c_rLCE_map_top->SaveAs(canvasfile);
	
	/*=================================================================*/
	// generate relative LCE map BOTTOM PMTs
	/*=================================================================*/
	style_2D->cd();
	TCanvas *c_rLCE_map_bottom = new TCanvas("rLCE_map_bottom","rLCE_map_bottom",canvas_x,canvas_y);
	TH2F* h_rLCE_map_bottom = new TH2F("rLCE_map_bottom", "relative LCE map of ^{83m}Kr data (BOTTOM PMTs)", TPC.Get_nbinsR(), TPC.Get_LXe_minRR(), TPC.Get_LXe_maxRR(), TPC.Get_nbinsZ(), TPC.Get_LXe_minZ(), TPC.Get_LXe_maxZ());
	h_rLCE_map_bottom->SetXTitle("R^{2} [cm^{2}]");
	h_rLCE_map_bottom->GetXaxis()->CenterTitle();
	h_rLCE_map_bottom->SetYTitle("Z [cm]");
	h_rLCE_map_bottom->GetYaxis()->CenterTitle();
	h_rLCE_map_bottom->SetZTitle("relative LCE");
	h_rLCE_map_bottom->GetZaxis()->CenterTitle();
	h_rLCE_map_bottom->Add(h_rLCE_map,1);
	h_rLCE_map_bottom->Add(h_rLCE_map_top,-1);
	h_rLCE_map_bottom->Draw("colz");
	if (file_outplot) c_rLCE_map_bottom->Write();
	sprintf(canvasfile,"%s/%s_rLCE_map_bottom.%s", workingdirectory.c_str(),suffix.c_str(),export_format.c_str());
	if (!(export_format=="")) c_rLCE_map_bottom->SaveAs(canvasfile);

	/*=================================================================*/
	gROOT->SetBatch(kFALSE);
	/*=================================================================*/
	/*=================================================================*/
	// generate LCE vs. Z
	/*=================================================================*/
	style_1D->cd();
	TH1F* h_LCE_LCEZ = new TH1F("LCE_LCEZ", "LCE vs. Z of ^{83m}Kr data", TPC.Get_nbinsZ(), TPC.Get_LXe_minZ(), TPC.Get_LXe_maxZ());
	h_LCE_LCEZ->Sumw2();
	for (int z=0; z<(TPC.Get_nbinsZ()); z++){
		h_LCE_LCEZ->SetBinContent(TPC.Get_nbinsZ()-z,lyZ[z]);
	}
	h_LCE_LCEZ->Scale((1./50.)/0.3*100);

	TH1F* h_LCE_LCEZ_top = new TH1F("LCE_LCEZ_top", "LCE vs. Z of ^{83m}Kr data TOP", TPC.Get_nbinsZ(), TPC.Get_LXe_minZ(), TPC.Get_LXe_maxZ());
	h_LCE_LCEZ_top->Sumw2();
	for (int z=0; z<(TPC.Get_nbinsZ()); z++){
		h_LCE_LCEZ_top->SetBinContent(TPC.Get_nbinsZ()-z,lyareatopZ[z]); // (peak.area_fraction_top * peak.area)/32.1498, so (S1Top/S1Total)*S1Total/Energy
	}
	h_LCE_LCEZ_top->Scale((1./50.)/0.3*100);

	TH1F* h_LCE_LCEZ_bottom = new TH1F("LCE_LCEZ_bottom", "LCE vs. Z of ^{83m}Kr data BOTTOM", TPC.Get_nbinsZ(), TPC.Get_LXe_minZ(), TPC.Get_LXe_maxZ());
	h_LCE_LCEZ_bottom->Add(h_LCE_LCEZ,1);
	h_LCE_LCEZ_bottom->Add(h_LCE_LCEZ_top,-1);
	
	style_1D->cd();
	TCanvas *c_LCE_LCEZ_ALL = new TCanvas("LCE_LCEZ_ALL","LCE_LCEZ_ALL",canvas_x,canvas_y);
	c_LCE_LCEZ_ALL->SetGridy();
	h_LCE_LCEZ->SetTitle("LCE vs. Z of ^{83m}Kr data");
	h_LCE_LCEZ->SetXTitle("Z [cm]");
	h_LCE_LCEZ->GetXaxis()->CenterTitle();
	h_LCE_LCEZ->SetYTitle("LCE [%]");
	h_LCE_LCEZ->GetYaxis()->CenterTitle();
	h_LCE_LCEZ->SetLineColor(kBlue);
	h_LCE_LCEZ->SetMarkerColor(kBlue);
	h_LCE_LCEZ->SetMarkerStyle(8);
	h_LCE_LCEZ->GetYaxis()->SetRangeUser(0.,50.);
	h_LCE_LCEZ->Draw("P");
	h_LCE_LCEZ_top->SetLineColor(kRed);
	h_LCE_LCEZ_top->SetMarkerColor(kRed);
	h_LCE_LCEZ_top->SetMarkerStyle(8);
	h_LCE_LCEZ_top->Draw("P same");
	h_LCE_LCEZ_bottom->SetLineColor(kGreen);
	h_LCE_LCEZ_bottom->SetMarkerColor(kGreen);
	h_LCE_LCEZ_bottom->SetMarkerStyle(8);
	h_LCE_LCEZ_bottom->Draw("P same");

	TLegend *leg_LCEZ = new TLegend(0.64,0.77,0.97,0.97);
	leg_LCEZ->SetFillColor(0);
	leg_LCEZ->SetTextSize(0.04);
	leg_LCEZ->SetTextAlign(22);         
	leg_LCEZ->AddEntry(h_LCE_LCEZ,"All PMTs","P"); 
	leg_LCEZ->AddEntry(h_LCE_LCEZ_top,"Top PMTs","P");
	leg_LCEZ->AddEntry(h_LCE_LCEZ_bottom,"Bottom PMTs","P");
	leg_LCEZ->Draw();    

	if (file_outplot) c_LCE_LCEZ_ALL->Write();	
	sprintf(canvasfile,"%s/%s_LCE_LCEZ.%s", workingdirectory.c_str(),suffix.c_str(),export_format.c_str());
	if (!(export_format=="")) c_LCE_LCEZ_ALL->SaveAs(canvasfile);
	
	/*=================================================================*/
	gROOT->SetBatch(kTRUE);
	/*=================================================================*/
	/*=================================================================*/
	// generate ly vs. Z
	/*=================================================================*/
	style_1D->cd();
	TH1F* h_ly_lyZ = new TH1F("ly_lyZ", "ly vs. Z of ^{83m}Kr data", TPC.Get_nbinsZ(), TPC.Get_LXe_minZ(), TPC.Get_LXe_maxZ());
	h_ly_lyZ->Sumw2();
	for (int z=0; z<(TPC.Get_nbinsZ()); z++){
		h_ly_lyZ->SetBinContent(TPC.Get_nbinsZ()-z,lyZ[z]);
	}

	TH1F* h_ly_lyZ_top = new TH1F("ly_lyZ_top", "ly vs. Z of ^{83m}Kr data TOP", TPC.Get_nbinsZ(), TPC.Get_LXe_minZ(), TPC.Get_LXe_maxZ());
	h_ly_lyZ_top->Sumw2();
	for (int z=0; z<(TPC.Get_nbinsZ()); z++){
		h_ly_lyZ_top->SetBinContent(TPC.Get_nbinsZ()-z,lyareatopZ[z]); // (peak.area_fraction_top * peak.area)/32.1498, so (S1Top/S1Total)*S1Total/Energy
	}
	
	TH1F* h_ly_lyZ_bottom = new TH1F("ly_lyZ_bottom", "ly vs. Z of ^{83m}Kr data BOTTOM", TPC.Get_nbinsZ(), TPC.Get_LXe_minZ(), TPC.Get_LXe_maxZ());
	h_ly_lyZ_bottom->Add(h_ly_lyZ,1);
	h_ly_lyZ_bottom->Add(h_ly_lyZ_top,-1);

	style_1D->cd();
	TCanvas *c_ly_lyZ_ALL = new TCanvas("ly_lyZ_ALL","ly_lyZ_ALL",canvas_x,canvas_y);
	c_ly_lyZ_ALL->SetGridy();
	h_ly_lyZ->SetTitle("ly vs. Z");
	h_ly_lyZ->SetXTitle("Z [cm]");
	h_ly_lyZ->GetXaxis()->CenterTitle();
	h_ly_lyZ->SetYTitle("ly vs. Z of ^{83m}Kr data");
	h_ly_lyZ->GetYaxis()->CenterTitle();
	h_ly_lyZ->SetLineColor(kBlue);
	h_ly_lyZ->SetMarkerColor(kBlue);
	h_ly_lyZ->SetMarkerStyle(8);
	h_ly_lyZ->GetYaxis()->SetRangeUser(0.,7.5);
	h_ly_lyZ->Draw("P");
	h_ly_lyZ_top->SetLineColor(kRed);
	h_ly_lyZ_top->SetMarkerColor(kRed);
	h_ly_lyZ_top->SetMarkerStyle(8);
	h_ly_lyZ_top->Draw("P same");
	h_ly_lyZ_bottom->SetLineColor(kGreen);
	h_ly_lyZ_bottom->SetMarkerColor(kGreen);
	h_ly_lyZ_bottom->SetMarkerStyle(8);
	h_ly_lyZ_bottom->Draw("P same");

	TLegend *leg_lyZ = new TLegend(0.64,0.77,0.97,0.97);
	leg_lyZ->SetFillColor(0);
	leg_lyZ->SetTextSize(0.04);
	leg_lyZ->SetTextAlign(22);         
	leg_lyZ->AddEntry(h_ly_lyZ,"All PMTs","P"); 
	leg_lyZ->AddEntry(h_ly_lyZ_top,"Top PMTs","P");
	leg_lyZ->AddEntry(h_ly_lyZ_bottom,"Bottom PMTs","P");
	leg_lyZ->Draw();    

	if (file_outplot) c_ly_lyZ_ALL->Write();	
	sprintf(canvasfile,"%s/%s_ly_lyZ.%s", workingdirectory.c_str(),suffix.c_str(),export_format.c_str());
	if (!(export_format=="")) c_ly_lyZ_ALL->SaveAs(canvasfile);
	
	gROOT->SetBatch(kFALSE);	
	//file_outplot->Close();  
	
	
}