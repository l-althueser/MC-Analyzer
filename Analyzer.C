/******************************************************************
 * Analyzer.C
 * 
 * analysis scripts for the data files of the Muenster dual phase 
 * xenon TPC simulations - ROOT files
 *
 * @author	Lutz Althueser
 *
 * @usage 	.x Analyzer.C	start the analysis framework
 *			.x Analyzer.C("/home/.../.../*.root")	process a specific file
 *			.x Analyzer.C("/home/.../.../all")	process all files in folder
 *
 ******************************************************************/

#include "src/analysis_all.C"
#include "src/plot_info.C"
#include "src/plot_LCE_2D.C"
#include "src/plot_LCE_3D.C"
#include "src/plot_LCE_ZSlices.C"
#include "src/plot_LCE_fits.C"
#include "src/plot_LM.C"
#include "src/plot_spectrum.C"
#include "src/plot_opticalphotons.C"
#include "src/process_LCE.C"
#include "src/process_LM.C"

#include <iostream>
#include <sstream>
#include <string>
#include <stdio.h>
#include <cctype>
#include <algorithm>
using namespace std;

int Bin1 = 0, Bin2 = 0, Bin3 = 0;
string s;
string workingdirectory;
string datafilename;

/*=================================================================*/

void Analyzer() {
	analysis();
}

/*=================================================================*/

void Analyzer(string datafile){
	analysis(datafile);
}

/*=================================================================*/

void analysis() {
	cout << "===========================================" << endl;
	cout << "Which datafile do you want to analyse? (/.../.../filename.root)" << endl;
	string datafile;
	cin >> datafile;
	cin.ignore();
	analysis(datafile);
}

/*=================================================================*/

void analysis(string datafile){
	size_t found=datafile.find_last_of("/\\");
	workingdirectory = datafile.substr(0,found);
	datafilename = datafile.substr(found+1);
	
	bool batch = false;
	
	if (datafilename == "") {
		analysis();
	}
	else if (datafilename == "all") {
		analysis_all(datafile);
	}
	else {
		cout << "===========================================" << endl;
		cout << "datafile:" << endl;
		cout << datafilename << endl;
		cout << "===========================================" << endl;
		if (workingdirectory==datafilename) {
			cout << "insert the directory of the datafile:" << endl;
			cin >> workingdirectory;
			cin.ignore();
		}
		else {
			cout << "working directoy:" << endl;
			cout << workingdirectory << endl;
		}
		cout << "===========================================" << endl;

		for (;;) {
			cout << "please insert a command:" << endl;
			cout << " info:" << endl;
			cout << "   -> I: information about the root trees" << endl;
			cout << " optical photons (7eV photons confined in LXe):" << endl;
			cout << "   -> B : B1 + B2" << endl;
			cout << "     -> B1: LCE DividingDetector" << endl;
			cout << "     -> B2: LCE Map" << endl;
			cout << "     -> B3: LY MapZSlices" << endl;
			cout << "     -> B4: 3D LCE" << endl;
			cout << "   -> abs: plot absorption length" << endl;
			cout << "   -> ref: plot reflectivity" << endl;
			cout << " gamma sources:" << endl;
			cout << "   -> S: energy spectrum in keV" << endl;
			cout << "   -> ZY : ZY1 + ZY2" << endl;
			cout << "     -> ZY1: LM DividingDetector zy" << endl;
			cout << "     -> ZY2: LM zy events" << endl;
			cout << "     -> ZY3: LM zy pmthits" << endl;
			cout << "   -> ZX : ZX1 + ZX2" << endl;
			cout << "     -> ZX1: LM DividingDetector zx" << endl;
			cout << "     -> ZX2: LM zx events" << endl;
			cout << "     -> ZX3: LM zx pmthits" << endl;
			cout << "   -> XY : XY1 + XY2" << endl;
			cout << "     -> XY1: LM DividingDetector xy" << endl;
			cout << "     -> XY2: LM xy events" << endl;
			cout << "     -> XY3: LM xy pmthits" << endl;
			cout << "     -> XY4: LM xy events 2 positions" << endl;
			cout << "     -> XY5: LM xy events 3 positions" << endl;
			cout << "     -> XY6: LM xy events 4 positions" << endl;
			cout << " settings:" << endl;
			cout << "   -> o: toggle root direct output on" << endl;
			cout << "   -> o-off: toggle root direct output off" << endl;
			cout << "   -> C: change data file" << endl;
			cout << " -> E .q exit." << endl;
			cout << "" << endl;
			cout << "you can execute multiple commands seperated by \"+\"" << endl;
			cout << "===========================================" << endl;
			
			if (batch == false) {
				batch = true;
				gROOT->SetBatch(kTRUE);
			}
			
			string input;
			getline(cin, input);
			cout << "===========================================" << endl;

			if (( input == "E" ) || ( input == "e"    ) || 
				 ( input == ".q" ) || ( input == "exit" ) ||
				 ( input == "."	 )) {
				break;
				//return 0;
			}
			if (( input == "C" ) || ( input == "c"    )) {
			  fseek(stdin,0L,SEEK_END);
				analysis();
				return 0;
			}

			if (exec_commands(input, datafile, false) == true) {
				cout << endl << ".. to modify the canvas you have to exit this loop or activate direct output!";
				cout << endl << "===========================================" << endl;
				cout << "-------------------------------------------" << endl;
				cout << "===========================================" << endl;
			}
			else {
				// dont know the command - restart loop
				cout << "this command is not available!" << endl;
				cout << "-------------------------------------------" << endl;
				cout << "===========================================" << endl;
				continue;
			}	
		}
	}
}

