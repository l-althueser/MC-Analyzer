/******************************************************************
 * process_LM.C
 * 
 * @author	Lutz Althueser
 * @date   	2015-04-01
 * @updated	2016-02-27
 *
 * @comment 
 *
 * @changes	 
 *
 ******************************************************************/
#include <iostream>
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <numeric>
#include <math.h> 
#include <cmath>
using namespace std;

/*=================================================================*/

void LM_DividingDetector_xy(){
	cout << "Please insert a datafile as parameter." << endl;
}

/*=================================================================*/

void LM_DividingDetector_xy(string datafile){
	LM_DividingDetector_xy(datafile, 40, 10, 40);
}

/*=================================================================*/

void LM_DividingDetector_xy(string datafile, const int NbRBin, const int NbTBin, const int NbZBin) {
	size_t found=datafile.find_last_of("/\\");
	string workingdirectory = datafile.substr(0,found);
	string datafilename = datafile.substr(found+1);
	TString Tdatafile = datafile;

	//////////////////////////////////////////////////////////////////

	// recommended values: NbRBin = 40; NbTBin = 10; NbZBin = 40;
	const int NbVolumes = NbRBin*NbTBin*NbZBin; 
	const float DetMax = -169.;//mm
	const float DetMin = -2.;//mm
	const float DetHeight = abs(DetMax - DetMin;//mm 
	const float DetRadius = 40.;//mm
	
	//////////////////////////////////////////////////////////////////

	int eventcounter[NbZBin][NbTBin][NbRBin]={0};
	int hitcounter[NbZBin][NbTBin][NbRBin]={0};
	int energycounter[NbZBin][NbTBin][NbRBin]={0};
	
	float zlow[NbZBin][NbTBin][NbRBin];
	float zhigh[NbZBin][NbTBin][NbRBin];
	float rlow[NbZBin][NbTBin][NbRBin];
	float rhigh[NbZBin][NbTBin][NbRBin];
	float tlow[NbZBin][NbTBin][NbRBin];
	float thigh[NbZBin][NbTBin][NbRBin];

	double pi = TMath::Pi();
	
	Double_t t_passed = 0, t_left, passed;
	TStopwatch timer;
	char p_t_left[100], p_t_passed[100];

	TFile *rootfile = new TFile(Tdatafile);
	TTree *T1 = (TTree*)rootfile->Get("t1");
	
	if ( rootfile->GetListOfKeys()->Contains("nbeventstosimulate") == 1 ) {
		TParameter<int> *NbEventsToSimulateParameter = rootfile->Get("nbeventstosimulate");
		const int NbSimuInitial = NbEventsToSimulateParameter->GetVal();
	}
	else {
		const int NbSimuInitial = 1000000;
	}
	long nbentries = T1->GetEntries();

	vector<int> *pmthits= new vector<int>;
	T1->SetBranchAddress("pmthits", &pmthits);
	int ntpmthits;
	T1->SetBranchAddress("ntpmthits", &ntpmthits);
	int nbpmthits;
	T1->SetBranchAddress("nbpmthits", &nbpmthits);
	vector<float> *ed= new vector<float>;
	T1->SetBranchAddress("ed", &ed);
	vector<string> *type= new vector<string>;
	T1->SetBranchAddress("type", &type);
	vector<string> *parenttype= new vector<string>;
	T1->SetBranchAddress("parenttype", &parenttype);
	vector<string> *edproc= new vector<string>;
	T1->SetBranchAddress("edproc", &edproc);
	vector<float> *xp= new vector<float>;
	T1->SetBranchAddress("xp", &xp);
	vector<float> *yp= new vector<float>;
	T1->SetBranchAddress("yp", &yp);
	vector<float> *zp= new vector<float>; 
	T1->SetBranchAddress("zp", &zp);

	//////////////////////////////////////////////////////////////////	

	cout << "===========================================" << endl;
	cout << "R is divided in " << NbRBin << " bins" << endl;
	cout << "Theta is divided in " << NbTBin << " bins" << endl;
	cout << "Z is divided in " << NbZBin << " bins" << endl;
	cout << "The Detector is divided into " << NbVolumes << " volumes" << endl;	 	
	cout << "Number of events to simulate = " << NbSimuInitial << endl;
	cout << "Total entries in loop: " << nbentries << endl;
	cout << "===========================================" << endl;

	//////////////////////////////////////////////////////////////////

	for (long i=0; i<nbentries; i++){
			T1->GetEntry(i);
			
		 	if(i%1000 == 0 && i!=0 || i == 100) {
				t_passed += timer.CpuTime();
				sprintf(p_t_passed, "%dh:%dm:%ds", floor(t_passed/3600.0), floor(fmod(t_passed,3600.0)/60.0), fmod(t_passed,60.0));
				timer.Start();
				t_left = (float)nbentries/((float)i/t_passed)-t_passed;
				sprintf(p_t_left, "%dh:%dm:%ds", floor(t_left/3600.0), floor(fmod(t_left,3600.0)/60.0), fmod(t_left,60.0));
				passed = (float)i/(float)nbentries*100 ;
				cerr << "events done: " << passed << "% - " 
						 << "time left: " << p_t_left << " - " 
						 << "time running: " << p_t_passed << " \r";
			}

		for (int vecsize=0; vecsize<(*yp).size(); vecsize++) {
			float Y = (*yp)[vecsize];
			float Z = (*zp)[vecsize];
			float X = (*xp)[vecsize];
			float hits = ntpmthits + nbpmthits;
			/*// for each pmt ...
			float hits=0;
			for (int m=0; m<14 ; m++){
				hits += (*pmthits).at(m);
			} */
			float energy = (*ed)[vecsize];
		
			// print Information about the current particle
			//cout << (*type)[vecsize] <<" "<< (*parenttype)[vecsize] <<" "<< (*edproc)[vecsize] <<" "<< (*ed)[vecsize] <<" "<< X <<" "<< Y <<" "<< Z <<" "<< endl;
		
			if ( X > -DetRadius && X < DetRadius && 
					 Y > -DetRadius && Y < DetRadius && 
					 Z > DetMax && Z < DetMin ) { 
					 
					double Radius = (TMath::Sqrt(TMath::Power(X,2) + TMath::Power(Y,2)));
					double Theta = TMath::ATan(Y/X);
				
					if (X < 0) Theta = Theta + TMath::Pi();
					if (Theta < 0) Theta = Theta + 2*TMath::Pi();
					
 		 			int z=floor(-((Z-DetMin)/(DetHeight/NbZBin)));
					int t=floor((Theta/(2*pi/NbTBin)));
					int r=floor((Radius/(DetRadius/NbRBin)));
					if (r == NbRBin) r--;
					int rr=floor((Radius*Radius/(DetRadius*DetRadius/NbRBin)));
					if (rr == NbRBin) rr--;				 

					if ( vecsize == (*yp).size()-1 ) {  
						hitcounter[z][t][r] += hits;
					}
					energycounter[z][t][r] += energy;
					eventcounter[z][t][r] += 1;
			}  
  	}
	}//loop on nentries

	t_passed += timer.CpuTime();
	sprintf(p_t_passed, "%dh:%dm:%ds", floor(t_passed/3600.0), floor(fmod(t_passed,3600.0)/60.0), fmod(t_passed,60.0));
	cout << endl << nbentries << " events in " << p_t_passed << endl;

	//////////////////////////////////////////////////////////////////

	//light yield calculation

	float QE = 0.30;
	float WValue = 1/50.;
	float NbSimu[NbZBin][NbTBin][NbRBin];
	float VolRel[NbZBin][NbTBin][NbRBin];
	float VolTot = DetHeight*pi*pow(DetRadius,2);
	float ly[NbZBin][NbTBin][NbRBin];

	ofstream txtfile;
	char filename[10000];
	sprintf(filename,"%s/%s_LM_ZSlice_R%d-T%d-Z%d.dat", workingdirectory.c_str(), datafilename.c_str(), NbRBin, NbTBin, NbZBin);
	
	cout << "===========================================" << endl;
	cout << "start print LY data to " << filename << endl;	

	txtfile.open(filename);
	txtfile << "z\t" << "t\t" << "r\t" << "zhigh\t"<< "tlow\t" << "rlow\t" << "ly\t" << "pmthits\t" << "events\t" << "energy\t" << endl;
	for (int z=0; z<NbZBin; z++){	
		for (int t=0; t<NbTBin; t++){	
		  for (int r=0; r<NbRBin ; r++){
				zhigh[z][t][r] = -z*DetHeight/NbZBin+DetMin;     //max value is -2
				zlow[z][t][r]  = (-1-z)*DetHeight/NbZBin+DetMin; //min value is -169
				rlow[z][t][r]  = r*DetRadius/NbRBin;      //min value is 0
				rhigh[z][t][r] = (r+1)*DetRadius/NbRBin;  //max value is DetRadius
				tlow[z][t][r]  = t*2*pi/NbTBin;           //min value is 0
				thigh[z][t][r] = 2*pi*(t+1)/NbTBin;       //max value is 2Pi

				VolRel[z][t][r] = 0.5*(pow(rhigh[z][t][r],2)-pow(rlow[z][t][r],2))*(thigh[z][t][r]-tlow[z][t][r])*(zhigh[z][t][r]-zlow[z][t][r]);
				NbSimu[z][t][r] =  NbSimuInitial*VolRel[z][t][r]/VolTot;

				ly[z][t][r]=(QE*hitcounter[z][t][r]/NbSimu[z][t][r])/WValue;		

				txtfile << z <<"\t"<< t <<"\t"<< r <<"\t"<< zhigh[z][t][r] <<"\t"<<tlow[z][t][r]<<"\t"<<rlow[z][t][r] << "\t"<< ly[z][t][r] <<"\t"<< hitcounter[z][t][r] <<"\t"<< eventcounter[z][t][r] <<"\t"<< energycounter[z][t][r] << endl;
			}
		}
	}
}

/*=================================================================*/

void LM_DividingDetector_zx(){
	cout << "Please insert a datafile as parameter." << endl;
}

/*=================================================================*/

void LM_DividingDetector_zx(string datafile){
	LM_DividingDetector_zx(datafile, 40, 40);
}

/*=================================================================*/

void LM_DividingDetector_zx(string datafile, const int NbZBin, const int NbXBin) {
	size_t found=datafile.find_last_of("/\\");
	string workingdirectory = datafile.substr(0,found);
	string datafilename = datafile.substr(found+1);
	TString Tdatafile = datafile;

	//////////////////////////////////////////////////////////////////

	// recommended values: NbZBin = 40; NbXBin = 40;
	const int NbVolumes = NbZBin*NbXBin; 
	const float DetMax = -169.;//mm
	const float DetMin = -2.;//mm
	const float DetHeight = abs(DetMax - DetMin);//mm 
	const float DetRadius = 40.;//mm
	
	//////////////////////////////////////////////////////////////////

	int eventcounter[NbZBin][NbXBin]={0};
	int hitcounter[NbZBin][NbXBin]={0};
	int energycounter[NbZBin][NbXBin]={0};
	
	float zlow[NbZBin][NbXBin];
	float zhigh[NbZBin][NbXBin];
	float xlow[NbZBin][NbXBin];
	float xhigh[NbZBin][NbXBin];

	double pi = TMath::Pi();

	Double_t t_passed = 0, t_left, passed;
	TStopwatch timer;
	char p_t_left[100], p_t_passed[100];

	TFile *rootfile = new TFile(Tdatafile);
	TTree *T1 = (TTree*)rootfile->Get("t1");
	
	if ( rootfile->GetListOfKeys()->Contains("nbeventstosimulate") == 1 ) {
		TParameter<int> *NbEventsToSimulateParameter = rootfile->Get("nbeventstosimulate");
		const int NbSimuInitial = NbEventsToSimulateParameter->GetVal();
	}
	else {
		const int NbSimuInitial = 1000000;
	}
	
	long nbentries = T1->GetEntries();

	vector<int> *pmthits= new vector<int>;
	T1->SetBranchAddress("pmthits", &pmthits);
	int ntpmthits;
	T1->SetBranchAddress("ntpmthits", &ntpmthits);
	int nbpmthits;
	T1->SetBranchAddress("nbpmthits", &nbpmthits);
	vector<float> *ed= new vector<float>;
	T1->SetBranchAddress("ed", &ed);
	vector<string> *type= new vector<string>;
	T1->SetBranchAddress("type", &type);
	vector<string> *parenttype= new vector<string>;
	T1->SetBranchAddress("parenttype", &parenttype);
	vector<string> *edproc= new vector<string>;
	T1->SetBranchAddress("edproc", &edproc);
	vector<float> *xp= new vector<float>;
	T1->SetBranchAddress("xp", &xp);
	vector<float> *yp= new vector<float>;
	T1->SetBranchAddress("yp", &yp);
	vector<float> *zp= new vector<float>; 
	T1->SetBranchAddress("zp", &zp);

	//////////////////////////////////////////////////////////////////	

	cout << "===========================================" << endl;
	cout << "Z is divided in " << NbZBin << " bins" << endl;
	cout << "X is divided in " << NbXBin << " bins" << endl;
	cout << "The Detector is divided into " << NbVolumes << " volumes" << endl;
	cout << "Number of events to simulate = " << NbSimuInitial << endl;
	cout << "Total entries in loop: " << nbentries << endl;
	cout << "===========================================" << endl;

	//////////////////////////////////////////////////////////////////

	for (long i=0; i<nbentries; i++){
			T1->GetEntry(i);
			
		 	if(i%1000 == 0 && i!=0 || i == 100) {
				t_passed += timer.CpuTime();
				sprintf(p_t_passed, "%dh:%dm:%ds", floor(t_passed/3600.0), floor(fmod(t_passed,3600.0)/60.0), fmod(t_passed,60.0));
				timer.Start();
				t_left = (float)nbentries/((float)i/t_passed)-t_passed;
				sprintf(p_t_left, "%dh:%dm:%ds", floor(t_left/3600.0), floor(fmod(t_left,3600.0)/60.0), fmod(t_left,60.0));
				passed = (float)i/(float)nbentries*100 ;
				cerr << "events done: " << passed << "% - " 
						 << "time left: " << p_t_left << " - " 
						 << "time running: " << p_t_passed << " \r";
			}

		for (int vecsize=0; vecsize<(*yp).size(); vecsize++) {
			float Y = (*yp)[vecsize];
			float Z = (*zp)[vecsize];
			float X = (*xp)[vecsize];
			float hits = ntpmthits + nbpmthits;
			/*// for each pmt ...
			float hits=0;
			for (int m=0; m<14 ; m++){
				hits += (*pmthits).at(m);
			} */
			float energy = (*ed)[vecsize];
		
			// print Information about the current particle
			//cout << (*type)[vecsize] <<" "<< (*parenttype)[vecsize] <<" "<< (*edproc)[vecsize] <<" "<< (*ed)[vecsize] <<" "<< X <<" "<< Y <<" "<< Z <<" "<< endl;
		
			if ( X > -DetRadius && X < DetRadius && 
					 Y > -DetRadius && Y < DetRadius && 
					 Z > DetMax && Z < DetMin ) { 
					 int z=-floor(Z/(-DetMax/NbZBin))-1;
					 int x=floor((X+DetRadius)/(2*DetRadius/NbXBin));
					 
					 // u can filter events with
					 //(*type)[vecsize] == "e-"
					 //(*edproc)[vecsize] == "LowEnPhotoElec"
					 
					 if ( vecsize == (*yp).size()-1 ) {  
					 //if ((*type)[vecsize] == "e-" ) {
					 //if ((*edproc)[vecsize] == "phot" ) {
					 	hitcounter[z][x] += hits;
					 }
					 energycounter[z][x] += energy;
					 eventcounter[z][x] += 1;
			}  
		}
	}//loop on nentries

	t_passed += timer.CpuTime();
	sprintf(p_t_passed, "%dh:%dm:%ds", floor(t_passed/3600.0), floor(fmod(t_passed,3600.0)/60.0), fmod(t_passed,60.0));
	cout << endl << nbentries << " events in " << p_t_passed << endl;

	//////////////////////////////////////////////////////////////////

	//light yield calculation

	float QE = 0.30;
	float WValue = 1/50.;
	float NbSimu[NbZBin][NbXBin];
	float VolRel[NbZBin][NbXBin];
	float VolTot = DetHeight*2*DetRadius;
	float ly[NbZBin][NbXBin];

	ofstream txtfile;
	char filename[10000];
	sprintf(filename,"%s/%s_LM_Z%d-X%d.dat", workingdirectory.c_str(), datafilename.c_str(), NbZBin, NbXBin);
	
	cout << "===========================================" << endl;
	cout << "start print LY data to " << filename << endl;	

	txtfile.open(filename);
	txtfile << "z\t" << "x\t" << "zhigh\t"<< "xlow\t" << "ly\t" << "pmthits\t" << "events\t" << "energy\t" << endl;
	for (int z=0; z<NbZBin; z++){	
		for (int x=0; x<NbXBin; x++){	
				zhigh[z][x]	= -z*DetHeight/NbZBin+DetMin;
				zlow[z][x] 	= (-1-z)*DetHeight/NbZBin+DetMin;
				xhigh[z][x]	= -DetRadius+(x+1)*2*DetRadius/NbXBin;
				xlow[z][x]	= -DetRadius+2*x*DetRadius/NbXBin;

			VolRel[z][x] =  (xhigh[z][x]-xlow[z][x])*(zhigh[z][x]-zlow[z][x]);
			NbSimu[z][x] =  NbSimuInitial*VolRel[z][x]/VolTot;

			ly[z][x]=(QE*hitcounter[z][x]/NbSimu[z][x])/WValue;

			txtfile << z <<"\t"<< x <<"\t"<< zhigh[z][x] <<"\t"<< xlow[z][x] <<"\t"<< ly[z][x] <<"\t"<< hitcounter[z][x] <<"\t"<< eventcounter[z][x] <<"\t"<< energycounter[z][x] << endl;
		}
	}
}

/*=================================================================*/

void LM_DividingDetector_zy(){
	cout << "Please insert a datafile as parameter." << endl;
}

/*=================================================================*/

void LM_DividingDetector_zy(string datafile){
	LM_DividingDetector_zy(datafile, 40, 40);
}

/*=================================================================*/

void LM_DividingDetector_zy(string datafile, const int NbZBin, const int NbYBin) {
	size_t found=datafile.find_last_of("/\\");
	string workingdirectory = datafile.substr(0,found);
	string datafilename = datafile.substr(found+1);
	TString Tdatafile = datafile;

	//////////////////////////////////////////////////////////////////

	// recommended values: NbZBin = 40; NbYBin = 40;
	const int NbVolumes = NbZBin*NbYBin;
	const float DetMax = -169.;//mm
	const float DetMin = -2.;//mm
	const float DetHeight = abs(DetMax - DetMin);//mm 
	const float DetRadius =  40.;//mm
	
	//////////////////////////////////////////////////////////////////

	int eventcounter[NbZBin][NbYBin]={0};
	int hitcounter[NbZBin][NbYBin]={0};
	int energycounter[NbZBin][NbYBin]={0};
	
	float zlow[NbZBin][NbYBin];
	float zhigh[NbZBin][NbYBin];
	float ylow[NbZBin][NbYBin];
	float yhigh[NbZBin][NbYBin];

	double pi = TMath::Pi();

	Double_t t_passed = 0, t_left, passed;
	TStopwatch timer;
	char p_t_left[100], p_t_passed[100];

	TFile *rootfile = new TFile(Tdatafile);
	TTree *T1 = (TTree*)rootfile->Get("t1");
	
	if ( rootfile->GetListOfKeys()->Contains("nbeventstosimulate") == 1 ) {
		TParameter<int> *NbEventsToSimulateParameter = rootfile->Get("nbeventstosimulate");
		const int NbSimuInitial = NbEventsToSimulateParameter->GetVal();
	}
	else {
		const int NbSimuInitial = 1000000;
	}
	long nbentries = T1->GetEntries();

	vector<int> *pmthits= new vector<int>;
	T1->SetBranchAddress("pmthits", &pmthits);
	int ntpmthits;
	T1->SetBranchAddress("ntpmthits", &ntpmthits);
	int nbpmthits;
	T1->SetBranchAddress("nbpmthits", &nbpmthits);
	vector<float> *ed= new vector<float>;
	T1->SetBranchAddress("ed", &ed);
	vector<string> *type= new vector<string>;
	T1->SetBranchAddress("type", &type);
	vector<string> *parenttype= new vector<string>;
	T1->SetBranchAddress("parenttype", &parenttype);
	vector<string> *edproc= new vector<string>;
	T1->SetBranchAddress("edproc", &edproc);
	vector<float> *xp= new vector<float>;
	T1->SetBranchAddress("xp", &xp);
	vector<float> *yp= new vector<float>;
	T1->SetBranchAddress("yp", &yp);
	vector<float> *zp= new vector<float>; 
	T1->SetBranchAddress("zp", &zp);

	//////////////////////////////////////////////////////////////////	

	cout << "===========================================" << endl;
	cout << "Z is divided in " << NbZBin << " bins" << endl;
	cout << "Y is divided in " << NbYBin << " bins" << endl;
	cout << "The Detector is divided into " << NbVolumes << " volumes" << endl;
	cout << "Number of events to simulate = " << NbSimuInitial << endl;
	cout << "Total entries in loop: " << nbentries << endl;
	cout << "===========================================" << endl;

	//////////////////////////////////////////////////////////////////

	for (long i=0; i<nbentries; i++){
			T1->GetEntry(i);
			
		 	if(i%1000 == 0 && i!=0 || i == 100) {
				t_passed += timer.CpuTime();
				sprintf(p_t_passed, "%dh:%dm:%ds", floor(t_passed/3600.0), floor(fmod(t_passed,3600.0)/60.0), fmod(t_passed,60.0));
				timer.Start();
				t_left = (float)nbentries/((float)i/t_passed)-t_passed;
				sprintf(p_t_left, "%dh:%dm:%ds", floor(t_left/3600.0), floor(fmod(t_left,3600.0)/60.0), fmod(t_left,60.0));
				passed = (float)i/(float)nbentries*100 ;
				cerr << "events done: " << passed << "% - " 
						 << "time left: " << p_t_left << " - " 
						 << "time running: " << p_t_passed << " \r";
			}

		for (int vecsize=0; vecsize<(*yp).size(); vecsize++) {
			float Y = (*yp)[vecsize];
			float Z = (*zp)[vecsize];
			float X = (*xp)[vecsize];
			float hits = ntpmthits + nbpmthits;
			/*// for each pmt ...
			float hits=0;
			for (int m=0; m<14 ; m++){
				hits += (*pmthits).at(m);
			} */
			float energy = (*ed)[vecsize];
		
			// print Information about the current particle
			//cout << (*type)[vecsize] <<" "<< (*parenttype)[vecsize] <<" "<< (*edproc)[vecsize] <<" "<< (*ed)[vecsize] <<" "<< X <<" "<< Y <<" "<< Z <<" "<< endl;
		
			if ( X > -DetRadius && X < DetRadius && 
					 Y > -DetRadius && Y < DetRadius && 
					 Z > DetMax && Z < DetMin ) { 
					 int z=-floor(Z/(-DetMax/NbZBin))-1;
					 int y=floor((Y+DetRadius)/(2*DetRadius/NbYBin));
					 
					 // u can filter events with
					 //(*type)[vecsize] == "e-"
					 //(*edproc)[vecsize] == "LowEnPhotoElec"
					 
					 if ( vecsize == (*yp).size()-1 ) {  
					 //if ((*type)[vecsize] == "e-" ) {
					 //if ((*edproc)[vecsize] == "phot" ) {
					 	hitcounter[z][y] += hits;
					 }
					 energycounter[z][y] += energy;
					 eventcounter[z][y] += 1;
			}  
		}
	}//loop on nentries

	t_passed += timer.CpuTime();
	sprintf(p_t_passed, "%dh:%dm:%ds", floor(t_passed/3600.0), floor(fmod(t_passed,3600.0)/60.0), fmod(t_passed,60.0));
	cout << endl << nbentries << " events in " << p_t_passed << endl;

	//////////////////////////////////////////////////////////////////

	//light yield calculation

	float QE = 0.30;
	float WValue = 1/50.;
	float NbSimu[NbZBin][NbYBin];
	float VolRel[NbZBin][NbYBin];
	float VolTot = DetHeight*2*DetRadius;
	float ly[NbZBin][NbYBin];

	ofstream txtfile;
	char filename[10000];
	sprintf(filename,"%s/%s_LM_Z%d-Y%d.dat", workingdirectory.c_str(), datafilename.c_str(), NbZBin, NbYBin);
	
	cout << "===========================================" << endl;
	cout << "start print LY data to " << filename << endl;	

	txtfile.open(filename);
	txtfile << "z\t" << "y\t" << "zhigh\t"<< "ylow\t" << "ly\t" << "pmthits\t" << "events\t" << "energy\t" << endl;
	for (int z=0; z<NbZBin; z++){	
		for (int y=0; y<NbYBin; y++){	
				zhigh[z][y]	= -z*DetHeight/NbZBin+DetMin;
				zlow[z][y] 	= (-1-z)*DetHeight/NbZBin+DetMin;
				yhigh[z][y]	= -DetRadius+(y+1)*2*DetRadius/NbYBin;
				ylow[z][y]	= -DetRadius+2*y*DetRadius/NbYBin;

			VolRel[z][y] =  (yhigh[z][y]-ylow[z][y])*(zhigh[z][y]-zlow[z][y]);
			NbSimu[z][y] =  NbSimuInitial*VolRel[z][y]/VolTot;

			ly[z][y]=(QE*hitcounter[z][y]/NbSimu[z][y])/WValue;

			txtfile << z <<"\t"<< y <<"\t"<< zhigh[z][y] <<"\t"<< ylow[z][y] <<"\t"<< ly[z][y] <<"\t"<< hitcounter[z][y] <<"\t"<< eventcounter[z][y] <<"\t"<< energycounter[z][y] << endl;
		}
	}
}

/*=================================================================*/
