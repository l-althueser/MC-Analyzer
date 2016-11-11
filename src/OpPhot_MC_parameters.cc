/******************************************************************
 * MC plot parameters
 * 
 * @author	Lutz Althueser
 *
 ******************************************************************/
 // include own functions
#include "fileoperations.cc"

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

/*=================================================================*/

void OpPhot_MC_parameters(string datafile, string export_format, string parameter, string suffix) {
	
	// read in datafilename and get working directory
	size_t found=datafile.find_last_of("/\\");
	string workingdirectory = datafile.substr(0,found);
	string datafilename = datafile.substr(found+1);
	
	int param_min = 0;
	int	param_max = 0;
	
	Int_t canvas_x = 650;
	Int_t canvas_y = 800;

	char canvasfile[10000];
	char draw_selection[10000];
	
	TPC_Definition TPC;
	
	TGaxis::SetMaxDigits(3);
	
	const Int_t NRGBs = 6;
	const Int_t NCont = 255;
	Double_t stops[NRGBs] = { 0.00, 0.15, 0.34, 0.61, 0.84, 1.00 };
	Double_t red[NRGBs]   = { 1.00, 0.00, 0.00, 0.87, 1.00, 0.51 };
	Double_t green[NRGBs] = { 1.00, 0.00, 0.81, 1.00, 0.20, 0.00 };
	Double_t blue[NRGBs]  = { 1.00, 0.51, 1.00, 0.12, 0.00, 0.00 };
	
	TStyle *style_1D = new TStyle("1D","1D");
	style_1D->SetCanvasColor(10);
	style_1D->SetTitleFillColor(0);
	style_1D->SetOptStat(0);
	style_1D->SetMarkerStyle(20);
	style_1D->SetMarkerSize(1.5);
	style_1D->SetPadLeftMargin(0.105);
	style_1D->SetPadRightMargin(0.075);
	style_1D->SetPadTopMargin(0.025);
	style_1D->SetPadBottomMargin(0.075);
	
	style_1D->SetTitleOffset(1.,"X");
	style_1D->SetTitleOffset(1.45,"Y");
	style_1D->SetTitleOffset(1.35,"Z");
	
	style_1D->SetPalette(1,0); 
	TColor::CreateGradientColorTable(NRGBs, stops, red, green, blue, NCont);
	style_1D->SetNumberContours(NCont);
	style_1D->cd();

	TFile *file_input;
	
	char file_outname[10000];
	sprintf(file_outname,"%s/%s_plot_MC_%s.dat", workingdirectory.c_str(),suffix.c_str(), parameter.c_str());
	
	ofstream file_outstat;
	file_outstat.open(file_outname);
	file_outstat << "============================================================" << "\n";
	file_outstat << "= geometry parameters ======================================" << "\n";
	file_outstat << "parameter: " << parameter << "\n";
	file_outstat << "chamber_minZ: " << TPC.Get_chamber_minZ() << "\n";
	file_outstat << "chamber_maxZ: " << TPC.Get_chamber_maxZ() << "\n";
	file_outstat << "chamber_minRR: " << TPC.Get_chamber_minRR() << "\n";
	file_outstat << "chamber_maxRR: " << TPC.Get_chamber_maxRR() << "\n";
	file_outstat << "LXe_minZ: " << TPC.Get_LXe_minZ() << "\n";
	file_outstat << "LXe_maxZ: " << TPC.Get_LXe_maxZ() << "\n";
	file_outstat << "LXe_minRR: " << TPC.Get_LXe_minRR() << "\n";
	file_outstat << "LXe_maxRR: " << TPC.Get_LXe_maxRR() << "\n";
	file_outstat << "nbinsZ: " << TPC.Get_nbinsZ() << "\n";
	file_outstat << "nbinsRR: " << TPC.Get_nbinsRR() << "\n";
	file_outstat << "LCE_min: " << TPC.Get_LCE_min() << "\n";
	file_outstat << "LCE_max: " << TPC.Get_LCE_max() << "\n";
	file_outstat << "============================================================" << "\n";
	cout << "============================================================" << endl;
	
	if (fileexists(datafile) == false) {
		cout << endl;
		cout << "x Error xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx" << endl;
		cout << "directory not found:" << endl;
		cout << "-> " << datafile << endl;
		cout << "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx" << endl;
		cout << endl;
		gApplication->Terminate();
	}
	else if ((fileexists(datafile) == true) && (datafilename=="")) {
		file_outstat << "= reading datafiles ==== dir mode ==========================" << "\n";
		cout << "= reading datafiles ==== dir mode ==========================" << endl;
		
		// generate plots
		TFile *file_outplot;  
		sprintf(file_outname,"%s/%s_plot_MC_%s.root", workingdirectory.c_str(),suffix.c_str(), parameter.c_str());
		file_outplot = new TFile(file_outname,"RECREATE");
		
		TH1F* h_LCE_paramter = new TH1F("LCE_parameter", "LCE_parameter", 15000, 0., 15000.);
		
		TH1F* h_LCE_paramter_top = new TH1F("LCE_parameter_top", "LCE_parameter_top", 15000, 0., 15000.);
		
		TH1F* h_AFT_paramter_MC = new TH1F("AFT_paramter_MC", "AFT_paramter_MC", 15000, 0., 15000.);
		
		TH1F* h_LCE_paramter_bottom = new TH1F("LCE_parameter_bottom", "LCE_parameter_bottom", 15000, 0., 15000.);
		
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
					string input_file = fname.Data();
					char filename[10000];
					sprintf(filename,"%s/%s", workingdirectory.c_str(), input_file.c_str());
					TChain *file_input_tree = new TChain("events/events");
					if (file_input_tree->GetEntries() == 0) {
						TFile *f = new TFile(filename,"READ");
						if ( (f->GetListOfKeys()->Contains("events")) && !(f->GetListOfKeys()->Contains("MC_TAG")) ) {
							TPC.TPC_Xe1T();
							TPC.Set_LCE_max(50);
						}
						else if ( (f->GetListOfKeys()->Contains("MC_TAG")) && (f->GetListOfKeys()->Contains("events")) ){
							TPC.TPC_MS();
							TPC.Set_LCE_max(30);
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
					file_input_tree->SetAlias("rrp_pri","(xp_pri*xp_pri + yp_pri*yp_pri)/10./10.");  
					const int nevents = file_input_tree->GetEntries();
					file_outstat << "= file: " << input_file << " " << nevents << " events total" << "\n";
					cout << "= file: " << input_file << " " << nevents << " events total" << endl;
					
					// read in parameter value
					size_t found_score=input_file.find_last_of("_");
					string parameter_value = input_file.substr(found_score+1);
					size_t found_dot=parameter_value.find_last_of(".");
					parameter_value = parameter_value.substr(0,found_dot);
					
					if (h_LCE_paramter->GetEntries() == 0) {
						param_min = atoi(parameter_value.c_str());
						param_max = atoi(parameter_value.c_str());
					}
					else {
						param_min = min(param_min,atoi(parameter_value.c_str()));
						param_max = max(param_max,atoi(parameter_value.c_str()));
					}
					
					sprintf(draw_selection,"zp_pri/10<=%f && zp_pri/10>=%f && rrp_pri>=%f && rrp_pri<=%f",TPC.Get_LXe_maxZ(),TPC.Get_LXe_minZ(),TPC.Get_LXe_minRR(),TPC.Get_LXe_maxRR());
					file_input_tree->Draw(">>e_list_LXe_gen",draw_selection,"goff");
					TEntryList *elist_LXe_gen = (TEntryList*)gDirectory->Get("e_list_LXe_gen");
					file_outstat << "generated events (LXe): " << elist_LXe_gen->GetEntriesToProcess() << "\n";
					sprintf(draw_selection,"zp_pri/10<=%f && zp_pri/10>=%f && rrp_pri>=%f && rrp_pri<=%f && (nbpmthits > 0 || ntpmthits > 0)",TPC.Get_LXe_maxZ(),TPC.Get_LXe_minZ(),TPC.Get_LXe_minRR(),TPC.Get_LXe_maxRR());
					file_input_tree->Draw(">>e_list_LXe_det",draw_selection,"goff");
					TEntryList *elist_LXe_det = (TEntryList*)gDirectory->Get("e_list_LXe_det");
					file_outstat << "detected events (LXe): " << elist_LXe_det->GetEntriesToProcess() << "\n";
					file_outstat << "Mean LCE LXe(detected/generated): " << (double)elist_LXe_det->GetEntriesToProcess()/(double)elist_LXe_gen->GetEntriesToProcess()*100. << "\n";
					
					h_LCE_paramter->Fill(atoi(parameter_value.c_str()),(double)elist_LXe_det->GetEntriesToProcess()/(double)elist_LXe_gen->GetEntriesToProcess()*100.);
					
					sprintf(draw_selection,"zp_pri/10<=%f && zp_pri/10>=%f && rrp_pri>=%f && rrp_pri<=%f && (ntpmthits > 0)",TPC.Get_LXe_maxZ(),TPC.Get_LXe_minZ(),TPC.Get_LXe_minRR(),TPC.Get_LXe_maxRR());
					file_input_tree->Draw(">>e_list_LXe_det_top",draw_selection,"goff");
					TEntryList *elist_LXe_det_top = (TEntryList*)gDirectory->Get("e_list_LXe_det_top");
					file_outstat << "detected events (LXe TOP): " << elist_LXe_det_top->GetEntriesToProcess() << "\n";
					file_outstat << "Mean LCE LXe(detected TOP/generated): " << (double)elist_LXe_det_top->GetEntriesToProcess()/(double)elist_LXe_gen->GetEntriesToProcess()*100. << "\n";
					file_outstat << "AreaFractionTop LXe(detected TOP/detected): " << (double)elist_LXe_det_top->GetEntriesToProcess()/(double)elist_LXe_det->GetEntriesToProcess()*100. << "\n";
					
					h_LCE_paramter_top->Fill(atoi(parameter_value.c_str()),(double)elist_LXe_det_top->GetEntriesToProcess()/(double)elist_LXe_gen->GetEntriesToProcess()*100.);
					h_AFT_paramter_MC->Fill(atoi(parameter_value.c_str()),(double)elist_LXe_det_top->GetEntriesToProcess()/(double)elist_LXe_det->GetEntriesToProcess()*100.);
					
					sprintf(draw_selection,"zp_pri/10<=%f && zp_pri/10>=%f && rrp_pri>=%f && rrp_pri<=%f && (nbpmthits > 0)",TPC.Get_LXe_maxZ(),TPC.Get_LXe_minZ(),TPC.Get_LXe_minRR(),TPC.Get_LXe_maxRR());
					file_input_tree->Draw(">>e_list_LXe_det_bottom",draw_selection,"goff");
					TEntryList *elist_LXe_det_bottom = (TEntryList*)gDirectory->Get("e_list_LXe_det_bottom");
					file_outstat << "detected events (LXe BOT): " << elist_LXe_det_bottom->GetEntriesToProcess() << "\n";
					file_outstat << "Mean LCE LXe(detected Bot/generated): " << (double)elist_LXe_det_bottom->GetEntriesToProcess()/(double)elist_LXe_gen->GetEntriesToProcess()*100. << "\n";
					file_outstat << "AreaFractionBot LXe(detected BOT/detected): " << (double)elist_LXe_det_bottom->GetEntriesToProcess()/(double)elist_LXe_det->GetEntriesToProcess()*100. << "\n";
					
					h_LCE_paramter_bottom->Fill(atoi(parameter_value.c_str()),(double)elist_LXe_det_bottom->GetEntriesToProcess()/(double)elist_LXe_gen->GetEntriesToProcess()*100.);
					
				}
			}
		}
		file_outstat << "============================================================" << "\n";
		cout << "============================================================" << endl;
		
		style_1D->cd();
		TCanvas *c_AFT_paramter = new TCanvas("AFT_parameter","AFT_parameter",canvas_x,canvas_y);
		
		c_AFT_paramter->SetGridy();
		h_AFT_paramter_MC->SetTitle("");
		h_AFT_paramter_MC->SetXTitle(parameter.c_str());
		h_AFT_paramter_MC->GetXaxis()->CenterTitle();
		h_AFT_paramter_MC->SetYTitle("AreaFractionTop [%]");
		h_AFT_paramter_MC->GetYaxis()->CenterTitle();
		h_AFT_paramter_MC->GetXaxis()->SetRangeUser(param_min-5,param_max+5);
		h_AFT_paramter_MC->GetYaxis()->SetRangeUser(TPC.Get_LCE_min(),TPC.Get_LCE_max());
		h_AFT_paramter_MC->SetLineColor(kRed);
		h_AFT_paramter_MC->SetMarkerColor(kRed);
		h_AFT_paramter_MC->Draw("P");  
		
		if (file_outplot) c_AFT_paramter->Write();	
	sprintf(canvasfile,"%s/%s_plot_MC_%s_AFT.%s", workingdirectory.c_str(),suffix.c_str(), parameter.c_str(),export_format.c_str());
		if (!(export_format=="")) c_AFT_paramter->SaveAs(canvasfile);
		
		style_1D->cd();
		TCanvas *c_LCE_paramter = new TCanvas("LCE_parameter","LCE_parameter",canvas_x,canvas_y);
		
		c_LCE_paramter->SetGridy();
		h_LCE_paramter->SetTitle("");
		h_LCE_paramter->SetXTitle(parameter.c_str());
		h_LCE_paramter->GetXaxis()->CenterTitle();
		h_LCE_paramter->SetYTitle("LCE [%]");
		h_LCE_paramter->GetYaxis()->CenterTitle();
		h_LCE_paramter->GetXaxis()->SetRangeUser(param_min-5,param_max+5);
		h_LCE_paramter->GetYaxis()->SetRangeUser(TPC.Get_LCE_min(),TPC.Get_LCE_max());
		h_LCE_paramter->SetLineColor(kBlue);
		h_LCE_paramter->SetMarkerColor(kBlue);
		h_LCE_paramter->Draw("P");
		h_LCE_paramter_top->SetLineColor(kRed);
		h_LCE_paramter_top->SetMarkerColor(kRed);
		h_LCE_paramter_top->Draw("P same");
		h_LCE_paramter_bottom->SetLineColor(kGreen);
		h_LCE_paramter_bottom->SetMarkerColor(kGreen);
		h_LCE_paramter_bottom->Draw("P same");
		
		TLegend *leg = new TLegend(0.64,0.8,0.97,0.97);
		leg->SetFillColor(0);
		leg->SetTextSize(0.04);
		leg->SetTextAlign(22);         
		leg->AddEntry(h_LCE_paramter,"All PMTs","P"); 
		leg->AddEntry(h_LCE_paramter_top,"Top PMTs","P");
		leg->AddEntry(h_LCE_paramter_bottom,"Bottom PMTs","P"); 
		leg->Draw();    
		
		if (file_outplot) c_LCE_paramter->Write();	
	sprintf(canvasfile,"%s/%s_plot_MC_%s.%s", workingdirectory.c_str(),suffix.c_str(), parameter.c_str(),export_format.c_str());
		if (!(export_format=="")) c_LCE_paramter->SaveAs(canvasfile);
		
	}
	else {
		cout << endl;
		cout << "x Error xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx" << endl;
		cout << "You have to specify a directory:" << endl;
		cout << "-> " << datafile << endl;
		cout << "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx" << endl;
		cout << endl;
		gApplication->Terminate();
	}

	file_outstat.close();
	gROOT->SetBatch(kFALSE);	
	//file_outplot->Close();  
}