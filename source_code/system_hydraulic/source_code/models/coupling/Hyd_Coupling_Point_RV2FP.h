#pragma once
/**\class Hyd_Coupling_Point_RV2FP
	\author Daniel Bachmann et al.
	\author produced by the Institute of Hydraulic Engineering (IWW), RWTH Aachen University
	\version 0.0.1                                                              
	\date 2009 
*/
#ifndef HYD_COUPLING_POINT_RV2FP_H
#define HYD_COUPLING_POINT_RV2FP_H


//system hyd class
//base class
#include "_Hyd_Coupling_Point.h"
//Class of the coupling partner (Base class of the profile types)
#include "_Hyd_River_Profile.h"

///Coupling point-class for the coupling between a river- (RV) and a floodplain-model (FP) \ingroup hyd
/**
The coupling between the river- and a floodplain-model is a linear coupling. The founded points by a
a geometrical model interception are related to the bank-lines of the river-model (right and left bank lines in flow direction) (Hyd_Model_River).
The flow over the bank-lines is evaluated by the Poleni-formula (overflow). The discharge to/out of the coupling related floodplain standard
element (Hyd_Floodplain_Element_Type_Standard) is set as a discharge boundary condition to this element, constant
over the syncronisation timestep. The discharge to/out of the coupling related profiles is equivalent, but with a opposite sign (+ - /- +).
Generally, there are two related river profiles.
The points are managed and stored in a list (Hyd_Coupling_Point_RV2FP_List).
\see Hyd_Model_Floodplain
*/
class Hyd_Coupling_Point_RV2FP : public _Hyd_Coupling_Point
{
///Class for managing this points in a list
friend class Hyd_Coupling_Point_RV2FP_List;

public:
	///Default constructor
	Hyd_Coupling_Point_RV2FP(void);
	//Copy constructor
	Hyd_Coupling_Point_RV2FP(const Hyd_Coupling_Point_RV2FP& object);
	///Default destructor
	~Hyd_Coupling_Point_RV2FP(void);

	//methods
	///Set the pointer/index members (without the geometrical members) from the given point
	void set_couplingpoint_members(Hyd_Coupling_Point_RV2FP *object);


	///Output the header for the setted member (static)
	static void output_header_setted_member(ostringstream *cout);
	///Output the setted  members
	void output_setted_members(ostringstream *cout);

	///Set the indices for the upwards river profile (corresponding to the index of the polysegment of the river bank line) and the element index of the Hyd_Floodplain_Element, which is used for calculation
	void set_indices(const int up_river_profile, const int floodplain_model_elem);
	///Set the pointers to the river profiles with help of the indices
	void set_pointer_river_profiles(Hyd_Model_River *river_model);

	///Set the indexof the first found Hyd_Floodplain_Element; it is not necessarily used for calculation (e.g. river elements)
	void set_first_found_elem_index(const int index);
	///Set the index of the floodplain element
	void set_index_floodplain_element(const int index);
	///Get the index of the floodplain element
	int get_index_floodplain_element(void);
	///Set the pointers to the floodplain elements
	void set_pointer_floodplain_element(Hyd_Element_Floodplain *floodplain);

	///Set the index of the coupled floodplain
	void set_floodplain_index(const int index);
	///Get the index of the coupled floodplain
	int get_floodplain_index(void);

	///Get the index of the coupled fpl-section
	int get_fpl_section_index(void);

	///Get the mid of waterlevel of the two profiles (global) in the river
	double get_mid_waterlevel(void);
	///Calculate the current mid-waterlevel via the factors in the river
	void calculate_mid_waterlevel(void);
	///Calculate the current delta h between river and floodplain in case of an break
	void calculate_delta_h_break(void);
	///Get a pointer to the current delta h between river and floodplain in case of an break
	double *get_delta_h_break(void);

	///Get the mid height for the overflow-coupling (global)
	double get_mid_height(void);
	///Get the mid basepoint height (profile or element) for a break coupling (global)
	double get_mid_basepoint_height(void);
	///Get the mid basepoint height (just profile) for a break coupling (global)
	double get_mid_basepoint_height_profile(void);

	///Get the pointer to the downstream river profile
	_Hyd_River_Profile* get_downstream_rvprofile(void);
	///Get the pointer to the upstream river profile
	_Hyd_River_Profile* get_upstream_rvprofile(void);
	
	///Get the index of the downstream river profile
	int get_index_downstream_rvprofile(void);
	///Get the index to the upstream river profile
	int get_index_upstream_rvprofile(void);

	///Get the pointer to the floodplain element
	Hyd_Element_Floodplain* get_floodplain_element(void);
	///Get the upwards factor for the mid-value calculation of the mid_height and the mid_waterlevel depending on the distances
	double get_upwards_mid_factor(void);
	///Get the downwards factor for the mid-value calculation of the mid_height and the mid_waterlevel depending on the distances
	double get_downwards_mid_factor(void);

	///Get the distance from the coupling point to the element mid point
	double get_distancecoupling2elem_mid(void);

	///Transfer the coupling characteristics of the coupled elements
	void transfer_coupling_characteristics(const bool left_river_flag);
	///Reset the current coupling discharge of the points and the coupled element
	void reset_coupling_discharge(const bool left_river_flag);
	///Syncronisation of the coupled models with the couplingspoint
	void syncronisation_coupled_models(const double timepoint, const double delta_t, const bool left_river_flag, const bool time_check, const int internal_counter);

