/******************************************************************
 * plot_spectrum.C
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
void plot_spectrum(){
	cout << "Please insert a data file as parameter." << endl;
}

void plot_spectrum(string datafile) {
	size_t found=datafile.find_last_of("/\\");
	string workingdirectory = datafile.substr(0,found);
	string datafilename = datafile.substr(found+1);
	TString Tdatafile = datafile;

	TFile *rootfile = new TFile(Tdatafile);
	TTree *t1 = (TTree*)rootfile->Get("t1");

	char filename[10000];
	char histname[100];

	TStyle *myStyle = new TStyle("myStyle","my style");
	myStyle->SetPalette(1,0);
	myStyle->SetCanvasColor(10);
	myStyle->SetTitleFillColor(0);
	myStyle->cd();
	
	gStyle->SetPadLeftMargin(0.105);
	gStyle->SetPadRightMargin(0.02);
  gStyle->SetPadTopMargin(0.03);
	gStyle->SetPadBottomMargin(0.11);
	
	TCanvas *infocanvas = new TCanvas("spectrum", "spectrum");
	
	t1->Draw("etot");
	TH1F *htemp = (TH1F*)gPad->GetPrimitive("htemp");
	
	// you can hide the stats
	gStyle->SetOptStat(0);
	gStyle->SetStatY(0.9);
	gStyle->SetStatX(0.9);
	gStyle->SetStatW(0.125);
	gStyle->SetStatH(0.125); 
	
	sprintf(histname,"spectrum of %s", datafilename.c_str());
	htemp->SetTitle(histname);
	htemp->GetYaxis()->SetTitle("counts");
	htemp->GetXaxis()->SetTitle("E [keV]");
	gPad->SetLogy();
	
	htemp->GetXaxis()->SetLabelSize(0.05);
  htemp->GetYaxis()->SetLabelSize(0.05);
  htemp->GetXaxis()->SetTitleSize(0.05);
  htemp->GetYaxis()->SetTitleSize(0.05);
 
  // hide title and use the free space
  gStyle->SetOptTitle(0);
		
	sprintf(filename,"%s/%s_spectrum.pdf", workingdirectory.c_str(), datafilename.c_str());
	infocanvas->SaveAs(filename);	
}

