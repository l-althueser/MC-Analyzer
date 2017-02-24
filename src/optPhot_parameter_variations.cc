/******************************************************************
 * MC plot parameters
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
#include <math.h>
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
#include "TPaveText.h"
#include "TCanvas.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TGraph.h"
#include "TGraphErrors.h"
#include "TLegend.h"
#include "TStyle.h"
#include "TMultiGraph.h"
#include "TFile.h"
#include "TLine.h"
#include "TColor.h"
#include "TEntryList.h"
#include "TGaxis.h"
#include "TError.h"

using namespace std;

void optPhot_parameter_variations(string datadir, string parameter, string output_dir, string export_format, bool batch);
void optPhot_parameter_variations(string datadir, string parameter, int bin_z, string output_dir, string export_format, bool batch);

/*=================================================================*/
void optPhot_parameter_variations(string datadir, string parameter, string output_dir = "", string export_format = "png", bool batch = true) {
	optPhot_parameter_variations(datadir,parameter,30,output_dir,export_format,batch);
}

/*=================================================================*/

void optPhot_parameter_variations(string datadir, string parameter, int bin_z, string output_dir = "", string export_format = "png", bool batch = true) {
	
	//gErrorIgnoreLevel = kPrint, kInfo, kWarning, kError, kBreak, kSysError, kFatal;
	gErrorIgnoreLevel = kPrint;
	
	// read in datafilename and get working directory
	size_t found=datadir.find_last_of("/\\");
	string workingdirectory = datadir.substr(0,found);
	string datafilename = datadir.substr(found+1);
	
	if (output_dir == "") {output_dir = workingdirectory;}
	else if (output_dir.find("\\", output_dir.size())) {output_dir = output_dir.substr(0,output_dir.size()-1);}
	if (fileexists(output_dir) == false) {
		cout << endl;
		cout << "x Error xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx" << endl;
		cout << "output directory not found:" << endl;
		cout << "-> " << output_dir << endl;
		cout << "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx" << endl;
		cout << endl;
		gApplication->Terminate();
	}
	
	int param_min = 0;
	int	param_max = 0;
	int param_min_S2 = 0;
	int	param_max_S2 = 0;
	
	Int_t canvas_x = 1000;
	Int_t canvas_y = 625;

	char canvasfile[10000];
	char draw_selection[10000];
	string g_TitleX;
	
	const char* const DELIMITER = " ";
	const char* const DELIMITER_ = "_";
	char* token[100] = {}; // initialize to 0
	
	TPC_Definition TPC(bin_z,50,22); // bin_z = 30
	TNamed *G4MCname = 0;
	TNamed *MCVERSION_TAG = 0;
	
	//TGaxis::SetMaxDigits(4);
	//TGaxis::SetExponentOffset(-0.01, 0.01, "y"); // X and Y offset for Y axis
	//TGaxis::SetExponentOffset(0.01, -0.0325, "x"); // Y and Y offset for X axis
	
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
	style_1D->SetMarkerStyle(20);
	style_1D->SetMarkerSize(1.5);
	style_1D->SetPadLeftMargin(0.08);
	style_1D->SetPadRightMargin(0.03);
	style_1D->SetPadTopMargin(0.025);
	style_1D->SetPadBottomMargin(0.085);
	
	style_1D->SetTitleOffset(1.1,"X");
	style_1D->SetTitleOffset(1.05,"Y");
	style_1D->SetTitleOffset(1.35,"Z");
	
	style_1D->SetPalette(NCont,ColPalette);
	style_1D->SetNumberContours(NCont);
	style_1D->cd();
	
	gStyle->SetPalette(NCont,ColPalette);

	TFile *file_input;
	TFile *file_outplot; 
	
	char file_outname[10000];
	sprintf(file_outname,"%s/paramvar_%s.dat", output_dir.c_str(), parameter.c_str());
	
	ofstream file_outstat;
	file_outstat.open(file_outname);
	file_outstat << "============================================================" << "\n";
	file_outstat << "= geometry parameters ======================================" << "\n";
	file_outstat << "parameter: " << parameter << "\n";
	file_outstat << "binning: " << bin_z << " " << "\n";
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
	cout << "============================================================" << endl;
	
	if (fileexists(datadir) == false) {
		cout << endl;
		cout << "x Error xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx" << endl;
		cout << "directory not found:" << endl;
		cout << "-> " << datadir << endl;
		cout << "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx" << endl;
		cout << endl;
		gApplication->Terminate();
	}
	else if ((fileexists(datadir) == true) && (datafilename=="")) {
		file_outstat << "= reading datafiles ==== dir mode ==========================" << "\n";
		cout << "= reading datafiles ==== dir mode ==========================" << endl;
		
		// generate plots 
		sprintf(file_outname,"%s/paramvar_%s.root", output_dir.c_str(), parameter.c_str());
		file_outplot = new TFile(file_outname,"RECREATE");
		
		vector<double> param_value;
		vector<double> LCE_sum;
		vector<double> LCE_sum_err;
		vector<double> LCE_top;
		vector<double> LCE_top_err;
		vector<double> LCE_bottom;
		vector<double> LCE_bottom_err;
		vector<double> AFT;
		vector<double> AFT_err;
		
		vector<double> param_value_S2;
		vector<double> AFT_S2;
		vector<double> AFT_S2_err;
		
		vector<TH1F*> h_LCEZ_sum;
		vector<TH1F*> h_LCEZ_top;
		vector<TH1F*> h_LCEZ_bottom;
		vector<TH1F*> h_AFTZ;
		
		string ext = ".root";
		string exclude = "paramvar_";
		TSystemDirectory dir(workingdirectory.c_str(), workingdirectory.c_str());
		TList *files = dir.GetListOfFiles();
		if (files) {
			TSystemFile *file;
			TString fname;
			TIter next(files);
			while ((file=(TSystemFile*)next())) {
				fname = file->GetName();
				if (!file->IsDirectory() && fname.EndsWith(ext.c_str()) && !(fname.Contains(exclude.c_str()))) {
					string input_file = fname.Data();
					char filename[10000];
					sprintf(filename,"%s/%s", workingdirectory.c_str(), input_file.c_str());
					TChain *file_input_tree = new TChain("events/events");
					
					TFile *f = new TFile(filename,"READ");
					if (f->GetListOfKeys()->Contains("MC_TAG")) {
						f->GetObject("MC_TAG",G4MCname);
						f->GetObject("MCVERSION_TAG",MCVERSION_TAG);
					}
					else {
						G4MCname = new TNamed("MC_TAG","Xenon1t");
						MCVERSION_TAG = new TNamed("MCVERSION_TAG","unknown");
					}
					TPC.Init(G4MCname);
					f->Close();
					
					file_input_tree->AddFile(filename); 
					file_input_tree->SetAlias("rrp_pri","(xp_pri*xp_pri + yp_pri*yp_pri)/10./10.");  
					const int nevents = file_input_tree->GetEntries();
					if (nevents==0) {continue;}
					
					// read in parameter value
					// e.g.: rev338_S1_90_90_5000_30_30_163_01 -> VERSIONTAG_SIGNTYPE_LXeTR_GXeTR_LXeAbsL_GXeAbsL_LXeRSL_LXeRef_NUMBER
					char* buf = strdup(fname.Data());
					token[0] = strtok(buf, DELIMITER_); // first token
					for (int n = 1; n < 10; n++) {
						token[n] = strtok(0, DELIMITER_); // subsequent tokens
						if (!token[n]) break; // no more tokens
					}
					
					if (strcmp(token[1],"S1") == 0) {
						
						if ((parameter == "LXeTR") || (parameter=="LXeTeflonReflectivity")) {
							param_value.push_back(atoi(token[2]));  
							g_TitleX="LXeTeflonReflectivity [%]";
						} else if ((parameter == "GXeTR") || (parameter=="GXeTeflonReflectivity")) {
							param_value.push_back(atoi(token[3]));   
							g_TitleX="GXeTeflonReflectivity [%]";
						} else if ((parameter == "LXeAbsL") || (parameter=="LXeAbsorbtionLength")) {
							param_value.push_back(atoi(token[4]));   
							g_TitleX="LXeAbsorbtionLength [cm]";
						} else if ((parameter == "GXeAbsL") || (parameter=="GXeAbsorbtionLength")) {
							param_value.push_back(atoi(token[5]));  
							g_TitleX="GXeAbsorbtionLength [cm]";
						} else if ((parameter == "LXeRSL") || (parameter=="LXeRayScatterLength")) {
							param_value.push_back(atoi(token[6]));  
							g_TitleX="LXeRayScatterLength [cm]";
						} else if ((parameter == "LXeRef") || (parameter=="LXeRefractionIndex")) {
							param_value.push_back(atoi(token[7]));  
							g_TitleX="LXeRefractionIndex";
						} else {
							return;  
						}  
						
						file_outstat << "= file: " << input_file << " " << nevents << " events total." << " param: " << param_value.back() << "\n";
						cout << "= file: " << input_file << " " << nevents << " events total." << " param: " << param_value.back() << endl;
						
						//if (h_LCE_paramter->GetEntries() == 0) {
						if (LCE_sum.size() == 0) {
							param_min = param_value.back();
							param_max = param_value.back();
						}
						else {
							param_min = min(param_min,(int)param_value.back());
							param_max = max(param_max,(int)param_value.back());
						}

						sprintf(draw_selection,"zp_pri/10<=%f && zp_pri/10>=%f && rrp_pri>=%f && rrp_pri<=%f",TPC.Get_LXe_maxZ(),TPC.Get_LXe_minZ(),TPC.Get_LXe_minRR(),TPC.Get_LXe_maxRR());
						file_input_tree->Draw(">>elist_LXe_gen",draw_selection,"goff");
						TEntryList *elist_LXe_gen = (TEntryList*)gDirectory->Get("elist_LXe_gen");

						file_outstat << "generated events (LXe): " << elist_LXe_gen->GetEntriesToProcess() << "\n";
						sprintf(draw_selection,"zp_pri/10<=%f && zp_pri/10>=%f && rrp_pri>=%f && rrp_pri<=%f && (nbpmthits > 0 || ntpmthits > 0)",TPC.Get_LXe_maxZ(),TPC.Get_LXe_minZ(),TPC.Get_LXe_minRR(),TPC.Get_LXe_maxRR());
						file_input_tree->Draw(">>e_list_LXe_det",draw_selection,"goff");
						TEntryList *elist_LXe_det = (TEntryList*)gDirectory->Get("e_list_LXe_det");

						file_outstat << "detected events (LXe): " << elist_LXe_det->GetEntriesToProcess() << "\n";
						file_outstat << "Mean LCE LXe(detected/generated): " << (double)elist_LXe_det->GetEntriesToProcess()/(double)elist_LXe_gen->GetEntriesToProcess()*100. << "\n";
						LCE_sum.push_back((double)elist_LXe_det->GetEntriesToProcess()/(double)elist_LXe_gen->GetEntriesToProcess()*100.);
						LCE_sum_err.push_back((1./(double)elist_LXe_gen->GetEntriesToProcess())*sqrt((double)elist_LXe_det->GetEntriesToProcess()*(1.-(double)elist_LXe_det->GetEntriesToProcess()/(double)elist_LXe_gen->GetEntriesToProcess()))*100.);

						sprintf(draw_selection,"zp_pri/10<=%f && zp_pri/10>=%f && rrp_pri>=%f && rrp_pri<=%f && (ntpmthits > 0)",TPC.Get_LXe_maxZ(),TPC.Get_LXe_minZ(),TPC.Get_LXe_minRR(),TPC.Get_LXe_maxRR());
						file_input_tree->Draw(">>e_list_LXe_det_top",draw_selection,"goff");
						TEntryList *elist_LXe_det_top = (TEntryList*)gDirectory->Get("e_list_LXe_det_top");
						file_outstat << "detected events (LXe TOP): " << elist_LXe_det_top->GetEntriesToProcess() << "\n";
						file_outstat << "Mean LCE LXe(detected TOP/generated): " << (double)elist_LXe_det_top->GetEntriesToProcess()/(double)elist_LXe_gen->GetEntriesToProcess()*100. << "\n";
						file_outstat << "AreaFractionTop LXe(detected TOP/detected): " << (double)elist_LXe_det_top->GetEntriesToProcess()/(double)elist_LXe_det->GetEntriesToProcess()*100. << "\n";
						LCE_top.push_back((double)elist_LXe_det_top->GetEntriesToProcess()/(double)elist_LXe_gen->GetEntriesToProcess()*100.);
						LCE_top_err.push_back((1./(double)elist_LXe_gen->GetEntriesToProcess())*sqrt((double)elist_LXe_det_top->GetEntriesToProcess()*(1.-(double)elist_LXe_det_top->GetEntriesToProcess()/(double)elist_LXe_gen->GetEntriesToProcess()))*100.);
						AFT.push_back((double)elist_LXe_det_top->GetEntriesToProcess()/(double)elist_LXe_det->GetEntriesToProcess()*100.);
						AFT_err.push_back((1./(double)elist_LXe_det->GetEntriesToProcess())*sqrt((double)elist_LXe_det_top->GetEntriesToProcess()*(1.-(double)elist_LXe_det_top->GetEntriesToProcess()/(double)elist_LXe_det->GetEntriesToProcess()))*100.);

						sprintf(draw_selection,"zp_pri/10<=%f && zp_pri/10>=%f && rrp_pri>=%f && rrp_pri<=%f && (nbpmthits > 0)",TPC.Get_LXe_maxZ(),TPC.Get_LXe_minZ(),TPC.Get_LXe_minRR(),TPC.Get_LXe_maxRR());
						file_input_tree->Draw(">>e_list_LXe_det_bottom",draw_selection,"goff");
						TEntryList *elist_LXe_det_bottom = (TEntryList*)gDirectory->Get("e_list_LXe_det_bottom");
						file_outstat << "detected events (LXe BOT): " << elist_LXe_det_bottom->GetEntriesToProcess() << "\n";
						file_outstat << "Mean LCE LXe(detected Bot/generated): " << (double)elist_LXe_det_bottom->GetEntriesToProcess()/(double)elist_LXe_gen->GetEntriesToProcess()*100. << "\n";
						file_outstat << "AreaFractionBot LXe(detected BOT/detected): " << (double)elist_LXe_det_bottom->GetEntriesToProcess()/(double)elist_LXe_det->GetEntriesToProcess()*100. << "\n";
						LCE_bottom.push_back((double)elist_LXe_det_bottom->GetEntriesToProcess()/(double)elist_LXe_gen->GetEntriesToProcess()*100.);
						LCE_bottom_err.push_back((1./(double)elist_LXe_gen->GetEntriesToProcess())*sqrt((double)elist_LXe_det_bottom->GetEntriesToProcess()*(1.-(double)elist_LXe_det_bottom->GetEntriesToProcess()/(double)elist_LXe_gen->GetEntriesToProcess()))*100.);

						/*=================================================================*/
						// generated events vs. Z
						/*=================================================================*/
						style_1D->cd();
						TH1F* h_LCEZ_gen = new TH1F(Form("LCEZ_gen_%d",(int)h_LCEZ_sum.size()), "MC: generated events vs. Z (All PMTs)", TPC.Get_nbinsZ(), TPC.Get_LXe_minZ(), TPC.Get_LXe_maxZ());
						sprintf(draw_selection,"zp_pri/10<=%f && zp_pri/10>=%f && rrp_pri>=%f && rrp_pri<=%f",TPC.Get_LXe_maxZ(),TPC.Get_LXe_minZ(),TPC.Get_LXe_minRR(),TPC.Get_LXe_maxRR());
						file_input_tree->Draw(Form("zp_pri/10. >> LCEZ_gen_%d",(int)h_LCEZ_sum.size()), draw_selection, "goff");
						
						/*=================================================================*/
						// detected events vs. Z (TOP PMTs)
						/*=================================================================*/
						style_1D->cd();
						TH1F* h_LCEZ_det_top = new TH1F(Form("LCEZ_det_top_%d",(int)h_LCEZ_sum.size()), "MC: detected events vs. Z (TOP PMTs)", TPC.Get_nbinsZ(), TPC.Get_LXe_minZ(), TPC.Get_LXe_maxZ());
						sprintf(draw_selection,"zp_pri/10<=%f && zp_pri/10>=%f && rrp_pri>=%f && rrp_pri<=%f && (ntpmthits > 0)",TPC.Get_LXe_maxZ(),TPC.Get_LXe_minZ(),TPC.Get_LXe_minRR(),TPC.Get_LXe_maxRR());
						file_input_tree->Draw(Form("zp_pri/10. >> LCEZ_det_top_%d",(int)h_LCEZ_sum.size()), draw_selection, "goff");
						
						/*=================================================================*/
						// detected events vs. Z (BOTTOM PMTs)
						/*=================================================================*/
						style_1D->cd();
						TH1F* h_LCEZ_det_bottom = new TH1F(Form("LCEZ_det_bottom_%d",(int)h_LCEZ_sum.size()), "MC: detected events vs. Z (BOTTOM PMTs)", TPC.Get_nbinsZ(), TPC.Get_LXe_minZ(), TPC.Get_LXe_maxZ());
						sprintf(draw_selection,"zp_pri/10<=%f && zp_pri/10>=%f && rrp_pri>=%f && rrp_pri<=%f && (nbpmthits > 0)",TPC.Get_LXe_maxZ(),TPC.Get_LXe_minZ(),TPC.Get_LXe_minRR(),TPC.Get_LXe_maxRR());
						file_input_tree->Draw(Form("zp_pri/10. >> LCEZ_det_bottom_%d",(int)h_LCEZ_sum.size()), draw_selection, "goff");
						
						/*=================================================================*/
						// LCE vs. Z
						/*=================================================================*/
						style_1D->cd();
						TH1F* h_LCEZ_sum_temp = new TH1F(Form("LCEZ_sum_%d",(int)h_LCEZ_sum.size()), "MC: LCE vs. Z (ALL PMTs)", TPC.Get_nbinsZ(), TPC.Get_LXe_minZ(), TPC.Get_LXe_maxZ());
						h_LCEZ_sum_temp->Sumw2();
						h_LCEZ_sum_temp->Add(h_LCEZ_det_top, 1.);
						h_LCEZ_sum_temp->Add(h_LCEZ_det_bottom, 1.);
						h_LCEZ_sum_temp->Divide(h_LCEZ_gen);
						h_LCEZ_sum_temp->Scale(100.);
						h_LCEZ_sum.push_back(h_LCEZ_sum_temp);
						
						/*=================================================================*/
						// LCE vs. Z (TOP PMTs)
						/*=================================================================*/
						style_1D->cd();
						TH1F* h_LCEZ_top_temp = new TH1F(Form("LCEZ_top_%d",(int)h_LCEZ_sum.size()), "MC: LCE vs. Z (TOP PMTs)", TPC.Get_nbinsZ(), TPC.Get_LXe_minZ(), TPC.Get_LXe_maxZ());
						h_LCEZ_top_temp->Sumw2();
						h_LCEZ_top_temp->Divide(h_LCEZ_det_top, h_LCEZ_gen, 1., 1., "b");
						h_LCEZ_top_temp->Scale(100.);
						h_LCEZ_top.push_back(h_LCEZ_top_temp);
						
						/*=================================================================*/
						// LCE vs. Z (BOTTOM PMTs)
						/*=================================================================*/
						style_1D->cd();
						TH1F* h_LCEZ_bottom_temp = new TH1F(Form("LCEZ_bottom_%d",(int)h_LCEZ_sum.size()), "MC: LCE vs. Z (BOTTOM PMTs)", TPC.Get_nbinsZ(), TPC.Get_LXe_minZ(), TPC.Get_LXe_maxZ());
						h_LCEZ_bottom_temp->Sumw2();
						h_LCEZ_bottom_temp->Divide(h_LCEZ_det_bottom, h_LCEZ_gen, 1., 1., "b");
						h_LCEZ_bottom_temp->Scale(100.);
						h_LCEZ_bottom.push_back(h_LCEZ_bottom_temp);
						
						/*=================================================================*/
						// AFT vs. Z
						/*=================================================================*/
						style_1D->cd();
						TH1F* h_AFTZ_temp = new TH1F(Form("AFTZ_%d",(int)h_LCEZ_sum.size()), "MC: AFT vs. Z", TPC.Get_nbinsZ(), TPC.Get_LXe_minZ(), TPC.Get_LXe_maxZ());
						h_AFTZ_temp->Sumw2();
						h_AFTZ_temp->Divide(h_LCEZ_top_temp, h_LCEZ_sum_temp, 1.,1., "b");
						h_AFTZ_temp->Scale(100.);
						h_AFTZ.push_back(h_AFTZ_temp);
						
					} else if (strcmp(token[1],"S2") == 0) {
						
						if ((parameter == "LXeTR") || (parameter=="LXeTeflonReflectivity")) {
							param_value_S2.push_back(atoi(token[2]));  
							g_TitleX="LXeTeflonReflectivity [%]";
						} else if ((parameter == "GXeTR") || (parameter=="GXeTeflonReflectivity")) {
							param_value_S2.push_back(atoi(token[3]));   
							g_TitleX="GXeTeflonReflectivity [%]";
						} else if ((parameter == "LXeAbsL") || (parameter=="LXeAbsorbtionLength")) {
							param_value_S2.push_back(atoi(token[4]));   
							g_TitleX="LXeAbsorbtionLength [cm]";
						} else if ((parameter == "GXeAbsL") || (parameter=="GXeAbsorbtionLength")) {
							param_value_S2.push_back(atoi(token[5]));  
							g_TitleX="GXeAbsorbtionLength [cm]";
						} else if ((parameter == "LXeRSL") || (parameter=="LXeRayScatterLength")) {
							param_value_S2.push_back(atoi(token[6]));  
							g_TitleX="LXeRayScatterLength [cm]";
						} else if ((parameter == "LXeRef") || (parameter=="LXeRefractionIndex")) {
							param_value_S2.push_back(atoi(token[7]));  
							g_TitleX="LXeRefractionIndex";
						} else {
							return;  
						}  
						
						file_outstat << "= file: " << input_file << " " << nevents << " events total." << " param: " << param_value_S2.back() << "\n";
						cout << "= file: " << input_file << " " << nevents << " events total." << " param: " << param_value_S2.back() << endl;
						
						//if (h_LCE_paramter->GetEntries() == 0) {
						if (LCE_sum.size() == 0) {
							param_min_S2 = param_value_S2.back();
							param_max_S2 = param_value_S2.back();
						}
						else {
							param_min_S2 = min(param_min_S2,(int)param_value_S2.back());
							param_max_S2 = max(param_max_S2,(int)param_value_S2.back());
						}
						
						file_input_tree->Draw(">>elist_top_S2","(ntpmthits > 0)","goff");
						TEntryList *elist_top_S2 = (TEntryList*)gDirectory->Get("elist_top_S2");

						file_input_tree->Draw(">>elist_bottom_S2","(nbpmthits > 0)","goff");
						TEntryList *elist_bottom_S2 = (TEntryList*)gDirectory->Get("elist_bottom_S2");
						
						file_outstat << "AreaFractionTop S2 (detected Top/detected): " << ((double)elist_top_S2->GetEntriesToProcess())/(((double)elist_bottom_S2->GetEntriesToProcess())+((double)elist_top_S2->GetEntriesToProcess()))*100 << "\n";
						AFT_S2.push_back(((double)elist_top_S2->GetEntriesToProcess())/(((double)elist_bottom_S2->GetEntriesToProcess())+((double)elist_top_S2->GetEntriesToProcess()))*100);
						AFT_S2_err.push_back((1./(((double)elist_bottom_S2->GetEntriesToProcess())+((double)elist_top_S2->GetEntriesToProcess())))*sqrt((double)elist_top_S2->GetEntriesToProcess()*(1.-(double)elist_top_S2->GetEntriesToProcess()/(((double)elist_bottom_S2->GetEntriesToProcess())+((double)elist_top_S2->GetEntriesToProcess()))))*100.);
						
					} else {
						cout << "Skip unusual file!" << endl; continue;
					}
						
					delete file_input_tree;
				}
			}
		}
		file_outstat << "============================================================" << "\n";
		cout << "============================================================" << endl;
		
		file_outplot->cd();
		
		gROOT->SetBatch(kTRUE);
		/*=================================================================*/
		if (!batch) {gROOT->SetBatch(kFALSE);}
		/*=================================================================*/
		
		TPaveText *pt_MCINFO = new TPaveText(0.735,0.775,0.97,0.975,"NDC");
		pt_MCINFO->SetFillColor(0);   
		pt_MCINFO->SetBorderSize(1);
		pt_MCINFO->SetTextFont(42);
		pt_MCINFO->SetTextAlign(12); 
		sprintf(canvasfile,"MC: %s v%s", G4MCname->GetTitle(), MCVERSION_TAG->GetTitle());
		pt_MCINFO->AddText(canvasfile);
		sprintf(canvasfile,"   optPhot source (178nm)");
		pt_MCINFO->AddText(canvasfile);
		sprintf(canvasfile,"   top PMTs QE: 100%%");
		pt_MCINFO->AddText(canvasfile);
		sprintf(canvasfile,"   bottom PMTs QE: 100%%");
		pt_MCINFO->AddText(canvasfile);

		
		if (param_value.size() != 0) {
			
			style_1D->cd();
			gStyle->SetPalette(NCont,ColPalette);
			TCanvas *c_AFT = new TCanvas("AFT","AFT",canvas_x,canvas_y);
			c_AFT->SetGrid();
			
			TGraph *g_AFT = new TGraphErrors((int)param_value.size(),&(param_value[0]),&(AFT[0]),0,&(AFT_err[0]));
			g_AFT->SetTitle("");
			g_AFT->GetXaxis()->SetTitle(g_TitleX.c_str());
			g_AFT->GetXaxis()->CenterTitle();
			g_AFT->GetXaxis()->SetRangeUser(param_min-5,param_max+5);
			g_AFT->GetYaxis()->SetTitle("AreaFractionTop S1 [%]");
			g_AFT->GetYaxis()->CenterTitle();
			//g_AFT->GetYaxis()->SetRangeUser(TPC.Get_LCE_min(),TPC.Get_LCE_max());
			g_AFT->SetLineColor(kRed);
			g_AFT->SetMarkerColor(kRed);
			g_AFT->Draw("AP");
			
			pt_MCINFO->Draw();
			
			if (file_outplot) c_AFT->Write();	
			sprintf(canvasfile,"%s/paramvar_%s_AFT.%s", output_dir.c_str(), parameter.c_str(),export_format.c_str());
			if (!(export_format=="")) c_AFT->SaveAs(canvasfile);

			style_1D->cd();
			TCanvas *c_AFTZ = new TCanvas("c_AFTZ","c_AFTZ",canvas_x,canvas_y);
			TLegend *leg_AFTZ = new TLegend(0.25,0.8,0.7,0.975);
			h_AFTZ.at(0)->SetTitle("");
			h_AFTZ.at(0)->SetXTitle("Z [cm]");
			h_AFTZ.at(0)->GetXaxis()->CenterTitle();
			h_AFTZ.at(0)->SetYTitle("AreaFractionTop S1 [%]");
			h_AFTZ.at(0)->GetYaxis()->CenterTitle();
			h_AFTZ.at(0)->GetYaxis()->SetRangeUser(0.,120.);
			h_AFTZ.at(0)->SetLineColor(kBlue);
			h_AFTZ.at(0)->SetLineStyle(1);
			h_AFTZ.at(0)->SetLineWidth(2);
			h_AFTZ.at(0)->SetMarkerColor(kBlue);
			h_AFTZ.at(0)->SetMarkerStyle(6);
			h_AFTZ.at(0)->Draw("E");
			leg_AFTZ->AddEntry(h_AFTZ.at(0),Form("%s: %d",g_TitleX.c_str(),(int)param_value.at(0)),"LP"); 
			if (h_AFTZ.size()>2) {
				h_AFTZ.at(ceil(h_AFTZ.size()/2.)-1)->SetLineColor(kRed);
				h_AFTZ.at(ceil(h_AFTZ.size()/2.)-1)->SetLineStyle(3);
				h_AFTZ.at(ceil(h_AFTZ.size()/2.)-1)->SetLineWidth(2);
				h_AFTZ.at(ceil(h_AFTZ.size()/2.)-1)->SetMarkerColor(kRed);
				h_AFTZ.at(ceil(h_AFTZ.size()/2.)-1)->SetMarkerStyle(28);
				h_AFTZ.at(ceil(h_AFTZ.size()/2.)-1)->SetMarkerSize(0.8);
				h_AFTZ.at(ceil(h_AFTZ.size()/2.)-1)->Draw("E same");
				leg_AFTZ->AddEntry(h_AFTZ.at(ceil(h_AFTZ.size()/2.)-1),Form("%s: %d",g_TitleX.c_str(),(int)param_value.at(ceil(h_AFTZ.size()/2.)-1)),"LP"); 
			}
			if (h_AFTZ.size()>1) {
				h_AFTZ.at(h_AFTZ.size()-1)->SetLineColor(kGreen);
				h_AFTZ.at(h_AFTZ.size()-1)->SetLineStyle(2);
				h_AFTZ.at(h_AFTZ.size()-1)->SetLineWidth(2);
				h_AFTZ.at(h_AFTZ.size()-1)->SetMarkerColor(kGreen);
				h_AFTZ.at(h_AFTZ.size()-1)->SetMarkerStyle(4);
				h_AFTZ.at(h_AFTZ.size()-1)->SetMarkerSize(0.8);
				h_AFTZ.at(h_AFTZ.size()-1)->Draw("E same");
				leg_AFTZ->AddEntry(h_AFTZ.at(h_AFTZ.size()-1),Form("%s: %d",g_TitleX.c_str(),(int)param_value.at(h_AFTZ.size()-1)),"LP");
			}
			pt_MCINFO->Draw();
			
			leg_AFTZ->SetFillColor(0);
			leg_AFTZ->SetBorderSize(1);
			//leg_AFTZ->SetTextSize(0.04);
			leg_AFTZ->SetTextFont(42);
			leg_AFTZ->SetTextAlign(12);         
			leg_AFTZ->Draw();   
			
			if (file_outplot) c_AFTZ->Write();	
			sprintf(canvasfile,"%s/paramvar_%s_AFTz.%s", output_dir.c_str(), parameter.c_str(),export_format.c_str());
			if (!(export_format=="")) c_AFTZ->SaveAs(canvasfile);
			
			style_1D->cd();
			TCanvas *c_LCE = new TCanvas("LCE","LCE",canvas_x,canvas_y);
			c_LCE->SetGrid();
			
			TMultiGraph *mg_LCE = new TMultiGraph();
			
			TGraph *g_LCE_sum = new TGraphErrors((int)param_value.size(),&(param_value[0]),&(LCE_sum[0]),0,&(LCE_sum_err[0]));
			g_LCE_sum->SetLineColor(kBlue);
			g_LCE_sum->SetMarkerColor(kBlue);
			mg_LCE->Add(g_LCE_sum);
			
			TGraph *g_LCE_top = new TGraphErrors((int)param_value.size(),&(param_value[0]),&(LCE_top[0]),0,&(LCE_top_err[0]));
			g_LCE_top->SetLineColor(kRed);
			g_LCE_top->SetMarkerColor(kRed);
			mg_LCE->Add(g_LCE_top);
			
			TGraph *g_LCE_bottom = new TGraphErrors((int)param_value.size(),&(param_value[0]),&(LCE_bottom[0]),0,&(LCE_bottom_err[0]));
			g_LCE_bottom->SetLineColor(kGreen);
			g_LCE_bottom->SetMarkerColor(kGreen);
			mg_LCE->Add(g_LCE_bottom);
			
			mg_LCE->Draw("AP");
			mg_LCE->SetTitle("");
			mg_LCE->GetXaxis()->SetTitle(g_TitleX.c_str());
			mg_LCE->GetXaxis()->CenterTitle();
			mg_LCE->GetXaxis()->SetRangeUser(param_min-5,param_max+5);
			mg_LCE->GetYaxis()->SetTitle("LightCollectionEfficiency S1 [%]");
			mg_LCE->GetYaxis()->CenterTitle();
			mg_LCE->GetYaxis()->SetRangeUser(TPC.Get_LCE_min(),TPC.Get_LCE_max()+10.);
			mg_LCE->Draw("AP");
			
			pt_MCINFO->Draw();
			
			TLegend *leg_LCE = new TLegend(0.4,0.8,0.7,0.975);
			leg_LCE->SetFillColor(0);
			leg_LCE->SetBorderSize(1);
			//leg_LCE->SetTextSize(0.04);
			leg_LCE->SetTextFont(42);
			leg_LCE->SetTextAlign(12);         
			leg_LCE->AddEntry(g_LCE_sum,"All PMTs","PE"); 
			leg_LCE->AddEntry(g_LCE_top,"Top PMTs","PE");
			leg_LCE->AddEntry(g_LCE_bottom,"Bottom PMTs","PE"); 
			leg_LCE->Draw();   
			
			if (file_outplot) c_LCE->Write();	
			sprintf(canvasfile,"%s/paramvar_%s_LCE.%s", output_dir.c_str(), parameter.c_str(),export_format.c_str());
			if (!(export_format=="")) c_LCE->SaveAs(canvasfile);
			
			style_1D->cd();
			TCanvas *c_LCEZ = new TCanvas("c_LCEZ","c_LCEZ",canvas_x,canvas_y);
			TLegend *leg_LCEZ = new TLegend(0.1,0.775,0.71,0.975);
			leg_LCEZ->SetNColumns(4);
			leg_LCEZ->AddEntry((TObject*)0,Form("%s: %d",g_TitleX.c_str(),(int)param_value.at(0)),"");
			h_LCEZ_sum.at(0)->SetTitle("");
			h_LCEZ_sum.at(0)->SetXTitle("Z [cm]");
			h_LCEZ_sum.at(0)->GetXaxis()->CenterTitle();
			h_LCEZ_sum.at(0)->SetYTitle("LightCollectionEfficiency S1 [%]");
			h_LCEZ_sum.at(0)->GetYaxis()->CenterTitle();
			h_LCEZ_sum.at(0)->GetYaxis()->SetRangeUser(TPC.Get_LCE_min(),TPC.Get_LCE_max()+15.);
			h_LCEZ_sum.at(0)->SetLineColor(kBlue);
			h_LCEZ_sum.at(0)->SetLineStyle(1);
			h_LCEZ_sum.at(0)->SetLineWidth(2);
			h_LCEZ_sum.at(0)->SetMarkerColor(kBlue);
			h_LCEZ_sum.at(0)->SetMarkerStyle(6);
			h_LCEZ_sum.at(0)->Draw("E");
			leg_LCEZ->AddEntry(h_LCEZ_sum.at(0),"sum","LP"); 
			h_LCEZ_top.at(0)->SetLineColor(kRed);
			h_LCEZ_top.at(0)->SetLineStyle(1);
			h_LCEZ_top.at(0)->SetLineWidth(2);
			h_LCEZ_top.at(0)->SetMarkerColor(kRed);
			h_LCEZ_top.at(0)->SetMarkerStyle(6);
			h_LCEZ_top.at(0)->Draw("E same");
			leg_LCEZ->AddEntry(h_LCEZ_top.at(0),"top","LP"); 
			h_LCEZ_bottom.at(0)->SetLineColor(kGreen);
			h_LCEZ_bottom.at(0)->SetLineStyle(1);
			h_LCEZ_bottom.at(0)->SetLineWidth(2);
			h_LCEZ_bottom.at(0)->SetMarkerColor(kGreen);
			h_LCEZ_bottom.at(0)->SetMarkerStyle(6);
			h_LCEZ_bottom.at(0)->Draw("E same");
			leg_LCEZ->AddEntry(h_LCEZ_bottom.at(0),"bottom","LP"); 
			if (h_LCEZ_sum.size()>2) {
				leg_LCEZ->AddEntry((TObject*)0,Form("%s: %d",g_TitleX.c_str(),(int)param_value.at(ceil(h_LCEZ_sum.size()/2.)-1)),"");
				h_LCEZ_sum.at(ceil(h_LCEZ_sum.size()/2.)-1)->SetLineColor(kBlue);
				h_LCEZ_sum.at(ceil(h_LCEZ_sum.size()/2.)-1)->SetLineStyle(3);
				h_LCEZ_sum.at(ceil(h_LCEZ_sum.size()/2.)-1)->SetLineWidth(2);
				h_LCEZ_sum.at(ceil(h_LCEZ_sum.size()/2.)-1)->SetMarkerColor(kBlue);
				h_LCEZ_sum.at(ceil(h_LCEZ_sum.size()/2.)-1)->SetMarkerStyle(28);
				h_LCEZ_sum.at(ceil(h_LCEZ_sum.size()/2.)-1)->SetMarkerSize(0.8);
				h_LCEZ_sum.at(ceil(h_LCEZ_sum.size()/2.)-1)->Draw("E same");
				leg_LCEZ->AddEntry(h_LCEZ_sum.at(ceil(h_LCEZ_sum.size()/2.)-1),"sum","LP"); 
				h_LCEZ_top.at(ceil(h_LCEZ_sum.size()/2.)-1)->SetLineColor(kRed);
				h_LCEZ_top.at(ceil(h_LCEZ_sum.size()/2.)-1)->SetLineStyle(3);
				h_LCEZ_top.at(ceil(h_LCEZ_sum.size()/2.)-1)->SetLineWidth(2);
				h_LCEZ_top.at(ceil(h_LCEZ_sum.size()/2.)-1)->SetMarkerColor(kRed);
				h_LCEZ_top.at(ceil(h_LCEZ_sum.size()/2.)-1)->SetMarkerStyle(28);
				h_LCEZ_top.at(ceil(h_LCEZ_sum.size()/2.)-1)->SetMarkerSize(0.8);
				h_LCEZ_top.at(ceil(h_LCEZ_sum.size()/2.)-1)->Draw("E same");
				leg_LCEZ->AddEntry(h_LCEZ_top.at(ceil(h_LCEZ_sum.size()/2.)-1),"top","LP"); 
				h_LCEZ_bottom.at(ceil(h_LCEZ_sum.size()/2.)-1)->SetLineColor(kGreen);
				h_LCEZ_bottom.at(ceil(h_LCEZ_sum.size()/2.)-1)->SetLineStyle(3);
				h_LCEZ_bottom.at(ceil(h_LCEZ_sum.size()/2.)-1)->SetLineWidth(2);
				h_LCEZ_bottom.at(ceil(h_LCEZ_sum.size()/2.)-1)->SetMarkerColor(kGreen);
				h_LCEZ_bottom.at(ceil(h_LCEZ_sum.size()/2.)-1)->SetMarkerStyle(28);
				h_LCEZ_bottom.at(ceil(h_LCEZ_sum.size()/2.)-1)->SetMarkerSize(0.8);
				h_LCEZ_bottom.at(ceil(h_LCEZ_sum.size()/2.)-1)->Draw("E same");
				leg_LCEZ->AddEntry(h_LCEZ_bottom.at(ceil(h_LCEZ_sum.size()/2.)-1),"bottom","LP"); 
			}
			if (h_LCEZ_sum.size()>1) {
				leg_LCEZ->AddEntry((TObject*)0,Form("%s: %d",g_TitleX.c_str(),(int)param_value.at(h_LCEZ_sum.size()-1)),"");
				h_LCEZ_sum.at(h_LCEZ_sum.size()-1)->SetLineColor(kBlue);
				h_LCEZ_sum.at(h_LCEZ_sum.size()-1)->SetLineStyle(2);
				h_LCEZ_sum.at(h_LCEZ_sum.size()-1)->SetLineWidth(2);
				h_LCEZ_sum.at(h_LCEZ_sum.size()-1)->SetMarkerColor(kBlue);
				h_LCEZ_sum.at(h_LCEZ_sum.size()-1)->SetMarkerStyle(4);
				h_LCEZ_sum.at(h_LCEZ_sum.size()-1)->SetMarkerSize(0.8);
				h_LCEZ_sum.at(h_LCEZ_sum.size()-1)->Draw("E same");
				leg_LCEZ->AddEntry(h_LCEZ_sum.at(h_LCEZ_sum.size()-1),"sum","LP"); 
				h_LCEZ_top.at(h_LCEZ_top.size()-1)->SetLineColor(kRed);
				h_LCEZ_top.at(h_LCEZ_top.size()-1)->SetLineStyle(2);
				h_LCEZ_top.at(h_LCEZ_top.size()-1)->SetLineWidth(2);
				h_LCEZ_top.at(h_LCEZ_top.size()-1)->SetMarkerColor(kRed);
				h_LCEZ_top.at(h_LCEZ_top.size()-1)->SetMarkerStyle(4);
				h_LCEZ_top.at(h_LCEZ_top.size()-1)->SetMarkerSize(0.8);
				h_LCEZ_top.at(h_LCEZ_top.size()-1)->Draw("E same");
				leg_LCEZ->AddEntry(h_LCEZ_top.at(h_LCEZ_top.size()-1),"top","LP"); 
				h_LCEZ_bottom.at(h_LCEZ_bottom.size()-1)->SetLineColor(kGreen);
				h_LCEZ_bottom.at(h_LCEZ_bottom.size()-1)->SetLineStyle(2);
				h_LCEZ_bottom.at(h_LCEZ_bottom.size()-1)->SetLineWidth(2);
				h_LCEZ_bottom.at(h_LCEZ_bottom.size()-1)->SetMarkerColor(kGreen);
				h_LCEZ_bottom.at(h_LCEZ_bottom.size()-1)->SetMarkerStyle(4);
				h_LCEZ_bottom.at(h_LCEZ_bottom.size()-1)->SetMarkerSize(0.8);
				h_LCEZ_bottom.at(h_LCEZ_bottom.size()-1)->Draw("E same");
				leg_LCEZ->AddEntry(h_LCEZ_bottom.at(h_LCEZ_bottom.size()-1),"bottom","LP"); 
			}
			pt_MCINFO->Draw();
			
			leg_LCEZ->SetFillColor(0);
			leg_LCEZ->SetBorderSize(1);
			//leg_LCEZ->SetTextSize(0.04);
			leg_LCEZ->SetTextFont(42);
			leg_LCEZ->SetTextAlign(12);         
			leg_LCEZ->Draw();   
			
			if (file_outplot) c_LCEZ->Write();	
			sprintf(canvasfile,"%s/paramvar_%s_LCEz.%s", output_dir.c_str(), parameter.c_str(),export_format.c_str());
			if (!(export_format=="")) c_LCEZ->SaveAs(canvasfile);
			
			style_1D->cd();
			TCanvas *c_rLCEZ = new TCanvas("c_rLCEZ","c_rLCEZ",canvas_x,canvas_y);
			TLegend *leg_rLCEZ = new TLegend(0.1,0.775,0.71,0.975);
			leg_rLCEZ->SetNColumns(4);
			leg_rLCEZ->AddEntry((TObject*)0,Form("%s: %d",g_TitleX.c_str(),(int)param_value.at(0)),"");
			double h_rLCEZ_mean_1 = 0;
			for (int z=0; z<(TPC.Get_nbinsZ()); z++){
				h_rLCEZ_mean_1 += h_LCEZ_sum.at(0)->GetBinContent(z+1)/(TPC.Get_nbinsZ());
			}
			TH1F* h_rLCEZ_sum = (TH1F*)h_LCEZ_sum.at(0)->Clone();
			h_rLCEZ_sum->Scale(1./h_rLCEZ_mean_1);
			h_rLCEZ_sum->SetYTitle("relative LightCollectionEfficiency S1");
			h_rLCEZ_sum->GetYaxis()->SetRangeUser(0.,2.5);
			h_rLCEZ_sum->Draw("E");
			leg_rLCEZ->AddEntry(h_rLCEZ_sum,"sum","LP"); 
			h_LCEZ_top.at(0)->Scale(1./h_rLCEZ_mean_1);
			h_LCEZ_top.at(0)->Draw("E same");
			leg_rLCEZ->AddEntry(h_LCEZ_top.at(0),"top","LP"); 
			h_LCEZ_bottom.at(0)->Scale(1./h_rLCEZ_mean_1);
			h_LCEZ_bottom.at(0)->Draw("E same");
			leg_rLCEZ->AddEntry(h_LCEZ_bottom.at(0),"bottom","LP"); 
			if (h_LCEZ_sum.size()>2) {
				leg_rLCEZ->AddEntry((TObject*)0,Form("%s: %d",g_TitleX.c_str(),(int)param_value.at(ceil(h_LCEZ_sum.size()/2.)-1)),"");
				double h_rLCEZ_mean_2 = 0;
				for (int z=0; z<(TPC.Get_nbinsZ()); z++){
					h_rLCEZ_mean_2 += h_LCEZ_sum.at(ceil(h_LCEZ_sum.size()/2.)-1)->GetBinContent(z+1)/(TPC.Get_nbinsZ());
				}
				h_LCEZ_sum.at(ceil(h_LCEZ_sum.size()/2.)-1)->Scale(1./h_rLCEZ_mean_2);
				h_LCEZ_sum.at(ceil(h_LCEZ_sum.size()/2.)-1)->Draw("E same");
				leg_rLCEZ->AddEntry(h_LCEZ_sum.at(ceil(h_LCEZ_sum.size()/2.)-1),"sum","LP"); 
				h_LCEZ_top.at(ceil(h_LCEZ_sum.size()/2.)-1)->Scale(1./h_rLCEZ_mean_2);
				h_LCEZ_top.at(ceil(h_LCEZ_sum.size()/2.)-1)->Draw("E same");
				leg_rLCEZ->AddEntry(h_LCEZ_top.at(ceil(h_LCEZ_sum.size()/2.)-1),"top","LP"); 
				h_LCEZ_bottom.at(ceil(h_LCEZ_sum.size()/2.)-1)->Scale(1./h_rLCEZ_mean_2);
				h_LCEZ_bottom.at(ceil(h_LCEZ_sum.size()/2.)-1)->Draw("E same");
				leg_rLCEZ->AddEntry(h_LCEZ_bottom.at(ceil(h_LCEZ_sum.size()/2.)-1),"bottom","LP"); 
			}
			if (h_LCEZ_sum.size()>1) {
				leg_rLCEZ->AddEntry((TObject*)0,Form("%s: %d",g_TitleX.c_str(),(int)param_value.at(h_LCEZ_sum.size()-1)),"");
				double h_rLCEZ_mean_3 = 0;
				for (int z=0; z<(TPC.Get_nbinsZ()); z++){
					h_rLCEZ_mean_3 += h_LCEZ_sum.at(h_LCEZ_sum.size()-1)->GetBinContent(z+1)/(TPC.Get_nbinsZ());
				}
				h_LCEZ_sum.at(h_LCEZ_sum.size()-1)->Scale(1./h_rLCEZ_mean_3);
				h_LCEZ_sum.at(h_LCEZ_sum.size()-1)->Draw("E same");
				leg_rLCEZ->AddEntry(h_LCEZ_sum.at(h_LCEZ_sum.size()-1),"sum","LP"); 
				h_LCEZ_top.at(h_LCEZ_sum.size()-1)->Scale(1./h_rLCEZ_mean_3);
				h_LCEZ_top.at(h_LCEZ_top.size()-1)->Draw("E same");
				leg_rLCEZ->AddEntry(h_LCEZ_top.at(h_LCEZ_top.size()-1),"top","LP"); 
				h_LCEZ_bottom.at(h_LCEZ_sum.size()-1)->Scale(1./h_rLCEZ_mean_3);
				h_LCEZ_bottom.at(h_LCEZ_bottom.size()-1)->Draw("E same");
				leg_rLCEZ->AddEntry(h_LCEZ_bottom.at(h_LCEZ_bottom.size()-1),"bottom","LP"); 
			}
			pt_MCINFO->Draw();
			
			leg_rLCEZ->SetFillColor(0);
			leg_rLCEZ->SetBorderSize(1);
			//leg_rLCEZ->SetTextSize(0.04);
			leg_rLCEZ->SetTextFont(42);
			leg_rLCEZ->SetTextAlign(12);         
			leg_rLCEZ->Draw();   
			
			if (file_outplot) c_rLCEZ->Write();	
			sprintf(canvasfile,"%s/paramvar_%s_rLCEz.%s", output_dir.c_str(), parameter.c_str(),export_format.c_str());
			if (!(export_format=="")) c_rLCEZ->SaveAs(canvasfile);
		}
		
		if (param_value_S2.size() != 0) {
			style_1D->cd();
			gStyle->SetPalette(NCont,ColPalette);
			TCanvas *c_AFT_S2 = new TCanvas("c_AFT_S2","c_AFT_S2",canvas_x,canvas_y);
			c_AFT_S2->SetGrid();
			
			TGraph *g_AFT_S2 = new TGraphErrors((int)param_value_S2.size(),&(param_value_S2[0]),&(AFT_S2[0]),0,&(AFT_S2_err[0]));
			g_AFT_S2->SetTitle("");
			g_AFT_S2->GetXaxis()->SetTitle(g_TitleX.c_str());
			g_AFT_S2->GetXaxis()->CenterTitle();
			g_AFT_S2->GetXaxis()->SetRangeUser(param_min_S2-5,param_max_S2+5);
			g_AFT_S2->GetYaxis()->SetTitle("AreaFractionTop S2 [%]");
			g_AFT_S2->GetYaxis()->CenterTitle();
			//g_AFT_S2->GetYaxis()->SetRangeUser(TPC.Get_LCE_min(),TPC.Get_LCE_max());
			g_AFT_S2->SetLineColor(kRed);
			g_AFT_S2->SetMarkerColor(kRed);
			g_AFT_S2->Draw("AP");
			
			pt_MCINFO->Draw();
			
			if (file_outplot) c_AFT_S2->Write();	
			sprintf(canvasfile,"%s/paramvar_%s_AFT_S2.%s", output_dir.c_str(), parameter.c_str(),export_format.c_str());
			if (!(export_format=="")) c_AFT_S2->SaveAs(canvasfile);
		}
		
		param_value.clear();
		LCE_sum.clear();
		LCE_sum_err.clear();
		LCE_top.clear();
		LCE_top_err.clear();
		LCE_bottom.clear();
		LCE_bottom_err.clear();
		AFT.clear();
		AFT_err.clear();
		
		param_value_S2.clear();
		AFT_S2.clear();
		AFT_S2_err.clear();
		
		h_LCEZ_sum.clear();
		h_LCEZ_top.clear();
		h_LCEZ_bottom.clear();
		h_AFTZ.clear();
	}
	else {
		cout << endl;
		cout << "x Error xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx" << endl;
		cout << "You have to specify a directory:" << endl;
		cout << "-> " << datadir << endl;
		cout << "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx" << endl;
		cout << endl;
		gApplication->Terminate();
	}
	
	file_outstat.close();
	gROOT->SetBatch(kFALSE);	
	if (batch) {file_outplot->Close();}  
}