/*=================================================================*/

bool exec_commands(string input, string datafile, bool returnValue) {
	size_t found=datafile.find_last_of("/\\");
	workingdirectory = datafile.substr(0,found);
	datafilename = datafile.substr(found+1);
	
	for(int i=0; i<input.length(); i++)
  	{ if(input[i] == ' ') input.erase(i,1); }
	while (input.find_last_of("+") != string::npos) {
		returnValue = exec_commands(input.substr(0,input.find_last_of("+")), datafile, false);
		input = input.substr(input.find_last_of("+")+1);
	}
	
	cout << "-------------------------------------------" << endl;		
	cout << "executing: " << input << endl;
	cout << "file: " << datafilename << endl;
	cout << "-------------------------------------------" << endl;	
	
	if (( input == "I" ) || 
			( input == "i" )) {
		cout << "info about: " << datafilename << endl;
		plot_info(datafile);
		returnValue = true;
	}
	if (( input == "O" ) || 
			( input == "o" ) ||
		  ( input == "o-on" )) {
		cout << "root direct output: on" << endl;
		gROOT->SetBatch(kFALSE);
		//show_canvas();
		returnValue = true;
	}
	if ( input == "o-off" ) {
		cout << "root direct output: off" << endl;
		gROOT->SetBatch(kTRUE);
		returnValue = true;
	}
	if (( input == "op" ) || 
		( input == "Op" )) {
		if (! bins()) getbin_dim3();
		cout << "Opticalphotons of: " << datafilename << endl;
		plot_opticalphotons(datafile, Bin1, Bin2, Bin3);
		show_canvas();
		returnValue = true;
	}
	if (( input == "B" ) || 
			( input == "b" ) ||
			( input == "B1") || 
			( input == "b1")) {
		if (! bins()) getbin_dim3();
		cout << "LCE DividingDetector of: " << datafilename << endl;
		LCE_DividingDetector(datafile, Bin1, Bin2, Bin3);
		returnValue = true;
	}
	if (( input == "B" ) || 
			( input == "b" ) ||
			( input == "B2") || 
			( input == "b2")) {
		if (! bins()) getbin_dim3();
		cout << "LCE Map of: " << datafilename << endl;
		LCE_Map(datafile, Bin1, Bin2, Bin3);
		show_canvas();
		returnValue = true;
	}
	if (( input == "B3") || 
			( input == "b3")) {
		if (! bins()) getbin_dim3();
		cout << "LY MapZSlices of: " << datafilename << endl;
		LCE_MapZSlices(datafile, Bin1, Bin2, Bin3);
		show_canvas();
		returnValue = true;
	}
	if (( input == "B4") || 
			( input == "b4")) {
		if (! bins()) getbin_dim3();
		cout << "3D LCE of: " << datafilename << endl;
		LCE_3D(datafile, Bin1, Bin2, Bin3);
		show_canvas();
		returnValue = true;
	}
	if (( input == "abs") || 
			( input == "ABS")) {
		cout << "absorption length: " << datafilename << endl;
		abslength(datafile);
		show_canvas();
		returnValue = true;
	}
	if (( input == "ref") || 
			( input == "REF")) {
		cout << "reflectivity: " << datafilename << endl;
		ref(datafile);
		show_canvas();
		returnValue = true;
	}
		
	if (( input == "S" ) || 
			( input == "s" )) {
		cout << "spectrum of: " << datafilename << endl;
		plot_spectrum(datafile);
		returnValue = true;
	}
	if (( input == "ZY" ) || 
			( input == "zy" ) ||
			( input == "ZY1") || 
			( input == "zy1")) {
		if (! bins()) getbin_dim2();
		cout << "LM DividingDetector zy of: " << datafilename << endl;
		LM_DividingDetector_zy(datafile, Bin1, Bin2);
		returnValue = true;
	}
	if (( input == "ZY" ) || 
			( input == "zy" ) ||
			( input == "ZY2") || 
			( input == "zy2")) {
		if (! bins()) getbin_dim2();
		cout << "LM zy of: " << datafilename << endl;
		LM_zy(datafile, Bin1, Bin2);
		show_canvas();
		returnValue = true;
	}
	if (( input == "ZY3" ) || 
			( input == "zy3" )) {
		if (! bins()) getbin_dim2();
		cout << "LM zy of: " << datafilename << endl;
		LM_zy(datafile, "pmthits", Bin1, Bin2);
		show_canvas();
		returnValue = true;
	}
		
	if (( input == "ZX" ) || 
			( input == "zx" ) ||
			( input == "ZX1" ) || 
			( input == "zx1" )) {
		if (! bins()) getbin_dim2();
		cout << "LM DividingDetector zx of: " << datafilename << endl;
		LM_DividingDetector_zx(datafile, Bin1, Bin2);
		returnValue = true;
	}
	if (( input == "ZX" ) || 
			( input == "zx" ) ||
			( input == "ZX2" ) || 
			( input == "zx2" )) {
		if (! bins()) getbin_dim2();
		cout << "LM zx of: " << datafilename << endl;
		LM_zx(datafile, Bin1, Bin2);
		show_canvas();
		returnValue = true;
	}
	if (( input == "ZX3" ) || 
			( input == "zx3" )) {
		if (! bins()) getbin_dim2();
		cout << "LM zx of: " << datafilename << endl;
		LM_zx(datafile, "pmthits", Bin1, Bin2);
		show_canvas();
		returnValue = true;
	}
		
	if (( input == "XY" ) || 
			( input == "xy" ) ||
			( input == "YX" ) || 
			( input == "yx" ) ||
			( input == "YX1" ) || 
			( input == "yx1" ) ||
			( input == "XY1" ) || 
			( input == "xy1" ) ) {
		if (! bins()) getbin_dim3();
		cout << "LM DividingDetector xy of: " << datafilename << endl;
		LM_DividingDetector_xy(datafile, Bin1, Bin2, Bin3);
		returnValue = true;
	}
	if (( input == "XY" ) || 
			( input == "xy" ) ||
			( input == "YX" ) || 
			( input == "yx" ) ||
			( input == "YX2" ) || 
			( input == "yx2" ) ||
			( input == "XY2" ) || 
			( input == "xy2" ) ) {
		if (! bins()) getbin_dim3();
		cout << "LM xy of: " << datafilename << endl;
		LM_xy(datafile, Bin1, Bin2, Bin3);
		show_canvas();
		returnValue = true;
	}
	if (( input == "YX3" ) || 
			( input == "yx3" ) ||
			( input == "XY3" ) || 
			( input == "xy3" ) ) {
		if (! bins()) getbin_dim3();
		cout << "LM xy of: " << datafilename << endl;
		LM_xy(datafile, "pmthits", Bin1, Bin2, Bin3);
		show_canvas();
		returnValue = true;
	}
	if (( input == "YX4" ) || 
			( input == "yx4" ) ||
			( input == "XY4" ) || 
			( input == "xy4" ) ) {
		if (! bins()) getbin_dim3();
		cout << "LM xy of: " << datafilename << endl;
		LM_xy(datafile, "2pos", Bin1, Bin2, Bin3);
		show_canvas();
		returnValue = true;
	}
	if (( input == "YX5" ) || 
			( input == "yx5" ) ||
			( input == "XY5" ) || 
			( input == "xy5" ) ) {
		if (! bins()) getbin_dim3();
		cout << "LM xy of: " << datafilename << endl;
		LM_xy(datafile, "3pos", Bin1, Bin2, Bin3);
		show_canvas();
		returnValue = true;
	}
	if (( input == "YX6" ) || 
			( input == "yx6" ) ||
			( input == "XY6" ) || 
			( input == "xy6" ) ) {
		if (! bins()) getbin_dim3();
		cout << "LM xy of: " << datafilename << endl;
		LM_xy(datafile, "4pos", Bin1, Bin2, Bin3);
		show_canvas();
		returnValue = true;
	}
	
	//reset bins
	Bin1 = 0;
	Bin2 = 0;
	Bin3 = 0;		

	return returnValue;
}

