#pragma once
/**\class Hyd_Coupling_RV2RV_Diversion
	\author Daniel Bachmann et al.
	\author produced by the Institute of Hydraulic Engineering (IWW), RWTH Aachen University
	\version 0.0.1                                                              
	\date 2009 
*/
#ifndef HYD_COUPLING_RV2RV_DIVERSION_H
#define HYD_COUPLING_RV2RV_DIVERSION_H

//hyd-class
//base class
#include "Hyd_Coupling_RV2RV.h"
//Base class of the coupling structures
#include "_Hyd_Coupling_Structure.h"
//Types of coupling structures
#include "Hyd_Coupling_Structure_Weir.h"
#include "Hyd_Coupling_Structure_Gate.h"
//For a input per file
#include "Hyd_Parse_Coupling_Diversion.h"


///Coupling-class for a coupling between a river-model and a river-model via a diversion channel \ingroup hyd
/**
This class represents a user-defined type of coupling between a river- and a river-model. 
It is a punctuell coupling. The inflow profile of the diversion channel is coupled
to another river model via a hydraulic structure (_Hyd_Coupling_Structure). The outflow of the
diversion channel can be a coupling to another river-/floodplain-/coast-model or a boundary condition.
Which river-model represents the diversion channel is defined by the user.

\see Hyd_Coupling_RV2RV
*/
class Hyd_Coupling_RV2RV_Diversion : public Hyd_Coupling_RV2RV, public _Sys_Common_System 
{
public:
	///Default constructor
	Hyd_Coupling_RV2RV_Diversion(void);
	///Default destructor
	~Hyd_Coupling_RV2RV_Diversion(void);

	//methods

	///Input of the diversion channel parameters per file via a parser (Hyd_Parse_Coupling_Diversion)
	void input_parameter_per_file(const int index, const string file);
	///Input of the diversion channel parameters from a database selection
	void input_parameter_per_database(const QSqlTableModel *results, const int index, const bool output_flag);

	///Create the database table for the diversion channel data
	static void create_table(QSqlDatabase *ptr_database);
	///Set the database table for the diversion channel data: it sets the table name and the name of the columns and allocate them
	static void set_table(QSqlDatabase *ptr_database);
	///Delete all data in the database table for the diversion channel data
	static void delete_data_in_table(QSqlDatabase *ptr_database);
	///Close and delete the database table for the diversion channel data
	static void close_table(void);
	///Select and count the number of relevant diversion channels in a database table
	static int select_relevant_dv_channels_database(QSqlTableModel *results, const _sys_system_id id, const bool with_output = true);


	///Transfer the diversion channel data of a file into a database table
	void transfer_diversion_channel2database_table(QSqlDatabase *ptr_database);


	///Convert the indices of the coupled models into pointer
	void convert_indices2pointer(Hyd_Model_River *river_models, const int number_rv_models);
	///The couplings are initialized
	void init_coupling(void);
	///Synchronise the models
	void synchronise_models(void);
	///Reset the coupling discharges
	void reset_coupling_discharges(void);

	///Output the members
	void output_members(void);
	///Output the header for the coupled model indizes
	static void output_header_coupled_indices(ostringstream *cout);
	///Output the indizes of the coupled model
	void output_index_coupled_models(ostringstream *cout, const int number);

	///Insert the junction to a list of RV2FP-coupling points at inflow profile of the diversion channel
	void insert_junction_inflow_point2RV2FPlist(Hyd_Coupling_Point_RV2FP_List *list, const int river_index);
	///Insert the junction to a list of RV2FP-coupling points at outflow profile of the diversion channel
	void insert_junction_outflow_point2RV2FPlist(Hyd_Coupling_Point_RV2FP_List *list, const int river_index);

	///Clone the river to river couplings via diversion channel
	void clone_couplings(Hyd_Coupling_RV2RV_Diversion *coupling, Hyd_Hydraulic_System *system);

private:

