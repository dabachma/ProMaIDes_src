#pragma once
/**\class Hyd_River_Profile_Connection_Outflow
	\author Daniel Bachmann et al.
	\author produced by the Institute of Hydraulic Engineering (IWW), RWTH Aachen University
	\version 0.0.1                                                              
	\date 2009 
*/
#ifndef HYD_RIVER_PROFILE_CONNECTION_OUTFLOW_H
#define HYD_RIVER_PROFILE_CONNECTION_OUTFLOW_H

//hyd sys libs
#include "Hyd_River_Profile_Connection_Standard.h"

//forward declaration
class Hyd_River_Profile_Connection_Inflow;
class Hyd_River_Profile_Connection_Standard;

///Class of a river profile representing the outflow profile of a river model \ingroup hyd
/**

\see Hyd_Model_River
*/
class Hyd_River_Profile_Connection_Outflow : public _Hyd_River_Profile
{
public:
	///Default constructor
	Hyd_River_Profile_Connection_Outflow(void);
	///Default destructor
	~Hyd_River_Profile_Connection_Outflow(void);

	
	//methods
	///Transfer the river profile data to a database
	void transfer_profile_members2database(const int glob_prof_id, QSqlQuery *model, QSqlDatabase *ptr_database, const int rv_number);
	///Input the river profile data with the index from database selection
	void input_members_per_database(const int index, const QSqlQueryModel *query_result, QSqlDatabase *ptr_database);
	///Input the river profile boundary data from a database
	void input_boundary_data_per_database(QSqlDatabase *ptr_database, const int glob_prof_id);

	///Check the outflow profile with the two profiles before
	void check_profiles(_Hyd_River_Profile *upstream_profile, _Hyd_River_Profile *upupstream_profile);
	///Connect instationary boundary curve of waterlevel to the profiles
	void connect_instat_waterlevel_boundarycurve(Hyd_Instationary_Boundary *bound_curve);
	///Get the instationary found flag: the curve is found for lateral inflow
	bool get_instat_waterlevel_boundarycurve_found(void);
	///Get if a waterlevel boundary is set (true) or not
	bool get_boundary_waterlevel_flag(void);


	///Output the members to display/console
	void output_members(void);
	///Output the initialized and calculated members to display/console
	void output_setted_members(void);

	///Clone river profile
	void clone_profile(Hyd_River_Profile_Connection_Outflow *profile);

	///Output the result members for each riverprofiletype at every timestep
	void output_result_members_per_timestep(void);
	///Output the result members for each riverprofiletype at every timestep to tecplot file
	void output_result_members_per_timestep(ofstream *file);
	///Output the result members for each riverprofiletype at every timestep to csv file
	void output_result_members_per_timestep2csv(ofstream *file);

	///Reset all coupling discharges; use it before syncronization
	void reset_coupling_discharge(void);
	///Make the syncronistation of the elements to a given time point
	void make_syncronisation(const double time_point, _Hyd_River_Profile *downstream);
	///Get the maximum change in a river section
	void get_max_changes_section(double *max_change_h, double *max_change_v, const bool timecheck, const double timestep);


	///Calculate the values for a hydrological balance of the corresponding river segment (upstream) for each syncronisation step
	void calculate_hydrological_balance(const double time_point);

	///Set the actual river discharge through this profile (q_outflow) by the given upstream 
	void set_actual_river_discharge(_Hyd_River_Profile *upstream_profile);

	///Get the actual discharge through this profile (q_outflow)
	double get_calculated_river_discharge(void);
	///Get the actual discharge through this profile (q_outflow) and the coupling discharges
	double get_actual_river_discharge(void);

	///Set the outflow local waterlevel (h_outflow); take the given values by a boundary of other model or apply the normal flow condition
	void set_h_outflow_flow(const double global_delta_z_upstream, const double waterlevel_upstream);
    ///Reset the flag that the waterlevel (h_outflow) is given by a coupled model directly
    void reset_h_outflow_is_given(void);
	///Set the flag that the waterlevel (h_outflow) is given by a coupled model directly
	void set_h_outflow_is_given(void);
	///Get the flag that the waterlevel (h_outflow) is given by a coupled model directly
	bool get_h_outflow_is_given(void);
	///Set the waterlevel (h_outflow) by a coupled model directly
	void set_h_outflow_global(const double h_global);
	///Get the actual global waterlevel (s_value); here the h_outflow condition related to the global z coordinate is returned
	double get_actual_global_waterlevel(void);
	///Get the watervolume which flows out of the system for a hydrological balance
	double get_outflow_volume(void);
	///Get the watervolume difference between solver calculated outflow and the outflow volume at the syncronisation time point by a setted outflow waterlevel
	double get_watervolume_difference_direct_coupling(void);

