#include "source_code\Dam_Headers_Precompiled.h"
//#include "Dam_CI_Point.h"
//init static members
Tables *Dam_CI_Point::point_table = NULL;
Tables *Dam_CI_Point::point_erg_table = NULL;
Tables *Dam_CI_Point::point_instat_erg_table = NULL;

//Default constructor
Dam_CI_Point::Dam_CI_Point(void){
	
}

//Default destructor
Dam_CI_Point::~Dam_CI_Point(void){
}
//__________
//public
void Dam_CI_Point::input_point_perdatabase(const QSqlQueryModel *results, const int glob_index) {
	try {
		//todo
		this->number = results->record(glob_index).value((Dam_CI_Point::point_table->get_column_name(dam_label::point_id)).c_str()).toInt();
		//set the element members
		//this->cat_id = results->record(glob_index).value((Dam_CI_Point::point_table->get_column_name(dam_label::category_id)).c_str()).toInt();
		//this->score_value = results->record(glob_index).value((Dam_CI_Point::point_table->get_column_name(dam_label::sc_score)).c_str()).toDouble();
		//this->boundary_value = results->record(glob_index).value((Dam_CI_Point::point_table->get_column_name(dam_label::boundary_value)).c_str()).toDouble();
		//this->subcat_id = results->record(glob_index).value((Dam_CI_Point::point_table->get_column_name(dam_label::subcategory_id)).c_str()).toInt();
		//this->name = results->record(glob_index).value((Dam_CI_Point::point_table->get_column_name(dam_label::point_name)).c_str()).toString().toStdString();
		//this->is_connected = results->record(glob_index).value((Dam_CI_Point::point_table->get_column_name(dam_label::raster_connected)).c_str()).toBool();
		//this->index_fp = results->record(glob_index).value((Dam_CI_Point::point_table->get_column_name(dam_label::conn_fp_id)).c_str()).toInt();
		//this->index_fp_elem = results->record(glob_index).value((Dam_CI_Point::point_table->get_column_name(dam_label::conn_fp_elem_id)).c_str()).toInt();

		double x_mid = 0.0;
		double y_mid = 0.0;
		x_mid = results->record(glob_index).value((Dam_CI_Point::point_table->get_column_name(dam_label::elem_mid_x)).c_str()).toDouble();
		y_mid = results->record(glob_index).value((Dam_CI_Point::point_table->get_column_name(dam_label::elem_mid_y)).c_str()).toDouble();
		this->set_point_coordinate(x_mid, y_mid);
		this->global_index = results->record(glob_index).value((Dam_CI_Point::point_table->get_column_name(dam_label::glob_id)).c_str()).toInt();

		//this->category = this->convert_id2enum(this->cat_id);
		//this->check_members();
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
//Create the database table for the CI points (static)
void Dam_CI_Point::create_point_table(QSqlDatabase *ptr_database) {
	if (Dam_CI_Point::point_table == NULL) {
		ostringstream cout;
		cout << "Create CI point data database table..." << endl;
		Sys_Common_Output::output_dam->output_txt(&cout);
		//make specific input for this class
		const string tab_name = dam_label::tab_ci_point;
		const int num_col = 18;
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

		tab_col[10].name = dam_label::final_flag;
		tab_col[10].type = sys_label::tab_col_type_bool;
		tab_col[10].default_value = "false";


		tab_col[11].name = dam_label::elem_mid_x;
		tab_col[11].type = sys_label::tab_col_type_double;

		tab_col[12].name = dam_label::elem_mid_y;
		tab_col[12].type = sys_label::tab_col_type_double;

		tab_col[13].name = dam_label::sc_point;
		tab_col[13].type = sys_label::tab_col_type_point;

		tab_col[14].name = dam_label::conn_fp_id;
		tab_col[14].type = sys_label::tab_col_type_int;
		tab_col[14].default_value = "-1";

		tab_col[15].name = dam_label::conn_fp_elem_id;
		tab_col[15].type = sys_label::tab_col_type_int;
		tab_col[15].default_value = "-1";

		tab_col[16].name = dam_label::raster_connected;
		tab_col[16].type = sys_label::tab_col_type_bool;
		tab_col[16].default_value = "false";

		tab_col[17].name = label::description;
		tab_col[17].type = sys_label::tab_col_type_string;

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
void Dam_CI_Point::set_point_table(QSqlDatabase *ptr_database) {
	if (Dam_CI_Point::point_table == NULL) {
		//make specific input for this class
		const string tab_id_name = dam_label::tab_ci_point;
		string tab_col[18];

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
			Dam_CI_Point::close_point_table();
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
	query_string << Dam_CI_Point::point_table->get_column_name(dam_label::final_flag) << " , ";
	query_string << Dam_CI_Point::point_table->get_column_name(dam_label::elem_mid_x) << " , ";
	query_string << Dam_CI_Point::point_table->get_column_name(dam_label::elem_mid_y) << " , ";
	query_string << Dam_CI_Point::point_table->get_column_name(dam_label::sc_point) << " ) ";


	query_string << " VALUES ";
	string buffer;
	buffer = query_string.str();
	return buffer;
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
	case 0://the type of thread is not specified
		place.append("run(void)");
		reason = "The thread type is unknown";
		reaction = "No thread is launched";
		help = "Check the source code";
		type = 5;
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

