#pragma once
/**\class Hyd_Coupling_FP2CO_Dikebreak
	\author Daniel Bachmann et al.
	\author produced by the Institute of Hydraulic Engineering (IWW), RWTH Aachen University
	\version 0.0.1                                                              
	\date 2011 
*/
#ifndef HYD_COUPLING_FP2CO_DIKEBREAK_H
#define HYD_COUPLING_FP2CO_DIKEBREAK_H


//parser class
#include "_Hyd_Coupling_Dikebreak.h"
#include "Hyd_Parse_Coupling_FP2CO_Dikebreak.h"
#include "Hyd_Coupling_FP2CO.h"
#include "Hyd_Coupling_Point_FP2CO.h"





///Coupling-class for a coupling between a coast-model and a floodplain-model via a hydraulic dikebreak/wallbreak  \ingroup hyd
/**
This class represents a special type of coupling between a coast- and a floodplain-model. The breach development due to a
dikebreak/wallbreak is calculated here.
This class uses the merged list for the standard overflow
coupling between a coast-model and a floodplain-model (Hyd_Coupling_Point_FP2CO), where the overflow height is reduced to the
base-points of the coast line.


\see Hyd_Coupling_Point_FP2CO
*/

class Hyd_Coupling_FP2CO_Dikebreak : public _Hyd_Coupling_Dikebreak
{
public:
	///Default constructor
	Hyd_Coupling_FP2CO_Dikebreak(void);
	///Default destructor
	~Hyd_Coupling_FP2CO_Dikebreak(void);


	//method
	///Input of the dike-/wallbreak coupling parameters per file via a parser (Hyd_Parse_Coupling_Dikebreak)
	void input_parameter_per_file(const int index, const string file);
	///Input of the dike-/wallbreak coupling parameters from a database selection
	void input_parameter_per_database(const QSqlQueryModel *results, const int index, const bool output_flag);
	///Set the start parameters of a break via a given data structure
	void input_parameter(const int index_breach, const _hyd_fp2co_breach_start_parameters params);

	///Transfer the dikebreak/wallbreak data of a file into a database table
	void transfer_break_data2database_table(QSqlDatabase *ptr_database);


	///Set an additional coupling point, when the break is user-defined
	void set_additional_coupling_point(Hyd_Coupling_FP2CO_Merged *couplings);

	///The couplings are initialized with the already performed FP2CO couplings
	void init_coupling(Hyd_Coupling_FP2CO_Merged *couplings);
	///The couplings are initialized with the already performed FP2CO couplings; here a list of coast line points is also given for the creation of a list of possible starting breach points (scenario based risk approach)
	void init_coupling(Hyd_Coupling_FP2CO_Merged *couplings, Hyd_Floodplain_Dikeline_Point **involved_points, const int number_involved_points, const int section_id_fpl, _hyd_output_flags *output_flags);
	///The couplings are initialized with the already performed FP2CO couplings; the coupling point index, where the break should begin is directly given (catchment area risk approach)
	void init_coupling(Hyd_Coupling_FP2CO_Merged *couplings, const int section_id_fpl, const int point_id, _hyd_output_flags *output_flags);
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
	void clone_couplings(Hyd_Coupling_FP2CO_Dikebreak *coupling, Hyd_Coupling_FP2CO_Merged *merged_couplings);

	///Initiate the output to a tecplot file
	void init_output2file_tecplot(void);

	///Initiate the output to a csv file
	void init_output2file_csv(void);

private:

	//member
	///Pointer to the coupling point list
	Hyd_Coupling_Point_FP2CO_List *point_list;

	///Pointer to the coupling point (Hyd_Coupling_Point_FP2CO), where the dikebreak starts
	Hyd_Coupling_Point_FP2CO *start_coupling_point;
	///Pointer to the coupling point (Hyd_Coupling_Point_FP2CO), for the upstream breach developement
	Hyd_Coupling_Point_FP2CO *upstream_coupling_point;
	///Pointer to the coupling point (Hyd_Coupling_Point_FP2CO), for the downstream breach developement
	Hyd_Coupling_Point_FP2CO *downstream_coupling_point;

	///Pointer to the coast model
	Hyd_Coast_Model *coast;

	///Distance to the related dike line point
	double distance_related_point;
	///Index of the related dike line point
	int id_related_point;

	///List of other possible starting coupling points for the breach start
	Hyd_Coupling_Point_FP2CO **list_possible_start_points;
	///Number of possible starting coupling points
	int number_possible_start_points;

	//method
	///Allocate the list of possible starting coupling points
	void allocate_list_possible_start_points(void);
	///Delete the list of possible starting coupling points
	void delete_list_possible_start_points(void);

	///Create a list of possible starting coupling points
	void create_starting_coupling_point_list(Hyd_Floodplain_Dikeline_Point **involved_points, const int number_involved_points);
	///Observe and switch, if required, the possible starting coupling points
	void observe_starting_coupling_points(void);
	///Sort the list of the possible starting points after the waterlevel
	void sort_list_possible_start_points(void);


	///Calculate the downstream breach opening
	void calculate_downstream_opening(void);
	///Calculate the upstream breach opening
	void calculate_upstream_opening(void);

	///Set a new pointer to an upstream coupling point 
	void set_new_upstream_coupling_point(Hyd_Coupling_Point_FP2CO *new_coupling_point);
	///Set a new pointer to a downstream coupling point 
	void set_new_downstream_coupling_point(Hyd_Coupling_Point_FP2CO *new_coupling_point);

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