#pragma once
/**\class Madm_Solver_Saw
	\author Daniel Bachmann, Matthias Kufeld et al.
	\author produced by the Institute of Hydraulic Engineering (IWW), RWTH Aachen University
	\version 0.0.1                                                              
	\date 2009 
*/
#ifndef MADM_SOLVER_SAW_H
#define MADM_SOLVER_SAW_H


//system sys

//system madm
#include "_Madm_Solver.h"

///Class for the evaluation of a decision matrix via the SAW-approach \ingroup madm
/**
Simple additive weighting method (SAW)

*/
class Madm_Solver_Saw : public _Madm_Solver
{
public:
	///Default constructor
	Madm_Solver_Saw(void);
	///Default destructor
	~Madm_Solver_Saw(void);

	//method
	///Set normalisation type
	void set_normalisation_type(const _madm_normalisation_type type);

	///Solve the matrix
	void solve_matrix(void);

private:

	//method
	///Set warning(s)
	Warning set_warning(const int warn_type);
};
#endif
