#include "Fpl_Headers_Precompiled.h"
//#include "Fpl_Mech_Waverunup_Dune.h"


//Default constructor
Fpl_Mech_Waverunup_Dune::Fpl_Mech_Waverunup_Dune(void){



	Sys_Memory_Count::self()->add_mem(sizeof(Fpl_Mech_Waverunup_Dune)-sizeof(Fpl_Mech_Wave_Runup), _sys_system_modules::FPL_SYS);//count the memory


}
//Default destructor
Fpl_Mech_Waverunup_Dune::~Fpl_Mech_Waverunup_Dune(void){

	Sys_Memory_Count::self()->minus_mem(sizeof(Fpl_Mech_Waverunup_Dune)-sizeof(Fpl_Mech_Wave_Runup), _sys_system_modules::FPL_SYS);//count the memory

}
//__________
//public
//Set the random variables of the results for the wave runup mechanism per database table
void Fpl_Mech_Waverunup_Dune::set_result_variables(QSqlDatabase *ptr_database, const _sys_system_id id, const int section_id){
	try{
		this->discharge_overtopping.set_input(id, section_id, fpl_label::q_waveoverflow, true, ptr_database, fpl_label::mech_wave_runup, fpl_label::sec_dune, 0, false);
		this->discharge_overtopping.set_distribution_types(ptr_database);

		this->height_runup.set_input(id, section_id, fpl_label::height_runup, true, ptr_database, fpl_label::mech_wave_runup, fpl_label::sec_dune, 0, false);
		this->height_runup.set_distribution_types(ptr_database);
	}catch(Error msg){
		throw msg;
	}

}
//Set the random input variables per database table
void Fpl_Mech_Waverunup_Dune::set_variables(QSqlDatabase *ptr_database, const _sys_system_id id, const int section_id){
	try{
		this->gamma_wavedirection.set_input(id, section_id, fpl_label::gamma_wavedirection, true, ptr_database, fpl_label::mech_wave_runup, fpl_label::sec_dune, 0, false);
		this->gamma_wavedirection.set_distribution_types(ptr_database);
		this->mid_gradient_waterside.set_input(id, section_id, fpl_label::mid_gradient_waterside, true, ptr_database, fpl_label::mech_wave_runup, fpl_label::sec_dune, 0, false);
		this->mid_gradient_waterside.set_distribution_types(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}
}
//Transfer the wave-runup generation data to database, e.g. geometry, random variables etc.
void Fpl_Mech_Waverunup_Dune::transfer_data2database(const int section_id, const _sys_system_id id, QSqlDatabase *ptr_database){
	ostringstream prefix;
	ostringstream cout;
	prefix << "RUNUP> ";
	Sys_Common_Output::output_fpl->set_userprefix(prefix.str());

	cout << "Transfer variable members to database..."<<endl;
	Sys_Common_Output::output_fpl->output_txt(&cout);
	this->mid_gradient_waterside.transfer_rand2database(ptr_database,id, section_id, fpl_label::mech_wave_runup, fpl_label::sec_dune);
	this->gamma_wavedirection.transfer_rand2database(ptr_database,id, section_id, fpl_label::mech_wave_runup, fpl_label::sec_dune);
	this->waterside_cubature->transfer_data2database(section_id,id, ptr_database);
	//result members
	cout << "Transfer result members to database..."<<endl;
	Sys_Common_Output::output_fpl->output_txt(&cout);
	this->height_runup.transfer_rand2database(ptr_database,id, section_id, fpl_label::mech_wave_runup, fpl_label::sec_dune);
	this->discharge_overtopping.transfer_rand2database(ptr_database,id, section_id, fpl_label::mech_wave_runup, fpl_label::sec_dune);	

	Sys_Common_Output::output_fpl->rewind_userprefix();
}
//___________
//private
//Calculate the overtopping discharge
double Fpl_Mech_Waverunup_Dune::calculate_overtopping_discharge(const double wave_height, const double roughness_gamma){
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
		exponent=(-2.69*this->free_board/(wave_height*(1.5*this->gamma_wavedirection.get_result().end_result-0.5)*roughness_gamma*this->gamma_berms*this->breaker_factor));
		discharge=factor_1*0.0073*this->gamma_berms*this->breaker_factor*exp(exponent)/pow(this->mid_gradient_waterside.get_result().end_result,0.5);
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