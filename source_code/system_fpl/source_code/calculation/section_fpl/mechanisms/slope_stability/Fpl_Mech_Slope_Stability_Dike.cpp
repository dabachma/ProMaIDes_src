#include "Fpl_Headers_Precompiled.h"
//#include "Fpl_Mech_Slope_Stability_Dike.h"

//init static members
Tables *Fpl_Mech_Slope_Stability_Dike::control_table=NULL;

//Default constructor
Fpl_Mech_Slope_Stability_Dike::Fpl_Mech_Slope_Stability_Dike(void){
	this->x_origin=0.0;
	this->y_origin=0.0;
	this->delta_x=fpl_slope_control_param::def_delta_x;
	this->delta_y=fpl_slope_control_param::def_delta_y;
	this->number_x=fpl_slope_control_param::def_number_x;
	this->number_y=fpl_slope_control_param::def_number_y;
	this->delta_radius=fpl_slope_control_param::def_delta_radius;
	this->landside_flag=true;
	this->delta_h=fpl_slope_control_param::def_delta_h;
	this->slice_width=fpl_slope_control_param::def_slice_width;

	this->number_circle_no=0;
	this->number_circle_one = 0;
	this->number_circle_more_two=0;
	this->number_circle_area_small=0;
	this->number_circle_y_small=0;
	this->number_circle_geo_error=0;
	this->number_circle_reduced_slice_width=0;
	this->number_slice2small=0;
	this->number_circle_mid_in_polygon=0;
	this->number_beyond_crest_mid=0;
	this->number_circle_outer_bound=0;
	this->number_circle_radius_change=0;

	this->number_slip_circle=0;
	this->slip_circle=NULL;
	this->number_relevant_slip_circle=0;
	this->number_radius=0;

	this->maximum_radius=0.0;

	this->number_geo_material_zones=0;
	this->ptr_geo_material_zones=NULL;

	this->relevant_macro_circle=NULL;
	this->relevant_micro_circle=NULL;

	this->counter_single_prob_micro=0;
	this->counter_tree_prob_micro=0;

	Sys_Memory_Count::self()->add_mem(sizeof(Fpl_Mech_Slope_Stability_Dike), _sys_system_modules::FPL_SYS);//count the memory
}
//Default destructor
Fpl_Mech_Slope_Stability_Dike::~Fpl_Mech_Slope_Stability_Dike(void){
	this->delete_slip_circle();

	Sys_Memory_Count::self()->minus_mem(sizeof(Fpl_Mech_Slope_Stability_Dike), _sys_system_modules::FPL_SYS);//count the memory
}
//_________
//public
//Create the database table for the control parameters of the slope mechanism (static)
void Fpl_Mech_Slope_Stability_Dike::create_control_table(QSqlDatabase *ptr_database){
	if(Fpl_Mech_Slope_Stability_Dike::control_table==NULL){
			ostringstream cout;
			cout << "Create slope mechanism control database table..." << endl ;
			Sys_Common_Output::output_fpl->output_txt(&cout);
			//make specific input for this class
			const string tab_name=fpl_label::tab_fpl_control_slope;
			const int num_col=12;
			_Sys_data_tab_column tab_col[num_col];
			//init
			for(int i=0; i< num_col; i++){
				tab_col[i].key_flag=false;
				tab_col[i].unsigned_flag=false;
				tab_col[i].primary_key_flag=false;
			}

			tab_col[0].name=fpl_label::glob_id;
			tab_col[0].type=sys_label::tab_col_type_int;
			tab_col[0].unsigned_flag=true;
			tab_col[0].primary_key_flag=true;

			tab_col[1].name=fpl_label::section_id;
			tab_col[1].type=sys_label::tab_col_type_int;
			tab_col[1].unsigned_flag=true;
			tab_col[1].key_flag=true;

			tab_col[2].name=fpl_label::landside_flag;
			tab_col[2].type=sys_label::tab_col_type_bool;
			tab_col[2].default_value="true";
			tab_col[2].key_flag=true;

			tab_col[3].name=fpl_label::x_origin;
			tab_col[3].type=sys_label::tab_col_type_double;

			tab_col[4].name=fpl_label::y_origin;
			tab_col[4].type=sys_label::tab_col_type_double;

			tab_col[5].name=fpl_label::no_x;
			tab_col[5].type=sys_label::tab_col_type_int;

			tab_col[6].name=fpl_label::no_y;
			tab_col[6].type=sys_label::tab_col_type_int;

			tab_col[7].name=fpl_label::delta_x;
			tab_col[7].type=sys_label::tab_col_type_double;

			tab_col[8].name=fpl_label::delta_y;
			tab_col[8].type=sys_label::tab_col_type_double;

			tab_col[9].name=fpl_label::delta_r;
			tab_col[9].type=sys_label::tab_col_type_double;

			tab_col[10].name=fpl_label::delta_h;
			tab_col[10].type=sys_label::tab_col_type_double;

			tab_col[11].name=fpl_label::slice_width;
			tab_col[11].type=sys_label::tab_col_type_double;

			try{
				Fpl_Mech_Slope_Stability_Dike::control_table= new Tables();
				if(Fpl_Mech_Slope_Stability_Dike::control_table->create_non_existing_tables(tab_name, tab_col, num_col,ptr_database, _sys_table_type::fpl)==false){
					cout << " Table exists" << endl ;
					Sys_Common_Output::output_fpl->output_txt(&cout);
				};
			}
			catch(bad_alloc& t){
				Error msg;
				msg.set_msg("Fpl_Mech_Slope_Stability_Dike::create_control_table(QSqlDatabase *ptr_database)","Can not allocate the memory", "Check the memory", 10, false);
				ostringstream info;
				info<< "Info bad alloc: " << t.what() << endl;
				msg.make_second_info(info.str());
				throw msg;
			}
			catch(Error msg){
				Fpl_Mech_Slope_Stability_Dike::close_control_table();
				throw msg;
			}

		Fpl_Mech_Slope_Stability_Dike::close_control_table();
	}
}
//Delete all data in the database table for the control parameters of the slope mechanism (static)
void Fpl_Mech_Slope_Stability_Dike::delete_data_in_control_table(QSqlDatabase *ptr_database){
	//the table is set (the name and the column names) and allocated
	try{
		Fpl_Mech_Slope_Stability_Dike::set_control_table(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}
	//delete the table
	Fpl_Mech_Slope_Stability_Dike::control_table->delete_data_in_table(ptr_database);
}
//Delete all data in the database table for the control parameters of the slope mechanism for a specific fpl-section (static)
void Fpl_Mech_Slope_Stability_Dike::delete_data_in_control_table(QSqlDatabase *ptr_database, const int sec_id){
	//the table is set (the name and the column names) and allocated
	try{
		Fpl_Mech_Slope_Stability_Dike::set_control_table(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}
	QSqlQueryModel results;
	ostringstream test_filter;
	//delete
	test_filter<< "Delete ";
	test_filter << " from " << Fpl_Mech_Slope_Stability_Dike::control_table->get_table_name();
	test_filter << " where ";
	test_filter << Fpl_Mech_Slope_Stability_Dike::control_table->get_column_name(fpl_label::section_id) << " = " << sec_id ;
	Data_Base::database_request(&results, test_filter.str(), ptr_database);

	//check the request
	if(results.lastError().isValid()){
		Error msg;
		msg.set_msg("Fpl_Mech_Slope_Stability_Dike::delete_data_in_control_table(QSqlDatabase *ptr_database, const int sec_id)","Invalid database request", "Check the database", 2, false);
		ostringstream info;
		info << "Table Name      : " << Fpl_Mech_Slope_Stability_Dike::control_table->get_table_name() << endl;
		info << "Table error info: " << results.lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
}
//Set the database table for the control parameters of the slope mechanism: it sets the table name and the name of the columns and allocate them (static)
void Fpl_Mech_Slope_Stability_Dike::set_control_table(QSqlDatabase *ptr_database){
	if(Fpl_Mech_Slope_Stability_Dike::control_table==NULL){
		//make specific input for this class
		const string tab_id_name=fpl_label::tab_fpl_control_slope;
		string tab_col[12];
		tab_col[0]=fpl_label::glob_id;
		tab_col[1]=fpl_label::section_id;
		tab_col[2]=fpl_label::landside_flag;
		tab_col[3]=fpl_label::x_origin;
		tab_col[4]=fpl_label::y_origin;
		tab_col[5]=fpl_label::delta_x;
		tab_col[6]=fpl_label::delta_y;
		tab_col[7]=fpl_label::delta_r;
		tab_col[8]=fpl_label::delta_h;
		tab_col[9]=fpl_label::slice_width;
		tab_col[10]=fpl_label::no_x;
		tab_col[11]=fpl_label::no_y;

		try{
			Fpl_Mech_Slope_Stability_Dike::control_table= new Tables(tab_id_name, tab_col, sizeof(tab_col)/sizeof(tab_col[0]));
			Fpl_Mech_Slope_Stability_Dike::control_table->set_name(ptr_database, _sys_table_type::fpl);
		}
		catch(bad_alloc&t ){
			Error msg;
			msg.set_msg("void Fpl_Mech_Slope_Stability_Dike::set_control_table(QSqlDatabase *ptr_database)","Can not allocate the memory", "Check the memory", 10, false);
			ostringstream info;
			info<< "Info bad alloc: " << t.what() << endl;
			msg.make_second_info(info.str());
			throw msg;
		}
		catch(Error msg){
			Fpl_Mech_Slope_Stability_Dike::close_control_table();
			throw msg;
		}
	}
}
//Close and delete the database table for the control parameters of the slope mechanism (static)
void Fpl_Mech_Slope_Stability_Dike::close_control_table(void){
	if(Fpl_Mech_Slope_Stability_Dike::control_table!=NULL){
		delete Fpl_Mech_Slope_Stability_Dike::control_table;
		Fpl_Mech_Slope_Stability_Dike::control_table=NULL;
	}
}
//Select the control parameters of a fpl-section in a database table (static)
int Fpl_Mech_Slope_Stability_Dike::select_control_parameter_database(QSqlQueryModel *results, QSqlDatabase *ptr_database, const int sec_id, const bool landside_flag){
	int number=0;
	try{
		Fpl_Mech_Slope_Stability_Dike::set_control_table(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}

	ostringstream test_filter;
	test_filter<< "Select * ";

	test_filter << " from " << Fpl_Mech_Slope_Stability_Dike::control_table->get_table_name();
	test_filter << " where " ;
	test_filter << Fpl_Mech_Slope_Stability_Dike::control_table->get_column_name(fpl_label::section_id) << " = " << sec_id;
	test_filter << " and ";
	test_filter << Fpl_Mech_Slope_Stability_Dike::control_table->get_column_name(fpl_label::landside_flag) << "= "<<functions::convert_boolean2string(landside_flag);

	Data_Base::database_request(results, test_filter.str(), ptr_database);

	//check the request
	if(results->lastError().isValid()){
		Warning msg;
		msg.set_msg("int Fpl_Mech_Slope_Stability_Dike::select_control_parameter_database(QSqlQueryModel *results, QSqlDatabase *ptr_database, const int sec_id, const bool landside_flag)","Invalid database request", "Check the database", "", 2);
		ostringstream info;
		info << "Table Name      : " << Fpl_Mech_Slope_Stability_Dike::control_table->get_table_name() << endl;
		info << "Table error info: " << results->lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		msg.output_msg(1);
	}

	number= results->rowCount();

	return number;
}
//Set the random variables of the result members per database table
void Fpl_Mech_Slope_Stability_Dike::set_result_variables(QSqlDatabase *ptr_database, const _sys_system_id id, const int section_id){
	try{
		if(this->landside_flag==true){
			this->stability_macro.set_input(id, section_id,fpl_label::stab_slope_land_macro, true, ptr_database, fpl_label::mech_slope, fpl_label::sec_dike, 0, false);
			this->stability_macro.set_distribution_types(ptr_database);
			this->stability_micro.set_input(id, section_id,fpl_label::stab_slope_land_micro, true, ptr_database, fpl_label::mech_slope, fpl_label::sec_dike, 0, false);
			this->stability_micro.set_distribution_types(ptr_database);
		}
		else{
			this->stability_macro.set_input(id, section_id,fpl_label::stab_slope_water_macro, true, ptr_database, fpl_label::mech_slope, fpl_label::sec_dike, 0, false);
			this->stability_macro.set_distribution_types(ptr_database);
			this->stability_micro.set_input(id, section_id,fpl_label::stab_slope_water_micro, true, ptr_database, fpl_label::mech_slope, fpl_label::sec_dike, 0, false);
			this->stability_micro.set_distribution_types(ptr_database);
		}
	}catch(Error msg){
		throw msg;
	}
}
//Set the control parameter per database table
bool Fpl_Mech_Slope_Stability_Dike::set_control_parameter(QSqlDatabase *ptr_database, const int section_id){
	QSqlQueryModel result;
	try{
		if(Fpl_Mech_Slope_Stability_Dike::select_control_parameter_database(&result, ptr_database, section_id, this->landside_flag)==0){
			Warning msg=this->set_warning(6);
			ostringstream info;
			info <<" Table name         : " << Fpl_Mech_Slope_Stability_Dike::control_table->get_table_name()<<endl;
			info <<" Landside stability : " << functions::convert_boolean2string(this->landside_flag) <<endl;
			msg.make_second_info(info.str());
			msg.output_msg(1);
			return false;
		}
	}
	catch(Error msg){
		throw msg;
	}

	this->x_origin=result.record(0).value((Fpl_Mech_Slope_Stability_Dike::control_table->get_column_name(fpl_label::x_origin)).c_str()).toDouble();
	this->y_origin=result.record(0).value((Fpl_Mech_Slope_Stability_Dike::control_table->get_column_name(fpl_label::y_origin)).c_str()).toDouble();
	this->delta_x=result.record(0).value((Fpl_Mech_Slope_Stability_Dike::control_table->get_column_name(fpl_label::delta_x)).c_str()).toDouble();
	this->delta_y=result.record(0).value((Fpl_Mech_Slope_Stability_Dike::control_table->get_column_name(fpl_label::delta_y)).c_str()).toDouble();
	this->delta_radius=result.record(0).value((Fpl_Mech_Slope_Stability_Dike::control_table->get_column_name(fpl_label::delta_r)).c_str()).toDouble();
	this->delta_h=result.record(0).value((Fpl_Mech_Slope_Stability_Dike::control_table->get_column_name(fpl_label::delta_h)).c_str()).toDouble();
	this->slice_width=result.record(0).value((Fpl_Mech_Slope_Stability_Dike::control_table->get_column_name(fpl_label::slice_width)).c_str()).toDouble();
	this->number_x=result.record(0).value((Fpl_Mech_Slope_Stability_Dike::control_table->get_column_name(fpl_label::no_x)).c_str()).toInt();
	this->number_y=result.record(0).value((Fpl_Mech_Slope_Stability_Dike::control_table->get_column_name(fpl_label::no_y)).c_str()).toInt();

	this->check_control_parameters();
	return true;
}
//Set the random input variables per file
void Fpl_Mech_Slope_Stability_Dike::set_input(QFile *ifile, int *line_counter, const qint64 pos_file, const int line_counter_start){
	ostringstream prefix;
	if(this->landside_flag==true){
		prefix << "SLOPE_LAND> ";
	}
	else{
		prefix << "SLOPE_WATER> ";
	}
	Sys_Common_Output::output_fpl->set_userprefix(prefix.str());
	ostringstream cout;

	cout << "Input control parameters per file..." << endl;
	Sys_Common_Output::output_fpl->output_txt(&cout);
	try{
		this->set_controlparam_per_file(ifile, line_counter);
	}
	catch(Error msg){
		ostringstream info;
		info << " Landside slope : "<<functions::convert_boolean2string(this->landside_flag)<<endl;
		msg.make_second_info(info.str());
		throw msg;
	}
	ifile->seek(pos_file);
	*line_counter=line_counter_start;

	cout << "Input of control parameters per file is finsihed" << endl;
	Sys_Common_Output::output_fpl->output_txt(&cout);
	//chekc them
	this->check_control_parameters();

	cout << "Input result members per file..." << endl;
	Sys_Common_Output::output_fpl->output_txt(&cout);

	try{
		if(this->landside_flag==true){
			//result
			this->stability_macro.set_input(ifile, line_counter,fpl_label::stab_slope_land_macro,true, fpl_label::end_mech_slope);
			this->stability_macro.check_positiv_zero_variable();
			ifile->seek(pos_file);
			*line_counter=line_counter_start;
			this->stability_micro.set_input(ifile, line_counter,fpl_label::stab_slope_land_micro,true, fpl_label::end_mech_slope);
			this->stability_micro.check_positiv_zero_variable();
			ifile->seek(pos_file);
			*line_counter=line_counter_start;
		}
		else{
			//result
			this->stability_macro.set_input(ifile, line_counter,fpl_label::stab_slope_water_macro,true, fpl_label::end_mech_slope);
			this->stability_macro.check_positiv_zero_variable();
			ifile->seek(pos_file);
			*line_counter=line_counter_start;
			this->stability_micro.set_input(ifile, line_counter,fpl_label::stab_slope_water_micro,true, fpl_label::end_mech_slope);
			this->stability_micro.check_positiv_zero_variable();
			ifile->seek(pos_file);
			*line_counter=line_counter_start;
		}
	}
	catch(Error msg){
		Sys_Common_Output::output_fpl->rewind_userprefix();
		throw msg;
	}

	cout << "Input of result members per file is finished" << endl;
	Sys_Common_Output::output_fpl->output_txt(&cout);

	Sys_Common_Output::output_fpl->rewind_userprefix();
}
//Transfer the slope stability-mechanism data to database, e.g. control parameters, random variables etc.
void Fpl_Mech_Slope_Stability_Dike::transfer_data2database(const int section_id, const _sys_system_id id, QSqlDatabase *ptr_database){
	ostringstream prefix;
	if(this->landside_flag==true){
		prefix << "SLOPE_LAND> ";
	}
	else{
		prefix << "SLOPE_WATER> ";
	}
	Sys_Common_Output::output_fpl->set_userprefix(prefix.str());
	ostringstream cout;
	//control parameters
	cout << "Transfer control parameters to database..."<<endl;
	Sys_Common_Output::output_fpl->output_txt(&cout);
	this->transfer_controlparam2database(section_id, ptr_database);

	////variables of the material zones
	//cout << "Transfer variable members of "<< this->number_material_variable_zones<<" material zones to database..."<<endl;
	//Sys_Common_Output::output_fpl->output_txt(&cout);

	//for(int i=0; i< this->number_material_variable_zones; i++){
	//	this->material_variable_zone[i].transfer_data2database(section_id, id, ptr_database);

	//}

	//result members
	cout << "Transfer result members to database..."<<endl;
	Sys_Common_Output::output_fpl->output_txt(&cout);
	if(this->landside_flag==true){
		this->stability_macro.transfer_rand2database(ptr_database,id, section_id, fpl_label::mech_slope, fpl_label::sec_dike);
		this->stability_micro.transfer_rand2database(ptr_database,id, section_id, fpl_label::mech_slope, fpl_label::sec_dike);
	}
	else{
		this->stability_macro.transfer_rand2database(ptr_database,id, section_id, fpl_label::mech_slope, fpl_label::sec_dike);
		this->stability_micro.transfer_rand2database(ptr_database,id, section_id, fpl_label::mech_slope, fpl_label::sec_dike);
	}

	Sys_Common_Output::output_fpl->rewind_userprefix();
}
//Initiate the slope stability calculation
void Fpl_Mech_Slope_Stability_Dike::init_calculation(Geo_Simple_Polygon *outer_polygon, Geo_Polysegment *outer_polysegment, Geo_Point *base_land, Geo_Point *crest_mid, const int number_mat_zones, Fpl_Dike_Geo_Materialzone *mat_zones,  const double area_cubature){
	this->number_geo_material_zones=number_mat_zones;
	this->ptr_geo_material_zones=mat_zones;
	//determine the maximum radius
	double x_coor;
	double y_coor;

	if(this->landside_flag==true){
		//to the waterside toe
		x_coor=this->x_origin+(double)(this->number_x-1)*this->delta_x;
		y_coor=this->y_origin+(double)(this->number_y-1)*this->delta_y;
		this->maximum_radius=pow((pow(x_coor,2)+pow(y_coor,2)),0.5);
	}
	else{
		//to the landside toe
		x_coor=this->x_origin-base_land->get_xcoordinate();
		y_coor=this->y_origin-base_land->get_ycoordinate();;
		this->maximum_radius=pow((pow(x_coor,2)+pow(y_coor,2)),0.5);
	}
	//determine number of radius per point to check
	this->number_radius=(int)(this->maximum_radius/this->delta_radius);

	if(this->number_radius<1){
		Warning msg=this->set_warning(0);
		msg.output_msg(1);
		//reaction
		this->number_radius=1;
	}

	//this->number_x=1;
	//this->number_y=1;
	//this->number_radius=1;

	//determine number of slip circles
	this->number_slip_circle=this->number_radius*this->number_x*this->number_y;

	//allocate the slip circle
	this->allocate_slip_circle();

	double rad=this->delta_radius;

	x_coor=this->x_origin;
	y_coor=this->y_origin;
	int counter=0;

	ostringstream cout;
	cout <<"Set "<<this->number_slip_circle <<" slip circle data..."  << endl ;
	Sys_Common_Output::output_fpl->output_txt(&cout,false);
	//set the slip circle data
	for(int i=0; i<this->number_x; i++){
		if(i==0){
			x_coor=this->x_origin;
		}
		else{
			x_coor=x_coor+this->delta_x;
		}

		for(int j=0; j<this->number_y; j++){
			if(j==0){
				y_coor=this->y_origin;
			}
			else{
				y_coor=y_coor+this->delta_y;
			}
			rad=0.0;
			for(int k=0; k<this->number_radius; k++){
				Fpl_Calculation::check_stop_thread_flag();
				rad=this->maximum_radius-this->delta_radius*(double)k;
				//rad=30.761516553614705;
				//x_coor=2.08;
				//y_coor=23.0000;
				//
				this->slip_circle[counter].set_midpoint(x_coor,y_coor);
				this->slip_circle[counter].set_radius(rad);
				counter++;
			}
		}
	}

	//check if the mid points are in the section polygon=> no relevant slip circle
	for(int i=0; i< this->number_slip_circle; i++){
		if(outer_polygon->check_point_outside(this->slip_circle[i].get_mid_point())==false){
			this->number_circle_mid_in_polygon++;
			this->slip_circle[i].set_relevant_circle(false);
		}
		if(this->landside_flag==true){
			if(crest_mid->check_left_of(this->slip_circle[i].get_mid_point())==false){
				this->number_beyond_crest_mid++;
				this->slip_circle[i].set_relevant_circle(false);
			}
		}
		if(this->landside_flag==false){
			if(crest_mid->check_right_of(this->slip_circle[i].get_mid_point())==false){
				this->number_beyond_crest_mid++;
				this->slip_circle[i].set_relevant_circle(false);
			}
		}
	}

	this->number_relevant_slip_circle=0;
	//check if the slip circle is relevant
	for(int i=0; i< this->number_slip_circle; i++){
		this->slip_circle[i].check_interception2structure(outer_polysegment,crest_mid, &this->number_circle_no, &this->number_circle_one, &this->number_circle_more_two, &this->number_circle_y_small, outer_polygon, &this->number_circle_outer_bound);
		//count relevant circle
		if(this->slip_circle[i].get_relevant_circle()==true){
			this->number_relevant_slip_circle++;
		}
	}

	cout <<"Initialize the slices of "<<this->number_relevant_slip_circle <<" relevant slip circle(s)..."  << endl ;
	Sys_Common_Output::output_fpl->output_txt(&cout,false);
	//Init the slices of the slip circle
	double y_max=outer_polysegment->get_max_y();

	//initialize a buffer for the zone connector (do it here for the reason of performance)
	Fpl_Dike_Geo_Materialzone_Connector *buff_zone_connector;
	buff_zone_connector=NULL;
	try{
		buff_zone_connector=new Fpl_Dike_Geo_Materialzone_Connector[number_mat_zones];
	}
	catch(bad_alloc &t){
		Error msg=this->set_error(2);
		ostringstream info;
		info<< "Info bad alloc: " << t.what() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}

	try{
		counter=0;
		cout << "Start initializing..."<< endl;
		Sys_Common_Output::output_fpl->output_txt(&cout);
		bool output=false;
		for(int i=0; i< this->number_slip_circle; i++){
			if(counter%25==0 && output==true){
				cout << counter <<" ("<<this->number_relevant_slip_circle<<") relevant slip circles are initialized..."<< endl;
				Sys_Common_Output::output_fpl->output_txt(&cout);
				output=false;
			}
			Fpl_Calculation::check_stop_thread_flag();
			double y_min=0.0;
			if(base_land->get_ycoordinate()<0.0){
				y_min=base_land->get_ycoordinate();
			}
			//if(counter>227){
			//	cout << counter << " " << this->slip_circle[i].get_mid_point()->get_xcoordinate()<< " " << this->slip_circle[i].get_mid_point()->get_ycoordinate()<< " " << this->slip_circle[i].get_radius() << endl;
			//	Sys_Common_Output::output_fpl->output_txt(&cout);
			//}
			if(this->slip_circle[i].init_slices(outer_polygon, y_max, number_mat_zones, mat_zones, buff_zone_connector, this->delta_h, &this->number_circle_reduced_slice_width, y_min, &this->number_slice2small )==false){
				//change radius
				double new_radius=this->slip_circle[i].get_radius();
				new_radius=new_radius+0.001;
				this->slip_circle[i].set_radius(new_radius);
				this->number_circle_radius_change++;
				if(this->slip_circle[i].init_slices(outer_polygon, y_max, number_mat_zones, mat_zones, buff_zone_connector, this->delta_h,  &this->number_circle_reduced_slice_width, y_min, &this->number_slice2small )==false){
					this->slip_circle[i].set_relevant_circle(false);
					this->number_circle_geo_error++;
					this->number_circle_radius_change--;
					Warning msg=this->set_warning(9);
					ostringstream info;
					info << " Landside : " << functions::convert_boolean2string(this->landside_flag) <<endl;
					msg.make_second_info(info.str());
					msg.output_msg(1);
				}
			};
			if(this->slip_circle[i].get_relevant_circle()==true){
				counter++;
				output=true;
			}
		}
	}
	catch(Error msg){
		if(buff_zone_connector!=NULL){
			delete []buff_zone_connector;
			buff_zone_connector=NULL;
		}
		throw msg;
	}

	//delete the buffer
	if(buff_zone_connector!=NULL){
		delete []buff_zone_connector;
		buff_zone_connector=NULL;
	}

	this->number_relevant_slip_circle=0;
	//recount relevant slip circles
	for(int i=0; i< this->number_slip_circle; i++){
		this->slip_circle[i].check_area_intercept(area_cubature, &this->number_circle_area_small);
		//count relevant circle
		if(this->slip_circle[i].get_relevant_circle()==true){
			this->number_relevant_slip_circle++;
		}
	}

	for(int i=0; i< this->number_slip_circle; i++){
		this->slip_circle[i].set_involved_cubature_segment(outer_polysegment);
	}

	if((double)this->number_relevant_slip_circle/(double)(this->number_slip_circle-this->number_circle_no)<0.25){
		Warning msg=this->set_warning(8);
		ostringstream info;
		info << " Landside : " << functions::convert_boolean2string(this->landside_flag) <<endl;
		msg.make_second_info(info.str());
		msg.output_msg(1);
	}
	if((double)this->number_relevant_slip_circle/(double)(this->number_slip_circle - this->number_circle_no) <0.1){
		Error msg=this->set_error(8);
		ostringstream info;
		info << " Landside : " << functions::convert_boolean2string(this->landside_flag) <<endl;
		msg.make_second_info(info.str());
		throw msg;
	}
}
//Add the relevant points of the seepage line to the given list
void Fpl_Mech_Slope_Stability_Dike::add_seepage_line_point(Fpl_Seepage_Line_Point_List *seepage_list, Geo_Polysegment *outer_polysegment){
	ostringstream cout;
	cout <<"Set the relevant points of the seepage line..."  << endl ;
	Sys_Common_Output::output_fpl->output_txt(&cout,false);

	for(int i=0; i< this->number_slip_circle; i++){
		this->slip_circle[i].add_seepage_line_points(seepage_list, outer_polysegment);
	}
}
//Output members of the mechanism to display/console
void Fpl_Mech_Slope_Stability_Dike::output_members(void){
	ostringstream prefix;
	if(this->landside_flag==true){
		prefix << "SLOPE_LAND> ";
	}
	else{
		prefix << "SLOPE_WATER> ";
	}
	Sys_Common_Output::output_fpl->set_userprefix(prefix.str());
	ostringstream cout;

	if(this->landside_flag==true){
		cout << "Control parameters for the mechanism landside slope after Krey..." << endl;
	}
	else{
		cout << "Control parameters for the mechanism waterside slope after Krey..." << endl;
	}
	Sys_Common_Output::output_fpl->output_txt(&cout);
	cout << "SEARCH RASTER"<<endl;
	cout << " Origin x-ccordinate to dike toe   : " << this->x_origin<< label::m<< endl;
	cout << " Origin y-ccordinate to dike toe   : " << this->y_origin << label::m<< endl;
	cout << " Number x-direction                : " << this->number_x << label::no_unit<<  endl;
	cout << " Number y-direction                : " << this->number_y << label::no_unit <<  endl;
	cout << " Delta x                           : " << this->delta_x << label::m << endl;
	cout << " Delta y                           : " << this->delta_y << label::m << endl;
	cout << "SLIP CIRCLE"<<endl;
	cout << " Delta radius                      : " << this->delta_radius << label::m << endl;
	cout << " Maximum radius                    : " << this->maximum_radius << label::m << endl;
	cout << " Number slip circle                : " << this->number_slip_circle << label::no_unit << endl;
	if(this->number_circle_no>0){
		cout << "  Number no interception       : -" <<this->number_circle_no<<label::no_unit<<endl;
	}
	if (this->number_circle_one > 0) {
		cout << "  Number one interception       : -" << this->number_circle_one << label::no_unit << endl;
	}
	if(this->number_circle_outer_bound>0){
		cout << "  Number outer bound interception  : -" <<this->number_circle_outer_bound<<label::no_unit<<endl;
	}
	if(this->number_circle_area_small>0){
		cout << "  Number area to small             : -" <<this->number_circle_area_small<<label::no_unit<<endl;
	}
	if(this->number_circle_y_small>0){
		cout << "  Number first y to small          : -" <<this->number_circle_y_small<<label::no_unit<<endl;
	}
	if(this->number_circle_geo_error>0){
		cout << "  Number slip geometrical error    : -" <<this->number_circle_geo_error<<label::no_unit<<endl;
	}
	if(this->number_circle_mid_in_polygon>0){
		cout << "  Number circle midpoint inside    : -" <<this->number_circle_mid_in_polygon<<label::no_unit<<endl;
	}
	if(this->number_beyond_crest_mid>0){
		cout << "  Number midpoint beyond crest mid : -" <<this->number_beyond_crest_mid<<label::no_unit<<endl;
	}
	if(this->number_slice2small>0){
		cout << "  Number slice width is to small   : -" <<this->number_slice2small<<label::no_unit<<endl;
	}
	cout << " Number relevant slip circle       : " << this->number_relevant_slip_circle << label::no_unit << endl;
	if(this->number_circle_reduced_slice_width>0){
		cout << "  Number reduced slice width       : " << this->number_circle_reduced_slice_width << label::no_unit << endl;
	}
	if(this->number_circle_more_two>0){
		cout << "  Number more than two interception: " << this->number_circle_more_two << label::no_unit << endl;
	}
	if(this->number_circle_radius_change>0){
		cout << "  Number radius is changed by 1 mm : " << this->number_circle_radius_change << label::no_unit << endl;
	}

	Sys_Common_Output::output_fpl->output_txt(&cout);
	cout << "Output just the relevant slip circle(s):"<< endl;
	Sys_Common_Output::output_fpl->output_txt(&cout,true );
	//output the slip circle
	for(int i=0; i< this->number_slip_circle; i++){
		this->slip_circle[i].output_members(i);
	}

	if(this->landside_flag==true){
		cout << "Variable members mechanism landside slope after Krey..." << endl;
	}
	else{
		cout << "Variable members mechanism waterside slope after Krey..." << endl;
	}
	Sys_Common_Output::output_fpl->output_txt(&cout,true);
	cout<<" Material zones ("<<this->number_geo_material_zones<<")"<<endl;
	for(int i=0; i< this->number_geo_material_zones; i++){
		cout <<" "<<this->ptr_geo_material_zones[i].get_variable_zone_number()<<"  "<<this->ptr_geo_material_zones[i].get_zone_name()<<endl;
		this->ptr_geo_material_zones[i].get_ptr_relevant_variables()->grain_density.output_member();
		this->ptr_geo_material_zones[i].get_ptr_relevant_variables()->porosity.output_member();
		this->ptr_geo_material_zones[i].get_ptr_relevant_variables()->water_content.output_member();
		this->ptr_geo_material_zones[i].get_ptr_relevant_variables()->angle_friction.output_member();
		this->ptr_geo_material_zones[i].get_ptr_relevant_variables()->cohesion.output_member();
	}
	if(this->landside_flag==true){
		cout << "Result members mechanism landside slope after Krey..." << endl;
	}
	else{
		cout << "Result members mechanism waterside slope after Krey..." << endl;
	}
	Sys_Common_Output::output_fpl->output_txt(&cout,true);
	this->stability_macro.output_member();
	this->stability_micro.output_member();

	Sys_Common_Output::output_fpl->rewind_userprefix();
}
//Output the header to tecplot (static)
void Fpl_Mech_Slope_Stability_Dike::output_header2tecplot(const string slope_file){
	//open the file
	ofstream output;
	output.open(slope_file.c_str());
	if(output.is_open()==false){
		Error msg;
		msg.set_msg("Fpl_Mech_Slope_Stability_Dike::output_header2tecplot(const string slope_file)","Could not open the file for the tecplot output (slope stability results) of the FPL-section", "Check the file", 5, false);
		ostringstream info;
		info << "Filename : " << slope_file << endl;
		msg.make_second_info(info.str());
		throw msg;
	}

	//output the file header
	output << "TITLE = "<< "\" Slope stability  \"" << endl;
	output << "VARIABLES = " <<endl;
	output << "\" x " << label::m << "\" "<<endl;
	output << "\" z " << label::m << "\" "<< endl;
	output <<endl<<endl<<endl;

	output.close();
}
//Output the search raster and the critical slipe circle to tecplot
void Fpl_Mech_Slope_Stability_Dike::output_results2tecplot(const string slope_file, int *zone_counter){
	//open the file
	ofstream output;
	output.open(slope_file.c_str(), ios_base::app);
	if(output.is_open()==false){
		Error msg=this->set_error(7);
		ostringstream info;
		info << "Filename : " << slope_file << endl;
		msg.make_second_info(info.str());
		throw msg;
	}

	output << P(4) << FORMAT_FIXED_REAL;

	//search raster
	output << endl << "ZONE T= "<< "\" Searchraster points ";
	if(this->landside_flag==true){
		output << "landside ";
	}
	else{
		output << "waterside ";
	}
	output << "\" "<< " I = " << this->number_x*this->number_y << endl;
	(*zone_counter)++;

	//output points
	double x_coor=0.0;
	double y_coor=0.0;
	x_coor=this->x_origin;
	y_coor=this->y_origin;

	for(int i=0; i< this->number_x; i++){
		if(i==0){
			x_coor=this->x_origin;
		}
		else{
			x_coor=x_coor+this->delta_x;
		}

		for(int j=0; j<this->number_y; j++){
			if(j==0){
				y_coor=this->y_origin;
			}
			else{
				y_coor=y_coor+this->delta_y;
			}
			output << W(12) << x_coor << W(12) << y_coor <<  endl;
		}
		output.flush();
	}

	//critical macro circle
	if(this->relevant_macro_circle!=NULL){
		output << endl << "ZONE T= "<< "\" Critical circle macro stability ";
		(*zone_counter)++;
		if(this->landside_flag==true){
			output << "landside ";
		}
		else{
			output << "waterside ";
		}
		output << "(stability factor= "<<this->stability_macro.get_calculated_mean()<<")\" "<< " I = " << this->relevant_macro_circle->get_number_slices()+4 << endl;
		output<<" " <<W(12)<< this->relevant_macro_circle->get_mid_point()->get_xcoordinate()<<" "<<W(12)<<this->relevant_macro_circle->get_mid_point()->get_ycoordinate()<<endl;
		output.flush();
		this->relevant_macro_circle->output_slices2tecplot(&output,this->landside_flag);
		output<<" " <<W(12)<< this->relevant_macro_circle->get_mid_point()->get_xcoordinate()<<" "<<W(12)<<this->relevant_macro_circle->get_mid_point()->get_ycoordinate()<<endl<<endl;
	}
		//critical macro circle
	if(this->relevant_micro_circle!=NULL){
		output << endl << "ZONE T= "<< "\" Critical circle micro stability ";
		(*zone_counter)++;
		if(this->landside_flag==true){
			output << "landside ";
		}
		else{
			output << "waterside ";
		}
		output << "(stability factor= "<<this->stability_micro.get_calculated_mean()<<")\" "<< " I = " << this->relevant_micro_circle->get_number_slices()+4 << endl;
		output<<" " <<W(12)<< this->relevant_micro_circle->get_mid_point()->get_xcoordinate()<<" "<<W(12)<<this->relevant_micro_circle->get_mid_point()->get_ycoordinate()<<endl;
		output.flush();
		this->relevant_micro_circle->output_slices2tecplot(&output,this->landside_flag);
		output<<" " <<W(12)<< this->relevant_micro_circle->get_mid_point()->get_xcoordinate()<<" "<<W(12)<<this->relevant_micro_circle->get_mid_point()->get_ycoordinate()<<endl<<endl;
	}

	//close the file
	output.close();
}
//Output the search raster and the critical slipe circle to paraview
void Fpl_Mech_Slope_Stability_Dike::output_results2paraview(const string slope_file, int *zone_counter) {

	//open the file
	ofstream output;
	output.open(slope_file.c_str());
	if (output.is_open() == false) {
		Error msg = this->set_error(9);
		ostringstream info;
		info << "Filename : " << slope_file << endl;
		msg.make_second_info(info.str());
		throw msg;
	}

	output << " x,";
	int count_col_tot = 1;

	output << "Searchraster_points,";
	count_col_tot++;
	if (this->relevant_macro_circle != NULL) {
		output << "Critical_circle_macro_stability_(stability_factor=" << this->stability_macro.get_calculated_mean() << "),";
		count_col_tot++;
	}
	if (this->relevant_micro_circle != NULL) {
		output << "Critical_circle_micro_stability_(stability_factor=" << this->stability_micro.get_calculated_mean() << ")";
		count_col_tot++;
	}

	output << endl;
	output.flush();

	//points
	int counter_col_before = 0;
	int counter_col_after = count_col_tot - 1;

	counter_col_after--;
	//output points
	double x_coor = 0.0;
	double y_coor = 0.0;
	x_coor = this->x_origin;
	y_coor = this->y_origin;

	for (int i = 0; i < this->number_x; i++) {
		if (i == 0) {
			x_coor = this->x_origin;
		}
		else {
			x_coor = x_coor + this->delta_x;
		}

		for (int j = 0; j < this->number_y; j++) {
			if (j == 0) {
				y_coor = this->y_origin;
			}
			else {
				y_coor = y_coor + this->delta_y;
			}

			output << x_coor << ",";
			functions::add_seperator_csv("NAN,", &output, counter_col_before);
			output<< y_coor;
			functions::add_seperator_csv(",NAN", &output, counter_col_after);
			output << endl;
		}
		output.flush();
	}

	counter_col_before++;

	//circle macro
	counter_col_after--;
	if (this->relevant_macro_circle != NULL) {

		output << this->relevant_macro_circle->get_mid_point()->get_xcoordinate() << ",";
		functions::add_seperator_csv("NAN,", &output, counter_col_before);
		output << this->relevant_macro_circle->get_mid_point()->get_ycoordinate();
		functions::add_seperator_csv(",NAN", &output, counter_col_after);
		output << endl;
		
		this->relevant_macro_circle->output_slices2paraview(&output, this->landside_flag, counter_col_before, counter_col_after);


		output << this->relevant_macro_circle->get_mid_point()->get_xcoordinate() << ",";
		functions::add_seperator_csv("NAN,", &output, counter_col_before);
		output << this->relevant_macro_circle->get_mid_point()->get_ycoordinate();
		functions::add_seperator_csv(",NAN", &output, counter_col_after);
		output << endl;
		output.flush();

	}
	counter_col_before++;


	//circle micro
	counter_col_after--;
	if (this->relevant_micro_circle != NULL) {

		output << this->relevant_micro_circle->get_mid_point()->get_xcoordinate() << ",";
		functions::add_seperator_csv("NAN,", &output, counter_col_before);
		output << this->relevant_micro_circle->get_mid_point()->get_ycoordinate();
		functions::add_seperator_csv(",NAN", &output, counter_col_after);
		output << endl;

		this->relevant_micro_circle->output_slices2paraview(&output, this->landside_flag, counter_col_before, counter_col_after);


		output << this->relevant_micro_circle->get_mid_point()->get_xcoordinate() << ",";
		functions::add_seperator_csv("NAN,", &output, counter_col_before);
		output << this->relevant_micro_circle->get_mid_point()->get_ycoordinate();
		functions::add_seperator_csv(",NAN", &output, counter_col_after);
		output << endl;
		output.flush();

	}
	counter_col_before++;








	//close the file
	output.close();


}
//Output the search raster and the critical slipe circle to excel
void Fpl_Mech_Slope_Stability_Dike::output_results2excel(const string slope_file, int *zone_counter) {
	//open the file
	ofstream output;
	output.open(slope_file.c_str());
	if (output.is_open() == false) {
		Error msg = this->set_error(9);
		ostringstream info;
		info << "Filename : " << slope_file << endl;
		msg.make_second_info(info.str());
		throw msg;
	}

	output << " x;";
	int count_col_tot = 1;

	output << "Searchraster_points;";
	count_col_tot++;
	if (this->relevant_macro_circle != NULL) {
		output << "Critical_circle_macro_stability_(stability_factor=" << this->stability_macro.get_calculated_mean() << ");";
		count_col_tot++;
	}
	if (this->relevant_micro_circle != NULL) {
		output << "Critical_circle_micro_stability_(stability_factor=" << this->stability_micro.get_calculated_mean() << ")";
		count_col_tot++;
	}

	output << endl;
	output.flush();

	//points
	int counter_col_before = 0;
	int counter_col_after = count_col_tot - 1;

	counter_col_after--;
	//output points
	double x_coor = 0.0;
	double y_coor = 0.0;
	x_coor = this->x_origin;
	y_coor = this->y_origin;

	for (int i = 0; i < this->number_x; i++) {
		if (i == 0) {
			x_coor = this->x_origin;
		}
		else {
			x_coor = x_coor + this->delta_x;
		}

		for (int j = 0; j < this->number_y; j++) {
			if (j == 0) {
				y_coor = this->y_origin;
			}
			else {
				y_coor = y_coor + this->delta_y;
			}

			output << x_coor << ";";
			functions::add_seperator_csv(";", &output, counter_col_before);
			output << y_coor;
			functions::add_seperator_csv(";", &output, counter_col_after);
			output << endl;
		}
		output.flush();
	}

	counter_col_before++;

	//circle macro
	counter_col_after--;
	if (this->relevant_macro_circle != NULL) {

		output << this->relevant_macro_circle->get_mid_point()->get_xcoordinate() << ";";
		functions::add_seperator_csv(";", &output, counter_col_before);
		output << this->relevant_macro_circle->get_mid_point()->get_ycoordinate();
		functions::add_seperator_csv(";", &output, counter_col_after);
		output << endl;

		this->relevant_macro_circle->output_slices2excel(&output, this->landside_flag, counter_col_before, counter_col_after);


		output << this->relevant_macro_circle->get_mid_point()->get_xcoordinate() << ";";
		functions::add_seperator_csv(";", &output, counter_col_before);
		output << this->relevant_macro_circle->get_mid_point()->get_ycoordinate();
		functions::add_seperator_csv(";", &output, counter_col_after);
		output << endl;
		output.flush();

	}
	counter_col_before++;


	//circle micro
	counter_col_after--;
	if (this->relevant_micro_circle != NULL) {

		output << this->relevant_micro_circle->get_mid_point()->get_xcoordinate() << ";";
		functions::add_seperator_csv(";", &output, counter_col_before);
		output << this->relevant_micro_circle->get_mid_point()->get_ycoordinate();
		functions::add_seperator_csv(";", &output, counter_col_after);
		output << endl;

		this->relevant_micro_circle->output_slices2excel(&output, this->landside_flag, counter_col_before, counter_col_after);


		output << this->relevant_micro_circle->get_mid_point()->get_xcoordinate() << ";";
		functions::add_seperator_csv(";", &output, counter_col_before);
		output << this->relevant_micro_circle->get_mid_point()->get_ycoordinate();
		functions::add_seperator_csv(";", &output, counter_col_after);
		output << endl;
		output.flush();

	}
	counter_col_before++;








	//close the file
	output.close();

}
//Check the statistic of the random variables of the mechanism
void Fpl_Mech_Slope_Stability_Dike::check_statistic(void){
	this->stability_macro.check_statistic();
	this->stability_micro.check_statistic();
}
//Reset the statistic important values
void Fpl_Mech_Slope_Stability_Dike::reset_statistics(void){
	this->stability_macro.reset_statistics();
	this->stability_micro.reset_statistics();
	this->reset_counter_prob();
	this->reset_counter_prob_micro();
}
//Output the statistics of the random variables of the mechanism to display/console
void Fpl_Mech_Slope_Stability_Dike::output_statistic(void){
	ostringstream prefix;
	if(this->landside_flag==true){
		prefix << "SLOPE_LAND> ";
	}
	else{
		prefix << "SLOPE_WATER> ";
	}
	Sys_Common_Output::output_fpl->set_userprefix(prefix.str());

	ostringstream cout;
	cout << "Materialzone information" << endl;
	Sys_Common_Output::output_fpl->output_txt(&cout,true);
	cout<< " No. of geometrical materialzones : "<<this->number_geo_material_zones<<endl;

	cout << "Random result members of the mechanism slope landside..." << endl;
	Sys_Common_Output::output_fpl->output_txt(&cout,true);
	cout << " Macro instability"<<endl;
	Sys_Common_Output::output_fpl->output_txt(&cout,true);
	if(this->relevant_macro_circle!=NULL){
		this->stability_macro.output_statistic();
		cout << "  Relevant slip circle"<<endl;
		cout << "   Midpoint x-coordinate            : " << this->relevant_macro_circle->get_mid_point()->get_xcoordinate()<<label::m<< endl;
		cout << "   Midpoint y-coordinate            : " << this->relevant_macro_circle->get_mid_point()->get_ycoordinate()<<label::m<< endl;
		cout << "   Radius                           : " << this->relevant_macro_circle->get_radius()<<label::m<< endl;
	}
	else{
		cout << "  No relevant slip circle is found!"<<endl;
	}
	cout << " Micro instability"<<endl;
	Sys_Common_Output::output_fpl->output_txt(&cout,true);
	if(this->relevant_micro_circle!=NULL){
		this->stability_micro.output_statistic();
		cout << "  Relevant slip circle"<<endl;
		cout << "   Midpoint x-coordinate            : " << this->relevant_micro_circle->get_mid_point()->get_xcoordinate()<<label::m<< endl;
		cout << "   Midpoint y-coordinate            : " << this->relevant_micro_circle->get_mid_point()->get_ycoordinate()<<label::m<< endl;
		cout << "   Radius                           : " << this->relevant_micro_circle->get_radius()<<label::m<< endl;
	}
	else{
		cout << "  No relevant slip circle is found!"<<endl;
	}
	Sys_Common_Output::output_fpl->output_txt(&cout,true);
	Sys_Common_Output::output_fpl->rewind_userprefix();
}
//Output the results of a deterministic calculation to display/console
void Fpl_Mech_Slope_Stability_Dike::output_determ_results(void){
	ostringstream prefix;
	if(this->landside_flag==true){
		prefix << "SLOPE_LAND> ";
	}
	else{
		prefix << "SLOPE_WATER> ";
	}
	Sys_Common_Output::output_fpl->set_userprefix(prefix.str());
	ostringstream cout;
	cout << "Deterministic results of the mechanism slope after Krey..." << endl;
	cout << " Macro instability"<<endl;
	if(this->relevant_macro_circle!=NULL){
		cout << "  Factor of safety                  : " << this->stability_macro.get_result().end_result<<label::no_unit<<endl;
		cout << "  Relevant slip circle"<<endl;
		cout << "   Midpoint x-coordinate            : " << this->relevant_macro_circle->get_mid_point()->get_xcoordinate()<<label::m<< endl;
		cout << "   Midpoint y-coordinate            : " << this->relevant_macro_circle->get_mid_point()->get_ycoordinate()<<label::m<< endl;
		cout << "   Radius                           : " << this->relevant_macro_circle->get_radius()<<label::m<< endl;
	}
	else{
		cout << "  No relevant slip circle is found!"<<endl;
	}
	cout << " Micro instability"<<endl;
	if(this->relevant_micro_circle!=NULL){
		cout << "  Factor of safety                  : " << this->stability_micro.get_result().end_result<<label::no_unit<<endl;
		cout << "  Relevant slip circle"<<endl;
		cout << "   Midpoint x-coordinate            : " << this->relevant_micro_circle->get_mid_point()->get_xcoordinate()<<label::m<< endl;
		cout << "   Midpoint y-coordinate            : " << this->relevant_micro_circle->get_mid_point()->get_ycoordinate()<<label::m<< endl;
		cout << "   Radius                           : " << this->relevant_micro_circle->get_radius()<<label::m<< endl;
	}
	else{
		cout << "  No relevant slip circle is found!"<<endl;
	}
	Sys_Common_Output::output_fpl->output_txt(&cout,false);
	Sys_Common_Output::output_fpl->rewind_userprefix();
}
//Calculate the mechanism; main input parameter is the waterlevel before the dike section
void Fpl_Mech_Slope_Stability_Dike::calculate_mechanism(const bool random_calculation, Fpl_Seepage_Line_Point_List *seepage_list, const double water_density, bool *macro, bool *micro){
	this->reset_random_flag();
	this->relevant_macro_circle=NULL;
	this->relevant_micro_circle=NULL;

	double res_macro=0.0;
	double res_micro=0.0;
	double min_macro=100000.0;
	double min_micro=100000.0;
	bool min_macro_set=false;
	bool min_micro_set=false;

	//make the slip circles
	for(int i=0; i< this->number_slip_circle; i++){
		this->slip_circle[i].calculate_stability(seepage_list, &res_macro, &res_micro, water_density);

		//analysis of the results
		if(res_macro>=0.0){
			if(res_macro<min_macro){
				min_macro_set=true;
				min_macro=res_macro;
				this->relevant_macro_circle=&this->slip_circle[i];
			}
		}
		if(res_micro>=0.0){
			//search for the min value
			if(res_micro<min_micro){
				min_micro_set=true;
				min_micro=res_micro;
				this->relevant_micro_circle=&this->slip_circle[i];
			}
		}
	}
	Fpl_Calculation::check_stop_thread_flag();
	if(min_macro_set==true){
		this->stability_macro.set_new_middle(min_macro);
		this->stability_macro.calculate_variables(random_calculation);
		min_macro=this->stability_macro.get_result().end_result;
		if(min_macro<1.0){
			*macro=true;
		}
	}

	if(min_micro_set==true){
		this->stability_micro.set_new_middle(min_micro);
		this->stability_micro.calculate_variables(random_calculation);
		min_micro=this->stability_micro.get_result().end_result;
		if(min_micro<1.0){
			*micro=true;
		}
	}
}
//Set if it is a landside or a waterside slope mechanism
void Fpl_Mech_Slope_Stability_Dike::set_waterside_slope_mechanism(const bool flag){
	this->landside_flag=!flag;
}
//Sum up the counter of the single occurence probability by 1 of the micro instability
void Fpl_Mech_Slope_Stability_Dike::sum_up_counter_single_prob_micro(void){
	this->counter_single_prob_micro++;
}
//Sum up the counter of the fault-tree occurence probability by 1 of the micro instability
void Fpl_Mech_Slope_Stability_Dike::sum_up_counter_tree_prob_micro(void){
	this->counter_tree_prob_micro++;
}
//Get the counter of the single occurence probability of the micro instability
double Fpl_Mech_Slope_Stability_Dike::get_counter_single_prob_micro(void){
	return (double)this->counter_single_prob_micro;
}
//Get the counter of the fault-tree occurence probability of the micro instability
double Fpl_Mech_Slope_Stability_Dike::get_counter_tree_prob_micro(void){
	return (double)this->counter_tree_prob_micro;
}
//Reset the counter of the single occurence probability of the micro instability
void Fpl_Mech_Slope_Stability_Dike::reset_counter_single_prob_micro(void){
	this->counter_single_prob_micro=0;
}
//Reset the counter of the fault-tree occurence probability of the micro instability
void Fpl_Mech_Slope_Stability_Dike::reset_counter_tree_prob_micro(void){
	this->counter_tree_prob_micro=0;
}
//Reset the counter of the fault-tree occurence probability and of the single occurence probability of the micro instability
void Fpl_Mech_Slope_Stability_Dike::reset_counter_prob_micro(void){
	this->counter_single_prob_micro=0;
	this->counter_tree_prob_micro=0;
}
//________
//private
//Reset the random generated flag
void Fpl_Mech_Slope_Stability_Dike::reset_random_flag(void){
	this->stability_macro.reset_random_generated();
	this->stability_micro.reset_random_generated();
}
//Allocate the slip circle
void Fpl_Mech_Slope_Stability_Dike::allocate_slip_circle(void){
	this->delete_slip_circle();
	try{
		this->slip_circle=new Fpl_Slip_Circle[this->number_slip_circle];
	}
	catch(bad_alloc &t){
		Error msg=this->set_error(0);
		ostringstream info;
		info<< "Info bad alloc: " << t.what() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}

	for(int i=0; i< this->number_slip_circle; i++){
		this->slip_circle[i].set_waterside_slip_circle(!this->landside_flag);
		this->slip_circle[i].set_slices_width(this->slice_width);
	}
}
//Delete the slip circle
void Fpl_Mech_Slope_Stability_Dike::delete_slip_circle(void){
	if(this->slip_circle!=NULL){
		delete []this->slip_circle;
		this->slip_circle=NULL;
	}
}
//Transfer the control parameters to database table
void Fpl_Mech_Slope_Stability_Dike::transfer_controlparam2database(const int section_id, QSqlDatabase *ptr_database){
	try{
		Fpl_Mech_Slope_Stability_Dike::set_control_table(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}

	int glob_id=Fpl_Mech_Slope_Stability_Dike::control_table->maximum_int_of_column(Fpl_Mech_Slope_Stability_Dike::control_table->get_column_name(fpl_label::glob_id), ptr_database)+1;

	//transfer the data
	ostringstream query_string;
	query_string << "INSERT INTO  " << Fpl_Mech_Slope_Stability_Dike::control_table->get_table_name();
	query_string <<" ( ";
	query_string << Fpl_Mech_Slope_Stability_Dike::control_table->get_column_name(fpl_label::glob_id) <<" , ";
	query_string << Fpl_Mech_Slope_Stability_Dike::control_table->get_column_name(fpl_label::section_id) <<" , ";

	query_string << Fpl_Mech_Slope_Stability_Dike::control_table->get_column_name(fpl_label::landside_flag) <<" , ";
	query_string << Fpl_Mech_Slope_Stability_Dike::control_table->get_column_name(fpl_label::x_origin) <<" , ";
	query_string << Fpl_Mech_Slope_Stability_Dike::control_table->get_column_name(fpl_label::y_origin) <<" , ";

	query_string << Fpl_Mech_Slope_Stability_Dike::control_table->get_column_name(fpl_label::no_x) <<" , ";
	query_string << Fpl_Mech_Slope_Stability_Dike::control_table->get_column_name(fpl_label::no_y) <<" , ";

	query_string << Fpl_Mech_Slope_Stability_Dike::control_table->get_column_name(fpl_label::delta_x) <<" , ";
	query_string << Fpl_Mech_Slope_Stability_Dike::control_table->get_column_name(fpl_label::delta_y) <<" , ";
	query_string << Fpl_Mech_Slope_Stability_Dike::control_table->get_column_name(fpl_label::delta_r) <<" , ";
	query_string << Fpl_Mech_Slope_Stability_Dike::control_table->get_column_name(fpl_label::delta_h) <<" , ";
	query_string << Fpl_Mech_Slope_Stability_Dike::control_table->get_column_name(fpl_label::slice_width);
	query_string <<" ) ";
	query_string << " VALUES ( ";

	query_string << glob_id << " , " ;
	query_string << section_id<< " , " ;

	query_string << functions::convert_boolean2string(this->landside_flag)<< " , " ;
	query_string << this->x_origin<< " , " ;
	query_string << this->y_origin << " , " ;

	query_string << this->number_x<< " , " ;
	query_string << this->number_y << " , " ;

	query_string << this->delta_x << " , ";
	query_string << this->delta_y << " , ";
	query_string << this->delta_radius << " , ";
	query_string << this->delta_h << " , ";
	query_string << this->slice_width << " ) " ;

	//Set the query
	QSqlQuery query_buff(*ptr_database);
	//launch the request
	Data_Base::database_request(&query_buff, query_string.str(), ptr_database);

	if(query_buff.lastError().isValid()){
		Warning msg=this->set_warning(7);
		ostringstream info;
		info << "Table Name                : " << Fpl_Mech_Slope_Stability_Dike::control_table->get_table_name() << endl;
		info << "Table error info          : " << query_buff.lastError().text().toStdString() << endl;
		info << "Section index             : " << section_id << endl;
		info << "Landside stability        : " << functions::convert_boolean2string(this->landside_flag) << endl;
		msg.make_second_info(info.str());
		msg.output_msg(1);
	}
}
//Set the control parameters per file
void Fpl_Mech_Slope_Stability_Dike::set_controlparam_per_file(QFile *ifile, int *line_counter){
	//read in
	string myline;
	QString qmyline;
	stringstream my_stream;
	bool found_flag=false;
	int must_found_counter=0;

	//search for the keywords
	do{
		qmyline=ifile->readLine();
		myline=qmyline.toStdString();
		(*line_counter)++;
		functions::clean_string(&myline);
		if((this->landside_flag==true && myline==fpl_label::end_slope_control_land) || (this->landside_flag==false && myline==fpl_label::end_slope_control_water)){
			found_flag=true;
			break;
		}
		else if(myline.empty()==false){
			this->find_key_values_slope_control(myline, &must_found_counter);
		}
	}
	while(ifile->atEnd()==false);

	if(found_flag==false){
		Error msg=this->set_error(4);
		throw msg;
	}

	if(must_found_counter!=2){
		Error msg=this->set_error(5);
		throw msg;
	}
}
//Find the key-values for the file input of the slope control parameters
void Fpl_Mech_Slope_Stability_Dike::find_key_values_slope_control(const string myline, int *must_found_counter){
	int pos=-1;
	string buffer;
	stringstream buffer1;
	bool wrong_input=false;

	pos=myline.find(fpl_label::key_x_origin);
	if(pos>=0 && wrong_input==false){
		buffer=myline.substr(fpl_label::key_x_origin.length());
		functions::clean_string(&buffer);
		buffer1 << buffer;
		buffer1 >> this->x_origin;
		if(buffer1.fail()==true){
			wrong_input=true;
		}
		else{
			(*must_found_counter)++;
			return;
		}
	}

	pos=myline.find(fpl_label::key_y_origin);
	if(pos>=0 && wrong_input==false){
		buffer=myline.substr(fpl_label::key_y_origin.length());
		functions::clean_string(&buffer);
		buffer1 << buffer;
		buffer1 >> this->y_origin;
		if(buffer1.fail()==true){
			wrong_input=true;
		}
		else{
			(*must_found_counter)++;
			return;
		}
	}

	pos=myline.find(fpl_label::key_delta_x);
	if(pos>=0 && wrong_input==false){
		buffer=myline.substr(fpl_label::key_delta_x.length());
		functions::clean_string(&buffer);
		buffer1 << buffer;
		buffer1 >> this->delta_x;
		if(buffer1.fail()==true){
			wrong_input=true;
		}
		else{
			return;
		}
	}

	pos=myline.find(fpl_label::key_delta_y);
	if(pos>=0 && wrong_input==false){
		buffer=myline.substr(fpl_label::key_delta_y.length());
		functions::clean_string(&buffer);
		buffer1 << buffer;
		buffer1 >> this->delta_y;
		if(buffer1.fail()==true){
			wrong_input=true;
		}
		else{
			return;
		}
	}

	pos=myline.find(fpl_label::key_delta_r);
	if(pos>=0 && wrong_input==false){
		buffer=myline.substr(fpl_label::key_delta_r.length());
		functions::clean_string(&buffer);
		buffer1 << buffer;
		buffer1 >> this->delta_radius;
		if(buffer1.fail()==true){
			wrong_input=true;
		}
		else{
			return;
		}
	}

	pos=myline.find(fpl_label::key_delta_h);
	if(pos>=0 && wrong_input==false){
		buffer=myline.substr(fpl_label::key_delta_h.length());
		functions::clean_string(&buffer);
		buffer1 << buffer;
		buffer1 >> this->delta_h;
		if(buffer1.fail()==true){
			wrong_input=true;
		}
		else{
			return;
		}
	}

	pos=myline.find(fpl_label::key_no_x);
	if(pos>=0 && wrong_input==false){
		buffer=myline.substr(fpl_label::key_no_x.length());
		functions::clean_string(&buffer);
		buffer1 << buffer;
		buffer1 >> this->number_x;
		if(buffer1.fail()==true){
			wrong_input=true;
		}
		else{
			return;
		}
	}

	pos=myline.find(fpl_label::key_no_y);
	if(pos>=0 && wrong_input==false){
		buffer=myline.substr(fpl_label::key_no_y.length());
		functions::clean_string(&buffer);
		buffer1 << buffer;
		buffer1 >> this->number_y;
		if(buffer1.fail()==true){
			wrong_input=true;
		}
		else{
			return;
		}
	}

	pos=myline.find(fpl_label::key_slice_width);
	if(pos>=0 && wrong_input==false){
		buffer=myline.substr(fpl_label::key_slice_width.length());
		functions::clean_string(&buffer);
		buffer1 << buffer;
		buffer1 >> this->slice_width;
		if(buffer1.fail()==true){
			wrong_input=true;
		}
		else{
			return;
		}
	}

	if(wrong_input==true){
		ostringstream info;
		info << "Wrong input sequenze  : " << buffer1.str() << endl;
		Error msg=this->set_error(6);
		msg.make_second_info(info.str());
		throw msg;
	}
}
//Check the control parameters
void Fpl_Mech_Slope_Stability_Dike::check_control_parameters(void){
	if(this->delta_x<=0.0){
		Warning msg=this->set_warning(1);
		msg.output_msg(1);
		//reaction
		this->delta_x=fpl_slope_control_param::def_delta_x;
	}
	if(this->delta_y<=0.0){
		Warning msg=this->set_warning(2);
		msg.output_msg(1);
		//reaction
		this->delta_y=fpl_slope_control_param::def_delta_y;
	}
	if(this->delta_radius<=0.0){
		Warning msg=this->set_warning(3);
		msg.output_msg(1);
		//reaction
		this->delta_radius=fpl_slope_control_param::def_delta_radius;
	}
	if(this->delta_h<=0.0){
		Warning msg=this->set_warning(4);
		msg.output_msg(1);
		//reaction
		this->delta_h=fpl_slope_control_param::def_delta_h;
	}
	if(this->slice_width<=0){
		Warning msg=this->set_warning(5);
		msg.output_msg(1);
		//reaction
		this->slice_width=fpl_slope_control_param::def_slice_width;
	}
}
//set the error
Error Fpl_Mech_Slope_Stability_Dike::set_error(const int err_type){
		string place="Fpl_Mech_Slope_Stability_Dike::";
		string help;
		string reason;
		int type=0;
		bool fatal=false;
		stringstream info;
		Error msg;

	switch (err_type){
		case 0://bad_alloc
			place.append("allocate_slip_circle(void)");
			reason="Can not allocate the memory";
			help="Check the memory";
			type=10;
			break;
		case 1://bad_alloc
			place.append("compact_structure_cubature2polysegment(Fpl_Cub_Dike_Waterside *waterside, Fpl_Cub_Dike_Crest *crest, Fpl_Cub_Dike_Landside *landside)");
			reason="Can not allocate the memory";
			help="Check the memory";
			type=10;
			break;
		case 2://bad_alloc
			place.append("init_calculation(Fpl_Cub_Dike_Waterside *waterside, Fpl_Cub_Dike_Crest *crest, Fpl_Cub_Dike_Landside *landside, const int number_mat_zones, Fpl_Dike_Geo_Materialzone *mat_zones)");
			reason="Can not allocate the memory";
			help="Check the memory";
			type=10;
			break;
		case 3://do not find the begin of the general section information in file
			place.append("set_controlparam_per_file(QFile *ifile, int *line_counter)");
			reason="Can not find the begin of the slope control parameters in file";
			help="The slope control parameters has to start with !$BEGIN_CONTROL";
			type=1;
			break;
		case 4://do not find the end of the general section information in file
			place.append("set_controlparam_per_file(QFile *ifile, int *line_counter)");
			reason="Can not find the end of the slope control parameters in file";
			help="The slope control parameters has to end with !$END_CONTROL";
			type=1;
			break;
		case 5://do not find all required section infos
			place.append("set_controlparam_per_file(QFile *ifile, int *line_counter))");
			reason="Can not find all slope control parameters";
			help="Check if all slope control parameters are set in the file";
			info << "Required information:"<<endl;
			info << " " << fpl_label::key_x_origin<< endl;
			info << " " << fpl_label::key_y_origin<< endl;
			info << "Optional information:"<<endl;
			info << " " << fpl_label::key_no_x<< endl;
			info << " " << fpl_label::key_no_y<< endl;
			info << " " << fpl_label::key_delta_x<< endl;
			info << " " << fpl_label::key_delta_y<< endl;
			info << " " << fpl_label::key_delta_r<< endl;
			info << " " << fpl_label::key_delta_h<< endl;
			info << " " << fpl_label::key_slice_width<< endl;
			type=1;
			break;
		case 6://wrong input
			place.append("find_key_values_slope_control(const string myline, int *must_found_counter)");
			reason="There is a problem with the input; wrong sign is read in";
			help="Check the slope control parameters in file";
			type=1;
			break;
		case 7://could not open the tecplot file
			place.append("output_results2tecplot(const string slope_file)");
			reason="Could not open the file for the tecplot output (slope stability results) of the FPL-section";
			help="Check the file";
			type=5;
			break;
		case 8://0.90 percent of the slip circle are not relevant
			place.append("init_calculation(Geo_Simple_Polygon *outer_polygon, Geo_Polysegment *outer_polysegment, Geo_Point *base_land, Geo_Point *crest_mid, const int number_mat_zones, Fpl_Dike_Geo_Materialzone *mat_zones,  const double area_cubature)");
			reason="90 % of the theoretical slip circle are practical not relevant";
			help="Check the search raster";
			type=27;
			break;
		case 9://could not open the paraview file
			place.append("output_results2paraview(const string slope_file, int *zone_counter, const string file_name) ");
			reason = "Could not open the file for the paraview output (slope stability results) of the FPL-section";
			help = "Check the file";
			type = 5;
			break;
		case 10://could not open the excel file
			place.append("output_results2excel(const string slope_file, int *zone_counter, const string file_name) ");
			reason = "Could not open the file for the excel output (slope stability results) of the FPL-section";
			help = "Check the file";
			type = 5;
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
Warning Fpl_Mech_Slope_Stability_Dike::set_warning(const int warn_type){
	string place="Fpl_Mech_Slope_Stability_Dike::";
		string help;
		string reason;
		string reaction;
		int type=0;
		Warning msg;
		stringstream info;

	switch (warn_type){
		case 0://delta radius is greater than the maximum radius
			place.append("init_calculation(Fpl_Cub_Dike_Waterside *waterside, Fpl_Cub_Dike_Crest *crest, Fpl_Cub_Dike_Landside *landside, const int number_mat_zones, Fpl_Dike_Geo_Materialzone *mat_zones)");
			reason="The maximum radius of the slip circle is smaller than the given delta radius";
			help="Check the delta radius";
			reaction="Just one slip circle with the maximum radius is investigated per point";
			type=26;
			break;
		case 1://delta x <=0.0
			place.append("check_control_parameters(void)");
			reason="The delta x value for the set-up of the search raster is <= 0.0";
			help="Check the delta x value";
			reaction="It is set to the default value";
			type=26;
			break;
		case 2://delta y <=0.0
			place.append("check_control_parameters(void)");
			reason="The delta y value for the set-up of the search raster is <= 0.0";
			help="Check the delta y value";
			reaction="It is set to the default value";
			type=26;
			break;
		case 3://delta r <=0.0
			place.append("check_control_parameters(void)");
			reason="The delta radius value for the set-up of the search raster is <= 0.0";
			help="Check the delta radius value";
			reaction="It is set to the default value";
			type=26;
			break;
		case 4://delta h <=0.0
			place.append("check_control_parameters(void)");
			reason="The delta h value for the table storing of the wet slice zones is <= 0.0";
			help="Check the delta h value";
			reaction="It is set to the default value";
			type=26;
			break;
		case 5://slice width <=0.0
			place.append("check_control_parameters(void)");
			reason="The slice width <= 0.0";
			help="Check the slice width";
			reaction="It is set to the default value";
			type=26;
			break;
		case 6://no control parameters are found in database
			place.append("set_control_parameter(QSqlDatabase *ptr_database, const int section_id)");
			reason="No control parameter are found in database";
			help="Check the database table of the slope mechanism control parameter";
			reaction="The mechanism is not calculated";
			type=2;
			break;
		case 7://input datas can not submitted
			place.append("transfer_controlparam2database(const int section_id, QSqlDatabase *ptr_database)");
			reason="Can not submit the control parameter of the slope stability mechanism to database table";
			help="Check the database";
			type=2;
			break;
		case 8://0.75 percent of the slip circle are not relevant
			place.append("init_calculation(Geo_Simple_Polygon *outer_polygon, Geo_Polysegment *outer_polysegment, Geo_Point *base_land, Geo_Point *crest_mid, const int number_mat_zones, Fpl_Dike_Geo_Materialzone *mat_zones,  const double area_cubature)");
			reason="75 % of the theoretical slip circle are practical not relevant";
			help="Check the search raster";
			type=26;
			break;
		case 9://geometrical problems
			place.append("init_calculation(Geo_Simple_Polygon *outer_polygon, Geo_Polysegment *outer_polysegment, Geo_Point *base_land, Geo_Point *crest_mid, const int number_mat_zones, Fpl_Dike_Geo_Materialzone *mat_zones,  const double area_cubature)");
			reason="Geometrical problems are occurred; even after a radius change the geometrical calculation was not correct";
			reaction="The slip circle is not taken into account";
			help="Check the code";
			type=26;
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