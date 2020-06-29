#pragma once
/**\class Madm_Analysis
	\author Daniel Bachmann et al.
	\author produced by the Institute of Hydraulic Engineering (IWW), RWTH Aachen University
	\version 0.0.1                                                              
	\date 2009 
*/
#ifndef MADM_ANALYSIS_H
#define MADM_ANALYSIS_H


//system sys
#include "_Sys_Common_System.h"
#include "Common_Const.h"

//system madm
#include "Madm_Solver_Saw.h"
#include "Madm_Solver_Topsis.h"
#include "Madm_Solver_Electre.h"


///Class for the evaluation of a decision matrix based on file- or database input \ingroup madm
/**
	There are three types of madm-analysis implemented:
		- Simple additive weighting method (SAW) (Madm_Analysis_Saw)
		- Technique for Order Preference by Similarity to Ideal Solution (TOPSIS) (Madm_Analysis_Topsis)
		- ELimination Et Choix Traduisant la REalité (ELECTRE) (Madm_Analysis_Electre)

*/
class Madm_Analysis
{
public:
	///Default constructor
	Madm_Analysis(void);
	///Default destructor
	~Madm_Analysis(void);

	//members


	//methods


	///Read in the madm analysis data per file
 	void read_analysis_per_file(const string filename);
	///Read in the madm analysis data per database
 	void read_analysis_per_database(QSqlDatabase *ptr_database, const int number_alt, _alt_general_info *alternatives, const int number_crit, _madm_criteria *criteria, const _risk_type type);

	///Set the values of the decision matrix into database table
	bool set_decision_matrix2database(QSqlDatabase *ptr_database, const int area_state, const _risk_type type, const int number_alt, _alt_general_info *alts);

	///Change the criteria weights of the matrix
	void change_criteria_weight_matrix(const int number_crit, _madm_criteria *criteria);

	///Solve the matrix with the diverse madm-approaches for file-based analysis
	int calculate_matrix_file(void);
	///Solve the matrix with the diverse madm-approaches for database-based analysis
	int calculate_matrix_database(QSqlDatabase *ptr_database, const int area_state, const _risk_type type, const int set_id);

	///Convert a string into a madm approach type (_madm_approach_type)
	static _madm_approach_type convert_txt2madm_approach(const string txt);
	///Convert a madm approach (_madm_approach_type) into a string 
	static string convert_madm_approach2txt(const _madm_approach_type src);
	///Set the counter over the different approaches
	void set_counter_approaches(const int number);


private:

	//members
	///Analysis based on the SAW-approach
	Madm_Solver_Saw analysis_saw;
	///Analysis based on the TOPSIS-approach
	Madm_Solver_Topsis analysis_topsis;
	///Analysis based on the ELECTRE-approach
	Madm_Solver_Electre analysis_electre;

	///Decision-matrix to solve
	Madm_Decision_Matrix matrix;
	
	///Outputfile for the results of a calculation per file
	ofstream outfile;
	///Name and path of the outputfile for the results of a calculation per file
	string outfile_name;

	///Array of the ranked alternatives
	_madm_rank_values *total_ranking;
	///Number of rankings
	int number_total_rank;
	///Counter of the total ranking
	int count_total_ranking;

	//method
	///Allocate the total ranking array
	void allocate_total_ranking(void);
	///Delete the total ranking array
	void delete_total_ranking_array(void);
	///Initialize the total ranking
	void init_total_ranking(void);
	///Sum up total ranking
	void sum_up_total_ranking(_Madm_Solver *solver);
	///Finalize the total ranking
	void final_total_ranking(void);
	///Output total ranking to display/console
	void output_total_ranking(void);
	///Output total ranking to file
	void output_total_ranking(ofstream *outfile);
	///Ouput the total ranking to database
	void output_total_ranking2database(QSqlDatabase *ptr_database, const int area_state, const _risk_type type, const int set_id);


	///Set warning(s)
	Warning set_warning(const int warn_type);
	///Set error(s)
	Error set_error(const int err_type);

};
#endif
