/******************************************************************
 * fileoperations.cc
 *
 * This is a collection of regulary used file operations in C++/ROOT.
 * 
 * @author		Lutz Althueser
 *
 ******************************************************************/
#ifndef __FILEOPERATIONS_H__
#define __FILEOPERATIONS_H__

// include C++ libs
#include <iostream>
#include <stdio.h>
#include <string>
#include <fstream>
#include <sstream>

bool fileexists (const string& name);
bool fileexists (const char* name);
void fileremove (const string& name);
void fileremove (const char *name);

using namespace std;

// this function is used to test if a specific file on Unix exists
bool fileexists (const string& name) {
	return fileexists(name.c_str());
}

bool fileexists (const char* name) {
    ifstream f(name);
    if (f.good()) {
        f.close();
        return true;
    } else {
        f.close();
        return false;
    }   
}

void fileremove (const string& name) {
	fileremove(name.c_str());
}

void fileremove (const char *name) {
	if (fileexists(name) == true) {
		cout << "deleting previous generated file: " << name << endl;
		if( remove(name) != 0 )
    	perror( "-> Error deleting outputfile." );
  	else
    	puts( "-> Outputfile successfully deleted." );
	}
}

#endif // __FILEOPERATIONS_H__