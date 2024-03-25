#include "Fpl_Headers_Precompiled.h"
//#include "Fpl_Dike_Geo_Materialzone.h"

//init static members
Tables *Fpl_Dike_Geo_Materialzone::matzone_table=NULL;
Tables *Fpl_Dike_Geo_Materialzone::matzone_point_table=NULL;

//Default constructor
Fpl_Dike_Geo_Materialzone::Fpl_Dike_Geo_Materialzone(void){

	this->zone_number=-1;
	this->zone_name=label::not_set;
	this->variable_zone_number=-1;

	this->zone_type=_fpl_zone_type::not_set_zone;

	
	this->variables=NULL;

	Sys_Memory_Count::self()->add_mem(sizeof(Fpl_Dike_Geo_Materialzone)-sizeof(Geo_Simple_Polygon), _sys_system_modules::FPL_SYS);//count the memory
}
//Default destructor
Fpl_Dike_Geo_Materialzone::~Fpl_Dike_Geo_Materialzone(void){

	Sys_Memory_Count::self()->minus_mem(sizeof(Fpl_Dike_Geo_Materialzone)-sizeof(Geo_Simple_Polygon), _sys_system_modules::FPL_SYS);//count the memory
}
//__________
//public
//Read in the material zone from file
void Fpl_Dike_Geo_Materialzone::read_matzone_per_file(QFile *ifile, int *line_counter){

	qint64 pos_file=0;
	int store_line=0;
	int must_found_counter=0;
	

	//find the start of the info block
	try{
		pos_file=this->find_start_block_file(ifile, line_counter, fpl_label::begin_material_zone_info, fpl_label::end_material_zone_info, true);
		store_line=*line_counter;
	}
	catch(Error msg){
		throw msg;
	}

	//read in
	string myline;
	QString qmyline;
	//read in the zone infos
	do{
		qmyline=ifile->readLine();
		myline=qmyline.toStdString();
		(*line_counter)++;
		functions::clean_string(&myline);
		if(myline==fpl_label::end_material_zone_info){
			break;
		}
		if(myline.empty()==false){
			this->find_key_values_file(myline, &must_found_counter);
		}
		if(must_found_counter==3){
			break;
		}
	}
	while(ifile->atEnd()==false);


	if(must_found_counter!=3){
		Error msg=this->set_error(4);
		throw msg;
	}

	ostringstream name;
	name<<"Zone_"<<this->zone_number<<"_"<<this->zone_name;

	//read in the points
	Geo_Point *buffer=NULL;
	try{
		buffer=new Geo_Point[this->number_segments];
	}
	catch(bad_alloc &t){
		Error msg=this->set_error(5);
		ostringstream info;
		info<< "Info bad alloc: " << t.what() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}


	int counter_point=0;
	try{
		do{
			qmyline=ifile->readLine();
			myline=qmyline.toStdString();
			(*line_counter)++;
			functions::clean_string(&myline);
			if(myline==fpl_label::end_material_zone_info){
				break;
			}
			else if(myline.empty()==false){
				double buff_x=0.0;
				double buff_y=0.0;
				stringstream point_buff;
				point_buff << myline;
				point_buff >> buff_x >> buff_y;

	
				if(point_buff.fail()==true){
					Error msg=this->set_error(6);
					ostringstream info;
					info << "Read line : " << myline << endl;
					msg.make_second_info(info.str());
					throw msg;
				}
				buffer[counter_point].set_point_coordinate(buff_x, buff_y);
				buffer[counter_point].set_point_name(name.str());
				counter_point++;
			}

		}
		while(ifile->atEnd()==false && counter_point<this->number_segments);

	}
	catch(Error msg){
		throw msg;
	}
	if(counter_point!=this->number_segments){
		Error msg=this->set_error(7);
		ostringstream info;
		info << "Number points required : " << this->number_segments << endl;
		info << "Number points found    : " << counter_point << endl;
		msg.make_second_info(info.str());
		throw msg;
	}


	this->set_points(buffer);




	if(buffer!=NULL){
		delete []buffer;
		buffer=NULL;
	}

	//search for the end
	bool found_flag=false;
	do{
		qmyline=ifile->readLine();
		myline=qmyline.toStdString();
		(*line_counter)++;
		functions::clean_string(&myline);

		if(myline==fpl_label::end_material_zone_info){
			found_flag=true;
			break;
		}
	}
	while(ifile->atEnd()==false);


	if(found_flag==false ){
		Error msg=this->set_error(8);
		ostringstream info;
		info << "End keyword   : " << fpl_label::end_material_zone_info<<endl;
		msg.make_second_info(info.str());
 		throw msg;

	}
	

}
//Transfer the members read in per file to database
void Fpl_Dike_Geo_Materialzone::transfer_matzone2database(QSqlDatabase *ptr_database, const int section_id){

	try{
		Fpl_Dike_Geo_Materialzone::set_matzone_table(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}

	int glob_id=Fpl_Dike_Geo_Materialzone::matzone_table->maximum_int_of_column(Fpl_Dike_Geo_Materialzone::matzone_table->get_column_name(fpl_label::glob_id), ptr_database)+1;

	//transfer the data
	ostringstream query_string;
	query_string << "INSERT INTO  " << Fpl_Dike_Geo_Materialzone::matzone_table->get_table_name();
	query_string <<" ( ";
	query_string << Fpl_Dike_Geo_Materialzone::matzone_table->get_column_name(fpl_label::glob_id) <<" , ";
	query_string << Fpl_Dike_Geo_Materialzone::matzone_table->get_column_name(fpl_label::section_id) <<" , ";

	query_string << Fpl_Dike_Geo_Materialzone::matzone_table->get_column_name(label::areastate_id) <<" , ";
	query_string << Fpl_Dike_Geo_Materialzone::matzone_table->get_column_name(label::measure_id) <<" , ";
	query_string << Fpl_Dike_Geo_Materialzone::matzone_table->get_column_name(label::applied_flag) <<" , ";

	query_string << Fpl_Dike_Geo_Materialzone::matzone_table->get_column_name(fpl_label::zone_id) <<" , ";
	query_string << Fpl_Dike_Geo_Materialzone::matzone_table->get_column_name(fpl_label::zone_var_id) <<" , ";
	query_string << Fpl_Dike_Geo_Materialzone::matzone_table->get_column_name(fpl_label::zone_type) <<" , ";
	query_string << Fpl_Dike_Geo_Materialzone::matzone_table->get_column_name(fpl_label::zone_name);
	query_string <<" ) ";
	query_string << " VALUES ( ";

	query_string << glob_id << " , " ;
	query_string << section_id<< " , " ;

	query_string << this->system_id.area_state << " , " ;
	query_string << this->system_id.measure_nr<< " , " ;
	query_string << "true" << " , " ;

	query_string << this->zone_number<< " , " ;
	query_string << this->variable_zone_number<< " , " ;
	query_string <<"'"<< Fpl_Dike_Geo_Materialzone::convert_zone_type2txt(this->zone_type) << "' , " ;
	query_string <<"'"<< this->zone_name << "' ) " ;




	//Set the query
	QSqlQuery query_buff(*ptr_database);
	//launch the request
	Data_Base::database_request(&query_buff, query_string.str(), ptr_database);

	if(query_buff.lastError().isValid()){
		Warning msg=this->set_warning(0);
		ostringstream info;
		info << "Table Name                : " << Fpl_Dike_Geo_Materialzone::matzone_table->get_table_name() << endl;
		info << "Table error info          : " << query_buff.lastError().text().toStdString() << endl;
		info << "Zone name                 : " << this->zone_name << endl;
		info << "Zone id                   : " << this->zone_number << endl;
		msg.make_second_info(info.str());
		msg.output_msg(1);	
	}
	
	this->transfer_point_data2database(ptr_database, section_id);
	
}
//Set the input of the material zones per database
void Fpl_Dike_Geo_Materialzone::set_input(QSqlQueryModel *results, const int index, const int section_id,  QSqlDatabase *ptr_database){
	//output
	ostringstream cout;
	cout <<"Set the material zone "<< index << " of the fpl-section... "  << endl ;
	Sys_Common_Output::output_fpl->output_txt(&cout,false);


	//set the members of the zone
	this->zone_number=results->record(index).value((Fpl_Dike_Geo_Materialzone::matzone_table->get_column_name(fpl_label::zone_id)).c_str()).toInt();
	this->zone_name=results->record(index).value((Fpl_Dike_Geo_Materialzone::matzone_table->get_column_name(fpl_label::zone_name)).c_str()).toString().toStdString();
	this->variable_zone_number=results->record(index).value((Fpl_Dike_Geo_Materialzone::matzone_table->get_column_name(fpl_label::zone_var_id)).c_str()).toInt();
	this->zone_type=Fpl_Dike_Geo_Materialzone::convert_txt2zone_type(results->record(index).value((Fpl_Dike_Geo_Materialzone::matzone_table->get_column_name(fpl_label::zone_type)).c_str()).toString().toStdString());

	ostringstream name;
	name<<"Zone_"<<this->zone_number<<"_"<<this->zone_name;
	//set the points
	QSqlQueryModel point_result;
	Geo_Point *buffer=NULL;
	try{
		int number_points=Fpl_Dike_Geo_Materialzone::select_relevant_matzone_points_database(&point_result, ptr_database, this->system_id, section_id, this->zone_number);
		
		if(number_points==0){
			Error msg=this->set_error(1);
			throw msg;
		}

		number_points=number_points;
		try{
			buffer=new Geo_Point[number_points];
		}
		catch(bad_alloc &t){
			Error msg=this->set_error(0);
			ostringstream info;
			info<< "Info bad alloc: " << t.what() << endl;
			msg.make_second_info(info.str());
			throw msg;
		}

		//read in the points
		double coor=0.0;
		for(int i=0; i< number_points; i++){
			coor=point_result.record(i).value((Fpl_Dike_Geo_Materialzone::matzone_point_table->get_column_name(fpl_label::x_coor)).c_str()).toDouble();
			buffer[i].set_point_x_coordinate(coor);
			coor=point_result.record(i).value((Fpl_Dike_Geo_Materialzone::matzone_point_table->get_column_name(fpl_label::y_coor)).c_str()).toDouble();
			buffer[i].set_point_y_coordinate(coor);
			buffer[i].set_point_name(name.str());
		}

		//set it to the polygon
		this->set_number_points(number_points);
		this->set_points(buffer);

		//set name
	}
	catch(Error msg){
		if(buffer!=NULL){
			delete []buffer;
			buffer=NULL;
		}
		ostringstream info;
		info<< " Zone name   : "<<this->zone_name<<endl;
		info<< " Zone number : "<<this->zone_number<<endl;
		msg.make_second_info(info.str());
		throw msg;
	}

	//delete the buffer
	if(buffer!=NULL){
		delete []buffer;
		buffer=NULL;
	}

}
//Create the database table for the material zones (static)
void Fpl_Dike_Geo_Materialzone::create_matzone_table(QSqlDatabase *ptr_database){
	if(Fpl_Dike_Geo_Materialzone::matzone_table==NULL){
			ostringstream cout;
			cout << "Create material zones database table..." << endl ;
			Sys_Common_Output::output_fpl->output_txt(&cout);
			//make specific input for this class
			const string tab_name=fpl_label::tab_fpl_material_zones;
			const int num_col=9;
			_Sys_data_tab_column tab_col[num_col];
			//init
			for(int i=0; i< num_col; i++){
				tab_col[i].key_flag=false;
				tab_col[i].unsigned_flag=false;
				tab_col[i].primary_key_flag=false;
			}


			tab_col[0].name=fpl_label::glob_id;
			tab_col[0].type=sys_label::tab_col_type_int;
			tab_col[0].unsigned_flag=true;
			tab_col[0].primary_key_flag=true;

			tab_col[1].name=fpl_label::section_id;
			tab_col[1].type=sys_label::tab_col_type_int;
			tab_col[1].unsigned_flag=true;
			tab_col[1].key_flag=true;

			tab_col[2].name=label::areastate_id;
			tab_col[2].type=sys_label::tab_col_type_int;
			tab_col[2].unsigned_flag=true;
			tab_col[2].key_flag=true;

			tab_col[3].name=label::measure_id;
			tab_col[3].type=sys_label::tab_col_type_int;
			tab_col[3].unsigned_flag=true;
			tab_col[3].key_flag=true;

			tab_col[4].name=label::applied_flag;
			tab_col[4].type=sys_label::tab_col_type_bool;
			tab_col[4].default_value="true";
			tab_col[4].key_flag=true;

			tab_col[5].name=fpl_label::zone_id;
			tab_col[5].type=sys_label::tab_col_type_int;
			tab_col[5].unsigned_flag=true;

			tab_col[6].name=fpl_label::zone_name;
			tab_col[6].type=sys_label::tab_col_type_string;

			tab_col[7].name=fpl_label::zone_var_id;
			tab_col[7].type=sys_label::tab_col_type_int;
			tab_col[7].unsigned_flag=true;

			tab_col[8].name=fpl_label::zone_type;
			tab_col[8].type=sys_label::tab_col_type_string;

			


			try{
				Fpl_Dike_Geo_Materialzone::matzone_table= new Tables();
				if(Fpl_Dike_Geo_Materialzone::matzone_table->create_non_existing_tables(tab_name, tab_col, num_col,ptr_database, _sys_table_type::fpl)==false){
					cout << " Table exists" << endl ;
					Sys_Common_Output::output_fpl->output_txt(&cout);
				};
			}
			catch(bad_alloc& t){
				Error msg;
				msg.set_msg("Fpl_Dike_Geo_Materialzone::create_matzone_table(QSqlDatabase *ptr_database)","Can not allocate the memory", "Check the memory", 10, false);
				ostringstream info;
				info<< "Info bad alloc: " << t.what() << endl;
				msg.make_second_info(info.str());
				throw msg;	
			}
			catch(Error msg){
				Fpl_Dike_Geo_Materialzone::close_matzone_table();
				throw msg;
			}

		Fpl_Dike_Geo_Materialzone::close_matzone_table();
	}
}
//Delete all data in the database table for the material zones (static)
void Fpl_Dike_Geo_Materialzone::delete_data_in_matzone_table(QSqlDatabase *ptr_database){
	//the table is set (the name and the column names) and allocated
	try{
		Fpl_Dike_Geo_Materialzone::set_matzone_table(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}
	//delete the table
	Fpl_Dike_Geo_Materialzone::matzone_table->delete_data_in_table(ptr_database);
	Fpl_Dike_Geo_Materialzone::delete_data_in_matzone_point_table(ptr_database);

}
//Delete the data in the database table for the material zones of a specific fpl-sections (static)
void Fpl_Dike_Geo_Materialzone::delete_data_in_matzone_table(QSqlDatabase *ptr_database,  const _sys_system_id id, const int sec_id){

	QSqlQueryModel results;
	ostringstream test_filter;
	try{
		Fpl_Dike_Geo_Materialzone::set_matzone_table(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}

	//delete the points
	//delete 
	test_filter<< "Delete ";
	test_filter << " from " << Fpl_Dike_Geo_Materialzone::matzone_table->get_table_name();
	test_filter << " where ";
	test_filter << Fpl_Dike_Geo_Materialzone::matzone_table->get_column_name(fpl_label::section_id) << " = " << sec_id << " and ";
	test_filter << Fpl_Dike_Geo_Materialzone::matzone_table->get_column_name(label::areastate_id) << " = " << id.measure_nr << " and ";
	test_filter << Fpl_Dike_Geo_Materialzone::matzone_table->get_column_name(label::measure_id) << " = " << id.area_state;
	Data_Base::database_request(&results, test_filter.str(), ptr_database);

	//check the request
	if(results.lastError().isValid()){
		Error msg;
		msg.set_msg("Fpl_Dike_Geo_Materialzone::delete_data_in_matzone_table(QSqlDatabase *ptr_database,  const _sys_system_id id, const int sec_id)","Invalid database request", "Check the database", 2, false);
		ostringstream info;
		info << "Table Name      : " << Fpl_Dike_Geo_Materialzone::matzone_table->get_table_name() << endl;
		info << "Table error info: " << results.lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}

	Fpl_Dike_Geo_Materialzone::delete_data_in_matzone_point_table(ptr_database,id,sec_id);
}
//Set the database table for the material zones: it sets the table name and the name of the columns and allocate them (static)
void Fpl_Dike_Geo_Materialzone::set_matzone_table(QSqlDatabase *ptr_database){
	if(Fpl_Dike_Geo_Materialzone::matzone_table==NULL){
		//make specific input for this class
		const string tab_id_name=fpl_label::tab_fpl_material_zones;//FPL:=FloodProtectionLine
		string tab_col[9];
		tab_col[0]=fpl_label::glob_id;
		tab_col[1]=fpl_label::section_id;
		tab_col[2]=label::areastate_id;
		tab_col[3]=label::measure_id;
		tab_col[4]=label::applied_flag;
		tab_col[5]=fpl_label::zone_id;
		tab_col[6]=fpl_label::zone_name;
		tab_col[7]=fpl_label::zone_var_id;
		tab_col[8]=fpl_label::zone_type;


		//set the table
		try{
			Fpl_Dike_Geo_Materialzone::matzone_table= new Tables(tab_id_name, tab_col, sizeof(tab_col)/sizeof(tab_col[0]));
			Fpl_Dike_Geo_Materialzone::matzone_table->set_name(ptr_database, _sys_table_type::fpl);
		}
		catch(bad_alloc &t){
			Error msg;
			msg.set_msg("Fpl_Dike_Geo_Materialzone::set_matzone_table(QSqlDatabase *ptr_database)","Can not allocate the memory","Check the memory",10,false);
			ostringstream info;
			info<< "Info bad alloc: " << t.what() << endl;
			msg.make_second_info(info.str());
			throw msg;	
		}
		catch(Error msg){
			Fpl_Dike_Geo_Materialzone::close_matzone_table();
			throw msg;
		}
	}
}
//Close and delete the database table for the material zones (static)
void Fpl_Dike_Geo_Materialzone::close_matzone_table(void){
	if(Fpl_Dike_Geo_Materialzone::matzone_table!=NULL){
		delete Fpl_Dike_Geo_Materialzone::matzone_table;
		Fpl_Dike_Geo_Materialzone::matzone_table=NULL;
	}
}
//Switch the applied-flag material zones in the database table for a defined system state (static)
void Fpl_Dike_Geo_Materialzone::switch_applied_flag_matzone_table(QSqlDatabase *ptr_database, const _sys_system_id id, const bool flag){
	//the table is set (the name and the column names) and allocated
	try{
		Fpl_Dike_Geo_Materialzone::set_matzone_table(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}
	QSqlQueryModel query;

	ostringstream query_string;
	query_string <<"UPDATE ";
	query_string << Fpl_Dike_Geo_Materialzone::matzone_table->get_table_name();
	query_string << " SET " ;
	query_string  << Fpl_Dike_Geo_Materialzone::matzone_table->get_column_name(label::applied_flag) << " = "<< functions::convert_boolean2string(flag);
	query_string  << " WHERE ";
	query_string  << Fpl_Dike_Geo_Materialzone::matzone_table->get_column_name(label::areastate_id) << " = "  << id.area_state;
	query_string  << " AND ";
	query_string  <<  Fpl_Dike_Geo_Materialzone::matzone_table->get_column_name(label::measure_id) << " = " << id.measure_nr ;
	


	Data_Base::database_request(&query, query_string.str(), ptr_database);
	if(query.lastError().isValid()==true){
		Error msg;
		msg.set_msg("Fpl_Dike_Geo_Materialzone::switch_applied_flag_matzone_table(QSqlDatabase *ptr_database, const _sys_system_id id, const bool flag)","Invalid database request", "Check the database", 2, false);
		ostringstream info;
		info << "Table Name      : " << Fpl_Dike_Geo_Materialzone::matzone_table->get_table_name() << endl;
		info << "Table error info: " << query.lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
	Fpl_Dike_Geo_Materialzone::switch_applied_flag_matzone_point_table(ptr_database, id, flag);
}
//Switch the applied-flag material zones in the database table for a defined system state and section id (static)
void Fpl_Dike_Geo_Materialzone::switch_applied_flag_matzone_table(QSqlDatabase *ptr_database, const _sys_system_id id, const bool flag, const int sec_id){
//the table is set (the name and the column names) and allocated
	try{
		Fpl_Dike_Geo_Materialzone::set_matzone_table(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}
	QSqlQueryModel query;

	ostringstream query_string;
	query_string <<"UPDATE ";
	query_string << Fpl_Dike_Geo_Materialzone::matzone_table->get_table_name();
	query_string << " SET " ;
	query_string  << Fpl_Dike_Geo_Materialzone::matzone_table->get_column_name(label::applied_flag) << " = "<< functions::convert_boolean2string(flag);
	query_string  << " WHERE ";
	query_string  << Fpl_Dike_Geo_Materialzone::matzone_table->get_column_name(label::areastate_id) << " = "  << id.area_state;
	query_string  << " AND ";
	query_string  <<  Fpl_Dike_Geo_Materialzone::matzone_table->get_column_name(label::measure_id) << " = " << id.measure_nr ;
	query_string  << " AND ";
	query_string  <<  Fpl_Dike_Geo_Materialzone::matzone_table->get_column_name(fpl_label::section_id) << " = " << sec_id ;
	


	Data_Base::database_request(&query, query_string.str(), ptr_database);
	if(query.lastError().isValid()==true){
		Error msg;
		msg.set_msg("Fpl_Dike_Geo_Materialzone::switch_applied_flag_matzone_table(QSqlDatabase *ptr_database, const _sys_system_id id, const bool flag, const int sec_id)","Invalid database request", "Check the database", 2, false);
		ostringstream info;
		info << "Table Name      : " << Fpl_Dike_Geo_Materialzone::matzone_table->get_table_name() << endl;
		info << "Table error info: " << query.lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
	Fpl_Dike_Geo_Materialzone::switch_applied_flag_matzone_point_table(ptr_database, id, flag, sec_id);
}
//Copy the material zones of a fpl-section of a given system id to another one in database tables (static)
void Fpl_Dike_Geo_Materialzone::copy_fpl_matzone(QSqlDatabase *ptr_database, const _sys_system_id src, const _sys_system_id dest, const int sec_id){

	try{
		Fpl_Dike_Geo_Materialzone::set_matzone_table(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}

	int glob_id=0;
	glob_id=Fpl_Dike_Geo_Materialzone::matzone_table->maximum_int_of_column(Fpl_Dike_Geo_Materialzone::matzone_table->get_column_name(fpl_label::glob_id) ,ptr_database)+1;

	QSqlQueryModel model;
	ostringstream test_filter;
	test_filter<< "SELECT ";
	test_filter << Fpl_Dike_Geo_Materialzone::matzone_table->get_column_name(fpl_label::glob_id);
	test_filter << " from " << Fpl_Dike_Geo_Materialzone::matzone_table->get_table_name();
	test_filter << " WHERE ";
	test_filter << Fpl_Dike_Geo_Materialzone::matzone_table->get_column_name(label::areastate_id) << " = " << src.area_state;
	test_filter << " AND ";
	test_filter << Fpl_Dike_Geo_Materialzone::matzone_table->get_column_name(label::measure_id) << " = " << src.measure_nr ;
	test_filter << " AND ";
	test_filter << Fpl_Dike_Geo_Materialzone::matzone_table->get_column_name(fpl_label::section_id) << " = " << sec_id ;

	//submit it to the datbase
	Data_Base::database_request(&model, test_filter.str(), ptr_database);
	if(model.lastError().isValid()==true){
		Error msg;
		msg.set_msg("Fpl_Dike_Geo_Materialzone::copy_fpl_matzone(QSqlDatabase *ptr_database, const _sys_system_id src, const _sys_system_id dest, const int sec_id)","Invalid database request", "Check the database", 2, false);
		ostringstream info;
		info << "Table Name      : " << Fpl_Dike_Geo_Materialzone::matzone_table->get_table_name() << endl;
		info << "Table error info: " << model.lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
	test_filter.str("");

	QSqlQueryModel model1;
	for(int i=0; i< model.rowCount(); i++){
	
		test_filter.str("");
		test_filter << "INSERT INTO "<< Fpl_Dike_Geo_Materialzone::matzone_table->get_table_name();
		test_filter << " SELECT " << glob_id <<" , ";
		test_filter <<  Fpl_Dike_Geo_Materialzone::matzone_table->get_column_name(fpl_label::section_id)  <<" , ";
		test_filter <<  dest.area_state  <<" , ";
		test_filter <<  dest.measure_nr  <<" , ";
		test_filter <<  Fpl_Dike_Geo_Materialzone::matzone_table->get_column_name(label::applied_flag)  <<" , ";
		test_filter <<  Fpl_Dike_Geo_Materialzone::matzone_table->get_column_name(fpl_label::zone_id)  <<" , ";
		test_filter <<  Fpl_Dike_Geo_Materialzone::matzone_table->get_column_name(fpl_label::zone_type)  <<" , ";
		test_filter <<  Fpl_Dike_Geo_Materialzone::matzone_table->get_column_name(fpl_label::zone_name)  <<"  ";

		test_filter << " FROM " << Fpl_Dike_Geo_Materialzone::matzone_table->get_table_name() <<" ";
		test_filter << " WHERE " << Fpl_Dike_Geo_Materialzone::matzone_table->get_column_name(fpl_label::glob_id) << " = ";
		test_filter << model.record(i).value(Fpl_Dike_Geo_Materialzone::matzone_table->get_column_name(fpl_label::glob_id).c_str()).toInt();

		Data_Base::database_request(&model1, test_filter.str(), ptr_database);
		if(model1.lastError().isValid()==true){
			Error msg;
			msg.set_msg("Fpl_Dike_Geo_Materialzone::copy_fpl_matzone(QSqlDatabase *ptr_database, const _sys_system_id src, const _sys_system_id dest, const int sec_id)","Invalid database request", "Check the database", 2, false);
			ostringstream info;
			info << "Table Name      : " << Fpl_Dike_Geo_Materialzone::matzone_table->get_table_name() << endl;
			info << "Table error info: " << model1.lastError().text().toStdString() << endl;
			msg.make_second_info(info.str());
			throw msg;
		}
		glob_id++;
	}
	try{

		Fpl_Dike_Geo_Materialzone::switch_applied_flag_matzone_table(ptr_database, src, false,sec_id);
	}
	catch(Error msg){
		throw msg;

	}




	Fpl_Dike_Geo_Materialzone::copy_fpl_matzone_point(ptr_database, src, dest, sec_id);
}
//Select and count the number of relevant material zones in a database table (static)
int Fpl_Dike_Geo_Materialzone::select_relevant_matzone_database(QSqlQueryModel *results, QSqlDatabase *ptr_database, const _sys_system_id id, const int section_id, const bool with_output){
	int number=0;
	try{
		Fpl_Dike_Geo_Materialzone::set_matzone_table(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}

	if(with_output==true){
		ostringstream cout;
		cout << "Search for relevant material zones of the fpl-section in database..." << endl ;
		Sys_Common_Output::output_fpl->output_txt(&cout);

	}


	ostringstream test_filter;
	test_filter<< "SELECT * ";
	test_filter << " FROM " << Fpl_Dike_Geo_Materialzone::matzone_table->get_table_name();
	test_filter << " WHERE " ;
	test_filter << Fpl_Dike_Geo_Materialzone::matzone_table->get_column_name(label::applied_flag) << "= true";
	test_filter << " AND ";
	test_filter << Fpl_Dike_Geo_Materialzone::matzone_table->get_column_name(fpl_label::section_id) << " ="  <<section_id;
	test_filter << " AND ";
	test_filter << Fpl_Dike_Geo_Materialzone::matzone_table->get_column_name(label::areastate_id) << " ="  << id.area_state;
	test_filter << " AND (";
	test_filter << Fpl_Dike_Geo_Materialzone::matzone_table->get_column_name(label::measure_id) << " = " << 0 ;
	test_filter << " OR " ;
	test_filter << Fpl_Dike_Geo_Materialzone::matzone_table->get_column_name(label::measure_id) << " = " << id.measure_nr;
	test_filter << " ) " ;
	test_filter << " ORDER BY " <<  Fpl_Dike_Geo_Materialzone::matzone_table->get_column_name(fpl_label::zone_id);

	Data_Base::database_request(results, test_filter.str(), ptr_database);

	//check the request
	if(results->lastError().isValid()){
		Error msg;
		msg.set_msg("Fpl_Dike_Geo_Materialzone::select_relevant_matzone_database(QSqlQueryModel *results, QSqlDatabase *ptr_database, const _sys_system_id id, const int section_id)","Invalid database request", "Check the database", 2, false);
		ostringstream info;
		info << "Table Name      : " << Fpl_Dike_Geo_Materialzone::matzone_table->get_table_name() << endl;
		info << "Table error info: " << results->lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
	
	number= results->rowCount();

	if(with_output==true){
		ostringstream cout;
		cout << number << " relevant material zone(s) of the fpl-section are found in database" << endl ;
		Sys_Common_Output::output_fpl->output_txt(&cout);
	}

	return number;
}
//Create the database table for the material zone points (static)
void Fpl_Dike_Geo_Materialzone::create_matzone_point_table(QSqlDatabase *ptr_database){
	if(Fpl_Dike_Geo_Materialzone::matzone_point_table==NULL){
			ostringstream cout;
			cout << "Create material zone point database table..." << endl ;
			Sys_Common_Output::output_fpl->output_txt(&cout);
			//make specific input for this class
			const string tab_name=fpl_label::tab_fpl_material_zone_point;
			const int num_col=9;
			_Sys_data_tab_column tab_col[num_col];
			//init
			for(int i=0; i< num_col; i++){
				tab_col[i].key_flag=false;
				tab_col[i].unsigned_flag=false;
				tab_col[i].primary_key_flag=false;
			}


			tab_col[0].name=fpl_label::glob_id;
			tab_col[0].type=sys_label::tab_col_type_int;
			tab_col[0].unsigned_flag=true;
			tab_col[0].primary_key_flag=true;

			tab_col[1].name=fpl_label::section_id;
			tab_col[1].type=sys_label::tab_col_type_int;
			tab_col[1].unsigned_flag=true;
			tab_col[1].key_flag=true;

			tab_col[2].name=label::areastate_id;
			tab_col[2].type=sys_label::tab_col_type_int;
			tab_col[2].unsigned_flag=true;
			tab_col[2].key_flag=true;

			tab_col[3].name=label::measure_id;
			tab_col[3].type=sys_label::tab_col_type_int;
			tab_col[3].unsigned_flag=true;
			tab_col[3].key_flag=true;

			tab_col[4].name=label::applied_flag;
			tab_col[4].type=sys_label::tab_col_type_bool;
			tab_col[4].default_value="true";
			tab_col[4].key_flag=true;

			tab_col[5].name=fpl_label::zone_id;
			tab_col[5].type=sys_label::tab_col_type_int;
			tab_col[5].unsigned_flag=true;

			tab_col[6].name=fpl_label::point_id;
			tab_col[6].type=sys_label::tab_col_type_int;
			tab_col[6].unsigned_flag=true;

			tab_col[7].name=fpl_label::x_coor;
			tab_col[7].type=sys_label::tab_col_type_double;
			tab_col[7].default_value="0.0";

			tab_col[8].name=fpl_label::y_coor;
			tab_col[8].type=sys_label::tab_col_type_double;
			tab_col[8].default_value="0.0";



			try{
				Fpl_Dike_Geo_Materialzone::matzone_point_table= new Tables();
				if(Fpl_Dike_Geo_Materialzone::matzone_point_table->create_non_existing_tables(tab_name, tab_col, num_col,ptr_database, _sys_table_type::fpl)==false){
					cout << " Table exists" << endl ;
					Sys_Common_Output::output_fpl->output_txt(&cout);
				};
			}
			catch(bad_alloc& t){
				Error msg;
				msg.set_msg("Fpl_Dike_Geo_Materialzone::create_matzone_point_table(QSqlDatabase *ptr_database)","Can not allocate the memory", "Check the memory", 10, false);
				ostringstream info;
				info<< "Info bad alloc: " << t.what() << endl;
				msg.make_second_info(info.str());
				throw msg;	
			}
			catch(Error msg){
				Fpl_Dike_Geo_Materialzone::close_matzone_point_table();
				throw msg;
			}

		Fpl_Dike_Geo_Materialzone::close_matzone_point_table();
	}
}
//Delete all data in the database table for the material zone points (static)
void Fpl_Dike_Geo_Materialzone::delete_data_in_matzone_point_table(QSqlDatabase *ptr_database){
	//the table is set (the name and the column names) and allocated
	try{
		Fpl_Dike_Geo_Materialzone::set_matzone_point_table(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}
	//delete the table
	Fpl_Dike_Geo_Materialzone::matzone_point_table->delete_data_in_table(ptr_database);
}
//Set the database table for the material zone points: it sets the table name and the name of the columns and allocate them (static)
void Fpl_Dike_Geo_Materialzone::set_matzone_point_table(QSqlDatabase *ptr_database){
	if(Fpl_Dike_Geo_Materialzone::matzone_point_table==NULL){
		//make specific input for this class
		const string tab_id_name=fpl_label::tab_fpl_material_zone_point;//FPL:=FloodProtectionLine
		string tab_col[9];
		tab_col[0]=fpl_label::glob_id;
		tab_col[1]=fpl_label::section_id;
		tab_col[2]=fpl_label::zone_id;
		tab_col[3]=fpl_label::point_id;
		tab_col[4]=fpl_label::x_coor;
		tab_col[5]=fpl_label::y_coor;
		tab_col[6]=label::areastate_id;
		tab_col[7]=label::measure_id;
		tab_col[8]=label::applied_flag;


		//set the table
		try{
			matzone_point_table= new Tables(tab_id_name, tab_col, sizeof(tab_col)/sizeof(tab_col[0]));
			matzone_point_table->set_name(ptr_database, _sys_table_type::fpl);
		}
		catch(bad_alloc &t){
			Error msg;
			msg.set_msg("Fpl_Dike_Geo_Materialzone::set_matzone_point_table(QSqlDatabase *ptr_database)","Can not allocate the memory","Check the memory",10,false);
			ostringstream info;
			info<< "Info bad alloc: " << t.what() << endl;
			msg.make_second_info(info.str());
			throw msg;	
		}
		catch(Error msg){
			Fpl_Dike_Geo_Materialzone::close_matzone_point_table();
			throw msg;
		}
	}
}
//Close and delete the database table for the material zone points (static)
void Fpl_Dike_Geo_Materialzone::close_matzone_point_table(void){
	if(Fpl_Dike_Geo_Materialzone::matzone_point_table!=NULL){
		delete Fpl_Dike_Geo_Materialzone::matzone_point_table;
		Fpl_Dike_Geo_Materialzone::matzone_point_table=NULL;
	}
}
//Select and count the number of relevant material zone points in a database table
int Fpl_Dike_Geo_Materialzone::select_relevant_matzone_points_database(QSqlQueryModel *results, QSqlDatabase *ptr_database, const _sys_system_id id, const int section_id, const int zone_id){
	int number=0;
	try{
		Fpl_Dike_Geo_Materialzone::set_matzone_point_table(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}

	
	ostringstream test_filter;
	test_filter<< "SELECT * ";
	test_filter << " FROM " << Fpl_Dike_Geo_Materialzone::matzone_point_table->get_table_name();
	test_filter << " WHERE " ;
	test_filter << Fpl_Dike_Geo_Materialzone::matzone_point_table->get_column_name(label::applied_flag) << "= true";
	test_filter << " AND ";
	test_filter << Fpl_Dike_Geo_Materialzone::matzone_point_table->get_column_name(fpl_label::section_id) << "= "<< section_id;
	test_filter << " AND ";
	test_filter << Fpl_Dike_Geo_Materialzone::matzone_point_table->get_column_name(fpl_label::zone_id) << " ="  << zone_id;
	test_filter << " AND ";
	test_filter << Fpl_Dike_Geo_Materialzone::matzone_point_table->get_column_name(label::areastate_id) << " ="  << id.area_state;
	test_filter << " AND (";
	test_filter << Fpl_Dike_Geo_Materialzone::matzone_point_table->get_column_name(label::measure_id) << " = " << 0 ;
	test_filter << " OR " ;
	test_filter << Fpl_Dike_Geo_Materialzone::matzone_point_table->get_column_name(label::measure_id) << " = " << id.measure_nr;
	test_filter << " ) " ;
	test_filter << " ORDER BY " <<  Fpl_Dike_Geo_Materialzone::matzone_point_table->get_column_name(fpl_label::point_id);

	Data_Base::database_request(results, test_filter.str(), ptr_database);

	//check the request
	if(results->lastError().isValid()){
		Error msg;
		msg.set_msg("Fpl_Dike_Geo_Materialzone::select_relevant_matzone_points_database(QSqlQueryModel *results, QSqlDatabase *ptr_database, const int section_id, const int zone_id)","Invalid database request", "Check the database", 2, false);
		ostringstream info;
		info << "Table Name      : " << Fpl_Dike_Geo_Materialzone::matzone_point_table->get_table_name() << endl;
		info << "Table error info: " << results->lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
	
	number= results->rowCount();


	return number;

}
//Get the zone name
string Fpl_Dike_Geo_Materialzone::get_zone_name(void){
	return this->zone_name;
}
//Get the zone number
int Fpl_Dike_Geo_Materialzone::get_zone_number(void){
	return this->zone_number;
}
//Set the zone number
void Fpl_Dike_Geo_Materialzone::set_zone_number(const int number){
	this->zone_number=number;
}
//Get the material variable zone number
int Fpl_Dike_Geo_Materialzone::get_variable_zone_number(void){
	return this->variable_zone_number;
}
//Set the pointer to the relevant variables
void Fpl_Dike_Geo_Materialzone::set_ptr_relevant_variables(Fpl_Dike_Var_Materialzone *variables){
	this->variables=variables;
}
//Get the pointer to the relevant variables
Fpl_Dike_Var_Materialzone* Fpl_Dike_Geo_Materialzone::get_ptr_relevant_variables(void){
	return this->variables;
}
//Output the material zone to display/console
void Fpl_Dike_Geo_Materialzone::output_member(void){
	ostringstream cout;
	cout<<"MATERIAL ZONE"<< endl;
	cout <<" Zone name                     : " << this->zone_name << endl;
	cout <<" Zone id                       : " << this->zone_number  <<endl;
	cout <<" Zone area                     : " << this->calculate_area() << label::squaremeter<< endl;
	cout <<" Zone perimeter                : " << this->calculate_perimeter() << label::m<< endl;
	cout <<" Number of segments            : " << this->number_segments  << endl;
	Sys_Common_Output::output_fpl->output_txt(&cout,true);

	cout<<" ZONE SEGMENTS"<< endl;
	Sys_Common_Output::output_fpl->output_txt(&cout,true);
	//output the segments
	for(int i=0;i<this->number_segments;i++){
		this->segments[i].output_members(&cout);
		Sys_Common_Output::output_fpl->output_txt(&cout,true);
	}

}
//Output zone to tecplot
void Fpl_Dike_Geo_Materialzone::output2tecplot(ofstream *output){

	*output << endl << "ZONE T= "<< "\" Materialzone "<<this->zone_name<<"; Var_Zone_id "<<this->variable_zone_number<<""<< "\" "<< " I = " << this->number_segments+1 << endl;
	for(int i=0; i< this->number_segments; i++){
		*output << W(12) << this->segments[i].point1.get_xcoordinate() << W(12) << this->segments[i].point1.get_ycoordinate() <<  endl;
	}
	if(this->number_segments>0){
		*output << W(12) << this->segments[this->number_segments-1].point2.get_xcoordinate() << W(12) << this->segments[this->number_segments-1].point2.get_ycoordinate() <<  endl;
	}

	output->flush();
}
//Get the zone type
_fpl_zone_type Fpl_Dike_Geo_Materialzone::get_zone_type(void){
 return this->zone_type;
}
//Convert a string into the zone type (_fpl_zone_type) (static)
_fpl_zone_type Fpl_Dike_Geo_Materialzone::convert_txt2zone_type(const string txt){
	_fpl_zone_type type;
	string buffer=txt;

	functions::clean_string(&buffer);
	functions::convert_string2lower_case(&buffer);

	if(buffer==fpl_label::zone_impermeable_layer){
		type=_fpl_zone_type::imperm_layer;
	}
	else if(buffer==fpl_label::zone_permeable_layer){
		type=_fpl_zone_type::perm_layer;
	}
	else if(buffer==label::not_set){
		type=_fpl_zone_type::not_set_zone;
	}
	else{
		Error msg;
		msg.set_msg("Fpl_Dike_Geo_Materialzone::convert_txt2zone_type(const string txt)", "Can not convert this zone type", "Check the given type", 1, false);
		ostringstream info;
		info << "Zone type: " << txt << endl;
		info << "Possible types: "<< endl;
		info << " "<<fpl_label::zone_impermeable_layer  << endl;
		info << " "<<fpl_label::zone_permeable_layer  << endl;
		info << " "<<label::not_set  << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
	return type;
}
//Convert a zone type (_fpl_zone_type) into a string (static)
string Fpl_Dike_Geo_Materialzone::convert_zone_type2txt(const _fpl_zone_type type){
	string buffer;
	switch(type){
		case _fpl_zone_type::imperm_layer:
			buffer=fpl_label::zone_impermeable_layer;
			break;
		case _fpl_zone_type::perm_layer:
			buffer=fpl_label::zone_permeable_layer;
			break;
		default:
			buffer=label::not_set;
	}
	return buffer;
}
//Get the minimal x-coordiante 
double Fpl_Dike_Geo_Materialzone::get_min_x_coordinate(void){

	double buffer=0.0;
	for(int i=0; i<this->number_segments; i++){
		if(i==0){
			buffer=this->segments[i].point1.get_xcoordinate();
		}
		else{
			if(buffer>this->segments[i].point1.get_xcoordinate()){
				buffer=this->segments[i].point1.get_xcoordinate();
			}
		}
	}

	return buffer;

}
//________
//private
//Delete the data in the database table for the material zone points of a specific fpl-sections (static)
void Fpl_Dike_Geo_Materialzone::delete_data_in_matzone_point_table(QSqlDatabase *ptr_database,  const _sys_system_id id, const int sec_id){

	QSqlQueryModel results;
	ostringstream test_filter;
	try{
		Fpl_Dike_Geo_Materialzone::set_matzone_point_table(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}

	//delete the points
	//delete 
	test_filter<< "Delete ";
	test_filter << " from " << Fpl_Dike_Geo_Materialzone::matzone_point_table->get_table_name();
	test_filter << " where ";
	test_filter << Fpl_Dike_Geo_Materialzone::matzone_point_table->get_column_name(fpl_label::section_id) << " = " << sec_id << " and ";
	test_filter << Fpl_Dike_Geo_Materialzone::matzone_point_table->get_column_name(label::areastate_id) << " = " << id.measure_nr << " and ";
	test_filter << Fpl_Dike_Geo_Materialzone::matzone_point_table->get_column_name(label::measure_id) << " = " << id.area_state;

	Data_Base::database_request(&results, test_filter.str(), ptr_database);

	//check the request
	if(results.lastError().isValid()){
		Error msg;
		msg.set_msg("Fpl_Dike_Geo_Materialzone::delete_data_in_matzone_point_table(QSqlDatabase *ptr_database,  const int sec_id)","Invalid database request", "Check the database", 2, false);
		ostringstream info;
		info << "Table Name      : " << Fpl_Dike_Geo_Materialzone::matzone_point_table->get_table_name() << endl;
		info << "Table error info: " << results.lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}



}
//Switch the applied-flag material zone points in the database table for a defined system state (static)
void Fpl_Dike_Geo_Materialzone::switch_applied_flag_matzone_point_table(QSqlDatabase *ptr_database, const _sys_system_id id, const bool flag){
//the table is set (the name and the column names) and allocated
	try{
		Fpl_Dike_Geo_Materialzone::set_matzone_point_table(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}
	QSqlQueryModel query;

	ostringstream query_string;
	query_string <<"UPDATE ";
	query_string << Fpl_Dike_Geo_Materialzone::matzone_point_table->get_table_name();
	query_string << " SET " ;
	query_string  << Fpl_Dike_Geo_Materialzone::matzone_point_table->get_column_name(label::applied_flag) << " = "<< functions::convert_boolean2string(flag);
	query_string  << " WHERE ";
	query_string  << Fpl_Dike_Geo_Materialzone::matzone_point_table->get_column_name(label::areastate_id) << " = "  << id.area_state;
	query_string  << " AND ";
	query_string  <<  Fpl_Dike_Geo_Materialzone::matzone_point_table->get_column_name(label::measure_id) << " = " << id.measure_nr ;
	


	Data_Base::database_request(&query, query_string.str(), ptr_database);
	if(query.lastError().isValid()==true){
		Error msg;
		msg.set_msg("Fpl_Dike_Geo_Materialzone::switch_applied_flag_matzone_point_table(QSqlDatabase *ptr_database, const _sys_system_id id, const bool flag)","Invalid database request", "Check the database", 2, false);
		ostringstream info;
		info << "Table Name      : " << Fpl_Dike_Geo_Materialzone::matzone_point_table->get_table_name() << endl;
		info << "Table error info: " << query.lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
}
//Switch the applied-flag material zone points in the database table for a defined system state and section id (static)
void Fpl_Dike_Geo_Materialzone::switch_applied_flag_matzone_point_table(QSqlDatabase *ptr_database, const _sys_system_id id, const bool flag, const int sec_id){
//the table is set (the name and the column names) and allocated
	try{
		Fpl_Dike_Geo_Materialzone::set_matzone_point_table(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}
	QSqlQueryModel query;

	ostringstream query_string;
	query_string <<"UPDATE ";
	query_string << Fpl_Dike_Geo_Materialzone::matzone_point_table->get_table_name();
	query_string << " SET " ;
	query_string  << Fpl_Dike_Geo_Materialzone::matzone_point_table->get_column_name(label::applied_flag) << " = "<< functions::convert_boolean2string(flag);
	query_string  << " WHERE ";
	query_string  << Fpl_Dike_Geo_Materialzone::matzone_point_table->get_column_name(label::areastate_id) << " = "  << id.area_state;
	query_string  << " AND ";
	query_string  <<  Fpl_Dike_Geo_Materialzone::matzone_point_table->get_column_name(label::measure_id) << " = " << id.measure_nr ;
	query_string  << " AND ";
	query_string  <<  Fpl_Dike_Geo_Materialzone::matzone_point_table->get_column_name(fpl_label::section_id) << " = " << sec_id ;
	


	Data_Base::database_request(&query, query_string.str(), ptr_database);
	if(query.lastError().isValid()==true){
		Error msg;
		msg.set_msg("Fpl_Dike_Geo_Materialzone::switch_applied_flag_matzone_point_table(QSqlDatabase *ptr_database, const _sys_system_id id, const bool flag, const int sec_id)","Invalid database request", "Check the database", 2, false);
		ostringstream info;
		info << "Table Name      : " << Fpl_Dike_Geo_Materialzone::matzone_point_table->get_table_name() << endl;
		info << "Table error info: " << query.lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
}
//Copy the material zone points of a fpl-section of a given system id to another one in database tables (static)
void Fpl_Dike_Geo_Materialzone::copy_fpl_matzone_point(QSqlDatabase *ptr_database, const _sys_system_id src, const _sys_system_id dest, const int sec_id){
	try{
		Fpl_Dike_Geo_Materialzone::set_matzone_point_table(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}

	int glob_id=0;
	glob_id=Fpl_Dike_Geo_Materialzone::matzone_point_table->maximum_int_of_column(Fpl_Dike_Geo_Materialzone::matzone_point_table->get_column_name(fpl_label::glob_id) ,ptr_database)+1;

	QSqlQueryModel model;
	ostringstream test_filter;
	test_filter<< "SELECT ";
	test_filter << Fpl_Dike_Geo_Materialzone::matzone_point_table->get_column_name(fpl_label::glob_id);
	test_filter << " from " << Fpl_Dike_Geo_Materialzone::matzone_point_table->get_table_name();
	test_filter << " WHERE ";
	test_filter << Fpl_Dike_Geo_Materialzone::matzone_point_table->get_column_name(label::areastate_id) << " = " << src.area_state;
	test_filter << " AND ";
	test_filter << Fpl_Dike_Geo_Materialzone::matzone_point_table->get_column_name(label::measure_id) << " = " << src.measure_nr ;
	test_filter << " AND ";
	test_filter << Fpl_Dike_Geo_Materialzone::matzone_point_table->get_column_name(fpl_label::section_id) << " = " << sec_id ;

	//submit it to the datbase
	Data_Base::database_request(&model, test_filter.str(), ptr_database);
	if(model.lastError().isValid()==true){
		Error msg;
		msg.set_msg("Fpl_Dike_Geo_Materialzone::copy_fpl_matzone_point(QSqlDatabase *ptr_database, const _sys_system_id src, const _sys_system_id dest, const int sec_id)","Invalid database request", "Check the database", 2, false);
		ostringstream info;
		info << "Table Name      : " << Fpl_Dike_Geo_Materialzone::matzone_point_table->get_table_name() << endl;
		info << "Table error info: " << model.lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
	test_filter.str("");

	QSqlQueryModel model1;
	for(int i=0; i< model.rowCount(); i++){
	
		test_filter.str("");
		test_filter << "INSERT INTO "<< Fpl_Dike_Geo_Materialzone::matzone_point_table->get_table_name();
		test_filter << " SELECT " << glob_id <<" , ";
		test_filter <<  Fpl_Dike_Geo_Materialzone::matzone_point_table->get_column_name(fpl_label::section_id)  <<" , ";
		test_filter <<  dest.area_state  <<" , ";
		test_filter <<  dest.measure_nr  <<" , ";
		test_filter <<  Fpl_Dike_Geo_Materialzone::matzone_point_table->get_column_name(label::applied_flag)  <<" , ";
		test_filter <<  Fpl_Dike_Geo_Materialzone::matzone_point_table->get_column_name(fpl_label::zone_id)  <<" , ";
		test_filter <<  Fpl_Dike_Geo_Materialzone::matzone_point_table->get_column_name(fpl_label::zone_name)  <<"  ";

		test_filter << " FROM " << Fpl_Dike_Geo_Materialzone::matzone_point_table->get_table_name() <<" ";
		test_filter << " WHERE " << Fpl_Dike_Geo_Materialzone::matzone_point_table->get_column_name(fpl_label::glob_id) << " = ";
		test_filter << model.record(i).value(Fpl_Dike_Geo_Materialzone::matzone_point_table->get_column_name(fpl_label::glob_id).c_str()).toInt();

		Data_Base::database_request(&model1, test_filter.str(), ptr_database);
		if(model1.lastError().isValid()==true){
			Error msg;
			msg.set_msg("Fpl_Dike_Geo_Materialzone::copy_fpl_matzone_point(QSqlDatabase *ptr_database, const _sys_system_id src, const _sys_system_id dest, const int sec_id)","Invalid database request", "Check the database", 2, false);
			ostringstream info;
			info << "Table Name      : " << Fpl_Dike_Geo_Materialzone::matzone_point_table->get_table_name() << endl;
			info << "Table error info: " << model1.lastError().text().toStdString() << endl;
			msg.make_second_info(info.str());
			throw msg;
		}
		glob_id++;
	}
	try{

		Fpl_Dike_Geo_Materialzone::switch_applied_flag_matzone_point_table(ptr_database, src, false,sec_id);
	}
	catch(Error msg){
		throw msg;

	}
}
//Transfer the point data to database table
void Fpl_Dike_Geo_Materialzone::transfer_point_data2database(QSqlDatabase *ptr_database, const int section_id){

	try{
		Fpl_Dike_Geo_Materialzone::set_matzone_point_table(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}

	int glob_id;
	ostringstream query_string;
	//Set the query
	QSqlQuery query_buff(*ptr_database);

	//transfer the point data
	for(int i=0; i< this->number_segments; i++){
		glob_id=Fpl_Dike_Geo_Materialzone::matzone_point_table->maximum_int_of_column(Fpl_Dike_Geo_Materialzone::matzone_point_table->get_column_name(fpl_label::glob_id), ptr_database)+1;
		query_string.str("");

		//transfer the data
		query_string << "INSERT INTO  " << Fpl_Dike_Geo_Materialzone::matzone_point_table->get_table_name();
		query_string <<" ( ";
		query_string << Fpl_Dike_Geo_Materialzone::matzone_point_table->get_column_name(fpl_label::glob_id) <<" , ";
		query_string << Fpl_Dike_Geo_Materialzone::matzone_point_table->get_column_name(fpl_label::section_id) <<" , ";

		query_string << Fpl_Dike_Geo_Materialzone::matzone_point_table->get_column_name(label::areastate_id) <<" , ";
		query_string << Fpl_Dike_Geo_Materialzone::matzone_point_table->get_column_name(label::measure_id) <<" , ";
		query_string << Fpl_Dike_Geo_Materialzone::matzone_point_table->get_column_name(label::applied_flag) <<" , ";

		query_string << Fpl_Dike_Geo_Materialzone::matzone_point_table->get_column_name(fpl_label::zone_id) <<" , ";
		query_string << Fpl_Dike_Geo_Materialzone::matzone_point_table->get_column_name(fpl_label::point_id) <<" , ";

		query_string << Fpl_Dike_Geo_Materialzone::matzone_point_table->get_column_name(fpl_label::x_coor) <<" , ";
		query_string << Fpl_Dike_Geo_Materialzone::matzone_point_table->get_column_name(fpl_label::y_coor);
		query_string <<" ) ";
		query_string << " VALUES ( ";

		query_string << glob_id << " , " ;
		query_string << section_id<< " , " ;

		query_string << this->system_id.area_state << " , " ;
		query_string << this->system_id.measure_nr<< " , " ;
		query_string << "true" << " , " ;

		query_string << this->zone_number << " , " ;
		query_string << i<< " , " ;

		query_string << this->segments[i].point1.get_xcoordinate() << " , " ;
		query_string << this->segments[i].point1.get_ycoordinate() << " ) " ;


		//launch the request
		Data_Base::database_request(&query_buff, query_string.str(), ptr_database);

		if(query_buff.lastError().isValid()){
			Warning msg=this->set_warning(1);
			ostringstream info;
			info << "Table Name                : " << Fpl_Dike_Geo_Materialzone::matzone_point_table->get_table_name() << endl;
			info << "Table error info          : " << query_buff.lastError().text().toStdString() << endl;
			info << "Point is                  : " << i << endl;
			info << "Zone name                 : " << this->zone_name << endl;
			info << "Zone id                   : " << this->zone_number << endl;
			msg.make_second_info(info.str());
			msg.output_msg(1);	
		}
	}
}
//Find the start of an input block of the fpl-section in file
qint64 Fpl_Dike_Geo_Materialzone::find_start_block_file(QFile *ifile, int *line_counter, const string begin, const string end, const bool must_found){
	//read in
	string myline;
	QString qmyline;
	stringstream my_stream;
	bool found_flag=false;
	qint64 pos_file=0;

	//search for the begin
	do{
		qmyline=ifile->readLine();
		myline=qmyline.toStdString();
		(*line_counter)++;
		functions::clean_string(&myline);

		if(myline==begin){
			found_flag=true;
			pos_file=ifile->pos();
			break;
		}
		if(myline==end){
			break;
		}
	}
	while(ifile->atEnd()==false);


	if(found_flag==false && must_found==true){
		Error msg=this->set_error(2);
		ostringstream info;
		info << "Start keyword : " << begin << endl;
		info << "End keyword   : " << end<<endl;
		msg.make_second_info(info.str());
 		throw msg;

	}
	else{
		found_flag=false;
	}

	return pos_file;

}
//Find the key-values for the file input of the material zone data
void Fpl_Dike_Geo_Materialzone::find_key_values_file(const string myline, int *must_found_counter){
	int pos=-1;
	string buffer;
	stringstream buffer1;
	bool wrong_input=false;

	//number points
	pos=myline.find(fpl_label::key_no_points);
	if(pos>=0 && wrong_input==false){
		buffer=myline.substr(fpl_label::key_no_points.length());
		functions::clean_string(&buffer);
		int number_points=0;
		buffer1 << buffer;
		buffer1 >> number_points;
		this->set_number_points(number_points);
		if(buffer1.fail()==true){
			wrong_input=true;
		}
		else{
			(*must_found_counter)++;
			return;
		}
	}

	//zone id
	pos=myline.find(fpl_label::key_zone_id);
	if(pos>=0 && wrong_input==false){
		buffer=myline.substr(fpl_label::key_zone_id.length());
		functions::clean_string(&buffer);
		buffer1 << buffer;
		buffer1 >> this->variable_zone_number;
		if(buffer1.fail()==true){
			wrong_input=true;
		}
		else{
			(*must_found_counter)++;
			return;
		}
	}

	//zone name
	pos=myline.find(fpl_label::key_name_zone);
	if(pos>=0 && wrong_input==false){
		buffer=myline.substr(fpl_label::key_name_zone.length());
		functions::clean_string(&buffer);
		buffer1 << buffer;
		buffer1 >> this->zone_name;
		if(buffer1.fail()==true){
			wrong_input=true;
		}
		else{
			(*must_found_counter)++;
			return;
		}
	}

	//zone type
	pos=myline.find(fpl_label::key_type_zone);
	if(pos>=0 && wrong_input==false){
		buffer=myline.substr(fpl_label::key_type_zone.length());
		functions::clean_string(&buffer);
		buffer1 << buffer;
		this->zone_type=Fpl_Dike_Geo_Materialzone::convert_txt2zone_type(buffer1.str()); 
		if(buffer1.fail()==true){
			wrong_input=true;
		}
		else{
			return;
		}
	}

	if(wrong_input==true){
		ostringstream info;
		info << "Wrong input sequenze  : " << buffer1.str() << endl;
		Error msg=this->set_error(3);
		msg.make_second_info(info.str());
		throw msg;
	}
}
//Set warning(s)
Warning Fpl_Dike_Geo_Materialzone::set_warning(const int warn_type){
		string place="Fpl_Dike_Geo_Materialzone::";
		string help;
		string reason;
		string reaction;
		int type=0;
		Warning msg;
		stringstream info;

	switch (warn_type){
		case 0://input datas can not submitted
			place.append("transfer_matzone2database(QSqlDatabase *ptr_database, const int section_id)");
			reason="Can not submit the material zone to database table";
			help="Check the database";
			type=2;
			break;
		case 1://input datas can not submitted
			place.append("transfer_point_data2database(QSqlDatabase *ptr_database, const int section_id)");
			reason="Can not submit the material zone point to database table";
			help="Check the database";
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
//set the error
Error Fpl_Dike_Geo_Materialzone::set_error(const int err_type){
		string place="Fpl_Dike_Geo_Materialzone::";
		string help;
		string reason;
		int type=0;
		bool fatal=false;
		stringstream info;
		Error msg;

	switch (err_type){
		case 0://bad_alloc
			place.append("set_input(QSqlQueryModel *results, const int index, const int section_id,  QSqlDatabase *ptr_database)");
			reason="Can not allocate the memory";
			help="Check the memory";
			type=10;
			break;
		case 1://no points found
			place.append("set_input(QSqlQueryModel *results, const int index, const int section_id,  QSqlDatabase *ptr_database)");
			reason="No geometrical points are found to the material zone";
			help="Check the points of the material zone in database table";
			type=2;
			break;
		case 2://do not find the begin of the material zone information in file
			place.append("find_start_block_file(QFile *ifile, int *line_counter, const string begin, const string end, const bool must_found)");
			reason="Can not find the begin of the data block in file";
			help="The material zone block data has to start with the keys !$BEGIN_...";
			type=1;
			break;
		case 3://wrong input
			place.append("find_key_values_file(const string myline, int *must_found_counter)");
			reason="There is a problem with the input; wrong sign is read in";
			help="Check the information of the material zones in file";
			type=1;
			break;
		case 4://do not find all required section infos
			place.append("read_matzone_per_file(QFile *ifile, int *line_counter)");
			reason="Can not find all required material zone information";
			help="Check if all material zone information are set in the file";
			info << "Required information:"<<endl;
			info << " " << fpl_label::key_no_points<< endl;
			info << " " << fpl_label::key_zone_id<< endl;
			info << " " << fpl_label::key_name_zone<< endl;
			type=1;
			break;
		case 5://bad_alloc
			place.append("read_matzone_per_file(QFile *ifile, int *line_counter)");
			reason="Can not allocate the memory";
			help="Check the memory";
			type=10;
			break;
		case 6://wrong input
			place.append("read_matzone_per_file(QFile *ifile, int *line_counter)");
			reason="There is a problem with the input; wrong sign is read in";
			help="Check the information of the material zones in file";
			type=1;
			break;
		case 7://wrong input
			place.append("read_matzone_per_file(QFile *ifile, int *line_counter)");
			reason="Not the required number of points are found in file";
			help="Check the point information of the material zones in file";
			type=1;
			break;
		case 8://do not find the end of the material zone information in file
			place.append("read_matzone_per_file(QFile *ifile, int *line_counter)");
			reason="Can not find the begin of the data block in file";
			help="The material zone block data has to start with the keys !$END_...";
			type=1;
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
