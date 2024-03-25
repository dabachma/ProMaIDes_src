#include "Fpl_Headers_Precompiled.h"
//#include "Fpl_Sec_Type_Dune.h"

//init static members
Tables *Fpl_Sec_Type_Dune::result_table=NULL;

//Default constructor
Fpl_Sec_Type_Dune::Fpl_Sec_Type_Dune(void){
	//mechanism
	this->wave_wind_event=NULL;
	this->wave_runup_event=NULL;
	this->overflow_event=NULL;
	this->erosion_landside_event=NULL;
	this->erosion_waterside_gent_event=NULL;

	this->area_cubature=0.0;
	this->area_out_polygon=0.0;

	//set the point name
	this->base_land.set_point_name(fpl_label::base_land);
	this->crest_land.set_point_name(fpl_label::crest_land);

	this->waterlevel.set_unit(label::m);
	this->water_density.set_unit(label::kg_per_qm);

	this->current_waterlevel=0.0;

	this->number_mat_waterside_zones=0;
	this->material_waterside_zone=NULL;

	//common
	Sys_Memory_Count::self()->add_mem((sizeof(Fpl_Sec_Type_Dune)-2*sizeof(Fpl_Random_Variables)-
		sizeof(Fpl_Cub_Dike_Waterside)-sizeof(Fpl_Cub_Dike_Crest)-sizeof(Fpl_Cub_Dike_Landside)-sizeof(Fpl_Dike_Geo_Foreland)-sizeof(Fpl_Dike_Geo_Hinterland)-
		sizeof(Geo_Simple_Polygon)-sizeof(Geo_Polysegment)), _sys_system_modules::FPL_SYS);//count the memory
}
//Default destructor
Fpl_Sec_Type_Dune::~Fpl_Sec_Type_Dune(void){
	//delete the mechanism
	this->delete_overflow_mechanism();
	this->delete_wave_runup_mechanism();
	this->delete_wind_wave_mechanism();
	this->delete_erosion_landside_mechanism();
	this->delete_erosion_waterside_gent_mechanism();

	this->delete_waterside_variable_zone();

	//common
	Sys_Memory_Count::self()->minus_mem((sizeof(Fpl_Sec_Type_Dune)-2*sizeof(Fpl_Random_Variables)-
		sizeof(Fpl_Cub_Dike_Waterside)-sizeof(Fpl_Cub_Dike_Crest)-sizeof(Fpl_Cub_Dike_Landside)-sizeof(Fpl_Dike_Geo_Foreland)-sizeof(Fpl_Dike_Geo_Hinterland)-
		sizeof(Geo_Simple_Polygon)-sizeof(Geo_Polysegment)), _sys_system_modules::FPL_SYS);//count the memory
}
//______________
//public
//Set the input of the dune section (geometrie, variables, faulttree) per database
void Fpl_Sec_Type_Dune::set_input(const int section_id,  const bool frc_sim, QSqlDatabase *ptr_database){
	this->init_fault_tree(ptr_database);
	Sys_Common_Output::output_fpl->reset_prefix_was_outputed();
	//set general parameters
	this->set_general();
	Sys_Common_Output::output_fpl->reset_prefix_was_outputed();
	//read in the geometrie (always before the setting of the variables!)
	this->set_geometrie(ptr_database, section_id);
	Sys_Common_Output::output_fpl->reset_prefix_was_outputed();
	//check the geometry
	try{
		this->check_set_geometry();
	}
	catch(Error msg){
	}
	Sys_Common_Output::output_fpl->reset_prefix_was_outputed();
	//set the input for the varaibles which belongs to the section_type class
	this->set_variables(frc_sim, ptr_database, section_id);
	Sys_Common_Output::output_fpl->reset_prefix_was_outputed();
	//set the control parameters of the mechanisms
	this->set_control_parameter_mechanism(ptr_database, section_id, frc_sim);
	Sys_Common_Output::output_fpl->reset_prefix_was_outputed();
	//init the mechanism
	this->init_mechanism(section_id);
	Sys_Common_Output::output_fpl->reset_prefix_was_outputed();
}
//Read in the fpl-section type from file
void Fpl_Sec_Type_Dune::read_section_type_per_file(QFile *ifile, int *line_counter, const bool frc_sim){
	ostringstream cout;
	cout << "Input the general data of the dune section..."<<endl;
	Sys_Common_Output::output_fpl->output_txt(&cout);

	qint64 pos_file=0;
	int store_line=0;
	try{
		pos_file=this->find_start_block_file(ifile, line_counter, fpl_label::begin_sec_general, fpl_label::end_sec_general, true);
		store_line=*line_counter;
	}
	catch(Error msg){
		throw msg;
	}
	//read in the random variables general block
	try{
		if(frc_sim==false){
			this->waterlevel.set_input(ifile, line_counter, fpl_label::waterlevel, false, fpl_label::end_sec_general);
		}
		else{
			this->waterlevel.set_input(ifile, line_counter, fpl_label::waterlevel_frc, true, fpl_label::end_sec_general);
		}
		this->waterlevel.check_positiv_zero_variable();
		ifile->seek(pos_file);
		*line_counter=store_line;

		this->water_density.set_input(ifile, line_counter, fpl_label::water_density, false, fpl_label::end_sec_general);
		this->water_density.check_given_boundaries(2000.0, 700.0);
		ifile->seek(pos_file);
		*line_counter=store_line;
	}
	catch(Error msg){
		throw msg;
	}
	//read geometry general block
	try{
		this->crest_land.set_point_per_file(ifile, line_counter, fpl_label::crest_land,0, fpl_label::end_sec_general);
		ifile->seek(pos_file);
		*line_counter=store_line;
		this->base_land.set_point_per_file(ifile, line_counter, fpl_label::base_land,0, fpl_label::end_sec_general);
		ifile->seek(pos_file);
		*line_counter=store_line;
	}
	catch(Error msg){
		throw msg;
	}
	//set the crest segment (just one segment)
	this->crest_cubature.set_segment(this->crest_water, &this->crest_land);
	cout << "Input of the general data of the dune section is finished"<<endl;
	Sys_Common_Output::output_fpl->output_txt(&cout);
}
//Read the mechanisms of the fault-tree per file
void Fpl_Sec_Type_Dune::read_mechanism_faulttree_perfile(QFile *ifile, int *line_counter){
	qint64 pos_file=0;
	int store_line=0;
	ostringstream cout;
	cout << "Input the mechanisms of the dune section..."<<endl;
	Sys_Common_Output::output_fpl->output_txt(&cout);

	ifile->reset();
	*line_counter=0;
	Fpl_Calculation::check_stop_thread_flag();
	//overflow
	try{
		pos_file=this->find_start_block_file(ifile, line_counter, fpl_label::begin_mech_overflow, fpl_label::end_mech_overflow, false);
		if(pos_file!=0){
			store_line=*line_counter;
			this->overflow_event->set_variables(ifile, line_counter, pos_file, store_line);
			this->overflow_event->set_2calc_flag(true);
		}
	}
	catch(Error msg){
		throw msg;
	}
	ifile->reset();
	*line_counter=0;
	Fpl_Calculation::check_stop_thread_flag();;
	//wind wave
	try{
		pos_file=this->find_start_block_file(ifile, line_counter, fpl_label::begin_mech_wind_wave, fpl_label::end_mech_wind_wave, false);
		if(pos_file!=0){
			store_line=*line_counter;
			this->wave_wind_event->set_section_direction(this->section_direction);
			this->wave_wind_event->set_input(ifile, line_counter, pos_file, store_line);
			this->wave_wind_event->set_2calc_flag(true);
		}
	}
	catch(Error msg){
		throw msg;
	}
	ifile->reset();
	*line_counter=0;
	Fpl_Calculation::check_stop_thread_flag();
	//wave runup
	try{
		pos_file=this->find_start_block_file(ifile, line_counter, fpl_label::begin_mech_wave_runup, fpl_label::end_mech_wave_runup, false);
		if(pos_file!=0){
			store_line=*line_counter;
			this->wave_runup_event->set_input(ifile, line_counter, pos_file, store_line);
			this->wave_runup_event->set_2calc_flag(true);
		}
	}
	catch(Error msg){
		throw msg;
	}
	ifile->reset();
	*line_counter=0;
	Fpl_Calculation::check_stop_thread_flag();

	//landside erosion
	try{
		pos_file=this->find_start_block_file(ifile, line_counter, fpl_label::begin_mech_landside_ero, fpl_label::end_mech_landside_ero, true);
		if(pos_file!=0){
			store_line=*line_counter;
			this->erosion_landside_event->set_variables(ifile, line_counter, pos_file, store_line);
			this->erosion_landside_event->set_2calc_flag(true);
		}
	}
	catch(Error msg){
		throw msg;
	}
	ifile->reset();
	*line_counter=0;
	Fpl_Calculation::check_stop_thread_flag();

	//waterside erosion
	try{
		pos_file=this->find_start_block_file(ifile, line_counter, fpl_label::begin_mech_waterside_ero, fpl_label::end_mech_waterside_ero, false);
		if(pos_file!=0){
			store_line=*line_counter;
			this->erosion_waterside_gent_event->set_variables(ifile, line_counter, pos_file, store_line);
			this->erosion_waterside_gent_event->set_2calc_flag(true);
		}
	}
	catch(Error msg){
		throw msg;
	}
	ifile->reset();
	*line_counter=0;
	Fpl_Calculation::check_stop_thread_flag();

	this->check_fault_tree();

	cout << "Input of the mechanisms of the dune section is finished"<<endl;
	Sys_Common_Output::output_fpl->output_txt(&cout);
}
//Check which mechanisms of the fault-tree are required per file
void Fpl_Sec_Type_Dune::check_mechanism_faulttree_perfile(QFile *ifile, int *line_counter){
	qint64 pos_file=0;
	int store_line=0;
	ostringstream cout;
	cout << "Check the mechanisms of the dune section..."<<endl;
	Sys_Common_Output::output_fpl->output_txt(&cout);

	ifile->reset();
	*line_counter=0;
	Fpl_Calculation::check_stop_thread_flag();
	//overflow
	try{
		pos_file=this->find_start_block_file(ifile, line_counter, fpl_label::begin_mech_overflow, fpl_label::end_mech_overflow, false);
		if(pos_file!=0){
			store_line=*line_counter;
			this->allocate_overflow_mechanism();
		}
	}
	catch(Error msg){
		throw msg;
	}
	ifile->reset();
	*line_counter=0;
	Fpl_Calculation::check_stop_thread_flag();;
	//wind wave
	try{
		pos_file=this->find_start_block_file(ifile, line_counter, fpl_label::begin_mech_wind_wave, fpl_label::end_mech_wind_wave, false);
		if(pos_file!=0){
			store_line=*line_counter;
			this->allocate_wind_wave_mechanism();
		}
	}
	catch(Error msg){
		throw msg;
	}
	ifile->reset();
	*line_counter=0;
	Fpl_Calculation::check_stop_thread_flag();
	//wave runup
	try{
		pos_file=this->find_start_block_file(ifile, line_counter, fpl_label::begin_mech_wave_runup, fpl_label::end_mech_wave_runup, false);
		if(pos_file!=0){
			store_line=*line_counter;
			this->allocate_wave_runup_mechanism();
		}
	}
	catch(Error msg){
		throw msg;
	}
	ifile->reset();
	*line_counter=0;
	Fpl_Calculation::check_stop_thread_flag();


	//erosion landside
	try{
		pos_file=this->find_start_block_file(ifile, line_counter, fpl_label::begin_mech_landside_ero, fpl_label::end_mech_landside_ero, true);
		if(pos_file!=0){
			store_line=*line_counter;
			this->allocate_erosion_landside_mechanism();
		}
	}
	catch(Error msg){
		throw msg;
	}
	ifile->reset();
	*line_counter=0;
	Fpl_Calculation::check_stop_thread_flag();

	//erosion waterside
	try{
		pos_file=this->find_start_block_file(ifile, line_counter, fpl_label::begin_mech_waterside_ero, fpl_label::end_mech_waterside_ero, false);
		if(pos_file!=0){
			store_line=*line_counter;
			this->allocate_erosion_waterside_gent_mechanism();
		}
	}
	catch(Error msg){
		throw msg;
	}
	ifile->reset();
	*line_counter=0;
	Fpl_Calculation::check_stop_thread_flag();

	cout << "Check of the mechanisms of the dune section is finished"<<endl;
	Sys_Common_Output::output_fpl->output_txt(&cout);
}
//Read the waterside geometry of the section per file
void Fpl_Sec_Type_Dune::read_waterside_geometry(QFile *ifile, int *line_counter){
	ostringstream cout;
	cout << "Input the waterside cubature geometry of the dune section..."<<endl;
	Sys_Common_Output::output_fpl->output_txt(&cout);
	qint64 pos_file=0;
	int store_line=0;
	try{
		pos_file=this->find_start_block_file(ifile, line_counter, fpl_label::begin_waterside_cub, fpl_label::end_waterside_cub, false);
		store_line=*line_counter;
	}
	catch(Error msg){
		throw msg;
	}
	try{
		this->waterside_cubature.set_segments(&this->base_water, this->crest_water, ifile, line_counter);
	}
	catch(Error msg){
		throw msg;
	}
	//reset
	ifile->reset();
	*line_counter=0;

	cout << "Input of the waterside cubature geometry of the dune section is finished"<<endl;
	Sys_Common_Output::output_fpl->output_txt(&cout);

	cout << "Input the waterside geometry of the dune section (foreland)..."<<endl;
	Sys_Common_Output::output_fpl->output_txt(&cout);
	pos_file=0;
	store_line=0;
	try{
		pos_file=this->find_start_block_file(ifile, line_counter, fpl_label::begin_foreland, fpl_label::end_foreland, false);
		store_line=*line_counter;
	}
	catch(Error msg){
		throw msg;
	}
	try{
		this->foreland.set_segments(&this->base_water, ifile, line_counter);
	}
	catch(Error msg){
		throw msg;
	}

	cout << "Input of the waterside geometry of the dune section (foreland) is finished"<<endl;
	Sys_Common_Output::output_fpl->output_txt(&cout);
}
//Read the landside geometry of the section per file
void Fpl_Sec_Type_Dune::read_landside_geometry(QFile *ifile, int *line_counter){
		ostringstream cout;
	cout << "Input the landside cubature geometry of the dune section..."<<endl;
	Sys_Common_Output::output_fpl->output_txt(&cout);
	qint64 pos_file=0;
	int store_line=0;
	try{
		pos_file=this->find_start_block_file(ifile, line_counter, fpl_label::begin_landside_cub, fpl_label::end_landside_cub, false);
		store_line=*line_counter;
	}
	catch(Error msg){
		throw msg;
	}
	try{
		this->landside_cubature.set_segments(&this->base_land, &this->crest_land, ifile, line_counter);
	}
	catch(Error msg){
		throw msg;
	}

	//reset
	ifile->reset();
	*line_counter=0;

	cout << "Input of the landside cubature geometry of the dune section is finished"<<endl;
	Sys_Common_Output::output_fpl->output_txt(&cout);

	cout << "Input the landside geometry of the dune section (hinterland)..."<<endl;
	Sys_Common_Output::output_fpl->output_txt(&cout);
	pos_file=0;
	store_line=0;
	try{
		pos_file=this->find_start_block_file(ifile, line_counter, fpl_label::begin_hinterland, fpl_label::end_hinterland, false);
		store_line=*line_counter;
	}
	catch(Error msg){
		throw msg;
	}
	try{
		this->hinterland.set_segments(&this->base_land, ifile, line_counter);
	}
	catch(Error msg){
		throw msg;
	}

	cout << "Input of the landside geometry of the dune section (hinterland) is finished"<<endl;
	Sys_Common_Output::output_fpl->output_txt(&cout);
}
//Read the waterside material variable zone(s) of the section per file
void Fpl_Sec_Type_Dune::read_var_waterside_matzones(QFile *ifile, int *line_counter){
	ostringstream cout;
	cout << "Input the material variable zone(s) of the waterside dune cubature..."<<endl;
	Sys_Common_Output::output_fpl->output_txt(&cout);
	qint64 pos_file=0;
	int store_line=0;
	try{
		pos_file=this->find_start_block_file(ifile, line_counter, fpl_label::begin_material_variable_zone_waterside, fpl_label::end_material_variable_zone_waterside, false);
		store_line=*line_counter;
	}
	catch(Error msg){
		throw msg;
	}
	try{
		//read in
		string myline;
		QString qmyline;

		//find number of zones
		do{
			qmyline=ifile->readLine();
			myline=qmyline.toStdString();
			(*line_counter)++;
			functions::clean_string(&myline);
			if(myline==fpl_label::end_material_variable_zone_waterside){
				break;
			}
			if(myline.empty()==false){
				int pos=-1;
				string buffer;
				stringstream buffer1;
				bool wrong_input=false;

				pos=myline.find(fpl_label::key_no_material);
				if(pos>=0 && wrong_input==false){
					buffer=myline.substr(fpl_label::key_no_material.length());
					functions::clean_string(&buffer);
					buffer1 << buffer;
					buffer1 >> this->number_mat_waterside_zones;
					if(buffer1.fail()==true){
						wrong_input=true;
					}
				}

				if(wrong_input==true){
					ostringstream info;
					info << "Wrong input sequenze  : " << buffer1.str() << endl;
					Error msg=this->set_error(17);
					msg.make_second_info(info.str());
					throw msg;
				}
			}
			if(this->number_mat_waterside_zones>0){
				break;
			}
		}
		while(ifile->atEnd()==false);

		if(this->number_mat_waterside_zones==0){
			if(this->wave_runup_event==NULL){
				cout <<"No waterside material variable zone(s) are found... "  << endl ;
				Sys_Common_Output::output_fpl->output_txt(&cout,false);
				return;
			}
			else{
				Error msg=this->set_error(18);
				throw msg;
			}
		}

		//allocate the
		this->allocate_waterside_variable_zone();

		bool runup=false;
		if(this->wave_runup_event!=NULL){
			runup=true;
		}
		bool wave_impact=false;
		bool erosion=false;
		

		
		//read in the zones
		for(int i=0; i< this->number_mat_waterside_zones; i++){
			this->material_waterside_zone[i].read_matzone_per_file(ifile, line_counter, runup, wave_impact, erosion);
		}

		//connect zone
		this->connect_material_zones_waterside();
	}
	catch(Error msg){
		throw msg;
	}

	cout << "Input the material variable zone(s) of the waterside dune cubature is finished!"<<endl;
	Sys_Common_Output::output_fpl->output_txt(&cout);

}
//Transfer the section type data to database, e.g. geometry, random variables etc.
void Fpl_Sec_Type_Dune::transfer_sectiontype2database(const int section_id, QSqlDatabase *ptr_database){
	ostringstream cout;
	cout << "Transfer the section type data of a dune to database..."<<endl;
	Sys_Common_Output::output_fpl->output_txt(&cout);

	cout << "Transfer general random variables to database..."<<endl;
	Sys_Common_Output::output_fpl->output_txt(&cout);
	this->waterlevel.transfer_rand2database(ptr_database, this->system_id, section_id, fpl_label::mech_all, fpl_label::sec_dune);
	this->water_density.transfer_rand2database(ptr_database, this->system_id, section_id, fpl_label::mech_all, fpl_label::sec_dune);

	cout << "Transfer general geometry to database..."<<endl;
	Sys_Common_Output::output_fpl->output_txt(&cout);

	this->crest_land.transfer_point2database(this->system_id, section_id, ptr_database, fpl_label::region_cubature, fpl_label::sec_dune);
	this->base_land.transfer_point2database(this->system_id, section_id, ptr_database, fpl_label::region_cubature, fpl_label::sec_dune);

	this->foreland.transfer_geometry2database(section_id, this->system_id, ptr_database);
	this->waterside_cubature.transfer_geometry2database(section_id, this->system_id, ptr_database);
	this->landside_cubature.transfer_geometry2database(section_id, this->system_id, ptr_database);
	this->hinterland.transfer_geometry2database(section_id, this->system_id, ptr_database);

	//cout << "Transfer "<<this->number_material_zones<<" geometrical material zone(s) to database..."<<endl;
	//Sys_Common_Output::output_fpl->output_txt(&cout);
	////geometrical material zones
	//for(int i=0; i< this->number_material_zones;i++){
	//	this->material_zones[i].transfer_matzone2database(ptr_database, section_id);
	//}
	//cout << "Transfer "<<this->number_material_variable_zones<<" material variable zone(s) to database..."<<endl;
	//Sys_Common_Output::output_fpl->output_txt(&cout);
	////material zones
	//for(int i=0; i< this->number_material_variable_zones;i++){
	//	this->material_variable_zones[i].transfer_data2database(section_id, this->system_id, ptr_database);
	//}

	cout << "Transfer "<<this->number_mat_waterside_zones<<" waterside material variable zone(s) to database..."<<endl;
	Sys_Common_Output::output_fpl->output_txt(&cout);
	//material zones
	for(int i=0; i< this->number_mat_waterside_zones;i++){
		this->material_waterside_zone[i].transfer_data2database(section_id, this->system_id, ptr_database);
	}

	cout << "Transfer the mechanism data to database..."<<endl;
	Sys_Common_Output::output_fpl->output_txt(&cout);

	//output wind wave generation
	if(this->wave_wind_event!=NULL){
		this->wave_wind_event->transfer_data2database(section_id,this->system_id, ptr_database);
	}
	//output wave runup
	if(this->wave_runup_event!=NULL){
		this->wave_runup_event->transfer_data2database(section_id,this->system_id, ptr_database);
	}
	//output overflow
	if(this->overflow_event!=NULL){
		this->overflow_event->transfer_data2database(section_id,this->system_id, ptr_database);
	}
	//slope landside
	//if(this->slope_stability_landside_event!=NULL){
	//	this->slope_stability_landside_event->transfer_data2database(section_id,this->system_id, ptr_database);
	//}

	//slope landside erosion
	if(this->erosion_landside_event!=NULL){
		this->erosion_landside_event->transfer_data2database(section_id,this->system_id, ptr_database);
	}

	//slope waterside erosion
	if(this->erosion_waterside_gent_event!=NULL){
		this->erosion_waterside_gent_event->transfer_data2database(section_id,this->system_id, ptr_database);
	}
}
//Calculate the fault tree with random distributed variables (true) or with deterministic variables (false)
int Fpl_Sec_Type_Dune::make_faulttree(const bool random_calculation){
	int buff_failure=0;
	if(random_calculation==true){
		//reset the random generated flag
		this->reset_random_flag();
	}

	//dune section type variables
	//calculate the waterlevel
	this->current_waterlevel=this->waterlevel.calculate_variables(random_calculation).end_result;
		//check if a failure is possible
	if(this->current_waterlevel <= this->base_land.get_ycoordinate() || this->current_waterlevel <= this->base_water.get_ycoordinate()){
		this->low_waterlevel_flag=true;
		return buff_failure;
	}
	this->low_waterlevel_flag=false;
	//calculate the waterdensity
	this->water_density.calculate_variables(random_calculation);


	//calc the material zone variables of the waterside
	for(int i=0; i< this->number_mat_waterside_zones; i++){
		this->material_waterside_zone[i].generate_random_variables(random_calculation);
	}

	//wind wave generation
	//init the krylow result
	_fpl_waveparam_result buff_krylowII;
	buff_krylowII.mean_waterdepth=0.0;
	buff_krylowII.wave_height=0.0;
	buff_krylowII.wave_length=0.0;
	buff_krylowII.wave_period=0.0;
	buff_krylowII.wind_direction=0.0;
	buff_krylowII.wave_setup=0.0;
	buff_krylowII.wave_height_original=0.0;

	if(this->wave_wind_event!=NULL){
		if(this->wave_wind_event->get_2calc_flag()==true){
			buff_krylowII=this->wave_wind_event->calculate_wind_wave_generation(random_calculation, this->current_waterlevel);
		}
	}
	//waterlevel after wind setup
	this->current_waterlevel=this->current_waterlevel+buff_krylowII.wave_setup;

	bool total_is_set=false;
	//hydraulic
	//overflow event
	//init overflow results
	double buff_q_overflow=0.0;
	if(this->overflow_event!=NULL){
		if(this->overflow_event->get_2calc_flag()==true){
			buff_q_overflow=this->overflow_event->calculate_mechanism(random_calculation, this->current_waterlevel);
		}
	}
	//wave runup
	//init wave runup results
	_fpl_wave_runup_result buff_runup;
	buff_runup.wave_overtopping=0.0;
	buff_runup.wave_runup=0.0;
	if(this->wave_runup_event!=NULL){
		if(this->wave_runup_event->get_2calc_flag()==true){
			buff_runup=this->wave_runup_event->calculate_mechanism(random_calculation, buff_krylowII.wind_direction, this->section_direction, buff_krylowII.wave_height,buff_krylowII.wave_period,buff_krylowII.wave_length, this->current_waterlevel);
			
		}
	}
	
	//sum up the q overflow
	double total_q_buff=buff_q_overflow+buff_runup.wave_overtopping;
	//check landside erosion event
	bool erosion_landside_failure=false;
	if(this->erosion_landside_event!=NULL){
		if(this->erosion_landside_event->get_2calc_flag()==true){
			erosion_landside_failure=this->erosion_landside_event->calculate_mechanism(random_calculation, total_q_buff);
			if(erosion_landside_failure==true){
				buff_failure=1;
				if(total_is_set==false){
					if(buff_q_overflow>0.0){
						this->overflow_event->sum_up_counter_single_prob();
						this->overflow_event->sum_up_counter_tree_prob();
						total_is_set=true;
					}
					else{
						this->wave_runup_event->sum_up_counter_single_prob();
						this->wave_runup_event->sum_up_counter_tree_prob();
						total_is_set=true;
					}
				}
			}
		}
	}
	
	//check waterside erosion event
	bool erosion_waterside_failure=false;
	if(this->erosion_waterside_gent_event!=NULL){
		if(this->erosion_waterside_gent_event->get_2calc_flag()==true){
			erosion_waterside_failure=this->erosion_waterside_gent_event->calculate_mechanism(random_calculation, this->current_waterlevel, buff_krylowII.wave_height, buff_krylowII.wave_period, this->water_density.get_result().end_result, this->section_direction, buff_krylowII.wind_direction );
			if(erosion_waterside_failure==true){
				this->erosion_waterside_gent_event->sum_up_counter_single_prob();
				if(total_is_set==false){
					this->erosion_waterside_gent_event->sum_up_counter_tree_prob();
					total_is_set=true;
				}
			}
		}
	}

	//summon the failure to one failure
	if(buff_failure==1 ||  erosion_waterside_failure==true){
		buff_failure=1;
	}

	return buff_failure;

}
//Check the statistics of the random variables used in the section type
void Fpl_Sec_Type_Dune::check_statistic(void){
	this->waterlevel.check_statistic();
	this->water_density.check_statistic();

	//the material zone variables of the waterside
	for(int i=0; i< this->number_mat_waterside_zones; i++){
		this->material_waterside_zone[i].check_statistic();
	}

	//wind wave generation
	if(this->wave_wind_event!=NULL){
		if(this->wave_wind_event->get_2calc_flag()==true){
			this->wave_wind_event->check_statistic();
		}
	}
	//wave runup
	if(this->wave_runup_event!=NULL){
		if(this->wave_runup_event->get_2calc_flag()==true){
			this->wave_runup_event->check_statistic();
		}
	}
	//overflow event
	if(this->overflow_event!=NULL){
		if(this->overflow_event->get_2calc_flag()==true){
			this->overflow_event->check_statistic();
		}
	}
	//erosion landside
	if(this->erosion_landside_event!=NULL){
		if(this->erosion_landside_event->get_2calc_flag()==true){
			this->erosion_landside_event->check_statistic();
		}
	}

	//erosion waterside
	if(this->erosion_waterside_gent_event!=NULL){
		if(this->erosion_waterside_gent_event->get_2calc_flag()==true){
			this->erosion_waterside_gent_event->check_statistic();
		}
	}

	//slope stability landside
	//if(this->slope_stability_landside_event!=NULL){
	//	if(this->slope_stability_landside_event->get_2calc_flag()==true){
	//		this->slope_stability_landside_event->check_statistic();
	//	}
	//}
}
//Output the statistics of the random variables used in the section type to display/console
void Fpl_Sec_Type_Dune::output_statistic(void){
	ostringstream cout;
	cout << "Statistics of Variables (dune)..." << endl;
	Sys_Common_Output::output_fpl->output_txt(cout.str(),true);
	//dune section type members
	this->waterlevel.output_statistic();
	this->water_density.output_statistic();

	//the material zone variables of the waterside
	for(int i=0; i< this->number_mat_waterside_zones; i++){
		this->material_waterside_zone[i].output_statistic();
	}

	//output wind wave generation
	if(this->wave_wind_event!=NULL){
		if(this->wave_wind_event->get_2calc_flag()==true){
			this->wave_wind_event->output_statistic();
		}
	}
	//wave runup
	if(this->wave_runup_event!=NULL){
		if(this->wave_runup_event->get_2calc_flag()==true){
			this->wave_runup_event->output_statistic();
		}
	}
	//overflow event
	if(this->overflow_event!=NULL){
		if(this->overflow_event->get_2calc_flag()==true){
			this->overflow_event->output_statistic();
		}
	}
	//erosion landside
	if(this->erosion_landside_event!=NULL){
		if(this->erosion_landside_event->get_2calc_flag()==true){
			this->erosion_landside_event->output_statistic();
		}
	}

	//erosion waterside
	if(this->erosion_waterside_gent_event!=NULL){
		if(this->erosion_waterside_gent_event->get_2calc_flag()==true){
			this->erosion_waterside_gent_event->output_statistic();
		}
	}

	//slope stability landside
	//if(this->slope_stability_landside_event!=NULL){
	//	if(this->slope_stability_landside_event->get_2calc_flag()==true){
	//		this->slope_stability_landside_event->output_statistic();
	//	}
	//}
}
//Reset the statistic important values
void Fpl_Sec_Type_Dune::reset_statistics(void){
	this->waterlevel.reset_statistics();
	this->water_density.reset_statistics();

	//the material zone variables of the waterside
	for(int i=0; i< this->number_mat_waterside_zones; i++){
		this->material_waterside_zone[i].reset_statistic();
	}

	//output wind wave generation
	if(this->wave_wind_event!=NULL){
		if(this->wave_wind_event->get_2calc_flag()==true){
			this->wave_wind_event->reset_statistics();;
		}
	}
	//wave runup
	if(this->wave_runup_event!=NULL){
		if(this->wave_runup_event->get_2calc_flag()==true){
			this->wave_runup_event->reset_statistics();
		}
	}
	//overflow event
	if(this->overflow_event!=NULL){
		if(this->overflow_event->get_2calc_flag()==true){
			this->overflow_event->reset_statistics();
		}
	}
	//erosion landside
	if(this->erosion_landside_event!=NULL){
		if(this->erosion_landside_event->get_2calc_flag()==true){
			this->erosion_landside_event->reset_statistics();
		}
	}
	//erosion waterside
	if(this->erosion_waterside_gent_event!=NULL){
		if(this->erosion_waterside_gent_event->get_2calc_flag()==true){
			this->erosion_waterside_gent_event->reset_statistics();
		}
	}


}
//Output the section members to display/console
void Fpl_Sec_Type_Dune::output_member(void){
	ostringstream cout;
	ostringstream prefix;
	prefix << "GEO> ";
	//geometrie
	cout << "Geometrie members dune type..." << endl;
	Sys_Common_Output::output_fpl->output_txt(&cout,true);
	Sys_Common_Output::output_fpl->set_userprefix(&prefix);
	//main points of the dune
	this->base_water.output_members(&cout);
	this->crest_water->output_members(&cout);
	this->crest_land.output_members(&cout);
	this->base_land.output_members(&cout);
	Sys_Common_Output::output_fpl->output_txt(&cout,true);
	//foreland
	this->foreland.output_geometrie();
	//waterside cubature
	this->waterside_cubature.output_geometrie();
	//crest cubature
	this->crest_cubature.output_geometrie();
	//landside cubature
	this->landside_cubature.output_geometrie();
	//hinterland
	this->hinterland.output_geometrie();

	//the material zone variables of the waterside
	if(this->number_mat_waterside_zones>0){
		cout <<"Output "<<this->number_mat_waterside_zones<< " number of dike material zone(s) of the waterside..."<<endl;
		Sys_Common_Output::output_fpl->output_txt(&cout,true);
		for(int i=0; i< this->number_mat_waterside_zones; i++){
			this->material_waterside_zone[i].output_member();
		}
	}

	Sys_Common_Output::output_fpl->rewind_userprefix();

	//variables
	cout << "Variable members dune type..." << endl;
	Sys_Common_Output::output_fpl->output_txt(&cout,true);

	//section type members
	this->waterlevel.output_member();
	this->water_density.output_member();

	//output wind wave generation
	if(this->wave_wind_event!=NULL){
		this->wave_wind_event->output_members();
	}
	//output wave runup
	if(this->wave_runup_event!=NULL){
		this->wave_runup_event->output_members();
	}
	//output overflow
	if(this->overflow_event!=NULL){
		this->overflow_event->output_members();
	}
	//output erosion landside
	if(this->erosion_landside_event!=NULL){
		this->erosion_landside_event->output_members();
	}
	//output erosion waterside
	if(this->erosion_waterside_gent_event!=NULL){
		this->erosion_waterside_gent_event->output_members();
	}

	//slope stability landside
	//if(this->slope_stability_landside_event!=NULL){
	//	this->slope_stability_landside_event->output_members();
	//}
}
//Output the reliability of the fault tree mechanisms for a deterministic calculation to display/console
void Fpl_Sec_Type_Dune::output_reliability(string output_folder, const int sec_id, const string sec_name){
	ostringstream cout;
	ostringstream prefix;
	prefix << "RES"<<"> ";
	Sys_Common_Output::output_fpl->set_userprefix(prefix.str());
	cout << "Deterministic results of the sectiontype dune..." << endl;
	Sys_Common_Output::output_fpl->output_txt(&cout);
	if(this->low_waterlevel_flag==true){
		cout <<" There are no deterministic results due to low waterlevel"<<endl;
		Sys_Common_Output::output_fpl->output_txt(&cout);
		return;
	}
	//general
	this->waterlevel.output_deterministic();
	this->water_density.output_deterministic();

	//the material zone variables of the waterside
	for(int i=0; i< this->number_mat_waterside_zones; i++){
		this->material_waterside_zone[i].output_determ_results();
	}
	
	//wind wave generation
	if(this->wave_wind_event!=NULL){
		if(this->wave_wind_event->get_2calc_flag()==true){
			this->wave_wind_event->output_determ_results();
		}
	}
	//output wave runup
	if(this->wave_runup_event!=NULL){
		if(this->wave_runup_event->get_2calc_flag()==true){
			this->wave_runup_event->output_determ_results();
		}
	}
	//overflow event
	if(this->overflow_event!=NULL){
		if(this->overflow_event->get_2calc_flag()==true){
			this->overflow_event->output_determ_results();
		}
	}
	//erosion landside
	if(this->erosion_landside_event!=NULL){
		if(this->erosion_landside_event->get_2calc_flag()==true){
			this->erosion_landside_event->output_determ_results();
		}
	}

	//erosion waterside
	if(this->erosion_waterside_gent_event!=NULL){
		if(this->erosion_waterside_gent_event->get_2calc_flag()==true){
			this->erosion_waterside_gent_event->output_determ_results();

			if (this->output_flags.tec_output == true) {
				//check folder
				string folder_buff;
				folder_buff = Fpl_Section::check_generate_folder("tecplot", &output_folder);
				//create name
				ostringstream buffer;
				buffer << folder_buff << "NEWPROFILE_TEC_" << sec_id << "_" << sec_name << ".dat";
				this->erosion_waterside_gent_event->output_results2tecplot(buffer.str());
			}
			if (this->output_flags.para_output == true) {
				//check folder
				string folder_buff;
				folder_buff = Fpl_Section::check_generate_folder("paraview", &output_folder);
				//create name
				ostringstream buffer;
				buffer << folder_buff << "NEWPROFILE_PARA_" << sec_id << "_" << sec_name << ".csv";
				//TODO
				this->erosion_waterside_gent_event->output_results2paraview(buffer.str());
			}
			if (this->output_flags.excel_output == true) {
				//check folder
				string folder_buff;
				folder_buff = Fpl_Section::check_generate_folder("excel", &output_folder);
				//create name
				ostringstream buffer;
				buffer << folder_buff << "NEWPROFILE_EXCEL_" << sec_id << "_" << sec_name << ".csv";
				//TODO
				this->erosion_waterside_gent_event->output_results2excel(buffer.str());
			}

		}
	}

	Sys_Common_Output::output_fpl->rewind_userprefix();
}
//Output the geometry to tecplot
void Fpl_Sec_Type_Dune::output_geometry2tecplot(ofstream *output){
	//output the zone header
	*output  << P(4) << FORMAT_FIXED_REAL;

	*output <<"#######outer geometry########"<<endl;
	if(this->foreland.get_number_segments()>0){
		*output << endl << "ZONE T= "<< "\" Foreland \" "<< " I = " << this->foreland.get_number_segments()+1<< endl;
		for(int i=0; i< this->foreland.get_number_segments(); i++){
			*output << W(12) << this->foreland.get_segment(i)->point1.get_xcoordinate() << W(12) << this->foreland.get_segment(i)->point1.get_ycoordinate() <<  endl;
		}

		*output << W(12) << this->foreland.get_segment(this->foreland.get_number_segments()-1)->point2.get_xcoordinate() << W(12) << this->foreland.get_segment(this->foreland.get_number_segments()-1)->point2.get_ycoordinate() <<  endl;
	}
	output->flush();

	if(this->waterside_cubature.get_number_segments()>0){
		*output << endl << "ZONE T= "<< "\" Waterside cubature \" "<< " I = " << this->waterside_cubature.get_number_segments()+1 << endl;
		for(int i=0; i< this->waterside_cubature.get_number_segments(); i++){
			*output << W(12) << this->waterside_cubature.get_segment(i)->point1.get_xcoordinate() << W(12) << this->waterside_cubature.get_segment(i)->point1.get_ycoordinate() <<  endl;
		}

		*output << W(12) << this->waterside_cubature.get_segment(this->waterside_cubature.get_number_segments()-1)->point2.get_xcoordinate() << W(12) << this->waterside_cubature.get_segment(this->waterside_cubature.get_number_segments()-1)->point2.get_ycoordinate() <<  endl;
	}
	output->flush();

	*output << endl << "ZONE T= "<< "\" Crest cubature \" "<< " I = " << 2 << endl;
	*output << W(12) << this->crest_cubature.get_segment()->point1.get_xcoordinate() << W(12) << this->crest_cubature.get_segment()->point1.get_ycoordinate() <<  endl;
	*output << W(12) << this->crest_cubature.get_segment()->point2.get_xcoordinate() << W(12) << this->crest_cubature.get_segment()->point2.get_ycoordinate() <<  endl;
	output->flush();

	if(this->landside_cubature.get_number_segments()>0){
		*output << endl << "ZONE T= "<< "\" Landside cubature \" "<< " I = " << this->landside_cubature.get_number_segments()+1 << endl;
		for(int i=0; i< this->landside_cubature.get_number_segments(); i++){
			*output << W(12) << this->landside_cubature.get_segment(i)->point1.get_xcoordinate() << W(12) << this->landside_cubature.get_segment(i)->point1.get_ycoordinate() <<  endl;
		}
		*output << W(12) << this->landside_cubature.get_segment(this->landside_cubature.get_number_segments()-1)->point2.get_xcoordinate() << W(12) << this->landside_cubature.get_segment(this->landside_cubature.get_number_segments()-1)->point2.get_ycoordinate() <<  endl;
	}
	output->flush();

	if(this->hinterland.get_number_segments()>0){
		*output << endl << "ZONE T= "<< "\" Hinterland \" "<< " I = " << this->hinterland.get_number_segments()+1 << endl;
		for(int i=0; i< this->hinterland.get_number_segments(); i++){
			*output << W(12) << this->hinterland.get_segment(i)->point1.get_xcoordinate() << W(12) << this->hinterland.get_segment(i)->point1.get_ycoordinate() <<  endl;
		}
		*output << W(12) << this->hinterland.get_segment(this->hinterland.get_number_segments()-1)->point2.get_xcoordinate() << W(12) << this->hinterland.get_segment(this->hinterland.get_number_segments()-1)->point2.get_ycoordinate() <<  endl;
	}
	output->flush();
	*output<<endl;
}
//Output the geometry to paraview
void Fpl_Sec_Type_Dune::output_geometry2paraview(ofstream *output) {
	//output the zone header
	*output << P(4) << FORMAT_FIXED_REAL;
	//header
	*output << " x,";
	int count_col_tot = 1;
	if (this->foreland.get_number_segments() > 0) {
		*output << " foreland,";
		count_col_tot++;
	}
	if (this->waterside_cubature.get_number_segments() > 0) {
		*output << " waterside,";
		count_col_tot++;

	}
	if (this->crest_cubature.get_number_segments() > 0) {

		*output << " crest,";
		count_col_tot++;
	}

	if (this->landside_cubature.get_number_segments() > 0) {
		*output << " landside,";
		count_col_tot++;
	}
	if (this->hinterland.get_number_segments() > 0) {
		*output << " hinterland";
		count_col_tot++;
	}

	*output << endl;
	output->flush();
	int counter_col_before = 0;
	int counter_col_after = count_col_tot - 1;

	if (this->foreland.get_number_segments() > 0) {
		counter_col_after--;

		for (int i = 0; i < this->foreland.get_number_segments(); i++) {
			*output << this->foreland.get_segment(i)->point1.get_xcoordinate() << ",";
			functions::add_seperator_csv("NAN,", output, counter_col_before);
			*output << this->foreland.get_segment(i)->point1.get_ycoordinate();
			functions::add_seperator_csv(",NAN", output, counter_col_after);
			*output << endl;
		}

		*output << this->foreland.get_segment(this->foreland.get_number_segments() - 1)->point2.get_xcoordinate() << ",";
		functions::add_seperator_csv("NAN,", output, counter_col_before);
		*output << this->foreland.get_segment(this->foreland.get_number_segments() - 1)->point2.get_ycoordinate();
		functions::add_seperator_csv(",NAN", output, counter_col_after);
		*output << endl;
		counter_col_before++;
	}
	output->flush();

	if (this->waterside_cubature.get_number_segments() > 0) {
		counter_col_after--;

		for (int i = 0; i < this->waterside_cubature.get_number_segments(); i++) {
			*output << this->waterside_cubature.get_segment(i)->point1.get_xcoordinate() << ",";
			functions::add_seperator_csv("NAN,", output, counter_col_before);
			*output << this->waterside_cubature.get_segment(i)->point1.get_ycoordinate();
			functions::add_seperator_csv(",NAN", output, counter_col_after);
			*output << endl;
		}

		*output << waterside_cubature.get_segment(waterside_cubature.get_number_segments() - 1)->point2.get_xcoordinate() << ",";
		functions::add_seperator_csv("NAN,", output, counter_col_before);
		*output << this->waterside_cubature.get_segment(this->waterside_cubature.get_number_segments() - 1)->point2.get_ycoordinate();
		functions::add_seperator_csv(",NAN", output, counter_col_after);
		*output << endl;
		counter_col_before++;
	}
	output->flush();

	//crest
	if (this->crest_cubature.get_number_segments() > 0) {
		counter_col_after--;
		*output << this->crest_cubature.get_segment()->point1.get_xcoordinate() << ",";
		functions::add_seperator_csv("NAN,", output, counter_col_before);
		*output << this->crest_cubature.get_segment()->point1.get_ycoordinate();
		functions::add_seperator_csv(",NAN", output, counter_col_after);
		*output << endl;

		*output << this->crest_cubature.get_segment()->point2.get_xcoordinate() << ",";
		functions::add_seperator_csv("NAN,", output, counter_col_before);
		*output << this->crest_cubature.get_segment()->point2.get_ycoordinate();
		functions::add_seperator_csv(",NAN", output, counter_col_after);
		*output << endl;
		counter_col_before++;
	}
	output->flush();


	if (this->landside_cubature.get_number_segments() > 0) {
		counter_col_after--;

		for (int i = 0; i < this->landside_cubature.get_number_segments(); i++) {
			*output << this->landside_cubature.get_segment(i)->point1.get_xcoordinate() << ",";
			functions::add_seperator_csv("NAN,", output, counter_col_before);
			*output << this->landside_cubature.get_segment(i)->point1.get_ycoordinate();
			functions::add_seperator_csv(",NAN", output, counter_col_after);
			*output << endl;
		}

		*output << landside_cubature.get_segment(landside_cubature.get_number_segments() - 1)->point2.get_xcoordinate() << ",";
		functions::add_seperator_csv("NAN,", output, counter_col_before);
		*output << this->landside_cubature.get_segment(this->landside_cubature.get_number_segments() - 1)->point2.get_ycoordinate();
		functions::add_seperator_csv(",NAN", output, counter_col_after);
		*output << endl;
		counter_col_before++;
	}
	output->flush();

	if (this->hinterland.get_number_segments() > 0) {
		counter_col_after--;

		for (int i = 0; i < this->hinterland.get_number_segments(); i++) {
			*output << this->hinterland.get_segment(i)->point1.get_xcoordinate() << ",";
			functions::add_seperator_csv("NAN,", output, counter_col_before);
			*output << this->hinterland.get_segment(i)->point1.get_ycoordinate();
			functions::add_seperator_csv(",NAN", output, counter_col_after);
			*output << endl;
		}

		*output << this->hinterland.get_segment(this->hinterland.get_number_segments() - 1)->point2.get_xcoordinate() << ",";
		functions::add_seperator_csv("NAN,", output, counter_col_before);
		*output << this->hinterland.get_segment(this->hinterland.get_number_segments() - 1)->point2.get_ycoordinate();
		functions::add_seperator_csv(",NAN", output, counter_col_after);
		*output << endl;
		counter_col_before++;
	}
	output->flush();

}
//Output the geometry to excel
void Fpl_Sec_Type_Dune::output_geometry2excel(ofstream *output) {

	//output the zone header
	*output << P(4) << FORMAT_FIXED_REAL;
	//header
	*output << " x;";
	int count_col_tot = 1;
	if (this->foreland.get_number_segments() > 0) {
		*output << " foreland;";
		count_col_tot++;
	}
	if (this->waterside_cubature.get_number_segments() > 0) {
		*output << " waterside;";
		count_col_tot++;

	}
	if (this->crest_cubature.get_number_segments() > 0) {

		*output << " crest;";
		count_col_tot++;
	}

	if (this->landside_cubature.get_number_segments() > 0) {
		*output << " landside;";
		count_col_tot++;
	}
	if (this->hinterland.get_number_segments() > 0) {
		*output << " hinterland";
		count_col_tot++;
	}

	*output << endl;
	output->flush();
	int counter_col_before = 0;
	int counter_col_after = count_col_tot - 1;

	if (this->foreland.get_number_segments() > 0) {
		counter_col_after--;

		for (int i = 0; i < this->foreland.get_number_segments(); i++) {
			*output << this->foreland.get_segment(i)->point1.get_xcoordinate() << ";";
			functions::add_seperator_csv(";", output, counter_col_before);
			*output << this->foreland.get_segment(i)->point1.get_ycoordinate();
			functions::add_seperator_csv(";", output, counter_col_after);
			*output << endl;
		}

		*output << this->foreland.get_segment(this->foreland.get_number_segments() - 1)->point2.get_xcoordinate() << ";";
		functions::add_seperator_csv(";", output, counter_col_before);
		*output << this->foreland.get_segment(this->foreland.get_number_segments() - 1)->point2.get_ycoordinate();
		functions::add_seperator_csv(";", output, counter_col_after);
		*output << endl;
		counter_col_before++;
	}
	output->flush();

	if (this->waterside_cubature.get_number_segments() > 0) {
		counter_col_after--;

		for (int i = 0; i < this->waterside_cubature.get_number_segments(); i++) {
			*output << this->waterside_cubature.get_segment(i)->point1.get_xcoordinate() << ";";
			functions::add_seperator_csv(";", output, counter_col_before);
			*output << this->waterside_cubature.get_segment(i)->point1.get_ycoordinate();
			functions::add_seperator_csv(";", output, counter_col_after);
			*output << endl;
		}

		*output << waterside_cubature.get_segment(waterside_cubature.get_number_segments() - 1)->point2.get_xcoordinate() << ";";
		functions::add_seperator_csv(";", output, counter_col_before);
		*output << this->waterside_cubature.get_segment(this->waterside_cubature.get_number_segments() - 1)->point2.get_ycoordinate();
		functions::add_seperator_csv(";", output, counter_col_after);
		*output << endl;
		counter_col_before++;
	}
	output->flush();

	//crest
	if (this->crest_cubature.get_number_segments() > 0) {
		counter_col_after--;
		*output << this->crest_cubature.get_segment()->point1.get_xcoordinate() << ";";
		functions::add_seperator_csv(";", output, counter_col_before);
		*output << this->crest_cubature.get_segment()->point1.get_ycoordinate();
		functions::add_seperator_csv(";", output, counter_col_after);
		*output << endl;

		*output << this->crest_cubature.get_segment()->point2.get_xcoordinate() << ";";
		functions::add_seperator_csv(";", output, counter_col_before);
		*output << this->crest_cubature.get_segment()->point2.get_ycoordinate();
		functions::add_seperator_csv(";", output, counter_col_after);
		*output << endl;
		counter_col_before++;
	}
	output->flush();


	if (this->landside_cubature.get_number_segments() > 0) {
		counter_col_after--;

		for (int i = 0; i < this->landside_cubature.get_number_segments(); i++) {
			*output << this->landside_cubature.get_segment(i)->point1.get_xcoordinate() << ";";
			functions::add_seperator_csv(";", output, counter_col_before);
			*output << this->landside_cubature.get_segment(i)->point1.get_ycoordinate();
			functions::add_seperator_csv(";", output, counter_col_after);
			*output << endl;
		}

		*output << landside_cubature.get_segment(landside_cubature.get_number_segments() - 1)->point2.get_xcoordinate() << ";";
		functions::add_seperator_csv(";", output, counter_col_before);
		*output << this->landside_cubature.get_segment(this->landside_cubature.get_number_segments() - 1)->point2.get_ycoordinate();
		functions::add_seperator_csv(";", output, counter_col_after);
		*output << endl;
		counter_col_before++;
	}
	output->flush();

	if (this->hinterland.get_number_segments() > 0) {
		counter_col_after--;

		for (int i = 0; i < this->hinterland.get_number_segments(); i++) {
			*output << this->hinterland.get_segment(i)->point1.get_xcoordinate() << ";";
			functions::add_seperator_csv(";", output, counter_col_before);
			*output << this->hinterland.get_segment(i)->point1.get_ycoordinate();
			functions::add_seperator_csv(";", output, counter_col_after);
			*output << endl;
		}

		*output << this->hinterland.get_segment(this->hinterland.get_number_segments() - 1)->point2.get_xcoordinate() << ";";
		functions::add_seperator_csv(";", output, counter_col_before);
		*output << this->hinterland.get_segment(this->hinterland.get_number_segments() - 1)->point2.get_ycoordinate();
		functions::add_seperator_csv(";", output, counter_col_after);
		*output << endl;
		counter_col_before++;
	}
	output->flush();

}
//Output result members of the mechanisms to database table
void Fpl_Sec_Type_Dune::output_result2table(QSqlDatabase *ptr_database, _fpl_simulation_type simulation_type, _sys_system_id id, const int section_id, const int counter_mc_sim){
	try{
		//delete existing data and set table
		Fpl_Sec_Type_Dune::set_result_table(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}
	//mysql query with the query-class
	QSqlQuery model(*ptr_database);
	//evaluate the global identifier
	int id_glob=Fpl_Sec_Type_Dune::result_table->maximum_int_of_column(Fpl_Sec_Type_Dune::result_table->get_column_name(fpl_label::glob_id),ptr_database)+1;

	//set the query via a query string
	ostringstream query_string;
	query_string << "INSERT INTO  " << Fpl_Sec_Type_Dune::result_table->get_table_name();
	query_string <<" ( ";
	query_string << Fpl_Sec_Type_Dune::result_table->get_column_name(fpl_label::glob_id) <<" , ";
	query_string << Fpl_Sec_Type_Dune::result_table->get_column_name(fpl_label::section_id) <<" , ";
	query_string << Fpl_Sec_Type_Dune::result_table->get_column_name(label::areastate_id) <<" , ";
	query_string << Fpl_Sec_Type_Dune::result_table->get_column_name(label::measure_id) <<" , ";
	query_string << Fpl_Sec_Type_Dune::result_table->get_column_name(fpl_label::simulation_type) <<" , ";
	//data
	query_string << Fpl_Sec_Type_Dune::result_table->get_column_name(fpl_label::waterlevel) <<" , ";
	query_string << Fpl_Sec_Type_Dune::result_table->get_column_name(fpl_label::height) <<" , ";
	//wind wave
	query_string << Fpl_Sec_Type_Dune::result_table->get_column_name(fpl_label::wave_height) <<" , ";
	query_string << Fpl_Sec_Type_Dune::result_table->get_column_name(fpl_label::wave_length) <<" , ";
	query_string << Fpl_Sec_Type_Dune::result_table->get_column_name(fpl_label::wave_period) <<" , ";
	query_string << Fpl_Sec_Type_Dune::result_table->get_column_name(fpl_label::wave_setup) <<" , ";
	query_string << Fpl_Sec_Type_Dune::result_table->get_column_name(fpl_label::mean_depth) <<" , ";
	//overflow
	query_string << Fpl_Sec_Type_Dune::result_table->get_column_name(fpl_label::mue_factor) <<" , ";
	query_string << Fpl_Sec_Type_Dune::result_table->get_column_name(fpl_label::q_overflow) <<" , ";
	query_string << Fpl_Sec_Type_Dune::result_table->get_column_name(fpl_label::tree_prob_overflow) <<" , ";
	query_string << Fpl_Sec_Type_Dune::result_table->get_column_name(fpl_label::single_prob_overflow) <<" , ";
	//wave runup
	query_string << Fpl_Sec_Type_Dune::result_table->get_column_name(fpl_label::gamma_wavedirection) <<" , ";
	query_string << Fpl_Sec_Type_Dune::result_table->get_column_name(fpl_label::gamma_berms) <<" , ";
	query_string << Fpl_Sec_Type_Dune::result_table->get_column_name(fpl_label::mid_gradient_waterside) <<" , ";
	query_string << Fpl_Sec_Type_Dune::result_table->get_column_name(fpl_label::height_runup) <<" , ";
	query_string << Fpl_Sec_Type_Dune::result_table->get_column_name(fpl_label::q_waveoverflow) <<" , ";
	query_string << Fpl_Sec_Type_Dune::result_table->get_column_name(fpl_label::tree_prob_runup) <<" , ";
	query_string << Fpl_Sec_Type_Dune::result_table->get_column_name(fpl_label::single_prob_runup) <<" , ";
	//erosion landside
	query_string << Fpl_Sec_Type_Dune::result_table->get_column_name(fpl_label::crit_landside_discharge) <<" , ";
	//erosion waterside
	query_string << Fpl_Sec_Type_Dune::result_table->get_column_name(fpl_label::crit_crest_width) <<" , ";
	query_string << Fpl_Sec_Type_Dune::result_table->get_column_name(fpl_label::res_waterside_erosion) <<" , ";
	query_string << Fpl_Sec_Type_Dune::result_table->get_column_name(fpl_label::stress_waterside_erosion) <<" , ";
	query_string << Fpl_Sec_Type_Dune::result_table->get_column_name(fpl_label::tree_prob_water_ero) <<" , ";
	query_string << Fpl_Sec_Type_Dune::result_table->get_column_name(fpl_label::single_prob_water_ero) <<" ) ";

	query_string << " VALUES ( ";
	query_string << id_glob << " , " ;
	query_string << section_id << " , " ;
	query_string << id.area_state << " , " ;
	query_string << id.measure_nr<< " , " ;
	query_string << "'"<<Fpl_Mc_Sim::convert_sim_type2text(simulation_type)<<"'"<< " , " ;
	//data
	query_string << this->waterlevel.get_calculated_mean()<< " , " ;
	query_string << this->waterside_cubature.get_dike_height()<< " , " ;

	//wind wave generation
	if(this->wave_wind_event!=NULL && this->wave_wind_event->get_2calc_flag()==true){
		query_string << this->wave_wind_event->wave_calculator.wave_height.get_calculated_mean()<< " , " ;
		query_string << this->wave_wind_event->wave_calculator.wave_lenght.get_calculated_mean()<< " , " ;
		query_string << this->wave_wind_event->wave_calculator.wave_period.get_calculated_mean()<< " , " ;
		query_string << this->wave_wind_event->wave_calculator.wave_setup.get_calculated_mean()<< " , " ;
		query_string << this->wave_wind_event->wave_calculator.mean_depth.get_calculated_mean()<< " , " ;
	}
	else{
		query_string << 0.0 << " , " ;
		query_string << 0.0 << " , " ;
		query_string << 0.0 << " , " ;
		query_string << 0.0 << " , " ;
		query_string << 0.0 << " , " ;
	}
	//overflow event
	if(this->overflow_event!=NULL && this->overflow_event->get_2calc_flag()==true){
		query_string << this->overflow_event->mue_factor.get_calculated_mean()<< " , " ;
		query_string << this->overflow_event->q_overflow.get_calculated_mean()<< " , " ;
		query_string << this->overflow_event->get_counter_tree_prob()/(double)counter_mc_sim<< " , " ;
		query_string << this->overflow_event->get_counter_single_prob()/(double)counter_mc_sim<< " , " ;
	}
	else{
		query_string << 0.0 << " , " ;
		query_string << 0.0 << " , " ;
		query_string << 0.0 << " , " ;
		query_string << 0.0 << " , " ;
	}
	//wave runup
	if(this->wave_runup_event!=NULL && this->wave_runup_event->get_2calc_flag()==true){
		query_string << this->wave_runup_event->gamma_wavedirection.get_calculated_mean()<< " , " ;
		query_string << this->wave_runup_event->waterside_cubature->gamma_berm_total.get_calculated_mean()<< " , " ;
		query_string << this->wave_runup_event->mid_gradient_waterside.get_calculated_mean()<< " , " ;
		query_string << this->wave_runup_event->height_runup.get_calculated_mean()<< " , " ;
		query_string << this->wave_runup_event->discharge_overtopping.get_calculated_mean()<< " , " ;
		query_string << this->wave_runup_event->get_counter_tree_prob()/(double)counter_mc_sim<< " , " ;
		query_string << this->wave_runup_event->get_counter_single_prob()/(double)counter_mc_sim<< " , " ;
	}
	else{
		query_string << 0.0 << " , " ;
		query_string << 0.0 << " , " ;
		query_string << 0.0 << " , " ;
		query_string << 0.0 << " , " ;
		query_string << 0.0 << " , " ;
		query_string << 0.0 << " , " ;
		query_string << 0.0 << " , " ;
	}
	//erosion landside
	if(this->erosion_landside_event!=NULL && this->erosion_landside_event->get_2calc_flag()==true){
		query_string << this->erosion_landside_event->critical_discharge.get_calculated_mean()<< " , " ;
	}
	else{
		query_string << 0.0 << " , " ;
	}

	//water side erosion
	if(this->erosion_waterside_gent_event!=NULL && this->erosion_waterside_gent_event->get_2calc_flag()==true){
		//critical crest width (change it!)
		query_string << this->erosion_waterside_gent_event->critical_crest_width.get_calculated_mean()<< " , " ;
		query_string << this->erosion_waterside_gent_event->resistance.get_calculated_mean()<< " , " ;
		query_string << this->erosion_waterside_gent_event->stress.get_calculated_mean()<< " , " ;
		query_string << this->erosion_waterside_gent_event->get_counter_tree_prob()/(double)counter_mc_sim<< " , " ;
		query_string << this->erosion_waterside_gent_event->get_counter_single_prob()/(double)counter_mc_sim<< " ) " ;
	}
	else{
		query_string << 0.0 << " , " ;
		query_string << 0.0 << " , " ;
		query_string << 0.0 << " , " ;
		query_string << 0.0 << " , " ;
		query_string << 0.0 << " ) " ;
	}

	Data_Base::database_request(&model, query_string.str(), ptr_database);

	if(model.lastError().isValid()){
		Warning msg=this->set_warning(3);
		ostringstream info;
		info << "Table Name                : " << Fpl_Sec_Type_Dune::result_table->get_table_name() << endl;
		info << "Table error info          : " << model.lastError().text().toStdString() << endl;
		info << "Fpl section id            : " << section_id << endl;
		msg.make_second_info(info.str());
		msg.output_msg(1);
	}
}
//Output the deterministic results to tecplot
void Fpl_Sec_Type_Dune::output_determ_res2tecplot(ofstream *output, QSqlDatabase *ptr_database, _sys_system_id id, const int section_id) {
	//header
	*output << "VARIABLES = " << endl;




	QSqlQueryModel model;
	int number = 0;
	number = Fpl_Sec_Type_Dune::select_results_in_database(&model, ptr_database, id, section_id, _fpl_simulation_type::sim_determ);

	for (int j = 6; j < model.columnCount(); j++) {
		*output << "\"" << (model.headerData(j, Qt::Horizontal, Qt::DisplayRole)).toString().toStdString() << "\"     " << endl;

	}
	*output << endl << endl << endl;

	//results
	for (int row = 0; row < model.rowCount(); row++) {
		for (int j = 6; j < model.columnCount(); j++) {
			*output << (model.data(model.index(row, j), Qt::DisplayRole)).toString().toStdString();
			*output << W(5) << "  \t  ";
		}
		*output << endl;

	}

}
//Output the deterministic results to Paraview
void Fpl_Sec_Type_Dune::output_determ_res2paraview(ofstream *output, QSqlDatabase *ptr_database, _sys_system_id id, const int section_id) {
	//header

	//results
	QSqlQueryModel model;
	int number = 0;
	number = Fpl_Sec_Type_Dune::select_results_in_database(&model, ptr_database, id, section_id, _fpl_simulation_type::sim_determ);

	for (int j = 6; j < model.columnCount(); j++) {
		*output << (model.headerData(j, Qt::Horizontal, Qt::DisplayRole)).toString().toStdString() << ",";

	}
	*output << endl;

	//results
	for (int row = 0; row < model.rowCount(); row++) {
		for (int j = 6; j < model.columnCount(); j++) {
			*output << (model.data(model.index(row, j), Qt::DisplayRole)).toString().toStdString() << ",";
		}
		*output << endl;

	}


}
//Output the deterministic results to Excel
void Fpl_Sec_Type_Dune::output_determ_res2excel(ofstream *output, QSqlDatabase *ptr_database, _sys_system_id id, const int section_id) {
	//header


	//results
	QSqlQueryModel model;
	int number = 0;
	number = Fpl_Sec_Type_Dune::select_results_in_database(&model, ptr_database, id, section_id, _fpl_simulation_type::sim_determ);

	for (int j = 6; j < model.columnCount(); j++) {
		*output << (model.headerData(j, Qt::Horizontal, Qt::DisplayRole)).toString().toStdString() << ";";

	}
	*output << endl;

	//results
	for (int row = 0; row < model.rowCount(); row++) {
		for (int j = 6; j < model.columnCount(); j++) {
			*output << (model.data(model.index(row, j), Qt::DisplayRole)).toString().toStdString() << ";";
		}
		*output << endl;

	}


}
//Output the MC results to tecplot
void Fpl_Sec_Type_Dune::output_mc_res2tecplot(ofstream *output, QSqlDatabase *ptr_database, _sys_system_id id, const int section_id) {
	//header
	*output << "VARIABLES = " << endl;

	QSqlQueryModel model1;
	int number1 = 0;
	number1 = Fpl_Mc_Sim::select_results_in_database(&model1, ptr_database, id, section_id, true);
	for (int j = 6; j < model1.columnCount(); j++) {
		*output << "\"" << (model1.headerData(j, Qt::Horizontal, Qt::DisplayRole)).toString().toStdString() << "\"     " << endl;

	}


	QSqlQueryModel model;
	int number = 0;
	number = Fpl_Sec_Type_Dune::select_results_in_database(&model, ptr_database, id, section_id, _fpl_simulation_type::sim_mc);

	for (int j = 7; j < model.columnCount(); j++) {
		*output << "\"" << (model.headerData(j, Qt::Horizontal, Qt::DisplayRole)).toString().toStdString() << "\"     " << endl;

	}


	*output << endl << endl << endl;

	//results
	for (int row = 0; row < model1.rowCount(); row++) {
		for (int j = 6; j < model1.columnCount(); j++) {
			*output << (model1.data(model.index(row, j), Qt::DisplayRole)).toString().toStdString();
			*output << W(5) << "  \t  ";
		}
		for (int j = 7; j < model.columnCount(); j++) {
			*output << (model.data(model.index(row, j), Qt::DisplayRole)).toString().toStdString();
			*output << W(5) << "  \t  ";
		}
		*output << endl;

	}
}
//Output the MC results to Paraview
void Fpl_Sec_Type_Dune::output_mc_res2paraview(ofstream *output, QSqlDatabase *ptr_database, _sys_system_id id, const int section_id) {
	QSqlQueryModel model1;
	int number1 = 0;
	number1 = Fpl_Mc_Sim::select_results_in_database(&model1, ptr_database, id, section_id, true);
	for (int j = 6; j < model1.columnCount(); j++) {
		*output << (model1.headerData(j, Qt::Horizontal, Qt::DisplayRole)).toString().toStdString();
		if (j < model1.columnCount()) { *output << ","; };

	}

	//results
	QSqlQueryModel model;
	int number = 0;
	number = Fpl_Sec_Type_Dune::select_results_in_database(&model, ptr_database, id, section_id, _fpl_simulation_type::sim_mc);

	for (int j = 7; j < model.columnCount(); j++) {
		*output << (model.headerData(j, Qt::Horizontal, Qt::DisplayRole)).toString().toStdString();
		if (j < model.columnCount() - 1) { *output << ","; };

	}
	*output << endl;

	//results
	for (int row = 0; row < model.rowCount(); row++) {
		for (int j = 6; j < model1.columnCount(); j++) {
			*output << (model1.data(model.index(row, j), Qt::DisplayRole)).toString().toStdString();
			if (j < model1.columnCount()) { *output << ","; };
		}
		for (int j = 7; j < model.columnCount(); j++) {
			*output << (model.data(model.index(row, j), Qt::DisplayRole)).toString().toStdString();
			if (j < model.columnCount() - 1) { *output << ","; };
		}
		*output << endl;

	}
}
//Output the MC results to Excel
void Fpl_Sec_Type_Dune::output_mc_res2excel(ofstream *output, QSqlDatabase *ptr_database, _sys_system_id id, const int section_id) {
	QSqlQueryModel model1;
	int number1 = 0;
	number1 = Fpl_Mc_Sim::select_results_in_database(&model1, ptr_database, id, section_id, true);
	for (int j = 6; j < model1.columnCount(); j++) {
		*output << (model1.headerData(j, Qt::Horizontal, Qt::DisplayRole)).toString().toStdString();
		if (j < model1.columnCount()) { *output << ";"; };

	}

	//results
	QSqlQueryModel model;
	int number = 0;
	number = Fpl_Sec_Type_Dune::select_results_in_database(&model, ptr_database, id, section_id, _fpl_simulation_type::sim_mc);

	for (int j = 7; j < model.columnCount(); j++) {
		*output << (model.headerData(j, Qt::Horizontal, Qt::DisplayRole)).toString().toStdString();
		if (j < model.columnCount() - 1) { *output << ";"; };

	}
	*output << endl;

	//results
	for (int row = 0; row < model.rowCount(); row++) {
		for (int j = 6; j < model1.columnCount(); j++) {
			*output << (model1.data(model.index(row, j), Qt::DisplayRole)).toString().toStdString();
			if (j < model1.columnCount()) { *output << ";"; };
		}
		for (int j = 7; j < model.columnCount(); j++) {
			*output << (model.data(model.index(row, j), Qt::DisplayRole)).toString().toStdString();
			if (j < model.columnCount() - 1) { *output << ";"; };
		}
		*output << endl;

	}
}
//Output the FRC results to tecplot
void Fpl_Sec_Type_Dune::output_frc_res2tecplot(ofstream *output, QSqlDatabase *ptr_database, _sys_system_id id, const int section_id) {
	//header
	*output << "VARIABLES = " << endl;

	QSqlQueryModel model1;
	int number1 = 0;
	number1 = Fpl_Mc_Sim::select_results_in_database(&model1, ptr_database, id, section_id, false);
	for (int j = 6; j < model1.columnCount(); j++) {
		*output << "\"" << (model1.headerData(j, Qt::Horizontal, Qt::DisplayRole)).toString().toStdString() << "\"     " << endl;

	}


	QSqlQueryModel model;
	int number = 0;
	number = Fpl_Sec_Type_Dune::select_results_in_database(&model, ptr_database, id, section_id, _fpl_simulation_type::sim_frc);

	for (int j = 7; j < model.columnCount(); j++) {
		*output << "\"" << (model.headerData(j, Qt::Horizontal, Qt::DisplayRole)).toString().toStdString() << "\"     " << endl;

	}


	*output << endl << endl << endl;

	//results
	for (int row = 0; row < model1.rowCount(); row++) {
		for (int j = 6; j < model1.columnCount(); j++) {
			*output << (model1.data(model.index(row, j), Qt::DisplayRole)).toString().toStdString();
			*output << W(5) << "  \t  ";
		}
		for (int j = 7; j < model.columnCount(); j++) {
			*output << (model.data(model.index(row, j), Qt::DisplayRole)).toString().toStdString();
			*output << W(5) << "  \t  ";
		}
		*output << endl;
		output->flush();

	}
}
//Output the FRC results to Paraview
void Fpl_Sec_Type_Dune::output_frc_res2paraview(ofstream *output, QSqlDatabase *ptr_database, _sys_system_id id, const int section_id) {
	QSqlQueryModel model1;
	int number1 = 0;
	number1 = Fpl_Mc_Sim::select_results_in_database(&model1, ptr_database, id, section_id, false);
	for (int j = 6; j < model1.columnCount(); j++) {
		*output << (model1.headerData(j, Qt::Horizontal, Qt::DisplayRole)).toString().toStdString();
		if (j < model1.columnCount()) { *output << ","; };

	}

	//results
	QSqlQueryModel model;
	int number = 0;
	number = Fpl_Sec_Type_Dune::select_results_in_database(&model, ptr_database, id, section_id, _fpl_simulation_type::sim_frc);

	for (int j = 7; j < model.columnCount(); j++) {
		*output << (model.headerData(j, Qt::Horizontal, Qt::DisplayRole)).toString().toStdString();
		if (j < model.columnCount() - 1) { *output << ","; };

	}
	*output << endl;

	//results
	for (int row = 0; row < model.rowCount(); row++) {
		for (int j = 6; j < model1.columnCount(); j++) {
			*output << (model1.data(model.index(row, j), Qt::DisplayRole)).toString().toStdString();
			if (j < model1.columnCount()) { *output << ","; };
		}
		for (int j = 7; j < model.columnCount(); j++) {
			*output << (model.data(model.index(row, j), Qt::DisplayRole)).toString().toStdString();
			if (j < model.columnCount() - 1) { *output << ","; };
		}
		*output << endl;
		output->flush();

	}
}
//Output the FRC results to Excel
void Fpl_Sec_Type_Dune::output_frc_res2excel(ofstream *output, QSqlDatabase *ptr_database, _sys_system_id id, const int section_id) {
	QSqlQueryModel model1;
	int number1 = 0;
	number1 = Fpl_Mc_Sim::select_results_in_database(&model1, ptr_database, id, section_id, false);
	for (int j = 6; j < model1.columnCount(); j++) {
		*output << (model1.headerData(j, Qt::Horizontal, Qt::DisplayRole)).toString().toStdString();
		if (j < model1.columnCount()) { *output << ";"; };

	}

	//results
	QSqlQueryModel model;
	int number = 0;
	number = Fpl_Sec_Type_Dune::select_results_in_database(&model, ptr_database, id, section_id, _fpl_simulation_type::sim_frc);

	for (int j = 7; j < model.columnCount(); j++) {
		*output << (model.headerData(j, Qt::Horizontal, Qt::DisplayRole)).toString().toStdString();
		if (j < model.columnCount() - 1) { *output << ";"; };

	}
	*output << endl;

	//results
	for (int row = 0; row < model.rowCount(); row++) {
		for (int j = 6; j < model1.columnCount(); j++) {
			*output << (model1.data(model.index(row, j), Qt::DisplayRole)).toString().toStdString();
			if (j < model1.columnCount()) { *output << ";"; };
		}
		for (int j = 7; j < model.columnCount(); j++) {
			*output << (model.data(model.index(row, j), Qt::DisplayRole)).toString().toStdString();
			if (j < model.columnCount() - 1) { *output << ";"; };
		}
		*output << endl;
		output->flush();

	}
}
//Create the database table for the results of the mechanism of the dune section (static)
void Fpl_Sec_Type_Dune::create_result_table(QSqlDatabase *ptr_database){
		if(Fpl_Sec_Type_Dune::result_table==NULL){
			ostringstream cout;
			cout << "Create dune result database table..." << endl ;
			Sys_Common_Output::output_fpl->output_txt(&cout);
			//make specific input for this class
			const string tab_name=fpl_label::tab_fpl_dune_result;
			const int num_col=30;
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

			tab_col[5].name=fpl_label::simulation_type;
			tab_col[5].type=sys_label::tab_col_type_string;
			tab_col[5].key_flag=true;

			tab_col[6].name=fpl_label::waterlevel;
			tab_col[6].type=sys_label::tab_col_type_double;
			tab_col[6].unsigned_flag=true;

			tab_col[7].name=fpl_label::height;
			tab_col[7].type=sys_label::tab_col_type_double;
			tab_col[7].unsigned_flag=true;

			tab_col[8].name=fpl_label::wave_height;
			tab_col[8].type=sys_label::tab_col_type_double;
			tab_col[8].unsigned_flag=true;

			tab_col[9].name=fpl_label::wave_length;
			tab_col[9].type=sys_label::tab_col_type_double;
			tab_col[9].unsigned_flag=true;

			tab_col[10].name=fpl_label::wave_period;
			tab_col[10].type=sys_label::tab_col_type_double;
			tab_col[10].unsigned_flag=true;

			tab_col[11].name=fpl_label::wave_setup;
			tab_col[11].type=sys_label::tab_col_type_double;
			tab_col[11].unsigned_flag=true;

			tab_col[12].name=fpl_label::mean_depth;
			tab_col[12].type=sys_label::tab_col_type_double;
			tab_col[12].unsigned_flag=true;

			tab_col[13].name=fpl_label::q_overflow;
			tab_col[13].type=sys_label::tab_col_type_double;
			tab_col[13].unsigned_flag=true;

			tab_col[14].name=fpl_label::mue_factor;
			tab_col[14].type=sys_label::tab_col_type_double;
			tab_col[14].unsigned_flag=true;

			tab_col[15].name=fpl_label::gamma_wavedirection;
			tab_col[15].type=sys_label::tab_col_type_double;
			tab_col[15].unsigned_flag=true;

			tab_col[16].name=fpl_label::gamma_berms;
			tab_col[16].type=sys_label::tab_col_type_double;
			tab_col[16].unsigned_flag=true;

			tab_col[17].name=fpl_label::mid_gradient_waterside;
			tab_col[17].type=sys_label::tab_col_type_double;
			tab_col[17].unsigned_flag=true;

			tab_col[18].name=fpl_label::height_runup;
			tab_col[18].type=sys_label::tab_col_type_double;
			tab_col[18].unsigned_flag=true;

			tab_col[19].name=fpl_label::q_waveoverflow;
			tab_col[19].type=sys_label::tab_col_type_double;
			tab_col[19].unsigned_flag=true;

			tab_col[20].name=fpl_label::crit_landside_discharge;
			tab_col[20].type=sys_label::tab_col_type_double;
			tab_col[20].unsigned_flag=true;

			tab_col[21].name=fpl_label::crit_crest_width;
			tab_col[21].type=sys_label::tab_col_type_double;
			tab_col[21].unsigned_flag=true;

			tab_col[22].name=fpl_label::res_waterside_erosion;
			tab_col[22].type=sys_label::tab_col_type_double;
			tab_col[22].unsigned_flag=true;

			tab_col[23].name=fpl_label::stress_waterside_erosion;
			tab_col[23].type=sys_label::tab_col_type_double;
			tab_col[23].unsigned_flag=true;

			tab_col[24].name=fpl_label::single_prob_overflow;
			tab_col[24].type=sys_label::tab_col_type_double;
			tab_col[24].unsigned_flag=true;

			tab_col[25].name=fpl_label::tree_prob_overflow;
			tab_col[25].type=sys_label::tab_col_type_double;
			tab_col[25].unsigned_flag=true;

			tab_col[26].name=fpl_label::single_prob_runup;
			tab_col[26].type=sys_label::tab_col_type_double;
			tab_col[26].unsigned_flag=true;

			tab_col[27].name=fpl_label::tree_prob_runup;
			tab_col[27].type=sys_label::tab_col_type_double;
			tab_col[27].unsigned_flag=true;

			tab_col[28].name=fpl_label::single_prob_water_ero;
			tab_col[28].type=sys_label::tab_col_type_double;
			tab_col[28].unsigned_flag=true;

			tab_col[29].name=fpl_label::tree_prob_water_ero;
			tab_col[29].type=sys_label::tab_col_type_double;
			tab_col[29].unsigned_flag=true;

			try{
				Fpl_Sec_Type_Dune::result_table= new Tables();
				if(Fpl_Sec_Type_Dune::result_table->create_non_existing_tables(tab_name, tab_col, num_col,ptr_database, _sys_table_type::fpl)==false){
					cout << " Table exists" << endl ;
					Sys_Common_Output::output_fpl->output_txt(&cout);
				};
			}
			catch(bad_alloc& t){
				Error msg;
				msg.set_msg("Fpl_Sec_Type_Dune::create_result_table(QSqlDatabase *ptr_database)","Can not allocate the memory", "Check the memory", 10, false);
				ostringstream info;
				info<< "Info bad alloc: " << t.what() << endl;
				msg.make_second_info(info.str());
				throw msg;
			}
			catch(Error msg){
				Fpl_Sec_Type_Dune::close_result_table();
				throw msg;
			}

		Fpl_Sec_Type_Dune::close_result_table();
	}
}
//Delete all data in the database table for the results of the mechanism of the dune section  (static)
void Fpl_Sec_Type_Dune::delete_data_in_result_table(QSqlDatabase *ptr_database){
	//the table is set (the name and the column names) and allocated
	try{
		Fpl_Sec_Type_Dune::set_result_table(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}
	//delete the table
	Fpl_Sec_Type_Dune::result_table->delete_data_in_table(ptr_database);
}
//Delete a data set in the database table for the results of the mechanism of the dune section for a specific system state (static)
void Fpl_Sec_Type_Dune::delete_data_in_result_table(QSqlDatabase *ptr_database, const _sys_system_id id){
		//the table is set (the name and the column names) and allocated
	try{
		Fpl_Sec_Type_Dune::set_result_table(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}

	//delete the table
	QSqlQuery query(*ptr_database);
	ostringstream query_string;
	query_string <<"DELETE FROM " << Fpl_Sec_Type_Dune::result_table->get_table_name();
	query_string << " WHERE " ;
	query_string << Fpl_Sec_Type_Dune::result_table->get_column_name(label::areastate_id) << " ="  << id.area_state;
	query_string << " AND ";
	query_string << Fpl_Sec_Type_Dune::result_table->get_column_name(label::measure_id) << " = " << id.measure_nr;

	Data_Base::database_request(&query, query_string.str(), ptr_database);
	if(query.lastError().isValid()==true){
		Error msg;
		msg.set_msg("Fpl_Sec_Type_Dune::delete_data_in_result_table(QSqlDatabase *ptr_database,  const _sys_system_id id)","Invalid database request", "Check the database", 2, false);
		ostringstream info;
		info << "Table Name      : " << Fpl_Sec_Type_Dune::result_table->get_table_name() << endl;
		info << "Table error info: " << query.lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
}
//Delete a data set in the database table for the results of the mechanism of the dunde section (static)
void Fpl_Sec_Type_Dune::delete_data_in_result_table(QSqlDatabase *ptr_database, const int section_id, const _sys_system_id id, const _fpl_simulation_type sim_type){
		//the table is set (the name and the column names) and allocated
	try{
		Fpl_Sec_Type_Dune::set_result_table(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}
	//delete the table
	QSqlQuery query(*ptr_database);
	ostringstream query_string;
	query_string <<"DELETE FROM " << Fpl_Sec_Type_Dune::result_table->get_table_name();
	query_string << " WHERE " ;
	query_string << Fpl_Sec_Type_Dune::result_table->get_column_name(label::areastate_id) << " ="  << id.area_state;
	query_string << " AND ";
	query_string << Fpl_Sec_Type_Dune::result_table->get_column_name(label::measure_id) << " = " << id.measure_nr;
	query_string << " AND ";
	query_string << Fpl_Sec_Type_Dune::result_table->get_column_name(fpl_label::section_id) << " = " << section_id;
	query_string << " AND ";
	query_string << Fpl_Sec_Type_Dune::result_table->get_column_name(fpl_label::simulation_type) << " = '" << Fpl_Mc_Sim::convert_sim_type2text(sim_type)<<"'";

	Data_Base::database_request(&query, query_string.str(), ptr_database);
	if(query.lastError().isValid()==true){
		Error msg;
		msg.set_msg("Fpl_Sec_Type_Dune::delete_data_in_result_table(QSqlDatabase *ptr_database, const int section_id, const _sys_system_id id, const _fpl_simulation_type sim_type)","Invalid database request", "Check the database", 2, false);
		ostringstream info;
		info << "Table Name      : " << Fpl_Sec_Type_Dune::result_table->get_table_name() << endl;
		info << "Table error info: " << query.lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
}
//Set the database table for the results of the mechanism of the dune section : it sets the table name and the name of the columns and allocate them (static)
void Fpl_Sec_Type_Dune::set_result_table(QSqlDatabase *ptr_database, const bool not_close){
	if(Fpl_Sec_Type_Dune::result_table==NULL){
		//make specific input for this class
		const string tab_id_name=fpl_label::tab_fpl_dune_result;
		string tab_col[30];

		tab_col[0]=fpl_label::glob_id;
		tab_col[1]=fpl_label::section_id;
		tab_col[2]=label::areastate_id;
		tab_col[3]=label::measure_id;
		tab_col[4]=fpl_label::simulation_type;
		tab_col[5]=fpl_label::waterlevel;
		tab_col[6]=fpl_label::height;
		tab_col[7]=fpl_label::wave_height;
		tab_col[8]=fpl_label::wave_length;
		tab_col[9]=fpl_label::wave_period;
		tab_col[10]=fpl_label::wave_setup;
		tab_col[11]=fpl_label::mean_depth;
		tab_col[12]=fpl_label::q_overflow;
		tab_col[13]=fpl_label::mue_factor;
		tab_col[14]=fpl_label::gamma_wavedirection;
		tab_col[15]=fpl_label::gamma_berms;
		tab_col[16]=fpl_label::mid_gradient_waterside;
		tab_col[17]=fpl_label::height_runup;
		tab_col[18]=fpl_label::q_waveoverflow;
		tab_col[19]=fpl_label::res_waterside_erosion;
		tab_col[20]=fpl_label::stress_waterside_erosion;
		tab_col[21]=label::applied_flag;
		tab_col[22]=fpl_label::single_prob_overflow;
		tab_col[23]=fpl_label::tree_prob_overflow;
		tab_col[24]=fpl_label::single_prob_runup;
		tab_col[25]=fpl_label::tree_prob_runup;
		tab_col[26]=fpl_label::single_prob_water_ero;
		tab_col[27]=fpl_label::tree_prob_water_ero;
		tab_col[28]=fpl_label::crit_landside_discharge;
		tab_col[29]=fpl_label::crit_crest_width;

		try{
			Fpl_Sec_Type_Dune::result_table= new Tables(tab_id_name, tab_col, sizeof(tab_col)/sizeof(tab_col[0]));
			Fpl_Sec_Type_Dune::result_table->set_name(ptr_database, _sys_table_type::fpl);
		}
		catch(bad_alloc&t ){
			Error msg;
			msg.set_msg("void Fpl_Sec_Type_Dune::set_result_table(QSqlDatabase *ptr_database)","Can not allocate the memory", "Check the memory", 10, false);
			ostringstream info;
			info<< "Info bad alloc: " << t.what() << endl;
			msg.make_second_info(info.str());
			throw msg;
		}
		catch(Error msg){
			if(not_close==false){
				Fpl_Sec_Type_Dune::close_result_table();
			}
			throw msg;
		}
	}
}
//Select results of given fpl section in a database table (static)
int Fpl_Sec_Type_Dune::select_results_in_database(QSqlQueryModel *results, QSqlDatabase *ptr_database, const _sys_system_id id, const int sec_id, const _fpl_simulation_type sim_type) {
	int number = 0;
	try {
		Fpl_Sec_Type_Dune::set_result_table(ptr_database);
	}
	catch (Error msg) {
		throw msg;
	}
	ostringstream test_filter;
	test_filter << "SELECT *";
	test_filter << " FROM " << Fpl_Sec_Type_Dune::result_table->get_table_name();
	test_filter << " WHERE ";
	test_filter << Fpl_Sec_Type_Dune::result_table->get_column_name(fpl_label::section_id) << " = " << sec_id;
	test_filter << " AND ";
	test_filter << Fpl_Sec_Type_Dune::result_table->get_column_name(label::applied_flag) << "= true";
	test_filter << " AND ";
	test_filter << Fpl_Sec_Type_Dune::result_table->get_column_name(label::areastate_id) << " =" << id.area_state;
	test_filter << " AND (";
	test_filter << Fpl_Sec_Type_Dune::result_table->get_column_name(label::measure_id) << " = " << 0;
	test_filter << " OR ";
	test_filter << Fpl_Sec_Type_Dune::result_table->get_column_name(label::measure_id) << " = " << id.measure_nr;
	test_filter << " ) ";
	test_filter << " AND ";
	test_filter << Fpl_Sec_Type_Dune::result_table->get_column_name(fpl_label::simulation_type) << " = '" << Fpl_Mc_Sim::convert_sim_type2text(sim_type) << "'";
	test_filter << " ORDER BY " << Fpl_Sec_Type_Dune::result_table->get_column_name(fpl_label::waterlevel);

	Data_Base::database_request(results, test_filter.str(), ptr_database);

	//check the request
	if (results->lastError().isValid()) {
		Warning msg;
		msg.set_msg("Fpl_Sec_Type_Dune::select_results_in_database(QSqlQueryModel *results, QSqlDatabase *ptr_database, const _sys_system_id id, const int sec_id, const _fpl_simulation_type sim_type)", "Invalid database request", "Check the database", "", 2);
		ostringstream info;
		info << "Table Name      : " << Fpl_Sec_Type_Dune::result_table->get_table_name() << endl;
		info << "Table error info: " << results->lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		msg.output_msg(1);
	}
	number = results->rowCount();

	return number;

}
//Close and delete the database table for the results of the mechanism of the dune section  (static)
void Fpl_Sec_Type_Dune::close_result_table(void){
	if(Fpl_Sec_Type_Dune::result_table!=NULL){
		delete Fpl_Sec_Type_Dune::result_table;
		Fpl_Sec_Type_Dune::result_table=NULL;
	}
}
//Switch the applied-flag for the results of the mechanism of the dune section in the database table for a defined system state (static)
void Fpl_Sec_Type_Dune::switch_applied_flag_result_table(QSqlDatabase *ptr_database, const _sys_system_id id, const bool flag){
	//the table is set (the name and the column names) and allocated
	try{
		Fpl_Sec_Type_Dune::set_result_table(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}
	QSqlQueryModel query;

	ostringstream query_string;
	query_string <<"UPDATE ";
	query_string << Fpl_Sec_Type_Dune::result_table->get_table_name();
	query_string << " SET " ;
	query_string  << Fpl_Sec_Type_Dune::result_table->get_column_name(label::applied_flag) << " = "<< functions::convert_boolean2string(flag);
	query_string  << " WHERE ";
	query_string  << Fpl_Sec_Type_Dune::result_table->get_column_name(label::areastate_id) << " = "  << id.area_state;
	query_string  << " AND ";
	query_string  <<  Fpl_Sec_Type_Dune::result_table->get_column_name(label::measure_id) << " = " << id.measure_nr ;

	Data_Base::database_request(&query, query_string.str(), ptr_database);
	if(query.lastError().isValid()==true){
		Error msg;
		msg.set_msg("Fpl_Sec_Type_Dune::switch_applied_flag_result_table(QSqlDatabase *ptr_database, const _sys_system_id id, const bool flag)","Invalid database request", "Check the database", 2, false);
		ostringstream info;
		info << "Table Name      : " << Fpl_Sec_Type_Dune::result_table->get_table_name() << endl;
		info << "Table error info: " << query.lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
}
//Switch the applied-flag for the results of the mechanism of the dune section in the database table for a defined system state and id (static)
void Fpl_Sec_Type_Dune::switch_applied_flag_result_table(QSqlDatabase *ptr_database, const _sys_system_id id, const bool flag, const int sec_id){
		//the table is set (the name and the column names) and allocated
	try{
		Fpl_Sec_Type_Dune::set_result_table(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}
	QSqlQueryModel query;

	ostringstream query_string;
	query_string <<"UPDATE ";
	query_string << Fpl_Sec_Type_Dune::result_table->get_table_name();
	query_string << " SET " ;
	query_string  << Fpl_Sec_Type_Dune::result_table->get_column_name(label::applied_flag) << " = "<< functions::convert_boolean2string(flag);
	query_string  << " WHERE ";
	query_string  << Fpl_Sec_Type_Dune::result_table->get_column_name(label::areastate_id) << " = "  << id.area_state;
	query_string  << " AND ";
	query_string  <<  Fpl_Sec_Type_Dune::result_table->get_column_name(label::measure_id) << " = " << id.measure_nr ;
	query_string  << " AND ";
	query_string  <<  Fpl_Sec_Type_Dune::result_table->get_column_name(fpl_label::section_id) << " = " << sec_id ;

	Data_Base::database_request(&query, query_string.str(), ptr_database);
	if(query.lastError().isValid()==true){
		Error msg;
		msg.set_msg("Fpl_Sec_Type_Dune::switch_applied_flag_result_table(QSqlDatabase *ptr_database, const _sys_system_id id, const bool flag, const int sec_id)","Invalid database request", "Check the database", 2, false);
		ostringstream info;
		info << "Table Name      : " << Fpl_Sec_Type_Dune::result_table->get_table_name() << endl;
		info << "Table error info: " << query.lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
}
//Check and set the section geometry out of the read in geometry members
void Fpl_Sec_Type_Dune::check_set_geometry(void){
	//check if a foreland is set
	if(this->foreland.get_number_segments()==0){
		Error msg=this->set_error(12);
		throw msg;
	}

	//check foreland gradient
	double buff_grad=this->foreland.get_tot_vert_length()/this->foreland.get_tot_hori_length();
	if(buff_grad>0.08){
		//warning
		Warning msg=this->set_warning(4);
		ostringstream info;
		info << " Gradient foreland : " << buff_grad << label::no_unit <<endl;
		msg.make_second_info(info.str());
		msg.output_msg(1);

	}

	//make the polygon and the polysegment of the outer boundary
	try{
		this->make_geometrical_outer_dune_polygon();
	}
	catch(Error msg){
		ostringstream info;
		info<<"Try to set-up the outer dune polygon"<< endl;
		msg.make_second_info(info.str());
		throw msg;
	}
	try{
		this->make_geometrical_outer_dune_polysegment();
	}
	catch(Error msg){
		ostringstream info;
		info<<"Try to set-up the outer dune polysegement"<< endl;
		msg.make_second_info(info.str());
		throw msg;
	}

	this->calc_area_cubature();

	//wind wave generation
	if(this->wave_wind_event!=NULL){
		if(this->wave_wind_event->get_2calc_flag()==true){
			this->wave_wind_event->set_ptr_foreland(&this->foreland);
		}
	}

	Geo_Point crest_mid=this->crest_cubature.get_segment()->calc_mid_point();

	//check the mid-gradient of the waterside cubature of the dune
	double grad=this->waterside_cubature.calculate_mid_gradient();
	this->check_dune_gradients(grad, true);

	//check the mid-gradient of the landside cubature of the dune
	grad=this->landside_cubature.calculate_mid_gradient();
	this->check_dune_gradients(grad, false);
}
//_______________
//private
//Allocate the mechanism for wave generation induced by wind
void Fpl_Sec_Type_Dune::allocate_wind_wave_mechanism(void){
	if(this->wave_wind_event!=NULL){
		return;
	}
	try{
		this->wave_wind_event=new Fpl_Mech_Wave_Generation();
	}
	catch(bad_alloc &t){
		Error msg=this->set_error(2);
		ostringstream info;
		info<< "Info bad alloc: " << t.what() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
}
//Delete the mechanism for wave generation induced by wind
void Fpl_Sec_Type_Dune::delete_wind_wave_mechanism(void){
	if(this->wave_wind_event!=NULL){
		delete this->wave_wind_event;
		this->wave_wind_event=NULL;
	}
}
//Allocate the mechanism for wave forced runup
void Fpl_Sec_Type_Dune::allocate_wave_runup_mechanism(void){
	if(this->wave_runup_event!=NULL){
		return;
	}
	try{
		this->wave_runup_event=new Fpl_Mech_Waverunup_Dune();
		this->wave_runup_event->set_waterside_cubature(&this->waterside_cubature);
	}
	catch(bad_alloc &t){
		Error msg=this->set_error(3);
		ostringstream info;
		info<< "Info bad alloc: " << t.what() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
}
//Delete the mechanism for wave forced runup
void Fpl_Sec_Type_Dune::delete_wave_runup_mechanism(void){
	if(this->wave_runup_event!=NULL){
		delete this->wave_runup_event;
		this->wave_runup_event=NULL;
	}
}
//Allocate the mechanism for an overflow after modified Poleni-formula
void Fpl_Sec_Type_Dune::allocate_overflow_mechanism(void){
	if(this->overflow_event!=NULL){
		return;
	}

	try{
		this->overflow_event=new Fpl_Mech_Overflow_Dike();
	}
	catch(bad_alloc &t){
		Error msg=this->set_error(4);
		ostringstream info;
		info<< "Info bad alloc: " << t.what() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
}
//Delete the mechanism for an overflow after modified Poleni-formula
void Fpl_Sec_Type_Dune::delete_overflow_mechanism(void){
	if(this->overflow_event!=NULL){
		delete this->overflow_event;
		this->overflow_event=NULL;
	}
}
//Allocate the mechanism for erosion landside of the section
void Fpl_Sec_Type_Dune::allocate_erosion_landside_mechanism(void){
	if(this->erosion_landside_event!=NULL){
		return;
	}

	try{
		this->erosion_landside_event=new Fpl_Mech_Landside_Erosion();
	}
	catch(bad_alloc &t){
		Error msg=this->set_error(5);
		ostringstream info;
		info<< "Info bad alloc: " << t.what() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
}
//Delete the mechanism for erosion landside of the section
void Fpl_Sec_Type_Dune::delete_erosion_landside_mechanism(void){
	if(this->erosion_landside_event!=NULL){
		delete this->erosion_landside_event;
		this->erosion_landside_event=NULL;
	}
}
//Allocate the mechanism for erosion waterside of the section after van Gent
void Fpl_Sec_Type_Dune::allocate_erosion_waterside_gent_mechanism(void){
	if(this->erosion_waterside_gent_event!=NULL){
		return;
	}

	try{
		this->erosion_waterside_gent_event=new Fpl_Mech_Erosion_Dune_van_Gent();
	}
	catch(bad_alloc &t){
		Error msg=this->set_error(6);
		ostringstream info;
		info<< "Info bad alloc: " << t.what() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
}
//Delete the mechanism for erosion waterside of the section after van Gent
void Fpl_Sec_Type_Dune::delete_erosion_waterside_gent_mechanism(void){
	if(this->erosion_waterside_gent_event!=NULL){
		delete this->erosion_waterside_gent_event;
		this->erosion_waterside_gent_event=NULL;
	}
}
//Allocate the waterside material variable zone
void Fpl_Sec_Type_Dune::allocate_waterside_variable_zone(void){
	this->delete_waterside_variable_zone();
	try{
		this->material_waterside_zone=new Fpl_Cub_Waterside_Materialzone[this->number_mat_waterside_zones];
	}
	catch(bad_alloc &t){
		Error msg=this->set_error(13);
		ostringstream info;
		info<< "Info bad alloc: " << t.what() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
}
//Delete the waterside material variable zone
void Fpl_Sec_Type_Dune::delete_waterside_variable_zone(void){
	if(this->material_waterside_zone!=NULL){
		delete []this->material_waterside_zone;
		this->material_waterside_zone=NULL;
	}
}
//Set the general parameters of the section to the mechanisms
void Fpl_Sec_Type_Dune::set_general(void){
	//output
	ostringstream cout;
	cout <<"Set general parameters of dune... "  << endl ;
	Sys_Common_Output::output_fpl->output_txt(cout.str(),false);
	//wind wave generation
	if(this->wave_wind_event!=NULL){
		if(this->wave_wind_event->get_2calc_flag()==true){
			this->wave_wind_event->set_section_direction(this->section_direction);
		}
	}
}
//Set the random variables of this section and the relevant mechanisms from database table
void Fpl_Sec_Type_Dune::set_variables(const bool frc_sim, QSqlDatabase *ptr_database, const int section_id){
	//output
	ostringstream cout;
	cout <<"Set general variables of the dune section " <<section_id<<" ... "  << endl ;
	Sys_Common_Output::output_fpl->output_txt(&cout,false);
	try{
		//waterlevel is special (frc calculation or mc calc)
		if(frc_sim==false){
			this->waterlevel.set_input(this->system_id, section_id, fpl_label::waterlevel,false, ptr_database, fpl_label::mech_all,fpl_label::sec_dune,0);
			this->waterlevel.set_distribution_types(ptr_database);
		}
		else{
			this->waterlevel.set_input(this->system_id, section_id, fpl_label::waterlevel_frc,true, ptr_database, fpl_label::mech_all,fpl_label::sec_dune,0);
			this->waterlevel.set_distribution_types(ptr_database);
		}
		//water density
		this->water_density.set_input(this->system_id, section_id, fpl_label::water_density,false, ptr_database, fpl_label::mech_all,fpl_label::sec_dune,0);
		this->water_density.set_distribution_types(ptr_database);

		//wind wave generation
		try{
			if(this->wave_wind_event!=NULL){
				if(this->wave_wind_event->get_2calc_flag()==true){
					cout <<"Set variables of the wave generation mechanism..."  << endl ;
					Sys_Common_Output::output_fpl->output_txt(&cout,false);
					this->wave_wind_event->set_windspeed_direction(ptr_database, this->system_id, section_id);
					this->wave_wind_event->set_variables(ptr_database,this->system_id, section_id);
					this->wave_wind_event->set_result_variables(ptr_database,this->system_id, section_id);
				}
			}
		}
		catch(Error msg){
			Warning warn1;
			ostringstream reaction;
			reaction<<"Mechansim of the wave generation is not taken into account; also the waterside erosion mechanism are not calculated";
			warn1.error_to_warning(&msg, 20);
			warn1.set_reaction(reaction.str());
			warn1.output_msg(1);
			//reation
			this->delete_wind_wave_mechanism();
		}
		//wave runup
		try{
			if(this->wave_runup_event!=NULL){
				if(this->wave_runup_event->get_2calc_flag()==true){
					cout <<"Set variables of the wave run-up mechanism..."  << endl ;
					Sys_Common_Output::output_fpl->output_txt(&cout,false);
					this->waterside_cubature.set_gamma_berm(ptr_database,this->system_id, section_id);
					//init the runup mechanism
					this->wave_runup_event->set_variables(ptr_database, this->system_id, section_id);
					this->wave_runup_event->set_result_variables(ptr_database,this->system_id, section_id);
				}
			}
		}
		catch(Error msg){
			Warning warn1;
			ostringstream reaction;
			reaction<<"Mechansim wave run-up is not taken into account!";
			warn1.error_to_warning(&msg, 20);
			warn1.set_reaction(reaction.str());
			warn1.output_msg(1);
			//reation
			this->delete_wave_runup_mechanism();
		}
		//overflow event
		try{
			if(this->overflow_event!=NULL){
				if(this->overflow_event->get_2calc_flag()==true){
					cout <<"Set variables of the overflow mechanism..."  << endl ;
					Sys_Common_Output::output_fpl->output_txt(&cout,false);
					//set the waterside geometrie
					this->overflow_event->set_waterside_cubature(&this->waterside_cubature);
					//set the crest geometrie
					this->overflow_event->set_crest_cubature(&this->crest_cubature);
					//set the landside geometrie
					this->overflow_event->set_landside_cubature(&this->landside_cubature);
					//set the variables
					this->overflow_event->set_variables(ptr_database,this->system_id, section_id);
					this->overflow_event->set_result_variables(ptr_database,this->system_id, section_id);
				}
			}
		}
		catch(Error msg){
			Warning warn1;
			ostringstream reaction;
			reaction<<"Mechanism overflow is not taken into account!";
			warn1.error_to_warning(&msg, 20);
			warn1.set_reaction(reaction.str());
			warn1.output_msg(1);
			//reation
			this->delete_overflow_mechanism();
		}
		//erosion landside
		if(this->erosion_landside_event!=NULL){
			if(this->erosion_landside_event->get_2calc_flag()==true){
				cout <<"Set variables of the erosion landside mechanism..."  << endl ;
				Sys_Common_Output::output_fpl->output_txt(&cout,false);
				//set the variables
				this->erosion_landside_event->set_variables(ptr_database,this->system_id, section_id);
				this->erosion_landside_event->set_result_variables(ptr_database,this->system_id, section_id);
			}
		}

		//erosion waterside
		try{
			if(this->erosion_waterside_gent_event!=NULL){
				if(this->erosion_waterside_gent_event->get_2calc_flag()==true){
					cout <<"Set variables of the erosion waterside mechanism..."  << endl ;
					Sys_Common_Output::output_fpl->output_txt(&cout,false);
					//set the variables
					this->erosion_waterside_gent_event->set_variables(ptr_database,this->system_id, section_id);
					this->erosion_waterside_gent_event->set_result_variables(ptr_database,this->system_id, section_id);
				}
			}
		}
		catch(Error msg){
			Warning warn1;
			ostringstream reaction;
			reaction<<"Mechanism erosion waterside is not taken into account!";
			warn1.error_to_warning(&msg, 20);
			warn1.set_reaction(reaction.str());
			warn1.output_msg(1);
			//reation
			this->delete_erosion_waterside_gent_mechanism();
		}
	}
	catch(Error msg){
		throw msg;
	}
}
//Set the geometry of the section and their surrounding via a database table
void Fpl_Sec_Type_Dune::set_geometrie(QSqlDatabase *ptr_database, const int section_id){
	//output
	ostringstream cout;
	cout <<"Set the geometry of the dune section " <<section_id<<" ... "  << endl ;
	Sys_Common_Output::output_fpl->output_txt(&cout,false);
	try{
		//read the cubature
		this->read_cubature(ptr_database, section_id);
		//set the variable zone of the waterside
		this->read_mat_variable_zones_waterside(ptr_database, section_id);
		

		if(this->wave_wind_event!=NULL){
			if(this->wave_wind_event->get_2calc_flag()==true){
				try{
					cout <<"Set geometry of the dune surrounding... "  << endl ;
					Sys_Common_Output::output_fpl->output_txt(&cout,false);
					this->wave_wind_event->new_set_fetch(ptr_database, this->system_id, section_id);
				}
				catch(Error msg){
					Warning warn1;
					ostringstream reaction;
					reaction<<"Mechansim of the wave generation is not taken into account!";
					warn1.error_to_warning(&msg, 20);
					warn1.set_reaction(reaction.str());
					warn1.output_msg(1);
					//reation
					this->delete_wind_wave_mechanism();
				}
			}
		}
	}
	catch(Error msg){
		throw msg;
	}
}
//Set control parameters of the mechanisms
void Fpl_Sec_Type_Dune::set_control_parameter_mechanism(QSqlDatabase *ptr_database, const int section_id, const bool frc_sim){
	//output
	ostringstream cout;
	cout <<"Set the control parameters of the dune section " <<section_id<<" for the mechanisms... "  << endl ;
	Sys_Common_Output::output_fpl->output_txt(&cout,false);

	//this->seepage_calculator.set_input(ptr_database, frc_sim, true);

	//
	//try{
	//	if(this->slope_stability_landside_event!=NULL){
	//		if(this->slope_stability_landside_event->get_2calc_flag()==true){
	//			cout <<"Set the control parameters of the landside slope stability... "  << endl ;
	//			Sys_Common_Output::output_fpl->output_txt(&cout,false);
	//			if(this->slope_stability_landside_event->set_control_parameter(ptr_database, section_id)==false){
	//				cout <<"The landside slope stability is not calculated for dike section "  <<section_id<< endl ;
	//				this->slope_stability_landside_event->set_2calc_flag(false);
	//				Sys_Common_Output::output_fpl->output_txt(&cout,false);
	//			}
	//		}
	//	}
	//}
	//catch(Error msg){
	//	Warning warn1;
	//	ostringstream reaction;
	//	reaction<<"Mechansim of the landside slope stability is not taken into account!";
	//	warn1.error_to_warning(&msg, 20);
	//	warn1.set_reaction(reaction.str());
	//	warn1.output_msg(1);
	//	//reation
	//	this->delete_slope_stability_landside_mechanism();
	//}
}
//Initialize the mechanisms
void Fpl_Sec_Type_Dune::init_mechanism(const int section_id){
		//output
	ostringstream cout;
	cout <<"Initialize the mechanisms of the dune section " <<section_id<<"... "  << endl ;
	Sys_Common_Output::output_fpl->output_txt(&cout,false);
	try{
		if(this->erosion_waterside_gent_event!=NULL){
			if(this->erosion_waterside_gent_event->get_2calc_flag()==true){
				cout <<"Initialize the mechanism of the erosion waterside..."  << endl ;
				Sys_Common_Output::output_fpl->output_txt(&cout,false);
				//set the variables
				this->erosion_waterside_gent_event->set_ptr_area_poly_outer(&this->area_out_polygon, &this->outer_polygon);
				this->erosion_waterside_gent_event->set_ptr_crest_points(&this->crest_land, this->crest_water);
				this->erosion_waterside_gent_event->set_ptr_cubature(&this->foreland, &this->waterside_cubature, &this->crest_cubature, &this->area_cubature);
				this->erosion_waterside_gent_event->init_new_shape_poly();
			}
		}
	}
	catch(Error msg){
		throw msg;
	}
}
//Set the fault tree for this section type via a database table
void Fpl_Sec_Type_Dune::init_fault_tree(QSqlDatabase *ptr_database){
	//output
	ostringstream cout;
	cout <<"Initialize the fault tree for a dune-section... " << endl ;
	Sys_Common_Output::output_fpl->output_txt(cout.str(),false);

	//set the mechanism table
	try{
		this->set_table_mechanism(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}

	//mysql query with the table_model
	QSqlTableModel model(0,*ptr_database);
	//set the infos of the faulttree
	//string for the query for one section
	ostringstream filter;
	filter << _Fpl_Section_Type::table_mechanism->get_column_name(fpl_label::mecha_sec_type) << " = " << "'"<<fpl_label::sec_dune<<"'";
	filter << " and " <<  _Fpl_Section_Type::table_mechanism->get_column_name(fpl_label::mecha_applied) << " = true";
	model.setTable((_Fpl_Section_Type::table_mechanism->get_table_name()).c_str());
	model.setFilter(filter.str().c_str());
    //set the query
	Data_Base::database_request(&model);
	if(model.lastError().isValid()){//table is not found
		Error msg=set_error(0);
		ostringstream info;
		info << "Table Name              : " << _Fpl_Section_Type::table_mechanism->get_table_name() << endl;
		info << "Table error info        : " << model.lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw (msg);
	}
	//read out the results
	//section type is set (string)
	string mechanism_buffer;
	bool mechanism_flag=false;
	bool in_tree_flag=false;
	int number_result=model.rowCount();
	for(int i=0;i<number_result;i++){
		mechanism_flag=false;
		mechanism_buffer = model.record(i).value((_Fpl_Section_Type::table_mechanism->get_column_name(fpl_label::mecha_name)).c_str()).toString().toStdString();
		mechanism_flag = model.record(i).value((_Fpl_Section_Type::table_mechanism->get_column_name(fpl_label::mecha_applied)).c_str()).toBool();
		in_tree_flag = model.record(i).value((_Fpl_Section_Type::table_mechanism->get_column_name(fpl_label::mecha_applied_tree)).c_str()).toBool();
		this->decide_mechanism(mechanism_buffer, mechanism_flag, in_tree_flag);
	}
	//no mechanism is set
	if(this->number_mech==0){
		Warning msg=this->set_warning(0);
		msg.output_msg(1);
	}
	this->check_fault_tree();
}
//Decide and allocate the mechanism, which should by applied during calculation
void Fpl_Sec_Type_Dune::decide_mechanism(const string mechanism_name, const bool application_flag, const bool applied_in_tree){
	//application desired
	if(application_flag==true){
		if(mechanism_name==fpl_label::mech_overflow){
			this->allocate_overflow_mechanism();
			this->overflow_event->set_2calc_flag(true);
			this->overflow_event->set_in_tree_flag(true);
			this->number_mech++;
			//output
			ostringstream cout;
			cout <<"Mechanism overflow is initialized..." << endl ;
			Sys_Common_Output::output_fpl->output_txt(cout.str(),false);
		}
		else if(mechanism_name==fpl_label::mech_wind_wave){
			this->allocate_wind_wave_mechanism();
			this->wave_wind_event->set_2calc_flag(true);
			this->wave_wind_event->set_in_tree_flag(true);
			this->number_mech++;
			ostringstream cout;
			cout <<"Wind induced wave-generation mechanism is initialized..." << endl ;
			Sys_Common_Output::output_fpl->output_txt(cout.str(),false);
		}
		else if(mechanism_name==fpl_label::mech_wave_runup){
			this->allocate_wave_runup_mechanism();
			this->wave_runup_event->set_2calc_flag(true);
			this->wave_runup_event->set_in_tree_flag(true);
			this->number_mech++;
			ostringstream cout;
			cout <<"Wave runup mechanism is initialized..." << endl ;
			Sys_Common_Output::output_fpl->output_txt(cout.str(),false);
		}
		else if(mechanism_name==fpl_label::mech_erosion_landside){
			this->allocate_erosion_landside_mechanism();
			this->erosion_landside_event->set_2calc_flag(true);
			this->erosion_landside_event->set_in_tree_flag(true);
			this->number_mech++;
			ostringstream cout;
			cout <<"Erosion landside is initialized..." << endl ;
			Sys_Common_Output::output_fpl->output_txt(cout.str(),false);
		}
		else if(mechanism_name==fpl_label::mech_erosion_waterside_gent){
			this->allocate_erosion_waterside_gent_mechanism();
			this->erosion_waterside_gent_event->set_2calc_flag(true);
			this->erosion_waterside_gent_event->set_in_tree_flag(true);
			this->number_mech++;
			ostringstream cout;
			cout <<"Erosion waterside after van Gent is initialized..." << endl ;
			Sys_Common_Output::output_fpl->output_txt(cout.str(),false);
		}
	}
}
//Set the dune geometry via database and set it to the relevant mechanisms
void Fpl_Sec_Type_Dune::read_cubature(QSqlDatabase *ptr_database, const int section_id){
	//set the values for the crest point (waterside) and the base point (water side)[0,0]
	//base point water is always 0,0...all is related to them!
	this->base_water.set_point_coordinate(0.0,0.0);
	//read out the results
	try{
		//this->crest_water.set_point_per_database(this->system_id, section_id, fpl_label::crest_water, ptr_database, fpl_label::region_cubature, fpl_label::sec_dune, 0);
		this->crest_land.set_point_per_database(this->system_id, section_id, fpl_label::crest_land, ptr_database, fpl_label::region_cubature, fpl_label::sec_dune, 0);
		this->base_land.set_point_per_database(this->system_id, section_id, fpl_label::base_land, ptr_database, fpl_label::region_cubature, fpl_label::sec_dune, 0);
	}catch(Error msg){
		throw msg;
	}
	//set the geometry segments of the dune section
	try{
		//set the foreland geometry
		this->foreland.set_segments(&this->base_water, ptr_database, this->system_id, section_id);
		//set the waterside cubature
		this->waterside_cubature.set_segments(&this->base_water,this->crest_water, ptr_database, this->system_id, section_id);
		//set the crest segment (just one segment)
		this->crest_cubature.set_segment(this->crest_water, &this->crest_land);
		//set the landside cubature
		this->landside_cubature.set_segments(&this->base_land,&this->crest_land, ptr_database, this->system_id, section_id);
		//set the hinterland geometry
		this->hinterland.set_segments(&this->base_land, ptr_database, this->system_id, section_id);
	}
	catch(Error msg){
		throw msg;
	}
}
//Set the dike material variable zones of the waterside via database 
void Fpl_Sec_Type_Dune::read_mat_variable_zones_waterside(QSqlDatabase *ptr_database, const int section_id){

	//output
	ostringstream cout;
	cout <<"Set the material variable zone(s) of the dune waterside... "  << endl ;
	Sys_Common_Output::output_fpl->output_txt(&cout,false);

	QList<int> buffer;
	//check which zones are needed
	for(int i=0; i< this->waterside_cubature.get_number_segments(); i++){
		bool old_one=false;
		for(int j=0; j< buffer.count(); j++){
			if(buffer.at(j)==this->waterside_cubature.get_segment(i)->get_zone_number()){
				old_one=true;
				break;
			}
		}
		if(old_one==false){
			buffer.append(this->waterside_cubature.get_segment(i)->get_zone_number());
		}
	}

	bool runup=false;
	if(this->wave_runup_event!=NULL){
		runup=true;
	}
	bool wave_impact=false;
	bool erosion=false;
	
	this->number_mat_waterside_zones=buffer.count();
	if(this->number_mat_waterside_zones==0){
		if(this->wave_runup_event==NULL){
			cout <<"No material variable zone(s) of the waterside are found... "  << endl ;
			Sys_Common_Output::output_fpl->output_txt(&cout,false);
			return;
		}
		else{
			Error msg=this->set_error(14);
			throw msg;
		}
	}
	else{
		//output
		cout <<this->number_mat_waterside_zones<< " relevant material variable zone(s) of the waterside are found... "  << endl ;
		Sys_Common_Output::output_fpl->output_txt(&cout,false);
		this->allocate_waterside_variable_zone();

		//set the zone input
		for(int i=0; i<this->number_mat_waterside_zones; i++){
			this->material_waterside_zone[i].set_input(section_id, this->system_id, ptr_database, buffer.at(i), runup, wave_impact, erosion);
		}

		this->connect_material_zones_waterside();
	}
}
//Connect the material zones to the material variable zone of the waterside
void Fpl_Sec_Type_Dune::connect_material_zones_waterside(void){
		//output
	ostringstream cout;
	cout <<"Connect the waterside material variable zone to the material zone of the waterside cubature... "  << endl ;
	Sys_Common_Output::output_fpl->output_txt(&cout,false);

	//check the material variable zone id
	for(int i=0; i< this->number_mat_waterside_zones; i++){
		for(int j=0; j<this->number_mat_waterside_zones; j++){
			if(i!=j){
				if(this->material_waterside_zone[i].get_zone_id()==this->material_waterside_zone[j].get_zone_id()){
					Error msg=this->set_error(15);
					ostringstream info;
					info << "Id of the material variable zone (waterside) : "<< this->material_waterside_zone[i].get_zone_id()<<endl;
					msg.make_second_info(info.str());
					throw msg;
				}
			}
		}
	}

	//connect the material variable zone id to the geometrical zone
	for(int i=0; i< this->waterside_cubature.get_number_segments(); i++){
		bool connection_found=false;
		for(int j=0; j< this->number_mat_waterside_zones; j++){
			if(this->waterside_cubature.get_segment(i)->get_zone_number()==this->material_waterside_zone[j].get_zone_id()){
				this->waterside_cubature.get_segment(i)->ptr_mat_zone=(&this->material_waterside_zone[j]);
				connection_found=true;
				break;
			}
		}

		if(connection_found==false){
			Error msg=this->set_error(16);
			ostringstream info;
			info<<" Materialzone id waterside cubature  : "<<this->waterside_cubature.get_segment(i)->get_zone_number()<<endl;
			msg.make_second_info(info.str());
			throw msg;
		}
	}

	cout <<"Connection of the waterside material variable zone to the material zone of the waterside cubature is finished!"  << endl ;
	Sys_Common_Output::output_fpl->output_txt(&cout,false);
}
//Create the geometrical outer boundary of the dune as polysegment
void Fpl_Sec_Type_Dune::make_geometrical_outer_dune_polysegment(void){
	//make the polysegment
	//count the number of points, the first and the last are points horizontal to the waterside dune toe/landside dune toe
	int number_points=0;
	number_points=this->foreland.get_number_segments()+this->waterside_cubature.get_number_segments()+this->crest_cubature.get_number_segments()
		+this->landside_cubature.get_number_segments()+this->hinterland.get_number_segments()+1;

	//allocate a buffer for the points
	Geo_Point *buffer=NULL;
	try{
		buffer=new Geo_Point[number_points];
	}
	catch(bad_alloc &t){
		Error msg=this->set_error(7);
		ostringstream info;
		info<< "Info bad alloc: " << t.what() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}

	//set the point buffer
	int counter=0;
	//foreland
	for(int i=0; i< this->foreland.get_number_segments();i++){
		buffer[counter]=this->foreland.get_segment(i)->point1;
		counter++;
	}
	//waterside cubature
	for(int i=0; i< this->waterside_cubature.get_number_segments();i++){
		buffer[counter]=this->waterside_cubature.get_segment(i)->point1;
		counter++;
	}
	//crest
	buffer[counter]=this->crest_cubature.get_segment()->point1;
	counter++;
	//landside cubature
	for(int i=0; i< this->landside_cubature.get_number_segments();i++){
		buffer[counter]=this->landside_cubature.get_segment(i)->point1;
		counter++;
	}
	//hinterland
	for(int i=0; i< this->hinterland.get_number_segments();i++){
		buffer[counter]=this->hinterland.get_segment(i)->point1;
		counter++;
	}
	//set the last point
	if(this->hinterland.get_number_segments()==0){
		buffer[counter]=this->landside_cubature.get_segment(this->landside_cubature.get_number_segments()-1)->point2;
	}
	else{
		buffer[counter]=this->hinterland.get_segment(this->hinterland.get_number_segments()-1)->point2;
	}
	//set the polysegment
	try{
		this->outer_polysegment.set_segments(number_points,buffer);
	}
	catch(Error msg){
		if(buffer!=NULL){
			delete []buffer;
			buffer=NULL;
		}
		throw msg;
	}

	//delete the buffer
	if(buffer!=NULL){
		delete []buffer;
		buffer=NULL;
	}
}
//Create the geometrical outer boundary of the dune as polygon
void Fpl_Sec_Type_Dune::make_geometrical_outer_dune_polygon(void){
	//make the polysegment
	//count the number of points, the first and the last are points horizontal to the waterside dune toe/landside dune toe
	int number_points;
	number_points=3+this->foreland.get_number_segments()+this->waterside_cubature.get_number_segments()+this->crest_cubature.get_number_segments()+
		this->landside_cubature.get_number_segments()+this->hinterland.get_number_segments();

	//allocate a buffer for the points
	Geo_Point *buffer=NULL;
	try{
		buffer=new Geo_Point[number_points];
	}
	catch(bad_alloc &t){
		Error msg=this->set_error(8);
		ostringstream info;
		info<< "Info bad alloc: " << t.what() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}

	//set the point buffer
	double y_min=-10000.0;

	//set the first point
	int counter=0;
	if(this->foreland.get_number_segments()==0){
		buffer[counter].set_point_x_coordinate(this->waterside_cubature.get_segment(0)->point1.get_xcoordinate());
		buffer[counter].set_point_y_coordinate(y_min);
	}
	else{
		buffer[counter].set_point_x_coordinate(this->foreland.get_segment(0)->point1.get_xcoordinate());
		buffer[counter].set_point_y_coordinate(y_min);
	}
	counter++;
	//foreland
	for(int i=0; i< this->foreland.get_number_segments();i++){
		buffer[counter]=this->foreland.get_segment(i)->point1;
		counter++;
	}
	//waterside cubature
	for(int i=0; i< this->waterside_cubature.get_number_segments();i++){
		buffer[counter]=this->waterside_cubature.get_segment(i)->point1;
		counter++;
	}
	//crest
	buffer[counter]=this->crest_cubature.get_segment()->point1;
	counter++;
	//landside cubature
	for(int i=0; i< this->landside_cubature.get_number_segments();i++){
		buffer[counter]=this->landside_cubature.get_segment(i)->point1;
		counter++;
	}
	//hinterland
	for(int i=0; i< this->hinterland.get_number_segments();i++){
		buffer[counter]=this->hinterland.get_segment(i)->point1;
		counter++;
	}
	//set the two last point
	if(this->hinterland.get_number_segments()==0){
		buffer[counter]=this->landside_cubature.get_segment(this->landside_cubature.get_number_segments()-1)->point2;
		counter++;
		buffer[counter].set_point_x_coordinate(this->landside_cubature.get_segment(this->landside_cubature.get_number_segments()-1)->point2.get_xcoordinate());
		buffer[counter].set_point_y_coordinate(y_min);
		counter++;
	}
	else{
		buffer[counter]=this->hinterland.get_segment(this->hinterland.get_number_segments()-1)->point2;
		counter++;
		buffer[counter].set_point_x_coordinate(this->hinterland.get_segment(this->hinterland.get_number_segments()-1)->point2.get_xcoordinate());
		buffer[counter].set_point_y_coordinate(y_min);
		counter++;
	}
	//
	//set the polysegment
	try{
		this->outer_polygon.set_number_points(number_points);
		this->outer_polygon.set_points(buffer);
	}
	catch(Error msg){
		if(buffer!=NULL){
			delete []buffer;
			buffer=NULL;
		}
		throw msg;
	}

	//delete the buffer
	if(buffer!=NULL){
		delete []buffer;
		buffer=NULL;
	}

	this->area_out_polygon=this->outer_polygon.calculate_area();
}
//Calcualte the area of the cubature
void Fpl_Sec_Type_Dune::calc_area_cubature(void){
	//make the polysegment
	//count the number of points, the first and the last are points horizontal to the waterside dune toe/landside dune toe
	int number_points;
	number_points=1+this->waterside_cubature.get_number_segments()+this->crest_cubature.get_number_segments()
		+this->landside_cubature.get_number_segments();

	//allocate a buffer for the points
	Geo_Point *buffer=NULL;
	try{
		buffer=new Geo_Point[number_points];
	}
	catch(bad_alloc &t){
		Error msg=this->set_error(9);
		ostringstream info;
		info<< "Info bad alloc: " << t.what() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}

	int counter=0;
	//waterside cubature
	for(int i=0; i< this->waterside_cubature.get_number_segments();i++){
		buffer[counter]=this->waterside_cubature.get_segment(i)->point1;
		counter++;
	}
	//crest
	buffer[counter]=this->crest_cubature.get_segment()->point1;
	counter++;
	//landside cubature
	for(int i=0; i< this->landside_cubature.get_number_segments();i++){
		buffer[counter]=this->landside_cubature.get_segment(i)->point1;
		counter++;
	}
	//set the two last point
	buffer[counter]=this->landside_cubature.get_segment(this->landside_cubature.get_number_segments()-1)->point2;
	counter++;

	Geo_Simple_Polygon buff_poly;
	//set the polysegment
	try{
		buff_poly.set_number_points(number_points);
		buff_poly.set_points(buffer);
	}
	catch(Error msg){
		if(buffer!=NULL){
			delete []buffer;
			buffer=NULL;
		}
		throw msg;
	}

	//delete the buffer
	if(buffer!=NULL){
		delete []buffer;
		buffer=NULL;
	}

	this->area_cubature=buff_poly.calculate_area();
}
//Check the initialized fault-tree
void Fpl_Sec_Type_Dune::check_fault_tree(void){
}
//Check the gradient of a dune for their physical sense
void Fpl_Sec_Type_Dune::check_dune_gradients(const double grad, const bool waterside){
	ostringstream cub;
	if(waterside==true){
		cub << " Waterside cubature"<<endl;
	}
	else{
		cub << " Landside cubature"<<endl;
	}

	//flat
	if(grad <0.05 && grad >= 0.02){
		//warning
		Warning msg=this->set_warning(1);
		ostringstream info;
		info << cub.str();
		info << " Mid gradient : " << grad << label::no_unit <<endl;
		msg.make_second_info(info.str());
		msg.output_msg(1);
	}
	if(grad <0.02){
		//Error
		Error msg=this->set_error(11);
		ostringstream info;
		info << cub.str();
		info << " Mid gradient : " << grad << label::no_unit <<endl;
		msg.make_second_info(info.str());
		throw msg;
	}
	//step
	if(grad > 0.9 && grad <= 1.0){
		//warning
		Warning msg=this->set_warning(2);
		ostringstream info;
		info << cub.str();
		info << " Mid gradient : " << grad << label::no_unit <<endl;
		msg.make_second_info(info.str());
		msg.output_msg(1);
	}
	if(grad >1.0){
		//Error
		Error msg=this->set_error(10);
		ostringstream info;
		info << cub.str();
		info << " Mid gradient : " << grad << label::no_unit <<endl;
		msg.make_second_info(info.str());
		throw msg;
	}
}
//Reset the flags for the random generation
void Fpl_Sec_Type_Dune::reset_random_flag(void){
	_Fpl_Section_Type::reset_random_flag();
	this->water_density.reset_random_generated();

	//for(int i=0; i< this->number_material_variable_zones; i++){
	//	this->material_variable_zones[i].reset_random_flag();
	//}
}
//Set error(s)
Error Fpl_Sec_Type_Dune::set_error(const int err_type){
		string place="Fpl_Sec_Type_Dune::";
		string help;
		string reason;
		int type=0;
		bool fatal=false;
		stringstream info;
		Error msg;

	switch (err_type){
		case 0://table not found (fatal error)
			place.append("init_fault_tree(QSqlDatabase *ptr_database)");
			reason="Can not read in the fault-tree from the database table";
			help="Check the table";
			type=2;
			fatal=true;//fatal error;
			break;
		case 2://bad_alloc
			place.append("allocate_wind_wave_mechanism(void)");
			reason="Can not allocate the memory";
			help="Check the memory";
			type=10;
			break;
		case 3://bad_alloc
			place.append("allocate_wave_runup_mechanism(void)");
			reason="Can not allocate the memory";
			help="Check the memory";
			type=10;
			break;
		case 4://bad_alloc
			place.append("allocate_overflow_mechanism(void)");
			reason="Can not allocate the memory";
			help="Check the memory";
			type=10;
			break;
		case 5://bad_alloc
			place.append("allocate_erosion_landside_mechanism(void)");
			reason="Can not allocate the memory";
			help="Check the memory";
			type=10;
			break;
		case 6://bad_alloc
			place.append("allocate_erosion_waterside_gent_mechanism(void)");
			reason="Can not allocate the memory";
			help="Check the memory";
			type=10;
			break;
		case 7://bad_alloc
			place.append("make_geometrical_outer_dune_polysegment(void)");
			reason="Can not allocate the memory";
			help="Check the memory";
			type=10;
			break;
		case 8://bad_alloc
			place.append("make_geometrical_outer_dune_polygon(void)");
			reason="Can not allocate the memory";
			help="Check the memory";
			type=10;
			break;
		case 9://bad_alloc
			place.append("calc_area_cubature(void)");
			reason="Can not allocate the memory";
			help="Check the memory";
			type=10;
			break;
		case 10://very step gradient
			place.append("check_dune_gradients(const double grad, const bool waterside)");
			reason="The gradient of the dune cubature is to step for a dune";
			help="Check the dune cubature and the base point";
			type=1;
			break;
		case 11://very flat gradient
			place.append("check_dune_gradients(const double grad, const bool waterside)");
			reason="The gradient of the dune cubature is to flat for a dune";
			help="Check the dune cubature and the base point";
			type=11;
			break;
		case 12://no foreland
			place.append("check_set_geometry(void)");
			reason="No dune foreland is set";
			help="Set a dune foreland";
			type=11;
			break;
		case 13://bad_alloc
			place.append("allocate_waterside_variable_zone(void)");
			reason="Can not allocate the memory";
			help="Check the memory";
			type=10;
			break;
		case 14://no material zones are found
			place.append("read_mat_variable_zones_waterside(QSqlDatabase *ptr_database, const int section_id)");
			reason="No material zones of the waterside are found in database table";
			help="Check the waterside material zones";
			type=2;
			break;
		case 15://same material variable zone id is found
			place.append("connect_material_zones_waterside(void)");
			reason="Same waterside material variable zone id are found";
			help="Check the waterside variable material zone information";
			type=27;
			break;
		case 16://no material variable zone is found to the material zone
			place.append("connect_material_zones_waterside(void)");
			reason="No waterside material variable zone is found to the material zone of the waterside cubature";
			help="Check the geometrical and variable material zone information of the waterside cubature";
			type=27;
			break;
		case 17://wrong input
			place.append("read_var_waterside_matzones(QFile *ifile, int *line_counter)");
			reason="There is a problem with the input; wrong sign is read in";
			help="Check the information of the waterside material variable zones in file";
			type=1;
			break;
		case 18://no material zone is found in file
			place.append("read_var_waterside_matzones(QFile *ifile, int *line_counter)");
			reason="No material zone is found in file";
			help="Check the information of the waterside material variable zones in file";
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
Warning Fpl_Sec_Type_Dune::set_warning(const int warn_type){
		string place="Fpl_Sec_Type_Dune::";
		string help;
		string reason;
		string reaction;
		int type=0;
		Warning msg;
		stringstream info;

	switch (warn_type){
		case 0://no mechanism is set
			place.append("init_fault_tree(QSqlDatabase *ptr_database)");
			reason="No mechanism is set for the section type dune";
			help="Check the fault tree for the section type";
			reaction="No reliabilty calculation required";
			type=6;
			break;
		case 1://very flat gradient
			place.append("check_dune_gradients(const double grad, const bool waterside)");
			reason="The gradient of the dune cubature is very flat for a dune";
			help="Check the dune cubature and the base point";
			type=8;
			break;
		case 2://very step gradient
			place.append("check_dune_gradients(const double grad, const bool waterside)");
			reason="The gradient of the dune cubature is very step for a dune";
			help="Check the dune cubature and the base point";
			type=8;
			break;
		case 3://results datas can not submitted
			place.append("output_result2table(QSqlDatabase *ptr_database, _fpl_simulation_type simulation_type, _sys_system_id id, const int section_id)");
			reason="Can not submit the mechanism result data to the database";
			help="Check the database";
			type=2;
			break;
		case 4://results datas can not submitted
			place.append("check_set_geometry(void)");
			reason="The gradient of the dune forland is steeper than 0.08 (1:12.5)";
			help="Check the dune geometry";
			reaction="The dune profile can be very instable";
			type=8;
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