#pragma once
/**\class Hyd_Coupling_RV2RV
	\author Daniel Bachmann et al.
	\author produced by the Institute of Hydraulic Engineering (IWW), RWTH Aachen University
	\version 0.0.1                                                              
	\date 2009 
*/
#ifndef HYD_COUPLING_RV2RV_H
#define HYD_COUPLING_RV2RV_H

//hyd-class
//river model class
#include "Hyd_Model_River.h"
#include "Hyd_Coupling_Point_RV2FP_List.h"

//system_sys_ classes
#include "_Sys_Common_System.h"
//forward
class Hyd_Hydraulic_System;


///Coupling management-class for the coupling between a river- (RV) and a river-model (RV) \ingroup hyd
/**
The coupling between a river- and a river-model is a linear coupling. The coupling points (Hyd_Coupling_Point_RV2FP)
are gathered and managed in a list (Hyd_Coupling_Point_RV2FP_List). The flow between the models is evaluated as an overflow
via the Poleni-formula asA river-model requieres two lists:
One for the left bank-line, one for the right bank-line (in flow direction). These lines are represented
by the outer points (left/right) of the river profiles (_Hyd_River_Profile_Type). In this class just the coupling
between one floodplain- to one river-model is managed. Thus, it gets necessary to merge the lists after evaluation
all couplings of one river-model to diverse floodplain-models. This is done in Hyd_Coupling_RV2FP_Merged.

\see Hyd_Coupling_Point_RV2FP, Hyd_Coupling_Point_RV2FP_List
*/
class Hyd_Coupling_RV2RV
{
public:
	///Default constructor
	Hyd_Coupling_RV2RV(void);
	///Default destructor
	virtual ~Hyd_Coupling_RV2RV(void);

	//methods
	///The pointer of the models for coupling are set
	void set_ptr_coupling(Hyd_Model_River *inflow_river, Hyd_Model_River *outflow_river);
	///The couplings are initialized
	virtual void init_coupling(void);
	///Synchronise the models
	virtual void synchronise_models(void);
	///Reset the coupling discharges
	virtual void reset_coupling_discharges(void);
	///Output the header for the coupled model indizes
	static void output_header_coupled_indices(ostringstream *cout);
	///Output the indizes of the coupled model
	virtual void output_index_coupled_models(ostringstream *cout, const int number);

	///Get if the lateral junction is on the left or right bank (flow direction inflow river)
	bool get_left_bank_flag_inflow(void);
	///Get if the junction is lateral
	bool get_lateral_flag_inflow(void);

	///Insert the junction to a list of RV2FP-coupling points
	virtual void insert_junction_inflow_point2RV2FPlist(Hyd_Coupling_Point_RV2FP_List *list, const int river_index);

	///Clone the river to river couplings
	void clone_couplings(Hyd_Coupling_RV2RV *coupling, Hyd_Hydraulic_System *system);

protected:
	//members
	///Pointer to the inflow rivermodel
	Hyd_Model_River *inflow_rv_model;
	///Pointer to the outflow rivermodel
	Hyd_Model_River *outflow_rv_model;

	///Pointer to upstream inflow river profile
	_Hyd_River_Profile *inflow_profile_up;
	///Pointer to downstream inflow river profile
	_Hyd_River_Profile *inflow_profile_down;

	///Index of the inflow river profile
	int index_inflow_profile_down;
	///Index of the upwards inflow profile for reason of waterlevel interpolation between the profiles
	int index_inflow_profile_upwards;
	///Geometrical point where the inflow river is intercepted
	_Hyd_Coupling_Point inflow_point;
	///Flag at which bank side of the river inflow is intercepted by the coupling river (left:=true; right:=false)
	bool left_flag_inflow;


	///Total distance inflow profiles
	double total_distance_inflow;
	///Distance to the downwards inflow profile
	double distance_inflow_downwards;
	///Distance to the beginning of the river
	double distance2river_begin_inflow;

	///Flag if the coupling is applied
	bool coupling_flag_inflow;
	///Flag if the inflow is lateral (true) or an inflow as inflow boundary into the river (inflow profile)
	bool lateral_flag_inflow;

	///Flag for a horizontal backwater from downstream
	bool horizontal_backwater_flag_inflow;
	///Flag for a horizontal backwater from upstream
	bool horizontal_backwater_flag_inflow_upstream;

	///Mid-factor to the downstream profile for interpolation
	double mid_fac_down_inflow;
	///Mid-factor to the upstream profile for interpolation
	double mid_fac_up_inflow;

	///The mid height of the inflow river section
	double mid_height;

	//methods
	///Calculate the distance to the inflow profiles via an interception of last midline segment of the secondary river section and the river section polygon of the main channel
	virtual void calculate_distances_inflow(Hyd_Floodplain_Polygon *river_section_polygon, Geo_Segment *river_section_midline); 
	///Check the connection of the rivers; the global z_min coordinate if the outflow profile have to be greater/equal to the mid z_min coordinate of the inflow profiles
	virtual void check_connection(void);

	///Calculate the current mid-waterlevel in the main channel via interpolation
	double calculate_mid_waterlevel(void);

	///Calculate the mid factors for the interpolation of the waterlevel in the main channel
	void calculate_mid_factors(void);

	///Set error(s)
	Error set_error(const int err_type);
	///Set warning(s)
	Warning set_warning(const int warn_type);
};
#endif
