/******************************************************************
 * plot_info.C
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
void plot_info(){
	cout << "Please insert a data file as parameter." << endl;
}

void plot_info(string datafile) {
	size_t found=datafile.find_last_of("/\\");
	string workingdirectory = datafile.substr(0,found);
	string datafilename = datafile.substr(found+1);
	TString Tdatafile = datafile;

	TFile *rootfile = new TFile(Tdatafile);
	TTree *t1 = (TTree*)rootfile->Get("t1");

	char eventnumber[10000];
	char filename[10000];
	char histname[100];
	char cname[100];
	int x = 3;
	int y = 5;
	
	if ( rootfile->GetListOfKeys()->Contains("nbeventstosimulate") == 1 ) {
		TParameter<int> *NbEventsToSimulateParameter = rootfile->Get("nbeventstosimulate");
		const int NbSimuInitial = NbEventsToSimulateParameter->GetVal();
		sprintf(eventnumber,"#scale[0.6]{#splitline{events simulated}{%d}}", NbSimuInitial);
	}
	else {
		sprintf(eventnumber,"#scale[0.6]{#splitline{events simulated}{no information available}}");
	}
	
	TCanvas *infocanvas = new TCanvas("info","info",400*x,400*y);
	infocanvas->Divide(x,y,0.01,0.01);

	infocanvas->cd(1);
	sprintf(filename,"#scale[0.6]{#splitline{information about}{%s}}", datafilename.c_str());
	TLatex text1(0.05,0.7,filename); 
  text1.SetNDC(kTRUE);
  text1.Draw();
  TLatex text2(0.05,0.5,eventnumber); 
  text2.SetNDC(kTRUE);
  text2.Draw();
	infocanvas->cd(2);
	t1->Draw("e_pri");
	infocanvas->cd(3);
	t1->Draw("etot");
	TH1F *htemp = (TH1F*)gPad->GetPrimitive("htemp");
	//htemp->Set;
	gPad->SetLogy();
	infocanvas->cd(4);
	t1->Draw("xp_pri");
	infocanvas->cd(5);
	t1->Draw("yp_pri");
	infocanvas->cd(6);
	t1->Draw("zp_pri");
	infocanvas->cd(7);
	t1->Draw("xp");
	infocanvas->cd(8);
	t1->Draw("yp");
	infocanvas->cd(9);
	t1->Draw("zp");
	infocanvas->cd(10);
	t1->Draw("time");
	infocanvas->cd(11);
	t1->Draw("ed");
	infocanvas->cd(12);
	t1->Draw("nsteps");
	infocanvas->cd(13);
	t1->Draw("type_pri");
	infocanvas->cd(14);
	t1->Draw("edproc");
	infocanvas->cd(15);
	t1->Draw("creaproc");

	gStyle->SetOptStat("e");
	
	sprintf(filename,"%s/%s_info.pdf", workingdirectory.c_str(), datafilename.c_str());
	infocanvas->SaveAs(filename);
	
}
