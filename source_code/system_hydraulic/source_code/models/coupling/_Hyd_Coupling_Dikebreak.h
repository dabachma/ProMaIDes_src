#pragma once
/**\class _Hyd_Coupling_Dikebreak
	\author Daniel Bachmann et al.
	\author produced by the Institute of Hydraulic Engineering (IWW), RWTH Aachen University
	\version 0.0.1                                                              
	\date 2009 
*/
#ifndef _HYD_COUPLING_DIKEBREAK_H
#define _HYD_COUPLING_DIKEBREAK_H

//system_sys_ classes
#include "_Sys_Common_System.h"

//struct
///Structure for gathering the parameters for a breach start for a coupling of a river to a floodplain model \ingroup hyd
struct _hyd_breach_start_parameters{
	///Start height [m]
	double start_height;
	///Maximal breach width
	double max_breach_width;
	///Index of the related station, like river profile or coat dikelime point
	int id_related_station;
	///Index of the river model
	int id_rv_model;
	///Distance to the related river profile/ coast dikeline point [m]
	double distance2related;

};

///Structure to gather information of the break in list for an file output
struct _hyd_break_list{
	///Time point [s]
	QList<double> time;
	///Counter breach [-]
	QList<int> counter_breach;
	///Time of breach [s]
	QList<double> time_breach;
	///Total breach width [m]
	QList<double> total_breach;
	///Upstream breach width [m]
	QList<double> upstream_breach;
	///Downstream breach width [m]
	QList<double> downstream_breach;
	///Upstream delta waterlevel [m]
	QList<double> upstream_delta_h;
	///Downstream delta waterlevel [m]
	QList<double> downstream_delta_h;
	///Mean discharge [m^3/s]
	QList<double> mean_q;
	///Mean velocity [m/s]
	QList<double> mean_v;
	///Delta waterlevel to start height [m]
	QList<double> delta_h2start;
	///Stress to the upstream wall [m]
	QList<double> upstream_wall_stress;
	///Stress to the downstream wall [m]
	QList<double> downstream_wall_stress;


};


///Coupling-class for a coupling between via a hydraulic dikebreak or wallbreak  \ingroup hyd
/**
This class represents the coupling between two models of the hydraulic system. The breach development due to a
dikebreak/wallbreak is calculated here. Thus, the coupling is instationary, it starts punctuell at a starting point in the flood protection line. 
The breach development over the time depends on the type and the material parameters of the flood protection line.
The starting point is set by the user or automatically by the programm, 
where different break scenarios are set.

There are two possible types of breaching:
 - continuous breach developement as a dikebreak; the material parameter is the critical flow veloctiy representing the erosive resistance 
 - discontinuous breach development as a wallbreak


\see Hyd_Coupling_FP2CO_Dikebreak, Hyd_Coupling_RV2FP_Dikebreak
*/
class _Hyd_Coupling_Dikebreak : public _Hyd_Coupling_Point, public _Sys_Common_System
{
public:
	///Default constructor
	_Hyd_Coupling_Dikebreak(void);
	///Default destructor
	virtual ~_Hyd_Coupling_Dikebreak(void);

     //member
    ///Pointer to the table for the the dikebreak/wallbreak data in a database
    static Tables *break_data_table;
    ///Pointer to the table for the the dikebreak/wallbreak results in a database
    static Tables *break_result_table;

	//method
	///Create the database table for the dikebreak/wallbreak data
	static void create_table(QSqlDatabase *ptr_database);
	///Set the database table for the dikebreak/wallbreak data: it sets the table name and the name of the columns and allocate them
	static void set_table(QSqlDatabase *ptr_database);
	///Delete all data in the database table for the dikebreak/wallbreak data
	static void delete_data_in_table(QSqlDatabase *ptr_database);
	///Close and delete the database table for the dikebreak/wallbreak data
	static void close_table(void);
	///Select and count the number of dike-/wallbreak couplings in a database table
	static int select_relevant_break_data_database(QSqlDatabase *ptr_database, QSqlQueryModel *results, const _sys_system_id id, const bool with_output = true);
	///Select and count the number of dike-/wallbreak couplings in a database table for coast to floodplain models
	static int select_relevant_break_data_coast_database(QSqlDatabase *ptr_database, QSqlQueryModel *results, const _sys_system_id id, const bool with_output = true);

