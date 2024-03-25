#pragma once
/**\class Hyd_Coupling_Management
	\author Daniel Bachmann et al.
	\author produced by the Institute of Hydraulic Engineering (IWW), RWTH Aachen University
	\version 0.0.1                                                              
	\date 2009 
*/
#ifndef HYD_COUPLING_MANAGEMENT_H
#define HYD_COUPLING_MANAGEMENT_H

//hyd-class
#include"Hyd_Coupling_RV2CO.h"
#include "Hyd_Coupling_RV2FP.h"
#include "Hyd_Coupling_RV2FP_Merged.h"
#include "Hyd_Coupling_FP2CO.h"
#include "Hyd_Coupling_FP2CO_Merged.h"
#include "Hyd_Coupling_FP2FP.h"
#include "Hyd_Coupling_RV2RV.h"
#include "Hyd_Coupling_RV2RV_Diversion.h"
#include "Hyd_Coupling_RV2FP_Structure.h"
#include "Hyd_Coupling_RV2FP_Dikebreak.h"
#include "Hyd_Coupling_FP2CO_Dikebreak.h"

//system_sys_ classes
#include "_Sys_Common_System.h"
#include "Profiler.h"

class Hyd_Hydraulic_System;

///Managing-class for all possible coupling types between the models of the hydraulic system (floodplain (FP), river (RV), coast (CO)) \ingroup hyd
/**
This class manages the coupling between the models of the hydraulic system (Hyd_Hydraulic_System). It is one part of the hydraulic system.
The couplings representing the interaction between the models by a flow (discharge) from one model into another model. These flows are calculated at each
syncronisation time point.
Here the couplings are initialized, merged, and calculated at the syncronisation time point.
Possible couplings are:
- river to river (1d-1d; RV2RV)
- floodplain to floodplain (2d-2d; FP2FP)
- river to floodplain, direct or lateral via a overflow and a dikebreak (1d-2d; RV2FP)
- river to coast (1d-co; RV2CO)
- floodplain to coast (2d-co; FP2CO)

There are some special coupling types:
- river to river via a diversion channel (1d-diversion-1d)
- river to floodplain lateral by a given coupling structure (1d-2d-structure)

\see Hyd_Coupling_FP2FP, Hyd_Coupling_FP2CO, Hyd_Coupling_RV2RV, Hyd_Coupling_RV2RV_Diversion, Hyd_Coupling_RV2CO, Hyd_Coupling_RV2FP,
Hyd_Coupling_RV2FP_Structure, Hyd_Coupling_RV2FP_Dikebreak
*/
class Hyd_Coupling_Management : public _Sys_Common_System
{
public:
	///Default constructor
	Hyd_Coupling_Management(void);
	///Default destructor
	~Hyd_Coupling_Management(void);

	//members
	///Coupling of river- to coast-model
	Hyd_Coupling_RV2CO *coupling_rv2co;
	///Coupling of river- to floodplain-model
	Hyd_Coupling_RV2FP *coupling_rv2fp;
	///Coupling of river- to multiple floodplain-models (merged)
	Hyd_Coupling_RV2FP_Merged *coupling_merged_rv2fp;
	///Coupling of floodplain- to coast-model
	Hyd_Coupling_FP2CO *coupling_fp2co;
	///Coupling of coast-model to multiple floodplain-models (merged)
	Hyd_Coupling_FP2CO_Merged *coupling_merged_fp2co;
	///Coupling of floodplain- to floodplain-model
	Hyd_Coupling_FP2FP *coupling_fp2fp;
	///Coupling of river- to river-model
	Hyd_Coupling_RV2RV *coupling_rv2rv;
	///Coupling of river- to river model via a diversion
	Hyd_Coupling_RV2RV_Diversion *coupling_1d_diversion;
	///Coupling of river- to floodplain model via a hydraulic structure
	Hyd_Coupling_RV2FP_Structure *coupling_rv2fp_structure;


