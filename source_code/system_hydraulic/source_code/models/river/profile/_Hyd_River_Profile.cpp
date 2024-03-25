//#include "_Hyd_River_Profile.h"
#include "Hyd_Headers_Precompiled.h"

//init static members
Tables *_Hyd_River_Profile::profile_table=NULL;
Tables *_Hyd_River_Profile::boundary_table=NULL;
Tables *_Hyd_River_Profile::wallbreak_table=NULL;
Tables *_Hyd_River_Profile::dikebreak_table=NULL;
Tables *_Hyd_River_Profile::bridge_table=NULL;
Tables *_Hyd_River_Profile::erg_table=NULL;
Tables *_Hyd_River_Profile::erg_instat_table = NULL;

//constructor
_Hyd_River_Profile::_Hyd_River_Profile(void){
	this->name=label::not_set;
	this->river_station=0.0;
	this->profile_number=0;
	this->glob_prof_id=-1;

	this->connection=_hyd_connection_types::STANDARD_CONN;
	//for table calculation
	this->delta_x_table=0.1;

	this->break_flag_left=false;
	this->index_basepoint_left=-1;
	this->break_flag_right=false;
	this->index_basepoint_right=-1;

	this->h_max_right_base=0.0;
	this->h_max_left_base=0.0;

	this->overflow_left_flag=true;
	this->poleni_factor_left_overflow=constant::poleni_const;
	this->overflow_right_flag=true;
	this->poleni_factor_right_overflow=constant::poleni_const;

	this->river_width = -1;

	this->q_left_bank=0.0;
	this->q_right_bank=0.0;
	this->q_1d_coupling=0.0;
	this->q_structure_coupling=0.0;
	this->q_dikebreak_coupling_left=0.0;
	this->q_dikebreak_coupling_right=0.0;

	this->init_condition=0.0;
	this->distance2upstream=0.0;
	this->distance2downstream=0.0;
	this->watervolume_init=0.0;
	this->watervolume=0.0;
	this->old_time_point=0.0;
	this->delta_time=0.0;

	this->left_bank_overflow_volume.volume_in=0.0;
	this->left_bank_overflow_volume.volume_out=0.0;
	this->left_bank_overflow_volume.volume_total=0.0;
	this->right_bank_overflow_volume.volume_in=0.0;
	this->right_bank_overflow_volume.volume_out=0.0;
	this->right_bank_overflow_volume.volume_total=0.0;
	this->coupling_1d_volume.volume_in=0.0;
	this->coupling_1d_volume.volume_out=0.0;
	this->coupling_1d_volume.volume_total=0.0;
	this->structure_coupling_volume.volume_in=0.0;
	this->structure_coupling_volume.volume_out=0.0;
	this->structure_coupling_volume.volume_total=0.0;
	this->left_dikebreak_coupling_volume.volume_in=0.0;
	this->left_dikebreak_coupling_volume.volume_out=0.0;
	this->left_dikebreak_coupling_volume.volume_total=0.0;
	this->right_dikebreak_coupling_volume.volume_in=0.0;
	this->right_dikebreak_coupling_volume.volume_out=0.0;
	this->right_dikebreak_coupling_volume.volume_total=0.0;

	this->ptr_upstream_prof=NULL;
	this->ptr_downstream_prof=NULL;

	this->q_value_max.maximum=0.0;
	this->q_value_max.time_point=0.0;

	this->typ_of_profile=NULL;
	this->type=_hyd_profile_types::RIVER_TYPE;

	this->bridge_specific_value=NULL;

	//break value
	this->left_break_params=NULL;
	this->right_break_params=NULL;

	this->left_bank2fpl=false;
	this->right_bank2fpl=false;

	this->height_left_bank=0.0;
	this->height_right_bank=0.0;

	this->max_h_2break_left=0.0;
	this->max_delta_h_2break_left=0.0;
	this->max_h_2break_right=0.0;
	this->max_delta_h_2break_right=0.0;

	this->fpl_sec_id_left=-1;
	this->fpl_sec_id_right=-1;

	this->allocate_break_parameter_left();
	this->allocate_break_parameter_right();
}
//destructor
_Hyd_River_Profile::~_Hyd_River_Profile(void){
	this->delete_profile_type();
	this->delete_break_parameter_left();
	this->delete_break_parameter_right();
	this->delete_bridge_data();
}
//________________________________
//public
//Input the members of the profile per file
void _Hyd_River_Profile::input_members(QFile *profile_file, const int profile_number, string my_line, int *line_counter){
	//output
	ostringstream cout;
	cout <<"Read in Profilenumber " << profile_number << "..." <<endl;
	Sys_Common_Output::output_hyd->output_txt(&cout, true);

	//set the profile number
	this->profile_number=profile_number;

	//read in the information about the profile
	int buffer_number_points=0;

	int found_counter=0;

	string buffer;
	QString qbuffer;
	string key;
	int pos=-1;
	//file position
	qint64 get_pointer=0;

	stringstream buffer_stream;
	//search in the given line after name and number of points
	//find first find the name "name"
	pos=my_line.find("\"",0);
	buffer=my_line.erase(0,pos+1);
	pos=buffer.find("\"",0);
	this->name=buffer.substr(0,pos);
	buffer=buffer.erase(0,pos+1);
	//second find the number of proints I=
	pos=buffer.find("=",0);
	buffer.erase(0,pos+1);
	buffer_stream << buffer;
	buffer_stream >> buffer_number_points;
	if(buffer_stream.fail()==true){
		ostringstream info;
		info << "Wrong input sequenze " << buffer_stream.str() << endl;
		info << "Profile number " << profile_number <<endl;
		info << "Error occurs near line: "<< *line_counter << endl;
		Error msg=this->set_error(6);
		msg.make_second_info(info.str());
		throw msg;
	}
	buffer_stream.clear();
	buffer_stream.str("");

	try{
		do{
			get_pointer=profile_file->pos();
			(*line_counter)++;
			qbuffer=profile_file->readLine();
			buffer=qbuffer.toStdString();
			_Hyd_Parse_IO::erase_comment(&buffer);
			_Hyd_Parse_IO::erase_carriageReturn(&buffer);
			_Hyd_Parse_IO::erase_leading_whitespace_tabs(&buffer);
			_Hyd_Parse_IO::erase_end_whitespace_tabs(&buffer);

			pos=buffer.find("AUXDATA",0);
			if(pos==-1 && buffer.empty()!=true){
				//rewind the file
				profile_file->seek(get_pointer);
				(*line_counter)--;
				break;
			}
			else{
				if(buffer.empty()!=true){
					try{
						buffer=buffer.erase(0,pos+7);
						pos=buffer.find("=",0);
						buffer_stream << buffer.substr(0, pos);
						buffer_stream >> key;
						buffer_stream.clear();
						buffer_stream.str("");
						buffer=buffer.erase(0,pos);
						//read in the auxdata
						this->decide_keyvalues_file(key, buffer, &found_counter);
					}
					catch(Error msg){
						ostringstream info;
						info << "Error occurs near line: "<< *line_counter << endl;
						msg.make_second_info(info.str());
						throw msg;
					}
				}
			}
		}while(profile_file->atEnd()!=true);

		//check if all needed values are found (they are set in constructors of each connection type)
		if(found_counter!=this->must_found_number){
			Error msg=this->set_error(2);
			ostringstream info;
			info << "Found values          : " << found_counter <<endl;
			info << "Must-found values     : " << this->must_found_number << endl;
			info << "Error occurs near line: "<< *line_counter << endl;
			msg.make_second_info(info.str());
			throw msg;
		}
	}
	catch(Error msg){
		ostringstream info;
		info << "Profile number:"<<this->profile_number<<endl;
		info << "Profilname    :" << this->name << endl;
		msg.make_second_info(info.str());
		throw msg;
	}

	//alloc and read the infos of the profile type
	try{
		this->decide_alloc_profile_type(buffer_number_points);
		this->typ_of_profile->input_members(profile_file, line_counter, this->name, this->profile_number);
	}
	catch(Error msg){
		ostringstream info;
		info << "Profile number:"<<this->profile_number<<endl;
		info << "Profilname    :" << this->name << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
	this->check_set_base_points();
}
//Transfer the river profile boundary data of an hydraulc boundary scenario to a database
void _Hyd_River_Profile::transfer_hydraulic_boundary_sz2database(QSqlDatabase *ptr_database, const int rv_number){
	if(this->boundary_is_applied()==false){
		return;
	}

	//mysql query with the table_model
	QSqlQueryModel model;

	//the table is set (the name and the column names) and allocated
	try{
		_Hyd_River_Profile::set_profile_table(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}

	//evaluate th global id of the profile
	int id_glob=0;
	//generate the filter
	ostringstream filter;
	filter << "Select ";
	filter << _Hyd_River_Profile::profile_table->get_column_name(hyd_label::profdata_glob_id);
	filter << " from "<< _Hyd_River_Profile::profile_table->get_table_name();
	filter << " where ";
	filter << _Hyd_River_Profile::profile_table->get_column_name(label::applied_flag) << " = true";
	filter << " and ";
	filter << _Hyd_River_Profile::profile_table->get_column_name(label::areastate_id) << " = " << this->system_id.area_state;
	filter << " and (";
		filter << _Hyd_River_Profile::profile_table->get_column_name(label::measure_id) << " = " << 0 ;
		filter << " or " ;
		filter << _Hyd_River_Profile::profile_table->get_column_name(label::measure_id) << " = " << this->system_id.measure_nr;
	filter << " ) " ;
	filter << " and ";
	filter << _Hyd_River_Profile::profile_table->get_column_name(hyd_label::profdata_rvno) << " = " << rv_number;
	filter << " and ";
	filter << _Hyd_River_Profile::profile_table->get_column_name(hyd_label::profdata_prof_id) << " = " << this->profile_number;
	//database request
	Data_Base::database_request(&model,filter.str(), ptr_database);
	//check the query
	if(model.lastError().isValid()){
		//error
		Error msg=set_error(13);
		ostringstream info;
		info << "Table Name      : " <<_Hyd_River_Profile::profile_table->get_table_name() << endl;
		info << "Table error info: " << model.lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
	if(model.rowCount()==0){
		Error msg=this->set_error(12);
		ostringstream info;
		info << "Table Name      : " << _Hyd_River_Profile::profile_table->get_table_name() << endl;
		info << "Profile number  : " << this->profile_number << endl;
		info << "Profile name    : " << this->name << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
	id_glob=model.record(0).value((_Hyd_River_Profile::profile_table->get_column_name(hyd_label::profdata_glob_id)).c_str()).toInt();

	if(model.rowCount()>1){
		Warning msg=this->set_warning(4);
		ostringstream info;
		info << "Table Name      : " << _Hyd_River_Profile::profile_table->get_table_name() << endl;
		info << "Profile number  : " << this->profile_number << endl;
		info << "Profile name    : " << this->name << endl;
		info << "Take global id  : " << id_glob << endl;
		msg.make_second_info(info.str());
		msg.output_msg(2);
	}

	try{
		//boundary condition
		this->transfer_boundary_data2database(ptr_database, id_glob);
	}
	catch(Error msg){
		ostringstream info;
		info << "Profile number            : " << this->profile_number << endl;
		info << "Profile name              : " << this->name << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
}
//Select and count the relevant river profiles in the database table and return the number (static)
int _Hyd_River_Profile::select_relevant_profiles_in_database(QSqlQueryModel *query_result, QSqlDatabase *ptr_database, const _sys_system_id id, const int rv_number, const bool with_output){
	int number=0;
	try{
		_Hyd_River_Profile::set_profile_table(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}
	if(with_output==true){
			ostringstream cout;
			cout << "Search for relevant river profiles in database ..." << endl ;
			Sys_Common_Output::output_hyd->output_txt(&cout);
	}

	ostringstream test_filter;
	test_filter<< "Select ";
	test_filter << _Hyd_River_Profile::profile_table->get_column_name(hyd_label::profdata_glob_id) << " , ";
	test_filter << _Hyd_River_Profile::profile_table->get_column_name(hyd_label::profdata_prof_id) << " , ";
	test_filter << _Hyd_River_Profile::profile_table->get_column_name(hyd_label::profdata_name) << " , ";
	test_filter << _Hyd_River_Profile::profile_table->get_column_name(hyd_label::profdata_ldist) << " , ";
	test_filter << _Hyd_River_Profile::profile_table->get_column_name(hyd_label::profdata_deltax) << " , ";
	test_filter << _Hyd_River_Profile::profile_table->get_column_name(hyd_label::profdata_contyp) << " , ";
	test_filter << _Hyd_River_Profile::profile_table->get_column_name(hyd_label::profdata_proftyp) << " , ";
	test_filter << _Hyd_River_Profile::profile_table->get_column_name(hyd_label::profdata_over_r) << " , ";
	test_filter << _Hyd_River_Profile::profile_table->get_column_name(hyd_label::profdata_over_l) << " , ";
	test_filter << _Hyd_River_Profile::profile_table->get_column_name(hyd_label::profdata_poleni_r) << " , ";
	test_filter << _Hyd_River_Profile::profile_table->get_column_name(hyd_label::profdata_poleni_l) << " , ";
	test_filter << _Hyd_River_Profile::profile_table->get_column_name(hyd_label::profdata_base_right) << " , ";
	test_filter << _Hyd_River_Profile::profile_table->get_column_name(hyd_label::profdata_base_left) << " , ";
	test_filter << _Hyd_River_Profile::profile_table->get_column_name(hyd_label::profdata_init);
	test_filter << " from " << _Hyd_River_Profile::profile_table->get_table_name();
	test_filter << " where ";
	test_filter << _Hyd_River_Profile::profile_table->get_column_name(label::applied_flag) << " = true";
	test_filter << " and ";
	test_filter << _Hyd_River_Profile::profile_table->get_column_name(label::areastate_id) << " = " << id.area_state;
	test_filter << " and (";
	test_filter << _Hyd_River_Profile::profile_table->get_column_name(label::measure_id) << " = " << 0 ;
	test_filter << " or " ;
	test_filter << _Hyd_River_Profile::profile_table->get_column_name(label::measure_id) << " = " << id.measure_nr;
	test_filter << " ) " ;
	test_filter << " and ";
	test_filter << _Hyd_River_Profile::profile_table->get_column_name(hyd_label::profdata_rvno) << " = " << rv_number;
	test_filter << " order by " << _Hyd_River_Profile::profile_table->get_column_name(hyd_label::profdata_ldist)<< " DESC ";

	Data_Base::database_request(query_result, test_filter.str(), ptr_database);

	//check the request
	if(query_result->lastError().isValid()){
		Error msg;
		msg.set_msg("_Hyd_River_Profile::select_relevant_profiles_in_database(QSqlTableModel *query_result, const _sys_system_id id, const int rv_number, const bool with_output)","Invalid database request", "Check the database", 2, false);
		ostringstream info;
		info << "Table Name      : " << _Hyd_River_Profile::profile_table->get_table_name() << endl;
		info << "Table error info: " << query_result->lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}

	number= query_result->rowCount();

	if(with_output==true){
		ostringstream cout;
		cout << number << " relevant river profile(s) are found in database" << endl ;
		Sys_Common_Output::output_hyd->output_txt(&cout);
	}
	return number;
}
//Select and count the relevant river profiles in the database table and return the number just for the given system_id (static)
int _Hyd_River_Profile::select_profiles_in_database(QSqlQueryModel *query_result, QSqlDatabase *ptr_database, const _sys_system_id id){
		int number=0;
	try{
		_Hyd_River_Profile::set_profile_table(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}


	ostringstream test_filter;
	test_filter<< "Select ";
	test_filter << _Hyd_River_Profile::profile_table->get_column_name(hyd_label::profdata_glob_id) << " , ";
	test_filter << _Hyd_River_Profile::profile_table->get_column_name(hyd_label::profdata_prof_id) << " , ";
	test_filter << _Hyd_River_Profile::profile_table->get_column_name(hyd_label::profdata_name) << " , ";
	test_filter << _Hyd_River_Profile::profile_table->get_column_name(hyd_label::profdata_ldist) << " , ";
	test_filter << _Hyd_River_Profile::profile_table->get_column_name(hyd_label::profdata_deltax) << " , ";
	test_filter << _Hyd_River_Profile::profile_table->get_column_name(hyd_label::profdata_contyp) << " , ";
	test_filter << _Hyd_River_Profile::profile_table->get_column_name(hyd_label::profdata_proftyp) << " , ";
	test_filter << _Hyd_River_Profile::profile_table->get_column_name(hyd_label::profdata_over_r) << " , ";
	test_filter << _Hyd_River_Profile::profile_table->get_column_name(hyd_label::profdata_over_l) << " , ";
	test_filter << _Hyd_River_Profile::profile_table->get_column_name(hyd_label::profdata_poleni_r) << " , ";
	test_filter << _Hyd_River_Profile::profile_table->get_column_name(hyd_label::profdata_poleni_l) << " , ";
	test_filter << _Hyd_River_Profile::profile_table->get_column_name(hyd_label::profdata_base_right) << " , ";
	test_filter << _Hyd_River_Profile::profile_table->get_column_name(hyd_label::profdata_base_left) << " , ";
	test_filter << _Hyd_River_Profile::profile_table->get_column_name(hyd_label::profdata_init);
	test_filter << " from " << _Hyd_River_Profile::profile_table->get_table_name();
	test_filter << " where ";
	test_filter << _Hyd_River_Profile::profile_table->get_column_name(label::areastate_id) << " = " << id.area_state;
	test_filter << " and ";
	test_filter << _Hyd_River_Profile::profile_table->get_column_name(label::measure_id) << " = " << id.measure_nr;

	Data_Base::database_request(query_result, test_filter.str(), ptr_database);

	//check the request
	if(query_result->lastError().isValid()){
		Error msg;
		msg.set_msg("_Hyd_River_Profile::select_profiles_in_database(QSqlQueryModel *query_result, QSqlDatabase *ptr_database, const _sys_system_id id)","Invalid database request", "Check the database", 2, false);
		ostringstream info;
		info << "Table Name      : " << _Hyd_River_Profile::profile_table->get_table_name() << endl;
		info << "Table error info: " << query_result->lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}

	number= query_result->rowCount();


	return number;
}
//Select and count the relevant river profiles in the database table and return the number just for the given system_id and river number (static)
int _Hyd_River_Profile::select_profiles_in_database(QSqlQueryModel *query_result, QSqlDatabase *ptr_database, const _sys_system_id id, const int rv_number){
	int number=0;
	try{
		_Hyd_River_Profile::set_profile_table(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}


	ostringstream test_filter;
	test_filter<< "Select ";
	test_filter << _Hyd_River_Profile::profile_table->get_column_name(hyd_label::profdata_glob_id) << " , ";
	test_filter << _Hyd_River_Profile::profile_table->get_column_name(hyd_label::profdata_prof_id) << " , ";
	test_filter << _Hyd_River_Profile::profile_table->get_column_name(hyd_label::profdata_name) << " , ";
	test_filter << _Hyd_River_Profile::profile_table->get_column_name(hyd_label::profdata_ldist) << " , ";
	test_filter << _Hyd_River_Profile::profile_table->get_column_name(hyd_label::profdata_deltax) << " , ";
	test_filter << _Hyd_River_Profile::profile_table->get_column_name(hyd_label::profdata_contyp) << " , ";
	test_filter << _Hyd_River_Profile::profile_table->get_column_name(hyd_label::profdata_proftyp) << " , ";
	test_filter << _Hyd_River_Profile::profile_table->get_column_name(hyd_label::profdata_over_r) << " , ";
	test_filter << _Hyd_River_Profile::profile_table->get_column_name(hyd_label::profdata_over_l) << " , ";
	test_filter << _Hyd_River_Profile::profile_table->get_column_name(hyd_label::profdata_poleni_r) << " , ";
	test_filter << _Hyd_River_Profile::profile_table->get_column_name(hyd_label::profdata_poleni_l) << " , ";
	test_filter << _Hyd_River_Profile::profile_table->get_column_name(hyd_label::profdata_base_right) << " , ";
	test_filter << _Hyd_River_Profile::profile_table->get_column_name(hyd_label::profdata_base_left) << " , ";
	test_filter << _Hyd_River_Profile::profile_table->get_column_name(hyd_label::profdata_init);
	test_filter << " from " << _Hyd_River_Profile::profile_table->get_table_name();
	test_filter << " where ";
	test_filter << _Hyd_River_Profile::profile_table->get_column_name(label::areastate_id) << " = " << id.area_state;
	test_filter << " and ";
	test_filter << _Hyd_River_Profile::profile_table->get_column_name(label::measure_id) << " = " << id.measure_nr;
	test_filter << " and ";
	test_filter << _Hyd_River_Profile::profile_table->get_column_name(hyd_label::profdata_rvno) << " = " << rv_number;
	test_filter << " order by " << _Hyd_River_Profile::profile_table->get_column_name(hyd_label::profdata_ldist)<< " DESC ";

	Data_Base::database_request(query_result, test_filter.str(), ptr_database);

	//check the request
	if(query_result->lastError().isValid()){
		Error msg;
		msg.set_msg("_Hyd_River_Profile::select_profiles_in_database(QSqlTableModel *query_result, const _sys_system_id id, const int rv_number, const bool with_output)","Invalid database request", "Check the database", 2, false);
		ostringstream info;
		info << "Table Name      : " << _Hyd_River_Profile::profile_table->get_table_name() << endl;
		info << "Table error info: " << query_result->lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}

	number= query_result->rowCount();


	return number;

}
//Select the relevant river profiles in the database table for a given connection type (static)
int _Hyd_River_Profile::select_relevant_profiles_in_database(QSqlQueryModel *query_result, QSqlDatabase *ptr_database, const _sys_system_id id, const int rv_number, _hyd_connection_types conn_type, const bool with_output){
	int number=0;
	try{
		_Hyd_River_Profile::set_profile_table(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}
	if(with_output==true){
			ostringstream cout;
			cout << "Search for relevant river profiles in database ..." << endl ;
			Sys_Common_Output::output_hyd->output_txt(&cout);
	}

	ostringstream test_filter;
	test_filter<< "Select ";
	test_filter << _Hyd_River_Profile::profile_table->get_column_name(hyd_label::profdata_glob_id) << " , ";
	test_filter << _Hyd_River_Profile::profile_table->get_column_name(hyd_label::profdata_prof_id) << " , ";
	test_filter << _Hyd_River_Profile::profile_table->get_column_name(hyd_label::profdata_name) << " , ";
	test_filter << _Hyd_River_Profile::profile_table->get_column_name(hyd_label::profdata_ldist) << " , ";
	test_filter << _Hyd_River_Profile::profile_table->get_column_name(hyd_label::profdata_deltax) << " , ";
	test_filter << _Hyd_River_Profile::profile_table->get_column_name(hyd_label::profdata_contyp) << " , ";
	test_filter << _Hyd_River_Profile::profile_table->get_column_name(hyd_label::profdata_proftyp) << " , ";
	test_filter << _Hyd_River_Profile::profile_table->get_column_name(hyd_label::profdata_over_r) << " , ";
	test_filter << _Hyd_River_Profile::profile_table->get_column_name(hyd_label::profdata_over_l) << " , ";
	test_filter << _Hyd_River_Profile::profile_table->get_column_name(hyd_label::profdata_poleni_r) << " , ";
	test_filter << _Hyd_River_Profile::profile_table->get_column_name(hyd_label::profdata_poleni_l) << " , ";
	test_filter << _Hyd_River_Profile::profile_table->get_column_name(hyd_label::profdata_base_right) << " , ";
	test_filter << _Hyd_River_Profile::profile_table->get_column_name(hyd_label::profdata_base_left) << " , ";
	test_filter << _Hyd_River_Profile::profile_table->get_column_name(hyd_label::profdata_init);
	test_filter << " from " << _Hyd_River_Profile::profile_table->get_table_name();
	test_filter << " where ";
	test_filter << _Hyd_River_Profile::profile_table->get_column_name(label::applied_flag) << " = true";
	test_filter << " and ";
	test_filter << _Hyd_River_Profile::profile_table->get_column_name(label::areastate_id) << " = " << id.area_state;
	test_filter << " and (";
	test_filter << _Hyd_River_Profile::profile_table->get_column_name(label::measure_id) << " = " << 0 ;
	test_filter << " or " ;
	test_filter << _Hyd_River_Profile::profile_table->get_column_name(label::measure_id) << " = " << id.measure_nr;
	test_filter << " ) " ;
	test_filter << " and ";
	test_filter << _Hyd_River_Profile::profile_table->get_column_name(hyd_label::profdata_rvno) << " = " << rv_number;
	test_filter << " and ";
	test_filter << _Hyd_River_Profile::profile_table->get_column_name(hyd_label::profdata_contyp) << " = '" << _Hyd_River_Profile::convert_connectiontype2string(conn_type)<<"'";

	Data_Base::database_request(query_result, test_filter.str(), ptr_database);

	//check the request
	if(query_result->lastError().isValid()){
		Error msg;
		msg.set_msg("_Hyd_River_Profile::select_relevant_profiles_in_database(QSqlTableModel *query_result, const _sys_system_id id, const int rv_number, _hyd_connection_types conn_type, const bool with_output=true )","Invalid database request", "Check the database", 2, false);
		ostringstream info;
		info << "Table Name      : " << _Hyd_River_Profile::profile_table->get_table_name() << endl;
		info << "Table error info: " << query_result->lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}

	number= query_result->rowCount();

	if(with_output==true){
		ostringstream cout;
		cout << number << " relevant river profile(s) are found in database" << endl ;
		Sys_Common_Output::output_hyd->output_txt(&cout);
	}
	return number;
}
//Select and count the relevant river profiles in the database table with a given global profile number (static)
int _Hyd_River_Profile::select_relevant_profile_in_database(QSqlQueryModel *query_result, QSqlDatabase *ptr_database, const _sys_system_id id, const int rv_number, const int glob_prof_no){
	int number=0;
	try{
		_Hyd_River_Profile::set_profile_table(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}

	ostringstream test_filter;
	test_filter<< "Select ";
	test_filter << _Hyd_River_Profile::profile_table->get_column_name(hyd_label::profdata_glob_id) << " , ";
	test_filter << _Hyd_River_Profile::profile_table->get_column_name(hyd_label::profdata_prof_id) << " , ";
	test_filter << _Hyd_River_Profile::profile_table->get_column_name(hyd_label::profdata_name) << " , ";
	test_filter << _Hyd_River_Profile::profile_table->get_column_name(hyd_label::profdata_ldist) << " , ";
	test_filter << _Hyd_River_Profile::profile_table->get_column_name(hyd_label::profdata_deltax) << " , ";
	test_filter << _Hyd_River_Profile::profile_table->get_column_name(hyd_label::profdata_contyp) << " , ";
	test_filter << _Hyd_River_Profile::profile_table->get_column_name(hyd_label::profdata_proftyp) << " , ";
	test_filter << _Hyd_River_Profile::profile_table->get_column_name(hyd_label::profdata_over_r) << " , ";
	test_filter << _Hyd_River_Profile::profile_table->get_column_name(hyd_label::profdata_over_l) << " , ";
	test_filter << _Hyd_River_Profile::profile_table->get_column_name(hyd_label::profdata_poleni_r) << " , ";
	test_filter << _Hyd_River_Profile::profile_table->get_column_name(hyd_label::profdata_poleni_l) << " , ";
	test_filter << _Hyd_River_Profile::profile_table->get_column_name(hyd_label::profdata_base_right) << " , ";
	test_filter << _Hyd_River_Profile::profile_table->get_column_name(hyd_label::profdata_base_left) << " , ";
	test_filter << _Hyd_River_Profile::profile_table->get_column_name(hyd_label::profdata_init);
	test_filter << " from " << _Hyd_River_Profile::profile_table->get_table_name();
	test_filter << " where ";
	test_filter << _Hyd_River_Profile::profile_table->get_column_name(label::applied_flag) << " = true";
	test_filter << " and ";
	test_filter << _Hyd_River_Profile::profile_table->get_column_name(label::areastate_id) << " = " << id.area_state;
	test_filter << " and (";
	test_filter << _Hyd_River_Profile::profile_table->get_column_name(label::measure_id) << " = " << 0 ;
	test_filter << " or " ;
	test_filter << _Hyd_River_Profile::profile_table->get_column_name(label::measure_id) << " = " << id.measure_nr;
	test_filter << " ) " ;
	test_filter << " and ";
	test_filter << _Hyd_River_Profile::profile_table->get_column_name(hyd_label::profdata_rvno) << " = " << rv_number;
	test_filter << " and ";
	test_filter << _Hyd_River_Profile::profile_table->get_column_name(hyd_label::profdata_glob_id) << " = " << glob_prof_no<<"";

	Data_Base::database_request(query_result, test_filter.str(), ptr_database);

	//check the request
	if(query_result->lastError().isValid()){
		Error msg;
		msg.set_msg("_Hyd_River_Profile::select_relevant_profiles_in_database(QSqlTableModel *query_result, const _sys_system_id id, const int rv_number, const int glob_prof_no)","Invalid database request", "Check the database", 2, false);
		ostringstream info;
		info << "Table Name      : " << _Hyd_River_Profile::profile_table->get_table_name() << endl;
		info << "Table error info: " << query_result->lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}

	number= query_result->rowCount();

	return number;
}
//Get the maximum discharge of the outflow profile (static)
double _Hyd_River_Profile::get_maximum_outflow_discharge(QSqlDatabase *ptr_database, const _sys_system_id id, const int rv_number,const int bound_sz, const string break_sz){
	double discharge=-1.0;

	QSqlQueryModel model;
	int number=0;
	number=_Hyd_River_Profile::select_relevant_profiles_in_database(&model, ptr_database, id, rv_number, _hyd_connection_types::OUTFLOW_CONN, false);
	//nothing found
	if(number==0){
		return discharge;
	}
	//search for the outflow profile number
	int profile_id;
	//read out the results of the request for this index
	profile_id=model.record(0).value((_Hyd_River_Profile::profile_table->get_column_name(hyd_label::profdata_prof_id)).c_str()).toInt();
	//select the result
	 _Hyd_River_Profile::select_data_in_erg_table(&model, ptr_database, id, bound_sz, break_sz, rv_number, profile_id);
	discharge=model.record(0).value((_Hyd_River_Profile::erg_table->get_column_name(hyd_label::proferg_q_max)).c_str()).toDouble();

	return discharge;
}
//Set the database table for the profile data of the river model: it sets the table name and the name of the columns and allocate them (static)
void _Hyd_River_Profile::set_profile_table(QSqlDatabase *ptr_database){
	if(_Hyd_River_Profile::profile_table==NULL){
		//make specific input for this class
		const string tab_id_name=hyd_label::tab_rvprof;

		string tab_id_col[21];
		tab_id_col[0]=hyd_label::profdata_rvno;
		tab_id_col[1]=hyd_label::profdata_name;
		tab_id_col[2]=hyd_label::profdata_ldist;
		tab_id_col[3]=hyd_label::profdata_deltax;
		tab_id_col[4]=hyd_label::profdata_contyp;
		tab_id_col[5]=hyd_label::profdata_proftyp;
		tab_id_col[6]=hyd_label::profdata_init;
		tab_id_col[7]=hyd_label::profdata_prof_id;
		tab_id_col[8]=hyd_label::profdata_glob_id;
		tab_id_col[9]=hyd_label::profdata_over_r;
		tab_id_col[10]=hyd_label::profdata_over_l;
		tab_id_col[11]=hyd_label::profdata_poleni_r;
		tab_id_col[12]=hyd_label::profdata_poleni_l;
		tab_id_col[13]=hyd_label::profdata_base_left;
		tab_id_col[14]=hyd_label::profdata_base_right;
		tab_id_col[15]=hyd_label::profdata_polyline;
		tab_id_col[16]=label::description;
		tab_id_col[17]=label::link;
		tab_id_col[18]=label::areastate_id;
		tab_id_col[19]=label::measure_id;
		tab_id_col[20]=label::applied_flag;

		try{
			_Hyd_River_Profile::profile_table= new Tables(tab_id_name, tab_id_col, sizeof(tab_id_col)/sizeof(tab_id_col[0]));
			_Hyd_River_Profile::profile_table->set_name(ptr_database, _sys_table_type::hyd);
		}
		catch(bad_alloc& t){
			Error msg;
			msg.set_msg("_Hyd_River_Profile::set_profile_table(QSqlDatabase *ptr_database)","Can not allocate the memory", "Check the memory", 10, false);
			ostringstream info;
			info<< "Info bad alloc: " << t.what() << endl;
			msg.make_second_info(info.str());
			throw msg;
		}
		catch(Error msg){
			_Hyd_River_Profile::close_profile_table();
			throw msg;
		}
	}
}
//Set the database table for the boundary profile data: it sets the table name and the name of the columns and allocate them (static)
void _Hyd_River_Profile::set_profile_boundary_table(QSqlDatabase *ptr_database){
	if(_Hyd_River_Profile::boundary_table==NULL){
		//make specific input for this class
		const string tab_id_name=hyd_label::tab_rvprof_bound;

		string tab_id_col[8];
		tab_id_col[0]=hyd_label::profdata_glob_id;
		tab_id_col[1]=label::areastate_id;
		tab_id_col[2]=label::measure_id;
		tab_id_col[3]=hyd_label::sz_bound_id;
		tab_id_col[4]=hyd_label::bounddata_stat;
		tab_id_col[5]=hyd_label::bounddata_value;
		tab_id_col[6]=hyd_label::bounddata_type;
		tab_id_col[7]=label::applied_flag;

		try{
			_Hyd_River_Profile::boundary_table= new Tables(tab_id_name, tab_id_col, sizeof(tab_id_col)/sizeof(tab_id_col[0]));
			_Hyd_River_Profile::boundary_table->set_name(ptr_database, _sys_table_type::hyd);
		}
		catch(bad_alloc& t){
			Error msg;
			msg.set_msg("_Hyd_River_Profile::set_profile_boundary_table(QSqlDatabase *ptr_database)","Can not allocate the memory", "Check the memory", 10, false);
			ostringstream info;
			info<< "Info bad alloc: " << t.what() << endl;
			msg.make_second_info(info.str());
			throw msg;
		}
		catch(Error msg){
			_Hyd_River_Profile::close_boundary_table();
			throw msg;
		}
	}
}
//Set the database table for the wallbreak profile data: it sets the table name and the name of the columns and allocate them (static)
void _Hyd_River_Profile::set_profile_wallbreak_table(QSqlDatabase *ptr_database){
	if(_Hyd_River_Profile::wallbreak_table==NULL){
		//make specific input for this class
		const string tab_id_name=hyd_label::tab_rvprof_wallbreak;

		string tab_id_col[4];
		tab_id_col[0]=hyd_label::profdata_glob_id;
		tab_id_col[1]=hyd_label::profbreak_left;
		tab_id_col[2]=hyd_label::profwall_res;
		tab_id_col[3]=hyd_label::profwall_abrupt;

		try{
			_Hyd_River_Profile::wallbreak_table= new Tables(tab_id_name, tab_id_col, sizeof(tab_id_col)/sizeof(tab_id_col[0]));
			_Hyd_River_Profile::wallbreak_table->set_name(ptr_database, _sys_table_type::hyd);
		}
		catch(bad_alloc& t){
			Error msg;
			msg.set_msg("_Hyd_River_Profile::set_profile_wallbreak_table(QSqlDatabase *ptr_database)","Can not allocate the memory", "Check the memory", 10, false);
			ostringstream info;
			info<< "Info bad alloc: " << t.what() << endl;
			msg.make_second_info(info.str());
			throw msg;
		}
		catch(Error msg){
			_Hyd_River_Profile::close_wallbreak_table();
			throw msg;
		}
	}
}
//Set the database table for the dikebreak profile data: it sets the table name and the name of the columns and allocate them (static)
void _Hyd_River_Profile::set_profile_dikebreak_table(QSqlDatabase *ptr_database){
	if(_Hyd_River_Profile::dikebreak_table==NULL){
		//make specific input for this class
		const string tab_id_name=hyd_label::tab_rvprof_dikebreak;

		string tab_id_col[3];
		tab_id_col[0]=hyd_label::profdata_glob_id;
		tab_id_col[1]=hyd_label::profbreak_left;
		tab_id_col[2]=hyd_label::profdike_crit_v;

		try{
			_Hyd_River_Profile::dikebreak_table= new Tables(tab_id_name, tab_id_col, sizeof(tab_id_col)/sizeof(tab_id_col[0]));
			_Hyd_River_Profile::dikebreak_table->set_name(ptr_database, _sys_table_type::hyd);
		}
		catch(bad_alloc& t){
			Error msg;
			msg.set_msg("_Hyd_River_Profile::set_profile_dikebreak_table(QSqlDatabase *ptr_database)","Can not allocate the memory", "Check the memory", 10, false);
			ostringstream info;
			info<< "Info bad alloc: " << t.what() << endl;
			msg.make_second_info(info.str());
			throw msg;
		}
		catch(Error msg){
			_Hyd_River_Profile::close_dikebreak_table();
			throw msg;
		}
	}
}
//Set the database table for the bridge profile data: it sets the table name and the name of the columns and allocate them (static)
void _Hyd_River_Profile::set_profile_bridge_table(QSqlDatabase *ptr_database){
	if(_Hyd_River_Profile::bridge_table==NULL){
		//make specific input for this class
		const string tab_id_name=hyd_label::tab_rvprof_bridge;

		string tab_id_col[3];
		tab_id_col[0]=hyd_label::profdata_glob_id;
		tab_id_col[1]=hyd_label::profbridge_size;
		tab_id_col[2]=hyd_label::profbridge_height;

		try{
			_Hyd_River_Profile::bridge_table= new Tables(tab_id_name, tab_id_col, sizeof(tab_id_col)/sizeof(tab_id_col[0]));
			_Hyd_River_Profile::bridge_table->set_name(ptr_database, _sys_table_type::hyd);
		}
		catch(bad_alloc& t){
			Error msg;
			msg.set_msg("_Hyd_River_Profile::set_profile_bridge_table(QSqlDatabase *ptr_database)","Can not allocate the memory", "Check the memory", 10, false);
			ostringstream info;
			info<< "Info bad alloc: " << t.what() << endl;
			msg.make_second_info(info.str());
			throw msg;
		}
		catch(Error msg){
			_Hyd_River_Profile::close_bridge_table();
			throw msg;
		}
	}
}
//Create the database table for the profile data of the river model (static)
void _Hyd_River_Profile::create_profile_table(QSqlDatabase *ptr_database){
	if(_Hyd_River_Profile::profile_table==NULL){
		ostringstream cout;
		cout << "Create river profile database table..." << endl ;
		Sys_Common_Output::output_hyd->output_txt(&cout);
		//make specific input for this class
		const string tab_name=hyd_label::tab_rvprof;
		const int num_col=21;
		_Sys_data_tab_column tab_col[num_col];
		//init
		for(int i=0; i< num_col; i++){
			tab_col[i].key_flag=false;
			tab_col[i].unsigned_flag=false;
			tab_col[i].primary_key_flag=false;
		}

		tab_col[0].name=hyd_label::profdata_glob_id;
		tab_col[0].type=sys_label::tab_col_type_int;
		tab_col[0].unsigned_flag=true;
		tab_col[0].primary_key_flag=true;

		tab_col[1].name=hyd_label::profdata_rvno;
		tab_col[1].type=sys_label::tab_col_type_int;
		tab_col[1].unsigned_flag=true;
		tab_col[1].key_flag=true;

		tab_col[2].name=hyd_label::profdata_prof_id;
		tab_col[2].type=sys_label::tab_col_type_int;
		tab_col[2].unsigned_flag=true;

		tab_col[3].name=label::areastate_id;
		tab_col[3].type=sys_label::tab_col_type_int;
		tab_col[3].unsigned_flag=true;
		tab_col[3].key_flag=true;

		tab_col[4].name=label::measure_id;
		tab_col[4].type=sys_label::tab_col_type_int;
		tab_col[4].unsigned_flag=true;
		tab_col[4].key_flag=true;

		tab_col[5].name=label::applied_flag;
		tab_col[5].type=sys_label::tab_col_type_bool;
		tab_col[5].default_value="true";
		tab_col[5].key_flag=true;

		tab_col[6].name=hyd_label::profdata_name;
		tab_col[6].type=sys_label::tab_col_type_string;

		tab_col[7].name=hyd_label::profdata_ldist;
		tab_col[7].type=sys_label::tab_col_type_double;

		tab_col[8].name=hyd_label::profdata_deltax;
		tab_col[8].type=sys_label::tab_col_type_double;
		tab_col[8].unsigned_flag=true;
		tab_col[8].default_value="0.1";

		tab_col[9].name=hyd_label::profdata_contyp;
		tab_col[9].type=sys_label::tab_col_type_string;

		tab_col[10].name=hyd_label::profdata_proftyp;
		tab_col[10].type=sys_label::tab_col_type_string;

		tab_col[11].name=hyd_label::profdata_init;
		tab_col[11].type=sys_label::tab_col_type_double;
		tab_col[11].unsigned_flag=true;
		tab_col[11].default_value="0";

		tab_col[12].name=hyd_label::profdata_over_r;
		tab_col[12].type=sys_label::tab_col_type_bool;
		tab_col[12].default_value="false";

		tab_col[13].name=hyd_label::profdata_over_l;
		tab_col[13].type=sys_label::tab_col_type_bool;
		tab_col[13].default_value="false";

		tab_col[14].name=hyd_label::profdata_poleni_r;
		tab_col[14].type=sys_label::tab_col_type_double;
		tab_col[14].unsigned_flag=true;
		tab_col[14].default_value="0.577";

		tab_col[15].name=hyd_label::profdata_poleni_l;
		tab_col[15].type=sys_label::tab_col_type_double;
		tab_col[15].unsigned_flag=true;
		tab_col[15].default_value="0.577";

		tab_col[16].name=hyd_label::profdata_base_left;
		tab_col[16].type=sys_label::tab_col_type_int;
		tab_col[16].default_value="-1";

		tab_col[17].name=hyd_label::profdata_base_right;
		tab_col[17].type=sys_label::tab_col_type_int;
		tab_col[17].default_value="-1";

		tab_col[18].name=hyd_label::profdata_polyline;
		tab_col[18].type=sys_label::tab_col_type_polyline;

		tab_col[19].name=label::description;
		tab_col[19].type=sys_label::tab_col_type_string;

		tab_col[20].name=label::link;
		tab_col[20].type=sys_label::tab_col_type_string;

		try{
			_Hyd_River_Profile::profile_table= new Tables();
			if(_Hyd_River_Profile::profile_table->create_non_existing_tables(tab_name, tab_col, num_col,ptr_database, _sys_table_type::hyd)==false){
				cout << " Table exists" << endl ;
				Sys_Common_Output::output_hyd->output_txt(&cout);
			};
		}
		catch(bad_alloc& t){
			Error msg;
			msg.set_msg("_Hyd_River_Profile::create_profile_table(QSqlDatabase *ptr_database)","Can not allocate the memory", "Check the memory", 10, false);
			ostringstream info;
			info<< "Info bad alloc: " << t.what() << endl;
			msg.make_second_info(info.str());
			throw msg;
		}
		catch(Error msg){
			_Hyd_River_Profile::close_profile_table();
			throw msg;
		}

	_Hyd_River_Profile::close_profile_table();
	}
}
//Get the maximum value of the global index of the profiles in database table (static)
int _Hyd_River_Profile::get_max_glob_id_prof_table(QSqlDatabase *ptr_database){
	//the table is set (the name and the column names) and allocated
	try{
		_Hyd_River_Profile::set_profile_table(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}

	//evaluate the global identifier
	int id_glob=_Hyd_River_Profile::profile_table->maximum_int_of_column(_Hyd_River_Profile::profile_table->get_column_name(hyd_label::profdata_glob_id),ptr_database);
	return id_glob;
}
//Create the database table for the boundary profile data (static)
void _Hyd_River_Profile::create_profile_boundary_table(QSqlDatabase *ptr_database){
	if(_Hyd_River_Profile::boundary_table==NULL){
		ostringstream cout;
		cout << "Create river profile boundary database table..." << endl ;
		Sys_Common_Output::output_hyd->output_txt(&cout);
		//make specific input for this class
		const string tab_name=hyd_label::tab_rvprof_bound;
		const int num_col=8;
		_Sys_data_tab_column tab_col[num_col];
		//init
		for(int i=0; i< num_col; i++){
			tab_col[i].key_flag=false;
			tab_col[i].unsigned_flag=false;
			tab_col[i].primary_key_flag=false;
		}
		tab_col[0].name=hyd_label::profdata_glob_id;
		tab_col[0].type=sys_label::tab_col_type_int;
		tab_col[0].unsigned_flag=true;
		tab_col[0].key_flag=true;

		tab_col[1].name=label::areastate_id;
		tab_col[1].type=sys_label::tab_col_type_int;
		tab_col[1].unsigned_flag=true;
		tab_col[1].default_value="0";
		tab_col[1].key_flag=true;

		tab_col[2].name=label::measure_id;
		tab_col[2].type=sys_label::tab_col_type_int;
		tab_col[2].unsigned_flag=true;
		tab_col[2].default_value="0";
		tab_col[2].key_flag=true;

		tab_col[3].name=label::applied_flag;
		tab_col[3].type=sys_label::tab_col_type_bool;
		tab_col[3].default_value="true";
		tab_col[3].key_flag=true;

		tab_col[4].name=hyd_label::sz_bound_id;
		tab_col[4].type=sys_label::tab_col_type_int;
		tab_col[4].unsigned_flag=true;
		tab_col[4].default_value="0";
		tab_col[4].key_flag=true;

		tab_col[5].name=hyd_label::bounddata_stat;
		tab_col[5].type=sys_label::tab_col_type_bool;
		tab_col[5].default_value="false";

		tab_col[6].name=hyd_label::bounddata_value;
		tab_col[6].type=sys_label::tab_col_type_double;
		tab_col[6].default_value="0";

		tab_col[7].name=hyd_label::bounddata_type;
		tab_col[7].type=sys_label::tab_col_type_string;

		try{
			_Hyd_River_Profile::boundary_table= new Tables();
			if(_Hyd_River_Profile::boundary_table->create_non_existing_tables(tab_name, tab_col, num_col,ptr_database, _sys_table_type::hyd)==false){
				cout << " Table exists" << endl ;
				Sys_Common_Output::output_hyd->output_txt(&cout);
			};
		}
		catch(bad_alloc& t){
			Error msg;
			msg.set_msg("_Hyd_River_Profile::create_profile_boundary_table(QSqlDatabase *ptr_database)","Can not allocate the memory", "Check the memory", 10, false);
			ostringstream info;
			info<< "Info bad alloc: " << t.what() << endl;
			msg.make_second_info(info.str());
			throw msg;
		}
		catch(Error msg){
			_Hyd_River_Profile::close_boundary_table();
			throw msg;
		}

	_Hyd_River_Profile::close_boundary_table();
	}
}
//Create the database view for the connection of boundary ids and theriver profiles (static)
void _Hyd_River_Profile::create_bound2profile_view(QSqlDatabase *ptr_database) {
	//Set tables
	ostringstream cout;
	//the table is set (the name and the column names) and allocated
	try {
		_Hyd_River_Profile::set_profile_table(ptr_database);
		_Hyd_River_Profile::set_profile_boundary_table(ptr_database);
	}
	catch (Error msg) {
		throw msg;
	}


	cout << "Create database view to river profile element with boundary conditions..." << endl;
	Sys_Common_Output::output_hyd->output_txt(&cout);
	//make specific input for this class
	const string view_name = hyd_label::view_rvprofile2bound;

	QSqlQueryModel query;

	ostringstream query_string;
	query_string << "CREATE VIEW ";
	query_string << Sys_Project::get_complete_project_database_schemata_name() << "." << hyd_label::view_rvprofile2bound;
	query_string << " AS SELECT ";
	query_string << "row_number() over() AS Id" << " , ";
	query_string << _Hyd_River_Profile::profile_table->get_column_name_table(hyd_label::profdata_glob_id) << " , ";
	query_string << _Hyd_River_Profile::profile_table->get_column_name_table(hyd_label::profdata_rvno) << " , ";
	query_string << _Hyd_River_Profile::boundary_table->get_column_name_table(label::areastate_id) << " , ";
	query_string << _Hyd_River_Profile::boundary_table->get_column_name_table(label::measure_id) << " , ";
	query_string << _Hyd_River_Profile::boundary_table->get_column_name_table(hyd_label::sz_bound_id) << " , ";
	query_string << _Hyd_River_Profile::boundary_table->get_column_name_table(label::applied_flag) << " , ";

	query_string << _Hyd_River_Profile::boundary_table->get_column_name_table(hyd_label::bounddata_stat) << " , ";
	query_string << _Hyd_River_Profile::boundary_table->get_column_name_table(hyd_label::bounddata_value) << " , ";
	query_string << _Hyd_River_Profile::boundary_table->get_column_name_table(hyd_label::bounddata_type) << " , ";

	query_string << _Hyd_River_Profile::profile_table->get_column_name_table(hyd_label::profdata_polyline) << "  ";

	query_string << " FROM ";
	query_string << _Hyd_River_Profile::boundary_table->get_table_name() << "  ";
	query_string << " JOIN ";
	query_string << _Hyd_River_Profile::profile_table->get_table_name() << "  ";
	query_string << " ON ";
	query_string << _Hyd_River_Profile::boundary_table->get_column_name_table(hyd_label::profdata_glob_id) << "  ";
	query_string << " = ";
	query_string << _Hyd_River_Profile::profile_table->get_column_name_table(hyd_label::profdata_glob_id) << "  ";


	Data_Base::database_request(&query, query_string.str(), ptr_database);
	if (query.lastError().isValid() == true) {
		Error msg;
		msg.set_msg("create_bound2profile_view(QSqlDatabase *ptr_database)", "Invalid database request", "Check the database", 2, false);
		ostringstream info;
		info << "View Name      : " << hyd_label::view_rvprofile2bound << endl;
		info << "View error info: " << query.lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}


}
//Check if the view exists already (static)
bool _Hyd_River_Profile::check_bound2profile_view_exists(QSqlDatabase *ptr_database) {
	QSqlQueryModel query;

	ostringstream query_string;
	query_string << "SELECT EXISTS ( SELECT * FROM information_schema.tables ";
	query_string << "WHERE table_schema ='" << Sys_Project::get_complete_project_database_schemata_name() << "' ";
	query_string << "AND table_name ='" << functions::convert_string2lower_case(hyd_label::view_rvprofile2bound) << "' )";


	Data_Base::database_request(&query, query_string.str(), ptr_database);
	if (query.lastError().isValid() == true) {
		Error msg;
		msg.set_msg("_Hyd_River_Profile::check_bound2profile_view_exists(QSqlDatabase *ptr_database)", "Invalid database request", "Check the database", 2, false);
		ostringstream info;
		info << "View Name      : " << hyd_label::view_rvprofile2bound << endl;
		info << "View error info: " << query.lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}


	if (query.rowCount() > 0) {
		return true;
	}

	return false;

}
//Create the database table for the wallbreak profile data (static)
void _Hyd_River_Profile::create_profile_wallbreak_table(QSqlDatabase *ptr_database){
	if(_Hyd_River_Profile::wallbreak_table==NULL){
		ostringstream cout;
		cout << "Create river profile wallbreak database table..." << endl ;
		Sys_Common_Output::output_hyd->output_txt(&cout);
		//make specific input for this class
		const string tab_name=hyd_label::tab_rvprof_wallbreak;
		const int num_col=4;
		_Sys_data_tab_column tab_col[num_col];
		//init
		for(int i=0; i< num_col; i++){
			tab_col[i].key_flag=false;
			tab_col[i].unsigned_flag=false;
			tab_col[i].primary_key_flag=false;
		}
		tab_col[0].name=hyd_label::profdata_glob_id;
		tab_col[0].type=sys_label::tab_col_type_int;
		tab_col[0].unsigned_flag=true;
		tab_col[0].primary_key_flag=true;

		tab_col[1].name=hyd_label::profbreak_left;
		tab_col[1].type=sys_label::tab_col_type_bool;
		tab_col[1].default_value="false";
		tab_col[1].primary_key_flag=true;

		tab_col[2].name=hyd_label::profwall_res;
		tab_col[2].type=sys_label::tab_col_type_double;
		tab_col[2].default_value="100.0";

		tab_col[3].name=hyd_label::profwall_abrupt;
		tab_col[3].type=sys_label::tab_col_type_double;
		tab_col[3].default_value="1.0";

		try{
			_Hyd_River_Profile::wallbreak_table= new Tables();
			if(_Hyd_River_Profile::wallbreak_table->create_non_existing_tables(tab_name, tab_col, num_col,ptr_database, _sys_table_type::hyd)==false){
				cout << " Table exists" << endl ;
				Sys_Common_Output::output_hyd->output_txt(&cout);
			};
		}
		catch(bad_alloc& t){
			Error msg;
			msg.set_msg("_Hyd_River_Profile::create_profile_wallbreak_table(QSqlDatabase *ptr_database)","Can not allocate the memory", "Check the memory", 10, false);
			ostringstream info;
			info<< "Info bad alloc: " << t.what() << endl;
			msg.make_second_info(info.str());
			throw msg;
		}
		catch(Error msg){
			_Hyd_River_Profile::close_wallbreak_table();
			throw msg;
		}

	_Hyd_River_Profile::close_wallbreak_table();
	}
}
//Create the database table for the dikebreak profile data (static)
void _Hyd_River_Profile::create_profile_dikebreak_table(QSqlDatabase *ptr_database){
	if(_Hyd_River_Profile::dikebreak_table==NULL){
		ostringstream cout;
		cout << "Create river profile dikebreak database table..." << endl ;
		Sys_Common_Output::output_hyd->output_txt(&cout);
		//make specific input for this class
		const string tab_name=hyd_label::tab_rvprof_dikebreak;
		const int num_col=3;
		_Sys_data_tab_column tab_col[num_col];
		//init
		for(int i=0; i< num_col; i++){
			tab_col[i].key_flag=false;
			tab_col[i].unsigned_flag=false;
			tab_col[i].primary_key_flag=false;
		}
		tab_col[0].name=hyd_label::profdata_glob_id;
		tab_col[0].type=sys_label::tab_col_type_int;
		tab_col[0].unsigned_flag=true;
		tab_col[0].primary_key_flag=true;

		tab_col[1].name=hyd_label::profbreak_left;
		tab_col[1].type=sys_label::tab_col_type_bool;
		tab_col[1].default_value="false";
		tab_col[1].primary_key_flag=true;

		tab_col[2].name=hyd_label::profdike_crit_v;
		tab_col[2].type=sys_label::tab_col_type_double;
		tab_col[2].default_value="0.1";

		try{
			_Hyd_River_Profile::dikebreak_table= new Tables();
			if(_Hyd_River_Profile::dikebreak_table->create_non_existing_tables(tab_name, tab_col, num_col,ptr_database, _sys_table_type::hyd)==false){
				cout << " Table exists" << endl ;
				Sys_Common_Output::output_hyd->output_txt(&cout);
			};
		}
		catch(bad_alloc& t){
			Error msg;
			msg.set_msg("_Hyd_River_Profile::create_profile_dikebreak_table(QSqlDatabase *ptr_database)","Can not allocate the memory", "Check the memory", 10, false);
			ostringstream info;
			info<< "Info bad alloc: " << t.what() << endl;
			msg.make_second_info(info.str());
			throw msg;
		}
		catch(Error msg){
			_Hyd_River_Profile::close_dikebreak_table();
			throw msg;
		}

	_Hyd_River_Profile::close_dikebreak_table();
	}
}
//Create the database table for the bridge profile data (static)
void _Hyd_River_Profile::create_profile_bridge_table(QSqlDatabase *ptr_database){
	if(_Hyd_River_Profile::bridge_table==NULL){
		ostringstream cout;
		cout << "Create river profile bridge database table..." << endl ;
		Sys_Common_Output::output_hyd->output_txt(&cout);
		//make specific input for this class
		const string tab_name=hyd_label::tab_rvprof_bridge;
		const int num_col=3;
		_Sys_data_tab_column tab_col[num_col];
		//init
		for(int i=0; i< num_col; i++){
			tab_col[i].key_flag=false;
			tab_col[i].unsigned_flag=false;
			tab_col[i].primary_key_flag=false;
		}
		tab_col[0].name=hyd_label::profdata_glob_id;
		tab_col[0].type=sys_label::tab_col_type_int;
		tab_col[0].unsigned_flag=true;
		tab_col[0].primary_key_flag=true;

		tab_col[1].name=hyd_label::profbridge_size;
		tab_col[1].type=sys_label::tab_col_type_double;
		tab_col[1].default_value="0.2";

		tab_col[2].name=hyd_label::profbridge_height;
		tab_col[2].type=sys_label::tab_col_type_double;
		tab_col[2].default_value="0.0";

		try{
			_Hyd_River_Profile::bridge_table= new Tables();
			if(_Hyd_River_Profile::bridge_table->create_non_existing_tables(tab_name, tab_col, num_col,ptr_database, _sys_table_type::hyd)==false){
				cout << " Table exists" << endl ;
				Sys_Common_Output::output_hyd->output_txt(&cout);
			};
		}
		catch(bad_alloc& t){
			Error msg;
			msg.set_msg("_Hyd_River_Profile::create_profile_bridge_table(QSqlDatabase *ptr_database)","Can not allocate the memory", "Check the memory", 10, false);
			ostringstream info;
			info<< "Info bad alloc: " << t.what() << endl;
			msg.make_second_info(info.str());
			throw msg;
		}
		catch(Error msg){
			_Hyd_River_Profile::close_bridge_table();
			throw msg;
		}

	_Hyd_River_Profile::close_bridge_table();
	}
}
//Create the database table for the results of an hydraulic simulation for the river profiles (static)
void _Hyd_River_Profile::create_erg_table(QSqlDatabase *ptr_database){
	if(_Hyd_River_Profile::erg_table==NULL){
		ostringstream cout;
		cout << "Create river profile result database table..." << endl ;
		Sys_Common_Output::output_hyd->output_txt(&cout);
		//make specific input for this class
		const string tab_name=hyd_label::tab_rvprof_erg_max;
		const int num_col=33;
		_Sys_data_tab_column tab_col[num_col];
		//init
		for(int i=0; i< num_col; i++){
			tab_col[i].key_flag=false;
			tab_col[i].unsigned_flag=false;
			tab_col[i].primary_key_flag=false;
		}

		tab_col[0].name=label::glob_id;
		tab_col[0].type=sys_label::tab_col_type_int;
		tab_col[0].unsigned_flag=true;
		tab_col[0].primary_key_flag=true;

		tab_col[1].name=hyd_label::profdata_rvno;
		tab_col[1].type=sys_label::tab_col_type_int;
		tab_col[1].unsigned_flag=true;
		tab_col[1].key_flag=true;

		tab_col[2].name=hyd_label::profdata_prof_id;
		tab_col[2].type=sys_label::tab_col_type_int;
		tab_col[2].unsigned_flag=true;
		tab_col[2].key_flag=true;

		tab_col[3].name=label::areastate_id;
		tab_col[3].type=sys_label::tab_col_type_int;
		tab_col[3].unsigned_flag=true;
		tab_col[3].key_flag=true;

		tab_col[4].name=label::measure_id;
		tab_col[4].type=sys_label::tab_col_type_int;
		tab_col[4].unsigned_flag=true;
		tab_col[4].key_flag=true;

		tab_col[5].name=label::applied_flag;
		tab_col[5].type=sys_label::tab_col_type_bool;
		tab_col[5].default_value="true";
		tab_col[5].key_flag=true;

		tab_col[6].name=hyd_label::sz_bound_id;
		tab_col[6].type=sys_label::tab_col_type_int;
		tab_col[6].unsigned_flag=true;
		tab_col[6].key_flag=true;

		tab_col[7].name=risk_label::sz_break_id;
		tab_col[7].type=sys_label::tab_col_type_string;
		tab_col[7].key_flag=true;

		tab_col[8].name=hyd_label::proferg_h_max;
		tab_col[8].type=sys_label::tab_col_type_double;
		tab_col[8].unsigned_flag=true;
		tab_col[8].default_value="0.0";

		tab_col[9].name=hyd_label::proferg_s_max;
		tab_col[9].type=sys_label::tab_col_type_double;
		tab_col[9].unsigned_flag=true;
		tab_col[9].default_value="0.0";

		tab_col[10].name=hyd_label::proferg_hmax_lb;
		tab_col[10].type=sys_label::tab_col_type_double;
		tab_col[10].unsigned_flag=true;
		tab_col[10].default_value="0.0";

		tab_col[11].name=hyd_label::proferg_hmax_rb;
		tab_col[11].type=sys_label::tab_col_type_double;
		tab_col[11].unsigned_flag=true;
		tab_col[11].default_value="0.0";

		tab_col[12].name=hyd_label::proferg_t_hmax;
		tab_col[12].type=sys_label::tab_col_type_double;
		tab_col[12].unsigned_flag=true;
		tab_col[12].default_value="0.0";

		tab_col[13].name=hyd_label::proferg_hmax_lb_break;
		tab_col[13].type=sys_label::tab_col_type_double;
		tab_col[13].unsigned_flag=true;
		tab_col[13].default_value="0.0";

		tab_col[14].name=hyd_label::proferg_hmax_rb_break;
		tab_col[14].type=sys_label::tab_col_type_double;
		tab_col[14].unsigned_flag=true;
		tab_col[14].default_value="0.0";

		tab_col[15].name=hyd_label::proferg_v_max;
		tab_col[15].type=sys_label::tab_col_type_double;
		tab_col[15].default_value="0.0";

		tab_col[16].name = hyd_label::proferg_width_max;
		tab_col[16].type = sys_label::tab_col_type_double;
		tab_col[16].default_value = "0.0";

		tab_col[17].name=hyd_label::proferg_dur_wet;
		tab_col[17].type=sys_label::tab_col_type_double;
		tab_col[17].unsigned_flag=true;
		tab_col[17].default_value="0.0";

		tab_col[18].name=hyd_label::proferg_dur_dry;
		tab_col[18].type=sys_label::tab_col_type_double;
		tab_col[18].unsigned_flag=true;
		tab_col[18].default_value="0.0";

		tab_col[19].name=hyd_label::proferg_cv_rv_in;
		tab_col[19].type=sys_label::tab_col_type_double;
		tab_col[19].default_value="0.0";

		tab_col[20].name=hyd_label::proferg_cv_rv_out;
		tab_col[20].type=sys_label::tab_col_type_double;
		tab_col[20].default_value="0.0";

		tab_col[21].name=hyd_label::proferg_cv_struc_in;
		tab_col[21].type=sys_label::tab_col_type_double;
		tab_col[21].default_value="0.0";

		tab_col[22].name=hyd_label::proferg_cv_struc_out;
		tab_col[22].type=sys_label::tab_col_type_double;
		tab_col[22].default_value="0.0";

		tab_col[23].name=hyd_label::proferg_cv_l_ov_in;
		tab_col[23].type=sys_label::tab_col_type_double;
		tab_col[23].default_value="0.0";

		tab_col[24].name=hyd_label::proferg_cv_l_ov_out;
		tab_col[24].type=sys_label::tab_col_type_double;
		tab_col[24].default_value="0.0";

		tab_col[25].name=hyd_label::proferg_cv_r_ov_in;
		tab_col[25].type=sys_label::tab_col_type_double;
		tab_col[25].default_value="0.0";

		tab_col[26].name=hyd_label::proferg_cv_r_ov_out;
		tab_col[26].type=sys_label::tab_col_type_double;
		tab_col[26].default_value="0.0";

		tab_col[27].name=hyd_label::proferg_cv_l_db_in;
		tab_col[27].type=sys_label::tab_col_type_double;
		tab_col[27].default_value="0.0";

		tab_col[28].name=hyd_label::proferg_cv_l_db_out;
		tab_col[28].type=sys_label::tab_col_type_double;
		tab_col[28].default_value="0.0";

		tab_col[29].name=hyd_label::proferg_cv_r_db_in;
		tab_col[29].type=sys_label::tab_col_type_double;
		tab_col[29].default_value="0.0";

		tab_col[30].name=hyd_label::proferg_cv_r_db_out;
		tab_col[30].type=sys_label::tab_col_type_double;
		tab_col[30].default_value="0.0";

		tab_col[31].name=hyd_label::proferg_q_max;
		tab_col[31].type=sys_label::tab_col_type_double;
		tab_col[31].default_value="0.0";

		tab_col[32].name=hyd_label::proferg_polygon;
		tab_col[32].type=sys_label::tab_col_type_polygon;

		try{
			_Hyd_River_Profile::erg_table= new Tables();
			if(_Hyd_River_Profile::erg_table->create_non_existing_tables(tab_name, tab_col, num_col,ptr_database, _sys_table_type::hyd)==false){
				cout << " Table exists" << endl ;
				Sys_Common_Output::output_hyd->output_txt(&cout);
			};
		}
		catch(bad_alloc& t){
			Error msg;
			msg.set_msg("_Hyd_River_Profile::create_erg_table(QSqlDatabase *ptr_database)","Can not allocate the memory", "Check the memory", 10, false);
			ostringstream info;
			info<< "Info bad alloc: " << t.what() << endl;
			msg.make_second_info(info.str());
			throw msg;
		}
		catch(Error msg){
			_Hyd_River_Profile::close_erg_table();
			throw msg;
		}

	_Hyd_River_Profile::close_erg_table();

	//make indizes
	//the table is set (the name and the column names) and allocated
	try {
		_Hyd_River_Profile::set_erg_table(ptr_database);
	}
	catch (Error msg) {
		throw msg;
	}


	_Hyd_River_Profile::erg_table->create_spatial_index2column(ptr_database, _Hyd_River_Profile::erg_table->get_column_name(hyd_label::proferg_polygon));


	_Hyd_River_Profile::close_erg_table();
	}
}
//Close and delete the database table for the results of an hydraulic simulation the river profiles (static)
void _Hyd_River_Profile::close_erg_table(void) {
	if (_Hyd_River_Profile::erg_table != NULL) {
		delete _Hyd_River_Profile::erg_table;
		_Hyd_River_Profile::erg_table = NULL;
	}
}
//Set the database table for the results of an hydraulic simulation for the river profiles: it sets the table name and the name of the columns and allocate them (static)
void _Hyd_River_Profile::set_erg_table(QSqlDatabase *ptr_database, const bool not_close){
	if(_Hyd_River_Profile::erg_table==NULL){
		//make specific input for this class
		const string tab_id_name=hyd_label::tab_rvprof_erg_max;

		string tab_id_col[33];
		tab_id_col[0]=hyd_label::profdata_rvno;
		tab_id_col[1]=hyd_label::profdata_prof_id;
		tab_id_col[2]=label::areastate_id;
		tab_id_col[3]=label::measure_id;
		tab_id_col[4]=hyd_label::sz_bound_id;
		tab_id_col[5]=risk_label::sz_break_id;
		tab_id_col[6]=hyd_label::proferg_h_max;
		tab_id_col[7]=hyd_label::proferg_hmax_lb;
		tab_id_col[8]=hyd_label::proferg_hmax_rb;
		tab_id_col[9]=hyd_label::proferg_t_hmax;
		tab_id_col[10]=hyd_label::proferg_v_max;
		tab_id_col[11]=hyd_label::proferg_dur_wet;
		tab_id_col[12]=hyd_label::proferg_dur_dry;
		tab_id_col[13]=hyd_label::proferg_cv_rv_in;
		tab_id_col[14]=hyd_label::proferg_cv_rv_out;
		tab_id_col[15]=hyd_label::proferg_cv_struc_in;
		tab_id_col[16]=hyd_label::proferg_cv_struc_out;
		tab_id_col[17]=hyd_label::proferg_cv_l_ov_in;
		tab_id_col[18]=hyd_label::proferg_cv_l_ov_out;
		tab_id_col[19]=hyd_label::proferg_cv_r_ov_in;
		tab_id_col[20]=hyd_label::proferg_cv_r_ov_out;
		tab_id_col[21]=hyd_label::proferg_cv_l_db_in;
		tab_id_col[22]=hyd_label::proferg_cv_l_db_out;
		tab_id_col[23]=hyd_label::proferg_cv_r_db_in;
		tab_id_col[24]=hyd_label::proferg_cv_r_db_out;
		tab_id_col[25]=hyd_label::proferg_q_max;
		tab_id_col[26]=hyd_label::proferg_polygon;
		tab_id_col[27]=label::glob_id;
		tab_id_col[28]=label::applied_flag;
		tab_id_col[29]=hyd_label::proferg_hmax_lb_break;
		tab_id_col[30]=hyd_label::proferg_hmax_rb_break;
		tab_id_col[31]=hyd_label::proferg_s_max;
		tab_id_col[32] = hyd_label::proferg_width_max;

		try{
			_Hyd_River_Profile::erg_table= new Tables(tab_id_name, tab_id_col, sizeof(tab_id_col)/sizeof(tab_id_col[0]));
			_Hyd_River_Profile::erg_table->set_name(ptr_database, _sys_table_type::hyd);
		}
		catch(bad_alloc& t){
			Error msg;
			msg.set_msg("_Hyd_River_Profile::set_erg_table(QSqlDatabase *ptr_database)","Can not allocate the memory", "Check the memory", 10, false);
			ostringstream info;
			info<< "Info bad alloc: " << t.what() << endl;
			msg.make_second_info(info.str());
			throw msg;
		}
		catch(Error msg){
			if (not_close == false) {
				_Hyd_River_Profile::close_erg_table();
			}
			throw msg;
		}
	}
}
//Create the database table for the instationary results of an hydraulic simulation for the river profiles
void _Hyd_River_Profile::create_erg_instat_table(QSqlDatabase *ptr_database) {
	if (_Hyd_River_Profile::erg_instat_table == NULL) {
		ostringstream cout;
		cout << "Create river profile instationary result database table..." << endl;
		Sys_Common_Output::output_hyd->output_txt(&cout);
		//make specific input for this class
		const string tab_name = hyd_label::tab_rvprof_erg_instat;
		const int num_col = 14;
		_Sys_data_tab_column tab_col[num_col];
		//init
		for (int i = 0; i < num_col; i++) {
			tab_col[i].key_flag = false;
			tab_col[i].unsigned_flag = false;
			tab_col[i].primary_key_flag = false;
		}

		tab_col[0].name = label::glob_id;
		tab_col[0].type = sys_label::tab_col_type_int;
		tab_col[0].unsigned_flag = true;
		tab_col[0].primary_key_flag = true;

		tab_col[1].name = hyd_label::profdata_rvno;
		tab_col[1].type = sys_label::tab_col_type_int;
		tab_col[1].unsigned_flag = true;
		tab_col[1].key_flag = true;

		tab_col[2].name = label::areastate_id;
		tab_col[2].type = sys_label::tab_col_type_int;
		tab_col[2].unsigned_flag = true;
		tab_col[2].key_flag = true;

		tab_col[3].name = label::measure_id;
		tab_col[3].type = sys_label::tab_col_type_int;
		tab_col[3].unsigned_flag = true;
		tab_col[3].key_flag = true;

		tab_col[4].name = label::applied_flag;
		tab_col[4].type = sys_label::tab_col_type_bool;
		tab_col[4].default_value = "true";
		tab_col[4].key_flag = true;

		tab_col[5].name = hyd_label::sz_bound_id;
		tab_col[5].type = sys_label::tab_col_type_int;
		tab_col[5].unsigned_flag = true;
		tab_col[5].key_flag = true;

		tab_col[6].name = risk_label::sz_break_id;
		tab_col[6].type = sys_label::tab_col_type_string;
		tab_col[6].key_flag = true;

		tab_col[7].name = hyd_label::proferg_h_max;
		tab_col[7].type = sys_label::tab_col_type_double;
		tab_col[7].unsigned_flag = true;
		tab_col[7].default_value = "0.0";

		tab_col[8].name = hyd_label::proferg_s_max;
		tab_col[8].type = sys_label::tab_col_type_double;
		tab_col[8].unsigned_flag = true;
		tab_col[8].default_value = "0.0";


		tab_col[9].name = hyd_label::proferg_v_max;
		tab_col[9].type = sys_label::tab_col_type_double;
		tab_col[9].default_value = "0.0";

		tab_col[10].name = hyd_label::proferg_width_max;
		tab_col[10].type = sys_label::tab_col_type_double;
		tab_col[10].default_value = "0.0";


		tab_col[11].name = hyd_label::proferg_q_max;
		tab_col[11].type = sys_label::tab_col_type_double;
		tab_col[11].default_value = "0.0";

		tab_col[12].name = hyd_label::data_time;
		tab_col[12].type = sys_label::tab_col_type_string;
		tab_col[12].default_value = "";

		tab_col[13].name = hyd_label::proferg_polygon;
		tab_col[13].type = sys_label::tab_col_type_polygon;

		try {
			_Hyd_River_Profile::erg_instat_table = new Tables();
			if (_Hyd_River_Profile::erg_instat_table->create_non_existing_tables(tab_name, tab_col, num_col, ptr_database, _sys_table_type::hyd) == false) {
				cout << " Table exists" << endl;
				Sys_Common_Output::output_hyd->output_txt(&cout);
			};
		}
		catch (bad_alloc& t) {
			Error msg;
			msg.set_msg("_Hyd_River_Profile::create_erg_instat_table(QSqlDatabase *ptr_database)", "Can not allocate the memory", "Check the memory", 10, false);
			ostringstream info;
			info << "Info bad alloc: " << t.what() << endl;
			msg.make_second_info(info.str());
			throw msg;
		}
		catch (Error msg) {
			_Hyd_River_Profile::close_erg_instat_table();
			throw msg;
		}

		_Hyd_River_Profile::close_erg_instat_table();

		//make indizes
		//the table is set (the name and the column names) and allocated
		try {
			_Hyd_River_Profile::set_erg_instat_table(ptr_database);
		}
		catch (Error msg) {
			throw msg;
		}


		_Hyd_River_Profile::erg_instat_table->create_index2column(ptr_database, _Hyd_River_Profile::erg_instat_table->get_column_name(hyd_label::data_time));
		_Hyd_River_Profile::erg_instat_table->create_spatial_index2column(ptr_database, _Hyd_River_Profile::erg_instat_table->get_column_name(hyd_label::elemdata_polygon));


		_Hyd_River_Profile::close_erg_instat_table();
	}

}
//Set the database table for the instationary results of an hydraulic simulation for the river profiles: it sets the table name and the name of the columns and allocate them
void _Hyd_River_Profile::set_erg_instat_table(QSqlDatabase *ptr_database, const bool not_close) {
	if (_Hyd_River_Profile::erg_instat_table == NULL) {
		//make specific input for this class
		const string tab_id_name = hyd_label::tab_rvprof_erg_instat;

		string tab_id_col[14];
		tab_id_col[0] = label::glob_id;
		tab_id_col[1] = hyd_label::profdata_rvno;
		tab_id_col[2]= label::areastate_id;
		tab_id_col[3] = label::measure_id;
		tab_id_col[4] = label::applied_flag;
		tab_id_col[5] = hyd_label::sz_bound_id;
		tab_id_col[6] = risk_label::sz_break_id;
		tab_id_col[7] = hyd_label::proferg_h_max;
		tab_id_col[8] = hyd_label::proferg_s_max;
		tab_id_col[9] = hyd_label::proferg_v_max;
		tab_id_col[10] = hyd_label::proferg_q_max;
		tab_id_col[11] = hyd_label::proferg_polygon;
		tab_id_col[12] = hyd_label::data_time;
		tab_id_col[13] = hyd_label::proferg_width_max;



		try {
			_Hyd_River_Profile::erg_instat_table = new Tables(tab_id_name, tab_id_col, sizeof(tab_id_col) / sizeof(tab_id_col[0]));
			_Hyd_River_Profile::erg_instat_table->set_name(ptr_database, _sys_table_type::hyd);
		}
		catch (bad_alloc& t) {
			Error msg;
			msg.set_msg("_Hyd_River_Profile::set_erg_instat_table(QSqlDatabase *ptr_database)", "Can not allocate the memory", "Check the memory", 10, false);
			ostringstream info;
			info << "Info bad alloc: " << t.what() << endl;
			msg.make_second_info(info.str());
			throw msg;
		}
		catch (Error msg) {
			if (not_close == false) {
				_Hyd_River_Profile::close_erg_instat_table();
			}
			throw msg;
		}
	}


}
//Close and delete the database table for the instationary results of an hydraulic simulation the river profiles
void _Hyd_River_Profile::close_erg_instat_table(void) {
	if (_Hyd_River_Profile::erg_instat_table != NULL) {
		delete _Hyd_River_Profile::erg_instat_table;
		_Hyd_River_Profile::erg_instat_table = NULL;
	}

}
//Delete the results data in the result database table for a given boundary scenario (static)
void _Hyd_River_Profile::delete_results_by_scenario(QSqlDatabase *ptr_database, const int sc_id){
	QSqlQueryModel results;

	try{
		_Hyd_River_Profile::set_erg_table(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}
	ostringstream test_filter;

	test_filter<< "DELETE ";
	test_filter << " FROM " << _Hyd_River_Profile::erg_table->get_table_name();
	test_filter << " WHERE ";
	test_filter << _Hyd_River_Profile::erg_table->get_column_name(hyd_label::sz_bound_id) << " = " << sc_id;

	Data_Base::database_request(&results, test_filter.str(), ptr_database);

	//check the request
	if(results.lastError().isValid()){
		Error msg;
		msg.set_msg("_Hyd_River_Profile::delete_results_by_sceneario(QSqlDatabase *ptr_database, const int sc_id)","Invalid database request", "Check the database", 2, false);
		ostringstream info;
		info << "Table Name      : " << _Hyd_River_Profile::erg_table->get_table_name() << endl;
		info << "Table error info: " << results.lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
}
//Delete the results data in the maximum result database table for a given system state (static)
void _Hyd_River_Profile::delete_results_by_system_state(QSqlDatabase *ptr_database, const _sys_system_id id){
	QSqlQueryModel results;

	try{
		_Hyd_River_Profile::set_erg_table(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}
	ostringstream test_filter;

	test_filter<< "DELETE ";
	test_filter << " FROM " << _Hyd_River_Profile::erg_table->get_table_name();
	test_filter << " WHERE ";
	test_filter << _Hyd_River_Profile::erg_table->get_column_name(label::areastate_id) << " ="  << id.area_state;
	test_filter << " AND ";
	test_filter << _Hyd_River_Profile::erg_table->get_column_name(label::measure_id) << " = " << id.measure_nr;

	Data_Base::database_request(&results, test_filter.str(), ptr_database);

	//check the request
	if(results.lastError().isValid()){
		Error msg;
		msg.set_msg("_Hyd_River_Profile::delete_results_by_sceneario(QSqlDatabase *ptr_database, const _sys_system_id id)","Invalid database request", "Check the database", 2, false);
		ostringstream info;
		info << "Table Name      : " << _Hyd_River_Profile::erg_table->get_table_name() << endl;
		info << "Table error info: " << results.lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
}
//Delete the results data in the maximum result database table for specific parameters (static)
void _Hyd_River_Profile::delete_results_in_table(QSqlDatabase *ptr_database, const _sys_system_id id, const string break_sz, const bool like_flag){
		QSqlQueryModel results;

	try{
		_Hyd_River_Profile::set_erg_table(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}
	ostringstream test_filter;

	test_filter<< "DELETE ";
	test_filter << " FROM " << _Hyd_River_Profile::erg_table->get_table_name();
	test_filter << " WHERE ";
	test_filter << _Hyd_River_Profile::erg_table->get_column_name(label::areastate_id) << " ="  << id.area_state;
	test_filter << " AND ";
	test_filter << _Hyd_River_Profile::erg_table->get_column_name(label::measure_id) << " = " << id.measure_nr;
	test_filter << " AND ";
	if(like_flag==false){
		test_filter << _Hyd_River_Profile::erg_table->get_column_name(risk_label::sz_break_id) << " = '" << break_sz<<"'";
	}
	else{
		test_filter << _Hyd_River_Profile::erg_table->get_column_name(risk_label::sz_break_id) << " LIKE '" << break_sz<<"'";
	}

	Data_Base::database_request(&results, test_filter.str(), ptr_database);

	//check the request
	if(results.lastError().isValid()){
		Error msg;
		msg.set_msg("_Hyd_River_Profile::delete_results_in_table(QSqlDatabase *ptr_database, const _sys_system_id id, const string break_sz, const bool like_flag=false)","Invalid database request", "Check the database", 2, false);
		ostringstream info;
		info << "Table Name      : " << _Hyd_River_Profile::erg_table->get_table_name() << endl;
		info << "Table error info: " << results.lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
}
//Delete the results data in the maximum result database table for specific parameters (static)
void _Hyd_River_Profile::delete_results_in_table(QSqlDatabase *ptr_database, const _sys_system_id id, const int bound_sz, const string break_sz, const bool like_flag){
			QSqlQueryModel results;

	try{
		_Hyd_River_Profile::set_erg_table(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}
	ostringstream test_filter;

	test_filter<< "DELETE ";
	test_filter << " FROM " << _Hyd_River_Profile::erg_table->get_table_name();
	test_filter << " WHERE ";
	test_filter << _Hyd_River_Profile::erg_table->get_column_name(label::areastate_id) << " ="  << id.area_state;
	test_filter << " AND ";
	test_filter << _Hyd_River_Profile::erg_table->get_column_name(label::measure_id) << " = " << id.measure_nr;
	test_filter << " AND ";
	test_filter << _Hyd_River_Profile::erg_table->get_column_name(hyd_label::sz_bound_id) << " = " << bound_sz;
	test_filter << " AND ";
	if(like_flag==false){
		test_filter << _Hyd_River_Profile::erg_table->get_column_name(risk_label::sz_break_id) << " = '" << break_sz<<"'";
	}
	else{
		test_filter << _Hyd_River_Profile::erg_table->get_column_name(risk_label::sz_break_id) << " LIKE '" << break_sz<<"'";
	}

	Data_Base::database_request(&results, test_filter.str(), ptr_database);

	//check the request
	if(results.lastError().isValid()){
		Error msg;
		msg.set_msg("_Hyd_River_Profile::delete_results_in_table(QSqlDatabase *ptr_database, const _sys_system_id id, const int bound_sz, const string break_sz, const bool like_flag=false)","Invalid database request", "Check the database", 2, false);
		ostringstream info;
		info << "Table Name      : " << _Hyd_River_Profile::erg_table->get_table_name() << endl;
		info << "Table error info: " << results.lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
}
//Delete the instationary results data in the maximum result database table for a given boundary scenario
void _Hyd_River_Profile::delete_instat_results_by_scenario(QSqlDatabase *ptr_database, const int sc_id) {
	QSqlQueryModel results;

	try {
		_Hyd_River_Profile::set_erg_instat_table(ptr_database);
	}
	catch (Error msg) {
		throw msg;
	}
	ostringstream test_filter;

	test_filter << "DELETE ";
	test_filter << " FROM " << _Hyd_River_Profile::erg_instat_table->get_table_name();
	test_filter << " WHERE ";
	test_filter << _Hyd_River_Profile::erg_instat_table->get_column_name(hyd_label::sz_bound_id) << " = " << sc_id;

	Data_Base::database_request(&results, test_filter.str(), ptr_database);

	//check the request
	if (results.lastError().isValid()) {
		Error msg;
		msg.set_msg("_Hyd_River_Profile::delete_instat_results_by_sceneario(QSqlDatabase *ptr_database, const int sc_id)", "Invalid database request", "Check the database", 2, false);
		ostringstream info;
		info << "Table Name      : " << _Hyd_River_Profile::erg_instat_table->get_table_name() << endl;
		info << "Table error info: " << results.lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}

}
//Delete the instationary results data in the maximum result database table for a given system state
void _Hyd_River_Profile::delete_instat_results_by_system_state(QSqlDatabase *ptr_database, const _sys_system_id id) {
	QSqlQueryModel results;

	try {
		_Hyd_River_Profile::set_erg_instat_table(ptr_database);
	}
	catch (Error msg) {
		throw msg;
	}
	ostringstream test_filter;

	test_filter << "DELETE ";
	test_filter << " FROM " << _Hyd_River_Profile::erg_instat_table->get_table_name();
	test_filter << " WHERE ";
	test_filter << _Hyd_River_Profile::erg_instat_table->get_column_name(label::areastate_id) << " =" << id.area_state;
	test_filter << " AND ";
	test_filter << _Hyd_River_Profile::erg_instat_table->get_column_name(label::measure_id) << " = " << id.measure_nr;

	Data_Base::database_request(&results, test_filter.str(), ptr_database);

	//check the request
	if (results.lastError().isValid()) {
		Error msg;
		msg.set_msg("_Hyd_River_Profile::delete_instat_results_by_sceneario(QSqlDatabase *ptr_database, const _sys_system_id id)", "Invalid database request", "Check the database", 2, false);
		ostringstream info;
		info << "Table Name      : " << _Hyd_River_Profile::erg_instat_table->get_table_name() << endl;
		info << "Table error info: " << results.lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}

}
//Delete the instationary results data in the maximum result database table for specific parameters
void _Hyd_River_Profile::delete_instat_results_in_table(QSqlDatabase *ptr_database, const _sys_system_id id, const string break_sz, const bool like_flag) {
	QSqlQueryModel results;

	try {
		_Hyd_River_Profile::set_erg_instat_table(ptr_database);
	}
	catch (Error msg) {
		throw msg;
	}
	ostringstream test_filter;

	test_filter << "DELETE ";
	test_filter << " FROM " << _Hyd_River_Profile::erg_instat_table->get_table_name();
	test_filter << " WHERE ";
	test_filter << _Hyd_River_Profile::erg_instat_table->get_column_name(label::areastate_id) << " =" << id.area_state;
	test_filter << " AND ";
	test_filter << _Hyd_River_Profile::erg_instat_table->get_column_name(label::measure_id) << " = " << id.measure_nr;
	test_filter << " AND ";
	if (like_flag == false) {
		test_filter << _Hyd_River_Profile::erg_instat_table->get_column_name(risk_label::sz_break_id) << " = '" << break_sz << "'";
	}
	else {
		test_filter << _Hyd_River_Profile::erg_instat_table->get_column_name(risk_label::sz_break_id) << " LIKE '" << break_sz << "'";
	}

	Data_Base::database_request(&results, test_filter.str(), ptr_database);

	//check the request
	if (results.lastError().isValid()) {
		Error msg;
		msg.set_msg("_Hyd_River_Profile::delete_instat_results_in_table(QSqlDatabase *ptr_database, const _sys_system_id id, const string break_sz, const bool like_flag=false)", "Invalid database request", "Check the database", 2, false);
		ostringstream info;
		info << "Table Name      : " << _Hyd_River_Profile::erg_instat_table->get_table_name() << endl;
		info << "Table error info: " << results.lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}

}
//Delete the instationaryresults data in the maximum result database table for specific parameters
void _Hyd_River_Profile::delete_instat_results_in_table(QSqlDatabase *ptr_database, const _sys_system_id id, const int bound_sz, const string break_sz, const bool like_flag) {
	QSqlQueryModel results;

	try {
		_Hyd_River_Profile::set_erg_instat_table(ptr_database);
	}
	catch (Error msg) {
		throw msg;
	}
	ostringstream test_filter;

	test_filter << "DELETE ";
	test_filter << " FROM " << _Hyd_River_Profile::erg_instat_table->get_table_name();
	test_filter << " WHERE ";
	test_filter << _Hyd_River_Profile::erg_instat_table->get_column_name(label::areastate_id) << " =" << id.area_state;
	test_filter << " AND ";
	test_filter << _Hyd_River_Profile::erg_instat_table->get_column_name(label::measure_id) << " = " << id.measure_nr;
	test_filter << " AND ";
	test_filter << _Hyd_River_Profile::erg_instat_table->get_column_name(hyd_label::sz_bound_id) << " = " << bound_sz;
	test_filter << " AND ";
	if (like_flag == false) {
		test_filter << _Hyd_River_Profile::erg_instat_table->get_column_name(risk_label::sz_break_id) << " = '" << break_sz << "'";
	}
	else {
		test_filter << _Hyd_River_Profile::erg_instat_table->get_column_name(risk_label::sz_break_id) << " LIKE '" << break_sz << "'";
	}

	Data_Base::database_request(&results, test_filter.str(), ptr_database);

	//check the request
	if (results.lastError().isValid()) {
		Error msg;
		msg.set_msg("_Hyd_River_Profile::delete_instat_results_in_table(QSqlDatabase *ptr_database, const _sys_system_id id, const int bound_sz, const string break_sz, const bool like_flag=false)", "Invalid database request", "Check the database", 2, false);
		ostringstream info;
		info << "Table Name      : " << _Hyd_River_Profile::erg_instat_table->get_table_name() << endl;
		info << "Table error info: " << results.lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}

}
//Delete the profiles and appending data by a system id (static);
void _Hyd_River_Profile::delete_profile_in_table(QSqlDatabase *ptr_database, const _sys_system_id id){
	int number=0;
	QSqlQueryModel model;
	number=_Hyd_River_Profile::select_profiles_in_database(&model, ptr_database, id);

	int glob_id=0;
	for(int i=0; i<number; i++){
		glob_id=model.record(i).value((_Hyd_River_Profile::profile_table->get_column_name(hyd_label::profdata_glob_id)).c_str()).toInt();

		//delete the appended tables
		_Hyd_River_Profile::delete_boundary_cond_by_rv_profile(ptr_database, glob_id);
		_Hyd_River_Profile::delete_wall_break_by_rv_profile(ptr_database, glob_id);
		_Hyd_River_Profile::delete_dike_break_by_rv_profile(ptr_database, glob_id);
		_Hyd_River_Profile::delete_bridge_by_rv_profile(ptr_database, glob_id);
		_Hyd_River_Profile_Type::delete_points_by_rv_profile(ptr_database, glob_id);

	}


	//delete the profile data
	QSqlQueryModel results;

	
	ostringstream test_filter;

	test_filter<< "Delete ";
	test_filter << " from " << _Hyd_River_Profile::profile_table->get_table_name();
	test_filter  << " WHERE ";
	test_filter << _Hyd_River_Profile::profile_table->get_column_name(label::areastate_id) << " = "  << id.area_state;
	test_filter  << " AND ";
	test_filter << _Hyd_River_Profile::profile_table->get_column_name(label::measure_id) << " = "  << id.measure_nr;

	Data_Base::database_request(&results, test_filter.str(), ptr_database);

	//check the request
	if(results.lastError().isValid()){
		Error msg;
		msg.set_msg("delete_profile_in_table(QSqlDatabase *ptr_database, const _sys_system_id id)","Invalid database request", "Check the database", 2, false);
		ostringstream info;
		info << "Table Name      : " << _Hyd_River_Profile::profile_table->get_table_name() << endl;
		info << "Table error info: " << results.lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}

}
//Delete the boundary condition data in the database table for a given boundary scenario (static)
void _Hyd_River_Profile::delete_boundary_cond_by_scenario(QSqlDatabase *ptr_database, const int sc_id){
	QSqlQueryModel results;

	try{
		_Hyd_River_Profile::set_profile_boundary_table(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}
	ostringstream test_filter;

	test_filter<< "Delete ";
	test_filter << " from " << _Hyd_River_Profile::boundary_table->get_table_name();
	test_filter << " where ";
	test_filter << _Hyd_River_Profile::boundary_table->get_column_name(hyd_label::sz_bound_id) << " = " << sc_id;

	Data_Base::database_request(&results, test_filter.str(), ptr_database);

	//check the request
	if(results.lastError().isValid()){
		Error msg;
		msg.set_msg("_Hyd_River_Profile::delete_boundary_cond_by_scenario(QSqlDatabase *ptr_database, const int sc_id)","Invalid database request", "Check the database", 2, false);
		ostringstream info;
		info << "Table Name      : " << _Hyd_River_Profile::boundary_table->get_table_name() << endl;
		info << "Table error info: " << results.lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
}
//Delete the boundary condition data in the database table for a given system state (static)
void _Hyd_River_Profile::delete_boundary_cond_by_system_state(QSqlDatabase *ptr_database, const _sys_system_id id){
	QSqlQueryModel results;

	try{
		_Hyd_River_Profile::set_profile_boundary_table(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}
	ostringstream test_filter;

	test_filter<< "Delete ";
	test_filter << " from " << _Hyd_River_Profile::boundary_table->get_table_name();
	test_filter  << " WHERE ";
	test_filter << _Hyd_River_Profile::boundary_table->get_column_name(label::areastate_id) << " = "  << id.area_state;
	test_filter  << " AND ";
	test_filter  <<  _Hyd_River_Profile::boundary_table->get_column_name(label::measure_id) << " = " << id.measure_nr ;

	Data_Base::database_request(&results, test_filter.str(), ptr_database);

	//check the request
	if(results.lastError().isValid()){
		Error msg;
		msg.set_msg("_Hyd_River_Profile::delete_boundary_cond_by_scenario(QSqlDatabase *ptr_database, const _sys_system_id id)","Invalid database request", "Check the database", 2, false);
		ostringstream info;
		info << "Table Name      : " << _Hyd_River_Profile::boundary_table->get_table_name() << endl;
		info << "Table error info: " << results.lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
}
//Delete the boundary condition data in the database table for a given global profile id (static)
void _Hyd_River_Profile::delete_boundary_cond_by_rv_profile(QSqlDatabase *ptr_database, const int id){
		QSqlQueryModel results;

	try{
		_Hyd_River_Profile::set_profile_boundary_table(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}
	ostringstream test_filter;

	test_filter<< "Delete ";
	test_filter << " from " << _Hyd_River_Profile::boundary_table->get_table_name();
	test_filter  << " WHERE ";
	test_filter << _Hyd_River_Profile::boundary_table->get_column_name(hyd_label::profdata_glob_id) << " = "  << id;

	Data_Base::database_request(&results, test_filter.str(), ptr_database);

	//check the request
	if(results.lastError().isValid()){
		Error msg;
		msg.set_msg("_Hyd_River_Profile::delete_boundary_cond_by_rv_profile(QSqlDatabase *ptr_database, const int id)","Invalid database request", "Check the database", 2, false);
		ostringstream info;
		info << "Table Name      : " << _Hyd_River_Profile::boundary_table->get_table_name() << endl;
		info << "Table error info: " << results.lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
}
//Delete the wallbreak data in the database table for a given global profile id (static)
void _Hyd_River_Profile::delete_wall_break_by_rv_profile(QSqlDatabase *ptr_database, const int id){
	QSqlQueryModel results;

	try{
		_Hyd_River_Profile::set_profile_wallbreak_table(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}
	ostringstream test_filter;

	test_filter<< "Delete ";
	test_filter << " from " << _Hyd_River_Profile::wallbreak_table->get_table_name();
	test_filter  << " WHERE ";
	test_filter << _Hyd_River_Profile::wallbreak_table->get_column_name(hyd_label::profdata_glob_id) << " = "  << id;

	Data_Base::database_request(&results, test_filter.str(), ptr_database);

	//check the request
	if(results.lastError().isValid()){
		Error msg;
		msg.set_msg("_Hyd_River_Profile::delete_wall_break_by_rv_profile(QSqlDatabase *ptr_database, const int id)","Invalid database request", "Check the database", 2, false);
		ostringstream info;
		info << "Table Name      : " << _Hyd_River_Profile::wallbreak_table->get_table_name() << endl;
		info << "Table error info: " << results.lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
}
//Delete the dikebreak data in the database table for a given global profile id (static)
void _Hyd_River_Profile::delete_dike_break_by_rv_profile(QSqlDatabase *ptr_database, const int id){
	QSqlQueryModel results;

	try{
		_Hyd_River_Profile::set_profile_dikebreak_table(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}
	ostringstream test_filter;

	test_filter<< "Delete ";
	test_filter << " from " << _Hyd_River_Profile::dikebreak_table->get_table_name();
	test_filter  << " WHERE ";
	test_filter << _Hyd_River_Profile::dikebreak_table->get_column_name(hyd_label::profdata_glob_id) << " = "  << id;

	Data_Base::database_request(&results, test_filter.str(), ptr_database);

	//check the request
	if(results.lastError().isValid()){
		Error msg;
		msg.set_msg("_Hyd_River_Profile::delete_dike_break_by_rv_profile(QSqlDatabase *ptr_database, const int id)","Invalid database request", "Check the database", 2, false);
		ostringstream info;
		info << "Table Name      : " << _Hyd_River_Profile::dikebreak_table->get_table_name() << endl;
		info << "Table error info: " << results.lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
}
//Delete the bridge data in the database table for a given global profile id (static)
void _Hyd_River_Profile::delete_bridge_by_rv_profile(QSqlDatabase *ptr_database, const int id){
		QSqlQueryModel results;

	try{
		_Hyd_River_Profile::set_profile_bridge_table(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}
	ostringstream test_filter;

	test_filter<< "Delete ";
	test_filter << " from " << _Hyd_River_Profile::bridge_table->get_table_name();
	test_filter  << " WHERE ";
	test_filter << _Hyd_River_Profile::bridge_table->get_column_name(hyd_label::profdata_glob_id) << " = "  << id;

	Data_Base::database_request(&results, test_filter.str(), ptr_database);

	//check the request
	if(results.lastError().isValid()){
		Error msg;
		msg.set_msg("_Hyd_River_Profile::delete_bridge_by_rv_profile(QSqlDatabase *ptr_database, const int id)","Invalid database request", "Check the database", 2, false);
		ostringstream info;
		info << "Table Name      : " << _Hyd_River_Profile::bridge_table->get_table_name() << endl;
		info << "Table error info: " << results.lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}

}
//Select the data in the database table for the maximum results of the river profiles specified by the system id and the scenario-ids (static)
int _Hyd_River_Profile::select_data_in_erg_table(QSqlQueryModel *query, QSqlDatabase *ptr_database, const _sys_system_id id, const int bound_sz, const string break_sz , const bool like_flag){
	//the table is set (the name and the column names) and allocated
	try{
		_Hyd_River_Profile::set_erg_table(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}

	ostringstream query_string;
	query_string <<"SELECT ";
	query_string <<  _Hyd_River_Profile::erg_table->get_column_name(hyd_label::profdata_rvno)  <<" , ";
	query_string <<  _Hyd_River_Profile::erg_table->get_column_name(hyd_label::profdata_prof_id)  <<" , ";
	query_string <<  _Hyd_River_Profile::erg_table->get_column_name(hyd_label::proferg_h_max)  <<" , ";
	query_string <<  _Hyd_River_Profile::erg_table->get_column_name(hyd_label::proferg_hmax_lb)  <<" , ";
	query_string <<  _Hyd_River_Profile::erg_table->get_column_name(hyd_label::proferg_hmax_rb)  <<" , ";
	query_string <<  _Hyd_River_Profile::erg_table->get_column_name(hyd_label::proferg_q_max)  <<" , ";
	query_string <<  _Hyd_River_Profile::erg_table->get_column_name(risk_label::sz_break_id)  <<" , ";
	query_string <<  _Hyd_River_Profile::erg_table->get_column_name(label::glob_id);

	query_string << " FROM " << _Hyd_River_Profile::erg_table->get_table_name();
	query_string << " WHERE " ;
	query_string  << _Hyd_River_Profile::erg_table->get_column_name(label::applied_flag) << "= true";
	query_string  << " AND ";
	query_string  << _Hyd_River_Profile::erg_table->get_column_name(label::areastate_id) << " ="  << id.area_state;
	query_string  << " AND (";
	query_string  <<  _Hyd_River_Profile::erg_table->get_column_name(label::measure_id) << " = " << 0 ;
	query_string  << " OR " ;
	query_string  << _Hyd_River_Profile::erg_table->get_column_name(label::measure_id) << " = " << id.measure_nr;
	query_string  << " ) " ;
	query_string << " AND ";
	query_string << _Hyd_River_Profile::erg_table->get_column_name(hyd_label::sz_bound_id) << " = " << bound_sz;
	query_string << " AND ";
	if(like_flag==false){
		query_string << _Hyd_River_Profile::erg_table->get_column_name(risk_label::sz_break_id) << " = '" << break_sz<<"'";
	}
	else{
		query_string << _Hyd_River_Profile::erg_table->get_column_name(risk_label::sz_break_id) << " LIKE '" << break_sz<<"'";
	}

	Data_Base::database_request(query, query_string.str(), ptr_database);
	if(query->lastError().isValid()==true){
		Error msg;
		msg.set_msg("_Hyd_River_Profile::select_data_in_erg_table(QSqlQueryModel *query, QSqlDatabase *ptr_database, const _sys_system_id id, const int bound_sz, const string break_sz )","Invalid database request", "Check the database", 2, false);
		ostringstream info;
		info << "Table Name      : " << _Hyd_River_Profile::erg_table->get_table_name() << endl;
		info << "Table error info: " << query->lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
	return query->rowCount();
}
//Select the data in the database table for the maximum results of the river profiles specified by the system state, the scenario-ids, the river number and a specific profile (static)
int _Hyd_River_Profile::select_data_in_erg_table(QSqlQueryModel *query, QSqlDatabase *ptr_database, const _sys_system_id id, const int bound_sz, const string break_sz, const int rv_no, const int prof_no){
		//the table is set (the name and the column names) and allocated
	try{
		_Hyd_River_Profile::set_erg_table(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}

	ostringstream query_string;
	query_string <<"SELECT ";
	query_string <<  _Hyd_River_Profile::erg_table->get_column_name(hyd_label::profdata_rvno)  <<" , ";
	query_string <<  _Hyd_River_Profile::erg_table->get_column_name(hyd_label::profdata_prof_id)  <<" , ";
	query_string <<  _Hyd_River_Profile::erg_table->get_column_name(hyd_label::proferg_h_max)  <<" , ";
	query_string <<  _Hyd_River_Profile::erg_table->get_column_name(hyd_label::proferg_hmax_lb)  <<" , ";
	query_string <<  _Hyd_River_Profile::erg_table->get_column_name(hyd_label::proferg_hmax_rb)  <<" , ";
	query_string <<  _Hyd_River_Profile::erg_table->get_column_name(hyd_label::proferg_hmax_lb_break)  <<" , ";
	query_string <<  _Hyd_River_Profile::erg_table->get_column_name(hyd_label::proferg_hmax_rb_break)  <<" , ";
	query_string <<  _Hyd_River_Profile::erg_table->get_column_name(hyd_label::proferg_q_max)  <<" , ";
	query_string <<  _Hyd_River_Profile::erg_table->get_column_name(risk_label::sz_break_id)  <<" , ";
	query_string <<  _Hyd_River_Profile::erg_table->get_column_name(label::glob_id);

	query_string << " FROM " << _Hyd_River_Profile::erg_table->get_table_name();
	query_string << " WHERE " ;
	query_string  << _Hyd_River_Profile::erg_table->get_column_name(label::applied_flag) << "= true";
	query_string  << " AND ";
	query_string  << _Hyd_River_Profile::erg_table->get_column_name(label::areastate_id) << " ="  << id.area_state;
	query_string  << " AND (";
	query_string  <<  _Hyd_River_Profile::erg_table->get_column_name(label::measure_id) << " = " << 0 ;
	query_string  << " OR " ;
	query_string  << _Hyd_River_Profile::erg_table->get_column_name(label::measure_id) << " = " << id.measure_nr;
	query_string  << " ) " ;
	query_string << " AND ";
	query_string << _Hyd_River_Profile::erg_table->get_column_name(hyd_label::sz_bound_id) << " = " << bound_sz;
	query_string << " AND ";
	query_string << _Hyd_River_Profile::erg_table->get_column_name(risk_label::sz_break_id) << " = '" << break_sz<<"'";
	query_string << " AND ";
	query_string << _Hyd_River_Profile::erg_table->get_column_name(hyd_label::profdata_rvno) << " = " << rv_no;
	query_string << " AND ";
	query_string << _Hyd_River_Profile::erg_table->get_column_name(hyd_label::profdata_prof_id) << " = " << prof_no;

	Data_Base::database_request(query, query_string.str(), ptr_database);
	if(query->lastError().isValid()==true){
		Error msg;
		msg.set_msg("_Hyd_River_Profile::select_data_in_erg_table(QSqlQueryModel *query, QSqlDatabase *ptr_database, const _sys_system_id id, const int bound_sz, const string break_sz, const int rv_no, const int prof_no)","Invalid database request", "Check the database", 2, false);
		ostringstream info;
		info << "Table Name      : " << _Hyd_River_Profile::erg_table->get_table_name() << endl;
		info << "Table error info: " << query->lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}

	return query->rowCount();
}
//Select the data in the database table for the instationary results of the river profiles specified by the system state and the scenario-ids (static)
int _Hyd_River_Profile::select_data_in_instat_erg_table(QSqlQueryModel *query, QSqlDatabase *ptr_database, const _sys_system_id id, const int bound_sz, const string break_sz, const bool like_flag) {


	//the table is set (the name and the column names) and allocated
	try {
		_Hyd_River_Profile::set_erg_instat_table(ptr_database);
	}
	catch (Error msg) {
		throw msg;
	}

	ostringstream query_string;
	query_string << "SELECT ";
	query_string << _Hyd_River_Profile::erg_instat_table->get_column_name(hyd_label::profdata_rvno) << " , ";
	query_string << _Hyd_River_Profile::erg_instat_table->get_column_name(hyd_label::proferg_h_max) << " , ";
	query_string << _Hyd_River_Profile::erg_instat_table->get_column_name(hyd_label::proferg_s_max) << " , ";
	query_string << _Hyd_River_Profile::erg_instat_table->get_column_name(hyd_label::proferg_v_max) << " , ";
	query_string << _Hyd_River_Profile::erg_instat_table->get_column_name(hyd_label::proferg_q_max) << " , ";
	query_string << _Hyd_River_Profile::erg_instat_table->get_column_name(risk_label::sz_break_id) << " , ";
	query_string << _Hyd_River_Profile::erg_instat_table->get_column_name(hyd_label::data_time) << " , ";
	query_string << _Hyd_River_Profile::erg_instat_table->get_column_name(label::glob_id);


	query_string << " FROM " << _Hyd_River_Profile::erg_instat_table->get_table_name();
	query_string << " WHERE ";
	query_string << _Hyd_River_Profile::erg_instat_table->get_column_name(label::applied_flag) << "= true";
	query_string << " AND ";
	query_string << _Hyd_River_Profile::erg_instat_table->get_column_name(label::areastate_id) << " =" << id.area_state;
	query_string << " AND (";
	query_string << _Hyd_River_Profile::erg_instat_table->get_column_name(label::measure_id) << " = " << 0;
	query_string << " OR ";
	query_string << _Hyd_River_Profile::erg_instat_table->get_column_name(label::measure_id) << " = " << id.measure_nr;
	query_string << " ) ";
	query_string << " AND ";
	query_string << _Hyd_River_Profile::erg_instat_table->get_column_name(hyd_label::sz_bound_id) << " = " << bound_sz;
	query_string << " AND ";
	if (like_flag == false) {
		query_string << _Hyd_River_Profile::erg_instat_table->get_column_name(risk_label::sz_break_id) << " = '" << break_sz << "'";
	}
	else {
		query_string << _Hyd_River_Profile::erg_instat_table->get_column_name(risk_label::sz_break_id) << " LIKE '" << break_sz << "'";
	}

	Data_Base::database_request(query, query_string.str(), ptr_database);
	if (query->lastError().isValid() == true) {
		Error msg;
		msg.set_msg("_Hyd_River_Profile::select_data_in_instat_erg_table(QSqlQueryModel *query, QSqlDatabase *ptr_database, const _sys_system_id id, const int bound_sz, const string break_sz )", "Invalid database request", "Check the database", 2, false);
		ostringstream info;
		info << "Table Name      : " << _Hyd_River_Profile::erg_instat_table->get_table_name() << endl;
		info << "Table error info: " << query->lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
	return query->rowCount();


}
//Select the data in the database table for the instationary results of the river profiles specified by the system state and the scenario-ids (static)
int _Hyd_River_Profile::select_data_in_instat_erg_table(QSqlQueryModel *query, QSqlDatabase *ptr_database, const _sys_system_id id, const int bound_sz, const string break_sz, const int rv_no, const bool like_flag) {


	//the table is set (the name and the column names) and allocated
	try {
		_Hyd_River_Profile::set_erg_instat_table(ptr_database);
	}
	catch (Error msg) {
		throw msg;
	}

	ostringstream query_string;
	query_string << "SELECT ";
	query_string << _Hyd_River_Profile::erg_instat_table->get_column_name(hyd_label::profdata_rvno) << " , ";
	query_string << _Hyd_River_Profile::erg_instat_table->get_column_name(hyd_label::proferg_h_max) << " , ";
	query_string << _Hyd_River_Profile::erg_instat_table->get_column_name(hyd_label::proferg_s_max) << " , ";
	query_string << _Hyd_River_Profile::erg_instat_table->get_column_name(hyd_label::proferg_v_max) << " , ";
	query_string << _Hyd_River_Profile::erg_instat_table->get_column_name(hyd_label::proferg_q_max) << " , ";
	query_string << _Hyd_River_Profile::erg_instat_table->get_column_name(risk_label::sz_break_id) << " , ";
	query_string << _Hyd_River_Profile::erg_instat_table->get_column_name(hyd_label::data_time) << " , ";
	query_string << _Hyd_River_Profile::erg_instat_table->get_column_name(label::glob_id);


	query_string << " FROM " << _Hyd_River_Profile::erg_instat_table->get_table_name();
	query_string << " WHERE ";
	query_string << _Hyd_River_Profile::erg_instat_table->get_column_name(label::applied_flag) << "= true";
	query_string << " AND ";
	query_string << _Hyd_River_Profile::erg_instat_table->get_column_name(hyd_label::profdata_rvno) << "="<<rv_no;
	query_string << " AND ";
	query_string << _Hyd_River_Profile::erg_instat_table->get_column_name(label::areastate_id) << " =" << id.area_state;
	query_string << " AND (";
	query_string << _Hyd_River_Profile::erg_instat_table->get_column_name(label::measure_id) << " = " << 0;
	query_string << " OR ";
	query_string << _Hyd_River_Profile::erg_instat_table->get_column_name(label::measure_id) << " = " << id.measure_nr;
	query_string << " ) ";
	query_string << " AND ";
	query_string << _Hyd_River_Profile::erg_instat_table->get_column_name(hyd_label::sz_bound_id) << " = " << bound_sz;
	query_string << " AND ";
	if (like_flag == false) {
		query_string << _Hyd_River_Profile::erg_instat_table->get_column_name(risk_label::sz_break_id) << " = '" << break_sz << "'";
	}
	else {
		query_string << _Hyd_River_Profile::erg_instat_table->get_column_name(risk_label::sz_break_id) << " LIKE '" << break_sz << "'";
	}
	query_string << " order by " << _Hyd_River_Profile::erg_instat_table->get_column_name(label::glob_id) << " ASC ";


	Data_Base::database_request(query, query_string.str(), ptr_database);
	if (query->lastError().isValid() == true) {
		Error msg;
		msg.set_msg("_Hyd_River_Profile::select_data_in_instat_erg_table(QSqlQueryModel *query, QSqlDatabase *ptr_database, const _sys_system_id id, const int bound_sz, const int rv_no, const string break_sz )", "Invalid database request", "Check the database", 2, false);
		ostringstream info;
		info << "Table Name      : " << _Hyd_River_Profile::erg_instat_table->get_table_name() << endl;
		info << "Table error info: " << query->lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
	return query->rowCount();


}

//Delete all data in the database of all tables for the river profile data (static)
void _Hyd_River_Profile::delete_data_in_table(QSqlDatabase *ptr_database){
	//delete all tables
	_Hyd_River_Profile::delete_data_in_profile_table(ptr_database);
	_Hyd_River_Profile::delete_data_in_boundary_table(ptr_database);
	_Hyd_River_Profile::delete_data_in_wallbreak_table(ptr_database);
	_Hyd_River_Profile::delete_data_in_dikebreak_table(ptr_database);
	_Hyd_River_Profile::delete_data_in_bridge_table(ptr_database);
	_Hyd_River_Profile::delete_data_in_erg_table(ptr_database);
	_Hyd_River_Profile::delete_data_in_erg_instat_table(ptr_database);

	//delete the data of the appending table; the profile points
	_Hyd_River_Profile_Type::delete_data_in_table(ptr_database);
}
//Close and delete the database of all tables for the river profile data (static)
void _Hyd_River_Profile::close_table(void){
	//close all tables
	_Hyd_River_Profile::close_profile_table();
	_Hyd_River_Profile::close_boundary_table();
	_Hyd_River_Profile::close_wallbreak_table();
	_Hyd_River_Profile::close_dikebreak_table();
	_Hyd_River_Profile::close_bridge_table();
	_Hyd_River_Profile::close_erg_table();
	_Hyd_River_Profile::close_erg_instat_table();

	//close the appending table of the profile points
	_Hyd_River_Profile_Type::close_table();
}
//Switch the applied-flag for the 1-d river profiles in the database table for a defined system state
void _Hyd_River_Profile::switch_applied_flag_profile_table(QSqlDatabase *ptr_database, const _sys_system_id id, const bool flag){
	//the table is set (the name and the column names) and allocated
	try{
		_Hyd_River_Profile::set_profile_table(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}
	QSqlQueryModel query;

	ostringstream query_string;
	query_string <<"UPDATE ";
	query_string << _Hyd_River_Profile::profile_table->get_table_name();
	query_string << " SET " ;
	query_string  << _Hyd_River_Profile::profile_table->get_column_name(label::applied_flag) << " = "<< functions::convert_boolean2string(flag);
	query_string  << " WHERE ";
	query_string  << _Hyd_River_Profile::profile_table->get_column_name(label::areastate_id) << " = "  << id.area_state;
	query_string  << " AND ";
	query_string  <<  _Hyd_River_Profile::profile_table->get_column_name(label::measure_id) << " = " << id.measure_nr ;
	query_string  << " AND ";
	query_string  << _Hyd_River_Profile::profile_table->get_column_name(label::applied_flag) << " = "<< functions::convert_boolean2string(!flag);


	Data_Base::database_request(&query, query_string.str(), ptr_database);
	if(query.lastError().isValid()==true){
		Error msg;
		msg.set_msg("_Hyd_River_Profile::switch_applied_flag_profile_table(QSqlDatabase *ptr_database, const _sys_system_id id, const bool flag)","Invalid database request", "Check the database", 2, false);
		ostringstream info;
		info << "Table Name      : " << _Hyd_River_Profile::profile_table->get_table_name() << endl;
		info << "Table error info: " << query.lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
}
//Switch the applied-flag for the 1-d river profiles in the database table for a defined system state and river number (static)
void _Hyd_River_Profile::switch_applied_flag_profile_table(QSqlDatabase *ptr_database, const _sys_system_id id, const int rv_no, const bool flag){

	//the table is set (the name and the column names) and allocated
	try{
		_Hyd_River_Profile::set_profile_table(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}
	QSqlQueryModel query;

	ostringstream query_string;
	query_string <<"UPDATE ";
	query_string << _Hyd_River_Profile::profile_table->get_table_name();
	query_string << " SET " ;
	query_string  << _Hyd_River_Profile::profile_table->get_column_name(label::applied_flag) << " = "<< functions::convert_boolean2string(flag);
	query_string  << " WHERE ";
	query_string  << _Hyd_River_Profile::profile_table->get_column_name(label::areastate_id) << " = "  << id.area_state;
	query_string  << " AND ";
	query_string  <<  _Hyd_River_Profile::profile_table->get_column_name(label::measure_id) << " = " << id.measure_nr ;
	query_string  << " AND ";
	query_string  << _Hyd_River_Profile::profile_table->get_column_name(label::applied_flag) << " = "<< functions::convert_boolean2string(!flag);
	query_string  << " AND ";
	query_string  << _Hyd_River_Profile::profile_table->get_column_name(hyd_label::profdata_rvno) << " = "<< rv_no;

	Data_Base::database_request(&query, query_string.str(), ptr_database);
	if(query.lastError().isValid()==true){
		Error msg;
		msg.set_msg("_Hyd_River_Profile::switch_applied_flag_profile_table(QSqlDatabase *ptr_database, const _sys_system_id id, const int rv_no, const bool flag)","Invalid database request", "Check the database", 2, false);
		ostringstream info;
		info << "Table Name      : " << _Hyd_River_Profile::profile_table->get_table_name() << endl;
		info << "Table error info: " << query.lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
}
//Switch the applied-flag for the 1-d results in the database table for a defined system state (static)
void _Hyd_River_Profile::switch_applied_flag_erg_table(QSqlDatabase *ptr_database, const _sys_system_id id, const bool flag){
	//the table is set (the name and the column names) and allocated
	try{
		_Hyd_River_Profile::set_erg_table(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}
	QSqlQueryModel query;

	ostringstream query_string;
	query_string <<"UPDATE ";
	query_string << _Hyd_River_Profile::erg_table->get_table_name();
	query_string << " SET " ;
	query_string  << _Hyd_River_Profile::erg_table->get_column_name(label::applied_flag) << " = "<< functions::convert_boolean2string(flag);
	query_string  << " WHERE ";
	query_string  << _Hyd_River_Profile::erg_table->get_column_name(label::areastate_id) << " = "  << id.area_state;
	query_string  << " AND ";
	query_string  <<  _Hyd_River_Profile::erg_table->get_column_name(label::measure_id) << " = " << id.measure_nr ;
	query_string  << " AND ";
	query_string  << _Hyd_River_Profile::erg_table->get_column_name(label::applied_flag) << " = "<< functions::convert_boolean2string(!flag);

	Data_Base::database_request(&query, query_string.str(), ptr_database);
	if(query.lastError().isValid()==true){
		Error msg;
		msg.set_msg("_Hyd_River_Profile::switch_applied_flag_erg_table(QSqlDatabase *ptr_database, const _sys_system_id id, const bool flag)","Invalid database request", "Check the database", 2, false);
		ostringstream info;
		info << "Table Name      : " << _Hyd_River_Profile::erg_table->get_table_name() << endl;
		info << "Table error info: " << query.lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}

	_Hyd_River_Profile::switch_applied_flag_erg_instat_table(ptr_database, id, flag);
}
//Switch the applied-flag for the 1-d results in the database table for a defined system state (static)
void _Hyd_River_Profile::switch_applied_flag_erg_table(QSqlDatabase *ptr_database, const _sys_system_id id, const int hyd_sc, const bool flag){
	//the table is set (the name and the column names) and allocated
	try{
		_Hyd_River_Profile::set_erg_table(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}
	QSqlQueryModel query;

	ostringstream query_string;
	query_string <<"UPDATE ";
	query_string << _Hyd_River_Profile::erg_table->get_table_name();
	query_string << " SET " ;
	query_string  << _Hyd_River_Profile::erg_table->get_column_name(label::applied_flag) << " = "<< functions::convert_boolean2string(flag);
	query_string  << " WHERE ";
	query_string  << _Hyd_River_Profile::erg_table->get_column_name(label::areastate_id) << " = "  << id.area_state;
	query_string  << " AND ";
	query_string  <<  _Hyd_River_Profile::erg_table->get_column_name(label::measure_id) << " = " << id.measure_nr ;
	query_string  << " AND ";
	query_string  <<  _Hyd_River_Profile::erg_table->get_column_name(hyd_label::sz_bound_id) << " = " << hyd_sc ;
	query_string  << " AND ";
	query_string  << _Hyd_River_Profile::erg_table->get_column_name(label::applied_flag) << " = "<< functions::convert_boolean2string(!flag);

	Data_Base::database_request(&query, query_string.str(), ptr_database);
	if(query.lastError().isValid()==true){
		Error msg;
		msg.set_msg("_Hyd_River_Profile::switch_applied_flag_erg_table(QSqlDatabase *ptr_database, const _sys_system_id id, const int hyd_sc, const bool flag)","Invalid database request", "Check the database", 2, false);
		ostringstream info;
		info << "Table Name      : " << _Hyd_River_Profile::erg_table->get_table_name() << endl;
		info << "Table error info: " << query.lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
	_Hyd_River_Profile::switch_applied_flag_erg_instat_table(ptr_database, id, hyd_sc, flag);
}
//Switch the applied-flag for the 1-d instationary results in the database table for a defined system state
void _Hyd_River_Profile::switch_applied_flag_erg_instat_table(QSqlDatabase *ptr_database, const _sys_system_id id, const bool flag) {
	//the table is set (the name and the column names) and allocated
	try {
		_Hyd_River_Profile::set_erg_instat_table(ptr_database);
	}
	catch (Error msg) {
		throw msg;
	}
	QSqlQueryModel query;

	ostringstream query_string;
	query_string << "UPDATE ";
	query_string << _Hyd_River_Profile::erg_instat_table->get_table_name();
	query_string << " SET ";
	query_string << _Hyd_River_Profile::erg_instat_table->get_column_name(label::applied_flag) << " = " << functions::convert_boolean2string(flag);
	query_string << " WHERE ";
	query_string << _Hyd_River_Profile::erg_instat_table->get_column_name(label::areastate_id) << " = " << id.area_state;
	query_string << " AND ";
	query_string << _Hyd_River_Profile::erg_instat_table->get_column_name(label::measure_id) << " = " << id.measure_nr;
	query_string << " AND ";
	query_string << _Hyd_River_Profile::erg_instat_table->get_column_name(label::applied_flag) << " = " << functions::convert_boolean2string(!flag);

	Data_Base::database_request(&query, query_string.str(), ptr_database);
	if (query.lastError().isValid() == true) {
		Error msg;
		msg.set_msg("_Hyd_River_Profile::switch_applied_flag_erg_instat_table(QSqlDatabase *ptr_database, const _sys_system_id id, const bool flag)", "Invalid database request", "Check the database", 2, false);
		ostringstream info;
		info << "Table Name      : " << _Hyd_River_Profile::erg_instat_table->get_table_name() << endl;
		info << "Table error info: " << query.lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}

}
//Switch the applied-flag for the 1-d instationbary results in the database table for a defined system state
void _Hyd_River_Profile::switch_applied_flag_erg_instat_table(QSqlDatabase *ptr_database, const _sys_system_id id, const int hyd_sc, const bool flag) {
	//the table is set (the name and the column names) and allocated
	try {
		_Hyd_River_Profile::set_erg_instat_table(ptr_database);
	}
	catch (Error msg) {
		throw msg;
	}
	QSqlQueryModel query;

	ostringstream query_string;
	query_string << "UPDATE ";
	query_string << _Hyd_River_Profile::erg_instat_table->get_table_name();
	query_string << " SET ";
	query_string << _Hyd_River_Profile::erg_instat_table->get_column_name(label::applied_flag) << " = " << functions::convert_boolean2string(flag);
	query_string << " WHERE ";
	query_string << _Hyd_River_Profile::erg_instat_table->get_column_name(label::areastate_id) << " = " << id.area_state;
	query_string << " AND ";
	query_string << _Hyd_River_Profile::erg_instat_table->get_column_name(label::measure_id) << " = " << id.measure_nr;
	query_string << " AND ";
	query_string << _Hyd_River_Profile::erg_instat_table->get_column_name(hyd_label::sz_bound_id) << " = " << hyd_sc;
	query_string << " AND ";
	query_string << _Hyd_River_Profile::erg_instat_table->get_column_name(label::applied_flag) << " = " << functions::convert_boolean2string(!flag);

	Data_Base::database_request(&query, query_string.str(), ptr_database);
	if (query.lastError().isValid() == true) {
		Error msg;
		msg.set_msg("_Hyd_River_Profile::switch_applied_flag_erg_instat_table(QSqlDatabase *ptr_database, const _sys_system_id id, const int hyd_sc, const bool flag)", "Invalid database request", "Check the database", 2, false);
		ostringstream info;
		info << "Table Name      : " << _Hyd_River_Profile::erg_instat_table->get_table_name() << endl;
		info << "Table error info: " << query.lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}


}
//Switch the applied-flag for the boundary condition in the database table for a defined system state (static)
void _Hyd_River_Profile::switch_applied_flag_boundary_table(QSqlDatabase *ptr_database, const _sys_system_id id, const bool flag){
	//the table is set (the name and the column names) and allocated
	try{
		_Hyd_River_Profile::set_profile_boundary_table(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}
	QSqlQueryModel query;

	ostringstream query_string;
	query_string <<"UPDATE ";
	query_string << _Hyd_River_Profile::boundary_table->get_table_name();
	query_string << " SET " ;
	query_string  << _Hyd_River_Profile::boundary_table->get_column_name(label::applied_flag) << " = "<< functions::convert_boolean2string(flag);
	query_string  << " WHERE ";
	query_string  << _Hyd_River_Profile::boundary_table->get_column_name(label::areastate_id) << " = "  << id.area_state;
	query_string  << " AND ";
	query_string  <<  _Hyd_River_Profile::boundary_table->get_column_name(label::measure_id) << " = " << id.measure_nr ;

	Data_Base::database_request(&query, query_string.str(), ptr_database);
	if(query.lastError().isValid()==true){
		Error msg;
		msg.set_msg("_Hyd_River_Profile::switch_applied_flag_boundary_table(QSqlDatabase *ptr_database, const _sys_system_id id, const bool flag)","Invalid database request", "Check the database", 2, false);
		ostringstream info;
		info << "Table Name      : " << _Hyd_River_Profile::boundary_table->get_table_name() << endl;
		info << "Table error info: " << query.lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
}
//Switch the applied-flag for the boundary condition in the database table for a defined system state (static)
void _Hyd_River_Profile::switch_applied_flag_boundary_table(QSqlDatabase *ptr_database, const _sys_system_id id, const int hyd_sc, const bool flag){
	//the table is set (the name and the column names) and allocated
	try{
		_Hyd_River_Profile::set_profile_boundary_table(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}
	QSqlQueryModel query;

	ostringstream query_string;
	query_string <<"UPDATE ";
	query_string << _Hyd_River_Profile::boundary_table->get_table_name();
	query_string << " SET " ;
	query_string  << _Hyd_River_Profile::boundary_table->get_column_name(label::applied_flag) << " = "<< functions::convert_boolean2string(flag);
	query_string  << " WHERE ";
	query_string  << _Hyd_River_Profile::boundary_table->get_column_name(label::areastate_id) << " ="  << id.area_state;
	query_string  << " AND ";
	query_string  <<  _Hyd_River_Profile::boundary_table->get_column_name(label::measure_id) << " = " << id.measure_nr ;
	query_string  << " AND ";
	query_string  <<  _Hyd_River_Profile::boundary_table->get_column_name(hyd_label::sz_bound_id) << " = " << hyd_sc ;

	Data_Base::database_request(&query, query_string.str(), ptr_database);
	if(query.lastError().isValid()==true){
		Error msg;
		msg.set_msg("_Hyd_River_Profile::switch_applied_flag_boundary_table(QSqlDatabase *ptr_database, const _sys_system_id id, const int hyd_sc, const bool flag)","Invalid database request", "Check the database", 2, false);
		ostringstream info;
		info << "Table Name      : " << _Hyd_River_Profile::boundary_table->get_table_name() << endl;
		info << "Table error info: " << query.lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
}
//Copy the results of a given system id to another one in database table (static)
void _Hyd_River_Profile::copy_results(QSqlDatabase *ptr_database, const _sys_system_id src, const _sys_system_id dest){
	//the table is set (the name and the column names) and allocated
	try{
		_Hyd_River_Profile::set_erg_table(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}

	int glob_id=0;
	glob_id=_Hyd_River_Profile::erg_table->maximum_int_of_column(_Hyd_River_Profile::erg_table->get_column_name(label::glob_id) ,ptr_database)+1;

	QSqlQueryModel model;
	ostringstream test_filter;
	test_filter<< "SELECT ";
	test_filter << _Hyd_River_Profile::erg_table->get_column_name(label::glob_id);
	test_filter << " from " << _Hyd_River_Profile::erg_table->get_table_name();
	test_filter << " WHERE ";
	test_filter << _Hyd_River_Profile::erg_table->get_column_name(label::areastate_id) << " = " << src.area_state;
	test_filter << " AND ";
	test_filter << _Hyd_River_Profile::erg_table->get_column_name(label::measure_id) << " = " << src.measure_nr ;

	//submit it to the datbase
	Data_Base::database_request(&model, test_filter.str(), ptr_database);
	if(model.lastError().isValid()==true){
		Error msg;
		msg.set_msg("_Hyd_River_Profile::copy_results(QSqlDatabase *ptr_database, const _sys_system_id src, const _sys_system_id dest)","Invalid database request", "Check the database", 2, false);
		ostringstream info;
		info << "Table Name      : " << _Hyd_River_Profile::erg_table->get_table_name() << endl;
		info << "Table error info: " << model.lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
	test_filter.str("");

	QSqlQueryModel model1;

	ostringstream cout;
	cout <<"Copy "<<model.rowCount()<<" results of the river profile(s) to the new measure state..."<<endl;
	Sys_Common_Output::output_hyd->output_txt(&cout);
	for(int i=0; i< model.rowCount(); i++){
		if(i%1000==0 && i>0){
			cout << "Copy profile's results "<< i <<" to " << i+1000 <<"..."<< endl;
			Sys_Common_Output::output_hyd->output_txt(&cout);
		}
		test_filter.str("");
		test_filter << "INSERT INTO "<< _Hyd_River_Profile::erg_table->get_table_name();
		test_filter << " SELECT " << glob_id <<" , ";
		test_filter <<  _Hyd_River_Profile::erg_table->get_column_name(hyd_label::profdata_rvno)  <<" , ";
		test_filter <<  _Hyd_River_Profile::erg_table->get_column_name(hyd_label::profdata_prof_id)  <<" , ";
		test_filter <<  dest.area_state  <<" , ";
		test_filter <<  dest.measure_nr  <<" , ";
		test_filter <<  _Hyd_River_Profile::erg_table->get_column_name(label::applied_flag)  <<" , ";
		test_filter <<  _Hyd_River_Profile::erg_table->get_column_name(hyd_label::sz_bound_id)  <<" , ";
		test_filter <<  _Hyd_River_Profile::erg_table->get_column_name(risk_label::sz_break_id)  <<" , ";
		test_filter <<  _Hyd_River_Profile::erg_table->get_column_name(hyd_label::proferg_h_max)  <<" , ";
		test_filter <<  _Hyd_River_Profile::erg_table->get_column_name(hyd_label::proferg_s_max)  <<" , ";
		test_filter <<  _Hyd_River_Profile::erg_table->get_column_name(hyd_label::proferg_hmax_lb)  <<" , ";
		test_filter <<  _Hyd_River_Profile::erg_table->get_column_name(hyd_label::proferg_hmax_rb)  <<" , ";
		test_filter <<  _Hyd_River_Profile::erg_table->get_column_name(hyd_label::proferg_t_hmax)  <<" , ";
		test_filter <<  _Hyd_River_Profile::erg_table->get_column_name(hyd_label::proferg_hmax_lb_break)  <<" , ";
		test_filter <<  _Hyd_River_Profile::erg_table->get_column_name(hyd_label::proferg_hmax_rb_break)  <<" , ";
		test_filter <<  _Hyd_River_Profile::erg_table->get_column_name(hyd_label::proferg_v_max)  <<" , ";
		test_filter <<  _Hyd_River_Profile::erg_table->get_column_name(hyd_label::proferg_dur_wet)  <<" , ";
		test_filter <<  _Hyd_River_Profile::erg_table->get_column_name(hyd_label::proferg_dur_dry)  <<" , ";
		test_filter <<  _Hyd_River_Profile::erg_table->get_column_name(hyd_label::proferg_cv_rv_in)  <<" , ";
		test_filter <<  _Hyd_River_Profile::erg_table->get_column_name(hyd_label::proferg_cv_rv_out)  <<" , ";
		test_filter <<  _Hyd_River_Profile::erg_table->get_column_name(hyd_label::proferg_cv_struc_in)  <<" , ";
		test_filter <<  _Hyd_River_Profile::erg_table->get_column_name(hyd_label::proferg_cv_struc_out)  <<" , ";
		test_filter <<  _Hyd_River_Profile::erg_table->get_column_name(hyd_label::proferg_cv_l_ov_in)  <<" , ";
		test_filter <<  _Hyd_River_Profile::erg_table->get_column_name(hyd_label::proferg_cv_l_ov_out)  <<" , ";
		test_filter <<  _Hyd_River_Profile::erg_table->get_column_name(hyd_label::proferg_cv_r_ov_in)  <<" , ";
		test_filter <<  _Hyd_River_Profile::erg_table->get_column_name(hyd_label::proferg_cv_r_ov_out)  <<" , ";
		test_filter <<  _Hyd_River_Profile::erg_table->get_column_name(hyd_label::proferg_cv_l_db_in)  <<" , ";
		test_filter <<  _Hyd_River_Profile::erg_table->get_column_name(hyd_label::proferg_cv_l_db_out)  <<" , ";
		test_filter <<  _Hyd_River_Profile::erg_table->get_column_name(hyd_label::proferg_cv_r_db_in)  <<" , ";
		test_filter <<  _Hyd_River_Profile::erg_table->get_column_name(hyd_label::proferg_cv_r_db_out)  <<" , ";
		test_filter <<  _Hyd_River_Profile::erg_table->get_column_name(hyd_label::proferg_q_max)  <<" , ";
		test_filter <<  _Hyd_River_Profile::erg_table->get_column_name(hyd_label::proferg_polygon)  <<"  ";
		test_filter << " FROM " << _Hyd_River_Profile::erg_table->get_table_name() <<" ";
		test_filter << " WHERE " << _Hyd_River_Profile::erg_table->get_column_name(label::glob_id) << " = ";
		test_filter << model.record(i).value(_Hyd_River_Profile::erg_table->get_column_name(label::glob_id).c_str()).toInt();
		Data_Base::database_request(&model1, test_filter.str(), ptr_database);
		if(model1.lastError().isValid()==true){
			Error msg;
			msg.set_msg("_Hyd_River_Profile::copy_results(QSqlDatabase *ptr_database, const _sys_system_id src, const _sys_system_id dest)","Invalid database request", "Check the database", 2, false);
			ostringstream info;
			info << "Table Name      : " << _Hyd_River_Profile::erg_table->get_table_name() << endl;
			info << "Table error info: " << model1.lastError().text().toStdString() << endl;
			msg.make_second_info(info.str());
			throw msg;
		}
		glob_id++;
	}
	try{
		_Hyd_River_Profile::switch_applied_flag_erg_table(ptr_database, src, false);
	}
	catch(Error msg){
		throw msg;
	}
}
//Copy the instationary results of a given system id to another one in database table
void _Hyd_River_Profile::copy_instat_results(QSqlDatabase *ptr_database, const _sys_system_id src, const _sys_system_id dest) {
	//the table is set (the name and the column names) and allocated
	try {
		_Hyd_River_Profile::set_erg_instat_table(ptr_database);
	}
	catch (Error msg) {
		throw msg;
	}

	int glob_id = 0;
	glob_id = _Hyd_River_Profile::erg_instat_table->maximum_int_of_column(_Hyd_River_Profile::erg_instat_table->get_column_name(label::glob_id), ptr_database) + 1;

	QSqlQueryModel model;
	ostringstream test_filter;
	test_filter << "SELECT ";
	test_filter << _Hyd_River_Profile::erg_instat_table->get_column_name(label::glob_id);
	test_filter << " from " << _Hyd_River_Profile::erg_instat_table->get_table_name();
	test_filter << " WHERE ";
	test_filter << _Hyd_River_Profile::erg_instat_table->get_column_name(label::areastate_id) << " = " << src.area_state;
	test_filter << " AND ";
	test_filter << _Hyd_River_Profile::erg_instat_table->get_column_name(label::measure_id) << " = " << src.measure_nr;

	//submit it to the datbase
	Data_Base::database_request(&model, test_filter.str(), ptr_database);
	if (model.lastError().isValid() == true) {
		Error msg;
		msg.set_msg("_Hyd_River_Profile::copy_instat_results(QSqlDatabase *ptr_database, const _sys_system_id src, const _sys_system_id dest)", "Invalid database request", "Check the database", 2, false);
		ostringstream info;
		info << "Table Name      : " << _Hyd_River_Profile::erg_instat_table->get_table_name() << endl;
		info << "Table error info: " << model.lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
	test_filter.str("");

	QSqlQueryModel model1;

	ostringstream cout;
	cout << "Copy " << model.rowCount() << " instationary results of the river profile(s) to the new measure state..." << endl;
	Sys_Common_Output::output_hyd->output_txt(&cout);
	for (int i = 0; i < model.rowCount(); i++) {
		if (i % 1000 == 0 && i > 0) {
			cout << "Copy profile's results " << i << " to " << i + 1000 << "..." << endl;
			Sys_Common_Output::output_hyd->output_txt(&cout);
		}
		test_filter.str("");
		test_filter << "INSERT INTO " << _Hyd_River_Profile::erg_instat_table->get_table_name();
		test_filter << " SELECT " << glob_id << " , ";
		test_filter << _Hyd_River_Profile::erg_instat_table->get_column_name(hyd_label::profdata_rvno) << " , ";
		test_filter << dest.area_state << " , ";
		test_filter << dest.measure_nr << " , ";
		test_filter << _Hyd_River_Profile::erg_instat_table->get_column_name(label::applied_flag) << " , ";
		test_filter << _Hyd_River_Profile::erg_instat_table->get_column_name(hyd_label::sz_bound_id) << " , ";
		test_filter << _Hyd_River_Profile::erg_instat_table->get_column_name(risk_label::sz_break_id) << " , ";
		test_filter << _Hyd_River_Profile::erg_instat_table->get_column_name(hyd_label::proferg_h_max) << " , ";
		test_filter << _Hyd_River_Profile::erg_instat_table->get_column_name(hyd_label::proferg_s_max) << " , ";
		test_filter << _Hyd_River_Profile::erg_instat_table->get_column_name(hyd_label::proferg_v_max) << " , ";
		test_filter << _Hyd_River_Profile::erg_instat_table->get_column_name(hyd_label::proferg_q_max) << " , ";
		test_filter << _Hyd_River_Profile::erg_instat_table->get_column_name(hyd_label::data_time) << " , ";
		test_filter << _Hyd_River_Profile::erg_instat_table->get_column_name(hyd_label::proferg_polygon) << "  ";
		test_filter << " FROM " << _Hyd_River_Profile::erg_instat_table->get_table_name() << " ";
		test_filter << " WHERE " << _Hyd_River_Profile::erg_instat_table->get_column_name(label::glob_id) << " = ";
		test_filter << model.record(i).value(_Hyd_River_Profile::erg_instat_table->get_column_name(label::glob_id).c_str()).toInt();
		Data_Base::database_request(&model1, test_filter.str(), ptr_database);
		if (model1.lastError().isValid() == true) {
			Error msg;
			msg.set_msg("_Hyd_River_Profile::copy_instat_results(QSqlDatabase *ptr_database, const _sys_system_id src, const _sys_system_id dest)", "Invalid database request", "Check the database", 2, false);
			ostringstream info;
			info << "Table Name      : " << _Hyd_River_Profile::erg_instat_table->get_table_name() << endl;
			info << "Table error info: " << model1.lastError().text().toStdString() << endl;
			msg.make_second_info(info.str());
			throw msg;
		}
		glob_id++;
	}
	try {
		_Hyd_River_Profile::switch_applied_flag_erg_instat_table(ptr_database, src, false);
	}
	catch (Error msg) {
		throw msg;
	}

}
//Copy the boundary condition from the source global profile id to the target (static)
void _Hyd_River_Profile::copy_boundary_condition(QSqlDatabase *ptr_database, const _sys_system_id base, const int src, const int targ){

	//the table is set (the name and the column names) and allocated
	try{
		_Hyd_River_Profile::set_profile_boundary_table(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}


	ostringstream test_filter;

	QSqlQueryModel model1;

	test_filter.str("");
	test_filter << "INSERT INTO "<< _Hyd_River_Profile::boundary_table->get_table_name();
	test_filter << " SELECT " << targ <<" , ";
	test_filter <<  _Hyd_River_Profile::boundary_table->get_column_name(label::areastate_id)  <<" , ";
	test_filter <<  _Hyd_River_Profile::boundary_table->get_column_name(label::measure_id)  <<" , ";
	test_filter <<  functions::convert_boolean2string(true)  <<" , ";
	test_filter <<  _Hyd_River_Profile::boundary_table->get_column_name(hyd_label::sz_bound_id)  <<" , ";
	test_filter <<  _Hyd_River_Profile::boundary_table->get_column_name(hyd_label::bounddata_stat)  <<" , ";
	test_filter <<  _Hyd_River_Profile::boundary_table->get_column_name(hyd_label::bounddata_value)  <<" , ";
	test_filter <<  _Hyd_River_Profile::boundary_table->get_column_name(hyd_label::bounddata_type)  <<"  ";


	test_filter << " FROM " <<  _Hyd_River_Profile::boundary_table->get_table_name() <<" ";
	test_filter << " WHERE ";
	test_filter << _Hyd_River_Profile::boundary_table->get_column_name(label::areastate_id) << " = " << base.area_state;
	test_filter << " AND ";
	test_filter << _Hyd_River_Profile::boundary_table->get_column_name(label::measure_id) << " = " << base.measure_nr ;
	test_filter << " AND ";
	test_filter << _Hyd_River_Profile::boundary_table->get_column_name(hyd_label::profdata_glob_id) << " = " << src ;

		
		
	Data_Base::database_request(&model1, test_filter.str(), ptr_database);
	if(model1.lastError().isValid()==true){
		Error msg;
		msg.set_msg("_Hyd_River_Profile::copy_boundary_condition(QSqlDatabase *ptr_database, const _sys_system_id base, const int src, const int targ)","Invalid database request", "Check the database", 2, false);
		ostringstream info;
		info << "Table Name      : " << _Hyd_River_Profile::boundary_table->get_table_name() << endl;
		info << "Table error info: " << model1.lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}



}
//Initialize the profile
void _Hyd_River_Profile::init_profile(Hyd_Param_Material *material_table, const _hyd_profile_calc_setting user_setting){
	ostringstream cout;
	cout <<"Generate tables of profile " << this->profile_number << " ... "<< endl;
	Sys_Common_Output::output_hyd->output_txt(&cout, true);
	this->typ_of_profile->set_user_flags(user_setting);

	try{
		//check the table dicretization
		if(this->delta_x_table<=0.0){
			Warning msg=this->set_warning(0);
			this->delta_x_table=0.2;
			msg.output_msg(2);
		}

		this->typ_of_profile->calc_alloc_tables(this->delta_x_table, material_table);
		this->calculate_river_width();
	}
		catch(Error msg){
		ostringstream info;
		info << "Profile number:"<<this->profile_number<<endl;
		info << "Profilname    :" << this->name << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
}
//Set the initial condition to the profiles
void _Hyd_River_Profile::set_init_condition(void){
	//calculate the initial watervolume for a hydrological balance
	if(this->type!=_hyd_profile_types::WEIR_TYPE){
		this->watervolume_init=this->get_init_condition_value()*(this->distance2downstream+this->distance2upstream)*0.5;
	}
	//the watervolume after a weir is numerically not existing;
	//the section is not calculated, the discharge is given directly to the next section behind the weir/bridge section
	else{
		this->watervolume_init=0.0;
	}
	//set the waterlevels at the beginning to the init condition
	this->typ_of_profile->set_waterlevels2init_condition(this->init_condition);
}
//Get the Init-condition as an area from the class _Hyd_River_Profile_Type
double _Hyd_River_Profile::get_init_condition_value(void){
	return this->typ_of_profile->get_init_condition_value(this->init_condition);
}
//Clone river profile
void _Hyd_River_Profile::clone_profile(_Hyd_River_Profile *profile){
	this->name=profile->name;
	this->hyd_sz=profile->hyd_sz;
	this->system_id=profile->get_sys_system_id();
	this->river_station=profile->river_station;
	this->profile_number=profile->profile_number;

	this->connection=profile->connection;
	//for table calculation
	this->delta_x_table=profile->delta_x_table;

	this->break_flag_left=profile->break_flag_left;
	this->index_basepoint_left=profile->index_basepoint_left;
	this->break_flag_right=profile->break_flag_right;
	this->index_basepoint_right=profile->index_basepoint_right;

	this->init_condition=profile->init_condition;
	this->distance2upstream=profile->distance2upstream;
	this->distance2downstream=profile->distance2downstream;
	this->watervolume_init=profile->watervolume_init;

	this->max_h_2break_left=profile->max_h_2break_left;
	this->max_delta_h_2break_left=profile->max_delta_h_2break_left;
	this->max_h_2break_right=profile->max_h_2break_right;
	this->max_delta_h_2break_right=profile->max_delta_h_2break_right;
	this->river_width = profile->river_width;


	//bridge data
	if(profile->bridge_specific_value!=NULL){
		this->allocate_bridge_data();
		*this->bridge_specific_value=*profile->bridge_specific_value;
	}

	//clone profile type
	this->type=profile->type;
	this->decide_alloc_profile_type(profile->typ_of_profile->get_number_points());
	this->typ_of_profile->clone_profile_type(profile->typ_of_profile);
    ;

	if(profile->left_break_params!=NULL){
		this->allocate_break_parameter_left();
		*this->left_break_params=*profile->left_break_params;
	}
	if(profile->right_break_params!=NULL){
		this->allocate_break_parameter_right();
		*this->right_break_params=*profile->right_break_params;
	}

	this->left_bank2fpl=profile->left_bank2fpl;
	this->right_bank2fpl=profile->right_bank2fpl;

	this->height_left_bank=profile->height_left_bank;
	this->height_right_bank=profile->height_right_bank;

	this->overflow_left_flag=profile->overflow_left_flag;
	this->poleni_factor_left_overflow=profile->poleni_factor_left_overflow;
	this->overflow_right_flag=profile->overflow_right_flag;
	this->poleni_factor_right_overflow=profile->poleni_factor_right_overflow;


}
//Get the connection type of the profile
_hyd_connection_types _Hyd_River_Profile::get_connection_type(void){
	return this->connection;
}
//Get the profile type of the profile (river, weir etc.)
_hyd_profile_types _Hyd_River_Profile::get_profile_type(void){
	return this->type;
}
//Get the river_station of the profile
double _Hyd_River_Profile::get_river_station(void){
	return this->river_station;
}
//Get the distance to the upstream profile
double _Hyd_River_Profile::get_distance2upstream(void){
	return this->distance2upstream;
}
//Set the distance to the upstream profile (distance2upstream)
void _Hyd_River_Profile::set_distance2upstream(const double riverstation_upstream){
	this->distance2upstream=riverstation_upstream-this->river_station;
	if(this->distance2upstream<=0.0){
		Error msg=this->set_error(10);
		ostringstream info;
		info << "Riverstation profile (no. "<< this->profile_number << ") : " << this->river_station << label::m << endl;
		info << "Riverstation upstream profile (no. "<< this->profile_number-1 << ") : " << riverstation_upstream << label::m << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
}
//Get the distance to the downstream profile (distance2downstream)
double _Hyd_River_Profile::get_distance2downstream(void){
	return this->distance2downstream;
}
//Set the distance to the downstream profile
void _Hyd_River_Profile::set_distance2downstream(const double distance){
	this->distance2downstream=distance;
}
//Get the profile number
int _Hyd_River_Profile::get_profile_number(void){
	return this->profile_number;
}
//Get the profile name
string _Hyd_River_Profile::get_profile_name(void){
	return this->name;
}
//Get the width of the river
double _Hyd_River_Profile::get_river_width(void) {

	return this->river_width;
}
//Get if an overflow over the left river bank into the floodplain is given
bool _Hyd_River_Profile::get_overflow_flag_left(void){
	return false;
}
//Get the poleni-factor for an overflow over the left river bank into the floodplain
double _Hyd_River_Profile::get_overflow_poleni_left(void){
	return 0.0;
}
//Get if an overflow over the right river bank into the floodplain is given
bool _Hyd_River_Profile::get_overflow_flag_right(void){
	return false;
}
//Get the poleni-factor for an overflow over the right river bank into the floodplain
double _Hyd_River_Profile::get_overflow_poleni_right(void){
	return 0.0;
}
//Get the flag if a break of the left floodprotection line is possible
bool _Hyd_River_Profile::get_break_flag_left(void){
	return this->break_flag_left;
}
//Get the index of the left basepoint
int _Hyd_River_Profile::get_left_basepoint_index(void){
	return this->index_basepoint_left;
}
//Get a pointer to the left basepoint
Hyd_River_Profile_Point* _Hyd_River_Profile::get_left_basepoint(void){
	Hyd_River_Profile_Point *pointer=NULL;
	if(this->break_flag_left==true){
		return &(this->typ_of_profile->points[this->index_basepoint_left]);
	}
	return pointer;
}
//Get the flag if a break of the right floodprotection line is possible
bool _Hyd_River_Profile::get_break_flag_right(void){
	return this->break_flag_right;
}
//Get the index of the right basepoint
int _Hyd_River_Profile::get_right_basepoint_index(void){
	return this->index_basepoint_right;
}
//Get a pointer to the right basepoint
Hyd_River_Profile_Point* _Hyd_River_Profile::get_right_basepoint(void){
	Hyd_River_Profile_Point *pointer=NULL;
	if(this->break_flag_right==true){
		return &(this->typ_of_profile->points[this->index_basepoint_right]);
	}
	return pointer;
}
//Get the data structure for the break-parameters of the left bank
_hyd_break_parameters* _Hyd_River_Profile::get_break_parameter_left(void){
	if(this->connection==_hyd_connection_types::OUTFLOW_CONN){
		return NULL;
	}
	else{
		return this->left_break_params;
	}
}
//Get the data structure for the break-parameters of the right bank
_hyd_break_parameters* _Hyd_River_Profile::get_break_parameter_right(void){
	if(this->connection==_hyd_connection_types::OUTFLOW_CONN){
		return NULL;
	}
	else{
		return this->right_break_params;
	}
}
//Get the absolute height related to the left base point
double _Hyd_River_Profile::get_height_left_bank_abs(void){
	if(this->break_flag_left==false){
		return this->typ_of_profile->get_first_point()->get_global_z_coordinate();
	}
	else{
		return this->height_left_bank+this->typ_of_profile->points[this->index_basepoint_left].get_global_z_coordinate();
	}
}
//Get the absolute height related to the right base point
double _Hyd_River_Profile::get_height_right_bank_abs(void){
	if(this->break_flag_right==false){
		return this->typ_of_profile->get_last_point()->get_global_z_coordinate();
	}
	else{
		return this->height_right_bank+this->typ_of_profile->points[this->index_basepoint_right].get_global_z_coordinate();
	}
}
//Get the actual global waterlevel (s_value)
double _Hyd_River_Profile::get_actual_global_waterlevel(void){
	return this->typ_of_profile->get_actual_global_waterlevel();
}
//Get the watervolume of the init-conditions in the river segment up of the profiles
double _Hyd_River_Profile::get_watervolume_init(void){
	return this->watervolume_init;
}
//Get the watervolume in the river segment up of the profiles
double _Hyd_River_Profile::get_watervolume_element(void){
	this->calculate_watervolume_element();
	return this->watervolume;
}
//Calculate the values for a hydrological balance of the corresponding river segment (upstream) for each syncronisation step
void _Hyd_River_Profile::calculate_hydrological_balance(const double time_point){
	this->delta_time=time_point-this->old_time_point;
	this->old_time_point=time_point;
	//coupling left bank
	if(this->q_left_bank<0.0){
		//outflow
		this->left_bank_overflow_volume.volume_out=this->left_bank_overflow_volume.volume_out+abs(this->q_left_bank)*this->delta_time;
	}
	else if(this->q_left_bank>0.0){
		//inflow
		this->left_bank_overflow_volume.volume_in=this->left_bank_overflow_volume.volume_in+abs(this->q_left_bank)*this->delta_time;
	}
	//total
	this->left_bank_overflow_volume.volume_total=this->left_bank_overflow_volume.volume_in-this->left_bank_overflow_volume.volume_out;

	//coupling right bank
	if(this->q_right_bank<0.0){
		//outflow
		this->right_bank_overflow_volume.volume_out=this->right_bank_overflow_volume.volume_out+abs(this->q_right_bank)*this->delta_time;
	}
	else if(this->q_right_bank>0.0){
		//inflow
		this->right_bank_overflow_volume.volume_in=this->right_bank_overflow_volume.volume_in+abs(this->q_right_bank)*this->delta_time;
	}
	//total
	this->right_bank_overflow_volume.volume_total=this->right_bank_overflow_volume.volume_in-this->right_bank_overflow_volume.volume_out;

	//coupling 1d
	if(this->q_1d_coupling<0.0){
		//outflow
		this->coupling_1d_volume.volume_out=this->coupling_1d_volume.volume_out+abs(this->q_1d_coupling)*this->delta_time;
	}
	else if(this->q_1d_coupling>0.0){
		//inflow
		this->coupling_1d_volume.volume_in=this->coupling_1d_volume.volume_in+abs(this->q_1d_coupling)*this->delta_time;
	}
	//total
	this->coupling_1d_volume.volume_total=this->coupling_1d_volume.volume_in-this->coupling_1d_volume.volume_out;

	//structure coupling 1d2d
	if(this->q_structure_coupling<0.0){
		//outflow
		this->structure_coupling_volume.volume_out=this->structure_coupling_volume.volume_out+abs(this->q_structure_coupling)*this->delta_time;
	}
	else if(this->q_structure_coupling>0.0){
		//inflow
		this->structure_coupling_volume.volume_in=this->structure_coupling_volume.volume_in+abs(this->q_structure_coupling)*this->delta_time;
	}
	//total
	this->structure_coupling_volume.volume_total=this->structure_coupling_volume.volume_in-this->structure_coupling_volume.volume_out;

	//dikebreak coupling over the left bank 1d2d
	if(this->q_dikebreak_coupling_left<0.0){
		//outflow
		this->left_dikebreak_coupling_volume.volume_out=this->left_dikebreak_coupling_volume.volume_out+abs(this->q_dikebreak_coupling_left)*this->delta_time;
	}
	else if(this->q_dikebreak_coupling_left>0.0){
		//inflow
		this->left_dikebreak_coupling_volume.volume_in=this->left_dikebreak_coupling_volume.volume_in+abs(this->q_dikebreak_coupling_left)*this->delta_time;
	}
	//total
	this->left_dikebreak_coupling_volume.volume_total=this->left_dikebreak_coupling_volume.volume_in-this->left_dikebreak_coupling_volume.volume_out;

	//dikebreak coupling over the right bank 1d2d
	if(this->q_dikebreak_coupling_right<0.0){
		//outflow
		this->right_dikebreak_coupling_volume.volume_out=this->right_dikebreak_coupling_volume.volume_out+abs(this->q_dikebreak_coupling_right)*this->delta_time;
	}
	else if(this->q_dikebreak_coupling_right>0.0){
		//inflow
		this->right_dikebreak_coupling_volume.volume_in=this->right_dikebreak_coupling_volume.volume_in+abs(this->q_dikebreak_coupling_right)*this->delta_time;
	}
	//total
	this->right_dikebreak_coupling_volume.volume_total=this->right_dikebreak_coupling_volume.volume_in-this->right_dikebreak_coupling_volume.volume_out;
}
//Reset all coupling discharges; use it before syncronization
void _Hyd_River_Profile::reset_coupling_discharge(void){
	this->q_left_bank=0.0;
	this->q_right_bank=0.0;
	this->q_1d_coupling=0.0;
	this->q_structure_coupling=0.0;
	this->q_dikebreak_coupling_left=0.0;
	this->q_dikebreak_coupling_right=0.0;
}
//Add a discharge value to the coupling discharge over the left bank [m�/(s)] in flow direction (coupling RV2FP)
void _Hyd_River_Profile::add_coupling_discharge_left_bank(const double value){
	this->q_left_bank=this->q_left_bank+value;
}
//Add a discharge value to the coupling discharge over the right bank [m�/(s)] in flow direction (coupling RV2FP)
void _Hyd_River_Profile::add_coupling_discharge_right_bank(const double value){
	this->q_right_bank=this->q_right_bank+value;
}
//Reset the discharge value to the coupling discharge over the left bank  in flow direction (coupling RV2FP)
void _Hyd_River_Profile::reset_coupling_discharge_left_bank(void){
	this->q_left_bank=0.0;
}
//Reset the discharge value to the coupling discharge over the right bank  in flow direction (coupling RV2FP)
void _Hyd_River_Profile::reset_coupling_discharge_right_bank(void){
	this->q_right_bank=0.0;
}
//Get the discharge value due to the coupling discharge over the left bank in flow direction (coupling RV2FP)
double _Hyd_River_Profile::get_coupling_discharge_left_bank(void){
	return this->q_left_bank;
}
//Get the discharge value due to the coupling discharge over the right bank in flow direction (coupling RV2FP)
double _Hyd_River_Profile::get_coupling_discharge_right_bank(void){
	return this->q_right_bank;
}
//Add a discharge value to the 1d-coupling discharge (coupling RV2RV)
void _Hyd_River_Profile::add_coupling_discharge_1d(const double value){
	this->q_1d_coupling=this->q_1d_coupling+value;
}
//Reset the discharge value due to the 1d-coupling discharge (coupling RV2RV)
void _Hyd_River_Profile::reset_coupling_discharge_1d(void){
	this->q_1d_coupling=0.0;
}
//Get the discharge value due to the 1-coupling discharge (coupling RV2RV)
double _Hyd_River_Profile::get_coupling_discharge_1d(void){
	return this->q_1d_coupling;
}
//Add a discharge value to the coupling discharge through a hydraulic structure (coupling RV2FP via structure)
void _Hyd_River_Profile::add_structure_coupling_discharge(const double value){
	this->q_structure_coupling=this->q_structure_coupling+value;
}
//Reset a discharge value to the coupling discharge through a hydraulic structure (coupling RV2FP via structure)
void _Hyd_River_Profile::reset_structure_coupling_discharge(void){
	this->q_structure_coupling=0.0;
}
//Get a discharge value due to the coupling discharge through a hydraulic structure (coupling RV2FP via structure)
double _Hyd_River_Profile::get_structure_coupling_discharge(void){
	return this->q_structure_coupling;
}
//Add a discharge value to the coupling discharge due to a dikebreak at the left bank (coupling RV2FP via dikebreak)
void _Hyd_River_Profile::add_dikebreak_coupling_discharge_left_bank(const double value){
	this->q_dikebreak_coupling_left=this->q_dikebreak_coupling_left+value;
}
//Reset a discharge value to the coupling discharge due to a dikebreak at the left bank (coupling RV2FP via dikebreak)
void _Hyd_River_Profile::reset_dikebreak_coupling_discharge_left_bank(void){
	this->q_dikebreak_coupling_left=0.0;
}
//Get a value of the coupling discharge due to a dikebreak at the left bank (coupling RV2FP via dikebreak)
double _Hyd_River_Profile::get_dikebreak_coupling_discharge_left_bank(void){
	return this->q_dikebreak_coupling_left;
}
//Add a discharge value to the coupling discharge due to a dikebreak at the right bank (coupling RV2FP via dikebreak)
void _Hyd_River_Profile::add_dikebreak_coupling_discharge_right_bank(const double value){
	this->q_dikebreak_coupling_right=this->q_dikebreak_coupling_right+value;
}
//Reset a discharge value to the coupling discharge due to a dikebreak at the right bank (coupling RV2FP via dikebreak)
void _Hyd_River_Profile::reset_dikebreak_coupling_discharge_right_bank(void){
	this->q_dikebreak_coupling_right=0.0;
}
//Get a value of the coupling discharge due to a dikebreak at the right bank (coupling RV2FP via dikebreak)
double _Hyd_River_Profile::get_dikebreak_coupling_discharge_right_bank(void){
	return this->q_dikebreak_coupling_right;
}
//Add the hydrological balance value of the coupling condition discharges for the left bank overflow to the given pointer
void _Hyd_River_Profile::add_hydro_balance_leftbank_coupling(_hyd_hydrological_balance *given){
	given->volume_in=given->volume_in+this->left_bank_overflow_volume.volume_in;
	given->volume_out=given->volume_out+this->left_bank_overflow_volume.volume_out;
	given->volume_total=given->volume_total+this->left_bank_overflow_volume.volume_total;
}
//Add the hydrological balance value of the coupling condition discharges for the right bank overflow to the given pointer
void _Hyd_River_Profile::add_hydro_balance_rightbank_coupling(_hyd_hydrological_balance *given){
	given->volume_in=given->volume_in+this->right_bank_overflow_volume.volume_in;
	given->volume_out=given->volume_out+this->right_bank_overflow_volume.volume_out;
	given->volume_total=given->volume_total+this->right_bank_overflow_volume.volume_total;
}
//Add the hydrological balance value of the coupling condition discharges to other river-models to the given pointer
void _Hyd_River_Profile::add_hydro_balance_coupling_1d(_hyd_hydrological_balance *given){
	given->volume_in=given->volume_in+this->coupling_1d_volume.volume_in;
	given->volume_out=given->volume_out+this->coupling_1d_volume.volume_out;
	given->volume_total=given->volume_total+this->coupling_1d_volume.volume_total;
}
//Add the hydrological balance value of the coupling condition discharges to floodplain models via a hydraulic structure to the given pointer
void _Hyd_River_Profile::add_hydro_balance_structure_coupling(_hyd_hydrological_balance *given){
	given->volume_in=given->volume_in+this->structure_coupling_volume.volume_in;
	given->volume_out=given->volume_out+this->structure_coupling_volume.volume_out;
	given->volume_total=given->volume_total+this->structure_coupling_volume.volume_total;
}
//Add the hydrological balance value of the coupling condition discharges to floodplain models via a dikebreak at the left bank to the given pointer
void _Hyd_River_Profile::add_hydro_balance_dikebreak_left_coupling(_hyd_hydrological_balance *given){
	given->volume_in=given->volume_in+this->left_dikebreak_coupling_volume.volume_in;
	given->volume_out=given->volume_out+this->left_dikebreak_coupling_volume.volume_out;
	given->volume_total=given->volume_total+this->left_dikebreak_coupling_volume.volume_total;
}
//Add the hydrological balance value of the coupling condition discharges to floodplain models via a dikebreak at the right bank to the given pointer
void _Hyd_River_Profile::add_hydro_balance_dikebreak_right_coupling(_hyd_hydrological_balance *given){
	given->volume_in=given->volume_in+this->right_dikebreak_coupling_volume.volume_in;
	given->volume_out=given->volume_out+this->right_dikebreak_coupling_volume.volume_out;
	given->volume_total=given->volume_total+this->right_dikebreak_coupling_volume.volume_total;
}
//Output the header for the maximum result output to console/display
void _Hyd_River_Profile::output_header_max_results(void){
	ostringstream cout;
	cout << "No." << W(7) << "RvSt"<< label::m << W(8);
	_Hyd_River_Profile_Type::output_header_max_results(&cout);
	cout<<  "CVol_1D_in" << label::cubicmeter<< W(15);
	cout<<  "CVol_1D_out" << label::cubicmeter<< W(15);
	cout<<  "CVol_Struc_in" << label::cubicmeter<< W(15);
	cout<<  "CVol_Struc_out" << label::cubicmeter<< W(15);
	cout<<  "CVol_OV_Le_in" << label::cubicmeter<< W(15);
	cout<<  "CVol_OV_Le_out" << label::cubicmeter<< W(15);
	cout<<  "CVol_OV_Ri_in" << label::cubicmeter<< W(15);
	cout<<  "CVol_OV_Ri_out" << label::cubicmeter<< W(15);
	cout<<  "CVol_DB_Le_in" << label::cubicmeter<< W(15);
	cout<<  "CVol_DB_Le_out" << label::cubicmeter<< W(15);
	cout<<  "CVol_DB_Ri_in" << label::cubicmeter<< W(15);
	cout<<  "CVol_DB_Ri_out" << label::cubicmeter<< W(15);
	cout << "CVol_tot_in" <<label::cubicmeter<< W(15);
	cout << "CVol_tot_out" <<label::cubicmeter<< W(15);
	cout << "Q_max" <<label::qm_per_sec<< W(15);
	cout << "t(Q_max)" <<label::sec<< W(15);
	cout << endl;
	Sys_Common_Output::output_hyd->output_txt(&cout,true);
}
//Output the maximum result output to console/display
void _Hyd_River_Profile::output_max_results(void){
	ostringstream cout;
	cout << this->profile_number << W(11);
	cout << P(1) << FORMAT_FIXED_REAL<< this->river_station << W(12);
	this->typ_of_profile->output_max_results(&cout);
	cout << P(1) << FORMAT_FIXED_REAL<< this->coupling_1d_volume.volume_in << W(22);
	cout << P(1) << FORMAT_FIXED_REAL<< this->coupling_1d_volume.volume_out << W(22);
	cout << P(1) << FORMAT_FIXED_REAL<< this->structure_coupling_volume.volume_in << W(22);
	cout << P(1) << FORMAT_FIXED_REAL<< this->structure_coupling_volume.volume_out << W(22);
	cout << P(1) << FORMAT_FIXED_REAL<< this->left_bank_overflow_volume.volume_in << W(22);
	cout << P(1) << FORMAT_FIXED_REAL<< this->left_bank_overflow_volume.volume_out << W(22);
	cout << P(1) << FORMAT_FIXED_REAL<< this->right_bank_overflow_volume.volume_in << W(22);
	cout << P(1) << FORMAT_FIXED_REAL<< this->right_bank_overflow_volume.volume_out << W(22);
	cout << P(1) << FORMAT_FIXED_REAL<< this->left_dikebreak_coupling_volume.volume_in << W(22);
	cout << P(1) << FORMAT_FIXED_REAL<< this->left_dikebreak_coupling_volume.volume_out << W(22);
	cout << P(1) << FORMAT_FIXED_REAL<< this->right_dikebreak_coupling_volume.volume_in << W(22);
	cout << P(1) << FORMAT_FIXED_REAL<< this->right_dikebreak_coupling_volume.volume_out << W(22);
	double sum=0.0;
	sum=this->coupling_1d_volume.volume_in+this->structure_coupling_volume.volume_in+
		this->left_bank_overflow_volume.volume_in+this->right_bank_overflow_volume.volume_in+
		this->left_dikebreak_coupling_volume.volume_in+this->right_dikebreak_coupling_volume.volume_in;
	cout << P(1) << FORMAT_FIXED_REAL<< sum << W(22);
	sum=this->coupling_1d_volume.volume_out+this->structure_coupling_volume.volume_out+
		this->left_bank_overflow_volume.volume_out+this->right_bank_overflow_volume.volume_out+
		this->left_dikebreak_coupling_volume.volume_out+this->right_dikebreak_coupling_volume.volume_out;
	cout << P(1) << FORMAT_FIXED_REAL<< sum ;
	cout << P(2) << FORMAT_FIXED_REAL <<this->q_value_max.maximum;
	cout << P(0) << FORMAT_FIXED_REAL <<this->q_value_max.time_point;
	cout << endl;
	Sys_Common_Output::output_hyd->output_txt(&cout,true);
}
//Output the maximum result output to a given tecplot file
void _Hyd_River_Profile::output_max_results2file(ofstream *file){
	*file << P(1) << FORMAT_FIXED_REAL<< this->river_station << W(11);
	this->typ_of_profile->output_max_results2file(file);
	*file << P(1) << FORMAT_FIXED_REAL<< this->coupling_1d_volume.volume_in << W(15);
	*file << P(1) << FORMAT_FIXED_REAL<< this->coupling_1d_volume.volume_out << W(15);
	*file << P(1) << FORMAT_FIXED_REAL<< this->structure_coupling_volume.volume_in << W(15);
	*file << P(1) << FORMAT_FIXED_REAL<< this->structure_coupling_volume.volume_out << W(15);
	*file << P(1) << FORMAT_FIXED_REAL<< this->left_bank_overflow_volume.volume_in << W(15);
	*file << P(1) << FORMAT_FIXED_REAL<< this->left_bank_overflow_volume.volume_out << W(15);
	*file << P(1) << FORMAT_FIXED_REAL<< this->right_bank_overflow_volume.volume_in << W(15);
	*file << P(1) << FORMAT_FIXED_REAL<< this->right_bank_overflow_volume.volume_out << W(15);
	*file << P(1) << FORMAT_FIXED_REAL<< this->left_dikebreak_coupling_volume.volume_in << W(15);
	*file << P(1) << FORMAT_FIXED_REAL<< this->left_dikebreak_coupling_volume.volume_out << W(15);
	*file << P(1) << FORMAT_FIXED_REAL<< this->right_dikebreak_coupling_volume.volume_in << W(15);
	*file << P(1) << FORMAT_FIXED_REAL<< this->right_dikebreak_coupling_volume.volume_out << W(15);
	double sum=0.0;
	sum=this->coupling_1d_volume.volume_in+this->structure_coupling_volume.volume_in+
		this->left_bank_overflow_volume.volume_in+this->right_bank_overflow_volume.volume_in+
		this->left_dikebreak_coupling_volume.volume_in+this->right_dikebreak_coupling_volume.volume_in;
	*file << P(1) << FORMAT_FIXED_REAL<< sum << W(15);
	sum=this->coupling_1d_volume.volume_out+this->structure_coupling_volume.volume_out+
		this->left_bank_overflow_volume.volume_out+this->right_bank_overflow_volume.volume_out+
		this->left_dikebreak_coupling_volume.volume_out+this->right_dikebreak_coupling_volume.volume_out;
	*file << P(1) << FORMAT_FIXED_REAL<< sum << W(15);
	*file << P(2) << FORMAT_FIXED_REAL <<this->q_value_max.maximum << W(15);
	*file << P(0) << FORMAT_FIXED_REAL <<this->q_value_max.time_point;
	*file << endl;
}
//Output the maximum result output to a csv file
void _Hyd_River_Profile::output_max_results2csvfile(ofstream *file) {
	*file << P(1) << FORMAT_FIXED_REAL << this->river_station << W(11) << ",";
	this->typ_of_profile->output_max_results2csvfile(file);
	*file << P(1) << FORMAT_FIXED_REAL << this->coupling_1d_volume.volume_in << W(15) << ",";
	*file << P(1) << FORMAT_FIXED_REAL << this->coupling_1d_volume.volume_out << W(15) << ",";
	*file << P(1) << FORMAT_FIXED_REAL << this->structure_coupling_volume.volume_in << W(15) << ",";
	*file << P(1) << FORMAT_FIXED_REAL << this->structure_coupling_volume.volume_out << W(15) << ",";
	*file << P(1) << FORMAT_FIXED_REAL << this->left_bank_overflow_volume.volume_in << W(15) << ",";
	*file << P(1) << FORMAT_FIXED_REAL << this->left_bank_overflow_volume.volume_out << W(15) << ",";
	*file << P(1) << FORMAT_FIXED_REAL << this->right_bank_overflow_volume.volume_in << W(15) << ",";
	*file << P(1) << FORMAT_FIXED_REAL << this->right_bank_overflow_volume.volume_out << W(15) << ",";
	*file << P(1) << FORMAT_FIXED_REAL << this->left_dikebreak_coupling_volume.volume_in << W(15) << ",";
	*file << P(1) << FORMAT_FIXED_REAL << this->left_dikebreak_coupling_volume.volume_out << W(15) << ",";
	*file << P(1) << FORMAT_FIXED_REAL << this->right_dikebreak_coupling_volume.volume_in << W(15) << ",";
	*file << P(1) << FORMAT_FIXED_REAL << this->right_dikebreak_coupling_volume.volume_out << W(15) << ",";
	double sum = 0.0;
	sum = this->coupling_1d_volume.volume_in + this->structure_coupling_volume.volume_in +
		this->left_bank_overflow_volume.volume_in + this->right_bank_overflow_volume.volume_in +
		this->left_dikebreak_coupling_volume.volume_in + this->right_dikebreak_coupling_volume.volume_in;
	*file << P(1) << FORMAT_FIXED_REAL << sum << W(15) << ",";
	sum = this->coupling_1d_volume.volume_out + this->structure_coupling_volume.volume_out +
		this->left_bank_overflow_volume.volume_out + this->right_bank_overflow_volume.volume_out +
		this->left_dikebreak_coupling_volume.volume_out + this->right_dikebreak_coupling_volume.volume_out;
	*file << P(1) << FORMAT_FIXED_REAL << sum << W(15) << ",";
	*file << P(2) << FORMAT_FIXED_REAL << this->q_value_max.maximum << W(15) << ",";
	*file << P(0) << FORMAT_FIXED_REAL << this->q_value_max.time_point;

	*file << endl;

}
//Output the maximum calculated results to the database table (erg_table)
void _Hyd_River_Profile::output_max_results(QSqlDatabase *ptr_database, const int rv_no, const string polygon_string, int *glob_id, const string break_sc){
	//mysql query with the table_model
	QSqlQueryModel model;
	//the table is set (the name and the column names) and allocated
	try{
		_Hyd_River_Profile::set_erg_table(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}

	//delete the row for this result in table
	this->delete_max_result_row_in_table(ptr_database, rv_no, break_sc);

	//set the query via a query string
	ostringstream query_string;
	//put it in a functions
	this->generate_max_str2database(&query_string, rv_no, polygon_string, glob_id, break_sc);
	//query_string << "INSERT INTO  " << _Hyd_River_Profile::erg_table->get_table_name();
	//query_string <<" ( ";
	//query_string << _Hyd_River_Profile::erg_table->get_column_name(label::glob_id) <<" , ";
	//query_string << _Hyd_River_Profile::erg_table->get_column_name(hyd_label::profdata_rvno) <<" , ";
	//query_string << _Hyd_River_Profile::erg_table->get_column_name(hyd_label::profdata_prof_id) <<" , ";
	//query_string << _Hyd_River_Profile::erg_table->get_column_name(label::areastate_id) <<" , ";
	//query_string << _Hyd_River_Profile::erg_table->get_column_name(label::measure_id) <<" , ";
	//query_string << _Hyd_River_Profile::erg_table->get_column_name(hyd_label::sz_bound_id) <<" , ";
	//query_string << _Hyd_River_Profile::erg_table->get_column_name(risk_label::sz_break_id) <<" , ";
	////volumes
	//query_string << _Hyd_River_Profile::erg_table->get_column_name(hyd_label::proferg_cv_rv_in) <<" , ";
	//query_string << _Hyd_River_Profile::erg_table->get_column_name(hyd_label::proferg_cv_rv_out) <<" , ";
	//query_string << _Hyd_River_Profile::erg_table->get_column_name(hyd_label::proferg_cv_struc_in) <<" , ";
	//query_string << _Hyd_River_Profile::erg_table->get_column_name(hyd_label::proferg_cv_struc_out) <<" , ";

	//query_string << _Hyd_River_Profile::erg_table->get_column_name(hyd_label::proferg_cv_l_ov_in) <<" , ";
	//query_string << _Hyd_River_Profile::erg_table->get_column_name(hyd_label::proferg_cv_l_ov_out) <<" , ";
	//query_string << _Hyd_River_Profile::erg_table->get_column_name(hyd_label::proferg_cv_r_ov_in) <<" , ";
	//query_string << _Hyd_River_Profile::erg_table->get_column_name(hyd_label::proferg_cv_r_ov_out) <<" , ";

	//query_string << _Hyd_River_Profile::erg_table->get_column_name(hyd_label::proferg_cv_l_db_in) <<" , ";
	//query_string << _Hyd_River_Profile::erg_table->get_column_name(hyd_label::proferg_cv_l_db_out) <<" , ";
	//query_string << _Hyd_River_Profile::erg_table->get_column_name(hyd_label::proferg_cv_r_db_in) <<" , ";
	//query_string << _Hyd_River_Profile::erg_table->get_column_name(hyd_label::proferg_cv_r_db_out) <<" , ";
	////max values
	//query_string << _Hyd_River_Profile::erg_table->get_column_name(hyd_label::proferg_q_max) <<" , ";
	//query_string << _Hyd_River_Profile::erg_table->get_column_name(hyd_label::proferg_hmax_lb) <<" , ";
	//query_string << _Hyd_River_Profile::erg_table->get_column_name(hyd_label::proferg_hmax_rb) <<" , ";
	//query_string << _Hyd_River_Profile::erg_table->get_column_name(hyd_label::proferg_hmax_lb_break) <<" , ";
	//query_string << _Hyd_River_Profile::erg_table->get_column_name(hyd_label::proferg_hmax_rb_break) <<" , ";
	////max values from profile type
	//query_string << _Hyd_River_Profile::erg_table->get_column_name(hyd_label::proferg_h_max) <<" , ";
	//query_string << _Hyd_River_Profile::erg_table->get_column_name(hyd_label::proferg_s_max) <<" , ";
	//query_string << _Hyd_River_Profile::erg_table->get_column_name(hyd_label::proferg_t_hmax) <<" , ";
	//query_string << _Hyd_River_Profile::erg_table->get_column_name(hyd_label::proferg_v_max) <<" , ";
	//query_string << _Hyd_River_Profile::erg_table->get_column_name(hyd_label::proferg_width_max) << " , ";
	//query_string << _Hyd_River_Profile::erg_table->get_column_name(hyd_label::proferg_dur_wet) <<" , ";
	//query_string << _Hyd_River_Profile::erg_table->get_column_name(hyd_label::proferg_dur_dry) <<" , ";
	//query_string << _Hyd_River_Profile::erg_table->get_column_name(hyd_label::proferg_polygon) <<" ) ";

	//query_string << " VALUES ( ";
	//query_string << *glob_id << " , " ;
	//query_string << rv_no << " , " ;
	//query_string << this->profile_number << " , " ;
	//query_string << this->system_id.area_state << " , " ;
	//query_string << this->system_id.measure_nr << " , " ;
	//query_string << this->hyd_sz.get_id() << " , " ;
	//query_string << "'" << break_sc << "' , " ;
	////volumes
	//query_string << this->coupling_1d_volume.volume_in << " , " ;
	//query_string << this->coupling_1d_volume.volume_out << " , " ;
	//query_string << this->structure_coupling_volume.volume_in << " , " ;
	//query_string << this->structure_coupling_volume.volume_out << " , " ;

	//query_string << this->left_bank_overflow_volume.volume_in << " , " ;
	//query_string << this->left_bank_overflow_volume.volume_out << " , " ;
	//query_string << this->right_bank_overflow_volume.volume_in << " , " ;
	//query_string << this->right_bank_overflow_volume.volume_out << " , " ;

	//query_string << this->left_dikebreak_coupling_volume.volume_in << " , " ;
	//query_string << this->left_dikebreak_coupling_volume.volume_out << " , " ;
	//query_string << this->right_dikebreak_coupling_volume.volume_in << " , " ;
	//query_string << this->right_dikebreak_coupling_volume.volume_out << " , " ;
	////max values
	//query_string << this->q_value_max.maximum << " , " ;
	//query_string << this->h_max_left_base << " , " ;
	//query_string << this->h_max_right_base << " , " ;
	//query_string << this->max_h_2break_left<< " , " ;
	//query_string << this->max_h_2break_right<< " , " ;
	////max values from profile type
	//query_string << this->typ_of_profile->set_maximum_value2string();
	//query_string << polygon_string <<" ) ";

	//submit it to the datbase
	Data_Base::database_request(&model, query_string.str(), ptr_database);

	if(model.lastError().isValid()){
		//try new glob id
		//renew it
		*glob_id = _Hyd_River_Profile::erg_table->maximum_int_of_column(_Hyd_River_Profile::erg_table->get_column_name(label::glob_id), ptr_database)+1;
		query_string.clear();
		this->generate_max_str2database(&query_string, rv_no, polygon_string, glob_id, break_sc);
		model.clear();
		//second try
		Data_Base::database_request(&model, query_string.str(), ptr_database);
		if (model.lastError().isValid()) {
			Warning msg = this->set_warning(3);
			ostringstream info;
			info << "Table Name                : " << _Hyd_River_Profile::erg_table->get_table_name() << endl;
			info << "Table error info          : " << model.lastError().text().toStdString() << endl;
			info << "Profile number            : " << this->profile_number << endl;
			info << " Two glob ids were checked! " << endl;
			msg.make_second_info(info.str());
			msg.output_msg(2);
		}
		
	}
	*glob_id=(*glob_id)+1;
}
//Output the instationary calculated results to the database table (erg_table)
void _Hyd_River_Profile::output_instat_results(QSqlDatabase *ptr_database, const int rv_no, const string polygon_string, int *glob_id, const string break_sc, const string time) {

	//mysql query with the table_model
	QSqlQueryModel model;
	//the table is set (the name and the column names) and allocated
	try {
		_Hyd_River_Profile::set_erg_instat_table(ptr_database);
	}
	catch (Error msg) {
		throw msg;
	}



	//set the query via a query string
	ostringstream query_string;
	query_string << "INSERT INTO  " << _Hyd_River_Profile::erg_instat_table->get_table_name();
	query_string << " ( ";
	query_string << _Hyd_River_Profile::erg_instat_table->get_column_name(label::glob_id) << " , ";
	query_string << _Hyd_River_Profile::erg_instat_table->get_column_name(hyd_label::profdata_rvno) << " , ";
	query_string << _Hyd_River_Profile::erg_instat_table->get_column_name(label::areastate_id) << " , ";
	query_string << _Hyd_River_Profile::erg_instat_table->get_column_name(label::measure_id) << " , ";
	query_string << _Hyd_River_Profile::erg_instat_table->get_column_name(hyd_label::sz_bound_id) << " , ";
	query_string << _Hyd_River_Profile::erg_instat_table->get_column_name(risk_label::sz_break_id) << " , ";
	//max values
	query_string << _Hyd_River_Profile::erg_instat_table->get_column_name(hyd_label::proferg_q_max) << " , ";
	//max values from profile type
	query_string << _Hyd_River_Profile::erg_instat_table->get_column_name(hyd_label::proferg_h_max) << " , ";
	query_string << _Hyd_River_Profile::erg_instat_table->get_column_name(hyd_label::proferg_s_max) << " , ";
	query_string << _Hyd_River_Profile::erg_instat_table->get_column_name(hyd_label::proferg_v_max) << " , ";
	query_string << _Hyd_River_Profile::erg_instat_table->get_column_name(hyd_label::proferg_width_max) << " , ";
	query_string << _Hyd_River_Profile::erg_instat_table->get_column_name(hyd_label::data_time) << " , ";
	query_string << _Hyd_River_Profile::erg_instat_table->get_column_name(hyd_label::proferg_polygon) << " ) ";

	query_string << " VALUES ( ";
	query_string << *glob_id << " , ";
	query_string << rv_no << " , ";
	query_string << this->system_id.area_state << " , ";
	query_string << this->system_id.measure_nr << " , ";
	query_string << this->hyd_sz.get_id() << " , ";
	query_string << "'" << break_sc << "' , ";
	//max values
	query_string << this->get_Q()<< " , ";
	//max values from profile type
	query_string << this->typ_of_profile->set_instat_value2string();
	query_string << time << " , ";
	query_string << polygon_string << " ) ";

	//submit it to the datbase
	Data_Base::database_request(&model, query_string.str(), ptr_database);

	if (model.lastError().isValid()) {
		Warning msg = this->set_warning(9);
		ostringstream info;
		info << "Table Name                : " << _Hyd_River_Profile::erg_instat_table->get_table_name() << endl;
		info << "Table error info          : " << model.lastError().text().toStdString() << endl;
		info << "Profile number            : " << this->profile_number << endl;
		msg.make_second_info(info.str());
		msg.output_msg(2);
	}
	*glob_id = (*glob_id) + 1;






}
//Check the profiles
void _Hyd_River_Profile::check_profiles(void){
	//check if a init condition is set
	if(this->init_condition<0.0){
		Error msg=this->set_error(5);
		throw msg;
	}
	//check if the init condition is above the outer z coordinates
	if(this->init_condition>this->typ_of_profile->get_first_point()->get_global_z_coordinate()-this->typ_of_profile->get_global_z_min() ||
		this->init_condition>this->typ_of_profile->get_last_point()->get_global_z_coordinate()-this->typ_of_profile->get_global_z_min()){
			//reaction
			//set it to min z-coordinate
		//Daniel: Stop set down!
			//this->init_condition=min((this->typ_of_profile->get_last_point()->get_global_z_coordinate()-this->typ_of_profile->get_global_z_min()), (this->typ_of_profile->get_first_point()->get_global_z_coordinate()-this->typ_of_profile->get_global_z_min()));
			Warning msg=this->set_warning(1);
			msg.output_msg(2);
	}
}
//Output the header for the output of the result members to console/display (normal use for development) (static)
void _Hyd_River_Profile::output_result_members_per_timestep_header(void){
	ostringstream cout;
	cout << "No." << W(5) << "RvSt"<< label::m << W(10);
	cout<< "z_min" << label::m<< W(10);
	cout<< "s" << label::m<< W(10);
	cout << "h" <<label::m<< W(10);
	cout << "v" << label::m_per_sec<<W(10);
	cout << "fr" << label::no_unit <<W(10);
	cout << "width" << label::m<< W(10);
	cout << "q" << label::qm_per_sec;
	cout << endl;
	Sys_Common_Output::output_hyd->output_txt(&cout,true);
}
//Output the result members to console/display (normal use for development)
void _Hyd_River_Profile::output_result_members_per_timestep(ostringstream *cout){
	*cout << this->profile_number;
	*cout << W(10) << P(1) << FORMAT_FIXED_REAL<< this->river_station;
	*cout << W(10) << P(3) << FORMAT_FIXED_REAL;
	this->typ_of_profile->output_result_members_timestep(cout);
}
//Output the result members for each riverprofiletype at every timestep to file
void _Hyd_River_Profile::output_result_members_per_timestep(ofstream *file){
	*file << W(10) << P(2) << FORMAT_FIXED_REAL<< this->river_station;
	this->typ_of_profile->output_result_members_timestep(file);
}
//Output the result members for each riverprofiletype at every timestep to csv file
void _Hyd_River_Profile::output_result_members_per_timestep2csv(ofstream *file) {
	*file << W(10) << P(2) << FORMAT_FIXED_REAL << this->river_station << ",";
	this->typ_of_profile->output_result_members_timestep2csv(file);

}
//Get the stabilization discharge
double _Hyd_River_Profile::get_stabilization_discharge(void){
	return this->typ_of_profile->get_stabilization_discharge();
}
//Calculate the stabilization discharge
void _Hyd_River_Profile::calculate_stabilization_discharge(void){
	this->typ_of_profile->calculate_stabilization_discharge();
}
//Reset the hydrological balance value and the maximum values
void _Hyd_River_Profile::reset_hydrobalance_maxvalues(void){
	this->old_time_point=0.0;
	this->delta_time=0.0;
	//discharges per timestep
	this->q_left_bank=0.0;
	this->q_right_bank=0.0;
	this->q_1d_coupling=0.0;
	this->q_structure_coupling=0.0;
	this->q_dikebreak_coupling_left=0.0;
	this->q_dikebreak_coupling_right=0.0;
	//volumes
	this->left_bank_overflow_volume.volume_in=0.0;
	this->left_bank_overflow_volume.volume_out=0.0;
	this->left_bank_overflow_volume.volume_total=0.0;
	this->right_bank_overflow_volume.volume_in=0.0;
	this->right_bank_overflow_volume.volume_out=0.0;
	this->right_bank_overflow_volume.volume_total=0.0;
	this->coupling_1d_volume.volume_in=0.0;
	this->coupling_1d_volume.volume_out=0.0;
	this->coupling_1d_volume.volume_total=0.0;
	this->structure_coupling_volume.volume_in=0.0;
	this->structure_coupling_volume.volume_out=0.0;
	this->structure_coupling_volume.volume_total=0.0;
	this->left_dikebreak_coupling_volume.volume_in=0.0;
	this->left_dikebreak_coupling_volume.volume_out=0.0;
	this->left_dikebreak_coupling_volume.volume_total=0.0;
	this->right_dikebreak_coupling_volume.volume_in=0.0;
	this->right_dikebreak_coupling_volume.volume_out=0.0;
	this->right_dikebreak_coupling_volume.volume_total=0.0;
	this->q_value_max.maximum=0.0;
	this->q_value_max.time_point=0.0;

	if(this->typ_of_profile!=NULL){
		this->typ_of_profile->reset_result_max_values();
	}

	this->max_h_2break_left=0.0;
	this->max_delta_h_2break_left=0.0;
	this->max_h_2break_right=0.0;
	this->max_delta_h_2break_right=0.0;
}
//Set the pointer to the upstream and downstream profiles
void _Hyd_River_Profile::set_ptr_downstream_upstream_prof(_Hyd_River_Profile *down, _Hyd_River_Profile *up){
	this->ptr_downstream_prof=down;
	this->ptr_upstream_prof=up;
}
//Get the pointer to the downstream profile
_Hyd_River_Profile *_Hyd_River_Profile::get_ptr_downstream_prof(void){
	return this->ptr_downstream_prof;
}
//Get the pointer to the upstream profile
_Hyd_River_Profile *_Hyd_River_Profile::get_ptr_upstream_prof(void){
	return this->ptr_upstream_prof;
}
//Idealise the profile with given parameters of a fpl-section
void _Hyd_River_Profile::idealise_profile_fpl(const bool left_flag, _hyd_idealisation_parameters ideal_param,  const _hyd_connection_types fpl_section_connection, const int section_id){
	//ouput to the fpl-display!!
	ostringstream cout;
	cout << "Idealise river profile " << this->name << " (Id "<<this->profile_number<<")..."<<endl;
	Sys_Common_Output::output_fpl->output_txt(&cout);

	//left bank
	if(left_flag==true){
		//check if it is already idealised
		if(this->left_bank2fpl==true && fpl_section_connection==_hyd_connection_types::STANDARD_CONN){
			Error msg=this->set_error(15);
			ostringstream info;
			info <<"Profile index            : " << this->profile_number<<endl;
			info <<"Profilname               : " << this->name << endl;
			info <<"Riverstation             : " << this->river_station<<label::m << endl;
			info <<"FPL-section index        : " << ideal_param.index << endl;
			info <<"FPL-section name         : " << ideal_param.name << endl;
			msg.make_second_info(info.str());
			throw msg;
		}
		//check if a base point is set
		if(this->index_basepoint_left<0){
			Error msg=this->set_error(17);
			ostringstream info;
			info <<"Profile index            : " << this->profile_number<<endl;
			info <<"Profilname               : " << this->name << endl;
			info <<"Riverstation             : " << this->river_station<<label::m << endl;
			info <<"FPL-section index        : " << ideal_param.index << endl;
			info <<"FPL-section name         : " << ideal_param.name << endl;
			msg.make_second_info(info.str());
			throw msg;
		}
		if(this->break_flag_left==false){
			this->break_flag_left=true;
		}
		if(this->overflow_left_flag==false){
			this->overflow_left_flag=true;
		}
		//check the heights
		if((abs(ideal_param.struct_height-this->height_left_bank)/ideal_param.struct_height)>0.5 && this->system_id.area_state==0
			&& this->system_id.measure_nr==0){
			Warning msg=this->set_warning(5);
			ostringstream info;
			info <<"Profile index            : " << this->profile_number<<endl;
			info <<"Profilname               : " << this->name << endl;
			info <<"Riverstation             : " << this->river_station<<label::m << endl;
			info <<"Left bank height         : " << this->height_left_bank << label::m << endl;
			info <<"FPL-section index        : " << ideal_param.index << endl;
			info <<"FPL-section name         : " << ideal_param.name << endl;
			info <<"Structure height         : " << ideal_param.struct_height <<label::m<< endl;
			msg.make_second_info(info.str());
			//warning to fpl!!
			msg.output_msg(1);
		}
		//set the ideal height
		this->height_left_bank=ideal_param.struct_height;
		if(fpl_section_connection!=_hyd_connection_types::STANDARD_CONN && this->left_bank2fpl==true){
			this->height_left_bank=(this->height_left_bank+ideal_param.struct_height)*0.5;
		}
		else{
			this->height_left_bank=ideal_param.struct_height;
		}

		if(this->connection!=_hyd_connection_types::OUTFLOW_CONN && fpl_section_connection!=_hyd_connection_types::OUTFLOW_CONN){
			if(this->left_break_params==NULL){
				this->allocate_break_parameter_left();
			}
			else{
				//check the breach type
				if(this->left_break_params->abrupt_fails_flag!=ideal_param.break_params.abrupt_fails_flag && this->system_id.area_state==0
					&& this->system_id.measure_nr==0){
					Warning msg=this->set_warning(7);
					ostringstream info;
					info <<"Profile index            : " << this->profile_number<<endl;
					info <<"Profilname               : " << this->name << endl;
					info <<"Riverstation             : " << this->river_station<<label::m << endl;
					info <<"Abrupt breach profile    : " << functions::convert_boolean2string(this->left_break_params->abrupt_fails_flag) << endl;
					info <<"FPL-section index        : " << ideal_param.index << endl;
					info <<"FPL-section name         : " << ideal_param.name << endl;
					info <<"Abrupt breach section    : " << functions::convert_boolean2string(ideal_param.break_params.abrupt_fails_flag) << endl;
					msg.make_second_info(info.str());
					msg.output_msg(1);
				}
			}
			//set the parameters left bank
			*this->left_break_params=ideal_param.break_params;
		}
		this->left_bank2fpl=true;
		if(fpl_section_connection != _hyd_connection_types::OUTFLOW_CONN){
			this->fpl_sec_id_left=section_id;
		}
	}
	//right bank
	else{
		//check if it is already idealised
		if(this->right_bank2fpl==true && fpl_section_connection==_hyd_connection_types::STANDARD_CONN){
			Error msg=this->set_error(16);
			ostringstream info;
			info <<"Profile index            : " << this->profile_number<<endl;
			info <<"Profilname               : " << this->name << endl;
			info <<"Riverstation             : " << this->river_station<<label::m << endl;
			info <<"FPL-section index        : " << ideal_param.index << endl;
			info <<"FPL-section name         : " << ideal_param.name << endl;
			msg.make_second_info(info.str());
			throw msg;
		}
		//check if a base point is set
		if(this->index_basepoint_right<0){
			Error msg=this->set_error(18);
			ostringstream info;
			info <<"Profile index            : " << this->profile_number<<endl;
			info <<"Profilname               : " << this->name << endl;
			info <<"Riverstation             : " << this->river_station<<label::m << endl;
			info <<"FPL-section index        : " << ideal_param.index << endl;
			info <<"FPL-section name         : " << ideal_param.name << endl;
			msg.make_second_info(info.str());
			throw msg;
		}
		//set breakflag to true
		if(this->break_flag_right==false){
			this->break_flag_right=true;
		}
		if(this->overflow_right_flag==false){
			this->overflow_right_flag=true;
		}
		//check the heights
		if((abs(ideal_param.struct_height-this->height_right_bank)/ideal_param.struct_height)>0.5 && this->system_id.area_state==0
			&& this->system_id.measure_nr==0){
			Warning msg=this->set_warning(6);
			ostringstream info;
			info <<"Profile index            : " << this->profile_number<<endl;
			info <<"Profilname               : " << this->name << endl;
			info <<"Riverstation             : " << this->river_station<<label::m << endl;
			info <<"Left bank height         : " << this->height_right_bank << label::m << endl;
			info <<"FPL-section index        : " << ideal_param.index << endl;
			info <<"FPL-section name         : " << ideal_param.name << endl;
			info <<"Structure height         : " << ideal_param.struct_height <<label::m<< endl;
			msg.make_second_info(info.str());
			msg.output_msg(1);
		}
		//set the ideal height
		this->height_left_bank=ideal_param.struct_height;
		if(fpl_section_connection!=_hyd_connection_types::STANDARD_CONN && this->left_bank2fpl==true){
			this->height_right_bank=(this->height_left_bank+ideal_param.struct_height)*0.5;
		}
		else{
			this->height_right_bank=ideal_param.struct_height;
		}

		if(this->connection!=_hyd_connection_types::OUTFLOW_CONN && fpl_section_connection!=_hyd_connection_types::OUTFLOW_CONN){
			if(this->right_break_params==NULL){
				this->allocate_break_parameter_right();
			}
			else{
				//check the breach type
				if(this->right_break_params->abrupt_fails_flag!=ideal_param.break_params.abrupt_fails_flag && this->system_id.area_state==0
						&& this->system_id.measure_nr==0){
					Warning msg=this->set_warning(8);
					ostringstream info;
					info <<"Profile index            : " << this->profile_number<<endl;
					info <<"Profilname               : " << this->name << endl;
					info <<"Riverstation             : " << this->river_station<<label::m << endl;
					info <<"Abrupt breach profile    : " << functions::convert_boolean2string(this->right_break_params->abrupt_fails_flag) << endl;
					info <<"FPL-section index        : " << ideal_param.index << endl;
					info <<"FPL-section name         : " << ideal_param.name << endl;
					info <<"Abrupt breach section    : " << functions::convert_boolean2string(ideal_param.break_params.abrupt_fails_flag) << endl;
					msg.make_second_info(info.str());
					msg.output_msg(1);
				}
			}
			//set the parameters right bank
			*this->right_break_params=ideal_param.break_params;
		}

		this->right_bank2fpl=true;
		if(fpl_section_connection != _hyd_connection_types::OUTFLOW_CONN){
			this->fpl_sec_id_right=section_id;
		}
	}
}
//Set the waterlevel difference between river profile and connected floodplain element for a break and calculate the maximum one
void _Hyd_River_Profile::set_delta_waterlevel_rv2fp2break(const double waterlevel, const double delta_h, const bool left_flag){
	if(left_flag==true){
		if(delta_h>this->max_delta_h_2break_left && waterlevel-this->typ_of_profile->points[this->index_basepoint_left].get_global_z_coordinate()>0.0){
			this->max_delta_h_2break_left=delta_h;
			this->max_h_2break_left=waterlevel-this->typ_of_profile->points[this->index_basepoint_left].get_global_z_coordinate();
		}
	}
	else{
		if(delta_h>this->max_delta_h_2break_right && waterlevel-this->typ_of_profile->points[this->index_basepoint_right].get_global_z_coordinate()>0.0){
			this->max_delta_h_2break_right=delta_h;
			this->max_h_2break_right=waterlevel-this->typ_of_profile->points[this->index_basepoint_right].get_global_z_coordinate();
		}
	}
}
//convert string to profile type (static)
_hyd_profile_types _Hyd_River_Profile::convert_string2profiletype(const string str_profiletype){
	_hyd_profile_types type;
	string buffer=str_profiletype;

	functions::clean_string(&buffer);
	_Hyd_Parse_IO::string2lower(&buffer);

	if(buffer==hyd_label::river_type){
		type=_hyd_profile_types::RIVER_TYPE;
	}
	else if(buffer==hyd_label::weir_type){
		type=_hyd_profile_types::WEIR_TYPE;
	}
	else if(buffer==hyd_label::bridge_type){
		type=_hyd_profile_types::BRIDGE_TYPE;
	}
	else{
		Error msg;
		msg.set_msg("_hyd_profile_types _Hyd_River_Profile::convert_string2profiletype(string str_profiletype)", "Can not convert this profile type", "Check the given type", 1, false);
		ostringstream info;
		info << "Try to convert profile type: " << str_profiletype << endl;
		info << "Possible profile types: "<< endl;
		info << " "<<hyd_label::river_type  << endl;
		info << " "<<hyd_label::weir_type  << endl;
		info << " "<<hyd_label::bridge_type  << endl;
		msg.make_second_info(info.str());
		throw msg;
	}

	return type;
}
//Convert a enum _hyd_profile_types to a string (static)
string _Hyd_River_Profile::convert_profiletype2string(const _hyd_profile_types profiletype){
	string buffer;
	switch(profiletype){
		case _hyd_profile_types::RIVER_TYPE:
			buffer=hyd_label::river_type;
			break;
		case _hyd_profile_types::BRIDGE_TYPE:
			buffer=hyd_label::bridge_type;
			break;
		case _hyd_profile_types::WEIR_TYPE:
			buffer=hyd_label::weir_type;
			break;
		default:
			buffer=label::unknown_type;
	}
	return buffer;
}
//Convert string to connection type (static)
_hyd_connection_types _Hyd_River_Profile::convert_string2connectiontype(const string str_connectiontype){
	_hyd_connection_types buffer;
	string txt=str_connectiontype;
	functions::clean_string(&txt);
	_Hyd_Parse_IO::string2lower(&txt);

	if(txt==hyd_label::standard_conn){
		buffer=_hyd_connection_types::STANDARD_CONN;
	}
	else if(txt==hyd_label::inflow_conn){
		buffer=_hyd_connection_types::INFLOW_CONN;
	}
	else if(txt==hyd_label::outflow_conn){
		buffer=_hyd_connection_types::OUTFLOW_CONN;
	}
	else{
		Error msg;
		msg.set_msg("_hyd_connection_types _Hyd_River_Profile::convert_string2connectiontype(string str_connectiontype)", "Can not convert this connection type", "Check the given type", 1, false);
		ostringstream info;
		info << "Try to convert connection type: " << str_connectiontype << endl;
		info << "Possible connection types: "<< endl;
		info << " "<<hyd_label::inflow_conn  << endl;
		info << " "<<hyd_label::standard_conn  << endl;
		info << " "<<hyd_label::outflow_conn  << endl;
		msg.make_second_info(info.str());
		throw msg;
	}

	return buffer;
}
//Convert enum _hyd_connection_types to string (static)
string _Hyd_River_Profile::convert_connectiontype2string(const _hyd_connection_types type){
	string buffer;
	switch(type){
		case _hyd_connection_types::STANDARD_CONN:
			buffer=hyd_label::standard_conn;
			break;
		case _hyd_connection_types::INFLOW_CONN:
			buffer=hyd_label::inflow_conn;
			break;
		case _hyd_connection_types::OUTFLOW_CONN:
			buffer=hyd_label::outflow_conn;
			break;
		default:
			buffer=label::unknown_type;
	}
	return buffer;
}
//Get the fpl-section id of the left bank
int _Hyd_River_Profile::get_fpl_section_id_left(void){
	return this->fpl_sec_id_left;
}
//Get the fpl-section id of the right bank
int _Hyd_River_Profile::get_fpl_section_id_right(void){
	return this->fpl_sec_id_right;
}
//Get the global profile id in database
int _Hyd_River_Profile::get_prof_glob_id(void){
	return this->glob_prof_id;
}
//Append a string to the profile name
void _Hyd_River_Profile::append_str2name(const string append){
	this->name.append(append);
}
//_________________________________
//protected
//Allocate the data structure for the break-parameters of the left bank
void _Hyd_River_Profile::allocate_break_parameter_left(void){
	if(this->left_break_params==NULL){
		try{
			this->left_break_params=new _hyd_break_parameters;
		}
		catch(bad_alloc &t){
			Error msg=this->set_error(19);
			ostringstream info;
			info<< "Info bad alloc: " << t.what() << endl;
			msg.make_second_info(info.str());
			throw msg;
		}
		this->left_break_params->abrupt_opening=0.0;
		this->left_break_params->resistance=1e10;
		this->left_break_params->abrupt_fails_flag=false;
	}
}
//Delete the break-parameters of the left bank; it is not required for an outflow profile
void _Hyd_River_Profile::delete_break_parameter_left(void){
		if(this->left_break_params!=NULL){
		delete this->left_break_params;
		this->left_break_params=NULL;
	}
}
//Allocate the data structure for the break-parameters of the right bank
void _Hyd_River_Profile::allocate_break_parameter_right(void){
	if(this->right_break_params==NULL){
		try{
			this->right_break_params=new _hyd_break_parameters;
		}
		catch(bad_alloc &t){
			Error msg=this->set_error(19);
			ostringstream info;
			info<< "Info bad alloc: " << t.what() << endl;
			msg.make_second_info(info.str());
			throw msg;
		}

		this->right_break_params->abrupt_opening=0.0;
		this->right_break_params->resistance=1e10;
		this->right_break_params->abrupt_fails_flag=false;
	}
}
//Delete the break-parameters of the right bank; it is not required for an outflow profile
void _Hyd_River_Profile::delete_break_parameter_right(void){
	if(this->right_break_params!=NULL){
		delete this->right_break_params;
		this->right_break_params=NULL;
	}
}
//Allocate the bridge specific data for input
void _Hyd_River_Profile::allocate_bridge_data(void){
	if(this->bridge_specific_value==NULL){
		try{
				this->bridge_specific_value=new _hyd_bridge_values;
			}
			catch(bad_alloc&t){
				Error msg=this->set_error(7);
				ostringstream info;
				info<< "Info bad alloc: " << t.what() << endl;
				msg.make_second_info(info.str());
				throw msg;
		}
	}
}
//decide in the different keyvalues in the profile file
void _Hyd_River_Profile::decide_keyvalues_file(const string key, string buffer, int *found_counter){
	stringstream stream;
	//general key words
	if(key==hyd_label::InitCondition){
		this->get_keyvalues_file(&stream, buffer);
		stream >> this->init_condition;
		(*found_counter)++;
	}
	else if(key==hyd_label::ProfLDist){
		this->get_keyvalues_file(&stream, buffer);
		stream >> this->river_station;
		(*found_counter)++;
	}
	else if(key==hyd_label::ConnectionType){
		this->get_keyvalues_file(&stream, buffer);
		try{
			this->connection=this->convert_string2connectiontype(stream.str());
			(*found_counter)++;
			this->check_connection_type_profile();
			if(this->connection==_hyd_connection_types::OUTFLOW_CONN && this->break_flag_left==true){
				this->must_found_number--;
			}
			if(this->connection==_hyd_connection_types::OUTFLOW_CONN && this->break_flag_right==true){
				this->must_found_number--;
			}
		}
		catch(Error msg){
			throw msg;
		}
	}
	else if(key==hyd_label::DeltaXtable){
		this->get_keyvalues_file(&stream, buffer);
		stream >> this->delta_x_table;
	}
	else if(key==hyd_label::ProfType){
		this->get_keyvalues_file(&stream, buffer);
		(*found_counter)++;
		try{
			this->type=this->convert_string2profiletype(stream.str());
		}
		catch(Error msg){
			throw msg;
		}
		if(this->type==_hyd_profile_types::BRIDGE_TYPE){
			this->must_found_number=this->must_found_number+2;
		}
	}

	else if(key==hyd_label::BasePointRight){
		this->get_keyvalues_file(&stream, buffer);
		stream >> this->index_basepoint_right;
		this->break_flag_right=true;
		if(this->connection!= _hyd_connection_types::OUTFLOW_CONN){
			this->must_found_number++;
		}
	}
	else if(key==hyd_label::BasePointLeft){
		this->get_keyvalues_file(&stream, buffer);
		stream >> this->index_basepoint_left;
		this->break_flag_left=true;
		if(this->connection!= _hyd_connection_types::OUTFLOW_CONN){
			this->must_found_number++;
		}
	}
	else if(key==hyd_label::BridgeBodySize && this->type==_hyd_profile_types::BRIDGE_TYPE){
		this->allocate_bridge_data();
		this->get_keyvalues_file(&stream, buffer);
		stream >> this->bridge_specific_value->bridgebody_size;
		(*found_counter)++;
	}
	else if(key==hyd_label::LocalBridgeHeight && this->type==_hyd_profile_types::BRIDGE_TYPE){
		this->allocate_bridge_data();
		this->get_keyvalues_file(&stream, buffer);
		stream >> this->bridge_specific_value->local_bridge_height;
		(*found_counter)++;
	}

	if(stream.fail()==true){
		Error msg=this->set_error(3);
		ostringstream info;
		info << "Searched key word " << key << endl;
		info << "Wrong input sequenze " << stream.str() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
}
//decide and allocate the profile type
void _Hyd_River_Profile::decide_alloc_profile_type(const int number_points){
	try{
		//decide which profile type it is
		switch(this->type){
			case _hyd_profile_types::RIVER_TYPE:
				this->typ_of_profile=new Hyd_River_Profile_Type_Standard;
				this->typ_of_profile->set_number_points(number_points);
				break;
			case _hyd_profile_types::WEIR_TYPE:
				this->typ_of_profile=new Hyd_River_Profile_Type_Weir;
				this->typ_of_profile->set_number_points(number_points);
				break;
			case _hyd_profile_types::BRIDGE_TYPE:
				this->typ_of_profile=new Hyd_River_Profile_Type_Bridge;
				this->typ_of_profile->set_number_points(number_points);
				//set here the size of the bridge body
				this->typ_of_profile->set_profile_specific_value(*this->bridge_specific_value);
				break;
			default:
				break;
		}
	}
	catch(bad_alloc &t){
		Error msg=this->set_error(0);
		ostringstream info;
		info<< "Info bad alloc: " << t.what() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
	catch(Error msg){
		throw msg;
	}
}
//Delete the profile type
void _Hyd_River_Profile::delete_profile_type(void){
	if(this->typ_of_profile!=NULL){
		delete this->typ_of_profile;
		this->typ_of_profile=NULL;
	}
}
//get the different keyvalues in the profile file
void _Hyd_River_Profile::get_keyvalues_file(stringstream *stream, string buffer){
	int pos=-1;
	pos=buffer.find("\"",0);
	if(pos<0){
		Error msg=this->set_error(11);
		throw msg;
	}
	buffer=buffer.erase(0,pos+1);
	pos=buffer.find("\"",0);
	if(pos<0){
		Error msg=this->set_error(11);
		throw msg;
	}
	pos=buffer.find_last_of("\"",buffer.length()-1);
	if(buffer.empty()==true){
		Error msg=this->set_error(11);
		throw msg;
	}
	buffer=buffer.erase(pos,buffer.length()-1);
	_Hyd_Parse_IO::erase_carriageReturn(&buffer);
	_Hyd_Parse_IO::erase_leading_whitespace_tabs(&buffer);
	_Hyd_Parse_IO::erase_end_whitespace_tabs(&buffer);
	*stream << buffer.substr(0,pos);
}
//Input the river profile point data from a database table
void _Hyd_River_Profile::input_profile_points_per_database(QSqlDatabase *ptr_database, const int glob_prof_id){
	int number_points=0;
	QSqlQueryModel query_result;

	//set number of profile points
	number_points=_Hyd_River_Profile_Type::select_profile_points_in_database(&query_result, ptr_database, glob_prof_id);

	//allocate the profile type
	this->decide_alloc_profile_type(number_points);

	//input the profile points
	this->typ_of_profile->input_members(&query_result, this->name, this->profile_number);

}
//Calculate the watervolume in the element
void _Hyd_River_Profile::calculate_watervolume_element(void){
	this->watervolume=0.0;
}
//Calculate the maximum waterlevel above the left/right basepoints of a profile
void _Hyd_River_Profile::calculate_max_h2basepoint(void){
	this->h_max_left_base=this->typ_of_profile->get_hmax2related_profile_point(this->index_basepoint_left);
	this->h_max_right_base=this->typ_of_profile->get_hmax2related_profile_point(this->index_basepoint_right);
	

}
//Check the given basepoints and set the height of the river banks
void _Hyd_River_Profile::check_set_base_points(void){
	//check the base points
	if(this->index_basepoint_left==0){
		this->break_flag_left=false;
	}
	if(this->index_basepoint_right==this->typ_of_profile->get_number_points()-1){
		this->break_flag_right=false;
	}
	//check left one
	if(this->break_flag_left==true){
		if(this->index_basepoint_left>=this->typ_of_profile->get_number_points() || this->index_basepoint_left<0){
			ostringstream info;
			Error msg=this->set_error(9);
			info <<"Left basepoint index     : " << this->index_basepoint_left << endl;
			info <<"Number of profile points : " << this->typ_of_profile->get_number_points() << endl;
			info <<"Profile index            : " << this->profile_number<<endl;
			info <<"Profilname               : " << this->name << endl;
			msg.make_second_info(info.str());
			throw msg;
		}
	}
	//check right one
	if(this->break_flag_right==true){
		if(this->index_basepoint_right>=this->typ_of_profile->get_number_points() || this->index_basepoint_right<0){
			ostringstream info;
			Error msg=this->set_error(9);
			info <<"Right basepoint index    : " << this->index_basepoint_right << endl;
			info <<"Number of profile points : " << this->typ_of_profile->get_number_points() << endl;
			info <<"Profile index            : " << this->profile_number<<endl;
			info <<"Profilname               : " << this->name << endl;
			msg.make_second_info(info.str());
			throw msg;
		}
	}
	//check both
	if(this->break_flag_left==true && this->break_flag_right==true){
		if(this->index_basepoint_left>this->index_basepoint_right){
			ostringstream info;
			Error msg=this->set_error(8);
			info <<"Left basepoint index  : " << this->index_basepoint_left << endl;
			info <<"Right basepoint index : " << this->index_basepoint_right << endl;
			info <<"Profile index         : " << this->profile_number<<endl;
			info <<"Profilname            : " << this->name << endl;
			msg.make_second_info(info.str());
			throw msg;
		}
	}

	//set the height
	if(this->break_flag_left==true){
		this->height_left_bank=this->typ_of_profile->get_first_point()->get_global_z_coordinate()-
			this->typ_of_profile->points[this->index_basepoint_left].get_global_z_coordinate();
		if(this->height_left_bank<=0.0){
			ostringstream info;
			Error msg=this->set_error(14);
			info <<"Left basepoint index  : " << this->index_basepoint_left << endl;
			info <<"Left river bank height: " << this->height_left_bank<<label::m << endl;
			info <<"Profile index         : " << this->profile_number<<endl;
			info <<"Profilname            : " << this->name << endl;
			msg.make_second_info(info.str());
			throw msg;
		}
	}
	if(this->break_flag_right==true){
		this->height_right_bank=this->typ_of_profile->get_last_point()->get_global_z_coordinate()-
			this->typ_of_profile->points[this->index_basepoint_right].get_global_z_coordinate();
		if(this->height_right_bank<=0.0){
			ostringstream info;
			Error msg=this->set_error(14);
			info <<"Right basepoint index  : " << this->index_basepoint_right << endl;
			info <<"Right river bank height: " << this->height_right_bank<<label::m << endl;
			info <<"Profile index          : " << this->profile_number<<endl;
			info <<"Profilname             : " << this->name << endl;
			msg.make_second_info(info.str());
			throw msg;
		}
	}

	//set base points to first last point in case of nothing is set
	if(this->index_basepoint_left<0){
		this->index_basepoint_left=0;
	}
	if(this->index_basepoint_right<0){
		this->index_basepoint_right=this->typ_of_profile->get_number_points()-1;
	}
}
//Get boundary point condition is applied
bool _Hyd_River_Profile::boundary_point_is_applied(void) {
	return false;

}
//Get boundary point value
double _Hyd_River_Profile::get_boundary_point_value(const double time) {
	return 0.0;

}
//________________________________
//private
//Delete the bridge specific data after transferring it to the profile type
void _Hyd_River_Profile::delete_bridge_data(void){
	if(this->bridge_specific_value!=NULL){
		delete this->bridge_specific_value;
		this->bridge_specific_value=NULL;
	}
}
//Calculate the river width
void _Hyd_River_Profile::calculate_river_width(void) {
	this->river_width = this->typ_of_profile->get_first_point()->distance(this->typ_of_profile->get_last_point());
}
//Generate string for output max-results to database
void _Hyd_River_Profile::generate_max_str2database(ostringstream *query_string, const int rv_no, const string polygon_string, int *glob_id, const string break_sc) {
	*query_string << "INSERT INTO  " << _Hyd_River_Profile::erg_table->get_table_name();
	*query_string << " ( ";
	*query_string << _Hyd_River_Profile::erg_table->get_column_name(label::glob_id) << " , ";
	*query_string << _Hyd_River_Profile::erg_table->get_column_name(hyd_label::profdata_rvno) << " , ";
	*query_string << _Hyd_River_Profile::erg_table->get_column_name(hyd_label::profdata_prof_id) << " , ";
	*query_string << _Hyd_River_Profile::erg_table->get_column_name(label::areastate_id) << " , ";
	*query_string << _Hyd_River_Profile::erg_table->get_column_name(label::measure_id) << " , ";
	*query_string << _Hyd_River_Profile::erg_table->get_column_name(hyd_label::sz_bound_id) << " , ";
	*query_string << _Hyd_River_Profile::erg_table->get_column_name(risk_label::sz_break_id) << " , ";
	//volumes
	*query_string << _Hyd_River_Profile::erg_table->get_column_name(hyd_label::proferg_cv_rv_in) << " , ";
	*query_string << _Hyd_River_Profile::erg_table->get_column_name(hyd_label::proferg_cv_rv_out) << " , ";
	*query_string << _Hyd_River_Profile::erg_table->get_column_name(hyd_label::proferg_cv_struc_in) << " , ";
	*query_string << _Hyd_River_Profile::erg_table->get_column_name(hyd_label::proferg_cv_struc_out) << " , ";

	*query_string << _Hyd_River_Profile::erg_table->get_column_name(hyd_label::proferg_cv_l_ov_in) << " , ";
	*query_string << _Hyd_River_Profile::erg_table->get_column_name(hyd_label::proferg_cv_l_ov_out) << " , ";
	*query_string << _Hyd_River_Profile::erg_table->get_column_name(hyd_label::proferg_cv_r_ov_in) << " , ";
	*query_string << _Hyd_River_Profile::erg_table->get_column_name(hyd_label::proferg_cv_r_ov_out) << " , ";

	*query_string << _Hyd_River_Profile::erg_table->get_column_name(hyd_label::proferg_cv_l_db_in) << " , ";
	*query_string << _Hyd_River_Profile::erg_table->get_column_name(hyd_label::proferg_cv_l_db_out) << " , ";
	*query_string << _Hyd_River_Profile::erg_table->get_column_name(hyd_label::proferg_cv_r_db_in) << " , ";
	*query_string << _Hyd_River_Profile::erg_table->get_column_name(hyd_label::proferg_cv_r_db_out) << " , ";
	//max values
	*query_string << _Hyd_River_Profile::erg_table->get_column_name(hyd_label::proferg_q_max) << " , ";
	*query_string << _Hyd_River_Profile::erg_table->get_column_name(hyd_label::proferg_hmax_lb) << " , ";
	*query_string << _Hyd_River_Profile::erg_table->get_column_name(hyd_label::proferg_hmax_rb) << " , ";
	*query_string << _Hyd_River_Profile::erg_table->get_column_name(hyd_label::proferg_hmax_lb_break) << " , ";
	*query_string << _Hyd_River_Profile::erg_table->get_column_name(hyd_label::proferg_hmax_rb_break) << " , ";
	//max values from profile type
	*query_string << _Hyd_River_Profile::erg_table->get_column_name(hyd_label::proferg_h_max) << " , ";
	*query_string << _Hyd_River_Profile::erg_table->get_column_name(hyd_label::proferg_s_max) << " , ";
	*query_string << _Hyd_River_Profile::erg_table->get_column_name(hyd_label::proferg_t_hmax) << " , ";
	*query_string << _Hyd_River_Profile::erg_table->get_column_name(hyd_label::proferg_v_max) << " , ";
	*query_string << _Hyd_River_Profile::erg_table->get_column_name(hyd_label::proferg_width_max) << " , ";
	*query_string << _Hyd_River_Profile::erg_table->get_column_name(hyd_label::proferg_dur_wet) << " , ";
	*query_string << _Hyd_River_Profile::erg_table->get_column_name(hyd_label::proferg_dur_dry) << " , ";
	*query_string << _Hyd_River_Profile::erg_table->get_column_name(hyd_label::proferg_polygon) << " ) ";

	*query_string << " VALUES ( ";
	*query_string << *glob_id << " , ";
	*query_string << rv_no << " , ";
	*query_string << this->profile_number << " , ";
	*query_string << this->system_id.area_state << " , ";
	*query_string << this->system_id.measure_nr << " , ";
	*query_string << this->hyd_sz.get_id() << " , ";
	*query_string << "'" << break_sc << "' , ";
	//volumes
	*query_string << this->coupling_1d_volume.volume_in << " , ";
	*query_string << this->coupling_1d_volume.volume_out << " , ";
	*query_string << this->structure_coupling_volume.volume_in << " , ";
	*query_string << this->structure_coupling_volume.volume_out << " , ";

	*query_string << this->left_bank_overflow_volume.volume_in << " , ";
	*query_string << this->left_bank_overflow_volume.volume_out << " , ";
	*query_string << this->right_bank_overflow_volume.volume_in << " , ";
	*query_string << this->right_bank_overflow_volume.volume_out << " , ";

	*query_string << this->left_dikebreak_coupling_volume.volume_in << " , ";
	*query_string << this->left_dikebreak_coupling_volume.volume_out << " , ";
	*query_string << this->right_dikebreak_coupling_volume.volume_in << " , ";
	*query_string << this->right_dikebreak_coupling_volume.volume_out << " , ";
	//max values
	*query_string << this->q_value_max.maximum << " , ";
	*query_string << this->h_max_left_base << " , ";
	*query_string << this->h_max_right_base << " , ";
	*query_string << this->max_h_2break_left << " , ";
	*query_string << this->max_h_2break_right << " , ";
	//max values from profile type
	*query_string << this->typ_of_profile->set_maximum_value2string();
	*query_string << polygon_string << " ) ";




}
//Delete all data in the database table for the river profile data (static)
void _Hyd_River_Profile::delete_data_in_profile_table(QSqlDatabase *ptr_database){
	//the table is set (the name and the column names) and allocated
	try{
		_Hyd_River_Profile::set_profile_table(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}
	//delete the table
	_Hyd_River_Profile::profile_table->delete_data_in_table(ptr_database);
}
//Close and delete the database table for the river profile data (static)
void _Hyd_River_Profile::close_profile_table(void){
	if(_Hyd_River_Profile::profile_table!=NULL){
		delete _Hyd_River_Profile::profile_table;
		_Hyd_River_Profile::profile_table=NULL;
	}
}
//Delete all data in the database table for the boundary profile data (static)
void _Hyd_River_Profile::delete_data_in_boundary_table(QSqlDatabase *ptr_database){
	//the table is set (the name and the column names) and allocated
	try{
		_Hyd_River_Profile::set_profile_boundary_table(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}
	//delete the table
	_Hyd_River_Profile::boundary_table->delete_data_in_table(ptr_database);
}
//Close and delete the database table for the boundary profile data (static)
void _Hyd_River_Profile::close_boundary_table(void){
	if(_Hyd_River_Profile::boundary_table!=NULL){
		delete _Hyd_River_Profile::boundary_table;
		_Hyd_River_Profile::boundary_table=NULL;
	}
}
//Delete all data in the database table for the wallbreak profile data (static)
void _Hyd_River_Profile::delete_data_in_wallbreak_table(QSqlDatabase *ptr_database){
	//the table is set (the name and the column names) and allocated
	try{
		_Hyd_River_Profile::set_profile_wallbreak_table(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}
	//delete the table
	_Hyd_River_Profile::wallbreak_table->delete_data_in_table(ptr_database);
}
//Close and delete the database table for the wallbreak profile data (static)
void _Hyd_River_Profile::close_wallbreak_table(void){
	if(_Hyd_River_Profile::wallbreak_table!=NULL){
		delete _Hyd_River_Profile::wallbreak_table;
		_Hyd_River_Profile::wallbreak_table=NULL;
	}
}
//Delete all data in the database table for the dikebreak profile data (static)
void _Hyd_River_Profile::delete_data_in_dikebreak_table(QSqlDatabase *ptr_database){
	//the table is set (the name and the column names) and allocated
	try{
		_Hyd_River_Profile::set_profile_dikebreak_table(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}
	//delete the table
	_Hyd_River_Profile::dikebreak_table->delete_data_in_table(ptr_database);
}
//Close and delete the database table for the dikebreak profile data (static)
void _Hyd_River_Profile::close_dikebreak_table(void){
	if(_Hyd_River_Profile::dikebreak_table!=NULL){
		delete _Hyd_River_Profile::dikebreak_table;
		_Hyd_River_Profile::dikebreak_table=NULL;
	}
}
//Delete all data in the database table for the bridge profile data (static)
void _Hyd_River_Profile::delete_data_in_bridge_table(QSqlDatabase *ptr_database){
	//the table is set (the name and the column names) and allocated
	try{
		_Hyd_River_Profile::set_profile_bridge_table(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}
	//delete the table
	_Hyd_River_Profile::bridge_table->delete_data_in_table(ptr_database);
}
//Close and delete the database table for the bridge profile data (static)
void _Hyd_River_Profile::close_bridge_table(void){
	if(_Hyd_River_Profile::bridge_table!=NULL){
		delete _Hyd_River_Profile::bridge_table;
		_Hyd_River_Profile::bridge_table=NULL;
	}
}
//Delete all data in the database table for the results of an hydraulic simulation the river profiles (static)
void _Hyd_River_Profile::delete_data_in_erg_table(QSqlDatabase *ptr_database){
	//the table is set (the name and the column names) and allocated
	try{
		_Hyd_River_Profile::set_erg_table(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}
	//delete the table
	_Hyd_River_Profile::erg_table->delete_data_in_table(ptr_database);
}
//Delete all data in the database table for the instationary results of an hydraulic simulation the river profiles
void _Hyd_River_Profile::delete_data_in_erg_instat_table(QSqlDatabase *ptr_database) {
	//the table is set (the name and the column names) and allocated
	try {
		_Hyd_River_Profile::set_erg_instat_table(ptr_database);
	}
	catch (Error msg) {
		throw msg;
	}
	//delete the table
	_Hyd_River_Profile::erg_instat_table->delete_data_in_table(ptr_database);

}
//Delete the maximum result row in a database table for this profile
void _Hyd_River_Profile::delete_max_result_row_in_table(QSqlDatabase *ptr_database, const int rv_no, const string break_sc){
	//the table is set (the name and the column names) and allocated
	try{
		_Hyd_River_Profile::set_erg_table(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}

	QSqlQuery query(*ptr_database);
	ostringstream query_string;
	query_string << "DELETE FROM " << _Hyd_River_Profile::erg_table->get_table_name() << " ";
	query_string << " WHERE " << _Hyd_River_Profile::erg_table->get_column_name(hyd_label::profdata_rvno) << " = " << rv_no;
	query_string << " AND ";
	query_string << _Hyd_River_Profile::erg_table->get_column_name(hyd_label::profdata_prof_id) << " = " << this->profile_number;
	query_string << " AND ";
	query_string << _Hyd_River_Profile::erg_table->get_column_name(label::areastate_id) << " = " << this->system_id.area_state;
	query_string << " AND ";
	query_string << _Hyd_River_Profile::erg_table->get_column_name(label::measure_id) << " = " << this->system_id.measure_nr;
	query_string << " AND ";
	query_string << _Hyd_River_Profile::erg_table->get_column_name(hyd_label::sz_bound_id) << " = " << this->hyd_sz.get_id();
	query_string << " AND ";
	query_string << _Hyd_River_Profile::erg_table->get_column_name(risk_label::sz_break_id) << " = '" << break_sc<<"'";

	query.exec(query_string.str().c_str());
}

//set the error
Error _Hyd_River_Profile::set_error(const int err_type){
		string place="_Hyd_River_Profile::";
		string help;
		string reason;
		int type=0;
		bool fatal=false;
		stringstream info;
		Error msg;

	switch (err_type){
		case 0://bad alloc
			place.append("decide_alloc_profile_type(void)");
			reason="Can not allocate the memory";
			help="Check the memory";
			type=10;
			break;
		case 2://not all needed values are found
			place.append("input_members(ifstream *profile_file, const long int file_position, const int number_profile_points)");
			reason="Not all needed key values are found";
			help="Check the Profile in the profile file;";
			info << "Profile type " << this->convert_profiletype2string(this->type) << endl;
			type=14;
			break;
		case 3://wrong input
			place.append("decide_keyvalues_file(const string key, string buffer, int *found_counter)");
			reason="There is a problem with the input; wrong sign is read in";
			help="Check the Profile in the profile file";
			type=1;
			break;
		case 5://init connditions are dry
			place.append("check_profiles(_Hyd_River_Profile *upstream_profile, _Hyd_River_Profile *downstream_profile)");
			reason="The initial conditions of a river profile can not be below 0.0";
			help="Check the Profile in the profile file";
			type=14;
			break;
		case 6://problems with the number of points
			place.append("input_members(ifstream *profile_file, const int profile_number, string my_line)");
			reason="Can not read in properly the number of points of the profiles";
			help="Check number of point in the inputfile";
			type=1;
			break;
		case 7://bad alloc
			place.append("allocate_bridge_data(void)");
			reason="Can not allocate the memory for the bridge specific data for an input";
			help="Check the memory";
			type=10;
			break;
		case 8://base_point index left is greater than the right on
			place.append("check_set_base_points(_Hyd_River_Profile *upstream_profile, _Hyd_River_Profile *downstream_profile)");
			reason="The left basepoint index is greater to the right one";
			help="Check the given basepoints";
			type=14;
			break;
		case 9://base_points indices are greater  than the number of profile points
			place.append("check_set_base_points(_Hyd_River_Profile *upstream_profile, _Hyd_River_Profile *downstream_profile)");
			reason="The given basepoint index is greater than the number of profile points or is below 0";
			help="Check the given basepoint";
			type=14;
			break;
		case 10://the distance to the upstream profile is below 0.0
			place.append("set_distance2upstream(const double riverstation_upstream)");
			reason="The distance to the upstream profile is below 0.0; the riverstation must increase from downstream to upstream";
			help="Check the riverstation of this profile and the upstream profile";
			type=15;
			break;
		case 11://the keyvalues are not compatible
			place.append("get_keyvalues_file(stringstream *stream, string buffer)");
			reason="There are problems to read in the keyvalues";
			help="Check the the profile keyvalues; it has to look like: key=\"value\"";
			type=1;
			break;
		case 12://no matching global id set is found for the given river profile
			place.append("transfer_hydraulic_boundary_sz2database(QSqlDatabase *ptr_database, const int rv_number)") ;
			reason="There is no matching global id to the given river profile";
			help="Check the river profile table";
			type=2;
			break;
		case 13://problems with database query
			place.append("transfer_hydraulic_boundary_sz2database(QSqlDatabase *ptr_database, const int rv_number)") ;
			reason="Can not submit the query to the database";
			help="Check the query error";
			type=6;
			break;
		case 14://the river bank height is smaller 0.0
			place.append("check_set_base_points(_Hyd_River_Profile *upstream_profile, _Hyd_River_Profile *downstream_profile)");
			reason="The river bank height is <=0.0";
			help="Check the given basepoints";
			type=14;
			break;
		case 15://the left river bank is already idealised
			place.append("idealise_profile_fpl(const bool left_flag, _hyd_idealisation_parameters ideal_param)");
			reason="The left river bank is already idealised by another FPL-section. The FPL-section are intercepting each other";
			help="Check the hydraulic connection parameters of the FPL-section";
			type=28;
			break;
		case 16://the right river bank is already idealised
			place.append("idealise_profile_fpl(const bool left_flag, _hyd_idealisation_parameters ideal_param)");
			reason="The right river bank is already idealised by another FPL-section. The FPL-section are intercepting each other";
			help="Check the hydraulic connection parameters of the FPL-section";
			type=28;
			break;
		case 17://no breach possible
			place.append("idealise_profile_fpl(const bool left_flag, _hyd_idealisation_parameters ideal_param)");
			reason="There is no base-point set for the left river bank. A breach development and a connection to the FPL-section is not possible";
			help="Check the river profile";
			type=28;
			break;
		case 18://no breach possible
			place.append("idealise_profile_fpl(const bool left_flag, _hyd_idealisation_parameters ideal_param)");
			reason="There is no base-point set for the right river bank. A breach development and a connection to the FPL-section is not possible";
			help="Check the river profile";
			type=28;
			break;
		case 19://bad alloc
			place.append("allocate_break_parameter_...(void)");
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
Warning _Hyd_River_Profile::set_warning(const int warn_type){
		string place="_Hyd_River_Profile::";
		string help;
		string reason;
		string reaction;
		int type=0;
		Warning msg;
		stringstream info;

	switch (warn_type){
		case 0://the identity are not in the right order
			place.append("check_profiles(void)") ;
			reason="The delta-x value for the table discretisation is <= 0.0; it is not possible ";
			reaction="It is set to 0.2";
			help= "Check the profile x-table value";
			info << "Profile name   :" << this->name <<endl;
			info << "Profile number :" <<this->profile_number<< endl;
			type=9;
			break;
		case 1://the init condition is above the smallest outer z coordinate of the profile
			place.append("check_profiles(void)") ;
			reason="The initial condition is above the minimum outer z-coordinate of the profile;\n it could lead to instabilities by the overflow calculation (1d/2d) ";
			//reaction="It is set to the minimum of the outer z-coordinate";
			help= "Check the profile initial condition";
			info << "Profile name   :" << this->name <<endl;
			info << "Profile number :" <<this->profile_number<< endl;
			type=9;
			break;
		case 2://input datas can not submitted
			place.append("transfer_profile_members2database(QSqlDatabase *ptr_database, const int rv_number)");
			reason="Can not submit the data of the river profile to the database";
			help="Check the database";
			type=2;
			break;
		case 3://result datas can not submitted
			place.append("output_max_results(QSqlDatabase *ptr_database, const int rv_no)");
			reason="Can not submit the results data of the river profile to the database";
			help="Check the database";
			type=2;
			break;
		case 4://there are multiple data sets to the given profile
			place.append("transfer_hydraulic_boundary_sz2database(QSqlDatabase *ptr_database, const int rv_number)");
			reason="There are multiple global id(s) to the given profile";
			help="Check the database table";
			reaction="The first found global id of the profile is taken";
			type=2;
			break;
		case 5://structure height differs from the given fpl-section height (left bank)
			place.append("idealise_profile_fpl(const bool left_flag, _hyd_idealisation_parameters ideal_param)");
			reason="The structure height of the FPL-section differs more than 50% of the left bank height";
			help="Check the height of the left base point and the leftest (first) profile point of the river profile";
			reaction="The structure height is relevant for a hydraulic coupling";
			type=23;
			break;
		case 6://structure height differs from the given fpl-section height (right bank)
			place.append("idealise_profile_fpl(const bool left_flag, _hyd_idealisation_parameters ideal_param)");
			reason="The structure height of the FPL-section differs more than 50% of the right bank height";
			help="Check the height of the right base point and the rightest (last) profile point of the river profile";
			reaction="The structure height is relevant for a hydraulic coupling";
			type=23;
			break;
		case 7://abrupt fails setting differs from the given fpl-section setting (left bank)
			place.append("idealise_profile_fpl(const bool left_flag, _hyd_idealisation_parameters ideal_param)");
			reason="The left profile abrupt fail setting differs from the FPL-section settings";
			help="Check the abrupt fails setting of the profile and the FPL-section";
			reaction="The abrupt fail setting of the FPL-section is relevant";
			type=23;
			break;
		case 8://abrupt fails setting differs from the given fpl-section setting (right bank)
			place.append("idealise_profile_fpl(const bool left_flag, _hyd_idealisation_parameters ideal_param)");
			reason="The right profile abrupt fail setting differs from the FPL-section settings";
			help="Check the abrupt fails setting of the profile and the FPL-section";
			reaction="The abrupt fail setting of the FPL-section is relevant";
			type=23;
			break;
		case 9://result datas can not submitted
			place.append("output_instat_results(QSqlDatabase *ptr_database, const int rv_no, const string polygon_string, int *glob_id, const string break_sc, const string time)");
			reason = "Can not submit the results data of the river profile to the database";
			help = "Check the database";
			type = 2;
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
