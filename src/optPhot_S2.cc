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
#include <fstream>
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

void optPhot_S2(string, int, int, int, string);
void optPhot_S2(string, int, int, int, string, bool);

/*=================================================================*/
void optPhot_S2(string datafile, string export_format) {
	// Some good binnings
	//TPC.Set_Bins(26,50,22) - default
	//TPC.Set_Bins(52,100,44)- nevents > 10000000
	optPhot_S2(datafile,26,50,22,export_format,true);
}

void optPhot_S2(string datafile, int bin_z, int bin_r, int bin_rr, string export_format) {
	optPhot_S2(datafile,bin_z,bin_r,bin_rr,export_format,true);
}

/*=================================================================*/

void optPhot_S2(string datafile, int bin_z, int bin_r, int bin_rr, string export_format, bool batch) {
	
	//gErrorIgnoreLevel = kPrint, kInfo, kWarning, kError, kBreak, kSysError, kFatal;
	gErrorIgnoreLevel = kWarning;
	
	// read in datafilename and get working directory
	size_t found=datafile.find_last_of("/\\");
	string workingdirectory = datafile.substr(0,found);
	string datafilename = datafile.substr(found+1);
	string rawdatafilename = "";
	if (datafilename == "") {
		rawdatafilename = "optPhot";
	} else {
		size_t lastindex = datafilename.find_last_of("."); 
		rawdatafilename = datafilename.substr(0, lastindex); 
	}
	
	Int_t canvas_x = 650;
	Int_t canvas_y = 800;

	char canvasfile[10000];
	char draw_selection[10000];
	
	TPC_Definition TPC(bin_z, bin_r, bin_rr);
	TChain *file_input_tree = new TChain("events/events");
	TNamed *G4MCname;
	
	char file_outname[10000];
	sprintf(file_outname,"%s/%s_S2.dat", workingdirectory.c_str(), rawdatafilename.c_str());
	
	ofstream file_outstat;
	file_outstat.open(file_outname);
	file_outstat << "============================================================" << "\n";
	sprintf(file_outname,"%s_S2.root", rawdatafilename.c_str());
	int nevents = 0;
	
	if (fileexists(datafile) == false) {
		cout << endl;
		cout << "x Error xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx" << endl;
		cout << "file not found:" << endl;
		cout << "-> " << datafile << endl;
		cout << "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx" << endl;
		cout << endl;
		gApplication->Terminate();
	}
	else if ((fileexists(datafile) == true) && (datafilename=="")) {
		file_outstat << "= reading datafiles ==== dir mode ==========================" << "\n";
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
				if (!file->IsDirectory() && fname.EndsWith(ext.c_str()) && !(fname == file_outname)) {
					char filename[10000];
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
					file_outstat << " file: " << fname.Data() << " " << nevents << " events total" << "\n";
					cout << " file: " << fname.Data() << " " << nevents << " events total" << endl;
				}
			}
		}		
	}
	else {
		file_outstat << "= reading datafile ===== single file =======================" << "\n";
		cout << "= reading datafile ===== single file =======================" << endl;
		if (file_input_tree->GetEntries() == 0) {
			TFile *f = new TFile(datafile.c_str(),"READ");
			if (f->GetListOfKeys()->Contains("MC_TAG")) {
				f->GetObject("MC_TAG",G4MCname);
			}
			else {
				G4MCname = new TNamed("MC_TAG","Xenon1t");
			}
			TPC.Init(G4MCname);
			f->Close();
		}
		file_input_tree->AddFile(datafile.c_str()); 
		nevents = file_input_tree->GetEntries();
		file_outstat << " file: " << datafilename << " " << nevents << " events " << "\n";
		cout << " file: " << datafilename << " " << nevents << " events " << endl;
	}
	file_outstat << "============================================================" << "\n";
	cout << "============================================================" << endl;
	
	file_input_tree->SetAlias("rrp_pri","(xp_pri*xp_pri + yp_pri*yp_pri)/10./10.");  
	
	// generate plots  
	sprintf(file_outname,"%s/%s_S2.root", workingdirectory.c_str(), rawdatafilename.c_str());
	TFile *file_outplot = new TFile(file_outname,"RECREATE");
	
	file_outstat << "= geometry parameters ======================================" << "\n";
	file_outstat << "binning: " << bin_z << " " << bin_r << " " << bin_rr << "\n";
	file_outstat << "chamber_minZ: " << TPC.Get_chamber_minZ() << "\n";
	file_outstat << "chamber_maxZ: " << TPC.Get_chamber_maxZ() << "\n";
	file_outstat << "chamber_minRR: " << TPC.Get_chamber_minRR() << "\n";
	file_outstat << "chamber_maxRR: " << TPC.Get_chamber_maxRR() << "\n";
	file_outstat << "GXe_minZ: " << TPC.Get_LXe_minZ() << "\n";
	file_outstat << "GXe_maxZ: " << TPC.Get_LXe_maxZ() << "\n";
	file_outstat << "GXe_minRR: " << TPC.Get_LXe_minRR() << "\n";
	file_outstat << "GXe_maxRR: " << TPC.Get_LXe_maxRR() << "\n";
	file_outstat << "LCE_min: " << TPC.Get_LCE_min() << "\n";
	file_outstat << "LCE_max: " << TPC.Get_LCE_max() << "\n";
	file_outstat << "============================================================" << "\n";
	
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
	style_2D->SetPadRightMargin(0.15);
	style_2D->SetPadTopMargin(0.075);
	style_2D->SetPadBottomMargin(0.075);
	
	style_2D->SetTitleOffset(1.,"X");
	style_2D->SetTitleOffset(1.45,"Y");
	style_2D->SetTitleOffset(1.35,"Z");
	
	style_2D->SetPalette(NCont,ColPalette);
	style_2D->SetNumberContours(NCont);
	style_2D->cd();
	
	gStyle->SetPalette(NCont,ColPalette);
	
	file_outstat << "= analyse ttree ============================================" << "\n";
	file_outstat << "generated events: " << file_input_tree->GetEntries() << "\n";
	
	file_input_tree->Draw(">>elist_top","(ntpmthits > 0)","goff");
	TEntryList *elist_top = (TEntryList*)gDirectory->Get("elist_top");
	file_outstat << "Top PMT hits: " << elist_top->GetEntriesToProcess() << "\n";
	
	file_input_tree->Draw(">>elist_all","(nbpmthits > 0 || ntpmthits > 0)","goff");
	TEntryList *elist_all = (TEntryList*)gDirectory->Get("elist_all");
	file_outstat << "All PMT hits: " << elist_all->GetEntriesToProcess() << "\n";
	file_outstat << "AFT: " << (double)elist_top->GetEntriesToProcess()/(double)elist_all->GetEntriesToProcess() << "\n";
	
	file_outstat << "============================================================" << "\n";
	
	file_outstat << "============================================================" << "\n";
	file_outstat.close();	
	//file_outplot->Close(); 
}

/*=================================================================*/