	///Coupling of river- to floodplain model via a dikebreak
	Hyd_Coupling_RV2FP_Dikebreak *coupling_rv2fp_dikebreak;
	///Dikebreak coupling(s), which are automatically set via the fpl-system
	Hyd_Coupling_RV2FP_Dikebreak *coupling_rv2fp_dikebreak_fpl;

	///Coupling of river- to floodplain model via a dikebreak
	Hyd_Coupling_FP2CO_Dikebreak *coupling_fp2co_dikebreak;
	///Dikebreak coupling(s), which are automatically set via the fpl-system
	Hyd_Coupling_FP2CO_Dikebreak *coupling_fp2co_dikebreak_fpl;
 


	//methods
	///Output the members
	void output_members(void);
	///Output the setted members
	void output_setted_members(QSqlDatabase *ptr_database);
	///Output final result members
	void output_final_results(void);
	///Output final result members to database
	void output_final_results(QSqlDatabase *ptr_database, const int hyd_bound_sz, const string break_sc);
	///Output final results of the coupling points concerning the maximum waterlevel for the coupling to the fpl-sections
	void output_final_results_max_waterlevel(QSqlDatabase *ptr_database, const int hyd_bound_sz, const string break_sc);


	///Add number of river to coast coupling
	void add_rv2co(const int number);
	///Get the number of river to coast coupling
	int get_rv2co(void);

	///Add number of floodplain to coast coupling
	void add_fp2co(const int number);
	///Get the number of floodplain to coast coupling
	int get_fp2co(void);

	///Add number of floodplain to floodplain coupling
	void add_fp2fp(const int number);
	///Get the number of floodplain to floodplain coupling
	int get_fp2fp(void);

	///Add number of river to floodplain coupling
	void add_rv2fp(const int number);
	///Get the number of river to floodplain coupling
	int get_rv2fp(void);

	///Add number of river to river coupling
	void add_rv2rv(const int number);
	///Get the number of river to river coupling
	int get_rv2rv(void);

	///Set the number of dikebreak coupling, which are automatically set via the fpl-system; they are also allocated here (river model to floodplain model)
	void set_number_rv2fp_dikebreak_fpl_alloc(const int number);
	///Get the number of dikebreak coupling, which are automatically set via the fpl-system (river model to floodplain model)
	int get_number_rv2fp_dikebreak_fpl(void);
	///Set the breach start-parameters for the allocated coupling classes (river model to floodplain model)
	void set_start_rv2fp_dikebreak_fpl(const int index, const _hyd_breach_start_parameters params, const bool left_flag);
	///Initialise the dikebreak coupling, which are automatically set via the fpl-system (scenario based risk approach) (river model to floodplain model)
	void init_rv2fp_dikebreak_fpl(const int index, const int number_involved_profiles, _Hyd_River_Profile **involved_profiles, const int section_id_fpl);
	///Initialise the dikebreak coupling, which are automatically set via the fpl-system; the coupling point index, where the break should begin is directly given (catchment area risk approach) (river model to floodplain model)
	void init_rv2fp_dikebreak_fpl(const int index, const int section_id_fpl, const int point_id);
	///Clear the dikebreak coupling, which are automatically set via the fpl-system (river model to floodplain model)
	void clear_rv2fp_dikebreak_fpl(void);

