#include "Dam_Headers_Precompiled.h"
//#include "Dam_Ecn_Element.h"
//init static members
Tables *Dam_Ecn_Element::elem_table=NULL;
Tables *Dam_Ecn_Element::elem_erg_table=NULL;

//Default constructor
Dam_Ecn_Element::Dam_Ecn_Element(void){
	this->id_immob_dam=-1;
	this->ptr_immob_dam=NULL;
	this->id_mob_dam=-1;
	this->ptr_mob_dam=NULL;
	this->indiv_mob_stock_value=-9999.0;
	this->indiv_immob_stock_value=-9999.0;
	this->no_info_immob_elem=false;
	this->no_info_mob_elem=false;
	this->result_mob_dam.quantile_5_result=0.0;
	this->result_mob_dam.quantile_95_result=0.0;
	this->result_mob_dam.mid_result=0.0;
	this->result_immob_dam.quantile_5_result=0.0;
	this->result_immob_dam.quantile_95_result=0.0;
	this->result_immob_dam.mid_result=0.0;
	this->result_total.quantile_5_result=0.0;
	this->result_total.quantile_95_result=0.0;
	this->result_total.mid_result=0.0;

	//count the memory
	Sys_Memory_Count::self()->add_mem(sizeof(Dam_Ecn_Element), _sys_system_modules::DAM_SYS);
}
//Default destructor
Dam_Ecn_Element::~Dam_Ecn_Element(void){
	//count the memory
	Sys_Memory_Count::self()->minus_mem(sizeof(Dam_Ecn_Element), _sys_system_modules::DAM_SYS);
}
//_____________________
//public
//Transfer the economic damage raster element data to a database
void Dam_Ecn_Element::transfer_input_members2database(QSqlDatabase *ptr_database, const int raster_index, const double cellsize, const double x_origin, const double y_origin){
	//mysql query with the query-class
	QSqlQuery model(*ptr_database);
	//the table is set (the name and the column names) and allocated
	try{
		Dam_Ecn_Element::set_elem_table(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}

	//evaluate the global identifier
	int id_glob=Dam_Ecn_Element::elem_table->maximum_int_of_column(Dam_Ecn_Element::elem_table->get_column_name(dam_label::glob_id),ptr_database)+1;
	this->global_index=id_glob;

	//set the query via a query string
	ostringstream query_string;
	query_string << "INSERT INTO  " << Dam_Ecn_Element::elem_table->get_table_name();
	query_string <<" ( ";
	query_string << Dam_Ecn_Element::elem_table->get_column_name(dam_label::glob_id) <<" , ";
	query_string << Dam_Ecn_Element::elem_table->get_column_name(dam_label::elem_id) <<" , ";
	query_string << Dam_Ecn_Element::elem_table->get_column_name(dam_label::raster_id) <<" , ";
	query_string << Dam_Ecn_Element::elem_table->get_column_name(label::areastate_id) <<" , ";
	query_string << Dam_Ecn_Element::elem_table->get_column_name(label::measure_id) <<" , ";
	query_string << Dam_Ecn_Element::elem_table->get_column_name(label::applied_flag) <<" , ";
	//data
	query_string << Dam_Ecn_Element::elem_table->get_column_name(dam_label::immob_id) <<" , ";
	query_string << Dam_Ecn_Element::elem_table->get_column_name(dam_label::mob_id) <<" , ";
	query_string << Dam_Ecn_Element::elem_table->get_column_name(dam_label::immob_stock) <<" , ";
	query_string << Dam_Ecn_Element::elem_table->get_column_name(dam_label::mob_stock) <<" , ";
	query_string << Dam_Ecn_Element::elem_table->get_column_name(dam_label::elem_mid_x) <<" , ";
	query_string << Dam_Ecn_Element::elem_table->get_column_name(dam_label::elem_mid_y) <<" , ";
	query_string << Dam_Ecn_Element::elem_table->get_column_name(dam_label::area_elem) <<" , ";
	query_string << Dam_Ecn_Element::elem_table->get_column_name(dam_label::conn_fp_id) <<" , ";
	query_string << Dam_Ecn_Element::elem_table->get_column_name(dam_label::conn_fp_elem_id) <<" , ";

	query_string << Dam_Ecn_Element::elem_table->get_column_name(dam_label::no_immob_id) <<" , ";
	query_string << Dam_Ecn_Element::elem_table->get_column_name(dam_label::no_mob_id) <<" , ";

	query_string << Dam_Ecn_Element::elem_table->get_column_name(dam_label::elem_poly) <<" ) ";

	query_string << " VALUES ( ";
	query_string << id_glob << " , " ;
	query_string << this->elem_index<< " , " ;
	query_string << raster_index<< " , " ;
	query_string << this->system_id.area_state << " , " ;
	query_string << this->system_id.measure_nr << " , " ;
	query_string << "true" << " , " ;
	//data
	query_string << this->id_immob_dam<< " , " ;
	query_string << this->id_mob_dam<< " , " ;
	query_string << this->indiv_immob_stock_value<< " , " ;
	query_string << this->indiv_mob_stock_value<< " , " ;
	query_string << FORMAT_FIXED_REAL << P(8);
	query_string << this->mid_point.get_xcoordinate() << " , " ;
	query_string << this->mid_point.get_ycoordinate() << " , " ;
	query_string << this->area << " , " ;
	query_string << this->index_fp << " , " ;
	query_string << this->index_fp_elem << " , " ;

	query_string << ""<<functions::convert_boolean2string(this->no_info_immob_elem) << " , " ;
	query_string << ""<<functions::convert_boolean2string(this->no_info_mob_elem) << " , " ;

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
		info << "Table Name                : " << Dam_Ecn_Element::elem_table->get_table_name() << endl;
		info << "Table error info          : " << model.lastError().text().toStdString() << endl;
		info << "Element number            : " << this->elem_index << endl;
		msg.make_second_info(info.str());
		msg.output_msg(4);
	}
}
//Get the data-string to complete a insert-string for inserting the data of the element to database
string Dam_Ecn_Element::get_datastring_members2database(const int global_id, const int raster_index, const double cellsize, const double x_origin, const double y_origin){
	string buffer;//=label::not_set;
	ostringstream query_string ;
	query_string << " ( ";
	query_string << global_id << " , " ;
	query_string << this->elem_index<< " , " ;
	query_string << raster_index<< " , " ;
	query_string << this->system_id.area_state << " , " ;
	query_string << this->system_id.measure_nr << " , " ;
	query_string << "true" << " , " ;
	//data
	query_string << this->id_immob_dam<< " , " ;
	query_string << functions::convert_boolean2string(this->no_info_immob_elem)<< " , " ;
	query_string << this->id_mob_dam<< " , " ;
	query_string << functions::convert_boolean2string(this->no_info_mob_elem)<< " , " ;
	query_string << this->indiv_immob_stock_value<< " , " ;
	query_string << this->indiv_mob_stock_value<< " , " ;
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
//		query_string << this->polygon_string <<" ) ";

        this->polygon_string=this->get_elem_ploy_string(this->mid_point.get_xcoordinate(),this->mid_point.get_ycoordinate(),cellsize*0.5,cellsize*0.5);
        query_string << this->polygon_string <<" ) ";


	}
	else{
		query_string << this->polygon_string <<" ) ";
	}
	this->polygon_string=label::not_set;

	buffer=query_string.str();
	return buffer;
}
//Input the economic damage raster element data form a table model per database
void Dam_Ecn_Element::input_element_perdatabase(const QSqlQueryModel *results, const int glob_index){
	try{
		this->elem_index=results->record(glob_index).value((Dam_Ecn_Element::elem_table->get_column_name(dam_label::elem_id)).c_str()).toInt();
		//set the element members
		this->id_immob_dam=results->record(glob_index).value((Dam_Ecn_Element::elem_table->get_column_name(dam_label::immob_id)).c_str()).toInt();
		this->id_mob_dam=results->record(glob_index).value((Dam_Ecn_Element::elem_table->get_column_name(dam_label::mob_id)).c_str()).toInt();
		this->indiv_immob_stock_value=results->record(glob_index).value((Dam_Ecn_Element::elem_table->get_column_name(dam_label::immob_stock)).c_str()).toDouble();
		this->indiv_mob_stock_value=results->record(glob_index).value((Dam_Ecn_Element::elem_table->get_column_name(dam_label::mob_stock)).c_str()).toDouble();
		this->index_fp=results->record(glob_index).value((Dam_Ecn_Element::elem_table->get_column_name(dam_label::conn_fp_id)).c_str()).toInt();
		this->index_fp_elem=results->record(glob_index).value((Dam_Ecn_Element::elem_table->get_column_name(dam_label::conn_fp_elem_id)).c_str()).toInt();
		this->no_info_immob_elem=results->record(glob_index).value((Dam_Ecn_Element::elem_table->get_column_name(dam_label::no_immob_id)).c_str()).toBool();
		this->no_info_mob_elem=results->record(glob_index).value((Dam_Ecn_Element::elem_table->get_column_name(dam_label::no_mob_id)).c_str()).toBool();
		this->area=results->record(glob_index).value((Dam_Ecn_Element::elem_table->get_column_name(dam_label::area_elem)).c_str()).toDouble();
		double x_mid=0.0;
		double y_mid=0.0;
		x_mid=results->record(glob_index).value((Dam_Ecn_Element::elem_table->get_column_name(dam_label::elem_mid_x)).c_str()).toDouble();
		y_mid=results->record(glob_index).value((Dam_Ecn_Element::elem_table->get_column_name(dam_label::elem_mid_y)).c_str()).toDouble();
		this->mid_point.set_point_coordinate(x_mid, y_mid);
		this->global_index=results->record(glob_index).value((Dam_Ecn_Element::elem_table->get_column_name(dam_label::glob_id)).c_str()).toInt();
	}
	catch(Error msg){
		ostringstream info;
		info << "Element index            : " << this->elem_index << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
}
//Set the element data from the raster interception to the database table
void Dam_Ecn_Element::set_interception_elem_data2database(QSqlDatabase *ptr_database, QSqlQuery *elem_results, const int raster_id){
	//nothing to update
	if(this->area!=0.0 && this->index_fp<0 && this->index_fp_elem<0){
		return;
	}
	try{
		Dam_Ecn_Element::set_elem_table(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}

	//generate the filter
	ostringstream filter;
	filter << "UPDATE " << Dam_Ecn_Element::elem_table->get_table_name().c_str();
	filter << " SET ";
	filter << Dam_Ecn_Element::elem_table->get_column_name(dam_label::conn_fp_id).c_str() << " = " << this->index_fp << " , ";
	filter << Dam_Ecn_Element::elem_table->get_column_name(dam_label::conn_fp_elem_id).c_str() << " = " << this->index_fp_elem << " , ";
	filter << Dam_Ecn_Element::elem_table->get_column_name(dam_label::area_elem).c_str() << " = " << this->area;
	filter << " WHERE " ;
	filter << Dam_Ecn_Element::elem_table->get_column_name(dam_label::glob_id) << " = " << this->global_index;
	/*filter << Dam_Ecn_Element::elem_table->get_column_name(label::applied_flag) << "= true";
	filter << " and ";
	filter << Dam_Ecn_Element::elem_table->get_column_name(label::areastate_id) << " ="  << this->system_id.area_state;
	filter << " and (";
	filter << Dam_Ecn_Element::elem_table->get_column_name(label::measure_id) << " = " << 0 ;
	filter << " or " ;
	filter << Dam_Ecn_Element::elem_table->get_column_name(label::measure_id) << " = " << this->system_id.area_state;
	filter << " ) " ;
	filter << " and ";
	filter << Dam_Ecn_Element::elem_table->get_column_name(dam_label::raster_id) << " = " << raster_id;
	filter << " and ";
	filter << Dam_Ecn_Element::elem_table->get_column_name(dam_label::elem_id) << " = " << this->elem_index;*/

	//Sys_Common_Output::output_dam->output_txt(&filter);

	Data_Base::database_request(elem_results, filter.str(), ptr_database);
	if(elem_results->lastError().isValid()){
		Warning msg=this->set_warning(1);
		ostringstream info;
		info << "Table Name      : " << Dam_Ecn_Element::elem_table->get_table_name() << endl;
		info << "Table error info: " << elem_results->lastError().text().toStdString() << endl;
		info << "Raster-id       : "  << raster_id <<endl;
		info << "Element-id      : " << this->elem_index << endl;
		msg.make_second_info(info.str());
		msg.output_msg(4);
	}
}
//Get a string to set the element data from the raster interception to the database table: identifier of the floodplain, -floodplain element and the reduced area
bool Dam_Ecn_Element::get_string_interception_elem_data2database(ostringstream *text){
	if(this->area!=0.0 && this->index_fp<0 && this->index_fp_elem<0){
		return false;
	}

	*text << "UPDATE " << Dam_Ecn_Element::elem_table->get_table_name().c_str();
	*text << " SET ";
	*text << Dam_Ecn_Element::elem_table->get_column_name(dam_label::conn_fp_id).c_str() << " = " << this->index_fp << " , ";
	*text << Dam_Ecn_Element::elem_table->get_column_name(dam_label::conn_fp_elem_id).c_str() << " = " << this->index_fp_elem << " , ";
	*text << Dam_Ecn_Element::elem_table->get_column_name(dam_label::area_elem).c_str() << " = " << this->area;
	*text << " WHERE " ;
	*text << Dam_Ecn_Element::elem_table->get_column_name(dam_label::glob_id) << " = " << this->global_index <<";";
	return true;
}
//Set the index of the damage function for the land use category, calculating immobile damages
void Dam_Ecn_Element::set_index_immob_dam(const int index){
	this->id_immob_dam=index;
}
//Set the index of the damage function for the land use category, calculating mobile damages
void Dam_Ecn_Element::set_index_mob_dam(const int index){
	this->id_mob_dam=index;
}
//Set the individual stock value for this element, calculating mobile damages
void Dam_Ecn_Element::set_indiv_mob_stock_value(const double stockvalue){
	this->indiv_mob_stock_value=stockvalue;
}
//Set the individual stock value for this element, calculating immobile damages
void Dam_Ecn_Element::set_indiv_immob_stock_value(const double stockvalue){
	this->indiv_immob_stock_value=stockvalue;
}
//Get the index of the mobile damage function
int Dam_Ecn_Element::get_id_mobile_dam_func(void){
	return this->id_mob_dam;
}
//Get the index of the immobile damage function
int Dam_Ecn_Element::get_id_immobile_dam_func(void){
	return this->id_immob_dam;
}
//Get the result of the mobile damage calculation
_dam_ecn_result Dam_Ecn_Element::get_mob_damage_result(void){
	return this->result_mob_dam;
}
//Get the result of the immobile damage calculation
_dam_ecn_result Dam_Ecn_Element::get_immob_damage_result(void){
	return this->result_immob_dam;
}
//Get the total results of the damage calculation
_dam_ecn_result Dam_Ecn_Element::get_total_damage_result(void){
	return this->result_total;
}
//Output the damage members
void Dam_Ecn_Element::output_damage_members(ostringstream *txt){
	*txt << this->id_immob_dam << W(13) << this->id_mob_dam;
	*txt << W(19) << P(2)<< FORMAT_FIXED_REAL << this->indiv_immob_stock_value ;
	*txt << W(18) << this->indiv_mob_stock_value ;
}
//Connect the element to the mobile damage function
bool Dam_Ecn_Element::connect_mob_dam2damage_function(const int no_info, Dam_Ecn_Damage_Function *function){
	bool flag=false;

	if(this->id_mob_dam==no_info){
		this->no_info_mob_elem=true;
		flag=true;
	}
	else if(this->id_mob_dam==function->get_land_use_info().id){
		this->ptr_mob_dam=function;
		flag=true;
	}

	return flag;
}
//Connect the element to the immobile damage function
bool Dam_Ecn_Element::connect_immob_dam2damage_function(const int no_info, Dam_Ecn_Damage_Function *function){
	bool flag=false;

	if(this->no_info_immob_elem==true){
		flag=true;
	}
	else if(this->id_immob_dam==no_info){
		this->no_info_immob_elem=true;
		flag=true;
	}
	else if(this->id_immob_dam==function->get_land_use_info().id){
		this->ptr_immob_dam=function;
		flag=true;
	}
	return flag;
}
//Get a flag if the immobile damage function is found
bool Dam_Ecn_Element::immob_dam_func_is_found(void){
	if(this->no_info_immob_elem==true){
		return true;
	}
	if(this->ptr_immob_dam!=NULL){
		return true;
	}
	return false;
}
//Get a flag if the mobile damage function is found
bool Dam_Ecn_Element::mob_dam_func_is_found(void){
	if(this->no_info_mob_elem==true){
		return true;
	}
	if(this->ptr_mob_dam!=NULL){
		return true;
	}
	return false;
}
//Set the element to a no-info element concerning immobile damages
void Dam_Ecn_Element::set_noinfo_immob(void){
	this->no_info_immob_elem=true;
}
//Set the element to a no-info element concerning mobile damages
void Dam_Ecn_Element::set_noinfo_mob(void){
	this->no_info_mob_elem=true;
}
//Calculate the economical damages per element
void Dam_Ecn_Element::calculate_damages(Dam_Impact_Values *impact){
	_dam_ecn_function_point interpolated_result;
	interpolated_result.impact_value=0.0;
	interpolated_result.quant_5_rel_damage=0.0;
	interpolated_result.quant_95_rel_damage=0.0;
	interpolated_result.rel_damage=0.0;

	//reset the results
	this->result_immob_dam.mid_result=0.0;
	this->result_immob_dam.quantile_5_result=0.0;
	this->result_immob_dam.quantile_95_result=0.0;
	this->result_mob_dam.mid_result=0.0;
	this->result_mob_dam.quantile_5_result=0.0;
	this->result_mob_dam.quantile_95_result=0.0;
	this->result_total.mid_result=0.0;
	this->result_total.quantile_5_result=0.0;
	this->result_total.quantile_95_result=0.0;

	//element was not affected
	if(impact->get_was_wet_flag()==false){
		return;
	}

	//immobile damage calculation
	if(this->no_info_immob_elem==false){
		interpolated_result=this->ptr_immob_dam->interpolate_function(impact);
		if(this->indiv_immob_stock_value>=0.0){
			this->result_immob_dam.mid_result=this->area*this->indiv_immob_stock_value*interpolated_result.rel_damage/100.0;
			this->result_immob_dam.quantile_5_result=this->area*this->indiv_immob_stock_value*interpolated_result.quant_5_rel_damage/100.0;
			this->result_immob_dam.quantile_95_result=this->area*this->indiv_immob_stock_value*interpolated_result.quant_95_rel_damage/100.0;
		}
		else{
			this->result_immob_dam.mid_result=this->area*this->ptr_immob_dam->get_land_use_info().default_stockvalue*interpolated_result.rel_damage/100.0;
			this->result_immob_dam.quantile_5_result=this->area*this->ptr_immob_dam->get_land_use_info().def_stock_5*interpolated_result.quant_5_rel_damage/100.0;
			this->result_immob_dam.quantile_95_result=this->area*this->ptr_immob_dam->get_land_use_info().def_stock_95*interpolated_result.quant_95_rel_damage/100.0;
		}
	}
	//immobile damage calculation
	if(this->no_info_mob_elem==false){
		interpolated_result=this->ptr_mob_dam->interpolate_function(impact);
		if(this->indiv_mob_stock_value>=0.0){
			this->result_mob_dam.mid_result=this->area*this->indiv_mob_stock_value*interpolated_result.rel_damage/100.0;
			this->result_mob_dam.quantile_5_result=this->area*this->indiv_mob_stock_value*interpolated_result.quant_5_rel_damage/100.0;
			this->result_mob_dam.quantile_95_result=this->area*this->indiv_mob_stock_value*interpolated_result.quant_95_rel_damage/100.0;
		}
		else{
			this->result_mob_dam.mid_result=this->area*this->ptr_mob_dam->get_land_use_info().default_stockvalue*interpolated_result.rel_damage/100.0;
			this->result_mob_dam.quantile_5_result=this->area*this->ptr_mob_dam->get_land_use_info().def_stock_5*interpolated_result.quant_5_rel_damage/100.0;
			this->result_mob_dam.quantile_95_result=this->area*this->ptr_mob_dam->get_land_use_info().def_stock_95*interpolated_result.quant_95_rel_damage/100.0;
		}
	}
	//total result
	this->result_total.mid_result=this->result_mob_dam.mid_result+this->result_immob_dam.mid_result;
	this->result_total.quantile_5_result=this->result_mob_dam.quantile_5_result+this->result_immob_dam.quantile_5_result;
	this->result_total.quantile_95_result=this->result_mob_dam.quantile_95_result+this->result_immob_dam.quantile_95_result;
}
//Output the result memebrs to database
void Dam_Ecn_Element::output_result_members2database(QSqlDatabase *ptr_database, const int bound_sz, const string break_sz, const int raster_id, const double cellsize, const double x_origin, const double y_origin){
	if(this->result_total.mid_result < constant::zero_epsilon && this->result_total.quantile_5_result < constant::zero_epsilon &&
		this->result_total.quantile_95_result<constant::zero_epsilon){
		return;
	}

	//mysql query with the query-class
	QSqlQuery model(*ptr_database);
	//the table is set (the name and the column names) and allocated
	try{
		Dam_Ecn_Element::set_erg_table(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}

	//evaluate the global identifier
	int id_glob=Dam_Ecn_Element::elem_erg_table->maximum_int_of_column(Dam_Ecn_Element::elem_erg_table->get_column_name(dam_label::glob_id),ptr_database)+1;

	//set the query via a query string
	ostringstream query_string;
	query_string << "INSERT INTO  " << Dam_Ecn_Element::elem_erg_table->get_table_name();
	query_string <<" ( ";
	query_string << Dam_Ecn_Element::elem_erg_table->get_column_name(dam_label::glob_id) <<" , ";
	query_string << Dam_Ecn_Element::elem_erg_table->get_column_name(dam_label::elem_id) <<" , ";
	query_string << Dam_Ecn_Element::elem_erg_table->get_column_name(dam_label::raster_id) <<" , ";
	query_string << Dam_Ecn_Element::elem_erg_table->get_column_name(label::areastate_id) <<" , ";
	query_string << Dam_Ecn_Element::elem_erg_table->get_column_name(label::measure_id) <<" , ";
	query_string << Dam_Ecn_Element::elem_erg_table->get_column_name(hyd_label::sz_bound_id) <<" , ";
	query_string << Dam_Ecn_Element::elem_erg_table->get_column_name(risk_label::sz_break_id) <<" , ";
	//results
	query_string << Dam_Ecn_Element::elem_erg_table->get_column_name(dam_label::immob_dam) <<" , ";
	query_string << Dam_Ecn_Element::elem_erg_table->get_column_name(dam_label::mob_dam) <<" , ";
	query_string << Dam_Ecn_Element::elem_erg_table->get_column_name(dam_label::immob_5_quant) <<" , ";
	query_string << Dam_Ecn_Element::elem_erg_table->get_column_name(dam_label::immob_95_quant) <<" , ";
	query_string << Dam_Ecn_Element::elem_erg_table->get_column_name(dam_label::mob_5_quant) <<" , ";
	query_string << Dam_Ecn_Element::elem_erg_table->get_column_name(dam_label::mob_95_quant) <<" , ";

	query_string << Dam_Ecn_Element::elem_erg_table->get_column_name(dam_label::elem_poly) <<" ) ";

	query_string << " VALUES ( ";
	query_string << id_glob << " , " ;
	query_string << this->elem_index<< " , " ;
	query_string << raster_id<< " , " ;
	query_string << this->system_id.area_state << " , " ;
	query_string << this->system_id.measure_nr << " , " ;
	query_string << bound_sz << " , " ;
	query_string << "'"<<break_sz<< "' , " ;
	//results
	query_string << this->result_immob_dam.mid_result << " , " ;
	query_string << this->result_mob_dam.mid_result << " , " ;
	query_string << this->result_immob_dam.quantile_5_result << " , " ;
	query_string << this->result_immob_dam.quantile_95_result << " , " ;
	query_string << this->result_mob_dam.quantile_5_result << " , " ;
	query_string << this->result_mob_dam.quantile_95_result << " , " ;

	if(this->polygon_string==label::not_set){
		//set the polgon points
		Geo_Raster_Polygon poly_buff;
		poly_buff.set_points(&this->mid_point, cellsize, cellsize, 0.0, x_origin, y_origin);
		this->polygon_string=poly_buff.get_polygon2sql_string();
		query_string << this->polygon_string <<" ) ";
	}
	else{
		query_string << this->polygon_string <<" ) ";	}

	Data_Base::database_request(&model, query_string.str(), ptr_database);

	if(model.lastError().isValid()){
		Warning msg=this->set_warning(2);
		ostringstream info;
		info << "Table Name                : " << Dam_Ecn_Element::elem_erg_table->get_table_name() << endl;
		info << "Table error info          : " << model.lastError().text().toStdString() << endl;
		info << "Element number            : " << this->elem_index << endl;
		msg.make_second_info(info.str());
		msg.output_msg(4);
	}
}
//Get the data-string to complete a insert-string for inserting the data of the element results to database
string Dam_Ecn_Element::get_datastring_results2database(const int global_id, const int bound_sz, const string break_sz, const int raster_id, const double cellsize, const double x_origin, const double y_origin , const bool must_output){
	string buffer=label::not_set;
	if(must_output==false){
		if(this->result_total.mid_result < constant::zero_epsilon && this->result_total.quantile_5_result < constant::zero_epsilon &&
			this->result_total.quantile_95_result<constant::zero_epsilon){
			return buffer;
		}
	}
	//set the query via a query string
	ostringstream query_string;
	query_string << " ( ";
	query_string << global_id<< " , " ;
	query_string << this->elem_index<< " , " ;
	query_string << raster_id<< " , " ;
	query_string << this->system_id.area_state << " , " ;
	query_string << this->system_id.measure_nr << " , " ;
	query_string << bound_sz << " , " ;
	query_string << "'"<<break_sz<< "' , " ;
	//results
	query_string << this->result_immob_dam.mid_result << " , " ;
	query_string << this->result_mob_dam.mid_result << " , " ;
	query_string << this->result_total.mid_result << " , " ;
	query_string << this->result_immob_dam.quantile_5_result << " , " ;
	query_string << this->result_immob_dam.quantile_95_result << " , " ;
	query_string << this->result_mob_dam.quantile_5_result << " , " ;
	query_string << this->result_mob_dam.quantile_95_result << " , " ;

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
void Dam_Ecn_Element::create_elem_table(QSqlDatabase *ptr_database){
		if(Dam_Ecn_Element::elem_table==NULL){
			ostringstream cout;
			cout << "Create economical raster element data database table..." << endl ;
			Sys_Common_Output::output_dam->output_txt(&cout);
			//make specific input for this class
			const string tab_name=dam_label::tab_ecn_element;
			const int num_col=18;
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

			tab_col[6].name=dam_label::immob_id;
			tab_col[6].type=sys_label::tab_col_type_int;
			tab_col[6].default_value="-1";

			tab_col[7].name=dam_label::no_immob_id;
			tab_col[7].type=sys_label::tab_col_type_bool;
			tab_col[7].default_value="false";

			tab_col[8].name=dam_label::mob_id;
			tab_col[8].type=sys_label::tab_col_type_int;
			tab_col[8].default_value="-1";

			tab_col[9].name=dam_label::no_mob_id;
			tab_col[9].type=sys_label::tab_col_type_bool;
			tab_col[9].default_value="false";

			tab_col[10].name=dam_label::immob_stock;
			tab_col[10].type=sys_label::tab_col_type_double;
			tab_col[10].default_value="-1.0";

			tab_col[11].name=dam_label::mob_stock;
			tab_col[11].type=sys_label::tab_col_type_double;
			tab_col[11].default_value="-1.0";

			tab_col[12].name=dam_label::elem_mid_x;
			tab_col[12].type=sys_label::tab_col_type_double;

			tab_col[13].name=dam_label::elem_mid_y;
			tab_col[13].type=sys_label::tab_col_type_double;

			tab_col[14].name=dam_label::elem_poly;
			tab_col[14].type=sys_label::tab_col_type_polygon;

			tab_col[15].name=dam_label::conn_fp_id;
			tab_col[15].type=sys_label::tab_col_type_int;
			tab_col[15].default_value="-1";

			tab_col[16].name=dam_label::conn_fp_elem_id;
			tab_col[16].type=sys_label::tab_col_type_int;
			tab_col[16].default_value="-1";

			tab_col[17].name=dam_label::area_elem;
			tab_col[17].type=sys_label::tab_col_type_double;
			tab_col[17].default_value="0.0";

			try{
				Dam_Ecn_Element::elem_table= new Tables();
				if(Dam_Ecn_Element::elem_table->create_non_existing_tables(tab_name, tab_col, num_col,ptr_database, _sys_table_type::dam)==false){
					cout << " Table exists" << endl ;
					Sys_Common_Output::output_dam->output_txt(&cout);
				};
			}
			catch(bad_alloc& t){
				Error msg;
				msg.set_msg("Dam_Ecn_Element::create_elem_table(QSqlDatabase *ptr_database)","Can not allocate the memory", "Check the memory", 10, false);
				ostringstream info;
				info<< "Info bad alloc: " << t.what() << endl;
				msg.make_second_info(info.str());
				throw msg;
			}
			catch(Error msg){
				Dam_Ecn_Element::close_elem_table();
				throw msg;
			}

		Dam_Ecn_Element::close_elem_table();
	}
}
//Set the database table for the raster elements: it sets the table name and the name of the columns and allocate them (static)
void Dam_Ecn_Element::set_elem_table(QSqlDatabase *ptr_database){
	if(Dam_Ecn_Element::elem_table==NULL){
		//make specific input for this class
		const string tab_id_name=dam_label::tab_ecn_element;
		string tab_col[18];

		tab_col[0]=dam_label::glob_id;
		tab_col[1]=dam_label::elem_id;
		tab_col[2]=dam_label::raster_id;
		tab_col[3]=label::areastate_id;
		tab_col[4]=label::measure_id;
		tab_col[5]=dam_label::immob_id;
		tab_col[6]=dam_label::mob_id;
		tab_col[7]=dam_label::immob_stock;
		tab_col[8]=dam_label::mob_stock;
		tab_col[9]=dam_label::elem_mid_x;
		tab_col[10]=dam_label::elem_mid_y;
		tab_col[11]=dam_label::elem_poly;
		tab_col[12]=dam_label::no_immob_id;
		tab_col[13]=dam_label::no_mob_id;
		tab_col[14]=dam_label::conn_fp_id;
		tab_col[15]=dam_label::conn_fp_elem_id;
		tab_col[16]=label::applied_flag;
		tab_col[17]=dam_label::area_elem;

		try{
			Dam_Ecn_Element::elem_table= new Tables(tab_id_name, tab_col, sizeof(tab_col)/sizeof(tab_col[0]));
			Dam_Ecn_Element::elem_table->set_name(ptr_database, _sys_table_type::dam);
		}
		catch(bad_alloc& t){
			Error msg;
			msg.set_msg("Dam_Ecn_Element::set_elem_table(QSqlDatabase *ptr_database)","Can not allocate the memory", "Check the memory", 10, false);
			ostringstream info;
			info<< "Info bad alloc: " << t.what() << endl;
			msg.make_second_info(info.str());
			throw msg;
		}
		catch(Error msg){
			Dam_Ecn_Element::close_elem_table();
			throw msg;
		}
	}
}
//Delete all data in the database table for the raster elements (static)
void Dam_Ecn_Element::delete_data_in_elem_table(QSqlDatabase *ptr_database){
	//the table is set (the name and the column names) and allocated
	try{
		Dam_Ecn_Element::set_elem_table(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}
	//delete the table
	Dam_Ecn_Element::elem_table->delete_data_in_table(ptr_database);
}
//Delete all data in the database table for the raster elements with a given system id (static)
void Dam_Ecn_Element::delete_data_in_elem_table(QSqlDatabase *ptr_database, const _sys_system_id id){
	QSqlQueryModel results;
	ostringstream test_filter;
	try{
		Dam_Ecn_Element::set_elem_table(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}
	//delete
	test_filter<< "DELETE ";
	test_filter << " FROM " << Dam_Ecn_Element::elem_table->get_table_name();
	test_filter << " WHERE ";
	test_filter << Dam_Ecn_Element::elem_table->get_column_name(label::areastate_id) << " = " << id.area_state;
	test_filter << " AND ";
	test_filter<< Dam_Ecn_Element::elem_table->get_column_name(label::measure_id) << " = " << id.measure_nr;

	Data_Base::database_request(&results, test_filter.str(), ptr_database);

	//check the request
	if(results.lastError().isValid()){
		Error msg;
		msg.set_msg("Dam_Ecn_Element::delete_data_in_elem_table(QSqlDatabase *ptr_database, const _sys_system_id id)","Invalid database request", "Check the database", 2, false);
		ostringstream info;
		info << "Table Name      : " << Dam_Ecn_Element::elem_table->get_table_name() << endl;
		info << "Table error info: " << results.lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
}
//Delete all data in the database table for the raster elements of the given raster index (static)
void Dam_Ecn_Element::delete_data_in_elem_table(QSqlDatabase *ptr_database, const int raster_id){
	QSqlQueryModel results;
	ostringstream test_filter;
	try{
		Dam_Ecn_Element::set_elem_table(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}
	//delete
	test_filter<< "DELETE ";
	test_filter << " FROM " << Dam_Ecn_Element::elem_table->get_table_name();
	test_filter << " WHERE ";
	test_filter << Dam_Ecn_Element::elem_table->get_column_name(dam_label::raster_id) << " = " << raster_id;

	Data_Base::database_request(&results, test_filter.str(), ptr_database);

	//check the request
	if(results.lastError().isValid()){
		Error msg;
		msg.set_msg("Dam_Ecn_Element::delete_data_in_elem_table(QSqlDatabase *ptr_database, const int raster_id)","Invalid database request", "Check the database", 2, false);
		ostringstream info;
		info << "Table Name      : " << Dam_Ecn_Element::elem_table->get_table_name() << endl;
		info << "Table error info: " << results.lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
}
//Close and delete the database table for the raster elements (static)
void Dam_Ecn_Element::close_elem_table(void){
	if(Dam_Ecn_Element::elem_table!=NULL){
		delete Dam_Ecn_Element::elem_table;
		Dam_Ecn_Element::elem_table=NULL;
	}
}
//Get the maximum value of the global index of the element database table (static)
int Dam_Ecn_Element::get_max_glob_id_elem_table(QSqlDatabase *ptr_database){
	try{
		Dam_Ecn_Element::set_elem_table(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}

	//evaluate the global identifier
	int id_glob=Dam_Ecn_Element::elem_table->maximum_int_of_column(Dam_Ecn_Element::elem_table->get_column_name(dam_label::glob_id),ptr_database);
	return id_glob;
}
//Get the header for inserting the element data to database table (static)
string Dam_Ecn_Element::get_insert_header_elem_table(QSqlDatabase *ptr_database){
	try{
		Dam_Ecn_Element::set_elem_table(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}
	//set the query via a query string
	ostringstream query_string;
	query_string << "INSERT INTO  " << Dam_Ecn_Element::elem_table->get_table_name();
	query_string <<" ( ";
	query_string << Dam_Ecn_Element::elem_table->get_column_name(dam_label::glob_id) <<" , ";
	query_string << Dam_Ecn_Element::elem_table->get_column_name(dam_label::elem_id) <<" , ";
	query_string << Dam_Ecn_Element::elem_table->get_column_name(dam_label::raster_id) <<" , ";
	query_string << Dam_Ecn_Element::elem_table->get_column_name(label::areastate_id) <<" , ";
	query_string << Dam_Ecn_Element::elem_table->get_column_name(label::measure_id) <<" , ";
	query_string << Dam_Ecn_Element::elem_table->get_column_name(label::applied_flag) <<" , ";
	//data
	query_string << Dam_Ecn_Element::elem_table->get_column_name(dam_label::immob_id) <<" , ";
	query_string << Dam_Ecn_Element::elem_table->get_column_name(dam_label::no_immob_id) <<" , ";
	query_string << Dam_Ecn_Element::elem_table->get_column_name(dam_label::mob_id) <<" , ";
	query_string << Dam_Ecn_Element::elem_table->get_column_name(dam_label::no_mob_id) <<" , ";
	query_string << Dam_Ecn_Element::elem_table->get_column_name(dam_label::immob_stock) <<" , ";
	query_string << Dam_Ecn_Element::elem_table->get_column_name(dam_label::mob_stock) <<" , ";
	query_string << Dam_Ecn_Element::elem_table->get_column_name(dam_label::elem_mid_x) <<" , ";
	query_string << Dam_Ecn_Element::elem_table->get_column_name(dam_label::elem_mid_y) <<" , ";
	query_string << Dam_Ecn_Element::elem_table->get_column_name(dam_label::area_elem) <<" , ";

	query_string << Dam_Ecn_Element::elem_table->get_column_name(dam_label::elem_poly) <<" ) ";

	query_string << " VALUES ";
	string buffer;
	buffer=query_string.str();
	return buffer;
}
//Create the database table for the results of the damage calculation per element (static)
void Dam_Ecn_Element::create_erg_table(QSqlDatabase *ptr_database){
		if(Dam_Ecn_Element::elem_erg_table==NULL){
			ostringstream cout;
			cout << "Create economical raster element result database table..." << endl ;
			Sys_Common_Output::output_dam->output_txt(&cout);
			//make specific input for this class
			const string tab_name=dam_label::tab_ecn_erg;
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

			tab_col[6].name=hyd_label::sz_bound_id;
			tab_col[6].type=sys_label::tab_col_type_int;
			tab_col[6].unsigned_flag=true;
			tab_col[6].key_flag=true;

			tab_col[7].name=risk_label::sz_break_id;
			tab_col[7].type=sys_label::tab_col_type_string;
			tab_col[7].key_flag=true;

			tab_col[8].name=dam_label::immob_dam;
			tab_col[8].type=sys_label::tab_col_type_double;
			tab_col[8].default_value="0.0";
			tab_col[8].unsigned_flag=true;

			tab_col[9].name=dam_label::mob_dam;
			tab_col[9].type=sys_label::tab_col_type_double;
			tab_col[9].default_value="0.0";
			tab_col[9].unsigned_flag=true;

			tab_col[10].name=dam_label::total_dam;
			tab_col[10].type=sys_label::tab_col_type_double;
			tab_col[10].default_value="0.0";
			tab_col[10].unsigned_flag=true;

			tab_col[11].name=dam_label::immob_5_quant;
			tab_col[11].type=sys_label::tab_col_type_double;
			tab_col[11].default_value="0.0";
			tab_col[11].unsigned_flag=true;

			tab_col[12].name=dam_label::immob_95_quant;
			tab_col[12].type=sys_label::tab_col_type_double;
			tab_col[12].default_value="0.0";
			tab_col[12].unsigned_flag=true;

			tab_col[13].name=dam_label::mob_5_quant;
			tab_col[13].type=sys_label::tab_col_type_double;
			tab_col[13].default_value="0.0";
			tab_col[13].unsigned_flag=true;

			tab_col[14].name=dam_label::mob_95_quant;
			tab_col[14].type=sys_label::tab_col_type_double;
			tab_col[14].default_value="0.0";
			tab_col[14].unsigned_flag=true;

			tab_col[15].name=dam_label::elem_poly;
			tab_col[15].type=sys_label::tab_col_type_polygon;

			try{
				Dam_Ecn_Element::elem_erg_table= new Tables();
				if(Dam_Ecn_Element::elem_erg_table->create_non_existing_tables(tab_name, tab_col, num_col,ptr_database, _sys_table_type::dam)==false){
					cout << " Table exists" << endl ;
					Sys_Common_Output::output_dam->output_txt(&cout);
				};
			}
			catch(bad_alloc& t){
				Error msg;
				msg.set_msg("Dam_Ecn_Element::create_erg_table(QSqlDatabase *ptr_database)","Can not allocate the memory", "Check the memory", 10, false);
				ostringstream info;
				info<< "Info bad alloc: " << t.what() << endl;
				msg.make_second_info(info.str());
				throw msg;
			}
			catch(Error msg){
				Dam_Ecn_Element::close_erg_table();
				throw msg;
			}

		Dam_Ecn_Element::close_erg_table();
	}
}
//Set the database table for the results of the damage calculation per element: it sets the table name and the name of the columns and allocate them (static)
void Dam_Ecn_Element::set_erg_table(QSqlDatabase *ptr_database){
	if(Dam_Ecn_Element::elem_erg_table==NULL){
		//make specific input for this class
		const string tab_id_name=dam_label::tab_ecn_erg;
		string tab_col[16];

		tab_col[0]=dam_label::glob_id;
		tab_col[1]=dam_label::elem_id;
		tab_col[2]=dam_label::raster_id;
		tab_col[3]=label::areastate_id;
		tab_col[4]=label::measure_id;
		tab_col[5]=hyd_label::sz_bound_id;
		tab_col[6]=risk_label::sz_break_id;
		tab_col[7]=dam_label::immob_dam;
		tab_col[8]=dam_label::mob_dam;
		tab_col[9]=dam_label::immob_5_quant;
		tab_col[10]=dam_label::immob_95_quant;
		tab_col[11]=dam_label::mob_5_quant;
		tab_col[12]=dam_label::mob_95_quant;
		tab_col[13]=dam_label::elem_poly;
		tab_col[14]=label::applied_flag;
		tab_col[15]=dam_label::total_dam;

		try{
			Dam_Ecn_Element::elem_erg_table= new Tables(tab_id_name, tab_col, sizeof(tab_col)/sizeof(tab_col[0]));
			Dam_Ecn_Element::elem_erg_table->set_name(ptr_database, _sys_table_type::dam);
		}
		catch(bad_alloc& t){
			Error msg;
			msg.set_msg("Dam_Ecn_Element::set_erg_table(QSqlDatabase *ptr_database)","Can not allocate the memory", "Check the memory", 10, false);
			ostringstream info;
			info<< "Info bad alloc: " << t.what() << endl;
			msg.make_second_info(info.str());
			throw msg;
		}
		catch(Error msg){
			Dam_Ecn_Element::close_erg_table();
			throw msg;
		}
	}
}
//Delete all data in the database table for the results of the damage calculation per element (static)
void Dam_Ecn_Element::delete_data_in_erg_table(QSqlDatabase *ptr_database){
	//the table is set (the name and the column names) and allocated
	try{
		Dam_Ecn_Element::set_erg_table(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}
	//delete the table
	Dam_Ecn_Element::elem_erg_table->delete_data_in_table(ptr_database);
}
//Delete the data in the database table for the results of the damage calculation specified by the system id and the scenario-ids (static)
void Dam_Ecn_Element::delete_data_in_erg_table(QSqlDatabase *ptr_database, const _sys_system_id id){
	//the table is set (the name and the column names) and allocated
	try{
		Dam_Ecn_Element::set_erg_table(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}

	//delete the table
	QSqlQuery query(*ptr_database);
	ostringstream query_string;
	query_string <<"DELETE FROM " << Dam_Ecn_Element::elem_erg_table->get_table_name();
	query_string << " WHERE " ;
	query_string << Dam_Ecn_Element::elem_erg_table->get_column_name(label::areastate_id) << " = " << id.area_state;
	query_string << " AND ";
	query_string << Dam_Ecn_Element::elem_erg_table->get_column_name(label::measure_id) << " = " << id.measure_nr;

	Data_Base::database_request(&query, query_string.str(), ptr_database);
	if(query.lastError().isValid()==true){
		Error msg;
		msg.set_msg("Dam_Ecn_Element::delete_data_in_erg_table(QSqlDatabase *ptr_database, const _sys_system_id id)","Invalid database request", "Check the database", 2, false);
		ostringstream info;
		info << "Table Name      : " << Dam_Ecn_Element::elem_erg_table->get_table_name() << endl;
		info << "Table error info: " << query.lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
}
//Delete the data in the database table for the results of the damage calculation specified parameters (static)
void Dam_Ecn_Element::delete_data_in_erg_table(QSqlDatabase *ptr_database, const _sys_system_id id, const string break_sz, const bool like_flag){
	//the table is set (the name and the column names) and allocated
	try{
		Dam_Ecn_Element::set_erg_table(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}

	//delete the table
	QSqlQuery query(*ptr_database);
	ostringstream query_string;
	query_string <<"DELETE FROM " << Dam_Ecn_Element::elem_erg_table->get_table_name();
	query_string << " WHERE " ;
	query_string << Dam_Ecn_Element::elem_erg_table->get_column_name(label::areastate_id) << " = " << id.area_state;
	query_string << " AND ";
	query_string << Dam_Ecn_Element::elem_erg_table->get_column_name(label::measure_id) << " = " << id.measure_nr;
	query_string << " AND ";
	if(like_flag==false){
		query_string << Dam_Ecn_Element::elem_erg_table->get_column_name(risk_label::sz_break_id) << " = '" << break_sz<<"'";
	}
	else{
		query_string << Dam_Ecn_Element::elem_erg_table->get_column_name(risk_label::sz_break_id) << " LIKE '" << break_sz<<"'";
	}

	Data_Base::database_request(&query, query_string.str(), ptr_database);
	if(query.lastError().isValid()==true){
		Error msg;
		msg.set_msg("Dam_Ecn_Element::delete_data_in_erg_table(QSqlDatabase *ptr_database, const _sys_system_id id, const string break_sz, const bool like_flag)","Invalid database request", "Check the database", 2, false);
		ostringstream info;
		info << "Table Name      : " << Dam_Ecn_Element::elem_erg_table->get_table_name() << endl;
		info << "Table error info: " << query.lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
}
//Delete the data in the database table for the results of the damage calculation specified by the hydraulic scenario id (static)
void Dam_Ecn_Element::delete_data_in_erg_table(QSqlDatabase *ptr_database,  const int bound_sz){
	//the table is set (the name and the column names) and allocated
	try{
		Dam_Ecn_Element::set_erg_table(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}

	//delete the table
	QSqlQuery query(*ptr_database);
	ostringstream query_string;
	query_string <<"DELETE FROM " << Dam_Ecn_Element::elem_erg_table->get_table_name();
	query_string << " WHERE " ;
	query_string << Dam_Ecn_Element::elem_erg_table->get_column_name(hyd_label::sz_bound_id) << " = " << bound_sz;

	Data_Base::database_request(&query, query_string.str(), ptr_database);
	if(query.lastError().isValid()==true){
		Error msg;
		msg.set_msg("Dam_Ecn_Element::delete_data_in_erg_table(QSqlDatabase *ptr_database, const int bound_sz)","Invalid database request", "Check the database", 2, false);
		ostringstream info;
		info << "Table Name      : " << Dam_Ecn_Element::elem_erg_table->get_table_name() << endl;
		info << "Table error info: " << query.lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
}
//Delete the data in the database table for the results of the damage calculation specified by the system id and the scenario-ids (static)
void Dam_Ecn_Element::delete_data_in_erg_table(QSqlDatabase *ptr_database, const _sys_system_id id, const int bound_sz, const string break_sz ){
	//the table is set (the name and the column names) and allocated
	try{
		Dam_Ecn_Element::set_erg_table(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}

	//delete the table
	QSqlQuery query(*ptr_database);
	ostringstream query_string;
	query_string <<"DELETE FROM " << Dam_Ecn_Element::elem_erg_table->get_table_name();
	query_string << " WHERE " ;
	query_string << Dam_Ecn_Element::elem_erg_table->get_column_name(label::areastate_id) << " = " << id.area_state;
	query_string << " AND ";
	query_string << Dam_Ecn_Element::elem_erg_table->get_column_name(label::measure_id) << " = " << id.measure_nr;
	query_string << " AND ";
	query_string << Dam_Ecn_Element::elem_erg_table->get_column_name(hyd_label::sz_bound_id) << " = " << bound_sz;
	query_string << " AND ";
	query_string << Dam_Ecn_Element::elem_erg_table->get_column_name(risk_label::sz_break_id) << " = '" << break_sz<<"'";

	Data_Base::database_request(&query, query_string.str(), ptr_database);
	if(query.lastError().isValid()==true){
		Error msg;
		msg.set_msg("Dam_Ecn_Element::delete_data_in_erg_table(QSqlDatabase *ptr_database, const _sys_system_id id, const int bound_sz, const string break_sz )","Invalid database request", "Check the database", 2, false);
		ostringstream info;
		info << "Table Name      : " << Dam_Ecn_Element::elem_erg_table->get_table_name() << endl;
		info << "Table error info: " << query.lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
}
//Delete the data in the database table for the results of the damage calculation specified by the raster id (static)
void Dam_Ecn_Element::delete_data_in_erg_table_per_raster(QSqlDatabase *ptr_database, const int raster_id){
	//the table is set (the name and the column names) and allocated
	try{
		Dam_Ecn_Element::set_erg_table(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}

	//delete the table
	QSqlQuery query(*ptr_database);
	ostringstream query_string;
	query_string <<"DELETE FROM " << Dam_Ecn_Element::elem_erg_table->get_table_name();
	query_string << " WHERE " ;
	query_string << Dam_Ecn_Element::elem_erg_table->get_column_name(dam_label::raster_id) << " = " << raster_id;

	Data_Base::database_request(&query, query_string.str(), ptr_database);
	if(query.lastError().isValid()==true){
		Error msg;
		msg.set_msg("Dam_Ecn_Element::delete_data_in_erg_table(QSqlDatabase *ptr_database, const int raster_id)","Invalid database request", "Check the database", 2, false);
		ostringstream info;
		info << "Table Name      : " << Dam_Ecn_Element::elem_erg_table->get_table_name() << endl;
		info << "Table error info: " << query.lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
}
//Select data in the database table for the results of the damage calculation specified by the system id and the scenario-ids (static)
int Dam_Ecn_Element::select_data_in_erg_table(QSqlQueryModel *results, QSqlDatabase *ptr_database, const _sys_system_id id, const int bound_sz, const string break_sz ){
	//the table is set (the name and the column names) and allocated
	try{
		Dam_Ecn_Element::set_erg_table(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}

	ostringstream query_string;
	query_string <<"SELECT ";
	query_string << Dam_Ecn_Element::elem_erg_table->get_column_name(dam_label::elem_id) << " , ";
	query_string << Dam_Ecn_Element::elem_erg_table->get_column_name(dam_label::raster_id) << " , ";
	query_string << Dam_Ecn_Element::elem_erg_table->get_column_name(dam_label::immob_dam) << " , ";
	query_string << Dam_Ecn_Element::elem_erg_table->get_column_name(dam_label::mob_dam) << " , ";
	query_string << " ST_ASTEXT("<<Dam_Ecn_Element::elem_erg_table->get_column_name(dam_label::elem_poly) <<")";

	query_string<<" FROM " << Dam_Ecn_Element::elem_erg_table->get_table_name();
	query_string << " WHERE " ;
	query_string << Dam_Ecn_Element::elem_erg_table->get_column_name(label::applied_flag) << "= true";
	query_string << " AND ";
	query_string << Dam_Ecn_Element::elem_erg_table->get_column_name(label::areastate_id) << " ="  << id.area_state;
	query_string << " AND (";
	query_string << Dam_Ecn_Element::elem_erg_table->get_column_name(label::measure_id) << " = " << 0 ;
	query_string << " OR " ;
	query_string << Dam_Ecn_Element::elem_erg_table->get_column_name(label::measure_id) << " = " << id.measure_nr;
	query_string << " ) " ;
	query_string << " AND ";
	query_string << Dam_Ecn_Element::elem_erg_table->get_column_name(hyd_label::sz_bound_id) << " = " << bound_sz;
	query_string << " AND ";
	query_string << Dam_Ecn_Element::elem_erg_table->get_column_name(risk_label::sz_break_id) << " = '" << break_sz<<"'";
	query_string << " ORDER BY ";
	query_string << Dam_Ecn_Element::elem_erg_table->get_column_name(dam_label::raster_id) << " , ";
	query_string << Dam_Ecn_Element::elem_erg_table->get_column_name(dam_label::elem_id) << "  ";

	Data_Base::database_request(results, query_string.str(), ptr_database);
	if(results->lastError().isValid()==true){
		Error msg;
		msg.set_msg("Dam_Ecn_Element::select_data_in_erg_table(QSqlDatabase *ptr_database, const _sys_system_id id, const int bound_sz, const string break_sz )","Invalid database request", "Check the database", 2, false);
		ostringstream info;
		info << "Table Name      : " << Dam_Ecn_Element::elem_erg_table->get_table_name() << endl;
		info << "Table error info: " << results->lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}

	return results->rowCount();
}
//Select data in the database table for the results of the damage calculation specified by the system id and the scenario-ids (just part of it) (static)
int Dam_Ecn_Element::select_data_in_erg_table(QSqlQueryModel *results, QSqlDatabase *ptr_database, const _sys_system_id id, const int bound_sz, const string break_sz, const int offset, const int number_rows ){
		//the table is set (the name and the column names) and allocated
	try{
		Dam_Ecn_Element::set_erg_table(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}

	ostringstream query_string;
	query_string <<"SELECT ";
	query_string << Dam_Ecn_Element::elem_erg_table->get_column_name(dam_label::elem_id) << " , ";
	query_string << Dam_Ecn_Element::elem_erg_table->get_column_name(dam_label::raster_id) << " , ";
	query_string << Dam_Ecn_Element::elem_erg_table->get_column_name(dam_label::immob_dam) << " , ";
	query_string << Dam_Ecn_Element::elem_erg_table->get_column_name(dam_label::mob_dam) << " , ";
	query_string << " ST_ASTEXT("<<Dam_Ecn_Element::elem_erg_table->get_column_name(dam_label::elem_poly) <<")";

	query_string<<" FROM " << Dam_Ecn_Element::elem_erg_table->get_table_name();
	query_string << " WHERE " ;
	query_string << Dam_Ecn_Element::elem_erg_table->get_column_name(label::applied_flag) << "= true";
	query_string << " AND ";
	query_string << Dam_Ecn_Element::elem_erg_table->get_column_name(label::areastate_id) << " ="  << id.area_state;
	query_string << " AND (";
	query_string << Dam_Ecn_Element::elem_erg_table->get_column_name(label::measure_id) << " = " << 0 ;
	query_string << " OR " ;
	query_string << Dam_Ecn_Element::elem_erg_table->get_column_name(label::measure_id) << " = " << id.measure_nr;
	query_string << " ) " ;
	query_string << " AND ";
	query_string << Dam_Ecn_Element::elem_erg_table->get_column_name(hyd_label::sz_bound_id) << " = " << bound_sz;
	query_string << " AND ";
	query_string << Dam_Ecn_Element::elem_erg_table->get_column_name(risk_label::sz_break_id) << " = '" << break_sz<<"'";
	query_string << " ORDER BY ";
	query_string << Dam_Ecn_Element::elem_erg_table->get_column_name(dam_label::raster_id) << " , ";
	query_string << Dam_Ecn_Element::elem_erg_table->get_column_name(dam_label::elem_id) << "  ";
	query_string << " LIMIT " << number_rows;
	query_string << " OFFSET " << offset;

	Data_Base::database_request(results, query_string.str(), ptr_database);
	if(results->lastError().isValid()==true){
		Error msg;
		msg.set_msg("Dam_Ecn_Element::select_data_in_erg_table(QSqlDatabase *ptr_database, const _sys_system_id id, const int bound_sz, const string break_sz, const int offset, const int number_rows)","Invalid database request", "Check the database", 2, false);
		ostringstream info;
		info << "Table Name      : " << Dam_Ecn_Element::elem_erg_table->get_table_name() << endl;
		info << "Table error info: " << results->lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}

	return results->rowCount();
}
//Count data in the database table for the results of the damage calculation specified by the system id and the scenario-ids (static)
int Dam_Ecn_Element::count_data_in_erg_table(QSqlQueryModel *results, QSqlDatabase *ptr_database, const _sys_system_id id, const int bound_sz, const string break_sz ){
	//the table is set (the name and the column names) and allocated
	try{
		Dam_Ecn_Element::set_erg_table(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}

	int number=0;
	ostringstream query_string;
	query_string <<"SELECT COUNT(";
	query_string << Dam_Ecn_Element::elem_erg_table->get_column_name(dam_label::elem_id) << " ) ";
	query_string<<" FROM " << Dam_Ecn_Element::elem_erg_table->get_table_name();
	query_string << " WHERE " ;
	query_string << Dam_Ecn_Element::elem_erg_table->get_column_name(label::applied_flag) << "= true";
	query_string << " AND ";
	query_string << Dam_Ecn_Element::elem_erg_table->get_column_name(label::areastate_id) << " ="  << id.area_state;
	query_string << " AND (";
	query_string << Dam_Ecn_Element::elem_erg_table->get_column_name(label::measure_id) << " = " << 0 ;
	query_string << " OR " ;
	query_string << Dam_Ecn_Element::elem_erg_table->get_column_name(label::measure_id) << " = " << id.measure_nr;
	query_string << " ) " ;
	query_string << " AND ";
	query_string << Dam_Ecn_Element::elem_erg_table->get_column_name(hyd_label::sz_bound_id) << " = " << bound_sz;
	query_string << " AND ";
	query_string << Dam_Ecn_Element::elem_erg_table->get_column_name(risk_label::sz_break_id) << " = '" << break_sz<<"'";

	Data_Base::database_request(results, query_string.str(), ptr_database);
	if(results->lastError().isValid()==true){
		Error msg;
		msg.set_msg("Dam_Ecn_Element::count_data_in_erg_table(QSqlDatabase *ptr_database, const _sys_system_id id, const int bound_sz, const string break_sz )","Invalid database request", "Check the database", 2, false);
		ostringstream info;
		info << "Table Name      : " << Dam_Ecn_Element::elem_erg_table->get_table_name() << endl;
		info << "Table error info: " << results->lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
	if(results->rowCount()>0){
		number= results->record(0).field(0).value().toInt();
	}

	return number;
}
//Close and delete the database table for the results of the damage calculation per element (static)
void Dam_Ecn_Element::close_erg_table(void){
	if(Dam_Ecn_Element::elem_erg_table!=NULL){
		delete Dam_Ecn_Element::elem_erg_table;
		Dam_Ecn_Element::elem_erg_table=NULL;
	}
}
//Get the maximum value of the global index of the element result database table (static)
int Dam_Ecn_Element::get_max_glob_id_erg_table(QSqlDatabase *ptr_database){
	//the table is set (the name and the column names) and allocated
	try{
		Dam_Ecn_Element::set_erg_table(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}

	//evaluate the global identifier
	int id_glob=Dam_Ecn_Element::elem_erg_table->maximum_int_of_column(Dam_Ecn_Element::elem_erg_table->get_column_name(dam_label::glob_id),ptr_database);
	return id_glob;
}
//Get the header for inserting the element result data to database table (static)
string Dam_Ecn_Element::get_insert_header_erg_table(QSqlDatabase *ptr_database){
	try{
		Dam_Ecn_Element::set_erg_table(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}
	//set the query via a query string
	ostringstream query_string;
	query_string << "INSERT INTO  " << Dam_Ecn_Element::elem_erg_table->get_table_name();
	query_string <<" ( ";
	query_string << Dam_Ecn_Element::elem_erg_table->get_column_name(dam_label::glob_id) <<" , ";
	query_string << Dam_Ecn_Element::elem_erg_table->get_column_name(dam_label::elem_id) <<" , ";
	query_string << Dam_Ecn_Element::elem_erg_table->get_column_name(dam_label::raster_id) <<" , ";
	query_string << Dam_Ecn_Element::elem_erg_table->get_column_name(label::areastate_id) <<" , ";
	query_string << Dam_Ecn_Element::elem_erg_table->get_column_name(label::measure_id) <<" , ";
	query_string << Dam_Ecn_Element::elem_erg_table->get_column_name(hyd_label::sz_bound_id) <<" , ";
	query_string << Dam_Ecn_Element::elem_erg_table->get_column_name(risk_label::sz_break_id) <<" , ";
	//results
	query_string << Dam_Ecn_Element::elem_erg_table->get_column_name(dam_label::immob_dam) <<" , ";
	query_string << Dam_Ecn_Element::elem_erg_table->get_column_name(dam_label::mob_dam) <<" , ";
	query_string << Dam_Ecn_Element::elem_erg_table->get_column_name(dam_label::total_dam) <<" , ";
	query_string << Dam_Ecn_Element::elem_erg_table->get_column_name(dam_label::immob_5_quant) <<" , ";
	query_string << Dam_Ecn_Element::elem_erg_table->get_column_name(dam_label::immob_95_quant) <<" , ";
	query_string << Dam_Ecn_Element::elem_erg_table->get_column_name(dam_label::mob_5_quant) <<" , ";
	query_string << Dam_Ecn_Element::elem_erg_table->get_column_name(dam_label::mob_95_quant) <<" , ";

	query_string << Dam_Ecn_Element::elem_erg_table->get_column_name(dam_label::elem_poly) <<" ) ";

	query_string << " VALUES ";
	string buffer;
	buffer=query_string.str();
	return buffer;
}
//Switch the applied-flag for the economic damage results in the database table for a defined system state (static)
void Dam_Ecn_Element::switch_applied_flag_erg_table(QSqlDatabase *ptr_database, const _sys_system_id id, const bool flag){
//the table is set (the name and the column names) and allocated
	try{
		Dam_Ecn_Element::set_erg_table(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}
	QSqlQueryModel query;

	ostringstream query_string;
	query_string <<"UPDATE ";
	query_string << Dam_Ecn_Element::elem_erg_table->get_table_name();
	query_string << " SET " ;
	query_string  << Dam_Ecn_Element::elem_erg_table->get_column_name(label::applied_flag) << " = "<< functions::convert_boolean2string(flag);
	query_string  << " WHERE ";
	query_string  << Dam_Ecn_Element::elem_erg_table->get_column_name(label::areastate_id) << " = "  << id.area_state;
	query_string  << " AND ";
	query_string  <<  Dam_Ecn_Element::elem_erg_table->get_column_name(label::measure_id) << " = " << id.measure_nr ;
	query_string  << " AND ";
	query_string  << Dam_Ecn_Element::elem_erg_table->get_column_name(label::applied_flag) << " = "<< functions::convert_boolean2string(!flag);

	Data_Base::database_request(&query, query_string.str(), ptr_database);
	if(query.lastError().isValid()==true){
		Error msg;
		msg.set_msg("Dam_Ecn_Element::switch_applied_flag_erg_table(QSqlDatabase *ptr_database, const _sys_system_id id, const bool flag)","Invalid database request", "Check the database", 2, false);
		ostringstream info;
		info << "Table Name      : " << Dam_Ecn_Element::elem_erg_table->get_table_name() << endl;
		info << "Table error info: " << query.lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
}
//Switch the applied-flag for the economic damage results in the database table for a defined system state (static)
void Dam_Ecn_Element::switch_applied_flag_erg_table(QSqlDatabase *ptr_database, const _sys_system_id id, const int hyd_sc, const bool flag){
	//the table is set (the name and the column names) and allocated
	try{
		Dam_Ecn_Element::set_erg_table(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}
	QSqlQueryModel query;

	ostringstream query_string;
	query_string <<"UPDATE ";
	query_string << Dam_Ecn_Element::elem_erg_table->get_table_name();
	query_string << " SET " ;
	query_string  << Dam_Ecn_Element::elem_erg_table->get_column_name(label::applied_flag) << " = "<< functions::convert_boolean2string(flag);
	query_string  << " WHERE ";
	query_string  << Dam_Ecn_Element::elem_erg_table->get_column_name(label::areastate_id) << " = "  << id.area_state;
	query_string  << " AND ";
	query_string  <<  Dam_Ecn_Element::elem_erg_table->get_column_name(label::measure_id) << " = " << id.measure_nr ;
	query_string  << " AND ";
	query_string  <<  Dam_Ecn_Element::elem_erg_table->get_column_name(hyd_label::sz_bound_id) << " = " << hyd_sc ;
	query_string  << " AND ";
	query_string  << Dam_Ecn_Element::elem_erg_table->get_column_name(label::applied_flag) << " = "<< functions::convert_boolean2string(!flag);

	Data_Base::database_request(&query, query_string.str(), ptr_database);
	if(query.lastError().isValid()==true){
		Error msg;
		msg.set_msg("Dam_Ecn_Element::switch_applied_flag_erg_table(QSqlDatabase *ptr_database, const _sys_system_id id, const int hyd_sc, const bool flag)","Invalid database request", "Check the database", 2, false);
		ostringstream info;
		info << "Table Name      : " << Dam_Ecn_Element::elem_erg_table->get_table_name() << endl;
		info << "Table error info: " << query.lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
}
//Switch the applied-flag for the economic damage elements in the database table for a defined system state (static)
void Dam_Ecn_Element::switch_applied_flag_elem_table(QSqlDatabase *ptr_database, const _sys_system_id id, const bool flag){
	//the table is set (the name and the column names) and allocated
	try{
		Dam_Ecn_Element::set_elem_table(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}
	QSqlQueryModel query;

	ostringstream query_string;
	query_string <<"UPDATE ";
	query_string << Dam_Ecn_Element::elem_table->get_table_name();
	query_string << " SET " ;
	query_string  << Dam_Ecn_Element::elem_table->get_column_name(label::applied_flag) << " = "<< functions::convert_boolean2string(flag);
	query_string  << " WHERE ";
	query_string  << Dam_Ecn_Element::elem_table->get_column_name(label::areastate_id) << " = "  << id.area_state;
	query_string  << " AND ";
	query_string  <<  Dam_Ecn_Element::elem_table->get_column_name(label::measure_id) << " = " << id.measure_nr ;
	query_string  << " AND ";
	query_string  << Dam_Ecn_Element::elem_table->get_column_name(label::applied_flag) << " = "<< functions::convert_boolean2string(!flag);

	Data_Base::database_request(&query, query_string.str(), ptr_database);
	if(query.lastError().isValid()==true){
		Error msg;
		msg.set_msg("Dam_Ecn_Element::switch_applied_flag_elem_table(QSqlDatabase *ptr_database, const _sys_system_id id, const bool flag)","Invalid database request", "Check the database", 2, false);
		ostringstream info;
		info << "Table Name      : " << Dam_Ecn_Element::elem_table->get_table_name() << endl;
		info << "Table error info: " << query.lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
}
//Switch the applied-flag for the economic damage elements in the database table for a defined system state (static)
void Dam_Ecn_Element::switch_applied_flag_elem_table(QSqlDatabase *ptr_database, const _sys_system_id id, const int elem_id, const int raster_id, const bool flag){
	//the table is set (the name and the column names) and allocated
	try{
		Dam_Ecn_Element::set_elem_table(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}
	QSqlQueryModel query;

	ostringstream query_string;
	query_string <<"UPDATE ";
	query_string << Dam_Ecn_Element::elem_table->get_table_name();
	query_string << " SET " ;
	query_string  << Dam_Ecn_Element::elem_table->get_column_name(label::applied_flag) << " = "<< functions::convert_boolean2string(flag);
	query_string  << " WHERE ";
	query_string  << Dam_Ecn_Element::elem_table->get_column_name(label::areastate_id) << " = "  << id.area_state;
	query_string  << " AND ";
	query_string  <<  Dam_Ecn_Element::elem_table->get_column_name(label::measure_id) << " = " << id.measure_nr ;
	query_string  << " AND ";
	query_string  << Dam_Ecn_Element::elem_table->get_column_name(dam_label::elem_id) << " = "<< elem_id;
	query_string  << " AND ";
	query_string  << Dam_Ecn_Element::elem_table->get_column_name(dam_label::raster_id) << " = "<< raster_id;

	Data_Base::database_request(&query, query_string.str(), ptr_database);
	if(query.lastError().isValid()==true){
		Error msg;
		msg.set_msg("Dam_Ecn_Element::switch_applied_flag_elem_table(QSqlDatabase *ptr_database, const _sys_system_id id, const int elem_id, const int raster_id, const bool flag)","Invalid database request", "Check the database", 2, false);
		ostringstream info;
		info << "Table Name      : " << Dam_Ecn_Element::elem_table->get_table_name() << endl;
		info << "Table error info: " << query.lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
}
//Reswitch the applied-flag for the economic damage elements in the database table for the base state; a defined system state and his applied elements are decisive (static)
void Dam_Ecn_Element::reswitch_applied_flag_elem_table(QSqlDatabase *ptr_database, const _sys_system_id id, const bool flag){
	int number=0;
	try{
		Dam_Ecn_Element::set_elem_table(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}

	QSqlQueryModel results;
	ostringstream test_filter;
	test_filter<< "Select ";
	test_filter <<  Dam_Ecn_Element::elem_table->get_column_name(dam_label::elem_id) << " , ";
	test_filter <<  Dam_Ecn_Element::elem_table->get_column_name(dam_label::raster_id) << "  ";

	test_filter << " from " << Dam_Ecn_Element::elem_table->get_table_name();
	test_filter << " where " ;
	test_filter << Dam_Ecn_Element::elem_table->get_column_name(label::applied_flag) << "= true";
	test_filter << " and ";
	test_filter << Dam_Ecn_Element::elem_table->get_column_name(label::areastate_id) << " ="  << id.area_state;
	test_filter << " and ";
	test_filter << Dam_Ecn_Element::elem_table->get_column_name(label::measure_id) << " = " << id.measure_nr;

	Data_Base::database_request(&results, test_filter.str(), ptr_database);

	if(results.lastError().isValid()==true){
		Error msg;
		msg.set_msg("Dam_Ecn_Element::reswitch_applied_flag_elem_table(QSqlDatabase *ptr_database, const _sys_system_id id, const bool flag)","Invalid database request", "Check the database", 2, false);
		ostringstream info;
		info << "Table Name      : " << Dam_Ecn_Element::elem_table->get_table_name() << endl;
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
		rast_id=results.record(i).value((Dam_Ecn_Element::elem_table->get_column_name(dam_label::raster_id)).c_str()).toInt();
		elem_id=results.record(i).value((Dam_Ecn_Element::elem_table->get_column_name(dam_label::elem_id)).c_str()).toInt();

		Dam_Ecn_Element::switch_applied_flag_elem_table(ptr_database, base, elem_id, rast_id, flag);
	}
}
//Copy the results of a given system id to another one in database tables (static)
void Dam_Ecn_Element::copy_results(QSqlDatabase *ptr_database, const _sys_system_id src, const _sys_system_id dest){
//the table is set (the name and the column names) and allocated
	try{
		Dam_Ecn_Element::set_erg_table(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}

	int glob_id=0;
	glob_id=Dam_Ecn_Element::elem_erg_table->maximum_int_of_column(Dam_Ecn_Element::elem_erg_table->get_column_name(dam_label::glob_id) ,ptr_database)+1;

	QSqlQueryModel model;
	ostringstream test_filter;
	test_filter<< "SELECT ";
	test_filter << Dam_Ecn_Element::elem_erg_table->get_column_name(dam_label::glob_id);
	test_filter << " from " << Dam_Ecn_Element::elem_erg_table->get_table_name();
	test_filter << " WHERE ";
	test_filter << Dam_Ecn_Element::elem_erg_table->get_column_name(label::areastate_id) << " = " << src.area_state;
	test_filter << " AND ";
	test_filter << Dam_Ecn_Element::elem_erg_table->get_column_name(label::measure_id) << " = " << src.measure_nr ;

	//submit it to the datbase
	Data_Base::database_request(&model, test_filter.str(), ptr_database);
	if(model.lastError().isValid()==true){
		Error msg;
		msg.set_msg("Dam_Ecn_Element::copy_results(QSqlDatabase *ptr_database, const _sys_system_id src, const _sys_system_id dest)","Invalid database request", "Check the database", 2, false);
		ostringstream info;
		info << "Table Name      : " << Dam_Ecn_Element::elem_erg_table->get_table_name() << endl;
		info << "Table error info: " << model.lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
	test_filter.str("");

	ostringstream cout;
	cout <<"Copy "<<model.rowCount()<<" element results of the economic damages to the new measure state..."<<endl;
	Sys_Common_Output::output_dam->output_txt(&cout);

	QSqlQueryModel model1;
	for(int i=0; i< model.rowCount(); i++){
		if(i%10000==0 && i>0){
			cout << "Copy ECN-element's results "<< i <<" to " << i+10000 <<"..."<< endl;
			Sys_Common_Output::output_dam->output_txt(&cout);
		}

		test_filter.str("");
		test_filter << "INSERT INTO "<< Dam_Ecn_Element::elem_erg_table->get_table_name();
		test_filter << " SELECT " << glob_id <<" , ";
		test_filter <<  Dam_Ecn_Element::elem_erg_table->get_column_name(dam_label::elem_id)  <<" , ";
		test_filter <<  Dam_Ecn_Element::elem_erg_table->get_column_name(dam_label::raster_id)  <<" , ";
		test_filter <<  dest.area_state  <<" , ";
		test_filter <<  dest.measure_nr  <<" , ";
		test_filter <<  Dam_Ecn_Element::elem_erg_table->get_column_name(label::applied_flag)  <<" , ";
		test_filter <<  Dam_Ecn_Element::elem_erg_table->get_column_name(hyd_label::sz_bound_id)  <<" , ";
		test_filter <<  Dam_Ecn_Element::elem_erg_table->get_column_name(risk_label::sz_break_id)  <<" , ";
		test_filter <<  Dam_Ecn_Element::elem_erg_table->get_column_name(dam_label::immob_dam)  <<" , ";
		test_filter <<  Dam_Ecn_Element::elem_erg_table->get_column_name(dam_label::mob_dam)  <<" , ";
		test_filter <<  Dam_Ecn_Element::elem_erg_table->get_column_name(dam_label::total_dam)  <<" , ";
		test_filter <<  Dam_Ecn_Element::elem_erg_table->get_column_name(dam_label::immob_5_quant)  <<" , ";
		test_filter <<  Dam_Ecn_Element::elem_erg_table->get_column_name(dam_label::immob_95_quant)  <<" , ";
		test_filter <<  Dam_Ecn_Element::elem_erg_table->get_column_name(dam_label::mob_5_quant)  <<" , ";
		test_filter <<  Dam_Ecn_Element::elem_erg_table->get_column_name(dam_label::mob_95_quant)  <<" , ";
		test_filter <<  Dam_Ecn_Element::elem_erg_table->get_column_name(dam_label::elem_poly)  <<"  ";

		test_filter << " FROM " << Dam_Ecn_Element::elem_erg_table->get_table_name() <<" ";
		test_filter << " WHERE " << Dam_Ecn_Element::elem_erg_table->get_column_name(dam_label::glob_id) << " = ";
		test_filter << model.record(i).value(Dam_Ecn_Element::elem_erg_table->get_column_name(dam_label::glob_id).c_str()).toInt();
		Data_Base::database_request(&model1, test_filter.str(), ptr_database);
		if(model1.lastError().isValid()==true){
			Error msg;
			msg.set_msg("Dam_Ecn_Element::copy_results(QSqlDatabase *ptr_database, const _sys_system_id src, const _sys_system_id dest)","Invalid database request", "Check the database", 2, false);
			ostringstream info;
			info << "Table Name      : " << Dam_Ecn_Element::elem_erg_table->get_table_name() << endl;
			info << "Table error info: " << model1.lastError().text().toStdString() << endl;
			msg.make_second_info(info.str());
			throw msg;
		}
		glob_id++;
	}
}
//Select and count the number of relevant damage raster elements in a database table (static)
int Dam_Ecn_Element::select_relevant_elements_database(QSqlQueryModel *results, QSqlDatabase *ptr_database, const _sys_system_id id, const int raster_id, const bool with_output){
	int number=0;
	try{
		Dam_Ecn_Element::set_elem_table(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}
	if(with_output==true){
		ostringstream cout;
		cout << "Select relevant economical raster elements in database ..." << endl ;
		Sys_Common_Output::output_dam->output_txt(&cout);
	}

	ostringstream test_filter;
	test_filter<< "Select ";
	test_filter <<  Dam_Ecn_Element::elem_table->get_column_name(dam_label::elem_id) << " , ";
	test_filter <<  Dam_Ecn_Element::elem_table->get_column_name(dam_label::elem_mid_x) << " , ";
	test_filter <<  Dam_Ecn_Element::elem_table->get_column_name(dam_label::elem_mid_y) << " , ";
	test_filter <<  Dam_Ecn_Element::elem_table->get_column_name(dam_label::conn_fp_id) << " , ";
	test_filter <<  Dam_Ecn_Element::elem_table->get_column_name(dam_label::conn_fp_elem_id) << " , ";
	test_filter <<  Dam_Ecn_Element::elem_table->get_column_name(dam_label::area_elem) << " , ";
	test_filter <<  Dam_Ecn_Element::elem_table->get_column_name(dam_label::glob_id) << " , ";
	test_filter <<  Dam_Ecn_Element::elem_table->get_column_name(dam_label::immob_id) << " , ";
	test_filter <<  Dam_Ecn_Element::elem_table->get_column_name(dam_label::mob_id) << " , ";
	test_filter <<  Dam_Ecn_Element::elem_table->get_column_name(dam_label::immob_stock) << " , ";
	test_filter <<  Dam_Ecn_Element::elem_table->get_column_name(dam_label::mob_stock) << " , ";
	test_filter <<  Dam_Ecn_Element::elem_table->get_column_name(dam_label::no_immob_id) << " , ";
	test_filter <<  Dam_Ecn_Element::elem_table->get_column_name(dam_label::no_mob_id);

	test_filter << " from " << Dam_Ecn_Element::elem_table->get_table_name();
	test_filter << " where " ;
	test_filter << Dam_Ecn_Element::elem_table->get_column_name(label::applied_flag) << "= true";
	test_filter << " and ";
	test_filter << Dam_Ecn_Element::elem_table->get_column_name(label::areastate_id) << " ="  << id.area_state;
	test_filter << " and (";
	test_filter << Dam_Ecn_Element::elem_table->get_column_name(label::measure_id) << " = " << 0 ;
	test_filter << " or " ;
	test_filter << Dam_Ecn_Element::elem_table->get_column_name(label::measure_id) << " = " << id.measure_nr;
	test_filter << " ) " ;
	test_filter << " and ";
	test_filter << Dam_Ecn_Element::elem_table->get_column_name(dam_label::raster_id) << " = " << raster_id;
	test_filter << " order by " <<  Dam_Ecn_Element::elem_table->get_column_name(dam_label::elem_id);

	Data_Base::database_request(results, test_filter.str(), ptr_database);

	//check the request
	if(results->lastError().isValid()){
		Error msg;
		msg.set_msg("Dam_Ecn_Element::select_relevant_elements_database(QSqlTableModel *results, const _sys_system_id id, const int raster_id, const bool with_output)","Invalid database request", "Check the database", 2, false);
		ostringstream info;
		info << "Table Name      : " << Dam_Ecn_Element::elem_table->get_table_name() << endl;
		info << "Table error info: " << results->lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}

	number= results->rowCount();

	if(with_output==true){
		ostringstream cout;
		cout << number << " relevant economical raster elements(s) are selected in database" << endl ;
		Sys_Common_Output::output_dam->output_txt(&cout);
	}

	return number;
}
//Select and count the number of relevant damage raster elements in a database table (just part of it) (static)
int Dam_Ecn_Element::select_relevant_elements_database(QSqlQueryModel *results, QSqlDatabase *ptr_database, const _sys_system_id id, const int raster_id,  const int offset, const int number_rows, const bool with_output){
	int number=0;
	try{
		Dam_Ecn_Element::set_elem_table(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}
	if(with_output==true){
		ostringstream cout;
		cout << "Select relevant economical raster elements in database ..." << endl ;
		Sys_Common_Output::output_dam->output_txt(&cout);
	}

	ostringstream test_filter;
	test_filter<< "Select ";
	test_filter <<  Dam_Ecn_Element::elem_table->get_column_name(dam_label::elem_id) << " , ";
	test_filter <<  Dam_Ecn_Element::elem_table->get_column_name(dam_label::elem_mid_x) << " , ";
	test_filter <<  Dam_Ecn_Element::elem_table->get_column_name(dam_label::elem_mid_y) << " , ";
	test_filter <<  Dam_Ecn_Element::elem_table->get_column_name(dam_label::conn_fp_id) << " , ";
	test_filter <<  Dam_Ecn_Element::elem_table->get_column_name(dam_label::conn_fp_elem_id) << " , ";
	test_filter <<  Dam_Ecn_Element::elem_table->get_column_name(dam_label::area_elem) << " , ";
	test_filter <<  Dam_Ecn_Element::elem_table->get_column_name(dam_label::glob_id) << " , ";
	test_filter <<  Dam_Ecn_Element::elem_table->get_column_name(dam_label::immob_id) << " , ";
	test_filter <<  Dam_Ecn_Element::elem_table->get_column_name(dam_label::mob_id) << " , ";
	test_filter <<  Dam_Ecn_Element::elem_table->get_column_name(dam_label::immob_stock) << " , ";
	test_filter <<  Dam_Ecn_Element::elem_table->get_column_name(dam_label::mob_stock) << " , ";
	test_filter <<  Dam_Ecn_Element::elem_table->get_column_name(dam_label::no_immob_id) << " , ";
	test_filter <<  Dam_Ecn_Element::elem_table->get_column_name(dam_label::no_mob_id);

	test_filter << " from " << Dam_Ecn_Element::elem_table->get_table_name();
	test_filter << " where " ;
	test_filter << Dam_Ecn_Element::elem_table->get_column_name(label::applied_flag) << "= true";
	test_filter << " and ";
	test_filter << Dam_Ecn_Element::elem_table->get_column_name(label::areastate_id) << " ="  << id.area_state;
	test_filter << " and (";
	test_filter << Dam_Ecn_Element::elem_table->get_column_name(label::measure_id) << " = " << 0 ;
	test_filter << " or " ;
	test_filter << Dam_Ecn_Element::elem_table->get_column_name(label::measure_id) << " = " << id.measure_nr;
	test_filter << " ) " ;
	test_filter << " and ";
	test_filter << Dam_Ecn_Element::elem_table->get_column_name(dam_label::raster_id) << " = " << raster_id;
	test_filter << " order by " <<  Dam_Ecn_Element::elem_table->get_column_name(dam_label::elem_id);
	test_filter << " LIMIT " << number_rows;
	test_filter << " OFFSET " << offset;

	Data_Base::database_request(results, test_filter.str(), ptr_database);

	//check the request
	if(results->lastError().isValid()){
		Error msg;
		msg.set_msg("Dam_Ecn_Element::select_relevant_elements_database(QSqlTableModel *results, const _sys_system_id id, const int raster_id, const int offset, const int number_rows, const bool with_output)","Invalid database request", "Check the database", 2, false);
		ostringstream info;
		info << "Table Name      : " << Dam_Ecn_Element::elem_table->get_table_name() << endl;
		info << "Table error info: " << results->lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}

	number= results->rowCount();

	if(with_output==true){
		ostringstream cout;
		cout << number << " relevant economical raster elements(s) are selected in database" << endl ;
		Sys_Common_Output::output_dam->output_txt(&cout);
	}

	return number;
}
//Count the number of relevant damage raster elements in a database table (static)
int Dam_Ecn_Element::count_relevant_elements_database(QSqlQueryModel *results, QSqlDatabase *ptr_database, const _sys_system_id id, const int raster_id, const bool with_output){
	int number=0;
	try{
		Dam_Ecn_Element::set_elem_table(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}
	if(with_output==true){
		ostringstream cout;
		cout << "Count relevant economical raster elements in database ..." << endl ;
		Sys_Common_Output::output_dam->output_txt(&cout);
	}

	ostringstream test_filter;
	test_filter<< "Select COUNT(";
	test_filter <<  Dam_Ecn_Element::elem_table->get_column_name(dam_label::elem_id) << " ) ";

	test_filter << " from " << Dam_Ecn_Element::elem_table->get_table_name();
	test_filter << " where " ;
	test_filter << Dam_Ecn_Element::elem_table->get_column_name(label::applied_flag) << "= true";
	test_filter << " and ";
	test_filter << Dam_Ecn_Element::elem_table->get_column_name(label::areastate_id) << " ="  << id.area_state;
	test_filter << " and (";
	test_filter << Dam_Ecn_Element::elem_table->get_column_name(label::measure_id) << " = " << 0 ;
	test_filter << " or " ;
	test_filter << Dam_Ecn_Element::elem_table->get_column_name(label::measure_id) << " = " << id.measure_nr;
	test_filter << " ) " ;
	test_filter << " and ";
	test_filter << Dam_Ecn_Element::elem_table->get_column_name(dam_label::raster_id) << " = " << raster_id;

	Data_Base::database_request(results, test_filter.str(), ptr_database);

	//check the request
	if(results->lastError().isValid()){
		Error msg;
		msg.set_msg("Dam_Ecn_Element::count_relevant_elements_database(QSqlTableModel *results, const _sys_system_id id, const int raster_id, const bool with_output)","Invalid database request", "Check the database", 2, false);
		ostringstream info;
		info << "Table Name      : " << Dam_Ecn_Element::elem_table->get_table_name() << endl;
		info << "Table error info: " << results->lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}

	if(results->rowCount()>0){
		number= results->record(0).field(0).value().toInt();
	}

	if(with_output==true){
		ostringstream cout;
		cout << number << " relevant economical raster elements(s) are found in database" << endl ;
		Sys_Common_Output::output_dam->output_txt(&cout);
	}

	return number;
}
//________
//ptivate
//Set warning(s)
Warning Dam_Ecn_Element::set_warning(const int warn_type){
	string place="Dam_Ecn_Element::";
		string help;
		string reason;
		string reaction;
		int type=0;
		Warning msg;
		stringstream info;

	switch (warn_type){
		case 0://input datas can not submitted
			place.append("transfer_input_members2database(QSqlDatabase *ptr_database, const int elem_index, const int raster_index)");
			reason="Can not submit the element information of the economical damage raster to the database";
			help="Check the database";
			type=2;
			break;
		case 1://interception datas can not submitted
			place.append("set_interception_elem_data2database(QSqlDatabase *ptr_database, const int raster_id)");
			reason="Can not submit the element information of the economical damage raster evaluated by the raster interception to the database";
			help="Check the database";
			type=2;
			break;
		case 2://results datas can not submitted
			place.append("output_result_members2database(QSqlDatabase *ptr_database, const int bound_sz, const string break_sz, const int raster_id, const double cellsize, const double x_origin, const double y_origin)");
			reason="Can not submit the element results data of the economical damage raster to the database";
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
Error Dam_Ecn_Element::set_error(const int ){
		string place="Dam_Ecn_Element::";
		string help;
		string reason;
		int type=0;
		bool fatal=false;
		stringstream info;
		Error msg;

	//switch (err_type){
	//	//case 0://no matching global id set is found for the given river profile
	//	//	place.append("delete_data_in_erg_table(QSqlDatabase *ptr_database, const _sys_system_id id, const int bound_sz, const string break_sz )") ;
	//	//	reason="Can not delete the data in the element results table";
	//	//	help="Check the economical element result table";
	//	//	type=2;
	//	//	break;
	//	default:
	//		place.append("set_error(const int err_type)");
	//		reason ="Unknown flag!";
	//		help="Check the flags";
	//		type=6;
	//}
	msg.set_msg(place, reason, help, type, fatal);
	msg.make_second_info(info.str());
	return msg;
}
