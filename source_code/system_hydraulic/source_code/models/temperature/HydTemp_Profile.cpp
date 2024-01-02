//#include "HydTemp_Profile.h"
#include "Hyd_Headers_Precompiled.h" //Fail??



//init static members
Tables *HydTemp_Profile::profile_table=NULL;
Tables *HydTemp_Profile::boundary_table=NULL;
Tables *HydTemp_Profile::erg_table=NULL;
Tables *HydTemp_Profile::erg_instat_table = NULL;

//constructor
HydTemp_Profile::HydTemp_Profile(void){
	this->name=label::not_set;
	
	this->profile_number=0;
	this->glob_prof_id=-1;

	this->reset_temp_data_struct(&this->air_temp);
	this->reset_temp_data_struct(&this->solar_rad);

	this->reset_temp_data_struct(&this->humid);
	this->reset_temp_data_struct(&this->wind);

	this->reset_temp_data_struct(&this->cloud);
	this->reset_temp_data_struct(&this->shadow);

	this->reset_temp_data_struct(&this->inlet_temp);
	this->reset_temp_data_struct(&this->water_bound_temp);

	this->init_condition=0.0;
	this->must_found_number = 7;


	this->temp_value_max.maximum=0.0;
	this->temp_value_max.time_point=0.0;

	this->temp_value_min.maximum = 999999;
	this->temp_value_min.time_point = 0.0;

	this->temp_average = 0.0;
	this->temp_current = 0.0;

	this->boundary_found = false;
	this->count_average = 0;


	this->discharge_current = 0.0;
	this->waterdepth_current = 0.0;
	this->flow_velocity_current = 0.0;

	this->phi_bed = 0.0;
	this->phi_c = 0.0;
	this->phi_eva = 0.0;
	this->phi_lw = 0.0;
	this->phi_solar = 0.0;
	this->phi_total = 0.0;
	this->delta_temp_flow = 0.0;

	this->distance2up = 0.0;

	
}
//destructor
HydTemp_Profile::~HydTemp_Profile(void){

}
//________________________________
//public
//Input the members of the profile per file
void HydTemp_Profile::input_members(QFile *profile_file, const int profile_number, string my_line, int *line_counter){
	//output
	ostringstream cout;
	cout <<"Read in temperature data of Profilenumber " << profile_number << "..." <<endl;
	Sys_Common_Output::output_hyd->output_txt(&cout, true);

	//set the profile number
	this->profile_number=profile_number;


	int found_counter=0;
	int file_pos = profile_file->pos();

	string buffer;
	QString qbuffer;
	string key;
	int pos=-1;
	//file position
	qint64 get_pointer=0;

	stringstream buffer_stream;

	//search in the given line after name and number of points
	//find first find the name "name"
	pos = my_line.find("\"", 0);
	buffer = my_line.erase(0, pos + 1);
	pos = buffer.find("\"", 0);
	this->name = buffer.substr(0, pos);
	if (buffer_stream.fail() == true) {
		ostringstream info;
		info << "Wrong input sequenze " << buffer_stream.str() << endl;
		info << "Profile number " << profile_number << endl;
		info << "Error occurs near line: " << *line_counter << endl;
		Error msg = this->set_error(6);
		msg.make_second_info(info.str());
		throw msg;
	}
	buffer_stream.clear();
	buffer_stream.str("");

	try{
		do{
			get_pointer=profile_file->pos();
			//(*line_counter)++;
			qbuffer=profile_file->readLine();
			buffer=qbuffer.toStdString();
			_Hyd_Parse_IO::erase_comment(&buffer);
			_Hyd_Parse_IO::erase_carriageReturn(&buffer);
			_Hyd_Parse_IO::erase_leading_whitespace_tabs(&buffer);
			_Hyd_Parse_IO::erase_end_whitespace_tabs(&buffer);
			_Hyd_Parse_IO::erase_quote(&buffer);
			QString Qbuff;
			Qbuff = buffer.c_str();
			

			if (Qbuff.contains("AUXDATA",Qt::CaseInsensitive)==true){
				QStringList buff_list;
				buff_list = Qbuff.split(' ',QString::SkipEmptyParts);
				if (buff_list.contains("InitCondition", Qt::CaseInsensitive) == true) {
					bool correct = true;
					this->init_condition = buff_list[2].toDouble(&correct);
					if (correct == false) {
						//Error
						Error msg = this->set_error(3);
						ostringstream info;
						info << "Wrong input sequenze " << Qbuff.toStdString() << endl;
						info << "Error occurs near line: " << *line_counter << endl;
						info << "Line: " << Qbuff.toStdString() << endl;
						msg.make_second_info(info.str());
						throw msg;

					}
					else {
						(found_counter)++;
					}
				}
			}
			else if (Qbuff.contains("AIR_TEMP", Qt::CaseInsensitive) == true) {

				try {
					this->decide_keyvalues_file(Qbuff, &this->air_temp, &found_counter);
				}
				catch (Error msg) {
					ostringstream info;
					info << "Error occurs near line: " << *line_counter << endl;
					info << "Line: " << Qbuff.toStdString() << endl;
					msg.make_second_info(info.str());
					throw msg;
				}
				
			}
			else if (Qbuff.contains("SOLAR_RAD", Qt::CaseInsensitive) == true) {

				try {
					this->decide_keyvalues_file(Qbuff, &this->solar_rad, &found_counter);
				}
				catch (Error msg) {
					ostringstream info;
					info << "Error occurs near line: " << *line_counter << endl;
					info << "Line: " << Qbuff.toStdString() << endl;
					msg.make_second_info(info.str());
					throw msg;
				}

			}
			else if (Qbuff.contains("HUMID", Qt::CaseInsensitive) == true) {

				try {
					this->decide_keyvalues_file(Qbuff, &this->humid, &found_counter);
				}
				catch (Error msg) {
					ostringstream info;
					info << "Error occurs near line: " << *line_counter << endl;
					info << "Line: " << Qbuff.toStdString() << endl;
					msg.make_second_info(info.str());
					throw msg;
				}

			}
			else if (Qbuff.contains("WIND", Qt::CaseInsensitive) == true) {

				try {
					this->decide_keyvalues_file(Qbuff, &this->wind, &found_counter);
				}
				catch (Error msg) {
					ostringstream info;
					info << "Error occurs near line: " << *line_counter << endl;
					info << "Line: " << Qbuff.toStdString() << endl;
					msg.make_second_info(info.str());
					throw msg;
				}

			}
			else if (Qbuff.contains("CLOUD", Qt::CaseInsensitive) == true) {

				try {
					this->decide_keyvalues_file(Qbuff, &this->cloud, &found_counter);
				}
				catch (Error msg) {
					ostringstream info;
					info << "Error occurs near line: " << *line_counter << endl;
					info << "Line: " << Qbuff.toStdString() << endl;
					msg.make_second_info(info.str());
					throw msg;
				}

			}
			else if (Qbuff.contains("SHADOW", Qt::CaseInsensitive) == true) {

				try {
					this->decide_keyvalues_file(Qbuff, &this->shadow, &found_counter);
				}
				catch (Error msg) {
					ostringstream info;
					info << "Error occurs near line: " << *line_counter << endl;
					info << "Line: " << Qbuff.toStdString() << endl;
					msg.make_second_info(info.str());
					throw msg;
				}

			}
			else if (Qbuff.contains("INLET_TEMP", Qt::CaseInsensitive) == true) {
				this->must_found_number++;
				try {
					this->decide_keyvalues_file(Qbuff, &this->inlet_temp, &found_counter);
				}
				catch (Error msg) {
					ostringstream info;
					info << "Error occurs near line: " << *line_counter << endl;
					info << "Line: " << Qbuff.toStdString() << endl;
					msg.make_second_info(info.str());
					throw msg;
				}

			}
			else if (Qbuff.contains("WATER_TEMP", Qt::CaseInsensitive) == true) {
				this->must_found_number++;
				try {
					this->decide_keyvalues_file(Qbuff, &this->water_bound_temp, &found_counter);
				}
				catch (Error msg) {
					ostringstream info;
					info << "Error occurs near line: " << *line_counter << endl;
					info << "Line: " << Qbuff.toStdString() << endl;
					msg.make_second_info(info.str());
					throw msg;
				}

			}
			else if (Qbuff.contains("ZONE", Qt::CaseInsensitive) == true) {

				break;

			}

		}while(profile_file->atEnd()!=true);

		if (profile_number == 0) {
			if (water_bound_temp.applied_flag == false) {
				//Error
				Error msg = this->set_error(7);
				throw msg;


			}
			else {
				if (found_counter > 2) {
					//Warning not used
					Warning msg = this->set_warning(10);
					ostringstream info;
					info << "Profile number            : " << this->profile_number << endl;
					info << "Profile name              : " << this->name << endl;
					this->reset_temp_data_struct(&this->air_temp);
					this->reset_temp_data_struct(&this->solar_rad);

					this->reset_temp_data_struct(&this->humid);
					this->reset_temp_data_struct(&this->wind);

					this->reset_temp_data_struct(&this->cloud);
					this->reset_temp_data_struct(&this->shadow);

					this->reset_temp_data_struct(&this->inlet_temp);
					msg.make_second_info(info.str());
					msg.output_msg(2);
				}
			}



		}
		else {
			if (water_bound_temp.applied_flag == false && inlet_temp.applied_flag==false) {
				//check if all needed values are found (they are set in constructors of each connection type)
				if (found_counter != this->must_found_number) {
					Error msg = this->set_error(2);
					ostringstream info;
					info << "Found values          : " << found_counter << endl;
					info << "Must-found values     : " << this->must_found_number << endl;
					info << "Error occurs near line: " << *line_counter << endl;
					msg.make_second_info(info.str());
					throw msg;
				}
			}
			//temperture direct
			else if(water_bound_temp.applied_flag == true) {
				if (found_counter > 2) {
					//Warning not used
					Warning msg = this->set_warning(10);
					ostringstream info;
					info << "Profile number            : " << this->profile_number << endl;
					info << "Profile name              : " << this->name << endl;
					this->reset_temp_data_struct(&this->air_temp);
					this->reset_temp_data_struct(&this->solar_rad);

					this->reset_temp_data_struct(&this->humid);
					this->reset_temp_data_struct(&this->wind);

					this->reset_temp_data_struct(&this->cloud);
					this->reset_temp_data_struct(&this->shadow);

					this->reset_temp_data_struct(&this->inlet_temp);
					msg.make_second_info(info.str());
					msg.output_msg(2);
				}

			}
			//inlet
			else{
				if (found_counter > 2) {
					//Warning not used
					Warning msg = this->set_warning(11);
					ostringstream info;
					info << "Profile number            : " << this->profile_number << endl;
					info << "Profile name              : " << this->name << endl;
					this->reset_temp_data_struct(&this->air_temp);
					this->reset_temp_data_struct(&this->solar_rad);

					this->reset_temp_data_struct(&this->humid);
					this->reset_temp_data_struct(&this->wind);

					this->reset_temp_data_struct(&this->cloud);
					this->reset_temp_data_struct(&this->shadow);
					msg.make_second_info(info.str());
					msg.output_msg(2);
				}


			}
		}
	}
	catch(Error msg){
		ostringstream info;
		info << "Profile number:"<<this->profile_number<<endl;
		info << "Profilname    :" << this->name << endl;
		msg.make_second_info(info.str());
		throw msg;
	}

	//rewind file to the start!
	profile_file->seek(file_pos);


}
//Transfer the river profile temperature  data to a database
void HydTemp_Profile::transfer_profile_members2database(const int glob_prof_id, QSqlQuery *model, QSqlDatabase *ptr_database, const int rv_number) {
	if (Sys_Project::get_project_type() == _sys_project_type::proj_hyd_temp) {
		//the table is set (the name and the column names) and allocated
		try {
			HydTemp_Profile::set_profile_table(ptr_database);
		}
		catch (Error msg) {
			throw msg;
		}
		this->glob_prof_id = glob_prof_id;
		//set the query via a query string


		ostringstream query_string;
		query_string << "INSERT INTO  " << HydTemp_Profile::profile_table->get_table_name();
		query_string << " ( ";
		query_string << HydTemp_Profile::profile_table->get_column_name(hyd_label::profdata_rvno) << " , ";
		query_string << HydTemp_Profile::profile_table->get_column_name(hyd_label::profdata_name) << " , ";
		query_string << HydTemp_Profile::profile_table->get_column_name(hyd_label::profdata_init) << " , ";
		query_string << HydTemp_Profile::profile_table->get_column_name(hyd_label::profdata_prof_id) << " , ";
		query_string << HydTemp_Profile::profile_table->get_column_name(hyd_label::profdata_glob_id) << " , ";
		query_string << HydTemp_Profile::profile_table->get_column_name(label::areastate_id) << " , ";
		query_string << HydTemp_Profile::profile_table->get_column_name(label::measure_id) << " , ";
		query_string << HydTemp_Profile::profile_table->get_column_name(label::applied_flag) << " ) ";

		query_string << " VALUES ( ";
		query_string << rv_number << " , ";
		query_string << "'" << this->name << "' , ";
		query_string << FORMAT_FIXED_REAL << P(15);
		query_string << this->init_condition << " , ";
		query_string << this->profile_number << " , ";
		query_string << glob_prof_id << " , ";


		query_string << this->system_id.area_state << " , ";
		query_string << this->system_id.measure_nr << " , ";
		query_string << "true" << " ) ";



		Data_Base::database_request(model, query_string.str(), ptr_database);

		if (model->lastError().isValid()) {
			Warning msg = this->set_warning(2);
			ostringstream info;
			info << "Table Name                : " << HydTemp_Profile::profile_table->get_table_name() << endl;
			info << "Table error info          : " << model->lastError().text().toStdString() << endl;
			info << "Profile number            : " << this->profile_number << endl;
			info << "Profile name              : " << this->name << endl;
			msg.make_second_info(info.str());
			msg.output_msg(2);
		}

		try {
			//boundary condition
			this->transfer_boundary_data2database(ptr_database, glob_prof_id);
		}
		catch (Error msg) {
			ostringstream info;
			info << "Profile number            : " << this->profile_number << endl;
			info << "Profile name              : " << this->name << endl;
			msg.make_second_info(info.str());
			throw msg;
		}

	}
}
//Transfer the river profile boundary data of an hydraulc boundary scenario to a database
void HydTemp_Profile::transfer_hydraulic_boundary_sz2database(QSqlDatabase *ptr_database, const int rv_number){
	if(this->boundary_is_applied()==false){
		return;
	}

	//mysql query with the table_model
	QSqlQueryModel model;

	//the table is set (the name and the column names) and allocated
	try{
		HydTemp_Profile::set_profile_table(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}

	//evaluate th global id of the profile
	int id_glob=0;
	//generate the filter
	ostringstream filter;
	filter << "Select ";
	filter << HydTemp_Profile::profile_table->get_column_name(hyd_label::profdata_glob_id);
	filter << " from "<< HydTemp_Profile::profile_table->get_table_name();
	filter << " where ";
	filter << HydTemp_Profile::profile_table->get_column_name(label::applied_flag) << " = true";
	filter << " and ";
	filter << HydTemp_Profile::profile_table->get_column_name(label::areastate_id) << " = " << this->system_id.area_state;
	filter << " and (";
		filter << HydTemp_Profile::profile_table->get_column_name(label::measure_id) << " = " << 0 ;
		filter << " or " ;
		filter << HydTemp_Profile::profile_table->get_column_name(label::measure_id) << " = " << this->system_id.measure_nr;
	filter << " ) " ;
	filter << " and ";
	filter << HydTemp_Profile::profile_table->get_column_name(hyd_label::profdata_rvno) << " = " << rv_number;
	filter << " and ";
	filter << HydTemp_Profile::profile_table->get_column_name(hyd_label::profdata_prof_id) << " = " << this->profile_number;
	//database request
	Data_Base::database_request(&model,filter.str(), ptr_database);
	//check the query
	if(model.lastError().isValid()){
		//error
		Error msg=set_error(13);
		ostringstream info;
		info << "Table Name      : " <<HydTemp_Profile::profile_table->get_table_name() << endl;
		info << "Table error info: " << model.lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
	if(model.rowCount()==0){
		Error msg=this->set_error(12);
		ostringstream info;
		info << "Table Name      : " << HydTemp_Profile::profile_table->get_table_name() << endl;
		info << "Profile number  : " << this->profile_number << endl;
		info << "Profile name    : " << this->name << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
	id_glob=model.record(0).value((HydTemp_Profile::profile_table->get_column_name(hyd_label::profdata_glob_id)).c_str()).toInt();

	if(model.rowCount()>1){
		Warning msg=this->set_warning(4);
		ostringstream info;
		info << "Table Name      : " << HydTemp_Profile::profile_table->get_table_name() << endl;
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
//Input the river profile temperature data with the index from database selection
void HydTemp_Profile::input_members_per_database(const int index, const QSqlQueryModel *query_result, QSqlDatabase *ptr_database) {
	if (Sys_Project::get_project_type() == _sys_project_type::proj_hyd_temp) {
		try {
			//set the river profile number
			this->profile_number = query_result->record(index).value((HydTemp_Profile::profile_table->get_column_name(hyd_label::profdata_prof_id)).c_str()).toInt();

			//read out the results of the request for this index
			this->glob_prof_id = query_result->record(index).value((HydTemp_Profile::profile_table->get_column_name(hyd_label::profdata_glob_id)).c_str()).toInt();
			//name
			this->name = query_result->record(index).value((HydTemp_Profile::profile_table->get_column_name(hyd_label::profdata_name)).c_str()).toString().toStdString();
			//init-condition
			this->init_condition = query_result->record(index).value((HydTemp_Profile::profile_table->get_column_name(hyd_label::profdata_init)).c_str()).toDouble();


			//read in the appending tables
			//boundary data
			this->input_boundary_data_per_database(index, ptr_database, this->glob_prof_id);


		}
		catch (Error msg) {
			ostringstream info;
			info << "Profile number            : " << this->profile_number << endl;
			info << "Profile name              : " << this->name << endl;
			msg.make_second_info(info.str());
			throw msg;
		}

	}


}
//Select and count the relevant temperature  profiles in the database table and return the number (static)
int HydTemp_Profile::select_relevant_profiles_in_database(QSqlQueryModel *query_result, QSqlDatabase *ptr_database, const _sys_system_id id, const int rv_number, const bool with_output){
	int number=0;
	try{
		HydTemp_Profile::set_profile_table(ptr_database);
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
	test_filter << HydTemp_Profile::profile_table->get_column_name(hyd_label::profdata_glob_id) << " , ";
	test_filter << HydTemp_Profile::profile_table->get_column_name(hyd_label::profdata_prof_id) << " , ";
	test_filter << HydTemp_Profile::profile_table->get_column_name(hyd_label::profdata_name) << " , ";
	test_filter << HydTemp_Profile::profile_table->get_column_name(hyd_label::profdata_init);
	test_filter << " from " << HydTemp_Profile::profile_table->get_table_name();
	test_filter << " where ";
	test_filter << HydTemp_Profile::profile_table->get_column_name(label::applied_flag) << " = true";
	test_filter << " and ";
	test_filter << HydTemp_Profile::profile_table->get_column_name(label::areastate_id) << " = " << id.area_state;
	test_filter << " and (";
	test_filter << HydTemp_Profile::profile_table->get_column_name(label::measure_id) << " = " << 0 ;
	test_filter << " or " ;
	test_filter << HydTemp_Profile::profile_table->get_column_name(label::measure_id) << " = " << id.measure_nr;
	test_filter << " ) " ;
	test_filter << " and ";
	test_filter << HydTemp_Profile::profile_table->get_column_name(hyd_label::profdata_rvno) << " = " << rv_number;
	test_filter << " order by " << HydTemp_Profile::profile_table->get_column_name(hyd_label::profdata_prof_id)<< " ASC ";

	Data_Base::database_request(query_result, test_filter.str(), ptr_database);

	//check the request
	if(query_result->lastError().isValid()){
		Error msg;
		msg.set_msg("HydTemp_Profile::select_relevant_profiles_in_database(QSqlTableModel *query_result, const _sys_system_id id, const int rv_number, const bool with_output)","Invalid database request", "Check the database", 2, false);
		ostringstream info;
		info << "Table Name      : " << HydTemp_Profile::profile_table->get_table_name() << endl;
		info << "Table error info: " << query_result->lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}

	number= query_result->rowCount();

	if(with_output==true){
		ostringstream cout;
		cout << number << " relevant temperature profile(s) are found in database" << endl ;
		Sys_Common_Output::output_hyd->output_txt(&cout);
	}
	return number;
}
//Select and count the relevant temperature  profiles in the database table and return the number just for the given system_id (static)
int HydTemp_Profile::select_profiles_in_database(QSqlQueryModel *query_result, QSqlDatabase *ptr_database, const _sys_system_id id){
		int number=0;
	try{
		HydTemp_Profile::set_profile_table(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}


	ostringstream test_filter;
	test_filter<< "Select ";
	test_filter << HydTemp_Profile::profile_table->get_column_name(hyd_label::profdata_glob_id) << " , ";
	test_filter << HydTemp_Profile::profile_table->get_column_name(hyd_label::profdata_prof_id) << " , ";
	test_filter << HydTemp_Profile::profile_table->get_column_name(hyd_label::profdata_name) << " , ";
	test_filter << HydTemp_Profile::profile_table->get_column_name(hyd_label::profdata_init);
	test_filter << " from " << HydTemp_Profile::profile_table->get_table_name();
	test_filter << " where ";
	test_filter << HydTemp_Profile::profile_table->get_column_name(label::areastate_id) << " = " << id.area_state;
	test_filter << " and ";
	test_filter << HydTemp_Profile::profile_table->get_column_name(label::measure_id) << " = " << id.measure_nr;

	Data_Base::database_request(query_result, test_filter.str(), ptr_database);

	//check the request
	if(query_result->lastError().isValid()){
		Error msg;
		msg.set_msg("HydTemp_Profile::select_profiles_in_database(QSqlQueryModel *query_result, QSqlDatabase *ptr_database, const _sys_system_id id)","Invalid database request", "Check the database", 2, false);
		ostringstream info;
		info << "Table Name      : " << HydTemp_Profile::profile_table->get_table_name() << endl;
		info << "Table error info: " << query_result->lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}

	number= query_result->rowCount();


	return number;
}
//Select and count the relevant temperature  profiles in the database table and return the number just for the given system_id and river number (static)
int HydTemp_Profile::select_profiles_in_database(QSqlQueryModel *query_result, QSqlDatabase *ptr_database, const _sys_system_id id, const int rv_number){
	int number=0;
	try{
		HydTemp_Profile::set_profile_table(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}


	ostringstream test_filter;
	test_filter<< "Select ";
	test_filter << HydTemp_Profile::profile_table->get_column_name(hyd_label::profdata_glob_id) << " , ";
	test_filter << HydTemp_Profile::profile_table->get_column_name(hyd_label::profdata_prof_id) << " , ";
	test_filter << HydTemp_Profile::profile_table->get_column_name(hyd_label::profdata_name) << " , ";
	test_filter << HydTemp_Profile::profile_table->get_column_name(hyd_label::profdata_init);
	test_filter << " from " << HydTemp_Profile::profile_table->get_table_name();
	test_filter << " where ";
	test_filter << HydTemp_Profile::profile_table->get_column_name(label::areastate_id) << " = " << id.area_state;
	test_filter << " and ";
	test_filter << HydTemp_Profile::profile_table->get_column_name(label::measure_id) << " = " << id.measure_nr;
	test_filter << " and ";
	test_filter << HydTemp_Profile::profile_table->get_column_name(hyd_label::profdata_rvno) << " = " << rv_number;
	test_filter << " order by " << HydTemp_Profile::profile_table->get_column_name(hyd_label::profdata_prof_id)<< " ASC ";

	Data_Base::database_request(query_result, test_filter.str(), ptr_database);

	//check the request
	if(query_result->lastError().isValid()){
		Error msg;
		msg.set_msg("HydTemp_Profile::select_profiles_in_database(QSqlTableModel *query_result, const _sys_system_id id, const int rv_number, const bool with_output)","Invalid database request", "Check the database", 2, false);
		ostringstream info;
		info << "Table Name      : " << HydTemp_Profile::profile_table->get_table_name() << endl;
		info << "Table error info: " << query_result->lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}

	number= query_result->rowCount();


	return number;

}
//Select the relevant temperature  profiles in the database table for a given connection type (static)
int HydTemp_Profile::select_relevant_profiles_in_database(QSqlQueryModel *query_result, QSqlDatabase *ptr_database, const _sys_system_id id, const int rv_number, _hyd_connection_types conn_type, const bool with_output){
	int number=0;
	try{
		HydTemp_Profile::set_profile_table(ptr_database);
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
	test_filter << HydTemp_Profile::profile_table->get_column_name(hyd_label::profdata_glob_id) << " , ";
	test_filter << HydTemp_Profile::profile_table->get_column_name(hyd_label::profdata_prof_id) << " , ";
	test_filter << HydTemp_Profile::profile_table->get_column_name(hyd_label::profdata_name) << " , ";
	test_filter << HydTemp_Profile::profile_table->get_column_name(hyd_label::profdata_init);
	test_filter << " from " << HydTemp_Profile::profile_table->get_table_name();
	test_filter << " where ";
	test_filter << HydTemp_Profile::profile_table->get_column_name(label::applied_flag) << " = true";
	test_filter << " and ";
	test_filter << HydTemp_Profile::profile_table->get_column_name(label::areastate_id) << " = " << id.area_state;
	test_filter << " and (";
	test_filter << HydTemp_Profile::profile_table->get_column_name(label::measure_id) << " = " << 0 ;
	test_filter << " or " ;
	test_filter << HydTemp_Profile::profile_table->get_column_name(label::measure_id) << " = " << id.measure_nr;
	test_filter << " ) " ;
	test_filter << " and ";
	test_filter << HydTemp_Profile::profile_table->get_column_name(hyd_label::profdata_rvno) << " = " << rv_number;


	Data_Base::database_request(query_result, test_filter.str(), ptr_database);

	//check the request
	if(query_result->lastError().isValid()){
		Error msg;
		msg.set_msg("HydTemp_Profile::select_relevant_profiles_in_database(QSqlTableModel *query_result, const _sys_system_id id, const int rv_number, _hyd_connection_types conn_type, const bool with_output=true )","Invalid database request", "Check the database", 2, false);
		ostringstream info;
		info << "Table Name      : " << HydTemp_Profile::profile_table->get_table_name() << endl;
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
//Select and count the relevant temperature  profiles in the database table with a given global profile number (static)
int HydTemp_Profile::select_relevant_profile_in_database(QSqlQueryModel *query_result, QSqlDatabase *ptr_database, const _sys_system_id id, const int rv_number, const int glob_prof_no){
	int number=0;
	try{
		HydTemp_Profile::set_profile_table(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}

	ostringstream test_filter;
	test_filter<< "Select ";
	test_filter << HydTemp_Profile::profile_table->get_column_name(hyd_label::profdata_glob_id) << " , ";
	test_filter << HydTemp_Profile::profile_table->get_column_name(hyd_label::profdata_prof_id) << " , ";
	test_filter << HydTemp_Profile::profile_table->get_column_name(hyd_label::profdata_name) << " , ";
	test_filter << HydTemp_Profile::profile_table->get_column_name(hyd_label::profdata_init);
	test_filter << " from " << HydTemp_Profile::profile_table->get_table_name();
	test_filter << " where ";
	test_filter << HydTemp_Profile::profile_table->get_column_name(label::applied_flag) << " = true";
	test_filter << " and ";
	test_filter << HydTemp_Profile::profile_table->get_column_name(label::areastate_id) << " = " << id.area_state;
	test_filter << " and (";
	test_filter << HydTemp_Profile::profile_table->get_column_name(label::measure_id) << " = " << 0 ;
	test_filter << " or " ;
	test_filter << HydTemp_Profile::profile_table->get_column_name(label::measure_id) << " = " << id.measure_nr;
	test_filter << " ) " ;
	test_filter << " and ";
	test_filter << HydTemp_Profile::profile_table->get_column_name(hyd_label::profdata_rvno) << " = " << rv_number;
	test_filter << " and ";
	test_filter << HydTemp_Profile::profile_table->get_column_name(hyd_label::profdata_glob_id) << " = " << glob_prof_no<<"";

	Data_Base::database_request(query_result, test_filter.str(), ptr_database);

	//check the request
	if(query_result->lastError().isValid()){
		Error msg;
		msg.set_msg("HydTemp_Profile::select_relevant_profiles_in_database(QSqlTableModel *query_result, const _sys_system_id id, const int rv_number, const int glob_prof_no)","Invalid database request", "Check the database", 2, false);
		ostringstream info;
		info << "Table Name      : " << HydTemp_Profile::profile_table->get_table_name() << endl;
		info << "Table error info: " << query_result->lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}

	number= query_result->rowCount();

	return number;
}
//Set the database table for the temperature  profile data of the river model: it sets the table name and the name of the columns and allocate them (static)
void HydTemp_Profile::set_profile_table(QSqlDatabase *ptr_database){
	if (Sys_Project::get_project_type() == _sys_project_type::proj_hyd_temp) {
		if (HydTemp_Profile::profile_table == NULL) {
			//make specific input for this class
			const string tab_id_name = hyd_label::tab_tempprof;

			string tab_id_col[9];
			tab_id_col[0] = hyd_label::profdata_rvno;
			tab_id_col[1] = hyd_label::profdata_name;
			tab_id_col[2] = hyd_label::profdata_init;
			tab_id_col[3] = hyd_label::profdata_prof_id;
			tab_id_col[4] = hyd_label::profdata_glob_id;
			tab_id_col[5] = label::description;
			tab_id_col[6] = label::areastate_id;
			tab_id_col[7] = label::measure_id;
			tab_id_col[8] = label::applied_flag;

			try {
				HydTemp_Profile::profile_table = new Tables(tab_id_name, tab_id_col, sizeof(tab_id_col) / sizeof(tab_id_col[0]));
				HydTemp_Profile::profile_table->set_name(ptr_database, _sys_table_type::hyd);
			}
			catch (bad_alloc& t) {
				Error msg;
				msg.set_msg("HydTemp_Profile::set_profile_table(QSqlDatabase *ptr_database)", "Can not allocate the memory", "Check the memory", 10, false);
				ostringstream info;
				info << "Info bad alloc: " << t.what() << endl;
				msg.make_second_info(info.str());
				throw msg;
			}
			catch (Error msg) {
				HydTemp_Profile::close_profile_table();
				throw msg;
			}
		}
	}
}
//Set the database table for the boundary profile data: it sets the table name and the name of the columns and allocate them (static)
void HydTemp_Profile::set_profile_boundary_table(QSqlDatabase *ptr_database){
	if (Sys_Project::get_project_type() == _sys_project_type::proj_hyd_temp) {
		if (HydTemp_Profile::boundary_table == NULL) {
			//make specific input for this class
			const string tab_id_name = hyd_label::tab_tempprof_bound;

			string tab_id_col[9];
			tab_id_col[0] = hyd_label::profdata_glob_id;
			tab_id_col[1] = label::areastate_id;
			tab_id_col[2] = label::measure_id;
			tab_id_col[3] = hyd_label::sz_bound_id;
			tab_id_col[4] = hyd_label::bounddata_stat;
			tab_id_col[5] = hyd_label::bounddata_value;
			tab_id_col[6] = hyd_label::bounddata_type;
			tab_id_col[7] = hyd_label::bounddata_name;
			tab_id_col[8] = label::applied_flag;

			try {
				HydTemp_Profile::boundary_table = new Tables(tab_id_name, tab_id_col, sizeof(tab_id_col) / sizeof(tab_id_col[0]));
				HydTemp_Profile::boundary_table->set_name(ptr_database, _sys_table_type::hyd);
			}
			catch (bad_alloc& t) {
				Error msg;
				msg.set_msg("HydTemp_Profile::set_profile_boundary_table(QSqlDatabase *ptr_database)", "Can not allocate the memory", "Check the memory", 10, false);
				ostringstream info;
				info << "Info bad alloc: " << t.what() << endl;
				msg.make_second_info(info.str());
				throw msg;
			}
			catch (Error msg) {
				HydTemp_Profile::close_boundary_table();
				throw msg;
			}
		}
	}
}
//Create the database table for the profile data of the temperature  model (static)
void HydTemp_Profile::create_profile_table(QSqlDatabase *ptr_database){
	if (Sys_Project::get_project_type() == _sys_project_type::proj_hyd_temp) {
		if (HydTemp_Profile::profile_table == NULL) {
			ostringstream cout;
			cout << "Create temperature profile database table..." << endl;
			Sys_Common_Output::output_hyd->output_txt(&cout);
			//make specific input for this class
			const string tab_name = hyd_label::tab_tempprof;
			const int num_col = 9;
			_Sys_data_tab_column tab_col[num_col];
			//init
			for (int i = 0; i < num_col; i++) {
				tab_col[i].key_flag = false;
				tab_col[i].unsigned_flag = false;
				tab_col[i].primary_key_flag = false;
			}

			tab_col[0].name = hyd_label::profdata_glob_id;
			tab_col[0].type = sys_label::tab_col_type_int;
			tab_col[0].unsigned_flag = true;
			tab_col[0].primary_key_flag = true;

			tab_col[1].name = hyd_label::profdata_rvno;
			tab_col[1].type = sys_label::tab_col_type_int;
			tab_col[1].unsigned_flag = true;
			tab_col[1].key_flag = true;

			tab_col[2].name = hyd_label::profdata_prof_id;
			tab_col[2].type = sys_label::tab_col_type_int;
			tab_col[2].unsigned_flag = true;

			tab_col[3].name = label::areastate_id;
			tab_col[3].type = sys_label::tab_col_type_int;
			tab_col[3].unsigned_flag = true;
			tab_col[3].key_flag = true;

			tab_col[4].name = label::measure_id;
			tab_col[4].type = sys_label::tab_col_type_int;
			tab_col[4].unsigned_flag = true;
			tab_col[4].key_flag = true;

			tab_col[5].name = label::applied_flag;
			tab_col[5].type = sys_label::tab_col_type_bool;
			tab_col[5].default_value = "true";
			tab_col[5].key_flag = true;

			tab_col[6].name = hyd_label::profdata_name;
			tab_col[6].type = sys_label::tab_col_type_string;

			tab_col[7].name = hyd_label::profdata_init;
			tab_col[7].type = sys_label::tab_col_type_double;
			tab_col[7].unsigned_flag = true;
			tab_col[7].default_value = "0";


			tab_col[8].name = label::description;
			tab_col[8].type = sys_label::tab_col_type_string;


			try {
				HydTemp_Profile::profile_table = new Tables();
				if (HydTemp_Profile::profile_table->create_non_existing_tables(tab_name, tab_col, num_col, ptr_database, _sys_table_type::hyd) == false) {
					cout << " Table exists" << endl;
					Sys_Common_Output::output_hyd->output_txt(&cout);
				};
			}
			catch (bad_alloc& t) {
				Error msg;
				msg.set_msg("HydTemp_Profile::create_profile_table(QSqlDatabase *ptr_database)", "Can not allocate the memory", "Check the memory", 10, false);
				ostringstream info;
				info << "Info bad alloc: " << t.what() << endl;
				msg.make_second_info(info.str());
				throw msg;
			}
			catch (Error msg) {
				HydTemp_Profile::close_profile_table();
				throw msg;
			}

			HydTemp_Profile::close_profile_table();
		}
	}
}
//Get the maximum value of the global index of the profiles in database table (static)
int HydTemp_Profile::get_max_glob_id_prof_table(QSqlDatabase *ptr_database){

	//the table is set (the name and the column names) and allocated
	try{
		HydTemp_Profile::set_profile_table(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}

	//evaluate the global identifier
	int id_glob=HydTemp_Profile::profile_table->maximum_int_of_column(HydTemp_Profile::profile_table->get_column_name(hyd_label::profdata_glob_id),ptr_database);
	return id_glob;
}
//Create the database table for the temperature  boundary profile data (static)
void HydTemp_Profile::create_profile_boundary_table(QSqlDatabase *ptr_database){
	if (Sys_Project::get_project_type() == _sys_project_type::proj_hyd_temp) {
		if (HydTemp_Profile::boundary_table == NULL) {
			ostringstream cout;
			cout << "Create temperature profile boundary database table..." << endl;
			Sys_Common_Output::output_hyd->output_txt(&cout);
			//make specific input for this class
			const string tab_name = hyd_label::tab_tempprof_bound;
			const int num_col = 9;
			_Sys_data_tab_column tab_col[num_col];
			//init
			for (int i = 0; i < num_col; i++) {
				tab_col[i].key_flag = false;
				tab_col[i].unsigned_flag = false;
				tab_col[i].primary_key_flag = false;
			}
			tab_col[0].name = hyd_label::profdata_glob_id;
			tab_col[0].type = sys_label::tab_col_type_int;
			tab_col[0].unsigned_flag = true;
			tab_col[0].key_flag = true;

			tab_col[1].name = label::areastate_id;
			tab_col[1].type = sys_label::tab_col_type_int;
			tab_col[1].unsigned_flag = true;
			tab_col[1].default_value = "0";
			tab_col[1].key_flag = true;

			tab_col[2].name = label::measure_id;
			tab_col[2].type = sys_label::tab_col_type_int;
			tab_col[2].unsigned_flag = true;
			tab_col[2].default_value = "0";
			tab_col[2].key_flag = true;

			tab_col[3].name = label::applied_flag;
			tab_col[3].type = sys_label::tab_col_type_bool;
			tab_col[3].default_value = "true";
			tab_col[3].key_flag = true;

			tab_col[4].name = hyd_label::sz_bound_id;
			tab_col[4].type = sys_label::tab_col_type_int;
			tab_col[4].unsigned_flag = true;
			tab_col[4].default_value = "0";
			tab_col[4].key_flag = true;

			tab_col[5].name = hyd_label::bounddata_name;
			tab_col[5].type = sys_label::tab_col_type_string;

			tab_col[6].name = hyd_label::bounddata_stat;
			tab_col[6].type = sys_label::tab_col_type_bool;
			tab_col[6].default_value = "false";

			tab_col[7].name = hyd_label::bounddata_value;
			tab_col[7].type = sys_label::tab_col_type_double;
			tab_col[7].default_value = "0";

			tab_col[8].name = hyd_label::bounddata_type;
			tab_col[8].type = sys_label::tab_col_type_string;



			try {
				HydTemp_Profile::boundary_table = new Tables();
				if (HydTemp_Profile::boundary_table->create_non_existing_tables(tab_name, tab_col, num_col, ptr_database, _sys_table_type::hyd) == false) {
					cout << " Table exists" << endl;
					Sys_Common_Output::output_hyd->output_txt(&cout);
				};
			}
			catch (bad_alloc& t) {
				Error msg;
				msg.set_msg("HydTemp_Profile::create_profile_boundary_table(QSqlDatabase *ptr_database)", "Can not allocate the memory", "Check the memory", 10, false);
				ostringstream info;
				info << "Info bad alloc: " << t.what() << endl;
				msg.make_second_info(info.str());
				throw msg;
			}
			catch (Error msg) {
				HydTemp_Profile::close_boundary_table();
				throw msg;
			}

			HydTemp_Profile::close_boundary_table();
		}
	}
}
//Create the database view for the connection of boundary ids and theriver profiles (static)
void HydTemp_Profile::create_bound2profile_view(QSqlDatabase *ptr_database) {

	if (Sys_Project::get_project_type() == _sys_project_type::proj_hyd_temp) {

		//Set tables
		ostringstream cout;
		//the table is set (the name and the column names) and allocated
		try {
			_Hyd_River_Profile::set_profile_table(ptr_database);
			HydTemp_Profile::set_profile_boundary_table(ptr_database);
		}
		catch (Error msg) {
			throw msg;
		}


		cout << "Create database view to river profile element with temperature boundary conditions..." << endl;
		Sys_Common_Output::output_hyd->output_txt(&cout);
		//make specific input for this class
		const string view_name = hyd_label::view_rvprofile2tempbound;

		QSqlQueryModel query;

		ostringstream query_string;
		query_string << "CREATE VIEW ";
		query_string << Sys_Project::get_complete_project_database_schemata_name() << "." << hyd_label::view_rvprofile2tempbound;
		query_string << " AS SELECT ";
		query_string << "row_number() over() AS Id" << " , ";
		query_string << _Hyd_River_Profile::profile_table->get_column_name_table(hyd_label::profdata_glob_id) << " , ";
		query_string << _Hyd_River_Profile::profile_table->get_column_name_table(hyd_label::profdata_rvno) << " , ";
		query_string << HydTemp_Profile::boundary_table->get_column_name_table(label::areastate_id) << " , ";
		query_string << HydTemp_Profile::boundary_table->get_column_name_table(label::measure_id) << " , ";
		query_string << HydTemp_Profile::boundary_table->get_column_name_table(hyd_label::sz_bound_id) << " , ";
		query_string << HydTemp_Profile::boundary_table->get_column_name_table(label::applied_flag) << " , ";

		query_string << HydTemp_Profile::boundary_table->get_column_name_table(hyd_label::bounddata_name) << " , ";
		query_string << HydTemp_Profile::boundary_table->get_column_name_table(hyd_label::bounddata_stat) << " , ";
		query_string << HydTemp_Profile::boundary_table->get_column_name_table(hyd_label::bounddata_value) << " , ";
		query_string << HydTemp_Profile::boundary_table->get_column_name_table(hyd_label::bounddata_type) << " , ";

		query_string << _Hyd_River_Profile::profile_table->get_column_name_table(hyd_label::profdata_polyline) << "  ";

		query_string << " FROM ";
		query_string << HydTemp_Profile::boundary_table->get_table_name() << "  ";
		query_string << " JOIN ";
		query_string << _Hyd_River_Profile::profile_table->get_table_name() << "  ";
		query_string << " ON ";
		query_string << HydTemp_Profile::boundary_table->get_column_name_table(hyd_label::profdata_glob_id) << "  ";
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
}
//Check if the view exists already (static)
bool HydTemp_Profile::check_bound2profile_view_exists(QSqlDatabase *ptr_database) {
	if (Sys_Project::get_project_type() == _sys_project_type::proj_hyd_temp) {
		QSqlQueryModel query;

		ostringstream query_string;
		query_string << "SELECT EXISTS ( SELECT * FROM information_schema.tables ";
		query_string << "WHERE table_schema ='" << Sys_Project::get_complete_project_database_schemata_name() << "' ";
		query_string << "AND table_name ='" << functions::convert_string2lower_case(hyd_label::view_rvprofile2tempbound) << "' )";


		Data_Base::database_request(&query, query_string.str(), ptr_database);
		if (query.lastError().isValid() == true) {
			Error msg;
			msg.set_msg("HydTemp_Profile::check_bound2profile_view_exists(QSqlDatabase *ptr_database)", "Invalid database request", "Check the database", 2, false);
			ostringstream info;
			info << "View Name      : " << hyd_label::view_rvprofile2tempbound << endl;
			info << "View error info: " << query.lastError().text().toStdString() << endl;
			msg.make_second_info(info.str());
			throw msg;
		}


		if (query.rowCount() > 0) {
			return true;
		}
	}
	return false;

}
//Create the database table for the temperature results of an hydraulic simulation for the river profiles (static)
void HydTemp_Profile::create_erg_table(QSqlDatabase *ptr_database){
	if (Sys_Project::get_project_type() == _sys_project_type::proj_hyd_temp) {
		if (HydTemp_Profile::erg_table == NULL) {
			ostringstream cout;
			cout << "Create temperature profile result database table..." << endl;
			Sys_Common_Output::output_hyd->output_txt(&cout);
			//make specific input for this class
			const string tab_name = hyd_label::tab_tempprof_erg_max;
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

			tab_col[2].name = hyd_label::profdata_prof_id;
			tab_col[2].type = sys_label::tab_col_type_int;
			tab_col[2].unsigned_flag = true;
			tab_col[2].key_flag = true;

			tab_col[3].name = label::areastate_id;
			tab_col[3].type = sys_label::tab_col_type_int;
			tab_col[3].unsigned_flag = true;
			tab_col[3].key_flag = true;

			tab_col[4].name = label::measure_id;
			tab_col[4].type = sys_label::tab_col_type_int;
			tab_col[4].unsigned_flag = true;
			tab_col[4].key_flag = true;

			tab_col[5].name = label::applied_flag;
			tab_col[5].type = sys_label::tab_col_type_bool;
			tab_col[5].default_value = "true";
			tab_col[5].key_flag = true;

			tab_col[6].name = hyd_label::sz_bound_id;
			tab_col[6].type = sys_label::tab_col_type_int;
			tab_col[6].unsigned_flag = true;
			tab_col[6].key_flag = true;

			tab_col[7].name = risk_label::sz_break_id;
			tab_col[7].type = sys_label::tab_col_type_string;
			tab_col[7].key_flag = true;

			tab_col[8].name = hyd_label::proferg_T_max;
			tab_col[8].type = sys_label::tab_col_type_double;
			tab_col[8].unsigned_flag = true;
			tab_col[8].default_value = "0.0";

			tab_col[9].name = hyd_label::proferg_t_T_max;
			tab_col[9].type = sys_label::tab_col_type_double;
			tab_col[9].unsigned_flag = true;
			tab_col[9].default_value = "0.0";

			tab_col[10].name = hyd_label::proferg_T_min;
			tab_col[10].type = sys_label::tab_col_type_double;
			tab_col[10].unsigned_flag = true;
			tab_col[10].default_value = "0.0";

			tab_col[11].name = hyd_label::proferg_t_T_min;
			tab_col[11].type = sys_label::tab_col_type_double;
			tab_col[11].unsigned_flag = true;
			tab_col[11].default_value = "0.0";

			tab_col[12].name = hyd_label::proferg_T_av;
			tab_col[12].type = sys_label::tab_col_type_double;
			tab_col[12].unsigned_flag = true;
			tab_col[12].default_value = "0.0";



			tab_col[13].name = hyd_label::proferg_polygon;
			tab_col[13].type = sys_label::tab_col_type_polygon;

			try {
				HydTemp_Profile::erg_table = new Tables();
				if (HydTemp_Profile::erg_table->create_non_existing_tables(tab_name, tab_col, num_col, ptr_database, _sys_table_type::hyd) == false) {
					cout << " Table exists" << endl;
					Sys_Common_Output::output_hyd->output_txt(&cout);
				};
			}
			catch (bad_alloc& t) {
				Error msg;
				msg.set_msg("HydTemp_Profile::create_erg_table(QSqlDatabase *ptr_database)", "Can not allocate the memory", "Check the memory", 10, false);
				ostringstream info;
				info << "Info bad alloc: " << t.what() << endl;
				msg.make_second_info(info.str());
				throw msg;
			}
			catch (Error msg) {
				HydTemp_Profile::close_erg_table();
				throw msg;
			}

			HydTemp_Profile::close_erg_table();

			//make indizes
			//the table is set (the name and the column names) and allocated
			try {
				HydTemp_Profile::set_erg_table(ptr_database);
			}
			catch (Error msg) {
				throw msg;
			}


			HydTemp_Profile::erg_table->create_spatial_index2column(ptr_database, HydTemp_Profile::erg_table->get_column_name(hyd_label::proferg_polygon));


			HydTemp_Profile::close_erg_table();
		}
	}
}
//Close and delete the database table for the results of an hydraulic simulation the river profiles (static)
void HydTemp_Profile::close_erg_table(void) {
	if (HydTemp_Profile::erg_table != NULL) {
		delete HydTemp_Profile::erg_table;
		HydTemp_Profile::erg_table = NULL;
	}
}
//Set the database table for the results of an hydraulic simulation for the river profiles: it sets the table name and the name of the columns and allocate them (static)
void HydTemp_Profile::set_erg_table(QSqlDatabase *ptr_database, const bool not_close){
	if (Sys_Project::get_project_type() == _sys_project_type::proj_hyd_temp) {
		if (HydTemp_Profile::erg_table == NULL) {
			//make specific input for this class
			const string tab_id_name = hyd_label::tab_tempprof_erg_max;

			string tab_id_col[14];
			tab_id_col[0] = hyd_label::profdata_rvno;
			tab_id_col[1] = hyd_label::profdata_prof_id;
			tab_id_col[2] = label::areastate_id;
			tab_id_col[3] = label::measure_id;
			tab_id_col[4] = hyd_label::sz_bound_id;
			tab_id_col[5] = risk_label::sz_break_id;
			tab_id_col[6] = label::glob_id;
			tab_id_col[7] = label::applied_flag;
			tab_id_col[8] = hyd_label::proferg_T_max;
			tab_id_col[9] = hyd_label::proferg_t_T_max;
			tab_id_col[10] = hyd_label::proferg_T_min;
			tab_id_col[11] = hyd_label::proferg_t_T_min;
			tab_id_col[12] = hyd_label::proferg_T_av;
			tab_id_col[13] = hyd_label::proferg_polygon;






			try {
				HydTemp_Profile::erg_table = new Tables(tab_id_name, tab_id_col, sizeof(tab_id_col) / sizeof(tab_id_col[0]));
				HydTemp_Profile::erg_table->set_name(ptr_database, _sys_table_type::hyd);
			}
			catch (bad_alloc& t) {
				Error msg;
				msg.set_msg("HydTemp_Profile::set_erg_table(QSqlDatabase *ptr_database)", "Can not allocate the memory", "Check the memory", 10, false);
				ostringstream info;
				info << "Info bad alloc: " << t.what() << endl;
				msg.make_second_info(info.str());
				throw msg;
			}
			catch (Error msg) {
				if (not_close == false) {
					HydTemp_Profile::close_erg_table();
				}
				throw msg;
			}
		}
	}
}
//Create the database table for the instationary results of an hydraulic simulation for the river profiles
void HydTemp_Profile::create_erg_instat_table(QSqlDatabase *ptr_database) {
	if (Sys_Project::get_project_type() == _sys_project_type::proj_hyd_temp) {
		if (HydTemp_Profile::erg_instat_table == NULL) {
			ostringstream cout;
			cout << "Create temperature profile instationary result database table..." << endl;
			Sys_Common_Output::output_hyd->output_txt(&cout);
			//make specific input for this class
			const string tab_name = hyd_label::tab_tempprof_instat_erg_max;
			const int num_col = 17;
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



			tab_col[7].name = hyd_label::proferg_T_water;
			tab_col[7].type = sys_label::tab_col_type_double;
			tab_col[7].unsigned_flag = true;
			tab_col[7].default_value = "0.0";

			tab_col[8].name = hyd_label::air_temp;
			tab_col[8].type = sys_label::tab_col_type_double;
			tab_col[8].unsigned_flag = true;
			tab_col[8].default_value = "0.0";

			tab_col[9].name = hyd_label::solar_rad;
			tab_col[9].type = sys_label::tab_col_type_double;
			tab_col[9].unsigned_flag = true;
			tab_col[9].default_value = "0.0";

			tab_col[10].name = hyd_label::humidity;
			tab_col[10].type = sys_label::tab_col_type_double;
			tab_col[10].unsigned_flag = true;
			tab_col[10].default_value = "0.0";

			tab_col[11].name = hyd_label::wind;
			tab_col[11].type = sys_label::tab_col_type_double;
			tab_col[11].unsigned_flag = true;
			tab_col[11].default_value = "0.0";

			tab_col[12].name = hyd_label::cloud;
			tab_col[12].type = sys_label::tab_col_type_double;
			tab_col[12].unsigned_flag = true;
			tab_col[12].default_value = "0.0";

			tab_col[13].name = hyd_label::shadow;
			tab_col[13].type = sys_label::tab_col_type_double;
			tab_col[13].unsigned_flag = true;
			tab_col[13].default_value = "0.0";

			tab_col[14].name = hyd_label::inlet_temp;
			tab_col[14].type = sys_label::tab_col_type_double;
			tab_col[14].unsigned_flag = true;
			tab_col[14].default_value = "0.0";

			tab_col[15].name = hyd_label::data_time;
			tab_col[15].type = sys_label::tab_col_type_string;
			tab_col[15].default_value = "";


			tab_col[16].name = hyd_label::proferg_polygon;
			tab_col[16].type = sys_label::tab_col_type_polygon;

			try {
				HydTemp_Profile::erg_instat_table = new Tables();
				if (HydTemp_Profile::erg_instat_table->create_non_existing_tables(tab_name, tab_col, num_col, ptr_database, _sys_table_type::hyd) == false) {
					cout << " Table exists" << endl;
					Sys_Common_Output::output_hyd->output_txt(&cout);
				};
			}
			catch (bad_alloc& t) {
				Error msg;
				msg.set_msg("HydTemp_Profile::create_erg_instat_table(QSqlDatabase *ptr_database)", "Can not allocate the memory", "Check the memory", 10, false);
				ostringstream info;
				info << "Info bad alloc: " << t.what() << endl;
				msg.make_second_info(info.str());
				throw msg;
			}
			catch (Error msg) {
				HydTemp_Profile::close_erg_instat_table();
				throw msg;
			}

			HydTemp_Profile::close_erg_instat_table();

			//make indizes
			//the table is set (the name and the column names) and allocated
			try {
				HydTemp_Profile::set_erg_instat_table(ptr_database);
			}
			catch (Error msg) {
				throw msg;
			}


			HydTemp_Profile::erg_instat_table->create_index2column(ptr_database, HydTemp_Profile::erg_instat_table->get_column_name(hyd_label::data_time));
			HydTemp_Profile::erg_instat_table->create_spatial_index2column(ptr_database, HydTemp_Profile::erg_instat_table->get_column_name(hyd_label::elemdata_polygon));


			HydTemp_Profile::close_erg_instat_table();
		}
	}

}
//Set the database table for the instationary results of an hydraulic simulation for the river profiles: it sets the table name and the name of the columns and allocate them
void HydTemp_Profile::set_erg_instat_table(QSqlDatabase *ptr_database, const bool not_close) {
	if (Sys_Project::get_project_type() == _sys_project_type::proj_hyd_temp) {
		if (HydTemp_Profile::erg_instat_table == NULL) {
			//make specific input for this class
			const string tab_id_name = hyd_label::tab_tempprof_instat_erg_max;

			string tab_id_col[17];
			tab_id_col[0] = label::glob_id;
			tab_id_col[1] = hyd_label::profdata_rvno;
			tab_id_col[2] = label::areastate_id;
			tab_id_col[3] = label::measure_id;
			tab_id_col[4] = label::applied_flag;
			tab_id_col[5] = hyd_label::sz_bound_id;
			tab_id_col[6] = risk_label::sz_break_id;
			tab_id_col[7] = hyd_label::proferg_T_water;
			tab_id_col[8] = hyd_label::air_temp;
			tab_id_col[9] = hyd_label::solar_rad;
			tab_id_col[10] = hyd_label::humidity;
			tab_id_col[11] = hyd_label::wind;
			tab_id_col[12] = hyd_label::cloud;
			tab_id_col[13] = hyd_label::shadow;
			tab_id_col[14] = hyd_label::inlet_temp;
			tab_id_col[15] = hyd_label::proferg_polygon;
			tab_id_col[16] = hyd_label::data_time;




			try {
				HydTemp_Profile::erg_instat_table = new Tables(tab_id_name, tab_id_col, sizeof(tab_id_col) / sizeof(tab_id_col[0]));
				HydTemp_Profile::erg_instat_table->set_name(ptr_database, _sys_table_type::hyd);
			}
			catch (bad_alloc& t) {
				Error msg;
				msg.set_msg("HydTemp_Profile::set_erg_instat_table(QSqlDatabase *ptr_database)", "Can not allocate the memory", "Check the memory", 10, false);
				ostringstream info;
				info << "Info bad alloc: " << t.what() << endl;
				msg.make_second_info(info.str());
				throw msg;
			}
			catch (Error msg) {
				if (not_close == false) {
					HydTemp_Profile::close_erg_instat_table();
				}
				throw msg;
			}
		}

	}
}
//Close and delete the database table for the instationary results of an hydraulic simulation the river profiles
void HydTemp_Profile::close_erg_instat_table(void) {
	if (HydTemp_Profile::erg_instat_table != NULL) {
		delete HydTemp_Profile::erg_instat_table;
		HydTemp_Profile::erg_instat_table = NULL;
	}

}
//Delete the results data in the result database table for a given boundary scenario (static)
void HydTemp_Profile::delete_results_by_scenario(QSqlDatabase *ptr_database, const int sc_id){
	QSqlQueryModel results;

	try{
		HydTemp_Profile::set_erg_table(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}
	ostringstream test_filter;

	test_filter<< "DELETE ";
	test_filter << " FROM " << HydTemp_Profile::erg_table->get_table_name();
	test_filter << " WHERE ";
	test_filter << HydTemp_Profile::erg_table->get_column_name(hyd_label::sz_bound_id) << " = " << sc_id;

	Data_Base::database_request(&results, test_filter.str(), ptr_database);

	//check the request
	if(results.lastError().isValid()){
		Error msg;
		msg.set_msg("HydTemp_Profile::delete_results_by_sceneario(QSqlDatabase *ptr_database, const int sc_id)","Invalid database request", "Check the database", 2, false);
		ostringstream info;
		info << "Table Name      : " << HydTemp_Profile::erg_table->get_table_name() << endl;
		info << "Table error info: " << results.lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
}
//Delete the results data in the maximum result database table for a given system state (static)
void HydTemp_Profile::delete_results_by_system_state(QSqlDatabase *ptr_database, const _sys_system_id id){
	QSqlQueryModel results;

	try{
		HydTemp_Profile::set_erg_table(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}
	ostringstream test_filter;

	test_filter<< "DELETE ";
	test_filter << " FROM " << HydTemp_Profile::erg_table->get_table_name();
	test_filter << " WHERE ";
	test_filter << HydTemp_Profile::erg_table->get_column_name(label::areastate_id) << " ="  << id.area_state;
	test_filter << " AND ";
	test_filter << HydTemp_Profile::erg_table->get_column_name(label::measure_id) << " = " << id.measure_nr;

	Data_Base::database_request(&results, test_filter.str(), ptr_database);

	//check the request
	if(results.lastError().isValid()){
		Error msg;
		msg.set_msg("HydTemp_Profile::delete_results_by_sceneario(QSqlDatabase *ptr_database, const _sys_system_id id)","Invalid database request", "Check the database", 2, false);
		ostringstream info;
		info << "Table Name      : " << HydTemp_Profile::erg_table->get_table_name() << endl;
		info << "Table error info: " << results.lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
}
//Delete the results data in the maximum result database table for specific parameters (static)
void HydTemp_Profile::delete_results_in_table(QSqlDatabase *ptr_database, const _sys_system_id id, const string break_sz, const bool like_flag){
		QSqlQueryModel results;

	try{
		HydTemp_Profile::set_erg_table(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}
	ostringstream test_filter;

	test_filter<< "DELETE ";
	test_filter << " FROM " << HydTemp_Profile::erg_table->get_table_name();
	test_filter << " WHERE ";
	test_filter << HydTemp_Profile::erg_table->get_column_name(label::areastate_id) << " ="  << id.area_state;
	test_filter << " AND ";
	test_filter << HydTemp_Profile::erg_table->get_column_name(label::measure_id) << " = " << id.measure_nr;
	test_filter << " AND ";
	if(like_flag==false){
		test_filter << HydTemp_Profile::erg_table->get_column_name(risk_label::sz_break_id) << " = '" << break_sz<<"'";
	}
	else{
		test_filter << HydTemp_Profile::erg_table->get_column_name(risk_label::sz_break_id) << " LIKE '" << break_sz<<"'";
	}

	Data_Base::database_request(&results, test_filter.str(), ptr_database);

	//check the request
	if(results.lastError().isValid()){
		Error msg;
		msg.set_msg("HydTemp_Profile::delete_results_in_table(QSqlDatabase *ptr_database, const _sys_system_id id, const string break_sz, const bool like_flag=false)","Invalid database request", "Check the database", 2, false);
		ostringstream info;
		info << "Table Name      : " << HydTemp_Profile::erg_table->get_table_name() << endl;
		info << "Table error info: " << results.lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
}
//Delete the results data in the maximum result database table for specific parameters (static)
void HydTemp_Profile::delete_results_in_table(QSqlDatabase *ptr_database, const _sys_system_id id, const int bound_sz, const string break_sz, const bool like_flag){
			QSqlQueryModel results;

	try{
		HydTemp_Profile::set_erg_table(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}
	ostringstream test_filter;

	test_filter<< "DELETE ";
	test_filter << " FROM " << HydTemp_Profile::erg_table->get_table_name();
	test_filter << " WHERE ";
	test_filter << HydTemp_Profile::erg_table->get_column_name(label::areastate_id) << " ="  << id.area_state;
	test_filter << " AND ";
	test_filter << HydTemp_Profile::erg_table->get_column_name(label::measure_id) << " = " << id.measure_nr;
	test_filter << " AND ";
	test_filter << HydTemp_Profile::erg_table->get_column_name(hyd_label::sz_bound_id) << " = " << bound_sz;
	test_filter << " AND ";
	if(like_flag==false){
		test_filter << HydTemp_Profile::erg_table->get_column_name(risk_label::sz_break_id) << " = '" << break_sz<<"'";
	}
	else{
		test_filter << HydTemp_Profile::erg_table->get_column_name(risk_label::sz_break_id) << " LIKE '" << break_sz<<"'";
	}

	Data_Base::database_request(&results, test_filter.str(), ptr_database);

	//check the request
	if(results.lastError().isValid()){
		Error msg;
		msg.set_msg("HydTemp_Profile::delete_results_in_table(QSqlDatabase *ptr_database, const _sys_system_id id, const int bound_sz, const string break_sz, const bool like_flag=false)","Invalid database request", "Check the database", 2, false);
		ostringstream info;
		info << "Table Name      : " << HydTemp_Profile::erg_table->get_table_name() << endl;
		info << "Table error info: " << results.lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
}
//Delete the instationary results data in the maximum result database table for a given boundary scenario
void HydTemp_Profile::delete_instat_results_by_scenario(QSqlDatabase *ptr_database, const int sc_id) {
	QSqlQueryModel results;

	try {
		HydTemp_Profile::set_erg_instat_table(ptr_database);
	}
	catch (Error msg) {
		throw msg;
	}
	ostringstream test_filter;

	test_filter << "DELETE ";
	test_filter << " FROM " << HydTemp_Profile::erg_instat_table->get_table_name();
	test_filter << " WHERE ";
	test_filter << HydTemp_Profile::erg_instat_table->get_column_name(hyd_label::sz_bound_id) << " = " << sc_id;

	Data_Base::database_request(&results, test_filter.str(), ptr_database);

	//check the request
	if (results.lastError().isValid()) {
		Error msg;
		msg.set_msg("HydTemp_Profile::delete_instat_results_by_sceneario(QSqlDatabase *ptr_database, const int sc_id)", "Invalid database request", "Check the database", 2, false);
		ostringstream info;
		info << "Table Name      : " << HydTemp_Profile::erg_instat_table->get_table_name() << endl;
		info << "Table error info: " << results.lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}

}
//Delete the instationary results data in the maximum result database table for a given system state
void HydTemp_Profile::delete_instat_results_by_system_state(QSqlDatabase *ptr_database, const _sys_system_id id) {
	QSqlQueryModel results;

	try {
		HydTemp_Profile::set_erg_instat_table(ptr_database);
	}
	catch (Error msg) {
		throw msg;
	}
	ostringstream test_filter;

	test_filter << "DELETE ";
	test_filter << " FROM " << HydTemp_Profile::erg_instat_table->get_table_name();
	test_filter << " WHERE ";
	test_filter << HydTemp_Profile::erg_instat_table->get_column_name(label::areastate_id) << " =" << id.area_state;
	test_filter << " AND ";
	test_filter << HydTemp_Profile::erg_instat_table->get_column_name(label::measure_id) << " = " << id.measure_nr;

	Data_Base::database_request(&results, test_filter.str(), ptr_database);

	//check the request
	if (results.lastError().isValid()) {
		Error msg;
		msg.set_msg("HydTemp_Profile::delete_instat_results_by_sceneario(QSqlDatabase *ptr_database, const _sys_system_id id)", "Invalid database request", "Check the database", 2, false);
		ostringstream info;
		info << "Table Name      : " << HydTemp_Profile::erg_instat_table->get_table_name() << endl;
		info << "Table error info: " << results.lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}

}
//Delete the instationary results data in the maximum result database table for specific parameters
void HydTemp_Profile::delete_instat_results_in_table(QSqlDatabase *ptr_database, const _sys_system_id id, const string break_sz, const bool like_flag) {
	QSqlQueryModel results;

	try {
		HydTemp_Profile::set_erg_instat_table(ptr_database);
	}
	catch (Error msg) {
		throw msg;
	}
	ostringstream test_filter;

	test_filter << "DELETE ";
	test_filter << " FROM " << HydTemp_Profile::erg_instat_table->get_table_name();
	test_filter << " WHERE ";
	test_filter << HydTemp_Profile::erg_instat_table->get_column_name(label::areastate_id) << " =" << id.area_state;
	test_filter << " AND ";
	test_filter << HydTemp_Profile::erg_instat_table->get_column_name(label::measure_id) << " = " << id.measure_nr;
	test_filter << " AND ";
	if (like_flag == false) {
		test_filter << HydTemp_Profile::erg_instat_table->get_column_name(risk_label::sz_break_id) << " = '" << break_sz << "'";
	}
	else {
		test_filter << HydTemp_Profile::erg_instat_table->get_column_name(risk_label::sz_break_id) << " LIKE '" << break_sz << "'";
	}

	Data_Base::database_request(&results, test_filter.str(), ptr_database);

	//check the request
	if (results.lastError().isValid()) {
		Error msg;
		msg.set_msg("HydTemp_Profile::delete_instat_results_in_table(QSqlDatabase *ptr_database, const _sys_system_id id, const string break_sz, const bool like_flag=false)", "Invalid database request", "Check the database", 2, false);
		ostringstream info;
		info << "Table Name      : " << HydTemp_Profile::erg_instat_table->get_table_name() << endl;
		info << "Table error info: " << results.lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}

}
//Delete the instationaryresults data in the maximum result database table for specific parameters
void HydTemp_Profile::delete_instat_results_in_table(QSqlDatabase *ptr_database, const _sys_system_id id, const int bound_sz, const string break_sz, const bool like_flag) {
	QSqlQueryModel results;

	try {
		HydTemp_Profile::set_erg_instat_table(ptr_database);
	}
	catch (Error msg) {
		throw msg;
	}
	ostringstream test_filter;

	test_filter << "DELETE ";
	test_filter << " FROM " << HydTemp_Profile::erg_instat_table->get_table_name();
	test_filter << " WHERE ";
	test_filter << HydTemp_Profile::erg_instat_table->get_column_name(label::areastate_id) << " =" << id.area_state;
	test_filter << " AND ";
	test_filter << HydTemp_Profile::erg_instat_table->get_column_name(label::measure_id) << " = " << id.measure_nr;
	test_filter << " AND ";
	test_filter << HydTemp_Profile::erg_instat_table->get_column_name(hyd_label::sz_bound_id) << " = " << bound_sz;
	test_filter << " AND ";
	if (like_flag == false) {
		test_filter << HydTemp_Profile::erg_instat_table->get_column_name(risk_label::sz_break_id) << " = '" << break_sz << "'";
	}
	else {
		test_filter << HydTemp_Profile::erg_instat_table->get_column_name(risk_label::sz_break_id) << " LIKE '" << break_sz << "'";
	}

	Data_Base::database_request(&results, test_filter.str(), ptr_database);

	//check the request
	if (results.lastError().isValid()) {
		Error msg;
		msg.set_msg("HydTemp_Profile::delete_instat_results_in_table(QSqlDatabase *ptr_database, const _sys_system_id id, const int bound_sz, const string break_sz, const bool like_flag=false)", "Invalid database request", "Check the database", 2, false);
		ostringstream info;
		info << "Table Name      : " << HydTemp_Profile::erg_instat_table->get_table_name() << endl;
		info << "Table error info: " << results.lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}

}
//Delete the profiles and appending data by a system id (static);
void HydTemp_Profile::delete_profile_in_table(QSqlDatabase *ptr_database, const _sys_system_id id){
	int number=0;
	QSqlQueryModel model;
	number=HydTemp_Profile::select_profiles_in_database(&model, ptr_database, id);

	int glob_id=0;
	for(int i=0; i<number; i++){
		glob_id=model.record(i).value((HydTemp_Profile::profile_table->get_column_name(hyd_label::profdata_glob_id)).c_str()).toInt();

		//delete the appended tables
		HydTemp_Profile::delete_boundary_cond_by_profile(ptr_database, glob_id);
	}

	//delete the profile data
	QSqlQueryModel results;

	
	ostringstream test_filter;

	test_filter<< "Delete ";
	test_filter << " from " << HydTemp_Profile::profile_table->get_table_name();
	test_filter  << " WHERE ";
	test_filter << HydTemp_Profile::profile_table->get_column_name(label::areastate_id) << " = "  << id.area_state;
	test_filter  << " AND ";
	test_filter << HydTemp_Profile::profile_table->get_column_name(label::measure_id) << " = "  << id.measure_nr;

	Data_Base::database_request(&results, test_filter.str(), ptr_database);

	//check the request
	if(results.lastError().isValid()){
		Error msg;
		msg.set_msg("delete_profile_in_table(QSqlDatabase *ptr_database, const _sys_system_id id)","Invalid database request", "Check the database", 2, false);
		ostringstream info;
		info << "Table Name      : " << HydTemp_Profile::profile_table->get_table_name() << endl;
		info << "Table error info: " << results.lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}

}
//Delete the boundary condition data in the database table for a given boundary scenario (static)
void HydTemp_Profile::delete_boundary_cond_by_scenario(QSqlDatabase *ptr_database, const int sc_id){
	QSqlQueryModel results;

	try{
		HydTemp_Profile::set_profile_boundary_table(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}
	ostringstream test_filter;

	test_filter<< "Delete ";
	test_filter << " from " << HydTemp_Profile::boundary_table->get_table_name();
	test_filter << " where ";
	test_filter << HydTemp_Profile::boundary_table->get_column_name(hyd_label::sz_bound_id) << " = " << sc_id;

	Data_Base::database_request(&results, test_filter.str(), ptr_database);

	//check the request
	if(results.lastError().isValid()){
		Error msg;
		msg.set_msg("HydTemp_Profile::delete_boundary_cond_by_scenario(QSqlDatabase *ptr_database, const int sc_id)","Invalid database request", "Check the database", 2, false);
		ostringstream info;
		info << "Table Name      : " << HydTemp_Profile::boundary_table->get_table_name() << endl;
		info << "Table error info: " << results.lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
}
//Delete the boundary condition data in the database table for a given system state (static)
void HydTemp_Profile::delete_boundary_cond_by_system_state(QSqlDatabase *ptr_database, const _sys_system_id id){
	QSqlQueryModel results;

	try{
		HydTemp_Profile::set_profile_boundary_table(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}
	ostringstream test_filter;

	test_filter<< "Delete ";
	test_filter << " from " << HydTemp_Profile::boundary_table->get_table_name();
	test_filter  << " WHERE ";
	test_filter << HydTemp_Profile::boundary_table->get_column_name(label::areastate_id) << " = "  << id.area_state;
	test_filter  << " AND ";
	test_filter  <<  HydTemp_Profile::boundary_table->get_column_name(label::measure_id) << " = " << id.measure_nr ;

	Data_Base::database_request(&results, test_filter.str(), ptr_database);

	//check the request
	if(results.lastError().isValid()){
		Error msg;
		msg.set_msg("HydTemp_Profile::delete_boundary_cond_by_scenario(QSqlDatabase *ptr_database, const _sys_system_id id)","Invalid database request", "Check the database", 2, false);
		ostringstream info;
		info << "Table Name      : " << HydTemp_Profile::boundary_table->get_table_name() << endl;
		info << "Table error info: " << results.lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
}
//Delete the boundary condition data in the database table for a given global profile id (static)
void HydTemp_Profile::delete_boundary_cond_by_profile(QSqlDatabase *ptr_database, const int id) {

	QSqlQueryModel results;

	try {
		HydTemp_Profile::set_profile_boundary_table(ptr_database);
	}
	catch (Error msg) {
		throw msg;
	}
	ostringstream test_filter;

	test_filter << "Delete ";
	test_filter << " from " << HydTemp_Profile::boundary_table->get_table_name();
	test_filter << " WHERE ";
	test_filter << HydTemp_Profile::boundary_table->get_column_name(hyd_label::profdata_glob_id) << " = " << id;

	Data_Base::database_request(&results, test_filter.str(), ptr_database);

	//check the request
	if (results.lastError().isValid()) {
		Error msg;
		msg.set_msg("HydTemp_Profile::delete_boundary_cond_by_rv_profile(QSqlDatabase *ptr_database, const int id)", "Invalid database request", "Check the database", 2, false);
		ostringstream info;
		info << "Table Name      : " << HydTemp_Profile::boundary_table->get_table_name() << endl;
		info << "Table error info: " << results.lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}


}
//Select the data in the database table for the maximum results of the river profiles specified by the system id and the scenario-ids (static)
int HydTemp_Profile::select_data_in_erg_table(QSqlQueryModel *query, QSqlDatabase *ptr_database, const _sys_system_id id, const int bound_sz, const string break_sz , const bool like_flag){
	//the table is set (the name and the column names) and allocated
	try{
		HydTemp_Profile::set_erg_table(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}

	ostringstream query_string;
	query_string <<"SELECT ";
	query_string <<  HydTemp_Profile::erg_table->get_column_name(hyd_label::profdata_rvno)  <<" , ";
	query_string <<  HydTemp_Profile::erg_table->get_column_name(hyd_label::profdata_prof_id)  <<" , ";


	query_string <<  HydTemp_Profile::erg_table->get_column_name(hyd_label::proferg_T_max)  <<" , ";
	query_string <<  HydTemp_Profile::erg_table->get_column_name(hyd_label::proferg_t_T_max)  <<" , ";
	query_string <<  HydTemp_Profile::erg_table->get_column_name(hyd_label::proferg_T_min)  <<" , ";
	query_string <<  HydTemp_Profile::erg_table->get_column_name(hyd_label::proferg_t_T_min)  <<" , ";
	query_string << HydTemp_Profile::erg_table->get_column_name(hyd_label::proferg_T_av) << " , ";


	query_string <<  HydTemp_Profile::erg_table->get_column_name(risk_label::sz_break_id)  <<" , ";
	query_string <<  HydTemp_Profile::erg_table->get_column_name(label::glob_id);

	query_string << " FROM " << HydTemp_Profile::erg_table->get_table_name();
	query_string << " WHERE " ;
	query_string  << HydTemp_Profile::erg_table->get_column_name(label::applied_flag) << "= true";
	query_string  << " AND ";
	query_string  << HydTemp_Profile::erg_table->get_column_name(label::areastate_id) << " ="  << id.area_state;
	query_string  << " AND (";
	query_string  <<  HydTemp_Profile::erg_table->get_column_name(label::measure_id) << " = " << 0 ;
	query_string  << " OR " ;
	query_string  << HydTemp_Profile::erg_table->get_column_name(label::measure_id) << " = " << id.measure_nr;
	query_string  << " ) " ;
	query_string << " AND ";
	query_string << HydTemp_Profile::erg_table->get_column_name(hyd_label::sz_bound_id) << " = " << bound_sz;
	query_string << " AND ";
	if(like_flag==false){
		query_string << HydTemp_Profile::erg_table->get_column_name(risk_label::sz_break_id) << " = '" << break_sz<<"'";
	}
	else{
		query_string << HydTemp_Profile::erg_table->get_column_name(risk_label::sz_break_id) << " LIKE '" << break_sz<<"'";
	}

	Data_Base::database_request(query, query_string.str(), ptr_database);
	if(query->lastError().isValid()==true){
		Error msg;
		msg.set_msg("HydTemp_Profile::select_data_in_erg_table(QSqlQueryModel *query, QSqlDatabase *ptr_database, const _sys_system_id id, const int bound_sz, const string break_sz )","Invalid database request", "Check the database", 2, false);
		ostringstream info;
		info << "Table Name      : " << HydTemp_Profile::erg_table->get_table_name() << endl;
		info << "Table error info: " << query->lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
	return query->rowCount();
}
//Select the data in the database table for the maximum results of the river profiles specified by the system state, the scenario-ids, the river number and a specific profile (static)
int HydTemp_Profile::select_data_in_erg_table(QSqlQueryModel *query, QSqlDatabase *ptr_database, const _sys_system_id id, const int bound_sz, const string break_sz, const int rv_no, const int prof_no){
		//the table is set (the name and the column names) and allocated
	try{
		HydTemp_Profile::set_erg_table(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}

	ostringstream query_string;
	query_string <<"SELECT ";
	query_string <<  HydTemp_Profile::erg_table->get_column_name(hyd_label::profdata_rvno)  <<" , ";
	query_string <<  HydTemp_Profile::erg_table->get_column_name(hyd_label::profdata_prof_id)  <<" , ";
	query_string << HydTemp_Profile::erg_table->get_column_name(hyd_label::proferg_T_max) << " , ";
	query_string << HydTemp_Profile::erg_table->get_column_name(hyd_label::proferg_t_T_max) << " , ";
	query_string << HydTemp_Profile::erg_table->get_column_name(hyd_label::proferg_T_min) << " , ";
	query_string << HydTemp_Profile::erg_table->get_column_name(hyd_label::proferg_t_T_min) << " , ";
	query_string << HydTemp_Profile::erg_table->get_column_name(hyd_label::proferg_T_av) << " , ";
	query_string <<  HydTemp_Profile::erg_table->get_column_name(risk_label::sz_break_id)  <<" , ";
	query_string <<  HydTemp_Profile::erg_table->get_column_name(label::glob_id);

	query_string << " FROM " << HydTemp_Profile::erg_table->get_table_name();
	query_string << " WHERE " ;
	query_string  << HydTemp_Profile::erg_table->get_column_name(label::applied_flag) << "= true";
	query_string  << " AND ";
	query_string  << HydTemp_Profile::erg_table->get_column_name(label::areastate_id) << " ="  << id.area_state;
	query_string  << " AND (";
	query_string  <<  HydTemp_Profile::erg_table->get_column_name(label::measure_id) << " = " << 0 ;
	query_string  << " OR " ;
	query_string  << HydTemp_Profile::erg_table->get_column_name(label::measure_id) << " = " << id.measure_nr;
	query_string  << " ) " ;
	query_string << " AND ";
	query_string << HydTemp_Profile::erg_table->get_column_name(hyd_label::sz_bound_id) << " = " << bound_sz;
	query_string << " AND ";
	query_string << HydTemp_Profile::erg_table->get_column_name(risk_label::sz_break_id) << " = '" << break_sz<<"'";
	query_string << " AND ";
	query_string << HydTemp_Profile::erg_table->get_column_name(hyd_label::profdata_rvno) << " = " << rv_no;
	query_string << " AND ";
	query_string << HydTemp_Profile::erg_table->get_column_name(hyd_label::profdata_prof_id) << " = " << prof_no;

	Data_Base::database_request(query, query_string.str(), ptr_database);
	if(query->lastError().isValid()==true){
		Error msg;
		msg.set_msg("HydTemp_Profile::select_data_in_erg_table(QSqlQueryModel *query, QSqlDatabase *ptr_database, const _sys_system_id id, const int bound_sz, const string break_sz, const int rv_no, const int prof_no)","Invalid database request", "Check the database", 2, false);
		ostringstream info;
		info << "Table Name      : " << HydTemp_Profile::erg_table->get_table_name() << endl;
		info << "Table error info: " << query->lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}

	return query->rowCount();
}
//Select the data in the database table for the instationary results of the river profiles specified by the system state and the scenario-ids (static)
int HydTemp_Profile::select_data_in_instat_erg_table(QSqlQueryModel *query, QSqlDatabase *ptr_database, const _sys_system_id id, const int bound_sz, const string break_sz, const bool like_flag) {


	//the table is set (the name and the column names) and allocated
	try {
		HydTemp_Profile::set_erg_instat_table(ptr_database);
	}
	catch (Error msg) {
		throw msg;
	}

	ostringstream query_string;
	query_string << "SELECT ";
	query_string << HydTemp_Profile::erg_instat_table->get_column_name(hyd_label::profdata_rvno) << " , ";


	query_string << HydTemp_Profile::erg_instat_table->get_column_name(hyd_label::proferg_T_water) << " , ";

	query_string << HydTemp_Profile::erg_instat_table->get_column_name(risk_label::sz_break_id) << " , ";
	query_string << HydTemp_Profile::erg_instat_table->get_column_name(hyd_label::data_time) << " , ";
	query_string << HydTemp_Profile::erg_instat_table->get_column_name(label::glob_id);


	query_string << " FROM " << HydTemp_Profile::erg_instat_table->get_table_name();
	query_string << " WHERE ";
	query_string << HydTemp_Profile::erg_instat_table->get_column_name(label::applied_flag) << "= true";
	query_string << " AND ";
	query_string << HydTemp_Profile::erg_instat_table->get_column_name(label::areastate_id) << " =" << id.area_state;
	query_string << " AND (";
	query_string << HydTemp_Profile::erg_instat_table->get_column_name(label::measure_id) << " = " << 0;
	query_string << " OR ";
	query_string << HydTemp_Profile::erg_instat_table->get_column_name(label::measure_id) << " = " << id.measure_nr;
	query_string << " ) ";
	query_string << " AND ";
	query_string << HydTemp_Profile::erg_instat_table->get_column_name(hyd_label::sz_bound_id) << " = " << bound_sz;
	query_string << " AND ";
	if (like_flag == false) {
		query_string << HydTemp_Profile::erg_instat_table->get_column_name(risk_label::sz_break_id) << " = '" << break_sz << "'";
	}
	else {
		query_string << HydTemp_Profile::erg_instat_table->get_column_name(risk_label::sz_break_id) << " LIKE '" << break_sz << "'";
	}

	Data_Base::database_request(query, query_string.str(), ptr_database);
	if (query->lastError().isValid() == true) {
		Error msg;
		msg.set_msg("HydTemp_Profile::select_data_in_instat_erg_table(QSqlQueryModel *query, QSqlDatabase *ptr_database, const _sys_system_id id, const int bound_sz, const string break_sz )", "Invalid database request", "Check the database", 2, false);
		ostringstream info;
		info << "Table Name      : " << HydTemp_Profile::erg_instat_table->get_table_name() << endl;
		info << "Table error info: " << query->lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
	return query->rowCount();


}
//Delete all data in the database of all tables for the river profile data (static)
void HydTemp_Profile::delete_data_in_table(QSqlDatabase *ptr_database){
	//delete all tables
	if (Sys_Project::get_project_type() == _sys_project_type::proj_hyd_temp) {
		HydTemp_Profile::delete_data_in_profile_table(ptr_database);
		HydTemp_Profile::delete_data_in_boundary_table(ptr_database);

		HydTemp_Profile::delete_data_in_erg_table(ptr_database);
		HydTemp_Profile::delete_data_in_erg_instat_table(ptr_database);
	}


}
//Close and delete the database of all tables for the river profile data (static)
void HydTemp_Profile::close_table(void){
	//close all tables
	HydTemp_Profile::close_profile_table();
	HydTemp_Profile::close_boundary_table();

	HydTemp_Profile::close_erg_table();
	HydTemp_Profile::close_erg_instat_table();


}
//Switch the applied-flag for the 1-d river profiles in the database table for a defined system state
void HydTemp_Profile::switch_applied_flag_profile_table(QSqlDatabase *ptr_database, const _sys_system_id id, const bool flag){
	//the table is set (the name and the column names) and allocated
	try{
		HydTemp_Profile::set_profile_table(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}
	QSqlQueryModel query;

	ostringstream query_string;
	query_string <<"UPDATE ";
	query_string << HydTemp_Profile::profile_table->get_table_name();
	query_string << " SET " ;
	query_string  << HydTemp_Profile::profile_table->get_column_name(label::applied_flag) << " = "<< functions::convert_boolean2string(flag);
	query_string  << " WHERE ";
	query_string  << HydTemp_Profile::profile_table->get_column_name(label::areastate_id) << " = "  << id.area_state;
	query_string  << " AND ";
	query_string  <<  HydTemp_Profile::profile_table->get_column_name(label::measure_id) << " = " << id.measure_nr ;
	query_string  << " AND ";
	query_string  << HydTemp_Profile::profile_table->get_column_name(label::applied_flag) << " = "<< functions::convert_boolean2string(!flag);


	Data_Base::database_request(&query, query_string.str(), ptr_database);
	if(query.lastError().isValid()==true){
		Error msg;
		msg.set_msg("HydTemp_Profile::switch_applied_flag_profile_table(QSqlDatabase *ptr_database, const _sys_system_id id, const bool flag)","Invalid database request", "Check the database", 2, false);
		ostringstream info;
		info << "Table Name      : " << HydTemp_Profile::profile_table->get_table_name() << endl;
		info << "Table error info: " << query.lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
}
//Switch the applied-flag for the 1-d river profiles in the database table for a defined system state and river number (static)
void HydTemp_Profile::switch_applied_flag_profile_table(QSqlDatabase *ptr_database, const _sys_system_id id, const int rv_no, const bool flag){

	//the table is set (the name and the column names) and allocated
	try{
		HydTemp_Profile::set_profile_table(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}
	QSqlQueryModel query;

	ostringstream query_string;
	query_string <<"UPDATE ";
	query_string << HydTemp_Profile::profile_table->get_table_name();
	query_string << " SET " ;
	query_string  << HydTemp_Profile::profile_table->get_column_name(label::applied_flag) << " = "<< functions::convert_boolean2string(flag);
	query_string  << " WHERE ";
	query_string  << HydTemp_Profile::profile_table->get_column_name(label::areastate_id) << " = "  << id.area_state;
	query_string  << " AND ";
	query_string  <<  HydTemp_Profile::profile_table->get_column_name(label::measure_id) << " = " << id.measure_nr ;
	query_string  << " AND ";
	query_string  << HydTemp_Profile::profile_table->get_column_name(label::applied_flag) << " = "<< functions::convert_boolean2string(!flag);
	query_string  << " AND ";
	query_string  << HydTemp_Profile::profile_table->get_column_name(hyd_label::profdata_rvno) << " = "<< rv_no;

	Data_Base::database_request(&query, query_string.str(), ptr_database);
	if(query.lastError().isValid()==true){
		Error msg;
		msg.set_msg("HydTemp_Profile::switch_applied_flag_profile_table(QSqlDatabase *ptr_database, const _sys_system_id id, const int rv_no, const bool flag)","Invalid database request", "Check the database", 2, false);
		ostringstream info;
		info << "Table Name      : " << HydTemp_Profile::profile_table->get_table_name() << endl;
		info << "Table error info: " << query.lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
}
//Switch the applied-flag for the 1-d results in the database table for a defined system state (static)
void HydTemp_Profile::switch_applied_flag_erg_table(QSqlDatabase *ptr_database, const _sys_system_id id, const bool flag){
	//the table is set (the name and the column names) and allocated
	try{
		HydTemp_Profile::set_erg_table(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}
	QSqlQueryModel query;

	ostringstream query_string;
	query_string <<"UPDATE ";
	query_string << HydTemp_Profile::erg_table->get_table_name();
	query_string << " SET " ;
	query_string  << HydTemp_Profile::erg_table->get_column_name(label::applied_flag) << " = "<< functions::convert_boolean2string(flag);
	query_string  << " WHERE ";
	query_string  << HydTemp_Profile::erg_table->get_column_name(label::areastate_id) << " = "  << id.area_state;
	query_string  << " AND ";
	query_string  <<  HydTemp_Profile::erg_table->get_column_name(label::measure_id) << " = " << id.measure_nr ;
	query_string  << " AND ";
	query_string  << HydTemp_Profile::erg_table->get_column_name(label::applied_flag) << " = "<< functions::convert_boolean2string(!flag);

	Data_Base::database_request(&query, query_string.str(), ptr_database);
	if(query.lastError().isValid()==true){
		Error msg;
		msg.set_msg("HydTemp_Profile::switch_applied_flag_erg_table(QSqlDatabase *ptr_database, const _sys_system_id id, const bool flag)","Invalid database request", "Check the database", 2, false);
		ostringstream info;
		info << "Table Name      : " << HydTemp_Profile::erg_table->get_table_name() << endl;
		info << "Table error info: " << query.lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
}
//Switch the applied-flag for the 1-d results in the database table for a defined system state (static)
void HydTemp_Profile::switch_applied_flag_erg_table(QSqlDatabase *ptr_database, const _sys_system_id id, const int hyd_sc, const bool flag){
	//the table is set (the name and the column names) and allocated
	try{
		HydTemp_Profile::set_erg_table(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}
	QSqlQueryModel query;

	ostringstream query_string;
	query_string <<"UPDATE ";
	query_string << HydTemp_Profile::erg_table->get_table_name();
	query_string << " SET " ;
	query_string  << HydTemp_Profile::erg_table->get_column_name(label::applied_flag) << " = "<< functions::convert_boolean2string(flag);
	query_string  << " WHERE ";
	query_string  << HydTemp_Profile::erg_table->get_column_name(label::areastate_id) << " = "  << id.area_state;
	query_string  << " AND ";
	query_string  <<  HydTemp_Profile::erg_table->get_column_name(label::measure_id) << " = " << id.measure_nr ;
	query_string  << " AND ";
	query_string  <<  HydTemp_Profile::erg_table->get_column_name(hyd_label::sz_bound_id) << " = " << hyd_sc ;
	query_string  << " AND ";
	query_string  << HydTemp_Profile::erg_table->get_column_name(label::applied_flag) << " = "<< functions::convert_boolean2string(!flag);

	Data_Base::database_request(&query, query_string.str(), ptr_database);
	if(query.lastError().isValid()==true){
		Error msg;
		msg.set_msg("HydTemp_Profile::switch_applied_flag_erg_table(QSqlDatabase *ptr_database, const _sys_system_id id, const int hyd_sc, const bool flag)","Invalid database request", "Check the database", 2, false);
		ostringstream info;
		info << "Table Name      : " << HydTemp_Profile::erg_table->get_table_name() << endl;
		info << "Table error info: " << query.lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
}
//Switch the applied-flag for the 1-d instationary results in the database table for a defined system state
void HydTemp_Profile::switch_applied_flag_erg_instat_table(QSqlDatabase *ptr_database, const _sys_system_id id, const bool flag) {
	//the table is set (the name and the column names) and allocated
	try {
		HydTemp_Profile::set_erg_instat_table(ptr_database);
	}
	catch (Error msg) {
		throw msg;
	}
	QSqlQueryModel query;

	ostringstream query_string;
	query_string << "UPDATE ";
	query_string << HydTemp_Profile::erg_instat_table->get_table_name();
	query_string << " SET ";
	query_string << HydTemp_Profile::erg_instat_table->get_column_name(label::applied_flag) << " = " << functions::convert_boolean2string(flag);
	query_string << " WHERE ";
	query_string << HydTemp_Profile::erg_instat_table->get_column_name(label::areastate_id) << " = " << id.area_state;
	query_string << " AND ";
	query_string << HydTemp_Profile::erg_instat_table->get_column_name(label::measure_id) << " = " << id.measure_nr;
	query_string << " AND ";
	query_string << HydTemp_Profile::erg_instat_table->get_column_name(label::applied_flag) << " = " << functions::convert_boolean2string(!flag);

	Data_Base::database_request(&query, query_string.str(), ptr_database);
	if (query.lastError().isValid() == true) {
		Error msg;
		msg.set_msg("HydTemp_Profile::switch_applied_flag_erg_instat_table(QSqlDatabase *ptr_database, const _sys_system_id id, const bool flag)", "Invalid database request", "Check the database", 2, false);
		ostringstream info;
		info << "Table Name      : " << HydTemp_Profile::erg_instat_table->get_table_name() << endl;
		info << "Table error info: " << query.lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}

}
//Switch the applied-flag for the 1-d instationbary results in the database table for a defined system state
void HydTemp_Profile::switch_applied_flag_erg_instat_table(QSqlDatabase *ptr_database, const _sys_system_id id, const int hyd_sc, const bool flag) {
	//the table is set (the name and the column names) and allocated
	try {
		HydTemp_Profile::set_erg_instat_table(ptr_database);
	}
	catch (Error msg) {
		throw msg;
	}
	QSqlQueryModel query;

	ostringstream query_string;
	query_string << "UPDATE ";
	query_string << HydTemp_Profile::erg_instat_table->get_table_name();
	query_string << " SET ";
	query_string << HydTemp_Profile::erg_instat_table->get_column_name(label::applied_flag) << " = " << functions::convert_boolean2string(flag);
	query_string << " WHERE ";
	query_string << HydTemp_Profile::erg_instat_table->get_column_name(label::areastate_id) << " = " << id.area_state;
	query_string << " AND ";
	query_string << HydTemp_Profile::erg_instat_table->get_column_name(label::measure_id) << " = " << id.measure_nr;
	query_string << " AND ";
	query_string << HydTemp_Profile::erg_instat_table->get_column_name(hyd_label::sz_bound_id) << " = " << hyd_sc;
	query_string << " AND ";
	query_string << HydTemp_Profile::erg_instat_table->get_column_name(label::applied_flag) << " = " << functions::convert_boolean2string(!flag);

	Data_Base::database_request(&query, query_string.str(), ptr_database);
	if (query.lastError().isValid() == true) {
		Error msg;
		msg.set_msg("HydTemp_Profile::switch_applied_flag_erg_instat_table(QSqlDatabase *ptr_database, const _sys_system_id id, const int hyd_sc, const bool flag)", "Invalid database request", "Check the database", 2, false);
		ostringstream info;
		info << "Table Name      : " << HydTemp_Profile::erg_instat_table->get_table_name() << endl;
		info << "Table error info: " << query.lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}


}
//Switch the applied-flag for the boundary condition in the database table for a defined system state (static)
void HydTemp_Profile::switch_applied_flag_boundary_table(QSqlDatabase *ptr_database, const _sys_system_id id, const bool flag){
	//the table is set (the name and the column names) and allocated
	try{
		HydTemp_Profile::set_profile_boundary_table(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}
	QSqlQueryModel query;

	ostringstream query_string;
	query_string <<"UPDATE ";
	query_string << HydTemp_Profile::boundary_table->get_table_name();
	query_string << " SET " ;
	query_string  << HydTemp_Profile::boundary_table->get_column_name(label::applied_flag) << " = "<< functions::convert_boolean2string(flag);
	query_string  << " WHERE ";
	query_string  << HydTemp_Profile::boundary_table->get_column_name(label::areastate_id) << " = "  << id.area_state;
	query_string  << " AND ";
	query_string  <<  HydTemp_Profile::boundary_table->get_column_name(label::measure_id) << " = " << id.measure_nr ;

	Data_Base::database_request(&query, query_string.str(), ptr_database);
	if(query.lastError().isValid()==true){
		Error msg;
		msg.set_msg("HydTemp_Profile::switch_applied_flag_boundary_table(QSqlDatabase *ptr_database, const _sys_system_id id, const bool flag)","Invalid database request", "Check the database", 2, false);
		ostringstream info;
		info << "Table Name      : " << HydTemp_Profile::boundary_table->get_table_name() << endl;
		info << "Table error info: " << query.lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
}
//Switch the applied-flag for the boundary condition in the database table for a defined system state (static)
void HydTemp_Profile::switch_applied_flag_boundary_table(QSqlDatabase *ptr_database, const _sys_system_id id, const int hyd_sc, const bool flag){
	//the table is set (the name and the column names) and allocated
	try{
		HydTemp_Profile::set_profile_boundary_table(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}
	QSqlQueryModel query;

	ostringstream query_string;
	query_string <<"UPDATE ";
	query_string << HydTemp_Profile::boundary_table->get_table_name();
	query_string << " SET " ;
	query_string  << HydTemp_Profile::boundary_table->get_column_name(label::applied_flag) << " = "<< functions::convert_boolean2string(flag);
	query_string  << " WHERE ";
	query_string  << HydTemp_Profile::boundary_table->get_column_name(label::areastate_id) << " ="  << id.area_state;
	query_string  << " AND ";
	query_string  <<  HydTemp_Profile::boundary_table->get_column_name(label::measure_id) << " = " << id.measure_nr ;
	query_string  << " AND ";
	query_string  <<  HydTemp_Profile::boundary_table->get_column_name(hyd_label::sz_bound_id) << " = " << hyd_sc ;

	Data_Base::database_request(&query, query_string.str(), ptr_database);
	if(query.lastError().isValid()==true){
		Error msg;
		msg.set_msg("HydTemp_Profile::switch_applied_flag_boundary_table(QSqlDatabase *ptr_database, const _sys_system_id id, const int hyd_sc, const bool flag)","Invalid database request", "Check the database", 2, false);
		ostringstream info;
		info << "Table Name      : " << HydTemp_Profile::boundary_table->get_table_name() << endl;
		info << "Table error info: " << query.lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
}
//Copy the results of a given system id to another one in database table (static)
void HydTemp_Profile::copy_results(QSqlDatabase *ptr_database, const _sys_system_id src, const _sys_system_id dest){
	//the table is set (the name and the column names) and allocated
	try{
		HydTemp_Profile::set_erg_table(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}

	int glob_id=0;
	glob_id=HydTemp_Profile::erg_table->maximum_int_of_column(HydTemp_Profile::erg_table->get_column_name(label::glob_id) ,ptr_database)+1;

	QSqlQueryModel model;
	ostringstream test_filter;
	test_filter<< "SELECT ";
	test_filter << HydTemp_Profile::erg_table->get_column_name(label::glob_id);
	test_filter << " from " << HydTemp_Profile::erg_table->get_table_name();
	test_filter << " WHERE ";
	test_filter << HydTemp_Profile::erg_table->get_column_name(label::areastate_id) << " = " << src.area_state;
	test_filter << " AND ";
	test_filter << HydTemp_Profile::erg_table->get_column_name(label::measure_id) << " = " << src.measure_nr ;

	//submit it to the datbase
	Data_Base::database_request(&model, test_filter.str(), ptr_database);
	if(model.lastError().isValid()==true){
		Error msg;
		msg.set_msg("HydTemp_Profile::copy_results(QSqlDatabase *ptr_database, const _sys_system_id src, const _sys_system_id dest)","Invalid database request", "Check the database", 2, false);
		ostringstream info;
		info << "Table Name      : " << HydTemp_Profile::erg_table->get_table_name() << endl;
		info << "Table error info: " << model.lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
	test_filter.str("");

	QSqlQueryModel model1;

	ostringstream cout;
	cout <<"Copy "<<model.rowCount()<<" results of the temperature profile(s) to the new measure state..."<<endl;
	Sys_Common_Output::output_hyd->output_txt(&cout);
	for(int i=0; i< model.rowCount(); i++){
		if(i%1000==0 && i>0){
			cout << "Copy profile's results "<< i <<" to " << i+1000 <<"..."<< endl;
			Sys_Common_Output::output_hyd->output_txt(&cout);
		}
		test_filter.str("");
		test_filter << "INSERT INTO "<< HydTemp_Profile::erg_table->get_table_name();
		test_filter << " SELECT " << glob_id <<" , ";
		test_filter <<  HydTemp_Profile::erg_table->get_column_name(hyd_label::profdata_rvno)  <<" , ";
		test_filter <<  HydTemp_Profile::erg_table->get_column_name(hyd_label::profdata_prof_id)  <<" , ";
		test_filter <<  dest.area_state  <<" , ";
		test_filter <<  dest.measure_nr  <<" , ";
		test_filter <<  HydTemp_Profile::erg_table->get_column_name(label::applied_flag)  <<" , ";
		test_filter <<  HydTemp_Profile::erg_table->get_column_name(hyd_label::sz_bound_id)  <<" , ";
		test_filter <<  HydTemp_Profile::erg_table->get_column_name(risk_label::sz_break_id)  <<" , ";
		test_filter << HydTemp_Profile::erg_table->get_column_name(hyd_label::proferg_T_max) << " , ";
		test_filter << HydTemp_Profile::erg_table->get_column_name(hyd_label::proferg_t_T_max) << " , ";
		test_filter << HydTemp_Profile::erg_table->get_column_name(hyd_label::proferg_T_min) << " , ";
		test_filter << HydTemp_Profile::erg_table->get_column_name(hyd_label::proferg_t_T_min) << " , ";
		test_filter << HydTemp_Profile::erg_table->get_column_name(hyd_label::proferg_T_av) << " , ";
		test_filter <<  HydTemp_Profile::erg_table->get_column_name(hyd_label::proferg_polygon)  <<"  ";
		test_filter << " FROM " << HydTemp_Profile::erg_table->get_table_name() <<" ";
		test_filter << " WHERE " << HydTemp_Profile::erg_table->get_column_name(label::glob_id) << " = ";
		test_filter << model.record(i).value(HydTemp_Profile::erg_table->get_column_name(label::glob_id).c_str()).toInt();
		Data_Base::database_request(&model1, test_filter.str(), ptr_database);
		if(model1.lastError().isValid()==true){
			Error msg;
			msg.set_msg("HydTemp_Profile::copy_results(QSqlDatabase *ptr_database, const _sys_system_id src, const _sys_system_id dest)","Invalid database request", "Check the database", 2, false);
			ostringstream info;
			info << "Table Name      : " << HydTemp_Profile::erg_table->get_table_name() << endl;
			info << "Table error info: " << model1.lastError().text().toStdString() << endl;
			msg.make_second_info(info.str());
			throw msg;
		}
		glob_id++;
	}
	try{
		HydTemp_Profile::switch_applied_flag_erg_table(ptr_database, src, false);
	}
	catch(Error msg){
		throw msg;
	}
}
//Copy the instationary results of a given system id to another one in database table
void HydTemp_Profile::copy_instat_results(QSqlDatabase *ptr_database, const _sys_system_id src, const _sys_system_id dest) {
	//the table is set (the name and the column names) and allocated
	try {
		HydTemp_Profile::set_erg_instat_table(ptr_database);
	}
	catch (Error msg) {
		throw msg;
	}

	int glob_id = 0;
	glob_id = HydTemp_Profile::erg_instat_table->maximum_int_of_column(HydTemp_Profile::erg_instat_table->get_column_name(label::glob_id), ptr_database) + 1;

	QSqlQueryModel model;
	ostringstream test_filter;
	test_filter << "SELECT ";
	test_filter << HydTemp_Profile::erg_instat_table->get_column_name(label::glob_id);
	test_filter << " from " << HydTemp_Profile::erg_instat_table->get_table_name();
	test_filter << " WHERE ";
	test_filter << HydTemp_Profile::erg_instat_table->get_column_name(label::areastate_id) << " = " << src.area_state;
	test_filter << " AND ";
	test_filter << HydTemp_Profile::erg_instat_table->get_column_name(label::measure_id) << " = " << src.measure_nr;

	//submit it to the datbase
	Data_Base::database_request(&model, test_filter.str(), ptr_database);
	if (model.lastError().isValid() == true) {
		Error msg;
		msg.set_msg("HydTemp_Profile::copy_instat_results(QSqlDatabase *ptr_database, const _sys_system_id src, const _sys_system_id dest)", "Invalid database request", "Check the database", 2, false);
		ostringstream info;
		info << "Table Name      : " << HydTemp_Profile::erg_instat_table->get_table_name() << endl;
		info << "Table error info: " << model.lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
	test_filter.str("");

	QSqlQueryModel model1;

	ostringstream cout;
	cout << "Copy " << model.rowCount() << " instationary results of the temperature profile(s) to the new measure state..." << endl;
	Sys_Common_Output::output_hyd->output_txt(&cout);
	for (int i = 0; i < model.rowCount(); i++) {
		if (i % 1000 == 0 && i > 0) {
			cout << "Copy profile's results " << i << " to " << i + 1000 << "..." << endl;
			Sys_Common_Output::output_hyd->output_txt(&cout);
		}
		test_filter.str("");
		test_filter << "INSERT INTO " << HydTemp_Profile::erg_instat_table->get_table_name();
		test_filter << " SELECT " << glob_id << " , ";
		test_filter << HydTemp_Profile::erg_instat_table->get_column_name(hyd_label::profdata_rvno) << " , ";
		test_filter << dest.area_state << " , ";
		test_filter << dest.measure_nr << " , ";
		test_filter << HydTemp_Profile::erg_instat_table->get_column_name(label::applied_flag) << " , ";
		test_filter << HydTemp_Profile::erg_instat_table->get_column_name(hyd_label::sz_bound_id) << " , ";
		test_filter << HydTemp_Profile::erg_instat_table->get_column_name(risk_label::sz_break_id) << " , ";

		test_filter << HydTemp_Profile::erg_instat_table->get_column_name(hyd_label::air_temp) << " , ";
		test_filter << HydTemp_Profile::erg_instat_table->get_column_name(hyd_label::solar_rad) << " , ";
		test_filter << HydTemp_Profile::erg_instat_table->get_column_name(hyd_label::humidity) << " , ";
		test_filter << HydTemp_Profile::erg_instat_table->get_column_name(hyd_label::wind) << " , ";
		test_filter << HydTemp_Profile::erg_instat_table->get_column_name(hyd_label::cloud) << " , ";
		test_filter << HydTemp_Profile::erg_instat_table->get_column_name(hyd_label::shadow) << " , ";
		test_filter << HydTemp_Profile::erg_instat_table->get_column_name(hyd_label::inlet_temp) << " , ";


		test_filter << HydTemp_Profile::erg_instat_table->get_column_name(hyd_label::data_time) << " , ";
		test_filter << HydTemp_Profile::erg_instat_table->get_column_name(hyd_label::proferg_polygon) << "  ";
		test_filter << " FROM " << HydTemp_Profile::erg_instat_table->get_table_name() << " ";
		test_filter << " WHERE " << HydTemp_Profile::erg_instat_table->get_column_name(label::glob_id) << " = ";
		test_filter << model.record(i).value(HydTemp_Profile::erg_instat_table->get_column_name(label::glob_id).c_str()).toInt();
		Data_Base::database_request(&model1, test_filter.str(), ptr_database);
		if (model1.lastError().isValid() == true) {
			Error msg;
			msg.set_msg("HydTemp_Profile::copy_instat_results(QSqlDatabase *ptr_database, const _sys_system_id src, const _sys_system_id dest)", "Invalid database request", "Check the database", 2, false);
			ostringstream info;
			info << "Table Name      : " << HydTemp_Profile::erg_instat_table->get_table_name() << endl;
			info << "Table error info: " << model1.lastError().text().toStdString() << endl;
			msg.make_second_info(info.str());
			throw msg;
		}
		glob_id++;
	}
	try {
		HydTemp_Profile::switch_applied_flag_erg_instat_table(ptr_database, src, false);
	}
	catch (Error msg) {
		throw msg;
	}

}
//Copy the boundary condition from the source global profile id to the target (static)
void HydTemp_Profile::copy_boundary_condition(QSqlDatabase *ptr_database, const _sys_system_id base, const int src, const int targ){

	//the table is set (the name and the column names) and allocated
	try{
		HydTemp_Profile::set_profile_boundary_table(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}


	ostringstream test_filter;

	QSqlQueryModel model1;

	test_filter.str("");
	test_filter << "INSERT INTO "<< HydTemp_Profile::boundary_table->get_table_name();
	test_filter << " SELECT " << targ <<" , ";
	test_filter <<  HydTemp_Profile::boundary_table->get_column_name(label::areastate_id)  <<" , ";
	test_filter <<  HydTemp_Profile::boundary_table->get_column_name(label::measure_id)  <<" , ";
	test_filter <<  functions::convert_boolean2string(true)  <<" , ";
	test_filter <<  HydTemp_Profile::boundary_table->get_column_name(hyd_label::sz_bound_id)  <<" , ";
	test_filter <<  HydTemp_Profile::boundary_table->get_column_name(hyd_label::bounddata_stat)  <<" , ";
	test_filter <<  HydTemp_Profile::boundary_table->get_column_name(hyd_label::bounddata_value)  <<" , ";
	test_filter << HydTemp_Profile::boundary_table->get_column_name(hyd_label::bounddata_name) << " , ";
	test_filter <<  HydTemp_Profile::boundary_table->get_column_name(hyd_label::bounddata_type)  <<"  ";


	test_filter << " FROM " <<  HydTemp_Profile::boundary_table->get_table_name() <<" ";
	test_filter << " WHERE ";
	test_filter << HydTemp_Profile::boundary_table->get_column_name(label::areastate_id) << " = " << base.area_state;
	test_filter << " AND ";
	test_filter << HydTemp_Profile::boundary_table->get_column_name(label::measure_id) << " = " << base.measure_nr ;
	test_filter << " AND ";
	test_filter << HydTemp_Profile::boundary_table->get_column_name(hyd_label::profdata_glob_id) << " = " << src ;

		
		
	Data_Base::database_request(&model1, test_filter.str(), ptr_database);
	if(model1.lastError().isValid()==true){
		Error msg;
		msg.set_msg("HydTemp_Profile::copy_boundary_condition(QSqlDatabase *ptr_database, const _sys_system_id base, const int src, const int targ)","Invalid database request", "Check the database", 2, false);
		ostringstream info;
		info << "Table Name      : " << HydTemp_Profile::boundary_table->get_table_name() << endl;
		info << "Table error info: " << model1.lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}



}
//Get the Init-condition as an area from the class HydTemp_Profile_Type
double HydTemp_Profile::get_init_condition_value(void){
	return this->init_condition;
}
//Set the init-condition to the current value
void HydTemp_Profile::set_init_condition(void) {
	this->temp_current = this->init_condition;

}
//Clone river profile
void HydTemp_Profile::clone_profile(HydTemp_Profile *profile){
	this->name=profile->name;
	this->hyd_sz=profile->hyd_sz;
	this->system_id=profile->get_sys_system_id();
	
	this->profile_number=profile->profile_number;

	this->init_condition=profile->init_condition;

	this->air_temp = profile->air_temp;
	this->solar_rad = profile->solar_rad;
	this->humid = profile->humid;
	this->wind = profile->wind;
	this->cloud = profile->cloud;
	this->shadow = profile->inlet_temp;
	this->water_bound_temp = profile->water_bound_temp;


	this->distance2up = profile->distance2up;

	this->temp_value_max = profile->temp_value_max;

	this->temp_value_min = profile->temp_value_min;
	this->temp_average = profile->temp_average;
	this->temp_current = profile->temp_current;


}
//Get the profile number
int HydTemp_Profile::get_profile_number(void){
	return this->profile_number;
}
//Get the profile name
string HydTemp_Profile::get_profile_name(void){
	return this->name;
}
//Get the actual water temperature 
double HydTemp_Profile::get_actual_temperature(void) {
	return this->temp_current;
}
//Set the actuel temperature from solver
void HydTemp_Profile::set_actuel_temp_from_solver(const double temp) {
	this->temp_current = temp;
	if (this->water_bound_temp.applied_flag == true || this->inlet_temp.applied_flag==true) {
		this->temp_current = this->water_bound_temp.current_value;
	}
	else if (this->inlet_temp.applied_flag == true) {
		//change here for inlet!
		this->temp_current = temp;

	}
}
//Get if the water temperature boundary is applied 
bool HydTemp_Profile::get_water_temperature_applied(void) {
	return this->water_bound_temp.applied_flag;

}
//Get if the inlet temperature boundary is applied 
bool HydTemp_Profile::get_inlet_temperature_applied(void) {
	return this->inlet_temp.applied_flag;

}
//Make the syncronistation of the elements to a given time point
void HydTemp_Profile::make_syncronisation(const int index,  const double time_point, HydTemp_Param *params, HydTemp_Profile *upstream, HydTemp_Profile *downstream, Hyd_River_Profile_Connection_Standard *rv_profile) {

	if (this->water_bound_temp.applied_flag == true) {
		if (this->water_bound_temp.stat_flag == true) {
			//stationary
			this->water_bound_temp.current_value = this->water_bound_temp.index_value;

		}
		else {
			//instationary
			this->water_bound_temp.current_value = this->water_bound_temp.ptr_curve->calculate_actuel_boundary_value(time_point);
		}
		this->discharge_current = this->bound_discharge.calculate_actuel_boundary_value(time_point);
		this->waterdepth_current = this->bound_water_depth.calculate_actuel_boundary_value(time_point);
		this->flow_velocity_current = this->bound_velocity.calculate_actuel_boundary_value(time_point);

	}
	else if (this->inlet_temp.applied_flag == true) {
		if (rv_profile == NULL) {
			Error msg = this->set_error(11);
			ostringstream info;
			info << "Profile name  : " << this->get_profile_name() << endl;
			info << "Profile number: " << this->get_profile_number() << endl;
			msg.make_second_info(info.str());
			throw msg;
		}
		if (this->inlet_temp.stat_flag == true) {
			//stationary
			this->inlet_temp.current_value = this->inlet_temp.index_value;

		}
		else {
			//instationary
			this->inlet_temp.current_value = this->inlet_temp.ptr_curve->calculate_actuel_boundary_value(time_point);
		}


		this->discharge_current = this->bound_discharge.calculate_actuel_boundary_value(time_point);
		this->waterdepth_current = this->bound_water_depth.calculate_actuel_boundary_value(time_point);
		this->flow_velocity_current = this->bound_velocity.calculate_actuel_boundary_value(time_point);

		//upstream
		if (this->flow_velocity_current > 0.0) {
			double inlet_q = rv_profile->get_boundary_point_value(time_point);
			double q_river = upstream->bound_discharge.calculate_actuel_boundary_value(time_point);
			double temp_river= upstream->get_actual_temperature();
			this->water_bound_temp.current_value = (this->inlet_temp.current_value*inlet_q + q_river * temp_river) / (inlet_q + q_river);
			//ostringstream cout;
			//cout << " inlet temp " << this->inlet_temp.current_value << " inlet d " << inlet_q << " river temp " << temp_river << " river q  " << q_river << endl;
			//Sys_Common_Output::output_hyd->output_txt(&cout, true);

		}
		//downstream
		else {
			double inlet_q = rv_profile->get_boundary_point_value(time_point);
			double q_river = downstream->bound_discharge.calculate_actuel_boundary_value(time_point);
			double temp_river = downstream->get_actual_temperature();
			this->water_bound_temp.current_value = (this->inlet_temp.current_value*inlet_q + q_river * temp_river) / (inlet_q + q_river);

		}



	}
	else {
		if (this->air_temp.stat_flag == true) {
			//stationary
			this->air_temp.current_value = this->air_temp.index_value;

		}
		else {
			//instationary
			this->air_temp.current_value = this->air_temp.ptr_curve->calculate_actuel_boundary_value(time_point);
		}

		if (this->solar_rad.stat_flag == true) {
			//stationary
			this->solar_rad.current_value = this->solar_rad.index_value;

		}
		else {
			//instationary
			this->solar_rad.current_value = this->solar_rad.ptr_curve->calculate_actuel_boundary_value(time_point);
		}


		if (this->humid.stat_flag == true) {
			//stationary
			this->humid.current_value = this->humid.index_value;

		}
		else {
			//instationary
			this->humid.current_value = this->humid.ptr_curve->calculate_actuel_boundary_value(time_point);
		}

		if (this->wind.stat_flag == true) {
			//stationary
			this->wind.current_value = this->wind.index_value;

		}
		else {
			//instationary
			this->wind.current_value = this->wind.ptr_curve->calculate_actuel_boundary_value(time_point);
		}

		if (this->cloud.stat_flag == true) {
			//stationary
			this->cloud.current_value = this->cloud.index_value;

		}
		else {
			//instationary
			this->cloud.current_value = this->cloud.ptr_curve->calculate_actuel_boundary_value(time_point);
		}

		if (this->shadow.stat_flag == true) {
			//stationary
			this->shadow.current_value = this->shadow.index_value;

		}
		else {
			//instationary
			this->shadow.current_value = this->shadow.ptr_curve->calculate_actuel_boundary_value(time_point);
		}




		this->discharge_current = this->bound_discharge.calculate_actuel_boundary_value(time_point);
		this->waterdepth_current = this->bound_water_depth.calculate_actuel_boundary_value(time_point);
		this->flow_velocity_current = this->bound_velocity.calculate_actuel_boundary_value(time_point);



		this->calc_phi_bed(params);
		//Calculate heat flow evaporation
		this->calc_phi_eva(params);
		//Calculate heat flow conductivity
		this->calc_phi_c(params);

		//Calculate heat flow longwave radiation
		this->calc_phi_lw(params);
		//Calculate heat flow solar radiation
		this->calc_phi_solar(params);

		this->phi_total = this->phi_bed + this->phi_c + this->phi_eva + this->phi_lw + this->phi_solar;


		//Set a limit
		double water_buff = this->waterdepth_current;
		if (this->waterdepth_current < 0.10) {
			water_buff = 0.10;
		}

		this->delta_temp_flow = this->phi_total / (constant::dens_water*constant::c_water*water_buff);
	}


}
//Calculate the values for a temperature balance of the corresponding river segment (upstream) for each syncronisation step
void HydTemp_Profile::calculate_temp_balance(const double time_point) {
	//calculate the maximum discharge
	if (this->temp_value_max.maximum < this->temp_current && time_point > 120) {
		this->temp_value_max.maximum = this->temp_current;
		this->temp_value_max.time_point = time_point;
	}

	if (this->temp_value_min.maximum > this->temp_current && time_point>120) {
		this->temp_value_min.maximum = this->temp_current;
		this->temp_value_min.time_point = time_point;
	}
	this->count_average++;
	this->temp_average = this->temp_average + ((this->temp_current- this->temp_average)/ count_average);


}
//Output the header for the maximum result output to console/display
void HydTemp_Profile::output_header_max_results(void){
	ostringstream cout;
	cout << "No." << W(7) << "RvSt"<< label::m << W(8);
	cout << "Water_Temp_max" <<label::kelvin<< W(15);
	cout << "t(Temp_max)" <<label::sec<< W(15);
	cout << "Water_Temp_min" << label::kelvin << W(15);
	cout << "t(Temp_min)" << label::sec << W(15);
	cout << "Water_Temp_average" << label::kelvin << W(15);
	cout << endl;
	Sys_Common_Output::output_hyd->output_txt(&cout,true);
}
//Output the maximum result output to console/display
void HydTemp_Profile::output_max_results(const double rv_station){
	ostringstream cout;
	cout << this->profile_number << W(11);
	cout << P(1) << FORMAT_FIXED_REAL<< rv_station << W(12);

	cout << P(2) << FORMAT_FIXED_REAL <<this->temp_value_max.maximum << W(12);
	cout << P(0) << FORMAT_FIXED_REAL <<this->temp_value_max.time_point << W(12);
	cout << P(2) << FORMAT_FIXED_REAL << this->temp_value_min.maximum << W(12);
	cout << P(0) << FORMAT_FIXED_REAL << this->temp_value_min.time_point << W(12);
	cout << P(2) << FORMAT_FIXED_REAL << this->temp_average << W(12);
	cout << endl;
	Sys_Common_Output::output_hyd->output_txt(&cout,true);
}
//Output the maximum result output to a given tecplot file
void HydTemp_Profile::output_max_results2file(ofstream *file, const double rv_station){
	*file << P(1) << FORMAT_FIXED_REAL<< rv_station << W(11);

	*file << P(2) << FORMAT_FIXED_REAL <<this->temp_value_max.maximum << W(12);
	*file << P(0) << FORMAT_FIXED_REAL <<this->temp_value_max.time_point << W(12);
	*file << P(2) << FORMAT_FIXED_REAL << this->temp_value_min.maximum << W(12);
	*file << P(0) << FORMAT_FIXED_REAL << this->temp_value_min.time_point << W(12);
	*file << P(2) << FORMAT_FIXED_REAL << this->temp_average << W(12);
	*file << endl;
}
//Output the maximum result output to a csv file
void HydTemp_Profile::output_max_results2csvfile(ofstream *file, const double rv_station, const double z_min) {
	*file << P(1) << FORMAT_FIXED_REAL << rv_station << W(11) << ",";
	*file << P(1) << FORMAT_FIXED_REAL << z_min << W(11);
	*file << P(2) << FORMAT_FIXED_REAL << this->temp_value_max.maximum << W(10) << ",";
	*file << P(0) << FORMAT_FIXED_REAL << this->temp_value_max.time_point << W(10) << ",";
	*file << P(2) << FORMAT_FIXED_REAL << this->temp_value_min.maximum << W(10) << ",";
	*file << P(0) << FORMAT_FIXED_REAL << this->temp_value_min.time_point << W(10) << ",";
	*file << P(2) << FORMAT_FIXED_REAL << this->temp_average;

	*file << endl;

}
//Output the maximum calculated results to the database table (erg_table)
void HydTemp_Profile::output_max_results(QSqlDatabase *ptr_database, const int rv_no, const string polygon_string, int *glob_id, const string break_sc){
	//mysql query with the table_model
	QSqlQueryModel model;
	//the table is set (the name and the column names) and allocated
	try{
		HydTemp_Profile::set_erg_table(ptr_database);
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


	//submit it to the datbase
	Data_Base::database_request(&model, query_string.str(), ptr_database);

	if(model.lastError().isValid()){
		//try new glob id
		//renew it
		*glob_id = HydTemp_Profile::erg_table->maximum_int_of_column(HydTemp_Profile::erg_table->get_column_name(label::glob_id), ptr_database)+1;
		query_string.clear();
		this->generate_max_str2database(&query_string, rv_no, polygon_string, glob_id, break_sc);
		model.clear();
		//second try
		Data_Base::database_request(&model, query_string.str(), ptr_database);
		if (model.lastError().isValid()) {
			Warning msg = this->set_warning(3);
			ostringstream info;
			info << "Table Name                : " << HydTemp_Profile::erg_table->get_table_name() << endl;
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
void HydTemp_Profile::output_instat_results(QSqlDatabase *ptr_database, const int rv_no, const string polygon_string, int *glob_id, const string break_sc, const string time) {

	//mysql query with the table_model
	QSqlQueryModel model;
	//the table is set (the name and the column names) and allocated
	try {
		HydTemp_Profile::set_erg_instat_table(ptr_database);
	}
	catch (Error msg) {
		throw msg;
	}



	//set the query via a query string
	ostringstream query_string;
	query_string << "INSERT INTO  " << HydTemp_Profile::erg_instat_table->get_table_name();
	query_string << " ( ";
	query_string << HydTemp_Profile::erg_instat_table->get_column_name(label::glob_id) << " , ";
	query_string << HydTemp_Profile::erg_instat_table->get_column_name(hyd_label::profdata_rvno) << " , ";
	query_string << HydTemp_Profile::erg_instat_table->get_column_name(label::areastate_id) << " , ";
	query_string << HydTemp_Profile::erg_instat_table->get_column_name(label::measure_id) << " , ";
	query_string << HydTemp_Profile::erg_instat_table->get_column_name(hyd_label::sz_bound_id) << " , ";
	query_string << HydTemp_Profile::erg_instat_table->get_column_name(risk_label::sz_break_id) << " , ";
	//max values
	query_string << HydTemp_Profile::erg_instat_table->get_column_name(hyd_label::proferg_T_water) << " , ";
	query_string << HydTemp_Profile::erg_instat_table->get_column_name(hyd_label::air_temp) << " , ";
	query_string << HydTemp_Profile::erg_instat_table->get_column_name(hyd_label::solar_rad) << " , ";
	query_string << HydTemp_Profile::erg_instat_table->get_column_name(hyd_label::humidity) << " , ";
	query_string << HydTemp_Profile::erg_instat_table->get_column_name(hyd_label::wind) << " , ";
	query_string << HydTemp_Profile::erg_instat_table->get_column_name(hyd_label::cloud) << " , ";
	query_string << HydTemp_Profile::erg_instat_table->get_column_name(hyd_label::shadow) << " , ";
	query_string << HydTemp_Profile::erg_instat_table->get_column_name(hyd_label::inlet_temp) << " , ";

	query_string << HydTemp_Profile::erg_instat_table->get_column_name(hyd_label::data_time) << " , ";
	query_string << HydTemp_Profile::erg_instat_table->get_column_name(hyd_label::proferg_polygon) << " ) ";

	query_string << " VALUES ( ";
	query_string << *glob_id << " , ";
	query_string << rv_no << " , ";
	query_string << this->system_id.area_state << " , ";
	query_string << this->system_id.measure_nr << " , ";
	query_string << this->hyd_sz.get_id() << " , ";
	query_string << "'" << break_sc << "' , ";
	//max values
	query_string << this->temp_current<< " , ";
	query_string << this->air_temp.current_value << " , ";
	query_string << this->solar_rad.current_value << " , ";
	query_string << this->humid.current_value << " , ";
	query_string << this->wind.current_value << " , ";
	query_string << this->cloud.current_value << " , ";
	query_string << this->shadow.current_value << " , ";
	query_string << this->inlet_temp.current_value << " , ";


	query_string << time << " , ";
	query_string << polygon_string << " ) ";

	//submit it to the datbase
	Data_Base::database_request(&model, query_string.str(), ptr_database);

	if (model.lastError().isValid()) {
		Warning msg = this->set_warning(9);
		ostringstream info;
		info << "Table Name                : " << HydTemp_Profile::erg_instat_table->get_table_name() << endl;
		info << "Table error info          : " << model.lastError().text().toStdString() << endl;
		info << "Profile number            : " << this->profile_number << endl;
		msg.make_second_info(info.str());
		msg.output_msg(2);
	}
	*glob_id = (*glob_id) + 1;






}
//Check the temperature data of the profiles
void HydTemp_Profile::check_profiles(Hyd_River_Profile_Connection_Standard *rv_profile) {
	//check if a init condition is set
	if (this->init_condition < 0.0) {
		Error msg = this->set_error(5);
		ostringstream info;
		info << "Profile name  : " << this->get_profile_name() << endl;
		info << "Profile number: " << this->get_profile_number() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}

	//check if inlet temperature applied also a inflow boundary is applied
	if (this->inlet_temp.applied_flag == true) {
		if (rv_profile == NULL) {
			Error msg = this->set_error(10);
			ostringstream info;
			info << "Profile name  : " << this->get_profile_name() << endl;
			info << "Profile number: " << this->get_profile_number() << endl;
			msg.make_second_info(info.str());
			throw msg;
		}

		if (rv_profile->boundary_point_is_applied() == false) {
			Error msg = this->set_error(9);
			ostringstream info;
			info << "Profile name  : " << this->get_profile_name() << endl;
			info << "Profile number: " << this->get_profile_number() << endl;
			msg.make_second_info(info.str());
			throw msg;

		};


	}

	if (this->air_temp.stat_flag == true) {
		if (this->air_temp.index_value < 0.0) {
			Warning msg = this->set_warning(0);
			ostringstream info;
			this->air_temp.index_value = 283.0;
			info << "Profile " << this->profile_number << " Profilname " << this->name << endl;
			msg.make_second_info(info.str());
			msg.output_msg(2);
		}
	}

	//todo UDO further checks 
	if (this->humid.stat_flag == true) {
		//stationary
		if (this->humid.index_value < 0.0 || this->humid.index_value > 1.0) {
			//Set waring
			Warning msg = this->set_warning(6);
			ostringstream info;
			this->humid.index_value = 0.8;
			info << "Profile " << this->profile_number << " Profilname " << this->name << endl;
			msg.make_second_info(info.str());
			msg.output_msg(2);
		}
	}
	else {
		//instationary
		this->humid.ptr_curve->check_curve();
	}
	//check the stationary boundary for shadow factor
	if (this->shadow.stat_flag == true) {
		if (this->shadow.index_value < 0.0 || this->shadow.index_value > 1.0) {
			//Set waring
			Warning msg = this->set_warning(7);
			ostringstream info;
			this->shadow.index_value = 0.5;
			info << "Profile " << this->profile_number << " Profilname " << this->name << endl;
			msg.make_second_info(info.str());
			msg.output_msg(2);
		}
	}
	else {
		//instationary
		this->shadow.ptr_curve->check_curve();
	}
	//Check the stationary boundary for solar_radiation
	if (this->solar_rad.stat_flag == true) {
		Warning msg = this->set_warning(8);
		ostringstream info;
		info << "Profile " << this->profile_number << " Profilname " << this->name << endl;
		msg.make_second_info(info.str());
		msg.output_msg(2);
		if (this->solar_rad.index_value < 0.0) {
			//Set waring
			Warning msg = this->set_warning(12);
			ostringstream info;
			this->solar_rad.index_value = 150;
			info << "Profile " << this->profile_number << " Profilname " << this->name << endl;
			msg.make_second_info(info.str());
			msg.output_msg(2);
		}
	}
	else {
		//instationary
		//this->solar_rad.current_value = this->solar_rad.ptr_curve->calculate_actuel_boundary_value(time_point);
	}
	//Check the stationary boundary for wind speed/velocity
	if (this->wind.stat_flag == true) {
		if (this->wind.index_value < 0.0) {
			//Set waring
			Warning msg = this->set_warning(13);
			ostringstream info;
			this->wind.index_value = 2;
			info << "Profile " << this->profile_number << " Profilname " << this->name << endl;
			msg.make_second_info(info.str());
			msg.output_msg(2);
		}
	}
	else {
		//instationary
		//this->solar_rad.current_value = this->solar_rad.ptr_curve->calculate_actuel_boundary_value(time_point);
	}


}


//Output the members
void HydTemp_Profile::output_members(void) {
	ostringstream prefix;
	prefix << "PROF_" << this->profile_number << "> ";
	Sys_Common_Output::output_hyd->set_userprefix(prefix.str());

	ostringstream cout;
	cout << "GENERAL" << endl;
	cout << " Name             : " << this->name << endl;
	cout << " Init-h           : " << this->init_condition << label::kelvin<< endl;
	if (this->water_bound_temp.applied_flag == true) {
		cout << " Water Temperature: ";
		if (this->water_bound_temp.stat_flag == true) {
			cout << " Stationary value of " << this->water_bound_temp.index_value << label::kelvin << endl;
		}
		else {
			cout << " Instationary curve-no. of " << this->water_bound_temp.index_value << endl;
		}

	}
	else {
		cout << " Air Temperature  : ";
		if (this->air_temp.stat_flag == true) {
			cout << " Stationary value of " << this->air_temp.index_value << label::kelvin << endl;
		}
		else {
			cout << " Instationary curve-no. of " << this->air_temp.index_value << endl;
		}
		cout << " Solar radiation  : ";
		if (this->solar_rad.stat_flag == true) {
			cout << " Stationary value of " << this->solar_rad.index_value << label::watt_per_square_m << endl;
		}
		else {
			cout << " Instationary curve-no. of " << this->solar_rad.index_value << endl;
		}
		cout << " Humidity         : ";
		if (this->humid.stat_flag == true) {
			cout << " Stationary value of " << this->humid.index_value << label::no_unit << endl;
		}
		else {
			cout << " Instationary curve-no. of " << this->humid.index_value << endl;
		}
		cout << " Wind speed       : ";
		if (this->wind.stat_flag == true) {
			cout << " Stationary value of " << this->wind.index_value << label::m_per_sec << endl;
		}
		else {
			cout << " Instationary curve-no. of " << this->wind.index_value << endl;
		}

		cout << " Cloudness        : ";
		if (this->cloud.stat_flag == true) {
			cout << " Stationary value of " << this->cloud.index_value << label::no_unit << endl;
		}
		else {
			cout << " Instationary curve-no. of " << this->cloud.index_value << endl;
		}

		cout << " Shadow           : ";
		if (this->shadow.stat_flag == true) {
			cout << " Stationary value of " << this->shadow.index_value << label::no_unit << endl;
		}
		else {
			cout << " Instationary curve-no. of " << this->shadow.index_value << endl;
		}
		cout << " Inlet temperature: ";
		if (this->inlet_temp.applied_flag == true) {
			if (this->inlet_temp.stat_flag == true) {
				cout << " Stationary value of " << this->inlet_temp.index_value << label::kelvin << endl;
			}
			else {
				cout << " Instationary curve-no. of " << this->inlet_temp.index_value << endl;
			}
		}
		else {

			cout << " Not set" << endl;
		}
	}





	Sys_Common_Output::output_hyd->output_txt(&cout, true);
	Sys_Common_Output::output_hyd->rewind_userprefix();



}
//Output the setted and calculated members (e.g. tables, boundary conditions etc.)
void HydTemp_Profile::output_setted_members(void) {
	ostringstream prefix;
	prefix << "PROF_" << this->profile_number << "> ";
	Sys_Common_Output::output_hyd->set_userprefix(prefix.str());

	ostringstream cout;
	cout << "GENERAL" << endl;
	cout << " Name                : " << this->name << endl;
	cout << " Distance to upstream: " << this->distance2up << label::m << endl;



	Sys_Common_Output::output_hyd->output_txt(&cout, true);
	Sys_Common_Output::output_hyd->rewind_userprefix();

}
//Output the header for the output of the result members to console/display (normal use for development) (static)
void HydTemp_Profile::output_result_members_per_timestep_header(void){
	ostringstream cout;
	cout << "No." << W(5) << "RvSt"<< label::m << W(10);
	cout<< "Water_Temp" << label::kelvin<< W(10);
	cout << "Air_Temp" << label::kelvin << W(10);
	cout << "Solar_radiation" << label::watt_per_square_m << W(10);
	cout << "Humidity" << label::no_unit << W(10);
	cout << "Wind_speed" << label::m_per_sec << W(10);
	cout << "Cloudness" << label::no_unit << W(10);
	cout << "Shadow" << label::no_unit << W(10);
	cout << "Inlet_Temp" << label::kelvin << W(10);



	cout << endl;
	Sys_Common_Output::output_hyd->output_txt(&cout,true);
}
//Output the result members to console/display (normal use for development)
void HydTemp_Profile::output_result_members_per_timestep(ostringstream *cout, const double rv_station){
	*cout << this->profile_number;
	*cout << W(10) << P(1) << FORMAT_FIXED_REAL<< rv_station;
	*cout << W(10) << P(3) << FORMAT_FIXED_REAL<<this->temp_current;
	*cout << W(10) << P(3) << FORMAT_FIXED_REAL << this->air_temp.current_value;
	*cout << W(10) << P(3) << FORMAT_FIXED_REAL << this->solar_rad.current_value;
	*cout << W(10) << P(3) << FORMAT_FIXED_REAL << this->humid.current_value;
	*cout << W(10) << P(3) << FORMAT_FIXED_REAL << this->wind.current_value;
	*cout << W(10) << P(3) << FORMAT_FIXED_REAL << this->cloud.current_value;
	*cout << W(10) << P(3) << FORMAT_FIXED_REAL << this->shadow.current_value;
	*cout << W(10) << P(3) << FORMAT_FIXED_REAL << this->inlet_temp.current_value;

	*cout << endl;
}
//Output the result members for each riverprofiletype at every timestep to file
void HydTemp_Profile::output_result_members_per_timestep(ofstream *file, const double rv_station){
	*file << W(10) << P(2) << FORMAT_FIXED_REAL<< rv_station;
	*file << W(10) << P(3) << FORMAT_FIXED_REAL << this->temp_current;
	*file << W(10) << P(3) << FORMAT_FIXED_REAL << this->air_temp.current_value;
	*file << W(10) << P(3) << FORMAT_FIXED_REAL << this->solar_rad.current_value;
	*file << W(10) << P(3) << FORMAT_FIXED_REAL << this->humid.current_value;
	*file << W(10) << P(3) << FORMAT_FIXED_REAL << this->wind.current_value;
	*file << W(10) << P(3) << FORMAT_FIXED_REAL << this->cloud.current_value;
	*file << W(10) << P(3) << FORMAT_FIXED_REAL << this->shadow.current_value;
	*file << W(10) << P(3) << FORMAT_FIXED_REAL << this->inlet_temp.current_value <<endl;
	
}
//Output the result members for each riverprofiletype at every timestep to csv file
void HydTemp_Profile::output_result_members_per_timestep2csv(ofstream *file, const double rv_station, const double z_min) {
	*file << W(10) << P(2) << FORMAT_FIXED_REAL << rv_station << ",";
	//*file << W(10) << P(2) << FORMAT_FIXED_REAL << z_min << ",";
	*file << W(10) << P(3) << FORMAT_FIXED_REAL << this->temp_current << ",";
	//*file << W(10) << P(3) << FORMAT_FIXED_REAL << this->air_temp.current_value << ",";
	//*file << W(10) << P(3) << FORMAT_FIXED_REAL << this->solar_rad.current_value << ",";
	//*file << W(10) << P(3) << FORMAT_FIXED_REAL << this->humid.current_value << ",";
	//*file << W(10) << P(3) << FORMAT_FIXED_REAL << this->wind.current_value << ",";
	//*file << W(10) << P(3) << FORMAT_FIXED_REAL << this->cloud.current_value << ",";
	//*file << W(10) << P(3) << FORMAT_FIXED_REAL << this->shadow.current_value << ",";
	//*file << W(10) << P(3) << FORMAT_FIXED_REAL << this->inlet_temp.current_value << ",";
	*file << W(10) << P(3) << FORMAT_FIXED_REAL << this->phi_total << ",";
	*file << W(10) << P(3) << FORMAT_FIXED_REAL << this->phi_bed << ",";
	*file << W(10) << P(3) << FORMAT_FIXED_REAL << this->phi_c << ",";
	*file << W(10) << P(3) << FORMAT_FIXED_REAL << this->phi_eva<< ",";
	*file << W(10) << P(3) << FORMAT_FIXED_REAL << this->phi_lw<< ",";
	*file << W(10) << P(3) << FORMAT_FIXED_REAL << this->phi_solar << ",";
	//*file << W(10) << P(3) << FORMAT_FIXED_REAL << this->discharge_current << ",";
	//*file << W(10) << P(3) << FORMAT_FIXED_REAL << this->flow_velocity_current << ",";
	//*file << W(10) << P(3) << FORMAT_FIXED_REAL << z_min + this->waterdepth_current << ",";
	*file << W(10) << P(3) << FORMAT_FIXED_REAL << this->waterdepth_current<<endl;
	

}
//Reset the hydrological balance value and the maximum values
void HydTemp_Profile::reset_hydrobalance_maxvalues(void){

	this->temp_value_max.maximum=0.0;
	this->temp_value_max.time_point=0.0;
	this->temp_value_min.maximum = 999999;
	this->temp_value_min.maximum = 0.0;
	this->count_average = 0;
	

}
//Get the global profile id in database
int HydTemp_Profile::get_prof_glob_id(void){
	return this->glob_prof_id;
}
//Connect instationary boundary curve to the temperature profiles
void HydTemp_Profile::connect_instat_boundarycurve(Hyd_Instationary_Boundary *bound_curve, const int number) {
	if (this->water_bound_temp.applied_flag == true) {
		int counter_found = 0;
		int counter_must_found = 0;
		if (this->water_bound_temp.stat_flag == false) {
			counter_must_found++;
		}

		for (int i = 0; i < number; i++) {

			if (this->water_bound_temp.stat_flag == false) {
				if (bound_curve[i].get_curve_number() == this->water_bound_temp.index_value) {
					try {
						bound_curve[i].set_type(_hyd_bound_type::temperature);
					}
					catch (Error msg) {
						throw msg;
					}
					this->water_bound_temp.ptr_curve = &bound_curve[i];
					counter_found++;
				}
			}
		}

		if (counter_found != counter_must_found) {
			ostringstream info;
			info << "Water temperature boundary " << endl;
			info << "Number of instationary boundary found " << counter_found << endl;
			info << "Required number " << counter_must_found << endl;
			Error msg = this->set_error(14);
			msg.make_second_info(info.str());
			throw msg;

		}



	}
	else if (this->inlet_temp.applied_flag == true) {

		int counter_found = 0;
		int counter_must_found = 0;
		if (this->inlet_temp.stat_flag == false) {
			counter_must_found++;
		}

		for (int i = 0; i < number; i++) {

			if (this->inlet_temp.stat_flag == false) {
				if (bound_curve[i].get_curve_number() == this->inlet_temp.index_value) {
					try {
						bound_curve[i].set_type(_hyd_bound_type::temperature);
					}
					catch (Error msg) {
						throw msg;
					}
					this->inlet_temp.ptr_curve = &bound_curve[i];
					counter_found++;
				}
			}
		}

		if (counter_found != counter_must_found) {
			ostringstream info;
			info << "Inlet temperature boundary " << endl;
			info << "Number of instationary boundary found " << counter_found << endl;
			info << "Required number " << counter_must_found << endl;
			Error msg = this->set_error(14);
			msg.make_second_info(info.str());
			throw msg;

		}





	}
	else {

		int counter_found = 0;
		int counter_must_found = 0;
		if (this->air_temp.stat_flag == false) {
			counter_must_found++;
		}
		if (this->solar_rad.stat_flag == false) {
			counter_must_found++;
		}
		if (this->humid.stat_flag == false) {
			counter_must_found++;
		}
		if (this->wind.stat_flag == false) {
			counter_must_found++;
		}
		if (this->cloud.stat_flag == false) {
			counter_must_found++;
		}
		if (this->shadow.stat_flag == false) {
			counter_must_found++;
		}

		if (this->inlet_temp.applied_flag == true) {
			counter_must_found++;
		}
		for (int i = 0; i < number; i++) {

			if (this->air_temp.stat_flag == false) {
				if (bound_curve[i].get_curve_number() == this->air_temp.index_value) {
					try {
						bound_curve[i].set_type(_hyd_bound_type::temperature);
					}
					catch (Error msg) {
						throw msg;
					}
					this->air_temp.ptr_curve = &bound_curve[i];
					counter_found++;
				}
			}


			if (this->solar_rad.stat_flag == false) {
				if (bound_curve[i].get_curve_number() == this->solar_rad.index_value) {
					try {
						bound_curve[i].set_type(_hyd_bound_type::radiation);
					}
					catch (Error msg) {
						throw msg;
					}
					this->solar_rad.ptr_curve = &bound_curve[i];
					counter_found++;
				}
			}


			if (this->humid.stat_flag == false) {
				if (bound_curve[i].get_curve_number() == this->humid.index_value) {
					try {
						bound_curve[i].set_type(_hyd_bound_type::percentage);
					}
					catch (Error msg) {
						throw msg;
					}
					this->humid.ptr_curve = &bound_curve[i];
					counter_found++;
				}
			}


			if (this->wind.stat_flag == false) {
				if (bound_curve[i].get_curve_number() == this->wind.index_value) {
					try {
						bound_curve[i].set_type(_hyd_bound_type::speed);
					}
					catch (Error msg) {
						throw msg;
					}
					this->wind.ptr_curve = &bound_curve[i];
					counter_found++;
				}
			}


			if (this->cloud.stat_flag == false) {
				if (bound_curve[i].get_curve_number() == this->cloud.index_value) {
					try {
						bound_curve[i].set_type(_hyd_bound_type::percentage);
					}
					catch (Error msg) {
						throw msg;
					}
					this->cloud.ptr_curve = &bound_curve[i];
					counter_found++;
				}
			}


			if (this->shadow.stat_flag == false) {
				if (bound_curve[i].get_curve_number() == this->shadow.index_value) {
					try {
						bound_curve[i].set_type(_hyd_bound_type::percentage);
					}
					catch (Error msg) {
						throw msg;
					}
					this->shadow.ptr_curve = &bound_curve[i];
					counter_found++;
				}
			}


			if (this->inlet_temp.applied_flag == true) {
				if (this->inlet_temp.stat_flag == false) {
					if (bound_curve[i].get_curve_number() == this->inlet_temp.index_value) {
						try {
							bound_curve[i].set_type(_hyd_bound_type::temperature);
						}
						catch (Error msg) {
							throw msg;
						}
						this->inlet_temp.ptr_curve = &bound_curve[i];
						counter_found++;
					}
				}

			}

		}

		if (counter_found != counter_must_found) {
			ostringstream info;
			info << "Number of instationary boundary found " << counter_found << endl;
			info << "Required number " << counter_must_found << endl;
			Error msg = this->set_error(14);
			msg.make_second_info(info.str());
			throw msg;

		}
	}


}
//Get the instationary found flag
bool HydTemp_Profile::get_instat_boundarycurve_found(void) {

	return this->boundary_found;

}
///Reset the boundary data of the temperature profile
void HydTemp_Profile::reset_boundary_data(void) {


	this->inlet_temp.ptr_curve = NULL;
	this->shadow.ptr_curve = NULL;
	this->cloud.ptr_curve = NULL;

	this->air_temp.ptr_curve = NULL;
	this->humid.ptr_curve = NULL;
	this->solar_rad.ptr_curve = NULL;

	this->wind.ptr_curve = NULL;

	this->boundary_found = false;

}
//Get the temperature differnce due to heat flows
double HydTemp_Profile::get_delta_temp(void) {

	return this->delta_temp_flow;
}
//Get the distance to upstream profile
double HydTemp_Profile::get_distance2up(void) {

	return this->distance2up;
}
//Get the current flow velocity
double HydTemp_Profile::get_flow_velocity_current(void) {
	return this->flow_velocity_current;

}
//Set the distance to upstream profile
void HydTemp_Profile::set_distance2up(const double distance) {

	this->distance2up= distance;
}
//_________________________________
//private
//Calculate heat flow bed
void HydTemp_Profile::calc_phi_bed(HydTemp_Param *params) {
	this->phi_bed = params->con_bed_coef*(params->bed_temp - this->temp_current);
}
//Calculate heat flow conductivity
void HydTemp_Profile::calc_phi_c(HydTemp_Param *params) {
	double es = 0.0;
	double ea = 0.0;

	es = 6.1275*exp(((17.27*(this->temp_current - constant::kelvin_const)) / (237.3 + (this->temp_current - constant::kelvin_const))));
	ea = es * this->humid.current_value;
	if (this->phi_eva < constant::zero_epsilon && this->phi_eva > -1*constant::zero_epsilon) {
		this->phi_c = 0.0;
	}
	//else {
	//	this->phi_c = this->phi_eva*0.00061*constant::pressure_air*(this->temp_current - this->air_temp.current_value) / (es - ea);
	
	if (this->humid.current_value >= 0.90) {
		this->phi_c = this->phi_eva*(0.00061*constant::pressure_air*(this->temp_current - this->air_temp.current_value) / (es - (es*0.90)));
	}
	else {
		this->phi_c = this->phi_eva*(0.00061*constant::pressure_air*(this->temp_current - this->air_temp.current_value) / (es - ea));
	}


}
//Calculate heat flow evaporation
void HydTemp_Profile::calc_phi_eva(HydTemp_Param *params) {
	double es = 0.0;
	double ea = 0.0;
	double Le = 0.0;
	double Erate = 0.0;
	double s = 0.0;
	double ra = 0.0;

	//es= 6.1275*exp(((17.27*(this->air_temp.current_value- constant::kelvin_const)) / (237.3 + (this->air_temp.current_value - constant::kelvin_const))));
	es = 0.61275*exp(((17.27*(this->air_temp.current_value - constant::kelvin_const)) / (237.3 + (this->air_temp.current_value - constant::kelvin_const))));
	ea = es * this->humid.current_value;
	Le = 1000 * (2501.4 + (this->temp_current - constant::kelvin_const));
	s = 4100 * es / pow((237 + (this->air_temp.current_value - constant::kelvin_const)), 2);
	ra = 245 / (0.54 * this->wind.current_value + 0.5);
	Erate = s * (this->phi_solar + this->phi_lw) / (constant::dens_water * Le*(s + constant::psychometric)) + (constant::c_air*constant::dens_air*(es - ea)) / (constant::dens_water*Le*ra*(s + constant::psychometric));

	//this->phi_eva = -((constant::dens_air*constant::c_air) / constant::psychometric)*(0.0022*this->wind.current_value + 0.0021)*(es - ea);
	this->phi_eva = -constant::dens_water* Le * Erate;
}
//Calculate heat flow longwave radiation
void HydTemp_Profile::calc_phi_lw(HydTemp_Param *params) {
	double es = 0.0;
	double ea = 0.0;
	es = 0.61275*exp((17.27*(this->air_temp.current_value - constant::kelvin_const)) / (237.3 + (this->air_temp.current_value - constant::kelvin_const)));
	ea = es * this->humid.current_value;
	double phi_alw = 0.0;
	double phi_lclw = 0.0;
	double phi_slw = 0.0;
	//double eta_atm = 1.72*(1.0 + 0.22 + pow(this->cloud.current_value, 2))*pow((0.1*ea / this->air_temp.current_value), (1.0/7.0));
	double eta_atm = (1.1*params->brunt_coef + 0.094*pow(ea, 0.5));

	//phi_alw = pow(this->air_temp.current_value, 4)*0.96*constant::boltzman*eta_atm;
	phi_alw = 0.96*eta_atm*params->view2sky_coef*constant::boltzman*pow(this->air_temp.current_value, 4); //equation from Westhoff et. al. 2011 (Corrigendum!)
	phi_lclw = 0.96*(1 - params->view2sky_coef)*eta_atm*constant::boltzman*pow(this->air_temp.current_value, 4);
	phi_slw = -0.96*constant::boltzman*pow((this->temp_current), 4);
	this->phi_lw = phi_alw + phi_lclw + phi_slw;



	//ostringstream cout;
	//cout << " es " << es<< label::kelvin << " ea " << ea << " term1 " << -0.96*constant::boltzman*pow((this->temp_current), 4)<< " term 2 "<< pow((this->air_temp.current_value), 4)*0.96*constant::boltzman*1.72
		//<< " term 3 " << (1.0 + 0.22 + pow(this->cloud.current_value, 2))*pow(((0.1*ea) / this->air_temp.current_value), (1 / 7))<< " term 4 " << 0.96*params->view2sky_coef*(1.1*params->brunt_coef + 0.094*pow(ea, 0.5))*constant::boltzman*pow(this->air_temp.current_value, 4)<< endl;
	//Sys_Common_Output::output_hyd->output_txt(&cout, true);

}
//Calculate heat flow solar radiation
void HydTemp_Profile::calc_phi_solar(HydTemp_Param *params) {
	double phi_direct = 0.0;
	double phi_dif = 0.0;

	phi_dif = params->diffusiv_sol_rad_coeff*this->solar_rad.current_value;
	phi_direct=this->shadow.current_value*(1.0- params->diffusiv_sol_rad_coeff)*this->solar_rad.current_value;
	this->phi_solar = (1.0 - params->bed_warming_coeff)*(phi_dif+ phi_direct);

}
//Reset the temperature data structure
void HydTemp_Profile::reset_temp_data_struct(_hyd_temp_data *to_reset) {
	to_reset->applied_flag = false;
	to_reset->index_value = -9999;
	to_reset->stat_flag = false;
	to_reset->ptr_curve = NULL;
	to_reset->current_value = -9999;

}
//decide in the different keyvalues in the temperature data profile file
void HydTemp_Profile::decide_keyvalues_file(QString line, _hyd_temp_data *to_reset, int *found_counter){

	QStringList buff_list;
	buff_list = line.split(' ', QString::SkipEmptyParts);

	to_reset->stat_flag = functions::convert_string2boolean(buff_list[1].toStdString());
	if (to_reset->stat_flag == true) {
		//stat
		bool correct = true;
		to_reset->index_value = buff_list[2].toDouble(&correct);
		if (correct == false) {
			//Error
			Error msg = this->set_error(4);
			ostringstream info;
			info << "Wrong input sequenze " << line.toStdString() << endl;
			msg.make_second_info(info.str());
			throw msg;

		}
		
		(*found_counter)++;
		
	}
	else {
		//instat
		bool correct = true;
		to_reset->index_value = buff_list[2].toInt(&correct);
		if (correct == false) {
			//Error
			Error msg = this->set_error(4);
			ostringstream info;
			info << "Wrong input sequenze " << line.toStdString() << endl;
			msg.make_second_info(info.str());
			throw msg;

		}
		
		(*found_counter)++;
		


	}
	to_reset->applied_flag = true;
}
//Transfer the river profile boundary data to a database
void HydTemp_Profile::transfer_boundary_data2database(QSqlDatabase *ptr_database, const int glob_prof_id) {
	//the table is set (the name and the column names) and allocated
	try {
		HydTemp_Profile::set_profile_boundary_table(ptr_database);
	}
	catch (Error msg) {
		throw msg;
	}

	//insert lateral boundary condition

	//insert lateral
	QSqlQueryModel model;
	//set the query via a query string
	ostringstream query_string;
	query_string << "INSERT INTO  " << HydTemp_Profile::boundary_table->get_table_name();
	query_string << " ( ";
	query_string << HydTemp_Profile::boundary_table->get_column_name(hyd_label::profdata_glob_id) << " , ";
	query_string << HydTemp_Profile::boundary_table->get_column_name(label::areastate_id) << " , ";
	query_string << HydTemp_Profile::boundary_table->get_column_name(label::measure_id) << " , ";
	query_string << HydTemp_Profile::boundary_table->get_column_name(label::applied_flag) << " , ";
	query_string << HydTemp_Profile::boundary_table->get_column_name(hyd_label::sz_bound_id) << " , ";
	query_string << HydTemp_Profile::boundary_table->get_column_name(hyd_label::bounddata_name) << " , ";
	query_string << HydTemp_Profile::boundary_table->get_column_name(hyd_label::bounddata_stat) << " , ";
	query_string << HydTemp_Profile::boundary_table->get_column_name(hyd_label::bounddata_value) << " , ";
	query_string << HydTemp_Profile::boundary_table->get_column_name(hyd_label::bounddata_type) << " ) ";


	if (this->water_bound_temp.applied_flag==true) {
		query_string << " VALUES ( ";
		query_string << glob_prof_id << " , ";
		query_string << this->system_id.area_state << " , ";
		query_string << this->system_id.measure_nr << " , ";
		query_string << functions::convert_boolean2string(true) << " , ";
		query_string << this->hyd_sz.get_id() << " , ";
		query_string << " '" << hyd_label::water_temp.c_str() << "' , ";

		query_string << functions::convert_boolean2string(this->water_bound_temp.stat_flag) << " , ";
		query_string << this->water_bound_temp.index_value << " , ";

		query_string << " '" << Hyd_Instationary_Boundary::transfrom_instatboundtype2txt(_hyd_bound_type::temperature).c_str() << "')";

	}
	else {


		query_string << " VALUES ( ";
		query_string << glob_prof_id << " , ";
		query_string << this->system_id.area_state << " , ";
		query_string << this->system_id.measure_nr << " , ";
		query_string << functions::convert_boolean2string(true) << " , ";
		query_string << this->hyd_sz.get_id() << " , ";
		query_string << " '" << hyd_label::air_temp.c_str() << "' , ";

		query_string << functions::convert_boolean2string(this->air_temp.stat_flag) << " , ";
		query_string << this->air_temp.index_value << " , ";

		query_string << " '" << Hyd_Instationary_Boundary::transfrom_instatboundtype2txt(_hyd_bound_type::temperature).c_str() << "'),";


		query_string << " ( ";
		query_string << glob_prof_id << " , ";
		query_string << this->system_id.area_state << " , ";
		query_string << this->system_id.measure_nr << " , ";
		query_string << functions::convert_boolean2string(true) << " , ";
		query_string << this->hyd_sz.get_id() << " , ";
		query_string << " '" << hyd_label::solar_rad.c_str() << "' , ";

		query_string << functions::convert_boolean2string(this->solar_rad.stat_flag) << " , ";
		query_string << this->solar_rad.index_value << " , ";

		query_string << " '" << Hyd_Instationary_Boundary::transfrom_instatboundtype2txt(_hyd_bound_type::radiation).c_str() << "'),";

		query_string << " ( ";
		query_string << glob_prof_id << " , ";
		query_string << this->system_id.area_state << " , ";
		query_string << this->system_id.measure_nr << " , ";
		query_string << functions::convert_boolean2string(true) << " , ";
		query_string << this->hyd_sz.get_id() << " , ";
		query_string << " '" << hyd_label::humidity.c_str() << "' , ";

		query_string << functions::convert_boolean2string(this->humid.stat_flag) << " , ";
		query_string << this->humid.index_value << " , ";

		query_string << " '" << Hyd_Instationary_Boundary::transfrom_instatboundtype2txt(_hyd_bound_type::percentage).c_str() << "'),";


		query_string << " ( ";
		query_string << glob_prof_id << " , ";
		query_string << this->system_id.area_state << " , ";
		query_string << this->system_id.measure_nr << " , ";
		query_string << functions::convert_boolean2string(true) << " , ";
		query_string << this->hyd_sz.get_id() << " , ";
		query_string << " '" << hyd_label::wind.c_str() << "' , ";

		query_string << " '" << functions::convert_boolean2string(this->wind.stat_flag) << "' , ";
		query_string << this->wind.index_value << " , ";

		query_string << " '" << Hyd_Instationary_Boundary::transfrom_instatboundtype2txt(_hyd_bound_type::speed).c_str() << "'),";

		query_string << " ( ";
		query_string << glob_prof_id << " , ";
		query_string << this->system_id.area_state << " , ";
		query_string << this->system_id.measure_nr << " , ";
		query_string << functions::convert_boolean2string(true) << " , ";
		query_string << this->hyd_sz.get_id() << " , ";
		query_string << " '" << hyd_label::cloud.c_str() << "' , ";

		query_string << functions::convert_boolean2string(this->cloud.stat_flag) << " , ";
		query_string << this->cloud.index_value << " , ";

		query_string << " '" << Hyd_Instationary_Boundary::transfrom_instatboundtype2txt(_hyd_bound_type::percentage).c_str() << "'),";


		query_string << " ( ";
		query_string << glob_prof_id << " , ";
		query_string << this->system_id.area_state << " , ";
		query_string << this->system_id.measure_nr << " , ";
		query_string << functions::convert_boolean2string(true) << " , ";
		query_string << this->hyd_sz.get_id() << " , ";
		query_string << " '" << hyd_label::shadow.c_str() << "' , ";

		query_string << functions::convert_boolean2string(this->shadow.stat_flag) << " , ";
		query_string << this->shadow.index_value << " , ";

		query_string << " '" << Hyd_Instationary_Boundary::transfrom_instatboundtype2txt(_hyd_bound_type::percentage).c_str() << "')";

		if (this->inlet_temp.applied_flag == true) {
			query_string << ", ( ";
			query_string << glob_prof_id << " , ";
			query_string << this->system_id.area_state << " , ";
			query_string << this->system_id.measure_nr << " , ";
			query_string << functions::convert_boolean2string(true) << " , ";
			query_string << this->hyd_sz.get_id() << " , ";
			query_string << " '" << hyd_label::inlet_temp.c_str() << "' , ";

			query_string << functions::convert_boolean2string(this->inlet_temp.stat_flag) << " , ";
			query_string << this->inlet_temp.index_value << " , ";

			query_string << " '" << Hyd_Instationary_Boundary::transfrom_instatboundtype2txt(_hyd_bound_type::temperature).c_str() << "')";
		}
	}




	Data_Base::database_request(&model, query_string.str(), ptr_database);

	if (model.lastError().isValid()) {
		Warning msg = this->set_warning(5);
		ostringstream info;
		info << "Table Name                : " << HydTemp_Profile::boundary_table->get_table_name() << endl;
		info << "Table error info          : " << model.lastError().text().toStdString() << endl;
		info << "Profile number            : " << this->profile_number << endl;
		info << "Profile name              : " << this->name << endl;
		info << "Command             : " << query_string.str() << endl;
		info << "Trying to insert the temperature boundary condition" << endl;
		msg.make_second_info(info.str());
		msg.output_msg(2);
	}





}
//Input the temperature profile boundary data from a database
void HydTemp_Profile::input_boundary_data_per_database(const int index, QSqlDatabase *ptr_database, const int glob_prof_id) {
	//mysql query with the table_model
	QSqlTableModel model(0, *ptr_database);
	//the table is set (the name and the column names) and allocated
	try {
		HydTemp_Profile::set_profile_boundary_table(ptr_database);
	}
	catch (Error msg) {
		throw msg;
	}

	//give the table name to the query model
	model.setTable((HydTemp_Profile::boundary_table->get_table_name()).c_str());


	//set the query
	ostringstream filter;
	filter << HydTemp_Profile::boundary_table->get_column_name(hyd_label::profdata_glob_id) << " = " << glob_prof_id;
	filter << " and ";
	filter << HydTemp_Profile::boundary_table->get_column_name(label::areastate_id) << " = " << this->system_id.area_state;
	filter << " and ";
	filter << HydTemp_Profile::boundary_table->get_column_name(label::measure_id) << " = " << this->system_id.measure_nr;
	filter << " and ";
	filter << HydTemp_Profile::boundary_table->get_column_name(hyd_label::sz_bound_id) << " = " << this->hyd_sz.get_id();
	filter << " and ";
	filter << HydTemp_Profile::boundary_table->get_column_name(label::applied_flag) << " = " << " true ";

	model.setFilter(filter.str().c_str());
	//launch the request
	Data_Base::database_request(&model);
	//check the query
	if (model.lastError().isValid()) {
		//error
		Error msg = set_error(0);
		ostringstream info;
		info << "Table Name      : " << HydTemp_Profile::boundary_table->get_table_name() << endl;
		info << "Table error info: " << model.lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		Sys_Common_Output::output_hyd->rewind_userprefix();
		throw msg;
	}
	bool water_temp_applied = false;
	for (int i = 0; i < model.rowCount(); i++) {
		if (model.record(i).value((HydTemp_Profile::boundary_table->get_column_name(hyd_label::bounddata_name)).c_str()).toString().toStdString() == hyd_label::water_temp) {
			water_temp_applied = true;
			break;
		}

	}

	if (index == 0) {
		if (water_temp_applied == true) {
			for (int i = 0; i < model.rowCount(); i++) {
				if (model.record(i).value((HydTemp_Profile::boundary_table->get_column_name(hyd_label::bounddata_name)).c_str()).toString().toStdString() == hyd_label::water_temp) {
					this->water_bound_temp.applied_flag = true;
					this->water_bound_temp.stat_flag = model.record(i).value((HydTemp_Profile::boundary_table->get_column_name(hyd_label::bounddata_stat)).c_str()).toBool();
					this->water_bound_temp.index_value = model.record(i).value((HydTemp_Profile::boundary_table->get_column_name(hyd_label::bounddata_value)).c_str()).toDouble();
					break;
				}

			}

		}
		else {
			//Error
			Error msg = set_error(8);
			ostringstream info;
			info << "Table Name      : " << HydTemp_Profile::boundary_table->get_table_name() << endl;
			info << "Global profile id " << glob_prof_id << endl;
			msg.make_second_info(info.str());
			Sys_Common_Output::output_hyd->rewind_userprefix();
			throw msg;



		}



	}
	else {
		if (water_temp_applied == true) {
			for (int i = 0; i < model.rowCount(); i++) {
				if (model.record(i).value((HydTemp_Profile::boundary_table->get_column_name(hyd_label::bounddata_name)).c_str()).toString().toStdString() == hyd_label::water_temp) {
					this->water_bound_temp.applied_flag = true;
					this->water_bound_temp.stat_flag = model.record(i).value((HydTemp_Profile::boundary_table->get_column_name(hyd_label::bounddata_stat)).c_str()).toBool();
					this->water_bound_temp.index_value = model.record(i).value((HydTemp_Profile::boundary_table->get_column_name(hyd_label::bounddata_value)).c_str()).toDouble();
					break;
				}

			}


		}
		else {

			if (model.rowCount() < 6 || model.rowCount() > 7) {
				//Error!!
				//error
				Error msg = set_error(1);
				ostringstream info;
				info << "Table Name      : " << HydTemp_Profile::boundary_table->get_table_name() << endl;
				info << "Number of boundaries found " << model.rowCount() << endl;
				info << "Global profile id " << glob_prof_id << endl;
				msg.make_second_info(info.str());
				Sys_Common_Output::output_hyd->rewind_userprefix();
				throw msg;


			}
			int count_found = 0;

			for (int i = 0; i < model.rowCount(); i++) {
				if (model.record(i).value((HydTemp_Profile::boundary_table->get_column_name(hyd_label::bounddata_name)).c_str()).toString().toStdString() == hyd_label::air_temp) {
					this->air_temp.applied_flag = true;
					this->air_temp.stat_flag = model.record(i).value((HydTemp_Profile::boundary_table->get_column_name(hyd_label::bounddata_stat)).c_str()).toBool();
					this->air_temp.index_value = model.record(i).value((HydTemp_Profile::boundary_table->get_column_name(hyd_label::bounddata_value)).c_str()).toDouble();
					count_found++;
					break;
				}

			}

			for (int i = 0; i < model.rowCount(); i++) {
				if (model.record(i).value((HydTemp_Profile::boundary_table->get_column_name(hyd_label::bounddata_name)).c_str()).toString().toStdString() == hyd_label::solar_rad) {
					this->solar_rad.applied_flag = true;
					this->solar_rad.stat_flag = model.record(i).value((HydTemp_Profile::boundary_table->get_column_name(hyd_label::bounddata_stat)).c_str()).toBool();
					this->solar_rad.index_value = model.record(i).value((HydTemp_Profile::boundary_table->get_column_name(hyd_label::bounddata_value)).c_str()).toDouble();
					count_found++;
					break;
				}

			}

			for (int i = 0; i < model.rowCount(); i++) {
				if (model.record(i).value((HydTemp_Profile::boundary_table->get_column_name(hyd_label::bounddata_name)).c_str()).toString().toStdString() == hyd_label::humidity) {
					this->humid.applied_flag = true;
					this->humid.stat_flag = model.record(i).value((HydTemp_Profile::boundary_table->get_column_name(hyd_label::bounddata_stat)).c_str()).toBool();
					this->humid.index_value = model.record(i).value((HydTemp_Profile::boundary_table->get_column_name(hyd_label::bounddata_value)).c_str()).toDouble();
					count_found++;
					break;
				}

			}

			for (int i = 0; i < model.rowCount(); i++) {
				if (model.record(i).value((HydTemp_Profile::boundary_table->get_column_name(hyd_label::bounddata_name)).c_str()).toString().toStdString() == hyd_label::wind) {
					this->wind.applied_flag = true;
					this->wind.stat_flag = model.record(i).value((HydTemp_Profile::boundary_table->get_column_name(hyd_label::bounddata_stat)).c_str()).toBool();
					this->wind.index_value = model.record(i).value((HydTemp_Profile::boundary_table->get_column_name(hyd_label::bounddata_value)).c_str()).toDouble();
					count_found++;
					break;
				}

			}

			for (int i = 0; i < model.rowCount(); i++) {
				if (model.record(i).value((HydTemp_Profile::boundary_table->get_column_name(hyd_label::bounddata_name)).c_str()).toString().toStdString() == hyd_label::cloud) {
					this->cloud.applied_flag = true;
					this->cloud.stat_flag = model.record(i).value((HydTemp_Profile::boundary_table->get_column_name(hyd_label::bounddata_stat)).c_str()).toBool();
					this->cloud.index_value = model.record(i).value((HydTemp_Profile::boundary_table->get_column_name(hyd_label::bounddata_value)).c_str()).toDouble();
					count_found++;
					break;
				}

			}

			for (int i = 0; i < model.rowCount(); i++) {
				if (model.record(i).value((HydTemp_Profile::boundary_table->get_column_name(hyd_label::bounddata_name)).c_str()).toString().toStdString() == hyd_label::shadow) {
					this->shadow.applied_flag = true;
					this->shadow.stat_flag = model.record(i).value((HydTemp_Profile::boundary_table->get_column_name(hyd_label::bounddata_stat)).c_str()).toBool();
					this->shadow.index_value = model.record(i).value((HydTemp_Profile::boundary_table->get_column_name(hyd_label::bounddata_value)).c_str()).toDouble();
					count_found++;
					break;
				}

			}

			for (int i = 0; i < model.rowCount(); i++) {
				if (model.record(i).value((HydTemp_Profile::boundary_table->get_column_name(hyd_label::bounddata_name)).c_str()).toString().toStdString() == hyd_label::inlet_temp) {
					this->inlet_temp.applied_flag = true;
					this->inlet_temp.stat_flag = model.record(i).value((HydTemp_Profile::boundary_table->get_column_name(hyd_label::bounddata_stat)).c_str()).toBool();
					this->inlet_temp.index_value = model.record(i).value((HydTemp_Profile::boundary_table->get_column_name(hyd_label::bounddata_value)).c_str()).toDouble();
					break;
				}

			}

			if (count_found != 6) {
				//error
				Error msg = set_error(6);
				ostringstream info;
				info << "Table Name      : " << HydTemp_Profile::boundary_table->get_table_name() << endl;
				msg.make_second_info(info.str());
				Sys_Common_Output::output_hyd->rewind_userprefix();
				throw msg;


			}
		}
	}

}
//Get boundary condition is applied
bool HydTemp_Profile::boundary_is_applied(void) {
	return false;
}
//Generate string for output max-results to database
void HydTemp_Profile::generate_max_str2database(ostringstream *query_string, const int rv_no, const string polygon_string, int *glob_id, const string break_sc) {
	*query_string << "INSERT INTO  " << HydTemp_Profile::erg_table->get_table_name();
	*query_string << " ( ";
	*query_string << HydTemp_Profile::erg_table->get_column_name(label::glob_id) << " , ";
	*query_string << HydTemp_Profile::erg_table->get_column_name(hyd_label::profdata_rvno) << " , ";
	*query_string << HydTemp_Profile::erg_table->get_column_name(hyd_label::profdata_prof_id) << " , ";
	*query_string << HydTemp_Profile::erg_table->get_column_name(label::areastate_id) << " , ";
	*query_string << HydTemp_Profile::erg_table->get_column_name(label::measure_id) << " , ";
	*query_string << HydTemp_Profile::erg_table->get_column_name(hyd_label::sz_bound_id) << " , ";
	*query_string << HydTemp_Profile::erg_table->get_column_name(risk_label::sz_break_id) << " , ";

	//max values
	*query_string << HydTemp_Profile::erg_table->get_column_name(hyd_label::proferg_T_max) << " , ";
	*query_string << HydTemp_Profile::erg_table->get_column_name(hyd_label::proferg_t_T_max) << " , ";
	*query_string << HydTemp_Profile::erg_table->get_column_name(hyd_label::proferg_T_min) << " , ";
	*query_string << HydTemp_Profile::erg_table->get_column_name(hyd_label::proferg_t_T_min) << " , ";
	*query_string << HydTemp_Profile::erg_table->get_column_name(hyd_label::proferg_T_av) << " , ";
	
	*query_string << HydTemp_Profile::erg_table->get_column_name(hyd_label::proferg_polygon) << " ) ";



	*query_string << " VALUES ( ";
	*query_string << *glob_id << " , ";
	*query_string << rv_no << " , ";
	*query_string << this->profile_number << " , ";
	*query_string << this->system_id.area_state << " , ";
	*query_string << this->system_id.measure_nr << " , ";
	*query_string << this->hyd_sz.get_id() << " , ";
	*query_string << "'" << break_sc << "' , ";

	//max values
	*query_string << this->temp_value_max.maximum << " , ";
	*query_string << this->temp_value_max.time_point << " , ";
	*query_string << this->temp_value_min.maximum << " , ";
	*query_string << this->temp_value_min.time_point << " , ";
	*query_string << this->temp_average << " , ";
	
	*query_string << polygon_string << " ) ";




}
//Delete all data in the database table for the river profile data (static)
void HydTemp_Profile::delete_data_in_profile_table(QSqlDatabase *ptr_database){
	//the table is set (the name and the column names) and allocated
	try{
		HydTemp_Profile::set_profile_table(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}
	//delete the table
	HydTemp_Profile::profile_table->delete_data_in_table(ptr_database);
}
//Close and delete the database table for the river profile data (static)
void HydTemp_Profile::close_profile_table(void){
	if(HydTemp_Profile::profile_table!=NULL){
		delete HydTemp_Profile::profile_table;
		HydTemp_Profile::profile_table=NULL;
	}
}
//Delete all data in the database table for the boundary profile data (static)
void HydTemp_Profile::delete_data_in_boundary_table(QSqlDatabase *ptr_database){
	//the table is set (the name and the column names) and allocated
	try{
		HydTemp_Profile::set_profile_boundary_table(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}
	//delete the table
	HydTemp_Profile::boundary_table->delete_data_in_table(ptr_database);
}
//Close and delete the database table for the boundary profile data (static)
void HydTemp_Profile::close_boundary_table(void){
	if(HydTemp_Profile::boundary_table!=NULL){
		delete HydTemp_Profile::boundary_table;
		HydTemp_Profile::boundary_table=NULL;
	}
}
//Delete all data in the database table for the results of an hydraulic simulation the river profiles (static)
void HydTemp_Profile::delete_data_in_erg_table(QSqlDatabase *ptr_database){
	//the table is set (the name and the column names) and allocated
	try{
		HydTemp_Profile::set_erg_table(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}
	//delete the table
	HydTemp_Profile::erg_table->delete_data_in_table(ptr_database);
}
//Delete all data in the database table for the instationary results of an hydraulic simulation the river profiles
void HydTemp_Profile::delete_data_in_erg_instat_table(QSqlDatabase *ptr_database) {
	//the table is set (the name and the column names) and allocated
	try {
		HydTemp_Profile::set_erg_instat_table(ptr_database);
	}
	catch (Error msg) {
		throw msg;
	}
	//delete the table
	HydTemp_Profile::erg_instat_table->delete_data_in_table(ptr_database);

}
//Delete the maximum result row in a database table for this profile
void HydTemp_Profile::delete_max_result_row_in_table(QSqlDatabase *ptr_database, const int rv_no, const string break_sc){
	//the table is set (the name and the column names) and allocated
	try{
		HydTemp_Profile::set_erg_table(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}

	QSqlQuery query(*ptr_database);
	ostringstream query_string;
	query_string << "DELETE FROM " << HydTemp_Profile::erg_table->get_table_name() << " ";
	query_string << " WHERE " << HydTemp_Profile::erg_table->get_column_name(hyd_label::profdata_rvno) << " = " << rv_no;
	query_string << " AND ";
	query_string << HydTemp_Profile::erg_table->get_column_name(hyd_label::profdata_prof_id) << " = " << this->profile_number;
	query_string << " AND ";
	query_string << HydTemp_Profile::erg_table->get_column_name(label::areastate_id) << " = " << this->system_id.area_state;
	query_string << " AND ";
	query_string << HydTemp_Profile::erg_table->get_column_name(label::measure_id) << " = " << this->system_id.measure_nr;
	query_string << " AND ";
	query_string << HydTemp_Profile::erg_table->get_column_name(hyd_label::sz_bound_id) << " = " << this->hyd_sz.get_id();
	query_string << " AND ";
	query_string << HydTemp_Profile::erg_table->get_column_name(risk_label::sz_break_id) << " = '" << break_sc<<"'";

	query.exec(query_string.str().c_str());
}
//set the error
Error HydTemp_Profile::set_error(const int err_type){
		string place="HydTemp_Profile::";
		string help;
		string reason;
		int type=0;
		bool fatal=false;
		stringstream info;
		Error msg;

	switch (err_type){
		case 0://
			place.append("input_boundary_data_per_database(QSqlDatabase *ptr_database, const int glob_prof_id)");
			reason = "Can not submit the query to the database";
			help = "Check the query error";
			type = 6;
			break;
		case 1://
			place.append("input_boundary_data_per_database(QSqlDatabase *ptr_database, const int glob_prof_id)");
			reason = "Not enough temperature boundary data are found in database table; 6 or, with inlet temperature, 7 are required";
			help = "Check the database table";
			type = 14;
			break;
		case 2://not all needed values are found
			place.append("input_members(ifstream *profile_file, const long int file_position, const int number_profile_points)");
			reason="Not all needed key values are found";
			help="Check the temperature data in the profile file;";
			type=14;
			break;
		case 3://wrong input
			place.append("input_members(QFile *profile_file, const int profile_number, string my_line, int *line_counter)");
			reason="There is a problem with the input; wrong sign is read in";
			help="Check the temperature data in the profile file";
			type=1;
			break;
		case 4://wrong input
			place.append("decide_keyvalues_file(QString line, _hyd_temp_data *to_reset, int *found_counter)");
			reason = "There is a problem with the input; wrong sign is read in";
			help = "Check the temperature data in the profile file";
			type = 1;
			break;
		case 5://init connditions are to low
			place.append("check_profiles(void)");
			reason="The temperature initial conditions of a river profile can not be below 0.0 Kelvin";
			help="Check the Profile in the profile file";
			type=14;
			break;
		case 6://
			place.append("input_boundary_data_per_database(QSqlDatabase *ptr_database, const int glob_prof_id)");
			reason = "Wrong temperature boundary data are found in database table; 6 different types are required";
			help = "Check the database table";
			type = 6;
			break;
		case 7://wrong input
			place.append("input_members(QFile *profile_file, const int profile_number, string my_line, int *line_counter)");
			reason = "The inflow profile needs to have a water temperature boundary (Keyword: WATER_TEMP); all other bounary values are not taken into account";
			help = "Check the temperature data in the profile file for the inflow profile (first profile)";
			type = 1;
			break;
		case 8://wrong input
			place.append("input_boundary_data_per_database(const int index, QSqlDatabase *ptr_database, const int glob_prof_id)");
			reason = "The inflow profile needs to have a water temperature boundary";
			help = "Check the temperature data in the profile table for the inflow profile (first profile)";
			type = 4;
			break;
		case 9://wrong input
			place.append("check_profiles(void)");
			reason = "A temperature inlet boundary is applied; but no point inflow boundary in the river profile";
			help = "Check the Profile in the profile file (Temp and Hyd)";
			type = 14;
			break;
		case 10://wrong input
			place.append("check_profiles(void)");
			reason = "No river profile is available for inlet boundary condition; first or last river profile are not allowed!";
			help = "Check the Profile in the profile file (Temp and Hyd)";
			type = 14;
			break;
		case 11://wrong input
			place.append("make_syncronisation(const int index,  const double time_point, HydTemp_Param *params, HydTemp_Profile *upstream, HydTemp_Profile *downstream, Hyd_River_Profile_Connection_Standard *rv_profile)");
			reason = "No river profile is available for inlet boundary condition; first or last river profile are not allowed!";
			help = "Check the Profile in the profile file (Temp and Hyd)";
			type = 14;
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
		case 14://check all boundary curve are found
			place.append("connect_instat_boundarycurve(Hyd_Instationary_Boundary *bound_curve, const int number)");
			reason = "No all instationary boundary curves could be connected to the temperature profile";
			help = "Check the instationary boundary data";
			type = 14;
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
Warning HydTemp_Profile::set_warning(const int warn_type){
		string place="HydTemp_Profile::";
		string help;
		string reason;
		string reaction;
		int type=0;
		Warning msg;
		stringstream info;

	switch (warn_type){
		case 0://air temp to low
			place.append("check_profiles(void)") ;
			reason="The air temperature can not be below 0.0 Kelvin ";
			reaction="It is set to 283 Kelvin";
			type=9;//9 was here before
			break;
		case 2://input datas can not submitted
			place.append("transfer_profile_members2database(QSqlDatabase *ptr_database, const int rv_number)");
			reason="Can not submit the data of the temperature profile to the database";
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
		case 5://transfer boundary to database
			place.append("transfer_boundary_data2database(QSqlDatabase *ptr_database, const int glob_prof_id)");
			reason = "Can not submit the boundary data of the temperature profile to the database";
			help = "Check the database";
			type=2;
			break;
		case 6://humidity to low or high
			place.append("check_profiles(void)");
			reason = "The humidity can not be below 0.0 or above 1.0. It must be a value between 0-1.";
			reaction = "It is set to 0.8";
			type = 9;
			break;
		case 7://shadow is to low or high
			place.append("check_profiles(void)");
			reason = "The shadow factor can not be below 0.0 or above 1.0. It must be a value between 0-1.";
			reaction = "It is set to 0.5";
			type = 9;
			break;
		case 8://solar radiation is set stationary
			place.append("check_profiles(void)");
			reason = "The solar radiation is set as a stationary boundary condition. This will probably cause a permanent rise of temperature.";
			type = 9;
			break;
		case 9://result datas can not submitted
			place.append("output_instat_results(QSqlDatabase *ptr_database, const int rv_no, const string polygon_string, int *glob_id, const string break_sc, const string time)");
			reason = "Can not submit the results data of the river profile to the database";
			help = "Check the database";
			type = 2;
			break;
		case 10://a water temp boundary is set
			place.append("input_members(QFile *profile_file, const int profile_number, string my_line, int *line_counter)");
			reason = "A water temperature boundary is set; all other boundary values will not be applied";
			help = "Check the profile file";
			reaction = "The other boundary values are not further taken into account";
			type = 1;
			break;
		case 11://a inlet temp boundary is set
			place.append("input_members(QFile *profile_file, const int profile_number, string my_line, int *line_counter)");
			reason = "A inlet temperature boundary is set; all other boundary values will not be applied";
			help = "Check the profile file";
			reaction = "The other boundary values are not further taken into account";
			type = 1;
			break;
		case 12://solar radiation to low
			place.append("check_profiles(void)");
			reason = "The solar radiation can not be below 0.0. It must be a value above 0.0.";
			reaction = "It is set to 150";
			type = 9;
			break;
		case 13://wind velocity to low
			place.append("check_profiles(void)");
			reason = "The wind velocity can not be below 0.0. It must be a value above 0.0.";
			reaction = "It is set to 2";
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
