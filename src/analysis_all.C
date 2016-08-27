/******************************************************************
 * analysis_all.C
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
#include "../Analyzer.C"
#include <iostream>
#include <sstream>
#include <string>
#include <stdio.h>
using namespace std;

void analysis_all(string datafile){
	size_t found=datafile.find_last_of("/\\");
	string workingdirectory = datafile.substr(0,found);
	string datafilename = datafile.substr(found+1);
	
	cout << "===========================================" << endl;
	cout << "refreshing file/batch handling of:" << endl;

	string ext = ".root";
  TSystemDirectory dir(workingdirectory.c_str(), workingdirectory.c_str());
  TList *files = dir.GetListOfFiles();
  if (files) {
  	TSystemFile *file;
    TString fname;
    TIter next(files);
    while ((file=(TSystemFile*)next())) {
  	  fname = file->GetName();
      if (!file->IsDirectory() && fname.EndsWith(ext.c_str())) {
  	    cout << fname.Data() << endl;
      }
  	}
  }

	cout << "===========================================" << endl;
	cout << "continue? (Y/N)" << endl;
	string input;
	cin >> input;
	if ((input == "Y") || (input == "y")) {
		cout << "===========================================" << endl;
		if (files) {
  		TSystemFile *file;
    	TString fname;
    	TIter next(files);
    	while ((file=(TSystemFile*)next())) {
  	 	 fname = file->GetName();
     	 if (!file->IsDirectory() && fname.EndsWith(ext.c_str())) {
				 char filename[10000];
				 sprintf(filename,"%s/%s", workingdirectory.c_str(), fname.Data());
  	 	   analysis(filename);
     	 }
  		}
  	}
		cout << "===========================================" << endl;
	}
	else {
		cout << "===========================================" << endl;
		cout << "end." << endl;
		cout << "===========================================" << endl;
	} 
}

