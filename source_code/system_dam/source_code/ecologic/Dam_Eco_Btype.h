#pragma once
/**\class Dam_Eco_Btype
	\author Daniel Bachmann et al.
	\author produced by the Institute of Hydraulic Engineering (IWW), RWTH Aachen University
	\version 0.0.1                                                              
	\date 2009 
*/

//dam-lib
//biotope-type damage function
#include "Dam_Eco_Btype_Damage_Function.h"



#ifndef DAM_ECO_BTYPE_H
#define DAM_ECO_BTYPE_H
///Class for an ecological biotope-type \ingroup dam
/**

*/
class Dam_Eco_Btype
{
//class for displaying the data
friend class Sys_Data_Tree_Wid;
//class for displaying the data as widget
friend class DamGui_Eco_Btype_Wid;

public:
	///Default constructor
	Dam_Eco_Btype(void);
	///Default destructor
	~Dam_Eco_Btype(void);

	//member
	///Pointer to the table for the biotope-type general information in a database
	static Tables *table;

	//method
	///Get the index of the biotope-type
	int get_index(void);
	///Get the name of the biotope-type
	string get_name(void);
	///Get the risk factor
	int get_risk_factor(void);
	///Get the cost-value of the biotope-type
	double get_cost_value(void);

	///Output the members of the biotope-type to display/console
	void output_member(void);

	///Read in one ecologic biotope-type from file
	void read_biotope_type_per_file(ifstream *ifile, int *line_counter);
	///Transfer the biotope-type data to a database: the general settings of the biotope-type as well as the damage functions
	void transfer_input_members2database(const int id_glob, QSqlTableModel *model, QSqlDatabase *ptr_database);
	///Add biotope-type data to a database: the general settings of the functions (biotope-type information) as well as the function points
	void add_input_members2database(QSqlDatabase *ptr_database);
	///Input the the biotope-type data to a database: the general settings of the biotope-type as well as the damage functions
	void input_biotope_type_perdatabase(const QSqlTableModel *results, const int glob_index);

	///Calculate the damages to a biotope-type by a given impact value
	double calculate_damages(Dam_Impact_Values *impact);

	///Create the database table for the general information of the biotope-type
	static void create_table(QSqlDatabase *ptr_database);
	///Set the database table for the general information of the biotope-type: it sets the table name and the name of the columns and allocate them
	static void set_table(QSqlDatabase *ptr_database);
	///Delete all data in the database table for the general information of the biotope-type
	static void delete_data_in_table(QSqlDatabase *ptr_database);
	///Close and delete the database table for the general information of the biotope-type
	static void close_table(void);
	///Get the maximum value of the global index of the biotope-type database table
	static int get_max_glob_id_biotope_table(QSqlDatabase *ptr_database);

	///Select and count the number of relevant biotope-types in a database table
	static int select_relevant_biotope_types_database(QSqlTableModel *results, const bool with_output = true);

	///Select and count the number of relevant biotope-types in a database table by a given biotope-type
	static int select_relevant_biotope_types_database(QSqlTableModel *results, const int btype_id);

	///Get a pointer to the bitope-type function by a given index
	Dam_Eco_Btype_Damage_Function* get_ptr_btype_function(const int index);


private:
	//members
	///Name of the biotpe-type
	string name;
	///Id of the biotope-type
	int id;
	///Value [€/m²] of the biotope-type
	double cost;

	///Number of damage function
	int number_dam_function;
	///Pointer to the biotope-type damage functions
	Dam_Eco_Btype_Damage_Function *dam_function;


	///Risk factor between 1 to 5
	int risk_factor;

	///Slope for the linear weighting of the three impact values
	const double m;
	///Constant factor for the linear weighting of the three impact values
	const double b;

	//methods
	///Allocate the damage function(s)
	void allocate_damage_func(void);
	///Delete the damage function(s)
	void delete_damage_func(void);

	///Check if a biotope-type id is already in use; it this is the case return a new biotope-type id
	int check_biotope_type_id_is_used(QSqlDatabase *ptr_database, const int id);

	///Set error(s)
	Error set_error(const int err_type);
	///Set warning(s)
	Warning set_warning(const int warn_type);

};
#endif
