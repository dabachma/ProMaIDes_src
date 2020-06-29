#pragma once
/**\class Cost_System
	\author Daniel Bachmann et al.
	\author produced by the Institute of Hydraulic Engineering (IWW), RWTH Aachen University
	\version 0.0.1                                                              
	\date 2009 
*/
#ifndef COST_ECN_COST_SYSTEM_H
#define COST_ECN_COST_SYSTEM_H
//QT libs

//system sys
#include "Common_Const.h"
#include "_Sys_Common_System.h"
#include "Sys_Diverse_Text_Dia.h"

//system cost
#include "CostGui_Alternative_Ecn_Cost_Dia.h"

//system alt
#include "Alt_System.h"

///Class for the  evaluation of a alternative's economical costs  \ingroup cost
/**


*/
class Cost_Ecn_Cost_System
{
public:
	///Default constructor
	Cost_Ecn_Cost_System(void);
	///Default destructor
	~Cost_Ecn_Cost_System(void);

	//members
	///Pointer to the table for storing the economical cost of an alternative
	static Tables *table_cost;

	//method
	///Set the database table for the cost: it sets the table name and the name of the columns and allocate them
	static void set_table_cost(QSqlDatabase *ptr_database);
	///Create the database table for cost
	static void create_table_cost(QSqlDatabase *ptr_database);
	///Delete all data in the database table of the cost
	static void delete_data_in_table_cost(QSqlDatabase *ptr_database);
	///Delete all data in the database table of the cost for the specified values
	static void delete_data_in_table_cost(QSqlDatabase *ptr_database, const _sys_system_id id);
	///Delete data in the database table of the cost for the specified values
	static void delete_data_in_table_cost(QSqlDatabase *ptr_database, const double areastate);
	///Select data in the database table of the cost for the specified values
	static bool select_data_in_cost_table(QSqlDatabase *ptr_database, const _sys_system_id id,_cost_ecn_cost_data *data);
	///Check if data in the database table of the cost for the specified values are set
	static bool check_data_in_cost_table(QSqlDatabase *ptr_database, const _sys_system_id id);
	
	///Get the total cost of the database table for the specified parameters
	static bool get_total_cost_table(QSqlDatabase *ptr_database, const _sys_system_id id, double *total);
	///Close and delete the database table for the cost
	static void close_table_cost(void);

	///Set the cost per dialog
	bool set_cost_per_dialog(QWidget *parent, QSqlDatabase *ptr_database, const _sys_system_id id);

private:

	//methods
	///Calculate the cost-structure
	void calculate_cost_structure(_cost_ecn_cost_data *data);

	///Initialize the cost structure
	void init_cost_struct(_cost_ecn_cost_data *init);

	///Calculate planning cost after HOAI
	double calculate_planning_cost_hoai(const double build_cost);

	///Set cost data to database table
	void set_cost_data2table(QSqlDatabase *ptr_database, _sys_system_id id, const _cost_ecn_cost_data data);

	///Set warning(s)
	Warning set_warning(const int warn_type);
	///Set error(s)
	Error set_error(const int err_type);


};
#endif
