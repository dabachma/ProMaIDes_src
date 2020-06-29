#pragma once
/**\class Dam_Sc_Subcategory
	\author Daniel Bachmann et al.
	\author produced by the Institute of Hydraulic Engineering (IWW), RWTH Aachen University
	\version 0.0.1                                                              
	\date 2009 
*/
#ifndef DAM_SC_SUBCATEGORY_H
#define DAM_SC_SUBCATEGORY_H

//dam-libs

///Class for the categories of the simple counting damage \ingroup dam
/**

*/
class Dam_Sc_Subcategory
{
public:
	///Default constructor
	Dam_Sc_Subcategory(void);
	///Default destructor
	~Dam_Sc_Subcategory(void);

	//members

	///Pointer to the table for the simple-counting subcategories in a database
	static Tables *table;

	//method

	//Transfer the subcategory data to a database
	void transfer_input_members2database(QSqlDatabase *ptr_database);

	///Get the name of the subcategory
	string get_subcategory_name(void);
	///Get the index of the subcategory
	int get_id(void);

	///Input the simple counting subcategory data per database
	void input_subcategory_perdatabase(const QSqlQueryModel *results, const int glob_index);

	///Create the database table for simple counting subcategories
	static void create_table(QSqlDatabase *ptr_database);
	///Set the database table for simple counting subcategories: it sets the table name and the name of the columns and allocate them
	static void set_table(QSqlDatabase *ptr_database);
	///Delete all data in the database table for simple counting subcategories
	static void delete_data_in_table(QSqlDatabase *ptr_database);
	///Close and delete the database table for simple counting subcategories
	static void close_table(void);

	///Select and count the number of relevant subcategories in a database table
	static int select_relevant_subcategory_database(QSqlQueryModel *results, QSqlDatabase *ptr_database);


	///Output the members
	void output_members(ostringstream *cout);


	///Set members
	void set_members(const int id, const string name);

	///Add to affected score
	void add_affected_score(const double add);
	///Reset affected score
	void reset_affected_score(void);
	///Get affected score
	double get_affected_score(void);


private:

	//members

	///Name of the subcategory
	string name;
	///Index of the category
	int index;

	///Affected score per subcategory
	double affected_score;

	//methods
	///Set error(s)
	Error set_error(const int err_type);
};
#endif
