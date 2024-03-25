#include "Fpl_Headers_Precompiled.h"
//#include "Fpl_Random_Generator_Zikurat.h"


//init the static members 
unsigned long Fpl_Random_Generator_Zikurat::jz=123456789;
unsigned long Fpl_Random_Generator_Zikurat::jsr=123456789;
long Fpl_Random_Generator_Zikurat::hz;
unsigned long Fpl_Random_Generator_Zikurat::iz;
unsigned long Fpl_Random_Generator_Zikurat::kn[128];
double Fpl_Random_Generator_Zikurat::wn[128];
double Fpl_Random_Generator_Zikurat::fn[128];
bool Fpl_Random_Generator_Zikurat::flag_zikurat=false;

//Default constructor
Fpl_Random_Generator_Zikurat::Fpl_Random_Generator_Zikurat(void){

	Sys_Memory_Count::self()->add_mem(sizeof(Fpl_Random_Generator_Zikurat), _sys_system_modules::FPL_SYS);//count the memory
}
//Default destructor
Fpl_Random_Generator_Zikurat::~Fpl_Random_Generator_Zikurat(void){
	Sys_Memory_Count::self()->minus_mem(sizeof(Fpl_Random_Generator_Zikurat), _sys_system_modules::FPL_SYS);//count the memory

}
//_________
//public
//Initialize the random generator; the seed value is always the same (static)
void Fpl_Random_Generator_Zikurat::reinit_random(void){
	Fpl_Random_Generator_Zikurat::zigset(86947731);
}
//Initialize the random generator with a given seed value (static)
void Fpl_Random_Generator_Zikurat::reinit_random(const long int seed){
	Fpl_Random_Generator_Zikurat::zigset(seed);
	ostringstream cout;
	cout << "The Random Generator ziggurat is re-initialized with seed-value "<< seed<< " ! " <<endl;
	Sys_Common_Output::output_fpl->output_txt(cout.str(),true);
}
//Generation algorithm for zikurat
double Fpl_Random_Generator_Zikurat::run_generator(void){
	return this->rnor();
}
//_________
//private
//Generates a normal distributed variables [0..1]
double Fpl_Random_Generator_Zikurat::rnor(void){

	if(Fpl_Random_Generator_Zikurat::flag_zikurat==false) {//if the seed methode isn't initialized	
		Error msg=set_error(0);
		msg.output_msg(1);
	}

	hz=SHR3;
	iz=hz&127;
	if(abs(hz)<kn[iz]){
		return 0.5*hz*wn[iz]+0.5; 
	}
	else{ 
		return 0.5*this->nfix()+0.5;
	}
}
//This procedure sets the seed and creates the tables (static)
void Fpl_Random_Generator_Zikurat::zigset(unsigned long jsrseed){
	const double m1 = 2147483648.0;
	double dn=3.442619855899,tn=dn,vn=9.91256303526217e-3, q;
	
	int i;
	jsr^=jsrseed;

	//Set up tables for RNOR 
	q=vn/exp(-.5*dn*dn);
	kn[0]=(dn/q)*m1;
	kn[1]=0;

	wn[0]=q/m1;
	wn[127]=dn/m1;

	fn[0]=1.;
	fn[127]=exp(-.5*dn*dn);

	for(i=126;i>=1;i--){
	 dn=sqrt(-2.*log(vn/dn+exp(-.5*dn*dn)));
	 kn[i+1]=(dn/tn)*m1;
	 tn=dn;
	 fn[i]=exp(-.5*dn*dn);
	 wn[i]=dn/m1;
	}

	Fpl_Random_Generator_Zikurat::flag_zikurat=true;

}
//Generates variates from the residue when rejection in rnor-method occurs
double Fpl_Random_Generator_Zikurat::nfix(void){

	const double r = 3.442620f;     /* The start of the right tail */
	static double x, y;
	for(;;){
	 x=hz*wn[iz];      /* iz==0, handles the base strip */
	 if(iz==0)
	   { do{ x=-log(UNI)*0.2904764; y=-log(UNI);}	/* .2904764 is 1/r */
		while(y+y<x*x);
		return (hz>0)? r+x : -r-x;
	   }
						 /* iz>0, handle the wedges of other strips */
	  if( fn[iz]+UNI*(fn[iz-1]-fn[iz]) < exp(-.5*x*x) ) return x;

	 /* initiate, try to exit for(;;) for loop*/
	  hz=SHR3;
	  iz=hz&127;
	  if(abs(hz)<kn[iz]) return (hz*wn[iz]);
	}

}
//Set error(s)
Error Fpl_Random_Generator_Zikurat::set_error(const int err_type){
		string place="Fpl_Random_Generator_Zikurat::";
		string help;
		string reason;
		int type=0;
		bool fatal=false;
		stringstream info;
		Error msg;

	switch (err_type){
		case 0://table not found (fatal error)
			place.append("rnor(void)");
			reason="Random generator zikurat is not initialized";
			help="Initialized the random generator";
			type=7;
			fatal=true;//fatal error;
			break;
		default:
			place.append("set_error(int err_type)");
			reason ="Unknown flag!";
			help="Check the flags";
			type=6;
	}
	msg.set_msg(place, reason, help, type, fatal);
	msg.make_second_info(info.str());
	return msg;
}