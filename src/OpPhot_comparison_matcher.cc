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

using namespace std;

/*=================================================================*/

void OpPhot_comparison_matcher(string datafile_kr, string datafile_mc, string suffix) {
	
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
		cout << "directory not found:" << endl;
		cout << "-> " << datafile_mc << endl;
		cout << "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx" << endl;
		cout << endl;
		gApplication->Terminate();
	}
	
	TPC_Definition TPC;
	//TPC.Set_Bins(9,4,4);
	TPC.Set_Bins(49,1,1);
	//float nbinst[4] = {4.,6.,8.,12.}; // number of Bins in theta
	float nbinst[4] = {1.,1.,1.,1.};
	string bin_format = "big";
	double AFT_S2_Kr = 0.645;
	
	const Int_t canvas_x = 650;
	const Int_t canvas_y = 800;
	char draw_selection[10000];
	
	// read in datafilename and get working directory
	size_t found=datafile_mc.find_last_of("/\\");
	string workingdirectory = datafile_mc.substr(0,found);
	string datafilename = datafile_mc.substr(found+1);
	
	char file_outname[10000];
	sprintf(file_outname,"%s/%s_matcher.dat", workingdirectory.c_str(),suffix.c_str());
	
	ofstream file_outstat;
	file_outstat.open(file_outname);
	file_outstat.precision(6);
	file_outstat.setf(ios::fixed);
	file_outstat.setf(ios::showpoint);
	// VERSIONTAG_SIGNTYPE_LXeTR_GXeTR_LXeAbsL_GXeAbsL_LXeRSL_LXeRef_NUMBER
	file_outstat << "#" << "VERSIONTAG" << " " << "SIGNTYPE" << " " << "LXeTR" << " " << "GXeTR" << " " << "LXeAbsL" << " " << "GXeAbsL" << " " << "LXeRSL" << " " << "LXeRef" << " " << "#" << " " << "SOS_rLCE_S1" << " " << "SOS_AFT_S1" << " " << "SOS_ly_S1" << " " << "SOS_AFT_S2" << " " << "SOS_sum" << "\n";
	
	/*=================================================================*/
	/*=================================================================*/
	// Read in real data
	/*=================================================================*/
	/*=================================================================*/
	
	// Read raw file and determine data structure
	ifstream raw;
	string linebuffer;
	
	const char* const DELIMITER = " ";
	const char* const DELIMITER_ = "._";
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
	
	/*=================================================================*/
	// generate relative LCE vs. Z
	/*=================================================================*/
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
	TH1F* h_AFTZ_Kr = new TH1F("AFTZ_Kr", "^{83m}Kr: AFT vs. Z", TPC.Get_nbinsZ(), TPC.Get_LXe_minZ(), TPC.Get_LXe_maxZ());
	h_AFTZ_Kr->Sumw2();
	h_AFTZ_Kr->Divide(h_Kr_LCE_LCEZ_top, h_Kr_LCE_LCEZ, 1.,1., "b");
	h_AFTZ_Kr->Scale(100.);
	
	/*=================================================================*/
	/*=================================================================*/
	// Read in MC data
	/*=================================================================*/
	/*=================================================================*/
	
	// read in datafilename and get working directory
	found=datafile_mc.find_last_of("/\\");
	workingdirectory = datafile_mc.substr(0,found);
	datafilename = datafile_mc.substr(found+1);
	
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
		
		/*=================================================================*/
		// generated events vs. Z
		/*=================================================================*/
		TH1F* h_LCEZ_gen = new TH1F("LCEZ_gen", "MC: generated events vs. Z (All PMTs)", TPC.Get_nbinsZ(), TPC.Get_LXe_minZ(), TPC.Get_LXe_maxZ());
		/*=================================================================*/
		// detected events vs. Z (TOP PMTs)
		/*=================================================================*/
		TH1F* h_LCEZ_det_top = new TH1F("LCEZ_det_top", "MC: detected events vs. Z (TOP PMTs)", TPC.Get_nbinsZ(), TPC.Get_LXe_minZ(), TPC.Get_LXe_maxZ());
		/*=================================================================*/
		// detected events vs. Z (BOTTOM PMTs)
		/*=================================================================*/
		TH1F* h_LCEZ_det_bottom = new TH1F("LCEZ_det_bottom", "MC: detected events vs. Z (BOTTOM PMTs)", TPC.Get_nbinsZ(), TPC.Get_LXe_minZ(), TPC.Get_LXe_maxZ());
		/*=================================================================*/
		// LCE vs. Z
		/*=================================================================*/
		TH1F* h_LCE_LCEZ = new TH1F("LCE_LCEZ", "MC: LCE vs. Z (ALL PMTs)", TPC.Get_nbinsZ(), TPC.Get_LXe_minZ(), TPC.Get_LXe_maxZ());
		h_LCE_LCEZ->Sumw2();
		/*=================================================================*/
		// LCE vs. Z (TOP PMTs)
		/*=================================================================*/
		TH1F* h_LCE_LCEZ_top = new TH1F("LCE_LCEZ_top", "MC: LCE vs. Z (TOP PMTs)", TPC.Get_nbinsZ(), TPC.Get_LXe_minZ(), TPC.Get_LXe_maxZ());
		h_LCE_LCEZ_top->Sumw2();
		/*=================================================================*/
		// LCE vs. Z (BOTTOM PMTs)
		/*=================================================================*/
		TH1F* h_LCE_LCEZ_bottom = new TH1F("LCE_LCEZ_bottom", "MC: LCE vs. Z (BOTTOM PMTs)", TPC.Get_nbinsZ(), TPC.Get_LXe_minZ(), TPC.Get_LXe_maxZ());
		h_LCE_LCEZ_bottom->Sumw2();
		/*=================================================================*/
		// ly vs. Z
		/*=================================================================*/
		TH1F* h_ly_lyZ = new TH1F("ly_lyZ", "MC: ly vs. Z (ALL PMTs)", TPC.Get_nbinsZ(), TPC.Get_LXe_minZ(), TPC.Get_LXe_maxZ());
		h_ly_lyZ->Sumw2();
		/*=================================================================*/
		// ly vs. Z (TOP PMTs)
		/*=================================================================*/
		TH1F* h_ly_lyZ_top = new TH1F("ly_lyZ_top", "MC: ly vs. Z (TOP PMTs)", TPC.Get_nbinsZ(), TPC.Get_LXe_minZ(), TPC.Get_LXe_maxZ());
		h_ly_lyZ_top->Sumw2();
		/*=================================================================*/
		// ly vs. Z (BOTTOM PMTs)
		/*=================================================================*/
		TH1F* h_ly_lyZ_bottom = new TH1F("ly_lyZ_bottom", "MC: ly vs. Z (BOTTOM PMTs)", TPC.Get_nbinsZ(), TPC.Get_LXe_minZ(), TPC.Get_LXe_maxZ());
		h_ly_lyZ_bottom->Sumw2();
		/*=================================================================*/
		// AFT vs. Z
		/*=================================================================*/
		TH1F* h_AFTZ_MC = new TH1F("AFTZ_MC", "MC: AFT vs. Z", TPC.Get_nbinsZ(), TPC.Get_LXe_minZ(), TPC.Get_LXe_maxZ());
		h_AFTZ_MC->Sumw2();
		/*=================================================================*/
		// ratio AFT vs. Z
		/*=================================================================*/
		TH1F* h_ratio_AFTZ = new TH1F("ratio_AFTZ", "ratio_AFTZ", TPC.Get_nbinsZ(), TPC.Get_LXe_minZ(), TPC.Get_LXe_maxZ());
		h_ratio_AFTZ->Sumw2();
		/*=================================================================*/
		// ratio rLCE
		/*=================================================================*/
		TH1F* h_ratio_rLCE = new TH1F("ratio_rLCE", "ratio_rLCE", TPC.Get_nbinsZ(), TPC.Get_LXe_minZ(), TPC.Get_LXe_maxZ());
		h_ratio_rLCE->Sumw2();
		/*=================================================================*/
		// ratio rLCE TOP PMTs
		/*=================================================================*/
		TH1F* h_ratio_rLCE_top = new TH1F("ratio_rLCE_top", "ratio_rLCE_top", TPC.Get_nbinsZ(), TPC.Get_LXe_minZ(), TPC.Get_LXe_maxZ());
		h_ratio_rLCE_top->Sumw2();
		/*=================================================================*/
		// ratio rLCE BOTTOM PMTs
		/*=================================================================*/
		TH1F* h_ratio_rLCE_bottom = new TH1F("ratio_rLCE_bottom", "ratio_rLCE_bottom", TPC.Get_nbinsZ(), TPC.Get_LXe_minZ(), TPC.Get_LXe_maxZ());
		h_ratio_rLCE_bottom->Sumw2();
		/*=================================================================*/
		// ratio ly
		/*=================================================================*/
		TH1F* h_ratio_ly = new TH1F("ratio_ly", "ratio_ly", TPC.Get_nbinsZ(), TPC.Get_LXe_minZ(), TPC.Get_LXe_maxZ());
		h_ratio_ly->Sumw2();
		/*=================================================================*/
		// ratio ly TOP PMTs
		/*=================================================================*/
		TH1F* h_ratio_ly_top = new TH1F("ratio_ly_top", "ratio_ly_top", TPC.Get_nbinsZ(), TPC.Get_LXe_minZ(), TPC.Get_LXe_maxZ());
		h_ratio_ly_top->Sumw2();
		/*=================================================================*/
		// ratio ly BOTTOM PMTs
		/*=================================================================*/
		TH1F* h_ratio_ly_bottom = new TH1F("ratio_ly_bottom", "ratio_ly_bottom", TPC.Get_nbinsZ(), TPC.Get_LXe_minZ(), TPC.Get_LXe_maxZ());
		h_ratio_ly_bottom->Sumw2();
		
		string ext = ".root";
		TSystemDirectory dir(workingdirectory.c_str(), workingdirectory.c_str());
		TList *files = dir.GetListOfFiles();
		if (files) {
			TSystemFile *file;
			TString fname;
			TIter next(files);
			int filenumber = 0;
			double SOS_sum = 0;
			char SOS_sum_min_filename[10000];
			double SOS_sum_min = 0; //sum of squares
			double SOS_sum_min_rLCE = 0;
			double SOS_sum_min_ly = 0;
			double SOS_sum_min_AFT_S1 = 0;
			double SOS_sum_min_AFT_S2 = 0;
			
			double max_param = 0;
			while ((file=(TSystemFile*)next())) {
				fname = file->GetName();
				if (!file->IsDirectory() && fname.EndsWith(ext.c_str()) && !(fname.Contains(suffix.c_str())) && !(fname.Contains("_S2_"))) {
					char filename[10000];
					sprintf(filename,"%s/%s", workingdirectory.c_str(), fname.Data());
					
					TFile *f = new TFile(filename,"READ");
					if ( (f->GetListOfKeys()->Contains("events")) && !(f->GetListOfKeys()->Contains("MC_TAG")) ) {
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
					
					filenumber++;
					// read in parameter value
					// e.g.: rev338_S1_90_90_5000_30_30_163_01 -> VERSIONTAG_SIGNTYPE_LXeTR_GXeTR_LXeAbsL_GXeAbsL_LXeRSL_LXeRef_NUMBER
					char* buf = strdup(fname.Data());
					token[0] = strtok(buf, DELIMITER_); // first token
					for (int n = 1; n < 10; n++) {
						token[n] = strtok(0, DELIMITER_); // subsequent tokens
						if (!token[n]) break; // no more tokens
					}
					
					/*=================================================================*/
					// FILTER PARAMETER SETTINGS
					/*=================================================================*/
					//if (atoi(token[5])!=10000) {cout << "Skip!" << endl; continue;}
					
					// Search for S2 sim
					char filename_S2[10000];
					sprintf(filename_S2,"%s/%s_S2_%s_%s_%s_%s_%s_%s_%s.root", workingdirectory.c_str(), token[0], token[2], token[3], token[4], token[5], token[6], token[7], token[8]);
					TChain *file_input_tree_S2 = new TChain("events/events");
					file_input_tree_S2->AddFile(filename_S2); 
					const int nevents_S2 = file_input_tree_S2->GetEntries();
					double AFT_S2_ratio = 0;
					double AFT_S2 = 0;
					//double AFT_S2_Kr = 0.64; // defined above!
					if (nevents_S2 > 0) {
						cout << " file(" << filenumber << "): " << token[0] << "_S2_" << token[2] << "_" << token[3] << "_" << token[4] << "_" << token[5] << "_" << token[6] << "_" << token[7] << "_" << token[8] << ".root" << " " << nevents_S2 << " events total." <<  endl;
						
						file_input_tree_S2->Draw(">>elist_top_S2","(ntpmthits > 0)","goff");
						TEntryList *elist_top_S2 = (TEntryList*)gDirectory->Get("elist_top_S2");

						file_input_tree_S2->Draw(">>elist_bottom_S2","(nbpmthits > 0)","goff");
						TEntryList *elist_bottom_S2 = (TEntryList*)gDirectory->Get("elist_bottom_S2");
						
						AFT_S2 = ((double)elist_top_S2->GetEntriesToProcess()*TPC.Get_QE_top())/(((double)elist_bottom_S2->GetEntriesToProcess()*TPC.Get_QE_bottom())+((double)elist_top_S2->GetEntriesToProcess()*TPC.Get_QE_top()));
						AFT_S2_ratio = abs(AFT_S2_Kr - AFT_S2);
					}
					else {
						cout << " Skip file(" << filenumber << "): " << token[0] << "_S1_" << token[2] << "_" << token[3] << "_" << token[4] << "_" << token[5] << "_" << token[6] << "_" << token[7] << "_" << token[8] << ".root" << " " << "no S2 found." <<  endl;
						delete file_input_tree_S2;
						continue;
					}
					delete file_input_tree_S2;
					
					TChain *file_input_tree = new TChain("events/events");
					file_input_tree->AddFile(filename); 
					const int nevents = file_input_tree->GetEntries();
					
					cout << " file(" << filenumber << "): " << token[0] << "_S1_" << token[2] << "_" << token[3] << "_" << token[4] << "_" << token[5] << "_" << token[6] << "_" << token[7] << "_" << token[8] << ".root" << " " << nevents << " events total." <<  endl;
					
					file_input_tree->SetAlias("rrp_pri","(xp_pri*xp_pri + yp_pri*yp_pri)/10./10.");  
					/*=================================================================*/
					// generated events vs. Z
					/*=================================================================*/
					h_LCEZ_gen->Reset();
					sprintf(draw_selection,"zp_pri/10<=%f && zp_pri/10>=%f && rrp_pri>=%f && rrp_pri<=%f",TPC.Get_LXe_maxZ(),TPC.Get_LXe_minZ(),TPC.Get_LXe_minRR(),TPC.Get_LXe_maxRR());
					file_input_tree->Draw("zp_pri/10. >> LCEZ_gen", draw_selection, "goff");
					/*=================================================================*/
					// detected events vs. Z (TOP PMTs)
					/*=================================================================*/
					h_LCEZ_det_top->Reset();
					sprintf(draw_selection,"zp_pri/10<=%f && zp_pri/10>=%f && rrp_pri>=%f && rrp_pri<=%f && (ntpmthits > 0)",TPC.Get_LXe_maxZ(),TPC.Get_LXe_minZ(),TPC.Get_LXe_minRR(),TPC.Get_LXe_maxRR());
					file_input_tree->Draw("zp_pri/10. >> LCEZ_det_top", draw_selection, "goff");
					/*=================================================================*/
					// detected events vs. Z (BOTTOM PMTs)
					/*=================================================================*/
					h_LCEZ_det_bottom->Reset();
					sprintf(draw_selection,"zp_pri/10<=%f && zp_pri/10>=%f && rrp_pri>=%f && rrp_pri<=%f && (nbpmthits > 0)",TPC.Get_LXe_maxZ(),TPC.Get_LXe_minZ(),TPC.Get_LXe_minRR(),TPC.Get_LXe_maxRR());
					file_input_tree->Draw("zp_pri/10. >> LCEZ_det_bottom", draw_selection, "goff");
					/*=================================================================*/
					// LCE vs. Z (TOP PMTs)
					/*=================================================================*/
					h_LCE_LCEZ_top->Reset();
					h_LCE_LCEZ_top->Divide(h_LCEZ_det_top, h_LCEZ_gen, TPC.Get_QE_top(), 1., "b");
					h_LCE_LCEZ_top->Scale(100.);
					/*=================================================================*/
					// LCE vs. Z (BOTTOM PMTs)
					/*=================================================================*/
					h_LCE_LCEZ_bottom->Reset();
					h_LCE_LCEZ_bottom->Divide(h_LCEZ_det_bottom, h_LCEZ_gen, TPC.Get_QE_bottom(), 1., "b");
					h_LCE_LCEZ_bottom->Scale(100.);
					/*=================================================================*/
					// LCE vs. Z
					/*=================================================================*/
					h_LCE_LCEZ->Reset();
					h_LCE_LCEZ->Add(h_LCE_LCEZ_top, 1.);
					h_LCE_LCEZ->Add(h_LCE_LCEZ_bottom, 1.);
					/*=================================================================*/
					// ly vs. Z (TOP PMTs)
					/*=================================================================*/
					h_ly_lyZ_top->Reset();
					h_ly_lyZ_top->Divide(h_LCEZ_det_top, h_LCEZ_gen, TPC.Get_QE_top(), 1., "b");
					h_ly_lyZ_top->Scale(50.);
					/*=================================================================*/
					// ly vs. Z (BOTTOM PMTs)
					/*=================================================================*/
					h_ly_lyZ_bottom->Reset();
					h_ly_lyZ_bottom->Divide(h_LCEZ_det_bottom, h_LCEZ_gen, TPC.Get_QE_bottom(), 1., "b");
					h_ly_lyZ_bottom->Scale(50.);
					/*=================================================================*/
					// ly vs. Z
					/*=================================================================*/
					h_ly_lyZ->Reset();
					h_ly_lyZ->Add(h_ly_lyZ_top, 1.);
					h_ly_lyZ->Add(h_ly_lyZ_bottom, 1.);
					/*=================================================================*/
					// AFT vs. Z
					/*=================================================================*/
					h_AFTZ_MC->Reset();
					h_AFTZ_MC->Divide(h_LCE_LCEZ_top, h_LCE_LCEZ, 1.,1., "b");
					h_AFTZ_MC->Scale(100.);
					/*=================================================================*/
					// ratio AFT vs. Z
					/*=================================================================*/
					h_ratio_AFTZ->Reset();
					/*=================================================================*/
					// ratio rLCE
					/*=================================================================*/
					h_ratio_rLCE->Reset();
					/*=================================================================*/
					// ratio rLCE TOP PMTs
					/*=================================================================*/
					h_ratio_rLCE_top->Reset();
					/*=================================================================*/
					// ratio rLCE BOTTOM PMTs
					/*=================================================================*/
					h_ratio_rLCE_bottom->Reset();
					/*=================================================================*/
					// ratio ly
					/*=================================================================*/
					h_ratio_ly->Reset();
					/*=================================================================*/
					// ratio ly TOP PMTs
					/*=================================================================*/
					h_ratio_ly_top->Reset();
					/*=================================================================*/
					// ratio ly BOTTOM PMTs
					/*=================================================================*/
					h_ratio_ly_bottom->Reset();
					/*=================================================================*/
					// comparison relative LCE vs. Z
					/*=================================================================*/
					double h_rLCE_mean = 0;
					for (int z=0; z<(TPC.Get_nbinsZ()); z++){
						h_rLCE_mean += h_LCE_LCEZ->GetBinContent(z+1)/(TPC.Get_nbinsZ());
					}
					h_LCE_LCEZ->Scale(1./h_rLCE_mean);
					h_LCE_LCEZ_bottom->Scale(1./h_rLCE_mean);
					h_LCE_LCEZ_top->Scale(1./h_rLCE_mean);
					
					h_ratio_rLCE->Add(h_rLCE_LCEZ, 1.);
					h_ratio_rLCE->Add(h_LCE_LCEZ, -1.);

					h_ratio_rLCE_top->Add(h_rLCE_LCEZ_top, 1.);
					h_ratio_rLCE_top->Add(h_LCE_LCEZ_top, -1.);

					h_ratio_rLCE_bottom->Add(h_rLCE_LCEZ_bottom, 1.);
					h_ratio_rLCE_bottom->Add(h_LCE_LCEZ_bottom, -1.);
					
					double h_ratio_rLCE_sos_all = 0; //sum of squares
					double h_ratio_rLCE_md_all = 0; //maximum deviation
					double h_ratio_rLCE_sos_top = 0; //sum of squares
					double h_ratio_rLCE_md_top = 0; //maximum deviation
					double h_ratio_rLCE_sos_bottom = 0; //sum of squares
					double h_ratio_rLCE_md_bottom = 0; //maximum deviation
					for (int z=0; z<TPC.Get_nbinsZ(); z++){
						h_ratio_rLCE_sos_all += h_ratio_rLCE->GetBinContent(z)*h_ratio_rLCE->GetBinContent(z);
						if (abs(h_ratio_rLCE->GetBinContent(z)) > h_ratio_rLCE_md_all) {h_ratio_rLCE_md_all = abs(h_ratio_rLCE->GetBinContent(z));}
						h_ratio_rLCE_sos_top += h_ratio_rLCE_top->GetBinContent(z)*h_ratio_rLCE_top->GetBinContent(z);
						if (abs(h_ratio_rLCE_top->GetBinContent(z)) > h_ratio_rLCE_md_top) {h_ratio_rLCE_md_top = abs(h_ratio_rLCE_top->GetBinContent(z));}
						h_ratio_rLCE_sos_bottom += h_ratio_rLCE_bottom->GetBinContent(z)*h_ratio_rLCE_bottom->GetBinContent(z);
						if (abs(h_ratio_rLCE_bottom->GetBinContent(z)) > h_ratio_rLCE_md_bottom) {h_ratio_rLCE_md_bottom = abs(h_ratio_rLCE_bottom->GetBinContent(z));}
					}
					/*=================================================================*/
					// comparison ly vs. Z
					/*=================================================================*/
					h_ratio_ly->Add(h_Kr_LCE_LCEZ, 1.);
					h_ratio_ly->Add(h_ly_lyZ, -1.);

					h_ratio_ly_top->Add(h_Kr_LCE_LCEZ_top, 1.);
					h_ratio_ly_top->Add(h_ly_lyZ_top, -1.);

					h_ratio_ly_bottom->Add(h_Kr_LCE_LCEZ_bottom, 1.);
					h_ratio_ly_bottom->Add(h_ly_lyZ_bottom, -1.);
					
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
					/*=================================================================*/
					// comparison AFT vs. Z
					/*=================================================================*/
					h_ratio_AFTZ->Add(h_AFTZ_Kr, 1.);
					h_ratio_AFTZ->Add(h_AFTZ_MC, -1.);
					
					double h_ratio_AFTZ_sos = 0; //sum of squares
					double h_ratio_AFTZ_md = 0; //maximum deviation
					for (int z=0; z<TPC.Get_nbinsZ(); z++){
						h_ratio_AFTZ_sos += h_ratio_AFTZ->GetBinContent(z)*h_ratio_AFTZ->GetBinContent(z);
						if (abs(h_ratio_AFTZ->GetBinContent(z)) > h_ratio_AFTZ_md) {h_ratio_AFTZ_md = abs(h_ratio_AFTZ->GetBinContent(z));}
					}
					
					/*=================================================================*/
					/*=================================================================*/
					// S1 + S2
					SOS_sum = (h_ratio_rLCE_sos_all/(TPC.Get_nbinsZ()*0.05))+(h_ratio_ly_sos_all/(TPC.Get_nbinsZ()*0.2))+(h_ratio_AFTZ_sos/(TPC.Get_nbinsZ()*2.))+(AFT_S2_ratio/(1.*0.05));
					
					// S1
					//SOS_sum = (h_ratio_rLCE_sos_all/(TPC.Get_nbinsZ()*0.05))+(h_ratio_ly_sos_all/(TPC.Get_nbinsZ()*0.2))+(h_ratio_AFTZ_sos/(TPC.Get_nbinsZ()*2.));
					
					if (filenumber == 1) {
						strcpy(SOS_sum_min_filename,filename);
						SOS_sum_min_rLCE = h_ratio_rLCE_sos_all;
						SOS_sum_min_ly = h_ratio_ly_sos_all;
						SOS_sum_min_AFT_S1 = h_ratio_AFTZ_sos;
						SOS_sum_min_AFT_S2 = AFT_S2_ratio;
						SOS_sum_min = SOS_sum;
					}
					else {
						if (SOS_sum < SOS_sum_min) {
							strcpy(SOS_sum_min_filename,filename);
							SOS_sum_min_rLCE = h_ratio_rLCE_sos_all;
							SOS_sum_min_ly = h_ratio_ly_sos_all;
							SOS_sum_min_AFT_S1 = h_ratio_AFTZ_sos;
							SOS_sum_min_AFT_S2 = AFT_S2_ratio;
							SOS_sum_min = SOS_sum;
						}
					}

					file_outstat << token[0] << "\t" << token[1] << "\t" << token[2] << "\t" << token[3] << "\t" << token[4] << "\t" << token[5] << "\t" << token[6] << "\t" << token[7] << "\t" << token[8] << "\t" << h_ratio_rLCE_sos_all << "\t" << h_ratio_AFTZ_sos << "\t" << h_ratio_ly_sos_all << "\t" << AFT_S2_ratio << "\t" << SOS_sum << "\n";
					
					delete file_input_tree;
				}
			}
			cout << "------------------------------------------------------------" << endl;
			cout << "Best match file:" << SOS_sum_min_filename << endl;
			cout << "Minimum sos sum    of " << SOS_sum_min << endl;
			cout << "Minimum rLCE  sos  of " << SOS_sum_min_rLCE << endl;
			cout << "Minimum S1AFT sos  of " << SOS_sum_min_AFT_S1 << endl;
			cout << "Minimum ly    sos  of " << SOS_sum_min_ly << endl;
			cout << "Minimum S2AFT diff of " << SOS_sum_min_AFT_S2 << endl;
			cout << "------------------------------------------------------------" << endl;
			cout << "sort with: LC_ALL=C sort -k14 -g OpPhotStudy_matcher.dat -o OpPhotStudy_matcher_sort.dat" << endl;
			//gROOT->ProcessLine("LC_ALL=C sort -k14 -g OpPhotStudy_matcher.dat -o OpPhotStudy_matcher_sort.dat"); 
		}		
	}
	else {
		cout << endl;
		cout << "x Error xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx" << endl;
		cout << "You have to specify a directory:" << endl;
		cout << "-> " << datafile_mc << endl;
		cout << "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx" << endl;
		cout << endl;
		gApplication->Terminate();
	}
	cout << "============================================================" << endl;
	
}