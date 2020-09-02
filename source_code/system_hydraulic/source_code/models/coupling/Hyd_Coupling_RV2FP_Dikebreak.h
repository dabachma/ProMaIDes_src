#pragma once
/**\class Hyd_Coupling_RV2FP_Dikebreak
	\author Daniel Bachmann et al.
	\author produced by the Institute of Hydraulic Engineering (IWW), RWTH Aachen University
	\version 0.0.1                                                              
	\date 2009 
*/
#ifndef HYD_COUPLING_RV2FP_DIKEBREAK_H
#define HYD_COUPLING_RV2FP_DIKEBREAK_H



//hyd-system
#include "_Hyd_Coupling_Dikebreak.h"
#include "Hyd_Parse_Coupling_RV2FP_Dikebreak.h"
#include "Hyd_Coupling_RV2FP.h"
#include "Hyd_Coupling_Point_RV2FP.h"





///Coupling-class for a coupling between a river-model and a floodplain-model via a hydraulic dikebreak/wallbreak \ingroup hyd
/**
This class represents a special type of coupling between a river- and a floodplain-model. The breach development due to a
dikebreak/wallbreak is calculated here. 
This class uses the merged list for the standard overflow
coupling between a river-model and a floodplain-model (Hyd_Coupling_Point_RV2FP), where the overflow height is reduced to the
base-points (left/right bank) of the river profiles.

\see Hyd_Coupling_Point_RV2FP
*/
class Hyd_Coupling_RV2FP_Dikebreak : public _Hyd_Coupling_Dikebreak
{
public:
	///Default constructor
	Hyd_Coupling_RV2FP_Dikebreak(void);
	///Default destructor
	~Hyd_Coupling_RV2FP_Dikebreak(void);

	//methods

	///Input of the dike-/wallbreak coupling parameters per file via a parser (Hyd_Parse_Coupling_RV2FP_Dikebreak)
	void input_parameter_per_file(const int index, const string file);
	///Input of the dike-/wallbreak coupling parameters from a database selection
	void input_parameter_per_database(const QSqlQueryModel *results, const int index, const bool output_flag);
	///Set the start parameters of a break via a given data structure
	void input_parameter(const int index_breach, const _hyd_breach_start_parameters params, const bool left_flag);

	
	///Transfer the dikebreak/wallbreak data of a file into a database table
	void transfer_break_data2database_table(QSqlDatabase *ptr_database);

	///Set an additional coupling point, when the break is user-defined
	void set_additional_coupling_point(Hyd_Coupling_RV2FP_Merged *couplings);

	///The couplings are initialized with the already performed RV2FP couplings
	void init_coupling(Hyd_Coupling_RV2FP_Merged *couplings, const int number);
	///The couplings are initialized with the already performed RV2FP couplings; here a list of river profiles is also given for the creation of a list of possible starting breach points (scenario based risk approach)
	void init_coupling(Hyd_Coupling_RV2FP_Merged *couplings, const int number, _Hyd_River_Profile **involved_profiles, const int number_involved_profiles, const int section_id_fpl, _hyd_output_flags *output_flags);
	///The couplings are initialized with the already performed RV2FP couplings; the coupling point index, where the break should begin is directly given (catchment area risk approach)
	void init_coupling(Hyd_Coupling_RV2FP_Merged *couplings, const int number, const int section_id_fpl, const int point_id, _hyd_output_flags *output_flags);
	///Synchronise the models
	void synchronise_models(const double timepoint, const double delta_t, const bool time_check);


	///Output the members
	void output_members(void);
	///Output the header for the coupled model indizes
	static void output_header_coupled_indices(ostringstream *cout);
	///Output the indizes of the coupled model
	void output_index_coupled_models(ostringstream *cout, const int number);
	///Output result members to tecplot file
	void output_results2file_tecplot(double const global_time);
	///Output result members to csv file
	void output_results2file_csv(double const global_time);
	///Output final result members to display/console
	void output_final_results(void);
	///Output final results to database
	void output_final_results(QSqlDatabase *ptr_database, const string id_break, const int bound_sc);


	///Reset the hydrological balance values and the maximum calculated values
	void reset_hydro_balance_max_values(void);

	///Clone the dikebreak coupling
	void clone_couplings(Hyd_Coupling_RV2FP_Dikebreak *coupling, Hyd_Coupling_RV2FP_Merged *merged_couplings, const int number);

	///Initiate the output to a tecplot file
	void init_output2file_tecplot(void);
	///Initiate the output to a csv file
	void init_output2file_csv(void);

private:
	//members

	
	///Index of the river model
	int index_rv_model;
	///Pointer to river model
	Hyd_Model_River *ptr_river;
	///Index of the river profile related to the given distance
	int index_related_profile;
	///Direction of coupling in flow direction (true:= left; false:=right)
	bool left_flag;

	///Distance of the breach begin related to a profile
	double distance_start_related;

	
	///Pointer to the coupling point list
	Hyd_Coupling_Point_RV2FP_List *point_list;

	///Pointer to the coupling point (Hyd_Coupling_Point_RV2FP), where the dikebreak starts
	Hyd_Coupling_Point_RV2FP *start_coupling_point;
	///Pointer to the coupling point (Hyd_Coupling_Point_RV2FP), for the upstream breach developement
	Hyd_Coupling_Point_RV2FP *upstream_coupling_point;
	///Pointer to the coupling point (Hyd_Coupling_Point_RV2FP), for the downstream breach developement
	Hyd_Coupling_Point_RV2FP *downstream_coupling_point;



	///List of other possible starting coupling points for the breach start
	Hyd_Coupling_Point_RV2FP **list_possible_start_points;
	///Number of possible starting coupling points
	int number_possible_start_points;

	//methods

	///Allocate the list of possible starting coupling points
	void allocate_list_possible_start_points(void);
	///Delete the list of possible starting coupling points
	void delete_list_possible_start_points(void);

	///Create a list of possible starting coupling points
	void create_starting_coupling_point_list(_Hyd_River_Profile **involved_profiles, const int number_involved_profiles);
	///Observe and switch, if required, the possible starting coupling points
	void observe_starting_coupling_points(void);
	///Sort the list of the possible starting points after the waterlevel
	void sort_list_possible_start_points(void);


	///Set the dikebreak coupling member with a structure where the parser has gathered the informations (_hyd_rv2fp_dikebreak_coupling_params)
	void set_dikebreak_coupling_params(_hyd_rv2fp_dikebreak_coupling_params parameter);

	///Calculate the downstream breach opening
	void calculate_downstream_opening(void);
	///Calculate the upstream breach opening
	void calculate_upstream_opening(void);

	///Set a new pointer to an upstream coupling point 
	void set_new_upstream_coupling_point(Hyd_Coupling_Point_RV2FP *new_coupling_point);
	///Set a new pointer to a downstream coupling point 
	void set_new_downstream_coupling_point(Hyd_Coupling_Point_RV2FP *new_coupling_point);

	///Calculate the mean discharge/mean velocity over the breach per calculation step
	void calculate_mean_q_v(const double timepoint);

	///Check the dikebreak parameters
	void check_dikebreak_parameter(void);

	///Output the header to the result tecplot file
	void output_header_result2file_tecplot(void);

	///Output the header to the result csv file
	void output_header_result2file_csv(void);

	///Investigate and set the starting coupling point
	bool set_start_coupling_point(void);

	///Get a string of the breach polyline
	string get_breach_polyline(void);

	///Set error(s)
	Error set_error(const int err_type);
	///Set warning(s)
	Warning set_warning(const int warn_type);
};
#endif
