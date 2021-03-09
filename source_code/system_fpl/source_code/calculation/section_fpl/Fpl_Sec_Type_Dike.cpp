//#include "Fpl_Sec_Type_Dike.h"
#include "Fpl_Headers_Precompiled.h"
//init static members
Tables *Fpl_Sec_Type_Dike::result_table=NULL;

//Default constructor
Fpl_Sec_Type_Dike::Fpl_Sec_Type_Dike(void){
	//mechanism
	this->wave_wind_event=NULL;
	this->wave_runup_event=NULL;
	this->overflow_event=NULL;
	this->bursting_din_event=NULL;
	this->piping_sell_event=NULL;
	this->piping_lane_event=NULL;
	this->piping_schmertmann_event=NULL;

	this->slope_stability_landside_event=NULL;
	this->slope_stability_waterside_event=NULL;

	this->erosion_landside_event=NULL;

	this->erosion_waterside_event=NULL;
	this->erosion_waterside_start_event=NULL;
	this->wave_impact_event=NULL;

	this->area_cubature=0.0;
	//set the point name
	this->base_land.set_point_name(fpl_label::base_land);
	this->crest_land.set_point_name(fpl_label::crest_land);

	this->number_material_zones=0;
	this->material_zones=NULL;
	this->number_material_variable_zones=0;
	this->material_variable_zones=NULL;

	this->seepage_calculator.set_ptr_waterside_cub(&this->waterside_cubature);
	this->seepage_calculator.set_ptr_landside_cub(&this->landside_cubature);
	this->seepage_calculator.set_ptr_cubature(&this->outer_polysegment);

	this->waterlevel.set_unit(label::m);
	this->water_density.set_unit(label::kg_per_qm);

	this->current_waterlevel=0.0;

	this->index_h_imperm_layer=0;

	this->number_mat_waterside_zones=0;
	this->material_waterside_zone=NULL;

	Sys_Memory_Count::self()->add_mem((sizeof(Fpl_Sec_Type_Dike)-2*sizeof(Fpl_Random_Variables)-
		sizeof(Fpl_Cub_Dike_Waterside)-sizeof(Fpl_Cub_Dike_Crest)-sizeof(Fpl_Cub_Dike_Landside)-
		2*sizeof(Fpl_Seepage_Line_Point_List)-sizeof(Fpl_Dike_Geo_Foreland)-sizeof(Fpl_Dike_Geo_Hinterland)-
		sizeof(Geo_Simple_Polygon)-sizeof(Geo_Polysegment)-sizeof(Fpl_Seepage_Calculator_Dike)), _sys_system_modules::FPL_SYS);//count the memory
}
//Default destructor
Fpl_Sec_Type_Dike::~Fpl_Sec_Type_Dike(void){
	//delete the mechanism
	this->delete_overflow_mechanism();
	this->delete_wave_runup_mechanism();
	this->delete_wind_wave_mechanism();
	this->delete_bursting_din_mechanism();
	this->delete_piping_lane_mechanism();
	this->delete_piping_sell_mechanism();
	this->delete_piping_schmertmann_mechanism();
	this->delete_slope_stability_landside_mechanism();
	this->delete_slope_stability_waterside_mechanism();
	this->delete_erosion_landside_mechanism();
	this->delete_erosion_waterside_mechanism();
	this->delete_start_erosion_waterside_mechanism();
	this->delete_wave_impact_mechanism();

	this->delete_material_zones();
	this->delete_material_variable_zone();
	this->delete_waterside_variable_zone();

	Sys_Memory_Count::self()->minus_mem((sizeof(Fpl_Sec_Type_Dike)-2*sizeof(Fpl_Random_Variables)-
		sizeof(Fpl_Cub_Dike_Waterside)-sizeof(Fpl_Cub_Dike_Crest)-sizeof(Fpl_Cub_Dike_Landside)-
		2*sizeof(Fpl_Seepage_Line_Point_List)-sizeof(Fpl_Dike_Geo_Foreland)-sizeof(Fpl_Dike_Geo_Hinterland)-
		sizeof(Geo_Simple_Polygon)-sizeof(Geo_Polysegment)-sizeof(Fpl_Seepage_Calculator_Dike)), _sys_system_modules::FPL_SYS);//count the memory
}
//_______________
//public
//Set the input of the dike section (geometrie, variables, faulttree)
void Fpl_Sec_Type_Dike::set_input(const int section_id,  const bool frc_sim, QSqlDatabase *ptr_database){
	//set the fault-tree per table and allocate the needed classe

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
void Fpl_Sec_Type_Dike::read_section_type_per_file(QFile *ifile, int *line_counter, const bool frc_sim){
	ostringstream cout;
	cout << "Input the general data of the dike section..."<<endl;
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
	cout << "Input of the general data of the dike section is finished"<<endl;
	Sys_Common_Output::output_fpl->output_txt(&cout);
}
//Read the mechanisms of the fault-tree per file
void Fpl_Sec_Type_Dike::read_mechanism_faulttree_perfile(QFile *ifile, int *line_counter){
	qint64 pos_file=0;
	int store_line=0;
	ostringstream cout;
	cout << "Input the mechanisms of the dike section..."<<endl;
	Sys_Common_Output::output_fpl->output_txt(&cout);

	//bursting
	try{
		pos_file=this->find_start_block_file(ifile, line_counter, fpl_label::begin_mech_burst_din, fpl_label::end_mech_burst_din, false);
		if(pos_file!=0){
			store_line=*line_counter;
			this->bursting_din_event->set_variables(ifile, line_counter, pos_file, store_line, this->get_ptr_specified_material_variable_zone(_fpl_zone_type::imperm_layer));
			this->bursting_din_event->set_2calc_flag(true);
		}
	}
	catch(Error msg){
		throw msg;
	}

	ifile->reset();
	*line_counter=0;
	Fpl_Calculation::check_stop_thread_flag();
	//piping
	try{
		//sellmeijer
		pos_file=this->find_start_block_file(ifile, line_counter, fpl_label::begin_mech_pip_sell, fpl_label::end_mech_pip_sell, false);
		if(pos_file!=0){
			store_line=*line_counter;
			this->piping_sell_event->set_variables(ifile, line_counter, pos_file, store_line, this->get_ptr_specified_material_variable_zone(_fpl_zone_type::perm_layer));
			this->piping_sell_event->set_2calc_flag(true);
		}
	}
	catch(Error msg){
		throw msg;
	}

	ifile->reset();
	*line_counter=0;
	Fpl_Calculation::check_stop_thread_flag();
	try{
		//lane
		pos_file=this->find_start_block_file(ifile, line_counter, fpl_label::begin_mech_pip_lane, fpl_label::end_mech_pip_lane, false);
		if(pos_file!=0){
			store_line=*line_counter;
			this->piping_lane_event->set_variables(ifile, line_counter, pos_file, store_line);
			this->piping_lane_event->set_2calc_flag(true);
		}
	}
	catch(Error msg){
		throw msg;
	}

	ifile->reset();
	*line_counter=0;
	Fpl_Calculation::check_stop_thread_flag();
	try{
		//schmertmann
		pos_file=this->find_start_block_file(ifile, line_counter, fpl_label::begin_mech_pip_schmert, fpl_label::end_mech_pip_schmert, false);
		if(pos_file!=0){
			store_line=*line_counter;
			this->piping_schmertmann_event->set_variables(ifile, line_counter, pos_file, store_line, this->get_ptr_specified_material_variable_zone(_fpl_zone_type::perm_layer), this->piping_sell_event);
			this->piping_schmertmann_event->set_2calc_flag(true);
		}
	}
	catch(Error msg){
		throw msg;
	}

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

	//slope
	try{
		pos_file=this->find_start_block_file(ifile, line_counter, fpl_label::begin_mech_slope, fpl_label::end_mech_slope, false);
		if(pos_file!=0){
			store_line=*line_counter;
			qint64 pos_file2=0;
			int store_line2=0;
			//slope landside
			pos_file2=this->find_start_block_file(ifile, line_counter, fpl_label::begin_slope_control_land, fpl_label::end_slope_control_land, false);
			if(pos_file2!=0){
				store_line2=*line_counter;
				this->slope_stability_landside_event->set_input(ifile, line_counter, pos_file, store_line);
				this->slope_stability_landside_event->set_2calc_flag(true);
			}
			Fpl_Calculation::check_stop_thread_flag();
			*line_counter=store_line;
			ifile->seek(pos_file);
			//slope waterside
			pos_file2=this->find_start_block_file(ifile, line_counter, fpl_label::begin_slope_control_water, fpl_label::end_slope_control_water, false);
			if(pos_file2!=0){
				store_line2=*line_counter;
				this->slope_stability_waterside_event->set_input(ifile, line_counter, pos_file, store_line);
				this->slope_stability_waterside_event->set_2calc_flag(true);
			}
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



	//waterside erosion start
	try{
		pos_file=this->find_start_block_file(ifile, line_counter, fpl_label::begin_mech_start_waterside_ero, fpl_label::end_mech_start_waterside_ero, false);
		if(pos_file!=0){
			store_line=*line_counter;
			this->erosion_waterside_start_event->set_variables(ifile, line_counter, pos_file, store_line);
			this->erosion_waterside_start_event->set_2calc_flag(true);
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
			this->erosion_waterside_event->set_variables(ifile, line_counter, pos_file, store_line);
			this->erosion_waterside_event->set_2calc_flag(true);
		}
	}
	catch(Error msg){
		throw msg;
	}
	ifile->reset();
	*line_counter=0;
	Fpl_Calculation::check_stop_thread_flag();

	//waterside wave impact
	try{
		pos_file=this->find_start_block_file(ifile, line_counter, fpl_label::begin_mech_wave_impact, fpl_label::end_mech_wave_impact, false);
		if(pos_file!=0){
			store_line=*line_counter;
			this->wave_impact_event->set_variables(ifile, line_counter, pos_file, store_line);
			this->wave_impact_event->set_2calc_flag(true);
		}
	}
	catch(Error msg){
		throw msg;
	}
	ifile->reset();
	*line_counter=0;
	Fpl_Calculation::check_stop_thread_flag();

	this->check_fault_tree();

	cout << "Input of the mechanisms of the dike section is finished"<<endl;
	Sys_Common_Output::output_fpl->output_txt(&cout);
}
//Check which mechanisms of the fault-tree are required per file
void Fpl_Sec_Type_Dike::check_mechanism_faulttree_perfile(QFile *ifile, int *line_counter){
	qint64 pos_file=0;
	int store_line=0;
	ostringstream cout;
	cout << "Check the mechanisms of the dike section..."<<endl;
	Sys_Common_Output::output_fpl->output_txt(&cout);

	//bursting
	try{
		pos_file=this->find_start_block_file(ifile, line_counter, fpl_label::begin_mech_burst_din, fpl_label::end_mech_burst_din, false);
		if(pos_file!=0){
			store_line=*line_counter;
			this->allocate_bursting_din_mechanism();
		}
	}
	catch(Error msg){
		throw msg;
	}
	ifile->reset();
	*line_counter=0;
	Fpl_Calculation::check_stop_thread_flag();
	//piping
	try{
		//sellmeijer
		pos_file=this->find_start_block_file(ifile, line_counter, fpl_label::begin_mech_pip_sell, fpl_label::end_mech_pip_sell, false);
		if(pos_file!=0){
			store_line=*line_counter;
			this->allocate_piping_sell_mechanism();
		}

		ifile->reset();
		*line_counter=0;
		Fpl_Calculation::check_stop_thread_flag();

		//lane
		pos_file=this->find_start_block_file(ifile, line_counter, fpl_label::begin_mech_pip_lane, fpl_label::end_mech_pip_lane, false);
		if(pos_file!=0){
			store_line=*line_counter;
			this->allocate_piping_lane_mechanism();
		}

		ifile->reset();
		*line_counter=0;
		Fpl_Calculation::check_stop_thread_flag();

		//schmertmann
		pos_file=this->find_start_block_file(ifile, line_counter, fpl_label::begin_mech_pip_schmert, fpl_label::end_mech_pip_schmert, false);
		if(pos_file!=0){
			store_line=*line_counter;
			this->allocate_piping_schmertmann_mechanism();
		}
	}
	catch(Error msg){
		throw msg;
	}
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

	//slope
	try{
		pos_file=this->find_start_block_file(ifile, line_counter, fpl_label::begin_mech_slope, fpl_label::end_mech_slope, false);
		if(pos_file!=0){
			store_line=*line_counter;
			qint64 pos_file2=0;
			int store_line2=0;
			//slope landside
			pos_file2=this->find_start_block_file(ifile, line_counter, fpl_label::begin_slope_control_land, fpl_label::end_slope_control_land, false);
			if(pos_file2!=0){
				store_line2=*line_counter;
				this->allocate_slope_stability_landside_mechanism();
			}
			Fpl_Calculation::check_stop_thread_flag();
			*line_counter=store_line;
			ifile->seek(pos_file);
			//slope waterside
			pos_file2=this->find_start_block_file(ifile, line_counter, fpl_label::begin_slope_control_water, fpl_label::end_slope_control_water, false);
			if(pos_file2!=0){
				store_line2=*line_counter;
				this->allocate_slope_stability_waterside_mechanism();
			}
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

	//erosion waterside start
	try{
		pos_file=this->find_start_block_file(ifile, line_counter, fpl_label::begin_mech_start_waterside_ero, fpl_label::end_mech_start_waterside_ero, false);
		if(pos_file!=0){
			store_line=*line_counter;
			this->allocate_start_erosion_waterside_mechanism();
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
			this->allocate_erosion_waterside_mechanism();
		}
	}
	catch(Error msg){
		throw msg;
	}
	ifile->reset();
	*line_counter=0;
	Fpl_Calculation::check_stop_thread_flag();

	//wave impact waterside
	try{
		pos_file=this->find_start_block_file(ifile, line_counter, fpl_label::begin_mech_wave_impact, fpl_label::end_mech_wave_impact, false);
		if(pos_file!=0){
			store_line=*line_counter;
			this->allocate_wave_impact_mechanism();
		}
	}
	catch(Error msg){
		throw msg;
	}
	ifile->reset();
	*line_counter=0;
	Fpl_Calculation::check_stop_thread_flag();

	cout << "Check of the mechanisms of the dike section is finished"<<endl;
	Sys_Common_Output::output_fpl->output_txt(&cout);
}
//Read the waterside geometry of the section per file
void Fpl_Sec_Type_Dike::read_waterside_geometry(QFile *ifile, int *line_counter){
	ostringstream cout;
	cout << "Input the waterside cubature geometry of the dike section..."<<endl;
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

	cout << "Input of the waterside cubature geometry of the dike section is finished"<<endl;
	Sys_Common_Output::output_fpl->output_txt(&cout);

	cout << "Input the waterside geometry of the dike section (foreland)..."<<endl;
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

	cout << "Input of the waterside geometry of the dike section (foreland) is finished"<<endl;
	Sys_Common_Output::output_fpl->output_txt(&cout);
}
//Read the landside geometry of the section per file
void Fpl_Sec_Type_Dike::read_landside_geometry(QFile *ifile, int *line_counter){
	ostringstream cout;
	cout << "Input the landside cubature geometry of the dike section..."<<endl;
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

	cout << "Input of the landside cubature geometry of the dike section is finished"<<endl;
	Sys_Common_Output::output_fpl->output_txt(&cout);

	cout << "Input the landside geometry of the dike section (hinterland)..."<<endl;
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

	cout << "Input of the landside geometry of the dike section (hinterland) is finished"<<endl;
	Sys_Common_Output::output_fpl->output_txt(&cout);
}
//Read the geometrical material zone(s) of the section per file
void Fpl_Sec_Type_Dike::read_geo_matzones(QFile *ifile, int *line_counter){
	ostringstream cout;
	cout << "Input the geometrical material zone(s) of the dike section..."<<endl;
	Sys_Common_Output::output_fpl->output_txt(&cout);
	qint64 pos_file=0;
	int store_line=0;
	try{
		pos_file=this->find_start_block_file(ifile, line_counter, fpl_label::begin_material_zone, fpl_label::end_material_zone, false);
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
			if(myline==fpl_label::end_material_zone){
				break;
			}
			if(myline.empty()==false){
				int pos=-1;
				string buffer;
				stringstream buffer1;
				bool wrong_input=false;

				pos=myline.find(fpl_label::key_no_zone);
				if(pos>=0 && wrong_input==false){
					buffer=myline.substr(fpl_label::key_no_zone.length());
					functions::clean_string(&buffer);
					buffer1 << buffer;
					buffer1 >> this->number_material_zones;
					if(buffer1.fail()==true){
						wrong_input=true;
					}
				}

				if(wrong_input==true){
					ostringstream info;
					info << "Wrong input sequenze  : " << buffer1.str() << endl;
					Error msg=this->set_error(13);
					msg.make_second_info(info.str());
					throw msg;
				}
			}
			if(this->number_material_zones>0){
				break;
			}
		}
		while(ifile->atEnd()==false);

		if(this->number_material_zones==0){
			if(this->slope_stability_landside_event==NULL && this->slope_stability_waterside_event==NULL){
				cout <<"No material zone(s) are found"  << endl ;
				Sys_Common_Output::output_fpl->output_txt(&cout,false);
				return;
			}
			else{
				Error msg=this->set_error(14);
				throw msg;
			}
		}

		//allocate the
		this->allocate_material_zones();

		//read in the zones
		for(int i=0; i< this->number_material_zones; i++){
			this->material_zones[i].set_zone_number(i);
			this->material_zones[i].read_matzone_per_file(ifile, line_counter);
		}
	}
	catch(Error msg){
		throw msg;
	}
}
//Read the material variable zone(s) of the section per file
void Fpl_Sec_Type_Dike::read_var_matzones(QFile *ifile, int *line_counter){
	ostringstream cout;
	cout << "Input the material variable zone(s) of the dike section..."<<endl;
	Sys_Common_Output::output_fpl->output_txt(&cout);
	qint64 pos_file=0;
	int store_line=0;
	try{
		pos_file=this->find_start_block_file(ifile, line_counter, fpl_label::begin_material_variable_zone, fpl_label::end_material_variable_zone, false);
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
			if(myline==fpl_label::end_material_variable_zone){
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
					buffer1 >> this->number_material_variable_zones;
					if(buffer1.fail()==true){
						wrong_input=true;
					}
				}

				if(wrong_input==true){
					ostringstream info;
					info << "Wrong input sequenze  : " << buffer1.str() << endl;
					Error msg=this->set_error(16);
					msg.make_second_info(info.str());
					throw msg;
				}
			}
			if(this->number_material_variable_zones>0){
				break;
			}
		}
		while(ifile->atEnd()==false);

		if(this->number_material_variable_zones==0){
			if(this->slope_stability_landside_event==NULL && this->slope_stability_waterside_event==NULL){
				cout <<"No material variable zone(s) are found... "  << endl ;
				Sys_Common_Output::output_fpl->output_txt(&cout,false);
				return;
			}
			else{
				Error msg=this->set_error(17);
				throw msg;
			}
		}

		//allocate the
		this->allocate_material_variable_zone();

		bool slope_required=true;
		if(this->slope_stability_landside_event!=NULL || this->slope_stability_waterside_event!=NULL){
			slope_required=true;
		}
		//look if the grain size distribution is required
		int grain_size_distri_index=-1;
		for(int i=0; i< this->number_material_zones; i++){
			if(this->material_zones[i].get_zone_type()==_fpl_zone_type::perm_layer){
				grain_size_distri_index=this->material_zones[i].get_variable_zone_number();
			}
		}

		//read in the zones
		for(int i=0; i< this->number_material_variable_zones; i++){
			this->material_variable_zones[i].read_matzone_per_file(ifile, line_counter, slope_required, grain_size_distri_index);
		}

		//connect zone
		this->connect_material_zones();
	}
	catch(Error msg){
		throw msg;
	}
	cout << "Input the material variable zone(s) of the dike section is finished!"<<endl;
	Sys_Common_Output::output_fpl->output_txt(&cout);
}
//Read the waterside material variable zone(s) of the section per file
void Fpl_Sec_Type_Dike::read_var_waterside_matzones(QFile *ifile, int *line_counter){
	ostringstream cout;
	cout << "Input the material variable zone(s) of the waterside dike cubature..."<<endl;
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
					Error msg=this->set_error(41);
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
			if(this->wave_runup_event==NULL && this->wave_impact_event==NULL && this->erosion_waterside_event==NULL){
				cout <<"No waterside material variable zone(s) are found... "  << endl ;
				Sys_Common_Output::output_fpl->output_txt(&cout,false);
				return;
			}
			else{
				Error msg=this->set_error(42);
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
		if(this->wave_impact_event!=NULL){
			wave_impact=true;
		}
		bool erosion=false;
		if(this->erosion_waterside_event!=NULL){
			erosion=true;
		}

		
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
	cout << "Input the material variable zone(s) of the waterside dike cubature is finished!"<<endl;
	Sys_Common_Output::output_fpl->output_txt(&cout);
}
//Transfer the section type data to database, e.g. geometry, random variables etc.
void Fpl_Sec_Type_Dike::transfer_sectiontype2database(const int section_id, QSqlDatabase *ptr_database){
	ostringstream cout;
	cout << "Transfer the section type data of a dike to database..."<<endl;
	Sys_Common_Output::output_fpl->output_txt(&cout);

	cout << "Transfer general random variables to database..."<<endl;
	Sys_Common_Output::output_fpl->output_txt(&cout);
	this->waterlevel.transfer_rand2database(ptr_database, this->system_id, section_id, fpl_label::mech_all, fpl_label::sec_dike);
	this->water_density.transfer_rand2database(ptr_database, this->system_id, section_id, fpl_label::mech_all, fpl_label::sec_dike);

	cout << "Transfer general geometry to database..."<<endl;
	Sys_Common_Output::output_fpl->output_txt(&cout);

	this->crest_land.transfer_point2database(this->system_id, section_id, ptr_database, fpl_label::region_cubature, fpl_label::sec_dike);
	this->base_land.transfer_point2database(this->system_id, section_id, ptr_database, fpl_label::region_cubature, fpl_label::sec_dike);

	this->foreland.transfer_geometry2database(section_id, this->system_id, ptr_database);
	this->waterside_cubature.transfer_geometry2database(section_id, this->system_id, ptr_database);
	this->landside_cubature.transfer_geometry2database(section_id, this->system_id, ptr_database);
	this->hinterland.transfer_geometry2database(section_id, this->system_id, ptr_database);

	cout << "Transfer "<<this->number_material_zones<<" geometrical material zone(s) to database..."<<endl;
	Sys_Common_Output::output_fpl->output_txt(&cout);
	//geometrical material zones
	for(int i=0; i< this->number_material_zones;i++){
		this->material_zones[i].transfer_matzone2database(ptr_database, section_id);
	}
	cout << "Transfer "<<this->number_material_variable_zones<<" material variable zone(s) to database..."<<endl;
	Sys_Common_Output::output_fpl->output_txt(&cout);
	//material zones
	for(int i=0; i< this->number_material_variable_zones;i++){
		this->material_variable_zones[i].transfer_data2database(section_id, this->system_id, ptr_database);
	}

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
	//bursting din10712
	if(this->bursting_din_event!=NULL){
		this->bursting_din_event->transfer_data2database(section_id,this->system_id, ptr_database, this->get_ptr_specified_material_variable_zone(_fpl_zone_type::imperm_layer));
	}
	//piping lane
	if(this->piping_lane_event!=NULL){
		this->piping_lane_event->transfer_data2database(section_id,this->system_id, ptr_database);
	}
	//piping sellmeijer
	if(this->piping_sell_event!=NULL){
		this->piping_sell_event->transfer_data2database(section_id,this->system_id, ptr_database, this->get_ptr_specified_material_variable_zone(_fpl_zone_type::perm_layer));
	}
	//piping sellmeijer
	if(this->piping_schmertmann_event!=NULL){
		this->piping_schmertmann_event->transfer_data2database(section_id,this->system_id, ptr_database, this->get_ptr_specified_material_variable_zone(_fpl_zone_type::perm_layer), this->piping_sell_event);
	}
	//slope landside
	if(this->slope_stability_landside_event!=NULL){
		this->slope_stability_landside_event->transfer_data2database(section_id,this->system_id, ptr_database);
	}
	//slope waterside
	if(this->slope_stability_waterside_event!=NULL){
		this->slope_stability_waterside_event->transfer_data2database(section_id,this->system_id, ptr_database);
	}

	//slope landside erosion
	if(this->erosion_landside_event!=NULL){
		this->erosion_landside_event->transfer_data2database(section_id,this->system_id, ptr_database);
	}

	//slope waterside erosion start
	if(this->erosion_waterside_start_event!=NULL){
		this->erosion_waterside_start_event->transfer_data2database(section_id,this->system_id, ptr_database);
	}

	//slope waterside erosion
	if(this->erosion_waterside_event!=NULL){
		this->erosion_waterside_event->transfer_data2database(section_id,this->system_id, ptr_database);
	}

	//slope wave impact waterside
	if(this->wave_impact_event!=NULL){
		this->wave_impact_event->transfer_data2database(section_id,this->system_id, ptr_database);
	}
}
//Calculate the fault tree with random distributed variables (true) or with deterministic variables (false)
int Fpl_Sec_Type_Dike::make_faulttree(const bool random_calculation){
	int buff_failure=0;
	if(random_calculation==true){
		//reset the random generated flag
		this->reset_random_flag();
	}

	//dike section type variables
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

	//calc the material zone variables
	for(int i=0; i< this->number_material_variable_zones; i++){
		this->material_variable_zones[i].generate_random_variables(random_calculation);
		this->material_variable_zones[i].calc_densities(this->water_density.get_result().end_result);
		this->material_variable_zones[i].calc_tan_friction();
	}

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

	this->seepage_calculator.calculate_waterlevel_seepage_line(this->current_waterlevel, &this->seepage_body_ascending, &this->seepage_body_descending, false);

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


	//slope stability landside
	bool slope_landside_failure_macro=false;
	bool slope_landside_failure_micro=false;
	if(this->slope_stability_landside_event!=NULL){
		if(this->slope_stability_landside_event->get_2calc_flag()==true){
			this->slope_stability_landside_event->calculate_mechanism(random_calculation,&this->seepage_body_ascending, this->water_density.get_result().end_result, &slope_landside_failure_macro, &slope_landside_failure_micro);
			if(slope_landside_failure_macro==true){
				this->slope_stability_landside_event->sum_up_counter_single_prob();
				if(total_is_set==false){
					this->slope_stability_landside_event->sum_up_counter_tree_prob();
					total_is_set=true;
				}
			}
			if(slope_landside_failure_micro==true){
				this->slope_stability_landside_event->sum_up_counter_single_prob_micro();
				if(total_is_set==false){
					this->slope_stability_landside_event->sum_up_counter_tree_prob_micro();
					total_is_set=true;
				}
			}
		}
	}

	//erosion water side start
	bool ero_water_start=false;
	if(this->erosion_waterside_start_event!=NULL){
		if(this->erosion_waterside_start_event->get_2calc_flag()==true){
			ero_water_start=erosion_waterside_start_event->calculate_mechanism(random_calculation, this->current_waterlevel, buff_krylowII.wave_height, this->section_direction, buff_krylowII.wind_direction, buff_krylowII.wave_period, this->water_density.get_result().end_result);
			if(ero_water_start==true){
				this->erosion_waterside_start_event->sum_up_counter_single_prob();
			}
		}
	}

	//erosion water side
	int water_ero_failure=0;
	if(this->erosion_waterside_event!=NULL){
		if(this->erosion_waterside_event->get_2calc_flag()==true){
			water_ero_failure=erosion_waterside_event->calculate_mechanism(random_calculation, this->current_waterlevel, buff_krylowII.wave_height, this->section_direction, buff_krylowII.wind_direction);
			if(water_ero_failure>0 && ero_water_start==true){
				this->erosion_waterside_event->sum_up_counter_single_prob();
				if(total_is_set==false){
					this->erosion_waterside_event->sum_up_counter_tree_prob();
					total_is_set=true;
				}
			}
			else{
				water_ero_failure=0;
			}
		}
	}

	//wave pressure impact water side
	int wave_impact_failure=0;
	if(this->wave_impact_event!=NULL){
		if(this->wave_impact_event->get_2calc_flag()==true){
			wave_impact_failure=wave_impact_event->calculate_mechanism(random_calculation, this->current_waterlevel, buff_krylowII.wave_height, this->water_density.get_result().end_result, this->section_direction, buff_krylowII.wind_direction, buff_krylowII.wave_period);
			if(wave_impact_failure>0){
				this->wave_impact_event->sum_up_counter_single_prob();
				if(total_is_set==false){
					this->wave_impact_event->sum_up_counter_tree_prob();
					total_is_set=true;
				}
			}
		}
	}

	//slope stability waterside
	bool slope_waterside_failure_macro=false;
	bool slope_waterside_failure_micro=false;
	if(this->slope_stability_waterside_event!=NULL){
		if(this->slope_stability_waterside_event->get_2calc_flag()==true){
			this->slope_stability_waterside_event->calculate_mechanism(random_calculation,&this->seepage_body_descending, this->water_density.get_result().end_result, &slope_waterside_failure_macro, &slope_waterside_failure_micro);
			if(slope_waterside_failure_macro==true){
				this->slope_stability_waterside_event->sum_up_counter_single_prob();
				if(total_is_set==false){
					this->slope_stability_waterside_event->sum_up_counter_tree_prob();
					total_is_set=true;
				}
			}
			if(slope_waterside_failure_micro==true){
				this->slope_stability_waterside_event->sum_up_counter_single_prob_micro();
				if(total_is_set==false){
					this->slope_stability_waterside_event->sum_up_counter_tree_prob_micro();
					total_is_set=true;
				}
			}
		}
	}

	//underground
	int buff_failure_underground=0;
	bool bursting_failure=false;
	//bursting din10712
	if(this->bursting_din_event!=NULL){
		if(this->bursting_din_event->get_2calc_flag()==true){
			bursting_failure=this->bursting_din_event->calculate_mechanism(random_calculation, this->seepage_body_ascending.get_list_point(this->index_h_imperm_layer)->get_waterlevel()-this->base_land.get_ycoordinate(), this->water_density.get_result().end_result, &this->seepage_body_ascending);
			if(bursting_failure==true){
				this->bursting_din_event->sum_up_counter_single_prob();
			}
		}
	}
	int pip_failure_lane=0;
	int pip_failure_sell=0;
	int pip_failure_schmertmann=0;
	//piping lane
	if(this->piping_lane_event!=NULL){
		if(this->piping_lane_event->get_2calc_flag()==true){
			pip_failure_lane=this->piping_lane_event->calculate_mechanism(random_calculation, this->seepage_body_ascending.get_list_point(this->index_h_imperm_layer)->get_waterlevel()-this->base_land.get_ycoordinate(), this->bursting_din_event->get_total_leak_length());
			if(pip_failure_lane>0 && bursting_failure==true){
				this->piping_lane_event->sum_up_counter_single_prob();
				if(total_is_set==false){
					this->piping_lane_event->sum_up_counter_tree_prob();
				}
				if(this->piping_lane_event->get_in_tree_flag()==false){
					pip_failure_lane=0;
				}
			}
		}
	}
	//piping sellmeijer
	if(this->piping_sell_event!=NULL ){
		if(this->piping_sell_event->get_2calc_flag()==true){
			pip_failure_sell=this->piping_sell_event->calculate_mechanism(random_calculation, this->seepage_body_ascending.get_list_point(this->index_h_imperm_layer)->get_waterlevel()-this->base_land.get_ycoordinate(), this->water_density.get_result().end_result, this->bursting_din_event->get_total_leak_length() );
			if(pip_failure_sell>0 && bursting_failure==true){
				this->piping_sell_event->sum_up_counter_single_prob();
				if(total_is_set==false){
					this->piping_sell_event->sum_up_counter_tree_prob();
				}
				if(this->piping_sell_event->get_in_tree_flag()==false){
					pip_failure_sell=0;
				}
			}
		}
	}
	//piping schmertmann
	if(this->piping_schmertmann_event!=NULL ){
		if(this->piping_schmertmann_event->get_2calc_flag()==true){
			pip_failure_schmertmann=this->piping_schmertmann_event->calculate_mechanism(random_calculation, this->seepage_body_ascending.get_list_point(this->index_h_imperm_layer)->get_waterlevel()-this->base_land.get_ycoordinate(), this->bursting_din_event->get_total_leak_length());
			if(pip_failure_schmertmann>0 && bursting_failure==true){
				this->piping_schmertmann_event->sum_up_counter_single_prob();
				if(total_is_set==false){
					this->piping_schmertmann_event->sum_up_counter_tree_prob();
				}
				if(this->piping_schmertmann_event->get_in_tree_flag()==false){
					pip_failure_schmertmann=0;
				}
			}
		}
	}

	if((bursting_failure ==true && pip_failure_sell==1)){
		buff_failure_underground=1;
	}
	if((bursting_failure ==true && pip_failure_lane==1)){
		buff_failure_underground=1;
	}
	if((bursting_failure ==true && pip_failure_schmertmann==1)){
		buff_failure_underground=1;
	}

	//summon the failure to one failure
	if(buff_failure==1 || buff_failure_underground==1 || slope_landside_failure_macro==true
		|| slope_waterside_failure_macro ==true || slope_landside_failure_micro==true || slope_waterside_failure_micro==true || wave_impact_failure==1 || water_ero_failure==1){
		buff_failure=1;
	}

	return buff_failure;
}
//Check the statistics of the random variables used in the section type
void Fpl_Sec_Type_Dike::check_statistic(void){
	this->waterlevel.check_statistic();
	this->water_density.check_statistic();

	//material variable zone
	for(int i=0; i< this->number_material_variable_zones; i++){
		this->material_variable_zones[i].check_statistic();
	}

	//calc the material zone variables of the waterside
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
	//bursting din10712
	if(this->bursting_din_event!=NULL){
		if(this->bursting_din_event->get_2calc_flag()==true){
			this->bursting_din_event->check_statistic();
		}
	}
	//piping lane
	if(this->piping_lane_event!=NULL){
		if(this->piping_lane_event->get_2calc_flag()==true){
			this->piping_lane_event->check_statistic();
		}
	}
	//piping sellmeijer
	if(this->piping_sell_event!=NULL){
		if(this->piping_sell_event->get_2calc_flag()==true){
			this->piping_sell_event->check_statistic();
		}
	}
	//piping sellmeijer
	if(this->piping_schmertmann_event!=NULL){
		if(this->piping_schmertmann_event->get_2calc_flag()==true){
			this->piping_schmertmann_event->check_statistic();
		}
	}

	//slope stability landside
	if(this->slope_stability_landside_event!=NULL){
		if(this->slope_stability_landside_event->get_2calc_flag()==true){
			this->slope_stability_landside_event->check_statistic();
		}
	}
	//slope stability waterside
	if(this->slope_stability_waterside_event!=NULL){
		if(this->slope_stability_waterside_event->get_2calc_flag()==true){
			this->slope_stability_waterside_event->check_statistic();
		}
	}

	//erosion waterside
	if(this->erosion_waterside_event!=NULL){
		if(this->erosion_waterside_event->get_2calc_flag()==true){
			this->erosion_waterside_event->check_statistic();
		}
	}

	//erosion waterside start event
	if(this->erosion_waterside_start_event!=NULL){
		if(this->erosion_waterside_start_event->get_2calc_flag()==true){
			this->erosion_waterside_start_event->check_statistic();
		}
	}

	//wave impact
	if(this->wave_impact_event!=NULL){
		if(this->wave_impact_event->get_2calc_flag()==true){
			this->wave_impact_event->check_statistic();
		}
	}
}
//Output the statistics of the random variables used in the section type to display/console
void Fpl_Sec_Type_Dike::output_statistic(void){
	ostringstream cout;
	cout << "Statistics of Variables (dike)..." << endl;
	Sys_Common_Output::output_fpl->output_txt(cout.str(),true);
	//dike section type members
	this->waterlevel.output_statistic();
	this->water_density.output_statistic();

	//The material variable zones
	for(int i=0; i< this->number_material_variable_zones; i++){
		this->material_variable_zones[i].output_statistic();
	}

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
	if(this->erosion_waterside_event!=NULL){
		if(this->erosion_waterside_event->get_2calc_flag()==true){
			this->erosion_waterside_event->output_statistic();
		}
	}
	//erosion waterside start event
	if(this->erosion_waterside_start_event!=NULL){
		if(this->erosion_waterside_start_event->get_2calc_flag()==true){
			this->erosion_waterside_start_event->output_statistic();
		}
	}
	//wave impact
	if(this->wave_impact_event!=NULL){
		if(this->wave_impact_event->get_2calc_flag()==true){
			this->wave_impact_event->output_statistic();
		}
	}

	//bursting din10712
	if(this->bursting_din_event!=NULL){
		if(this->bursting_din_event->get_2calc_flag()==true){
			this->bursting_din_event->output_statistic();
		}
	}
	//piping lane
	if(this->piping_lane_event!=NULL){
		if(this->piping_lane_event->get_2calc_flag()==true){
			this->piping_lane_event->output_statistic();
		}
	}
	//piping sellmeijer
	if(this->piping_sell_event!=NULL){
		if(this->piping_sell_event->get_2calc_flag()==true){
			this->piping_sell_event->output_statistic();
		}
	}

	//piping schmertmann
	if(this->piping_schmertmann_event!=NULL){
		if(this->piping_schmertmann_event->get_2calc_flag()==true){
			this->piping_schmertmann_event->output_statistic();
		}
	}

	//slope stability landside
	if(this->slope_stability_landside_event!=NULL){
		if(this->slope_stability_landside_event->get_2calc_flag()==true){
			this->slope_stability_landside_event->output_statistic();
		}
	}
	//slope stability waterside
	if(this->slope_stability_waterside_event!=NULL){
		if(this->slope_stability_waterside_event->get_2calc_flag()==true){
			this->slope_stability_waterside_event->output_statistic();
		}
	}

	
}
//Reset the statistic values
void Fpl_Sec_Type_Dike::reset_statistics(void){
	this->waterlevel.reset_statistics();
	this->water_density.reset_statistics();
	//material variable zone
	for(int i=0; i< this->number_material_variable_zones; i++){
		this->material_variable_zones[i].reset_statistic();
	}

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
	//bursting din10712
	if(this->bursting_din_event!=NULL){
		if(this->bursting_din_event->get_2calc_flag()==true){
			this->bursting_din_event->reset_statistics();
		}
	}
	//piping lane
	if(this->piping_lane_event!=NULL){
		if(this->piping_lane_event->get_2calc_flag()==true){
			this->piping_lane_event->reset_statistics();;
		}
	}
	//piping sellmeijer
	if(this->piping_sell_event!=NULL){
		if(this->piping_sell_event->get_2calc_flag()==true){
			this->piping_sell_event->reset_statistics();
		}
	}

	//piping schmertmann
	if(this->piping_schmertmann_event!=NULL){
		if(this->piping_schmertmann_event->get_2calc_flag()==true){
			this->piping_schmertmann_event->reset_statistics();
		}
	}

	//slope stability landside
	if(this->slope_stability_landside_event!=NULL){
		if(this->slope_stability_landside_event->get_2calc_flag()==true){
			this->slope_stability_landside_event->reset_statistics();
		}
	}
	//slope stability waterside
	if(this->slope_stability_waterside_event!=NULL){
		if(this->slope_stability_waterside_event->get_2calc_flag()==true){
			this->slope_stability_waterside_event->reset_statistics();
		}
	}

	//erosion waterside
	if(this->erosion_waterside_event!=NULL){
		if(this->erosion_waterside_event->get_2calc_flag()==true){
			this->erosion_waterside_event->reset_statistics();
		}
	}
	//erosion waterside start event
	if(this->erosion_waterside_start_event!=NULL){
		if(this->erosion_waterside_start_event->get_2calc_flag()==true){
			this->erosion_waterside_start_event->reset_statistics();
		}
	}
	//wave impact
	if(this->wave_impact_event!=NULL){
		if(this->wave_impact_event->get_2calc_flag()==true){
			this->wave_impact_event->reset_statistics();
		}
	}
}
//Output the section members to display/console
void Fpl_Sec_Type_Dike::output_member(void){
	this->seepage_calculator.output_members();

	ostringstream cout;
	ostringstream prefix;
	prefix << "GEO> ";
	//geometrie
	cout << "Geometrie members dike type..." << endl;
	Sys_Common_Output::output_fpl->output_txt(&cout,true);
	Sys_Common_Output::output_fpl->set_userprefix(&prefix);
	//main points of the dike
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

	//The material zones
	if(this->number_material_zones>0){
		cout <<"Output "<<this->number_material_zones<< " number of dike material zone(s)..."<<endl;
		Sys_Common_Output::output_fpl->output_txt(&cout,true);
		for(int i=0; i< this->number_material_zones; i++){
			this->material_zones[i].output_member();
		}
	}
	//the material zone variables of the waterside
	if(this->number_mat_waterside_zones>0){
		int buff_art=0;
		int buff_grass=0;
		this->count_waterside_zones(&buff_art, &buff_grass);
		cout <<"Output "<<this->number_mat_waterside_zones<< " number of dike material zone(s) of the waterside..."<<endl;
		if(this->erosion_waterside_event!=NULL || this->wave_impact_event!=NULL){
			cout <<" Number of artificial revetment : "<< buff_art<<endl;
			cout <<" Number of grass cover          : "<< buff_grass<<endl;
		}
		Sys_Common_Output::output_fpl->output_txt(&cout,true);
		for(int i=0; i< this->number_mat_waterside_zones; i++){
			this->material_waterside_zone[i].output_member();
		}
	}

	Sys_Common_Output::output_fpl->rewind_userprefix();

	//variables
	cout << "Variable members dike type..." << endl;
	Sys_Common_Output::output_fpl->output_txt(&cout,true);

	//section type members
	this->waterlevel.output_member();
	this->water_density.output_member();
	//The material variable zones
	if(this->number_material_variable_zones>0){
		cout <<"Output "<<this->number_material_variable_zones<< " number of dike material variable zone(s)..."<<endl;
		Sys_Common_Output::output_fpl->output_txt(&cout,true);
		for(int i=0; i< this->number_material_variable_zones; i++){
			this->material_variable_zones[i].output_member();
		}
	}

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
	//erosion waterside
	if(this->erosion_waterside_event!=NULL){
		this->erosion_waterside_event->output_members();
	}
	//erosion waterside start event
	if(this->erosion_waterside_start_event!=NULL){
		this->erosion_waterside_start_event->output_members();
	}
	//wave impact
	if(this->wave_impact_event!=NULL){
		this->wave_impact_event->output_members();
	}

	//bursting din10712
	if(this->bursting_din_event!=NULL){
		this->bursting_din_event->output_members();
	}
	//piping lane
	if(this->piping_lane_event!=NULL){
		this->piping_lane_event->output_members();
	}
	//piping sellmeijer
	if(this->piping_sell_event!=NULL){
		this->piping_sell_event->output_members();
	}

	//piping sellmeijer
	if(this->piping_schmertmann_event!=NULL){
		this->piping_schmertmann_event->output_members();
	}

	//slope stability landside
	if(this->slope_stability_landside_event!=NULL){
		this->slope_stability_landside_event->output_members();
	}
	//slope stability waterside
	if(this->slope_stability_waterside_event!=NULL){
		this->slope_stability_waterside_event->output_members();
	}

	
}
//Output the reliability of the fault tree mechanisms for a deterministic calculation to display/console
void Fpl_Sec_Type_Dike::output_reliability(string output_folder, const int sec_id, const string sec_name){
	ostringstream cout;
	ostringstream prefix;
	prefix << "RES"<<"> ";
	Sys_Common_Output::output_fpl->set_userprefix(prefix.str());
	cout << "Deterministic results of the sectiontype dike..." << endl;
	Sys_Common_Output::output_fpl->output_txt(&cout);
	if(this->low_waterlevel_flag==true){
		cout <<" There are no deterministic results due to low waterlevel"<<endl;
		Sys_Common_Output::output_fpl->output_txt(&cout);
        Sys_Common_Output::output_fpl->rewind_userprefix();
		return;
	}
	//general
	this->waterlevel.output_deterministic();
	this->water_density.output_deterministic();

	//The material variable zones
	for(int i=0; i< this->number_material_variable_zones; i++){
		this->material_variable_zones[i].output_determ_results();
	}

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
	if(this->erosion_waterside_start_event!=NULL){
		if(this->erosion_waterside_start_event->get_2calc_flag()==true){
			this->erosion_waterside_start_event->output_determ_results();
		}
	}
	//erosion waterside
	if(this->erosion_waterside_event!=NULL){
		if(this->erosion_waterside_event->get_2calc_flag()==true){
			this->erosion_waterside_event->output_determ_results();
		}
	}

	//wave impact
	if(this->wave_impact_event!=NULL){
		if(this->wave_impact_event->get_2calc_flag()==true){
			this->wave_impact_event->output_determ_results();
		}
	}


	//bursting din10712
	if(this->bursting_din_event!=NULL){
		if(this->bursting_din_event->get_2calc_flag()==true){
			this->bursting_din_event->output_determ_results();
		}
	}
	//piping lane
	if(this->piping_lane_event!=NULL){
		if(this->piping_lane_event->get_2calc_flag()==true){
			this->piping_lane_event->output_determ_results();
		}
	}
	//piping sellmeijer
	if(this->piping_sell_event!=NULL){
		if(this->piping_sell_event->get_2calc_flag()==true){
			this->piping_sell_event->output_determ_results();
		}
	}
	//piping schmertmann
	if(this->piping_schmertmann_event!=NULL){
		if(this->piping_schmertmann_event->get_2calc_flag()==true){
			this->piping_schmertmann_event->output_determ_results();
		}
	}
	//slope stability landside 
	bool header_done=false;
	int zone_counter=0;
	if(this->slope_stability_landside_event!=NULL){
		if (this->slope_stability_landside_event->get_2calc_flag() == true) {
			this->slope_stability_landside_event->output_determ_results();
			if (this->output_flags.tec_output == true) {
				//check folder
				string folder_buff;
				folder_buff = Fpl_Section::check_generate_folder("tecplot", &output_folder);
				//create name
				ostringstream buffer;
				buffer << folder_buff << "SLOPE_TEC_" << sec_id << "_" << sec_name << ".dat";
				Fpl_Mech_Slope_Stability_Dike::output_header2tecplot(buffer.str());
				header_done = true;
				this->slope_stability_landside_event->output_results2tecplot(buffer.str(), &zone_counter);
			}
			if (this->output_flags.para_output == true) {
				//check folder
				string folder_buff;
				folder_buff = Fpl_Section::check_generate_folder("paraview", &output_folder);
				//create name
				ostringstream buffer;
				buffer << folder_buff << "SLOPE_PARA_LS_" << sec_id << "_" << sec_name << ".csv";
				this->slope_stability_landside_event->output_results2paraview(buffer.str(), &zone_counter);
			}
			if (this->output_flags.excel_output == true) {
				//check folder
				string folder_buff;
				folder_buff = Fpl_Section::check_generate_folder("excel", &output_folder);
				//create name
				ostringstream buffer;
				buffer << folder_buff << "SLOPE_EXCEL_LS_" << sec_id << "_" << sec_name << ".csv";
				this->slope_stability_landside_event->output_results2excel(buffer.str(), &zone_counter);
			}
		}
	}
	//slope stability waterside
	if(this->slope_stability_waterside_event!=NULL){
		if(this->slope_stability_waterside_event->get_2calc_flag()==true){
			this->slope_stability_waterside_event->output_determ_results();
			if (this->output_flags.tec_output == true) {
				if (header_done == false) {
					//check folder
					string folder_buff;
					folder_buff = Fpl_Section::check_generate_folder("tecplot", &output_folder);
					//create name
					ostringstream buffer;
					buffer << folder_buff << "SLOPE_TEC_" << sec_id << "_" << sec_name << ".dat";
					Fpl_Mech_Slope_Stability_Dike::output_header2tecplot(buffer.str());
					this->slope_stability_waterside_event->output_results2tecplot(buffer.str(), &zone_counter);
				}
				else {
					//check folder
					string folder_buff;
					folder_buff = Fpl_Section::check_generate_folder("tecplot", &output_folder);
					//create name
					ostringstream buffer;
					buffer << folder_buff << "SLOPE_TEC_" << sec_id << "_" << sec_name << ".dat";
					this->slope_stability_waterside_event->output_results2tecplot(buffer.str(), &zone_counter);
				}
			}
			if (this->output_flags.para_output == true) {
				//check folder
				string folder_buff;
				folder_buff = Fpl_Section::check_generate_folder("paraview", &output_folder);
				//create name
				ostringstream buffer;
				buffer << folder_buff << "SLOPE_PARA_WS_" << sec_id << "_" << sec_name << ".csv";
				this->slope_stability_waterside_event->output_results2paraview(buffer.str(), &zone_counter);
			}
			if (this->output_flags.excel_output == true) {
				//check folder
				string folder_buff;
				folder_buff = Fpl_Section::check_generate_folder("excel", &output_folder);
				//create name
				ostringstream buffer;
				buffer << folder_buff << "SLOPE_EXCEL_WS_" << sec_id << "_" << sec_name << ".csv";
				this->slope_stability_waterside_event->output_results2excel(buffer.str(), &zone_counter);
			}
		}
	}
	if (this->output_flags.tec_output == true) {
		//check folder
		string folder_buff;
		folder_buff = Fpl_Section::check_generate_folder("tecplot", &output_folder);
		//create name
		ostringstream buffer;
		buffer << folder_buff << "SEEPAGE_TEC_" << sec_id << "_" << sec_name << ".dat";
		
		this->output_seepage2tecplot(buffer.str());
	}
	if (this->output_flags.para_output == true) {
		//check folder
		string folder_buff;
		folder_buff = Fpl_Section::check_generate_folder("paraview", &output_folder);
		//create name
		ostringstream buffer;
		buffer << folder_buff << "SEEPAGE_PARA_" << sec_id << "_" << sec_name << ".csv";
		
		this->output_seepage2paraview(buffer.str());
	}
	if (this->output_flags.excel_output == true) {
		//check folder
		string folder_buff;
		folder_buff = Fpl_Section::check_generate_folder("excel", &output_folder);
		//create name
		ostringstream buffer;
		buffer << folder_buff << "SEEPAGE_EXCEL_" << sec_id << "_" << sec_name << ".csv";
		
		this->output_seepage2excel(buffer.str());
	}

	Sys_Common_Output::output_fpl->rewind_userprefix();
}
//Output the geometry to tecplot 
void Fpl_Sec_Type_Dike::output_geometry2tecplot(ofstream *output){
	//output the zone header
	*output  << P(4) << FORMAT_FIXED_REAL;

	*output <<"#######zone geometry########"<<endl;
	for(int i=0; i< this->number_material_zones; i++){
		this->material_zones[i].output2tecplot(output);
	}
	*output<<endl;

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
void Fpl_Sec_Type_Dike::output_geometry2paraview(ofstream *output) {
	//output the zone header
	*output << P(4) << FORMAT_FIXED_REAL;
	//header
	*output << "x,";
	int count_col_tot = 1;
	if(this->foreland.get_number_segments() > 0) {
		*output << "foreland,";
		count_col_tot++;
	}
	if (this->waterside_cubature.get_number_segments() > 0) {
		*output << "waterside,";
		count_col_tot++;

	}
	if (this->crest_cubature.get_number_segments() > 0) {

		*output << "crest,";
		count_col_tot++;
	}

	if (this->landside_cubature.get_number_segments() > 0) {
		*output << "landside,";
		count_col_tot++;
	}
	if (this->hinterland.get_number_segments() > 0) {
		*output << "hinterland,";
		count_col_tot++;
	}
	for (int i = 0; i < this->number_material_zones; i++) {
		*output << this->material_zones[i].get_zone_number() << "_" << this->material_zones[i].get_zone_name();
		count_col_tot++;
		if (i < this->number_material_zones - 1) {
			*output << ",";
		}
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
		*output << this->foreland.get_segment(this->foreland.get_number_segments() - 1)->point2.get_ycoordinate() ;
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
		*output << this->crest_cubature.get_segment()->point1.get_ycoordinate() ;
		functions::add_seperator_csv(",NAN", output, counter_col_after);
		*output << endl;

		*output << this->crest_cubature.get_segment()->point2.get_xcoordinate() << ",";
		functions::add_seperator_csv("NAN,", output, counter_col_before);
		 *output << this->crest_cubature.get_segment()->point2.get_ycoordinate() ;
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
		*output<< this->landside_cubature.get_segment(this->landside_cubature.get_number_segments() - 1)->point2.get_ycoordinate();
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


	//MAt-zones
	for (int j = 0; j < this->number_material_zones; j++) {

		if (this->material_zones[j].get_number_segments() > 0) {
			counter_col_after--;

			for (int i = 0; i < this->material_zones[j].get_number_segments(); i++) {
				*output << this->material_zones[j].get_segment(i)->point1.get_xcoordinate() << ",";
				functions::add_seperator_csv("NAN,", output, counter_col_before);
				*output << this->material_zones[j].get_segment(i)->point1.get_ycoordinate();
				functions::add_seperator_csv(",NAN", output, counter_col_after);
				*output << endl;
			}

			*output << this->material_zones[j].get_segment(this->material_zones[j].get_number_segments() - 1)->point2.get_xcoordinate() << ",";
			functions::add_seperator_csv("NAN,", output, counter_col_before);
			*output << this->material_zones[j].get_segment(this->material_zones[j].get_number_segments() - 1)->point2.get_ycoordinate();
			functions::add_seperator_csv(",NAN", output, counter_col_after);
			*output << endl;
			counter_col_before++;
		}
		output->flush();
		
	}
	
	
}
//Output the geometry to excel
void Fpl_Sec_Type_Dike::output_geometry2excel(ofstream *output) {
	//output the zone header
	*output << P(4) << FORMAT_FIXED_REAL;
	//header
	*output << "x;";
	int count_col_tot = 1;
	if (this->foreland.get_number_segments() > 0) {
		*output << "foreland;";
		count_col_tot++;
	}
	if (this->waterside_cubature.get_number_segments() > 0) {
		*output << "waterside;";
		count_col_tot++;

	}
	if (this->crest_cubature.get_number_segments() > 0) {

		*output << "crest;";
		count_col_tot++;
	}

	if (this->landside_cubature.get_number_segments() > 0) {
		*output << "landside;";
		count_col_tot++;
	}
	if (this->hinterland.get_number_segments() > 0) {
		*output << "hinterland;";
		count_col_tot++;
	}
	for (int i = 0; i < this->number_material_zones; i++) {
		*output << this->material_zones[i].get_zone_number() << "_" << this->material_zones[i].get_zone_name();
		count_col_tot++;
		if (i < this->number_material_zones - 1) {
			*output << ";";
		}
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


	//MAt-zones
	for (int j = 0; j < this->number_material_zones; j++) {

		if (this->material_zones[j].get_number_segments() > 0) {
			counter_col_after--;

			for (int i = 0; i < this->material_zones[j].get_number_segments(); i++) {
				*output << this->material_zones[j].get_segment(i)->point1.get_xcoordinate() << ";";
				functions::add_seperator_csv(";", output, counter_col_before);
				*output << this->material_zones[j].get_segment(i)->point1.get_ycoordinate();
				functions::add_seperator_csv(";", output, counter_col_after);
				*output << endl;
			}

			*output << this->material_zones[j].get_segment(this->material_zones[j].get_number_segments() - 1)->point2.get_xcoordinate() << ";";
			functions::add_seperator_csv(";", output, counter_col_before);
			*output << this->material_zones[j].get_segment(this->material_zones[j].get_number_segments() - 1)->point2.get_ycoordinate();
			functions::add_seperator_csv(";", output, counter_col_after);
			*output << endl;
			counter_col_before++;
		}
		output->flush();

	}

}
//Output result members to database table
void Fpl_Sec_Type_Dike::output_result2table(QSqlDatabase *ptr_database, _fpl_simulation_type simulation_type, _sys_system_id id, const int section_id, const int counter_mc_sim){
	try{
		//delete existing data and set table
		Fpl_Sec_Type_Dike::set_result_table(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}
	//mysql query with the query-class
	QSqlQuery model(*ptr_database);
	//evaluate the global identifier
	int id_glob=Fpl_Sec_Type_Dike::result_table->maximum_int_of_column(Fpl_Sec_Type_Dike::result_table->get_column_name(fpl_label::glob_id),ptr_database)+1;

	//set the query via a query string
	ostringstream query_string;
	query_string << "INSERT INTO  " << Fpl_Sec_Type_Dike::result_table->get_table_name();
	query_string <<" ( ";
	query_string << Fpl_Sec_Type_Dike::result_table->get_column_name(fpl_label::glob_id) <<" , ";
	query_string << Fpl_Sec_Type_Dike::result_table->get_column_name(fpl_label::section_id) <<" , ";
	query_string << Fpl_Sec_Type_Dike::result_table->get_column_name(label::areastate_id) <<" , ";
	query_string << Fpl_Sec_Type_Dike::result_table->get_column_name(label::measure_id) <<" , ";
	query_string << Fpl_Sec_Type_Dike::result_table->get_column_name(fpl_label::simulation_type) <<" , ";
	//data
	query_string << Fpl_Sec_Type_Dike::result_table->get_column_name(fpl_label::waterlevel) <<" , ";
	query_string << Fpl_Sec_Type_Dike::result_table->get_column_name(fpl_label::height) <<" , ";
	//wind wave
	query_string << Fpl_Sec_Type_Dike::result_table->get_column_name(fpl_label::wave_height) <<" , ";
	query_string << Fpl_Sec_Type_Dike::result_table->get_column_name(fpl_label::wave_length) <<" , ";
	query_string << Fpl_Sec_Type_Dike::result_table->get_column_name(fpl_label::wave_period) <<" , ";
	query_string << Fpl_Sec_Type_Dike::result_table->get_column_name(fpl_label::wave_setup) <<" , ";
	query_string << Fpl_Sec_Type_Dike::result_table->get_column_name(fpl_label::mean_depth) <<" , ";
	//overflow
	query_string << Fpl_Sec_Type_Dike::result_table->get_column_name(fpl_label::mue_factor) <<" , ";
	query_string << Fpl_Sec_Type_Dike::result_table->get_column_name(fpl_label::q_overflow) <<" , ";
	query_string << Fpl_Sec_Type_Dike::result_table->get_column_name(fpl_label::tree_prob_overflow) <<" , ";
	query_string << Fpl_Sec_Type_Dike::result_table->get_column_name(fpl_label::single_prob_overflow) <<" , ";
	//wave runup
	query_string << Fpl_Sec_Type_Dike::result_table->get_column_name(fpl_label::gamma_wavedirection) <<" , ";
	query_string << Fpl_Sec_Type_Dike::result_table->get_column_name(fpl_label::gamma_berms) <<" , ";
	query_string << Fpl_Sec_Type_Dike::result_table->get_column_name(fpl_label::mid_gradient_waterside) <<" , ";
	query_string << Fpl_Sec_Type_Dike::result_table->get_column_name(fpl_label::height_runup) <<" , ";
	query_string << Fpl_Sec_Type_Dike::result_table->get_column_name(fpl_label::q_waveoverflow) <<" , ";
	query_string << Fpl_Sec_Type_Dike::result_table->get_column_name(fpl_label::tree_prob_runup) <<" , ";
	query_string << Fpl_Sec_Type_Dike::result_table->get_column_name(fpl_label::single_prob_runup) <<" , ";
	//erosion landside
	query_string << Fpl_Sec_Type_Dike::result_table->get_column_name(fpl_label::crit_landside_discharge) <<" , ";
	//wave impact
	query_string << Fpl_Sec_Type_Dike::result_table->get_column_name(fpl_label::res_wave_impact) <<" , ";
	query_string << Fpl_Sec_Type_Dike::result_table->get_column_name(fpl_label::stress_wave_impact) <<" , ";
	query_string << Fpl_Sec_Type_Dike::result_table->get_column_name(fpl_label::single_prob_wave_impact) <<" , ";
	query_string << Fpl_Sec_Type_Dike::result_table->get_column_name(fpl_label::tree_prob_wave_impact) <<" , ";
	//erosion waterside
	query_string << Fpl_Sec_Type_Dike::result_table->get_column_name(fpl_label::res_waterside_erosion) <<" , ";
	query_string << Fpl_Sec_Type_Dike::result_table->get_column_name(fpl_label::stress_waterside_erosion) <<" , ";
	query_string << Fpl_Sec_Type_Dike::result_table->get_column_name(fpl_label::single_prob_water_ero) <<" , ";
	query_string << Fpl_Sec_Type_Dike::result_table->get_column_name(fpl_label::tree_prob_water_ero) <<" , ";
	//erosion waterside start
	query_string << Fpl_Sec_Type_Dike::result_table->get_column_name(fpl_label::res_waterside_erosion_start) <<" , ";
	query_string << Fpl_Sec_Type_Dike::result_table->get_column_name(fpl_label::stress_waterside_erosion_start) <<" , ";
	query_string << Fpl_Sec_Type_Dike::result_table->get_column_name(fpl_label::single_prob_water_ero_start) <<" , ";
	
	//bursting din 19712
	query_string << Fpl_Sec_Type_Dike::result_table->get_column_name(fpl_label::res_burst_din) <<" , ";
	query_string << Fpl_Sec_Type_Dike::result_table->get_column_name(fpl_label::stress_burst_din) <<" , ";
	query_string << Fpl_Sec_Type_Dike::result_table->get_column_name(fpl_label::single_prob_bursting) <<" , ";
	//piping lane
	query_string << Fpl_Sec_Type_Dike::result_table->get_column_name(fpl_label::res_piping_lane) <<" , ";
	query_string << Fpl_Sec_Type_Dike::result_table->get_column_name(fpl_label::tree_prob_pip_lane) <<" , ";
	query_string << Fpl_Sec_Type_Dike::result_table->get_column_name(fpl_label::single_prob_pip_lane) <<" , ";
	//piping sellmeijer
	query_string << Fpl_Sec_Type_Dike::result_table->get_column_name(fpl_label::res_piping_sell) <<" , ";
	query_string << Fpl_Sec_Type_Dike::result_table->get_column_name(fpl_label::tree_prob_pip_sell) <<" , ";
	query_string << Fpl_Sec_Type_Dike::result_table->get_column_name(fpl_label::single_prob_pip_sell) <<" , ";
	//piping schmertmann
	query_string << Fpl_Sec_Type_Dike::result_table->get_column_name(fpl_label::res_piping_schmert) <<" , ";
	query_string << Fpl_Sec_Type_Dike::result_table->get_column_name(fpl_label::tree_prob_pip_schmert) <<" , ";
	query_string << Fpl_Sec_Type_Dike::result_table->get_column_name(fpl_label::single_prob_pip_schmert) <<" , ";
	//slope landside
	query_string << Fpl_Sec_Type_Dike::result_table->get_column_name(fpl_label::stab_slope_land_macro) <<" , ";
	query_string << Fpl_Sec_Type_Dike::result_table->get_column_name(fpl_label::stab_slope_land_micro) <<" , ";
	query_string << Fpl_Sec_Type_Dike::result_table->get_column_name(fpl_label::tree_prob_slope_land_macro) <<" , ";
	query_string << Fpl_Sec_Type_Dike::result_table->get_column_name(fpl_label::single_prob_slope_land_macro) <<" , ";
	query_string << Fpl_Sec_Type_Dike::result_table->get_column_name(fpl_label::tree_prob_slope_land_micro) <<" , ";
	query_string << Fpl_Sec_Type_Dike::result_table->get_column_name(fpl_label::single_prob_slope_land_micro) <<" , ";
	//slope waterside
	query_string << Fpl_Sec_Type_Dike::result_table->get_column_name(fpl_label::stab_slope_water_macro) <<" , ";
	query_string << Fpl_Sec_Type_Dike::result_table->get_column_name(fpl_label::stab_slope_water_micro) <<" , ";
	query_string << Fpl_Sec_Type_Dike::result_table->get_column_name(fpl_label::tree_prob_slope_water_macro) <<" , ";
	query_string << Fpl_Sec_Type_Dike::result_table->get_column_name(fpl_label::single_prob_slope_water_macro) <<" , ";
	query_string << Fpl_Sec_Type_Dike::result_table->get_column_name(fpl_label::tree_prob_slope_water_micro) <<" , ";
	query_string << Fpl_Sec_Type_Dike::result_table->get_column_name(fpl_label::single_prob_slope_water_micro) <<" ) ";

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



	//wave impact
	if(this->wave_impact_event!=NULL && this->wave_impact_event->get_2calc_flag()==true){
		query_string << this->wave_impact_event->resistance.get_calculated_mean()<< " , " ;
		query_string << this->wave_impact_event->stress.get_calculated_mean()<< " , " ;
		query_string << this->wave_impact_event->get_counter_single_prob()/(double)counter_mc_sim<< " , " ;
		query_string << this->wave_impact_event->get_counter_tree_prob()/(double)counter_mc_sim<< " , " ;
	}
	else{
		query_string << 0.0 << " , " ;
		query_string << 0.0 << " , " ;
		query_string << 0.0 << " , " ;
		query_string << 0.0 << " , " ;
	}
	
	//erosion waterside
	if(this->erosion_waterside_event!=NULL && this->erosion_waterside_event->get_2calc_flag()==true){
		query_string << this->erosion_waterside_event->resistance.get_calculated_mean()<< " , " ;
		query_string << this->erosion_waterside_event->stress.get_calculated_mean()<< " , " ;
		query_string << this->erosion_waterside_event->get_counter_single_prob()/(double)counter_mc_sim<< " , " ;
		query_string << this->erosion_waterside_event->get_counter_tree_prob()/(double)counter_mc_sim<< " , " ;
	}
	else{
		query_string << 0.0 << " , " ;
		query_string << 0.0 << " , " ;
		query_string << 0.0 << " , " ;
		query_string << 0.0 << " , " ;
	}

	//erosion waterside start
	if(this->erosion_waterside_start_event!=NULL && this->erosion_waterside_start_event->get_2calc_flag()==true){
		query_string << this->erosion_waterside_start_event->resistance.get_calculated_mean()<< " , " ;
		query_string << this->erosion_waterside_start_event->stress.get_calculated_mean()<< " , " ;
		query_string << this->erosion_waterside_start_event->get_counter_single_prob()/(double)counter_mc_sim<< " , " ;
	}
	else{
		query_string << 0.0 << " , " ;
		query_string << 0.0 << " , " ;
		query_string << 0.0 << " , " ;
	}
	


	//bursting Din 19712
	if(this->bursting_din_event!=NULL && this->bursting_din_event->get_2calc_flag()==true){
		query_string << this->bursting_din_event->resistance.get_calculated_mean()<< " , " ;
		query_string << this->bursting_din_event->stress.get_calculated_mean()<< " , " ;
		query_string << this->bursting_din_event->get_counter_single_prob()/(double)counter_mc_sim<< " , " ;
	}
	else{
		query_string << 0.0 << " , " ;
		query_string << 0.0 << " , " ;
		query_string << 0.0 << " , " ;
	}
	//piping lane
	if(this->piping_lane_event!=NULL && this->piping_lane_event->get_2calc_flag()==true){
		query_string << this->piping_lane_event->resistance.get_calculated_mean()<< " , " ;
		query_string << this->piping_lane_event->get_counter_tree_prob()/(double)counter_mc_sim<< " , " ;
		query_string << this->piping_lane_event->get_counter_single_prob()/(double)counter_mc_sim<< " , " ;
	}
	else{
		query_string << 0.0 << " , " ;
		query_string << 0.0 << " , " ;
		query_string << 0.0 << " , " ;
	}
	//piping sellmeijer
	if(this->piping_sell_event!=NULL && this->piping_sell_event->get_2calc_flag()==true){
		query_string << this->piping_sell_event->resistance.get_calculated_mean()<< " , " ;
		query_string << this->piping_sell_event->get_counter_tree_prob()/(double)counter_mc_sim<< " , " ;
		query_string << this->piping_sell_event->get_counter_single_prob()/(double)counter_mc_sim<< " , " ;
	}
	else{
		query_string << 0.0 << " , " ;
		query_string << 0.0 << " , " ;
		query_string << 0.0 << " , " ;
	}

	//piping schmertmann
	if(this->piping_schmertmann_event!=NULL && this->piping_schmertmann_event->get_2calc_flag()==true){
		query_string << this->piping_schmertmann_event->resistance.get_calculated_mean()<< " , " ;
		query_string << this->piping_schmertmann_event->get_counter_tree_prob()/(double)counter_mc_sim<< " , " ;
		query_string << this->piping_schmertmann_event->get_counter_single_prob()/(double)counter_mc_sim<< " , " ;
	}
	else{
		query_string << 0.0 << " , " ;
		query_string << 0.0 << " , " ;
		query_string << 0.0 << " , " ;
	}

	//slope landside
	if(this->slope_stability_landside_event!=NULL && this->slope_stability_landside_event->get_2calc_flag()==true){
		query_string << this->slope_stability_landside_event->stability_macro.get_calculated_mean()<< " , " ;
		query_string << this->slope_stability_landside_event->stability_micro.get_calculated_mean()<< " , " ;
		query_string << this->slope_stability_landside_event->get_counter_tree_prob()/(double)counter_mc_sim<< " , " ;
		query_string << this->slope_stability_landside_event->get_counter_single_prob()/(double)counter_mc_sim<< " , " ;
		query_string << this->slope_stability_landside_event->get_counter_tree_prob_micro()/(double)counter_mc_sim<< " , " ;
		query_string << this->slope_stability_landside_event->get_counter_single_prob_micro()/(double)counter_mc_sim<< " , " ;
	}
	else{
		query_string << 0.0 << " , " ;
		query_string << 0.0 << " , " ;
		query_string << 0.0 << " , " ;
		query_string << 0.0 << " , " ;
		query_string << 0.0 << " , " ;
		query_string << 0.0 << " , " ;
	}

	//slope waterside
	if(this->slope_stability_waterside_event!=NULL && this->slope_stability_waterside_event->get_2calc_flag()==true){
		query_string << this->slope_stability_waterside_event->stability_macro.get_calculated_mean()<< " , " ;
		query_string << this->slope_stability_waterside_event->stability_micro.get_calculated_mean()<< " , " ;
		query_string << this->slope_stability_waterside_event->get_counter_tree_prob()/(double)counter_mc_sim<< " , " ;
		query_string << this->slope_stability_waterside_event->get_counter_single_prob()/(double)counter_mc_sim<< " , " ;
		query_string << this->slope_stability_waterside_event->get_counter_tree_prob_micro()/(double)counter_mc_sim<< " , " ;
		query_string << this->slope_stability_waterside_event->get_counter_single_prob_micro()/(double)counter_mc_sim<< " ) " ;
	}
	else{
		query_string << 0.0 << " , " ;
		query_string << 0.0 << " , " ;
		query_string << 0.0 << " , " ;
		query_string << 0.0 << " , " ;
		query_string << 0.0 << " , " ;
		query_string << 0.0 << " ) " ;
	}

	Data_Base::database_request(&model, query_string.str(), ptr_database);

	if(model.lastError().isValid()){
		Warning msg=this->set_warning(1);
		ostringstream info;
		info << "Table Name                : " << Fpl_Sec_Type_Dike::result_table->get_table_name() << endl;
		info << "Table error info          : " << model.lastError().text().toStdString() << endl;
		info << "Fpl section id            : " << section_id << endl;
		msg.make_second_info(info.str());
		msg.output_msg(1);
	}
}
//Output the deterministic results to tecplot
void Fpl_Sec_Type_Dike::output_determ_res2tecplot(ofstream *output, QSqlDatabase *ptr_database, _sys_system_id id, const int section_id) {
	//header
	*output << "VARIABLES = " << endl;

	

	
	QSqlQueryModel model;
	int number = 0;
	number = Fpl_Sec_Type_Dike::select_results_in_database(&model, ptr_database, id, section_id, _fpl_simulation_type::sim_determ);

	for (int j = 6; j < model.columnCount(); j++) {
		*output << "\""<< (model.headerData(j, Qt::Horizontal, Qt::DisplayRole)).toString().toStdString() << "\"     " << endl;

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
void Fpl_Sec_Type_Dike::output_determ_res2paraview(ofstream *output, QSqlDatabase *ptr_database, _sys_system_id id, const int section_id) {
	//header

	//results
	QSqlQueryModel model;
	int number = 0;
	number = Fpl_Sec_Type_Dike::select_results_in_database(&model, ptr_database, id, section_id, _fpl_simulation_type::sim_determ);

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
void Fpl_Sec_Type_Dike::output_determ_res2excel(ofstream *output, QSqlDatabase *ptr_database, _sys_system_id id, const int section_id) {
	//header


	//results
	QSqlQueryModel model;
	int number = 0;
	number = Fpl_Sec_Type_Dike::select_results_in_database(&model, ptr_database, id, section_id, _fpl_simulation_type::sim_determ);

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
void Fpl_Sec_Type_Dike::output_mc_res2tecplot(ofstream *output, QSqlDatabase *ptr_database, _sys_system_id id, const int section_id) {
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
	number = Fpl_Sec_Type_Dike::select_results_in_database(&model, ptr_database, id, section_id, _fpl_simulation_type::sim_mc);

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
void Fpl_Sec_Type_Dike::output_mc_res2paraview(ofstream *output, QSqlDatabase *ptr_database, _sys_system_id id, const int section_id) {
	QSqlQueryModel model1;
	int number1 = 0;
	number1 = Fpl_Mc_Sim::select_results_in_database(&model1, ptr_database, id, section_id, true);
	for (int j = 6; j < model1.columnCount(); j++) {
		*output << (model1.headerData(j, Qt::Horizontal, Qt::DisplayRole)).toString().toStdString();
		if (j < model1.columnCount() ) { *output << ","; };

	}
	
	//results
	QSqlQueryModel model;
	int number = 0;
	number = Fpl_Sec_Type_Dike::select_results_in_database(&model, ptr_database, id, section_id, _fpl_simulation_type::sim_mc);

	for (int j = 7; j < model.columnCount(); j++) {
		*output << (model.headerData(j, Qt::Horizontal, Qt::DisplayRole)).toString().toStdString();
		if (j < model.columnCount() - 1) { *output << ","; };

	}
	*output << endl;

	//results
	for (int row = 0; row < model.rowCount(); row++) {
		for (int j = 6; j < model1.columnCount(); j++) {
			*output << (model1.data(model.index(row, j), Qt::DisplayRole)).toString().toStdString();
			if (j < model1.columnCount() ) { *output << ","; };
		}
		for (int j = 7; j < model.columnCount(); j++) {
			*output << (model.data(model.index(row, j), Qt::DisplayRole)).toString().toStdString();
			if (j < model.columnCount() - 1) { *output << ","; };
		}
		*output << endl;

	}
}
//Output the MC results to Excel
void Fpl_Sec_Type_Dike::output_mc_res2excel(ofstream *output, QSqlDatabase *ptr_database, _sys_system_id id, const int section_id) {
	QSqlQueryModel model1;
	int number1 = 0;
	number1 = Fpl_Mc_Sim::select_results_in_database(&model1, ptr_database, id, section_id, true);
	for (int j = 6; j < model1.columnCount(); j++) {
		*output << (model1.headerData(j, Qt::Horizontal, Qt::DisplayRole)).toString().toStdString();
		if (j < model1.columnCount() ) { *output << ";"; };

	}

	//results
	QSqlQueryModel model;
	int number = 0;
	number = Fpl_Sec_Type_Dike::select_results_in_database(&model, ptr_database, id, section_id, _fpl_simulation_type::sim_mc);

	for (int j = 7; j < model.columnCount(); j++) {
		*output << (model.headerData(j, Qt::Horizontal, Qt::DisplayRole)).toString().toStdString() ;
		if (j < model.columnCount() - 1) { *output << ";"; };

	}
	*output << endl;

	//results
	for (int row = 0; row < model.rowCount(); row++) {
		for (int j = 6; j < model1.columnCount(); j++) {
			*output << (model1.data(model.index(row, j), Qt::DisplayRole)).toString().toStdString();
			if (j < model1.columnCount() ) { *output << ";"; };
		}
		for (int j = 7; j < model.columnCount(); j++) {
			*output << (model.data(model.index(row, j), Qt::DisplayRole)).toString().toStdString() ;
			if (j < model.columnCount() - 1) { *output << ";"; };
		}
		*output << endl;

	}
}
//Output the FRC results to tecplot
void Fpl_Sec_Type_Dike::output_frc_res2tecplot(ofstream *output, QSqlDatabase *ptr_database, _sys_system_id id, const int section_id) {
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
	number = Fpl_Sec_Type_Dike::select_results_in_database(&model, ptr_database, id, section_id, _fpl_simulation_type::sim_frc);

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
void Fpl_Sec_Type_Dike::output_frc_res2paraview(ofstream *output, QSqlDatabase *ptr_database, _sys_system_id id, const int section_id) {
	QSqlQueryModel model1;
	int number1 = 0;
	number1 = Fpl_Mc_Sim::select_results_in_database(&model1, ptr_database, id, section_id, false);
	for (int j = 6; j < model1.columnCount(); j++) {
		*output << (model1.headerData(j, Qt::Horizontal, Qt::DisplayRole)).toString().toStdString() ;
		if (j < model1.columnCount())  { *output << ","; };

	}

	//results
	QSqlQueryModel model;
	int number = 0;
	number = Fpl_Sec_Type_Dike::select_results_in_database(&model, ptr_database, id, section_id, _fpl_simulation_type::sim_frc);

	for (int j = 7; j < model.columnCount(); j++) {
		*output << (model.headerData(j, Qt::Horizontal, Qt::DisplayRole)).toString().toStdString() ;
		if (j < model.columnCount() - 1) { *output << ","; };

	}
	*output << endl;

	//results
	for (int row = 0; row < model.rowCount(); row++) {
		for (int j = 6; j < model1.columnCount(); j++) {
			*output << (model1.data(model.index(row, j), Qt::DisplayRole)).toString().toStdString();
			if (j < model1.columnCount() ) { *output << ","; };
		}
		for (int j = 7; j < model.columnCount(); j++) {
			*output << (model.data(model.index(row, j), Qt::DisplayRole)).toString().toStdString() ;
			if (j < model.columnCount() - 1) { *output << ","; };
		}
		*output << endl;
		output->flush();

	}
}
//Output the FRC results to Excel
void Fpl_Sec_Type_Dike::output_frc_res2excel(ofstream *output, QSqlDatabase *ptr_database, _sys_system_id id, const int section_id) {
	QSqlQueryModel model1;
	int number1 = 0;
	number1 = Fpl_Mc_Sim::select_results_in_database(&model1, ptr_database, id, section_id, false);
	for (int j = 6; j < model1.columnCount(); j++) {
		*output << (model1.headerData(j, Qt::Horizontal, Qt::DisplayRole)).toString().toStdString() ;
		if (j < model1.columnCount() ) { *output << ";"; };

	}

	//results
	QSqlQueryModel model;
	int number = 0;
	number = Fpl_Sec_Type_Dike::select_results_in_database(&model, ptr_database, id, section_id, _fpl_simulation_type::sim_frc);

	for (int j = 7; j < model.columnCount(); j++) {
		*output << (model.headerData(j, Qt::Horizontal, Qt::DisplayRole)).toString().toStdString();
		if (j < model.columnCount() - 1) { *output << ";"; };

	}
	*output << endl;

	//results
	for (int row = 0; row < model.rowCount(); row++) {
		for (int j = 6; j < model1.columnCount(); j++) {
			*output << (model1.data(model.index(row, j), Qt::DisplayRole)).toString().toStdString();
			if (j < model1.columnCount() ) { *output << ";"; };
		}
		for (int j = 7; j < model.columnCount(); j++) {
			*output << (model.data(model.index(row, j), Qt::DisplayRole)).toString().toStdString() ;
			if (j < model.columnCount() - 1) { *output << ";"; };
		}
		*output << endl;
		output->flush();

	}
}
//Create the database table for the results of the mechanism of the dike section (static)
void Fpl_Sec_Type_Dike::create_result_table(QSqlDatabase *ptr_database){
		if(Fpl_Sec_Type_Dike::result_table==NULL){
			ostringstream cout;
			cout << "Create dike result database table..." << endl ;
			Sys_Common_Output::output_fpl->output_txt(&cout);
			//make specific input for this class
			const string tab_name=fpl_label::tab_fpl_dike_result;
			const int num_col=62;
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



			tab_col[21].name=fpl_label::res_waterside_erosion_start;
			tab_col[21].type=sys_label::tab_col_type_double;
			tab_col[21].unsigned_flag=true;

			tab_col[22].name=fpl_label::stress_waterside_erosion_start;
			tab_col[22].type=sys_label::tab_col_type_double;
			tab_col[22].unsigned_flag=true;


			tab_col[23].name=fpl_label::res_waterside_erosion;
			tab_col[23].type=sys_label::tab_col_type_double;
			tab_col[23].unsigned_flag=true;

			tab_col[24].name=fpl_label::stress_waterside_erosion;
			tab_col[24].type=sys_label::tab_col_type_double;
			tab_col[24].unsigned_flag=true;

			tab_col[25].name=fpl_label::res_wave_impact;
			tab_col[25].type=sys_label::tab_col_type_double;
			tab_col[25].unsigned_flag=true;

			tab_col[26].name=fpl_label::stress_wave_impact;
			tab_col[26].type=sys_label::tab_col_type_double;
			tab_col[26].unsigned_flag=true;



			tab_col[27].name=fpl_label::res_burst_din;
			tab_col[27].type=sys_label::tab_col_type_double;
			tab_col[27].unsigned_flag=true;

			tab_col[28].name=fpl_label::stress_burst_din;
			tab_col[28].type=sys_label::tab_col_type_double;
			tab_col[28].unsigned_flag=true;

			tab_col[29].name=fpl_label::res_piping_lane;
			tab_col[29].type=sys_label::tab_col_type_double;
			tab_col[29].unsigned_flag=true;

			tab_col[30].name=fpl_label::res_piping_sell;
			tab_col[30].type=sys_label::tab_col_type_double;
			tab_col[30].unsigned_flag=true;

			tab_col[31].name=fpl_label::res_piping_schmert;
			tab_col[31].type=sys_label::tab_col_type_double;
			tab_col[31].unsigned_flag=true;

			tab_col[32].name=fpl_label::stab_slope_land_macro;
			tab_col[32].type=sys_label::tab_col_type_double;
			tab_col[32].unsigned_flag=true;

			tab_col[33].name=fpl_label::stab_slope_land_micro;
			tab_col[33].type=sys_label::tab_col_type_double;
			tab_col[33].unsigned_flag=true;

			tab_col[34].name=fpl_label::stab_slope_water_macro;
			tab_col[34].type=sys_label::tab_col_type_double;
			tab_col[34].unsigned_flag=true;

			tab_col[35].name=fpl_label::stab_slope_water_micro;
			tab_col[35].type=sys_label::tab_col_type_double;
			tab_col[35].unsigned_flag=true;

			tab_col[36].name=fpl_label::single_prob_overflow;
			tab_col[36].type=sys_label::tab_col_type_double;
			tab_col[36].unsigned_flag=true;

			tab_col[37].name=fpl_label::tree_prob_overflow;
			tab_col[37].type=sys_label::tab_col_type_double;
			tab_col[37].unsigned_flag=true;

			tab_col[38].name=fpl_label::single_prob_runup;
			tab_col[38].type=sys_label::tab_col_type_double;
			tab_col[38].unsigned_flag=true;

			tab_col[39].name=fpl_label::tree_prob_runup;
			tab_col[39].type=sys_label::tab_col_type_double;
			tab_col[39].unsigned_flag=true;


			tab_col[40].name=fpl_label::single_prob_water_ero_start;
			tab_col[40].type=sys_label::tab_col_type_double;
			tab_col[40].unsigned_flag=true;


			tab_col[41].name=fpl_label::single_prob_water_ero;
			tab_col[41].type=sys_label::tab_col_type_double;
			tab_col[41].unsigned_flag=true;

			tab_col[42].name=fpl_label::tree_prob_water_ero;
			tab_col[42].type=sys_label::tab_col_type_double;
			tab_col[42].unsigned_flag=true;

			tab_col[43].name=fpl_label::single_prob_wave_impact;
			tab_col[43].type=sys_label::tab_col_type_double;
			tab_col[43].unsigned_flag=true;

			tab_col[44].name=fpl_label::tree_prob_wave_impact;
			tab_col[44].type=sys_label::tab_col_type_double;
			tab_col[44].unsigned_flag=true;
			


			tab_col[45].name=fpl_label::single_prob_bursting;
			tab_col[45].type=sys_label::tab_col_type_double;
			tab_col[45].unsigned_flag=true;

			tab_col[46].name=fpl_label::single_prob_pip_sell;
			tab_col[46].type=sys_label::tab_col_type_double;
			tab_col[46].unsigned_flag=true;

			tab_col[47].name=fpl_label::tree_prob_pip_sell;
			tab_col[47].type=sys_label::tab_col_type_double;
			tab_col[47].unsigned_flag=true;

			tab_col[48].name=fpl_label::single_prob_pip_lane;
			tab_col[48].type=sys_label::tab_col_type_double;
			tab_col[48].unsigned_flag=true;

			tab_col[49].name=fpl_label::tree_prob_pip_lane;
			tab_col[49].type=sys_label::tab_col_type_double;
			tab_col[49].unsigned_flag=true;

			tab_col[50].name=fpl_label::single_prob_pip_schmert;
			tab_col[50].type=sys_label::tab_col_type_double;
			tab_col[50].unsigned_flag=true;
			tab_col[50].default_value="0.0";

			tab_col[51].name=fpl_label::tree_prob_pip_schmert;
			tab_col[51].type=sys_label::tab_col_type_double;
			tab_col[51].unsigned_flag=true;
			tab_col[51].default_value="0.0";

			tab_col[52].name=fpl_label::single_prob_slope_land_macro;
			tab_col[52].type=sys_label::tab_col_type_double;
			tab_col[52].unsigned_flag=true;

			tab_col[53].name=fpl_label::tree_prob_slope_land_macro;
			tab_col[53].type=sys_label::tab_col_type_double;
			tab_col[53].unsigned_flag=true;

			tab_col[54].name=fpl_label::single_prob_slope_land_micro;
			tab_col[54].type=sys_label::tab_col_type_double;
			tab_col[54].unsigned_flag=true;

			tab_col[55].name=fpl_label::tree_prob_slope_land_micro;
			tab_col[55].type=sys_label::tab_col_type_double;
			tab_col[55].unsigned_flag=true;

			tab_col[56].name=fpl_label::single_prob_slope_water_macro;
			tab_col[56].type=sys_label::tab_col_type_double;
			tab_col[56].unsigned_flag=true;

			tab_col[57].name=fpl_label::tree_prob_slope_water_macro;
			tab_col[57].type=sys_label::tab_col_type_double;
			tab_col[57].unsigned_flag=true;

			tab_col[58].name=fpl_label::single_prob_slope_water_micro;
			tab_col[58].type=sys_label::tab_col_type_double;
			tab_col[58].unsigned_flag=true;

			tab_col[59].name=fpl_label::tree_prob_slope_water_micro;
			tab_col[59].type=sys_label::tab_col_type_double;
			tab_col[59].unsigned_flag=true;

			tab_col[60].name=fpl_label::single_prob_seepage;
			tab_col[60].type=sys_label::tab_col_type_double;
			tab_col[60].unsigned_flag=true;
			tab_col[60].default_value="0.0";

			tab_col[61].name=fpl_label::tree_prob_seepage;
			tab_col[61].type=sys_label::tab_col_type_double;
			tab_col[61].unsigned_flag=true;
			tab_col[61].default_value="0.0";

			try{
				Fpl_Sec_Type_Dike::result_table= new Tables();
				if(Fpl_Sec_Type_Dike::result_table->create_non_existing_tables(tab_name, tab_col, num_col,ptr_database, _sys_table_type::fpl)==false){
					cout << " Table exists" << endl ;
					Sys_Common_Output::output_fpl->output_txt(&cout);
				};
			}
			catch(bad_alloc& t){
				Error msg;
				msg.set_msg("Fpl_Sec_Type_Dike::create_result_table(QSqlDatabase *ptr_database)","Can not allocate the memory", "Check the memory", 10, false);
				ostringstream info;
				info<< "Info bad alloc: " << t.what() << endl;
				msg.make_second_info(info.str());
				throw msg;
			}
			catch(Error msg){
				Fpl_Sec_Type_Dike::close_result_table();
				throw msg;
			}

		Fpl_Sec_Type_Dike::close_result_table();
	}
}
//Delete all data in the database table for the results of the mechanism of the dike section (static)
void Fpl_Sec_Type_Dike::delete_data_in_result_table(QSqlDatabase *ptr_database){
	//the table is set (the name and the column names) and allocated
	try{
		Fpl_Sec_Type_Dike::set_result_table(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}
	//delete the table
	Fpl_Sec_Type_Dike::result_table->delete_data_in_table(ptr_database);
}
//Delete a data set in the database table for the results of the mechanism of the dike section for a specific system state (static)
void Fpl_Sec_Type_Dike::delete_data_in_result_table(QSqlDatabase *ptr_database, const _sys_system_id id){
	//the table is set (the name and the column names) and allocated
	try{
		Fpl_Sec_Type_Dike::set_result_table(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}

	//delete the table
	QSqlQuery query(*ptr_database);
	ostringstream query_string;
	query_string <<"DELETE FROM " << Fpl_Sec_Type_Dike::result_table->get_table_name();
	query_string << " WHERE " ;
	query_string << Fpl_Sec_Type_Dike::result_table->get_column_name(label::areastate_id) << " ="  << id.area_state;
	query_string << " AND ";
	query_string << Fpl_Sec_Type_Dike::result_table->get_column_name(label::measure_id) << " = " << id.measure_nr;

	Data_Base::database_request(&query, query_string.str(), ptr_database);
	if(query.lastError().isValid()==true){
		Error msg;
		msg.set_msg("Fpl_Sec_Type_Dike::delete_data_in_result_table(QSqlDatabase *ptr_database,  const _sys_system_id id)","Invalid database request", "Check the database", 2, false);
		ostringstream info;
		info << "Table Name      : " << Fpl_Sec_Type_Dike::result_table->get_table_name() << endl;
		info << "Table error info: " << query.lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
}
//Delete a data set in the database table for the results of the mechanism of the dike section (static)
void Fpl_Sec_Type_Dike::delete_data_in_result_table(QSqlDatabase *ptr_database, const int section_id, const _sys_system_id id, const _fpl_simulation_type sim_type){
	//the table is set (the name and the column names) and allocated
	try{
		Fpl_Sec_Type_Dike::set_result_table(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}
	//delete the table
	QSqlQuery query(*ptr_database);
	ostringstream query_string;
	query_string <<"DELETE FROM " << Fpl_Sec_Type_Dike::result_table->get_table_name();
	query_string << " WHERE " ;
	query_string << Fpl_Sec_Type_Dike::result_table->get_column_name(label::areastate_id) << " ="  << id.area_state;
	query_string << " AND ";
	query_string << Fpl_Sec_Type_Dike::result_table->get_column_name(label::measure_id) << " = " << id.measure_nr;
	query_string << " AND ";
	query_string << Fpl_Sec_Type_Dike::result_table->get_column_name(fpl_label::section_id) << " = " << section_id;
	query_string << " AND ";
	query_string << Fpl_Sec_Type_Dike::result_table->get_column_name(fpl_label::simulation_type) << " = '" << Fpl_Mc_Sim::convert_sim_type2text(sim_type)<<"'";

	Data_Base::database_request(&query, query_string.str(), ptr_database);
	if(query.lastError().isValid()==true){
		Error msg;
		msg.set_msg("Fpl_Sec_Type_Dike::delete_data_in_result_table(QSqlDatabase *ptr_database, const int section_id, const _sys_system_id id, const _fpl_simulation_type sim_type)","Invalid database request", "Check the database", 2, false);
		ostringstream info;
		info << "Table Name      : " << Fpl_Sec_Type_Dike::result_table->get_table_name() << endl;
		info << "Table error info: " << query.lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
}
//Set the database table for the results of the mechanism of the dike section : it sets the table name and the name of the columns and allocate them (static)
void Fpl_Sec_Type_Dike::set_result_table(QSqlDatabase *ptr_database, const bool not_close){
	if(Fpl_Sec_Type_Dike::result_table==NULL){
		//make specific input for this class
		const string tab_id_name=fpl_label::tab_fpl_dike_result;
		string tab_col[62];

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
		tab_col[19]=fpl_label::res_burst_din;
		tab_col[20]=fpl_label::stress_burst_din;
		tab_col[21]=fpl_label::res_piping_lane;
		tab_col[22]=fpl_label::res_piping_sell;
		tab_col[23]=label::applied_flag;
		tab_col[24]=fpl_label::stab_slope_land_macro;
		tab_col[25]=fpl_label::stab_slope_land_micro;
		tab_col[26]=fpl_label::stab_slope_water_macro;
		tab_col[27]=fpl_label::stab_slope_water_micro;
		tab_col[28]=fpl_label::single_prob_overflow;
		tab_col[29]=fpl_label::tree_prob_overflow;
		tab_col[30]=fpl_label::single_prob_runup;
		tab_col[31]=fpl_label::tree_prob_runup;
		tab_col[32]=fpl_label::single_prob_bursting;
		tab_col[33]=fpl_label::single_prob_pip_sell;
		tab_col[34]=fpl_label::tree_prob_pip_sell;
		tab_col[35]=fpl_label::single_prob_pip_lane;
		tab_col[36]=fpl_label::tree_prob_pip_lane;
		tab_col[37]=fpl_label::single_prob_slope_land_macro;
		tab_col[38]=fpl_label::tree_prob_slope_land_macro;
		tab_col[39]=fpl_label::single_prob_slope_water_macro;
		tab_col[40]=fpl_label::tree_prob_slope_water_macro;

		tab_col[41]=fpl_label::single_prob_pip_schmert;
		tab_col[42]=fpl_label::tree_prob_pip_schmert;
		tab_col[43]=fpl_label::single_prob_seepage;
		tab_col[44]=fpl_label::tree_prob_seepage;

		tab_col[45]=fpl_label::single_prob_slope_land_micro;
		tab_col[46]=fpl_label::tree_prob_slope_land_micro;
		tab_col[47]=fpl_label::single_prob_slope_water_micro;
		tab_col[48]=fpl_label::tree_prob_slope_water_micro;
		tab_col[49]=fpl_label::res_piping_schmert;

		tab_col[50]=fpl_label::crit_landside_discharge;

		tab_col[51]=fpl_label::single_prob_water_ero;
		tab_col[52]=fpl_label::tree_prob_water_ero;
		tab_col[53]=fpl_label::single_prob_wave_impact;
		tab_col[54]=fpl_label::tree_prob_wave_impact;

		tab_col[55]=fpl_label::res_waterside_erosion;
		tab_col[56]=fpl_label::stress_waterside_erosion;
		tab_col[57]=fpl_label::res_wave_impact;
		tab_col[58]=fpl_label::stress_wave_impact;

		tab_col[59]=fpl_label::single_prob_water_ero_start;
		tab_col[60]=fpl_label::res_waterside_erosion_start;
		tab_col[61]=fpl_label::stress_waterside_erosion_start;

			
		try{
			Fpl_Sec_Type_Dike::result_table= new Tables(tab_id_name, tab_col, sizeof(tab_col)/sizeof(tab_col[0]));
			Fpl_Sec_Type_Dike::result_table->set_name(ptr_database, _sys_table_type::fpl);
		}
		catch(bad_alloc&t ){
			Error msg;
			msg.set_msg("void Fpl_Sec_Type_Dike::set_result_table(QSqlDatabase *ptr_database)","Can not allocate the memory", "Check the memory", 10, false);
			ostringstream info;
			info<< "Info bad alloc: " << t.what() << endl;
			msg.make_second_info(info.str());
			throw msg;
		}
		catch(Error msg){
			if(not_close==false){
				Fpl_Sec_Type_Dike::close_result_table();
			}
			throw msg;
		}
	}
}
//Select results of given fpl section in a database table (static)
int Fpl_Sec_Type_Dike::select_results_in_database(QSqlQueryModel *results, QSqlDatabase *ptr_database, const _sys_system_id id, const int sec_id, const _fpl_simulation_type sim_type){
    int number=0;
    try{
        Fpl_Sec_Type_Dike::set_result_table(ptr_database);
    }
    catch(Error msg){
        throw msg;
    }
    ostringstream test_filter;
    test_filter<< "SELECT *";
    test_filter << " FROM " << Fpl_Sec_Type_Dike::result_table->get_table_name();
    test_filter << " WHERE " ;
    test_filter << Fpl_Sec_Type_Dike::result_table->get_column_name(fpl_label::section_id) << " = " << sec_id;
    test_filter << " AND ";
    test_filter << Fpl_Sec_Type_Dike::result_table->get_column_name(label::applied_flag) << "= true";
    test_filter << " AND ";
    test_filter << Fpl_Sec_Type_Dike::result_table->get_column_name(label::areastate_id) << " ="  << id.area_state;
    test_filter << " AND (";
    test_filter <<  Fpl_Sec_Type_Dike::result_table->get_column_name(label::measure_id) << " = " << 0 ;
    test_filter << " OR " ;
    test_filter << Fpl_Sec_Type_Dike::result_table->get_column_name(label::measure_id) << " = " << id.measure_nr;
    test_filter << " ) " ;
    test_filter << " AND ";
    test_filter << Fpl_Sec_Type_Dike::result_table->get_column_name(fpl_label::simulation_type) << " = '" << Fpl_Mc_Sim::convert_sim_type2text(sim_type)<<"'";
    test_filter << " ORDER BY " <<  Fpl_Sec_Type_Dike::result_table->get_column_name(fpl_label::waterlevel);

    Data_Base::database_request(results, test_filter.str(), ptr_database);

    //check the request
    if(results->lastError().isValid()){
        Warning msg;
        msg.set_msg("Fpl_Sec_Type_Dike::select_results_in_database(QSqlQueryModel *results, QSqlDatabase *ptr_database, const _sys_system_id id, const int sec_id, const _fpl_simulation_type sim_type)","Invalid database request", "Check the database", "", 2);
        ostringstream info;
        info << "Table Name      : " << Fpl_Sec_Type_Dike::result_table->get_table_name() << endl;
        info << "Table error info: " << results->lastError().text().toStdString() << endl;
        msg.make_second_info(info.str());
        msg.output_msg(1);
    }
    number=results->rowCount();

    return number;

}
//Close and delete the database table for the results of the mechanism of the dike section (static)
void Fpl_Sec_Type_Dike::close_result_table(void){
	if(Fpl_Sec_Type_Dike::result_table!=NULL){
		delete Fpl_Sec_Type_Dike::result_table;
		Fpl_Sec_Type_Dike::result_table=NULL;
	}
}
//Switch the applied-flag for the results of the mechanism of the dike section in the database table for a defined system state (static)
void Fpl_Sec_Type_Dike::switch_applied_flag_result_table(QSqlDatabase *ptr_database, const _sys_system_id id, const bool flag){
	//the table is set (the name and the column names) and allocated
	try{
		Fpl_Sec_Type_Dike::set_result_table(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}
	QSqlQueryModel query;

	ostringstream query_string;
	query_string <<"UPDATE ";
	query_string << Fpl_Sec_Type_Dike::result_table->get_table_name();
	query_string << " SET " ;
	query_string  << Fpl_Sec_Type_Dike::result_table->get_column_name(label::applied_flag) << " = "<< functions::convert_boolean2string(flag);
	query_string  << " WHERE ";
	query_string  << Fpl_Sec_Type_Dike::result_table->get_column_name(label::areastate_id) << " = "  << id.area_state;
	query_string  << " AND ";
	query_string  <<  Fpl_Sec_Type_Dike::result_table->get_column_name(label::measure_id) << " = " << id.measure_nr ;

	Data_Base::database_request(&query, query_string.str(), ptr_database);
	if(query.lastError().isValid()==true){
		Error msg;
		msg.set_msg("Fpl_Sec_Type_Dike::switch_applied_flag_result_table(QSqlDatabase *ptr_database, const _sys_system_id id, const bool flag)","Invalid database request", "Check the database", 2, false);
		ostringstream info;
		info << "Table Name      : " << Fpl_Sec_Type_Dike::result_table->get_table_name() << endl;
		info << "Table error info: " << query.lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
}
//Switch the applied-flag for the results of the mechanism of the dike section in the database table for a defined system state and id (static)
void Fpl_Sec_Type_Dike::switch_applied_flag_result_table(QSqlDatabase *ptr_database, const _sys_system_id id, const bool flag, const int sec_id){
	//the table is set (the name and the column names) and allocated
	try{
		Fpl_Sec_Type_Dike::set_result_table(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}
	QSqlQueryModel query;

	ostringstream query_string;
	query_string <<"UPDATE ";
	query_string << Fpl_Sec_Type_Dike::result_table->get_table_name();
	query_string << " SET " ;
	query_string  << Fpl_Sec_Type_Dike::result_table->get_column_name(label::applied_flag) << " = "<< functions::convert_boolean2string(flag);
	query_string  << " WHERE ";
	query_string  << Fpl_Sec_Type_Dike::result_table->get_column_name(label::areastate_id) << " = "  << id.area_state;
	query_string  << " AND ";
	query_string  <<  Fpl_Sec_Type_Dike::result_table->get_column_name(label::measure_id) << " = " << id.measure_nr ;
	query_string  << " AND ";
	query_string  <<  Fpl_Sec_Type_Dike::result_table->get_column_name(fpl_label::section_id) << " = " << sec_id ;

	Data_Base::database_request(&query, query_string.str(), ptr_database);
	if(query.lastError().isValid()==true){
		Error msg;
		msg.set_msg("Fpl_Sec_Type_Dike::switch_applied_flag_result_table(QSqlDatabase *ptr_database, const _sys_system_id id, const bool flag, const int sec_id)","Invalid database request", "Check the database", 2, false);
		ostringstream info;
		info << "Table Name      : " << Fpl_Sec_Type_Dike::result_table->get_table_name() << endl;
		info << "Table error info: " << query.lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
}
//Check and set the section geometry out of the read in geometry members
void Fpl_Sec_Type_Dike::check_set_geometry(void){
	//make th polygon and the polysegment of the outer boundary
	try{
		this->make_geometrical_outer_dike_polygon();
	}
	catch(Error msg){
		ostringstream info;
		info<<"Try to set-up the outer dike polygon"<< endl;
		msg.make_second_info(info.str());
		throw msg;
	}
	try{
		this->make_geometrical_outer_dike_polysegment();
	}
	catch(Error msg){
		ostringstream info;
		info<<"Try to set-up the outer dike polysegement"<< endl;
		msg.make_second_info(info.str());
		throw msg;
	}

	this->calc_area_cubature();

	Geo_Point crest_mid=this->crest_cubature.get_segment()->calc_mid_point();

	//wind wave generation
	if(this->wave_wind_event!=NULL){
		if(this->wave_wind_event->get_2calc_flag()==true){
			this->wave_wind_event->set_ptr_foreland(&this->foreland);
		}
	}

	//set the mid point to the list...it has always the index 0
	this->seepage_body_ascending.add_new_point(crest_mid.get_xcoordinate(),0.0, true, &this->outer_polysegment);
	//set the first point of the cubature ...it has always the index 1
	this->seepage_body_ascending.add_new_point(this->outer_polysegment.get_first_point()->get_xcoordinate(),0.0, false, &this->outer_polysegment);
	//set the first point of the cubature ...it has always the index 1
	this->seepage_body_descending.add_new_point(crest_mid.get_xcoordinate(),0.0, true, &this->outer_polysegment);
	//set the first point of the cubature ...it has always the index 1
	this->seepage_body_descending.add_new_point(this->outer_polysegment.get_first_point()->get_xcoordinate(),0.0, false, &this->outer_polysegment);

	if(this->number_material_zones>0){
		//check if the material zone polygons are not intercepting each other
		for(int i=0; i< this->number_material_zones; i++){
			for(int j=i; j<this->number_material_zones; j++){
				if(i!=j){
					double buff=this->material_zones[i].polygon_interception(&this->material_zones[j]);
					if(buff>constant::area_epsilon){
						//Error
						Error msg=this->set_error(20);
						ostringstream info;
						info << "1. Polygon name (id) : "<<this->material_zones[i].get_zone_name()<<" ("<<this->material_zones[i].get_zone_number()<<")"<<endl;
						info << "2. Polygon name (id) : "<<this->material_zones[j].get_zone_name()<<" ("<<this->material_zones[j].get_zone_number()<<")"<<endl;
						msg.make_second_info(info.str());
						throw msg;
					}
				}
			}
		}

		double intercept_area=0.0;
		double poly_area=0.0;
		double total_area=0.0;
		//check if the material zone polygons  inside the section polygon
		for(int i=0; i< this->number_material_zones; i++){
			poly_area=this->material_zones[i].calculate_area();
			total_area=total_area+poly_area;
			intercept_area=this->outer_polygon.polygon_interception(&this->material_zones[i]);
			if(abs(poly_area-intercept_area)>constant::area_epsilon){
				//Error
				Error msg=this->set_error(21);
				ostringstream info;
				info << "Polygon name (id) : "<<this->material_zones[i].get_zone_name()<<" ("<<this->material_zones[i].get_variable_zone_number()<<")"<<endl;
				info << "Difference of area : "<< abs(poly_area-intercept_area)<<endl;
				info << "Check the direction of points of the polygon (clockwise setting)"<<endl;
				msg.make_second_info(info.str());
				throw msg;
			}
		}

		//check if the section polygon is completely filled with material zone polygons
		double area_out_poly=this->outer_polygon.calculate_area();
		if(abs(area_out_poly-total_area)>constant::area_epsilon){
			//Error
			Error msg=this->set_error(22);
			ostringstream info;
			info << "Filled area [%] : "<<(total_area/area_out_poly)*100.0<<endl;
			msg.make_second_info(info.str());
			throw msg;
		}
	}

	//check the material variable zone
	for(int i=0; i<this->number_material_variable_zones; i++){
		if(this->material_variable_zones[i].get_zone_id()<=0){
			Error msg=this->set_error(23);
			ostringstream info;
			info<< "Found material variable zone id : "<< this->material_variable_zones[i].get_zone_id()<<endl;
			msg.make_second_info(info.str());
			throw msg;
		}
		for(int j=i; j< this->number_material_variable_zones; j++){
			if(i!=j){
				if(this->material_variable_zones[i].get_zone_id()==this->material_variable_zones[j].get_zone_id()){
					Error msg=this->set_error(24);
					ostringstream info;
					info<< "Equal material variable zone id : "<< this->material_variable_zones[i].get_zone_id()<<endl;
					msg.make_second_info(info.str());
					throw msg;
				}
			}
		}
	}
	//check the uniqnes of the zone types
	int counter=0;
	for(int i=0; i<this->number_material_zones; i++){
		if(this->material_zones[i].get_zone_type()==_fpl_zone_type::imperm_layer){
			counter++;
		}
	}
	if(counter>1){
		Error msg=this->set_error(30);
		throw msg;
	}
	counter=0;
	for(int i=0; i<this->number_material_zones; i++){
		if(this->material_zones[i].get_zone_type()==_fpl_zone_type::perm_layer){
			counter++;
		}
	}
	if(counter>1){
		Error msg=this->set_error(31);
		throw msg;
	}

	//check the mid-gradient of the waterside cubature of the dike
	double grad=this->waterside_cubature.calculate_mid_gradient();
	this->check_dike_gradients(grad, true);

	//check the mid-gradient of the landside cubature of the dike
	grad=this->landside_cubature.calculate_mid_gradient();
	this->check_dike_gradients(grad, false);
}
//_______________
//private
//Allocate the mechanism for wave generation induced by wind
void Fpl_Sec_Type_Dike::allocate_wind_wave_mechanism(void){
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
void Fpl_Sec_Type_Dike::delete_wind_wave_mechanism(void){
	if(this->wave_wind_event!=NULL){
		delete this->wave_wind_event;
		this->wave_wind_event=NULL;
	}
}
//Allocate the mechanism for wave forced runup
void Fpl_Sec_Type_Dike::allocate_wave_runup_mechanism(void){
	if(this->wave_runup_event!=NULL){
		return;
	}
	try{
		this->wave_runup_event=new Fpl_Mech_Wave_Runup();
		this->wave_runup_event->set_waterside_cubature(&this->waterside_cubature);
	}
	catch(bad_alloc &t){
		Error msg=this->set_error(5);
		ostringstream info;
		info<< "Info bad alloc: " << t.what() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
}
//Delete the mechanism for wave forced runup
void Fpl_Sec_Type_Dike::delete_wave_runup_mechanism(void){
	if(this->wave_runup_event!=NULL){
		delete this->wave_runup_event;
		this->wave_runup_event=NULL;
	}
}
//Allocate the mechanism for an overflow
void Fpl_Sec_Type_Dike::allocate_overflow_mechanism(void){
	if(this->overflow_event!=NULL){
		return;
	}

	try{
		this->overflow_event=new Fpl_Mech_Overflow_Dike();
	}
	catch(bad_alloc &t){
		Error msg=this->set_error(6);
		ostringstream info;
		info<< "Info bad alloc: " << t.what() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
}
//Delete the mechanism for an overflow
void Fpl_Sec_Type_Dike::delete_overflow_mechanism(void){
	if(this->overflow_event!=NULL){
		delete this->overflow_event;
		this->overflow_event=NULL;
	}
}
//Allocate the mechanism for bursting after DIN19712
void Fpl_Sec_Type_Dike::allocate_bursting_din_mechanism(void){
	if(this->bursting_din_event!=NULL){
		return;
	}

	try{
		this->bursting_din_event=new Fpl_Mech_Bursting_Din_Dike();
	}
	catch(bad_alloc &t){
		Error msg=this->set_error(3);
		ostringstream info;
		info<< "Info bad alloc: " << t.what() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
}
//Delete the mechanism for bursting after DIN19712
void Fpl_Sec_Type_Dike::delete_bursting_din_mechanism(void){
	if(this->bursting_din_event!=NULL){
		delete this->bursting_din_event;
		this->bursting_din_event=NULL;
	}
}
//Allocate the mechanism for piping after Sellmeijer
void Fpl_Sec_Type_Dike::allocate_piping_sell_mechanism(void){
	if(this->piping_sell_event!=NULL){
		return;
	}

	try{
		this->piping_sell_event=new Fpl_Mech_Piping_Sell_Dike();
	}
	catch(bad_alloc &t){
		Error msg=this->set_error(1);
		ostringstream info;
		info<< "Info bad alloc: " << t.what() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
}
//Delete the mechanism for piping after Sellmeijer
void Fpl_Sec_Type_Dike::delete_piping_sell_mechanism(void){
	if(this->piping_sell_event!=NULL){
		delete this->piping_sell_event;
		this->piping_sell_event=NULL;
	}
}
//Allocate the mechanism for piping after Lane
void Fpl_Sec_Type_Dike::allocate_piping_lane_mechanism(void){
	if(this->piping_lane_event!=NULL){
		return;
	}

	try{
		this->piping_lane_event=new Fpl_Mech_Piping_Lane_Dike();
	}
	catch(bad_alloc &t){
		Error msg=this->set_error(4);
		ostringstream info;
		info<< "Info bad alloc: " << t.what() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
}
//Delete the mechanism for piping after Lane
void Fpl_Sec_Type_Dike::delete_piping_lane_mechanism(void){
	if(this->piping_lane_event!=NULL){
		delete this->piping_lane_event;
		this->piping_lane_event=NULL;
	}
}
//Allocate the mechanism for piping after Schmertmann
void Fpl_Sec_Type_Dike::allocate_piping_schmertmann_mechanism(void){
	if(this->piping_schmertmann_event!=NULL){
		return;
	}

	try{
		this->piping_schmertmann_event=new Fpl_Mech_Piping_Schmertmann_Dike();
	}
	catch(bad_alloc &t){
		Error msg=this->set_error(28);
		ostringstream info;
		info<< "Info bad alloc: " << t.what() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
}
//Delete the mechanism for piping after Schmertmann
void Fpl_Sec_Type_Dike::delete_piping_schmertmann_mechanism(void){
	if(this->piping_schmertmann_event!=NULL){
		delete this->piping_schmertmann_event;
		this->piping_schmertmann_event=NULL;
	}
}
//Allocate the mechanism for slope stability landside of a section after Krey
void Fpl_Sec_Type_Dike::allocate_slope_stability_landside_mechanism(void){
	if(this->slope_stability_landside_event!=NULL){
		return;
	}

	try{
		this->slope_stability_landside_event=new Fpl_Mech_Slope_Stability_Dike();
		this->slope_stability_landside_event->set_waterside_slope_mechanism(false);
	}
	catch(bad_alloc &t){
		Error msg=this->set_error(8);
		ostringstream info;
		info<< "Info bad alloc: " << t.what() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
}
//Delete the mechanism for slope stability landside of a section after Krey
void Fpl_Sec_Type_Dike::delete_slope_stability_landside_mechanism(void){
	if(this->slope_stability_landside_event!=NULL){
		delete this->slope_stability_landside_event;
		this->slope_stability_landside_event=NULL;
	}
}
//Allocate the mechanism for slope stability waterside of the section after Krey
void Fpl_Sec_Type_Dike::allocate_slope_stability_waterside_mechanism(void){
	if(this->slope_stability_waterside_event!=NULL){
		return;
	}

	try{
		this->slope_stability_waterside_event=new Fpl_Mech_Slope_Stability_Dike();
		this->slope_stability_waterside_event->set_waterside_slope_mechanism(true);
	}
	catch(bad_alloc &t){
		Error msg=this->set_error(10);
		ostringstream info;
		info<< "Info bad alloc: " << t.what() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
}
//Delete the mechanism for slope stability waterside of the section after Krey
void Fpl_Sec_Type_Dike::delete_slope_stability_waterside_mechanism(void){
	if(this->slope_stability_waterside_event!=NULL){
		delete this->slope_stability_waterside_event;
		this->slope_stability_waterside_event=NULL;
	}
}
//Allocate the mechanism for erosion landside of the section
void Fpl_Sec_Type_Dike::allocate_erosion_landside_mechanism(void){
	if(this->erosion_landside_event!=NULL){
		return;
	}

	try{
		this->erosion_landside_event=new Fpl_Mech_Landside_Erosion();
	}
	catch(bad_alloc &t){
		Error msg=this->set_error(34);
		ostringstream info;
		info<< "Info bad alloc: " << t.what() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
}
//Delete the mechanism for erosion landside of the section
void Fpl_Sec_Type_Dike::delete_erosion_landside_mechanism(void){
	if(this->erosion_landside_event!=NULL){
		delete this->erosion_landside_event;
		this->erosion_landside_event=NULL;
	}
}
//Allocate the mechanism for erosion waterside of the section
void Fpl_Sec_Type_Dike::allocate_erosion_waterside_mechanism(void){
	if(this->erosion_waterside_event!=NULL){
		return;
	}

	try{
		this->erosion_waterside_event=new Fpl_Mech_Waterside_Erosion();
		this->erosion_waterside_event->set_waterside_cubature(&this->waterside_cubature);
	}
	catch(bad_alloc &t){
		Error msg=this->set_error(35);
		ostringstream info;
		info<< "Info bad alloc: " << t.what() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
}
//Delete the mechanism for erosion waterside of the section 
void Fpl_Sec_Type_Dike::delete_erosion_waterside_mechanism(void){
	if(this->erosion_waterside_event!=NULL){
		delete this->erosion_waterside_event;
		this->erosion_waterside_event=NULL;
	}
}
//Allocate the mechanism for start the erosion waterside of the section
void Fpl_Sec_Type_Dike::allocate_start_erosion_waterside_mechanism(void){
	if(this->erosion_waterside_start_event!=NULL){
		return;
	}

	try{
		this->erosion_waterside_start_event=new Fpl_Mech_Waterside_Erosion_Start();
		this->erosion_waterside_start_event->set_waterside_cubature(&this->waterside_cubature);
	}
	catch(bad_alloc &t){
		Error msg=this->set_error(43);
		ostringstream info;
		info<< "Info bad alloc: " << t.what() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
}
//Delete the mechanism for start the erosion waterside of the section 
void Fpl_Sec_Type_Dike::delete_start_erosion_waterside_mechanism(void){
	if(this->erosion_waterside_start_event!=NULL){
		delete this->erosion_waterside_start_event;
		this->erosion_waterside_start_event=NULL;
	}
}
//Allocate the mechanism of wave pressure impact on the waterside of the section
void Fpl_Sec_Type_Dike::allocate_wave_impact_mechanism(void){
	if(this->wave_impact_event!=NULL){
		return;
	}

	try{
		this->wave_impact_event=new Fpl_Mech_Wave_Impact_Waterside_Dike();
		this->wave_impact_event->set_waterside_cubature(&this->waterside_cubature);
	}
	catch(bad_alloc &t){
		Error msg=this->set_error(36);
		ostringstream info;
		info<< "Info bad alloc: " << t.what() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
}
//Delete the mechanism of wave pressure impact on the waterside of the section 
void Fpl_Sec_Type_Dike::delete_wave_impact_mechanism(void){
	if(this->wave_impact_event!=NULL){
		delete this->wave_impact_event;
		this->wave_impact_event=NULL;
	}
}
//Allocate the material zones of the dike
void Fpl_Sec_Type_Dike::allocate_material_zones(void){
	try{
		this->material_zones=new Fpl_Dike_Geo_Materialzone[this->number_material_zones];
	}
	catch(bad_alloc &t){
		Error msg=this->set_error(9);
		ostringstream info;
		info<< "Info bad alloc: " << t.what() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
	for(int i=0; i< this->number_material_zones; i++){
		this->material_zones[i].set_systemid(this->system_id);
	}
}
//Delete the material zones of the dike
void Fpl_Sec_Type_Dike::delete_material_zones(void){
	if(this->material_zones!=NULL){
		delete []this->material_zones;
		this->material_zones=NULL;
	}
}
//Allocate the material variable zone
void Fpl_Sec_Type_Dike::allocate_material_variable_zone(void){
	this->delete_material_variable_zone();
	try{
		this->material_variable_zones=new Fpl_Dike_Var_Materialzone[this->number_material_variable_zones];
	}
	catch(bad_alloc &t){
		Error msg=this->set_error(15);
		ostringstream info;
		info<< "Info bad alloc: " << t.what() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
}
//Delete the material variable zone
void Fpl_Sec_Type_Dike::delete_material_variable_zone(void){
	if(this->material_variable_zones!=NULL){
		delete []this->material_variable_zones;
		this->material_variable_zones=NULL;
	}
}
//Allocate the waterside material variable zone
void Fpl_Sec_Type_Dike::allocate_waterside_variable_zone(void){
	this->delete_waterside_variable_zone();
	try{
		this->material_waterside_zone=new Fpl_Cub_Waterside_Materialzone[this->number_mat_waterside_zones];
	}
	catch(bad_alloc &t){
		Error msg=this->set_error(37);
		ostringstream info;
		info<< "Info bad alloc: " << t.what() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
}
//Delete the waterside material variable zone
void Fpl_Sec_Type_Dike::delete_waterside_variable_zone(void){
	if(this->material_waterside_zone!=NULL){
		delete []this->material_waterside_zone;
		this->material_waterside_zone=NULL;
	}
}
//Set the general parameters of the section to the mechanisms
void Fpl_Sec_Type_Dike::set_general(void){
	//output
	ostringstream cout;
	cout <<"Set general parameters of dike... "  << endl ;
	Sys_Common_Output::output_fpl->output_txt(cout.str(),false);
	//wind wave generation
	if(this->wave_wind_event!=NULL){
		if(this->wave_wind_event->get_2calc_flag()==true){
			this->wave_wind_event->set_section_direction(this->section_direction);
		}
	}
}
//Set the random variables of this section and the relevant mechanisms from database table
void Fpl_Sec_Type_Dike::set_variables(const bool frc_sim, QSqlDatabase *ptr_database, const int section_id){
	//output
	ostringstream cout;
	cout <<"Set general variables of the dike section " <<section_id<<" ... "  << endl ;
	Sys_Common_Output::output_fpl->output_txt(&cout,false);
	try{
		//waterlevel is special (frc calculation or mc calc)
		if(frc_sim==false){
			this->waterlevel.set_input(this->system_id, section_id, fpl_label::waterlevel,false, ptr_database, fpl_label::mech_all,fpl_label::sec_dike,0);
			this->waterlevel.set_distribution_types(ptr_database);
		}
		else{
			this->waterlevel.set_input(this->system_id, section_id, fpl_label::waterlevel_frc,true, ptr_database, fpl_label::mech_all,fpl_label::sec_dike,0);
			this->waterlevel.set_distribution_types(ptr_database);
		}
		//water density
		this->water_density.set_input(this->system_id, section_id, fpl_label::water_density,false, ptr_database, fpl_label::mech_all,fpl_label::sec_dike,0);
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
			reaction<<"Mechansim of the wave generation is not taken into account!";
			warn1.error_to_warning(&msg, 20);
			warn1.set_reaction(reaction.str());
			warn1.output_msg(1);
			//reaction
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
			reaction<<"Mechansim wave run-up and wave impact is not taken into account!";
			warn1.error_to_warning(&msg, 20);
			warn1.set_reaction(reaction.str());
			warn1.output_msg(1);
			//reaction
			this->delete_wave_runup_mechanism();
			this->delete_wave_impact_mechanism();
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
		try{
			if(this->erosion_landside_event!=NULL){
				if(this->erosion_landside_event->get_2calc_flag()==true){
					cout <<"Set variables of the erosion landside mechanism..."  << endl ;
					Sys_Common_Output::output_fpl->output_txt(&cout,false);
					//set the variables
					this->erosion_landside_event->set_variables(ptr_database,this->system_id, section_id);
					this->erosion_landside_event->set_result_variables(ptr_database,this->system_id, section_id);
				}
			}
		}
		catch(Error msg){
			Warning warn1;
			ostringstream reaction;
			reaction<<"Mechansim erosion landside is not taken into account!";
			warn1.error_to_warning(&msg, 20);
			warn1.set_reaction(reaction.str());
			warn1.output_msg(1);
			//reaction
			this->delete_erosion_landside_mechanism();
		}

		//erosion waterside start
		try{
			if(this->erosion_waterside_start_event!=NULL){
				if(this->erosion_waterside_start_event->get_2calc_flag()==true){
					cout <<"Set variables of the erosion waterside start mechanism..."  << endl ;
					Sys_Common_Output::output_fpl->output_txt(&cout,false);
					//set the variables
					this->erosion_waterside_start_event->set_variables(ptr_database,this->system_id, section_id);
					this->erosion_waterside_start_event->set_result_variables(ptr_database,this->system_id, section_id);
				}
			}
		}
		catch(Error msg){
			Warning warn1;
			ostringstream reaction;
			reaction<<"Start mechansim erosion waterside is not taken into account; also the erosion waterside mechanism is not taken into account!";
			warn1.error_to_warning(&msg, 20);
			warn1.set_reaction(reaction.str());
			warn1.output_msg(1);
			//reaction
			this->delete_start_erosion_waterside_mechanism();
			this->delete_erosion_waterside_mechanism();
		}


		//erosion waterside
		try{
			if(this->erosion_waterside_event!=NULL){
				if(this->erosion_waterside_event->get_2calc_flag()==true){
					cout <<"Set variables of the erosion waterside mechanism..."  << endl ;
					Sys_Common_Output::output_fpl->output_txt(&cout,false);
					//set the variables
					this->erosion_waterside_event->set_variables(ptr_database,this->system_id, section_id);
					this->erosion_waterside_event->set_result_variables(ptr_database,this->system_id, section_id);
				}
			}
		}
		catch(Error msg){
			Warning warn1;
			ostringstream reaction;
			reaction<<"Mechansim erosion waterside is not taken into account!";
			warn1.error_to_warning(&msg, 20);
			warn1.set_reaction(reaction.str());
			warn1.output_msg(1);
			//reaction
			this->delete_erosion_waterside_mechanism();
		}
		//wave impact
		try{
		
			if(this->wave_impact_event!=NULL){
				if(this->wave_impact_event->get_2calc_flag()==true){
					cout <<"Set variables of the wave impact mechanism to waterside..."  << endl ;
					Sys_Common_Output::output_fpl->output_txt(&cout,false);
					//set the variables
					this->wave_impact_event->set_variables(ptr_database,this->system_id, section_id);
					this->wave_impact_event->set_result_variables(ptr_database,this->system_id, section_id);
				}
			}
		}
		catch(Error msg){
			Warning warn1;
			ostringstream reaction;
			reaction<<"Mechansim wave impact to waterside is not taken into account!";
			warn1.error_to_warning(&msg, 20);
			warn1.set_reaction(reaction.str());
			warn1.output_msg(1);
			//reaction
			this->delete_wave_impact_mechanism();
		}

		//bursting after din 19712
		try{
			if(this->bursting_din_event!=NULL){
				if(this->bursting_din_event->get_2calc_flag()==true){
					cout <<"Set variables of the bursting mechanism..."  << endl ;
					Sys_Common_Output::output_fpl->output_txt(&cout,false);
					//set the leakage length
					if(this->number_material_zones==0){
						this->bursting_din_event->set_fixed_leak_length(this->base_water.horizontal_distance(&this->base_land));
					}
					else{
						this->bursting_din_event->set_ptr_no_material_zone(this->number_material_zones, this->material_zones);
						double buff_x=0.0;
						buff_x=this->bursting_din_event->calculate_geometrical_values(&this->base_land,&this->base_water, &this->crest_land, &this->hinterland, &this->seepage_body_ascending, &this->outer_polysegment);
						this->index_h_imperm_layer=this->seepage_body_ascending.add_new_point(buff_x,0.0,false, &this->outer_polysegment);
					}
					//set the variables
					this->bursting_din_event->set_variables(ptr_database,this->system_id, section_id, this->get_ptr_specified_material_variable_zone(_fpl_zone_type::imperm_layer));
					this->bursting_din_event->set_result_variables(ptr_database,this->system_id, section_id);
				}
			}
		}
		catch(Error msg){
			Warning warn1;
			ostringstream reaction;
			reaction<<"Mechanism bursting after DIN is not taken into account; also the piping mechanism are not calculated!";
			warn1.error_to_warning(&msg, 20);
			warn1.set_reaction(reaction.str());
			warn1.output_msg(1);
			//reaction
			this->delete_bursting_din_mechanism();
			this->delete_piping_lane_mechanism();
			this->delete_piping_sell_mechanism();
			this->delete_piping_schmertmann_mechanism();
		}
		//piping after lane
		try{
			if(this->piping_lane_event!=NULL){
				if(this->piping_lane_event->get_2calc_flag()==true){
					cout <<"Set variables of the piping mechanism (Lane)..."  << endl ;
					Sys_Common_Output::output_fpl->output_txt(&cout,false);
					//set the variables
					this->piping_lane_event->set_variables(ptr_database,this->system_id, section_id);
					this->piping_lane_event->set_result_variables(ptr_database,this->system_id, section_id);
				}
			}
		}
		catch(Error msg){
			Warning warn1;
			ostringstream reaction;
			reaction<<"Mechanism piping after Lane is not taken into account!";
			warn1.error_to_warning(&msg, 20);
			warn1.set_reaction(reaction.str());
			warn1.output_msg(1);
			//reaction
			this->delete_piping_lane_mechanism();
		}
		//piping after sellmeijer
		try{
			if(this->piping_sell_event!=NULL){
				if(this->piping_sell_event->get_2calc_flag()==true){
					cout <<"Set variables of the piping mechanism (Sellmeijer)..."  << endl ;
					Sys_Common_Output::output_fpl->output_txt(&cout,false);
					//set the variables
					this->piping_sell_event->set_variables(ptr_database,this->system_id, section_id, this->get_ptr_specified_material_variable_zone(_fpl_zone_type::perm_layer));
					this->piping_sell_event->set_result_variables(ptr_database,this->system_id, section_id);
				}
			}
		}
		catch(Error msg){
			Warning warn1;
			ostringstream reaction;
			reaction<<"Mechansim piping after Sellmeijer is not taken into account!";
			warn1.error_to_warning(&msg, 20);
			warn1.set_reaction(reaction.str());
			warn1.output_msg(1);
			//reaction
			this->delete_piping_sell_mechanism();
		}
		//piping after schmertmann
		try{
			if(this->piping_schmertmann_event!=NULL){
				if(this->piping_schmertmann_event->get_2calc_flag()==true){
					cout <<"Set variables of the piping mechanism (Schmertmann)..."  << endl ;
					Sys_Common_Output::output_fpl->output_txt(&cout,false);
					//set the variables
					this->piping_schmertmann_event->set_variables(ptr_database,this->system_id, section_id, this->get_ptr_specified_material_variable_zone(_fpl_zone_type::perm_layer), this->piping_sell_event);
					this->piping_schmertmann_event->set_result_variables(ptr_database,this->system_id, section_id);
				}
			}
		}
		catch(Error msg){
			Warning warn1;
			ostringstream reaction;
			reaction<<"Mechansim piping after Schmertmann is not taken into account!";
			warn1.error_to_warning(&msg, 20);
			warn1.set_reaction(reaction.str());
			warn1.output_msg(1);
			//reaction
			this->delete_piping_schmertmann_mechanism();
		}
		//slope landside
		if(this->slope_stability_landside_event!=NULL){
			if(this->slope_stability_landside_event->get_2calc_flag()==true){
				cout <<"Set variables of the landside slope stability mechanism (Krey)..."  << endl ;
				Sys_Common_Output::output_fpl->output_txt(&cout,false);
				//set the variables
				this->slope_stability_landside_event->set_result_variables(ptr_database,this->system_id, section_id);
			}
		}
		//slope waterside
		if(this->slope_stability_waterside_event!=NULL){
			if(this->slope_stability_waterside_event->get_2calc_flag()==true){
				cout <<"Set variables of the waterside slope stability mechanism (Krey)..."  << endl ;
				Sys_Common_Output::output_fpl->output_txt(&cout,false);
				//set the variables
				this->slope_stability_waterside_event->set_result_variables(ptr_database,this->system_id, section_id);
			}
		}


	}
	catch(Error msg){
		throw msg;
	}
}
//Set the geometry of the section and their surrounding via a database table
void Fpl_Sec_Type_Dike::set_geometrie(QSqlDatabase *ptr_database, const int section_id){
	//output
	ostringstream cout;
	cout <<"Set the geometry of the dike section " <<section_id<<" ... "  << endl ;
	Sys_Common_Output::output_fpl->output_txt(&cout,false);
	try{
		//read the cubature
		this->read_cubature(ptr_database, section_id);
		//set the material zones
		this->read_matzones(ptr_database, section_id);
		//set the variable zones
		this->read_mat_variable_zones(ptr_database, section_id);
		//set the variable zone of the waterside
		this->read_mat_variable_zones_waterside(ptr_database, section_id);


		if(this->wave_wind_event!=NULL){
			if(this->wave_wind_event->get_2calc_flag()==true){
				try{
					cout <<"Set geometry of the dike surrounding... "  << endl ;
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
void Fpl_Sec_Type_Dike::set_control_parameter_mechanism(QSqlDatabase *ptr_database, const int section_id, const bool frc_sim){
	//output
	ostringstream cout;
	cout <<"Set the control parameters of the dike section " <<section_id<<" for the mechanisms... "  << endl ;
	Sys_Common_Output::output_fpl->output_txt(&cout,false);

	this->seepage_calculator.set_input(ptr_database, frc_sim, true);

	try{
		if(this->slope_stability_landside_event!=NULL){
			if(this->slope_stability_landside_event->get_2calc_flag()==true){
				cout <<"Set the control parameters of the landside slope stability... "  << endl ;
				Sys_Common_Output::output_fpl->output_txt(&cout,false);
				if(this->slope_stability_landside_event->set_control_parameter(ptr_database, section_id)==false){
					cout <<"The landside slope stability is not calculated for dike section "  <<section_id<< endl ;
					this->slope_stability_landside_event->set_2calc_flag(false);
					Sys_Common_Output::output_fpl->output_txt(&cout,false);
				}
			}
		}
	}
	catch(Error msg){
		Warning warn1;
		ostringstream reaction;
		reaction<<"Mechansim of the landside slope stability is not taken into account!";
		warn1.error_to_warning(&msg, 20);
		warn1.set_reaction(reaction.str());
		warn1.output_msg(1);
		//reation
		this->delete_slope_stability_landside_mechanism();
	}
	try{
		if(this->slope_stability_waterside_event!=NULL){
			if(this->slope_stability_waterside_event->get_2calc_flag()==true){
				cout <<"Set the control parameters of the waterside slope stability... "  << endl ;
				Sys_Common_Output::output_fpl->output_txt(&cout,false);
				if(this->slope_stability_waterside_event->set_control_parameter(ptr_database, section_id)==false){
					cout <<"The waterside slope stability is not calculated for dike section "  <<section_id<< endl ;
					this->slope_stability_waterside_event->set_2calc_flag(false);
					Sys_Common_Output::output_fpl->output_txt(&cout,false);
				}
			}
		}
	}
	catch(Error msg){
		Warning warn1;
		ostringstream reaction;
		reaction<<"Mechansim of the waterside slope stability is not taken into account!";
		warn1.error_to_warning(&msg, 20);
		warn1.set_reaction(reaction.str());
		warn1.output_msg(1);
		//reation
		this->delete_slope_stability_waterside_mechanism();
	}
}
//Initialize the mechanisms
void Fpl_Sec_Type_Dike::init_mechanism(const int section_id){
	//output
	ostringstream cout;
	cout <<"Initialize the mechanisms of the dike section " <<section_id<<"... "  << endl ;
	Sys_Common_Output::output_fpl->output_txt(&cout,false);
	try{
		Geo_Point crest_mid=this->crest_cubature.get_segment()->calc_mid_point();
		if(this->slope_stability_landside_event!=NULL){
			if(this->slope_stability_landside_event->get_2calc_flag()==true){
				cout <<"Initialize the mechanism of the landside slope stability... "  << endl ;
				Sys_Common_Output::output_fpl->output_txt(&cout,false);
				this->slope_stability_landside_event->init_calculation(&this->outer_polygon, &this->outer_polysegment, &this->base_land, &crest_mid ,this->number_material_zones, this->material_zones, this->area_cubature);
				this->slope_stability_landside_event->add_seepage_line_point(&this->seepage_body_ascending, &this->outer_polysegment);
			}
		}
		if(this->slope_stability_waterside_event!=NULL){
			if(this->slope_stability_waterside_event->get_2calc_flag()==true){
				cout <<"Initialize the mechanism of the waterside slope stability... "  << endl ;
				Sys_Common_Output::output_fpl->output_txt(&cout,false);
				this->slope_stability_waterside_event->init_calculation(&this->outer_polygon, &this->outer_polysegment, &this->base_land, &crest_mid , this->number_material_zones, this->material_zones, this->area_cubature);
				this->slope_stability_waterside_event->add_seepage_line_point(&this->seepage_body_descending, &this->outer_polysegment);
			}
		}
		
	}
	catch(Error msg){
		throw msg;
	}
}
//Set the fault tree for this section type via a database table
void Fpl_Sec_Type_Dike::init_fault_tree(QSqlDatabase *ptr_database){
	//output
	ostringstream cout;
	cout <<"Initialize the fault tree for a dike-section... " << endl ;
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
	filter << _Fpl_Section_Type::table_mechanism->get_column_name(fpl_label::mecha_sec_type) << " = " << "'"<<fpl_label::sec_dike<<"'";
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
void Fpl_Sec_Type_Dike::decide_mechanism(const string mechanism_name, const bool application_flag, const bool applied_in_tree){
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
		else if(mechanism_name==fpl_label::mech_bursting_din){
			this->allocate_bursting_din_mechanism();
			this->bursting_din_event->set_2calc_flag(true);
			this->bursting_din_event->set_in_tree_flag(true);
			this->number_mech++;
			ostringstream cout;
			cout <<"Bursting mechanism (DIN) is initialized..." << endl ;
			Sys_Common_Output::output_fpl->output_txt(cout.str(),false);
		}
		else if(mechanism_name==fpl_label::mech_piping_sell){
			this->allocate_piping_sell_mechanism();
			this->piping_sell_event->set_2calc_flag(true);
			this->piping_sell_event->set_in_tree_flag(applied_in_tree);
			this->number_mech++;
			ostringstream cout;
			cout <<"Piping mechanism (Sellmeijer) is initialized..." << endl ;
			Sys_Common_Output::output_fpl->output_txt(cout.str(),false);
		}
		else if(mechanism_name==fpl_label::mech_piping_lane){
			this->allocate_piping_lane_mechanism();
			this->piping_lane_event->set_2calc_flag(true);
			this->piping_lane_event->set_in_tree_flag(applied_in_tree);
			this->number_mech++;
			ostringstream cout;
			cout <<"Piping mechanism (Lane) is initialized..." << endl ;
			Sys_Common_Output::output_fpl->output_txt(cout.str(),false);
		}
		else if(mechanism_name==fpl_label::mech_piping_schmert){
			this->allocate_piping_schmertmann_mechanism();
			this->piping_schmertmann_event->set_2calc_flag(true);
			this->piping_schmertmann_event->set_in_tree_flag(applied_in_tree);
			this->number_mech++;
			ostringstream cout;
			cout <<"Piping mechanism (Schmertmann) is initialized..." << endl ;
			Sys_Common_Output::output_fpl->output_txt(cout.str(),false);
		}
		else if(mechanism_name==fpl_label::mech_slope_landside){
			this->allocate_slope_stability_landside_mechanism();
			this->slope_stability_landside_event->set_2calc_flag(true);
			this->slope_stability_landside_event->set_in_tree_flag(true);
			this->number_mech++;
			ostringstream cout;
			cout <<"Slope stability landside is initialized..." << endl ;
			Sys_Common_Output::output_fpl->output_txt(cout.str(),false);
		}
		else if(mechanism_name==fpl_label::mech_slope_waterside){
			this->allocate_slope_stability_waterside_mechanism();
			this->slope_stability_waterside_event->set_2calc_flag(true);
			this->slope_stability_waterside_event->set_in_tree_flag(true);
			this->number_mech++;
			ostringstream cout;
			cout <<"Slope stability waterside is initialized..." << endl ;
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
		else if(mechanism_name==fpl_label::mech_erosion_waterside_start){
			this->allocate_start_erosion_waterside_mechanism();
			this->erosion_waterside_start_event->set_2calc_flag(true);
			this->erosion_waterside_start_event->set_in_tree_flag(true);
			this->number_mech++;
			ostringstream cout;
			cout <<"Start erosion waterside is initialized..." << endl ;
			Sys_Common_Output::output_fpl->output_txt(cout.str(),false);
		}
		else if(mechanism_name==fpl_label::mech_erosion_waterside){
			this->allocate_erosion_waterside_mechanism();
			this->erosion_waterside_event->set_2calc_flag(true);
			this->erosion_waterside_event->set_in_tree_flag(true);
			this->number_mech++;
			ostringstream cout;
			cout <<"Erosion waterside is initialized..." << endl ;
			Sys_Common_Output::output_fpl->output_txt(cout.str(),false);
		}
		else if(mechanism_name==fpl_label::mech_wave_impact){
			this->allocate_wave_impact_mechanism();
			this->wave_impact_event->set_2calc_flag(true);
			this->wave_impact_event->set_in_tree_flag(true);
			this->number_mech++;
			ostringstream cout;
			cout <<"Wave impact waterside is initialized..." << endl ;
			Sys_Common_Output::output_fpl->output_txt(cout.str(),false);
		}
	}
}
//Set the dike geometry via database and set it to the relevant mechanisms
void Fpl_Sec_Type_Dike::read_cubature(QSqlDatabase *ptr_database, const int section_id){
	//set the values for the crest point (waterside) and the base point (water side)[0,0]
	//base point water is always 0,0...all is related to them!
	this->base_water.set_point_coordinate(0.0,0.0);
	//read out the results
	try{
		//this->crest_water.set_point_per_database(this->system_id, section_id, fpl_label::crest_water, ptr_database, fpl_label::region_cubature, fpl_label::sec_dike, 0);
		this->crest_land.set_point_per_database(this->system_id, section_id, fpl_label::crest_land, ptr_database, fpl_label::region_cubature, fpl_label::sec_dike, 0);
		this->base_land.set_point_per_database(this->system_id, section_id, fpl_label::base_land, ptr_database, fpl_label::region_cubature, fpl_label::sec_dike, 0);
	}catch(Error msg){
		throw msg;
	}
	//set the geometry segments of the dike section
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
//Set the dike material zones via database
void Fpl_Sec_Type_Dike::read_matzones(QSqlDatabase *ptr_database, const int section_id){
	//output
	ostringstream cout;
	cout <<"Set the material zone(s) of the dike... "  << endl ;
	Sys_Common_Output::output_fpl->output_txt(&cout,false);

	QSqlQueryModel results;
	try{
		this->number_material_zones=Fpl_Dike_Geo_Materialzone::select_relevant_matzone_database(&results, ptr_database, this->system_id, section_id);
		//search for the dike material zones
		if(this->number_material_zones==0){
			if(this->slope_stability_landside_event==NULL && this->slope_stability_waterside_event==NULL){
				cout <<"No material zone(s) are found... "  << endl ;
				Sys_Common_Output::output_fpl->output_txt(&cout,false);
				return;
			}
			else{
				Error msg=this->set_error(12);
				throw msg;
			}
		}
		else{
			cout <<this->number_material_zones<< " material zone(s) are found... "  << endl ;
			Sys_Common_Output::output_fpl->output_txt(&cout,false);
			this->allocate_material_zones();
		}

		for(int i=0; i< this->number_material_zones; i++){
			this->material_zones[i].set_input(&results, i, section_id, ptr_database);
		}
	}
	catch(Error msg){
		Warning warn1;
		ostringstream reaction;
		reaction<<"Mechansim of the slope stability is not taken into account!";
		warn1.error_to_warning(&msg, 20);
		warn1.set_reaction(reaction.str());
		warn1.output_msg(1);
		//reation
		this->delete_slope_stability_landside_mechanism();
		this->delete_slope_stability_waterside_mechanism();
	}
}
//Set the dike material variable zones via database
void Fpl_Sec_Type_Dike::read_mat_variable_zones(QSqlDatabase *ptr_database, const int section_id){
	//output
	ostringstream cout;
	cout <<"Set the material variable zone(s) of the dike... "  << endl ;
	Sys_Common_Output::output_fpl->output_txt(&cout,false);

	QList<int> buffer;
	//check which zones are needed
	for(int i=0; i< this->number_material_zones; i++){
		bool old_one=false;
		for(int j=0; j< buffer.count(); j++){
			if(buffer.at(j)==this->material_zones[i].get_variable_zone_number()){
				old_one=true;
				break;
			}
		}
		if(old_one==false){
			buffer.append(this->material_zones[i].get_variable_zone_number());
		}
	}

	bool slope_required=false;
	if(this->slope_stability_landside_event!=NULL || this->slope_stability_waterside_event!=NULL || this->bursting_din_event!=NULL){
		slope_required=true;
	}
	//look if the grain size distribution is required
	int grain_size_distri_index=-1;
	for(int i=0; i< this->number_material_zones; i++){
		if(this->material_zones[i].get_zone_type()==_fpl_zone_type::perm_layer){
			grain_size_distri_index=this->material_zones[i].get_variable_zone_number();
		}
	}

	this->number_material_variable_zones=buffer.count();
	if(this->number_material_variable_zones==0){
		if(this->slope_stability_landside_event==NULL && this->slope_stability_waterside_event==NULL){
			cout <<"No material variable zone(s) are found... "  << endl ;
			Sys_Common_Output::output_fpl->output_txt(&cout,false);
			return;
		}
		else{
			Error msg=this->set_error(12);
			throw msg;
		}
	}
	else{
		//output
		cout <<this->number_material_variable_zones<< " relevant material variable zone(s) are found... "  << endl ;
		Sys_Common_Output::output_fpl->output_txt(&cout,false);
		this->allocate_material_variable_zone();

		//set the zone input
		for(int i=0; i<this->number_material_variable_zones; i++){
			if(buffer.at(i)==grain_size_distri_index){
				this->material_variable_zones[i].set_input(section_id, this->system_id, ptr_database, buffer.at(i), slope_required, true);
			}
			else{
				this->material_variable_zones[i].set_input(section_id, this->system_id, ptr_database, buffer.at(i), slope_required, false);
			}
		}

		this->connect_material_zones();
	}
}
//Connect the material zones to the material variable zone
void Fpl_Sec_Type_Dike::connect_material_zones(void){
	//output
	ostringstream cout;
	cout <<"Connect the material variable zone to the material zone of the dike... "  << endl ;
	Sys_Common_Output::output_fpl->output_txt(&cout,false);

	//check the material variable zone id
	for(int i=0; i< this->number_material_variable_zones; i++){
		for(int j=0; j<this->number_material_variable_zones; j++){
			if(i!=j){
				if(this->material_variable_zones[i].get_zone_id()==this->material_variable_zones[j].get_zone_id()){
					Error msg=this->set_error(19);
					ostringstream info;
					info << "Id of the material variable zone : "<< this->material_variable_zones[i].get_zone_id()<<endl;
					msg.make_second_info(info.str());
					throw msg;
				}
			}
		}
	}

	//connect the material variable zone id to the geometrical zone
	for(int i=0; i< this->number_material_zones; i++){
		bool connection_found=false;
		for(int j=0; j< this->number_material_variable_zones; j++){
			if(this->material_zones[i].get_variable_zone_number()==this->material_variable_zones[j].get_zone_id()){
				this->material_zones[i].set_ptr_relevant_variables(&this->material_variable_zones[j]);
				connection_found=true;
				break;
			}
		}

		if(connection_found==false){
			Error msg=this->set_error(18);
			ostringstream info;
			info<<" Materialzone name : "<<this->material_zones[i].get_zone_name()<<endl;
			info<<" Materialzone id   : "<<this->material_zones[i].get_variable_zone_number()<<endl;
			msg.make_second_info(info.str());
			throw msg;
		}
	}

	cout <<"Connection of the material variable zone to the material zone of the dike is finished!"  << endl ;
	Sys_Common_Output::output_fpl->output_txt(&cout,false);
}
//Set the dike material variable zones of the waterside via database 
void Fpl_Sec_Type_Dike::read_mat_variable_zones_waterside(QSqlDatabase *ptr_database, const int section_id){

	//output
	ostringstream cout;
	cout <<"Set the material variable zone(s) of the dike waterside... "  << endl ;
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
	if(this->wave_impact_event!=NULL){
		wave_impact=true;
	}
	bool erosion=false;
	if(this->erosion_waterside_event!=NULL){
		erosion=true;
	}
	

	this->number_mat_waterside_zones=buffer.count();
	if(this->number_mat_waterside_zones==0){
		if(this->wave_runup_event==NULL && this->wave_impact_event==NULL && this->erosion_waterside_event==NULL){
			cout <<"No material variable zone(s) of the waterside are found... "  << endl ;
			Sys_Common_Output::output_fpl->output_txt(&cout,false);
			return;
		}
		else{
			Error msg=this->set_error(38);
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
void Fpl_Sec_Type_Dike::connect_material_zones_waterside(void){
	//output
	ostringstream cout;
	cout <<"Connect the waterside material variable zone to the material zone of the waterside cubature... "  << endl ;
	Sys_Common_Output::output_fpl->output_txt(&cout,false);

	//check the material variable zone id
	for(int i=0; i< this->number_mat_waterside_zones; i++){
		for(int j=0; j<this->number_mat_waterside_zones; j++){
			if(i!=j){
				if(this->material_waterside_zone[i].get_zone_id()==this->material_waterside_zone[j].get_zone_id()){
					Error msg=this->set_error(39);
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
			Error msg=this->set_error(40);
			ostringstream info;
			info<<" Materialzone id waterside cubature  : "<<this->waterside_cubature.get_segment(i)->get_zone_number()<<endl;
			msg.make_second_info(info.str());
			throw msg;
		}
	}

	cout <<"Connection of the waterside material variable zone to the material zone of the waterside cubature is finished!"  << endl ;
	Sys_Common_Output::output_fpl->output_txt(&cout,false);
}
//Create the geometrical outer boundary of the dike, as polysegment
void Fpl_Sec_Type_Dike::make_geometrical_outer_dike_polysegment(void){
	//make the polysegment
	//count the number of points, the first and the last are points horizontal to the waterside dike toe/landside dike toe
	int number_points=0;
	number_points=this->foreland.get_number_segments()+this->waterside_cubature.get_number_segments()+this->crest_cubature.get_number_segments()
		+this->landside_cubature.get_number_segments()+this->hinterland.get_number_segments()+1;

	//allocate a buffer for the points
	Geo_Point *buffer=NULL;
	try{
		buffer=new Geo_Point[number_points];
	}
	catch(bad_alloc &t){
		Error msg=this->set_error(25);
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
//Create the geometrical outer boundary of the dike as polygon
void Fpl_Sec_Type_Dike::make_geometrical_outer_dike_polygon(void) {
	//make the polysegment
	//count the number of points, the first and the last are points horizontal to the waterside dike toe/landside dike toe
	int number_points;
	number_points = 3 + this->foreland.get_number_segments() + this->waterside_cubature.get_number_segments() + this->crest_cubature.get_number_segments() +
		this->landside_cubature.get_number_segments() + this->hinterland.get_number_segments();

	//allocate a buffer for the points
	Geo_Point *buffer = NULL;
	try {
		buffer = new Geo_Point[number_points];
	}
	catch (bad_alloc &t) {
		Error msg = this->set_error(26);
		ostringstream info;
		info << "Info bad alloc: " << t.what() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}

	//set the point buffer
	double y_min = -10000.0;

	Geo_Point_List buff_list;
	for (int i = 0; i < this->number_material_zones; i++) {
		buff_list.set_new_points(&this->material_zones[i]);
	}
	if (this->number_material_zones > 0) {
		y_min = buff_list.get_smallest_y();
	}
	//set the first point
	int counter = 0;
	if (this->foreland.get_number_segments() == 0) {
		buffer[counter].set_point_x_coordinate(this->waterside_cubature.get_segment(0)->point1.get_xcoordinate());
		buffer[counter].set_point_y_coordinate(y_min);
	}
	else {
		buffer[counter].set_point_x_coordinate(this->foreland.get_segment(0)->point1.get_xcoordinate());
		buffer[counter].set_point_y_coordinate(y_min);
	}
	counter++;
	//foreland
	for (int i = 0; i < this->foreland.get_number_segments(); i++) {
		buffer[counter] = this->foreland.get_segment(i)->point1;
		counter++;
	}
	//waterside cubature
	for (int i = 0; i < this->waterside_cubature.get_number_segments(); i++) {
		buffer[counter] = this->waterside_cubature.get_segment(i)->point1;
		counter++;
	}
	//crest
	buffer[counter] = this->crest_cubature.get_segment()->point1;
	counter++;
	//landside cubature
	for (int i = 0; i < this->landside_cubature.get_number_segments(); i++) {
		buffer[counter] = this->landside_cubature.get_segment(i)->point1;
		counter++;
	}
	//hinterland
	for (int i = 0; i < this->hinterland.get_number_segments(); i++) {
		buffer[counter] = this->hinterland.get_segment(i)->point1;
		counter++;
	}
	//set the two last point
	if (this->hinterland.get_number_segments() == 0) {
		buffer[counter] = this->landside_cubature.get_segment(this->landside_cubature.get_number_segments() - 1)->point2;
		counter++;
		buffer[counter].set_point_x_coordinate(this->landside_cubature.get_segment(this->landside_cubature.get_number_segments() - 1)->point2.get_xcoordinate());
		buffer[counter].set_point_y_coordinate(y_min);
		counter++;
	}
	else {
		buffer[counter] = this->hinterland.get_segment(this->hinterland.get_number_segments() - 1)->point2;
		counter++;
		buffer[counter].set_point_x_coordinate(this->hinterland.get_segment(this->hinterland.get_number_segments() - 1)->point2.get_xcoordinate());
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
}
//Calcualte the area of the cubature
void Fpl_Sec_Type_Dike::calc_area_cubature(void){
//make the polysegment
	//count the number of points, the first and the last are points horizontal to the waterside dike toe/landside dike toe
	int number_points;
	number_points=2+this->waterside_cubature.get_number_segments()+this->crest_cubature.get_number_segments()
		+this->landside_cubature.get_number_segments();

	//allocate a buffer for the points
	Geo_Point *buffer=NULL;
	try{
		buffer=new Geo_Point[number_points];
	}
	catch(bad_alloc &t){
		Error msg=this->set_error(27);
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
	if(buffer[counter-1].get_ycoordinate()<0.0){
		buffer[counter].set_point_coordinate(buffer[0].get_xcoordinate(),buffer[counter-1].get_ycoordinate());
	}
	else if (buffer[counter - 1].get_ycoordinate() == 0.0) {
		buffer[counter].set_point_coordinate(buffer[counter - 1].get_xcoordinate()/2, 0.0);
	}
	else{
		buffer[counter].set_point_coordinate(buffer[counter-1].get_xcoordinate(),buffer[0].get_ycoordinate());
	}
	



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
void Fpl_Sec_Type_Dike::check_fault_tree(void){
	//two piping mechanism
	if(this->piping_lane_event!=NULL && this->piping_sell_event!=NULL){
		if(this->piping_lane_event->get_in_tree_flag()==true && this->piping_sell_event->get_in_tree_flag()==true){
			Warning msg=this->set_warning(2);
			ostringstream info;
			info<<"Applied mechanism: Piping after Sellmeijer"<<endl;
			msg.make_second_info(info.str());
			msg.output_msg(1);
			//reaction
			this->piping_lane_event->set_in_tree_flag(false);
		}
	}

	//when zone are set but no impemeable zone is specified, than noch bursting or piping
	if(this->number_material_zones>0){
		if(this->get_ptr_specified_material_variable_zone(_fpl_zone_type::imperm_layer)==NULL){
			Warning msg=this->set_warning(4);
			msg.output_msg(1);
			this->delete_bursting_din_mechanism();
			this->delete_piping_lane_mechanism();
			this->delete_piping_schmertmann_mechanism();
			this->delete_piping_sell_mechanism();
		}
	}

	//no piping without bursting
	if(this->bursting_din_event==NULL){
		if((this->piping_lane_event!=NULL && this->piping_lane_event->get_2calc_flag()==true) ||
			(this->piping_sell_event!=NULL && this->piping_sell_event->get_2calc_flag()==true)){
				Warning msg=this->set_warning(3);
				msg.output_msg(1);
				this->delete_piping_lane_mechanism();
				this->delete_piping_schmertmann_mechanism();
				this->delete_piping_sell_mechanism();
		}
	}
	else{
		if(this->bursting_din_event->get_2calc_flag()==false){
			if((this->piping_lane_event!=NULL && this->piping_lane_event->get_2calc_flag()==true) ||
				(this->piping_sell_event!=NULL && this->piping_sell_event->get_2calc_flag()==true)){
					Warning msg=this->set_warning(3);
					msg.output_msg(1);
					this->delete_piping_lane_mechanism();
					this->delete_piping_schmertmann_mechanism();
					this->delete_piping_sell_mechanism();
			}
		}
	}

	//no wave impact or waterside erosion without wave run up
	/*if(this->wave_runup_event==NULL){
		if((this->wave_impact_event!=NULL && this->wave_impact_event->get_2calc_flag()==true) ||
			(this->erosion_waterside_event!=NULL && this->erosion_waterside_event->get_2calc_flag()==true) || 
			(this->erosion_waterside_start_event!=NULL && this->erosion_waterside_start_event->get_2calc_flag()==true) ){
				Warning msg=this->set_warning(7);
				msg.output_msg(1);
				this->delete_erosion_waterside_mechanism();
				this->delete_wave_impact_mechanism();
				this->delete_start_erosion_waterside_mechanism();
		}
	}
	else{
		if(this->wave_runup_event->get_2calc_flag()==false){
			if((this->wave_impact_event!=NULL && this->wave_impact_event->get_2calc_flag()==true) ||
				(this->erosion_waterside_event!=NULL && this->erosion_waterside_event->get_2calc_flag()==true)|| 
				(this->erosion_waterside_start_event!=NULL && this->erosion_waterside_start_event->get_2calc_flag()==true) ){
					Warning msg=this->set_warning(7);
					msg.output_msg(1);
					this->delete_erosion_waterside_mechanism();
					this->delete_wave_impact_mechanism();
					this->delete_start_erosion_waterside_mechanism();
			}
		}
	}*/

}
//Check the gradient of a dike for their physical sense
void Fpl_Sec_Type_Dike::check_dike_gradients(const double grad, const bool waterside){
	ostringstream cub;
	if(waterside==true){
		cub << " Waterside cubature"<<endl;
	}
	else{
		cub << " Landside cubature"<<endl;
	}

	//flat
	if(grad <0.10 && grad >= 0.05){
		//warning
		Warning msg=this->set_warning(5);
		ostringstream info;
		info << cub.str();
		info << " Mid gradient : " << grad << label::no_unit <<endl;
		msg.make_second_info(info.str());
		msg.output_msg(1);
	}
	if(grad <0.05){
		//Error
		Error msg=this->set_error(33);
		ostringstream info;
		info << cub.str();
		info << " Mid gradient : " << grad << label::no_unit <<endl;
		msg.make_second_info(info.str());
		throw msg;
	}
	//step
	if(grad > 1.5 && grad <= 2.5){
		//warning
		Warning msg=this->set_warning(6);
		ostringstream info;
		info << cub.str();
		info << " Mid gradient : " << grad << label::no_unit <<endl;
		msg.make_second_info(info.str());
		msg.output_msg(1);
	}
	if(grad >2.5){
		//Error
		Error msg=this->set_error(32);
		ostringstream info;
		info << cub.str();
		info << " Mid gradient : " << grad << label::no_unit <<endl;
		msg.make_second_info(info.str());
		throw msg;
	}
}
//Reset the flags for the random generation
void Fpl_Sec_Type_Dike::reset_random_flag(void){
	_Fpl_Section_Type::reset_random_flag();
	this->water_density.reset_random_generated();

	for(int i=0; i< this->number_material_variable_zones; i++){
		this->material_variable_zones[i].reset_random_flag();
	}
}
//Output the seepage line to tecplot 
void Fpl_Sec_Type_Dike::output_seepage2tecplot(const string seepage_file){
	//first calculate the seepage line for output
	Fpl_Seepage_Line_Point_List buff_ascending;
	Fpl_Seepage_Line_Point_List buff_descending;
	Geo_Point crest_mid=this->crest_cubature.get_segment()->calc_mid_point();
	//set the mid point to the list...it has always the index 0
	buff_ascending.add_new_point(crest_mid.get_xcoordinate(),0.0, true, &this->outer_polysegment);
	buff_descending.add_new_point(crest_mid.get_xcoordinate(),0.0, true, &this->outer_polysegment);

	//set some points for the visualisation
	double x_buff=-1.5;

	do{
		if(x_buff>=this->outer_polysegment.get_first_point()->get_xcoordinate() && x_buff<=this->outer_polysegment.get_last_point()->get_xcoordinate()){
			buff_ascending.add_new_point(x_buff,0.0, true, &this->outer_polysegment);
			buff_descending.add_new_point(x_buff,0.0, true, &this->outer_polysegment);
		}

		x_buff=x_buff+0.05;
	}
	while(x_buff<=this->base_land.get_xcoordinate()+1.0);

	this->seepage_calculator.calculate_waterlevel_seepage_line(this->current_waterlevel, &buff_ascending, &buff_descending, true);

	//open the file
	ofstream tecplot_output;
	tecplot_output.open(seepage_file.c_str());
	if(tecplot_output.is_open()==false){
		Error msg=this->set_error(29);
		ostringstream info;
		info << "Filename : " << seepage_file << endl;
		msg.make_second_info(info.str());
		throw msg;
	}

	//output the file header
	tecplot_output << "TITLE = "<< "\" Seepage lines  \"" << endl;
	tecplot_output << "VARIABLES = " <<endl;
	tecplot_output << "\" x " << label::m << "\" "<<endl;
	tecplot_output << "\" z " << label::m << "\" "<< endl;
	tecplot_output <<endl<<endl<<endl;
	tecplot_output << P(4) << FORMAT_FIXED_REAL;

	tecplot_output <<"#######seepage landside########"<<endl;
	//if(this->foreland.get_number_segments()>0){
		tecplot_output << endl << "ZONE T= "<< "\" Seepage landside \" "<< " I = " << buff_ascending.count_number_inside_points()<< endl;
		buff_ascending.output_members2tecplot(&tecplot_output, &this->outer_polysegment);
	//}
	tecplot_output.flush();

	tecplot_output <<"#######seepage waterside########"<<endl;
	//if(this->foreland.get_number_segments()>0){
		tecplot_output << endl << "ZONE T= "<< "\" Seepage waterside \" "<< " I = " << buff_descending.count_number_inside_points()<< endl;
		buff_descending.output_members2tecplot(&tecplot_output, &this->outer_polysegment);
	//}
	tecplot_output.flush();

	//close the file
	tecplot_output.close();
}
//Output the seepage line to paraview
void Fpl_Sec_Type_Dike::output_seepage2paraview(const string seepage_file) {

	//first calculate the seepage line for output
	Fpl_Seepage_Line_Point_List buff_ascending;
	Fpl_Seepage_Line_Point_List buff_descending;
	Geo_Point crest_mid = this->crest_cubature.get_segment()->calc_mid_point();
	//set the mid point to the list...it has always the index 0
	buff_ascending.add_new_point(crest_mid.get_xcoordinate(), 0.0, true, &this->outer_polysegment);
	buff_descending.add_new_point(crest_mid.get_xcoordinate(), 0.0, true, &this->outer_polysegment);

	//set some points for the visualisation
	double x_buff = -1.5;

	do {
		if (x_buff >= this->outer_polysegment.get_first_point()->get_xcoordinate() && x_buff <= this->outer_polysegment.get_last_point()->get_xcoordinate()) {
			buff_ascending.add_new_point(x_buff, 0.0, true, &this->outer_polysegment);
			buff_descending.add_new_point(x_buff, 0.0, true, &this->outer_polysegment);
		}

		x_buff = x_buff + 0.05;
	} while (x_buff <= this->base_land.get_xcoordinate() + 1.0);

	this->seepage_calculator.calculate_waterlevel_seepage_line(this->current_waterlevel, &buff_ascending, &buff_descending, true);



	//open the file
	ofstream output;
	output.open(seepage_file.c_str());
	if (output.is_open() == false) {
		Error msg = this->set_error(44);
		ostringstream info;
		info << "Filename : " << seepage_file << endl;
		msg.make_second_info(info.str());
		throw msg;
	}

	output << P(4) << FORMAT_FIXED_REAL;
	//header
	output << "x,";
	int count_col_tot = 1;
	output << "z_seepage_landside,";
	count_col_tot++;
	output << "z_seepage_waterside";
	count_col_tot++;
	output << endl;
	output.flush();
	int counter_col_before = 0;
	int counter_col_after = count_col_tot - 1;
	counter_col_after--;
	buff_ascending.output_members2paraview(&output, &this->outer_polysegment, counter_col_before, counter_col_after);
	counter_col_before++;
	counter_col_after--;
	buff_descending.output_members2paraview(&output, &this->outer_polysegment, counter_col_before, counter_col_after);
	counter_col_before++;


	output.flush();
	//close the file
	output.close();
}
//Output the seepage line to excel
void Fpl_Sec_Type_Dike::output_seepage2excel(const string seepage_file) {

	//first calculate the seepage line for output
	Fpl_Seepage_Line_Point_List buff_ascending;
	Fpl_Seepage_Line_Point_List buff_descending;
	Geo_Point crest_mid = this->crest_cubature.get_segment()->calc_mid_point();
	//set the mid point to the list...it has always the index 0
	buff_ascending.add_new_point(crest_mid.get_xcoordinate(), 0.0, true, &this->outer_polysegment);
	buff_descending.add_new_point(crest_mid.get_xcoordinate(), 0.0, true, &this->outer_polysegment);

	//set some points for the visualisation
	double x_buff = -1.5;

	do {
		if (x_buff >= this->outer_polysegment.get_first_point()->get_xcoordinate() && x_buff <= this->outer_polysegment.get_last_point()->get_xcoordinate()) {
			buff_ascending.add_new_point(x_buff, 0.0, true, &this->outer_polysegment);
			buff_descending.add_new_point(x_buff, 0.0, true, &this->outer_polysegment);
		}

		x_buff = x_buff + 0.05;
	} while (x_buff <= this->base_land.get_xcoordinate() + 1.0);

	this->seepage_calculator.calculate_waterlevel_seepage_line(this->current_waterlevel, &buff_ascending, &buff_descending, true);



	//open the file
	ofstream output;
	output.open(seepage_file.c_str());
	if (output.is_open() == false) {
		Error msg = this->set_error(45);
		ostringstream info;
		info << "Filename : " << seepage_file << endl;
		msg.make_second_info(info.str());
		throw msg;
	}

	output << P(4) << FORMAT_FIXED_REAL;
	//header
	output << "x;";
	int count_col_tot = 1;
	output << "z_seepage_landside;";
	count_col_tot++;
	output << "z_seepage_waterside";
	count_col_tot++;
	output << endl;
	output.flush();
	int counter_col_before = 0;
	int counter_col_after = count_col_tot - 1;
	counter_col_after--;
	buff_ascending.output_members2excel(&output, &this->outer_polysegment, counter_col_before, counter_col_after);
	counter_col_before++;
	counter_col_after--;
	buff_descending.output_members2excel(&output, &this->outer_polysegment, counter_col_before, counter_col_after);
	counter_col_before++;


	output.flush();
	//close the file
	output.close();
}
//Get a pointer to a material variable zone
Fpl_Dike_Var_Materialzone* Fpl_Sec_Type_Dike::get_ptr_specified_material_variable_zone(_fpl_zone_type specifier){
	int index=-1;
	Fpl_Dike_Var_Materialzone *zone=NULL;
	for(int i=0; i< this->number_material_zones; i++){
		if(this->material_zones[i].get_zone_type()==specifier){
			index=this->material_zones[i].get_variable_zone_number();
		}
	}
	if(index==-1){
		return zone;
	}

	for(int i=0; i< this->number_material_variable_zones; i++){
		if(this->material_variable_zones[i].get_zone_id()==index){
			return &this->material_variable_zones[i];
		}
	}

	return NULL;
}
//Count the number of grass cover zones and artificial revetments of the waterside
void Fpl_Sec_Type_Dike::count_waterside_zones(int *no_artif, int *no_grass){

	int buff1=0;
	int buff2=0;

	for(int i=0; i< this->number_mat_waterside_zones; i++){
		if(this->material_waterside_zone[i].get_grass_cover_flag()==true){
			buff2++;
		}
		else{
			buff1++;
		}

	}

	*no_artif=buff1;
	*no_grass=buff1;
}
//set the error
Error Fpl_Sec_Type_Dike::set_error(const int err_type){
		string place="Fpl_Sec_Type_Dike::";
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
		case 1://bad_alloc
			place.append("allocate_piping_sell_mechanism(void)");
			reason="Can not allocate the memory";
			help="Check the memory";
			type=10;
			break;
		case 2://bad_alloc
			place.append("allocate_wind_wave_mechanism(void)");
			reason="Can not allocate the memory";
			help="Check the memory";
			type=10;
			break;
		case 3://bad_alloc
			place.append("allocate_bursting_din_mechanism(void)");
			reason="Can not allocate the memory";
			help="Check the memory";
			type=10;
			break;
		case 4://bad_alloc
			place.append("allocate_piping_lane_mechanism(void))");
			reason="Can not allocate the memory";
			help="Check the memory";
			type=10;
			break;
		case 5://bad_alloc
			place.append("allocate_wave_runup_mechanism(void)");
			reason="Can not allocate the memory";
			help="Check the memory";
			type=10;
			break;
		case 6://bad_alloc
			place.append("allocate_overflow_mechanism(void)");
			reason="Can not allocate the memory";
			help="Check the memory";
			type=10;
			break;
		case 8://bad_alloc
			place.append("allocate_slope_stability_landside_mechanism(void)");
			reason="Can not allocate the memory";
			help="Check the memory";
			type=10;
			break;
		case 9://bad_alloc
			place.append("allocate_material_zones(void)");
			reason="Can not allocate the memory";
			help="Check the memory";
			type=10;
			break;
		case 10://bad_alloc
			place.append("allocate_slope_stability_waterside_mechanism(void)");
			reason="Can not allocate the memory";
			help="Check the memory";
			type=10;
			break;
		case 11://no interception is found
			place.append("calculate_waterlevel_seepage_line(const double water_level)");
			reason="No interception point of the waterlevel with the waterside dike cubature is found";
			help="Code has to be verified";
			type=6;
			break;
		case 12://no material zones are found
			place.append("read_matzones(QSqlDatabase *ptr_database, const int section_id)");
			reason="No material zones are found in database table";
			help="Check the material zones";
			type=2;
			break;
		case 13://wrong input
			place.append("read_geo_matzones(QFile *ifile, int *line_counter)");
			reason="There is a problem with the input; wrong sign is read in";
			help="Check the information of the material zones in file";
			type=1;
			break;
		case 14://no material zone is found in file
			place.append("read_geo_matzones(QFile *ifile, int *line_counter)");
			reason="No material zone is found in file, which is required for the stability calculation";
			help="Check the information of the material zones in file; check the information of the stability calculation";
			type=1;
			break;
		case 15://bad_alloc
			place.append("allocate_material_variable_zone(void)");
			reason="Can not allocate the memory";
			help="Check the memory";
			type=10;
			break;
		case 16://wrong input
			place.append("read_var_matzones(QFile *ifile, int *line_counter)");
			reason="There is a problem with the input; wrong sign is read in";
			help="Check the information of the material variable zones in file";
			type=1;
			break;
		case 17://no material zone is found in file
			place.append("read_var_matzones(QFile *ifile, int *line_counter)");
			reason="No material zone is found in file";
			help="Check the information of the material variable zones in file";
			type=1;
			break;
		case 18://no material variable zone is found to the material zone
			place.append("connect_material_zones(void)");
			reason="No material variable zone is found to the material zone of the dike section";
			help="Check the geometrical and variable material zone information";
			type=27;
			break;
		case 19://same material variable zone id is found
			place.append("connect_material_zones(void)");
			reason="Same material variable zone id are found";
			help="Check the variable material zone information";
			type=27;
			break;
		case 20://material zone are intercepting each other
			place.append("check_set_geometry(void)");
			reason="The material zone polygons are intercepting each other";
			help="Check the material zone information";
			type=27;
			break;
		case 21://not completely inside
			place.append("check_set_geometry(void)");
			reason="The material zone polygons are not completely inside the outer polygon of the section";
			help="Check the material zone information";
			type=27;
			break;
		case 22://not all is filled
			place.append("check_set_geometry(void)");
			reason="The outer polygon of the section is not completely filled with material zones";
			help="Check the material zone information";
			type=27;
			break;
		case 23://material variable zone is smaller/equal zero
			place.append("check_set_geometry(void)");
			reason="The id of the material variable zone has to be greater than 0";
			help="Check the zone id of the material variable zone(s)";
			type=27;
			break;
		case 24://no equal material variable zone ids allowed
			place.append("check_set_geometry(void)");
			reason="Equal ids of the material variable zone(s) are not allowed";
			help="Check the zone id of the material variable zone(s)";
			type=27;
			break;
		case 25://bad_alloc
			place.append("make_geometrical_outer_dike_polysegment(void)");
			reason="Can not allocate the memory";
			help="Check the memory";
			type=10;
			break;
		case 26://bad_alloc
			place.append("make_geometrical_outer_dike_polygon(void)");
			reason="Can not allocate the memory";
			help="Check the memory";
			type=10;
			break;
		case 27://bad_alloc
			place.append("calc_area_cubature(void)");
			reason="Can not allocate the memory";
			help="Check the memory";
			type=10;
			break;
		case 28://bad_alloc
			place.append("allocate_piping_schmertmann_mechanism(void))");
			reason="Can not allocate the memory";
			help="Check the memory";
			type=10;
			break;
		case 29://could not open the tecplot file
			place.append("output_seepage2tecplot(const string seepage_file)");
			reason="Could not open the file for the tecplot output (seepage) of the FPL-section";
			help="Check the file";
			type=5;
			break;
		case 30://more than one impermeable layer is found
			place.append("check_set_geometry(void)");
			reason="There are more than one impermeable layer as material zone found";
			help="Check the zone type of the material zone(s)";
			type=27;
			break;
		case 31://more than one permeable layer is found
			place.append("check_set_geometry(void)");
			reason="There are more than one permeable layer as material zone found";
			help="Check the zone type of the material zone(s)";
			type=27;
			break;
		case 32://very step gradient
			place.append("check_dike_gradients(const double grad, const bool waterside)");
			reason="The gradient of the dike cubature is to step for a dike";
			help="Check the dike cubature and the base point";
			type=1;
			break;
		case 33://very flat gradient
			place.append("check_dike_gradients(const double grad, const bool waterside)");
			reason="The gradient of the dike cubature is to flat for a dike";
			help="Check the dike cubature and the base point";
			type=11;
			break;
		case 34://bad_alloc
			place.append("allocate_erosion_landside_mechanism(void)");
			reason="Can not allocate the memory";
			help="Check the memory";
			type=10;
			break;
		case 35://bad_alloc
			place.append("allocate_erosion_waterside_mechanism(void)");
			reason="Can not allocate the memory";
			help="Check the memory";
			type=10;
			break;
		case 36://bad_alloc
			place.append("allocate_wave_impact_mechanism(void)");
			reason="Can not allocate the memory";
			help="Check the memory";
			type=10;
			break;
		case 37://bad_alloc
			place.append("allocate_waterside_variable_zone(void)");
			reason="Can not allocate the memory";
			help="Check the memory";
			type=10;
			break;
		case 38://no material zones are found
			place.append("read_mat_variable_zones_waterside(QSqlDatabase *ptr_database, const int section_id)");
			reason="No material zones of the waterside are found in database table";
			help="Check the waterside material zones";
			type=2;
			break;
		case 39://same material variable zone id is found
			place.append("connect_material_zones_waterside(void)");
			reason="Same waterside material variable zone id are found";
			help="Check the waterside variable material zone information";
			type=27;
			break;
		case 40://no material variable zone is found to the material zone
			place.append("connect_material_zones_waterside(void)");
			reason="No waterside material variable zone is found to the material zone of the waterside cubature";
			help="Check the geometrical and variable material zone information of the waterside cubature";
			type=27;
			break;
		case 41://wrong input
			place.append("read_var_waterside_matzones(QFile *ifile, int *line_counter)");
			reason="There is a problem with the input; wrong sign is read in";
			help="Check the information of the waterside material variable zones in file";
			type=1;
			break;
		case 42://no material zone is found in file
			place.append("read_var_waterside_matzones(QFile *ifile, int *line_counter)");
			reason="No material zone is found in file";
			help="Check the information of the waterside material variable zones in file";
			type=1;
			break;
		case 43://bad_alloc
			place.append("allocate_start_erosion_waterside_mechanism(void)");
			reason="Can not allocate the memory";
			help="Check the memory";
			type=10;
			break;
		case 44://could not open the paraview file
			place.append("output_seepage2paraview(const string seepage_file)");
			reason = "Could not open the file for the paraview output (seepage) of the FPL-section";
			help = "Check the file";
			type = 5;
			break;
		case 45://could not open the excel file
			place.append("output_seepage2excel(const string seepage_file)");
			reason = "Could not open the file for the excel output (seepage) of the FPL-section";
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
Warning Fpl_Sec_Type_Dike::set_warning(const int warn_type){
	string place="Fpl_Sec_Type_Dike::";
		string help;
		string reason;
		string reaction;
		int type=0;
		Warning msg;
		stringstream info;

	switch (warn_type){
		case 0://no mechanism is set
			place.append("init_fault_tree(QSqlDatabase *ptr_database)");
			reason="No mechanism is set for the section type dike";
			help="Check the fault tree for the section type";
			reaction="No reliabilty calculation required";
			type=6;
			break;
		case 1://results datas can not submitted
			place.append("output_result2table(QSqlDatabase *ptr_database, _fpl_simulation_type simulation_type, _sys_system_id id, const int section_id)");
			reason="Can not submit the mechanism result data to the database";
			help="Check the database";
			type=2;
			break;
		case 2://both piping mechanism are not possible
			place.append("check_fault_tree(void)");
			reason="There are multiple piping mechanism applied in the fault-tree";
			help="Check the fault-tree settings";
			reaction="Just one piping mechanisms will be applied in the fault-tree";
			type=20;
			break;
		case 3://no piping without bursting mechanism
			place.append("check_fault_tree(void)");
			reason="There can not be a piping mechanism without any bursting mechanism before";
			help="Check the fault-tree settings";
			reaction="No piping mechanism is applied";
			type=20;
			break;
		case 4://no bursting mechanism, when no impermeable zone is specified
			place.append("check_fault_tree(void)");
			reason="There are no impermeable zone specified; the bursting mechanism is not applied";
			help="Check the fault-tree settings and the material zone settings of the dike";
			reaction="No bursting mechansim and consequently no piping mechanisms are applied";
			type=20;
			break;
		case 5://very flat gradient
			place.append("check_dike_gradients(const double grad, const bool waterside)");
			reason="The gradient of the dike cubature is very flat for a dike";
			help="Check the dike cubature and the base point";
			type=8;
			break;
		case 6://very step gradient
			place.append("check_dike_gradients(const double grad, const bool waterside)");
			reason="The gradient of the dike cubature is very step for a dike";
			help="Check the dike cubature and the base point";
			type=8;
			break;
		case 7://no wave impact or erosion water, when no wave runaup is specified
			place.append("check_fault_tree(void)");
			reason="There is no wave runup event specifeid; the wave impact mechanism and the waterside erosion mechanism is not applied";
			help="Check the fault-tree settings and the mechanism settings of the dike";
			reaction="No wave impact mechanism and no waterside erosion mechanism are applied";
			type=20;
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
