#pragma once
/**\class Hyd_River_Profile_Type_Bridge
	\author Daniel Bachmann et al.
	\author produced by the Institute of Hydraulic Engineering (IWW), RWTH Aachen University
	\version 0.0.1                                                              
	\date 2009 
*/
#ifndef HYD_RIVER_PROFILE_TYPE_BRIDGE_H
#define HYD_RIVER_PROFILE_TYPE_BRIDGE_H

//hyd system classes
//base class
#include "Hyd_River_Profile_Type_Standard.h"


///Data structure for the flow parameters of the pressure flow \ingroup hyd
struct _hyd_pressure_flow_param{
	///The opening width;
	double opening_width;
	///The local opening height
	double opening_height;
	///The global sill height
	double sill_height;
	///Flag if the pressure flow is applied
	bool applied_flag;
};


///Class of a river profile type representing a bridge profile \ingroup hyd
/**

\see _Hyd_River_Profile_Type
*/
class Hyd_River_Profile_Type_Bridge : public Hyd_River_Profile_Type_Standard
{
public:
	///Default constructor
	Hyd_River_Profile_Type_Bridge(void);
	///Default destructor
	~Hyd_River_Profile_Type_Bridge(void);

	//methods
	///Set the bridge data (here the profile specific value)
	void set_profile_specific_value(const _hyd_bridge_values data);
	///Get the bridge data specific values
	_hyd_bridge_values get_bridge_data(void);

	///Get the bridge data as a geometrie for a plot
	int get_bridge_data2plot(double **x, double **z);

	///Get the convenyance factor total; it is limited by the bridge height
	double get_c_value(void);

	///Output the members of profile type
	void output_members(ostringstream *cout);

	///Calculate and allocate the tables (Hyd_Tables): area, waterlevel, convenyance for left bank, right bank and mid channel; set the bridge height and width
	void calc_alloc_tables(const double delta_x, Hyd_Param_Material *material_table);

	///Calculate the actual discharge through this profile by a given upstream and downstream profile
	double calculate_actual_discharge(_Hyd_River_Profile *upstream_profile,  _Hyd_River_Profile *downstream_profile, const double distance_upstream);

	///Reset the maximum values and the result
	void reset_result_max_values(void);

	///Clone river profile type
	void clone_profile_type(_Hyd_River_Profile_Type *prof_type);


private:
	//members
	///Defines the bridge data as local height and the size of the bridge body
	_hyd_bridge_values bridge_data;

	///The global bridge height
	double global_bridge_height;

	///Area for the pressure flow
	double pressure_flow_area;
	//Flag if the pressure flow is applied
	bool flag_pressure_flow;
	///Area for the weir flow
	double weir_flow_area;
	///Area of the profile to the upper brink of the bridge
	double upper_bridge_area;

	///Flow velocity under the bridge construction
	double flow_v_under;
	///Discharge under the bridge construction
	double q_under;
	///Flow velocity over the bridge construction
	double flow_v_over;
	///Discharge over the bridge construction
	double q_over;

	///Fixd discharge for the normal flow before the transition normal flow to pressure flow
	//double fixed_normal_q;
	
	///The pressure flow data for each profile segment
	_hyd_pressure_flow_param *press_flow_data;

	//methods
	///Check the points and the areatypes of consistency and check the bridge specific data
	void check_points(const string name, const int number);
	///Check the segments of the bridge profile
	void check_bridge_segments(void);
	///Allocate and calculate the pressure flow data for each river profile segment
	void alloc_calc_pressure_flow_data(void);
	///Calculate the discharge via pressure flow (gate formula)
	double calculate_pressure_flow(const double flow_h, const double reducing_h);
	///Calculation of the flow coefficient for a gate flow (after "Ueberfaelle und Wehre", 2005, G. Peter)
	double calculate_flow_coefficient(const double h_local_before, const double opening_h);
	///Calculation of the contraction factor for a gate of 90°-inclination (after "Ueberfaelle und Wehre", 2005, G. Peter, vieweg)
	double calculate_contraction_factor(const double h_local_before, const double opening_h, const double flow_coeff);
	///Calculation of the reduction factor for backwater effect
	double calculate_reduction_factor(const double h_local_before, const double h_local_after, const double contraction_fac, const double opening_h);

	///Calculate an additional discharge via weir flow (weir formula)
	double calculate_weir_flow(const double flow_h, const double reducing_h);

	///Set error(s)
	Error set_error(const int err_type);
};
#endif
