#include "Dam_Headers_Precompiled.h"
//#include "Dam_Hyd_Result_Raster.h"

//Default constructor
Dam_Hyd_Result_Raster::Dam_Hyd_Result_Raster(void){
	this->elements=NULL;
	this->name=label::not_set;
	this->no_info_value=-9999.9;
	this->number=0;
	this->is_set=false;
	this->characteristic=NULL;

	//count the memory
	Sys_Memory_Count::self()->add_mem(sizeof(Dam_Hyd_Result_Raster), _sys_system_modules::DAM_SYS);
}
//Default destructor
Dam_Hyd_Result_Raster::~Dam_Hyd_Result_Raster(void){
	this->delete_result_elements();
	this->delete_characteristics();
	//count the memory
	Sys_Memory_Count::self()->minus_mem(sizeof(Dam_Hyd_Result_Raster), _sys_system_modules::DAM_SYS);
}
//____________
//public
//Read all five hydraulic result file for one floodplain raster, which is to be imported for damage calculation to database
void Dam_Hyd_Result_Raster::read_all_hydraulic_rasters_per_file(string fname_waterlevel, string fname_height, string fname_v, string fname_duration, string fname_dhdt){
	try{
		if(fname_waterlevel!=label::not_set){
			this->type=_dam_raster_types::hyd_waterlevel;
			this->read_damage_raster_per_file(fname_waterlevel);
			//allocate the element information
			this->allocate_result_elements();
			this->set_waterlevel2elements(this);
			this->is_set=true;
		}
		else{
			Error msg=this->set_error(1);
			ostringstream info;
			info << "Raster of the waterlevel results"<< endl;
			msg.make_second_info(info.str());
			throw msg;
		}
		if(fname_height!=label::not_set){
			if(this->is_set!=true){
				Error msg=this->set_error(10);
				ostringstream info;
				info << "Error while trying to set height results"<< endl;
				msg.make_second_info(info.str());
				throw msg;
			}
			else{
				Dam_Hyd_Result_Raster buffer;
				buffer.type=_dam_raster_types::hyd_height;
				buffer.read_damage_raster_per_file(fname_height);
				if(this->compare_raster_infos(&buffer)==true){
					this->set_height2elements(&buffer);
				}
				else{
					Error msg=this->set_error(11);
					ostringstream info;
					info << "Error with the height result raster"<< endl;
					msg.make_second_info(info.str());
					throw msg;
				}
			}
		}
		if(fname_v!=label::not_set){
			if(this->is_set!=true){
				Error msg=this->set_error(10);
				ostringstream info;
				info << "Error while trying to set velocity results"<< endl;
				msg.make_second_info(info.str());
				throw msg;
			}
			else{
				Dam_Hyd_Result_Raster buffer;
				buffer.type=_dam_raster_types::hyd_velocity;
				buffer.read_damage_raster_per_file(fname_v);
				if(this->compare_raster_infos(&buffer)==true){
					this->set_velocity2elements(&buffer);
				}
				else{
					Error msg=this->set_error(11);
					ostringstream info;
					info << "Error with the velocity result raster"<< endl;
					msg.make_second_info(info.str());
					throw msg;
				}
			}
		}
		if(fname_duration!=label::not_set){
			if(this->is_set!=true){
				Error msg=this->set_error(10);
				ostringstream info;
				info << "Error while trying to set duration wet results"<< endl;
				msg.make_second_info(info.str());
				throw msg;
			}
			else{
				Dam_Hyd_Result_Raster buffer;
				buffer.type=_dam_raster_types::hyd_duration;
				buffer.read_damage_raster_per_file(fname_duration);
				if(this->compare_raster_infos(&buffer)==true){
					this->set_duration2elements(&buffer);
				}
				else{
					Error msg=this->set_error(11);
					ostringstream info;
					info << "Error with the duration result raster"<< endl;
					msg.make_second_info(info.str());
					throw msg;
				}
			}
		}
		if(fname_dhdt!=label::not_set){
			if(this->is_set!=true){
				Error msg=this->set_error(10);
				ostringstream info;
				info << "Error while trying to set speed of the waterlevel rise results"<< endl;
				msg.make_second_info(info.str());
				throw msg;
			}
			else{
				Dam_Hyd_Result_Raster buffer;
				buffer.type=_dam_raster_types::hyd_dhdt;
				buffer.read_damage_raster_per_file(fname_dhdt);
				if(this->compare_raster_infos(&buffer)==true){
					this->set_dhdt2elements(&buffer);
				}
				else{
					Error msg=this->set_error(11);
					ostringstream info;
					info << "Error with the dhdt result raster"<< endl;
					msg.make_second_info(info.str());
					throw msg;
				}
			}
		}
	}
	catch(Error msg){
		ostringstream info;
		msg.make_second_info(info.str());
		throw msg;
	}
}
//Transfer the hydraulic result data to the database
void Dam_Hyd_Result_Raster::transfer_input_members2database(QSqlDatabase *ptr_database, const int fp_index, Hyd_Boundary_Szenario *sc_id, const bool base_sc){
	try{
		ostringstream prefix;
		ostringstream cout;
		Hyd_Model_Floodplain buffer_FP;
		buffer_FP.set_new_hyd_bound_sz_id(*sc_id);
		//Set general floodplain parameters and allocate the elements
		buffer_FP.set_members_directly(this->name, fp_index, this->no_elems_x, this->no_elems_y, this->width_x, this->x_coor_origin, this->y_coor_origin, this->no_info_value);

		//transfer the charcteristics to the elements of the floodplain
		for (int i=0; i< this->number_polygons;i++){
			//check the element type
			if(this->elements[i].get_waterlevel()!=this->no_info_value){
				//set the general memebers of the element
				buffer_FP.floodplain_elems[i].set_members_directly(i,this->elements[i].get_mid_point(), this->elements[i].get_height(),_hyd_elem_type::STANDARD_ELEM);
				//set the result members of the element
				buffer_FP.floodplain_elems[i].element_type->set_max_results_directly(this->elements[i].get_waterlevel(), this->elements[i].get_velocity(), this->elements[i].get_duration_wet(), this->elements[i].get_dhdt());
			}else{
				//set the general members of the element
				buffer_FP.floodplain_elems[i].set_members_directly(i,this->elements[i].get_mid_point(), this->elements[i].get_height(),_hyd_elem_type::NOINFO_ELEM);
			}
		}

		//init the hydraulic system
		cout << "Transfer HYD element data to database. Further information see HYD display..." << endl ;
		Sys_Common_Output::output_dam->output_txt(&cout);
		Sys_Common_Output::output_hyd->set_focus_on_display();
		prefix << "DAM> ";
		Sys_Common_Output::output_hyd->set_userprefix(&prefix);
		cout << "Initialize the HYD-system for interception with the DAM-system..." << endl ;
		Sys_Common_Output::output_hyd->output_txt(&cout);
		if(base_sc==true){
			cout << "Transfer element data of floodplain "<< this->name<<" ("<<fp_index<<")..." << endl;
			Sys_Common_Output::output_hyd->output_txt(&cout);
			//Transfer the general elements members to database
			buffer_FP.transfer_input_members2database(ptr_database);
			cout << "Transfer element data of floodplain "<< this->name<<" ("<<fp_index<<") is finished!" << endl ;
			Sys_Common_Output::output_hyd->output_txt(&cout);
		}
		cout << "Transfer result data of floodplain "<< this->name<<" ("<<fp_index<<")..." << endl ;
		Sys_Common_Output::output_hyd->output_txt(&cout);
		//Transfer the result elements members to database
		bool was_output=false;
		bool must_output=false;
		buffer_FP.output_result_max2database(ptr_database, "CA", &was_output, must_output);
		cout << "Transfer result data of floodplain "<< this->name<<" ("<<fp_index<<") is finished!" << endl ;
		Sys_Common_Output::output_hyd->output_txt(&cout);

		Sys_Common_Output::output_dam->set_focus_on_display();
		Sys_Common_Output::output_hyd->reset_userprefix();
	}catch(Error msg){
		Sys_Common_Output::output_dam->set_focus_on_display();
		Sys_Common_Output::output_hyd->reset_userprefix();
		ostringstream info;
		msg.make_second_info(info.str());
		throw msg;
	}
}
//Set the geometrical member of the raster by a Sql-query
void Dam_Hyd_Result_Raster::set_geometrical_member_database(const int fp_index, QSqlQueryModel *query_result){
	this->number=fp_index;
	this->name=query_result->record(0).value((Hyd_Model_Floodplain::general_param_table->get_column_name(hyd_label::genmod_name)).c_str()).toString().toStdString();
	this->no_elems_x=query_result->record(0).value((Hyd_Model_Floodplain::general_param_table->get_column_name(hyd_label::nx)).c_str()).toInt();
	this->no_elems_y=query_result->record(0).value((Hyd_Model_Floodplain::general_param_table->get_column_name(hyd_label::ny)).c_str()).toInt();
	this->width_x=query_result->record(0).value((Hyd_Model_Floodplain::general_param_table->get_column_name(hyd_label::elemwidth_x)).c_str()).toDouble();
	this->width_y=query_result->record(0).value((Hyd_Model_Floodplain::general_param_table->get_column_name(hyd_label::elemwidth_y)).c_str()).toDouble();
	this->x_coor_origin=query_result->record(0).value((Hyd_Model_Floodplain::general_param_table->get_column_name(hyd_label::lowleftx)).c_str()).toDouble();
	this->y_coor_origin=query_result->record(0).value((Hyd_Model_Floodplain::general_param_table->get_column_name(hyd_label::lowlefty)).c_str()).toDouble();
	this->no_info_value=query_result->record(0).value((Hyd_Model_Floodplain::general_param_table->get_column_name(hyd_label::noinfovalue)).c_str()).toDouble();
}
//Output the members of hydraulic result raster to display/console
void Dam_Hyd_Result_Raster::output_member(void){
	ostringstream cout;
	cout << "GENERAL"<< endl;
	cout<<  " Number             " << this->number<<endl;
	cout << " Name               " << this->name<< endl;
	cout << " Type               " << Dam_Raster::convert_raster_type2txt(this->type) << endl;
	cout << "GEOMETRY"<< P(2)<< FORMAT_FIXED_REAL<< endl;
	cout << " No. elements       " << this->no_elems_x*this->no_elems_y << endl;
	cout << "  No. elements x    " << this->no_elems_x << endl;
	cout << "  No. elements y    " << this->no_elems_y << endl;
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
//Clear the raster information
void Dam_Hyd_Result_Raster::clear_raster(void){
	Geo_Raster::clear_raster();
	this->delete_result_elements();
	this->delete_characteristics();
	this->name=label::not_set;
	this->no_info_value=-9999.9;
	this->number=0;
	this->is_set=false;
}
//Compare two raster for equality of the raster informations
bool Dam_Hyd_Result_Raster::compare_raster_infos(Dam_Hyd_Result_Raster *buffer){
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
//Get the name of the raster
string Dam_Hyd_Result_Raster::get_raster_name(void){
	return this->name;
}
//Set the number of the raster
void Dam_Hyd_Result_Raster::set_raster_number(const int number){
	this->number=number;
}
//____________
//private
//Allocate the characteristics for each element
void Dam_Hyd_Result_Raster::allocate_characteristics(void){
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
void Dam_Hyd_Result_Raster::delete_characteristics(void){
	if(this->characteristic!=NULL){
		delete []this->characteristic;
		this->characteristic=NULL;
		//count the memory
        Sys_Memory_Count::self()->minus_mem(sizeof(double)*this->number_polygons, _sys_system_modules::DAM_SYS);
	}
}
//Read in the raster data from file
void Dam_Hyd_Result_Raster::read_damage_raster_per_file(const string file_raster){
	int line_counter=0;
	ifstream ifile;
	//read in damage function
	//open file
	ifile.open(file_raster.c_str(), ios_base::in);
	if(ifile.is_open()==false){
		Error msg=this->set_error(2);
		ostringstream info;
		info << "Filename: " << file_raster << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
	try{
		this->read_raster_infos(&ifile, &line_counter);
		this->init_raster();
		this->generate_boundary();
		this->read_raster_elem_characteristic(&ifile, &line_counter);
		this->sort_raster_characteristics();
	}
	catch(Error msg){
		ifile.close();
		ostringstream info;
		info << "Error occurs near line: "<< line_counter << endl;
		info << "Filename              : "<< file_raster << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
	ifile.close();
}
//Initialize the raster: the points, segments and characteristics
void Dam_Hyd_Result_Raster::init_raster(void){
	Geo_Raster::init_raster();
	this->allocate_characteristics();
}
//Read the raster information per file
void Dam_Hyd_Result_Raster::read_raster_infos(ifstream *ifile, int *line_counter){
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
		Error msg=this->set_error(3);
		throw msg;
	}
	else{
		found_flag=false;
	}
	//search for the keywords
	do{
		do{
			getline(*ifile, myline,'\n');
			(*line_counter)++;
			functions::clean_string(&myline);
		}
		while(myline.empty()==true && ifile->eof()==false);

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
void Dam_Hyd_Result_Raster::read_raster_elem_characteristic(ifstream *ifile, int *line_counter){
	//read in
	string myline;
	stringstream my_stream;
	bool found_flag=false;

	//search for the begin
	do{
		do{
			getline(*ifile, myline,'\n');
			(*line_counter)++;
			functions::clean_string(&myline);
		}
		while(myline.empty()==true && ifile->eof()==false);

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
		getline(*ifile, myline,'\n');
		(*line_counter)++;
		functions::clean_string(&myline);
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
				info<< "No. of columns found    :" << counter_col;
				info <<"Error occurs near line  : "<< line_counter << endl;
				msg.make_second_info(info.str());
				msg.output_msg(4);
			}
			if(counter_col<this->no_elems_x){
				Error msg=this->set_error(7);
				ostringstream info;
				info<< "No. of columns required :" << this->no_elems_x << endl;
				info<< "No. of columns found    :" << counter_col;
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
		info<< "No. of characteristics found    :" << counter_total;
		msg.make_second_info(info.str());
		throw msg;
	}
	if(found_flag==false){
		Error msg=this->set_error(10);
		throw msg;
	}
}
//Find the key values for the file input
void Dam_Hyd_Result_Raster::find_key_values_file(string myline , int *must_found_counter){
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
		Error msg=this->set_error(6);
		msg.make_second_info(info.str());
		throw msg;
	}
}
//Sort raster characteristics from read down-right to up-left
void Dam_Hyd_Result_Raster::sort_raster_characteristics(void){
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
//Allocate the result elements
void Dam_Hyd_Result_Raster::allocate_result_elements(void){
	if(this->elements==NULL){
		try{
			this->elements=new Dam_Hydimport_Element[this->number_polygons];
		}
		catch(bad_alloc &t){
			Error msg=this->set_error(0);
			ostringstream info;
			info<< "Info bad alloc: " << t.what() << endl;
			msg.make_second_info(info.str());
			throw msg;
		}
		//set the area, index
		for(int i=0; i<this->number_polygons;i++){
			this->elements[i].set_area_element(this->area);
			this->elements[i].set_index(i);
		}
		this->transfer_midpoint2elem_info();
	}
}
//Delete the result elements
void Dam_Hyd_Result_Raster::delete_result_elements(void){
	if(this->elements!=NULL){
		delete []this->elements;
		this->elements=NULL;
	}
}
//Transfer the midpoint information from the raster element to the raster element information
void Dam_Hyd_Result_Raster::transfer_midpoint2elem_info(void){
//	for(int i=0; i<this->number_polygons;i++){
//		this->set_geometrical_raster_polygon(i);
//		this->elements[i].mid_point=this->raster_elem.get_mid_point();
//	}

    //no segments and points of the raster are needed
    double x_buff=0.0;
    double y_buff=0.0;
    double buff_xorigin=this->x_coor_origin+this->width_x*0.5;
    double buff_yorigin=this->y_coor_origin+this->width_y*0.5;;
    int counterx=0;
    int countery=0;
    for(int i=0; i<this->number_polygons;i++){

        x_buff=buff_xorigin+((double)counterx)*this->width_x;
        y_buff=buff_yorigin+((double)countery)*this->width_y;
        counterx++;
        if(counterx==this->no_elems_x){
            counterx=0;
            countery++;
        }

        this->elements[i].mid_point.set_point_coordinate(x_buff,y_buff);
    }
}
//Set the waterlevel to the elements
void Dam_Hyd_Result_Raster::set_waterlevel2elements(Dam_Hyd_Result_Raster *raster){
	for(int i=0; i< this->number_polygons;i++){
		raster->elements[i].set_waterlevel(this->characteristic[i]);
		/*this->ecn_elem[i].set_index_mob_dam(raster->get_characteristics_asint(i));
		if(raster->get_characteristics_asint(i) == (int)this->no_info_value){
			this->ecn_elem[i].set_noinfo_mob();
		}*/
	}
}
//Set the height to the elements
void Dam_Hyd_Result_Raster::set_height2elements(Dam_Hyd_Result_Raster *raster){
	for(int i=0; i< this->number_polygons;i++){
		this->elements[i].set_height(raster->characteristic[i]);
		/*this->ecn_elem[i].set_index_mob_dam(raster->get_characteristics_asint(i));
		if(raster->get_characteristics_asint(i) == (int)this->no_info_value){
			this->ecn_elem[i].set_noinfo_mob();
		}*/
	}
}
//Set the velocity to the elements
void Dam_Hyd_Result_Raster::set_velocity2elements(Dam_Hyd_Result_Raster *raster){
	for(int i=0; i< this->number_polygons;i++){
		this->elements[i].set_velocity(raster->characteristic[i]);
		/*this->ecn_elem[i].set_index_mob_dam(raster->get_characteristics_asint(i));
		if(raster->get_characteristics_asint(i) == (int)this->no_info_value){
			this->ecn_elem[i].set_noinfo_mob();
		}*/
	}
}
//Set the duration to the elements
void Dam_Hyd_Result_Raster::set_duration2elements(Dam_Hyd_Result_Raster *raster){
	for(int i=0; i< this->number_polygons;i++){
		this->elements[i].set_duration_wet(raster->characteristic[i]);
		/*this->ecn_elem[i].set_index_mob_dam(raster->get_characteristics_asint(i));
		if(raster->get_characteristics_asint(i) == (int)this->no_info_value){
			this->ecn_elem[i].set_noinfo_mob();
		}*/
	}
}
//Set the dhdt-value to the elements
void Dam_Hyd_Result_Raster::set_dhdt2elements(Dam_Hyd_Result_Raster *raster){
	for(int i=0; i< this->number_polygons;i++){
		this->elements[i].set_dhdt(raster->characteristic[i]);
		/*this->ecn_elem[i].set_index_mob_dam(raster->get_characteristics_asint(i));
		if(raster->get_characteristics_asint(i) == (int)this->no_info_value){
			this->ecn_elem[i].set_noinfo_mob();
		}*/
	}
}
//Set error(s)
Error Dam_Hyd_Result_Raster::set_error(const int err_type){
	string place="Dam_Hyd_Result_Raster::";
	string help;
	string reason;
	int type=0;
	bool fatal=false;
	stringstream info;
	Error msg;

	switch (err_type){
		case 0://bad alloc
			place.append("");
			reason="Can not allocate the memory";
			help="Check the memory";
			type=10;
			break;
		case 1://no filename
			place.append("read_all_hydraulic_rasters_per_file(string fname_waterlevel, string fname_height, string fname_v, string fname_duration, string fname_dhdt)");
			reason="Filename of the waterlevel hydraulic result raster is not defined";
			help="Check the input";
			type=2;
			break;
		case 2://cannot open file
			place.append("read_damage_raster_per_file(const string file_raster)");
			reason="Can not open file";
			help="Check the file!";
			type=5;
			break;
		case 3://do not find the begin of the raster information in file
			place.append("read_raster_infos(ifstream *ifile, int *line_counter)");
			reason="Can not find the begin of the raster information in file";
			help="The raster information has to start with !$BEGIN_RASTERINFO";
			type=1;
			break;
		case 4://do not find the end of the raster information in file
			place.append("read_raster_infos(ifstream *ifile, int *line_counter)");
			reason="Can not find the end of the raster information in file";
			help="The raster information has to start with !$END_RASTERINFO";
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
		case 6://wrong input
			place.append("find_key_values_file(string myline , int *must_found_counter)");
			reason="There is a problem with the input; wrong sign is read in";
			help="Check the raster information in file";
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
		case 10://elements not allocated
			place.append("read_all_hydraulic_rasters_per_file(string fname_waterlevel, string fname_height, string fname_v, string fname_duration, string fname_dhdt)");
			reason="The hyd result raster elements haven't been allocated correctly as for the waterlevel raster";
			help="Check the input";
			type=1;
			break;
		case 11://rasters differ
			place.append("read_all_hydraulic_rasters_per_file(string fname_waterlevel, string fname_height, string fname_v, string fname_duration, string fname_dhdt)");
			reason="The given geometrical information of the hydraulic result raster are not identical to the reference raster";
			help="Check the input";
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
///Set warning(s)
Warning Dam_Hyd_Result_Raster::set_warning(const int warn_type){
	string place="Dam_Hyd_Result_Raster::";
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
