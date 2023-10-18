#include "Fpl_Headers_Precompiled.h"
//#include "Fpl_Mech_Piping_Sell_Dike.h"

//Default constructor
Fpl_Mech_Piping_Sell_Dike::Fpl_Mech_Piping_Sell_Dike(void){
	this->buff_stress=0.0;
	this->buff_res=0.0;
	this->failure=0;
	this->buff_c=0.0;
	this->buff_alpha=0.0;
	this->buff_k=0.0;
	this->permeable_zone=NULL;

	this->size_perm_layer.set_unit(label::m);
	this->d_70.set_unit(label::m);
	this->dens_perm_layer.set_unit(label::kg_per_qm);
	this->k_f_perm_layer.set_unit(label::m_per_sec);
	this->rolling_resistance_angle.set_unit(label::degree);
	this->resistance.set_unit(label::m);

	Sys_Memory_Count::self()->add_mem(sizeof(Fpl_Mech_Piping_Sell_Dike)-sizeof(Fpl_Random_Variables)*7,_sys_system_modules::FPL_SYS);//count the memory
}
//Default destructor
Fpl_Mech_Piping_Sell_Dike::~Fpl_Mech_Piping_Sell_Dike(void){

	Sys_Memory_Count::self()->minus_mem(sizeof(Fpl_Mech_Piping_Sell_Dike)-sizeof(Fpl_Random_Variables)*7,_sys_system_modules::FPL_SYS);//count the memory
}
//_________
//public
//Set the random variables of the result members
void Fpl_Mech_Piping_Sell_Dike::set_result_variables(QSqlDatabase *ptr_database, const _sys_system_id id, const int section_id){
	try{
		this->resistance.set_input(id, section_id,fpl_label::res_piping_sell, true, ptr_database, fpl_label::mech_piping_sell, fpl_label::sec_dike, 0, false);
		this->resistance.set_distribution_types(ptr_database);

	}catch(Error msg){
		throw msg;
	}
}
//Set the random input variables per database table
void Fpl_Mech_Piping_Sell_Dike::set_variables(QSqlDatabase *ptr_database, const _sys_system_id id, const int section_id, Fpl_Dike_Var_Materialzone *zone){
	try{
		this->size_perm_layer.set_input(id, section_id,fpl_label::size_perm_lay, false, ptr_database, fpl_label::mech_piping_sell, fpl_label::sec_dike, 0, false);
		this->size_perm_layer.set_distribution_types(ptr_database);


		if(zone==NULL){
			try{
				this->dens_perm_layer.set_input(id, section_id,fpl_label::dens_perm_lay, false, ptr_database, fpl_label::mech_piping_sell, fpl_label::sec_dike, -1, false);
				this->d_70.set_input(id, section_id,fpl_label::d_70, false, ptr_database, fpl_label::mech_piping_sell, fpl_label::sec_dike, 0, false);
				this->dens_perm_layer.set_distribution_types(ptr_database);
				this->d_70.set_distribution_types(ptr_database);
			}
			catch(Error msg){
				ostringstream info;
				info<<" No permeable zone is found!"<<endl;
				msg.make_second_info(info.str());
				throw msg;
			}
		}
		else{
			
			this->permeable_zone=zone;
			ostringstream cout;
			cout <<"The permeable layer is connected to material variable zone "<<zone->get_zone_id()  << endl ;
			Sys_Common_Output::output_fpl->output_txt(&cout,false);

		}

		this->k_f_perm_layer.set_input(id, section_id,fpl_label::kf_perm_lay, false, ptr_database, fpl_label::mech_piping_sell, fpl_label::sec_dike, 0, false);
		this->k_f_perm_layer.set_distribution_types(ptr_database);
		this->drag_force_factor.set_input(id, section_id,fpl_label::dragforce_fac, false, ptr_database, fpl_label::mech_piping_sell, fpl_label::sec_dike, 0, false);
		this->drag_force_factor.set_distribution_types(ptr_database);
		this->rolling_resistance_angle.set_input(id, section_id,fpl_label::roll_res_angle, false, ptr_database, fpl_label::mech_piping_sell, fpl_label::sec_dike, 0, false);
		this->rolling_resistance_angle.set_distribution_types(ptr_database);
				

	}catch(Error msg){
		throw msg;
	}

}
//Set the random input variables per file
void Fpl_Mech_Piping_Sell_Dike::set_variables(QFile *ifile, int *line_counter, const qint64 pos_file, const int line_counter_start, Fpl_Dike_Var_Materialzone *zone){
	ostringstream prefix;
	prefix << "PIP_SELL> ";
	Sys_Common_Output::output_fpl->set_userprefix(prefix.str());
	ostringstream cout;
	cout << "Input variable members per file..." << endl;
	Sys_Common_Output::output_fpl->output_txt(&cout);
	
	try{
		this->size_perm_layer.set_input(ifile, line_counter,fpl_label::size_perm_lay,false, fpl_label::end_mech_pip_sell);
		this->size_perm_layer.check_positiv_variable();
		ifile->seek(pos_file);
		*line_counter=line_counter_start;

		if(zone==NULL){
			try{
				this->dens_perm_layer.set_input(ifile, line_counter,fpl_label::dens_perm_lay,false, fpl_label::end_mech_pip_sell);
				this->dens_perm_layer.check_given_boundaries(5000.0,800.0);
				ifile->seek(pos_file);
				*line_counter=line_counter_start;
				

				this->d_70.set_input(ifile, line_counter,fpl_label::d_70,false, fpl_label::end_mech_pip_sell);
				this->d_70.check_given_boundaries(1.0,1e-20);
				ifile->seek(pos_file);
				*line_counter=line_counter_start;
			}
			catch(Error msg){
				ostringstream info;
				info<<" No permeable zone is found!"<<endl;
				msg.make_second_info(info.str());
				throw msg;
			}
		}

		this->k_f_perm_layer.set_input(ifile, line_counter,fpl_label::kf_perm_lay,false, fpl_label::end_mech_pip_sell);
		this->k_f_perm_layer.check_given_boundaries(1.0,1e-20);
		ifile->seek(pos_file);
		*line_counter=line_counter_start;

		this->drag_force_factor.set_input(ifile, line_counter,fpl_label::dragforce_fac,false, fpl_label::end_mech_pip_sell);
		this->drag_force_factor.check_given_boundaries(1.0,0.01);
		ifile->seek(pos_file);
		*line_counter=line_counter_start;

		this->rolling_resistance_angle.set_input(ifile, line_counter,fpl_label::roll_res_angle,false, fpl_label::end_mech_pip_sell);
		this->rolling_resistance_angle.check_given_boundaries(70.0,20.0);
		ifile->seek(pos_file);
		*line_counter=line_counter_start;



		//result
		this->resistance.set_input(ifile, line_counter,fpl_label::res_piping_sell,true, fpl_label::end_mech_pip_sell);
		this->resistance.check_positiv_zero_variable();
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
//Transfer the piping-mechanism (Sellmeijer) data to database, e.g. geometry, random variables etc.
void Fpl_Mech_Piping_Sell_Dike::transfer_data2database(const int section_id, const _sys_system_id id, QSqlDatabase *ptr_database, Fpl_Dike_Var_Materialzone *zone){
	ostringstream prefix;
	ostringstream cout;
	prefix << "PIP_SELL> ";
	Sys_Common_Output::output_fpl->set_userprefix(prefix.str());

	cout << "Transfer variable members to database..."<<endl;
	Sys_Common_Output::output_fpl->output_txt(&cout);
	this->size_perm_layer.transfer_rand2database(ptr_database,id, section_id, fpl_label::mech_piping_sell, fpl_label::sec_dike);
	if(zone==NULL){
		this->dens_perm_layer.transfer_rand2database(ptr_database,id, section_id, fpl_label::mech_piping_sell, fpl_label::sec_dike);
		this->d_70.transfer_rand2database(ptr_database,id, section_id, fpl_label::mech_piping_sell, fpl_label::sec_dike);
	}
	this->rolling_resistance_angle.transfer_rand2database(ptr_database,id, section_id, fpl_label::mech_piping_sell, fpl_label::sec_dike);
	this->drag_force_factor.transfer_rand2database(ptr_database,id, section_id, fpl_label::mech_piping_sell, fpl_label::sec_dike);
	this->k_f_perm_layer.transfer_rand2database(ptr_database,id, section_id, fpl_label::mech_piping_sell, fpl_label::sec_dike);
	//result members
	cout << "Transfer result members to database..."<<endl;
	Sys_Common_Output::output_fpl->output_txt(&cout);
	this->resistance.transfer_rand2database(ptr_database,id, section_id, fpl_label::mech_piping_sell, fpl_label::sec_dike);
	
	Sys_Common_Output::output_fpl->rewind_userprefix();
}
//Output members of the mechanism to display/console
void Fpl_Mech_Piping_Sell_Dike::output_members(void){
	ostringstream prefix;
	prefix << "PIP_SELL> ";
	Sys_Common_Output::output_fpl->set_userprefix(prefix.str());
	ostringstream cout;
	cout << "Variable members mechanism piping after Sellmeijer..." << endl;
	Sys_Common_Output::output_fpl->output_txt(&cout,true);
	this->size_perm_layer.output_member();
	if(this->permeable_zone==NULL){
		this->dens_perm_layer.output_member();
		this->d_70.output_member();
	}
	else{
		cout << "The density of the permeable layer is found in material variable zone " << this->permeable_zone->get_zone_id() << endl;
		cout << "The d_70 of the permeable layer is found in material variable zone " << this->permeable_zone->get_zone_id() << endl;
		Sys_Common_Output::output_fpl->output_txt(&cout,true);
	}
	this->rolling_resistance_angle.output_member();
	this->drag_force_factor.output_member();
	this->k_f_perm_layer.output_member();
	cout << "Result members mechanism piping after Sellmeijer..." << endl;
	Sys_Common_Output::output_fpl->output_txt(&cout,true);
	this->resistance.output_member();

	Sys_Common_Output::output_fpl->rewind_userprefix();
}
//Check the statistic of the random variables of the mechanism
void Fpl_Mech_Piping_Sell_Dike::check_statistic(void){
	this->resistance.check_statistic();
	this->size_perm_layer.check_statistic();
	if(this->permeable_zone==NULL){
		this->dens_perm_layer.check_statistic();
		this->d_70.check_statistic();
	}
	this->rolling_resistance_angle.check_statistic();
	this->drag_force_factor.check_statistic();
	
	this->k_f_perm_layer.check_statistic();

}
//Reset the statistic important values
void Fpl_Mech_Piping_Sell_Dike::reset_statistics(void){
	this->resistance.reset_statistics();
	this->size_perm_layer.reset_statistics();
	if(this->permeable_zone==NULL){
		this->dens_perm_layer.reset_statistics();
		this->d_70.reset_statistics();
	}
	this->rolling_resistance_angle.reset_statistics();
	this->drag_force_factor.reset_statistics();
	
	this->k_f_perm_layer.reset_statistics();

	this->reset_counter_prob();
}
//Output the statistics of the random variables of the mechanism to display/console
void Fpl_Mech_Piping_Sell_Dike::output_statistic(void){
	ostringstream prefix;
	prefix << "PIP_SELL> ";
	Sys_Common_Output::output_fpl->set_userprefix(prefix.str());
	ostringstream cout;
	cout << "Variable members mechanism piping after Sellmeijer..." << endl;
	Sys_Common_Output::output_fpl->output_txt(&cout,true);
	this->size_perm_layer.output_statistic();
	if(this->permeable_zone==NULL){
		this->dens_perm_layer.output_statistic();
		this->d_70.output_statistic();
	}
	else{
		cout << "The density of the permeable layer is found in material variable zone " << this->permeable_zone->get_zone_id() << endl;
		cout << "The d_70 of the permeable layer is found in material variable zone " << this->permeable_zone->get_zone_id() << endl;
		Sys_Common_Output::output_fpl->output_txt(&cout,true);
	}
	this->rolling_resistance_angle.output_statistic();
	this->drag_force_factor.output_statistic();
	
	this->k_f_perm_layer.output_statistic();
	cout << "Result members mechanism piping after Sellmeijer..." << endl;
	Sys_Common_Output::output_fpl->output_txt(&cout,true);
	this->resistance.output_statistic();

	Sys_Common_Output::output_fpl->rewind_userprefix();
}
//Output the results of a deterministic calculation to display/console
void Fpl_Mech_Piping_Sell_Dike::output_determ_results(void){
	ostringstream prefix;
	prefix << "PIP_SELL> ";
	Sys_Common_Output::output_fpl->set_userprefix(prefix.str());
	ostringstream cout;
	cout << "Variable members mechanism piping after Sellmeijer..." << endl;
	cout << " Size permeable layer              : "<< this->size_perm_layer.get_result().end_result << label::m << endl;
	if(this->permeable_zone==NULL){
		cout << " Density material permeable layer  : "<< this->dens_perm_layer.get_result().end_result << label::kg_per_qm << endl;
		cout << " d_70                              : "<< this->d_70.get_result().end_result << label::m << endl;

	}
	else{
		cout << " Density material permeable layer  : "<< "see material variable zone "<< this->permeable_zone->get_zone_id() << endl;
		cout << " d_70                              : "<< "see material variable zone "<< this->permeable_zone->get_zone_id() << endl;

	}
	cout << " Rolling resistance angle          : "<< this->rolling_resistance_angle.get_result().end_result << label::degree << endl;
	cout << " Dragforce factor                  : "<< this->drag_force_factor.get_result().end_result << label::no_unit << endl;
	cout << " Permeability permeable layer      : "<< this->k_f_perm_layer.get_result().end_result << label::m_per_sec<< endl;
	cout << "Result members mechanism piping after Sellmeijer..." << endl;
	cout << " Resistance                : "<< this->resistance.get_result().end_result<<label::m<<endl;
	Sys_Common_Output::output_fpl->output_txt(&cout,false);
	Sys_Common_Output::output_fpl->rewind_userprefix();
}
//Calculate the mechanism; main input parameter is the waterlevel before the dike section
int Fpl_Mech_Piping_Sell_Dike::calculate_mechanism(const bool random_calculation, const double waterlevel, const double water_density, const double leak_length){
	this->failure=0;
	//reset the random flags
	this->reset_random_flag();
	this->buff_stress=waterlevel;
	this->buff_res=0.0;

	if(waterlevel<=0.0){
		this->failure=0;
		return this->failure;
	}

	//alpha
	this->buff_alpha=0.0;
	this->size_perm_layer.calculate_variables(random_calculation);
	double buff_size=this->size_perm_layer.get_result().end_result;
	if(buff_size/leak_length==1.0){
		buff_size=buff_size+constant::meter_epsilon;
	}
	this->buff_alpha=0.28/(pow((buff_size/leak_length),2.8)-1.0);
	this->buff_alpha=pow((buff_size/leak_length),this->buff_alpha);

	//k-value (intrinsinc permeability)
	this->buff_k=0.0;
	this->k_f_perm_layer.calculate_variables(random_calculation);
	this->buff_k=(constant::Cvisco_water/constant::Cgg)*this->k_f_perm_layer.get_result().end_result;

	//c-value
	this->buff_c=0.0;
	this->drag_force_factor.calculate_variables(random_calculation);
	//
	double d_70_buff=0.0;
	if(this->permeable_zone==NULL){
		this->d_70.calculate_variables(random_calculation);
		d_70_buff=this->d_70.get_result().end_result;
	}
	else{
		d_70_buff=this->permeable_zone->d_70.get_result().end_result;
	}
	this->buff_c=this->drag_force_factor.get_result().end_result*d_70_buff*pow((1.0/(buff_k*leak_length)),1.0/3.0);

	//resistance
	double density=0.0;
	if(this->permeable_zone==NULL){
		this->dens_perm_layer.calculate_variables(random_calculation);
		density=this->dens_perm_layer.get_result().end_result;
	}
	else{
		density=this->permeable_zone->grain_density.get_result().end_result;
	}
	this->rolling_resistance_angle.calculate_variables(random_calculation);
	this->buff_res=this->buff_alpha*this->buff_c*(density-water_density)/water_density*leak_length*tan(_Geo_Geometrie::grad_to_rad(this->rolling_resistance_angle.get_result().end_result));
	if(this->buff_c<=0.0){
		this->buff_c=constant::zero_epsilon;
	}
	else{
		this->buff_res=this->buff_res*(0.68-0.1*log(this->buff_c));
	}

	//set the results variables
	this->resistance.set_new_middle(this->buff_res);
	this->resistance.calculate_variables(random_calculation);

	if(this->resistance.get_result().end_result<=0.0){
		this->failure=1;
		return this->failure;
	}

	//check for failure
	if(this->buff_stress>=this->resistance.get_result().end_result){
		//failure
		this->failure=1;
	}
	else{
		this->failure=0;
	}




	return this->failure;

}
//Set the pointer to the permeable layer material variable zone
void Fpl_Mech_Piping_Sell_Dike::set_ptr_permeable_layer(Fpl_Dike_Var_Materialzone *zone){
	this->permeable_zone=zone;
}
//__________
//private
//Reset the random generated flags of the random variables
void Fpl_Mech_Piping_Sell_Dike::reset_random_flag(void){
	this->resistance.reset_random_generated();
	this->size_perm_layer.reset_random_generated();
	if(this->permeable_zone==NULL){
		this->dens_perm_layer.reset_random_generated();
	}
	this->rolling_resistance_angle.reset_random_generated();
	this->drag_force_factor.reset_random_generated();
	this->d_70.reset_random_generated();
	this->k_f_perm_layer.reset_random_generated();
}
//Set warning(s)
Warning Fpl_Mech_Piping_Sell_Dike::set_warning(const int warn_type){
	string place="Fpl_Mech_Piping_Sell_Dike::";
		string help;
		string reason;
		string reaction;
		int type=0;
		Warning msg;
		stringstream info;

	switch (warn_type){
		case 0://no mechanism is set
			place.append("set_variables(QSqlDatabase *ptr_database, const _sys_system_id id, const int section_id, Fpl_Dike_Var_Materialzone *zone, const int number)");
			reason="No material variable zone is found for the density of the permeable layer";
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