	///Set the number of dikebreak coupling, which are automatically set via the fpl-system; they are also allocated here (coast model to floodplain model)
	void set_number_fp2co_dikebreak_fpl_alloc(const int number);
	///Get the number of dikebreak coupling, which are automatically set via the fpl-system (coast model to floodplain model)
	int get_number_fp2co_dikebreak_fpl(void); 
	///Set the breach start-parameters for the allocated coupling classes (coast model to floodplain model)
	void set_start_fp2co_dikebreak_fpl(const int index, const _hyd_breach_start_parameters params);
	///Initialise the dikebreak coupling, which are automatically set via the fpl-system (scenario based risk approach) (coast model to floodplain model)
	void init_fp2co_dikebreak_fpl(const int index, const int number_involved_points, Hyd_Floodplain_Dikeline_Point **involved_points, const int section_id_fpl);
	///Initialise the dikebreak coupling, which are automatically set via the fpl-system; the coupling point index, where the break should begin is directly given (catchment area risk approach) (coast model to floodplain model)
	void init_fp2co_dikebreak_fpl(const int index, const int section_id_fpl, const int point_id);
	///Clear the dikebreak coupling, which are automatically set via the fpl-system (coast model to floodplain model)
	void clear_fp2co_dikebreak_fpl(void);


	///Get if the breach for a coupling of a coast model/river model to a floodplain model has started for a specific section id of the fpl-system
	bool get_fpl_breach_has_started(const int sec_id);
	///Get the observed maximum waterlevel for a breach development
	double get_max_observed_waterlevel_breach(const int sec_id);



	///Set number of river to river coupling via diversion; here they are also allocated
	void set_rv2rv_diversion(const int number);
	///Input the parameters for a river coupling via a diversion channel per file
	void read_rv2rv_diversion_per_file(const string file);
	///Input the parameters for all river coupling via a diversion channel from a selection of a database table
	void read_rv2rv_diversion_per_database(const QSqlTableModel *results, const _sys_system_id id);
	///Transfer the parameters for a river coupling via a diversion channel to a database
	void transfer_rv2rv_diversion2database(QSqlDatabase *ptr_database);
	///Convert the indices of the coupled models into pointer for all river coupling via a diversion channel
	void set_pointer_rv2rv_diversion(Hyd_Model_River *river_models, const int number_rv_models);
	///Get the number of river to river coupling via diversion
	int get_rv2rv_diversion(void);

	///Set number of river to floodplain coupling via a hydraulic structure; here they are also allocated
	void set_rv2fp_structure(const int number);
	///Input the parameters for a river to floodplain coupling via a hydraulic structure per file
	void read_rv2fp_structure_per_file(const string file);
	///Input the parameters for a river to floodplain coupling via a hydraulic structure from a selection of a database table
	void read_rv2fp_structure_per_database(const QSqlQueryModel *results, const _sys_system_id id);
	///Transfer the parameters for a river to floodplain coupling via a hydraulic structure to a database
	void transfer_rv2fp_structure2database(QSqlDatabase *ptr_database);
	///Get the number of river to floodplain coupling via a hydraulic structure
	int get_rv2fp_structure(void);

	///Set number of river to floodplain coupling via a dikebreak; here they are also allocated
	void set_rv2fp_dikebreak(const int number);
	///Input the parameters for a river to floodplain coupling via a dikebreak per file
	void read_rv2fp_dikebreak_per_file(const string file);
	///Input the parameters for a river to floodplain coupling via a dikebreak from a selection of a database table
	void read_rv2fp_dikebreak_per_database(const QSqlQueryModel *results, const _sys_system_id id);
	///Transfer the parameters for a river to floodplain coupling via a dikebreak to a database
	void transfer_rv2fp_dikebreak2database(QSqlDatabase *ptr_database);
	///Get the number of river to floodplain coupling via a dikebreak
	int get_rv2fp_dikebreak(void);


	///Set number of coast to floodplain coupling via a dikebreak; here they are also allocated
	void set_fp2co_dikebreak(const int number);
	///Input the parameters for a coast to floodplain coupling via a dikebreak per file
	void read_fp2co_dikebreak_per_file(const string file);
	///Input the parameters for a coast to floodplain coupling via a dikebreak from a selection of a database table
	void read_fp2co_dikebreak_per_database(const QSqlQueryModel *results, const _sys_system_id id);
	///Transfer the parameters for a coast to floodplain coupling via a dikebreak to a database
	void transfer_fp2co_dikebreak2database(QSqlDatabase *ptr_database);
	///Get the number of coast to floodplain coupling via a dikebreak
	int get_fp2co_dikebreak(void);


