#pragma once
/**\class _Hyd_Element_Floodplain_Type
	\author Daniel Bachmann et al.
	\author produced by the Institute of Hydraulic Engineering (IWW), RWTH Aachen University
	\version 0.0.1                                                              
	\date 2009 
*/
#ifndef _HYD_ELEMENT_FLOODPLAIN_TYPE_H
#define _HYD_ELEMENT_FLOODPLAIN_TYPE_H

//hyd_system class
#include "_Hyd_River_Profile_Type.h"
#include "Hyd_Param_Material.h"
#include "Hyd_Param_FP.h"
#include "Hyd_Instationary_Boundary.h"
#include "Hyd_Element_Bound_Members.h"
#include "Hyd_Element_Coup_Members.h"

//sys_system class
#include "Common_Const.h"
class Hyd_Element_Floodplain;

///Enumerator for the direction of the neighbouring elements (just orthogonal) \ingroup hyd
enum _hyd_neighbouring_direction{
	///x-direction [0°]
	X_DIR,
	///y-direction [90°]
	Y_DIR,
	///Negative x-direction [180°]
	MINUS_X_DIR,
	///Negative y-direction [270°]
	MINUS_Y_DIR
};
///Enumerator for all direction of neighbouring elemnets (orthogonal and diagonal) \ingroup hyd
enum _hyd_all_element_direction{
	///x-direction [0°] 
	X_DIRECT,
	///Half direction between x and y [45°]
	XY_DIRECT,
	///y-direction [90°]
	Y_DIRECT,
	///Half direction between negative x-direction and y [135°]
	YMINUS_X_DIRECT,
	///Negative x-direction [180°]
	MINUS_X_DIRECT,
	///Half direction between negative x-direction and negative y-direction [225°]
	MINUS_X_MINUS_Y_DIRECT,
	///Negative y-direction [270°]
	MINUS_Y_DIRECT,
	///Half direction between x-direction and negative y-direction [315°]
	MINUS_Y_X_DIRECT
};
///Data structure for storing the flow relevant parameters \ingroup hyd
struct _hyd_flowdata_floodplain_elem{
	///Integervalue for the material (n-value)
	int mat_type;
	///Manning-value n of the element [calculation variables]
    double n_value;
	///Initialcondition
    double init_condition;
	///Element flag for flow in x-direction (true=> flow in x-direction; false=> noflow in x-direction)
	bool no_flow_x_flag;
	///Element flag for flow in y-direction (true=> flow in y-direction; false=> noflow in y-direction)
	bool no_flow_y_flag;
	///Absolute element border height z (m) in x-direction 
    double height_border_x_abs;
	///Absolute element border height z (m) in y-direction
    double height_border_y_abs;
	///Flag if the Manning formula is used or the Poleni-formula in x-direction
	bool poleni_flag_x;
	///Flag if the Manning formula is used or the Poleni-formula in y-direction
	bool poleni_flag_y;
	///Poleni-factor in x-direction
    double poleni_x;
	///Poleni-factor in y-direction
    double poleni_y;

};

//forward declaration
struct _hyd_neighbouring_elems;

///Base class for all element types. The floodplainmodel consists of elements, which has a specific type \ingroup hyd
/**
The elements (Hyd_Element_Floodplain) defines the floodplain, every element has a specific type. The flow between the elements is calculated via the Manning-formula, 
if the are dikelines (see Hyd_Floodplain_Polysegment)are set to the element the flow is calculated via the Poleni-formula.
There are different types of elements:
	- standard elements (see Hyd_Element_Floodplain_Type_Standard);
	- river elements; they are intercepted by a river midline; there is no flow, but for a connecting element, from which the waterlevel is transferred, is automatically searched
	 (see Hyd_Element_Floodplain_Type_River);
	- dikeline element; they are intercepted dy a dikeline, else like standard elements (see Hyd_Element_Floodplain_Type_Standard)
	- coast elements; they are inseide a coast polygon; here no numerical calculation is performed (see Hyd_Element_Floodplain_Type_Coast)
	- noflow elements; they are inside a user defined noflow-polygon (see Hyd_Element_Floodplain_Type_Noflow)
	- noinfo elements; there are no information like the geodetical height (see Hyd_Element_Floodplain_Type_Noflow)

The geometrical attributes and the flow specific informations (e.g. Manning value, geodetical height) are stored here, the flow between the elements is calculated in this class.
*/
class _Hyd_Element_Floodplain_Type
{

	friend class Hyd_Element_Floodplain;
	
public:

	///Default constructor
	_Hyd_Element_Floodplain_Type(void);
	///Default destructor
	virtual ~_Hyd_Element_Floodplain_Type(void);

	//method

