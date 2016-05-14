/******************************************************************
 * plot_LCE_fits.C
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
 
Double_t fitexpabs(Double_t *x,Double_t *par)
{
Double_t fitval =par[0]-TMath::Exp(-par[1]-par[2]*x[0]);
return fitval;
}

void abslength(string datafile) {
	size_t found=datafile.find_last_of("/\\");
	string workingdirectory = datafile.substr(0,found);
	string datafilename = datafile.substr(found+1);
	TString Tdatafile = datafile;

	char filename[10000];
	char histname[100];

	TStyle *myStyle = new TStyle("myStyle","my style");
	myStyle->SetPalette(1,0);
	myStyle->SetCanvasColor(10);
	myStyle->SetTitleFillColor(0);
	myStyle->cd();
	
	gStyle->SetPadRightMargin(0.0355);
	gStyle->SetPadBottomMargin(0.105);
	
	TCanvas *plotcanvas = new TCanvas("AbsorptionLength");
	TNtuple val("val","values","abslength:ly");
	val.ReadFile(Tdatafile);
	val.SetMarkerStyle(8);
	val.SetMarkerColor(kRed);
	val.Draw("ly:abslength","","");

	plotcanvas->SetGridx();
	plotcanvas->SetGridy();

	TH1F *htemp = (TH1F*)gPad->GetPrimitive("htemp");

	sprintf(histname,"Absorption Length Influence on the Light Yield");
	htemp->SetTitle(histname);
	htemp->GetYaxis()->SetTitle("ly [pe/keV]");
	htemp->GetXaxis()->SetTitle("absorption length [cm]");
	htemp->GetXaxis()->SetNdivisions(20,5,0);
	
	htemp->GetXaxis()->SetLabelSize(0.05);
  htemp->GetYaxis()->SetLabelSize(0.05);
  htemp->GetXaxis()->SetTitleSize(0.05);
  htemp->GetYaxis()->SetTitleSize(0.05);

  htemp->GetXaxis()->SetRangeUser(0, 520);
  
  TGraph g(Tdatafile);
  TF1 *func = new TF1("fit",fitexpabs,-3,3,3);
  func->SetParameters(3,0.3,0.001);
  func->SetParNames ("a","b","c");
	func->SetLineColor(kRed);
	cout << "===========================================" << endl;
	//gStyle->SetOptFit();
	// you can hide the stats
	gStyle->SetOptStat(0);
	gStyle->SetStatY(0.9);
	gStyle->SetStatX(0.9);
	gStyle->SetStatW(0.125);
	gStyle->SetStatH(0.125);   
	//g.Fit("fit");
  g.Draw("P");
	cout << "===========================================" << endl;

	sprintf(filename,"%s/%s.pdf", workingdirectory.c_str(), datafilename.c_str());
	plotcanvas->SaveAs(filename);

}

Double_t fitexpref(Double_t *x,Double_t *par)
{
Double_t fitval =par[0]+TMath::Exp(par[1]+par[2]*x[0]);
return fitval;
}

void ref(string datafile) {
	size_t found=datafile.find_last_of("/\\");
	string workingdirectory = datafile.substr(0,found);
	string datafilename = datafile.substr(found+1);
	TString Tdatafile = datafile;

	char filename[10000];
	char histname[100];

	TStyle *myStyle = new TStyle("myStyle","my style");
	myStyle->SetPalette(1,0);
	myStyle->SetCanvasColor(10);
	myStyle->SetTitleFillColor(0);
	myStyle->cd();
	
	gStyle->SetPadRightMargin(0.02);
	gStyle->SetPadBottomMargin(0.11);
	
	TCanvas *plotcanvas = new TCanvas("Reflectivity");
	TNtuple val("val","values","ref:ly");
	val.ReadFile(Tdatafile);
	val.SetMarkerStyle(8);
	val.SetMarkerColor(kRed);
	val.Draw("ly:ref","","");

	plotcanvas->SetGridx();
	plotcanvas->SetGridy();

	TH1F *htemp = (TH1F*)gPad->GetPrimitive("htemp");

	sprintf(histname,"Teflon Reflectivity Influence on the Light Yield");
	htemp->SetTitle(histname);
	htemp->GetYaxis()->SetTitle("ly [pe/keV]");
	htemp->GetXaxis()->SetTitle("teflon reflectivity [cm]");
	//htemp->GetXaxis()->SetNdivisions(20,5,0);
	
	htemp->GetXaxis()->SetLabelSize(0.05);
  htemp->GetYaxis()->SetLabelSize(0.05);
  htemp->GetXaxis()->SetTitleSize(0.05);
  htemp->GetYaxis()->SetTitleSize(0.05);

  TGraph g(Tdatafile);
  TF1 *func = new TF1("fit",fitexpref,-3,3,3);
  func->SetParameters(0.5,0.3,0.001);
  func->SetParNames ("a","b","c");
	func->SetLineColor(kRed);
	cout << "===========================================" << endl;
	//gStyle->SetOptFit();
	// you can hide the stats
	gStyle->SetOptStat(0);
	gStyle->SetStatY(0.9);
	gStyle->SetStatX(0.9);
	gStyle->SetStatW(0.125);
	gStyle->SetStatH(0.125);   
	//g.Fit("fit");
  g.Draw("P");
	cout << "===========================================" << endl;

	sprintf(filename,"%s/%s.pdf", workingdirectory.c_str(), datafilename.c_str());
	plotcanvas->SaveAs(filename);

}
