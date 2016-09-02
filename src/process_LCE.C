/******************************************************************
 * process_LCE.C
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
#include <stdio.h>
#include <stdlib.h>
#include <string>
using namespace std;

//////////////////////////////////////////////////////////////////
// anaylsis_LCE:
// 	DividingDetector
//////////////////////////////////////////////////////////////////

/*=================================================================*/

void LCE_DividingDetector(){
	cout << "Please insert a datafile as parameter." << endl;
}

/*=================================================================*/

void LCE_DividingDetector(string datafile){
	R_LCE_DividingDetector(datafile, 40, 10, 40);
}

/*=================================================================*/

void LCE_DividingDetector(string datafile, const int NbRBin, const int NbTBin, const int NbZBin) {
	size_t found=datafile.find_last_of("/\\");
	string workingdirectory = datafile.substr(0,found);
	string datafilename = datafile.substr(found+1);
	TString Tdatafile = datafile;

	//////////////////////////////////////////////////////////////////

	// recommended values: NbRBin = 40; NbTBin = 10; NbZBin = 40;
	const int NbVolumes = NbRBin*NbTBin*NbZBin; 
	const float DetMax = -169.;//mm
	const float DetMin = -2;//mm
	const float DetHeight = abs(DetMax - DetMin);//mm 
	const float DetRadius = 40.;

	//////////////////////////////////////////////////////////////////

	double counter[NbZBin][NbTBin][NbRBin]={0};
	double counter_pri[NbZBin][NbTBin][NbRBin]={0};
	double LCE[NbZBin][NbTBin][NbRBin]={0};
	double LCEabs[NbZBin][NbTBin][NbRBin]={0};
	
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
		const int NbSimuInitial = 100000000;
	}
	long nbentries = T1->GetEntries();

	vector<int> *pmthits= new vector<int>;
	T1->SetBranchAddress("pmthits", &pmthits);
	int ntpmthits;
	T1->SetBranchAddress("ntpmthits", &ntpmthits);
	int nbpmthits;
	T1->SetBranchAddress("nbpmthits", &nbpmthits);
	float xp_pri;
	T1->SetBranchAddress("xp_pri", &xp_pri);
	float yp_pri;
	T1->SetBranchAddress("yp_pri", &yp_pri);
	float zp_pri;
	T1->SetBranchAddress("zp_pri", &zp_pri);

	//////////////////////////////////////////////////////////////////	

	cout << "===========================================" << endl;
	cout << "R is divided in " << NbRBin << " bins" << endl;
	cout << "Theta is divided in " << NbTBin << " bins" << endl;
	cout << "Z is divided in " << NbZBin << " bins" << endl;
	cout << "The Detector is divided into " << NbVolumes << " volumes" << endl;
	cout << "Number of events to simulate = " << NbSimuInitial << endl;
	cout << "Total entries: " << nbentries << endl;
 	cout << "===========================================" << endl;

	//////////////////////////////////////////////////////////////////
	 	
	float detphot = 0; 	
	 	
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

	float x = xp_pri;
	float y = yp_pri;
	float Z = zp_pri;

	if ( x > -DetRadius && x < DetRadius && 
			 y > -DetRadius && y < DetRadius && 
			 Z > DetMax && Z < DetMin ) { 
	 		 		 
 		 	double Radius = (TMath::Sqrt(TMath::Power(x,2) + TMath::Power(y,2)));
			double Theta = TMath::ATan(y/x);
				
			if (x < 0) Theta = Theta + TMath::Pi();
			if (Theta < 0) Theta = Theta + 2*TMath::Pi();
			
 		 	int z=floor(-((Z-DetMin)/(DetHeight/NbZBin)));
			int t=floor((Theta/(2*pi/NbTBin)));
			int r=floor(Radius/(DetRadius/NbRBin));
			if (r == NbRBin) r--;
			int rr=floor(Radius*Radius/(DetRadius*DetRadius/NbRBin));
			if (rr == NbRBin) rr--;				

	 		detphot++; //number of entries (events with LXeHit or PMTHit)
			counter_pri[z][t][r]++; //entries per volume
			counter[z][t][r] += ntpmthits + nbpmthits;
			LCE[z][t][rr] += ntpmthits + nbpmthits;
			LCEabs[z][t][rr] += ntpmthits + nbpmthits;

			/*
			// for a single pmt -> (*pmthits).at(m)
			counter[z][t][r] += (*pmthits).at(0);
			LCE[z][t][rr] += (*pmthits).at(0);
			LCEabs[z][t][rr] += (*pmthits).at(0);
			*/
	}
}//loop on nentries

	t_passed += timer.CpuTime();
	sprintf(p_t_passed, "%dh:%dm:%ds", floor(t_passed/3600.0), floor(fmod(t_passed,3600.0)/60.0), fmod(t_passed,60.0));
	cout << endl << nbentries << " events in " << p_t_passed << endl;

	//////////////////////////////////////////////////////////////////
