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
#include <time.h>
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

void optPhot_matching(string datafile_kr, double AFT_S2_Kr, string datafile_mc, int bin_z, int bin_r, int bin_rr, string strnbinst, int filenumber_start, int filenumber_end, bool batch);
void optPhot_matching(string datafile_kr, string datafile_PMT, double AFT_S2_Kr, string datafile_mc, int bin_z, int bin_r, int bin_rr, string strnbinst, int filenumber_start, int filenumber_end, bool batch);

/*=================================================================*/

void optPhot_matching(string datafile_kr, double AFT_S2_Kr, string datafile_mc, int bin_z, int bin_r, int bin_rr, string strnbinst, int filenumber_start = 0, int filenumber_end = 0, bool batch = true) {
	optPhot_matching(datafile_kr,"",AFT_S2_Kr,datafile_mc,bin_z,bin_r,bin_rr,strnbinst,filenumber_start,filenumber_end,batch);
}

/*=================================================================*/
void optPhot_matching(string datafile_kr, string datafile_PMT, double AFT_S2_Kr, string datafile_mc, int bin_z, int bin_r, int bin_rr, string strnbinst, int filenumber_start = 0, int filenumber_end = 0, bool batch = true) {
	
	//gErrorIgnoreLevel = kPrint, kInfo, kWarning, kError, kBreak, kSysError, kFatal;
	gErrorIgnoreLevel = kPrint;
	
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
		cout << "directory not found:" << endl;
		cout << "-> " << datafile_mc << endl;
		cout << "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx" << endl;
		cout << endl;
		gApplication->Terminate();
	}
	
	if ( (filenumber_start > filenumber_end) ) {
		cout << endl;
		cout << "x Error xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx" << endl;
		cout << "file index start is greater then file index end!" << endl;
		cout << "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx" << endl;
		cout << endl;
		gApplication->Terminate();
	}
	
	TPC_Definition TPC;
	TPC.Set_Bins(bin_z,bin_r,bin_rr);
	
	time_t     now = time(0);
    struct tm  tstruct;
    char       starttime[80];
    tstruct = *localtime(&now);
    strftime(starttime, sizeof(starttime), "%X", &tstruct);

	stringstream stream(strnbinst);
	std::vector<int> nbinst;
	int element;
	while(stream >> element){
		nbinst.push_back(element);
	}
	
	const Int_t canvas_x = 650;
	const Int_t canvas_y = 800;
	char draw_selection[10000];
	
	// read in datafilename and get working directory
	size_t found=datafile_mc.find_last_of("/\\");
	string workingdirectory = datafile_mc.substr(0,found);
	string datafilename = datafile_mc.substr(found+1);
	found=workingdirectory.find_last_of("/\\");
	string workingdirectory_topname = workingdirectory.substr(found+1);
	
	found=datafile_kr.find_last_of("/\\");
	string datafilename_kr = datafile_kr.substr(found+1);
	size_t lastindex = datafilename_kr.find_last_of("."); 
	string rawdatafilename_kr = datafilename_kr.substr(0, lastindex); 
	
	if ( (filenumber_start == 0) && (filenumber_start != filenumber_end) ) {filenumber_start = 1;}
	
	char file_outname[10000];
	if (filenumber_start != filenumber_end) {
		sprintf(file_outname,"%s/matching_%d_to_%d_%s_vs_%s.dat", workingdirectory.c_str(), filenumber_start, filenumber_end, workingdirectory_topname.c_str(), rawdatafilename_kr.c_str());
	} else {
		sprintf(file_outname,"%s/matching_%s_vs_%s.dat", workingdirectory.c_str(), workingdirectory_topname.c_str(), rawdatafilename_kr.c_str());
	}
	
	ofstream file_outstat;
	file_outstat.open(file_outname);
	file_outstat.precision(6);
	file_outstat.setf(ios::fixed);
	file_outstat.setf(ios::showpoint);
	// VERSIONTAG_SIGNTYPE_LXeTR_GXeTR_LXeAbsL_GXeAbsL_LXeRSL_LXeRef_NUMBER
	file_outstat << "#" << "VTag" << "\t" << "STyp" << "\t" << "LXeTR" << "\t" << "GXeTR" << "\t" 
				 << "LXeAbsL" << "\t" << "GXeAbsL" << "\t" << "LXeRSL" << "\t" << "LXeRef" << "\t" << "#" 
				 << "\t" << "RMSD_rLCE_S1" << "\t" << "RMSD_AFT_S1" << "\t" << "RMSD_ly_S1" << "\t" 
				 << "RMSD_AFT_S2" << "\t" << "RMSD_sum" << "\t" << "SOS_sum" << "\n";
	
	/*=================================================================*/
	/*=================================================================*/
	// Read in real data
	/*=================================================================*/
	/*=================================================================*/
	
	// Read raw file and determine data structure
	ifstream raw;
	string linebuffer;
	
	const char* const DELIMITER = " ";
	const char* const DELIMITER_ = "_";
	const char* const DELIMITER_p = "._";
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
	
	cout << "============================================================" << endl;
	cout << "= reading PMT.ini ==========================================" << endl;
	
	vector<double> QE_PMT;
	vector<double> On_PMT;
	
	if ((datafile_PMT == "")) {
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
		if (filenumber_start != filenumber_end) {
			cout << "= from file " << filenumber_start << " to file " << filenumber_end << endl;
		} else {
			cout << "= all files in directory" << endl;
		}
		
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
		
		TH2F* check_pmt_details = new TH2F("check_pmt_details", "check_pmt_details", TPC.Get_nbinsRR(), TPC.Get_LXe_minRR(), TPC.Get_LXe_maxRR(), TPC.Get_nbinsZ(), TPC.Get_LXe_minZ(), TPC.Get_LXe_maxZ());
		
		string ext = ".root";
		int files_to_process = 0;
		
		TSystemDirectory test_dir(workingdirectory.c_str(), workingdirectory.c_str());
		TList *test_files = test_dir.GetListOfFiles();
		if (test_files) {
			TSystemFile *test_file;
			TString test_fname;
			TIter test_next(test_files);
			while ((test_file=(TSystemFile*)test_next())) {
					test_fname = test_file->GetName();
					if (!test_file->IsDirectory() && test_fname.EndsWith(ext.c_str()) && !(test_fname.Contains("comparison_")) && !(test_fname.Contains("_S2_"))) {
						files_to_process += 1;
					}
			}
			
		}
		
		if ( (filenumber_start < files_to_process) && (filenumber_end <= files_to_process) && (filenumber_start != filenumber_end) ) {
			files_to_process = filenumber_end - filenumber_start + 1;
		}
		
		cout << "= " << files_to_process << " files to process" << endl;
		cout << "============================================================" << endl;
		
		TSystemDirectory dir(workingdirectory.c_str(), workingdirectory.c_str());
		TList *files = dir.GetListOfFiles();
		if (files) {
			TSystemFile *file;
			TString fname;
			TIter next(files);
			int filenumber = 0;
			double SOS_sum = 0;
			double RMSD_sum = 0;
			
			char filename[10000];	
			char filename_S2[10000];			
			double max_param = 0;
			char* buf = 0;
			long nevents = 0;
			long nevents_S2 = 0;
			bool no_PMT_details = false;
			double AFT_S2_ratio = 0;
			double AFT_S2 = 0;
			
			long nbentries = 0;
			Int_t ntpmthits = 0;
			Int_t nbpmthits = 0;
			vector<int> *pmthitID = 0;
			Float_t xp_pri = 0;
			Float_t yp_pri = 0;
			Float_t zp_pri = 0;
			Float_t rrp_pri = 0;
			double S2_hits_top = 0;
			double S2_hits_bottom = 0;
			
			double h_rLCE_mean = 0;
			double h_ratio_rLCE_sos_all = 0; //sum of squares
			double h_ratio_rLCE_md_all = 0; //maximum deviation
			double h_ratio_rLCE_sos_top = 0; //sum of squares
			double h_ratio_rLCE_md_top = 0; //maximum deviation
			double h_ratio_rLCE_sos_bottom = 0; //sum of squares
			double h_ratio_rLCE_md_bottom = 0; //maximum deviation
			
			double h_ratio_ly_sos_all = 0; //sum of squares
			double h_ratio_ly_md_all = 0; //maximum deviation
			double h_ratio_ly_sos_top = 0; //sum of squares
			double h_ratio_ly_md_top = 0; //maximum deviation
			double h_ratio_ly_sos_bottom = 0; //sum of squares
			double h_ratio_ly_md_bottom = 0; //maximum deviation
			
			double h_ratio_AFTZ_sos = 0; //sum of squares
			double h_ratio_AFTZ_md = 0; //maximum deviation
			
			while ((file=(TSystemFile*)next())) {
				fname = file->GetName();
				if (!file->IsDirectory() && fname.EndsWith(ext.c_str()) && !(fname.Contains("comparison_")) && !(fname.Contains("_S2_"))) {
					
					filenumber++;
					if ( ((filenumber < filenumber_start) || (filenumber > filenumber_end)) && (filenumber_start != filenumber_end) ) {continue;}
					
					now = time(0);
					char currtime[80];
					tstruct = *localtime(&now);
					strftime(currtime, sizeof(currtime), "%X", &tstruct);
					
					sprintf(filename,"%s/%s", workingdirectory.c_str(), fname.Data());
					
					TFile *f_S1 = new TFile(filename,"READ");
					TNamed *G4MCname;
					if (f_S1->GetListOfKeys()->Contains("MC_TAG")) {
						f_S1->GetObject("MC_TAG",G4MCname);
					}
					else {
						G4MCname = new TNamed("MC_TAG","Xenon1t");
					}
					if ( (strcmp(G4MCname->GetTitle(),"Xenon1t") == 0) && (!(f_S1->IsZombie())) ) {
						f_S1->Close();
						delete f_S1;
					}
					else {
						cout << endl;
						cout << "x Error xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx" << endl;
						cout << "File format not known (or Zombie file):" << endl;
						cout << "-> " << filename << endl;
						cout << "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx" << endl;
						cout << endl;
						gApplication->Terminate();
					}
					
					// read in parameter value
					// e.g.: rev338_S1_90_90_5000_30_30_163_01 -> VERSIONTAG_SIGNTYPE_LXeTR_GXeTR_LXeAbsL_GXeAbsL_LXeRSL_LXeRef_NUMBER
					buf = strdup(fname.Data());
					token[0] = strtok(buf, DELIMITER_); // first token
					for (int n = 1; n < 10; n++) {
						if (n > 2) {
							token[n] = strtok(0, DELIMITER_p); // subsequent tokens
						} else {
							token[n] = strtok(0, DELIMITER_); // subsequent tokens
						}
						if (!token[n]) break; // no more tokens
					}
					
					TChain *file_input_tree = new TChain("events/events");
					file_input_tree->AddFile(filename); 
					nevents = file_input_tree->GetEntries();
					
					file_input_tree->SetAlias("rrp_pri","(xp_pri*xp_pri + yp_pri*yp_pri)/10./10.");  
					
					check_pmt_details->Reset();
					file_input_tree->Draw("zp_pri/10. : rrp_pri >> check_pmt_details","pmthitID[0]>0","goff");
					no_PMT_details = (bool)(check_pmt_details->GetEntries() == 0);
					
					/*=================================================================*/
					// FILTER PARAMETER SETTINGS
					/*=================================================================*/
					//if (atoi(token[5])!=10000) {cout << "Skip!" << endl; continue;}
					
					// Search for S2 sim
					
					sprintf(filename_S2,"%s/%s_S2_%s_%s_%s_%s_%s_%s_%s.root", workingdirectory.c_str(), token[0], token[2], token[3], token[4], token[5], token[6], token[7], token[8]);
					TChain *file_input_tree_S2 = new TChain("events/events");
					file_input_tree_S2->AddFile(filename_S2); 
					nevents_S2 = file_input_tree_S2->GetEntries();
					
					TFile *f_S2 = new TFile(filename_S2,"READ");
					
					S2_hits_top = 0;
					S2_hits_bottom = 0;
					
					if ( (nevents_S2 > 0) && (!(f_S2->IsZombie())) ) {
						cout << " file(" << filenumber << "): " << token[0] << "_S2_" << token[2] << "_" << token[3] << "_" << token[4] << "_" << token[5] << "_" << token[6] << "_" << token[7] << "_" << token[8] << ".root" << " " << nevents_S2 << " events total.\t" << (double)(filenumber-filenumber_start+1)/(double)files_to_process*100 << "% - " << currtime << " (since " << starttime << ")" <<   endl;
						
						nbentries = nevents_S2;
						
						file_input_tree_S2->SetBranchAddress("ntpmthits", &ntpmthits);
						file_input_tree_S2->SetBranchAddress("nbpmthits", &nbpmthits);
						file_input_tree_S2->SetBranchAddress("pmthitID", &pmthitID);
						file_input_tree_S2->SetBranchAddress("xp_pri", &xp_pri);
						file_input_tree_S2->SetBranchAddress("yp_pri", &yp_pri);
						file_input_tree_S2->SetBranchAddress("zp_pri", &zp_pri);
						
						for (long i=0; i<nbentries; i++) {
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
									S2_hits_top += ntpmthits*TPC.Get_QE_top();
									S2_hits_bottom += nbpmthits*TPC.Get_QE_bottom();
								} else { 
									S2_hits_top += ntpmthits*QE_PMT[(*pmthitID)[0]]*On_PMT[(*pmthitID)[0]];
									S2_hits_bottom += nbpmthits*QE_PMT[(*pmthitID)[0]]*On_PMT[(*pmthitID)[0]];
								}
							}
						}
						AFT_S2 = (S2_hits_top)/(S2_hits_bottom+S2_hits_top);
						AFT_S2_ratio = abs(AFT_S2_Kr - AFT_S2);
					}
					else {
						cout << " Skip file(" << filenumber << "): " << token[0] << "_S1_" << token[2] << "_" << token[3] << "_" << token[4] << "_" << token[5] << "_" << token[6] << "_" << token[7] << "_" << token[8] << ".root" << " " << "no S2 found." <<  endl;
						delete file_input_tree_S2;
						continue;
					}
					f_S2->Close();
					delete f_S2;
					delete file_input_tree_S2;
					
					cout << " file(" << filenumber << "): " << token[0] << "_S1_" << token[2] << "_" << token[3] << "_" << token[4] << "_" << token[5] << "_" << token[6] << "_" << token[7] << "_" << token[8] << ".root" << " " << nevents << " events total.\t" << (double)(filenumber-filenumber_start+1)/(double)files_to_process*100 << "% - " << currtime << " (since " << starttime << ")" <<  endl;
					
					nbentries = file_input_tree->GetEntries();
	
					file_input_tree->SetBranchAddress("ntpmthits", &ntpmthits);
					file_input_tree->SetBranchAddress("nbpmthits", &nbpmthits);
					file_input_tree->SetBranchAddress("pmthitID", &pmthitID);
					file_input_tree->SetBranchAddress("xp_pri", &xp_pri);
					file_input_tree->SetBranchAddress("yp_pri", &yp_pri);
					file_input_tree->SetBranchAddress("zp_pri", &zp_pri);
					
					h_LCEZ_gen->Reset();
					h_LCEZ_det_top->Reset();
					h_LCEZ_det_bottom->Reset();
					
					for (long i=0; i<nbentries; i++){
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
						} else {
							if (no_PMT_details) {
								h_LCEZ_det_top->Fill(zp_pri/10., ntpmthits*TPC.Get_QE_top());
								h_LCEZ_det_bottom->Fill(zp_pri/10., nbpmthits*TPC.Get_QE_bottom());
							} else { 
								h_LCEZ_det_top->Fill(zp_pri/10., ntpmthits*QE_PMT[(*pmthitID)[0]]*On_PMT[(*pmthitID)[0]]);
								h_LCEZ_det_bottom->Fill(zp_pri/10., nbpmthits*QE_PMT[(*pmthitID)[0]]*On_PMT[(*pmthitID)[0]]);
							}
						}
						// All events
						h_LCEZ_gen->Fill(zp_pri/10., 1.);
					}
					
					/*=================================================================*/
					// LCE vs. Z (TOP PMTs)
					/*=================================================================*/
					h_LCE_LCEZ_top->Reset();
					h_LCE_LCEZ_top->Divide(h_LCEZ_det_top, h_LCEZ_gen, 1., 1., "b");
					h_LCE_LCEZ_top->Scale(100.);
					/*=================================================================*/
					// LCE vs. Z (BOTTOM PMTs)
					/*=================================================================*/
					h_LCE_LCEZ_bottom->Reset();
					h_LCE_LCEZ_bottom->Divide(h_LCEZ_det_bottom, h_LCEZ_gen, 1., 1., "b");
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
					h_ly_lyZ_top->Divide(h_LCEZ_det_top, h_LCEZ_gen, 1., 1., "b");
					h_ly_lyZ_top->Scale(50.);
					/*=================================================================*/
					// ly vs. Z (BOTTOM PMTs)
					/*=================================================================*/
					h_ly_lyZ_bottom->Reset();
					h_ly_lyZ_bottom->Divide(h_LCEZ_det_bottom, h_LCEZ_gen, 1., 1., "b");
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
					h_rLCE_mean = 0;
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
					
					h_ratio_rLCE_sos_all = 0; //sum of squares
					h_ratio_rLCE_md_all = 0; //maximum deviation
					h_ratio_rLCE_sos_top = 0; //sum of squares
					h_ratio_rLCE_md_top = 0; //maximum deviation
					h_ratio_rLCE_sos_bottom = 0; //sum of squares
					h_ratio_rLCE_md_bottom = 0; //maximum deviation
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
					
					h_ratio_ly_sos_all = 0; //sum of squares
					h_ratio_ly_md_all = 0; //maximum deviation
					h_ratio_ly_sos_top = 0; //sum of squares
					h_ratio_ly_md_top = 0; //maximum deviation
					h_ratio_ly_sos_bottom = 0; //sum of squares
					h_ratio_ly_md_bottom = 0; //maximum deviation
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
					
					h_ratio_AFTZ_sos = 0; //sum of squares
					h_ratio_AFTZ_md = 0; //maximum deviation
					for (int z=0; z<TPC.Get_nbinsZ(); z++){
						h_ratio_AFTZ_sos += h_ratio_AFTZ->GetBinContent(z)*h_ratio_AFTZ->GetBinContent(z);
						if (abs(h_ratio_AFTZ->GetBinContent(z)) > h_ratio_AFTZ_md) {h_ratio_AFTZ_md = abs(h_ratio_AFTZ->GetBinContent(z));}
					}
					
					/*=================================================================*/
					/*=================================================================*/
					// S1 + S2
					SOS_sum = (h_ratio_rLCE_sos_all/(TPC.Get_nbinsZ()*0.05))+(h_ratio_ly_sos_all/(TPC.Get_nbinsZ()*0.2))+(h_ratio_AFTZ_sos/(TPC.Get_nbinsZ()*2.))+(AFT_S2_ratio/(1.*0.05));
					RMSD_sum = sqrt(h_ratio_rLCE_sos_all/(double)TPC.Get_nbinsZ()) + sqrt(h_ratio_AFTZ_sos/(double)TPC.Get_nbinsZ()) + sqrt(h_ratio_ly_sos_all/(double)TPC.Get_nbinsZ()) + sqrt(AFT_S2_ratio*AFT_S2_ratio);

					// S1
					//SOS_sum = (h_ratio_rLCE_sos_all/(TPC.Get_nbinsZ()*0.05))+(h_ratio_ly_sos_all/(TPC.Get_nbinsZ()*0.2))+(h_ratio_AFTZ_sos/(TPC.Get_nbinsZ()*2.));

					file_outstat << token[0] << "\t" << token[1] << "\t" << token[2] << "\t" << token[3] << "\t" << token[4] << "\t" << token[5] << "\t" << token[6] << "\t" << token[7] << "\t" << token[8] << "\t" << sqrt(h_ratio_rLCE_sos_all/(double)TPC.Get_nbinsZ()) << "\t" << sqrt(h_ratio_AFTZ_sos/(double)TPC.Get_nbinsZ()) << "\t" << sqrt(h_ratio_ly_sos_all/(double)TPC.Get_nbinsZ()) << "\t" << sqrt(AFT_S2_ratio*AFT_S2_ratio) << "\t" << RMSD_sum << "\t" << SOS_sum << "\n";
					
					delete file_input_tree;
				}
			}
			cout << "------------------------------------------------------------" << endl;
			cout << "sort with: LC_ALL=C sort -k14 -g matching_*.dat -o matching_sorted.dat" << endl;
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