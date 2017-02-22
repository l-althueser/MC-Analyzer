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

void optPhot_S1(string, int, int, int, string);
void optPhot_S1(string, int, int, int, string, bool);

/*=================================================================*/
void optPhot_S1(string datafile, string export_format) {
	// Some good binnings
	//TPC.Set_Bins(26,50,22) - default
	//TPC.Set_Bins(52,100,44)- nevents > 10000000
	optPhot_S1(datafile,26,50,22,export_format,true);
}

void optPhot_S1(string datafile, int bin_z, int bin_r, int bin_rr, string export_format) {
	optPhot_S1(datafile,bin_z,bin_r,bin_rr,export_format,true);
}

void optPhot_S1(string datafile, int bin_z, int bin_r, int bin_rr, string export_format, bool batch) {
	
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
	
	Int_t canvas_x = 850;
	Int_t canvas_y = 800;

	char canvasfile[10000];
	char draw_selection[10000];
	
	TPC_Definition TPC(bin_z, bin_r, bin_rr);
	TChain *file_input_tree = new TChain("events/events");
	TNamed *G4MCname;
	
	char file_outname[10000];
	sprintf(file_outname,"%s/%s_S1.dat", workingdirectory.c_str(), rawdatafilename.c_str());
	
	ofstream file_outstat;
	file_outstat.open(file_outname);
	file_outstat << "============================================================" << "\n";
	sprintf(file_outname,"%s_S1.root", rawdatafilename.c_str());
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
	sprintf(file_outname,"%s/%s_S1.root", workingdirectory.c_str(), rawdatafilename.c_str());
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
	style_1D->SetPadRightMargin(0.085);
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
	style_2D->SetPadRightMargin(0.145);
	style_2D->SetPadTopMargin(0.08);
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
	file_outstat << "generated events: " << file_input_tree->GetEntries() << "\n";
	
	sprintf(draw_selection,"zp_pri/10<=%f && zp_pri/10>=%f && rrp_pri>=%f && rrp_pri<=%f",TPC.Get_LXe_maxZ(),TPC.Get_LXe_minZ(),TPC.Get_LXe_minRR(),TPC.Get_LXe_maxRR());
	file_input_tree->Draw(">>elist_LXe_gen",draw_selection,"goff");
	TEntryList *elist_LXe_gen = (TEntryList*)gDirectory->Get("elist_LXe_gen");
	file_outstat << "generated events (LXe): " << elist_LXe_gen->GetEntriesToProcess() << "\n";
	sprintf(draw_selection,"zp_pri/10<=%f && zp_pri/10>=%f && rrp_pri>=%f && rrp_pri<=%f && (nbpmthits > 0 || ntpmthits > 0)",TPC.Get_LXe_maxZ(),TPC.Get_LXe_minZ(),TPC.Get_LXe_minRR(),TPC.Get_LXe_maxRR());
	file_input_tree->Draw(">>elist_LXe_det",draw_selection,"goff");
	TEntryList *elist_LXe_det = (TEntryList*)gDirectory->Get("elist_LXe_det");
	file_outstat << "detected events (LXe): " << elist_LXe_det->GetEntriesToProcess() << "\n";
	file_outstat << "Mean LCE LXe(detected/generated): " << (double)elist_LXe_det->GetEntriesToProcess()/(double)elist_LXe_gen->GetEntriesToProcess()*100. << "\n";
	file_outstat << "------------------------------------------------------------" << "\n";
	sprintf(draw_selection,"zp_pri/10<=%f && zp_pri/10>=%f && rrp_pri>=%f && rrp_pri<=%f && (ntpmthits > 0)",TPC.Get_LXe_maxZ(),TPC.Get_LXe_minZ(),TPC.Get_LXe_minRR(),TPC.Get_LXe_maxRR());
	file_input_tree->Draw(">>elist_LXe_det_top",draw_selection,"goff");
	TEntryList *elist_LXe_det_top = (TEntryList*)gDirectory->Get("elist_LXe_det_top");
	file_outstat << "detected events (LXe TOP): " << elist_LXe_det_top->GetEntriesToProcess() << "\n";
	file_outstat << "Mean LCE LXe(detected TOP/generated): " << (double)elist_LXe_det_top->GetEntriesToProcess()/(double)elist_LXe_gen->GetEntriesToProcess()*100. << "\n";
	file_outstat << "AreaFractionTop LXe(detected TOP/detected ALL): " << (double)elist_LXe_det_top->GetEntriesToProcess()/(double)elist_LXe_det->GetEntriesToProcess()*100. << "\n";
	file_outstat << "------------------------------------------------------------" << "\n";
	sprintf(draw_selection,"zp_pri/10<=%f && zp_pri/10>=%f && rrp_pri>=%f && rrp_pri<=%f && (nbpmthits > 0)",TPC.Get_LXe_maxZ(),TPC.Get_LXe_minZ(),TPC.Get_LXe_minRR(),TPC.Get_LXe_maxRR());
	file_input_tree->Draw(">>elist_LXe_det_bottom",draw_selection,"goff");
	TEntryList *elist_LXe_det_bottom = (TEntryList*)gDirectory->Get("elist_LXe_det_bottom");
	file_outstat << "detected events (LXe BOTTOM): " << elist_LXe_det_bottom->GetEntriesToProcess() << "\n";
	file_outstat << "Mean LCE LXe(detected BOTTOM/generated): " << (double)elist_LXe_det_bottom->GetEntriesToProcess()/(double)elist_LXe_gen->GetEntriesToProcess()*100. << "\n";
	file_outstat << "AreaFractionBottom LXe(detected BOTTOM/detected ALL): " << (double)elist_LXe_det_bottom->GetEntriesToProcess()/(double)elist_LXe_det->GetEntriesToProcess()*100. << "\n";
	file_outstat << "============================================================" << "\n";
	
	TLine* lin_cath = new TLine(0.,TPC.Get_LXe_maxZ(),TPC.Get_LXe_maxRR(),TPC.Get_LXe_maxZ());
	lin_cath->SetLineStyle(9);
	lin_cath->SetLineColor(1);
	lin_cath->SetLineWidth(4);
	
	TLine* lin_grnd = new TLine(0.,TPC.Get_LXe_minZ(),TPC.Get_LXe_maxRR(),TPC.Get_LXe_minZ());
	lin_grnd->SetLineStyle(9);
	lin_grnd->SetLineColor(1);
	lin_grnd->SetLineWidth(4);
	
	TLine* lin_cath_Z = new TLine(TPC.Get_LXe_maxZ(),TPC.Get_LCE_min(),TPC.Get_LXe_maxZ(),TPC.Get_LCE_max());
	lin_cath_Z->SetLineStyle(9);
	lin_cath_Z->SetLineColor(1);
	lin_cath_Z->SetLineWidth(4);
	
	TLine* lin_grnd_Z = new TLine(TPC.Get_LXe_minZ(),TPC.Get_LCE_min(),TPC.Get_LXe_minZ(),TPC.Get_LCE_max());
	lin_grnd_Z->SetLineStyle(9);
	lin_grnd_Z->SetLineColor(1);
	lin_grnd_Z->SetLineWidth(4);
	
	TLine* lin_cath_rr = new TLine(TPC.Get_LXe_minRR(),TPC.Get_LCE_min(),TPC.Get_LXe_minRR(),TPC.Get_LCE_max());
	lin_cath_rr->SetLineStyle(9);
	lin_cath_rr->SetLineColor(1);
	lin_cath_rr->SetLineWidth(4);
	
	TLine* lin_grnd_rr = new TLine(TPC.Get_LXe_maxRR(),TPC.Get_LCE_min(),TPC.Get_LXe_maxRR(),TPC.Get_LCE_max());
	lin_grnd_rr->SetLineStyle(9);
	lin_grnd_rr->SetLineColor(1);
	lin_grnd_rr->SetLineWidth(4);
	
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
	// X vs. Y of generated events chamber
	/*=================================================================*/
	style_2D->cd();
	gStyle->SetPalette(NCont,ColPalette);
	TCanvas *c_xy_chamber = new TCanvas("xy_chamber","xy_chamber",canvas_x,canvas_x);
	TH2F* h_xy_chamber = new TH2F("xy_chamber_pri", "X vs. Y generated events", TPC.Get_nbinsR(), -TPC.Get_chamber_maxR(), TPC.Get_chamber_maxR(), TPC.Get_nbinsR(), -TPC.Get_chamber_maxR(), TPC.Get_chamber_maxR());
	h_xy_chamber->SetXTitle("X [cm]");
	h_xy_chamber->GetXaxis()->CenterTitle();
	h_xy_chamber->SetYTitle("Y [cm]");
	h_xy_chamber->GetYaxis()->CenterTitle();
	h_xy_chamber->SetZTitle("events [#]");
	h_xy_chamber->GetZaxis()->CenterTitle();
	file_input_tree->Draw("xp_pri/10. : yp_pri/10. >> xy_chamber_pri", 0, "goff");
	h_xy_chamber->Draw("colz");
	if (file_outplot) c_xy_chamber->Write();

	/*=================================================================*/
	// X vs. Y of generated events LXe
	/*=================================================================*/
	style_2D->cd();
	TCanvas *c_xy = new TCanvas("xy","xy",canvas_x,canvas_x);
	TH2F* h_xy = new TH2F("xy_pri", "X vs. Y generated events", TPC.Get_nbinsR(), -TPC.Get_LXe_maxR(), TPC.Get_LXe_maxR(), TPC.Get_nbinsR(), -TPC.Get_LXe_maxR(), TPC.Get_LXe_maxR());
	h_xy->SetXTitle("X [cm]");
	h_xy->GetXaxis()->CenterTitle();
	h_xy->SetYTitle("Y [cm]");
	h_xy->GetYaxis()->CenterTitle();
	h_xy->SetZTitle("events [#]");
	h_xy->GetZaxis()->CenterTitle();
	sprintf(draw_selection,"zp_pri/10<=%f && zp_pri/10>=%f && rrp_pri>=%f && rrp_pri<=%f",TPC.Get_LXe_maxZ(),TPC.Get_LXe_minZ(),TPC.Get_LXe_minRR(),TPC.Get_LXe_maxRR());
	file_input_tree->Draw("xp_pri/10. : yp_pri/10. >> xy_pri", draw_selection, "goff");
	h_xy->Draw("colz");
	if (file_outplot) c_xy->Write();
	
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
	h_xy_det->SetZTitle("detected [#]");
	h_xy_det->GetZaxis()->CenterTitle();
	sprintf(draw_selection,"zp_pri/10<=%f && zp_pri/10>=%f && rrp_pri>=%f && rrp_pri<=%f && (nbpmthits > 0 || ntpmthits > 0)",TPC.Get_LXe_maxZ(),TPC.Get_LXe_minZ(),TPC.Get_LXe_minRR(),TPC.Get_LXe_maxRR());
	file_input_tree->Draw("xp_pri/10. : yp_pri/10. >> xy_det", draw_selection, "goff");
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
	h_xy_det_top->SetZTitle("TOP detected [#]");
	h_xy_det_top->GetZaxis()->CenterTitle();
	sprintf(draw_selection,"zp_pri/10<=%f && zp_pri/10>=%f && rrp_pri>=%f && rrp_pri<=%f && (ntpmthits > 0)",TPC.Get_LXe_maxZ(),TPC.Get_LXe_minZ(),TPC.Get_LXe_minRR(),TPC.Get_LXe_maxRR());
	file_input_tree->Draw("xp_pri/10. : yp_pri/10. >> xy_det_top", draw_selection, "goff");
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
	h_xy_det_bottom->SetZTitle("BOTTOM detected [#]");
	h_xy_det_bottom->GetZaxis()->CenterTitle();
	sprintf(draw_selection,"zp_pri/10<=%f && zp_pri/10>=%f && rrp_pri>=%f && rrp_pri<=%f && (nbpmthits > 0)",TPC.Get_LXe_maxZ(),TPC.Get_LXe_minZ(),TPC.Get_LXe_minRR(),TPC.Get_LXe_maxRR());
	file_input_tree->Draw("xp_pri/10. : yp_pri/10. >> xy_det_bottom", draw_selection, "goff");
	h_xy_det_bottom->Draw("colz");
	if (file_outplot) c_xy_det_bottom->Write();
	
	/*=================================================================*/
	if (!batch) {gROOT->SetBatch(kFALSE);}
	/*=================================================================*/
	/*=================================================================*/
	// LCE of X vs. Y
	/*=================================================================*/	
	style_2D->cd();
	TCanvas *c_LCE_xy = new TCanvas("LCE_xy","LCE_xy",canvas_x,canvas_x);
	TH2F* h_LCE_xy = new TH2F("LCE_xy", "LCE of X vs. Y (ALL PMTs)", TPC.Get_nbinsR(), -TPC.Get_LXe_maxR(), TPC.Get_LXe_maxR(), TPC.Get_nbinsR(), -TPC.Get_LXe_maxR(), TPC.Get_LXe_maxR());
	h_LCE_xy->SetXTitle("X [cm]");
	h_LCE_xy->GetXaxis()->CenterTitle();
	h_LCE_xy->SetYTitle("Y [cm]");
	h_LCE_xy->GetYaxis()->CenterTitle();
	h_LCE_xy->SetZTitle("LCE [%]");
	h_LCE_xy->GetZaxis()->CenterTitle();
	h_LCE_xy->Sumw2();
	h_LCE_xy->Divide(h_xy_det, h_xy, 1.,1., "b");
	h_LCE_xy->Scale(100.);
	h_LCE_xy->Draw("colz");
	if (file_outplot) c_LCE_xy->Write();
	sprintf(canvasfile,"%s/%s_S1_xy_LCE.%s", workingdirectory.c_str(), rawdatafilename.c_str(), export_format.c_str());
	if (!(export_format=="")) c_LCE_xy->SaveAs(canvasfile);
	
	/*=================================================================*/
	gROOT->SetBatch(kTRUE);
	/*=================================================================*/
	/*=================================================================*/
	// LCE of X vs. Y (TOP PMTs)
	/*=================================================================*/
	style_2D->cd();
	TCanvas *c_LCE_xy_top = new TCanvas("LCE_xy_top","LCE_xy_top",canvas_x,canvas_x);
	TH2F* h_LCE_xy_top = new TH2F("LCE_xy_top", "LCE of X vs. Y (TOP PMTs)", TPC.Get_nbinsR(), -TPC.Get_LXe_maxR(), TPC.Get_LXe_maxR(), TPC.Get_nbinsR(), -TPC.Get_LXe_maxR(), TPC.Get_LXe_maxR());
	h_LCE_xy_top->SetXTitle("X [cm]");
	h_LCE_xy_top->GetXaxis()->CenterTitle();
	h_LCE_xy_top->SetYTitle("Y [cm]");
	h_LCE_xy_top->GetYaxis()->CenterTitle();
	h_LCE_xy_top->SetZTitle("TOP LCE [%]");
	h_LCE_xy_top->GetZaxis()->CenterTitle();
	h_LCE_xy_top->Sumw2();
	h_LCE_xy_top->Divide(h_xy_det_top, h_xy, 1.,1., "b");
	h_LCE_xy_top->Scale(100.);
	h_LCE_xy_top->Draw("colz");
	if (file_outplot) c_LCE_xy_top->Write();
	sprintf(canvasfile,"%s/%s_S1_xy_LCE_top.%s", workingdirectory.c_str(), rawdatafilename.c_str(), export_format.c_str());
	if (!(export_format=="")) c_LCE_xy_top->SaveAs(canvasfile);
	
	/*=================================================================*/
	// LCE of X vs. Y (BOTTOM PMTs)
	/*=================================================================*/
	style_2D->cd();
	TCanvas *c_LCE_xy_bottom = new TCanvas("LCE_xy_bottom","LCE_xy_bottom",canvas_x,canvas_x);
	TH2F* h_LCE_xy_bottom = new TH2F("LCE_xy_bottom", "LCE of X vs. Y (BOTTOM PMTs)", TPC.Get_nbinsR(), -TPC.Get_LXe_maxR(), TPC.Get_LXe_maxR(), TPC.Get_nbinsR(), -TPC.Get_LXe_maxR(), TPC.Get_LXe_maxR());
	h_LCE_xy_bottom->SetXTitle("X [cm]");
	h_LCE_xy_bottom->GetXaxis()->CenterTitle();
	h_LCE_xy_bottom->SetYTitle("Y [cm]");
	h_LCE_xy_bottom->GetYaxis()->CenterTitle();
	h_LCE_xy_bottom->SetZTitle("BOTTOM LCE [%]");
	h_LCE_xy_bottom->GetZaxis()->CenterTitle();
	h_LCE_xy_bottom->Sumw2();
	h_LCE_xy_bottom->Divide(h_xy_det_bottom, h_xy, 1.,1., "b");
	h_LCE_xy_bottom->Scale(100.);
	h_LCE_xy_bottom->Draw("colz");
	if (file_outplot) c_LCE_xy_bottom->Write();
	sprintf(canvasfile,"%s/%s_S1_xy_LCE_bottom.%s", workingdirectory.c_str(), rawdatafilename.c_str(), export_format.c_str());
	if (!(export_format=="")) c_LCE_xy_bottom->SaveAs(canvasfile);
	
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
	// R^{2} vs. Z of generated events chamber
	/*=================================================================*/
	style_2D->cd();
	TCanvas *c_rrZ_camber = new TCanvas("rrZ_chamber","rrZ_chamber",canvas_x,canvas_y);
	TH2F* h_rrZ_chamber = new TH2F("rrZ_chamber_pri", "R^{2} vs. Z generated events", TPC.Get_nbinsRR(), TPC.Get_chamber_minRR(), TPC.Get_chamber_maxRR(), TPC.Get_nbinsZ(), TPC.Get_chamber_minZ(), TPC.Get_chamber_maxZ());
	h_rrZ_chamber->SetXTitle("R^{2} [cm^{2}]");
	h_rrZ_chamber->GetXaxis()->CenterTitle();
	h_rrZ_chamber->SetYTitle("Z [cm]");
	h_rrZ_chamber->GetYaxis()->CenterTitle();
	h_rrZ_chamber->SetZTitle("events [#]");
	h_rrZ_chamber->GetZaxis()->CenterTitle();
	file_input_tree->Draw("zp_pri/10. : rrp_pri >> rrZ_chamber_pri", 0, "goff");
	h_rrZ_chamber->Draw("colz");
	lin_cath->Draw("same");
	lin_grnd->Draw("same");
	if (file_outplot) c_rrZ_camber->Write();
	sprintf(canvasfile,"%s/%s_S1_rrZ_gen.%s", workingdirectory.c_str(), rawdatafilename.c_str(), export_format.c_str());
	if (!(export_format=="")) c_rrZ_camber->SaveAs(canvasfile);
	
	/*=================================================================*/
	// R^{2} vs. Z of generated events LXe
	/*=================================================================*/
	style_2D->cd();
	TCanvas *c_rrZ = new TCanvas("rrZ","rrZ",canvas_x,canvas_y);
	TH2F* h_rrZ = new TH2F("rrZ_pri", "R^{2} vs. Z generated events", TPC.Get_nbinsRR(), TPC.Get_LXe_minRR(), TPC.Get_LXe_maxRR(), TPC.Get_nbinsZ(), TPC.Get_LXe_minZ(), TPC.Get_LXe_maxZ());
	h_rrZ->SetXTitle("R^{2} [cm^{2}]");
	h_rrZ->GetXaxis()->CenterTitle();
	h_rrZ->SetYTitle("Z [cm]");
	h_rrZ->GetYaxis()->CenterTitle();
	h_rrZ->SetZTitle("events [#]");
	h_rrZ->GetZaxis()->CenterTitle();
	sprintf(draw_selection,"zp_pri/10<=%f && zp_pri/10>=%f && rrp_pri>=%f && rrp_pri<=%f",TPC.Get_LXe_maxZ(),TPC.Get_LXe_minZ(),TPC.Get_LXe_minRR(),TPC.Get_LXe_maxRR());
	file_input_tree->Draw("zp_pri/10. : rrp_pri >> rrZ_pri", draw_selection, "goff");
	h_rrZ->Draw("colz");
	lin_cath->Draw("same");
	lin_grnd->Draw("same");
	if (file_outplot) c_rrZ->Write();
	
	/*=================================================================*/
	// R^{2} vs. Z detected events (TOP + BOTTOM PMTs)
	/*=================================================================*/
	style_2D->cd();
	TCanvas *c_rrZ_det = new TCanvas("rrZ_det","rrZ_det",canvas_x,canvas_y);
	TH2F* h_rrZ_det = new TH2F("rrZ_det", "R^{2} vs. Z detected events (ALL PMTs)", TPC.Get_nbinsRR(), TPC.Get_LXe_minRR(), TPC.Get_LXe_maxRR(), TPC.Get_nbinsZ(), TPC.Get_LXe_minZ(), TPC.Get_LXe_maxZ());
	h_rrZ_det->SetXTitle("R^{2} [cm^{2}]");
	h_rrZ_det->GetXaxis()->CenterTitle();
	h_rrZ_det->SetYTitle("Z [cm]");
	h_rrZ_det->GetYaxis()->CenterTitle();
	h_rrZ_det->SetZTitle("detected [#]");
	h_rrZ_det->GetZaxis()->CenterTitle();
	sprintf(draw_selection,"zp_pri/10<=%f && zp_pri/10>=%f && rrp_pri>=%f && rrp_pri<=%f && (nbpmthits > 0 || ntpmthits > 0)",TPC.Get_LXe_maxZ(),TPC.Get_LXe_minZ(),TPC.Get_LXe_minRR(),TPC.Get_LXe_maxRR());
	file_input_tree->Draw("zp_pri/10. : rrp_pri >> rrZ_det", draw_selection, "goff");
	h_rrZ_det->Draw("colz");
	lin_cath->Draw("same");
	lin_grnd->Draw("same");
	if (file_outplot) c_rrZ_det->Write();
	sprintf(canvasfile,"%s/%s_S1_rrZ_det.%s", workingdirectory.c_str(), rawdatafilename.c_str(), export_format.c_str());
	if (!(export_format=="")) c_rrZ_det->SaveAs(canvasfile);
	
	/*=================================================================*/
	// R^{2} vs. Z detected events (TOP PMTs)
	/*=================================================================*/
	style_2D->cd();
	TCanvas *c_rrZ_det_top = new TCanvas("rrZ_det_top","rrZ_det_top",canvas_x,canvas_y);
	TH2F* h_rrZ_det_top = new TH2F("rrZ_det_top", "R^{2} vs. Z detected events (TOP PMTs)", TPC.Get_nbinsRR(), TPC.Get_LXe_minRR(), TPC.Get_LXe_maxRR(), TPC.Get_nbinsZ(), TPC.Get_LXe_minZ(), TPC.Get_LXe_maxZ());
	h_rrZ_det_top->SetXTitle("R^{2} [cm^{2}]");
	h_rrZ_det_top->GetXaxis()->CenterTitle();
	h_rrZ_det_top->SetYTitle("Z [cm]");
	h_rrZ_det_top->GetYaxis()->CenterTitle();
	h_rrZ_det_top->SetZTitle("TOP detected [#]");
	h_rrZ_det_top->GetZaxis()->CenterTitle();
	sprintf(draw_selection,"zp_pri/10<=%f && zp_pri/10>=%f && rrp_pri>=%f && rrp_pri<=%f && (ntpmthits > 0)",TPC.Get_LXe_maxZ(),TPC.Get_LXe_minZ(),TPC.Get_LXe_minRR(),TPC.Get_LXe_maxRR());
	file_input_tree->Draw("zp_pri/10. : rrp_pri >> rrZ_det_top", draw_selection, "goff");
	h_rrZ_det_top->Draw("colz");
	lin_cath->Draw("same");
	lin_grnd->Draw("same");
	if (file_outplot) c_rrZ_det_top->Write();
	sprintf(canvasfile,"%s/%s_S1_rrZ_det_top.%s", workingdirectory.c_str(), rawdatafilename.c_str(), export_format.c_str());
	if (!(export_format=="")) c_rrZ_det_top->SaveAs(canvasfile);
	
	/*=================================================================*/
	// R^{2} vs. Z detected events (BOTTOM PMTs)
	/*=================================================================*/
	style_2D->cd();
	TCanvas *c_rrZ_det_bottom = new TCanvas("rrZ_det_bottom","rrZ_det_bottom",canvas_x,canvas_y);
	TH2F* h_rrZ_det_bottom = new TH2F("rrZ_det_bottom", "R^{2} vs. Z detected events (BOTTOM PMTs)", TPC.Get_nbinsRR(), TPC.Get_LXe_minRR(), TPC.Get_LXe_maxRR(), TPC.Get_nbinsZ(), TPC.Get_LXe_minZ(), TPC.Get_LXe_maxZ());
	h_rrZ_det_bottom->SetXTitle("R^{2} [cm^{2}]");
	h_rrZ_det_bottom->GetXaxis()->CenterTitle();
	h_rrZ_det_bottom->SetYTitle("Z [cm]");
	h_rrZ_det_bottom->GetYaxis()->CenterTitle();
	h_rrZ_det_bottom->SetZTitle("BOTTOM detected [#]");
	h_rrZ_det_bottom->GetZaxis()->CenterTitle();
	sprintf(draw_selection,"zp_pri/10<=%f && zp_pri/10>=%f && rrp_pri>=%f && rrp_pri<=%f && (nbpmthits > 0)",TPC.Get_LXe_maxZ(),TPC.Get_LXe_minZ(),TPC.Get_LXe_minRR(),TPC.Get_LXe_maxRR());
	file_input_tree->Draw("zp_pri/10. : rrp_pri >> rrZ_det_bottom", draw_selection, "goff");
	h_rrZ_det_bottom->Draw("colz");
	lin_cath->Draw("same");
	lin_grnd->Draw("same");
	if (file_outplot) c_rrZ_det_bottom->Write();
	sprintf(canvasfile,"%s/%s_S1_rrZ_det_bottom.%s", workingdirectory.c_str(), rawdatafilename.c_str(), export_format.c_str());
	if (!(export_format=="")) c_rrZ_det_bottom->SaveAs(canvasfile);
	
	/*=================================================================*/
	if (!batch) {gROOT->SetBatch(kFALSE);}
	/*=================================================================*/
	/*=================================================================*/
	// LCE of R^{2} vs. Z
	/*=================================================================*/	
	style_2D->cd();
	TCanvas *c_LCE_rrZ = new TCanvas("LCE_rrZ","LCE_rrZ",canvas_x,canvas_y);
	TH2F* h_LCE_rrZ = new TH2F("LCE_rrZ", "LCE of R^{2} vs. Z (ALL PMTs)", TPC.Get_nbinsRR(), TPC.Get_LXe_minRR(), TPC.Get_LXe_maxRR(), TPC.Get_nbinsZ(), TPC.Get_LXe_minZ(), TPC.Get_LXe_maxZ());
	h_LCE_rrZ->SetXTitle("R^{2} [cm^{2}]");
	h_LCE_rrZ->GetXaxis()->CenterTitle();
	h_LCE_rrZ->SetYTitle("Z [cm]");
	h_LCE_rrZ->GetYaxis()->CenterTitle();
	h_LCE_rrZ->SetZTitle("LCE [%]");
	h_LCE_rrZ->GetZaxis()->CenterTitle();
	h_LCE_rrZ->Sumw2();
	h_LCE_rrZ->Divide(h_rrZ_det, h_rrZ, 1.,1., "b");
	h_LCE_rrZ->Scale(100.);
	h_LCE_rrZ->Draw("colz");
	lin_cath->Draw("same");
	lin_grnd->Draw("same");
	if (file_outplot) c_LCE_rrZ->Write();
	sprintf(canvasfile,"%s/%s_S1_rrZ_LCE.%s", workingdirectory.c_str(), rawdatafilename.c_str(), export_format.c_str());
	if (!(export_format=="")) c_LCE_rrZ->SaveAs(canvasfile);
	
	/*=================================================================*/
	gROOT->SetBatch(kTRUE);
	/*=================================================================*/
	/*=================================================================*/
	// LCE of R^{2} vs. Z (TOP PMTs)
	/*=================================================================*/
	style_2D->cd();
	TCanvas *c_LCE_rrZ_top = new TCanvas("LCE_rrZ_top","LCE_rrZ_top",canvas_x,canvas_y);
	TH2F* h_LCE_rrZ_top = new TH2F("LCE_rrZ_top", "LCE of R^{2} vs. Z (TOP PMTs)", TPC.Get_nbinsRR(), TPC.Get_LXe_minRR(), TPC.Get_LXe_maxRR(), TPC.Get_nbinsZ(), TPC.Get_LXe_minZ(), TPC.Get_LXe_maxZ());
	h_LCE_rrZ_top->SetXTitle("R^{2} [cm^{2}]");
	h_LCE_rrZ_top->GetXaxis()->CenterTitle();
	h_LCE_rrZ_top->SetYTitle("Z [cm]");
	h_LCE_rrZ_top->GetYaxis()->CenterTitle();
	h_LCE_rrZ_top->SetZTitle("TOP LCE [%]");
	h_LCE_rrZ_top->GetZaxis()->CenterTitle();
	h_LCE_rrZ_top->Sumw2();
	h_LCE_rrZ_top->Divide(h_rrZ_det_top, h_rrZ, 1.,1., "b");
	h_LCE_rrZ_top->Scale(100.);
	h_LCE_rrZ_top->Draw("colz");
	lin_cath->Draw("same");
	lin_grnd->Draw("same");
	if (file_outplot) c_LCE_rrZ_top->Write();
	sprintf(canvasfile,"%s/%s_S1_rrZ_LCE_top.%s", workingdirectory.c_str(), rawdatafilename.c_str(), export_format.c_str());
	if (!(export_format=="")) c_LCE_rrZ_top->SaveAs(canvasfile);
	
	/*=================================================================*/
	// LCE of R^{2} vs. Z (BOTTOM PMTs)
	/*=================================================================*/
	style_2D->cd();
	TCanvas *c_LCE_rrZ_bottom = new TCanvas("LCE_rrZ_bottom","LCE_rrZ_bottom",canvas_x,canvas_y);
	TH2F* h_LCE_rrZ_bottom = new TH2F("LCE_rrZ_bottom", "LCE of R^{2} vs. Z (BOTTOM PMTs)", TPC.Get_nbinsRR(), TPC.Get_LXe_minRR(), TPC.Get_LXe_maxRR(), TPC.Get_nbinsZ(), TPC.Get_LXe_minZ(), TPC.Get_LXe_maxZ());
	h_LCE_rrZ_bottom->SetXTitle("R^{2} [cm^{2}]");
	h_LCE_rrZ_bottom->GetXaxis()->CenterTitle();
	h_LCE_rrZ_bottom->SetYTitle("Z [cm]");
	h_LCE_rrZ_bottom->GetYaxis()->CenterTitle();
	h_LCE_rrZ_bottom->SetZTitle("BOTTOM LCE [%]");
	h_LCE_rrZ_bottom->GetZaxis()->CenterTitle();
	h_LCE_rrZ_bottom->Sumw2();
	h_LCE_rrZ_bottom->Divide(h_rrZ_det_bottom, h_rrZ, 1.,1., "b");
	h_LCE_rrZ_bottom->Scale(100.);
	h_LCE_rrZ_bottom->Draw("colz");
	lin_cath->Draw("same");
	lin_grnd->Draw("same");
	if (file_outplot) c_LCE_rrZ_bottom->Write();
	sprintf(canvasfile,"%s/%s_S1_rrZ_LCE_bottom.%s", workingdirectory.c_str(), rawdatafilename.c_str(), export_format.c_str());
	if (!(export_format=="")) c_LCE_rrZ_bottom->SaveAs(canvasfile);
	
	/*=================================================================*/
	// relative LCE of R^{2} vs. Z
	/*=================================================================*/
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
	file_outstat << "Mean LCE: " << h_LCE_rrZ_mean << "\n";
	h_rLCE_rrZ->Draw("colz");
	lin_cath->Draw("same");
	lin_grnd->Draw("same");
	if (file_outplot) c_rLCE_rrZ->Write();
	sprintf(canvasfile,"%s/%s_S1_rrZ_rLCE.%s", workingdirectory.c_str(), rawdatafilename.c_str(), export_format.c_str());
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
	h_rLCE_rrZ_top->Draw("colz");
	lin_cath->Draw("same");
	lin_grnd->Draw("same");
	if (file_outplot) c_rLCE_rrZ_top->Write();
	sprintf(canvasfile,"%s/%s_S1_rrZ_rLCE_top.%s", workingdirectory.c_str(), rawdatafilename.c_str(), export_format.c_str());
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
	h_rLCE_rrZ_bottom->Draw("colz");
	lin_cath->Draw("same");
	lin_grnd->Draw("same");
	if (file_outplot) c_rLCE_rrZ_bottom->Write();
	sprintf(canvasfile,"%s/%s_S1_rrZ_rLCE_bottom.%s", workingdirectory.c_str(), rawdatafilename.c_str(), export_format.c_str());
	if (!(export_format=="")) c_rLCE_rrZ_bottom->SaveAs(canvasfile);
	
	/*=================================================================*/
	// relative LCE of R^{2} vs. Z 3D
	/*=================================================================*/
	style_3D->cd();
	TCanvas *c_rLCE_rrZ_3D = new TCanvas("rLCE_rrZ_3D","rLCE_rrZ_3D",canvas_x,canvas_y);
	TH2F* h_rLCE_rrZ_3D = new TH2F("rLCE_rrZ_3D", "relative LCE of R^{2} vs. Z (ALL PMTs)", TPC.Get_nbinsZ(), TPC.Get_LXe_minZ(), TPC.Get_LXe_maxZ(), TPC.Get_nbinsRR(), TPC.Get_LXe_minRR(), TPC.Get_LXe_maxRR());
	h_rLCE_rrZ_3D->SetYTitle("R^{2} [cm^{2}]");
	h_rLCE_rrZ_3D->GetYaxis()->CenterTitle();
	h_rLCE_rrZ_3D->SetXTitle("Z [cm]");
	h_rLCE_rrZ_3D->GetXaxis()->CenterTitle();
	h_rLCE_rrZ_3D->SetZTitle("relative LCE");
	h_rLCE_rrZ_3D->GetZaxis()->CenterTitle();
	for (int z=0; z<TPC.Get_nbinsZ(); z++){
		for (int r=0; r<TPC.Get_nbinsRR(); r++){
				h_rLCE_rrZ_3D->SetBinContent(z+1,r+1,h_rLCE_rrZ->GetBinContent(r+1,z+1));
		}
	}
	h_rLCE_rrZ_3D->Draw("surf1");
	c_rLCE_rrZ_3D->SetTheta(20.);
	c_rLCE_rrZ_3D->SetPhi(220.);
	if (file_outplot) c_rLCE_rrZ_3D->Write();
	sprintf(canvasfile,"%s/%s_S1_rrZ_rLCE_3D.%s", workingdirectory.c_str(), rawdatafilename.c_str(), export_format.c_str());
	if (!(export_format=="")) c_rLCE_rrZ_3D->SaveAs(canvasfile);
	
	/*=================================================================*/
	/*=================================================================*/
	/*=================================================================*/
	file_outstat << "= plot LCEZ ================================================" << "\n";
	/*=================================================================*/
	/*=================================================================*/
	/*=================================================================*/
	style_1D->cd();
	/*=================================================================*/
	// generated events vs. Z
	/*=================================================================*/
	TH1F* h_LCEZ_gen = new TH1F("LCEZ_gen", "generated events vs. Z", TPC.Get_nbinsZ(), TPC.Get_LXe_minZ(), TPC.Get_LXe_maxZ());
	sprintf(draw_selection,"zp_pri/10<=%f && zp_pri/10>=%f && rrp_pri>=%f && rrp_pri<=%f",TPC.Get_LXe_maxZ(),TPC.Get_LXe_minZ(),TPC.Get_LXe_minRR(),TPC.Get_LXe_maxRR());
	file_input_tree->Draw("zp_pri/10. >> LCEZ_gen", draw_selection, "goff");
	
	/*=================================================================*/
	// detected events vs. Z (TOP + BOTTOM PMTs)
	/*=================================================================*/
	TH1F* h_LCEZ_det = new TH1F("LCEZ_det", "detected events vs. Z (ALL PMTs)", TPC.Get_nbinsZ(), TPC.Get_LXe_minZ(), TPC.Get_LXe_maxZ());
	sprintf(draw_selection,"zp_pri/10<=%f && zp_pri/10>=%f && rrp_pri>=%f && rrp_pri<=%f && (nbpmthits > 0 || ntpmthits > 0)",TPC.Get_LXe_maxZ(),TPC.Get_LXe_minZ(),TPC.Get_LXe_minRR(),TPC.Get_LXe_maxRR());
	file_input_tree->Draw("zp_pri/10. >> LCEZ_det", draw_selection, "goff");
	
	/*=================================================================*/
	// detected events vs. Z (TOP PMTs)
	/*=================================================================*/
	TH1F* h_LCEZ_det_top = new TH1F("LCEZ_det_top", "detected events vs. Z (TOP PMTs)", TPC.Get_nbinsZ(), TPC.Get_LXe_minZ(), TPC.Get_LXe_maxZ());
	sprintf(draw_selection,"zp_pri/10<=%f && zp_pri/10>=%f && rrp_pri>=%f && rrp_pri<=%f && (ntpmthits > 0)",TPC.Get_LXe_maxZ(),TPC.Get_LXe_minZ(),TPC.Get_LXe_minRR(),TPC.Get_LXe_maxRR());
	file_input_tree->Draw("zp_pri/10. >> LCEZ_det_top", draw_selection, "goff");
	
	/*=================================================================*/
	// detected events vs. Z (BOTTOM PMTs)
	/*=================================================================*/
	TH1F* h_LCEZ_det_bottom = new TH1F("LCEZ_det_bottom", "detected events vs. Z (BOTTOM PMTs)", TPC.Get_nbinsZ(), TPC.Get_LXe_minZ(), TPC.Get_LXe_maxZ());
	sprintf(draw_selection,"zp_pri/10<=%f && zp_pri/10>=%f && rrp_pri>=%f && rrp_pri<=%f && (nbpmthits > 0)",TPC.Get_LXe_maxZ(),TPC.Get_LXe_minZ(),TPC.Get_LXe_minRR(),TPC.Get_LXe_maxRR());
	file_input_tree->Draw("zp_pri/10. >> LCEZ_det_bottom", draw_selection, "goff");
	
	/*=================================================================*/
	// LCE vs. Z
	/*=================================================================*/
	TH1F* h_LCE_LCEZ = new TH1F("LCE_LCEZ", "LCE vs. Z (ALL PMTs)", TPC.Get_nbinsZ(), TPC.Get_LXe_minZ(), TPC.Get_LXe_maxZ());
	h_LCE_LCEZ->Sumw2();
	h_LCE_LCEZ->Divide(h_LCEZ_det, h_LCEZ_gen, 1.,1., "b");
	h_LCE_LCEZ->Scale(100.);
	
	/*=================================================================*/
	// LCE vs. Z (TOP PMTs)
	/*=================================================================*/
	TH1F* h_LCE_LCEZ_top = new TH1F("LCE_LCEZ_top", "LCE vs. Z (TOP PMTs)", TPC.Get_nbinsZ(), TPC.Get_LXe_minZ(), TPC.Get_LXe_maxZ());
	h_LCE_LCEZ_top->Sumw2();
	h_LCE_LCEZ_top->Divide(h_LCEZ_det_top, h_LCEZ_gen, 1.,1., "b");
	h_LCE_LCEZ_top->Scale(100.);
	
	/*=================================================================*/
	// LCE vs. Z (BOTTOM PMTs)
	/*=================================================================*/
	TH1F* h_LCE_LCEZ_bottom = new TH1F("LCE_LCEZ_bottom", "LCE vs. Z (BOTTOM PMTs)", TPC.Get_nbinsZ(), TPC.Get_LXe_minZ(), TPC.Get_LXe_maxZ());
	h_LCE_LCEZ_bottom->Sumw2();
	h_LCE_LCEZ_bottom->Divide(h_LCEZ_det_bottom, h_LCEZ_gen, 1.,1., "b");
	h_LCE_LCEZ_bottom->Scale(100.);
	
	/*=================================================================*/
	if (!batch) {gROOT->SetBatch(kFALSE);}
	/*=================================================================*/
	/*=================================================================*/
	// LCE vs. Z (ALL)
	/*=================================================================*/
	style_1D->cd();
	TCanvas *c_LCE_LCEZ_ALL = new TCanvas("LCE_LCEZ_ALL","LCE_LCEZ_ALL",canvas_y,canvas_x);
	c_LCE_LCEZ_ALL->SetGridy();
	h_LCE_LCEZ->SetTitle("LCE vs. Z");
	h_LCE_LCEZ->SetXTitle("Z [cm]");
	h_LCE_LCEZ->GetXaxis()->CenterTitle();
	h_LCE_LCEZ->SetYTitle("LCE [%]");
	h_LCE_LCEZ->GetYaxis()->CenterTitle();
	h_LCE_LCEZ->SetLineColor(kBlue);
	h_LCE_LCEZ->GetYaxis()->SetRangeUser(TPC.Get_LCE_min(),TPC.Get_LCE_max());
	h_LCE_LCEZ->Draw();
	h_LCE_LCEZ_bottom->SetLineColor(kGreen);
	h_LCE_LCEZ_bottom->Draw("e same");
	h_LCE_LCEZ_top->SetLineColor(kRed);
	h_LCE_LCEZ_top->Draw("e same");
	lin_cath_Z->Draw("same");
	lin_grnd_Z->Draw("same");
	
	TLegend *leg_LCE_LCEZ = new TLegend(0.64,0.77,0.97,0.97);
	leg_LCE_LCEZ->SetFillColor(0);
	leg_LCE_LCEZ->SetTextSize(0.04);
	leg_LCE_LCEZ->SetTextAlign(22);         
	leg_LCE_LCEZ->AddEntry(h_LCE_LCEZ,"All PMTs","l"); 
	leg_LCE_LCEZ->AddEntry(h_LCE_LCEZ_top,"Top PMTs","l");
	leg_LCE_LCEZ->AddEntry(h_LCE_LCEZ_bottom,"Bottom PMTs","l");
	leg_LCE_LCEZ->Draw();    

	if (file_outplot) c_LCE_LCEZ_ALL->Write();	
	sprintf(canvasfile,"%s/%s_S1_LCEz.%s", workingdirectory.c_str(), rawdatafilename.c_str(), export_format.c_str());
	if (!(export_format=="")) c_LCE_LCEZ_ALL->SaveAs(canvasfile);

	/*=================================================================*/
	gROOT->SetBatch(kTRUE);
	/*=================================================================*/
	/*=================================================================*/
	// relative LCE vs. Z (ALL)
	/*=================================================================*/
	style_1D->cd();
	TCanvas *c_LCE_rLCEZ_ALL = new TCanvas("LCE_rLCEZ_ALL","LCE_rLCEZ_ALL",canvas_y,canvas_x);
	
	c_LCE_rLCEZ_ALL->SetGridy();
	double h_rLCE_LCEZ_mean = 0;
	for (int z=0; z<(TPC.Get_nbinsZ()); z++){
		h_rLCE_LCEZ_mean += h_LCE_LCEZ->GetBinContent(z+1)/(TPC.Get_nbinsZ());
	}
	file_outstat << "Mean LCE: " << h_rLCE_LCEZ_mean << "\n";
	h_LCE_LCEZ->Scale(1./h_rLCE_LCEZ_mean);
	h_LCE_LCEZ->SetYTitle("relative LCE");
	h_LCE_LCEZ->Draw();
	h_LCE_LCEZ_bottom->Scale(1./h_rLCE_LCEZ_mean);
	h_LCE_LCEZ_bottom->Draw();
	h_LCE_LCEZ_top->Scale(1./h_rLCE_LCEZ_mean);
	h_LCE_LCEZ_top->Draw();
	
	h_LCE_LCEZ->SetTitle("relative LCE vs. Z");
	h_LCE_LCEZ->SetLineColor(kBlue);
	h_LCE_LCEZ->GetYaxis()->SetRangeUser(0,2.5);
	h_LCE_LCEZ->Draw();
	h_LCE_LCEZ_bottom->SetLineColor(kGreen);
	h_LCE_LCEZ_bottom->Draw("e same");
	h_LCE_LCEZ_top->SetLineColor(kRed);
	h_LCE_LCEZ_top->Draw("e same");
	lin_cath_Z->Draw("same");
	lin_grnd_Z->Draw("same");

	TLegend *leg_LCE_rLCEZ = new TLegend(0.64,0.77,0.97,0.97);
	leg_LCE_rLCEZ->SetFillColor(0);
	leg_LCE_rLCEZ->SetTextSize(0.04);
	leg_LCE_rLCEZ->SetTextAlign(22);         
	leg_LCE_rLCEZ->AddEntry(h_LCE_LCEZ,"All PMTs","l"); 
	leg_LCE_rLCEZ->AddEntry(h_LCE_LCEZ_top,"Top PMTs","l");
	leg_LCE_rLCEZ->AddEntry(h_LCE_LCEZ_bottom,"Bottom PMTs","l"); 
	leg_LCE_rLCEZ->Draw();    

	if (file_outplot) c_LCE_rLCEZ_ALL->Write();	
	sprintf(canvasfile,"%s/%s_S1_rLCEz.%s", workingdirectory.c_str(), rawdatafilename.c_str(), export_format.c_str());
	if (!(export_format=="")) c_LCE_rLCEZ_ALL->SaveAs(canvasfile);
	
	/*=================================================================*/
	/*=================================================================*/
	/*=================================================================*/
	file_outstat << "= plot LCErr ===============================================" << "\n";
	/*=================================================================*/
	/*=================================================================*/
	/*=================================================================*/
	style_1D->cd();
	/*=================================================================*/
	if (!batch) {gROOT->SetBatch(kFALSE);}
	/*=================================================================*/
	/*=================================================================*/
	// generated events vs. R^{2}
	/*=================================================================*/
	TH1F* h_LCErr_gen = new TH1F("LCErr_gen", "generated events vs. R^{2}", TPC.Get_nbinsRR(), TPC.Get_LXe_minRR(), TPC.Get_LXe_maxRR());
	sprintf(draw_selection,"zp_pri/10<=%f && zp_pri/10>=%f && rrp_pri>=%f && rrp_pri<=%f",TPC.Get_LXe_maxZ(),TPC.Get_LXe_minZ(),TPC.Get_LXe_minRR(),TPC.Get_LXe_maxRR());
	file_input_tree->Draw("rrp_pri >> LCErr_gen", draw_selection, "goff");
	
	/*=================================================================*/
	// detected events vs. R^{2} (TOP + BOTTOM PMTs)
	/*=================================================================*/
	TH1F* h_LCErr_det = new TH1F("LCErr_det", "detected events vs. R^{2} (ALL PMTs)", TPC.Get_nbinsRR(), TPC.Get_LXe_minRR(), TPC.Get_LXe_maxRR());
	sprintf(draw_selection,"zp_pri/10<=%f && zp_pri/10>=%f && rrp_pri>=%f && rrp_pri<=%f && (nbpmthits > 0 || ntpmthits > 0)",TPC.Get_LXe_maxZ(),TPC.Get_LXe_minZ(),TPC.Get_LXe_minRR(),TPC.Get_LXe_maxRR());
	file_input_tree->Draw("rrp_pri >> LCErr_det", draw_selection, "goff");
	
	/*=================================================================*/
	// detected events vs. R^{2} (TOP PMTs)
	/*=================================================================*/
	TH1F* h_LCErr_det_top = new TH1F("LCErr_det_top", "detected events vs. R^{2} (TOP PMTs)", TPC.Get_nbinsRR(), TPC.Get_LXe_minRR(), TPC.Get_LXe_maxRR());
	sprintf(draw_selection,"zp_pri/10<=%f && zp_pri/10>=%f && rrp_pri>=%f && rrp_pri<=%f && (ntpmthits > 0)",TPC.Get_LXe_maxZ(),TPC.Get_LXe_minZ(),TPC.Get_LXe_minRR(),TPC.Get_LXe_maxRR());
	file_input_tree->Draw("rrp_pri >> LCErr_det_top", draw_selection, "goff");
	
	/*=================================================================*/
	// detected events vs. R^{2} (BOTTOM PMTs)
	/*=================================================================*/
	TH1F* h_LCErr_det_bottom = new TH1F("LCErr_det_bottom", "detected events vs. R^{2} (BOTTOM PMTs)", TPC.Get_nbinsRR(), TPC.Get_LXe_minRR(), TPC.Get_LXe_maxRR());
	sprintf(draw_selection,"zp_pri/10<=%f && zp_pri/10>=%f && rrp_pri>=%f && rrp_pri<=%f && (nbpmthits > 0)",TPC.Get_LXe_maxZ(),TPC.Get_LXe_minZ(),TPC.Get_LXe_minRR(),TPC.Get_LXe_maxRR());
	file_input_tree->Draw("rrp_pri >> LCErr_det_bottom", draw_selection, "goff");
	
	/*=================================================================*/
	// LCE vs. R^{2}
	/*=================================================================*/
	TH1F* h_LCE_LCErr = new TH1F("LCE_LCErr", "LCE vs. R^{2} (ALL PMTs)", TPC.Get_nbinsRR(), TPC.Get_LXe_minRR(), TPC.Get_LXe_maxRR());
	h_LCE_LCErr->Sumw2();
	h_LCE_LCErr->Divide(h_LCErr_det, h_LCErr_gen, 1.,1., "b");
	h_LCE_LCErr->Scale(100.);

	/*=================================================================*/
	// LCE vs. R^{2} (TOP PMTs)
	/*=================================================================*/
	TH1F* h_LCE_LCErr_top = new TH1F("LCE_LCErr_top", "LCE vs. R^{2} (TOP PMTs)", TPC.Get_nbinsRR(), TPC.Get_LXe_minRR(), TPC.Get_LXe_maxRR());
	h_LCE_LCErr_top->Sumw2();
	h_LCE_LCErr_top->Divide(h_LCErr_det_top, h_LCErr_gen, 1.,1., "b");
	h_LCE_LCErr_top->Scale(100.);
	
	/*=================================================================*/
	// LCE vs. R^{2} (BOTTOM PMTs)
	/*=================================================================*/
	TH1F* h_LCE_LCErr_bottom = new TH1F("LCE_LCErr_bottom", "LCE of R^{2} vs. Z (BOTTOM PMTs)", TPC.Get_nbinsRR(), TPC.Get_LXe_minRR(), TPC.Get_LXe_maxRR());
	h_LCE_LCErr_bottom->Sumw2();
	h_LCE_LCErr_bottom->Divide(h_LCErr_det_bottom, h_LCErr_gen, 1.,1., "b");
	h_LCE_LCErr_bottom->Scale(100.);

	/*=================================================================*/
	// LCE vs. R^{2} (ALL)
	/*=================================================================*/
	style_1D->cd();
	TCanvas *c_LCE_LCErr_ALL = new TCanvas("LCE_LCErr_ALL","LCE_LCErr_ALL",canvas_x,canvas_y);
	
	c_LCE_LCErr_ALL->SetGridy();
	double h_rLCE_LCErr_mean = 0;
	for (int R=0; R<(TPC.Get_nbinsRR()); R++){
		h_rLCE_LCErr_mean += h_LCE_LCErr->GetBinContent(R+1)/(TPC.Get_nbinsRR());
	}
	file_outstat << "Mean LCE: " << h_rLCE_LCErr_mean << "\n";
	h_LCE_LCErr->SetTitle("LCE vs. R^{2}");
	h_LCE_LCErr->SetXTitle("R^{2} [cm^{2}]");
	h_LCE_LCErr->GetXaxis()->CenterTitle();
	h_LCE_LCErr->SetYTitle("LCE [%]");
	h_LCE_LCErr->GetYaxis()->CenterTitle();
	h_LCE_LCErr->SetLineColor(kBlue);
	h_LCE_LCErr->GetYaxis()->SetRangeUser(TPC.Get_LCE_min(),TPC.Get_LCE_max());
	h_LCE_LCErr->Draw();
	h_LCE_LCErr_bottom->SetLineColor(kGreen);
	h_LCE_LCErr_bottom->Draw("e same");
	h_LCE_LCErr_top->SetLineColor(kRed);
	h_LCE_LCErr_top->Draw("e same");
	lin_cath_rr->Draw("same");
	lin_grnd_rr->Draw("same");

	TLegend *leg = new TLegend(0.64,0.77,0.97,0.97);
	leg->SetFillColor(0);
	leg->SetTextSize(0.04);
	leg->SetTextAlign(22);         
	leg->AddEntry(h_LCE_LCErr,"All PMTs","l"); 
	leg->AddEntry(h_LCE_LCErr_top,"Top PMTs","l");
	leg->AddEntry(h_LCE_LCErr_bottom,"Bottom PMTs","l"); 
	leg->Draw();    

	if (file_outplot) c_LCE_LCErr_ALL->Write();	
	sprintf(canvasfile,"%s/%s_S1_LCErr.%s", workingdirectory.c_str(), rawdatafilename.c_str(), export_format.c_str());
	if (!(export_format=="")) c_LCE_LCErr_ALL->SaveAs(canvasfile);
	
	/*=================================================================*/
	gROOT->SetBatch(kFALSE);
	/*=================================================================*/
	file_outstat << "============================================================" << "\n";
	file_outstat.close();	
	//file_outplot->Close(); 
}

/*=================================================================*/
