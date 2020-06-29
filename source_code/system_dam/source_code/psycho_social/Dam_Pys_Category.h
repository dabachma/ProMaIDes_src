#pragma once
/**\class Dam_Pys_Category
	\author Daniel Bachmann et al.
	\author produced by the Institute of Hydraulic Engineering (IWW), RWTH Aachen University
	\version 0.0.1                                                              
	\date 2009 
*/
#ifndef DAM_PYS_CATEGORY_H
#define DAM_PYS_CATEGORY_H

//dam-libs
#include "Dam_Pys_Function.h"

///Class for the managing of the psycho-social category constisting of score-functions and rasters \ingroup dam
/**

*/
class Dam_Pys_Category
{
//class for displaying the data
friend class Sys_Data_Tree_Wid;
//class for displaying the data in a widget
friend class DamGui_Pys_Categories_Wid;

public:
	///Default constructor
	Dam_Pys_Category(void);
	///Default destructor
	~Dam_Pys_Category(void);

	//members
	///Pointer to the score functions
	Dam_Pys_Function *score_function;

	//method
	///Get the index of the category
	int get_index(void);
	///Get the name of the category
	string get_name(void);

	///Output the members
	void output_member(void);

	///Read in one psycho-social category from file
	void read_category_per_file(ifstream *ifile, int *line_counter);
	///Transfer the psycho-social category data to a database: the general settings of the psycho-social category as well as the score function
	void transfer_input_members2database(const int id_glob, QSqlTableModel *model, QSqlDatabase *ptr_database);
	///Add psycho-social category data to a database: the general settings of the psycho-social category as well as the function points
	void add_input_members2database(QSqlDatabase *ptr_database);
	///Input the the psycho-social category data to a database: the general settings of the psycho-social category as well as the score functions
	void input_category_perdatabase(const QSqlTableModel *results, const int glob_index);

	///Get the score value for a given criteria and the corresponding value
	double get_score_value(const double value, const _dam_pys_criteria crit);

	///Create the database table for psycho-social categories
	static void create_table(QSqlDatabase *ptr_database);
	///Set the database table for psycho-social categories: it sets the table name and the name of the columns and allocate them
	static void set_table(QSqlDatabase *ptr_database);
	///Delete all data in the database table for psycho-social categories
	static void delete_data_in_table(QSqlDatabase *ptr_database);
	///Close and delete the database table for psycho-social categories
	static void close_table(void);
	///Get the maximum value of the global index of the category database table
	static int get_max_glob_id_category_table(QSqlDatabase *ptr_database);

	///Select and count the number of relevant psycho-social categories in a database table
	static int select_relevant_category_database(QSqlTableModel *results, const bool with_output = true);

	///Select and count the number of relevant psycho-social categories in a database table by a given category identifier
	static int select_relevant_category_database(QSqlTableModel *results, const int id);

	///Set the score criteria function to a plot widget
	void set_score_function2plot_widget(Sys_Plot_Wid *widget, _dam_pys_criteria criteria);

private:

	///Number of categories
	int no_score_func;

	///Name of the category
	string name;

	///Index of the category
	int index;

	///Pointer to the table for the psycho-social categories in a database
	static Tables *table;

	//methods

	///Allocate the score function
	void allocate_score_function(void);
	///Delete the score function
	void delete_score_function(void);

	///Check if a category id is already in use; it this is the case return a new category id
	int check_category_id_is_used(QSqlDatabase *ptr_database, const int id);

	///Set error(s)
	Error set_error(const int err_type);
	///Set warning(s)
	Warning set_warning(const int warn_type);
};
#endif
