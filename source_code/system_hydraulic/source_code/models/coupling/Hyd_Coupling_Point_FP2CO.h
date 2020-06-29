#pragma once
/**\class Hyd_Coupling_Point_FP2CO
	\author Daniel Bachmann et al.
	\author produced by the Institute of Hydraulic Engineering (IWW), RWTH Aachen University
	\version 0.0.1                                                              
	\date 2009 
*/
#ifndef HYD_COUPLING_POINT_FP2CO_H
#define HYD_COUPLING_POINT_FP2CO_H



//system hyd
//base class
#include "_Hyd_Coupling_Point.h"
#include "Hyd_Coast_Model.h"
#include "Hyd_Floodplain_Dikeline_Point.h"

///Coupling point-class for the coupling between a floodplain- (FP) and a coast-model (CO) \ingroup hyd
/**
The coupling between the floodplain- and the coast-model is a linear coupling. The founded points by a
a geometrical model interception are related to the coast-line (Hyd_Coast_Model).
The flow over the coast-line is evaluated by the Poleni-formula. The discharge to/out of the coupling related floodplain standard
element (Hyd_Floodplain_Element_Type_Standard) is set as a discharge boundary condition to this element, constant
over the syncronisation timestep. The discharge to the coast-model is not further considered.
The points are managed and stored in a list (Hyd_Coupling_Point_FP2CO_List).
*/
class Hyd_Coupling_Point_FP2CO: public _Hyd_Coupling_Point
{
///Class for managing this points in a list
friend class Hyd_Coupling_Point_FP2CO_List;

public:
	///Default constructor
	Hyd_Coupling_Point_FP2CO(void);
	///Copy constructor
	Hyd_Coupling_Point_FP2CO(const Hyd_Coupling_Point_FP2CO& object);
	///Default destructor
	~Hyd_Coupling_Point_FP2CO(void);

	//methods
	///Set the pointer/index members (without the geometrical members) from the given point
	void set_couplingpoint_members(Hyd_Coupling_Point_FP2CO *object);
	///Output the header for the setted member (static)
	static void output_header_setted_member(ostringstream *cout);
	///Output the setted members
	void output_setted_members(ostringstream *cout);
	///Set the indices of the dikeline and the floodplain element 
	void set_indices(const int coast_dikeline_index, const int floodplain_model_elem);
	///Set the pointer to the dikeline points
	void set_ptr_dikeline_points(Hyd_Floodplain_Dikeline_Point *upstream, Hyd_Floodplain_Dikeline_Point *downstream);
	///Get the pointer to upstream dikeline point
	Hyd_Floodplain_Dikeline_Point *get_ptr_upstream_point(void);
	///Get the pointer to downstream dikeline point
	Hyd_Floodplain_Dikeline_Point *get_ptr_downstream_point(void);

	///Set the flag if a coupling due to a overflow is applicable
	void set_overflow_flag(const bool flag);

	///Set the index of the coupled floodplain
	void set_floodplain_index(const int index);
	///Get the index of the coupled floodplain
	int get_floodplain_index(void);

	///Set the pointer to the floodplain element
	void set_pointers(Hyd_Element_Floodplain *floodplain); 
	///Get the pointer to the floodplain element
	Hyd_Element_Floodplain* get_floodplain_element(void);


	///Transfer the coupling characteristics of the coupled elements
	void transfer_coupling_characteristics(void);
	///Reset the current coupling discharge of the points and the coupled element
	void reset_coupling_discharge(void);
	///Syncronisation of the coupled models with the couplingspoint
	void syncronisation_coupled_models(const double timepoint, const double delta_t, const double coast_water_level, const bool time_check, const int internal_counter);

	///Get the distance from the coupling point to the element mid point
	double get_distancecoupling2elem_mid(void);

	///Get the index of the coupled fpl-section
	int get_fpl_section_index(void);

	///Get the flag if a coupling due to a break is applicable
	bool get_break_flag(void);
	///Get a flag if the break width is as same as the total overflow width => break is total
	bool get_break_is_total(void);
	///Set the flag if a coupling due to a break is applicable
	void set_break_flag(const bool flag);

	///Set the break width; the break height is related to the base points
	bool add_break_width(const double delta_break_width);
	///Get the mid basepoint height for a break coupling (global)
	double get_mid_basepoint_height(void);
	///Get the fixed basepoint height for a break coupling (global)
	double get_fixed_basepoint_height(void);
	//Calculate the current delta h between coast and floodplain in case of an break
	void calculate_delta_h_break(const double co_waterlevel);
	///Get a pointer to the current delta h between coast and floodplain in case of an break
	double *get_delta_h_break(void);
	///Set the starting break width
	void set_begin_break_width(const double begin_break_width);
	///Set a reduction of the overflow height in the starting breach (slow reduction of the height due to numerical reasons)
	bool set_overflow_height_reduction(const double height_reduction);
	///Set the mid basepoint height as the break height
	void set_mid_basepoint2breakheight(void);

	///Set the flag, that a breach is stopped here to true
	void set_stop_break_flag(void);
	///Get the flag, that a breach is stopped here
	bool get_stop_break_flag(void);

	///Set the index of the break class
	void set_index_break_class(const int index);
	///Get the index of the break class
	int get_index_break_class(void);

	///Get the current discharge through a break
	double get_q_break(void);
	///Get the current velocity through a break
	double get_v_break(void);
	
	///Get the maximum waterlevel at this coupling point by the maximum delta waterlevel relevant for a break 
	_hyd_max_values get_max_h_2break(void);
	///Get the delta waterlevel between river and floodplain at this coupling point
	double get_max_deltah_2break(void);
	///Get maximum waterlevel at this coupling point related to the mid base points of the profiles 
	_hyd_max_values get_max_h(void);

