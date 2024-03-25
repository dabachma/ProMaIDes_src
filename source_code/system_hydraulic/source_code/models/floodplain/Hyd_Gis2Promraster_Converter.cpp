#include "Hyd_Headers_Precompiled.h"
//#include "Hyd_Gis2Promraster_Converter.h"

//Default constructor
Hyd_Gis2Promraster_Converter::Hyd_Gis2Promraster_Converter(void){

	this->second_infos=NULL;
	this->prom_raster=true;
	this->noinfo_value=-9999.0;
	this->file_name=label::not_set;
	this->last_found_id=0;
	//count the memory
	Sys_Memory_Count::self()->add_mem(sizeof(Hyd_Gis2Promraster_Converter), _sys_system_modules::HYD_SYS);
}
//Default destructor
Hyd_Gis2Promraster_Converter::~Hyd_Gis2Promraster_Converter(void){
	this->delete_secondary_info();
	//count the memory
	Sys_Memory_Count::self()->minus_mem(sizeof(Hyd_Gis2Promraster_Converter), _sys_system_modules::HYD_SYS);
}
//_____________
//public
//Ask the input files for the conversion per dialog
//Read in and set the Gis raster
void Hyd_Gis2Promraster_Converter::input_gis_raster(QString file_name){

	ostringstream prefix;
	prefix << "GIS2PROM> ";
	Sys_Common_Output::output_hyd->set_userprefix(prefix.str());
	ostringstream cout;
	cout << "Input and initialise the GIS generated raster..."<<endl;
	Sys_Common_Output::output_hyd->output_txt(&cout);

	this->file_name=file_name.toStdString();
	this->prom_raster=false;
	if(file_name==label::not_set.c_str()){
		Error msg=this->set_error(3);
		throw msg;
	}
	int line_counter=0;
	ifstream ifile;

	//read in
	//open file
	ifile.open(file_name.toStdString().c_str(), ios_base::in);
	if(ifile.is_open()==false){
		Error msg=this->set_error(1);
		ostringstream info;
		info << "Filename: " << file_name.toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}

	try{
		//read raster
		this->read_raster_infos(&ifile, &line_counter);
		Hyd_Multiple_Hydraulic_Systems::check_stop_thread_flag();

		this->set_geometric_info(this->geo_info);
		this->generate_alloc_points_segments();
		this->make_boundary2polygon();
		this->allocate_secondary_info();
		Hyd_Multiple_Hydraulic_Systems::check_stop_thread_flag();

		//read raster elem info
		this->read_raster_elem_characteristic(&ifile, &line_counter);
		Hyd_Multiple_Hydraulic_Systems::check_stop_thread_flag();
		this->sort_raster_characteristics();

		//init raster

		cout << "Input and initialisation of the GIS generated raster is finished!"<<endl;
		Sys_Common_Output::output_hyd->output_txt(&cout);

	}
	catch(Error msg){
		ifile.close();
		ostringstream info;
		info << "Error occurs near line: "<< line_counter << endl;
		info << "Filename              : "<< file_name.toStdString() << endl;
		if(this->prom_raster==true){
			info<< "ProMaIDes-raster expected"<<endl;
		}
		else{
			info<< "GIS-raster expected"<<endl;
		}
		msg.make_second_info(info.str());
		Sys_Common_Output::output_hyd->rewind_userprefix();
		throw msg;
	}
	ifile.close();

	Sys_Common_Output::output_hyd->rewind_userprefix();
}
//Read in and set the Prom-raster
void Hyd_Gis2Promraster_Converter::input_prom_raster(QString file_name){
	ostringstream prefix;
	prefix << "GIS2PROM> ";
	Sys_Common_Output::output_hyd->set_userprefix(prefix.str());
	ostringstream cout;
	cout << "Input and initialise the ProMaIdes HYD-raster..."<<endl;
	Sys_Common_Output::output_hyd->output_txt(&cout);

	this->prom_raster=true;
	this->file_name=file_name.toStdString();
	if(file_name==label::not_set.c_str()){
		Error msg=this->set_error(4);
		throw msg;
	}
	int line_counter=0;
	ifstream ifile;

	//read in
	//open file
	ifile.open(file_name.toStdString().c_str(), ios_base::in);
	if(ifile.is_open()==false){
		Error msg=this->set_error(2);
		ostringstream info;
		info << "Filename: " << file_name.toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}

	try{
		//read raster
		this->read_raster_infos(&ifile, &line_counter);
		Hyd_Multiple_Hydraulic_Systems::check_stop_thread_flag();
		//init raster
		this->set_geometric_info(this->geo_info);
		this->generate_alloc_points_segments();
		this->make_boundary2polygon();
		this->allocate_secondary_info();
		Hyd_Multiple_Hydraulic_Systems::check_stop_thread_flag();
		cout << "Input and initialisation of the ProMaIdes HYD-raster is finished!"<<endl;
		Sys_Common_Output::output_hyd->output_txt(&cout);
	}
	catch(Error msg){
		ifile.close();
		ostringstream info;
		info << "Error occurs near line: "<< line_counter << endl;
		info << "Filename              : "<< file_name.toStdString() << endl;
		msg.make_second_info(info.str());
		Sys_Common_Output::output_hyd->rewind_userprefix();
		throw msg;
	}
	ifile.close();
	Sys_Common_Output::output_hyd->rewind_userprefix();

}
//Intercept the rasters
bool Hyd_Gis2Promraster_Converter::intercept_rasters(Hyd_Gis2Promraster_Converter *gis_raster){
Hyd_Multiple_Hydraulic_Systems::check_stop_thread_flag();
	ostringstream prefix;
	prefix << "GIS2PROM> ";
	Sys_Common_Output::output_hyd->set_userprefix(prefix.str());
	ostringstream cout;
	cout << "Intercept the rasters..."<<endl;
	Sys_Common_Output::output_hyd->output_txt(&cout);


	//check if the raster are intercepting each other
	double int_area=this->geometrical_bound.polygon_interception(&gis_raster->geometrical_bound);
	if(int_area==0.0){
		cout <<"The raster do not intercept each other! Nothing to do..."<<endl;
		Sys_Common_Output::output_hyd->output_txt(&cout);
		return false;
	}
	double perc=this->geometrical_bound.calculate_area()/int_area;
	//set just the list, if 0.8 percent is outside
	if(perc<0.8){
		//fill the list
		this->outside_list.clear();
		for(int i=0; i<this->number_polygons; i++){
			this->outside_list.append(false);
		}
		//set which elements are completly outside 
		this->set_completly_ouside_elements(&gis_raster->geometrical_bound);
	}

	




	//check the diagonals 
	bool identical=this->check_raster_identical(gis_raster);
	if(this->geo_info.angle!=0.0 && identical==false){
		double diagonal=pow(pow(gis_raster->geo_info.width_x,2.0)*2.0,0.5);
		double min_buff=min(this->geo_info.width_x,this->geo_info.width_y);
		if(min_buff<diagonal){
			//Error
			Error msg=this->set_error(17);
			ostringstream info;
			info << " Diagonal of the GIS-raster     : " <<diagonal <<label::m<<endl;
			info << " Minum width of the Prom-raster : " <<min_buff <<label::m<<endl;
			msg.make_second_info(info.str());
			throw msg;
		}
	}
	//check the width
	else if(identical==false){
		if(this->geo_info.width_x<gis_raster->geo_info.width_x || this->geo_info.width_y<gis_raster->geo_info.width_x){
			Error msg=this->set_error(18);
			ostringstream info;
			if(this->geo_info.width_x<gis_raster->geo_info.width_x){
				info << " x-width GIS-raster     : " <<gis_raster->geo_info.width_x <<label::m<<endl;
				info << " x-width Prom-raster    : " <<this->geo_info.width_x <<label::m<<endl;
			}
			if(this->geo_info.width_y<gis_raster->geo_info.width_y){
				info << " y-width GIS-raster     : " <<gis_raster->geo_info.width_y <<label::m<<endl;
				info << " y-width Prom-raster    : " <<this->geo_info.width_y <<label::m<<endl;
			}
			msg.make_second_info(info.str());
			throw msg;
		}
	}


	//make the interception per element mid-point
	Geo_Point mid;
	for(int i=0; i< gis_raster->number_polygons; i++){
		Hyd_Multiple_Hydraulic_Systems::check_stop_thread_flag();

		if(i%1000==0 && i>0){
			cout << "Interception for elements (GIS-raster) "<< i <<" to " << i+1000 <<" ("<<gis_raster->geo_info.number_x*gis_raster->geo_info.number_y<<")..."<< endl;
			Sys_Common_Output::output_hyd->output_txt(&cout);
		}
		if(abs(gis_raster->second_infos[i].last()-gis_raster->noinfo_value)<=constant::zero_epsilon){
			continue;
		}

		gis_raster->set_geometrical_raster_polygon(i);
		gis_raster->raster_elem.set_mid_point(&mid);
		if(this->geometrical_bound.check_point_inside_atboundary(&mid)==true){
			this->search_element(&mid, gis_raster, i);
		}
	}



	//calculate the final value per element
	this->calculate_value_element(gis_raster->noinfo_value);
	
	cout << "The raster interception is finished!"<<endl;
	Sys_Common_Output::output_hyd->output_txt(&cout);
	Sys_Common_Output::output_hyd->rewind_userprefix();
	return true;

}
//Output the converted Prom-raster to file
void Hyd_Gis2Promraster_Converter::output_prom_raster2file(void){
	string buffer;
	buffer=this->file_name.append(".out");

	ostringstream prefix;
	prefix << "GIS2PROM> ";
	Sys_Common_Output::output_hyd->set_userprefix(prefix.str());
	ostringstream cout;
	cout << "Output the generated raster to file "<<buffer.c_str()<<"..."<<endl;
	Sys_Common_Output::output_hyd->output_txt(&cout);

	ofstream output;
	//open the file
	output.open(this->file_name.c_str());
	if(output.is_open()==false){
		Error msg=this->set_error(16);
		ostringstream info;
		info << "Filename " << this->file_name << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
	Hyd_Multiple_Hydraulic_Systems::check_stop_thread_flag();
	output<<"#****************************************************************************************************************"<<endl;
	output<<"#  Automatically generated ProMaIDes HYD-raster for a 2D-hydraulic simulation"<<endl;
	output<<"#  Raster infos   : "<<this->file_name << endl;
	output<<"#  Output for the .ilm file..."<<endl;
	output<<"#!GENERAL= <SET>   								# 	Floodplainmodel defining parameters "<<endl;
	output<<"#   $NX = "<<this->geo_info.number_x					<<"  #   Number of elements in x-direction "<<endl;       
	output<<"#   $NY = "<<this->geo_info.number_y					<<"  #   Number of elements in y-direction  "<<endl;   
	output<<"#   $ELEMWIDTH_X = "<<this->geo_info.width_x     		<<"  #   Width of the element in x-direction "<<endl;
	output<<"#   $ELEMWIDTH_Y = "<<this->geo_info.width_y     		<<"  #   Width of the element in y-direction "<<endl; 
	output<<"#   $ANGLE = "<<this->geo_info.angle					<<"  #   Angle of the coordinate system "<<endl;
	output<<"#   $LOWLEFTX = "<<this->geo_info.origin_global_x  	<<"  #   Low left X coordinate "<<endl;
	output<<"#   $LOWLEFTY = "<<this->geo_info.origin_global_y   	<<"  #   Low left Y coordinate "<<endl;
	output<<"#   $NOINFOVALUE =  "<< this->noinfo_value				<<"  # 	 Value for an element with no information "<<endl;
	output<<"#  </SET> "<<endl;
	output<<"#****************************************************************************************************************"<<endl;


	output<<"# elementnr[-]	z-value[m] mattype[#;default:7]	init-condition[m]  boundarycondition[true/false] stationary/instationary[true/false]    boundaryvalue_stat[m3/s]/boundary-curvenr_instat[-]   boundary-type(area[(m³/s)/m²], point[m³/s])"<<endl;
	output<<"!BEGIN"<<endl;

	for(int i=0; i< this->number_polygons; i++){
		output << " " <<i<<"   "<< this->second_infos[i].last()<< "    " << 7 << "   " << 0.0 <<"   "<<"false     false    0"<<endl; 
	}
	output<<"!END"<<endl;


	output.close();
	cout << "Output of the generated raster to file "<<buffer.c_str()<<" is finsihed!"<<endl;
	Sys_Common_Output::output_hyd->output_txt(&cout);
	Sys_Common_Output::output_hyd->rewind_userprefix();
}
//_________________
//private
//Read the raster information per file
void Hyd_Gis2Promraster_Converter::read_raster_infos(ifstream *ifile, int *line_counter){
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

		if(myline==hyd_label::begin_rasterinfo){
			found_flag=true;
			break;
		}
	}
	while(ifile->eof()==false);
	if(found_flag==false){
		Error msg=this->set_error(5);
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
		if(myline==hyd_label::end_rasterinfo){
			found_flag=true;
			break;
		}
		else if(myline==hyd_label::begin_charac){
			break;
		}
		else if(myline.empty()==false){
			this->find_key_values_file(myline, &must_found_counter);
		}

	}
	while(ifile->eof()==false);

	if(found_flag==false){
		Error msg=this->set_error(6);
		throw msg;
	}

	if(must_found_counter!=7){
		Error msg=this->set_error(7);
		throw msg;
	}

	//check the raster information
	this->check_raster_info();
}
//Read the charactersistics of the elements per file
void Hyd_Gis2Promraster_Converter::read_raster_elem_characteristic(ifstream *ifile, int *line_counter){

	if(this->prom_raster==true){
		return;
	}


	//read in
	string myline;
	stringstream my_stream;
	bool found_flag=false;

	//search for the begin
	do{
		getline(*ifile, myline,'\n');
		(*line_counter)++;
		functions::clean_string(&myline);

		if(myline==hyd_label::begin_charac){
			found_flag=true;
			break;
		}
	}
	while(ifile->eof()==false);
	if(found_flag==false){
		Error msg=this->set_error(10);
		throw msg;

	}
	else{
		found_flag=false;
	}

	int counter_col=0;
	int counter_row=0;
	int counter_total=0;
	int pos=-1;
	double buffer_info=0.0;

	//read in the charactersistics
	do{
		getline(*ifile, myline,'\n');
		(*line_counter)++;
		functions::clean_string(&myline);
		pos=myline.find(hyd_label::end_charac);
		if(pos>=0){
			found_flag=true;
			break;
		}
		if(counter_row==this->geo_info.number_y){
			break;
		}
		if(myline.empty()==false){
			counter_row++;
			do{
				string buffer= functions::get_column(&myline);
				my_stream << buffer;
				my_stream >> buffer_info;
				Hyd_Multiple_Hydraulic_Systems::check_stop_thread_flag();
				this->second_infos[counter_total].append(buffer_info);

				if(my_stream.fail()==true){
					my_stream.clear();
					ostringstream info;
					info << "Wrong input sequenze  : " << my_stream.str() << myline << endl;
					Error msg=this->set_error(12);
					msg.make_second_info(info.str());
					throw msg;
				}
				my_stream.str("");
				my_stream.clear();
				counter_col++;
				counter_total++;

				if(counter_col==this->geo_info.number_x){
					break;
				}
			}
			while(myline.empty()==false);
		

			if(myline.empty()!=false){
				Warning msg=this->set_warning(2);
				ostringstream info;
				info<< "No. of columns required :" << this->geo_info.number_x << endl;
				info<< "No. of columns found    :" << counter_col;
				info <<"Error occurs near line  : "<< line_counter << endl;
				msg.make_second_info(info.str());
				msg.output_msg(2);

			}
			if(counter_col<this->geo_info.number_x){
				Error msg=this->set_error(11);
				ostringstream info;
				info<< "No. of columns required :" << this->geo_info.number_x << endl;
				info<< "No. of columns found    :" << counter_col;
				msg.make_second_info(info.str());
				throw msg;
			}
			counter_col=0;
		}

	}
	while(ifile->eof()==false);

	if(counter_total!=this->number_polygons){
		Error msg=this->set_error(13);
		ostringstream info;
		info<< "No. of characteristics required :" << this->number_polygons << endl;
		info<< "No. of characteristics found    :" << counter_total;
		msg.make_second_info(info.str());
		throw msg;

	}
	if(found_flag==false){
		Error msg=this->set_error(14);
		throw msg;
	}
}
//Find the key values for the file input
void Hyd_Gis2Promraster_Converter::find_key_values_file(string myline , int *must_found_counter){
	int pos=-1;
	string buffer;
	stringstream buffer1;
	bool wrong_input=false;

	pos=myline.find(hyd_label::raster_no_x );
	if(pos>=0 && wrong_input==false){
		buffer=myline.substr(hyd_label::raster_no_x.length());
		functions::clean_string(&buffer);
		buffer1 << buffer;
		buffer1 >> this->geo_info.number_x;
		if(buffer1.fail()==true){
			wrong_input=true;
		}
		else{
			(*must_found_counter)++;
			return;
		}

	}
	pos=myline.find(hyd_label::raster_no_y);
	if(pos>=0 && wrong_input==false){
		buffer=myline.substr(hyd_label::raster_no_y.length());
		functions::clean_string(&buffer);
		buffer1 << buffer;
		buffer1 >> this->geo_info.number_y;
		if(buffer1.fail()==true){
			wrong_input=true;
		}
		else{
			(*must_found_counter)++;
			return;
		}

	}
	pos=myline.find(hyd_label::raster_x_origin);
	if(pos>=0 && wrong_input==false){
		buffer=myline.substr(hyd_label::raster_x_origin.length());
		functions::clean_string(&buffer);
		buffer1 << buffer;
		buffer1 >> this->geo_info.origin_global_x;
		if(buffer1.fail()==true){
			wrong_input=true;
		}
		else{
			(*must_found_counter)++;
			return;
		}

	}
	pos=myline.find(hyd_label::raster_y_origin);
	if(pos>=0 && wrong_input==false){
		buffer=myline.substr(hyd_label::raster_y_origin.length());
		functions::clean_string(&buffer);
		buffer1 << buffer;
		buffer1 >> this->geo_info.origin_global_y;
		if(buffer1.fail()==true){
			wrong_input=true;
		}
		else{
			(*must_found_counter)++;
			return;
		}

	}
	pos=myline.find(hyd_label::raster_width_x);
	if(pos>=0 && wrong_input==false){
		buffer=myline.substr(hyd_label::raster_width_x.length());
		functions::clean_string(&buffer);
		buffer1 << buffer;
		buffer1 >> this->geo_info.width_x;
		if(buffer1.fail()==true){
			wrong_input=true;
		}
		else{
			(*must_found_counter)++;
			return;
		}
	}
	pos=myline.find(hyd_label::raster_width_y);
	if(pos>=0 && wrong_input==false){
		buffer=myline.substr(hyd_label::raster_width_y.length());
		functions::clean_string(&buffer);
		buffer1 << buffer;
		buffer1 >> this->geo_info.width_y;
		if(buffer1.fail()==true){
			wrong_input=true;
		}
		else{
			(*must_found_counter)++;
			return;
		}
	}
	if(this->prom_raster==true){
		pos=myline.find(hyd_label::raster_angle);
		if(pos>=0 && wrong_input==false){
			buffer=myline.substr(hyd_label::raster_angle.length());
			functions::clean_string(&buffer);
			buffer1 << buffer;
			buffer1 >> this->geo_info.angle;
			if(buffer1.fail()==true){
				wrong_input=true;
			}
			else{
				(*must_found_counter)++;
				return;
			}
		}
	}


	if(this->prom_raster==false){
		pos=myline.find(hyd_label::raster_noinfo);
		if(pos>=0 && wrong_input==false){
			buffer=myline.substr(hyd_label::raster_noinfo.length());
			functions::clean_string(&buffer);
			buffer1 << buffer;
			buffer1 >> this->noinfo_value;
			if(buffer1.fail()==true){
				wrong_input=true;
			}
			else{
				(*must_found_counter)++;
				return;
			}
		}
	}

	if(wrong_input==true){
		ostringstream info;
		info << "Wrong input sequenze  : " << buffer1.str() << endl;
		Error msg=this->set_error(9);
		msg.make_second_info(info.str());
		throw msg;
	}

}
//Allocate the secondary information per raster element
void Hyd_Gis2Promraster_Converter::allocate_secondary_info(void){
	try{
		this->second_infos=new QList<double>[this->number_polygons];
	}
	catch(bad_alloc &t){
		Error msg=this->set_error(0);
		ostringstream info;
		info << t.what() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
}
//Delete the secondary information per raster element 
void Hyd_Gis2Promraster_Converter::delete_secondary_info(void){
	if(this->second_infos!=NULL){
		delete []this->second_infos;
		this->second_infos=NULL;
	}
}
//Check the geometrical information of the raster
void Hyd_Gis2Promraster_Converter::check_raster_info(void){
	ostringstream info;
	if(this->geo_info.number_x<=0){
		Error msg=this->set_error(8);
		info <<"The number of raster elements in x-direction must be greater 0" << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
	if(this->geo_info.number_y<=0){
		Error msg=this->set_error(8);
		info <<"The number of raster elements in y-direction must be greater 0" << endl;
		msg.make_second_info(info.str());
		throw msg;

	}
	if(this->geo_info.width_x<=0){
		Error msg=this->set_error(8);
		info <<"The width of the raster elements in x-direction must be greater 0"<< label::m << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
	if(this->geo_info.width_y<=0){
		Error msg=this->set_error(8);
		info <<"The width of the raster elements in y-direction must be greater 0"<< label::m << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
	if(this->prom_raster==false){
		if(this->geo_info.width_x!=this->geo_info.width_y){
			Warning msg=this->set_warning(1);
			ostringstream info;
			info << " Element width x-direction : " << this->geo_info.width_x<< label::m<<endl;
			info << " Element width y-direction : " << this->geo_info.width_y<< label::m<<endl;
			msg.output_msg(2);
			//reaction
			this->geo_info.width_y=this->geo_info.width_x;
		}	
	}

}
//Sort raster characteristics from read down-right to up-left
void Hyd_Gis2Promraster_Converter::sort_raster_characteristics(void){
	double *buffer=NULL;
	//allocate the buffer
	try{
		buffer=new double[this->number_polygons];
		//count the memory
		Sys_Memory_Count::self()->add_mem(sizeof(double)*this->number_polygons, _sys_system_modules::HYD_SYS);
	}
	catch(bad_alloc &t){
		Error msg=this->set_error(15);
		ostringstream info;
		info<< "Info bad alloc: " << t.what() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}

	//copy it
	for(int i=0; i< this->number_polygons; i++){
		buffer[i]=this->second_infos[i].at(0);
	}


	//sort it
	int counter=0;
	for(int i=0; i< this->geo_info.number_y; i++){
		for(int j=0; j< this->geo_info.number_x; j++){
			this->second_infos[counter].replace(0,buffer[this->number_polygons-(i+1)*(this->geo_info.number_x)+j]);	
			counter++;
		}
	}

	//delete the buffer
	if(buffer!=NULL){
		delete []buffer;
		//count the memory
		Sys_Memory_Count::self()->minus_mem(sizeof(double)*this->number_polygons, _sys_system_modules::HYD_SYS);
		buffer=NULL;
	}
}
//Check raster as identical
bool Hyd_Gis2Promraster_Converter::check_raster_identical(Hyd_Gis2Promraster_Converter *gis_raster){
	if(abs(this->geo_info.angle-gis_raster->geo_info.angle)>constant::angle_epsilon){
		return false;
	}
	if(this->geo_info.number_x!=gis_raster->geo_info.number_x){
		return false;
	}
	if(this->geo_info.number_y!=gis_raster->geo_info.number_y){
		return false;
	}
	if(abs(this->geo_info.origin_global_x-gis_raster->geo_info.origin_global_x)>constant::meter_epsilon){
		return false;
	}
	if(abs(this->geo_info.origin_global_y-gis_raster->geo_info.origin_global_y)>constant::meter_epsilon){
		return false;
	}
	if(abs(this->geo_info.width_x-gis_raster->geo_info.width_x)>constant::meter_epsilon){
		return false;
	}
	if(abs(this->geo_info.width_y-gis_raster->geo_info.width_y)>constant::meter_epsilon){
		return false;
	}

	return true;

}
//Search for the elemet
void Hyd_Gis2Promraster_Converter::search_element(Geo_Point *mid, Hyd_Gis2Promraster_Converter *gis_raster, const int id_element){

	bool list_required=false;
	if(this->outside_list.count()>0){
		list_required=true;
	}

	//direction search
	int buff_id=this->last_found_id;
	//last one
	this->set_geometrical_raster_polygon(buff_id);
	if(this->raster_elem.check_point_inside_atboundary(mid)==true){
			this->second_infos[buff_id].append(gis_raster->second_infos[id_element].last());
			return;
	}
	//next x
	_hyd_all_element_direction dir;
	dir=_hyd_all_element_direction::X_DIRECT;
	buff_id=this->convert_2neighbouring_elemindex(dir, this->last_found_id);
	if(buff_id>=0){
		this->set_geometrical_raster_polygon(buff_id);
		if(this->raster_elem.check_point_inside_atboundary(mid)==true){
				this->second_infos[buff_id].append(gis_raster->second_infos[id_element].last());
				this->last_found_id=buff_id;
				return;
		}
	}
	//next xy
	dir=_hyd_all_element_direction::XY_DIRECT;
	buff_id=this->convert_2neighbouring_elemindex(dir, this->last_found_id);
	if(buff_id>=0){
		this->set_geometrical_raster_polygon(buff_id);
		if(this->raster_elem.check_point_inside_atboundary(mid)==true){
				this->second_infos[buff_id].append(gis_raster->second_infos[id_element].last());
				this->last_found_id=buff_id;
				return;
		}
	}
	//next y
	dir=_hyd_all_element_direction::Y_DIRECT;
	buff_id=this->convert_2neighbouring_elemindex(dir, this->last_found_id);
	if(buff_id>=0){
		this->set_geometrical_raster_polygon(buff_id);
		if(this->raster_elem.check_point_inside_atboundary(mid)==true){
				this->second_infos[buff_id].append(gis_raster->second_infos[id_element].last());
				this->last_found_id=buff_id;
				return;
		}
	}
	//next y-x
	dir=_hyd_all_element_direction::YMINUS_X_DIRECT;
	buff_id=this->convert_2neighbouring_elemindex(dir, this->last_found_id);
	if(buff_id>=0){
		this->set_geometrical_raster_polygon(buff_id);
		if(this->raster_elem.check_point_inside_atboundary(mid)==true){
				this->second_infos[buff_id].append(gis_raster->second_infos[id_element].last());
				this->last_found_id=buff_id;
				return;
		}
	}
	//next -x
	dir=_hyd_all_element_direction::MINUS_X_DIRECT;
	buff_id=this->convert_2neighbouring_elemindex(dir, this->last_found_id);
	if(buff_id>=0){
		this->set_geometrical_raster_polygon(buff_id);
		if(this->raster_elem.check_point_inside_atboundary(mid)==true){
				this->second_infos[buff_id].append(gis_raster->second_infos[id_element].last());
				this->last_found_id=buff_id;
				return;
		}
	}
	//next -x-y
	dir=_hyd_all_element_direction::MINUS_X_MINUS_Y_DIRECT;
	buff_id=this->convert_2neighbouring_elemindex(dir, this->last_found_id);
	if(buff_id>=0){
		this->set_geometrical_raster_polygon(buff_id);
		if(this->raster_elem.check_point_inside_atboundary(mid)==true){
				this->second_infos[buff_id].append(gis_raster->second_infos[id_element].last());
				this->last_found_id=buff_id;
				return;
		}
	}
	//next -y
	dir=_hyd_all_element_direction::MINUS_Y_DIRECT;
	buff_id=this->convert_2neighbouring_elemindex(dir, this->last_found_id);
	if(buff_id>=0){
		this->set_geometrical_raster_polygon(buff_id);
		if(this->raster_elem.check_point_inside_atboundary(mid)==true){
				this->second_infos[buff_id].append(gis_raster->second_infos[id_element].last());
				this->last_found_id=buff_id;
				return;
		}
	}
	//next x-y
	dir=_hyd_all_element_direction::MINUS_Y_X_DIRECT;
	buff_id=this->convert_2neighbouring_elemindex(dir, this->last_found_id);
	if(buff_id>=0){
		this->set_geometrical_raster_polygon(buff_id);
		if(this->raster_elem.check_point_inside_atboundary(mid)==true){
				this->second_infos[buff_id].append(gis_raster->second_infos[id_element].last());
				this->last_found_id=buff_id;
				return;
		}
	}
	
	


	//normal search
	int start=this->last_found_id;
	if(start!=0){
		start=start-1;
	}
	int counter=0;
	for(int i=start; i< this->number_polygons; i++){
		if(list_required==true && this->outside_list.at(i)==true){
			continue;
		}

		this->set_geometrical_raster_polygon(i);
		counter++;
		if(this->raster_elem.check_point_inside_atboundary(mid)==true){
			this->second_infos[i].append(gis_raster->second_infos[id_element].last());
			this->last_found_id=i;
			break;
		}
		if(i==this->number_polygons-1 && counter<this->number_polygons){
			start=0;
			i=0;
		}
		if(counter==this->number_polygons){
			this->last_found_id=0;
			break;
		}
	}
}
//Calculate the value for each element
void Hyd_Gis2Promraster_Converter::calculate_value_element(const double no_info){
	ostringstream cout;
	cout << "Calculate the values of the secondary information per element of the ProMaIdes HYD-raster..."<<endl;
	Sys_Common_Output::output_hyd->output_txt(&cout);


	double sum=0.0;
	for(int i=0; i< this->number_polygons; i++){
		if(this->second_infos[i].count()==0){
			this->second_infos[i].append(no_info);
		}
		else{
			sum=0.0;
			for(int j=0; j< this->second_infos[i].count(); j++){
				sum=sum+this->second_infos[i].at(j);
			}
			sum=sum/(double)this->second_infos[i].count();
			this->second_infos[i].append(sum);
		}
	}

	cout << "Calculation of the values of the secondary information per element of the ProMaIdes HYD-raster is finished!"<<endl;
	Sys_Common_Output::output_hyd->output_txt(&cout);
}
//Set if elements are completely outside the raster
void Hyd_Gis2Promraster_Converter::set_completly_ouside_elements(Hyd_Floodplain_Polygon *gis_bound){

	ostringstream cout;
	cout << "Set the completly ouside-falgs for each element of the ProMaIdes HYD-raster..."<<endl;
	Sys_Common_Output::output_hyd->output_txt(&cout);

	Geo_Point mid;
	for(int i=0; i<this->number_polygons; i++){
		this->set_geometrical_raster_polygon(i);
		this->raster_elem.set_mid_point(&mid);
		if(gis_bound->check_point_outside(&mid)==true){
			this->outside_list.replace(i,true);
		}
	}
	cout << "The setting of the completly ouside-falgs for each element of the ProMaIdes HYD-raster is finished!"<<endl;
	Sys_Common_Output::output_hyd->output_txt(&cout);

}
//Set warning(s)
Warning Hyd_Gis2Promraster_Converter::set_warning(const int warn_type){
		string place="Hyd_Gis2Promraster_Converter::";
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
		case 1://element width have to be eqaul
			place.append("check_raster_info(void)");
			reason="The width of the Gis-element have to be equal";
			help="Check the element width in file";
			reaction="The width in x-direction is taken";
			type=1;
			break;
		case 2://more columns to read in than required
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
};
//Set error(s)
Error Hyd_Gis2Promraster_Converter::set_error(const int err_type){
	string place="Hyd_Gis2Promraster_Converter::";
	string help;
	string reason;
	int type=0;
	bool fatal=false;
	stringstream info;
	Error msg;
	switch (err_type){
		case 0://bad alloc
			place.append("allocate_secondary_info(void)");
			reason="Can not allocate the memory";
			help="Check the memory";
			type=10;
			break;
		case 1://can not open file
			place.append("input_gis_raster(QString file_name)");
			reason="Can not open the file of the Gis-raster";
			help="Check the file";
			type=5;
			break;
		case 2://can not open file
			place.append("input_prom_raster(QString file_name)");
			reason="Can not open the file of the Prom-raster";
			help="Check the file";
			type=5;
			break;
		case 3://file name is not set
			place.append("input_gis_raster(QString file_name)");
			reason="No file name is set";
			help="Check the file";
			type=5;
			break;
		case 4://file name is not set
			place.append("input_prom_raster(QString file_name)");
			reason="No file name is set";
			help="Check the file";
			type=5;
			break;
		case 5://do not find the begin of the raster information in file
			place.append("read_raster_infos(ifstream *ifile, int *line_counter)");
			reason="Can not find the begin of the raster information in file";
			help="The raster information has to start with !$BEGIN_RASTERINFO";
			type=1;
			break;
		case 6://do not find the end of the raster information in file
			place.append("read_raster_infos(ifstream *ifile, int *line_counter)");
			reason="Can not find the end of the raster information in file";
			help="The raster information has to end with !$END_RASTERINFO";
			type=1;
			break;
		case 7://do not find all required raster infos
			place.append("read_raster_infos(ifstream *ifile, int *line_counter)");
			reason="Can not find all required raster information";
			help="Check if all raster information are set in the file";
			info << "Required information:"<<endl;
			info << " " << hyd_label::raster_no_x<< endl;
			info << " " << hyd_label::raster_no_y<< endl;
			info << " " << hyd_label::raster_x_origin<< endl;
			info << " " << hyd_label::raster_y_origin<< endl;
			info << " " << hyd_label::raster_width_x<< endl;
			info << " " << hyd_label::raster_width_y<< endl;
			if(this->prom_raster==true){
				info << " " << hyd_label::raster_angle<< endl;
			}
			if(this->prom_raster==false){
				info << " " << hyd_label::raster_noinfo<< endl;
			}
			type=1;
			break;
		case 8://error in the raster information
			place.append("check_raster_info(void)");
			reason="The raster informations are not properly set";
			help="Check the raster informations";
			type=11;
			break;
		case 9://wrong input
			place.append("find_key_values_file(string myline , int *must_found_counter)");
			reason="There is a problem with the input; wrong sign is read in";
			help="Check the raster information in file";
			type=1;
			break;
		case 10://do not find the begin of the characteristics of the raster elements in file
			place.append("read_raster_elem_characteristic(ifstream *ifile, int *line_counter)");
			reason="Can not find the begin of the characterístic information of the raster elements in file";
			help="The raster information has to start with !$BEGIN_CHARAC";
			type=1;
			break;
		case 11://not enough columns to read in the characteristis
			place.append("read_raster_elem_characteristic(ifstream *ifile, int *line_counter)");
			reason="There are not enough columns in file for the input of the element characteristics";
			help="Check the raster element information in file";
			type=1;
			break;
		case 12://wrong input
			place.append("read_raster_elem_characteristic(ifstream *ifile, int *line_counter)");
			reason="There is a problem with the input; wrong sign is read in";
			help="Check the raster element characteristics in file";
			type=1;
			break;
		case 13://not enough values to read in the characteristis
			place.append("read_raster_elem_characteristic(ifstream *ifile, int *line_counter)");
			reason="The number of given characteristics in file do not correspond to the required number";
			help="Check the raster element characteristics in file";
			type=1;
			break;
		case 14://do not find the end of the characteristics of the raster elements in file
			place.append("read_raster_elem_characteristic(ifstream *ifile, int *line_counter)");
			reason="Can not find the end of the characterístic information of the raster elements in file";
			help="The raster information has to start with !$END_CHARAC";
			type=1;
			break;
		case 15://bad alloc
			place.append("sort_raster_characteristics(void)");
			reason="Can not allocate the memory";
			help="Check the memory";
			type=10;
			break;
		case 16://can not open file
			place.append("output_prom_raster2file(QString file_name)");
			reason="Can not open the output file";
			help="Check the output file";
			type=10;
			break;
		case 17://diagonal is greater than width
			place.append("intercept_rasters(Hyd_Gis2Promraster_Converter *gis_raster)");
			reason="The diagonal between element midpoints of the GIS-raster is greater than the minimum of the element width of the Prom-raster; not all element of the Prom-raster may get a secondary information value";
			help="Check raster information";
			type=9;
			break;
		case 18://width of gis elem is greater than width of prom elem
			place.append("intercept_rasters(Hyd_Gis2Promraster_Converter *gis_raster)");
			reason="The width of the GIS-raster is/are greater than the width of the Prom-raster elements; not all element of the Prom-raster may get a secondary information value";
			help="Check raster information";
			type=9;
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