	///Initialize the element (call it backward: not from the origin; start at the diagonal corner of the origin)
	virtual void init_element(_hyd_neighbouring_elems neigh_elem, double *width_x, double *width_y, double *area, Hyd_Param_Material *mat_table, const bool clone)=0;
	///Connect the element with a instationary boundary curve if requiered (just needed in child-class Hyd_Element_Floodplain_Type_Standard)
	virtual void connect_instat_boundarycurve(Hyd_Instationary_Boundary *bound_curve);
	
	///Calculate the hydolocigal balance for boundary condition (every syncronisation step)
	virtual void calculate_hydrolocigal_balance_boundary(const double delta_t);
	///Calculate the hydolocigal balance for coupling (every syncronisation step)
	virtual void calculate_hydrolocigal_balance_coupling(const double delta_t);
	

	///Calculate the boundary value to a given time point
	virtual void calculate_boundary_value(const double time_point);

	///Calculate the maximum values and the wet duration
	virtual void calc_max_values(const double time_point, const double wet_boundary); 
	
	
	///Set the result value (just needed in child-class Hyd_Element_Floodplain_Type_Standard)
	virtual void set_solver_result_value(const double value);
	///Set a value to the the variation of the global waterlevel over time
	virtual void set_ds2dt_value(const double value);
	///Get the global waterlevel (s_value); here the z_value (here the global geodetic height (z_value) is return, because the cell is always dry)
	virtual double get_s_value(void);
	///Get the local waterlevel (h_value)
	virtual double get_h_value(void);

	///Get the relevant area of the element
	virtual double get_relevant_area(void);

	///Get the ds2dt value due to coupling
	virtual double get_coupling_ds2dt(void);

	///Rewind the element
	virtual void rewind_element(const double h_value);

	///Calculate the variation of the global waterlevel over time (just needed in child-class Hyd_Element_Floodplain_Type_Standard)
	virtual void calculate_ds_dt(void);
	///Calculate the flow velocities over time for output (just needed in child-class Hyd_Element_Floodplain_Type_Standard)
	virtual void calculate_v_out(const double sin_value, const double cos_value);
	///Add a value to the the variation of the global waterlevel over time (just needed in child-class Hyd_Element_Floodplain_Type_Standard)
	virtual void add_ds_dt(const double value);


	///Get the variation of the global waterlevel over time as result value (ds_dt_value)
	virtual double get_ds2dt_value(void);
	///Get the flow velocity in x-direction
	virtual double get_flowvelocity_vx(void);
	///Get the flow velocity in y-direction
	virtual double get_flowvelocity_vy(void);

	///Get the flow velocity in x-direction for output
	virtual double get_flowvelocity_vx_out(void);
	///Get the flow velocity in y-direction for output
	virtual double get_flowvelocity_vy_out(void);

	///Get the total flow velocity
	virtual double get_flowvelocity_vtotal(void);
	///Get the if the element is wet
	virtual double get_wet_flag(void);

	///Add the hydrological balance value of the boundary condition discharges to the given pointer
	virtual void add_hydro_balance_boundary(_hyd_hydrological_balance *given);
	///Add the hydrological balance value of the coupling discharges to the element due to a coupling between two floodplain models to the given pointer
	virtual void add_hydro_balance_coupling_fp(_hyd_hydrological_balance *given);
	///Add the hydrological balance value of the coupling discharges to the element due to river overflow to the given pointer
	virtual void add_hydro_balance_overflow_coupling_rv(_hyd_hydrological_balance *given);
	///Add the hydrological balance value of the coupling discharges to the element due to river coupling via a hydraulic structure to the given pointer
	virtual void add_hydro_balance_structure_coupling_rv(_hyd_hydrological_balance *given);
	///Add the hydrological balance value of the coupling discharges to the element due to river coupling via a dikebreak to the given pointer
	virtual void add_hydro_balance_dikebreak_coupling_rv(_hyd_hydrological_balance *given);
	///Add the hydrological balance value of the coupling discharges to the element due to costal overflow to the given pointer
	virtual void add_hydro_balance_overflow_coupling_co(_hyd_hydrological_balance *given);
	///Add the hydrological balance value of the coupling discharges to the element due to coast-model coupling via a dikebreak to the given pointer
	virtual void add_hydro_balance_dikebreak_coupling_co(_hyd_hydrological_balance *given);
	///Add the hydrological balance value of the coupling discharges from a coupled river model directly from the outflow profile to the given pointer
	virtual void add_hydro_balance_direct_coupling_rv(_hyd_hydrological_balance *given);
	
