#include "Dam_Headers_Precompiled.h"
#include "Dam_CI_Element_List.h"

//init static members
Tables *Dam_CI_Element_List::connection_table = NULL;
Tables *Dam_CI_Element_List::connection_erg_table = NULL;
Tables *Dam_CI_Element_List::connection_instat_erg_table = NULL;

//Default constructor
Dam_CI_Element_List::Dam_CI_Element_List(void){

	//count the memory
	Sys_Memory_Count::self()->add_mem(sizeof(Dam_CI_Element_List)+ sizeof(QList<QList<int>>), _sys_system_modules::DAM_SYS);
}
//Default destructor
Dam_CI_Element_List::~Dam_CI_Element_List(void){
	//count the memory
	Sys_Memory_Count::self()->minus_mem(sizeof(Dam_CI_Element_List) + sizeof(QList<QList<int>>), _sys_system_modules::DAM_SYS);
}
//__________
//public
//Input CI connection data with database
void Dam_CI_Element_List::input_connection_perdatabase(const QSqlQueryModel *results, const int glob_index) {
	try {

		QList<int> list_buff;


		list_buff.append(results->record(glob_index).value((Dam_CI_Element_List::connection_table->get_column_name(dam_label::in_id)).c_str()).toInt());
		list_buff.append(results->record(glob_index).value((Dam_CI_Element_List::connection_table->get_column_name(dam_label::in_point_flag)).c_str()).toInt());
		list_buff.append(results->record(glob_index).value((Dam_CI_Element_List::connection_table->get_column_name(dam_label::out_id)).c_str()).toInt());
		list_buff.append(results->record(glob_index).value((Dam_CI_Element_List::connection_table->get_column_name(dam_label::out_point_flag)).c_str()).toInt());

		this->elem_list.append(list_buff);

		

	}
	catch (Error msg) {
		ostringstream info;
		info << "Connection index           : " << this->get_number_connection()+1 << endl;
		msg.make_second_info(info.str());
		throw msg;
	}


}
//Create the database table for the CI connections (static)
void Dam_CI_Element_List::create_connection_table(QSqlDatabase *ptr_database) {
	if (Dam_CI_Element_List::connection_table == NULL) {
		ostringstream cout;
		cout << "Create CI connection data database table..." << endl;
		Sys_Common_Output::output_dam->output_txt(&cout);
		//make specific input for this class
		const string tab_name = dam_label::tab_ci_connection;
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

		tab_col[4].name = dam_label::in_id;
		tab_col[4].type = sys_label::tab_col_type_int;
		tab_col[4].default_value = "-1";

		tab_col[5].name = dam_label::in_point_flag;
		tab_col[5].type = sys_label::tab_col_type_int;
		tab_col[5].default_value = "0";

		tab_col[6].name = dam_label::sector_id;
		tab_col[6].type = sys_label::tab_col_type_int;
		tab_col[6].default_value = "-1";

		tab_col[7].name = dam_label::sector_name;
		tab_col[7].type = sys_label::tab_col_type_string;

		tab_col[8].name = dam_label::out_id;
		tab_col[8].type = sys_label::tab_col_type_int;
		tab_col[8].default_value = "-1";




		tab_col[9].name = dam_label::out_point_flag;
		tab_col[9].type = sys_label::tab_col_type_int;
		tab_col[9].default_value = "0";

		tab_col[10].name = dam_label::transsec_flag;
		tab_col[10].type = sys_label::tab_col_type_bool;
		tab_col[10].default_value = "true";;


		tab_col[11].name = hyd_label::polyseg_out;
		tab_col[11].type = sys_label::tab_col_type_polyline;

		tab_col[12].name = label::description;
		tab_col[12].type = sys_label::tab_col_type_string;

		try {
			Dam_CI_Element_List::connection_table = new Tables();
			if (Dam_CI_Element_List::connection_table->create_non_existing_tables(tab_name, tab_col, num_col, ptr_database, _sys_table_type::dam) == false) {
				cout << " Table exists" << endl;
				Sys_Common_Output::output_dam->output_txt(&cout);
			};
		}
		catch (bad_alloc& t) {
			Error msg;
			msg.set_msg("Dam_CI_Element_List::create_connection_table(QSqlDatabase *ptr_database)", "Can not allocate the memory", "Check the memory", 10, false);
			ostringstream info;
			info << "Info bad alloc: " << t.what() << endl;
			msg.make_second_info(info.str());
			throw msg;
		}
		catch (Error msg) {
			Dam_CI_Element_List::close_connection_table();
			throw msg;
		}

		Dam_CI_Element_List::close_connection_table();
	}

}
//Set the database table for the CI connection: it sets the table name and the name of the columns and allocate them
void Dam_CI_Element_List::set_connection_table(QSqlDatabase *ptr_database, const bool not_close) {
	if (Dam_CI_Element_List::connection_table == NULL) {
		//make specific input for this class
		const string tab_id_name = dam_label::tab_ci_connection;
		string tab_col[13];

		tab_col[0] = dam_label::glob_id;
		tab_col[1] = label::areastate_id;
		tab_col[2] = label::measure_id;
		tab_col[3] = label::applied_flag;
		tab_col[4] = dam_label::in_id;
		tab_col[5] = dam_label::in_point_flag;
		tab_col[6] = dam_label::out_id;
		tab_col[7] = dam_label::out_point_flag;
		tab_col[8] = hyd_label::polyseg_out;
		tab_col[9] = label::description;
		tab_col[10]= dam_label::sector_id;
		tab_col[11]= dam_label::sector_name;
		tab_col[12] = dam_label::transsec_flag;


		try {
			Dam_CI_Element_List::connection_table = new Tables(tab_id_name, tab_col, sizeof(tab_col) / sizeof(tab_col[0]));
			Dam_CI_Element_List::connection_table->set_name(ptr_database, _sys_table_type::dam);
		}
		catch (bad_alloc& t) {
			Error msg;
			msg.set_msg("Dam_CI_Element_List::set_connection_table(QSqlDatabase *ptr_database)", "Can not allocate the memory", "Check the memory", 10, false);
			ostringstream info;
			info << "Info bad alloc: " << t.what() << endl;
			msg.make_second_info(info.str());
			throw msg;
		}
		catch (Error msg) {
			if (not_close == false) {
				Dam_CI_Element_List::close_connection_table();
			}
			throw msg;
		}
	}
}
//Delete all data in the database table for the CI connection (static)
void Dam_CI_Element_List::delete_data_in_connection_table(QSqlDatabase *ptr_database) {
	//the table is set (the name and the column names) and allocated
	try {
		Dam_CI_Element_List::set_connection_table(ptr_database);
	}
	catch (Error msg) {
		throw msg;
	}
	//delete the table
	Dam_CI_Element_List::connection_table->delete_data_in_table(ptr_database);
}
//Delete all data in the database table for the CI connection of the given system state (static)
void Dam_CI_Element_List::delete_data_in_connection_table(QSqlDatabase *ptr_database, const _sys_system_id id) {
	QSqlQueryModel results;
	ostringstream test_filter;
	try {
		Dam_CI_Element_List::set_connection_table(ptr_database);
	}
	catch (Error msg) {
		throw msg;
	}
	//delete
	test_filter << "DELETE ";
	test_filter << " FROM " << Dam_CI_Element_List::connection_table->get_table_name();
	test_filter << " WHERE ";
	test_filter << Dam_CI_Element_List::connection_table->get_column_name(label::areastate_id) << " = " << id.area_state;
	test_filter << " AND ";
	test_filter << Dam_CI_Element_List::connection_table->get_column_name(label::measure_id) << " = " << id.measure_nr;

	Data_Base::database_request(&results, test_filter.str(), ptr_database);

	//check the request
	if (results.lastError().isValid()) {
		Error msg;
		msg.set_msg("Dam_CI_Element_List::delete_data_in_connection_table(QSqlDatabase *ptr_database, const _sys_system_id id)", "Invalid database request", "Check the database", 2, false);
		ostringstream info;
		info << "Table Name      : " << Dam_CI_Element_List::connection_table->get_table_name() << endl;
		info << "Table error info: " << results.lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
}
//Close and delete the database table for the CI connection (static)
void Dam_CI_Element_List::close_connection_table(void) {
	if (Dam_CI_Element_List::connection_table != NULL) {
		delete Dam_CI_Element_List::connection_table;
		Dam_CI_Element_List::connection_table = NULL;
	}
}
//Get the maximum value of the global index of the CI connection database table (static)
int Dam_CI_Element_List::get_max_glob_id_connection_table(QSqlDatabase *ptr_database) {
	try {
		Dam_CI_Element_List::set_connection_table(ptr_database);
	}
	catch (Error msg) {
		throw msg;
	}

	//evaluate the global identifier
	int id_glob = Dam_CI_Element_List::connection_table->maximum_int_of_column(Dam_CI_Element_List::connection_table->get_column_name(dam_label::glob_id), ptr_database);
	return id_glob;
}
//Get the header for inserting the CI connection data to database table (static)
string Dam_CI_Element_List::get_insert_header_connection_table(QSqlDatabase *ptr_database) {
	try {
		Dam_CI_Element_List::set_connection_table(ptr_database);
	}
	catch (Error msg) {
		throw msg;
	}
	//set the query via a query string
	ostringstream query_string;
	query_string << "INSERT INTO  " << Dam_CI_Element_List::connection_table->get_table_name();
	query_string << " ( ";
	query_string << Dam_CI_Element_List::connection_table->get_column_name(dam_label::glob_id) << " , ";
	query_string << Dam_CI_Element_List::connection_table->get_column_name(label::areastate_id) << " , ";
	query_string << Dam_CI_Element_List::connection_table->get_column_name(label::measure_id) << " , ";
	query_string << Dam_CI_Element_List::connection_table->get_column_name(label::applied_flag) << " , ";
	//data
	query_string << Dam_CI_Element_List::connection_table->get_column_name(dam_label::in_id) << " , ";
	query_string << Dam_CI_Element_List::connection_table->get_column_name(dam_label::in_point_flag) << " , ";
	query_string << Dam_CI_Element_List::connection_table->get_column_name(dam_label::sector_id) << " , ";
	query_string << Dam_CI_Element_List::connection_table->get_column_name(dam_label::sector_name) << " , ";
	query_string << Dam_CI_Element_List::connection_table->get_column_name(dam_label::out_id) << " , ";
	query_string << Dam_CI_Element_List::connection_table->get_column_name(dam_label::transsec_flag) << " , ";
	query_string << Dam_CI_Element_List::connection_table->get_column_name(dam_label::out_point_flag) << " , ";
	query_string << Dam_CI_Element_List::connection_table->get_column_name(hyd_label::polyseg_out) << " ) ";


	query_string << " VALUES ";
	string buffer;
	buffer = query_string.str();
	return buffer;

}
//Get the data-string to complete a insert-string for inserting the data of the connection to database
string Dam_CI_Element_List::get_datastring_members2database(const int global_id, const int index_con, Geo_Point *in, Geo_Point *out, const int sec_id, const string sec_name, const bool transsec_flag) {
	string buffer = label::not_set;


	ostringstream query_string;
	query_string << " ( ";
	query_string << global_id << " , ";
	query_string << this->system_id.area_state << " , ";
	query_string << this->system_id.measure_nr << " , ";
	query_string << "true" << " , ";
	//data
	query_string << this->elem_list.at(index_con).at(0)<< " , ";
	query_string << this->elem_list.at(index_con).at(1) << " , ";
	query_string << sec_id << " , ";
	query_string << "'" << sec_name<< "' , ";
	query_string << this->elem_list.at(index_con).at(2) << " , ";
	query_string << "'" << functions::convert_boolean2string(transsec_flag) << "' , ";
	query_string << this->elem_list.at(index_con).at(3) << " , ";
	query_string.clear();
	

	
	query_string << Geo_Polysegment::get_line2sql_string(in->get_xcoordinate(), in->get_ycoordinate(), out->get_xcoordinate(), out->get_ycoordinate()) << " ) ";



	buffer = query_string.str();
	return buffer;
}
//Select and count the number of relevant CI-connections in a database table 
int Dam_CI_Element_List::select_relevant_connection_database(QSqlQueryModel *results, QSqlDatabase *ptr_database, const _sys_system_id id, const bool with_output) {
	int number = 0;
	try {
		Dam_CI_Element_List::set_connection_table(ptr_database);
	}
	catch (Error msg) {
		throw msg;
	}
	if (with_output == true) {
		ostringstream cout;
		cout << "Select relevant CI-connections in database ..." << endl;
		Sys_Common_Output::output_dam->output_txt(&cout);
	}

	ostringstream test_filter;
	test_filter << "Select ";
	test_filter << Dam_CI_Element_List::connection_table->get_column_name(dam_label::in_id) << " , ";
	test_filter << Dam_CI_Element_List::connection_table->get_column_name(dam_label::in_point_flag) << " , ";
	test_filter << Dam_CI_Element_List::connection_table->get_column_name(dam_label::sector_id) << " , ";
	test_filter << Dam_CI_Element_List::connection_table->get_column_name(dam_label::sector_name) << " , ";
	test_filter << Dam_CI_Element_List::connection_table->get_column_name(dam_label::transsec_flag) << " , ";
	test_filter << Dam_CI_Element_List::connection_table->get_column_name(dam_label::out_id) << " , ";
	test_filter << Dam_CI_Element_List::connection_table->get_column_name(dam_label::out_point_flag) << "  ";


	test_filter << " from " << Dam_CI_Element_List::connection_table->get_table_name();
	test_filter << " where ";
	test_filter << Dam_CI_Element_List::connection_table->get_column_name(label::applied_flag) << "= true";
	test_filter << " and ";
	test_filter << Dam_CI_Element_List::connection_table->get_column_name(label::areastate_id) << " =" << id.area_state;
	test_filter << " and (";
	test_filter << Dam_CI_Element_List::connection_table->get_column_name(label::measure_id) << " = " << 0;
	test_filter << " or ";
	test_filter << Dam_CI_Element_List::connection_table->get_column_name(label::measure_id) << " = " << id.measure_nr;
	test_filter << " ) ";
	test_filter << " order by " << Dam_CI_Element_List::connection_table->get_column_name(dam_label::glob_id);

	Data_Base::database_request(results, test_filter.str(), ptr_database);

	//check the request
	if (results->lastError().isValid()) {
		Error msg;
		msg.set_msg("Dam_CI_Element_List::select_relevant_connection_database(QSqlQueryModel *results, QSqlDatabase *ptr_database, const _sys_system_id id, const bool with_output)", "Invalid database request", "Check the database", 2, false);
		ostringstream info;
		info << "Table Name      : " << Dam_CI_Element_List::connection_table->get_table_name() << endl;
		info << "Table error info: " << results->lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}

	number = results->rowCount();

	if (with_output == true) {
		ostringstream cout;
		cout << number << " relevant simple CI-connection(s) are selected in database" << endl;
		Sys_Common_Output::output_dam->output_txt(&cout);
	}

	return number;




}
//Select and count the number of relevant CI connection in a database table (just part of it) 
int Dam_CI_Element_List::select_relevant_connection_database(QSqlQueryModel *results, QSqlDatabase *ptr_database, const _sys_system_id id, const int offset, const int number_rows, const bool with_output) {
	int number = 0;
	try {
		Dam_CI_Element_List::set_connection_table(ptr_database);
	}
	catch (Error msg) {
		throw msg;
	}
	if (with_output == true) {
		ostringstream cout;
		cout << "Select relevant CI-connections in database ..." << endl;
		Sys_Common_Output::output_dam->output_txt(&cout);
	}

	ostringstream test_filter;
	test_filter << "Select ";
	test_filter << Dam_CI_Element_List::connection_table->get_column_name(dam_label::in_id) << " , ";
	test_filter << Dam_CI_Element_List::connection_table->get_column_name(dam_label::in_point_flag) << " , ";
	test_filter << Dam_CI_Element_List::connection_table->get_column_name(dam_label::sector_id) << " , ";
	test_filter << Dam_CI_Element_List::connection_table->get_column_name(dam_label::sector_name) << " , ";
	test_filter << Dam_CI_Element_List::connection_table->get_column_name(dam_label::transsec_flag) << " , ";
	test_filter << Dam_CI_Element_List::connection_table->get_column_name(dam_label::out_id) << " , ";
	test_filter << Dam_CI_Element_List::connection_table->get_column_name(dam_label::out_point_flag) << "  ";


	test_filter << " from " << Dam_CI_Element_List::connection_table->get_table_name();
	test_filter << " where ";
	test_filter << Dam_CI_Element_List::connection_table->get_column_name(label::applied_flag) << "= true";
	test_filter << " and ";
	test_filter << Dam_CI_Element_List::connection_table->get_column_name(label::areastate_id) << " =" << id.area_state;
	test_filter << " and (";
	test_filter << Dam_CI_Element_List::connection_table->get_column_name(label::measure_id) << " = " << 0;
	test_filter << " or ";
	test_filter << Dam_CI_Element_List::connection_table->get_column_name(label::measure_id) << " = " << id.measure_nr;
	test_filter << " ) ";
	test_filter << " order by " << Dam_CI_Element_List::connection_table->get_column_name(dam_label::glob_id);
	test_filter << " LIMIT " << number_rows;
	test_filter << " OFFSET " << offset;

	Data_Base::database_request(results, test_filter.str(), ptr_database);

	//check the request
	if (results->lastError().isValid()) {
		Error msg;
		msg.set_msg("Dam_CI_Element_List::select_relevant_connection_database(QSqlQueryModel *results, QSqlDatabase *ptr_database, const _sys_system_id id, const int offset, const int number_rows, const bool with_output)", "Invalid database request", "Check the database", 2, false);
		ostringstream info;
		info << "Table Name      : " << Dam_CI_Element_List::connection_table->get_table_name() << endl;
		info << "Table error info: " << results->lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}

	number = results->rowCount();

	if (with_output == true) {
		ostringstream cout;
		cout << number << " relevant simple CI-connection(s) are selected in database" << endl;
		Sys_Common_Output::output_dam->output_txt(&cout);
	}

	return number;




}
//Count the number of relevant CI connection in a database table 
int Dam_CI_Element_List::count_relevant_connection_database(QSqlQueryModel *results, QSqlDatabase *ptr_database, const _sys_system_id id, const bool with_output) {
	int number = 0;
	try {
		Dam_CI_Element_List::set_connection_table(ptr_database);
	}
	catch (Error msg) {
		throw msg;
	}
	if (with_output == true) {
		ostringstream cout;
		cout << "Count relevant CI-connections in database ..." << endl;
		Sys_Common_Output::output_dam->output_txt(&cout);
	}

	ostringstream test_filter;
	test_filter << "Select COUNT(";
	test_filter << Dam_CI_Element_List::connection_table->get_column_name(dam_label::in_id) << " ) ";

	test_filter << " from " << Dam_CI_Element_List::connection_table->get_table_name();
	test_filter << " where ";
	test_filter << Dam_CI_Element_List::connection_table->get_column_name(label::applied_flag) << "= true";
	test_filter << " and ";
	test_filter << Dam_CI_Element_List::connection_table->get_column_name(label::areastate_id) << " =" << id.area_state;
	test_filter << " and (";
	test_filter << Dam_CI_Element_List::connection_table->get_column_name(label::measure_id) << " = " << 0;
	test_filter << " or ";
	test_filter << Dam_CI_Element_List::connection_table->get_column_name(label::measure_id) << " = " << id.measure_nr;
	test_filter << " ) ";

	Data_Base::database_request(results, test_filter.str(), ptr_database);

	//check the request
	if (results->lastError().isValid()) {
		Error msg;
		msg.set_msg("Dam_CI_Element_List::count_relevant_connection_database(QSqlQueryModel *results, QSqlDatabase *ptr_database, const _sys_system_id id, const bool with_output)", "Invalid database request", "Check the database", 2, false);
		ostringstream info;
		info << "Table Name      : " << Dam_CI_Element_List::connection_table->get_table_name() << endl;
		info << "Table error info: " << results->lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}

	if (results->rowCount() > 0) {
		number = results->record(0).field(0).value().toInt();
	}

	if (with_output == true) {
		ostringstream cout;
		cout << number << " relevant CI-connection(s) are found in database" << endl;
		Sys_Common_Output::output_dam->output_txt(&cout);
	}

	return number;
}
//Create the database table for the results of the damage calculation per connection (static)
void Dam_CI_Element_List::create_erg_table(QSqlDatabase *ptr_database) {
	if (Dam_CI_Element_List::connection_erg_table == NULL) {
		ostringstream cout;
		cout << "Create CI-connection result database table..." << endl;
		Sys_Common_Output::output_dam->output_txt(&cout);
		//make specific input for this class
		const string tab_name = dam_label::tab_ci_connection_erg;
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

		tab_col[4].name = hyd_label::sz_bound_id;
		tab_col[4].type = sys_label::tab_col_type_int;
		tab_col[4].unsigned_flag = true;
		tab_col[4].key_flag = true;

		tab_col[5].name = risk_label::sz_break_id;
		tab_col[5].type = sys_label::tab_col_type_string;
		tab_col[5].key_flag = true;

		tab_col[6].name = dam_label::in_id;
		tab_col[6].type = sys_label::tab_col_type_int;
		tab_col[6].default_value = "-1";

		tab_col[7].name = dam_label::in_point_flag;
		tab_col[7].type = sys_label::tab_col_type_int;
		tab_col[7].default_value = "0";

		tab_col[8].name = dam_label::sector_id;
		tab_col[8].type = sys_label::tab_col_type_int;
		tab_col[8].default_value = "-1";

		tab_col[9].name = dam_label::sector_level;
		tab_col[9].type = sys_label::tab_col_type_int;
		tab_col[9].default_value = "-1";

		tab_col[10].name = dam_label::out_id;
		tab_col[10].type = sys_label::tab_col_type_int;
		tab_col[10].default_value = "-1";

		tab_col[11].name = dam_label::out_point_flag;
		tab_col[11].type = sys_label::tab_col_type_int;
		tab_col[11].default_value = "0";

		tab_col[12].name = dam_label::failure_type;
		tab_col[12].type = sys_label::tab_col_type_string;

		tab_col[13].name = dam_label::regular_flag;
		tab_col[13].type = sys_label::tab_col_type_bool;
		tab_col[13].default_value = "true";

		tab_col[14].name = hyd_label::polyseg_out;
		tab_col[14].type = sys_label::tab_col_type_polyline;

		try {
			Dam_CI_Element_List::connection_erg_table = new Tables();
			if (Dam_CI_Element_List::connection_erg_table->create_non_existing_tables(tab_name, tab_col, num_col, ptr_database, _sys_table_type::dam) == false) {
				cout << " Table exists" << endl;
				Sys_Common_Output::output_dam->output_txt(&cout);
			};
		}
		catch (bad_alloc& t) {
			Error msg;
			msg.set_msg("Dam_CI_Element_List::create_erg_table(QSqlDatabase *ptr_database)", "Can not allocate the memory", "Check the memory", 10, false);
			ostringstream info;
			info << "Info bad alloc: " << t.what() << endl;
			msg.make_second_info(info.str());
			throw msg;
		}
		catch (Error msg) {
			Dam_CI_Element_List::close_erg_table();
			throw msg;
		}

		Dam_CI_Element_List::close_erg_table();
	}
}
//Set the database table for the results of the damage calculation per connection: it sets the table name and the name of the columns and allocate them (static)
void Dam_CI_Element_List::set_erg_table(QSqlDatabase *ptr_database, const bool not_close) {
	if (Dam_CI_Element_List::connection_erg_table == NULL) {
		//make specific input for this class
		const string tab_id_name = dam_label::tab_ci_connection_erg;
		string tab_col[15];

		tab_col[0] = dam_label::glob_id;
		tab_col[1] = dam_label::regular_flag;
		tab_col[2] = label::areastate_id;
		tab_col[3] = label::measure_id;
		tab_col[4] = label::applied_flag;
		tab_col[5] = hyd_label::sz_bound_id;
		tab_col[6] = risk_label::sz_break_id;
		tab_col[7] = dam_label::sector_id;
		tab_col[8] = dam_label::sector_level;
		tab_col[9] = dam_label::failure_type;
		tab_col[10] = hyd_label::polyseg_out;
		tab_col[11] = dam_label::in_id;
		tab_col[12] = dam_label::in_point_flag;
		tab_col[13] = dam_label::out_id;
		tab_col[14] = dam_label::out_point_flag;
		

		try {
			Dam_CI_Element_List::connection_erg_table = new Tables(tab_id_name, tab_col, sizeof(tab_col) / sizeof(tab_col[0]));
			Dam_CI_Element_List::connection_erg_table->set_name(ptr_database, _sys_table_type::dam);
		}
		catch (bad_alloc& t) {
			Error msg;
			msg.set_msg("Dam_CI_Element_List::set_erg_table(QSqlDatabase *ptr_database)", "Can not allocate the memory", "Check the memory", 10, false);
			ostringstream info;
			info << "Info bad alloc: " << t.what() << endl;
			msg.make_second_info(info.str());
			throw msg;
		}
		catch (Error msg) {
			if (not_close == false) {
				Dam_CI_Element_List::close_erg_table();
			}
			throw msg;
		}
	}
}
//Delete all data in the database table for the results of the damage calculation per connection (static)
void Dam_CI_Element_List::delete_data_in_erg_table(QSqlDatabase *ptr_database) {
	//the table is set (the name and the column names) and allocated
	try {
		Dam_CI_Element_List::set_erg_table(ptr_database);
	}
	catch (Error msg) {
		throw msg;
	}
	//delete the table
	Dam_CI_Element_List::connection_erg_table->delete_data_in_table(ptr_database);
}
//Delete the data in the database table for the results of the damage calculation specified by the system state (static)
void Dam_CI_Element_List::delete_data_in_erg_table(QSqlDatabase *ptr_database, const _sys_system_id id) {
	//the table is set (the name and the column names) and allocated
	try {
		Dam_CI_Element_List::set_erg_table(ptr_database);
	}
	catch (Error msg) {
		throw msg;
	}

	//delete the table
	QSqlQuery query(*ptr_database);
	ostringstream query_string;
	query_string << "DELETE FROM " << Dam_CI_Element_List::connection_erg_table->get_table_name();
	query_string << " WHERE ";
	query_string << Dam_CI_Element_List::connection_erg_table->get_column_name(label::areastate_id) << " = " << id.area_state;
	query_string << " AND ";
	query_string << Dam_CI_Element_List::connection_erg_table->get_column_name(label::measure_id) << " = " << id.measure_nr;

	Data_Base::database_request(&query, query_string.str(), ptr_database);
	if (query.lastError().isValid() == true) {
		Error msg;
		msg.set_msg("Dam_CI_Element_List::delete_data_in_erg_table(QSqlDatabase *ptr_database, const _sys_system_id id)", "Invalid database request", "Check the database", 2, false);
		ostringstream info;
		info << "Table Name      : " << Dam_CI_Element_List::connection_erg_table->get_table_name() << endl;
		info << "Table error info: " << query.lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
}
//Delete the data in the database table for the results of the damage calculation specified parameters (static)
void Dam_CI_Element_List::delete_data_in_erg_table(QSqlDatabase *ptr_database, const _sys_system_id id, const string break_sz, const bool like_flag) {
	//the table is set (the name and the column names) and allocated
	try {
		Dam_CI_Element_List::set_erg_table(ptr_database);
	}
	catch (Error msg) {
		throw msg;
	}

	//delete the table
	QSqlQuery query(*ptr_database);
	ostringstream query_string;
	query_string << "DELETE FROM " << Dam_CI_Element_List::connection_erg_table->get_table_name();
	query_string << " WHERE ";
	query_string << Dam_CI_Element_List::connection_erg_table->get_column_name(label::areastate_id) << " = " << id.area_state;
	query_string << " AND ";
	query_string << Dam_CI_Element_List::connection_erg_table->get_column_name(label::measure_id) << " = " << id.measure_nr;
	query_string << " AND ";
	if (like_flag == false) {
		query_string << Dam_CI_Element_List::connection_erg_table->get_column_name(risk_label::sz_break_id) << " = '" << break_sz << "'";
	}
	else {
		query_string << Dam_CI_Element_List::connection_erg_table->get_column_name(risk_label::sz_break_id) << " LIKE '" << break_sz << "'";
	}

	Data_Base::database_request(&query, query_string.str(), ptr_database);
	if (query.lastError().isValid() == true) {
		Error msg;
		msg.set_msg("Dam_CI_Element_List::delete_data_in_erg_table(QSqlDatabase *ptr_database, const _sys_system_id id, const string break_sz, const bool like_flag)", "Invalid database request", "Check the database", 2, false);
		ostringstream info;
		info << "Table Name      : " << Dam_CI_Element_List::connection_erg_table->get_table_name() << endl;
		info << "Table error info: " << query.lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
}
//Delete the data in the database table for the results of the damage calculation specified by the system id and the scenario-ids (static)
void Dam_CI_Element_List::delete_data_in_erg_table(QSqlDatabase *ptr_database, const _sys_system_id id, const int bound_sz, const string break_sz) {
	//the table is set (the name and the column names) and allocated
	try {
		Dam_CI_Element_List::set_erg_table(ptr_database);
	}
	catch (Error msg) {
		throw msg;
	}

	//delete the table
	QSqlQuery query(*ptr_database);
	ostringstream query_string;
	query_string << "DELETE FROM " << Dam_CI_Element_List::connection_erg_table->get_table_name();
	query_string << " WHERE ";
	query_string << Dam_CI_Element_List::connection_erg_table->get_column_name(label::areastate_id) << " = " << id.area_state;
	query_string << " AND ";
	query_string << Dam_CI_Element_List::connection_erg_table->get_column_name(label::measure_id) << " = " << id.measure_nr;
	query_string << " AND ";
	query_string << Dam_CI_Element_List::connection_erg_table->get_column_name(hyd_label::sz_bound_id) << " = " << bound_sz;
	query_string << " AND ";
	query_string << Dam_CI_Element_List::connection_erg_table->get_column_name(risk_label::sz_break_id) << " = '" << break_sz << "'";

	Data_Base::database_request(&query, query_string.str(), ptr_database);
	if (query.lastError().isValid() == true) {
		Error msg;
		msg.set_msg("Dam_CI_Element_List::delete_data_in_erg_table(QSqlDatabase *ptr_database, const _sys_system_id id, const int bound_sz, const string break_sz )", "Invalid database request", "Check the database", 2, false);
		ostringstream info;
		info << "Table Name      : " << Dam_CI_Element_List::connection_erg_table->get_table_name() << endl;
		info << "Table error info: " << query.lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
}
//Delete the data in the database table for the results of the damage calculation specified by the hydraulic scenario-ids (static)
void Dam_CI_Element_List::delete_data_in_erg_table(QSqlDatabase *ptr_database, const int bound_sz) {
	//the table is set (the name and the column names) and allocated
	try {
		Dam_CI_Element_List::set_erg_table(ptr_database);
	}
	catch (Error msg) {
		throw msg;
	}

	//delete the table
	QSqlQuery query(*ptr_database);
	ostringstream query_string;
	query_string << "DELETE FROM " << Dam_CI_Element_List::connection_erg_table->get_table_name();
	query_string << " WHERE ";
	query_string << Dam_CI_Element_List::connection_erg_table->get_column_name(hyd_label::sz_bound_id) << " = " << bound_sz;

	Data_Base::database_request(&query, query_string.str(), ptr_database);
	if (query.lastError().isValid() == true) {
		Error msg;
		msg.set_msg("Dam_CI_Element_List::delete_data_in_erg_table(QSqlDatabase *ptr_database, const int bound_sz)", "Invalid database request", "Check the database", 2, false);
		ostringstream info;
		info << "Table Name      : " << Dam_CI_Element_List::connection_erg_table->get_table_name() << endl;
		info << "Table error info: " << query.lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
}
//Close and delete the database table for the results of the damage calculation per connection (static)
void Dam_CI_Element_List::close_erg_table(void) {
	if (Dam_CI_Element_List::connection_erg_table != NULL) {
		delete Dam_CI_Element_List::connection_erg_table;
		Dam_CI_Element_List::connection_erg_table = NULL;
	}
}
//Get the maximum value of the global index of the result connection database table (static)
int Dam_CI_Element_List::get_max_glob_id_connection_erg_table(QSqlDatabase *ptr_database) {
	try {
		Dam_CI_Element_List::set_erg_table(ptr_database);
	}
	catch (Error msg) {
		throw msg;
	}

	//evaluate the global identifier
	int id_glob = Dam_CI_Element_List::connection_erg_table->maximum_int_of_column(Dam_CI_Element_List::connection_erg_table->get_column_name(dam_label::glob_id), ptr_database);
	return id_glob;
}
//Get the header for inserting the connection result data to database table (static)
string Dam_CI_Element_List::get_insert_header_erg_table(QSqlDatabase *ptr_database) {
	try {
		Dam_CI_Element_List::set_erg_table(ptr_database);
	}
	catch (Error msg) {
		throw msg;
	}
	//set the query via a query string
	ostringstream query_string;
	query_string << "INSERT INTO  " << Dam_CI_Element_List::connection_erg_table->get_table_name();
	query_string << " ( ";
	query_string << Dam_CI_Element_List::connection_erg_table->get_column_name(dam_label::glob_id) << " , ";
	query_string << Dam_CI_Element_List::connection_erg_table->get_column_name(label::areastate_id) << " , ";
	query_string << Dam_CI_Element_List::connection_erg_table->get_column_name(label::measure_id) << " , ";
	query_string << Dam_CI_Element_List::connection_erg_table->get_column_name(hyd_label::sz_bound_id) << " , ";
	query_string << Dam_CI_Element_List::connection_erg_table->get_column_name(risk_label::sz_break_id) << " , ";
	//results
	query_string << Dam_CI_Element_List::connection_erg_table->get_column_name(dam_label::in_id) << " , ";
	query_string << Dam_CI_Element_List::connection_erg_table->get_column_name(dam_label::in_point_flag) << " , ";
	query_string << Dam_CI_Element_List::connection_erg_table->get_column_name(dam_label::sector_id) << " , ";
	query_string << Dam_CI_Element_List::connection_erg_table->get_column_name(dam_label::sector_level) << " , ";
	query_string << Dam_CI_Element_List::connection_erg_table->get_column_name(dam_label::out_id) << " , ";
	query_string << Dam_CI_Element_List::connection_erg_table->get_column_name(dam_label::out_point_flag) << " , ";
	query_string << Dam_CI_Element_List::connection_erg_table->get_column_name(dam_label::failure_type) << " , ";
	query_string << Dam_CI_Element_List::connection_erg_table->get_column_name(dam_label::regular_flag) << " , ";

	query_string << Dam_CI_Element_List::connection_erg_table->get_column_name(hyd_label::polyseg_out) << ")";




	query_string << " VALUES ";
	string buffer;
	buffer = query_string.str();
	return buffer;
}
//Switch the applied-flag for the CI-connection damage results in the database table for a defined system state (static)
void Dam_CI_Element_List::switch_applied_flag_erg_table(QSqlDatabase *ptr_database, const _sys_system_id id, const bool flag) {
	//the table is set (the name and the column names) and allocated
	try {
		Dam_CI_Element_List::set_erg_table(ptr_database);
	}
	catch (Error msg) {
		throw msg;
	}
	QSqlQueryModel query;

	ostringstream query_string;
	query_string << "UPDATE ";
	query_string << Dam_CI_Element_List::connection_erg_table->get_table_name();
	query_string << " SET ";
	query_string << Dam_CI_Element_List::connection_erg_table->get_column_name(label::applied_flag) << " = " << functions::convert_boolean2string(flag);
	query_string << " WHERE ";
	query_string << Dam_CI_Element_List::connection_erg_table->get_column_name(label::areastate_id) << " = " << id.area_state;
	query_string << " AND ";
	query_string << Dam_CI_Element_List::connection_erg_table->get_column_name(label::measure_id) << " = " << id.measure_nr;
	query_string << " AND ";
	query_string << Dam_CI_Element_List::connection_erg_table->get_column_name(label::applied_flag) << " = " << functions::convert_boolean2string(!flag);

	Data_Base::database_request(&query, query_string.str(), ptr_database);
	if (query.lastError().isValid() == true) {
		Error msg;
		msg.set_msg("Dam_CI_Element_List::switch_applied_flag_erg_table(QSqlDatabase *ptr_database, const _sys_system_id id, const bool flag)", "Invalid database request", "Check the database", 2, false);
		ostringstream info;
		info << "Table Name      : " << Dam_CI_Element_List::connection_erg_table->get_table_name() << endl;
		info << "Table error info: " << query.lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}


	//instat
	//the table is set (the name and the column names) and allocated
	try {
		Dam_CI_Element_List::set_instat_erg_table(ptr_database);
	}
	catch (Error msg) {
		throw msg;
	}
	QSqlQueryModel query1;

	ostringstream query_string1;
	query_string1 << "UPDATE ";
	query_string1 << Dam_CI_Element_List::connection_instat_erg_table->get_table_name();
	query_string1 << " SET ";
	query_string1 << Dam_CI_Element_List::connection_instat_erg_table->get_column_name(label::applied_flag) << " = " << functions::convert_boolean2string(flag);
	query_string1 << " WHERE ";
	query_string1 << Dam_CI_Element_List::connection_instat_erg_table->get_column_name(label::areastate_id) << " = " << id.area_state;
	query_string1 << " AND ";
	query_string1 << Dam_CI_Element_List::connection_instat_erg_table->get_column_name(label::measure_id) << " = " << id.measure_nr;
	query_string1 << " AND ";
	query_string1 << Dam_CI_Element_List::connection_instat_erg_table->get_column_name(label::applied_flag) << " = " << functions::convert_boolean2string(!flag);

	Data_Base::database_request(&query1, query_string1.str(), ptr_database);
	if (query1.lastError().isValid() == true) {
		Error msg;
		msg.set_msg("Dam_CI_Element_List::switch_applied_flag_erg_table(QSqlDatabase *ptr_database, const _sys_system_id id, const bool flag)", "Invalid database request", "Check the database", 2, false);
		ostringstream info;
		info << "Table Name      : " << Dam_CI_Element_List::connection_instat_erg_table->get_table_name() << endl;
		info << "Table error info: " << query1.lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}

}
//Switch the applied-flag for the CI-connection damage results in the database table for a defined system state (static)
void Dam_CI_Element_List::switch_applied_flag_erg_table(QSqlDatabase *ptr_database, const _sys_system_id id, const int hyd_sc, const bool flag) {
	//the table is set (the name and the column names) and allocated
	try {
		Dam_CI_Element_List::set_erg_table(ptr_database);
	}
	catch (Error msg) {
		throw msg;
	}
	QSqlQueryModel query;

	ostringstream query_string;
	query_string << "UPDATE ";
	query_string << Dam_CI_Element_List::connection_erg_table->get_table_name();
	query_string << " SET ";
	query_string << Dam_CI_Element_List::connection_erg_table->get_column_name(label::applied_flag) << " = " << functions::convert_boolean2string(flag);
	query_string << " WHERE ";
	query_string << Dam_CI_Element_List::connection_erg_table->get_column_name(label::areastate_id) << " = " << id.area_state;
	query_string << " AND ";
	query_string << Dam_CI_Element_List::connection_erg_table->get_column_name(label::measure_id) << " = " << id.measure_nr;
	query_string << " AND ";
	query_string << Dam_CI_Element_List::connection_erg_table->get_column_name(hyd_label::sz_bound_id) << " = " << hyd_sc;
	query_string << " AND ";
	query_string << Dam_CI_Element_List::connection_erg_table->get_column_name(label::applied_flag) << " = " << functions::convert_boolean2string(!flag);

	Data_Base::database_request(&query, query_string.str(), ptr_database);
	if (query.lastError().isValid() == true) {
		Error msg;
		msg.set_msg("Dam_CI_Element_List::switch_applied_flag_erg_table(QSqlDatabase *ptr_database, const _sys_system_id id, const bool flag)", "Invalid database request", "Check the database", 2, false);
		ostringstream info;
		info << "Table Name      : " << Dam_CI_Element_List::connection_erg_table->get_table_name() << endl;
		info << "Table error info: " << query.lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}

	//instat
	//the table is set (the name and the column names) and allocated
	try {
		Dam_CI_Element_List::set_instat_erg_table(ptr_database);
	}
	catch (Error msg) {
		throw msg;
	}
	QSqlQueryModel query1;

	ostringstream query_string1;
	query_string1 << "UPDATE ";
	query_string1 << Dam_CI_Element_List::connection_instat_erg_table->get_table_name();
	query_string1 << " SET ";
	query_string1 << Dam_CI_Element_List::connection_instat_erg_table->get_column_name(label::applied_flag) << " = " << functions::convert_boolean2string(flag);
	query_string1 << " WHERE ";
	query_string1 << Dam_CI_Element_List::connection_instat_erg_table->get_column_name(label::areastate_id) << " = " << id.area_state;
	query_string1 << " AND ";
	query_string1 << Dam_CI_Element_List::connection_instat_erg_table->get_column_name(label::measure_id) << " = " << id.measure_nr;
	query_string1 << " AND ";
	query_string1 << Dam_CI_Element_List::connection_instat_erg_table->get_column_name(hyd_label::sz_bound_id) << " = " << hyd_sc;
	query_string1 << " AND ";
	query_string1 << Dam_CI_Element_List::connection_instat_erg_table->get_column_name(label::applied_flag) << " = " << functions::convert_boolean2string(!flag);

	Data_Base::database_request(&query1, query_string1.str(), ptr_database);
	if (query1.lastError().isValid() == true) {
		Error msg;
		msg.set_msg("Dam_CI_Element_List::switch_applied_flag_erg_table(QSqlDatabase *ptr_database, const _sys_system_id id, const bool flag)", "Invalid database request", "Check the database", 2, false);
		ostringstream info;
		info << "Table Name      : " << Dam_CI_Element_List::connection_instat_erg_table->get_table_name() << endl;
		info << "Table error info: " << query1.lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
}
//Copy the results of a given system id to another one in database tables (static)
void Dam_CI_Element_List::copy_results(QSqlDatabase *ptr_database, const _sys_system_id src, const _sys_system_id dest) {
	//the table is set (the name and the column names) and allocated
	try {
		Dam_CI_Element_List::set_erg_table(ptr_database);
	}
	catch (Error msg) {
		throw msg;
	}

	int glob_id = 0;
	glob_id = Dam_CI_Element_List::connection_erg_table->maximum_int_of_column(Dam_CI_Element_List::connection_erg_table->get_column_name(dam_label::glob_id), ptr_database) + 1;

	QSqlQueryModel model;
	ostringstream test_filter;
	test_filter << "SELECT ";
	test_filter << Dam_CI_Element_List::connection_erg_table->get_column_name(dam_label::glob_id);
	test_filter << " from " << Dam_CI_Element_List::connection_erg_table->get_table_name();
	test_filter << " WHERE ";
	test_filter << Dam_CI_Element_List::connection_erg_table->get_column_name(label::areastate_id) << " = " << src.area_state;
	test_filter << " AND ";
	test_filter << Dam_CI_Element_List::connection_erg_table->get_column_name(label::measure_id) << " = " << src.measure_nr;

	//submit it to the datbase
	Data_Base::database_request(&model, test_filter.str(), ptr_database);
	if (model.lastError().isValid() == true) {
		Error msg;
		msg.set_msg("Dam_CI_Element_List::copy_results(QSqlDatabase *ptr_database, const _sys_system_id src, const _sys_system_id dest)", "Invalid database request", "Check the database", 2, false);
		ostringstream info;
		info << "Table Name      : " << Dam_CI_Element_List::connection_erg_table->get_table_name() << endl;
		info << "Table error info: " << model.lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
	test_filter.str("");

	ostringstream cout;
	cout << "Copy " << model.rowCount() << " connection results of the CI-system to the new measure state..." << endl;
	Sys_Common_Output::output_dam->output_txt(&cout);

	QSqlQueryModel model1;
	for (int i = 0; i < model.rowCount(); i++) {
		if (i % 10000 == 0 && i > 0) {
			cout << "Copy SC-connection results " << i << " to " << i + 10000 << "..." << endl;
			Sys_Common_Output::output_dam->output_txt(&cout);
		}
		test_filter.str("");
		test_filter << "INSERT INTO " << Dam_CI_Element_List::connection_erg_table->get_table_name();
		test_filter << " SELECT " << glob_id << " , ";
		test_filter << dest.area_state << " , ";
		test_filter << dest.measure_nr << " , ";
		test_filter << Dam_CI_Element_List::connection_erg_table->get_column_name(label::applied_flag) << " , ";
		test_filter << Dam_CI_Element_List::connection_erg_table->get_column_name(hyd_label::sz_bound_id) << " , ";
		test_filter << Dam_CI_Element_List::connection_erg_table->get_column_name(risk_label::sz_break_id) << " , ";

		test_filter << Dam_CI_Element_List::connection_erg_table->get_column_name(dam_label::out_id) << " , ";
		test_filter << Dam_CI_Element_List::connection_erg_table->get_column_name(dam_label::out_point_flag) << " , ";
		test_filter << Dam_CI_Element_List::connection_erg_table->get_column_name(dam_label::in_id) << " , ";
		test_filter << Dam_CI_Element_List::connection_erg_table->get_column_name(dam_label::in_point_flag) << " , ";
		test_filter << Dam_CI_Element_List::connection_erg_table->get_column_name(dam_label::sector_id) << " , ";
		test_filter << Dam_CI_Element_List::connection_erg_table->get_column_name(dam_label::sector_level) << " , ";
		test_filter << Dam_CI_Element_List::connection_erg_table->get_column_name(dam_label::failure_type) << " , ";
		test_filter << Dam_CI_Element_List::connection_erg_table->get_column_name(dam_label::regular_flag) << " , ";



		test_filter << Dam_CI_Element_List::connection_erg_table->get_column_name(hyd_label::polyseg_out) << "  ";

		test_filter << " FROM " << Dam_CI_Element_List::connection_erg_table->get_table_name() << " ";
		test_filter << " WHERE " << Dam_CI_Element_List::connection_erg_table->get_column_name(dam_label::glob_id) << " = ";
		test_filter << model.record(i).value(Dam_CI_Element_List::connection_erg_table->get_column_name(dam_label::glob_id).c_str()).toInt();
		Data_Base::database_request(&model1, test_filter.str(), ptr_database);
		if (model1.lastError().isValid() == true) {
			Error msg;
			msg.set_msg("Dam_CI_Element_List::copy_results(QSqlDatabase *ptr_database, const _sys_system_id src, const _sys_system_id dest)", "Invalid database request", "Check the database", 2, false);
			ostringstream info;
			info << "Table Name      : " << Dam_CI_Element_List::connection_erg_table->get_table_name() << endl;
			info << "Table error info: " << model1.lastError().text().toStdString() << endl;
			msg.make_second_info(info.str());
			throw msg;
		}
		glob_id++;
	}
}
//Create the database table for the instationary results of the damage calculation per CI-connection (static)
void Dam_CI_Element_List::create_instat_erg_table(QSqlDatabase *ptr_database) {
	if (Dam_CI_Element_List::connection_instat_erg_table == NULL) {
		ostringstream cout;
		cout << "Create CI-connection instationary result database table..." << endl;
		Sys_Common_Output::output_dam->output_txt(&cout);
		//make specific input for this class
		const string tab_name = dam_label::tab_ci_connection_instat_erg;
		const int num_col = 16;
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

		tab_col[4].name = hyd_label::sz_bound_id;
		tab_col[4].type = sys_label::tab_col_type_int;
		tab_col[4].unsigned_flag = true;
		tab_col[4].key_flag = true;

		tab_col[5].name = risk_label::sz_break_id;
		tab_col[5].type = sys_label::tab_col_type_string;
		tab_col[5].key_flag = true;

		tab_col[6].name = dam_label::in_id;
		tab_col[6].type = sys_label::tab_col_type_int;
		tab_col[6].default_value = "-1";

		tab_col[7].name = dam_label::in_point_flag;
		tab_col[7].type = sys_label::tab_col_type_int;
		tab_col[7].default_value = "0";



		tab_col[8].name = dam_label::sector_id;
		tab_col[8].type = sys_label::tab_col_type_int;
		tab_col[8].default_value = "-1";

		tab_col[9].name = dam_label::sector_level;
		tab_col[9].type = sys_label::tab_col_type_int;
		tab_col[9].default_value = "-1";

		tab_col[10].name = dam_label::out_id;
		tab_col[10].type = sys_label::tab_col_type_int;
		tab_col[10].default_value = "-1";

		tab_col[11].name = dam_label::out_point_flag;
		tab_col[11].type = sys_label::tab_col_type_int;
		tab_col[11].default_value = "0";


		tab_col[12].name = dam_label::failure_type;
		tab_col[12].type = sys_label::tab_col_type_string;

		tab_col[13].name = dam_label::regular_flag;
		tab_col[13].type = sys_label::tab_col_type_bool;
		tab_col[13].default_value = "true";

		tab_col[14].name = hyd_label::data_time;
		tab_col[14].type = sys_label::tab_col_type_string;
		tab_col[14].default_value = "";

		tab_col[15].name = hyd_label::polyseg_out;
		tab_col[15].type = sys_label::tab_col_type_polyline;

		try {
			Dam_CI_Element_List::connection_instat_erg_table = new Tables();
			if (Dam_CI_Element_List::connection_instat_erg_table->create_non_existing_tables(tab_name, tab_col, num_col, ptr_database, _sys_table_type::dam) == false) {
				cout << " Table exists" << endl;
				Sys_Common_Output::output_dam->output_txt(&cout);
			};
		}
		catch (bad_alloc& t) {
			Error msg;
			msg.set_msg("Dam_CI_Element_List::create_instat_erg_table(QSqlDatabase *ptr_database)", "Can not allocate the memory", "Check the memory", 10, false);
			ostringstream info;
			info << "Info bad alloc: " << t.what() << endl;
			msg.make_second_info(info.str());
			throw msg;
		}
		catch (Error msg) {
			Dam_CI_Element_List::close_instat_erg_table();
			throw msg;
		}

		Dam_CI_Element_List::close_instat_erg_table();
	}


}
//Set the database table for the instationary results of the damage calculation per CI-connection: it sets the table name and the name of the columns and allocate them (static)
void Dam_CI_Element_List::set_instat_erg_table(QSqlDatabase *ptr_database, const bool not_close) {
	if (Dam_CI_Element_List::connection_instat_erg_table == NULL) {
		//make specific input for this class
		const string tab_id_name = dam_label::tab_ci_connection_instat_erg;
		string tab_col[16];

		tab_col[0] = dam_label::glob_id;
		tab_col[1] = dam_label::regular_flag;
		tab_col[2] = label::areastate_id;
		tab_col[3] = label::measure_id;
		tab_col[4] = label::applied_flag;
		tab_col[5] = hyd_label::sz_bound_id;
		tab_col[6] = risk_label::sz_break_id;
		tab_col[7] = dam_label::sector_id;
		tab_col[8] = hyd_label::data_time;
		tab_col[9] = dam_label::sector_level;
		tab_col[10] = dam_label::failure_type;
		tab_col[11] = hyd_label::polyseg_out;
		tab_col[12] = dam_label::in_id;
		tab_col[13] = dam_label::in_point_flag;
		tab_col[14] = dam_label::out_id;
		tab_col[15] = dam_label::out_point_flag;


		try {
			Dam_CI_Element_List::connection_instat_erg_table = new Tables(tab_id_name, tab_col, sizeof(tab_col) / sizeof(tab_col[0]));
			Dam_CI_Element_List::connection_instat_erg_table->set_name(ptr_database, _sys_table_type::dam);
		}
		catch (bad_alloc& t) {
			Error msg;
			msg.set_msg("Dam_CI_Element_List::set_instat_erg_table(QSqlDatabase *ptr_database)", "Can not allocate the memory", "Check the memory", 10, false);
			ostringstream info;
			info << "Info bad alloc: " << t.what() << endl;
			msg.make_second_info(info.str());
			throw msg;
		}
		catch (Error msg) {
			if (not_close == false) {
				Dam_CI_Element_List::close_instat_erg_table();
			}
			throw msg;
		}
	}
}
//Delete all data in the database table for the instationary results of the damage calculation per CI-connection (static)
void Dam_CI_Element_List::delete_data_in_instat_erg_table(QSqlDatabase *ptr_database) {
	//the table is set (the name and the column names) and allocated
	try {
		Dam_CI_Element_List::set_instat_erg_table(ptr_database);
	}
	catch (Error msg) {
		throw msg;
	}
	//delete the table
	Dam_CI_Element_List::connection_instat_erg_table->delete_data_in_table(ptr_database);
}
//Delete the data in the database table for the instationary results of the damage calculation specified by the system state (static)
void Dam_CI_Element_List::delete_data_in_instat_erg_table(QSqlDatabase *ptr_database, const _sys_system_id id) {
	//the table is set (the name and the column names) and allocated
	try {
		Dam_CI_Element_List::set_instat_erg_table(ptr_database);
	}
	catch (Error msg) {
		throw msg;
	}

	//delete the table
	QSqlQuery query(*ptr_database);
	ostringstream query_string;
	query_string << "DELETE FROM " << Dam_CI_Element_List::connection_instat_erg_table->get_table_name();
	query_string << " WHERE ";
	query_string << Dam_CI_Element_List::connection_instat_erg_table->get_column_name(label::areastate_id) << " = " << id.area_state;
	query_string << " AND ";
	query_string << Dam_CI_Element_List::connection_instat_erg_table->get_column_name(label::measure_id) << " = " << id.measure_nr;

	Data_Base::database_request(&query, query_string.str(), ptr_database);
	if (query.lastError().isValid() == true) {
		Error msg;
		msg.set_msg("Dam_CI_Element_List::delete_data_in_instat_erg_table(QSqlDatabase *ptr_database, const _sys_system_id id)", "Invalid database request", "Check the database", 2, false);
		ostringstream info;
		info << "Table Name      : " << Dam_CI_Element_List::connection_instat_erg_table->get_table_name() << endl;
		info << "Table error info: " << query.lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
}
//Delete the data in the database table for the instationary results of the damage calculation specified parameters (static)
void Dam_CI_Element_List::delete_data_in_instat_erg_table(QSqlDatabase *ptr_database, const _sys_system_id id, const string break_sz, const bool like_flag) {
	//the table is set (the name and the column names) and allocated
	try {
		Dam_CI_Element_List::set_instat_erg_table(ptr_database);
	}
	catch (Error msg) {
		throw msg;
	}

	//delete the table
	QSqlQuery query(*ptr_database);
	ostringstream query_string;
	query_string << "DELETE FROM " << Dam_CI_Element_List::connection_instat_erg_table->get_table_name();
	query_string << " WHERE ";
	query_string << Dam_CI_Element_List::connection_instat_erg_table->get_column_name(label::areastate_id) << " = " << id.area_state;
	query_string << " AND ";
	query_string << Dam_CI_Element_List::connection_instat_erg_table->get_column_name(label::measure_id) << " = " << id.measure_nr;
	query_string << " AND ";
	if (like_flag == false) {
		query_string << Dam_CI_Element_List::connection_instat_erg_table->get_column_name(risk_label::sz_break_id) << " = '" << break_sz << "'";
	}
	else {
		query_string << Dam_CI_Element_List::connection_instat_erg_table->get_column_name(risk_label::sz_break_id) << " LIKE '" << break_sz << "'";
	}

	Data_Base::database_request(&query, query_string.str(), ptr_database);
	if (query.lastError().isValid() == true) {
		Error msg;
		msg.set_msg("Dam_CI_Element_List::delete_data_in_instat_erg_table(QSqlDatabase *ptr_database, const _sys_system_id id, const string break_sz, const bool like_flag)", "Invalid database request", "Check the database", 2, false);
		ostringstream info;
		info << "Table Name      : " << Dam_CI_Element_List::connection_instat_erg_table->get_table_name() << endl;
		info << "Table error info: " << query.lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
}
//Delete the data in the database table for the instationary results of the damage calculation specified by the system state and the scenario-ids (static)
void Dam_CI_Element_List::delete_data_in_instat_erg_table(QSqlDatabase *ptr_database, const _sys_system_id id, const int bound_sz, const string break_sz) {
	//the table is set (the name and the column names) and allocated
	try {
		Dam_CI_Element_List::set_instat_erg_table(ptr_database);
	}
	catch (Error msg) {
		throw msg;
	}

	//delete the table
	QSqlQuery query(*ptr_database);
	ostringstream query_string;
	query_string << "DELETE FROM " << Dam_CI_Element_List::connection_instat_erg_table->get_table_name();
	query_string << " WHERE ";
	query_string << Dam_CI_Element_List::connection_instat_erg_table->get_column_name(label::areastate_id) << " = " << id.area_state;
	query_string << " AND ";
	query_string << Dam_CI_Element_List::connection_instat_erg_table->get_column_name(label::measure_id) << " = " << id.measure_nr;
	query_string << " AND ";
	query_string << Dam_CI_Element_List::connection_instat_erg_table->get_column_name(hyd_label::sz_bound_id) << " = " << bound_sz;
	query_string << " AND ";
	query_string << Dam_CI_Element_List::connection_instat_erg_table->get_column_name(risk_label::sz_break_id) << " = '" << break_sz << "'";

	Data_Base::database_request(&query, query_string.str(), ptr_database);
	if (query.lastError().isValid() == true) {
		Error msg;
		msg.set_msg("Dam_CI_Element_List::delete_data_in_instat_erg_table(QSqlDatabase *ptr_database, const _sys_system_id id, const int bound_sz, const string break_sz )", "Invalid database request", "Check the database", 2, false);
		ostringstream info;
		info << "Table Name      : " << Dam_CI_Element_List::connection_instat_erg_table->get_table_name() << endl;
		info << "Table error info: " << query.lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
}
//Delete the data in the database table for the instationary results of the damage calculation specified by the hydraulic scenario-ids (static)
void Dam_CI_Element_List::delete_data_in_instat_erg_table(QSqlDatabase *ptr_database, const int bound_sz) {
	//the table is set (the name and the column names) and allocated
	try {
		Dam_CI_Element_List::set_instat_erg_table(ptr_database);
	}
	catch (Error msg) {
		throw msg;
	}

	//delete the table
	QSqlQuery query(*ptr_database);
	ostringstream query_string;
	query_string << "DELETE FROM " << Dam_CI_Element_List::connection_instat_erg_table->get_table_name();
	query_string << " WHERE ";
	query_string << Dam_CI_Element_List::connection_instat_erg_table->get_column_name(hyd_label::sz_bound_id) << " = " << bound_sz;

	Data_Base::database_request(&query, query_string.str(), ptr_database);
	if (query.lastError().isValid() == true) {
		Error msg;
		msg.set_msg("Dam_CI_Element_List::delete_data_in_instat_erg_table(QSqlDatabase *ptr_database, const int bound_sz)", "Invalid database request", "Check the database", 2, false);
		ostringstream info;
		info << "Table Name      : " << Dam_CI_Element_List::connection_instat_erg_table->get_table_name() << endl;
		info << "Table error info: " << query.lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
}
//Close and delete the database table for the instationary results of the damage calculation per CI-connection (static)
void Dam_CI_Element_List::close_instat_erg_table(void) {
	if (Dam_CI_Element_List::connection_instat_erg_table != NULL) {
		delete Dam_CI_Element_List::connection_instat_erg_table;
		Dam_CI_Element_List::connection_instat_erg_table = NULL;
	}
}
//Get the maximum value of the global index of the instationary result CI-point database table (static)
int Dam_CI_Element_List::get_max_glob_id_connection_instat_erg_table(QSqlDatabase *ptr_database) {
	try {
		Dam_CI_Element_List::set_instat_erg_table(ptr_database);
	}
	catch (Error msg) {
		throw msg;
	}

	//evaluate the global identifier
	int id_glob = Dam_CI_Element_List::connection_instat_erg_table->maximum_int_of_column(Dam_CI_Element_List::connection_instat_erg_table->get_column_name(dam_label::glob_id), ptr_database);
	return id_glob;
}
//Get the header for inserting the CI-connection instationary result data to database table (static)
string Dam_CI_Element_List::get_insert_header_instat_erg_table(QSqlDatabase *ptr_database) {
	try {
		Dam_CI_Element_List::set_instat_erg_table(ptr_database);
	}
	catch (Error msg) {
		throw msg;
	}
	//set the query via a query string
	ostringstream query_string;
	query_string << "INSERT INTO  " << Dam_CI_Element_List::connection_instat_erg_table->get_table_name();
	query_string << " ( ";
	query_string << Dam_CI_Element_List::connection_instat_erg_table->get_column_name(dam_label::glob_id) << " , ";
	query_string << Dam_CI_Element_List::connection_instat_erg_table->get_column_name(label::areastate_id) << " , ";
	query_string << Dam_CI_Element_List::connection_instat_erg_table->get_column_name(label::measure_id) << " , ";
	query_string << Dam_CI_Element_List::connection_instat_erg_table->get_column_name(hyd_label::sz_bound_id) << " , ";
	query_string << Dam_CI_Element_List::connection_instat_erg_table->get_column_name(risk_label::sz_break_id) << " , ";
	//results

	query_string << Dam_CI_Element_List::connection_instat_erg_table->get_column_name(dam_label::in_id) << " , ";
	query_string << Dam_CI_Element_List::connection_instat_erg_table->get_column_name(dam_label::in_point_flag) << " , ";
	query_string << Dam_CI_Element_List::connection_instat_erg_table->get_column_name(dam_label::sector_id) << " , ";
	query_string << Dam_CI_Element_List::connection_instat_erg_table->get_column_name(dam_label::sector_level) << " , ";
	query_string << Dam_CI_Element_List::connection_instat_erg_table->get_column_name(dam_label::out_id) << " , ";
	query_string << Dam_CI_Element_List::connection_instat_erg_table->get_column_name(dam_label::out_point_flag) << " , ";
	query_string << Dam_CI_Element_List::connection_instat_erg_table->get_column_name(dam_label::failure_type) << " , ";
	query_string << Dam_CI_Element_List::connection_instat_erg_table->get_column_name(dam_label::regular_flag) << " , ";
	query_string << Dam_CI_Element_List::connection_instat_erg_table->get_column_name(hyd_label::data_time) << " , ";

	query_string << Dam_CI_Element_List::connection_instat_erg_table->get_column_name(hyd_label::polyseg_out) << ")";

	query_string << " VALUES ";
	string buffer;
	buffer = query_string.str();
	return buffer;
}
//Copy the instationaryresults of a given system id to another one in database tables per CI-connection (static) 
void Dam_CI_Element_List::copy_instat_results(QSqlDatabase *ptr_database, const _sys_system_id src, const _sys_system_id dest) {
	//the table is set (the name and the column names) and allocated
	try {
		Dam_CI_Element_List::set_instat_erg_table(ptr_database);
	}
	catch (Error msg) {
		throw msg;
	}

	int glob_id = 0;
	glob_id = Dam_CI_Element_List::connection_instat_erg_table->maximum_int_of_column(Dam_CI_Element_List::connection_instat_erg_table->get_column_name(dam_label::glob_id), ptr_database) + 1;

	QSqlQueryModel model;
	ostringstream test_filter;
	test_filter << "SELECT ";
	test_filter << Dam_CI_Element_List::connection_instat_erg_table->get_column_name(dam_label::glob_id);
	test_filter << " from " << Dam_CI_Element_List::connection_instat_erg_table->get_table_name();
	test_filter << " WHERE ";
	test_filter << Dam_CI_Element_List::connection_instat_erg_table->get_column_name(label::areastate_id) << " = " << src.area_state;
	test_filter << " AND ";
	test_filter << Dam_CI_Element_List::connection_instat_erg_table->get_column_name(label::measure_id) << " = " << src.measure_nr;

	//submit it to the datbase
	Data_Base::database_request(&model, test_filter.str(), ptr_database);
	if (model.lastError().isValid() == true) {
		Error msg;
		msg.set_msg("Dam_CI_Element_List::copy_instat_results(QSqlDatabase *ptr_database, const _sys_system_id src, const _sys_system_id dest)", "Invalid database request", "Check the database", 2, false);
		ostringstream info;
		info << "Table Name      : " << Dam_CI_Element_List::connection_instat_erg_table->get_table_name() << endl;
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
		test_filter << "INSERT INTO " << Dam_CI_Element_List::connection_instat_erg_table->get_table_name();
		test_filter << " SELECT " << glob_id << " , ";
		test_filter << dest.area_state << " , ";
		test_filter << dest.measure_nr << " , ";
		test_filter << Dam_CI_Element_List::connection_instat_erg_table->get_column_name(label::applied_flag) << " , ";
		test_filter << Dam_CI_Element_List::connection_instat_erg_table->get_column_name(hyd_label::sz_bound_id) << " , ";
		test_filter << Dam_CI_Element_List::connection_instat_erg_table->get_column_name(risk_label::sz_break_id) << " , ";

		test_filter << Dam_CI_Element_List::connection_instat_erg_table->get_column_name(dam_label::out_id) << " , ";
		test_filter << Dam_CI_Element_List::connection_instat_erg_table->get_column_name(dam_label::out_point_flag) << " , ";
		test_filter << Dam_CI_Element_List::connection_instat_erg_table->get_column_name(dam_label::in_id) << " , ";
		test_filter << Dam_CI_Element_List::connection_instat_erg_table->get_column_name(dam_label::in_point_flag) << " , ";

		test_filter << Dam_CI_Element_List::connection_instat_erg_table->get_column_name(dam_label::sector_id) << " , ";
		test_filter << Dam_CI_Element_List::connection_instat_erg_table->get_column_name(dam_label::sector_level) << " , ";
		test_filter << Dam_CI_Element_List::connection_instat_erg_table->get_column_name(dam_label::regular_flag) << " , ";
		test_filter << Dam_CI_Element_List::connection_instat_erg_table->get_column_name(dam_label::failure_type) << " , ";
		test_filter << Dam_CI_Element_List::connection_instat_erg_table->get_column_name(hyd_label::data_time) << " , ";

		test_filter << Dam_CI_Element_List::connection_instat_erg_table->get_column_name(hyd_label::polyseg_out) << "  ";

		test_filter << " FROM " << Dam_CI_Element_List::connection_instat_erg_table->get_table_name() << " ";
		test_filter << " WHERE " << Dam_CI_Element_List::connection_instat_erg_table->get_column_name(dam_label::glob_id) << " = ";
		test_filter << model.record(i).value(Dam_CI_Element_List::connection_instat_erg_table->get_column_name(dam_label::glob_id).c_str()).toInt();
		Data_Base::database_request(&model1, test_filter.str(), ptr_database);
		if (model1.lastError().isValid() == true) {
			Error msg;
			msg.set_msg("Dam_CI_Element_List::copy_instat_results(QSqlDatabase *ptr_database, const _sys_system_id src, const _sys_system_id dest)", "Invalid database request", "Check the database", 2, false);
			ostringstream info;
			info << "Table Name      : " << Dam_CI_Element_List::connection_instat_erg_table->get_table_name() << endl;
			info << "Table error info: " << model1.lastError().text().toStdString() << endl;
			msg.make_second_info(info.str());
			throw msg;
		}
		glob_id++;
	}
}
//Get number of connections
int Dam_CI_Element_List::get_number_connection(void) {

	return this->elem_list.count();
}
//Get output the connections
void Dam_CI_Element_List::output_connection(ostringstream *cout) {
	for (int i = 0; i < this->elem_list.count(); i++) {
		*cout << W(13) << P(0) << i;
		for (int j = 0; j < this->elem_list.at(i).count(); j++) {
			*cout << W(13) << P(0) << this->elem_list.at(i).at(j);
		}
		*cout << endl;	
	}
}
//Check the last set connections
void Dam_CI_Element_List::check_members(void) {
	//check first polygon
	if (this->elem_list.last().at(1)==1) {
		Error msg = this->set_error(1);
		ostringstream info;
		info << "Id in        :" << this->elem_list.last().at(0) << endl;
		info << "Id out       :" << this->elem_list.last().at(2) << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
	//check circle
	if (this->elem_list.last().at(1) == this->elem_list.last().at(3)) {
		if (this->elem_list.last().at(0) == this->elem_list.last().at(2)) {
			Error msg = this->set_error(2);
			ostringstream info;
			info << "Id in        :" << this->elem_list.last().at(0) << endl;
			info << "Id out       :" << this->elem_list.last().at(2) << endl;
			msg.make_second_info(info.str());
			throw msg;
		}
	} 



}
//____________
//private
//Set error(s)
Error Dam_CI_Element_List::set_error(const int err_type) {
	string place = "Dam_CI_Element_List::";
	string help;
	string reason;
	int type = 0;
	bool fatal = false;
	stringstream info;
	Error msg;

	switch (err_type) {
	case 0://bad alloc
		place.append("get_datastring_members2database(const int global_id, const int index_con, Geo_Point *in, Geo_Point *out)");
		reason = "Can not allocate the memory";
		help = "Check the memory";
		type = 10;
		break;
	case 1://
		place.append("check_members(void)");
		reason = "The incoming (first) CI-structure is defined as polygon; just point-structures are allowed as incoming CI-element";
		help = "Avoid circle; check CI-connection data";
		type = 34;
		break;
	case 2://
		place.append("check_members(void)");
		reason = "The incoming (first) CI-structure is the same as the outgoing CI-structure (second)";
		help = "Check CI-connection data; avoid circles in the CI-system";
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