/*
	// uncomment for debugging
	cout << "===========================================" << endl;
	cout << "debugging information:" << endl;
		for (int z=0; z<NbZBin; z++){
		 for (int t=0; t<NbTBin; t++){
			for (int r=0; r<NbRBin ; r++){
	 
	 	cout << "Initial #photon in volume: " << counter_pri[z][t][r] <<" for z= " << z <<" and t= " << t <<" and r= " << r <<   endl;
	 	cout << "#pmt hit in volume: " << counter[z][t][r] <<" for z= " << z <<" and t= " << t <<" and r= " << r <<   endl;
	 
			}
		 }
		}
	cout << "===========================================" << endl;
*/
	//////////////////////////////////////////////////////////////////

	//light yield calculation

	float QE = 0.30; //quantum efficiency of the PMTs
	float WValue = 0.020; //keV - average energy required to produce one scintillation photon
	float NbSimu[NbZBin][NbTBin][NbRBin]={0};
	float VolRel[NbZBin][NbTBin][NbRBin]={0};
	float VolTot = DetHeight*pi*pow(DetRadius,2);
	float ly[NbZBin][NbTBin][NbRBin]={0};

	ofstream txtfile;
	char filename[10000];
	sprintf(filename,"%s/%s_LCE-LY_R%d-T%d-Z%d.dat", workingdirectory.c_str(), datafilename.c_str(), NbRBin, NbTBin, NbZBin);

	cout << "===========================================" << endl;
	cout << "start print LY data." << endl;

	double test=0.;

	txtfile.open(filename);
	txtfile << "z\t" << "t\t" << "r\t" << "zhigh\t"<< "tlow\t" << "rlow\t" << "ly\t" << "LCEabs\t" << "LCE\t" << "counter\t" << endl;
	for (int z=0; z<NbZBin; z++){	
		for (int t=0; t<NbTBin; t++){	
		  for (int r=0; r<NbRBin; r++){
				zhigh[z][t][r] = -z*DetHeight/NbZBin+DetMin;     //max value is -2
				zlow[z][t][r]  = (-1-z)*DetHeight/NbZBin+DetMin; //min value is -169
				rlow[z][t][r]  = r*DetRadius/NbRBin;      //min value is 0
				rhigh[z][t][r] = (r+1)*DetRadius/NbRBin;  //max value is DetRadius
				tlow[z][t][r]  = t*2*pi/NbTBin;           //min value is 0
				thigh[z][t][r] = 2*pi*(t+1)/NbTBin;       //max value is 2Pi

				VolRel[z][t][r] = 0.5*(pow(rhigh[z][t][r],2)-pow(rlow[z][t][r],2))*(thigh[z][t][r]-tlow[z][t][r])*(zhigh[z][t][r]-zlow[z][t][r]);
				NbSimu[z][t][r] = NbSimuInitial*VolRel[z][t][r]/VolTot;

				ly[z][t][r]=(QE*counter[z][t][r]/NbSimu[z][t][r])/WValue;
				counter[z][t][r]=counter[z][t][r]/NbSimu[z][t][r];
				
				VolRel[z][t][r] = 0.5*( pow(sqrt((r+1)*DetRadius*DetRadius/NbRBin),2)-pow(sqrt(r*DetRadius*DetRadius/NbRBin),2))*(thigh[z][t][r]-tlow[z][t][r])*(zhigh[z][t][r]-zlow[z][t][r]);
			  LCE[z][t][r]=LCE[z][t][r]/(NbSimuInitial*(VolRel[z][t][r]/VolTot));	
			
				txtfile << z <<"\t"<< t <<"\t"<< r <<"\t"<< zhigh[z][t][r] <<"\t"<<tlow[z][t][r]<<"\t"<<rlow[z][t][r] << "\t"<< ly[z][t][r] << "\t" << LCEabs[z][t][r] << "\t" << LCE[z][t][r] << "\t" << counter[z][t][r] << endl;
			}
		}
 }
}

/*=================================================================*/