	///Calculate the values for a hydrological balance for the outflow discharge; it is done every calculation step
	void calculate_hydrological_balance_outflow(const double time_point);

	///Set a pointer to the inbetween profiles of the river and the inflow-profile and the number of inbetween profiles (see check_hboundary2zmin_profiles(const double h_boundary))
	void set_ptr_riverprofiles(Hyd_River_Profile_Connection_Standard *ptr_inbetween_prof, const int number_inbetween_prof, Hyd_River_Profile_Connection_Inflow *ptr_inflow_prof);

	///Reset the hydrological balance value and the maximum values
	void reset_hydrobalance_maxvalues(void);
	///Reset the boundary data of the river profile
	void reset_boundary_data(void);

	///Get boundary condition is applied
	bool boundary_is_applied(void);
	///Get q-value
	double get_Q(void);

private:
	//members

	///Flag if a waterlevel boundary condition is set [m] (true)
	bool boundary_cond_waterlevel_flag;
	///Flag if the waterlevel boundary condition [m] is stationary (true)
	bool boundary_waterlevel_stat_flag;
	///Pointer for a stationary waterlevel boundary condition value [m]; it is allocated if this option is chosen
	double *value_boundary_waterlevel_stat;
	///Pointer which specifies for a instationary waterlevel boundary condition [m] the corresponding curve number; it is allocated if this option is chosen
	int *number_boundary_waterlevel_curve;
	///Pointer which specifies for a instationary waterlevell boundary condition [m] the corresponding curve
	Hyd_Instationary_Boundary *ptr_boundarycurve_waterlevel;
	///Flag if the instationary waterlevel boundary condition [m] curve is found
	bool instatbound_waterlevel_found_flag;

	///Discharge through the outflow profile
	double q_outflow;
	///Sum of an additional discharge through the outflow profile due to coupling discharges
	double sum_coupling_q;

	///Outflow boundary condition (local waterlevel)
	double h_outflow;
	///Outflow waterlevel for a normal flow condition
	double h_normal_flow;
	///The given outflow waterlevel 
	double h_outflow_given;


	///Flag that the waterlevel(h_outflow) is given by a coupled model directly
	bool h_outflow_is_given;

	///Watervolume which flows out of the system for a hydrological balance
	double outflow_volume;
	///The timepoint for the calculation step before (t-1)
	double old_calc_time_point;
	///The delta time to the old time step (t-1)
	double old_delta_t;
	///The discharge of the old time step (t-1)
	double old_discharge;
	///The discharge of the old time step (t-2)
	double old_discharge_2;
	///The maximum outflow discharge during calculation and the corresponding point in time; just outflow of the river is regarded
	_hyd_max_values max_outflow_q;
	///Watervolume which flows out of the system applied at the syncronisation time step; it is not equal to outflow_volume
	double outflow_volume_syncron;
	///Watervoulume which is additional given by coupling discharges
	double watervolume_by_coupling;


	///The waterlevel for the internal timestep before for boundary and coupling
	double h_coup_old;

	///Pointer to the other river inbetween profiles; it is needed to check the zmin values by a setted h-boundary, because h_bound can not be equal h_zmin(profile)
	Hyd_River_Profile_Connection_Standard *ptr_inbetween_profiles;
	///Number of inbetween profiles
	int number_inbetween_profiles;
	///Pointer to the inflow profile of the river (see ptr_inbetween_profiles) 
	Hyd_River_Profile_Connection_Inflow *ptr_inflow_profiles;


	//methods

	///Decide in the different keyvalues in the profile file
	void decide_keyvalues_file(const string key, string buffer, int *found_counter);
	///Decide which boundary conditions are choosen for a waterlevel condition (boundary value is given in [m])
	void decide_boundary_waterlevel_conditions(const double nr_value);
	///Check the connection profile type outflow
	void check_connection_type_profile(void);

	///Set the outflow local waterlevel (h_outflow) correspondig to to the stationary/instationary flag and the point in time by a boundary condition
	void set_h_outflow_by_boundary(const double time_point);

	///Calculate maximum values (q-max)
	void calculate_maximum_values(const double time_point);

	///Check if the zmin values of the river profiles are not equal to the setted h-boundary; if they are equal h-boundary is reduced by 0.001 m
	double check_hboundary2zmin_profiles(const double h_boundary_global);

	///Transfer the river profile boundary data to a database
	void transfer_boundary_data2database(QSqlDatabase *ptr_database, const int glob_prof_id);

	///Set error(s)
	Error set_error(const int err_type);
	///Set warning(s)
	Warning set_warning(const int warn_type);
};
#endif
