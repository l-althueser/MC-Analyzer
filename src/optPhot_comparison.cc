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
#include "TPaveText.h"
#include "TStyle.h"
#include "TFile.h"
#include "TLine.h"
#include "TColor.h"
#include "TEntryList.h"
#include "TGaxis.h"
#include "TError.h"

using namespace std;

void optPhot_comparison(string, double, string, int, int, int, string, string);
void optPhot_comparison(string, string, double, string, int, int, int, string, string);
void optPhot_comparison(string, string, double, string, int, int, int, string, string, bool);

/*=================================================================*/

void optPhot_comparison(string datafile_kr, double AFT_S2_Kr, string datafile_mc, int bin_z, int bin_r, int bin_rr, string strnbinst, string export_format) {
	optPhot_comparison(datafile_kr,"",AFT_S2_Kr,datafile_mc,bin_z,bin_r,bin_rr,strnbinst,export_format,true);
}

void optPhot_comparison(string datafile_kr, string datafile_PMT, double AFT_S2_Kr, string datafile_mc, int bin_z, int bin_r, int bin_rr, string strnbinst, string export_format) {
	optPhot_comparison(datafile_kr,datafile_PMT,AFT_S2_Kr,datafile_mc,bin_z,bin_r,bin_rr,strnbinst,export_format,true);
}

