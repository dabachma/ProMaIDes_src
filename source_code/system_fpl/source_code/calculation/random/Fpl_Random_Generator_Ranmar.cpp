#include "Fpl_Headers_Precompiled.h"
//#include "Fpl_Random_Generator_Ranmar.h"

//init the static members (members of ranmar generator)
double Fpl_Random_Generator_Ranmar::c=0.0;
double Fpl_Random_Generator_Ranmar::u[98]={0.0};
double Fpl_Random_Generator_Ranmar::cd=0.0;
double Fpl_Random_Generator_Ranmar::cm=0.0;
int Fpl_Random_Generator_Ranmar::i97=0;
int Fpl_Random_Generator_Ranmar::j97=0;
bool Fpl_Random_Generator_Ranmar::flag_ranmar=false;


//Default constructor
Fpl_Random_Generator_Ranmar::Fpl_Random_Generator_Ranmar(void){
	Sys_Memory_Count::self()->add_mem(sizeof(Fpl_Random_Generator_Ranmar), _sys_system_modules::FPL_SYS);//count the memory

}
//Default destructor
Fpl_Random_Generator_Ranmar::~Fpl_Random_Generator_Ranmar(void){
	Sys_Memory_Count::self()->minus_mem(sizeof(Fpl_Random_Generator_Ranmar), _sys_system_modules::FPL_SYS);//count the memory

}
//_________
//public
//Initialize the random generator; the seed value is always the same (static)
void Fpl_Random_Generator_Ranmar::reinit_random(void){
	Fpl_Random_Generator_Ranmar::seed_ran1(1977);

}
//Initialize the random generator with a given seed value (static)
void Fpl_Random_Generator_Ranmar::reinit_random(const int seed){
	Fpl_Random_Generator_Ranmar::seed_ran1(seed);
	ostringstream cout;
	cout << "The Random Generator ranmar is re-initialized with seed-value "<< seed<< " ! " <<endl;
	Sys_Common_Output::output_fpl->output_txt(cout.str(),true);
}
//Generation algorithm for ranmar
double Fpl_Random_Generator_Ranmar::run_generator(void){
	double uni;  /* the random number itself */
	
	if (Fpl_Random_Generator_Ranmar::flag_ranmar==false) {//if the seed methode isn't initialized	
		Error msg=set_error(0);
		msg.output_msg(1);
	}

	uni = u[i97] - u[j97];    /* difference between two [0..1] #s */
	if (uni < 0.0) uni += 1.0;
	u[i97] = uni;
	i97--;                  /* i97 ptr decrements and wraps around */
	if (i97==0) i97 = 97;
	j97--;                  /* j97 ptr decrements likewise */
	if (j97==0) j97 = 97;
	c -= cd;                /* finally, condition with c-decrement */
	if (c<0.0) c += cm;     /* cm > cd we hope! (only way c always >0) */
	uni -= c;
	if (uni<0.0) uni += 1.0;

	return (uni);            /* return the random # */

}
//_________
//private
//Initialize of the random generator by one integer (part of ranmar)
void Fpl_Random_Generator_Ranmar::seed_ran1(const int sval){
 int i, j;

  i = (97 * abs(sval)) % 31329;
  j = (33 * abs(sval)) % 30082;
  rmarin(i,j);

}
//Calculate the seed (part of ranmar)
void Fpl_Random_Generator_Ranmar::rmarin(const int ij, const int kl){
/**
 This is the initialization routine for the random number generator RANMAR()
 NOTE: The seed variables can have values between:    0 <= IJ <= 31328
                                                      0 <= KL <= 30081
 The random number sequences created by these two seeds are of sufficient 
 length to complete an entire calculation with. For example, if several
 different groups are working on different parts of the same calculation,
 each group could be assigned its own IJ seed. This would leave each group
 with 30000 choices for the second seed. That is to say, this random 
 number generator can create 900 million different subsequences -- with 
 each subsequence having a length of approximately 10^30.
 
 Use IJ = 1802 & KL = 9373 to test the random number generator. The
 subroutine RANMAR should be used to generate 20000 random numbers.
 Then display the next six random numbers generated multiplied by 4096*4096
 If the random number generator is working properly, the random numbers
 should be:
           6533892.0  14220222.0  7275067.0
           6172232.0  8354498.0   10633180.0
*/
	int i, j, k, l, ii, jj, m;
	double s, t;
	
	if (ij<0 || ij>31328 || kl<0 || kl>30081) {
		puts("The first random number seed must have a value between 0 and 31328.");
		puts("The second seed must have a value between 0 and 30081.");
		exit(1);
	}
	
	i = (ij/177)%177 + 2;
	j = ij%177 + 2;
	k = (kl/169)%178 + 1;
	l = kl%169;
	
	for (ii=1; ii<=97; ii++) {
		s = 0.0;
		t = 0.5;
		for (jj=1; jj<=24; jj++) {
			m = (((i*j)%179)*k) % 179;
			i = j;
			j = k;
			k = m;
			l = (53*l + 1) % 169;
			if ((l*m)%64 >= 32) s += t;
			t *= 0.5;
		}
		u[ii] = s;
	}
	
	c = 362436.0 / 16777216.0;
	cd = 7654321.0 / 16777216.0;
	cm = 16777213.0 / 16777216.0;
	
	i97 = 97;
	j97 = 33;
	
	
	Fpl_Random_Generator_Ranmar::flag_ranmar=true;
}
//Set error(s)
Error Fpl_Random_Generator_Ranmar::set_error(const int err_type){
		string place="Fpl_Random_Generator_Ranmar::";
		string help;
		string reason;
		int type=0;
		bool fatal=false;
		stringstream info;
		Error msg;

	switch (err_type){
		case 0://table not found (fatal error)
			place.append("ranmar_generator(void)");
			reason="Random generator ranmar is not initialized";
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