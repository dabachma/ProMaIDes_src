#include "Fpl_Headers_Precompiled.h"
//#include "Fpl_Dike_Var_Materialzone.h"


//Default constructor
Fpl_Dike_Var_Materialzone::Fpl_Dike_Var_Materialzone(void){

	this->zone_id=-1;
	this->uplift_density=0.0;
	this->wet_density=0.0;
	this->tan_friction=0.0;
	this->grain_density.set_unit(label::kg_per_qm);
	this->d_10.set_unit(label::m);
	this->d_60.set_unit(label::m);
	this->d_70.set_unit(label::m);
	this->angle_friction.set_unit(label::degree);
	this->cohesion.set_unit(label::n_per_sqm);

	this->slope_required=false;
	this->grain_size_required=false;

	Sys_Memory_Count::self()->add_mem(sizeof(Fpl_Dike_Var_Materialzone)-5*sizeof(Fpl_Random_Variables), _sys_system_modules::FPL_SYS);//count the memory
}
//Default destructor
Fpl_Dike_Var_Materialzone::~Fpl_Dike_Var_Materialzone(void){

	Sys_Memory_Count::self()->minus_mem(sizeof(Fpl_Dike_Var_Materialzone)-5*sizeof(Fpl_Random_Variables), _sys_system_modules::FPL_SYS);//count the memory
}
//_________
//public
//Set the variable of the material variable zone via database
void Fpl_Dike_Var_Materialzone::set_input(const int section_id, const _sys_system_id id, QSqlDatabase *ptr_database, const int zone_id, const bool slope_required, const bool grain_size_required){
	try{
		this->zone_id=zone_id;
		this->slope_required=slope_required;
		this->grain_size_required=grain_size_required;


		if(this->slope_required==true){
			this->porosity.set_input(id, section_id, fpl_label::porosity, false, ptr_database, fpl_label::material_zone, fpl_label::sec_dike, zone_id, false);
			this->porosity.set_distribution_types(ptr_database);
			this->water_content.set_input(id, section_id, fpl_label::water_content, false, ptr_database, fpl_label::material_zone, fpl_label::sec_dike, zone_id, false);
			this->water_content.set_distribution_types(ptr_database);
			this->grain_density.set_input(id, section_id, fpl_label::grain_density, false, ptr_database, fpl_label::material_zone, fpl_label::sec_dike, zone_id, false);
			this->grain_density.set_distribution_types(ptr_database);
			this->angle_friction.set_input(id, section_id, fpl_label::friction_angle, false, ptr_database, fpl_label::material_zone, fpl_label::sec_dike, zone_id, false);
			this->angle_friction.set_distribution_types(ptr_database);
			this->cohesion.set_input(id, section_id, fpl_label::cohesion, false, ptr_database, fpl_label::material_zone, fpl_label::sec_dike, zone_id, false);
			this->cohesion.set_distribution_types(ptr_database);
		}
		if(this->grain_size_required){
			this->d_10.set_input(id, section_id, fpl_label::d_10, false, ptr_database, fpl_label::material_zone, fpl_label::sec_dike, zone_id, false);
			this->d_10.set_distribution_types(ptr_database);
			this->d_60.set_input(id, section_id, fpl_label::d_60, false, ptr_database, fpl_label::material_zone, fpl_label::sec_dike, zone_id, false);
			this->d_60.set_distribution_types(ptr_database);
			this->d_70.set_input(id, section_id, fpl_label::d_70, false, ptr_database, fpl_label::material_zone, fpl_label::sec_dike, zone_id, false);
			this->d_70.set_distribution_types(ptr_database);
			this->check_grain_size_distribution();
		}
	}catch(Error msg){
		throw msg;
	}

}
//Transfer the slope stability-mechanism random variables data to database
void Fpl_Dike_Var_Materialzone::transfer_data2database(const int section_id, const _sys_system_id id, QSqlDatabase *ptr_database){
	
	if(this->slope_required==true){
		this->porosity.transfer_rand2database(ptr_database,id, section_id, fpl_label::material_zone, fpl_label::sec_dike);
		this->water_content.transfer_rand2database(ptr_database,id, section_id, fpl_label::material_zone, fpl_label::sec_dike);
		this->grain_density.transfer_rand2database(ptr_database,id, section_id, fpl_label::material_zone, fpl_label::sec_dike);

		this->angle_friction.transfer_rand2database(ptr_database,id, section_id, fpl_label::material_zone, fpl_label::sec_dike);
		this->cohesion.transfer_rand2database(ptr_database,id, section_id, fpl_label::material_zone, fpl_label::sec_dike);
	}
	if(this->grain_size_required==true){
		this->d_10.transfer_rand2database(ptr_database,id, section_id, fpl_label::material_zone, fpl_label::sec_dike);
		this->d_60.transfer_rand2database(ptr_database,id, section_id, fpl_label::material_zone, fpl_label::sec_dike);
		this->d_70.transfer_rand2database(ptr_database,id, section_id, fpl_label::material_zone, fpl_label::sec_dike);

	}
}
//Read the material variable zone per file
void Fpl_Dike_Var_Materialzone::read_matzone_per_file(QFile *ifile, int *line_counter, const bool slope_required, const int index_perm_layer){

	qint64 pos_file=0;
	int store_line=0;
	int must_found_counter=0;

	this->slope_required=slope_required;
	

	//find the start of the info block
	try{
		pos_file=this->find_start_block_file(ifile, line_counter, fpl_label::begin_material_zone_info, fpl_label::end_material_zone_info, true);
		store_line=*line_counter;
	}
	catch(Error msg){
		throw msg;
	}

	//read in
	string myline;
	QString qmyline;
	//read in the zone infos
	do{
		qmyline=ifile->readLine();
		myline=qmyline.toStdString();
		(*line_counter)++;
		functions::clean_string(&myline);
		if(myline==fpl_label::end_material_zone_info){
			break;
		}
		if(myline.empty()==false){
			this->find_key_values_file(myline, &must_found_counter);
		}
		if(must_found_counter==1){
			break;
		}
	}
	while(ifile->atEnd()==false);


	if(must_found_counter!=1){
		Error msg=this->set_error(4);
		throw msg;
	}

	if(this->zone_id==index_perm_layer){
		this->grain_size_required=true;
	}

	//read in random variables
	try{
		if(this->slope_required==true){
			this->porosity.set_input(ifile, line_counter, fpl_label::porosity, false,fpl_label::end_material_zone_info);
			this->porosity.check_given_boundaries(1.0,0.0);
			ifile->seek(pos_file);
			*line_counter=store_line;
			this->water_content.set_input(ifile, line_counter, fpl_label::water_content, false,fpl_label::end_material_zone_info);
			this->water_content.check_given_boundaries(1.0,0.0);
			ifile->seek(pos_file);
			*line_counter=store_line;
			this->grain_density.set_input(ifile, line_counter, fpl_label::grain_density, false,fpl_label::end_material_zone_info);
			this->grain_density.check_given_boundaries(10000.0,500.0);
			ifile->seek(pos_file);
			*line_counter=store_line;
			this->cohesion.set_input(ifile, line_counter, fpl_label::cohesion, false,fpl_label::end_material_zone_info);
			this->cohesion.check_positiv_zero_variable();
			ifile->seek(pos_file);
			*line_counter=store_line;
			this->angle_friction.set_input(ifile, line_counter, fpl_label::friction_angle, false,fpl_label::end_material_zone_info);
			this->angle_friction.check_given_boundaries(70.0,0.0);
			ifile->seek(pos_file);
			*line_counter=store_line;
		}
		if(this->grain_size_required==true){
			this->d_10.set_input(ifile, line_counter, fpl_label::d_10, false,fpl_label::end_material_zone_info);
			this->d_10.check_given_boundaries(1.0,1e-20);
			ifile->seek(pos_file);
			*line_counter=store_line;
			this->d_60.set_input(ifile, line_counter, fpl_label::d_60, false,fpl_label::end_material_zone_info);
			this->d_60.check_given_boundaries(1.0,1e-20);
			ifile->seek(pos_file);
			*line_counter=store_line;
			this->d_70.set_input(ifile, line_counter, fpl_label::d_70, false,fpl_label::end_material_zone_info);
			this->d_70.check_given_boundaries(1.0,1e-20);
			ifile->seek(pos_file);
			*line_counter=store_line;
			this->check_grain_size_distribution();
		}

	}
	catch(Error msg){
		ostringstream info;
		info<<"Material variable zone id: " << this->zone_id <<endl;
		msg.make_second_info(info.str());
		throw msg;
	}

	//set the zone id
	if(this->slope_required==true){
		this->porosity.set_zone_id(this->zone_id);
		this->water_content.set_zone_id(this->zone_id);
		this->grain_density.set_zone_id(this->zone_id);
		this->cohesion.set_zone_id(this->zone_id);
		this->angle_friction.set_zone_id(this->zone_id);
	}
	if(this->grain_size_required==true){
		this->d_10.set_zone_id(this->zone_id);
		this->d_60.set_zone_id(this->zone_id);
		this->d_70.set_zone_id(this->zone_id);
	}


	//search for the end
	bool found_flag=false;
	do{
		qmyline=ifile->readLine();
		myline=qmyline.toStdString();
		(*line_counter)++;
		functions::clean_string(&myline);

		if(myline==fpl_label::end_material_zone_info){
			found_flag=true;
			break;
		}
	}
	while(ifile->atEnd()==false);


	if(found_flag==false ){
		Error msg=this->set_error(8);
		ostringstream info;
		info << "End keyword   : " << fpl_label::end_material_zone_info<<endl;
		msg.make_second_info(info.str());
 		throw msg;

	}

}
//Get the zone identifier
int Fpl_Dike_Var_Materialzone::get_zone_id(void){
	return this->zone_id;
}
//Calculate the densities
void Fpl_Dike_Var_Materialzone::calc_densities(const double water_density){

	//uplift density
	//rho_uplift=(1.0-porosity)*(rho_grain-rho_water)
	this->uplift_density=(1.0-this->porosity.get_result().end_result)*(this->grain_density.get_result().end_result-water_density);
	if(this->uplift_density<=1.0){
		this->uplift_density=1.0;
	}
	//wet density
	//rho_wet=(1.0-porosity)*rho_grain*(1.0+water_content)
	this->wet_density=(1.0-this->porosity.get_result().end_result)*this->grain_density.get_result().end_result*(1.0+this->water_content.get_result().end_result);
	if(this->wet_density<=1.0){
		this->wet_density=1.0;
	}
}
//Get the uplift density of the soil under water
double Fpl_Dike_Var_Materialzone::get_uplift_density(void){
	return this->uplift_density;
}
//Get the wet density of the soil
double Fpl_Dike_Var_Materialzone::get_wet_density(void){
	return this->wet_density;
}
//Reset the statistics of the random variables of the zone variables to display/console
void Fpl_Dike_Var_Materialzone::reset_statistic(void){
	if(this->slope_required==true){
		this->angle_friction.reset_statistics();
		this->grain_density.reset_statistics();
		this->porosity.reset_statistics();
		this->water_content.reset_statistics();
		this->cohesion.reset_statistics();
	}
	if(this->grain_size_required==true){
		this->d_10.reset_statistics();
		this->d_60.reset_statistics();
		this->d_70.reset_statistics();
	}
}
//Check the statistics of the random variables of the zone variables to display/console
void Fpl_Dike_Var_Materialzone::check_statistic(void){
	if(this->slope_required==true){
		this->angle_friction.check_statistic();
		this->grain_density.check_statistic();
		this->porosity.check_statistic();
		this->water_content.check_statistic();
		this->cohesion.check_statistic();
	}
	if(this->grain_size_required==true){
		this->d_10.check_statistic();
		this->d_60.check_statistic();
		this->d_70.check_statistic();
	}
}
//Output the statistics of the random variables of the zone variables to display/console
void Fpl_Dike_Var_Materialzone::output_statistic(void){
	ostringstream prefix;
	prefix << "ZONE_"<<this->zone_id<<"> ";
	Sys_Common_Output::output_fpl->set_userprefix(&prefix);
	if(this->slope_required==true){
		this->grain_density.output_statistic();
		this->porosity.output_statistic();
		this->water_content.output_statistic();
		this->angle_friction.output_statistic();
		this->cohesion.output_statistic();
	}
	if(this->grain_size_required==true){
		this->d_10.output_statistic();
		this->d_60.output_statistic();
		this->d_70.output_statistic();
	}
	Sys_Common_Output::output_fpl->rewind_userprefix();
}
//Output the results of a deterministic calculation to display/console
void Fpl_Dike_Var_Materialzone::output_determ_results(void){
	ostringstream prefix;
	prefix << "ZONE_"<<this->zone_id<<"> ";
	Sys_Common_Output::output_fpl->set_userprefix(&prefix);
	ostringstream cout;
	cout << "DETERMINISTIC"<<endl;
	Sys_Common_Output::output_fpl->output_txt(&cout, false);
	if(this->slope_required==true){
		this->grain_density.output_deterministic();
		this->porosity.output_deterministic();
		this->water_content.output_deterministic();
		this->angle_friction.output_deterministic();
		this->cohesion.output_deterministic();
	}
	if(this->grain_size_required==true){
		this->d_10.output_deterministic();
		this->d_60.output_deterministic();
		this->d_70.output_deterministic();
	}
	Sys_Common_Output::output_fpl->rewind_userprefix();
}
//Reset the random generated flag
void Fpl_Dike_Var_Materialzone::reset_random_flag(void){
	if(this->slope_required==true){
		this->angle_friction.reset_random_generated();
		this->grain_density.reset_random_generated();
		this->porosity.reset_random_generated();
		this->water_content.reset_random_generated();
		this->cohesion.reset_random_generated();
	}
	if(this->grain_size_required==true){
		this->d_10.reset_random_generated();
		this->d_60.reset_random_generated();
		this->d_70.reset_random_generated();
	}
}
//Reset the random generated flag
void Fpl_Dike_Var_Materialzone::generate_random_variables(const bool random_calculation){
	if(this->slope_required==true){
		this->angle_friction.calculate_variables(random_calculation);
		this->grain_density.calculate_variables(random_calculation);
		this->porosity.calculate_variables(random_calculation);
		this->water_content.calculate_variables(random_calculation);
		this->cohesion.calculate_variables(random_calculation);
	}
	if(this->grain_size_required==true){
		this->d_70.calculate_variables(random_calculation);
		this->d_60.set_max_boundary(this->d_70.get_result().end_result);
		this->d_60.calculate_variables(random_calculation);
		this->d_10.set_max_boundary(this->d_60.get_result().end_result);
		this->d_10.calculate_variables(random_calculation);
	}
}
//Output the material variable zone to display/console
void Fpl_Dike_Var_Materialzone::output_member(void){

	ostringstream prefix;
	prefix << "ZONE_"<<this->zone_id<<"> ";
	Sys_Common_Output::output_fpl->set_userprefix(&prefix);
	if(this->slope_required==true){
		this->grain_density.output_member();
		this->porosity.output_member();
		this->water_content.output_member();
		this->angle_friction.output_member();
		this->cohesion.output_member();
	}
	if(this->grain_size_required==true){
		this->d_10.output_member();
		this->d_60.output_member();
		this->d_70.output_member();
	
	}

	Sys_Common_Output::output_fpl->rewind_userprefix();

}
//Calculculate the tangens of the friction angle
void Fpl_Dike_Var_Materialzone::calc_tan_friction(void){
	this->tan_friction=tan(_Geo_Geometrie::grad_to_rad(this->angle_friction.get_result().end_result));
}
//Get the tangens friction angle
double Fpl_Dike_Var_Materialzone::get_tan_friction_angle(void){
	return this->tan_friction;
}
//________
//private
//Find the start of an input block of the fpl-section in file
qint64 Fpl_Dike_Var_Materialzone::find_start_block_file(QFile *ifile, int *line_counter, const string begin, const string end, const bool must_found){
	//read in
	string myline;
	QString qmyline;
	stringstream my_stream;
	bool found_flag=false;
	qint64 pos_file=0;

	//search for the begin
	do{
		qmyline=ifile->readLine();
		myline=qmyline.toStdString();
		(*line_counter)++;
		functions::clean_string(&myline);

		if(myline==begin){
			found_flag=true;
			pos_file=ifile->pos();
			break;
		}
		if(myline==end){
			break;
		}
	}
	while(ifile->atEnd()==false);


	if(found_flag==false && must_found==true){
		Error msg=this->set_error(0);
		ostringstream info;
		info << "Start keyword : " << begin << endl;
		info << "End keyword   : " << end<<endl;
		msg.make_second_info(info.str());
 		throw msg;

	}
	else{
		found_flag=false;
	}

	return pos_file;

}
//Find the key-values for the file input of the material zone data
void Fpl_Dike_Var_Materialzone::find_key_values_file(const string myline, int *must_found_counter){
	int pos=-1;
	string buffer;
	stringstream buffer1;
	bool wrong_input=false;



	//zone id
	pos=myline.find(fpl_label::key_zone_id);
	if(pos>=0 && wrong_input==false){
		buffer=myline.substr(fpl_label::key_zone_id.length());
		functions::clean_string(&buffer);
		buffer1 << buffer;
		buffer1 >> this->zone_id;
		if(buffer1.fail()==true){
			wrong_input=true;
		}
		else if(this->zone_id<=0){
			Error msg=this->set_error(9);
			ostringstream info;
			info<<"Found material variable zone id : "<< this->zone_id<<endl;
			msg.make_second_info(info.str());
			throw msg;
		}
		else{
			(*must_found_counter)++;
			return;
		}
	}
	if(wrong_input==true){
		ostringstream info;
		info << "Wrong input sequenze  : " << buffer1.str() << endl;
		Error msg=this->set_error(1);
		msg.make_second_info(info.str());
		throw msg;
	}
}
//Check the grain size distribution if no permeable layer is set
void Fpl_Dike_Var_Materialzone::check_grain_size_distribution(void){

	if(this->d_10.get_deterministic_mid()>this->d_60.get_deterministic_mid()){
		Error msg=this->set_error(10);
		ostringstream info;
		info << "Grain size d_10 : "<< this->d_10.get_deterministic_mid() << label::m<<endl; 
		info << "Grain size d_60 : "<< this->d_60.get_deterministic_mid() << label::m<<endl; 
		msg.make_second_info(info.str());
		throw msg;

	}
	
	if(this->d_60.get_deterministic_mid()>this->d_70.get_deterministic_mid()){
		Error msg=this->set_error(11);
		ostringstream info;
		info << "Grain size d_60 : "<< this->d_60.get_deterministic_mid() << label::m<<endl; 
		info << "Grain size d_70 : "<< d_70.get_deterministic_mid() << label::m<<endl; 
		msg.make_second_info(info.str());
		throw msg;
	}
	


}
//set the error
Error Fpl_Dike_Var_Materialzone::set_error(const int err_type){
		string place="Fpl_Dike_Var_Materialzone::";
		string help;
		string reason;
		int type=0;
		bool fatal=false;
		stringstream info;
		Error msg;

	switch (err_type){

		case 0://do not find the begin of the material zone information in file
			place.append("find_start_block_file(QFile *ifile, int *line_counter, const string begin, const string end, const bool must_found)");
			reason="Can not find the begin of the data block in file";
			help="The material variable zone block data has to start with the keys !$BEGIN_...";
			type=1;
			break;
		case 1://wrong input
			place.append("find_key_values_file(const string myline, int *must_found_counter)");
			reason="There is a problem with the input; wrong sign is read in";
			help="Check the information of the material zones in file";
			type=1;
			break;
		case 4://do not find all required section infos
			place.append("read_matzone_per_file(QFile *ifile, int *line_counter)");
			reason="Can not find all required material zone information";
			help="Check if all material variable zone information are set in the file";
			info << "Required information:"<<endl;
			info << " " << fpl_label::key_zone_id<< endl;
			type=1;
			break;
		case 6://wrong input
			place.append("read_matzone_per_file(QFile *ifile, int *line_counter)");
			reason="There is a problem with the input; wrong sign is read in";
			help="Check the information of the material zones in file";
			type=1;
			break;
		case 7://wrong input
			place.append("read_matzone_per_file(QFile *ifile, int *line_counter)");
			reason="Not the required number of points are found in file";
			help="Check the point information of the material zones in file";
			type=1;
			break;
		case 8://do not find the begin of the material zone information in file
			place.append("read_matzone_per_file(QFile *ifile, int *line_counter)");
			reason="Can not find the end of the data block in file";
			help="The material variable zone block data has to end with the keys !$END_...";
			type=1;
			break;
		case 9://zone id must be greater than zero
			place.append("find_key_values_file(const string myline, int *must_found_counter)");
			reason="The zone id has to be greater than 0";
			help="Check the zone id of the material variable zone(s) in file";
			type=1;
			break;
		case 10://d_60<d_10
			place.append("check_grain_size_distribution(void)");
			reason="The grain size distribution is wrong set. d_10 is greater than d_60";
			help="Check the grain size distribution (d_10, d_60)";
			type=3;
			break;
		case 11://d_60>d_70
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
