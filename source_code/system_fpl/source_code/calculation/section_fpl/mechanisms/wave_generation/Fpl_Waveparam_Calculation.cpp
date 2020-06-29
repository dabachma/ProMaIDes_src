//#include "Fpl_Waveparam_Calculation.h"
#include "Fpl_Headers_Precompiled.h"

//Default constructor
Fpl_Waveparam_Calculation::Fpl_Waveparam_Calculation(void){
	this->number_local_direction=0;
	this->local_fetches=NULL;

	this->wave_height.set_unit(label::m);
	this->wave_lenght.set_unit(label::m);
	this->wave_period.set_unit(label::sec);
	this->mean_depth.set_unit(label::m);
	this->wave_setup.set_unit(label::m);

	this->b_corr_fac.set_unit(label::sec);
	this->m_corr_fac.set_unit(label::sec_per_m);
	this->e_corr_fac.set_unit(label::no_unit);
	this->goda_fac.set_unit(label::m);

	this->mean_depth_foreland=0.0;
	this->mean_slope_foreland=0.0;

	this->user_defined=false;

	Sys_Memory_Count::self()->add_mem(sizeof(Fpl_Waveparam_Calculation)-sizeof(Fpl_Random_Variables)*8, _sys_system_modules::FPL_SYS);//count the memory
}
//Default destructor
Fpl_Waveparam_Calculation::~Fpl_Waveparam_Calculation(void){
	if(this->local_fetches!=NULL){
		delete []this->local_fetches;
		this->local_fetches=NULL;
	}
	Sys_Memory_Count::self()->minus_mem(sizeof(Fpl_Waveparam_Calculation)-sizeof(Fpl_Random_Variables)*8, _sys_system_modules::FPL_SYS);//count the memory
}
//___________
//public
//Set the random variables for the results via database table
void Fpl_Waveparam_Calculation::set_result_variables(QSqlDatabase *ptr_database, const _sys_system_id id, const int section_id, const bool user_defined){
	try{
		this->user_defined=user_defined;
		this->wave_height.set_input(id, section_id, fpl_label::wave_height, !user_defined, ptr_database, fpl_label::mech_wind_wave, fpl_label::sec_all, 0, false);
		this->wave_height.set_distribution_types(ptr_database);
		this->wave_lenght.set_input(id, section_id, fpl_label::wave_length, true, ptr_database, fpl_label::mech_wind_wave, fpl_label::sec_all, 0, false);
		this->wave_lenght.set_distribution_types(ptr_database);
		this->wave_period.set_input(id, section_id, fpl_label::wave_period, true, ptr_database, fpl_label::mech_wind_wave, fpl_label::sec_all, 0, false);
		this->wave_period.set_distribution_types(ptr_database);
		this->mean_depth.set_input(id, section_id, fpl_label::mean_depth, true, ptr_database, fpl_label::mech_wind_wave, fpl_label::sec_all, 0, false);
		this->mean_depth.set_distribution_types(ptr_database);
		this->wave_setup.set_input(id, section_id, fpl_label::wave_setup, !user_defined, ptr_database, fpl_label::mech_wind_wave, fpl_label::sec_all, 0, false);
		this->wave_setup.set_distribution_types(ptr_database);
	}catch(Error msg){
		throw msg;
	}
}
//Set the random input variables per database table
void Fpl_Waveparam_Calculation::set_variables(QSqlDatabase *ptr_database, const _sys_system_id id, const int section_id){
	this->b_corr_fac.set_input(id, section_id, fpl_label::b_corr_factor, false, ptr_database, fpl_label::mech_wind_wave, fpl_label::sec_all, 0, false);
	this->b_corr_fac.set_distribution_types(ptr_database);

	this->m_corr_fac.set_input(id, section_id, fpl_label::m_corr_factor, false, ptr_database, fpl_label::mech_wind_wave, fpl_label::sec_all, 0, false);
	this->m_corr_fac.set_distribution_types(ptr_database);

	this->e_corr_fac.set_input(id, section_id, fpl_label::e_corr_factor, false, ptr_database, fpl_label::mech_wind_wave, fpl_label::sec_all, 0, false);
	this->e_corr_fac.set_distribution_types(ptr_database);

	this->goda_fac.set_input(id, section_id, fpl_label::goda_fac, true, ptr_database, fpl_label::mech_wind_wave, fpl_label::sec_all, 0, false);
	this->goda_fac.set_distribution_types(ptr_database);


}
//Set the random input variables per file
void Fpl_Waveparam_Calculation::set_variables(QFile *ifile, int *line_counter, const qint64 pos_file, const int line_counter_start, const bool user_defined){
	try{
		//result
		this->user_defined=user_defined;
		this->wave_height.set_input(ifile, line_counter,fpl_label::wave_height,!user_defined, fpl_label::end_mech_wind_wave);
		this->wave_height.check_positiv_zero_variable();
		ifile->seek(pos_file);
		*line_counter=line_counter_start;

		this->wave_lenght.set_input(ifile, line_counter,fpl_label::wave_length,true, fpl_label::end_mech_wind_wave);
		this->wave_lenght.check_positiv_zero_variable();
		ifile->seek(pos_file);
		*line_counter=line_counter_start;

		this->wave_period.set_input(ifile, line_counter,fpl_label::wave_period,true, fpl_label::end_mech_wind_wave);
		this->wave_period.check_positiv_zero_variable();
		ifile->seek(pos_file);
		*line_counter=line_counter_start;

		this->mean_depth.set_input(ifile, line_counter,fpl_label::mean_depth,true, fpl_label::end_mech_wind_wave);
		this->mean_depth.check_positiv_zero_variable();
		ifile->seek(pos_file);
		*line_counter=line_counter_start;

		this->wave_setup.set_input(ifile, line_counter,fpl_label::wave_setup,!user_defined, fpl_label::end_mech_wind_wave);
		this->wave_setup.check_positiv_zero_variable();
		ifile->seek(pos_file);
		*line_counter=line_counter_start;

		if(user_defined==true){
			this->b_corr_fac.set_input(ifile, line_counter,fpl_label::b_corr_factor,false, fpl_label::end_mech_wind_wave);
			ifile->seek(pos_file);
			*line_counter=line_counter_start;

			this->m_corr_fac.set_input(ifile, line_counter,fpl_label::m_corr_factor,false, fpl_label::end_mech_wind_wave);
			ifile->seek(pos_file);
			*line_counter=line_counter_start;

			this->e_corr_fac.set_input(ifile, line_counter,fpl_label::e_corr_factor,false, fpl_label::end_mech_wind_wave);
			ifile->seek(pos_file);
			*line_counter=line_counter_start;

			this->goda_fac.set_input(ifile, line_counter,fpl_label::goda_fac,true, fpl_label::end_mech_wind_wave);
			this->goda_fac.check_positiv_zero_variable();
			ifile->seek(pos_file);
			*line_counter=line_counter_start;

		}
	}
	catch(Error msg){
		Sys_Common_Output::output_fpl->rewind_userprefix();
		throw msg;
	}
}
//Calculate the required wave parameter with user defined values
_fpl_waveparam_result Fpl_Waveparam_Calculation::calculate_user_defined_params(const bool random_calculation, const double waterlevel, Fpl_Dike_Geo_Foreland *foreland){
	_fpl_waveparam_result wave_results_determ;
	//set the wave results
	wave_results_determ.wave_height=0.0;
	wave_results_determ.wave_length=0.0;
	wave_results_determ.wave_period=0.0;
	wave_results_determ.mean_waterdepth=0.0;
	wave_results_determ.wave_setup=0.0;
	wave_results_determ.wind_direction=0.0;
	wave_results_determ.wave_height_original=0.0;

	this->reset_random_flag();

	//calculate the wave height
	wave_results_determ.wave_height=this->wave_height.calculate_variables(random_calculation).end_result;
	wave_results_determ.wave_height_original=wave_results_determ.wave_height;
	//calculate the wave setup
	wave_results_determ.wave_setup=this->wave_setup.calculate_variables(random_calculation).end_result;


	//calculate the wave periode (use correlation)
	this->b_corr_fac.calculate_variables(random_calculation);
	this->e_corr_fac.calculate_variables(random_calculation);
	this->m_corr_fac.calculate_variables(random_calculation);

	wave_results_determ.wave_period=this->m_corr_fac.get_result().end_result*pow(this->wave_height.get_result().end_result,this->e_corr_fac.get_result().end_result)+this->b_corr_fac.get_result().end_result;
	this->wave_period.set_new_middle(wave_results_determ.wave_period);
	wave_results_determ.wave_period=this->wave_period.calculate_variables(random_calculation).end_result;


	//calculate the wave length
	wave_results_determ.wave_length=this->iterate_wavelength(wave_results_determ.wave_period, 1000.0);
	this->wave_lenght.set_new_middle(wave_results_determ.wave_length);
	wave_results_determ.wave_length=this->wave_lenght.calculate_variables(random_calculation).end_result;


	//calculate the mean depth
	double buff=0.0;
	this->calc_mean_foreland_depth(foreland, wave_results_determ.wave_length); 
	buff=waterlevel-this->mean_depth_foreland;
	this->mean_depth.set_new_middle(buff);
	wave_results_determ.mean_waterdepth=this->mean_depth.calculate_variables(random_calculation).end_result;


	//check the wave height after Goda 2000 (Random seas and desigtn of maritime structures 2nd edition ISBN 981-02-3256-x)
	if(wave_results_determ.wave_length>0.0){
		if(wave_results_determ.mean_waterdepth/wave_results_determ.wave_length>=0.2){
			double ks=0.0;
			buff=constant::Cpi*wave_results_determ.mean_waterdepth/wave_results_determ.wave_length;
			ks=1.0/pow(((1.0+4.0*buff/sinh(4.0*buff))*tanh(2.0*buff)),0.5);


			buff=ks*wave_results_determ.wave_height;
		}
		else{
			double ks=0.0;
			double beta_zero=0.0;
			double beta_one=0.0;
			double beta_max=0.0;

			beta_zero=0.028*pow((wave_results_determ.wave_height/wave_results_determ.wave_length),-0.38)*exp(20.0*pow(mean_slope_foreland,1.5));
			beta_one=0.52*exp(4.2*this->mean_slope_foreland);
			beta_max=max(0.92, 0.32*pow((wave_results_determ.wave_height/wave_results_determ.wave_length),-0.29)*exp(2.4*mean_slope_foreland));
			
			buff=constant::Cpi*wave_results_determ.mean_waterdepth/wave_results_determ.wave_length;
			ks=1.0/pow(((1.0+4.0*buff/sinh(4.0*buff))*tanh(2.0*buff)),0.5);

			buff=min((beta_zero*wave_results_determ.wave_height+beta_one*wave_results_determ.mean_waterdepth), beta_max*wave_results_determ.wave_height);
			buff=min(buff, ks*wave_results_determ.wave_height);

			

		}
		this->goda_fac.set_new_middle(buff);
		buff=this->goda_fac.calculate_variables(random_calculation).end_result;

		//check it
		wave_results_determ.wave_height=buff;
		

		//calculate new wave length
		wave_results_determ.wave_length=this->iterate_wavelength(wave_results_determ.wave_period, wave_results_determ.mean_waterdepth);


	}


	return wave_results_determ;
}
//Calculate the required wave parameter after the KrylowII algorithm
_fpl_waveparam_result Fpl_Waveparam_Calculation::calculate_krylow_params(const bool random_calculation, const double wind_direction,  const double section_direction, const double windspeed, Fpl_Wind_Fetch *global_fetches, const double waterlevel){
	_fpl_waveparam_result wave_results_determ;
	//set the wave results
	wave_results_determ.wave_height=0.0;
	wave_results_determ.wave_length=0.0;
	wave_results_determ.wave_period=0.0;
	wave_results_determ.mean_waterdepth=0.0;
	wave_results_determ.wave_setup=0.0;
	wave_results_determ.wave_height_original=0.0;
	wave_results_determ.wind_direction=wind_direction;

	this->reset_random_flag();

	//is the index of the local fetches; id is the wind direction
	int index=0;

	//inter results for the wave param calculation
	double hw_i=0.0;
	double deep_i=0.0;;
	double fetch_length_i=0.0;
	double a_i=0.0;
	double set_up=0.0;

	//check wind direction

	//search for the local fetches for this wind direction; if it not there it will be allocated
	try{
		index=this->find_local_fetches(wind_direction, section_direction, global_fetches);
	}
	catch(Error msg){
		throw msg;
	}
	//calculate the wave height after krylowII
	for(int i=0; i<this->local_fetches[index].give_number_fetches(); i++){//loop over the generated local fetches for this winddirection
		if(this->local_fetches[index].fetch_param[i].dry_flag==true){//local fetch is on the land (dry)
			hw_i=0.0;
			a_i=0.0;
			deep_i=0.0;
			set_up=0.0;
		}
		else{
			//waterdepth mean per fetch
			int global_fetchzone_index=this->local_fetches[index].fetch_param[i].index_variable;
			//total deep per fetch
			deep_i=waterlevel-global_fetches->calculate_batymetrie(random_calculation, global_fetchzone_index);

			if(deep_i<=0.0){//batymethrie is over the waterlevel (dry)
				hw_i=0.0;
				a_i=0.0;
				deep_i=0.0;
			}
			else{
				//the set up after zuidersee-formula (DVWK 246/1997)
				set_up=pow(windspeed,2.0)*this->local_fetches[index].fetch_param[i].ortho_length/(4861110.0*deep_i);
				//sum up the water detph; just in this case a waterdetph per fetch zon is calculated
				wave_results_determ.mean_waterdepth=wave_results_determ.mean_waterdepth+deep_i;
				//norm orthogonal fetchlength per fetch
				fetch_length_i=constant::Cgg*this->local_fetches[index].fetch_param[i].ortho_length/pow(windspeed,2.0);
				//norm deep fetch
				deep_i=constant::Cgg*deep_i/pow(windspeed,2.0);
				//wave height per fetch
				hw_i=pow(windspeed,2.0)*0.16/constant::Cgg;
				hw_i=hw_i*(1.0-1.0/(pow((1.0+0.006*pow(fetch_length_i,0.5)),2.0)));
				hw_i=hw_i*tanh(0.625*pow(deep_i,0.8)/(1.0-1.0/(pow((1+0.006*pow(fetch_length_i,0.5)),2.0))));
				//the angle factor
				a_i=this->local_fetches[index].fetch_param[i].a_angle;
			}
		}
		//sum up the mean wave height
		wave_results_determ.wave_height=a_i*pow(hw_i,2.0)+wave_results_determ.wave_height;
		//sum up the setup
		wave_results_determ.wave_setup=a_i*pow(set_up,2.0)+wave_results_determ.wave_setup;

		//check if wave set-up is higher than wave-height
		if(wave_results_determ.wave_setup>wave_results_determ.wave_height){
			//set maximum boundary of the wave setup by the wave height; important for small water depth
			wave_results_determ.wave_setup=wave_results_determ.wave_height;
		}
	}//end of local fetches loop

	//take the square root wave height
	wave_results_determ.wave_height=pow(wave_results_determ.wave_height,0.5);
	//take the square root set-up
	wave_results_determ.wave_setup=pow(wave_results_determ.wave_setup,0.5);

	//calculated random the results
	//set new middle for the random result
	this->wave_height.set_new_middle(wave_results_determ.wave_height);
	//randomize it
	wave_results_determ.wave_height=this->wave_height.calculate_variables(random_calculation).end_result;
	wave_results_determ.wave_height_original=wave_results_determ.wave_height;
	//the setup
	this->wave_setup.set_new_middle(wave_results_determ.wave_setup);
	wave_results_determ.wave_setup=this->wave_setup.calculate_variables(random_calculation).end_result;

	//calculate the mean depth
	if(this->local_fetches[index].get_number_relevant_fetches()!=0){
		wave_results_determ.mean_waterdepth=wave_results_determ.mean_waterdepth/this->local_fetches[index].get_number_relevant_fetches();
	}
	else{
		wave_results_determ.mean_waterdepth=0.0;
	}
	this->mean_depth.set_new_middle(wave_results_determ.mean_waterdepth);
	wave_results_determ.mean_waterdepth=this->mean_depth.calculate_variables(random_calculation).end_result;

	//calculate the wave periode
	wave_results_determ.wave_period=6.2*windspeed*constant::Cpi*pow(constant::Cgg*wave_results_determ.wave_height/pow(windspeed,2.0),0.625)/constant::Cgg;
	this->wave_period.set_new_middle(wave_results_determ.wave_period);
	wave_results_determ.wave_period=this->wave_period.calculate_variables(random_calculation).end_result;

	//calculate the wave length
	wave_results_determ.wave_length=this->iterate_wavelength(wave_results_determ.wave_period, wave_results_determ.mean_waterdepth);
	this->wave_lenght.set_new_middle(wave_results_determ.wave_length);
	wave_results_determ.wave_length=this->wave_lenght.calculate_variables(random_calculation).end_result;

	return wave_results_determ;
}
//Transfer the wind-wave result data (KrylowII) to database, e.g. geometry, random variables etc.
void Fpl_Waveparam_Calculation::transfer_data2database(const int section_id, const _sys_system_id id, QSqlDatabase *ptr_database){
	this->wave_height.transfer_rand2database(ptr_database,id, section_id, fpl_label::mech_wind_wave, fpl_label::sec_all);
	this->wave_lenght.transfer_rand2database(ptr_database,id, section_id, fpl_label::mech_wind_wave, fpl_label::sec_all);
	this->wave_period.transfer_rand2database(ptr_database,id, section_id, fpl_label::mech_wind_wave, fpl_label::sec_all);
	this->mean_depth.transfer_rand2database(ptr_database,id, section_id, fpl_label::mech_wind_wave, fpl_label::sec_all);
	this->wave_setup.transfer_rand2database(ptr_database,id, section_id, fpl_label::mech_wind_wave, fpl_label::sec_all);
	if(this->user_defined==true){
		this->b_corr_fac.transfer_rand2database(ptr_database,id, section_id, fpl_label::mech_wind_wave, fpl_label::sec_all);
		this->m_corr_fac.transfer_rand2database(ptr_database,id, section_id, fpl_label::mech_wind_wave, fpl_label::sec_all);
		this->e_corr_fac.transfer_rand2database(ptr_database,id, section_id, fpl_label::mech_wind_wave, fpl_label::sec_all);
		this->goda_fac.transfer_rand2database(ptr_database,id, section_id, fpl_label::mech_wind_wave, fpl_label::sec_all);
	}
}
//Output members to display/console
void Fpl_Waveparam_Calculation::output_member(void){
	ostringstream cout;
	cout << " Parameter of the wave generation..." << endl;
	Sys_Common_Output::output_fpl->output_txt(&cout,true);
	this->wave_height.output_member();
	this->wave_lenght.output_member();
	this->wave_period.output_member();
	this->mean_depth.output_member();
	this->wave_setup.output_member();
	if(this->user_defined==true){
		this->b_corr_fac.output_member();
		this->m_corr_fac.output_member();
		this->e_corr_fac.output_member();
		this->goda_fac.output_member();
	}
}
//Check the statistic of the random variables
void Fpl_Waveparam_Calculation::check_statistic(void){
	//of the results
	this->wave_height.check_statistic();
	this->wave_lenght.check_statistic();
	this->wave_period.check_statistic();
	this->mean_depth.check_statistic();
	this->wave_setup.check_statistic();
	if(this->user_defined==true){
		this->b_corr_fac.check_statistic();
		this->m_corr_fac.check_statistic();
		this->e_corr_fac.check_statistic();
		this->goda_fac.check_statistic();
	}
}
//Reset the statistic important values
void Fpl_Waveparam_Calculation::reset_statistics(void){
	this->wave_height.reset_statistics();
	this->wave_lenght.reset_statistics();
	this->wave_period.reset_statistics();
	this->mean_depth.reset_statistics();
	this->wave_setup.reset_statistics();
	if(this->user_defined==true){
		this->b_corr_fac.reset_statistics();
		this->m_corr_fac.reset_statistics();
		this->e_corr_fac.reset_statistics();
		this->goda_fac.reset_statistics();
	}
}
//Output the statistics of the random variables to display/console
void Fpl_Waveparam_Calculation::output_statistic(void){
	ostringstream cout;
	cout << " Wave parameter..." << endl;
	Sys_Common_Output::output_fpl->output_txt(&cout,true);
	this->wave_height.output_statistic();
	this->wave_period.output_statistic();
	this->mean_depth.output_statistic();
	this->wave_lenght.output_statistic();
	this->wave_setup.output_statistic();
	if(this->user_defined==true){
		this->b_corr_fac.output_statistic();
		this->m_corr_fac.output_statistic();
		this->e_corr_fac.output_statistic();
		this->goda_fac.output_statistic();
	}
}
//Output the found local fetches (part of the given fetches), which are used for the simulation run
void Fpl_Waveparam_Calculation::output_found_localfetches(void){
	ostringstream cout;
	cout<<" Local fetches found " << this->number_local_direction <<"..."<< endl;
	Sys_Common_Output::output_fpl->output_txt(&cout,true);
	for(int i=0; i< this->number_local_direction; i++){
		this->local_fetches[i].output_members();
	}
}
//Reset the random generated flags of the random variables
void Fpl_Waveparam_Calculation::reset_random_flag(void){
	this->wave_height.reset_random_generated();
	this->wave_lenght.reset_random_generated();
	this->wave_period.reset_random_generated();
	this->mean_depth.reset_random_generated();
	this->wave_setup.reset_random_generated();
	if(this->user_defined==true){
		this->b_corr_fac.reset_random_generated();
		this->m_corr_fac.reset_random_generated();
		this->e_corr_fac.reset_random_generated();
		this->goda_fac.reset_random_generated();
	}
}
//______________________
//private
//Search for an already allocated local fetch set to the corresponding winddirection and return the index
int Fpl_Waveparam_Calculation::find_local_fetches(const double wind_direction, const double section_direction, Fpl_Wind_Fetch *global_fetches){
	int index=0;
	//check over all loacl direction if the direction is already calculated
	for(int i=0;i<this->number_local_direction;i++){
		if(abs(wind_direction-this->local_fetches[i].give_wind_direction())<=constant::angle_epsilon){
			//set the index which is found
			index=i;
			return i;
		}
	}
	//if it is not yet calculated renew it
	//allocate plus 1
	try{
		this->renew_local_fetches();
		//set the new one
		this->local_fetches[this->number_local_direction-1].set_fetches(wind_direction, section_direction, global_fetches);
	}
	catch(Error msg){
		throw msg;
	}
	//set the index and the flag
	index=this->number_local_direction-1;
	return index;
}
//Generate a new local fetch set, if no local fetches for the corresponding winddirection is found
void Fpl_Waveparam_Calculation::renew_local_fetches(void){
	//store the already allocated values in a buffer
	Fpl_Local_Wind_Fetch *buffer=NULL;

	try{
		buffer=new Fpl_Local_Wind_Fetch[this->number_local_direction];
	}
	catch(bad_alloc&){
		Error msg=this->set_error(0);
		throw msg;
	}

	//copy it
	for(int i=0;i<this->number_local_direction;i++){
		buffer[i]=this->local_fetches[i];
	}
	//delete the local fetches
	if(this->local_fetches!=NULL){
		delete []this->local_fetches;
		this->local_fetches=NULL;
	}

	//allocate new
	this->number_local_direction++;
	try{
		this->local_fetches=new Fpl_Local_Wind_Fetch[this->number_local_direction];
	}
	catch(bad_alloc&){
		Error msg=this->set_error(0);
		throw msg;
	}

	//recopy
	for(int i=0;i<this->number_local_direction-1;i++){
		this->local_fetches[i]=buffer[i];
	}
	//delete the buffer
	if(buffer!=NULL){
		delete []buffer;
		buffer=NULL;
	}
}
//Calculate the wave length iterativly (implicit formula)
double Fpl_Waveparam_Calculation::iterate_wavelength(const double wave_period, const double mean_waterdepth){
	//coefficient
	double a_coeff=0.0;//=g*Twe^2/(2*pi)
	double b_coeff=0.0;//(2*pi*d_mean)
	a_coeff=constant::Cgg*pow(wave_period,2.0)/(2.0*constant::Cpi);
	b_coeff=2.0*constant::Cpi*mean_waterdepth;
	//iteration values
	double start_value=a_coeff;
	int max_iteration=100;
	double rel_criterion=0.01;
	double abs_criterion=0.01;
	//the calculated values
	double x=start_value;
	double x_new=0.0;
	double y=0.0;
	double y_diff=0.0;
	bool success_flag=false;

	//not if it is zero
	if(a_coeff==0.0){
		return 0.0;
	}

	//loop of iteration (Newton/rapson)
	for(int i=0; i<=max_iteration; i++){
		y=a_coeff*tanh(b_coeff/x)-x;
		y_diff=a_coeff*(-b_coeff/pow(x,2.0))/pow(cosh(b_coeff/x),2.0)-1.0;
		x_new=x-y/y_diff;

		//abort criterion
		if(abs(x_new-x)<abs_criterion && abs((x_new-x)/x)<rel_criterion){
			success_flag=true;
			break;
		}
		//copy new to old
		x=x_new;
	}//end iteration loop
	//set warning if it is not reach
	if(success_flag==false){
		Warning msg=this->set_warning(0);
		ostringstream info;
		info << "Maximum iteration " << max_iteration << " Relative criterion " << rel_criterion << " Absolute criterion " << abs_criterion << endl;
		info << "Calculated and used value " << x << endl;
		msg.make_second_info(info.str());
		msg.output_msg(1);
	}
	return x;
}
//Calculate the mean depth in the foreland and the mean foreland slope
void Fpl_Waveparam_Calculation::calc_mean_foreland_depth(Fpl_Dike_Geo_Foreland *foreland, const double wave_length){
	
	

	if(foreland==NULL || foreland->get_number_segments()==0){
		this->mean_depth_foreland=-20.0;
		this->mean_slope_foreland=0.1;
	}
	else{
		
		double buffer=0.0;
		double buff_length=-2.0*wave_length;
		double sum_grad=0.0;



		for(int i=foreland->get_number_segments()-1; i>=0 ; i--){
			if(foreland->get_segment(i)->point1.get_xcoordinate()>=buff_length){
				buffer=buffer+(foreland->get_segment(i)->point1.get_ycoordinate()+foreland->get_segment(i)->point2.get_ycoordinate())*0.5*foreland->get_segment(i)->get_horizontal_dist();
				sum_grad=sum_grad+foreland->get_segment(i)->get_gradient()*foreland->get_segment(i)->get_horizontal_dist();

			}
			else if(foreland->get_segment(i)->point1.get_xcoordinate()<buff_length && foreland->get_segment(i)->point2.get_xcoordinate()>buff_length){

				double d_x=abs(foreland->get_segment(i)->point2.get_xcoordinate()-buff_length);
				double d_xges=foreland->get_segment(i)->point2.get_xcoordinate()-foreland->get_segment(i)->point1.get_xcoordinate();
				double y=foreland->get_segment(i)->point2.get_ycoordinate()+((foreland->get_segment(i)->point1.get_ycoordinate()-foreland->get_segment(i)->point2.get_ycoordinate())/d_xges)*d_x;
				sum_grad=sum_grad+foreland->get_segment(i)->get_gradient()*d_x;
				buffer=buffer+(y+foreland->get_segment(i)->point2.get_ycoordinate())*0.5*d_x;

				
			}
		}

		
		this->mean_depth_foreland=buffer/(abs(buff_length));
		this->mean_slope_foreland=sum_grad/(abs(buff_length));
		
	}

}
//Set error(s)
Error Fpl_Waveparam_Calculation::set_error(const int err_type){
		string place="Fpl_Waveparam_Calculation::";
		string help;
		string reason;
		int type=0;
		bool fatal=false;
		stringstream info;
		Error msg;

	switch (err_type){
		case 0://bad_alloc
			place.append("renew_local_fetches(void)");
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
//Set warning(s)
Warning Fpl_Waveparam_Calculation::set_warning(const int warn_type){
		string place="Fpl_Waveparam_Calculation::";
		string help;
		string reason;
		string reaction;
		int type=0;
		Warning msg;
		stringstream info;

	switch (warn_type){
		case 0://no unique result found
			place.append("iterate_wavelength(void)") ;
			reason="Abort criterion is not reached by Newton Iteration";
			reaction="Calculated wave length is used";
			help= "Check abort criterion in code and the numbers of iteration";
			type=5;
			msg.set_during_calculation_flag(false);//just output to the exception output
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