#include "Dam_Headers_Precompiled.h"
//#include "Dam_Raster.h"

///Default constructor
Dam_Raster::Dam_Raster(void){

	this->characteristic=NULL;
	this->no_info_value=-9999.0;
	this->name=label::not_set;
	this->number=0;
	this->connected_flag=false;
	this->is_selected=false;
	//count the memory
	Sys_Memory_Count::self()->add_mem(sizeof(Dam_Raster), _sys_system_modules::DAM_SYS);
}
///Default destructor
Dam_Raster::~Dam_Raster(void){
	this->delete_characteristics();
	//count the memory
	Sys_Memory_Count::self()->minus_mem(sizeof(Dam_Raster), _sys_system_modules::DAM_SYS);
}
//__________
//public
//Read in the economic damage data from file
void Dam_Raster::read_damage_raster_per_file(const string file_dam_raster){
	int line_counter=0;
	ifstream ifile;

	//read in damage function
	//open file
	ifile.open(file_dam_raster.c_str(), ios_base::in);
	if(ifile.is_open()==false){
		Error msg=this->set_error(1);
		ostringstream info;
		info << "Filename: " << file_dam_raster << endl;
		msg.make_second_info(info.str());
		throw msg;
	}

	try{
        this->read_raster_infos(&ifile, &line_counter);
		this->check_raster_info();
		this->init_raster();
		this->generate_boundary();
		this->read_raster_elem_characteristic(&ifile, &line_counter);
		this->sort_raster_characteristics();



	}
	catch(Error msg){
		ifile.close();
		ostringstream info;
		info << "Error occurs near line: "<< line_counter << endl;
		info << "Filename              : "<< file_dam_raster << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
	ifile.close();
}
//Initialize the raster: the points, segments and characteristics
void Dam_Raster::init_raster(void){
    //Do not use init_raster() because segements and points are not needed for the damage raster (memory optimization
    //Geo_Raster::init_raster();
    this->area=this->width_x*this->width_y;
    //set number of points
    this->number_points=(this->no_elems_x+1)*(this->no_elems_y+1);
    //set number of elements
    this->number_polygons=this->no_elems_x*this->no_elems_y;
    //set the segments
    this->number_segments=(this->no_elems_x+ this->no_elems_y)+(this->no_elems_y*2*this->no_elems_x);
	this->allocate_characteristics();
}
//Delete the raster information: points and segments and characteristics
void Dam_Raster::clear_raster(void){
	Geo_Raster::clear_raster();
	this->delete_characteristics();
	this->delete_raster_elem_info();
}
//Output the members of geometrical damage raster to display/console
void Dam_Raster::output_member(void){
	ostringstream cout;
	cout << "GENERAL"<< endl;
	cout<<  " Number             " << this->number<<endl;
	cout << " Name               " << this->name<< endl;
	cout << " Type               " << Dam_Raster::convert_raster_type2txt(this->type) << endl;
	cout << "GEOMETRY"<< P(2)<< FORMAT_FIXED_REAL<< endl;
	cout << " No. elements       " << this->no_elems_x*this->no_elems_y << endl;
	cout << "  No. elements x    " << this->no_elems_x << endl;
	cout << "  No. elements y    " << no_elems_y << endl;
	cout << " Width x-direction  " << this->width_x << label::m<< endl;
	cout << " Width y-direction  " << this->width_y << label::m<< endl;
	cout << " Origin x           " << this->x_coor_origin << label::m << endl;
	cout << " Origin y           " << this->y_coor_origin << label::m << endl;
	cout << "BOUNDARY" <<endl;
	cout << " Area               " << this->geometrical_bound.calculate_area() << label::squaremeter<< endl;
	cout << " Perimeter          " << this->geometrical_bound.calculate_perimeter() << label::m<< endl;
	cout << "CHARACTERISTICS"<<endl;
	cout << " No-Info value      " << this->no_info_value << endl;
	Sys_Common_Output::output_dam->output_txt(&cout,false);

}
//Get the characteristics as double value
double Dam_Raster::get_characteristics_asdouble(const int index){
	return this->characteristic[index];
}
//Get the characteristics as integer value
int Dam_Raster::get_characteristics_asint(const int index){
	return (int)this->characteristic[index];
}
//Get the characteristics as double value
double Dam_Raster::get_no_info_asdouble(void){
	return this->no_info_value;
}
//Get the characteristics as integer value
int Dam_Raster::get_no_info_asint(void){
	return (int)this->no_info_value;
}
//Get the raster name
string Dam_Raster::get_raster_name(void){
	return this->name;
}
//Set the raster number
void Dam_Raster::set_raster_number(const int number){
	this->number=number;
}
//Get the raster number
int Dam_Raster::get_raster_number(void){
	return this->number;
}
//Compare two raster for equality of the raster informations
bool Dam_Raster::compare_raster_infos(Dam_Raster *buffer){
	
	if(this->no_elems_x!=buffer->no_elems_x){
		return false;
	}
	if(this->no_elems_y!=buffer->no_elems_y){
		return false;
	}
	if(abs(this->width_x-buffer->width_x)>constant::meter_epsilon){
		return false;
	}
	if(abs(this->width_y-buffer->width_y)>constant::meter_epsilon){
		return false;
	}
	if(abs(this->x_coor_origin-buffer->x_coor_origin)>constant::meter_epsilon){
		return false;
	}
	if(abs(this->y_coor_origin-buffer->y_coor_origin)>constant::meter_epsilon){
		return false;
	}
	if(abs(this->no_info_value-buffer->no_info_value)>constant::zero_epsilon){
		return false;
	}
	if(this->name!=buffer->name){
		return false;
	}


	return true;
}
//Check if two rasters are intercepting each other
bool Dam_Raster::check_interception(Dam_Raster *buffer){
	if(this->geometrical_bound.check_polygon_interception(&buffer->geometrical_bound)==_polygon_intercept::complete_outside ||
		this->geometrical_bound.check_polygon_interception(&buffer->geometrical_bound)==_polygon_intercept::outside_with_boundary){
		return false;
	}
	else{
		return true;
	}
}
//Check if there are an interception between a raster and a polygon
bool Dam_Raster::check_interception(Dam_Polygon *buffer){
	if(this->geometrical_bound.check_polygon_interception(buffer)==_polygon_intercept::complete_outside ||
		this->geometrical_bound.check_polygon_interception(buffer)==_polygon_intercept::outside_with_boundary){
		return false;
	}
	else{
		return true;
	}
}
//Get the type of the raster
_dam_raster_types Dam_Raster::get_raster_type(void){
	return this->type;
}
//Set the type of the raster
void Dam_Raster::set_raster_type(_dam_raster_types type){
	this->type=type;
}
//Set intercepted elements to no-info value
void Dam_Raster::set_intercepted_elem2noinfo(Dam_Raster *raster){
	//no interception
	if(this->geometrical_bound.check_polygon_interception(&raster->geometrical_bound)==_polygon_intercept::complete_outside ||
		this->geometrical_bound.check_polygon_interception(&raster->geometrical_bound)==_polygon_intercept::outside_with_boundary){
		return;
	}

	//set a warning
	Warning msg=this->set_warning(1);
	ostringstream info;
	info <<"Raster name 1 : " << this->name << endl;
	info <<"Raster name 2 : " << raster->name<<endl;
	msg.make_second_info(info.str());
	msg.output_msg(4);

	//which has the smaller area
	if(this->get_area()<=raster->get_area()){
		for(int i=0; i< raster->get_number_elements(); i++){
			raster->set_geometrical_raster_polygon(i);
			if(this->geometrical_bound.check_point_inside(&raster->raster_elem.get_mid_point())==true){
				raster->set_charcacteristic2no_info(i);
			}
		}
	}
	else{
		for(int i=0; i< this->get_number_elements(); i++){
			this->set_geometrical_raster_polygon(i);
			if(raster->geometrical_bound.check_point_inside(&this->raster_elem.get_mid_point())==true){
				this->set_charcacteristic2no_info(i);
			}
		}
	}	
}
//Convert a string into a raster type (_dam_raster_types) (static)
_dam_raster_types Dam_Raster::convert_txt2raster_type(const string txt){
	_dam_raster_types type;
	string buffer=txt;

	functions::clean_string(&buffer);
	functions::convert_string2lower_case(&buffer);

	if(buffer==dam_label::rast_ecn_immob){
		type=_dam_raster_types::ecn_immob;
	}
	else if(buffer==dam_label::rast_ecn_mob){
		type=_dam_raster_types::ecn_mob;
	}
	else if(buffer==dam_label::rast_ecn_immob_stock){
		type=_dam_raster_types::ecn_immob_stock;
	}
	else if(buffer==dam_label::rast_ecn_mob_stock){
		type=_dam_raster_types::ecn_mob_stock;
	}
	else if(buffer==dam_label::rast_eco_btype){
		type=_dam_raster_types::eco_btype;
	}
	else if(buffer==dam_label::rast_eco_soil_cost){
		type=_dam_raster_types::eco_soil_cost;
	}
	else if(buffer==dam_label::rast_eco_soil_erosion){
		type=_dam_raster_types::eco_soil_erosion;
	}
	else if(buffer==dam_label::rast_pop_dam_category){
		type=_dam_raster_types::pop_dam_category;
	}
	else if(buffer==dam_label::rast_pop_density){
		type=_dam_raster_types::pop_density;
	}
	else if(buffer==dam_label::rast_pys_category){
		type=_dam_raster_types::pys_category;
	}
	else if(buffer==dam_label::rast_pys_crit_age50_49){
		type=_dam_raster_types::pys_crit_age50_59;
	}
	else if(buffer==dam_label::rast_pys_crit_age80){
		type=_dam_raster_types::pys_crit_age80;
	}
	else if(buffer==dam_label::rast_pys_crit_female){
		type=_dam_raster_types::pys_crit_female;
	}
	else if(buffer==dam_label::rast_pys_crit_owner){
		type=_dam_raster_types::pys_crit_owner;
	}
	else if(buffer==dam_label::rast_pys_total){
		type=_dam_raster_types::pys_pys_total;
	}
	else if(buffer==dam_label::rast_hyd_waterlevel){
		type=_dam_raster_types::hyd_waterlevel;
	}
	else if(buffer==dam_label::rast_hyd_geo_height){
		type=_dam_raster_types::hyd_height;
	}
	else if(buffer==dam_label::rast_hyd_max_v){
		type=_dam_raster_types::hyd_velocity;
	}
	else if(buffer==dam_label::rast_hyd_duration){
		type=_dam_raster_types::hyd_duration;
	}
	else if(buffer==dam_label::rast_hyd_dh_dt){
		type=_dam_raster_types::hyd_dhdt;
	}
	else{
		Error msg;
		msg.set_msg("Dam_Raster::convert_txt2raster_type(const string txt)", "Can not convert this raster type", "Check the given type", 1, false);
		ostringstream info;
		info << "Try to convert raster type: " << txt << endl;
		info << "Possible types: "<< endl;
		info << " ECN"<<endl;
		info << "  "<<dam_label::rast_ecn_immob  << endl;
		info << "  "<<dam_label::rast_ecn_mob  << endl;
		info << "  "<<dam_label::rast_ecn_immob_stock  << endl;
		info << "  "<<dam_label::rast_ecn_mob_stock  << endl;
		info << " ECO"<<endl;
		info << "  "<<dam_label::rast_eco_btype  << endl;
		info << "  "<<dam_label::rast_eco_soil_cost  << endl;
		info << "  "<<dam_label::rast_eco_soil_erosion  << endl;
		info << " POP"<<endl;
		info << "  "<<dam_label::rast_pop_dam_category  << endl;
		info << " POP/PYS"<<endl;
		info << "  "<<dam_label::rast_pop_density  << endl;
		info << " PYS"<<endl;
		info << "  "<<dam_label::rast_pys_category  << endl;
		info << "  "<<dam_label::rast_pys_crit_age50_49 << endl;
		info << "  "<<dam_label::rast_pys_crit_age80  << endl;
		info << "  "<<dam_label::rast_pys_crit_female << endl;
		info << "  "<<dam_label::rast_pys_crit_owner  << endl;
		if(Sys_Project::get_project_type()==_sys_project_type::proj_dam){
			info<<"In case of importing hydraulic results information following types are possible:"<<endl;
			info << " HYD"<<endl;
			info << "  "<<dam_label::rast_hyd_waterlevel << endl;
			info << "  "<<dam_label::rast_hyd_geo_height << endl;
			info << "  "<<dam_label::rast_hyd_max_v << endl;
			info << "  "<<dam_label::rast_hyd_duration << endl;
			info << "  "<<dam_label::rast_hyd_dh_dt << endl;

		}
		msg.make_second_info(info.str());
		throw msg;

	}

	return type;
}
//Convert a raster type (_dam_raster_types) into a string (static)
string Dam_Raster::convert_raster_type2txt(const _dam_raster_types type){
	string buffer;
	switch(type){
		case _dam_raster_types::ecn_immob:
			buffer=dam_label::rast_ecn_immob;
			break;
		case _dam_raster_types::ecn_immob_stock:
			buffer=dam_label::rast_ecn_immob_stock;
			break;
		case _dam_raster_types::ecn_mob:
			buffer=dam_label::rast_ecn_mob;
			break;
		case _dam_raster_types::ecn_mob_stock:
			buffer=dam_label::rast_ecn_mob_stock;
			break;
		case _dam_raster_types::ecn_total:
			buffer=dam_label::rast_ecn_total;
			break;
		case _dam_raster_types::eco_btype:
			buffer=dam_label::rast_eco_btype;
			break;
		case _dam_raster_types::eco_soil_cost:
			buffer=dam_label::rast_eco_soil_cost;
			break;
		case _dam_raster_types::eco_soil_erosion:
			buffer=dam_label::rast_eco_soil_erosion;
			break;
		case _dam_raster_types::eco_soil_total:
			buffer=dam_label::rast_eco_soil_total;
			break;
		case _dam_raster_types::pop_dam_category:
			buffer=dam_label::rast_pop_dam_category;
			break;
		case _dam_raster_types::pop_density:
			buffer=dam_label::rast_pop_density;
			break;
		case _dam_raster_types::pop_total:
			buffer=dam_label::rast_pop_total;
			break;
		case _dam_raster_types::pys_category:
			buffer=dam_label::rast_pys_category;
			break;
		case _dam_raster_types::pys_crit_age50_59:
			buffer=dam_label::rast_pys_crit_age50_49;
			break;
		case _dam_raster_types::pys_crit_age80:
			buffer=dam_label::rast_pys_crit_age80;
			break;
		case _dam_raster_types::pys_crit_female:
			buffer=dam_label::rast_pys_crit_female;
			break;
		case _dam_raster_types::pys_crit_owner:
			buffer=dam_label::rast_pys_crit_owner;
			break;
		case _dam_raster_types::pys_pys_total:
			buffer=dam_label::rast_pys_total;
			break;
		case _dam_raster_types::hyd_waterlevel:
			buffer=dam_label::rast_hyd_waterlevel;
			break;
		case _dam_raster_types::hyd_height:
			buffer=dam_label::rast_hyd_geo_height;
			break;
		case _dam_raster_types::hyd_duration:
			buffer=dam_label::rast_hyd_duration;
			break;
		case _dam_raster_types::hyd_velocity:
			buffer=dam_label::rast_hyd_max_v;
			break;
		case _dam_raster_types::hyd_dhdt:
			buffer=dam_label::rast_hyd_dh_dt;
			break;
		default:
			buffer=label::unknown_type;
	}
	return buffer;
}
//Get the flag if the raster is connected
bool Dam_Raster::get_connected_flag(void){
	return this->connected_flag;
}
//Get the flag if the raster is selected
bool Dam_Raster::get_is_selected(void){
	return this->is_selected;
}
//Set the flag if the raster is selected
void Dam_Raster::set_raster_is_selected(const bool flag){
	this->is_selected=flag;
}
//__________
//protected

//__________
//private
//Allocate the characteristics for each element
void Dam_Raster::allocate_characteristics(void){
	try{
		if(this->characteristic==NULL){
            this->characteristic=new double[this->number_polygons];
			//count the memory
            Sys_Memory_Count::self()->add_mem(sizeof(double)*this->number_polygons, _sys_system_modules::DAM_SYS);
		}
	}
	catch(bad_alloc &t){
		Error msg=this->set_error(0);
		ostringstream info;
		info<< "Info bad alloc: " << t.what() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
	for(int i=0; i< this->number_polygons; i++){
		this->characteristic[i]=-9999.9;

	}
}
//Delete the characteristics for each element
void Dam_Raster::delete_characteristics(void){
	if(this->characteristic!=NULL){
		delete []this->characteristic;
		this->characteristic=NULL;
		//count the memory
        Sys_Memory_Count::self()->minus_mem(sizeof(double)*this->number_polygons, _sys_system_modules::DAM_SYS);

	}
}
//Read the raster information per file
void Dam_Raster::read_raster_infos(ifstream *ifile, int *line_counter){
	//read in
	string myline;
	stringstream my_stream;
	bool found_flag=false;
	int must_found_counter=0;

	//search for the begin
	do{
		getline(*ifile, myline,'\n');
		(*line_counter)++;
		functions::clean_string(&myline);

		if(myline==dam_label::begin_rasterinfo){
			found_flag=true;
			break;
		}
	}
	while(ifile->eof()==false);
	if(found_flag==false){
		Error msg=this->set_error(2);
		throw msg;

	}
	else{
		found_flag=false;
	}
	//search for the keywords
	do{
		getline(*ifile, myline,'\n');
		(*line_counter)++;
		functions::clean_string(&myline);
		if(myline==dam_label::end_rasterinfo){
			found_flag=true;
			break;
		}
		else if(myline==dam_label::begin_charac){
			break;
		}
		else if(myline.empty()==false){
			this->find_key_values_file(myline, &must_found_counter);
		}

	}
	while(ifile->eof()==false);

	if(found_flag==false){
		Error msg=this->set_error(4);
		throw msg;
	}

	if(must_found_counter!=8){
		Error msg=this->set_error(5);
		throw msg;
	}

	//check the raster information
	this->check_raster_info();
}
//Read the charactersistics of the elements per file
void Dam_Raster::read_raster_elem_characteristic(ifstream *ifile, int *line_counter){
	//read in
	string myline;
	stringstream my_stream;
	bool found_flag=false;

	//search for the begin
	do{
		getline(*ifile, myline,'\n');
		(*line_counter)++;
		functions::clean_string(&myline);

		if(myline==dam_label::begin_charac){
			found_flag=true;
			break;
		}
	}
	while(ifile->eof()==false);
	if(found_flag==false){
		Error msg=this->set_error(6);
		throw msg;

	}
	else{
		found_flag=false;
	}

	int counter_col=0;
	int counter_row=0;
	int counter_total=0;
	int pos=-1;

	//read in the charactersistics

	do{
		do{
			getline(*ifile, myline,'\n');
			(*line_counter)++;
			functions::clean_string(&myline);
		}
		while(myline.empty()==true && ifile->eof()==false);

		pos=myline.find(dam_label::end_charac);
		if(pos>=0){
			found_flag=true;
			break;
		}
		if(counter_row==this->no_elems_y){
			break;
		}
		if(myline.empty()==false){
			counter_row++;
			do{
				string buffer= functions::get_column(&myline);
				my_stream << buffer;
				my_stream >> this->characteristic[counter_total];
				if(my_stream.fail()==true){
					my_stream.clear();
					ostringstream info;
					info << "Wrong input sequenze  : " << my_stream.str() << myline << endl;
					Error msg=this->set_error(8);
					msg.make_second_info(info.str());
					throw msg;
				}
				my_stream.str("");
				my_stream.clear();
				counter_col++;
				counter_total++;

				if(counter_col==this->no_elems_x){
					break;
				}
			}
			while(myline.empty()==false);
		

			if(myline.empty()!=false){
				Warning msg=this->set_warning(0);
				ostringstream info;
				info<< "No. of columns required :" << this->no_elems_x << endl;
				info<< "No. of columns found    :" << counter_col<<endl;
				info <<"Error occurs near line  : "<< line_counter << endl;
				msg.make_second_info(info.str());
				msg.output_msg(4);

			}
			if(counter_col<this->no_elems_x){
				Error msg=this->set_error(7);
				ostringstream info;
				info<< "No. of columns required :" << this->no_elems_x << endl;
				info<< "No. of columns found    :" << counter_col<<endl;
				msg.make_second_info(info.str());
				throw msg;
			}
			counter_col=0;
		}

	}
	while(ifile->eof()==false);

	if(counter_total!=this->number_polygons){
		Error msg=this->set_error(9);
		ostringstream info;
		info<< "No. of characteristics required :" << this->number_polygons << endl;
		info<< "No. of characteristics found    :" << counter_total<<endl;
		msg.make_second_info(info.str());
		throw msg;

	}
	if(found_flag==false){
		Error msg=this->set_error(10);
		throw msg;
	}
}
//Find the key values for the file input
void Dam_Raster::find_key_values_file(string myline , int *must_found_counter){
	int pos=-1;
	string buffer;
	stringstream buffer1;
	bool wrong_input=false;

	pos=myline.find(dam_label::raster_no_x );
	if(pos>=0 && wrong_input==false){
		buffer=myline.substr(dam_label::raster_no_x.length());
		functions::clean_string(&buffer);
		buffer1 << buffer;
		buffer1 >> this->no_elems_x;
		if(buffer1.fail()==true){
			wrong_input=true;
		}
		else{
			(*must_found_counter)++;
			return;
		}

	}
	pos=myline.find(dam_label::raster_no_y);
	if(pos>=0 && wrong_input==false){
		buffer=myline.substr(dam_label::raster_no_y.length());
		functions::clean_string(&buffer);
		buffer1 << buffer;
		buffer1 >> this->no_elems_y;
		if(buffer1.fail()==true){
			wrong_input=true;
		}
		else{
			(*must_found_counter)++;
			return;
		}

	}
	pos=myline.find(dam_label::raster_x_origin);
	if(pos>=0 && wrong_input==false){
		buffer=myline.substr(dam_label::raster_x_origin.length());
		functions::clean_string(&buffer);
		buffer1 << buffer;
		buffer1 >> this->x_coor_origin;
		if(buffer1.fail()==true){
			wrong_input=true;
		}
		else{
			(*must_found_counter)++;
			return;
		}

	}
	pos=myline.find(dam_label::raster_y_origin);
	if(pos>=0 && wrong_input==false){
		buffer=myline.substr(dam_label::raster_y_origin.length());
		functions::clean_string(&buffer);
		buffer1 << buffer;
		buffer1 >> this->y_coor_origin;
		if(buffer1.fail()==true){
			wrong_input=true;
		}
		else{
			(*must_found_counter)++;
			return;
		}

	}
	pos=myline.find(dam_label::raster_cellsize);
	if(pos>=0 && wrong_input==false){
		buffer=myline.substr(dam_label::raster_cellsize.length());
		functions::clean_string(&buffer);
		buffer1 << buffer;
		buffer1 >> this->width_x;
		this->width_y=this->width_x;
		if(buffer1.fail()==true){
			wrong_input=true;
		}
		else{
			(*must_found_counter)++;
			return;
		}

	}
	pos=myline.find(dam_label::raster_name);
	if(pos>=0 && wrong_input==false){
		buffer=myline.substr(dam_label::raster_name.length());
		functions::clean_string(&buffer);
		buffer1 << buffer;
		buffer1 >> this->name;
		(*must_found_counter)++;
		return;
	}
	pos=myline.find(dam_label::raster_noinfo);
	if(pos>=0 && wrong_input==false){
		buffer=myline.substr(dam_label::raster_noinfo.length());
		functions::clean_string(&buffer);
		buffer1 << buffer;
		buffer1 >> this->no_info_value;
		if(buffer1.fail()==true){
			wrong_input=true;
		}
		else{
			(*must_found_counter)++;
			return;
		}
	}
	pos=myline.find(dam_label::raster_type);
	if(pos>=0 && wrong_input==false){
		buffer=myline.substr(dam_label::raster_type.length());
		functions::clean_string(&buffer);	
		_dam_raster_types buffer_type;

		buffer_type=Dam_Raster::convert_txt2raster_type(buffer);
		if(buffer_type!=this->type){
			Error msg=this->set_error(11);
			ostringstream info;
			info<<"Found raster type    : " << buffer <<endl;
			info<<"Required raster type : " << Dam_Raster::convert_raster_type2txt(this->type) <<endl;
			msg.make_second_info(info.str());
			throw msg;
		}
		(*must_found_counter)++;
		return;
	}

	if(wrong_input==true){
		ostringstream info;
		info << "Wrong input sequenze  : " << buffer1.str() << endl;
		Error msg=this->set_error(3);
		msg.make_second_info(info.str());
		throw msg;
	}

}
//Set the characteristics of an element with the given index to no-info
void Dam_Raster::set_charcacteristic2no_info(const int index){
	if(index<0 || index > this->number_polygons){
		return;
	}
	else{
		this->characteristic[index]=this->no_info_value;
	}
}
//Sort raster characteristics from read down-right to up-left
void Dam_Raster::sort_raster_characteristics(void){
	double *buffer=NULL;
	//allocate the buffer
	try{
		buffer=new double[this->number_polygons];
		//count the memory
		Sys_Memory_Count::self()->add_mem(sizeof(double)*this->number_polygons, _sys_system_modules::DAM_SYS);
	}
	catch(bad_alloc &t){
		Error msg=this->set_error(0);
		ostringstream info;
		info<< "Info bad alloc: " << t.what() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}

	//copy it
	for(int i=0; i< this->number_polygons; i++){
		buffer[i]=this->characteristic[i];
	}


	//sort it
	int counter=0;
	for(int i=0; i< this->no_elems_y; i++){
		for(int j=0; j< this->no_elems_x; j++){
			this->characteristic[counter]=buffer[this->number_polygons-(i+1)*(this->no_elems_x)+j];	
			counter++;
		}
	}

	//delete the buffer
	if(buffer!=NULL){
		delete []buffer;
		//count the memory
		Sys_Memory_Count::self()->minus_mem(sizeof(double)*this->number_polygons, _sys_system_modules::DAM_SYS);
		buffer=NULL;
	}
}
//Set error(s)
Error Dam_Raster::set_error(const int err_type){
	string place="Dam_Raster::";
	string help;
	string reason;
	int type=0;
	bool fatal=false;
	stringstream info;
	Error msg;

	switch (err_type){
		case 0://bad alloc
			place.append("allocate_characteristics(void)/sort_raster_characteristics(void)");
			reason="Can not allocate the memory";
			help="Check the memory";
			type=10;
			break;
		case 1://can not open damage function file
			place.append("read_damage_raster_per_file(const string file_dam_raster)");
			reason="Can not open the file of the damage raster";
			help="Check the file";
			type=5;
			break;
		case 2://do not find the begin of the raster information in file
			place.append("read_raster_infos(ifstream *ifile, int *line_counter)");
			reason="Can not find the begin of the raster information in file";
			help="The raster information has to start with !$BEGIN_RASTERINFO";
			type=1;
			break;
		case 3://wrong input
			place.append("find_key_values_file(string myline , int *must_found_counter)");
			reason="There is a problem with the input; wrong sign is read in";
			help="Check the raster information in file";
			type=1;
			break;
		case 4://do not find the end of the raster information in file
			place.append("read_raster_infos(ifstream *ifile, int *line_counter)");
			reason="Can not find the end of the raster information in file";
			help="The raster information has to end with !$END_RASTERINFO";
			type=1;
			break;
		case 5://do not find all required raster infos
			place.append("read_raster_infos(ifstream *ifile, int *line_counter)");
			reason="Can not find all required raster information";
			help="Check if all raster information are set in the file";
			info << "Required information:"<<endl;
			info << " " << dam_label::raster_no_x<< endl;
			info << " " << dam_label::raster_no_y<< endl;
			info << " " << dam_label::raster_x_origin<< endl;
			info << " " << dam_label::raster_y_origin<< endl;
			info << " " << dam_label::raster_cellsize<< endl;
			info << " " << dam_label::raster_noinfo<< endl;
			info << " " << dam_label::raster_type<< endl;
			info << " " << dam_label::raster_name<< endl;
			type=1;
			break;
		case 6://do not find the begin of the characteristics of the raster elements in file
			place.append("read_raster_elem_characteristic(ifstream *ifile, int *line_counter)");
			reason="Can not find the begin of the characterístic information of the raster elements in file";
			help="The raster information has to start with !$BEGIN_CHARAC";
			type=1;
			break;
		case 7://not enough columns to read in the characteristis
			place.append("read_raster_elem_characteristic(ifstream *ifile, int *line_counter)");
			reason="There are not enough columns in file for the input of the element characteristics";
			help="Check the raster element information in file";
			type=1;
			break;
		case 8://wrong input
			place.append("read_raster_elem_characteristic(ifstream *ifile, int *line_counter)");
			reason="There is a problem with the input; wrong sign is read in";
			help="Check the raster element characteristics in file";
			type=1;
			break;
		case 9://not enough values to read in the characteristis
			place.append("read_raster_elem_characteristic(ifstream *ifile, int *line_counter)");
			reason="The number of given characteristics in file do not correspond to the required number";
			help="Check the raster element characteristics in file";
			type=1;
			break;
		case 10://do not find the end of the characteristics of the raster elements in file
			place.append("read_raster_elem_characteristic(ifstream *ifile, int *line_counter)");
			reason="Can not find the end of the characterístic information of the raster elements in file";
			help="The raster information has to end with !$END_CHARAC";
			type=1;
			break;
		case 11://found raster type in file do not match to the required one
			place.append("find_key_values_file(string myline , int *must_found_counter)");
			reason="The found raster type in file do not match to the required one";
			help="Check the raster information in file";
			type=1;
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
//Set warning(s)
Warning Dam_Raster::set_warning(const int warn_type){
	string place="Dam_Raster::";
		string help;
		string reason;
		string reaction;
		int type=0;
		Warning msg;
		stringstream info;

	switch (warn_type){
		case 0://more columns to read in than required
			place.append("read_raster_elem_characteristic(ifstream *ifile, int *line_counter)");
			reason="There are not enough columns in file for the input of the element charactersistics";
			help="Check the raster element information in file";
			reaction="The surplus columns are ignored";
			type=1;
			break;
		case 1://the raster are intercpeting each other
			place.append("set_intercepted_elem2noinfo(Dam_Raster *raster)");
			reason="The raster are intercepting each other";
			help="Check the raster";
			reaction="The elements of the raster with the greater elements are set to no-info";
			type=15;
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
