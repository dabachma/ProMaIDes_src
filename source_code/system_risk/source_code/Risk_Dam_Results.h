#pragma once
/**\class Risk_Dam_Results
	\author Daniel Bachmann et al.
	\author produced by the Institute of Hydraulic Engineering (IWW), RWTH Aachen University
	\version 0.0.1                                                              
	\date 2010 
*/

#ifndef RISK_DAM_RESULTS_H
#define RISK_DAM_RESULTS_H

//system sys
#include "Common_Const.h"

///Data structure for storing the individuel damage result of one element
struct _risk_dam_elem_result{

	///Raster identifier
	int raster_id;
	///Element identifier
	int elem_id;
	///Damage result
	double dam_result;

};
///Class for storing the damage results \ingroup risk
/**


\see Risk_Damage_Predictor, Dam_Damage_System

*/
class Risk_Dam_Results
{
public:
	///Default constructor
	Risk_Dam_Results(void);
	///Default destructor
	~Risk_Dam_Results(void);


	//method
	///Set and allocate the number of available results
	void set_number_results(const int number);

	///Set the result of one element
	void set_elem_result(const int index, const int raster_id, const int elem_id, const double value);

	///Get the pointer to the result of one element
	_risk_dam_elem_result *get_ptr_elem_result(const int index);

	///Compare to result classes
	void compare_results(Risk_Dam_Results *compare);

	///Get the sum of all values
	double get_sum_value(void);

	///Output the result structure
	void output_dam_result(void);

private:
	//member
	///Number of results
	int number_res;

	///Pointer to the damage results
	_risk_dam_elem_result *results;


	//method
	///Allocate the damage result structure
	void allocate_results(void);
	///Delete the damage result structure
	void delete_results(void);

	///Set error(s)
	Error set_error(const int err_type);
};
#endif
