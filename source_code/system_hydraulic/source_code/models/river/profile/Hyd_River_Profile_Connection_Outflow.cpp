//#include "Hyd_River_Profile_Connection_Outflow.h"
#include "Hyd_Headers_Precompiled.h"

//constructor
Hyd_River_Profile_Connection_Outflow::Hyd_River_Profile_Connection_Outflow(void){
	this->boundary_cond_waterlevel_flag=false;
	this->boundary_waterlevel_stat_flag=false;
	this->value_boundary_waterlevel_stat=NULL;
	this->number_boundary_waterlevel_curve=NULL;
	this->ptr_boundarycurve_waterlevel=NULL;
	this->instatbound_waterlevel_found_flag=false;
	this->must_found_number=4;
	this->h_outflow=0.0;
	this->h_outflow_is_given=false;

	this->q_outflow=0.0;
	this->sum_coupling_q=0.0;

	this->old_calc_time_point=0.0;
	this->old_delta_t=0.0;
	this->old_discharge=0.0;
	this->old_discharge_2=0.0;
	this->outflow_volume=0.0;
	this->h_normal_flow=0.0;
	this->h_outflow_given=0.0;
	this->max_outflow_q.maximum=0.0;
	this->max_outflow_q.time_point=0.0;
	this->outflow_volume_syncron=0.0;
	this->watervolume_by_coupling=0.0;

	this->ptr_inbetween_profiles=NULL;
	this->number_inbetween_profiles=0;
	this->ptr_inflow_profiles=NULL;

	this->h_coup_old=0.0;

	//count the memory
	Sys_Memory_Count::self()->add_mem(sizeof(Hyd_River_Profile_Connection_Outflow), _sys_system_modules::HYD_SYS);
}
//destructor
Hyd_River_Profile_Connection_Outflow::~Hyd_River_Profile_Connection_Outflow(void){

	if(this->value_boundary_waterlevel_stat!=NULL){
		delete this->value_boundary_waterlevel_stat;
		this->value_boundary_waterlevel_stat=NULL;
	}
	if(this->number_boundary_waterlevel_curve!=NULL){
		delete this->number_boundary_waterlevel_curve;
		this->number_boundary_waterlevel_curve=NULL;
	}

	//count the memory
	Sys_Memory_Count::self()->minus_mem(sizeof(Hyd_River_Profile_Connection_Outflow), _sys_system_modules::HYD_SYS);
}
//_______________
//public
//Transfer the river profile data to a database
void Hyd_River_Profile_Connection_Outflow::transfer_profile_members2database(const int glob_prof_id, QSqlQuery *model, QSqlDatabase *ptr_database, const int rv_number){
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
	query_string << "'"<<this->name << "' , " ;
	query_string<<FORMAT_FIXED_REAL<<P(15);
	query_string << this->river_station<< " , " ;
	query_string << this->delta_x_table<< " , " ;
	query_string << "'"<<this->convert_connectiontype2string(this->connection)<< "' , " ;
	query_string << "'"<<this->convert_profiletype2string(this->type)<< "' , " ;
	query_string <<  this->init_condition<< " , " ;
	query_string << this->profile_number<< " , " ;
	query_string << glob_prof_id<< " , " ;

	query_string << "false"<< " , " ;
	query_string << "false"<< " , " ;
	query_string << 0.0 << " , " ;
	query_string << 0.0 << " , " ;
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
		Warning msg=this->set_warning(1);
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
void Hyd_River_Profile_Connection_Outflow::input_members_per_database(const int index, const QSqlQueryModel *query_result, QSqlDatabase *ptr_database){
	

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
void Hyd_River_Profile_Connection_Outflow::input_boundary_data_per_database(QSqlDatabase *ptr_database, const int glob_prof_id){

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
	//set the query
	ostringstream filter;
	filter << _Hyd_River_Profile::boundary_table->get_column_name(hyd_label::profdata_glob_id) << " = " <<  glob_prof_id;
	filter << " and " ;
	filter << _Hyd_River_Profile::boundary_table->get_column_name(label::areastate_id)  << " = "<< this->system_id.area_state;
	filter << " and " ;
	filter << _Hyd_River_Profile::boundary_table->get_column_name(label::measure_id)  << " = "<< this->system_id.measure_nr;
	filter << " and " ;
	filter << _Hyd_River_Profile::boundary_table->get_column_name(hyd_label::sz_bound_id)  << " = "<< this->hyd_sz.get_id();
	filter << " and " ;
	filter << _Hyd_River_Profile::boundary_table->get_column_name(hyd_label::bounddata_type)  << " = '"<< Hyd_Instationary_Boundary::transfrom_instatboundtype2txt(_hyd_bound_type::waterlevel_type)<<"'";
	filter << " and " ;
	filter << _Hyd_River_Profile::boundary_table->get_column_name(label::applied_flag)<< " = " << " true " ;

	model.setFilter(filter.str().c_str());
	//launch the request
	Data_Base::database_request(&model);
	//check the query
	if(model.lastError().isValid()){
		//error
		Error msg=set_error(8);
		ostringstream info;
		info << "Table Name      : " << _Hyd_River_Profile::boundary_table->get_table_name() << endl;
		info << "Table error info: " << model.lastError().text().toStdString() << endl;
		info << "Boundary type   : " << Hyd_Instationary_Boundary::transfrom_instatboundtype2txt(_hyd_bound_type::waterlevel_type) << endl;
		msg.make_second_info(info.str());
		Sys_Common_Output::output_hyd->rewind_userprefix();
		throw msg;
	}
	if(model.rowCount()>1){
		Warning msg=this->set_warning(3);
		ostringstream info;
		info << "Table Name      : " << _Hyd_River_Profile::boundary_table->get_table_name() << endl;
		info << "Global id       : " << glob_prof_id << endl;
		info << "Boundary type   : " << Hyd_Instationary_Boundary::transfrom_instatboundtype2txt(_hyd_bound_type::waterlevel_type) << endl;
		msg.make_second_info(info.str());
		msg.output_msg(2);
	}

	if(model.rowCount()>0){
		this->boundary_cond_waterlevel_flag=true;
		//read out the results from the request
		this->boundary_waterlevel_stat_flag=model.record(0).value((_Hyd_River_Profile::boundary_table->get_column_name(hyd_label::bounddata_stat)).c_str()).toBool();
		double buffer=model.record(0).value((_Hyd_River_Profile::boundary_table->get_column_name(hyd_label::bounddata_value)).c_str()).toDouble();
		this->decide_boundary_waterlevel_conditions(buffer);

	}
	else if(model.rowCount()==0){
		//second search
		//set the query
		ostringstream filter;
		filter << _Hyd_River_Profile::boundary_table->get_column_name(hyd_label::profdata_glob_id) << " = "  <<  glob_prof_id;
		filter << " and " ;
		filter << _Hyd_River_Profile::boundary_table->get_column_name(label::areastate_id) << " = "  << this->system_id.area_state;
		filter << " and " ;
		filter << _Hyd_River_Profile::boundary_table->get_column_name(label::measure_id) << " = "<< 0;
		filter << " and " ;
		filter << _Hyd_River_Profile::boundary_table->get_column_name(hyd_label::sz_bound_id) << " = "<< this->hyd_sz.get_id();
		filter << " and " ;
		filter << _Hyd_River_Profile::boundary_table->get_column_name(hyd_label::bounddata_type) << " = '"<< Hyd_Instationary_Boundary::transfrom_instatboundtype2txt(_hyd_bound_type::waterlevel_type)<<"'";
		filter << " and " ;
		filter << _Hyd_River_Profile::boundary_table->get_column_name(label::applied_flag)<< " = " << " true " ;

		model.setFilter(filter.str().c_str());
		//launch the request
		Data_Base::database_request(&model);
		//check the query
		if(model.lastError().isValid()){
			//error
			Error msg=set_error(8);
			ostringstream info;
			info << "Table Name      : " << _Hyd_River_Profile::boundary_table->get_table_name() << endl;
			info << "Table error info: " << model.lastError().text().toStdString() << endl;
			info << "Profile no.     : " << this->profile_number << endl;
			info << "Boundary type   : " << Hyd_Instationary_Boundary::transfrom_instatboundtype2txt(_hyd_bound_type::waterlevel_type) << endl;
			msg.make_second_info(info.str());
			Sys_Common_Output::output_hyd->rewind_userprefix();
			throw msg;
		}
		if(model.rowCount()>1){
			Warning msg=this->set_warning(3);
			ostringstream info;
			info << "Table Name      : " << _Hyd_River_Profile::boundary_table->get_table_name() << endl;
			info << "Profile no.     : " << this->profile_number << endl;
			info << "Global id       : " << glob_prof_id << endl;
			info << "Boundary type   : " << Hyd_Instationary_Boundary::transfrom_instatboundtype2txt(_hyd_bound_type::waterlevel_type) << endl;
			msg.make_second_info(info.str());
			msg.output_msg(2);
		}
		if(model.rowCount()>0){
			this->boundary_cond_waterlevel_flag=true;
			//read out the results from the request
			this->boundary_waterlevel_stat_flag=model.record(0).value((_Hyd_River_Profile::boundary_table->get_column_name(hyd_label::bounddata_stat)).c_str()).toBool();
			double buffer=model.record(0).value((_Hyd_River_Profile::boundary_table->get_column_name(hyd_label::bounddata_value)).c_str()).toDouble();
			this->decide_boundary_waterlevel_conditions(buffer);


		}
		else{
			this->boundary_cond_waterlevel_flag=false;
		}
	}

}
//Check the profiles
void Hyd_River_Profile_Connection_Outflow::check_profiles(_Hyd_River_Profile *upstream_profile, _Hyd_River_Profile *upupstream_profile){
	//before last profile have to be a river type profile if a normal flow boundary is set
	if(upstream_profile->get_profile_type()!=_hyd_profile_types::RIVER_TYPE){
		Error msg=this->set_error(4);
		throw msg;
	}
	//there must be a slope in the last two river segment
	if(upstream_profile->typ_of_profile->get_global_z_min()<=this->typ_of_profile->get_global_z_min() && this->boundary_cond_waterlevel_flag==false){
		Error msg=this->set_error(5);
		throw msg;
	}
	if(upupstream_profile!=NULL){
		if(upupstream_profile->typ_of_profile->get_global_z_min()<=upstream_profile->typ_of_profile->get_global_z_min() && this->boundary_cond_waterlevel_flag==false){
			Error msg=this->set_error(5);
			throw msg;
		}
	}
	//check if a stationary boundary condition is >=0.0
	if(this->boundary_cond_waterlevel_flag==true){
		if(this->boundary_waterlevel_stat_flag==true){
			if(*(this->value_boundary_waterlevel_stat)<0.0){
				Error msg=this->set_error(7);
				throw msg;
			}
		}
	}

}
//Connect instationary boundary curve of waterlevel to the profiles
void Hyd_River_Profile_Connection_Outflow::connect_instat_waterlevel_boundarycurve(Hyd_Instationary_Boundary *bound_curve){
	ostringstream cout;
	if(this->boundary_cond_waterlevel_flag==false){//no boundary
		return;
	}
	else if(this->boundary_cond_waterlevel_flag==true && this->boundary_waterlevel_stat_flag==true){//stat boundary
        if(this->h_outflow_is_given==true){
            Warning msg=this->set_warning(0);
            //reaction
            this->boundary_cond_waterlevel_flag=false;
            msg.output_msg(2);
        }
		return;
	}
	else if(this->boundary_cond_waterlevel_flag==true && this->boundary_waterlevel_stat_flag==false){//instat boundary
		if(*this->number_boundary_waterlevel_curve==bound_curve->get_curve_number()){//the right curve is found
			try{
				bound_curve->set_type(_hyd_bound_type::waterlevel_type);
			}
			catch(Error msg){
				throw msg;
			}
			this->ptr_boundarycurve_waterlevel=bound_curve;
			this->instatbound_waterlevel_found_flag=true;
			cout <<"Connection of the last Profile " << this->profile_number << " with waterlevel boundary curve " << bound_curve->get_curve_number() << " is found" << endl;
			Sys_Common_Output::output_hyd->output_txt(&cout);
			//check the instationary boundary curve, just for the inflow profile
			try{
				this->ptr_boundarycurve_waterlevel->check_boundary_curve_neg_null_values();

				this->ptr_boundarycurve_waterlevel->check_starting_time(0.0);
			}
			catch(Error msg){
				throw msg;
			}

			if(this->h_outflow_is_given==true){
				Warning msg=this->set_warning(0);
				//reaction
				this->boundary_cond_waterlevel_flag=false;
				msg.output_msg(2);
			}
		}
	}
}
//Get the instationary found flag: the curve is found for lateral inflow
bool Hyd_River_Profile_Connection_Outflow::get_instat_waterlevel_boundarycurve_found(void){
	if(this->boundary_cond_waterlevel_flag==false){//no boundary
		return true;
	}
	else if(this->boundary_cond_waterlevel_flag==true && this->boundary_waterlevel_stat_flag==true){//stat boundary
		return true;
	}
	else if(this->boundary_cond_waterlevel_flag==true && this->boundary_waterlevel_stat_flag==false){//instat boundary
		if(this->instatbound_waterlevel_found_flag==true){//it is found
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
//Get if a waterlevel boundary is set 
bool Hyd_River_Profile_Connection_Outflow::get_boundary_waterlevel_flag(void){
	return this->boundary_cond_waterlevel_flag;
}
//Output the members
void Hyd_River_Profile_Connection_Outflow::output_members(void){
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
	if(this->boundary_cond_waterlevel_flag==false){
		cout << " No explicit waterlevel boundary is found; a slope boundary is used (normal flow condition) " << endl;
	}
	else if (this->boundary_cond_waterlevel_flag==true && this->boundary_waterlevel_stat_flag==true){
		cout << " Waterlevel stationary boundary value          : " << *this->value_boundary_waterlevel_stat << label::m << endl; 
	}
	else if (this->boundary_cond_waterlevel_flag==true && this->boundary_waterlevel_stat_flag==false){
		cout << " Waterlevel instationary boundary curve-no.    : " << *this->number_boundary_waterlevel_curve<<endl; 
	}

	cout <<"POINTS"<< endl;
	this->typ_of_profile->output_members(&cout);

	Sys_Common_Output::output_hyd->output_txt(&cout,true);
	Sys_Common_Output::output_hyd->rewind_userprefix();

}
//Output the setted and calculated members
void Hyd_River_Profile_Connection_Outflow::output_setted_members(void){
	//set the prefix
	ostringstream prefix;
	prefix << "PROF_"<< this->profile_number<<"> ";
	Sys_Common_Output::output_hyd->set_userprefix(prefix.str());

	ostringstream cout;
	cout << "Distance to the upstream profile " <<  P(2) << FORMAT_FIXED_REAL << this->distance2upstream << label::m << endl;
	Sys_Common_Output::output_hyd->output_txt(&cout,true);

	if(this->boundary_cond_waterlevel_flag==true && this->boundary_waterlevel_stat_flag==false){
		cout << "Waterlevel boundary curve is found: " << functions::convert_boolean2string(this->instatbound_waterlevel_found_flag)<<" No. " << *this->number_boundary_waterlevel_curve<<endl; ;
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
void Hyd_River_Profile_Connection_Outflow::clone_profile(Hyd_River_Profile_Connection_Outflow *profile){
	_Hyd_River_Profile::clone_profile(profile);

    this->h_outflow_is_given=profile->h_outflow_is_given;
	this->boundary_cond_waterlevel_flag=profile->boundary_cond_waterlevel_flag;
	this->boundary_waterlevel_stat_flag=profile->boundary_waterlevel_stat_flag;
	if(profile->value_boundary_waterlevel_stat!=NULL){
		this->value_boundary_waterlevel_stat =new double;
		*this->value_boundary_waterlevel_stat=*profile->value_boundary_waterlevel_stat;
	}
	if(profile->number_boundary_waterlevel_curve!=NULL){
		this->number_boundary_waterlevel_curve =new int;
		*this->number_boundary_waterlevel_curve=*profile->number_boundary_waterlevel_curve;
	}


}
//Output the result members for each riverprofiletype at every timestep
void Hyd_River_Profile_Connection_Outflow::output_result_members_per_timestep(void){
	ostringstream cout;
	_Hyd_River_Profile::output_result_members_per_timestep(&cout);
	cout << W(10) <<  this->q_outflow +this->sum_coupling_q;
	cout << endl;
	Sys_Common_Output::output_hyd->output_txt(&cout,true); 
}
//Output the result members for each riverprofiletype at every timestep to file
void Hyd_River_Profile_Connection_Outflow::output_result_members_per_timestep(ofstream *file){
		_Hyd_River_Profile::output_result_members_per_timestep(file);
		*file  << W(12) << P(4) <<FORMAT_FIXED_REAL << this->q_outflow+this->sum_coupling_q<< endl;
}
//Output the result members for each riverprofiletype at every timestep to csv file
void Hyd_River_Profile_Connection_Outflow::output_result_members_per_timestep2csv(ofstream *file) {
	_Hyd_River_Profile::output_result_members_per_timestep2csv(file);
	*file << W(12) << P(4) << FORMAT_FIXED_REAL << this->q_outflow + this->sum_coupling_q << endl;

}
//Reset all coupling discharges; use it before syncronization
void Hyd_River_Profile_Connection_Outflow::reset_coupling_discharge(void){
	_Hyd_River_Profile::reset_coupling_discharge();
}
//Make the syncronistation of the profiles to a given time point
void Hyd_River_Profile_Connection_Outflow::make_syncronisation(const double time_point, _Hyd_River_Profile *downstream){
	this->set_h_outflow_by_boundary(time_point);
	this->typ_of_profile->calculate_maximum_values(time_point);
	this->calculate_maximum_values(time_point);

	//calculate the maximum discharge
	if(this->q_outflow>this->q_value_max.maximum){
		this->q_value_max.maximum=this->q_outflow;
		this->q_value_max.time_point=time_point;
	}

	this->calculate_max_h2basepoint();
	this->typ_of_profile->calculate_velocity_energy_head(time_point, downstream);
	this->typ_of_profile->set_old_time_point(time_point);
	//sum up the additional coupling q
	this->sum_coupling_q=this->q_1d_coupling+this->q_dikebreak_coupling_left+this->q_dikebreak_coupling_right+
		this->q_left_bank+this->q_right_bank+this->q_structure_coupling;

}
//Get the maximum change in a river section
void Hyd_River_Profile_Connection_Outflow::get_max_changes_section(double *max_change_h, double *max_change_v, const bool timecheck, const double timestep){

	if(timestep>0.0){
		//max h change
		*max_change_h=abs(this->h_outflow-this->h_coup_old)/timestep;
		if(timecheck==false){
			this->h_coup_old=this->h_outflow;
		}
	}

}
//Calculate the values for a hydrological balance of the corresponding river segment (upstream) for each syncronisation step
void Hyd_River_Profile_Connection_Outflow::calculate_hydrological_balance(const double time_point){
	_Hyd_River_Profile::calculate_hydrological_balance(time_point);
	
	this->outflow_volume_syncron=this->outflow_volume_syncron+this->delta_time*(this->q_outflow);

	this->watervolume_by_coupling=this->watervolume_by_coupling+this->delta_time*this->sum_coupling_q;

}
//Set the actual river discharge through this profile (q_outflow) by the given upstream 
void Hyd_River_Profile_Connection_Outflow::set_actual_river_discharge(_Hyd_River_Profile *upstream_profile){
	if(upstream_profile->get_profile_type()==_hyd_profile_types::RIVER_TYPE || upstream_profile->get_profile_type()==_hyd_profile_types::BRIDGE_TYPE){
		this->q_outflow=this->typ_of_profile->calculate_actual_discharge(upstream_profile, NULL, this->distance2upstream);
	}
}
//Get the actual discharge through this profile (q_outflow)
double Hyd_River_Profile_Connection_Outflow::get_calculated_river_discharge(void){
	return this->q_outflow;
}
//Get the actual discharge through this profile (q_outflow) and the coupling discharges
double Hyd_River_Profile_Connection_Outflow::get_actual_river_discharge(void){
	return this->q_outflow+this->sum_coupling_q;
}
//Set outflow local waterlevel (h_outflow) by the normal flow waterlevel
void Hyd_River_Profile_Connection_Outflow::set_h_outflow_flow(const double global_delta_z_upstream, const double waterlevel_upstream){
	this->h_normal_flow=waterlevel_upstream-global_delta_z_upstream;
	

	if(this->boundary_cond_waterlevel_flag==true){
		//nothing to do; the table values are setted by the syncronisation
		return;
	}
	else if(this->h_outflow_is_given==true){
		if(this->h_outflow_given>=this->h_normal_flow){
			this->h_normal_flow=this->h_outflow_given;
		}
		else{
			double quot=(this->h_normal_flow-this->h_outflow_given)/this->h_normal_flow;
			//make the mid of the waterlevels
			this->h_normal_flow=this->h_normal_flow*quot+this->h_outflow_given*(1.0-quot);
		}
	}

	//apply the normal flow waterlevel approach
	this->h_outflow=this->h_normal_flow;

	try{
		this->typ_of_profile->set_actuel_profilevalues_by_waterlevel(this->h_outflow+this->typ_of_profile->get_global_z_min());
	}
	catch(Error msg){
		ostringstream info;
		info << "Tried to set the normal flow condition for the outflow profile" << endl;
		msg.make_second_info(info.str());
		throw msg;
	}

}
//Reset the flag that the waterlevel (h_outflow) is given by a coupled model directly
void Hyd_River_Profile_Connection_Outflow::reset_h_outflow_is_given(void){
    this->h_outflow_is_given=false;
}
//Set the flag that the waterlevel (h_outflow) is given by a coupled model directly
void Hyd_River_Profile_Connection_Outflow::set_h_outflow_is_given(void){
    if(this->h_outflow_is_given==true){
        Error msg=this->set_error(6);
        throw msg;
    }

	if(this->boundary_cond_waterlevel_flag==true){
		Warning msg=this->set_warning(0);
		//reaction
		this->boundary_cond_waterlevel_flag=false;
		msg.output_msg(2);
	}
	this->h_outflow_is_given=true;
}
//Get the flag that the waterlevel (h_outflow) is given by a coupled model directly
bool Hyd_River_Profile_Connection_Outflow::get_h_outflow_is_given(void){
	return this->h_outflow_is_given;
}
//Set the waterlevel (h_outflow) by a coupled model directly as a global waterlevel
void Hyd_River_Profile_Connection_Outflow::set_h_outflow_global(const double h_global){
	this->h_outflow_given=h_global-this->typ_of_profile->get_global_z_min();

	if(this->h_outflow_is_given==false){

		return;
	}
	else{
		if(this->h_outflow_given<0.0){
			this->h_outflow_given=0.0;
			this->h_outflow=this->h_outflow_given;
		}
		else{
			this->h_outflow=this->h_outflow_given;

			//check the setted outflow value, if it is not equal to any river profile zmin value
			this->h_outflow=this->check_hboundary2zmin_profiles(this->h_outflow+this->typ_of_profile->get_global_z_min());

		}
	}
}
//Get the actual global waterlevel (s_value); here the h_outflow condition related to the global z coordinate is returned
double Hyd_River_Profile_Connection_Outflow::get_actual_global_waterlevel(void){
	return this->h_outflow+this->typ_of_profile->get_global_z_min();
}
//Get the watervolume which flows out of the system for a hydrological balance
double Hyd_River_Profile_Connection_Outflow::get_outflow_volume(void){
	return this->outflow_volume+this->watervolume_by_coupling;	
}
//Get the watervolume difference at a direct coupling 2d-1d between solver calculated inflow volume by a setted inflow waterlevel and the coupled inflow volume 
double Hyd_River_Profile_Connection_Outflow::get_watervolume_difference_direct_coupling(void){
	if(this->h_outflow_is_given==false){
		//no direct coupling=>no difference	
		return 0.0;
	}
	else{	
		return abs(this->outflow_volume_syncron-this->outflow_volume);
	}
}
//Calculate the values for a hydrological balance for the outflow discharge; it is done every calculation step
void Hyd_River_Profile_Connection_Outflow::calculate_hydrological_balance_outflow(const double time_point){
	double delta_time=time_point-this->old_calc_time_point;
	if(abs(delta_time)<constant::sec_epsilon){
		return;
	}
	if(delta_time>=0.0){
		this->old_calc_time_point=time_point;
		this->outflow_volume=this->outflow_volume+(this->old_discharge*(1.0/3.0)+this->get_calculated_river_discharge()*(2.0/3.0))*delta_time;
		this->old_delta_t=delta_time;
		this->old_discharge_2=this->old_discharge;
		this->old_discharge=(this->old_discharge*(1.0/3.0)+this->get_calculated_river_discharge()*(2.0/3.0));
	}
	else{
		this->outflow_volume=this->outflow_volume-this->old_discharge*this->old_delta_t;
		this->old_calc_time_point=this->old_calc_time_point-this->old_delta_t;
		delta_time=time_point-this->old_calc_time_point;
		this->outflow_volume=this->outflow_volume+(this->old_discharge_2*(1.0/3.0)+this->get_calculated_river_discharge()*(2.0/3.0))*delta_time;
		this->old_delta_t=delta_time;
		this->old_discharge=(this->old_discharge_2*(1.0/3.0)+this->get_calculated_river_discharge()*(2.0/3.0));
		this->old_calc_time_point=time_point;
	}
	
}
//Set a pointer to the inbetween profiles of the river and the inflow-profile and the number of inbetween profiles (see check_hboundary2zmin_profiles(const double h_boundary))
void Hyd_River_Profile_Connection_Outflow::set_ptr_riverprofiles(Hyd_River_Profile_Connection_Standard *ptr_inbetween_prof, const int number_inbetween_prof, Hyd_River_Profile_Connection_Inflow *ptr_inflow_prof){
	this->ptr_inbetween_profiles=ptr_inbetween_prof;
	this->number_inbetween_profiles=number_inbetween_prof;
	this->ptr_inflow_profiles=ptr_inflow_prof;
}
//Reset the hydrological balance value and the maximum values
void Hyd_River_Profile_Connection_Outflow::reset_hydrobalance_maxvalues(void){
	_Hyd_River_Profile::reset_hydrobalance_maxvalues();

	this->q_outflow=0.0;
	this->sum_coupling_q=0.0;
	this->old_calc_time_point=0.0;
	this->old_delta_t=0.0;
	this->old_discharge=0.0;
	this->old_discharge_2=0.0;
	this->outflow_volume=0.0;
	this->h_normal_flow=0.0;
	this->h_outflow_given=0.0;
	this->max_outflow_q.maximum=0.0;
	this->max_outflow_q.time_point=0.0;
	this->outflow_volume_syncron=0.0;
	this->watervolume_by_coupling=0.0;
	this->h_outflow=0.0;
	this->h_coup_old=0.0;

}
//Reset the boundary data of the river profile
void Hyd_River_Profile_Connection_Outflow::reset_boundary_data(void){
	this->boundary_cond_waterlevel_flag=false;
	this->boundary_waterlevel_stat_flag=false;
	this->value_boundary_waterlevel_stat=NULL;
	this->number_boundary_waterlevel_curve=NULL;
	this->ptr_boundarycurve_waterlevel=NULL;
	this->instatbound_waterlevel_found_flag=false;
	
	
}
//Get boundary condition is applied
bool Hyd_River_Profile_Connection_Outflow::boundary_is_applied(void){
	if(this->boundary_cond_waterlevel_flag==true){
		return true;
	}
	else{
		return false;
	}
}
//Get q-value
double Hyd_River_Profile_Connection_Outflow::get_Q(void) {

	return this->q_outflow + this->sum_coupling_q;
}
//________________
//private
//decide in the different keyvalues in the profile file
void Hyd_River_Profile_Connection_Outflow::decide_keyvalues_file(const string key, string buffer, int *found_counter){
	
	try{
		_Hyd_River_Profile::decide_keyvalues_file(key, buffer, found_counter);

	}catch(Error msg){
		throw msg;
	}
	
	stringstream stream;

	 if(key==hyd_label::BoundaryWaterlevelCondition){
		this->get_keyvalues_file(&stream, buffer);
		this->boundary_cond_waterlevel_flag=_Hyd_Parse_IO::transform_string2boolean(stream.str());
		if(this->boundary_cond_waterlevel_flag==true){
			this->must_found_number=this->must_found_number+2;
		}
	}
	else if(key==hyd_label::BoundaryWaterlevelStationary && this->boundary_cond_waterlevel_flag==true){
		this->get_keyvalues_file(&stream, buffer);
		this->boundary_waterlevel_stat_flag=_Hyd_Parse_IO::transform_string2boolean(stream.str());
		(*found_counter)++;
	}
	else if(key==hyd_label::BoundaryWaterlevelValue && this->boundary_cond_waterlevel_flag==true){
		this->get_keyvalues_file(&stream, buffer);
		double value;
		stream >> value;
		this->decide_boundary_waterlevel_conditions(value);
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
//Decide which boundary conditions are choosen for a waterlevel condition (boundary value is given in [m])
void Hyd_River_Profile_Connection_Outflow::decide_boundary_waterlevel_conditions(const double nr_value){
	if(this->boundary_cond_waterlevel_flag==false){//no boundary condition
		return;
	}
	else if(this->boundary_cond_waterlevel_flag==true && this->boundary_waterlevel_stat_flag==true){
        this->value_boundary_waterlevel_stat=new double;
		*this->value_boundary_waterlevel_stat=nr_value;
	}
	else if(this->boundary_cond_waterlevel_flag==true && this->boundary_waterlevel_stat_flag==false){
		this->number_boundary_waterlevel_curve=new int;
		*this->number_boundary_waterlevel_curve=(int)nr_value;
	}
}
//Check the connection profile type
void Hyd_River_Profile_Connection_Outflow::check_connection_type_profile(void){
	if(this->connection!=_hyd_connection_types::OUTFLOW_CONN){
		Error msg=this->set_error(2);
		throw msg;
	}
	if(this->type!=_hyd_profile_types::RIVER_TYPE){
		Error msg=this->set_error(3);
		throw msg;
	}
}
//Set the outflow local waterlevel (h_outflow) correspondig to the stationary/instationary flag and the point in time by a boundary condition
void Hyd_River_Profile_Connection_Outflow::set_h_outflow_by_boundary(const double time_point){
	
	
	if(this->boundary_cond_waterlevel_flag==false){
		return;
	}
	else if(this->h_outflow_is_given==true){
		return;
	}
	else{
		//stationary
		if(this->boundary_waterlevel_stat_flag==true){
			this->h_outflow_given=*this->value_boundary_waterlevel_stat;
		}
		//instationary
		else if(this->boundary_waterlevel_stat_flag==false){
			this->h_outflow_given=this->ptr_boundarycurve_waterlevel->calculate_actuel_boundary_value(time_point);
		}
		//set the value
		try{
			this->h_outflow=this->h_outflow_given;
			//check the setted outflow value, if it is not equal to any river profile zmin value
			this->h_outflow=this->check_hboundary2zmin_profiles(this->h_outflow+this->typ_of_profile->get_global_z_min());

			this->typ_of_profile->set_actuel_profilevalues_by_waterlevel(this->h_outflow+this->typ_of_profile->get_global_z_min());
		}
		catch(Error msg){
			ostringstream info;
			info << "Tried to set outflow condition by a waterlevel boundary condition" << endl;
			msg.make_second_info(info.str());
			throw msg;
		}
	}
}
//calculate maximum values 
void Hyd_River_Profile_Connection_Outflow::calculate_maximum_values(const double time_point){
	//outflow q
	double q_buff=this->get_actual_river_discharge();
	if(q_buff>this->max_outflow_q.maximum){
		this->max_outflow_q.maximum=q_buff;
		this->max_outflow_q.time_point=time_point;
	}
}
//Check if the zmin values of the river profiles are not equal to the setted h-boundary; if they are equal h-boundary is reduced by 0.001 m
double Hyd_River_Profile_Connection_Outflow::check_hboundary2zmin_profiles(const double h_boundary_global){
	double h_buff=h_boundary_global;
	if(this->ptr_inbetween_profiles==NULL && this->ptr_inflow_profiles==NULL){
		return h_buff-this->typ_of_profile->get_global_z_min();
	}
	//no inbetween profiles
	else if(this->ptr_inbetween_profiles==NULL && this->ptr_inflow_profiles!=NULL){
		//check the heights for the inflow profile
		if(abs(h_buff-this->ptr_inflow_profiles->typ_of_profile->get_global_z_min())<=constant::dry_hyd_epsilon){
			h_buff=h_buff-constant::dry_hyd_epsilon*10.0;
		}
		else{
			return h_buff-this->typ_of_profile->get_global_z_min();
		}
	}
	else if(this->ptr_inbetween_profiles!=NULL && this->ptr_inflow_profiles!=NULL){
		bool reduced_flag=false;
		do{
			bool stop_flag=false;
			int counter=this->number_inbetween_profiles-1;
			reduced_flag=false;
			//check the heights for the inbetween profiles
			while(stop_flag==false && counter >=0){
				if(abs(h_buff-this->ptr_inbetween_profiles[counter].typ_of_profile->get_global_z_min())<=constant::dry_hyd_epsilon){
					h_buff=h_buff-constant::dry_hyd_epsilon*10.0;
					stop_flag=true;
					reduced_flag=true;
				}
				else if(h_buff<this->ptr_inbetween_profiles[counter].typ_of_profile->get_global_z_min()){
					stop_flag=true;
					reduced_flag=false;	
				}
				counter--;
			}//end while

			//check the heights for the inflow profile
			if(abs(h_buff-this->ptr_inflow_profiles->typ_of_profile->get_global_z_min())<=constant::dry_hyd_epsilon){
				h_buff=h_buff-constant::dry_hyd_epsilon*10.0;
				stop_flag=true;
				reduced_flag=true;
			}
		}while(reduced_flag==true);//end do
	}
	return h_buff-this->typ_of_profile->get_global_z_min();
}
//Transfer the river profile boundary data to a database
void Hyd_River_Profile_Connection_Outflow::transfer_boundary_data2database(QSqlDatabase *ptr_database, const int glob_prof_id){
	if(this->boundary_cond_waterlevel_flag==false){
		return;
	}
	//mysql query with the table_model
	QSqlTableModel model(0,*ptr_database);

	//the table is set (the name and the column names) and allocated
	try{
		_Hyd_River_Profile::set_profile_boundary_table(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}
	//give the table name to the query model
	model.setTable((_Hyd_River_Profile::boundary_table->get_table_name()).c_str());

	//give the table name to the query model
	model.setTable((_Hyd_River_Profile::boundary_table->get_table_name()).c_str());
	//insert lateral boundary condition
	//insert row
	model.insertRows(0, 1);
						
	//insert data
	model.setData(model.index(0,model.record().indexOf((_Hyd_River_Profile::boundary_table->get_column_name(hyd_label::profdata_glob_id)).c_str())),glob_prof_id);
	model.setData(model.index(0,model.record().indexOf((_Hyd_River_Profile::boundary_table->get_column_name(label::areastate_id)).c_str())), this->system_id.area_state);
	model.setData(model.index(0,model.record().indexOf((_Hyd_River_Profile::boundary_table->get_column_name(label::measure_id)).c_str())), this->system_id.measure_nr);
	model.setData(model.index(0,model.record().indexOf((_Hyd_River_Profile::boundary_table->get_column_name(label::applied_flag)).c_str())), true);
	model.setData(model.index(0,model.record().indexOf((_Hyd_River_Profile::boundary_table->get_column_name(hyd_label::sz_bound_id)).c_str())), this->hyd_sz.get_id());
	model.setData(model.index(0,model.record().indexOf((_Hyd_River_Profile::boundary_table->get_column_name(hyd_label::bounddata_stat)).c_str())), this->boundary_waterlevel_stat_flag);
	if(this->boundary_waterlevel_stat_flag==true){
		model.setData(model.index(0,model.record().indexOf((_Hyd_River_Profile::boundary_table->get_column_name(hyd_label::bounddata_value)).c_str())), *this->value_boundary_waterlevel_stat);
	}
	else{
		model.setData(model.index(0,model.record().indexOf((_Hyd_River_Profile::boundary_table->get_column_name(hyd_label::bounddata_value)).c_str())), *this->number_boundary_waterlevel_curve);
	}	
	model.setData(model.index(0,model.record().indexOf((_Hyd_River_Profile::boundary_table->get_column_name(hyd_label::bounddata_type)).c_str())), Hyd_Instationary_Boundary::transfrom_instatboundtype2txt(_hyd_bound_type::waterlevel_type).c_str());

	//submit it to the datbase
	Data_Base::database_submit(&model);

	if(model.lastError().isValid()){
		Warning msg=this->set_warning(2);
		ostringstream info;
		info << "Table Name                : " << _Hyd_River_Profile::boundary_table->get_table_name() << endl;
		info << "Table error info          : " << model.lastError().text().toStdString() << endl;
		info <<"Trying to insert the waterlevel boundary condition" << endl;
		msg.make_second_info(info.str());
		msg.output_msg(2);	
	}
}
//set the error
Error Hyd_River_Profile_Connection_Outflow::set_error(const int err_type){
		string place="Hyd_River_Profile_Connection_Outflow::";
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
			help="Check the Profile in the profile file";
			type=1;
			break;
		case 2://the last profil is not a inflow type
			place.append("check_connection_type_profile(void)");
			reason="The last profile must be an outflow profile";
			help="Check the connection type of the last profile;\n check also, if the setted number of profiles in the global file is corresponding to the given profiles";
			type=15;
			break;
		case 3://the last profil have to be a river profile 
			place.append("check_connection_type_profile(void)");
			reason="The outflow profile must be a river profile";
			help="Check the profile type of the outflow profile";
			type=15;
			break;
		case 4://the before last profile have to be of a river type
			place.append("check_profiles(_Hyd_River_Profile *upstream_profile, _Hyd_River_Profile *upupstream_profile)");
			reason="The profile before the outflow profile must be a river profile if the normal flow boundary condition is chosen";
			help="Check the profile type of the profile before the outflow profile";
			type=15;
			break;
		case 5://there is no slope for the normal flow condition in the before the outflow profile
			place.append("check_profiles(_Hyd_River_Profile *upstream_profile, _Hyd_River_Profile *upupstream_profile)");
			reason="There must be a slope in the last two river sections";
			help="Check the slope between the outflow profile i, the profile before i-1 and i-2";
			type=15;
			break;
		case 6://it is not possible to set two couplings
			place.append("set_h_outflow_is_given(void)");
			reason="It is not possible to set two couplings to the river outflow profile";
			help="Check the couplings to other river models or to the coast model";
			type=16;
			break;
		case 7:
			place.append("check_profiles(_Hyd_River_Profile *upstream_profile, _Hyd_River_Profile *upupstream_profile)");
			reason="The given stationary boundary waterlevel is <0.0";
			help="Check the outflow profile of the river";
			type=15;
			break;
		case 8://problems with database query
			place.append("input_..._per_database(...)") ;
			reason="Can not submit the query to the database";
			help="Check the query error";
			type=6;	
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
Warning Hyd_River_Profile_Connection_Outflow::set_warning(const int warn_type){
	string place="Hyd_River_Profile_Connection_Outflow::";
		string help;
		string reason;
		string reaction;
		int type=0;
		Warning msg;
		stringstream info;

	switch (warn_type){
		case 0://Type of material value does not match
			place.append("set_h_outflow_is_given(void)/connect_instat_waterlevel_boundarycurve(Hyd_Instationary_Boundary *bound_curve)") ;
			reason="The connected boundary values for the waterlevel are not used; a coupling is set";
			reaction="The boundary condition flag for the waterlevel is set to false";
			help= "Do not set any boundary condition in a case of coupling the outflow profile to a river- or coast model";
			type=10;	
			break;
		case 1://input datas can not submitted
			place.append("transfer_profile_members2database(QSqlDatabase *ptr_database, const int rv_number)");
			reason="Can not submit the data of the river profile to the database";
			help="Check the database";
			type=2;
			break;
		case 2://input datas can not submitted
			place.append("transfer_boundary_data2database(QSqlDatabase *ptr_database, const int glob_prof_id)");
			reason="Can not submit the data of the river outflow profile to the database";
			help="Check the database";
			type=2;
			break;
		case 3://there are multiple data sets to the given profile
			place.append("input_..._data_per_database(QSqlDatabase *ptr_database, const int glob_prof_id)");
			reason="There are multiple data sets (see table name) to the given global profile id";
			help="Check the database";
			reaction="The first found data set is taken";
			info << "Areastate/Measureno.   : " << this->system_id.area_state <<"/"<< this->system_id.measure_nr << endl; 
			type=2;
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
