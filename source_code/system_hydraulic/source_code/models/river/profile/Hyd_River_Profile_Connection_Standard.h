#pragma once
/**\class Hyd_River_Profile_Connection_Standard
	\author Daniel Bachmann et al.
	\author produced by the Institute of Hydraulic Engineering (IWW), RWTH Aachen University
	\version 0.0.1                                                              
	\date 2009 
*/
#ifndef _HYD_RIVER_PROFILE_CONNECTION_STANDARD_H
#define _HYD_RIVER_PROFILE_CONNECTION_STANDARD_H
//libs hyd_system
#include "_Hyd_River_Profile.h"


///Class of a river profile representing a standard profile (inbetween profile) of a river model \ingroup hyd
/**

\see Hyd_Model_River
*/
class Hyd_River_Profile_Connection_Standard : public _Hyd_River_Profile
{
//class for displaying the data
friend class HydGui_Rv_Profile_General_Wid;

public:

	///Default constructor
	Hyd_River_Profile_Connection_Standard(void);
	///Default destructor
	virtual ~Hyd_River_Profile_Connection_Standard(void);

	//methods
	///Transfer the river profile data to a database
	void transfer_profile_members2database(const int glob_prof_id, QSqlQuery *model, QSqlDatabase *ptr_database, const int rv_number);

	///Input the river profile data with the index from database selection
	void input_members_per_database(const int index, const QSqlQueryModel *query_result, QSqlDatabase *ptr_database);
	///Input the river profile boundary data from a database
	void input_boundary_data_per_database(QSqlDatabase *ptr_database, const int glob_prof_id);

	///Get if an overflow over the left river bank into the floodplain is given
	bool get_overflow_flag_left(void);
	///Get the poleni-factor for an overflow over the left river bank into the floodplain 
	double get_overflow_poleni_left(void);
	///Get if an overflow over the right river bank into the floodplain is given
	bool get_overflow_flag_right(void);
	///Get the poleni-factor for an overflow over the right river bank into the floodplain 
	double get_overflow_poleni_right(void);

	///Set the profile to no overflow (left and right riverbank)
	void set_left_right_bank2no_overflow(void);

	///Check the standard profiles
	void check_profiles(_Hyd_River_Profile *upstream_profile, _Hyd_River_Profile *downstream_profile);

	///Output the members to display/console
	void output_members(void);
	///Output the initialized and calculated members to display/console
	void output_setted_members(void);

	///Clone river profile
	virtual void clone_profile(Hyd_River_Profile_Connection_Standard *profile);

	///Get the flag if a lateral boundary condition is set
	bool get_boundary_lateral_flag(void);
	///Connect instationary boundary curve to the profiles (lateral inflow)
	void connect_instat_lateral_boundarycurve(Hyd_Instationary_Boundary *bound_curve);
	///Get the instationary found flag: the curve is found for lateral inflow
	bool get_instat_lateral_boundarycurve_found(void);

	///Connect instationary boundary curve to the profiles (pointwise inflow [m³/s])
	virtual void connect_instat_point_boundarycurve(Hyd_Instationary_Boundary *bound_curve);
	///Get the flag if a point boundary condition is set
	bool get_boundary_point_flag(void);
	///Get the instationary found flag: the curve is found for pointwise inflow [m³/s]
	bool get_instat_point_boundarycurve_found(void);

	///Set the actual river discharge through this profile (q_river) by the given upstream and downstream profiles
	void set_actual_river_discharge(_Hyd_River_Profile *upstream_profile, _Hyd_River_Profile *downstream_profile);

	///Get the actual discharge through this profile
	virtual double get_actual_river_discharge(void);

	///Reset all coupling discharges; use it before syncronization
	void reset_coupling_discharge(void);

	///Make the syncronistation of the elements to a given time point
	virtual void make_syncronisation(const double time_point, _Hyd_River_Profile *downstream);
	///Get the maximum change in a river section
	virtual void get_max_changes_section(double *max_change_h, double *max_change_v, const bool timecheck);


