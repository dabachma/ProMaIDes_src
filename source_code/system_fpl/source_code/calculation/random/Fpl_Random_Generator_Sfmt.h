#pragma once
/**\class Fpl_Random_Generator_Sfmt
	\author Daniel Bachmann et al.
	\author produced by the Institute of Hydraulic Engineering (IWW), RWTH Aachen University
	\version 0.0.1                                                              
	\date 2009 
*/

#ifndef FPL_RANDOM_GENERATOR_SFMT_H
#define FPL_RANDOM_GENERATOR_SFMT_H

//libs
#include <emmintrin.h>                 // Define SSE2 intrinsics
//system sys
#include "Common_Const.h"
//system fpl
#include "_Fpl_Random_Generator.h"

//Definition for the random generator
// Choose one of the possible Mersenne exponents.
// Higher values give longer cycle length and use more memory:
//#define MEXP   607
//#define MEXP  1279
//#define MEXP  2281
//#define MEXP  4253
  #define MEXP 11213
//#define MEXP 19937
//#define MEXP 44497

// Define constants for the selected Mersenne exponent:
#if MEXP == 44497
#define SFMT_N    348                  // Size of state vector
#define SFMT_M    330                  // Position of intermediate feedback
#define SFMT_SL1    5                  // Left shift of W[N-1], 32-bit words
#define SFMT_SL2	  3                  // Left shift of W[0], *8, 128-bit words
#define SFMT_SR1    9                  // Right shift of W[M], 32-bit words
#define SFMT_SR2	  3                  // Right shift of W[N-2], *8, 128-bit words
#define SFMT_MASK	  0xeffffffb,0xdfbebfff,0xbfbf7bef,0x9ffd7bff // AND mask
#define SFMT_PARITY 1,0,0xa3ac4000,0xecc1327a   // Period certification vector

#elif MEXP == 19937
#define SFMT_N    156                  // Size of state vector
#define SFMT_M    122                  // Position of intermediate feedback
#define SFMT_SL1   18                  // Left shift of W[N-1], 32-bit words
#define SFMT_SL2	  1                  // Left shift of W[0], *8, 128-bit words
#define SFMT_SR1   11                  // Right shift of W[M], 32-bit words
#define SFMT_SR2	  1                  // Right shift of W[N-2], *8, 128-bit words
#define SFMT_MASK	  0xdfffffef,0xddfecb7f,0xbffaffff,0xbffffff6 // AND mask
#define SFMT_PARITY 1,0,0,0x13c9e684   // Period certification vector

#elif MEXP == 11213
#define SFMT_N    88                   // Size of state vector
#define SFMT_M    68                   // Position of intermediate feedback
#define SFMT_SL1	14                   // Left shift of W[N-1], 32-bit words
#define SFMT_SL2	 3                   // Left shift of W[0], *8, 128-bit words
#define SFMT_SR1	 7                   // Right shift of W[M], 32-bit words
#define SFMT_SR2	 3                   // Right shift of W[N-2], *8, 128-bit words
#define SFMT_MASK	 0xeffff7fb,0xffffffef,0xdfdfbfff,0x7fffdbfd // AND mask
#define SFMT_PARITY 1,0,0xe8148000,0xd0c7afa3 // Period certification vector

#elif MEXP == 4253
#define SFMT_N    34                   // Size of state vector
#define SFMT_M    17                   // Position of intermediate feedback
#define SFMT_SL1	20                   // Left shift of W[N-1], 32-bit words
#define SFMT_SL2	 1                   // Left shift of W[0], *8, 128-bit words
#define SFMT_SR1	 7                   // Right shift of W[M], 32-bit words
#define SFMT_SR2	 1                   // Right shift of W[N-2], *8, 128-bit words
#define SFMT_MASK	 0x9f7bffff, 0x9fffff5f, 0x3efffffb, 0xfffff7bb // AND mask
#define SFMT_PARITY 0xa8000001, 0xaf5390a3, 0xb740b3f8, 0x6c11486d // Period certification vector

