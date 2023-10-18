#include "Hyd_Headers_Precompiled.h"
//#include "Hyd_Parse_Coupling_Diversion.h"

//constructor
Hyd_Parse_Coupling_Diversion::Hyd_Parse_Coupling_Diversion(void){
	this->params.rv_inflow_no=-1;
	this->params.rv_model_no=-1;
	this->params.rv_outflow_no=-1;
	this->params.type=_hyd_1d_outflow_types::UNKNOWN_OUT;

	this->structure_param.width=0.0;
	this->structure_param.controlled_param=0.0;
	this->structure_param.uncontrollable_param=0.65;
	this->structure_param.controlled_flag=false;

	this->found_no_rvmodel=false;
	this->found_inflow=false;
	this->found_couplingstructure=false;

	//count the memory
	Sys_Memory_Count::self()->add_mem(sizeof(Hyd_Parse_Coupling_Diversion), _sys_system_modules::HYD_SYS);
}
//destructor
Hyd_Parse_Coupling_Diversion::~Hyd_Parse_Coupling_Diversion(void){
	//count the memory
	Sys_Memory_Count::self()->minus_mem(sizeof(Hyd_Parse_Coupling_Diversion), _sys_system_modules::HYD_SYS);
}
//____________
//public
//Parse for the diversion channel model the keywords and read in the parameters; the inex specifies the model number in the file 
void Hyd_Parse_Coupling_Diversion::parse_params(const int index){
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
		if(Key == eBEGINDVCHANNEL){
			int number=-1;
			buffer >> number;
			if(buffer.fail()==true){
				ostringstream info;
				info << "Wrong input sequenze " << buffer.str() << endl;
				Error msg=this->set_error(0);
				msg.make_second_info(info.str());
				throw msg;
			}
		//check if the index and the model number is the same
		if(number==index){
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
		info << "Given index of diversion channel coupling: " << index << endl;
		msg.make_second_info(info.str());
		throw msg;	
	}

	//search for the specific input
	try{
		while (this->GetLine(Command) && model_read_finished_flag==false ){
			Key = ParseNextKeyword(Command);
			//check the keywords
			switch (Key){
				case eNORIVER:	
					this->parse_no_river(Key, Command);
					break;
				case eINFLOW_MODEL:	
					this->parse_inflow_parameter(Key, Command);
					break;
				case eOUTFLOW_MODEL:
					this->parse_outflow_parameter(Key, Command);
					break;
				case eCOUPLING_STRUCT:	
					this->parse_coupling_structure_parameter(Key, Command);
					break;
				case eENDDVCHANNEL:
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
		info << "Diversion channel couling index: " << index << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
}
//Get the read in diversion channel parameters 
_hyd_div_channel_params Hyd_Parse_Coupling_Diversion::get_params(void){
	return this->params;
}
//Get the parameter of the coupling structure
_hyd_coupling_structure_data Hyd_Parse_Coupling_Diversion::get_coupling_structure_data(void){
	return this->structure_param;
}
//_____________
//private
//Parse for the number of the river model, which represents the diversion channel
void Hyd_Parse_Coupling_Diversion::parse_no_river(_hyd_keyword_file Key, word Command){
	// There must not be a following keyword.
	if (strlen(Command) <= 0){
		this->GetLine(Command);
	}
	Key = ParseNextKeyword(Command);
	stringstream buffer;
	buffer<<Command;
	if (Key == eFAIL){
		buffer>>this->params.rv_model_no;
		this->found_no_rvmodel=true;
	}
	if(buffer.fail()==true){
		ostringstream info;
		info << "Wrong input sequenze " << buffer.str() << endl;
		Error msg=this->set_error(3);
		msg.make_second_info(info.str());
		throw msg;
	}
}
//Parse for the inflow parameters 
void Hyd_Parse_Coupling_Diversion::parse_inflow_parameter(_hyd_keyword_file Key, word Command){
	int must_found=0;
	// Next keyword must be SET
	do{
		if (Key == eFAIL){
			if (!this->GetLine(Command)){
				Error msg=this->set_error(4);
				throw msg;
			}
		}
		Key = ParseNextKeyword(Command);
	}while (Key != eSET);

	// Get general model information ...
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
		if (Key == eMODEL_NO){
			buffer>>this->params.rv_inflow_no;
			must_found++;
		}
		else if (Key == eCOUPSTRUCT_TYPE){
			string buffer_txt;
			buffer>>buffer_txt;
			
			this->params.type=_Hyd_Coupling_Structure::convert_string2coupling_type(buffer_txt);
			if(this->params.type==_hyd_1d_outflow_types::UNKNOWN_OUT){
				Error msg=this->set_error(5);
				ostringstream info;
				info << "Given type of the coupling structure " << buffer_txt << endl;
				info << "Number of the diversion channel " << this->params.rv_model_no << endl;
				info << "Error occurs near line: "<< this->line_counter << endl;
				msg.make_second_info(info.str());
				throw msg;
			}
			must_found++;
		}

		if(buffer.fail()==true){
			ostringstream info;
			info << "Wrong input sequenze " << buffer.str() << endl;
			Error msg=this->set_error(4);
			msg.make_second_info(info.str());
			throw msg;
		}

	}while (Key != eSET); // End of inflow specifications are marked by </Set>

	if(must_found!=2){
		ostringstream info;
		info << "Not all needed infos are found " <<  endl;
		Error msg=this->set_error(4);
		msg.make_second_info(info.str());
		throw msg;
	}

	this->found_inflow=true;
}
//Parse for the parameters of the coupling structure
void Hyd_Parse_Coupling_Diversion::parse_coupling_structure_parameter(_hyd_keyword_file Key, word Command){
	int must_found=0;
	// Next keyword must be SET
	do{
		if (Key == eFAIL){
			if (!this->GetLine(Command)){
				Error msg=this->set_error(7);
				throw msg;
			}
		}
		Key = ParseNextKeyword(Command);
	}while (Key != eSET);

	do{
		if (Key == eFAIL){
			if (!this->GetLine(Command)){
				Error msg=this->set_error(7);
				throw msg;
			}
		}
		Key = ParseNextKeyword(Command);
		stringstream buffer;
		buffer<<Command;
		if (Key == eWIDTH){
			buffer>>this->structure_param.width;
			must_found++;
		}
		else if (Key == eCONTROLLABLE_PARAM){
			buffer>>this->structure_param.controlled_param;
			must_found++;
		}
		else if (Key == eUNCONTROLLABLE_PARAM){
			buffer>>this->structure_param.uncontrollable_param;
			must_found++;
		}
		else if (Key == eCONTROLLED){
			string buffer_txt;
			buffer>>buffer_txt;
			try{
				this->structure_param.controlled_flag=_Hyd_Parse_IO::transform_string2boolean(buffer_txt);
			}
			catch(Error msg){
				ostringstream info;
				info << "Filename: " << this->input_file_name <<endl;
				info << "Error occurs near line: "<< this->line_counter << endl;
				info << "Controlled-parameter of the coupling structure (RV2RV diversion) can not be read"<<endl;
				info << "Number of the diversion channel " << this->params.rv_model_no << endl;
				info << "Error occurs near line: "<< this->line_counter << endl;
				msg.make_second_info(info.str());
				throw msg;
			}
			must_found++;
		}
		if(buffer.fail()==true){
			ostringstream info;
			info << "Wrong input sequenze " << buffer.str() << endl;
			info << "Number of the diversion channel " << this->params.rv_model_no << endl;
			Error msg=this->set_error(7);
			msg.make_second_info(info.str());
			throw msg;
		}

	}while (Key != eSET); // End of outflow specifications are marked by </Set>

	if(must_found!=4){
		ostringstream info;
		info << "Not all needed infos are found " <<  endl;
		info << "Number of the diversion channel " << this->params.rv_model_no << endl;
		Error msg=this->set_error(7);
		msg.make_second_info(info.str());
		throw msg;
	}
	this->found_couplingstructure=true;
}
//Parse for the outflow parameters 
void Hyd_Parse_Coupling_Diversion::parse_outflow_parameter(_hyd_keyword_file Key, word Command){

	int must_found=0;
	// Next keyword must be SET
	do{
		if (Key == eFAIL){
			if (!this->GetLine(Command)){
				Error msg=this->set_error(6);
				throw msg;
			}
		}
		Key = ParseNextKeyword(Command);
	}while (Key != eSET);

	do{
		if (Key == eFAIL){
			if (!this->GetLine(Command)){
				Error msg=this->set_error(6);
				throw msg;
			}
		}
		Key = ParseNextKeyword(Command);
		stringstream buffer;
		buffer<<Command;
		if (Key == eMODEL_NO){
			buffer>>this->params.rv_outflow_no;
			must_found++;
		}
		if(buffer.fail()==true){
			ostringstream info;
			info << "Wrong input sequenze " << buffer.str() << endl;
			info << "Number of the diversion channel " << this->params.rv_model_no << endl;
			Error msg=this->set_error(6);
			msg.make_second_info(info.str());
			throw msg;
		}
	}while (Key != eSET); // End of outflow specifications are marked by </Set>

	if(must_found!=1){
		ostringstream info;
		info << "Not all needed infos are found " <<  endl;
		info << "Number of the diversion channel " << this->params.rv_model_no << endl;
		Error msg=this->set_error(6);
		msg.make_second_info(info.str());
		throw msg;
	}
}
//Check if the requiered paramaters were found
void Hyd_Parse_Coupling_Diversion::check_parameters_found(void){
	if(this->found_no_rvmodel==false){
		Error msg=this->set_error(8);
		throw msg;
	}
	if(this->found_inflow==false){
		Error msg=this->set_error(9);
		ostringstream info;
		info << "Number of the diversion channel " << this->params.rv_model_no << endl;
		msg.make_second_info(info.str());
		throw msg;

	}
	if(this->found_couplingstructure==false){
		Error msg=this->set_error(10);
		ostringstream info;
		info << "Number of the diversion channel " << this->params.rv_model_no << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
}
//Set error(s)
Error Hyd_Parse_Coupling_Diversion::set_error(const int err_type){
	string place="Hyd_Parse_Coupling_Diversion::";
	string help;
	string reason;
	int type=0;
	bool fatal=false;
	stringstream info;
	Error msg;

	switch (err_type){
		case 0://wrong input
			place.append("parse_params(const int index)");
			reason="There is a problem with the input; wrong sign is read in";
			help="Check the diversion channel number in the file";
			info << "Filename: " << this->input_file_name <<endl;
			type=1;
			break;
		case 1://no diversion channel coupling is found to the given index
			place.append("parse_params(const int index)");
			reason="There are no data corresponding to the given index of diversion channel coupling";
			help="The coupling indices have to be sequentially; starting by 0; check the file";
			info << "Filename: " << this->input_file_name <<endl;
			type=1;
			break;
		case 2://no end is found
			place.append("parse_params(const int index)");
			reason="Do not find the end of the description of the diversion channel coupling; it has to end with !$ENDDVCHANNEL";
			help="Check the inputfile";
			info << "Filename: " << this->input_file_name <<endl;
			type=1;
			break;
		case 3://wrong input
			place.append("parse_no_river(_hyd_keyword_file Key, word Command)");
			reason="There is a problem with the input; wrong sign is read in";
			help="Check the number of river model in the diversion channel coupling settings in file";
			info << "Filename: " << this->input_file_name <<endl;
			info << "Error occurs near line: "<< this->line_counter << endl;
			type=1;
			break;
		case 4://problem with inflow setting parameter settings
			place.append("parse_inflow_parameter(_hyd_keyword_file Key, word Command)");
			reason="Can not read in properly the inflow parameter block";
			help="Check the inflow parameter block in the inputfile";
			info << "Filename: " << this->input_file_name <<endl;
			info << "Error occurs near line: "<< this->line_counter << endl;
			type=1;
			break;
		case 5://can not convert the given type of the coupling structure
			place.append("parse_inflow_parameter(_hyd_keyword_file Key, word Command)");
			reason="Can not convert the given type of the coupling structure";
			help="Check the given type";
			info << "Filename: " << this->input_file_name <<endl;
			info << "Error occurs near line: "<< this->line_counter << endl;
			type=1;
			break;
		case 6://problem with outflow setting parameter 
			place.append("parse_outflow_parameter(_hyd_keyword_file Key, word Command)");
			reason="Can not read in properly the outflow parameter block";
			help="Check the outflow parameter block in the inputfile";
			info << "Filename: " << this->input_file_name <<endl;
			info << "Error occurs near line: "<< this->line_counter << endl;
			type=1;
			break;
		case 7://problem with coupling structure settings 
			place.append("parse_coupling_structure_parameter(_hyd_keyword_file Key, word Command)");
			reason="Can not read in properly the coupling structure parameter block";
			help="Check the coupling structure parameter block in the inputfile";
			info << "Filename: " << this->input_file_name <<endl;
			info << "Error occurs near line: "<< this->line_counter << endl;
			type=1;
			break;
		case 8://river index for the diversion channel is not found
			place.append("check_parameters_found(void)");
			reason="Did not find the index of the river, which represents the diversion channel";
			help="Check the river index of the diversion channel";
			info << "Filename: " << this->input_file_name <<endl;
			type=1;
			break;
		case 9://inflow parameter settings are not found
			place.append("check_parameters_found(void)");
			reason="Did not find the inflow parameter settings";
			help="Check the inflow parameter settings";
			info << "Filename: " << this->input_file_name <<endl;
			type=1;
			break;
		case 10://coupling structure parameter settings are not found
			place.append("check_parameters_found(void)");
			reason="Did not find the coupling structure parameter settings";
			help="Check the coupling structure parameter settings";
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