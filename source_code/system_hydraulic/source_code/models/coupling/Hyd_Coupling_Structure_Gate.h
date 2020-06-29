#pragma once
/**\class Hyd_Coupling_Structure_Gate
	\author Daniel Bachmann et al.
	\author produced by the Institute of Hydraulic Engineering (IWW), RWTH Aachen University
	\version 0.0.1                                                              
	\date 2009 
*/
#ifndef HYD_COUPLING_STRUCTURE_GATE_H
#define HYD_COUPLING_STRUCTURE_GATE_H

//system-hyd
//Base class for the coupling structures
#include "_Hyd_Coupling_Structure.h"

///Coupling structure class for a lateral outflow of a river-model; it is a gate-structure \ingroup hyd
/**
The discharge through gates are calculated via combination of the Poleni-formula
for no-pressure flow and the pressure-formula for pressure flow.
The structure parameters, e.g. sill height, gate opening width etc., are specified by the user.
\see Hyd_Coupling_Structure_Weir
*/
class Hyd_Coupling_Structure_Gate : public _Hyd_Coupling_Structure
{
public:
	///Default constructor
	Hyd_Coupling_Structure_Gate(void);
	///Default destructor
	~Hyd_Coupling_Structure_Gate(void);

	//methods
	///Get and calculate the discharge for given global waterlevels before (global_h_1) and after the structure (global_h_2)
	double get_discharge(const double global_h_1, const double global_h_2);
	///Set the coupling structure member with a structure where the parser has gathered the informations (_hyd_coupling_structure_data)
	void set_coupling_structure_data(_hyd_coupling_structure_data data);
	///Get the coupling structure data
	_hyd_coupling_structure_data get_coupling_structure_data(void);

	///Output the members
	void output_members(ostringstream *cout);
	///Clone the gate structure
	void clone_coupling_structure(_Hyd_Coupling_Structure *structure);

private:
	//members
	///Local height of the opening
	double local_opening_h;

	///Contraction factor for a gate of 90°-inclination (after "Ueberfaelle und Wehre", 2005, G. Peter, vieweg)
	double contraction_factor;
	///Reduction factor for backwater effect
	double reduction_backwater;
	///Flow coefficient for a gate flow (after "Ueberfaelle und Wehre", 2005, G. Peter, vieweg)
	double flow_coefficient;
	///Poleni coefficient (0.577) for a broad crested weir is taken (for waterlevel below the gate opening)
	const double poleni_fac;

private:
	//methods
	///Calculation of the flow coefficient for a gate flow (after "Ueberfaelle und Wehre", 2005, G. Peter, vieweg)
	void calculate_flow_coefficient(const double h_local_before);
	///Calculation of the contraction factor for a gate (after "Ueberfaelle und Wehre", 2005, G. Peter, vieweg)
	void calculate_contraction_factor(const double h_local_before);
	///Calculation of the reduction factor for backwater effect
	void calculate_reduction_factor(const double h_local_before, const double h_local_after);

	///Calculate the discharge for given global waterlevels before (global_h_1) and after the structure (global_h_2) for a weir (waterlevel below the gate opening) 
	void calculate_poleni_discharge(const double global_h_1, const double global_h_2);

	///check the parameters
	void check_parameters(void);

	///Set error(s)
	Error set_error(const int err_type);

};
#endif
