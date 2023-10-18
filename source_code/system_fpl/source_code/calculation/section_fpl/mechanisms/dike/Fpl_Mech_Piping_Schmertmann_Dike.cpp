#include "Fpl_Headers_Precompiled.h"
//#include "Fpl_Mech_Piping_Schmertmann_Dike.h"


//Default constructor
Fpl_Mech_Piping_Schmertmann_Dike::Fpl_Mech_Piping_Schmertmann_Dike(void){

	this->failure=0;
	this->buff_res=0.0;
	this->permeable_zone=NULL;
	this->sellmeijer_mechanism=NULL;

	this->d_10.set_unit(label::m);
	this->d_60.set_unit(label::m);
	this->size_perm_layer.set_unit(label::m);
	this->resistance.set_unit(label::m);



	Sys_Memory_Count::self()->add_mem(sizeof(Fpl_Mech_Piping_Schmertmann_Dike)-sizeof(Fpl_Random_Variables)*4,_sys_system_modules::FPL_SYS);//count the memory
}
//Default destructor
Fpl_Mech_Piping_Schmertmann_Dike::~Fpl_Mech_Piping_Schmertmann_Dike(void){


	Sys_Memory_Count::self()->minus_mem(sizeof(Fpl_Mech_Piping_Schmertmann_Dike)-sizeof(Fpl_Random_Variables)*4,_sys_system_modules::FPL_SYS);//count the memory

}
//__________
//public
//Set the random variables of the result members
void Fpl_Mech_Piping_Schmertmann_Dike::set_result_variables(QSqlDatabase *ptr_database, const _sys_system_id id, const int section_id){
	try{
		this->resistance.set_input(id, section_id,fpl_label::res_piping_schmert, true, ptr_database, fpl_label::mech_piping_schmert, fpl_label::sec_dike, 0, false);
		this->resistance.set_distribution_types(ptr_database);

	}catch(Error msg){
		throw msg;
	}
}
//Set the random input variables per database table
void Fpl_Mech_Piping_Schmertmann_Dike::set_variables(QSqlDatabase *ptr_database, const _sys_system_id id, const int section_id, Fpl_Dike_Var_Materialzone *zone, Fpl_Mech_Piping_Sell_Dike *selli){
	try{
		ostringstream cout;
		if(zone==NULL){
			try{
				this->d_10.set_input(id, section_id,fpl_label::d_10, false, ptr_database, fpl_label::mech_piping_schmert, fpl_label::sec_dike, -1, false);
				this->d_60.set_input(id, section_id,fpl_label::d_60, false, ptr_database, fpl_label::mech_piping_schmert, fpl_label::sec_dike, -1, false);
				this->d_10.set_distribution_types(ptr_database);
				this->d_60.set_distribution_types(ptr_database);
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
			
			cout <<"The permeable layer is connected to material variable zone "<<zone->get_zone_id()  << endl ;
			Sys_Common_Output::output_fpl->output_txt(&cout,false);
	
		}


		if(selli==NULL){
			try{
				this->size_perm_layer.set_input(id, section_id,fpl_label::size_perm_lay, false, ptr_database, fpl_label::mech_piping_schmert, fpl_label::sec_dike, -1, false);
				this->size_perm_layer.set_distribution_types(ptr_database);
			}
			catch(Error msg){
				ostringstream info;
				info<<" The piping mechanism of Sellmeijer is not found!"<<endl;
				msg.make_second_info(info.str());
				throw msg;
			}
		}
		else{
			this->sellmeijer_mechanism=selli;
			cout <<"The Schmertmann mechanism is connected to Sellmeijer's piping mechanism" << endl ;
			Sys_Common_Output::output_fpl->output_txt(&cout,false);
			if(zone==NULL){
				this->check_grain_size_distribution();
			}
		}
		
	}catch(Error msg){
		throw msg;
	}
}
//Set the random input variables per file
void Fpl_Mech_Piping_Schmertmann_Dike::set_variables(QFile *ifile, int *line_counter, const qint64 pos_file, const int line_counter_start, Fpl_Dike_Var_Materialzone *zone, Fpl_Mech_Piping_Sell_Dike *selli){
	ostringstream prefix;
	prefix << "PIP_SCHMERT> ";
	Sys_Common_Output::output_fpl->set_userprefix(prefix.str());
	ostringstream cout;
	cout << "Input variable members per file..." << endl;
	Sys_Common_Output::output_fpl->output_txt(&cout);
	
	try{

		if(zone==NULL){
			try{
				this->d_10.set_input(ifile, line_counter,fpl_label::d_10,false, fpl_label::end_mech_pip_schmert);
				this->d_10.check_given_boundaries(1.0,1e-20);
				ifile->seek(pos_file);
				*line_counter=line_counter_start;
				this->d_60.set_input(ifile, line_counter,fpl_label::d_60,false, fpl_label::end_mech_pip_schmert);
				this->d_60.check_given_boundaries(1.0,1e-20);
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
		if(selli==NULL){
			try{
				this->size_perm_layer.set_input(ifile, line_counter,fpl_label::size_perm_lay,false, fpl_label::end_mech_pip_schmert);
				this->size_perm_layer.check_positiv_variable();
				ifile->seek(pos_file);
				*line_counter=line_counter_start;
			}
			catch(Error msg){
				ostringstream info;
				info<<" The piping mechanism of Sellmeijer is not found!"<<endl;
				msg.make_second_info(info.str());
				throw msg;
			}
		}
		else{
			this->sellmeijer_mechanism=selli;
			cout <<"The Schmertmann mechanism is connected to Sellmeijer's piping mechanism" << endl ;
			Sys_Common_Output::output_fpl->output_txt(&cout,false);
			if(zone==NULL){
				this->check_grain_size_distribution();
			}
		}

		//result
		this->resistance.set_input(ifile, line_counter,fpl_label::res_piping_schmert,true, fpl_label::end_mech_pip_schmert);
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
//Transfer the piping-mechanism data to database, e.g. geometry, random variables etc.
void Fpl_Mech_Piping_Schmertmann_Dike::transfer_data2database(const int section_id, const _sys_system_id id, QSqlDatabase *ptr_database, Fpl_Dike_Var_Materialzone *zone, Fpl_Mech_Piping_Sell_Dike *selli){
	ostringstream prefix;
	ostringstream cout;
	prefix << "PIP_SCHMERT> ";
	Sys_Common_Output::output_fpl->set_userprefix(prefix.str());

	cout << "Transfer variable members to database..."<<endl;
	Sys_Common_Output::output_fpl->output_txt(&cout);
	if(zone==NULL){
		this->d_10.transfer_rand2database(ptr_database,id, section_id, fpl_label::mech_piping_schmert, fpl_label::sec_dike);
		this->d_60.transfer_rand2database(ptr_database,id, section_id, fpl_label::mech_piping_schmert, fpl_label::sec_dike);
	}
	if(selli==NULL){
		this->size_perm_layer.transfer_rand2database(ptr_database,id, section_id, fpl_label::mech_piping_schmert, fpl_label::sec_dike);
	}
	//result members
	cout << "Transfer result members to database..."<<endl;
	Sys_Common_Output::output_fpl->output_txt(&cout);
	this->resistance.transfer_rand2database(ptr_database,id, section_id, fpl_label::mech_piping_schmert, fpl_label::sec_dike);


	Sys_Common_Output::output_fpl->rewind_userprefix();
}
//Output members of the mechanism to display/console
void Fpl_Mech_Piping_Schmertmann_Dike::output_members(void){
	ostringstream prefix;
	prefix << "PIP_SCHMERT> ";
	Sys_Common_Output::output_fpl->set_userprefix(prefix.str());
	ostringstream cout;
	cout << "Variable members mechanism piping after Schmertmann..." << endl;
	Sys_Common_Output::output_fpl->output_txt(&cout,true);
	if(this->permeable_zone==NULL){
		this->d_10.output_member();
		this->d_60.output_member();
	}
	else{
		cout << "The d_10 of the permeable layer is found in material variable zone " << this->permeable_zone->get_zone_id() << endl;
		cout << "The d_60 of the permeable layer is found in material variable zone " << this->permeable_zone->get_zone_id() << endl;
		Sys_Common_Output::output_fpl->output_txt(&cout,true);

	}
	if(this->sellmeijer_mechanism==NULL){
		this->size_perm_layer.output_member();
	}
	else{
		cout << "The size of the permeable layer is found in the output of the Sellmeijer mechanism" << endl;
		Sys_Common_Output::output_fpl->output_txt(&cout,true);
	}

	
	cout << "Result members mechanism piping after Schmertmann..." << endl;
	Sys_Common_Output::output_fpl->output_txt(&cout,true);
	this->resistance.output_member();

	Sys_Common_Output::output_fpl->rewind_userprefix();
}
//Check the statistic of the random variables of the mechanism
void Fpl_Mech_Piping_Schmertmann_Dike::check_statistic(void){
	if(this->permeable_zone==NULL){
		this->d_10.check_statistic();
		this->d_60.check_statistic();
	}
	if(this->sellmeijer_mechanism==NULL){
		this->size_perm_layer.check_statistic();
	}

	this->resistance.check_statistic();
}
//Reset the statistic important values
void Fpl_Mech_Piping_Schmertmann_Dike::reset_statistics(void){
	if(this->permeable_zone==NULL){
		this->d_10.reset_statistics();
		this->d_60.reset_statistics();
	}
	if(this->sellmeijer_mechanism==NULL){
		this->size_perm_layer.reset_statistics();
	}

	this->resistance.reset_statistics();

	this->reset_counter_prob();
}
//Output the statistics of the random variables of the mechanism to display/console
void Fpl_Mech_Piping_Schmertmann_Dike::output_statistic(void){
	ostringstream prefix;
	prefix << "PIP_SCHMERT> ";
	Sys_Common_Output::output_fpl->set_userprefix(prefix.str());
	ostringstream cout;
	cout << "Variable members mechanism piping after Schmertmann..." << endl;
	Sys_Common_Output::output_fpl->output_txt(&cout,true);
	if(this->permeable_zone==NULL){
		this->d_10.output_statistic();
		this->d_60.output_statistic();
	}
	else{
		cout << "The d_10 of the permeable layer is found in material variable zone " << this->permeable_zone->get_zone_id() << endl;
		cout << "The d_60 of the permeable layer is found in material variable zone " << this->permeable_zone->get_zone_id() << endl;
		Sys_Common_Output::output_fpl->output_txt(&cout,true);

	}
	if(this->sellmeijer_mechanism==NULL){
		this->size_perm_layer.output_statistic();
	}
	else{
		cout << "The size of the permeable layer is found in the output of the Sellmeijer mechanism" << endl;
		Sys_Common_Output::output_fpl->output_txt(&cout,true);
	}


	cout << "Result members mechanism piping after Schmertmann..." << endl;
	Sys_Common_Output::output_fpl->output_txt(&cout,true);
	this->resistance.output_statistic();

	Sys_Common_Output::output_fpl->rewind_userprefix();
}
//Output the results of a deterministic calculation to display/console
void Fpl_Mech_Piping_Schmertmann_Dike::output_determ_results(void){
	ostringstream prefix;
	prefix << "PIP_SCHMERT> ";
	Sys_Common_Output::output_fpl->set_userprefix(prefix.str());
	ostringstream cout;
	cout << "Variable members mechanism piping after Schmertmann..." << endl;
	if(this->permeable_zone==NULL){
		cout << " d_10                              : "<< this->d_10.get_result().end_result << label::m << endl;
		cout << " d_60                              : "<< this->d_60.get_result().end_result << label::m << endl;
	}
	else{
		cout << " d_10                              : "<< "see material variable zone "<< this->permeable_zone->get_zone_id() << endl;
		cout << " d_60                              : "<< "see material variable zone "<< this->permeable_zone->get_zone_id() << endl;
	}
	if(this->sellmeijer_mechanism==NULL){
		cout << " Size of the permeable zone        : "<< this->size_perm_layer.get_result().end_result << label::m << endl;
	}
	else{
		cout << " Size of the permeable zone        : "<< "see output of the Sellmeijer mechanism" << endl;

	}

	cout << "Result members mechanism piping after Schmertmann..." << endl;
	cout << " Resistance                : "<< this->resistance.get_result().end_result<<label::m<<endl;
	Sys_Common_Output::output_fpl->output_txt(&cout,false);
	Sys_Common_Output::output_fpl->rewind_userprefix();
}
//Calculate the mechanism; main input parameter is the waterlevel before the dike section
int Fpl_Mech_Piping_Schmertmann_Dike::calculate_mechanism(const bool random_calculation, const double waterlevel, const double leak_length){
	this->failure=0;
	//reset the random flags
	this->reset_random_flag();
	this->buff_res=0.0;

	//C_l factor
	this->buff_res=pow(1.5239/leak_length,0.2);
	//C_s factor

	double d_10_buff=0.0;
	double d_60_buff=0.0;
	if(this->permeable_zone==NULL){
		if(this->sellmeijer_mechanism!=NULL){
			this->d_60.set_max_boundary(this->sellmeijer_mechanism->d_70.get_result().end_result);
		}
		this->d_60.calculate_variables(random_calculation);
		d_60_buff=this->d_60.get_result().end_result;
		this->d_10.set_max_boundary(d_60_buff);
		this->d_10.calculate_variables(random_calculation);
		d_10_buff=this->d_10.get_result().end_result;
	}
	else{
		d_10_buff=this->permeable_zone->d_10.get_result().end_result;
		d_60_buff=this->permeable_zone->d_60.get_result().end_result;
	}

	this->buff_res=this->buff_res*pow(d_10_buff*1000.0/0.2,0.2);

	double buff_size=0.0;
	if(this->sellmeijer_mechanism==NULL){
		this->size_perm_layer.calculate_variables(random_calculation);
		buff_size=this->size_perm_layer.get_result().end_result;
	}
	else{
		//use it from the sellmeijer calculation
		buff_size=this->sellmeijer_mechanism->size_perm_layer.get_result().end_result;
	}
	if(buff_size/leak_length==1.0){
		buff_size=buff_size+constant::meter_epsilon;
	}

	this->buff_res=this->buff_res/1.39836*pow((buff_size/leak_length),(0.2/(pow((buff_size/leak_length),2.0)-1.0)));

	this->buff_res=this->buff_res*(0.05+0.183*((d_60_buff/d_10_buff)-1.0))*leak_length;

	//set the results variables
	this->resistance.set_new_middle(this->buff_res);
	this->resistance.calculate_variables(random_calculation);

	if(this->resistance.get_result().end_result<=0.0){
		this->failure=1;
		return this->failure;
	}

	//check for failure
	if(waterlevel>=this->resistance.get_result().end_result){
		//failure
		this->failure=1;
	}
	else{
		this->failure=0;
	}


	return this->failure;
}
//Set the pointer to the permeable layer material variable zone
void Fpl_Mech_Piping_Schmertmann_Dike::set_ptr_permeable_layer(Fpl_Dike_Var_Materialzone *zone){
	this->permeable_zone=zone;
}
//Set the pointer to the Sellmeijer piping mechanism
void Fpl_Mech_Piping_Schmertmann_Dike::set_ptr_sellmeijer_piping(Fpl_Mech_Piping_Sell_Dike *selli){
	this->sellmeijer_mechanism=selli;
}
//_________
//private
//Reset the random generated flags of the random variables
void Fpl_Mech_Piping_Schmertmann_Dike::reset_random_flag(void){
	if(this->permeable_zone==NULL){
		this->d_10.reset_random_generated();
		this->d_60.reset_random_generated();
	}
	if(this->sellmeijer_mechanism==NULL){
		this->size_perm_layer.reset_random_generated();
	}
	this->resistance.reset_random_generated();
}
//Check the grain size distribution if no permeable layer is set
void Fpl_Mech_Piping_Schmertmann_Dike::check_grain_size_distribution(void){

	if(this->d_10.get_deterministic_mid()>this->d_60.get_deterministic_mid()){
		Error msg=this->set_error(0);
		ostringstream info;
		info << "Grain size d_10 : "<< this->d_10.get_deterministic_mid() << label::m<<endl; 
		info << "Grain size d_60 : "<< this->d_60.get_deterministic_mid() << label::m<<endl; 
		msg.make_second_info(info.str());
		throw msg;

	}
	if(this->sellmeijer_mechanism!=NULL){
		if(this->d_60.get_deterministic_mid()>this->sellmeijer_mechanism->d_70.get_deterministic_mid()){
			Error msg=this->set_error(1);
			ostringstream info;
			info << "Grain size d_60 : "<< this->d_60.get_deterministic_mid() << label::m<<endl; 
			info << "Grain size d_70 : "<< this->sellmeijer_mechanism->d_70.get_deterministic_mid() << label::m<<endl; 
			msg.make_second_info(info.str());
			throw msg;
		}
	}
}
//set the error
Error Fpl_Mech_Piping_Schmertmann_Dike::set_error(const int err_type){
		string place="Fpl_Mech_Piping_Schmertmann_Dike::";
		string help;
		string reason;
		int type=0;
		bool fatal=false;
		stringstream info;
		Error msg;

	switch (err_type){
		case 0://d_60<d_10
			place.append("check_grain_size_distribution(void)");
			reason="The grain size distribution is wrong set. d_10 is greater than d_60";
			help="Check the grain size distribution (d_10, d_60)";
			type=3;
			break;
		case 1://d_60>d_70
			place.append("check_grain_size_distribution(void)");
			reason="The grain size distribution is wrong set. d_60 is greater than d_70 (piping after Sellmeijer)";
			help="Check the grain size distribution (d_60, d_70)";
			type=3;
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
