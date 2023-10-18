#include "Fpl_Headers_Precompiled.h"
//#include "Fpl_Section_Points.h"

//init static variables
Tables *Fpl_Section_Points::point_table=NULL;

//Default constructor
Fpl_Section_Points::Fpl_Section_Points(void){

	this->zone_number=0;

	//count the memory
	Sys_Memory_Count::self()->add_mem(sizeof(Fpl_Section_Points)-sizeof(Geo_Point), _sys_system_modules::FPL_SYS);
}
//Copy constructor
Fpl_Section_Points::Fpl_Section_Points(const Fpl_Section_Points& object){
	Geo_Point::operator =(object);

	this->zone_number=object.zone_number;
	//count the memory
	Sys_Memory_Count::self()->add_mem(sizeof(Fpl_Section_Points)-sizeof(Geo_Point), _sys_system_modules::FPL_SYS);
}
//Constructor with a given point name 
Fpl_Section_Points::Fpl_Section_Points(const string name){

	this->zone_number=-1;
	this->name=name;

	//count the memory
	Sys_Memory_Count::self()->add_mem(sizeof(Fpl_Section_Points)-sizeof(Geo_Point), _sys_system_modules::FPL_SYS);

}
//Default destructor
Fpl_Section_Points::~Fpl_Section_Points(void){
	//count the memory
	Sys_Memory_Count::self()->minus_mem(sizeof(Fpl_Section_Points)-sizeof(Geo_Point), _sys_system_modules::FPL_SYS);
}
//__________
//public