/*=================================================================*/
//.x ../src/optPhot_comparison.cc++("./comparison_perPMT_QE/Xe_Kr83m.txt","./comparison_perPMT_QE/Xe_Kr83m_PMT.ini",0.645,"./comparison_perPMT_QE/MC_Xe_TPC_optPhot_S1_1e5.root",9,4,4,"4 6 8 12","png");
void optPhot_comparison(string datafile_kr, string datafile_PMT, double AFT_S2_Kr, string datafile_mc, int bin_z, int bin_r, int bin_rr, string strnbinst, string export_format, bool batch) {
	
	//gErrorIgnoreLevel = kPrint, kInfo, kWarning, kError, kBreak, kSysError, kFatal;
	gErrorIgnoreLevel = kWarning;
	
	if (fileexists(datafile_kr) == false) {
		cout << endl;
		cout << "x Error xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx" << endl;
		cout << "file not found:" << endl;
		cout << "-> " << datafile_kr << endl;
		cout << "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx" << endl;
		cout << endl;
		gApplication->Terminate();
	}
	
	if (fileexists(datafile_PMT) == false) {
		cout << endl;
		cout << "x WARNING xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx" << endl;
		cout << "PMT definition file not found (or defined):" << endl;
		cout << "-> " << datafile_PMT << endl;
		cout << "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx" << endl;
		cout << endl;
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
	string datafilename_mc = datafile_mc.substr(found+1);
	string rawdatafilename_mc = "";
	size_t lastindex = 0;
	if (datafilename_mc == "") {
		rawdatafilename_mc = "optPhot";
	} else {
		lastindex = datafilename_mc.find_last_of("."); 
		rawdatafilename_mc = datafilename_mc.substr(0, lastindex); 
	}
	
	found=datafile_kr.find_last_of("/\\");
	string datafilename_kr = datafile_kr.substr(found+1);
	lastindex = datafilename_kr.find_last_of("."); 
	string rawdatafilename_kr = datafilename_kr.substr(0, lastindex); 
	
	sprintf(file_outname,"%s/comparison_%s_vs_%s.root", workingdirectory.c_str(), rawdatafilename_mc.c_str(), rawdatafilename_kr.c_str());
	file_outplot = new TFile(file_outname,"RECREATE");
	
	TPC_Definition TPC;
	TPC.Set_Bins(bin_z,bin_r,bin_rr);

	stringstream stream(strnbinst);
	std::vector<int> nbinst;
	int element;
	while(stream >> element){
		nbinst.push_back(element);
	}

	double AFT_S2 = 0;
	double AFT_S2_ratio = 0;
	
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
	style_1D->SetTitleBorderSize(1);
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
	style_2D->SetTitleBorderSize(1);
	style_2D->SetOptStat(0);
	style_2D->SetPadLeftMargin(0.105);
	style_2D->SetPadRightMargin(0.165);
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
	style_3D->SetTitleBorderSize(1);
	style_3D->SetOptStat(0);
	style_3D->SetPadLeftMargin(0.15);
	style_3D->SetPadRightMargin(0.05);
	style_3D->SetPadTopMargin(0.075);
	style_3D->SetPadBottomMargin(0.075);
	
	style_3D->SetTitleOffset(1.75,"X");
	style_3D->SetTitleOffset(1.75,"Y");
	style_3D->SetTitleOffset(2.25,"Z");
	
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
	style_2D_Sym->SetTitleBorderSize(1);
	style_2D_Sym->SetOptStat(0);
	style_2D_Sym->SetPadLeftMargin(0.105);
	style_2D_Sym->SetPadRightMargin(0.165);
	style_2D_Sym->SetPadTopMargin(0.075);
	style_2D_Sym->SetPadBottomMargin(0.075);
	
	style_2D_Sym->SetTitleOffset(1.,"X");
	style_2D_Sym->SetTitleOffset(1.45,"Y");
	style_2D_Sym->SetTitleOffset(1.6,"Z");
	
	style_2D_Sym->SetPalette(NCont_Sym,ColPalette_Sym);
	style_2D_Sym->SetNumberContours(NCont_Sym);
	style_2D_Sym->cd();
	
	TStyle *style_3D_Sym = new TStyle("3D_Sym","3D_Sym");
	style_3D_Sym->SetCanvasColor(10);
	style_3D_Sym->SetTitleFillColor(0);
	style_3D_Sym->SetTitleBorderSize(1);
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
	const char* const DELIMITER_t = " \t";
	char* token[100] = {}; // initialize to 0
	
	// store LCE data
	float ly[200][200]={0};
	float lyZ[200]={0};
	float lyrr[200]={0};
	float lysigma[200][200]={0};
	float lyareatop[200][200]={0};
	float lyareatopZ[200]={0};
	float lyareatoprr[200]={0};
	float lyareatopsigma[200][200]={0};
	
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
			lysigma[atoi(token[0])][atoi(token[2])] += atof(token[8])/nbinst[atoi(token[2])];

			lyareatop[atoi(token[0])][atoi(token[2])] += atof(token[10])/nbinst[atoi(token[2])];
			lyareatopZ[atoi(token[0])] += atof(token[10])/(nbinst[atoi(token[2])]*TPC.Get_nbinsR());
			lyareatoprr[atoi(token[2])] += atof(token[10])/(nbinst[atoi(token[2])]*TPC.Get_nbinsZ());
			lyareatopsigma[atoi(token[0])][atoi(token[2])] += atof(token[12])/nbinst[atoi(token[2])]; // same as lysigma @20161129
		}
	raw.close();
	
	cout << "============================================================" << endl;
	
	/*=================================================================*/
	gROOT->SetBatch(kTRUE);
	/*=================================================================*/
	/*=================================================================*/
	// generate ly sigma map
	/*=================================================================*/
	style_2D->cd();
	TCanvas *c_ly_sigma_map = new TCanvas("ly_sigma_map","ly_sigma_map",canvas_x,canvas_y);
	TH2F* h_ly_sigma_map = new TH2F("ly_sigma_map", "^{83m}Kr: ly sigma map", TPC.Get_nbinsR(), TPC.Get_LXe_minRR(), TPC.Get_LXe_maxRR(), TPC.Get_nbinsZ(), TPC.Get_LXe_minZ(), TPC.Get_LXe_maxZ());
	h_ly_sigma_map->SetXTitle("R^{2} [cm^{2}]");
	h_ly_sigma_map->GetXaxis()->CenterTitle();
	h_ly_sigma_map->SetYTitle("Z [cm]");
	h_ly_sigma_map->GetYaxis()->CenterTitle();
	h_ly_sigma_map->SetZTitle("ly sigma");
	h_ly_sigma_map->GetZaxis()->CenterTitle();
	for (int z=0; z<(TPC.Get_nbinsZ()); z++){
		for (int r=0; r<TPC.Get_nbinsR() ; r++){
			h_ly_sigma_map->SetBinContent(r+1,TPC.Get_nbinsZ()-z,lysigma[z][r]);
		}
	}
	h_ly_sigma_map->Draw("colz");
	if (file_outplot) c_ly_sigma_map->Write();
	//sprintf(canvasfile,"%s/%s_comparison_ly_sigma_rrZ_Kr.%s", workingdirectory.c_str(),suffix.c_str(),export_format.c_str());
	//if (!(export_format=="")) c_ly_sigma_map->SaveAs(canvasfile);
	
	/*=================================================================*/
	gROOT->SetBatch(kTRUE);
	/*=================================================================*/
	/*=================================================================*/
	// generate ly map
	/*=================================================================*/
	style_2D->cd();
	TCanvas *c_ly_map = new TCanvas("ly_map","ly_map",canvas_x,canvas_y);
	TH2F* h_ly_map = new TH2F("ly_map", "^{83m}Kr: ly map (All PMTs)", TPC.Get_nbinsR(), TPC.Get_LXe_minRR(), TPC.Get_LXe_maxRR(), TPC.Get_nbinsZ(), TPC.Get_LXe_minZ(), TPC.Get_LXe_maxZ());
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
	sprintf(canvasfile,"%s/comparison_%s_vs_%s_rrZ_Kr_ly.%s", workingdirectory.c_str(), rawdatafilename_mc.c_str(), rawdatafilename_kr.c_str(),export_format.c_str());
	if (!(export_format=="")) c_ly_map->SaveAs(canvasfile);
	
	/*=================================================================*/
	// generate ly map TOP PMTs
	/*=================================================================*/
	style_2D->cd();
	TCanvas *c_ly_map_top = new TCanvas("ly_map_top","ly_map_top",canvas_x,canvas_y);
	TH2F* h_ly_map_top = new TH2F("ly_map_top", "^{83m}Kr: ly map (TOP PMTs)", TPC.Get_nbinsR(), TPC.Get_LXe_minRR(), TPC.Get_LXe_maxRR(), TPC.Get_nbinsZ(), TPC.Get_LXe_minZ(), TPC.Get_LXe_maxZ());
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
	sprintf(canvasfile,"%s/comparison_%s_vs_%s_rrZ_Kr_ly_top.%s", workingdirectory.c_str(), rawdatafilename_mc.c_str(), rawdatafilename_kr.c_str(),export_format.c_str());
	if (!(export_format=="")) c_ly_map_top->SaveAs(canvasfile);
	
	/*=================================================================*/
	// generate ly map BOTTOM PMTs
	/*=================================================================*/
	style_2D->cd();
	TCanvas *c_ly_map_bottom = new TCanvas("ly_map_bottom","ly_map_bottom",canvas_x,canvas_y);
	TH2F* h_ly_map_bottom = new TH2F("ly_map_bottom", "^{83m}Kr: ly map (BOTTOM PMTs)", TPC.Get_nbinsR(), TPC.Get_LXe_minRR(), TPC.Get_LXe_maxRR(), TPC.Get_nbinsZ(), TPC.Get_LXe_minZ(), TPC.Get_LXe_maxZ());
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
	sprintf(canvasfile,"%s/comparison_%s_vs_%s_rrZ_Kr_ly_bottom.%s", workingdirectory.c_str(), rawdatafilename_mc.c_str(), rawdatafilename_kr.c_str(),export_format.c_str());
	if (!(export_format=="")) c_ly_map_bottom->SaveAs(canvasfile);
	
	/*=================================================================*/
	// generate relative LCE map
	/*=================================================================*/
	style_2D->cd();
	TCanvas *c_rLCE_map = new TCanvas("rLCE_map","rLCE_map",canvas_x,canvas_y);
	TH2F* h_rLCE_map = new TH2F("rLCE_map", "^{83m}Kr: relative LCE map (ALL PMTs)", TPC.Get_nbinsR(), TPC.Get_LXe_minRR(), TPC.Get_LXe_maxRR(), TPC.Get_nbinsZ(), TPC.Get_LXe_minZ(), TPC.Get_LXe_maxZ());
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
	cout << "------------------------------------------------------------" << endl;
	cout << "Mean Kr83m ly: " << h_rLCE_map_mean << endl;
	cout << "------------------------------------------------------------" << endl;
	h_rLCE_map->Scale(1./h_rLCE_map_mean);
	h_rLCE_map->SetMaximum(1.5);
	h_rLCE_map->SetMinimum(0.5);
	h_rLCE_map->Draw("colz");
	if (file_outplot) c_rLCE_map->Write();
	sprintf(canvasfile,"%s/comparison_%s_vs_%s_rrZ_Kr_rLCE.%s", workingdirectory.c_str(), rawdatafilename_mc.c_str(), rawdatafilename_kr.c_str(),export_format.c_str());
	if (!(export_format=="")) c_rLCE_map->SaveAs(canvasfile);
	
	/*=================================================================*/
	// generate relative LCE map 3D
	/*=================================================================*/
	style_3D->cd();
	TH2F* h_rLCE_map_3D = new TH2F("rLCE_map_3D", "^{83m}Kr: relative LCE map 3D (All PMTs)", TPC.Get_nbinsZ(), TPC.Get_LXe_minZ(), TPC.Get_LXe_maxZ(), TPC.Get_nbinsR(), TPC.Get_LXe_minRR(), TPC.Get_LXe_maxRR());
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
	TH2F* h_rLCE_map_top = new TH2F("rLCE_map_top", "^{83m}Kr: relative LCE map (TOP PMTs)", TPC.Get_nbinsR(), TPC.Get_LXe_minRR(), TPC.Get_LXe_maxRR(), TPC.Get_nbinsZ(), TPC.Get_LXe_minZ(), TPC.Get_LXe_maxZ());
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
	sprintf(canvasfile,"%s/comparison_%s_vs_%s_rrZ_Kr_rLCE_top.%s", workingdirectory.c_str(), rawdatafilename_mc.c_str(), rawdatafilename_kr.c_str(),export_format.c_str());
	if (!(export_format=="")) c_rLCE_map_top->SaveAs(canvasfile);

	/*=================================================================*/
	// generate relative LCE map BOTTOM PMTs
	/*=================================================================*/
	style_2D->cd();
	TCanvas *c_rLCE_map_bottom = new TCanvas("rLCE_map_bottom","rLCE_map_bottom",canvas_x,canvas_y);
	TH2F* h_rLCE_map_bottom = new TH2F("rLCE_map_bottom", "^{83m}Kr: relative LCE map (BOTTOM PMTs)", TPC.Get_nbinsR(), TPC.Get_LXe_minRR(), TPC.Get_LXe_maxRR(), TPC.Get_nbinsZ(), TPC.Get_LXe_minZ(), TPC.Get_LXe_maxZ());
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
	sprintf(canvasfile,"%s/comparison_%s_vs_%s_rrZ_Kr_rLCE_bottom.%s", workingdirectory.c_str(), rawdatafilename_mc.c_str(), rawdatafilename_kr.c_str(),export_format.c_str());
	if (!(export_format=="")) c_rLCE_map_bottom->SaveAs(canvasfile);
	
	/*=================================================================*/
	// generate relative LCE vs. Z
	/*=================================================================*/
	style_1D->cd();
	TH1F* h_rLCE_LCEZ = new TH1F("rLCE_LCEZ", "^{83m}Kr: relative LCE vs. Z", TPC.Get_nbinsZ(), TPC.Get_LXe_minZ(), TPC.Get_LXe_maxZ());
	h_rLCE_LCEZ->Sumw2();
	double h_rLCE_map_meanZ = 0;
	for (int z=0; z<(TPC.Get_nbinsZ()); z++){
		h_rLCE_LCEZ->SetBinContent(TPC.Get_nbinsZ()-z,lyZ[z]);
		h_rLCE_map_meanZ += lyZ[z]/(TPC.Get_nbinsZ());
	}
	h_rLCE_LCEZ->Scale(1./h_rLCE_map_meanZ);

	TH1F* h_rLCE_LCEZ_top = new TH1F("rLCE_LCEZ_top", "^{83m}Kr: relative LCE vs. Z (TOP PMTs)", TPC.Get_nbinsZ(), TPC.Get_LXe_minZ(), TPC.Get_LXe_maxZ());
	h_rLCE_LCEZ_top->Sumw2();
	for (int z=0; z<(TPC.Get_nbinsZ()); z++){
		h_rLCE_LCEZ_top->SetBinContent(TPC.Get_nbinsZ()-z,lyareatopZ[z]); // (peak.area_fraction_top * peak.area)/32.1498, so (S1Top/S1Total)*S1Total/Energy
	}
	h_rLCE_LCEZ_top->Scale(1./h_rLCE_map_meanZ);

	TH1F* h_rLCE_LCEZ_bottom = new TH1F("rLCE_LCEZ_bottom", "^{83m}Kr: relative LCE vs. Z (BOTTOM PMTs)", TPC.Get_nbinsZ(), TPC.Get_LXe_minZ(), TPC.Get_LXe_maxZ());
	h_rLCE_LCEZ_bottom->Add(h_rLCE_LCEZ,1);
	h_rLCE_LCEZ_bottom->Add(h_rLCE_LCEZ_top,-1);
	
	/*=================================================================*/
	// LCE vs. Z
	/*=================================================================*/
	style_1D->cd();
	TH1F* h_Kr_LCE_LCEZ = new TH1F("Kr_LCE_LCEZ", "^{83m}Kr: LCE vs. Z (All PMTs)", TPC.Get_nbinsZ(), TPC.Get_LXe_minZ(), TPC.Get_LXe_maxZ());
	h_Kr_LCE_LCEZ->Sumw2();
	for (int z=0; z<(TPC.Get_nbinsZ()); z++){
		h_Kr_LCE_LCEZ->SetBinContent(TPC.Get_nbinsZ()-z,lyZ[z]);
	}

	TH1F* h_Kr_LCE_LCEZ_top = new TH1F("Kr_LCE_LCEZ_top", "^{83m}Kr: LCE vs. Z (TOP PMTs)", TPC.Get_nbinsZ(), TPC.Get_LXe_minZ(), TPC.Get_LXe_maxZ());
	h_Kr_LCE_LCEZ_top->Sumw2();
	for (int z=0; z<(TPC.Get_nbinsZ()); z++){
		h_Kr_LCE_LCEZ_top->SetBinContent(TPC.Get_nbinsZ()-z,lyareatopZ[z]); // (peak.area_fraction_top * peak.area)/32.1498, so (S1Top/S1Total)*S1Total/Energy
	}
	
	TH1F* h_Kr_LCE_LCEZ_bottom = new TH1F("Kr_LCE_LCEZ_bottom", "^{83m}Kr: LCE vs. Z (BOTTOM PMTs)", TPC.Get_nbinsZ(), TPC.Get_LXe_minZ(), TPC.Get_LXe_maxZ());
	h_Kr_LCE_LCEZ_bottom->Sumw2();
	for (int z=0; z<(TPC.Get_nbinsZ()); z++){
		h_Kr_LCE_LCEZ_bottom->SetBinContent(TPC.Get_nbinsZ()-z,lyZ[z]-lyareatopZ[z]); // (peak.area_fraction_top * peak.area)/32.1498, so (S1Top/S1Total)*S1Total/Energy
	}
	
	/*=================================================================*/
	// AFT vs. Z
	/*=================================================================*/
	style_1D->cd();
	TH1F* h_AFTZ_Kr = new TH1F("AFTZ_Kr", "^{83m}Kr: AFT vs. Z", TPC.Get_nbinsZ(), TPC.Get_LXe_minZ(), TPC.Get_LXe_maxZ());
	h_AFTZ_Kr->Sumw2();
	h_AFTZ_Kr->Divide(h_Kr_LCE_LCEZ_top, h_Kr_LCE_LCEZ, 1.,1., "b");
	h_AFTZ_Kr->Scale(100.);
	
	/*=================================================================*/
	// LCE vs. rr
	/*=================================================================*/
	style_1D->cd();
	TH1F* h_Kr_LCE_LCErr = new TH1F("Kr_LCE_LCErr", "^{83m}Kr: LCE vs. R^{2} (All PMTs)", TPC.Get_nbinsR(), TPC.Get_LXe_minRR(), TPC.Get_LXe_maxRR());
	h_Kr_LCE_LCErr->Sumw2();
	for (int rr=0; rr<(TPC.Get_nbinsR()); rr++){
		h_Kr_LCE_LCErr->SetBinContent(TPC.Get_nbinsR()-rr,lyrr[rr]);
	}

	TH1F* h_Kr_LCE_LCErr_top = new TH1F("Kr_LCE_LCErr_top", "^{83m}Kr: LCE vs. R^{2} (TOP PMTs)", TPC.Get_nbinsR(), TPC.Get_LXe_minRR(), TPC.Get_LXe_maxRR());
	h_Kr_LCE_LCErr_top->Sumw2();
	for (int rr=0; rr<(TPC.Get_nbinsR()); rr++){
		h_Kr_LCE_LCErr_top->SetBinContent(TPC.Get_nbinsR()-rr,lyareatoprr[rr]); // (peak.area_fraction_top * peak.area)/32.1498, so (S1Top/S1Total)*S1Total/Energy
	}
	
	/*=================================================================*/
	// AFT vs. rr
	/*=================================================================*/
	style_1D->cd();
	TH1F* h_AFTrr_Kr = new TH1F("AFTrr_Kr", "^{83m}Kr: AFT vs. R^{2}", TPC.Get_nbinsR(), TPC.Get_LXe_minRR(), TPC.Get_LXe_maxRR());
	h_AFTrr_Kr->Sumw2();
	h_AFTrr_Kr->Divide(h_Kr_LCE_LCErr_top, h_Kr_LCE_LCErr, 1.,1., "b");
	h_AFTrr_Kr->Scale(100.);
	
	/*=================================================================*/
	if (!batch) {gROOT->SetBatch(kFALSE);}
	/*=================================================================*/
	/*=================================================================*/
	/*=================================================================*/
	// Read in MC data
	/*=================================================================*/
	/*=================================================================*/
	
	TChain *file_input_tree = new TChain("events/events");
	TChain *file_input_tree_S2 = new TChain("events/events");
	TNamed *G4MCname;
	string S1_Tag = "_S1_";
	char filename[10000];
	char filename_S2[10000];
	long nevents = 0;
	long nevents_S2 = 0;
	
	if (fileexists(datafile_mc) == false) {
		cout << endl;
		cout << "x Error xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx" << endl;
		cout << "file not found:" << endl;
		cout << "-> " << datafile_mc << endl;
		cout << "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx" << endl;
		cout << endl;
		gApplication->Terminate();
	}
	else if ((fileexists(datafile_mc) == true) && (datafilename_mc=="")) {
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
				if (!file->IsDirectory() && fname.EndsWith(ext.c_str()) && !(fname.Contains("comparison_")) && !(fname.Contains("_S2_"))) {
					sprintf(filename,"%s/%s", workingdirectory.c_str(), fname.Data());
					
					if (file_input_tree->GetEntries() == 0) {
						TFile *f = new TFile(filename,"READ");
						if (f->GetListOfKeys()->Contains("MC_TAG")) {
							f->GetObject("MC_TAG",G4MCname);
						}
						else {
							G4MCname = new TNamed("MC_TAG","Xenon1t");
						}
						TPC.Init(G4MCname);
						f->Close();
					}
					
					file_input_tree->AddFile(filename); 
					nevents = file_input_tree->GetEntries();
					cout << " file: " << fname.Data() << " " << nevents << " events total" << endl;
					
					// Search for S2 sim
					if (((string)fname).find(S1_Tag) != std::string::npos) {
						string datafilename_S2 = ((string)fname).replace(((string)fname).find(S1_Tag),S1_Tag.length(),"_S2_");
						sprintf(filename_S2,"%s/%s", workingdirectory.c_str(), datafilename_S2.c_str());
						if (fileexists(filename_S2) == true) {
							file_input_tree_S2->AddFile(filename_S2); 
							nevents_S2 = file_input_tree->GetEntries();
							cout << " file: " << datafilename_S2 << " " << nevents_S2 << " events " << endl;
						}
					}
				}
			}
		}		
	}
	else {
		cout << "= reading datafile ===== single file =======================" << endl;
		if (file_input_tree->GetEntries() == 0) {
			TFile *f = new TFile(datafile_mc.c_str(),"READ");
			if (f->GetListOfKeys()->Contains("MC_TAG")) {
				f->GetObject("MC_TAG",G4MCname);
			}
			else {
				G4MCname = new TNamed("MC_TAG","Xenon1t");
			}
			TPC.Init(G4MCname);
			f->Close();
		}
		file_input_tree->AddFile(datafile_mc.c_str()); 
		nevents = file_input_tree->GetEntries();
		cout << " file: " << datafilename_mc << " " << nevents << " events " << endl;
		
		// Search for S2 sim
		if (datafilename_mc.find(S1_Tag) != std::string::npos) {
			string datafilename_S2 = datafilename_mc.replace(datafilename_mc.find(S1_Tag),S1_Tag.length(),"_S2_");
			sprintf(filename_S2,"%s/%s", workingdirectory.c_str(), datafilename_S2.c_str());
			
			if (fileexists(filename_S2) == true) {
				file_input_tree_S2->AddFile(filename_S2); 
				nevents_S2 = file_input_tree->GetEntries();
				cout << " file: " << filename_S2 << " " << nevents_S2 << " events " << endl;
			}
		}
	}
	cout << "============================================================" << endl;
	cout << "= reading PMT.ini ==========================================" << endl;
	
	file_input_tree->SetAlias("rrp_pri","(xp_pri*xp_pri + yp_pri*yp_pri)/10./10.");  
	
	TH2F* check_pmt_details = new TH2F("check_pmt_details", "check_pmt_details", TPC.Get_nbinsRR(), TPC.Get_LXe_minRR(), TPC.Get_LXe_maxRR(), TPC.Get_nbinsZ(), TPC.Get_LXe_minZ(), TPC.Get_LXe_maxZ());
	file_input_tree->Draw("zp_pri/10. : rrp_pri >> check_pmt_details","pmthitID[0]>0","goff");
	bool no_PMT_details = (bool)(check_pmt_details->GetEntries() == 0);
	
	vector<double> QE_PMT;
	vector<double> On_PMT;
	
	if ((datafile_PMT == "") || (no_PMT_details)) {
		cout << endl;
		cout << "x WARNING xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx" << endl;
		cout << "No PMT details in MC or no PMT.ini found." << endl;
		cout << "Assuming average values!" << endl;
		cout << "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx" << endl;
		cout << endl;
		
		// generate PMT values with average QEs and all On
		for (int i = 0; i < TPC.Get_PMTs_top(); i++) {
			QE_PMT.push_back(TPC.Get_QE_top());
			On_PMT.push_back(1.);
		}
		for (int i = TPC.Get_PMTs_top(); i < (TPC.Get_PMTs_top()+TPC.Get_PMTs_bottom()); i++) {
			QE_PMT.push_back(TPC.Get_QE_bottom());
			On_PMT.push_back(1.);
		}
	} else {
		// read PMT.ini file 
		raw.open(datafile_PMT.c_str());
		// check if the file could be opened successfully 
		if(!raw) {cout << "-> Error opening PMT.ini file." << endl; gApplication->Terminate();}
			getline(raw, linebuffer);
			// read the data file line by line
			while (getline(raw, linebuffer)) {
				// parse the line
				char* buf = strdup(linebuffer.c_str());
				token[0] = strtok(buf, DELIMITER_t); // first token
				for (int n = 1; n < 100; n++) {
					token[n] = strtok(0, DELIMITER_t); // subsequent tokens
					if (!token[n]) break; // no more tokens
				}
				if (!(QE_PMT.size() == (unsigned)atoi(token[0]))) {
					cout << endl;
					cout << "x Error xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx" << endl;
					cout << "PMT numbering in bad order!" << endl;
					cout << "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx" << endl;
					cout << endl;
					gApplication->Terminate();
				}
				QE_PMT.push_back(atof(token[1]));
				On_PMT.push_back(atoi(token[2]));
			}
		raw.close();
	}
	
	cout << "============================================================" << endl;
	
	float progress = 0.0;
	int barWidth = 53;
	
	long nbentries = 0;
	Int_t ntpmthits = 0;
	Int_t nbpmthits = 0;
	vector<int> *pmthitID = 0;
	Float_t xp_pri = 0;
	Float_t yp_pri = 0;
	Float_t zp_pri = 0;
	Float_t rrp_pri = 0;
	int pmtID = 0;
	double S2_hits_top = 0;
	double S2_hits_bottom = 0;
	
	nevents_S2 = file_input_tree_S2->GetEntries();

	if (nevents_S2 > 0) {	
		cout << "Calculating maps with per-PMT values (S2):" << endl;
		nbentries = nevents_S2;

		file_input_tree_S2->SetBranchAddress("ntpmthits", &ntpmthits);
		file_input_tree_S2->SetBranchAddress("nbpmthits", &nbpmthits);
		file_input_tree_S2->SetBranchAddress("pmthitID", &pmthitID);
		file_input_tree_S2->SetBranchAddress("xp_pri", &xp_pri);
		file_input_tree_S2->SetBranchAddress("yp_pri", &yp_pri);
		file_input_tree_S2->SetBranchAddress("zp_pri", &zp_pri);
		
		for (long i=0; i<nbentries; i++){
			// little progress bar
			if (i%10000 == 0 || i==0) {
				progress = (float)i/(float)nbentries;
				std::cout << "[";
				int pos = barWidth * progress;
				for (int g = 0; g < barWidth; ++g) {
					if (g < pos) std::cout << "=";
					else if (g == pos) std::cout << ">";
					else std::cout << " ";
				}
				std::cout << "] " << int(progress * 100.0) << " %\r";
				std::cout.flush();
			}
			file_input_tree_S2->GetEntry(i);
			
			// This calculation works only with one simulated photon per event
			if ((ntpmthits+nbpmthits) > 1) {
				cout << endl;
				cout << "x Error xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx" << endl;
				cout << "Only one photon per event is allowed!" << endl;
				cout << "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx" << endl;
				cout << endl;
				gApplication->Terminate();
			}
			if ((ntpmthits+nbpmthits) == 1) {
				if (no_PMT_details) {
					if (ntpmthits == 1) {pmtID = TPC.Get_PMTs_top()-1;}
					else {pmtID = TPC.Get_PMTs_top();}
				} else { pmtID = (*pmthitID)[0]; }
				
				S2_hits_top += ntpmthits*QE_PMT[pmtID]*On_PMT[pmtID];
				S2_hits_bottom += nbpmthits*QE_PMT[pmtID]*On_PMT[pmtID];
			}
		
			AFT_S2 = (S2_hits_top)/(S2_hits_bottom+S2_hits_top);
			AFT_S2_ratio = abs(AFT_S2_Kr - AFT_S2);
		}
		std::cout << std::endl;	
	} else {
		cout << "No S2 file found!" << endl;
	}

	nbentries = file_input_tree->GetEntries();
	
	file_input_tree->SetBranchAddress("ntpmthits", &ntpmthits);
	file_input_tree->SetBranchAddress("nbpmthits", &nbpmthits);
	file_input_tree->SetBranchAddress("pmthitID", &pmthitID);
	file_input_tree->SetBranchAddress("xp_pri", &xp_pri);
	file_input_tree->SetBranchAddress("yp_pri", &yp_pri);
	file_input_tree->SetBranchAddress("zp_pri", &zp_pri);
	
	/*=================================================================*/
	gROOT->SetBatch(kTRUE);
	/*=================================================================*/
	/*=================================================================*/
	// Define all TH2F which will be calculated directly from the TTree
	/*=================================================================*/
	TH2F* h_rrZ = new TH2F("rrZ_pri", "MC: R^{2} vs. Z generated events", TPC.Get_nbinsRR(), TPC.Get_LXe_minRR(), TPC.Get_LXe_maxRR(), TPC.Get_nbinsZ(), TPC.Get_LXe_minZ(), TPC.Get_LXe_maxZ());
	TH2F* h_rrZ_det_top = new TH2F("rrZ_det_top", "MC: R^{2} vs. Z detected events (TOP PMTs)", TPC.Get_nbinsRR(), TPC.Get_LXe_minRR(), TPC.Get_LXe_maxRR(), TPC.Get_nbinsZ(), TPC.Get_LXe_minZ(), TPC.Get_LXe_maxZ());
	TH2F* h_rrZ_det_bottom = new TH2F("rrZ_det_bottom", "MC: R^{2} vs. Z detected events (BOTTOM PMTs)", TPC.Get_nbinsRR(), TPC.Get_LXe_minRR(), TPC.Get_LXe_maxRR(), TPC.Get_nbinsZ(), TPC.Get_LXe_minZ(), TPC.Get_LXe_maxZ());
	TH2F* h_rrZ_det = new TH2F("rrZ_det", "MC: R^{2} vs. Z detected events (ALL PMTs)", TPC.Get_nbinsRR(), TPC.Get_LXe_minRR(), TPC.Get_LXe_maxRR(), TPC.Get_nbinsZ(), TPC.Get_LXe_minZ(), TPC.Get_LXe_maxZ());
	TH1F* h_LCEZ_gen = new TH1F("LCEZ_gen", "MC: generated events vs. Z (All PMTs)", TPC.Get_nbinsZ(), TPC.Get_LXe_minZ(), TPC.Get_LXe_maxZ());
	TH1F* h_LCEZ_det_top = new TH1F("LCEZ_det_top", "MC: detected events vs. Z (TOP PMTs)", TPC.Get_nbinsZ(), TPC.Get_LXe_minZ(), TPC.Get_LXe_maxZ());
	TH1F* h_LCEZ_det_bottom = new TH1F("LCEZ_det_bottom", "MC: detected events vs. Z (BOTTOM PMTs)", TPC.Get_nbinsZ(), TPC.Get_LXe_minZ(), TPC.Get_LXe_maxZ());
	TH1F* h_LCEZ_det = new TH1F("LCEZ_det", "MC: detected events vs. Z (ALL PMTs)", TPC.Get_nbinsZ(), TPC.Get_LXe_minZ(), TPC.Get_LXe_maxZ());
	TH1F* h_LCErr_det_bottom = new TH1F("LCErr_det_bottom", "MC: detected events vs. R^{2} (BOTTOM PMTs)", TPC.Get_nbinsRR(), TPC.Get_LXe_minRR(), TPC.Get_LXe_maxRR());
	TH1F* h_LCErr_det_top = new TH1F("LCErr_det_top", "MC: detected events vs. R^{2} (TOP PMTs)", TPC.Get_nbinsRR(), TPC.Get_LXe_minRR(), TPC.Get_LXe_maxRR());
	TH1F* h_LCErr_det = new TH1F("LCErr_det", "MC: detected events vs. R^{2} (ALL PMTs)", TPC.Get_nbinsRR(), TPC.Get_LXe_minRR(), TPC.Get_LXe_maxRR());

	cout << "Calculating maps with per-PMT values (S1):" << endl;
	progress = 0.0;
	barWidth = 53;
	
	for (long i=0; i<nbentries; i++){
		// little progress bar
		if (i%10000 == 0 || i==0) {
			progress = (float)i/(float)nbentries;
			std::cout << "[";
			int pos = barWidth * progress;
			for (int g = 0; g < barWidth; ++g) {
				if (g < pos) std::cout << "=";
				else if (g == pos) std::cout << ">";
				else std::cout << " ";
			}
			std::cout << "] " << int(progress * 100.0) << " %\r";
			std::cout.flush();
		}
		
		file_input_tree->GetEntry(i);
		rrp_pri = (xp_pri*xp_pri + yp_pri*yp_pri)/10.;
		
		if (!( (zp_pri/10.<=TPC.Get_LXe_maxZ()) && (zp_pri/10.>=TPC.Get_LXe_minZ()) && 
		       (rrp_pri/10.>=TPC.Get_LXe_minRR()) && (rrp_pri/10.<=TPC.Get_LXe_maxRR()) )) {
			// event outside TPC
			continue;
		}
		
		// This calculation works only with one simulated photon per event
		if ((ntpmthits+nbpmthits) > 1) {
			cout << endl;
			cout << "x Error xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx" << endl;
			cout << "Only one photon per event is allowed!" << endl;
			cout << "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx" << endl;
			cout << endl;
			gApplication->Terminate();
		}
		if ((ntpmthits+nbpmthits) == 1) {
			if (no_PMT_details) {
				if (ntpmthits == 1) {pmtID = TPC.Get_PMTs_top()-1;}
				else {pmtID = TPC.Get_PMTs_top();}
			} else { pmtID = (*pmthitID)[0]; }
			
			// If a pmthit is seen
			h_rrZ_det_top->Fill(rrp_pri/10., zp_pri/10., ntpmthits*QE_PMT[pmtID]*On_PMT[pmtID]);
			h_rrZ_det_bottom->Fill(rrp_pri/10., zp_pri/10., nbpmthits*QE_PMT[pmtID]*On_PMT[pmtID]);
			h_rrZ_det->Fill(rrp_pri/10., zp_pri/10., (ntpmthits+nbpmthits)*QE_PMT[pmtID]*On_PMT[pmtID]);
			h_LCEZ_det_top->Fill(zp_pri/10., ntpmthits*QE_PMT[pmtID]*On_PMT[pmtID]);
			h_LCEZ_det_bottom->Fill(zp_pri/10., nbpmthits*QE_PMT[pmtID]*On_PMT[pmtID]);
			h_LCEZ_det->Fill(zp_pri/10., (ntpmthits+nbpmthits)*QE_PMT[pmtID]*On_PMT[pmtID]);
			h_LCErr_det_top->Fill(rrp_pri/10., ntpmthits*QE_PMT[pmtID]*On_PMT[pmtID]);
			h_LCErr_det_bottom->Fill(rrp_pri/10., nbpmthits*QE_PMT[pmtID]*On_PMT[pmtID]);
			h_LCErr_det->Fill(rrp_pri/10., (ntpmthits+nbpmthits)*QE_PMT[pmtID]*On_PMT[pmtID]);
		}
		// All events
		h_rrZ->Fill(rrp_pri/10., zp_pri/10., 1.);
		h_LCEZ_gen->Fill(zp_pri/10., 1.);
	}
	std::cout << std::endl;	
	
	file_outplot->cd();
	
	/*=================================================================*/
	// ly of R^{2} vs. Z
	/*=================================================================*/
	gStyle->SetPalette(NCont,ColPalette);
	style_2D->cd();
	TCanvas *c_ly_rrZ = new TCanvas("ly_rrZ","ly_rrZ",canvas_x,canvas_y);
	TH2F* h_ly_rrZ = new TH2F("ly_rrZ", "MC: ly of R^{2} vs. Z (ALL PMTs)", TPC.Get_nbinsRR(), TPC.Get_LXe_minRR(), TPC.Get_LXe_maxRR(), TPC.Get_nbinsZ(), TPC.Get_LXe_minZ(), TPC.Get_LXe_maxZ());
	h_ly_rrZ->SetXTitle("R^{2} [cm^{2}]");
	h_ly_rrZ->GetXaxis()->CenterTitle();
	h_ly_rrZ->SetYTitle("Z [cm]");
	h_ly_rrZ->GetYaxis()->CenterTitle();
	h_ly_rrZ->SetZTitle("ly [pe/keV]");
	h_ly_rrZ->GetZaxis()->CenterTitle();
	h_ly_rrZ->Sumw2();
	// Assume the average photon yield from NEST (50 ph/keV, at 32 keV, at 150 V/cm). QE*CE =~ 31%
	h_ly_rrZ->Add(h_rrZ_det_top, 1.);
	h_ly_rrZ->Add(h_rrZ_det_bottom, 1.);
	h_ly_rrZ->Divide(h_rrZ);
	h_ly_rrZ->Scale(50.); 
	double h_ly_rrZ_mean = 0;
	for (int z=0; z<TPC.Get_nbinsZ(); z++){
		for (int r=0; r<TPC.Get_nbinsRR() ; r++){
				h_ly_rrZ_mean += h_ly_rrZ->GetBinContent(r+1,TPC.Get_nbinsZ()-z)/(TPC.Get_nbinsRR()*TPC.Get_nbinsZ());
		}
	}
	cout << "------------------------------------------------------------" << endl;
	cout << "Mean MC ly (calculated): " << h_ly_rrZ_mean << endl;
	cout << "- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - " << endl;
	cout << "Assume the average photon yield from NEST (50 ph/keV, " << endl;
	cout << "at 32 keV, at 150 V/cm). QE*CE =~ 31%" << endl;
	cout << "------------------------------------------------------------" << endl;
	h_ly_rrZ->Draw("colz");
	
	TPaveText *pt_ly_rrZ_QE = new TPaveText(0.725,0.9575,0.995,0.995,"NDC");
	pt_ly_rrZ_QE->SetFillColor(0);   
	pt_ly_rrZ_QE->SetBorderSize(1);
	pt_ly_rrZ_QE->SetTextAlign(22);  
	sprintf(canvasfile,"MC_QE_top: %0.3f", TPC.Get_QE_top());
	pt_ly_rrZ_QE->AddText(canvasfile);
	sprintf(canvasfile,"MC_QE_bottom: %0.3f", TPC.Get_QE_bottom());
	pt_ly_rrZ_QE->AddText(canvasfile);
	pt_ly_rrZ_QE->Draw();

	if (file_outplot) c_ly_rrZ->Write();
	sprintf(canvasfile,"%s/comparison_%s_vs_%s_rrZ_MC_ly.%s", workingdirectory.c_str(), rawdatafilename_mc.c_str(), rawdatafilename_kr.c_str(),export_format.c_str());
	if (!(export_format=="")) c_ly_rrZ->SaveAs(canvasfile);
	
	/*=================================================================*/
	gROOT->SetBatch(kTRUE);
	/*=================================================================*/
	/*=================================================================*/
	// ly RMS of R^{2} vs. Z
	/*=================================================================*/
	
	/* NOT WORKING !!!! */
	
	gStyle->SetPalette(NCont,ColPalette);
	style_2D->cd();
	TCanvas *c_ly_rms_rrZ = new TCanvas("ly_rms_rrZ","ly_rms_rrZ",canvas_x,canvas_y);
	TH2F* h_ly_rms_rrZ = new TH2F("ly_rms_rrZ", "MC: ly RMS of R^{2} vs. Z (ALL PMTs)", TPC.Get_nbinsRR(), TPC.Get_LXe_minRR(), TPC.Get_LXe_maxRR(), TPC.Get_nbinsZ(), TPC.Get_LXe_minZ(), TPC.Get_LXe_maxZ());
	h_ly_rms_rrZ->SetXTitle("R^{2} [cm^{2}]");
	h_ly_rms_rrZ->GetXaxis()->CenterTitle();
	h_ly_rms_rrZ->SetYTitle("Z [cm]");
	h_ly_rms_rrZ->GetYaxis()->CenterTitle();
	h_ly_rms_rrZ->SetZTitle("ly RMS");
	h_ly_rms_rrZ->GetZaxis()->CenterTitle();
	h_ly_rms_rrZ->Sumw2();
	h_ly_rms_rrZ->Add(h_ly_rrZ, 1.);
	
	for (int z=0; z<TPC.Get_nbinsZ(); z++){
		for (int r=0; r<TPC.Get_nbinsRR() ; r++){
			h_ly_rms_rrZ->GetXaxis()->SetRange(r*(TPC.Get_LXe_maxRR()/TPC.Get_nbinsRR()),(r+1)*(TPC.Get_LXe_maxRR()/TPC.Get_nbinsRR()));
			h_ly_rms_rrZ->GetYaxis()->SetRange(z*(TPC.Get_LXe_maxZ()/TPC.Get_nbinsZ()),(z+1)*(TPC.Get_LXe_maxZ()/TPC.Get_nbinsZ()));
			h_ly_rms_rrZ->SetBinContent(r+1,z+1,h_ly_rms_rrZ->GetMean());
		}
	}
	h_ly_rms_rrZ->GetXaxis()->SetRange();
	h_ly_rms_rrZ->GetYaxis()->SetRange();
	h_ly_rms_rrZ->Draw("colz");
	
	TPaveText *pt_ly_rms_rrZ_QE = new TPaveText(0.725,0.9575,0.995,0.995,"NDC");
	pt_ly_rms_rrZ_QE->SetFillColor(0);   
	pt_ly_rms_rrZ_QE->SetBorderSize(1);
	pt_ly_rms_rrZ_QE->SetTextAlign(22);  
	sprintf(canvasfile,"MC_QE_top: %0.3f", TPC.Get_QE_top());
	pt_ly_rms_rrZ_QE->AddText(canvasfile);
	sprintf(canvasfile,"MC_QE_bottom: %0.3f", TPC.Get_QE_bottom());
	pt_ly_rms_rrZ_QE->AddText(canvasfile);
	pt_ly_rms_rrZ_QE->Draw();
	
	if (file_outplot) c_ly_rms_rrZ->Write();
	//sprintf(canvasfile,"%s/%s_comparison_ly_sigma_rrZ_MC.%s", workingdirectory.c_str(),suffix.c_str(),export_format.c_str());
	//if (!(export_format=="")) c_ly_rms_rrZ->SaveAs(canvasfile);
	
	/*=================================================================*/
	gROOT->SetBatch(kTRUE);
	/*=================================================================*/
	/*=================================================================*/
	// ly of R^{2} vs. Z TOP PMTs
	/*=================================================================*/
	gStyle->SetPalette(NCont,ColPalette);
	style_2D->cd();
	TCanvas *c_ly_rrZ_top = new TCanvas("ly_rrZ_top","ly_rrZ_top",canvas_x,canvas_y);
	TH2F* h_ly_rrZ_top = new TH2F("ly_rrZ_top", "MC: ly of R^{2} vs. Z (TOP PMTs)", TPC.Get_nbinsRR(), TPC.Get_LXe_minRR(), TPC.Get_LXe_maxRR(), TPC.Get_nbinsZ(), TPC.Get_LXe_minZ(), TPC.Get_LXe_maxZ());
	h_ly_rrZ_top->SetXTitle("R^{2} [cm^{2}]");
	h_ly_rrZ_top->GetXaxis()->CenterTitle();
	h_ly_rrZ_top->SetYTitle("Z [cm]");
	h_ly_rrZ_top->GetYaxis()->CenterTitle();
	h_ly_rrZ_top->SetZTitle("ly [pe/keV]");
	h_ly_rrZ_top->GetZaxis()->CenterTitle();
	h_ly_rrZ_top->Sumw2();
	// Assume the average photon yield from NEST (50 ph/keV, at 32 keV, at 150 V/cm). QE*CE =~ 31%
	h_ly_rrZ_top->Add(h_rrZ_det_top, 1.);
	h_ly_rrZ_top->Divide(h_rrZ);
	h_ly_rrZ_top->Scale(50.); 
	h_ly_rrZ_top->Draw("colz");
	
	TPaveText *pt_ly_rrZ_top_QE = new TPaveText(0.725,0.9575,0.995,0.995,"NDC");
	pt_ly_rrZ_top_QE->SetFillColor(0);   
	pt_ly_rrZ_top_QE->SetBorderSize(1);
	pt_ly_rrZ_top_QE->SetTextAlign(22);  
	sprintf(canvasfile,"MC_QE_top: %0.3f", TPC.Get_QE_top());
	pt_ly_rrZ_top_QE->AddText(canvasfile);
	sprintf(canvasfile,"MC_QE_bottom: %0.3f", TPC.Get_QE_bottom());
	pt_ly_rrZ_top_QE->AddText(canvasfile);
	pt_ly_rrZ_top_QE->Draw();
	
	if (file_outplot) c_ly_rrZ_top->Write();
	sprintf(canvasfile,"%s/comparison_%s_vs_%s_rrZ_MC_ly_top.%s", workingdirectory.c_str(), rawdatafilename_mc.c_str(), rawdatafilename_kr.c_str(),export_format.c_str());
	if (!(export_format=="")) c_ly_rrZ_top->SaveAs(canvasfile);
	
	/*=================================================================*/
	// ly of R^{2} vs. Z BOTTOM PMTs
	/*=================================================================*/
	gStyle->SetPalette(NCont,ColPalette);
	style_2D->cd();
	TCanvas *c_ly_rrZ_bottom = new TCanvas("ly_rrZ_bottom","ly_rrZ_bottom",canvas_x,canvas_y);
	TH2F* h_ly_rrZ_bottom = new TH2F("ly_rrZ_bottom", "MC: ly of R^{2} vs. Z (BOTTOM PMTs)", TPC.Get_nbinsRR(), TPC.Get_LXe_minRR(), TPC.Get_LXe_maxRR(), TPC.Get_nbinsZ(), TPC.Get_LXe_minZ(), TPC.Get_LXe_maxZ());
	h_ly_rrZ_bottom->SetXTitle("R^{2} [cm^{2}]");
	h_ly_rrZ_bottom->GetXaxis()->CenterTitle();
	h_ly_rrZ_bottom->SetYTitle("Z [cm]");
	h_ly_rrZ_bottom->GetYaxis()->CenterTitle();
	h_ly_rrZ_bottom->SetZTitle("ly [pe/keV]");
	h_ly_rrZ_bottom->GetZaxis()->CenterTitle();
	h_ly_rrZ_bottom->Sumw2();
	// Assume the average photon yield from NEST (50 ph/keV, at 32 keV, at 150 V/cm). QE*CE =~ 31%
	h_ly_rrZ_bottom->Add(h_rrZ_det_bottom, 1.);
	h_ly_rrZ_bottom->Divide(h_rrZ);
	h_ly_rrZ_bottom->Scale(50.); 
	h_ly_rrZ_bottom->Draw("colz");	
	
	TPaveText *pt_ly_rrZ_bottom_QE = new TPaveText(0.725,0.9575,0.995,0.995,"NDC");
	pt_ly_rrZ_bottom_QE->SetFillColor(0);   
	pt_ly_rrZ_bottom_QE->SetBorderSize(1);
	pt_ly_rrZ_bottom_QE->SetTextAlign(22);  
	sprintf(canvasfile,"MC_QE_top: %0.3f", TPC.Get_QE_top());
	pt_ly_rrZ_bottom_QE->AddText(canvasfile);
	sprintf(canvasfile,"MC_QE_bottom: %0.3f", TPC.Get_QE_bottom());
	pt_ly_rrZ_bottom_QE->AddText(canvasfile);
	pt_ly_rrZ_bottom_QE->Draw();
	
	if (file_outplot) c_ly_rrZ_bottom->Write();
	sprintf(canvasfile,"%s/comparison_%s_vs_%s_rrZ_MC_ly_bottom.%s", workingdirectory.c_str(), rawdatafilename_mc.c_str(), rawdatafilename_kr.c_str(),export_format.c_str());
	if (!(export_format=="")) c_ly_rrZ_bottom->SaveAs(canvasfile);
	
	/*=================================================================*/
	// comparison ly of R^{2} vs. Z
	/*=================================================================*/
	gStyle->SetPalette(NCont_Sym,ColPalette_Sym);
	style_2D_Sym->cd();
	TCanvas *c_cly_rrZ = new TCanvas("cly_rrZ","cly_rrZ",canvas_x,canvas_y);
	TH2F* h_cly_rrZ = new TH2F("cly_rrZ", "Comparison: ly of R^{2} vs. Z (ALL PMTs)", TPC.Get_nbinsRR(), TPC.Get_LXe_minRR(), TPC.Get_LXe_maxRR(), TPC.Get_nbinsZ(), TPC.Get_LXe_minZ(), TPC.Get_LXe_maxZ());
	h_cly_rrZ->SetXTitle("R^{2} [cm^{2}]");
	h_cly_rrZ->GetXaxis()->CenterTitle();
	h_cly_rrZ->SetYTitle("Z [cm]");
	h_cly_rrZ->GetYaxis()->CenterTitle();
	h_cly_rrZ->SetZTitle("ly ^{83m}Kr - ly MC [pe/keV]");
	h_cly_rrZ->GetZaxis()->CenterTitle();
	h_cly_rrZ->Add(h_ly_rrZ,-1);
	h_cly_rrZ->Add(h_ly_map,1);
	h_cly_rrZ->Draw("colz");
	
	TPaveText *pt_cly_rrZ_QE = new TPaveText(0.725,0.9575,0.995,0.995,"NDC");
	pt_cly_rrZ_QE->SetFillColor(0);   
	pt_cly_rrZ_QE->SetBorderSize(1);
	pt_cly_rrZ_QE->SetTextAlign(22);  
	sprintf(canvasfile,"MC_QE_top: %0.3f", TPC.Get_QE_top());
	pt_cly_rrZ_QE->AddText(canvasfile);
	sprintf(canvasfile,"MC_QE_bottom: %0.3f", TPC.Get_QE_bottom());
	pt_cly_rrZ_QE->AddText(canvasfile);
	pt_cly_rrZ_QE->Draw();

	if ( (abs(h_cly_rrZ->GetMaximum()) > abs(h_cly_rrZ->GetMinimum())) && (h_cly_rrZ->GetMaximum() > 0.) )
		{h_cly_rrZ->SetMinimum(-abs(h_cly_rrZ->GetMaximum()));}
	else if ( (abs(h_cly_rrZ->GetMaximum()) < abs(h_cly_rrZ->GetMinimum())) && (h_cly_rrZ->GetMinimum() < 0.) ) 
		{h_cly_rrZ->SetMaximum(abs(h_cly_rrZ->GetMinimum()));}
	// else do nothing; 
	if (file_outplot) c_cly_rrZ->Write();
	sprintf(canvasfile,"%s/comparison_%s_vs_%s_rrZ_ly.%s", workingdirectory.c_str(), rawdatafilename_mc.c_str(), rawdatafilename_kr.c_str(),export_format.c_str());
	if (!(export_format=="")) c_cly_rrZ->SaveAs(canvasfile);
	
	/*=================================================================*/
	// comparison ly of R^{2} vs. Z TOP PMTs
	/*=================================================================*/
	gStyle->SetPalette(NCont_Sym,ColPalette_Sym);
	style_2D_Sym->cd();
	TCanvas *c_cly_rrZ_top = new TCanvas("cly_rrZ_top","cly_rrZ_top",canvas_x,canvas_y);
	TH2F* h_cly_rrZ_top = new TH2F("cly_rrZ_top", "Comparison: ly of R^{2} vs. Z (TOP PMTs)", TPC.Get_nbinsRR(), TPC.Get_LXe_minRR(), TPC.Get_LXe_maxRR(), TPC.Get_nbinsZ(), TPC.Get_LXe_minZ(), TPC.Get_LXe_maxZ());
	h_cly_rrZ_top->SetXTitle("R^{2} [cm^{2}]");
	h_cly_rrZ_top->GetXaxis()->CenterTitle();
	h_cly_rrZ_top->SetYTitle("Z [cm]");
	h_cly_rrZ_top->GetYaxis()->CenterTitle();
	h_cly_rrZ_top->SetZTitle("ly ^{83m}Kr - ly MC [pe/keV]");
	h_cly_rrZ_top->GetZaxis()->CenterTitle();
	h_cly_rrZ_top->Add(h_ly_rrZ_top,-1);
	h_cly_rrZ_top->Add(h_ly_map_top,1);
	h_cly_rrZ_top->Draw("colz");
	
	TPaveText *pt_cly_rrZ_top_QE = new TPaveText(0.725,0.9575,0.995,0.995,"NDC");
	pt_cly_rrZ_top_QE->SetFillColor(0);   
	pt_cly_rrZ_top_QE->SetBorderSize(1);
	pt_cly_rrZ_top_QE->SetTextAlign(22);  
	sprintf(canvasfile,"MC_QE_top: %0.3f", TPC.Get_QE_top());
	pt_cly_rrZ_top_QE->AddText(canvasfile);
	sprintf(canvasfile,"MC_QE_bottom: %0.3f", TPC.Get_QE_bottom());
	pt_cly_rrZ_top_QE->AddText(canvasfile);
	pt_cly_rrZ_top_QE->Draw();
	
	if ( (abs(h_cly_rrZ_top->GetMaximum()) > abs(h_cly_rrZ_top->GetMinimum())) && (h_cly_rrZ_top->GetMaximum() > 0.) )
		{h_cly_rrZ_top->SetMinimum(-abs(h_cly_rrZ_top->GetMaximum()));}
	else if ( (abs(h_cly_rrZ_top->GetMaximum()) < abs(h_cly_rrZ_top->GetMinimum())) && (h_cly_rrZ_top->GetMinimum() < 0.) ) 
		{h_cly_rrZ_top->SetMaximum(abs(h_cly_rrZ_top->GetMinimum()));}
	// else do nothing; 
	if (file_outplot) c_cly_rrZ_top->Write();
	sprintf(canvasfile,"%s/comparison_%s_vs_%s_rrZ_ly_top.%s", workingdirectory.c_str(), rawdatafilename_mc.c_str(), rawdatafilename_kr.c_str(),export_format.c_str());
	if (!(export_format=="")) c_cly_rrZ_top->SaveAs(canvasfile);
	
	/*=================================================================*/
	// comparison ly of R^{2} vs. Z BOTTOM PMTs
	/*=================================================================*/
	gStyle->SetPalette(NCont_Sym,ColPalette_Sym);
	style_2D_Sym->cd();
	TCanvas *c_cly_rrZ_bottom = new TCanvas("cly_rrZ_bottom","cly_rrZ_bottom",canvas_x,canvas_y);
	TH2F* h_cly_rrZ_bottom = new TH2F("cly_rrZ_bottom", "Comparison: ly of R^{2} vs. Z (BOTTOM PMTs)", TPC.Get_nbinsRR(), TPC.Get_LXe_minRR(), TPC.Get_LXe_maxRR(), TPC.Get_nbinsZ(), TPC.Get_LXe_minZ(), TPC.Get_LXe_maxZ());
	h_cly_rrZ_bottom->SetXTitle("R^{2} [cm^{2}]");
	h_cly_rrZ_bottom->GetXaxis()->CenterTitle();
	h_cly_rrZ_bottom->SetYTitle("Z [cm]");
	h_cly_rrZ_bottom->GetYaxis()->CenterTitle();
	h_cly_rrZ_bottom->SetZTitle("ly ^{83m}Kr - ly MC [pe/keV]");
	h_cly_rrZ_bottom->GetZaxis()->CenterTitle();
	h_cly_rrZ_bottom->Add(h_ly_rrZ_bottom,-1);
	h_cly_rrZ_bottom->Add(h_ly_map_bottom,1);
	h_cly_rrZ_bottom->Draw("colz");
	
	TPaveText *pt_cly_rrZ_bottom_QE = new TPaveText(0.725,0.9575,0.995,0.995,"NDC");
	pt_cly_rrZ_bottom_QE->SetFillColor(0);   
	pt_cly_rrZ_bottom_QE->SetBorderSize(1);
	pt_cly_rrZ_bottom_QE->SetTextAlign(22);  
	sprintf(canvasfile,"MC_QE_top: %0.3f", TPC.Get_QE_top());
	pt_cly_rrZ_bottom_QE->AddText(canvasfile);
	sprintf(canvasfile,"MC_QE_bottom: %0.3f", TPC.Get_QE_bottom());
	pt_cly_rrZ_bottom_QE->AddText(canvasfile);
	pt_cly_rrZ_bottom_QE->Draw();
	
	if ( (abs(h_cly_rrZ_bottom->GetMaximum()) > abs(h_cly_rrZ_bottom->GetMinimum())) && (h_cly_rrZ_bottom->GetMaximum() > 0.) )
		{h_cly_rrZ_bottom->SetMinimum(-abs(h_cly_rrZ_bottom->GetMaximum()));}
	else if ( (abs(h_cly_rrZ_bottom->GetMaximum()) < abs(h_cly_rrZ_bottom->GetMinimum())) && (h_cly_rrZ_bottom->GetMinimum() < 0.) ) 
		{h_cly_rrZ_bottom->SetMaximum(abs(h_cly_rrZ_bottom->GetMinimum()));}
	// else do nothing; 
	if (file_outplot) c_cly_rrZ_bottom->Write();
	sprintf(canvasfile,"%s/comparison_%s_vs_%s_rrZ_ly_bottom.%s", workingdirectory.c_str(), rawdatafilename_mc.c_str(), rawdatafilename_kr.c_str(),export_format.c_str());
	if (!(export_format=="")) c_cly_rrZ_bottom->SaveAs(canvasfile);
	
	/*=================================================================*/
	// relative LCE of R^{2} vs. Z
	/*=================================================================*/
	gStyle->SetPalette(NCont,ColPalette);
	style_2D->cd();
	TCanvas *c_rLCE_rrZ = new TCanvas("rLCE_rrZ","rLCE_rrZ",canvas_x,canvas_y);
	TH2F* h_rLCE_rrZ = new TH2F("rLCE_rrZ", "MC: relative LCE of R^{2} vs. Z (ALL PMTs)", TPC.Get_nbinsRR(), TPC.Get_LXe_minRR(), TPC.Get_LXe_maxRR(), TPC.Get_nbinsZ(), TPC.Get_LXe_minZ(), TPC.Get_LXe_maxZ());
	h_rLCE_rrZ->SetXTitle("R^{2} [cm^{2}]");
	h_rLCE_rrZ->GetXaxis()->CenterTitle();
	h_rLCE_rrZ->SetYTitle("Z [cm]");
	h_rLCE_rrZ->GetYaxis()->CenterTitle();
	h_rLCE_rrZ->SetZTitle("relative LCE");
	h_rLCE_rrZ->GetZaxis()->CenterTitle();
	h_rLCE_rrZ->Sumw2();
	h_rLCE_rrZ->Add(h_rrZ_det_top, 1.);
	h_rLCE_rrZ->Add(h_rrZ_det_bottom, 1.);
	h_rLCE_rrZ->Divide(h_rrZ);	
	h_rLCE_rrZ->Scale(100.);
	double h_LCE_rrZ_mean = 0;
	for (int z=0; z<TPC.Get_nbinsZ(); z++){
		for (int r=0; r<TPC.Get_nbinsRR() ; r++){
				h_LCE_rrZ_mean += h_rLCE_rrZ->GetBinContent(r+1,TPC.Get_nbinsZ()-z)/(TPC.Get_nbinsRR()*TPC.Get_nbinsZ());
		}
	}
	cout << "------------------------------------------------------------" << endl;
	cout << "Mean MC LCE: " << h_LCE_rrZ_mean << endl;
	cout << "------------------------------------------------------------" << endl;
	h_rLCE_rrZ->Scale(1./h_LCE_rrZ_mean);
	h_rLCE_rrZ->SetMaximum(1.5);
	h_rLCE_rrZ->SetMinimum(0.5);
	h_rLCE_rrZ->Draw("colz");
	
	TPaveText *pt_rLCE_rrZ_QE = new TPaveText(0.725,0.9575,0.995,0.995,"NDC");
	pt_rLCE_rrZ_QE->SetFillColor(0);   
	pt_rLCE_rrZ_QE->SetBorderSize(1);
	pt_rLCE_rrZ_QE->SetTextAlign(22);  
	sprintf(canvasfile,"MC_QE_top: %0.3f", TPC.Get_QE_top());
	pt_rLCE_rrZ_QE->AddText(canvasfile);
	sprintf(canvasfile,"MC_QE_bottom: %0.3f", TPC.Get_QE_bottom());
	pt_rLCE_rrZ_QE->AddText(canvasfile);
	pt_rLCE_rrZ_QE->Draw();
	
	if (file_outplot) c_rLCE_rrZ->Write();
	sprintf(canvasfile,"%s/comparison_%s_vs_%s_rrZ_MC_rLCE.%s", workingdirectory.c_str(), rawdatafilename_mc.c_str(), rawdatafilename_kr.c_str(),export_format.c_str());
	if (!(export_format=="")) c_rLCE_rrZ->SaveAs(canvasfile);
	
	/*=================================================================*/
	// relative LCE of R^{2} vs. Z TOP
	/*=================================================================*/
	style_2D->cd();
	TCanvas *c_rLCE_rrZ_top = new TCanvas("rLCE_rrZ_top","rLCE_rrZ_top",canvas_x,canvas_y);
	TH2F* h_rLCE_rrZ_top = new TH2F("rLCE_rrZ_top", "MC: relative LCE of R^{2} vs. Z (TOP PMTs)", TPC.Get_nbinsRR(), TPC.Get_LXe_minRR(), TPC.Get_LXe_maxRR(), TPC.Get_nbinsZ(), TPC.Get_LXe_minZ(), TPC.Get_LXe_maxZ());
	h_rLCE_rrZ_top->SetXTitle("R^{2} [cm^{2}]");
	h_rLCE_rrZ_top->GetXaxis()->CenterTitle();
	h_rLCE_rrZ_top->SetYTitle("Z [cm]");
	h_rLCE_rrZ_top->GetYaxis()->CenterTitle();
	h_rLCE_rrZ_top->SetZTitle("relative LCE");
	h_rLCE_rrZ_top->GetZaxis()->CenterTitle();
	h_rLCE_rrZ_top->Sumw2();
	h_rLCE_rrZ_top->Divide(h_rrZ_det_top, h_rrZ, 1., 1., "b");
	h_rLCE_rrZ_top->Scale(100.);
	h_rLCE_rrZ_top->Scale(1./h_LCE_rrZ_mean);
	h_rLCE_rrZ_top->SetMaximum(0.4);
	h_rLCE_rrZ_top->SetMinimum(0);
	h_rLCE_rrZ_top->Draw("colz");
	
	TPaveText *pt_rLCE_rrZ_top_QE = new TPaveText(0.725,0.9575,0.995,0.995,"NDC");
	pt_rLCE_rrZ_top_QE->SetFillColor(0);   
	pt_rLCE_rrZ_top_QE->SetBorderSize(1);
	pt_rLCE_rrZ_top_QE->SetTextAlign(22);  
	sprintf(canvasfile,"MC_QE_top: %0.3f", TPC.Get_QE_top());
	pt_rLCE_rrZ_top_QE->AddText(canvasfile);
	sprintf(canvasfile,"MC_QE_bottom: %0.3f", TPC.Get_QE_bottom());
	pt_rLCE_rrZ_top_QE->AddText(canvasfile);
	pt_rLCE_rrZ_top_QE->Draw();
	
	if (file_outplot) c_rLCE_rrZ_top->Write();
	sprintf(canvasfile,"%s/comparison_%s_vs_%s_rrZ_MC_rLCE_top.%s", workingdirectory.c_str(), rawdatafilename_mc.c_str(), rawdatafilename_kr.c_str(),export_format.c_str());
	if (!(export_format=="")) c_rLCE_rrZ_top->SaveAs(canvasfile);
	
	/*=================================================================*/
	// relative LCE of R^{2} vs. Z BOTTOM
	/*=================================================================*/
	style_2D->cd();
	TCanvas *c_rLCE_rrZ_bottom = new TCanvas("rLCE_rrZ_bottom","rLCE_rrZ_bottom",canvas_x,canvas_y);
	TH2F* h_rLCE_rrZ_bottom = new TH2F("rLCE_rrZ_bottom", "MC: relative LCE of R^{2} vs. Z (BOTTOM PMTs)", TPC.Get_nbinsRR(), TPC.Get_LXe_minRR(), TPC.Get_LXe_maxRR(), TPC.Get_nbinsZ(), TPC.Get_LXe_minZ(), TPC.Get_LXe_maxZ());
	h_rLCE_rrZ_bottom->SetXTitle("R^{2} [cm^{2}]");
	h_rLCE_rrZ_bottom->GetXaxis()->CenterTitle();
	h_rLCE_rrZ_bottom->SetYTitle("Z [cm]");
	h_rLCE_rrZ_bottom->GetYaxis()->CenterTitle();
	h_rLCE_rrZ_bottom->SetZTitle("relative LCE");
	h_rLCE_rrZ_bottom->GetZaxis()->CenterTitle();
	h_rLCE_rrZ_bottom->Sumw2();
	h_rLCE_rrZ_bottom->Divide(h_rrZ_det_bottom, h_rrZ, 1., 1., "b");
	h_rLCE_rrZ_bottom->Scale(100.);
	h_rLCE_rrZ_bottom->Scale(1./h_LCE_rrZ_mean);
	h_rLCE_rrZ_bottom->SetMaximum(1.4);
	h_rLCE_rrZ_bottom->SetMinimum(0.4);
	h_rLCE_rrZ_bottom->Draw("colz");
	
	TPaveText *pt_rLCE_rrZ_bottom_QE = new TPaveText(0.725,0.9575,0.995,0.995,"NDC");
	pt_rLCE_rrZ_bottom_QE->SetFillColor(0);   
	pt_rLCE_rrZ_bottom_QE->SetBorderSize(1);
	pt_rLCE_rrZ_bottom_QE->SetTextAlign(22);  
	sprintf(canvasfile,"MC_QE_top: %0.3f", TPC.Get_QE_top());
	pt_rLCE_rrZ_bottom_QE->AddText(canvasfile);
	sprintf(canvasfile,"MC_QE_bottom: %0.3f", TPC.Get_QE_bottom());
	pt_rLCE_rrZ_bottom_QE->AddText(canvasfile);
	pt_rLCE_rrZ_bottom_QE->Draw();
	
	if (file_outplot) c_rLCE_rrZ_bottom->Write();
	sprintf(canvasfile,"%s/comparison_%s_vs_%s_rrZ_MC_rLCE_bottom.%s", workingdirectory.c_str(), rawdatafilename_mc.c_str(), rawdatafilename_kr.c_str(),export_format.c_str());
	if (!(export_format=="")) c_rLCE_rrZ_bottom->SaveAs(canvasfile);
	
	/*=================================================================*/
	// comparison relative LCE of R^{2} vs. Z
	/*=================================================================*/
	gStyle->SetPalette(NCont_Sym,ColPalette_Sym);
	style_2D_Sym->cd();
	TCanvas *c_crLCE_rrZ = new TCanvas("crLCE_rrZ","crLCE_rrZ",canvas_x,canvas_y);
	TH2F* h_crLCE_rrZ = new TH2F("crLCE_rrZ", "Comparison: relative LCE of R^{2} vs. Z (ALL PMTs)", TPC.Get_nbinsRR(), TPC.Get_LXe_minRR(), TPC.Get_LXe_maxRR(), TPC.Get_nbinsZ(), TPC.Get_LXe_minZ(), TPC.Get_LXe_maxZ());
	h_crLCE_rrZ->SetXTitle("R^{2} [cm^{2}]");
	h_crLCE_rrZ->GetXaxis()->CenterTitle();
	h_crLCE_rrZ->SetYTitle("Z [cm]");
	h_crLCE_rrZ->GetYaxis()->CenterTitle();
	h_crLCE_rrZ->SetZTitle("rLCE ^{83m}Kr - rLCE MC");
	h_crLCE_rrZ->GetZaxis()->CenterTitle();
	h_crLCE_rrZ->Add(h_rLCE_rrZ,-1);
	h_crLCE_rrZ->Add(h_rLCE_map,1);
	h_crLCE_rrZ->Draw("colz");
	
	TPaveText *pt_crLCE_rrZ_QE = new TPaveText(0.725,0.9575,0.995,0.995,"NDC");
	pt_crLCE_rrZ_QE->SetFillColor(0);   
	pt_crLCE_rrZ_QE->SetBorderSize(1);
	pt_crLCE_rrZ_QE->SetTextAlign(22);  
	sprintf(canvasfile,"MC_QE_top: %0.3f", TPC.Get_QE_top());
	pt_crLCE_rrZ_QE->AddText(canvasfile);
	sprintf(canvasfile,"MC_QE_bottom: %0.3f", TPC.Get_QE_bottom());
	pt_crLCE_rrZ_QE->AddText(canvasfile);
	pt_crLCE_rrZ_QE->Draw();
	
	if ( (abs(h_crLCE_rrZ->GetMaximum()) > abs(h_crLCE_rrZ->GetMinimum())) && (h_crLCE_rrZ->GetMaximum() > 0.) )
		{h_crLCE_rrZ->SetMinimum(-abs(h_crLCE_rrZ->GetMaximum()));}
	else if ( (abs(h_crLCE_rrZ->GetMaximum()) < abs(h_crLCE_rrZ->GetMinimum())) && (h_crLCE_rrZ->GetMinimum() < 0.) ) 
		{h_crLCE_rrZ->SetMaximum(abs(h_crLCE_rrZ->GetMinimum()));}
	// else do nothing; 
	if (file_outplot) c_crLCE_rrZ->Write();
	sprintf(canvasfile,"%s/comparison_%s_vs_%s_rrZ_rLCE.%s", workingdirectory.c_str(), rawdatafilename_mc.c_str(), rawdatafilename_kr.c_str(),export_format.c_str());
	if (!(export_format=="")) c_crLCE_rrZ->SaveAs(canvasfile);
	
	/*=================================================================*/
	// comparison relative LCE of R^{2} vs. Z TOP
	/*=================================================================*/
	style_2D_Sym->cd();
	TCanvas *c_crLCE_rrZ_top = new TCanvas("crLCE_rrZ_top","crLCE_rrZ_top",canvas_x,canvas_y);
	TH2F* h_crLCE_rrZ_top = new TH2F("crLCE_rrZ_top", "Comparison: relative LCE of R^{2} vs. Z (TOP PMTs)", TPC.Get_nbinsRR(), TPC.Get_LXe_minRR(), TPC.Get_LXe_maxRR(), TPC.Get_nbinsZ(), TPC.Get_LXe_minZ(), TPC.Get_LXe_maxZ());
	h_crLCE_rrZ_top->SetXTitle("R^{2} [cm^{2}]");
	h_crLCE_rrZ_top->GetXaxis()->CenterTitle();
	h_crLCE_rrZ_top->SetYTitle("Z [cm]");
	h_crLCE_rrZ_top->GetYaxis()->CenterTitle();
	h_crLCE_rrZ_top->SetZTitle("rLCE ^{83m}Kr - rLCE MC");
	h_crLCE_rrZ_top->GetZaxis()->CenterTitle();
	h_crLCE_rrZ_top->Add(h_rLCE_rrZ_top,-1);
	h_crLCE_rrZ_top->Add(h_rLCE_map_top,1);
	h_crLCE_rrZ_top->Draw("colz");
	
	TPaveText *pt_crLCE_rrZ_top_QE = new TPaveText(0.725,0.9575,0.995,0.995,"NDC");
	pt_crLCE_rrZ_top_QE->SetFillColor(0);   
	pt_crLCE_rrZ_top_QE->SetBorderSize(1);
	pt_crLCE_rrZ_top_QE->SetTextAlign(22);  
	sprintf(canvasfile,"MC_QE_top: %0.3f", TPC.Get_QE_top());
	pt_crLCE_rrZ_top_QE->AddText(canvasfile);
	sprintf(canvasfile,"MC_QE_bottom: %0.3f", TPC.Get_QE_bottom());
	pt_crLCE_rrZ_top_QE->AddText(canvasfile);
	pt_crLCE_rrZ_top_QE->Draw();
	
	if ( (abs(h_crLCE_rrZ_top->GetMaximum()) > abs(h_crLCE_rrZ_top->GetMinimum())) && (h_crLCE_rrZ_top->GetMaximum() > 0.) )
		{h_crLCE_rrZ_top->SetMinimum(-abs(h_crLCE_rrZ_top->GetMaximum()));}
	else if ( (abs(h_crLCE_rrZ_top->GetMaximum()) < abs(h_crLCE_rrZ_top->GetMinimum())) && (h_crLCE_rrZ_top->GetMinimum() < 0.) ) 
		{h_crLCE_rrZ_top->SetMaximum(abs(h_crLCE_rrZ_top->GetMinimum()));}
	// else do nothing;
	if (file_outplot) c_crLCE_rrZ_top->Write();
	sprintf(canvasfile,"%s/comparison_%s_vs_%s_rrZ_rLCE_top.%s", workingdirectory.c_str(), rawdatafilename_mc.c_str(), rawdatafilename_kr.c_str(),export_format.c_str());
	if (!(export_format=="")) c_crLCE_rrZ_top->SaveAs(canvasfile);
	
	/*=================================================================*/
	// comparison relative LCE of R^{2} vs. Z BOTTOM
	/*=================================================================*/
	style_2D_Sym->cd();
	TCanvas *c_crLCE_rrZ_bottom = new TCanvas("crLCE_rrZ_bottom","crLCE_rrZ_bottom",canvas_x,canvas_y);
	TH2F* h_crLCE_rrZ_bottom = new TH2F("crLCE_rrZ_bottom", "Comparison: relative LCE of R^{2} vs. Z (BOTTOM PMTs)", TPC.Get_nbinsRR(), TPC.Get_LXe_minRR(), TPC.Get_LXe_maxRR(), TPC.Get_nbinsZ(), TPC.Get_LXe_minZ(), TPC.Get_LXe_maxZ());
	h_crLCE_rrZ_bottom->SetXTitle("R^{2} [cm^{2}]");
	h_crLCE_rrZ_bottom->GetXaxis()->CenterTitle();
	h_crLCE_rrZ_bottom->SetYTitle("Z [cm]");
	h_crLCE_rrZ_bottom->GetYaxis()->CenterTitle();
	h_crLCE_rrZ_bottom->SetZTitle("rLCE ^{83m}Kr - rLCE MC");
	h_crLCE_rrZ_bottom->GetZaxis()->CenterTitle();
	h_crLCE_rrZ_bottom->Add(h_rLCE_rrZ_bottom,-1);
	h_crLCE_rrZ_bottom->Add(h_rLCE_map_bottom,1);
	h_crLCE_rrZ_bottom->Draw("colz");
	
	TPaveText *pt_crLCE_rrZ_bottom_QE = new TPaveText(0.725,0.9575,0.995,0.995,"NDC");
	pt_crLCE_rrZ_bottom_QE->SetFillColor(0);   
	pt_crLCE_rrZ_bottom_QE->SetBorderSize(1);
	pt_crLCE_rrZ_bottom_QE->SetTextAlign(22);  
	sprintf(canvasfile,"MC_QE_top: %0.3f", TPC.Get_QE_top());
	pt_crLCE_rrZ_bottom_QE->AddText(canvasfile);
	sprintf(canvasfile,"MC_QE_bottom: %0.3f", TPC.Get_QE_bottom());
	pt_crLCE_rrZ_bottom_QE->AddText(canvasfile);
	pt_crLCE_rrZ_bottom_QE->Draw();
	
	if ( (abs(h_crLCE_rrZ_bottom->GetMaximum()) > abs(h_crLCE_rrZ_bottom->GetMinimum())) && (h_crLCE_rrZ_bottom->GetMaximum() > 0.) )
		{h_crLCE_rrZ_bottom->SetMinimum(-abs(h_crLCE_rrZ_bottom->GetMaximum()));}
	else if ( (abs(h_crLCE_rrZ_bottom->GetMaximum()) < abs(h_crLCE_rrZ_bottom->GetMinimum())) && (h_crLCE_rrZ_bottom->GetMinimum() < 0.) ) 
		{h_crLCE_rrZ_bottom->SetMaximum(abs(h_crLCE_rrZ_bottom->GetMinimum()));}
	// else do nothing; 
	if (file_outplot) c_crLCE_rrZ_bottom->Write();
	sprintf(canvasfile,"%s/comparison_%s_vs_%s_rrZ_rLCE_bottom.%s", workingdirectory.c_str(), rawdatafilename_mc.c_str(), rawdatafilename_kr.c_str(),export_format.c_str());
	if (!(export_format=="")) c_crLCE_rrZ_bottom->SaveAs(canvasfile);
	
	/*=================================================================*/
	// relative LCE of R^{2} vs. Z 3D
	/*=================================================================*/
	style_3D->cd();
	TH2F* h_rLCE_rrZ_3D = new TH2F("rLCE_rrZ_3D", "MC: relative LCE of R^{2} vs. Z (ALL PMTs)", TPC.Get_nbinsZ(), TPC.Get_LXe_minZ(), TPC.Get_LXe_maxZ(), TPC.Get_nbinsRR(), TPC.Get_LXe_minRR(), TPC.Get_LXe_maxRR());
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
	TH2F* h_crLCE_rrZ_3D = new TH2F("crLCE_rrZ_3D", "Comparison: relative LCE of R^{2} vs. Z (ALL PMTs)", TPC.Get_nbinsZ(), TPC.Get_LXe_minZ(), TPC.Get_LXe_maxZ(), TPC.Get_nbinsRR(), TPC.Get_LXe_minRR(), TPC.Get_LXe_maxRR());
	h_crLCE_rrZ_3D->SetYTitle("R^{2} [cm^{2}]");
	h_crLCE_rrZ_3D->GetYaxis()->CenterTitle();
	h_crLCE_rrZ_3D->SetXTitle("Z [cm]");
	h_crLCE_rrZ_3D->GetXaxis()->CenterTitle();
	h_crLCE_rrZ_3D->SetZTitle("rLCE ^{83m}Kr - rLCE MC");
	h_crLCE_rrZ_3D->GetZaxis()->CenterTitle();
	h_crLCE_rrZ_3D->Add(h_rLCE_rrZ_3D,-1);
	h_crLCE_rrZ_3D->Add(h_rLCE_map_3D,1);
	h_crLCE_rrZ_3D->Draw("surf1");
	
	TPaveText *pt_crLCE_rrZ_3D_QE = new TPaveText(0.725,0.93,0.995,0.995,"NDC");
	pt_crLCE_rrZ_3D_QE->SetFillColor(0);   
	pt_crLCE_rrZ_3D_QE->SetBorderSize(1);
	pt_crLCE_rrZ_3D_QE->SetTextAlign(22);  
	sprintf(canvasfile,"MC_QE_top: %0.3f", TPC.Get_QE_top());
	pt_crLCE_rrZ_3D_QE->AddText(canvasfile);
	sprintf(canvasfile,"MC_QE_bottom: %0.3f", TPC.Get_QE_bottom());
	pt_crLCE_rrZ_3D_QE->AddText(canvasfile);
	pt_crLCE_rrZ_3D_QE->Draw();
	
	if ( (abs(h_crLCE_rrZ_3D->GetMaximum()) > abs(h_crLCE_rrZ_3D->GetMinimum())) && (h_crLCE_rrZ_3D->GetMaximum() > 0.) )
		{h_crLCE_rrZ_bottom->SetMinimum(-abs(h_crLCE_rrZ_bottom->GetMaximum()));}
	else if ( (abs(h_crLCE_rrZ_3D->GetMaximum()) < abs(h_crLCE_rrZ_3D->GetMinimum())) && (h_crLCE_rrZ_3D->GetMinimum() < 0.) ) 
		{h_crLCE_rrZ_bottom->SetMaximum(abs(h_crLCE_rrZ_bottom->GetMinimum()));}
	// else do nothing; 
	c_crLCE_rrZ_3D->SetTheta(20.);
	c_crLCE_rrZ_3D->SetPhi(220.);
	if (file_outplot) c_crLCE_rrZ_3D->Write();
	//sprintf(canvasfile,"%s/%s_comparison_rLCE_rrZ_3D.%s", workingdirectory.c_str(),suffix.c_str(),export_format.c_str());
	//if (!(export_format=="")) c_crLCE_rrZ_3D->SaveAs(canvasfile);
	
	/*=================================================================*/
	// LCE vs. Z
	/*=================================================================*/
	style_1D->cd();
	TH1F* h_LCE_LCEZ = new TH1F("LCE_LCEZ", "MC: LCE vs. Z (ALL PMTs)", TPC.Get_nbinsZ(), TPC.Get_LXe_minZ(), TPC.Get_LXe_maxZ());
	h_LCE_LCEZ->Sumw2();
	h_LCE_LCEZ->Add(h_LCEZ_det_top, 1.);
	h_LCE_LCEZ->Add(h_LCEZ_det_bottom, 1.);
	h_LCE_LCEZ->Divide(h_LCEZ_gen);
	h_LCE_LCEZ->Scale(100.);
	
	/*=================================================================*/
	// LCE vs. Z (TOP PMTs)
	/*=================================================================*/
	style_1D->cd();
	TH1F* h_LCE_LCEZ_top = new TH1F("LCE_LCEZ_top", "MC: LCE vs. Z (TOP PMTs)", TPC.Get_nbinsZ(), TPC.Get_LXe_minZ(), TPC.Get_LXe_maxZ());
	h_LCE_LCEZ_top->Sumw2();
	h_LCE_LCEZ_top->Divide(h_LCEZ_det_top, h_LCEZ_gen, 1., 1., "b");
	h_LCE_LCEZ_top->Scale(100.);
	
	/*=================================================================*/
	// LCE vs. Z (BOTTOM PMTs)
	/*=================================================================*/
	style_1D->cd();
	TH1F* h_LCE_LCEZ_bottom = new TH1F("LCE_LCEZ_bottom", "MC: LCE vs. Z (BOTTOM PMTs)", TPC.Get_nbinsZ(), TPC.Get_LXe_minZ(), TPC.Get_LXe_maxZ());
	h_LCE_LCEZ_bottom->Sumw2();
	h_LCE_LCEZ_bottom->Divide(h_LCEZ_det_bottom, h_LCEZ_gen, 1., 1., "b");
	h_LCE_LCEZ_bottom->Scale(100.);
	
	/*=================================================================*/
	// ly vs. Z
	/*=================================================================*/
	style_1D->cd();
	TH1F* h_ly_lyZ = new TH1F("ly_lyZ", "MC: ly vs. Z (ALL PMTs)", TPC.Get_nbinsZ(), TPC.Get_LXe_minZ(), TPC.Get_LXe_maxZ());
	h_ly_lyZ->Sumw2();
	h_ly_lyZ->Add(h_LCEZ_det_top, 1.);
	h_ly_lyZ->Add(h_LCEZ_det_bottom, 1.);
	h_ly_lyZ->Divide(h_LCEZ_gen);
	h_ly_lyZ->Scale(50.);
	
	/*=================================================================*/
	// ly vs. Z (TOP PMTs)
	/*=================================================================*/
	style_1D->cd();
	TH1F* h_ly_lyZ_top = new TH1F("ly_lyZ_top", "MC: ly vs. Z (TOP PMTs)", TPC.Get_nbinsZ(), TPC.Get_LXe_minZ(), TPC.Get_LXe_maxZ());
	h_ly_lyZ_top->Sumw2();
	h_ly_lyZ_top->Divide(h_LCEZ_det_top, h_LCEZ_gen, 1., 1., "b");
	h_ly_lyZ_top->Scale(50.);
	
	/*=================================================================*/
	// ly vs. Z (BOTTOM PMTs)
	/*=================================================================*/
	style_1D->cd();
	TH1F* h_ly_lyZ_bottom = new TH1F("ly_lyZ_bottom", "MC: ly vs. Z (BOTTOM PMTs)", TPC.Get_nbinsZ(), TPC.Get_LXe_minZ(), TPC.Get_LXe_maxZ());
	h_ly_lyZ_bottom->Sumw2();
	h_ly_lyZ_bottom->Divide(h_LCEZ_det_bottom, h_LCEZ_gen, 1., 1., "b");
	h_ly_lyZ_bottom->Scale(50.);
	
	/*=================================================================*/
	// AFT vs. Z
	/*=================================================================*/
	style_1D->cd();
	TH1F* h_AFTZ_MC = new TH1F("AFTZ_MC", "MC: AFT vs. Z", TPC.Get_nbinsZ(), TPC.Get_LXe_minZ(), TPC.Get_LXe_maxZ());
	h_AFTZ_MC->Sumw2();
	h_AFTZ_MC->Divide(h_LCE_LCEZ_top, h_LCE_LCEZ, 1.,1., "b");
	h_AFTZ_MC->Scale(100.);
	
	/*=================================================================*/
	// ratio AFT vs. Z
	/*=================================================================*/
	style_1D->cd();
	TH1F* h_ratio_AFTZ = new TH1F("ratio_AFTZ", "ratio_AFTZ", TPC.Get_nbinsZ(), TPC.Get_LXe_minZ(), TPC.Get_LXe_maxZ());
	h_ratio_AFTZ->Sumw2();
	
	/*=================================================================*/
	// ratio rLCE
	/*=================================================================*/
	style_1D->cd();
	TH1F* h_ratio_rLCE = new TH1F("ratio_rLCE", "ratio_rLCE", TPC.Get_nbinsZ(), TPC.Get_LXe_minZ(), TPC.Get_LXe_maxZ());
	h_ratio_rLCE->Sumw2();
	
	/*=================================================================*/
	// ratio rLCE TOP PMTs
	/*=================================================================*/
	style_1D->cd();
	TH1F* h_ratio_rLCE_top = new TH1F("ratio_rLCE_top", "ratio_rLCE_top", TPC.Get_nbinsZ(), TPC.Get_LXe_minZ(), TPC.Get_LXe_maxZ());
	h_ratio_rLCE_top->Sumw2();
	
	/*=================================================================*/
	// ratio rLCE BOTTOM PMTs
	/*=================================================================*/
	style_1D->cd();
	TH1F* h_ratio_rLCE_bottom = new TH1F("ratio_rLCE_bottom", "ratio_rLCE_bottom", TPC.Get_nbinsZ(), TPC.Get_LXe_minZ(), TPC.Get_LXe_maxZ());
	h_ratio_rLCE_bottom->Sumw2();
	
	/*=================================================================*/
	// ratio ly
	/*=================================================================*/
	style_1D->cd();
	TH1F* h_ratio_ly = new TH1F("ratio_ly", "ratio_ly", TPC.Get_nbinsZ(), TPC.Get_LXe_minZ(), TPC.Get_LXe_maxZ());
	h_ratio_ly->Sumw2();
	
	/*=================================================================*/
	// ratio ly TOP PMTs
	/*=================================================================*/
	style_1D->cd();
	TH1F* h_ratio_ly_top = new TH1F("ratio_ly_top", "ratio_ly_top", TPC.Get_nbinsZ(), TPC.Get_LXe_minZ(), TPC.Get_LXe_maxZ());
	h_ratio_ly_top->Sumw2();
	
	/*=================================================================*/
	// ratio ly BOTTOM PMTs
	/*=================================================================*/
	style_1D->cd();
	TH1F* h_ratio_ly_bottom = new TH1F("ratio_ly_bottom", "ratio_ly_bottom", TPC.Get_nbinsZ(), TPC.Get_LXe_minZ(), TPC.Get_LXe_maxZ());
	h_ratio_ly_bottom->Sumw2();

	/*=================================================================*/
	if (!batch) {gROOT->SetBatch(kFALSE);}
	/*=================================================================*/
	/*=================================================================*/
	// comparison relative LCE vs. Z
	/*=================================================================*/
	TCanvas *c_crLCEZ_ALL = new TCanvas("crLCEZ_ALL","crLCEZ_ALL",canvas_x,canvas_y);
	TPad *top = new TPad("top", "top", 0., 0.4, 1., 1., 0, 0, 0);
	style_1D->cd();
	top->SetGridy();
	top->Draw();
	TPad *bottom = new TPad("bottom", "bottom", 0., 0., 1., 0.4, 0, 0, 0);
	style_1D->cd();
	bottom->SetGridy();
	bottom->SetTopMargin(0.02);
	bottom->SetBottomMargin(0.1);
	bottom->SetLeftMargin(0.105);
	bottom->Draw();
	top->cd();
	gStyle->SetPalette(NCont,ColPalette);
	style_1D->cd();
	double h_rLCE_mean = 0;
	for (int z=0; z<(TPC.Get_nbinsZ()); z++){
		h_rLCE_mean += h_LCE_LCEZ->GetBinContent(z+1)/(TPC.Get_nbinsZ());
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
	
	h_LCE_LCEZ->SetTitle("Comparison S1: relative LCE vs. Z");
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
	
	TPaveText *pt_crLCE_QE = new TPaveText(0.725,0.93,0.99,0.99,"NDC");
	pt_crLCE_QE->SetFillColor(0);   
	pt_crLCE_QE->SetBorderSize(1);
	pt_crLCE_QE->SetTextAlign(22);  
	sprintf(canvasfile,"MC_QE_top: %0.3f", TPC.Get_QE_top());
	pt_crLCE_QE->AddText(canvasfile);
	sprintf(canvasfile,"MC_QE_bottom: %0.3f", TPC.Get_QE_bottom());
	pt_crLCE_QE->AddText(canvasfile);
	pt_crLCE_QE->Draw();
	
	TLegend *leg_crLCE = new TLegend(0.49,0.6,0.99,0.925);
	leg_crLCE->SetFillColor(0);
	leg_crLCE->SetTextSize(0.04);
	leg_crLCE->SetBorderSize(1);
	leg_crLCE->SetTextAlign(22);         
	leg_crLCE->AddEntry(h_rLCE_LCEZ,"^{83m}Kr All PMTs","P"); 
	leg_crLCE->AddEntry(h_rLCE_LCEZ_top,"^{83m}Kr Top PMTs","P");
	leg_crLCE->AddEntry(h_rLCE_LCEZ_bottom,"^{83m}Kr Bottom PMTs","P"); 
	leg_crLCE->AddEntry(h_LCE_LCEZ,"MC All PMTs","l"); 
	leg_crLCE->AddEntry(h_LCE_LCEZ_top,"MC Top PMTs","l");
	leg_crLCE->AddEntry(h_LCE_LCEZ_bottom,"MC Bottom PMTs","l"); 
	leg_crLCE->Draw();  
	
	bottom->cd();
	gStyle->SetPalette(NCont,ColPalette);
	style_1D->cd();
	h_ratio_rLCE->Add(h_rLCE_LCEZ, 1.);
	h_ratio_rLCE->Add(h_LCE_LCEZ, -1.);
	h_ratio_rLCE_top->Add(h_rLCE_LCEZ_top, 1.);
	h_ratio_rLCE_top->Add(h_LCE_LCEZ_top, -1.);
	h_ratio_rLCE_bottom->Add(h_rLCE_LCEZ_bottom, 1.);
	h_ratio_rLCE_bottom->Add(h_LCE_LCEZ_bottom, -1.);
	h_ratio_rLCE->SetTitle("");
	h_ratio_rLCE->SetXTitle("Z [cm]");
	h_ratio_rLCE->GetXaxis()->CenterTitle();
	h_ratio_rLCE->SetYTitle("#Delta rLCE");
	h_ratio_rLCE->GetYaxis()->CenterTitle();
	h_ratio_rLCE->SetTitleOffset(1.,"X");
	h_ratio_rLCE->SetTitleOffset(1.,"Y");
	h_ratio_rLCE->GetYaxis()->SetTitleSize(0.05);
	h_ratio_rLCE->GetYaxis()->SetLabelSize(0.05);
	h_ratio_rLCE->GetXaxis()->SetTitleSize(0.05);
	h_ratio_rLCE->GetXaxis()->SetLabelSize(0.05);
	h_ratio_rLCE->SetLineColor(kBlue);
	h_ratio_rLCE->GetYaxis()->SetRangeUser(-0.25,0.25);
	h_ratio_rLCE->SetFillStyle(3001);
	h_ratio_rLCE->SetFillColor(kBlue);
	h_ratio_rLCE->Draw("hist same");
	h_ratio_rLCE_bottom->SetLineColor(kGreen);
	h_ratio_rLCE_bottom->SetFillStyle(3002);
	h_ratio_rLCE_bottom->SetFillColor(kGreen);
	h_ratio_rLCE_bottom->Draw("hist same");
	h_ratio_rLCE_top->SetLineColor(kRed);
	h_ratio_rLCE_top->SetFillStyle(3003);
	h_ratio_rLCE_top->SetFillColor(kRed);
	h_ratio_rLCE_top->Draw("hist same");
	
	double h_ratio_AFTZ_sos_all = 0; //sum of squares
	double h_ratio_AFTZ_md_all = 0; //maximum deviation
	double h_ratio_AFTZ_sos_top = 0; //sum of squares
	double h_ratio_AFTZ_md_top = 0; //maximum deviation
	double h_ratio_AFTZ_sos_bottom = 0; //sum of squares
	double h_ratio_AFTZ_md_bottom = 0; //maximum deviation
	for (int z=0; z<TPC.Get_nbinsZ(); z++){
		h_ratio_AFTZ_sos_all += h_ratio_rLCE->GetBinContent(z)*h_ratio_rLCE->GetBinContent(z);
		if (abs(h_ratio_rLCE->GetBinContent(z)) > h_ratio_AFTZ_md_all) {h_ratio_AFTZ_md_all = abs(h_ratio_rLCE->GetBinContent(z));}
		h_ratio_AFTZ_sos_top += h_ratio_rLCE_top->GetBinContent(z)*h_ratio_rLCE_top->GetBinContent(z);
		if (abs(h_ratio_rLCE_top->GetBinContent(z)) > h_ratio_AFTZ_md_top) {h_ratio_AFTZ_md_top = abs(h_ratio_rLCE_top->GetBinContent(z));}
		h_ratio_AFTZ_sos_bottom += h_ratio_rLCE_bottom->GetBinContent(z)*h_ratio_rLCE_bottom->GetBinContent(z);
		if (abs(h_ratio_rLCE_bottom->GetBinContent(z)) > h_ratio_AFTZ_md_bottom) {h_ratio_AFTZ_md_bottom = abs(h_ratio_rLCE_bottom->GetBinContent(z));}
	}
	
	TPaveText *pt_ratio_rLCE = new TPaveText(0.105,0.88,0.305,0.98,"NDC");
	pt_ratio_rLCE->SetFillColor(0);   
	pt_ratio_rLCE->SetBorderSize(1);
	pt_ratio_rLCE->SetTextAlign(32);  
	sprintf(canvasfile,"sum of squares:");
	pt_ratio_rLCE->AddText(canvasfile);
	sprintf(canvasfile,"maximum deviation:");
	pt_ratio_rLCE->AddText(canvasfile);
	pt_ratio_rLCE->Draw();
	
	TPaveText *pt_ratio_rLCE_all = new TPaveText(0.305,0.88,0.375,0.98,"NDC");
	pt_ratio_rLCE_all->SetFillColor(0);   
	pt_ratio_rLCE_all->SetBorderSize(1);
	pt_ratio_rLCE_all->SetTextAlign(22);
	pt_ratio_rLCE_all->SetTextColor(kBlue);	
	sprintf(canvasfile,"%0.3f", h_ratio_AFTZ_sos_all);
	pt_ratio_rLCE_all->AddText(canvasfile);
	sprintf(canvasfile,"%0.3f", h_ratio_AFTZ_md_all);
	pt_ratio_rLCE_all->AddText(canvasfile);
	pt_ratio_rLCE_all->Draw();
	
	TPaveText *pt_ratio_rLCE_top = new TPaveText(0.375,0.88,0.445,0.98,"NDC");
	pt_ratio_rLCE_top->SetFillColor(0);   
	pt_ratio_rLCE_top->SetBorderSize(1);
	pt_ratio_rLCE_top->SetTextAlign(22);
	pt_ratio_rLCE_top->SetTextColor(kRed);	
	sprintf(canvasfile,"%0.3f", h_ratio_AFTZ_sos_top);
	pt_ratio_rLCE_top->AddText(canvasfile);
	sprintf(canvasfile,"%0.3f", h_ratio_AFTZ_md_top);
	pt_ratio_rLCE_top->AddText(canvasfile);
	pt_ratio_rLCE_top->Draw();
	
	TPaveText *pt_ratio_rLCE_bottom = new TPaveText(0.445,0.88,0.515,0.98,"NDC");
	pt_ratio_rLCE_bottom->SetFillColor(0);   
	pt_ratio_rLCE_bottom->SetBorderSize(1);
	pt_ratio_rLCE_bottom->SetTextAlign(22);
	pt_ratio_rLCE_bottom->SetTextColor(kGreen);	
	sprintf(canvasfile,"%0.3f", h_ratio_AFTZ_sos_bottom);
	pt_ratio_rLCE_bottom->AddText(canvasfile);
	sprintf(canvasfile,"%0.3f", h_ratio_AFTZ_md_bottom);
	pt_ratio_rLCE_bottom->AddText(canvasfile);
	pt_ratio_rLCE_bottom->Draw();
	
	TLegend *leg_crLCE_ratio = new TLegend(0.69,0.8,0.99,0.98);
	leg_crLCE_ratio->SetFillColor(0);
	leg_crLCE_ratio->SetTextSize(0.04);
	leg_crLCE_ratio->SetBorderSize(1);
	leg_crLCE_ratio->SetTextAlign(22);         
	leg_crLCE_ratio->AddEntry(h_ratio_rLCE,"All PMTs","l"); 
	leg_crLCE_ratio->AddEntry(h_ratio_rLCE_top,"Top PMTs","l");
	leg_crLCE_ratio->AddEntry(h_ratio_rLCE_bottom,"Bottom PMTs","l"); 
	leg_crLCE_ratio->Draw();
	
	if (file_outplot) c_crLCEZ_ALL->Write();	
	sprintf(canvasfile,"%s/comparison_%s_vs_%s_rLCEz.%s", workingdirectory.c_str(), rawdatafilename_mc.c_str(), rawdatafilename_kr.c_str(),export_format.c_str());
	if (!(export_format=="")) c_crLCEZ_ALL->SaveAs(canvasfile);
	
	/*=================================================================*/
	// comparison ly vs. Z
	/*=================================================================*/
	TCanvas *c_clyZ_ALL = new TCanvas("clyZ_ALL","clyZ_ALL",canvas_x,canvas_y);
	TPad *ly_top = new TPad("ly_top", "ly_top", 0., 0.4, 1., 1., 0, 0, 0);
	style_1D->cd();
	ly_top->SetGridy();
	ly_top->Draw();
	TPad *ly_bottom = new TPad("ly_bottom", "ly_bottom", 0., 0., 1., 0.4, 0, 0, 0);
	style_1D->cd();
	ly_bottom->SetGridy();
	ly_bottom->SetTopMargin(0.02);
	ly_bottom->SetBottomMargin(0.1);
	ly_bottom->SetLeftMargin(0.105);
	ly_bottom->Draw();
	ly_top->cd();
	gStyle->SetPalette(NCont,ColPalette);
	style_1D->cd();
	h_ly_lyZ->SetXTitle("Z [cm]");
	h_ly_lyZ->GetXaxis()->CenterTitle();
	h_ly_lyZ->SetYTitle("ly [pe/keV]");
	h_ly_lyZ->GetYaxis()->CenterTitle();
	
	h_ly_lyZ->SetTitle("Comparison S1: ly vs. Z");
	h_ly_lyZ->SetLineColor(kBlue);
	h_ly_lyZ->GetYaxis()->SetRangeUser(0,10);
	h_ly_lyZ->Draw("");
	h_ly_lyZ_bottom->SetLineColor(kGreen);
	h_ly_lyZ_bottom->Draw("same");
	h_ly_lyZ_top->SetLineColor(kRed);
	h_ly_lyZ_top->Draw("same");
	
	h_Kr_LCE_LCEZ->SetLineColor(kBlue);
	h_Kr_LCE_LCEZ->SetMarkerColor(kBlue);
	h_Kr_LCE_LCEZ->SetMarkerStyle(8);
	h_Kr_LCE_LCEZ->Draw("P same");
	h_Kr_LCE_LCEZ_top->SetLineColor(kRed);
	h_Kr_LCE_LCEZ_top->SetMarkerColor(kRed);
	h_Kr_LCE_LCEZ_top->SetMarkerStyle(8);
	h_Kr_LCE_LCEZ_top->Draw("P same");
	h_Kr_LCE_LCEZ_bottom->SetLineColor(kGreen);
	h_Kr_LCE_LCEZ_bottom->SetMarkerColor(kGreen);
	h_Kr_LCE_LCEZ_bottom->SetMarkerStyle(8);
	h_Kr_LCE_LCEZ_bottom->Draw("P same");
	
	TPaveText *pt_cly_QE = new TPaveText(0.725,0.93,0.99,0.99,"NDC");
	pt_cly_QE->SetFillColor(0);   
	pt_cly_QE->SetBorderSize(1);
	pt_cly_QE->SetTextAlign(22);  
	sprintf(canvasfile,"MC_QE_top: %0.3f", TPC.Get_QE_top());
	pt_cly_QE->AddText(canvasfile);
	sprintf(canvasfile,"MC_QE_bottom: %0.3f", TPC.Get_QE_bottom());
	pt_cly_QE->AddText(canvasfile);
	pt_cly_QE->Draw();
	
	TLegend *leg_clyZ = new TLegend(0.49,0.6,0.99,0.925);
	leg_clyZ->SetFillColor(0);
	leg_clyZ->SetTextSize(0.04);
	leg_clyZ->SetBorderSize(1);
	leg_clyZ->SetTextAlign(22);         
	leg_clyZ->AddEntry(h_Kr_LCE_LCEZ,"^{83m}Kr All PMTs","P"); 
	leg_clyZ->AddEntry(h_Kr_LCE_LCEZ_top,"^{83m}Kr Top PMTs","P");
	leg_clyZ->AddEntry(h_Kr_LCE_LCEZ_bottom,"^{83m}Kr Bottom PMTs","P"); 
	leg_clyZ->AddEntry(h_ly_lyZ,"MC All PMTs","l"); 
	leg_clyZ->AddEntry(h_ly_lyZ_top,"MC Top PMTs","l");
	leg_clyZ->AddEntry(h_ly_lyZ_bottom,"MC Bottom PMTs","l"); 
	leg_clyZ->Draw();  
	
	ly_bottom->cd();
	gStyle->SetPalette(NCont,ColPalette);
	style_1D->cd();
	h_ratio_ly->Add(h_Kr_LCE_LCEZ, 1.);
	h_ratio_ly->Add(h_ly_lyZ, -1.);
	h_ratio_ly_top->Add(h_Kr_LCE_LCEZ_top, 1.);
	h_ratio_ly_top->Add(h_ly_lyZ_top, -1.);
	h_ratio_ly_bottom->Add(h_Kr_LCE_LCEZ_bottom, 1.);
	h_ratio_ly_bottom->Add(h_ly_lyZ_bottom, -1.);
	h_ratio_ly->SetTitle("");
	h_ratio_ly->SetXTitle("Z [cm]");
	h_ratio_ly->GetXaxis()->CenterTitle();
	h_ratio_ly->SetYTitle("#Delta ly");
	h_ratio_ly->GetYaxis()->CenterTitle();
	h_ratio_ly->SetTitleOffset(1.,"X");
	h_ratio_ly->SetTitleOffset(1.,"Y");
	h_ratio_ly->GetYaxis()->SetTitleSize(0.05);
	h_ratio_ly->GetYaxis()->SetLabelSize(0.05);
	h_ratio_ly->GetXaxis()->SetTitleSize(0.05);
	h_ratio_ly->GetXaxis()->SetLabelSize(0.05);
	h_ratio_ly->SetLineColor(kBlue);
	h_ratio_ly->GetYaxis()->SetRangeUser(-1,1);
	h_ratio_ly->SetFillStyle(3001);
	h_ratio_ly->SetFillColor(kBlue);
	h_ratio_ly->Draw("hist same");
	h_ratio_ly_bottom->SetLineColor(kGreen);
	h_ratio_ly_bottom->SetFillStyle(3002);
	h_ratio_ly_bottom->SetFillColor(kGreen);
	h_ratio_ly_bottom->Draw("hist same");
	h_ratio_ly_top->SetLineColor(kRed);
	h_ratio_ly_top->SetFillStyle(3003);
	h_ratio_ly_top->SetFillColor(kRed);
	h_ratio_ly_top->Draw("hist same");
	
	double h_ratio_ly_sos_all = 0; //sum of squares
	double h_ratio_ly_md_all = 0; //maximum deviation
	double h_ratio_ly_sos_top = 0; //sum of squares
	double h_ratio_ly_md_top = 0; //maximum deviation
	double h_ratio_ly_sos_bottom = 0; //sum of squares
	double h_ratio_ly_md_bottom = 0; //maximum deviation
	for (int z=0; z<TPC.Get_nbinsZ(); z++){
		h_ratio_ly_sos_all += h_ratio_ly->GetBinContent(z)*h_ratio_ly->GetBinContent(z);
		if (abs(h_ratio_ly->GetBinContent(z)) > h_ratio_ly_md_all) {h_ratio_ly_md_all = abs(h_ratio_ly->GetBinContent(z));}
		h_ratio_ly_sos_top += h_ratio_ly_top->GetBinContent(z)*h_ratio_ly_top->GetBinContent(z);
		if (abs(h_ratio_ly_top->GetBinContent(z)) > h_ratio_ly_md_top) {h_ratio_ly_md_top = abs(h_ratio_ly_top->GetBinContent(z));}
		h_ratio_ly_sos_bottom += h_ratio_ly_bottom->GetBinContent(z)*h_ratio_ly_bottom->GetBinContent(z);
		if (abs(h_ratio_ly_bottom->GetBinContent(z)) > h_ratio_ly_md_bottom) {h_ratio_ly_md_bottom = abs(h_ratio_ly_bottom->GetBinContent(z));}
	}
	
	TPaveText *pt_ratio_ly = new TPaveText(0.105,0.88,0.305,0.98,"NDC");
	pt_ratio_ly->SetFillColor(0);   
	pt_ratio_ly->SetBorderSize(1);
	pt_ratio_ly->SetTextAlign(32);  
	sprintf(canvasfile,"sum of squares:");
	pt_ratio_ly->AddText(canvasfile);
	sprintf(canvasfile,"maximum deviation:");
	pt_ratio_ly->AddText(canvasfile);
	pt_ratio_ly->Draw();
	
	TPaveText *pt_ratio_ly_all = new TPaveText(0.305,0.88,0.375,0.98,"NDC");
	pt_ratio_ly_all->SetFillColor(0);   
	pt_ratio_ly_all->SetBorderSize(1);
	pt_ratio_ly_all->SetTextAlign(22);
	pt_ratio_ly_all->SetTextColor(kBlue);	
	sprintf(canvasfile,"%0.3f", h_ratio_ly_sos_all);
	pt_ratio_ly_all->AddText(canvasfile);
	sprintf(canvasfile,"%0.3f", h_ratio_ly_md_all);
	pt_ratio_ly_all->AddText(canvasfile);
	pt_ratio_ly_all->Draw();
	
	TPaveText *pt_ratio_ly_top = new TPaveText(0.375,0.88,0.445,0.98,"NDC");
	pt_ratio_ly_top->SetFillColor(0);   
	pt_ratio_ly_top->SetBorderSize(1);
	pt_ratio_ly_top->SetTextAlign(22);
	pt_ratio_ly_top->SetTextColor(kRed);	
	sprintf(canvasfile,"%0.3f", h_ratio_ly_sos_top);
	pt_ratio_ly_top->AddText(canvasfile);
	sprintf(canvasfile,"%0.3f", h_ratio_ly_md_top);
	pt_ratio_ly_top->AddText(canvasfile);
	pt_ratio_ly_top->Draw();
	
	TPaveText *pt_ratio_ly_bottom = new TPaveText(0.445,0.88,0.515,0.98,"NDC");
	pt_ratio_ly_bottom->SetFillColor(0);   
	pt_ratio_ly_bottom->SetBorderSize(1);
	pt_ratio_ly_bottom->SetTextAlign(22);
	pt_ratio_ly_bottom->SetTextColor(kGreen);	
	sprintf(canvasfile,"%0.3f", h_ratio_ly_sos_bottom);
	pt_ratio_ly_bottom->AddText(canvasfile);
	sprintf(canvasfile,"%0.3f", h_ratio_ly_md_bottom);
	pt_ratio_ly_bottom->AddText(canvasfile);
	pt_ratio_ly_bottom->Draw();
	
	TLegend *leg_cly_ratio = new TLegend(0.69,0.8,0.99,0.98);
	leg_cly_ratio->SetFillColor(0);
	leg_cly_ratio->SetTextSize(0.04);
	leg_cly_ratio->SetBorderSize(1);
	leg_cly_ratio->SetTextAlign(22);         
	leg_cly_ratio->AddEntry(h_ratio_ly,"All PMTs","l"); 
	leg_cly_ratio->AddEntry(h_ratio_ly_top,"Top PMTs","l");
	leg_cly_ratio->AddEntry(h_ratio_ly_bottom,"Bottom PMTs","l"); 
	leg_cly_ratio->Draw();
	
	if (file_outplot) c_clyZ_ALL->Write();	
	sprintf(canvasfile,"%s/comparison_%s_vs_%s_lyz.%s", workingdirectory.c_str(), rawdatafilename_mc.c_str(), rawdatafilename_kr.c_str(),export_format.c_str());
	if (!(export_format=="")) c_clyZ_ALL->SaveAs(canvasfile);
	
	/*=================================================================*/
	// comparison AFT vs. Z
	/*=================================================================*/
	gStyle->SetPalette(NCont,ColPalette);
	style_1D->cd();
	TCanvas *c_cAFTZ = new TCanvas("cAFTZ","cAFTZ",canvas_x,canvas_y);
	TPad *top_AFTZ = new TPad("top_AFTZ", "top_AFTZ", 0., 0.4, 1., 1., 0, 0, 0);
	style_1D->cd();
	top_AFTZ->SetGridy();
	top_AFTZ->SetTopMargin(0.095);
	top_AFTZ->Draw();
	TPad *bottom_AFTZ = new TPad("bottom_AFTZ", "bottom_AFTZ", 0., 0., 1., 0.4, 0, 0, 0);
	style_1D->cd();
	bottom_AFTZ->SetGridy();
	bottom_AFTZ->SetTopMargin(0.02);
	bottom_AFTZ->SetBottomMargin(0.1);
	bottom_AFTZ->SetLeftMargin(0.105);
	bottom_AFTZ->Draw();
	top_AFTZ->cd();
	gStyle->SetPalette(NCont,ColPalette);
	style_1D->cd();
	h_AFTZ_MC->SetTitle("Comparison S1/S2: AreaFractionTop vs. Z");
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
	
	TPaveText *pt_cAFTZ_QE = new TPaveText(0.725,0.91,0.99,0.99,"NDC");
	pt_cAFTZ_QE->SetFillColor(0);   
	pt_cAFTZ_QE->SetBorderSize(1);
	pt_cAFTZ_QE->SetTextAlign(22);  
	sprintf(canvasfile,"MC_QE_top: %0.3f", TPC.Get_QE_top());
	pt_cAFTZ_QE->AddText(canvasfile);
	sprintf(canvasfile,"MC_QE_bottom: %0.3f", TPC.Get_QE_bottom());
	pt_cAFTZ_QE->AddText(canvasfile);
	pt_cAFTZ_QE->Draw();
	
	if (AFT_S2 > 0) {
		TPaveText *pt_AFT_S2 = new TPaveText(0.105,0.80,0.375,0.905,"NDC");
		pt_AFT_S2->SetFillColor(0);   
		pt_AFT_S2->SetBorderSize(1);
		pt_AFT_S2->SetTextAlign(22);  
		sprintf(canvasfile,"^{83m}Kr data AFT S2: %0.3f", AFT_S2_Kr);
		pt_AFT_S2->AddText(canvasfile);
		sprintf(canvasfile,"MC data AFT S2: %0.3f", AFT_S2);
		pt_AFT_S2->AddText(canvasfile);
		pt_AFT_S2->Draw();		
	}

	TLegend *leg_cAFTZ = new TLegend(0.59,0.75,0.99,0.905);
	leg_cAFTZ->SetFillColor(0);
	leg_cAFTZ->SetTextSize(0.04);
	leg_cAFTZ->SetBorderSize(1);
	leg_cAFTZ->SetTextAlign(22);         
	leg_cAFTZ->AddEntry(h_AFTZ_Kr,"^{83m}Kr data","P"); 
	leg_cAFTZ->AddEntry(h_AFTZ_MC,"MC","l"); 
	leg_cAFTZ->Draw();    
	
	bottom_AFTZ->cd();
	gStyle->SetPalette(NCont,ColPalette);
	style_1D->cd();
	h_ratio_AFTZ->Add(h_AFTZ_Kr, 1.);
	h_ratio_AFTZ->Add(h_AFTZ_MC, -1.);
	
	double h_ratio_AFTZ_sos = 0; //sum of squares
	double h_ratio_AFTZ_md = 0; //maximum deviation
	for (int z=0; z<TPC.Get_nbinsZ(); z++){
		h_ratio_AFTZ_sos += h_ratio_AFTZ->GetBinContent(z)*h_ratio_AFTZ->GetBinContent(z);
		if (abs(h_ratio_AFTZ->GetBinContent(z)) > h_ratio_AFTZ_md) {h_ratio_AFTZ_md = abs(h_ratio_AFTZ->GetBinContent(z));}
	}
	
	h_ratio_AFTZ->SetTitle("");
	h_ratio_AFTZ->SetXTitle("Z [cm]");
	h_ratio_AFTZ->GetXaxis()->CenterTitle();
	h_ratio_AFTZ->SetYTitle("#Delta AreaFractionTop [%]");
	h_ratio_AFTZ->GetYaxis()->CenterTitle();
	h_ratio_AFTZ->SetTitleOffset(1.,"X");
	h_ratio_AFTZ->SetTitleOffset(1.,"Y");
	h_ratio_AFTZ->GetYaxis()->SetTitleSize(0.05);
	h_ratio_AFTZ->GetYaxis()->SetLabelSize(0.05);
	h_ratio_AFTZ->GetXaxis()->SetTitleSize(0.05);
	h_ratio_AFTZ->GetXaxis()->SetLabelSize(0.05);
	h_ratio_AFTZ->SetLineColor(kRed);
	h_ratio_AFTZ->GetYaxis()->SetRangeUser(-10.,10.);
	h_ratio_AFTZ->SetFillStyle(3002);
	h_ratio_AFTZ->SetFillColor(kRed);
	h_ratio_AFTZ->Draw("hist same");
	
	TPaveText *pt_ratio_AFTZ = new TPaveText(0.105,0.88,0.375,0.98,"NDC");
	pt_ratio_AFTZ->SetFillColor(0);   
	pt_ratio_AFTZ->SetBorderSize(1);
	pt_ratio_AFTZ->SetTextAlign(22);  
	sprintf(canvasfile,"sum of squares: %0.2f", h_ratio_AFTZ_sos);
	pt_ratio_AFTZ->AddText(canvasfile);
	sprintf(canvasfile,"maximum deviation: %0.2f", h_ratio_AFTZ_md);
	pt_ratio_AFTZ->AddText(canvasfile);
	pt_ratio_AFTZ->Draw();

	if (file_outplot) c_cAFTZ->Write();	
	sprintf(canvasfile,"%s/comparison_%s_vs_%s_AFTz.%s", workingdirectory.c_str(), rawdatafilename_mc.c_str(), rawdatafilename_kr.c_str(),export_format.c_str());
	if (!(export_format=="")) c_cAFTZ->SaveAs(canvasfile);
	
	/*=================================================================*/
	gROOT->SetBatch(kTRUE);
	/*=================================================================*/

	/*=================================================================*/
	// AFT vs. rr
	/*=================================================================*/
	style_1D->cd();
	TH1F* h_AFTrr_MC = new TH1F("AFTrr_MC", "MC: AFT vs. R^{2}", TPC.Get_nbinsR(), 0., TPC.Get_LXe_maxRR());
	h_AFTrr_MC->Sumw2();
	h_AFTrr_MC->Divide(h_LCErr_det_top, h_LCErr_det, 1., 1., "b");
	h_AFTrr_MC->Scale(100.);
	
	/*=================================================================*/
	// comparison AFT vs. rr
	/*=================================================================*/
	gStyle->SetPalette(NCont,ColPalette);
	style_1D->cd();
	TCanvas *c_cAFTrr = new TCanvas("cAFTrr","cAFTrr",canvas_x,canvas_y);
	c_cAFTrr->SetGridy();
	h_AFTrr_MC->SetTitle("Comparison S1: AreaFractionTop vs. R^{2}");
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

	TPaveText *pt_cAFTrr_QE = new TPaveText(0.725,0.93,0.99,0.99,"NDC");
	pt_cAFTrr_QE->SetFillColor(0);   
	pt_cAFTrr_QE->SetBorderSize(1);
	pt_cAFTrr_QE->SetTextAlign(22);  
	sprintf(canvasfile,"MC_QE_top: %0.3f", TPC.Get_QE_top());
	pt_cAFTrr_QE->AddText(canvasfile);
	sprintf(canvasfile,"MC_QE_bottom: %0.3f", TPC.Get_QE_bottom());
	pt_cAFTrr_QE->AddText(canvasfile);
	pt_cAFTrr_QE->Draw();
	
	TLegend *leg_cAFTrr = new TLegend(0.59,0.85,0.99,0.925);
	leg_cAFTrr->SetFillColor(0);
	leg_cAFTrr->SetTextSize(0.04);
	leg_cAFTrr->SetBorderSize(1);
	leg_cAFTrr->SetTextAlign(22);         
	leg_cAFTrr->AddEntry(h_AFTrr_Kr,"^{83m}Kr data","P"); 
	leg_cAFTrr->AddEntry(h_AFTrr_MC,"MC","l"); 
	leg_cAFTrr->Draw();    

	if (file_outplot) c_cAFTrr->Write();	
	sprintf(canvasfile,"%s/comparison_%s_vs_%s_AFTrr.%s", workingdirectory.c_str(), rawdatafilename_mc.c_str(), rawdatafilename_kr.c_str(),export_format.c_str());
	if (!(export_format=="")) c_cAFTrr->SaveAs(canvasfile);
	
	gStyle->SetPalette(NCont,ColPalette);
}