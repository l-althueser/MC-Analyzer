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

void OpPhot_comparison(string datafile_kr, string datafile_mc, string export_format, string suffix) {
	
	if (fileexists(datafile_kr) == false) {
		cout << endl;
		cout << "x Error xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx" << endl;
		cout << "file not found:" << endl;
		cout << "-> " << datafile_kr << endl;
		cout << "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx" << endl;
		cout << endl;
		gApplication->Terminate();
	}
	
	if (fileexists(datafile_mc) == false) {
		cout << endl;
		cout << "x Error xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx" << endl;
		cout << "file not found:" << endl;
		cout << "-> " << datafile_mc << endl;
		cout << "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx" << endl;
		cout << endl;
		gApplication->Terminate();
	}
	
	// generate plots
	TFile *file_outplot;  
	char file_outname[10000];
	size_t found=datafile_mc.find_last_of("/\\");
	string workingdirectory = datafile_mc.substr(0,found);
	sprintf(file_outname,"%s/%s_comparison_plot.root", workingdirectory.c_str(),suffix.c_str());
	file_outplot = new TFile(file_outname,"RECREATE");
	
	TPC_Definition TPC;
	TPC.Set_Bins(9,4,4);
	string bin_format = "big";
	
	const Int_t canvas_x = 650;
	const Int_t canvas_y = 800;
	char canvasfile[10000];
	char draw_selection[10000];
	
	TGaxis::SetMaxDigits(3);
	//TGaxis::SetExponentOffset(-0.0325, +0.01, "z"); // X and Y offset for Z axis
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
	
	const Int_t NRGBs_Sym = 3;
	const Int_t NCont_Sym = 255;
	static Int_t ColPalette_Sym[255];
	Double_t stops_Sym[NRGBs_Sym] = { 0.00, 0.5, 1.00 };
	Double_t red_Sym[NRGBs_Sym]   = { 0.00, 1.0, 1.00 };
	Double_t green_Sym[NRGBs_Sym] = { 0.00, 1.0, 0.00 };
	Double_t blue_Sym[NRGBs_Sym]  = { 1.00, 1.0, 0.00 };
	
	Int_t FI_Sym = TColor::CreateGradientColorTable(NRGBs_Sym,stops_Sym,red_Sym,green_Sym,blue_Sym,NCont_Sym);
    for (int i=0; i<NCont_Sym; i++) ColPalette_Sym[i] = FI_Sym+i;
	
	TStyle *style_2D_Sym = new TStyle("2D_Sym","2D_Sym");
	style_2D_Sym->SetCanvasColor(10);
	style_2D_Sym->SetTitleFillColor(0);
	style_2D_Sym->SetOptStat(0);
	style_2D_Sym->SetPadLeftMargin(0.105);
	style_2D_Sym->SetPadRightMargin(0.15);
	style_2D_Sym->SetPadTopMargin(0.075);
	style_2D_Sym->SetPadBottomMargin(0.075);
	
	style_2D_Sym->SetTitleOffset(1.,"X");
	style_2D_Sym->SetTitleOffset(1.45,"Y");
	style_2D_Sym->SetTitleOffset(1.35,"Z");
	
	style_2D_Sym->SetPalette(NCont_Sym,ColPalette_Sym);
	style_2D_Sym->SetNumberContours(NCont_Sym);
	style_2D_Sym->cd();
	
	TStyle *style_3D_Sym = new TStyle("3D_Sym","3D_Sym");
	style_3D_Sym->SetCanvasColor(10);
	style_3D_Sym->SetTitleFillColor(0);
	style_3D_Sym->SetOptStat(0);
	style_3D_Sym->SetPadLeftMargin(0.15);
	style_3D_Sym->SetPadRightMargin(0.05);
	style_3D_Sym->SetPadTopMargin(0.075);
	style_3D_Sym->SetPadBottomMargin(0.075);
	
	style_3D_Sym->SetTitleOffset(1.75,"X");
	style_3D_Sym->SetTitleOffset(1.75,"Y");
	style_3D_Sym->SetTitleOffset(2.,"Z");
	
	style_3D_Sym->SetPalette(NCont_Sym,ColPalette_Sym);
	style_3D_Sym->SetNumberContours(99);
	style_3D_Sym->cd();
	
	gStyle->SetPalette(NCont,ColPalette);
	
	/*=================================================================*/
	/*=================================================================*/
	// Read in real data
	/*=================================================================*/
	/*=================================================================*/
	
	// Read raw file and determine data structure
	ifstream raw;
	string linebuffer;
	
	const char* const DELIMITER = " ";
	char* token[100] = {}; // initialize to 0
	
	// store LCE data
	float ly[1000][1000]={0};
	float lyZ[1000]={0};
	float lyrr[1000]={0};
	float lyareatop[1000][1000]={0};
	float lyareatopZ[1000]={0};
	float lyareatoprr[1000]={0};
	float nbinst[4] = {4.,6.,8.,12.};
	
	// open datafile
	raw.open(datafile_kr.c_str());
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
			lyrr[atoi(token[2])] += atof(token[6])/(nbinst[atoi(token[2])]*TPC.Get_nbinsZ());
			
			lyareatop[atoi(token[0])][atoi(token[2])] += atof(token[10])/nbinst[atoi(token[2])];
			lyareatopZ[atoi(token[0])] += atof(token[10])/(nbinst[atoi(token[2])]*TPC.Get_nbinsR());
			lyareatoprr[atoi(token[2])] += atof(token[10])/(nbinst[atoi(token[2])]*TPC.Get_nbinsZ());
		}
	raw.close();
	
	/*=================================================================*/
	gROOT->SetBatch(kTRUE);
	/*=================================================================*/
	/*=================================================================*/
	// generate ly map
	/*=================================================================*/
	style_2D->cd();
	TCanvas *c_ly_map = new TCanvas("ly_map","ly_map",canvas_x,canvas_y);
	TH2F* h_ly_map = new TH2F("ly_map", "ly map of ^{83m}Kr", TPC.Get_nbinsR(), TPC.Get_LXe_minRR(), TPC.Get_LXe_maxRR(), TPC.Get_nbinsZ(), TPC.Get_LXe_minZ(), TPC.Get_LXe_maxZ());
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
	sprintf(canvasfile,"%s/%s_comparison_ly_rrZ_Kr.%s", workingdirectory.c_str(),suffix.c_str(),export_format.c_str());
	if (!(export_format=="")) c_ly_map->SaveAs(canvasfile);
	
	/*=================================================================*/
	// generate ly map TOP PMTs
	/*=================================================================*/
	style_2D->cd();
	TCanvas *c_ly_map_top = new TCanvas("ly_map_top","ly_map_top",canvas_x,canvas_y);
	TH2F* h_ly_map_top = new TH2F("ly_map_top", "ly map of ^{83m}Kr (TOP PMTs)", TPC.Get_nbinsR(), TPC.Get_LXe_minRR(), TPC.Get_LXe_maxRR(), TPC.Get_nbinsZ(), TPC.Get_LXe_minZ(), TPC.Get_LXe_maxZ());
	h_ly_map_top->SetXTitle("R^{2} [cm^{2}]");
	h_ly_map_top->GetXaxis()->CenterTitle();
	h_ly_map_top->SetYTitle("Z [cm]");
	h_ly_map_top->GetYaxis()->CenterTitle();
	h_ly_map_top->SetZTitle("ly [pe/keV]");
	h_ly_map_top->GetZaxis()->CenterTitle();
	for (int z=0; z<(TPC.Get_nbinsZ()); z++){
		for (int r=0; r<TPC.Get_nbinsR() ; r++){
			h_ly_map_top->SetBinContent(r+1,TPC.Get_nbinsZ()-z,lyareatop[z][r]);
		}
	}
	h_ly_map_top->Draw("colz");
	if (file_outplot) c_ly_map_top->Write();
	sprintf(canvasfile,"%s/%s_comparison_ly_rrZ_top_Kr.%s", workingdirectory.c_str(),suffix.c_str(),export_format.c_str());
	if (!(export_format=="")) c_ly_map_top->SaveAs(canvasfile);
	
	/*=================================================================*/
	// generate ly map BOTTOM PMTs
	/*=================================================================*/
	style_2D->cd();
	TCanvas *c_ly_map_bottom = new TCanvas("ly_map_bottom","ly_map_bottom",canvas_x,canvas_y);
	TH2F* h_ly_map_bottom = new TH2F("ly_map_bottom", "ly map of ^{83m}Kr (BOTTOM PMTs)", TPC.Get_nbinsR(), TPC.Get_LXe_minRR(), TPC.Get_LXe_maxRR(), TPC.Get_nbinsZ(), TPC.Get_LXe_minZ(), TPC.Get_LXe_maxZ());
	h_ly_map_bottom->SetXTitle("R^{2} [cm^{2}]");
	h_ly_map_bottom->GetXaxis()->CenterTitle();
	h_ly_map_bottom->SetYTitle("Z [cm]");
	h_ly_map_bottom->GetYaxis()->CenterTitle();
	h_ly_map_bottom->SetZTitle("ly [pe/keV]");
	h_ly_map_bottom->GetZaxis()->CenterTitle();
	h_ly_map_bottom->Add(h_ly_map,1);
	h_ly_map_bottom->Add(h_ly_map_top,-1);
	h_ly_map_bottom->Draw("colz");
	if (file_outplot) c_ly_map_bottom->Write();
	sprintf(canvasfile,"%s/%s_comparison_ly_rrZ_bottom_Kr.%s", workingdirectory.c_str(),suffix.c_str(),export_format.c_str());
	if (!(export_format=="")) c_ly_map_bottom->SaveAs(canvasfile);
	
	/*=================================================================*/
	// generate relative LCE map
	/*=================================================================*/
	style_2D->cd();
	TCanvas *c_rLCE_map = new TCanvas("rLCE_map","rLCE_map",canvas_x,canvas_y);
	TH2F* h_rLCE_map = new TH2F("rLCE_map", "relative LCE map of ^{83m}Kr", TPC.Get_nbinsR(), TPC.Get_LXe_minRR(), TPC.Get_LXe_maxRR(), TPC.Get_nbinsZ(), TPC.Get_LXe_minZ(), TPC.Get_LXe_maxZ());
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
	h_rLCE_map->SetMaximum(1.5);
	h_rLCE_map->SetMinimum(0.5);
	h_rLCE_map->Draw("colz");
	if (file_outplot) c_rLCE_map->Write();
	sprintf(canvasfile,"%s/%s_comparison_rLCE_rrZ_Kr.%s", workingdirectory.c_str(),suffix.c_str(),export_format.c_str());
	if (!(export_format=="")) c_rLCE_map->SaveAs(canvasfile);
	
	/*=================================================================*/
	// generate relative LCE map 3D
	/*=================================================================*/
	style_3D->cd();
	TH2F* h_rLCE_map_3D = new TH2F("rLCE_map_3D", "relative LCE map of ^{83m}Kr", TPC.Get_nbinsZ(), TPC.Get_LXe_minZ(), TPC.Get_LXe_maxZ(), TPC.Get_nbinsR(), TPC.Get_LXe_minRR(), TPC.Get_LXe_maxRR());
	for (int z=0; z<TPC.Get_nbinsZ(); z++){
		for (int r=0; r<TPC.Get_nbinsR(); r++){
				h_rLCE_map_3D->SetBinContent(z+1,r+1,h_rLCE_map->GetBinContent(r+1,z+1));
		}
	}
	
	/*=================================================================*/
	// generate relative LCE map TOP PMTs
	/*=================================================================*/
	style_2D->cd();
	TCanvas *c_rLCE_map_top = new TCanvas("rLCE_map_top","rLCE_map_top",canvas_x,canvas_y);
	TH2F* h_rLCE_map_top = new TH2F("rLCE_map_top", "relative LCE map of ^{83m}Kr (TOP PMTs)", TPC.Get_nbinsR(), TPC.Get_LXe_minRR(), TPC.Get_LXe_maxRR(), TPC.Get_nbinsZ(), TPC.Get_LXe_minZ(), TPC.Get_LXe_maxZ());
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
	h_rLCE_map_top->SetMaximum(0.4);
	h_rLCE_map_top->SetMinimum(0);
	h_rLCE_map_top->Draw("colz");
	if (file_outplot) c_rLCE_map_top->Write();
	sprintf(canvasfile,"%s/%s_comparison_rLCE_rrZ_top_Kr.%s", workingdirectory.c_str(),suffix.c_str(),export_format.c_str());
	if (!(export_format=="")) c_rLCE_map_top->SaveAs(canvasfile);

	/*=================================================================*/
	// generate relative LCE map BOTTOM PMTs
	/*=================================================================*/
	style_2D->cd();
	TCanvas *c_rLCE_map_bottom = new TCanvas("rLCE_map_bottom","rLCE_map_bottom",canvas_x,canvas_y);
	TH2F* h_rLCE_map_bottom = new TH2F("rLCE_map_bottom", "relative LCE map of ^{83m}Kr (BOTTOM PMTs)", TPC.Get_nbinsR(), TPC.Get_LXe_minRR(), TPC.Get_LXe_maxRR(), TPC.Get_nbinsZ(), TPC.Get_LXe_minZ(), TPC.Get_LXe_maxZ());
	h_rLCE_map_bottom->SetXTitle("R^{2} [cm^{2}]");
	h_rLCE_map_bottom->GetXaxis()->CenterTitle();
	h_rLCE_map_bottom->SetYTitle("Z [cm]");
	h_rLCE_map_bottom->GetYaxis()->CenterTitle();
	h_rLCE_map_bottom->SetZTitle("relative LCE");
	h_rLCE_map_bottom->GetZaxis()->CenterTitle();
	h_rLCE_map_bottom->Add(h_rLCE_map,1);
	h_rLCE_map_bottom->Add(h_rLCE_map_top,-1);
	h_rLCE_map_bottom->SetMaximum(1.4);
	h_rLCE_map_bottom->SetMinimum(0.4);
	h_rLCE_map_bottom->Draw("colz");
	if (file_outplot) c_rLCE_map_bottom->Write();
	sprintf(canvasfile,"%s/%s_comparison_rLCE_rrZ_bottom_Kr.%s", workingdirectory.c_str(),suffix.c_str(),export_format.c_str());
	if (!(export_format=="")) c_rLCE_map_bottom->SaveAs(canvasfile);
	
	/*=================================================================*/
	// generate relative LCE vs. Z
	/*=================================================================*/
	style_1D->cd();
	TH1F* h_rLCE_LCEZ = new TH1F("rLCE_LCEZ", "relative LCE vs. Z of ^{83m}Kr", TPC.Get_nbinsZ(), TPC.Get_LXe_minZ(), TPC.Get_LXe_maxZ());
	h_rLCE_LCEZ->Sumw2();
	double h_rLCE_map_meanZ = 0;
	for (int z=0; z<(TPC.Get_nbinsZ()); z++){
		h_rLCE_LCEZ->SetBinContent(TPC.Get_nbinsZ()-z,lyZ[z]);
		h_rLCE_map_meanZ += lyZ[z]/(TPC.Get_nbinsZ());
	}
	h_rLCE_LCEZ->Scale(1./h_rLCE_map_meanZ);

	TH1F* h_rLCE_LCEZ_top = new TH1F("rLCE_LCEZ_top", "relative LCE vs. Z of ^{83m}Kr TOP", TPC.Get_nbinsZ(), TPC.Get_LXe_minZ(), TPC.Get_LXe_maxZ());
	h_rLCE_LCEZ_top->Sumw2();
	for (int z=0; z<(TPC.Get_nbinsZ()); z++){
		h_rLCE_LCEZ_top->SetBinContent(TPC.Get_nbinsZ()-z,lyareatopZ[z]); // (peak.area_fraction_top * peak.area)/32.1498, so (S1Top/S1Total)*S1Total/Energy
	}
	h_rLCE_LCEZ_top->Scale(1./h_rLCE_map_meanZ);

	TH1F* h_rLCE_LCEZ_bottom = new TH1F("rLCE_LCEZ_bottom", "relative LCE vs. Z of ^{83m}Kr TOP", TPC.Get_nbinsZ(), TPC.Get_LXe_minZ(), TPC.Get_LXe_maxZ());
	h_rLCE_LCEZ_bottom->Add(h_rLCE_LCEZ,1);
	h_rLCE_LCEZ_bottom->Add(h_rLCE_LCEZ_top,-1);
	
	/*=================================================================*/
	// LCE vs. Z
	/*=================================================================*/
	style_1D->cd();
	TH1F* h_Kr_LCE_LCEZ = new TH1F("Kr_LCE_LCEZ", "LCE vs. Z of ^{83m}Kr", TPC.Get_nbinsZ(), TPC.Get_LXe_minZ(), TPC.Get_LXe_maxZ());
	h_Kr_LCE_LCEZ->Sumw2();
	for (int z=0; z<(TPC.Get_nbinsZ()); z++){
		h_Kr_LCE_LCEZ->SetBinContent(TPC.Get_nbinsZ()-z,lyZ[z]);
	}

	TH1F* h_Kr_LCE_LCEZ_top = new TH1F("Kr_LCE_LCEZ_top", "LCE vs. Z of ^{83m}Kr TOP", TPC.Get_nbinsZ(), TPC.Get_LXe_minZ(), TPC.Get_LXe_maxZ());
	h_Kr_LCE_LCEZ_top->Sumw2();
	for (int z=0; z<(TPC.Get_nbinsZ()); z++){
		h_Kr_LCE_LCEZ_top->SetBinContent(TPC.Get_nbinsZ()-z,lyareatopZ[z]); // (peak.area_fraction_top * peak.area)/32.1498, so (S1Top/S1Total)*S1Total/Energy
	}
	//h_rLCE_LCEZ_top->Draw("");
	
	/*=================================================================*/
	// AFT vs. Z
	/*=================================================================*/
	style_1D->cd();
	TH1F* h_AFTZ_Kr = new TH1F("AFTZ_Kr", "AFT vs. Z", TPC.Get_nbinsZ(), TPC.Get_LXe_minZ(), TPC.Get_LXe_maxZ());
	h_AFTZ_Kr->Sumw2();
	h_AFTZ_Kr->Divide(h_Kr_LCE_LCEZ_top, h_Kr_LCE_LCEZ, 1.,1., "b");
	h_AFTZ_Kr->Scale(100.);
	
	/*=================================================================*/
	// LCE vs. rr
	/*=================================================================*/
	style_1D->cd();
	TH1F* h_Kr_LCE_LCErr = new TH1F("Kr_LCE_LCErr", "LCE vs. rr of ^{83m}Kr", TPC.Get_nbinsR(), TPC.Get_LXe_minRR(), TPC.Get_LXe_maxRR());
	h_Kr_LCE_LCErr->Sumw2();
	for (int rr=0; rr<(TPC.Get_nbinsR()); rr++){
		h_Kr_LCE_LCErr->SetBinContent(TPC.Get_nbinsR()-rr,lyrr[rr]);
	}

	TH1F* h_Kr_LCE_LCErr_top = new TH1F("Kr_LCE_LCErr_top", "LCE vs. rr of ^{83m}Kr TOP", TPC.Get_nbinsR(), TPC.Get_LXe_minRR(), TPC.Get_LXe_maxRR());
	h_Kr_LCE_LCErr_top->Sumw2();
	for (int rr=0; rr<(TPC.Get_nbinsR()); rr++){
		h_Kr_LCE_LCErr_top->SetBinContent(TPC.Get_nbinsR()-rr,lyareatoprr[rr]); // (peak.area_fraction_top * peak.area)/32.1498, so (S1Top/S1Total)*S1Total/Energy
	}
	
	/*=================================================================*/
	// AFT vs. rr
	/*=================================================================*/
	style_1D->cd();
	TH1F* h_AFTrr_Kr = new TH1F("AFTrr_Kr", "AFT vs. rr", TPC.Get_nbinsR(), TPC.Get_LXe_minRR(), TPC.Get_LXe_maxRR());
	h_AFTrr_Kr->Sumw2();
	h_AFTrr_Kr->Divide(h_Kr_LCE_LCErr_top, h_Kr_LCE_LCErr, 1.,1., "b");
	h_AFTrr_Kr->Scale(100.);
	
	/*=================================================================*/
	gROOT->SetBatch(kFALSE);
	/*=================================================================*/
	/*=================================================================*/
	/*=================================================================*/
	// Read in MC data
	/*=================================================================*/
	/*=================================================================*/
	
	// read in datafilename and get working directory
	found=datafile_mc.find_last_of("/\\");
	workingdirectory = datafile_mc.substr(0,found);
	string datafilename = datafile_mc.substr(found+1);
	
	TChain *file_input_tree = new TChain("events/events");
	
	if (fileexists(datafile_mc) == false) {
		cout << endl;
		cout << "x Error xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx" << endl;
		cout << "file not found:" << endl;
		cout << "-> " << datafile_mc << endl;
		cout << "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx" << endl;
		cout << endl;
		gApplication->Terminate();
	}
	else if ((fileexists(datafile_mc) == true) && (datafilename=="")) {
		cout << "= reading datafiles ==== dir mode ==========================" << endl;
		
			string ext = ".root";
		TSystemDirectory dir(workingdirectory.c_str(), workingdirectory.c_str());
		TList *files = dir.GetListOfFiles();
		if (files) {
			TSystemFile *file;
			TString fname;
			TIter next(files);
			while ((file=(TSystemFile*)next())) {
				fname = file->GetName();
				if (!file->IsDirectory() && fname.EndsWith(ext.c_str()) && !(fname.Contains(suffix.c_str()))) {
					char filename[10000];
					sprintf(filename,"%s/%s", workingdirectory.c_str(), fname.Data());
					
					if (file_input_tree->GetEntries() == 0) {
						TFile *f = new TFile(filename,"READ");
						if ( (f->GetListOfKeys()->Contains("events")) && !(f->GetListOfKeys()->Contains("MC_TAG")) ) {
							TPC.TPC_Xe1T();
							TPC.Set_LCE_max(50);
							f->Close();
						}
						else if ( (f->GetListOfKeys()->Contains("MC_TAG")) && (f->GetListOfKeys()->Contains("events")) ){
							TPC.TPC_MS();
							TPC.Set_LCE_max(30);
							f->Close();
						}
						else {
							cout << endl;
							cout << "x Error xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx" << endl;
							cout << "File format not known:" << endl;
							cout << "-> " << filename << endl;
							cout << "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx" << endl;
							cout << endl;
							gApplication->Terminate();
						}						
					}
					
					file_input_tree->AddFile(filename); 
					const int nevents = file_input_tree->GetEntries();
					cout << " file: " << fname.Data() << " " << nevents << " events total" << endl;
				}
			}
		}		
	}
	else {
		cout << "= reading datafile ===== single file =======================" << endl;
		if (file_input_tree->GetEntries() == 0) {
			TFile *f = new TFile(datafile_mc.c_str(),"READ");
			if ( (f->GetListOfKeys()->Contains("events")) && !(f->GetListOfKeys()->Contains("MC_TAG")) ) {
				TPC.TPC_Xe1T();
				TPC.Set_LCE_max(50);
				f->Close();
			}
			else if ( (f->GetListOfKeys()->Contains("MC_TAG")) && (f->GetListOfKeys()->Contains("events")) ){
				TPC.TPC_MS();
				TPC.Set_LCE_max(30);
				f->Close();
			}
			else {
				cout << endl;
				cout << "x Error xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx" << endl;
				cout << "File format not known:" << endl;
				cout << "-> " << datafile_mc << endl;
				cout << "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx" << endl;
				cout << endl;
				gApplication->Terminate();
			}						
		}
		file_input_tree->AddFile(datafile_mc.c_str()); 
		const int nevents = file_input_tree->GetEntries();
		cout << " file: " << datafilename << " " << nevents << " events " << endl;
	}
	cout << "============================================================" << endl;
	
	file_outplot->cd();
	
	file_input_tree->SetAlias("rrp_pri","(xp_pri*xp_pri + yp_pri*yp_pri)/10./10.");  
	
	/*=================================================================*/
	gROOT->SetBatch(kTRUE);
	/*=================================================================*/
	/*=================================================================*/
	// R^{2} vs. Z of generated events LXe
	/*=================================================================*/
	style_2D->cd();
	TH2F* h_rrZ = new TH2F("rrZ_pri", "R^{2} vs. Z generated events", TPC.Get_nbinsRR(), TPC.Get_LXe_minRR(), TPC.Get_LXe_maxRR(), TPC.Get_nbinsZ(), TPC.Get_LXe_minZ(), TPC.Get_LXe_maxZ());
	sprintf(draw_selection,"zp_pri/10<=%f && zp_pri/10>=%f && rrp_pri>=%f && rrp_pri<=%f",TPC.Get_LXe_maxZ(),TPC.Get_LXe_minZ(),TPC.Get_LXe_minRR(),TPC.Get_LXe_maxRR());
	file_input_tree->Draw("zp_pri/10. : rrp_pri >> rrZ_pri", draw_selection, "goff");

	/*=================================================================*/
	// R^{2} vs. Z detected events (TOP + BOTTOM PMTs)
	/*=================================================================*/
	style_2D->cd();
	TH2F* h_rrZ_det = new TH2F("rrZ_det", "R^{2} vs. Z detected events (ALL PMTs)", TPC.Get_nbinsRR(), TPC.Get_LXe_minRR(), TPC.Get_LXe_maxRR(), TPC.Get_nbinsZ(), TPC.Get_LXe_minZ(), TPC.Get_LXe_maxZ());
	sprintf(draw_selection,"zp_pri/10<=%f && zp_pri/10>=%f && rrp_pri>=%f && rrp_pri<=%f && (nbpmthits > 0 || ntpmthits > 0)",TPC.Get_LXe_maxZ(),TPC.Get_LXe_minZ(),TPC.Get_LXe_minRR(),TPC.Get_LXe_maxRR());
	file_input_tree->Draw("zp_pri/10. : rrp_pri >> rrZ_det", draw_selection, "goff");
	
	/*=================================================================*/
	// R^{2} vs. Z detected events (TOP PMTs)
	/*=================================================================*/
	style_2D->cd();
	TH2F* h_rrZ_det_top = new TH2F("rrZ_det_top", "R^{2} vs. Z detected events (TOP PMTs)", TPC.Get_nbinsRR(), TPC.Get_LXe_minRR(), TPC.Get_LXe_maxRR(), TPC.Get_nbinsZ(), TPC.Get_LXe_minZ(), TPC.Get_LXe_maxZ());
	sprintf(draw_selection,"zp_pri/10<=%f && zp_pri/10>=%f && rrp_pri>=%f && rrp_pri<=%f && (ntpmthits > 0)",TPC.Get_LXe_maxZ(),TPC.Get_LXe_minZ(),TPC.Get_LXe_minRR(),TPC.Get_LXe_maxRR());
	file_input_tree->Draw("zp_pri/10. : rrp_pri >> rrZ_det_top", draw_selection, "goff");
	
	/*=================================================================*/
	// R^{2} vs. Z detected events (BOTTOM PMTs)
	/*=================================================================*/
	style_2D->cd();
	TH2F* h_rrZ_det_bottom = new TH2F("rrZ_det_bottom", "R^{2} vs. Z detected events (BOTTOM PMTs)", TPC.Get_nbinsRR(), TPC.Get_LXe_minRR(), TPC.Get_LXe_maxRR(), TPC.Get_nbinsZ(), TPC.Get_LXe_minZ(), TPC.Get_LXe_maxZ());
	sprintf(draw_selection,"zp_pri/10<=%f && zp_pri/10>=%f && rrp_pri>=%f && rrp_pri<=%f && (nbpmthits > 0)",TPC.Get_LXe_maxZ(),TPC.Get_LXe_minZ(),TPC.Get_LXe_minRR(),TPC.Get_LXe_maxRR());
	file_input_tree->Draw("zp_pri/10. : rrp_pri >> rrZ_det_bottom", draw_selection, "goff");
	
	/*=================================================================*/
	// ly of R^{2} vs. Z
	/*=================================================================*/
	gStyle->SetPalette(NCont,ColPalette);
	style_2D->cd();
	TCanvas *c_ly_rrZ = new TCanvas("ly_rrZ","ly_rrZ",canvas_x,canvas_y);
	TH2F* h_ly_rrZ = new TH2F("ly_rrZ", "ly of R^{2} vs. Z (ALL PMTs)", TPC.Get_nbinsRR(), TPC.Get_LXe_minRR(), TPC.Get_LXe_maxRR(), TPC.Get_nbinsZ(), TPC.Get_LXe_minZ(), TPC.Get_LXe_maxZ());
	h_ly_rrZ->SetXTitle("R^{2} [cm^{2}]");
	h_ly_rrZ->GetXaxis()->CenterTitle();
	h_ly_rrZ->SetYTitle("Z [cm]");
	h_ly_rrZ->GetYaxis()->CenterTitle();
	h_ly_rrZ->SetZTitle("ly [pe/keV]");
	h_ly_rrZ->GetZaxis()->CenterTitle();
	h_ly_rrZ->Sumw2();
	// Assume the average photon yield from NEST (50 ph/keV, at 32 keV, at 150 V/cm). QE*CE =~ 31%
	h_ly_rrZ->Add(h_rrZ_det_top, TPC.Get_QE_top());
	h_ly_rrZ->Add(h_rrZ_det_bottom, TPC.Get_QE_bottom());
	h_ly_rrZ->Divide(h_rrZ);
	h_ly_rrZ->Scale(50.); 
	h_ly_rrZ->Draw("colz");
	TLegend *leg_ly_rrZ = new TLegend(0.725,0.935,1.,1.);
	leg_ly_rrZ->SetFillColor(0);     
	sprintf(canvasfile,"MC_QE_top: %0.3f", TPC.Get_QE_top());
	leg_ly_rrZ->AddEntry((TObject*)0,canvasfile,""); 
	sprintf(canvasfile,"MC_QE_bottom: %0.3f", TPC.Get_QE_bottom());
	leg_ly_rrZ->AddEntry((TObject*)0,canvasfile,"");
	leg_ly_rrZ->Draw();  
	if (file_outplot) c_ly_rrZ->Write();
	sprintf(canvasfile,"%s/%s_comparison_ly_rrZ_MC.%s", workingdirectory.c_str(),suffix.c_str(),export_format.c_str());
	if (!(export_format=="")) c_ly_rrZ->SaveAs(canvasfile);
	
	/*=================================================================*/
	// ly of R^{2} vs. Z TOP PMTs
	/*=================================================================*/
	gStyle->SetPalette(NCont,ColPalette);
	style_2D->cd();
	TCanvas *c_ly_rrZ_top = new TCanvas("ly_rrZ_top","ly_rrZ_top",canvas_x,canvas_y);
	TH2F* h_ly_rrZ_top = new TH2F("ly_rrZ_top", "ly of R^{2} vs. Z (TOP PMTs)", TPC.Get_nbinsRR(), TPC.Get_LXe_minRR(), TPC.Get_LXe_maxRR(), TPC.Get_nbinsZ(), TPC.Get_LXe_minZ(), TPC.Get_LXe_maxZ());
	h_ly_rrZ_top->SetXTitle("R^{2} [cm^{2}]");
	h_ly_rrZ_top->GetXaxis()->CenterTitle();
	h_ly_rrZ_top->SetYTitle("Z [cm]");
	h_ly_rrZ_top->GetYaxis()->CenterTitle();
	h_ly_rrZ_top->SetZTitle("ly [pe/keV]");
	h_ly_rrZ_top->GetZaxis()->CenterTitle();
	h_ly_rrZ_top->Sumw2();
	// Assume the average photon yield from NEST (50 ph/keV, at 32 keV, at 150 V/cm). QE*CE =~ 31%
	h_ly_rrZ_top->Add(h_rrZ_det_top, TPC.Get_QE_top());
	h_ly_rrZ_top->Divide(h_rrZ);
	h_ly_rrZ_top->Scale(50.); 
	h_ly_rrZ_top->Draw("colz");
	if (file_outplot) c_ly_rrZ_top->Write();
	sprintf(canvasfile,"%s/%s_comparison_ly_rrZ_top_MC.%s", workingdirectory.c_str(),suffix.c_str(),export_format.c_str());
	if (!(export_format=="")) c_ly_rrZ_top->SaveAs(canvasfile);
	
	/*=================================================================*/
	// ly of R^{2} vs. Z BOTTOM PMTs
	/*=================================================================*/
	gStyle->SetPalette(NCont,ColPalette);
	style_2D->cd();
	TCanvas *c_ly_rrZ_bottom = new TCanvas("ly_rrZ_bottom","ly_rrZ_bottom",canvas_x,canvas_y);
	TH2F* h_ly_rrZ_bottom = new TH2F("ly_rrZ_bottom", "ly of R^{2} vs. Z (BOTTOM PMTs)", TPC.Get_nbinsRR(), TPC.Get_LXe_minRR(), TPC.Get_LXe_maxRR(), TPC.Get_nbinsZ(), TPC.Get_LXe_minZ(), TPC.Get_LXe_maxZ());
	h_ly_rrZ_bottom->SetXTitle("R^{2} [cm^{2}]");
	h_ly_rrZ_bottom->GetXaxis()->CenterTitle();
	h_ly_rrZ_bottom->SetYTitle("Z [cm]");
	h_ly_rrZ_bottom->GetYaxis()->CenterTitle();
	h_ly_rrZ_bottom->SetZTitle("ly [pe/keV]");
	h_ly_rrZ_bottom->GetZaxis()->CenterTitle();
	h_ly_rrZ_bottom->Sumw2();
	// Assume the average photon yield from NEST (50 ph/keV, at 32 keV, at 150 V/cm). QE*CE =~ 31%
	h_ly_rrZ_bottom->Add(h_rrZ_det_bottom, TPC.Get_QE_bottom());
	h_ly_rrZ_bottom->Divide(h_rrZ);
	h_ly_rrZ_bottom->Scale(50.); 
	h_ly_rrZ_bottom->Draw("colz");	
	if (file_outplot) c_ly_rrZ_bottom->Write();
	sprintf(canvasfile,"%s/%s_comparison_ly_rrZ_bottom_MC.%s", workingdirectory.c_str(),suffix.c_str(),export_format.c_str());
	if (!(export_format=="")) c_ly_rrZ_bottom->SaveAs(canvasfile);
	
	/*=================================================================*/
	gROOT->SetBatch(kFALSE);
	/*=================================================================*/
	/*=================================================================*/
	// comparison ly of R^{2} vs. Z
	/*=================================================================*/
	gStyle->SetPalette(NCont_Sym,ColPalette_Sym);
	style_2D_Sym->cd();
	TCanvas *c_cly_rrZ = new TCanvas("cly_rrZ","cly_rrZ",canvas_x,canvas_y);
	TH2F* h_cly_rrZ = new TH2F("cly_rrZ", "ly of R^{2} vs. Z (ALL PMTs) of ^{83m}Kr vs. MC", TPC.Get_nbinsRR(), TPC.Get_LXe_minRR(), TPC.Get_LXe_maxRR(), TPC.Get_nbinsZ(), TPC.Get_LXe_minZ(), TPC.Get_LXe_maxZ());
	h_cly_rrZ->SetXTitle("R^{2} [cm^{2}]");
	h_cly_rrZ->GetXaxis()->CenterTitle();
	h_cly_rrZ->SetYTitle("Z [cm]");
	h_cly_rrZ->GetYaxis()->CenterTitle();
	h_cly_rrZ->SetZTitle("ly ^{83m}Kr - ly MC [pe/keV]");
	h_cly_rrZ->GetZaxis()->CenterTitle();
	h_cly_rrZ->Add(h_ly_rrZ,-1);
	h_cly_rrZ->Add(h_ly_map,1);
	h_cly_rrZ->Draw("colz");
	TLegend *leg_cly_rrZ = new TLegend(0.725,0.935,1.,1.);
	leg_cly_rrZ->SetFillColor(0);    
	sprintf(canvasfile,"MC_QE_top: %0.3f", TPC.Get_QE_top());
	leg_cly_rrZ->AddEntry((TObject*)0,canvasfile,""); 
	sprintf(canvasfile,"MC_QE_bottom: %0.3f", TPC.Get_QE_bottom());
	leg_cly_rrZ->AddEntry((TObject*)0,canvasfile,"");
	leg_cly_rrZ->Draw();  
	if ( (abs(h_cly_rrZ->GetMaximum()) > abs(h_cly_rrZ->GetMinimum())) && (h_cly_rrZ->GetMaximum() > 0.) )
		{h_cly_rrZ->SetMinimum(-abs(h_cly_rrZ->GetMaximum()));}
	else if ( (abs(h_cly_rrZ->GetMaximum()) < abs(h_cly_rrZ->GetMinimum())) && (h_cly_rrZ->GetMinimum() < 0.) ) 
		{h_cly_rrZ->SetMaximum(abs(h_cly_rrZ->GetMinimum()));}
	// else do nothing; 
	if (file_outplot) c_cly_rrZ->Write();
	sprintf(canvasfile,"%s/%s_comparison_ly_rrZ.%s", workingdirectory.c_str(),suffix.c_str(),export_format.c_str());
	if (!(export_format=="")) c_cly_rrZ->SaveAs(canvasfile);
	
	/*=================================================================*/
	gROOT->SetBatch(kTRUE);
	/*=================================================================*/
	/*=================================================================*/
	// comparison ly of R^{2} vs. Z TOP PMTs
	/*=================================================================*/
	gStyle->SetPalette(NCont_Sym,ColPalette_Sym);
	style_2D_Sym->cd();
	TCanvas *c_cly_rrZ_top = new TCanvas("cly_rrZ_top","cly_rrZ_top",canvas_x,canvas_y);
	TH2F* h_cly_rrZ_top = new TH2F("cly_rrZ_top", "ly of R^{2} vs. Z (TOP PMTs) of ^{83m}Kr vs. MC", TPC.Get_nbinsRR(), TPC.Get_LXe_minRR(), TPC.Get_LXe_maxRR(), TPC.Get_nbinsZ(), TPC.Get_LXe_minZ(), TPC.Get_LXe_maxZ());
	h_cly_rrZ_top->SetXTitle("R^{2} [cm^{2}]");
	h_cly_rrZ_top->GetXaxis()->CenterTitle();
	h_cly_rrZ_top->SetYTitle("Z [cm]");
	h_cly_rrZ_top->GetYaxis()->CenterTitle();
	h_cly_rrZ_top->SetZTitle("ly ^{83m}Kr - ly MC [pe/keV]");
	h_cly_rrZ_top->GetZaxis()->CenterTitle();
	h_cly_rrZ_top->Add(h_ly_rrZ_top,-1);
	h_cly_rrZ_top->Add(h_ly_map_top,1);
	h_cly_rrZ_top->Draw("colz");
	if ( (abs(h_cly_rrZ_top->GetMaximum()) > abs(h_cly_rrZ_top->GetMinimum())) && (h_cly_rrZ_top->GetMaximum() > 0.) )
		{h_cly_rrZ_top->SetMinimum(-abs(h_cly_rrZ_top->GetMaximum()));}
	else if ( (abs(h_cly_rrZ_top->GetMaximum()) < abs(h_cly_rrZ_top->GetMinimum())) && (h_cly_rrZ_top->GetMinimum() < 0.) ) 
		{h_cly_rrZ_top->SetMaximum(abs(h_cly_rrZ_top->GetMinimum()));}
	// else do nothing; 
	if (file_outplot) c_cly_rrZ_top->Write();
	sprintf(canvasfile,"%s/%s_comparison_ly_rrZ_top.%s", workingdirectory.c_str(),suffix.c_str(),export_format.c_str());
	if (!(export_format=="")) c_cly_rrZ_top->SaveAs(canvasfile);
	
	/*=================================================================*/
	// comparison ly of R^{2} vs. Z BOTTOM PMTs
	/*=================================================================*/
	gStyle->SetPalette(NCont_Sym,ColPalette_Sym);
	style_2D_Sym->cd();
	TCanvas *c_cly_rrZ_bottom = new TCanvas("cly_rrZ_bottom","cly_rrZ_bottom",canvas_x,canvas_y);
	TH2F* h_cly_rrZ_bottom = new TH2F("cly_rrZ_bottom", "ly of R^{2} vs. Z (BOTTOM PMTs) of ^{83m}Kr vs. MC", TPC.Get_nbinsRR(), TPC.Get_LXe_minRR(), TPC.Get_LXe_maxRR(), TPC.Get_nbinsZ(), TPC.Get_LXe_minZ(), TPC.Get_LXe_maxZ());
	h_cly_rrZ_bottom->SetXTitle("R^{2} [cm^{2}]");
	h_cly_rrZ_bottom->GetXaxis()->CenterTitle();
	h_cly_rrZ_bottom->SetYTitle("Z [cm]");
	h_cly_rrZ_bottom->GetYaxis()->CenterTitle();
	h_cly_rrZ_bottom->SetZTitle("ly ^{83m}Kr - ly MC [pe/keV]");
	h_cly_rrZ_bottom->GetZaxis()->CenterTitle();
	h_cly_rrZ_bottom->Add(h_ly_rrZ_bottom,-1);
	h_cly_rrZ_bottom->Add(h_ly_map_bottom,1);
	h_cly_rrZ_bottom->Draw("colz");
	if ( (abs(h_cly_rrZ_bottom->GetMaximum()) > abs(h_cly_rrZ_bottom->GetMinimum())) && (h_cly_rrZ_bottom->GetMaximum() > 0.) )
		{h_cly_rrZ_bottom->SetMinimum(-abs(h_cly_rrZ_bottom->GetMaximum()));}
	else if ( (abs(h_cly_rrZ_bottom->GetMaximum()) < abs(h_cly_rrZ_bottom->GetMinimum())) && (h_cly_rrZ_bottom->GetMinimum() < 0.) ) 
		{h_cly_rrZ_bottom->SetMaximum(abs(h_cly_rrZ_bottom->GetMinimum()));}
	// else do nothing; 
	if (file_outplot) c_cly_rrZ_bottom->Write();
	sprintf(canvasfile,"%s/%s_comparison_ly_rrZ_bottom.%s", workingdirectory.c_str(),suffix.c_str(),export_format.c_str());
	if (!(export_format=="")) c_cly_rrZ_bottom->SaveAs(canvasfile);
	
	/*=================================================================*/
	// relative LCE of R^{2} vs. Z
	/*=================================================================*/
	gStyle->SetPalette(NCont,ColPalette);
	style_2D->cd();
	TCanvas *c_rLCE_rrZ = new TCanvas("rLCE_rrZ","rLCE_rrZ",canvas_x,canvas_y);
	TH2F* h_rLCE_rrZ = new TH2F("rLCE_rrZ", "relative LCE of R^{2} vs. Z (ALL PMTs)", TPC.Get_nbinsRR(), TPC.Get_LXe_minRR(), TPC.Get_LXe_maxRR(), TPC.Get_nbinsZ(), TPC.Get_LXe_minZ(), TPC.Get_LXe_maxZ());
	h_rLCE_rrZ->SetXTitle("R^{2} [cm^{2}]");
	h_rLCE_rrZ->GetXaxis()->CenterTitle();
	h_rLCE_rrZ->SetYTitle("Z [cm]");
	h_rLCE_rrZ->GetYaxis()->CenterTitle();
	h_rLCE_rrZ->SetZTitle("relative LCE");
	h_rLCE_rrZ->GetZaxis()->CenterTitle();
	h_rLCE_rrZ->Sumw2();
	h_rLCE_rrZ->Divide(h_rrZ_det, h_rrZ, 1.,1., "b");
	h_rLCE_rrZ->Scale(100.);
	double h_LCE_rrZ_mean = 0;
	for (int z=0; z<TPC.Get_nbinsZ(); z++){
		for (int r=0; r<TPC.Get_nbinsRR() ; r++){
				h_LCE_rrZ_mean += h_rLCE_rrZ->GetBinContent(r+1,TPC.Get_nbinsZ()-z)/(TPC.Get_nbinsRR()*TPC.Get_nbinsZ());
		}
	}
	h_rLCE_rrZ->Scale(1./h_LCE_rrZ_mean);
	h_rLCE_rrZ->SetMaximum(1.5);
	h_rLCE_rrZ->SetMinimum(0.5);
	h_rLCE_rrZ->Draw("colz");
	if (file_outplot) c_rLCE_rrZ->Write();
	sprintf(canvasfile,"%s/%s_comparison_rLCE_rrZ_MC.%s", workingdirectory.c_str(),suffix.c_str(),export_format.c_str());
	if (!(export_format=="")) c_rLCE_rrZ->SaveAs(canvasfile);
	
	/*=================================================================*/
	// relative LCE of R^{2} vs. Z TOP
	/*=================================================================*/
	style_2D->cd();
	TCanvas *c_rLCE_rrZ_top = new TCanvas("rLCE_rrZ_top","rLCE_rrZ_top",canvas_x,canvas_y);
	TH2F* h_rLCE_rrZ_top = new TH2F("rLCE_rrZ_top", "relative LCE of R^{2} vs. Z (TOP PMTs)", TPC.Get_nbinsRR(), TPC.Get_LXe_minRR(), TPC.Get_LXe_maxRR(), TPC.Get_nbinsZ(), TPC.Get_LXe_minZ(), TPC.Get_LXe_maxZ());
	h_rLCE_rrZ_top->SetXTitle("R^{2} [cm^{2}]");
	h_rLCE_rrZ_top->GetXaxis()->CenterTitle();
	h_rLCE_rrZ_top->SetYTitle("Z [cm]");
	h_rLCE_rrZ_top->GetYaxis()->CenterTitle();
	h_rLCE_rrZ_top->SetZTitle("relative LCE");
	h_rLCE_rrZ_top->GetZaxis()->CenterTitle();
	h_rLCE_rrZ_top->Sumw2();
	h_rLCE_rrZ_top->Divide(h_rrZ_det_top, h_rrZ, 1.,1., "b");
	h_rLCE_rrZ_top->Scale(100.);
	h_rLCE_rrZ_top->Scale(1./h_LCE_rrZ_mean);
	h_rLCE_rrZ_top->SetMaximum(0.4);
	h_rLCE_rrZ_top->SetMinimum(0);
	h_rLCE_rrZ_top->Draw("colz");
	if (file_outplot) c_rLCE_rrZ_top->Write();
	sprintf(canvasfile,"%s/%s_comparison_rLCE_rrZ_top_MC.%s", workingdirectory.c_str(),suffix.c_str(),export_format.c_str());
	if (!(export_format=="")) c_rLCE_rrZ_top->SaveAs(canvasfile);
	
	/*=================================================================*/
	// relative LCE of R^{2} vs. Z BOTTOM
	/*=================================================================*/
	style_2D->cd();
	TCanvas *c_rLCE_rrZ_bottom = new TCanvas("rLCE_rrZ_bottom","rLCE_rrZ_bottom",canvas_x,canvas_y);
	TH2F* h_rLCE_rrZ_bottom = new TH2F("rLCE_rrZ_bottom", "relative LCE of R^{2} vs. Z (BOTTOM PMTs)", TPC.Get_nbinsRR(), TPC.Get_LXe_minRR(), TPC.Get_LXe_maxRR(), TPC.Get_nbinsZ(), TPC.Get_LXe_minZ(), TPC.Get_LXe_maxZ());
	h_rLCE_rrZ_bottom->SetXTitle("R^{2} [cm^{2}]");
	h_rLCE_rrZ_bottom->GetXaxis()->CenterTitle();
	h_rLCE_rrZ_bottom->SetYTitle("Z [cm]");
	h_rLCE_rrZ_bottom->GetYaxis()->CenterTitle();
	h_rLCE_rrZ_bottom->SetZTitle("relative LCE");
	h_rLCE_rrZ_bottom->GetZaxis()->CenterTitle();
	h_rLCE_rrZ_bottom->Sumw2();
	h_rLCE_rrZ_bottom->Divide(h_rrZ_det_bottom, h_rrZ, 1.,1., "b");
	h_rLCE_rrZ_bottom->Scale(100.);
	h_rLCE_rrZ_bottom->Scale(1./h_LCE_rrZ_mean);
	h_rLCE_rrZ_bottom->SetMaximum(1.4);
	h_rLCE_rrZ_bottom->SetMinimum(0.4);
	h_rLCE_rrZ_bottom->Draw("colz");
	if (file_outplot) c_rLCE_rrZ_bottom->Write();
	sprintf(canvasfile,"%s/%s_comparison_rLCE_rrZ_bottom_MC.%s", workingdirectory.c_str(),suffix.c_str(),export_format.c_str());
	if (!(export_format=="")) c_rLCE_rrZ_bottom->SaveAs(canvasfile);
	
	/*=================================================================*/
	gROOT->SetBatch(kFALSE);
	/*=================================================================*/
	/*=================================================================*/
	// comparison relative LCE of R^{2} vs. Z
	/*=================================================================*/
	gStyle->SetPalette(NCont_Sym,ColPalette_Sym);
	style_2D_Sym->cd();
	TCanvas *c_crLCE_rrZ = new TCanvas("crLCE_rrZ","crLCE_rrZ",canvas_x,canvas_y);
	TH2F* h_crLCE_rrZ = new TH2F("crLCE_rrZ", "relative LCE of R^{2} vs. Z (ALL PMTs) of ^{83m}Kr vs MC", TPC.Get_nbinsRR(), TPC.Get_LXe_minRR(), TPC.Get_LXe_maxRR(), TPC.Get_nbinsZ(), TPC.Get_LXe_minZ(), TPC.Get_LXe_maxZ());
	h_crLCE_rrZ->SetXTitle("R^{2} [cm^{2}]");
	h_crLCE_rrZ->GetXaxis()->CenterTitle();
	h_crLCE_rrZ->SetYTitle("Z [cm]");
	h_crLCE_rrZ->GetYaxis()->CenterTitle();
	h_crLCE_rrZ->SetZTitle("rLCE ^{83m}Kr - rLCE MC");
	h_crLCE_rrZ->GetZaxis()->CenterTitle();
	h_crLCE_rrZ->Add(h_rLCE_rrZ,-1);
	h_crLCE_rrZ->Add(h_rLCE_map,1);
	h_crLCE_rrZ->Draw("colz");
	if ( (abs(h_crLCE_rrZ->GetMaximum()) > abs(h_crLCE_rrZ->GetMinimum())) && (h_crLCE_rrZ->GetMaximum() > 0.) )
		{h_crLCE_rrZ->SetMinimum(-abs(h_crLCE_rrZ->GetMaximum()));}
	else if ( (abs(h_crLCE_rrZ->GetMaximum()) < abs(h_crLCE_rrZ->GetMinimum())) && (h_crLCE_rrZ->GetMinimum() < 0.) ) 
		{h_crLCE_rrZ->SetMaximum(abs(h_crLCE_rrZ->GetMinimum()));}
	// else do nothing; 
	if (file_outplot) c_crLCE_rrZ->Write();
	sprintf(canvasfile,"%s/%s_comparison_rLCE_rrZ.%s", workingdirectory.c_str(),suffix.c_str(),export_format.c_str());
	if (!(export_format=="")) c_crLCE_rrZ->SaveAs(canvasfile);
	
	/*=================================================================*/
	gROOT->SetBatch(kTRUE);
	/*=================================================================*/
	/*=================================================================*/
	// comparison relative LCE of R^{2} vs. Z TOP
	/*=================================================================*/
	style_2D_Sym->cd();
	TCanvas *c_crLCE_rrZ_top = new TCanvas("crLCE_rrZ_top","crLCE_rrZ_top",canvas_x,canvas_y);
	TH2F* h_crLCE_rrZ_top = new TH2F("crLCE_rrZ_top", "relative LCE of R^{2} vs. Z (TOP PMTs) of ^{83m}Kr vs MC", TPC.Get_nbinsRR(), TPC.Get_LXe_minRR(), TPC.Get_LXe_maxRR(), TPC.Get_nbinsZ(), TPC.Get_LXe_minZ(), TPC.Get_LXe_maxZ());
	h_crLCE_rrZ_top->SetXTitle("R^{2} [cm^{2}]");
	h_crLCE_rrZ_top->GetXaxis()->CenterTitle();
	h_crLCE_rrZ_top->SetYTitle("Z [cm]");
	h_crLCE_rrZ_top->GetYaxis()->CenterTitle();
	h_crLCE_rrZ_top->SetZTitle("rLCE ^{83m}Kr - rLCE MC");
	h_crLCE_rrZ_top->GetZaxis()->CenterTitle();
	h_crLCE_rrZ_top->Add(h_rLCE_rrZ_top,-1);
	h_crLCE_rrZ_top->Add(h_rLCE_map_top,1);
	h_crLCE_rrZ_top->Draw("colz");
	if ( (abs(h_crLCE_rrZ_top->GetMaximum()) > abs(h_crLCE_rrZ_top->GetMinimum())) && (h_crLCE_rrZ_top->GetMaximum() > 0.) )
		{h_crLCE_rrZ_top->SetMinimum(-abs(h_crLCE_rrZ_top->GetMaximum()));}
	else if ( (abs(h_crLCE_rrZ_top->GetMaximum()) < abs(h_crLCE_rrZ_top->GetMinimum())) && (h_crLCE_rrZ_top->GetMinimum() < 0.) ) 
		{h_crLCE_rrZ_top->SetMaximum(abs(h_crLCE_rrZ_top->GetMinimum()));}
	// else do nothing;
	if (file_outplot) c_crLCE_rrZ_top->Write();
	sprintf(canvasfile,"%s/%s_comparison_rLCE_rrZ_top.%s", workingdirectory.c_str(),suffix.c_str(),export_format.c_str());
	if (!(export_format=="")) c_crLCE_rrZ_top->SaveAs(canvasfile);
	
	/*=================================================================*/
	// comparison relative LCE of R^{2} vs. Z BOTTOM
	/*=================================================================*/
	style_2D_Sym->cd();
	TCanvas *c_crLCE_rrZ_bottom = new TCanvas("crLCE_rrZ_bottom","crLCE_rrZ_bottom",canvas_x,canvas_y);
	TH2F* h_crLCE_rrZ_bottom = new TH2F("crLCE_rrZ_bottom", "relative LCE of R^{2} vs. Z (BOTTOM PMTs) of ^{83m}Kr vs MC", TPC.Get_nbinsRR(), TPC.Get_LXe_minRR(), TPC.Get_LXe_maxRR(), TPC.Get_nbinsZ(), TPC.Get_LXe_minZ(), TPC.Get_LXe_maxZ());
	h_crLCE_rrZ_bottom->SetXTitle("R^{2} [cm^{2}]");
	h_crLCE_rrZ_bottom->GetXaxis()->CenterTitle();
	h_crLCE_rrZ_bottom->SetYTitle("Z [cm]");
	h_crLCE_rrZ_bottom->GetYaxis()->CenterTitle();
	h_crLCE_rrZ_bottom->SetZTitle("rLCE ^{83m}Kr - rLCE MC");
	h_crLCE_rrZ_bottom->GetZaxis()->CenterTitle();
	h_crLCE_rrZ_bottom->Add(h_rLCE_rrZ_bottom,-1);
	h_crLCE_rrZ_bottom->Add(h_rLCE_map_bottom,1);
	h_crLCE_rrZ_bottom->Draw("colz");
	if ( (abs(h_crLCE_rrZ_bottom->GetMaximum()) > abs(h_crLCE_rrZ_bottom->GetMinimum())) && (h_crLCE_rrZ_bottom->GetMaximum() > 0.) )
		{h_crLCE_rrZ_bottom->SetMinimum(-abs(h_crLCE_rrZ_bottom->GetMaximum()));}
	else if ( (abs(h_crLCE_rrZ_bottom->GetMaximum()) < abs(h_crLCE_rrZ_bottom->GetMinimum())) && (h_crLCE_rrZ_bottom->GetMinimum() < 0.) ) 
		{h_crLCE_rrZ_bottom->SetMaximum(abs(h_crLCE_rrZ_bottom->GetMinimum()));}
	// else do nothing; 
	if (file_outplot) c_crLCE_rrZ_bottom->Write();
	sprintf(canvasfile,"%s/%s_comparison_rLCE_rrZ_bottom.%s", workingdirectory.c_str(),suffix.c_str(),export_format.c_str());
	if (!(export_format=="")) c_crLCE_rrZ_bottom->SaveAs(canvasfile);
	
	/*=================================================================*/
	// relative LCE of R^{2} vs. Z 3D
	/*=================================================================*/
	style_3D->cd();
	TH2F* h_rLCE_rrZ_3D = new TH2F("rLCE_rrZ_3D", "relative LCE of R^{2} vs. Z (ALL PMTs)", TPC.Get_nbinsZ(), TPC.Get_LXe_minZ(), TPC.Get_LXe_maxZ(), TPC.Get_nbinsRR(), TPC.Get_LXe_minRR(), TPC.Get_LXe_maxRR());
	for (int z=0; z<TPC.Get_nbinsZ(); z++){
		for (int r=0; r<TPC.Get_nbinsRR(); r++){
				h_rLCE_rrZ_3D->SetBinContent(z+1,r+1,h_rLCE_rrZ->GetBinContent(r+1,z+1));
		}
	}
	
	/*=================================================================*/
	// comparison relative LCE of R^{2} vs. Z 3D
	/*=================================================================*/
	gStyle->SetPalette(NCont_Sym,ColPalette_Sym);
	style_3D_Sym->cd();
	TCanvas *c_crLCE_rrZ_3D = new TCanvas("crLCE_rrZ_3D","crLCE_rrZ_3D",canvas_x,canvas_y);
	TH2F* h_crLCE_rrZ_3D = new TH2F("crLCE_rrZ_3D", "relative LCE of R^{2} vs. Z (ALL PMTs) of ^{83m}Kr vs MC", TPC.Get_nbinsZ(), TPC.Get_LXe_minZ(), TPC.Get_LXe_maxZ(), TPC.Get_nbinsRR(), TPC.Get_LXe_minRR(), TPC.Get_LXe_maxRR());
	h_crLCE_rrZ_3D->SetYTitle("R^{2} [cm^{2}]");
	h_crLCE_rrZ_3D->GetYaxis()->CenterTitle();
	h_crLCE_rrZ_3D->SetXTitle("Z [cm]");
	h_crLCE_rrZ_3D->GetXaxis()->CenterTitle();
	h_crLCE_rrZ_3D->SetZTitle("rLCE ^{83m}Kr - rLCE MC");
	h_crLCE_rrZ_3D->GetZaxis()->CenterTitle();
	h_crLCE_rrZ_3D->Add(h_rLCE_rrZ_3D,-1);
	h_crLCE_rrZ_3D->Add(h_rLCE_map_3D,1);
	h_crLCE_rrZ_3D->Draw("surf1");
	if ( (abs(h_crLCE_rrZ_3D->GetMaximum()) > abs(h_crLCE_rrZ_3D->GetMinimum())) && (h_crLCE_rrZ_3D->GetMaximum() > 0.) )
		{h_crLCE_rrZ_bottom->SetMinimum(-abs(h_crLCE_rrZ_bottom->GetMaximum()));}
	else if ( (abs(h_crLCE_rrZ_3D->GetMaximum()) < abs(h_crLCE_rrZ_3D->GetMinimum())) && (h_crLCE_rrZ_3D->GetMinimum() < 0.) ) 
		{h_crLCE_rrZ_bottom->SetMaximum(abs(h_crLCE_rrZ_bottom->GetMinimum()));}
	// else do nothing; 
	c_crLCE_rrZ_3D->SetTheta(20.);
	c_crLCE_rrZ_3D->SetPhi(220.);
	if (file_outplot) c_crLCE_rrZ_3D->Write();
	sprintf(canvasfile,"%s/%s_comparison_rLCE_rrZ_3D.%s", workingdirectory.c_str(),suffix.c_str(),export_format.c_str());
	if (!(export_format=="")) c_crLCE_rrZ_3D->SaveAs(canvasfile);
	
	/*=================================================================*/
	// generated events vs. Z
	/*=================================================================*/
	style_1D->cd();
	TH1F* h_LCEZ_gen = new TH1F("LCEZ_gen", "generated events vs. Z", TPC.Get_nbinsZ(), TPC.Get_LXe_minZ(), TPC.Get_LXe_maxZ());
	sprintf(draw_selection,"zp_pri/10<=%f && zp_pri/10>=%f && rrp_pri>=%f && rrp_pri<=%f",TPC.Get_LXe_maxZ(),TPC.Get_LXe_minZ(),TPC.Get_LXe_minRR(),TPC.Get_LXe_maxRR());
	file_input_tree->Draw("zp_pri/10. >> LCEZ_gen", draw_selection, "goff");
	
	/*=================================================================*/
	// detected events vs. Z (TOP + BOTTOM PMTs)
	/*=================================================================*/
	style_1D->cd();
	TH1F* h_LCEZ_det = new TH1F("LCEZ_det", "detected events vs. Z (ALL PMTs)", TPC.Get_nbinsZ(), TPC.Get_LXe_minZ(), TPC.Get_LXe_maxZ());
	sprintf(draw_selection,"zp_pri/10<=%f && zp_pri/10>=%f && rrp_pri>=%f && rrp_pri<=%f && (nbpmthits > 0 || ntpmthits > 0)",TPC.Get_LXe_maxZ(),TPC.Get_LXe_minZ(),TPC.Get_LXe_minRR(),TPC.Get_LXe_maxRR());
	file_input_tree->Draw("zp_pri/10. >> LCEZ_det", draw_selection, "goff");
	
	/*=================================================================*/
	// detected events vs. Z (TOP PMTs)
	/*=================================================================*/
	style_1D->cd();
	TH1F* h_LCEZ_det_top = new TH1F("LCEZ_det_top", "detected events vs. Z (TOP PMTs)", TPC.Get_nbinsZ(), TPC.Get_LXe_minZ(), TPC.Get_LXe_maxZ());
	sprintf(draw_selection,"zp_pri/10<=%f && zp_pri/10>=%f && rrp_pri>=%f && rrp_pri<=%f && (ntpmthits > 0)",TPC.Get_LXe_maxZ(),TPC.Get_LXe_minZ(),TPC.Get_LXe_minRR(),TPC.Get_LXe_maxRR());
	file_input_tree->Draw("zp_pri/10. >> LCEZ_det_top", draw_selection, "goff");
	
	/*=================================================================*/
	// detected events vs. Z (BOTTOM PMTs)
	/*=================================================================*/
	style_1D->cd();
	TH1F* h_LCEZ_det_bottom = new TH1F("LCEZ_det_bottom", "detected events vs. Z (BOTTOM PMTs)", TPC.Get_nbinsZ(), TPC.Get_LXe_minZ(), TPC.Get_LXe_maxZ());
	sprintf(draw_selection,"zp_pri/10<=%f && zp_pri/10>=%f && rrp_pri>=%f && rrp_pri<=%f && (nbpmthits > 0)",TPC.Get_LXe_maxZ(),TPC.Get_LXe_minZ(),TPC.Get_LXe_minRR(),TPC.Get_LXe_maxRR());
	file_input_tree->Draw("zp_pri/10. >> LCEZ_det_bottom", draw_selection, "goff");
	
	/*=================================================================*/
	// LCE vs. Z
	/*=================================================================*/
	style_1D->cd();
	TH1F* h_LCE_LCEZ = new TH1F("LCE_LCEZ", "LCE vs. Z (ALL PMTs)", TPC.Get_nbinsZ(), TPC.Get_LXe_minZ(), TPC.Get_LXe_maxZ());
	h_LCE_LCEZ->Sumw2();
	h_LCE_LCEZ->Divide(h_LCEZ_det, h_LCEZ_gen, 1.,1., "b");
	h_LCE_LCEZ->Scale(100.);
	
	/*=================================================================*/
	// LCE vs. Z (TOP PMTs)
	/*=================================================================*/
	style_1D->cd();
	TH1F* h_LCE_LCEZ_top = new TH1F("LCE_LCEZ_top", "LCE vs. Z (TOP PMTs)", TPC.Get_nbinsZ(), TPC.Get_LXe_minZ(), TPC.Get_LXe_maxZ());
	h_LCE_LCEZ_top->Sumw2();
	h_LCE_LCEZ_top->Divide(h_LCEZ_det_top, h_LCEZ_gen, 1.,1., "b");
	h_LCE_LCEZ_top->Scale(100.);
	
	/*=================================================================*/
	// LCE vs. Z (BOTTOM PMTs)
	/*=================================================================*/
	style_1D->cd();
	TH1F* h_LCE_LCEZ_bottom = new TH1F("LCE_LCEZ_bottom", "LCE vs. Z (BOTTOM PMTs)", TPC.Get_nbinsZ(), TPC.Get_LXe_minZ(), TPC.Get_LXe_maxZ());
	h_LCE_LCEZ_bottom->Sumw2();
	h_LCE_LCEZ_bottom->Divide(h_LCEZ_det_bottom, h_LCEZ_gen, 1.,1., "b");
	h_LCE_LCEZ_bottom->Scale(100.);
	
	/*=================================================================*/
	// AFT vs. Z
	/*=================================================================*/
	style_1D->cd();
	TH1F* h_AFTZ_MC = new TH1F("AFTZ_MC", "AFT vs. Z", TPC.Get_nbinsZ(), TPC.Get_LXe_minZ(), TPC.Get_LXe_maxZ());
	h_AFTZ_MC->Sumw2();
	h_AFTZ_MC->Divide(h_LCE_LCEZ_top, h_LCE_LCEZ, 1.,1., "b");
	h_AFTZ_MC->Scale(100.);

	/*=================================================================*/
	gROOT->SetBatch(kFALSE);
	/*=================================================================*/
	/*=================================================================*/
	// comparison relative LCE vs. Z
	/*=================================================================*/
	gStyle->SetPalette(NCont,ColPalette);
	style_1D->cd();
	TCanvas *c_crLCEZ_ALL = new TCanvas("crLCEZ_ALL","crLCEZ_ALL",canvas_x,canvas_y);
	c_crLCEZ_ALL->SetGridy();
	double h_rLCE_mean = 0;
	for (int z=0; z<(TPC.Get_nbinsZ()); z++){
		h_rLCE_mean += h_LCE_LCEZ->GetBinContent(z)/(TPC.Get_nbinsZ());
	}
	h_LCE_LCEZ->Scale(1./h_rLCE_mean);
	h_LCE_LCEZ->SetXTitle("Z [cm]");
	h_LCE_LCEZ->GetXaxis()->CenterTitle();
	h_LCE_LCEZ->SetYTitle("relative LCE");
	h_LCE_LCEZ->GetYaxis()->CenterTitle();
	h_LCE_LCEZ->Draw();
	h_LCE_LCEZ_bottom->Scale(1./h_rLCE_mean);
	h_LCE_LCEZ_bottom->Draw();
	h_LCE_LCEZ_top->Scale(1./h_rLCE_mean);
	h_LCE_LCEZ_top->Draw();
	
	h_LCE_LCEZ->SetTitle("relative LCE vs. Z of ^{83m}Kr vs. MC");
	h_LCE_LCEZ->SetLineColor(kBlue);
	h_LCE_LCEZ->GetYaxis()->SetRangeUser(0,2.5);
	h_LCE_LCEZ->Draw("");
	h_LCE_LCEZ_bottom->SetLineColor(kGreen);
	h_LCE_LCEZ_bottom->Draw("same");
	h_LCE_LCEZ_top->SetLineColor(kRed);
	h_LCE_LCEZ_top->Draw("same");
	
	h_rLCE_LCEZ->SetLineColor(kBlue);
	h_rLCE_LCEZ->SetMarkerColor(kBlue);
	h_rLCE_LCEZ->SetMarkerStyle(8);
	h_rLCE_LCEZ->Draw("P same");
	h_rLCE_LCEZ_top->SetLineColor(kRed);
	h_rLCE_LCEZ_top->SetMarkerColor(kRed);
	h_rLCE_LCEZ_top->SetMarkerStyle(8);
	h_rLCE_LCEZ_top->Draw("P same");
	h_rLCE_LCEZ_bottom->SetLineColor(kGreen);
	h_rLCE_LCEZ_bottom->SetMarkerColor(kGreen);
	h_rLCE_LCEZ_bottom->SetMarkerStyle(8);
	h_rLCE_LCEZ_bottom->Draw("P same");

	TLegend *leg_crLCE = new TLegend(0.45,0.7,0.95,0.935);
	leg_crLCE->SetFillColor(0);
	leg_crLCE->SetTextSize(0.04);
	leg_crLCE->SetTextAlign(22);         
	leg_crLCE->AddEntry(h_rLCE_LCEZ,"^{83m}Kr All PMTs","LP"); 
	leg_crLCE->AddEntry(h_rLCE_LCEZ_top,"^{83m}Kr Top PMTs","LP");
	leg_crLCE->AddEntry(h_rLCE_LCEZ_bottom,"^{83m}Kr Bottom PMTs","LP"); 
	leg_crLCE->AddEntry(h_LCE_LCEZ,"MC All PMTs","l"); 
	leg_crLCE->AddEntry(h_LCE_LCEZ_top,"MC Top PMTs","l");
	leg_crLCE->AddEntry(h_LCE_LCEZ_bottom,"MC Bottom PMTs","l"); 
	leg_crLCE->Draw();    

	if (file_outplot) c_crLCEZ_ALL->Write();	
	sprintf(canvasfile,"%s/%s_comparison_rLCEZ.%s", workingdirectory.c_str(),suffix.c_str(),export_format.c_str());
	if (!(export_format=="")) c_crLCEZ_ALL->SaveAs(canvasfile);
	
	/*=================================================================*/
	// comparison AFT vs. Z
	/*=================================================================*/
	gStyle->SetPalette(NCont,ColPalette);
	style_1D->cd();
	TCanvas *c_cAFTZ = new TCanvas("cAFTZ","cAFTZ",canvas_x,canvas_y);
	c_cAFTZ->SetGridy();
	h_AFTZ_MC->SetTitle("AreaFractionTop vs. Z of ^{83m}Kr vs. MC");
	h_AFTZ_MC->SetXTitle("Z [cm]");
	h_AFTZ_MC->GetXaxis()->CenterTitle();
	h_AFTZ_MC->SetYTitle("AreaFractionTop [%]");
	h_AFTZ_MC->GetYaxis()->CenterTitle();
	h_AFTZ_MC->SetLineColor(kRed);
	h_AFTZ_MC->GetYaxis()->SetRangeUser(0,50);
	h_AFTZ_MC->Draw("");
	
	h_AFTZ_Kr->SetLineColor(kRed);
	h_AFTZ_Kr->SetMarkerColor(kRed);
	h_AFTZ_Kr->SetMarkerStyle(8);
	h_AFTZ_Kr->Draw("P same");

	TLegend *leg_cAFTZ = new TLegend(0.55,0.8,0.95,0.935);
	leg_cAFTZ->SetFillColor(0);
	leg_cAFTZ->SetTextSize(0.04);
	leg_cAFTZ->SetTextAlign(22);         
	leg_cAFTZ->AddEntry(h_AFTZ_Kr,"^{83m}Kr data","P"); 
	leg_cAFTZ->AddEntry(h_AFTZ_MC,"MC","l"); 
	leg_cAFTZ->Draw();    

	if (file_outplot) c_cAFTZ->Write();	
	sprintf(canvasfile,"%s/%s_comparison_AFTZ.%s", workingdirectory.c_str(),suffix.c_str(),export_format.c_str());
	if (!(export_format=="")) c_cAFTZ->SaveAs(canvasfile);
	
	/*=================================================================*/
	gROOT->SetBatch(kTRUE);
	/*=================================================================*/
	/*=================================================================*/
	// detected events vs. R^{2} (TOP + BOTTOM PMTs)
	/*=================================================================*/
	style_1D->cd();
	TH1F* h_LCErr_det = new TH1F("LCErr_det", "detected events vs. R^{2} (ALL PMTs)", TPC.Get_nbinsRR(), 0., TPC.Get_LXe_maxRR());
	sprintf(draw_selection,"zp_pri/10<=%f && zp_pri/10>=%f && rrp_pri>=%f && rrp_pri<=%f && (nbpmthits > 0 || ntpmthits > 0)",TPC.Get_LXe_maxZ(),TPC.Get_LXe_minZ(),TPC.Get_LXe_minRR(),TPC.Get_LXe_maxRR());
	file_input_tree->Draw("rrp_pri >> LCErr_det", draw_selection, "goff");
	
	/*=================================================================*/
	// detected events vs. R^{2} (TOP PMTs)
	/*=================================================================*/
	style_1D->cd();
	TH1F* h_LCErr_det_top = new TH1F("LCErr_det_top", "detected events vs. R^{2} (TOP PMTs)", TPC.Get_nbinsRR(), 0., TPC.Get_LXe_maxRR());
	sprintf(draw_selection,"zp_pri/10<=%f && zp_pri/10>=%f && rrp_pri>=%f && rrp_pri<=%f && (ntpmthits > 0)",TPC.Get_LXe_maxZ(),TPC.Get_LXe_minZ(),TPC.Get_LXe_minRR(),TPC.Get_LXe_maxRR());
	file_input_tree->Draw("rrp_pri >> LCErr_det_top", draw_selection, "goff");

	/*=================================================================*/
	// AFT vs. rr
	/*=================================================================*/
	style_1D->cd();
	TH1F* h_AFTrr_MC = new TH1F("AFTrr_MC", "AFT vs. R^{2}", TPC.Get_nbinsR(), 0., TPC.Get_LXe_maxRR());
	h_AFTrr_MC->Sumw2();
	h_AFTrr_MC->Divide(h_LCErr_det_top, h_LCErr_det, 1.,1., "b");
	h_AFTrr_MC->Scale(100.);
	
	/*=================================================================*/
	gROOT->SetBatch(kFALSE);
	/*=================================================================*/
	/*=================================================================*/
	// comparison AFT vs. rr
	/*=================================================================*/
	gStyle->SetPalette(NCont,ColPalette);
	style_1D->cd();
	TCanvas *c_cAFTrr = new TCanvas("cAFTrr","cAFTrr",canvas_x,canvas_y);
	c_cAFTrr->SetGridy();
	h_AFTrr_MC->SetTitle("AreaFractionTop vs. R^{2} of ^{83m}Kr vs. MC");
	h_AFTrr_MC->SetXTitle("R^{2} [cm^{2}]");
	h_AFTrr_MC->GetXaxis()->CenterTitle();
	h_AFTrr_MC->SetYTitle("AreaFractionTop [%]");
	h_AFTrr_MC->GetYaxis()->CenterTitle();
	h_AFTrr_MC->SetLineColor(kRed);
	h_AFTrr_MC->GetYaxis()->SetRangeUser(0,50);
	h_AFTrr_MC->Draw("");
	
	h_AFTrr_Kr->SetLineColor(kRed);
	h_AFTrr_Kr->SetMarkerColor(kRed);
	h_AFTrr_Kr->SetMarkerStyle(8);
	h_AFTrr_Kr->Draw("P same");

	TLegend *leg_cAFTrr = new TLegend(0.55,0.8,0.95,0.935);
	leg_cAFTrr->SetFillColor(0);
	leg_cAFTrr->SetTextSize(0.04);
	leg_cAFTrr->SetTextAlign(22);         
	leg_cAFTrr->AddEntry(h_AFTrr_Kr,"^{83m}Kr data","P"); 
	leg_cAFTrr->AddEntry(h_AFTrr_MC,"MC","l"); 
	leg_cAFTrr->Draw();    

	if (file_outplot) c_cAFTrr->Write();	
	sprintf(canvasfile,"%s/%s_comparison_AFTrr.%s", workingdirectory.c_str(),suffix.c_str(),export_format.c_str());
	if (!(export_format=="")) c_cAFTrr->SaveAs(canvasfile);
	
	gStyle->SetPalette(NCont,ColPalette);
		
}