/*=================================================================*/

void show_canvas() {
	// show canvas
	gPad->Modified();
	gPad->Update();
	//gSystem->ProcessEvents(); 
}

/*=================================================================*/

void getbin_dim2() {
	cout << "Do you want to change the Bins? (press return to skip)" << endl;	
	cout << "Nb?Bin Nb?Bin (default: 20 20)" << endl;
	fseek(stdin,0L,SEEK_END);
	s.clear();
	while (getline( cin, s ) && !s.empty() && (bins() == false)) {
		stringstream ss( s );
		ss >> Bin1 >> Bin2;
		if (!ss) {
		  cout << "Invalid input. Try again" << endl;
			Bin1 = 0;
			Bin2 = 0;
			Bin3 = 0;		
			break;	
		}
		else {
			cout << "Nb?Bin Nb?Bin: " << Bin1 << " " << Bin2 << endl;
			break;			
		}
	}
	if ( s.empty() ) {
		cout << "Nb?Bin Nb?Bin: 20 20" << endl;
		Bin1 = 20;
		Bin2 = 20;
		Bin3 = 0;		
	}
}

/*=================================================================*/

void getbin_dim3() {
	cout << "Do you want to change the Bins? (press return to skip)" << endl;
	cout << "NbRBin NbTBin NbZBin (default: 40 10 40)" << endl;
	fseek(stdin,0L,SEEK_END);
	//cin.ignore();
	s.clear();
	while (getline( cin, s ) && !s.empty() && (bins() == false)) {
		stringstream ss( s );
		ss >> Bin1 >> Bin2 >> Bin3;
		if (!ss) {
	 	  cout << "Invalid input. Try again" << endl;
			Bin1 = 0;
			Bin2 = 0;
			Bin3 = 0;	
			break;		
		}
	 	else {
	 		cout << "NbRBin NbTBin NbZBin: " << Bin1 << " " << Bin2 << " " << Bin3 << endl;
			break;			
		}
	}
	if ( s.empty() ) {
		Bin1 = 40;
		Bin2 = 10;
		Bin3 = 40;
		cout << "NbRBin NbTBin NbZBin: " << Bin1 << " " << Bin2 << " " << Bin3 << endl;		
	}
}

/*=================================================================*/

bool bins() {
	if (Bin1 + Bin2 + Bin3 == 0)
		return false;

	return true;
}

/*=================================================================*/
