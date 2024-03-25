#include "Dam_Headers_Precompiled.h"
//#include "Dam_Pys_Element.h"

//init static members
Tables *Dam_Pys_Element::elem_table=NULL;
Tables *Dam_Pys_Element::elem_erg_table=NULL;

//Default constructor
Dam_Pys_Element::Dam_Pys_Element(void){
	this->id_category=-9999;
	this->ptr_category=NULL;
	this->pop_density=0.0;
	this->crit_age50_59=0.0;
	this->crit_age80=0.0;
	this->crit_female=0.0;
	this->crit_owner=0.0;
	this->ptr_density_func=NULL;

	this->reset_result_value();

	this->no_info_elem=false;

	//count the memory
	Sys_Memory_Count::self()->add_mem(sizeof(Dam_Pys_Element), _sys_system_modules::DAM_SYS);
}
//Default destructor
Dam_Pys_Element::~Dam_Pys_Element(void){
	//count the memory
	Sys_Memory_Count::self()->minus_mem(sizeof(Dam_Pys_Element), _sys_system_modules::DAM_SYS);
}
//_______
//public
//Transfer the psycho-social raster element data to a database
void Dam_Pys_Element::transfer_input_members2database(QSqlDatabase *ptr_database, const int raster_index, const double cellsize, const double x_origin, const double y_origin){
		//mysql query with the query-class
	QSqlQuery model(*ptr_database);
	//the table is set (the name and the column names) and allocated
	try{
		Dam_Pys_Element::set_elem_table(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}

	//evaluate the global identifier
	int id_glob=Dam_Pys_Element::elem_table->maximum_int_of_column(Dam_Pys_Element::elem_table->get_column_name(dam_label::glob_id),ptr_database)+1;
	this->global_index=id_glob;
	//set the query via a query string
	ostringstream query_string;
	query_string << "INSERT INTO  " << Dam_Pys_Element::elem_table->get_table_name();
	query_string <<" ( ";
	query_string << Dam_Pys_Element::elem_table->get_column_name(dam_label::glob_id) <<" , ";
	query_string << Dam_Pys_Element::elem_table->get_column_name(dam_label::elem_id) <<" , ";
	query_string << Dam_Pys_Element::elem_table->get_column_name(dam_label::raster_id) <<" , ";
	query_string << Dam_Pys_Element::elem_table->get_column_name(label::areastate_id) <<" , ";
	query_string << Dam_Pys_Element::elem_table->get_column_name(label::measure_id) <<" , ";
	query_string << Dam_Pys_Element::elem_table->get_column_name(label::applied_flag) <<" , ";
	//data
	query_string << Dam_Pys_Element::elem_table->get_column_name(dam_label::category_id) <<" , ";
	query_string << Dam_Pys_Element::elem_table->get_column_name(dam_label::crit_age_50_59) <<" , ";
	query_string << Dam_Pys_Element::elem_table->get_column_name(dam_label::crit_age_80) <<" , ";
	query_string << Dam_Pys_Element::elem_table->get_column_name(dam_label::crit_female) <<" , ";
	query_string << Dam_Pys_Element::elem_table->get_column_name(dam_label::crit_owner) <<" , ";
	query_string << Dam_Pys_Element::elem_table->get_column_name(dam_label::pop_density) <<" , ";

	query_string << Dam_Pys_Element::elem_table->get_column_name(dam_label::elem_mid_x) <<" , ";
	query_string << Dam_Pys_Element::elem_table->get_column_name(dam_label::elem_mid_y) <<" , ";
	query_string << Dam_Pys_Element::elem_table->get_column_name(dam_label::area_elem) <<" , ";
	query_string << Dam_Pys_Element::elem_table->get_column_name(dam_label::conn_fp_id) <<" , ";
	query_string << Dam_Pys_Element::elem_table->get_column_name(dam_label::conn_fp_elem_id) <<" , ";
	query_string << Dam_Pys_Element::elem_table->get_column_name(dam_label::no_category_id) <<" , ";

	query_string << Dam_Pys_Element::elem_table->get_column_name(dam_label::elem_poly) <<" ) ";

	query_string << " VALUES ( ";
	query_string << id_glob << " , " ;
	query_string << this->elem_index<< " , " ;
	query_string << raster_index<< " , " ;
	query_string << this->system_id.area_state << " , " ;
	query_string << this->system_id.measure_nr << " , " ;
	query_string << "true" << " , " ;
	//data
	query_string << this->id_category<< " , " ;
	query_string << this->crit_age50_59<< " , " ;
	query_string << this->crit_age80<< " , " ;
	query_string << this->crit_female<< " , " ;
	query_string << this->crit_owner<< " , " ;
	query_string << this->pop_density<< " , " ;
	query_string << FORMAT_FIXED_REAL << P(8);
	query_string << this->mid_point.get_xcoordinate() << " , " ;
	query_string << this->mid_point.get_ycoordinate() << " , " ;
	query_string << this->area << " , " ;

	query_string << this->index_fp << " , " ;
	query_string << this->index_fp_elem << " , " ;
	query_string << ""<<functions::convert_boolean2string(this->no_info_elem) << " , " ;

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
		Warning msg=this->set_warning(1);
		ostringstream info;
		info << "Table Name                : " << Dam_Pys_Element::elem_table->get_table_name() << endl;
		info << "Table error info          : " << model.lastError().text().toStdString() << endl;
		info << "Element number            : " << this->elem_index << endl;
		msg.make_second_info(info.str());
		msg.output_msg(4);
	}
}
//Get the data-string to complete a insert-string for inserting the data of the element to database
string Dam_Pys_Element::get_datastring_members2database(const int global_id, const int raster_index, const double cellsize, const double x_origin, const double y_origin){
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
	query_string << this->id_category<< " , " ;
	query_string << functions::convert_boolean2string(this->no_info_elem) << " , " ;
	query_string << this->pop_density<< " , " ;
	query_string << this->crit_age50_59<< " , " ;
	query_string << this->crit_age80<< " , " ;
	query_string << this->crit_female<< " , " ;
	query_string << this->crit_owner<< " , " ;
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
//Input the psycho-social damage raster element data per database
void Dam_Pys_Element::input_element_perdatabase(const QSqlQueryModel *results, const int glob_index){
	try{
		this->elem_index=results->record(glob_index).value((Dam_Pys_Element::elem_table->get_column_name(dam_label::elem_id)).c_str()).toInt();
		//set the element members
		this->id_category=results->record(glob_index).value((Dam_Pys_Element::elem_table->get_column_name(dam_label::category_id)).c_str()).toInt();
		this->crit_age50_59=results->record(glob_index).value((Dam_Pys_Element::elem_table->get_column_name(dam_label::crit_age_50_59)).c_str()).toDouble();
		this->crit_age80=results->record(glob_index).value((Dam_Pys_Element::elem_table->get_column_name(dam_label::crit_age_80)).c_str()).toDouble();
		this->crit_female=results->record(glob_index).value((Dam_Pys_Element::elem_table->get_column_name(dam_label::crit_female)).c_str()).toDouble();
		this->crit_owner=results->record(glob_index).value((Dam_Pys_Element::elem_table->get_column_name(dam_label::crit_owner)).c_str()).toDouble();
		this->index_fp=results->record(glob_index).value((Dam_Pys_Element::elem_table->get_column_name(dam_label::conn_fp_id)).c_str()).toInt();
		this->index_fp_elem=results->record(glob_index).value((Dam_Pys_Element::elem_table->get_column_name(dam_label::conn_fp_elem_id)).c_str()).toInt();
		this->pop_density=results->record(glob_index).value((Dam_Pys_Element::elem_table->get_column_name(dam_label::pop_density)).c_str()).toDouble();
		this->no_info_elem=results->record(glob_index).value((Dam_Pys_Element::elem_table->get_column_name(dam_label::no_category_id)).c_str()).toBool();
		this->area=results->record(glob_index).value((Dam_Pys_Element::elem_table->get_column_name(dam_label::area_elem)).c_str()).toDouble();
		double x_mid=0.0;
		double y_mid=0.0;
		x_mid=results->record(glob_index).value((Dam_Pys_Element::elem_table->get_column_name(dam_label::elem_mid_x)).c_str()).toDouble();
		y_mid=results->record(glob_index).value((Dam_Pys_Element::elem_table->get_column_name(dam_label::elem_mid_y)).c_str()).toDouble();
		this->mid_point.set_point_coordinate(x_mid, y_mid);
		this->global_index=results->record(glob_index).value((Dam_Pys_Element::elem_table->get_column_name(dam_label::glob_id)).c_str()).toInt();
	}
	catch(Error msg){
		ostringstream info;
		info << "Element index            : " << this->elem_index << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
}
//Set the element data from the raster interception to the database table: identifier of the floodplain, -floodplain element and the reduced area
void Dam_Pys_Element::set_interception_elem_data2database(QSqlDatabase *ptr_database, QSqlQuery *elem_results, const int raster_id){
	//nothing to update
	if(this->area!=0.0 && this->index_fp<0 && this->index_fp_elem<0){
		return;
	}
	try{
		Dam_Pys_Element::set_elem_table(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}

	//generate the filter
	ostringstream filter;
	filter << "UPDATE " << Dam_Pys_Element::elem_table->get_table_name().c_str();
	filter << " SET ";
	filter << Dam_Pys_Element::elem_table->get_column_name(dam_label::conn_fp_id).c_str() << " = " << this->index_fp << " , ";
	filter << Dam_Pys_Element::elem_table->get_column_name(dam_label::conn_fp_elem_id).c_str() << " = " << this->index_fp_elem << " , ";
	filter << Dam_Pys_Element::elem_table->get_column_name(dam_label::area_elem).c_str() << " = " << this->area;
	filter << " WHERE " ;
	filter << Dam_Pys_Element::elem_table->get_column_name(dam_label::glob_id) << " = " << this->global_index;

	Data_Base::database_request(elem_results, filter.str(), ptr_database);
	if(elem_results->lastError().isValid()){
		Warning msg=this->set_warning(0);
		ostringstream info;
		info << "Table Name      : " << Dam_Pys_Element::elem_table->get_table_name() << endl;
		info << "Table error info: " << elem_results->lastError().text().toStdString() << endl;
		info << "Raster-id       : "  << raster_id <<endl;
		info << "Element-id      : " << this->elem_index << endl;
		msg.make_second_info(info.str());
		msg.output_msg(4);
	}
}
//Get a string to set the element data from the raster interception to the database table: identifier of the floodplain, -floodplain element and the reduced area (static)
bool Dam_Pys_Element::get_string_interception_elem_data2database(ostringstream *text){
//nothing to update
	if(this->area!=0.0 && this->index_fp<0 && this->index_fp_elem<0){
		return false;
	}

	//generate the filter

	*text << "UPDATE " << Dam_Pys_Element::elem_table->get_table_name().c_str();
	*text << " SET ";
	*text << Dam_Pys_Element::elem_table->get_column_name(dam_label::conn_fp_id).c_str() << " = " << this->index_fp << " , ";
	*text << Dam_Pys_Element::elem_table->get_column_name(dam_label::conn_fp_elem_id).c_str() << " = " << this->index_fp_elem << " , ";
	*text << Dam_Pys_Element::elem_table->get_column_name(dam_label::area_elem).c_str() << " = " << this->area;
	*text << " WHERE " ;
	*text << Dam_Pys_Element::elem_table->get_column_name(dam_label::glob_id) << " = " << this->global_index<<"; ";

	return true;
}
//Set the index of the psycho-social category
void Dam_Pys_Element::set_id_category(const int id){
	this->id_category=id;
}
//Set the individual stock value of the population density
void Dam_Pys_Element::set_population_density(const double stock_pop){
	this->pop_density=stock_pop;
}
//Set the value of the psycho-social criteria age class 50 to 59
void Dam_Pys_Element::set_criteria_age_50_59(const double value){
	this->crit_age50_59=value;
}
//Set the value of the psycho-social criteria age class greater 80
void Dam_Pys_Element::set_criteria_age_80(const double value){
	this->crit_age80=value;
}
//Set the value of the psycho-social criteria female gender
void Dam_Pys_Element::set_criteria_female(const double value){
	this->crit_female=value;
}
//Set the value of the psycho-social criteria home owner
void Dam_Pys_Element::set_criteria_owner(const double value){
	this->crit_owner=value;
}
//Get the index of the psycho-social category
int Dam_Pys_Element::get_id_category(void){
	return this->id_category;
}
//Get the individual stock value of the population density
double Dam_Pys_Element::get_population_density(void){
	return this->pop_density;
}
//Get the value of the psycho-social criteria age class 50 to 59
double Dam_Pys_Element::get_criteria_age_50_59(void){
	return this->crit_age50_59;
}
//Get the value of the psycho-social criteria age class greater 80
double Dam_Pys_Element::get_criteria_age_80(void){
	return this->crit_age80;
}
//Get the value of the psycho-social criteria age female gender
double Dam_Pys_Element::get_criteria_female(void){
	return this->crit_female;
}
//Get the value of the psycho-social criteria home owner
double Dam_Pys_Element::get_criteria_owner(void){
	return this->crit_owner;
}
//Output the damage members
void Dam_Pys_Element::output_damage_members(ostringstream *txt){
	*txt << this->id_category ;
	*txt << W(19) << P(2)<< FORMAT_SCIENTIFIC_REAL<< this->pop_density;
	*txt << W(19) << P(2)<< FORMAT_FIXED_REAL << this->crit_age50_59 ;
	*txt << W(19) << P(2)<< FORMAT_FIXED_REAL << this->crit_age80 ;
	*txt << W(19) << P(2)<< FORMAT_FIXED_REAL << this->crit_female ;
	*txt << W(19) << P(2)<< FORMAT_FIXED_REAL << this->crit_owner ;
}
//Connect the element to the psycho-social category
bool Dam_Pys_Element::connect_element2category(const int no_info, Dam_Pys_Category *category){
	bool flag=false;

	if(this->no_info_elem==true){
		flag=true;
	}
	else if(this->id_category==no_info){
		this->no_info_elem=true;
		flag=true;
	}
	else if(this->id_category==category->get_index()){
		this->ptr_category=category;
		flag=true;
	}
	return flag;
}
//Get a flag if the psycho-social category is found
bool Dam_Pys_Element::category_function_is_found(void){
	if(this->no_info_elem==true){
		return true;
	}
	if(this->ptr_category!=NULL){
		return true;
	}
	return false;
}
//Set the pointer to the density function
void Dam_Pys_Element::set_ptr_density_function(Dam_Pys_Density_Function *ptr_function){
	this->ptr_density_func=ptr_function;
}
//Set the element to a no-info element concerning psycho-social damage calculation
void Dam_Pys_Element::set_noinfo_elem(void){
	this->no_info_elem=true;
}
//Calculate the psycho-social damages per element
void Dam_Pys_Element::calculate_damages(Dam_Impact_Values *impact){
	this->reset_result_value();
	double dens_factor=1.0;
	if(impact->get_was_wet_flag()==false){
		return;
	}

	if(this->no_info_elem==false){
		//density
		if(this->ptr_density_func!=NULL){
			if(this->ptr_density_func->get_function_is_set()==true){
				dens_factor=this->ptr_density_func->get_y_value(this->pop_density);
			}
			else{
				dens_factor=1.0;
			}
		}
		//crit age 50 to 59
		this->res_crit_age50_59.score_value=this->area*this->ptr_category->get_score_value(this->crit_age50_59,_dam_pys_criteria::crit_age_50_59);
		this->res_crit_age50_59.score_pop_value=this->res_crit_age50_59.score_value*dens_factor;

		//crit age greater 80
		this->res_crit_age80.score_value=this->area*this->ptr_category->get_score_value(this->crit_age80,_dam_pys_criteria::crit_age_80);
		this->res_crit_age80.score_pop_value=this->res_crit_age80.score_value*dens_factor;

		//crit age female gender
		this->res_crit_female.score_value=this->area*this->ptr_category->get_score_value(this->crit_female,_dam_pys_criteria::crit_female);
		this->res_crit_female.score_pop_value=this->res_crit_female.score_value*dens_factor;

		//crit age home owners
		this->res_crit_owner.score_value=this->area*this->ptr_category->get_score_value(this->crit_owner,_dam_pys_criteria::crit_owner);
		this->res_crit_owner.score_pop_value=this->res_crit_owner.score_value*dens_factor;
	}
}
//Get the data-string to complete a insert-string for inserting the data of the element results to database
string Dam_Pys_Element::get_datastring_results2database(const int global_id, const int bound_sz, const string break_sz, const int raster_id, const double cellsize, const double x_origin, const double y_origin, const bool must_output){
	string buffer=label::not_set;
	if(must_output==false){
		if(this->res_crit_age50_59.score_value < constant::zero_epsilon && this->res_crit_age80.score_value< constant::zero_epsilon &&
			this->res_crit_female.score_value < constant::zero_epsilon && this->res_crit_owner.score_value< constant::zero_epsilon){
			return buffer;
		}
	}

	//set the query via a query string
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
	query_string << this->res_crit_age50_59.score_value << " , " ;
	query_string << this->res_crit_age80.score_value << " , " ;
	query_string << this->res_crit_female.score_value << " , " ;
	query_string << this->res_crit_owner.score_value << " , " ;
	query_string << (this->res_crit_owner.score_value+this->res_crit_female.score_value+ this->res_crit_age80.score_value+this->res_crit_age50_59.score_value)<< " , " ;
	query_string << this->res_crit_age50_59.score_pop_value << " , " ;
	query_string << this->res_crit_age80.score_pop_value << " , " ;
	query_string << this->res_crit_female.score_pop_value << " , " ;
	query_string << this->res_crit_owner.score_pop_value << " , " ;
	query_string << (this->res_crit_owner.score_pop_value+this->res_crit_female.score_pop_value+ this->res_crit_age80.score_pop_value+this->res_crit_age50_59.score_pop_value)<< " , " ;

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
//Get the results for the psycho-social criteria age class 50 to 59
_dam_pys_result Dam_Pys_Element::get_result_crit_age50_59(void){
	return this->res_crit_age50_59;
}
//Get the results for the psycho-social criteria age class greater 80
_dam_pys_result Dam_Pys_Element::get_result_crit_age80(void){
	return this->res_crit_age80;
}
//Get the results for the psycho-social criteria female gender
_dam_pys_result Dam_Pys_Element::get_result_crit_female(void){
	return this->res_crit_female;
}
//Get the results for the psycho-social criteria home owner
_dam_pys_result Dam_Pys_Element::get_result_crit_owner(void){
	return this->res_crit_owner;
}
//Create the database table for the raster elements (static)
void Dam_Pys_Element::create_elem_table(QSqlDatabase *ptr_database){
		if(Dam_Pys_Element::elem_table==NULL){
			ostringstream cout;
			cout << "Create psycho-social raster element data database table..." << endl ;
			Sys_Common_Output::output_dam->output_txt(&cout);
			//make specific input for this class
			const string tab_name=dam_label::tab_pys_element;
			const int num_col=19;
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

			tab_col[6].name=dam_label::category_id;
			tab_col[6].type=sys_label::tab_col_type_int;
			tab_col[6].default_value="-1";

			tab_col[7].name=dam_label::crit_age_50_59;
			tab_col[7].type=sys_label::tab_col_type_double;
			tab_col[7].default_value="0.0";

			tab_col[8].name=dam_label::crit_age_80;
			tab_col[8].type=sys_label::tab_col_type_double;
			tab_col[8].default_value="0.0";

			tab_col[9].name=dam_label::crit_female;
			tab_col[9].type=sys_label::tab_col_type_double;
			tab_col[9].default_value="0.0";

			tab_col[10].name=dam_label::crit_owner;
			tab_col[10].type=sys_label::tab_col_type_double;
			tab_col[10].default_value="0.0";

			tab_col[11].name=dam_label::pop_density;
			tab_col[11].type=sys_label::tab_col_type_double;
			tab_col[11].unsigned_flag=true;
			tab_col[11].default_value="0.0";

			tab_col[12].name=dam_label::no_category_id;
			tab_col[12].type=sys_label::tab_col_type_bool;
			tab_col[12].default_value="false";

			tab_col[13].name=dam_label::elem_mid_x;
			tab_col[13].type=sys_label::tab_col_type_double;

			tab_col[14].name=dam_label::elem_mid_y;
			tab_col[14].type=sys_label::tab_col_type_double;

			tab_col[15].name=dam_label::area_elem;
			tab_col[15].type=sys_label::tab_col_type_double;
			tab_col[15].default_value="0.0";

			tab_col[16].name=dam_label::elem_poly;
			tab_col[16].type=sys_label::tab_col_type_polygon;

			tab_col[17].name=dam_label::conn_fp_id;
			tab_col[17].type=sys_label::tab_col_type_int;
			tab_col[17].default_value="-1";

			tab_col[18].name=dam_label::conn_fp_elem_id;
			tab_col[18].type=sys_label::tab_col_type_int;
			tab_col[18].default_value="-1";

			try{
				Dam_Pys_Element::elem_table= new Tables();
				if(Dam_Pys_Element::elem_table->create_non_existing_tables(tab_name, tab_col, num_col,ptr_database, _sys_table_type::dam)==false){
					cout << " Table exists" << endl ;
					Sys_Common_Output::output_dam->output_txt(&cout);
				};
			}
			catch(bad_alloc& t){
				Error msg;
				msg.set_msg("Dam_Pys_Element::create_elem_table(QSqlDatabase *ptr_database)","Can not allocate the memory", "Check the memory", 10, false);
				ostringstream info;
				info<< "Info bad alloc: " << t.what() << endl;
				msg.make_second_info(info.str());
				throw msg;
			}
			catch(Error msg){
				Dam_Pys_Element::close_elem_table();
				throw msg;
			}

		Dam_Pys_Element::close_elem_table();
	}
}
//Set the database table for the raster elements: it sets the table name and the name of the columns and allocate them (static)
void Dam_Pys_Element::set_elem_table(QSqlDatabase *ptr_database){
	if(Dam_Pys_Element::elem_table==NULL){
		//make specific input for this class
		const string tab_id_name=dam_label::tab_pys_element;
		string tab_col[19];

		tab_col[0]=dam_label::glob_id;
		tab_col[1]=dam_label::elem_id;
		tab_col[2]=dam_label::raster_id;
		tab_col[3]=label::areastate_id;
		tab_col[4]=label::measure_id;
		tab_col[5]=dam_label::category_id;
		tab_col[6]=dam_label::crit_age_50_59;
		tab_col[7]=dam_label::pop_density;
		tab_col[8]=dam_label::elem_mid_x;
		tab_col[9]=dam_label::elem_mid_y;
		tab_col[10]=dam_label::elem_poly;
		tab_col[11]=dam_label::crit_age_80;
		tab_col[12]=dam_label::crit_female;
		tab_col[13]=dam_label::crit_owner;
		tab_col[14]=dam_label::no_category_id;
		tab_col[15]=dam_label::area_elem;
		tab_col[16]=dam_label::conn_fp_id;
		tab_col[17]=dam_label::conn_fp_elem_id;
		tab_col[18]=label::applied_flag;

		try{
			Dam_Pys_Element::elem_table= new Tables(tab_id_name, tab_col, sizeof(tab_col)/sizeof(tab_col[0]));
			Dam_Pys_Element::elem_table->set_name(ptr_database, _sys_table_type::dam);
		}
		catch(bad_alloc& t){
			Error msg;
			msg.set_msg("Dam_Pys_Element::set_elem_table(QSqlDatabase *ptr_database)","Can not allocate the memory", "Check the memory", 10, false);
			ostringstream info;
			info<< "Info bad alloc: " << t.what() << endl;
			msg.make_second_info(info.str());
			throw msg;
		}
		catch(Error msg){
			Dam_Pys_Element::close_elem_table();
			throw msg;
		}
	}
}
//Delete all data in the database table for the raster elements (static)
void Dam_Pys_Element::delete_data_in_elem_table(QSqlDatabase *ptr_database){
	//the table is set (the name and the column names) and allocated
	try{
		Dam_Pys_Element::set_elem_table(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}
	//delete the table
	Dam_Pys_Element::elem_table->delete_data_in_table(ptr_database);
}
//Close and delete the database table for the raster elements (static)
void Dam_Pys_Element::close_elem_table(void){
	if(Dam_Pys_Element::elem_table!=NULL){
		delete Dam_Pys_Element::elem_table;
		Dam_Pys_Element::elem_table=NULL;
	}
}
//Get the maximum value of the global index of the element database table (static)
int Dam_Pys_Element::get_max_glob_id_elem_table(QSqlDatabase *ptr_database){
	try{
		Dam_Pys_Element::set_elem_table(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}

	//evaluate the global identifier
	int id_glob=Dam_Pys_Element::elem_table->maximum_int_of_column(Dam_Pys_Element::elem_table->get_column_name(dam_label::glob_id),ptr_database);
	return id_glob;
}
//Get the header for inserting the element data to database table (static)
string Dam_Pys_Element::get_insert_header_elem_table(QSqlDatabase *ptr_database){
	try{
		Dam_Pys_Element::set_elem_table(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}
	//set the query via a query string
	ostringstream query_string;
	query_string << "INSERT INTO  " << Dam_Pys_Element::elem_table->get_table_name();
	query_string <<" ( ";
	query_string << Dam_Pys_Element::elem_table->get_column_name(dam_label::glob_id) <<" , ";
	query_string << Dam_Pys_Element::elem_table->get_column_name(dam_label::elem_id) <<" , ";
	query_string << Dam_Pys_Element::elem_table->get_column_name(dam_label::raster_id) <<" , ";
	query_string << Dam_Pys_Element::elem_table->get_column_name(label::areastate_id) <<" , ";
	query_string << Dam_Pys_Element::elem_table->get_column_name(label::measure_id) <<" , ";
	query_string << Dam_Pys_Element::elem_table->get_column_name(label::applied_flag) <<" , ";
	//data
	query_string << Dam_Pys_Element::elem_table->get_column_name(dam_label::category_id) <<" , ";
	query_string << Dam_Pys_Element::elem_table->get_column_name(dam_label::no_category_id) <<" , ";
	query_string << Dam_Pys_Element::elem_table->get_column_name(dam_label::pop_density) <<" , ";
	query_string << Dam_Pys_Element::elem_table->get_column_name(dam_label::crit_age_50_59) <<" , ";
	query_string << Dam_Pys_Element::elem_table->get_column_name(dam_label::crit_age_80) <<" , ";
	query_string << Dam_Pys_Element::elem_table->get_column_name(dam_label::crit_female) <<" , ";
	query_string << Dam_Pys_Element::elem_table->get_column_name(dam_label::crit_owner) <<" , ";
	query_string << Dam_Pys_Element::elem_table->get_column_name(dam_label::elem_mid_x) <<" , ";
	query_string << Dam_Pys_Element::elem_table->get_column_name(dam_label::elem_mid_y) <<" , ";
	query_string << Dam_Pys_Element::elem_table->get_column_name(dam_label::area_elem) <<" , ";

	query_string << Dam_Pys_Element::elem_table->get_column_name(dam_label::elem_poly) <<" ) ";

	query_string << " VALUES ";
	string buffer;
	buffer=query_string.str();
	return buffer;
}
//Delete all data in the database table for the raster elements of the given raster index (static)
void Dam_Pys_Element::delete_data_in_elem_table(QSqlDatabase *ptr_database, const int raster_id){
	QSqlQueryModel results;
	ostringstream test_filter;
	try{
		Dam_Pys_Element::set_elem_table(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}
	//delete
	test_filter<< "DELETE ";
	test_filter << " FROM " << Dam_Pys_Element::elem_table->get_table_name();
	test_filter << " WHERE ";
	test_filter << Dam_Pys_Element::elem_table->get_column_name(dam_label::raster_id) << " = " << raster_id;

	Data_Base::database_request(&results, test_filter.str(), ptr_database);

	//check the request
	if(results.lastError().isValid()){
		Error msg;
		msg.set_msg("Dam_Pys_Element::delete_data_in_elem_table(QSqlDatabase *ptr_database, const int raster_id)","Invalid database request", "Check the database", 2, false);
		ostringstream info;
		info << "Table Name      : " << Dam_Pys_Element::elem_table->get_table_name() << endl;
		info << "Table error info: " << results.lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
}
//Delete all data in the database table for the raster elements of the given system state (static)
void Dam_Pys_Element::delete_data_in_elem_table(QSqlDatabase *ptr_database, const _sys_system_id id){
	QSqlQueryModel results;
	ostringstream test_filter;
	try{
		Dam_Pys_Element::set_elem_table(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}
	//delete
	test_filter<< "DELETE ";
	test_filter << " FROM " << Dam_Pys_Element::elem_table->get_table_name();
	test_filter << " WHERE ";
	test_filter << Dam_Pys_Element::elem_table->get_column_name(label::areastate_id) << " = " << id.area_state;
	test_filter << " AND ";
	test_filter << Dam_Pys_Element::elem_table->get_column_name(label::measure_id) << " = " << id.measure_nr;

	Data_Base::database_request(&results, test_filter.str(), ptr_database);

	//check the request
	if(results.lastError().isValid()){
		Error msg;
		msg.set_msg("Dam_Pys_Element::delete_data_in_elem_table(QSqlDatabase *ptr_database, const _sys_system_id id)","Invalid database request", "Check the database", 2, false);
		ostringstream info;
		info << "Table Name      : " << Dam_Pys_Element::elem_table->get_table_name() << endl;
		info << "Table error info: " << results.lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
}
//Create the database table for the results of the damage calculation per element (static)
void Dam_Pys_Element::create_erg_table(QSqlDatabase *ptr_database){
		if(Dam_Pys_Element::elem_erg_table==NULL){
			ostringstream cout;
			cout << "Create psycho-social raster element result database table..." << endl ;
			Sys_Common_Output::output_dam->output_txt(&cout);
			//make specific input for this class
			const string tab_name=dam_label::tab_pys_erg;
			const int num_col=19;
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

			tab_col[8].name=dam_label::crit_age_50_59;
			tab_col[8].type=sys_label::tab_col_type_double;
			tab_col[8].default_value="0.0";
			tab_col[8].unsigned_flag=true;

			tab_col[9].name=dam_label::crit_age_80;
			tab_col[9].type=sys_label::tab_col_type_double;
			tab_col[9].default_value="0.0";
			tab_col[9].unsigned_flag=true;

			tab_col[10].name=dam_label::crit_female;
			tab_col[10].type=sys_label::tab_col_type_double;
			tab_col[10].default_value="0.0";
			tab_col[10].unsigned_flag=true;

			tab_col[11].name=dam_label::crit_owner;
			tab_col[11].type=sys_label::tab_col_type_double;
			tab_col[11].default_value="0.0";
			tab_col[11].unsigned_flag=true;

			tab_col[12].name=dam_label::total_score;
			tab_col[12].type=sys_label::tab_col_type_double;
			tab_col[12].default_value="0.0";
			tab_col[12].unsigned_flag=true;

			tab_col[13].name=dam_label::denscrit_age50_59;
			tab_col[13].type=sys_label::tab_col_type_double;
			tab_col[13].default_value="0.0";
			tab_col[13].unsigned_flag=true;

			tab_col[14].name=dam_label::denscrit_age_80;
			tab_col[14].type=sys_label::tab_col_type_double;
			tab_col[14].default_value="0.0";
			tab_col[14].unsigned_flag=true;

			tab_col[15].name=dam_label::denscrit_female;
			tab_col[15].type=sys_label::tab_col_type_double;
			tab_col[15].default_value="0.0";
			tab_col[15].unsigned_flag=true;

			tab_col[16].name=dam_label::denscrit_owner;
			tab_col[16].type=sys_label::tab_col_type_double;
			tab_col[16].default_value="0.0";
			tab_col[16].unsigned_flag=true;

			tab_col[17].name=dam_label::total_dens_score;
			tab_col[17].type=sys_label::tab_col_type_double;
			tab_col[17].default_value="0.0";
			tab_col[17].unsigned_flag=true;

			tab_col[18].name=dam_label::elem_poly;
			tab_col[18].type=sys_label::tab_col_type_polygon;

			try{
				Dam_Pys_Element::elem_erg_table= new Tables();
				if(Dam_Pys_Element::elem_erg_table->create_non_existing_tables(tab_name, tab_col, num_col,ptr_database, _sys_table_type::dam)==false){
					cout << " Table exists" << endl ;
					Sys_Common_Output::output_dam->output_txt(&cout);
				};
			}
			catch(bad_alloc& t){
				Error msg;
				msg.set_msg("Dam_Pys_Element::create_erg_table(QSqlDatabase *ptr_database)","Can not allocate the memory", "Check the memory", 10, false);
				ostringstream info;
				info<< "Info bad alloc: " << t.what() << endl;
				msg.make_second_info(info.str());
				throw msg;
			}
			catch(Error msg){
				Dam_Pys_Element::close_erg_table();
				throw msg;
			}

		Dam_Pys_Element::close_erg_table();
	}
}
//Set the database table for the results of the damage calculation per element: it sets the table name and the name of the columns and allocate them (static)
void Dam_Pys_Element::set_erg_table(QSqlDatabase *ptr_database){
	if(Dam_Pys_Element::elem_erg_table==NULL){
		//make specific input for this class
		const string tab_id_name=dam_label::tab_pys_erg;
		string tab_col[19];

		tab_col[0]=dam_label::glob_id;
		tab_col[1]=dam_label::elem_id;
		tab_col[2]=dam_label::raster_id;
		tab_col[3]=label::areastate_id;
		tab_col[4]=label::measure_id;
		tab_col[5]=hyd_label::sz_bound_id;
		tab_col[6]=risk_label::sz_break_id;
		tab_col[7]=dam_label::crit_age_50_59;
		tab_col[8]=dam_label::crit_age_80;
		tab_col[9]=dam_label::crit_female;
		tab_col[10]=dam_label::crit_owner;
		tab_col[11]=dam_label::denscrit_age50_59;
		tab_col[12]=dam_label::denscrit_age_80;
		tab_col[13]=dam_label::denscrit_female;
		tab_col[14]=dam_label::denscrit_owner;
		tab_col[15]=dam_label::elem_poly;
		tab_col[16]=label::applied_flag;
		tab_col[17]=dam_label::total_dens_score;
		tab_col[18]=dam_label::total_score;

		try{
			Dam_Pys_Element::elem_erg_table= new Tables(tab_id_name, tab_col, sizeof(tab_col)/sizeof(tab_col[0]));
			Dam_Pys_Element::elem_erg_table->set_name(ptr_database, _sys_table_type::dam);
		}
		catch(bad_alloc& t){
			Error msg;
			msg.set_msg("Dam_Pys_Element::set_erg_table(QSqlDatabase *ptr_database)","Can not allocate the memory", "Check the memory", 10, false);
			ostringstream info;
			info<< "Info bad alloc: " << t.what() << endl;
			msg.make_second_info(info.str());
			throw msg;
		}
		catch(Error msg){
			Dam_Pys_Element::close_erg_table();
			throw msg;
		}
	}
}
//Delete all data in the database table for the results of the damage calculation per element (static)
void Dam_Pys_Element::delete_data_in_erg_table(QSqlDatabase *ptr_database){
	//the table is set (the name and the column names) and allocated
	try{
		Dam_Pys_Element::set_erg_table(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}
	//delete the table
	Dam_Pys_Element::elem_erg_table->delete_data_in_table(ptr_database);
}
//Delete the data in the database table for the results of the damage calculation specified by the system state (static)
void Dam_Pys_Element::delete_data_in_erg_table(QSqlDatabase *ptr_database, const _sys_system_id id){
	//the table is set (the name and the column names) and allocated
	try{
		Dam_Pys_Element::set_erg_table(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}

	//delete the table
	QSqlQuery query(*ptr_database);
	ostringstream query_string;
	query_string <<"DELETE FROM " << Dam_Pys_Element::elem_erg_table->get_table_name();
	query_string << " WHERE " ;
	query_string << Dam_Pys_Element::elem_erg_table->get_column_name(label::areastate_id) << " = " << id.area_state;
	query_string << " AND ";
	query_string << Dam_Pys_Element::elem_erg_table->get_column_name(label::measure_id) << " = " << id.measure_nr;

	Data_Base::database_request(&query, query_string.str(), ptr_database);
	if(query.lastError().isValid()==true){
		Error msg;
		msg.set_msg("Dam_Pys_Element::delete_data_in_erg_table(QSqlDatabase *ptr_database, const _sys_system_id id)","Invalid database request", "Check the database", 2, false);
		ostringstream info;
		info << "Table Name      : " << Dam_Pys_Element::elem_erg_table->get_table_name() << endl;
		info << "Table error info: " << query.lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
}
//Delete the data in the database table for the results of the damage calculation specified parameters (static)
void Dam_Pys_Element::delete_data_in_erg_table(QSqlDatabase *ptr_database, const _sys_system_id id, const string break_sz, const bool like_flag){
	//the table is set (the name and the column names) and allocated
	try{
		Dam_Pys_Element::set_erg_table(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}

	//delete the table
	QSqlQuery query(*ptr_database);
	ostringstream query_string;
	query_string <<"DELETE FROM " << Dam_Pys_Element::elem_erg_table->get_table_name();
	query_string << " WHERE " ;
	query_string << Dam_Pys_Element::elem_erg_table->get_column_name(label::areastate_id) << " = " << id.area_state;
	query_string << " AND ";
	query_string << Dam_Pys_Element::elem_erg_table->get_column_name(label::measure_id) << " = " << id.measure_nr;
	query_string << " AND ";
	if(like_flag==false){
		query_string << Dam_Pys_Element::elem_erg_table->get_column_name(risk_label::sz_break_id) << " = '" << break_sz<<"'";
	}
	else{
		query_string << Dam_Pys_Element::elem_erg_table->get_column_name(risk_label::sz_break_id) << " LIKE '" << break_sz<<"'";
	}

	Data_Base::database_request(&query, query_string.str(), ptr_database);
	if(query.lastError().isValid()==true){
		Error msg;
		msg.set_msg("Dam_Pys_Element::delete_data_in_erg_table(QSqlDatabase *ptr_database, const _sys_system_id id, const string break_sz, const bool like_flag)","Invalid database request", "Check the database", 2, false);
		ostringstream info;
		info << "Table Name      : " << Dam_Pys_Element::elem_erg_table->get_table_name() << endl;
		info << "Table error info: " << query.lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
}
//Delete the data in the database table for the results of the damage calculation specified by the system id and the scenario-ids (static)
void Dam_Pys_Element::delete_data_in_erg_table(QSqlDatabase *ptr_database, const _sys_system_id id, const int bound_sz, const string break_sz ){
	//the table is set (the name and the column names) and allocated
	try{
		Dam_Pys_Element::set_erg_table(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}

	//delete the table
	QSqlQuery query(*ptr_database);
	ostringstream query_string;
	query_string <<"DELETE FROM " << Dam_Pys_Element::elem_erg_table->get_table_name();
	query_string << " WHERE " ;
	query_string << Dam_Pys_Element::elem_erg_table->get_column_name(label::areastate_id) << " = " << id.area_state;
	query_string << " AND ";
	query_string << Dam_Pys_Element::elem_erg_table->get_column_name(label::measure_id) << " = " << id.measure_nr;
	query_string << " AND ";
	query_string << Dam_Pys_Element::elem_erg_table->get_column_name(hyd_label::sz_bound_id) << " = " << bound_sz;
	query_string << " AND ";
	query_string << Dam_Pys_Element::elem_erg_table->get_column_name(risk_label::sz_break_id) << " = '" << break_sz<<"'";

	Data_Base::database_request(&query, query_string.str(), ptr_database);
	if(query.lastError().isValid()==true){
		Error msg;
		msg.set_msg("Dam_Pys_Element::delete_data_in_erg_table(QSqlDatabase *ptr_database, const _sys_system_id id, const int bound_sz, const string break_sz )","Invalid database request", "Check the database", 2, false);
		ostringstream info;
		info << "Table Name      : " << Dam_Pys_Element::elem_erg_table->get_table_name() << endl;
		info << "Table error info: " << query.lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
}
//Delete the data in the database table for the results of the damage calculation specified by the hydraulic scenario-ids (static)
void Dam_Pys_Element::delete_data_in_erg_table(QSqlDatabase *ptr_database, const int bound_sz){
	//the table is set (the name and the column names) and allocated
	try{
		Dam_Pys_Element::set_erg_table(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}

	//delete the table
	QSqlQuery query(*ptr_database);
	ostringstream query_string;
	query_string <<"DELETE FROM " << Dam_Pys_Element::elem_erg_table->get_table_name();
	query_string << " WHERE " ;
	query_string << Dam_Pys_Element::elem_erg_table->get_column_name(hyd_label::sz_bound_id) << " = " << bound_sz;

	Data_Base::database_request(&query, query_string.str(), ptr_database);
	if(query.lastError().isValid()==true){
		Error msg;
		msg.set_msg("Dam_Pys_Element::delete_data_in_erg_table(QSqlDatabase *ptr_database, const int bound_sz)","Invalid database request", "Check the database", 2, false);
		ostringstream info;
		info << "Table Name      : " << Dam_Pys_Element::elem_erg_table->get_table_name() << endl;
		info << "Table error info: " << query.lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
}
//Delete the data in the database table for the results of the damage calculation specified by the raster id (static)
void Dam_Pys_Element::delete_data_in_erg_table_per_raster(QSqlDatabase *ptr_database, const int raster_id){
	//the table is set (the name and the column names) and allocated
	try{
		Dam_Pys_Element::set_erg_table(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}

	//delete the table
	QSqlQuery query(*ptr_database);
	ostringstream query_string;
	query_string <<"DELETE FROM " << Dam_Pys_Element::elem_erg_table->get_table_name();
	query_string << " WHERE " ;
	query_string << Dam_Pys_Element::elem_erg_table->get_column_name(dam_label::raster_id) << " = " << raster_id;

	Data_Base::database_request(&query, query_string.str(), ptr_database);
	if(query.lastError().isValid()==true){
		Error msg;
		msg.set_msg("Dam_Pys_Element::delete_data_in_erg_table(QSqlDatabase *ptr_database, const int raster_id)","Invalid database request", "Check the database", 2, false);
		ostringstream info;
		info << "Table Name      : " << Dam_Pys_Element::elem_erg_table->get_table_name() << endl;
		info << "Table error info: " << query.lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
}
//Close and delete the database table for the results of the damage calculation per element (static)
void Dam_Pys_Element::close_erg_table(void){
	if(Dam_Pys_Element::elem_erg_table!=NULL){
		delete Dam_Pys_Element::elem_erg_table;
		Dam_Pys_Element::elem_erg_table=NULL;
	}
}
//Select data in the database table for the results of the damage calculation specified by the system id and the scenario-ids (static)
int Dam_Pys_Element::select_data_in_erg_table(QSqlQueryModel *results, QSqlDatabase *ptr_database, const _sys_system_id id, const int bound_sz, const string break_sz ){
	//the table is set (the name and the column names) and allocated
	try{
		Dam_Pys_Element::set_erg_table(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}

	ostringstream query_string;
	query_string <<"SELECT ";
	query_string << Dam_Pys_Element::elem_erg_table->get_column_name(dam_label::elem_id) << " , ";
	query_string << Dam_Pys_Element::elem_erg_table->get_column_name(dam_label::raster_id) << " , ";
	query_string << Dam_Pys_Element::elem_erg_table->get_column_name(dam_label::crit_age_50_59) << " , ";
	query_string << Dam_Pys_Element::elem_erg_table->get_column_name(dam_label::crit_age_80) << " , ";
	query_string << Dam_Pys_Element::elem_erg_table->get_column_name(dam_label::crit_female) << " , ";
	query_string << Dam_Pys_Element::elem_erg_table->get_column_name(dam_label::crit_owner) << " , ";

	query_string << Dam_Pys_Element::elem_erg_table->get_column_name(dam_label::denscrit_age50_59) << " , ";
	query_string << Dam_Pys_Element::elem_erg_table->get_column_name(dam_label::denscrit_age_80) << " , ";
	query_string << Dam_Pys_Element::elem_erg_table->get_column_name(dam_label::denscrit_female) << " , ";
	query_string << Dam_Pys_Element::elem_erg_table->get_column_name(dam_label::denscrit_owner) << " , ";
	query_string << Dam_Pys_Element::elem_erg_table->get_column_name(dam_label::total_dens_score) << " , ";
	query_string << Dam_Pys_Element::elem_erg_table->get_column_name(dam_label::total_score) << " , ";

	query_string << " ST_ASTEXT("<<Dam_Pys_Element::elem_erg_table->get_column_name(dam_label::elem_poly) <<")";

	query_string<<" FROM " << Dam_Pys_Element::elem_erg_table->get_table_name();
	query_string << " WHERE " ;
	query_string << Dam_Pys_Element::elem_erg_table->get_column_name(label::applied_flag) << "= true";
	query_string << " AND ";
	query_string << Dam_Pys_Element::elem_erg_table->get_column_name(label::areastate_id) << " ="  << id.area_state;
	query_string << " AND (";
	query_string << Dam_Pys_Element::elem_erg_table->get_column_name(label::measure_id) << " = " << 0 ;
	query_string << " OR " ;
	query_string << Dam_Pys_Element::elem_erg_table->get_column_name(label::measure_id) << " = " << id.measure_nr;
	query_string << " ) " ;
	query_string << " AND ";
	query_string << Dam_Pys_Element::elem_erg_table->get_column_name(hyd_label::sz_bound_id) << " = " << bound_sz;
	query_string << " AND ";
	query_string << Dam_Pys_Element::elem_erg_table->get_column_name(risk_label::sz_break_id) << " = '" << break_sz<<"'";

	query_string << " ORDER BY ";
	query_string << Dam_Pys_Element::elem_erg_table->get_column_name(dam_label::raster_id) << " , ";
	query_string << Dam_Pys_Element::elem_erg_table->get_column_name(dam_label::elem_id) << "  ";

	Data_Base::database_request(results, query_string.str(), ptr_database);
	if(results->lastError().isValid()==true){
		Error msg;
		msg.set_msg("Dam_Pys_Element::select_data_in_erg_table(QSqlDatabase *ptr_database, const _sys_system_id id, const int bound_sz, const string break_sz )","Invalid database request", "Check the database", 2, false);
		ostringstream info;
		info << "Table Name      : " << Dam_Pys_Element::elem_erg_table->get_table_name() << endl;
		info << "Table error info: " << results->lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}

	return results->rowCount();
}
//Select data in the database table for the results of the damage calculation specified by the system id and the scenario-ids (just part of it)
int Dam_Pys_Element::select_data_in_erg_table(QSqlQueryModel *results, QSqlDatabase *ptr_database, const _sys_system_id id, const int bound_sz, const string break_sz, const int offset, const int number_row ){
		//the table is set (the name and the column names) and allocated
	try{
		Dam_Pys_Element::set_erg_table(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}

	ostringstream query_string;
	query_string <<"SELECT ";
	query_string << Dam_Pys_Element::elem_erg_table->get_column_name(dam_label::elem_id) << " , ";
	query_string << Dam_Pys_Element::elem_erg_table->get_column_name(dam_label::raster_id) << " , ";
	query_string << Dam_Pys_Element::elem_erg_table->get_column_name(dam_label::crit_age_50_59) << " , ";
	query_string << Dam_Pys_Element::elem_erg_table->get_column_name(dam_label::crit_age_80) << " , ";
	query_string << Dam_Pys_Element::elem_erg_table->get_column_name(dam_label::crit_female) << " , ";
	query_string << Dam_Pys_Element::elem_erg_table->get_column_name(dam_label::crit_owner) << " , ";

	query_string << Dam_Pys_Element::elem_erg_table->get_column_name(dam_label::denscrit_age50_59) << " , ";
	query_string << Dam_Pys_Element::elem_erg_table->get_column_name(dam_label::denscrit_age_80) << " , ";
	query_string << Dam_Pys_Element::elem_erg_table->get_column_name(dam_label::denscrit_female) << " , ";
	query_string << Dam_Pys_Element::elem_erg_table->get_column_name(dam_label::denscrit_owner) << " , ";
	query_string << Dam_Pys_Element::elem_erg_table->get_column_name(dam_label::total_dens_score) << " , ";
	query_string << Dam_Pys_Element::elem_erg_table->get_column_name(dam_label::total_score) << " , ";

	query_string << " ST_ASTEXT("<<Dam_Pys_Element::elem_erg_table->get_column_name(dam_label::elem_poly) <<")";

	query_string<<" FROM " << Dam_Pys_Element::elem_erg_table->get_table_name();
	query_string << " WHERE " ;
	query_string << Dam_Pys_Element::elem_erg_table->get_column_name(label::applied_flag) << "= true";
	query_string << " AND ";
	query_string << Dam_Pys_Element::elem_erg_table->get_column_name(label::areastate_id) << " ="  << id.area_state;
	query_string << " AND (";
	query_string << Dam_Pys_Element::elem_erg_table->get_column_name(label::measure_id) << " = " << 0 ;
	query_string << " OR " ;
	query_string << Dam_Pys_Element::elem_erg_table->get_column_name(label::measure_id) << " = " << id.measure_nr;
	query_string << " ) " ;
	query_string << " AND ";
	query_string << Dam_Pys_Element::elem_erg_table->get_column_name(hyd_label::sz_bound_id) << " = " << bound_sz;
	query_string << " AND ";
	query_string << Dam_Pys_Element::elem_erg_table->get_column_name(risk_label::sz_break_id) << " = '" << break_sz<<"'";

	query_string << " ORDER BY ";
	query_string << Dam_Pys_Element::elem_erg_table->get_column_name(dam_label::raster_id) << " , ";
	query_string << Dam_Pys_Element::elem_erg_table->get_column_name(dam_label::elem_id) << "  ";
	query_string << " LIMIT " << number_row;
	query_string << " OFFSET " << offset;

	Data_Base::database_request(results, query_string.str(), ptr_database);
	if(results->lastError().isValid()==true){
		Error msg;
		msg.set_msg("Dam_Pys_Element::select_data_in_erg_table(QSqlDatabase *ptr_database, const _sys_system_id id, const int bound_sz, const string break_sz, const int offset, const int number_row )","Invalid database request", "Check the database", 2, false);
		ostringstream info;
		info << "Table Name      : " << Dam_Pys_Element::elem_erg_table->get_table_name() << endl;
		info << "Table error info: " << results->lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}

	return results->rowCount();
}
//Count data in the database table for the results of the damage calculation specified by the system id and the scenario-ids
int Dam_Pys_Element::count_data_in_erg_table(QSqlQueryModel *results, QSqlDatabase *ptr_database, const _sys_system_id id, const int bound_sz, const string break_sz ){
	//the table is set (the name and the column names) and allocated
	try{
		Dam_Pys_Element::set_erg_table(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}

	int number=0;
	ostringstream query_string;
	query_string <<"SELECT COUNT(";
	query_string << Dam_Pys_Element::elem_erg_table->get_column_name(dam_label::elem_id) << " ) ";

	query_string<<" FROM " << Dam_Pys_Element::elem_erg_table->get_table_name();
	query_string << " WHERE " ;
	query_string << Dam_Pys_Element::elem_erg_table->get_column_name(label::applied_flag) << "= true";
	query_string << " AND ";
	query_string << Dam_Pys_Element::elem_erg_table->get_column_name(label::areastate_id) << " ="  << id.area_state;
	query_string << " AND (";
	query_string << Dam_Pys_Element::elem_erg_table->get_column_name(label::measure_id) << " = " << 0 ;
	query_string << " OR " ;
	query_string << Dam_Pys_Element::elem_erg_table->get_column_name(label::measure_id) << " = " << id.measure_nr;
	query_string << " ) " ;
	query_string << " AND ";
	query_string << Dam_Pys_Element::elem_erg_table->get_column_name(hyd_label::sz_bound_id) << " = " << bound_sz;
	query_string << " AND ";
	query_string << Dam_Pys_Element::elem_erg_table->get_column_name(risk_label::sz_break_id) << " = '" << break_sz<<"'";

	Data_Base::database_request(results, query_string.str(), ptr_database);
	if(results->lastError().isValid()==true){
		Error msg;
		msg.set_msg("Dam_Pys_Element::count_data_in_erg_table(QSqlDatabase *ptr_database, const _sys_system_id id, const int bound_sz, const string break_sz)","Invalid database request", "Check the database", 2, false);
		ostringstream info;
		info << "Table Name      : " << Dam_Pys_Element::elem_erg_table->get_table_name() << endl;
		info << "Table error info: " << results->lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}

	if(results->rowCount()>0){
		number= results->record(0).field(0).value().toInt();
	}

	return number;
}
//Get the maximum value of the global index of the element result database table
int Dam_Pys_Element::get_max_glob_id_erg_table(QSqlDatabase *ptr_database){
	//the table is set (the name and the column names) and allocated
	try{
		Dam_Pys_Element::set_erg_table(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}

	//evaluate the global identifier
	int id_glob=Dam_Pys_Element::elem_erg_table->maximum_int_of_column(Dam_Pys_Element::elem_erg_table->get_column_name(dam_label::glob_id),ptr_database);
	return id_glob;
}
//Get the header for inserting the element result data to database table
string Dam_Pys_Element::get_insert_header_erg_table(QSqlDatabase *ptr_database){
	try{
		Dam_Pys_Element::set_erg_table(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}
	//set the query via a query string
	ostringstream query_string;
	query_string << "INSERT INTO  " << Dam_Pys_Element::elem_erg_table->get_table_name();
	query_string <<" ( ";
	query_string << Dam_Pys_Element::elem_erg_table->get_column_name(dam_label::glob_id) <<" , ";
	query_string << Dam_Pys_Element::elem_erg_table->get_column_name(dam_label::elem_id) <<" , ";
	query_string << Dam_Pys_Element::elem_erg_table->get_column_name(dam_label::raster_id) <<" , ";
	query_string << Dam_Pys_Element::elem_erg_table->get_column_name(label::areastate_id) <<" , ";
	query_string << Dam_Pys_Element::elem_erg_table->get_column_name(label::measure_id) <<" , ";
	query_string << Dam_Pys_Element::elem_erg_table->get_column_name(hyd_label::sz_bound_id) <<" , ";
	query_string << Dam_Pys_Element::elem_erg_table->get_column_name(risk_label::sz_break_id) <<" , ";
	//results
	query_string << Dam_Pys_Element::elem_erg_table->get_column_name(dam_label::crit_age_50_59) <<" , ";
	query_string << Dam_Pys_Element::elem_erg_table->get_column_name(dam_label::crit_age_80) <<" , ";
	query_string << Dam_Pys_Element::elem_erg_table->get_column_name(dam_label::crit_female) <<" , ";
	query_string << Dam_Pys_Element::elem_erg_table->get_column_name(dam_label::crit_owner) <<" , ";
	query_string << Dam_Pys_Element::elem_erg_table->get_column_name(dam_label::total_score) <<" , ";
	query_string << Dam_Pys_Element::elem_erg_table->get_column_name(dam_label::denscrit_age50_59) <<" , ";
	query_string << Dam_Pys_Element::elem_erg_table->get_column_name(dam_label::denscrit_age_80) <<" , ";
	query_string << Dam_Pys_Element::elem_erg_table->get_column_name(dam_label::denscrit_female) <<" , ";
	query_string << Dam_Pys_Element::elem_erg_table->get_column_name(dam_label::denscrit_owner) <<" , ";
	query_string << Dam_Pys_Element::elem_erg_table->get_column_name(dam_label::total_dens_score) <<" , ";
	query_string << Dam_Pys_Element::elem_erg_table->get_column_name(dam_label::elem_poly) <<" ) ";

	query_string << " VALUES ";
	string buffer;
	buffer=query_string.str();
	return buffer;
}
//Switch the applied-flag for the psycho-social damage results in the database table for a defined system state (static)
void Dam_Pys_Element::switch_applied_flag_erg_table(QSqlDatabase *ptr_database, const _sys_system_id id, const bool flag){
//the table is set (the name and the column names) and allocated
	try{
		Dam_Pys_Element::set_erg_table(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}
	QSqlQueryModel query;

	ostringstream query_string;
	query_string <<"UPDATE ";
	query_string << Dam_Pys_Element::elem_erg_table->get_table_name();
	query_string << " SET " ;
	query_string  << Dam_Pys_Element::elem_erg_table->get_column_name(label::applied_flag) << " = "<< functions::convert_boolean2string(flag);
	query_string  << " WHERE ";
	query_string  << Dam_Pys_Element::elem_erg_table->get_column_name(label::areastate_id) << " = "  << id.area_state;
	query_string  << " AND ";
	query_string  <<  Dam_Pys_Element::elem_erg_table->get_column_name(label::measure_id) << " = " << id.measure_nr ;
	query_string  << " AND ";
	query_string  << Dam_Pys_Element::elem_erg_table->get_column_name(label::applied_flag) << " = "<< functions::convert_boolean2string(!flag);

	Data_Base::database_request(&query, query_string.str(), ptr_database);
	if(query.lastError().isValid()==true){
		Error msg;
		msg.set_msg("Dam_Pys_Element::switch_applied_flag_erg_table(QSqlDatabase *ptr_database, const _sys_system_id id, const bool flag)","Invalid database request", "Check the database", 2, false);
		ostringstream info;
		info << "Table Name      : " << Dam_Pys_Element::elem_erg_table->get_table_name() << endl;
		info << "Table error info: " << query.lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
}
//Switch the applied-flag for the psycho-social damage results in the database table for a defined system state (static)
void Dam_Pys_Element::switch_applied_flag_erg_table(QSqlDatabase *ptr_database, const _sys_system_id id, const int hyd_sc, const bool flag){
	//the table is set (the name and the column names) and allocated
	try{
		Dam_Pys_Element::set_erg_table(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}
	QSqlQueryModel query;

	ostringstream query_string;
	query_string <<"UPDATE ";
	query_string << Dam_Pys_Element::elem_erg_table->get_table_name();
	query_string << " SET " ;
	query_string  << Dam_Pys_Element::elem_erg_table->get_column_name(label::applied_flag) << " = "<< functions::convert_boolean2string(flag);
	query_string  << " WHERE ";
	query_string  << Dam_Pys_Element::elem_erg_table->get_column_name(label::areastate_id) << " = "  << id.area_state;
	query_string  << " AND ";
	query_string  <<  Dam_Pys_Element::elem_erg_table->get_column_name(label::measure_id) << " = " << id.measure_nr ;
	query_string  << " AND ";
	query_string  <<  Dam_Pys_Element::elem_erg_table->get_column_name(hyd_label::sz_bound_id) << " = " << hyd_sc ;
	query_string  << " AND ";
	query_string  << Dam_Pys_Element::elem_erg_table->get_column_name(label::applied_flag) << " = "<< functions::convert_boolean2string(!flag);

	Data_Base::database_request(&query, query_string.str(), ptr_database);
	if(query.lastError().isValid()==true){
		Error msg;
		msg.set_msg("Dam_Pys_Element::switch_applied_flag_erg_table(QSqlDatabase *ptr_database, const _sys_system_id id, const int hyd_sc, const bool flag)","Invalid database request", "Check the database", 2, false);
		ostringstream info;
		info << "Table Name      : " << Dam_Pys_Element::elem_erg_table->get_table_name() << endl;
		info << "Table error info: " << query.lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
}
//Switch the applied-flag for the psycho-social damage elements in the database table for a defined system state (static)
void Dam_Pys_Element::switch_applied_flag_elem_table(QSqlDatabase *ptr_database, const _sys_system_id id, const bool flag){
//the table is set (the name and the column names) and allocated
	try{
		Dam_Pys_Element::set_elem_table(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}
	QSqlQueryModel query;

	ostringstream query_string;
	query_string <<"UPDATE ";
	query_string << Dam_Pys_Element::elem_table->get_table_name();
	query_string << " SET " ;
	query_string  << Dam_Pys_Element::elem_table->get_column_name(label::applied_flag) << " = "<< functions::convert_boolean2string(flag);
	query_string  << " WHERE ";
	query_string  << Dam_Pys_Element::elem_table->get_column_name(label::areastate_id) << " = "  << id.area_state;
	query_string  << " AND ";
	query_string  <<  Dam_Pys_Element::elem_table->get_column_name(label::measure_id) << " = " << id.measure_nr ;
	query_string  << " AND ";
	query_string  << Dam_Pys_Element::elem_table->get_column_name(label::applied_flag) << " = "<< functions::convert_boolean2string(!flag);

	Data_Base::database_request(&query, query_string.str(), ptr_database);
	if(query.lastError().isValid()==true){
		Error msg;
		msg.set_msg("Dam_Pys_Element::switch_applied_flag_elem_table(QSqlDatabase *ptr_database, const _sys_system_id id, const bool flag)","Invalid database request", "Check the database", 2, false);
		ostringstream info;
		info << "Table Name      : " << Dam_Pys_Element::elem_table->get_table_name() << endl;
		info << "Table error info: " << query.lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
}
//Switch the applied-flag for the psycho-social damage elements in the database table for a defined system state (static)
void Dam_Pys_Element::switch_applied_flag_elem_table(QSqlDatabase *ptr_database, const _sys_system_id id, const int elem_id, const int raster_id, const bool flag){
	//the table is set (the name and the column names) and allocated
	try{
		Dam_Pys_Element::set_elem_table(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}
	QSqlQueryModel query;

	ostringstream query_string;
	query_string <<"UPDATE ";
	query_string << Dam_Pys_Element::elem_table->get_table_name();
	query_string << " SET " ;
	query_string  << Dam_Pys_Element::elem_table->get_column_name(label::applied_flag) << " = "<< functions::convert_boolean2string(flag);
	query_string  << " WHERE ";
	query_string  << Dam_Pys_Element::elem_table->get_column_name(label::areastate_id) << " = "  << id.area_state;
	query_string  << " AND ";
	query_string  <<  Dam_Pys_Element::elem_table->get_column_name(label::measure_id) << " = " << id.measure_nr ;
	query_string  << " AND ";
	query_string  << Dam_Pys_Element::elem_table->get_column_name(dam_label::elem_id) << " = "<< elem_id;
	query_string  << " AND ";
	query_string  << Dam_Pys_Element::elem_table->get_column_name(dam_label::raster_id) << " = "<< raster_id;

	Data_Base::database_request(&query, query_string.str(), ptr_database);
	if(query.lastError().isValid()==true){
		Error msg;
		msg.set_msg("Dam_Pys_Element::switch_applied_flag_elem_table(QSqlDatabase *ptr_database, const _sys_system_id id, const int elem_id, const int raster_id, const bool flag)","Invalid database request", "Check the database", 2, false);
		ostringstream info;
		info << "Table Name      : " << Dam_Pys_Element::elem_table->get_table_name() << endl;
		info << "Table error info: " << query.lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
}
//Reswitch the applied-flag for the psycho-social damage elements in the database table for the base state; a defined system state and his applied elements are decisive (static)
void Dam_Pys_Element::reswitch_applied_flag_elem_table(QSqlDatabase *ptr_database, const _sys_system_id id, const bool flag){
	int number=0;
	try{
		Dam_Pys_Element::set_elem_table(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}

	QSqlQueryModel results;
	ostringstream test_filter;
	test_filter<< "Select ";
	test_filter <<  Dam_Pys_Element::elem_table->get_column_name(dam_label::elem_id) << " , ";
	test_filter <<  Dam_Pys_Element::elem_table->get_column_name(dam_label::raster_id) << "  ";

	test_filter << " from " << Dam_Pys_Element::elem_table->get_table_name();
	test_filter << " where " ;
	test_filter << Dam_Pys_Element::elem_table->get_column_name(label::applied_flag) << "= true";
	test_filter << " and ";
	test_filter << Dam_Pys_Element::elem_table->get_column_name(label::areastate_id) << " ="  << id.area_state;
	test_filter << " and ";
	test_filter << Dam_Pys_Element::elem_table->get_column_name(label::measure_id) << " = " << id.measure_nr;

	Data_Base::database_request(&results, test_filter.str(), ptr_database);

	if(results.lastError().isValid()==true){
		Error msg;
		msg.set_msg("Dam_Pys_Element::reswitch_applied_flag_elem_table(QSqlDatabase *ptr_database, const _sys_system_id id, const bool flag)","Invalid database request", "Check the database", 2, false);
		ostringstream info;
		info << "Table Name      : " << Dam_Pys_Element::elem_table->get_table_name() << endl;
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
		rast_id=results.record(i).value((Dam_Pys_Element::elem_table->get_column_name(dam_label::raster_id)).c_str()).toInt();
		elem_id=results.record(i).value((Dam_Pys_Element::elem_table->get_column_name(dam_label::elem_id)).c_str()).toInt();

		Dam_Pys_Element::switch_applied_flag_elem_table(ptr_database, base, elem_id, rast_id, flag);
	}
}
//Copy the results of a given system id to another one in database tables (static)
void Dam_Pys_Element::copy_results(QSqlDatabase *ptr_database, const _sys_system_id src, const _sys_system_id dest){
	//the table is set (the name and the column names) and allocated
	try{
		Dam_Pys_Element::set_erg_table(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}

	int glob_id=0;
	glob_id=Dam_Pys_Element::elem_erg_table->maximum_int_of_column(Dam_Pys_Element::elem_erg_table->get_column_name(dam_label::glob_id) ,ptr_database)+1;

	QSqlQueryModel model;
	ostringstream test_filter;
	test_filter<< "SELECT ";
	test_filter << Dam_Pys_Element::elem_erg_table->get_column_name(dam_label::glob_id);
	test_filter << " from " << Dam_Pys_Element::elem_erg_table->get_table_name();
	test_filter << " WHERE ";
	test_filter << Dam_Pys_Element::elem_erg_table->get_column_name(label::areastate_id) << " = " << src.area_state;
	test_filter << " AND ";
	test_filter << Dam_Pys_Element::elem_erg_table->get_column_name(label::measure_id) << " = " << src.measure_nr ;

	//submit it to the datbase
	Data_Base::database_request(&model, test_filter.str(), ptr_database);
	if(model.lastError().isValid()==true){
		Error msg;
		msg.set_msg("Dam_Pys_Element::copy_results(QSqlDatabase *ptr_database, const _sys_system_id src, const _sys_system_id dest)","Invalid database request", "Check the database", 2, false);
		ostringstream info;
		info << "Table Name      : " << Dam_Pys_Element::elem_erg_table->get_table_name() << endl;
		info << "Table error info: " << model.lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
	test_filter.str("");

	ostringstream cout;
	cout <<"Copy "<<model.rowCount()<<" element results of the psycho-social damages to the new measure state..."<<endl;
	Sys_Common_Output::output_dam->output_txt(&cout);

	QSqlQueryModel model1;
	for(int i=0; i< model.rowCount(); i++){
		if(i%10000==0 && i>0){
			cout << "Copy PYS-element's results "<< i <<" to " << i+10000 <<"..."<< endl;
			Sys_Common_Output::output_dam->output_txt(&cout);
		}
		test_filter.str("");
		test_filter << "INSERT INTO "<< Dam_Pys_Element::elem_erg_table->get_table_name();
		test_filter << " SELECT " << glob_id <<" , ";
		test_filter <<  Dam_Pys_Element::elem_erg_table->get_column_name(dam_label::elem_id)  <<" , ";
		test_filter <<  Dam_Pys_Element::elem_erg_table->get_column_name(dam_label::raster_id)  <<" , ";
		test_filter <<  dest.area_state  <<" , ";
		test_filter <<  dest.measure_nr  <<" , ";
		test_filter <<  Dam_Pys_Element::elem_erg_table->get_column_name(label::applied_flag)  <<" , ";
		test_filter <<  Dam_Pys_Element::elem_erg_table->get_column_name(hyd_label::sz_bound_id)  <<" , ";
		test_filter <<  Dam_Pys_Element::elem_erg_table->get_column_name(risk_label::sz_break_id)  <<" , ";
		test_filter <<  Dam_Pys_Element::elem_erg_table->get_column_name(dam_label::crit_age_50_59)  <<" , ";
		test_filter <<  Dam_Pys_Element::elem_erg_table->get_column_name(dam_label::crit_age_80)  <<" , ";
		test_filter <<  Dam_Pys_Element::elem_erg_table->get_column_name(dam_label::crit_female)  <<" , ";
		test_filter <<  Dam_Pys_Element::elem_erg_table->get_column_name(dam_label::crit_owner)  <<" , ";
		test_filter <<  Dam_Pys_Element::elem_erg_table->get_column_name(dam_label::total_score)  <<" , ";
		test_filter <<  Dam_Pys_Element::elem_erg_table->get_column_name(dam_label::denscrit_age50_59)  <<" , ";
		test_filter <<  Dam_Pys_Element::elem_erg_table->get_column_name(dam_label::denscrit_age_80)  <<" , ";
		test_filter <<  Dam_Pys_Element::elem_erg_table->get_column_name(dam_label::denscrit_female)  <<" , ";
		test_filter <<  Dam_Pys_Element::elem_erg_table->get_column_name(dam_label::denscrit_owner)  <<" , ";
		test_filter <<  Dam_Pys_Element::elem_erg_table->get_column_name(dam_label::total_dens_score)  <<" , ";
		test_filter <<  Dam_Pys_Element::elem_erg_table->get_column_name(dam_label::elem_poly)  <<"  ";

		test_filter << " FROM " << Dam_Pys_Element::elem_erg_table->get_table_name() <<" ";
		test_filter << " WHERE " << Dam_Pys_Element::elem_erg_table->get_column_name(dam_label::glob_id) << " = ";
		test_filter << model.record(i).value(Dam_Pys_Element::elem_erg_table->get_column_name(dam_label::glob_id).c_str()).toInt();
		Data_Base::database_request(&model1, test_filter.str(), ptr_database);
		if(model1.lastError().isValid()==true){
			Error msg;
			msg.set_msg("Dam_Pys_Element::copy_results(QSqlDatabase *ptr_database, const _sys_system_id src, const _sys_system_id dest)","Invalid database request", "Check the database", 2, false);
			ostringstream info;
			info << "Table Name      : " << Dam_Pys_Element::elem_erg_table->get_table_name() << endl;
			info << "Table error info: " << model1.lastError().text().toStdString() << endl;
			msg.make_second_info(info.str());
			throw msg;
		}
		glob_id++;
	}
}
//Select and count the number of relevant damage raster elements in a database table (static)
int Dam_Pys_Element::select_relevant_elements_database(QSqlQueryModel *results, QSqlDatabase *ptr_database, const _sys_system_id id, const int raster_id, const bool with_output){
	int number=0;
	try{
		Dam_Pys_Element::set_elem_table(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}
	if(with_output==true){
		ostringstream cout;
		cout << "Select relevant pyscho-social raster elements in database ..." << endl ;
		Sys_Common_Output::output_dam->output_txt(&cout);
	}

	ostringstream test_filter;
	test_filter<< "Select ";
	test_filter <<  Dam_Pys_Element::elem_table->get_column_name(dam_label::elem_id) << " , ";
	test_filter <<  Dam_Pys_Element::elem_table->get_column_name(dam_label::elem_mid_x) << " , ";
	test_filter <<  Dam_Pys_Element::elem_table->get_column_name(dam_label::elem_mid_y) << " , ";
	test_filter <<  Dam_Pys_Element::elem_table->get_column_name(dam_label::conn_fp_id) << " , ";
	test_filter <<  Dam_Pys_Element::elem_table->get_column_name(dam_label::conn_fp_elem_id) << " , ";
	test_filter <<  Dam_Pys_Element::elem_table->get_column_name(dam_label::area_elem) << " , ";
	test_filter <<  Dam_Pys_Element::elem_table->get_column_name(dam_label::glob_id) << " , ";
	//set the element members
	test_filter <<  Dam_Pys_Element::elem_table->get_column_name(dam_label::category_id) << " , ";
	test_filter <<  Dam_Pys_Element::elem_table->get_column_name(dam_label::crit_age_50_59) << " , ";
	test_filter <<  Dam_Pys_Element::elem_table->get_column_name(dam_label::crit_age_80) << " , ";
	test_filter <<  Dam_Pys_Element::elem_table->get_column_name(dam_label::crit_female) << " , ";
	test_filter <<  Dam_Pys_Element::elem_table->get_column_name(dam_label::crit_owner) << " , ";
	test_filter <<  Dam_Pys_Element::elem_table->get_column_name(dam_label::pop_density) << " , ";
	test_filter <<  Dam_Pys_Element::elem_table->get_column_name(dam_label::no_category_id);

	test_filter << " from " << Dam_Pys_Element::elem_table->get_table_name();
	test_filter << " where " ;
	test_filter << Dam_Pys_Element::elem_table->get_column_name(label::applied_flag) << "= true";
	test_filter << " and ";
	test_filter << Dam_Pys_Element::elem_table->get_column_name(label::areastate_id) << " ="  << id.area_state;
	test_filter << " and (";
	test_filter << Dam_Pys_Element::elem_table->get_column_name(label::measure_id) << " = " << 0 ;
	test_filter << " or " ;
	test_filter << Dam_Pys_Element::elem_table->get_column_name(label::measure_id) << " = " << id.measure_nr;
	test_filter << " ) " ;
	test_filter << " and ";
	test_filter << Dam_Pys_Element::elem_table->get_column_name(dam_label::raster_id) << " = " << raster_id;
	test_filter << " order by " <<  Dam_Pys_Element::elem_table->get_column_name(dam_label::elem_id);

	Data_Base::database_request(results, test_filter.str(), ptr_database);

	//check the request
	if(results->lastError().isValid()){
		Error msg;
		msg.set_msg("Dam_Pys_Element::select_relevant_elements_database(QSqlTableModel *results, const _sys_system_id id, const int raster_id, const bool with_output)","Invalid database request", "Check the database", 2, false);
		ostringstream info;
		info << "Table Name      : " << Dam_Pys_Element::elem_table->get_table_name() << endl;
		info << "Table error info: " << results->lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}

	number= results->rowCount();

	if(with_output==true){
		ostringstream cout;
		cout << number << " relevant psycho-social raster elements(s) are selected in database" << endl ;
		Sys_Common_Output::output_dam->output_txt(&cout);
	}

	return number;
}
//Select and count the number of relevant damage raster elements in a database table (just part of it) (static)
int Dam_Pys_Element::select_relevant_elements_database(QSqlQueryModel *results, QSqlDatabase *ptr_database, const _sys_system_id id, const int raster_id,  const int offset, const int number_rows, const bool with_output){
	int number=0;
	try{
		Dam_Pys_Element::set_elem_table(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}
	if(with_output==true){
		ostringstream cout;
		cout << "Select relevant pyscho-social raster elements in database ..." << endl ;
		Sys_Common_Output::output_dam->output_txt(&cout);
	}

	ostringstream test_filter;
	test_filter<< "Select ";
	test_filter <<  Dam_Pys_Element::elem_table->get_column_name(dam_label::elem_id) << " , ";
	test_filter <<  Dam_Pys_Element::elem_table->get_column_name(dam_label::elem_mid_x) << " , ";
	test_filter <<  Dam_Pys_Element::elem_table->get_column_name(dam_label::elem_mid_y) << " , ";
	test_filter <<  Dam_Pys_Element::elem_table->get_column_name(dam_label::conn_fp_id) << " , ";
	test_filter <<  Dam_Pys_Element::elem_table->get_column_name(dam_label::conn_fp_elem_id) << " , ";
	test_filter <<  Dam_Pys_Element::elem_table->get_column_name(dam_label::area_elem) << " , ";
	test_filter <<  Dam_Pys_Element::elem_table->get_column_name(dam_label::glob_id) << " , ";
	//set the element members
	test_filter <<  Dam_Pys_Element::elem_table->get_column_name(dam_label::category_id) << " , ";
	test_filter <<  Dam_Pys_Element::elem_table->get_column_name(dam_label::crit_age_50_59) << " , ";
	test_filter <<  Dam_Pys_Element::elem_table->get_column_name(dam_label::crit_age_80) << " , ";
	test_filter <<  Dam_Pys_Element::elem_table->get_column_name(dam_label::crit_female) << " , ";
	test_filter <<  Dam_Pys_Element::elem_table->get_column_name(dam_label::crit_owner) << " , ";
	test_filter <<  Dam_Pys_Element::elem_table->get_column_name(dam_label::pop_density) << " , ";
	test_filter <<  Dam_Pys_Element::elem_table->get_column_name(dam_label::no_category_id);

	test_filter << " from " << Dam_Pys_Element::elem_table->get_table_name();
	test_filter << " where " ;
	test_filter << Dam_Pys_Element::elem_table->get_column_name(label::applied_flag) << "= true";
	test_filter << " and ";
	test_filter << Dam_Pys_Element::elem_table->get_column_name(label::areastate_id) << " ="  << id.area_state;
	test_filter << " and (";
	test_filter << Dam_Pys_Element::elem_table->get_column_name(label::measure_id) << " = " << 0 ;
	test_filter << " or " ;
	test_filter << Dam_Pys_Element::elem_table->get_column_name(label::measure_id) << " = " << id.measure_nr;
	test_filter << " ) " ;
	test_filter << " and ";
	test_filter << Dam_Pys_Element::elem_table->get_column_name(dam_label::raster_id) << " = " << raster_id;
	test_filter << " order by " <<  Dam_Pys_Element::elem_table->get_column_name(dam_label::elem_id);
	test_filter << " LIMIT " << number_rows;
	test_filter << " OFFSET " << offset;

	Data_Base::database_request(results, test_filter.str(), ptr_database);

	//check the request
	if(results->lastError().isValid()){
		Error msg;
		msg.set_msg("Dam_Pys_Element::select_relevant_elements_database(QSqlTableModel *results, const _sys_system_id id, const int raster_id, const int offset, const int number_rows, const bool with_output)","Invalid database request", "Check the database", 2, false);
		ostringstream info;
		info << "Table Name      : " << Dam_Pys_Element::elem_table->get_table_name() << endl;
		info << "Table error info: " << results->lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}

	number= results->rowCount();

	if(with_output==true){
		ostringstream cout;
		cout << number << " relevant psycho-social raster elements(s) are selected in database" << endl ;
		Sys_Common_Output::output_dam->output_txt(&cout);
	}

	return number;
}
//Count the number of relevant damage raster elements in a database table (static)
int Dam_Pys_Element::count_relevant_elements_database(QSqlQueryModel *results, QSqlDatabase *ptr_database, const _sys_system_id id, const int raster_id, const bool with_output){
	int number=0;
	try{
		Dam_Pys_Element::set_elem_table(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}
	if(with_output==true){
		ostringstream cout;
		cout << "Count relevant pyscho-social raster elements in database ..." << endl ;
		Sys_Common_Output::output_dam->output_txt(&cout);
	}

	ostringstream test_filter;
	test_filter<< "Select COUNT(";
	test_filter <<  Dam_Pys_Element::elem_table->get_column_name(dam_label::elem_id) << " ) ";

	test_filter << " from " << Dam_Pys_Element::elem_table->get_table_name();
	test_filter << " where " ;
	test_filter << Dam_Pys_Element::elem_table->get_column_name(label::applied_flag) << "= true";
	test_filter << " and ";
	test_filter << Dam_Pys_Element::elem_table->get_column_name(label::areastate_id) << " ="  << id.area_state;
	test_filter << " and (";
	test_filter << Dam_Pys_Element::elem_table->get_column_name(label::measure_id) << " = " << 0 ;
	test_filter << " or " ;
	test_filter << Dam_Pys_Element::elem_table->get_column_name(label::measure_id) << " = " << id.measure_nr;
	test_filter << " ) " ;
	test_filter << " and ";
	test_filter << Dam_Pys_Element::elem_table->get_column_name(dam_label::raster_id) << " = " << raster_id;

	Data_Base::database_request(results, test_filter.str(), ptr_database);

	//check the request
	if(results->lastError().isValid()){
		Error msg;
		msg.set_msg("Dam_Pys_Element::count_relevant_elements_database(QSqlTableModel *results, const _sys_system_id id, const int raster_id, const bool with_output)","Invalid database request", "Check the database", 2, false);
		ostringstream info;
		info << "Table Name      : " << Dam_Pys_Element::elem_table->get_table_name() << endl;
		info << "Table error info: " << results->lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}

	if(results->rowCount()>0){
		number= results->record(0).field(0).value().toInt();
	}

	if(with_output==true){
		ostringstream cout;
		cout << number << " relevant psycho-social raster elements(s) are found in database" << endl ;
		Sys_Common_Output::output_dam->output_txt(&cout);
	}

	return number;
}
//________
//private
//Reset the result value of the element
void Dam_Pys_Element::reset_result_value(void){
	this->res_crit_age50_59.score_pop_value=0.0;
	this->res_crit_age50_59.score_value=0.0;
	this->res_crit_age80.score_pop_value=0.0;
	this->res_crit_age80.score_value=0.0;
	this->res_crit_female.score_pop_value=0.0;
	this->res_crit_female.score_value=0.0;
	this->res_crit_owner.score_pop_value=0.0;
	this->res_crit_owner.score_value=0.0;
}
//Set warning(s)
Warning Dam_Pys_Element::set_warning(const int warn_type){
	string place="Dam_Pys_Element::";
		string help;
		string reason;
		string reaction;
		int type=0;
		Warning msg;
		stringstream info;

	switch (warn_type){
		case 0://interception datas can not submitted
			place.append("set_interception_elem_data2database(QSqlDatabase *ptr_database, const int raster_id)");
			reason="Can not submit the element information of the psycho-social raster evaluated by the raster interception to the database";
			help="Check the database";
			type=2;
			break;
		case 1://element data con not bes submitted
			place.append("transfer_input_members2database(QSqlDatabase *ptr_database, const int raster_index, const double cellsize, const double x_origin, const double y_origin)");
			reason="Can not submit the element data of the psycho-social damage raster to the database";
			help="Check the database";
			type=2;
			break;
		case 2://results datas can not submitted
			place.append("output_result_members2database(QSqlDatabase *ptr_database, const int bound_sz, const string break_sz, const int raster_id, const double cellsize, const double x_origin, const double y_origin)");
			reason="Can not submit the element results data of the psycho-social damage raster to the database";
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
