//#include "Hyd_River_Profile_Connection_Standard.h"
#include "Hyd_Headers_Precompiled.h"

//constructor
Hyd_River_Profile_Connection_Standard::Hyd_River_Profile_Connection_Standard(void){

	//lateral boundary condition
	this->boundary_cond_lateral_flag=false;
	this->boundary_lateral_stat_flag=false;
	this->value_boundary_lateral_stat=NULL;
	this->number_boundary_lateral_curve=NULL;
	this->ptr_boundarycurve_lateral=NULL;
	this->instatbound_lateral_found_flag=false; 

	//point boundary
	this->boundary_cond_point_flag=false;
	this->boundary_point_stat_flag=false;
	this->value_boundary_point_stat=NULL;
	this->number_boundary_point_curve=NULL;
	this->ptr_boundarycurve_point=NULL;
	this->instatbound_point_found_flag=false;



	//discharges per timestep
	this->q_river=0.0;
	this->q_lateral=0.0;
	this->q_point=0.0;

	this->ds_dt_coup_old=0.0;
	this->ds_dt_coup=0.0;

	//hydrological balance values
	this->lateral_boundary_volume.volume_in=0.0;
	this->lateral_boundary_volume.volume_out=0.0;
	this->lateral_boundary_volume.volume_total=0.0;
	this->point_boundary_volume.volume_in=0.0;
	this->point_boundary_volume.volume_out=0.0;
	this->point_boundary_volume.volume_total=0.0;
	
	this->error_zero_outflow_volume=0.0;
	this->q_zero_outflow_error=0.0;
	this->old_timepoint_error=0.0;
	this->old_delta_t_error=0.0;
	this->old_discharge_error=0.0;
	this->old_discharge_2_error=0.0;

	//for input per file
	this->must_found_number=4;

	//count the memory
	Sys_Memory_Count::self()->add_mem(sizeof(Hyd_River_Profile_Connection_Standard), _sys_system_modules::HYD_SYS);
}
//destructor
Hyd_River_Profile_Connection_Standard::~Hyd_River_Profile_Connection_Standard(void){
	
	if(this->number_boundary_lateral_curve!=NULL){
		delete this->number_boundary_lateral_curve;
		this->number_boundary_lateral_curve=NULL;
	}
	if(this->value_boundary_lateral_stat!=NULL){
		delete this->value_boundary_lateral_stat;
		this->value_boundary_lateral_stat=NULL;
	}
	if(this->number_boundary_point_curve!=NULL){
		delete this->number_boundary_point_curve;
		this->number_boundary_point_curve=NULL;
	}
	if(this->value_boundary_point_stat!=NULL){
		delete this->value_boundary_point_stat;
		this->value_boundary_point_stat=NULL;
	}
	//count the memory
	Sys_Memory_Count::self()->minus_mem(sizeof(Hyd_River_Profile_Connection_Standard), _sys_system_modules::HYD_SYS);
}
//__________________________________
//public
//Transfer the river profile data to a database
void Hyd_River_Profile_Connection_Standard::transfer_profile_members2database(const int glob_prof_id, QSqlQuery *model, QSqlDatabase *ptr_database, const int rv_number){

	//the table is set (the name and the column names) and allocated
	try{
		_Hyd_River_Profile::set_profile_table(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}
	this->glob_prof_id=glob_prof_id;
	//set the query via a query string
	ostringstream query_string;
	query_string << "INSERT INTO  " << _Hyd_River_Profile::profile_table->get_table_name();
	query_string <<" ( ";
	query_string << _Hyd_River_Profile::profile_table->get_column_name(hyd_label::profdata_rvno) <<" , ";
	query_string << _Hyd_River_Profile::profile_table->get_column_name(hyd_label::profdata_name) <<" , ";
	query_string << _Hyd_River_Profile::profile_table->get_column_name(hyd_label::profdata_ldist) <<" , ";
	query_string << _Hyd_River_Profile::profile_table->get_column_name(hyd_label::profdata_deltax) <<" , ";
	query_string << _Hyd_River_Profile::profile_table->get_column_name(hyd_label::profdata_contyp) <<" , ";
	query_string << _Hyd_River_Profile::profile_table->get_column_name(hyd_label::profdata_proftyp) <<" , ";
	query_string << _Hyd_River_Profile::profile_table->get_column_name(hyd_label::profdata_init) <<" , ";
	query_string << _Hyd_River_Profile::profile_table->get_column_name(hyd_label::profdata_prof_id) <<" , ";
	query_string << _Hyd_River_Profile::profile_table->get_column_name(hyd_label::profdata_glob_id) <<" , ";
	
	query_string << _Hyd_River_Profile::profile_table->get_column_name(hyd_label::profdata_over_r) <<" , ";
	query_string << _Hyd_River_Profile::profile_table->get_column_name(hyd_label::profdata_over_l) <<" , ";
	query_string << _Hyd_River_Profile::profile_table->get_column_name(hyd_label::profdata_poleni_r) <<" , ";
	query_string << _Hyd_River_Profile::profile_table->get_column_name(hyd_label::profdata_poleni_l) <<" , ";
	query_string << _Hyd_River_Profile::profile_table->get_column_name(hyd_label::profdata_base_left) <<" , ";
	query_string << _Hyd_River_Profile::profile_table->get_column_name(hyd_label::profdata_base_right) <<" , ";

	query_string << _Hyd_River_Profile::profile_table->get_column_name(label::areastate_id) <<" , ";
	query_string << _Hyd_River_Profile::profile_table->get_column_name(label::measure_id) <<" , ";
	query_string << _Hyd_River_Profile::profile_table->get_column_name(label::applied_flag) <<" , ";

	query_string << _Hyd_River_Profile::profile_table->get_column_name(hyd_label::profdata_polyline) <<" ) ";
	
	query_string << " VALUES ( ";
	query_string << rv_number << " , " ;
	query_string << "'"<< this->name<< "' , " ;
	query_string << FORMAT_FIXED_REAL << P(15);
	query_string << this->river_station<< " , " ;
	query_string << this->delta_x_table<< " , " ;
	query_string << "'"<<this->convert_connectiontype2string(this->connection) << "' , " ;
	query_string << "'"<<this->convert_profiletype2string(this->type) << "' , " ;
	query_string <<  this->init_condition<< " , " ;
	query_string << this->profile_number<< " , " ;
	query_string << glob_prof_id<< " , " ;

	query_string << functions::convert_boolean2string(this->overflow_right_flag)<< " , " ;
	query_string << functions::convert_boolean2string(this->overflow_left_flag)<< " , " ;

	query_string << this->poleni_factor_right_overflow<< " , " ;
	query_string << this->poleni_factor_left_overflow<< " , " ;
	query_string << this->index_basepoint_left<< " , " ;
	query_string << this->index_basepoint_right<< " , " ;

	query_string << this->system_id.area_state<< " , " ;
	query_string << this->system_id.measure_nr<< " , " ;
	query_string << "true" << " , " ;

	//set the polgon points
	string polyline_string;
	polyline_string=this->typ_of_profile->get_polyline2sql_string();
	query_string << polyline_string <<" ) ";


	Data_Base::database_request(model, query_string.str(), ptr_database);
	
	if(model->lastError().isValid()){
		Warning msg=this->set_warning(0);
		ostringstream info;
		info << "Table Name                : " << _Hyd_River_Profile::profile_table->get_table_name() << endl;
		info << "Table error info          : " << model->lastError().text().toStdString() << endl;
		info << "Profile number            : " << this->profile_number << endl;
		info << "Profile name              : " << this->name << endl;
		msg.make_second_info(info.str());
		msg.output_msg(2);	
	}

	try{
		//boundary condition
		this->transfer_boundary_data2database(ptr_database, glob_prof_id);
		//wall break
		this->transfer_wallbreak_data2database(ptr_database, glob_prof_id);
		//dike break
		this->transfer_dikebreak_data2database(ptr_database, glob_prof_id);
		//bridge
		this->transfer_bridge_data2database(ptr_database, glob_prof_id);
		//profile points of the profile type
		this->typ_of_profile->transfer_profile_points2database(ptr_database, glob_prof_id);
	}
	catch(Error msg){
		ostringstream info;
		info << "Profile number            : " << this->profile_number << endl;
		info << "Profile name              : " << this->name << endl;
		msg.make_second_info(info.str());
		throw msg;
	}

}
//Input the river profile data with the index from database selection
void Hyd_River_Profile_Connection_Standard::input_members_per_database(const int index, const QSqlQueryModel *query_result, QSqlDatabase *ptr_database){

	
	try{
		//set the river profile number
		this->profile_number=query_result->record(index).value((_Hyd_River_Profile::profile_table->get_column_name(hyd_label::profdata_prof_id)).c_str()).toInt();

		//read out the results of the request for this index
		this->glob_prof_id=query_result->record(index).value((_Hyd_River_Profile::profile_table->get_column_name(hyd_label::profdata_glob_id)).c_str()).toInt();
		//name
		this->name=query_result->record(index).value((_Hyd_River_Profile::profile_table->get_column_name(hyd_label::profdata_name)).c_str()).toString().toStdString();
		//riverstation
		this->river_station=query_result->record(index).value((_Hyd_River_Profile::profile_table->get_column_name(hyd_label::profdata_ldist)).c_str()).toDouble();
		//delta_x
		this->delta_x_table=query_result->record(index).value((_Hyd_River_Profile::profile_table->get_column_name(hyd_label::profdata_deltax)).c_str()).toDouble();
		//connection type
		this->connection=this->convert_string2connectiontype(query_result->record(index).value((_Hyd_River_Profile::profile_table->get_column_name(hyd_label::profdata_contyp)).c_str()).toString().toStdString());
		//profile type
		this->type=this->convert_string2profiletype(query_result->record(index).value((_Hyd_River_Profile::profile_table->get_column_name(hyd_label::profdata_proftyp)).c_str()).toString().toStdString());
		//init-condition
		this->init_condition=query_result->record(index).value((_Hyd_River_Profile::profile_table->get_column_name(hyd_label::profdata_init)).c_str()).toDouble();
		//overflow parameters
		this->overflow_right_flag=query_result->record(index).value((_Hyd_River_Profile::profile_table->get_column_name(hyd_label::profdata_over_r)).c_str()).toBool();
		this->overflow_left_flag=query_result->record(index).value((_Hyd_River_Profile::profile_table->get_column_name(hyd_label::profdata_over_l)).c_str()).toBool();
		this->poleni_factor_right_overflow=query_result->record(index).value((_Hyd_River_Profile::profile_table->get_column_name(hyd_label::profdata_poleni_r)).c_str()).toDouble();
		this->poleni_factor_left_overflow=query_result->record(index).value((_Hyd_River_Profile::profile_table->get_column_name(hyd_label::profdata_poleni_l)).c_str()).toDouble();
		//base points
		this->index_basepoint_right=query_result->record(index).value((_Hyd_River_Profile::profile_table->get_column_name(hyd_label::profdata_base_right)).c_str()).toInt();
		this->index_basepoint_left=query_result->record(index).value((_Hyd_River_Profile::profile_table->get_column_name(hyd_label::profdata_base_left)).c_str()).toInt();

		if(this->index_basepoint_right>=0){
			this->break_flag_right=true;
		}
		if(this->index_basepoint_left>=0){
			this->break_flag_left=true;
		}
	

		//read in the appending tables
		//boundary data
		this->input_boundary_data_per_database(ptr_database, this->glob_prof_id);
		//bridge data
		this->input_bridge_data_per_database(ptr_database, this->glob_prof_id);
		//dikebreak data
		this->input_dikebreak_data_per_database(ptr_database, this->glob_prof_id);
		//wallbreak data
		this->input_wallbreak_data_per_database(ptr_database, this->glob_prof_id);

		//profile point data
		this->input_profile_points_per_database(ptr_database, this->glob_prof_id);

	}
	catch(Error msg){
		ostringstream info;
		info << "Profile number            : " << this->profile_number << endl;
		info << "Profile name              : " << this->name << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
	this->check_set_base_points();
	
}
//Input the river profile boundary data from a database
void Hyd_River_Profile_Connection_Standard::input_boundary_data_per_database(QSqlDatabase *ptr_database, const int glob_prof_id){
	//mysql query with the table_model
	QSqlTableModel model(0,*ptr_database);
	//the table is set (the name and the column names) and allocated
	try{
		_Hyd_River_Profile::set_profile_boundary_table(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}

	///give the table name to the query model
	model.setTable((_Hyd_River_Profile::boundary_table->get_table_name()).c_str());

	//lateral conditions
	this->input_lateral_boundary_per_database(glob_prof_id, &model);
	//point_conditions
	this->input_point_boundary_per_database(glob_prof_id, &model);
}
//Get if an overflow over the left river bank into the floodplain is given
bool Hyd_River_Profile_Connection_Standard::get_overflow_flag_left(void){
	return this->overflow_left_flag;
}
//Get the poleni-factor for an overflow over the left river bank into the floodplain 
double Hyd_River_Profile_Connection_Standard::get_overflow_poleni_left(void){
	return this->poleni_factor_left_overflow;
}
//Get if an overflow over the right river bank into the floodplain is given
bool Hyd_River_Profile_Connection_Standard::get_overflow_flag_right(void){
	return this->overflow_right_flag;
}
//Get the poleni-factor for an overflow over the right river bank into the floodplain 
double Hyd_River_Profile_Connection_Standard::get_overflow_poleni_right(void){
	return this->poleni_factor_right_overflow;
}
//Set the profile to no overflow (left and right riverbank)
void Hyd_River_Profile_Connection_Standard::set_left_right_bank2no_overflow(void){
	this->overflow_left_flag=false;
	this->overflow_right_flag=false;
}
//Check the profiles
void Hyd_River_Profile_Connection_Standard::check_profiles(_Hyd_River_Profile *upstream_profile, _Hyd_River_Profile *downstream_profile){

	try{
		_Hyd_River_Profile::check_profiles();
	}
	catch(Error msg){
		throw msg;
	}

	//special type checks
	if(this->type!=_hyd_profile_types::RIVER_TYPE){
		//check if there are special profiles sequential
		if(upstream_profile->get_profile_type()!=_hyd_profile_types::RIVER_TYPE || downstream_profile->get_profile_type()!=_hyd_profile_types::RIVER_TYPE){
			Error msg=this->set_error(6);
			ostringstream info;
			info << "Profile "<<this->profile_number<<" Profilname " << this->name << endl;
			msg.make_second_info(info.str());
			throw msg;
		}
		if(this->type==_hyd_profile_types::WEIR_TYPE){
			//check if the global z coordinate of the profile before is under the weir crest
			if(upstream_profile->typ_of_profile->get_global_z_min()>this->typ_of_profile->get_global_z_min()){
				Error msg=this->set_error(7);
				ostringstream info;
				info << "Profile "<<this->profile_number<<" Profilname " << this->name << endl;
				msg.make_second_info(info.str());
				throw msg;
			}
			//check if the global z coordinate of the profile after is under the weir crest
			if(downstream_profile->typ_of_profile->get_global_z_min()>this->typ_of_profile->get_global_z_min()){
				Error msg=this->set_error(8);
				ostringstream info;
				info << "Profile "<<this->profile_number<<" Profilname " << this->name << endl;
				msg.make_second_info(info.str());
				throw msg;
			}
		}
		if(this->type==_hyd_profile_types::BRIDGE_TYPE){
			//check if the global z coordinate of the profile before is not greater than the global z_min + 0.5 times the min height of the bridge 
			if(upstream_profile->typ_of_profile->get_global_z_min()-this->typ_of_profile->get_global_z_min()>(constant::min_height_bridge*0.5)){
				Error msg=this->set_error(9);
				ostringstream info;
				info << "Profile "<<this->profile_number<<" Profilname " << this->name << endl;
				msg.make_second_info(info.str());
				throw msg;
			}
			//check if the global z coordinate of the profile after is not greater than the global z_min + 0.5 times the min height of the bridge 
			if(downstream_profile->typ_of_profile->get_global_z_min()-this->typ_of_profile->get_global_z_min()>(constant::min_height_bridge*0.5)){
				Error msg=this->set_error(11);
				ostringstream info;
				info << "Profile "<<this->profile_number<<" Profilname " << this->name << endl;
				msg.make_second_info(info.str());
				throw msg;
			}
		}


	}
	//check poleni left
	if(this->poleni_factor_left_overflow<0.35 ||this->poleni_factor_left_overflow>1.0){
		Warning msg=this->set_warning(5);
		ostringstream info;
		info << "Left poleni factor"<<endl;
		info << "Given poleni factor : "<< this->poleni_factor_left_overflow<<label::no_unit<<endl;
		info << "New poleni factor   : "<< constant::poleni_const<<label::no_unit<<endl;
		info << "Profile "<<this->profile_number<<" Profilname " << this->name << endl;
		msg.make_second_info(info.str());
		msg.output_msg(2);
		this->poleni_factor_left_overflow=constant::poleni_const;

	}
	//check poleni right
	if(this->poleni_factor_right_overflow<0.35 || this->poleni_factor_right_overflow>1.0 ){
		Warning msg=this->set_warning(5);
		ostringstream info;
		info << "Right poleni factor"<<endl;
		info << "Given poleni factor : "<< this->poleni_factor_right_overflow<<label::no_unit<<endl;
		info << "New poleni factor   : "<< constant::poleni_const<<label::no_unit<<endl;
		info << "Profile "<<this->profile_number<<" Profilname " << this->name << endl;
		msg.make_second_info(info.str());
		msg.output_msg(2);
		this->poleni_factor_right_overflow=constant::poleni_const;

	}
	//check break parameters
	if(this->break_flag_left==true){
		if(this->left_break_params->resistance<0.0){
			Error msg=this->set_error(10);
			ostringstream info;
			info << "Resistance factor of the left break-parameter is: " << this->left_break_params->resistance <<endl;
			info << "Profile "<<this->profile_number<<" Profilname " << this->name << endl;
			msg.make_second_info(info.str());
			throw msg;
		}
		if(this->left_break_params->abrupt_opening<0.0){
			Error msg=this->set_error(10);
			ostringstream info;
			info << "Abrupt opening width of the left break-parameter is: " << this->left_break_params->abrupt_opening << label::m <<endl;
			info << "Profile "<<this->profile_number<<" Profilname " << this->name << endl;
			msg.make_second_info(info.str());
			throw msg;

		}

	}
	if(this->break_flag_right==true){
		if(this->right_break_params->resistance<0.0){
			Error msg=this->set_error(10);
			ostringstream info;
			info << "Resistance factor of the right break-parameter is: " << this->right_break_params->resistance <<endl;
			info << "Profile "<<this->profile_number<<" Profilname " << this->name << endl;
			msg.make_second_info(info.str());
			throw msg;

		}
		if(this->right_break_params->abrupt_opening<0.0){
			Error msg=this->set_error(10);
			ostringstream info;
			info << "Abrupt opening width of the right break-parameter is: " << this->right_break_params->abrupt_opening << label::m <<endl;
			info << "Profile "<<this->profile_number<<" Profilname " << this->name << endl;
			msg.make_second_info(info.str());
			throw msg;
		}
	}
}
//Output the members
void Hyd_River_Profile_Connection_Standard::output_members(void){
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
	cout << " BOUNDARY"<< endl; 
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
			cout <<" Dikebreak; critical velocity                    : " << this->left_break_params->resistance<< label::m_per_sec << endl;
		}
		else{
			cout <<" Wallbreak; resistance                           : " << this->left_break_params->resistance<< label::stress_unit <<endl;
			cout <<" Abrupt opening                                  : " <<this->left_break_params->abrupt_opening << label::m << endl;
		}
	}

	if(this->break_flag_right==false){
		cout << " No breaking of the right bank" <<endl;
	}
	else{
		cout << " Right base point index                        : " << this->index_basepoint_right <<endl;
		cout << " Global z                                      : "<< this->typ_of_profile->points[this->index_basepoint_right].get_global_z_coordinate() << label::m <<endl;
		if(this->right_break_params->abrupt_fails_flag==false){
			cout <<" Dikebreak; critical velocity                    : " << this->right_break_params->resistance<< label::m_per_sec << endl;
		}
		else{
			cout <<" Wallbreak; resistance                           : " << this->right_break_params->resistance<< label::stress_unit <<endl;
			cout <<" Abrupt opening                                  : " <<this->right_break_params->abrupt_opening << label::m << endl;
		}
	}
	cout <<"POINTS"<< endl;
	this->typ_of_profile->output_members(&cout);

	Sys_Common_Output::output_hyd->output_txt(&cout,true);
	Sys_Common_Output::output_hyd->rewind_userprefix();

}
//Output the setted and calculated members
void Hyd_River_Profile_Connection_Standard::output_setted_members(void){
	//set the prefix
	ostringstream prefix;
	prefix << "PROF_"<< this->profile_number<<"> ";
	Sys_Common_Output::output_hyd->set_userprefix(prefix.str());

	ostringstream cout;
	cout << "Distance to the upstream profile " << P(2) << FORMAT_FIXED_REAL << this->distance2upstream << label::m << endl;
	Sys_Common_Output::output_hyd->output_txt(&cout,true);
	if(this->boundary_cond_point_flag==true && this->boundary_point_stat_flag==false){
		cout << "Pointwise boundary curve is found: " << functions::convert_boolean2string(this->instatbound_point_found_flag)<<" No. " << *this->number_boundary_point_curve<<endl; ;
		Sys_Common_Output::output_hyd->output_txt(&cout,true);
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
void Hyd_River_Profile_Connection_Standard::clone_profile(Hyd_River_Profile_Connection_Standard *profile){
	_Hyd_River_Profile::clone_profile(profile);

	//lateral boundary condition
	this->boundary_cond_lateral_flag=profile->boundary_cond_lateral_flag;
	this->boundary_lateral_stat_flag=profile->boundary_lateral_stat_flag;
	if(profile->value_boundary_lateral_stat!=NULL){
		this->value_boundary_lateral_stat=new double;
		*this->value_boundary_lateral_stat=*profile->value_boundary_lateral_stat;
	}
	if(profile->number_boundary_lateral_curve!=NULL){
		this->number_boundary_lateral_curve=new int;
		*this->number_boundary_lateral_curve=*profile->number_boundary_lateral_curve;
	} 

	//point boundary
	this->boundary_cond_point_flag=profile->boundary_cond_point_flag;
	this->boundary_point_stat_flag=profile->boundary_point_stat_flag;
	if(profile->value_boundary_point_stat!=NULL){
		this->value_boundary_point_stat=new double;
		*this->value_boundary_point_stat=*profile->value_boundary_point_stat;
	}
	if(profile->number_boundary_point_curve!=NULL){
		this->number_boundary_point_curve=new int;
		*this->number_boundary_point_curve=*profile->number_boundary_point_curve;
	}




}
//Get the flag if a lateral boundary condition is set
bool Hyd_River_Profile_Connection_Standard::get_boundary_lateral_flag(void){
	return this->boundary_cond_lateral_flag;
}
//Connect instationary boundary curve to the profiles (lateral inflow)
void Hyd_River_Profile_Connection_Standard::connect_instat_lateral_boundarycurve(Hyd_Instationary_Boundary *bound_curve){
	ostringstream cout;
	if(this->boundary_cond_lateral_flag==false){//no boundary
		return;
	}
	else if(this->boundary_cond_lateral_flag==true && this->boundary_lateral_stat_flag==true){//stat boundary
		return;
	}
	else if(this->boundary_cond_lateral_flag==true && this->boundary_lateral_stat_flag==false){//instat boundary
		if(*this->number_boundary_lateral_curve==bound_curve->get_curve_number()){//the right curve is found

			try{
				bound_curve->set_type(_hyd_bound_type::length_type);
			}
			catch(Error msg){
				throw msg;
			}
			this->ptr_boundarycurve_lateral=bound_curve;
			this->instatbound_lateral_found_flag=true;
			cout <<"Connection of Profile " << this->profile_number << " with lateral boundary curve " << bound_curve->get_curve_number() << " is found" << endl;
			Sys_Common_Output::output_hyd->output_txt(&cout);
		}

	}
}
//Get the instationary found flag: the curve is found for lateral inflow
bool Hyd_River_Profile_Connection_Standard::get_instat_lateral_boundarycurve_found(void){
	if(this->boundary_cond_lateral_flag==false){//no boundary
		return true;
	}
	else if(this->boundary_cond_lateral_flag==true && this->boundary_lateral_stat_flag==true){//stat boundary
		return true;
	}
	else if(this->boundary_cond_lateral_flag==true && this->boundary_lateral_stat_flag==false){//instat boundary
		if(this->instatbound_lateral_found_flag==true){//it is found
			return true;
		}
		else{
			return false;
		}
	}
	else{
		return false;
	}
}
//Connect instationary boundary curve to the profiles (pointwise inflow)
void Hyd_River_Profile_Connection_Standard::connect_instat_point_boundarycurve(Hyd_Instationary_Boundary *bound_curve){
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
			if(this->connection==_hyd_connection_types::INFLOW_CONN){
				try{
					this->ptr_boundarycurve_point->check_boundary_curve_neg_null_values();
					this->ptr_boundarycurve_point->check_starting_time(0.0);
				}
				catch(Error msg){
					throw msg;
				}

			}
		}
	}
}
//Get the flag if a point boundary condition is set
bool Hyd_River_Profile_Connection_Standard::get_boundary_point_flag(void){
	return this->boundary_cond_point_flag;
}
//Get the instationary found flag: the curve is found for pointwise inflow
bool Hyd_River_Profile_Connection_Standard::get_instat_point_boundarycurve_found(void){
	if(this->boundary_cond_point_flag==false){//no boundary
		return true;
	}
	else if(this->boundary_cond_point_flag==true && this->boundary_point_stat_flag==true){//stat boundary
		return true;
	}
	else if(this->boundary_cond_point_flag==true && this->boundary_point_stat_flag==false){//instat boundary
		if(this->instatbound_point_found_flag==true){//it is found
			return true;
		}
		else{
			return false;
		}
	}
	else{
		return false;
	}
}
//Set the actual river discharge through this profile (q_river) by the given upstream and downstream profiles
void Hyd_River_Profile_Connection_Standard::set_actual_river_discharge(_Hyd_River_Profile *upstream_profile, _Hyd_River_Profile *downstream_profile){


	if(upstream_profile->get_profile_type()==_hyd_profile_types::RIVER_TYPE || upstream_profile->get_profile_type()==_hyd_profile_types::BRIDGE_TYPE){
		this->q_river=this->typ_of_profile->calculate_actual_discharge(upstream_profile, downstream_profile, this->distance2upstream);
	}
	else if(upstream_profile->get_profile_type()==_hyd_profile_types::WEIR_TYPE){
		//cumulate the discharge from the segment between weir profile and this profile; here the solved area is not relevant
		this->q_river=upstream_profile->get_actual_river_discharge();
		this->typ_of_profile->set_actuel_flowvelocity(this->q_river);

	}

}
//Get the actual discharge through this profile
double Hyd_River_Profile_Connection_Standard::get_actual_river_discharge(void){
	return this->q_river;
}
//Reset all coupling discharges; use it before syncronization
void Hyd_River_Profile_Connection_Standard::reset_coupling_discharge(void){
	_Hyd_River_Profile::reset_coupling_discharge();
	//this->q_river=0.0;
	this->q_lateral=0.0;
	this->q_point=0.0;
	this->q_zero_outflow_error=0.0;
}
//Make the syncronistation of the profiles to a given time point
void Hyd_River_Profile_Connection_Standard::make_syncronisation(const double time_point, _Hyd_River_Profile *downstream){
	this->set_actual_lateral_discharge(time_point);
	this->set_actual_point_discharge(time_point);
	this->calculate_max_h2basepoint();
	this->typ_of_profile->calculate_velocity_energy_head(time_point, downstream);
}
//Get the maximum change in a river section
void Hyd_River_Profile_Connection_Standard::get_max_changes_section(double *max_change_h, double *max_change_v, const bool timecheck){

	if(this->typ_of_profile!=NULL && this->type!=_hyd_profile_types::WEIR_TYPE){

		//max h change
		
		this->ds_dt_coup=this->q_lateral+this->q_point+this->q_1d_coupling+
			this->q_dikebreak_coupling_left+this->q_dikebreak_coupling_right+
			this->q_left_bank+this->q_right_bank+this->q_structure_coupling;

		this->ds_dt_coup=this->ds_dt_coup/((this->get_distance2downstream()+this->get_distance2upstream())*0.5*this->typ_of_profile->get_area2calc()/this->typ_of_profile->get_actual_local_waterlevel_h());


		*max_change_h=abs(this->ds_dt_coup_old-this->ds_dt_coup);
		if(timecheck==false){
			this->ds_dt_coup_old=this->ds_dt_coup;
		}
		

		//max v change

	}
}
//Calculate the values for a hydrological balance for each syncronisation step
void Hyd_River_Profile_Connection_Standard::calculate_hydrological_balance(const double time_point){
	this->typ_of_profile->calculate_maximum_values(time_point);

	//calculate the maximum discharge
	if(this->q_river>this->q_value_max.maximum){
		this->q_value_max.maximum=this->q_river;
		this->q_value_max.time_point=time_point;
	}
	this->typ_of_profile->set_old_time_point(time_point);

	_Hyd_River_Profile::calculate_hydrological_balance(time_point);
	//lateral boundary
	if(this->q_lateral<0.0){
		//outflow
		this->lateral_boundary_volume.volume_out=this->lateral_boundary_volume.volume_out+abs(this->q_lateral)*this->delta_time;
	}
	else if(this->q_lateral>0.0){
		//inflow
		this->lateral_boundary_volume.volume_in=this->lateral_boundary_volume.volume_in+abs(this->q_lateral)*this->delta_time;
	}
	//total
	this->lateral_boundary_volume.volume_total=this->lateral_boundary_volume.volume_in-this->lateral_boundary_volume.volume_out;

	//point boundary
	if(this->q_point<0.0){
		//outflow
		this->point_boundary_volume.volume_out=this->point_boundary_volume.volume_out+abs(this->q_point)*this->delta_time;
	}
	else if(this->q_point>0.0){
		//inflow
		this->point_boundary_volume.volume_in=this->point_boundary_volume.volume_in+abs(this->q_point)*this->delta_time;
	}
	//total
	this->point_boundary_volume.volume_total=this->point_boundary_volume.volume_in-this->point_boundary_volume.volume_out;

	

	//calculate the water volume
	this->calculate_watervolume_element();
}
//Get the total boundary and coupling discharge to this profile
double Hyd_River_Profile_Connection_Standard::get_total_boundary_coupling_discharge(const double time_point, const double q_river_in_out){
	double q_buff=0.0;
	this->q_zero_outflow_error=0.0;
////DEBUG!!!!!
//	if (this->name == "A_Elbe_8" || this->name == "A_Elbe_9") {
//
//		if (this->q_right_bank != 0.0) {
//			ostringstream cout;
//			cout << time_point << "   "<< this->name << "   " << this->q_right_bank << endl;
//			Sys_Common_Output::output_hyd->output_txt(&cout, true, true);
//		}
//
//	}

	if(this->typ_of_profile->get_outflow_reduction_flag()==false){
		q_buff=this->q_lateral+this->q_point+this->q_1d_coupling+
			this->q_dikebreak_coupling_left+this->q_dikebreak_coupling_right+
			this->q_left_bank+this->q_right_bank+this->q_structure_coupling+q_river_in_out;
	}
	else{
		if(this->q_lateral>=0.0){
			q_buff=q_buff+this->q_lateral;
		}
		else{
			this->q_zero_outflow_error=this->q_zero_outflow_error+this->q_lateral;
		}
		if(this->q_point>=0.0){
			q_buff=q_buff+this->q_point;
		}
		else{
			this->q_zero_outflow_error=this->q_zero_outflow_error+this->q_point;
		}
		if(this->q_1d_coupling>=0.0){
			q_buff=q_buff+this->q_1d_coupling;
		}
		else{
			this->q_zero_outflow_error=this->q_zero_outflow_error+this->q_1d_coupling;
		}

		if(this->q_dikebreak_coupling_left>=0.0){
			q_buff=q_buff+this->q_dikebreak_coupling_left;
		}
		else{
			this->q_zero_outflow_error=this->q_zero_outflow_error+this->q_dikebreak_coupling_left;
		}
		if(this->q_dikebreak_coupling_right>=0.0){
			q_buff=q_buff+this->q_dikebreak_coupling_right;
		}
		else{
			this->q_zero_outflow_error=this->q_zero_outflow_error+this->q_dikebreak_coupling_right;
		}
		if(this->q_left_bank>=0.0){
			q_buff=q_buff+this->q_left_bank;
		}
		else{
			this->q_zero_outflow_error=this->q_zero_outflow_error+this->q_left_bank;
		}
		if(this->q_right_bank>=0.0){
			q_buff=q_buff+this->q_right_bank;
		}
		else{
			this->q_zero_outflow_error=this->q_zero_outflow_error+this->q_right_bank;
		}
		if(this->q_structure_coupling>=0.0){
			q_buff=q_buff+this->q_structure_coupling;
		}
		else{
			this->q_zero_outflow_error=this->q_zero_outflow_error+this->q_structure_coupling;
		}

		q_buff=q_river_in_out+q_buff;

		//part error; some parts can by taken out
		if(q_buff>0.0 && abs(this->q_zero_outflow_error)>q_buff){
			this->q_zero_outflow_error=(this->q_zero_outflow_error+q_buff);
			q_buff=0.0;
		}
		//no error all can be taken out
		else if(q_buff>0.0 && abs(this->q_zero_outflow_error)<q_buff){
			q_buff=q_buff+this->q_zero_outflow_error;
			this->q_zero_outflow_error=0.0;
		}
		//full error is applied
		else{
			this->q_zero_outflow_error=this->q_zero_outflow_error;
		}
		

	}
	this->calculate_hydrolocigal_balance_error_zero_outflow(time_point);
	return q_buff;
}
//Get the actual lateral discharge (q_lateral)
double Hyd_River_Profile_Connection_Standard::get_actual_lateral_discharge(void){
	return this->q_lateral;
}
//Add a discharge value to the current lateral discharge
void Hyd_River_Profile_Connection_Standard::add_lateral_discharge(const double value){
	this->q_lateral=this->q_lateral+value;
}
//Get the actual point discharge (q_point)
double Hyd_River_Profile_Connection_Standard::get_actual_point_discharge(void){
	return this->q_point;
}
//Add a discharge value to the current point discharge
void Hyd_River_Profile_Connection_Standard::add_point_discharge(const double value){
	this->q_point=this->q_point+value;
}
//Add the hydrological balance value of the point boundary condition discharges to the given pointer
void Hyd_River_Profile_Connection_Standard::add_hydro_balance_point_boundary(_hyd_hydrological_balance *given){
	given->volume_in=given->volume_in+this->point_boundary_volume.volume_in;
	given->volume_out=given->volume_out+this->point_boundary_volume.volume_out;
	given->volume_total=given->volume_total+this->point_boundary_volume.volume_total;
}
//Add the hydrological balance value of the lateral boundary condition discharges to the given pointer
void Hyd_River_Profile_Connection_Standard::add_hydro_balance_lateral_boundary(_hyd_hydrological_balance *given){
	given->volume_in=given->volume_in+this->lateral_boundary_volume.volume_in;
	given->volume_out=given->volume_out+this->lateral_boundary_volume.volume_out;
	given->volume_total=given->volume_total+this->lateral_boundary_volume.volume_total;
}
//Add the total coupling/boundary discharges to the given downstream profile (needed e.g. for weir types)
void Hyd_River_Profile_Connection_Standard::transfer_coupling_boundary2profile(Hyd_River_Profile_Connection_Standard *profile2transfer){
	
	if(this->typ_of_profile!=NULL && this->type==_hyd_profile_types::WEIR_TYPE){
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
			profile2transfer->add_point_discharge(this->q_point);
			this->q_point=0.0;
		}
	}
}
//Get the error volume due to outflow by zero volume in the profile
double Hyd_River_Profile_Connection_Standard::get_outflow_zero_error_volume(void){
	return (-1.0)*this->error_zero_outflow_volume;
}
//Output the result members for each riverprofiletype at every timestep
void Hyd_River_Profile_Connection_Standard::output_result_members_per_timestep(void){
	ostringstream cout;
	_Hyd_River_Profile::output_result_members_per_timestep(&cout);
	cout << W(10) <<  this->q_river;
	cout << endl;
	Sys_Common_Output::output_hyd->output_txt(&cout,true);
}
//Output the result members for each riverprofiletype at every timestep to tecplot file
void Hyd_River_Profile_Connection_Standard::output_result_members_per_timestep(ofstream *file){
		_Hyd_River_Profile::output_result_members_per_timestep(file);
		*file  << W(12) << P(4) <<FORMAT_FIXED_REAL << this->q_river << endl;
}
///Output the result members for each riverprofiletype at every timestep to csv file
void Hyd_River_Profile_Connection_Standard::output_result_members_per_timestep2csv(ofstream *file) {
	_Hyd_River_Profile::output_result_members_per_timestep2csv(file);
	*file << W(12) << P(4) << FORMAT_FIXED_REAL << this->q_river << endl;
}
//Set new break-parameters of the left bank; the structure must be already allocated
void Hyd_River_Profile_Connection_Standard::set_break_parameter_left(_hyd_break_parameters* new_values){
	if(this->break_flag_left==true){
		if(this->left_break_params!=NULL){
			this->left_break_params->abrupt_opening=new_values->abrupt_opening;
			this->left_break_params->resistance=new_values->resistance;
		}
	}

}
//Set new break-parameters of the right bank; the structure must be already allocated
void Hyd_River_Profile_Connection_Standard::set_break_parameter_right(_hyd_break_parameters* new_values){
	if(this->break_flag_right==true){
		if(this->right_break_params!=NULL){
			this->right_break_params->abrupt_opening=new_values->abrupt_opening;
			this->right_break_params->resistance=new_values->resistance;
		}
	}
}
//Reset the hydrological balance value and the maximum values
void Hyd_River_Profile_Connection_Standard::reset_hydrobalance_maxvalues(void){
	_Hyd_River_Profile::reset_hydrobalance_maxvalues();
	//discharges per timestep
	this->q_river=0.0;
	this->q_lateral=0.0;
	this->q_point=0.0;

	this->ds_dt_coup_old=0.0;
	this->ds_dt_coup=0.0;


	//hydrological balance values
	this->lateral_boundary_volume.volume_in=0.0;
	this->lateral_boundary_volume.volume_out=0.0;
	this->lateral_boundary_volume.volume_total=0.0;
	this->point_boundary_volume.volume_in=0.0;
	this->point_boundary_volume.volume_out=0.0;
	this->point_boundary_volume.volume_total=0.0;

	
	this->error_zero_outflow_volume=0.0;
	this->q_zero_outflow_error=0.0;
	this->old_timepoint_error=0.0;
	this->old_delta_t_error=0.0;
	this->old_discharge_error=0.0;
	this->old_discharge_2_error=0.0;

}
//Reset the boundary data of the river profile
void Hyd_River_Profile_Connection_Standard::reset_boundary_data(void){
	//lateral boundary condition
	this->boundary_cond_lateral_flag=false;
	this->boundary_lateral_stat_flag=false;
	this->value_boundary_lateral_stat=NULL;
	this->number_boundary_lateral_curve=NULL;
	this->ptr_boundarycurve_lateral=NULL;
	this->instatbound_lateral_found_flag=false; 

	//point boundary
	this->boundary_cond_point_flag=false;
	this->boundary_point_stat_flag=false;
	this->value_boundary_point_stat=NULL;
	this->number_boundary_point_curve=NULL;
	this->ptr_boundarycurve_point=NULL;
	this->instatbound_point_found_flag=false;
	this->must_found_number=0;
	

}
//Get boundary condition is applied
bool Hyd_River_Profile_Connection_Standard::boundary_is_applied(void){
	if(this->boundary_cond_lateral_flag==true ||this->boundary_cond_point_flag==true){
		return true;
	}
	else{
		return false;
	}
}
//Get boundary point condition is applied
bool Hyd_River_Profile_Connection_Standard::boundary_point_is_applied(void) {
	return this->boundary_cond_point_flag;

}
//Get boundary point value
double Hyd_River_Profile_Connection_Standard::get_boundary_point_value(const double time) {
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
double Hyd_River_Profile_Connection_Standard::get_Q(void) {
	return this->q_river;
}
//___________________
//protected
//decide in the different keyvalues in the profile file
void Hyd_River_Profile_Connection_Standard::decide_keyvalues_file(const string key, string buffer, int *found_counter){
	try{
		_Hyd_River_Profile::decide_keyvalues_file(key, buffer, found_counter); 

	}catch(Error msg){
		throw msg;
	}
	
	stringstream stream;


	if(key==hyd_label::BoundaryLateralCondition){
		this->get_keyvalues_file(&stream, buffer);
		this->boundary_cond_lateral_flag=_Hyd_Parse_IO::transform_string2boolean(stream.str());
		if(this->boundary_cond_lateral_flag==true){
			this->must_found_number=this->must_found_number+2;
		}
	}
	else if(key==hyd_label::BoundaryLateralStationary && this->boundary_cond_lateral_flag==true){
		this->get_keyvalues_file(&stream, buffer);
		this->boundary_lateral_stat_flag=_Hyd_Parse_IO::transform_string2boolean(stream.str());
		(*found_counter)++;
	}
	else if(key==hyd_label::BoundaryLateralValue && this->boundary_cond_lateral_flag==true){
		this->get_keyvalues_file(&stream, buffer);
		double value;
		stream >> value;
		this->decide_boundary_lateral_conditions(value);
		(*found_counter)++;
	}
	//point boundary
	else if(key==hyd_label::BoundaryPointCondition){
		this->get_keyvalues_file(&stream, buffer);
		this->boundary_cond_point_flag=_Hyd_Parse_IO::transform_string2boolean(stream.str());
		if(this->boundary_cond_point_flag==true){
			this->must_found_number=this->must_found_number+2;
		}			
	}
	else if(key==hyd_label::BoundaryPointStationary && this->boundary_cond_point_flag==true){
		this->get_keyvalues_file(&stream, buffer);
		this->boundary_point_stat_flag=_Hyd_Parse_IO::transform_string2boolean(stream.str());
		(*found_counter)++;
	}
	else if(key==hyd_label::BoundaryPointValue && this->boundary_cond_point_flag==true){
		this->get_keyvalues_file(&stream, buffer);
		double value;
		stream >> value;
		this->decide_boundary_point_conditions(value);
		(*found_counter)++;
	}
	else if(key==hyd_label::OverflowCouplingLeft){
		this->get_keyvalues_file(&stream, buffer);
		this->overflow_left_flag=_Hyd_Parse_IO::transform_string2boolean(stream.str());
	}
	else if(key==hyd_label::PoleniFacLeft && this->overflow_left_flag==true){
		this->get_keyvalues_file(&stream, buffer);
		stream >> this->poleni_factor_left_overflow;
	}
	else if(key==hyd_label::OverflowCouplingRight){
		this->get_keyvalues_file(&stream, buffer);
		this->overflow_right_flag=_Hyd_Parse_IO::transform_string2boolean(stream.str());
	}
	else if(key==hyd_label::PoleniFacRight && this->overflow_right_flag==true){
		this->get_keyvalues_file(&stream, buffer);
		stream >> this->poleni_factor_right_overflow;
	}
	else if(key==hyd_label::CritVelocityLeft && this->break_flag_left==true){
		this->allocate_break_parameter_left();
		this->get_keyvalues_file(&stream, buffer);
		stream >> this->left_break_params->resistance;
		this->left_break_params->abrupt_fails_flag=false;
		(*found_counter)++;
	}
	else if(key==hyd_label::CritVelocityRight && this->break_flag_right==true){
		this->allocate_break_parameter_right();
		this->get_keyvalues_file(&stream, buffer);
		stream >> this->right_break_params->resistance;
		this->right_break_params->abrupt_fails_flag=false;
		(*found_counter)++;
	}
	else if(key==hyd_label::WallResistanceRight && this->break_flag_right==true){
		if(this->right_break_params==NULL){
			this->allocate_break_parameter_right();
			
		}
		
		this->get_keyvalues_file(&stream, buffer);
		stream >> this->right_break_params->resistance;
		if(this->right_break_params->abrupt_fails_flag==false){
			this->must_found_number++;
		}
		this->right_break_params->abrupt_fails_flag=true;
		(*found_counter)++;
	}
	else if(key==hyd_label::WallResistanceLeft && this->break_flag_left==true){
		if(this->left_break_params==NULL){
			this->allocate_break_parameter_left();
			
		}
		this->get_keyvalues_file(&stream, buffer);
		stream >> this->left_break_params->resistance;
		if(this->left_break_params->abrupt_fails_flag==false){
			this->must_found_number++;
		}
		this->left_break_params->abrupt_fails_flag=true;
		(*found_counter)++;
	}
	else if(key==hyd_label::AbruptBreachOpenRight && this->break_flag_right==true){
		if(this->right_break_params==NULL){
			this->allocate_break_parameter_right();
			
		}
		this->get_keyvalues_file(&stream, buffer);
		stream >> this->right_break_params->abrupt_opening;
		if(this->right_break_params->abrupt_fails_flag==false){
			this->must_found_number++;
		}
		this->right_break_params->abrupt_fails_flag=true;
		(*found_counter)++;
		
	}
	else if(key==hyd_label::AbruptBreachOpenLeft && this->break_flag_left==true){
		if(this->left_break_params==NULL){
			this->allocate_break_parameter_left();
			
		}
		this->get_keyvalues_file(&stream, buffer);
		stream >> this->left_break_params->abrupt_opening;
		if(this->left_break_params->abrupt_fails_flag==false){
			this->must_found_number++;
		}
		this->right_break_params->abrupt_fails_flag=true;
		(*found_counter)++;
	}

	if(stream.fail()==true){
		Error msg=this->set_error(1);
		ostringstream info;
		info << "Searched key word " << key << endl;
		info << "Wrong input sequenze " << stream.str() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
}
//decide which boundary conditions are choosen for pointwise inflow
void Hyd_River_Profile_Connection_Standard::decide_boundary_point_conditions(const double nr_value){
	if(this->boundary_cond_point_flag==false){//no boundary condition
		return;
	}
	else if(this->boundary_cond_point_flag==true && this->boundary_point_stat_flag==true){
		this->value_boundary_point_stat=new double;
		*this->value_boundary_point_stat=nr_value;
	}
	else if(this->boundary_cond_point_flag==true && this->boundary_point_stat_flag==false){
		this->number_boundary_point_curve=new int;
		*this->number_boundary_point_curve=(int)nr_value;
	}
}
//decide which boundary conditions are choosen for lateral inflow
void Hyd_River_Profile_Connection_Standard::decide_boundary_lateral_conditions(const double nr_value){
	if(this->boundary_cond_lateral_flag==false){//no boundary condition
		return;
	}
	else if(this->boundary_cond_lateral_flag==true && this->boundary_lateral_stat_flag==true){
		this->value_boundary_lateral_stat=new double;
		*this->value_boundary_lateral_stat=nr_value;
	}
	else if(this->boundary_cond_lateral_flag==true && this->boundary_lateral_stat_flag==false){
		this->number_boundary_lateral_curve=new int;
		*this->number_boundary_lateral_curve=(int)nr_value;
	}
}
//Set the actual lateral discharge (q_lateral) correspondig to to the stationary/instationary flag and the point in time
void Hyd_River_Profile_Connection_Standard::set_actual_lateral_discharge(const double time_point){

	if(this->boundary_cond_lateral_flag==false){
		this->q_lateral=0.0;
		return;
	}
	else{
		//stationary
		if(this->boundary_lateral_stat_flag==true){
			this->q_lateral=*this->value_boundary_lateral_stat*0.5*(this->distance2downstream+this->distance2upstream);
		}
		//instationary
		else if(this->boundary_lateral_stat_flag==false){
			this->ptr_boundarycurve_lateral->set_specific_value(0.5*(this->distance2downstream+this->distance2upstream));
			this->q_lateral=this->ptr_boundarycurve_lateral->calculate_actuel_boundary_value(time_point);
		}
	}
}
//Set the actual point discharge (q_point) correspondig to to the stationary/instationary flag and the point in time
void Hyd_River_Profile_Connection_Standard::set_actual_point_discharge(const double time_point){
	
	if(this->boundary_cond_point_flag==false){
		this->q_point=0.0;
		return;
	}
	else{
		//stationary
		if(this->boundary_point_stat_flag==true){
			this->q_point=*this->value_boundary_point_stat;
		}
		//instationary
		else if(this->boundary_point_stat_flag==false){
			this->q_point=this->ptr_boundarycurve_point->calculate_actuel_boundary_value(time_point);
		}
	}

}
//check the connection profile type
void Hyd_River_Profile_Connection_Standard::check_connection_type_profile(void){

	if(this->connection!=_hyd_connection_types::STANDARD_CONN){
		Error msg=this->set_error(2);
		throw msg;
	}

}
//Calculate the watervolume in the element
void Hyd_River_Profile_Connection_Standard::calculate_watervolume_element(void){
	if(this->type!=_hyd_profile_types::WEIR_TYPE){
		this->watervolume=this->typ_of_profile->get_area2calc()*(this->distance2downstream+this->distance2upstream)*0.5;
	}
	//the watervolume after a weir is numerically not existing; 
	//the section is not calculated, the discharge is given directly to the next section behind the weir/bridge section
	else{
		this->watervolume=0.0;
	}
}
//Calculate the hydrological balance error, if the profile volume is zero, but there is still an outflow set
void Hyd_River_Profile_Connection_Standard::calculate_hydrolocigal_balance_error_zero_outflow(const double time_point){
	double delta_time=time_point-this->old_timepoint_error;
	if(abs(delta_time)<constant::sec_epsilon){
		return;
	}


	if(delta_time>=0.0){
		this->old_timepoint_error=time_point;
		this->error_zero_outflow_volume=this->error_zero_outflow_volume+(this->old_discharge_error*(1.0/3.0)+this->q_zero_outflow_error*(2.0/3.0))*delta_time;
		this->old_delta_t_error=delta_time;
		this->old_discharge_2_error=this->old_discharge_error;
		this->old_discharge_error=(this->old_discharge_error*(1.0/3.0)+this->q_zero_outflow_error*(2.0/3.0));
	}
	else{
		this->error_zero_outflow_volume=this->error_zero_outflow_volume-this->old_discharge_error*this->old_delta_t_error;
		this->old_timepoint_error=this->old_timepoint_error-this->old_delta_t_error;
		delta_time=time_point-this->old_timepoint_error;
		this->error_zero_outflow_volume=this->error_zero_outflow_volume+(this->old_discharge_2_error*(1.0/3.0)+this->q_zero_outflow_error*(2.0/3.0))*delta_time;
		this->old_delta_t_error=delta_time;
		this->old_discharge_error=(this->old_discharge_2_error*(1.0/3.0)+this->q_zero_outflow_error*(2.0/3.0));
		this->old_timepoint_error=time_point;
	}
}
//___________________
//private
//Transfer the river profile boundary data to a database
void Hyd_River_Profile_Connection_Standard::transfer_boundary_data2database(QSqlDatabase *ptr_database, const int glob_prof_id){
	if(this->boundary_cond_lateral_flag==false && this->boundary_cond_point_flag==false){
		return;
	}

	//the table is set (the name and the column names) and allocated
	try{
		_Hyd_River_Profile::set_profile_boundary_table(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}

	//insert lateral boundary condition
	if(this->boundary_cond_lateral_flag==true){
		//insert lateral
		QSqlQueryModel model;
		//set the query via a query string
		ostringstream query_string;
		query_string << "INSERT INTO  " << _Hyd_River_Profile::boundary_table->get_table_name();
		query_string <<" ( ";
		query_string << _Hyd_River_Profile::boundary_table->get_column_name(hyd_label::profdata_glob_id) <<" , ";
		query_string << _Hyd_River_Profile::boundary_table->get_column_name(label::areastate_id) <<" , ";
		query_string << _Hyd_River_Profile::boundary_table->get_column_name(label::measure_id) <<" , ";
		query_string << _Hyd_River_Profile::boundary_table->get_column_name(label::applied_flag) <<" , ";
		query_string << _Hyd_River_Profile::boundary_table->get_column_name(hyd_label::sz_bound_id)<<" , ";
		query_string << _Hyd_River_Profile::boundary_table->get_column_name(hyd_label::bounddata_stat) <<" , ";
		query_string << _Hyd_River_Profile::boundary_table->get_column_name(hyd_label::bounddata_value) <<" , ";
		query_string << _Hyd_River_Profile::boundary_table->get_column_name(hyd_label::bounddata_type) <<" ) ";

		
		query_string << " VALUES ( ";
		query_string << glob_prof_id << " , " ;
		query_string << this->system_id.area_state << " , " ;
		query_string << this->system_id.measure_nr << " , " ;
		query_string << functions::convert_boolean2string(true)<< " , " ;
		query_string << this->hyd_sz.get_id() << " , " ;
		query_string << functions::convert_boolean2string(this->boundary_lateral_stat_flag)<< " , " ;
		if(this->boundary_lateral_stat_flag==true){
			query_string << *this->value_boundary_lateral_stat<< " , " ;
		}
		else{
			query_string << *this->number_boundary_lateral_curve<< " , " ;
		}
		query_string <<" '"<< Hyd_Instationary_Boundary::transfrom_instatboundtype2txt(_hyd_bound_type::length_type).c_str()<< "')" ;
		


		Data_Base::database_request(&model, query_string.str(), ptr_database);
		
		if(model.lastError().isValid()){
			Warning msg=this->set_warning(0);
			ostringstream info;
			info << "Table Name                : " << _Hyd_River_Profile::boundary_table->get_table_name() << endl;
			info << "Table error info          : " << model.lastError().text().toStdString() << endl;
			info << "Profile number            : " << this->profile_number << endl;
			info << "Profile name              : " << this->name << endl;
			info  <<"Trying to insert the lateral boundary condition" << endl;
			msg.make_second_info(info.str());
			msg.output_msg(2);		
		}
	}
	else if(this->boundary_cond_point_flag==true){
		//insert lateral
		QSqlQueryModel model;
		//set the query via a query string
		ostringstream query_string;
		query_string << "INSERT INTO  " << _Hyd_River_Profile::boundary_table->get_table_name();
		query_string <<" ( ";
		query_string << _Hyd_River_Profile::boundary_table->get_column_name(hyd_label::profdata_glob_id) <<" , ";
		query_string << _Hyd_River_Profile::boundary_table->get_column_name(label::areastate_id) <<" , ";
		query_string << _Hyd_River_Profile::boundary_table->get_column_name(label::measure_id) <<" , ";
		query_string << _Hyd_River_Profile::boundary_table->get_column_name(label::applied_flag) <<" , ";
		query_string << _Hyd_River_Profile::boundary_table->get_column_name(hyd_label::sz_bound_id)<<" , ";
		query_string << _Hyd_River_Profile::boundary_table->get_column_name(hyd_label::bounddata_stat) <<" , ";
		query_string << _Hyd_River_Profile::boundary_table->get_column_name(hyd_label::bounddata_value) <<" , ";
		query_string << _Hyd_River_Profile::boundary_table->get_column_name(hyd_label::bounddata_type) <<" ) ";

		
		query_string << " VALUES ( ";
		query_string << glob_prof_id << " , " ;
		query_string << this->system_id.area_state << " , " ;
		query_string << this->system_id.measure_nr << " , " ;
		query_string << functions::convert_boolean2string(true)<< " , " ;
		query_string << this->hyd_sz.get_id() << " , " ;
		query_string << functions::convert_boolean2string(this->boundary_point_stat_flag)<< " , " ;
		if(this->boundary_point_stat_flag==true){
			query_string << *this->value_boundary_point_stat<< " , " ;
		}
		else{
			query_string << *this->number_boundary_point_curve<< " , " ;
		}
		query_string <<" '"<< Hyd_Instationary_Boundary::transfrom_instatboundtype2txt(_hyd_bound_type::point_type).c_str()<< "')" ;
		


		Data_Base::database_request(&model, query_string.str(), ptr_database);
		
		if(model.lastError().isValid()){
			Warning msg=this->set_warning(0);
			ostringstream info;
			info << "Table Name                : " << _Hyd_River_Profile::boundary_table->get_table_name() << endl;
			info << "Table error info          : " << model.lastError().text().toStdString() << endl;
			info << "Profile number            : " << this->profile_number << endl;
			info << "Profile name              : " << this->name << endl;
			info  <<"Trying to insert the point boundary condition" << endl;
			msg.make_second_info(info.str());
			msg.output_msg(2);		
		}

	}





	////mysql query with the table_model
	//QSqlTableModel model(0,*ptr_database);

	////the table is set (the name and the column names) and allocated
	//try{
	//	_Hyd_River_Profile::set_profile_boundary_table(ptr_database);
	//}
	//catch(Error msg){
	//	throw msg;
	//}
	//

	////give the table name to the query model
	//model.setTable((_Hyd_River_Profile::boundary_table->get_table_name()).c_str());
	////insert lateral boundary condition
	//if(this->boundary_cond_lateral_flag==true){
	//	//insert row
	//	model.insertRows(0, 1);
	//						
	//	//insert data
	//	model.setData(model.index(0,model.record().indexOf((_Hyd_River_Profile::boundary_table->get_column_name(hyd_label::profdata_glob_id)).c_str())),glob_prof_id);
	//	model.setData(model.index(0,model.record().indexOf((_Hyd_River_Profile::boundary_table->get_column_name(label::areastate_id)).c_str())), this->system_id.area_state);
	//	model.setData(model.index(0,model.record().indexOf((_Hyd_River_Profile::boundary_table->get_column_name(label::measure_id)).c_str())), this->system_id.measure_nr);
	//	model.setData(model.index(0,model.record().indexOf((_Hyd_River_Profile::boundary_table->get_column_name(label::applied_flag)).c_str())), true);
	//	model.setData(model.index(0,model.record().indexOf((_Hyd_River_Profile::boundary_table->get_column_name(hyd_label::sz_bound_id)).c_str())), this->hyd_sz.get_id());
	//	model.setData(model.index(0,model.record().indexOf((_Hyd_River_Profile::boundary_table->get_column_name(hyd_label::bounddata_stat)).c_str())), this->boundary_lateral_stat_flag);
	//	if(this->boundary_lateral_stat_flag==true){
	//		model.setData(model.index(0,model.record().indexOf((_Hyd_River_Profile::boundary_table->get_column_name(hyd_label::bounddata_value)).c_str())), *this->value_boundary_lateral_stat);
	//	}
	//	else{
	//		model.setData(model.index(0,model.record().indexOf((_Hyd_River_Profile::boundary_table->get_column_name(hyd_label::bounddata_value)).c_str())), *this->number_boundary_lateral_curve);
	//	}	
	//	model.setData(model.index(0,model.record().indexOf((_Hyd_River_Profile::boundary_table->get_column_name(hyd_label::bounddata_type)).c_str())), Hyd_Instationary_Boundary::transfrom_instatboundtype2txt(_hyd_bound_type::length_type).c_str());

	//	//submit it to the datbase
	//	Data_Base::database_submit(&model);

	//	if(model.lastError().isValid()){
	//		Warning msg=this->set_warning(0);
	//		ostringstream info;
	//		info << "Table Name                : " << _Hyd_River_Profile::boundary_table->get_table_name() << endl;
	//		info << "Table error info          : " << model.lastError().text().toStdString() << endl;
	//		info << "Profile number            : " << this->profile_number << endl;
	//		info << "Profile name              : " << this->name << endl;
	//		info  <<"Trying to insert the lateral boundary condition" << endl;
	//		msg.make_second_info(info.str());
	//		msg.output_msg(2);	
	//	}
	//}
	////insert point boundary condition
	//if(this->boundary_cond_point_flag==true){
	//	//insert row
	//	model.insertRows(0, 1);
	//						
	//	//insert data
	//	model.setData(model.index(0,model.record().indexOf((_Hyd_River_Profile::boundary_table->get_column_name(hyd_label::profdata_glob_id)).c_str())),glob_prof_id);
	//	model.setData(model.index(0,model.record().indexOf((_Hyd_River_Profile::boundary_table->get_column_name(label::areastate_id)).c_str())), this->system_id.area_state);
	//	model.setData(model.index(0,model.record().indexOf((_Hyd_River_Profile::boundary_table->get_column_name(label::measure_id)).c_str())), this->system_id.measure_nr);
	//	model.setData(model.index(0,model.record().indexOf((_Hyd_River_Profile::boundary_table->get_column_name(label::applied_flag)).c_str())), true);
	//	model.setData(model.index(0,model.record().indexOf((_Hyd_River_Profile::boundary_table->get_column_name(hyd_label::sz_bound_id)).c_str())), this->hyd_sz.get_id());
	//	model.setData(model.index(0,model.record().indexOf((_Hyd_River_Profile::boundary_table->get_column_name(hyd_label::bounddata_stat)).c_str())), this->boundary_point_stat_flag);
	//	if(this->boundary_point_stat_flag==true){
	//		model.setData(model.index(0,model.record().indexOf((_Hyd_River_Profile::boundary_table->get_column_name(hyd_label::bounddata_value)).c_str())), *this->value_boundary_point_stat);
	//	}
	//	else{
	//		model.setData(model.index(0,model.record().indexOf((_Hyd_River_Profile::boundary_table->get_column_name(hyd_label::bounddata_value)).c_str())), *this->number_boundary_point_curve);
	//	}	
	//	model.setData(model.index(0,model.record().indexOf((_Hyd_River_Profile::boundary_table->get_column_name(hyd_label::bounddata_type)).c_str())), Hyd_Instationary_Boundary::transfrom_instatboundtype2txt(_hyd_bound_type::point_type).c_str());

	//	//submit it to the datbase
	//	Data_Base::database_submit(&model);

	//	if(model.lastError().isValid()){
	//		Warning msg=this->set_warning(0);
	//		ostringstream info;
	//		info << "Table Name                : " << _Hyd_River_Profile::boundary_table->get_table_name() << endl;
	//		info << "Table error info          : " << model.lastError().text().toStdString() << endl;
	//		info << "Profile number            : " << this->profile_number << endl;
	//		info << "Profile name              : " << this->name << endl;
	//		info << "Trying to insert the point boundary condition" << endl;
	//		msg.make_second_info(info.str());
	//		msg.output_msg(2);	
	//	}
	//}
}
//Transfer the river profile wall break data to a database
void Hyd_River_Profile_Connection_Standard::transfer_wallbreak_data2database(QSqlDatabase *ptr_database, const int glob_prof_id){
	if(this->break_flag_left==false && this->break_flag_right==false){
		return;
	}
	//mysql query with the table_model
	QSqlTableModel model(0,*ptr_database);
	//the table is set (the name and the column names) and allocated
	try{
		_Hyd_River_Profile::set_profile_wallbreak_table(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}
	//give the table name to the query model
	model.setTable((_Hyd_River_Profile::wallbreak_table->get_table_name()).c_str());
	if(this->break_flag_left==true){
		if(this->left_break_params->abrupt_fails_flag==true){
			//insert row
			model.insertRows(0, 1);
			//insert data
			model.setData(model.index(0,model.record().indexOf((_Hyd_River_Profile::wallbreak_table->get_column_name(hyd_label::profdata_glob_id)).c_str())),glob_prof_id);
			model.setData(model.index(0,model.record().indexOf((_Hyd_River_Profile::wallbreak_table->get_column_name(hyd_label::profbreak_left)).c_str())), true);
			model.setData(model.index(0,model.record().indexOf((_Hyd_River_Profile::wallbreak_table->get_column_name(hyd_label::profwall_res)).c_str())), this->left_break_params->resistance);
			model.setData(model.index(0,model.record().indexOf((_Hyd_River_Profile::wallbreak_table->get_column_name(hyd_label::profwall_abrupt)).c_str())), this->left_break_params->abrupt_opening);
			//submit it to the datbase
			Data_Base::database_submit(&model);

			if(model.lastError().isValid()){
				Warning msg=this->set_warning(0);
				ostringstream info;
				info << "Table Name                : " << _Hyd_River_Profile::wallbreak_table->get_table_name() << endl;
				info << "Table error info          : " << model.lastError().text().toStdString() << endl;
				info << "Profile number            : " << this->profile_number << endl;
				info << "Profile name              : " << this->name << endl;
				info << "Trying to insert the left bank profile data" << endl;
				msg.make_second_info(info.str());
				msg.output_msg(2);	
			}
		}
	}
	if(this->break_flag_right==true){
		if(this->right_break_params->abrupt_fails_flag==true){
			//insert row
			model.insertRows(0, 1);
			//insert data
			model.setData(model.index(0,model.record().indexOf((_Hyd_River_Profile::wallbreak_table->get_column_name(hyd_label::profdata_glob_id)).c_str())),glob_prof_id);
			model.setData(model.index(0,model.record().indexOf((_Hyd_River_Profile::wallbreak_table->get_column_name(hyd_label::profbreak_left)).c_str())), false);
			model.setData(model.index(0,model.record().indexOf((_Hyd_River_Profile::wallbreak_table->get_column_name(hyd_label::profwall_res)).c_str())), this->right_break_params->resistance);
			model.setData(model.index(0,model.record().indexOf((_Hyd_River_Profile::wallbreak_table->get_column_name(hyd_label::profwall_abrupt)).c_str())), this->right_break_params->abrupt_opening);
			//submit it to the datbase
			Data_Base::database_submit(&model);

			if(model.lastError().isValid()){
				Warning msg=this->set_warning(0);
				ostringstream info;
				info << "Table Name                : " << _Hyd_River_Profile::wallbreak_table->get_table_name() << endl;
				info << "Table error info          : " << model.lastError().text().toStdString() << endl;
				info << "Profile number            : " << this->profile_number << endl;
				info << "Profile name              : " << this->name << endl;
				info << "Trying to insert the right bank profile data" << endl;
				msg.make_second_info(info.str());
				msg.output_msg(2);	
			}
		}
	}
}
//Transfer the river profile dike break data to a database
void Hyd_River_Profile_Connection_Standard::transfer_dikebreak_data2database(QSqlDatabase *ptr_database, const int glob_prof_id){
	if(this->break_flag_left==false && this->break_flag_right==false){
		return;
	}
	//mysql query with the table_model
	QSqlTableModel model(0,*ptr_database);
	//the table is set (the name and the column names) and allocated
	try{
		_Hyd_River_Profile::set_profile_dikebreak_table(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}
	//give the table name to the query model
	model.setTable((_Hyd_River_Profile::dikebreak_table->get_table_name()).c_str());
	if(this->break_flag_left==true){
		if(this->left_break_params->abrupt_fails_flag==false){
			//insert row
			model.insertRows(0, 1);
			//insert data
			model.setData(model.index(0,model.record().indexOf((_Hyd_River_Profile::dikebreak_table->get_column_name(hyd_label::profdata_glob_id)).c_str())),glob_prof_id);
			model.setData(model.index(0,model.record().indexOf((_Hyd_River_Profile::dikebreak_table->get_column_name(hyd_label::profbreak_left)).c_str())), true);
			model.setData(model.index(0,model.record().indexOf((_Hyd_River_Profile::dikebreak_table->get_column_name(hyd_label::profdike_crit_v)).c_str())), this->left_break_params->resistance);
			//submit it to the datbase
			Data_Base::database_submit(&model);

			if(model.lastError().isValid()){
				Warning msg=this->set_warning(0);
				ostringstream info;
				info << "Table Name                : " << _Hyd_River_Profile::dikebreak_table->get_table_name() << endl;
				info << "Table error info          : " << model.lastError().text().toStdString() << endl;
				info << "Profile number            : " << this->profile_number << endl;
				info << "Profile name              : " << this->name << endl;	
				info << "Trying to insert the left bank profile data" << endl;
				msg.make_second_info(info.str());
				msg.output_msg(2);	
			}
		}
	}
	if(this->break_flag_right==true){
		if(this->right_break_params->abrupt_fails_flag==false){
			//insert row
			model.insertRows(0, 1);
			//insert data
			model.setData(model.index(0,model.record().indexOf((_Hyd_River_Profile::dikebreak_table->get_column_name(hyd_label::profdata_glob_id)).c_str())),glob_prof_id);
			model.setData(model.index(0,model.record().indexOf((_Hyd_River_Profile::dikebreak_table->get_column_name(hyd_label::profbreak_left)).c_str())), false);
			model.setData(model.index(0,model.record().indexOf((_Hyd_River_Profile::dikebreak_table->get_column_name(hyd_label::profdike_crit_v)).c_str())), this->right_break_params->resistance);
			
			//submit it to the datbase
			Data_Base::database_submit(&model);

			if(model.lastError().isValid()){
				Warning msg=this->set_warning(0);
				ostringstream info;
				info << "Table Name                : " << _Hyd_River_Profile::dikebreak_table->get_table_name() << endl;
				info << "Table error info          : " << model.lastError().text().toStdString() << endl;
				info << "Profile number            : " << this->profile_number << endl;
				info << "Profile name              : " << this->name << endl;
				info << "Trying to insert the right bank profile data" << endl;
				msg.make_second_info(info.str());
				msg.output_msg(2);	
			}
		}
	}
}
//Transfer the river profile bridge data to a database
void Hyd_River_Profile_Connection_Standard::transfer_bridge_data2database(QSqlDatabase *ptr_database, const int glob_prof_id){
	if(this->type!=_hyd_profile_types::BRIDGE_TYPE){
		return;
	}
	//mysql query with the table_model
	QSqlTableModel model(0,*ptr_database);

	//the table is set (the name and the column names) and allocated
	try{
		_Hyd_River_Profile::set_profile_bridge_table(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}
	//give the table name to the query model
	model.setTable((_Hyd_River_Profile::bridge_table->get_table_name()).c_str());
	//insert row
	model.insertRows(0, 1);
	//insert data
	model.setData(model.index(0,model.record().indexOf((_Hyd_River_Profile::bridge_table->get_column_name(hyd_label::profdata_glob_id)).c_str())),glob_prof_id);
	model.setData(model.index(0,model.record().indexOf((_Hyd_River_Profile::bridge_table->get_column_name(hyd_label::profbridge_size)).c_str())), this->bridge_specific_value->bridgebody_size);
	model.setData(model.index(0,model.record().indexOf((_Hyd_River_Profile::bridge_table->get_column_name(hyd_label::profbridge_height)).c_str())), this->bridge_specific_value->local_bridge_height);
	//submit it to the datbase
	Data_Base::database_submit(&model);

	if(model.lastError().isValid()){
		Warning msg=this->set_warning(0);
		ostringstream info;
		info << "Table Name                : " << _Hyd_River_Profile::bridge_table->get_table_name() << endl;
		info << "Table error info          : " << model.lastError().text().toStdString() << endl;
		info << "Profile number            : " << this->profile_number << endl;
		info << "Profile name              : " << this->name << endl;
		info << "Trying to insert the bridge profile data" << endl;
		msg.make_second_info(info.str());
		msg.output_msg(2);	
	}

}
//Input the lateral boundary data from a database
void Hyd_River_Profile_Connection_Standard::input_lateral_boundary_per_database(const int glob_prof_id, QSqlTableModel *model){
	
	//set the query
	ostringstream filter;
	filter << _Hyd_River_Profile::boundary_table->get_column_name(hyd_label::profdata_glob_id) << " = " <<  glob_prof_id;
	filter << " and " ;
	filter << _Hyd_River_Profile::boundary_table->get_column_name(label::areastate_id) << " = " << this->system_id.area_state;
	filter << " and " ;
	filter << _Hyd_River_Profile::boundary_table->get_column_name(label::measure_id) << " = " << this->system_id.measure_nr;
	filter << " and " ;
	filter << _Hyd_River_Profile::boundary_table->get_column_name(hyd_label::sz_bound_id) << " = " << this->hyd_sz.get_id();
	filter << " and " ;
	filter << _Hyd_River_Profile::boundary_table->get_column_name(hyd_label::bounddata_type)<< " = '"  << Hyd_Instationary_Boundary::transfrom_instatboundtype2txt(_hyd_bound_type::length_type) << "'";
	filter << " and " ;
	filter << _Hyd_River_Profile::boundary_table->get_column_name(label::applied_flag)<< " = " << " true " ;

	model->setFilter(filter.str().c_str());
	//launch the request
	Data_Base::database_request(model);
	//check the query
	if(model->lastError().isValid()){
		//error
		Error msg=set_error(12);
		ostringstream info;
		info << "Table Name      : " << _Hyd_River_Profile::boundary_table->get_table_name() << endl;
		info << "Table error info: " << model->lastError().text().toStdString() << endl;
		info << "Boundary type   : " << Hyd_Instationary_Boundary::transfrom_instatboundtype2txt(_hyd_bound_type::length_type) << endl;
		msg.make_second_info(info.str());
		Sys_Common_Output::output_hyd->rewind_userprefix();
		throw msg;
	}
	if(model->rowCount()>1){
		Warning msg=this->set_warning(2);
		ostringstream info;
		info << "Table Name      : " << _Hyd_River_Profile::boundary_table->get_table_name() << endl;
		info << "Profile number  : " << this->profile_number << endl;
		info << "Profile name    : " << this->name << endl;
		info << "Global id       : " << glob_prof_id << endl;
		info << "Boundary type   : " << Hyd_Instationary_Boundary::transfrom_instatboundtype2txt(_hyd_bound_type::length_type) << endl;
		msg.make_second_info(info.str());
		msg.output_msg(2);
	}

	if(model->rowCount()>0){
		this->boundary_cond_lateral_flag=true;
		//read out the results from the request
		this->boundary_lateral_stat_flag=model->record(0).value((_Hyd_River_Profile::boundary_table->get_column_name(hyd_label::bounddata_stat)).c_str()).toBool();
		double buffer=model->record(0).value((_Hyd_River_Profile::boundary_table->get_column_name(hyd_label::bounddata_value)).c_str()).toDouble();
		this->decide_boundary_lateral_conditions(buffer);
	}
	else if(model->rowCount()==0){
		//second search
		//set the query
		ostringstream filter;
		filter << _Hyd_River_Profile::boundary_table->get_column_name(hyd_label::profdata_glob_id) << " = " <<  glob_prof_id;
		filter << " and " ;
		filter << _Hyd_River_Profile::boundary_table->get_column_name(label::areastate_id) << " = " << this->system_id.area_state;
		filter << " and " ;
		filter << _Hyd_River_Profile::boundary_table->get_column_name(label::measure_id) << " = " << 0;
		filter << " and " ;
		filter << _Hyd_River_Profile::boundary_table->get_column_name(hyd_label::sz_bound_id) << " = "  << this->hyd_sz.get_id();
		filter << " and " ;
		filter << _Hyd_River_Profile::boundary_table->get_column_name(hyd_label::bounddata_type) << " = '" << Hyd_Instationary_Boundary::transfrom_instatboundtype2txt(_hyd_bound_type::length_type)<< "'";
		filter << " and " ;
		filter << _Hyd_River_Profile::boundary_table->get_column_name(label::applied_flag)<< " = "  << " true " ;

		model->setFilter(filter.str().c_str());
		//launch the request
		Data_Base::database_request(model);
		//check the query
		if(model->lastError().isValid()){
			//error
			Error msg=set_error(12);
			ostringstream info;
			info << "Table Name      : " << _Hyd_River_Profile::boundary_table->get_table_name() << endl;
			info << "Table error info: " << model->lastError().text().toStdString() << endl;
			info << "Profile no.     : " << this->profile_number << endl;
			info << "Boundary type   : " << Hyd_Instationary_Boundary::transfrom_instatboundtype2txt(_hyd_bound_type::length_type) << endl;
			msg.make_second_info(info.str());
			Sys_Common_Output::output_hyd->rewind_userprefix();
			throw msg;
		}
		if(model->rowCount()>1){
			Warning msg=this->set_warning(2);
			ostringstream info;
			info << "Table Name      : " << _Hyd_River_Profile::boundary_table->get_table_name() << endl;
			info << "Profile no.     : " << this->profile_number << endl;
			info << "Global id       : " << glob_prof_id << endl;
			info << "Boundary type   : " << Hyd_Instationary_Boundary::transfrom_instatboundtype2txt(_hyd_bound_type::length_type) << endl;
			msg.make_second_info(info.str());
			msg.output_msg(2);
		}
		if(model->rowCount()>0){
			this->boundary_cond_lateral_flag=true;
			//read out the results from the request
			this->boundary_lateral_stat_flag=model->record(0).value((_Hyd_River_Profile::boundary_table->get_column_name(hyd_label::bounddata_stat)).c_str()).toBool();
			double buffer=model->record(0).value((_Hyd_River_Profile::boundary_table->get_column_name(hyd_label::bounddata_value)).c_str()).toDouble();
			this->decide_boundary_lateral_conditions(buffer);
		}
		else{
			this->boundary_cond_lateral_flag=false;
		}
	}
}
//Input the point boundary data from a database
void Hyd_River_Profile_Connection_Standard::input_point_boundary_per_database(const int glob_prof_id, QSqlTableModel *model){
	//set the query
	ostringstream filter;
	filter << _Hyd_River_Profile::boundary_table->get_column_name(hyd_label::profdata_glob_id) << " = " <<  glob_prof_id;
	filter << " and " ;
	filter << _Hyd_River_Profile::boundary_table->get_column_name(label::areastate_id) << " = "<< this->system_id.area_state;
	filter << " and " ;
	filter << _Hyd_River_Profile::boundary_table->get_column_name(label::measure_id) << " = "<< this->system_id.measure_nr;
	filter << " and " ;
	filter << _Hyd_River_Profile::boundary_table->get_column_name(hyd_label::sz_bound_id) << " = "<< this->hyd_sz.get_id();
	filter << " and " ;
	filter << _Hyd_River_Profile::boundary_table->get_column_name(hyd_label::bounddata_type)<< " = '" << Hyd_Instationary_Boundary::transfrom_instatboundtype2txt(_hyd_bound_type::point_type)<<"'";
	filter << " and " ;
	filter << _Hyd_River_Profile::boundary_table->get_column_name(label::applied_flag)<< " = "  << " true " ;

	model->setFilter(filter.str().c_str());
	//launch the request
	Data_Base::database_request(model);
	//check the query
	if(model->lastError().isValid()){
		//error
		Error msg=set_error(12);
		ostringstream info;
		info << "Table Name      : " << _Hyd_River_Profile::boundary_table->get_table_name() << endl;
		info << "Table error info: " << model->lastError().text().toStdString() << endl;
		info << "Boundary type   : " << Hyd_Instationary_Boundary::transfrom_instatboundtype2txt(_hyd_bound_type::point_type) << endl;
		msg.make_second_info(info.str());
		Sys_Common_Output::output_hyd->rewind_userprefix();
		throw msg;
	}
	if(model->rowCount()>1){
		Warning msg=this->set_warning(2);
		ostringstream info;
		info << "Table Name      : " << _Hyd_River_Profile::boundary_table->get_table_name() << endl;
		info << "Profile number  : " << this->profile_number << endl;
		info << "Profile name    : " << this->name << endl;
		info << "Global id       : " << glob_prof_id << endl;
		info << "Boundary type   : " << Hyd_Instationary_Boundary::transfrom_instatboundtype2txt(_hyd_bound_type::point_type) << endl;
		msg.make_second_info(info.str());
		msg.output_msg(2);
	}

	if(model->rowCount()>0){
		this->boundary_cond_point_flag=true;
		//read out the results from the request
		this->boundary_point_stat_flag=model->record(0).value((_Hyd_River_Profile::boundary_table->get_column_name(hyd_label::bounddata_stat)).c_str()).toBool();
		double buffer=model->record(0).value((_Hyd_River_Profile::boundary_table->get_column_name(hyd_label::bounddata_value)).c_str()).toDouble();
		this->decide_boundary_point_conditions(buffer);
	}
	else if(model->rowCount()==0){
		//second search
		//set the query
		ostringstream filter;
		filter << _Hyd_River_Profile::boundary_table->get_column_name(hyd_label::profdata_glob_id) << " = " <<  glob_prof_id;
		filter << " and " ;
		filter << _Hyd_River_Profile::boundary_table->get_column_name(label::areastate_id) << " = "<< this->system_id.area_state;
		filter << " and " ;
		filter << _Hyd_River_Profile::boundary_table->get_column_name(label::measure_id) << " = "<< 0;
		filter << " and " ;
		filter << _Hyd_River_Profile::boundary_table->get_column_name(hyd_label::sz_bound_id) << " = "<< this->hyd_sz.get_id();
		filter << " and " ;
		filter << _Hyd_River_Profile::boundary_table->get_column_name(hyd_label::bounddata_type) << " = '"<< Hyd_Instationary_Boundary::transfrom_instatboundtype2txt(_hyd_bound_type::point_type)<<"'";
		filter << " and " ;
		filter << _Hyd_River_Profile::boundary_table->get_column_name(label::applied_flag)<< " = "  << " true " ;

		model->setFilter(filter.str().c_str());
		//launch the request
		Data_Base::database_request(model);
		//check the query
		if(model->lastError().isValid()){
			//error
			Error msg=set_error(12);
			ostringstream info;
			info << "Table Name      : " << _Hyd_River_Profile::boundary_table->get_table_name() << endl;
			info << "Table error info: " << model->lastError().text().toStdString() << endl;
			info << "Boundary type   : " << Hyd_Instationary_Boundary::transfrom_instatboundtype2txt(_hyd_bound_type::point_type) << endl;
			msg.make_second_info(info.str());
			Sys_Common_Output::output_hyd->rewind_userprefix();
			throw msg;
		}
		if(model->rowCount()>1){
			Warning msg=this->set_warning(2);
			ostringstream info;
			info << "Table Name      : " << _Hyd_River_Profile::boundary_table->get_table_name() << endl;
			info << "Global id       : " << glob_prof_id << endl;
			info << "Boundary type   : " << Hyd_Instationary_Boundary::transfrom_instatboundtype2txt(_hyd_bound_type::point_type) << endl;
			msg.make_second_info(info.str());
			msg.output_msg(2);
		}
		if(model->rowCount()>0){
			this->boundary_cond_point_flag=true;
			//read out the results from the request
			this->boundary_point_stat_flag=model->record(0).value((_Hyd_River_Profile::boundary_table->get_column_name(hyd_label::bounddata_stat)).c_str()).toBool();
			double buffer=model->record(0).value((_Hyd_River_Profile::boundary_table->get_column_name(hyd_label::bounddata_value)).c_str()).toDouble();
			this->decide_boundary_point_conditions(buffer);
		}
		else{
			this->boundary_cond_point_flag=false;
		}
	}
}
//Input the river profile wall break data from a database
void Hyd_River_Profile_Connection_Standard::input_wallbreak_data_per_database(QSqlDatabase *ptr_database, const int glob_prof_id){
	if(this->break_flag_left==false && this->break_flag_right==false){
		return;
	}
	//mysql query with the table_model
	QSqlTableModel model(0,*ptr_database);
	//the table is set (the name and the column names) and allocated
	try{
		_Hyd_River_Profile::set_profile_wallbreak_table(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}

	///give the table name to the query model
	model.setTable((_Hyd_River_Profile::wallbreak_table->get_table_name()).c_str());

	//left bank
	if(this->break_flag_left==true){
		//set the query
		ostringstream filter;
		filter << _Hyd_River_Profile::wallbreak_table->get_column_name(hyd_label::profdata_glob_id) << " = " <<  glob_prof_id;
		filter << " and " ;
		filter << _Hyd_River_Profile::wallbreak_table->get_column_name(hyd_label::profbreak_left) << "= true ";

		model.setFilter(filter.str().c_str());
		//launch the request
		Data_Base::database_request(&model);
		//check the query
		if(model.lastError().isValid()){
			//error
			Error msg=set_error(12);
			ostringstream info;
			info << "Table Name      : " << _Hyd_River_Profile::wallbreak_table->get_table_name() << endl;
			info << "Table error info: " << model.lastError().text().toStdString() << endl;
			msg.make_second_info(info.str());
			Sys_Common_Output::output_hyd->rewind_userprefix();
			throw msg;
		}
		if(model.rowCount()>1){
			Warning msg=this->set_warning(2);
			ostringstream info;
			info << "Table Name      : " << _Hyd_River_Profile::wallbreak_table->get_table_name() << endl;
			info << "Profile number  : " << this->profile_number << endl;
			info << "Profile name    : " << this->name << endl;
			info << "Global id       : " << glob_prof_id << endl;
			msg.make_second_info(info.str());
			msg.output_msg(2);
		}
		if(model.rowCount()>0){
			this->allocate_break_parameter_left();
			//read out the results from the request
			this->left_break_params->resistance=model.record(0).value((_Hyd_River_Profile::wallbreak_table->get_column_name(hyd_label::profwall_res)).c_str()).toDouble();
			this->left_break_params->abrupt_opening=model.record(0).value((_Hyd_River_Profile::wallbreak_table->get_column_name(hyd_label::profwall_abrupt)).c_str()).toDouble();
			this->left_break_params->abrupt_fails_flag=true;
		}
	}
	//right bank
	if(this->break_flag_right==true){
		//set the query
		ostringstream filter;
		filter << _Hyd_River_Profile::wallbreak_table->get_column_name(hyd_label::profdata_glob_id) << "=" <<  glob_prof_id;
		filter << " and " ;
		filter << _Hyd_River_Profile::wallbreak_table->get_column_name(hyd_label::profbreak_left) << "= false ";
		
		model.setFilter(filter.str().c_str());
		//launch the request
		Data_Base::database_request(&model);
		//check the query
		if(model.lastError().isValid()){
			//error
			Error msg=set_error(12);
			ostringstream info;
			info << "Table Name      : " << _Hyd_River_Profile::wallbreak_table->get_table_name() << endl;
			info << "Table error info: " << model.lastError().text().toStdString() << endl;
			msg.make_second_info(info.str());
			Sys_Common_Output::output_hyd->rewind_userprefix();
			throw msg;
		}
		if(model.rowCount()>1){
			Warning msg=this->set_warning(2);
			ostringstream info;
			info << "Table Name      : " << _Hyd_River_Profile::wallbreak_table->get_table_name() << endl;
			info << "Profile number  : " << this->profile_number << endl;
			info << "Profile name    : " << this->name << endl;
			info << "Global id       : " << glob_prof_id << endl;
			msg.make_second_info(info.str());
			msg.output_msg(2);
		}
		if(model.rowCount()>0){
			this->allocate_break_parameter_right();
			//read out the results from the request
			this->right_break_params->resistance=model.record(0).value((_Hyd_River_Profile::wallbreak_table->get_column_name(hyd_label::profwall_res)).c_str()).toDouble();
			this->right_break_params->abrupt_opening=model.record(0).value((_Hyd_River_Profile::wallbreak_table->get_column_name(hyd_label::profwall_abrupt)).c_str()).toDouble();
			this->right_break_params->abrupt_fails_flag=true;
		}
	}
}
//Input the river profile dike break data from a database
void Hyd_River_Profile_Connection_Standard::input_dikebreak_data_per_database(QSqlDatabase *ptr_database, const int glob_prof_id){

	if(this->break_flag_left==false && this->break_flag_right==false){
		return;
	}
	//mysql query with the table_model
	QSqlTableModel model(0,*ptr_database);
	//the table is set (the name and the column names) and allocated
	try{
		_Hyd_River_Profile::set_profile_dikebreak_table(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}

	///give the table name to the query model
	model.setTable((_Hyd_River_Profile::dikebreak_table->get_table_name()).c_str());

	//left bank
	if(this->break_flag_left==true){
		//set the query
		ostringstream filter;
		filter << _Hyd_River_Profile::dikebreak_table->get_column_name(hyd_label::profdata_glob_id) << " = " <<  glob_prof_id;
		filter << " and " ;
		filter << _Hyd_River_Profile::dikebreak_table->get_column_name(hyd_label::profbreak_left) << "= true ";

		model.setFilter(filter.str().c_str());
		//launch the request
		Data_Base::database_request(&model);
		//check the query
		if(model.lastError().isValid()){
			//error
			Error msg=set_error(12);
			ostringstream info;
			info << "Table Name      : " << _Hyd_River_Profile::dikebreak_table->get_table_name() << endl;
			info << "Table error info: " << model.lastError().text().toStdString() << endl;
			msg.make_second_info(info.str());
			Sys_Common_Output::output_hyd->rewind_userprefix();
			throw msg;
		}
		if(model.rowCount()>1){
			Warning msg=this->set_warning(2);
			ostringstream info;
			info << "Table Name      : " << _Hyd_River_Profile::dikebreak_table->get_table_name() << endl;
			info << "Profile number  : " << this->profile_number << endl;
			info << "Profile name    : " << this->name << endl;
			info << "Global id       : " << glob_prof_id << endl;
			msg.make_second_info(info.str());
			msg.output_msg(2);
		}
		if(model.rowCount()>0){
			this->allocate_break_parameter_left();
			//read out the results from the request
			this->left_break_params->resistance=model.record(0).value((_Hyd_River_Profile::dikebreak_table->get_column_name(hyd_label::profdike_crit_v)).c_str()).toDouble();
			this->left_break_params->abrupt_fails_flag=false;
		}
	}
	//right bank
	if(this->break_flag_right==true){
		//set the query
		ostringstream filter;
		filter << _Hyd_River_Profile::dikebreak_table->get_column_name(hyd_label::profdata_glob_id) << " = " <<  glob_prof_id;
		filter << " and " ;
		filter << _Hyd_River_Profile::dikebreak_table->get_column_name(hyd_label::profbreak_left) << "= false ";
		
		model.setFilter(filter.str().c_str());
		//launch the request
		Data_Base::database_request(&model);
		//check the query
		if(model.lastError().isValid()){
			//error
			Error msg=set_error(12);
			ostringstream info;
			info << "Table Name      : " << _Hyd_River_Profile::dikebreak_table->get_table_name() << endl;
			info << "Table error info: " << model.lastError().text().toStdString() << endl;
			msg.make_second_info(info.str());
			Sys_Common_Output::output_hyd->rewind_userprefix();
			throw msg;
		}
		if(model.rowCount()>1){
			Warning msg=this->set_warning(2);
			ostringstream info;
			info << "Table Name      : " << _Hyd_River_Profile::dikebreak_table->get_table_name() << endl;
			info << "Profile number  : " << this->profile_number << endl;
			info << "Profile name    : " << this->name << endl;
			info << "Global id       : " << glob_prof_id << endl;
			msg.make_second_info(info.str());
			msg.output_msg(2);
		}
		if(model.rowCount()>0){
			this->allocate_break_parameter_right();
			//read out the results from the request
			this->right_break_params->resistance=model.record(0).value((_Hyd_River_Profile::dikebreak_table->get_column_name(hyd_label::profdike_crit_v)).c_str()).toDouble();
			this->right_break_params->abrupt_fails_flag=false;
		}
	}
}
//Input the river profile bridge data from a database
void Hyd_River_Profile_Connection_Standard::input_bridge_data_per_database(QSqlDatabase *ptr_database, const int glob_prof_id){
	//check for a bridge profile
	if(this->type==_hyd_profile_types::BRIDGE_TYPE){
		
		//mysql query with the table_model
		QSqlTableModel model(0,*ptr_database);
		//the table is set (the name and the column names) and allocated
		try{
			_Hyd_River_Profile::set_profile_bridge_table(ptr_database);
		}
		catch(Error msg){
			throw msg;
		}

		///give the table name to the query model
		model.setTable((_Hyd_River_Profile::bridge_table->get_table_name()).c_str());


		//set the query
		ostringstream filter;
		filter << _Hyd_River_Profile::bridge_table->get_column_name(hyd_label::profdata_glob_id) << " = " <<  glob_prof_id;
		
		model.setFilter(filter.str().c_str());
		//launch the request
		Data_Base::database_request(&model);
		//check the query
		if(model.lastError().isValid()){
			//error
			Error msg=set_error(12);
			ostringstream info;
			info << "Table Name      : " << _Hyd_River_Profile::bridge_table->get_table_name() << endl;
			info << "Table error info: " << model.lastError().text().toStdString() << endl;
			msg.make_second_info(info.str());
			Sys_Common_Output::output_hyd->rewind_userprefix();
			throw msg;
		}
		if(model.rowCount()==0){
			Error msg=this->set_error(13);
			ostringstream info;
			info << "Table Name      : " << _Hyd_River_Profile::bridge_table->get_table_name() << endl;			
			msg.make_second_info(info.str());
			Sys_Common_Output::output_hyd->rewind_userprefix();
			throw msg;
		}
		if(model.rowCount()>1){
			Warning msg=this->set_warning(2);
			ostringstream info;
			info << "Table Name      : " << _Hyd_River_Profile::bridge_table->get_table_name() << endl;
			info << "Profile number  : " << this->profile_number << endl;
			info << "Profile name    : " << this->name << endl;
			info << "Global id       : " << glob_prof_id << endl;
			msg.make_second_info(info.str());
			msg.output_msg(2);
		}
		//all is ok
		this->allocate_bridge_data();
		//read out the results from the request
		this->bridge_specific_value->local_bridge_height=model.record(0).value((_Hyd_River_Profile::bridge_table->get_column_name(hyd_label::profbridge_height)).c_str()).toDouble();
		this->bridge_specific_value->bridgebody_size=model.record(0).value((_Hyd_River_Profile::bridge_table->get_column_name(hyd_label::profbridge_size)).c_str()).toDouble();

	}
}
//set the error
Error Hyd_River_Profile_Connection_Standard::set_error(const int err_type){
		string place="Hyd_River_Profile_Connection_Standard::";
		string help;
		string reason;
		int type=0;
		bool fatal=false;
		stringstream info;
		Error msg;

	switch (err_type){

		case 1://wrong input
			place.append("decide_keyvalues_file(const string key, string buffer, int *found_counter)");
			reason="There is a problem with the input; wrong sign is read in";
			help="Check the profile in the profile file";
			type=1;
			break;
		case 2://the inbetween profiles are not all standrad types
			place.append("check_connection_type_profile(void)");
			reason="The inbetween profile must be a standard profile";
			help="Check the connection type of the profiles";
			type=15;
			break;
		case 3://init connditions are dry
			place.append("check_connection_type_profile(void)");
			reason="The initialconditions of a riverprofile can not be 0.0 or below 0.0";
			help="Check the rofile";
			type=14;
			break;
		case 6://no sequentiell special profile types
			place.append("check_profiles(_Hyd_River_Profile *upstream_profile, _Hyd_River_Profile *downstream_profile)");
			reason="Two direct following special types of profiles (e.g. weir weir) is not possible";
			help="Check the profile type of this profile and the up-and downstream one; introduce an profile of river profile type inbetween";
			type=15;
			break;
		case 7://z min coordinate of profile before weir type is above weir crest
			place.append("check_profiles(_Hyd_River_Profile *upstream_profile, _Hyd_River_Profile *downstream_profile)");
			reason="The global z min coordinate of the profile before a weir profile must be below the z min coordinate of the weir; best is to have a river profile corresponding to the upstream weir base";
			help="Check this weir profile and the profile before";
			type=15;
			break;
		case 8://z min coordinate of profile after a weir type is above weir crest
			place.append("check_profiles(_Hyd_River_Profile *upstream_profile, _Hyd_River_Profile *downstream_profile)");
			reason="The global z min coordinate of the profile after a weir profile must be below the z min coordinate of the weir; best is to have a river profile corresponding to the downstream weir base";
			help="Check this weir profile and the profile after";
			type=15;
			break;
		case 9://z min coordinate of profile before a bridge type is greater than 0.5*the minimal local bridge height
			place.append("check_profiles(_Hyd_River_Profile *upstream_profile, _Hyd_River_Profile *downstream_profile)");
			info << "The global z min coordinate of the profile before a bridge profile must be below the z min coordinate +"<<0.5*constant::min_height_bridge << label::m <<" of the bridge profile;";
			info <<" for numerical reasons";
			reason=info.str();
			info.str("");
			info.clear();
			help="Check this bridge profile and the profile before";
			type=15;
			break;
		case 10://break-parameters are <0.0
			place.append("check_profiles(_Hyd_River_Profile *upstream_profile, _Hyd_River_Profile *downstream_profile)");
			reason="The break-parameters are < 0.0";
			help="Check break-parameters of the profile";
			type=15;
			break;
		case 11://z min coordinate of profile after a bridge type is greater than 0.5*the minimal local bridge height
			place.append("check_profiles(_Hyd_River_Profile *upstream_profile, _Hyd_River_Profile *downstream_profile)");
			info <<"The global z min coordinate of the profile after a bridge profile must be below the z min coordinate +"<<0.5*constant::min_height_bridge << label::m <<" of the bridge profile;";
			info <<" for numerical reasons";
			reason=info.str();
			info.str("");
			info.clear();
			help="Check this bridge profile and the profile after";
			type=15;
			break;
		case 12://problems with database query
			place.append("input_..._per_database(...)") ;
			reason="Can not submit the query to the database";
			help="Check the query error";
			type=6;	
			break;
		case 13://no matching parameter set is found for the given river index
			place.append("input_..._per_database(...)") ;
			reason="There is no matching data set for the given index";
			help="Check the river profile table or appending tables (see table name)";
			type=2;	
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

//set warning
Warning Hyd_River_Profile_Connection_Standard::set_warning(const int warn_type){
		string place="Hyd_River_Profile_Connection_Standard::";
		string help;
		string reason;
		string reaction;
		int type=0;
		Warning msg;
		stringstream info;

	switch (warn_type){
		case 0://input datas can not submitted
			place.append("transfer_...data...2database(QSqlDatabase *ptr_database, ...)");
			reason="Can not submit the data of the river profile to the database";
			help="Check the database";
			type=2;
			break;
		case 1://input datas can not submitted the polyline data
			place.append("transfer_profile_members2database(QSqlDatabase *ptr_database, const int rv_number)");
			reason="Can not submit the polyline data of the river profile to the database";
			help="Check the database";
			type=2;
			break;
		case 2://there are multiple data sets to the given profile
			place.append("input_..._data_per_database(...)");
			reason="There are multiple data sets (see table name) to the given global profile id";
			help="Check the database";
			reaction="The first found data set is taken";
			info << "Areastate/Measureno.   : " << this->system_id.area_state <<"/"<< this->system_id.measure_nr << endl; 
			type=2;
			break;
		case 3://try to allocate the left break parameter several times
			place.append("allocate_break_parameter_left(void)");
			reason="Try to allocate the parameter structure for a break (this->left_break_params) several times";
			help="Check the code";
			reaction="It is not allocated again";
			type=5;
			break;
		case 4://try to allocate the right break parameter several times
			place.append("allocate_break_parameter_right(void)");
			reason="Try to allocate the parameter structure for a break (this->right_break_params) several times";
			help="Check the code";
			reaction="It is not allocated again";
			type=5;
			break;
		case 5://the poleni factor is below zero
			place.append("check_profiles(void)");
			reason="The poleni factor is not in a physical interval (0.35-1.0)";
			help="Check the poleni factor of the profile";
			reaction="The factor is set to the default factor";
			type=3;
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
};