	//members

	///Pointer to the table for the the dikebreak/wallbreak data in a database
	static Tables *diversion_channel_table;
	///Index of the coupling
	int index;
	///Index of the river model which represents the diversion channel
	int index_diversion_rv_model;
	///Pointer to the inflow rivermodel
	Hyd_Model_River *diversion_rv_model;

	///Flag if the outflow of the diversion channel goes to another river model (true);
	bool outflow_river_flag;
	///Index of the outflow river, where the outflow of the diversion channel goes into
	int index_outflow_rv;
	///Index of the outflow river profile downstream 
	int index_outflow_profile_down;
	///Index of the outflow river profile upwards
	int index_outflow_profile_upwards;

	///Pointer to upstream outflow river profile
	_Hyd_River_Profile *outflow_profile_up;
	///Pointer to downstream outflow river profile
	_Hyd_River_Profile *outflow_profile_down;

	///Total distance between the outflow profiles
	double total_distance_outflow;
	///Distance to the downwards outflow profile
	double distance_outflow_downwards;
	///Distance to the beginning of the outflow river (outflow of the div-channel)
	double distance2river_begin_outflow;
	///Geometrical point where the outflow river (outflow of the div-channel) is intercepted
	_Hyd_Coupling_Point outflow_point;
	///Flag at which bank side of the outflow river (outflow of the div-channel) is intercepted by the coupling river (left:=true; right:=false)
	bool left_flag_outflow;

	///Flag for a horizontal backwater in the outflow river from downstream
	bool horizontal_backwater_flag_outflow;
	///Flag for a horizontal backwater in the outflow river from upstream
	bool horizontal_backwater_flag_outflow_upstream;
	///Mid-factor to the downstream profile for interpolation in the outflow river
	double mid_fac_down_outflow;
	///Mid-factor to the upstream profile for interpolation in the outflow river
	double mid_fac_up_outflow;
	///Flag if the coupling is applied to the outflow of the diversion channel
	bool coupling_flag_outflow;


	///Index of the inflow river; inflow river is the river from which the diversion channel takes his inflow
	int index_inflow_rv;
	///Specifies the type of lateral inflow to the diversion channel
	_hyd_1d_outflow_types inflow_type;
	///Maximum of the mid of height of the two profiles of the inflow river (global) and the global zmin coordinate of the diversion channel inflow profile 
	double mid_height_inflow;

	///Pointer to the coupling structure; it will be decided which type is needed (weir/gate)
	_Hyd_Coupling_Structure *coupling_struct;

	//methods

	///Allocate the given coupling structure type
	void allocate_coupling_structure(void);

	///Set the diversion channel member with a structure where the parser has gathered the informations (_hyd_div_channel_params)
	void set_diversion_channel_params(_hyd_div_channel_params parameter);

	///Calculate the distance to the outflow profiles via an interception of the last midline segment of the diversion channel and a river section polygon of the outflow river
	void calculate_distances_outflow(Hyd_Floodplain_Polygon *river_section_polygon, Geo_Segment *river_section_midline);
	///Calculate the distance to the inflow profiles via an interception of the first midline segment of the diversion channel and a river section polygon of the inflow river
	void calculate_distances_inflow(Hyd_Floodplain_Polygon *river_section_polygon, Geo_Segment *river_section_midline); 

	///Check the connection of the rivers; the global z_min coordinate if the diversion channel´s outflow profile have to be greater/equal to the mid z_min coordinate of the profiles or the outflow river
	void check_connection(void);

	///Calculate the current mid-waterlevel via interpolation in the outflow river
	double calculate_mid_waterlevel_outflow(void);

	///Calculate the mid factors for the interpolation of the waterlevel in the outflow river
	void calculate_mid_factors_outflow(void);

	///Set error(s)
	Error set_error(const int err_type);
	///Set warning(s)
	Warning set_warning(const int warn_type);

};

#endif