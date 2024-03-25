#pragma once
/**\class Hyd_Element_Floodplain_Type_Standard
	\author Daniel Bachmann et al.
	\author produced by the Institute of Hydraulic Engineering (IWW), RWTH Aachen University
	\version 0.0.1                                                              
	\date 2009 
*/
#ifndef HYD_ELEMENT_FLOODPLAIN_TYPE_STANDARD_H
#define HYD_ELEMENT_FLOODPLAIN_TYPE_STANDARD_H

//hyd system
//base class
#include "_Hyd_Element_Floodplain_Type.h"


///This class represents the standard elements of a floodplain model, with the associated geometrical and flow characteristics \ingroup hyd
/**

*/
class Hyd_Element_Floodplain_Type_Standard : public _Hyd_Element_Floodplain_Type
{
///Class where the element type is decided and which work with the element type
friend class Hyd_Element_Floodplain;

public:
	///Default constructor
	Hyd_Element_Floodplain_Type_Standard(void);
	///Default destructor
	~Hyd_Element_Floodplain_Type_Standard(void);

	//member


	//methods
	///Initialize the element (call it backward: not from the origin; start at the diagonal corner of the origin)
	void init_element(_hyd_neighbouring_elems neigh_elem, double *width_x, double *width_y, double *area, Hyd_Param_Material *mat_table, const bool clone);

	///Connect the element with a instationary boundary curve if requiered
	void connect_instat_boundarycurve(Hyd_Instationary_Boundary *bound_curve);

	///Calculate the hydolocigal balance for boundary condition (every syncronisation step)
	void calculate_hydrolocigal_balance_boundary(const double delta_t);
	///Calculate the hydolocigal balance for coupling (every syncronisation step)
	void calculate_hydrolocigal_balance_coupling(const double delta_t);
	

	///Calculate the boundary value to a given time point
	void calculate_boundary_value(const double time_point);
	
	///Calculate the maximum values and the wet duration
	void calc_max_values(const double time_point, const double wet_boundary); 



	///Set the result value from the solver
	void set_solver_result_value(const double result);
	///Get the global waterlevel (s_value)
	double get_s_value(void);
	///Get the local waterlevel (h_value)
	double get_h_value(void);


	///Calculate the variation of the global waterlevel over time
	void calculate_ds_dt(void);
	///Calculate the flow velocities over time for output
	void calculate_v_out(const double sin_value, const double cos_value);
	///Set a value to the the variation of the global waterlevel over time
	void set_ds2dt_value(const double value);
	///Get the flow velocity in x-direction
	void set_flowvelocity_vx(double);
	///Get the flow velocity in y-direction
	void set_flowvelocity_vy(double);

	///Get the variation of the global waterlevel over time as result value ds_dt_value
	double get_ds2dt_value(void);
	///Get the flow velocity in x-direction
	double get_flowvelocity_vx(void);
	///Get the flow velocity in y-direction
	double get_flowvelocity_vy(void);
	///Get the flow velocity in x-direction for output
	double get_flowvelocity_vx_out(void);
	///Get the flow velocity in y-direction for output
	double get_flowvelocity_vy_out(void);
	///Get the total flow velocity
	double get_flowvelocity_vtotal(void);
	///Get the if the element is wet
	double get_wet_flag(void);

	///Get the relevant area of the element
	double get_relevant_area(void);
	///Get the ds2dt value due to coupling
	double get_coupling_ds2dt(void);

	///Add the hydrological balance value of the boundary condition discharges to the given pointer
	void add_hydro_balance_boundary(_hyd_hydrological_balance *given);
	///Add the hydrological balance value of the coupling discharges to the element due to a coupling between two floodplain models to the given pointer
	void add_hydro_balance_coupling_fp(_hyd_hydrological_balance *given);
	///Add the hydrological balance value of the coupling discharges to the element due to river overflow to the given pointer
	void add_hydro_balance_overflow_coupling_rv(_hyd_hydrological_balance *given);
	///Add the hydrological balance value of the coupling discharges to the element due to river coupling via a hydraulic structure to the given pointer
	void add_hydro_balance_structure_coupling_rv(_hyd_hydrological_balance *given);
	///Add the hydrological balance value of the coupling discharges to the element due to river coupling via a dikebreak to the given pointer
	void add_hydro_balance_dikebreak_coupling_rv(_hyd_hydrological_balance *given);
	///Add the hydrological balance value of the coupling discharges to the element due to costal overflow to the given pointer
	void add_hydro_balance_overflow_coupling_co(_hyd_hydrological_balance *given);
	///Add the hydrological balance value of the coupling discharges to the element due to coast-model coupling via a dikebreak to the given pointer
	void add_hydro_balance_dikebreak_coupling_co(_hyd_hydrological_balance *given);
	///Add the hydrological balance value of the coupling discharges from a coupled river model directly from the outflow profile (coupling_q_rv_direct)
	void add_hydro_balance_direct_coupling_rv(_hyd_hydrological_balance *given);

