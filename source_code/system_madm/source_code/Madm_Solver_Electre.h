#pragma once
/**\class Madm_Solver_Electre
	\author Daniel Bachmann, Matthias Kufeld et al.
	\author produced by the Institute of Hydraulic Engineering (IWW), RWTH Aachen University
	\version 0.0.1                                                              
	\date 2009 
*/
#ifndef MADM_SOLVER_ELECTRE_H
#define MADM_SOLVER_ELECTRE_H


//system sys

//system madm
#include "_Madm_Solver.h"

///Class for the evaluation of a decision matrix via the ELECTRE-approach \ingroup madm
/**
ELimination Et Choix Traduisant la REalité (ELECTRE)

*/
class Madm_Solver_Electre : public _Madm_Solver
{
public:
	///Default constructor
	Madm_Solver_Electre(void);
	///Default destructor
	~Madm_Solver_Electre(void);

	//method

	///Set the decision matrix
	void set_decision_matrix(Madm_Decision_Matrix *matrix);
	///Solve the matrix
	void solve_matrix(void);

private:

	//members
	///Concordance matrix (alternative A is better than alternative B)
	int **concordance_matrix;
	///Discordance matrix (alternative A is worse than alternative B)
	int **discordance_matrix;

	///Concordance index per matrix value (sum of attribute weights for which alternative A is better than alternative B)
	double *concordance_index;
	///Discordance index per matrix value (distance between attribute values for which alternative A is worse than alternative B divided by the total distance between attribute values of alternative A and alternative B 
	double *discordance_index;
	///Total distance between attribute values of alternative A and alternative B per matrix value
	double *diff_norm_attr;

	///Net-concordance index per alternative
	double *net_concordance_indices;
	///Net-discordance index per alternative
	double *net_discordance_indices;

	///Array of the ranked alternatives after concordance values
	_madm_rank_values *concordance_ranking;
	///Array of the ranked alternatives after discordance values
	_madm_rank_values *discordance_ranking;

	//methods
	///Allocate the dis-/concordance matrices
	void allocate_dis_concordance_matrices(void);
	///Delete the dis-/concordance matrices
	void delete_dis_concordance_matrices(void);

	///Allocate the dis-/concordance indices and the distances of the attribute values
	void allocate_dis_concordance_indices(void);
	///Delete the dis-/concordance indices and the distances of the attribute values
	void delete_dis_concordance_indices(void);

	///Allocate the net dis-/concordance indices per alternative
	void allocate_net_dis_concordance_indices(void);
	///Deletethe net dis-/concordance indices per alternative
	void delete_net_dis_concordance_indices(void);

	///Allocate the ranking array
	void allocate_ranking(void);
	///Delete the ranking array
	void delete_ranking_array(void);

	///Initialize and calculate the dis-/concordance matrices
	void init_dis_concordance_matrices(void);
	///Initialize and calculate the dis-/concordance indices
	void init_dis_concordance_indices(void);
	///Initialize and calculate the net dis-/concordance indices per alternative
	void init_net_dis_concordance_indices(void);

	///Calculate final ranking
	void calc_final_ranking(void);


	///Set error(s)
	Error set_error(const int err_type);
	///Set warning(s)
	Warning set_warning(const int warn_type);


};
#endif
