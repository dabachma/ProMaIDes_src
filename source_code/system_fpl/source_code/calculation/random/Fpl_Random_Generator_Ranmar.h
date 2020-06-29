#pragma once
/**\class Fpl_Random_Generator_Ranmar
	\author Daniel Bachmann et al.
	\author produced by the Institute of Hydraulic Engineering (IWW), RWTH Aachen University
	\version 0.0.1                                                              
	\date 2009 
*/
#ifndef FPL_RANDOM_GENERATOR_RANMAR_H
#define FPL_RANDOM_GENERATOR_RANMAR_H

//system sys
#include "Common_Const.h"
//system fpl
#include "_Fpl_Random_Generator.h"


///Random generator class, where the ranmar algorithm (George Marsaglia) is implemented  \ingroup fpl
/**
In this class also the random generator ranmar is implemnted (c-Code). It generate uniformaly distributed
variables between 0 and 1.
Folowing information about the random generator ranmar is given in the code:
 <i>This random number generator originally appeared in "Toward a Universal 
 Random Number Generator" by George Marsaglia and Arif Zaman. 
 Florida State University Report: FSU-SCRI-87-50 (1987)
 
 It was later modified by F. James and published in "A Review of Pseudo-
 random Number Generators" 
 THIS IS THE BEST KNOWN RANDOM NUMBER GENERATOR AVAILABLE.
   (However, a newly discovered technique can yield 
    a period of 10^600. But that is still in the development stage.)
 It passes ALL of the tests for random number generators and has a period 
   of 2^144, is completely portable (gives bit identical results on all 
   machines with at least 24-bit mantissas in the doubleing point 
   representation). 
 The algorithm is a combination of a Fibonacci sequence (with lags of 97
   and 33, and operation "subtraction plus one, modulo one") and an 
   "arithmetic sequence" (using subtraction).
  This C language version was written by Jim Butler, and was based on a
	FORTRAN program posted by David LaSalle of Florida State University.
	seed_ran1() and ran1() added by John Beale 7/23/95 </i>
*/
class Fpl_Random_Generator_Ranmar : public _Fpl_Random_Generator
{
public:
	///Default constructor
	Fpl_Random_Generator_Ranmar(void);
	///Default destructor
	~Fpl_Random_Generator_Ranmar(void);


	///Generation algorithm for ranmar
	double run_generator(void);
	///Initialize the random generator; the seed value is always the same 
	static void reinit_random(void);
	///Initialize the random generator with a given seed value
	static void reinit_random(const int seed);


private:
	//members
	///Static variables for random generator ranmar (part of ranmar)
	static double u[98];
	///Static variables for random generator ranmar (part of ranmar)
	static double c;
	///Static variables for random generator ranmar (part of ranmar)
	static double cd; 
	///Static variables for random generator ranmar (part of ranmar)
	static double cm;
	///Static variables for random generator ranmar (part of ranmar)
	static int i97;
	///Static variables for random generator ranmar (part of ranmar)
	static int j97;
	///Flag if the generator is initialized (part of ranmar)
	static bool flag_ranmar;

	//methods
	///Initialize the random generator by one integer (part of ranmar)
	static void seed_ran1(const int seed); 
	///Calculate the seed (part of ranmar)
	static void rmarin(const int ij, const int kl);
	///Set error(s)
	Error set_error(const int err_type);
};
#endif
