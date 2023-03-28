#pragma once
/**\class HydTemp_Param
	\author Daniel Bachmann et al.
	\author produced by the AG-FRM University of applied sciences Magdeburg-Stendal
	\version 0.0.1                                                              
	\date 2023 
*/
#ifndef HYDTEMP_PARSE_H
#define HYDTEMP_PARSE_H

//hyd_system class
#include "_Hyd_Parse_IO.h"
#include "HydTemp_Param.h"

///Class for parsing the general input parameter in a file for a temperatur model \ingroup hyd
/**
The general parameter are stored in HydTemp_Param.

\see HydTemp_Model, HydTemp_Param
*/
class HydTemp_Parse : public _Hyd_Parse_IO 
{
public:
	///Default constructor
	HydTemp_Parse(void);
	///Default destructor
	~HydTemp_Parse(void);
	
	//members

	//methods
	///Parse for the global temperature model keywords in the rivermodel parameter file and read in the parameters for the river model with the given index
	void parse_tempmodel_params(const int rv_index);
	///Get the globals temperature model parameters, which are read in per file parser
	HydTemp_Param get_tempmodel_params(void);

//private:

    //members
	///Container for all global temperatur model parameters; it is filled per fileparser
	HydTemp_Param temp_params;

	///Flag if the temperature profile data file is found
	bool found_temp_profile_data_file;

	//method
	///Parse for the temperature river information
	void parse_temp_infos(_hyd_keyword_file Key, word Command);

	///Check if the requiered paramaters were found
	void check_parameters_found(void);

	///Set the warning
	Warning set_warning(const int warn_type);
	///Set error(s)
	Error set_error(const int err_type);
		
};
#endif