	///Get the hydrological balance value of the coupling discharges to the element due to a coupling between two floodplain models 
	virtual _hyd_hydrological_balance get_hydro_balance_coupling_fp(void);
	///Get the hydrological balance value of the coupling discharges to the element due to river overflow 
	virtual _hyd_hydrological_balance get_hydro_balance_overflow_coupling_rv(void);
	///Get the hydrological balance value of the coupling discharges to the element due to river coupling via a hydraulic structure 
	virtual _hyd_hydrological_balance get_hydro_balance_structure_coupling_rv(void);
	///Get the hydrological balance value of the coupling discharges to the element due to river coupling via a dikebreak 
	virtual _hyd_hydrological_balance get_hydro_balance_dikebreak_coupling_rv(void);
	///Get the hydrological balance value of the coupling discharges to the element due to costal overflow 
	virtual _hyd_hydrological_balance get_hydro_balance_overflow_coupling_co(void);
	///Get the hydrological balance value of the coupling discharges to the element due to coast-model coupling via a dikebreak 
	virtual _hyd_hydrological_balance get_hydro_balance_dikebreak_coupling_co(void);
	///Get the hydrological balance value of the coupling discharges from a coupled river model directly from the outflow profile 
	virtual _hyd_hydrological_balance get_hydro_balance_direct_coupling_rv(void);
	///Get the total coupling inflow volume (without boundary conditions)
	virtual double get_total_coupling_inflow_volume(void);
	///Get the total coupling outflow volume (without boundary conditions)
	virtual double get_total_coupling_outflow_volume(void);

	///Get the watervolume in the element
	virtual double get_watervolume_element(void);
	///Get the watervolume in of the init-conditions
	virtual double get_watervolume_init(void);

	///Get the conveance in x-direction
	virtual double get_cx(void);
	///Get the conveance in y-direction
	virtual double get_cy(void);
	///Get the maximum of geodetic height in x-direction
	virtual double get_smax_x(void);
	///Get the maximum of geodetic height in y-direction
	virtual double get_smax_y(void);

	///Get the boundary discharge
	virtual double get_bound_discharge(void);
	///Get the coupling discharge
	virtual double get_coupling_discharge(void);


	///Set the maximum results values directly
	virtual void set_max_results_directly(const double waterlevel, const double max_v, const double duration, const double dh_dt);

	///Get the maximum local waterlevel (h_value)
	virtual _hyd_max_values get_max_h_value(void);
	///Get the maximal variation of the global waterlevel over time as result value (ds_dt_value)
	virtual _hyd_max_values get_max_ds2dt(void);
	///Get the maximum flow velocity in x-direction
	virtual _hyd_max_values get_max_v_x(void);
	///Get the maximum flow velocity in y-direction
	virtual _hyd_max_values get_max_v_y(void);
	///Get the maximum total flow velocity
	virtual _hyd_max_values get_max_v_total(void);
	///Get the maximum product of folow velocity times waterlevel
	virtual _hyd_max_values get_max_hv(void);
	///Get the overall was_wet_flag: the element was wet during calculation
	virtual bool get_was_wet_flag(void);
	///Get the arrivial time: first point in time when the element get wet
	virtual double get_arrival_time(void);
	///Get the duration of flooding
	virtual double get_wet_duration(void);

	///Reset the discharge, which comming from coupled floodplain models (coupling_q_fp)
	virtual void reset_coupling_discharge_fp(void);
	///Add coupling discharge, which comming from coupled floodplain models (coupling_q_fp) during syncronisation 
	virtual void add_coupling_discharge_fp(const double discharge);
	///Reset the discharge, which comming from coupled river models due to overflow (coupling_q_rv_overflow)
	virtual void reset_coupling_discharge_rv_overflow(void);
	///Add coupling discharge, which comming from coupled river models due to overflow (coupling_q_rv_overflow) during syncronisation 
	virtual void add_coupling_discharge_rv_overflow(const double discharge);
	///Reset the discharge, which comming from coupled river models due to hydraulic structure coupling (coupling_q_rv_structure)
	virtual void reset_coupling_discharge_rv_structure(void);
	///Add coupling discharge, which comming from coupled river models due to hydraulic structure coupling (coupling_q_rv_structure) during syncronisation 
	virtual void add_coupling_discharge_rv_structure(const double discharge);
	///Reset the discharge, which comming from coupled river models due to dikebreak coupling (coupling_q_rv_dikebreak)
	virtual void reset_coupling_discharge_rv_dikebreak(void);
	///Add coupling discharge, which comming from coupled river models due to dikebreak coupling (coupling_q_rv_dikebreak) during syncronisation 
	virtual void add_coupling_discharge_rv_dikebreak(const double discharge);
	///Reset the discharge, which comming from a coupled coast model due to overflow (coupling_q_co_overflow)
	virtual void reset_coupling_discharge_co_overflow(void);
	///Add coupling discharge, which comming from a coupled coast model due to overflow (coupling_q_co_overflow) during syncronisation 
	virtual void add_coupling_discharge_co_overflow(const double discharge);
	///Reset the discharge, which comming from a coupled coast model due to dikebreak coupling (coupling_q_co_dikebreak)
	virtual void reset_coupling_discharge_co_dikebreak(void);
	///Add coupling discharge, which comming from a coupled coast model due to dikebreak coupling (coupling_q_co_dikebreak) during syncronisation 
	virtual void add_coupling_discharge_co_dikebreak(const double discharge);
	///Reset the discharge, which comming from a coupled river model directly from the outflow profile (coupling_q_rv_direct)
	virtual void reset_coupling_discharge_rv_direct(void);
	///Add coupling discharge, which comming from a coupled river model directly from the outflow profile (coupling_q_rv_direct)
	virtual void add_coupling_discharge_rv_direct(const double discharge);

