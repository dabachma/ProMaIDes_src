//#include "HydTemp_Param.h"
#include "Hyd_Headers_Precompiled.h"

//constructor
HydTemp_Param::HydTemp_Param(void){

	this->temp_profile_File= label::not_set;

	this->gw_temp =-1;
	this->brunt_coef = -1;
	this->view2sky_coef = -1;
	this->con_bed_coef = -1;
	this->bed_temp = -1;
	this->bed_warming_coeff = -1;
	this->diffusiv_sol_rad_coeff = -1;

	this->Param_RV = NULL;
	

	//count the memory
	Sys_Memory_Count::self()->add_mem(sizeof(HydTemp_Param), _sys_system_modules::HYD_SYS);
}
//copy constructor
HydTemp_Param::HydTemp_Param(const HydTemp_Param& par){

	this->temp_profile_File = par.temp_profile_File;

	this->gw_temp = par.gw_temp;
	this->brunt_coef = par.brunt_coef;
	this->view2sky_coef = par.view2sky_coef;
	this->con_bed_coef = par.con_bed_coef;
	this->bed_temp = par.bed_temp;
	this->bed_warming_coeff = par.bed_warming_coeff;
	this->diffusiv_sol_rad_coeff = par.diffusiv_sol_rad_coeff;

	this->Param_RV = par.Param_RV;



	//count the memory
	Sys_Memory_Count::self()->add_mem(sizeof(HydTemp_Param), _sys_system_modules::HYD_SYS);
}
//destructor
HydTemp_Param::~HydTemp_Param(void){
	//count the memory
	Sys_Memory_Count::self()->minus_mem(sizeof(HydTemp_Param), _sys_system_modules::HYD_SYS);
}
//_______________
//public
//Set the temperature model parameters of the river model per parser from file 
void HydTemp_Param::parameter_per_parser(const string file, const int index, const string path){
	HydTemp_Parse temp_parser;
	ostringstream cout;
	cout << "Read the temperature model parameters of the river model from file " <<file << "..." <<endl;
	Sys_Common_Output::output_hyd->output_txt(&cout);

	//set the file name 
	temp_parser.set_input_filename(file);
	temp_parser.temp_params.Param_RV = this->Param_RV;
	
	try{
		//open file
		temp_parser.open_input_file();
		//read in the parameters
		temp_parser.parse_tempmodel_params(index);
	}
	catch(Error msg){
		temp_parser.close_input_file();
		throw msg;
	}

	//close the file
	temp_parser.close_input_file();

	*this= temp_parser.get_tempmodel_params();

	this->complete_filenames_with_path(path);

}
//output the members of the floodplain
void HydTemp_Param::output_members(void){
	//set prefix for output
	ostringstream prefix;
	prefix << "GEN> ";
	Sys_Common_Output::output_hyd->set_userprefix(prefix.str());
	//output
	ostringstream cout;
	// Put out specific information for the model
	cout << "GENERAL" << endl;
	cout << " Number                       : " << W(7) << this->Param_RV->get_river_number() << endl; 
	cout << " Name                         : " << W(7)  << this->Param_RV->get_river_name() << endl;
	cout << "PROFILES"<<endl;
	cout << " Number of profiles           : " << W(7)  << this->Param_RV->get_number_profiles() << endl;
	cout << " Profile data file            : " << W(7) << Hyd_Param_Global::get_print_filename(37, this->temp_profile_File.c_str())<< endl;
	if(Param_RV->number_instat_boundary>0){
		cout << "INSTAT-BOUNDARYCONDITIONS  " << endl;
		cout << " Instationary boundaryfile    : " << W(7) << Hyd_Param_Global::get_print_filename(37, Param_RV->inst_boundary_file_name.c_str())<< endl;
		cout << " Number of curves             : " << W(7) << Param_RV->number_instat_boundary<< endl;
	}

	cout << "VALUES" << endl;
	cout << " Groundwater temperature [K]               : " << W(7) << P(2)<< this->gw_temp <<endl;
	cout << " Brunt coefficient [-]                     : " << W(7) << P(3)<< this->brunt_coef <<endl;
	cout << " View-to-sky coefficient [-]               : " << W(7) << P(2) << this->view2sky_coef << endl;
	cout << " Coefficient of heat conductivity [-]      : " << W(7) << P(3) << this->con_bed_coef << endl;

	cout << " Bed temperature [K]                       : " << W(7) << P(3) << this->bed_temp << endl;
	cout << " Coefficient river bed warming [-]         : " << W(7) << P(2) << this->bed_warming_coeff << endl;
	cout << " Coefficient diffusive solar radiation [-] : " << W(7) << P(3) << this->diffusiv_sol_rad_coeff << endl;

	Sys_Common_Output::output_hyd->output_txt(&cout);

	//rewind the prefix
	Sys_Common_Output::output_hyd->rewind_userprefix();
}
//check the members
void HydTemp_Param::check_members(void){


	//warnings
	//check GW-temp
	if(this->gw_temp < 0.0){
		Warning msg=this->set_warning(0);
		stringstream info;
		info << "Default value: 287 K"<< endl;
		msg.make_second_info(info.str());
		//reaction
		this->gw_temp = 287.0;
		msg.output_msg(2);
	}




	//TODO UDO: weitere Checks!


}
//Get the filename for the rivermodel results file as 2d for file
string HydTemp_Param::get_filename_result2file_2d(const string type){
	string buffer;
	buffer=this->Param_RV->tecplot_outfile_name_2d;
	
	if(buffer!=label::not_set){
		stringstream suffix;
		suffix << "RVTEMP_RES_2D_"<<this->Param_RV->RVNumber;
		buffer = functions::make_complete_output_path(buffer, type, suffix.str());
	}
	return buffer;
}
//Get the filename for the rivermodel results file as 1d for file
string HydTemp_Param::get_filename_result2file_1d(const string type){
	string buffer;
	buffer=this->Param_RV->tecplot_outfile_name_1d;
	
	if(buffer!=label::not_set){
		stringstream suffix;
		suffix << "RVTEMP_RES_1D_"<<this->Param_RV->RVNumber;
		buffer = functions::make_complete_output_path(buffer, type, suffix.str());
	}
	return buffer;
}
//Get the filename for the rivermodel maximum result to file as 1d for file
string HydTemp_Param::get_filename_result2file_1d_maxvalues(const string type){
	string buffer;
	buffer=this->Param_RV->tecplot_outfile_name_1d;
	
	if(buffer!=label::not_set){
		stringstream suffix;
		suffix << "RVTEMP_MAXRES_1D_"<<this->Param_RV->RVNumber;
		buffer = functions::make_complete_output_path(buffer, type, suffix.str());
	}
	return buffer;
}
///Get the filename for the rivermodel maximum result to file as 2d file output
string HydTemp_Param::get_filename_result2file_2d_maxvalues(const string type) {
	string buffer;
	buffer = this->Param_RV->tecplot_outfile_name_2d;

	if (buffer != label::not_set) {
		stringstream suffix;
		suffix << "RVTEMP_MAXRES_2D_" << this->Param_RV->RVNumber;
		buffer = functions::make_complete_output_path(buffer, type, suffix.str());
	}
	return buffer;
}
//Get the filename for the rivermodel observation points to file 
string HydTemp_Param::get_filename_result2file_1d_obs_point(const string type){
	string buffer;
	buffer=this->Param_RV->tecplot_outfile_name_1d;
	
	if(buffer!=label::not_set){
		stringstream suffix;
		suffix << "RVTEMP_OBS_POINT";
		buffer = functions::make_complete_output_path(buffer, type, suffix.str());
	}
	return buffer;
}
//Set the temperature data profile file directly
void HydTemp_Param::set_temp_profile_file(const string file){
	this->temp_profile_File=file;
}
//copy operator
HydTemp_Param& HydTemp_Param::operator= (const HydTemp_Param& par){
	this->temp_profile_File = par.temp_profile_File;

	this->gw_temp = par.gw_temp;
	this->brunt_coef = par.brunt_coef;
	this->view2sky_coef = par.view2sky_coef;
	this->con_bed_coef = par.con_bed_coef;
	this->bed_temp = par.bed_temp;
	this->bed_warming_coeff = par.bed_warming_coeff;
	this->diffusiv_sol_rad_coeff = par.diffusiv_sol_rad_coeff;

	this->Param_RV = par.Param_RV;

	return *this;
}
////________________
//private
//Complete the filenames with the path
void HydTemp_Param::complete_filenames_with_path(string global_path) {
	ostringstream buff;
	buff << global_path << this->temp_profile_File;
	this->temp_profile_File = buff.str();
	buff.str("");
	buff.clear();

}
//Set the warning
Warning HydTemp_Param::set_warning(const int warn_type){
	string place="HydTemp_Param::";
		string help;
		string reason;
		string reaction;
		int type=0;
		Warning msg;
		stringstream info;

	switch (warn_type){
		case 0://absolute solver tolerance
			place.append("check_members(void)") ;
			reason="The groundwater temperature is below 0.0 K";
			reaction="The default value of 287 k is taken";
			help= "Check the given groundwater temperature";
			info << "River Model number : " << this->Param_RV->get_river_number()<< endl;
            type=3;
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
//set the error
Error HydTemp_Param::set_error(const int err_type){
	string place="HydTemp_Param::";
		string help;
		string reason;
		int type=0;
		bool fatal=false;
		stringstream info;
		Error msg;

	switch (err_type){
		case 0://minum numbers of profiles is 2 currently not used
			place.append("check_members(void)");
			reason="There have to be minimal two profiles";
			help="Check the number of profiles in the global file for this River";
			type=15;
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
