#pragma once
/**\class Hyd_Coupling_Structure_Weir
	\author Daniel Bachmann et al.
	\author produced by the Institute of Hydraulic Engineering (IWW), RWTH Aachen University
	\version 0.0.1                                                              
	\date 2009 
*/
#ifndef HYD_COUPLING_STRUCTURE_WEIR_H
#define HYD_COUPLING_STRUCTURE_WEIR_H

//system-hyd
//Base class for the coupling structures
#include "_Hyd_Coupling_Structure.h"

///Coupling structure class for a lateral outflow of a river-model; it is a weir-structure \ingroup hyd
/**
The discharge calculation over the weir is done via the Poleni-formula. The structure parameters, e.g. 
Poleni-factor, weir width etc., are specified by the user.
\see Hyd_Coupling_Structure_Gate
*/
class Hyd_Coupling_Structure_Weir : public _Hyd_Coupling_Structure
{
public:
	///Default constructor
	Hyd_Coupling_Structure_Weir(void);
	///Default destructor
	~Hyd_Coupling_Structure_Weir(void);

	//methods
	///Get and calculate the discharge for given global waterlevels before (global_h_1) and after the structure (global_h_2)
	double get_discharge(const double global_h_1, const double global_h_2);
	///Set the coupling structure member with a structure where the parser has gathered the informations (_hyd_coupling_structure_data)
	void set_coupling_structure_data(_hyd_coupling_structure_data data);
	///Get the coupling structure data
	_hyd_coupling_structure_data get_coupling_structure_data(void);

	///Output the members
	void output_members(ostringstream *cout);

	///Clone the weir structure
	void clone_coupling_structure(_Hyd_Coupling_Structure *structure);

private:
	//members
	///Poleni factor
	double poleni_fac;

	///check the parameters
	void check_parameters(void);

	///Set error(s)
	Error set_error(const int err_type);
	///Set warning(s)
	Warning set_warning(const int warn_type);
};
#endif
