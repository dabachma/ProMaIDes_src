#pragma once
/**\class _Hyd_Coupling_Structure
	\author Daniel Bachmann et al.
	\author produced by the Institute of Hydraulic Engineering (IWW), RWTH Aachen University
	\version 0.0.1                                                              
	\date 2009 
*/
#ifndef _HYD_COUPLING_STRUCTURE_H
#define _HYD_COUPLING_STRUCTURE_H


//system_sys_ classes
#include "_Sys_Common_System.h"

///Enumerator for the type of lateral outflow of a river section \ingroup hyd
enum _hyd_1d_outflow_types{
	///The lateral outflow is defined by a weir overflow
	WEIR_OUT,
	///The lateral outflow is defined by a gate
	GATE_OUT,
	///Unknown type
	UNKNOWN_OUT,
};
///Data structure where the parameters of a hydraulic coupling structure are gathered \ingroup hyd
struct _hyd_coupling_structure_data{
	///Width of the structure sill (m)
	double width;
	///Falg if it is a controlled (true) or non-cotrolled structure (false)
	bool controlled_flag;
	///Controlled parameter: WEIR the local sill height (m); GATE the local opening height (m)
	double controlled_param;
	///Uncontrolled parameter: WEIR value for the Poleni Factor (e.g. 0.65) (-); GATE value for the local sill height (m)
	double uncontrollable_param;
};

///Base class of the coupling structures for lateral outflow of a river \ingroup hyd
/**
The coupling structures for lateral outflow of a river are:
- weirs
- gates

For weirs the Poleni-formula is use, the discharge through gates are calculated via combination of the Poleni-formula
for no-pressure flow and the pressure-formula for pressure flow.
*/
class _Hyd_Coupling_Structure
{
public:
	///Default constructor
	_Hyd_Coupling_Structure(void);
	///Default destructor
	virtual ~_Hyd_Coupling_Structure(void);

	//methods
	///Set the global sill height
	void set_global_sill_height(double max_z_min);
	///Get and calculate the discharge for given global waterlevels before (global_h_1) and after the structure (global_h_2)
	virtual double get_discharge(const double global_h_1, const double global_h_2)=0;

	///Set the coupling structure member with a structure where the parser has gathered the informations (_hyd_coupling_structure_data)
	virtual void set_coupling_structure_data(_hyd_coupling_structure_data data);

	///Get if the controll curve is found
	bool get_control_curve_is_found(void);

	///Get the width of the structure
	double get_width(void);

	///Get the coupling structure data
	virtual _hyd_coupling_structure_data get_coupling_structure_data(void)=0;

	///Output the members
	virtual void output_members(ostringstream *cout)=0;

	///Convert a string to _1d_outflow_types of the coupling structure
	static _hyd_1d_outflow_types convert_string2coupling_type(string type);

	///Convert a _1d_outflow_types of the coupling structure to a string
	static string convert_coupling_type2string(_hyd_1d_outflow_types type);

	///Reset the coupling structure
	void reset_structure(void);

	///Clone the coupling structure
	virtual void clone_coupling_structure(_Hyd_Coupling_Structure *structure);

protected:

	//members
	///Global height of the structures sill
	double global_h_sill;
	///Local height of the structures sill
	double local_h_sill;
	///Width of the structure
	double width;
	///Flag if the outflow is controlled or uncontrolled
	bool controlled_flag;

	//Pointer to the controll function;
	//todo
	///Flag if the control curve is found
	bool control_curve_is_found;

	///The discharge from a solver step before
	double old_discharge;

	///Discharge through/over the structure
	double discharge;

	///Flow velocity through/over the structure
	double coupling_v;

	///Constant boundary for the discharge through the profiles
	const double discharge_boundary;

	//methods
	///Allocate the control function and set the index \todo this methods have to be implemented
	void allocate_control_function(int const index);
	///Smooth the coupling discharge with the coupling discharge calculated one internal timestep before
	double smooth_coupling_discharge(const double q_buff);

	///check the parameters
	void check_parameters(void);

	///Set error(s)
	Error set_error(const int err_type);


};
#endif
