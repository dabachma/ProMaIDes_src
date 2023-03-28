#pragma once
/**\class Madm_Decision_Matrix
	\author Daniel Bachmann et al.
	\author produced by the Institute of Hydraulic Engineering (IWW), RWTH Aachen University
	\version 0.0.1                                                              
	\date 2009 
*/
#ifndef MADM_DECISION_MATRIX_H
#define MADM_DECISION_MATRIX_H


//system sys
#include "_Sys_Common_System.h"
#include "Common_Const.h"

//system alt
#include "Alt_System.h"
//system alt
#include "Risk_System.h"

///Data structure for gathering the criteria specific values \ingroup madm
struct _madm_criteria{
	///Name of the criteria
	string name;
	///Index of the criteria
	int index;
	///Weight of the criteria
	double weight;
	///Minimum-/maximum flag
	bool max_flag;
};

///Data structure for gathering the alternative's specific values \ingroup madm
struct _madm_alternative{
	///Name of the alternative
	string name;
	///Index of the alternative
	int index;
};

///Enumerator to distinguish the type of the normalisation schemes \ingroup madm
enum _madm_normalisation_type{
	///standard normalisation 
	norm_standard,
	///range normalisation 
	norm_range,
	///vectoriell normalisation  
	norm_vector,
	//no normalisation applied
	norm_no,
};

///Class for gathering the data of the decision matrix, like alternatives, criteria and the decision matrix values \ingroup madm
/**
The columns of the matrix represents the criteria, the rows the alternatives. 

*/
class Madm_Decision_Matrix
{
public:
	///Default constructor
	Madm_Decision_Matrix(void);
	///Copy constructor
	Madm_Decision_Matrix(Madm_Decision_Matrix &copy);
	///Default destructor
	~Madm_Decision_Matrix(void);

	//members
	///Pointer to the table for the decision matrix consisting of the alternative's criteria values
	static Tables *table_matrix;

	//method
	///Set the database table for the decision matrix: it sets the table name and the name of the columns and allocate them
	static void set_table_matrix(QSqlDatabase *ptr_database, const bool not_close = false);
	///Create the database table for the decision matrix
	static void create_table_matrix(QSqlDatabase *ptr_database);
	///Delete all data in the database table for the decision matrix
	static void delete_data_in_table_matrix(QSqlDatabase *ptr_database);
	//Delete data in the database table for the decision matrix for the given systemstate 
	static void delete_data_in_table_matrix(QSqlDatabase *ptr_database, const _sys_system_id id);
	///Close and delete the database table for the decision matrix
	static void close_table_matrix(void);
	///Select the data in the database table for madm-matrix for the specified parameters
	static int select_data_in_table_matrix(QSqlQueryModel *model, QSqlDatabase *ptr_database, _risk_type type);



	///Read the matrix in per file
	void read_matrix_per_file(const string filename);
	///Read the matrix in per database
	void read_matrix_per_database(QSqlDatabase *ptr_database, const int area_state, const _risk_type type);
	///Set values of the matrix table 
	bool set_matrix_values2database(QSqlDatabase *ptr_database, const int area_state, const _risk_type type, const int number_alt, _alt_general_info *alts);

	///Clear the matrix
	void clear_matrix(void);

	///Change the weights of the criteria
	void change_weights(const int index, const double weight);

	///Initialize the matrix
	void init_matrix(const int number_alternatives, const int number_criteria);
	///Set a alternative specified by the given index
	void set_alternative(const _madm_alternative alternative, const int index);
	///Set a criteria specified by the given index
	void set_criteria(const _madm_criteria criteria, const int index);

	///Get number of alternatives
	int get_number_alternatives(void);
	///Get the alternative specified by the given index
	_madm_alternative get_alternative(const int index);
	///Get number of criteria
	int get_number_criteria(void);
	///Get the criteria specified by the given index
	_madm_criteria get_criteria(const int index);
	///Get a matrix value specified by the given indices
	double get_matrix_value(const int index_alt, const int index_crit);

	///Apply the weigthing of the matrix (use it after the normalisation!)
	void apply_weighting(void);
	///Set the normalisation type and apply the normalsation
	void apply_matrix_normalisation(const _madm_normalisation_type type);

	///Output matrix to display/console
	void output_matrix(void);
	///Output matrix to file
	void output_matrix(ofstream *outfile);

	///Convert a string into a madm normalisation scheme (_madm_normalisation_type)
	static _madm_normalisation_type convert_txt2norm_scheme(const string txt);
	///Convert a madm approach (_madm_normalisation_type) into a string 
	static string convert_norm_scheme2txt(const _madm_normalisation_type scheme);

	///Get the name of the matrix
	string get_matrix_name(void);

	///Copy operator
	Madm_Decision_Matrix& operator= (const Madm_Decision_Matrix& copy);

private:

	//members

	///Name of the matrix
	string name;

	///Number of criteria
	int number_crit;
	///Array of criteria
	_madm_criteria *crit;

	///Number of alternatives
	int number_alt;
	///Array of alternatives
	_madm_alternative *alt;

	///Matrix values
	double **matrix;

	///Applied normalisation scheme
	_madm_normalisation_type norm_type;

	//methods

	///Allocate the criteria
	void allocate_criteria(void);
	///Delete the criteria
	void delete_criteria(void);

	///Allocate the alternatives
	void allocate_alternatives(void);
	///Delete the alternatives
	void delete_alternatives(void);

	///Allocate the matrix
	void allocate_matrix(void);
	///Delete the matrix
	void delete_matrix(void);

	///Initialize the criteria structure
	void init_crit(_madm_criteria *init);
	///Initialize the alternative structure
	void init_alt(_madm_alternative *init);


	///Normalisation of the matrix after the standard normalisation scheme
	void standard_norm_scheme(void);
	///Normalisation of the matrix after the range normalisation scheme
	void range_norm_scheme(void);
	///Normalisation of the matrix after the vector normalisation scheme
	void vector_scheme(void);

	///Normalisation of the criteria weights
	void crit_weight_normalisation(void);


	///Check the file for the madm-analysis if the !$BEGIN... always are close with !$END...
	void check_input_file_analysis(const string file_name);
	///Read in the general data block for the matrix per file
	void read_in_general_data_file(QFile *ifile, int *line_counter);
	///Read in the criteria data block for the matrix per file
	void read_in_criteria_data_file(QFile *ifile, int *line_counter);
	///Read in the matrix data block for the matrix per file
	void read_in_matrix_data_file(QFile *ifile, int *line_counter);

	///Find the key values for the general data block for an file-input
	void find_keyvalues_general_data_file(const string myline, int *must_found);
	///Find the key values for the criteria data block for an file-input
	void find_keyvalues_criteria_data_file(const string myline, int *must_found);
	///Find the key values for the matrix data block for an file-input
	void find_keyvalues_matrix_data_file(const string myline, int *must_found);
	

	///Set warning(s)
	Warning set_warning(const int warn_type);
	///Set error(s)
	Error set_error(const int err_type);
	

};
#endif
