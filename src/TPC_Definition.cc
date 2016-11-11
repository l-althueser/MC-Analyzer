#ifndef __TPC_DEFINITION_H__
#define __TPC_DEFINITION_H__

class TPC_Definition {
    int nbinsZ, nbinsRR, nbinsR;
	int LCE_min, LCE_max;
	double chamber_minZ, chamber_maxZ;
	double chamber_minRR, chamber_maxRR, chamber_maxR;
	double LXe_minZ, LXe_maxZ;
	double LXe_minRR, LXe_maxRR, LXe_maxR;
	
  public:
	TPC_Definition ();
	TPC_Definition (int,int,int);
    void TPC_Xe1T ();
	void TPC_MS ();
	void Set_Bins (int,int,int);
	void Set_LCE_max (int);
    int Get_nbinsZ (void) {return nbinsZ;}
	int Get_nbinsRR (void) {return nbinsRR;}
	int Get_nbinsR (void) {return nbinsR;}
	int Get_LCE_min (void) {return LCE_min;}
	int Get_LCE_max (void) {return LCE_max;}
	double Get_chamber_minZ (void) {return chamber_minZ;}
	double Get_chamber_maxZ (void) {return chamber_maxZ;}
	double Get_chamber_minRR (void) {return chamber_minRR;}
	double Get_chamber_maxRR (void) {return chamber_maxRR;}
	double Get_chamber_maxR (void) {return chamber_maxR;}
	double Get_LXe_minZ (void) {return LXe_minZ;}
	double Get_LXe_maxZ (void) {return LXe_maxZ;}
	double Get_LXe_minRR (void) {return LXe_minRR;}
	double Get_LXe_maxRR (void) {return LXe_maxRR;}
	double Get_LXe_maxR (void) {return LXe_maxR;}
};

TPC_Definition::TPC_Definition () {
	TPC_Xe1T();
	nbinsZ  = 26;
	nbinsRR = 22;
	nbinsR  = 50;
	LCE_min = 0;
	LCE_max = 100;
}

TPC_Definition::TPC_Definition (int Z, int R, int RR) {
	TPC_Definition ();
	Set_Bins (Z, R, RR);
}

void TPC_Definition::TPC_Xe1T () {
  	chamber_minZ  = -140.;
	chamber_maxZ  = 6.;
	chamber_minRR = 0.;
	chamber_maxR  = 60.;
	chamber_maxRR = chamber_maxR*chamber_maxR;
	
	LXe_minZ  = -96.7;
	LXe_maxZ  = 0.;
	LXe_minRR = 0.;
	LXe_maxR  = 47.9;
	LXe_maxRR = LXe_maxR*LXe_maxR;
}

void TPC_Definition::TPC_MS () {
  	chamber_minZ  = -1.8;
	chamber_maxZ  = 0.5;
	chamber_minRR = 0.;
	chamber_maxR  = 5.;
	chamber_maxRR = chamber_maxR*chamber_maxR;
	
	LXe_minZ  = -1.69;
	LXe_maxZ  = -0.02;
	LXe_minRR = 0.;
	LXe_maxR  = 4;
	LXe_maxRR = LXe_maxR*LXe_maxR;
}

void TPC_Definition::Set_Bins (int Z, int R, int RR) {
	nbinsZ  = Z;
	nbinsRR = RR;
	nbinsR  = R;
}

void TPC_Definition::Set_LCE_max (int LCE) {
	LCE_max = LCE;
}

#endif // __TPC_DEFINITION_H__