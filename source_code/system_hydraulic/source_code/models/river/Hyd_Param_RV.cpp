//#include "Hyd_Param_RV.h"
#include "Hyd_Headers_Precompiled.h"

//constructor
Hyd_Param_RV::Hyd_Param_RV(void):default_rel_tol(1.0e-7),default_abs_tol(1.0e-8){
	this->RVName=label::not_set;
	this->RVNumber = -1;

	this->number_instat_boundary=0;
	this->inst_boundary_file_name=label::not_set;

	this->RVNofProf = 0;
	this->RVGeomtryFile=label::not_set;

	this->tecplot_outfile_name_1d=label::not_set;
	this->tecplot_outfile_name_2d=label::not_set;

	this->abs_tolerance=this->default_abs_tol;
	this->rel_tolerance=this->default_rel_tol;

	this->user_setting.explict_v_is_used=false;
	this->user_setting.use_interface=false;

	

	//count the memory
	Sys_Memory_Count::self()->add_mem(sizeof(Hyd_Param_RV), _sys_system_modules::HYD_SYS);
}
//copy constructor
Hyd_Param_RV::Hyd_Param_RV(const Hyd_Param_RV& par):default_rel_tol(1.0e-7),default_abs_tol(1.0e-8){
	this->RVName=par.RVName;
	this->RVNumber = par.RVNumber;

	this->number_instat_boundary=par.number_instat_boundary;
	this->inst_boundary_file_name=par.inst_boundary_file_name;

	this->RVNofProf = par.RVNofProf;
	this->RVGeomtryFile=par.RVGeomtryFile;

	this->tecplot_outfile_name_1d=par.tecplot_outfile_name_1d;
	this->tecplot_outfile_name_2d=par.tecplot_outfile_name_2d;

	this->abs_tolerance=par.abs_tolerance;
	this->rel_tolerance=par.rel_tolerance;

	this->user_setting=par.user_setting;



	//count the memory
	Sys_Memory_Count::self()->add_mem(sizeof(Hyd_Param_RV), _sys_system_modules::HYD_SYS);
}
//destructor
Hyd_Param_RV::~Hyd_Param_RV(void){
	//count the memory
	Sys_Memory_Count::self()->minus_mem(sizeof(Hyd_Param_RV), _sys_system_modules::HYD_SYS);
}
//_______________
//public
//set the rv parameters per parser from file 
void Hyd_Param_RV::parameter_per_parser(const string file, const int index, const string path){
	Hyd_Parse_RV rv_parser;
	ostringstream cout;
	cout << "Read the rivermodel parameters from file " <<file << "..." <<endl;
	Sys_Common_Output::output_hyd->output_txt(&cout);

	//set the file name 
	rv_parser.set_input_filename(file);
	try{
		//open file
		rv_parser.open_input_file();
		//read in the parameters
		rv_parser.parse_rivermodel_params(index);
	}
	catch(Error msg){
		rv_parser.close_input_file();
		throw msg;
	}

	//close the file
	rv_parser.close_input_file();

	*this= rv_parser.get_rivermodel_params();

	this->complete_filenames_with_path(path);

}
//Get the river number
int Hyd_Param_RV::get_river_number(void){
	return this->RVNumber;
}
//Get the river name
string Hyd_Param_RV::get_river_name(void){
	return this->RVName;
}
//Get a pointer to the absolute tolerance for the solver
double* Hyd_Param_RV::get_absolute_solver_tolerance(void){
	return &this->abs_tolerance;
}
//Get the relative tolerance for the solver
double Hyd_Param_RV::get_relative_solver_tolerance(void){
	return this->rel_tolerance;
}
//output the members of the floodplain
void Hyd_Param_RV::output_members(void){
	//set prefix for output
	ostringstream prefix;
	prefix << "GEN> ";
	Sys_Common_Output::output_hyd->set_userprefix(prefix.str());
	//output
	ostringstream cout;
	// Put out specific information for the model
	cout << "GENERAL" << endl;
	cout << " Number                       : " << W(7) << this->RVNumber << endl; 
	cout << " Name                         : " << W(7)  <<this->RVName << endl;
	cout << "PROFILES"<<endl;
	cout << " Number of profiles           : " << W(7)  <<this->RVNofProf << endl;
	cout << " Geometryfile                 : " << W(7) << Hyd_Param_Global::get_print_filename(37, this->RVGeomtryFile.c_str())<< endl;
	if(this->number_instat_boundary>0){
		cout << "INSTAT-BOUNDARYCONDITIONS  " << endl;
		cout << " Instationary boundaryfile    : " << W(7) << Hyd_Param_Global::get_print_filename(37,this->inst_boundary_file_name.c_str())<< endl;
		cout << " Number of curves             : " << W(7) << this->number_instat_boundary<< endl;
	}

	cout << "SOLVER/CALCULATION" << endl;
	cout << " Absolute solver tolerance    : " << W(7) << P(2)<< FORMAT_SCIENTIFIC_REAL << this->abs_tolerance <<endl;
	cout << " Relative solver tolerance    : " << W(7) << P(2)<< FORMAT_SCIENTIFIC_REAL<< this->rel_tolerance  <<endl;
	cout << " Use velocity-head explicit   : " << functions::convert_boolean2string(this->user_setting.explict_v_is_used) <<endl;
	cout << " Use interface (mid to bank)  : " << functions::convert_boolean2string(this->user_setting.use_interface)  <<endl;

	if(this->tecplot_outfile_name_1d!=label::not_set){
		cout << "1D-RESULTFILE     " << endl;
		cout << " 1d-results in file           : " << W(7) << Hyd_Param_Global::get_print_filename(60,this->get_crude_filename_result_1d().c_str()) << endl;
	}
	if(this->tecplot_outfile_name_2d!=label::not_set){
		cout << "2D-RESULTFILE     " << endl;
		cout << " 2d-geometry in folder          : " << W(7) << Hyd_Param_Global::get_print_filename(60, this->get_crude_filename_result_1d().c_str()) << endl;
		cout << " 2d-results  in folder          : " << W(7) << Hyd_Param_Global::get_print_filename(60, this->get_crude_filename_result_1d().c_str()) << endl;
	}
	Sys_Common_Output::output_hyd->output_txt(&cout);

	//rewind the prefix
	Sys_Common_Output::output_hyd->rewind_userprefix();
}
//check the mebers
void Hyd_Param_RV::check_members(void){
	//check the numbers of profiles
	if(this->RVNofProf<2){
		Error msg=this->set_error(0);
		throw msg;
	}

	//warnings
	if(this->abs_tolerance>=1e-4 || this->abs_tolerance<=1e-15){
		Warning msg=this->set_warning(0);
		stringstream info;
		info << "Default value: " << this->default_abs_tol << endl;
		msg.make_second_info(info.str());
		//reaction
		this->abs_tolerance=this->default_abs_tol;
		msg.output_msg(2);

	}
	if(this->rel_tolerance>=1e-4 || this->rel_tolerance<=1e-15){
		Warning msg=this->set_warning(1);
		stringstream info;
		info << "Default value: " << this->default_rel_tol << endl;
		msg.make_second_info(info.str());
		//reaction
		this->rel_tolerance=this->default_rel_tol;
		msg.output_msg(2);
	}

}
//Get the filename for the rivermodel as 2d (geometrie) for file
string Hyd_Param_RV::get_filename_geometrie2file_2d(const string type){
	string buffer;
	buffer=this->tecplot_outfile_name_2d;
	
	if(buffer!=label::not_set){
		stringstream suffix;
		suffix << "RV_GEO_2D_"<<this->RVNumber;
		buffer=functions::make_complete_output_path(buffer, type, suffix.str());
	}
	return buffer;
}
//Get the filename for the rivermodel results file as 2d for file
string Hyd_Param_RV::get_filename_result2file_2d(const string type){
	string buffer;
	buffer=this->tecplot_outfile_name_2d;
	
	if(buffer!=label::not_set){
		stringstream suffix;
		suffix << "RV_RES_2D_"<<this->RVNumber;
		buffer = functions::make_complete_output_path(buffer, type, suffix.str());
	}
	return buffer;
}
//Get the filename for the rivermodel results file as 1d for file
string Hyd_Param_RV::get_filename_result2file_1d(const string type){
	string buffer;
	buffer=this->tecplot_outfile_name_1d;
	
	if(buffer!=label::not_set){
		stringstream suffix;
		suffix << "RV_RES_1D_"<<this->RVNumber;
		buffer = functions::make_complete_output_path(buffer, type, suffix.str());
	}
	return buffer;
}
//Get the filename for the rivermodel maximum result to file as 1d for file
string Hyd_Param_RV::get_filename_result2file_1d_maxvalues(const string type){
	string buffer;
	buffer=this->tecplot_outfile_name_1d;
	
	if(buffer!=label::not_set){
		stringstream suffix;
		suffix << "RV_MAXRES_1D_"<<this->RVNumber;
		buffer = functions::make_complete_output_path(buffer, type, suffix.str());
	}
	return buffer;
}
///Get the filename for the rivermodel maximum result to file as 2d file output
string Hyd_Param_RV::get_filename_result2file_2d_maxvalues(const string type) {
	string buffer;
	buffer = this->tecplot_outfile_name_2d;

	if (buffer != label::not_set) {
		stringstream suffix;
		suffix << "RV_MAXRES_2D_" << this->RVNumber;
		buffer = functions::make_complete_output_path(buffer, type, suffix.str());
	}
	return buffer;
}
//Get the filename for the rivermodel observation points to file 
string Hyd_Param_RV::get_filename_result2file_1d_obs_point(const string type){
	string buffer;
	buffer=this->tecplot_outfile_name_1d;
	
	if(buffer!=label::not_set){
		stringstream suffix;
		suffix << "RV_OBS_POINT";
		buffer = functions::make_complete_output_path(buffer, type, suffix.str());
	}
	return buffer;
}
//Get the crude version of the filename for 1d output
string Hyd_Param_RV::get_crude_filename_result_1d(void){
	return this->tecplot_outfile_name_1d;
}
//Get the number of profiles
int Hyd_Param_RV::get_number_profiles(void){
	return this->RVNofProf;
}

