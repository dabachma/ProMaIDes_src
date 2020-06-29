//#include "Fpl_Mech_Waverunup_Dike.h"
#include "Fpl_Headers_Precompiled.h"

//Default constructor
Fpl_Mech_Wave_Runup::Fpl_Mech_Wave_Runup(void){
	//members
	this->no_segment_intercept=0;
	this->local_wave_direction=0.0;
	this->free_board=0.0;
	this->real_free_board=0.0;
	this->wave_steepness=0.0;
	this->breaker_factor=0.0;
	this->gamma_berms=1.0;
	this->waterside_cubature=NULL;

	this->height_runup.set_unit(label::m);
	this->discharge_overtopping.set_unit(label::qm_per_secm);

	Sys_Memory_Count::self()->add_mem(sizeof(Fpl_Mech_Wave_Runup)-4*sizeof(Fpl_Random_Variables)-sizeof(Geo_Straight_Line), _sys_system_modules::FPL_SYS);//count the memory
}
//Default destructor
Fpl_Mech_Wave_Runup::~Fpl_Mech_Wave_Runup(void){
	Sys_Memory_Count::self()->minus_mem(sizeof(Fpl_Mech_Wave_Runup)-4*sizeof(Fpl_Random_Variables)-sizeof(Geo_Straight_Line), _sys_system_modules::FPL_SYS);//count the memory
}
//__________
//public
//Set the pointer to the waterside cubature, which is allocated in the dike-section class
void Fpl_Mech_Wave_Runup::set_waterside_cubature(Fpl_Cub_Dike_Waterside *waterside_cubature){
	this->waterside_cubature=waterside_cubature;
}
//Set the random variables of the results for the wave runup mechanism per database table
void Fpl_Mech_Wave_Runup::set_result_variables(QSqlDatabase *ptr_database, const _sys_system_id id, const int section_id){
	try{
		this->discharge_overtopping.set_input(id, section_id, fpl_label::q_waveoverflow, true, ptr_database, fpl_label::mech_wave_runup, fpl_label::sec_dike, 0, false);
		this->discharge_overtopping.set_distribution_types(ptr_database);

		this->height_runup.set_input(id, section_id, fpl_label::height_runup, true, ptr_database, fpl_label::mech_wave_runup, fpl_label::sec_dike, 0, false);
		this->height_runup.set_distribution_types(ptr_database);
	}catch(Error msg){
		throw msg;
	}
}
//Set the random input variables per database table
void Fpl_Mech_Wave_Runup::set_variables(QSqlDatabase *ptr_database, const _sys_system_id id, const int section_id){
	try{
		this->gamma_wavedirection.set_input(id, section_id, fpl_label::gamma_wavedirection, true, ptr_database, fpl_label::mech_wave_runup, fpl_label::sec_dike, 0, false);
		this->gamma_wavedirection.set_distribution_types(ptr_database);
		this->mid_gradient_waterside.set_input(id, section_id, fpl_label::mid_gradient_waterside, true, ptr_database, fpl_label::mech_wave_runup, fpl_label::sec_dike, 0, false);
		this->mid_gradient_waterside.set_distribution_types(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}
}
//Set the random input variables as well as the geometrical data per file
void Fpl_Mech_Wave_Runup::set_input(QFile *ifile, int *line_counter, const qint64 pos_file, const int line_counter_start){
	ostringstream prefix;
	prefix << "RUNUP> ";
	Sys_Common_Output::output_fpl->set_userprefix(prefix.str());
	ostringstream cout;
	//random variables
	cout << "Input variable members per file..." << endl;
	Sys_Common_Output::output_fpl->output_txt(&cout);

	try{
		this->gamma_wavedirection.set_input(ifile, line_counter,fpl_label::gamma_wavedirection,true, fpl_label::end_mech_wave_runup);
		this->gamma_wavedirection.check_given_boundaries(1.0,0.0);
		ifile->seek(pos_file);
		*line_counter=line_counter_start;

		this->mid_gradient_waterside.set_input(ifile, line_counter,fpl_label::mid_gradient_waterside,true, fpl_label::end_mech_wave_runup);
		this->mid_gradient_waterside.check_positiv_zero_variable();
		ifile->seek(pos_file);
		*line_counter=line_counter_start;

		this->discharge_overtopping.set_input(ifile, line_counter,fpl_label::q_overflow,true, fpl_label::end_mech_wave_runup);
		this->discharge_overtopping.check_positiv_zero_variable();
		ifile->seek(pos_file);
		*line_counter=line_counter_start;

		this->height_runup.set_input(ifile, line_counter,fpl_label::height_runup,true, fpl_label::end_mech_wave_runup);
		this->height_runup.check_positiv_zero_variable();
		ifile->seek(pos_file);
		*line_counter=line_counter_start;

		this->waterside_cubature->set_gamma_berm(ifile, line_counter);
		ifile->seek(pos_file);
		*line_counter=line_counter_start;

	}
	catch(Error msg){
		throw msg;
	}

	cout << "Input of variable members per file is finsihed" << endl;
	Sys_Common_Output::output_fpl->output_txt(&cout);

	//geometry
	cout << "Input geometrical members per file..." << endl;
	Sys_Common_Output::output_fpl->output_txt(&cout);
	try{
	}
	catch(Error msg){
		throw msg;
	}

	cout << "Input of geometrical members per file is finsihed" << endl;
	Sys_Common_Output::output_fpl->output_txt(&cout);

	Sys_Common_Output::output_fpl->rewind_userprefix();
}
//Transfer the wave-runup generation data to database, e.g. geometry, random variables etc.
void Fpl_Mech_Wave_Runup::transfer_data2database(const int section_id, const _sys_system_id id, QSqlDatabase *ptr_database){
	ostringstream prefix;
	ostringstream cout;
	prefix << "RUNUP> ";
	Sys_Common_Output::output_fpl->set_userprefix(prefix.str());

	cout << "Transfer variable members to database..."<<endl;
	Sys_Common_Output::output_fpl->output_txt(&cout);
	this->mid_gradient_waterside.transfer_rand2database(ptr_database,id, section_id, fpl_label::mech_wave_runup, fpl_label::sec_dike);
	this->gamma_wavedirection.transfer_rand2database(ptr_database,id, section_id, fpl_label::mech_wave_runup, fpl_label::sec_dike);
	this->waterside_cubature->transfer_data2database(section_id,id, ptr_database);
	//result members
	cout << "Transfer result members to database..."<<endl;
	Sys_Common_Output::output_fpl->output_txt(&cout);
	this->height_runup.transfer_rand2database(ptr_database,id, section_id, fpl_label::mech_wave_runup, fpl_label::sec_dike);
	this->discharge_overtopping.transfer_rand2database(ptr_database,id, section_id, fpl_label::mech_wave_runup, fpl_label::sec_dike);

	Sys_Common_Output::output_fpl->rewind_userprefix();
}
//Output members to display console
void Fpl_Mech_Wave_Runup::output_members(void){
	ostringstream prefix;
	prefix << "RUNUP> ";
	Sys_Common_Output::output_fpl->set_userprefix(prefix.str());
	ostringstream cout;
	cout << "Random variable members of the mechanism wave run-up..." << endl;
	Sys_Common_Output::output_fpl->output_txt(&cout,true);
	this->mid_gradient_waterside.output_member();
	this->gamma_wavedirection.output_member();
	this->waterside_cubature->output_members();

	cout << "Random result members of the mechanism wave run-up..." << endl;
	Sys_Common_Output::output_fpl->output_txt(&cout,true);
	this->height_runup.output_member();
	this->discharge_overtopping.output_member();

	Sys_Common_Output::output_fpl->rewind_userprefix();
}
//Calculate the mechanism; main input parameter are the wave condition due to the wind machanism as well as the geometrical waterside cubature
_fpl_wave_runup_result Fpl_Mech_Wave_Runup::calculate_mechanism(const bool random_calculation, const double wind_direction, const double section_direction, double wave_height, double wave_period, const double wave_length, const double waterlevel){
	//set the other parameters
	_fpl_wave_runup_result result;
	result.wave_overtopping=0.0;
	result.wave_runup=0.0;
	this->gamma_berms=1.0;

	//set the wave height by small waterlevels
	if(waterlevel*0.5<wave_height){
		wave_height=waterlevel*0.5;
	}
	//reset the random flags for a new calculation
	this->reset_random_flag();

	//calculate the freeboard
	this->free_board=this->waterside_cubature->get_dike_height()-waterlevel;
	this->real_free_board=this->free_board;
	//check if a calculation is necessary
	if((this->free_board< 0.0 && abs(this->free_board)>1.5*wave_height) || wave_height <=0.0 || wave_period <=0.0){
		return result;
	}

	//calculate the angle of wave attack and the gamma factor and randomize it; have to be done in every case eventually the wave height is reduced
	this->gamma_wavedirection.set_new_middle(this->calculate_gamma_wavedirection(wind_direction, section_direction, &wave_height, &wave_period));
	this->gamma_wavedirection.calculate_variables(random_calculation);

	//calculate the berm factor and the reduction of the wave height in case of foreshore; have to be done in every case eventually the wave height is reduced
	if(this->waterside_cubature->get_with_berm_flag()==true){
		this->waterside_cubature->calculate_gammaberm_waveheightreduction(wave_length, &wave_height, waterlevel);
		this->gamma_berms=this->waterside_cubature->get_total_gamma_berm(random_calculation);
	}

	//no waves=> no runup or overtopping volume
	if(wave_height <=0.0 || wave_period <=0.0 ){
		return result;
	}
	//check again after the waveheight reduction if a calculation is necessary
	if(this->free_board< 0.0 && abs(this->free_board)>1.5*wave_height){
		return result;
	}

	//calculate the area of influence
	//make a line of the relevant waterlevel -1.5*wave_height
	this->make_line_relevant_waterlevel(waterlevel, wave_height);
	//calculate the relevant cubature (intersection with hw-1.5*hwave)
	this->waterside_cubature->calculate_intercept_cubature_firstup(&this->waterline_minus_hm, &this->no_segment_intercept);

	//calculate the gamma_material with the real relevant cubature
	double gamma_material=0.0;
	gamma_material=this->waterside_cubature->calculate_gamma_material(random_calculation, wave_height, this->no_segment_intercept);
	if(gamma_material<=0.0){
		gamma_material=1.0e-20;
	}

	if(this->free_board>0.0){//waterline is under the dike crest
		//calculate mid gradient with the real relevant cubature and give t to the random variables
		this->mid_gradient_waterside.set_new_middle(this->waterside_cubature->calculate_mid_relevant_gradient(this->no_segment_intercept));
		this->mid_gradient_waterside.calculate_variables(random_calculation);

		//calculate the breaker factor
		this->calculate_breaker_factor(wave_height, wave_period);

		//calculate the runup
		result.wave_runup=this->calculate_runup(wave_height, gamma_material);
		this->height_runup.set_new_middle(result.wave_runup);
		result.wave_runup=this->height_runup.calculate_variables(random_calculation).end_result;

		//calculate the volume
		if(result.wave_runup>0.0 && (result.wave_runup+waterlevel)>this->waterside_cubature->get_dike_height()){
			result.wave_overtopping=this->calculate_overtopping_discharge(wave_height, gamma_material);
			this->discharge_overtopping.set_new_middle(result.wave_overtopping);
			result.wave_overtopping=this->discharge_overtopping.calculate_variables(random_calculation).end_result;
		}
		else{//wave runup + waterlevel is smaller than the dike crest: no overtopping is possible
			result.wave_overtopping=0.0;
		}
	}
	else if(this->free_board<=0.0 && abs(this->free_board) <= 1.5*wave_height){//waterlevel is higher than the crest once the wave height
		//reset the freedoard to zero and use the same overtopping formulas as for freeboard > 0;
		this->free_board=0.0;

		//calculate mid gradient with the real relevant cubature and give t to the random variables
		double mid_gradient=0.0;
		mid_gradient=this->waterside_cubature->calculate_mid_relevant_gradient(this->no_segment_intercept);
		//interpolate for a smooth transition between zero wave run up abs(freebord)<1.5 wave_height and 0
		this->interpolate_mid_gradient_overcrest(&mid_gradient, wave_height);
		this->mid_gradient_waterside.set_new_middle(mid_gradient);
		this->mid_gradient_waterside.calculate_variables(random_calculation);

		//calculate the breaker factor
		this->calculate_breaker_factor(wave_height, wave_period);

		//calculation
		result.wave_overtopping=this->calculate_overtopping_discharge(wave_height, gamma_material);
		this->discharge_overtopping.set_new_middle(result.wave_overtopping);
		result.wave_overtopping=this->discharge_overtopping.calculate_variables(random_calculation).end_result;
	}
	else{//the waterlevel is higher more than once the wave height
		result.wave_overtopping=0.0;
	}
	return result;
}
//Check the statistic of the random variables
void Fpl_Mech_Wave_Runup::check_statistic(void){
	//variables
	this->mid_gradient_waterside.check_statistic();
	this->gamma_wavedirection.check_statistic();
	//geometrie
	this->waterside_cubature->check_statistic();
	//result variables
	this->height_runup.check_statistic();
	this->discharge_overtopping.check_statistic();
}
//Reset the statistic important values
void Fpl_Mech_Wave_Runup::reset_statistics(void){
	//variables
	this->mid_gradient_waterside.reset_statistics();
	this->gamma_wavedirection.reset_statistics();
	//geometrie
	this->waterside_cubature->reset_statistics();
	//result variables
	this->height_runup.reset_statistics();
	this->discharge_overtopping.reset_statistics();

	this->reset_counter_prob();
}
//Output the statistics of the random variables to display/console
void Fpl_Mech_Wave_Runup::output_statistic(void){
	ostringstream prefix;
	prefix << "RUNUP> ";
	Sys_Common_Output::output_fpl->set_userprefix(prefix.str());

	ostringstream cout;
	cout << "Random variable members of the mechanism wave run-up..." << endl;
	Sys_Common_Output::output_fpl->output_txt(&cout,true);
	cout.str("");
	this->mid_gradient_waterside.output_statistic();
	this->gamma_wavedirection.output_statistic();

	//out put the cubature variables
	this->waterside_cubature->output_statistic();

	cout << "Random result members of the mechanism wave run-up..." << endl;
	Sys_Common_Output::output_fpl->output_txt(&cout,true);
	this->height_runup.output_statistic();
	this->discharge_overtopping.output_statistic();

	Sys_Common_Output::output_fpl->rewind_userprefix();
}
//Output the deterministic results to display/console in case of an determistic calculation
void Fpl_Mech_Wave_Runup::output_determ_results(void){
	ostringstream prefix;
	prefix << "RUNUP> ";
	Sys_Common_Output::output_fpl->set_userprefix(prefix.str());
	ostringstream cout;
	cout << "Deterministic results of the mechanism wave run-up..." << endl;
	cout << " The waterside cubature is intercepted by waterlevel-1.5 x wave_height at segment: "<< this->no_segment_intercept << endl;
	cout << " Wave runup height      : "<< this->height_runup.get_result().end_result << label::m<<endl;
	cout << " Waveoverflow discharge : " << this->discharge_overtopping.get_result().end_result << label::qm_per_secm << endl;
	cout << " Mid relevant gradient  : " << this->mid_gradient_waterside.get_result().end_result << label::no_unit<<endl;
	cout << " Gamma_wavedirection    : " << this->gamma_wavedirection.get_result().end_result << label::no_unit<<endl;
	cout << " Gamma_berms            : " << this->gamma_berms << label::no_unit << endl;
	cout << " Freeboard              : " << this->real_free_board << label::m << endl;
	Sys_Common_Output::output_fpl->output_txt(&cout,false);
	this->waterside_cubature->output_determ_results();
	Sys_Common_Output::output_fpl->rewind_userprefix();
}
//Get a pointer to the mid breaker factor
double *Fpl_Mech_Wave_Runup::get_ptr_mid_breaker_fac(void){
	return &this->breaker_factor;
}
//_____________
//protected
//Reset the random generated flags of the random variables
void Fpl_Mech_Wave_Runup::reset_random_flag(void){
	//reset the random generated flags of the material zones
	this->waterside_cubature->reset_random_flag();
	this->discharge_overtopping.reset_random_generated();
	this->gamma_wavedirection.reset_random_generated();
	this->mid_gradient_waterside.reset_random_generated();
	this->height_runup.reset_random_generated();
}
//Calculate the angle of wave attack, from which the reduction factor for the wave direction is determined
double Fpl_Mech_Wave_Runup::calculate_gamma_wavedirection(const double wind_direction, const double section_direction, double *wave_height, double *wave_period){
	//gamma wave direction is accoording to TAW (Wave-Runup and wave overtopping at dikes 2002; Eurotop-Manual 2007)
	//calculate the angle of wave attack
	//0° is orthogonal to the section!
	
	
	this->local_wave_direction=abs(wind_direction-section_direction);

	//calculate the gamma factor
	double gamma=0.0;
	if(abs(this->local_wave_direction) <=80.0){//only reduction factor
		gamma=1.0-0.0022*abs(this->local_wave_direction);
	}
	else if(abs(this->local_wave_direction)>80.0 && abs(this->local_wave_direction)<=110.0){//reduction factor + reduction of the wave height
		gamma=1.0-0.0022*80.0;
		*wave_height=*wave_height*(110.0-abs(this->local_wave_direction))/30.0;
		*wave_period=*wave_period*pow(((110.0-abs(this->local_wave_direction))/30.0),0.5);
	}
	else{//wave height is zero
		*wave_height=0.0;
	}
	return gamma;
}
//Generate the line of the relevant waterlevel (waterlevel-1.5*hm)
void Fpl_Mech_Wave_Runup::make_line_relevant_waterlevel(const double waterlevel, const double wave_height){
	double height=waterlevel-1.5*wave_height;
	if(height<0.0){
		height=0.0;
	}
	this->waterline_minus_hm.set_coordinates(0.0,height);
}
//Calculate the breaker factor and the wave steepness
void Fpl_Mech_Wave_Runup::calculate_breaker_factor(const double wave_height, const double wave_period){
	//steepness and breaker parameter
	this->wave_steepness=2.0*constant::Cpi*wave_height/(constant::Cgg*pow(wave_period,2));
	this->breaker_factor=this->mid_gradient_waterside.get_result().end_result/pow(wave_steepness,0.5);
}
//Calculate the wave runup height
double Fpl_Mech_Wave_Runup::calculate_runup(const double wave_height, const double roughness_gamma){
	//calculations accoording to TAW (Wave-Runup and wave overtopping at dikes 2002; Eurotop-Manual 2007)
	double runup=0.0;
	double max_runup=0.0;

	//calculate the runup
	max_runup=wave_height*roughness_gamma* this->gamma_wavedirection.get_result().end_result*(4.0-1.5/pow(this->breaker_factor,0.5));
	runup=1.65*wave_height*this->gamma_berms*roughness_gamma*this->gamma_wavedirection.get_result().end_result*this->breaker_factor;
	//check for the maximum value
	if(max_runup < runup){
		runup=max_runup;
	}
	return runup;
}
//Calculate the overtopping discharge
double Fpl_Mech_Wave_Runup::calculate_overtopping_discharge(const double wave_height, const double roughness_gamma){
/**
The formula calculates the overtopping discharge [m³/s per m].
*/
	double discharge=0.0;
	double max_discharge=0.0;

	double factor_1=constant::Cgg*pow(wave_height,3);
	factor_1=pow(factor_1,0.5);

	double exponent=0.0;
	if(this->breaker_factor <=5.0){
		//maximum (gamma_wavedirection_overtopp=1.5-gamma_wavedirection_runup-0.5)
		exponent=(-2.6*this->free_board)/(wave_height*(1.5*this->gamma_wavedirection.get_result().end_result-0.5)*roughness_gamma);
		max_discharge=factor_1*0.2*exp(exponent);
		//discharge
		exponent=(-4.75*this->free_board/(wave_height*(1.5*this->gamma_wavedirection.get_result().end_result-0.5)*roughness_gamma*this->gamma_berms*this->breaker_factor));
		discharge=factor_1*0.067*this->gamma_berms*this->breaker_factor*exp(exponent)/pow(this->mid_gradient_waterside.get_result().end_result,0.5);
		//check the maximum
		if(max_discharge< discharge){
			discharge=max_discharge;
		}
	}
	else if(this->breaker_factor >= 7.0){
		exponent=-this->free_board/(roughness_gamma*(1.5*this->gamma_wavedirection.get_result().end_result-0.5)*wave_height*(0.33+0.022*this->breaker_factor));
		discharge=factor_1*pow(10,-0.92)*exp(exponent);
	}
	//linear interpolation
	else if(this->breaker_factor>5.0 && this->breaker_factor< 7.0){//averaging the discharges for the breaker factors
		double discharge1=0.0;
		double discharge2=0.0;
		//discharge 1
		//maximum
		exponent=(-2.6*this->free_board)/(wave_height*(1.5*this->gamma_wavedirection.get_result().end_result-0.5)*roughness_gamma);
		max_discharge=0.2*exp(exponent);
		//discharge
		exponent=(-4.75*this->free_board/(wave_height*(1.5*this->gamma_wavedirection.get_result().end_result-0.5)*roughness_gamma*this->gamma_berms*this->breaker_factor));
		discharge1=0.067*this->gamma_berms*this->breaker_factor*exp(exponent)/pow(this->mid_gradient_waterside.get_result().end_result,0.5);
		//check the maximum
		if(max_discharge< discharge1){
			discharge1=max_discharge;
		}
		//discharge 2
		exponent=-this->free_board/(roughness_gamma*(1.5*this->gamma_wavedirection.get_result().end_result-0.5)*wave_height*(0.33+0.022*this->breaker_factor));
		discharge2=pow(10,-0.92)*exp(exponent);
		//interpolate with the logs between breaker paramter 5 und 7
		if(discharge2<=0.0 && discharge1>0.0){
			discharge2=0.00000000000001;
		}
		else if(discharge2<=0.0 && discharge1<=0.0){
			discharge=0.0;
			return discharge;
		}
		else if(discharge2>0.0 && discharge1<=0.0){
			discharge1=0.00000000000001;
		}
		double m=(discharge2-discharge1)/(7.0-5.0);
		double b=discharge1-m*5.0;
		discharge=m*this->breaker_factor+b;
		discharge=factor_1*discharge;
	}

	return discharge;
}
//Interpolate the mid gradient in case of freeboard < 0 (waterlevel over crest) for a smooth transition
void Fpl_Mech_Wave_Runup::interpolate_mid_gradient_overcrest(double *mid_gradient, const double wave_height){
	double quotient=abs(this->free_board)/(1.5*wave_height);
	*mid_gradient=(1.0-quotient)*(*mid_gradient);
}