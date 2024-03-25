//#include "Hyd_Parse_RV.h"
#include "Hyd_Headers_Precompiled.h"

//constructor
Hyd_Parse_RV::Hyd_Parse_RV(void){

	this->found_no_profiles=false;
	this->found_geometry_file=false;

	//count the memory
	Sys_Memory_Count::self()->add_mem(sizeof(Hyd_Parse_RV), _sys_system_modules::HYD_SYS);
}
//destructor
Hyd_Parse_RV::~Hyd_Parse_RV(void){
	//count the memory
	Sys_Memory_Count::self()->minus_mem(sizeof(Hyd_Parse_RV), _sys_system_modules::HYD_SYS);
}
//___________________
//public
//Parse for the global rivermodel keywords in the rivermodel parameter file and read in the parameters for the river model with the given index
void Hyd_Parse_RV::parse_rivermodel_params(const int rv_index){

	_hyd_keyword_file Key;
	word Command;
	bool found_flag=false;
	bool model_read_finished_flag=false;

	//find the position of the rivermodel with index
	while (this->GetLine(Command)){
		//look for a keyword
		Key = this->ParseNextKeyword(Command);
		stringstream buffer;
		buffer<<Command;
		//if the keword is found, read the first line
		if(Key == eBEGINRVMODEL){
			int rv_number=-1;
			buffer >> rv_number;
			if(buffer.fail()==true){
				ostringstream info;
				info << "Wrong input sequenze " << buffer.str() << endl;
				Error msg=this->set_error(7);
				msg.make_second_info(info.str());
				throw msg;
			}
		//check if the index and the model number is the same
		if(rv_number==rv_index){
			rv_params.RVNumber=rv_number;
			found_flag=true;
			// There could be some spaces in the model text
			char buffer[512];
			RemoveDelimiters(Command, buffer);
			rv_params.RVName=buffer;
			break;
		}
		}
		else if(Key ==eENDDESCRIPTION){
			break;
		}
	}
	if(found_flag==false){
		Error msg=this->set_error(1);
		ostringstream info;
		info << "Given rivernumber: " << rv_index << endl;
		msg.make_second_info(info.str());
		throw msg;	
	}

	//search for the specific input
	try{
		while (this->GetLine(Command) && model_read_finished_flag==false ){
			Key = ParseNextKeyword(Command);
			//check the keywords
			switch (Key){
				case eNOFPROF:	
					this->parse_number_profiles(Key, Command);
					break;
				case eGEOMETRYFILE:	
					this->parse_geometry_file(Key, Command);
					break;
				case eINSTATBOUNDFILE:
					this->parse_instat_boundary(Key, Command);
					break;
				case e1DOUTPUT:	
					this->parse_1d_result_file(Key, Command);
					break;
				case e2DOUTPUT:	
					this->parse_2d_result_file(Key, Command);
					break;
				case eLIMITS:
					this->parse_calculation_limits(Key, Command);
					break;
				case eENDRVMODEL:
					model_read_finished_flag=true;
					break;
				default:
					break;
				}
			}
			if(model_read_finished_flag==false){
				Error msg=this->set_error(2);
				throw msg;
			}
			this->check_parameters_found();
	}
	catch(Error msg){
		stringstream info;
		info << "Rivernumber: " << rv_index << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
	
}
//Get the globals river model parameters, which are read in per file parser
Hyd_Param_RV Hyd_Parse_RV::get_rivermodel_params(void)	{
	return this->rv_params;
}
//___________________
//private
//Parse for the number of profiles
void Hyd_Parse_RV::parse_number_profiles(_hyd_keyword_file Key, word Command){
	// There must not be a following keyword.
	if (strlen(Command) <= 0){
		this->GetLine(Command);
	}
	Key = ParseNextKeyword(Command);
	stringstream buffer;
	buffer<<Command;
	if (Key == eFAIL){
		buffer>>this->rv_params.RVNofProf;
		this->found_no_profiles=true;
	}
	if(buffer.fail()==true){
		ostringstream info;
		info << "Wrong input sequenze " << buffer.str() << endl;
		Error msg=this->set_error(5);
		msg.make_second_info(info.str());
		throw msg;
	}
}
//Parse for the geometry_file name
void Hyd_Parse_RV::parse_geometry_file(_hyd_keyword_file Key, word Command){
	// There is not a following keyword.
	if (strlen(Command) <= 0){
		this->GetLine(Command);
	}
	Key = ParseNextKeyword(Command);
	if (Key == eFAIL) {
		char buffer[512];
		RemoveDelimiters(Command, buffer); 
		UseLinuxSlash(buffer);
		this->rv_params.RVGeomtryFile=buffer;
		this->found_geometry_file=true;
	}
			
}
//Parse for the instationary_boundary_file name and number of curves
void Hyd_Parse_RV::parse_instat_boundary(_hyd_keyword_file Key, word Command){
	int must_found=0;
	// Next keyword must be SET
	do{
		if (Key == eFAIL){
			if (!this->GetLine(Command)){
				Error msg=this->set_error(0);
				throw msg;
			}
		}
		Key = ParseNextKeyword(Command);
	}while (Key != eSET);

	// Get general model information ...
	do{
		if (Key == eFAIL){
			if (!this->GetLine(Command)){
				Error msg=this->set_error(0);
				throw msg;
			}
		}
		Key = ParseNextKeyword(Command);
		stringstream buffer;
		buffer<<Command;
		if (Key == eFILENAME){
			char buffer_char[512];
			RemoveDelimiters(Command, buffer_char); 
			UseLinuxSlash(buffer_char);
			this->rv_params.inst_boundary_file_name=buffer_char;
			must_found++;
		}
		else if (Key == eNUMBER_CURVE){
			buffer>>this->rv_params.number_instat_boundary;
			must_found++;
		}
		if(buffer.fail()==true){
			ostringstream info;
			info << "Wrong input sequenze " << buffer.str() << endl;
			Error msg=this->set_error(6);
			msg.make_second_info(info.str());
			throw msg;
		}

	}while (Key != eSET); // End of Time specifications are marked by </Set>

	if(must_found!=2){
		ostringstream info;
		info << "Not all needed infos are found " <<  endl;
		Error msg=this->set_error(0);
		msg.make_second_info(info.str());
		throw msg;
	}

}
//Parse for the 1d_result_file name for tecplot
void Hyd_Parse_RV::parse_1d_result_file(_hyd_keyword_file Key, word Command){
	// There must not be a following keyword.
	if (strlen(Command) <= 0){
		this->GetLine(Command);
	}
	Key = ParseNextKeyword(Command);
	if (Key == eFAIL){
		char buffer[512];
		RemoveDelimiters(Command, buffer);
		UseLinuxSlash(buffer);
		string buff2=buffer;
		_Hyd_Parse_IO::erase_carriageReturn(&buff2);
		_Hyd_Parse_IO::erase_leading_whitespace_tabs(&buff2);
		_Hyd_Parse_IO::erase_end_whitespace_tabs(&buff2);
		//this->rv_params.tecplot_outfile_name_1d=buff2;
		ostringstream info;
		info << "See  !1DOUTPUT keyword:" << buffer << " in the RV-section" << endl;
		Warning msg = this->set_warning(1);
		msg.make_second_info(info.str());
		msg.output_msg(2);
	}
}
//Parse for the 2d_result_file name for tecplot
void Hyd_Parse_RV::parse_2d_result_file(_hyd_keyword_file Key, word Command){
	// There must not be a following keyword.
	if (strlen(Command) <= 0){
		this->GetLine(Command);
	}
	Key = ParseNextKeyword(Command);
	if (Key == eFAIL){
		char buffer[512];
		RemoveDelimiters(Command, buffer);
		UseLinuxSlash(buffer);
		string buff2=buffer;
		_Hyd_Parse_IO::erase_carriageReturn(&buff2);
		_Hyd_Parse_IO::erase_leading_whitespace_tabs(&buff2);
		_Hyd_Parse_IO::erase_end_whitespace_tabs(&buff2);
		//this->rv_params.tecplot_outfile_name_2d=buff2;
		ostringstream info;
		info << "See  !2DOUTPUT keyword:" << buffer << " in the RV-section" << endl;
		Warning msg = this->set_warning(1);
		msg.make_second_info(info.str());
		msg.output_msg(2);
	}
}
//Parse for the calculation limits
void Hyd_Parse_RV::parse_calculation_limits(_hyd_keyword_file Key, word Command){
	// Next keyword is STANDARD or SET
	do{
		if (Key == eFAIL){
			if (!this->GetLine(Command)){
				Error msg=this->set_error(4);
				throw msg;
			}
		}
		Key = ParseNextKeyword(Command);
	}while ((Key != eSTANDARD) && (Key != eSET));

	if(Key==eSTANDARD){
		return;
	}
	else if (Key == eSET){
		do{
			if (Key == eFAIL){
				if (!this->GetLine(Command)){
					Error msg=this->set_error(4);
					throw msg;
				}
			}
			Key = ParseNextKeyword(Command);
			stringstream buffer;
			buffer<<Command;
			if (Key == eATOL){
				buffer >> this->rv_params.abs_tolerance;
			}
			else if (Key == eRTOL){
				buffer>>this->rv_params.rel_tolerance;
			}
			else if(Key == ePROF_INTERFACE){
				string buff1;
				buffer >>buff1;
				this->rv_params.user_setting.use_interface=_Hyd_Parse_IO::transform_string2boolean(buff1);
			}
			else if(Key == eV_EXPLICIT){
				string buff1;
				buffer >>buff1;
				this->rv_params.user_setting.explict_v_is_used=_Hyd_Parse_IO::transform_string2boolean(buff1);
			}
			if(buffer.fail()==true){
				ostringstream info;
				info << "Wrong input sequenze " << buffer.str() << endl;
				Error msg=this->set_error(8);
				msg.make_second_info(info.str());
				throw msg;
			}

		}while (Key != eSET); // End of Init specifications are marked by </SET>
	}
}
//Chek if the requiered paramaters were found
void Hyd_Parse_RV::check_parameters_found(void){

	if(this->found_no_profiles==false){
		Error msg=this->set_error(3);
		throw msg;
	}
    if(this->found_geometry_file==false){
		Error msg=this->set_error(4);
		throw msg;
	}

}
//Set the warning
Warning Hyd_Parse_RV::set_warning(const int warn_type) {
	string place = "Hyd_Parse_RV::";
	string help;
	string reason;
	string reaction;
	int type = 0;
	Warning msg;
	stringstream info;

	switch (warn_type) {
	case 0://output settings are changed
		place.append("parse_1d_result_file(_hyd_keyword_file Key, word Command)");
		reason = "Keyword is not more supported; the output-settings are done now in the global parameter section";
		reaction = "No reaction";
		help = "Just erase the following line in the .ilm-file";
		type = 1;
		break;
	case 1://output settings are changed
		place.append("parse_2d_result_file(_hyd_keyword_file Key, word Command)");
		reason = "Keyword is not more supported; the output-settings are done now in the global parameter section";
		reaction = "No reaction";
		help = "Just erase the following line in the .ilm-file";
		type = 1;
		break;

	default:
		place.append("set_warning(const int warn_type)");
		reason = "Unknown flag!";
		help = "Check the flags";
		type = 5;
	}
	msg.set_msg(place, reason, help, reaction, type);
	msg.make_second_info(info.str());
	return msg;
}
//set the error
Error Hyd_Parse_RV::set_error(const int err_type){
		string place="Hyd_Parse_RV::";
		string help;
		string reason;
		int type=0;
		bool fatal=false;
		stringstream info;
		Error msg;

	switch (err_type){
		case 0://problem with instationary boundary settings
			place.append("parse_instat_boundary(_hyd_keyword_file Key, word Command)");
			reason="Can not read in properly the instationary boundary block";
			help="Check the instationary boundary block in the inputfile";
			info << "Filename: " << this->input_file_name <<endl;
			type=1;
			break;
		case 1://no river model is found to the given index
			place.append("parse_rivermodel_params(const int rv_index)");
			reason="There are no data corresponding to the given number of rivermodel";
			help="The river numbers have to be sequentially; starting by 0; check the file";
			info << "Filename: " << this->input_file_name <<endl;
			type=1;
			break;
		case 2://no end is found
			place.append("parse_rivermodel_params(const int rv_index)");
			reason="Do not find the end of the description of the rivermodel parameters; it has to end with !$ENDRVMODEL";
			help="Check the inputfile";
			info << "Filename: " << this->input_file_name <<endl;
			type=1;
			break;
		case 3://number of profiles is not found
			place.append("check_parameters_found(void)");
			reason="Did not find the number of profiles";
			help="Check the number of profiles setting in the river model description";
			info << "Filename: " << this->input_file_name <<endl;
			type=1;
			break;
		case 4://geometry file is not found
			place.append("check_parameters_found(void)");
			reason="Did not find the name of the geometry file";
			help="Check the name of the geometryfile in the river model description";
			info << "Filename: " << this->input_file_name <<endl;
			type=1;
			break;
		case 5://wrong input
			place.append("parse_number_profiles(_hyd_keyword_file Key, word Command)");
			reason="There is a problem with the input; wrong sign is read in";
			help="Check the number of profiles in the global rivermodel parameters in file";
			info << "Filename: " << this->input_file_name <<endl;
			info << "Error occurs near line: "<< this->line_counter << endl;
			type=1;
			break;
		case 6://wrong input
			place.append("parse_instat_boundary(_hyd_keyword_file Key, word Command)");
			reason="There is a problem with the input";
			help="Check the number of instationary boundary curves in the global rivermodel parameters in file";
			info << "Filename: " << this->input_file_name <<endl;
			info << "Error occurs near line: "<< this->line_counter << endl;
			type=1;
			break;
		case 7://wrong input
			place.append("parse_rivermodel_params(const int rv_index)");
			reason="There is a problem with the input; wrong sign is read in";
			help="Check the rivermodel number in the global rivermodel parameters in file";
			info << "Filename: " << this->input_file_name <<endl;
			info << "Error occurs near line: "<< this->line_counter << endl;
			type=1;
			break;
		case 8://wrong input
			place.append("parse_calculation_limits(_hyd_keyword_file Key, word Command)");
			reason="There is a problem with the input; wrong sign is read in";
			help="Check the calculation-limits settings in the global rivermodel parameters in file";
			info << "Filename: " << this->input_file_name <<endl;
			info << "Error occurs near line: "<< this->line_counter << endl;
			type=1;
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