	///Get the hydrological balance value of the coupling discharges to the element due to a coupling between two floodplain models 
	_hyd_hydrological_balance get_hydro_balance_coupling_fp(void);
	///Get the hydrological balance value of the coupling discharges to the element due to river overflow 
	_hyd_hydrological_balance get_hydro_balance_overflow_coupling_rv(void);
	///Get the hydrological balance value of the coupling discharges to the element due to river coupling via a hydraulic structure 
	_hyd_hydrological_balance get_hydro_balance_structure_coupling_rv(void);
	///Get the hydrological balance value of the coupling discharges to the element due to river coupling via a dikebreak 
	_hyd_hydrological_balance get_hydro_balance_dikebreak_coupling_rv(void);
	///Get the hydrological balance value of the coupling discharges to the element due to costal overflow 
	_hyd_hydrological_balance get_hydro_balance_overflow_coupling_co(void);
	///Get the hydrological balance value of the coupling discharges to the element due to coast-model coupling via a dikebreak 
	_hyd_hydrological_balance get_hydro_balance_dikebreak_coupling_co(void);
	///Get the hydrological balance value of the coupling discharges from a coupled river model directly from the outflow profile 
	_hyd_hydrological_balance get_hydro_balance_direct_coupling_rv(void);
	///Get the total coupling inflow volume (without boundary conditions)
	double get_total_coupling_inflow_volume(void);
	///Get the total coupling outflow volume (without boundary conditions)
	double get_total_coupling_outflow_volume(void);

	///Reset all hydrologicla balance value
	void reset_hydro_balance(void);

	///Get the watervolume in the element
	double get_watervolume_element(void);
	///Get the watervolume in of the init-conditions
	double get_watervolume_init(void);

	///Set the maximum results values directly
	void set_max_results_directly(const double waterlevel, const double max_v, const double duration, const double dh_dt);


	///Get the maximum local waterlevel (h_value)
	_hyd_max_values get_max_h_value(void);
	///Get the maximal variation of the global waterlevel over time as result value (ds_dt_value)
	_hyd_max_values get_max_ds2dt(void);
	///Get the maximum flow velocity in x-direction
	_hyd_max_values get_max_v_x(void);
	///Get the maximum flow velocity in y-direction
	_hyd_max_values get_max_v_y(void);
	///Get the maximum total flow velocity
	_hyd_max_values get_max_v_total(void);
	///Get the maximum of the product flow velocity time waterlevel (hv)
	_hyd_max_values get_max_hv(void);
	///Get the overall was_wet_flag: the element was wet during calculation
	bool get_was_wet_flag(void);
	///Get the arrivial time: first point in time when the element get wet
	double get_arrival_time(void);
	///Get the duration of flooding
	double get_wet_duration(void);

	
	///Get the conveance in x-direction
	double get_cx(void);
	///Get the conveance in y-direction
	double get_cy(void);
	///Get the maximum of geodetic height in x-direction
	double get_smax_x(void);
	///Get the maximum of geodetic height in y-direction
	double get_smax_y(void);
	///Get the boundary discharge
	double get_bound_discharge(void);
	///Get the coupling discharge
	double get_coupling_discharge(void);

