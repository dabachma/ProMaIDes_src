#pragma once
/**\class Hyd_Parse_Coupling_FP2CO_Dikebreak
	\author Daniel Bachmann et al.
	\author produced by the Institute of Hydraulic Engineering (IWW), RWTH Aachen University
	\version 0.0.1                                                              
	\date 2011 
*/
#ifndef HYD_PARSE_COUPLING_FP2CO_DIKEBREAK_H
#define HYD_PARSE_COUPLING_FP2CO_DIKEBREAK_H

//hyd_system class
//base class for the input per file
#include "_Hyd_Parse_IO.h"

//struct
///Structure for gathering the parameters for a breach start for a coupling of a coast to a floodplain model \ingroup hyd
struct _hyd_fp2co_breach_start_parameters{
	///Start height [m]
	double start_height;
	///Maximal breach width
	double max_breach_width;
	///Index of the related point of the coast line
	int id_related_point;
	///Distance to the related point of the coast line [m]
	double distance2related;
};



///Class for parsing the input parameter in a file for a coupling between a coast model and a floodplain model via a dikebreak \ingroup hyd
/**

*/
class Hyd_Parse_Coupling_FP2CO_Dikebreak : public _Hyd_Parse_IO
{
public:
	///Default constructor
	Hyd_Parse_Coupling_FP2CO_Dikebreak(void);
	///Default destructor
	~Hyd_Parse_Coupling_FP2CO_Dikebreak(void);


	//method
	///Parse for the coupling parameters for a coupling of a coast- and floodplain-model via a dikebreak; the index specifies number in the file 
	void parse_params(const int index);

	///Get the read in coupling parameters for a coupling of a coast- and floodplain-model via a dikebreak
	_hyd_fp2co_breach_start_parameters get_params(void);


private:

	//members
	///The read in parameters of Hyd_Parse_Coupling_FP2CO_Dikebreak are gathered here
	_hyd_fp2co_breach_start_parameters params;

	///Flag if the location parameters are found
	bool found_location;

	//methods
	///Parse for the location of the breach in the coast-line 
	void parse_location_parameter(_hyd_keyword_file Key, word Command);
	///Check if the requiered paramaters were found
	void check_parameters_found(void);

	///Set error(s)
	Error set_error(const int err_type);


};
#endif
