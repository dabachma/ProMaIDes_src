#include "Fpl_Headers_Precompiled.h"
//#include "Fpl_Random_Generator_MofAll.h"

//init static members
short Fpl_Random_Generator_MofAll::mother1[10];
short Fpl_Random_Generator_MofAll::mother2[10];

//Default constructor
Fpl_Random_Generator_MofAll::Fpl_Random_Generator_MofAll(void){
	Sys_Memory_Count::self()->add_mem(sizeof(Fpl_Random_Generator_MofAll), _sys_system_modules::FPL_SYS);//count the memory

}
//Default destructor
Fpl_Random_Generator_MofAll::~Fpl_Random_Generator_MofAll(void){
	Sys_Memory_Count::self()->minus_mem(sizeof(Fpl_Random_Generator_MofAll), _sys_system_modules::FPL_SYS);//count the memory

}
//________
//public
//Initialize the random generator; the seed value is always the same 
void Fpl_Random_Generator_MofAll::reinit_random(void){

	unsigned long  number;
    short n,*p;
    unsigned short sNumber;
	unsigned long *pSeed;
	
	unsigned long seed=1977657;
	pSeed=&seed;


	//Initialize motheri with 9 random values the first time */
	sNumber=*pSeed&m16Mask;   /* The low 16 bits */
	number=*pSeed&m31Mask;   /* Only want 31 bits */

	p=mother1;
	for (n=18;n--;) {
			number=30903*sNumber+(number>>16);   
			*p++=sNumber=number&m16Mask;
			if (n==9)
					p=mother2;
	}
	/* make cary 15 bits */
	mother1[0]&=m15Mask;
	mother2[0]&=m15Mask;
	ostringstream cout;
	cout << "The Random Generator mother-of-all is re-initialized! " <<endl;
	Sys_Common_Output::output_fpl->output_txt(cout.str(),true);
}
//Initialize the random generator with a given seed value
void Fpl_Random_Generator_MofAll::reinit_random(const long int seed){
	unsigned long  number;
    short n,*p;
    unsigned short sNumber;
	unsigned long *pSeed;
	unsigned long buffer;
	buffer=seed;
	
	pSeed=&buffer;


	//Initialize motheri with 9 random values the first time */
	sNumber=*pSeed&m16Mask;   /* The low 16 bits */
	number=*pSeed&m31Mask;   /* Only want 31 bits */

	p=mother1;
	for (n=18;n--;) {
			number=30903*sNumber+(number>>16);   
			*p++=sNumber=number&m16Mask;
			if (n==9)
					p=mother2;
	}
	/* make cary 15 bits */
	mother1[0]&=m15Mask;
	mother2[0]&=m15Mask;
	ostringstream cout;
	cout << "The Random Generator mother-of-all is re-initialized with seed-value "<< seed<< " ! " <<endl;
	Sys_Common_Output::output_fpl->output_txt(cout.str(),true);
}
//Generation algorithm for zikurat
double Fpl_Random_Generator_MofAll::run_generator(void){
	double result;
	unsigned long number1;
	unsigned long number2;
	unsigned long pSeed;
    /* Move elements 1 to 8 to 2 to 9 */
    memcpy((char*)mother1+2,(char*)mother1+1,8*sizeof(short));
    memcpy((char*)mother2+2,(char*)mother2+1,8*sizeof(short));

     /* Put the carry values in numberi */
    number1=mother1[0];
    number2=mother2[0];

    /* Form the linear combinations */
	number1+= 1941 * mother1[2] + 1860 * mother1[3] +
		  1812 * mother1[4] + 1776 * mother1[5] +
	  	  1492 * mother1[6] + 1215 * mother1[7] + 
		  1066 * mother1[8] + 12013 * mother1[9];

	number2 += 1111 * mother2[2] + 2222 * mother2[3] + 
		   3333 * mother2[4] + 4444 * mother2[5] + 
		   5555 * mother2[6] + 6666 * mother2[7] + 
		   7777 * mother2[8] + 9272 * mother2[9];

     /* Save the high bits of numberi as the new carry */
    mother1[0]=number1/m16Long;
    mother2[0]=number2/m16Long;
            /* Put the low bits of numberi into motheri[1] */
    mother1[1]=m16Mask&number1;
    mother2[1]=m16Mask&number2;

      /* Combine the two 16 bit random numbers into one 32 bit */
    pSeed=(((long)mother1[1])<<16)+(long)mother2[1];

      /* Return a double value between 0 and 1 */
    result= ((double)pSeed)/m32Double;

	return result;
}