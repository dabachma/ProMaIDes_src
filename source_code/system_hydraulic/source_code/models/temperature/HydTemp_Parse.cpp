//#include "HydTemp_Parse.h"
#include "Hyd_Headers_Precompiled.h"

//constructor
HydTemp_Parse::HydTemp_Parse(void){

	
	this->found_temp_profile_data_file = false;

	//count the memory
	Sys_Memory_Count::self()->add_mem(sizeof(HydTemp_Parse), _sys_system_modules::HYD_SYS);
}
//destructor
HydTemp_Parse::~HydTemp_Parse(void){
	//count the memory
	Sys_Memory_Count::self()->minus_mem(sizeof(HydTemp_Parse), _sys_system_modules::HYD_SYS);
}
//___________________
//public
//Parse for the global temperature model keywords in the rivermodel parameter file and read in the parameters for the river model with the given index
void HydTemp_Parse::parse_tempmodel_params(const int rv_index){

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
			found_flag=true;	
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
				case eTEMP_INFO:
					this->parse_temp_infos(Key, Command);
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
//Get the globals temperature model parameters, which are read in per file parser
HydTemp_Param HydTemp_Parse::get_tempmodel_params(void)	{
	return this->temp_params;
}
//___________________
//private
//Parse for the calculation limits
void HydTemp_Parse::parse_temp_infos(_hyd_keyword_file Key, word Command){
	// Next keyword is STANDARD or SET
	do{
		if (Key == eFAIL){
			if (!this->GetLine(Command)){
				Error msg=this->set_error(4);
				throw msg;
			}
		}
		Key = ParseNextKeyword(Command);
	}while ((Key != eSET));


	if (Key == eSET){
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
			if (Key == eFILENAME){
				char buff1[512];
				RemoveDelimiters(Command, buff1);
				UseLinuxSlash(buff1);
				this->temp_params.temp_profile_File = buff1;
				this->found_temp_profile_data_file = true;

				
			}
			else if (Key == eGW_TEMP){
				buffer>> this->temp_params.gw_temp;
			}
			else if (Key == eBRUNT) {
				buffer >> this->temp_params.brunt_coef;
			}
			else if (Key == eVIEW2SKY) {
				buffer >> this->temp_params.view2sky_coef;
			}
			else if (Key == eHEAT_COND) {
				buffer >> this->temp_params.con_bed_coef;
			}
			else if (Key == eBED_TEMP) {
				buffer >> this->temp_params.bed_temp;
			}
			else if (Key == eBED_WARM ) {
				buffer >> this->temp_params.bed_warming_coeff;
			}
			else if (Key == eSOLAR_RAD ) {
				buffer >> this->temp_params.diffusiv_sol_rad_coeff;
			}


			if(buffer.fail()==true){
				ostringstream info;
				info << "Wrong input sequenze " << buffer.str() << endl;
				Error msg=this->set_error(6);
				msg.make_second_info(info.str());
				throw msg;
			}

		}while (Key != eSET); // End of Init specifications are marked by </SET>
	}
	else {
		Error msg = this->set_error(0);
		throw msg;

	}


}
//Chek if the requiered paramaters were found
void HydTemp_Parse::check_parameters_found(void){


   if(found_temp_profile_data_file ==false){
	   Warning msg = this->set_warning(0);
	   msg.output_msg(2);
	}

}
//Set the warning
Warning HydTemp_Parse::set_warning(const int warn_type) {
	string place = "HydTemp_Parse::";
	string help;
	string reason;
	string reaction;
	int type = 0;
	Warning msg;
	stringstream info;

	switch (warn_type) {
	case 0://nbo temp model found
		place.append("check_parameters_found(void)");
		reason = "Did not find the name of the temperature profile file";
		reaction = "No temperature model for this river is applied";
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
Error HydTemp_Parse::set_error(const int err_type){
		string place="HydTemp_Parse::";
		string help;
		string reason;
		int type=0;
		bool fatal=false;
		stringstream info;
		Error msg;

	switch (err_type){
		case 0://no temperature model info found 
			place.append("parse_temp_infos(_hyd_keyword_file Key, word Command)");
			reason = "There is no data for the tmperature model found";
			help = "The section needs to start with |SET> and end with </SET>";
			info << "Filename: " << this->input_file_name << endl;
			type = 1;
			break;
		case 1://no river model is found to the given index
			place.append("parse_tempmodel_params(const int rv_index)");
			reason="There are no data corresponding to the given number of rivermodel";
			help="The river numbers have to be sequentially; starting by 0; check the file";
			info << "Filename: " << this->input_file_name <<endl;
			type=1;
			break;
		case 2://no end is found
			place.append("parse_tempmodel_params(const int rv_index)");
			reason="Do not find the end of the description of the rivermodel parameters; it has to end with !$ENDRVMODEL";
			help="Check the inputfile";
			info << "Filename: " << this->input_file_name <<endl;
			type=1;
			break;
		case 6://wrong input
			place.append("parse_temp_infos(_hyd_keyword_file Key, word Command)");
			reason="There is a problem with the input";
			help="Check the temperature model data in the global rivermodel parameters in file";
			info << "Filename: " << this->input_file_name <<endl;
			info << "Error occurs near line: "<< this->line_counter << endl;
			type=1;
			break;
		case 7://wrong input
			place.append("parse_tempmodel_params(const int rv_index)");
			reason="There is a problem with the input; wrong sign is read in";
			help="Check the rivermodel number in the global rivermodel parameters in file";
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