	///Set the flag if a coupling due to a overflow is applicable
	void set_overflow_flag(const bool flag);
	///Set the fixed overflow flag to true; it is not more dependent of the profiles
	void set_fixed_overflow_flag(void);

	///Get the flag if a coupling due to a break is applicable
	bool get_break_flag(void);
	///Get a flag if the break width is as same as the total overflow width => break is total
	bool get_break_is_total(void);
	///Set the flag if a coupling due to a break is applicable
	void set_break_flag(const bool flag);

	///Set the break width; the break height is related to the base points
	bool add_break_width(const double delta_break_width);
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

	///Get the maximum waterlevel gradient 
	double get_maximum_h_grad(void);


	///Copy operator
	Hyd_Coupling_Point_RV2FP& operator=(const Hyd_Coupling_Point_RV2FP& object);


private:
	//members
	///Index of the river profile upwards
	int river_profile_index_up;
	///Pointer to the river profile upwards
	_Hyd_River_Profile *river_profile_up;
	///Index of the river profile downwards
	int river_profile_index_down;
	///Pointer to the river profile downwards
	_Hyd_River_Profile *river_profile_down;
	///Index of the coupled floodplain
	int floodplain_index;
	///Index of the floodplain element
	int floodplain_elem_index;
	///Index of the first found coupling element
	int first_found_elem_index;
	///Pointer to the floodplain element
	Hyd_Element_Floodplain *floodplain_elem;

	///Index of a Fpl-section
	int fpl_section_id;

	///Mid of height of the two profiles
    double mid_height;
	///Mid base point height of the two profiles or the floodplain element
    double mid_basepoint;
	///Mid base point of the two profiles
	double mid_basepoint_profile;
	///Factor (upwards) for the mid-value calculation of the mid_height and the mid_waterlevel depending on the distances
	double mid_fac_up;
	///Factor (downwards) for the mid-value calculation of the mid_height and the mid_waterlevel depending on the distances
	double mid_fac_down;
	///Mid of waterlevel of the two profiles in the river
	double mid_waterlevel;


	///Total flow width (total width minus fixed width reduction)
	double total_flow_width;

	///Flag if an overflow is desired
	bool overflow_flag;
	///Flag if the overflow flag is fixed (e.g. structure, river junctions)
	bool overflow_flag_fixed;

	///Overflow width
	double overflow_width;
	///Waterlevel difference between river and floodplain related to the breakheight
	double delta_h_rv2fp_break;
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

	///Flag for a horizontal backwater from downstream
	bool horizontal_backwater_flag;
	///Flag for a horizontal backwater from upstream
	bool horizontal_backwater_flag_upstream;


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


	///Predicted waterlevel two (RV)
	double predicted_h_two;
	///Corrected waterlevel two (RV)
	double corrected_h_two;
	///Calculated waterlevel two (RV)
	double calc_h_two;
	///Old calculated waterlvel two (RV)
	double old_calc_h_two;

	///List of gradients of waterlevel two (RV)
	QList <double> gradient_list_h_two;
	///Predicted gradient of waterlevel two (RV)
	double gradient_h_two;

	///Stored gradient one syncronisation step before
	double store_grad_q_before;
	///Stored smoother for the oscilations
	double store_oscilation_smoother;
	///Stored counter if there are  oscilation of overflow discharge
	double store_number_osci_counter;
	///Stored counter if there are no oscilation
	double store_no_osci_counter;
	///Stored gradient one syncronisation step before of discharge due to dikebreak coupling
	double store_grad_q_break_before;
	///Stored smoother for the oscilations of discharge due to dikebreak coupling
	double store_oscilation_smoother_break;
	///Stored counter if there are oscilation of discharge due to dikebreak coupling
	double store_number_osci_counter_break;
	///Stored counter if there are no oscilation of discharge due to dikebreak coupling
	double store_no_osci_counter_break;
	///Stored the coupling discharge through a break one internal timestep before for smoothing
	double store_old_q_break;

	//methods
	///Transfer the coupling characteristics of the coupled elements for a left river bank (in flow direction)
	void transfer_coupling_characteristics_leftbank(void);
	///Transfer the coupling characteristics of the coupled elements for a right river bank (in flow direction)
	void transfer_coupling_characteristics_rightbank(void);
	///Syncronisation of the coupled models with the couplingspoint for overflow
	void syncronisation_coupled_models_overflow(const double timepoint, const double poleni, const double h_one, const double h_two);
	///Syncronisation of the coupled models with the couplingspoint for the discharge throught the break
	void syncronisation_coupled_models_break(const double timepoint, const double h_one, const double h_two);
	///Smooth the coupling discharge with the coupling discharge calculated one internal timestep before
	double smooth_coupling_discharge(const double q_current, double *old_q);
	///Smooth the coupling discharge with the coupling dikebreak discharge calculated one internal timestep before
	double smooth_coupling_dikebreak_discharge(const double q_current, double *old_q);


	///Calculate the maximum waterlevel at the maximim waterlevel difference between river and floodplain
	void calculate_max_h2break(const bool left_flag, const double time_point);

	///Predict the values
	void predict_values(const int int_counter);

};
#endif