	///Calculate the values for a hydrological balance of the corresponding river segment (upstream) for each syncronisation step
	virtual void calculate_hydrological_balance(const double time_point);

	///Get the total boundary and coupling discharge to this profile
	double get_total_boundary_coupling_discharge(const double time_point, const double q_river_in_out);

	///Get the actual lateral discharge (q_lateral)
	double get_actual_lateral_discharge(void);
	///Add a discharge value to the current lateral discharge
	void add_lateral_discharge(const double value);
	///Get the actual point discharge (q_point)
	double get_actual_point_discharge(void);
	///Add a discharge value to the current point discharge
	virtual void add_point_discharge(const double value);

	///Add the hydrological balance value of the point boundary condition discharges to the given pointer
	virtual void add_hydro_balance_point_boundary(_hyd_hydrological_balance *given);
	///Add the hydrological balance value of the lateral boundary condition discharges to the given pointer
	void add_hydro_balance_lateral_boundary(_hyd_hydrological_balance *given);
	
	///Add the total coupling/boundary discharges to the given downstream profile (needed e.g. for weir types)
	virtual void transfer_coupling_boundary2profile(Hyd_River_Profile_Connection_Standard *profile2transfer);

	///Get the error volume due to outflow by zero volume in the profile
	double get_outflow_zero_error_volume(void);


	///Output the result members for each riverprofiletype at every timestep
	virtual void output_result_members_per_timestep(void);

	///Output the result members for each riverprofiletype at every timestep to tecplot file
	virtual void output_result_members_per_timestep(ofstream *file);
	///Output the result members for each riverprofiletype at every timestep to csv file
	virtual void output_result_members_per_timestep2csv(ofstream *file);

	//Set new break-parameters of the left bank; the structure must be already allocated
	void set_break_parameter_left(_hyd_break_parameters* new_values);
	//Set new break-parameters of the right bank; the structure must be already allocated
	void set_break_parameter_right(_hyd_break_parameters* new_values);

	///Reset the hydrological balance value and the maximum values
	virtual void reset_hydrobalance_maxvalues(void);

	///Reset the boundary data of the river profile
	void reset_boundary_data(void);

	///Get boundary condition is applied
	bool boundary_is_applied(void);

	///Get boundary point condition is applied
	bool boundary_point_is_applied(void);
	///Get boundary point value
	double get_boundary_point_value(const double time);

	///Get q-value
	double get_Q(void);



protected:

	//member
	///Actual discharge through this profile for this timestep [qm³/s]
	double q_river;
	///Actual lateral discharge for the corresponding upstream river segment [m³/s]
	double q_lateral;
	///Actual point discharge for the corresponding upstream river segment [m³/s]
	double q_point;

	///Variation of the waterlevel over time for boundary and coupling
	double ds_dt_coup;
	///Variation of the waterlevel over time for the internal timestep before for boundary and coupling
	double ds_dt_coup_old;


	//members for a hydrological balance
	///The error in the hydrological balance due to a setted outflow but no watervolume is left in the profile
	double error_zero_outflow_volume;
	///Error discharge due to a setted outflow but no watervolume is left in the profile
	double q_zero_outflow_error;
	///The old timepoint (one solver step before) for the error volume calculation due to a setted outflow but no watervolume is left in the profile  
	double old_timepoint_error;
	///The delta time to the old time step (t-1) for the error volume calculation due to a setted outflow but no watervolume is left in the profile
	double old_delta_t_error;
	///The discharge of the old time step (t-1) for the error volume calculation due to a setted outflow but no watervolume is left in the profile
	double old_discharge_error;
	///The discharge of the old time step (t-2) for the error volume calculation due to a setted outflow but no watervolume is left in the profile
	double old_discharge_2_error;


