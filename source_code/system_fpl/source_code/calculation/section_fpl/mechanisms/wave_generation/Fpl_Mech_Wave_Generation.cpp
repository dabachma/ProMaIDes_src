//#include "Fpl_Mech_Wave_Generation.h"
#include "Fpl_Headers_Precompiled.h"

//Default constructor
Fpl_Mech_Wave_Generation::Fpl_Mech_Wave_Generation(void){
	this->section_direction=0.0;
	this->anticlockwise_water_bank=0.0;
	this->clockwise_water_bank=0.0;
	this->determ_result.wind_direction=0.0;
	this->determ_result.mean_waterdepth=0.0;
	this->determ_result.wave_height=0.0;
	this->determ_result.wave_length=0.0;
	this->determ_result.wave_period=0.0;
	this->determ_result.wave_setup=0.0;
	this->user_defined=false;


	this->ptr_foreland=NULL;

	this->corrected_w_dir=NULL;
	this->number_corr_w_dir=-1;

	this->wind_speed_direction.set_unit(label::wind_dir_speed);

	Sys_Memory_Count::self()->add_mem(sizeof(Fpl_Mech_Wave_Generation)-sizeof(Fpl_Waveparam_Calculation)-sizeof(Fpl_Wind_Fetch)-sizeof(Fpl_Random_Variables), _sys_system_modules::FPL_SYS);//count the memory
}
//Constructor, where the section direction is given
Fpl_Mech_Wave_Generation::Fpl_Mech_Wave_Generation(const double direction){
	this->section_direction=direction;
	//calculate the banks
	this->anticlockwise_water_bank=_Geo_Geometrie::angle_to_360(direction-90.0);
	this->clockwise_water_bank=_Geo_Geometrie::angle_to_360(direction+90.0);
	//as standard constrictor
	this->determ_result.mean_waterdepth=0.0;
	this->determ_result.wave_height=0.0;
	this->determ_result.wave_length=0.0;
	this->determ_result.wave_period=0.0;
	this->determ_result.wave_setup=0.0;

	this->delete_corr_w_dir();

	Sys_Memory_Count::self()->add_mem(sizeof(Fpl_Mech_Wave_Generation)-sizeof(Fpl_Waveparam_Calculation)-sizeof(Fpl_Wind_Fetch)-sizeof(Fpl_Random_Variables), _sys_system_modules::FPL_SYS);//count the memory
}
//Default destructor
Fpl_Mech_Wave_Generation::~Fpl_Mech_Wave_Generation(void){

	Sys_Memory_Count::self()->minus_mem(sizeof(Fpl_Mech_Wave_Generation)-sizeof(Fpl_Waveparam_Calculation)-sizeof(Fpl_Wind_Fetch)-sizeof(Fpl_Random_Variables), _sys_system_modules::FPL_SYS);//count the memory
}
//________________________
//public
//Set the fetches (the geometric parameters) via database table 
void Fpl_Mech_Wave_Generation::new_set_fetch(QSqlDatabase *ptr_database, const _sys_system_id id, const int section_id){
	if(this->user_defined==true){
		return;
	}
	//set the values
	try{
		//for the fetches (length, angle, zone number)
		this->my_fetch.new_set_global_fetches(ptr_database, id, this->section_direction, section_id);
		this->my_fetch.new_set_bathymetry(ptr_database, id, section_id);
		this->my_fetch.connect_fetches2bathy();
	}
	catch(Error msg){
		throw msg;
	}
}
//Set the random variable of the wind-speed-direction via database table
void Fpl_Mech_Wave_Generation::set_windspeed_direction(QSqlDatabase *ptr_database, const _sys_system_id id, const int section_id){
	
	bool found_flag=false;

	try{
		QSqlQueryModel results;
		int no=0;
		no=Fpl_Random_Variables::select_variables_database(&results, id, section_id, fpl_label::wind_speed_direction, ptr_database, fpl_label::mech_wind_wave, fpl_label::sec_all, 0);
		
		if(no>0){
			this->user_defined=false;
			this->wind_speed_direction.set_unit(label::wind_dir_speed);
			this->wind_speed_direction.set_input(id,section_id, fpl_label::wind_speed_direction, false, ptr_database, fpl_label::mech_wind_wave, fpl_label::sec_all, 0, false);
			this->wind_speed_direction.set_distribution_types(ptr_database);
			found_flag=true;
		}
		else{
			no=Fpl_Random_Variables::select_variables_database(&results, id, section_id, fpl_label::wave_direction, ptr_database, fpl_label::mech_wind_wave, fpl_label::sec_all, 0);
			if(no>0){
				this->user_defined=true;
				this->wind_speed_direction.set_unit(label::degree);
				this->wind_speed_direction.set_input(id,section_id, fpl_label::wave_direction, false, ptr_database, fpl_label::mech_wind_wave, fpl_label::sec_all, 0, false);
				this->wind_speed_direction.set_distribution_types(ptr_database);
				found_flag=true;
			}
		}

	}catch(Error msg){
		throw msg;
	}

	if(found_flag==false){
		//set error
		Error msg=this->set_error(5);
		throw msg;

	}
	if(this->user_defined==false){
		if(this->wind_speed_direction.get_distribution_type()!=_fpl_distribution_type::discrete2mean_type){
			Error msg=this->set_error(1);
			throw msg;
		}

		if(this->my_fetch.get_fetch_number()==0){
			Error msg=this->set_error(2);
			throw msg;
		}

	}
	else{
		if(this->wind_speed_direction.get_distribution_type()!=_fpl_distribution_type::discret_type){
			Error msg=this->set_error(4);
			throw msg;
		}

	}
}
//Set the random variables for the results via database table
void Fpl_Mech_Wave_Generation::set_result_variables(QSqlDatabase *ptr_database, const _sys_system_id id, const int section_id){
	try{
		this->wave_calculator.set_result_variables(ptr_database, id, section_id, this->user_defined);
	}catch(Error msg){
		throw msg;
	}
}
//Set the random input variables per database table
void Fpl_Mech_Wave_Generation::set_variables(QSqlDatabase *ptr_database, const _sys_system_id id, const int section_id){
	try{
		if(this->user_defined==true){
			this->wave_calculator.set_variables(ptr_database, id, section_id);
		}
	}catch(Error msg){
		throw msg;
	}
}
//Set the random input variables as well as the geometrical data per file
void Fpl_Mech_Wave_Generation::set_input(QFile *ifile, int *line_counter, const qint64 pos_file, const int line_counter_start){
	ostringstream prefix;
	prefix << "WIND> ";
	Sys_Common_Output::output_fpl->set_userprefix(prefix.str());
	ostringstream cout;
	//random variables
	cout << "Input variable members per file..." << endl;
	Sys_Common_Output::output_fpl->output_txt(&cout);

	try{
		bool found_flag=false;
		try{
			
			//input of wind_speed direction or wave_direction
			found_flag=this->wind_speed_direction.check_variable_in_file(ifile, line_counter,fpl_label::wind_speed_direction, fpl_label::end_mech_wind_wave);
			ifile->seek(pos_file);
			*line_counter=line_counter_start;

			if(found_flag==true){
				this->user_defined=false;
				this->wind_speed_direction.set_unit(label::wind_dir_speed);
				this->wind_speed_direction.set_input(ifile, line_counter,fpl_label::wind_speed_direction,false, fpl_label::end_mech_wind_wave);
				ifile->seek(pos_file);
				*line_counter=line_counter_start;
			}
			else{
				found_flag=this->wind_speed_direction.check_variable_in_file(ifile, line_counter,fpl_label::wave_direction, fpl_label::end_mech_wind_wave);
				ifile->seek(pos_file);
				*line_counter=line_counter_start;
				if(found_flag==true){
					this->user_defined=true;
					this->wind_speed_direction.set_unit(label::degree);
					this->wind_speed_direction.set_input(ifile, line_counter,fpl_label::wave_direction,false, fpl_label::end_mech_wind_wave);
					ifile->seek(pos_file);
					*line_counter=line_counter_start;
				}

			}
		}catch(Error msg){
			throw msg;
		}

		if(found_flag==false){
			//set error
			Error msg=this->set_error(5);
			throw msg;

		}

		//check them
		if(this->user_defined==false){
			if(this->wind_speed_direction.get_distribution_type()!=_fpl_distribution_type::discrete2mean_type){
				Error msg=this->set_error(3);
				throw msg;
			}

		}
		else{
			if(this->wind_speed_direction.get_distribution_type()!=_fpl_distribution_type::discret_type){
				Error msg=this->set_error(6);
				throw msg;
			}
		}

		
		//next variables
		this->wave_calculator.set_variables(ifile, line_counter, pos_file, line_counter_start, this->user_defined);
		ifile->seek(pos_file);
		*line_counter=line_counter_start;

	}
	catch(Error msg){
		Sys_Common_Output::output_fpl->rewind_userprefix();
		throw msg;
	}

	if(this->user_defined==false){
		cout << "Input of variable members per file is finished" << endl;
		Sys_Common_Output::output_fpl->output_txt(&cout);
		
		//geometry
		cout << "Input geometrical members per file..." << endl;
		Sys_Common_Output::output_fpl->output_txt(&cout);
		try{
			this->my_fetch.new_set_global_fetches(ifile, line_counter, this->section_direction);
			this->my_fetch.new_set_bathymetry(ifile, line_counter);

		}
		catch(Error msg){
			Sys_Common_Output::output_fpl->rewind_userprefix();
			throw msg;
		}
		//connect the fetches
		this->my_fetch.connect_fetches2bathy();

		cout << "Input of geometrical members per file is finished" << endl;
		Sys_Common_Output::output_fpl->output_txt(&cout);
	}

	Sys_Common_Output::output_fpl->rewind_userprefix();
}
//Set the section direction
void Fpl_Mech_Wave_Generation::set_section_direction(const double direction){
	this->section_direction=direction;
	//calculate the banks
	this->anticlockwise_water_bank=_Geo_Geometrie::angle_to_360(direction-90.0);
	this->clockwise_water_bank=_Geo_Geometrie::angle_to_360(direction+90.0);
}
//Transfer the wind-wave generation data to database, e.g. geometry, random variables etc.
void Fpl_Mech_Wave_Generation::transfer_data2database(const int section_id, const _sys_system_id id, QSqlDatabase *ptr_database){
	ostringstream prefix;
	ostringstream cout;
	prefix << "WIND> ";
	Sys_Common_Output::output_fpl->set_userprefix(prefix.str());

	if(this->user_defined==false){
		cout << "Transfer geometrical fetches..."<<endl;
		Sys_Common_Output::output_fpl->output_txt(&cout);
		this->my_fetch.transfer_data2database(section_id,id, ptr_database);
	}

	cout << "Transfer variable members to database..."<<endl;
	Sys_Common_Output::output_fpl->output_txt(&cout);
	this->wind_speed_direction.transfer_rand2database(ptr_database,id, section_id, fpl_label::mech_wind_wave, fpl_label::sec_all);
	//result members
	cout << "Transfer result members to database..."<<endl;
	Sys_Common_Output::output_fpl->output_txt(&cout);
	this->wave_calculator.transfer_data2database(section_id,id, ptr_database);
	
	Sys_Common_Output::output_fpl->rewind_userprefix();
}
//Output members to display/console
void Fpl_Mech_Wave_Generation::output_members(void){
	ostringstream prefix;
	prefix << "WIND> ";
	Sys_Common_Output::output_fpl->set_userprefix(prefix.str());
	ostringstream cout;
	cout << "User defined wave parameters : " << functions::convert_boolean2string(this->user_defined)<<endl;
	cout << "Geometry members of the mechanism wind induced wave-generation..." << endl;
	Sys_Common_Output::output_fpl->output_txt(&cout,true);
	cout << " Anticlockwise water bank direction : " << this->anticlockwise_water_bank<< label::degree<<endl;
	cout << " Clockwise water bank direction     : " << this->clockwise_water_bank<< label::degree<<endl;
	if(this->user_defined==false){
		cout << " Global fetch members..." << endl;
		Sys_Common_Output::output_fpl->output_txt(&cout,true);
		this->my_fetch.output_members();
	}
	
	cout << "Variable members of the mechanism wind induced wave-generation..." << endl;
	Sys_Common_Output::output_fpl->output_txt(&cout,true);
	this->wind_speed_direction.output_member();	

	//of the wave generation calculator
	cout << "Random result members of the mechanism wind induced wave-generation..." << endl;
	Sys_Common_Output::output_fpl->output_txt(&cout,true);
	this->wave_calculator.output_member();

	Sys_Common_Output::output_fpl->rewind_userprefix();
}
//Calculate the relevant wave parameters caused by a given wind speed and direction
_fpl_waveparam_result Fpl_Mech_Wave_Generation::calculate_wind_wave_generation(const bool random_calculation, const double waterlevel){
	
	//init 
	this->determ_result.wind_direction=0.0;
	this->determ_result.mean_waterdepth=0.0;
	this->determ_result.wave_height=0.0;
	this->determ_result.wave_length=0.0;
	this->determ_result.wave_period=0.0;
	this->determ_result.wave_setup=0.0;
	this->determ_result.wave_height_original=0.0;
	//direction and speed
	_fpl_result wind;

	//reset the random flags
	this->reset_random_flag();

	//make the calculation
	wind=this->wind_speed_direction.calculate_variables(random_calculation);
	//no wave generation
	if(wind.end_result<=0.0 && this->user_defined==false){
		return this->determ_result;
	}

	//set the wind direction
	this->determ_result.wind_direction=wind.inter_result;


	if(this->user_defined==true){
		this->calculate_user_defined(random_calculation, waterlevel);
	}
	else{
        //check why this function is used daniel 27.11.2015
        this->determ_result.wind_direction=this->check_wind_direction(random_calculation, this->determ_result.wind_direction);

		this->calculate_krylowII(random_calculation, waterlevel, &wind);
	}
	
	return this->determ_result;
}
//Check the statistic of the random variables
void Fpl_Mech_Wave_Generation::check_statistic(void){
	this->wind_speed_direction.check_statistic();
	if(this->user_defined==false){
		this->my_fetch.check_statistic();
	}
	this->wave_calculator.check_statistic();
}
//Reset the statistic important values
void Fpl_Mech_Wave_Generation::reset_statistics(void){
	this->wind_speed_direction.reset_statistics();
	if(this->user_defined==false){
		this->my_fetch.reset_statistics();
	}
	this->wave_calculator.reset_statistics();
	this->reset_counter_prob();
}
//Output the statistics of the random variables to display/console
void Fpl_Mech_Wave_Generation::output_statistic(void){
	ostringstream cout;
	ostringstream prefix;
	prefix << "WIND> ";
	Sys_Common_Output::output_fpl->set_userprefix(prefix.str());

	if(this->user_defined==false){
		cout << "Global fetch members of the mechanism wind induced wave-generation..." << endl;
		Sys_Common_Output::output_fpl->output_txt(&cout,true);
		this->my_fetch.output_statistic();
		cout << "Generated local fetches of the mechanism wind induced wave-generation..."<<endl;
		Sys_Common_Output::output_fpl->output_txt(&cout,true);
		this->wave_calculator.output_found_localfetches();
	}

	cout << "Variable members of the mechanism wind induced wave-generation..." << endl;
	Sys_Common_Output::output_fpl->output_txt(&cout,true);
	this->wind_speed_direction.output_statistic();

	cout << "Random result members of the mechanism wind induced wave-generation..." << endl;
	Sys_Common_Output::output_fpl->output_txt(&cout,true);
	this->wave_calculator.output_statistic();

	Sys_Common_Output::output_fpl->rewind_userprefix();
} 
//Output the deterministic results to display/console in case of an determistic calculation
void Fpl_Mech_Wave_Generation::output_determ_results(void){
	ostringstream prefix;
	prefix << "WIND> ";
	Sys_Common_Output::output_fpl->set_userprefix(prefix.str());
	ostringstream cout;
	cout << "Deterministic results of the mechanism wind induced wave-generation..."<< endl;
	if(this->user_defined==false){
		cout << " Generated Windspeed : "<< this->wind_speed_direction.get_result().end_result << label::m_per_sec<<endl;
	}
	cout << " Winddirection       : " << this->wind_speed_direction.get_result().inter_result << label::degree << endl;
	cout << " Waveheight          : " << this->determ_result.wave_height_original << label::m<<endl;
	cout << " Waveperiod          : " << this->determ_result.wave_period <<label::sec<<endl;
	cout << " Wavelength          : " << this->determ_result.wave_length << label::m<<endl;
	cout << " Mean waterdepth     : " << this->determ_result.mean_waterdepth << label::m<< endl;
	cout << " Static wavesetup    : " << this->determ_result.wave_setup << label::m << endl;
	if(this->user_defined==true){
		cout << " m-correlation factor: " <<this->wave_calculator.m_corr_fac.get_result().end_result<<label::no_unit<<endl;
		cout << " b-correlation factor: " <<this->wave_calculator.b_corr_fac.get_result().end_result<<label::no_unit<<endl;
		cout << " e-correlation factor: " <<this->wave_calculator.e_corr_fac.get_result().end_result<<label::no_unit<<endl;
		cout << " Goda wave height    : " <<this->wave_calculator.goda_fac.get_result().end_result<<label::m<<endl;
	}
	Sys_Common_Output::output_fpl->output_txt(&cout,false);
	if(this->user_defined==false){
		cout << "Generated local fetches of the mechanism wind induced wave-generation..."<<endl;
		Sys_Common_Output::output_fpl->output_txt(&cout,false);
		this->wave_calculator.output_found_localfetches();
	}

	Sys_Common_Output::output_fpl->rewind_userprefix();
}
//Set the pointer to the foreland
void Fpl_Mech_Wave_Generation::set_ptr_foreland(Fpl_Dike_Geo_Foreland *foreland){
	this->ptr_foreland=foreland;
}
//____________________
//private
//Calculate user difined wave parameter
_fpl_waveparam_result Fpl_Mech_Wave_Generation::calculate_user_defined(const bool random_calculation, const double waterlevel){
	double buffer=this->determ_result.wind_direction;
	this->determ_result=this->wave_calculator.calculate_user_defined_params(random_calculation, waterlevel, this->ptr_foreland);
	this->determ_result.wind_direction=buffer;
	return this->determ_result; 
}
//Calculate the mechanism; here the method after KrylowII is applied 
_fpl_waveparam_result Fpl_Mech_Wave_Generation::calculate_krylowII(const bool random_calculation, const double waterlevel, _fpl_result *wind){
/**
The KrylowII calculation is based on the DVWK 246/1997. If required implement other 
algorithm for the wave generation, e.g. Brettschneider formula or the SPM-approach.
*/
	
	double buffer=this->determ_result.wind_direction;
	//the local fetch is allocated if it is not yet done
	this->determ_result=this->wave_calculator.calculate_krylow_params(random_calculation, wind->inter_result, this->section_direction, wind->end_result, &this->my_fetch, waterlevel);
	this->determ_result.wind_direction=buffer;
	return this->determ_result; 

}
//Allocate the corrected wind/wave direction
void Fpl_Mech_Wave_Generation::alloc_corr_w_dir(void){
	try{
		this->corrected_w_dir=new double[this->number_corr_w_dir];
	}
	catch (bad_alloc &t){
		Error msg=set_error(8);
		ostringstream info;
		info<< "Info bad alloc: " << t.what() << endl;
		msg.make_second_info(info.str());	
		throw msg;
	}
}
//Delete the corrected wind/wave direction
void Fpl_Mech_Wave_Generation::delete_corr_w_dir(void){
	if(this->corrected_w_dir!=NULL){
		delete []this->corrected_w_dir;
		this->corrected_w_dir=NULL;
	}
}
//Check the wind direction
double Fpl_Mech_Wave_Generation::check_wind_direction(const bool random, const double direction){

	double dir_buff=direction;
	if(random==false){
		if(dir_buff>180.0){
			dir_buff=cos(Geo_Point::grad_to_rad(dir_buff));
			dir_buff=Geo_Point::rad_to_grad(acos(dir_buff));
		}

	}
	else{
		if(this->number_corr_w_dir==-1){
			this->number_corr_w_dir=this->wind_speed_direction.distribution->get_number_point();
			if(this->number_corr_w_dir>0){
				this->alloc_corr_w_dir();
				//set the pointer
				for(int i=0; i<this->number_corr_w_dir; i++){
					this->corrected_w_dir[i]=this->wind_speed_direction.distribution->get_point_value(i);
					this->corrected_w_dir[i]=cos(Geo_Point::grad_to_rad(this->corrected_w_dir[i]));
					this->corrected_w_dir[i]=Geo_Point::rad_to_grad(acos(this->corrected_w_dir[i]));

				}

			}
		}
		
		if(this->number_corr_w_dir==0){
			if(dir_buff>180.0){
				dir_buff=cos(Geo_Point::grad_to_rad(dir_buff));
				dir_buff=Geo_Point::rad_to_grad(acos(dir_buff));
			}
		}
		else{
			dir_buff=this->corrected_w_dir[this->wind_speed_direction.distribution->get_result_index()];

		}

	}



	return dir_buff;
}
//Reset the random generated flags of the random variables
void Fpl_Mech_Wave_Generation::reset_random_flag(void){
	//reset the random generated flags 
	this->wind_speed_direction.reset_random_generated();
	if(this->user_defined==false){
		//reset the random generated variables of the globals fetch zone
		this->my_fetch.reset_random_flag();
	}
	this->wave_calculator.reset_random_flag();
}
//Set error(s)
Error Fpl_Mech_Wave_Generation::set_error(const int err_type){
		string place="Fpl_Mech_Wave_Generation::";
		string help;
		string reason;
		int type=0;
		bool fatal=false;
		stringstream info;
		Error msg;
	switch (err_type){
		case 0://bad_alloc
			place.append("new_set_fetch(QSqlDatabase *ptr_database, const _section_id id)");
			reason="Can not allocate the memory";
			help="Check the memory";
			type=10;
			break;
		case 1://wind_speed_direction is not distribution class discret_mean
			place.append("set_windspeed_direction(QSqlDatabase *ptr_database, const _sys_system_id id, const int section_id)");
			reason="The random variable of the wind_speed_direction is not of distribution class discret2mean ";
			help="Check the random variable of the wind_speed_direction and their distribution class in database";
			type=1;
			break;
		case 2://no fetches are found
			place.append("set_windspeed_direction(QSqlDatabase *ptr_database, const _sys_system_id id, const int section_id)");
			reason="There are no fetches found, which are required because the wave parameter are calculated";
			help="Check the fetch-settings in database";
			type=1;
			break;
		case 3://wind_speed_direction is not distribution class discret_mean
			place.append("set_input(QFile *ifile, int *line_counter, const qint64 pos_file, const int line_counter_start)");
			reason="The random variable of the wind_speed_direction is not of distribution discret2mean ";
			help="Check the random variable of the wind_speed_direction and their distribution class in file";
			type=1;
			break;
		case 4://wave_direction is not distribution class discret
			place.append("set_windspeed_direction(QSqlDatabase *ptr_database, const _sys_system_id id, const int section_id)");
			reason="The random variable of the wave_direction is not of distribution class discret";
			help="Check the random variable of the wave_direction and their distribution class in database";
			type=1;
			break;
		case 5://wind_speed_direction/wave direction is not database
			place.append("set_windspeed_direction(QSqlDatabase *ptr_database, const _sys_system_id id, const int section_id)");
			reason="The random variable of the wave_direction or wave_direction is not found in database";
			help="Check the random variable of the wave_direction or wave_direction in database";
			type=1;
			break;
		case 6://wave_direction is not distribution class discret
			place.append("set_input(QFile *ifile, int *line_counter, const qint64 pos_file, const int line_counter_start)");
			reason="The random variable of the wave_direction is not of distribution class discret";
			help="Check the random variable of the wave_direction and their distribution class in file";
			type=1;
			break;
		case 7://wind_speed_direction/wave direction is not in file
			place.append("set_input(QFile *ifile, int *line_counter, const qint64 pos_file, const int line_counter_start)");
			reason="The random variable of the wave_direction or wave_direction is not found in file";
			help="Check the random variable of the wave_direction or wave_direction in file";
			type=1;
			break;
		case 8://bad alloc
			place.append("alloc_corr_w_dir(void)");
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
