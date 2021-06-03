//#include "Hyd_Element_Floodplain.h"
#include "Hyd_Headers_Precompiled.h"

//init static members
Tables *Hyd_Element_Floodplain::elem_table=NULL;
Tables *Hyd_Element_Floodplain::boundary_table=NULL;
Tables *Hyd_Element_Floodplain::erg_table=NULL;
Tables *Hyd_Element_Floodplain::erg_instat_table = NULL;

//constructor
Hyd_Element_Floodplain::Hyd_Element_Floodplain(void){
	//input
	this->elem_number=0;
	this->glob_elem_number = -1;
	this->element_type=NULL;
	this->type=_hyd_elem_type::STANDARD_ELEM;
	this->mid_point.set_point_name(label::elem_mid);
	this->buffer_boundary_data=NULL;
	this->buffer_flow_data=NULL;
	this->z_value=0.0;
	this->polygon_string=label::not_set;
	this->hyd_sz=NULL;

	try{
		this->allocate_data_buffer();
	}
	catch(Error msg){
		throw msg;
	}
	//count the memory
	Sys_Memory_Count::self()->add_mem(sizeof(Hyd_Element_Floodplain), _sys_system_modules::HYD_SYS);
}
//destructor
Hyd_Element_Floodplain::~Hyd_Element_Floodplain(void){
	this->delete_element_type();
	this->delete_data_buffer();
	this->polygon_string="";

	//count the memory
	Sys_Memory_Count::self()->minus_mem(sizeof(Hyd_Element_Floodplain), _sys_system_modules::HYD_SYS);
}
//_____________
//public
//set the input from a string which is read from a file
void Hyd_Element_Floodplain::input_value(const string line_of_file){
	try{
		stringstream my_stream;
		string boundary_str_buff;
		string stationary_str_buff;
		string boundary_type_buff;
		double buffer_bound_value;

		my_stream << line_of_file;
		my_stream >> this->elem_number>> this->z_value>> this->buffer_flow_data->mat_type>> this->buffer_flow_data->init_condition >> boundary_str_buff;
		if(my_stream.fail()==true){
			ostringstream info;
			info << "Wrong input sequenze   :" << my_stream.str() << endl;
			Error msg=this->set_error(2);
			msg.make_second_info(info.str());
			throw msg;
		}
		//transform the strings to bool flags
		this->buffer_boundary_data->boundary_flag=_Hyd_Parse_IO::transform_string2boolean(boundary_str_buff);
		if(this->buffer_boundary_data->boundary_flag==true){
			my_stream >> stationary_str_buff>> buffer_bound_value>> boundary_type_buff;
			if(my_stream.fail()==true){
				ostringstream info;
				info << "Wrong input sequenze   :" << my_stream.str() << endl;
				Error msg=this->set_error(2);
				msg.make_second_info(info.str());
				throw msg;
			}
			this->buffer_boundary_data->stationary_flag=_Hyd_Parse_IO::transform_string2boolean(stationary_str_buff);
			this->buffer_boundary_data->boundary_type=Hyd_Instationary_Boundary::transfrom_txt2_instatboundtype(boundary_type_buff);
			//decide which type of value is taken (curvenumber, discharge value)
			this->decide_boundary_value(buffer_bound_value);
		}
	}
	catch(Error msg){
		ostringstream info;
		info<<"Elementnumber          :" << this->elem_number << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
}
//Transfer the floodplain element data to a database
void Hyd_Element_Floodplain::transfer_element_members2database(const int glob_elem_id, QSqlQuery *model, QSqlDatabase *ptr_database, const int fp_number, const _hyd_floodplain_geo_info geo_info){
	//the table is set (the name and the column names) and allocated
	try{
		Hyd_Element_Floodplain::set_table(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}

	//set the query via a query string
	ostringstream query_string;
	query_string << "INSERT INTO  " << Hyd_Element_Floodplain::elem_table->get_table_name();
	query_string <<" ( ";
	query_string << Hyd_Element_Floodplain::elem_table->get_column_name(hyd_label::elemdata_fpno) <<" , ";
	query_string << Hyd_Element_Floodplain::elem_table->get_column_name(hyd_label::elemdata_id) <<" , ";
	query_string << Hyd_Element_Floodplain::elem_table->get_column_name(hyd_label::elemdata_z) <<" , ";
	query_string << Hyd_Element_Floodplain::elem_table->get_column_name(hyd_label::elemdata_matid) <<" , ";
	query_string << Hyd_Element_Floodplain::elem_table->get_column_name(hyd_label::elemdata_init) <<" , ";
	query_string << Hyd_Element_Floodplain::elem_table->get_column_name(hyd_label::elemdata_glob_id) <<" , ";
	query_string << Hyd_Element_Floodplain::elem_table->get_column_name(hyd_label::elemdata_mid_x) <<" , ";
	query_string << Hyd_Element_Floodplain::elem_table->get_column_name(hyd_label::elemdata_mid_y) <<" , ";
	query_string << Hyd_Element_Floodplain::elem_table->get_column_name(label::areastate_id) <<" , ";
	query_string << Hyd_Element_Floodplain::elem_table->get_column_name(label::measure_id) <<" , ";
	query_string << Hyd_Element_Floodplain::elem_table->get_column_name(label::applied_flag) <<" , ";

	query_string << Hyd_Element_Floodplain::elem_table->get_column_name(hyd_label::elemdata_polygon) <<" ) ";

	query_string << " VALUES ( ";
	query_string << fp_number << " , " ;
	query_string << this->elem_number<< " , " ;
	query_string << this->z_value<< " , " ;
	query_string << this->get_flow_data().mat_type << " , " ;
	query_string << this->get_flow_data().init_condition << " , " ;
	query_string <<  glob_elem_id<< " , " ;
	query_string << FORMAT_FIXED_REAL << P(8);
	query_string << this->mid_point.get_xcoordinate()<< " , " ;
	query_string << this->mid_point.get_ycoordinate()<< " , " ;
	query_string.clear();
	query_string << this->system_id.area_state<< " , " ;
	query_string << this->system_id.measure_nr<< " , " ;

	query_string << "true" << " , " ;

	if(this->polygon_string==label::not_set){
		//set the polgon points
		Geo_Raster_Polygon poly_buff;
		poly_buff.set_points(&this->mid_point, geo_info.width_x, geo_info.width_y, geo_info.angle, geo_info.origin_global_x, geo_info.origin_global_y);
		this->polygon_string=poly_buff.get_polygon2sql_string();
		query_string << this->polygon_string <<" ) ";
	}
	else{
		query_string << this->polygon_string <<" ) ";
	}

	Data_Base::database_request(model, query_string.str(), ptr_database);

	if(model->lastError().isValid()){
		Warning msg=this->set_warning(1);
		ostringstream info;
		info << "Table Name                : " << Hyd_Element_Floodplain::elem_table->get_table_name() << endl;
		info << "Table error info          : " << model->lastError().text().toStdString() << endl;
		info << "Element number            : " << this->elem_number << endl;
		msg.make_second_info(info.str());
		msg.output_msg(2);
	}

	//appending tables
	this->transfer_element_boundarydata2database(ptr_database, glob_elem_id);
}
//Get a string for transfering the data to database
string Hyd_Element_Floodplain::get_datastring2database(QSqlDatabase *ptr_database, const int glob_elem_id, const int fp_number, const _hyd_floodplain_geo_info geo_info){
	string buffer;

	ostringstream query_string;
	query_string << " ( ";
	query_string << fp_number << " , " ;
	query_string << this->elem_number<< " , " ;
	query_string << this->z_value<< " , " ;
	query_string << this->get_flow_data().mat_type << " , " ;
	query_string << this->get_flow_data().init_condition << " , " ;
	query_string <<  glob_elem_id<< " , " ;
	query_string << FORMAT_FIXED_REAL << P(8);
	query_string << this->mid_point.get_xcoordinate()<< " , " ;
	query_string << this->mid_point.get_ycoordinate()<< " , " ;
	query_string.clear();
	query_string << this->system_id.area_state<< " , " ;
	query_string << this->system_id.measure_nr<< " , " ;

	query_string << "true" << " , " ;

	if(this->polygon_string==label::not_set){
		//set the polgon points
		Geo_Raster_Polygon poly_buff;
		poly_buff.set_points(&this->mid_point, geo_info.width_x, geo_info.width_y, geo_info.angle, geo_info.origin_global_x, geo_info.origin_global_y);
		this->polygon_string=poly_buff.get_polygon2sql_string();
		query_string << this->polygon_string <<" ) ";
	}
	else{
		query_string << this->polygon_string <<" ) ";
	}

	//appending tables
	//this->transfer_element_boundarydata2database(ptr_database, glob_elem_id);
	this->glob_elem_number = glob_elem_id;

	buffer=query_string.str();
	return buffer;
}
//Get the header for inserting the element data to database table (static)
string Hyd_Element_Floodplain::get_insert_header_data_table(QSqlDatabase *ptr_database){
	try{
		Hyd_Element_Floodplain::set_table(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}
	//set the query via a query string
	ostringstream query_string;
	query_string << "INSERT INTO  " << Hyd_Element_Floodplain::elem_table->get_table_name();
	query_string <<" ( ";
	query_string << Hyd_Element_Floodplain::elem_table->get_column_name(hyd_label::elemdata_fpno) <<" , ";
	query_string << Hyd_Element_Floodplain::elem_table->get_column_name(hyd_label::elemdata_id) <<" , ";
	query_string << Hyd_Element_Floodplain::elem_table->get_column_name(hyd_label::elemdata_z) <<" , ";
	query_string << Hyd_Element_Floodplain::elem_table->get_column_name(hyd_label::elemdata_matid) <<" , ";
	query_string << Hyd_Element_Floodplain::elem_table->get_column_name(hyd_label::elemdata_init) <<" , ";
	query_string << Hyd_Element_Floodplain::elem_table->get_column_name(hyd_label::elemdata_glob_id) <<" , ";
	query_string << Hyd_Element_Floodplain::elem_table->get_column_name(hyd_label::elemdata_mid_x) <<" , ";
	query_string << Hyd_Element_Floodplain::elem_table->get_column_name(hyd_label::elemdata_mid_y) <<" , ";
	query_string << Hyd_Element_Floodplain::elem_table->get_column_name(label::areastate_id) <<" , ";
	query_string << Hyd_Element_Floodplain::elem_table->get_column_name(label::measure_id) <<" , ";
	query_string << Hyd_Element_Floodplain::elem_table->get_column_name(label::applied_flag) <<" , ";

	query_string << Hyd_Element_Floodplain::elem_table->get_column_name(hyd_label::elemdata_polygon) <<" ) ";

	query_string << " VALUES ";

	string buffer;
	buffer=query_string.str();
	return buffer;
}
//Transfer the floodplain element boundary data of an hydraulc boundary scenario to a database
void Hyd_Element_Floodplain::transfer_hydraulic_boundary_sz2database(QSqlDatabase *ptr_database, const int ){
	if(this->get_boundary_data().boundary_flag==false){
		return;
	}
	try{
		//boundary condition
		this->transfer_element_boundarydata2database(ptr_database, this->glob_elem_number);
	}
	catch(Error msg){
		ostringstream info;
		info << "Element number            : " << this->elem_number << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
}
//Input the floodplain element data with the index from the selected data set of a database table
void Hyd_Element_Floodplain::input_members(const int index, const QSqlQueryModel *query_result,  QSqlQueryModel *bound_result, const int bound_number, const bool just_elems, int *last_index){
	
	try{
		this->glob_elem_number=query_result->record(index).value((Hyd_Element_Floodplain::elem_table->get_column_name(hyd_label::elemdata_glob_id)).c_str()).toInt();
		//set the element members
		this->elem_number=query_result->record(index).value((Hyd_Element_Floodplain::elem_table->get_column_name(hyd_label::elemdata_id)).c_str()).toInt();
		this->buffer_flow_data->mat_type=query_result->record(index).value((Hyd_Element_Floodplain::elem_table->get_column_name(hyd_label::elemdata_matid)).c_str()).toInt();
		this->z_value=query_result->record(index).value((Hyd_Element_Floodplain::elem_table->get_column_name(hyd_label::elemdata_z)).c_str()).toDouble();
		this->buffer_flow_data->init_condition=query_result->record(index).value((Hyd_Element_Floodplain::elem_table->get_column_name(hyd_label::elemdata_init)).c_str()).toDouble();
		double x_mid=0.0;
		double y_mid=0.0;
		x_mid=query_result->record(index).value((Hyd_Element_Floodplain::elem_table->get_column_name(hyd_label::elemdata_mid_x)).c_str()).toDouble();
		y_mid=query_result->record(index).value((Hyd_Element_Floodplain::elem_table->get_column_name(hyd_label::elemdata_mid_y)).c_str()).toDouble();
		this->mid_point.set_point_coordinate(x_mid, y_mid);
	}
	catch(Error msg){
		ostringstream info;
		info << "Element number            : " << this->elem_number << endl;
		msg.make_second_info(info.str());
		throw msg;
	}

	if(just_elems==false){

		//set the boundary conditions
        this->input_element_boundarydata2database(bound_result, bound_number, this->glob_elem_number, last_index);
	}
}
//Set members directly
void Hyd_Element_Floodplain::set_members_directly(const int elem_number, Geo_Point* midpoint, const double z_value, const _hyd_elem_type type){
	this->elem_number=elem_number;
	this->type=type;
	this->allocate_element_type();
	this->z_value=z_value;
	this->mid_point=*midpoint;

	this->buffer_flow_data->mat_type=1;
	this->element_type->set_data_buffers(*this->buffer_boundary_data, *this->buffer_flow_data, &this->z_value);
	this->delete_data_buffer();
}
//Create the database table for the element data of the floodplain model (static)
void Hyd_Element_Floodplain::create_table(QSqlDatabase *ptr_database){
	if(Hyd_Element_Floodplain::elem_table==NULL){
			ostringstream cout;
			cout << "Create floodplain model element database table..." << endl ;
			Sys_Common_Output::output_hyd->output_txt(&cout);
			//make specific input for this class
			const string tab_name=hyd_label::tab_fpelem;
			const int num_col=12;
			_Sys_data_tab_column tab_col[num_col];
			//init
			for(int i=0; i< num_col; i++){
				tab_col[i].key_flag=false;
				tab_col[i].unsigned_flag=false;
				tab_col[i].primary_key_flag=false;
			}

			tab_col[0].name=hyd_label::elemdata_glob_id;
			tab_col[0].type=sys_label::tab_col_type_int;
			tab_col[0].unsigned_flag=true;
			tab_col[0].primary_key_flag=true;

			tab_col[1].name=hyd_label::elemdata_fpno;
			tab_col[1].type=sys_label::tab_col_type_int;
			tab_col[1].unsigned_flag=true;
			tab_col[1].key_flag=true;

			tab_col[2].name=hyd_label::elemdata_id;
			tab_col[2].type=sys_label::tab_col_type_int;
			tab_col[2].unsigned_flag=true;
			tab_col[2].default_value="0";

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

			tab_col[6].name=hyd_label::elemdata_z;
			tab_col[6].type=sys_label::tab_col_type_double;
			tab_col[6].default_value="0.0";

			tab_col[7].name=hyd_label::elemdata_matid;
			tab_col[7].type=sys_label::tab_col_type_int;
			tab_col[7].unsigned_flag=true;

			tab_col[8].name=hyd_label::elemdata_init;
			tab_col[8].type=sys_label::tab_col_type_double;
			tab_col[8].unsigned_flag=true;
			tab_col[8].default_value="0.0";

			tab_col[9].name=hyd_label::elemdata_mid_x;
			tab_col[9].type=sys_label::tab_col_type_double;
			tab_col[9].default_value="0.0";

			tab_col[10].name=hyd_label::elemdata_mid_y;
			tab_col[10].type=sys_label::tab_col_type_double;
			tab_col[10].default_value="0.0";

			tab_col[11].name=hyd_label::elemdata_polygon;
			tab_col[11].type=sys_label::tab_col_type_polygon;

			try{
				Hyd_Element_Floodplain::elem_table= new Tables();
				if(Hyd_Element_Floodplain::elem_table->create_non_existing_tables(tab_name, tab_col, num_col,ptr_database, _sys_table_type::hyd)==false){
					cout << " Table exists" << endl ;
					Sys_Common_Output::output_hyd->output_txt(&cout);
				};
			}
			catch(bad_alloc& t){
				Error msg;
				msg.set_msg("Hyd_Element_Floodplain::create_table(QSqlDatabase *ptr_database)","Can not allocate the memory", "Check the memory", 10, false);
				ostringstream info;
				info<< "Info bad alloc: " << t.what() << endl;
				msg.make_second_info(info.str());
				throw msg;
			}
			catch(Error msg){
				Hyd_Element_Floodplain::close_table();
				throw msg;
			}

		Hyd_Element_Floodplain::close_table();
	}
}
//Set the database table for the element data of the floodplain model: it sets the table name and the name of the columns and allocate them (static)
void Hyd_Element_Floodplain::set_table(QSqlDatabase *ptr_database){
	if(Hyd_Element_Floodplain::elem_table==NULL){
		//make specific input for this class
		const string tab_id_name=hyd_label::tab_fpelem;
		string tab_id_col[12];
		tab_id_col[0]=hyd_label::elemdata_fpno;
		tab_id_col[1]=hyd_label::elemdata_glob_id;
		tab_id_col[2]=hyd_label::elemdata_id;
		tab_id_col[3]=hyd_label::elemdata_z;
		tab_id_col[4]=hyd_label::elemdata_matid;
		tab_id_col[5]=hyd_label::elemdata_init;
		tab_id_col[6]=hyd_label::elemdata_mid_x;
		tab_id_col[7]=hyd_label::elemdata_mid_y;
		tab_id_col[8]=hyd_label::elemdata_polygon;
		tab_id_col[9]=label::areastate_id;
		tab_id_col[10]=label::measure_id;
		tab_id_col[11]=label::applied_flag;

		try{
			Hyd_Element_Floodplain::elem_table= new Tables(tab_id_name, tab_id_col, sizeof(tab_id_col)/sizeof(tab_id_col[0]));
			Hyd_Element_Floodplain::elem_table->set_name(ptr_database, _sys_table_type::hyd);
		}
		catch(bad_alloc& t){
			Error msg;
			msg.set_msg("Hyd_Element_Floodplain::set_table(QSqlDatabase *ptr_database)","Can not allocate the memory", "Check the memory", 10, false);
			ostringstream info;
			info<< "Info bad alloc: " << t.what() << endl;
			msg.make_second_info(info.str());
			throw msg;
		}
		catch(Error msg){
			Hyd_Element_Floodplain::close_table();
			throw msg;
		}
	}
}
//Delete all data in the database table for the floodplain element data (static)
void Hyd_Element_Floodplain::delete_data_in_table(QSqlDatabase *ptr_database){
	//the table is set (the name and the column names) and allocated
	try{
		Hyd_Element_Floodplain::set_table(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}
	//delete the table
	Hyd_Element_Floodplain::elem_table->delete_data_in_table(ptr_database);

	//appending boundary table
	Hyd_Element_Floodplain::delete_data_in_boundary_table(ptr_database);
	//appending result table
	Hyd_Element_Floodplain::delete_data_in_erg_table(ptr_database);

}
//Delete all data in the database table for the floodplain element data for the given system state (static)
void Hyd_Element_Floodplain::delete_data_in_table(QSqlDatabase *ptr_database, const _sys_system_id id){
	QSqlQueryModel results;

	try{
		Hyd_Element_Floodplain::set_table(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}
	ostringstream test_filter;

	test_filter<< "Delete ";
	test_filter << " from " << Hyd_Element_Floodplain::elem_table->get_table_name();
	test_filter  << " WHERE ";
	test_filter  << Hyd_Element_Floodplain::elem_table->get_column_name(label::areastate_id) << " = "  << id.area_state;
	test_filter  << " AND ";
	test_filter  <<  Hyd_Element_Floodplain::elem_table->get_column_name(label::measure_id) << " = " << id.measure_nr ;

	Data_Base::database_request(&results, test_filter.str(), ptr_database);

	//check the request
	if(results.lastError().isValid()){
		Error msg;
		msg.set_msg("Hyd_Element_Floodplain::delete_data_in_table(QSqlDatabase *ptr_database, const _sys_system_id id)","Invalid database request", "Check the database", 2, false);
		ostringstream info;
		info << "Table Name      : " << Hyd_Element_Floodplain::elem_table->get_table_name() << endl;
		info << "Table error info: " << results.lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
}
//Close and delete the database table for the floodplain element data and appending boundary table (static)
void Hyd_Element_Floodplain::close_table(void){
	if(Hyd_Element_Floodplain::elem_table!=NULL){
		delete Hyd_Element_Floodplain::elem_table;
		Hyd_Element_Floodplain::elem_table=NULL;
	}
	//appending boundary table
	Hyd_Element_Floodplain::close_boundary_table();
	//appending result table
	Hyd_Element_Floodplain::close_erg_table();
	Hyd_Element_Floodplain::close_erg_instat_table();

}
//Select and count the number of relevant floodplain elements for one floodplain model in a database table (static)
int Hyd_Element_Floodplain::select_relevant_elements_database(QSqlQueryModel *results, QSqlDatabase *ptr_database, const _sys_system_id id, const int fp_number, const bool with_output ){
	int number=0;
	try{
		Hyd_Element_Floodplain::set_table(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}
	if(with_output==true){
			ostringstream cout;
			cout << "Search for relevant floodplain elements in database (select)..." << endl ;
			Sys_Common_Output::output_hyd->output_txt(&cout);
	}

	ostringstream test_filter;
	test_filter<< "Select ";
	test_filter << Hyd_Element_Floodplain::elem_table->get_column_name(hyd_label::elemdata_glob_id) << " , ";
	test_filter << Hyd_Element_Floodplain::elem_table->get_column_name(hyd_label::elemdata_id) << " , ";
	test_filter << Hyd_Element_Floodplain::elem_table->get_column_name(hyd_label::elemdata_matid) << " , ";
	test_filter << Hyd_Element_Floodplain::elem_table->get_column_name(hyd_label::elemdata_z) << " , ";
	test_filter << Hyd_Element_Floodplain::elem_table->get_column_name(hyd_label::elemdata_init) << " , ";
	test_filter << Hyd_Element_Floodplain::elem_table->get_column_name(hyd_label::elemdata_mid_x) << " , ";
	test_filter << Hyd_Element_Floodplain::elem_table->get_column_name(hyd_label::elemdata_mid_y);
	test_filter << " from " << Hyd_Element_Floodplain::elem_table->get_table_name();
	test_filter << " where ";
	test_filter << Hyd_Element_Floodplain::elem_table->get_column_name(label::applied_flag) << "= true";
	test_filter << " and ";
	test_filter << Hyd_Element_Floodplain::elem_table->get_column_name(label::areastate_id) << " ="  << id.area_state;
	test_filter << " and (";
	test_filter << Hyd_Element_Floodplain::elem_table->get_column_name(label::measure_id) << " = " << 0 ;
	test_filter << " or " ;
	test_filter << Hyd_Element_Floodplain::elem_table->get_column_name(label::measure_id) << " = " << id.measure_nr;
	test_filter << " ) " ;
	test_filter << " and ";
	test_filter << Hyd_Element_Floodplain::elem_table->get_column_name(hyd_label::elemdata_fpno) << " = " << fp_number;
	test_filter << " order by " << Hyd_Element_Floodplain::elem_table->get_column_name(hyd_label::elemdata_id);

	Data_Base::database_request(results, test_filter.str(), ptr_database);

	//check the request
	if(results->lastError().isValid()){
		Error msg;
		msg.set_msg("Hyd_Element_Floodplain::select_relevant_elements_database(QSqlTableModel *results, const int fp_number, const bool with_output )","Invalid database request", "Check the database", 2, false);
		ostringstream info;
		info << "Table Name      : " << Hyd_Element_Floodplain::elem_table->get_table_name() << endl;
		info << "Table error info: " << results->lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
	number= results->rowCount();

	if(with_output==true){
		ostringstream cout;
		cout << number << " relevant floodplain element(s) are selected in database" << endl ;
		Sys_Common_Output::output_hyd->output_txt(&cout);
	}

	return number;
}
///Select and count the number of relevant floodplain elements for one floodplain model in a database table (just parts of it)
int Hyd_Element_Floodplain::select_relevant_elements_database(QSqlQueryModel *results, QSqlDatabase *ptr_database, const _sys_system_id id, const int fp_number, const int offset, const int number_rows, const bool with_output){
	int number=0;
	try{
		Hyd_Element_Floodplain::set_table(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}
	if(with_output==true){
			ostringstream cout;
			cout << "Search for relevant floodplain elements in database (select)..." << endl ;
			Sys_Common_Output::output_hyd->output_txt(&cout);
	}

	ostringstream test_filter;
	test_filter<< "Select ";
	test_filter << Hyd_Element_Floodplain::elem_table->get_column_name(hyd_label::elemdata_glob_id) << " , ";
	test_filter << Hyd_Element_Floodplain::elem_table->get_column_name(hyd_label::elemdata_id) << " , ";
	test_filter << Hyd_Element_Floodplain::elem_table->get_column_name(hyd_label::elemdata_matid) << " , ";
	test_filter << Hyd_Element_Floodplain::elem_table->get_column_name(hyd_label::elemdata_z) << " , ";
	test_filter << Hyd_Element_Floodplain::elem_table->get_column_name(hyd_label::elemdata_init) << " , ";
	test_filter << Hyd_Element_Floodplain::elem_table->get_column_name(hyd_label::elemdata_mid_x) << " , ";
	test_filter << Hyd_Element_Floodplain::elem_table->get_column_name(hyd_label::elemdata_mid_y);
	test_filter << " from " << Hyd_Element_Floodplain::elem_table->get_table_name();
	test_filter << " where ";
	test_filter << Hyd_Element_Floodplain::elem_table->get_column_name(label::applied_flag) << "= true";
	test_filter << " and ";
	test_filter << Hyd_Element_Floodplain::elem_table->get_column_name(label::areastate_id) << " ="  << id.area_state;
	test_filter << " and (";
	test_filter << Hyd_Element_Floodplain::elem_table->get_column_name(label::measure_id) << " = " << 0 ;
	test_filter << " or " ;
	test_filter << Hyd_Element_Floodplain::elem_table->get_column_name(label::measure_id) << " = " << id.measure_nr;
	test_filter << " ) " ;
	test_filter << " and ";
	test_filter << Hyd_Element_Floodplain::elem_table->get_column_name(hyd_label::elemdata_fpno) << " = " << fp_number;
	test_filter << " order by " << Hyd_Element_Floodplain::elem_table->get_column_name(hyd_label::elemdata_id);
	test_filter << " LIMIT " << number_rows;
	test_filter << " OFFSET " << offset;
	Data_Base::database_request(results, test_filter.str(), ptr_database);

	//check the request
	if(results->lastError().isValid()){
		Error msg;
		msg.set_msg("Hyd_Element_Floodplain::select_relevant_elements_database(QSqlQueryModel *results, QSqlDatabase *ptr_database, const _sys_system_id id, const int fp_number, const int offset, const int number_rows, const bool with_output)","Invalid database request", "Check the database", 2, false);
		ostringstream info;
		info << "Table Name      : " << Hyd_Element_Floodplain::elem_table->get_table_name() << endl;
		info << "Table error info: " << results->lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
	number= results->rowCount();

	if(with_output==true){
		ostringstream cout;
		cout << number << " relevant floodplain element(s) are selected in database" << endl ;
		Sys_Common_Output::output_hyd->output_txt(&cout);
	}

	return number;
}
//Count the number of relevant elements (static)
int Hyd_Element_Floodplain::count_relevant_elements_database(QSqlQueryModel *results, QSqlDatabase *ptr_database, const _sys_system_id id, const int fp_number, const bool with_output){
	int number=0;
	try{
		Hyd_Element_Floodplain::set_table(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}
	if(with_output==true){
			ostringstream cout;
			cout << "Search for relevant floodplain elements in database (count)..." << endl ;
			Sys_Common_Output::output_hyd->output_txt(&cout);
	}

	ostringstream test_filter;
	test_filter<< "Select count(";
	test_filter << Hyd_Element_Floodplain::elem_table->get_column_name(hyd_label::elemdata_glob_id) << ")  ";
	test_filter << " from " << Hyd_Element_Floodplain::elem_table->get_table_name();
	test_filter << " where ";
	test_filter << Hyd_Element_Floodplain::elem_table->get_column_name(label::applied_flag) << "= true";
	test_filter << " and ";
	test_filter << Hyd_Element_Floodplain::elem_table->get_column_name(label::areastate_id) << " ="  << id.area_state;
	test_filter << " and (";
	test_filter << Hyd_Element_Floodplain::elem_table->get_column_name(label::measure_id) << " = " << 0 ;
	test_filter << " or " ;
	test_filter << Hyd_Element_Floodplain::elem_table->get_column_name(label::measure_id) << " = " << id.measure_nr;
	test_filter << " ) " ;
	test_filter << " and ";
	test_filter << Hyd_Element_Floodplain::elem_table->get_column_name(hyd_label::elemdata_fpno) << " = " << fp_number;

	Data_Base::database_request(results, test_filter.str(), ptr_database);

	//check the request
	if(results->lastError().isValid()){
		Error msg;
		msg.set_msg("Hyd_Element_Floodplain::count_relevant_elements_database(QSqlTableModel *results, const int fp_number, const bool with_output )","Invalid database request", "Check the database", 2, false);
		ostringstream info;
		info << "Table Name      : " << Hyd_Element_Floodplain::elem_table->get_table_name() << endl;
		info << "Table error info: " << results->lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
	if(results->rowCount()>0){
		number= results->record(0).field(0).value().toInt();
	}
	if(with_output==true){
		ostringstream cout;
		cout << number << " relevant floodplain element(s) are found in database" << endl ;
		Sys_Common_Output::output_hyd->output_txt(&cout);
	}

	return number;
}
//Create the database table for the results of an hydraulic simulation for the floodplain elements (static)
void Hyd_Element_Floodplain::create_erg_table(QSqlDatabase *ptr_database){
		if(Hyd_Element_Floodplain::erg_table==NULL){
			ostringstream cout;
			cout << "Create floodplain model element result database table..." << endl ;
			Sys_Common_Output::output_hyd->output_txt(&cout);
			//make specific input for this class
			const string tab_name=hyd_label::tab_fpelem_erg_max;
			const int num_col=35;
			_Sys_data_tab_column tab_col[num_col];
			//init
			for(int i=0; i< num_col; i++){
				tab_col[i].key_flag=false;
				tab_col[i].unsigned_flag=false;
				tab_col[i].primary_key_flag=false;
			}


			tab_col[0].name=hyd_label::elemdata_glob_id;
			tab_col[0].type=sys_label::tab_col_type_int;
			tab_col[0].unsigned_flag=true;
			tab_col[0].primary_key_flag=true;

			tab_col[1].name=hyd_label::elemdata_fpno;
			tab_col[1].type=sys_label::tab_col_type_int;
			tab_col[1].unsigned_flag=true;
			tab_col[1].key_flag=true;

			tab_col[2].name=hyd_label::elemdata_id;
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

			tab_col[6].name=hyd_label::sz_bound_id;
			tab_col[6].type=sys_label::tab_col_type_int;
			tab_col[6].unsigned_flag=true;
			tab_col[6].key_flag=true;

			tab_col[7].name=risk_label::sz_break_id;
			tab_col[7].type=sys_label::tab_col_type_string;
			tab_col[7].key_flag=true;

			tab_col[8].name=hyd_label::elemerg_h_max;
			tab_col[8].type=sys_label::tab_col_type_double;
			tab_col[8].unsigned_flag=true;
			tab_col[8].default_value="0.0";

			tab_col[9].name=hyd_label::elemerg_s_max;
			tab_col[9].type=sys_label::tab_col_type_double;
			tab_col[9].default_value="0.0";

			tab_col[10].name=hyd_label::elemerg_dsdt_max;
			tab_col[10].type=sys_label::tab_col_type_double;
			tab_col[10].default_value="0.0";

			tab_col[11].name=hyd_label::elemerg_vx_max;
			tab_col[11].type=sys_label::tab_col_type_double;
			tab_col[11].default_value="0.0";

			tab_col[12].name=hyd_label::elemerg_vy_max;
			tab_col[12].type=sys_label::tab_col_type_double;
			tab_col[12].default_value="0.0";

			tab_col[13].name=hyd_label::elemerg_vtot_max;
			tab_col[13].type=sys_label::tab_col_type_double;
			tab_col[13].default_value="0.0";

			tab_col[14].name=hyd_label::elemerg_hv_max;
			tab_col[14].type=sys_label::tab_col_type_double;
			tab_col[14].default_value="0.0";

			tab_col[15].name=hyd_label::elemerg_t_first;
			tab_col[15].type=sys_label::tab_col_type_double;
			tab_col[15].default_value="-1.0";

			tab_col[16].name=hyd_label::elemerg_dur_wet;
			tab_col[16].type=sys_label::tab_col_type_double;
			tab_col[16].default_value="0.0";

			tab_col[17].name=hyd_label::elemerg_cv_bound_in;
			tab_col[17].type=sys_label::tab_col_type_double;
			tab_col[17].default_value="0.0";

			tab_col[18].name=hyd_label::elemerg_cv_bound_out;
			tab_col[18].type=sys_label::tab_col_type_double;
			tab_col[18].default_value="0.0";

			tab_col[19].name=hyd_label::elemerg_cv_struc_in;
			tab_col[19].type=sys_label::tab_col_type_double;
			tab_col[19].default_value="0.0";

			tab_col[20].name=hyd_label::elemerg_cv_struc_out;
			tab_col[20].type=sys_label::tab_col_type_double;
			tab_col[20].default_value="0.0";

			tab_col[21].name=hyd_label::elemerg_cv_dir_in;
			tab_col[21].type=sys_label::tab_col_type_double;
			tab_col[21].default_value="0.0";

			tab_col[22].name=hyd_label::elemerg_cv_dir_out;
			tab_col[22].type=sys_label::tab_col_type_double;
			tab_col[22].default_value="0.0";

			tab_col[23].name=hyd_label::elemerg_cv_rv_ov_in;
			tab_col[23].type=sys_label::tab_col_type_double;
			tab_col[23].default_value="0.0";

			tab_col[24].name=hyd_label::elemerg_cv_rv_ov_out;
			tab_col[24].type=sys_label::tab_col_type_double;
			tab_col[24].default_value="0.0";

			tab_col[25].name=hyd_label::elemerg_cv_rv_db_in;
			tab_col[25].type=sys_label::tab_col_type_double;
			tab_col[25].default_value="0.0";

			tab_col[26].name=hyd_label::elemerg_cv_rv_db_out;
			tab_col[26].type=sys_label::tab_col_type_double;
			tab_col[26].default_value="0.0";

			tab_col[27].name=hyd_label::elemerg_cv_co_ov_in;
			tab_col[27].type=sys_label::tab_col_type_double;
			tab_col[27].default_value="0.0";

			tab_col[28].name=hyd_label::elemerg_cv_co_ov_out;
			tab_col[28].type=sys_label::tab_col_type_double;
			tab_col[28].default_value="0.0";

			tab_col[29].name=hyd_label::elemerg_cv_co_db_in;
			tab_col[29].type=sys_label::tab_col_type_double;
			tab_col[29].default_value="0.0";

			tab_col[30].name=hyd_label::elemerg_cv_co_db_out;
			tab_col[30].type=sys_label::tab_col_type_double;
			tab_col[30].default_value="0.0";

			tab_col[31].name=hyd_label::elemerg_cv_fp_in;
			tab_col[31].type=sys_label::tab_col_type_double;
			tab_col[31].default_value="0.0";

			tab_col[32].name=hyd_label::elemerg_cv_fp_out;
			tab_col[32].type=sys_label::tab_col_type_double;
			tab_col[32].default_value="0.0";

			tab_col[33].name=hyd_label::elemerg_end_vol;
			tab_col[33].type=sys_label::tab_col_type_double;
			tab_col[33].default_value="0.0";

			tab_col[34].name=hyd_label::elemdata_polygon;
			tab_col[34].type=sys_label::tab_col_type_polygon;

			try{
				Hyd_Element_Floodplain::erg_table= new Tables();
				if(Hyd_Element_Floodplain::erg_table->create_non_existing_tables(tab_name, tab_col, num_col,ptr_database, _sys_table_type::hyd)==false){
					cout << " Table exists" << endl ;
					Sys_Common_Output::output_hyd->output_txt(&cout);
				};
			}
			catch(bad_alloc& t){
				Error msg;
				msg.set_msg("Hyd_Element_Floodplain::create_erg_table(QSqlDatabase *ptr_database)","Can not allocate the memory", "Check the memory", 10, false);
				ostringstream info;
				info<< "Info bad alloc: " << t.what() << endl;
				msg.make_second_info(info.str());
				throw msg;
			}
			catch(Error msg){
				Hyd_Element_Floodplain::close_erg_table();
				throw msg;
			}

		Hyd_Element_Floodplain::close_erg_table();

		//make indizes
		//the table is set (the name and the column names) and allocated
		try {
			Hyd_Element_Floodplain::set_erg_table(ptr_database);
		}
		catch (Error msg) {
			throw msg;
		}



		Hyd_Element_Floodplain::erg_table->create_spatial_index2column(ptr_database, Hyd_Element_Floodplain::erg_table->get_column_name(hyd_label::elemdata_polygon));


		Hyd_Element_Floodplain::close_erg_table();
	}
}
//Set the database table for the results of an hydraulic simulation for the floodplain elements: it sets the table name and the name of the columns and allocate them (static)
void Hyd_Element_Floodplain::set_erg_table(QSqlDatabase *ptr_database, const bool not_close){
	if(Hyd_Element_Floodplain::erg_table==NULL){
		//make specific input for this class
		const string tab_id_name=hyd_label::tab_fpelem_erg_max;
		string tab_id_col[35];
		tab_id_col[0]=hyd_label::elemdata_fpno;
		tab_id_col[1]=hyd_label::elemdata_id;
		tab_id_col[2]=label::areastate_id;
		tab_id_col[3]=label::measure_id;
		tab_id_col[4]=hyd_label::sz_bound_id;
		tab_id_col[5]=risk_label::sz_break_id;
		tab_id_col[6]=hyd_label::elemerg_h_max;
		tab_id_col[7]=hyd_label::elemerg_dsdt_max;
		tab_id_col[8]=hyd_label::elemerg_vx_max;
		tab_id_col[9]=hyd_label::elemerg_vy_max;
		tab_id_col[10]=hyd_label::elemerg_vtot_max;
		tab_id_col[11]=hyd_label::elemerg_t_first;
		tab_id_col[12]=hyd_label::elemerg_cv_bound_in;
		tab_id_col[13]=hyd_label::elemerg_cv_bound_out;
		tab_id_col[14]=hyd_label::elemerg_cv_struc_in;
		tab_id_col[15]=hyd_label::elemerg_cv_struc_out;
		tab_id_col[16]=hyd_label::elemerg_cv_dir_in;
		tab_id_col[17]=hyd_label::elemerg_cv_dir_out;
		tab_id_col[18]=hyd_label::elemerg_cv_rv_ov_in;
		tab_id_col[19]=hyd_label::elemerg_cv_rv_ov_out;
		tab_id_col[20]=hyd_label::elemerg_cv_rv_db_in;
		tab_id_col[21]=hyd_label::elemerg_cv_rv_db_out;
		tab_id_col[22]=hyd_label::elemerg_cv_co_ov_in;
		tab_id_col[23]=hyd_label::elemerg_cv_co_ov_out;
		tab_id_col[24]=hyd_label::elemerg_cv_co_db_in;
		tab_id_col[25]=hyd_label::elemerg_cv_co_db_out;
		tab_id_col[26]=hyd_label::elemerg_cv_fp_in;
		tab_id_col[27]=hyd_label::elemerg_cv_fp_out;
		tab_id_col[28]=hyd_label::elemerg_dur_wet;
		tab_id_col[29]=hyd_label::elemerg_end_vol;
		tab_id_col[30]=hyd_label::elemdata_polygon;
		tab_id_col[31]=hyd_label::elemdata_glob_id;
		tab_id_col[32]=hyd_label::elemerg_hv_max;
		tab_id_col[33]=label::applied_flag;
		tab_id_col[34]=hyd_label::elemerg_s_max;

		try{
			Hyd_Element_Floodplain::erg_table= new Tables(tab_id_name, tab_id_col, sizeof(tab_id_col)/sizeof(tab_id_col[0]));
			Hyd_Element_Floodplain::erg_table->set_name(ptr_database, _sys_table_type::hyd);
		}
		catch(bad_alloc& t){
			Error msg;
			msg.set_msg("Hyd_Element_Floodplain::set_erg_table(QSqlDatabase *ptr_database)","Can not allocate the memory", "Check the memory", 10, false);
			ostringstream info;
			info<< "Info bad alloc: " << t.what() << endl;
			msg.make_second_info(info.str());
			throw msg;
		}
		catch(Error msg){
			if(not_close==false){
				Hyd_Element_Floodplain::close_erg_table();
			}
			throw msg;
		}
	}
}
//Select and count the number of relevant results per floodplain elements for one floodplain model in a database table
int Hyd_Element_Floodplain::select_relevant_results_elements_database(QSqlQueryModel *results, QSqlDatabase *ptr_database, const _sys_system_id id, const int fp_number, const int bound_sz, const string break_sz, const bool with_output){
		int number=0;
	try{
		Hyd_Element_Floodplain::set_erg_table(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}
	if(with_output==true){
			ostringstream cout;
			cout << "Select relevant results of the floodplain elements in database..." << endl ;
			Sys_Common_Output::output_hyd->output_txt(&cout);
	}

	ostringstream test_filter;
	test_filter<< "SELECT ";
	test_filter << Hyd_Element_Floodplain::erg_table->get_column_name(hyd_label::elemdata_id) << " , ";
	test_filter << Hyd_Element_Floodplain::erg_table->get_column_name(hyd_label::elemerg_h_max) << " , ";
	test_filter << Hyd_Element_Floodplain::erg_table->get_column_name(hyd_label::elemerg_vtot_max) << " , ";
	test_filter << Hyd_Element_Floodplain::erg_table->get_column_name(hyd_label::elemerg_dsdt_max) << " , ";
	test_filter << Hyd_Element_Floodplain::erg_table->get_column_name(hyd_label::elemerg_dur_wet) << " , ";
	test_filter << Hyd_Element_Floodplain::erg_table->get_column_name(hyd_label::elemerg_t_first) << " , ";
	test_filter << Hyd_Element_Floodplain::erg_table->get_column_name(hyd_label::elemerg_hv_max) << " , ";
	test_filter << Hyd_Element_Floodplain::erg_table->get_column_name(hyd_label::elemerg_end_vol) ;
	test_filter << " FROM " << Hyd_Element_Floodplain::erg_table->get_table_name();
	test_filter << " WHERE ";
	test_filter << Hyd_Element_Floodplain::erg_table->get_column_name(risk_label::sz_break_id) << " = '" << break_sz<<"'";
	test_filter << " AND ";
	test_filter << Hyd_Element_Floodplain::erg_table->get_column_name(hyd_label::sz_bound_id) << " = " << bound_sz;
	test_filter << " AND ";
	test_filter << Hyd_Element_Floodplain::erg_table->get_column_name(label::applied_flag) << "= true";
	test_filter << " AND ";
	test_filter << Hyd_Element_Floodplain::erg_table->get_column_name(label::areastate_id) << " ="  << id.area_state;
	test_filter << " AND (";
	test_filter << Hyd_Element_Floodplain::erg_table->get_column_name(label::measure_id) << " = " << 0 ;
	test_filter << " OR " ;
	test_filter << Hyd_Element_Floodplain::erg_table->get_column_name(label::measure_id) << " = " << id.measure_nr;
	test_filter << " ) " ;
	test_filter << " AND ";
	test_filter << Hyd_Element_Floodplain::erg_table->get_column_name(hyd_label::elemdata_fpno) << " = " << fp_number;
	test_filter << " ORDER BY " << Hyd_Element_Floodplain::erg_table->get_column_name(hyd_label::elemdata_id);

	Data_Base::database_request(results, test_filter.str(), ptr_database);

	//check the request
	if(results->lastError().isValid()){
		Error msg;
		msg.set_msg("Hyd_Element_Floodplain::select_relevant_results_elements_database(QSqlTableModel *results, const _sys_system_id id, const int fp_number, , const int bound_sz, const string break_sz, const bool with_output)","Invalid database request", "Check the database", 2, false);
		ostringstream info;
		info << "Table Name      : " << Hyd_Element_Floodplain::erg_table->get_table_name() << endl;
		info << "Table error info: " << results->lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}

	number= results->rowCount();

	if(with_output==true){
		ostringstream cout;
		cout << number << " relevant results of the floodplain element(s) are selected in database" << endl ;
		Sys_Common_Output::output_hyd->output_txt(&cout);
	}

	return number;
}
//Select and count the number of relevant results per floodplain elements for one floodplain model in a database table (just part of it) (static)
int Hyd_Element_Floodplain::select_relevant_results_elements_database(QSqlQueryModel *results, QSqlDatabase *ptr_database, const _sys_system_id id, const int fp_number, const int bound_sz, const string break_sz, const int offset, const int number_rows, const bool with_output ){
			int number=0;
	try{
		Hyd_Element_Floodplain::set_erg_table(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}
	if(with_output==true){
			ostringstream cout;
			cout << "Select relevant results of the floodplain elements in database..." << endl ;
			Sys_Common_Output::output_hyd->output_txt(&cout);
	}

	ostringstream test_filter;
	test_filter<< "SELECT ";
	test_filter << Hyd_Element_Floodplain::erg_table->get_column_name(hyd_label::elemdata_id) << " , ";
	test_filter << Hyd_Element_Floodplain::erg_table->get_column_name(hyd_label::elemerg_h_max) << " , ";
	test_filter << Hyd_Element_Floodplain::erg_table->get_column_name(hyd_label::elemerg_vtot_max) << " , ";
	test_filter << Hyd_Element_Floodplain::erg_table->get_column_name(hyd_label::elemerg_dsdt_max) << " , ";
	test_filter << Hyd_Element_Floodplain::erg_table->get_column_name(hyd_label::elemerg_dur_wet) << " , ";
	test_filter << Hyd_Element_Floodplain::erg_table->get_column_name(hyd_label::elemerg_t_first) << " , ";
	test_filter << Hyd_Element_Floodplain::erg_table->get_column_name(hyd_label::elemerg_hv_max) << " , ";
	test_filter << Hyd_Element_Floodplain::erg_table->get_column_name(hyd_label::elemerg_end_vol) ;
	test_filter << " FROM " << Hyd_Element_Floodplain::erg_table->get_table_name();
	test_filter << " WHERE ";
	test_filter << Hyd_Element_Floodplain::erg_table->get_column_name(risk_label::sz_break_id) << " = '" << break_sz<<"'";
	test_filter << " AND ";
	test_filter << Hyd_Element_Floodplain::erg_table->get_column_name(hyd_label::sz_bound_id) << " = " << bound_sz;
	test_filter << " AND ";
	test_filter << Hyd_Element_Floodplain::erg_table->get_column_name(label::applied_flag) << "= true";
	test_filter << " AND ";
	test_filter << Hyd_Element_Floodplain::erg_table->get_column_name(label::areastate_id) << " ="  << id.area_state;
	test_filter << " AND (";
	test_filter << Hyd_Element_Floodplain::erg_table->get_column_name(label::measure_id) << " = " << 0 ;
	test_filter << " OR " ;
	test_filter << Hyd_Element_Floodplain::erg_table->get_column_name(label::measure_id) << " = " << id.measure_nr;
	test_filter << " ) " ;
	test_filter << " AND ";
	test_filter << Hyd_Element_Floodplain::erg_table->get_column_name(hyd_label::elemdata_fpno) << " = " << fp_number;
	test_filter << " ORDER BY " << Hyd_Element_Floodplain::erg_table->get_column_name(hyd_label::elemdata_id);
	test_filter << " LIMIT " << number_rows;
	test_filter << " OFFSET " << offset;

	Data_Base::database_request(results, test_filter.str(), ptr_database);

	//check the request
	if(results->lastError().isValid()){
		Error msg;
		msg.set_msg("Hyd_Element_Floodplain::select_relevant_results_elements_database(QSqlTableModel *results, const _sys_system_id id, const int fp_number, , const int bound_sz, const string break_sz, const int offset, const int number_rows, const bool with_output)","Invalid database request", "Check the database", 2, false);
		ostringstream info;
		info << "Table Name      : " << Hyd_Element_Floodplain::erg_table->get_table_name() << endl;
		info << "Table error info: " << results->lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}

	number= results->rowCount();

	if(with_output==true){
		ostringstream cout;
		cout << number << " relevant results of the floodplain element(s) are selected in database" << endl ;
		Sys_Common_Output::output_hyd->output_txt(&cout);
	}

	return number;
}
//Count the number of relevant results per floodplain elements for one floodplain model in a database table (static)
int Hyd_Element_Floodplain::count_relevant_results_elements_database(QSqlQueryModel *results, QSqlDatabase *ptr_database, const _sys_system_id id, const int fp_number, const int bound_sz, const string break_sz, const bool with_output){
			int number=0;
	try{
		Hyd_Element_Floodplain::set_erg_table(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}
	if(with_output==true){
			ostringstream cout;
			cout << "Search for relevant results of the floodplain elements in database..." << endl ;
			Sys_Common_Output::output_hyd->output_txt(&cout);
	}

	ostringstream test_filter;
	test_filter<< "SELECT COUNT(";
	test_filter << Hyd_Element_Floodplain::erg_table->get_column_name(hyd_label::elemdata_id) << " ) ";
	test_filter << " FROM " << Hyd_Element_Floodplain::erg_table->get_table_name();
	test_filter << " WHERE ";
	test_filter << Hyd_Element_Floodplain::erg_table->get_column_name(risk_label::sz_break_id) << " = '" << break_sz<<"'";
	test_filter << " AND ";
	test_filter << Hyd_Element_Floodplain::erg_table->get_column_name(hyd_label::sz_bound_id) << " = " << bound_sz;
	test_filter << " AND ";
	test_filter << Hyd_Element_Floodplain::erg_table->get_column_name(label::applied_flag) << "= true";
	test_filter << " AND ";
	test_filter << Hyd_Element_Floodplain::erg_table->get_column_name(label::areastate_id) << " ="  << id.area_state;
	test_filter << " AND (";
	test_filter << Hyd_Element_Floodplain::erg_table->get_column_name(label::measure_id) << " = " << 0 ;
	test_filter << " OR " ;
	test_filter << Hyd_Element_Floodplain::erg_table->get_column_name(label::measure_id) << " = " << id.measure_nr;
	test_filter << " ) " ;
	test_filter << " AND ";
	test_filter << Hyd_Element_Floodplain::erg_table->get_column_name(hyd_label::elemdata_fpno) << " = " << fp_number;

	Data_Base::database_request(results, test_filter.str(), ptr_database);

	//check the request
	if(results->lastError().isValid()){
		Error msg;
		msg.set_msg("Hyd_Element_Floodplain::count_relevant_results_elements_database(QSqlTableModel *results, const _sys_system_id id, const int fp_number, , const int bound_sz, const string break_sz, const bool with_output)","Invalid database request", "Check the database", 2, false);
		ostringstream info;
		info << "Table Name      : " << Hyd_Element_Floodplain::erg_table->get_table_name() << endl;
		info << "Table error info: " << results->lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}

	if(results->rowCount()>0){
		number= results->record(0).field(0).value().toInt();
	}

	if(with_output==true){
		ostringstream cout;
		cout << number << " relevant results of the floodplain element(s) are found in database" << endl ;
		Sys_Common_Output::output_hyd->output_txt(&cout);
	}

	return number;
}
//Select the data in the database table for the maximum results of the floodplain specified by the system id and the scenario-ids (static)
int Hyd_Element_Floodplain::select_data_in_erg_table(QSqlQueryModel *query, QSqlDatabase *ptr_database, const _sys_system_id id, const int bound_sz, const string break_sz , const bool like_flag){
	int number=0;
	try{
		Hyd_Element_Floodplain::set_erg_table(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}

	ostringstream test_filter;
	test_filter<< "SELECT ";
	test_filter << Hyd_Element_Floodplain::erg_table->get_column_name(hyd_label::elemdata_id) << " , ";
	test_filter << Hyd_Element_Floodplain::erg_table->get_column_name(hyd_label::elemerg_h_max) << " , ";
	test_filter << Hyd_Element_Floodplain::erg_table->get_column_name(hyd_label::elemerg_vtot_max) << " , ";
	test_filter << Hyd_Element_Floodplain::erg_table->get_column_name(hyd_label::elemerg_dsdt_max) << " , ";
	test_filter << Hyd_Element_Floodplain::erg_table->get_column_name(hyd_label::elemerg_dur_wet) << " , ";
	test_filter << Hyd_Element_Floodplain::erg_table->get_column_name(hyd_label::elemerg_t_first) << " , ";
	test_filter << Hyd_Element_Floodplain::erg_table->get_column_name(hyd_label::elemerg_hv_max) << " , ";
	test_filter << Hyd_Element_Floodplain::erg_table->get_column_name(hyd_label::elemerg_end_vol) ;
	test_filter << " FROM " << Hyd_Element_Floodplain::erg_table->get_table_name();
	test_filter << " WHERE ";
	test_filter << Hyd_Element_Floodplain::erg_table->get_column_name(risk_label::sz_break_id) << " = '" << break_sz<<"'";
	test_filter << " AND ";
	test_filter << Hyd_Element_Floodplain::erg_table->get_column_name(hyd_label::sz_bound_id) << " = " << bound_sz;
	test_filter << " AND ";
	test_filter << Hyd_Element_Floodplain::erg_table->get_column_name(label::applied_flag) << "= true";
	test_filter << " AND ";
	test_filter << Hyd_Element_Floodplain::erg_table->get_column_name(label::areastate_id) << " ="  << id.area_state;
	test_filter << " AND (";
	test_filter << Hyd_Element_Floodplain::erg_table->get_column_name(label::measure_id) << " = " << 0 ;
	test_filter << " OR " ;
	test_filter << Hyd_Element_Floodplain::erg_table->get_column_name(label::measure_id) << " = " << id.measure_nr;
	test_filter << " ) " ;
	test_filter << " AND ";
	if(like_flag==false){
		test_filter << Hyd_Element_Floodplain::erg_table->get_column_name(risk_label::sz_break_id) << " = '" << break_sz<<"'";
	}
	else{
		test_filter << Hyd_Element_Floodplain::erg_table->get_column_name(risk_label::sz_break_id) << " LIKE '" << break_sz<<"'";
	}
	test_filter << " ORDER BY " << Hyd_Element_Floodplain::erg_table->get_column_name(hyd_label::elemdata_id);

	Data_Base::database_request(query, test_filter.str(), ptr_database);

	//check the request
	if(query->lastError().isValid()){
		Error msg;
		msg.set_msg("Hyd_Element_Floodplain::select_data_in_erg_table(QSqlQueryModel *query, QSqlDatabase *ptr_database, const _sys_system_id id, const int bound_sz, const string break_sz , const bool like_flag)","Invalid database request", "Check the database", 2, false);
		ostringstream info;
		info << "Table Name      : " << Hyd_Element_Floodplain::erg_table->get_table_name() << endl;
		info << "Table error info: " << query->lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}

	number= query->rowCount();

	return number;
}
//Select the data if a flood szenario is calculated specified by the system id and the scenario-ids (static)
bool Hyd_Element_Floodplain::check_calc_in_erg_table(QSqlQueryModel *query, QSqlDatabase *ptr_database, const _sys_system_id id, const int bound_sz, const string break_sz , const bool like_flag){
    bool calc=false;
    try{
        Hyd_Element_Floodplain::set_erg_table(ptr_database);
    }
    catch(Error msg){
        throw msg;
    }

    ostringstream test_filter;
    test_filter<< "SELECT EXISTS (SELECT ";
    test_filter << Hyd_Element_Floodplain::erg_table->get_column_name(hyd_label::elemdata_id);

    test_filter << " FROM " << Hyd_Element_Floodplain::erg_table->get_table_name();
    test_filter << " WHERE ";
    test_filter << Hyd_Element_Floodplain::erg_table->get_column_name(risk_label::sz_break_id) << " = '" << break_sz<<"'";
    test_filter << " AND ";
    test_filter << Hyd_Element_Floodplain::erg_table->get_column_name(hyd_label::sz_bound_id) << " = " << bound_sz;
    test_filter << " AND ";
    test_filter << Hyd_Element_Floodplain::erg_table->get_column_name(label::applied_flag) << "= true";
    test_filter << " AND ";
    test_filter << Hyd_Element_Floodplain::erg_table->get_column_name(label::areastate_id) << " ="  << id.area_state;
    test_filter << " AND (";
    test_filter << Hyd_Element_Floodplain::erg_table->get_column_name(label::measure_id) << " = " << 0 ;
    test_filter << " OR " ;
    test_filter << Hyd_Element_Floodplain::erg_table->get_column_name(label::measure_id) << " = " << id.measure_nr;
    test_filter << " ) " ;
    test_filter << " AND ";
    if(like_flag==false){
        test_filter << Hyd_Element_Floodplain::erg_table->get_column_name(risk_label::sz_break_id) << " = '" << break_sz<<"'";
    }
    else{
        test_filter << Hyd_Element_Floodplain::erg_table->get_column_name(risk_label::sz_break_id) << " LIKE '" << break_sz<<"'";
    }
    test_filter << " ) ";

    Data_Base::database_request(query, test_filter.str(), ptr_database);

    //check the request
    if(query->lastError().isValid()){
        Error msg;
        msg.set_msg("Hyd_Element_Floodplain::check_calc_in_erg_table(QSqlQueryModel *query, QSqlDatabase *ptr_database, const _sys_system_id id, const int bound_sz, const string break_sz , const bool like_flag)","Invalid database request", "Check the database", 2, false);
        ostringstream info;
        info << "Table Name      : " << Hyd_Element_Floodplain::erg_table->get_table_name() << endl;
        info << "Table error info: " << query->lastError().text().toStdString() << endl;
        msg.make_second_info(info.str());
        throw msg;
    }

    calc= query->record(0).field(0).value().toBool();

    return calc;
}
//Select the data in the database table for the instationary results of the river profiles specified by the system id and the scenario-ids (static)
int Hyd_Element_Floodplain::select_data_in_instat_erg_table(QSqlQueryModel *query, QSqlDatabase *ptr_database, const _sys_system_id id, const int bound_sz, const string break_sz, const bool like_flag) {


	int number = 0;
	try {
		Hyd_Element_Floodplain::set_erg_instat_table(ptr_database);
	}
	catch (Error msg) {
		throw msg;
	}


	ostringstream test_filter;
	test_filter << "SELECT ";
	test_filter << Hyd_Element_Floodplain::erg_instat_table->get_column_name(hyd_label::elemdata_glob_id) << " , ";
	test_filter << Hyd_Element_Floodplain::erg_instat_table->get_column_name(hyd_label::elemdata_fpno) << " , ";
	test_filter << Hyd_Element_Floodplain::erg_instat_table->get_column_name(hyd_label::elemdata_id) << " , ";
	test_filter << Hyd_Element_Floodplain::erg_instat_table->get_column_name(hyd_label::elemerg_h_max) << " , ";
	test_filter << Hyd_Element_Floodplain::erg_instat_table->get_column_name(hyd_label::elemerg_vtot_max) << " , ";
	test_filter << Hyd_Element_Floodplain::erg_instat_table->get_column_name(hyd_label::elemerg_dsdt_max) << " , ";
	test_filter << Hyd_Element_Floodplain::erg_instat_table->get_column_name(hyd_label::data_time) << " , ";
	test_filter << Hyd_Element_Floodplain::erg_instat_table->get_column_name(hyd_label::elemerg_hv_max) << "  ";

	test_filter << " FROM " << Hyd_Element_Floodplain::erg_instat_table->get_table_name();
	test_filter << " WHERE ";
	test_filter << Hyd_Element_Floodplain::erg_instat_table->get_column_name(risk_label::sz_break_id) << " = '" << break_sz << "'";
	test_filter << " AND ";
	test_filter << Hyd_Element_Floodplain::erg_instat_table->get_column_name(hyd_label::sz_bound_id) << " = " << bound_sz;
	test_filter << " AND ";
	test_filter << Hyd_Element_Floodplain::erg_instat_table->get_column_name(label::applied_flag) << "= true";
	test_filter << " AND ";
	test_filter << Hyd_Element_Floodplain::erg_instat_table->get_column_name(label::areastate_id) << " =" << id.area_state;
	test_filter << " AND (";
	test_filter << Hyd_Element_Floodplain::erg_instat_table->get_column_name(label::measure_id) << " = " << 0;
	test_filter << " OR ";
	test_filter << Hyd_Element_Floodplain::erg_instat_table->get_column_name(label::measure_id) << " = " << id.measure_nr;
	test_filter << " ) ";
	test_filter << " AND ";
	if (like_flag == false) {
		test_filter << Hyd_Element_Floodplain::erg_instat_table->get_column_name(risk_label::sz_break_id) << " = '" << break_sz << "'";
	}
	else {
		test_filter << Hyd_Element_Floodplain::erg_instat_table->get_column_name(risk_label::sz_break_id) << " LIKE '" << break_sz << "'";
	}
	test_filter << " ORDER BY " << Hyd_Element_Floodplain::erg_instat_table->get_column_name(hyd_label::elemdata_glob_id);

	Data_Base::database_request(query, test_filter.str(), ptr_database);

	//check the request
	if (query->lastError().isValid()) {
		Error msg;
		msg.set_msg("Hyd_Element_Floodplain::select_data_in_instat_erg_table(QSqlQueryModel *query, QSqlDatabase *ptr_database, const _sys_system_id id, const int bound_sz, const string break_sz , const bool like_flag)", "Invalid database request", "Check the database", 2, false);
		ostringstream info;
		info << "Table Name      : " << Hyd_Element_Floodplain::erg_instat_table->get_table_name() << endl;
		info << "Table error info: " << query->lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}

	number = query->rowCount();

	return number;
}
//Select the data if a flood szenario is calculated specified by the system id and the scenario-ids (static)
bool Hyd_Element_Floodplain::check_calc_in_instat_erg_table(QSqlQueryModel *query, QSqlDatabase *ptr_database, const _sys_system_id id, const int bound_sz, const string break_sz, const bool like_flag) {

	bool calc = false;
	try {
		Hyd_Element_Floodplain::set_erg_instat_table(ptr_database);
	}
	catch (Error msg) {
		throw msg;
	}

	ostringstream test_filter;
	test_filter << "SELECT EXISTS (SELECT ";
	test_filter << Hyd_Element_Floodplain::erg_instat_table->get_column_name(hyd_label::elemdata_id);

	test_filter << " FROM " << Hyd_Element_Floodplain::erg_instat_table->get_table_name();
	test_filter << " WHERE ";
	test_filter << Hyd_Element_Floodplain::erg_instat_table->get_column_name(risk_label::sz_break_id) << " = '" << break_sz << "'";
	test_filter << " AND ";
	test_filter << Hyd_Element_Floodplain::erg_instat_table->get_column_name(hyd_label::sz_bound_id) << " = " << bound_sz;
	test_filter << " AND ";
	test_filter << Hyd_Element_Floodplain::erg_instat_table->get_column_name(label::applied_flag) << "= true";
	test_filter << " AND ";
	test_filter << Hyd_Element_Floodplain::erg_instat_table->get_column_name(label::areastate_id) << " =" << id.area_state;
	test_filter << " AND (";
	test_filter << Hyd_Element_Floodplain::erg_instat_table->get_column_name(label::measure_id) << " = " << 0;
	test_filter << " OR ";
	test_filter << Hyd_Element_Floodplain::erg_instat_table->get_column_name(label::measure_id) << " = " << id.measure_nr;
	test_filter << " ) ";
	test_filter << " AND ";
	if (like_flag == false) {
		test_filter << Hyd_Element_Floodplain::erg_instat_table->get_column_name(risk_label::sz_break_id) << " = '" << break_sz << "'";
	}
	else {
		test_filter << Hyd_Element_Floodplain::erg_instat_table->get_column_name(risk_label::sz_break_id) << " LIKE '" << break_sz << "'";
	}
	test_filter << " ) ";

	Data_Base::database_request(query, test_filter.str(), ptr_database);

	//check the request
	if (query->lastError().isValid()) {
		Error msg;
		msg.set_msg("Hyd_Element_Floodplain::check_calc_in_instat_erg_table(QSqlQueryModel *query, QSqlDatabase *ptr_database, const _sys_system_id id, const int bound_sz, const string break_sz , const bool like_flag)", "Invalid database request", "Check the database", 2, false);
		ostringstream info;
		info << "Table Name      : " << Hyd_Element_Floodplain::erg_instat_table->get_table_name() << endl;
		info << "Table error info: " << query->lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}

	calc = query->record(0).field(0).value().toBool();

	return calc;

}
//Delete the results data in the result database table for a given boundary scenario (static)
void Hyd_Element_Floodplain::delete_results_by_scenario(QSqlDatabase *ptr_database, const int sc_id){
	QSqlQueryModel results;

	try{
		Hyd_Element_Floodplain::set_erg_table(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}
	ostringstream test_filter;

	test_filter<< "DELETE ";
	test_filter << " FROM " << Hyd_Element_Floodplain::erg_table->get_table_name();
	test_filter << " WHERE ";
	test_filter << Hyd_Element_Floodplain::erg_table->get_column_name(hyd_label::sz_bound_id) << " = " << sc_id;

	Data_Base::database_request(&results, test_filter.str(), ptr_database);

	//check the request
	if(results.lastError().isValid()){
		Error msg;
		msg.set_msg("Hyd_Element_Floodplain::delete_results_by_sceneario(QSqlDatabase *ptr_database, const int sc_id)","Invalid database request", "Check the database", 2, false);
		ostringstream info;
		info << "Table Name      : " << Hyd_Element_Floodplain::erg_table->get_table_name() << endl;
		info << "Table error info: " << results.lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
	//instationary results
	results.clear();

	try {
		Hyd_Element_Floodplain::set_erg_instat_table(ptr_database);
	}
	catch (Error msg) {
		throw msg;
	}
	test_filter.str("");
	test_filter.clear();

	test_filter << "DELETE ";
	test_filter << " FROM " << Hyd_Element_Floodplain::erg_instat_table->get_table_name();
	test_filter << " WHERE ";
	test_filter << Hyd_Element_Floodplain::erg_instat_table->get_column_name(hyd_label::sz_bound_id) << " = " << sc_id;

	Data_Base::database_request(&results, test_filter.str(), ptr_database);

	//check the request
	if (results.lastError().isValid()) {
		Error msg;
		msg.set_msg("Hyd_Element_Floodplain::delete_results_by_sceneario(QSqlDatabase *ptr_database, const int sc_id)", "Invalid database request", "Check the database", 2, false);
		ostringstream info;
		info << "Table Name      : " << Hyd_Element_Floodplain::erg_instat_table->get_table_name() << endl;
		info << "Table error info: " << results.lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}


}
//Delete the results data in the result database table for a the specified parameters (static)
void Hyd_Element_Floodplain::delete_results_in_table(QSqlDatabase *ptr_database, const _sys_system_id id, const string break_sz, const bool like_flag){
	QSqlQueryModel results;

	try{
		Hyd_Element_Floodplain::set_erg_table(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}
	ostringstream test_filter;

	test_filter<< "DELETE ";
	test_filter << " FROM " << Hyd_Element_Floodplain::erg_table->get_table_name();
	test_filter << " WHERE ";
	test_filter << Hyd_Element_Floodplain::erg_table->get_column_name(label::areastate_id) << " ="  << id.area_state;
	test_filter << " AND ";
	test_filter << Hyd_Element_Floodplain::erg_table->get_column_name(label::measure_id) << " = " << id.measure_nr;
	test_filter << " AND ";
	if(like_flag==false){
		test_filter << Hyd_Element_Floodplain::erg_table->get_column_name(risk_label::sz_break_id) << " = '" << break_sz<<"'";
	}
	else{
		test_filter << Hyd_Element_Floodplain::erg_table->get_column_name(risk_label::sz_break_id) << " LIKE '" << break_sz<<"'";
	}
	Data_Base::database_request(&results, test_filter.str(), ptr_database);

	//check the request
	if(results.lastError().isValid()){
		Error msg;
		msg.set_msg("Hyd_Element_Floodplain::delete_results_in_table(QSqlDatabase *ptr_database, const _sys_system_id id, const string break_sz, const bool like_flag)","Invalid database request", "Check the database", 2, false);
		ostringstream info;
		info << "Table Name      : " << Hyd_Element_Floodplain::erg_table->get_table_name() << endl;
		info << "Table error info: " << results.lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}

	results.clear();
	//instationary results

	try {
		Hyd_Element_Floodplain::set_erg_instat_table(ptr_database);
	}
	catch (Error msg) {
		throw msg;
	}
	test_filter.str("");
	test_filter.clear();

	test_filter << "DELETE ";
	test_filter << " FROM " << Hyd_Element_Floodplain::erg_instat_table->get_table_name();
	test_filter << " WHERE ";
	test_filter << Hyd_Element_Floodplain::erg_instat_table->get_column_name(label::areastate_id) << " =" << id.area_state;
	test_filter << " AND ";
	test_filter << Hyd_Element_Floodplain::erg_instat_table->get_column_name(label::measure_id) << " = " << id.measure_nr;
	test_filter << " AND ";
	if (like_flag == false) {
		test_filter << Hyd_Element_Floodplain::erg_instat_table->get_column_name(risk_label::sz_break_id) << " = '" << break_sz << "'";
	}
	else {
		test_filter << Hyd_Element_Floodplain::erg_instat_table->get_column_name(risk_label::sz_break_id) << " LIKE '" << break_sz << "'";
	}
	Data_Base::database_request(&results, test_filter.str(), ptr_database);

	//check the request
	if (results.lastError().isValid()) {
		Error msg;
		msg.set_msg("Hyd_Element_Floodplain::delete_results_in_table(QSqlDatabase *ptr_database, const _sys_system_id id, const string break_sz, const bool like_flag)", "Invalid database request", "Check the database", 2, false);
		ostringstream info;
		info << "Table Name      : " << Hyd_Element_Floodplain::erg_instat_table->get_table_name() << endl;
		info << "Table error info: " << results.lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}

}
//Delete the results data in the result database table for a the specified parameters (static)
void Hyd_Element_Floodplain::delete_results_in_table(QSqlDatabase *ptr_database, const _sys_system_id id, const int sc_id, const string break_sz, const bool like_flag){
		QSqlQueryModel results;

	try{
		Hyd_Element_Floodplain::set_erg_table(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}
	ostringstream test_filter;

	test_filter<< "DELETE ";
	test_filter << " FROM " << Hyd_Element_Floodplain::erg_table->get_table_name();
	test_filter << " WHERE ";
	test_filter << Hyd_Element_Floodplain::erg_table->get_column_name(label::areastate_id) << " ="  << id.area_state;
	test_filter << " AND ";
	test_filter << Hyd_Element_Floodplain::erg_table->get_column_name(label::measure_id) << " = " << id.measure_nr;
	test_filter << " AND ";
	test_filter << Hyd_Element_Floodplain::erg_table->get_column_name(hyd_label::sz_bound_id) << " = " << sc_id;
	test_filter << " AND ";
	if(like_flag==false){
		test_filter << Hyd_Element_Floodplain::erg_table->get_column_name(risk_label::sz_break_id) << " = '" << break_sz<<"'";
	}
	else{
		test_filter << Hyd_Element_Floodplain::erg_table->get_column_name(risk_label::sz_break_id) << " LIKE '" << break_sz<<"'";
	}
	Data_Base::database_request(&results, test_filter.str(), ptr_database);

	//check the request
	if(results.lastError().isValid()){
		Error msg;
		msg.set_msg("Hyd_Element_Floodplain::delete_results_in_table(QSqlDatabase *ptr_database, const _sys_system_id id, const int sc_id, const string break_sz, const bool like_flag)","Invalid database request", "Check the database", 2, false);
		ostringstream info;
		info << "Table Name      : " << Hyd_Element_Floodplain::erg_table->get_table_name() << endl;
		info << "Table error info: " << results.lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}

	results.clear();

	//instationary results

	try {
		Hyd_Element_Floodplain::set_erg_instat_table(ptr_database);
	}
	catch (Error msg) {
		throw msg;
	}
	test_filter.str("");
	test_filter.clear();

	test_filter << "DELETE ";
	test_filter << " FROM " << Hyd_Element_Floodplain::erg_instat_table->get_table_name();
	test_filter << " WHERE ";
	test_filter << Hyd_Element_Floodplain::erg_instat_table->get_column_name(label::areastate_id) << " =" << id.area_state;
	test_filter << " AND ";
	test_filter << Hyd_Element_Floodplain::erg_instat_table->get_column_name(label::measure_id) << " = " << id.measure_nr;
	test_filter << " AND ";
	test_filter << Hyd_Element_Floodplain::erg_instat_table->get_column_name(hyd_label::sz_bound_id) << " = " << sc_id;
	test_filter << " AND ";
	if (like_flag == false) {
		test_filter << Hyd_Element_Floodplain::erg_instat_table->get_column_name(risk_label::sz_break_id) << " = '" << break_sz << "'";
	}
	else {
		test_filter << Hyd_Element_Floodplain::erg_instat_table->get_column_name(risk_label::sz_break_id) << " LIKE '" << break_sz << "'";
	}
	Data_Base::database_request(&results, test_filter.str(), ptr_database);

	//check the request
	if (results.lastError().isValid()) {
		Error msg;
		msg.set_msg("Hyd_Element_Floodplain::delete_results_in_table(QSqlDatabase *ptr_database, const _sys_system_id id, const int sc_id, const string break_sz, const bool like_flag)", "Invalid database request", "Check the database", 2, false);
		ostringstream info;
		info << "Table Name      : " << Hyd_Element_Floodplain::erg_instat_table->get_table_name() << endl;
		info << "Table error info: " << results.lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}


}
//Close and delete the database table for the results of an hydraulic simulation for the floodplain elements (static)
void Hyd_Element_Floodplain::close_erg_table(void){
	if(Hyd_Element_Floodplain::erg_table!=NULL){
		delete Hyd_Element_Floodplain::erg_table;
		Hyd_Element_Floodplain::erg_table=NULL;
	}
}
//Get the maximum value of the global index of the element result database table (static)
int Hyd_Element_Floodplain::get_max_glob_id_erg_table(QSqlDatabase *ptr_database){
	//the table is set (the name and the column names) and allocated
	try{
		Hyd_Element_Floodplain::set_erg_table(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}

	//evaluate the global identifier
	int id_glob=Hyd_Element_Floodplain::erg_table->maximum_int_of_column(Hyd_Element_Floodplain::erg_table->get_column_name(hyd_label::elemdata_glob_id),ptr_database);
	return id_glob;
}
//Delete all data in the database table for the results of an hydraulic simulation for the floodplain elements (static)
void Hyd_Element_Floodplain::delete_data_in_erg_table(QSqlDatabase *ptr_database){
	//the table is set (the name and the column names) and allocated
	try{
		Hyd_Element_Floodplain::set_erg_table(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}
	//delete data in the table
	Hyd_Element_Floodplain::erg_table->delete_data_in_table(ptr_database);

	//instationary results
	//the table is set (the name and the column names) and allocated
	try {
		Hyd_Element_Floodplain::set_erg_instat_table(ptr_database);
	}
	catch (Error msg) {
		throw msg;
	}
	//delete data in the table
	Hyd_Element_Floodplain::erg_instat_table->delete_data_in_table(ptr_database);
}
//Delete the data in the database table for the results of the hydraulic calculation for a floodplain specified by the system state (static)
void Hyd_Element_Floodplain::delete_data_in_erg_table(QSqlDatabase *ptr_database, const _sys_system_id id){
	//the table is set (the name and the column names) and allocated
	try{
		Hyd_Element_Floodplain::set_erg_table(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}

	//delete the table
	QSqlQuery query(*ptr_database);
	ostringstream query_string;
	query_string <<"DELETE FROM " << Hyd_Element_Floodplain::erg_table->get_table_name();
	query_string << " WHERE " ;
	query_string << Hyd_Element_Floodplain::erg_table->get_column_name(label::areastate_id) << " ="  << id.area_state;
	query_string << " AND ";
	query_string << Hyd_Element_Floodplain::erg_table->get_column_name(label::measure_id) << " = " << id.measure_nr;

	Data_Base::database_request(&query, query_string.str(), ptr_database);
	if(query.lastError().isValid()==true){
		Error msg;
		msg.set_msg("Hyd_Element_Floodplain::delete_data_in_erg_table(QSqlDatabase *ptr_database, const _sys_system_id id)","Invalid database request", "Check the database", 2, false);
		ostringstream info;
		info << "Table Name      : " << Hyd_Element_Floodplain::erg_table->get_table_name() << endl;
		info << "Table error info: " << query.lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}

	//instationary results
	try {
		Hyd_Element_Floodplain::set_erg_instat_table(ptr_database);
	}
	catch (Error msg) {
		throw msg;
	}

	//delete the table
	QSqlQuery query2(*ptr_database);
	query_string.str("");
	query_string.clear();
	query_string << "DELETE FROM " << Hyd_Element_Floodplain::erg_instat_table->get_table_name();
	query_string << " WHERE ";
	query_string << Hyd_Element_Floodplain::erg_instat_table->get_column_name(label::areastate_id) << " =" << id.area_state;
	query_string << " AND ";
	query_string << Hyd_Element_Floodplain::erg_instat_table->get_column_name(label::measure_id) << " = " << id.measure_nr;

	Data_Base::database_request(&query2, query_string.str(), ptr_database);
	if (query2.lastError().isValid() == true) {
		Error msg;
		msg.set_msg("Hyd_Element_Floodplain::delete_data_in_erg_table(QSqlDatabase *ptr_database, const _sys_system_id id)", "Invalid database request", "Check the database", 2, false);
		ostringstream info;
		info << "Table Name      : " << Hyd_Element_Floodplain::erg_instat_table->get_table_name() << endl;
		info << "Table error info: " << query.lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
}
//Delete the data in the database table for the results of the hydraulic calculation for a floodplain specified by the system id and the scenario-ids (static)
void Hyd_Element_Floodplain::delete_data_in_erg_table(QSqlDatabase *ptr_database, const _sys_system_id id, const int bound_sz, const string break_sz ){
	//the table is set (the name and the column names) and allocated
	try{
		Hyd_Element_Floodplain::set_erg_table(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}

	//delete the table
	QSqlQuery query(*ptr_database);
	ostringstream query_string;
	query_string <<"DELETE FROM " << Hyd_Element_Floodplain::erg_table->get_table_name();
	query_string << " WHERE " ;
	query_string << Hyd_Element_Floodplain::erg_table->get_column_name(label::areastate_id) << " ="  << id.area_state;
	query_string << " AND ";
	query_string << Hyd_Element_Floodplain::erg_table->get_column_name(label::measure_id) << " = " << id.measure_nr;
	query_string << " AND ";
	query_string << Hyd_Element_Floodplain::erg_table->get_column_name(hyd_label::sz_bound_id) << " = " << bound_sz;
	query_string << " AND ";
	query_string << Hyd_Element_Floodplain::erg_table->get_column_name(risk_label::sz_break_id) << " = '" << break_sz<<"'";

	Data_Base::database_request(&query, query_string.str(), ptr_database);
	if(query.lastError().isValid()==true){
		Error msg;
		msg.set_msg("Hyd_Element_Floodplain::delete_data_in_erg_table(QSqlDatabase *ptr_database, const _sys_system_id id, const int bound_sz, const string break_sz )","Invalid database request", "Check the database", 2, false);
		ostringstream info;
		info << "Table Name      : " << Hyd_Element_Floodplain::erg_table->get_table_name() << endl;
		info << "Table error info: " << query.lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}

}
//Delete the data in the database table for the instationaryresults of the hydraulic calculation for a floodplain specified by the system id and the scenario-ids (static)
void Hyd_Element_Floodplain::delete_data_in_instat_erg_table(QSqlDatabase *ptr_database, const _sys_system_id id, const int bound_sz, const string break_sz) {


	//instationary results

	//the table is set (the name and the column names) and allocated
	try {
		Hyd_Element_Floodplain::set_erg_instat_table(ptr_database);
	}
	catch (Error msg) {
		throw msg;
	}

	//delete the table
	QSqlQuery query2(*ptr_database);
	ostringstream query_string;
	query_string << "DELETE FROM " << Hyd_Element_Floodplain::erg_instat_table->get_table_name();
	query_string << " WHERE ";
	query_string << Hyd_Element_Floodplain::erg_instat_table->get_column_name(label::areastate_id) << " =" << id.area_state;
	query_string << " AND ";
	query_string << Hyd_Element_Floodplain::erg_instat_table->get_column_name(label::measure_id) << " = " << id.measure_nr;
	query_string << " AND ";
	query_string << Hyd_Element_Floodplain::erg_instat_table->get_column_name(hyd_label::sz_bound_id) << " = " << bound_sz;
	query_string << " AND ";
	query_string << Hyd_Element_Floodplain::erg_instat_table->get_column_name(risk_label::sz_break_id) << " = '" << break_sz << "'";

	Data_Base::database_request(&query2, query_string.str(), ptr_database);
	if (query2.lastError().isValid() == true) {
		Error msg;
		msg.set_msg("Hyd_Element_Floodplain::delete_data_in_instat_erg_table(QSqlDatabase *ptr_database, const _sys_system_id id, const int bound_sz, const string break_sz )", "Invalid database request", "Check the database", 2, false);
		ostringstream info;
		info << "Table Name      : " << Hyd_Element_Floodplain::erg_instat_table->get_table_name() << endl;
		info << "Table error info: " << query2.lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
}
//Switch the applied-flag for the 2-d results in the database table for a defined system state (static)
void Hyd_Element_Floodplain::switch_applied_flag_erg_table(QSqlDatabase *ptr_database, const _sys_system_id id, const bool flag){
	//the table is set (the name and the column names) and allocated
	try{
		Hyd_Element_Floodplain::set_erg_table(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}
	QSqlQueryModel query;

	ostringstream query_string;
	query_string <<"UPDATE ";
	query_string << Hyd_Element_Floodplain::erg_table->get_table_name();
	query_string << " SET " ;
	query_string  << Hyd_Element_Floodplain::erg_table->get_column_name(label::applied_flag) << " = "<< functions::convert_boolean2string(flag);
	query_string  << " WHERE ";
	query_string  << Hyd_Element_Floodplain::erg_table->get_column_name(label::areastate_id) << " = "  << id.area_state;
	query_string  << " AND ";
	query_string  <<  Hyd_Element_Floodplain::erg_table->get_column_name(label::measure_id) << " = " << id.measure_nr ;
		query_string  << " AND ";
	query_string  << Hyd_Element_Floodplain::erg_table->get_column_name(label::applied_flag) << " = "<< functions::convert_boolean2string(!flag);

	Data_Base::database_request(&query, query_string.str(), ptr_database);
	if(query.lastError().isValid()==true){
		Error msg;
		msg.set_msg("Hyd_Element_Floodplain::switch_applied_flag_erg_table(QSqlDatabase *ptr_database, const _sys_system_id id, const bool flag)","Invalid database request", "Check the database", 2, false);
		ostringstream info;
		info << "Table Name      : " << Hyd_Element_Floodplain::erg_table->get_table_name() << endl;
		info << "Table error info: " << query.lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}


	//instationary results
	//the table is set (the name and the column names) and allocated
	try {
		Hyd_Element_Floodplain::set_erg_instat_table(ptr_database);
	}
	catch (Error msg) {
		throw msg;
	}
	query.clear();

	query_string.str("");
	query_string.clear();
	query_string << "UPDATE ";
	query_string << Hyd_Element_Floodplain::erg_instat_table->get_table_name();
	query_string << " SET ";
	query_string << Hyd_Element_Floodplain::erg_instat_table->get_column_name(label::applied_flag) << " = " << functions::convert_boolean2string(flag);
	query_string << " WHERE ";
	query_string << Hyd_Element_Floodplain::erg_instat_table->get_column_name(label::areastate_id) << " = " << id.area_state;
	query_string << " AND ";
	query_string << Hyd_Element_Floodplain::erg_instat_table->get_column_name(label::measure_id) << " = " << id.measure_nr;
	query_string << " AND ";
	query_string << Hyd_Element_Floodplain::erg_instat_table->get_column_name(label::applied_flag) << " = " << functions::convert_boolean2string(!flag);

	Data_Base::database_request(&query, query_string.str(), ptr_database);
	if (query.lastError().isValid() == true) {
		Error msg;
		msg.set_msg("Hyd_Element_Floodplain::switch_applied_flag_erg_table(QSqlDatabase *ptr_database, const _sys_system_id id, const bool flag)", "Invalid database request", "Check the database", 2, false);
		ostringstream info;
		info << "Table Name      : " << Hyd_Element_Floodplain::erg_instat_table->get_table_name() << endl;
		info << "Table error info: " << query.lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
}
//Switch the applied-flag for the 2-d results in the database table for a defined system state (static)
void Hyd_Element_Floodplain::switch_applied_flag_erg_table(QSqlDatabase *ptr_database, const _sys_system_id id, const int hyd_sc, const bool flag){
	//the table is set (the name and the column names) and allocated
	try{
		Hyd_Element_Floodplain::set_erg_table(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}
	QSqlQueryModel query;

	ostringstream query_string;
	query_string <<"UPDATE ";
	query_string << Hyd_Element_Floodplain::erg_table->get_table_name();
	query_string << " SET " ;
	query_string  << Hyd_Element_Floodplain::erg_table->get_column_name(label::applied_flag) << " = "<< functions::convert_boolean2string(flag);
	query_string  << " WHERE ";
	query_string  << Hyd_Element_Floodplain::erg_table->get_column_name(label::areastate_id) << " = "  << id.area_state;
	query_string  << " AND ";
	query_string  <<  Hyd_Element_Floodplain::erg_table->get_column_name(label::measure_id) << " = " << id.measure_nr ;
	query_string  << " AND ";
	query_string  <<  Hyd_Element_Floodplain::erg_table->get_column_name(hyd_label::sz_bound_id) << " = " << hyd_sc ;
	query_string  << " AND ";
	query_string  << Hyd_Element_Floodplain::erg_table->get_column_name(label::applied_flag) << " = "<< functions::convert_boolean2string(!flag);

	Data_Base::database_request(&query, query_string.str(), ptr_database);
	if(query.lastError().isValid()==true){
		Error msg;
		msg.set_msg("Hyd_Element_Floodplain::switch_applied_flag_erg_table(QSqlDatabase *ptr_database, const _sys_system_id id, const int hyd_sc, const bool flag)","Invalid database request", "Check the database", 2, false);
		ostringstream info;
		info << "Table Name      : " << Hyd_Element_Floodplain::erg_table->get_table_name() << endl;
		info << "Table error info: " << query.lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}


	//instationary results
	//the table is set (the name and the column names) and allocated
	try {
		Hyd_Element_Floodplain::set_erg_instat_table(ptr_database);
	}
	catch (Error msg) {
		throw msg;
	}
	query.clear();

	query_string.str("");
	query_string.clear();
	query_string << "UPDATE ";
	query_string << Hyd_Element_Floodplain::erg_instat_table->get_table_name();
	query_string << " SET ";
	query_string << Hyd_Element_Floodplain::erg_instat_table->get_column_name(label::applied_flag) << " = " << functions::convert_boolean2string(flag);
	query_string << " WHERE ";
	query_string << Hyd_Element_Floodplain::erg_instat_table->get_column_name(label::areastate_id) << " = " << id.area_state;
	query_string << " AND ";
	query_string << Hyd_Element_Floodplain::erg_instat_table->get_column_name(label::measure_id) << " = " << id.measure_nr;
	query_string << " AND ";
	query_string << Hyd_Element_Floodplain::erg_instat_table->get_column_name(hyd_label::sz_bound_id) << " = " << hyd_sc;
	query_string << " AND ";
	query_string << Hyd_Element_Floodplain::erg_instat_table->get_column_name(label::applied_flag) << " = " << functions::convert_boolean2string(!flag);

	Data_Base::database_request(&query, query_string.str(), ptr_database);
	if (query.lastError().isValid() == true) {
		Error msg;
		msg.set_msg("Hyd_Element_Floodplain::switch_applied_flag_erg_table(QSqlDatabase *ptr_database, const _sys_system_id id, const int hyd_sc, const bool flag)", "Invalid database request", "Check the database", 2, false);
		ostringstream info;
		info << "Table Name      : " << Hyd_Element_Floodplain::erg_instat_table->get_table_name() << endl;
		info << "Table error info: " << query.lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
}
//Switch the applied-flag for the boundary condition in the database table for a defined system state (static)
void Hyd_Element_Floodplain::switch_applied_flag_boundary_table(QSqlDatabase *ptr_database, const _sys_system_id id, const bool flag){
	//the table is set (the name and the column names) and allocated
	try{
		Hyd_Element_Floodplain::set_element_boundary_table(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}
	QSqlQueryModel query;

	ostringstream query_string;
	query_string <<"UPDATE ";
	query_string << Hyd_Element_Floodplain::boundary_table->get_table_name();
	query_string << " SET " ;
	query_string  << Hyd_Element_Floodplain::boundary_table->get_column_name(label::applied_flag) << " = "<< functions::convert_boolean2string(flag);
	query_string  << " WHERE ";
	query_string  << Hyd_Element_Floodplain::boundary_table->get_column_name(label::areastate_id) << " = "  << id.area_state;
	query_string  << " AND ";
	query_string  <<  Hyd_Element_Floodplain::boundary_table->get_column_name(label::measure_id) << " = " << id.measure_nr ;

	Data_Base::database_request(&query, query_string.str(), ptr_database);
	if(query.lastError().isValid()==true){
		Error msg;
		msg.set_msg("Hyd_Element_Floodplain::switch_applied_flag_boundary_table(QSqlDatabase *ptr_database, const _sys_system_id id, const bool flag)","Invalid database request", "Check the database", 2, false);
		ostringstream info;
		info << "Table Name      : " << Hyd_Element_Floodplain::boundary_table->get_table_name() << endl;
		info << "Table error info: " << query.lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}


}
//Switch the applied-flag for the boundary condition in the database table for a defined system state (static)
void Hyd_Element_Floodplain::switch_applied_flag_boundary_table(QSqlDatabase *ptr_database, const _sys_system_id id, const int hyd_sc, const bool flag){
	//the table is set (the name and the column names) and allocated
	try{
		Hyd_Element_Floodplain::set_element_boundary_table(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}
	QSqlQueryModel query;

	ostringstream query_string;
	query_string <<"UPDATE ";
	query_string << Hyd_Element_Floodplain::boundary_table->get_table_name();
	query_string << " SET " ;
	query_string  << Hyd_Element_Floodplain::boundary_table->get_column_name(label::applied_flag) << " = "<< functions::convert_boolean2string(flag);
	query_string  << " WHERE ";
	query_string  << Hyd_Element_Floodplain::boundary_table->get_column_name(label::areastate_id) << " = "  << id.area_state;
	query_string  << " AND ";
	query_string  <<  Hyd_Element_Floodplain::boundary_table->get_column_name(label::measure_id) << " = " << id.measure_nr ;
	query_string  << " AND ";
	query_string  << Hyd_Element_Floodplain::boundary_table->get_column_name(hyd_label::sz_bound_id) << " = " << hyd_sc ;

	Data_Base::database_request(&query, query_string.str(), ptr_database);
	if(query.lastError().isValid()==true){
		Error msg;
		msg.set_msg("Hyd_Element_Floodplain::switch_applied_flag_boundary_table(QSqlDatabase *ptr_database, const _sys_system_id id, const int hyd_sc, const bool flag)","Invalid database request", "Check the database", 2, false);
		ostringstream info;
		info << "Table Name      : " << Hyd_Element_Floodplain::boundary_table->get_table_name() << endl;
		info << "Table error info: " << query.lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
}
//Copy the results of a given system id to another one in database table (static)
void Hyd_Element_Floodplain::copy_results(QSqlDatabase *ptr_database, const _sys_system_id src, const _sys_system_id dest){
	//the table is set (the name and the column names) and allocated
	try {
		Hyd_Element_Floodplain::set_erg_table(ptr_database);
	}
	catch (Error msg) {
		throw msg;
	}

	int glob_id = 0;
	glob_id = Hyd_Element_Floodplain::erg_table->maximum_int_of_column(Hyd_Element_Floodplain::erg_table->get_column_name(hyd_label::elemdata_glob_id), ptr_database) + 1;

	QSqlQueryModel model;
	ostringstream test_filter;
	test_filter << "SELECT ";
	test_filter << Hyd_Element_Floodplain::erg_table->get_column_name(hyd_label::elemdata_glob_id);
	test_filter << " from " << Hyd_Element_Floodplain::erg_table->get_table_name();
	test_filter << " WHERE ";
	test_filter << Hyd_Element_Floodplain::erg_table->get_column_name(label::areastate_id) << " = " << src.area_state;
	test_filter << " AND ";
	test_filter << Hyd_Element_Floodplain::erg_table->get_column_name(label::measure_id) << " = " << src.measure_nr;

	//submit it to the datbase
	Data_Base::database_request(&model, test_filter.str(), ptr_database);
	if (model.lastError().isValid() == true) {
		Error msg;
		msg.set_msg("Hyd_Element_Floodplain::copy_results(QSqlDatabase *ptr_database, const _sys_system_id src, const _sys_system_id dest)", "Invalid database request", "Check the database", 2, false);
		ostringstream info;
		info << "Table Name      : " << Hyd_Element_Floodplain::erg_table->get_table_name() << endl;
		info << "Table error info: " << model.lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
	test_filter.str("");
	ostringstream cout;
	cout << "Copy " << model.rowCount() << " results of the floodplain element(s) to the new measure state..." << endl;
	Sys_Common_Output::output_hyd->output_txt(&cout);

	QSqlQueryModel model1;
	for (int i = 0; i < model.rowCount(); i++) {
		if (i % 10000 == 0 && i > 0) {
			cout << "Copy element's results " << i << " to " << i + 10000 << "..." << endl;
			Sys_Common_Output::output_hyd->output_txt(&cout);
		}
		test_filter.str("");
		test_filter << "INSERT INTO " << Hyd_Element_Floodplain::erg_table->get_table_name();
		test_filter << " SELECT " << glob_id << " , ";
		test_filter << Hyd_Element_Floodplain::erg_table->get_column_name(hyd_label::elemdata_fpno) << " , ";
		test_filter << Hyd_Element_Floodplain::erg_table->get_column_name(hyd_label::elemdata_id) << " , ";
		test_filter << dest.area_state << " , ";
		test_filter << dest.measure_nr << " , ";
		test_filter << Hyd_Element_Floodplain::erg_table->get_column_name(label::applied_flag) << " , ";
		test_filter << Hyd_Element_Floodplain::erg_table->get_column_name(hyd_label::sz_bound_id) << " , ";
		test_filter << Hyd_Element_Floodplain::erg_table->get_column_name(risk_label::sz_break_id) << " , ";
		test_filter << Hyd_Element_Floodplain::erg_table->get_column_name(hyd_label::elemerg_h_max) << " , ";
		test_filter << Hyd_Element_Floodplain::erg_table->get_column_name(hyd_label::elemerg_s_max) << " , ";
		test_filter << Hyd_Element_Floodplain::erg_table->get_column_name(hyd_label::elemerg_dsdt_max) << " , ";
		test_filter << Hyd_Element_Floodplain::erg_table->get_column_name(hyd_label::elemerg_vx_max) << " , ";
		test_filter << Hyd_Element_Floodplain::erg_table->get_column_name(hyd_label::elemerg_vy_max) << " , ";
		test_filter << Hyd_Element_Floodplain::erg_table->get_column_name(hyd_label::elemerg_vtot_max) << " , ";
		test_filter << Hyd_Element_Floodplain::erg_table->get_column_name(hyd_label::elemerg_hv_max) << " , ";
		test_filter << Hyd_Element_Floodplain::erg_table->get_column_name(hyd_label::elemerg_t_first) << " , ";
		test_filter << Hyd_Element_Floodplain::erg_table->get_column_name(hyd_label::elemerg_dur_wet) << " , ";
		test_filter << Hyd_Element_Floodplain::erg_table->get_column_name(hyd_label::elemerg_cv_bound_in) << " , ";
		test_filter << Hyd_Element_Floodplain::erg_table->get_column_name(hyd_label::elemerg_cv_bound_out) << " , ";
		test_filter << Hyd_Element_Floodplain::erg_table->get_column_name(hyd_label::elemerg_cv_struc_in) << " , ";
		test_filter << Hyd_Element_Floodplain::erg_table->get_column_name(hyd_label::elemerg_cv_struc_out) << " , ";
		test_filter << Hyd_Element_Floodplain::erg_table->get_column_name(hyd_label::elemerg_cv_dir_in) << " , ";
		test_filter << Hyd_Element_Floodplain::erg_table->get_column_name(hyd_label::elemerg_cv_dir_out) << " , ";
		test_filter << Hyd_Element_Floodplain::erg_table->get_column_name(hyd_label::elemerg_cv_rv_ov_in) << " , ";
		test_filter << Hyd_Element_Floodplain::erg_table->get_column_name(hyd_label::elemerg_cv_rv_ov_out) << " , ";
		test_filter << Hyd_Element_Floodplain::erg_table->get_column_name(hyd_label::elemerg_cv_rv_db_in) << " , ";
		test_filter << Hyd_Element_Floodplain::erg_table->get_column_name(hyd_label::elemerg_cv_rv_db_out) << " , ";
		test_filter << Hyd_Element_Floodplain::erg_table->get_column_name(hyd_label::elemerg_cv_co_ov_in) << " , ";
		test_filter << Hyd_Element_Floodplain::erg_table->get_column_name(hyd_label::elemerg_cv_co_ov_out) << " , ";
		test_filter << Hyd_Element_Floodplain::erg_table->get_column_name(hyd_label::elemerg_cv_co_db_in) << " , ";
		test_filter << Hyd_Element_Floodplain::erg_table->get_column_name(hyd_label::elemerg_cv_co_db_out) << " , ";
		test_filter << Hyd_Element_Floodplain::erg_table->get_column_name(hyd_label::elemerg_cv_fp_in) << " , ";
		test_filter << Hyd_Element_Floodplain::erg_table->get_column_name(hyd_label::elemerg_cv_fp_out) << " , ";
		test_filter << Hyd_Element_Floodplain::erg_table->get_column_name(hyd_label::elemerg_end_vol) << " , ";
		test_filter << Hyd_Element_Floodplain::erg_table->get_column_name(hyd_label::elemdata_polygon) << "  ";

		test_filter << " FROM " << Hyd_Element_Floodplain::erg_table->get_table_name() << " ";
		test_filter << " WHERE " << Hyd_Element_Floodplain::erg_table->get_column_name(hyd_label::elemdata_glob_id) << " = ";
		test_filter << model.record(i).value(Hyd_Element_Floodplain::erg_table->get_column_name(hyd_label::elemdata_glob_id).c_str()).toInt();
		Data_Base::database_request(&model1, test_filter.str(), ptr_database);
		if (model1.lastError().isValid() == true) {
			Error msg;
			msg.set_msg("Hyd_Element_Floodplain::copy_results(QSqlDatabase *ptr_database, const _sys_system_id src, const _sys_system_id dest)", "Invalid database request", "Check the database", 2, false);
			ostringstream info;
			info << "Table Name      : " << Hyd_Element_Floodplain::erg_table->get_table_name() << endl;
			info << "Table error info: " << model1.lastError().text().toStdString() << endl;
			msg.make_second_info(info.str());
			throw msg;
		}
		glob_id++;
	}
	//it is done in the copy of instat results
	//try {
	//	Hyd_Element_Floodplain::switch_applied_flag_erg_table(ptr_database, src, false);
	//}
	//catch (Error msg) {
	//	throw msg;
	//}
}
//Create the database table for the instationary results of an hydraulic simulation for the floodplain elements
void Hyd_Element_Floodplain::create_erg_instat_table(QSqlDatabase *ptr_database) {

	if (Hyd_Element_Floodplain::erg_instat_table == NULL) {
		ostringstream cout;
		cout << "Create floodplain model element instationary result database table..." << endl;
		Sys_Common_Output::output_hyd->output_txt(&cout);
		//make specific input for this class
		const string tab_name = hyd_label::tab_fpelem_erg_instat;
		const int num_col = 18;
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

		tab_col[1].name = hyd_label::elemdata_glob_id;
		tab_col[1].type = sys_label::tab_col_type_int;
		tab_col[1].unsigned_flag = true;


		tab_col[2].name = hyd_label::elemdata_fpno;
		tab_col[2].type = sys_label::tab_col_type_int;
		tab_col[2].unsigned_flag = true;
		tab_col[2].key_flag = true;

		tab_col[3].name = hyd_label::elemdata_id;
		tab_col[3].type = sys_label::tab_col_type_int;
		tab_col[3].unsigned_flag = true;


		tab_col[4].name = label::areastate_id;
		tab_col[4].type = sys_label::tab_col_type_int;
		tab_col[4].unsigned_flag = true;
		tab_col[4].key_flag = true;

		tab_col[5].name = label::measure_id;
		tab_col[5].type = sys_label::tab_col_type_int;
		tab_col[5].unsigned_flag = true;
		tab_col[5].key_flag = true;

		tab_col[6].name = label::applied_flag;
		tab_col[6].type = sys_label::tab_col_type_bool;
		tab_col[6].default_value = "true";
		tab_col[6].key_flag = true;

		tab_col[7].name = hyd_label::sz_bound_id;
		tab_col[7].type = sys_label::tab_col_type_int;
		tab_col[7].unsigned_flag = true;
		tab_col[7].key_flag = true;

		tab_col[8].name = risk_label::sz_break_id;
		tab_col[8].type = sys_label::tab_col_type_string;
		tab_col[8].key_flag = true;

		tab_col[9].name = hyd_label::elemerg_h_max;
		tab_col[9].type = sys_label::tab_col_type_double;
		tab_col[9].unsigned_flag = true;
		tab_col[9].default_value = "0.0";



		tab_col[10].name = hyd_label::elemerg_dsdt_max;
		tab_col[10].type = sys_label::tab_col_type_double;
		tab_col[10].default_value = "0.0";

		tab_col[11].name = hyd_label::elemerg_vx_max;
		tab_col[11].type = sys_label::tab_col_type_double;
		tab_col[11].default_value = "0.0";

		tab_col[12].name = hyd_label::elemerg_vy_max;
		tab_col[12].type = sys_label::tab_col_type_double;
		tab_col[12].default_value = "0.0";

		tab_col[13].name = hyd_label::elemerg_vtot_max;
		tab_col[13].type = sys_label::tab_col_type_double;
		tab_col[13].default_value = "0.0";

		tab_col[14].name = hyd_label::elemerg_hv_max;
		tab_col[14].type = sys_label::tab_col_type_double;
		tab_col[14].default_value = "0.0";

		tab_col[15].name = hyd_label::elemerg_s_max;
		tab_col[15].type = sys_label::tab_col_type_double;
		tab_col[15].default_value = "0.0";

		tab_col[16].name = hyd_label::data_time;
		tab_col[16].type = sys_label::tab_col_type_string;
		tab_col[16].default_value = "";

		tab_col[17].name = hyd_label::elemdata_polygon;
		tab_col[17].type = sys_label::tab_col_type_polygon;


		try {
			Hyd_Element_Floodplain::erg_instat_table = new Tables();
			if (Hyd_Element_Floodplain::erg_instat_table->create_non_existing_tables(tab_name, tab_col, num_col, ptr_database, _sys_table_type::hyd) == false) {
				cout << " Table exists" << endl;
				Sys_Common_Output::output_hyd->output_txt(&cout);
			};
		}
		catch (bad_alloc& t) {
			Error msg;
			msg.set_msg("Hyd_Element_Floodplain::create_erg_instat_table(QSqlDatabase *ptr_database)", "Can not allocate the memory", "Check the memory", 10, false);
			ostringstream info;
			info << "Info bad alloc: " << t.what() << endl;
			msg.make_second_info(info.str());
			throw msg;
		}
		catch (Error msg) {
			Hyd_Element_Floodplain::close_erg_instat_table();
			throw msg;
		}

		Hyd_Element_Floodplain::close_erg_instat_table();


		//make indizes
		//the table is set (the name and the column names) and allocated
		try {
			Hyd_Element_Floodplain::set_erg_instat_table(ptr_database);
		}
		catch (Error msg) {
			throw msg;
		}


		Hyd_Element_Floodplain::erg_instat_table->create_index2column(ptr_database, Hyd_Element_Floodplain::erg_instat_table->get_column_name(hyd_label::data_time));
		Hyd_Element_Floodplain::erg_instat_table->create_spatial_index2column(ptr_database, Hyd_Element_Floodplain::erg_instat_table->get_column_name(hyd_label::elemdata_polygon));


		Hyd_Element_Floodplain::close_erg_instat_table();	
	}


}
//Create the database view for the instationary results of an hydraulic simulation for the floodplain elements
void Hyd_Element_Floodplain::create_erg_instat_view1(QSqlDatabase *ptr_database) {

	//Set tables
	ostringstream cout;
	//the table is set (the name and the column names) and allocated
	try {
		Hyd_Element_Floodplain::set_erg_instat_table(ptr_database);
		Hyd_Element_Floodplain::set_table(ptr_database);
	}
	catch (Error msg) {
		throw msg;
	}


	cout << "Create floodplain model element instationary view to database table..." << endl;
	Sys_Common_Output::output_hyd->output_txt(&cout);
	//make specific input for this class
	const string view_name = hyd_label::view_fpelem_erg_instat;

	QSqlQueryModel query;

	ostringstream query_string;
	query_string << "CREATE VIEW ";
	query_string << Sys_Project::get_complete_project_database_schemata_name() << "." << hyd_label::view_fpelem_erg_instat;
	query_string << " AS SELECT ";
	query_string << Hyd_Element_Floodplain::erg_instat_table->get_column_name_table(label::glob_id) << " , ";
	query_string << Hyd_Element_Floodplain::elem_table->get_column_name_table(hyd_label::elemdata_glob_id) << " , ";
	query_string << Hyd_Element_Floodplain::elem_table->get_column_name_table(hyd_label::elemdata_fpno) << " , ";
	query_string << Hyd_Element_Floodplain::erg_instat_table->get_column_name_table(label::areastate_id) << " , ";
	query_string << Hyd_Element_Floodplain::erg_instat_table->get_column_name_table(label::measure_id) << " , ";
	query_string << Hyd_Element_Floodplain::erg_instat_table->get_column_name_table(hyd_label::sz_bound_id) << " , ";
	query_string << Hyd_Element_Floodplain::erg_instat_table->get_column_name_table(risk_label::sz_break_id) << " , ";
	query_string << Hyd_Element_Floodplain::erg_instat_table->get_column_name_table(label::applied_flag) << " , ";

	query_string << Hyd_Element_Floodplain::erg_instat_table->get_column_name_table(hyd_label::elemerg_h_max) << " , ";
	query_string << Hyd_Element_Floodplain::erg_instat_table->get_column_name_table(hyd_label::elemerg_s_max) << " , ";
	query_string << Hyd_Element_Floodplain::erg_instat_table->get_column_name_table(hyd_label::elemerg_dsdt_max) << " , ";
	query_string << Hyd_Element_Floodplain::erg_instat_table->get_column_name_table(hyd_label::elemerg_vx_max) << " , ";
	query_string << Hyd_Element_Floodplain::erg_instat_table->get_column_name_table(hyd_label::elemerg_vy_max) << " , ";

	query_string << Hyd_Element_Floodplain::erg_instat_table->get_column_name_table(hyd_label::elemerg_vtot_max) << " , ";
	query_string << Hyd_Element_Floodplain::erg_instat_table->get_column_name_table(hyd_label::elemerg_hv_max) << " , ";
	query_string << Hyd_Element_Floodplain::erg_instat_table->get_column_name_table(hyd_label::data_time) << " , ";
	query_string << Hyd_Element_Floodplain::elem_table->get_column_name_table(hyd_label::elemdata_polygon) << "  ";
	query_string << " FROM ";
	query_string << Hyd_Element_Floodplain::elem_table->get_table_name() << "  ";
	query_string << " JOIN ";
	query_string << Hyd_Element_Floodplain::erg_instat_table->get_table_name() << "  ";
	query_string << " ON ";
	query_string << Hyd_Element_Floodplain::elem_table->get_column_name_table(hyd_label::elemdata_glob_id) << "  ";
	query_string << " = ";
	query_string << Hyd_Element_Floodplain::erg_instat_table->get_column_name_table(hyd_label::elemdata_glob_id) << "  ";

	cout << query_string.str() << endl;
	Sys_Common_Output::output_hyd->output_txt(&cout);

	Data_Base::database_request(&query, query_string.str(), ptr_database);
	if (query.lastError().isValid() == true) {
		Error msg;
		msg.set_msg("Hyd_Element_Floodplain::create_erg_instat_view(QSqlDatabase *ptr_database)", "Invalid database request", "Check the database", 2, false);
		ostringstream info;
		info << "View Name      : " << hyd_label::view_fpelem_erg_instat << endl;
		info << "View error info: " << query.lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}

}
///Set the database table for the results of an hydraulic simulation for the floodplain elements: it sets the table name and the name of the columns and allocate them
void Hyd_Element_Floodplain::set_erg_instat_table(QSqlDatabase *ptr_database, const bool not_close) {

	if (Hyd_Element_Floodplain::erg_instat_table == NULL) {
		//make specific input for this class
		const string tab_id_name = hyd_label::tab_fpelem_erg_instat;
		string tab_id_col[18];
		tab_id_col[0] = label::areastate_id;
		tab_id_col[1] = label::measure_id;
		tab_id_col[2] = hyd_label::sz_bound_id;
		tab_id_col[3] = risk_label::sz_break_id;
		tab_id_col[4] = hyd_label::elemerg_h_max;
		tab_id_col[5] = hyd_label::elemerg_dsdt_max;
		tab_id_col[6] = hyd_label::elemerg_vx_max;
		tab_id_col[7] = hyd_label::elemerg_vy_max;
		tab_id_col[8] = hyd_label::elemerg_vtot_max;
		tab_id_col[9] = hyd_label::elemdata_glob_id;
		tab_id_col[10] = hyd_label::elemerg_hv_max;
		tab_id_col[11] = label::applied_flag;
		tab_id_col[12] = hyd_label::elemerg_s_max;
		tab_id_col[13] = label::glob_id;
		tab_id_col[14] = hyd_label::data_time;
		tab_id_col[15] = hyd_label::elemdata_polygon;
		tab_id_col[16] = hyd_label::elemdata_fpno;
		tab_id_col[17] = hyd_label::elemdata_id;

		try {
			Hyd_Element_Floodplain::erg_instat_table = new Tables(tab_id_name, tab_id_col, sizeof(tab_id_col) / sizeof(tab_id_col[0]));
			Hyd_Element_Floodplain::erg_instat_table->set_name(ptr_database, _sys_table_type::hyd);
		}
		catch (bad_alloc& t) {
			Error msg;
			msg.set_msg("Hyd_Element_Floodplain::set_erg_instat_table(QSqlDatabase *ptr_database)", "Can not allocate the memory", "Check the memory", 10, false);
			ostringstream info;
			info << "Info bad alloc: " << t.what() << endl;
			msg.make_second_info(info.str());
			throw msg;
		}
		catch (Error msg) {
			if (not_close == false) {
				Hyd_Element_Floodplain::close_erg_instat_table();
			}
			throw msg;
		}
	}

}
//Close and delete the database table for the instationary results of an hydraulic simulation for the floodplain elements
void Hyd_Element_Floodplain::close_erg_instat_table(void) {
	if (Hyd_Element_Floodplain::erg_instat_table != NULL) {
		delete Hyd_Element_Floodplain::erg_instat_table;
		Hyd_Element_Floodplain::erg_instat_table = NULL;
	}

}
///Get the maximum value of the global index of the element instationary result database table
int Hyd_Element_Floodplain::get_max_glob_id_erg_instat_table(QSqlDatabase *ptr_database){
	//the table is set (the name and the column names) and allocated
	try {
		Hyd_Element_Floodplain::set_erg_instat_table(ptr_database);
	}
	catch (Error msg) {
		throw msg;
	}

	//evaluate the global identifier
	int id_glob = Hyd_Element_Floodplain::erg_instat_table->maximum_int_of_column(Hyd_Element_Floodplain::erg_instat_table->get_column_name(label::glob_id), ptr_database);
	return id_glob;

}
//Select and count the number of relevant instationary results per floodplain elements for one floodplain model in a database table (static)
int Hyd_Element_Floodplain::select_relevant_instat_results_elements_database(QSqlQueryModel *results, QSqlDatabase *ptr_database, const _sys_system_id id, const int fp_number, const int bound_sz, const string break_sz, const bool with_output) {
	int number = 0;
	try {
		Hyd_Element_Floodplain::set_erg_instat_table(ptr_database);
	}
	catch (Error msg) {
		throw msg;
	}
	if (with_output == true) {
		ostringstream cout;
		cout << "Select relevant instationary results of the floodplain elements in database..." << endl;
		Sys_Common_Output::output_hyd->output_txt(&cout);
	}


	ostringstream test_filter;
	test_filter << "SELECT ";
	test_filter << Hyd_Element_Floodplain::erg_instat_table->get_column_name(hyd_label::elemdata_id) << " , ";
	test_filter << Hyd_Element_Floodplain::erg_instat_table->get_column_name(hyd_label::elemerg_h_max) << " , ";
	test_filter << Hyd_Element_Floodplain::erg_instat_table->get_column_name(hyd_label::elemerg_vtot_max) << " , ";
	test_filter << Hyd_Element_Floodplain::erg_instat_table->get_column_name(hyd_label::elemerg_dsdt_max) << " , ";
	test_filter << Hyd_Element_Floodplain::erg_instat_table->get_column_name(hyd_label::elemerg_hv_max) << " , ";
	test_filter << Hyd_Element_Floodplain::erg_instat_table->get_column_name(hyd_label::data_time);
	test_filter << " FROM " << Hyd_Element_Floodplain::erg_instat_table->get_table_name();
	test_filter << " WHERE ";
	test_filter << Hyd_Element_Floodplain::erg_instat_table->get_column_name(risk_label::sz_break_id) << " = '" << break_sz << "'";
	test_filter << " AND ";
	test_filter << Hyd_Element_Floodplain::erg_instat_table->get_column_name(hyd_label::sz_bound_id) << " = " << bound_sz;
	test_filter << " AND ";
	test_filter << Hyd_Element_Floodplain::erg_instat_table->get_column_name(label::applied_flag) << "= true";
	test_filter << " AND ";
	test_filter << Hyd_Element_Floodplain::erg_instat_table->get_column_name(label::areastate_id) << " =" << id.area_state;
	test_filter << " AND (";
	test_filter << Hyd_Element_Floodplain::erg_instat_table->get_column_name(label::measure_id) << " = " << 0;
	test_filter << " OR ";
	test_filter << Hyd_Element_Floodplain::erg_instat_table->get_column_name(label::measure_id) << " = " << id.measure_nr;
	test_filter << " ) ";
	test_filter << " AND ";
	test_filter << Hyd_Element_Floodplain::erg_instat_table->get_column_name(hyd_label::elemdata_fpno) << " = " << fp_number;
	test_filter << " ORDER BY " << Hyd_Element_Floodplain::erg_instat_table->get_column_name(label::glob_id);

	Data_Base::database_request(results, test_filter.str(), ptr_database);

	//check the request
	if (results->lastError().isValid()) {
		Error msg;
		msg.set_msg("Hyd_Element_Floodplain::select_relevant_instat_results_elements_database(QSqlTableModel *results, const _sys_system_id id, const int fp_number, , const int bound_sz, const string break_sz, const bool with_output)", "Invalid database request", "Check the database", 2, false);
		ostringstream info;
		info << "Table Name      : " << Hyd_Element_Floodplain::erg_instat_table->get_table_name() << endl;
		info << "Table error info: " << results->lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}

	number = results->rowCount();

	if (with_output == true) {
		ostringstream cout;
		cout << number << " relevant instationary results of the floodplain element(s) are selected in database" << endl;
		Sys_Common_Output::output_hyd->output_txt(&cout);
	}

	return number;

}
//Select and count the number of relevant instationary results per floodplain elements for one floodplain model in a database table (just part of it) (static)
int Hyd_Element_Floodplain::select_relevant_instat_results_elements_database(QSqlQueryModel *results, QSqlDatabase *ptr_database, const _sys_system_id id, const int fp_number, const int bound_sz, const string break_sz, const int offset, const int number_rows, const string time_date, const bool with_output) {
	int number = 0;
	try {
		Hyd_Element_Floodplain::set_erg_instat_table(ptr_database);
	}
	catch (Error msg) {
		throw msg;
	}
	if (with_output == true) {
		ostringstream cout;
		cout << "Select relevant instationary results of the floodplain elements in database..." << endl;
		Sys_Common_Output::output_hyd->output_txt(&cout);
	}


	ostringstream test_filter;
	test_filter << "SELECT ";
	test_filter << Hyd_Element_Floodplain::erg_instat_table->get_column_name(hyd_label::elemdata_id) << " , ";
	test_filter << Hyd_Element_Floodplain::erg_instat_table->get_column_name(hyd_label::elemerg_h_max) << " , ";
	test_filter << Hyd_Element_Floodplain::erg_instat_table->get_column_name(hyd_label::elemerg_vtot_max) << " , ";
	test_filter << Hyd_Element_Floodplain::erg_instat_table->get_column_name(hyd_label::elemerg_dsdt_max) << " , ";
	test_filter << Hyd_Element_Floodplain::erg_instat_table->get_column_name(hyd_label::elemerg_hv_max) << " , ";
	test_filter << Hyd_Element_Floodplain::erg_instat_table->get_column_name(hyd_label::data_time);
	test_filter << " FROM " << Hyd_Element_Floodplain::erg_instat_table->get_table_name();
	test_filter << " WHERE ";
	test_filter << Hyd_Element_Floodplain::erg_instat_table->get_column_name(risk_label::sz_break_id) << " = '" << break_sz << "'";
	test_filter << " AND ";
	test_filter << Hyd_Element_Floodplain::erg_instat_table->get_column_name(hyd_label::sz_bound_id) << " = " << bound_sz;
	test_filter << " AND ";
	test_filter << Hyd_Element_Floodplain::erg_instat_table->get_column_name(label::applied_flag) << "= true";
	test_filter << " AND ";
	test_filter << Hyd_Element_Floodplain::erg_instat_table->get_column_name(label::areastate_id) << " =" << id.area_state;
	test_filter << " AND (";
	test_filter << Hyd_Element_Floodplain::erg_instat_table->get_column_name(label::measure_id) << " = " << 0;
	test_filter << " OR ";
	test_filter << Hyd_Element_Floodplain::erg_instat_table->get_column_name(label::measure_id) << " = " << id.measure_nr;
	test_filter << " ) ";
	test_filter << " AND ";
	test_filter << Hyd_Element_Floodplain::erg_instat_table->get_column_name(hyd_label::elemdata_fpno) << " = " << fp_number;
	test_filter << " AND ";
	test_filter << Hyd_Element_Floodplain::erg_instat_table->get_column_name(hyd_label::data_time) << " = '" << time_date<<"'";
	test_filter << " ORDER BY " << Hyd_Element_Floodplain::erg_instat_table->get_column_name(label::glob_id);
	test_filter << " LIMIT " << number_rows;
	test_filter << " OFFSET " << offset;

	Data_Base::database_request(results, test_filter.str(), ptr_database);

	//check the request
	if (results->lastError().isValid()) {
		Error msg;
		msg.set_msg("Hyd_Element_Floodplain::select_relevant_instat_results_elements_database(QSqlQueryModel *results, QSqlDatabase *ptr_database, const _sys_system_id id, const int fp_number, const int bound_sz, const string break_sz, const int offset, const int number_rows, const bool with_output)", "Invalid database request", "Check the database", 2, false);
		ostringstream info;
		info << "Table Name      : " << Hyd_Element_Floodplain::erg_instat_table->get_table_name() << endl;
		info << "Table error info: " << results->lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}

	number = results->rowCount();

	if (with_output == true) {
		ostringstream cout;
		cout << number << " relevant instationary results of the floodplain element(s) are selected in database" << endl;
		Sys_Common_Output::output_hyd->output_txt(&cout);
	}

	return number;

}
//Count the number of relevant instationary results per floodplain elements for one floodplain model in a database table (static)
int Hyd_Element_Floodplain::count_relevant_instat_results_elements_database(QSqlQueryModel *results, QSqlDatabase *ptr_database, const _sys_system_id id, const int fp_number, const int bound_sz, const string break_sz, const string time_date, const bool with_output) {
	int number = 0;
	try {
		Hyd_Element_Floodplain::set_erg_instat_table(ptr_database);
	}
	catch (Error msg) {
		throw msg;
	}
	if (with_output == true) {
		ostringstream cout;
		cout << "Search for relevant instationary results of the floodplain elements in database..." << endl;
		Sys_Common_Output::output_hyd->output_txt(&cout);
	}

	ostringstream test_filter;
	test_filter << "SELECT COUNT(";
	test_filter << Hyd_Element_Floodplain::erg_instat_table->get_column_name(hyd_label::elemdata_id) << " ) ";
	test_filter << " FROM " << Hyd_Element_Floodplain::erg_instat_table->get_table_name();
	test_filter << " WHERE ";
	test_filter << Hyd_Element_Floodplain::erg_instat_table->get_column_name(risk_label::sz_break_id) << " = '" << break_sz << "'";
	test_filter << " AND ";
	test_filter << Hyd_Element_Floodplain::erg_instat_table->get_column_name(hyd_label::sz_bound_id) << " = " << bound_sz;
	test_filter << " AND ";
	test_filter << Hyd_Element_Floodplain::erg_instat_table->get_column_name(label::applied_flag) << "= true";
	test_filter << " AND ";
	test_filter << Hyd_Element_Floodplain::erg_instat_table->get_column_name(label::areastate_id) << " =" << id.area_state;
	test_filter << " AND (";
	test_filter << Hyd_Element_Floodplain::erg_instat_table->get_column_name(label::measure_id) << " = " << 0;
	test_filter << " OR ";
	test_filter << Hyd_Element_Floodplain::erg_instat_table->get_column_name(label::measure_id) << " = " << id.measure_nr;
	test_filter << " ) ";
	test_filter << " AND ";
	test_filter << Hyd_Element_Floodplain::erg_instat_table->get_column_name(hyd_label::elemdata_fpno) << " = " << fp_number;
	test_filter << " AND ";
	test_filter << Hyd_Element_Floodplain::erg_instat_table->get_column_name(hyd_label::data_time) << " = '" << time_date << "'";

	Data_Base::database_request(results, test_filter.str(), ptr_database);

	//check the request
	if (results->lastError().isValid()) {
		Error msg;
		msg.set_msg("Hyd_Element_Floodplain::count_relevant_instat_results_elements_database(QSqlTableModel *results, const _sys_system_id id, const int fp_number, , const int bound_sz, const string break_sz, const bool with_output)", "Invalid database request", "Check the database", 2, false);
		ostringstream info;
		info << "Table Name      : " << Hyd_Element_Floodplain::erg_instat_table->get_table_name() << endl;
		info << "Table error info: " << results->lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}

	if (results->rowCount() > 0) {
		number = results->record(0).field(0).value().toInt();
	}

	if (with_output == true) {
		ostringstream cout;
		cout << number << " relevant instationary results of the floodplain element(s) are found in database" << endl;
		Sys_Common_Output::output_hyd->output_txt(&cout);
	}

	return number;

}
//Get a list of dicstinct date-time strings from database table (static)
int Hyd_Element_Floodplain::get_distinct_date_time_instat_results_elements_database(QStringList *list, QSqlDatabase *ptr_database, const _sys_system_id id, const int bound_sz, const string break_sz, const bool with_output) {

	int number = 0;
	try {
		Hyd_Element_Floodplain::set_erg_instat_table(ptr_database);
	}
	catch (Error msg) {
		throw msg;
	}
	if (with_output == true) {
		ostringstream cout;
		cout << "Search for relevant date-time string of the instationary results of floodplain elements in database..." << endl;
		Sys_Common_Output::output_hyd->output_txt(&cout);
	}
	QSqlQueryModel results;

	ostringstream test_filter;
	test_filter << "SELECT * FROM (";
	test_filter << "SELECT DISTINCT(";
	test_filter << Hyd_Element_Floodplain::erg_instat_table->get_column_name(hyd_label::data_time) << " ) ";
	test_filter << Hyd_Element_Floodplain::erg_instat_table->get_column_name(label::glob_id);
	test_filter << " FROM " << Hyd_Element_Floodplain::erg_instat_table->get_table_name();
	test_filter << " WHERE ";
	test_filter << Hyd_Element_Floodplain::erg_instat_table->get_column_name(risk_label::sz_break_id) << " = '" << break_sz << "'";
	test_filter << " AND ";
	test_filter << Hyd_Element_Floodplain::erg_instat_table->get_column_name(hyd_label::sz_bound_id) << " = " << bound_sz;
	test_filter << " AND ";
	test_filter << Hyd_Element_Floodplain::erg_instat_table->get_column_name(label::applied_flag) << "= true";
	test_filter << " AND ";
	test_filter << Hyd_Element_Floodplain::erg_instat_table->get_column_name(label::areastate_id) << " =" << id.area_state;
	test_filter << " AND (";
	test_filter << Hyd_Element_Floodplain::erg_instat_table->get_column_name(label::measure_id) << " = " << 0;
	test_filter << " OR ";
	test_filter << Hyd_Element_Floodplain::erg_instat_table->get_column_name(label::measure_id) << " = " << id.measure_nr;
	test_filter << " )) p";
	test_filter << " ORDER BY " << Hyd_Element_Floodplain::erg_instat_table->get_column_name(label::glob_id);

	Data_Base::database_request(&results, test_filter.str(), ptr_database);

	//check the request
	if (results.lastError().isValid()) {
		Error msg;
		msg.set_msg("Hyd_Element_Floodplain::count_relevant_instat_results_elements_database(QSqlTableModel *results, const _sys_system_id id, const int fp_number, , const int bound_sz, const string break_sz, const bool with_output)", "Invalid database request", "Check the database", 2, false);
		ostringstream info;
		info << "Table Name      : " << Hyd_Element_Floodplain::erg_instat_table->get_table_name() << endl;
		info << "Table error info: " << results.lastError().text().toStdString() << endl;
		//info << test_Filter.str()<< endl;
		msg.make_second_info(info.str());
		throw msg;
	}

	
	number = results.rowCount();
	

	if (with_output == true) {
		ostringstream cout;
		cout << number << " relevant date-time string of the instationary results of floodplain elements in database are found in database" << endl;
		Sys_Common_Output::output_hyd->output_txt(&cout);
	}

	for (int i = 0; i < number; i++) {
		//list->append(results.record(i).value((Hyd_Element_Floodplain::erg_instat_table->get_column_name(hyd_label::data_time)).c_str()).toString());
		list->append(results.record(i).value(0).toString());

	}

	return number;







}
//Copy the instationary results of a given system id to another one in database table
void Hyd_Element_Floodplain::copy_instat_results(QSqlDatabase *ptr_database, const _sys_system_id src, const _sys_system_id dest) {
	//the table is set (the name and the column names) and allocated
	try {
		Hyd_Element_Floodplain::set_erg_instat_table(ptr_database);
	}
	catch (Error msg) {
		throw msg;
	}

	int glob_id = 0;
	glob_id = Hyd_Element_Floodplain::erg_instat_table->maximum_int_of_column(Hyd_Element_Floodplain::erg_instat_table->get_column_name(label::glob_id), ptr_database) + 1;

	QSqlQueryModel model;
	ostringstream test_filter;
	test_filter << "SELECT ";
	test_filter << Hyd_Element_Floodplain::erg_instat_table->get_column_name(hyd_label::elemdata_glob_id);
	test_filter << " from " << Hyd_Element_Floodplain::erg_instat_table->get_table_name();
	test_filter << " WHERE ";
	test_filter << Hyd_Element_Floodplain::erg_instat_table->get_column_name(label::areastate_id) << " = " << src.area_state;
	test_filter << " AND ";
	test_filter << Hyd_Element_Floodplain::erg_instat_table->get_column_name(label::measure_id) << " = " << src.measure_nr;

	//submit it to the datbase
	Data_Base::database_request(&model, test_filter.str(), ptr_database);
	if (model.lastError().isValid() == true) {
		Error msg;
		msg.set_msg("Hyd_Element_Floodplain::copy_results(QSqlDatabase *ptr_database, const _sys_system_id src, const _sys_system_id dest)", "Invalid database request", "Check the database", 2, false);
		ostringstream info;
		info << "Table Name      : " << Hyd_Element_Floodplain::erg_instat_table->get_table_name() << endl;
		info << "Table error info: " << model.lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
	test_filter.str("");
	ostringstream cout;
	cout << "Copy " << model.rowCount() << " instationary results of the floodplain element(s) to the new measure state..." << endl;
	Sys_Common_Output::output_hyd->output_txt(&cout);

	QSqlQueryModel model1;
	for (int i = 0; i < model.rowCount(); i++) {
		if (i % 10000 == 0 && i > 0) {
			cout << "Copy element's instationary results " << i << " to " << i + 10000 << "..." << endl;
			Sys_Common_Output::output_hyd->output_txt(&cout);
		}
		test_filter.str("");
		test_filter << "INSERT INTO " << Hyd_Element_Floodplain::erg_instat_table->get_table_name();
		test_filter << " SELECT " << glob_id << " , ";
		test_filter << dest.area_state << " , ";
		test_filter << dest.measure_nr << " , ";
		test_filter << Hyd_Element_Floodplain::erg_instat_table->get_column_name(label::applied_flag) << " , ";
		test_filter << Hyd_Element_Floodplain::erg_instat_table->get_column_name(hyd_label::elemdata_polygon) << " , ";
		test_filter << Hyd_Element_Floodplain::erg_instat_table->get_column_name(hyd_label::elemdata_fpno) << " , ";
		test_filter << Hyd_Element_Floodplain::erg_instat_table->get_column_name(hyd_label::sz_bound_id) << " , ";
		test_filter << Hyd_Element_Floodplain::erg_instat_table->get_column_name(risk_label::sz_break_id) << " , ";
		test_filter << Hyd_Element_Floodplain::erg_instat_table->get_column_name(hyd_label::elemerg_h_max) << " , ";
		test_filter << Hyd_Element_Floodplain::erg_instat_table->get_column_name(hyd_label::elemerg_s_max) << " , ";
		test_filter << Hyd_Element_Floodplain::erg_instat_table->get_column_name(hyd_label::elemerg_dsdt_max) << " , ";
		test_filter << Hyd_Element_Floodplain::erg_instat_table->get_column_name(hyd_label::elemerg_vx_max) << " , ";
		test_filter << Hyd_Element_Floodplain::erg_instat_table->get_column_name(hyd_label::elemerg_vy_max) << " , ";
		test_filter << Hyd_Element_Floodplain::erg_instat_table->get_column_name(hyd_label::elemerg_vtot_max) << " , ";
		test_filter << Hyd_Element_Floodplain::erg_instat_table->get_column_name(hyd_label::elemerg_hv_max) << " , ";
		test_filter << Hyd_Element_Floodplain::erg_instat_table->get_column_name(hyd_label::data_time) << " , ";
		test_filter << Hyd_Element_Floodplain::erg_instat_table->get_column_name(hyd_label::elemdata_polygon) << "  ";


		test_filter << " FROM " << Hyd_Element_Floodplain::erg_instat_table->get_table_name() << " ";
		test_filter << " WHERE " << Hyd_Element_Floodplain::erg_instat_table->get_column_name(hyd_label::elemdata_glob_id) << " = ";
		test_filter << model.record(i).value(Hyd_Element_Floodplain::erg_instat_table->get_column_name(hyd_label::elemdata_glob_id).c_str()).toInt();
		Data_Base::database_request(&model1, test_filter.str(), ptr_database);
		if (model1.lastError().isValid() == true) {
			Error msg;
			msg.set_msg("Hyd_Element_Floodplain::copy_results(QSqlDatabase *ptr_database, const _sys_system_id src, const _sys_system_id dest)", "Invalid database request", "Check the database", 2, false);
			ostringstream info;
			info << "Table Name      : " << Hyd_Element_Floodplain::erg_instat_table->get_table_name() << endl;
			info << "Table error info: " << model1.lastError().text().toStdString() << endl;
			msg.make_second_info(info.str());
			throw msg;
		}
		glob_id++;
	}
	try {
		Hyd_Element_Floodplain::switch_applied_flag_erg_table(ptr_database, src, false);
	}
	catch (Error msg) {
		throw msg;
	}
}
//Clone the members of the floodplain elements
void Hyd_Element_Floodplain::clone_element(Hyd_Element_Floodplain *elements){
	this->glob_elem_number = elements->glob_elem_number;
	this->elem_number=elements->elem_number;

	this->type=elements->type;
	this->z_value=elements->z_value;
	this->polygon_string=elements->polygon_string;
	this->mid_point=elements->mid_point;

	*this->buffer_flow_data=elements->get_flow_data();

	*this->buffer_boundary_data=elements->get_boundary_data();
	if(this->buffer_boundary_data->ptr_boundarycurve!=NULL){
		this->buffer_boundary_data->ptr_boundarycurve=NULL;
		this->buffer_boundary_data->found_flag=false;
	}
}
//Create the database table for the boundary element data (static)
void Hyd_Element_Floodplain::create_element_boundary_table(QSqlDatabase *ptr_database){
	if(Hyd_Element_Floodplain::boundary_table==NULL){
		ostringstream cout;
		cout << "Create floodplain element boundary database table..." << endl ;
		Sys_Common_Output::output_hyd->output_txt(&cout);
		//make specific input for this class
		const string tab_name=hyd_label::tab_fpelem_bound;
		const int num_col=8;
		_Sys_data_tab_column tab_col[num_col];
		//init
		for(int i=0; i< num_col; i++){
			tab_col[i].key_flag=false;
			tab_col[i].unsigned_flag=false;
			tab_col[i].primary_key_flag=false;
		}

		tab_col[0].name=hyd_label::elemdata_glob_id;
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
			Hyd_Element_Floodplain::boundary_table= new Tables();
			if(Hyd_Element_Floodplain::boundary_table->create_non_existing_tables(tab_name, tab_col, num_col,ptr_database, _sys_table_type::hyd)==false){
				cout << " Table exists" << endl ;
				Sys_Common_Output::output_hyd->output_txt(&cout);
			};
		}
		catch(bad_alloc& t){
			Error msg;
			msg.set_msg("Hyd_Element_Floodplain::create_element_boundary_table(QSqlDatabase *ptr_database)","Can not allocate the memory", "Check the memory", 10, false);
			ostringstream info;
			info<< "Info bad alloc: " << t.what() << endl;
			msg.make_second_info(info.str());
			throw msg;
		}
		catch(Error msg){
			Hyd_Element_Floodplain::close_boundary_table();
			throw msg;
		}

	Hyd_Element_Floodplain::close_boundary_table();
	}
}
//Create the database view for the connection of boundary ids and the floodplain elements (static)
void Hyd_Element_Floodplain::create_bound2elems_view(QSqlDatabase *ptr_database) {
	//Set tables
	ostringstream cout;
	//the table is set (the name and the column names) and allocated
	try {
		Hyd_Element_Floodplain::set_element_boundary_table(ptr_database);
		Hyd_Element_Floodplain::set_table(ptr_database);
	}
	catch (Error msg) {
		throw msg;
	}


	cout << "Create database view to floodplain model element with boundary conditions..." << endl;
	Sys_Common_Output::output_hyd->output_txt(&cout);
	//make specific input for this class
	const string view_name = hyd_label::view_fpelem2bound;

	QSqlQueryModel query;

	ostringstream query_string;
	query_string << "CREATE VIEW ";
	query_string << Sys_Project::get_complete_project_database_schemata_name() << "." << hyd_label::view_fpelem2bound;
	query_string << " AS SELECT ";
	query_string << "row_number() over() AS Id" << " , ";
	query_string << Hyd_Element_Floodplain::elem_table->get_column_name_table(hyd_label::elemdata_glob_id) << " , ";
	query_string << Hyd_Element_Floodplain::elem_table->get_column_name_table(hyd_label::elemdata_fpno) << " , ";
	query_string << Hyd_Element_Floodplain::boundary_table->get_column_name_table(label::areastate_id) << " , ";
	query_string << Hyd_Element_Floodplain::boundary_table->get_column_name_table(label::measure_id) << " , ";
	query_string << Hyd_Element_Floodplain::boundary_table->get_column_name_table(hyd_label::sz_bound_id) << " , ";
	query_string << Hyd_Element_Floodplain::boundary_table->get_column_name_table(label::applied_flag) << " , ";

	query_string << Hyd_Element_Floodplain::boundary_table->get_column_name_table(hyd_label::bounddata_stat) << " , ";
	query_string << Hyd_Element_Floodplain::boundary_table->get_column_name_table(hyd_label::bounddata_value) << " , ";
	query_string << Hyd_Element_Floodplain::boundary_table->get_column_name_table(hyd_label::bounddata_type) << " , ";

	query_string << Hyd_Element_Floodplain::elem_table->get_column_name_table(hyd_label::elemdata_polygon) << "  ";
	
	query_string << " FROM ";
	query_string << Hyd_Element_Floodplain::boundary_table->get_table_name() << "  ";
	query_string << " JOIN ";
	query_string << Hyd_Element_Floodplain::elem_table->get_table_name() << "  ";
	query_string << " ON ";
	query_string << Hyd_Element_Floodplain::boundary_table->get_column_name_table(hyd_label::elemdata_glob_id) << "  ";
	query_string << " = ";
	query_string << Hyd_Element_Floodplain::elem_table->get_column_name_table(hyd_label::elemdata_glob_id) << "  ";




	Data_Base::database_request(&query, query_string.str(), ptr_database);
	if (query.lastError().isValid() == true) {
		Error msg;
		msg.set_msg("Hyd_Element_Floodplain::create_bound2elems_view(QSqlDatabase *ptr_database)", "Invalid database request", "Check the database", 2, false);
		ostringstream info;
		info << "View Name      : " << hyd_label::view_fpelem2bound << endl;
		info << "View error info: " << query.lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}


}
//Check if the view exists already (static) 
bool Hyd_Element_Floodplain::check_bound2elems_view_exists(QSqlDatabase *ptr_database) {

	QSqlQueryModel query;

	ostringstream query_string;
	query_string << "SELECT EXISTS ( SELECT * FROM information_schema.tables ";
	query_string << "WHERE table_schema ='" << Sys_Project::get_complete_project_database_schemata_name() << "' ";
	query_string << "AND table_name ='" << functions::convert_string2lower_case(hyd_label::view_fpelem2bound) << "' )";


	Data_Base::database_request(&query, query_string.str(), ptr_database);
	if (query.lastError().isValid() == true) {
		Error msg;
		msg.set_msg("Hyd_Element_Floodplain::check_bound2elems_view_exists(QSqlDatabase *ptr_database)", "Invalid database request", "Check the database", 2, false);
		ostringstream info;
		info << "View Name      : " << hyd_label::view_fpelem2bound << endl;
		info << "View error info: " << query.lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}


	if (query.rowCount() > 0) {
		return true;
	}

	return false;

}
//Set the database table for the boundary element data: it sets the table name and the name of the columns and allocate them (static)
void Hyd_Element_Floodplain::set_element_boundary_table(QSqlDatabase *ptr_database){
	if(Hyd_Element_Floodplain::boundary_table==NULL){
		//make specific input for this class
		const string tab_id_name=hyd_label::tab_fpelem_bound;
		string tab_id_col[8];
		tab_id_col[0]=hyd_label::elemdata_glob_id;
		tab_id_col[1]=label::areastate_id;
		tab_id_col[2]=label::measure_id;
		tab_id_col[3]=hyd_label::sz_bound_id;
		tab_id_col[4]=hyd_label::bounddata_stat;
		tab_id_col[5]=hyd_label::bounddata_value;
		tab_id_col[6]=hyd_label::bounddata_type;
		tab_id_col[7]=label::applied_flag;

		try{
			Hyd_Element_Floodplain::boundary_table= new Tables(tab_id_name, tab_id_col, sizeof(tab_id_col)/sizeof(tab_id_col[0]));
			Hyd_Element_Floodplain::boundary_table->set_name(ptr_database, _sys_table_type::hyd);
		}
		catch(bad_alloc& t){
			Error msg;
			msg.set_msg("Hyd_Element_Floodplain::set_element_boundary_table(QSqlDatabase *ptr_database)","Can not allocate the memory", "Check the memory", 10, false);
			ostringstream info;
			info<< "Info bad alloc: " << t.what() << endl;
			msg.make_second_info(info.str());
			throw msg;
		}
		catch(Error msg){
			Hyd_Element_Floodplain::close_boundary_table();
			throw msg;
		}
	}
}
//Delete the boundary condition data in the database table for a given boundary scenario (static)
void Hyd_Element_Floodplain::delete_boundary_cond_by_scenario(QSqlDatabase *ptr_database, const int sc_id){
	QSqlQueryModel results;

	try{
		Hyd_Element_Floodplain::set_element_boundary_table(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}
	ostringstream test_filter;

	test_filter<< "Delete ";
	test_filter << " from " << Hyd_Element_Floodplain::boundary_table->get_table_name();
	test_filter << " where ";
	test_filter << Hyd_Element_Floodplain::boundary_table->get_column_name(hyd_label::sz_bound_id) << " = " << sc_id;

	Data_Base::database_request(&results, test_filter.str(), ptr_database);

	//check the request
	if(results.lastError().isValid()){
		Error msg;
		msg.set_msg("Hyd_Element_Floodplain::delete_boundary_cond_by_scenario(QSqlDatabase *ptr_database, const int sc_id)","Invalid database request", "Check the database", 2, false);
		ostringstream info;
		info << "Table Name      : " << Hyd_Element_Floodplain::boundary_table->get_table_name() << endl;
		info << "Table error info: " << results.lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
}
//Delete the boundary condition data in the database table for a given system state
void Hyd_Element_Floodplain::delete_boundary_cond_by_system_state(QSqlDatabase *ptr_database, const _sys_system_id id){
	QSqlQueryModel results;

	try{
		Hyd_Element_Floodplain::set_element_boundary_table(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}
	ostringstream test_filter;

	test_filter<< "Delete ";
	test_filter << " from " << Hyd_Element_Floodplain::boundary_table->get_table_name();
	test_filter  << " WHERE ";
	test_filter  << Hyd_Element_Floodplain::boundary_table->get_column_name(label::areastate_id) << " = "  << id.area_state;
	test_filter  << " AND ";
	test_filter  <<  Hyd_Element_Floodplain::boundary_table->get_column_name(label::measure_id) << " = " << id.measure_nr ;

	Data_Base::database_request(&results, test_filter.str(), ptr_database);

	//check the request
	if(results.lastError().isValid()){
		Error msg;
		msg.set_msg("Hyd_Element_Floodplain::delete_boundary_cond_by_scenario(QSqlDatabase *ptr_database, const _sys_system_id id)","Invalid database request", "Check the database", 2, false);
		ostringstream info;
		info << "Table Name      : " << Hyd_Element_Floodplain::boundary_table->get_table_name() << endl;
		info << "Table error info: " << results.lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
}
//Select and count the number of relevant boundary_condition in a database table (static)
int Hyd_Element_Floodplain::select_relevant_boundary_cond_database(QSqlQueryModel *results, QSqlDatabase *ptr_database, const _sys_system_id id, const int sc_bound_id, const bool with_output){
	int number=0;
	//the table is set (the name and the column names) and allocated
	try{
		Hyd_Element_Floodplain::set_element_boundary_table(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}
	if(with_output==true){
			ostringstream cout;
			cout << "Search for relevant floodplain elements boundary condition(s) in database ..." << endl ;
			Sys_Common_Output::output_hyd->output_txt(&cout);
	}

	ostringstream test_filter;
	test_filter<< "Select ";
	test_filter << Hyd_Element_Floodplain::boundary_table->get_column_name(hyd_label::elemdata_glob_id) << " , ";
	test_filter << Hyd_Element_Floodplain::boundary_table->get_column_name(hyd_label::bounddata_stat) << " , ";
	test_filter << Hyd_Element_Floodplain::boundary_table->get_column_name(hyd_label::bounddata_type) << " , ";
	test_filter << Hyd_Element_Floodplain::boundary_table->get_column_name(hyd_label::bounddata_value) << " , ";
	test_filter << Hyd_Element_Floodplain::boundary_table->get_column_name(label::measure_id) << "  ";
	test_filter << " from " << Hyd_Element_Floodplain::boundary_table->get_table_name();
	test_filter << " where ";
	test_filter << Hyd_Element_Floodplain::boundary_table->get_column_name(label::applied_flag) << "= true";
	test_filter << " and ";
	test_filter << Hyd_Element_Floodplain::boundary_table->get_column_name(label::areastate_id) << " = "  << id.area_state;
	test_filter << " and (";
	test_filter << Hyd_Element_Floodplain::boundary_table->get_column_name(label::measure_id) << " = " << 0 ;
	test_filter << " or " ;
	test_filter << Hyd_Element_Floodplain::boundary_table->get_column_name(label::measure_id) << " = " << id.measure_nr;
	test_filter << " ) " ;
	test_filter << " and ";
	test_filter << Hyd_Element_Floodplain::boundary_table->get_column_name(hyd_label::sz_bound_id) << " = " << sc_bound_id;
	test_filter << " order by " << Hyd_Element_Floodplain::boundary_table->get_column_name(hyd_label::elemdata_glob_id);

	Data_Base::database_request(results, test_filter.str(), ptr_database);

	//check the request
	if(results->lastError().isValid()){
		Error msg;
		msg.set_msg("Hyd_Element_Floodplain::select_relevant_boundary_cond_database(QSqlQueryModel *results, QSqlDatabase *ptr_database, const _sys_system_id id, const bool with_output = true)","Invalid database request", "Check the database", 2, false);
		ostringstream info;
		info << "Table Name      : " << Hyd_Element_Floodplain::boundary_table->get_table_name() << endl;
		info << "Table error info: " << results->lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
	number= results->rowCount();

	if(with_output==true){
		ostringstream cout;
		cout << number << " relevant floodplain elements boundary condition(s) are found in database" << endl ;
		Sys_Common_Output::output_hyd->output_txt(&cout);
	}

	return number;
}
//Get a string for transfering the boundary data to database 
string Hyd_Element_Floodplain::get_bound_datastring2database(QSqlDatabase *ptr_database, const int fp_number) {
	string buffer;
	if (this->get_boundary_data().boundary_flag == false) {
		buffer= label::not_set;
		return buffer;
	}

	ostringstream query_string;
	query_string << " ( ";
	query_string << this->glob_elem_number << " , ";
	query_string << this->system_id.area_state << " , ";
	query_string << this->system_id.measure_nr << " , ";
	query_string << "true" << " , ";
	query_string << this->hyd_sz->get_id() << " , ";
	query_string << functions::convert_boolean2string(this->get_boundary_data().stationary_flag) << " , ";
	if (this->get_boundary_data().stationary_flag == true) {
		query_string << this->get_boundary_data().discharge_value << " , ";
	}
	else {
		query_string << this->get_boundary_data().curve_number << " , ";
	}
	query_string << "'" << Hyd_Instationary_Boundary::transfrom_instatboundtype2txt(this->get_boundary_data().boundary_type) << "'" << " ) ";

	buffer = query_string.str();
	return buffer;



}
//Get the header for inserting the element boundary data to database table (static)
string Hyd_Element_Floodplain::get_insert_header_bound_table(QSqlDatabase *ptr_database) {
	try {
		Hyd_Element_Floodplain::set_element_boundary_table(ptr_database);
	}
	catch (Error msg) {
		throw msg;
	}
	//set the query via a query string
	ostringstream query_string;
	query_string << "INSERT INTO  " << Hyd_Element_Floodplain::boundary_table->get_table_name();
	query_string << " ( ";
	query_string << Hyd_Element_Floodplain::boundary_table->get_column_name(hyd_label::elemdata_glob_id) << " , ";
	query_string << Hyd_Element_Floodplain::boundary_table->get_column_name(label::areastate_id) << " , ";
	query_string << Hyd_Element_Floodplain::boundary_table->get_column_name(label::measure_id) << " , ";
	query_string << Hyd_Element_Floodplain::boundary_table->get_column_name(label::applied_flag) << " , ";
	query_string << Hyd_Element_Floodplain::boundary_table->get_column_name(hyd_label::sz_bound_id) << " , ";
	query_string << Hyd_Element_Floodplain::boundary_table->get_column_name(hyd_label::bounddata_stat) << " , ";
	query_string << Hyd_Element_Floodplain::boundary_table->get_column_name(hyd_label::bounddata_value) << " , ";
	query_string << Hyd_Element_Floodplain::boundary_table->get_column_name(hyd_label::bounddata_type) << " ) ";


	query_string << " VALUES  ";

	string buffer;
	buffer = query_string.str();
	return buffer;





}
//Initialize the _Hyd_Element_Floodplain_Type; decide which type is chosen
void Hyd_Element_Floodplain::init_element_type(_hyd_neighbouring_elems neigh_elem, double *width_x, double *width_y, double *area, Hyd_Param_Material *mat_table, const bool clone){
	//allocate the element_type
	try{
		this->allocate_element_type();
	}
	catch(Error msg){
		throw msg;
	}

	try{
		//init the element type
		this->element_type->set_data_buffers(*this->buffer_boundary_data, *this->buffer_flow_data, &this->z_value);
		


		this->delete_data_buffer();

		this->element_type->init_element(neigh_elem, width_x, width_y, area, mat_table, clone);
	}
	catch(Error msg){
		ostringstream info;
		info << "Elementnumber " << this->elem_number << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
}
//set the element number
void Hyd_Element_Floodplain::set_elem_number(const int number){
	this->elem_number=number;
}
//Set the mid_point of the element
void Hyd_Element_Floodplain::set_mid_point(Geo_Point *mid_point){
	this->mid_point=*mid_point;
}
//Set the element type
void Hyd_Element_Floodplain::set_element_type(_hyd_elem_type type){
	//1. noinfo
	if(this->type==_hyd_elem_type::NOINFO_ELEM ){
		return;
	}
	if(type==_hyd_elem_type::NOINFO_ELEM){
		this->type=type;
		return;
	}
	//2. noflow
	if(this->type==_hyd_elem_type::NOFLOW_ELEM ){
		return;
	}
	if(type==_hyd_elem_type::NOFLOW_ELEM){
		this->type=type;
		return;
	}
	//3. otherfp
	if(this->type==_hyd_elem_type::OTHER_FP_ELEM){
		return;
	}
	if(type==_hyd_elem_type::OTHER_FP_ELEM){
		this->type=type;
		return;
	}
	//4. coast
	if(this->type==_hyd_elem_type::COAST_ELEM){
		return;
	}
	if(type==_hyd_elem_type::COAST_ELEM){
		this->type=type;
		return;
	}

	//6. river
	if(this->type==_hyd_elem_type::RIVER_ELEM){
		return;
	}
	if(type==_hyd_elem_type::RIVER_ELEM){
		this->type=type;
		return;
	}
	//7. dikeline
	if(this->type==_hyd_elem_type::DIKELINE_ELEM){
		return;
	}
	if(type==_hyd_elem_type::DIKELINE_ELEM){
		this->type=type;
		return;
	}
	//rest
	this->type=type;
	return;
}
//Set element flag for flow in x-direction
void Hyd_Element_Floodplain::set_x_noflow_flag(const bool flag){
	if(this->buffer_flow_data!=NULL){
		this->buffer_flow_data->no_flow_x_flag=flag;
	}
	if(this->element_type!=NULL){
		this->element_type->set_x_noflow_flag(flag);
	}
}
//Set element flag for flow in y-direction
void Hyd_Element_Floodplain::set_y_noflow_flag(const bool flag){
	if(this->buffer_flow_data!=NULL){
		this->buffer_flow_data->no_flow_y_flag=flag;
	}
	if(this->element_type!=NULL){
		this->element_type->set_y_noflow_flag(flag);
	}
}
//Set the absolute element border height z (m) in x-direction and the Poleni-factor
void Hyd_Element_Floodplain::set_x_border_z(const double abs_height, const double pol_factor){
	if(this->buffer_flow_data!=NULL){
		if(this->buffer_flow_data->poleni_flag_x==false){
			this->buffer_flow_data->height_border_x_abs=abs_height;
			this->buffer_flow_data->poleni_x=pol_factor;
			this->buffer_flow_data->poleni_flag_x=true;
		}
		else{//take the mid value
			this->buffer_flow_data->poleni_x=(pol_factor+this->buffer_flow_data->poleni_x)*0.5;
			this->buffer_flow_data->height_border_x_abs=(abs_height+this->buffer_flow_data->height_border_x_abs)*0.5;
		}
	}
	if(this->element_type!=NULL){
		this->element_type->set_x_border_z(abs_height, pol_factor);
	}
}
//Set the absolute element border height z (m) in y-direction and the Poleni-factor
void Hyd_Element_Floodplain::set_y_border_z(const double abs_height, const double pol_factor){
	if(this->buffer_flow_data!=NULL){
		if(this->buffer_flow_data->poleni_flag_y==false){
			this->buffer_flow_data->height_border_y_abs=abs_height;
			this->buffer_flow_data->poleni_y=pol_factor;
			this->buffer_flow_data->poleni_flag_y=true;
		}
		else{//take the mid value
			this->buffer_flow_data->poleni_y=(pol_factor+this->buffer_flow_data->poleni_y)*0.5;
			this->buffer_flow_data->height_border_y_abs=(abs_height+this->buffer_flow_data->height_border_y_abs)*0.5;
		}
	}
	if(this->element_type!=NULL){
		this->element_type->set_y_border_z(abs_height, pol_factor);
	}
}
//Set if there are no data are available for an element
void Hyd_Element_Floodplain::set_no_info_element(const int no_info_value){
	int casted_height;
	casted_height=(int)this->z_value;
	if(casted_height==no_info_value){
		this->type=_hyd_elem_type::NOINFO_ELEM;
	}
}
//Get a pointer to the flow data
_hyd_flowdata_floodplain_elem Hyd_Element_Floodplain::get_flow_data(void){
	if(this->element_type!=NULL){
		return (this->element_type->get_flow_data());
	}
	else{
		return *this->buffer_flow_data;
	}
}
//Get a pointer to the boundary data
_hyd_boundarydata_floodplain_elem Hyd_Element_Floodplain::get_boundary_data(void){
	if(this->element_type!=NULL){
		return (this->element_type->get_boundary_data());
	}
	else{
		return *this->buffer_boundary_data;
	}
}
//get the geodetic height z_value
double Hyd_Element_Floodplain::get_z_value(void){
	return this->z_value;
}
//Get the mid point
Geo_Point* Hyd_Element_Floodplain::get_mid_point(void){
	return &(this->mid_point);
}
//Get element number
int Hyd_Element_Floodplain::get_elem_number(void){
	return this->elem_number;
}
//Get the coupling relevant element index
int Hyd_Element_Floodplain::get_coupling_relevant_elem_index(void){
	if(this->element_type==NULL){
		return this->elem_number;
	}
	else{
		if(this->element_type->get_coupling_relevant_element_number()==-1){
			return this->elem_number;
		}
		else{
			return this->element_type->get_coupling_relevant_element_number();
		}
	}
}
//Get element type
_hyd_elem_type Hyd_Element_Floodplain::get_elem_type(void){
	return this->type;
}
//output the input values
void Hyd_Element_Floodplain::output_members(ostringstream *cout){
	*cout << this->elem_number ;
	*cout << W(12) << P(2) << FORMAT_FIXED_REAL << this->z_value;
	*cout << W(12) << P(2) << FORMAT_FIXED_REAL << this->get_flow_data().init_condition;
	*cout << W(18) << this->get_flow_data().mat_type;
	*cout << W(20) << functions::convert_boolean2string(this->get_boundary_data().boundary_flag);
	if(this->get_boundary_data().boundary_flag!= false){
		*cout << W(15) << functions::convert_boolean2string(this->get_boundary_data().stationary_flag);
		if(this->get_boundary_data().stationary_flag==true){
			*cout << W(17) << P(2) << FORMAT_FIXED_REAL << this->get_boundary_data().discharge_value;
		}
		else{
			*cout << W(17) << this->get_boundary_data().curve_number;
		}
		*cout << W(17) << Hyd_Instationary_Boundary::transfrom_instatboundtype2txt(this->get_boundary_data().boundary_type);
	}
	*cout << endl;
}
//(static) output the header of the setted members
void Hyd_Element_Floodplain::output_member_header(ostringstream *cout){
	*cout << "No." << W(8) <<  "Height" << label::m << W(15) << "Initial Condition" << label::m;
	*cout << W(15) << "Material Type" << W(16) << "Boundary Flag" ;
	*cout << W(17) << "Stationary flag" << W(17) << "Boundary Value" << W(17) << "Boundary Type" << endl;
}
//output setted members
void Hyd_Element_Floodplain::output_setted_members(ostringstream *cout){
	*cout << this->elem_number ;
	*cout << W(10) << Hyd_Element_Floodplain::convert_elem_type2txt(this->type);
	*cout << W(13) << P(2) << FORMAT_FIXED_REAL<< this->mid_point.get_xcoordinate();
	*cout << W(15) << P(2) << FORMAT_FIXED_REAL<< this->mid_point.get_ycoordinate();

	if(this->element_type!=NULL){
		this->element_type->output_setted_members(cout);
	}
	*cout << endl;
}
//(static) output the header of the setted members
void Hyd_Element_Floodplain::output_setted_header(ostringstream *cout){
	*cout << "No." << W(8) << "Type";
	*cout<< W(10) << "x_mid" << label::m << W(10) << "y_mid" << label::m << W(10) << "height" << label::m ;
	*cout << W(10) << "x-flow" << W(10) << "x-Border" << label::m ;
	*cout << W(10) << "y-flow" << W(10) << "y-Border" << label::m;
	*cout << W(10) << "Boundary" << W(10) << "Value/Nr";
	*cout << W(10) << "Mat-Type" << W(10) << "n-value";
	*cout << W(10) <<"Init" << label::m;
	*cout << W(10) <<"Conn-Elem";
	*cout<< endl;
}
//Ouput the result members per internal timestep
void Hyd_Element_Floodplain::output_result_values(ostringstream *cout){
	*cout<<this->elem_number;
	if(this->element_type!=NULL){
		this->element_type->output_result_values(cout);
	}
	else{
		*cout <<"Not set" << endl;
	}
}
//(static) Ouput the header for the maximum calculated values output
void Hyd_Element_Floodplain::output_header_result_values(ostringstream *cout){
	*cout<<"Result values:"<<endl;
	*cout << "No.";
	_Hyd_Element_Floodplain_Type::output_header_result_values(cout);
}
//Ouput the maximum calculated values
void Hyd_Element_Floodplain::output_maximum_calculated_values(ostringstream *cout){
	*cout<<this->elem_number;
	if(this->element_type!=NULL){
		this->element_type->output_maximum_calculated_values(cout);
	}
	else{
		*cout <<"Not set" << endl;
	}
}
//(static) Ouput the header for the maximum calculated values output
void Hyd_Element_Floodplain::output_header_maximum_calculated_values(ostringstream *cout){
	*cout<<"MAXIMUM VALUES/VOLUMES PER ELEMENT"<<endl;
	*cout << "No.";
	_Hyd_Element_Floodplain_Type::output_header_maximum_calculated_values(cout);
}
//Get the header for inserting the element result data to database table (static)
string Hyd_Element_Floodplain::get_insert_header_erg_data_table(QSqlDatabase *ptr_database){
	//the table is set (the name and the column names) and allocated
	try{
		Hyd_Element_Floodplain::set_erg_table(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}

	//set the query via a query string
	ostringstream query_string;
	query_string << "INSERT INTO  " << Hyd_Element_Floodplain::erg_table->get_table_name();
	query_string <<" ( ";
	query_string << Hyd_Element_Floodplain::erg_table->get_column_name(hyd_label::elemdata_fpno) <<" , ";
	query_string << Hyd_Element_Floodplain::erg_table->get_column_name(hyd_label::elemdata_id) <<" , ";
	query_string << Hyd_Element_Floodplain::erg_table->get_column_name(label::areastate_id) <<" , ";
	query_string << Hyd_Element_Floodplain::erg_table->get_column_name(label::measure_id) <<" , ";
	query_string << Hyd_Element_Floodplain::erg_table->get_column_name(hyd_label::sz_bound_id) <<" , ";
	query_string << Hyd_Element_Floodplain::erg_table->get_column_name(risk_label::sz_break_id) <<" , ";
	query_string << Hyd_Element_Floodplain::erg_table->get_column_name(hyd_label::elemdata_glob_id) <<" , ";
	//max values
	query_string << Hyd_Element_Floodplain::erg_table->get_column_name(hyd_label::elemerg_h_max) <<" , ";
	query_string << Hyd_Element_Floodplain::erg_table->get_column_name(hyd_label::elemerg_s_max) <<" , ";
	query_string << Hyd_Element_Floodplain::erg_table->get_column_name(hyd_label::elemerg_dsdt_max) <<" , ";
	query_string << Hyd_Element_Floodplain::erg_table->get_column_name(hyd_label::elemerg_vx_max) <<" , ";
	query_string << Hyd_Element_Floodplain::erg_table->get_column_name(hyd_label::elemerg_vy_max) <<" , ";
	query_string << Hyd_Element_Floodplain::erg_table->get_column_name(hyd_label::elemerg_vtot_max) <<" , ";
	query_string << Hyd_Element_Floodplain::erg_table->get_column_name(hyd_label::elemerg_hv_max) <<" , ";
	query_string << Hyd_Element_Floodplain::erg_table->get_column_name(hyd_label::elemerg_t_first) <<" , ";
	query_string << Hyd_Element_Floodplain::erg_table->get_column_name(hyd_label::elemerg_dur_wet) <<" , ";
	query_string << Hyd_Element_Floodplain::erg_table->get_column_name(hyd_label::elemerg_end_vol) <<" , ";
	//volumes
	query_string << Hyd_Element_Floodplain::erg_table->get_column_name(hyd_label::elemerg_cv_bound_in) <<" , ";
	query_string << Hyd_Element_Floodplain::erg_table->get_column_name(hyd_label::elemerg_cv_bound_out) <<" , ";

	query_string << Hyd_Element_Floodplain::erg_table->get_column_name(hyd_label::elemerg_cv_struc_in) <<" , ";
	query_string << Hyd_Element_Floodplain::erg_table->get_column_name(hyd_label::elemerg_cv_struc_out) <<" , ";
	query_string << Hyd_Element_Floodplain::erg_table->get_column_name(hyd_label::elemerg_cv_dir_in) <<" , ";
	query_string << Hyd_Element_Floodplain::erg_table->get_column_name(hyd_label::elemerg_cv_dir_out) <<" , ";

	query_string << Hyd_Element_Floodplain::erg_table->get_column_name(hyd_label::elemerg_cv_rv_ov_in) <<" , ";
	query_string << Hyd_Element_Floodplain::erg_table->get_column_name(hyd_label::elemerg_cv_rv_ov_out) <<" , ";
	query_string << Hyd_Element_Floodplain::erg_table->get_column_name(hyd_label::elemerg_cv_rv_db_in) <<" , ";
	query_string << Hyd_Element_Floodplain::erg_table->get_column_name(hyd_label::elemerg_cv_rv_db_out) <<" , ";

	query_string << Hyd_Element_Floodplain::erg_table->get_column_name(hyd_label::elemerg_cv_co_ov_in) <<" , ";
	query_string << Hyd_Element_Floodplain::erg_table->get_column_name(hyd_label::elemerg_cv_co_ov_out) <<" , ";
	query_string << Hyd_Element_Floodplain::erg_table->get_column_name(hyd_label::elemerg_cv_co_db_in) <<" , ";
	query_string << Hyd_Element_Floodplain::erg_table->get_column_name(hyd_label::elemerg_cv_co_db_out) <<" , ";

	query_string << Hyd_Element_Floodplain::erg_table->get_column_name(hyd_label::elemerg_cv_fp_in) <<" , ";
	query_string << Hyd_Element_Floodplain::erg_table->get_column_name(hyd_label::elemerg_cv_fp_out) <<" , ";

	query_string << Hyd_Element_Floodplain::erg_table->get_column_name(hyd_label::elemdata_polygon) <<" ) ";

	query_string << " VALUES ";

	string buffer;
	buffer=query_string.str();
	return buffer;
}
//Get a string for transfering the result data to database
string Hyd_Element_Floodplain::get_datastring_erg2database(const int glob_elem_id, const int fp_number, const _hyd_floodplain_geo_info geo_info, const string break_sz, const bool must_output){
	string buffer=label::not_set;

	if(this->element_type->get_was_wet_flag()==false && must_output==false){
		return buffer;
	}

	ostringstream query_string;
	query_string << " ( ";
	query_string << fp_number << " , " ;
	query_string << this->elem_number << " , " ;
	query_string << this->system_id.area_state << " , " ;
	query_string << this->system_id.measure_nr << " , " ;
	query_string << this->hyd_sz->get_id() << " , " ;
	query_string << "'"<<break_sz<< "' , " ;
	query_string << glob_elem_id << " , " ;
	this->element_type->set_maximum_value2querystring(&query_string);

	if(this->polygon_string==label::not_set){
		//set the polgon points
		Geo_Raster_Polygon poly_buff;
		poly_buff.set_points(&this->mid_point, geo_info.width_x, geo_info.width_y, geo_info.angle, geo_info.origin_global_x, geo_info.origin_global_y);
		this->polygon_string=poly_buff.get_polygon2sql_string();
		query_string << this->polygon_string <<" ) ";
	}
	else{
		query_string << this->polygon_string <<" ) ";
	}
	this->polygon_string=label::not_set;

	buffer=query_string.str();
	return buffer;
}
//Get the header for inserting the element instationary result data to database table (static)
string Hyd_Element_Floodplain::get_insert_header_erg_instat_data_table(QSqlDatabase *ptr_database) {
	//the table is set (the name and the column names) and allocated
	try {
		Hyd_Element_Floodplain::set_erg_instat_table(ptr_database);
	}
	catch (Error msg) {
		throw msg;
	}

	//set the query via a query string
	ostringstream query_string;
	query_string << "INSERT INTO  " << Hyd_Element_Floodplain::erg_instat_table->get_table_name();
	query_string << " ( ";
	query_string << Hyd_Element_Floodplain::erg_instat_table->get_column_name(label::glob_id) << " , ";
	query_string << Hyd_Element_Floodplain::erg_instat_table->get_column_name(label::areastate_id) << " , ";
	query_string << Hyd_Element_Floodplain::erg_instat_table->get_column_name(label::measure_id) << " , ";
	query_string << Hyd_Element_Floodplain::erg_instat_table->get_column_name(hyd_label::sz_bound_id) << " , ";
	query_string << Hyd_Element_Floodplain::erg_instat_table->get_column_name(risk_label::sz_break_id) << " , ";
	query_string << Hyd_Element_Floodplain::erg_instat_table->get_column_name(hyd_label::elemdata_glob_id) << " , ";
	query_string << Hyd_Element_Floodplain::erg_instat_table->get_column_name(hyd_label::elemdata_fpno) << " , ";
	query_string << Hyd_Element_Floodplain::erg_instat_table->get_column_name(hyd_label::elemdata_id) << " , ";


	//max values
	query_string << Hyd_Element_Floodplain::erg_instat_table->get_column_name(hyd_label::elemerg_h_max) << " , ";
	query_string << Hyd_Element_Floodplain::erg_instat_table->get_column_name(hyd_label::elemerg_s_max) << " , ";
	query_string << Hyd_Element_Floodplain::erg_instat_table->get_column_name(hyd_label::elemerg_dsdt_max) << " , ";
	query_string << Hyd_Element_Floodplain::erg_instat_table->get_column_name(hyd_label::elemerg_vx_max) << " , ";
	query_string << Hyd_Element_Floodplain::erg_instat_table->get_column_name(hyd_label::elemerg_vy_max) << " , ";
	query_string << Hyd_Element_Floodplain::erg_instat_table->get_column_name(hyd_label::elemerg_vtot_max) << " , ";
	query_string << Hyd_Element_Floodplain::erg_instat_table->get_column_name(hyd_label::elemerg_hv_max) << " , ";
	query_string << Hyd_Element_Floodplain::erg_instat_table->get_column_name(hyd_label::data_time) << " , ";
	query_string << Hyd_Element_Floodplain::erg_instat_table->get_column_name(hyd_label::elemdata_polygon) << " ) ";

	

	query_string << " VALUES ";

	string buffer;
	buffer = query_string.str();
	return buffer;

}
//Get a string for transfering the instationary result data to database 
string Hyd_Element_Floodplain::get_datastring_erg_instat_2database(const int id, const int fp_number, const int elem_id, const string break_sz, const string time, const _hyd_floodplain_geo_info geo_info) {
	string buffer = label::not_set;

	if (this->element_type->get_was_wet_flag() == false && id !=0 ) {
		return buffer;
	}

	ostringstream query_string;
	query_string << " ( ";
	query_string << elem_id << " , ";
	query_string << this->system_id.area_state << " , ";
	query_string << this->system_id.measure_nr << " , ";
	query_string << this->hyd_sz->get_id() << " , ";
	query_string << "'" << break_sz << "' , ";
	query_string << this->glob_elem_number << " , ";
	query_string << fp_number << " , ";
	query_string << this->elem_number << " , ";
	this->element_type->set_instat_value2querystring(&query_string);
	query_string << time << " , ";


	if (this->polygon_string == label::not_set) {
		//set the polgon points
		Geo_Raster_Polygon poly_buff;
		poly_buff.set_points(&this->mid_point, geo_info.width_x, geo_info.width_y, geo_info.angle, geo_info.origin_global_x, geo_info.origin_global_y);
		this->polygon_string = poly_buff.get_polygon2sql_string();
		query_string << this->polygon_string << " ) ";
	}
	else {
		query_string << this->polygon_string << " ) ";
	}
	this->polygon_string = label::not_set;

	


	buffer = query_string.str();
	return buffer;


}
//Output the maximum calculated values to the database table (erg_table)
void Hyd_Element_Floodplain::output_maximum_calculated_values(int *id_glob, QSqlQuery *model, QSqlDatabase *ptr_database, const int fp_no, const _hyd_floodplain_geo_info geo_info, const string break_sz){
	if(this->element_type->get_was_wet_flag()==false){
		return;
	}

	//the table is set (the name and the column names) and allocated
	try{
		Hyd_Element_Floodplain::set_erg_table(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}

	//set the query via a query string
	ostringstream query_string;
	query_string << "INSERT INTO  " << Hyd_Element_Floodplain::erg_table->get_table_name();
	query_string <<" ( ";
	query_string << Hyd_Element_Floodplain::erg_table->get_column_name(hyd_label::elemdata_fpno) <<" , ";
	query_string << Hyd_Element_Floodplain::erg_table->get_column_name(hyd_label::elemdata_id) <<" , ";
	query_string << Hyd_Element_Floodplain::erg_table->get_column_name(label::areastate_id) <<" , ";
	query_string << Hyd_Element_Floodplain::erg_table->get_column_name(label::measure_id) <<" , ";
	query_string << Hyd_Element_Floodplain::erg_table->get_column_name(hyd_label::sz_bound_id) <<" , ";
	query_string << Hyd_Element_Floodplain::erg_table->get_column_name(risk_label::sz_break_id) <<" , ";
	query_string << Hyd_Element_Floodplain::erg_table->get_column_name(hyd_label::elemdata_glob_id) <<" , ";
	//max values
	query_string << Hyd_Element_Floodplain::erg_table->get_column_name(hyd_label::elemerg_h_max) <<" , ";
	query_string << Hyd_Element_Floodplain::erg_table->get_column_name(hyd_label::elemerg_dsdt_max) <<" , ";
	query_string << Hyd_Element_Floodplain::erg_table->get_column_name(hyd_label::elemerg_vx_max) <<" , ";
	query_string << Hyd_Element_Floodplain::erg_table->get_column_name(hyd_label::elemerg_vy_max) <<" , ";
	query_string << Hyd_Element_Floodplain::erg_table->get_column_name(hyd_label::elemerg_vtot_max) <<" , ";
	query_string << Hyd_Element_Floodplain::erg_table->get_column_name(hyd_label::elemerg_hv_max) <<" , ";
	query_string << Hyd_Element_Floodplain::erg_table->get_column_name(hyd_label::elemerg_t_first) <<" , ";
	query_string << Hyd_Element_Floodplain::erg_table->get_column_name(hyd_label::elemerg_dur_wet) <<" , ";
	query_string << Hyd_Element_Floodplain::erg_table->get_column_name(hyd_label::elemerg_end_vol) <<" , ";
	//volumes
	query_string << Hyd_Element_Floodplain::erg_table->get_column_name(hyd_label::elemerg_cv_bound_in) <<" , ";
	query_string << Hyd_Element_Floodplain::erg_table->get_column_name(hyd_label::elemerg_cv_bound_out) <<" , ";

	query_string << Hyd_Element_Floodplain::erg_table->get_column_name(hyd_label::elemerg_cv_struc_in) <<" , ";
	query_string << Hyd_Element_Floodplain::erg_table->get_column_name(hyd_label::elemerg_cv_struc_out) <<" , ";
	query_string << Hyd_Element_Floodplain::erg_table->get_column_name(hyd_label::elemerg_cv_dir_in) <<" , ";
	query_string << Hyd_Element_Floodplain::erg_table->get_column_name(hyd_label::elemerg_cv_dir_out) <<" , ";

	query_string << Hyd_Element_Floodplain::erg_table->get_column_name(hyd_label::elemerg_cv_rv_ov_in) <<" , ";
	query_string << Hyd_Element_Floodplain::erg_table->get_column_name(hyd_label::elemerg_cv_rv_ov_out) <<" , ";
	query_string << Hyd_Element_Floodplain::erg_table->get_column_name(hyd_label::elemerg_cv_rv_db_in) <<" , ";
	query_string << Hyd_Element_Floodplain::erg_table->get_column_name(hyd_label::elemerg_cv_rv_db_out) <<" , ";

	query_string << Hyd_Element_Floodplain::erg_table->get_column_name(hyd_label::elemerg_cv_co_ov_in) <<" , ";
	query_string << Hyd_Element_Floodplain::erg_table->get_column_name(hyd_label::elemerg_cv_co_ov_out) <<" , ";
	query_string << Hyd_Element_Floodplain::erg_table->get_column_name(hyd_label::elemerg_cv_co_db_in) <<" , ";
	query_string << Hyd_Element_Floodplain::erg_table->get_column_name(hyd_label::elemerg_cv_co_db_out) <<" , ";

	query_string << Hyd_Element_Floodplain::erg_table->get_column_name(hyd_label::elemerg_cv_fp_in) <<" , ";
	query_string << Hyd_Element_Floodplain::erg_table->get_column_name(hyd_label::elemerg_cv_fp_out) <<" , ";

	query_string << Hyd_Element_Floodplain::erg_table->get_column_name(hyd_label::elemdata_polygon) <<" ) ";
	query_string << " VALUES ( ";
	query_string << fp_no << " , " ;
	query_string << this->elem_number << " , " ;
	query_string << this->system_id.area_state << " , " ;
	query_string << this->system_id.measure_nr << " , " ;
	query_string << this->hyd_sz->get_id() << " , " ;
	query_string << "'"<<break_sz<< "' , " ;
	query_string << *id_glob << " , " ;
	this->element_type->set_maximum_value2querystring(&query_string);

	if(this->polygon_string==label::not_set){
		//set the polgon points
		Geo_Raster_Polygon poly_buff;
		poly_buff.set_points(&this->mid_point, geo_info.width_x, geo_info.width_y, geo_info.angle, geo_info.origin_global_x, geo_info.origin_global_y);
		this->polygon_string=poly_buff.get_polygon2sql_string();
		query_string << this->polygon_string <<" ) ";
	}
	else{
		query_string << this->polygon_string <<" ) ";	}

	Data_Base::database_request(model, query_string.str(), ptr_database);

	//count the global index
	*id_glob=*id_glob+1;

	if(model->lastError().isValid()){
		Warning msg=this->set_warning(3);
		ostringstream info;
		info << "Table Name                : " << Hyd_Element_Floodplain::erg_table->get_table_name() << endl;
		info << "Table error info          : " << model->lastError().text().toStdString() << endl;
		info << "Element number            : " << this->elem_number << endl;
		msg.make_second_info(info.str());
		msg.output_msg(2);
	}
}
//Output the boundaries of the element as a geometry to tecplot
void Hyd_Element_Floodplain::output_boundaries2tecplot(_hyd_floodplain_geo_info *geo_info, ofstream *output_file){
	//x-direction
	//noflow
	if(this->get_flow_data().no_flow_x_flag==true){
		Geo_Point buffer1=this->convert_midpoint2corner(geo_info,_hyd_neighbouring_direction::X_DIR);
		Geo_Point buffer2=this->convert_midpoint2corner(geo_info,_hyd_neighbouring_direction::MINUS_Y_DIR);
		//output a text
		*output_file <<"TEXT X="<<(buffer1.get_xcoordinate()+buffer2.get_xcoordinate())*0.5 <<", Y="<< (buffer1.get_ycoordinate()+buffer2.get_ycoordinate())*0.5;
		*output_file << "T=\"Noflow\", CS=GRID, C=RED, HU=GRID, H=0.5, AN=CENTER, BX=FILLED, BXF=WHITE, BXO=WHITE, ZN=1, A="<<-geo_info->angle-90.0 <<"\""<<endl;
		*output_file << endl;
		//output geometrie
		*output_file << "#Noflow-direction Element-No. " << this->elem_number << endl;
		//*output_file <<"GEOMETRY  X=" <<geo_info->origin_global_x <<", Y="<<geo_info->origin_global_y<<", T=LINE, C=RED, LT=0.25, L=SOLID, CS=GRID, ZN=1" << endl;
		*output_file <<"GEOMETRY  X=" <<0.0 <<", Y="<<0.0<<", T=LINE, C=RED, LT=0.25, L=SOLID, CS=GRID, ZN=1" << endl;

		*output_file <<"1 " << endl;
		*output_file <<"2 " << endl;
		*output_file << "#coordinates x  y" << endl;
		*output_file << buffer1.get_xcoordinate() << "  " << buffer1.get_ycoordinate() << endl;
		*output_file << buffer2.get_xcoordinate() << "  "<< buffer2.get_ycoordinate() << endl;
	}
	//poleni
	if (this->get_flow_data().poleni_flag_x==true){
		Geo_Point buffer1=this->convert_midpoint2corner(geo_info,_hyd_neighbouring_direction::X_DIR);
		Geo_Point buffer2=this->convert_midpoint2corner(geo_info,_hyd_neighbouring_direction::MINUS_Y_DIR);
		//output a text
		*output_file <<"TEXT X="<<(buffer1.get_xcoordinate()+buffer2.get_xcoordinate())*0.5 <<", Y="<< (buffer1.get_ycoordinate()+buffer2.get_ycoordinate())*0.5;
		*output_file<< "T=\"Poleni z="<<this->get_flow_data().height_border_x_abs << label::m ;
		*output_file<<"\", CS=GRID, ZN=1, C=RED, HU=GRID, H=0.5, AN=CENTER, BX=FILLED, BXF=WHITE, BXO=WHITE, A="<<-geo_info->angle-90.0 <<"\""<<endl;
		*output_file << endl;
		//output geometrie
		*output_file << "#Poleni-direction Element-No. " << this->elem_number << endl;
		*output_file <<"GEOMETRY  X=" <<0.0 <<", Y="<<0.0<<", T=LINE, C=RED, LT=0.25, L=DASHED, PL=0.7, CS=GRID, ZN=1" << endl;
		*output_file <<"1" << endl;
		*output_file <<"2 " << endl;
		*output_file << "#coordinates x  y" << endl;
		*output_file << buffer1.get_xcoordinate() << "  " << buffer1.get_ycoordinate() << endl;
		*output_file << buffer2.get_xcoordinate() << "  "<< buffer2.get_ycoordinate() << endl;
	}

	//y-direction
	//noflow
	if(this->get_flow_data().no_flow_y_flag==true){
		Geo_Point buffer1=this->convert_midpoint2corner(geo_info,_hyd_neighbouring_direction::Y_DIR);
		Geo_Point buffer2=this->convert_midpoint2corner(geo_info,_hyd_neighbouring_direction::X_DIR);
		//output a text
		*output_file <<"TEXT X="<< (buffer1.get_xcoordinate()+buffer2.get_xcoordinate())*0.5 <<", Y="<< (buffer1.get_ycoordinate()+buffer2.get_ycoordinate())*0.5;
		*output_file << "T=\"Noflow\", CS=GRID, C=RED, HU=GRID, H=0.5, AN=CENTER, BX=FILLED, BXF=WHITE, BXO=WHITE, ZN=1, A="<<-geo_info->angle <<"\""<<endl;
		*output_file << endl;
		*output_file << "#Noflow-direction Element-No. " << this->elem_number << endl;
		*output_file <<"GEOMETRY  X=" <<0.0 <<", Y="<<0.0<<", T=LINE, C=RED, LT=0.25, L=SOLID, CS=GRID, ZN=1" << endl;
		*output_file <<"1 " << endl;
		*output_file <<"2 " << endl;
		*output_file << "#coordinates x  y" << endl;
		*output_file << buffer1.get_xcoordinate() << "  " << buffer1.get_ycoordinate() << endl;
		*output_file << buffer2.get_xcoordinate() << "  "<< buffer2.get_ycoordinate() << endl;
	}
	//poleni
	if (this->get_flow_data().poleni_flag_y==true){
		Geo_Point buffer1=this->convert_midpoint2corner(geo_info,_hyd_neighbouring_direction::Y_DIR);
		Geo_Point buffer2=this->convert_midpoint2corner(geo_info,_hyd_neighbouring_direction::X_DIR);
		//output a text
		*output_file <<"TEXT X="<< (buffer1.get_xcoordinate()+buffer2.get_xcoordinate())*0.5 <<", Y="<< (buffer1.get_ycoordinate()+buffer2.get_ycoordinate())*0.5;
		*output_file << "T=\"Poleni z="<<this->get_flow_data().height_border_y_abs << label::m ;
		*output_file<<"\", CS=GRID,  C=RED, HU=GRID, H=0.5, AN=CENTER, BX=FILLED, BXF=WHITE, BXO=WHITE, ZN=1, A="<<-geo_info->angle <<"\""<<endl;
		//output geometrie
		*output_file << endl;
		*output_file << "#Poleni-y-direction Element-No. " << this->elem_number << endl;
		*output_file <<"GEOMETRY  X=" <<0.0 <<", Y="<<0.0<<", T=LINE, C=RED, LT=0.25, L=DASHED, PL=0.7, CS=GRID, ZN=1" << endl;
		*output_file <<"1 " << endl;
		*output_file <<"2 " << endl;
		*output_file << "#coordinates x  y" << endl;
		*output_file << buffer1.get_xcoordinate() << "  " << buffer1.get_ycoordinate() << endl;
		*output_file << buffer2.get_xcoordinate() << "  "<< buffer2.get_ycoordinate() << endl;
	}
}
//(static) Conversion function for output
string Hyd_Element_Floodplain::convert_elem_type2txt(_hyd_elem_type type){
	string txt;
	switch(type){
		case _hyd_elem_type::COAST_ELEM:
			txt=hyd_label::coast_elem;
			break;
		case _hyd_elem_type::NOFLOW_ELEM:
			txt=hyd_label::noflow_elem;
			break;
		case _hyd_elem_type::STANDARD_ELEM:
			txt=hyd_label::standard_elem;
			break;
		case _hyd_elem_type::NOINFO_ELEM:
			txt=hyd_label::noinfo_elem;
			break;
		case _hyd_elem_type::RIVER_ELEM:
			txt=hyd_label::river_elem;
			break;
		case _hyd_elem_type::OTHER_FP_ELEM:
			txt=hyd_label::other_fp_elem;
			break;
		case _hyd_elem_type::DIKELINE_ELEM:
			txt=hyd_label::dikeline_elem;
			break;
		default:
			txt==label::unknown_type;
	}
	return txt;
}
//Reset the boundary data of the floodplain element
void Hyd_Element_Floodplain::reset_boundary_data(void){
	if(this->element_type!=NULL && this->element_type->get_bound_flag()==true){
		this->element_type->delete_bound_data();
	}
	this->allocate_data_buffer();
}
//Input the floodplain element boundary data from a database table
void Hyd_Element_Floodplain::input_element_boundarydata2database(QSqlQueryModel *results, const int number, const int glob_id, int *last_index){
	if(number==0){
		return;
	}

    //ostringstream cout;
    //cout <<"Set bound element no " << glob_id<< " "<<*last_index<<endl;
    //Sys_Common_Output::output_hyd->output_txt(&cout);

	int buff_id=0;
	int meas_id=-1;
    for(int i=*last_index; i< number; i++){
		buff_id=results->record(i).value((Hyd_Element_Floodplain::boundary_table->get_column_name(hyd_label::elemdata_glob_id)).c_str()).toInt();
		meas_id=-1;
		if(buff_id>glob_id){
            *last_index=i;
			return;
		}
		if(buff_id==glob_id){
			this->buffer_boundary_data->boundary_flag=true;
			if(results->record(i).value((Hyd_Element_Floodplain::boundary_table->get_column_name(label::measure_id)).c_str()).toInt()>meas_id){
				meas_id=results->record(i).value((Hyd_Element_Floodplain::boundary_table->get_column_name(label::measure_id)).c_str()).toInt();
				//read out the results from the request
				this->buffer_boundary_data->stationary_flag=results->record(i).value((Hyd_Element_Floodplain::boundary_table->get_column_name(hyd_label::bounddata_stat)).c_str()).toBool();
				this->buffer_boundary_data->boundary_type=Hyd_Instationary_Boundary::transfrom_txt2_instatboundtype(results->record(i).value((Hyd_Element_Floodplain::boundary_table->get_column_name(hyd_label::bounddata_type)).c_str()).toString().toStdString());
				if(this->buffer_boundary_data->stationary_flag==true){
					this->buffer_boundary_data->discharge_value=results->record(i).value((Hyd_Element_Floodplain::boundary_table->get_column_name(hyd_label::bounddata_value)).c_str()).toDouble();
				}
				else{
					this->buffer_boundary_data->curve_number=results->record(i).value((Hyd_Element_Floodplain::boundary_table->get_column_name(hyd_label::bounddata_value)).c_str()).toInt();
				}
			}
		}
		if (i == number - 1) {
			*last_index = i;
			return;
		}
	}
}
//Set the boundary data buffer to the element type
void Hyd_Element_Floodplain::set_boundary_data_buffer2type(void){
	try{
		//init the element type
		this->element_type->set_boundary_condition_data(*this->buffer_boundary_data);
		this->delete_data_buffer();
	}
	catch(Error msg){
		ostringstream info;
		info << "Elementnumber " << this->elem_number << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
}
//Set the global element id
void Hyd_Element_Floodplain::set_glob_elem_id(const int id) {
	this->glob_elem_number = id;
}
//Get the global element id
int Hyd_Element_Floodplain::get_glob_elem_id(void) {
	return this->glob_elem_number;
}
//____________
//private
//Allocate the element type
void Hyd_Element_Floodplain::allocate_element_type(void){
	try{
		if(Sys_Project::get_project_type()==_sys_project_type::proj_dam){
			this->element_type=new Hyd_Element_Floodplain_Type_Dam;
		}
		else if(this->type==_hyd_elem_type::STANDARD_ELEM || this->type==_hyd_elem_type::DIKELINE_ELEM){
			this->element_type=new Hyd_Element_Floodplain_Type_Standard;
		}
		else if(this->type==_hyd_elem_type::COAST_ELEM){
			this->element_type=new Hyd_Element_Floodplain_Type_Coast;
		}
		else if(this->type==_hyd_elem_type::NOFLOW_ELEM || this->type==_hyd_elem_type::NOINFO_ELEM || this->type==_hyd_elem_type::OTHER_FP_ELEM){
			this->element_type=new Hyd_Element_Floodplain_Type_Noflow;
		}
		else if(this->type==_hyd_elem_type::RIVER_ELEM){
			this->element_type=new Hyd_Element_Floodplain_Type_River;
		}
	}
	catch(bad_alloc &t){
		Error msg=this->set_error(3);
		ostringstream info;
		info<< "Info bad alloc: " << t.what() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
}
//Delete the element type
void Hyd_Element_Floodplain::delete_element_type(void){
	if(this->element_type!=NULL){
		delete this->element_type;
		this->element_type=NULL;
	}
}
//Conversion function for input
_hyd_elem_type Hyd_Element_Floodplain::convert_txt2elem_type(string txt){
	_hyd_elem_type type;
	_Hyd_Parse_IO::string2lower(&txt);

	if(txt==hyd_label::coast_elem){
		type=_hyd_elem_type::COAST_ELEM;
	}
	else if(txt==hyd_label::noflow_elem){
		type=_hyd_elem_type::NOFLOW_ELEM;
	}
	else if(txt==hyd_label::standard_elem){
		type=_hyd_elem_type::STANDARD_ELEM;
	}
	else if(txt==hyd_label::noinfo_elem){
		type=_hyd_elem_type::NOINFO_ELEM;
	}
	else if(txt==hyd_label::river_elem){
		type=_hyd_elem_type::RIVER_ELEM;
	}
	else if(txt==hyd_label::other_fp_elem){
		type=_hyd_elem_type::OTHER_FP_ELEM;
	}
	else if(txt==hyd_label::dikeline_elem){
		type=_hyd_elem_type::DIKELINE_ELEM;
	}

	else{
		Error msg=this->set_error(0);
		ostringstream info;
		info << "Try to convert text: " << txt << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
	return type;
}
///Allocate the data buffers (_hyd_boundarydata_floodplain_elem and _hyd_flowdata_floodplain_elem)
void Hyd_Element_Floodplain::allocate_data_buffer(void){
	this->delete_data_buffer();

	//allocate the boundary data buffer
	try{
		this->buffer_boundary_data=new _hyd_boundarydata_floodplain_elem;
	}
	catch(bad_alloc &t){
		Error msg=this->set_error(1);
		ostringstream info;
		info<< "Info bad alloc: " << t.what() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
	//count the memory
	Sys_Memory_Count::self()->add_mem(sizeof(_hyd_boundarydata_floodplain_elem), _sys_system_modules::HYD_SYS);
	//init the structure
	this->buffer_boundary_data->boundary_flag=false;
	this->buffer_boundary_data->found_flag=true;
	this->buffer_boundary_data->curve_number=-1;
	this->buffer_boundary_data->stationary_flag=false;
	this->buffer_boundary_data->discharge_value=0.0;
	this->buffer_boundary_data->ptr_boundarycurve=NULL;

	//allocate the flowdata buffer
	try{
		this->buffer_flow_data=new _hyd_flowdata_floodplain_elem;
	}
	catch(bad_alloc &t){
		Error msg=this->set_error(1);
		ostringstream info;
		info<< "Info bad alloc: " << t.what() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
	//count the memory
	Sys_Memory_Count::self()->add_mem(sizeof(_hyd_flowdata_floodplain_elem), _sys_system_modules::HYD_SYS);
	//init the structure
	this->buffer_flow_data->height_border_x_abs=0.0;
	this->buffer_flow_data->height_border_y_abs=0.0;
	this->buffer_flow_data->init_condition=0.0;
	this->buffer_flow_data->mat_type=-1;
	this->buffer_flow_data->n_value=0.0;
	this->buffer_flow_data->no_flow_x_flag=false;
	this->buffer_flow_data->no_flow_y_flag=false;
	this->buffer_flow_data->poleni_flag_x=false;
	this->buffer_flow_data->poleni_flag_y=false;
	this->buffer_flow_data->poleni_x=constant::poleni_const;
	this->buffer_flow_data->poleni_y=constant::poleni_const;
}
///Delete the data buffers (_hyd_boundarydata_floodplain_elem and _hyd_flowdata_floodplain_elem)
void Hyd_Element_Floodplain::delete_data_buffer(void){
	if(this->buffer_boundary_data!=NULL){
		delete this->buffer_boundary_data;
		this->buffer_boundary_data=NULL;
		//count the memory
		Sys_Memory_Count::self()->minus_mem(sizeof(_hyd_boundarydata_floodplain_elem), _sys_system_modules::HYD_SYS);
	}

	if(this->buffer_flow_data!=NULL){
		delete this->buffer_flow_data;
		this->buffer_flow_data=NULL;
		//count the memory
		Sys_Memory_Count::self()->minus_mem(sizeof(_hyd_flowdata_floodplain_elem), _sys_system_modules::HYD_SYS);
	}
}
//Decide for the value of the boundary conditon (stationary => double value; instationary => integer value for the curve number)
void Hyd_Element_Floodplain::decide_boundary_value(const double value){
	if(this->get_boundary_data().boundary_flag==true){
		if(this->get_boundary_data().stationary_flag==true){
			this->buffer_boundary_data->discharge_value=value;
		}
		else{
			this->buffer_boundary_data->curve_number=(int)value;
		}
	}
}
//Convert the midpoint of the element to a corner point with a given direction (Y_DIR:=up left; X_DIR:=up right; MINUS_Y_DIR:=down right; MINUS_X_DIR:=down left)
Geo_Point Hyd_Element_Floodplain::convert_midpoint2corner(_hyd_floodplain_geo_info *geo_info, _hyd_neighbouring_direction dir){
	Geo_Point buffer;
	double x_coor=0.0;
	double y_coor=0.0;
	double rad_angle=_Geo_Geometrie::grad_to_rad(geo_info->angle);
	if(dir==_hyd_neighbouring_direction::Y_DIR){
		x_coor=this->mid_point.get_xcoordinate()-(geo_info->width_x*0.5*cos(rad_angle)-geo_info->width_y*0.5*sin(rad_angle));
		y_coor=this->mid_point.get_ycoordinate()+(geo_info->width_x*0.5*(1.0)*sin(rad_angle)+geo_info->width_y*0.5*cos(rad_angle));
	}
	else if(dir==_hyd_neighbouring_direction::X_DIR){
		x_coor=this->mid_point.get_xcoordinate()+(geo_info->width_x*0.5*cos(rad_angle)+geo_info->width_y*0.5*sin(rad_angle));
		y_coor=this->mid_point.get_ycoordinate()+(geo_info->width_x*0.5*(-1.0)*sin(rad_angle)+geo_info->width_y*0.5*cos(rad_angle));
	}
	else if(dir==_hyd_neighbouring_direction::MINUS_Y_DIR){
		x_coor=this->mid_point.get_xcoordinate()+(geo_info->width_x*0.5*cos(rad_angle)-geo_info->width_y*0.5*sin(rad_angle));
		y_coor=this->mid_point.get_ycoordinate()-(geo_info->width_x*0.5*(1.0)*sin(rad_angle)+geo_info->width_y*0.5*cos(rad_angle));
	}
	else if(dir==_hyd_neighbouring_direction::MINUS_X_DIR){
		x_coor=this->mid_point.get_xcoordinate()-(geo_info->width_x*0.5*cos(rad_angle)+geo_info->width_y*0.5*sin(rad_angle));
		y_coor=this->mid_point.get_ycoordinate()-(geo_info->width_x*0.5*(-1.0)*sin(rad_angle)+geo_info->width_y*0.5*cos(rad_angle));
	}
	buffer.set_point_name(label::elem_corner);
	buffer.set_point_coordinate(x_coor, y_coor);

	return buffer;
}
//Delete all data in the database table for the boundary element data (static)
void Hyd_Element_Floodplain::delete_data_in_boundary_table(QSqlDatabase *ptr_database){
	if(Sys_Project::get_project_type()==_sys_project_type::proj_dam){
		return;
	}
	//the table is set (the name and the column names) and allocated
	try{
		Hyd_Element_Floodplain::set_element_boundary_table(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}
	//delete the table
	Hyd_Element_Floodplain::boundary_table->delete_data_in_table(ptr_database);
}
//Close and delete the database table for the boundary element data (static)
void Hyd_Element_Floodplain::close_boundary_table(void){
	if(Hyd_Element_Floodplain::boundary_table!=NULL){
		delete Hyd_Element_Floodplain::boundary_table;
		Hyd_Element_Floodplain::boundary_table=NULL;
	}
}
//Transfer the floodplain element boundary data to a database table
void Hyd_Element_Floodplain::transfer_element_boundarydata2database(QSqlDatabase *ptr_database, const int glob_id){
	if(this->get_boundary_data().boundary_flag==false){
		return;
	}


    //the table is set (the name and the column names) and allocated
    try{
        Hyd_Element_Floodplain::set_element_boundary_table(ptr_database);
    }
    catch(Error msg){
        throw msg;
    }
    //mysql query with the table_model
    QSqlQueryModel model;

    //set the query via a query string
    ostringstream query_string;
    query_string << "INSERT INTO  " << Hyd_Element_Floodplain::boundary_table->get_table_name();
    query_string <<" ( ";
    query_string << Hyd_Element_Floodplain::boundary_table->get_column_name(hyd_label::elemdata_glob_id) <<" , ";
    query_string << Hyd_Element_Floodplain::boundary_table->get_column_name(label::areastate_id) <<" , ";
    query_string << Hyd_Element_Floodplain::boundary_table->get_column_name(label::measure_id) <<" , ";
    query_string << Hyd_Element_Floodplain::boundary_table->get_column_name(label::applied_flag) <<" , ";
    query_string << Hyd_Element_Floodplain::boundary_table->get_column_name(hyd_label::sz_bound_id) <<" , ";
    query_string << Hyd_Element_Floodplain::boundary_table->get_column_name(hyd_label::bounddata_stat) <<" , ";
    query_string << Hyd_Element_Floodplain::boundary_table->get_column_name(hyd_label::bounddata_value) <<" , ";
    query_string << Hyd_Element_Floodplain::boundary_table->get_column_name(hyd_label::bounddata_type) <<" ) ";


    query_string << " VALUES ( ";
    query_string << glob_id << " , " ;
    query_string << this->system_id.area_state<< " , " ;
    query_string << this->system_id.measure_nr<< " , " ;
    query_string << "true" << " , " ;
    query_string << this->hyd_sz->get_id() << " , " ;
    query_string <<  functions::convert_boolean2string(this->get_boundary_data().stationary_flag)<< " , " ;
    if(this->get_boundary_data().stationary_flag==true){
        query_string << this->get_boundary_data().discharge_value<< " , " ;
    }
    else{
       query_string << this->get_boundary_data().curve_number<< " , " ;
    }
    query_string <<"'"<<Hyd_Instationary_Boundary::transfrom_instatboundtype2txt(this->get_boundary_data().boundary_type)<<"'"<< " ) " ;




    Data_Base::database_request(&model, query_string.str(), ptr_database);

    if(model.lastError().isValid()){
        Warning msg=this->set_warning(0);
        ostringstream info;
        info << "Table Name                : " << Hyd_Element_Floodplain::boundary_table->get_table_name() << endl;
        info << "Table error info          : " << model.lastError().text().toStdString() << endl;
        info << "Element number            : " << this->elem_number << endl;
        msg.make_second_info(info.str());
        msg.output_msg(2);
    }




}
//Delete the maximum result row in a database table for this element
void Hyd_Element_Floodplain::delete_max_result_row_in_table(QSqlDatabase *ptr_database, const int fp_no, const string break_sz){
	//the table is set (the name and the column names) and allocated
	try{
		Hyd_Element_Floodplain::set_erg_table(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}

	QSqlQuery query(*ptr_database);
	ostringstream query_string;
	query_string << "DELETE FROM " << Hyd_Element_Floodplain::erg_table->get_table_name() << " ";
	query_string << " WHERE " << Hyd_Element_Floodplain::erg_table->get_column_name(hyd_label::elemdata_fpno) << " = " << fp_no;
	query_string << " AND ";
	query_string << Hyd_Element_Floodplain::erg_table->get_column_name(hyd_label::elemdata_id) << " = " << this->elem_number;
	query_string << " AND ";
	query_string << Hyd_Element_Floodplain::erg_table->get_column_name(label::areastate_id) << " = " << this->system_id.area_state;
	query_string << " AND ";
	query_string << Hyd_Element_Floodplain::erg_table->get_column_name(label::measure_id) << " = " << this->system_id.measure_nr;
	query_string << " AND ";
	query_string << Hyd_Element_Floodplain::erg_table->get_column_name(hyd_label::sz_bound_id) << " = " << this->hyd_sz->get_id();
	query_string << " AND ";
	query_string << Hyd_Element_Floodplain::erg_table->get_column_name(risk_label::sz_break_id) << " = '" << break_sz<<"'";

	//query.exec(query_string.str().c_str());
	Data_Base::database_request(&query, query_string.str(), ptr_database);
}
//Set warning(s)
Warning Hyd_Element_Floodplain::set_warning(const int warn_type){
		string place="Hyd_Element_Floodplain::";
		string help;
		string reason;
		string reaction;
		int type=0;
		Warning msg;
		stringstream info;

	switch (warn_type){
		case 0://input datas can not submitted
			place.append("transfer_element_members2database(QSqlDatabase *ptr_database)");
			reason="Can not submit the data of the floodplain element to the database";
			help="Check the database";
			type=2;
			break;
		case 1://input datas can not submitted the polyline data
			place.append("transfer_element_members2database(QSqlDatabase *ptr_database)");
			reason="Can not submit the polygon data of the floodplain element to the database";
			help="Check the database";
			type=2;
			break;
		case 2://there are multiple data sets to the given element
			place.append("input_element_boundarydata2database(QSqlDatabase *ptr_database, const int glob_id)");
			reason="There are multiple data sets (see table name) to the given global element id";
			help="Check the database";
			reaction="The first found data set is taken";
			info << "Areastate/Measureno.   : " << this->system_id.area_state <<"/"<< this->system_id.measure_nr << endl;
			type=2;
			break;
		case 3://result datas can not submitted
			place.append("output_maximum_calculated_values(QSqlDatabase *ptr_database, const int fp_no)");
			reason="Can not submit the result data of the floodplain element to the database";
			help="Check the database";
			type=2;
			break;
		case 4://there are multiple data sets to the given profile
			place.append("transfer_hydraulic_boundary_sz2database(QSqlDatabase *ptr_database, const int rv_number)");
			reason="There are multiple global id(s) to the given element";
			help="Check the database table";
			reaction="The first found global id of the element is taken";
			info << "Areastate/Measureno.   : " << this->system_id.area_state <<"/"<< this->system_id.measure_nr << endl;
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
//set the error
Error Hyd_Element_Floodplain::set_error(const int err_type){
		string place="Hyd_Element_Floodplain::";
		string help;
		string reason;
		int type=0;
		bool fatal=false;
		stringstream info;
		Error msg;

	switch (err_type){
		case 0://timestep is <=0.0
			place.append("convert_txt2elem_type(const string txt)");
			reason="Unknown element type";
			help="Check the element type";
			info << "Element number: "<< this->elem_number<<endl;
			type=1;
			break;
		case 1://bad alloc
			place.append("allocate_buffer_type_data(void)");
			reason="Can not allocate the memory";
			help="Check the memory";
			type=10;
			break;
		case 2://wrong input sequenze
			place.append("input_value(const string line_of_file)");
			reason="Can not read the element properties properly";
			help="Check the element data in file";
			info << "Element number: "<< this->elem_number<<endl;
			type=5;
			break;
		case 3://bad alloc
			place.append("allocate_element_type(void)");
			reason="Can not allocate the memory";
			help="Check the memory";
			info << "Element number: "<< this->elem_number<<endl;
			type=10;
			break;
		case 4://problems with database query
			place.append("input_element_boundarydata2database(QSqlDatabase *ptr_database, const int glob_id)") ;
			reason="Can not submit the query to the database";
			help="Check the query error";
			type=6;
			break;
		case 5://no matching global id set is found for the given river profile
			place.append("transfer_hydraulic_boundary_sz2database(QSqlDatabase *ptr_database, const int rv_number)") ;
			reason="There is no matching global id to the given floodplain element";
			help="Check the floodplain element table";
			type=2;
			break;
		case 6://problems with database query
			place.append("transfer_hydraulic_boundary_sz2database(QSqlDatabase *ptr_database, const int rv_number)") ;
			reason="Can not submit the query to the database";
			help="Check the query error";
			type=6;
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
