#include "source_code\Dam_Headers_Precompiled.h"
//#include "Dam_CI_Polygon.h"

//init static members
Tables *Dam_CI_Polygon::polygon_table = NULL;
Tables *Dam_CI_Polygon::polygon_erg_table = NULL;
Tables *Dam_CI_Polygon::polygon_point_table = NULL;
Tables *Dam_CI_Polygon::polygon_instat_erg_table = NULL;

//Default constructor
Dam_CI_Polygon::Dam_CI_Polygon(void){
}
//Default destructor
Dam_CI_Polygon::~Dam_CI_Polygon(void){
}
//__________
//public
//Create the database table for the CI polygons (static)
void Dam_CI_Polygon::create_polygon_table(QSqlDatabase *ptr_database) {
	if (Dam_CI_Polygon::polygon_table == NULL) {
		ostringstream cout;
		cout << "Create CI polygon data database table..." << endl;
		Sys_Common_Output::output_dam->output_txt(&cout);
		//make specific input for this class
		const string tab_name = dam_label::tab_ci_polygon;
		const int num_col = 10;
		_Sys_data_tab_column tab_col[num_col];
		//init
		for (int i = 0; i < num_col; i++) {
			tab_col[i].key_flag = false;
			tab_col[i].unsigned_flag = false;
			tab_col[i].primary_key_flag = false;
		}

		tab_col[0].name = dam_label::glob_id;
		tab_col[0].type = sys_label::tab_col_type_int;
		tab_col[0].unsigned_flag = true;
		tab_col[0].primary_key_flag = true;


		tab_col[1].name = label::areastate_id;
		tab_col[1].type = sys_label::tab_col_type_int;
		tab_col[1].unsigned_flag = true;
		tab_col[1].key_flag = true;

		tab_col[2].name = label::measure_id;
		tab_col[2].type = sys_label::tab_col_type_int;
		tab_col[2].unsigned_flag = true;
		tab_col[2].key_flag = true;

		tab_col[3].name = label::applied_flag;
		tab_col[3].type = sys_label::tab_col_type_bool;
		tab_col[3].default_value = "true";
		tab_col[3].key_flag = true;

		tab_col[4].name = dam_label::polygon_name;
		tab_col[4].type = sys_label::tab_col_type_string;

		tab_col[5].name = dam_label::sector_id;
		tab_col[5].type = sys_label::tab_col_type_int;
		tab_col[5].default_value = "-1";

		tab_col[6].name = dam_label::sector_name;
		tab_col[6].type = sys_label::tab_col_type_string;

		tab_col[7].name = dam_label::end_user;
		tab_col[7].type = sys_label::tab_col_type_double;
		tab_col[7].default_value = "0.0";

		tab_col[8].name = hyd_label::polygon_out;
		tab_col[8].type = sys_label::tab_col_type_polygon;

		tab_col[9].name = label::description;
		tab_col[9].type = sys_label::tab_col_type_string;


		try {
			Dam_CI_Polygon::polygon_table = new Tables();
			if (Dam_CI_Polygon::polygon_table->create_non_existing_tables(tab_name, tab_col, num_col, ptr_database, _sys_table_type::dam) == false) {
				cout << " Table exists" << endl;
				Sys_Common_Output::output_dam->output_txt(&cout);
			};
		}
		catch (bad_alloc& t) {
			Error msg;
			msg.set_msg("Dam_CI_Polygon::create_polygon_table(QSqlDatabase *ptr_database)", "Can not allocate the memory", "Check the memory", 10, false);
			ostringstream info;
			info << "Info bad alloc: " << t.what() << endl;
			msg.make_second_info(info.str());
			throw msg;
		}
		catch (Error msg) {
			Dam_CI_Polygon::close_polygon_table();
			throw msg;
		}

		Dam_CI_Polygon::close_polygon_table();
		Dam_CI_Polygon::create_point_table(ptr_database);
	}
}
//Set the database table for the CI polygons: it sets the table name and the name of the columns and allocate them (static)
void Dam_CI_Polygon::set_polygon_table(QSqlDatabase *ptr_database) {
	if (Dam_CI_Polygon::polygon_table == NULL) {
		//make specific input for this class
		const string tab_id_name = dam_label::tab_ci_polygon;
		string tab_col[10];
		
		tab_col[0] = dam_label::glob_id;
		tab_col[1] = label::areastate_id;
		tab_col[2] = label::measure_id;
		tab_col[3] = label::applied_flag;
		tab_col[4] = dam_label::polygon_name;
		tab_col[5] = dam_label::sector_id;
		tab_col[6] = dam_label::sector_name;
		tab_col[7] = dam_label::end_user;
		tab_col[8] = hyd_label::polygon_out;
		tab_col[9] = label::description;


		try {
			Dam_CI_Polygon::polygon_table = new Tables(tab_id_name, tab_col, sizeof(tab_col) / sizeof(tab_col[0]));
			Dam_CI_Polygon::polygon_table->set_name(ptr_database, _sys_table_type::dam);
		}
		catch (bad_alloc& t) {
			Error msg;
			msg.set_msg("Dam_CI_Polygon::set_polygon_table(QSqlDatabase *ptr_database)", "Can not allocate the memory", "Check the memory", 10, false);
			ostringstream info;
			info << "Info bad alloc: " << t.what() << endl;
			msg.make_second_info(info.str());
			throw msg;
		}
		catch (Error msg) {
			Dam_CI_Polygon::close_polygon_table();
			throw msg;
		}
	}
}
//Delete all data in the database table for the sCI polygons (static)
void Dam_CI_Polygon::delete_data_in_polygon_table(QSqlDatabase *ptr_database) {
	//the table is set (the name and the column names) and allocated
	try {
		Dam_CI_Polygon::set_polygon_table(ptr_database);
	}
	catch (Error msg) {
		throw msg;
	}
	//delete the table
	Dam_CI_Polygon::polygon_table->delete_data_in_table(ptr_database);
	Dam_CI_Polygon::delete_point_data_in_table(ptr_database);
}
//Delete all data in the database table for the CI polygons of the given system state (static)
void Dam_CI_Polygon::delete_data_in_polygon_table(QSqlDatabase *ptr_database, const _sys_system_id id) {
	QSqlQueryModel results;
	ostringstream test_filter;
	try {
		Dam_CI_Polygon::set_polygon_table(ptr_database);
	}
	catch (Error msg) {
		throw msg;
	}
	//delete
	test_filter << "DELETE ";
	test_filter << " FROM " << Dam_CI_Polygon::polygon_table->get_table_name();
	test_filter << " WHERE ";
	test_filter << Dam_CI_Polygon::polygon_table->get_column_name(label::areastate_id) << " = " << id.area_state;
	test_filter << " AND ";
	test_filter << Dam_CI_Polygon::polygon_table->get_column_name(label::measure_id) << " = " << id.measure_nr;

	Data_Base::database_request(&results, test_filter.str(), ptr_database);

	//check the request
	if (results.lastError().isValid()) {
		Error msg;
		msg.set_msg("Dam_CI_Polygon::delete_data_in_polygon_table(QSqlDatabase *ptr_database, const _sys_system_id id)", "Invalid database request", "Check the database", 2, false);
		ostringstream info;
		info << "Table Name      : " << Dam_CI_Polygon::polygon_table->get_table_name() << endl;
		info << "Table error info: " << results.lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}

	//todo: delete also points
}
//Close and delete the database table for the CI polygon (static)
void Dam_CI_Polygon::close_polygon_table(void) {
	if (Dam_CI_Polygon::polygon_table != NULL) {
		delete Dam_CI_Polygon::polygon_table;
		Dam_CI_Polygon::polygon_table = NULL;
	}
	Dam_CI_Polygon::close_point_table();
}
//Get the maximum value of the global index of the CI polygon database table (static)
int Dam_CI_Polygon::get_max_glob_id_polygon_table(QSqlDatabase *ptr_database) {
	try {
		Dam_CI_Polygon::set_polygon_table(ptr_database);
	}
	catch (Error msg) {
		throw msg;
	}

	//evaluate the global identifier
	int id_glob = Dam_CI_Polygon::polygon_table->maximum_int_of_column(Dam_CI_Polygon::polygon_table->get_column_name(dam_label::glob_id), ptr_database);
	return id_glob;
}
//Get the header for inserting the CI polygon data to database table (static)
string Dam_CI_Polygon::get_insert_header_polygon_table(QSqlDatabase *ptr_database) {
	try {
		Dam_CI_Polygon::set_polygon_table(ptr_database);
	}
	catch (Error msg) {
		throw msg;
	}
	//todo
	//set the query via a query string
	ostringstream query_string;
	query_string << "INSERT INTO  " << Dam_CI_Polygon::polygon_table->get_table_name();
	query_string << " ( ";
	//query_string << Dam_CI_Polygon::polygon_table->get_column_name(dam_label::glob_id) << " , ";
	//query_string << Dam_CI_Polygon::polygon_table->get_column_name(dam_label::point_id) << " , ";
	//query_string << Dam_CI_Polygon::polygon_table->get_column_name(label::areastate_id) << " , ";
	//query_string << Dam_CI_Polygon::polygon_table->get_column_name(label::measure_id) << " , ";
	//query_string << Dam_CI_Polygon::polygon_table->get_column_name(label::applied_flag) << " , ";
	////data
	//query_string << Dam_CI_Polygon::polygon_table->get_column_name(dam_label::point_name) << " , ";
	//query_string << Dam_CI_Polygon::polygon_table->get_column_name(dam_label::sector_id) << " , ";
	//query_string << Dam_CI_Polygon::polygon_table->get_column_name(dam_label::sector_name) << " , ";
	//query_string << Dam_CI_Polygon::polygon_table->get_column_name(dam_label::sector_level) << " , ";
	//query_string << Dam_CI_Polygon::polygon_table->get_column_name(dam_label::boundary_value) << " , ";
	//query_string << Dam_CI_Polygon::polygon_table->get_column_name(dam_label::final_flag) << " , ";
	//query_string << Dam_CI_Polygon::polygon_table->get_column_name(dam_label::elem_mid_x) << " , ";
	//query_string << Dam_CI_Polygon::polygon_table->get_column_name(dam_label::elem_mid_y) << " , ";
	//query_string << Dam_CI_Polygon::polygon_table->get_column_name(dam_label::sc_point) << " ) ";


	query_string << " VALUES ";
	string buffer;
	buffer = query_string.str();
	return buffer;
}
//____________
//private
//Create the database table for the polygon point data in a database (static)
void Dam_CI_Polygon::create_point_table(QSqlDatabase *ptr_database) {
	if (Dam_CI_Polygon::polygon_point_table == NULL) {
		ostringstream cout;
		cout << "Create CI polygon point data database table..." << endl;
		Sys_Common_Output::output_hyd->output_txt(&cout);
		//make specific input for this class
		const string tab_name = dam_label::tab_ci_polygon_point;
		const int num_col = 5;
		_Sys_data_tab_column tab_col[num_col];
		//init
		for (int i = 0; i < num_col; i++) {
			tab_col[i].key_flag = false;
			tab_col[i].primary_key_flag = false;
			tab_col[i].unsigned_flag = false;
		}

		tab_col[0].name = hyd_label::polygon_point_glob_id;
		tab_col[0].type = sys_label::tab_col_type_int;
		tab_col[0].primary_key_flag = true;
		tab_col[0].unsigned_flag = true;

		tab_col[1].name = hyd_label::polygon_glob_id;
		tab_col[1].type = sys_label::tab_col_type_int;
		tab_col[1].key_flag = true;
		tab_col[1].unsigned_flag = true;

		tab_col[2].name = hyd_label::polygon_point_id;
		tab_col[2].type = sys_label::tab_col_type_int;
		tab_col[2].key_flag = true;
		tab_col[2].unsigned_flag = true;

		tab_col[3].name = hyd_label::polygon_point_x;
		tab_col[3].type = sys_label::tab_col_type_double;
		tab_col[3].default_value == "0.0";

		tab_col[4].name = hyd_label::polygon_point_y;
		tab_col[4].type = sys_label::tab_col_type_double;
		tab_col[4].default_value == "0.0";



		try {
			Dam_CI_Polygon::polygon_point_table = new Tables();
			if (Dam_CI_Polygon::polygon_point_table->create_non_existing_tables(tab_name, tab_col, num_col, ptr_database, _sys_table_type::hyd) == false) {
				cout << " Table exists" << endl;
				Sys_Common_Output::output_hyd->output_txt(&cout);
			};
		}
		catch (bad_alloc& t) {
			Error msg;
			msg.set_msg("Dam_CI_Polygon::create_point_table(QSqlDatabase *ptr_database)", "Can not allocate the memory", "Check the memory", 10, false);
			ostringstream info;
			info << "Info bad alloc: " << t.what() << endl;
			msg.make_second_info(info.str());
			throw msg;
		}
		catch (Error msg) {
			Dam_CI_Polygon::close_point_table();
			throw msg;
		}

		Dam_CI_Polygon::close_point_table();
	}

}
//Set the database table for the polygon point data: it sets the table name and the name of the columns and allocate them (static)
void Dam_CI_Polygon::set_point_table(QSqlDatabase *ptr_database) {
	if (Dam_CI_Polygon::polygon_point_table == NULL) {
		//make specific input for this class
		const string tab_id_name = dam_label::tab_ci_polygon_point;
		string tab_id_col[5];

		tab_id_col[0] = hyd_label::polygon_glob_id;
		tab_id_col[1] = hyd_label::polygon_point_id;
		tab_id_col[2] = hyd_label::polygon_point_x;
		tab_id_col[3] = hyd_label::polygon_point_y;
		tab_id_col[4] = hyd_label::polygon_point_glob_id;

		try {
			Dam_CI_Polygon::polygon_point_table = new Tables(tab_id_name, tab_id_col, sizeof(tab_id_col) / sizeof(tab_id_col[0]));
			Dam_CI_Polygon::polygon_point_table->set_name(ptr_database, _sys_table_type::hyd);
		}
		catch (bad_alloc& t) {
			Error msg;
			msg.set_msg("Dam_CI_Polygon::set_point_table(QSqlDatabase *ptr_database)", "Can not allocate the memory", "Check the memory", 10, false);
			ostringstream info;
			info << "Info bad alloc: " << t.what() << endl;
			msg.make_second_info(info.str());
			throw msg;
		}
		catch (Error msg) {
			Dam_CI_Polygon::close_point_table();
			throw msg;
		}
	}
}
//Delete all data in the database table for the polygon point data (static)
void Dam_CI_Polygon::delete_point_data_in_table(QSqlDatabase *ptr_database) {
	//the table is set (the name and the column names) and allocated
	try {
		Dam_CI_Polygon::set_point_table(ptr_database);
	}
	catch (Error msg) {
		throw msg;
	}
	//delete the table
	Dam_CI_Polygon::polygon_point_table->delete_data_in_table(ptr_database);

}
//Transfer the polygon point data of a file into a database table
void Dam_CI_Polygon::transfer_polygon_point2database_table(QSqlDatabase *ptr_database, const int poly_id) {
	//mysql query with the query_model
	QSqlQuery query_buff(*ptr_database);
	//the table is set (the name and the column names) and allocated
	try {
		Dam_CI_Polygon::set_point_table(ptr_database);
	}
	catch (Error msg) {
		throw msg;
	}

	int glob_id = Dam_CI_Polygon::polygon_point_table->maximum_int_of_column(Dam_CI_Polygon::polygon_point_table->get_column_name(hyd_label::polygon_point_glob_id), ptr_database) + 1;

	ostringstream query_header;
	query_header << "INSERT INTO  " << Dam_CI_Polygon::polygon_point_table->get_table_name();
	query_header << " ( ";
	query_header << Dam_CI_Polygon::polygon_point_table->get_column_name(hyd_label::polygon_point_glob_id) << " , ";

	query_header << Dam_CI_Polygon::polygon_point_table->get_column_name(hyd_label::polygon_glob_id) << " , ";
	query_header << Dam_CI_Polygon::polygon_point_table->get_column_name(hyd_label::polygon_point_id) << " , ";
	query_header << Dam_CI_Polygon::polygon_point_table->get_column_name(hyd_label::polygon_point_x) << " , ";
	query_header << Dam_CI_Polygon::polygon_point_table->get_column_name(hyd_label::polygon_point_y) << " ) ";


	query_header << " VALUES ";
	ostringstream query_data;

	for (int i = 0; i < this->number_segments; i++) {
		query_data << " ( ";
		query_data << glob_id << " , ";
		query_data << poly_id << " , ";
		query_data << i + 1 << " , ";
		query_data << FORMAT_FIXED_REAL << P(8);
		query_data << this->segments[i].point1.get_xcoordinate() << " , ";
		query_data << this->segments[i].point1.get_ycoordinate();
		query_data.clear();
		query_data << " ) " << " ,";
		glob_id++;
	}


	string buff;
	query_header << query_data.str();
	buff = query_header.str();
	buff.erase(buff.length() - 1);
	Data_Base::database_request(&query_buff, buff, ptr_database);
	if (query_buff.lastError().isValid()) {
		Warning msg = this->set_warning(0);
		ostringstream info;
		info << "Table Name                : " << Dam_CI_Polygon::polygon_point_table->get_table_name() << endl;
		info << "Table error info          : " << query_buff.lastError().text().toStdString() << endl;
		info << "Polygon Id                : " << poly_id << endl;
		msg.make_second_info(info.str());
		msg.output_msg(4);
	}

}
//Close and delete the database table for the polygon point data (static)
void Dam_CI_Polygon::close_point_table(void) {
	if (Dam_CI_Polygon::polygon_point_table != NULL) {
		delete Dam_CI_Polygon::polygon_point_table;
		Dam_CI_Polygon::polygon_point_table = NULL;
	}

}
//Select and count the number of relevant polygon points in a database table (static)
int Dam_CI_Polygon::select_relevant_points_database(QSqlTableModel *results, const int global_polygon_id) {
	int number = 0;
	try {
		Dam_CI_Polygon::set_point_table(&results->database());
	}
	catch (Error msg) {
		throw msg;
	}

	//give the table name to the query model
	results->setTable((Dam_CI_Polygon::polygon_point_table->get_table_name()).c_str());

	//generate the filter
	ostringstream filter;
	filter << Dam_CI_Polygon::polygon_point_table->get_column_name(hyd_label::polygon_glob_id) << " = " << global_polygon_id;

	//set the filter
	results->setFilter(filter.str().c_str());
	//database request
	Data_Base::database_request(results);
	//check the request
	if (results->lastError().isValid()) {
		Error msg;
		msg.set_msg("Dam_CI_Polygon::select_relevant_points_database(QSqlTableModel *results , const int global_polygon_id)", "Invalid database request", "Check the database", 2, false);
		ostringstream info;
		info << "Table Name      : " << Dam_CI_Polygon::polygon_point_table->get_table_name() << endl;
		info << "Table error info: " << results->lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
	//sort it
	results->sort(results->fieldIndex(Dam_CI_Polygon::polygon_point_table->get_column_name(hyd_label::polygon_point_id).c_str()), Qt::AscendingOrder);

	number = results->rowCount();


	return number;
}
//Set the warning
Warning Dam_CI_Polygon::set_warning(const int warn_type) {
	string place = "Dam_CI_Polygon::";
	string help;
	string reason;
	string reaction;
	int type = 0;
	Warning msg;
	stringstream info;

	switch (warn_type) {
	case 0://input datas can not submitted the data
		place.append("transfer_polygon_point2database_table(QSqlDatabase *ptr_database, const int poly_id)");
		reason = "Can not submit the CI polygon point data  to the database";
		help = "Check the database";
		type = 2;
		break;
	default:
		place.append("set_warning(const int warn_type)");
		reason = "Unknown flag!";
		help = "Check the flags";
		type = 5;
	}
	msg.set_msg(place, reason, help, reaction, type);
	msg.make_second_info(info.str());
	return msg;
}
//Set error(s)
Error Dam_CI_Polygon::set_error(const int err_type) {
	string place = "Dam_CI_Polygon::";
	string help;
	string reason;
	int type = 0;
	bool fatal = false;
	stringstream info;
	Error msg;

	switch (err_type) {
	case 0://bad alloc
		place.append("allocate_impact_floodplain(void)");
		reason = "Can not allocate the memory";
		help = "Check the memory";
		type = 10;
		break;
	
	default:
		place.append("set_error(const int err_type)");
		reason = "Unknown flag!";
		help = "Check the flags";
		type = 6;
	}
	msg.set_msg(place, reason, help, type, fatal);
	msg.make_second_info(info.str());
	return msg;
}