///Get the zonenumber
int Fpl_Section_Points::get_zone_number(void){
	return this->zone_number;
}
//Set the zone number
void Fpl_Section_Points::set_zone_number(const int zone){
	this->zone_number=zone;
}
//Set the database table for the geometric points of the fpl-sections: it sets the table name and the name of the columns and allocate them (static)
void Fpl_Section_Points::set_point_table(QSqlDatabase *ptr_database){
	if(Fpl_Section_Points::point_table==NULL){
		//make specific input for this class
		const string tab_id_name=fpl_label::tab_fpl_geo_point;
		string tab_col[11];
		tab_col[0]=fpl_label::glob_id;
		tab_col[1]=fpl_label::section_id;
		tab_col[2]=label::areastate_id;
		tab_col[3]=label::measure_id;
		tab_col[4]=label::applied_flag;
		tab_col[5]=fpl_label::point_name;
		tab_col[6]=fpl_label::point_region;
		tab_col[7]=fpl_label::zone_id;
		tab_col[8]=fpl_label::point_x_coor;
		tab_col[9]=fpl_label::point_y_coor;
		tab_col[10]=fpl_label::section_type;

		//set table
		try{
			Fpl_Section_Points::point_table= new Tables(tab_id_name, tab_col, sizeof(tab_col)/sizeof(tab_col[0]));
			Fpl_Section_Points::point_table->set_name(ptr_database, _sys_table_type::fpl);
		}
		catch(bad_alloc&t ){
			Error msg;
			msg.set_msg("void Fpl_Section_Points::set_point_table(QSqlDatabase *ptr_database)","Can not allocate the memory", "Check the memory", 10, false);
			ostringstream info;
			info<< "Info bad alloc: " << t.what() << endl;
			msg.make_second_info(info.str());
			throw msg;	
		}
		catch(Error msg){
			Fpl_Section_Points::close_point_table();
			throw msg;
		}
	}
}
//Create the database table for the geometric points of the fpl-sections (static)
void Fpl_Section_Points::create_point_table(QSqlDatabase *ptr_database){
	if(Fpl_Section_Points::point_table==NULL){
			ostringstream cout;
			cout << "Create geometric point fpl-sections database table..." << endl ;
			Sys_Common_Output::output_fpl->output_txt(&cout);
			//make specific input for this class
			const string tab_name=fpl_label::tab_fpl_geo_point;
			const int num_col=11;
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

			tab_col[5].name=fpl_label::point_name;
			tab_col[5].type=sys_label::tab_col_type_string;
			tab_col[5].key_flag=true;

			tab_col[6].name=fpl_label::point_region;
			tab_col[6].type=sys_label::tab_col_type_string;
			tab_col[6].key_flag=true;

			tab_col[7].name=fpl_label::section_type;
			tab_col[7].type=sys_label::tab_col_type_string;
			tab_col[7].key_flag=true;

			tab_col[8].name=fpl_label::zone_id;
			tab_col[8].type=sys_label::tab_col_type_int;
			tab_col[8].unsigned_flag=true;

			tab_col[9].name=fpl_label::point_x_coor;
			tab_col[9].type=sys_label::tab_col_type_double;
	
			tab_col[10].name=fpl_label::point_y_coor;
			tab_col[10].type=sys_label::tab_col_type_double;



			try{
				Fpl_Section_Points::point_table= new Tables();
				if(Fpl_Section_Points::point_table->create_non_existing_tables(tab_name, tab_col, num_col,ptr_database, _sys_table_type::fpl)==false){
					cout << " Table exists" << endl ;
					Sys_Common_Output::output_fpl->output_txt(&cout);
				};
			}
			catch(bad_alloc& t){
				Error msg;
				msg.set_msg("Fpl_Section_Points::create_point_table(QSqlDatabase *ptr_database)","Can not allocate the memory", "Check the memory", 10, false);
				ostringstream info;
				info<< "Info bad alloc: " << t.what() << endl;
				msg.make_second_info(info.str());
				throw msg;	
			}
			catch(Error msg){
				Fpl_Section_Points::close_point_table();
				throw msg;
			}

		Fpl_Section_Points::close_point_table();
	}
}
//Delete all data in the database table for geometric points of the fpl-sections (static)
void Fpl_Section_Points::delete_data_in_point_table(QSqlDatabase *ptr_database){
	//the table is set (the name and the column names) and allocated
	try{
		Fpl_Section_Points::set_point_table(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}
	//delete data in table
	Fpl_Section_Points::point_table->delete_data_in_table(ptr_database);
}
//Delete the data in the database table for geometric points of a specific fpl-sections
void Fpl_Section_Points::delete_data_in_point_table(QSqlDatabase *ptr_database,  const _sys_system_id id, const int sec_id){

	QSqlQueryModel results;
	ostringstream test_filter;
	try{
		Fpl_Section_Points::set_point_table(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}

	//delete the points
	//delete 
	test_filter<< "Delete ";
	test_filter << " from " << Fpl_Section_Points::point_table->get_table_name();
	test_filter << " where ";
	test_filter << Fpl_Section_Points::point_table->get_column_name(fpl_label::section_id) << " = " << sec_id << " and ";
	test_filter << Fpl_Section_Points::point_table->get_column_name(label::areastate_id) << " = " << id.measure_nr << " and ";
	test_filter << Fpl_Section_Points::point_table->get_column_name(label::measure_id) << " = " << id.area_state;
	Data_Base::database_request(&results, test_filter.str(), ptr_database);

	//check the request
	if(results.lastError().isValid()){
		Error msg;
		msg.set_msg("Fpl_Section_Points::delete_data_in_point_table(QSqlDatabase *ptr_database,  const _sys_system_id id, const int sec_id)","Invalid database request", "Check the database", 2, false);
		ostringstream info;
		info << "Table Name      : " << Fpl_Section_Points::point_table->get_table_name() << endl;
		info << "Table error info: " << results.lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}

}
//Close and delete the database table for geometric points of the fpl-sections (static)
void Fpl_Section_Points::close_point_table(void){
	if(Fpl_Section_Points::point_table!=NULL){
		delete Fpl_Section_Points::point_table;
		Fpl_Section_Points::point_table=NULL;
	}
}
//Switch the applied-flag for the results of the geometrical points in the database table for a defined system state
void Fpl_Section_Points::switch_applied_flag_point_table(QSqlDatabase *ptr_database, const _sys_system_id id, const bool flag){
	//the table is set (the name and the column names) and allocated
	try{
		Fpl_Section_Points::set_point_table(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}
	QSqlQueryModel query;

	ostringstream query_string;
	query_string <<"UPDATE ";
	query_string << Fpl_Section_Points::point_table->get_table_name();
	query_string << " SET " ;
	query_string  << Fpl_Section_Points::point_table->get_column_name(label::applied_flag) << " = "<< functions::convert_boolean2string(flag);
	query_string  << " WHERE ";
	query_string  << Fpl_Section_Points::point_table->get_column_name(label::areastate_id) << " = "  << id.area_state;
	query_string  << " AND ";
	query_string  <<  Fpl_Section_Points::point_table->get_column_name(label::measure_id) << " = " << id.measure_nr ;
	


	Data_Base::database_request(&query, query_string.str(), ptr_database);
	if(query.lastError().isValid()==true){
		Error msg;
		msg.set_msg("Fpl_Section_Points::switch_applied_flag_point_table(QSqlDatabase *ptr_database, const _sys_system_id id, const bool flag)","Invalid database request", "Check the database", 2, false);
		ostringstream info;
		info << "Table Name      : " << Fpl_Section_Points::point_table->get_table_name() << endl;
		info << "Table error info: " << query.lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
}
//Switch the applied-flag for the results of the geometrical points in the database table for a defined system state and id
void Fpl_Section_Points::switch_applied_flag_point_table(QSqlDatabase *ptr_database, const _sys_system_id id, const bool flag, const int sec_id){
	//the table is set (the name and the column names) and allocated
	try{
		Fpl_Section_Points::set_point_table(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}
	QSqlQueryModel query;

	ostringstream query_string;
	query_string <<"UPDATE ";
	query_string << Fpl_Section_Points::point_table->get_table_name();
	query_string << " SET " ;
	query_string  << Fpl_Section_Points::point_table->get_column_name(label::applied_flag) << " = "<< functions::convert_boolean2string(flag);
	query_string  << " WHERE ";
	query_string  << Fpl_Section_Points::point_table->get_column_name(label::areastate_id) << " = "  << id.area_state;
	query_string  << " AND ";
	query_string  <<  Fpl_Section_Points::point_table->get_column_name(label::measure_id) << " = " << id.measure_nr ;
	query_string  << " AND ";
	query_string  <<  Fpl_Section_Points::point_table->get_column_name(fpl_label::section_id) << " = " << sec_id ;
	


	Data_Base::database_request(&query, query_string.str(), ptr_database);
	if(query.lastError().isValid()==true){
		Error msg;
		msg.set_msg("Fpl_Section_Points::switch_applied_flag_point_table(QSqlDatabase *ptr_database, const _sys_system_id id, const bool flag, const int sec_id)","Invalid database request", "Check the database", 2, false);
		ostringstream info;
		info << "Table Name      : " << Fpl_Section_Points::point_table->get_table_name() << endl;
		info << "Table error info: " << query.lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
}
//Copy the geometrical points of a fpl-section of a given system id to another one in database tables
void Fpl_Section_Points::copy_fpl_geo_points(QSqlDatabase *ptr_database, const _sys_system_id src, const _sys_system_id dest, const int sec_id){
	try{
		Fpl_Section_Points::set_point_table(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}

	int glob_id=0;
	glob_id=Fpl_Section_Points::point_table->maximum_int_of_column(Fpl_Section_Points::point_table->get_column_name(fpl_label::glob_id) ,ptr_database)+1;

	QSqlQueryModel model;
	ostringstream test_filter;
	test_filter<< "SELECT ";
	test_filter << Fpl_Section_Points::point_table->get_column_name(fpl_label::glob_id);
	test_filter << " from " << Fpl_Section_Points::point_table->get_table_name();
	test_filter << " WHERE ";
	test_filter << Fpl_Section_Points::point_table->get_column_name(label::areastate_id) << " = " << src.area_state;
	test_filter << " AND ";
	test_filter << Fpl_Section_Points::point_table->get_column_name(label::measure_id) << " = " << src.measure_nr ;
	test_filter << " AND ";
	test_filter << Fpl_Section_Points::point_table->get_column_name(fpl_label::section_id) << " = " << sec_id ;

	//submit it to the datbase
	Data_Base::database_request(&model, test_filter.str(), ptr_database);
	if(model.lastError().isValid()==true){
		Error msg;
		msg.set_msg("Fpl_Section_Points::copy_fpl_geo_points(QSqlDatabase *ptr_database, const _sys_system_id src, const _sys_system_id dest, const int sec_id)","Invalid database request", "Check the database", 2, false);
		ostringstream info;
		info << "Table Name      : " << Fpl_Section_Points::point_table->get_table_name() << endl;
		info << "Table error info: " << model.lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
	test_filter.str("");

	QSqlQueryModel model1;
	for(int i=0; i< model.rowCount(); i++){
	
		test_filter.str("");
		test_filter << "INSERT INTO "<< Fpl_Section_Points::point_table->get_table_name();
		test_filter << " SELECT " << glob_id <<" , ";
		test_filter <<  Fpl_Section_Points::point_table->get_column_name(fpl_label::section_id)  <<" , ";
		test_filter <<  dest.area_state  <<" , ";
		test_filter <<  dest.measure_nr  <<" , ";
		test_filter <<  Fpl_Section_Points::point_table->get_column_name(label::applied_flag)  <<" , ";
		test_filter <<  Fpl_Section_Points::point_table->get_column_name(fpl_label::point_name)  <<" , ";
		test_filter <<  Fpl_Section_Points::point_table->get_column_name(fpl_label::point_region)  <<" , ";
		test_filter <<  Fpl_Section_Points::point_table->get_column_name(fpl_label::section_type)  <<" , ";
		test_filter <<  Fpl_Section_Points::point_table->get_column_name(fpl_label::zone_id)  <<" , ";
		test_filter <<  Fpl_Section_Points::point_table->get_column_name(fpl_label::point_x_coor)  <<" , ";
		test_filter <<  Fpl_Section_Points::point_table->get_column_name(fpl_label::point_y_coor)  <<"  ";


		test_filter << " FROM " << Fpl_Section_Points::point_table->get_table_name() <<" ";
		test_filter << " WHERE " << Fpl_Section_Points::point_table->get_column_name(fpl_label::glob_id) << " = ";
		test_filter << model.record(i).value(Fpl_Section_Points::point_table->get_column_name(fpl_label::glob_id).c_str()).toInt();

		Data_Base::database_request(&model1, test_filter.str(), ptr_database);
		if(model1.lastError().isValid()==true){
			Error msg;
			msg.set_msg("Fpl_Section_Points::copy_fpl_geo_points(QSqlDatabase *ptr_database, const _sys_system_id src, const _sys_system_id dest, const int sec_id)","Invalid database request", "Check the database", 2, false);
			ostringstream info;
			info << "Table Name      : " << Fpl_Section_Points::point_table->get_table_name() << endl;
			info << "Table error info: " << model1.lastError().text().toStdString() << endl;
			msg.make_second_info(info.str());
			throw msg;
		}
		glob_id++;
	}
	try{

		Fpl_Section_Points::switch_applied_flag_point_table(ptr_database, src, false,sec_id);
	}
	catch(Error msg){
		throw msg;

	}
}
//Read in a geometric point from the database table and retrieve them 
void Fpl_Section_Points::set_point_per_database(const _sys_system_id id_sys, const int section_id, const string point_name, QSqlDatabase *ptr_database, const string region, const string section_type, const int zone_number, const bool with_output, const bool applied_important){
	//output
	ostringstream cout;
	if(with_output==true){
		cout << "Search for point " << point_name << " in region " << region <<" in zone " << zone_number<<"..."<< endl;
		Sys_Common_Output::output_fpl->output_txt(&cout,false);
	}
	//the table is set the name and the column names
	try{
		Fpl_Section_Points::set_point_table(ptr_database);
	}catch(Error msg){
		throw msg;
	}
	this->set_point_name(point_name);

	QSqlQueryModel results;
	ostringstream test_filter;
	test_filter<< "Select * ";
	test_filter << " from " << Fpl_Section_Points::point_table->get_table_name();
	test_filter << " where " ;
	if(applied_important==true){
		test_filter << Fpl_Section_Points::point_table->get_column_name(label::applied_flag) << "= true";
		test_filter << " and ";
		test_filter << Fpl_Section_Points::point_table->get_column_name(label::areastate_id) << " ="  << id_sys.area_state;
		test_filter << " and (";
		test_filter << Fpl_Section_Points::point_table->get_column_name(label::measure_id) << " = " << 0 ;
		test_filter << " or " ;
		test_filter << Fpl_Section_Points::point_table->get_column_name(label::measure_id) << " = " << id_sys.measure_nr;
		test_filter << " ) " ;
		test_filter << " and ";
	}


	
	test_filter << Fpl_Section_Points::point_table->get_column_name(fpl_label::section_id) << " = " << section_id;	
	test_filter << " and ";
	test_filter << Fpl_Section_Points::point_table->get_column_name(fpl_label::point_name) << " = '" << point_name << "'";
	test_filter << " and ";
	test_filter << Fpl_Section_Points::point_table->get_column_name(fpl_label::point_region) << " = '" << region << "'";
	test_filter << " and ";
	test_filter << Fpl_Section_Points::point_table->get_column_name(fpl_label::section_type) << " = '" << section_type << "'";	
	test_filter << " and ";
	test_filter << Fpl_Section_Points::point_table->get_column_name(fpl_label::zone_id) << " = " << zone_number ;

	Data_Base::database_request(&results, test_filter.str(), ptr_database);

	//check the request
	if(results.lastError().isValid()){
		Error msg=this->set_error(0);
		ostringstream info;
		info << "Table Name      : " << Fpl_Section_Points::point_table->get_table_name() << endl;
		info << "Table error info: " << results.lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}

	int number=results.rowCount();
	//no results found
	if(number==0){
		Error msg=this->set_error(1);
		ostringstream info;
		info << "Table Name      : " << Fpl_Section_Points::point_table->get_table_name() << endl;
		info << "Point name      : " << point_name << endl;
		info << "Section id      : " << section_id << endl;
		info << "Region          : " << region << endl;
		info << "Zone id         : " << zone_number << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
	//multiple results found
	//if(number>1){
	//	Warning msg=this->set_warning(0);
	//	ostringstream info;
	//	info << "Table Name      : " << Fpl_Section_Points::point_table->get_table_name() << endl;
	//	info << "Point name      : " << point_name << endl;
	//	info << "Section id      : " << section_id << endl;
	//	info << "Region          : " << region << endl;
	//	info << "Zone id         : " << zone_number << endl;
	//	msg.make_second_info(info.str());
	//	msg.output_msg(1);	
	//}

	//input values
	double x_buff=0.0;
	double y_buff=0.0;

	x_buff=results.record(0).value((Fpl_Section_Points::point_table->get_column_name(fpl_label::point_x_coor)).c_str()).toDouble();
	y_buff=results.record(0).value((Fpl_Section_Points::point_table->get_column_name(fpl_label::point_y_coor)).c_str()).toDouble();
	
	this->set_point(x_buff, y_buff, point_name);
	this->zone_number=zone_number;


}
//Read in a geometric point from the database table and retrieve it 
void Fpl_Section_Points::set_point_per_database(const _sys_system_id id_sys, const int section_id, const string point_name, QSqlDatabase *ptr_database, const string region, const string section_type,  const bool with_output, const bool applied_important){
		//output
	ostringstream cout;
	if(with_output==true){
		cout << "Search for point " << point_name << " in region " <<"..."<< endl;
		Sys_Common_Output::output_fpl->output_txt(&cout,false);
	}
	//the table is set the name and the column names
	try{
		Fpl_Section_Points::set_point_table(ptr_database);
	}catch(Error msg){
		throw msg;
	}
	this->set_point_name(point_name);

	QSqlQueryModel results;
	ostringstream test_filter;
	test_filter<< "Select * ";
	test_filter << " from " << Fpl_Section_Points::point_table->get_table_name();
	test_filter << " where " ;
	if(applied_important==true){
		test_filter << Fpl_Section_Points::point_table->get_column_name(label::applied_flag) << "= true";
		test_filter << " and ";
		test_filter << Fpl_Section_Points::point_table->get_column_name(label::areastate_id) << " ="  << id_sys.area_state;
		test_filter << " and (";
		test_filter << Fpl_Section_Points::point_table->get_column_name(label::measure_id) << " = " << 0 ;
		test_filter << " or " ;
		test_filter << Fpl_Section_Points::point_table->get_column_name(label::measure_id) << " = " << id_sys.measure_nr;
		test_filter << " ) " ;
		test_filter << " and ";
	}


	
	test_filter << Fpl_Section_Points::point_table->get_column_name(fpl_label::section_id) << " = " << section_id;	
	test_filter << " and ";
	test_filter << Fpl_Section_Points::point_table->get_column_name(fpl_label::point_name) << " = '" << point_name << "'";
	test_filter << " and ";
	test_filter << Fpl_Section_Points::point_table->get_column_name(fpl_label::point_region) << " = '" << region << "'";
	test_filter << " and ";
	test_filter << Fpl_Section_Points::point_table->get_column_name(fpl_label::section_type) << " = '" << section_type << "'";	


	Data_Base::database_request(&results, test_filter.str(), ptr_database);

	//check the request
	if(results.lastError().isValid()){
		Error msg=this->set_error(0);
		ostringstream info;
		info << "Table Name      : " << Fpl_Section_Points::point_table->get_table_name() << endl;
		info << "Table error info: " << results.lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}

	int number=results.rowCount();
	//no results found
	if(number==0){
		Error msg=this->set_error(1);
		ostringstream info;
		info << "Table Name      : " << Fpl_Section_Points::point_table->get_table_name() << endl;
		info << "Point name      : " << point_name << endl;
		info << "Section id      : " << section_id << endl;
		info << "Region          : " << region << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
	//multiple results found
	//if(number>1){
	//	Warning msg=this->set_warning(0);
	//	ostringstream info;
	//	info << "Table Name      : " << Fpl_Section_Points::point_table->get_table_name() << endl;
	//	info << "Point name      : " << point_name << endl;
	//	info << "Section id      : " << section_id << endl;
	//	info << "Region          : " << region << endl;
	//	info << "Zone id         : " << zone_number << endl;
	//	msg.make_second_info(info.str());
	//	msg.output_msg(1);	
	//}

	//input values
	double x_buff=0.0;
	double y_buff=0.0;

	x_buff=results.record(0).value((Fpl_Section_Points::point_table->get_column_name(fpl_label::point_x_coor)).c_str()).toDouble();
	y_buff=results.record(0).value((Fpl_Section_Points::point_table->get_column_name(fpl_label::point_y_coor)).c_str()).toDouble();
	
	this->set_point(x_buff, y_buff, point_name);
	this->zone_number=results.record(0).value((Fpl_Section_Points::point_table->get_column_name(fpl_label::zone_id)).c_str()).toInt();

}
//Read in a geometric point from model which was filled with data from a database table and retrieve it 
void Fpl_Section_Points::set_point_per_database(QSqlQueryModel *results, const int index){
	
	//input values
	double x_buff=0.0;
	double y_buff=0.0;
	string name_buff;

	x_buff=results->record(index).value((Fpl_Section_Points::point_table->get_column_name(fpl_label::point_x_coor)).c_str()).toDouble();
	y_buff=results->record(index).value((Fpl_Section_Points::point_table->get_column_name(fpl_label::point_y_coor)).c_str()).toDouble();
	name_buff=results->record(0).value((Fpl_Section_Points::point_table->get_column_name(fpl_label::point_name)).c_str()).toString().toStdString();
	this->zone_number=results->record(index).value((Fpl_Section_Points::point_table->get_column_name(fpl_label::zone_id)).c_str()).toInt();
	this->set_point(x_buff, y_buff, name_buff);

}
//Set the input for the gemetrical points of a fpl-section per file
void Fpl_Section_Points::set_point_per_file(QFile *ifile, int *line_counter, const string name, const int zone_number, const string end_block_str){

	//read in
	string myline;
	QString qmyline;
	stringstream my_stream;
	bool found_flag=false;

	this->set_point_name(name);

	try{
		//search for the begin
		do{
			qmyline=ifile->readLine();
			myline=qmyline.toStdString();
			(*line_counter)++;
			functions::clean_string(&myline);

			if(myline==fpl_label::begin_point){
				found_flag=true;
				break;
			}
		}
		while(ifile->atEnd()==false);


		if(found_flag==false){
			Error msg=this->set_error(2);
			throw msg;
		}
		else{
			found_flag=false;
		}
		//serach for the point
		do{
			qmyline=ifile->readLine();
			myline=qmyline.toStdString();
			(*line_counter)++;
			functions::clean_string(&myline);
			my_stream.str("");
			my_stream.clear();
			my_stream << myline;
			if(myline==fpl_label::end_point){
				break;
			}
			if(myline==end_block_str){
				break;
			}
			if(myline.empty()==false){
				my_stream >> this->name >> this->x_coordinate>> this->y_coordinate>>this->zone_number;
				if(my_stream.fail()==true){
					ostringstream info;
					info << "Wrong input sequenze  : " << myline << endl;
					Error msg=this->set_error(3);
					msg.make_second_info(info.str());
					throw msg;

				}
				if(this->name==name && this->zone_number==zone_number){
					found_flag=true;
				}
			}
			if(found_flag==true){
				break;
			}
		}
		while(ifile->atEnd()==false);

		if(found_flag==false){
			Error msg=this->set_error(4);
			throw msg;

		}
	}
	catch(Error msg){
		ostringstream info;
		info << "Point name      : " << name << endl;
		info << "Zone id         : " << zone_number << endl;
		msg.make_second_info(info.str());
		throw msg;
	}


}
//Transfer the point data to database table
void Fpl_Section_Points::transfer_point2database(const _sys_system_id id_sys, const int section_id, QSqlDatabase *ptr_database, const string region, const string section_type){
	//the table is set the name and the column names
	try{
		Fpl_Section_Points::set_point_table(ptr_database);
	}catch(Error msg){
		throw msg;
	}

	int glob_id=Fpl_Section_Points::point_table->maximum_int_of_column(Fpl_Section_Points::point_table->get_column_name(fpl_label::glob_id), ptr_database)+1;

	//transfer the data
	ostringstream query_string;
	query_string << "INSERT INTO  " << Fpl_Section_Points::point_table->get_table_name();
	query_string <<" ( ";
	query_string << Fpl_Section_Points::point_table->get_column_name(fpl_label::glob_id) <<" , ";
	query_string << Fpl_Section_Points::point_table->get_column_name(fpl_label::section_id) <<" , ";
	query_string << Fpl_Section_Points::point_table->get_column_name(label::areastate_id) <<" , ";
	query_string << Fpl_Section_Points::point_table->get_column_name(label::measure_id) <<" , ";
	query_string << Fpl_Section_Points::point_table->get_column_name(label::applied_flag) <<" , ";

	query_string << Fpl_Section_Points::point_table->get_column_name(fpl_label::point_name) <<" , ";
	query_string << Fpl_Section_Points::point_table->get_column_name(fpl_label::point_region) <<" , ";
	query_string << Fpl_Section_Points::point_table->get_column_name(fpl_label::zone_id) <<" , ";

	query_string << Fpl_Section_Points::point_table->get_column_name(fpl_label::point_x_coor) <<" , ";
	query_string << Fpl_Section_Points::point_table->get_column_name(fpl_label::point_y_coor) <<" , ";
	query_string << Fpl_Section_Points::point_table->get_column_name(fpl_label::section_type) <<" ) ";

	query_string << " VALUES ( ";
	query_string << glob_id << " , " ;
	query_string << section_id<< " , " ;
	query_string << id_sys.area_state << " , " ;
	query_string << id_sys.measure_nr << " , " ;
	query_string << "true" << " , " ;

	query_string <<"'"<< this->name << "' , " ;
	query_string <<"'"<< region << "' , " ;
	query_string << this->zone_number << " , " ;

	query_string << this->x_coordinate<< " , " ;
	query_string << this->y_coordinate << " , " ;
	query_string <<"'"<< section_type << "' ) " ;

	//Set the query
	QSqlQuery query_buff(*ptr_database);
	//launch the request
	Data_Base::database_request(&query_buff, query_string.str(), ptr_database);

	if(query_buff.lastError().isValid()){
		Warning msg=this->set_warning(1);
		ostringstream info;
		info << "Table Name                : " << Fpl_Section_Points::point_table->get_table_name() << endl;
		info << "Table error info          : " << query_buff.lastError().text().toStdString() << endl;
		info << "Section index             : " << section_id << endl;
		info << "Section type              : " << section_type<< endl;
		info << "Point name                : " << this->name << endl;
		msg.make_second_info(info.str());
		msg.output_msg(1);	
	}
}
//Select points of multiple zone into a QSqlQueryModel (static)
void Fpl_Section_Points::select_different_zone_points(QSqlQueryModel *results, const _sys_system_id id_sys, const int section_id, const string point_name, QSqlDatabase *ptr_database, const string region, const string section_type, const bool with_output){
	//output
	ostringstream cout;
	if(with_output==true){
		cout << "Search for point(s) " << point_name << " in region for the zone..."<< endl;
		Sys_Common_Output::output_fpl->output_txt(&cout,false);
	}
	//the table is set the name and the column names
	try{
		Fpl_Section_Points::set_point_table(ptr_database);
	}catch(Error msg){
		throw msg;
	}

	ostringstream test_filter;
	test_filter<< "Select * ";
	test_filter << " from " << Fpl_Section_Points::point_table->get_table_name();
	test_filter << " where " ;
	test_filter << Fpl_Section_Points::point_table->get_column_name(label::applied_flag) << "= true";
	test_filter << " and ";
	test_filter << Fpl_Section_Points::point_table->get_column_name(label::areastate_id) << " ="  << id_sys.area_state;
	test_filter << " and (";
	test_filter << Fpl_Section_Points::point_table->get_column_name(label::measure_id) << " = " << 0 ;
	test_filter << " or " ;
	test_filter << Fpl_Section_Points::point_table->get_column_name(label::measure_id) << " = " << id_sys.measure_nr;
	test_filter << " ) " ;
	test_filter << " and ";
	test_filter << Fpl_Section_Points::point_table->get_column_name(fpl_label::section_id) << " = " << section_id;	
	test_filter << " and ";
	test_filter << Fpl_Section_Points::point_table->get_column_name(fpl_label::point_name) << " = '" << point_name << "'";
	test_filter << " and ";
	test_filter << Fpl_Section_Points::point_table->get_column_name(fpl_label::point_region) << " = '" << region << "'";
	test_filter << " and ";
	test_filter << Fpl_Section_Points::point_table->get_column_name(fpl_label::section_type) << " = '" << section_type << "'";	
	test_filter << " order by " << Fpl_Section_Points::point_table->get_column_name(fpl_label::glob_id);


	Data_Base::database_request(results, test_filter.str(), ptr_database);

	//check the request
	if(results->lastError().isValid()){
		Error msg;
		msg.set_msg("select_different_zone_points(QSqlQueryModel *results, const _sys_system_id id_sys, const int section_id, const string point_name, QSqlDatabase *ptr_database, const string region, const string section_type, const bool with_output)","Invalid database request", "Check the table in database", 2, false);
		ostringstream info;
		info << "Table Name      : " << Fpl_Section_Points::point_table->get_table_name() << endl;
		info << "Table error info: " << results->lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
}
//Output the members to disply/console
void Fpl_Section_Points::output_members(ostringstream *cout){
	*cout<<" SEC_POINT"<<endl;
	*cout<<"  Name         : "<<this->name<<endl;
	*cout<<"  x-coordinate : "<< this->x_coordinate<< label::m<<endl;
	*cout<<"  y-coordinate : "<< this->y_coordinate<< label::m<<endl;
	if(this->zone_number>=0){
		*cout<<"  Zone         : "<< this->zone_number<<endl;
	}

}
//copy operator
Fpl_Section_Points& Fpl_Section_Points::operator=(const Fpl_Section_Points& object){
	Geo_Point::operator =(object);
	this->zone_number=object.zone_number;

	return *this;
}
//___________
//private
//Set error(s)
Error Fpl_Section_Points::set_error(const int err_type){
		string place="Fpl_Section_Points::";
		string help;
		string reason;
		int type=0;
		bool fatal=false;
		stringstream info;
		Error msg;

	switch (err_type){
		case 0://invalid request
			place.append("set_point_per_database(const _sys_system_id id_sys, const int section_id, const string point_name, QSqlDatabase *ptr_database, const string region, const string section_type, const int zone_number, const bool with_output)");
			reason="Invalid database request";
			help="Check the table in database";
			type=2;
			fatal=true;//fatal error;
			break;
		case 1://no results found (non fatal error)
			place.append("set_point_per_database(const _sys_system_id id_sys, const int section_id, const string point_name, QSqlDatabase *ptr_database, const string region, const string section_type, const int zone_number, const bool with_output)");
			reason="No results found in table";
			help="Check the geometric point";
			type=2;
			break;
		case 2://do not find the begin of the geometrical points information in file
			place.append("set_point_per_file(QFile *ifile, int *line_counter, const string name, const int zone_number, const string end_block_str)");
			reason="Can not find the begin of the geometric points in file";
			help="The geometric point information has to start with !$BEGIN_POINT";
			type=1;
			break;
		case 3://wrong input
			place.append("set_point_per_file(QFile *ifile, int *line_counter, const string name, const int zone_number, const string end_block_str)");
			reason="There is a problem with the input; wrong sign is read in";
			help="Check point information in file";
			type=1;
			break;
		case 4://do not find the point
			place.append("set_point_per_file(QFile *ifile, int *line_counter, const string name, const int zone_number, const string end_block_str)");
			reason="The point is not found in file";
			help="Check point information in file";
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
//Set warning(s)
Warning Fpl_Section_Points::set_warning(const int warn_type){
		string place="Fpl_Section_Points::";
		string help;
		string reason;
		string reaction;
		int type=0;
		Warning msg;
		stringstream info;

	switch (warn_type){
		case 0://no unique result found
			place.append("set_point_per_database(const _sys_system_id id_sys, const int section_id, const string point_name, QSqlDatabase *ptr_database, const string region, const string section_type, const int zone_number, const bool with_output)") ;
			reason="No unique result is found";
			reaction="First result is taken";
			help= "Check the geometric point";
			type=2;	
			break;
		case 1://input datas can not submitted
			place.append("transfer_point2database(const _sys_system_id id_sys, const int section_id, QSqlDatabase *ptr_database, const string region, const string section_type)");
			reason="Can not submit the point data to the database";
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
};