#pragma once
/**\class _Madm_Solver
	\author Daniel Bachmann, Matthias Kufeld et al.
	\author produced by the Institute of Hydraulic Engineering (IWW), RWTH Aachen University
	\version 0.0.1                                                              
	\date 2009 
*/
#ifndef _MADM_SOLVER_H
#define _MADM_SOLVER_H


//system sys
#include "_Sys_Common_System.h"
#include "Common_Const.h"

//system madm
#include "Madm_Decision_Matrix.h"


///Data structure for gathering the ranking values \ingroup madm
struct _madm_rank_values{
	///Alternative
	_madm_alternative alternative;
	///Score value of the alternative
	double score;
	///Rank of the alternative
	int rank;
};

///Enumerator to distinguish the type of the madm-analysis \ingroup madm
enum _madm_approach_type{
	///Simple additive weighting method (standard normalisation) (SAW)
	approach_saw_standard,
	///Simple additive weighting method (range normalisation) (SAW)
	approach_saw_range,
	///Technique for Order Preference by Similarity to Ideal Solution (TOPSIS)
	approach_topsis,
	///ELimination Et Choix Traduisant la REalité (ELECTRE)
	approach_electre,
	///All scores are taken into account
	approach_total,
	///Unknown analysis type
	approach_unknown,

};

///Parent-class to apply the mathematical algorithm of the madm-ranking \ingroup madm
/**
	There are three types of madm-approaches implemented:
		- Simple additive weighting method (SAW) (Madm_Analysis_Saw)
		- Technique for Order Preference by Similarity to Ideal Solution (TOPSIS) (Madm_Analysis_Topsis)
		- ELimination Et Choix Traduisant la REalité (ELECTRE) (Madm_Analysis_Electre)

*/
class _Madm_Solver
{
public:
	///Default constructor
	_Madm_Solver(void);
	///Default destructor
	virtual ~_Madm_Solver(void);

	//members
	///Decision matrix
	Madm_Decision_Matrix decision;
	///Pointer to the table for the ranking of the alternative
	static Tables *table_ranking;

	//method
	
	///Set the database table for the alternative's ranking: it sets the table name and the name of the columns and allocate them
	static void set_table_ranking(QSqlDatabase *ptr_database);
	///Create the database table for the alternative's ranking
	static void create_table_ranking(QSqlDatabase *ptr_database);
	///Delete all data in the database table for the alternative's ranking
	static void delete_data_in_table_ranking(QSqlDatabase *ptr_database);
	///Delete all data in the database table for the alternative's ranking for a given system id
	static void delete_data_in_table_ranking(QSqlDatabase *ptr_database, const _sys_system_id id);
	///Close and delete the database table for the alternative's ranking
	static void close_table_ranking(void);
	///Select the ranking data of the database table for the specific paramters
	static int select_data_in_ranking_table(QSqlQueryModel *model, QSqlDatabase *ptr_database, const int areastate, const int set_id, const _risk_type risktype, const _madm_approach_type ana_type);
	///Check if resluts are available for a given risk type
	static bool check_data_in_ranking_table(QSqlDatabase *ptr_database, const int areastate,  const _risk_type risktype);



	///Set the decision matrix
	virtual void set_decision_matrix(Madm_Decision_Matrix *matrix);

	///Apply the weigthing of the matrix (use it after the normalisation!)
	void apply_weighting(void);
	///Set the normalisation type and apply the normalsation
	void apply_matrix_normalisation(void);

	///Set normalisation type
	virtual void set_normalisation_type(const _madm_normalisation_type type);

	///Solve the matrix
	virtual void solve_matrix(void)=0;

	///Output the ranking to display/console
	void output_ranking(const bool output_detail=false);
	///Output the ranking to file
	void output_ranking(ofstream *outfile);

	///Output the ranking to database
	void output_ranking2database(QSqlDatabase *ptr_database, const int area_state, const _risk_type type, const int set_id); 

	///Get the score values with a given alternative index
	double get_score_value(const int alt_index);
	///Get the rank values with a given alternative index as double
	double get_rank_value(const int alt_index);

	///Sort the alternatives after the score
	static void sort_alternatives(const int number_rank, _madm_rank_values *rank, const bool max_first);

protected:

	//members


	///Type of normalisation
	_madm_normalisation_type norm_type;

	///Array of the ranked alternatives
	_madm_rank_values *ranking;
	///Number of rankings
	int number_rank;

	///Type of madm-solver
	_madm_approach_type solver_type;

	//method
	///Allocate the ranking array
	virtual void allocate_ranking(void);
	///Delete the ranking array
	virtual void delete_ranking_array(void);


private:
	//members


	//method
	///Set warning(s)
	Warning set_warning(const int warn_type);
	///Set error(s)
	Error set_error(const int err_type);
};
#endif
