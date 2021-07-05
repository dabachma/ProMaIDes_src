#pragma once
/**\class Hyd_Tables
	\author Daniel Bachmann et al.
	\author produced by the Institute of Hydraulic Engineering (IWW), RWTH Aachen University
	\version 0.0.1                                                              
	\date 2009 
*/
#ifndef HYD_TABLES_H
#define HYD_TABLES_H

//system_sys_ classes
#include "_Sys_Common_System.h"

//Data structure for the dependent and independent values of one table point \ingroup hyd
struct _hyd_table_values{
	///Independent value
	double independentvalue;
	///Dependent value
	double dependingvalue;
};

///Class for the river profile tables \ingroup hyd
/**
In this profile tables the information of a river profile are calculated before the 
numerical calculation to avoid time costly geometrical calculation during the 
numerical simulation. Via interpolation the result is approximated during the
numerical simualtion.

\see _Hyd_River_Profile_Type
*/
class Hyd_Tables
{
public:
	///Default constructor
	Hyd_Tables(void);
	///Default destructor
	~Hyd_Tables(void);

	//methods
	///Set the table name => the table type
	void set_table_type(const string type_name);

	///Allocate the number of points required for the table 
	void allocate_dependent_values(const int number);
	///Set the x-value of table; it must be allocated outside this class with number_values
	void set_independent_values(double *independent_value);
	///Set the x-value of table; it is the depending value of an other table
	void set_independent_values(Hyd_Tables *independent_table);

	///Get the number of values stored in the table
	int get_number_values(void);
	///Set the y-value of the table
	void set_values(const int counter, const double dependingvalue);
	///Get the table values for a given index
	_hyd_table_values get_values(const int counter);
	///Get the interpolated values
	double get_interpolated_values(const double value2interpolation);

	///Output the table and their values to display/console
	void output_table(ostringstream *cout);

	///Clone table
	void clone_tables(Hyd_Tables *table);



private:
	//members

	///y-value of the table, e.g. waterlevel
	double *depending_value; 
	///x-value of table, e.g. area
	double *independent_value;
	///Number of values stored in the table
	int number_values;
	///Indentifier for the old interpolation interval a interpolation request to this table before
	int old_node;
	///Name of the table
	string table_type_name;


	//method
	///Allocate the table values with the given number of values
	void allocate_table(void);
	///Delete the table values 
	void delete_table(void);

	///Extrapolate the given value, if it is outside the table values
	double extrapolate_value(const double value2interpolation);
	///Interpolate the given value upwards from the old interpolation interval
	double interpolateupwards_value(const double value2interpolation);
	///Interpolate the given value downwards from the old interpolation interval
	double interpolatedownwards_value(const double value2interpolation);


	///Set the error(s)
	Error set_error(const int err_type);
};
#endif