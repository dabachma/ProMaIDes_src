#include "Hyd_Headers_Precompiled.h"
//#include "Hyd_Parse_Coupling_RV2FP_Dikebreak.h"

//constructor
Hyd_Parse_Coupling_RV2FP_Dikebreak::Hyd_Parse_Coupling_RV2FP_Dikebreak(void){
	this->params.begin_waterlevel=0.0;
	this->params.distance=0.0;
	this->params.left_flag=false;
	this->params.profile_index=-1;
	this->params.rv_model_no=-1;
	this->params.max_breach_width=-1.0;

	this->found_location=false;

	//count the memory
	Sys_Memory_Count::self()->add_mem(sizeof(Hyd_Parse_Coupling_RV2FP_Dikebreak), _sys_system_modules::HYD_SYS);
}
//destructor
Hyd_Parse_Coupling_RV2FP_Dikebreak::~Hyd_Parse_Coupling_RV2FP_Dikebreak(void){
	//count the memory
	Sys_Memory_Count::self()->minus_mem(sizeof(Hyd_Parse_Coupling_RV2FP_Dikebreak), _sys_system_modules::HYD_SYS);
}
//______________
//public
//Parse for the coupling parameters for a coupling of a river- and floodplain-model via a dikebreak; the index specifies number in the file 
void Hyd_Parse_Coupling_RV2FP_Dikebreak::parse_params(const int index){
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
		if(Key == eBEGINDIKEBREAKCOUPLING){
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
		throw msg;	
	}

	//search for the specific input
	try{
		while (this->GetLine(Command) && model_read_finished_flag==false ){
			Key = ParseNextKeyword(Command);
			//check the keywords
			switch (Key){
				case eDIKEBREAK_LOCATION:
					this->parse_location_parameter(Key, Command);
					break;
				case eENDDIKEBREAKCOUPLING:
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
		throw msg;
	}
}
//Get the read in coupling parameters for a coupling of a river- and floodplain-model via a dikebreak
_hyd_rv2fp_dikebreak_coupling_params Hyd_Parse_Coupling_RV2FP_Dikebreak::get_params(void){
	return this->params;
}
//______________
//private
//Parse for the location of the breach in the river model
void Hyd_Parse_Coupling_RV2FP_Dikebreak::parse_location_parameter(_hyd_keyword_file Key, word Command){
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
			buffer>>this->params.rv_model_no;
			must_found++;
		}
		else if(Key== eDISTANCE){
			buffer>>this->params.distance;
			must_found++;
		}
		else if(Key== eLEFTDIRECTION){
			try{
				this->params.left_flag=_Hyd_Parse_IO::transform_string2boolean(buffer.str());
			}
			catch(Error msg){
				ostringstream info;
				info << "Try to convert the direction of the dikebreak coupling into a boolean" << endl;
				info << "Error occurs near line: "<< this->line_counter << endl;
				msg.make_second_info(info.str());
				throw msg;
			}
			must_found++;
		}
		else if (Key == ePROFILE_NO){
			buffer>>this->params.profile_index;
			must_found++;
		}
		else if(Key == eSTART_WATERH){
			buffer>>this->params.begin_waterlevel;
			must_found++;
		}
		else if(Key == eMAX_BREACHWIDTH){//optional
			buffer>>this->params.max_breach_width;
			must_found++;
		}


		if(buffer.fail()==true){
			ostringstream info;
			info << "Wrong input sequenze " << buffer.str() << endl;
			Error msg=this->set_error(3);
			msg.make_second_info(info.str());
			throw msg;
		}
	}while (Key != eSET); // End of outflow specifications are marked by </Set>

	if(must_found!=6){
		ostringstream info;
		info << "Not all needed infos are found " <<  endl;
		Error msg=this->set_error(3);
		msg.make_second_info(info.str());
		throw msg;
	}

	this->found_location=true;
}
//Check if the requiered paramaters were found
void Hyd_Parse_Coupling_RV2FP_Dikebreak::check_parameters_found(void){
	if(this->found_location==false){
		Error msg=this->set_error(4);
		throw msg;
	}
}
//Set error(s)
Error Hyd_Parse_Coupling_RV2FP_Dikebreak::set_error(const int err_type){
	string place="Hyd_Parse_Coupling_RV2FP_Dikebreak::";
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
			help="Check the RV2FP-dikebreak coupling in file";
			info << "Filename: " << this->input_file_name <<endl;
			info << "Error occurs near line: "<< this->line_counter << endl;
			type=1;
			break;
		case 1://No RV2FP dikebreak coupling in file coupling is found to the given index
			place.append("parse_params(const int index)");
			reason="There are no data corresponding to the given index of RV2FP-dikebreak coupling in file";
			help="The coupling indices have to be sequentially; starting by 0; check the file";
			info << "Filename: " << this->input_file_name <<endl;
			type=1;
			break;
		case 3://problem with location settings
			place.append("parse_location_parameter(_hyd_keyword_file Key, word Command)");
			reason="Can not read in properly the location parameter block";
			help="Check the location parameter block in the inputfile";
			info << "Filename: " << this->input_file_name <<endl;
			info << "Error occurs near line: "<< this->line_counter << endl;
			type=1;
			break;
		case 4://loactaion parameters are not found
			place.append("check_parameters_found(void)");
			reason="Did not find the location parameter settings";
			help="Check the location parameter settings";
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