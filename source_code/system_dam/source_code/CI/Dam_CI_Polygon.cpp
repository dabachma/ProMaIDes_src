#include "Dam_Headers_Precompiled.h"
//#include "Dam_CI_Polygon.h"

//init static members
Tables *Dam_CI_Polygon::polygon_table = NULL;
Tables *Dam_CI_Polygon::polygon_erg_table = NULL;
Tables *Dam_CI_Polygon::polygon_point_table = NULL;
Tables *Dam_CI_Polygon::polygon_instat_erg_table = NULL;

//Default constructor
Dam_CI_Polygon::Dam_CI_Polygon(void){
	this->enduser = 0.0;
	this->enduser_duration = 0.0;
	this->polygon_str=label::not_set;
	this->final_flag = true;
	this->ptr_point = &this->mid_point;
	this->is_point_id = 1;
	

	//count the memory
	Sys_Memory_Count::self()->add_mem(sizeof(Dam_CI_Polygon), _sys_system_modules::DAM_SYS);
}
//Default destructor
Dam_CI_Polygon::~Dam_CI_Polygon(void){


	//count the memory
	Sys_Memory_Count::self()->minus_mem(sizeof(Dam_CI_Polygon), _sys_system_modules::DAM_SYS);
}
//__________
//public
//Input the CI polygon data per database
void Dam_CI_Polygon::input_polygon_perdatabase(const QSqlQueryModel *results, const int glob_index) {
	try {
		this->mid_point.set_number(results->record(glob_index).value((Dam_CI_Polygon::polygon_table->get_column_name(dam_label::polygon_id)).c_str()).toInt());
		//set the element members
		this->sector_id = results->record(glob_index).value((Dam_CI_Polygon::polygon_table->get_column_name(dam_label::sector_id)).c_str()).toInt();
		this->sector_name = results->record(glob_index).value((Dam_CI_Polygon::polygon_table->get_column_name(dam_label::sector_name)).c_str()).toString().toStdString();
		this->mid_point.set_point_name(results->record(glob_index).value((Dam_CI_Polygon::polygon_table->get_column_name(dam_label::polygon_name)).c_str()).toString().toStdString());
		this->enduser = results->record(glob_index).value((Dam_CI_Polygon::polygon_table->get_column_name(dam_label::end_user)).c_str()).toDouble();
		this->polygon_str = results->record(glob_index).value(9).toString().toStdString(); 

		

		double x_mid = 0.0;
		double y_mid = 0.0;
		x_mid = results->record(glob_index).value((Dam_CI_Point::point_table->get_column_name(dam_label::elem_mid_x)).c_str()).toDouble();
		y_mid = results->record(glob_index).value((Dam_CI_Point::point_table->get_column_name(dam_label::elem_mid_y)).c_str()).toDouble();
		this->mid_point.set_point_coordinate(x_mid, y_mid);

		this->global_index = results->record(glob_index).value((Dam_CI_Point::point_table->get_column_name(dam_label::glob_id)).c_str()).toInt();
		this->stat_value = results->record(glob_index).value((Dam_CI_Point::point_table->get_column_name(dam_label::cv_value)).c_str()).toDouble();

	}
	catch (Error msg) {
		ostringstream info;
		info << "Polygon number           : " << this->mid_point.get_number() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}


}
//Create the database table for the CI polygons (static)
void Dam_CI_Polygon::create_polygon_table(QSqlDatabase *ptr_database) {
	if (Dam_CI_Polygon::polygon_table == NULL) {
		ostringstream cout;
		cout << "Create CI polygon data database table..." << endl;
		Sys_Common_Output::output_dam->output_txt(&cout);
		//make specific input for this class
		const string tab_name = dam_label::tab_ci_polygon;
		const int num_col = 15;
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

		tab_col[1].name = dam_label::polygon_id;
		tab_col[1].type = sys_label::tab_col_type_int;
		tab_col[1].unsigned_flag = true;

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

		tab_col[5].name = dam_label::polygon_name;
		tab_col[5].type = sys_label::tab_col_type_string;

		tab_col[6].name = dam_label::sector_id;
		tab_col[6].type = sys_label::tab_col_type_int;
		tab_col[6].default_value = "-1";

		tab_col[7].name = dam_label::sector_name;
		tab_col[7].type = sys_label::tab_col_type_string;

		tab_col[8].name = dam_label::end_user;
		tab_col[8].type = sys_label::tab_col_type_double;
		tab_col[8].default_value = "0.0";

		tab_col[9].name = dam_label::aut_value;
		tab_col[9].type = sys_label::tab_col_type_int;
		tab_col[9].default_value = "0";

		tab_col[10].name = dam_label::cv_value;
		tab_col[10].type = sys_label::tab_col_type_double;
		tab_col[10].default_value = "-1.0";


		tab_col[11].name = dam_label::elem_mid_x;
		tab_col[11].type = sys_label::tab_col_type_double;

		tab_col[12].name = dam_label::elem_mid_y;
		tab_col[12].type = sys_label::tab_col_type_double;

		tab_col[13].name = hyd_label::polygon_out;
		tab_col[13].type = sys_label::tab_col_type_polygon;

		tab_col[14].name = label::description;
		tab_col[14].type = sys_label::tab_col_type_string;


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
void Dam_CI_Polygon::set_polygon_table(QSqlDatabase *ptr_database, const bool not_close) {
	if (Dam_CI_Polygon::polygon_table == NULL) {
		//make specific input for this class
		const string tab_id_name = dam_label::tab_ci_polygon;
		string tab_col[15];
		
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
		tab_col[10] = dam_label::polygon_id;
		tab_col[11]= dam_label::elem_mid_y;
		tab_col[12] = dam_label::elem_mid_x;
		tab_col[13] = dam_label::aut_value;
		tab_col[14] = dam_label::cv_value;




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
			if (not_close == false) {
				Dam_CI_Polygon::close_polygon_table();
			}
			throw msg;
		}
	}
	Dam_CI_Polygon::set_point_table(ptr_database);
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
	query_string << Dam_CI_Polygon::polygon_table->get_column_name(dam_label::glob_id) << " , ";
	query_string << Dam_CI_Polygon::polygon_table->get_column_name(dam_label::polygon_id) << " , ";
	query_string << Dam_CI_Polygon::polygon_table->get_column_name(label::areastate_id) << " , ";
	query_string << Dam_CI_Polygon::polygon_table->get_column_name(label::measure_id) << " , ";
	query_string << Dam_CI_Polygon::polygon_table->get_column_name(label::applied_flag) << " , ";
	//data
	query_string << Dam_CI_Polygon::polygon_table->get_column_name(dam_label::polygon_name) << " , ";
	query_string << Dam_CI_Polygon::polygon_table->get_column_name(dam_label::sector_id) << " , ";
	query_string << Dam_CI_Polygon::polygon_table->get_column_name(dam_label::sector_name) << " , ";
	query_string << Dam_CI_Polygon::polygon_table->get_column_name(dam_label::end_user) << " , ";
	query_string << Dam_CI_Polygon::polygon_table->get_column_name(dam_label::elem_mid_x) << " , ";
	query_string << Dam_CI_Polygon::polygon_table->get_column_name(dam_label::elem_mid_y) << " , ";
	query_string << Dam_CI_Polygon::polygon_table->get_column_name(hyd_label::polygon_out) << " ) ";


	query_string << " VALUES ";
	string buffer;
	buffer = query_string.str();
	return buffer;
}
//Select and count the number of relevant CI-polygons in a database table (static)
int Dam_CI_Polygon::select_relevant_polygon_database(QSqlQueryModel *results, QSqlDatabase *ptr_database, const _sys_system_id id, const bool with_output) {
	int number = 0;
	try {
		Dam_CI_Polygon::set_polygon_table(ptr_database);
	}
	catch (Error msg) {
		throw msg;
	}
	if (with_output == true) {
		ostringstream cout;
		cout << "Select relevant CI-polygons in database ..." << endl;
		Sys_Common_Output::output_dam->output_txt(&cout);
	}


	ostringstream test_filter;
	test_filter << "Select ";
	test_filter << Dam_CI_Polygon::polygon_table->get_column_name(dam_label::polygon_id) << " , ";
	test_filter << Dam_CI_Polygon::polygon_table->get_column_name(dam_label::elem_mid_x) << " , ";
	test_filter << Dam_CI_Polygon::polygon_table->get_column_name(dam_label::elem_mid_y) << " , ";
	test_filter << Dam_CI_Polygon::polygon_table->get_column_name(dam_label::glob_id) << " , ";
	//set the element members
	test_filter << Dam_CI_Polygon::polygon_table->get_column_name(dam_label::polygon_name) << " , ";
	test_filter << Dam_CI_Polygon::polygon_table->get_column_name(dam_label::sector_id) << " , ";
	test_filter << Dam_CI_Polygon::polygon_table->get_column_name(dam_label::sector_name) << " , ";
	test_filter << Dam_CI_Polygon::polygon_table->get_column_name(dam_label::end_user) << " , ";
	test_filter << Dam_CI_Polygon::polygon_table->get_column_name(dam_label::cv_value) << " , ";
	//be aware an index is used in input_polygon_perdatabase
	test_filter << " ST_ASTEXT(" << Dam_CI_Polygon::polygon_table->get_column_name(hyd_label::polygon_out) << ")";



	test_filter << " from " << Dam_CI_Polygon::polygon_table->get_table_name();
	test_filter << " where ";
	test_filter << Dam_CI_Polygon::polygon_table->get_column_name(label::applied_flag) << "= true";
	test_filter << " and ";
	test_filter << Dam_CI_Polygon::polygon_table->get_column_name(label::areastate_id) << " =" << id.area_state;
	test_filter << " and (";
	test_filter << Dam_CI_Polygon::polygon_table->get_column_name(label::measure_id) << " = " << 0;
	test_filter << " or ";
	test_filter << Dam_CI_Polygon::polygon_table->get_column_name(label::measure_id) << " = " << id.measure_nr;
	test_filter << " ) ";
	test_filter << " order by " << Dam_CI_Polygon::polygon_table->get_column_name(dam_label::polygon_id);

	Data_Base::database_request(results, test_filter.str(), ptr_database);

	//check the request
	if (results->lastError().isValid()) {
		Error msg;
		msg.set_msg("Dam_CI_Polygon::select_relevant_polygon_database(QSqlTableModel *results, const _sys_system_id id, const bool with_output)", "Invalid database request", "Check the database", 2, false);
		ostringstream info;
		info << "Table Name      : " << Dam_CI_Polygon::polygon_table->get_table_name() << endl;
		info << "Table error info: " << results->lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}

	number = results->rowCount();

	if (with_output == true) {
		ostringstream cout;
		cout << number << " relevant CI-polygon(s) are selected in database" << endl;
		Sys_Common_Output::output_dam->output_txt(&cout);
	}

	return number;
}
//Select and count the number of relevant CI-polygon in a database table (just part of it) (static)
int Dam_CI_Polygon::select_relevant_polygon_database(QSqlQueryModel *results, QSqlDatabase *ptr_database, const _sys_system_id id, const int offset, const int number_rows, const bool with_output) {
	int number = 0;
	try {
		Dam_CI_Polygon::set_polygon_table(ptr_database);
	}
	catch (Error msg) {
		throw msg;
	}
	if (with_output == true) {
		ostringstream cout;
		cout << "Select relevant CI-polygons in database ..." << endl;
		Sys_Common_Output::output_dam->output_txt(&cout);
	}


	ostringstream test_filter;
	test_filter << "Select ";
	test_filter << Dam_CI_Polygon::polygon_table->get_column_name(dam_label::polygon_id) << " , ";
	test_filter << Dam_CI_Polygon::polygon_table->get_column_name(dam_label::elem_mid_x) << " , ";
	test_filter << Dam_CI_Polygon::polygon_table->get_column_name(dam_label::elem_mid_y) << " , ";
	test_filter << Dam_CI_Polygon::polygon_table->get_column_name(dam_label::glob_id) << " , ";
	//set the element members
	test_filter << Dam_CI_Polygon::polygon_table->get_column_name(dam_label::polygon_name) << " , ";
	test_filter << Dam_CI_Polygon::polygon_table->get_column_name(dam_label::sector_id) << " , ";
	test_filter << Dam_CI_Polygon::polygon_table->get_column_name(dam_label::sector_name) << " , ";

	test_filter << Dam_CI_Polygon::polygon_table->get_column_name(dam_label::end_user) << " , ";
	test_filter << Dam_CI_Polygon::polygon_table->get_column_name(dam_label::cv_value) << " , ";
	//be aware an index is used in input_polygon_perdatabase
	test_filter << " ST_ASTEXT(" << Dam_CI_Polygon::polygon_table->get_column_name(hyd_label::polygon_out) << ")";



	test_filter << " from " << Dam_CI_Polygon::polygon_table->get_table_name();
	test_filter << " where ";
	test_filter << Dam_CI_Polygon::polygon_table->get_column_name(label::applied_flag) << "= true";
	test_filter << " and ";
	test_filter << Dam_CI_Polygon::polygon_table->get_column_name(label::areastate_id) << " =" << id.area_state;
	test_filter << " and (";
	test_filter << Dam_CI_Polygon::polygon_table->get_column_name(label::measure_id) << " = " << 0;
	test_filter << " or ";
	test_filter << Dam_CI_Polygon::polygon_table->get_column_name(label::measure_id) << " = " << id.measure_nr;
	test_filter << " ) ";
	test_filter << " order by " << Dam_CI_Polygon::polygon_table->get_column_name(dam_label::polygon_id);
	test_filter << " LIMIT " << number_rows;
	test_filter << " OFFSET " << offset;

	Data_Base::database_request(results, test_filter.str(), ptr_database);

	//check the request
	if (results->lastError().isValid()) {
		Error msg;
		msg.set_msg("Dam_CI_Polygon::select_relevant_polygon_database(QSqlQueryModel *results, QSqlDatabase *ptr_database, const _sys_system_id id, const int offset, const int number_rows, const bool with_output)", "Invalid database request", "Check the database", 2, false);
		ostringstream info;
		info << "Table Name      : " << Dam_CI_Polygon::polygon_table->get_table_name() << endl;
		info << "Table error info: " << results->lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}

	number = results->rowCount();

	if (with_output == true) {
		ostringstream cout;
		cout << number << " relevant CI-polygon(s) are selected in database" << endl;
		Sys_Common_Output::output_dam->output_txt(&cout);
	}

	return number;
}
//Count the number of relevant CI-polygon in a database table (static)
int Dam_CI_Polygon::count_relevant_polygon_database(QSqlQueryModel *results, QSqlDatabase *ptr_database, const _sys_system_id id, const bool with_output) {
	int number = 0;
	try {
		Dam_CI_Polygon::set_polygon_table(ptr_database);
	}
	catch (Error msg) {
		throw msg;
	}
	if (with_output == true) {
		ostringstream cout;
		cout << "Count relevant CI-polygons in database ..." << endl;
		Sys_Common_Output::output_dam->output_txt(&cout);
	}

	ostringstream test_filter;
	test_filter << "Select COUNT(";
	test_filter << Dam_CI_Polygon::polygon_table->get_column_name(dam_label::polygon_id) << " ) ";

	test_filter << " from " << Dam_CI_Polygon::polygon_table->get_table_name();
	test_filter << " where ";
	test_filter << Dam_CI_Polygon::polygon_table->get_column_name(label::applied_flag) << "= true";
	test_filter << " and ";
	test_filter << Dam_CI_Polygon::polygon_table->get_column_name(label::areastate_id) << " =" << id.area_state;
	test_filter << " and (";
	test_filter << Dam_CI_Polygon::polygon_table->get_column_name(label::measure_id) << " = " << 0;
	test_filter << " or ";
	test_filter << Dam_CI_Polygon::polygon_table->get_column_name(label::measure_id) << " = " << id.measure_nr;
	test_filter << " ) ";

	Data_Base::database_request(results, test_filter.str(), ptr_database);

	//check the request
	if (results->lastError().isValid()) {
		Error msg;
		msg.set_msg("Dam_CI_Polygon::count_relevant_polygons_database(QSqlTableModel *results, const _sys_system_id id, const bool with_output)", "Invalid database request", "Check the database", 2, false);
		ostringstream info;
		info << "Table Name      : " << Dam_CI_Polygon::polygon_table->get_table_name() << endl;
		info << "Table error info: " << results->lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}

	if (results->rowCount() > 0) {
		number = results->record(0).field(0).value().toInt();
	}

	if (with_output == true) {
		ostringstream cout;
		cout << number << " relevant CI-polygon(s) are found in database" << endl;
		Sys_Common_Output::output_dam->output_txt(&cout);
	}

	return number;
}
//Get a string to set the polygon data from the statistical calculation to the database table
bool Dam_CI_Polygon::get_string_stat_polygon_data2database(ostringstream *text) {
	//generate the filter
	//todo
	*text << "UPDATE " << Dam_CI_Polygon::polygon_table->get_table_name().c_str();
	*text << " SET ";
	*text << Dam_CI_Polygon::polygon_table->get_column_name(dam_label::aut_value).c_str() << " = " << this->no_incoming << " , ";
	*text << Dam_CI_Polygon::polygon_table->get_column_name(dam_label::cv_value).c_str() << " = " << this->stat_value << "  ";



	*text << " WHERE ";
	*text << Dam_CI_Polygon::polygon_table->get_column_name(dam_label::glob_id) << " = " << this->global_index << "; ";

	return true;
}
//Create the database table for the results of the damage calculation per CI-polygon (static)
void Dam_CI_Polygon::create_erg_table(QSqlDatabase *ptr_database) {
	if (Dam_CI_Polygon::polygon_erg_table == NULL) {
		ostringstream cout;
		cout << "Create CI-polygon result database table..." << endl;
		Sys_Common_Output::output_dam->output_txt(&cout);
		//make specific input for this class
		const string tab_name = dam_label::tab_ci_polygon_erg;
		const int num_col = 13;
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

		tab_col[1].name = dam_label::polygon_id;
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

		tab_col[7].name = dam_label::sector_id;
		tab_col[7].type = sys_label::tab_col_type_int;
		tab_col[7].default_value = "-1";

		tab_col[8].name = dam_label::end_user;
		tab_col[8].type = sys_label::tab_col_type_double;
		tab_col[8].default_value = "0.0";


		tab_col[9].name = dam_label::failure_duration;
		tab_col[9].type = sys_label::tab_col_type_double;
		tab_col[9].default_value = "0.0";

		tab_col[10].name = dam_label::enduser_duration;
		tab_col[10].type = sys_label::tab_col_type_double;
		tab_col[10].default_value = "0.0";

		tab_col[11].name = dam_label::failure_type;
		tab_col[11].type = sys_label::tab_col_type_string;


		tab_col[12].name = hyd_label::polygon_out;
		tab_col[12].type = sys_label::tab_col_type_polygon;

		try {
			Dam_CI_Polygon::polygon_erg_table = new Tables();
			if (Dam_CI_Polygon::polygon_erg_table->create_non_existing_tables(tab_name, tab_col, num_col, ptr_database, _sys_table_type::dam) == false) {
				cout << " Table exists" << endl;
				Sys_Common_Output::output_dam->output_txt(&cout);
			};
		}
		catch (bad_alloc& t) {
			Error msg;
			msg.set_msg("Dam_CI_Polygon::create_erg_table(QSqlDatabase *ptr_database)", "Can not allocate the memory", "Check the memory", 10, false);
			ostringstream info;
			info << "Info bad alloc: " << t.what() << endl;
			msg.make_second_info(info.str());
			throw msg;
		}
		catch (Error msg) {
			Dam_CI_Polygon::close_erg_table();
			throw msg;
		}

		Dam_CI_Polygon::close_erg_table();
	}
}
//Set the database table for the results of the damage calculation per CI-polygon: it sets the table name and the name of the columns and allocate them (static)
void Dam_CI_Polygon::set_erg_table(QSqlDatabase *ptr_database, const bool not_close) {
	if (Dam_CI_Polygon::polygon_erg_table == NULL) {
		//make specific input for this class
		const string tab_id_name = dam_label::tab_ci_polygon_erg;
		string tab_col[13];

		tab_col[0] = dam_label::glob_id;
		tab_col[1] = dam_label::polygon_id;
		tab_col[2] = label::areastate_id;
		tab_col[3] = label::measure_id;
		tab_col[4] = label::applied_flag;
		tab_col[5] = hyd_label::sz_bound_id;
		tab_col[6] = risk_label::sz_break_id;
		tab_col[7] = dam_label::sector_id;
		tab_col[8] = dam_label::end_user;
		tab_col[9] = dam_label::enduser_duration;
		tab_col[10] = dam_label::failure_duration;
		tab_col[11] = hyd_label::polygon_out;
		tab_col[12] = dam_label::failure_type;
		

		try {
			Dam_CI_Polygon::polygon_erg_table = new Tables(tab_id_name, tab_col, sizeof(tab_col) / sizeof(tab_col[0]));
			Dam_CI_Polygon::polygon_erg_table->set_name(ptr_database, _sys_table_type::dam);
		}
		catch (bad_alloc& t) {
			Error msg;
			msg.set_msg("Dam_CI_Polygon::set_erg_table(QSqlDatabase *ptr_database)", "Can not allocate the memory", "Check the memory", 10, false);
			ostringstream info;
			info << "Info bad alloc: " << t.what() << endl;
			msg.make_second_info(info.str());
			throw msg;
		}
		catch (Error msg) {
			if (not_close == false) {
				Dam_CI_Polygon::close_erg_table();
			}
			throw msg;
		}
	}
}
//Delete all data in the database table for the results of the damage calculation per CI-polygon (static)
void Dam_CI_Polygon::delete_data_in_erg_table(QSqlDatabase *ptr_database) {
	//the table is set (the name and the column names) and allocated
	try {
		Dam_CI_Polygon::set_erg_table(ptr_database);
	}
	catch (Error msg) {
		throw msg;
	}
	//delete the table
	Dam_CI_Polygon::polygon_erg_table->delete_data_in_table(ptr_database);
}
//Delete the data in the database table for the results of the damage calculation specified by the system state (static)
void Dam_CI_Polygon::delete_data_in_erg_table(QSqlDatabase *ptr_database, const _sys_system_id id) {
	//the table is set (the name and the column names) and allocated
	try {
		Dam_CI_Polygon::set_erg_table(ptr_database);
	}
	catch (Error msg) {
		throw msg;
	}

	//delete the table
	QSqlQuery query(*ptr_database);
	ostringstream query_string;
	query_string << "DELETE FROM " << Dam_CI_Polygon::polygon_erg_table->get_table_name();
	query_string << " WHERE ";
	query_string << Dam_CI_Polygon::polygon_erg_table->get_column_name(label::areastate_id) << " = " << id.area_state;
	query_string << " AND ";
	query_string << Dam_CI_Polygon::polygon_erg_table->get_column_name(label::measure_id) << " = " << id.measure_nr;

	Data_Base::database_request(&query, query_string.str(), ptr_database);
	if (query.lastError().isValid() == true) {
		Error msg;
		msg.set_msg("Dam_CI_Polygon::delete_data_in_erg_table(QSqlDatabase *ptr_database, const _sys_system_id id)", "Invalid database request", "Check the database", 2, false);
		ostringstream info;
		info << "Table Name      : " << Dam_CI_Polygon::polygon_erg_table->get_table_name() << endl;
		info << "Table error info: " << query.lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
}
//Delete the data in the database table for the results of the damage calculation specified parameters (static)
void Dam_CI_Polygon::delete_data_in_erg_table(QSqlDatabase *ptr_database, const _sys_system_id id, const string break_sz, const bool like_flag) {
	//the table is set (the name and the column names) and allocated
	try {
		Dam_CI_Polygon::set_erg_table(ptr_database);
	}
	catch (Error msg) {
		throw msg;
	}

	//delete the table
	QSqlQuery query(*ptr_database);
	ostringstream query_string;
	query_string << "DELETE FROM " << Dam_CI_Polygon::polygon_erg_table->get_table_name();
	query_string << " WHERE ";
	query_string << Dam_CI_Polygon::polygon_erg_table->get_column_name(label::areastate_id) << " = " << id.area_state;
	query_string << " AND ";
	query_string << Dam_CI_Polygon::polygon_erg_table->get_column_name(label::measure_id) << " = " << id.measure_nr;
	query_string << " AND ";
	if (like_flag == false) {
		query_string << Dam_CI_Polygon::polygon_erg_table->get_column_name(risk_label::sz_break_id) << " = '" << break_sz << "'";
	}
	else {
		query_string << Dam_CI_Polygon::polygon_erg_table->get_column_name(risk_label::sz_break_id) << " LIKE '" << break_sz << "'";
	}

	Data_Base::database_request(&query, query_string.str(), ptr_database);
	if (query.lastError().isValid() == true) {
		Error msg;
		msg.set_msg("Dam_CI_Polygon::delete_data_in_erg_table(QSqlDatabase *ptr_database, const _sys_system_id id, const string break_sz, const bool like_flag)", "Invalid database request", "Check the database", 2, false);
		ostringstream info;
		info << "Table Name      : " << Dam_CI_Polygon::polygon_erg_table->get_table_name() << endl;
		info << "Table error info: " << query.lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
}
//Delete the data in the database table for the results of the damage calculation specified by the system id and the scenario-ids (static)
void Dam_CI_Polygon::delete_data_in_erg_table(QSqlDatabase *ptr_database, const _sys_system_id id, const int bound_sz, const string break_sz) {
	//the table is set (the name and the column names) and allocated
	try {
		Dam_CI_Polygon::set_erg_table(ptr_database);
	}
	catch (Error msg) {
		throw msg;
	}

	//delete the table
	QSqlQuery query(*ptr_database);
	ostringstream query_string;
	query_string << "DELETE FROM " << Dam_CI_Polygon::polygon_erg_table->get_table_name();
	query_string << " WHERE ";
	query_string << Dam_CI_Polygon::polygon_erg_table->get_column_name(label::areastate_id) << " = " << id.area_state;
	query_string << " AND ";
	query_string << Dam_CI_Polygon::polygon_erg_table->get_column_name(label::measure_id) << " = " << id.measure_nr;
	query_string << " AND ";
	query_string << Dam_CI_Polygon::polygon_erg_table->get_column_name(hyd_label::sz_bound_id) << " = " << bound_sz;
	query_string << " AND ";
	query_string << Dam_CI_Polygon::polygon_erg_table->get_column_name(risk_label::sz_break_id) << " = '" << break_sz << "'";

	Data_Base::database_request(&query, query_string.str(), ptr_database);
	if (query.lastError().isValid() == true) {
		Error msg;
		msg.set_msg("Dam_CI_Polygon::delete_data_in_erg_table(QSqlDatabase *ptr_database, const _sys_system_id id, const int bound_sz, const string break_sz )", "Invalid database request", "Check the database", 2, false);
		ostringstream info;
		info << "Table Name      : " << Dam_CI_Polygon::polygon_erg_table->get_table_name() << endl;
		info << "Table error info: " << query.lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
}
//Delete the data in the database table for the results of the damage calculation specified by the hydraulic scenario-ids (static)
void Dam_CI_Polygon::delete_data_in_erg_table(QSqlDatabase *ptr_database, const int bound_sz) {
	//the table is set (the name and the column names) and allocated
	try {
		Dam_CI_Polygon::set_erg_table(ptr_database);
	}
	catch (Error msg) {
		throw msg;
	}

	//delete the table
	QSqlQuery query(*ptr_database);
	ostringstream query_string;
	query_string << "DELETE FROM " << Dam_CI_Polygon::polygon_erg_table->get_table_name();
	query_string << " WHERE ";
	query_string << Dam_CI_Polygon::polygon_erg_table->get_column_name(hyd_label::sz_bound_id) << " = " << bound_sz;

	Data_Base::database_request(&query, query_string.str(), ptr_database);
	if (query.lastError().isValid() == true) {
		Error msg;
		msg.set_msg("Dam_CI_Polygon::delete_data_in_erg_table(QSqlDatabase *ptr_database, const int bound_sz)", "Invalid database request", "Check the database", 2, false);
		ostringstream info;
		info << "Table Name      : " << Dam_CI_Polygon::polygon_erg_table->get_table_name() << endl;
		info << "Table error info: " << query.lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
}
//Close and delete the database table for the results of the damage calculation per CI-polygon (static)
void Dam_CI_Polygon::close_erg_table(void) {
	if (Dam_CI_Polygon::polygon_erg_table != NULL) {
		delete Dam_CI_Polygon::polygon_erg_table;
		Dam_CI_Polygon::polygon_erg_table = NULL;
	}
}
//Get the maximum value of the global index of the result polygon database table (static)
int Dam_CI_Polygon::get_max_glob_id_polygon_erg_table(QSqlDatabase *ptr_database) {
	try {
		Dam_CI_Polygon::set_erg_table(ptr_database);
	}
	catch (Error msg) {
		throw msg;
	}

	//evaluate the global identifier
	int id_glob = Dam_CI_Polygon::polygon_erg_table->maximum_int_of_column(Dam_CI_Polygon::polygon_erg_table->get_column_name(dam_label::glob_id), ptr_database);
	return id_glob;
}
//Get the header for inserting the CI-polygon result data to database table (static)
string Dam_CI_Polygon::get_insert_header_erg_table(QSqlDatabase *ptr_database) {
	try {
		Dam_CI_Polygon::set_erg_table(ptr_database);
	}
	catch (Error msg) {
		throw msg;
	}
	//set the query via a query string
	ostringstream query_string;
	query_string << "INSERT INTO  " << Dam_CI_Polygon::polygon_erg_table->get_table_name();
	query_string << " ( ";
	query_string << Dam_CI_Polygon::polygon_erg_table->get_column_name(dam_label::glob_id) << " , ";
	query_string << Dam_CI_Polygon::polygon_erg_table->get_column_name(dam_label::polygon_id) << " , ";
	query_string << Dam_CI_Polygon::polygon_erg_table->get_column_name(label::areastate_id) << " , ";
	query_string << Dam_CI_Polygon::polygon_erg_table->get_column_name(label::measure_id) << " , ";
	query_string << Dam_CI_Polygon::polygon_erg_table->get_column_name(hyd_label::sz_bound_id) << " , ";
	query_string << Dam_CI_Polygon::polygon_erg_table->get_column_name(risk_label::sz_break_id) << " , ";
	//results
	query_string << Dam_CI_Polygon::polygon_erg_table->get_column_name(dam_label::sector_id) << " , ";
	query_string << Dam_CI_Polygon::polygon_erg_table->get_column_name(dam_label::end_user) << " , ";
	query_string << Dam_CI_Polygon::polygon_erg_table->get_column_name(dam_label::enduser_duration) << " , ";
	query_string << Dam_CI_Polygon::polygon_erg_table->get_column_name(dam_label::failure_duration) << " , ";
	query_string << Dam_CI_Polygon::polygon_erg_table->get_column_name(dam_label::failure_type) << " , ";

	query_string << Dam_CI_Polygon::polygon_erg_table->get_column_name(hyd_label::polygon_out) << ")";

	query_string << " VALUES ";
	string buffer;
	buffer = query_string.str();
	return buffer;
}
//Select data in the database table for the results of the damage calculation specified by the system id and the scenario-ids (static)
int Dam_CI_Polygon::select_data_in_erg_table(QSqlQueryModel *results, QSqlDatabase *ptr_database, const _sys_system_id id, const int bound_sz, const string break_sz) {
	//the table is set (the name and the column names) and allocated
	try {
		Dam_CI_Polygon::set_erg_table(ptr_database);
	}
	catch (Error msg) {
		throw msg;
	}



	ostringstream query_string;
	query_string << "SELECT ";
	query_string << Dam_CI_Polygon::polygon_erg_table->get_column_name(dam_label::polygon_id) << " , ";

	query_string << Dam_CI_Polygon::polygon_erg_table->get_column_name(dam_label::sector_id) << " , ";
	query_string << Dam_CI_Polygon::polygon_erg_table->get_column_name(dam_label::enduser_duration) << " , ";
	query_string << Dam_CI_Polygon::polygon_erg_table->get_column_name(dam_label::failure_duration) << " , ";
	query_string << Dam_CI_Polygon::polygon_erg_table->get_column_name(dam_label::end_user) << " , ";
	query_string << Dam_CI_Polygon::polygon_erg_table->get_column_name(dam_label::failure_type) << " , ";

	query_string << " ST_ASTEXT(" << Dam_CI_Polygon::polygon_erg_table->get_column_name(hyd_label::polygon_out) << ")";

	query_string << " FROM " << Dam_CI_Polygon::polygon_erg_table->get_table_name();
	query_string << " WHERE ";
	query_string << Dam_CI_Polygon::polygon_erg_table->get_column_name(label::applied_flag) << "= true";
	query_string << " AND ";
	query_string << Dam_CI_Polygon::polygon_erg_table->get_column_name(label::areastate_id) << " =" << id.area_state;
	query_string << " AND (";
	query_string << Dam_CI_Polygon::polygon_erg_table->get_column_name(label::measure_id) << " = " << 0;
	query_string << " OR ";
	query_string << Dam_CI_Polygon::polygon_erg_table->get_column_name(label::measure_id) << " = " << id.measure_nr;
	query_string << " ) ";
	query_string << " AND ";
	query_string << Dam_CI_Polygon::polygon_erg_table->get_column_name(hyd_label::sz_bound_id) << " = " << bound_sz;
	query_string << " AND ";
	query_string << Dam_CI_Polygon::polygon_erg_table->get_column_name(risk_label::sz_break_id) << " = '" << break_sz << "'";

	query_string << " ORDER BY ";
	query_string << Dam_CI_Polygon::polygon_erg_table->get_column_name(dam_label::polygon_id) << "  ";

	Data_Base::database_request(results, query_string.str(), ptr_database);
	if (results->lastError().isValid() == true) {
		Error msg;
		msg.set_msg("Dam_CI_Polygon::select_data_in_erg_table(QSqlDatabase *ptr_database, const _sys_system_id id, const int bound_sz, const string break_sz )", "Invalid database request", "Check the database", 2, false);
		ostringstream info;
		info << "Table Name      : " << Dam_CI_Polygon::polygon_erg_table->get_table_name() << endl;
		info << "Table error info: " << results->lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}

	return results->rowCount();
}
//Select data in the database table for the results of the damage calculation specified by the system id and the scenario-ids (just part of it) (static)
int Dam_CI_Polygon::select_data_in_erg_table(QSqlQueryModel *results, QSqlDatabase *ptr_database, const _sys_system_id id, const int bound_sz, const string break_sz, const int offset, const int number_row) {
	//the table is set (the name and the column names) and allocated
	try {
		Dam_CI_Polygon::set_erg_table(ptr_database);
	}
	catch (Error msg) {
		throw msg;
	}

	ostringstream query_string;
	query_string << "SELECT ";
	query_string << Dam_CI_Polygon::polygon_erg_table->get_column_name(dam_label::polygon_id) << " , ";

	query_string << Dam_CI_Polygon::polygon_erg_table->get_column_name(dam_label::sector_id) << " , ";
	query_string << Dam_CI_Polygon::polygon_erg_table->get_column_name(dam_label::enduser_duration) << " , ";
	query_string << Dam_CI_Polygon::polygon_erg_table->get_column_name(dam_label::failure_duration) << " , ";
	query_string << Dam_CI_Polygon::polygon_erg_table->get_column_name(dam_label::end_user) << " , ";
	query_string << Dam_CI_Polygon::polygon_erg_table->get_column_name(dam_label::failure_type) << " , ";

	query_string << " ST_ASTEXT(" << Dam_CI_Polygon::polygon_erg_table->get_column_name(hyd_label::polygon_out) << ")";

	query_string << " FROM " << Dam_CI_Polygon::polygon_erg_table->get_table_name();
	query_string << " WHERE ";
	query_string << Dam_CI_Polygon::polygon_erg_table->get_column_name(label::applied_flag) << "= true";
	query_string << " AND ";
	query_string << Dam_CI_Polygon::polygon_erg_table->get_column_name(label::areastate_id) << " =" << id.area_state;
	query_string << " AND (";
	query_string << Dam_CI_Polygon::polygon_erg_table->get_column_name(label::measure_id) << " = " << 0;
	query_string << " OR ";
	query_string << Dam_CI_Polygon::polygon_erg_table->get_column_name(label::measure_id) << " = " << id.measure_nr;
	query_string << " ) ";
	query_string << " AND ";
	query_string << Dam_CI_Polygon::polygon_erg_table->get_column_name(hyd_label::sz_bound_id) << " = " << bound_sz;
	query_string << " AND ";
	query_string << Dam_CI_Polygon::polygon_erg_table->get_column_name(risk_label::sz_break_id) << " = '" << break_sz << "'";

	query_string << " ORDER BY ";
	query_string << Dam_CI_Polygon::polygon_erg_table->get_column_name(dam_label::polygon_id) << "  ";
	query_string << " LIMIT " << number_row;
	query_string << " OFFSET " << offset;

	Data_Base::database_request(results, query_string.str(), ptr_database);
	if (results->lastError().isValid() == true) {
		Error msg;
		msg.set_msg("Dam_CI_Polygon::select_data_in_erg_table(QSqlDatabase *ptr_database, const _sys_system_id id, const int bound_sz, const string break_sz, const int offset, const int number_row )", "Invalid database request", "Check the database", 2, false);
		ostringstream info;
		info << "Table Name      : " << Dam_CI_Polygon::polygon_erg_table->get_table_name() << endl;
		info << "Table error info: " << results->lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}

	return results->rowCount();
}
//Count data in the database table for the results of the damage calculation specified by the system id and the scenario-ids (static)
int Dam_CI_Polygon::count_data_in_erg_table(QSqlQueryModel *results, QSqlDatabase *ptr_database, const _sys_system_id id, const int bound_sz, const string break_sz) {
	//the table is set (the name and the column names) and allocated
	try {
		Dam_CI_Polygon::set_erg_table(ptr_database);
	}
	catch (Error msg) {
		throw msg;
	}

	int number = 0;
	ostringstream query_string;
	query_string << "SELECT COUNT(";
	query_string << Dam_CI_Polygon::polygon_erg_table->get_column_name(dam_label::polygon_id) << " ) ";

	query_string << " FROM " << Dam_CI_Polygon::polygon_erg_table->get_table_name();
	query_string << " WHERE ";
	query_string << Dam_CI_Polygon::polygon_erg_table->get_column_name(label::applied_flag) << "= true";
	query_string << " AND ";
	query_string << Dam_CI_Polygon::polygon_erg_table->get_column_name(label::areastate_id) << " =" << id.area_state;
	query_string << " AND (";
	query_string << Dam_CI_Polygon::polygon_erg_table->get_column_name(label::measure_id) << " = " << 0;
	query_string << " OR ";
	query_string << Dam_CI_Polygon::polygon_erg_table->get_column_name(label::measure_id) << " = " << id.measure_nr;
	query_string << " ) ";
	query_string << " AND ";
	query_string << Dam_CI_Polygon::polygon_erg_table->get_column_name(hyd_label::sz_bound_id) << " = " << bound_sz;
	query_string << " AND ";
	query_string << Dam_CI_Polygon::polygon_erg_table->get_column_name(risk_label::sz_break_id) << " = '" << break_sz << "'";

	Data_Base::database_request(results, query_string.str(), ptr_database);
	if (results->lastError().isValid() == true) {
		Error msg;
		msg.set_msg("Dam_CI_Polygon::count_data_in_erg_table(QSqlDatabase *ptr_database, const _sys_system_id id, const int bound_sz, const string break_sz)", "Invalid database request", "Check the database", 2, false);
		ostringstream info;
		info << "Table Name      : " << Dam_CI_Polygon::polygon_erg_table->get_table_name() << endl;
		info << "Table error info: " << results->lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}

	if (results->rowCount() > 0) {
		number = results->record(0).field(0).value().toInt();
	}

	return number;
}
//Switch the applied-flag for the CI-polygon damage results in the database table for a defined system state (static)
void Dam_CI_Polygon::switch_applied_flag_erg_table(QSqlDatabase *ptr_database, const _sys_system_id id, const bool flag) {
	//the table is set (the name and the column names) and allocated
	try {
		Dam_CI_Polygon::set_erg_table(ptr_database);
	}
	catch (Error msg) {
		throw msg;
	}
	QSqlQueryModel query;

	ostringstream query_string;
	query_string << "UPDATE ";
	query_string << Dam_CI_Polygon::polygon_erg_table->get_table_name();
	query_string << " SET ";
	query_string << Dam_CI_Polygon::polygon_erg_table->get_column_name(label::applied_flag) << " = " << functions::convert_boolean2string(flag);
	query_string << " WHERE ";
	query_string << Dam_CI_Polygon::polygon_erg_table->get_column_name(label::areastate_id) << " = " << id.area_state;
	query_string << " AND ";
	query_string << Dam_CI_Polygon::polygon_erg_table->get_column_name(label::measure_id) << " = " << id.measure_nr;
	query_string << " AND ";
	query_string << Dam_CI_Polygon::polygon_erg_table->get_column_name(label::applied_flag) << " = " << functions::convert_boolean2string(!flag);

	Data_Base::database_request(&query, query_string.str(), ptr_database);
	if (query.lastError().isValid() == true) {
		Error msg;
		msg.set_msg("Dam_CI_Polygon::switch_applied_flag_erg_table(QSqlDatabase *ptr_database, const _sys_system_id id, const bool flag)", "Invalid database request", "Check the database", 2, false);
		ostringstream info;
		info << "Table Name      : " << Dam_CI_Polygon::polygon_erg_table->get_table_name() << endl;
		info << "Table error info: " << query.lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}


	//instat
	//the table is set (the name and the column names) and allocated
	try {
		Dam_CI_Polygon::set_instat_erg_table(ptr_database);
	}
	catch (Error msg) {
		throw msg;
	}
	QSqlQueryModel query1;

	ostringstream query_string1;
	query_string1 << "UPDATE ";
	query_string1 << Dam_CI_Polygon::polygon_instat_erg_table->get_table_name();
	query_string1 << " SET ";
	query_string1 << Dam_CI_Polygon::polygon_instat_erg_table->get_column_name(label::applied_flag) << " = " << functions::convert_boolean2string(flag);
	query_string1 << " WHERE ";
	query_string1 << Dam_CI_Polygon::polygon_instat_erg_table->get_column_name(label::areastate_id) << " = " << id.area_state;
	query_string1 << " AND ";
	query_string1 << Dam_CI_Polygon::polygon_instat_erg_table->get_column_name(label::measure_id) << " = " << id.measure_nr;
	query_string1 << " AND ";
	query_string1 << Dam_CI_Polygon::polygon_instat_erg_table->get_column_name(label::applied_flag) << " = " << functions::convert_boolean2string(!flag);

	Data_Base::database_request(&query1, query_string1.str(), ptr_database);
	if (query1.lastError().isValid() == true) {
		Error msg;
		msg.set_msg("Dam_CI_Polygon::switch_applied_flag_erg_table(QSqlDatabase *ptr_database, const _sys_system_id id, const bool flag)", "Invalid database request", "Check the database", 2, false);
		ostringstream info;
		info << "Table Name      : " << Dam_CI_Polygon::polygon_instat_erg_table->get_table_name() << endl;
		info << "Table error info: " << query1.lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}

}
//Switch the applied-flag for the CI-polygon damage results in the database table for a defined system state (static)
void Dam_CI_Polygon::switch_applied_flag_erg_table(QSqlDatabase *ptr_database, const _sys_system_id id, const int hyd_sc, const bool flag) {
	//the table is set (the name and the column names) and allocated
	try {
		Dam_CI_Polygon::set_erg_table(ptr_database);
	}
	catch (Error msg) {
		throw msg;
	}
	QSqlQueryModel query;

	ostringstream query_string;
	query_string << "UPDATE ";
	query_string << Dam_CI_Polygon::polygon_erg_table->get_table_name();
	query_string << " SET ";
	query_string << Dam_CI_Polygon::polygon_erg_table->get_column_name(label::applied_flag) << " = " << functions::convert_boolean2string(flag);
	query_string << " WHERE ";
	query_string << Dam_CI_Polygon::polygon_erg_table->get_column_name(label::areastate_id) << " = " << id.area_state;
	query_string << " AND ";
	query_string << Dam_CI_Polygon::polygon_erg_table->get_column_name(label::measure_id) << " = " << id.measure_nr;
	query_string << " AND ";
	query_string << Dam_CI_Polygon::polygon_erg_table->get_column_name(hyd_label::sz_bound_id) << " = " << hyd_sc;
	query_string << " AND ";
	query_string << Dam_CI_Polygon::polygon_erg_table->get_column_name(label::applied_flag) << " = " << functions::convert_boolean2string(!flag);

	Data_Base::database_request(&query, query_string.str(), ptr_database);
	if (query.lastError().isValid() == true) {
		Error msg;
		msg.set_msg("Dam_CI_Polygon::switch_applied_flag_erg_table(QSqlDatabase *ptr_database, const _sys_system_id id, const bool flag)", "Invalid database request", "Check the database", 2, false);
		ostringstream info;
		info << "Table Name      : " << Dam_CI_Polygon::polygon_erg_table->get_table_name() << endl;
		info << "Table error info: " << query.lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}

	//instat
	//the table is set (the name and the column names) and allocated
	try {
		Dam_CI_Polygon::set_instat_erg_table(ptr_database);
	}
	catch (Error msg) {
		throw msg;
	}
	QSqlQueryModel query1;

	ostringstream query_string1;
	query_string1 << "UPDATE ";
	query_string1 << Dam_CI_Polygon::polygon_instat_erg_table->get_table_name();
	query_string1 << " SET ";
	query_string1 << Dam_CI_Polygon::polygon_instat_erg_table->get_column_name(label::applied_flag) << " = " << functions::convert_boolean2string(flag);
	query_string1 << " WHERE ";
	query_string1 << Dam_CI_Polygon::polygon_instat_erg_table->get_column_name(label::areastate_id) << " = " << id.area_state;
	query_string1 << " AND ";
	query_string1 << Dam_CI_Polygon::polygon_instat_erg_table->get_column_name(label::measure_id) << " = " << id.measure_nr;
	query_string1 << " AND ";
	query_string1 << Dam_CI_Polygon::polygon_instat_erg_table->get_column_name(hyd_label::sz_bound_id) << " = " << hyd_sc;
	query_string1 << " AND ";
	query_string1 << Dam_CI_Polygon::polygon_instat_erg_table->get_column_name(label::applied_flag) << " = " << functions::convert_boolean2string(!flag);

	Data_Base::database_request(&query1, query_string1.str(), ptr_database);
	if (query1.lastError().isValid() == true) {
		Error msg;
		msg.set_msg("Dam_CI_Polygon::switch_applied_flag_erg_table(QSqlDatabase *ptr_database, const _sys_system_id id, const bool flag)", "Invalid database request", "Check the database", 2, false);
		ostringstream info;
		info << "Table Name      : " << Dam_CI_Polygon::polygon_instat_erg_table->get_table_name() << endl;
		info << "Table error info: " << query1.lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
}
//Switch the applied-flag for the CI-polygon in the database table for a defined system state (static)
void Dam_CI_Polygon::switch_applied_flag_polygon_table(QSqlDatabase *ptr_database, const _sys_system_id id, const bool flag) {
	try {
		Dam_CI_Polygon::set_polygon_table(ptr_database);
	}
	catch (Error msg) {
		throw msg;
	}
	QSqlQueryModel query;

	ostringstream query_string;
	query_string << "UPDATE ";
	query_string << Dam_CI_Polygon::polygon_table->get_table_name();
	query_string << " SET ";
	query_string << Dam_CI_Polygon::polygon_table->get_column_name(label::applied_flag) << " = " << functions::convert_boolean2string(flag);
	query_string << " WHERE ";
	query_string << Dam_CI_Polygon::polygon_table->get_column_name(label::areastate_id) << " = " << id.area_state;
	query_string << " AND ";
	query_string << Dam_CI_Polygon::polygon_table->get_column_name(label::measure_id) << " = " << id.measure_nr;
	query_string << " AND ";
	query_string << Dam_CI_Polygon::polygon_table->get_column_name(label::applied_flag) << " = " << functions::convert_boolean2string(!flag);

	Data_Base::database_request(&query, query_string.str(), ptr_database);
	if (query.lastError().isValid() == true) {
		Error msg;
		msg.set_msg("Dam_CI_Polygon::switch_applied_flag_point_table(QSqlDatabase *ptr_database, const _sys_system_id id, const bool flag)", "Invalid database request", "Check the database", 2, false);
		ostringstream info;
		info << "Table Name      : " << Dam_CI_Polygon::polygon_table->get_table_name() << endl;
		info << "Table error info: " << query.lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}


}
//Switch the applied-flag for the CI-polygons in the database table for a defined system state (static)
void Dam_CI_Polygon::switch_applied_flag_polygon_table(QSqlDatabase *ptr_database, const _sys_system_id id, const int polygon_id, const bool flag) {
	try {
		Dam_CI_Polygon::set_polygon_table(ptr_database);
	}
	catch (Error msg) {
		throw msg;
	}
	QSqlQueryModel query;

	ostringstream query_string;
	query_string << "UPDATE ";
	query_string << Dam_CI_Polygon::polygon_table->get_table_name();
	query_string << " SET ";
	query_string << Dam_CI_Polygon::polygon_table->get_column_name(label::applied_flag) << " = " << functions::convert_boolean2string(flag);
	query_string << " WHERE ";
	query_string << Dam_CI_Polygon::polygon_table->get_column_name(label::areastate_id) << " = " << id.area_state;
	query_string << " AND ";
	query_string << Dam_CI_Polygon::polygon_table->get_column_name(label::measure_id) << " = " << id.measure_nr;
	query_string << " AND ";
	query_string << Dam_CI_Polygon::polygon_table->get_column_name(dam_label::polygon_id) << " = " << polygon_id;
	query_string << " AND ";
	query_string << Dam_CI_Polygon::polygon_table->get_column_name(label::applied_flag) << " = " << functions::convert_boolean2string(!flag);

	Data_Base::database_request(&query, query_string.str(), ptr_database);
	if (query.lastError().isValid() == true) {
		Error msg;
		msg.set_msg("Dam_CI_Polygon::switch_applied_flag_point_table(QSqlDatabase *ptr_database, const _sys_system_id id, const bool flag)", "Invalid database request", "Check the database", 2, false);
		ostringstream info;
		info << "Table Name      : " << Dam_CI_Polygon::polygon_table->get_table_name() << endl;
		info << "Table error info: " << query.lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}

	
}
//Reswitch the applied-flag for the CI-polygons in the database table for the base state; a defined system state and his applied elements are decisive (static)
void Dam_CI_Polygon::reswitch_applied_flag_polygon_table(QSqlDatabase *ptr_database, const _sys_system_id id, const bool flag) {
	int number = 0;
	try {
		Dam_CI_Polygon::set_polygon_table(ptr_database);
	}
	catch (Error msg) {
		throw msg;
	}

	QSqlQueryModel results;
	ostringstream test_filter;
	test_filter << "Select ";
	test_filter << Dam_CI_Polygon::polygon_table->get_column_name(dam_label::polygon_id) << " , ";

	test_filter << " from " << Dam_CI_Polygon::polygon_table->get_table_name();
	test_filter << " where ";
	test_filter << Dam_CI_Polygon::polygon_table->get_column_name(label::applied_flag) << "= true";
	test_filter << " and ";
	test_filter << Dam_CI_Polygon::polygon_table->get_column_name(label::areastate_id) << " =" << id.area_state;
	test_filter << " and ";
	test_filter << Dam_CI_Polygon::polygon_table->get_column_name(label::measure_id) << " = " << id.measure_nr;

	Data_Base::database_request(&results, test_filter.str(), ptr_database);

	if (results.lastError().isValid() == true) {
		Error msg;
		msg.set_msg("Dam_CI_Polygon::reswitch_applied_flag_polygon_table(QSqlDatabase *ptr_database, const _sys_system_id id, const bool flag)", "Invalid database request", "Check the database", 2, false);
		ostringstream info;
		info << "Table Name      : " << Dam_CI_Polygon::polygon_table->get_table_name() << endl;
		info << "Table error info: " << results.lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}

	
	number = results.rowCount();
	int polygon_id = 0;
	_sys_system_id base;
	base.area_state = id.area_state;
	base.measure_nr = 0;

	for (int i = 0; i < number; i++) {
		polygon_id = results.record(i).value((Dam_CI_Polygon::polygon_table->get_column_name(dam_label::polygon_id)).c_str()).toInt();

		Dam_CI_Polygon::switch_applied_flag_polygon_table(ptr_database, base, polygon_id, flag);
	}


}
//Copy the results of a given system id to another one in database tables (static)
void Dam_CI_Polygon::copy_results(QSqlDatabase *ptr_database, const _sys_system_id src, const _sys_system_id dest) {
	//the table is set (the name and the column names) and allocated
	try {
		Dam_CI_Polygon::set_erg_table(ptr_database);
	}
	catch (Error msg) {
		throw msg;
	}

	int glob_id = 0;
	glob_id = Dam_CI_Polygon::polygon_erg_table->maximum_int_of_column(Dam_CI_Polygon::polygon_erg_table->get_column_name(dam_label::glob_id), ptr_database) + 1;

	QSqlQueryModel model;
	ostringstream test_filter;
	test_filter << "SELECT ";
	test_filter << Dam_CI_Polygon::polygon_erg_table->get_column_name(dam_label::glob_id);
	test_filter << " from " << Dam_CI_Polygon::polygon_erg_table->get_table_name();
	test_filter << " WHERE ";
	test_filter << Dam_CI_Polygon::polygon_erg_table->get_column_name(label::areastate_id) << " = " << src.area_state;
	test_filter << " AND ";
	test_filter << Dam_CI_Polygon::polygon_erg_table->get_column_name(label::measure_id) << " = " << src.measure_nr;

	//submit it to the datbase
	Data_Base::database_request(&model, test_filter.str(), ptr_database);
	if (model.lastError().isValid() == true) {
		Error msg;
		msg.set_msg("Dam_CI_Polygon::copy_results(QSqlDatabase *ptr_database, const _sys_system_id src, const _sys_system_id dest)", "Invalid database request", "Check the database", 2, false);
		ostringstream info;
		info << "Table Name      : " << Dam_CI_Polygon::polygon_erg_table->get_table_name() << endl;
		info << "Table error info: " << model.lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
	test_filter.str("");

	ostringstream cout;
	cout << "Copy " << model.rowCount() << " polygon results of the CI-system to the new measure state..." << endl;
	Sys_Common_Output::output_dam->output_txt(&cout);

	QSqlQueryModel model1;
	for (int i = 0; i < model.rowCount(); i++) {
		if (i % 10000 == 0 && i > 0) {
			cout << "Copy SC-polygon results " << i << " to " << i + 10000 << "..." << endl;
			Sys_Common_Output::output_dam->output_txt(&cout);
		}
		test_filter.str("");
		test_filter << "INSERT INTO " << Dam_CI_Polygon::polygon_erg_table->get_table_name();
		test_filter << " SELECT " << glob_id << " , ";
		test_filter << Dam_CI_Polygon::polygon_erg_table->get_column_name(dam_label::polygon_id) << " , ";
		test_filter << dest.area_state << " , ";
		test_filter << dest.measure_nr << " , ";
		test_filter << Dam_CI_Polygon::polygon_erg_table->get_column_name(label::applied_flag) << " , ";
		test_filter << Dam_CI_Polygon::polygon_erg_table->get_column_name(hyd_label::sz_bound_id) << " , ";
		test_filter << Dam_CI_Polygon::polygon_erg_table->get_column_name(risk_label::sz_break_id) << " , ";


		test_filter << Dam_CI_Polygon::polygon_erg_table->get_column_name(dam_label::sector_id) << " , ";
		test_filter << Dam_CI_Polygon::polygon_erg_table->get_column_name(dam_label::enduser_duration) << " , ";
		test_filter << Dam_CI_Polygon::polygon_erg_table->get_column_name(dam_label::failure_duration) << " , ";
		test_filter << Dam_CI_Polygon::polygon_erg_table->get_column_name(dam_label::end_user) << " , ";
		test_filter << Dam_CI_Polygon::polygon_erg_table->get_column_name(dam_label::failure_duration) << " , ";




		test_filter << Dam_CI_Polygon::polygon_erg_table->get_column_name(hyd_label::polygon_out) << "  ";

		test_filter << " FROM " << Dam_CI_Polygon::polygon_erg_table->get_table_name() << " ";
		test_filter << " WHERE " << Dam_CI_Polygon::polygon_erg_table->get_column_name(dam_label::glob_id) << " = ";
		test_filter << model.record(i).value(Dam_CI_Polygon::polygon_erg_table->get_column_name(dam_label::glob_id).c_str()).toInt();
		Data_Base::database_request(&model1, test_filter.str(), ptr_database);
		if (model1.lastError().isValid() == true) {
			Error msg;
			msg.set_msg("Dam_CI_Polygon::copy_results(QSqlDatabase *ptr_database, const _sys_system_id src, const _sys_system_id dest)", "Invalid database request", "Check the database", 2, false);
			ostringstream info;
			info << "Table Name      : " << Dam_CI_Polygon::polygon_erg_table->get_table_name() << endl;
			info << "Table error info: " << model1.lastError().text().toStdString() << endl;
			msg.make_second_info(info.str());
			throw msg;
		}
		glob_id++;
	}
}
//Create the database table for the instationary results of the damage calculation per CI-polygon (static)
void Dam_CI_Polygon::create_instat_erg_table(QSqlDatabase *ptr_database) {
	if (Dam_CI_Polygon::polygon_instat_erg_table == NULL) {
		ostringstream cout;
		cout << "Create CI-polygon instationary result database table..." << endl;
		Sys_Common_Output::output_dam->output_txt(&cout);
		//make specific input for this class
		const string tab_name = dam_label::tab_ci_polygon_instat_erg;
		const int num_col = 13;
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

		tab_col[1].name = dam_label::polygon_id;
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

		tab_col[7].name = dam_label::sector_id;
		tab_col[7].type = sys_label::tab_col_type_int;
		tab_col[7].default_value = "-1";

		tab_col[8].name = dam_label::active_flag;
		tab_col[8].type = sys_label::tab_col_type_bool;
		tab_col[8].default_value = "true";

		tab_col[9].name = dam_label::failure_type;
		tab_col[9].type = sys_label::tab_col_type_string;

		tab_col[10].name = dam_label::failure_duration;
		tab_col[10].type = sys_label::tab_col_type_double;
		tab_col[10].default_value = "0.0";

		tab_col[11].name = hyd_label::data_time;
		tab_col[11].type = sys_label::tab_col_type_string;
		tab_col[11].default_value = "";

		tab_col[12].name = hyd_label::polygon_out;
		tab_col[12].type = sys_label::tab_col_type_polygon;

		try {
			Dam_CI_Polygon::polygon_instat_erg_table = new Tables();
			if (Dam_CI_Polygon::polygon_instat_erg_table->create_non_existing_tables(tab_name, tab_col, num_col, ptr_database, _sys_table_type::dam) == false) {
				cout << " Table exists" << endl;
				Sys_Common_Output::output_dam->output_txt(&cout);
			};
		}
		catch (bad_alloc& t) {
			Error msg;
			msg.set_msg("Dam_CI_Polygon::create_instat_erg_table(QSqlDatabase *ptr_database)", "Can not allocate the memory", "Check the memory", 10, false);
			ostringstream info;
			info << "Info bad alloc: " << t.what() << endl;
			msg.make_second_info(info.str());
			throw msg;
		}
		catch (Error msg) {
			Dam_CI_Polygon::close_instat_erg_table();
			throw msg;
		}

		Dam_CI_Polygon::close_instat_erg_table();
	}


}
//Set the database table for the instationary results of the damage calculation per CI-polygon: it sets the table name and the name of the columns and allocate them (static)
void Dam_CI_Polygon::set_instat_erg_table(QSqlDatabase *ptr_database, const bool not_close) {
	if (Dam_CI_Polygon::polygon_instat_erg_table == NULL) {
		//make specific input for this class
		const string tab_id_name = dam_label::tab_ci_polygon_instat_erg;
		string tab_col[13];

		tab_col[0] = dam_label::glob_id;
		tab_col[1] = dam_label::polygon_id;
		tab_col[2] = label::areastate_id;
		tab_col[3] = label::measure_id;
		tab_col[4] = label::applied_flag;
		tab_col[5] = hyd_label::sz_bound_id;
		tab_col[6] = risk_label::sz_break_id;
		tab_col[7] = dam_label::sector_id;
		tab_col[8] = dam_label::active_flag;
		tab_col[9]= hyd_label::polygon_out;
		tab_col[10] = hyd_label::data_time;
		tab_col[11] = dam_label::failure_type;
		tab_col[12] = dam_label::failure_duration;

		try {
			Dam_CI_Polygon::polygon_instat_erg_table = new Tables(tab_id_name, tab_col, sizeof(tab_col) / sizeof(tab_col[0]));
			Dam_CI_Polygon::polygon_instat_erg_table->set_name(ptr_database, _sys_table_type::dam);
		}
		catch (bad_alloc& t) {
			Error msg;
			msg.set_msg("Dam_CI_Polygon::set_instat_erg_table(QSqlDatabase *ptr_database)", "Can not allocate the memory", "Check the memory", 10, false);
			ostringstream info;
			info << "Info bad alloc: " << t.what() << endl;
			msg.make_second_info(info.str());
			throw msg;
		}
		catch (Error msg) {
			if (not_close == false) {
				Dam_CI_Polygon::close_instat_erg_table();
			}
			throw msg;
		}
	}
}
//Delete all data in the database table for the instationary results of the damage calculation per CI-polygon (static)
void Dam_CI_Polygon::delete_data_in_instat_erg_table(QSqlDatabase *ptr_database) {
	//the table is set (the name and the column names) and allocated
	try {
		Dam_CI_Polygon::set_instat_erg_table(ptr_database);
	}
	catch (Error msg) {
		throw msg;
	}
	//delete the table
	Dam_CI_Polygon::polygon_instat_erg_table->delete_data_in_table(ptr_database);
}
//Delete the data in the database table for the instationary results of the damage calculation specified by the system state (static)
void Dam_CI_Polygon::delete_data_in_instat_erg_table(QSqlDatabase *ptr_database, const _sys_system_id id) {
	//the table is set (the name and the column names) and allocated
	try {
		Dam_CI_Polygon::set_instat_erg_table(ptr_database);
	}
	catch (Error msg) {
		throw msg;
	}

	//delete the table
	QSqlQuery query(*ptr_database);
	ostringstream query_string;
	query_string << "DELETE FROM " << Dam_CI_Polygon::polygon_instat_erg_table->get_table_name();
	query_string << " WHERE ";
	query_string << Dam_CI_Polygon::polygon_instat_erg_table->get_column_name(label::areastate_id) << " = " << id.area_state;
	query_string << " AND ";
	query_string << Dam_CI_Polygon::polygon_instat_erg_table->get_column_name(label::measure_id) << " = " << id.measure_nr;

	Data_Base::database_request(&query, query_string.str(), ptr_database);
	if (query.lastError().isValid() == true) {
		Error msg;
		msg.set_msg("Dam_CI_Polygon::delete_data_in_instat_erg_table(QSqlDatabase *ptr_database, const _sys_system_id id)", "Invalid database request", "Check the database", 2, false);
		ostringstream info;
		info << "Table Name      : " << Dam_CI_Polygon::polygon_instat_erg_table->get_table_name() << endl;
		info << "Table error info: " << query.lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
}
//Delete the data in the database table for the instationary results of the damage calculation specified parameters (static)
void Dam_CI_Polygon::delete_data_in_instat_erg_table(QSqlDatabase *ptr_database, const _sys_system_id id, const string break_sz, const bool like_flag) {
	//the table is set (the name and the column names) and allocated
	try {
		Dam_CI_Polygon::set_instat_erg_table(ptr_database);
	}
	catch (Error msg) {
		throw msg;
	}

	//delete the table
	QSqlQuery query(*ptr_database);
	ostringstream query_string;
	query_string << "DELETE FROM " << Dam_CI_Polygon::polygon_instat_erg_table->get_table_name();
	query_string << " WHERE ";
	query_string << Dam_CI_Polygon::polygon_instat_erg_table->get_column_name(label::areastate_id) << " = " << id.area_state;
	query_string << " AND ";
	query_string << Dam_CI_Polygon::polygon_instat_erg_table->get_column_name(label::measure_id) << " = " << id.measure_nr;
	query_string << " AND ";
	if (like_flag == false) {
		query_string << Dam_CI_Polygon::polygon_instat_erg_table->get_column_name(risk_label::sz_break_id) << " = '" << break_sz << "'";
	}
	else {
		query_string << Dam_CI_Polygon::polygon_instat_erg_table->get_column_name(risk_label::sz_break_id) << " LIKE '" << break_sz << "'";
	}

	Data_Base::database_request(&query, query_string.str(), ptr_database);
	if (query.lastError().isValid() == true) {
		Error msg;
		msg.set_msg("Dam_CI_Polygon::delete_data_in_instat_erg_table(QSqlDatabase *ptr_database, const _sys_system_id id, const string break_sz, const bool like_flag)", "Invalid database request", "Check the database", 2, false);
		ostringstream info;
		info << "Table Name      : " << Dam_CI_Polygon::polygon_instat_erg_table->get_table_name() << endl;
		info << "Table error info: " << query.lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
}
//Delete the data in the database table for the instationary results of the damage calculation specified by the system state and the scenario-ids (static)
void Dam_CI_Polygon::delete_data_in_instat_erg_table(QSqlDatabase *ptr_database, const _sys_system_id id, const int bound_sz, const string break_sz) {
	//the table is set (the name and the column names) and allocated
	try {
		Dam_CI_Polygon::set_instat_erg_table(ptr_database);
	}
	catch (Error msg) {
		throw msg;
	}

	//delete the table
	QSqlQuery query(*ptr_database);
	ostringstream query_string;
	query_string << "DELETE FROM " << Dam_CI_Polygon::polygon_instat_erg_table->get_table_name();
	query_string << " WHERE ";
	query_string << Dam_CI_Polygon::polygon_instat_erg_table->get_column_name(label::areastate_id) << " = " << id.area_state;
	query_string << " AND ";
	query_string << Dam_CI_Polygon::polygon_instat_erg_table->get_column_name(label::measure_id) << " = " << id.measure_nr;
	query_string << " AND ";
	query_string << Dam_CI_Polygon::polygon_instat_erg_table->get_column_name(hyd_label::sz_bound_id) << " = " << bound_sz;
	query_string << " AND ";
	query_string << Dam_CI_Polygon::polygon_instat_erg_table->get_column_name(risk_label::sz_break_id) << " = '" << break_sz << "'";

	Data_Base::database_request(&query, query_string.str(), ptr_database);
	if (query.lastError().isValid() == true) {
		Error msg;
		msg.set_msg("Dam_CI_Polygon::delete_data_in_instat_erg_table(QSqlDatabase *ptr_database, const _sys_system_id id, const int bound_sz, const string break_sz )", "Invalid database request", "Check the database", 2, false);
		ostringstream info;
		info << "Table Name      : " << Dam_CI_Polygon::polygon_instat_erg_table->get_table_name() << endl;
		info << "Table error info: " << query.lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
}
//Delete the data in the database table for the instationary results of the damage calculation specified by the hydraulic scenario-ids (static)
void Dam_CI_Polygon::delete_data_in_instat_erg_table(QSqlDatabase *ptr_database, const int bound_sz) {
	//the table is set (the name and the column names) and allocated
	try {
		Dam_CI_Polygon::set_instat_erg_table(ptr_database);
	}
	catch (Error msg) {
		throw msg;
	}

	//delete the table
	QSqlQuery query(*ptr_database);
	ostringstream query_string;
	query_string << "DELETE FROM " << Dam_CI_Polygon::polygon_instat_erg_table->get_table_name();
	query_string << " WHERE ";
	query_string << Dam_CI_Polygon::polygon_instat_erg_table->get_column_name(hyd_label::sz_bound_id) << " = " << bound_sz;

	Data_Base::database_request(&query, query_string.str(), ptr_database);
	if (query.lastError().isValid() == true) {
		Error msg;
		msg.set_msg("Dam_CI_Polygon::delete_data_in_instat_erg_table(QSqlDatabase *ptr_database, const int bound_sz)", "Invalid database request", "Check the database", 2, false);
		ostringstream info;
		info << "Table Name      : " << Dam_CI_Polygon::polygon_instat_erg_table->get_table_name() << endl;
		info << "Table error info: " << query.lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
}
//Close and delete the database table for the instationary results of the damage calculation per CI-polygon (static)
void Dam_CI_Polygon::close_instat_erg_table(void) {
	if (Dam_CI_Polygon::polygon_instat_erg_table != NULL) {
		delete Dam_CI_Polygon::polygon_instat_erg_table;
		Dam_CI_Polygon::polygon_instat_erg_table = NULL;
	}
}
//Get the maximum value of the global index of the instationary result CI-polygon database table (static)
int Dam_CI_Polygon::get_max_glob_id_polygon_instat_erg_table(QSqlDatabase *ptr_database) {
	try {
		Dam_CI_Polygon::set_instat_erg_table(ptr_database);
	}
	catch (Error msg) {
		throw msg;
	}

	//evaluate the global identifier
	int id_glob = Dam_CI_Polygon::polygon_instat_erg_table->maximum_int_of_column(Dam_CI_Polygon::polygon_instat_erg_table->get_column_name(dam_label::glob_id), ptr_database);
	return id_glob;
}
//Get the header for inserting the CI-polygon instationary result data to database table (static)
string Dam_CI_Polygon::get_insert_header_instat_erg_table(QSqlDatabase *ptr_database) {
	try {
		Dam_CI_Polygon::set_instat_erg_table(ptr_database);
	}
	catch (Error msg) {
		throw msg;
	}
	//set the query via a query string
	ostringstream query_string;
	query_string << "INSERT INTO  " << Dam_CI_Polygon::polygon_instat_erg_table->get_table_name();
	query_string << " ( ";
	query_string << Dam_CI_Polygon::polygon_instat_erg_table->get_column_name(dam_label::glob_id) << " , ";
	query_string << Dam_CI_Polygon::polygon_instat_erg_table->get_column_name(dam_label::polygon_id) << " , ";
	query_string << Dam_CI_Polygon::polygon_instat_erg_table->get_column_name(label::areastate_id) << " , ";
	query_string << Dam_CI_Polygon::polygon_instat_erg_table->get_column_name(label::measure_id) << " , ";
	query_string << Dam_CI_Polygon::polygon_instat_erg_table->get_column_name(hyd_label::sz_bound_id) << " , ";
	query_string << Dam_CI_Polygon::polygon_instat_erg_table->get_column_name(risk_label::sz_break_id) << " , ";
	//results
	query_string << Dam_CI_Polygon::polygon_instat_erg_table->get_column_name(dam_label::sector_id) << " , ";
	query_string << Dam_CI_Polygon::polygon_instat_erg_table->get_column_name(dam_label::active_flag) << " , ";
	query_string << Dam_CI_Polygon::polygon_instat_erg_table->get_column_name(dam_label::failure_type) << " , ";
	query_string << Dam_CI_Polygon::polygon_instat_erg_table->get_column_name(dam_label::failure_duration) << " , ";
	query_string << Dam_CI_Polygon::polygon_instat_erg_table->get_column_name(hyd_label::data_time) << " , ";

	query_string << Dam_CI_Polygon::polygon_instat_erg_table->get_column_name(hyd_label::polygon_out) << ")";



	query_string << " VALUES ";
	string buffer;
	buffer = query_string.str();
	return buffer;
}
//Copy the instationaryresults of a given system id to another one in database tables per CI-polygon (static) 
void Dam_CI_Polygon::copy_instat_results(QSqlDatabase *ptr_database, const _sys_system_id src, const _sys_system_id dest) {
	//the table is set (the name and the column names) and allocated
	try {
		Dam_CI_Polygon::set_instat_erg_table(ptr_database);
	}
	catch (Error msg) {
		throw msg;
	}

	int glob_id = 0;
	glob_id = Dam_CI_Polygon::polygon_instat_erg_table->maximum_int_of_column(Dam_CI_Polygon::polygon_instat_erg_table->get_column_name(dam_label::glob_id), ptr_database) + 1;

	QSqlQueryModel model;
	ostringstream test_filter;
	test_filter << "SELECT ";
	test_filter << Dam_CI_Polygon::polygon_instat_erg_table->get_column_name(dam_label::glob_id);
	test_filter << " from " << Dam_CI_Polygon::polygon_instat_erg_table->get_table_name();
	test_filter << " WHERE ";
	test_filter << Dam_CI_Polygon::polygon_instat_erg_table->get_column_name(label::areastate_id) << " = " << src.area_state;
	test_filter << " AND ";
	test_filter << Dam_CI_Polygon::polygon_instat_erg_table->get_column_name(label::measure_id) << " = " << src.measure_nr;

	//submit it to the datbase
	Data_Base::database_request(&model, test_filter.str(), ptr_database);
	if (model.lastError().isValid() == true) {
		Error msg;
		msg.set_msg("Dam_CI_Polygon::copy_instat_results(QSqlDatabase *ptr_database, const _sys_system_id src, const _sys_system_id dest)", "Invalid database request", "Check the database", 2, false);
		ostringstream info;
		info << "Table Name      : " << Dam_CI_Polygon::polygon_instat_erg_table->get_table_name() << endl;
		info << "Table error info: " << model.lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
	test_filter.str("");

	ostringstream cout;
	cout << "Copy " << model.rowCount() << " instationary polygon results of the CI-system to the new measure state..." << endl;
	Sys_Common_Output::output_dam->output_txt(&cout);

	QSqlQueryModel model1;
	for (int i = 0; i < model.rowCount(); i++) {
		if (i % 10000 == 0 && i > 0) {
			cout << "Copy SC-polygon results " << i << " to " << i + 10000 << "..." << endl;
			Sys_Common_Output::output_dam->output_txt(&cout);
		}
		test_filter.str("");
		test_filter << "INSERT INTO " << Dam_CI_Polygon::polygon_instat_erg_table->get_table_name();
		test_filter << " SELECT " << glob_id << " , ";
		test_filter << Dam_CI_Polygon::polygon_instat_erg_table->get_column_name(dam_label::polygon_id) << " , ";
		test_filter << dest.area_state << " , ";
		test_filter << dest.measure_nr << " , ";
		test_filter << Dam_CI_Polygon::polygon_instat_erg_table->get_column_name(label::applied_flag) << " , ";
		test_filter << Dam_CI_Polygon::polygon_instat_erg_table->get_column_name(hyd_label::sz_bound_id) << " , ";
		test_filter << Dam_CI_Polygon::polygon_instat_erg_table->get_column_name(risk_label::sz_break_id) << " , ";

		test_filter << Dam_CI_Polygon::polygon_instat_erg_table->get_column_name(dam_label::sector_id) << " , ";
		test_filter << Dam_CI_Polygon::polygon_instat_erg_table->get_column_name(dam_label::active_flag) << " , ";
		test_filter << Dam_CI_Polygon::polygon_instat_erg_table->get_column_name(dam_label::failure_type) << " , ";
		test_filter << Dam_CI_Polygon::polygon_instat_erg_table->get_column_name(dam_label::failure_duration) << " , ";
		test_filter << Dam_CI_Polygon::polygon_instat_erg_table->get_column_name(hyd_label::data_time) << " , ";


		test_filter << Dam_CI_Polygon::polygon_instat_erg_table->get_column_name(hyd_label::polygon_out) << "  ";

		test_filter << " FROM " << Dam_CI_Polygon::polygon_instat_erg_table->get_table_name() << " ";
		test_filter << " WHERE " << Dam_CI_Polygon::polygon_instat_erg_table->get_column_name(dam_label::glob_id) << " = ";
		test_filter << model.record(i).value(Dam_CI_Polygon::polygon_instat_erg_table->get_column_name(dam_label::glob_id).c_str()).toInt();
		Data_Base::database_request(&model1, test_filter.str(), ptr_database);
		if (model1.lastError().isValid() == true) {
			Error msg;
			msg.set_msg("Dam_CI_Polygon::copy_instat_results(QSqlDatabase *ptr_database, const _sys_system_id src, const _sys_system_id dest)", "Invalid database request", "Check the database", 2, false);
			ostringstream info;
			info << "Table Name      : " << Dam_CI_Polygon::polygon_instat_erg_table->get_table_name() << endl;
			info << "Table error info: " << model1.lastError().text().toStdString() << endl;
			msg.make_second_info(info.str());
			throw msg;
		}
		glob_id++;
	}
}
//Set members
void Dam_CI_Polygon::set_members(const int sector_id, const double endusers, const string name, const int id) {
	this->sector_id = sector_id;
	this->sector_name = this->convert_sector_id2txt(this->convert_id2enum(this->sector_id));
	this->enduser = endusers;
	this->mid_point.set_point_name(name);
	this->mid_point.set_number(id);
	this->check_members();
}
///Get endusers
double Dam_CI_Polygon::get_enduser(void) {
	return this->enduser;
}
//Output point members
void Dam_CI_Polygon::output_point(ostringstream *cout) {
	for (int i = 0; i < this->number_segments; i++) {
		*cout << W(3) << P(2) << this->segments[i].point1.get_xcoordinate();
		*cout << W(13) << P(2) << this->segments[i].point1.get_ycoordinate()<<endl;
	}
	
}
//Polygon mid to mid point
void Dam_CI_Polygon::set_polygon_mid2mid_point(void) {
	Geo_Point buffer;
	buffer = this->get_mid_point();
	this->mid_point.set_point_coordinate(buffer.get_xcoordinate(), buffer.get_ycoordinate());

}
//Get the data-string to complete a insert-string for inserting the data of the polygon to database
string Dam_CI_Polygon::get_datastring_members2database(const int global_id) {
	string buffer = label::not_set;
	this->global_index = global_id;


	ostringstream query_string;
	query_string << " ( ";
	query_string << global_id << " , ";
	query_string << this->mid_point.get_number()<< " , ";
	query_string << this->system_id.area_state << " , ";
	query_string << this->system_id.measure_nr << " , ";
	query_string << "true" << " , ";
	//data
	query_string << "'" << this->mid_point.get_point_name() << "' , ";
	query_string << this->sector_id << " , ";
	query_string << "'" << this->sector_name << "' , ";
	query_string << this->enduser<< " , ";
	query_string << FORMAT_FIXED_REAL << P(8);
	query_string << this->mid_point.get_xcoordinate() << " , ";
	query_string << this->mid_point.get_ycoordinate() << " , ";
	query_string.clear();

	query_string << this->get_polygon2sql_string() << " ) ";

	buffer = query_string.str();
	return buffer;
}
//Get the data-string to complete a insert-string for inserting the data of the polygon results to database
string Dam_CI_Polygon::get_datastring_results2database(const int global_id, const int bound_sz, const string break_sz, const bool must_output) {
	string buffer = label::not_set;
	if (must_output == false) {
		if (this->was_affected == false) {
			return buffer;
		}
	}

	//set the query via a query string
	ostringstream query_string;
	query_string << " ( ";
	query_string << global_id << " , ";
	query_string << this->mid_point.get_number() << " , ";
	query_string << this->system_id.area_state << " , ";
	query_string << this->system_id.measure_nr << " , ";
	query_string << bound_sz << " , ";
	query_string << "'" << break_sz << "' , ";
	query_string << this->sector_id << " , ";
	query_string << this->enduser<< " , ";
	query_string << this->enduser_duration << " , ";
	query_string << this->failure_duration << " , ";
	query_string << "'" << this->failure_type << "' , ";

	query_string << "'" << this->polygon_str << "' ) ";

	buffer = query_string.str();
	return buffer;

}
//Get the data-string to complete a insert-string for inserting the data of the polygon instationary results to database
string Dam_CI_Polygon::get_datastring_instat_results2database(const int global_id, const int bound_sz, const string break_sz, string date_time, const bool must_output) {
	string buffer = label::not_set;
	if (must_output == false) {
		if (this->was_affected == false) {
			return buffer;
		}
		if (this->failure_type_enum == _dam_ci_failure_type::no_failure) {
			return buffer;
		}
	}

	//set the query via a query string
	ostringstream query_string;
	query_string << " ( ";
	query_string << global_id << " , ";
	query_string << this->mid_point.get_number() << " , ";
	query_string << this->system_id.area_state << " , ";
	query_string << this->system_id.measure_nr << " , ";
	query_string << bound_sz << " , ";
	query_string << "'" << break_sz << "' , ";
	query_string << this->sector_id << " , ";
	query_string << "'" << functions::convert_boolean2string(this->active_flag) << "' , ";
	query_string << "'" << this->failure_type << "' , ";
	query_string << this->failure_duration << " , ";
	query_string << "'" << date_time << "' , ";

	query_string << "'" << this->polygon_str << "') ";

	buffer = query_string.str();
	return buffer;



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
//Reset result values
void Dam_CI_Polygon::reset_result_values(void) {
	_Dam_CI_Element::reset_result_values();
	this->enduser_duration = 0.0;
}
//Finalize results
void Dam_CI_Polygon::finalize_results(void) {
	
	this->enduser_duration = this->enduser*this->failure_duration;

}
//Check polygon incoming 
void Dam_CI_Polygon::check_polygon_incoming(void) {


	if (this->sector_id < 10) {
		for (int i = 0; i < this->no_incoming; i++) {
			if (this->sector_id != this->incomings[i]->get_sector_id()) {
				Error msg = this->set_error(1);
				ostringstream info;
				info << "Name                  :" << this->mid_point.get_point_name() << endl;
				info << "Sector id             :" << this->sector_id << endl;
				info << "Sector id incoming    :" << this->incomings[i]->get_sector_id() << endl;
				msg.make_second_info(info.str());
				throw msg;

			}

		}

	}


}
//Check the polygons
void Dam_CI_Polygon::check_members(void) {
	//check sec_id
	if (this->sector_name == label::not_defined) {
		Error msg = this->set_error(0);
		ostringstream info;
		info << "Name         :" << this->mid_point.get_point_name() << endl;
		info << "Sector id    :" << this->sector_id << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
	//recovery > 0
	//enduser > 0
	if (this->enduser <= 0.0) {
		Warning msg = this->set_warning(1);
		ostringstream info;
		info << "Name         :" << this->mid_point.get_point_name() << endl;
		info << "Sector id    :" << this->sector_id << endl;
		info << "Sector name  :" << this->sector_name << endl;
		msg.make_second_info(info.str());
		this->enduser = 1.0;
		msg.output_msg(4);
	}



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
void Dam_CI_Polygon::set_point_table(QSqlDatabase *ptr_database, const bool not_close) {
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
			Dam_CI_Polygon::polygon_point_table->set_name(ptr_database, _sys_table_type::dam);
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
			if (not_close == false) {
				Dam_CI_Polygon::close_point_table();
			}
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
		reason = "Can not submit the CI polygon point data to the database";
		reaction = "No data submitted";
		help = "Check the database";
		type = 2;
		break;
	case 1:
		place.append("check_members(void)");
		reason = "The enduser for a CI-structure is <= 0.0 ; it must be > 0.0 ";
		reaction = "The enduser is set to 1.0 ";
		help = "Check CI-polygon data";
		type = 27;
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
		place.append("check_members(void)");
		reason = "The CI-sector is not defined; sectors are available between 1 - 4 and 10 - 19";
		help = "Check CI-polygon data";
		type = 34;
		break;
	case 1://bad alloc
		place.append("check_polygon_incoming(void)");
		reason = "The CI-sector is 1 - 4; for these CI-polygons just incomings of the same sector are allowed ";
		help = "Check CI-polygon data and their connections";
		type = 34;
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
