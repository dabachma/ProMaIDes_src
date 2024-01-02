//#include "Hyd_Parse_Glob.h"
#include "Hyd_Headers_Precompiled.h"


//constructor
Hyd_Parse_Glob::Hyd_Parse_Glob(void){
		
	this->found_no_river=false;
	this->found_no_floodplain=false;
	this->found_no_int_timesteps=false;
	this->found_no_timesteps=false;
	this->found_timestep=false;
	this->found_starttime=false;
	this->found_material_file=false;

	//count the memory
	Sys_Memory_Count::self()->add_mem(sizeof(Hyd_Parse_Glob), _sys_system_modules::HYD_SYS);
}
//destructor
Hyd_Parse_Glob::~Hyd_Parse_Glob(void){
	//count the memory
	Sys_Memory_Count::self()->minus_mem(sizeof(Hyd_Parse_Glob), _sys_system_modules::HYD_SYS);
}
//____________________
//public
//check if the input file is valid
void Hyd_Parse_Glob::check_valid_file(void){

	_hyd_keyword_file Key;
	word Command;
	bool begin_found=false;
	bool end_found =false;
	int line_count=0;
	
	// Test, if input file starts with BEGINDESCRIPTION and ends with ENDDESCRIPTION
	while ( (GetLine(Command)) && (Key = ParseNextKeyword(Command)) && (Key != eBEGINDESCRIPTION));
	if (Key == eBEGINDESCRIPTION){
		begin_found=true;
		do{
			this->GetLine(Command);
			Key = this->ParseNextKeyword(Command);
			if (Key == eENDDESCRIPTION){
				end_found =true;
				break;
			}
		}
		while(this->input_file.eof()!=true);
	}
	if(begin_found==false){
		Error msg=this->set_error(0);
		throw msg;
	}
	if(end_found==false){
		Error msg=this->set_error(1);
		throw msg;
	}

	//rewind the file
	this->Rewind();

	// Check whole syntax and report all no valid commands
	int count = 0;
	stringstream novalid_commands;
	novalid_commands << "No valid command(s): " << endl;
	while (this->GetLine(Command)){
		Key = ParseNextKeyword(Command);
		if(Key == eENDDESCRIPTION){
			break;
		}
		if (Key == eSKIP) {
			count++;
			novalid_commands << " " << Command << " in line " << line_count << endl; 			
		}
		line_count++;
	}
	if (count > 0){
		Warning msg=this->set_warning(0);
		stringstream info;
		info <<"Numbers of no valid Commands: " << count << endl;
		info << novalid_commands.str();
		msg.make_second_info(info.str());
		msg.output_msg(2);
	}	
	

	//rewind the file
	this->Rewind();
	line_count=0;

	string line;

	//check if there are text in it, which is not a comment, empty line or a keyword
	while (this->input_file.eof()==false){
		getline(this->input_file, line, '\n');
		_Hyd_Parse_IO::erase_carriageReturn(&line);
		_Hyd_Parse_IO::erase_leading_whitespace_tabs(&line);
		_Hyd_Parse_IO::erase_comment(&line);
		
		
		if(line.empty()!=true){
			strcpy(Command, line.c_str());
			Key = ParseNextKeyword(Command);
			if(Key == eFAIL){
				Error msg=this->set_error(21);
				ostringstream info;
				info <<"Error occured in line number: " << line_count << endl;
				info <<"Found invalid text is: " << line << endl;
				msg.make_second_info(info.str());
				throw msg;

			}
		}
		line_count++;
	}
	//rewind the file
	this->Rewind();
	line_count=0;
	begin_found=false;
	end_found=false;
	string begin_command;
	string part_begin_command;
	string part_end_command;
	//check if the begin blocks are closed with the corresponding end blocks
	while (this->input_file.eof()==false){
		getline(this->input_file, line, '\n');
		_Hyd_Parse_IO::erase_carriageReturn(&line);
		_Hyd_Parse_IO::erase_leading_whitespace_tabs(&line);
		_Hyd_Parse_IO::erase_comment(&line);

		if(line.empty()!=true){
			_Hyd_Parse_IO::string2lower(&line);
			int pos=line.find("!$begindescription");
			pos=line.find("!$begin");
			pos=line.find("!$enddescription");
			pos=line.find("!$end");
			if(line.find("!$begindescription")==string::npos && line.find("!$begin")!=string::npos){
				if(begin_found==false){
					begin_command=line;
					begin_found=true;
					//extract the part command
					int pos=line.find("!$begin");
					line.erase(pos, 7);
					if(line.find("=")!=string::npos){
						pos=line.find("=");
						line=line.substr(0,pos);
					}
					_Hyd_Parse_IO::erase_end_whitespace_tabs(&line);
					part_begin_command=line;


				}
				else if(begin_found==true ){
					Error msg=this->set_error(22);
					ostringstream info;
					info<< "Error near line: " << line_count << endl;
					info<< "Not closed begin-command " << begin_command << endl;
					msg.make_second_info(info.str());
					throw msg;
				}
			}
			else if(line.find("!$enddescription")==string::npos && line.find("!$end")!=string::npos){
				if(begin_found!=true){
					Error msg=this->set_error(23);
					ostringstream info;
					info<< "Error near line: " << line_count << endl;
					info<< "Not opened end-command " << line << endl;
					msg.make_second_info(info.str());
					throw msg;		
				}
				else if(begin_found==true){

					//check if the part commands are corresponding
					//extract the part command
					int pos=line.find("!$end");
					line.erase(pos, 5);
					_Hyd_Parse_IO::erase_end_whitespace_tabs(&line);
					part_end_command=line;

					if(part_end_command!=part_begin_command){
						Error msg=this->set_error(24);
						ostringstream info;
						info<< "Error near line: " << line_count << endl;
						info<< "No corresponding end command to  " << begin_command << endl;
						info << "Found end-command is !$end"<<part_end_command << endl;
						msg.make_second_info(info.str());
						throw msg;	
					}

					begin_found=false;
					begin_command.clear();
					part_end_command.clear();
					part_end_command.clear();
				}


			}
		}
		line_count++;
	}
	//rewind the file
	this->Rewind();
	line_count=0;

}
//parse for the global keyword in the global parameter file
void Hyd_Parse_Glob::parse_global_params(void){
	_hyd_keyword_file Key;
	word Command;
	bool begin_found=false;
	bool read_global_finished=false;

		
	//search for the begin
	while ((GetLine(Command))){
		Key = ParseNextKeyword(Command);
		if (Key == eBEGINGLOBAL){
			begin_found=true;
			break;
		}
	}

	if(begin_found==false){
		Error msg=this->set_error(2);
		throw msg;
	}

	//read in the global parameters
	try{
		while ((this->GetLine(Command)) && read_global_finished==false){ 
			Key = ParseNextKeyword(Command); 

			// switch through possible cases
			switch (Key){
				case eTIME:
					this->parse_time_setting(Key, Command);
					break;
				case eNOFFP:
					this->parse_floodplain_number(Key, Command);
					break;
				case eNOFRV:
					this->parse_river_number(Key, Command);
					break;
				case eNOFDC:
					this->parse_diversion_channel_number(Key, Command);
					break;
				case eNOFSC:
					this->parse_structure_coupling_number(Key, Command);
					break;
				case eNOFDBC:
					this->parse_dikebreak_rv2fp_coupling_number(Key, Command);
					break;
				case eNOFFP2COBREAK:
					this->parse_dikebreak_fp2co_coupling_number(Key, Command);
					break;
				case eCOASTMODEL:
					this->parse_coastmodel(Key, Command);
					break;
				case eTEMPMODEL:
					this->parse_tempmodel(Key, Command);
					break;
				case eINTEGRATION:
					this->parse_integration_setting(Key, Command);
					break;
				case eOUTPUT:
					this->parse_output_setting(Key, Command);
					break;
				case ePRECON:
					this->parse_precon_setting(Key, Command);
					break;
				case eSYNCRON:
					this->parse_sync_setting(Key, Command);
					break;
				case eMATERIALFILE:	
					this->parse_material_file(Key, Command);
					break;
				case eOBSPOINTFILE:
					this->parse_obs_point_file(Key, Command);
					break;
				case eENDGLOBAL: //end is found
					read_global_finished=true;
					break;
				default:
					break;
			}
		}
		if(read_global_finished==false){
			Error msg=this->set_error(13);
			throw msg;
		}

		this->check_parameters_found();
	}
	catch(Error msg){
		throw msg;
	}

}
//get the globals parameter which are read in
Hyd_Param_Global Hyd_Parse_Glob::get_global_params(void){
	return this->Globals;
}
//________________
//private
//parse time settings
void Hyd_Parse_Glob::parse_time_setting(_hyd_keyword_file Key, word Command){
	
	// Next keyword must be SET
	do{
		if (Key == eFAIL){
			if (!this->GetLine(Command)){
				Error msg=this->set_error(3);
				throw msg;
			}
		}
		Key = ParseNextKeyword(Command);
	}while (Key != eSET);

	// Get specifications for Timesteps...
	do{
		if (Key == eFAIL){
			if (!this->GetLine(Command)){
				Error msg=this->set_error(3);
				throw msg;
			}
		}

		Key = ParseNextKeyword(Command);
		stringstream buffer;
		buffer<<Command;
		if (Key == eTSTART){
			buffer >> this->Globals.GlobTStart;
			this->found_starttime=true;
		}
		else if(Key == eTSTEP){
			 buffer >> this->Globals.GlobTStep;
			 this->found_timestep=true;
		}
		else if(Key == eTNOF){
			 buffer >> this->Globals.GlobTNof;
			 this->found_no_timesteps=true;
		}
		else if(Key == eNOFITS){
			 buffer >> this->Globals.GlobNofITS;
			 this->found_no_int_timesteps=true;
		}
		if(buffer.fail()==true){
			ostringstream info;
			info << "Wrong input sequenze " << buffer.str() << endl;
			Error msg=this->set_error(14);
			msg.make_second_info(info.str());
			throw msg;
		}
	}while (Key != eSET); // End of Time specifications are marked by </Set>
}
//parse floodplain number
void Hyd_Parse_Glob::parse_floodplain_number(_hyd_keyword_file Key, word Command){
	// There is not a following keyword.
	if (strlen(Command) <= 0){
		this->GetLine(Command);
	}
	Key = ParseNextKeyword(Command);
	stringstream buffer;
	buffer<<Command;
	if (Key == eFAIL){
		buffer >> this->Globals.GlobNofFP;
		this->found_no_floodplain=true;
	}
	if(buffer.fail()==true){
		ostringstream info;
		info << "Wrong input sequenze " << buffer.str() << endl;
		Error msg=this->set_error(15);
		msg.make_second_info(info.str());
		throw msg;
	}
}
//parse river number
void Hyd_Parse_Glob::parse_river_number(_hyd_keyword_file Key, word Command){
	// There is not a following keyword.
	if (strlen(Command) <= 0){
		this->GetLine(Command);
	}
	Key = ParseNextKeyword(Command);
	stringstream buffer;
	buffer<<Command;
	if (Key == eFAIL){
		buffer >> this->Globals.GlobNofRV;
		this->found_no_river=true;
	}
	if(buffer.fail()==true){
		ostringstream info;
		info << "Wrong input sequenze " << buffer.str() << endl;
		Error msg=this->set_error(16);
		msg.make_second_info(info.str());
		throw msg;
	}
}
//Parse number of diversion channels 
void Hyd_Parse_Glob::parse_diversion_channel_number(_hyd_keyword_file Key, word Command){
	// There is not a following keyword.
	if (strlen(Command) <= 0){
		this->GetLine(Command);
	}
	Key = ParseNextKeyword(Command);
	stringstream buffer;
	buffer<<Command;
	if (Key == eFAIL){
		buffer >> this->Globals.number_div_channel;
	}
	if(buffer.fail()==true){
		ostringstream info;
		info << "Wrong input sequenze " << buffer.str() << endl;
		Error msg=this->set_error(18);
		msg.make_second_info(info.str());
		throw msg;
	}
}
//Parse number of structure coupling (1d2d) 
void Hyd_Parse_Glob::parse_structure_coupling_number(_hyd_keyword_file Key, word Command){
	// There is not a following keyword.
	if (strlen(Command) <= 0){
		this->GetLine(Command);
	}
	Key = ParseNextKeyword(Command);
	stringstream buffer;
	buffer<<Command;
	if (Key == eFAIL){
		buffer >> this->Globals.number_struc_coupling;
	}
	if(buffer.fail()==true){
		ostringstream info;
		info << "Wrong input sequenze " << buffer.str() << endl;
		Error msg=this->set_error(19);
		msg.make_second_info(info.str());
		throw msg;
	}
}
//Parse number of dikebreak coupling river to floodplain models 
void Hyd_Parse_Glob::parse_dikebreak_rv2fp_coupling_number(_hyd_keyword_file Key, word Command){
	// There is not a following keyword.
	if (strlen(Command) <= 0){
		this->GetLine(Command);
	}
	Key = ParseNextKeyword(Command);
	stringstream buffer;
	buffer<<Command;
	if (Key == eFAIL){
		buffer >> this->Globals.number_dikebreak_rv2fp_coupling;
	}
	if(buffer.fail()==true){
		ostringstream info;
		info << "Wrong input sequenze " << buffer.str() << endl;
		Error msg=this->set_error(20);
		msg.make_second_info(info.str());
		throw msg;
	}

}
//Parse number of dikebreak coupling coast to floodplain models 
void Hyd_Parse_Glob::parse_dikebreak_fp2co_coupling_number(_hyd_keyword_file Key, word Command){
	// There is not a following keyword.
	if (strlen(Command) <= 0){
		this->GetLine(Command);
	}
	Key = ParseNextKeyword(Command);
	stringstream buffer;
	buffer<<Command;
	if (Key == eFAIL){
		buffer >> this->Globals.number_dikebreak_fp2co_coupling;
	}
	if(buffer.fail()==true){
		ostringstream info;
		info << "Wrong input sequenze " << buffer.str() << endl;
		Error msg=this->set_error(20);
		msg.make_second_info(info.str());
		throw msg;
	}

}
//Appling of a coastmodel
void Hyd_Parse_Glob::parse_coastmodel(_hyd_keyword_file Key, word Command){
	// There is not a following keyword.
	if (strlen(Command) <= 0){
		this->GetLine(Command);
	}
	Key = ParseNextKeyword(Command);
	stringstream buffer;
	buffer<<Command;
	if (Key == eFAIL){
		string str_buff;
		buffer >> str_buff;
		try{
			this->Globals.coastmodel_applied=_Hyd_Parse_IO::transform_string2boolean(str_buff);
		}
		catch(Error msg){
			ostringstream info;
			info << "Filename: " << this->input_file_name <<endl;
			info << "Error occurs near line: "<< this->line_counter << endl;
			info << "Settings for the coast model" <<endl;
			msg.make_second_info(info.str());
			throw msg;
		}
	}
}
//Appling of a temperature model
void Hyd_Parse_Glob::parse_tempmodel(_hyd_keyword_file Key, word Command) {
	// There is not a following keyword.
	if (strlen(Command) <= 0) {
		this->GetLine(Command);
	}
	Key = ParseNextKeyword(Command);
	stringstream buffer;
	buffer << Command;
	if (Key == eFAIL) {
		string str_buff;
		buffer >> str_buff;
		try {
			this->Globals.tempmodel_applied = _Hyd_Parse_IO::transform_string2boolean(str_buff);
		}
		catch (Error msg) {
			ostringstream info;
			info << "Filename: " << this->input_file_name << endl;
			info << "Error occurs near line: " << this->line_counter << endl;
			info << "Settings for the temerature model" << endl;
			msg.make_second_info(info.str());
			throw msg;
		}
	}

}
//parse integration setting
void Hyd_Parse_Glob::parse_integration_setting(_hyd_keyword_file Key, word Command){
	// Next keyword is SET or STANDARD
	do{
		if (Key == eFAIL){
			if (!this->GetLine(Command)){	
				Error msg=this->set_error(4);
				throw msg;
			}
		}
		Key = ParseNextKeyword(Command);
	}while ((Key != eSET) && (Key != eSTANDARD));

	if(Key == eSTANDARD){
		return;
	}
	else if (Key == eSET){
		// Get Integration constants
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
			if (Key == eMAXNUMSTEPS){
				buffer >> this->Globals.GlobMaxNumSteps;
			}
			else if (Key == eMAXSTEPSIZE){
				buffer >> this->Globals.GlobMaxStepSize;
			}
			else if (Key == eINISTEPSIZE){
				buffer >>this->Globals.GlobInitStepSize;
			}
			if(buffer.fail()==true){
				ostringstream info;
				info << "Wrong input sequenze " << buffer.str() << endl;
				Error msg=this->set_error(17);
				msg.make_second_info(info.str());
				throw msg;
			}
		}while (Key != eSET); // End of Integration specifications are marked by </Set>
	}
}
//Parse output setting
void Hyd_Parse_Glob::parse_output_setting(_hyd_keyword_file Key, word Command) {
	// Next keyword is SET or STANDARD
	do {
		if (Key == eFAIL) {
			if (!this->GetLine(Command)) {
				Error msg = this->set_error(5);
				throw msg;
			}
		}
		Key = ParseNextKeyword(Command);
	} while ((Key != eSET) && (Key != eSTANDARD));

	// Set specifications for STANDARD
	if (Key == eSTANDARD) {
		return;
	}
	else if (Key == eSET) {
		//
		do {
			if (Key == eFAIL) {
				if (!this->GetLine(Command)) {
					Error msg = this->set_error(5);
					throw msg;
				}
			}
			Key = ParseNextKeyword(Command);
			stringstream buffer;
			string str_buff;
			buffer << Command;
			buffer >> str_buff;
			if (Key == eTECPLOT1D) {
				this->Globals.output_flags.tecplot_1d_required = _Hyd_Parse_IO::transform_string2boolean(str_buff);
			}
			else if (Key == eTECPLOT2D) {
				this->Globals.output_flags.tecplot_2d_required = _Hyd_Parse_IO::transform_string2boolean(str_buff);
			}
			else if (Key == eBLUEKENUE2D) {
				this->Globals.output_flags.bluekenue_2d_required= _Hyd_Parse_IO::transform_string2boolean(str_buff);
			}
			else if (Key == ePARAVIEW1D) {
				this->Globals.output_flags.paraview_1d_required= _Hyd_Parse_IO::transform_string2boolean(str_buff);
			}
			else if (Key == ePARAVIEW2D) {
				this->Globals.output_flags.paraview_2d_required= _Hyd_Parse_IO::transform_string2boolean(str_buff);
			}
			else if (Key == eDATABASE_INSTAT) {
				this->Globals.output_flags.database_instat_required = _Hyd_Parse_IO::transform_string2boolean(str_buff);
			}
			else if (Key == eOUTPUT_FOLDER) {
				_Hyd_Parse_IO::erase_carriageReturn(&str_buff);
				_Hyd_Parse_IO::erase_leading_whitespace_tabs(&str_buff);
				_Hyd_Parse_IO::erase_end_whitespace_tabs(&str_buff);
				this->Globals.output_flags.output_folder = _Hyd_Parse_IO::insert_linux_slash2string(str_buff);
			}
		} while (Key != eSET); // End of Precon specifications are marked by </Set>
	}
}
//parse preconditioner settings
void Hyd_Parse_Glob::parse_precon_setting(_hyd_keyword_file Key, word Command){
	// Next keyword is SET or STANDARD
	do{
		if (Key == eFAIL){
			if (!this->GetLine(Command)){
				Error msg=this->set_error(5);
				throw msg;
			}
		}
		Key = ParseNextKeyword(Command);
	}while ((Key != eSET) && (Key != eSTANDARD));

	// Set specifications for STANDARD
	if(Key==eSTANDARD){
		return;
	}
	else if (Key == eSET){
		// _hyd_prec_type and GramSchmidt are specified
		do{
			if (Key == eFAIL){
				if (!this->GetLine(Command)){
					Error msg=this->set_error(5);
					throw msg;
				}
			}
			Key = ParseNextKeyword(Command);
			stringstream buffer;
			string str_buff;
			buffer<<Command;
			buffer>>str_buff;
			if (Key == ePRETYPE){
				this->Globals.GlobPreType =this->Globals.convert_txt2precontype(str_buff);
			}
			else if (Key == eGRAMSCHMIDT){
				this->Globals.GlobGramSchmidt=this->Globals.convert_txt2gramschmidttype(str_buff);
			}
		}while (Key != eSET); // End of Precon specifications are marked by </Set>
	}
}
//Parse syncronisation settings
void Hyd_Parse_Glob::parse_sync_setting(_hyd_keyword_file Key, word Command){
	// Next keyword is SET or STANDARD
	do{
		if (Key == eFAIL){
			if (!this->GetLine(Command)){
				Error msg=this->set_error(25);
				throw msg;
			}
		}
		Key = ParseNextKeyword(Command);
	}while ((Key != eSET) && (Key != eSTANDARD));

	// Set specifications for STANDARD
	if(Key==eSTANDARD){
		return;
	}
	else if (Key == eSET){
		
		do{
			if (Key == eFAIL){
				if (!this->GetLine(Command)){
					Error msg=this->set_error(25);
					throw msg;
				}
			}
			Key = ParseNextKeyword(Command);
			stringstream buffer;
			buffer<<Command;
			if (Key == eSYNMAXHFP){
				buffer>>this->Globals.max_h_change_fp;
			}
			else if (Key == eSYNMAXHRV){
				buffer>>this->Globals.max_h_change_rv;
			}
			else if (Key == eSYNMINTSTEP){
				buffer>>this->Globals.min_internal_step;
			}
			else if (Key == eSYNMAXVRV){
				buffer>>this->Globals.max_v_change_rv;
			}
		}while (Key != eSET); // End of coup specifications are marked by </Set>
	}
}
//parse file name where the manning values are set
void Hyd_Parse_Glob::parse_material_file(_hyd_keyword_file Key, word Command){
	// There is not a following keyword.
	if (strlen(Command) <= 0){
		this->GetLine(Command);
	}

	Key = ParseNextKeyword(Command);

	if (Key == eFAIL){
		char buffer[512];
		this->RemoveDelimiters(Command, buffer); 
		this->UseLinuxSlash(buffer);
		Globals.material_file=buffer;
		this->found_material_file=true;
	}
}
//Parse file name, where the observation points are set
void Hyd_Parse_Glob::parse_obs_point_file(_hyd_keyword_file Key, word Command){
	// There is not a following keyword.
	if (strlen(Command) <= 0){
		this->GetLine(Command);
	}

	Key = ParseNextKeyword(Command);

	if (Key == eFAIL){
		char buffer[512];
		this->RemoveDelimiters(Command, buffer); 
		this->UseLinuxSlash(buffer);
		Globals.obs_point_file=buffer;
	}

}
//Chek if the requiered paramaters were found
void Hyd_Parse_Glob::check_parameters_found(void){

	if(this->found_material_file==false){
		Error msg=this->set_error(6);
		throw msg;
	}
	if(this->found_no_river==false){
		Error msg=this->set_error(7);
		throw msg;
	}
	if(this->found_no_floodplain==false){
		Error msg=this->set_error(8);
		throw msg;
	}
	if(this->found_starttime==false){
		Error msg=this->set_error(9);
		throw msg;
	}
	if(this->found_no_timesteps ==false){
		Error msg=this->set_error(10);
		throw msg;
	}
	if(this->found_no_int_timesteps ==false){
		Error msg=this->set_error(11);
		throw msg;
	}
	if(this->found_timestep ==false){
		Error msg=this->set_error(12);
		throw msg;
	}

}
//Set the warning
Warning Hyd_Parse_Glob::set_warning(const int warn_type){
	string place="Hyd_Parse_Glob::";
		string help;
		string reason;
		string reaction;
		int type=0;
		Warning msg;
		stringstream info;

	switch (warn_type){
		case 0://Type of material value does not match
			place.append("check_valid_file(void)") ;
			reason="No valid commands are found in file";
			reaction="Commands are ignored";
			help= "Check this commands";
			info << "Filename: " << this->input_file_name <<endl;
			type=1;	
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
Error Hyd_Parse_Glob::set_error(const int err_type){
		string place="Hyd_Parse_Glob::";
		string help;
		string reason;
		int type=0;
		bool fatal=false;
		stringstream info;
		Error msg;

	switch (err_type){
		case 0://no end or begin description is found
			place.append("check_valid_file(void)");
			reason="The input file do not start with !$BEGINDESCRIPTION";
			help="Check the inputfile";
			info << "Filename: " << this->input_file_name <<endl;
			type=1;
			break;
		case 1://no end description is found
			place.append("check_valid_file(void)");
			reason="The input file do not end with !$ENDDESCRIPTION";
			help="Check the inputfile";
			info << "Filename: " << this->input_file_name <<endl;
			type=1;
			break;
		case 2://no begin of global parameters is found
			place.append("parse_global_params(void)");
			reason="Do not find the begin of the global parameters; it has to start with !$BEGINGLOBAL";
			help="Check the inputfile";
			info << "Filename: " << this->input_file_name <<endl;
			type=1;
			break;
		case 3://problem with time setting
			place.append("parse_time_setting(_hyd_keyword_file Key, word Command)");
			reason="Can not read in properly the global time setting block";
			help="Check the time setting block in the inputfile";
			info << "Filename: " << this->input_file_name <<endl;
			info << "Error occurs near line: "<< this->line_counter << endl;
			type=1;
			break;
		case 4://problem with integretion settings
			place.append("parse_integration_setting(_hyd_keyword_file Key, word Command)");
			reason="Can not read in properly the global integration setting block";
			help="Check the integration setting block in the inputfile";
			info << "Filename: " << this->input_file_name <<endl;
			info << "Error occurs near line: "<< this->line_counter << endl;
			type=1;
			break;
		case 5://problem with preconditionersettings
			place.append("parse_precon_setting(_hyd_keyword_file Key, word Command)");
			reason="Can not read in properly the global preconditioner setting block";
			help="Check the preconditioner setting block in the inputfile";
			info << "Filename: " << this->input_file_name <<endl;
			info << "Error occurs near line: "<< this->line_counter << endl;
			type=1;
			break;
		case 6://materialfile-name not found
			place.append("check_parameters_found(void)");
			reason="Did not find the materialfile-name";
			help="Check the materialfile-name setting in the inputfile";
			info << "Filename: " << this->input_file_name <<endl;
			type=1;
			break;
		case 7://number of river models not found
			place.append("check_parameters_found(void)");
			reason="Did not find the number of river models";
			help="Check the number of river models setting in the inputfile";
			info << "Filename: " << this->input_file_name <<endl;
			type=1;
			break;
		case 8://number of floodplain models not found
			place.append("check_parameters_found(void)");
			reason="Did not find the number of floodplain models";
			help="Check the number of floodplainmodels setting in the inputfile";
			info << "Filename: " << this->input_file_name <<endl;
			type=1;
			break;
		case 9://startime not found
			place.append("check_parameters_found(void)");
			reason="Did not find the starttime";
			help="Check the startime setting in the inputfile";
			info << "Filename: " << this->input_file_name <<endl;
			type=1;
			break;
		case 10://number of timesteps not found
			place.append("check_parameters_found(void)");
			reason="Did not find the number of timesteps";
			help="Check the timesteps setting in the inputfile";
			info << "Filename: " << this->input_file_name <<endl;
			type=1;
			break;
		case 11://number of internal timesteps not found
			place.append("check_parameters_found(void)");
			reason="Did not find the number of interal timesteps";
			help="Check the internal timesteps setting in the inputfile";
			info << "Filename: " << this->input_file_name <<endl;
			type=1;
			break;
		case 12://timestep not found
			place.append("check_parameters_found(void)");
			reason="Did not find the timestep length";
			help="Check the timestep length setting in the inputfile";
			info << "Filename: " << this->input_file_name <<endl;
			type=1;
			break;
		case 13://no end is found
			place.append("parse_global_params(void)");
			reason="Do not find the end of the global description of the global parameters; it has to end with !$ENDGLOBAL";
			help="Check the inputfile";
			info << "Filename: " << this->input_file_name <<endl;
			type=1;
			break;
		case 14://wrong input
			place.append("parse_time_setting(_hyd_keyword_file Key, word Command)");
			reason="There is a problem with the input; wrong sign is read in";
			help="Check the time settings of the global parameters in file";
			info << "Filename: " << this->input_file_name <<endl;
			info << "Error occurs near line: "<< this->line_counter << endl;
			type=1;
			break;
		case 15://wrong input
			place.append("parse_floodplain_number(_hyd_keyword_file Key, word Command)");
			reason="There is a problem with the input; wrong sign is read in";
			help="Check the floodplain number of the global parameters in file";
			info << "Filename: " << this->input_file_name <<endl;
			info << "Error occurs near line: "<< this->line_counter << endl;
			type=1;
			break;
		case 16://wrong input
			place.append("parse_river_number(_hyd_keyword_file Key, word Command)");
			reason="There is a problem with the input; wrong sign is read in";
			help="Check the river number of the global parameters in file";
			info << "Filename: " << this->input_file_name <<endl;
			info << "Error occurs near line: "<< this->line_counter << endl;
			type=1;
			break;
		case 17://wrong input
			place.append("parse_river_number(_hyd_keyword_file Key, word Command)");
			reason="There is a problem with the input; wrong sign is read in";
			help="Check the river number of the global parameters in file";
			info << "Filename: " << this->input_file_name <<endl;
			info << "Error occurs near line: "<< this->line_counter << endl;
			type=1;
			break;
		case 18://wrong input
			place.append("parse_diversion_channel_number(_hyd_keyword_file Key, word Command");
			reason="There is a problem with the input; wrong sign is read in";
			help="Check the number of diversion channels of the global parameters in file";
			info << "Filename: " << this->input_file_name <<endl;
			info << "Error occurs near line: "<< this->line_counter << endl;
			type=1;
			break;
		case 19://wrong input
			place.append("parse_structure_coupling_number(_hyd_keyword_file Key, word Command");
			reason="There is a problem with the input; wrong sign is read in";
			help="Check the number of structure coupling of the global parameters in file";
			info << "Filename: " << this->input_file_name <<endl;
			info << "Error occurs near line: "<< this->line_counter << endl;
			type=1;
			break;
		case 20://wrong input
			place.append("parse_dikebreak_coupling_number(_hyd_keyword_file Key, word Command");
			reason="There is a problem with the input; wrong sign is read in";
			help="Check the number of dikebreak coupling of the global parameters in file";
			info << "Filename: " << this->input_file_name <<endl;
			info << "Error occurs near line: "<< this->line_counter << endl;
			type=1;
			break;
		case 21://invalid text is found
			place.append("check_valid_file(void)");
			reason="There is invalid text in the global file";
			help="Check the global input file; use comments \"#\" ";
			info << "Filename: " << this->input_file_name <<endl;
			type=1;
			break;
		case 22://no end of a block is found
			place.append("check_valid_file(void)");
			reason="There is no !$END... of a !$BEGIN... command";
			help="Check the global input file; every begin-command of a block must be closed with the corresponding end-command ";
			info << "Filename: " << this->input_file_name <<endl;
			type=1;
			break;
		case 23://no begin of a block is found
			place.append("check_valid_file(void)");
			reason="There is no !$BEGIN... to a !$END... command";
			help="Check the global input file; every end-command of a block must be opened with the corresponding begin-command ";
			info << "Filename: " << this->input_file_name <<endl;
			type=1;
			break;
		case 24://no corresponding end command found
			place.append("check_valid_file(void)");
			reason="There is no matching end-command";
			help="Check the global input file; the begin-command must be closed with the corresponding end-command";
			info << "Filename: " << this->input_file_name <<endl;
			type=1;
			break;
		case 25://problem with coupling settings
			place.append("parse_coup_setting(_hyd_keyword_file Key, word Command)");
			reason="Can not read in properly the global coupling setting block";
			help="Check the coupling setting block in the inputfile";
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