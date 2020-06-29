#pragma once
/**\class Fpl_Random_Generator_Zikurat
	\author Daniel Bachmann et al.
	\author produced by the Institute of Hydraulic Engineering (IWW), RWTH Aachen University
	\version 0.0.1                                                              
	\date 2009 
*/
#ifndef FPL_RANDOM_GENERATOR_ZIKURAT_H
#define FPL_RANDOM_GENERATOR_ZIKURAT_H

//system sys
#include "Common_Const.h"
//system fpl
#include "_Fpl_Random_Generator.h"

//macros required for the zikurat generator
#define SHR3 (jz=jsr, jsr^=(jsr<<13), jsr^=(jsr>>17), jsr^=(jsr<<5),jz+jsr)
#define UNI (.5 + (signed) SHR3*.2328306e-9)
#define IUNI SHR3

///Random generator class, where the zikurat algorithm (George Marsaglia) is implemented  \ingroup fpl
/**
In this class the random generator zikurat is implemented (c-Code). It generate standard normal variate with mean zero, variance 1.
(comment: REXP is not implemented here!)
Folowing information about the random generator zikurat is given in the code:
 <i> The ziggurat method for RNOR and REXP
Combine the code below with the main program in which you want
normal or exponential variates. Then use of RNOR in any expression
will provide a standard normal variate with mean zero, variance 1,
while use of REXP in any expression will provide an exponential variate
with density exp(-x),x>0. Before using RNOR or REXP in your main, insert a command such as
zigset(86947731 ); with your own choice of seed value>0, rather than 86947731.
(If you do not invoke zigset(...) you will get all zeros for RNOR and REXP.)
For details of the method, see Marsaglia and Tsang, "The ziggurat method
for generating random variables", Journ. Statistical Software.
 </i>
*/
class Fpl_Random_Generator_Zikurat : public _Fpl_Random_Generator
{
public:
	///Default constructor
	Fpl_Random_Generator_Zikurat(void);
	///Default destructor
	~Fpl_Random_Generator_Zikurat(void);


	//methods
	///Initialize the random generator; the seed value is always the same 
	static void reinit_random(void);
	///Initialize the random generator with a given seed value
	static void reinit_random(const long int seed);
	///Generation algorithm for zikurat
	double run_generator(void);


private:
	//members

	///Static variables for random generator zikurat
	static unsigned long jz;
	///Static variables for random generator zikurat
	static unsigned long jsr;
	///Static variables for random generator zikurat
	static long hz;
	///Static variables for random generator zikurat
	static unsigned long iz;
	///Static variables for random generator zikurat
	static unsigned long kn[128];
	///Static variables for random generator zikurat
	static double wn[128];
	///Static variables for random generator zikurat
	static double fn[128];

	///Flag if the random generator is initialized
	static bool flag_zikurat;



	//methods
	///Generates a normal distributed variables [0..1]
	double rnor(void);
	///This procedure sets the seed and creates the tables
	static void zigset(unsigned long jsrseed);

	///Generates variates from the residue when rejection in RNOR occurs
	double nfix(void);

	///Set error(s)
	Error set_error(const int err_type);
};
#endif
