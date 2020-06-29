#pragma once
/**\class Hyd_Coupling_Point_RV2FP_List
	\author Daniel Bachmann et al.
	\author produced by the Institute of Hydraulic Engineering (IWW), RWTH Aachen University
	\version 0.0.1                                                              
	\date 2009 
*/
#ifndef HYD_COUPLING_POINT_RV2FP_LIST_H
#define HYD_COUPLING_POINT_RV2FP_LIST_H


//system_hyd 
#include "Hyd_Coupling_Point_RV2FP.h"
//base class
#include "_Hyd_Coupling_Point_List.h"




//forward deklaration
class Hyd_Coupling_RV2FP_Structure;
class Hyd_Coupling_RV2FP_Dikebreak;
class Hyd_Hydraulic_System;

///List-class where the coupling points between a river- (RV) and a floodplain-model (FP) are stored and managed\ingroup hyd
/**
In this class the coupling points between a river- (RV) and a floodplain-model (FP) are stored and managed in a list.
The related polysegment to the list are the bank-lines (left/right in flow-direction) of the river-model (Hyd_Model_River). 
This list is managed in Hyd_Coupling_RV2FP for one floodplain-model, alternatively in Hyd_Coupling_RV2FP_Merged
for multiple floodplain-models.
\see Hyd_Coupling_Point_RV2FP, Hyd_Model_Floodplain
*/
class Hyd_Coupling_Point_RV2FP_List : public _Hyd_Coupling_Point_List
{

public:
	///Default constructor
	Hyd_Coupling_Point_RV2FP_List(void);
	///Default destructor
	~Hyd_Coupling_Point_RV2FP_List(void);

	//methods
	///Get the number of coupling points
	int get_number_couplings(void);
	///Get the total list as a pointer
	Hyd_Coupling_Point_RV2FP* get_total_list(void);
	///Get a specific coupling points with the given index
	Hyd_Coupling_Point_RV2FP get_couplingpoint(const int index);
	///Get a pointer to specific coupling points with the given index
	Hyd_Coupling_Point_RV2FP* get_ptr_couplingpoint(const int index);

	///Get the next coupling point upwards in the list
	Hyd_Coupling_Point_RV2FP* get_next_point_upwards(Hyd_Coupling_Point_RV2FP* coupling_point);
	///Get the next coupling point downwards in the list
	Hyd_Coupling_Point_RV2FP* get_next_point_downwards(Hyd_Coupling_Point_RV2FP* coupling_point);

	///Get the first coupling points of the list
	Hyd_Coupling_Point_RV2FP get_first_couplingpoint(void);
	///Get the last coupling points of the list
	Hyd_Coupling_Point_RV2FP get_last_couplingpoint(void);

	///Output the setted members
	void output_setted_members(ostringstream *cout);

	///Set a new coupling points
	void set_new_couplingpoint(Hyd_Coupling_Point_RV2FP *new_point);

	///Set if it is the left river bank line or the right one (in flowdirection) 
	void set_leftriver_bank_line(const bool flag);
	//Get if it is the left river bank line or the right one (in flowdirection) 
	bool get_leftriver_bank_line(void);

	///Find a coupling point in the list in which area the given distance from the beginning of the river lies in
	Hyd_Coupling_Point_RV2FP* find_coupling_point(const double distance, _Hyd_Coupling_Point *point);
	///Find a corresponding coupling point to a given distance along the defining polysegment
	Hyd_Coupling_Point_RV2FP* find_coupling_point(const double distance);

	///Insert two coupling point in the list from the given midpoint of a structure or riverjunction up- and downstream and set a stop break flag and a no overflow flag
	void insert_constricting_coupling_points(const double width, _Hyd_Coupling_Point *point);

	///Add additional coupling points for more detailed coupling between the models
	void add_additional_coupling_points(void);

	///Find the distance along the river bank line to a given related profile
	bool find_distance2related_profile(const int related_profile_index, double *distance);


	///Delete the list
	void delete_list(void);

	///Sort the points after the distance to the beginning of the defining polysegment
	void sort_distance_along_polysegment(void);

	///Calculate for all points the distance to the next upward point and downward point
	void calculate_alldistance_up_down(void);

	///Add the relevant coupling points of the defining_polysegment to the list
	void add_relevant_polysegment_points(Hyd_Floodplain_Polygon *fp_boundary);

	///Convert the profiles indices to a pointer to the profile
	void convert_profiles_index2pointer(Hyd_Model_River *river_model);

	///Transfer the information of the coupled elements/profiles etc to the coupling points
	void transfer_informations2points(void);

	///Reset the coupling dicharges of the coupling points
	void reset_coupling_discharge(void);
	///Reset the coupling points
	void reset_points(void);

	///Syncronisation of the coupled models with the stored couplingspoints in the list
	void syncronisation_models_bylistpoints(const double timepoint, const double delta_t, const bool time_check, const int internal_counter);
	///Get the maximum waterlevel gradient 
	double get_maximum_h_grad(void);


	///Merge this list with another given list by same defining polysegments; the merged list (result) is the given list
	void merge_coupling_list(Hyd_Coupling_Point_RV2FP_List *merging_list);

	///Check if the connecting segment between coupling point and floodplain element mid_point is intersecting a river_bankline; a coupling is than not possible
	void check_intercepting_riverbankline2connecting_segment(Hyd_Floodplain_Polysegment *river_bankline);

	///Clone the coupling point list
	void clone_list(Hyd_Coupling_Point_RV2FP_List *list, Hyd_Hydraulic_System *system, const int rv_number);

	///Get pointer to the coupling point specified by the given
	Hyd_Coupling_Point_RV2FP* get_ptr_couplingpoint(Hyd_Coupling_Point_RV2FP coupling_point);

	///Set the integer list of the different fpl-section ids
	void set_list_fpl_sec_id(void);
	///Get a pointer to the list of the fpl-section ids
	QList<int>* get_ptr_fpl_section_ids(void);

	///Get the maximum waterlevel for a coupling of hydraulic and fpl-system and the corresponding coupling point id
	void get_max_h2fpl(const int fpl_sec_id, _hyd_max_values *max_h, int *point_id);

	///Get the maximum waterlevel for a coupling of river- and floodplain-model via a break
	void get_max_h2break(const int fpl_sec_id, _hyd_max_values *max_h, int *point_id);

private:

	//members
	///Coupling points in list
	Hyd_Coupling_Point_RV2FP *points;
	///Decide if it is the left river bank line or the right one (in flowdirection)
	bool left_river_flag;

	///Connecting Geo_Segment between a coupling points and the element midpoint
	Geo_Segment connecting_segment;
	///The mid_point between two coupling points
	Geo_Point mid_point;

	///List of different fpl-section ids
	QList <int> fpl_sec_ids;


	//methods
	///Delete the allocated coupling points
	void delete_couplingpoints(void);
	///Allocate the coupling points
	void allocate_couplingpoints(void);

	///Get the connecting Geo_Segment between the mid_point of two coupling points and floodplain element mid_point for the given point index
	Geo_Segment *get_connecting_segment_point_elem(const int index);

	///Get the mid_point between two coupling points
	Geo_Point *get_mid_point(const int index);



	///Calculate the distance to the startpoint 
	void calculate_distances(void);

	///Verify the merged list after merging
	void verify_merged_list(void);

	///Set error(s)
	Error set_error(const int err_type);
};
#endif
