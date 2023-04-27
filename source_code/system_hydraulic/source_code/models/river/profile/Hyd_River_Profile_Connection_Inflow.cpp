//#include "Hyd_River_Profile_Connection_Inflow.h"
#include "Hyd_Headers_Precompiled.h"

//constructor
Hyd_River_Profile_Connection_Inflow::Hyd_River_Profile_Connection_Inflow(void){
	this->q_inflow=0.0;
	this->inflow_volume=0.0;
	
	this->q_inflow_is_given=false;
	this->h_inflow_is_given=false;
	this->global_h_boundary=0.0;

	this->q_interim=0.0;
	this->v_interim=0.0;
	this->old_calc_time_point=0.0;
	this->old_delta_t=0.0;
	this->old_discharge=0.0;
	this->old_discharge_2=0.0;

	this->must_found_number=4;

	//count the memory
	Sys_Memory_Count::self()->add_mem(sizeof(Hyd_River_Profile_Connection_Inflow)-sizeof(Hyd_River_Profile_Connection_Standard), _sys_system_modules::HYD_SYS);
}
//destructor
Hyd_River_Profile_Connection_Inflow::~Hyd_River_Profile_Connection_Inflow(void){
	//count the memory
	Sys_Memory_Count::self()->minus_mem(sizeof(Hyd_River_Profile_Connection_Inflow)-sizeof(Hyd_River_Profile_Connection_Standard), _sys_system_modules::HYD_SYS);
}
//__________________________________
//public
//Check the profiles
void Hyd_River_Profile_Connection_Inflow::check_profiles(void){
	try{
		_Hyd_River_Profile::check_profiles();
	}
	catch(Error msg){
		throw msg;
	}	

	//check if a boundary condition is set
	if(this->q_inflow_is_given!=true){
		if(this->boundary_cond_point_flag!=true){
			Error msg=this->set_error(0);
			throw msg;
		}
	}
	//check if a stationary boundary condition is >=0.0
	if(this->q_inflow_is_given!=true){
		if(this->boundary_cond_point_flag==true){
			if(this->boundary_point_stat_flag==true){
				if(*(this->value_boundary_point_stat)<0.0){
					Error msg=this->set_error(4);
					throw msg;
				}
			}
		}
	}

}
//Set the initial condition to the profiles
void Hyd_River_Profile_Connection_Inflow::set_init_condition(void){
	//calculate the initial watervolume for a hydrological balance
	this->watervolume_init=this->get_init_condition_value()*this->distance2downstream*0.5;
	//set the waterlevels at the beginning to the init condition
	this->typ_of_profile->set_waterlevels2init_condition(this->init_condition);
}
//Set the distance to the upstream profile (distance2upstream)
void Hyd_River_Profile_Connection_Inflow::set_distance2upstream(const double riverstation_upstream){
	//first to get no warning
	this->distance2upstream=riverstation_upstream;
	//inflow profile has no distance to upstream!
	this->distance2upstream=0.0;
}
//Output the members
void Hyd_River_Profile_Connection_Inflow::output_members(void){
	ostringstream prefix;
	prefix << "PROF_"<< this->profile_number<<"> ";
	Sys_Common_Output::output_hyd->set_userprefix(prefix.str());

	ostringstream cout;
	cout << "GENERAL" << endl;
	cout << " Name                                          : " << this->name <<endl;
	cout << " Delta x for the tables                        : " << this->delta_x_table << label::m<<endl;
	cout << " Connection-Type                               : " << this->convert_connectiontype2string(this->connection) << endl;
	cout << " Type                                          : " << this->convert_profiletype2string(this->type)<<endl;;
	cout << " Init-h                                        : " << this->init_condition<< label::m<<endl;
	cout << "BOUNDARY"<< endl; 
	if(this->boundary_cond_point_flag==false){
		cout << " No pointwise boundary " <<endl;;
	}
	else if (this->boundary_cond_point_flag==true && this->boundary_point_stat_flag==true){
		cout << " Pointwise stationary boundary value           : " << *this->value_boundary_point_stat << label::qm_per_sec << endl; 
	}
	else if (this->boundary_cond_point_flag==true && this->boundary_point_stat_flag==false){
		cout << " Pointwise instationary boundary curve-no.     : " << *this->number_boundary_point_curve <<endl; 
	}
	if(this->boundary_cond_lateral_flag==false){
		cout << " No lateral boundary " << endl;
	}
	else if (this->boundary_cond_lateral_flag==true && this->boundary_lateral_stat_flag==true){
		cout << " Lateral stationary boundary value             : " << *this->value_boundary_lateral_stat << label::qm_per_secm << endl; 
	}
	else if (this->boundary_cond_lateral_flag==true && this->boundary_lateral_stat_flag==false){
		cout << " Lateral instationary boundary curve-no.       : " << *this->number_boundary_lateral_curve<<endl; 
	}
	cout <<"COUPLING"<< endl;
	if(this->overflow_left_flag==false){
		cout << " No overflow coupling of the left bank" <<endl;
	}
	else{
		cout << " Overflow coupling left bank Polenifactor      : " << this->poleni_factor_left_overflow << label::no_unit<< endl; 
	}
	if(this->overflow_right_flag==false){
		cout << " No overflow coupling of the right bank" <<endl;
	}
	else{
		cout << " Overflow coupling right bank Polenifactor     : " << this->poleni_factor_right_overflow << label::no_unit<< endl; 
	}
	cout <<"BREAKING"<< endl;
	if(this->break_flag_left==false){
		cout << " No breaking of the left bank" <<endl;
	}
	else{
		cout << " Left base point index                         : " << this->index_basepoint_left << endl;
		cout << " Global z                                      : "<< this->typ_of_profile->points[this->index_basepoint_left].get_global_z_coordinate() << label::m <<endl;
		if(this->left_break_params->abrupt_fails_flag==false){
			cout << " Dikebreak; critical velocity                  : " << this->left_break_params->resistance<< label::m_per_sec << endl;
		}
		else{
			cout << " Wallbreak; resistance                         : " << this->left_break_params->resistance<< label::stress_unit << endl;
			cout << " Abrupt opening                                : " <<this->left_break_params->abrupt_opening << label::m << endl;
		}
	}

	if(this->break_flag_right==false){
		cout << " No breaking of the right bank" <<endl;
	}
	else{
		cout << " Right base point index                        : " << this->index_basepoint_right <<endl;
		cout << " Global z                                      : "<< this->typ_of_profile->points[this->index_basepoint_right].get_global_z_coordinate() << label::m <<endl;
		if(this->right_break_params->abrupt_fails_flag==false){
			cout <<" Dikebreak; critical velocity                  : " << this->right_break_params->resistance<< label::m_per_sec << endl;
		}
		else{
			cout << " Wallbreak; resistance                         : " << this->right_break_params->resistance<< label::stress_unit <<endl;
			cout << " Abrupt opening                                : " <<this->right_break_params->abrupt_opening << label::m << endl;
		}
	}
	cout <<"POINTS"<< endl;
	this->typ_of_profile->output_members(&cout);

	Sys_Common_Output::output_hyd->output_txt(&cout,true);
	Sys_Common_Output::output_hyd->rewind_userprefix();
}
//Output the setted and calculated members
void Hyd_River_Profile_Connection_Inflow::output_setted_members(void){
	//set the prefix
	ostringstream prefix;
	prefix << "PROF_"<< this->profile_number<<"> ";
	Sys_Common_Output::output_hyd->set_userprefix(prefix.str());

	ostringstream cout;

	if(this->boundary_cond_point_flag==true && this->boundary_point_stat_flag==false){
		cout << "Pointwise boundary curve is found: " << functions::convert_boolean2string(this->instatbound_point_found_flag)<<" No. " << *this->number_boundary_point_curve<<endl; ;
		Sys_Common_Output::output_hyd->output_txt(&cout,true);;
	}
	if(this->boundary_cond_lateral_flag==true && this->boundary_lateral_stat_flag==false){
		cout << "Lateral boundary curve is found: " << functions::convert_boolean2string(this->instatbound_lateral_found_flag)<<" No. " << *this->number_boundary_lateral_curve<<endl; ;
		Sys_Common_Output::output_hyd->output_txt(&cout,true);
	}
	

	//output the sgements (for development)
	//this->typ_of_profile->output_area_segments();

	//output the tables (for development)
	//this->typ_of_profile->output_tables();

	//rewind the prefix
	Sys_Common_Output::output_hyd->rewind_userprefix();
}
//Clone river profile
void Hyd_River_Profile_Connection_Inflow::clone_profile(Hyd_River_Profile_Connection_Inflow *profile){
	Hyd_River_Profile_Connection_Standard::clone_profile(profile);
	this->q_inflow_is_given=profile->q_inflow_is_given;
	this->h_inflow_is_given=profile->h_inflow_is_given;
}
//Make the syncronistation of the elements to a given time point
void Hyd_River_Profile_Connection_Inflow::make_syncronisation(const double time_point, _Hyd_River_Profile *downstream){
	this->set_actual_lateral_discharge(time_point);
	this->set_q_inflow(time_point);

	this->calculate_max_h2basepoint();
	this->typ_of_profile->calculate_velocity_energy_head(time_point, downstream);
	
}
//Get the maximum change in a river section
void Hyd_River_Profile_Connection_Inflow::get_max_changes_section(double *max_change_h, double *max_change_v, const bool timecheck){

	//max h change
	this->ds_dt_coup=0.0;
	this->ds_dt_coup=this->q_lateral+this->q_point+this->q_1d_coupling+
		this->q_dikebreak_coupling_left+this->q_dikebreak_coupling_right+
		this->q_left_bank+this->q_right_bank+this->q_structure_coupling +this->q_inflow;

	this->ds_dt_coup=this->ds_dt_coup/((this->get_distance2downstream())*0.5*this->typ_of_profile->get_area2calc()/this->typ_of_profile->get_actual_local_waterlevel_h());

	//this->ds_dt_coup=this->typ_of_profile->get_actual_local_waterlevel_h()-this->typ_of_profile->get_waterlevel_from_table(this->typ_of_profile->get_area2calc()+this->ds_dt_coup);
	//this->ds_dt_coup=this->ds_dt_coup/(this->typ_of_profile->get_area2calc()/(this->typ_of_profile->get_actual_local_waterlevel_h()));


	*max_change_h=abs(this->ds_dt_coup_old-this->ds_dt_coup);
	//*max_change_h=abs(this->ds_dt_coup);
	if(timecheck==false){
		this->ds_dt_coup_old=this->ds_dt_coup;
	}


}
//Calculate the values for a hydrological balance
void Hyd_River_Profile_Connection_Inflow::calculate_hydrological_balance(const double time_point){

	//calculate the maximum discharge
	if(this->q_inflow>this->q_value_max.maximum){
		this->q_value_max.maximum=this->q_inflow;
		this->q_value_max.time_point=time_point;
	}
	this->typ_of_profile->calculate_maximum_values(time_point);
	this->typ_of_profile->set_old_time_point(time_point);

	Hyd_River_Profile_Connection_Standard::calculate_hydrological_balance(time_point);
	
	this->inflow_volume=this->inflow_volume+this->q_inflow*this->delta_time;

	this->calculate_watervolume_element();
}
//get q_inflow
double Hyd_River_Profile_Connection_Inflow::get_q_inflow(void){
	//set the velocity of this inflow profile
	this->typ_of_profile->set_actuel_flowvelocity(this->q_inflow);
	return this->q_inflow;
}
//Get the actual discharge through this profile (same as double get_q_inflow(void))
double Hyd_River_Profile_Connection_Inflow::get_actual_river_discharge(void){
	//set the velocity of this inflow profile
	this->typ_of_profile->set_actuel_flowvelocity(this->q_inflow);
	return this->q_inflow;
}
//Set the flag that the inflow discharge (q_inflow) is given by a coupled model directly
void Hyd_River_Profile_Connection_Inflow::set_q_inflow_is_given(void){

	if(this->boundary_cond_point_flag==true){
		Warning msg=this->set_warning(0);
		//reaction
		this->boundary_cond_point_flag=false;
		msg.output_msg(2);
	}
	
	this->q_inflow_is_given=true;
}
//Get the flag that the inflow discharge (q_inflow) is given by a coupled model directly
bool Hyd_River_Profile_Connection_Inflow::get_q_inflow_is_given(void){
	return this->q_inflow_is_given;
}
//Set a interim q inflow by the solver
void Hyd_River_Profile_Connection_Inflow::set_q_inflow_interim(const double value, const double time_point){
	this->q_interim=value;

	double delta_time=time_point-this->old_calc_time_point;
	if(abs(delta_time)<1e-12){
		return;
	}
	if(delta_time>=0.0){
		this->old_calc_time_point=time_point;
		this->v_interim=this->v_interim+(this->old_discharge*(1.0/3.0)+this->q_interim*(2.0/3.0))*delta_time;
		this->old_delta_t=delta_time;
		this->old_discharge_2=this->old_discharge;
		if(this->old_discharge<0.0){
			this->old_discharge=(this->old_discharge*(1.0/3.0)+this->q_interim*(2.0/3.0));
		}
		else{
			this->old_discharge=this->q_interim;
			this->old_discharge_2=this->old_discharge;
		}
	}
	else{
		this->v_interim=this->v_interim-this->old_discharge*this->old_delta_t;
		this->old_calc_time_point=this->old_calc_time_point-this->old_delta_t;
		delta_time=time_point-this->old_calc_time_point;
		this->v_interim=this->v_interim+(this->old_discharge_2*(1.0/3.0)+this->q_interim*(2.0/3.0))*delta_time;
		this->old_delta_t=delta_time;
		this->old_discharge=(this->old_discharge_2*(1.0/3.0)+this->q_interim*(2.0/3.0));
		this->old_calc_time_point=time_point;
	}
}
//Set the flag that the inflow boundary is given by a h-value
void Hyd_River_Profile_Connection_Inflow::set_h_inflow_is_given(void){
	this->h_inflow_is_given=true;
	//is given by the solver by the outflow of the zero riversegment
	this->set_q_inflow_is_given();
}
//Set a global h-value as a boundary condition (coupling 2d-1d directly)
void Hyd_River_Profile_Connection_Inflow::set_global_h_boundary(const double global_h){
	if(global_h<this->typ_of_profile->get_global_z_min()){
		this->global_h_boundary=this->typ_of_profile->get_global_z_min();
	}
	else{
		this->global_h_boundary=global_h;
	}
}
//Get a global h-value as a boundary condition (coupling 2d-1d directly); it is set to the zero river segment
double Hyd_River_Profile_Connection_Inflow::get_global_h_boundary(void){
	return this->global_h_boundary;
}
//Get the flag that the inflow boundary is given by a h-value
bool Hyd_River_Profile_Connection_Inflow::get_h_inflow_is_given(void){
	return this->h_inflow_is_given;
}
//Add a discharge to the inflow discharge by a coupled model
void Hyd_River_Profile_Connection_Inflow::add_q_inflow_coupling(const double q_inflow){
	if(this->q_inflow_is_given==true){
		this->q_inflow=this->q_inflow+q_inflow;
	}
}
//Add a discharge value to the current point discharge
void Hyd_River_Profile_Connection_Inflow::add_point_discharge(const double value){
	if(this->q_inflow_is_given==true){
		this->q_inflow=this->q_inflow+value;
	};
}
//Reset the inflow dicharge 
void Hyd_River_Profile_Connection_Inflow::reset_q_inflow_coupling(void){
	if(this->q_inflow_is_given==true){
		this->q_inflow=0.0;
	}
}
//Get the watervolume which flows in the system for a hydrological balance
double Hyd_River_Profile_Connection_Inflow::get_inflow_volume(void){
	return this->inflow_volume;
}
//Get the watervolume difference at a direct coupling 2d-1d between solver calculated inflow volume by a setted inflow waterlevel and the coupled inflow volume 
double Hyd_River_Profile_Connection_Inflow::get_watervolume_difference_direct_coupling(void){
	if(this->h_inflow_is_given==false){
		//no direct coupling=>no difference	
		return 0.0;
	}
	else{	
		return abs(this->v_interim-this->inflow_volume);
	}
}
//Output the result members for each riverprofiletype at every timestep
void Hyd_River_Profile_Connection_Inflow::output_result_members_per_timestep(void){
	ostringstream cout;
	_Hyd_River_Profile::output_result_members_per_timestep(&cout);
	cout << W(10) <<  this->q_inflow ;
	cout << endl;
	Sys_Common_Output::output_hyd->output_txt(&cout,true);

}
//Output the result members for each riverprofiletype at every timestep to tecplot file
void Hyd_River_Profile_Connection_Inflow::output_result_members_per_timestep(ofstream *file){
	
	_Hyd_River_Profile::output_result_members_per_timestep(file);
	*file << W(12) << P(4) << FORMAT_FIXED_REAL << this->q_inflow <<  endl;
}
///Output the result members for each riverprofiletype at every timestep to csvfile
void Hyd_River_Profile_Connection_Inflow::output_result_members_per_timestep2csv(ofstream *file) {
	_Hyd_River_Profile::output_result_members_per_timestep2csv(file);
	*file << W(12) << P(4) << FORMAT_FIXED_REAL << this->q_inflow << endl;
}
///Add the hydrological balance value of the point boundary condition discharges to the given pointer (here nothing happen)
void Hyd_River_Profile_Connection_Inflow::add_hydro_balance_point_boundary(_hyd_hydrological_balance *given){
	//here nothing happen, the point inflow of the inflow profile is extra counted
	_hyd_hydrological_balance *dummy;
	dummy=given;
}	
//Add the total coupling/boundary discharges to the given profile (needed e.g. for h_inflow is given)
void Hyd_River_Profile_Connection_Inflow::transfer_coupling_boundary2profile(Hyd_River_Profile_Connection_Standard *profile2transfer){
	if(this->typ_of_profile!=NULL && this->h_inflow_is_given==true){
		if(profile2transfer!=NULL){
			profile2transfer->add_coupling_discharge_1d(this->q_1d_coupling);
			this->q_1d_coupling=0.0;
			profile2transfer->add_coupling_discharge_left_bank(this->q_left_bank);
			this->q_left_bank=0.0;
			profile2transfer->add_coupling_discharge_right_bank(this->q_right_bank);
			this->q_right_bank=0.0;
			profile2transfer->add_dikebreak_coupling_discharge_left_bank(this->q_dikebreak_coupling_left);
			this->q_dikebreak_coupling_left=0.0;
			profile2transfer->add_dikebreak_coupling_discharge_right_bank(this->q_dikebreak_coupling_right);
			this->q_dikebreak_coupling_right=0.0;
			profile2transfer->add_structure_coupling_discharge(this->q_structure_coupling);
			this->q_structure_coupling=0.0;
			profile2transfer->add_lateral_discharge(this->q_lateral);
			this->q_lateral=0.0;
		}

	}
}
///Get the stabilization discharge
double Hyd_River_Profile_Connection_Inflow::get_stabilization_discharge(void){
	return constant::stabilization_q;
}
//Reset the hydrological balance value and the maximum values
void Hyd_River_Profile_Connection_Inflow::reset_hydrobalance_maxvalues(void){
	Hyd_River_Profile_Connection_Standard::reset_hydrobalance_maxvalues();
	this->global_h_boundary=0.0;

	this->q_interim=0.0;
	this->v_interim=0.0;
	this->old_calc_time_point=0.0;
	this->old_delta_t=0.0;
	this->old_discharge=0.0;
	this->old_discharge_2=0.0;

	this->q_inflow=0.0;
	this->inflow_volume=0.0;

}
//Connect instationary boundary curve to the profiles (pointwise inflow)
void Hyd_River_Profile_Connection_Inflow::connect_instat_point_boundarycurve(Hyd_Instationary_Boundary *bound_curve){
	ostringstream cout;
	if(this->boundary_cond_point_flag==false){//no boundary
		return;
	}
	else if(this->boundary_cond_point_flag==true && this->boundary_point_stat_flag==true){//stat boundary
		return;
	}
	else if(this->boundary_cond_point_flag==true && this->boundary_point_stat_flag==false){//instat boundary
		if(*this->number_boundary_point_curve==bound_curve->get_curve_number()){//the right curve is found
			try{
				bound_curve->set_type(_hyd_bound_type::point_type);
			}
			catch(Error msg){
				throw msg;
			}
			this->ptr_boundarycurve_point=bound_curve;
			this->instatbound_point_found_flag=true;
			cout <<"Connection of Profile " << this->profile_number << " with point boundary curve " << bound_curve->get_curve_number() << " is found" << endl;
			Sys_Common_Output::output_hyd->output_txt(&cout);

			//check the instationary boundary curve, just for the inflow profile
			try{
				this->ptr_boundarycurve_point->check_boundary_curve_neg_null_values();
				this->ptr_boundarycurve_point->check_starting_time(0.0);
			}
			catch(Error msg){
				throw msg;
			}
			if(this->q_inflow_is_given==true){
				Warning msg=this->set_warning(0);
				//reaction
				this->boundary_cond_point_flag=false;
				msg.output_msg(2);
			}

		
		}
	}
}
	//Set that a point boundary is given