	///Allocate the coupling classes for river to coast coupling
	void allocate_coupling_class_rv2co(void);
	///Allocate the coupling classes for river to floodplain coupling
	void allocate_coupling_class_rv2fp(void);
	///Allocate the coupling classes for floodplain to coast coupling
	void allocate_coupling_class_fp2co(void);
	///Allocate the coupling classes for floodplain to floodplain coupling
	void allocate_coupling_class_fp2fp(void);
	///Allocate the coupling classes for river to river coupling
	void allocate_coupling_class_rv2rv(void);

	///Initialize the couplings
	void init_couplings(void);

	///Synchronise the couplings 
	void synchronise_couplings(const double timepoint, const double delta_t, const bool time_check, const int internal_counter, Profiler* profiler);

	///Output the couplings (e.g. dikebreak development) per calculation step
	void output_coupling_calculation_steps2file(const double timepoint);

	///Reset the coupling for a new hydraulic simulation without deleting the objects
	void reset_couplings(void);

	///Total reset of the couplings
	void total_reset(void);

	///Clone the couplings
	void clone_couplings(Hyd_Coupling_Management *coupling, Hyd_Hydraulic_System *system);

	///Initialize output files, e.g. dike break
	void init_output_files(void);

	///Set the pointer to the output flags
	void set_ptr2outputflags(_hyd_output_flags *flags);


private:
	
	//members
	///Total number of couplings
	int total_number;
	///Number of river to coast couplings
	int number_rv2co;
	///Number of floodplain to floodplain couplings
	int number_fp2fp;
	///Number of floodplain to river couplings
	int number_rv2fp;
	///Number of merged floodplain to river couplings
	int number_merged_rv2fp;
	///Number of floodplain to coast couplings
	int number_fp2co;
	///Number of merged floodplain to coast couplings
	int number_merged_fp2co;
	///Number of river to river couplings
	int number_rv2rv;
	///Number of river- to river model couplings via a diversion
	int number_rv2rv_diversion;
	///Number of river- to floodplain model couplings via a hydraulic structure
	int number_rv2fp_structure;
	///Number of river- to floodplain model couplings via a dikebreak
	int number_rv2fp_dikebreak;
	///Number of coast- to floodplain model couplings via a dikebreak
	int number_fp2co_dikebreak;
	///Number of river- to floodplain model direct coupling (outflow)
	int number_rv2fp_direct_out;
	///Number of river- to floodplain model direct coupling (inflow)
	int number_rv2fp_direct_in;
	///Number of dikebreak coupling floodplain to river model, which are automatically set via the fpl-system
	int number_rv2fp_dikebreak_fpl;
	///Number of dikebreak coupling floodplain to coast model, which are automatically set via the fpl-system
	int number_fp2co_dikebreak_fpl;

	///Total number of couplings
	int total_number_coupling;

	///The time point before
	double old_time_point;
	///Delta time
	double delta_t;

	///Pointer to the output settings
	_hyd_output_flags *ptr_output_flags;



	//methods
	///Insert special points to the merged RV2FP point list (e.g. river junctions, diversion channels)
	void insert_river_junctions2RV2FPlist(void);
	///Insert special points to the merged RV2FP point list (e.g. dikebreak)
	void insert_breaks2RV2FPlist(void);
	///Insert additional coupling points for a finer coupling discretisation to the merged RV2FP point list
	void insert_add_coupling_point2RV2FPlist(void);
	//Insert special points to the merged FP2CO point list (e.g. river junctions)
	void insert_river_junctions2FP2COlist(void);
	///Insert special points to the merged FP2CO point list (e.g. dikebreak)
	void insert_breaks2FP2COlist(void);
	///Insert additional coupling points for a finer coupling discretisation to the merged FP2CO point list
	void insert_add_coupling_point2FP2COlist(void);

