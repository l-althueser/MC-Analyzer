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

using namespace std;

// include own functions
#include "src/OpPhot_MC.cc"
#include "src/OpPhot_Xe1T_data.cc"
#include "src/OpPhot_comparison.cc"
#include "src/OpPhot_MC_parameters.cc"
#include "src/Source_MC.cc"

/*=================================================================*/

void MC_Analyzer() {
	
	std::ifstream configfile_in("./.MC_Analyzer.config");
	string lastfile_MC, lastfile_Kr83m, lastfile_parameters;
	
	if(configfile_in) {
		configfile_in >> lastfile_MC;
		configfile_in >> lastfile_Kr83m;
		configfile_in >> lastfile_parameters;
	}
	
	cout << "============================================================" << endl;
	cout << "============================================================" << endl;
	cout << "= Starting MC-Analyzer =====================================" << endl;
	cout << "============================================================" << endl;
	cout << "============================================================" << endl;
	cout << "= (1) OpPhot - MC analysis =================================" << endl;
	cout << "= (2) OpPhot - Kr83m data LCE map ==========================" << endl;
	cout << "= (3) OpPhot - Comparison of Kr83m data vs. MC =============" << endl;
	cout << "= (4) OpPhot - analyze parameter dir =======================" << endl;
	cout << "============================================================" << endl;
	cout << "= (5) Source - MC analysis =================================" << endl;
	cout << "============================================================" << endl;
	
	string cinput;
	cin >> cinput;
	cin.ignore();
	
	if (( cinput == "1" ) || ( cinput == "MC" )){
		// execute MC analysis
		cout << "= OpPhot - MC analysis =====================================" << endl;
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
		
		OpPhot_MC(datafile,"png","big","OpPhotStudy");
		OpPhot_MC(datafile,"png","small","OpPhotStudy");
		
	}
	if (( cinput == "2" ) || ( cinput == "Kr83m" )){
		// execute LCE map generator
		cout << "= OpPhot - Kr83m data LCE map ==============================" << endl;
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
		
		OpPhot_Xe1T_data(datafile,"png","OpPhotStudy");
		
	}
	if (( cinput == "3" )){
		// execute LCE map generator
		cout << "= OpPhot - Comparison of Kr83m data vs. MC =================" << endl;
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
		
		OpPhot_comparison(datafile_kr,datafile_mc,"png","OpPhotStudy");
		
	}
	if (( cinput == "4" ) || ( cinput == "parameters" )){
		cout << "= OpPhot - analyze parameter dir ===========================" << endl;
		cout << "============================================================" << endl;
		cout << "Which datafile(s) do you want to analyse (only directories)?" << endl;
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
		
		OpPhot_MC_parameters(datafile,"png",parameter,"OpPhotStudy");
		
	}
	if ( cinput == "5" ){
		cout << "= Source - MC analysis =====================================" << endl;
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
		
		Source_MC(datafile,"png","small","OpPhotStudy");
		
	}

	
	ofstream configfile_out;
	configfile_out.open ("./.MC_Analyzer.config");
	configfile_out << lastfile_MC << "\n";
	configfile_out << lastfile_Kr83m << "\n";
	configfile_out << lastfile_parameters << "\n";
	configfile_out.close();
}