#pragma once
/**\class Madm_Solver_Topsis
	\author Daniel Bachmann, Matthias Kufeld et al.
	\author produced by the Institute of Hydraulic Engineering (IWW), RWTH Aachen University
	\version 0.0.1                                                              
	\date 2009 
*/
#ifndef MADM_SOLVER_TOPSIS_H
#define MADM_SOLVER_TOPSIS_H


//system sys


//system madm
#include "_Madm_Solver.h"

///Class for the evaluation of a decision matrix via the TOPSIS-approach \ingroup madm
/**
Technique for Order Preference by Similarity to Ideal Solution (TOPSIS)

*/
class Madm_Solver_Topsis : public _Madm_Solver
{
public:
	///Default constructor
	Madm_Solver_Topsis(void);
	///Default destructor
	~Madm_Solver_Topsis(void);

	//method
	///Set the decision matrix
	void set_decision_matrix(Madm_Decision_Matrix *matrix);

	///Solve the matrix
	void solve_matrix(void);

private:

	//members
	///Ideal best solution
	double *ideal_best;
	///Ideal worst solution
	double *ideal_worst;

	///Distances of the alternative to the best ideal solution
	double *distance2ideal_best;
	///Distances of the alternative to the worst ideal solution
	double *distance2ideal_worst;

	//method
	///Allocate the ideal solutions (best case/worst case) per criteria
	void allocate_ideal_solution(void);
	///Delete the ideal solutions
	void delete_ideal_solution(void);

	///Allocate the distances of the alternatives to the ideal solution
	void allocate_dist2ideal(void);
	///delete the distances of the alternatives to the ideal solution
	void delete_dist2ideal(void);

	///Initialize and calculate the ideal solutions per criteria
	void init_ideal_solution(void);
	///Initialize and calculate the distance of the alternatives to the ideal solutions per alternative
	void init_dist2ideal_solution(void);

	///Set error(s)
	Error set_error(const int err_type);
};
#endif