void Hyd_River_Profile_Connection_Inflow::set_point_bound_flag(const bool flag){
	this->boundary_cond_point_flag=flag;
}
//Get boundary point condition is applied
bool Hyd_River_Profile_Connection_Inflow::boundary_point_is_applied(void) {
	return this->boundary_cond_point_flag;

}
//Get boundary point value
double Hyd_River_Profile_Connection_Inflow::get_boundary_point_value(const double time) {
	double value = 0.0;

	if (this->boundary_cond_point_flag == false) {
		value = 0.0;

	}
	else {
		//stationary
		if (this->boundary_point_stat_flag == true) {
			value = *this->value_boundary_point_stat;
		}
		//instationary
		else if (this->boundary_point_stat_flag == false) {
			value = this->ptr_boundarycurve_point->calculate_actuel_boundary_value(time);
		}
	}

	return value;

}
///Get q-value
double Hyd_River_Profile_Connection_Inflow::get_Q(void) {
	return this->q_inflow;
}
//___________________________________
//private
//Check the connection profile type
void Hyd_River_Profile_Connection_Inflow::check_connection_type_profile(void){
	if(this->connection!=_hyd_connection_types::INFLOW_CONN){
		Error msg=this->set_error(2);
		throw msg;
	}
	if(this->type!=_hyd_profile_types::RIVER_TYPE){
		Error msg=this->set_error(3);
		throw msg;
	}

}
//Set the inflow discharge (q_inflow) correspondig to to the stationary/instationary flag and the point in time
void Hyd_River_Profile_Connection_Inflow::set_q_inflow(const double time_point){
	if(this->q_inflow_is_given==true && this->h_inflow_is_given==false){
		return;
	}
	else if(this->q_inflow_is_given==true && this->h_inflow_is_given==true){
		this->q_inflow=this->q_interim;
		return;
		
	}

	this->q_inflow=0.0;
	
	//stationary
	if(this->boundary_point_stat_flag==true){
		this->q_inflow=*this->value_boundary_point_stat;
	}
	//instationary
	else if(this->boundary_point_stat_flag==false){
		this->q_inflow=this->ptr_boundarycurve_point->calculate_actuel_boundary_value(time_point);
	}
}
//Calculate the watervolume in the element
void Hyd_River_Profile_Connection_Inflow::calculate_watervolume_element(void){
	if(this->h_inflow_is_given==false){
		this->watervolume=this->typ_of_profile->get_area2calc()*this->distance2downstream*0.5;
	}
	else{
		this->watervolume=0.0;
	}
}
//set the error
Error Hyd_River_Profile_Connection_Inflow::set_error(const int err_type){
		string place="Hyd_River_Profile_Connection_Inflow::";
		string help;
		string reason;
		int type=0;
		bool fatal=false;
		stringstream info;
		Error msg;

	switch (err_type){
		case 0://check if a boundary condition is set 
			place.append("check_profiles(_Hyd_River_Profile *upstream_profile, _Hyd_River_Profile *downstream_profile)");
			reason="No boundary condition is set; it is necessary for the inflow profile";
			help="Check the inflow profile of the river";
			type=15;
			break;
		case 1://wrong input
			place.append("decide_keyvalues_file(const string key, string buffer, int *found_counter)");
			reason="There is a problem with the input; wrong sign is read in";
			help="Check the Profile in the profile file";
			type=1;
			break;
		case 2://the first profil is not a inflow type
			place.append("check_connection_type_profile");
			reason="The first profile must be an inflow profile";
			help="Check the connection type of the first profile";
			type=15;
			break;
		case 3://the first profil have to be a river profile 
			place.append("check_connection_type_profile");
			reason="The inflow profile must be a river profile";
			help="Check the profile type of the inflow profile";
			type=15;
			break;
		case 4:
			place.append("check_profiles(_Hyd_River_Profile *upstream_profile, _Hyd_River_Profile *downstream_profile)");
			reason="The given stationary inflow discharge is <0.0";
			help="Check the inflow profile of the river";
			type=15;
			break;
		default:
			place.append("set_error(const int err_type)");
			reason ="Unknown flag!";
			help="Check the flags";
			type=6;
	}
	msg.set_msg(place, reason, help, type, fatal);
	msg.make_second_info(info.str());
	return msg;
}

//Set the warning
Warning Hyd_River_Profile_Connection_Inflow::set_warning(const int warn_type){
	string place="Hyd_River_Profile_Connection_Inflow::";
		string help;
		string reason;
		string reaction;
		int type=0;
		Warning msg;
		stringstream info;

	switch (warn_type){
		case 0://Type of material value does not match
			place.append("set_q_inflow_is_given(void)/connect_instat_point_boundarycurve(Hyd_Instationary_Boundary *bound_curve)") ;
			reason="The connected boundary values for the inflow discharge are not used; a coupling is set";
			reaction="The boundary condition flag for the inflow discharge is set to false";
			help= "Do not set any boundary condition in a case of coupling the inflow profile to an other river model";
			type=10;	
			break;
		default:
			place.append("set_warning(const int warn_type)");
			reason ="Unknown flag!";
			help="Check the flags";
			type=5;
	}
	msg.set_msg(place,reason,help,reaction,type);
	msg.make_second_info(info.str());
	return msg;
}