	///Reset the discharge, which comming from coupled floodplain models (coupling_q_fp)
	void reset_coupling_discharge_fp(void);
	///Add coupling discharge, which comming from coupled floodplain models (coupling_q_fp) during syncronisation 
	void add_coupling_discharge_fp(const double discharge);
	///Reset the discharge, which comming from coupled river models due to overflow (coupling_q_rv_overflow)
	void reset_coupling_discharge_rv_overflow(void);
	///Add coupling discharge, which comming from coupled river models due to overflow (coupling_q_rv_overflow) during syncronisation 
	void add_coupling_discharge_rv_overflow(const double discharge);
	///Reset the discharge, which comming from coupled river models due to hydraulic structure coupling (coupling_q_rv_structure)
	void reset_coupling_discharge_rv_structure(void);
	///Add coupling discharge, which comming from coupled river models due to hydraulic structure coupling (coupling_q_rv_structure) during syncronisation 
	void add_coupling_discharge_rv_structure(const double discharge);
	///Reset the discharge, which comming from coupled river models due to dikebreak coupling (coupling_q_rv_dikebreak)
	void reset_coupling_discharge_rv_dikebreak(void);
	///Add coupling discharge, which comming from coupled river models due to dikebreak coupling (coupling_q_rv_dikebreak) during syncronisation 
	void add_coupling_discharge_rv_dikebreak(const double discharge);
	///Reset the discharge, which comming from a coupled coast model due to overflow (coupling_q_co_overflow)
	void reset_coupling_discharge_co_overflow(void);
	///Add coupling discharge, which comming from a coupled coast model due to overflow (coupling_q_co_overflow) during syncronisation 
	void add_coupling_discharge_co_overflow(const double discharge);
	///Reset the discharge, which comming from a coupled coast model due to dikebreak coupling (coupling_q_co_dikebreak)
	void reset_coupling_discharge_co_dikebreak(void);
	///Add coupling discharge, which comming from a coupled coast model due to dikebreak coupling (coupling_q_co_dikebreak) during syncronisation 
	void add_coupling_discharge_co_dikebreak(const double discharge);
	///Reset the discharge, which comming from a coupled river model directly from the outflow profile (coupling_q_rv_direct)
	void reset_coupling_discharge_rv_direct(void);
	///Add coupling discharge, which comming from a coupled river model directly from the outflow profile (coupling_q_rv_direct)
	void add_coupling_discharge_rv_direct(const double discharge);

	///Set the maximum result values of an element to an query string to transfer them into a database table (Hyd_Element_Floodplain
	void set_maximum_value2querystring(ostringstream *query_string);

	///Set the instationary result values of an element to an query string to transfer them into a database table (Hyd_Element_Floodplain
	void set_instat_value2querystring(ostringstream *query_string);


	///Reset the hydrological balance value and the maximum values
	void reset_hydrobalance_maxvalues(void);

	///Get the maximum change in a element 
	void get_max_changes_elements(double *max_change_h, const bool timecheck);

	///Get the coupling flag
	bool get_coupling_flag(void);
	///Get boundary condition flag
	bool get_bound_flag(void);

	///Delete the data of boundary condition
	void delete_bound_data(void);

	///Set coupling data
	void set_coupling_data(void);





private:
	//members
	///Pointer to width of the element in x-direction [calculation variables]
	double *x_width;
	///Pointer to width of the element in x-direction [calculation variables]
	double *y_width;
	///Pointer to element area
	double *area;
	///Maximal global geodetic height in x-direction [calculation variables]
    double s_max_x;
	///Maximal global geodetic height in y-direction [calculation variables]
    double s_max_y;
	///Summarized flowparameter in x-direction [calculation variables]
	double c_x;
	///Summarized flowparameter in y-direction [calculation variables]
	double c_y;
	///Pointer to the element in x-direction
	Hyd_Element_Floodplain *element_x;
	///Pointer to the element in y-direction
	Hyd_Element_Floodplain *element_y;
	///Pointer to the element in minus x-direction
	Hyd_Element_Floodplain *element_x_minus;
	///Pointer to the element in minus y-direction
	Hyd_Element_Floodplain *element_y_minus;


	///Data structure of the flow data of the element (_hyd_flowdata_floodplain_elem) [calculation variables]
	_hyd_flowdata_floodplain_elem flow_data;
	