//Set the geometry file directly
void Hyd_Param_RV::set_geometry_file(const string file){
	this->RVGeomtryFile=file;
}
//Set the river name directly
void Hyd_Param_RV::set_river_name(const string name){
	this->RVName=name;
}
//Set number of instationary boudary curves
void Hyd_Param_RV::set_number_instant_bound(const int no){
	this->number_instat_boundary=no;
}

//copy operator
Hyd_Param_RV& Hyd_Param_RV::operator= (const Hyd_Param_RV& par){
	this->RVName=par.RVName;
	this->RVNumber = par.RVNumber;

	this->number_instat_boundary=par.number_instat_boundary;
	this->inst_boundary_file_name=par.inst_boundary_file_name;

	this->RVNofProf = par.RVNofProf;
	this->RVGeomtryFile=par.RVGeomtryFile;

	this->tecplot_outfile_name_1d=par.tecplot_outfile_name_1d;
	this->tecplot_outfile_name_2d=par.tecplot_outfile_name_2d;

	this->abs_tolerance=par.abs_tolerance;
	this->rel_tolerance=par.rel_tolerance;
	this->user_setting=par.user_setting;



	return *this;
}
//________________
//private
//Complete the filenames with the path
void Hyd_Param_RV::complete_filenames_with_path(string global_path){
	ostringstream buff;
	buff<<global_path<<this->RVGeomtryFile;
	this->RVGeomtryFile=buff.str();
	buff.str("");
	buff.clear();

	if(this->tecplot_outfile_name_1d!=label::not_set){
		buff<<global_path<<this->tecplot_outfile_name_1d;
		this->tecplot_outfile_name_1d=_Hyd_Parse_IO::insert_linux_slash2string(buff.str());
		buff.str("");
		buff.clear();

	}
	if(this->tecplot_outfile_name_2d!=label::not_set){
		buff<<global_path<<this->tecplot_outfile_name_2d;
		this->tecplot_outfile_name_2d=_Hyd_Parse_IO::insert_linux_slash2string(buff.str());
		buff.str("");
		buff.clear();

	}
	if(this->inst_boundary_file_name!=label::not_set){
		buff<<global_path<<this->inst_boundary_file_name;
		this->inst_boundary_file_name=_Hyd_Parse_IO::insert_linux_slash2string(buff.str());
		buff.str("");
		buff.clear();

	}
}
//Set the warning
Warning Hyd_Param_RV::set_warning(const int warn_type){
	string place="Hyd_Param_RV::";
		string help;
		string reason;
		string reaction;
		int type=0;
		Warning msg;
		stringstream info;

	switch (warn_type){
		case 0://absolute solver tolerance
			place.append("check_members(void)") ;
			reason="The absolute solver tolerance have to be: 1e-15<= abs_tol <=1e-4";
			reaction="The default value is taken";
			help= "Check the given absolute solver tolerance";
			info << "River Model number : " << this->RVNumber << endl;
            type=3;
			break;
		case 1://relative solver tolerance
			place.append("check_members(void)") ;
			reason="The relative solver tolerance have to be: 1e-15<= abs_tol <=1e-4";
			reaction="The default value is taken";
			help= "Check the given relative solver tolerance";
			info << "River Model number : " << this->RVNumber << endl;
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
Error Hyd_Param_RV::set_error(const int err_type){
	string place="Hyd_Param_RV::";
		string help;
		string reason;
		int type=0;
		bool fatal=false;
		stringstream info;
		Error msg;

	switch (err_type){
		case 0://minum numbers of profiles is 2
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
