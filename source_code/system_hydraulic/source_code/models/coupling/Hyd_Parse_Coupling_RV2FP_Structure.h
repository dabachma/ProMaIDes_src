#pragma once
/**\class Hyd_Parse_Coupling_RV2FP_Structure
	\author Daniel Bachmann et al.
	\author produced by the Institute of Hydraulic Engineering (IWW), RWTH Aachen University
	\version 0.0.1                                                              
	\date 2009 
*/
#ifndef HYD_PARSE_COUPLING_RV2FP_STRUCTURE_H
#define HYD_PARSE_COUPLING_RV2FP_STRUCTURE_H


//hyd_system class
//base class for the input per file
#include "_Hyd_Parse_IO.h"
//base class of the coupling structures
#include "_Hyd_Coupling_Structure.h"
//Types of coupling structures
#include "Hyd_Coupling_Structure_Weir.h"
#include "Hyd_Coupling_Structure_Gate.h"


///Data structure where the parameter of coupling between a river model and a floodplain model via a hydraulic structure (weir, gate) are gathered by the parser \ingroup hyd
struct _hyd_rv2fp_struct_coupling_params{
	///Number of the river model 
	int rv_model_no;
	///Distance beginning from the given profile index of the river along the bank line related to the given profile index, where the coupling is allocated
	double distance;
	///Profile index, where the distance is related to
	int profile_index;
	///Type of the coupling structure
	_hyd_1d_outflow_types type;
	///River bank side, where the of coupling is located in flow direction (true:= left; false:=right)
	bool left_flag;
};

///Class for parsing the input parameter in a file for a coupling between a river model and a floodplain model via a hydraulic structure (weir or gate) \ingroup hyd
/**

*/
class Hyd_Parse_Coupling_RV2FP_Structure : public _Hyd_Parse_IO
{
public:
	///Default constructor
	Hyd_Parse_Coupling_RV2FP_Structure(void);
	///Default destructor
	~Hyd_Parse_Coupling_RV2FP_Structure(void);


	//method
	///Parse for the coupling parameters for a coupling of a river- and floodplain-model via a structure; the index specifies number in the file 
	void parse_params(const int index);
	///Get the read in coupling parameters for a coupling of a river- and floodplain-model via a structure
	_hyd_rv2fp_struct_coupling_params get_params(void);
	///Get the parameter of the coupling structure
	_hyd_coupling_structure_data get_coupling_structure_data(void);

private:
	//members
	///The read in parameters of Hyd_Parse_Coupling_RV2FP_Structure are gathered here
	_hyd_rv2fp_struct_coupling_params params;

	///The read in parameters of the _Hyd_Coupling_Structure are gathered here
	_hyd_coupling_structure_data structure_param;

	///Flag if the number of the river model  is found
	bool found_rvmodel;
	///Flag if the parameters for the coupling structure sre found
	bool found_couplingstructure;


	//methods
	///Parse for the parameters of the coupling structure
	void parse_coupling_structure_parameter(_hyd_keyword_file Key, word Command);
	///Parse for the outflow parameters (location in the river model; it is the outflow model)
	void parse_outflow_parameter(_hyd_keyword_file Key, word Command);


	///Check if the requiered paramaters were found
	void check_parameters_found(void);

	///Set error(s)
	Error set_error(const int err_type);


};
#endif