	//members boundary condition lateral
	///Flag if a lateral boundary condition is set [m³/(s*m)] (true)
	bool boundary_cond_lateral_flag;
	///Flag if the lateral boundary condition [m³/(s*m)] is stationary (true)
	bool boundary_lateral_stat_flag;
	///Pointer for a stationary lateral boundary condition value [m³/(s*m)]; it is allocated if this option is chosen
	double *value_boundary_lateral_stat;
	///Pointer which specifies for a instationary lateral boundary condition [m³/(s*m)] the corresponding curve number; it is allocated if this option is chosen
	int *number_boundary_lateral_curve;
	///Pointer which specifies for a instationary lateral boundary condition [m³/(s*m)] the corresponding curve
	Hyd_Instationary_Boundary *ptr_boundarycurve_lateral;
	///Flag if the instationary lateral boundary condition [m³/(s*m)] curve is found
	bool instatbound_lateral_found_flag;
	///Flag if a point boundary condition is set [m³/s] (true)
	bool boundary_cond_point_flag;
	///Flag if the point boundary condition [m³/s] is stationary (true)
	bool boundary_point_stat_flag;
	///Pointer for a stationary point boundary condition [m³/s] value; it is allocated if this option is chosen
	double *value_boundary_point_stat;
	///Pointer which specifies for a instationary point boundary condition [m³/s] the corresponding curve number; it is allocated if this option is chosen
	int *number_boundary_point_curve;
	///Pointer which specifies for a instationary point boundary condition [m³/s] the corresponding curve
	Hyd_Instationary_Boundary *ptr_boundarycurve_point;
	///Flag if the instationary point boundary condition [m³/s] curve is found
	bool instatbound_point_found_flag;



	//methods
	///Decide which key value is read and give the read-in value to the corresponding member
	void decide_keyvalues_file(const string key, string buffer, int *found_counter);
	///Decide which boundary conditions (statinary/instationary) are choosen for pointwise inflow (discharge is devided by the distance between the profiles [m³/s])
	void decide_boundary_point_conditions(const double nr_value);
	///Decide which boundary conditions are choosen for lateral inflow (discharge is given per m river length [(m³/s)/m])
	void decide_boundary_lateral_conditions(const double nr_value);
	///Set the actual lateral discharge (q_lateral) correspondig to to the stationary/instationary flag and the point in time
	void set_actual_lateral_discharge(const double time_point);
	///Set the actual point discharge (q_point) correspondig to to the stationary/instationary flag and the point in time
	void set_actual_point_discharge(const double time_point);

	///Check the connection profile type
	void check_connection_type_profile(void);
	///Calculate the watervolume in the element
	virtual void calculate_watervolume_element(void);
	//Calculate the hydrological balance error, if the profile volume is zero, but there is still an outflow set
	void calculate_hydrolocigal_balance_error_zero_outflow(const double time_point);



private:
	//methods

	///Transfer the river profile boundary data to a database
	void transfer_boundary_data2database(QSqlDatabase *ptr_database, const int glob_prof_id);
	///Transfer the river profile wall break data to a database
	void transfer_wallbreak_data2database(QSqlDatabase *ptr_database, const int glob_prof_id);
	///Transfer the river profile dike break data to a database
	void transfer_dikebreak_data2database(QSqlDatabase *ptr_database, const int glob_prof_id);
	///Transfer the river profile bridge data to a database
	void transfer_bridge_data2database(QSqlDatabase *ptr_database, const int glob_prof_id);


	///Input the lateral boundary data from a database
	void input_lateral_boundary_per_database(const int glob_prof_id, QSqlTableModel *model);
	///Input the point boundary data from a database
	void input_point_boundary_per_database(const int glob_prof_id, QSqlTableModel *model);

	///Input the river profile wall break data from a database
	void input_wallbreak_data_per_database(QSqlDatabase *ptr_database, const int glob_prof_id);
	///Input the river profile dike break data from a database
	void input_dikebreak_data_per_database(QSqlDatabase *ptr_database, const int glob_prof_id);
	///Input the river profile bridge data from a database
	void input_bridge_data_per_database(QSqlDatabase *ptr_database, const int glob_prof_id);

	///Set error(s)
	Error set_error(const int err_type);

	///Set warning(s)
	Warning set_warning(const int warn_type);

};

#endif