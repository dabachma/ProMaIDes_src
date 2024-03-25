#include "Fpl_Headers_Precompiled.h"
//#include "Fpl_Seepage_Calculator_Dike.h"


//Default constructor
Fpl_Seepage_Calculator_Dike::Fpl_Seepage_Calculator_Dike(void){

	this->ptr_waterside_cub=NULL;
	this->ptr_landside_cub=NULL;
	this->ptr_cubature=NULL;

	this->seepage_calc_max=_fpl_max_waterlevel_seepage::kozeny;
	this->seepage_calc_min=_fpl_min_waterlevel_seepage::water_base;


	Sys_Memory_Count::self()->add_mem(sizeof(Fpl_Seepage_Calculator_Dike),_sys_system_modules::FPL_SYS);//count the memory

}

//Default destructor
Fpl_Seepage_Calculator_Dike::~Fpl_Seepage_Calculator_Dike(void){

	Sys_Memory_Count::self()->minus_mem(sizeof(Fpl_Seepage_Calculator_Dike),_sys_system_modules::FPL_SYS);//count the memory

}
//___________
//public
//Set the pointer to waterside cubature;
void Fpl_Seepage_Calculator_Dike::set_ptr_waterside_cub(Fpl_Cub_Dike_Waterside *cub){
	this->ptr_waterside_cub=cub;
}
//Set the pointer to landside cubature;
void Fpl_Seepage_Calculator_Dike::set_ptr_landside_cub(Fpl_Cub_Dike_Landside *cub){
	this->ptr_landside_cub=cub;
}
//Set the pointer to the cubature
void Fpl_Seepage_Calculator_Dike::set_ptr_cubature(Geo_Polysegment *cubature){
	this->ptr_cubature=cubature;
}
//Calculate new waterlevels of the seepage line
void Fpl_Seepage_Calculator_Dike::calculate_waterlevel_seepage_line(const double water_level, Fpl_Seepage_Line_Point_List *ascending, Fpl_Seepage_Line_Point_List *descending, const bool for_output){

	if(this->seepage_calc_max==_fpl_max_waterlevel_seepage::one_third_waterlevel){
		this->calculate_seepage_line_land_one_third(water_level, ascending, for_output);
		this->calculate_seepage_line_land_one_third(water_level, descending, for_output);
		if(this->seepage_calc_min==_fpl_min_waterlevel_seepage::one_third_mid_waterlevel){
			this->calculate_seepage_line_mid_water_one_third(water_level, descending, for_output);	
		}
		else{
			this->calculate_seepage_line_mid_water_base(water_level, descending, for_output);	
		}
	}
	else if(this->seepage_calc_max==_fpl_max_waterlevel_seepage::kozeny){
		try{
			this->calculate_seepage_line_land_kozeny(water_level, ascending, for_output);
			this->calculate_seepage_line_land_kozeny(water_level, descending, for_output);
		}
		catch(Error msg){
			throw msg;
		}
		if(this->seepage_calc_min==_fpl_min_waterlevel_seepage::one_third_mid_waterlevel){
			this->calculate_seepage_line_mid_water_one_third(water_level, descending, for_output);	
		}
		else{
			this->calculate_seepage_line_mid_water_base(water_level, descending, for_output);	
		}
	}
	else {
		this->calculate_seepage_line_land_base(water_level, ascending, for_output);
		this->calculate_seepage_line_land_base(water_level, descending, for_output);
		if(this->seepage_calc_min==_fpl_min_waterlevel_seepage::one_third_mid_waterlevel){
			this->calculate_seepage_line_mid_water_one_third(water_level, descending, for_output);	
		}
		else{
			this->calculate_seepage_line_mid_water_base(water_level, descending, for_output);	
		}
	}
}
//Input the control parameters per database table
void Fpl_Seepage_Calculator_Dike::set_input(QSqlDatabase *ptr_database, const bool frc_sim, const bool output){

	if(output==true){
		ostringstream cout;
		cout <<"Set the control parameters of the seepage calculation... "  << endl ;
		Sys_Common_Output::output_fpl->output_txt(&cout,false);
	}

	//mysql query with the table_model
	QSqlTableModel model(0,*ptr_database);
	int number_result=0;
	//the table is set the name and the column names
	try{
		Fpl_Mc_Sim::set_table(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}

	//give the complet table of control parameters FPL
	model.setTable(Fpl_Mc_Sim::table->get_table_name().c_str());
    //set the query
	Data_Base::database_request(&model);

	number_result=model.rowCount();

	//read out the results
	 //the set of the name-column has to match to this parameter
	//output
	if(number_result>0){
		
		//read out from the search result
		string buffer;
		for(int i=0; i< number_result; i++){
			buffer=model.record(i).value((Fpl_Mc_Sim::table->get_column_name(fpl_label::control_name)).c_str()).toString().toStdString();
			if(buffer==fpl_seepage_method::seepage_maximum){
				this->seepage_calc_max=Fpl_Seepage_Calculator_Dike::convert_txt2seepagetype_max_waterlevel(
					model.record(i).value((Fpl_Mc_Sim::table->get_column_name(fpl_label::control_value)).c_str()).toString().toStdString());
							}
			else if(buffer==fpl_seepage_method::seepage_minimum){
				this->seepage_calc_min=Fpl_Seepage_Calculator_Dike::convert_txt2seepagetype_min_waterlevel(
					model.record(i).value((Fpl_Mc_Sim::table->get_column_name(fpl_label::control_value)).c_str()).toString().toStdString());
							
			}
		}

	}

	if(frc_sim==true && this->seepage_calc_min==_fpl_min_waterlevel_seepage::one_third_mid_waterlevel){
		Warning msg=this->set_warning(0);
		msg.output_msg(1);
		this->seepage_calc_min=_fpl_min_waterlevel_seepage::water_base;
	}
	
}
//Output the control parameters of the seepage calculation to display/console
void Fpl_Seepage_Calculator_Dike::output_members(void){

	Sys_Common_Output::output_fpl->reset_prefix_was_outputed();
	ostringstream cout;
	cout << "SEEPAGE-CALCULATION " <<  endl;
	cout << " Method for the maximum waterlevel  : " << Fpl_Seepage_Calculator_Dike::convert_seepagetype_max_waterlevel2txt(this->seepage_calc_max) << endl;
	cout << " Method for the minimum waterlevel  : " << Fpl_Seepage_Calculator_Dike::convert_seepagetype_min_waterlevel2txt(this->seepage_calc_min) << endl;
	Sys_Common_Output::output_fpl->output_txt(cout.str(),false);

}
//Convert a string into seepage type at maximum waterlevel (_fpl_max_waterlevel_seepage) (static)
_fpl_max_waterlevel_seepage Fpl_Seepage_Calculator_Dike::convert_txt2seepagetype_max_waterlevel(const string txt){

	_fpl_max_waterlevel_seepage type;
	string buffer=txt;

	functions::clean_string(&buffer);
	functions::convert_string2lower_case(&buffer);

	if(buffer==fpl_label::seepage_max_base_land){
		type=_fpl_max_waterlevel_seepage::land_base;
	}
	else if(buffer==fpl_label::seepage_max_one_third){
		type=_fpl_max_waterlevel_seepage::one_third_waterlevel;
	}
	else if(buffer==fpl_label::seepage_max_kozeny){
		type=_fpl_max_waterlevel_seepage::kozeny;
	}
	else{
		Error msg;
		msg.set_msg("Fpl_Seepage_Calculator_Dike::convert_txt2seepagetype_max_waterlevel(const string txt)", "Can not convert this seepage maximum waterlevel type", "Check the given type", 1, false);
		ostringstream info;
		info << "seepage maximum waterlevel type: " << txt << endl;
		info << "Possible types: "<< endl;
		info << " "<<fpl_label::seepage_max_base_land  << endl;
		info << " "<<fpl_label::seepage_max_one_third  << endl;
		info << " "<<fpl_label::seepage_max_kozeny  << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
	return type;
}
//Convert a seepage type at maximum waterlevel (_fpl_max_waterlevel_seepage) into a string (static)
string Fpl_Seepage_Calculator_Dike::convert_seepagetype_max_waterlevel2txt(const _fpl_max_waterlevel_seepage type){
	string buffer;
	switch(type){
		case _fpl_max_waterlevel_seepage::land_base:
			buffer=fpl_label::seepage_max_base_land;
			break;
		case _fpl_max_waterlevel_seepage::one_third_waterlevel:
			buffer=fpl_label::seepage_max_one_third;
			break;
		case _fpl_max_waterlevel_seepage::kozeny:
			buffer=fpl_label::seepage_max_kozeny;
			break;
		default:
			buffer=label::unknown_type;
	}
	return buffer;
}
//Convert a string into seepage type at minimum waterlevel (_fpl_min_waterlevel_seepage) (static)
_fpl_min_waterlevel_seepage Fpl_Seepage_Calculator_Dike::convert_txt2seepagetype_min_waterlevel(const string txt){
	_fpl_min_waterlevel_seepage type;
	string buffer=txt;

	functions::clean_string(&buffer);
	functions::convert_string2lower_case(&buffer);

	if(buffer==fpl_label::seepage_min_base_land){
		type=_fpl_min_waterlevel_seepage::water_base;
	}
	else if(buffer==fpl_label::seepage_min_one_third){
		type=_fpl_min_waterlevel_seepage::one_third_mid_waterlevel;
	}
	else{
		Error msg;
		msg.set_msg("Fpl_Seepage_Calculator_Dike::convert_txt2seepagetype_min_waterlevel(const string txt)", "Can not convert this seepage minimum waterlevel type", "Check the given type", 1, false);
		ostringstream info;
		info << "Seepage minimum waterlevel type: " << txt << endl;
		info << "Possible types: "<< endl;
		info << " "<<fpl_label::seepage_min_base_land  << endl;
		info << " "<<fpl_label::seepage_min_one_third  << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
	return type;
}
//Convert a seepage type at minimum waterlevel (_fpl_min_waterlevel_seepage) into a string (static) 
string Fpl_Seepage_Calculator_Dike::convert_seepagetype_min_waterlevel2txt(const _fpl_min_waterlevel_seepage type){
	string buffer;
	switch(type){
		case _fpl_min_waterlevel_seepage::water_base:
			buffer=fpl_label::seepage_min_base_land;
			break;
		case _fpl_min_waterlevel_seepage::one_third_mid_waterlevel:
			buffer=fpl_label::seepage_min_one_third;
			break;
		default:
			buffer=label::unknown_type;
	}
	return buffer;
}
//Write the default value of the control parameters into the database table (static)
void Fpl_Seepage_Calculator_Dike::set_predefined_data2control_table(QSqlDatabase *ptr_database, QSqlQuery *model, int *id_glob, ostringstream *fix_string){
	try{
		Fpl_Mc_Sim::set_table(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}
	ostringstream total;
	//Keystring for calculation method for maximum waterlevel
	ostringstream query_string;
	query_string << " VALUES ( ";
	query_string << *id_glob << " , " ;
	query_string <<"'"<< fpl_seepage_method::seepage_maximum <<"'"<< " , " ;
	query_string <<"'"<< fpl_label::seepage_max_one_third << "' , " ;
	query_string << "'Calculation method for the maximum waterlevel: "<< fpl_label::seepage_max_base_land <<", "<<fpl_label::seepage_max_kozeny<<", "<<fpl_label::seepage_max_kozeny  <<"' ) ";
	total <<fix_string->str() <<  query_string.str();
	Data_Base::database_request(model, total.str(), ptr_database);
	total.str("");
	query_string.str("");
	(*id_glob)++;

	//Keystring for calculation method for minimum waterlevel
	query_string << " VALUES ( ";
	query_string << *id_glob << " , " ;
	query_string <<"'"<< fpl_seepage_method::seepage_minimum <<"'"<< " , " ;
	query_string <<"'"<< fpl_label::seepage_min_base_land << "' , " ;
	query_string << "'Calculation method for the minimum waterlevel: "<< fpl_label::seepage_min_base_land <<", "<<fpl_label::seepage_min_one_third  <<"(just for deterministic calculation)' ) ";
	total <<fix_string->str() <<  query_string.str();
	Data_Base::database_request(model, total.str(), ptr_database);
	total.str("");
	query_string.str("");
	(*id_glob)++;
}
//_________
//private
//Calculate the seepage line: horizontal
void Fpl_Seepage_Calculator_Dike::calculate_seepage_line_horizontal(double h_rel, Fpl_Seepage_Line_Point_List *point_list, const bool for_output){

	
	if(h_rel>this->ptr_waterside_cub->get_ptr_last_point()->get_ycoordinate()){
		h_rel=this->ptr_waterside_cub->get_ptr_last_point()->get_ycoordinate();
	}
	
	if(h_rel<0.0){
		h_rel=0.0;
	}

	//waterlevel is below landside dike base=> horizontal line
	if(h_rel<this->ptr_landside_cub->get_ptr_last_point()->get_ycoordinate()){
		//find the interception point with waterside
		Geo_Segment buffer;
		buffer.set_coordinates(0.0,h_rel,this->ptr_landside_cub->get_ptr_last_point()->get_xcoordinate(),h_rel);
		_geo_interception_point buff_point_water;
		buff_point_water.interception_point.set_point_name(label::interception_point);

		for(int i=this->ptr_waterside_cub->get_number_segments()-1; i>=0; i--){
			buffer.calc_interception(this->ptr_waterside_cub->get_segment(i),&buff_point_water);
			if(buff_point_water.interception_flag==true){
				break;
			}
		}

		if(for_output==true){
			point_list->add_new_point(buff_point_water.interception_point.get_xcoordinate(),buff_point_water.interception_point.get_ycoordinate(),true, this->ptr_cubature);
		}

		//descending waterlevel =>take a horizontal line
		for(int i=0; i< point_list->get_number_points(); i++){
			//left or right of the dike body
			if(point_list->get_list_point(i)->get_x_coordinate()<buff_point_water.interception_point.get_xcoordinate()){
					point_list->get_list_point(i)->set_waterlevel(h_rel);
					point_list->get_list_point(i)->set_inside_dike_body_flag(false);
			}
			//inbetween (dike body)
			else{
				point_list->get_list_point(i)->set_waterlevel(h_rel);
				point_list->get_list_point(i)->set_inside_dike_body_flag(true);
			}
		}
	}
	//waterlevel horizontal
	else{
		//find the interception point with waterside
		Geo_Segment buffer;
		buffer.set_coordinates(0.0,h_rel,this->ptr_landside_cub->get_ptr_last_point()->get_xcoordinate(),h_rel);
		_geo_interception_point buff_point_water;
		buff_point_water.interception_point.set_point_name(label::interception_point);
		for(int i=this->ptr_waterside_cub->get_number_segments()-1; i>=0; i--){
			buffer.calc_interception(this->ptr_waterside_cub->get_segment(i),&buff_point_water);
			if(buff_point_water.interception_flag==true){
				break;
			}
		}
		_geo_interception_point buff_point_land;
		buff_point_land.interception_point.set_point_name(label::interception_point);
		for(int i=0; i< this->ptr_landside_cub->get_number_segments(); i++){
			buffer.calc_interception(this->ptr_landside_cub->get_segment(i),&buff_point_land);
			if(buff_point_land.interception_flag==true){
				break;
			}
		}

		for(int i=0; i< point_list->get_number_points(); i++){
			//left or right of the dike body
			if(point_list->get_list_point(i)->get_x_coordinate()<buff_point_water.interception_point.get_xcoordinate() ||
				point_list->get_list_point(i)->get_x_coordinate()>buff_point_land.interception_point.get_xcoordinate()){
					point_list->get_list_point(i)->set_waterlevel(h_rel);
					point_list->get_list_point(i)->set_inside_dike_body_flag(false);
			}
			//inbetween (dike body)
			else{
				point_list->get_list_point(i)->set_waterlevel(h_rel);
				point_list->get_list_point(i)->set_inside_dike_body_flag(true);
			}
		}

	}
}
//Calculate the seepage line to the mid: one third of the waterlevel at the crest mid at the waterside to waterlevel at crest mid; the rest have to be calculated before
void Fpl_Seepage_Calculator_Dike::calculate_seepage_line_mid_water_one_third(double h_rel, Fpl_Seepage_Line_Point_List *point_list, const bool for_output){
	if(h_rel>this->ptr_waterside_cub->get_ptr_last_point()->get_ycoordinate()){
		h_rel=this->ptr_waterside_cub->get_ptr_last_point()->get_ycoordinate();
	}
	
	if(h_rel<0.0){
		h_rel=0.0;
	}
	Fpl_Seepage_Line_Point *mid_point;
	mid_point=point_list->get_list_point(0);

	//find the interception point with waterside
	Geo_Segment buffer;
	buffer.set_coordinates(0.0,mid_point->get_waterlevel()*1.0/3.0,mid_point->get_x_coordinate(),mid_point->get_waterlevel()*1.0/3.0);
	_geo_interception_point buff_point_real_water;
	buff_point_real_water.interception_point.set_point_name(label::interception_point);
	for(int i=0; i<this->ptr_waterside_cub->get_number_segments(); i++){
		buffer.calc_interception(this->ptr_waterside_cub->get_segment(i),&buff_point_real_water);
		if(buff_point_real_water.interception_flag==true){
			break;
		}
	}
	
	buffer.set_coordinates(buff_point_real_water.interception_point.get_xcoordinate(),buff_point_real_water.interception_point.get_ycoordinate(),
		mid_point->get_x_coordinate(),mid_point->get_waterlevel());



	if(for_output==true){
		point_list->add_new_point(buff_point_real_water.interception_point.get_xcoordinate(),buff_point_real_water.interception_point.get_ycoordinate(),true, this->ptr_cubature);
		mid_point=point_list->get_list_point(0);
	}

	double deltay=mid_point->get_waterlevel()-buff_point_real_water.interception_point.get_ycoordinate();
	double deltax=mid_point->get_x_coordinate()-buff_point_real_water.interception_point.get_xcoordinate();


	for(int i=0; i< point_list->get_number_points(); i++){
		//left of the waterside interception
		if(point_list->get_list_point(i)->get_x_coordinate()<buff_point_real_water.interception_point.get_xcoordinate()){

			point_list->get_list_point(i)->set_waterlevel(buff_point_real_water.interception_point.get_ycoordinate());
			point_list->get_list_point(i)->set_inside_dike_body_flag(true);

		}
		//right of the mid point
		else if(point_list->get_list_point(i)->get_x_coordinate()> mid_point->get_x_coordinate()){
			//do nothing it has to be set before
		}
		//inbetween
		else{
			double y=buff_point_real_water.interception_point.get_ycoordinate()+(point_list->get_list_point(i)->get_x_coordinate()-buff_point_real_water.interception_point.get_xcoordinate())*
				deltay/deltax;
			if(y<=point_list->get_list_point(i)->get_y_coordinate_cubature()){
				point_list->get_list_point(i)->set_waterlevel(y);
			}
			else{
				point_list->get_list_point(i)->set_waterlevel(point_list->get_list_point(i)->get_y_coordinate_cubature());
			}
			point_list->get_list_point(i)->set_inside_dike_body_flag(true);
		}
	}
	
}
//Calculate the seepage line to the mid: base point at the waterside to the waterlevel at the crest mid; the rest have to be calculated before
void Fpl_Seepage_Calculator_Dike::calculate_seepage_line_mid_water_base(double h_rel, Fpl_Seepage_Line_Point_List *point_list, const bool ){

	if(h_rel>this->ptr_waterside_cub->get_ptr_last_point()->get_ycoordinate()){
		h_rel=this->ptr_waterside_cub->get_ptr_last_point()->get_ycoordinate();
	}
	
	if(h_rel<0.0){
		h_rel=0.0;
	}

	Fpl_Seepage_Line_Point *mid_point;
	mid_point=point_list->get_list_point(0);
	

	//find the interception point with waterside
	Geo_Segment buffer;
	buffer.set_coordinates(0.0,0.0,mid_point->get_x_coordinate(),mid_point->get_waterlevel());
	_geo_interception_point buff_point_water;
	buff_point_water.interception_point.set_point_name(label::interception_point);
	for(int i=this->ptr_waterside_cub->get_number_segments()-1; i>=0; i--){
		buffer.calc_interception(this->ptr_waterside_cub->get_segment(i),&buff_point_water);
		if(buff_point_water.interception_flag==true){
			break;
		}
	}

	double deltay=mid_point->get_waterlevel()-buff_point_water.interception_point.get_ycoordinate();
	double deltax=mid_point->get_x_coordinate()-buff_point_water.interception_point.get_xcoordinate();



	for(int i=0; i< point_list->get_number_points(); i++){
		
		//left of the waterside 
		if(point_list->get_list_point(i)->get_x_coordinate()<0.0){
			point_list->get_list_point(i)->set_waterlevel(0.0);
			point_list->get_list_point(i)->set_inside_dike_body_flag(true);
		}
		//right of the mid point
		else if(point_list->get_list_point(i)->get_x_coordinate()> mid_point->get_x_coordinate()){
			//do nothing it has to be set before
		}
		//inbetween
		else{
			double y=buff_point_water.interception_point.get_ycoordinate()+(point_list->get_list_point(i)->get_x_coordinate()-buff_point_water.interception_point.get_xcoordinate())*
				deltay/deltax;
			if(y<=point_list->get_list_point(i)->get_y_coordinate_cubature()){
				point_list->get_list_point(i)->set_waterlevel(y);
			}
			else{
				point_list->get_list_point(i)->set_waterlevel(point_list->get_list_point(i)->get_y_coordinate_cubature());
			}
			point_list->get_list_point(i)->set_inside_dike_body_flag(true);
		}
	}

}
//Output the seepage line: to the landside base point
void Fpl_Seepage_Calculator_Dike::calculate_seepage_line_land_base(double h_rel, Fpl_Seepage_Line_Point_List *point_list, const bool for_output){
	if(h_rel>this->ptr_waterside_cub->get_ptr_last_point()->get_ycoordinate()){
		h_rel=this->ptr_waterside_cub->get_ptr_last_point()->get_ycoordinate();
	}
	
	if(h_rel<0.0){
		h_rel=0.0;
	}

	//waterlevel is below landside dike base=> horizontal line
	if(h_rel<this->ptr_landside_cub->get_ptr_last_point()->get_ycoordinate()){
		this->calculate_seepage_line_horizontal(h_rel, point_list,for_output);
	}
	//waterlevel is above landside dike base
	else{
		//find the interception point with waterside
		Geo_Segment buffer;
		buffer.set_coordinates(0.0,h_rel,this->ptr_landside_cub->get_ptr_last_point()->get_xcoordinate(),h_rel);
		_geo_interception_point buff_point_water;
		buff_point_water.interception_point.set_point_name(label::interception_point);
		for(int i=0; i<this->ptr_waterside_cub->get_number_segments(); i++){
			buffer.calc_interception(this->ptr_waterside_cub->get_segment(i),&buff_point_water);
			if(buff_point_water.interception_flag==true){
				break;
			}
		}

		if(for_output==true){
			point_list->add_new_point(buff_point_water.interception_point.get_xcoordinate(),buff_point_water.interception_point.get_ycoordinate(),true, this->ptr_cubature);
		}

		//find the interception point with landside
		buffer.set_coordinates(buff_point_water.interception_point.get_xcoordinate(),buff_point_water.interception_point.get_ycoordinate(),this->ptr_landside_cub->get_ptr_last_point()->get_xcoordinate(),this->ptr_landside_cub->get_ptr_last_point()->get_ycoordinate());
		_geo_interception_point buff_point_land;
		buff_point_land.interception_point.set_point_name(label::interception_point);
		for(int i=0; i<this->ptr_landside_cub->get_number_segments(); i++){
			buffer.calc_interception(this->ptr_landside_cub->get_segment(i),&buff_point_land);
			if(buff_point_land.interception_flag==true){
				break;
			}
		}
		if(for_output==true){
			point_list->add_new_point(buff_point_land.interception_point.get_xcoordinate(),buff_point_land.interception_point.get_ycoordinate(),true, this->ptr_cubature);
		
		}
		double deltay=buff_point_water.interception_point.get_ycoordinate()-buff_point_land.interception_point.get_ycoordinate();
		double deltax=buff_point_land.interception_point.get_xcoordinate()-buff_point_water.interception_point.get_xcoordinate();
		double h_buff=0.0;
		for(int i=0; i< point_list->get_number_points(); i++){
			//left of the waterside base
			if(point_list->get_list_point(i)->get_x_coordinate()<buff_point_water.interception_point.get_xcoordinate()){
				h_buff=h_rel;
				point_list->get_list_point(i)->set_waterlevel(h_buff);
			}
			//right of the landside base
			else if(point_list->get_list_point(i)->get_x_coordinate()> buff_point_land.interception_point.get_xcoordinate()){
				h_buff=buff_point_land.interception_point.get_ycoordinate();
				point_list->get_list_point(i)->set_waterlevel(h_buff);
				if(h_buff<=point_list->get_list_point(i)->get_y_coordinate_cubature()){
					point_list->get_list_point(i)->set_waterlevel(h_buff);
				}
				else{
					point_list->get_list_point(i)->set_waterlevel(point_list->get_list_point(i)->get_y_coordinate_cubature());
				}
			}
			//inbetween
			else{
				h_buff=h_rel-(point_list->get_list_point(i)->get_x_coordinate()-buff_point_water.interception_point.get_xcoordinate())*
					deltay/deltax;
				if(h_buff<=point_list->get_list_point(i)->get_y_coordinate_cubature()){
					point_list->get_list_point(i)->set_waterlevel(h_buff);
				}
				else{
					point_list->get_list_point(i)->set_waterlevel(point_list->get_list_point(i)->get_y_coordinate_cubature());
				}
			}

			point_list->get_list_point(i)->set_inside_dike_body_flag(true);
		}
	}
}
//Output the seepage line: to the landside one third of the waterlevel
void Fpl_Seepage_Calculator_Dike::calculate_seepage_line_land_one_third(double h_rel, Fpl_Seepage_Line_Point_List *point_list, const bool for_output){
	if(h_rel>this->ptr_waterside_cub->get_ptr_last_point()->get_ycoordinate()){
		h_rel=this->ptr_waterside_cub->get_ptr_last_point()->get_ycoordinate();
	}
	
	if(h_rel<0.0){
		h_rel=0.0;
	}

	//waterlevel is below landside dike base=> horizontal line
	if(h_rel<this->ptr_landside_cub->get_ptr_last_point()->get_ycoordinate()){
		this->calculate_seepage_line_horizontal(h_rel, point_list,for_output);
	}
	//waterlevel is above landside dike base
	else{
		//find the interception point with waterside
		Geo_Segment buffer;
		buffer.set_coordinates(0.0,h_rel,this->ptr_landside_cub->get_ptr_last_point()->get_xcoordinate(),h_rel);
		_geo_interception_point buff_point_water;
		buff_point_water.interception_point.set_point_name(label::interception_point);
		for(int i=0; i<this->ptr_waterside_cub->get_number_segments(); i++){
			buffer.calc_interception(this->ptr_waterside_cub->get_segment(i),&buff_point_water);
			if(buff_point_water.interception_flag==true){
				break;
			}
		}
		if(for_output==true){
			point_list->add_new_point(buff_point_water.interception_point.get_xcoordinate(),buff_point_water.interception_point.get_ycoordinate(),true, this->ptr_cubature);
		}

		//find the interception point with landside: first horizontal line at 1/3 of the waterlevel
		buffer.set_coordinates(buff_point_water.interception_point.get_xcoordinate(),buff_point_water.interception_point.get_ycoordinate()*1.0/3.0+this->ptr_landside_cub->get_ptr_last_point()->get_ycoordinate()*2.0/3.0,
			this->ptr_landside_cub->get_ptr_last_point()->get_xcoordinate(),buff_point_water.interception_point.get_ycoordinate()*1.0/3.0+this->ptr_landside_cub->get_ptr_last_point()->get_ycoordinate()*2.0/3.0);
		_geo_interception_point buff_point_land;
		buff_point_land.interception_point.set_point_name(label::interception_point);
		for(int i=0; i<this->ptr_landside_cub->get_number_segments(); i++){
			buffer.calc_interception(this->ptr_landside_cub->get_segment(i),&buff_point_land);
			if(buff_point_land.interception_flag==true){
				break;
			}
		}

		//find the interception point with landside: second line waterlevel to 1/3 of the waterlevel
		buffer.set_coordinates(buff_point_water.interception_point.get_xcoordinate(),buff_point_water.interception_point.get_ycoordinate(),buff_point_land.interception_point.get_xcoordinate(),buff_point_land.interception_point.get_ycoordinate());
		for(int i=0; i<this->ptr_landside_cub->get_number_segments(); i++){
			buffer.calc_interception(this->ptr_landside_cub->get_segment(i),&buff_point_land);
			if(buff_point_land.interception_flag==true){
				break;
			}
		}
		if(for_output==true){
			point_list->add_new_point(buff_point_land.interception_point.get_xcoordinate(),buff_point_land.interception_point.get_ycoordinate(),true, this->ptr_cubature);
		
		}

		double deltay=buff_point_water.interception_point.get_ycoordinate()-buff_point_land.interception_point.get_ycoordinate();
		double deltax=buff_point_land.interception_point.get_xcoordinate()-buff_point_water.interception_point.get_xcoordinate();
		
		double h_buff=0.0;
		for(int i=0; i< point_list->get_number_points(); i++){
			//left of the waterside base
			if(point_list->get_list_point(i)->get_x_coordinate()<buff_point_water.interception_point.get_xcoordinate()){
				h_buff=h_rel;
				point_list->get_list_point(i)->set_waterlevel(h_buff);
			}
			//right of the landside base
			else if(point_list->get_list_point(i)->get_x_coordinate()>buff_point_land.interception_point.get_xcoordinate()){
				h_buff=buff_point_land.interception_point.get_ycoordinate();
				point_list->get_list_point(i)->set_waterlevel(h_buff);
				if(h_buff<=point_list->get_list_point(i)->get_y_coordinate_cubature()){
					point_list->get_list_point(i)->set_waterlevel(h_buff);
				}
				else{
					point_list->get_list_point(i)->set_waterlevel(point_list->get_list_point(i)->get_y_coordinate_cubature());
				}

			}
			//inbetween
			else{
				h_buff=h_rel-(point_list->get_list_point(i)->get_x_coordinate()-buff_point_water.interception_point.get_xcoordinate())*
					deltay/deltax;
				if(h_buff<=point_list->get_list_point(i)->get_y_coordinate_cubature()){
					point_list->get_list_point(i)->set_waterlevel(h_buff);
				}
				else{
					point_list->get_list_point(i)->set_waterlevel(point_list->get_list_point(i)->get_y_coordinate_cubature());
				}
			}
			point_list->get_list_point(i)->set_inside_dike_body_flag(true);
		}
	}
}
//Calculate the seepage line: to the landside after the Kozeny
void Fpl_Seepage_Calculator_Dike::calculate_seepage_line_land_kozeny(double h_rel, Fpl_Seepage_Line_Point_List *point_list, const bool for_output){
	if(h_rel>this->ptr_waterside_cub->get_ptr_last_point()->get_ycoordinate()){
		h_rel=this->ptr_waterside_cub->get_ptr_last_point()->get_ycoordinate();
	}
	
	if(h_rel<0.0){
		h_rel=0.0;
	}

	//waterlevel is below landside dike base=> horizontal line
	if(h_rel<this->ptr_landside_cub->get_ptr_last_point()->get_ycoordinate()){
		this->calculate_seepage_line_horizontal(h_rel, point_list, for_output);
	}
	//waterlevel is above landside dike base
	else{
		//find the interception point with waterside
		Geo_Segment buffer;
		Geo_Point *land_point;
		land_point=this->ptr_landside_cub->get_ptr_last_point();
		buffer.set_coordinates(0.0,h_rel,this->ptr_landside_cub->get_ptr_last_point()->get_xcoordinate(),h_rel);
		_geo_interception_point buff_point_water;
		buff_point_water.interception_point.set_point_name(label::interception_point);
		int index_segment_water=0;
		for(int i=0; i<this->ptr_waterside_cub->get_number_segments(); i++){
			buffer.calc_interception(this->ptr_waterside_cub->get_segment(i),&buff_point_water);
			if(buff_point_water.interception_flag==true){
				index_segment_water=i;
				break;
			}
		}

		if(for_output==true){
			point_list->add_new_point(buff_point_water.interception_point.get_xcoordinate(),buff_point_water.interception_point.get_ycoordinate(),true, this->ptr_cubature);
		
		}

		double s=0.0;
		bool interception=false;

		double h_drei=0.0;
		double l_strich=0.0;
		int index_segment_land=0;
		double c1=0.0;
		double c2=0.0;
		double c3=0.0;

		int counter=this->ptr_landside_cub->get_number_segments()-1;
		index_segment_land=counter;

		Geo_Point_List sq_interception;

		do{
			interception=false;
			//coordinate of the water base point is (0/0)
			s=buff_point_water.interception_point.get_xcoordinate();
			c1=pow((h_rel-land_point->get_ycoordinate()),2.0);
			c3=(buff_point_water.interception_point.get_xcoordinate()-0.3*s);
			double l=this->ptr_landside_cub->get_segment(counter)->point2.get_xcoordinate()-buff_point_water.interception_point.get_xcoordinate();
			
			double d=l+0.3*s;
			double y_null=pow((c1+pow(d,2.0)),0.5)-d;
			double beta_land=_Geo_Geometrie::rad_to_grad(atan(abs(this->ptr_landside_cub->get_segment(counter)->get_gradient())));

			double a=0.0;
			h_drei=0.0;
			if(beta_land<30.0){
				a=pow(d,2.0)-pow((h_rel-land_point->get_ycoordinate()),2.0)*pow(1.0/tan(_Geo_Geometrie::grad_to_rad(beta_land)),2.0);
				if(a<0.0){
					interception=true;
					counter--;
					if(counter<0){
						Error msg=this->set_error(1);
						throw msg;

					}
					land_point=&(this->ptr_landside_cub->get_segment(counter)->point2);
					continue;
				}
				a=pow((pow((h_rel-land_point->get_ycoordinate()),2.0)+pow(d,2.0)),0.5)-(pow(a,0.5));

				if(a>this->ptr_landside_cub->get_segment(counter)->get_distance()){
					interception=true;
					counter--;
					if(counter<0){
						Error msg=this->set_error(1);
						throw msg;

					}
					land_point=&(this->ptr_landside_cub->get_segment(counter)->point2);
					continue;
				}
			}
			else{
				//approximated with a line
				a=y_null/(1.0-cos(_Geo_Geometrie::grad_to_rad(beta_land)))*(0.00256*beta_land+0.53);

			}

			h_drei=a*sin(_Geo_Geometrie::grad_to_rad(beta_land));
			l_strich=d-h_drei*1.0/abs(this->ptr_landside_cub->get_segment(counter)->get_gradient());
		
			c2=(c1-pow(h_drei,2.0))/l_strich;

			//check interception with landside; if there is one, change the relevant segment
			/*for(int i=0; i< counter ;i++){
				this->ptr_landside_cub->get_segment(i)->calc_interception_square_root(&sq_interception,c1,c2,c3, land_point->get_ycoordinate());
				if(sq_interception.get_number_points()>0){
					interception=true;
					counter=i;
					land_point=&this->ptr_landside_cub->get_segment(counter)->point2;
					break;
				}
			}*/
		}
		while(interception==true);

		if(for_output==true){
			point_list->add_new_point(this->ptr_landside_cub->get_segment(counter)->point2.get_xcoordinate()-h_drei/abs(this->ptr_landside_cub->get_segment(counter)->get_gradient()),0.0,true, this->ptr_cubature);
		
		}

		double x_buff=0.0;
		double y_buff=0.0;
		Geo_Point buffer_point;
		buffer_point.set_point_coordinate(buff_point_water.interception_point.get_xcoordinate()+h_rel*this->ptr_waterside_cub->get_segment(index_segment_water)->get_gradient(),0.0);
		buffer.set_points(&buff_point_water.interception_point, &buffer_point);
		
		buffer.calc_interception_square_root(&sq_interception,c1,c2,c3, land_point->get_ycoordinate());

		if(sq_interception.get_number_points()==0){
			Error msg=this->set_error(0);
			throw msg;
		}
		else{
			buffer.set_points(&buff_point_water.interception_point, sq_interception.get_ptr_point_max_y());
			if(for_output==true){
				point_list->add_new_point(sq_interception.get_ptr_point_max_y()->get_xcoordinate(),sq_interception.get_ptr_point_max_y()->get_ycoordinate(),true, this->ptr_cubature);
		
			}
		}
		
		/*for(int i=0; i< point_list->get_number_points(); i++){
			cout << point_list->get_list_point(i)->get_x_coordinate() << " "<<endl; 
		}*/

		for(int i=0; i< point_list->get_number_points(); i++){
			x_buff=point_list->get_list_point(i)->get_x_coordinate();
			//left of the waterside base
			if(x_buff<buff_point_water.interception_point.get_xcoordinate()){
				point_list->get_list_point(i)->set_waterlevel(h_rel);
				point_list->get_list_point(i)->set_inside_dike_body_flag(true);
			}
			//right of the landside base
			else if(x_buff>this->ptr_landside_cub->get_segment(index_segment_land)->point2.get_xcoordinate()){
				
				point_list->get_list_point(i)->set_waterlevel(this->ptr_landside_cub->get_segment(index_segment_land)->point2.get_ycoordinate());
				point_list->get_list_point(i)->set_inside_dike_body_flag(true);
			}
			//use a straight line
			else if(x_buff<=buffer.point2.get_xcoordinate()){
				y_buff=buffer.get_gradient()*x_buff+buffer.get_y_interception();
				point_list->get_list_point(i)->set_waterlevel(y_buff);
				point_list->get_list_point(i)->set_inside_dike_body_flag(true);
			}
			//inbetween
			else{
				double h_buff=0.0;
				y_buff=c2;
				x_buff=x_buff-c3;
				y_buff=y_buff*(x_buff);
				if(c1>=y_buff){
					y_buff=pow(c1-y_buff,0.5);
					if(y_buff>=h_drei-constant::meter_epsilon){
						h_buff=y_buff+land_point->get_ycoordinate();
					}
					else{
						if(i==0){
							Error msg=this->set_error(2);
							throw msg;
						}
						h_buff=point_list->get_list_point(i-1)->get_waterlevel();
					}
				}
				else{
					if(i==0){
						Error msg=this->set_error(2);
						throw msg;
					}
					h_buff=point_list->get_list_point(i-1)->get_waterlevel();

				}
				if(h_buff<=point_list->get_list_point(i)->get_y_coordinate_cubature()){
					point_list->get_list_point(i)->set_waterlevel(h_buff);
				}
				else{
					point_list->get_list_point(i)->set_waterlevel(point_list->get_list_point(i)->get_y_coordinate_cubature());
				}	
				point_list->get_list_point(i)->set_inside_dike_body_flag(true);

			}

		}
	}
}
//Set warning(s)
Warning Fpl_Seepage_Calculator_Dike::set_warning(const int warn_type){
	string place="Fpl_Seepage_Calculator_Dike::";
		string help;
		string reason;
		string reaction;
		int type=0;
		Warning msg;
		stringstream info;

	switch (warn_type){
		case 0://method is not applicable
			place.append("set_input(QSqlDatabase *ptr_database, const bool frc_sim, const bool output)") ;
			reason="The method of 1/3 for the waterside seepage line calculation is not applicable in a frc-calculation";
			reaction="Method is changed to the base point method";
			help= "Seepage calculation method (waterside)";
			type=7;	
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
Error Fpl_Seepage_Calculator_Dike::set_error(const int err_type){
		string place="Fpl_Seepage_Calculator_Dike::";
		string help;
		string reason;
		int type=0;
		bool fatal=false;
		stringstream info;
		Error msg;

	switch (err_type){
		case 0://no interception found
			place.append("calculate_seepage_line_land_kozeny(double h_rel, Fpl_Seepage_Line_Point_List *point_list)");
			reason="Can not find any interception between the starting line and the Kozeny parabola";
			help="Check the code";
			type=6;
			break;
		case 1://land side to flat
			place.append("calculate_seepage_line_land_kozeny(double h_rel, Fpl_Seepage_Line_Point_List *point_list)");
			reason="Can not find any interception between the Kozeny parabola and the land side cubatur; it is to flat";
			help="Check the land side cubature";
			type=11;
			break;
		case 2://land side to flat
			place.append("calculate_seepage_line_land_kozeny(double h_rel, Fpl_Seepage_Line_Point_List *point_list)");
			reason="There is no point in list before i=0";
			help="Check the code";
			type=11;
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
