#pragma once
/**\class Hyd_River_Profile_Connection_Inflow
	\author Daniel Bachmann et al.
	\author produced by the Institute of Hydraulic Engineering (IWW), RWTH Aachen University
	\version 0.0.1                                                              
	\date 2009 
*/
#ifndef HYD_RIVER_PROFILE_CONNECTION_INFLOW_H
#define HYD_RIVER_PROFILE_CONNECTION_INFLOW_H

//hyd sys libs
#include "_Hyd_River_Profile.h"


///Class of a river profile representing the inflow profile of a river model \ingroup hyd
/**

\see Hyd_Model_River
*/
class Hyd_River_Profile_Connection_Inflow : public Hyd_River_Profile_Connection_Standard 
{
public:
	///Default constructor
	Hyd_River_Profile_Connection_Inflow(void);
	///Default destructor
	~Hyd_River_Profile_Connection_Inflow(void);

	//methods
	///Check the inflow profile
	void check_profiles(void);
	///Set the initial condition to the profiles
	void set_init_condition(void);
	///Set the distance to the upstream profile (distance2upstream)
	void set_distance2upstream(const double riverstation_upstream);

	///Output the members to display/console
	void output_members(void);
	///Output the initialized and calculated members to display/console
	void output_setted_members(void);

	///Clone river profile
	void clone_profile(Hyd_River_Profile_Connection_Inflow *profile);

	///Make the syncronistation of the elements to a given time point
	void make_syncronisation(const double time_point, _Hyd_River_Profile *downstream);
	///Get the maximum change in a river section
	void get_max_changes_section(double *max_change_h, double *max_change_v, const bool timecheck);
	///Calculate the values for a hydrological balance
	void calculate_hydrological_balance(const double time_point);

	///Get the actual discharge for inflow (q_inflow)
	double get_q_inflow(void);

	///Get the actual discharge through this profile (same as double get_q_inflow(void))
	double get_actual_river_discharge(void);

	///Set the flag that the inflow discharge (q_inflow) is given by a coupled model directly
	void set_q_inflow_is_given(void);
	///Get the flag that the inflow discharge (q_inflow) is given by a coupled model directly
	bool get_q_inflow_is_given(void);
	///Set a interim q inflow by the solver
	void set_q_inflow_interim(const double value, const double time_point);

	///Set the flag that the inflow boundary is given by a h-value
	void set_h_inflow_is_given(void);
	///Get the flag that the inflow boundary is given by a h-value
	bool get_h_inflow_is_given(void);
	///Set a global h-value as a boundary condition (coupling 2d-1d directly)
	void set_global_h_boundary(const double global_h);
	///Get a global h-value as a boundary condition (coupling 2d-1d directly); it is set to the zero river segment
	double get_global_h_boundary(void);

	///Add a discharge to the inflow dicharge by a coupled model
	void add_q_inflow_coupling(const double q_inflow);
	///Add a discharge value to the current point discharge (same as add_q_inflow_coupling(const double q_inflow))
	void add_point_discharge(const double value);
	///Reset the inflow discharge 
	void reset_q_inflow_coupling(void);


	///Get the watervolume which flows in the system for a hydrological balance at the syncronisation timestep
	double get_inflow_volume(void);
	///Get the watervolume difference at a direct coupling 2d-1d between solver calculated inflow volume by a setted inflow waterlevel and the coupled inflow volume 
	double get_watervolume_difference_direct_coupling(void);

	///Output the result members for each riverprofiletype at every timestep
	void output_result_members_per_timestep(void);
	///Output the result members for each riverprofiletype at every timestep to tecplot file
	void output_result_members_per_timestep(ofstream *file);
	///Output the result members for each riverprofiletype at every timestep to csvfile
	void output_result_members_per_timestep2csv(ofstream *file);



	///Add the hydrological balance value of the point boundary condition discharges to the given pointer (here nothing happen)
	void add_hydro_balance_point_boundary(_hyd_hydrological_balance *given);

	///Add the total coupling/boundary discharges to the given profile (needed e.g. for h_inflow is given)
	void transfer_coupling_boundary2profile(Hyd_River_Profile_Connection_Standard *profile2transfer);

	///Get the stabilization discharge
	double get_stabilization_discharge(void);

	///Reset the hydrological balance value and the maximum values
	void reset_hydrobalance_maxvalues(void);

	///Connect instationary boundary curve to the profiles (pointwise inflow [m³/s])
	void connect_instat_point_boundarycurve(Hyd_Instationary_Boundary *bound_curve);

	///Set that a point boundary is given
	void set_point_bound_flag(const bool flag);

	///Get boundary point condition is applied
	bool boundary_point_is_applied(void);
	///Get boundary point value
	double get_boundary_point_value(const double time);

	///Get q-value
	double get_Q(void);

private:
	//members
	///Actual inflow discharge for this timestep 
	double q_inflow;
	///Watervolume which flows out of the system for a hydrological balance
	double inflow_volume;

	///Flag that the inflow discharge (q_inflow) is given by a coupled model directly
	bool q_inflow_is_given;

	///Flag if the inflow boundary is given by a h-value
	bool h_inflow_is_given;

	///Global waterlevel as boundary condition
	double global_h_boundary;
	///A interim discharge value given by the solver
	double q_interim;
	///Sum up the volume of the interim discharge given the solver to calculate the hydrological balance error due to coupling 2d-1d direct (waterlevel is given by the floodplain model)
	double v_interim;
	///The timepoint for the calculation step before (t-1); needed for the interim volume calculation 
	double old_calc_time_point;
	///The delta time to the old time step (t-1); needed for the interim volume calculation
	double old_delta_t;
	///The discharge of the old time step (t-1); needed for the interim volume calculation
	double old_discharge;
	///The discharge of the old time step (t-2); needed for the interim volume calculation
	double old_discharge_2;

	//methods
	///Check the connection profile type
	void check_connection_type_profile(void);
	///Set the inflow discharge (q_inflow) correspondig to to the stationary/instationary flag and the point in time
	void set_q_inflow(const double time_point);

	///Calculate the watervolume in the element
	void calculate_watervolume_element(void);

	///Set error(s)
	Error set_error(const int err_type);

	///Set warning(s)
	Warning set_warning(const int warn_type);
};
#endif