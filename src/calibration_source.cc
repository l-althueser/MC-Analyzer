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

void calibration_source(string, int, int, int, string);
void calibration_source(string, int, int, int, string, bool);

/*=================================================================*/
void calibration_source(string datafile, string export_format) {
	// Some good binnings
	//TPC.Set_Bins(26,50,22) - default
	//TPC.Set_Bins(52,100,44)- nevents > 10000000
	calibration_source(datafile,50,45,22,export_format,true);
}

void calibration_source(string datafile, int bin_z, int bin_r, int bin_rr, string export_format) {
	calibration_source(datafile,bin_z,bin_r,bin_rr,export_format,true);
}
/*=================================================================*/

void calibration_source(string datafile, int bin_z, int bin_r, int bin_rr, string export_format, bool batch) {
	// read in datafilename and get working directory
	size_t found=datafile.find_last_of("/\\");
	string workingdirectory = datafile.substr(0,found);
	string datafilename = datafile.substr(found+1);
	size_t lastindex = datafilename.find_last_of("."); 
	string rawdatafilename = datafilename.substr(0, lastindex); 
	
	Int_t canvas_x = 850;
	Int_t canvas_y = 800;

	char canvasfile[10000];
	char draw_selection[10000];
	
	TPC_Definition TPC(bin_z, bin_r, bin_rr);
	TChain *file_input_tree = new TChain("events/events");

	char file_outname[10000];
	sprintf(file_outname,"%s/%s_calsource.dat", workingdirectory.c_str(), rawdatafilename.c_str());
	
	ofstream file_outstat;
	file_outstat.open(file_outname);
	file_outstat << "============================================================" << "\n";
	
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
				if (!file->IsDirectory() && fname.EndsWith(ext.c_str()) && !(fname == datafilename)) {
					char filename[10000];
					sprintf(filename,"%s/%s", workingdirectory.c_str(), fname.Data());
					
					if (file_input_tree->GetEntries() == 0) {
						TFile *f = new TFile(filename,"READ");
						TNamed *G4MCname;
						f->GetObject("MC_TAG",G4MCname);
						TPC.Init(G4MCname);
						f->Close();
					}
					
					file_input_tree->AddFile(filename); 
					const int nevents = file_input_tree->GetEntries();
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
			TNamed *G4MCname;
			f->GetObject("MC_TAG",G4MCname);
			TPC.Init(G4MCname);
			f->Close();
		}
		file_input_tree->AddFile(datafile.c_str()); 
		const int nevents = file_input_tree->GetEntries();
		file_outstat << " file: " << datafilename << " " << nevents << " events " << "\n";
		cout << " file: " << datafilename << " " << nevents << " events " << endl;
	}
	file_outstat << "============================================================" << "\n";
	cout << "============================================================" << endl;
	
	file_input_tree->SetAlias("rrp","(xp*xp + yp*yp)/10./10."); 
	file_input_tree->SetAlias("rp","TMath::Sqrt((xp*xp + yp*yp))"); 	
	
	// generate plots  
	sprintf(file_outname,"%s/%s_calsource.root", workingdirectory.c_str(), rawdatafilename.c_str());
	TFile *file_outplot = new TFile(file_outname,"RECREATE");
	
	file_outstat << "= geometry parameters ======================================" << "\n";
	file_outstat << "binning: " << bin_z << " " << bin_r << " " << bin_rr << "\n";
	file_outstat << "chamber_minZ: " << TPC.Get_chamber_minZ() << "\n";
	file_outstat << "chamber_maxZ: " << TPC.Get_chamber_maxZ() << "\n";
	file_outstat << "chamber_minRR: " << TPC.Get_chamber_minRR() << "\n";
	file_outstat << "chamber_maxRR: " << TPC.Get_chamber_maxRR() << "\n";
	file_outstat << "LXe_minZ: " << TPC.Get_LXe_minZ() << "\n";
	file_outstat << "LXe_maxZ: " << TPC.Get_LXe_maxZ() << "\n";
	file_outstat << "LXe_minRR: " << TPC.Get_LXe_minRR() << "\n";
	file_outstat << "LXe_maxRR: " << TPC.Get_LXe_maxRR() << "\n";
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
	style_3D->SetTitleOffset(2.,"Z");
	
	style_3D->SetPalette(NCont,ColPalette);
	style_3D->SetNumberContours(99);
	style_3D->cd();
	
	gStyle->SetPalette(NCont,ColPalette);
	
	file_outstat << "= analyse ttree ============================================" << "\n";
	file_outstat << "events: " << file_input_tree->GetEntries() << "\n";
	sprintf(draw_selection,"zp/10<=%f && zp/10>=%f && rrp>=%f && rrp<=%f",TPC.Get_LXe_maxZ(),TPC.Get_LXe_minZ(),TPC.Get_LXe_minRR(),TPC.Get_LXe_maxRR());
	file_input_tree->Draw(">>elist_LXe_gen",draw_selection,"goff");
	TEntryList *elist_LXe_gen = (TEntryList*)gDirectory->Get("elist_LXe_gen");
	file_outstat << "events (LXe): " << elist_LXe_gen->GetEntriesToProcess() << "\n";
	sprintf(draw_selection,"zp/10<=%f && zp/10>=%f && rrp>=%f && rrp<=%f && (nbpmthits > 0 || ntpmthits > 0)",TPC.Get_LXe_maxZ(),TPC.Get_LXe_minZ(),TPC.Get_LXe_minRR(),TPC.Get_LXe_maxRR());
	file_input_tree->Draw(">>elist_LXe_det",draw_selection,"goff");
	TEntryList *elist_LXe_det = (TEntryList*)gDirectory->Get("elist_LXe_det");
	file_outstat << "detected pmthits (LXe): " << elist_LXe_det->GetEntriesToProcess() << "\n";
	file_outstat << "------------------------------------------------------------" << "\n";
	sprintf(draw_selection,"zp/10<=%f && zp/10>=%f && rrp>=%f && rrp<=%f && (ntpmthits > 0)",TPC.Get_LXe_maxZ(),TPC.Get_LXe_minZ(),TPC.Get_LXe_minRR(),TPC.Get_LXe_maxRR());
	file_input_tree->Draw(">>elist_LXe_det_top",draw_selection,"goff");
	TEntryList *elist_LXe_det_top = (TEntryList*)gDirectory->Get("elist_LXe_det_top");
	file_outstat << "detected pmthits (LXe TOP): " << elist_LXe_det_top->GetEntriesToProcess() << "\n";
	file_outstat << "------------------------------------------------------------" << "\n";
	sprintf(draw_selection,"zp/10<=%f && zp/10>=%f && rrp>=%f && rrp<=%f && (nbpmthits > 0)",TPC.Get_LXe_maxZ(),TPC.Get_LXe_minZ(),TPC.Get_LXe_minRR(),TPC.Get_LXe_maxRR());
	file_input_tree->Draw(">>elist_LXe_det_bottom",draw_selection,"goff");
	TEntryList *elist_LXe_det_bottom = (TEntryList*)gDirectory->Get("elist_LXe_det_bottom");
	file_outstat << "detected pmthits (LXe BOTTOM): " << elist_LXe_det_bottom->GetEntriesToProcess() << "\n";
	file_outstat << "============================================================" << "\n";
	
	TLine* lin_cath = new TLine(-TPC.Get_LXe_maxR(),TPC.Get_LXe_maxZ(),TPC.Get_LXe_maxR(),TPC.Get_LXe_maxZ());
	lin_cath->SetLineStyle(9);
	lin_cath->SetLineColor(1);
	lin_cath->SetLineWidth(4);
	
	TLine* lin_grnd = new TLine(-TPC.Get_LXe_maxR(),TPC.Get_LXe_minZ(),TPC.Get_LXe_maxR(),TPC.Get_LXe_minZ());
	lin_grnd->SetLineStyle(9);
	lin_grnd->SetLineColor(1);
	lin_grnd->SetLineWidth(4);
	
	/*=================================================================*/
	/*=================================================================*/
	/*=================================================================*/
	file_outstat << "= plot spectrum ============================================" << "\n";
	/*=================================================================*/
	/*=================================================================*/
	/*=================================================================*/
	
	/*=================================================================*/
	gROOT->SetBatch(kTRUE);
	/*=================================================================*/
	/*=================================================================*/
	// E spectrum
	/*=================================================================*/
	style_1D->cd();
	gStyle->SetPalette(NCont,ColPalette);
	TCanvas *c_spectrum = new TCanvas("cspectrum","cspectrum",canvas_x,canvas_x);
	TH1F* h_spectrum = new TH1F("spectrum", "energy spectrum",250.,0.,1000.);
	file_input_tree->Draw("etot >> spectrum", 0, "goff");
	gPad->SetLogy();
	h_spectrum->SetTitle("energy spectrum");
	h_spectrum->GetYaxis()->SetTitle("counts");
	h_spectrum->GetXaxis()->SetTitle("E [keV]");
	h_spectrum->GetXaxis()->CenterTitle();
	h_spectrum->GetYaxis()->CenterTitle();
	h_spectrum->Draw();
	if (file_outplot) c_spectrum->Write();
	sprintf(canvasfile,"%s/%s_calsource_spectrum.%s", workingdirectory.c_str(), rawdatafilename.c_str(), export_format.c_str());
	if (!(export_format=="")) c_spectrum->SaveAs(canvasfile);
	
	/*=================================================================*/
	/*=================================================================*/
	/*=================================================================*/
	file_outstat << "= plot xy ==================================================" << "\n";
	/*=================================================================*/
	/*=================================================================*/
	/*=================================================================*/
	
	/*=================================================================*/
	gROOT->SetBatch(kTRUE);
	/*=================================================================*/
	/*=================================================================*/
	// X vs. Y of events chamber
	/*=================================================================*/
	style_2D->cd();
	TCanvas *c_xy_chamber = new TCanvas("xy_chamber","xy_chamber",canvas_x,canvas_x);
	TH2F* h_xy_chamber = new TH2F("xy_chamber", "X vs. Y events", TPC.Get_nbinsR(), -TPC.Get_chamber_maxR(), TPC.Get_chamber_maxR(), TPC.Get_nbinsR(), -TPC.Get_chamber_maxR(), TPC.Get_chamber_maxR());
	h_xy_chamber->SetXTitle("X [cm]");
	h_xy_chamber->GetXaxis()->CenterTitle();
	h_xy_chamber->SetYTitle("Y [cm]");
	h_xy_chamber->GetYaxis()->CenterTitle();
	h_xy_chamber->SetZTitle("events [%]");
	h_xy_chamber->GetZaxis()->CenterTitle();
	file_input_tree->Draw("xp/10. : yp/10. >> xy_chamber", 0, "goff");
	h_xy_chamber->Scale(1./h_xy_chamber->GetEntries());
	h_xy_chamber->Draw("colz");
	if (file_outplot) c_xy_chamber->Write();

	/*=================================================================*/
	if (!batch) {gROOT->SetBatch(kFALSE);}
	/*=================================================================*/
	/*=================================================================*/
	// X vs. Y of events LXe
	/*=================================================================*/
	style_2D->cd();
	TCanvas *c_xy = new TCanvas("xy","xy",canvas_x,canvas_x);
	TH2F* h_xy = new TH2F("xy", "X vs. Y events", TPC.Get_nbinsR(), -TPC.Get_LXe_maxR(), TPC.Get_LXe_maxR(), TPC.Get_nbinsR(), -TPC.Get_LXe_maxR(), TPC.Get_LXe_maxR());
	h_xy->SetXTitle("X [cm]");
	h_xy->GetXaxis()->CenterTitle();
	h_xy->SetYTitle("Y [cm]");
	h_xy->GetYaxis()->CenterTitle();
	h_xy->SetZTitle("events [%]");
	h_xy->GetZaxis()->CenterTitle();
	sprintf(draw_selection,"zp/10<=%f && zp/10>=%f && rrp>=%f && rrp<=%f",TPC.Get_LXe_maxZ(),TPC.Get_LXe_minZ(),TPC.Get_LXe_minRR(),TPC.Get_LXe_maxRR());
	file_input_tree->Draw("xp/10. : yp/10. >> xy", draw_selection, "goff");
	h_xy->Scale(1./h_xy->GetEntries());
	h_xy->Draw("colz");
	if (file_outplot) c_xy->Write();
	sprintf(canvasfile,"%s/%s_calsource_xy.%s", workingdirectory.c_str(), rawdatafilename.c_str(), export_format.c_str());
	if (!(export_format=="")) c_xy->SaveAs(canvasfile);
	
	/*=================================================================*/
	gROOT->SetBatch(kTRUE);
	/*=================================================================*/
	if (elist_LXe_det->GetEntriesToProcess() > 0) {
		/*=================================================================*/
		// X vs. Y detected events (TOP + BOTTOM PMTs)
		/*=================================================================*/
		style_2D->cd();
		TCanvas *c_xy_det = new TCanvas("xy_det","xy_det",canvas_x,canvas_x);
		TH2F* h_xy_det = new TH2F("xy_det", "X vs. Y detected events (ALL PMTs)", TPC.Get_nbinsR(), -TPC.Get_LXe_maxR(), TPC.Get_LXe_maxR(), TPC.Get_nbinsR(), -TPC.Get_LXe_maxR(), TPC.Get_LXe_maxR());
		h_xy_det->SetXTitle("X [cm]");
		h_xy_det->GetXaxis()->CenterTitle();
		h_xy_det->SetYTitle("Y [cm]");
		h_xy_det->GetYaxis()->CenterTitle();
		h_xy_det->SetZTitle("detected [%]");
		h_xy_det->GetZaxis()->CenterTitle();
		sprintf(draw_selection,"zp/10<=%f && zp/10>=%f && rrp>=%f && rrp<=%f && (nbpmthits > 0 || ntpmthits > 0)",TPC.Get_LXe_maxZ(),TPC.Get_LXe_minZ(),TPC.Get_LXe_minRR(),TPC.Get_LXe_maxRR());
		file_input_tree->Draw("xp/10. : yp/10. >> xy_det", draw_selection, "goff");
		h_xy_det->Scale(1./h_xy_det->GetEntries());
		h_xy_det->Draw("colz");
		if (file_outplot) c_xy_det->Write();
		
		/*=================================================================*/
		// X vs. Y detected events (TOP PMTs)
		/*=================================================================*/
		style_2D->cd();
		TCanvas *c_xy_det_top = new TCanvas("xy_det_top","xy_det_top",canvas_x,canvas_x);
		TH2F* h_xy_det_top = new TH2F("xy_det_top", "X vs. Y detected events (TOP PMTs)", TPC.Get_nbinsR(), -TPC.Get_LXe_maxR(), TPC.Get_LXe_maxR(), TPC.Get_nbinsR(), -TPC.Get_LXe_maxR(), TPC.Get_LXe_maxR());
		h_xy_det_top->SetXTitle("X [cm]");
		h_xy_det_top->GetXaxis()->CenterTitle();
		h_xy_det_top->SetYTitle("Y [cm]");
		h_xy_det_top->GetYaxis()->CenterTitle();
		h_xy_det_top->SetZTitle("TOP detected [%]");
		h_xy_det_top->GetZaxis()->CenterTitle();
		sprintf(draw_selection,"zp/10<=%f && zp/10>=%f && rrp>=%f && rrp<=%f && (ntpmthits > 0)",TPC.Get_LXe_maxZ(),TPC.Get_LXe_minZ(),TPC.Get_LXe_minRR(),TPC.Get_LXe_maxRR());
		file_input_tree->Draw("xp/10. : yp/10. >> xy_det_top", draw_selection, "goff");
		h_xy_det_top->Scale(1./h_xy_det_top->GetEntries());
		h_xy_det_top->Draw("colz");
		if (file_outplot) c_xy_det_top->Write();
		
		/*=================================================================*/
		// X vs. Y detected events (BOTTOM PMTs)
		/*=================================================================*/
		style_2D->cd();
		TCanvas *c_xy_det_bottom = new TCanvas("xy_det_bottom","xy_det_bottom",canvas_x,canvas_x);
		TH2F* h_xy_det_bottom = new TH2F("xy_det_bottom", "X vs. Y detected events (BOTTOM PMTs)", TPC.Get_nbinsR(), -TPC.Get_LXe_maxR(), TPC.Get_LXe_maxR(), TPC.Get_nbinsR(), -TPC.Get_LXe_maxR(), TPC.Get_LXe_maxR());
		h_xy_det_bottom->SetXTitle("X [cm]");
		h_xy_det_bottom->GetXaxis()->CenterTitle();
		h_xy_det_bottom->SetYTitle("Y [cm]");
		h_xy_det_bottom->GetYaxis()->CenterTitle();
		h_xy_det_bottom->SetZTitle("BOTTOM detected [%]");
		h_xy_det_bottom->GetZaxis()->CenterTitle();
		sprintf(draw_selection,"zp/10<=%f && zp/10>=%f && rrp>=%f && rrp<=%f && (nbpmthits > 0)",TPC.Get_LXe_maxZ(),TPC.Get_LXe_minZ(),TPC.Get_LXe_minRR(),TPC.Get_LXe_maxRR());
		file_input_tree->Draw("xp/10. : yp/10. >> xy_det_bottom", draw_selection, "goff");
		h_xy_det_bottom->Scale(1./h_xy_det_bottom->GetEntries());
		h_xy_det_bottom->Draw("colz");
		if (file_outplot) c_xy_det_bottom->Write();
	}	
	/*=================================================================*/
	/*=================================================================*/
	/*=================================================================*/
	file_outstat << "= plot rrZ =================================================" << "\n";
	/*=================================================================*/
	/*=================================================================*/
	/*=================================================================*/
	
	/*=================================================================*/
	gROOT->SetBatch(kTRUE);
	/*=================================================================*/
	/*=================================================================*/
	// R vs. Z of events chamber
	/*=================================================================*/
	style_2D->cd();
	TCanvas *c_rZ_camber = new TCanvas("rZ_chamber","rZ_chamber",canvas_x,canvas_y);
	TH2F* h_rZ_chamber = new TH2F("rZ_chamber", "R vs. Z events", TPC.Get_nbinsR(), -TPC.Get_chamber_maxR(), TPC.Get_chamber_maxR(), TPC.Get_nbinsZ(), TPC.Get_chamber_minZ(), TPC.Get_chamber_maxZ());
	h_rZ_chamber->SetXTitle("Y [cm]");
	h_rZ_chamber->GetXaxis()->CenterTitle();
	h_rZ_chamber->SetYTitle("Z [cm]");
	h_rZ_chamber->GetYaxis()->CenterTitle();
	h_rZ_chamber->SetZTitle("events [%]");
	h_rZ_chamber->GetZaxis()->CenterTitle();
	file_input_tree->Draw("zp/10. : yp/10. >> rZ_chamber", 0, "goff");
	h_rZ_chamber->Scale(1./h_rZ_chamber->GetEntries());
	h_rZ_chamber->Draw("colz");
	lin_cath->Draw("same");
	lin_grnd->Draw("same");
	if (file_outplot) c_rZ_camber->Write();
	
	/*=================================================================*/
	if (!batch) {gROOT->SetBatch(kFALSE);}
	/*=================================================================*/
	/*=================================================================*/
	// R vs. Z of events LXe
	/*=================================================================*/
	style_2D->cd();
	TCanvas *c_rZ = new TCanvas("rZ","rZ",canvas_x,canvas_y);
	TH2F* h_rZ = new TH2F("rZ", "R vs. Z events", TPC.Get_nbinsR(), -TPC.Get_LXe_maxR(), TPC.Get_LXe_maxR(), TPC.Get_nbinsZ(), TPC.Get_LXe_minZ(), TPC.Get_LXe_maxZ());
	h_rZ->SetXTitle("Y [cm]");
	h_rZ->GetXaxis()->CenterTitle();
	h_rZ->SetYTitle("Z [cm]");
	h_rZ->GetYaxis()->CenterTitle();
	h_rZ->SetZTitle("events [%]");
	h_rZ->GetZaxis()->CenterTitle();
	sprintf(draw_selection,"zp/10<=%f && zp/10>=%f && rrp>=%f && rrp<=%f",TPC.Get_LXe_maxZ(),TPC.Get_LXe_minZ(),TPC.Get_LXe_minRR(),TPC.Get_LXe_maxRR());
	file_input_tree->Draw("zp/10. : yp/10. >> rZ", draw_selection, "goff");
	h_rZ->Scale(1./h_rZ->GetEntries());
	h_rZ->Draw("colz");
	lin_cath->Draw("same");
	lin_grnd->Draw("same");
	if (file_outplot) c_rZ->Write();
	sprintf(canvasfile,"%s/%s_calsource_yz.%s", workingdirectory.c_str(), rawdatafilename.c_str(), export_format.c_str());
	if (!(export_format=="")) c_rZ->SaveAs(canvasfile);
	
	/*=================================================================*/
	gROOT->SetBatch(kTRUE);
	/*=================================================================*/
	if (elist_LXe_det->GetEntriesToProcess() > 0) {
		/*=================================================================*/
		// R vs. Z detected events (TOP + BOTTOM PMTs)
		/*=================================================================*/
		style_2D->cd();
		TCanvas *c_rZ_det = new TCanvas("rZ_det","rZ_det",canvas_x,canvas_y);
		TH2F* h_rZ_det = new TH2F("rZ_det", "R vs. Z detected events (ALL PMTs)", TPC.Get_nbinsR(), -TPC.Get_LXe_maxR(), TPC.Get_LXe_maxR(), TPC.Get_nbinsZ(), TPC.Get_LXe_minZ(), TPC.Get_LXe_maxZ());
		h_rZ_det->SetXTitle("Y [cm]");
		h_rZ_det->GetXaxis()->CenterTitle();
		h_rZ_det->SetYTitle("Z [cm]");
		h_rZ_det->GetYaxis()->CenterTitle();
		h_rZ_det->SetZTitle("detected [%]");
		h_rZ_det->GetZaxis()->CenterTitle();
		sprintf(draw_selection,"zp/10<=%f && zp/10>=%f && rrp>=%f && rrp<=%f && (nbpmthits > 0 || ntpmthits > 0)",TPC.Get_LXe_maxZ(),TPC.Get_LXe_minZ(),TPC.Get_LXe_minRR(),TPC.Get_LXe_maxRR());
		file_input_tree->Draw("zp/10. : yp/10. >> rZ_det", draw_selection, "goff");
		h_rZ_det->Scale(1./h_rZ_det->GetEntries());
		h_rZ_det->Draw("colz");
		lin_cath->Draw("same");
		lin_grnd->Draw("same");
		if (file_outplot) c_rZ_det->Write();
		sprintf(canvasfile,"%s/%s_calsource_yz_det.%s", workingdirectory.c_str(), rawdatafilename.c_str(), export_format.c_str());
		if (!(export_format=="")) c_rZ_det->SaveAs(canvasfile);
		
		/*=================================================================*/
		// R vs. Z detected events (TOP PMTs)
		/*=================================================================*/
		style_2D->cd();
		TCanvas *c_rZ_det_top = new TCanvas("rZ_det_top","rZ_det_top",canvas_x,canvas_y);
		TH2F* h_rZ_det_top = new TH2F("rZ_det_top", "R vs. Z detected events (TOP PMTs)", TPC.Get_nbinsR(), -TPC.Get_LXe_maxR(), TPC.Get_LXe_maxR(), TPC.Get_nbinsZ(), TPC.Get_LXe_minZ(), TPC.Get_LXe_maxZ());
		h_rZ_det_top->SetXTitle("Y [cm]");
		h_rZ_det_top->GetXaxis()->CenterTitle();
		h_rZ_det_top->SetYTitle("Z [cm]");
		h_rZ_det_top->GetYaxis()->CenterTitle();
		h_rZ_det_top->SetZTitle("TOP detected [%]");
		h_rZ_det_top->GetZaxis()->CenterTitle();
		sprintf(draw_selection,"zp/10<=%f && zp/10>=%f && rrp>=%f && rrp<=%f && (ntpmthits > 0)",TPC.Get_LXe_maxZ(),TPC.Get_LXe_minZ(),TPC.Get_LXe_minRR(),TPC.Get_LXe_maxRR());
		file_input_tree->Draw("zp/10. : yp/10. >> rZ_det_top", draw_selection, "goff");
		h_rZ_det_top->Scale(1./h_rZ_det_top->GetEntries());
		h_rZ_det_top->Draw("colz");
		lin_cath->Draw("same");
		lin_grnd->Draw("same");
		if (file_outplot) c_rZ_det_top->Write();
		sprintf(canvasfile,"%s/%s_calsource_yz_det_top.%s", workingdirectory.c_str(), rawdatafilename.c_str(), export_format.c_str());
		if (!(export_format=="")) c_rZ_det_top->SaveAs(canvasfile);
		
		/*=================================================================*/
		// R vs. Z detected events (BOTTOM PMTs)
		/*=================================================================*/
		style_2D->cd();
		TCanvas *c_rZ_det_bottom = new TCanvas("rZ_det_bottom","rZ_det_bottom",canvas_x,canvas_y);
		TH2F* h_rZ_det_bottom = new TH2F("rZ_det_bottom", "R vs. Z detected events (BOTTOM PMTs)", TPC.Get_nbinsR(), -TPC.Get_LXe_maxR(), TPC.Get_LXe_maxR(), TPC.Get_nbinsZ(), TPC.Get_LXe_minZ(), TPC.Get_LXe_maxZ());
		h_rZ_det_bottom->SetXTitle("Y [cm]");
		h_rZ_det_bottom->GetXaxis()->CenterTitle();
		h_rZ_det_bottom->SetYTitle("Z [cm]");
		h_rZ_det_bottom->GetYaxis()->CenterTitle();
		h_rZ_det_bottom->SetZTitle("BOTTOM detected [%]");
		h_rZ_det_bottom->GetZaxis()->CenterTitle();
		sprintf(draw_selection,"zp/10<=%f && zp/10>=%f && rrp>=%f && rrp<=%f && (nbpmthits > 0)",TPC.Get_LXe_maxZ(),TPC.Get_LXe_minZ(),TPC.Get_LXe_minRR(),TPC.Get_LXe_maxRR());
		file_input_tree->Draw("zp/10. : yp/10. >> rZ_det_bottom", draw_selection, "goff");
		h_rZ_det_bottom->Scale(1./h_rZ_det_bottom->GetEntries());
		h_rZ_det_bottom->Draw("colz");
		lin_cath->Draw("same");
		lin_grnd->Draw("same");
		if (file_outplot) c_rZ_det_bottom->Write();
		sprintf(canvasfile,"%s/%s_calsource_yz_det_bottom.%s", workingdirectory.c_str(), rawdatafilename.c_str(), export_format.c_str());
		if (!(export_format=="")) c_rZ_det_bottom->SaveAs(canvasfile);
	}	
	/*=================================================================*/
	if (!batch) {gROOT->SetBatch(kFALSE);}
	/*=================================================================*/
	file_outstat << "============================================================" << "\n";
	file_outstat.close();	
	//file_outplot->Close(); 
}

/*=================================================================*/
