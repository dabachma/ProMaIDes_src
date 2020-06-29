#include "Fpl_Headers_Precompiled.h"
//#include "Fpl_Mech_Waterside_Erosion.h"

//Default constructor
Fpl_Mech_Waterside_Erosion::Fpl_Mech_Waterside_Erosion(void){


	this->failure=false;
	this->buff_res=0.0;
	this->buff_stress=0.0;

	this->waterside_cubature=NULL;

	this->resistance.set_unit(label::sec);
	this->stress.set_unit(label::sec);

	this->wave_attack_duration.set_unit(label::sec);

	Sys_Memory_Count::self()->add_mem(sizeof(Fpl_Mech_Waterside_Erosion)-sizeof(Fpl_Random_Variables)*3,_sys_system_modules::FPL_SYS);//count the memory

}
//Default destructor
Fpl_Mech_Waterside_Erosion::~Fpl_Mech_Waterside_Erosion(void){

	Sys_Memory_Count::self()->minus_mem(sizeof(Fpl_Mech_Waterside_Erosion)-sizeof(Fpl_Random_Variables)*3,_sys_system_modules::FPL_SYS);//count the memory

}
//_________
//public
//Set the random variables of the result members
void Fpl_Mech_Waterside_Erosion::set_result_variables(QSqlDatabase *ptr_database, const _sys_system_id id, const int section_id){
	try{
		this->resistance.set_input(id, section_id,fpl_label::res_waterside_erosion, true, ptr_database, fpl_label::mech_erosion_waterside, fpl_label::sec_dike, 0, false);
		this->resistance.set_distribution_types(ptr_database);

		this->stress.set_input(id, section_id,fpl_label::stress_waterside_erosion, true, ptr_database, fpl_label::mech_erosion_waterside, fpl_label::sec_dike, 0, false);
		this->stress.set_distribution_types(ptr_database);

	}catch(Error msg){
		throw msg;
	}
}
//Set the random input variables per database table
void Fpl_Mech_Waterside_Erosion::set_variables(QSqlDatabase *ptr_database, const _sys_system_id id, const int section_id){
	try{
		this->wave_attack_duration.set_input(id, section_id,fpl_label::duration_wave_attack, false, ptr_database, fpl_label::mech_erosion_waterside, fpl_label::sec_dike, 0, false);
		this->wave_attack_duration.set_distribution_types(ptr_database);
		
	}catch(Error msg){
		throw msg;
	}
}
//Set the random input variables per file
void Fpl_Mech_Waterside_Erosion::set_variables(QFile *ifile, int *line_counter, const qint64 pos_file, const int line_counter_start){
	ostringstream prefix;
	prefix << "WATERSIDE_ERO> ";
	Sys_Common_Output::output_fpl->set_userprefix(prefix.str());
	ostringstream cout;
	cout << "Input variable members per file..." << endl;
	Sys_Common_Output::output_fpl->output_txt(&cout);
	
	try{

		this->wave_attack_duration.set_input(ifile, line_counter,fpl_label::duration_wave_attack, false, fpl_label::end_mech_waterside_ero);
		this->wave_attack_duration.check_positiv_zero_variable();
		ifile->seek(pos_file);
		*line_counter=line_counter_start;

		//results
		this->resistance.set_input(ifile, line_counter,fpl_label::res_waterside_erosion, true, fpl_label::end_mech_waterside_ero);
		this->resistance.check_positiv_zero_variable();
		ifile->seek(pos_file);
		*line_counter=line_counter_start;

		this->stress.set_input(ifile, line_counter,fpl_label::stress_waterside_erosion, true, fpl_label::end_mech_waterside_ero);
		this->stress.check_positiv_zero_variable();
		ifile->seek(pos_file);
		*line_counter=line_counter_start;

	}
	catch(Error msg){
		Sys_Common_Output::output_fpl->rewind_userprefix();
		throw msg;
	}

	cout << "Input of variable members per file is finsihed" << endl;
	Sys_Common_Output::output_fpl->output_txt(&cout);

	Sys_Common_Output::output_fpl->rewind_userprefix();
}
//Transfer the bursting-mechanism (DIN 19712) data to database, e.g. geometry, random variables etc.
void Fpl_Mech_Waterside_Erosion::transfer_data2database(const int section_id, const _sys_system_id id, QSqlDatabase *ptr_database){
	ostringstream prefix;
	ostringstream cout;
	prefix << "WATERSIDE_ERO> ";
	Sys_Common_Output::output_fpl->set_userprefix(prefix.str());

	cout << "Transfer variable members to database..."<<endl;
	Sys_Common_Output::output_fpl->output_txt(&cout);
	this->wave_attack_duration.transfer_rand2database(ptr_database,id, section_id, fpl_label::mech_erosion_waterside, fpl_label::sec_dike);

	//result members
	cout << "Transfer result members to database..."<<endl;
	Sys_Common_Output::output_fpl->output_txt(&cout);
	this->resistance.transfer_rand2database(ptr_database,id, section_id, fpl_label::mech_erosion_waterside, fpl_label::sec_dike);
	this->stress.transfer_rand2database(ptr_database,id, section_id, fpl_label::mech_erosion_waterside, fpl_label::sec_dike);
	Sys_Common_Output::output_fpl->rewind_userprefix();
}
//Output members of the mechanism to display/console
void Fpl_Mech_Waterside_Erosion::output_members(void){
	ostringstream prefix;
	prefix << "WATERSIDE_ERO> ";
	Sys_Common_Output::output_fpl->set_userprefix(prefix.str());
	ostringstream cout;
	cout << "Variable members mechanism erosion of the waterside slope..." << endl;
	Sys_Common_Output::output_fpl->output_txt(&cout,true);
	this->wave_attack_duration.output_member();
	
	cout << "Result members mechanism erosion of the waterside slope..." << endl;
	Sys_Common_Output::output_fpl->output_txt(&cout,true);
	this->resistance.output_member();
	this->stress.output_member();
	

	Sys_Common_Output::output_fpl->rewind_userprefix();
}
//Check the statistic of the random variables of the mechanism
void Fpl_Mech_Waterside_Erosion::check_statistic(void){
	this->wave_attack_duration.check_statistic();
	this->resistance.check_statistic();
	this->stress.check_statistic();
}
//Reset the statistic important values
void Fpl_Mech_Waterside_Erosion::reset_statistics(void){
	this->wave_attack_duration.reset_statistics();
	this->resistance.reset_statistics();
	this->stress.reset_statistics();

	this->reset_counter_prob();
}
//Output the statistics of the random variables of the mechanism to display/console
void Fpl_Mech_Waterside_Erosion::output_statistic(void){
	ostringstream prefix;
	prefix << "WATERSIDE_ERO> ";
	Sys_Common_Output::output_fpl->set_userprefix(prefix.str());
	ostringstream cout;
	cout << "Variable members mechanism erosion of the waterside slope..." << endl;
	Sys_Common_Output::output_fpl->output_txt(&cout,true);
	this->wave_attack_duration.output_statistic();
	

	cout << "Result members mechanism erosion of the waterside slope..." << endl;
	Sys_Common_Output::output_fpl->output_txt(&cout,true);
	this->resistance.output_statistic();
	this->stress.output_statistic();

	Sys_Common_Output::output_fpl->rewind_userprefix();
}
//Output the results of a deterministic calculation to display/console
void Fpl_Mech_Waterside_Erosion::output_determ_results(void){
	ostringstream prefix;
	prefix << "WATERSIDE_ERO> ";
	Sys_Common_Output::output_fpl->set_userprefix(prefix.str());
	ostringstream cout;
	cout << "Deterministic results mechanism erosion of the waterside slope..."<< endl;
	if(this->wave2zero==false){
		cout << " Wave attack duration      : "<< this->wave_attack_duration.get_result().end_result<<label::sec << endl;
		Sys_Common_Output::output_fpl->output_txt(&cout,false);
		cout << "Result members mechanism erosion of the waterside slope..." << endl;
		cout << " Resistance                : "<< this->resistance.get_result().end_result<<label::sec<<endl;
		cout << " Stress                    : "<< this->stress.get_result().end_result<<label::sec<<endl;
		cout << "Failure in segment(s)..."<<endl;
		for(int i=0; i< this->waterside_cubature->get_number_segments(); i++){
			if(this->waterside_cubature->get_segment(i)->get_erosion_failed()==true){
				cout << " Segmentno. " << i <<"; Zone-Id "<<this->waterside_cubature->get_segment(i)->ptr_mat_zone->get_zone_id()<<"; ";
				if(this->waterside_cubature->get_segment(i)->ptr_mat_zone->get_grass_cover_flag()==true){
						cout << " grass ";
				}
				else{
					cout << " artificial ";
				}
				cout <<endl;
			}
		}
	}
	else{
		cout << " Wave height is reduced to zero due to the wave direction!" << endl;
	}
	Sys_Common_Output::output_fpl->output_txt(&cout,false);
	Sys_Common_Output::output_fpl->rewind_userprefix();
}
//Calculate the mechanism; main input parameter is discharge over the waterside slope
bool Fpl_Mech_Waterside_Erosion::calculate_mechanism(const bool random_calculation, const double waterlevel, double wave_height, const double section_direction, const double wave_direction){
	this->failure=false;
	//reset the random flags
	this->reset_random_flag();

	double buffer=0.0;
	this->calculate_wavedirection_reduction(wave_direction, section_direction, &wave_height, &buffer);

	if(waterlevel==0.0 || wave_height==0.0 || this->waterside_cubature==NULL){
		return this->failure;
	}
	

	double height_up=0.0;
	double height_down=0.0;
	height_up=waterlevel+1.5*wave_height;
	height_down=waterlevel-1.5*wave_height;
	if(height_down<0.0){
		height_down=0.0;
	}

	double pow_w_h=pow(wave_height,2.0);

	this->wave_attack_duration.calculate_variables(random_calculation);
	//check the relevant segments
	double diff=-9999999.0;
	double buff1=0.0;
	double buff2=0.0;
	
	//stress
	buff2=this->wave_attack_duration.get_result().end_result;

	for(int i=0; i< this->waterside_cubature->get_number_segments(); i++){
		if(this->waterside_cubature->get_segment(i)->get_erosion_start()==true){
			if((this->waterside_cubature->get_segment(i)->point1.get_ycoordinate()<=height_down && this->waterside_cubature->get_segment(i)->point2.get_ycoordinate()>=height_down)||
				(this->waterside_cubature->get_segment(i)->point1.get_ycoordinate()>=height_down && this->waterside_cubature->get_segment(i)->point2.get_ycoordinate()<=height_up)||
				(this->waterside_cubature->get_segment(i)->point1.get_ycoordinate()<=height_up && this->waterside_cubature->get_segment(i)->point2.get_ycoordinate()>=height_up)){
				//check the segment
				if(this->waterside_cubature->get_segment(i)->ptr_mat_zone!=NULL){
					//grass cover
					if(this->waterside_cubature->get_segment(i)->ptr_mat_zone->get_grass_cover_flag()==true){
						//resistance
						//calculate the thickness of the layer
						double d_rest;
						double d_grass;
						d_rest=this->waterside_cubature->get_segment(i)->ptr_mat_zone->cover_thickness_variables.get_result().end_result-
							this->waterside_cubature->get_segment(i)->ptr_mat_zone->rand_grass->root_zone_thickness_variables.get_result().end_result;
						if(d_rest<0.0){
							d_rest=0.0;
						}

						buff1=this->waterside_cubature->get_segment(i)->ptr_mat_zone->cover_thickness_variables.get_result().end_result-
							this->waterside_cubature->get_segment(i)->ptr_mat_zone->rand_grass->crack_thickness_variables.get_result().end_result;
						if(buff1<0.0){
							buff1=0.0;
						}
						d_rest=min(d_rest,buff1);
						if(this->waterside_cubature->get_segment(i)->ptr_mat_zone->rand_grass->crack_thickness_variables.get_result().end_result>
							this->waterside_cubature->get_segment(i)->ptr_mat_zone->rand_grass->root_zone_thickness_variables.get_result().end_result){
							d_grass=0.0;
						}
						else{
							d_grass=this->waterside_cubature->get_segment(i)->ptr_mat_zone->rand_grass->root_zone_thickness_variables.get_result().end_result-
								this->waterside_cubature->get_segment(i)->ptr_mat_zone->rand_grass->crack_thickness_variables.get_result().end_result;
						}
						//calculate the duration
						double t_rest;
						double t_grass;
						t_rest=d_rest/(this->waterside_cubature->get_segment(i)->ptr_mat_zone->rand_grass_cover_ero->stability_under_layer.get_result().end_result*
							pow_w_h*this->waterside_cubature->get_segment(i)->get_sin_alpha());
						t_grass=d_grass/(this->waterside_cubature->get_segment(i)->ptr_mat_zone->rand_grass_cover_ero->grass_quality.get_result().end_result*
							pow_w_h);
						buff1=t_rest+t_grass;

						if(buff2>buff1){
							this->waterside_cubature->get_segment(i)->set_erosion_failed(true);
						}

						//diff
						if(diff<=buff2-buff1){
							diff=buff2-buff1;
							this->buff_res=buff1;
							this->buff_stress=buff2;
						}
					}
					//artificial revetment
					else{

						//resistance
						//calculate the thickness of the under laying zone
						double d_rest;
						d_rest=this->waterside_cubature->get_segment(i)->ptr_mat_zone->rand_arti_revet_ero->thickness_under_layer.get_result().end_result;
						if(d_rest<0.0){
							d_rest=0.0;
						}
						//calculate the duratiob
						double t_rest;
						t_rest=d_rest/(this->waterside_cubature->get_segment(i)->ptr_mat_zone->rand_arti_revet_ero->stability_under_layer.get_result().end_result*
							pow_w_h*this->waterside_cubature->get_segment(i)->get_sin_alpha());
				
						buff1=t_rest;
					
						if(buff2>buff1){
							this->waterside_cubature->get_segment(i)->set_erosion_failed(true);
						}
						//diff
						if(diff<=buff2-buff1){
							diff=buff2-buff1;
							this->buff_res=buff1;
							this->buff_stress=buff2;
						}
					}
				}
			}
		}
	}

	
	//set the results variables
	this->resistance.set_new_middle(this->buff_res);
	this->resistance.calculate_variables(random_calculation);
	this->stress.set_new_middle(this->buff_stress);
	this->stress.calculate_variables(random_calculation);

	//check for failure
	if(this->stress.get_result().end_result>this->resistance.get_result().end_result){
		//failure
		this->failure=true;
	}
	else{
		this->failure=false;
	}

	return this->failure;
}
//Set the pointer to the waterside cubature, which is allocated in the dike-section class
void Fpl_Mech_Waterside_Erosion::set_waterside_cubature(Fpl_Cub_Dike_Waterside *waterside_cubature){
	this->waterside_cubature=waterside_cubature;
}
//__________
//private
//Reset the random generated flags of the random variables
void Fpl_Mech_Waterside_Erosion::reset_random_flag(void){
	this->wave_attack_duration.reset_random_generated();
	this->resistance.reset_random_generated();
	this->stress.reset_random_generated();

	//reset erosion start flag at the waterside cubature
	for(int i=0; i< this->waterside_cubature->get_number_segments(); i++){
		this->waterside_cubature->get_segment(i)->set_erosion_failed(false);

	}
}
//Set warning(s)
Warning Fpl_Mech_Waterside_Erosion::set_warning(const int warn_type){
		string place="Fpl_Mech_Waterside_Erosion::";
		string help;
		string reason;
		string reaction;
		int type=0;
		Warning msg;
		stringstream info;

	switch (warn_type){
		case 0://no mechanism is set
			place.append("set_variables(QSqlDatabase *ptr_database, const _sys_system_id id, const int section_id, Fpl_Dike_Var_Materialzone *zone, const int number)");
			reason="No material variable zone is found for the density of the impermeable layer";
			help="Check the random variable and the material variable zone";
			reaction="No connection can be established between the material variable zone and the random variable; the results can be wrong";
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
//Set error(s)
Error Fpl_Mech_Waterside_Erosion::set_error(const int err_type){
	string place="Fpl_Mech_Waterside_Erosion::";
	string help;
	string reason;
	int type=0;
	bool fatal=false;
	stringstream info;
	Error msg;

	switch (err_type){
		case 0://bad alloc
			place.append("allocate_bursting_slices(void)");
			reason="Can not allocate the memory";
			help="Check the memory";
			type=10;
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