	///Reset the break width
	void reset_break_width(void);

	///Reset the hydrological balance values and the maximum calculated values
	void reset_hydro_balance_max_values(void);
	///Reset the smoothing calculation members
	void reset_smoothing(void);

	//Output the discharges of break to file (just for testing)
	//void output_discharge_break_list(void);


	///Get the maximum waterlevel gradient 
	double get_maximum_h_grad(void);

	///Copy operator
	Hyd_Coupling_Point_FP2CO& operator=(const Hyd_Coupling_Point_FP2CO& object);

private:
	//members
	///Height of the coast dikeline
	double coastdikeline_height;
	///Index of the relevant coast dikeline segment
	int coastdikeline_index;
	///Index of the floodplain element
	int floodplain_elem_index;
	///Index of the coupled floodplain
	int floodplain_index;
	///Pointer to the floodplain element
	Hyd_Element_Floodplain *floodplain_elem;

	///Pointer to the upward dikeline point
	Hyd_Floodplain_Dikeline_Point *ptr_upstream_point;
	///Pointer to the downward dikeline point
	Hyd_Floodplain_Dikeline_Point *ptr_downstream_point;

	///Poleni coefficient (0.577) for a broad crested weir is taken
	double poleni_coefficient;

	///Flag if an overflow is desired
	bool overflow_flag;

	///Total flow width (total width minus fixed width reduction)
	double total_flow_width;

	///Index of a Fpl-section
	int fpl_section_id;

	///Mid base point height of the two points or the floodplain element
	double mid_basepoint;
	///Mid base point height of the two points of a dikeline
	double mid_basepoint_point;

	///Overflow width
	double overflow_width;
	///Waterlevel difference between river and floodplain related to the breakheight
	double delta_h_fp2co_break;
	///Maximum waterlevel at this coupling point for a break; this is used for a coupling between river- and floodplain-model via a break 
	_hyd_max_values max_h_2break;
	///Maximum delta h for a break
	double max_deltah2break;
	///Maximum waterlevel at this coupling point related to the mid base points of the profiles; this is essential for the coupling to the fpl-system  
	_hyd_max_values max_h;

	///Flag if a break is set
	bool break_flag;
	///Flag if a breach is stopped here, e.g. weir, bridge types or inflow and outflow
	bool stop_break_flag;
	///Index of the break class; it show also if a break is currently applied to this point
	int index_break;
	///Sill height of the break
	double break_height;
	///Width of a break
	double break_width;
	///Flow velocity through the break
	double coupling_v_break;
	///Current coupling discharge through the break
	double current_q_break;
	///The coupling discharge through a break one internal timestep before for smoothing
	double old_q_break;
	///Maximum of the coupling velocity through abreak
	_hyd_max_values max_coupling_v_break;
	///Defines the values for a hydrological balance of this coupling thruogh the break
	_hyd_hydrological_balance coupling_volume_break;



	///Current gradient of overflow discharge
	double grad_q_current;
	///Gradient one syncronisation step before of overflow discharge
	double grad_q_before;
	///Smoother for the oscilations of overflow discharge
	double oscilation_smoother;
	///Counter if there are no oscilation of overflow discharge
	double no_osci_counter;
	///Counter if there are  oscilation of overflow discharge
	double number_osci_counter;


	///Current gradient of discharge due to dikebreak coupling
	double grad_q_break_current;
	///Gradient one syncronisation step before of discharge due to dikebreak coupling
	double grad_q_break_before;
	///Smoother for the oscilations of discharge due to dikebreak coupling
	double oscilation_smoother_break;
	///Counter if there are no oscilation of discharge due to dikebreak coupling
	double no_osci_counter_break;
	///Counter if there are oscilation of discharge due to dikebreak coupling
	double number_osci_counter_break;
	//List of the break discharges over time just for testing)
	//_hyd_dicharge_lists break_q_list;

	///Stored gradient one syncronisation step before
	double store_grad_q_before;
	///Stored smoother for the oscilations
	double store_oscilation_smoother;
	///Stored counter if there are no oscilation
	double store_no_osci_counter;
	///Stored counter if there are  oscilation of overflow discharge
	double store_number_osci_counter;
	///Stored gradient one syncronisation step before of discharge due to dikebreak coupling
	double store_grad_q_break_before;
	///Stored smoother for the oscilations of discharge due to dikebreak coupling
	double store_oscilation_smoother_break;
	///Stored counter if there are no oscilation of discharge due to dikebreak coupling
	double store_no_osci_counter_break;
	///Stored counter if there are oscilation of discharge due to dikebreak coupling
	double store_number_osci_counter_break;
	///Stored the coupling discharge through a break one internal timestep before for smoothing
	double store_old_q_break;





	//method
	///Syncronisation of the coupled models with the couplingspoint for overflow
	void syncronisation_coupled_models_overflow(const double timepoint, const double coast_water_level, const double h_one);
	///Syncronisation of the coupled models with the couplingspoint for the discharge throught the break
	void syncronisation_coupled_models_break(const double timepoint, const double coast_water_level, const double h_one);
	///Smooth the coupling discharge with the coupling discharge calculated one internal timestep before
	double smooth_coupling_discharge(const double q_current, double *old_q);
	///Smooth the coupling discharge with the coupling dikebreak discharge calculated one internal timestep before
	double smooth_coupling_dikebreak_discharge(const double q_current, double *old_q);

	///Calculate the maximum waterlevel at the maximim waterlevel difference between coast and floodplain
	void calculate_max_h2break(const double timepoint, const double coast_water_level);


	///Predict the values
	void predict_values(const int int_counter);




};
#endif