#elif MEXP == 2281
#define SFMT_N    18                   // Size of state vector
#define SFMT_M    12                   // Position of intermediate feedback
#define SFMT_SL1	19                   // Left shift of W[N-1], 32-bit words
#define SFMT_SL2	 1                   // Left shift of W[0], *8, 128-bit words
#define SFMT_SR1	 5                   // Right shift of W[M], 32-bit words
#define SFMT_SR2	 1                   // Right shift of W[N-2], *8, 128-bit words
#define SFMT_MASK	 0xbff7ffbf, 0xfdfffffe, 0xf7ffef7f, 0xf2f7cbbf // AND mask
#define SFMT_PARITY 0x00000001, 0x00000000, 0x00000000, 0x41dfa600  // Period certification vector

#elif MEXP == 1279
#define SFMT_N    10                   // Size of state vector
#define SFMT_M     7                   // Position of intermediate feedback
#define SFMT_SL1	14                   // Left shift of W[N-1], 32-bit words
#define SFMT_SL2	 3                   // Left shift of W[0], *8, 128-bit words
#define SFMT_SR1	 5                   // Right shift of W[M], 32-bit words
#define SFMT_SR2	 1                   // Right shift of W[N-2], *8, 128-bit words
#define SFMT_MASK	  0xf7fefffd, 0x7fefcfff, 0xaff3ef3f, 0xb5ffff7f  // AND mask
#define SFMT_PARITY 0x00000001, 0x00000000, 0x00000000, 0x20000000  // Period certification vector

#elif MEXP == 607
#define SFMT_N     5                   // Size of state vector
#define SFMT_M     2                   // Position of intermediate feedback
#define SFMT_SL1	15                   // Left shift of W[N-1], 32-bit words
#define SFMT_SL2	 3                   // Left shift of W[0], *8, 128-bit words
#define SFMT_SR1	13                   // Right shift of W[M], 32-bit words
#define SFMT_SR2	 3                   // Right shift of W[N-2], *8, 128-bit words
#define SFMT_MASK	  0xfdff37ff, 0xef7f3f7d, 0xff777b7d, 0x7ff7fb2f  // AND mask
#define SFMT_PARITY 0x00000001, 0x00000000, 0x00000000, 0x5986f054  // Period certification vector
#endif

// Define integer types with known size: int32_t, uint32_t, int64_t, uint64_t.
// If this doesn't work then insert compiler-specific definitions here:
#if defined(__GNUC__)
  // Compilers supporting C99 or C++0x have inttypes.h defining these integer types
  #include <inttypes.h>
  #define INT64_SUPPORTED // Remove this if the compiler doesn't support 64-bit integers
#elif defined(_WIN16) || defined(__MSDOS__) || defined(_MSDOS) 
   // 16 bit systems use long int for 32 bit integer
  typedef   signed long int int32_t;
  typedef unsigned long int uint32_t;
#elif defined(_MSC_VER)
  // Microsoft have their own definition
  typedef   signed __int32  int32_t;
  typedef unsigned __int32 uint32_t;
  typedef   signed __int64  int64_t;
  typedef unsigned __int64 uint64_t;
  #define INT64_SUPPORTED // Remove this if the compiler doesn't support 64-bit integers
#else
  // This works with most compilers
  typedef signed int          int32_t;
  typedef unsigned int       uint32_t;
  typedef long long           int64_t;
  typedef unsigned long long uint64_t;
  //#define INT64_SUPPORTED // Remove this if the compiler doesn't support 64-bit integers
#endif

