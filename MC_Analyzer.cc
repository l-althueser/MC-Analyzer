/******************************************************************
 * MC Analyzer for S1 analysis
 * 
 * @author	Lutz Althueser
 *
 ******************************************************************/
 // include C++ libs
#include <vector>
#include <iostream>
#include <fstream>
#include <stdio.h>
#include <string>
#include <sstream>
#include <algorithm>

#include <stdlib.h>

#include "TError.h"

using namespace std;

// include own functions
#include "src/optPhot_S1.cc"
#include "src/optPhot_S2.cc"
#include "src/data_maps.cc"
#include "src/optPhot_comparison.cc"
#include "src/optPhot_matching.cc"
#include "src/optPhot_parameter_variations.cc"
#include "src/calibration_source.cc"

/*=================================================================*/

void MC_Analyzer() {
	
	//gErrorIgnoreLevel = kPrint, kInfo, kWarning, kError, kBreak, kSysError, kFatal;
	gErrorIgnoreLevel = kWarning;
	
	std::ifstream configfile_in("./.MC_Analyzer.config");
	string lastfile_MC, lastfile_Kr83m, lastfile_parameters, lastfile_pmtini;
	
	if(configfile_in) {
		configfile_in >> lastfile_MC;
		configfile_in >> lastfile_Kr83m;
		configfile_in >> lastfile_parameters;
		configfile_in >> lastfile_pmtini;
	}
	
	cout << "============================================================" << endl;
	cout << "============================================================" << endl;
	cout << "= Starting MC-Analyzer =====================================" << endl;
	cout << "============================================================" << endl;
	cout << "============================================================" << endl;
	cout << "= (1) S1 - MC analysis =====================================" << endl;
	cout << "============================================================" << endl;
	cout << "= (2) data maps (Kr83m) ====================================" << endl;
	cout << "============================================================" << endl;
	cout << "= (3) S1 - comparison of MC vs. data maps ==================" << endl;
	cout << "= (4) S1 - parameter variations (of dir) ===================" << endl;
	cout << "============================================================" << endl;
	cout << "= (5) calibration source ===================================" << endl;
	cout << "============================================================" << endl;
	cout << "= (6) matching prepare MC values ===========================" << endl;
	cout << "= (7) matching process =====================================" << endl;
	cout << "============================================================" << endl;
	cout << "= (8) S2 - MC analysis =====================================" << endl;
	cout << "============================================================" << endl;
	
	string cinput;
	cin >> cinput;
	cin.ignore();
	
	if (( cinput == "1" ) || ( cinput == "MC" )){
		// execute MC analysis
		cout << "= S1 - MC analysis =========================================" << endl;
		cout << "============================================================" << endl;
		cout << "Which datafile(s) do you want to analyse?" << endl;
		cout << "------------------------------------------------------------" << endl;
		cout << "(l) for: " << lastfile_MC << endl;
		string datafile;
		cin >> datafile;
		cin.ignore();
		
		if (datafile=="l") {
			datafile = lastfile_MC;
		}
		else {
			lastfile_MC = datafile;
		}
		
		//optPhot_S1(datafile,26,50,22,"png");
		optPhot_S1(datafile,"png");
		
	}
	if (( cinput == "2" ) || ( cinput == "Kr83m" )){
		// execute LCE map generator
		cout << "= S1 - Kr83m data LCE map ==================================" << endl;
		cout << "============================================================" << endl;
		cout << "Which datafile(s) do you want to analyse?" << endl;
		cout << "------------------------------------------------------------" << endl;
		cout << "(l) for: " << lastfile_Kr83m << endl;
		string datafile;
		cin >> datafile;
		cin.ignore();
		
		if (datafile=="l") {
			datafile = lastfile_Kr83m;
		}
		else {
			lastfile_Kr83m = datafile;
		}
		
		data_maps(datafile,9,4,4,"png");
		
	}
	if (( cinput == "3" )){
		// execute LCE map generator
		cout << "= S1 - Comparison of Kr83m data vs. MC =====================" << endl;
		cout << "============================================================" << endl;
		cout << "Which Kr83m datafile do you want to analyse?" << endl;
		cout << "------------------------------------------------------------" << endl;
		cout << "(l) for: " << lastfile_Kr83m << endl;
		string datafile_kr;
		cin >> datafile_kr;
		cin.ignore();
		
		if (datafile_kr=="l") {
			datafile_kr = lastfile_Kr83m;
		}
		else {
			lastfile_Kr83m = datafile_kr;
		}
		cout << "------------------------------------------------------------" << endl;
		cout << "Which PMT.ini file should be used?" << endl;
		cout << "------------------------------------------------------------" << endl;
		cout << "(l) for: " << lastfile_pmtini << endl;
		string pmtini;
		cin >> pmtini;
		cin.ignore();
		
		if (pmtini=="l") {
			pmtini = lastfile_pmtini;
		}
		else {
			lastfile_pmtini = pmtini;
		}
		cout << "------------------------------------------------------------" << endl;
		cout << "Which MC datafile(s) do you want to analyse?" << endl;
		cout << "------------------------------------------------------------" << endl;
		cout << "(l) for: " << lastfile_MC << endl;
		string datafile_mc;
		cin >> datafile_mc;
		cin.ignore();
		
		if (datafile_mc=="l") {
			datafile_mc = lastfile_MC;
		}
		else {
			lastfile_MC = datafile_mc;
		}
		
		optPhot_comparison(datafile_kr,pmtini,0.645,datafile_mc,9,4,4,"4 6 8 12","png");
		
	}
	if (( cinput == "4" ) || ( cinput == "parameters" )){
		cout << "= S1 - analyze parameter dir ===============================" << endl;
		cout << "============================================================" << endl;
		cout << "Which datafiles do you want to analyse (only directories)?" << endl;
		cout << "------------------------------------------------------------" << endl;
		cout << "(l) for: " << lastfile_parameters << endl;
		string datafile;
		cin >> datafile;
		cin.ignore();
		cout << "------------------------------------------------------------" << endl;
		cout << "Which parameter do you want to analyse?" << endl;
		cout << "------------------------------------------------------------" << endl;
		string parameter;
		cin >> parameter;
		cin.ignore();
		
		if (datafile=="l") {
			datafile = lastfile_parameters;
		}
		else {
			lastfile_parameters = datafile;
		}
		
		optPhot_parameter_variations(datafile,parameter,"png");
		
	}
	if ( cinput == "5" ){
		cout << "= Events - MC analysis =====================================" << endl;
		cout << "============================================================" << endl;
		cout << "Which datafile(s) do you want to analyse?" << endl;
		cout << "------------------------------------------------------------" << endl;
		cout << "(l) for: " << lastfile_MC << endl;
		string datafile;
		cin >> datafile;
		cin.ignore();
		
		if (datafile=="l") {
			datafile = lastfile_MC;
		}
		else {
			lastfile_MC = datafile;
		}
		
		calibration_source(datafile,"png");
		
	}

	if (( cinput == "7" ) && ( cinput == "6" )){
		// execute LCE map generator
		cout << "= S1 - Comparison of Kr83m data vs. MC (matcher) ===========" << endl;
		cout << "============================================================" << endl;
		cout << "Which Kr83m datafile do you want to analyse?" << endl;
		cout << "------------------------------------------------------------" << endl;
		cout << "(l) for: " << lastfile_Kr83m << endl;
		string datafile_kr;
		cin >> datafile_kr;
		cin.ignore();
		
		if (datafile_kr=="l") {
			datafile_kr = lastfile_Kr83m;
		}
		else {
			lastfile_Kr83m = datafile_kr;
		}
		cout << "------------------------------------------------------------" << endl;
		cout << "Which MC datafiles do you want to analyse? (only directories)" << endl;
		cout << "------------------------------------------------------------" << endl;
		cout << "(l) for: " << lastfile_parameters << endl;
		string datafile_mc;
		cin >> datafile_mc;
		cin.ignore();
		
		if (datafile_mc=="l") {
			datafile_mc = lastfile_parameters;
		}
		else {
			lastfile_parameters = datafile_mc;
		}
		
		optPhot_matching(datafile_kr,0.645,datafile_mc,9,4,4,"4 6 8 12","png");
		
	}
	if (( cinput == "8" )){
		// execute MC analysis
		cout << "= S2 - MC analysis =========================================" << endl;
		cout << "============================================================" << endl;
		cout << "Which datafile(s) do you want to analyse?" << endl;
		cout << "------------------------------------------------------------" << endl;
		cout << "(l) for: " << lastfile_MC << endl;
		string datafile;
		cin >> datafile;
		cin.ignore();
		
		if (datafile=="l") {
			datafile = lastfile_MC;
		}
		else {
			lastfile_MC = datafile;
		}
		
		//optPhot_S1(datafile,26,50,22,"png");
		optPhot_S2(datafile,"png");
		
	}

	
	ofstream configfile_out;
	configfile_out.open ("./.MC_Analyzer.config");
	configfile_out << lastfile_MC << "\n";
	configfile_out << lastfile_Kr83m << "\n";
	configfile_out << lastfile_parameters << "\n";
	configfile_out << lastfile_pmtini << "\n";
	configfile_out.close();
}