	///Check if there there are RV2FP point in the lists, which are coupled beyond river; there a coupling is not possible; do it after the merged lists are completly set
	void check_RV2FPpoints_beyond_rivers(void);

	///Reset coupling discharges between the model (use it before syncronisation)
	void reset_coupling_discharges(void);

	///Delete the coupling classes for river to coast coupling
	void delete_coupling_class_rv2co(void);
	///Delete the coupling classes for river to floodplain coupling
	void delete_coupling_class_rv2fp(void);
	///Delete the coupling classes for floodplain to coast coupling
	void delete_coupling_class_fp2co(void);
	///Delete the coupling classes for floodplain to floodplain coupling
	void delete_coupling_class_fp2fp(void);
	///Delete the coupling classes for river to river coupling
	void delete_coupling_class_rv2rv(void);

	///Init the coupling classes for river to multiple floodplains (merged)
	void init_coupling_class_rv2fp_merged(void);
	///Init the coupling classes for multiple floodplains to one coast model (merged)
	void init_coupling_class_fp2co_merged(void);

	///Allocate the coupling classes for river to multiple floodplains (merged)
	void allocate_coupling_class_rv2fp_merged(void);
	///Allocate the coupling classes for multiple floodplains to one coast model (merged)
	void allocate_coupling_class_fp2co_merged(void);
	///Delete the coupling classes for river to multiple floodplains (merged)
	void delete_coupling_class_rv2fp_merged(void);
	///Delete the coupling classes for multiple floodplains to one coast model (merged)
	void delete_coupling_class_fp2co_merged(void);

	///Allocate the coupling classes for river to river coupling via diversion
	void allocate_coupling_class_rv2rv_diversion(void);
	///Delete the coupling classes for river to river coupling via diversion
	void delete_coupling_class_rv2rv_diversion(void);

	///Allocate the coupling classes for river to floodplain coupling via a hydraulic structure
	void allocate_coupling_class_rv2fp_structure(void);
	///Delete the coupling classes for river to floodplain coupling via a hydraulic structure
	void delete_coupling_class_rv2fp_structure(void);

	///Allocate the coupling classes for river to floodplain coupling via a dikebreak
	void allocate_coupling_class_rv2fp_dikebreak(void);
	///Delete the coupling classes for river to floodplain coupling via a dikebreak
	void delete_coupling_class_rv2fp_dikebreak(void);

	///Allocate the coupling classes for river to floodplain coupling via a dikebreak auotmatically set by the fpl-system
	void allocate_coupling_class_rv2fp_dikebreak_fpl(void);
	///Delete the coupling classes for river to floodplain coupling via a dikebreak auotmatically set by the fpl-system
	void delete_coupling_class_rv2fp_dikebreak_fpl(void);

	///Allocate the coupling classes for coast to floodplain coupling via a dikebreak
	void allocate_coupling_class_fp2co_dikebreak(void);
	///Delete the coupling classes for coast to floodplain coupling via a dikebreak
	void delete_coupling_class_fp2co_dikebreak(void);

	///Allocate the coupling classes for coast to floodplain coupling via a dikebreak auotmatically set by the fpl-system
	void allocate_coupling_class_fp2co_dikebreak_fpl(void);
	///Delete the coupling classes for coast to floodplain coupling via a dikebreak auotmatically set by the fpl-system
	void delete_coupling_class_fp2co_dikebreak_fpl(void);


	///Output the coupling statistics
	void output_coupling_statistic(void);

	///Calculated total number of couplings
	void calculate_total_number_coupling(void);

	///Generate a breach-index for the break-coupling between river - and floodplain-models
	int generate_breachindex_rv2fp_break_coupling(void);
	///Generate a breach-index for the break-coupling between coast - and floodplain-models
	int generate_breachindex_fp2co_break_coupling(void);


	///Set error(s)
	Error set_error(const int err_type);


};
#endif
