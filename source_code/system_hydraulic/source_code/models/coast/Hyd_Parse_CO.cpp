#include "Hyd_Headers_Precompiled.h"
//#include "Hyd_Parse_CO.h"

//constructor
Hyd_Parse_CO::Hyd_Parse_CO(void){
	this->params.filename_coastline=label::not_set;
	this->params.filename_instat_boundary=label::not_set;
	this->params.model_name=label::not_set;
	this->params.filename_geometry_tecplot=label::not_set;

	//count the memory
	Sys_Memory_Count::self()->add_mem(sizeof(Hyd_Parse_CO), _sys_system_modules::HYD_SYS);
}
//destructor
Hyd_Parse_CO::~Hyd_Parse_CO(void){
	//count the memory
	Sys_Memory_Count::self()->minus_mem(sizeof(Hyd_Parse_CO), _sys_system_modules::HYD_SYS);
}
//_____________
//public
//Parse for the global coastmodel keywords and read in the parameters for the coastmodel
void Hyd_Parse_CO::parse_coastmodel_params(void){

	_hyd_keyword_file Key;
	word Command;
	bool model_read_finished_flag=false;

	//find the begin of floodplain model with the given index
	while (this->GetLine(Command)){
		 //look for a keyword
		 Key = this->ParseNextKeyword(Command);
		 stringstream buffer;
		 buffer<<Command;
		 //if the keword is found, read the first line
		 if(Key == eBEGINCOASTMODEL){
			// There could be some spaces in the model text
			char buffer[512];
			RemoveDelimiters(Command, buffer);
			this->params.model_name=buffer;
			break;
		 }
		 else if(Key ==eENDDESCRIPTION){
			break;
		}
	}

	//search for the specific input
	try{
		while(this->GetLine(Command) && model_read_finished_flag==false ){
				Key = ParseNextKeyword(Command);
			//check the keywords
			switch (Key){
				case eCOASTLINEFILE:
					this->parse_coastline(Key, Command);
					break;
				case eINSTATBOUNDFILE:
					this->parse_instat_boundary(Key, Command);
					break;
				case e2DOUTPUT:
					this->parse_geometry_file_tecplot(Key, Command);
					break;
				case eENDCOASTMODEL:
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

	}
	catch(Error msg){
		stringstream info;
		info << "Error in the Coastmodel " <<  endl;
		msg.make_second_info(info.str());
		throw msg;
	}
}
//Get the read in coast parameter (_hyd_coast_params)
_hyd_coast_params Hyd_Parse_CO::get_coastmodel_params(void){
	return this->params;
}
//___________________
//private
//Parse for the name of the coastline file name
void Hyd_Parse_CO::parse_coastline(_hyd_keyword_file Key, word Command){
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
				Error msg=this->set_error(1);
				throw msg;
			}
		}
		Key = ParseNextKeyword(Command);
		stringstream buffer;
		buffer<<Command;
		if (Key == eFILENAME){
			char buffer[512];
			RemoveDelimiters(Command, buffer); 
			UseLinuxSlash(buffer);
			this->params.filename_coastline=buffer;
			must_found++;

		}
		if(buffer.fail()==true){
			ostringstream info;
			info << "Wrong input sequenze " << buffer.str() << endl;
			Error msg=this->set_error(4);
			msg.make_second_info(info.str());
			throw msg;
		}

	}while (Key != eSET); // End of Time specifications are marked by </Set>

	if(must_found!=1){
		ostringstream info;
		info << "Not all needed infos are found " <<  endl;
		Error msg=this->set_error(4);
		msg.make_second_info(info.str());
		throw msg;
	}
}
//Parse for the instationary_boundary_file name
void Hyd_Parse_CO::parse_instat_boundary(_hyd_keyword_file Key, word Command){
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
			char buffer[512];
			RemoveDelimiters(Command, buffer); 
			UseLinuxSlash(buffer);
			this->params.filename_instat_boundary=buffer;
			must_found++;

		}
		if(buffer.fail()==true){
			ostringstream info;
			info << "Wrong input sequenze " << buffer.str() << endl;
			Error msg=this->set_error(3);
			msg.make_second_info(info.str());
			throw msg;
		}

	}while (Key != eSET); // End of Time specifications are marked by </Set>

	if(must_found!=1){
		ostringstream info;
		info << "Not all needed infos are found " <<  endl;
		Error msg=this->set_error(3);
		msg.make_second_info(info.str());
		throw msg;
	}
}
//Parse for the geometry_file name
void Hyd_Parse_CO::parse_geometry_file_tecplot(_hyd_keyword_file Key, word Command){
		// There must not be a following keyword.
	if (strlen(Command) <= 0){
		this->GetLine(Command);
	}
	Key = ParseNextKeyword(Command);
	if (Key == eFAIL){
		char buffer[512];
		RemoveDelimiters(Command, buffer);
		UseLinuxSlash(buffer);
		//this->params.filename_geometry_tecplot=buffer;
		ostringstream info;
		info << "See  !2DOUTPUT keyword:" << buffer << " in the CO-section" << endl;
		Warning msg = this->set_warning(0);
		msg.make_second_info(info.str());
		msg.output_msg(2);
	}
}
//Check if the requiered paramaters were found
void Hyd_Parse_CO::check_parameters_found(void){

}
//Set the warning
Warning Hyd_Parse_CO::set_warning(const int warn_type) {
	string place = "Hyd_Parse_CO::";
	string help;
	string reason;
	string reaction;
	int type = 0;
	Warning msg;
	stringstream info;

	switch (warn_type) {
	case 0://output settings are changed
		place.append("parse_geometry_file_tecplot(_hyd_keyword_file Key, word Command)");
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
Error Hyd_Parse_CO::set_error(const int err_type){
		string place="Hyd_Parse_CO::";
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
		case 1://problem with coastline settings
			place.append("parse_coastline(_hyd_keyword_file Key, word Command)");
			reason="Can not read in properly the coastline block";
			help="Check the coastline boundary block in the inputfile";
			info << "Filename: " << this->input_file_name <<endl;
			type=1;
			break;
		case 2://no end is found
			place.append("parse_coastmodel_params");
			reason="Do not find the end of the description of the coastmodel parameters; it has to end with !$ENDCOASTMODEL";
			help="Check the inputfile";
			info << "Filename: " << this->input_file_name <<endl;
			type=1;
			break;
		case 3://wrong input
			place.append("parse_instat_boundary(_hyd_keyword_file Key, word Command)");
			reason="There is a problem with the input";
			help="Check the instationary boundary curves settings in file";
			info << "Filename: " << this->input_file_name <<endl;
			type=1;
			break;
		case 4://wrong input
			place.append("parse_coastline(_hyd_keyword_file Key, word Command)");
			reason="There is a problem with the input";
			help="Check the coastline settings in file";
			info << "Filename: " << this->input_file_name <<endl;
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