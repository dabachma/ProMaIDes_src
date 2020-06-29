#pragma once
/**\class Hyd_Coupling_Point_FP2CO_List
	\author Daniel Bachmann et al.
	\author produced by the Institute of Hydraulic Engineering (IWW), RWTH Aachen University
	\version 0.0.1                                                              
	\date 2009 
*/
#ifndef HYD_COUPLING_POINT_FP2CO_LIST_H
#define HYD_COUPLING_POINT_FP2CO_LIST_H


//system_hyd 
#include "Hyd_Coupling_Point_FP2CO.h"
//base class
#include "_Hyd_Coupling_Point_List.h"

//forward
class Hyd_Hydraulic_System;

///List-class where the coupling points between the coast- (CO) and floodplain-models (FP) are stored and managed \ingroup hyd
/**
In this class the coupling points between the coast- (CO) and floodplain-models (FP) are stored and managed in a list.
The related polysegment to the list is the coast-line of the coast-model (Hyd_Coast_Model).
This list is managed in Hyd_Coupling_FP2CO for one floodplain-model, alternatively in Hyd_Coupling_FP2CO_Merged
for multiple floodplain-models.
\see Hyd_Coupling_Point_FP2CO, Hyd_Coast_Model
*/
class Hyd_Coupling_Point_FP2CO_List: public _Hyd_Coupling_Point_List
{

public:
	///Default constructor
	Hyd_Coupling_Point_FP2CO_List(void);
	///Default destructor
	~Hyd_Coupling_Point_FP2CO_List(void);

	//methods
	///Get the number of coupling points
	int get_number_couplings(void);
	///Get the total list as a pointer
	Hyd_Coupling_Point_FP2CO* get_total_list(void);
	///Get a specific coupling pointswith the given index
	Hyd_Coupling_Point_FP2CO get_couplingpoint(const int index);
	///Get a pointer to specific coupling points with the given index
	Hyd_Coupling_Point_FP2CO* get_ptr_couplingpoint(const int index);
	///Get pointer to the coupling point specified by the given
	Hyd_Coupling_Point_FP2CO* get_ptr_couplingpoint(Hyd_Coupling_Point_FP2CO coupling_point);
	///Get the first coupling points of the list
	Hyd_Coupling_Point_FP2CO get_first_couplingpoint(void);
	///Get the last coupling points of the list
	Hyd_Coupling_Point_FP2CO get_last_couplingpoint(void);

	///Output the setted members
	void output_setted_members(ostringstream *cout);

	///Set a new coupling points
	void set_new_couplingpoint(Hyd_Coupling_Point_FP2CO *new_point);
	///Find the distance along the coast line to a given related point
	bool find_distance2related_point(const int related_point_index, double *distance);

	///Delete the list
	void delete_list(void);

	///Sort the points after the distance to the beginning of the defining polysegment
	void sort_distance_along_polysegment(void);

	///Calculate for all points the distance to the next upward point and downward point
	void calculate_alldistance_up_down(void);

	///Add the relevant coupling points of the defining_polysegment
	void add_relevant_polysegment_points(Hyd_Floodplain_Polygon *fp_boundary);

	///Get the next coupling point upwards in the list
	Hyd_Coupling_Point_FP2CO* get_next_point_upwards(Hyd_Coupling_Point_FP2CO* coupling_point);
	///Get the next coupling point downwards in the list
	Hyd_Coupling_Point_FP2CO* get_next_point_downwards(Hyd_Coupling_Point_FP2CO* coupling_point);
	
	///Get a point between the coupling points for the given distance from the beginning of the river; if nothing is found a NULL-pointer is returned
	Hyd_Coupling_Point_FP2CO* find_coupling_point(const double distance, _Hyd_Coupling_Point *point);
	///Find a corresponding coupling point to a given distance along the defining polysegment
	Hyd_Coupling_Point_FP2CO* find_coupling_point(const double distance);

	///Transfer the information of the coupled elements/profiles etc to the coupling points
	void transfer_informations2points(void);

	///Reset the coupling dicharges of the coupling points
	void reset_coupling_discharge(void);
	///Reset the coupling points
	void reset_points(void);

	///Syncronisation of the coupled models with the stored couplingspoints in the list
	void syncronisation_models_bylistpoints(const double timepoint, const double delta_t, const double coast_water_level, const bool time_check, const int internal_counter);
	///Get the maximum waterlevel gradient 
	double get_maximum_h_grad(void);


	///Merge this list with another given list by same defining polysegments; the merged list (result) is the given list
	void merge_coupling_list(Hyd_Coupling_Point_FP2CO_List *merging_list);

	///Insert two coupling point in the list from the given midpoint of a riverjunction 
	void insert_constricting_coupling_points(const double width, _Hyd_Coupling_Point *point);

	///Add additional coupling points for more detailed coupling between the models
	void add_additional_coupling_points(void);

	///Clone the coupling point list
	void clone_list(Hyd_Coupling_Point_FP2CO_List *list, Hyd_Hydraulic_System *system);

	///Set the integer list of the different fpl-section ids
	void set_list_fpl_sec_id(void);
	///Get a pointer to the list of the fpl-section ids
	QList<int>* get_ptr_fpl_section_ids(void);

	///Get the maximum waterlevel for a coupling of hydraulic and fpl-system and the corresponding coupling point id
	void get_max_h2fpl(const int fpl_sec_id, _hyd_max_values *max_h, int *point_id);

	///Get the maximum waterlevel for a coupling of coast- and floodplain-model via a break
	void get_max_h2break(const int fpl_sec_id, _hyd_max_values *max_h, int *point_id);

	//Output the discharge list to file (just for testing)
	//void output_discharge_lists(void);

private:

	//members
	///Coupling points in list
	Hyd_Coupling_Point_FP2CO *points;

	///List of different fpl-section ids
	QList <int> fpl_sec_ids;


	//methods
	///Delete the allocated coupling points
	void delete_couplingpoints(void);
	///Allocate the coupling points
	void allocate_couplingpoints(void);

	///Calculate the distance to the startpoint
	void calculate_distances(void);
	//Verify the merged list after merging
	void verify_merged_list(void);


	///Set error(s)
	Error set_error(const int err_type);
};
#endif