	///Gloabl waterlevel:= geodetic height (z) + waterlevel (h); it is also the result of the solver [result variables]
	double s_value;
	///Local waterlevel:= waterlevel (h) [result variables]
	double h_value;
	///Variation of the global waterlevel over time [result variables]
	double ds_dt_value;
	///Flow velocity in x-direction [result variables]
    float v_x;
	///Flow velocity in y-direction [result variables]
    float v_y;
	///Flow velocity in x-direction for output [result variables]
	float v_x_out;
	///Flow velocity in y-direction for output [result variables]
	float v_y_out;
	///Total flow velocity [result variables]
    float v_total;
	///Flag if the element is wet [result variables]
	bool wet_flag;
	///Point in time when the element gets wet [result variables]
    float time_wet_start;
	///Point in time when the water arrives first at this element
    float first_arrival_time;
	///Duration how long the element was wet [result variables]
    float wet_duration;
	///Maximum local waterlevel [maximum result variables]
	_hyd_max_values max_h_value;
	///Maximum of the variation of the global waterlevel over time [maximum result variables]
	_hyd_max_values max_ds_dt;
	///Maximum flow velocity in x-direction [maximum result variables]
	_hyd_max_values max_v_x;
	///Maximum flow velocity in y-direction [maximum result variables]
	_hyd_max_values max_v_y;
	///Maximum total flow velocity [maximum result variables]
	_hyd_max_values max_v_total;
	///Maximum value of the product of the total flow velocity times waterlevel (h*v)
	_hyd_max_values max_hv;

	///Flag if the element is coupled
	bool coupling_flag;
	///Coupling mebers
	Hyd_Element_Coup_Members *coup_mem;

	///Flag if the element has a boundary condition
	bool boundary_flag;
	///Boundary mebers
	Hyd_Element_Bound_Members *bound_mem;


	///Variation of the waterlevel over time for boundary and coupling
	double ds_dt_coup;
	///Variation of the waterlevel over time for the internal timestep before for boundary and coupling
	double ds_dt_coup_old;

	///The watervolume in the element
    double watervolume;
	///The watervolume of the init-conditions
    double watervolume_init;

	///Infiltration rate
	const double infiltration_rate;





	//method
	///Get the flow data structure
	_hyd_flowdata_floodplain_elem get_flow_data(void);
	///Get the boundary data structure
	_hyd_boundarydata_floodplain_elem get_boundary_data(void);

	///Output the setted members
	void output_setted_members(ostringstream *cout);
	///Ouput the result members per internal timestep
	void output_result_values(ostringstream *cout);
	///Ouput the maximum calculated values
	void output_maximum_calculated_values(ostringstream *cout);

	///Set the data buffers of _hyd_boundarydata_floodplain_elem, _hyd_flowdata_floodplain_elem and the global geodetic height from class Hyd_Floodplain_Element 
    void set_data_buffers(const _hyd_boundarydata_floodplain_elem boundary, const _hyd_flowdata_floodplain_elem flow, double *z_value);


	///Set new boundary condition data
	void set_boundary_condition_data(const _hyd_boundarydata_floodplain_elem boundary);


	///Set element flag for flow in x-direction
	void set_x_noflow_flag(const bool flag);
	///Set element flag for flow in y-direction
	void set_y_noflow_flag(const bool flag);

	///Set the absolute element border height z (m) in x-direction and the Poleni-factor
	void set_x_border_z(const double abs_height, const double pol_factor);
	///Set the absolute element border height z (m) in y-direction and the Poleni-factor
	void set_y_border_z(const double abs_height, const double pol_factor);



	///Calculate the variation of the global waterlevel (s-value) over time for flow in x-direction with the Manning-formula
	void calculate_ds_dt_manning_x(void);
	///Calculate the variation of the global waterlevel (s-value) over time for flow in y-direction with the Manning-formula
	void calculate_ds_dt_manning_y(void);
	///Calculate the variation of the global waterlevel (s-value) over time for flow in x-direction with the Poleni-formula
	void calculate_ds_dt_poleni_x(void);
	///Calculate the variation of the global waterlevel (s-value) over time for flow in y-direction with the Poleni-formula
	void calculate_ds_dt_poleni_y(void);
	


	///Calculate the maximum values to a given time point
	void calculate_maximum_values(const double time_point);
	///Calculate if the element gets wet or dry to a given timepoint and the duration of being wet
	void calculate_wet_dry_duration(const double time_point, const double wet_boundary);
	///Calculate the watervolume in the element
	void calculate_watervolume_element(const bool infiltration=false);


	///Check if there are unphysical waterlevel calculated by the solver
	void check_unphysical_waterlevels(void);


	///Allocate the data of coupling
	void allocate_coup_data(void);
	///Delete the data of coupling
	void delete_coup_data(void);
	///Allocate the data of boundary condition
	void allocate_bound_data(void);



	///Set error(s)
	Error set_error(const int err_type);
};

#endif
