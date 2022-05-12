//#include "_Hyd_River_Profile_Type.h"
#include "Hyd_Headers_Precompiled.h"

//init static members
Tables *_Hyd_River_Profile_Type::profile_point_table=NULL;

//constructor
_Hyd_River_Profile_Type::_Hyd_River_Profile_Type(void): increment_max_height(2.0){
	this->number_points=0;
	this->global_z_min=0.0;
	this->global_x_zmin=0.0;
	this->global_y_zmin=0.0;
	this->global_z_max=0.0;
	this->global_x_zmax=0.0;
	this->global_y_zmax=0.0;
	this->points=NULL;
	this->zmax_marker=0;
	this->zmin_marker=0;
	this->number_table_points=0;
	this->waterlevel_line=NULL;
	this->plot=NULL;


	this->profile_width=0.0;

	//result variables per time step
	this->h_value=0.0;
	this->v_energy=0.0;
	this->v_energy_old=0.0;
	this->v_energy_old_old=0.0;
	this->delta_t_v_energy=0.0;
	this->gradient=0.001;
	this->s_value=0.0;
	this->s_value_max.maximum=0.0;
	this->s_value_max.time_point=0.0;
	this->h_value_max.maximum=0.0;
	this->h_value_max.time_point=0.0;
	this->v_value=0.0;
	this->v_value_max.maximum=0.0;
	this->v_value_max.time_point=0.0;

	this->width = 0.0;;
	this->width_value_max.maximum=0.0;
	this->width_value_max.maximum = 0.0;


	this->froude_no=0.0;

	this->was_dry_flag=false;
	this->dry_duration=0.0;
	this->wet_duration=0.0;
	this->old_time_point=0.0;
	this->old_energy_time=0.0;


	this->solved_area=0.0;
	this->area2calc=0.0;
	this->stab_discharge=0.0;

	this->user_setting.explict_v_is_used=false;
	this->user_setting.use_interface=false;

	this->zero_outflow_reduction_flag=false;

}
//destructor
_Hyd_River_Profile_Type::~_Hyd_River_Profile_Type(void){
	if(this->points!=NULL){
		delete []this->points;
		this->points=NULL;
		//count memory
		Sys_Memory_Count::self()->minus_mem(sizeof(Hyd_River_Profile_Point)*this->number_points, _sys_system_modules::HYD_SYS);
	}
	
	if(this->waterlevel_line!=NULL){
		delete this->waterlevel_line;
		this->waterlevel_line=NULL;	
	}
	if(this->plot!=NULL){
		delete this->plot;
		this->plot=NULL;	
	}
}
//________________________
//public
//set number of points
void _Hyd_River_Profile_Type::set_number_points(const int number){
	this->number_points=number;
	try{
		this->allocate_points();
	}
	catch(Error msg){
		throw msg;
	}
}
//Set a profile specific value (needed for the bridge type)
void _Hyd_River_Profile_Type::set_profile_specific_value(const _hyd_bridge_values data){
	_hyd_bridge_values buff;
	buff=data;
}
//input the mebers per file
void _Hyd_River_Profile_Type::input_members(QFile *profile_file, int *line_counter, const string name, const int number){

	int point_counter=0;
	try{
		do{
			if(this->points[point_counter].input_members(profile_file, &point_counter, line_counter)==false){
				break;	
			};

		}while(profile_file->atEnd() !=true && point_counter<this->number_points);
	}
	catch(Error msg){
		ostringstream info;
		info << "Error occurs near line: "<< *line_counter << endl;
		msg.make_second_info(info.str());
		throw msg;	
	}

	//check if all points ar found
	if(point_counter!=this->number_points){
		Error msg=this->set_error(1);
		ostringstream info;
		info << "Number of profile points found " << point_counter << endl;
		info << "Error occurs near line: "<< *line_counter << endl;
		msg.make_second_info(info.str());
		throw msg;
	}

	//set the zmin coordinates
	this->set_zmin_coordinates();
	//set the zmax coordinates
	this->set_zmax_coordinates();
	//calculate the local values
	this->calculate_local_coordinates();
	//calculate the midpoint
	this->calculate_global_midpoint_x_y();
	this->calculate_global_minpoint_x_y();
	//check the points
	try{	
		this->check_points(name, number);
	}
	catch(Error msg){
		throw msg;
	}

}
//Input the river profile point data from a selection of a database table
void _Hyd_River_Profile_Type::input_members(const QSqlQueryModel *query_results, const string name, const int number){


	//read out the results for each p
	for(int i=0; i< this->number_points; i++){
		this->points[i].input_members_per_database(query_results, i);	
	}

	//set the zmin coordinates
	this->set_zmin_coordinates();
	//set the zmax coordinates
	this->set_zmax_coordinates();
	//calculate the local values
	this->calculate_local_coordinates();
	//calculate the midpoint
	this->calculate_global_midpoint_x_y();
	this->calculate_global_minpoint_x_y();
	//check the points
	try{	
		this->check_points(name, number);
	}
	catch(Error msg){
		throw msg;
	}

}
//Transfer the river profile point data to a database
void _Hyd_River_Profile_Type::transfer_profile_points2database(QSqlDatabase *ptr_database, const int prof_id){
	//mysql query with the query_model
	QSqlQuery query_buff(*ptr_database);

	//the table is set (the name and the column names) and allocated
	try{
		_Hyd_River_Profile_Type::set_profile_point_table(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}

	int glob_id=_Hyd_River_Profile_Type::profile_point_table->maximum_int_of_column(_Hyd_River_Profile_Type::profile_point_table->get_column_name(hyd_label::profpoint_glob_id),ptr_database)+1;
	
	ostringstream query_header;
	query_header << "INSERT INTO  " << _Hyd_River_Profile_Type::profile_point_table->get_table_name();
	query_header <<" ( ";
	query_header << _Hyd_River_Profile_Type::profile_point_table->get_column_name(hyd_label::profpoint_glob_id) <<" , ";
	query_header << _Hyd_River_Profile_Type::profile_point_table->get_column_name(hyd_label::profdata_prof_id) <<" , ";
	query_header << _Hyd_River_Profile_Type::profile_point_table->get_column_name(hyd_label::profpoint_id) <<" , ";
	query_header << _Hyd_River_Profile_Type::profile_point_table->get_column_name(hyd_label::profpoint_x_co) <<" , ";
	query_header << _Hyd_River_Profile_Type::profile_point_table->get_column_name(hyd_label::profpoint_y_co) <<" , ";
	query_header << _Hyd_River_Profile_Type::profile_point_table->get_column_name(hyd_label::profpoint_z_co) <<" , ";
	query_header << _Hyd_River_Profile_Type::profile_point_table->get_column_name(hyd_label::profpoint_dist) <<" , ";
	query_header << _Hyd_River_Profile_Type::profile_point_table->get_column_name(hyd_label::profpoint_mat_id) <<" , ";
	query_header << _Hyd_River_Profile_Type::profile_point_table->get_column_name(hyd_label::profpoint_ch_id) <<" , ";
	query_header << _Hyd_River_Profile_Type::profile_point_table->get_column_name(hyd_label::profpoint_point) ;
	query_header <<" ) ";

	query_header << " VALUES ";
	ostringstream query_data;
	


	for(int i=0; i< this->number_points; i++){
		query_data << " ( ";
		query_data << glob_id <<" , ";
		query_data << prof_id <<" , ";
		query_data << i<<" , ";
		query_data << FORMAT_FIXED_REAL << P(8);
		query_data << this->points[i].get_global_x_y_coordinates().get_xcoordinate() <<" , ";
		query_data << this->points[i].get_global_x_y_coordinates().get_ycoordinate() <<" , ";
		query_data.clear();
		query_data << this->points[i].get_global_z_coordinate() <<" , ";
		query_data << this->points[i].get_xcoordinate() <<" , ";
		query_data << this->points[i].mat_type <<" , ";
		query_data <<this->points[i].identity <<" , ";
		query_data <<Geo_Point::get_point2sql_string(this->points[i].get_global_x_y_coordinates().get_xcoordinate(), this->points[i].get_global_x_y_coordinates().get_ycoordinate());
		query_data <<" ) " << " ,";
		glob_id++;
			
	}


	string buff;
	query_header<< query_data.str();
	buff = query_header.str();
	buff.erase(buff.length()-1);
	Data_Base::database_request(&query_buff, buff, ptr_database);
	if(query_buff.lastError().isValid()){
		Warning msg=this->set_warning(1);
		ostringstream info;
		info << "Table Name                : " << _Hyd_River_Profile_Type::profile_point_table->get_table_name() << endl;
		info << "Table error info          : " << query_buff.lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		msg.output_msg(2);	
	}

}
//Set the database table for the profile points data: it sets the table name and the name of the columns and allocate them (static)
void _Hyd_River_Profile_Type::set_profile_point_table(QSqlDatabase *ptr_database){
	if(_Hyd_River_Profile_Type::profile_point_table==NULL){
		//make specific input for this class
		const string tab_id_name=hyd_label::tab_rvprof_points;

		string tab_id_col[10];
		tab_id_col[0]=hyd_label::profdata_prof_id;
		tab_id_col[1]=hyd_label::profpoint_id;
		tab_id_col[2]=hyd_label::profpoint_x_co;
		tab_id_col[3]=hyd_label::profpoint_y_co;
		tab_id_col[4]=hyd_label::profpoint_z_co;
		tab_id_col[5]=hyd_label::profpoint_dist;
		tab_id_col[6]=hyd_label::profpoint_mat_id;
		tab_id_col[7]=hyd_label::profpoint_ch_id;
		tab_id_col[8]=hyd_label::profpoint_glob_id;
		tab_id_col[9]=hyd_label::profpoint_point;


		try{
			_Hyd_River_Profile_Type::profile_point_table= new Tables(tab_id_name, tab_id_col, sizeof(tab_id_col)/sizeof(tab_id_col[0]));
			_Hyd_River_Profile_Type::profile_point_table->set_name(ptr_database, _sys_table_type::hyd);
		}
		catch(bad_alloc& t){
			Error msg;
			msg.set_msg("_Hyd_River_Profile_Type::set_profile_point_table(QSqlDatabase *ptr_database)","Can not allocate the memory", "Check the memory", 10, false);
			ostringstream info;
			info<< "Info bad alloc: " << t.what() << endl;
			msg.make_second_info(info.str());
			throw msg;	
		}
		catch(Error msg){
			_Hyd_River_Profile_Type::close_table();
			throw msg;
		}
	}

}
//Create the database table for the profile points data (static)
void _Hyd_River_Profile_Type::create_profile_point_table(QSqlDatabase *ptr_database){
	if(_Hyd_River_Profile_Type::profile_point_table==NULL){
		ostringstream cout;
		cout << "Create river profile type point database table..." << endl ;
		Sys_Common_Output::output_hyd->output_txt(&cout);
		//make specific input for this class
		const string tab_name=hyd_label::tab_rvprof_points;
		const int num_col=10;
		_Sys_data_tab_column tab_col[num_col];
		//init
		for(int i=0; i< num_col; i++){
			tab_col[i].key_flag=false;
			tab_col[i].unsigned_flag=false;
			tab_col[i].primary_key_flag=false;
		}
		tab_col[0].name=hyd_label::profpoint_glob_id;
		tab_col[0].type=sys_label::tab_col_type_int;
		tab_col[0].unsigned_flag=true;
		tab_col[0].primary_key_flag=true;

		
		tab_col[1].name=hyd_label::profdata_prof_id;
		tab_col[1].type=sys_label::tab_col_type_int;
		tab_col[1].unsigned_flag=true;
		tab_col[1].key_flag=true;

		tab_col[2].name=hyd_label::profpoint_id;
		tab_col[2].type=sys_label::tab_col_type_int;
		tab_col[2].unsigned_flag=true;

		tab_col[3].name=hyd_label::profpoint_x_co;
		tab_col[3].type=sys_label::tab_col_type_double;
		tab_col[3].default_value="0.0";

		tab_col[4].name=hyd_label::profpoint_y_co;
		tab_col[4].type=sys_label::tab_col_type_double;
		tab_col[4].default_value="0.0";

		tab_col[5].name=hyd_label::profpoint_z_co;
		tab_col[5].type=sys_label::tab_col_type_double;
		tab_col[5].default_value="0.0";

		tab_col[6].name=hyd_label::profpoint_dist;
		tab_col[6].type=sys_label::tab_col_type_double;
		tab_col[6].default_value="0.0";

		tab_col[7].name=hyd_label::profpoint_mat_id;
		tab_col[7].type=sys_label::tab_col_type_int;
		tab_col[7].unsigned_flag=true;

		tab_col[8].name=hyd_label::profpoint_ch_id;
		tab_col[8].type=sys_label::tab_col_type_int;
		tab_col[8].unsigned_flag=true;

		tab_col[9].name=hyd_label::profpoint_point;
		tab_col[9].type=sys_label::tab_col_type_point;
		


		try{
			_Hyd_River_Profile_Type::profile_point_table= new Tables();
			if(_Hyd_River_Profile_Type::profile_point_table->create_non_existing_tables(tab_name, tab_col, num_col,ptr_database, _sys_table_type::hyd)==false){
				cout << " Table exists" << endl ;
				Sys_Common_Output::output_hyd->output_txt(&cout);
			};
		}
		catch(bad_alloc& t){
			Error msg;
			msg.set_msg("_Hyd_River_Profile_Type::create_profile_point_table(QSqlDatabase *ptr_database)","Can not allocate the memory", "Check the memory", 10, false);
			ostringstream info;
			info<< "Info bad alloc: " << t.what() << endl;
			msg.make_second_info(info.str());
			throw msg;	
		}
		catch(Error msg){
			_Hyd_River_Profile_Type::close_table();
			throw msg;
		}

	_Hyd_River_Profile_Type::close_table();
	}

}
//Delete all data in the database of the profile points data (static) 
void _Hyd_River_Profile_Type::delete_data_in_table(QSqlDatabase *ptr_database){
	//the table is set (the name and the column names) and allocated
	try{
		_Hyd_River_Profile_Type::set_profile_point_table(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}
	//delete the table
	_Hyd_River_Profile_Type::profile_point_table->delete_data_in_table(ptr_database);

}
//Close and delete the database of the profile points data (static)
void _Hyd_River_Profile_Type::close_table(void){
	if(_Hyd_River_Profile_Type::profile_point_table!=NULL){
		delete _Hyd_River_Profile_Type::profile_point_table;
		_Hyd_River_Profile_Type::profile_point_table=NULL;
	}

}
//Select and count the river profiles points in the database table and return the number (static)
int _Hyd_River_Profile_Type::select_profile_points_in_database(QSqlQueryModel *query_result, QSqlDatabase *ptr_database, const int glob_id){
	int number=0;
	try{
		_Hyd_River_Profile_Type::set_profile_point_table(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}
	ostringstream test_filter;
	test_filter<< "Select ";
	test_filter << _Hyd_River_Profile_Type::profile_point_table->get_column_name(hyd_label::profpoint_x_co) << " , ";
	test_filter << _Hyd_River_Profile_Type::profile_point_table->get_column_name(hyd_label::profpoint_y_co) << " , ";
	test_filter << _Hyd_River_Profile_Type::profile_point_table->get_column_name(hyd_label::profpoint_z_co) << " , ";
	test_filter << _Hyd_River_Profile_Type::profile_point_table->get_column_name(hyd_label::profpoint_dist) << " , ";
	test_filter << _Hyd_River_Profile_Type::profile_point_table->get_column_name(hyd_label::profpoint_mat_id) << " , ";
	test_filter << _Hyd_River_Profile_Type::profile_point_table->get_column_name(hyd_label::profpoint_ch_id) ;
	test_filter << " from " << _Hyd_River_Profile_Type::profile_point_table->get_table_name();
	test_filter << " where ";
	test_filter << _Hyd_River_Profile_Type::profile_point_table->get_column_name(hyd_label::profdata_prof_id) << " = "<< glob_id;
	test_filter << " order by " << _Hyd_River_Profile_Type::profile_point_table->get_column_name(hyd_label::profpoint_id);

	Data_Base::database_request(query_result, test_filter.str(), ptr_database);

	//check the request
	if(query_result->lastError().isValid()){
		Error msg;
		msg.set_msg("_Hyd_River_Profile_Type::select_relevant_profiles_in_database(QSqlTableModel *query_result, const int glob_id)","Invalid database request", "Check the database", 2, false);
		ostringstream info;
		info << "Table Name      : " << _Hyd_River_Profile_Type::profile_point_table->get_table_name() << endl;
		info << "Table error info: " << query_result->lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
	
	number= query_result->rowCount();

	return number;
}
//Delete the point data in the database table for a given global profile id (static)
void _Hyd_River_Profile_Type::delete_points_by_rv_profile(QSqlDatabase *ptr_database, const int id){
		QSqlQueryModel results;

	try{
		_Hyd_River_Profile_Type::set_profile_point_table(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}
	ostringstream test_filter;

	test_filter<< "Delete ";
	test_filter << " from " << _Hyd_River_Profile_Type::profile_point_table->get_table_name();
	test_filter  << " WHERE ";
	test_filter << _Hyd_River_Profile_Type::profile_point_table->get_column_name(hyd_label::profdata_prof_id) << " = "  << id;

	Data_Base::database_request(&results, test_filter.str(), ptr_database);

	//check the request
	if(results.lastError().isValid()){
		Error msg;
		msg.set_msg("_Hyd_River_Profile_Type::delete_points_by_rv_profile(QSqlDatabase *ptr_database, const int id)","Invalid database request", "Check the database", 2, false);
		ostringstream info;
		info << "Table Name      : " << _Hyd_River_Profile_Type::profile_point_table->get_table_name() << endl;
		info << "Table error info: " << results.lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}

}
//Get the number of profile points
int _Hyd_River_Profile_Type::get_number_points(void){
	return this->number_points;
}
//Get the points of the left bank
int _Hyd_River_Profile_Type::get_points_left_bank(double **x, double **z){
	int no=0;
	for(int i=0; i< this->number_points; i++){
		if(this->points[i].get_channel_type()==1){
			no++;
		}
	}
	try{
		(*x)=new double[no];
		(*z)=new double[no];

	}
	catch(bad_alloc &t){
		Error msg=this->set_error(8);
		ostringstream info;
		info<< "Info bad alloc: " << t.what() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
	int counter=0;
	for(int i=0; i< this->number_points; i++){
		if(this->points[i].get_channel_type()==1){
			(*x)[counter]=this->points[i].get_xcoordinate();
			(*z)[counter]=this->points[i].get_ycoordinate();
			counter++;
		}
	}
	return no;
}
//Get the points of the main channel
int _Hyd_River_Profile_Type::get_points_main_channel(double **x, double **z){
	int no=0;
	for(int i=0; i< this->number_points; i++){
		if(this->points[i].get_channel_type()==2){
			no++;
		}
	}
	no++;
	try{
		(*x)=new double[no];
		(*z)=new double[no];

	}
	catch(bad_alloc &t){
		Error msg=this->set_error(8);
		ostringstream info;
		info<< "Info bad alloc: " << t.what() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
	int counter=0;
	for(int i=0; i< this->number_points; i++){
		if(i<this->number_points-1){
			if(this->points[i].get_channel_type()==1 && this->points[i+1].get_channel_type()==2){
				(*x)[counter]=this->points[i].get_xcoordinate();
				(*z)[counter]=this->points[i].get_ycoordinate();
				counter++;
			}
		}
		if(this->points[i].get_channel_type()==2){
			(*x)[counter]=this->points[i].get_xcoordinate();
			(*z)[counter]=this->points[i].get_ycoordinate();
			counter++;
		}
	}


	return no;
}
//Get the points of the right bank
int _Hyd_River_Profile_Type::get_points_right_bank(double **x, double **z){
	int no=0;
	for(int i=0; i< this->number_points; i++){
		if(this->points[i].get_channel_type()==3){
			no++;
		}
	}
	if(no>0){
		no++;
	}
	try{
		(*x)=new double[no];
		(*z)=new double[no];

	}
	catch(bad_alloc &t){
		Error msg=this->set_error(8);
		ostringstream info;
		info<< "Info bad alloc: " << t.what() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
	int counter=0;
	for(int i=0; i< this->number_points; i++){
		if(i<this->number_points-1){
			if(this->points[i].get_channel_type()==2 && this->points[i+1].get_channel_type()==3){
				(*x)[counter]=this->points[i].get_xcoordinate();
				(*z)[counter]=this->points[i].get_ycoordinate();
				counter++;
			}
		}
		if(this->points[i].get_channel_type()==3){
			(*x)[counter]=this->points[i].get_xcoordinate();
			(*z)[counter]=this->points[i].get_ycoordinate();
			counter++;
		}
	}


	return no;
}
//Get local zmax value
double _Hyd_River_Profile_Type::get_local_zmax(void){
	return this->global_z_max-this->global_z_min;
}
//Get the first point of the profile
Hyd_River_Profile_Point * _Hyd_River_Profile_Type::get_first_point(void){
	if(this->points==NULL){
		return NULL;
	}
	else{
		return &(this->points[0]);
	}
}
//Get the last point of the profile
Hyd_River_Profile_Point * _Hyd_River_Profile_Type::get_last_point(void){
	if(this->points==NULL){
		return NULL;
	}
	else{
		return &(this->points[this->number_points-1]);
	}
}
//Get the global x-coordinate of a profile point
double _Hyd_River_Profile_Type::get_global_x(const int index) {
	if (this->points == NULL) {
		return NULL;
	}
	else {
		return (this->points[index].get_global_x_y_coordinates().get_xcoordinate());
	}
}
//Get the global y-coordinate of a profile point
double _Hyd_River_Profile_Type::get_global_y(const int index) {
	if (this->points == NULL) {
		return NULL;
	}
	else {
		return (this->points[index].get_global_x_y_coordinates().get_ycoordinate());
	}
}
//Get the minimal z-coordinate (global_z_min) 
double _Hyd_River_Profile_Type::get_global_z_min(void){
	return this->global_z_min;
}
//Get the Init-condition as an area by a given local waterlevel
double _Hyd_River_Profile_Type::get_init_condition_value(const double init_value_h){
	double init_area;
	init_area=this->area_table.get_interpolated_values(init_value_h+this->global_z_min);
	return init_area;
}
//Get the actual global waterlevel (s_value)
double _Hyd_River_Profile_Type::get_actual_global_waterlevel(void){
	return this->s_value;
}
//Set the waterlevel to the init-condition
void _Hyd_River_Profile_Type::set_waterlevels2init_condition(const double h_init){
	this->h_value=h_init;
	this->s_value=this->global_z_min+h_init;
}
//Get the actual local waterlevel (h_value)
double _Hyd_River_Profile_Type::get_actual_local_waterlevel_h(void){
	return this->h_value;
}
//Get the actual local flowvelocity
double _Hyd_River_Profile_Type::get_actual_flow_velocity(void){
	return this->v_value;
}
//Get the solved area
double _Hyd_River_Profile_Type::get_solved_area(void){
	return this->solved_area;
}
//Get the area for used for the calculation
double _Hyd_River_Profile_Type::get_area2calc(void){
	return this->area2calc;
}
//Get a maximum waterlevel related to a profile point specified by the given point index; ifthe point do not exists 0.0 m will be returned
double _Hyd_River_Profile_Type::get_hmax2related_profile_point(const int profile_point_index){
	double h_buff=0.0;
	if(profile_point_index<0 || profile_point_index>=this->number_points){
		return h_buff=0.0;
	}
	else{
		h_buff=this->h_value_max.maximum-this->points[profile_point_index].get_ycoordinate();
	}
	return h_buff;
}
//Get a array of the profile points (x_gloabl, y_global), the number is returned
int _Hyd_River_Profile_Type::get_points_x_y_glob(Geo_Point **my_points){
	try{
		*my_points=new Geo_Point[this->number_points];
	}
	catch(bad_alloc &t){
		Error msg=this->set_error(0);
		ostringstream info;
		info << "Bad alloc info : " << t.what() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
	for(int i=0; i< this->number_points; i++){
		(*my_points)[i]=this->points[i].get_global_x_y_coordinates();
	}

	return this->number_points;

}
//Get a string for inserting a polyline data of the profile to a sql-database
string _Hyd_River_Profile_Type::get_polyline2sql_string(void){
	ostringstream buff;
	buff << FORMAT_FIXED_REAL << P(3);
	buff << " ST_GEOMFROMTEXT('LINESTRING(";
	for(int i=0; i< this->number_points-1; i++){
		buff<<this->points[i].get_global_x_y_coordinates().get_xcoordinate() << "  ";
		buff<<this->points[i].get_global_x_y_coordinates().get_ycoordinate()<< ", ";
	}
	//set the first point again
	if(this->number_points!=0){
		buff<<this->points[this->number_points-1].get_global_x_y_coordinates().get_xcoordinate() << "  ";
		buff<<this->points[this->number_points-1].get_global_x_y_coordinates().get_ycoordinate()<< " ";
	}
	buff << ")') ";

	
	return buff.str();
}
//out the profile type members
void _Hyd_River_Profile_Type::output_members(ostringstream *cout){
	
	*cout << " Number of points " << this->number_points << endl;
	*cout << " Globals: zmin_z " << P(2) << FORMAT_FIXED_REAL << this->global_z_min << label::m;
	*cout << "; zmin_x " << P(2) << FORMAT_FIXED_REAL <<this->global_x_zmin <<label::m; 
	*cout << "; zmin_y " << P(2) << FORMAT_FIXED_REAL << this->global_y_zmin << label::m << endl;
	*cout << " Globals: zmax_z " << P(2) << FORMAT_FIXED_REAL << this->global_z_max <<label::m;
	*cout << "; zmax_x " << P(2) << FORMAT_FIXED_REAL << this->global_x_zmax <<label::m;
	*cout << "; zmax_y " << P(2) << FORMAT_FIXED_REAL << this->global_y_zmax << label::m << endl;
	*cout << " The points are... " << endl;
	*cout << W(8) << "No." << W(12)<<"x(glob)" << label::m<< W(12) <<"y(glob)" << label::m;
	*cout << W(12) <<"z(glob)" << label::m <<W(10) <<"x(loc)" <<label::m;
	*cout <<W(10) <<"z(loc)" <<label::m <<W(10) <<"mattype" <<label::no_unit<< W(10) <<"ident"<<label::no_unit<< endl;
	for(int i=0 ; i< this->number_points ;i++){
		this->points[i].output_members(cout);
	}
}
//calculate maximum values 
void _Hyd_River_Profile_Type::calculate_maximum_values(const double time_point){
	double delta_t=time_point-this->old_time_point;
	
	
	//global waterlevel
	if(this->s_value>this->s_value_max.maximum){
		this->s_value_max.maximum=this->s_value;
		this->s_value_max.time_point=time_point;
	}
	//local waterlevel
	if(this->h_value>this->h_value_max.maximum){
		this->h_value_max.maximum=this->h_value;
		this->h_value_max.time_point=time_point;
	}
	//velocity
	if(this->v_value>this->v_value_max.maximum){
		this->v_value_max.maximum=this->v_value;
		this->v_value_max.time_point=time_point;
	}
	if(this->h_value<=constant::dry_hyd_epsilon){
		this->was_dry_flag=true;
		this->dry_duration=this->dry_duration+delta_t;
	}
	else{
		this->wet_duration=wet_duration+delta_t;
	}
	//width
	if (this->width > this->width_value_max.maximum) {
		this->width_value_max.maximum = this->width;
		this->width_value_max.time_point = time_point;
	}

}
//output the result members
void _Hyd_River_Profile_Type::output_result_members_timestep(ostringstream *cout){
	*cout << W(11) <<this->global_z_min;
	*cout << W(11) <<this->s_value;
	*cout << W(11) <<this->h_value;
	*cout << W(11) <<this->v_value;	
	*cout << W(11) <<this->froude_no;
	*cout << W(11) << this->width;
}
//Output the result members each internal timestep to file
void _Hyd_River_Profile_Type::output_result_members_timestep(ofstream *file){
	*file << W(12) << P(4) << FORMAT_FIXED_REAL << this->global_z_min;
	*file << W(12) << P(4) << FORMAT_FIXED_REAL << this->get_first_point()->get_global_z_coordinate() ;
	*file << W(12) << P(4) << FORMAT_FIXED_REAL << this->get_last_point()->get_global_z_coordinate() ;
	*file << W(12) << P(4) <<FORMAT_FIXED_REAL <<this->s_value ;
	*file << W(12) << P(5) <<FORMAT_FIXED_REAL <<this->h_value ;
	*file << W(12) << P(4) <<FORMAT_FIXED_REAL <<this->v_value ;
	*file << W(12) << P(4) <<FORMAT_FIXED_REAL <<this->froude_no ;
	*file << W(12) << P(4) << FORMAT_FIXED_REAL << this->width;
}
///Output the result members each internal timestep to csv file
void _Hyd_River_Profile_Type::output_result_members_timestep2csv(ofstream *file) {
	*file << W(12) << P(4) << FORMAT_FIXED_REAL << this->global_z_min<<",";
	*file << W(12) << P(4) << FORMAT_FIXED_REAL << this->get_first_point()->get_global_z_coordinate() << ",";
	*file << W(12) << P(4) << FORMAT_FIXED_REAL << this->get_last_point()->get_global_z_coordinate() << ",";
	*file << W(12) << P(4) << FORMAT_FIXED_REAL << this->s_value << ",";
	*file << W(12) << P(5) << FORMAT_FIXED_REAL << this->h_value << ",";
	*file << W(12) << P(4) << FORMAT_FIXED_REAL << this->v_value << ",";
	*file << W(12) << P(4) << FORMAT_FIXED_REAL << this->froude_no << ",";
	*file << W(12) << P(4) << FORMAT_FIXED_REAL << this->width << ",";
}
//Output the header for the maximum result output to console/display 
void _Hyd_River_Profile_Type::output_header_max_results(ostringstream *cout){
	*cout <<  "s_max" << label::m<< W(10); 
	*cout<<  "t(s_max)" << label::sec << W(10);
	*cout <<  "h_max" << label::m<< W(10); 
	*cout<<  "t(h_max)" << label::sec << W(10); 
	*cout << "v_max" <<label::m_per_sec<< W(10);
	*cout<<  "t(v_max)" << label::sec << W(10); 
	*cout << "was_dry" << label::no_unit<<W(10);
	*cout << "t(dry)" << label::sec<<W(10);
	*cout << "t(wet)" << label::sec<<W(15);
	*cout << "width_max" << label::m << W(10);
	*cout << "t(width_max)" << label::sec << W(10);

}
//Output the maximum result output to console/display 
void _Hyd_River_Profile_Type::output_max_results(ostringstream *cout){
	*cout << P(2) << FORMAT_FIXED_REAL<< this->s_value_max.maximum << W(15);
	*cout << P(0) << FORMAT_FIXED_REAL<< this->s_value_max.time_point << W(17);
	*cout << P(2) << FORMAT_FIXED_REAL<< this->h_value_max.maximum << W(13);
	*cout << P(0) << FORMAT_FIXED_REAL<< this->h_value_max.time_point << W(19);
	*cout << P(2) << FORMAT_FIXED_REAL<< this->v_value_max.maximum << W(13);
	*cout << P(0) << FORMAT_FIXED_REAL<< this->v_value_max.time_point << W(15);
	*cout <<this->was_dry_flag << W(14);
	*cout << P(0) << FORMAT_FIXED_REAL<<this->dry_duration<< W(16);
	*cout << P(0) << FORMAT_FIXED_REAL<<this->wet_duration<< W(15);
	*cout << P(2) << FORMAT_FIXED_REAL << this->width_value_max.maximum << W(14) << ",";
	*cout << P(0) << FORMAT_FIXED_REAL << this->width_value_max.time_point << W(14) << ",";

}
//Output the maximum result output to a given file 
void _Hyd_River_Profile_Type::output_max_results2file(ofstream *file){
	*file << P(2) << FORMAT_FIXED_REAL<< this->global_z_min<< W(14);
	*file << P(2) << FORMAT_FIXED_REAL << this->get_first_point()->get_global_z_coordinate() << W(14) ;
	*file << P(2) << FORMAT_FIXED_REAL << this->get_last_point()->get_global_z_coordinate() << W(14);
	*file << P(2) << FORMAT_FIXED_REAL<< this->s_value_max.maximum << W(14);
	*file << P(0) << FORMAT_FIXED_REAL<< this->s_value_max.time_point << W(14);
	*file << P(2) << FORMAT_FIXED_REAL<< this->h_value_max.maximum << W(14);
	*file << P(0) << FORMAT_FIXED_REAL<< this->h_value_max.time_point << W(14);
	*file << P(2) << FORMAT_FIXED_REAL<< this->v_value_max.maximum << W(14);
	*file << P(0) << FORMAT_FIXED_REAL<< this->v_value_max.time_point << W(14);
	*file <<this->was_dry_flag << W(12);
	*file << P(0) << FORMAT_FIXED_REAL<<this->dry_duration<< W(14);
	*file << P(0) << FORMAT_FIXED_REAL<<this->wet_duration<< W(14);
	*file << P(2) << FORMAT_FIXED_REAL << this->width_value_max.maximum << W(14) << ",";
	*file << P(0) << FORMAT_FIXED_REAL << this->width_value_max.time_point << W(14) << ",";
}
//Output the maximum result output to a csv file 
void _Hyd_River_Profile_Type::output_max_results2csvfile(ofstream *file) {
	*file << P(2) << FORMAT_FIXED_REAL << this->global_z_min << W(14) << ",";
	*file << P(2) << FORMAT_FIXED_REAL << this->get_first_point()->get_global_z_coordinate() << W(14) << ",";
	*file << P(2) << FORMAT_FIXED_REAL << this->get_last_point()->get_global_z_coordinate() << W(14) << ",";
	*file << P(2) << FORMAT_FIXED_REAL << this->s_value_max.maximum << W(14) << ",";
	*file << P(0) << FORMAT_FIXED_REAL << this->s_value_max.time_point << W(14) << ",";
	*file << P(2) << FORMAT_FIXED_REAL << this->h_value_max.maximum << W(14) << ",";
	*file << P(0) << FORMAT_FIXED_REAL << this->h_value_max.time_point << W(14) << ",";
	*file << P(2) << FORMAT_FIXED_REAL << this->v_value_max.maximum << W(14) << ",";
	*file << P(0) << FORMAT_FIXED_REAL << this->v_value_max.time_point << W(14) << ",";
	*file << this->was_dry_flag << W(12) << ",";
	*file << P(0) << FORMAT_FIXED_REAL << this->dry_duration << W(14) << ",";
	*file << P(0) << FORMAT_FIXED_REAL << this->wet_duration << W(14) << ",";
	*file << P(2) << FORMAT_FIXED_REAL << this->width_value_max.maximum << W(14) << ",";
	*file << P(0) << FORMAT_FIXED_REAL << this->width_value_max.time_point << W(14) << ",";

}
//Set the maximum result values of profile type to a string for the transfer into a database table (_Hyd_River_Profile)
string _Hyd_River_Profile_Type::set_maximum_value2string(void){
	string buffer;
	ostringstream query_string;	
	query_string << this->h_value_max.maximum << " , " ;
	query_string << this->s_value_max.maximum << " , " ;
	query_string << this->h_value_max.time_point << " , " ;
	query_string << this->v_value_max.maximum << " , " ;
	query_string << this->width_value_max.maximum << " , ";
	query_string << this->wet_duration << " , " ;
	query_string << this->dry_duration << " , " ;
	buffer=query_string.str();
	return buffer;
}
//Set the instationary result values of profile type to a string for the transfer into a database table (_Hyd_River_Profile)
string _Hyd_River_Profile_Type::set_instat_value2string(void) {
	string buffer;
	ostringstream query_string;
	query_string << this->h_value << " , ";
	query_string << this->s_value << " , ";
	query_string << this->v_value << " , ";
	query_string << this->width << " , ";
	buffer = query_string.str();
	return buffer;
}
//Set the actuel flow velocity by a given discharge and the corresponding froude-number
void _Hyd_River_Profile_Type::set_actuel_flowvelocity(const double discharge){
	if(this->area2calc<=constant::area_epsilon){
		this->v_value=0.0;
		this->froude_no=0.0;
	}
	else{
		this->v_value=discharge/this->area2calc;
		this->froude_no=this->v_value/pow((constant::Cgg*this->h_value),0.5);
	}
}
//Calculate the velocity energy head per syncronisation step
void _Hyd_River_Profile_Type::calculate_velocity_energy_head(const double time_point, _Hyd_River_Profile *downstream){

	

	this->delta_t_v_energy=this->delta_t_v_energy+(time_point-this->old_time_point);

	if(this->delta_t_v_energy>10){
		double v_buff=this->v_value;
		if(downstream!=NULL){
			if(downstream->get_profile_type()!=_hyd_profile_types::WEIR_TYPE){
				v_buff=downstream->typ_of_profile->get_actual_flow_velocity();
			}
		}
		v_buff=0.5*(v_buff+this->v_value);

		//23/12  -16/12  5/12 *delta t

		this->delta_t_v_energy=0.0;
		this->v_energy=pow(v_buff,2.0)/(2.0*constant::Cgg);
		this->v_energy_old=this->v_energy;
		this->v_energy_old_old=this->v_energy_old;

		//this->v_energy=this->v_energy*1/10+this->v_energy_old*9/10;


	}

}
//Get the velocity energy head per syncronisation step
double _Hyd_River_Profile_Type::get_velocity_energy_head(void){
	return this->v_energy;
}
//Set the time point of the syncronisation step before
void _Hyd_River_Profile_Type::set_old_time_point(const double time_point){
	this->old_time_point=time_point;
}
//Get the time point of the syncronisation step before
double _Hyd_River_Profile_Type::get_old_time_point(void){
	return this->old_time_point;
}
//Calculate the stabilization discharge
void _Hyd_River_Profile_Type::calculate_stabilization_discharge(void){
	this->stab_discharge=constant::stabilization_q;
	if(this->solved_area<=constant::flow_epsilon){
		this->stab_discharge=-1.0/(10.0)*this->solved_area;
		this->stab_discharge=this->stab_discharge+(-1.0)*constant::stabilization_q;
	}
	else{
		this->stab_discharge=constant::stabilization_q*(-1.0);
	}
}
//Get the stabilization discharge
double _Hyd_River_Profile_Type::get_stabilization_discharge(void){

	return this->stab_discharge;;
}
//Get the flag if an outflow reduction is applied due to a setted outflow but no watervolume is left in the profile
bool _Hyd_River_Profile_Type::get_outflow_reduction_flag(void){
	return this->zero_outflow_reduction_flag;
}
//Reset the maximum values and the result
void _Hyd_River_Profile_Type::reset_result_max_values(void){
	//result variables per time step
	this->h_value=0.0;
	this->s_value=0.0;
	this->s_value_max.maximum=0.0;
	this->s_value_max.time_point=0.0;
	this->h_value_max.maximum=0.0;
	this->h_value_max.time_point=0.0;
	this->v_value=0.0;
	this->v_value_max.maximum=0.0;
	this->v_value_max.time_point=0.0;
	this->solved_area=0.0;
	this->area2calc=0.0;
	this->stab_discharge=0.0;

	this->width = 0.0;;
	this->width_value_max.maximum = 0.0;
	this->width_value_max.maximum = 0.0;


	this->v_energy=0.0;
	this->v_energy_old=0.0;
	this->v_energy_old_old=0.0;
	this->delta_t_v_energy=0.0;
	old_energy_time=0.0;

	this->was_dry_flag=false;
	this->dry_duration=0.0;
	this->wet_duration=0.0;
	this->old_time_point=0.0;

	this->zero_outflow_reduction_flag=false;

}
//Get the width of the profile start and end point 
double _Hyd_River_Profile_Type::get_width_end_start_profile(void){
	return this->profile_width;
}
//Get the froude-number for this profile
double _Hyd_River_Profile_Type::get_froude_number(void){
	return this->froude_no;
}
//Clone profile type
void _Hyd_River_Profile_Type::clone_profile_type(_Hyd_River_Profile_Type *prof_type){
	this->global_z_min=prof_type->global_z_min;
	this->global_x_zmin=prof_type->global_x_zmin;
	this->global_y_zmin=prof_type->global_y_zmin;
	this->global_z_max=prof_type->global_z_max;
	this->global_x_zmax=prof_type->global_x_zmax;
	this->global_y_zmax=prof_type->global_y_zmax;
	this->zmax_marker=prof_type->zmax_marker;
	this->zmin_marker=prof_type->zmin_marker;
	this->number_table_points=prof_type->number_table_points;
	this->profile_width=prof_type->profile_width;

	this->mid_point_global_x_y=prof_type->mid_point_global_x_y;
	this->min_point_global_x_y=prof_type->min_point_global_x_y;
    this->user_setting=prof_type->user_setting;

	//copy points
	for(int i=0; i< this->number_points; i++){
		this->points[i].copy_points(&prof_type->points[i]);
	}

	//copy tables
	this->area_table.clone_tables(&prof_type->area_table);
	this->h_table.clone_tables(&prof_type->h_table);
	this->width_table.clone_tables(&prof_type->width_table);
	this->area_table.set_independent_values(&this->h_table);
	this->h_table.set_independent_values(&this->area_table);
	this->width_table.set_independent_values(&this->area_table);

}
//Channel type to text (static)
string _Hyd_River_Profile_Type::channel_type2text(const int type){
	string buffer;

	if(type==1){
		buffer=hyd_label::channel_left;
	}
	else if(type==2){
		buffer=hyd_label::channel_main;
	}
	else if(type==3){
		buffer=hyd_label::channel_right;
	}
	else{
		buffer=label::unknown_type;
	}

	return buffer;

}
//Set the user flags for the calculation
void _Hyd_River_Profile_Type::set_user_flags(const _hyd_profile_calc_setting flags){
	this->user_setting=flags;
}
//Get the user flags for the calculation
_hyd_profile_calc_setting _Hyd_River_Profile_Type::get_user_flags(void){
	return this->user_setting;
}
//Get h_max-value
_hyd_max_values _Hyd_River_Profile_Type::get_max_h(void) {

	return  this->h_value_max;

}
//Get width_max-value
_hyd_max_values _Hyd_River_Profile_Type::get_max_width(void) {

	return  this->width_value_max;

}
//Get s_max-value
_hyd_max_values _Hyd_River_Profile_Type::get_max_s(void) {

	return  this->s_value_max;

}
//Get max vtot-value
_hyd_max_values _Hyd_River_Profile_Type::get_max_vtot(void) {

	return  this->v_value_max;

}
///Get h-value
double _Hyd_River_Profile_Type::get_h(void) {
	return this->h_value;
}
///Get width-value
double _Hyd_River_Profile_Type::get_width(void) {
	return this->width;
}
///Get s-value
double _Hyd_River_Profile_Type::get_s(void) {
	return this->s_value;
}
///Get vtot-value
double _Hyd_River_Profile_Type::get_vtot(void) {
	return this->v_value;
}
///Get q-value
//double _Hyd_River_Profile_Type::get_Q(void) {
//
//	return 0.0;
//}
//___________________
//protected
//methods
//plot generation
void _Hyd_River_Profile_Type::generate_plot(QWidget *parent){

	if(this->plot!=NULL){
		this->plot->close();
		delete this->plot;
	}
	this->plot = new HydGui_Profile_Plot(parent);
}
//Set the actuel local Waterlevel (h) and the global waterlevel (s) with a given area from tables
void _Hyd_River_Profile_Type::set_actual_waterlevel_by_table(const double area){
	try{
		this->s_value=this->h_table.get_interpolated_values(area);
		this->h_value=this->s_value-this->global_z_min;
		if(this->h_value <= constant::flow_epsilon){
			this->h_value=0.0;
			this->s_value=this->global_z_min;
			this->area2calc=0.0;
		}
		this->width = this->width_table.get_interpolated_values(area);

	}
	catch(Error msg){
		throw msg;
	}
}
//Set the waterlevels of the profile directly without the use of a table
void _Hyd_River_Profile_Type::set_waterlevel_direct(double const global_waterlevel){
	this->h_value=global_waterlevel-this->global_z_min;
	this->s_value=global_waterlevel;
	if(this->h_value <= constant::flow_epsilon){
		this->h_value=0.0;
		this->s_value=this->global_z_min;
	}
}
//check the points and the areatypes
void _Hyd_River_Profile_Type::check_points(const string name, const int number){

	//check number of points
	if(this->number_points<3){
		Error msg=this->set_error(4);
		throw msg;
	}
	//check the bank-identity of right- (3), left bank( 1), and mid channel (2)
	for(int i=1; i< this->number_points; i++){
		if(this->points[i].identity< 1 || this->points[i].identity > 3){
			Error msg=this->set_error(7);
			ostringstream info;
			info << "Profile point number : " << i+1 << endl;
			info << "Given identity       : " << this->points[i].identity << endl;
			info << "x-coordinate         : " << FORMAT_FIXED_REAL <<P(3) << this->points[i].get_global_x_y_coordinates().get_xcoordinate() << endl;
			info << "y-coordinate         : " << this->points[i].get_global_x_y_coordinates().get_ycoordinate() << endl;
			msg.make_second_info(info.str());
			throw msg;
		}
	}
	//check the bank-identity of the points in sequenz:1 1 1 2 2 2 3 3 => ok; 1 1 2 1 2 2 3 2 3 3 => false
	for(int i=1; i< this->number_points; i++){
		if(this->points[i].identity<this->points[i-1].identity){
			Warning msg=this->set_warning(0);
			ostringstream info;
			info << "Given bank identity  : " << this->points[i].identity << endl;
			info << "It is switched to    : " << this->points[i-1].identity << endl;
			info << "Profile point number : " << i << endl;
			info << "x-coordinate         : " << FORMAT_FIXED_REAL << P(3) << this->points[i].get_global_x_y_coordinates().get_xcoordinate() << endl;
			info << "y-coordinate         : " << this->points[i].get_global_x_y_coordinates().get_ycoordinate() << endl;
			info << "Profile name         : " << name << endl;
			info << "Profile number       : " << number << endl;

			//reaction
			this->points[i].identity=this->points[i-1].identity;
			msg.make_second_info(info.str());
			msg.output_msg(2);
		}
	}
	for(int i=0; i< this->number_points-1; i++){
		if(this->points[i].identity>this->points[i+1].identity){
			Warning msg=this->set_warning(0);
			ostringstream info;
			info << "Given bank identity  :" << this->points[i].identity << endl;
			info << "It is switched to    :" << this->points[i+1].identity << endl;
			info << "Profile point number : " << i << endl;
			info << "x-coordinate         : " << FORMAT_FIXED_REAL << P(3) << this->points[i].get_global_x_y_coordinates().get_xcoordinate() << endl;
			info << "y-coordinate         : " << this->points[i].get_global_x_y_coordinates().get_ycoordinate() << endl;
			info << "Profile name         : " << name << endl;
			info << "Profile number       : " << number << endl;
			//reaction
			this->points[i].identity=this->points[i+1].identity;
			msg.make_second_info(info.str());
			msg.output_msg(2);
		}
	}
	//check if there same neighbouring points
	for(int i=0; i<this->number_points-1;i++){
		if(this->points[i]==this->points[i+1]){
			this->points[i].set_point_y_coordinate(this->points[i].get_ycoordinate() + 0.001);
			if (this->points[i] == this->points[i + 1]) {
				//error
				Error msg = this->set_error(2);
				ostringstream info;
				info << "Numbers " << i << " " << i + 1 << endl;
				msg.make_second_info(info.str());
				throw msg;
			}
		}
	}
	//check the first and the last point
	if(this->points[0].check_below_of(&this->points[1]) || this->points[0].get_ycoordinate()==this->points[1].get_ycoordinate()){
		//error
		/*Error msg=this->set_error(3);
		throw msg;*/
		
		Warning msg = this->set_warning(2);
		ostringstream info;
		info << "Point (start/end) :" << "Start point" << endl;
		info << "Given height start  :" << this->points[0].get_global_z_coordinate() << endl;
		info << "Given height start + 1  :" << this->points[1].get_global_z_coordinate() << endl;
		info << "Profile name         : " << name << endl;
		info << "Profile number       : " << number << endl;
		
		//reaction
		this->points[0].set_point_y_coordinate(this->points[1].get_ycoordinate() + 0.001);
		msg.make_second_info(info.str());
		msg.output_msg(2);
	}
	if(this->points[this->number_points-1].check_below_of(&this->points[this->number_points-2]) || this->points[this->number_points-1].get_ycoordinate()==this->points[this->number_points-2].get_ycoordinate()){
		//error
		/*Error msg=this->set_error(3);
		throw msg;*/
		Warning msg = this->set_warning(2);
		ostringstream info;
		info << "Point (start/end) :" << "End point" << endl;
		info << "Given height end  :" << this->points[this->number_points - 1].get_global_z_coordinate() << endl;
		info << "Given height end - 1  :" << this->points[this->number_points - 2].get_global_z_coordinate() << endl;
		info << "Profile name         : " << name << endl;
		info << "Profile number       : " << number << endl;
		//reaction
		this->points[this->number_points - 1].set_point_y_coordinate(this->points[this->number_points - 2].get_ycoordinate() + 0.001);
		msg.make_second_info(info.str());
		msg.output_msg(2);
	}

	//check if there are no interception of the profile line
	Geo_Polysegment test_line;
	//extract Geo_Points
	Geo_Point *test_points=NULL;
	try{
		test_points=new Geo_Point[this->number_points];
	}
	catch(bad_alloc &){
		Error msg=this->set_error(0);
		ostringstream info;
		info << "Try to allocate the check points" << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
	for(int i=0; i<this->number_points; i++){
		test_points[i]=this->points[i];
	}
	try{
		//check it
		test_line.set_segments(this->number_points, test_points);
	}
	catch(Error msg){
		if(test_points!=NULL){
			delete []test_points;
			test_points=NULL;
		}
		ostringstream info;
		info << "The profile segments are intercepting each other; it is not possible" << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
	if(test_points!=NULL){
		delete []test_points;
		test_points=NULL;
	}
}
//Check if there are unphysical waterlevel calculated by the solver
void _Hyd_River_Profile_Type::check_unphysical_waterlevels(void){
	if(abs(this->h_value)>19882.0){
		Error msg=this->set_error(5);
		throw msg;
	}
	if(functions::check_infinite(this->h_value)==true){
		Error msg=this->set_error(6);
		throw msg;
	}
}
//______________________
//private
//allocate the points
void _Hyd_River_Profile_Type::allocate_points(void){
	try{
		this->points=new Hyd_River_Profile_Point[this->number_points];
	}
	catch(bad_alloc &t){
		Error msg=this->set_error(0);
		ostringstream info;
		info<< "Info bad alloc: " << t.what() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
	//count memory
	Sys_Memory_Count::self()->add_mem(sizeof(Hyd_River_Profile_Point)*this->number_points, _sys_system_modules::HYD_SYS);
}
//set the coordinates of zmin;
void _Hyd_River_Profile_Type::set_zmin_coordinates(void){
	
	this->global_z_min=this->points[0].z_global;

	for(int i=0; i< this->number_points;i++){
		if(this->global_z_min> this->points[i].z_global){
			this->global_z_min=this->points[i].z_global;
			this->zmin_marker=i;
		}
	}
	//set the rest
	this->global_x_zmin=this->points[this->zmin_marker].x_global;
	this->global_y_zmin=this->points[this->zmin_marker].y_global;

}
//set the coordinates of zmax;
void _Hyd_River_Profile_Type::set_zmax_coordinates(void){
	
	this->global_z_max=this->points[0].z_global;

	for(int i=0; i< this->number_points;i++){
		if(this->global_z_max< this->points[i].z_global){
			this->global_z_max=this->points[i].z_global;
			this->zmax_marker=i;
		}
	}
	//set the rest
	this->global_x_zmax=this->points[this->zmax_marker].x_global;
	this->global_y_zmax=this->points[this->zmax_marker].y_global;

}
//calculate th local coordinates
void _Hyd_River_Profile_Type::calculate_local_coordinates(void){
	this->points[0].y_coordinate=this->points[0].z_global-this->global_z_min;
	for(int i=1; i< this->number_points;i++){
		this->points[i].y_coordinate=this->points[i].z_global-this->global_z_min;
		this->points[i].x_coordinate=this->points[i].x_coordinate-this->points[0].x_coordinate;
	}
	//after calcualtion of difference is done the first point can be set to zero
	this->points[0].x_coordinate=0.0;

	//calculate the width of the start and end-point
	this->profile_width=this->points[this->number_points-1].x_coordinate;
	
}
//Calculate the global midpoint
void _Hyd_River_Profile_Type::calculate_global_midpoint_x_y(void){
	if(this->number_points>0){
		double y_mid=0.0;
		double x_mid=0.0;
		x_mid=this->points[0].x_global+(this->points[this->number_points-1].x_global-this->points[0].x_global)*0.5;
		y_mid=this->points[0].y_global+(this->points[this->number_points-1].y_global-this->points[0].y_global)*0.5;
		//Daniel: is the mid-linbe needed; changed it to the min points! TEST iT! 1.1.2019
		//this->mid_point_global_x_y.set_point_coordinate(x_mid,y_mid);
		this->mid_point_global_x_y.set_point_coordinate(this->global_x_zmin, this->global_y_zmin);
		this->mid_point_global_x_y.set_point_name(hyd_label::profile_midpoint_x_y);
	}
}
	///Calculate the global minpoint
void _Hyd_River_Profile_Type::calculate_global_minpoint_x_y(void){

	if(this->number_points>0){
		this->min_point_global_x_y.set_point_coordinate(this->global_x_zmin,this->global_y_zmin);
		this->min_point_global_x_y.set_point_name(hyd_label::profile_minpoint_x_y);
	}
}
//set the error
Error _Hyd_River_Profile_Type::set_error(const int err_type){
		string place="_Hyd_River_Profile_Type::";
		string help;
		string reason;
		int type=0;
		bool fatal=false;
		stringstream info;
		Error msg;

	switch (err_type){
		case 0://bad alloc
			place.append("allocate_points(void)/allocate_area_values(const int number)/check_points(void)/get_polysegment_x_y_glob(Geo_Polysegment *my_poly)");
			reason="Can not allocate the memory";
			help="Check the memory";
			type=10;
			break;
		case 1://not all profile points are found
			place.append("input_members(ifstream *profile_file)");
			reason="Error by reading the profile points: Not all points found or wrong point values";
			help="Check the profile points";
			info<< "Number of point needed to be found " << this->number_points << endl;
			type=1;
			break;
		case 2://there are identical points
			place.append("check_points(void)");
			reason="There are identical neighbouring points";
			help="Check the profile points";
			type=11;
			break;
		case 3://end points of the profile are not correct
			place.append("check_points(void)");
			reason="The start/end of the profile is not correct; the startpoint/endpoint should be always above the following point (start) or the point before (end) ";
			help="Check the start/end profile points";
			type=11;
			break;
		case 4://to less points
			place.append("check_points(void)");
			reason="The number of points have to be >=3 ";
			help="Check the number of profile points";
			info << "Number of points: " << this->number_points << endl;
			type=11;
			break;
		case 5://emergency stop of the solver
			place.append("check_unphysical_waterlevels(void)");
			reason="The waterlevel has no physical sense; the absolute value are > 19882.0 m (Marian Gap + Mont Everest)";
			help="Check the model and solver settings";
			type=17;
			break;
		case 6://the waterlevel are not defined
			place.append("check_unphysical_waterlevels(void)");
			reason="The waterlevel is not defined";
			help="Check the model and solver settings";
			type=17;
			break;
		case 7://wrong bank identity
			place.append("check_points(void)");
			reason="The bank identity of the profile point is wrong";
			help="Check the number of profile points";
			info << "Possible bank identities: "<<endl;
			info << " 1 = left bank  "<<endl;
			info << " 2 = mid channel "<<endl;
			info << " 3 = right bank  "<<endl;
			type=11;
			break;
		case 8://bad alloc
			place.append("get_points_...(double **x, double **z)");
			reason="Can not allocate the memory";
			help="Check the memory";
			type=10;
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
Warning _Hyd_River_Profile_Type::set_warning(const int warn_type){
		string place="_Hyd_River_Profile_Type::";
		string help;
		string reason;
		string reaction;
		int type=0;
		Warning msg;
		stringstream info;

	switch (warn_type){
		case 0://the identity are not in the right order
			place.append("check_points(void)") ;
			reason="The ident values of the river profile are not in the correct order ";
			reaction="The ident-type of the previous point is taken";
			help= "Check the profile";
			type=9;	
			break;
		case 1://input datas can not submitted
			place.append("transfer_profile_points2database(QSqlDatabase *ptr_database, const int prof_id)");
			reason="Can not submit the data of the river profile points to the database";
			help="Check the database";
			type=2;
			break;
		case 2://point below 
			place.append("check_points(void)");
			reason = "The start/end of the profile is not correct; the startpoint/endpoint should be always above the following point (start) or the point before (end) ";
			reaction = "Point are highten to following point (start) or the point before (end) +0.001 m";
			help = "Check the start/end profile points";
			type = 9;
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
