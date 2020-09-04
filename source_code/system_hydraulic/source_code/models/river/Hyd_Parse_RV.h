#pragma once
/**\class Hyd_Parse_RV
	\author Daniel Bachmann et al.
	\author produced by the Institute of Hydraulic Engineering (IWW), RWTH Aachen University
	\version 0.0.1                                                              
	\date 2009 
*/
#ifndef HYD_PARSE_RV_H
#define HYD_PARSE_RV_H

//hyd_system class
#include "_Hyd_Parse_IO.h"
#include "Hyd_Param_RV.h"

///Class for parsing the general input parameter in a file for a river model \ingroup hyd
/**
The general parameter are stored in Hyd_Param_RV.

\see Hyd_Model_River, Hyd_Param_RV
*/
class Hyd_Parse_RV : public _Hyd_Parse_IO 
{
public:
	///Default constructor
	Hyd_Parse_RV(void);
	///Default destructor
	~Hyd_Parse_RV(void);
	
	//members

	//methods
	///Parse for the global rivermodel keywords in the rivermodel parameter file and read in the parameters for the river model with the given index
	void parse_rivermodel_params(const int rv_index);
	///Get the globals river model parameters, which are read in per file parser
	Hyd_Param_RV get_rivermodel_params(void);

private:
	//members
	///Container for all global rivermodel parameters; it is filled per fileparser
	Hyd_Param_RV rv_params;

	///Foundflag for the number of profiles
	bool found_no_profiles;
	///Foundflag for the geomety_file name
	bool found_geometry_file;

	//method
	///Parse for the number of profiles
	void parse_number_profiles(_hyd_keyword_file Key, word Command);
	///Parse for the geometry_file name (profiles)
	void parse_geometry_file(_hyd_keyword_file Key, word Command);
	///Parse for the instationary_boundary_file name and number of curves
	void parse_instat_boundary(_hyd_keyword_file Key, word Command);
	///Parse for the 1d_result_file name for tecplot
	void parse_1d_result_file(_hyd_keyword_file Key, word Command);
	///Parse for the 2d_result_file name for tecplot
	void parse_2d_result_file(_hyd_keyword_file Key, word Command);
	///Parse for the calculation limits
	void parse_calculation_limits(_hyd_keyword_file Key, word Command);
	///Check if the requiered paramaters were found
	void check_parameters_found(void);

	///Set the warning
	Warning set_warning(const int warn_type);
	///Set error(s)
	Error set_error(const int err_type);
		
};
#endif