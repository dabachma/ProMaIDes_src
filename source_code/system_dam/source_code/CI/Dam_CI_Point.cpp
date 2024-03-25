#include "Dam_Headers_Precompiled.h"
//#include "Dam_CI_Point.h"
//init static members
Tables *Dam_CI_Point::point_table = NULL;
Tables *Dam_CI_Point::point_erg_table = NULL;
Tables *Dam_CI_Point::point_instat_erg_table = NULL;

//Default constructor
Dam_CI_Point::Dam_CI_Point(void){



	this->sector_level=-999;
	this->boundary_value=-999;
	this->recovery_time=0;
	this->final_flag=false;
	this->ptr_point = this;
	this->is_point_id = 0;
	

	//count the memory
	Sys_Memory_Count::self()->add_mem(sizeof(Dam_CI_Point), _sys_system_modules::DAM_SYS);
	
}
//Default destructor
Dam_CI_Point::~Dam_CI_Point(void){

	//count the memory
	Sys_Memory_Count::self()->minus_mem(sizeof(Dam_CI_Point), _sys_system_modules::DAM_SYS);
}
//__________
//public
void Dam_CI_Point::input_point_perdatabase(const QSqlQueryModel *results, const int glob_index) {
	try {
		this->number = results->record(glob_index).value((Dam_CI_Point::point_table->get_column_name(dam_label::point_id)).c_str()).toInt();
		//set the element members
		this->sector_id = results->record(glob_index).value((Dam_CI_Point::point_table->get_column_name(dam_label::sector_id)).c_str()).toInt();
		this->sector_level = results->record(glob_index).value((Dam_CI_Point::point_table->get_column_name(dam_label::sector_level)).c_str()).toInt();
		this->sector_name = results->record(glob_index).value((Dam_CI_Point::point_table->get_column_name(dam_label::sector_name)).c_str()).toString().toStdString();
		this->boundary_value = results->record(glob_index).value((Dam_CI_Point::point_table->get_column_name(dam_label::boundary_value)).c_str()).toDouble();
		this->recovery_time = results->record(glob_index).value((Dam_CI_Point::point_table->get_column_name(dam_label::recovery_time)).c_str()).toDouble();
		this->regular_flag = results->record(glob_index).value((Dam_CI_Point::point_table->get_column_name(dam_label::regular_flag)).c_str()).toBool();
		this->activation_time = results->record(glob_index).value((Dam_CI_Point::point_table->get_column_name(dam_label::activation_time)).c_str()).toDouble();
		this->name = results->record(glob_index).value((Dam_CI_Point::point_table->get_column_name(dam_label::point_name)).c_str()).toString().toStdString();
		this->final_flag = results->record(glob_index).value((Dam_CI_Point::point_table->get_column_name(dam_label::final_flag)).c_str()).toBool();
		this->is_connected = results->record(glob_index).value((Dam_CI_Point::point_table->get_column_name(dam_label::raster_connected)).c_str()).toBool();
		this->index_fp = results->record(glob_index).value((Dam_CI_Point::point_table->get_column_name(dam_label::conn_fp_id)).c_str()).toInt();
		this->index_fp_elem = results->record(glob_index).value((Dam_CI_Point::point_table->get_column_name(dam_label::conn_fp_elem_id)).c_str()).toInt();
		this->orig_recovery_time = this->recovery_time;
		this->orig_activation_time = this->activation_time;
		double x_mid = 0.0;
		double y_mid = 0.0;
		x_mid = results->record(glob_index).value((Dam_CI_Point::point_table->get_column_name(dam_label::elem_mid_x)).c_str()).toDouble();
		y_mid = results->record(glob_index).value((Dam_CI_Point::point_table->get_column_name(dam_label::elem_mid_y)).c_str()).toDouble();
		this->set_point_coordinate(x_mid, y_mid);
		this->global_index = results->record(glob_index).value((Dam_CI_Point::point_table->get_column_name(dam_label::glob_id)).c_str()).toInt();

		double buff_cv = -1.0;
		buff_cv = results->record(glob_index).value((Dam_CI_Point::point_table->get_column_name(dam_label::cp_value)).c_str()).toDouble();
		if (buff_cv > 0.0) {
			this->stat_value = buff_cv;
		}
		else {
			buff_cv = results->record(glob_index).value((Dam_CI_Point::point_table->get_column_name(dam_label::cv_value)).c_str()).toDouble();
			this->stat_value = buff_cv;
		}


	}
	catch (Error msg) {
		ostringstream info;
		info << "Point number           : " << this->number << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
}
//Get a string to set the point data from the raster interception to the database table: identifier of the floodplain, -floodplain element
bool Dam_CI_Point::get_string_interception_point_data2database(ostringstream *text) {
	//generate the filter
	//todo
	*text << "UPDATE " << Dam_CI_Point::point_table->get_table_name().c_str();
	*text << " SET ";
	*text << Dam_CI_Point::point_table->get_column_name(dam_label::conn_fp_id).c_str() << " = " << this->index_fp << " , ";
	*text << Dam_CI_Point::point_table->get_column_name(dam_label::conn_fp_elem_id).c_str() << " = " << this->index_fp_elem << " , ";
	*text << Dam_CI_Point::point_table->get_column_name(dam_label::raster_connected).c_str() << " = " << functions::convert_boolean2string(true) << "  ";

	*text << " WHERE ";
	*text << Dam_CI_Point::point_table->get_column_name(dam_label::glob_id) << " = " << this->global_index << "; ";

	return true;
}
//Get a string to set the point data from the statistical calculation to the database table
bool Dam_CI_Point::get_string_stat_point_data2database(ostringstream *text) {
	//generate the filter
	//todo
	*text << "UPDATE " << Dam_CI_Point::point_table->get_table_name().c_str();
	*text << " SET ";
	*text << Dam_CI_Point::point_table->get_column_name(dam_label::hub_value).c_str() << " = " << this->no_outgoing << " , ";
	*text << Dam_CI_Point::point_table->get_column_name(dam_label::aut_value).c_str() << " = " << this->no_incoming << " , ";
	if (this->get_end_level_flag() == false) {
		*text << Dam_CI_Point::point_table->get_column_name(dam_label::cp_value).c_str() << " = " << this->stat_value << " , ";
		*text << Dam_CI_Point::point_table->get_column_name(dam_label::cv_value).c_str() << " = " << " -1.0" << "  ";
	}
	else {
		*text << Dam_CI_Point::point_table->get_column_name(dam_label::cv_value).c_str() << " = " << this->stat_value << " , ";
		*text << Dam_CI_Point::point_table->get_column_name(dam_label::cp_value).c_str() << " = " << " -1.0" << "  ";
	}


	*text << " WHERE ";
	*text << Dam_CI_Point::point_table->get_column_name(dam_label::glob_id) << " = " << this->global_index << "; ";

	return true;
}
//Create the database table for the CI points (static)
void Dam_CI_Point::create_point_table(QSqlDatabase *ptr_database) {
	if (Dam_CI_Point::point_table == NULL) {
		ostringstream cout;
		cout << "Create CI point data database table..." << endl;
		Sys_Common_Output::output_dam->output_txt(&cout);
		//make specific input for this class
		const string tab_name = dam_label::tab_ci_point;
		const int num_col = 25;
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

		tab_col[1].name = dam_label::point_id;
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

		tab_col[5].name = dam_label::point_name;
		tab_col[5].type = sys_label::tab_col_type_string;

		tab_col[6].name = dam_label::sector_id;
		tab_col[6].type = sys_label::tab_col_type_int;
		tab_col[6].default_value = "-1";

		tab_col[7].name = dam_label::sector_name;
		tab_col[7].type = sys_label::tab_col_type_string;

		tab_col[8].name = dam_label::sector_level;
		tab_col[8].type = sys_label::tab_col_type_int;
		tab_col[8].default_value = "-1";

		tab_col[9].name = dam_label::boundary_value;
		tab_col[9].type = sys_label::tab_col_type_double;
		tab_col[9].default_value = "0.0";

		tab_col[10].name = dam_label::recovery_time;
		tab_col[10].type = sys_label::tab_col_type_double;
		tab_col[10].default_value = "0.0";

		tab_col[11].name = dam_label::final_flag;
		tab_col[11].type = sys_label::tab_col_type_bool;
		tab_col[11].default_value = "false";

		tab_col[12].name = dam_label::regular_flag;
		tab_col[12].type = sys_label::tab_col_type_bool;
		tab_col[12].default_value = "true";

		tab_col[13].name = dam_label::activation_time;
		tab_col[13].type = sys_label::tab_col_type_double;
		tab_col[13].default_value = "0.0";


		tab_col[14].name = dam_label::hub_value;
		tab_col[14].type = sys_label::tab_col_type_int;
		tab_col[14].default_value = "0";

		tab_col[15].name = dam_label::aut_value;
		tab_col[15].type = sys_label::tab_col_type_int;
		tab_col[15].default_value = "0";

		tab_col[16].name = dam_label::cv_value;
		tab_col[16].type = sys_label::tab_col_type_double;
		tab_col[16].default_value = "-1.0";

		tab_col[17].name = dam_label::cp_value;
		tab_col[17].type = sys_label::tab_col_type_double;
		tab_col[17].default_value = "-1.0";


		tab_col[18].name = dam_label::elem_mid_x;
		tab_col[18].type = sys_label::tab_col_type_double;

		tab_col[19].name = dam_label::elem_mid_y;
		tab_col[19].type = sys_label::tab_col_type_double;

		tab_col[20].name = dam_label::sc_point;
		tab_col[20].type = sys_label::tab_col_type_point;

		tab_col[21].name = dam_label::conn_fp_id;
		tab_col[21].type = sys_label::tab_col_type_int;
		tab_col[21].default_value = "-1";

		tab_col[22].name = dam_label::conn_fp_elem_id;
		tab_col[22].type = sys_label::tab_col_type_int;
		tab_col[22].default_value = "-1";

		tab_col[23].name = dam_label::raster_connected;
		tab_col[23].type = sys_label::tab_col_type_bool;
		tab_col[23].default_value = "false";

		tab_col[24].name = label::description;
		tab_col[24].type = sys_label::tab_col_type_string;

		try {
			Dam_CI_Point::point_table = new Tables();
			if (Dam_CI_Point::point_table->create_non_existing_tables(tab_name, tab_col, num_col, ptr_database, _sys_table_type::dam) == false) {
				cout << " Table exists" << endl;
				Sys_Common_Output::output_dam->output_txt(&cout);
			};
		}
		catch (bad_alloc& t) {
			Error msg;
			msg.set_msg("Dam_CI_Point::create_point_table(QSqlDatabase *ptr_database)", "Can not allocate the memory", "Check the memory", 10, false);
			ostringstream info;
			info << "Info bad alloc: " << t.what() << endl;
			msg.make_second_info(info.str());
			throw msg;
		}
		catch (Error msg) {
			Dam_CI_Point::close_point_table();
			throw msg;
		}

		Dam_CI_Point::close_point_table();
	}
}
//Set the database table for the CI points: it sets the table name and the name of the columns and allocate them (static)
void Dam_CI_Point::set_point_table(QSqlDatabase *ptr_database, const bool not_close) {
	if (Dam_CI_Point::point_table == NULL) {
		//make specific input for this class
		const string tab_id_name = dam_label::tab_ci_point;
		string tab_col[25];

		tab_col[0] = dam_label::glob_id;
		tab_col[1] = dam_label::point_id;
		tab_col[2] = label::areastate_id;
		tab_col[3] = label::measure_id;
		tab_col[4] = label::applied_flag;

		tab_col[5] = dam_label::point_name;
		tab_col[6] = dam_label::sector_id;
		tab_col[7] = dam_label::sector_name;
		tab_col[8] = dam_label::sector_level;
		tab_col[9] = dam_label::boundary_value;
		tab_col[10] = dam_label::final_flag;

		tab_col[11] = dam_label::elem_mid_x;
		tab_col[12] = dam_label::elem_mid_y;
		tab_col[13] = dam_label::sc_point;
		tab_col[14] = dam_label::conn_fp_id;
		tab_col[15] = dam_label::conn_fp_elem_id;
		tab_col[16] = dam_label::raster_connected;
		tab_col[17] = label::description;
		tab_col[18] = dam_label::recovery_time;
		tab_col[19] = dam_label::regular_flag;
		tab_col[20] = dam_label::activation_time;

		tab_col[21] = dam_label::hub_value;
		tab_col[22] = dam_label::aut_value;
		tab_col[23] = dam_label::cv_value;
		tab_col[24] = dam_label::cp_value;


		

		try {
			Dam_CI_Point::point_table = new Tables(tab_id_name, tab_col, sizeof(tab_col) / sizeof(tab_col[0]));
			Dam_CI_Point::point_table->set_name(ptr_database, _sys_table_type::dam);
		}
		catch (bad_alloc& t) {
			Error msg;
			msg.set_msg("Dam_CI_Point::set_point_table(QSqlDatabase *ptr_database)", "Can not allocate the memory", "Check the memory", 10, false);
			ostringstream info;
			info << "Info bad alloc: " << t.what() << endl;
			msg.make_second_info(info.str());
			throw msg;
		}
		catch (Error msg) {
			if (not_close == false) {
				Dam_CI_Point::close_point_table();
			}
			throw msg;
		}
	}
}
//Delete all data in the database table for the CI points (static)
void Dam_CI_Point::delete_data_in_point_table(QSqlDatabase *ptr_database) {
	//the table is set (the name and the column names) and allocated
	try {
		Dam_CI_Point::set_point_table(ptr_database);
	}
	catch (Error msg) {
		throw msg;
	}
	//delete the table
	Dam_CI_Point::point_table->delete_data_in_table(ptr_database);
}
//Delete all data in the database table for the CI points of the given system state (static)
void Dam_CI_Point::delete_data_in_point_table(QSqlDatabase *ptr_database, const _sys_system_id id) {
	QSqlQueryModel results;
	ostringstream test_filter;
	try {
		Dam_CI_Point::set_point_table(ptr_database);
	}
	catch (Error msg) {
		throw msg;
	}
	//delete
	test_filter << "DELETE ";
	test_filter << " FROM " << Dam_CI_Point::point_table->get_table_name();
	test_filter << " WHERE ";
	test_filter << Dam_CI_Point::point_table->get_column_name(label::areastate_id) << " = " << id.area_state;
	test_filter << " AND ";
	test_filter << Dam_CI_Point::point_table->get_column_name(label::measure_id) << " = " << id.measure_nr;

	Data_Base::database_request(&results, test_filter.str(), ptr_database);

	//check the request
	if (results.lastError().isValid()) {
		Error msg;
		msg.set_msg("Dam_CI_Point::delete_data_in_point_table(QSqlDatabase *ptr_database, const _sys_system_id id)", "Invalid database request", "Check the database", 2, false);
		ostringstream info;
		info << "Table Name      : " << Dam_CI_Point::point_table->get_table_name() << endl;
		info << "Table error info: " << results.lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
}
//Close and delete the database table for the CI points (static)
void Dam_CI_Point::close_point_table(void) {
	if (Dam_CI_Point::point_table != NULL) {
		delete Dam_CI_Point::point_table;
		Dam_CI_Point::point_table = NULL;
	}
}
//Get the maximum value of the global index of the CI point database table (static)
int Dam_CI_Point::get_max_glob_id_point_table(QSqlDatabase *ptr_database) {
	try {
		Dam_CI_Point::set_point_table(ptr_database);
	}
	catch (Error msg) {
		throw msg;
	}

	//evaluate the global identifier
	int id_glob = Dam_CI_Point::point_table->maximum_int_of_column(Dam_CI_Point::point_table->get_column_name(dam_label::glob_id), ptr_database);
	return id_glob;
}
//Get the header for inserting the CI point data to database table (static)
string Dam_CI_Point::get_insert_header_point_table(QSqlDatabase *ptr_database) {
	try {
		Dam_CI_Point::set_point_table(ptr_database);
	}
	catch (Error msg) {
		throw msg;
	}
	//set the query via a query string
	ostringstream query_string;
	query_string << "INSERT INTO  " << Dam_CI_Point::point_table->get_table_name();
	query_string << " ( ";
	query_string << Dam_CI_Point::point_table->get_column_name(dam_label::glob_id) << " , ";
	query_string << Dam_CI_Point::point_table->get_column_name(dam_label::point_id) << " , ";
	query_string << Dam_CI_Point::point_table->get_column_name(label::areastate_id) << " , ";
	query_string << Dam_CI_Point::point_table->get_column_name(label::measure_id) << " , ";
	query_string << Dam_CI_Point::point_table->get_column_name(label::applied_flag) << " , ";
	//data
	query_string << Dam_CI_Point::point_table->get_column_name(dam_label::point_name) << " , ";
	query_string << Dam_CI_Point::point_table->get_column_name(dam_label::sector_id) << " , ";
	query_string << Dam_CI_Point::point_table->get_column_name(dam_label::sector_name) << " , ";
	query_string << Dam_CI_Point::point_table->get_column_name(dam_label::sector_level) << " , ";
	query_string << Dam_CI_Point::point_table->get_column_name(dam_label::boundary_value) << " , ";
	query_string << Dam_CI_Point::point_table->get_column_name(dam_label::recovery_time) << " , ";
	query_string << Dam_CI_Point::point_table->get_column_name(dam_label::regular_flag) << " , ";
	query_string << Dam_CI_Point::point_table->get_column_name(dam_label::activation_time) << " , ";
	query_string << Dam_CI_Point::point_table->get_column_name(dam_label::final_flag) << " , ";
	query_string << Dam_CI_Point::point_table->get_column_name(dam_label::elem_mid_x) << " , ";
	query_string << Dam_CI_Point::point_table->get_column_name(dam_label::elem_mid_y) << " , ";
	query_string << Dam_CI_Point::point_table->get_column_name(dam_label::sc_point) << " ) ";


	query_string << " VALUES ";
	string buffer;
	buffer = query_string.str();
	return buffer;
	
}
//Select and count the number of relevant damage points in a database table (static)
int Dam_CI_Point::select_relevant_points_database(QSqlQueryModel *results, QSqlDatabase *ptr_database, const _sys_system_id id, const bool with_output) {
	int number = 0;
	try {
		Dam_CI_Point::set_point_table(ptr_database);
	}
	catch (Error msg) {
		throw msg;
	}
	if (with_output == true) {
		ostringstream cout;
		cout << "Select relevant CI-points in database ..." << endl;
		Sys_Common_Output::output_dam->output_txt(&cout);
	}


	ostringstream test_filter;
	test_filter << "Select ";
	test_filter << Dam_CI_Point::point_table->get_column_name(dam_label::point_id) << " , ";
	test_filter << Dam_CI_Point::point_table->get_column_name(dam_label::elem_mid_x) << " , ";
	test_filter << Dam_CI_Point::point_table->get_column_name(dam_label::elem_mid_y) << " , ";
	test_filter << Dam_CI_Point::point_table->get_column_name(dam_label::conn_fp_id) << " , ";
	test_filter << Dam_CI_Point::point_table->get_column_name(dam_label::conn_fp_elem_id) << " , ";
	test_filter << Dam_CI_Point::point_table->get_column_name(dam_label::glob_id) << " , ";
	//set the element members
	test_filter << Dam_CI_Point::point_table->get_column_name(dam_label::point_name) << " , ";
	test_filter << Dam_CI_Point::point_table->get_column_name(dam_label::sector_id) << " , ";
	test_filter << Dam_CI_Point::point_table->get_column_name(dam_label::sector_name) << " , ";
	test_filter << Dam_CI_Point::point_table->get_column_name(dam_label::sector_level) << " , ";
	test_filter << Dam_CI_Point::point_table->get_column_name(dam_label::boundary_value) << " , ";
	test_filter << Dam_CI_Point::point_table->get_column_name(dam_label::final_flag) << " , ";
	test_filter << Dam_CI_Point::point_table->get_column_name(dam_label::recovery_time) << " , ";
	test_filter << Dam_CI_Point::point_table->get_column_name(dam_label::regular_flag) << " , ";
	test_filter << Dam_CI_Point::point_table->get_column_name(dam_label::activation_time) << " , ";
	test_filter << Dam_CI_Point::point_table->get_column_name(dam_label::cv_value) << " , ";
	test_filter << Dam_CI_Point::point_table->get_column_name(dam_label::cp_value) << " , ";
	test_filter << Dam_CI_Point::point_table->get_column_name(dam_label::raster_connected) << "  ";


	test_filter << " from " << Dam_CI_Point::point_table->get_table_name();
	test_filter << " where ";
	test_filter << Dam_CI_Point::point_table->get_column_name(label::applied_flag) << "= true";
	test_filter << " and ";
	test_filter << Dam_CI_Point::point_table->get_column_name(label::areastate_id) << " =" << id.area_state;
	test_filter << " and (";
	test_filter << Dam_CI_Point::point_table->get_column_name(label::measure_id) << " = " << 0;
	test_filter << " or ";
	test_filter << Dam_CI_Point::point_table->get_column_name(label::measure_id) << " = " << id.measure_nr;
	test_filter << " ) ";
	test_filter << " order by " << Dam_CI_Point::point_table->get_column_name(dam_label::point_id);

	Data_Base::database_request(results, test_filter.str(), ptr_database);

	//check the request
	if (results->lastError().isValid()) {
		Error msg;
		msg.set_msg("Dam_CI_Point::select_relevant_points_database(QSqlTableModel *results, const _sys_system_id id, const bool with_output)", "Invalid database request", "Check the database", 2, false);
		ostringstream info;
		info << "Table Name      : " << Dam_CI_Point::point_table->get_table_name() << endl;
		info << "Table error info: " << results->lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}

	number = results->rowCount();

	if (with_output == true) {
		ostringstream cout;
		cout << number << " relevant simple CI-point(s) are selected in database" << endl;
		Sys_Common_Output::output_dam->output_txt(&cout);
	}

	return number;

}
//Select and count the number of relevant damage points in a database table (just part of it) (static)
int Dam_CI_Point::select_relevant_points_database(QSqlQueryModel *results, QSqlDatabase *ptr_database, const _sys_system_id id, const int offset, const int number_rows, const bool with_output) {
	int number = 0;
	try {
		Dam_CI_Point::set_point_table(ptr_database);
	}
	catch (Error msg) {
		throw msg;
	}
	if (with_output == true) {
		ostringstream cout;
		cout << "Select relevant CI-points in database ..." << endl;
		Sys_Common_Output::output_dam->output_txt(&cout);
	}


	ostringstream test_filter;
	test_filter << "Select ";
	test_filter << Dam_CI_Point::point_table->get_column_name(dam_label::point_id) << " , ";
	test_filter << Dam_CI_Point::point_table->get_column_name(dam_label::elem_mid_x) << " , ";
	test_filter << Dam_CI_Point::point_table->get_column_name(dam_label::elem_mid_y) << " , ";
	test_filter << Dam_CI_Point::point_table->get_column_name(dam_label::conn_fp_id) << " , ";
	test_filter << Dam_CI_Point::point_table->get_column_name(dam_label::conn_fp_elem_id) << " , ";
	test_filter << Dam_CI_Point::point_table->get_column_name(dam_label::glob_id) << " , ";
	//set the element members
	test_filter << Dam_CI_Point::point_table->get_column_name(dam_label::point_name) << " , ";
	test_filter << Dam_CI_Point::point_table->get_column_name(dam_label::sector_id) << " , ";
	test_filter << Dam_CI_Point::point_table->get_column_name(dam_label::sector_name) << " , ";
	test_filter << Dam_CI_Point::point_table->get_column_name(dam_label::sector_level) << " , ";
	test_filter << Dam_CI_Point::point_table->get_column_name(dam_label::boundary_value) << " , ";
	test_filter << Dam_CI_Point::point_table->get_column_name(dam_label::final_flag) << " , ";
	test_filter << Dam_CI_Point::point_table->get_column_name(dam_label::recovery_time) << " , ";
	test_filter << Dam_CI_Point::point_table->get_column_name(dam_label::regular_flag) << " , ";
	test_filter << Dam_CI_Point::point_table->get_column_name(dam_label::activation_time) << " , ";
	test_filter << Dam_CI_Point::point_table->get_column_name(dam_label::cv_value) << " , ";
	test_filter << Dam_CI_Point::point_table->get_column_name(dam_label::cp_value) << " , ";
	test_filter << Dam_CI_Point::point_table->get_column_name(dam_label::raster_connected) << "  ";


	test_filter << " from " << Dam_CI_Point::point_table->get_table_name();
	test_filter << " where ";
	test_filter << Dam_CI_Point::point_table->get_column_name(label::applied_flag) << "= true";
	test_filter << " and ";
	test_filter << Dam_CI_Point::point_table->get_column_name(label::areastate_id) << " =" << id.area_state;
	test_filter << " and (";
	test_filter << Dam_CI_Point::point_table->get_column_name(label::measure_id) << " = " << 0;
	test_filter << " or ";
	test_filter << Dam_CI_Point::point_table->get_column_name(label::measure_id) << " = " << id.measure_nr;
	test_filter << " ) ";
	test_filter << " order by " << Dam_CI_Point::point_table->get_column_name(dam_label::point_id);
	test_filter << " LIMIT " << number_rows;
	test_filter << " OFFSET " << offset;

	Data_Base::database_request(results, test_filter.str(), ptr_database);

	//check the request
	if (results->lastError().isValid()) {
		Error msg;
		msg.set_msg("Dam_CI_Point::select_relevant_points_database(QSqlQueryModel *results, QSqlDatabase *ptr_database, const _sys_system_id id, const int offset, const int number_rows, const bool with_output)", "Invalid database request", "Check the database", 2, false);
		ostringstream info;
		info << "Table Name      : " << Dam_CI_Point::point_table->get_table_name() << endl;
		info << "Table error info: " << results->lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}

	number = results->rowCount();

	if (with_output == true) {
		ostringstream cout;
		cout << number << " relevant simple CI-point(s) are selected in database" << endl;
		Sys_Common_Output::output_dam->output_txt(&cout);
	}

	return number;

}
//Count the number of relevant damage points in a database table (static)
int Dam_CI_Point::count_relevant_points_database(QSqlQueryModel *results, QSqlDatabase *ptr_database, const _sys_system_id id, const bool with_output) {
	int number = 0;
	try {
		Dam_CI_Point::set_point_table(ptr_database);
	}
	catch (Error msg) {
		throw msg;
	}
	if (with_output == true) {
		ostringstream cout;
		cout << "Count relevant CI-points in database ..." << endl;
		Sys_Common_Output::output_dam->output_txt(&cout);
	}

	ostringstream test_filter;
	test_filter << "Select COUNT(";
	test_filter << Dam_CI_Point::point_table->get_column_name(dam_label::point_id) << " ) ";

	test_filter << " from " << Dam_CI_Point::point_table->get_table_name();
	test_filter << " where ";
	test_filter << Dam_CI_Point::point_table->get_column_name(label::applied_flag) << "= true";
	test_filter << " and ";
	test_filter << Dam_CI_Point::point_table->get_column_name(label::areastate_id) << " =" << id.area_state;
	test_filter << " and (";
	test_filter << Dam_CI_Point::point_table->get_column_name(label::measure_id) << " = " << 0;
	test_filter << " or ";
	test_filter << Dam_CI_Point::point_table->get_column_name(label::measure_id) << " = " << id.measure_nr;
	test_filter << " ) ";

	Data_Base::database_request(results, test_filter.str(), ptr_database);

	//check the request
	if (results->lastError().isValid()) {
		Error msg;
		msg.set_msg("Dam_CI_Point::count_relevant_points_database(QSqlTableModel *results, const _sys_system_id id, const bool with_output)", "Invalid database request", "Check the database", 2, false);
		ostringstream info;
		info << "Table Name      : " << Dam_CI_Point::point_table->get_table_name() << endl;
		info << "Table error info: " << results->lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}

	if (results->rowCount() > 0) {
		number = results->record(0).field(0).value().toInt();
	}

	if (with_output == true) {
		ostringstream cout;
		cout << number << " relevant CI-point(s) are found in database" << endl;
		Sys_Common_Output::output_dam->output_txt(&cout);
	}

	return number;
}
//Count number of not connected points to the hydraulic model (static)
int Dam_CI_Point::count_not_connected_points2hyd(QSqlDatabase *ptr_database, const _sys_system_id id) {
	int number = 0;
	try {
		Dam_CI_Point::set_point_table(ptr_database);
	}
	catch (Error msg) {
		throw msg;
	}

	QSqlQueryModel results;

	ostringstream test_filter;
	test_filter << "Select COUNT(";
	test_filter << Dam_CI_Point::point_table->get_column_name(dam_label::point_id) << " ) ";

	test_filter << " from " << Dam_CI_Point::point_table->get_table_name();
	test_filter << " where ";
	test_filter << Dam_CI_Point::point_table->get_column_name(label::applied_flag) << "= true";
	test_filter << " and ";
	test_filter << Dam_CI_Point::point_table->get_column_name(label::areastate_id) << " =" << id.area_state;
	test_filter << " and (";
	test_filter << Dam_CI_Point::point_table->get_column_name(label::measure_id) << " = " << 0;
	test_filter << " or ";
	test_filter << Dam_CI_Point::point_table->get_column_name(label::measure_id) << " = " << id.measure_nr;
	test_filter << " ) ";
	test_filter << " and ";
	test_filter << Dam_CI_Point::point_table->get_column_name(dam_label::raster_connected) << " = " << functions::convert_boolean2string(false);

	Data_Base::database_request(&results, test_filter.str(), ptr_database);

	//check the request
	if (results.lastError().isValid()) {
		Error msg;
		msg.set_msg("Dam_CI_Point::count_not_connected_points2hyd(QSqlDatabase *ptr_database, const _sys_system_id id)", "Invalid database request", "Check the database", 2, false);
		ostringstream info;
		info << "Table Name      : " << Dam_CI_Point::point_table->get_table_name() << endl;
		info << "Table error info: " << results.lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}

	if (results.rowCount() > 0) {
		number = results.record(0).field(0).value().toInt();
	}

	return number;
}
//Create the database table for the results of the damage calculation per point (static)
void Dam_CI_Point::create_erg_table(QSqlDatabase *ptr_database) {
	if (Dam_CI_Point::point_erg_table == NULL) {
		ostringstream cout;
		cout << "Create CI-point result database table..." << endl;
		Sys_Common_Output::output_dam->output_txt(&cout);
		//make specific input for this class
		const string tab_name = dam_label::tab_ci_point_erg;
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

		tab_col[1].name = dam_label::point_id;
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

		tab_col[8].name = dam_label::sector_level;
		tab_col[8].type = sys_label::tab_col_type_int;
		tab_col[8].default_value = "-1";

		tab_col[9].name = dam_label::failure_type;
		tab_col[9].type = sys_label::tab_col_type_string;

		tab_col[10].name = dam_label::failure_duration;
		tab_col[10].type = sys_label::tab_col_type_double;
		tab_col[10].default_value = "0.0";

		tab_col[11].name = dam_label::regular_flag;
		tab_col[11].type = sys_label::tab_col_type_bool;
		tab_col[11].default_value = "true";


		tab_col[12].name = dam_label::sc_point;
		tab_col[12].type = sys_label::tab_col_type_point;

		try {
			Dam_CI_Point::point_erg_table = new Tables();
			if (Dam_CI_Point::point_erg_table->create_non_existing_tables(tab_name, tab_col, num_col, ptr_database, _sys_table_type::dam) == false) {
				cout << " Table exists" << endl;
				Sys_Common_Output::output_dam->output_txt(&cout);
			};
		}
		catch (bad_alloc& t) {
			Error msg;
			msg.set_msg("Dam_CI_Point::create_erg_table(QSqlDatabase *ptr_database)", "Can not allocate the memory", "Check the memory", 10, false);
			ostringstream info;
			info << "Info bad alloc: " << t.what() << endl;
			msg.make_second_info(info.str());
			throw msg;
		}
		catch (Error msg) {
			Dam_CI_Point::close_erg_table();
			throw msg;
		}

		Dam_CI_Point::close_erg_table();
	}
}
//Set the database table for the results of the damage calculation per point: it sets the table name and the name of the columns and allocate them (static)
void Dam_CI_Point::set_erg_table(QSqlDatabase *ptr_database, const bool not_close) {
	if (Dam_CI_Point::point_erg_table == NULL) {
		//make specific input for this class
		const string tab_id_name = dam_label::tab_ci_point_erg;
		string tab_col[13];

		tab_col[0] = dam_label::glob_id;
		tab_col[1] = dam_label::point_id;
		tab_col[2] = label::areastate_id;
		tab_col[3] = label::measure_id;
		tab_col[4] = label::applied_flag;
		tab_col[5] = hyd_label::sz_bound_id;
		tab_col[6] = risk_label::sz_break_id;
		tab_col[7] = dam_label::sector_id;
		tab_col[8]= dam_label::sector_level;
		tab_col[9] = dam_label::failure_type;
		tab_col[10] = dam_label::failure_duration;
		tab_col[11] = dam_label::sc_point;
		tab_col[12] = dam_label::regular_flag;

		try {
			Dam_CI_Point::point_erg_table = new Tables(tab_id_name, tab_col, sizeof(tab_col) / sizeof(tab_col[0]));
			Dam_CI_Point::point_erg_table->set_name(ptr_database, _sys_table_type::dam);
		}
		catch (bad_alloc& t) {
			Error msg;
			msg.set_msg("Dam_CI_Point::set_erg_table(QSqlDatabase *ptr_database)", "Can not allocate the memory", "Check the memory", 10, false);
			ostringstream info;
			info << "Info bad alloc: " << t.what() << endl;
			msg.make_second_info(info.str());
			throw msg;
		}
		catch (Error msg) {
			if (not_close == false) {
				Dam_CI_Point::close_erg_table();
			}
			throw msg;
		}
	}
}
//Delete all data in the database table for the results of the damage calculation per point (static)
void Dam_CI_Point::delete_data_in_erg_table(QSqlDatabase *ptr_database) {
	//the table is set (the name and the column names) and allocated
	try {
		Dam_CI_Point::set_erg_table(ptr_database);
	}
	catch (Error msg) {
		throw msg;
	}
	//delete the table
	Dam_CI_Point::point_erg_table->delete_data_in_table(ptr_database);
}
//Delete the data in the database table for the results of the damage calculation specified by the system state (static)
void Dam_CI_Point::delete_data_in_erg_table(QSqlDatabase *ptr_database, const _sys_system_id id) {
	//the table is set (the name and the column names) and allocated
	try {
		Dam_CI_Point::set_erg_table(ptr_database);
	}
	catch (Error msg) {
		throw msg;
	}

	//delete the table
	QSqlQuery query(*ptr_database);
	ostringstream query_string;
	query_string << "DELETE FROM " << Dam_CI_Point::point_erg_table->get_table_name();
	query_string << " WHERE ";
	query_string << Dam_CI_Point::point_erg_table->get_column_name(label::areastate_id) << " = " << id.area_state;
	query_string << " AND ";
	query_string << Dam_CI_Point::point_erg_table->get_column_name(label::measure_id) << " = " << id.measure_nr;

	Data_Base::database_request(&query, query_string.str(), ptr_database);
	if (query.lastError().isValid() == true) {
		Error msg;
		msg.set_msg("Dam_CI_Point::delete_data_in_erg_table(QSqlDatabase *ptr_database, const _sys_system_id id)", "Invalid database request", "Check the database", 2, false);
		ostringstream info;
		info << "Table Name      : " << Dam_CI_Point::point_erg_table->get_table_name() << endl;
		info << "Table error info: " << query.lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
}
//Delete the data in the database table for the results of the damage calculation specified parameters (static)
void Dam_CI_Point::delete_data_in_erg_table(QSqlDatabase *ptr_database, const _sys_system_id id, const string break_sz, const bool like_flag) {
	//the table is set (the name and the column names) and allocated
	try {
		Dam_CI_Point::set_erg_table(ptr_database);
	}
	catch (Error msg) {
		throw msg;
	}

	//delete the table
	QSqlQuery query(*ptr_database);
	ostringstream query_string;
	query_string << "DELETE FROM " << Dam_CI_Point::point_erg_table->get_table_name();
	query_string << " WHERE ";
	query_string << Dam_CI_Point::point_erg_table->get_column_name(label::areastate_id) << " = " << id.area_state;
	query_string << " AND ";
	query_string << Dam_CI_Point::point_erg_table->get_column_name(label::measure_id) << " = " << id.measure_nr;
	query_string << " AND ";
	if (like_flag == false) {
		query_string << Dam_CI_Point::point_erg_table->get_column_name(risk_label::sz_break_id) << " = '" << break_sz << "'";
	}
	else {
		query_string << Dam_CI_Point::point_erg_table->get_column_name(risk_label::sz_break_id) << " LIKE '" << break_sz << "'";
	}

	Data_Base::database_request(&query, query_string.str(), ptr_database);
	if (query.lastError().isValid() == true) {
		Error msg;
		msg.set_msg("Dam_CI_Point::delete_data_in_erg_table(QSqlDatabase *ptr_database, const _sys_system_id id, const string break_sz, const bool like_flag)", "Invalid database request", "Check the database", 2, false);
		ostringstream info;
		info << "Table Name      : " << Dam_CI_Point::point_erg_table->get_table_name() << endl;
		info << "Table error info: " << query.lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
}
//Delete the data in the database table for the results of the damage calculation specified by the system id and the scenario-ids (static)
void Dam_CI_Point::delete_data_in_erg_table(QSqlDatabase *ptr_database, const _sys_system_id id, const int bound_sz, const string break_sz) {
	//the table is set (the name and the column names) and allocated
	try {
		Dam_CI_Point::set_erg_table(ptr_database);
	}
	catch (Error msg) {
		throw msg;
	}

	//delete the table
	QSqlQuery query(*ptr_database);
	ostringstream query_string;
	query_string << "DELETE FROM " << Dam_CI_Point::point_erg_table->get_table_name();
	query_string << " WHERE ";
	query_string << Dam_CI_Point::point_erg_table->get_column_name(label::areastate_id) << " = " << id.area_state;
	query_string << " AND ";
	query_string << Dam_CI_Point::point_erg_table->get_column_name(label::measure_id) << " = " << id.measure_nr;
	query_string << " AND ";
	query_string << Dam_CI_Point::point_erg_table->get_column_name(hyd_label::sz_bound_id) << " = " << bound_sz;
	query_string << " AND ";
	query_string << Dam_CI_Point::point_erg_table->get_column_name(risk_label::sz_break_id) << " = '" << break_sz << "'";

	Data_Base::database_request(&query, query_string.str(), ptr_database);
	if (query.lastError().isValid() == true) {
		Error msg;
		msg.set_msg("Dam_CI_Point::delete_data_in_erg_table(QSqlDatabase *ptr_database, const _sys_system_id id, const int bound_sz, const string break_sz )", "Invalid database request", "Check the database", 2, false);
		ostringstream info;
		info << "Table Name      : " << Dam_CI_Point::point_erg_table->get_table_name() << endl;
		info << "Table error info: " << query.lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
}
//Delete the data in the database table for the results of the damage calculation specified by the hydraulic scenario-ids (static)
void Dam_CI_Point::delete_data_in_erg_table(QSqlDatabase *ptr_database, const int bound_sz) {
	//the table is set (the name and the column names) and allocated
	try {
		Dam_CI_Point::set_erg_table(ptr_database);
	}
	catch (Error msg) {
		throw msg;
	}

	//delete the table
	QSqlQuery query(*ptr_database);
	ostringstream query_string;
	query_string << "DELETE FROM " << Dam_CI_Point::point_erg_table->get_table_name();
	query_string << " WHERE ";
	query_string << Dam_CI_Point::point_erg_table->get_column_name(hyd_label::sz_bound_id) << " = " << bound_sz;

	Data_Base::database_request(&query, query_string.str(), ptr_database);
	if (query.lastError().isValid() == true) {
		Error msg;
		msg.set_msg("Dam_CI_Point::delete_data_in_erg_table(QSqlDatabase *ptr_database, const int bound_sz)", "Invalid database request", "Check the database", 2, false);
		ostringstream info;
		info << "Table Name      : " << Dam_CI_Point::point_erg_table->get_table_name() << endl;
		info << "Table error info: " << query.lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
}
//Close and delete the database table for the results of the damage calculation per point (static)
void Dam_CI_Point::close_erg_table(void) {
	if (Dam_CI_Point::point_erg_table != NULL) {
		delete Dam_CI_Point::point_erg_table;
		Dam_CI_Point::point_erg_table = NULL;
	}
}
//Get the maximum value of the global index of the result point database table (static)
int Dam_CI_Point::get_max_glob_id_point_erg_table(QSqlDatabase *ptr_database) {
	try {
		Dam_CI_Point::set_erg_table(ptr_database);
	}
	catch (Error msg) {
		throw msg;
	}

	//evaluate the global identifier
	int id_glob = Dam_CI_Point::point_erg_table->maximum_int_of_column(Dam_CI_Point::point_erg_table->get_column_name(dam_label::glob_id), ptr_database);
	return id_glob;
}
//Get the header for inserting the point result data to database table (static)
string Dam_CI_Point::get_insert_header_erg_table(QSqlDatabase *ptr_database) {
	try {
		Dam_CI_Point::set_erg_table(ptr_database);
	}
	catch (Error msg) {
		throw msg;
	}
	//set the query via a query string
	ostringstream query_string;
	query_string << "INSERT INTO  " << Dam_CI_Point::point_erg_table->get_table_name();
	query_string << " ( ";
	query_string << Dam_CI_Point::point_erg_table->get_column_name(dam_label::glob_id) << " , ";
	query_string << Dam_CI_Point::point_erg_table->get_column_name(dam_label::point_id) << " , ";
	query_string << Dam_CI_Point::point_erg_table->get_column_name(label::areastate_id) << " , ";
	query_string << Dam_CI_Point::point_erg_table->get_column_name(label::measure_id) << " , ";
	query_string << Dam_CI_Point::point_erg_table->get_column_name(hyd_label::sz_bound_id) << " , ";
	query_string << Dam_CI_Point::point_erg_table->get_column_name(risk_label::sz_break_id) << " , ";
	//results
	query_string << Dam_CI_Point::point_erg_table->get_column_name(dam_label::sector_id) << " , ";
	query_string << Dam_CI_Point::point_erg_table->get_column_name(dam_label::sector_level) << " , ";
	query_string << Dam_CI_Point::point_erg_table->get_column_name(dam_label::failure_type) << " , ";
	query_string << Dam_CI_Point::point_erg_table->get_column_name(dam_label::failure_duration) << " , ";
	query_string << Dam_CI_Point::point_erg_table->get_column_name(dam_label::regular_flag) << " , ";

	query_string << Dam_CI_Point::point_erg_table->get_column_name(dam_label::sc_point) << ")";

	query_string << " VALUES ";
	string buffer;
	buffer = query_string.str();
	return buffer;
}
//Select data in the database table for the results of the damage calculation specified by the system id and the scenario-ids (static)
int Dam_CI_Point::select_data_in_erg_table(QSqlQueryModel *results, QSqlDatabase *ptr_database, const _sys_system_id id, const int bound_sz, const string break_sz) {
	//the table is set (the name and the column names) and allocated
	try {
		Dam_CI_Point::set_erg_table(ptr_database);
	}
	catch (Error msg) {
		throw msg;
	}

	ostringstream query_string;
	query_string << "SELECT ";
	query_string << Dam_CI_Point::point_erg_table->get_column_name(dam_label::point_id) << " , ";

	query_string << Dam_CI_Point::point_erg_table->get_column_name(dam_label::sector_id) << " , ";
	query_string << Dam_CI_Point::point_erg_table->get_column_name(dam_label::sector_level) << " , ";
	query_string << Dam_CI_Point::point_erg_table->get_column_name(dam_label::failure_type) << " , ";
	query_string << Dam_CI_Point::point_erg_table->get_column_name(dam_label::failure_duration) << " , ";
	query_string << Dam_CI_Point::point_erg_table->get_column_name(dam_label::regular_flag) << " , ";

	query_string << " ST_ASTEXT(" << Dam_CI_Point::point_erg_table->get_column_name(dam_label::sc_point) << ")";

	query_string << " FROM " << Dam_CI_Point::point_erg_table->get_table_name();
	query_string << " WHERE ";
	query_string << Dam_CI_Point::point_erg_table->get_column_name(label::applied_flag) << "= true";
	query_string << " AND ";
	query_string << Dam_CI_Point::point_erg_table->get_column_name(label::areastate_id) << " =" << id.area_state;
	query_string << " AND (";
	query_string << Dam_CI_Point::point_erg_table->get_column_name(label::measure_id) << " = " << 0;
	query_string << " OR ";
	query_string << Dam_CI_Point::point_erg_table->get_column_name(label::measure_id) << " = " << id.measure_nr;
	query_string << " ) ";
	query_string << " AND ";
	query_string << Dam_CI_Point::point_erg_table->get_column_name(hyd_label::sz_bound_id) << " = " << bound_sz;
	query_string << " AND ";
	query_string << Dam_CI_Point::point_erg_table->get_column_name(risk_label::sz_break_id) << " = '" << break_sz << "'";

	query_string << " ORDER BY ";
	query_string << Dam_CI_Point::point_erg_table->get_column_name(dam_label::point_id) << "  ";

	Data_Base::database_request(results, query_string.str(), ptr_database);
	if (results->lastError().isValid() == true) {
		Error msg;
		msg.set_msg("Dam_CI_Point::select_data_in_erg_table(QSqlDatabase *ptr_database, const _sys_system_id id, const int bound_sz, const string break_sz )", "Invalid database request", "Check the database", 2, false);
		ostringstream info;
		info << "Table Name      : " << Dam_CI_Point::point_erg_table->get_table_name() << endl;
		info << "Table error info: " << results->lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}

	return results->rowCount();
}
//Select data in the database table for the results of the damage calculation specified by the system id and the scenario-ids (just part of it) (static)
int Dam_CI_Point::select_data_in_erg_table(QSqlQueryModel *results, QSqlDatabase *ptr_database, const _sys_system_id id, const int bound_sz, const string break_sz, const int offset, const int number_row) {
	//the table is set (the name and the column names) and allocated
	try {
		Dam_CI_Point::set_erg_table(ptr_database);
	}
	catch (Error msg) {
		throw msg;
	}

	ostringstream query_string;
	query_string << "SELECT ";
	query_string << Dam_CI_Point::point_erg_table->get_column_name(dam_label::point_id) << " , ";

	query_string << Dam_CI_Point::point_erg_table->get_column_name(dam_label::sector_id) << " , ";
	query_string << Dam_CI_Point::point_erg_table->get_column_name(dam_label::sector_level) << " , ";
	query_string << Dam_CI_Point::point_erg_table->get_column_name(dam_label::failure_type) << " , ";
	query_string << Dam_CI_Point::point_erg_table->get_column_name(dam_label::failure_duration) << " , ";
	query_string << Dam_CI_Point::point_erg_table->get_column_name(dam_label::regular_flag) << " , ";

	query_string << " ST_ASTEXT(" << Dam_CI_Point::point_erg_table->get_column_name(dam_label::sc_point) << ")";

	query_string << " FROM " << Dam_CI_Point::point_erg_table->get_table_name();
	query_string << " WHERE ";
	query_string << Dam_CI_Point::point_erg_table->get_column_name(label::applied_flag) << "= true";
	query_string << " AND ";
	query_string << Dam_CI_Point::point_erg_table->get_column_name(label::areastate_id) << " =" << id.area_state;
	query_string << " AND (";
	query_string << Dam_CI_Point::point_erg_table->get_column_name(label::measure_id) << " = " << 0;
	query_string << " OR ";
	query_string << Dam_CI_Point::point_erg_table->get_column_name(label::measure_id) << " = " << id.measure_nr;
	query_string << " ) ";
	query_string << " AND ";
	query_string << Dam_CI_Point::point_erg_table->get_column_name(hyd_label::sz_bound_id) << " = " << bound_sz;
	query_string << " AND ";
	query_string << Dam_CI_Point::point_erg_table->get_column_name(risk_label::sz_break_id) << " = '" << break_sz << "'";

	query_string << " ORDER BY ";
	query_string << Dam_CI_Point::point_erg_table->get_column_name(dam_label::point_id) << "  ";
	query_string << " LIMIT " << number_row;
	query_string << " OFFSET " << offset;

	Data_Base::database_request(results, query_string.str(), ptr_database);
	if (results->lastError().isValid() == true) {
		Error msg;
		msg.set_msg("Dam_CI_Point::select_data_in_erg_table(QSqlDatabase *ptr_database, const _sys_system_id id, const int bound_sz, const string break_sz, const int offset, const int number_row )", "Invalid database request", "Check the database", 2, false);
		ostringstream info;
		info << "Table Name      : " << Dam_CI_Point::point_erg_table->get_table_name() << endl;
		info << "Table error info: " << results->lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}

	return results->rowCount();
}
//Count data in the database table for the results of the damage calculation specified by the system id and the scenario-ids (static)
int Dam_CI_Point::count_data_in_erg_table(QSqlQueryModel *results, QSqlDatabase *ptr_database, const _sys_system_id id, const int bound_sz, const string break_sz) {
	//the table is set (the name and the column names) and allocated
	try {
		Dam_CI_Point::set_erg_table(ptr_database);
	}
	catch (Error msg) {
		throw msg;
	}

	int number = 0;
	ostringstream query_string;
	query_string << "SELECT COUNT(";
	query_string << Dam_CI_Point::point_erg_table->get_column_name(dam_label::point_id) << " ) ";

	query_string << " FROM " << Dam_CI_Point::point_erg_table->get_table_name();
	query_string << " WHERE ";
	query_string << Dam_CI_Point::point_erg_table->get_column_name(label::applied_flag) << "= true";
	query_string << " AND ";
	query_string << Dam_CI_Point::point_erg_table->get_column_name(label::areastate_id) << " =" << id.area_state;
	query_string << " AND (";
	query_string << Dam_CI_Point::point_erg_table->get_column_name(label::measure_id) << " = " << 0;
	query_string << " OR ";
	query_string << Dam_CI_Point::point_erg_table->get_column_name(label::measure_id) << " = " << id.measure_nr;
	query_string << " ) ";
	query_string << " AND ";
	query_string << Dam_CI_Point::point_erg_table->get_column_name(hyd_label::sz_bound_id) << " = " << bound_sz;
	query_string << " AND ";
	query_string << Dam_CI_Point::point_erg_table->get_column_name(risk_label::sz_break_id) << " = '" << break_sz << "'";

	Data_Base::database_request(results, query_string.str(), ptr_database);
	if (results->lastError().isValid() == true) {
		Error msg;
		msg.set_msg("Dam_CI_Point::count_data_in_erg_table(QSqlDatabase *ptr_database, const _sys_system_id id, const int bound_sz, const string break_sz)", "Invalid database request", "Check the database", 2, false);
		ostringstream info;
		info << "Table Name      : " << Dam_CI_Point::point_erg_table->get_table_name() << endl;
		info << "Table error info: " << results->lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}

	if (results->rowCount() > 0) {
		number = results->record(0).field(0).value().toInt();
	}

	return number;
}
//Switch the applied-flag for the CI-points damage results in the database table for a defined system state (static)
void Dam_CI_Point::switch_applied_flag_erg_table(QSqlDatabase *ptr_database, const _sys_system_id id, const bool flag) {
	//the table is set (the name and the column names) and allocated
	try {
		Dam_CI_Point::set_erg_table(ptr_database);
	}
	catch (Error msg) {
		throw msg;
	}
	QSqlQueryModel query;

	ostringstream query_string;
	query_string << "UPDATE ";
	query_string << Dam_CI_Point::point_erg_table->get_table_name();
	query_string << " SET ";
	query_string << Dam_CI_Point::point_erg_table->get_column_name(label::applied_flag) << " = " << functions::convert_boolean2string(flag);
	query_string << " WHERE ";
	query_string << Dam_CI_Point::point_erg_table->get_column_name(label::areastate_id) << " = " << id.area_state;
	query_string << " AND ";
	query_string << Dam_CI_Point::point_erg_table->get_column_name(label::measure_id) << " = " << id.measure_nr;
	query_string << " AND ";
	query_string << Dam_CI_Point::point_erg_table->get_column_name(label::applied_flag) << " = " << functions::convert_boolean2string(!flag);

	Data_Base::database_request(&query, query_string.str(), ptr_database);
	if (query.lastError().isValid() == true) {
		Error msg;
		msg.set_msg("Dam_CI_Point::switch_applied_flag_erg_table(QSqlDatabase *ptr_database, const _sys_system_id id, const bool flag)", "Invalid database request", "Check the database", 2, false);
		ostringstream info;
		info << "Table Name      : " << Dam_CI_Point::point_erg_table->get_table_name() << endl;
		info << "Table error info: " << query.lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}


	//instat
	//the table is set (the name and the column names) and allocated
	try {
		Dam_CI_Point::set_instat_erg_table(ptr_database);
	}
	catch (Error msg) {
		throw msg;
	}
	QSqlQueryModel query1;

	ostringstream query_string1;
	query_string1 << "UPDATE ";
	query_string1 << Dam_CI_Point::point_instat_erg_table->get_table_name();
	query_string1 << " SET ";
	query_string1 << Dam_CI_Point::point_instat_erg_table->get_column_name(label::applied_flag) << " = " << functions::convert_boolean2string(flag);
	query_string1 << " WHERE ";
	query_string1 << Dam_CI_Point::point_instat_erg_table->get_column_name(label::areastate_id) << " = " << id.area_state;
	query_string1 << " AND ";
	query_string1 << Dam_CI_Point::point_instat_erg_table->get_column_name(label::measure_id) << " = " << id.measure_nr;
	query_string1 << " AND ";
	query_string1 << Dam_CI_Point::point_instat_erg_table->get_column_name(label::applied_flag) << " = " << functions::convert_boolean2string(!flag);

	Data_Base::database_request(&query1, query_string1.str(), ptr_database);
	if (query1.lastError().isValid() == true) {
		Error msg;
		msg.set_msg("Dam_CI_Point::switch_applied_flag_erg_table(QSqlDatabase *ptr_database, const _sys_system_id id, const bool flag)", "Invalid database request", "Check the database", 2, false);
		ostringstream info;
		info << "Table Name      : " << Dam_CI_Point::point_instat_erg_table->get_table_name() << endl;
		info << "Table error info: " << query1.lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}

}
//Switch the applied-flag for the CI-points damage results in the database table for a defined system state (static)
void Dam_CI_Point::switch_applied_flag_erg_table(QSqlDatabase *ptr_database, const _sys_system_id id, const int hyd_sc, const bool flag) {
	//the table is set (the name and the column names) and allocated
	try {
		Dam_CI_Point::set_erg_table(ptr_database);
	}
	catch (Error msg) {
		throw msg;
	}
	QSqlQueryModel query;

	ostringstream query_string;
	query_string << "UPDATE ";
	query_string << Dam_CI_Point::point_erg_table->get_table_name();
	query_string << " SET ";
	query_string << Dam_CI_Point::point_erg_table->get_column_name(label::applied_flag) << " = " << functions::convert_boolean2string(flag);
	query_string << " WHERE ";
	query_string << Dam_CI_Point::point_erg_table->get_column_name(label::areastate_id) << " = " << id.area_state;
	query_string << " AND ";
	query_string << Dam_CI_Point::point_erg_table->get_column_name(label::measure_id) << " = " << id.measure_nr;
	query_string << " AND ";
	query_string << Dam_CI_Point::point_erg_table->get_column_name(hyd_label::sz_bound_id) << " = " << hyd_sc;
	query_string << " AND ";
	query_string << Dam_CI_Point::point_erg_table->get_column_name(label::applied_flag) << " = " << functions::convert_boolean2string(!flag);

	Data_Base::database_request(&query, query_string.str(), ptr_database);
	if (query.lastError().isValid() == true) {
		Error msg;
		msg.set_msg("Dam_CI_Point::switch_applied_flag_erg_table(QSqlDatabase *ptr_database, const _sys_system_id id, const bool flag)", "Invalid database request", "Check the database", 2, false);
		ostringstream info;
		info << "Table Name      : " << Dam_CI_Point::point_erg_table->get_table_name() << endl;
		info << "Table error info: " << query.lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}

	//instat
	//the table is set (the name and the column names) and allocated
	try {
		Dam_CI_Point::set_instat_erg_table(ptr_database);
	}
	catch (Error msg) {
		throw msg;
	}
	QSqlQueryModel query1;

	ostringstream query_string1;
	query_string1 << "UPDATE ";
	query_string1 << Dam_CI_Point::point_instat_erg_table->get_table_name();
	query_string1 << " SET ";
	query_string1 << Dam_CI_Point::point_instat_erg_table->get_column_name(label::applied_flag) << " = " << functions::convert_boolean2string(flag);
	query_string1 << " WHERE ";
	query_string1 << Dam_CI_Point::point_instat_erg_table->get_column_name(label::areastate_id) << " = " << id.area_state;
	query_string1 << " AND ";
	query_string1 << Dam_CI_Point::point_instat_erg_table->get_column_name(label::measure_id) << " = " << id.measure_nr;
	query_string1 << " AND ";
	query_string1 << Dam_CI_Point::point_instat_erg_table->get_column_name(hyd_label::sz_bound_id) << " = " << hyd_sc;
	query_string1 << " AND ";
	query_string1 << Dam_CI_Point::point_instat_erg_table->get_column_name(label::applied_flag) << " = " << functions::convert_boolean2string(!flag);

	Data_Base::database_request(&query1, query_string1.str(), ptr_database);
	if (query1.lastError().isValid() == true) {
		Error msg;
		msg.set_msg("Dam_CI_Point::switch_applied_flag_erg_table(QSqlDatabase *ptr_database, const _sys_system_id id, const bool flag)", "Invalid database request", "Check the database", 2, false);
		ostringstream info;
		info << "Table Name      : " << Dam_CI_Point::point_instat_erg_table->get_table_name() << endl;
		info << "Table error info: " << query1.lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
}
//Switch the applied-flag for the CI-points in the database table for a defined system state (static)
void Dam_CI_Point::switch_applied_flag_point_table(QSqlDatabase *ptr_database, const _sys_system_id id, const bool flag) {
	try {
		Dam_CI_Point::set_point_table(ptr_database);
	}
	catch (Error msg) {
		throw msg;
	}
	QSqlQueryModel query;

	ostringstream query_string;
	query_string << "UPDATE ";
	query_string << Dam_CI_Point::point_table->get_table_name();
	query_string << " SET ";
	query_string << Dam_CI_Point::point_table->get_column_name(label::applied_flag) << " = " << functions::convert_boolean2string(flag);
	query_string << " WHERE ";
	query_string << Dam_CI_Point::point_table->get_column_name(label::areastate_id) << " = " << id.area_state;
	query_string << " AND ";
	query_string << Dam_CI_Point::point_table->get_column_name(label::measure_id) << " = " << id.measure_nr;
	query_string << " AND ";
	query_string << Dam_CI_Point::point_table->get_column_name(label::applied_flag) << " = " << functions::convert_boolean2string(!flag);

	Data_Base::database_request(&query, query_string.str(), ptr_database);
	if (query.lastError().isValid() == true) {
		Error msg;
		msg.set_msg("Dam_CI_Point::switch_applied_flag_point_table(QSqlDatabase *ptr_database, const _sys_system_id id, const bool flag)", "Invalid database request", "Check the database", 2, false);
		ostringstream info;
		info << "Table Name      : " << Dam_CI_Point::point_table->get_table_name() << endl;
		info << "Table error info: " << query.lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
}
//Switch the applied-flag for the CI-points in the database table for a defined system state (static)
void Dam_CI_Point::switch_applied_flag_point_table(QSqlDatabase *ptr_database, const _sys_system_id id, const int point_id, const bool flag) {
	try {
		Dam_CI_Point::set_point_table(ptr_database);
	}
	catch (Error msg) {
		throw msg;
	}
	QSqlQueryModel query;

	ostringstream query_string;
	query_string << "UPDATE ";
	query_string << Dam_CI_Point::point_table->get_table_name();
	query_string << " SET ";
	query_string << Dam_CI_Point::point_table->get_column_name(label::applied_flag) << " = " << functions::convert_boolean2string(flag);
	query_string << " WHERE ";
	query_string << Dam_CI_Point::point_table->get_column_name(label::areastate_id) << " = " << id.area_state;
	query_string << " AND ";
	query_string << Dam_CI_Point::point_table->get_column_name(label::measure_id) << " = " << id.measure_nr;
	query_string << " AND ";
	query_string << Dam_CI_Point::point_table->get_column_name(dam_label::point_id) << " = " << point_id;
	query_string << " AND ";
	query_string << Dam_CI_Point::point_table->get_column_name(label::applied_flag) << " = " << functions::convert_boolean2string(!flag);

	Data_Base::database_request(&query, query_string.str(), ptr_database);
	if (query.lastError().isValid() == true) {
		Error msg;
		msg.set_msg("Dam_CI_Point::switch_applied_flag_point_table(QSqlDatabase *ptr_database, const _sys_system_id id, const bool flag)", "Invalid database request", "Check the database", 2, false);
		ostringstream info;
		info << "Table Name      : " << Dam_CI_Point::point_table->get_table_name() << endl;
		info << "Table error info: " << query.lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
}
//Reswitch the applied-flag for the CI-points in the database table for the base state; a defined system state and his applied elements are decisive (static)
void Dam_CI_Point::reswitch_applied_flag_point_table(QSqlDatabase *ptr_database, const _sys_system_id id, const bool flag) {
	int number = 0;
	try {
		Dam_CI_Point::set_point_table(ptr_database);
	}
	catch (Error msg) {
		throw msg;
	}

	QSqlQueryModel results;
	ostringstream test_filter;
	test_filter << "Select ";
	test_filter << Dam_CI_Point::point_table->get_column_name(dam_label::point_id) << " , ";

	test_filter << " from " << Dam_CI_Point::point_table->get_table_name();
	test_filter << " where ";
	test_filter << Dam_CI_Point::point_table->get_column_name(label::applied_flag) << "= true";
	test_filter << " and ";
	test_filter << Dam_CI_Point::point_table->get_column_name(label::areastate_id) << " =" << id.area_state;
	test_filter << " and ";
	test_filter << Dam_CI_Point::point_table->get_column_name(label::measure_id) << " = " << id.measure_nr;

	Data_Base::database_request(&results, test_filter.str(), ptr_database);

	if (results.lastError().isValid() == true) {
		Error msg;
		msg.set_msg("Dam_CI_Point::reswitch_applied_flag_point_table(QSqlDatabase *ptr_database, const _sys_system_id id, const bool flag)", "Invalid database request", "Check the database", 2, false);
		ostringstream info;
		info << "Table Name      : " << Dam_CI_Point::point_table->get_table_name() << endl;
		info << "Table error info: " << results.lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}

	number = results.rowCount();
	int point_id = 0;
	_sys_system_id base;
	base.area_state = id.area_state;
	base.measure_nr = 0;

	for (int i = 0; i < number; i++) {
		point_id = results.record(i).value((Dam_CI_Point::point_table->get_column_name(dam_label::point_id)).c_str()).toInt();

		Dam_CI_Point::switch_applied_flag_point_table(ptr_database, base, point_id, flag);
	}
}
//Copy the results of a given system id to another one in database tables (static)
void Dam_CI_Point::copy_results(QSqlDatabase *ptr_database, const _sys_system_id src, const _sys_system_id dest) {
	//the table is set (the name and the column names) and allocated
	try {
		Dam_CI_Point::set_erg_table(ptr_database);
	}
	catch (Error msg) {
		throw msg;
	}

	int glob_id = 0;
	glob_id = Dam_CI_Point::point_erg_table->maximum_int_of_column(Dam_CI_Point::point_erg_table->get_column_name(dam_label::glob_id), ptr_database) + 1;

	QSqlQueryModel model;
	ostringstream test_filter;
	test_filter << "SELECT ";
	test_filter << Dam_CI_Point::point_erg_table->get_column_name(dam_label::glob_id);
	test_filter << " from " << Dam_CI_Point::point_erg_table->get_table_name();
	test_filter << " WHERE ";
	test_filter << Dam_CI_Point::point_erg_table->get_column_name(label::areastate_id) << " = " << src.area_state;
	test_filter << " AND ";
	test_filter << Dam_CI_Point::point_erg_table->get_column_name(label::measure_id) << " = " << src.measure_nr;

	//submit it to the datbase
	Data_Base::database_request(&model, test_filter.str(), ptr_database);
	if (model.lastError().isValid() == true) {
		Error msg;
		msg.set_msg("Dam_CI_Point::copy_results(QSqlDatabase *ptr_database, const _sys_system_id src, const _sys_system_id dest)", "Invalid database request", "Check the database", 2, false);
		ostringstream info;
		info << "Table Name      : " << Dam_CI_Point::point_erg_table->get_table_name() << endl;
		info << "Table error info: " << model.lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
	test_filter.str("");

	ostringstream cout;
	cout << "Copy " << model.rowCount() << " point results of the CI-system to the new measure state..." << endl;
	Sys_Common_Output::output_dam->output_txt(&cout);

	QSqlQueryModel model1;
	for (int i = 0; i < model.rowCount(); i++) {
		if (i % 10000 == 0 && i > 0) {
			cout << "Copy SC-point results " << i << " to " << i + 10000 << "..." << endl;
			Sys_Common_Output::output_dam->output_txt(&cout);
		}
		test_filter.str("");
		test_filter << "INSERT INTO " << Dam_CI_Point::point_erg_table->get_table_name();
		test_filter << " SELECT " << glob_id << " , ";
		test_filter << Dam_CI_Point::point_erg_table->get_column_name(dam_label::point_id) << " , ";
		test_filter << dest.area_state << " , ";
		test_filter << dest.measure_nr << " , ";
		test_filter << Dam_CI_Point::point_erg_table->get_column_name(label::applied_flag) << " , ";
		test_filter << Dam_CI_Point::point_erg_table->get_column_name(hyd_label::sz_bound_id) << " , ";
		test_filter << Dam_CI_Point::point_erg_table->get_column_name(risk_label::sz_break_id) << " , ";

		test_filter << Dam_CI_Point::point_erg_table->get_column_name(dam_label::sector_id) << " , ";
		test_filter << Dam_CI_Point::point_erg_table->get_column_name(dam_label::sector_level) << " , ";
		test_filter << Dam_CI_Point::point_erg_table->get_column_name(dam_label::failure_type) << " , ";
		test_filter << Dam_CI_Point::point_erg_table->get_column_name(dam_label::failure_duration) << " , ";
		test_filter << Dam_CI_Point::point_erg_table->get_column_name(dam_label::regular_flag) << " , ";



		test_filter << Dam_CI_Point::point_erg_table->get_column_name(dam_label::sc_point) << "  ";

		test_filter << " FROM " << Dam_CI_Point::point_erg_table->get_table_name() << " ";
		test_filter << " WHERE " << Dam_CI_Point::point_erg_table->get_column_name(dam_label::glob_id) << " = ";
		test_filter << model.record(i).value(Dam_CI_Point::point_erg_table->get_column_name(dam_label::glob_id).c_str()).toInt();
		Data_Base::database_request(&model1, test_filter.str(), ptr_database);
		if (model1.lastError().isValid() == true) {
			Error msg;
			msg.set_msg("Dam_CI_Point::copy_results(QSqlDatabase *ptr_database, const _sys_system_id src, const _sys_system_id dest)", "Invalid database request", "Check the database", 2, false);
			ostringstream info;
			info << "Table Name      : " << Dam_CI_Point::point_erg_table->get_table_name() << endl;
			info << "Table error info: " << model1.lastError().text().toStdString() << endl;
			msg.make_second_info(info.str());
			throw msg;
		}
		glob_id++;
	}
}
//Create the database table for the instationary results of the damage calculation per CI-point (static)
void Dam_CI_Point::create_instat_erg_table(QSqlDatabase *ptr_database) {
	if (Dam_CI_Point::point_instat_erg_table == NULL) {
		ostringstream cout;
		cout << "Create CI-point instationary result database table..." << endl;
		Sys_Common_Output::output_dam->output_txt(&cout);
		//make specific input for this class
		const string tab_name = dam_label::tab_ci_point_instat_erg;
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

		tab_col[1].name = dam_label::point_id;
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

		tab_col[8].name = dam_label::sector_level;
		tab_col[8].type = sys_label::tab_col_type_int;
		tab_col[8].default_value = "-1";

		tab_col[9].name = dam_label::active_flag;
		tab_col[9].type = sys_label::tab_col_type_bool;
		tab_col[9].default_value = "true";

		tab_col[10].name = dam_label::failure_type;
		tab_col[10].type = sys_label::tab_col_type_string;

		tab_col[11].name = dam_label::regular_flag;
		tab_col[11].type = sys_label::tab_col_type_bool;
		tab_col[11].default_value = "true";


		tab_col[12].name = dam_label::failure_duration;
		tab_col[12].type = sys_label::tab_col_type_double;
		tab_col[12].default_value = "0.0";

		tab_col[13].name = hyd_label::data_time;
		tab_col[13].type = sys_label::tab_col_type_string;
		tab_col[13].default_value = "";



		tab_col[14].name = dam_label::sc_point;
		tab_col[14].type = sys_label::tab_col_type_point;

		try {
			Dam_CI_Point::point_instat_erg_table = new Tables();
			if (Dam_CI_Point::point_instat_erg_table->create_non_existing_tables(tab_name, tab_col, num_col, ptr_database, _sys_table_type::dam) == false) {
				cout << " Table exists" << endl;
				Sys_Common_Output::output_dam->output_txt(&cout);
			};
		}
		catch (bad_alloc& t) {
			Error msg;
			msg.set_msg("Dam_CI_Point::create_instat_erg_table(QSqlDatabase *ptr_database)", "Can not allocate the memory", "Check the memory", 10, false);
			ostringstream info;
			info << "Info bad alloc: " << t.what() << endl;
			msg.make_second_info(info.str());
			throw msg;
		}
		catch (Error msg) {
			Dam_CI_Point::close_instat_erg_table();
			throw msg;
		}

		Dam_CI_Point::close_instat_erg_table();
	}


}
//Set the database table for the instationary results of the damage calculation per CI-point: it sets the table name and the name of the columns and allocate them (static)
void Dam_CI_Point::set_instat_erg_table(QSqlDatabase *ptr_database, const bool not_close) {
	if (Dam_CI_Point::point_instat_erg_table == NULL) {
		//make specific input for this class
		const string tab_id_name = dam_label::tab_ci_point_instat_erg;
		string tab_col[15];

		tab_col[0] = dam_label::glob_id;
		tab_col[1] = dam_label::point_id;
		tab_col[2] = label::areastate_id;
		tab_col[3] = label::measure_id;
		tab_col[4] = label::applied_flag;
		tab_col[5] = hyd_label::sz_bound_id;
		tab_col[6] = risk_label::sz_break_id;
		tab_col[7] = dam_label::sector_id;
		tab_col[8] = dam_label::active_flag;
		tab_col[9] = dam_label::sc_point; 
		tab_col[10] = hyd_label::data_time;
		tab_col[11] = dam_label::sector_level;
		tab_col[12] = dam_label::failure_type;
		tab_col[13] = dam_label::regular_flag;
		tab_col[14] = dam_label::failure_duration;

		try {
			Dam_CI_Point::point_instat_erg_table = new Tables(tab_id_name, tab_col, sizeof(tab_col) / sizeof(tab_col[0]));
			Dam_CI_Point::point_instat_erg_table->set_name(ptr_database, _sys_table_type::dam);
		}
		catch (bad_alloc& t) {
			Error msg;
			msg.set_msg("Dam_CI_Point::set_instat_erg_table(QSqlDatabase *ptr_database)", "Can not allocate the memory", "Check the memory", 10, false);
			ostringstream info;
			info << "Info bad alloc: " << t.what() << endl;
			msg.make_second_info(info.str());
			throw msg;
		}
		catch (Error msg) {
			if (not_close == false) {
				Dam_CI_Point::close_instat_erg_table();
			}
			throw msg;
		}
	}
}
//Delete all data in the database table for the instationary results of the damage calculation per CI-point (static)
void Dam_CI_Point::delete_data_in_instat_erg_table(QSqlDatabase *ptr_database) {
	//the table is set (the name and the column names) and allocated
	try {
		Dam_CI_Point::set_instat_erg_table(ptr_database);
	}
	catch (Error msg) {
		throw msg;
	}
	//delete the table
	Dam_CI_Point::point_instat_erg_table->delete_data_in_table(ptr_database);
}
//Delete the data in the database table for the instationary results of the damage calculation specified by the system state (static)
void Dam_CI_Point::delete_data_in_instat_erg_table(QSqlDatabase *ptr_database, const _sys_system_id id) {
	//the table is set (the name and the column names) and allocated
	try {
		Dam_CI_Point::set_instat_erg_table(ptr_database);
	}
	catch (Error msg) {
		throw msg;
	}

	//delete the table
	QSqlQuery query(*ptr_database);
	ostringstream query_string;
	query_string << "DELETE FROM " << Dam_CI_Point::point_instat_erg_table->get_table_name();
	query_string << " WHERE ";
	query_string << Dam_CI_Point::point_instat_erg_table->get_column_name(label::areastate_id) << " = " << id.area_state;
	query_string << " AND ";
	query_string << Dam_CI_Point::point_instat_erg_table->get_column_name(label::measure_id) << " = " << id.measure_nr;

	Data_Base::database_request(&query, query_string.str(), ptr_database);
	if (query.lastError().isValid() == true) {
		Error msg;
		msg.set_msg("Dam_CI_Point::delete_data_in_instat_erg_table(QSqlDatabase *ptr_database, const _sys_system_id id)", "Invalid database request", "Check the database", 2, false);
		ostringstream info;
		info << "Table Name      : " << Dam_CI_Point::point_instat_erg_table ->get_table_name() << endl;
		info << "Table error info: " << query.lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
}
//Delete the data in the database table for the instationary results of the damage calculation specified parameters (static)
void Dam_CI_Point::delete_data_in_instat_erg_table(QSqlDatabase *ptr_database, const _sys_system_id id, const string break_sz, const bool like_flag) {
	//the table is set (the name and the column names) and allocated
	try {
		Dam_CI_Point::set_instat_erg_table(ptr_database);
	}
	catch (Error msg) {
		throw msg;
	}

	//delete the table
	QSqlQuery query(*ptr_database);
	ostringstream query_string;
	query_string << "DELETE FROM " << Dam_CI_Point::point_instat_erg_table->get_table_name();
	query_string << " WHERE ";
	query_string << Dam_CI_Point::point_instat_erg_table->get_column_name(label::areastate_id) << " = " << id.area_state;
	query_string << " AND ";
	query_string << Dam_CI_Point::point_instat_erg_table->get_column_name(label::measure_id) << " = " << id.measure_nr;
	query_string << " AND ";
	if (like_flag == false) {
		query_string << Dam_CI_Point::point_instat_erg_table->get_column_name(risk_label::sz_break_id) << " = '" << break_sz << "'";
	}
	else {
		query_string << Dam_CI_Point::point_instat_erg_table->get_column_name(risk_label::sz_break_id) << " LIKE '" << break_sz << "'";
	}

	Data_Base::database_request(&query, query_string.str(), ptr_database);
	if (query.lastError().isValid() == true) {
		Error msg;
		msg.set_msg("Dam_CI_Point::delete_data_in_instat_erg_table(QSqlDatabase *ptr_database, const _sys_system_id id, const string break_sz, const bool like_flag)", "Invalid database request", "Check the database", 2, false);
		ostringstream info;
		info << "Table Name      : " << Dam_CI_Point::point_instat_erg_table->get_table_name() << endl;
		info << "Table error info: " << query.lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
}
//Delete the data in the database table for the instationary results of the damage calculation specified by the system state and the scenario-ids (static)
void Dam_CI_Point::delete_data_in_instat_erg_table(QSqlDatabase *ptr_database, const _sys_system_id id, const int bound_sz, const string break_sz) {
	//the table is set (the name and the column names) and allocated
	try {
		Dam_CI_Point::set_instat_erg_table(ptr_database);
	}
	catch (Error msg) {
		throw msg;
	}

	//delete the table
	QSqlQuery query(*ptr_database);
	ostringstream query_string;
	query_string << "DELETE FROM " << Dam_CI_Point::point_instat_erg_table->get_table_name();
	query_string << " WHERE ";
	query_string << Dam_CI_Point::point_instat_erg_table->get_column_name(label::areastate_id) << " = " << id.area_state;
	query_string << " AND ";
	query_string << Dam_CI_Point::point_instat_erg_table->get_column_name(label::measure_id) << " = " << id.measure_nr;
	query_string << " AND ";
	query_string << Dam_CI_Point::point_instat_erg_table->get_column_name(hyd_label::sz_bound_id) << " = " << bound_sz;
	query_string << " AND ";
	query_string << Dam_CI_Point::point_instat_erg_table->get_column_name(risk_label::sz_break_id) << " = '" << break_sz << "'";

	Data_Base::database_request(&query, query_string.str(), ptr_database);
	if (query.lastError().isValid() == true) {
		Error msg;
		msg.set_msg("Dam_CI_Point::delete_data_in_instat_erg_table(QSqlDatabase *ptr_database, const _sys_system_id id, const int bound_sz, const string break_sz )", "Invalid database request", "Check the database", 2, false);
		ostringstream info;
		info << "Table Name      : " << Dam_CI_Point::point_instat_erg_table->get_table_name() << endl;
		info << "Table error info: " << query.lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
}
//Delete the data in the database table for the instationary results of the damage calculation specified by the hydraulic scenario-ids (static)
void Dam_CI_Point::delete_data_in_instat_erg_table(QSqlDatabase *ptr_database, const int bound_sz) {
	//the table is set (the name and the column names) and allocated
	try {
		Dam_CI_Point::set_instat_erg_table(ptr_database);
	}
	catch (Error msg) {
		throw msg;
	}

	//delete the table
	QSqlQuery query(*ptr_database);
	ostringstream query_string;
	query_string << "DELETE FROM " << Dam_CI_Point::point_instat_erg_table->get_table_name();
	query_string << " WHERE ";
	query_string << Dam_CI_Point::point_instat_erg_table->get_column_name(hyd_label::sz_bound_id) << " = " << bound_sz;

	Data_Base::database_request(&query, query_string.str(), ptr_database);
	if (query.lastError().isValid() == true) {
		Error msg;
		msg.set_msg("Dam_CI_Point::delete_data_in_instat_erg_table(QSqlDatabase *ptr_database, const int bound_sz)", "Invalid database request", "Check the database", 2, false);
		ostringstream info;
		info << "Table Name      : " << Dam_CI_Point::point_instat_erg_table->get_table_name() << endl;
		info << "Table error info: " << query.lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
}
//Close and delete the database table for the instationary results of the damage calculation per CI-point (static)
void Dam_CI_Point::close_instat_erg_table(void) {
	if (Dam_CI_Point::point_instat_erg_table != NULL) {
		delete Dam_CI_Point::point_instat_erg_table;
		Dam_CI_Point::point_instat_erg_table = NULL;
	}
}
//Get the maximum value of the global index of the instationary result CI-point database table (static)
int Dam_CI_Point::get_max_glob_id_point_instat_erg_table(QSqlDatabase *ptr_database) {
	try {
		Dam_CI_Point::set_instat_erg_table(ptr_database);
	}
	catch (Error msg) {
		throw msg;
	}

	//evaluate the global identifier
	int id_glob = Dam_CI_Point::point_instat_erg_table->maximum_int_of_column(Dam_CI_Point::point_instat_erg_table->get_column_name(dam_label::glob_id), ptr_database);
	return id_glob;
}
//Get the header for inserting the CI-point instationary result data to database table (static)
string Dam_CI_Point::get_insert_header_instat_erg_table(QSqlDatabase *ptr_database) {
	try {
		Dam_CI_Point::set_instat_erg_table(ptr_database);
	}
	catch (Error msg) {
		throw msg;
	}
	//set the query via a query string
	ostringstream query_string;
	query_string << "INSERT INTO  " << Dam_CI_Point::point_instat_erg_table->get_table_name();
	query_string << " ( ";
	query_string << Dam_CI_Point::point_instat_erg_table->get_column_name(dam_label::glob_id) << " , ";
	query_string << Dam_CI_Point::point_instat_erg_table->get_column_name(dam_label::point_id) << " , ";
	query_string << Dam_CI_Point::point_instat_erg_table->get_column_name(label::areastate_id) << " , ";
	query_string << Dam_CI_Point::point_instat_erg_table->get_column_name(label::measure_id) << " , ";
	query_string << Dam_CI_Point::point_instat_erg_table->get_column_name(hyd_label::sz_bound_id) << " , ";
	query_string << Dam_CI_Point::point_instat_erg_table->get_column_name(risk_label::sz_break_id) << " , ";
	//results
	query_string << Dam_CI_Point::point_instat_erg_table->get_column_name(dam_label::sector_id) << " , ";
	query_string << Dam_CI_Point::point_instat_erg_table->get_column_name(dam_label::sector_level) << " , ";
	query_string << Dam_CI_Point::point_instat_erg_table->get_column_name(dam_label::active_flag) << " , ";
	query_string << Dam_CI_Point::point_instat_erg_table->get_column_name(dam_label::failure_type) << " , ";
	query_string << Dam_CI_Point::point_instat_erg_table->get_column_name(dam_label::regular_flag) << " , ";
	query_string << Dam_CI_Point::point_instat_erg_table->get_column_name(dam_label::failure_duration) << " , ";
	query_string << Dam_CI_Point::point_instat_erg_table->get_column_name(hyd_label::data_time) << " , ";

	query_string << Dam_CI_Point::point_instat_erg_table->get_column_name(dam_label::sc_point) << ")";

	query_string << " VALUES ";
	string buffer;
	buffer = query_string.str();
	return buffer;
}
//Copy the instationaryresults of a given system id to another one in database tables per CI-point(static) 
void Dam_CI_Point::copy_instat_results(QSqlDatabase *ptr_database, const _sys_system_id src, const _sys_system_id dest) {
	//the table is set (the name and the column names) and allocated
	try {
		Dam_CI_Point::set_instat_erg_table(ptr_database);
	}
	catch (Error msg) {
		throw msg;
	}

	int glob_id = 0;
	glob_id = Dam_CI_Point::point_instat_erg_table->maximum_int_of_column(Dam_CI_Point::point_instat_erg_table->get_column_name(dam_label::glob_id), ptr_database) + 1;

	QSqlQueryModel model;
	ostringstream test_filter;
	test_filter << "SELECT ";
	test_filter << Dam_CI_Point::point_instat_erg_table->get_column_name(dam_label::glob_id);
	test_filter << " from " << Dam_CI_Point::point_instat_erg_table->get_table_name();
	test_filter << " WHERE ";
	test_filter << Dam_CI_Point::point_instat_erg_table->get_column_name(label::areastate_id) << " = " << src.area_state;
	test_filter << " AND ";
	test_filter << Dam_CI_Point::point_instat_erg_table->get_column_name(label::measure_id) << " = " << src.measure_nr;

	//submit it to the datbase
	Data_Base::database_request(&model, test_filter.str(), ptr_database);
	if (model.lastError().isValid() == true) {
		Error msg;
		msg.set_msg("Dam_CI_Point::copy_instat_results(QSqlDatabase *ptr_database, const _sys_system_id src, const _sys_system_id dest)", "Invalid database request", "Check the database", 2, false);
		ostringstream info;
		info << "Table Name      : " << Dam_CI_Point::point_instat_erg_table->get_table_name() << endl;
		info << "Table error info: " << model.lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
	test_filter.str("");

	ostringstream cout;
	cout << "Copy " << model.rowCount() << " instationary point results of the CI-system to the new measure state..." << endl;
	Sys_Common_Output::output_dam->output_txt(&cout);

	QSqlQueryModel model1;
	for (int i = 0; i < model.rowCount(); i++) {
		if (i % 10000 == 0 && i > 0) {
			cout << "Copy SC-point results " << i << " to " << i + 10000 << "..." << endl;
			Sys_Common_Output::output_dam->output_txt(&cout);
		}
		test_filter.str("");
		test_filter << "INSERT INTO " << Dam_CI_Point::point_instat_erg_table->get_table_name();
		test_filter << " SELECT " << glob_id << " , ";
		test_filter << Dam_CI_Point::point_instat_erg_table->get_column_name(dam_label::point_id) << " , ";
		test_filter << dest.area_state << " , ";
		test_filter << dest.measure_nr << " , ";
		test_filter << Dam_CI_Point::point_instat_erg_table->get_column_name(label::applied_flag) << " , ";
		test_filter << Dam_CI_Point::point_instat_erg_table->get_column_name(hyd_label::sz_bound_id) << " , ";
		test_filter << Dam_CI_Point::point_instat_erg_table->get_column_name(risk_label::sz_break_id) << " , ";

		test_filter << Dam_CI_Point::point_instat_erg_table->get_column_name(dam_label::sector_id) << " , ";
		test_filter << Dam_CI_Point::point_instat_erg_table->get_column_name(dam_label::sector_level) << " , ";
		test_filter << Dam_CI_Point::point_instat_erg_table->get_column_name(dam_label::active_flag) << " , ";
		test_filter << Dam_CI_Point::point_instat_erg_table->get_column_name(dam_label::regular_flag) << " , ";
		test_filter << Dam_CI_Point::point_instat_erg_table->get_column_name(dam_label::failure_duration) << " , ";
		test_filter << Dam_CI_Point::point_instat_erg_table->get_column_name(dam_label::failure_type) << " , ";
		test_filter << Dam_CI_Point::point_instat_erg_table->get_column_name(hyd_label::data_time) << " , ";


		test_filter << Dam_CI_Point::point_instat_erg_table->get_column_name(dam_label::sc_point) << "  ";

		test_filter << " FROM " << Dam_CI_Point::point_instat_erg_table->get_table_name() << " ";
		test_filter << " WHERE " << Dam_CI_Point::point_instat_erg_table->get_column_name(dam_label::glob_id) << " = ";
		test_filter << model.record(i).value(Dam_CI_Point::point_instat_erg_table->get_column_name(dam_label::glob_id).c_str()).toInt();
		Data_Base::database_request(&model1, test_filter.str(), ptr_database);
		if (model1.lastError().isValid() == true) {
			Error msg;
			msg.set_msg("Dam_CI_Point::copy_instat_results(QSqlDatabase *ptr_database, const _sys_system_id src, const _sys_system_id dest)", "Invalid database request", "Check the database", 2, false);
			ostringstream info;
			info << "Table Name      : " << Dam_CI_Point::point_instat_erg_table->get_table_name() << endl;
			info << "Table error info: " << model1.lastError().text().toStdString() << endl;
			msg.make_second_info(info.str());
			throw msg;
		}
		glob_id++;
	}
}
//Get the boundary value
double Dam_CI_Point::get_boundary_value(void) {
	return this->boundary_value;
}
//Get sector level
int Dam_CI_Point::get_sec_level(void) {
	return this->sector_level;

}
//Set the sector id, sector level, boundary value, recovery time, final flag
void Dam_CI_Point::set_members(const int sector_id, const int sector_level, const double boundary, const double recovery_time, const bool final_flag, const bool regular, const double activation_time) {
	this->sector_id = sector_id;
	this->sector_name = this->convert_sector_id2txt(this->convert_id2enum(this->sector_id));
	this->sector_level = sector_level;
	this->boundary_value = boundary;
	this->recovery_time = recovery_time;
	this->final_flag = final_flag;
	this->regular_flag = regular;
	this->activation_time = activation_time;
	this->orig_activation_time = activation_time;
	this->orig_recovery_time= recovery_time;
	if (this->regular_flag == false) {
		this->active_flag = false;
	}

	this->check_members();

}
//Get the data-string to complete a insert-string for inserting the data of the point to database
string Dam_CI_Point::get_datastring_members2database(const int global_id) {
	string buffer = label::not_set;
	this->global_index = global_id;


	ostringstream query_string;
	query_string << " ( ";
	query_string << global_id << " , ";
	query_string << this->number << " , ";
	query_string << this->system_id.area_state << " , ";
	query_string << this->system_id.measure_nr << " , ";
	query_string << "true" << " , ";
	//data
	query_string << "'" << this->name << "' , ";
	query_string << this->sector_id << " , ";
	query_string << "'" << this->sector_name<< "' , ";
	query_string << this->sector_level << " , ";
	query_string << this->boundary_value << " , ";
	query_string << this->recovery_time<< " , ";
	query_string << "'" << functions::convert_boolean2string(this->regular_flag) << "' , ";
	query_string << this->activation_time << " , ";
	query_string << "'" << functions::convert_boolean2string(this->final_flag) << "' , ";
	query_string << FORMAT_FIXED_REAL << P(8);
	query_string << this->get_xcoordinate() << " , ";
	query_string << this->get_ycoordinate() << " , ";
	query_string.clear();

	query_string << this->get_point2sql_string(this->x_coordinate, this->y_coordinate) << " ) ";

	buffer = query_string.str();
	return buffer;
}
//Get the data-string to complete a insert-string for inserting the data of the point results to database
string Dam_CI_Point::get_datastring_results2database(const int global_id, const int bound_sz, const string break_sz, const bool must_output) {
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
	query_string << this->number << " , ";
	query_string << this->system_id.area_state << " , ";
	query_string << this->system_id.measure_nr << " , ";
	query_string << bound_sz << " , ";
	query_string << "'" << break_sz << "' , ";
	query_string << this->sector_id << " , ";
	query_string << this->sector_level << " , ";
	query_string << "'" << this->failure_type << "' , ";
	query_string << this->failure_duration << " , ";
	query_string << "'" << functions::convert_boolean2string(this->regular_flag) << "' , ";
	query_string << this->get_point2sql_string(this->x_coordinate, this->y_coordinate) << " ) ";

	buffer = query_string.str();
	return buffer;




}
//Get the data-string to complete a insert-string for inserting the data of the point instationary results to database
string Dam_CI_Point::get_datastring_instat_results2database(const int global_id, const int bound_sz, const string break_sz, string date_time, const bool must_output) {
	string buffer = label::not_set;
	if (must_output == false) {
		if (this->was_affected == false) {
			return buffer;
		}
		if (this->failure_type_enum == _dam_ci_failure_type::no_failure && this->regular_flag == true) {
			return buffer;
		}
		if (this->active_flag ==false && this->regular_flag == false) {
			return buffer;
		}
	}

	//set the query via a query string
	ostringstream query_string;
	query_string << " ( ";
	query_string << global_id << " , ";
	query_string << this->number << " , ";
	query_string << this->system_id.area_state << " , ";
	query_string << this->system_id.measure_nr << " , ";
	query_string << bound_sz << " , ";
	query_string << "'" << break_sz << "' , ";
	query_string << this->sector_id << " , ";
	query_string << this->sector_level << " , ";
	query_string << "'" << functions::convert_boolean2string(this->active_flag) << "' , ";
	query_string << "'" << this->failure_type << "' , ";
	query_string << "'" << functions::convert_boolean2string(this->regular_flag) << "' , ";
	query_string << this->failure_duration << " , ";
	query_string << "'" << date_time << "' , ";

	query_string << this->get_point2sql_string(this->x_coordinate, this->y_coordinate) << " ) ";

	buffer = query_string.str();
	return buffer;

}
///Get the data-string to complete a insert-string for inserting the data of the connection results to database
string Dam_CI_Point::get_datastring_conect_results2database(int *global_id, const int bound_sz, const string break_sz, const bool must_output) {
	string buffer = label::not_set;
	if (must_output == false) {
		if (this->was_affected == false) {
			return buffer;
		}
	}

	//set the query via a query string
	ostringstream query_string;
	for (int i = 0; i < this->no_outgoing; i++) {
		query_string << " ( ";
		query_string << *global_id << " , ";
		query_string << this->system_id.area_state << " , ";
		query_string << this->system_id.measure_nr << " , ";
		query_string << bound_sz << " , ";
		query_string << "'" << break_sz << "' , ";
		query_string << this->get_ptr_point()->get_number() << " , ";
		query_string <<  0 << " , ";
		query_string << this->sector_id << " , ";
		query_string << this->sector_level << " , ";
		query_string << this->outgoing[i]->get_ptr_point()->get_number() << " , ";
		query_string << this->outgoing[i]->get_is_point_id() << " , ";

		query_string << "'" << this->failure_type << "' , ";
		query_string << "'" << functions::convert_boolean2string(this->regular_flag) << "' , ";

		//Polyline gemacht werden


		query_string << Geo_Polysegment::get_line2sql_string(this->x_coordinate, this->y_coordinate, this->outgoing[i]->get_ptr_point()->get_xcoordinate(), this->outgoing[i]->get_ptr_point()->get_ycoordinate()) << " ) ";
		if (i < this->no_outgoing - 1) {
			query_string << ", ";
		}

		(*global_id)++;
	}

	buffer = query_string.str();
	return buffer;


}
///Get the data-string to complete a insert-string for inserting the data of the connection instationary results to database
string Dam_CI_Point::get_datastring_conect_instat_results2database(int *global_id, const int bound_sz, const string break_sz, string date_time, const bool must_output) {
	string buffer = label::not_set;
	if (must_output == false) {
		if (this->was_affected == false) {
			return buffer;
		}
		if (this->failure_type_enum == _dam_ci_failure_type::no_failure && this->regular_flag == true) {
			return buffer;
		}
		if (this->active_flag == false && this->regular_flag == false) {
			return buffer;
		}
	}

	ostringstream query_string;
	for (int i = 0; i < this->no_outgoing; i++) {
		query_string << " ( ";
		query_string << *global_id << " , ";
		query_string << this->system_id.area_state << " , ";
		query_string << this->system_id.measure_nr << " , ";
		query_string << bound_sz << " , ";
		query_string << "'" << break_sz << "' , ";
		query_string << this->get_ptr_point()->get_number() << " , ";
		query_string << 0 << " , ";
		query_string << this->sector_id << " , ";
		query_string << this->sector_level << " , ";
		query_string << this->outgoing[i]->get_ptr_point()->get_number() << " , ";
		query_string << this->outgoing[i]->get_is_point_id() << " , ";
		query_string << "'" << this->failure_type << "' , ";
		query_string << "'" << functions::convert_boolean2string(this->regular_flag) << "' , ";
		query_string << "'" << date_time << "' , ";
		


		query_string << Geo_Polysegment::get_line2sql_string(this->x_coordinate, this->y_coordinate, this->outgoing[i]->get_ptr_point()->get_xcoordinate(), this->outgoing[i]->get_ptr_point()->get_ycoordinate()) << " )  ";
		if (i < this->no_outgoing - 1) {
			query_string << ", ";
		}
		(*global_id)++;
	}

	buffer = query_string.str();
	return buffer;
}
//Calculate direct damages
void Dam_CI_Point::calculate_direct_damages(Dam_Impact_Values *impact) {
	

	if (impact->get_was_wet_flag() == false) {
		return;
	}
	if (this->boundary_value < 0.0) {
		return;
	}

	if (impact->get_impact_h() >= this->boundary_value) {
		this->was_affected = true;
		this->active_flag = false;
		
		this->failure_type_enum = _dam_ci_failure_type::direct;
		this->failure_type = _Dam_CI_Element::convert_failuretype2txt(this->failure_type_enum); 
		
		this->failure_duration = this->recovery_time;
		
	}

}
//Calculate direct damages instationary
void Dam_CI_Point::calculate_direct_damages_instationary(Dam_Impact_Values *impact, const double time) {
	if (impact->get_was_wet_flag() == false && this->failure_type_enum== _dam_ci_failure_type::no_failure && this->regular_flag ==true) {
		return;
	}
	if (this->boundary_value < 0.0 && this->regular_flag == true) {
		return;
	}
	if (this->regular_flag == false && this->active_flag==false) {
		return;
	}
	if (this->failure_type_enum == _dam_ci_failure_type::direct) {
		this->recovery_time = this->recovery_time - time / (constant::day_second);
		
		this->failure_duration = this->recovery_time;

		
		if (this->recovery_time <= 0.0 + constant::sec_epsilon) {
			//this->was_affected = true;
			this->active_flag = true;
			this->recovery_time = 0.0;
			this->failure_duration = 0.0;
			this->boundary_value = - 999;
			this->failure_type_enum = _dam_ci_failure_type::no_failure;
			this->last_instat_required = true;
			this->failure_type = _Dam_CI_Element::convert_failuretype2txt(this->failure_type_enum);
		}

	}
	else {
		if (impact->get_impact_h() >= this->boundary_value && this->was_affected ==false) {
			this->was_affected = true;
			this->active_flag = false;

			this->failure_type_enum = _dam_ci_failure_type::direct;
			this->failure_type = _Dam_CI_Element::convert_failuretype2txt(this->failure_type_enum);
			this->failure_duration = this->recovery_time;
			
		}
	}



}
//Check the points
void Dam_CI_Point::check_members(void) {

	//check sec_id
	if (this->sector_name == label::not_defined) {
		Error msg = this->set_error(0);
		ostringstream info;
		info << "Name         :" << this->name << endl;
		info << "Sector id    :" << this->sector_id << endl;
		info << "Sector level :" << this->sector_level << endl;
		msg.make_second_info(info.str());
		throw msg;
	}

	//check point in 1-4 sector no endflag
	if (this->sector_id <= 4 && this->final_flag == true) {
		Error msg = this->set_error(2);
		ostringstream info;
		info << "Name         :" << this->name << endl;
		info << "Sector id    :" << this->sector_id << endl;
		info << "Sector level :" << this->sector_level << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
	//recovery > 0
	if (this->recovery_time <= 0.0) {
		Warning msg = this->set_warning(0);
		ostringstream info;
		info << "Name         :" << this->name << endl;
		info << "Sector id    :" << this->sector_id << endl;
		info << "Sector name  :" << this->sector_name << endl;
		info << "Sector level :" << this->sector_level << endl;
		msg.make_second_info(info.str());
		this->recovery_time = 0.01;
		msg.output_msg(4);
	}
	if (this->activation_time < 0.0) {
		Warning msg = this->set_warning(1);
		ostringstream info;
		info << "Name         :" << this->name << endl;
		info << "Sector id    :" << this->sector_id << endl;
		info << "Sector name  :" << this->sector_name << endl;
		info << "Sector level :" << this->sector_level << endl;
		msg.make_second_info(info.str());
		this->activation_time = 0.0;
		msg.output_msg(4);
	}
	//activation > recovery> 0
	if (this->regular_flag == false && this->recovery_time < this->activation_time) {
		Warning msg = this->set_warning(2);
		ostringstream info;
		info << "Name         :" << this->name << endl;
		info << "Sector id    :" << this->sector_id << endl;
		info << "Sector name  :" << this->sector_name << endl;
		info << "Sector level :" << this->sector_level << endl;
		msg.make_second_info(info.str());
		this->recovery_time = this->activation_time;
		msg.output_msg(4);
	}
	//emergency just 1-10
	if (this->regular_flag == false && this->sector_id > 9) {
		Error msg = this->set_error(1);
		ostringstream info;
		info << "Name         :" << this->name << endl;
		info << "Sector id    :" << this->sector_id << endl;
		info << "Sector name  :" << this->sector_name << endl;
		info << "Sector level :" << this->sector_level << endl;
		msg.make_second_info(info.str());
		throw msg;
	}


}

//____________
//private
//Set the warning
Warning Dam_CI_Point::set_warning(const int warn_type) {
	string place = "Dam_CI_Point::";
	string help;
	string reason;
	string reaction;
	int type = 0;
	Warning msg;
	stringstream info;

	switch (warn_type) {
	case 0://recovery time<0
		place.append("check_members(void)");
		reason = "The recovery time for a CI-structure is <= 0.0 d; it must be > 0.0 d";
		reaction = "The recovery time is set to 0.01 d ";
		help = "Check CI-point data";
		type = 27;
		break;
	case 1://activation time<0
		place.append("check_members(void)");
		reason = "The activation time for a CI-structure is < 0.0 d; it must be >= 0.0 d";
		reaction = "The activation time is set to 0.0 d ";
		help = "Check CI-point data";
		type = 27;
		break;
	case 2://activation time>recovery
		place.append("check_members(void)");
		reason = "The activation time for a CI-structure is > the recovery time; the recovery time includes the activation time";
		reaction = "The recovery time is set to activation time";
		help = "Check CI-point data";
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
Error Dam_CI_Point::set_error(const int err_type) {
	string place = "Dam_CI_Point::";
	string help;
	string reason;
	int type = 0;
	bool fatal = false;
	stringstream info;
	Error msg;

	switch (err_type) {
	case 0://sec_type
		place.append("check_members(void)");
		reason = "The CI-sector is not defined; sectors are available between 1 - 4 and 10 - 19";
		help = "Check CI-point data";
		type = 34;
		break;
	case 1://sec_type
		place.append("check_members(void)");
		reason = "A emergency CI-point needs to have an sector type between 1 - 4!";
		help = "Check CI-point data";
		type = 34;
		break;
	case 2://sec_type
		place.append("check_members(void)");
		reason = "A CI-point between sector 1 - 4 are not allowed to be an enduser; it needs a connection to a CI-polygon!";
		help = "Check CI-point data";
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

