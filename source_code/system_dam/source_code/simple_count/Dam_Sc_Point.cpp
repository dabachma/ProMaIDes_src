#include "Dam_Headers_Precompiled.h"
//#include "Dam_Sc_Point.h"
//init static members
Tables *Dam_Sc_Point::point_table=NULL;
Tables *Dam_Sc_Point::point_erg_table=NULL;

//Default constructor
Dam_Sc_Point::Dam_Sc_Point(void){
	this->cat_id=-1;
	this->subcat_id=-1;
	this->score_value=0.0;
	this->boundary_value=0.0;
	this->is_affected=false;

	this->is_connected=false;

	this->index_fp=-1;
	this->index_fp_elem=-1;
	this->global_index=-1;

	//count the memory
	Sys_Memory_Count::self()->add_mem(sizeof(Dam_Sc_Point), _sys_system_modules::DAM_SYS);
}
//Default destructor
Dam_Sc_Point::~Dam_Sc_Point(void){
	//count the memory
	Sys_Memory_Count::self()->minus_mem(sizeof(Dam_Sc_Point), _sys_system_modules::DAM_SYS);
}
//___________
//public
//Input the simple counting point data per database
void Dam_Sc_Point::input_point_perdatabase(const QSqlQueryModel *results, const int glob_index){
	try{
		this->number=results->record(glob_index).value((Dam_Sc_Point::point_table->get_column_name(dam_label::point_id)).c_str()).toInt();
		//set the element members
		this->cat_id=results->record(glob_index).value((Dam_Sc_Point::point_table->get_column_name(dam_label::category_id)).c_str()).toInt();
		this->score_value=results->record(glob_index).value((Dam_Sc_Point::point_table->get_column_name(dam_label::sc_score)).c_str()).toDouble();
		this->boundary_value=results->record(glob_index).value((Dam_Sc_Point::point_table->get_column_name(dam_label::boundary_value)).c_str()).toDouble();
		this->subcat_id=results->record(glob_index).value((Dam_Sc_Point::point_table->get_column_name(dam_label::subcategory_id)).c_str()).toInt();
		this->name=results->record(glob_index).value((Dam_Sc_Point::point_table->get_column_name(dam_label::point_name)).c_str()).toString().toStdString();
		this->is_connected=results->record(glob_index).value((Dam_Sc_Point::point_table->get_column_name(dam_label::raster_connected)).c_str()).toBool();
		this->index_fp=results->record(glob_index).value((Dam_Sc_Point::point_table->get_column_name(dam_label::conn_fp_id)).c_str()).toInt();
		this->index_fp_elem=results->record(glob_index).value((Dam_Sc_Point::point_table->get_column_name(dam_label::conn_fp_elem_id)).c_str()).toInt();

		double x_mid=0.0;
		double y_mid=0.0;
		x_mid=results->record(glob_index).value((Dam_Sc_Point::point_table->get_column_name(dam_label::elem_mid_x)).c_str()).toDouble();
		y_mid=results->record(glob_index).value((Dam_Sc_Point::point_table->get_column_name(dam_label::elem_mid_y)).c_str()).toDouble();
		this->set_point_coordinate(x_mid, y_mid);
		this->global_index=results->record(glob_index).value((Dam_Sc_Point::point_table->get_column_name(dam_label::glob_id)).c_str()).toInt();

		this->category=this->convert_id2enum(this->cat_id);
		this->check_members();
	}
	catch(Error msg){
		ostringstream info;
		info << "Point number           : " << this->number << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
}
//Get a string to set the point data from the raster interception to the database table: identifier of the floodplain, -floodplain element
bool Dam_Sc_Point::get_string_interception_point_data2database(ostringstream *text){
	//generate the filter
	*text << "UPDATE " << Dam_Sc_Point::point_table->get_table_name().c_str();
	*text << " SET ";
	*text << Dam_Sc_Point::point_table->get_column_name(dam_label::conn_fp_id).c_str() << " = " << this->index_fp << " , ";
	*text << Dam_Sc_Point::point_table->get_column_name(dam_label::conn_fp_elem_id).c_str() << " = " << this->index_fp_elem << " , ";
	*text << Dam_Sc_Point::point_table->get_column_name(dam_label::raster_connected).c_str() << " = " << functions::convert_boolean2string(true) << "  ";

	*text << " WHERE " ;
	*text << Dam_Sc_Point::point_table->get_column_name(dam_label::glob_id) << " = " << this->global_index<<"; ";

	return true;
}
//Create the database table for the simple counting points (static)
void Dam_Sc_Point::create_point_table(QSqlDatabase *ptr_database){
		if(Dam_Sc_Point::point_table==NULL){
			ostringstream cout;
			cout << "Create simple-counting point data database table..." << endl ;
			Sys_Common_Output::output_dam->output_txt(&cout);
			//make specific input for this class
			const string tab_name=dam_label::tab_sc_point;
			const int num_col=17;
			_Sys_data_tab_column tab_col[num_col];
			//init
			for(int i=0; i< num_col; i++){
				tab_col[i].key_flag=false;
				tab_col[i].unsigned_flag=false;
				tab_col[i].primary_key_flag=false;
			}

			tab_col[0].name=dam_label::glob_id;
			tab_col[0].type=sys_label::tab_col_type_int;
			tab_col[0].unsigned_flag=true;
			tab_col[0].primary_key_flag=true;

			tab_col[1].name=dam_label::point_id;
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

			tab_col[5].name=dam_label::category_id;
			tab_col[5].type=sys_label::tab_col_type_int;
			tab_col[5].default_value="-1";

			tab_col[6].name=dam_label::category_name;
			tab_col[6].type=sys_label::tab_col_type_string;

			tab_col[7].name=dam_label::sc_score;
			tab_col[7].type=sys_label::tab_col_type_double;
			tab_col[7].default_value="0.0";

			tab_col[8].name=dam_label::boundary_value;
			tab_col[8].type=sys_label::tab_col_type_double;
			tab_col[8].default_value="0.0";

			tab_col[9].name=dam_label::point_name;
			tab_col[9].type=sys_label::tab_col_type_string;

			tab_col[10].name=dam_label::subcategory_id;
			tab_col[10].type=sys_label::tab_col_type_int;
			tab_col[10].default_value="-1";

			tab_col[11].name=dam_label::elem_mid_x;
			tab_col[11].type=sys_label::tab_col_type_double;

			tab_col[12].name=dam_label::elem_mid_y;
			tab_col[12].type=sys_label::tab_col_type_double;

			tab_col[13].name=dam_label::sc_point;
			tab_col[13].type=sys_label::tab_col_type_point;

			tab_col[14].name=dam_label::conn_fp_id;
			tab_col[14].type=sys_label::tab_col_type_int;
			tab_col[14].default_value="-1";

			tab_col[15].name=dam_label::conn_fp_elem_id;
			tab_col[15].type=sys_label::tab_col_type_int;
			tab_col[15].default_value="-1";

			tab_col[16].name=dam_label::raster_connected;
			tab_col[16].type=sys_label::tab_col_type_bool;
			tab_col[16].default_value="false";

			try{
				Dam_Sc_Point::point_table= new Tables();
				if(Dam_Sc_Point::point_table->create_non_existing_tables(tab_name, tab_col, num_col,ptr_database, _sys_table_type::dam)==false){
					cout << " Table exists" << endl ;
					Sys_Common_Output::output_dam->output_txt(&cout);
				};
			}
			catch(bad_alloc& t){
				Error msg;
				msg.set_msg("Dam_Sc_Point::create_point_table(QSqlDatabase *ptr_database)","Can not allocate the memory", "Check the memory", 10, false);
				ostringstream info;
				info<< "Info bad alloc: " << t.what() << endl;
				msg.make_second_info(info.str());
				throw msg;
			}
			catch(Error msg){
				Dam_Sc_Point::close_point_table();
				throw msg;
			}

		Dam_Sc_Point::close_point_table();
	}
}
//Set the database table for the simple counting points: it sets the table name and the name of the columns and allocate them (static)
void Dam_Sc_Point::set_point_table(QSqlDatabase *ptr_database, const bool not_close){
	if(Dam_Sc_Point::point_table==NULL){
		//make specific input for this class
		const string tab_id_name=dam_label::tab_sc_point;
		string tab_col[17];

		tab_col[0]=dam_label::glob_id;
		tab_col[1]=dam_label::point_id;
		tab_col[2]=label::areastate_id;
		tab_col[3]=label::measure_id;
		tab_col[4]=dam_label::category_id;
		tab_col[5]=dam_label::category_name;
		tab_col[6]=dam_label::sc_score;
		tab_col[7]=dam_label::boundary_value;
		tab_col[8]=dam_label::point_name;
		tab_col[9]=dam_label::subcategory_id;
		tab_col[10]=dam_label::elem_mid_x;
		tab_col[11]=dam_label::elem_mid_y;
		tab_col[12]=dam_label::sc_point;
		tab_col[13]=dam_label::conn_fp_id;
		tab_col[14]=dam_label::conn_fp_elem_id;
		tab_col[15]=dam_label::raster_connected;
		tab_col[16]=label::applied_flag;

		try{
			Dam_Sc_Point::point_table= new Tables(tab_id_name, tab_col, sizeof(tab_col)/sizeof(tab_col[0]));
			Dam_Sc_Point::point_table->set_name(ptr_database, _sys_table_type::dam);
		}
		catch(bad_alloc& t){
			Error msg;
			msg.set_msg("Dam_Sc_Point::set_point_table(QSqlDatabase *ptr_database)","Can not allocate the memory", "Check the memory", 10, false);
			ostringstream info;
			info<< "Info bad alloc: " << t.what() << endl;
			msg.make_second_info(info.str());
			throw msg;
		}
		catch(Error msg){
			if (not_close == false) {
				Dam_Sc_Point::close_point_table();
				
			}
			throw msg;
		}
	}
}
//Delete all data in the database table for the simple counting points (static)
void Dam_Sc_Point::delete_data_in_point_table(QSqlDatabase *ptr_database){
	//the table is set (the name and the column names) and allocated
	try{
		Dam_Sc_Point::set_point_table(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}
	//delete the table
	Dam_Sc_Point::point_table->delete_data_in_table(ptr_database);
}
//Delete all data in the database table for the points of the given system state (static)
void Dam_Sc_Point::delete_data_in_point_table(QSqlDatabase *ptr_database, const _sys_system_id id){
	QSqlQueryModel results;
	ostringstream test_filter;
	try{
		Dam_Sc_Point::set_point_table(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}
	//delete
	test_filter<< "DELETE ";
	test_filter << " FROM " << Dam_Sc_Point::point_table->get_table_name();
	test_filter << " WHERE ";
	test_filter << Dam_Sc_Point::point_table->get_column_name(label::areastate_id) << " = " << id.area_state;
	test_filter << " AND ";
	test_filter << Dam_Sc_Point::point_table->get_column_name(label::measure_id) << " = " << id.measure_nr;

	Data_Base::database_request(&results, test_filter.str(), ptr_database);

	//check the request
	if(results.lastError().isValid()){
		Error msg;
		msg.set_msg("Dam_Sc_Point::delete_data_in_elem_table(QSqlDatabase *ptr_database, const _sys_system_id id)","Invalid database request", "Check the database", 2, false);
		ostringstream info;
		info << "Table Name      : " << Dam_Sc_Point::point_table->get_table_name() << endl;
		info << "Table error info: " << results.lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
}
//Close and delete the database table for the simple counting points (static)
void Dam_Sc_Point::close_point_table(void){
	if(Dam_Sc_Point::point_table!=NULL){
		delete Dam_Sc_Point::point_table;
		Dam_Sc_Point::point_table=NULL;
	}
}
//Get the maximum value of the global index of the point database table (static)
int Dam_Sc_Point::get_max_glob_id_point_table(QSqlDatabase *ptr_database){
	try{
		Dam_Sc_Point::set_point_table(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}

	//evaluate the global identifier
	int id_glob=Dam_Sc_Point::point_table->maximum_int_of_column(Dam_Sc_Point::point_table->get_column_name(dam_label::glob_id),ptr_database);
	return id_glob;
}
//Get the header for inserting the point data to database table (static)
string Dam_Sc_Point::get_insert_header_point_table(QSqlDatabase *ptr_database){
	try{
		Dam_Sc_Point::set_point_table(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}
	//set the query via a query string
	ostringstream query_string;
	query_string << "INSERT INTO  " << Dam_Sc_Point::point_table->get_table_name();
	query_string <<" ( ";
	query_string << Dam_Sc_Point::point_table->get_column_name(dam_label::glob_id) <<" , ";
	query_string << Dam_Sc_Point::point_table->get_column_name(dam_label::point_id) <<" , ";
	query_string << Dam_Sc_Point::point_table->get_column_name(label::areastate_id) <<" , ";
	query_string << Dam_Sc_Point::point_table->get_column_name(label::measure_id) <<" , ";
	query_string << Dam_Sc_Point::point_table->get_column_name(label::applied_flag) <<" , ";
	//data
	query_string <<  Dam_Sc_Point::point_table->get_column_name(dam_label::category_id) << " , ";
	query_string <<  Dam_Sc_Point::point_table->get_column_name(dam_label::category_name) << " , ";
	query_string <<  Dam_Sc_Point::point_table->get_column_name(dam_label::sc_score) << " , ";
	query_string <<  Dam_Sc_Point::point_table->get_column_name(dam_label::boundary_value) << " , ";
	query_string <<  Dam_Sc_Point::point_table->get_column_name(dam_label::point_name) << " , ";
	query_string <<  Dam_Sc_Point::point_table->get_column_name(dam_label::subcategory_id) << " , ";
	query_string << Dam_Sc_Point::point_table->get_column_name(dam_label::elem_mid_x) <<" , ";
	query_string << Dam_Sc_Point::point_table->get_column_name(dam_label::elem_mid_y) <<" , ";
	query_string << Dam_Sc_Point::point_table->get_column_name(dam_label::sc_point) <<" ) ";

	query_string << " VALUES ";
	string buffer;
	buffer=query_string.str();
	return buffer;
}
///Select and count the number of relevant damage points in a database table (static)
int Dam_Sc_Point::select_relevant_points_database(QSqlQueryModel *results, QSqlDatabase *ptr_database, const _sys_system_id id, const bool with_output ){
	int number=0;
	try{
		Dam_Sc_Point::set_point_table(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}
	if(with_output==true){
		ostringstream cout;
		cout << "Select relevant simple counting points in database ..." << endl ;
		Sys_Common_Output::output_dam->output_txt(&cout);
	}

	ostringstream test_filter;
	test_filter<< "Select ";
	test_filter <<  Dam_Sc_Point::point_table->get_column_name(dam_label::point_id) << " , ";
	test_filter <<  Dam_Sc_Point::point_table->get_column_name(dam_label::elem_mid_x) << " , ";
	test_filter <<  Dam_Sc_Point::point_table->get_column_name(dam_label::elem_mid_y) << " , ";
	test_filter <<  Dam_Sc_Point::point_table->get_column_name(dam_label::conn_fp_id) << " , ";
	test_filter <<  Dam_Sc_Point::point_table->get_column_name(dam_label::conn_fp_elem_id) << " , ";
	test_filter <<  Dam_Sc_Point::point_table->get_column_name(dam_label::glob_id) << " , ";
	//set the element members
	test_filter <<  Dam_Sc_Point::point_table->get_column_name(dam_label::category_id) << " , ";
	test_filter <<  Dam_Sc_Point::point_table->get_column_name(dam_label::category_name) << " , ";
	test_filter <<  Dam_Sc_Point::point_table->get_column_name(dam_label::sc_score) << " , ";
	test_filter <<  Dam_Sc_Point::point_table->get_column_name(dam_label::boundary_value) << " , ";
	test_filter <<  Dam_Sc_Point::point_table->get_column_name(dam_label::point_name) << " , ";
	test_filter <<  Dam_Sc_Point::point_table->get_column_name(dam_label::raster_connected) << " , ";
	test_filter <<  Dam_Sc_Point::point_table->get_column_name(dam_label::subcategory_id) << "  ";

	test_filter << " from " << Dam_Sc_Point::point_table->get_table_name();
	test_filter << " where " ;
	test_filter << Dam_Sc_Point::point_table->get_column_name(label::applied_flag) << "= true";
	test_filter << " and ";
	test_filter << Dam_Sc_Point::point_table->get_column_name(label::areastate_id) << " ="  << id.area_state;
	test_filter << " and (";
	test_filter << Dam_Sc_Point::point_table->get_column_name(label::measure_id) << " = " << 0 ;
	test_filter << " or " ;
	test_filter << Dam_Sc_Point::point_table->get_column_name(label::measure_id) << " = " << id.measure_nr;
	test_filter << " ) " ;
	test_filter << " order by " <<  Dam_Sc_Point::point_table->get_column_name(dam_label::point_id);

	Data_Base::database_request(results, test_filter.str(), ptr_database);

	//check the request
	if(results->lastError().isValid()){
		Error msg;
		msg.set_msg("Dam_Sc_Point::select_relevant_points_database(QSqlTableModel *results, const _sys_system_id id, const bool with_output)","Invalid database request", "Check the database", 2, false);
		ostringstream info;
		info << "Table Name      : " << Dam_Sc_Point::point_table->get_table_name() << endl;
		info << "Table error info: " << results->lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}

	number= results->rowCount();

	if(with_output==true){
		ostringstream cout;
		cout << number << " relevant simple counting point(s) are selected in database" << endl ;
		Sys_Common_Output::output_dam->output_txt(&cout);
	}

	return number;
}
//Select and count the number of relevant damage points in a database table (just part of it) (static)
int Dam_Sc_Point::select_relevant_points_database(QSqlQueryModel *results, QSqlDatabase *ptr_database, const _sys_system_id id, const int offset, const int number_rows, const bool with_output){
	int number=0;
	try{
		Dam_Sc_Point::set_point_table(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}
	if(with_output==true){
		ostringstream cout;
		cout << "Select relevant simple counting points in database ..." << endl ;
		Sys_Common_Output::output_dam->output_txt(&cout);
	}

	ostringstream test_filter;
	test_filter<< "Select ";
	test_filter <<  Dam_Sc_Point::point_table->get_column_name(dam_label::point_id) << " , ";
	test_filter <<  Dam_Sc_Point::point_table->get_column_name(dam_label::elem_mid_x) << " , ";
	test_filter <<  Dam_Sc_Point::point_table->get_column_name(dam_label::elem_mid_y) << " , ";
	test_filter <<  Dam_Sc_Point::point_table->get_column_name(dam_label::conn_fp_id) << " , ";
	test_filter <<  Dam_Sc_Point::point_table->get_column_name(dam_label::conn_fp_elem_id) << " , ";
	test_filter <<  Dam_Sc_Point::point_table->get_column_name(dam_label::glob_id) << " , ";
	//set the element members
	test_filter <<  Dam_Sc_Point::point_table->get_column_name(dam_label::category_id) << " , ";
	test_filter <<  Dam_Sc_Point::point_table->get_column_name(dam_label::category_name) << " , ";
	test_filter <<  Dam_Sc_Point::point_table->get_column_name(dam_label::sc_score) << " , ";
	test_filter <<  Dam_Sc_Point::point_table->get_column_name(dam_label::boundary_value) << " , ";
	test_filter <<  Dam_Sc_Point::point_table->get_column_name(dam_label::point_name) << " , ";
	test_filter <<  Dam_Sc_Point::point_table->get_column_name(dam_label::raster_connected) << " , ";
	test_filter <<  Dam_Sc_Point::point_table->get_column_name(dam_label::subcategory_id) << "  ";

	test_filter << " from " << Dam_Sc_Point::point_table->get_table_name();
	test_filter << " where " ;
	test_filter << Dam_Sc_Point::point_table->get_column_name(label::applied_flag) << "= true";
	test_filter << " and ";
	test_filter << Dam_Sc_Point::point_table->get_column_name(label::areastate_id) << " ="  << id.area_state;
	test_filter << " and (";
	test_filter << Dam_Sc_Point::point_table->get_column_name(label::measure_id) << " = " << 0 ;
	test_filter << " or " ;
	test_filter << Dam_Sc_Point::point_table->get_column_name(label::measure_id) << " = " << id.measure_nr;
	test_filter << " ) " ;
	test_filter << " order by " <<  Dam_Sc_Point::point_table->get_column_name(dam_label::point_id);
	test_filter << " LIMIT " << number_rows;
	test_filter << " OFFSET " << offset;

	Data_Base::database_request(results, test_filter.str(), ptr_database);

	//check the request
	if(results->lastError().isValid()){
		Error msg;
		msg.set_msg("Dam_Sc_Point::select_relevant_points_database(QSqlQueryModel *results, QSqlDatabase *ptr_database, const _sys_system_id id, const int offset, const int number_rows, const bool with_output)","Invalid database request", "Check the database", 2, false);
		ostringstream info;
		info << "Table Name      : " << Dam_Sc_Point::point_table->get_table_name() << endl;
		info << "Table error info: " << results->lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}

	number= results->rowCount();

	if(with_output==true){
		ostringstream cout;
		cout << number << " relevant simple counting point(s) are selected in database" << endl ;
		Sys_Common_Output::output_dam->output_txt(&cout);
	}

	return number;
}
//Count the number of relevant damage points in a database table (static)
int Dam_Sc_Point::count_relevant_points_database(QSqlQueryModel *results, QSqlDatabase *ptr_database, const _sys_system_id id, const bool with_output){
	int number=0;
	try{
		Dam_Sc_Point::set_point_table(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}
	if(with_output==true){
		ostringstream cout;
		cout << "Count relevant simple counting points in database ..." << endl ;
		Sys_Common_Output::output_dam->output_txt(&cout);
	}

	ostringstream test_filter;
	test_filter<< "Select COUNT(";
	test_filter <<  Dam_Sc_Point::point_table->get_column_name(dam_label::point_id) << " ) ";

	test_filter << " from " << Dam_Sc_Point::point_table->get_table_name();
	test_filter << " where " ;
	test_filter << Dam_Sc_Point::point_table->get_column_name(label::applied_flag) << "= true";
	test_filter << " and ";
	test_filter << Dam_Sc_Point::point_table->get_column_name(label::areastate_id) << " ="  << id.area_state;
	test_filter << " and (";
	test_filter << Dam_Sc_Point::point_table->get_column_name(label::measure_id) << " = " << 0 ;
	test_filter << " or " ;
	test_filter << Dam_Sc_Point::point_table->get_column_name(label::measure_id) << " = " << id.measure_nr;
	test_filter << " ) " ;

	Data_Base::database_request(results, test_filter.str(), ptr_database);

	//check the request
	if(results->lastError().isValid()){
		Error msg;
		msg.set_msg("Dam_Sc_Point::count_relevant_points_database(QSqlTableModel *results, const _sys_system_id id, const bool with_output)","Invalid database request", "Check the database", 2, false);
		ostringstream info;
		info << "Table Name      : " << Dam_Sc_Point::point_table->get_table_name() << endl;
		info << "Table error info: " << results->lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}

	if(results->rowCount()>0){
		number= results->record(0).field(0).value().toInt();
	}

	if(with_output==true){
		ostringstream cout;
		cout << number << " relevant simple counting point(s) are found in database" << endl ;
		Sys_Common_Output::output_dam->output_txt(&cout);
	}

	return number;
}
//Count number of not connected points to the hydraulic model (static)
int Dam_Sc_Point::count_not_connected_points2hyd(QSqlDatabase *ptr_database, const _sys_system_id id){
	int number=0;
	try{
		Dam_Sc_Point::set_point_table(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}

	QSqlQueryModel results;

	ostringstream test_filter;
	test_filter<< "Select COUNT(";
	test_filter <<  Dam_Sc_Point::point_table->get_column_name(dam_label::point_id) << " ) ";

	test_filter << " from " << Dam_Sc_Point::point_table->get_table_name();
	test_filter << " where " ;
	test_filter << Dam_Sc_Point::point_table->get_column_name(label::applied_flag) << "= true";
	test_filter << " and ";
	test_filter << Dam_Sc_Point::point_table->get_column_name(label::areastate_id) << " ="  << id.area_state;
	test_filter << " and (";
	test_filter << Dam_Sc_Point::point_table->get_column_name(label::measure_id) << " = " << 0 ;
	test_filter << " or " ;
	test_filter << Dam_Sc_Point::point_table->get_column_name(label::measure_id) << " = " << id.measure_nr;
	test_filter << " ) " ;
	test_filter << " and ";
	test_filter << Dam_Sc_Point::point_table->get_column_name(dam_label::raster_connected) << " = " << functions::convert_boolean2string(false);

	Data_Base::database_request(&results, test_filter.str(), ptr_database);

	//check the request
	if(results.lastError().isValid()){
		Error msg;
		msg.set_msg("Dam_Sc_Point::count_not_connected_points2hyd(QSqlDatabase *ptr_database, const _sys_system_id id)","Invalid database request", "Check the database", 2, false);
		ostringstream info;
		info << "Table Name      : " << Dam_Sc_Point::point_table->get_table_name() << endl;
		info << "Table error info: " << results.lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}

	if(results.rowCount()>0){
		number= results.record(0).field(0).value().toInt();
	}

	return number;
}
//Create the database table for the results of the damage calculation per point (static)
void Dam_Sc_Point::create_erg_table(QSqlDatabase *ptr_database){
		if(Dam_Sc_Point::point_erg_table==NULL){
			ostringstream cout;
			cout << "Create simple-counting point result database table..." << endl ;
			Sys_Common_Output::output_dam->output_txt(&cout);
			//make specific input for this class
			const string tab_name=dam_label::tab_sc_erg;
			const int num_col=11;
			_Sys_data_tab_column tab_col[num_col];
			//init
			for(int i=0; i< num_col; i++){
				tab_col[i].key_flag=false;
				tab_col[i].unsigned_flag=false;
				tab_col[i].primary_key_flag=false;
			}

			tab_col[0].name=dam_label::glob_id;
			tab_col[0].type=sys_label::tab_col_type_int;
			tab_col[0].unsigned_flag=true;
			tab_col[0].primary_key_flag=true;

			tab_col[1].name=dam_label::point_id;
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

			tab_col[5].name=hyd_label::sz_bound_id;
			tab_col[5].type=sys_label::tab_col_type_int;
			tab_col[5].unsigned_flag=true;
			tab_col[5].key_flag=true;

			tab_col[6].name=risk_label::sz_break_id;
			tab_col[6].type=sys_label::tab_col_type_string;
			tab_col[6].key_flag=true;

			tab_col[7].name=dam_label::affected_score;
			tab_col[7].type=sys_label::tab_col_type_double;
			tab_col[7].default_value="0.0";
			tab_col[7].unsigned_flag=true;

			tab_col[8].name=dam_label::category_id;
			tab_col[8].type=sys_label::tab_col_type_int;
			tab_col[8].default_value="-1";

			tab_col[9].name=dam_label::subcategory_id;
			tab_col[9].type=sys_label::tab_col_type_int;
			tab_col[9].default_value="-1";

			tab_col[10].name=dam_label::sc_point;
			tab_col[10].type=sys_label::tab_col_type_point;

			try{
				Dam_Sc_Point::point_erg_table= new Tables();
				if(Dam_Sc_Point::point_erg_table->create_non_existing_tables(tab_name, tab_col, num_col,ptr_database, _sys_table_type::dam)==false){
					cout << " Table exists" << endl ;
					Sys_Common_Output::output_dam->output_txt(&cout);
				};
			}
			catch(bad_alloc& t){
				Error msg;
				msg.set_msg("Dam_Sc_Point::create_erg_table(QSqlDatabase *ptr_database)","Can not allocate the memory", "Check the memory", 10, false);
				ostringstream info;
				info<< "Info bad alloc: " << t.what() << endl;
				msg.make_second_info(info.str());
				throw msg;
			}
			catch(Error msg){
				Dam_Sc_Point::close_erg_table();
				throw msg;
			}

		Dam_Sc_Point::close_erg_table();
	}
}
//Set the database table for the results of the damage calculation per point: it sets the table name and the name of the columns and allocate them (static)
void Dam_Sc_Point::set_erg_table(QSqlDatabase *ptr_database){
	if(Dam_Sc_Point::point_erg_table==NULL){
		//make specific input for this class
		const string tab_id_name=dam_label::tab_sc_erg;
		string tab_col[11];

		tab_col[0]=dam_label::glob_id;
		tab_col[1]=dam_label::point_id;
		tab_col[2]=label::areastate_id;
		tab_col[3]=label::measure_id;
		tab_col[4]=label::applied_flag;
		tab_col[5]=hyd_label::sz_bound_id;
		tab_col[6]=risk_label::sz_break_id;
		tab_col[7]=dam_label::affected_score;
		tab_col[8]=dam_label::category_id;
		tab_col[9]=dam_label::subcategory_id;
		tab_col[10]=dam_label::sc_point;

		try{
			Dam_Sc_Point::point_erg_table= new Tables(tab_id_name, tab_col, sizeof(tab_col)/sizeof(tab_col[0]));
			Dam_Sc_Point::point_erg_table->set_name(ptr_database, _sys_table_type::dam);
		}
		catch(bad_alloc& t){
			Error msg;
			msg.set_msg("Dam_Sc_Point::set_erg_table(QSqlDatabase *ptr_database)","Can not allocate the memory", "Check the memory", 10, false);
			ostringstream info;
			info<< "Info bad alloc: " << t.what() << endl;
			msg.make_second_info(info.str());
			throw msg;
		}
		catch(Error msg){
			Dam_Sc_Point::close_erg_table();
			throw msg;
		}
	}
}
//Delete all data in the database table for the results of the damage calculation per point (static)
void Dam_Sc_Point::delete_data_in_erg_table(QSqlDatabase *ptr_database){
	//the table is set (the name and the column names) and allocated
	try{
		Dam_Sc_Point::set_erg_table(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}
	//delete the table
	Dam_Sc_Point::point_erg_table->delete_data_in_table(ptr_database);
}
//Delete the data in the database table for the results of the damage calculation specified by the system state (static)
void Dam_Sc_Point::delete_data_in_erg_table(QSqlDatabase *ptr_database, const _sys_system_id id){
	//the table is set (the name and the column names) and allocated
	try{
		Dam_Sc_Point::set_erg_table(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}

	//delete the table
	QSqlQuery query(*ptr_database);
	ostringstream query_string;
	query_string <<"DELETE FROM " << Dam_Sc_Point::point_erg_table->get_table_name();
	query_string << " WHERE " ;
	query_string << Dam_Sc_Point::point_erg_table->get_column_name(label::areastate_id) << " = " << id.area_state;
	query_string << " AND ";
	query_string << Dam_Sc_Point::point_erg_table->get_column_name(label::measure_id) << " = " << id.measure_nr;

	Data_Base::database_request(&query, query_string.str(), ptr_database);
	if(query.lastError().isValid()==true){
		Error msg;
		msg.set_msg("Dam_Sc_Point::delete_data_in_erg_table(QSqlDatabase *ptr_database, const _sys_system_id id)","Invalid database request", "Check the database", 2, false);
		ostringstream info;
		info << "Table Name      : " << Dam_Sc_Point::point_erg_table->get_table_name() << endl;
		info << "Table error info: " << query.lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
}
//Delete the data in the database table for the results of the damage calculation specified parameters (static)
void Dam_Sc_Point::delete_data_in_erg_table(QSqlDatabase *ptr_database, const _sys_system_id id, const string break_sz, const bool like_flag){
	//the table is set (the name and the column names) and allocated
	try{
		Dam_Sc_Point::set_erg_table(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}

	//delete the table
	QSqlQuery query(*ptr_database);
	ostringstream query_string;
	query_string <<"DELETE FROM " << Dam_Sc_Point::point_erg_table->get_table_name();
	query_string << " WHERE " ;
	query_string << Dam_Sc_Point::point_erg_table->get_column_name(label::areastate_id) << " = " << id.area_state;
	query_string << " AND ";
	query_string << Dam_Sc_Point::point_erg_table->get_column_name(label::measure_id) << " = " << id.measure_nr;
	query_string << " AND ";
	if(like_flag==false){
		query_string << Dam_Sc_Point::point_erg_table->get_column_name(risk_label::sz_break_id) << " = '" << break_sz<<"'";
	}
	else{
		query_string << Dam_Sc_Point::point_erg_table->get_column_name(risk_label::sz_break_id) << " LIKE '" << break_sz<<"'";
	}

	Data_Base::database_request(&query, query_string.str(), ptr_database);
	if(query.lastError().isValid()==true){
		Error msg;
		msg.set_msg("Dam_Sc_Point::delete_data_in_erg_table(QSqlDatabase *ptr_database, const _sys_system_id id, const string break_sz, const bool like_flag)","Invalid database request", "Check the database", 2, false);
		ostringstream info;
		info << "Table Name      : " << Dam_Sc_Point::point_erg_table->get_table_name() << endl;
		info << "Table error info: " << query.lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
}
//Delete the data in the database table for the results of the damage calculation specified by the system id and the scenario-ids (static)
void Dam_Sc_Point::delete_data_in_erg_table(QSqlDatabase *ptr_database, const _sys_system_id id, const int bound_sz, const string break_sz ){
	//the table is set (the name and the column names) and allocated
	try{
		Dam_Sc_Point::set_erg_table(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}

	//delete the table
	QSqlQuery query(*ptr_database);
	ostringstream query_string;
	query_string <<"DELETE FROM " << Dam_Sc_Point::point_erg_table->get_table_name();
	query_string << " WHERE " ;
	query_string << Dam_Sc_Point::point_erg_table->get_column_name(label::areastate_id) << " = " << id.area_state;
	query_string << " AND ";
	query_string << Dam_Sc_Point::point_erg_table->get_column_name(label::measure_id) << " = " << id.measure_nr;
	query_string << " AND ";
	query_string << Dam_Sc_Point::point_erg_table->get_column_name(hyd_label::sz_bound_id) << " = " << bound_sz;
	query_string << " AND ";
	query_string << Dam_Sc_Point::point_erg_table->get_column_name(risk_label::sz_break_id) << " = '" << break_sz<<"'";

	Data_Base::database_request(&query, query_string.str(), ptr_database);
	if(query.lastError().isValid()==true){
		Error msg;
		msg.set_msg("Dam_Sc_Point::delete_data_in_erg_table(QSqlDatabase *ptr_database, const _sys_system_id id, const int bound_sz, const string break_sz )","Invalid database request", "Check the database", 2, false);
		ostringstream info;
		info << "Table Name      : " << Dam_Sc_Point::point_erg_table->get_table_name() << endl;
		info << "Table error info: " << query.lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
}
//Delete the data in the database table for the results of the damage calculation specified by the hydraulic scenario-ids (static)
void Dam_Sc_Point::delete_data_in_erg_table(QSqlDatabase *ptr_database, const int bound_sz){
	//the table is set (the name and the column names) and allocated
	try{
		Dam_Sc_Point::set_erg_table(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}

	//delete the table
	QSqlQuery query(*ptr_database);
	ostringstream query_string;
	query_string <<"DELETE FROM " << Dam_Sc_Point::point_erg_table->get_table_name();
	query_string << " WHERE " ;
	query_string << Dam_Sc_Point::point_erg_table->get_column_name(hyd_label::sz_bound_id) << " = " << bound_sz;

	Data_Base::database_request(&query, query_string.str(), ptr_database);
	if(query.lastError().isValid()==true){
		Error msg;
		msg.set_msg("Dam_Sc_Point::delete_data_in_erg_table(QSqlDatabase *ptr_database, const int bound_sz)","Invalid database request", "Check the database", 2, false);
		ostringstream info;
		info << "Table Name      : " << Dam_Sc_Point::point_erg_table->get_table_name() << endl;
		info << "Table error info: " << query.lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
}
//Close and delete the database table for the results of the damage calculation per point (static)
void Dam_Sc_Point::close_erg_table(void){
	if(Dam_Sc_Point::point_erg_table!=NULL){
		delete Dam_Sc_Point::point_erg_table;
		Dam_Sc_Point::point_erg_table=NULL;
	}
}
//Get the maximum value of the global index of the result point database table (static)
int Dam_Sc_Point::get_max_glob_id_point_erg_table(QSqlDatabase *ptr_database){
	try{
		Dam_Sc_Point::set_erg_table(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}

	//evaluate the global identifier
	int id_glob=Dam_Sc_Point::point_erg_table->maximum_int_of_column(Dam_Sc_Point::point_erg_table->get_column_name(dam_label::glob_id),ptr_database);
	return id_glob;
}
//Get the header for inserting the point result data to database table (static)
string Dam_Sc_Point::get_insert_header_erg_table(QSqlDatabase *ptr_database){
	try{
		Dam_Sc_Point::set_erg_table(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}
	//set the query via a query string
	ostringstream query_string;
	query_string << "INSERT INTO  " << Dam_Sc_Point::point_erg_table->get_table_name();
	query_string <<" ( ";
	query_string << Dam_Sc_Point::point_erg_table->get_column_name(dam_label::glob_id) <<" , ";
	query_string << Dam_Sc_Point::point_erg_table->get_column_name(dam_label::point_id) <<" , ";
	query_string << Dam_Sc_Point::point_erg_table->get_column_name(label::areastate_id) <<" , ";
	query_string << Dam_Sc_Point::point_erg_table->get_column_name(label::measure_id) <<" , ";
	query_string << Dam_Sc_Point::point_erg_table->get_column_name(hyd_label::sz_bound_id) <<" , ";
	query_string << Dam_Sc_Point::point_erg_table->get_column_name(risk_label::sz_break_id) <<" , ";
	//results
	query_string << Dam_Sc_Point::point_erg_table->get_column_name(dam_label::affected_score) << " , ";
	query_string << Dam_Sc_Point::point_erg_table->get_column_name(dam_label::category_id) << " , ";
	query_string << Dam_Sc_Point::point_erg_table->get_column_name(dam_label::subcategory_id) << " , ";

	query_string <<Dam_Sc_Point::point_erg_table->get_column_name(dam_label::sc_point) <<")";

	query_string << " VALUES ";
	string buffer;
	buffer=query_string.str();
	return buffer;
}
//Select data in the database table for the results of the damage calculation specified by the system id and the scenario-ids (static)
int Dam_Sc_Point::select_data_in_erg_table(QSqlQueryModel *results, QSqlDatabase *ptr_database, const _sys_system_id id, const int bound_sz, const string break_sz ){
	//the table is set (the name and the column names) and allocated
	try{
		Dam_Sc_Point::set_erg_table(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}

	ostringstream query_string;
	query_string <<"SELECT ";
	query_string << Dam_Sc_Point::point_erg_table->get_column_name(dam_label::point_id) << " , ";

	query_string << Dam_Sc_Point::point_erg_table->get_column_name(dam_label::affected_score) << " , ";
	query_string << Dam_Sc_Point::point_erg_table->get_column_name(dam_label::category_id) << " , ";
	query_string << Dam_Sc_Point::point_erg_table->get_column_name(dam_label::subcategory_id) << " , ";

	query_string << " ST_ASTEXT("<<Dam_Sc_Point::point_erg_table->get_column_name(dam_label::sc_point) <<")";

	query_string<<" FROM " << Dam_Sc_Point::point_erg_table->get_table_name();
	query_string << " WHERE " ;
	query_string << Dam_Sc_Point::point_erg_table->get_column_name(label::applied_flag) << "= true";
	query_string << " AND ";
	query_string << Dam_Sc_Point::point_erg_table->get_column_name(label::areastate_id) << " ="  << id.area_state;
	query_string << " AND (";
	query_string << Dam_Sc_Point::point_erg_table->get_column_name(label::measure_id) << " = " << 0 ;
	query_string << " OR " ;
	query_string << Dam_Sc_Point::point_erg_table->get_column_name(label::measure_id) << " = " << id.measure_nr;
	query_string << " ) " ;
	query_string << " AND ";
	query_string << Dam_Sc_Point::point_erg_table->get_column_name(hyd_label::sz_bound_id) << " = " << bound_sz;
	query_string << " AND ";
	query_string << Dam_Sc_Point::point_erg_table->get_column_name(risk_label::sz_break_id) << " = '" << break_sz<<"'";

	query_string << " ORDER BY ";
	query_string << Dam_Sc_Point::point_erg_table->get_column_name(dam_label::point_id) << "  ";

	Data_Base::database_request(results, query_string.str(), ptr_database);
	if(results->lastError().isValid()==true){
		Error msg;
		msg.set_msg("Dam_Sc_Point::select_data_in_erg_table(QSqlDatabase *ptr_database, const _sys_system_id id, const int bound_sz, const string break_sz )","Invalid database request", "Check the database", 2, false);
		ostringstream info;
		info << "Table Name      : " << Dam_Sc_Point::point_erg_table->get_table_name() << endl;
		info << "Table error info: " << results->lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}

	return results->rowCount();
}
//Select data in the database table for the results of the damage calculation specified by the system id and the scenario-ids (just part of it)
int Dam_Sc_Point::select_data_in_erg_table(QSqlQueryModel *results, QSqlDatabase *ptr_database, const _sys_system_id id, const int bound_sz, const string break_sz, const int offset, const int number_row ){
		//the table is set (the name and the column names) and allocated
	try{
		Dam_Sc_Point::set_erg_table(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}

	ostringstream query_string;
	query_string <<"SELECT ";
	query_string << Dam_Sc_Point::point_erg_table->get_column_name(dam_label::point_id) << " , ";

	query_string << Dam_Sc_Point::point_erg_table->get_column_name(dam_label::affected_score) << " , ";
	query_string << Dam_Sc_Point::point_erg_table->get_column_name(dam_label::category_id) << " , ";
	query_string << Dam_Sc_Point::point_erg_table->get_column_name(dam_label::subcategory_id) << " , ";

	query_string << " ST_ASTEXT("<<Dam_Sc_Point::point_erg_table->get_column_name(dam_label::sc_point) <<")";

	query_string<<" FROM " << Dam_Sc_Point::point_erg_table->get_table_name();
	query_string << " WHERE " ;
	query_string << Dam_Sc_Point::point_erg_table->get_column_name(label::applied_flag) << "= true";
	query_string << " AND ";
	query_string << Dam_Sc_Point::point_erg_table->get_column_name(label::areastate_id) << " ="  << id.area_state;
	query_string << " AND (";
	query_string << Dam_Sc_Point::point_erg_table->get_column_name(label::measure_id) << " = " << 0 ;
	query_string << " OR " ;
	query_string << Dam_Sc_Point::point_erg_table->get_column_name(label::measure_id) << " = " << id.measure_nr;
	query_string << " ) " ;
	query_string << " AND ";
	query_string << Dam_Sc_Point::point_erg_table->get_column_name(hyd_label::sz_bound_id) << " = " << bound_sz;
	query_string << " AND ";
	query_string << Dam_Sc_Point::point_erg_table->get_column_name(risk_label::sz_break_id) << " = '" << break_sz<<"'";

	query_string << " ORDER BY ";
	query_string << Dam_Sc_Point::point_erg_table->get_column_name(dam_label::point_id) << "  ";
	query_string << " LIMIT " << number_row;
	query_string << " OFFSET " << offset;

	Data_Base::database_request(results, query_string.str(), ptr_database);
	if(results->lastError().isValid()==true){
		Error msg;
		msg.set_msg("Dam_Sc_Point::select_data_in_erg_table(QSqlDatabase *ptr_database, const _sys_system_id id, const int bound_sz, const string break_sz, const int offset, const int number_row )","Invalid database request", "Check the database", 2, false);
		ostringstream info;
		info << "Table Name      : " << Dam_Sc_Point::point_erg_table->get_table_name() << endl;
		info << "Table error info: " << results->lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}

	return results->rowCount();
}
//Count data in the database table for the results of the damage calculation specified by the system id and the scenario-ids
int Dam_Sc_Point::count_data_in_erg_table(QSqlQueryModel *results, QSqlDatabase *ptr_database, const _sys_system_id id, const int bound_sz, const string break_sz ){
	//the table is set (the name and the column names) and allocated
	try{
		Dam_Sc_Point::set_erg_table(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}

	int number=0;
	ostringstream query_string;
	query_string <<"SELECT COUNT(";
	query_string << Dam_Sc_Point::point_erg_table->get_column_name(dam_label::point_id) << " ) ";

	query_string<<" FROM " << Dam_Sc_Point::point_erg_table->get_table_name();
	query_string << " WHERE " ;
	query_string << Dam_Sc_Point::point_erg_table->get_column_name(label::applied_flag) << "= true";
	query_string << " AND ";
	query_string << Dam_Sc_Point::point_erg_table->get_column_name(label::areastate_id) << " ="  << id.area_state;
	query_string << " AND (";
	query_string << Dam_Sc_Point::point_erg_table->get_column_name(label::measure_id) << " = " << 0 ;
	query_string << " OR " ;
	query_string << Dam_Sc_Point::point_erg_table->get_column_name(label::measure_id) << " = " << id.measure_nr;
	query_string << " ) " ;
	query_string << " AND ";
	query_string << Dam_Sc_Point::point_erg_table->get_column_name(hyd_label::sz_bound_id) << " = " << bound_sz;
	query_string << " AND ";
	query_string << Dam_Sc_Point::point_erg_table->get_column_name(risk_label::sz_break_id) << " = '" << break_sz<<"'";

	Data_Base::database_request(results, query_string.str(), ptr_database);
	if(results->lastError().isValid()==true){
		Error msg;
		msg.set_msg("Dam_Sc_Point::count_data_in_erg_table(QSqlDatabase *ptr_database, const _sys_system_id id, const int bound_sz, const string break_sz)","Invalid database request", "Check the database", 2, false);
		ostringstream info;
		info << "Table Name      : " << Dam_Sc_Point::point_erg_table->get_table_name() << endl;
		info << "Table error info: " << results->lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}

	if(results->rowCount()>0){
		number= results->record(0).field(0).value().toInt();
	}

	return number;
}
//Switch the applied-flag for the simple counting damage results in the database table for a defined system state
void Dam_Sc_Point::switch_applied_flag_erg_table(QSqlDatabase *ptr_database, const _sys_system_id id, const bool flag){
	//the table is set (the name and the column names) and allocated
	try{
		Dam_Sc_Point::set_erg_table(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}
	QSqlQueryModel query;

	ostringstream query_string;
	query_string <<"UPDATE ";
	query_string << Dam_Sc_Point::point_erg_table->get_table_name();
	query_string << " SET " ;
	query_string  << Dam_Sc_Point::point_erg_table->get_column_name(label::applied_flag) << " = "<< functions::convert_boolean2string(flag);
	query_string  << " WHERE ";
	query_string  << Dam_Sc_Point::point_erg_table->get_column_name(label::areastate_id) << " = "  << id.area_state;
	query_string  << " AND ";
	query_string  <<  Dam_Sc_Point::point_erg_table->get_column_name(label::measure_id) << " = " << id.measure_nr ;
	query_string  << " AND ";
	query_string  << Dam_Sc_Point::point_erg_table->get_column_name(label::applied_flag) << " = "<< functions::convert_boolean2string(!flag);

	Data_Base::database_request(&query, query_string.str(), ptr_database);
	if(query.lastError().isValid()==true){
		Error msg;
		msg.set_msg("Dam_Sc_Point::switch_applied_flag_erg_table(QSqlDatabase *ptr_database, const _sys_system_id id, const bool flag)","Invalid database request", "Check the database", 2, false);
		ostringstream info;
		info << "Table Name      : " << Dam_Sc_Point::point_erg_table->get_table_name() << endl;
		info << "Table error info: " << query.lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
}
//Switch the applied-flag for the simple counting damage results in the database table for a defined system state
void Dam_Sc_Point::switch_applied_flag_erg_table(QSqlDatabase *ptr_database, const _sys_system_id id, const int hyd_sc, const bool flag){
		//the table is set (the name and the column names) and allocated
	try{
		Dam_Sc_Point::set_erg_table(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}
	QSqlQueryModel query;

	ostringstream query_string;
	query_string <<"UPDATE ";
	query_string << Dam_Sc_Point::point_erg_table->get_table_name();
	query_string << " SET " ;
	query_string  << Dam_Sc_Point::point_erg_table->get_column_name(label::applied_flag) << " = "<< functions::convert_boolean2string(flag);
	query_string  << " WHERE ";
	query_string  << Dam_Sc_Point::point_erg_table->get_column_name(label::areastate_id) << " = "  << id.area_state;
	query_string  << " AND ";
	query_string  <<  Dam_Sc_Point::point_erg_table->get_column_name(label::measure_id) << " = " << id.measure_nr ;
	query_string  << " AND ";
	query_string  <<  Dam_Sc_Point::point_erg_table->get_column_name(hyd_label::sz_bound_id) << " = " << hyd_sc ;
	query_string  << " AND ";
	query_string  << Dam_Sc_Point::point_erg_table->get_column_name(label::applied_flag) << " = "<< functions::convert_boolean2string(!flag);

	Data_Base::database_request(&query, query_string.str(), ptr_database);
	if(query.lastError().isValid()==true){
		Error msg;
		msg.set_msg("Dam_Sc_Point::switch_applied_flag_erg_table(QSqlDatabase *ptr_database, const _sys_system_id id, const bool flag)","Invalid database request", "Check the database", 2, false);
		ostringstream info;
		info << "Table Name      : " << Dam_Sc_Point::point_erg_table->get_table_name() << endl;
		info << "Table error info: " << query.lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
}
//Switch the applied-flag for the simple counting damage points in the database table for a defined system state
void Dam_Sc_Point::switch_applied_flag_point_table(QSqlDatabase *ptr_database, const _sys_system_id id, const bool flag){
	try{
		Dam_Sc_Point::set_point_table(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}
	QSqlQueryModel query;

	ostringstream query_string;
	query_string <<"UPDATE ";
	query_string << Dam_Sc_Point::point_table->get_table_name();
	query_string << " SET " ;
	query_string  << Dam_Sc_Point::point_table->get_column_name(label::applied_flag) << " = "<< functions::convert_boolean2string(flag);
	query_string  << " WHERE ";
	query_string  << Dam_Sc_Point::point_table->get_column_name(label::areastate_id) << " = "  << id.area_state;
	query_string  << " AND ";
	query_string  <<  Dam_Sc_Point::point_table->get_column_name(label::measure_id) << " = " << id.measure_nr ;
	query_string  << " AND ";
	query_string  << Dam_Sc_Point::point_table->get_column_name(label::applied_flag) << " = "<< functions::convert_boolean2string(!flag);

	Data_Base::database_request(&query, query_string.str(), ptr_database);
	if(query.lastError().isValid()==true){
		Error msg;
		msg.set_msg("Dam_Sc_Point::switch_applied_flag_point_table(QSqlDatabase *ptr_database, const _sys_system_id id, const bool flag)","Invalid database request", "Check the database", 2, false);
		ostringstream info;
		info << "Table Name      : " << Dam_Sc_Point::point_table->get_table_name() << endl;
		info << "Table error info: " << query.lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
}
//Switch the applied-flag for the simple counting damage points in the database table for a defined system state
void Dam_Sc_Point::switch_applied_flag_point_table(QSqlDatabase *ptr_database, const _sys_system_id id, const int point_id, const bool flag){
	try{
		Dam_Sc_Point::set_point_table(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}
	QSqlQueryModel query;

	ostringstream query_string;
	query_string <<"UPDATE ";
	query_string << Dam_Sc_Point::point_table->get_table_name();
	query_string << " SET " ;
	query_string  << Dam_Sc_Point::point_table->get_column_name(label::applied_flag) << " = "<< functions::convert_boolean2string(flag);
	query_string  << " WHERE ";
	query_string  << Dam_Sc_Point::point_table->get_column_name(label::areastate_id) << " = "  << id.area_state;
	query_string  << " AND ";
	query_string  <<  Dam_Sc_Point::point_table->get_column_name(label::measure_id) << " = " << id.measure_nr ;
	query_string  << " AND ";
	query_string  << Dam_Sc_Point::point_table->get_column_name(dam_label::point_id) << " = "<< point_id;
	query_string  << " AND ";
	query_string  << Dam_Sc_Point::point_table->get_column_name(label::applied_flag) << " = "<< functions::convert_boolean2string(!flag);

	Data_Base::database_request(&query, query_string.str(), ptr_database);
	if(query.lastError().isValid()==true){
		Error msg;
		msg.set_msg("Dam_Sc_Point::switch_applied_flag_point_table(QSqlDatabase *ptr_database, const _sys_system_id id, const bool flag)","Invalid database request", "Check the database", 2, false);
		ostringstream info;
		info << "Table Name      : " << Dam_Sc_Point::point_table->get_table_name() << endl;
		info << "Table error info: " << query.lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
}
//Reswitch the applied-flag for the simple counting damage points in the database table for the base state; a defined system state and his applied elements are decisive
void Dam_Sc_Point::reswitch_applied_flag_point_table(QSqlDatabase *ptr_database, const _sys_system_id id, const bool flag){
	int number=0;
	try{
		Dam_Sc_Point::set_point_table(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}

	QSqlQueryModel results;
	ostringstream test_filter;
	test_filter<< "Select ";
	test_filter <<  Dam_Sc_Point::point_table->get_column_name(dam_label::point_id) << " , ";

	test_filter << " from " << Dam_Sc_Point::point_table->get_table_name();
	test_filter << " where " ;
	test_filter << Dam_Sc_Point::point_table->get_column_name(label::applied_flag) << "= true";
	test_filter << " and ";
	test_filter << Dam_Sc_Point::point_table->get_column_name(label::areastate_id) << " ="  << id.area_state;
	test_filter << " and ";
	test_filter << Dam_Sc_Point::point_table->get_column_name(label::measure_id) << " = " << id.measure_nr;

	Data_Base::database_request(&results, test_filter.str(), ptr_database);

	if(results.lastError().isValid()==true){
		Error msg;
		msg.set_msg("Dam_Sc_Point::reswitch_applied_flag_point_table(QSqlDatabase *ptr_database, const _sys_system_id id, const bool flag)","Invalid database request", "Check the database", 2, false);
		ostringstream info;
		info << "Table Name      : " << Dam_Sc_Point::point_table->get_table_name() << endl;
		info << "Table error info: " << results.lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}

	number=results.rowCount();
	int point_id=0;
	_sys_system_id base;
	base.area_state=id.area_state;
	base.measure_nr=0;

	for(int i=0; i< number ; i++){
		point_id=results.record(i).value((Dam_Sc_Point::point_table->get_column_name(dam_label::point_id)).c_str()).toInt();

		Dam_Sc_Point::switch_applied_flag_point_table(ptr_database, base, point_id, flag);
	}
}
//Copy the results of a given system id to another one in database tables
void Dam_Sc_Point::copy_results(QSqlDatabase *ptr_database, const _sys_system_id src, const _sys_system_id dest){
		//the table is set (the name and the column names) and allocated
	try{
		Dam_Sc_Point::set_erg_table(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}

	int glob_id=0;
	glob_id=Dam_Sc_Point::point_erg_table->maximum_int_of_column(Dam_Sc_Point::point_erg_table->get_column_name(dam_label::glob_id) ,ptr_database)+1;

	QSqlQueryModel model;
	ostringstream test_filter;
	test_filter<< "SELECT ";
	test_filter << Dam_Sc_Point::point_erg_table->get_column_name(dam_label::glob_id);
	test_filter << " from " << Dam_Sc_Point::point_erg_table->get_table_name();
	test_filter << " WHERE ";
	test_filter << Dam_Sc_Point::point_erg_table->get_column_name(label::areastate_id) << " = " << src.area_state;
	test_filter << " AND ";
	test_filter << Dam_Sc_Point::point_erg_table->get_column_name(label::measure_id) << " = " << src.measure_nr ;

	//submit it to the datbase
	Data_Base::database_request(&model, test_filter.str(), ptr_database);
	if(model.lastError().isValid()==true){
		Error msg;
		msg.set_msg("Dam_Sc_Point::copy_results(QSqlDatabase *ptr_database, const _sys_system_id src, const _sys_system_id dest)","Invalid database request", "Check the database", 2, false);
		ostringstream info;
		info << "Table Name      : " << Dam_Sc_Point::point_erg_table->get_table_name() << endl;
		info << "Table error info: " << model.lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
	test_filter.str("");

	ostringstream cout;
	cout <<"Copy "<<model.rowCount()<<" point results of the simple counting damages to the new measure state..."<<endl;
	Sys_Common_Output::output_dam->output_txt(&cout);

	QSqlQueryModel model1;
	for(int i=0; i< model.rowCount(); i++){
		if(i%10000==0 && i>0){
			cout << "Copy SC-point results "<< i <<" to " << i+10000 <<"..."<< endl;
			Sys_Common_Output::output_dam->output_txt(&cout);
		}
		test_filter.str("");
		test_filter << "INSERT INTO "<< Dam_Sc_Point::point_erg_table->get_table_name();
		test_filter << " SELECT " << glob_id <<" , ";
		test_filter <<  Dam_Sc_Point::point_erg_table->get_column_name(dam_label::point_id)  <<" , ";
		test_filter <<  dest.area_state  <<" , ";
		test_filter <<  dest.measure_nr  <<" , ";
		test_filter <<  Dam_Sc_Point::point_erg_table->get_column_name(label::applied_flag)  <<" , ";
		test_filter <<  Dam_Sc_Point::point_erg_table->get_column_name(hyd_label::sz_bound_id)  <<" , ";
		test_filter <<  Dam_Sc_Point::point_erg_table->get_column_name(risk_label::sz_break_id)  <<" , ";
		test_filter << Dam_Sc_Point::point_erg_table->get_column_name(dam_label::affected_score) << " , ";
		test_filter << Dam_Sc_Point::point_erg_table->get_column_name(dam_label::category_id) << " , ";
		test_filter << Dam_Sc_Point::point_erg_table->get_column_name(dam_label::subcategory_id) << " , ";
		test_filter <<  Dam_Sc_Point::point_erg_table->get_column_name(dam_label::sc_point)  <<"  ";

		test_filter << " FROM " << Dam_Sc_Point::point_erg_table->get_table_name() <<" ";
		test_filter << " WHERE " << Dam_Sc_Point::point_erg_table->get_column_name(dam_label::glob_id) << " = ";
		test_filter << model.record(i).value(Dam_Sc_Point::point_erg_table->get_column_name(dam_label::glob_id).c_str()).toInt();
		Data_Base::database_request(&model1, test_filter.str(), ptr_database);
		if(model1.lastError().isValid()==true){
			Error msg;
			msg.set_msg("Dam_Sc_Point::copy_results(QSqlDatabase *ptr_database, const _sys_system_id src, const _sys_system_id dest)","Invalid database request", "Check the database", 2, false);
			ostringstream info;
			info << "Table Name      : " << Dam_Sc_Point::point_erg_table->get_table_name() << endl;
			info << "Table error info: " << model1.lastError().text().toStdString() << endl;
			msg.make_second_info(info.str());
			throw msg;
		}
		glob_id++;
	}
}
//Convert category (_dam_sc_category) to string (static)
string Dam_Sc_Point::convert_category2txt(const _dam_sc_category cat){
	string buffer;
	switch(cat){
	case _dam_sc_category::pub_build:
			buffer=dam_label::sc_pub_build;
			break;
	case _dam_sc_category::eco_build:
			buffer=dam_label::sc_eco_build;
			break;
	case _dam_sc_category::cult_build:
			buffer=dam_label::sc_cult_build;
			break;
	case _dam_sc_category::person_build:
			buffer=dam_label::sc_person_build;
			break;
		default:
			buffer=label::not_defined;
	}
	return buffer;
}
//Get the category id
int Dam_Sc_Point::get_cat_id(void){
	return this->cat_id;
}
//Get the subcategory id
int Dam_Sc_Point::get_subcat_id(void){
	return this->subcat_id;
}
//Get the score value
double Dam_Sc_Point::get_score_value(void){
	return this->score_value;
}
//Get the affected score value
double Dam_Sc_Point::get_affected_score_value(void){
	if(this->is_affected==true){
		return this->score_value;
	}
	else{
		return 0.0;
	}
}
//Get the boundary value
double Dam_Sc_Point::get_boundary_value(void){
	return this->boundary_value;
}
//Set the category id, the subcategory id, the score value and the boundary value
void Dam_Sc_Point::set_members(const int cat_id, const double score, const double bound, const int subcat_id){
	this->cat_id=cat_id;
	this->subcat_id=subcat_id;
	this->score_value=score;
	this->boundary_value=bound;
	this->category=this->convert_id2enum(cat_id);
	this->check_members();
}
//Get the data-string to complete a insert-string for inserting the data of the point to database
string Dam_Sc_Point::get_datastring_members2database(const int global_id){
	string buffer=label::not_set;

	if(this->cat_id==5){
		return buffer;
	}

	ostringstream query_string ;
	query_string << " ( ";
	query_string << global_id << " , " ;
	query_string << this->number<< " , " ;
	query_string << this->system_id.area_state << " , " ;
	query_string << this->system_id.measure_nr << " , " ;
	query_string << "true" << " , " ;
	//data
	query_string << this->cat_id<< " , " ;
	query_string << "'"<< this->convert_category2txt(this->category)<< "' , " ;
	query_string << this->score_value<< " , " ;
	query_string << this->boundary_value<< " , " ;
	query_string << "'"<<this->name<< "' , " ;
	query_string << this->subcat_id<< " , " ;
	query_string << FORMAT_FIXED_REAL << P(8);
	query_string << this->get_xcoordinate() << " , " ;
	query_string << this->get_ycoordinate() << " , " ;
	query_string.clear();

	query_string << this->get_point2sql_string(this->x_coordinate, this->y_coordinate) <<" ) ";

	buffer=query_string.str();
	return buffer;
}
//Get the data-string to complete a insert-string for inserting the data of the point results to database
string Dam_Sc_Point::get_datastring_results2database(const int global_id, const int bound_sz, const string break_sz, const bool must_output){
	string buffer=label::not_set;
	if(must_output==false){
		if(this->is_affected==false){
			return buffer;
		}
	}

	//set the query via a query string
	ostringstream query_string;
	query_string << " ( ";
	query_string << global_id << " , " ;
	query_string << this->number<< " , " ;
	query_string << this->system_id.area_state << " , " ;
	query_string << this->system_id.measure_nr << " , " ;
	query_string << bound_sz << " , " ;
	query_string << "'"<<break_sz<< "' , " ;
	//results
	if(this->is_affected==true){
		query_string << this->score_value << " , " ;
	}
	else{
		query_string << 0.0 << " , " ;
	}
	query_string << this->cat_id << " , " ;
	query_string << this->subcat_id << " , " ;

	query_string << this->get_point2sql_string(this->x_coordinate, this->y_coordinate) <<" ) ";

	buffer=query_string.str();
	return buffer;
}
//Set the index of the connected floodplain
void Dam_Sc_Point::set_index_floodplain(const int index){
	this->index_fp=index;
}
//Get the index of the connected floodplain
int Dam_Sc_Point::get_index_floodplain(void){
	return this->index_fp;
}
//Set the index of the connected floodplain element
void Dam_Sc_Point::set_index_floodplain_element(const int index){
	this->index_fp_elem=index;
}
//Get the index of the connected floodplain element
int Dam_Sc_Point::get_index_floodplain_element(void){
	return this->index_fp_elem;
}
//get the flag if the point is connected
bool Dam_Sc_Point::get_connected_flag(void){
	return this->is_connected;
}
//Reset result values
void Dam_Sc_Point::reset_result_value(void){
	this->is_affected=false;
}
//Calculate the simple counting damages per point
void Dam_Sc_Point::calculate_damages(Dam_Impact_Values *impact){
	this->reset_result_value();

	if(impact->get_was_wet_flag()==false){
		return;
	}

	if(impact->get_impact_h()>=this->boundary_value){
		this->is_affected=true;
	}
}
//___________
//private
//Transfer the category id in the enum _dam_sc_category
_dam_sc_category Dam_Sc_Point::convert_id2enum(const int id){
	_dam_sc_category buffer;
	switch(id){
	case 1:
		buffer=_dam_sc_category::pub_build;
		break;
	case 2:
		buffer=_dam_sc_category::eco_build;
		break;
	case 3:
		buffer=_dam_sc_category::cult_build;
		break;
	case 4:
		buffer=_dam_sc_category::person_build;
		break;
	default:
		buffer=_dam_sc_category::nodefined_build;
	}

	return buffer;
}
//Check the points
void Dam_Sc_Point::check_members(void){
	if(this->score_value<0.0){
		ostringstream info;
		info << "Point name    : " << this->name << endl;
		info << "Point id      : " << this->number << endl;
		info << "Score value   : " << this->score_value<<label::score << endl;
		Warning msg=this->set_warning(0);
		msg.make_second_info(info.str());
		msg.output_msg(4);
		this->score_value=0.0;
	}

	if(this->boundary_value<0.0){
		ostringstream info;
		info << "Point name     : " << this->name << endl;
		info << "Point id       : " << this->number << endl;
		info << "Boundary value : " << this->boundary_value<<label::m << endl;
		Warning msg=this->set_warning(1);
		msg.make_second_info(info.str());
		msg.output_msg(4);
		this->boundary_value=0.0;
	}
	if(this->category==_dam_sc_category::nodefined_build){
		ostringstream info;
		info << "Point name     : " << this->name << endl;
		info << "Point id       : " << this->number << endl;
		info << "Possible ids and categories are : "<<endl;
		info << " 1 public buildings "<<endl;
		info << " 2 ecologic perilous sites or buildings  "<<endl;
		info << " 3 cultural heritage   "<<endl;
		info << " 4 buildings with highly vulnerable person   "<<endl;
		Warning msg=this->set_warning(2);
		msg.make_second_info(info.str());
		msg.output_msg(4);
		this->cat_id=5;
	}
}
//Set warning(s)
Warning Dam_Sc_Point::set_warning(const int warn_type){
	string place="Dam_Sc_Point::";
	string help;
	string reason;
	string reaction;
	int type=0;
	Warning msg;
	stringstream info;

	switch (warn_type){
		case 0://score value <=0
			place.append("check_members(void)") ;
			reason="Score value is < 0.0 score";
			reaction="The default value 0.0 is used";
			help= "Check the score value";
			type=3;
			break;
		case 1://boundary value <=0
			place.append("check_members(void)") ;
			reason="Boundary value is < 0.0 m";
			reaction="The default value 0.0 is used";
			help= "Check the boundary value";
			type=3;
			break;
		case 2://category is not defined
			place.append("check_members(void)") ;
			reason="The category of the simple counting point is not defined";
			reaction="It is not further taken into account";
			help= "Check the category";
			type=3;
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