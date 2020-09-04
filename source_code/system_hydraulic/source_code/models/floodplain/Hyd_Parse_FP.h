#pragma once
/**\class Hyd_Parse_FP
	\author Daniel Bachmann et al.
	\author produced by the Institute of Hydraulic Engineering (IWW), RWTH Aachen University
	\version 0.0.1                                                              
	\date 2009 
*/
#ifndef HYD_PARSE_FP_H
#define HYD_PARSE_FP_H


//hyd_system class
#include "_Hyd_Parse_IO.h"
#include "Hyd_Param_FP.h"
///Class for parsing the general input parameter in a file for a floodplain model \ingroup hyd
/**
The general parameter are stored in Hyd_Param_FP.

\see Hyd_Model_Floodplain, Hyd_Param_FP
*/
class Hyd_Parse_FP : public _Hyd_Parse_IO 
{
public:
	///Default constructor
	Hyd_Parse_FP(void);
	///Default destructor
	~Hyd_Parse_FP(void);

	//members

	//method
	///Parse for the global floodplainmodel keywords in the rivermodel parameter file and read in the parameters for the river model with the given index
	void parse_floodplainmodel_params(const int fp_index);
	///Get the globals flodplain model parameters, which are read in per file parser
	Hyd_Param_FP get_flooplainmodel_params(void);


private:
	//members
	///Container for all global floodplainmodel parameters; it is filled per fileparser
	Hyd_Param_FP fp_params;

	///Foundflag for the floodplain file name
	bool found_floodplainfile_name;
	///Foundflag for number of elements in x-direction
	bool found_no_elem_x;
	///Foundflag for number of elements in y-direction
	bool found_no_elem_y;
	///Foundflag for the element-width in x-direction
	bool found_elem_width_x;
	///Foundflag for the element-width in y-direction
	bool found_elem_width_y;
	///Foundflag for the angle to the normal x-direction
	bool found_angle;
	
	//methods
	///Parse for general settings
	void parse_general(_hyd_keyword_file Key, word Command);
	///Parse for the name of the floodplain file
	void parse_floodplain_filename(_hyd_keyword_file Key, word Command);
	///Parse for the instationary_boundary_file name and number of curves
	void parse_instat_boundary(_hyd_keyword_file Key, word Command);
	///Parse for the file and the number of noflow polygons (enclosing noflow cells)
	void parse_no_flow(_hyd_keyword_file Key, word Command);
	///Parse for the file and the number of dikelines 
	void parse_dikelines(_hyd_keyword_file Key, word Command);
	///Parse for the calculation limits
	void parse_calculation_limits(_hyd_keyword_file Key, word Command);
	///Parse for the 2d_result_file name for tecplot
	void parse_2d_result_file(_hyd_keyword_file Key, word Command);

	///Check if the requiered paramaters were found
	void check_parameters_found(void);


	///Set the warning
	Warning set_warning(const int warn_type);
	///Set error(s)
	Error set_error(const int err_type);


};
#endif