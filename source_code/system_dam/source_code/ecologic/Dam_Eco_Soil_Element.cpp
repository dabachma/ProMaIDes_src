#include "Dam_Headers_Precompiled.h"
//#include "Dam_Eco_Soil_Element.h"
//init static members
Tables *Dam_Eco_Soil_Element::elem_table=NULL;
Tables *Dam_Eco_Soil_Element::elem_erg_table=NULL;

//Default constructor
Dam_Eco_Soil_Element::Dam_Eco_Soil_Element(void){
	this->id_soil_type=-1;
	this->ptr_soil_type=NULL;
	this->id_erosion_type=-1;
	this->ptr_erosion_type=NULL;
	this->no_info_soil_type=false;
	this->no_info_erosion_type=false;

	this->result_dam=0.0;
	this->soil_erosion_zone=-1;

	//count the memory
	Sys_Memory_Count::self()->add_mem(sizeof(Dam_Eco_Soil_Element), _sys_system_modules::DAM_SYS);
}
//Default destructor
Dam_Eco_Soil_Element::~Dam_Eco_Soil_Element(void){
	//count the memory
	Sys_Memory_Count::self()->minus_mem(sizeof(Dam_Eco_Soil_Element), _sys_system_modules::DAM_SYS);
}

//____________
//public
//Transfer the soil-erosion raster element data to a database
void Dam_Eco_Soil_Element::transfer_input_members2database(QSqlDatabase *ptr_database, const int raster_index, const double cellsize, const double x_origin, const double y_origin){
		//mysql query with the query-class
	QSqlQuery model(*ptr_database);
	//the table is set (the name and the column names) and allocated
	try{
		Dam_Eco_Soil_Element::set_elem_table(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}

	//evaluate the global identifier
	int id_glob=Dam_Eco_Soil_Element::elem_table->maximum_int_of_column(Dam_Eco_Soil_Element::elem_table->get_column_name(dam_label::glob_id),ptr_database)+1;
	this->global_index=id_glob;
	//set the query via a query string
	ostringstream query_string;
	query_string << "INSERT INTO  " << Dam_Eco_Soil_Element::elem_table->get_table_name();
	query_string <<" ( ";
	query_string << Dam_Eco_Soil_Element::elem_table->get_column_name(dam_label::glob_id) <<" , ";
	query_string << Dam_Eco_Soil_Element::elem_table->get_column_name(dam_label::elem_id) <<" , ";
	query_string << Dam_Eco_Soil_Element::elem_table->get_column_name(dam_label::raster_id) <<" , ";
	query_string << Dam_Eco_Soil_Element::elem_table->get_column_name(label::areastate_id) <<" , ";
	query_string << Dam_Eco_Soil_Element::elem_table->get_column_name(label::measure_id) <<" , ";
	query_string << Dam_Eco_Soil_Element::elem_table->get_column_name(label::applied_flag) <<" , ";
	//data
	query_string << Dam_Eco_Soil_Element::elem_table->get_column_name(dam_label::soil_type) <<" , ";
	query_string << Dam_Eco_Soil_Element::elem_table->get_column_name(dam_label::erosion_type) <<" , ";
	query_string << Dam_Eco_Soil_Element::elem_table->get_column_name(dam_label::elem_mid_x) <<" , ";
	query_string << Dam_Eco_Soil_Element::elem_table->get_column_name(dam_label::elem_mid_y) <<" , ";
	query_string << Dam_Eco_Soil_Element::elem_table->get_column_name(dam_label::area_elem) <<" , ";
	query_string << Dam_Eco_Btype_Element::elem_table->get_column_name(dam_label::conn_fp_id) <<" , ";
	query_string << Dam_Eco_Btype_Element::elem_table->get_column_name(dam_label::conn_fp_elem_id) <<" , ";
	query_string << Dam_Eco_Btype_Element::elem_table->get_column_name(dam_label::no_erosion_type) <<" , ";
	query_string << Dam_Eco_Btype_Element::elem_table->get_column_name(dam_label::no_soil_type) <<" , ";

	query_string << Dam_Eco_Soil_Element::elem_table->get_column_name(dam_label::elem_poly) <<" ) ";

	query_string << " VALUES ( ";
	query_string << id_glob << " , " ;
	query_string << this->elem_index<< " , " ;
	query_string << raster_index<< " , " ;
	query_string << this->system_id.area_state << " , " ;
	query_string << this->system_id.measure_nr << " , " ;
	query_string << "true" << " , " ;
	//data
	query_string << this->id_soil_type<< " , " ;
	query_string << this->id_erosion_type<< " , " ;
	query_string << FORMAT_FIXED_REAL << P(8);
	query_string << this->mid_point.get_xcoordinate() << " , " ;
	query_string << this->mid_point.get_ycoordinate() << " , " ;
	query_string << this->area << " , " ;

	query_string << this->index_fp << " , " ;
	query_string << this->index_fp_elem << " , " ;
	query_string << ""<<functions::convert_boolean2string(this->no_info_erosion_type) << " , " ;
	query_string << ""<<functions::convert_boolean2string(this->no_info_soil_type) << " , " ;

	if(this->polygon_string==label::not_set){
		//set the polgon points
		Geo_Raster_Polygon poly_buff;
		poly_buff.set_points(&this->mid_point, cellsize, cellsize, 0.0, x_origin, y_origin);
		this->polygon_string=poly_buff.get_polygon2sql_string();
		query_string << this->polygon_string <<" ) ";
	}
	else{
		query_string << this->polygon_string <<" ) ";
	}

	Data_Base::database_request(&model, query_string.str(), ptr_database);

	if(model.lastError().isValid()){
		Warning msg=this->set_warning(0);
		ostringstream info;
		info << "Table Name                : " << Dam_Eco_Soil_Element::elem_table->get_table_name() << endl;
		info << "Table error info          : " << model.lastError().text().toStdString() << endl;
		info << "Element number            : " << this->elem_index << endl;
		msg.make_second_info(info.str());
		msg.output_msg(4);
	}
}
//Get the data-string to complete a insert-string for inserting the data of the element to database
string Dam_Eco_Soil_Element::get_datastring_members2database(const int global_id, const int raster_index, const double cellsize, const double x_origin, const double y_origin){
	string buffer;//=label::not_set;
	//set the query via a query string
	ostringstream query_string;
	query_string << " ( ";
	query_string << global_id << " , " ;
	query_string << this->elem_index<< " , " ;
	query_string << raster_index<< " , " ;
	query_string << this->system_id.area_state << " , " ;
	query_string << this->system_id.measure_nr << " , " ;
	query_string << "true" << " , " ;
	//data
	query_string << this->id_soil_type<< " , " ;
	query_string << functions::convert_boolean2string(this->no_info_soil_type)<< " , " ;
	query_string << this->id_erosion_type<< " , " ;
	query_string << functions::convert_boolean2string(this->no_info_erosion_type)<< " , " ;
	query_string << FORMAT_FIXED_REAL << P(8);
	query_string << this->mid_point.get_xcoordinate() << " , " ;
	query_string << this->mid_point.get_ycoordinate() << " , " ;
	query_string.clear();
	query_string << this->area << " , " ;

	if(this->polygon_string==label::not_set){
		//set the polgon points
//		Geo_Raster_Polygon poly_buff;
//		poly_buff.set_points(&this->mid_point, cellsize, cellsize, 0.0, x_origin, y_origin);
//		this->polygon_string=poly_buff.get_polygon2sql_string();

        this->polygon_string=this->get_elem_ploy_string(this->mid_point.get_xcoordinate(),this->mid_point.get_ycoordinate(),cellsize*0.5,cellsize*0.5);

	}

	query_string << this->polygon_string ;
	query_string <<" ) ";

	this->polygon_string=label::not_set;
	buffer=query_string.str();

	return buffer;
}
//Input the soil-erosion raster element data per database
void Dam_Eco_Soil_Element::input_element_perdatabase(const QSqlQueryModel *results, const int glob_index){
	try{
		this->elem_index=results->record(glob_index).value((Dam_Eco_Soil_Element::elem_table->get_column_name(dam_label::elem_id)).c_str()).toInt();
		//set the element members
		this->id_soil_type=results->record(glob_index).value((Dam_Eco_Soil_Element::elem_table->get_column_name(dam_label::soil_type)).c_str()).toInt();
		this->id_erosion_type=results->record(glob_index).value((Dam_Eco_Soil_Element::elem_table->get_column_name(dam_label::erosion_type)).c_str()).toInt();
		this->index_fp=results->record(glob_index).value((Dam_Eco_Soil_Element::elem_table->get_column_name(dam_label::conn_fp_id)).c_str()).toInt();
		this->index_fp_elem=results->record(glob_index).value((Dam_Eco_Soil_Element::elem_table->get_column_name(dam_label::conn_fp_elem_id)).c_str()).toInt();
		this->no_info_erosion_type=results->record(glob_index).value((Dam_Eco_Soil_Element::elem_table->get_column_name(dam_label::no_erosion_type)).c_str()).toBool();
		this->no_info_soil_type=results->record(glob_index).value((Dam_Eco_Soil_Element::elem_table->get_column_name(dam_label::no_soil_type)).c_str()).toBool();
		double x_mid=0.0;
		double y_mid=0.0;
		x_mid=results->record(glob_index).value((Dam_Eco_Soil_Element::elem_table->get_column_name(dam_label::elem_mid_x)).c_str()).toDouble();
		y_mid=results->record(glob_index).value((Dam_Eco_Soil_Element::elem_table->get_column_name(dam_label::elem_mid_y)).c_str()).toDouble();
		this->mid_point.set_point_coordinate(x_mid, y_mid);
		this->area=results->record(glob_index).value((Dam_Eco_Soil_Element::elem_table->get_column_name(dam_label::area_elem)).c_str()).toDouble();
		this->global_index=results->record(glob_index).value((Dam_Eco_Soil_Element::elem_table->get_column_name(dam_label::glob_id)).c_str()).toInt();
	}
	catch(Error msg){
		ostringstream info;
		info << "Element index            : " << this->elem_index << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
}
//Set the element data from the raster interception to the database table: identifier of the floodplain, -floodplain element and the reduced area
void Dam_Eco_Soil_Element::set_interception_elem_data2database(QSqlDatabase *ptr_database, QSqlQuery *elem_results, const int raster_id){
	//nothing to update
	if(this->area!=0.0 && this->index_fp<0 && this->index_fp_elem<0){
		return;
	}
	try{
		Dam_Eco_Soil_Element::set_elem_table(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}

	//generate the filter
	ostringstream filter;
	filter << "UPDATE " << Dam_Eco_Soil_Element::elem_table->get_table_name().c_str();
	filter << " SET ";
	filter << Dam_Eco_Soil_Element::elem_table->get_column_name(dam_label::conn_fp_id).c_str() << " = " << this->index_fp << " , ";
	filter << Dam_Eco_Soil_Element::elem_table->get_column_name(dam_label::conn_fp_elem_id).c_str() << " = " << this->index_fp_elem << " , ";
	filter << Dam_Eco_Soil_Element::elem_table->get_column_name(dam_label::area_elem).c_str() << " = " << this->area;
	filter << " WHERE " ;
	filter << Dam_Eco_Soil_Element::elem_table->get_column_name(dam_label::glob_id) << " = " << this->global_index;

	Data_Base::database_request(elem_results, filter.str(), ptr_database);
	if(elem_results->lastError().isValid()){
		Warning msg=this->set_warning(1);
		ostringstream info;
		info << "Table Name      : " << Dam_Eco_Soil_Element::elem_table->get_table_name() << endl;
		info << "Table error info: " << elem_results->lastError().text().toStdString() << endl;
		info << "Raster-id       : "  << raster_id <<endl;
		info << "Element-id      : " << this->elem_index << endl;
		msg.make_second_info(info.str());
		msg.output_msg(4);
	}
}
//Get a string to set the element data from the raster interception to the database table: identifier of the floodplain, -floodplain element and the reduced area (static)
bool Dam_Eco_Soil_Element::get_string_interception_elem_data2database(ostringstream *text){
//nothing to update
	if(this->area!=0.0 && this->index_fp<0 && this->index_fp_elem<0){
		return false;
	}

	//generate the filter

	*text << "UPDATE " << Dam_Eco_Soil_Element::elem_table->get_table_name().c_str();
	*text << " SET ";
	*text << Dam_Eco_Soil_Element::elem_table->get_column_name(dam_label::conn_fp_id).c_str() << " = " << this->index_fp << " , ";
	*text << Dam_Eco_Soil_Element::elem_table->get_column_name(dam_label::conn_fp_elem_id).c_str() << " = " << this->index_fp_elem << " , ";
	*text << Dam_Eco_Soil_Element::elem_table->get_column_name(dam_label::area_elem).c_str() << " = " << this->area;
	*text << " WHERE " ;
	*text << Dam_Eco_Soil_Element::elem_table->get_column_name(dam_label::glob_id) << " = " << this->global_index<<"; ";

	return true;
}
//Set the index of the soil type
void Dam_Eco_Soil_Element::set_index_soil_type(const int index){
	this->id_soil_type=index;
}
//Set the index of the erosion type
void Dam_Eco_Soil_Element::set_index_ero_type(const int index){
	this->id_erosion_type=index;
}
//Get the index of the soil type
int Dam_Eco_Soil_Element::get_id_soil_type(void){
	return this->id_soil_type;
}
//Get the index of the erosion type
int Dam_Eco_Soil_Element::get_id_erosion_type(void){
	return this->id_erosion_type;
}
//Output the soil members
void Dam_Eco_Soil_Element::output_soil_members(ostringstream *txt){
	*txt << this->id_soil_type << W(13) << this->id_erosion_type;
}
//Connect the element to the soil type
bool Dam_Eco_Soil_Element::connect_soiltype2damage_function(const int no_info, Dam_Eco_Soil_Cost_Function *function){
	bool flag=false;

	if(this->no_info_soil_type==true){
		flag=true;
	}
	else if(this->id_soil_type==no_info){
		this->no_info_soil_type=true;
		flag=true;
	}
	else if(this->id_soil_type==function->get_index()){
		this->ptr_soil_type=function;
		flag=true;
	}
	return flag;
}
//Connect the element to the erosion
bool Dam_Eco_Soil_Element::connect_erotype2damage_function(const int no_info, Dam_Eco_Soil_Erosion_Function *function){
	bool flag=false;

	if(this->no_info_erosion_type==true){
		flag=true;
	}
	else if(this->id_erosion_type==no_info){
		this->no_info_erosion_type=true;
		flag=true;
	}
	else if(this->id_erosion_type==function->get_index()){
		this->ptr_erosion_type=function;
		flag=true;
	}
	return flag;
}
//Get a flag if the soil type is found
bool Dam_Eco_Soil_Element::soil_type_is_found(void){
	if(this->no_info_soil_type==true){
		return true;
	}
	if(this->ptr_soil_type!=NULL){
		return true;
	}
	return false;
}
//Get a flag if the erosion type is found
bool Dam_Eco_Soil_Element::erotype_is_found(void){
	if(this->no_info_erosion_type==true){
		return true;
	}
	if(this->ptr_erosion_type!=NULL){
		return true;
	}
	return false;
}
//Set the element to a no-info element concerning soil type
void Dam_Eco_Soil_Element::set_noinfo_soiltype(void){
	this->no_info_soil_type=true;
}
//Set the element to a no-info element concerning the erosion type
void Dam_Eco_Soil_Element::set_noinfo_erotype(void){
	this->no_info_erosion_type=true;
}
//Get the result of the damage calculation
double Dam_Eco_Soil_Element::get_damage_result(void){
	return this->result_dam;
}
//Calculate the economical damages per element
void Dam_Eco_Soil_Element::calculate_damages(Dam_Impact_Values *impact){
	double soil_ero_zone_double=0.0;
	this->soil_erosion_zone=0;
	this->result_dam=0.0;

	if(impact->get_was_wet_flag()==false){
		return;
	}

	if(this->no_info_erosion_type==false && this->no_info_soil_type==false){
		soil_ero_zone_double=this->ptr_erosion_type->get_y_value(impact->get_impact_v_tot());
		this->result_dam=this->ptr_soil_type->get_y_value(soil_ero_zone_double);
		this->result_dam=this->result_dam*this->area;
		this->soil_erosion_zone=(int)soil_ero_zone_double;
	}
}
//Output the result memebrs to database
void Dam_Eco_Soil_Element::output_result_members2database(QSqlDatabase *ptr_database, const int bound_sz, const string break_sz, const int raster_id, const double cellsize, const double x_origin, const double y_origin){
	if(this->result_dam<constant::zero_epsilon){
		return;
	}
	//mysql query with the query-class
	QSqlQuery model(*ptr_database);
	//the table is set (the name and the column names) and allocated
	try{
		Dam_Eco_Soil_Element::set_erg_table(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}

	//evaluate the global identifier
	int id_glob=Dam_Eco_Soil_Element::elem_erg_table->maximum_int_of_column(Dam_Eco_Soil_Element::elem_erg_table->get_column_name(dam_label::glob_id),ptr_database)+1;

	//set the query via a query string
	ostringstream query_string;
	query_string << "INSERT INTO  " << Dam_Eco_Soil_Element::elem_erg_table->get_table_name();
	query_string <<" ( ";
	query_string << Dam_Eco_Soil_Element::elem_erg_table->get_column_name(dam_label::glob_id) <<" , ";
	query_string << Dam_Eco_Soil_Element::elem_erg_table->get_column_name(dam_label::elem_id) <<" , ";
	query_string << Dam_Eco_Soil_Element::elem_erg_table->get_column_name(dam_label::raster_id) <<" , ";
	query_string << Dam_Eco_Soil_Element::elem_erg_table->get_column_name(label::areastate_id) <<" , ";
	query_string << Dam_Eco_Soil_Element::elem_erg_table->get_column_name(label::measure_id) <<" , ";
	query_string << Dam_Eco_Soil_Element::elem_erg_table->get_column_name(hyd_label::sz_bound_id) <<" , ";
	query_string << Dam_Eco_Soil_Element::elem_erg_table->get_column_name(risk_label::sz_break_id) <<" , ";
	//results
	query_string << Dam_Eco_Soil_Element::elem_erg_table->get_column_name(dam_label::soil_cost) <<" , ";
	query_string << Dam_Eco_Soil_Element::elem_erg_table->get_column_name(dam_label::soil_erosion_zone) <<" , ";

	query_string << Dam_Eco_Soil_Element::elem_erg_table->get_column_name(dam_label::elem_poly) <<" ) ";

	query_string << " VALUES ( ";
	query_string << id_glob << " , " ;
	query_string << this->elem_index<< " , " ;
	query_string << raster_id<< " , " ;
	query_string << this->system_id.area_state << " , " ;
	query_string << this->system_id.measure_nr << " , " ;
	query_string << bound_sz << " , " ;
	query_string << "'"<<break_sz<< "' , " ;
	//results
	query_string << this->result_dam << " , " ;
	query_string << this->soil_erosion_zone << " , " ;

	if(this->polygon_string==label::not_set){
		//set the polgon points
		Geo_Raster_Polygon poly_buff;
		poly_buff.set_points(&this->mid_point, cellsize, cellsize, 0.0, x_origin, y_origin);
		this->polygon_string=poly_buff.get_polygon2sql_string();
		query_string << this->polygon_string <<" ) ";
	}
	else{
		query_string << this->polygon_string <<" ) ";
	}

	Data_Base::database_request(&model, query_string.str(), ptr_database);

	if(model.lastError().isValid()){
		Warning msg=this->set_warning(2);
		ostringstream info;
		info << "Table Name                : " << Dam_Eco_Soil_Element::elem_erg_table->get_table_name() << endl;
		info << "Table error info          : " << model.lastError().text().toStdString() << endl;
		info << "Element number            : " << this->elem_index << endl;
		msg.make_second_info(info.str());
		msg.output_msg(4);
	}
}
//Get the data-string to complete a insert-string for inserting the data of the element results to database
string Dam_Eco_Soil_Element::get_datastring_results2database(const int global_id, const int bound_sz, const string break_sz, const int raster_id, const double cellsize, const double x_origin, const double y_origin, const bool must_output){
	string buffer=label::not_set;
	//no result is written to database
	if(must_output==false){
		if(this->result_dam<constant::zero_epsilon){
			return buffer;
		}
	}
	ostringstream query_string;
	query_string << " ( ";
	query_string << global_id << " , " ;
	query_string << this->elem_index<< " , " ;
	query_string << raster_id<< " , " ;
	query_string << this->system_id.area_state << " , " ;
	query_string << this->system_id.measure_nr << " , " ;
	query_string << bound_sz << " , " ;
	query_string << "'"<<break_sz<< "' , " ;
	//results
	query_string << this->result_dam << " , " ;
	query_string << this->soil_erosion_zone << " , " ;

	if(this->polygon_string==label::not_set){
		//set the polgon points
		Geo_Raster_Polygon poly_buff;
		poly_buff.set_points(&this->mid_point, cellsize, cellsize, 0.0, x_origin, y_origin);
		this->polygon_string=poly_buff.get_polygon2sql_string();
		query_string << this->polygon_string <<" ) ";
	}
	else{
		query_string << this->polygon_string <<" ) ";
	}

	buffer=query_string.str();

	return buffer;
}
//Create the database table for the raster elements (static)
void Dam_Eco_Soil_Element::create_elem_table(QSqlDatabase *ptr_database){
	if(Dam_Eco_Soil_Element::elem_table==NULL){
			ostringstream cout;
			cout << "Create ecological soil-erosion raster element data database table..." << endl ;
			Sys_Common_Output::output_dam->output_txt(&cout);
			//make specific input for this class
			const string tab_name=dam_label::tab_eco_soil_element;
			const int num_col=16;
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

			tab_col[1].name=dam_label::elem_id;
			tab_col[1].type=sys_label::tab_col_type_int;
			tab_col[1].unsigned_flag=true;
			tab_col[1].key_flag=true;

			tab_col[2].name=dam_label::raster_id;
			tab_col[2].type=sys_label::tab_col_type_int;
			tab_col[2].unsigned_flag=true;
			tab_col[2].key_flag=true;

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

			tab_col[6].name=dam_label::soil_type;
			tab_col[6].type=sys_label::tab_col_type_int;
			tab_col[6].default_value="-1";

			tab_col[7].name=dam_label::no_soil_type;
			tab_col[7].type=sys_label::tab_col_type_bool;
			tab_col[7].default_value="false";

			tab_col[8].name=dam_label::erosion_type;
			tab_col[8].type=sys_label::tab_col_type_int;
			tab_col[8].default_value="-1";

			tab_col[9].name=dam_label::no_erosion_type;
			tab_col[9].type=sys_label::tab_col_type_bool;
			tab_col[9].default_value="false";

			tab_col[10].name=dam_label::elem_mid_x;
			tab_col[10].type=sys_label::tab_col_type_double;

			tab_col[11].name=dam_label::elem_mid_y;
			tab_col[11].type=sys_label::tab_col_type_double;

			tab_col[12].name=dam_label::elem_poly;
			tab_col[12].type=sys_label::tab_col_type_polygon;

			tab_col[13].name=dam_label::conn_fp_id;
			tab_col[13].type=sys_label::tab_col_type_int;
			tab_col[13].default_value="-1";

			tab_col[14].name=dam_label::conn_fp_elem_id;
			tab_col[14].type=sys_label::tab_col_type_int;
			tab_col[14].default_value="-1";

			tab_col[15].name=dam_label::area_elem;
			tab_col[15].type=sys_label::tab_col_type_double;
			tab_col[15].default_value="0.0";

			try{
				Dam_Eco_Soil_Element::elem_table= new Tables();
				if(Dam_Eco_Soil_Element::elem_table->create_non_existing_tables(tab_name, tab_col, num_col,ptr_database, _sys_table_type::dam)==false){
					cout << " Table exists" << endl ;
					Sys_Common_Output::output_dam->output_txt(&cout);
				};
			}
			catch(bad_alloc& t){
				Error msg;
				msg.set_msg("Dam_Eco_Soil_Element::create_elem_table(QSqlDatabase *ptr_database)","Can not allocate the memory", "Check the memory", 10, false);
				ostringstream info;
				info<< "Info bad alloc: " << t.what() << endl;
				msg.make_second_info(info.str());
				throw msg;
			}
			catch(Error msg){
				Dam_Eco_Soil_Element::close_elem_table();
				throw msg;
			}

		Dam_Eco_Soil_Element::close_elem_table();
	}
}
//Set the database table for the raster elements: it sets the table name and the name of the columns and allocate them (static)
void Dam_Eco_Soil_Element::set_elem_table(QSqlDatabase *ptr_database){
	if(Dam_Eco_Soil_Element::elem_table==NULL){
		//make specific input for this class
		const string tab_id_name=dam_label::tab_eco_soil_element;
		string tab_col[16];

		tab_col[0]=dam_label::glob_id;
		tab_col[1]=dam_label::elem_id;
		tab_col[2]=dam_label::raster_id;
		tab_col[3]=label::areastate_id;
		tab_col[4]=label::measure_id;
		tab_col[5]=dam_label::soil_type;
		tab_col[6]=dam_label::erosion_type;
		tab_col[7]=dam_label::elem_mid_x;
		tab_col[8]=dam_label::elem_mid_y;
		tab_col[9]=dam_label::elem_poly;
		tab_col[10]=dam_label::no_erosion_type;
		tab_col[11]=dam_label::no_soil_type;
		tab_col[12]=dam_label::conn_fp_id;
		tab_col[13]=dam_label::conn_fp_elem_id;
		tab_col[14]=label::applied_flag;
		tab_col[15]=dam_label::area_elem;

		try{
			Dam_Eco_Soil_Element::elem_table= new Tables(tab_id_name, tab_col, sizeof(tab_col)/sizeof(tab_col[0]));
			Dam_Eco_Soil_Element::elem_table->set_name(ptr_database, _sys_table_type::dam);
		}
		catch(bad_alloc& t){
			Error msg;
			msg.set_msg("Dam_Eco_Soil_Element::set_elem_table(QSqlDatabase *ptr_database)","Can not allocate the memory", "Check the memory", 10, false);
			ostringstream info;
			info<< "Info bad alloc: " << t.what() << endl;
			msg.make_second_info(info.str());
			throw msg;
		}
		catch(Error msg){
			Dam_Eco_Soil_Element::close_elem_table();
			throw msg;
		}
	}
}
//Delete all data in the database table for the raster elements (static)
void Dam_Eco_Soil_Element::delete_data_in_elem_table(QSqlDatabase *ptr_database){
	//the table is set (the name and the column names) and allocated
	try{
		Dam_Eco_Soil_Element::set_elem_table(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}
	//delete the table
	Dam_Eco_Soil_Element::elem_table->delete_data_in_table(ptr_database);
}
//Close and delete the database table for the raster elements (static)
void Dam_Eco_Soil_Element::close_elem_table(void){
	if(Dam_Eco_Soil_Element::elem_table!=NULL){
		delete Dam_Eco_Soil_Element::elem_table;
		Dam_Eco_Soil_Element::elem_table=NULL;
	}
}
//Get the maximum value of the global index of the element database table (static)
int Dam_Eco_Soil_Element::get_max_glob_id_elem_table(QSqlDatabase *ptr_database){
	//the table is set (the name and the column names) and allocated
	try{
		Dam_Eco_Soil_Element::set_elem_table(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}

	//evaluate the global identifier
	int id_glob=Dam_Eco_Soil_Element::elem_table->maximum_int_of_column(Dam_Eco_Soil_Element::elem_table->get_column_name(dam_label::glob_id),ptr_database);
	return id_glob;
}
//Get the header for inserting the element data to database table (static)
string Dam_Eco_Soil_Element::get_insert_header_elem_table(QSqlDatabase *ptr_database){
	string buffer;
	//the table is set (the name and the column names) and allocated
	try{
		Dam_Eco_Soil_Element::set_elem_table(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}

	//set the query via a query string
	ostringstream query_string;
	query_string << "INSERT INTO  " << Dam_Eco_Soil_Element::elem_table->get_table_name();
	query_string <<" ( ";
	query_string << Dam_Eco_Soil_Element::elem_table->get_column_name(dam_label::glob_id) <<" , ";
	query_string << Dam_Eco_Soil_Element::elem_table->get_column_name(dam_label::elem_id) <<" , ";
	query_string << Dam_Eco_Soil_Element::elem_table->get_column_name(dam_label::raster_id) <<" , ";
	query_string << Dam_Eco_Soil_Element::elem_table->get_column_name(label::areastate_id) <<" , ";
	query_string << Dam_Eco_Soil_Element::elem_table->get_column_name(label::measure_id) <<" , ";
	query_string << Dam_Eco_Soil_Element::elem_table->get_column_name(label::applied_flag) <<" , ";
	//data
	query_string << Dam_Eco_Soil_Element::elem_table->get_column_name(dam_label::soil_type) <<" , ";
	query_string << Dam_Eco_Soil_Element::elem_table->get_column_name(dam_label::no_soil_type) <<" , ";
	query_string << Dam_Eco_Soil_Element::elem_table->get_column_name(dam_label::erosion_type) <<" , ";
	query_string << Dam_Eco_Soil_Element::elem_table->get_column_name(dam_label::no_erosion_type) <<" , ";
	query_string << Dam_Eco_Soil_Element::elem_table->get_column_name(dam_label::elem_mid_x) <<" , ";
	query_string << Dam_Eco_Soil_Element::elem_table->get_column_name(dam_label::elem_mid_y) <<" , ";
	query_string << Dam_Eco_Soil_Element::elem_table->get_column_name(dam_label::area_elem) <<" , ";

	query_string << Dam_Eco_Soil_Element::elem_table->get_column_name(dam_label::elem_poly) <<" ) ";

	query_string << " VALUES ";

	buffer=query_string.str();

	return buffer;
}
//Delete all data in the database table for the raster elements of the given raster index (static)
void Dam_Eco_Soil_Element::delete_data_in_elem_table(QSqlDatabase *ptr_database, const int raster_id){
	QSqlQueryModel results;
	ostringstream test_filter;
	try{
		Dam_Eco_Soil_Element::set_elem_table(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}
	//delete
	test_filter<< "DELETE ";
	test_filter << " FROM " << Dam_Eco_Soil_Element::elem_table->get_table_name();
	test_filter << " WHERE ";
	test_filter << Dam_Eco_Soil_Element::elem_table->get_column_name(dam_label::raster_id) << " = " << raster_id;

	Data_Base::database_request(&results, test_filter.str(), ptr_database);

	//check the request
	if(results.lastError().isValid()){
		Error msg;
		msg.set_msg("Dam_Eco_Soil_Element::delete_data_in_elem_table(QSqlDatabase *ptr_database, const int raster_id)","Invalid database request", "Check the database", 2, false);
		ostringstream info;
		info << "Table Name      : " << Dam_Eco_Soil_Element::elem_table->get_table_name() << endl;
		info << "Table error info: " << results.lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
}
//Delete all data in the database table for the raster elements of the given system id (static)
void Dam_Eco_Soil_Element::delete_data_in_elem_table(QSqlDatabase *ptr_database, const _sys_system_id id){
	QSqlQueryModel results;
	ostringstream test_filter;
	try{
		Dam_Eco_Soil_Element::set_elem_table(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}
	//delete
	test_filter<< "DELETE ";
	test_filter << " FROM " << Dam_Eco_Soil_Element::elem_table->get_table_name();
	test_filter << " WHERE ";
	test_filter << Dam_Eco_Soil_Element::elem_table->get_column_name(label::areastate_id) << " = " << id.area_state;
	test_filter << " AND ";
	test_filter << Dam_Eco_Soil_Element::elem_table->get_column_name(label::measure_id) << " = " << id.measure_nr;

	Data_Base::database_request(&results, test_filter.str(), ptr_database);

	//check the request
	if(results.lastError().isValid()){
		Error msg;
		msg.set_msg("Dam_Eco_Soil_Element::delete_data_in_elem_table(QSqlDatabase *ptr_database, const _sys_system_id id))","Invalid database request", "Check the database", 2, false);
		ostringstream info;
		info << "Table Name      : " << Dam_Eco_Soil_Element::elem_table->get_table_name() << endl;
		info << "Table error info: " << results.lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
}
//Create the database table for the results of the damage calculation per element (static)
void Dam_Eco_Soil_Element::create_erg_table(QSqlDatabase *ptr_database){
		if(Dam_Eco_Soil_Element::elem_erg_table==NULL){
			ostringstream cout;
			cout << "Create ecological soil-erosion raster element result database table..." << endl ;
			Sys_Common_Output::output_dam->output_txt(&cout);
			//make specific input for this class
			const string tab_name=dam_label::tab_eco_soil_erg;
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

			tab_col[1].name=dam_label::elem_id;
			tab_col[1].type=sys_label::tab_col_type_int;
			tab_col[1].unsigned_flag=true;
			tab_col[1].key_flag=true;

			tab_col[2].name=dam_label::raster_id;
			tab_col[2].type=sys_label::tab_col_type_int;
			tab_col[2].unsigned_flag=true;
			tab_col[2].key_flag=true;

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

			tab_col[8].name=dam_label::soil_cost;
			tab_col[8].type=sys_label::tab_col_type_double;
			tab_col[8].default_value="0.0";
			tab_col[8].unsigned_flag=true;

			tab_col[9].name=dam_label::soil_erosion_zone;
			tab_col[9].type=sys_label::tab_col_type_int;
			tab_col[9].unsigned_flag=true;
			tab_col[9].default_value="0";

			tab_col[10].name=dam_label::elem_poly;
			tab_col[10].type=sys_label::tab_col_type_polygon;

			try{
				Dam_Eco_Soil_Element::elem_erg_table= new Tables();
				if(Dam_Eco_Soil_Element::elem_erg_table->create_non_existing_tables(tab_name, tab_col, num_col,ptr_database, _sys_table_type::dam)==false){
					cout << " Table exists" << endl ;
					Sys_Common_Output::output_dam->output_txt(&cout);
				};
			}
			catch(bad_alloc& t){
				Error msg;
				msg.set_msg("Dam_Eco_Soil_Element::create_erg_table(QSqlDatabase *ptr_database)","Can not allocate the memory", "Check the memory", 10, false);
				ostringstream info;
				info<< "Info bad alloc: " << t.what() << endl;
				msg.make_second_info(info.str());
				throw msg;
			}
			catch(Error msg){
				Dam_Eco_Soil_Element::close_erg_table();
				throw msg;
			}
		Dam_Eco_Soil_Element::close_erg_table();
	}
}
//Set the database table for the results of the damage calculation per element: it sets the table name and the name of the columns and allocate them (static)
void Dam_Eco_Soil_Element::set_erg_table(QSqlDatabase *ptr_database){
	if(Dam_Eco_Soil_Element::elem_erg_table==NULL){
		//make specific input for this class
		const string tab_id_name=dam_label::tab_eco_soil_erg;
		string tab_col[11];

		tab_col[0]=dam_label::glob_id;
		tab_col[1]=dam_label::elem_id;
		tab_col[2]=dam_label::raster_id;
		tab_col[3]=label::areastate_id;
		tab_col[4]=label::measure_id;
		tab_col[5]=hyd_label::sz_bound_id;
		tab_col[6]=risk_label::sz_break_id;
		tab_col[7]=dam_label::soil_cost;
		tab_col[8]=dam_label::soil_erosion_zone;
		tab_col[9]=dam_label::elem_poly;
		tab_col[10]=label::applied_flag;

		try{
			Dam_Eco_Soil_Element::elem_erg_table= new Tables(tab_id_name, tab_col, sizeof(tab_col)/sizeof(tab_col[0]));
			Dam_Eco_Soil_Element::elem_erg_table->set_name(ptr_database, _sys_table_type::dam);
		}
		catch(bad_alloc& t){
			Error msg;
			msg.set_msg("Dam_Eco_Soil_Element::set_erg_table(QSqlDatabase *ptr_database)","Can not allocate the memory", "Check the memory", 10, false);
			ostringstream info;
			info<< "Info bad alloc: " << t.what() << endl;
			msg.make_second_info(info.str());
			throw msg;
		}
		catch(Error msg){
			Dam_Eco_Soil_Element::close_erg_table();
			throw msg;
		}
	}
}
//Delete all data in the database table for the results of the damage calculation per element (static)
void Dam_Eco_Soil_Element::delete_data_in_erg_table(QSqlDatabase *ptr_database){
	//the table is set (the name and the column names) and allocated
	try{
		Dam_Eco_Soil_Element::set_erg_table(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}
	//delete the table
	Dam_Eco_Soil_Element::elem_erg_table->delete_data_in_table(ptr_database);
}
//Delete the data in the database table for the results of the damage calculation specified by the system state (static)
void Dam_Eco_Soil_Element::delete_data_in_erg_table(QSqlDatabase *ptr_database, const _sys_system_id id){
	try{
		Dam_Eco_Soil_Element::set_erg_table(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}

	//delete the table
	QSqlQuery query(*ptr_database);
	ostringstream query_string;
	query_string <<"DELETE FROM " << Dam_Eco_Soil_Element::elem_erg_table->get_table_name();
	query_string << " WHERE " ;
	query_string << Dam_Eco_Soil_Element::elem_erg_table->get_column_name(label::areastate_id) << " = " << id.area_state;
	query_string << " AND ";
	query_string << Dam_Eco_Soil_Element::elem_erg_table->get_column_name(label::measure_id) << " = " << id.measure_nr;

	Data_Base::database_request(&query, query_string.str(), ptr_database);
	if(query.lastError().isValid()==true){
		Error msg;
		msg.set_msg("Dam_Eco_Soil_Element::delete_data_in_erg_table(QSqlDatabase *ptr_database, const _sys_system_id id)","Invalid database request", "Check the database", 2, false);
		ostringstream info;
		info << "Table Name      : " << Dam_Eco_Soil_Element::elem_erg_table->get_table_name() << endl;
		info << "Table error info: " << query.lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
}
//Delete the data in the database table for the results of the damage calculation specified parameters (static)
void Dam_Eco_Soil_Element::delete_data_in_erg_table(QSqlDatabase *ptr_database, const _sys_system_id id, const string break_sz, const bool like_flag){
	try{
		Dam_Eco_Soil_Element::set_erg_table(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}

	//delete the table
	QSqlQuery query(*ptr_database);
	ostringstream query_string;
	query_string <<"DELETE FROM " << Dam_Eco_Soil_Element::elem_erg_table->get_table_name();
	query_string << " WHERE " ;
	query_string << Dam_Eco_Soil_Element::elem_erg_table->get_column_name(label::areastate_id) << " = " << id.area_state;
	query_string << " AND ";
	query_string << Dam_Eco_Soil_Element::elem_erg_table->get_column_name(label::measure_id) << " = " << id.measure_nr;
	query_string << " AND ";
	if(like_flag==false){
		query_string << Dam_Eco_Soil_Element::elem_erg_table->get_column_name(risk_label::sz_break_id) << " = '" << break_sz<<"'";
	}
	else{
		query_string << Dam_Eco_Soil_Element::elem_erg_table->get_column_name(risk_label::sz_break_id) << " LIKE '" << break_sz<<"'";
	}

	Data_Base::database_request(&query, query_string.str(), ptr_database);
	if(query.lastError().isValid()==true){
		Error msg;
		msg.set_msg("Dam_Eco_Soil_Element::delete_data_in_erg_table(QSqlDatabase *ptr_database, const _sys_system_id id, const string break_sz, const bool like_flag)","Invalid database request", "Check the database", 2, false);
		ostringstream info;
		info << "Table Name      : " << Dam_Eco_Soil_Element::elem_erg_table->get_table_name() << endl;
		info << "Table error info: " << query.lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
}
//Delete the data in the database table for the results of the damage calculation specified by the system id and the scenario-ids (static)
void Dam_Eco_Soil_Element::delete_data_in_erg_table(QSqlDatabase *ptr_database, const _sys_system_id id, const int bound_sz, const string break_sz ){
//the table is set (the name and the column names) and allocated
	try{
		Dam_Eco_Soil_Element::set_erg_table(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}

	//delete the table
	QSqlQuery query(*ptr_database);
	ostringstream query_string;
	query_string <<"DELETE FROM " << Dam_Eco_Soil_Element::elem_erg_table->get_table_name();
	query_string << " WHERE " ;
	query_string << Dam_Eco_Soil_Element::elem_erg_table->get_column_name(label::areastate_id) << " = " << id.area_state;
	query_string << " AND ";
	query_string << Dam_Eco_Soil_Element::elem_erg_table->get_column_name(label::measure_id) << " = " << id.measure_nr;
	query_string << " AND ";
	query_string << Dam_Eco_Soil_Element::elem_erg_table->get_column_name(hyd_label::sz_bound_id) << " = " << bound_sz;
	query_string << " AND ";
	query_string << Dam_Eco_Soil_Element::elem_erg_table->get_column_name(risk_label::sz_break_id) << " = '" << break_sz<<"'";

	Data_Base::database_request(&query, query_string.str(), ptr_database);
	if(query.lastError().isValid()==true){
		Error msg;
		msg.set_msg("Dam_Eco_Soil_Element::delete_data_in_erg_table(QSqlDatabase *ptr_database, const _sys_system_id id, const int bound_sz, const string break_sz )","Invalid database request", "Check the database", 2, false);
		ostringstream info;
		info << "Table Name      : " << Dam_Eco_Soil_Element::elem_erg_table->get_table_name() << endl;
		info << "Table error info: " << query.lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
}
//Delete the data in the database table for the results of the damage calculation specified by the hydraulic scenario-ids (static)
void Dam_Eco_Soil_Element::delete_data_in_erg_table(QSqlDatabase *ptr_database, const int bound_sz ){
//the table is set (the name and the column names) and allocated
	try{
		Dam_Eco_Soil_Element::set_erg_table(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}

	//delete the table
	QSqlQuery query(*ptr_database);
	ostringstream query_string;
	query_string <<"DELETE FROM " << Dam_Eco_Soil_Element::elem_erg_table->get_table_name();
	query_string << " WHERE " ;
	query_string << Dam_Eco_Soil_Element::elem_erg_table->get_column_name(hyd_label::sz_bound_id) << " = " << bound_sz;

	Data_Base::database_request(&query, query_string.str(), ptr_database);
	if(query.lastError().isValid()==true){
		Error msg;
		msg.set_msg("Dam_Eco_Soil_Element::delete_data_in_erg_table(QSqlDatabase *ptr_database, const int bound_sz)","Invalid database request", "Check the database", 2, false);
		ostringstream info;
		info << "Table Name      : " << Dam_Eco_Soil_Element::elem_erg_table->get_table_name() << endl;
		info << "Table error info: " << query.lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
}
//Delete the data in the database table for the results of the damage calculation specified by the raster id (static)
void Dam_Eco_Soil_Element::delete_data_in_erg_table_per_raster(QSqlDatabase *ptr_database, const int raster_id){
	//the table is set (the name and the column names) and allocated
	try{
		Dam_Eco_Soil_Element::set_erg_table(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}

	//delete the table
	QSqlQuery query(*ptr_database);
	ostringstream query_string;
	query_string <<"DELETE FROM " << Dam_Eco_Soil_Element::elem_erg_table->get_table_name();
	query_string << " WHERE " ;
	query_string << Dam_Eco_Soil_Element::elem_erg_table->get_column_name(dam_label::raster_id) << " = " << raster_id;

	Data_Base::database_request(&query, query_string.str(), ptr_database);
	if(query.lastError().isValid()==true){
		Error msg;
		msg.set_msg("Dam_Eco_Soil_Element::delete_data_in_erg_table(QSqlDatabase *ptr_database, const int raster_id)","Invalid database request", "Check the database", 2, false);
		ostringstream info;
		info << "Table Name      : " << Dam_Eco_Soil_Element::elem_erg_table->get_table_name() << endl;
		info << "Table error info: " << query.lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
}
//Close and delete the database table for the results of the damage calculation per element (static)
void Dam_Eco_Soil_Element::close_erg_table(void){
	if(Dam_Eco_Soil_Element::elem_erg_table!=NULL){
		delete Dam_Eco_Soil_Element::elem_erg_table;
		Dam_Eco_Soil_Element::elem_erg_table=NULL;
	}
}
//Select data in the database table for the results of the damage calculation specified by the system id and the scenario-ids (static)
int Dam_Eco_Soil_Element::select_data_in_erg_table(QSqlQueryModel *results, QSqlDatabase *ptr_database, const _sys_system_id id, const int bound_sz, const string break_sz ){
	//the table is set (the name and the column names) and allocated
	try{
		Dam_Eco_Soil_Element::set_erg_table(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}

	ostringstream query_string;
	query_string <<"SELECT ";
	query_string << Dam_Eco_Soil_Element::elem_erg_table->get_column_name(dam_label::elem_id) << " , ";
	query_string << Dam_Eco_Soil_Element::elem_erg_table->get_column_name(dam_label::raster_id) << " , ";
	query_string << Dam_Eco_Soil_Element::elem_erg_table->get_column_name(dam_label::soil_cost) << " , ";
	query_string << " ST_ASTEXT("<<Dam_Eco_Soil_Element::elem_erg_table->get_column_name(dam_label::elem_poly) <<")";

	query_string<<" FROM " << Dam_Eco_Soil_Element::elem_erg_table->get_table_name();
	query_string << " WHERE ";
	query_string << Dam_Eco_Soil_Element::elem_erg_table->get_column_name(label::applied_flag) << "= true";
	query_string << " AND ";
	query_string << Dam_Eco_Soil_Element::elem_erg_table->get_column_name(label::areastate_id) << " ="  << id.area_state;
	query_string << " AND (";
	query_string << Dam_Eco_Soil_Element::elem_erg_table->get_column_name(label::measure_id) << " = " << 0 ;
	query_string << " OR " ;
	query_string << Dam_Eco_Soil_Element::elem_erg_table->get_column_name(label::measure_id) << " = " << id.measure_nr;
	query_string << " ) " ;
	query_string << " AND ";
	query_string << Dam_Eco_Soil_Element::elem_erg_table->get_column_name(hyd_label::sz_bound_id) << " = " << bound_sz;
	query_string << " AND ";
	query_string << Dam_Eco_Soil_Element::elem_erg_table->get_column_name(risk_label::sz_break_id) << " = '" << break_sz<<"'";

	query_string << " ORDER BY ";
	query_string << Dam_Eco_Soil_Element::elem_erg_table->get_column_name(dam_label::raster_id) << " , ";
	query_string << Dam_Eco_Soil_Element::elem_erg_table->get_column_name(dam_label::elem_id) << "  ";

	Data_Base::database_request(results, query_string.str(), ptr_database);
	if(results->lastError().isValid()==true){
		Error msg;
		msg.set_msg("Dam_Eco_Soil_Element::select_data_in_erg_table(QSqlDatabase *ptr_database, const _sys_system_id id, const int bound_sz, const string break_sz )","Invalid database request", "Check the database", 2, false);
		ostringstream info;
		info << "Table Name      : " << Dam_Eco_Soil_Element::elem_erg_table->get_table_name() << endl;
		info << "Table error info: " << results->lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}

	return results->rowCount();
}
//Select data in the database table for the results of the damage calculation specified by the system id and the scenario-ids (just part of it) (static)
int Dam_Eco_Soil_Element::select_data_in_erg_table(QSqlQueryModel *results, QSqlDatabase *ptr_database, const _sys_system_id id, const int bound_sz, const string break_sz, const int offset, const int number_row ){
		//the table is set (the name and the column names) and allocated
	try{
		Dam_Eco_Soil_Element::set_erg_table(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}

	ostringstream query_string;
	query_string <<"SELECT ";
	query_string << Dam_Eco_Soil_Element::elem_erg_table->get_column_name(dam_label::elem_id) << " , ";
	query_string << Dam_Eco_Soil_Element::elem_erg_table->get_column_name(dam_label::raster_id) << " , ";
	query_string << Dam_Eco_Soil_Element::elem_erg_table->get_column_name(dam_label::soil_cost) << " , ";
	query_string << " ST_ASTEXT("<<Dam_Eco_Soil_Element::elem_erg_table->get_column_name(dam_label::elem_poly) <<")";

	query_string<<" FROM " << Dam_Eco_Soil_Element::elem_erg_table->get_table_name();
	query_string << " WHERE ";
	query_string << Dam_Eco_Soil_Element::elem_erg_table->get_column_name(label::applied_flag) << "= true";
	query_string << " AND ";
	query_string << Dam_Eco_Soil_Element::elem_erg_table->get_column_name(label::areastate_id) << " ="  << id.area_state;
	query_string << " AND (";
	query_string << Dam_Eco_Soil_Element::elem_erg_table->get_column_name(label::measure_id) << " = " << 0 ;
	query_string << " OR " ;
	query_string << Dam_Eco_Soil_Element::elem_erg_table->get_column_name(label::measure_id) << " = " << id.measure_nr;
	query_string << " ) " ;
	query_string << " AND ";
	query_string << Dam_Eco_Soil_Element::elem_erg_table->get_column_name(hyd_label::sz_bound_id) << " = " << bound_sz;
	query_string << " AND ";
	query_string << Dam_Eco_Soil_Element::elem_erg_table->get_column_name(risk_label::sz_break_id) << " = '" << break_sz<<"'";

	query_string << " ORDER BY ";
	query_string << Dam_Eco_Soil_Element::elem_erg_table->get_column_name(dam_label::raster_id) << " , ";
	query_string << Dam_Eco_Soil_Element::elem_erg_table->get_column_name(dam_label::elem_id) << "  ";
	query_string << " LIMIT " << number_row;
	query_string << " OFFSET " << offset;

	Data_Base::database_request(results, query_string.str(), ptr_database);
	if(results->lastError().isValid()==true){
		Error msg;
		msg.set_msg("Dam_Eco_Soil_Element::select_data_in_erg_table(QSqlDatabase *ptr_database, const _sys_system_id id, const int bound_sz, const string break_sz, const int offset, const int number_row)","Invalid database request", "Check the database", 2, false);
		ostringstream info;
		info << "Table Name      : " << Dam_Eco_Soil_Element::elem_erg_table->get_table_name() << endl;
		info << "Table error info: " << results->lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}

	return results->rowCount();
}
//Count data in the database table for the results of the damage calculation specified by the system id and the scenario-ids (static)
int Dam_Eco_Soil_Element::count_data_in_erg_table(QSqlQueryModel *results, QSqlDatabase *ptr_database, const _sys_system_id id, const int bound_sz, const string break_sz ){
	//the table is set (the name and the column names) and allocated
	try{
		Dam_Eco_Soil_Element::set_erg_table(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}

	ostringstream query_string;
	query_string <<"SELECT COUNT(";
	query_string << Dam_Eco_Soil_Element::elem_erg_table->get_column_name(dam_label::elem_id) << " ) ";

	query_string<<" FROM " << Dam_Eco_Soil_Element::elem_erg_table->get_table_name();
	query_string << " WHERE ";
	query_string << Dam_Eco_Soil_Element::elem_erg_table->get_column_name(label::applied_flag) << "= true";
	query_string << " AND ";
	query_string << Dam_Eco_Soil_Element::elem_erg_table->get_column_name(label::areastate_id) << " ="  << id.area_state;
	query_string << " AND (";
	query_string << Dam_Eco_Soil_Element::elem_erg_table->get_column_name(label::measure_id) << " = " << 0 ;
	query_string << " OR " ;
	query_string << Dam_Eco_Soil_Element::elem_erg_table->get_column_name(label::measure_id) << " = " << id.measure_nr;
	query_string << " ) " ;
	query_string << " AND ";
	query_string << Dam_Eco_Soil_Element::elem_erg_table->get_column_name(hyd_label::sz_bound_id) << " = " << bound_sz;
	query_string << " AND ";
	query_string << Dam_Eco_Soil_Element::elem_erg_table->get_column_name(risk_label::sz_break_id) << " = '" << break_sz<<"'";

	Data_Base::database_request(results, query_string.str(), ptr_database);
	int number=0;
	if(results->lastError().isValid()==true){
		Error msg;
		msg.set_msg("Dam_Eco_Soil_Element::count_data_in_erg_table(QSqlDatabase *ptr_database, const _sys_system_id id, const int bound_sz, const string break_sz )","Invalid database request", "Check the database", 2, false);
		ostringstream info;
		info << "Table Name      : " << Dam_Eco_Soil_Element::elem_erg_table->get_table_name() << endl;
		info << "Table error info: " << results->lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
	if(results->rowCount()>0){
		number= results->record(0).field(0).value().toInt();
	}

	return number;
}
//Get the maximum value of the global index of the element result database table (static)
int Dam_Eco_Soil_Element::get_max_glob_id_erg_table(QSqlDatabase *ptr_database){
	//the table is set (the name and the column names) and allocated
	try{
		Dam_Eco_Soil_Element::set_erg_table(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}

	//evaluate the global identifier
	int id_glob=Dam_Eco_Soil_Element::elem_erg_table->maximum_int_of_column(Dam_Eco_Soil_Element::elem_erg_table->get_column_name(dam_label::glob_id),ptr_database);
	return id_glob;
}
//Get the header for inserting the element result data to database table (static)
string Dam_Eco_Soil_Element::get_insert_header_erg_table(QSqlDatabase *ptr_database){
	string buffer;
	//the table is set (the name and the column names) and allocated
	try{
		Dam_Eco_Soil_Element::set_erg_table(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}
	//set the query via a query string
	ostringstream query_string;
	query_string << "INSERT INTO  " << Dam_Eco_Soil_Element::elem_erg_table->get_table_name();
	query_string <<" ( ";
	query_string << Dam_Eco_Soil_Element::elem_erg_table->get_column_name(dam_label::glob_id) <<" , ";
	query_string << Dam_Eco_Soil_Element::elem_erg_table->get_column_name(dam_label::elem_id) <<" , ";
	query_string << Dam_Eco_Soil_Element::elem_erg_table->get_column_name(dam_label::raster_id) <<" , ";
	query_string << Dam_Eco_Soil_Element::elem_erg_table->get_column_name(label::areastate_id) <<" , ";
	query_string << Dam_Eco_Soil_Element::elem_erg_table->get_column_name(label::measure_id) <<" , ";
	query_string << Dam_Eco_Soil_Element::elem_erg_table->get_column_name(hyd_label::sz_bound_id) <<" , ";
	query_string << Dam_Eco_Soil_Element::elem_erg_table->get_column_name(risk_label::sz_break_id) <<" , ";
	//results
	query_string << Dam_Eco_Soil_Element::elem_erg_table->get_column_name(dam_label::soil_cost) <<" , ";
	query_string << Dam_Eco_Soil_Element::elem_erg_table->get_column_name(dam_label::soil_erosion_zone) <<" , ";

	query_string << Dam_Eco_Soil_Element::elem_erg_table->get_column_name(dam_label::elem_poly) <<" ) ";

	query_string << " VALUES ";

	buffer=query_string.str();
	return buffer;
}
//Switch the applied-flag for the ecological soil-erosion damage results in the database table for a defined system state (static)
void Dam_Eco_Soil_Element::switch_applied_flag_erg_table(QSqlDatabase *ptr_database, const _sys_system_id id, const bool flag){
//the table is set (the name and the column names) and allocated
	try{
		Dam_Eco_Soil_Element::set_erg_table(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}
	QSqlQueryModel query;

	ostringstream query_string;
	query_string <<"UPDATE ";
	query_string << Dam_Eco_Soil_Element::elem_erg_table->get_table_name();
	query_string << " SET " ;
	query_string  << Dam_Eco_Soil_Element::elem_erg_table->get_column_name(label::applied_flag) << " = "<< functions::convert_boolean2string(flag);
	query_string  << " WHERE ";
	query_string  << Dam_Eco_Soil_Element::elem_erg_table->get_column_name(label::areastate_id) << " = "  << id.area_state;
	query_string  << " AND ";
	query_string  <<  Dam_Eco_Soil_Element::elem_erg_table->get_column_name(label::measure_id) << " = " << id.measure_nr ;
	query_string  << " AND ";
	query_string  << Dam_Eco_Soil_Element::elem_erg_table->get_column_name(label::applied_flag) << " = "<< functions::convert_boolean2string(!flag);

	Data_Base::database_request(&query, query_string.str(), ptr_database);
	if(query.lastError().isValid()==true){
		Error msg;
		msg.set_msg("Dam_Eco_Soil_Element::switch_applied_flag_erg_table(QSqlDatabase *ptr_database, const _sys_system_id id, const bool flag)","Invalid database request", "Check the database", 2, false);
		ostringstream info;
		info << "Table Name      : " << Dam_Eco_Soil_Element::elem_erg_table->get_table_name() << endl;
		info << "Table error info: " << query.lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
}
//Switch the applied-flag for the ecological soil-erosion damage results in the database table for a defined system state (static)
void Dam_Eco_Soil_Element::switch_applied_flag_erg_table(QSqlDatabase *ptr_database, const _sys_system_id id, const int hyd_sc, const bool flag){
	//the table is set (the name and the column names) and allocated
	try{
		Dam_Eco_Soil_Element::set_erg_table(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}
	QSqlQueryModel query;

	ostringstream query_string;
	query_string <<"UPDATE ";
	query_string << Dam_Eco_Soil_Element::elem_erg_table->get_table_name();
	query_string << " SET " ;
	query_string  << Dam_Eco_Soil_Element::elem_erg_table->get_column_name(label::applied_flag) << " = "<< functions::convert_boolean2string(flag);
	query_string  << " WHERE ";
	query_string  << Dam_Eco_Soil_Element::elem_erg_table->get_column_name(label::areastate_id) << " = "  << id.area_state;
	query_string  << " AND ";
	query_string  <<  Dam_Eco_Soil_Element::elem_erg_table->get_column_name(label::measure_id) << " = " << id.measure_nr ;
	query_string  << " AND ";
	query_string  <<  Dam_Eco_Soil_Element::elem_erg_table->get_column_name(hyd_label::sz_bound_id) << " = " << hyd_sc ;
	query_string  << " AND ";
	query_string  << Dam_Eco_Soil_Element::elem_erg_table->get_column_name(label::applied_flag) << " = "<< functions::convert_boolean2string(!flag);

	Data_Base::database_request(&query, query_string.str(), ptr_database);
	if(query.lastError().isValid()==true){
		Error msg;
		msg.set_msg("Dam_Eco_Soil_Element::switch_applied_flag_erg_table(QSqlDatabase *ptr_database, const _sys_system_id id, const int hyd_sc, const bool flag)","Invalid database request", "Check the database", 2, false);
		ostringstream info;
		info << "Table Name      : " << Dam_Eco_Soil_Element::elem_erg_table->get_table_name() << endl;
		info << "Table error info: " << query.lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
}
//Switch the applied-flag for the ecological biotope-type damage elements in the database table for a defined system state (static)
void Dam_Eco_Soil_Element::switch_applied_flag_elem_table(QSqlDatabase *ptr_database, const _sys_system_id id, const bool flag){
//the table is set (the name and the column names) and allocated
	try{
		Dam_Eco_Soil_Element::set_elem_table(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}
	QSqlQueryModel query;

	ostringstream query_string;
	query_string <<"UPDATE ";
	query_string << Dam_Eco_Soil_Element::elem_table->get_table_name();
	query_string << " SET " ;
	query_string  << Dam_Eco_Soil_Element::elem_table->get_column_name(label::applied_flag) << " = "<< functions::convert_boolean2string(flag);
	query_string  << " WHERE ";
	query_string  << Dam_Eco_Soil_Element::elem_table->get_column_name(label::areastate_id) << " = "  << id.area_state;
	query_string  << " AND ";
	query_string  <<  Dam_Eco_Soil_Element::elem_table->get_column_name(label::measure_id) << " = " << id.measure_nr ;
	query_string  << " AND ";
	query_string  << Dam_Eco_Soil_Element::elem_table->get_column_name(label::applied_flag) << " = "<< functions::convert_boolean2string(!flag);

	Data_Base::database_request(&query, query_string.str(), ptr_database);
	if(query.lastError().isValid()==true){
		Error msg;
		msg.set_msg("Dam_Eco_Soil_Element::switch_applied_flag_elem_table(QSqlDatabase *ptr_database, const _sys_system_id id, const bool flag)","Invalid database request", "Check the database", 2, false);
		ostringstream info;
		info << "Table Name      : " << Dam_Eco_Soil_Element::elem_table->get_table_name() << endl;
		info << "Table error info: " << query.lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
}
//Switch the applied-flag for the ecological soil-erosion damage elements in the database table for a defined system state (static)
void Dam_Eco_Soil_Element::switch_applied_flag_elem_table(QSqlDatabase *ptr_database, const _sys_system_id id, const int elem_id, const int raster_id, const bool flag){
//the table is set (the name and the column names) and allocated
	try{
		Dam_Eco_Soil_Element::set_elem_table(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}
	QSqlQueryModel query;

	ostringstream query_string;
	query_string <<"UPDATE ";
	query_string << Dam_Eco_Soil_Element::elem_table->get_table_name();
	query_string << " SET " ;
	query_string  << Dam_Eco_Soil_Element::elem_table->get_column_name(label::applied_flag) << " = "<< functions::convert_boolean2string(flag);
	query_string  << " WHERE ";
	query_string  << Dam_Eco_Soil_Element::elem_table->get_column_name(label::areastate_id) << " = "  << id.area_state;
	query_string  << " AND ";
	query_string  <<  Dam_Eco_Soil_Element::elem_table->get_column_name(label::measure_id) << " = " << id.measure_nr ;
	query_string  << " AND ";
	query_string  << Dam_Eco_Soil_Element::elem_table->get_column_name(dam_label::elem_id) << " = "<< elem_id;
	query_string  << " AND ";
	query_string  << Dam_Eco_Soil_Element::elem_table->get_column_name(dam_label::raster_id) << " = "<< raster_id;

	Data_Base::database_request(&query, query_string.str(), ptr_database);
	if(query.lastError().isValid()==true){
		Error msg;
		msg.set_msg("Dam_Eco_Soil_Element::switch_applied_flag_elem_table(QSqlDatabase *ptr_database, const _sys_system_id id, const int elem_id, const int raster_id, const bool flag)","Invalid database request", "Check the database", 2, false);
		ostringstream info;
		info << "Table Name      : " << Dam_Eco_Soil_Element::elem_table->get_table_name() << endl;
		info << "Table error info: " << query.lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
}
//Reswitch the applied-flag for the ecological soil-erosion damage elements in the database table for the base state; a defined system state and his applied elements are decisive (static)
void Dam_Eco_Soil_Element::reswitch_applied_flag_elem_table(QSqlDatabase *ptr_database, const _sys_system_id id, const bool flag){
	int number=0;
	try{
		Dam_Eco_Soil_Element::set_elem_table(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}

	QSqlQueryModel results;
	ostringstream test_filter;
	test_filter<< "Select ";
	test_filter <<  Dam_Eco_Soil_Element::elem_table->get_column_name(dam_label::elem_id) << " , ";
	test_filter <<  Dam_Eco_Soil_Element::elem_table->get_column_name(dam_label::raster_id) << "  ";

	test_filter << " from " << Dam_Eco_Soil_Element::elem_table->get_table_name();
	test_filter << " where " ;
	test_filter << Dam_Eco_Soil_Element::elem_table->get_column_name(label::applied_flag) << "= true";
	test_filter << " and ";
	test_filter << Dam_Eco_Soil_Element::elem_table->get_column_name(label::areastate_id) << " ="  << id.area_state;
	test_filter << " and ";
	test_filter << Dam_Eco_Soil_Element::elem_table->get_column_name(label::measure_id) << " = " << id.measure_nr;

	Data_Base::database_request(&results, test_filter.str(), ptr_database);

	if(results.lastError().isValid()==true){
		Error msg;
		msg.set_msg("Dam_Eco_Soil_Element::reswitch_applied_flag_elem_table(QSqlDatabase *ptr_database, const _sys_system_id id, const bool flag)","Invalid database request", "Check the database", 2, false);
		ostringstream info;
		info << "Table Name      : " << Dam_Eco_Soil_Element::elem_table->get_table_name() << endl;
		info << "Table error info: " << results.lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}

	number=results.rowCount();
	int rast_id=0;
	int elem_id=0;
	_sys_system_id base;
	base.area_state=id.area_state;
	base.measure_nr=0;

	for(int i=0; i< number ; i++){
		rast_id=results.record(i).value((Dam_Eco_Soil_Element::elem_table->get_column_name(dam_label::raster_id)).c_str()).toInt();
		elem_id=results.record(i).value((Dam_Eco_Soil_Element::elem_table->get_column_name(dam_label::elem_id)).c_str()).toInt();

		Dam_Eco_Soil_Element::switch_applied_flag_elem_table(ptr_database, base, elem_id, rast_id, flag);
	}
}
//Copy the results of a given system id to another one in database tables (static)
void Dam_Eco_Soil_Element::copy_results(QSqlDatabase *ptr_database, const _sys_system_id src, const _sys_system_id dest){
	//the table is set (the name and the column names) and allocated
	try{
		Dam_Eco_Soil_Element::set_erg_table(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}

	int glob_id=0;
	glob_id=Dam_Eco_Soil_Element::elem_erg_table->maximum_int_of_column(Dam_Eco_Soil_Element::elem_erg_table->get_column_name(dam_label::glob_id) ,ptr_database)+1;

	QSqlQueryModel model;
	ostringstream test_filter;
	test_filter<< "SELECT ";
	test_filter << Dam_Eco_Soil_Element::elem_erg_table->get_column_name(dam_label::glob_id);
	test_filter << " from " << Dam_Eco_Soil_Element::elem_erg_table->get_table_name();
	test_filter << " WHERE ";
	test_filter << Dam_Eco_Soil_Element::elem_erg_table->get_column_name(label::areastate_id) << " = " << src.area_state;
	test_filter << " AND ";
	test_filter << Dam_Eco_Soil_Element::elem_erg_table->get_column_name(label::measure_id) << " = " << src.measure_nr ;

	//submit it to the datbase
	Data_Base::database_request(&model, test_filter.str(), ptr_database);
	if(model.lastError().isValid()==true){
		Error msg;
		msg.set_msg("Dam_Eco_Soil_Element::copy_results(QSqlDatabase *ptr_database, const _sys_system_id src, const _sys_system_id dest)","Invalid database request", "Check the database", 2, false);
		ostringstream info;
		info << "Table Name      : " << Dam_Eco_Soil_Element::elem_erg_table->get_table_name() << endl;
		info << "Table error info: " << model.lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
	test_filter.str("");

	ostringstream cout;
	cout <<"Copy "<<model.rowCount()<<" element results of the ecological soil-erosion damages to the new measure state..."<<endl;
	Sys_Common_Output::output_dam->output_txt(&cout);

	QSqlQueryModel model1;
	for(int i=0; i< model.rowCount(); i++){
		if(i%10000==0 && i>0){
			cout << "Copy ECO-element's (soil-erosion) results "<< i <<" to " << i+10000 <<"..."<< endl;
			Sys_Common_Output::output_dam->output_txt(&cout);
		}
		test_filter.str("");
		test_filter << "INSERT INTO "<< Dam_Eco_Soil_Element::elem_erg_table->get_table_name();
		test_filter << " SELECT " << glob_id <<" , ";
		test_filter <<  Dam_Eco_Soil_Element::elem_erg_table->get_column_name(dam_label::elem_id)  <<" , ";
		test_filter <<  Dam_Eco_Soil_Element::elem_erg_table->get_column_name(dam_label::raster_id)  <<" , ";
		test_filter <<  dest.area_state  <<" , ";
		test_filter <<  dest.measure_nr  <<" , ";
		test_filter <<  Dam_Eco_Soil_Element::elem_erg_table->get_column_name(label::applied_flag)  <<" , ";
		test_filter <<  Dam_Eco_Soil_Element::elem_erg_table->get_column_name(hyd_label::sz_bound_id)  <<" , ";
		test_filter <<  Dam_Eco_Soil_Element::elem_erg_table->get_column_name(risk_label::sz_break_id)  <<" , ";
		test_filter <<  Dam_Eco_Soil_Element::elem_erg_table->get_column_name(dam_label::soil_cost)  <<" , ";
		test_filter <<  Dam_Eco_Soil_Element::elem_erg_table->get_column_name(dam_label::soil_erosion_zone)  <<" , ";
		test_filter <<  Dam_Eco_Soil_Element::elem_erg_table->get_column_name(dam_label::elem_poly)  <<"  ";

		test_filter << " FROM " << Dam_Eco_Soil_Element::elem_erg_table->get_table_name() <<" ";
		test_filter << " WHERE " << Dam_Eco_Soil_Element::elem_erg_table->get_column_name(dam_label::glob_id) << " = ";
		test_filter << model.record(i).value(Dam_Eco_Soil_Element::elem_erg_table->get_column_name(dam_label::glob_id).c_str()).toInt();
		Data_Base::database_request(&model1, test_filter.str(), ptr_database);
		if(model1.lastError().isValid()==true){
			Error msg;
			msg.set_msg("Dam_Eco_Soil_Element::copy_results(QSqlDatabase *ptr_database, const _sys_system_id src, const _sys_system_id dest)","Invalid database request", "Check the database", 2, false);
			ostringstream info;
			info << "Table Name      : " << Dam_Eco_Soil_Element::elem_erg_table->get_table_name() << endl;
			info << "Table error info: " << model1.lastError().text().toStdString() << endl;
			msg.make_second_info(info.str());
			throw msg;
		}
		glob_id++;
	}
}
//Select and count the number of relevant damage raster elements in a database table (static)
int Dam_Eco_Soil_Element::select_relevant_elements_database(QSqlQueryModel *results, QSqlDatabase *ptr_database, const _sys_system_id id, const int raster_id, const bool with_output){
	int number=0;
	try{
		Dam_Eco_Soil_Element::set_elem_table(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}
	if(with_output==true){
		ostringstream cout;
		cout << "Select relevant ecological soil-erosion raster elements in database ..." << endl ;
		Sys_Common_Output::output_dam->output_txt(&cout);
	}
	ostringstream test_filter;
	test_filter<< "Select ";
	test_filter <<  Dam_Eco_Soil_Element::elem_table->get_column_name(dam_label::elem_id) << " , ";
	test_filter <<  Dam_Eco_Soil_Element::elem_table->get_column_name(dam_label::elem_mid_x) << " , ";
	test_filter <<  Dam_Eco_Soil_Element::elem_table->get_column_name(dam_label::elem_mid_y) << " , ";
	test_filter <<  Dam_Eco_Soil_Element::elem_table->get_column_name(dam_label::conn_fp_id) << " , ";
	test_filter <<  Dam_Eco_Soil_Element::elem_table->get_column_name(dam_label::conn_fp_elem_id) << " , ";
	test_filter <<  Dam_Eco_Soil_Element::elem_table->get_column_name(dam_label::area_elem) << " , ";
	test_filter <<  Dam_Eco_Soil_Element::elem_table->get_column_name(dam_label::glob_id) << " , ";
	test_filter <<  Dam_Eco_Soil_Element::elem_table->get_column_name(dam_label::soil_type) << " , ";
	test_filter <<  Dam_Eco_Soil_Element::elem_table->get_column_name(dam_label::erosion_type) << " , ";
	test_filter <<  Dam_Eco_Soil_Element::elem_table->get_column_name(dam_label::no_erosion_type) << " , ";
	test_filter <<  Dam_Eco_Soil_Element::elem_table->get_column_name(dam_label::no_soil_type);

	test_filter << " from " << Dam_Eco_Soil_Element::elem_table->get_table_name();
	test_filter << " where " ;
	test_filter << Dam_Eco_Soil_Element::elem_table->get_column_name(label::applied_flag) << "= true";
	test_filter << " and ";
	test_filter << Dam_Eco_Soil_Element::elem_table->get_column_name(label::areastate_id) << " ="  << id.area_state;
	test_filter << " and (";
	test_filter << Dam_Eco_Soil_Element::elem_table->get_column_name(label::measure_id) << " = " << 0 ;
	test_filter << " or " ;
	test_filter << Dam_Eco_Soil_Element::elem_table->get_column_name(label::measure_id) << " = " << id.measure_nr;
	test_filter << " ) " ;
	test_filter << " and ";
	test_filter << Dam_Eco_Soil_Element::elem_table->get_column_name(dam_label::raster_id) << " = " << raster_id;
	test_filter << " order by " <<  Dam_Eco_Soil_Element::elem_table->get_column_name(dam_label::elem_id);

	Data_Base::database_request(results, test_filter.str(), ptr_database);

	//check the request
	if(results->lastError().isValid()){
		Error msg;
		msg.set_msg("Dam_Eco_Soil_Element::select_relevant_elements_database(QSqlTableModel *results, const _sys_system_id id, const int raster_id, const bool with_output)","Invalid database request", "Check the database", 2, false);
		ostringstream info;
		info << "Table Name      : " << Dam_Eco_Soil_Element::elem_table->get_table_name() << endl;
		info << "Table error info: " << results->lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}

	number= results->rowCount();

	if(with_output==true){
		ostringstream cout;
		cout << number << " relevant ecological soil-erosion raster elements(s) are selected in database" << endl ;
		Sys_Common_Output::output_dam->output_txt(&cout);
	}

	return number;
}
//Select and count the number of relevant damage raster elements in a database table (just part of it) (static)
int Dam_Eco_Soil_Element::select_relevant_elements_database(QSqlQueryModel *results, QSqlDatabase *ptr_database, const _sys_system_id id, const int raster_id,  const int offset, const int number_rows, const bool with_output){
		int number=0;
	try{
		Dam_Eco_Soil_Element::set_elem_table(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}
	if(with_output==true){
		ostringstream cout;
		cout << "Select relevant ecological soil-erosion raster elements in database ..." << endl ;
		Sys_Common_Output::output_dam->output_txt(&cout);
	}
	ostringstream test_filter;
	test_filter<< "Select ";
	test_filter <<  Dam_Eco_Soil_Element::elem_table->get_column_name(dam_label::elem_id) << " , ";
	test_filter <<  Dam_Eco_Soil_Element::elem_table->get_column_name(dam_label::elem_mid_x) << " , ";
	test_filter <<  Dam_Eco_Soil_Element::elem_table->get_column_name(dam_label::elem_mid_y) << " , ";
	test_filter <<  Dam_Eco_Soil_Element::elem_table->get_column_name(dam_label::conn_fp_id) << " , ";
	test_filter <<  Dam_Eco_Soil_Element::elem_table->get_column_name(dam_label::conn_fp_elem_id) << " , ";
	test_filter <<  Dam_Eco_Soil_Element::elem_table->get_column_name(dam_label::area_elem) << " , ";
	test_filter <<  Dam_Eco_Soil_Element::elem_table->get_column_name(dam_label::glob_id) << " , ";
	test_filter <<  Dam_Eco_Soil_Element::elem_table->get_column_name(dam_label::soil_type) << " , ";
	test_filter <<  Dam_Eco_Soil_Element::elem_table->get_column_name(dam_label::erosion_type) << " , ";
	test_filter <<  Dam_Eco_Soil_Element::elem_table->get_column_name(dam_label::no_erosion_type) << " , ";
	test_filter <<  Dam_Eco_Soil_Element::elem_table->get_column_name(dam_label::no_soil_type);

	test_filter << " from " << Dam_Eco_Soil_Element::elem_table->get_table_name();
	test_filter << " where " ;
	test_filter << Dam_Eco_Soil_Element::elem_table->get_column_name(label::applied_flag) << "= true";
	test_filter << " and ";
	test_filter << Dam_Eco_Soil_Element::elem_table->get_column_name(label::areastate_id) << " ="  << id.area_state;
	test_filter << " and (";
	test_filter << Dam_Eco_Soil_Element::elem_table->get_column_name(label::measure_id) << " = " << 0 ;
	test_filter << " or " ;
	test_filter << Dam_Eco_Soil_Element::elem_table->get_column_name(label::measure_id) << " = " << id.measure_nr;
	test_filter << " ) " ;
	test_filter << " and ";
	test_filter << Dam_Eco_Soil_Element::elem_table->get_column_name(dam_label::raster_id) << " = " << raster_id;
	test_filter << " order by " <<  Dam_Eco_Soil_Element::elem_table->get_column_name(dam_label::elem_id);
	test_filter << " LIMIT " << number_rows;
	test_filter << " OFFSET " << offset;

	Data_Base::database_request(results, test_filter.str(), ptr_database);

	//check the request
	if(results->lastError().isValid()){
		Error msg;
		msg.set_msg("Dam_Eco_Soil_Element::select_relevant_elements_database(QSqlTableModel *results, const _sys_system_id id, const int raster_id, const int offset, const int number_rows, const bool with_output)","Invalid database request", "Check the database", 2, false);
		ostringstream info;
		info << "Table Name      : " << Dam_Eco_Soil_Element::elem_table->get_table_name() << endl;
		info << "Table error info: " << results->lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}

	number= results->rowCount();

	if(with_output==true){
		ostringstream cout;
		cout << number << " relevant ecological soil-erosion raster elements(s) are selected in database" << endl ;
		Sys_Common_Output::output_dam->output_txt(&cout);
	}

	return number;
}
//Count the number of relevant damage raster elements in a database table (static)
int Dam_Eco_Soil_Element::count_relevant_elements_database(QSqlQueryModel *results, QSqlDatabase *ptr_database, const _sys_system_id id, const int raster_id, const bool with_output){
		int number=0;
	try{
		Dam_Eco_Soil_Element::set_elem_table(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}
	if(with_output==true){
		ostringstream cout;
		cout << "Count relevant ecological soil-erosion raster elements in database ..." << endl ;
		Sys_Common_Output::output_dam->output_txt(&cout);
	}
	ostringstream test_filter;
	test_filter<< "Select COUNT(";
	test_filter <<  Dam_Eco_Soil_Element::elem_table->get_column_name(dam_label::elem_id) << " ) ";

	test_filter << " from " << Dam_Eco_Soil_Element::elem_table->get_table_name();
	test_filter << " where " ;
	test_filter << Dam_Eco_Soil_Element::elem_table->get_column_name(label::applied_flag) << "= true";
	test_filter << " and ";
	test_filter << Dam_Eco_Soil_Element::elem_table->get_column_name(label::areastate_id) << " ="  << id.area_state;
	test_filter << " and (";
	test_filter << Dam_Eco_Soil_Element::elem_table->get_column_name(label::measure_id) << " = " << 0 ;
	test_filter << " or " ;
	test_filter << Dam_Eco_Soil_Element::elem_table->get_column_name(label::measure_id) << " = " << id.measure_nr;
	test_filter << " ) " ;
	test_filter << " and ";
	test_filter << Dam_Eco_Soil_Element::elem_table->get_column_name(dam_label::raster_id) << " = " << raster_id;

	Data_Base::database_request(results, test_filter.str(), ptr_database);

	//check the request
	if(results->lastError().isValid()){
		Error msg;
		msg.set_msg("Dam_Eco_Soil_Element::count_relevant_elements_database(QSqlTableModel *results, const _sys_system_id id, const int raster_id, const bool with_output)","Invalid database request", "Check the database", 2, false);
		ostringstream info;
		info << "Table Name      : " << Dam_Eco_Soil_Element::elem_table->get_table_name() << endl;
		info << "Table error info: " << results->lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}

	if(results->rowCount()>0){
		number= results->record(0).field(0).value().toInt();
	}

	if(with_output==true){
		ostringstream cout;
		cout << number << " relevant ecological soil-erosion raster elements(s) are found in database" << endl ;
		Sys_Common_Output::output_dam->output_txt(&cout);
	}

	return number;
}
//________
//private
//Set warning(s)
Warning Dam_Eco_Soil_Element::set_warning(const int warn_type){
	string place="Dam_Eco_Soil_Element::";
		string help;
		string reason;
		string reaction;
		int type=0;
		Warning msg;
		stringstream info;

	switch (warn_type){
		case 0://input datas can not submitted
			place.append("transfer_input_members2database(QSqlDatabase *ptr_database, const int elem_index, const int raster_index)");
			reason="Can not submit the element information of the soil-erosion raster to the database";
			help="Check the database";
			type=2;
			break;
		case 1://interception datas can not submitted
			place.append("set_interception_elem_data2database(QSqlDatabase *ptr_database, const int raster_id)");
			reason="Can not submit the element information of the ecological soil-erosion damage raster evaluated by the raster interception to the database";
			help="Check the database";
			type=2;
			break;
		case 2://results datas can not submitted
			place.append("output_result_members2database(QSqlDatabase *ptr_database, const int bound_sz, const string break_sz, const int raster_id, const double cellsize, const double x_origin, const double y_origin)");
			reason="Can not submit the element results data of the ecological soil-erosion damage raster to the database";
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