	///Create the database table for the dikebreak/wallbreak result data
	static void create_result_table(QSqlDatabase *ptr_database);
	///Set the database table for the dikebreak/wallbreak result data: it sets the table name and the name of the columns and allocate them
	static void set_result_table(QSqlDatabase *ptr_database);
	///Delete all data in the database table for the dikebreak/wallbreak result data
	static void delete_data_in_result_table(QSqlDatabase *ptr_database);
	///Delete all data in the database table for the dikebreak/wallbreak result data
	static void delete_data_in_result_table(QSqlDatabase *ptr_database,  const _sys_system_id id);

	///Delete all data in the database table for the dikebreak/wallbreak result data
	static void delete_data_in_result_table(QSqlDatabase *ptr_database,  const _sys_system_id id, const string id_break, const int hyd_bound_id);
	///Delete all data in the database table for the dikebreak/wallbreak result data 
	static void delete_data_in_result_table(QSqlDatabase *ptr_database,  const _sys_system_id id, const string id_break, const bool like_flag);
	///Close and delete the database table for the dikebreak/wallbreak result data
	static void close_result_table(void);

	///Switch the applied-flag in the database table for the dikebreak/wallbreak result data
	static void switch_applied_flag_erg_table(QSqlDatabase *ptr_database, const _sys_system_id id, const bool flag);
	///Switch the applied-flag in the database table for the dikebreak/wallbreak result data for a given boundary scenario
	static void switch_applied_flag_erg_table(QSqlDatabase *ptr_database, const _sys_system_id id, const int hyd_sc, const bool flag);

	///Copy the results of a given system id to another one in database table 
	static void copy_results(QSqlDatabase *ptr_database, const _sys_system_id src, const _sys_system_id dest);



	///Synchronise the models
	virtual void synchronise_models(const double timepoint, const double delta_t, const bool prediction)=0;

	///Reset the hydrological balance values and the maximum calculated values
	virtual void reset_hydro_balance_max_values(void);

	///Set the flag if the breach development is user defined (true) or dynamically set (false)
	void set_flag_user_defined(const bool flag);


	///Get the breach index
	int get_breach_index(void);
	///Get the index of the fpl-section
	int get_index_fpl_section(void);

	///Get if the breach has started
	bool get_breach_has_started(void);
	///Get maximal obeserved waterlevel
	double get_max_observed_waterlevel(void);

	///Ouput the point to database as Geo_point data
	void output_point2database(QSqlDatabase *ptr_database);

	///Clear break list
	void clear_break_list(void);


protected:


	//members




	///Lists where information about the break parameters are stored
	_hyd_break_list break_info_list;



	///Index of the breach-coupling
	int index;
	///Index of the fpl-section; it is just required in case of a dynamical breach setting
	int index_section_fpl;
	///Index of the floodplain model
	int index_fp_model_start;
	///Flag if it is a breaching at wall (true) or a dike (false)
	bool wall_breach_flag;

	///Local starting waterlevel of the breach
	double start_waterlevel_local;
	///Global starting waterlevel of the breach
	double start_waterlevel_global;
	///The maximum possible breach width
	double max_breach_width;

	///Maximal observed waterlevel
	double max_observed_waterlevel;

	///Default value for the maximum breach width
	const double default_max_breach_width;

	///Flag if a dikebreak coupling is applied
	bool coupling_flag;

	///Flag for an opening in downstream direction
	bool downstream_open_flag;
	///Flag for an opening in upstream direction
	bool upstream_open_flag;

	///Flag if the break have started
	bool started_flag;
	///Flag if the initial-phase has finished
	bool init_phase_finished;
	///Counter of the syncronization-calls since starting the breach
	int breach_counter;
	///The starting time point of the breach
	double starting_time_point;
	///The time of breaching
	double breach_time;

