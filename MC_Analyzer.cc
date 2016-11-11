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
#include "src/MC_plot_LCE.cc"
#include "src/Xe1T_LCE_map.cc"
#include "src/comparisons.cc"
#include "src/MC_plot_parameters.cc"

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
	cout << "= You can choose between two different analysis tools to ===" << endl;
	cout << "= execute. =================================================" << endl;
	cout << "============================================================" << endl;
	cout << "= (1) MC analysis ==========================================" << endl;
	cout << "= (2) Generate Kr83m LCE map from file =====================" << endl;
	cout << "= (3) Comparison of Kr83m data and MC ======================" << endl;
	cout << "= (4) MC analysis - parameters =============================" << endl;
	cout << "============================================================" << endl;
	
	string cinput;
	cin >> cinput;
	cin.ignore();
	
	if (( cinput == "1" ) || ( cinput == "MC" )){
		// execute MC analysis
		cout << "= MC analysis ==============================================" << endl;
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
		
		MC_plot_LCE(datafile,"png","big");
		MC_plot_LCE(datafile,"png","small");
		
	}
	if (( cinput == "2" ) || ( cinput == "Kr83m" )){
		// execute LCE map generator
		cout << "= LCE map generator =======================================" << endl;
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
		
		Xe1T_LCE_map(datafile,"png");
		
	}
	if (( cinput == "3" )){
		// execute LCE map generator
		cout << "= Comparison Kr83m vs MC ==================================" << endl;
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
		
		comparison(datafile_kr,datafile_mc,"png");
		
	}
	if (( cinput == "4" ) || ( cinput == "parameters" )){
		cout << "= MC analysis - parameters ================================" << endl;
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
		
		MC_plot_parameters(datafile,"png",parameter);
		
	}

	
	ofstream configfile_out;
	configfile_out.open ("./.MC_Analyzer.config");
	configfile_out << lastfile_MC << "\n";
	configfile_out << lastfile_Kr83m << "\n";
	configfile_out << lastfile_parameters << "\n";
	configfile_out.close();
}