///Random generator class, where the sfmt algorithm (Fog, Saito, Matsumoto) is implemented  \ingroup fpl
/**
Following is written in the source-code of smft:
In this class the random generator sfmt is implemented (c++-Code). It generate uniformly distributed
random number (0,1].
Folowing information about the random generator sfmt is given in the code:
<i>
* Authors:
* Mutsuo Saito (Hiroshima University)
* Makoto Matsumoto (Hiroshima University)
* Agner Fog (Copenhagen University College of Engineering)
* Date created:  2006
* Last modified: 2009-02-08
* Project:       randomc
* Platform:      This C++ version requires an x86 family microprocessor 
*                with the SSE2 or later instruction set and a compiler 
*                that supports intrinsic functions.
* Source URL:    www.agner.org/random
* Source URL for original C language implementation:
*                www.math.sci.hiroshima-u.ac.jp/~m-mat/MT/SFMT/index.html
*
* Description:
* "SIMD-oriented Fast Mersenne Twister" (SFMT) random number generator.
* The SFMT random number generator is a modification of the Mersenne Twister 
* with improved randomness and speed, adapted to the SSE2 instruction set.
* The SFMT was invented by Mutsuo Saito and Makoto Matsumoto.
* The present C++ implementation is by Agner Fog.
*
* Class description and member functions: See sfmt.h
*
* Example:
* ========
* The file EX-RAN.CPP contains an example of how to generate random numbers.
*
* Library version:
* ================
* An optimized version of this random number generator is provided as function
* libraries in randoma.zip. These function libraries are coded in assembly
* language and support only x86 platforms, including 32-bit and 64-bit
* Windows, Linux, BSD, Mac OS-X (Intel based). Use randoma.h from randoma.zip
*
*
* Further documentation:
* ======================
* See the file ran-instructions.pdf for detailed instructions and documentation
*
*
* Copyright notice
* ================
* GNU General Public License http://www.gnu.org/licenses/gpl.html
* This C++ implementation of SFMT contains parts of the original C code
* which was published under the following BSD license, which is therefore
* in effect in addition to the GNU General Public License.
*
Copyright (c) 2006, 2007 by Mutsuo Saito, Makoto Matsumoto and Hiroshima University.
Copyright (c) 2008 by Agner Fog.
All rights reserved.
Redistribution and use in source and binary forms, with or without 
modification, are permitted provided that the following conditions are met:
    > Redistributions of source code must retain the above copyright notice, 
      this list of conditions and the following disclaimer.
    > Redistributions in binary form must reproduce the above copyright notice, 
      this list of conditions and the following disclaimer in the documentation
      and/or other materials provided with the distribution.
    > Neither the name of the Hiroshima University nor the names of its 
      contributors may be used to endorse or promote products derived from 
      this software without specific prior written permission.
THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
"AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
<\i>
*/
  class Fpl_Random_Generator_Sfmt : public _Fpl_Random_Generator
{
public:
	///Default constructor
	Fpl_Random_Generator_Sfmt(void);
	///Default destructor
	~Fpl_Random_Generator_Sfmt(void);


	//method

	///Initialize the random generator
	static void reinit_generator(long int seed);
	///Initialize the random generator, with always the same seed value => same random numbers in genration process
	static void reinit_generator(void);

	///Run the random generator
	double run_generator(void);

private:

	//method
	///Re-seed the generator
	void RandomInit(int seed); 
	///Seed by more than 32 bits
	void RandomInitByArray(int const seeds[], int NumSeeds); 
	///Output random integer
	int IRandom  (int min, int max); 
	///Output random integer, exact
	int IRandomX (int min, int max); 
	///Output random floating point number
	double Random(void); 
	///Output random bits
	uint32_t BRandom(void);                           

	///Various initializations and period certification
	static void Init2(void);
	///Fill state array with new random numbers
	static void Generate(void);                              
	
	//members
	//Index into state array
	static uint32_t ix;                                  
	///Last interval length for IRandom
	uint32_t LastInterval;                        
	///Rejection limit used by IRandom
	uint32_t RLimit;  
	///Mask
	static __m128i  mask;                              
	///State vector for SFMT generator
	static __m128i  state[SFMT_N];                       
	 
};
#endif