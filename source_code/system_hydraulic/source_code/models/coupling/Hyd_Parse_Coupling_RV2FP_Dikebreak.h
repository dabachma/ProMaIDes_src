#pragma once
/**\class Hyd_Parse_Coupling_RV2FP_Dikebreak
	\author Daniel Bachmann et al.
	\author produced by the Institute of Hydraulic Engineering (IWW), RWTH Aachen University
	\version 0.0.1                                                              
	\date 2009 
*/
#ifndef HYD_PARSE_COUPLING_RV2FP_DIKEBREAK_H
#define HYD_PARSE_COUPLING_RV2FP_DIKEBREAK_H

//hyd_system class
//base class for the input per file
#include "_Hyd_Parse_IO.h"


///Data structure where the parameter of coupling between a river model and a floodplain model via a dikebreak are gathered by the parser \ingroup hyd
struct _hyd_rv2fp_dikebreak_coupling_params{
	///Number of the river model 
	int rv_model_no;
	///Distance beginning from the given profile index of the river along the bank line related to the given profile index, where the coupling is allocated
	double distance;
	///Profile index, where the distance is related to
	int profile_index;
	///River bank side, where the of coupling is located in flow direction (true:= left; false:=right)
	bool left_flag;
	///Waterlevel height related to the base-point of the connected profiles, when the breach begins
	double begin_waterlevel;
	///Maximum breach width
	double max_breach_width;
};

///Class for parsing the input parameter in a file for a coupling between a river model and a floodplain model via a dikebreak \ingroup hyd
/**

*/
class Hyd_Parse_Coupling_RV2FP_Dikebreak : public _Hyd_Parse_IO
{
public:
	///Default constructor
	Hyd_Parse_Coupling_RV2FP_Dikebreak(void);
	///Default destructor
	~Hyd_Parse_Coupling_RV2FP_Dikebreak(void);

	//method
	///Parse for the coupling parameters for a coupling of a river- and floodplain-model via a dikebreak; the index specifies number in the file 
	void parse_params(const int index);

	///Get the read in coupling parameters for a coupling of a river- and floodplain-model via a dikebreak
	_hyd_rv2fp_dikebreak_coupling_params get_params(void);


private:

	//members
	///The read in parameters of Hyd_Parse_Coupling_RV2FP_Dikebreak are gathered here
	_hyd_rv2fp_dikebreak_coupling_params params;

	///Flag if the location parameters are found
	bool found_location;

	//methods
	///Parse for the location of the breach in the river model
	void parse_location_parameter(_hyd_keyword_file Key, word Command);
	///Check if the requiered paramaters were found
	void check_parameters_found(void);

	///Set error(s)
	Error set_error(const int err_type);



};
#endif
