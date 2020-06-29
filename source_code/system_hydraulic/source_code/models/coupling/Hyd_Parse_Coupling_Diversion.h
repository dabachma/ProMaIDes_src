#pragma once
/**\class Hyd_Parse_Coupling_Diversion
	\author Daniel Bachmann et al.
	\author produced by the Institute of Hydraulic Engineering (IWW), RWTH Aachen University
	\version 0.0.1                                                              
	\date 2009 
*/
#ifndef HYD_PARSE_COUPLING_DIVERSION_H
#define HYD_PARSE_COUPLING_DIVERSION_H

//hyd_system class
//base class for the input per file
#include "_Hyd_Parse_IO.h"
//base class of the coupling structures
#include "_Hyd_Coupling_Structure.h"

///Data structure where the parameter of Coupling via a diversion channel model are gathered \ingroup hyd
struct _hyd_div_channel_params{
	///Number of the river model which represents the diversion channel model
	int rv_model_no;
	///Number of the river model from which the inflow comes for the diversion channel
	int rv_inflow_no;
	///Type of the coupling structure
	_hyd_1d_outflow_types type;
	///Number of the river model where the outflow goes of the diversion channel
	int rv_outflow_no;

};


///Class for parsing the input parameter in a file for a coupling via a diversion channel (river model) between a river models \ingroup hyd
/**

*/
class Hyd_Parse_Coupling_Diversion : public _Hyd_Parse_IO 
{
public:
	///Default constructor
	Hyd_Parse_Coupling_Diversion(void);
	///Default destructor
	~Hyd_Parse_Coupling_Diversion(void);

	//method
	///Parse for the diversion channel model the keywords and read in the parameters; the index specifies the model number in the file 
	void parse_params(const int index);
	///Get the read in diversion channel parameters 
	_hyd_div_channel_params get_params(void);
	///Get the parameter of the coupling structure
	_hyd_coupling_structure_data get_coupling_structure_data(void);

private:
	//members
	///The read in parameters of Hyd_Coupling_RV2RV_Diversion are gathered here
	_hyd_div_channel_params params;

	///The read in parameters of the _Hyd_Coupling_Structure are gathered here
	_hyd_coupling_structure_data structure_param;

	///Flag if the number of the river model which represents the diversion channel model is found
	bool found_no_rvmodel;
	///Flag if the inflow parameter settings are found
	bool found_inflow;
	///Flag if the parameters for the coupling structure sre found
	bool found_couplingstructure;


	//methods
	///Parse for the number of the river model, which represents the diversion channel
	void parse_no_river(_hyd_keyword_file Key, word Command);
	///Parse for the inflow parameters 
	void parse_inflow_parameter(_hyd_keyword_file Key, word Command);
	///Parse for the parameters of the coupling structure
	void parse_coupling_structure_parameter(_hyd_keyword_file Key, word Command);
	///Parse for the outflow parameters 
	void parse_outflow_parameter(_hyd_keyword_file Key, word Command);

	///Check if the requiered paramaters were found
	void check_parameters_found(void);

	///Set error(s)
	Error set_error(const int err_type);

};

#endif