	///Set the maximum result values of an element to an query string to transfer them into a database table (Hyd_Element_Floodplain
	virtual void set_maximum_value2querystring(ostringstream *query_string);

	///Set the instationary result values of an element to an query string to transfer them into a database table (Hyd_Element_Floodplain)
	virtual void set_instat_value2querystring(ostringstream *query_string);

	///Get the next _hyd_all_element_direction to the given direction counterclockwise
	static _hyd_all_element_direction get_next_element_direction_counterclockwise(const _hyd_all_element_direction dir);
	///Get the next _hyd_all_element_direction to the given direction clockwise
	static _hyd_all_element_direction get_next_element_direction_clockwise(const _hyd_all_element_direction dir);

	///Reset the hydrological balance value and the maximum values
	virtual void reset_hydrobalance_maxvalues(void);

	///Set the connected element (just relevant for river element types)
	virtual void set_connected_element(_hyd_neighbouring_elems neigh_elem);

	///Set the pointer to the sea waterlevel
	virtual void set_ptr_sea_waterlevel(double *ptr);

	///Get the maximum change in a element 
	virtual void get_max_changes_elements(double *max_change_h, const bool timecheck);

	///Get a pointer to the flow data
	virtual _hyd_flowdata_floodplain_elem get_flow_data(void);
	///Get a pointer to the boundary data
	virtual _hyd_boundarydata_floodplain_elem get_boundary_data(void);


	///Get the coupling flag
	virtual bool get_coupling_flag(void);
	///Get boundary condition flag
	virtual bool get_bound_flag(void);

	///Delete the data of boundary condition
	virtual void delete_bound_data(void);

	///Set coupling data
	virtual void set_coupling_data(void);

protected:
	//members
	///Global geodetic height
    double *z_value;
	///Flag if the element was once wet [result variables]
	bool was_wet_flag;


	//method
	///Initialize the data structures of _hyd_flowdata_floodplain_elem
	void init_flowdata_structures(_hyd_flowdata_floodplain_elem * data);
	///Get the element index of the connected element (important for Hyd_Element_Floodplian_Type_River because here the connected element number is returned)
	virtual int get_coupling_relevant_element_number(void);



private:
	//methods


	///Output the setted members
	virtual void output_setted_members(ostringstream *cout)=0;
	///Ouput the maximum calculated values
	virtual void output_maximum_calculated_values(ostringstream *cout)=0;
	///Ouput the result members per internal timestep
	virtual void output_result_values(ostringstream *cout)=0;
	///Ouput the header for the maximum calculated values output
	static void output_header_result_values(ostringstream *cout);
	///Ouput the header for the maximum calculated values output
	static void output_header_maximum_calculated_values(ostringstream *cout);

	
	///Set the data buffers of _hyd_boundarydata_floodplain_elem, _hyd_flowdata_floodplain_elem and the global geodetic height from class Hyd_Floodplain_Element  
    virtual void set_data_buffers(const _hyd_boundarydata_floodplain_elem boundary, const _hyd_flowdata_floodplain_elem flow, double *z_value);
	
	///Set new boundary condition data
	virtual void set_boundary_condition_data(const _hyd_boundarydata_floodplain_elem boundary);

	
	///Set element flag for flow in x-direction (just needed in child-class Hyd_Element_Floodplain_Type_Standard)
	virtual void set_x_noflow_flag(const bool flag);
	///Set element flag for flow in y-direction (just needed in child-class Hyd_Element_Floodplain_Type_Standard)
	virtual void set_y_noflow_flag(const bool flag);

	///Set the absolute element border height z (m) in x-direction and the Poleni-factor (just needed in child-class Hyd_Element_Floodplain_Type_Standard)
	virtual void set_x_border_z(const double abs_height, const double pol_factor);
	///Set the absolute element border height z (m) in y-direction and the Poleni-factor (just needed in child-class Hyd_Element_Floodplain_Type_Standard)
	virtual void set_y_border_z(const double abs_height, const double pol_factor);

};
#endif
