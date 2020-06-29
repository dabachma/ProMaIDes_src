#pragma once
/**\class _Dam_2value_Function
	\author Daniel Bachmann et al.
	\author produced by the Institute of Hydraulic Engineering (IWW), RWTH Aachen University
	\version 0.0.1                                                              
	\date 2009 
*/

#ifndef _DAM_2VALUE_FUNCTION_H
#define _DAM_2VALUE_FUNCTION_H

//sys-libs
#include "_Sys_Common_System.h"
#include "Sys_Plot_Wid.h"

///Data structure for gathering the point data of a damage function 
struct _dam_function_point{
	///The x-value of the function (x-axis)
	double x_value;
	///The y-value of the function (y-axis)
	double y_value;
};

///Base class for the damage function with an x- and a y-value  \ingroup dam
/**

*/
class _Dam_2value_Function
{
public:
	///Default constructor
	_Dam_2value_Function(void);
	///Default destructor
	virtual ~_Dam_2value_Function(void);


	//methods
	///Read in the damage function per file
	virtual void read_function(ifstream *ifile, int *line_counter)=0;

	///Get the y-value by a x-value
	double get_y_value(const double x_value);
	///Output the members of the function to display/console
	void output_member(void);

	///Set the mebers to a plot widget
	void set_members2plot_wid(Sys_Plot_Wid *widget);


protected:

	//members
	///Number of function points
	int number_points;
	///Pointer to the function points
	_dam_function_point *function_points;
	///Flag if the function is continuous (false) or a step-wise function (true)
	bool stepwise_flag;

	//methods
	///Get a discret y-value by a given x-value (step-function)
	double get_discret_y_value(const double x_value);
	///Get a continuous yvalue by a given x-value (linear interpüolation)
	double get_conti_x_value(const double x_value);

	///Allocate the points of the functions 
	void allocate_points(void);
	///Delete the points of the functions
	void delete_points(void);

	///Check the read in line if there is an !END or !BEGIN
	void check_line(string line, const int line_counter);

	///Check the points of the function
	virtual void check_function(void);

	///Set error(s)
	Error set_error(const int err_type);
};

#endif