/******************************************************************
 * debug.C
 * 
 * @author	Lutz Althueser
 * @date   	2015-04-01
 * @updated	2016-03-17
 *
 * @comment 
 *
 * @changes	 
 *
 ******************************************************************/

void debug(string datafile) {
	count_hits(datafile);
	count_hits_plot(datafile);
}

void count_events(string datafile) {
	size_t found=datafile.find_last_of("/\\");
	string workingdirectory = datafile.substr(0,found);
	string datafilename = datafile.substr(found+1);
	TString Tdatafile = datafile;

	char filename[10000];

	//TFile output(Tdatafile, "recreate");
	//output.cd();

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

	cout << "nbeventstosimulate: " << NbSimuInitial << endl;
	cout << "nbentries: " << nbentries << endl;

}

void count_hits_plot(string datafile) {
	size_t found=datafile.find_last_of("/\\");
	string workingdirectory = datafile.substr(0,found);
	string datafilename = datafile.substr(found+1);
	
	char filename[10000];
	sprintf(filename,"%s/%s_hits.dat", workingdirectory.c_str(), datafilename.c_str());
	string strfilename(filename);

	TString Tdatafile = strfilename;

	gROOT->SetBatch(kTRUE);

	char histname[100];

	TStyle *myStyle = new TStyle("myStyle","my style");
	myStyle->SetPalette(1,0);
	myStyle->SetCanvasColor(10);
	myStyle->SetTitleFillColor(0);
	myStyle->cd();
	
	gStyle->SetPadRightMargin(0.0355);
	gStyle->SetPadBottomMargin(0.105);
	
	TCanvas *plotcanvas = new TCanvas("PMT hits");
	TNtuple val("val","values","PMT:hits:pct");
	val.ReadFile(Tdatafile);
	val.SetMarkerStyle(8);
	val.SetMarkerColor(kRed);
	val.Draw("pct:PMT>>htemp(14,-0.5,13.5,100,0,100)","","");

	//TH1F *htemp = (TH1F*)gPad->GetPrimitive("htemp");
	TH1* htemp = (TH1*) gPad->GetListOfPrimitives()->FindObject("htemp"); 

	sprintf(histname,"PMT hits of each PMT in pct for 10e6 83mKr events");
	htemp->SetTitle(histname);
	htemp->GetXaxis()->SetTitle("PMT [#]");
	htemp->GetYaxis()->SetTitle("PMT hits [pct]");

	char vall[30];
	sprintf(vall,"top PMTs");
  TLatex caption(0.25,0.15,vall); 
  caption.SetNDC(kTRUE);
  caption.Draw();

	sprintf(vall,"bottom PMTs");
  TLatex captionn(0.67,0.15,vall); 
  captionn.SetNDC(kTRUE);
  captionn.Draw();
	
	htemp->GetXaxis()->SetLabelSize(0.05);
  htemp->GetYaxis()->SetLabelSize(0.05);
  htemp->GetXaxis()->SetTitleSize(0.05);
  htemp->GetYaxis()->SetTitleSize(0.05);
  
	//gStyle->SetOptFit();
	// you can hide the stats
	gStyle->SetOptStat(0);
	gStyle->SetStatY(0.9);
	gStyle->SetStatX(0.9);
	gStyle->SetStatW(0.125);
	gStyle->SetStatH(0.125);   

	sprintf(filename,"%s/%s_hits.pdf", workingdirectory.c_str(), datafilename.c_str());
	plotcanvas->SaveAs(filename);
}

string count_hits(string datafile) {
	size_t found=datafile.find_last_of("/\\");
	string workingdirectory = datafile.substr(0,found);
	string datafilename = datafile.substr(found+1);
	TString Tdatafile = datafile;

	//////////////////////////////////////////////////////////////////

	const float DetMax = 		169.;//mm
	const float DetMin = 			2.;//mm
	const float DetHeight = DetMax - DetMin;//mm 
	const float DetRadius = 40.;//mm
	
	//////////////////////////////////////////////////////////////////

	double hitcounter[14]={0};
	double hitpct[14]={0};
	double pmt3=0;
	double pmt10=0;
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

			if ( X > -DetRadius && X < DetRadius && 
					 Y > -DetRadius && Y < DetRadius && 
					 Z > -DetMax && Z < -DetMin ) { 
					 
					 float hits=0;
					 for (int m=0; m<14 ; m++){
					 	hitcounter[m] += (*pmthits).at(m);
						hits += (*pmthits).at(m);
					 }
					if (hits == 1) {
						if ((*pmthits).at(3)==1) pmt3++;
						if ((*pmthits).at(10)==1) pmt10++;
					}
			}  
		}
	}//loop on nentries

	t_passed += timer.CpuTime();
	sprintf(p_t_passed, "%dh:%dm:%ds", floor(t_passed/3600.0), floor(fmod(t_passed,3600.0)/60.0), fmod(t_passed,60.0));
	cout << endl << nbentries << " events in " << p_t_passed << endl;
	cout << "pmt3: " << pmt3 << endl;
	cout << "pmt10: " << pmt10 << endl;

  float max = hitcounter[0];
  for(int k = 0; k<14; k++) {
  	if(hitcounter[k] > max)
    	max = hitcounter[k];
  }

  for(int n = 0; n<14; n++) {
  	hitpct[n] = hitcounter[n]/max * 100;
  }

	//////////////////////////////////////////////////////////////////

	ofstream txtfile;
	char filename[10000];
	sprintf(filename,"%s/%s_hits.dat", workingdirectory.c_str(), datafilename.c_str());
	
	cout << "===========================================" << endl;
	cout << "start print data to " << filename << endl;	

	txtfile.open(filename);
	txtfile << "#\t" << "hits\t" << "pct\t" << endl;
	for (int m=0; m<14; m++){	
		txtfile << m <<"\t"<< hitcounter[m] <<"\t"<< hitpct[m] << endl;
	}
	return filename;
}