	///Total breach width
	double total_breach_width;
	///Breach width upstream of the starting point
	double breach_width_upstream;
	///Delta of the breach with for this timestep in upstream direction
	double delta_width_upstream;
	///Breach width downstream of the starting point
	double breach_width_downstream;
	///Delta of the breach with for this timestep in downstream direction
	double delta_width_downstream;

	///Counter for the applied stress in case of sudden breaks upstream
	double counter_stress_upstream;
	///Counter for the applied stress in case of sudden breaks downstream
	double counter_stress_downstream;

	///Parameters for the upstream part of the breach
	_hyd_break_parameters *break_params_upstream;
	///Delta h for the breach relevant for the upstream part
	double *deltah_upstream;
	///Distance to the next upstream coupling point
	double distance_next_point_upstream;
	///Parameters for the downstream part of the breach
	_hyd_break_parameters *break_params_downstream;
	///Delta h for the breach relevant for the downstream part
	double *deltah_downstream;
	///Distance to the next downstream coupling point
	double distance_next_point_downstream;

	///Constant for the intial breach width
	const double init_breach_width;
	///Constant for the numerical breach opening velocity
	const double numerical_breach_v;

	///Constant for the continuous breach opening f1 (see Verheij; "Aanpassen van het bresgroeimodel binnen HIS-OM"; 2003)
	const double f1;
	///Constant for the continuous breach opening f2 (see Verheij; "Aanpassen van het bresgroeimodel binnen HIS-OM"; 2003)
	const double f2;

	///Flag if the abrupt breaking in upstream direction is in the transition phase=> breach opening is not totaly abrupt (numerical reason)
	bool transition_upstream_flag;
	///Flag if the abrupt breaking in downstream direction is in the transition phase=> breach opening is not totaly abrupt (numerical reason)
	bool transition_downstream_flag;
	///Width in upstream direction which is still needed to translate to the coupling point in the transion pahse
	double transition_width_upstream;
	///Width in downstream direction which is still needed to translate to the coupling point in the transion pahse
	double transition_width_downstream;

	///The mean velocity over the breach per calculation step
	double mean_v;
	///The mean discharge over the breach per calculation step
	double mean_q;
	///The mean delta h over the breach over the breach time
	double total_mean_dh;

	///File stream for output the breach developement in tecplot
	ofstream output_file;
	///Name of the file for breach developement in tecplot
	string file_name;

	///File stream for output the breach developement in csv
	ofstream output_file_csv;
	///Name of the file for breach developement in csv
	string file_name_csv;

	///Flag if the breach-development is user-defined
	bool user_defined;


	//method

	///Calculate the downstream breach opening
	virtual void calculate_downstream_opening(void)=0;
	///Calculate the upstream breach opening
	virtual void calculate_upstream_opening(void)=0;

	///Calculate the mean discharge/mean velocity over the breach per calculation step
	virtual void calculate_mean_q_v(const double timepoint)=0;

	///Check the dikebreak parameters
	virtual void check_dikebreak_parameter(void);

	///Calculate the breach width with an abrupt opening for walls
	double calculate_abrupt_breach(_hyd_break_parameters *break_params, const double d_h, double *counter_stress);
	///Calculate the breach width with continuous opening for dikes
	double calculate_continuous_breach(_hyd_break_parameters *break_params, const double d_h);

	///Calculate the ideal time which would be needed by the given parameters for the breach width
	double calculate_ideal_breach_time(const double faktor1, const double faktor2, const double d_h);

	///Calculate the total mean delta h at the breach heads 
	void calculate_total_mean_delta_h(void);

	///Close the output file for tecplot
	void close_output_file(void);
	///Open the outpufile for tecplot; if it is open it will be closed
	void open_output_file(void);

	///Output the header to the result file for tecplot
	virtual void output_header_result2file_tecplot(void)=0;


	///Close the output file for csv
	void close_output_file_csv(void);
	///Open the outpufile for csv; if it is open it will be closed
	void open_output_file_csv(void);

	///Output the header to the result file for csv
	virtual void output_header_result2file_csv(void) = 0;

private:

	//method
	///Set error(s)
	Error set_error(const int err_type);
	///Set warning(s)
	Warning set_warning(const int warn_type);



};
#endif
