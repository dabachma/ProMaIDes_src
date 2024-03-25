
//#include "_Hyd_Parse_IO.h"
#include "Hyd_Headers_Precompiled.h"

//default constructor 
_Hyd_Parse_IO::_Hyd_Parse_IO(void){
	this->input_file_name=label::not_set;
	this->line_counter=0;
}
//Constructor where the file name of global parameter is given
_Hyd_Parse_IO::_Hyd_Parse_IO(const string file_name){
	this->input_file_name=file_name;
	this->line_counter=0;
}
//destructor
_Hyd_Parse_IO::~_Hyd_Parse_IO(void){
	if (this->input_file.is_open()==true){
		this->input_file.close();		
	}
}
//_________________
//public
//set the input file name of the global file
void _Hyd_Parse_IO::set_input_filename(const string file_name){
	this->input_file_name=file_name;
}
//open the global input file 
void _Hyd_Parse_IO::open_input_file(void){
	if (this->input_file.is_open()==false){
		this->input_file.open(this->input_file_name.c_str());		
	}
	if(this->input_file.is_open()==false){
		Error msg=this->set_error(0);
		throw msg;
	}

}
//reopen the global input file
void _Hyd_Parse_IO::reopen_input_file(void){
	if (this->input_file.is_open()==true){
		this->input_file.close();
	}
	this->input_file.open(this->input_file_name.c_str());

	if(this->input_file.is_open()==false){
		Error msg=this->set_error(0);
		throw msg;
	}
	
}
//Close the input file
void _Hyd_Parse_IO::close_input_file(void){
	if (this->input_file.is_open()==true){
		this->input_file.close();
	}
}
//(static) transform string to boolean
bool _Hyd_Parse_IO::transform_string2boolean(string my_string_flag){

	string2lower(&my_string_flag);
	erase_carriageReturn(&my_string_flag);
	erase_leading_whitespace_tabs(&my_string_flag);
	erase_end_whitespace_tabs(&my_string_flag);

	if(my_string_flag=="false"){
		return false;
	}
	else if (my_string_flag=="true"){
		return true;
	}
	else{
		Error msg;
		msg.set_msg("_Hyd_Parse_IO::transform_string2boolean(string my_string_flag)", "Can not convert this boolean type", "Check the given type", 1, false);
		ostringstream info;
		info << "Try to convert boolean type: " << my_string_flag << endl;
		info << "Possible types: "<< endl;
		info << " true"  << endl;
		info << " false"  << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
}
//(static) erase carriage return "\r" from end of file (using when parsing text files with Windows Encoding on a Unix System)
void _Hyd_Parse_IO::erase_carriageReturn(string* my_string) {
	char one_char[1];
	int length = my_string->length();
	for (int i = length - 1; i >= 0; i--) {
		my_string->copy(one_char, 1, i);
		if (one_char[0] == '\r') {
			my_string->erase(i, 1);
		}
		else {
			break;
		}
	}
}
//(static) erase whitespaces and tabs before the first sign
void _Hyd_Parse_IO::erase_leading_whitespace_tabs(string *my_string){
	char one_char[1];
	int length=my_string->length();

	for (int i=0; i< length;i++){
		my_string->copy(one_char, 1, 0);
		if(one_char[0]==' ' || one_char[0]=='\t'){
			my_string->erase(0,1);
		}
		else{
			break;
		}
	}

}
//(static) Erase end whitespaces and tabs after the last sign
void _Hyd_Parse_IO::erase_end_whitespace_tabs(string *my_string){
	
	if(my_string->empty()==true){
		return;
	}

	int length=my_string->length();
	int new_length=length;
	
	do{ 
		//my_string->copy(one_char, 1, new_length-1);
		if(my_string->at(new_length-1)==' ' || my_string->at(new_length-1)=='\t' || 
			my_string->at(new_length-1)=='\n' || my_string->at(new_length-1)=='\r'){
			my_string->erase(new_length-1, 1);
			new_length=my_string->length();
		}
		else{
			break;
		}
	}
	while(my_string->empty()==false );
}

//(static) Convert keywords to lower case to compare them 
void _Hyd_Parse_IO::string2lower(string *convert_string){

	int length = convert_string->length();
	for(int i=0; i<length; ++i){
		(*convert_string)[i] = tolower((*convert_string)[i]);
	}
}
//(static) Erase all text behind a comment "#"
void _Hyd_Parse_IO::erase_comment(string *erase_str){
	//delete the comments with #
	int pos=erase_str->find("#",0);
	if(pos>=0){
		erase_str->erase(pos);
	}
}
//(static) Erase all text behind a comment """
void _Hyd_Parse_IO::erase_quote(string *erase_str) {
	//delete the comments with "
	int pos = erase_str->find("\"", 0);
	if (pos >= 0) {
		erase_str->erase(pos);
	}
}
//(static) Use the unix slash ans insert it in str 
void _Hyd_Parse_IO::UseLinuxSlash(char *str){
	for (int i=0; i< (signed)strlen(str); i++){
		if (str[i]=='\\'){
			str[i]='/';
		}
	}
}
//(static) Use the unix slash and insert it in the string object, which is returned 
string _Hyd_Parse_IO::insert_linux_slash2string(string str){

	for (int i=0; i< (signed)str.length(); i++){
		if (str.at(i)=='\\'){
			str[i]='/';
		}
	}
	return str;
}
//___________________
//protected
//Parse the next line to a keyword begin 
bool _Hyd_Parse_IO::GetLine(char* Return){
	bool IsCommand = false;
	string Command;
	do{
		getline(this->input_file, Command, '\n');
		this->line_counter++;
		// Only if Line is not empty....
		if (!Command.empty()){
			_Hyd_Parse_IO::erase_comment(&Command);
			// delete white spaces and tabs at begin
			_Hyd_Parse_IO::erase_carriageReturn(&Command);
			_Hyd_Parse_IO::erase_leading_whitespace_tabs(&Command);
		}

		// Now check, if it is a command, that begins with ! or $ ! 
		if (!Command.empty() && ((Command[0] == '!') || (Command[0] == '$') || (Command[0] == '<'))) {
			IsCommand = true;
		}

		// If reached file end 
		if  (this->input_file.eof()){
			return (false);
		}

	}while (!IsCommand);
	strcpy(Return, Command.c_str());
	return (true);
}
//rewind the file
void _Hyd_Parse_IO::Rewind(void){
	this->input_file.clear();
	this->input_file.seekg(0, ios_base::beg); 
	this->line_counter=0;
}
//search for the next keyword
_hyd_keyword_file _Hyd_Parse_IO::ParseNextKeyword(char *CommandList){
	_hyd_keyword_file Keyword = eFAIL;

//the keywords are case insensitive

	string Command = CommandList;	

	//erase comments
	//delete the comments with #
	int pos=Command.find("#",0);
	if(pos>=0){
		Command.erase(pos);
	}

	this->erase_carriageReturn(&Command);

	//erase leading white spaces
	this->erase_leading_whitespace_tabs(&Command);

	// This copy id uppercased now! (For FIND)
	for (int i=0; i < (signed int)Command.length(); i++){
		Command[i]=toupper(Command[i]);
	}

	// Let�s see, which command we found. 
	//string RValueString;
	string Command1=Command;
	int PosA = Command.find_first_of(">", 0);
	int PosB = Command.find_first_of("=", 0);
	int Pos=0;
	if (PosA > 0 && PosB > 0 && PosA < PosB)  Pos = PosA;
	else if (PosA > 0 && PosB > 0 && PosB < PosA)  Pos = PosB;
	else if (PosA <= 0 && PosB <= 0)  Pos = Command.length();
	else if (PosA < 0 && PosB > 0)  Pos = PosB;
	else if (PosB < 0 && PosA > 0)  Pos = PosA;
	else  Pos = Command.length();

    unsigned int posc=Command.find_first_of("=", 0);
	if(posc>0){
		Command1=Command.substr(0,posc);
	}
	this->erase_end_whitespace_tabs(&Command1);
	posc=Command1.length();
	
	//overall begin and end marker
	string buffer;
	//if (buffer.assign("!$BEGINDESCRIPTION") ,buffer.length()==posc && FIND("!$BEGINDESCRIPTION")		>= 0 && FIND("!$BEGINDESCRIPTION")	< Pos)	Keyword = eBEGINDESCRIPTION;
	if(FIND1("!$BEGINDESCRIPTION")				) Keyword = eBEGINDESCRIPTION;
	else if (FIND1("!$ENDDESCRIPTION")			)	Keyword = eENDDESCRIPTION;
	//marker for the global settings
	else if (FIND1("!$BEGINGLOBAL")				)	Keyword = eBEGINGLOBAL;
	else if (FIND1("!$ENDGLOBAL")				)  Keyword = eENDGLOBAL;
	//marker for the floodplain model settings
	else if (FIND1("!$BEGINFPMODEL")			)	Keyword = eBEGINFPMODEL;
	else if (FIND1("!$ENDFPMODEL")				)  Keyword = eENDFPMODEL;
	//marker for the river model settings
	else if (FIND1("!$BEGINRVMODEL")			)	Keyword = eBEGINRVMODEL;
	else if (FIND1("!$ENDRVMODEL")				)  Keyword = eENDRVMODEL;
	//marker for the coast model settings
	else if (FIND1("!$BEGINCOASTMODEL")			)	Keyword = eBEGINCOASTMODEL;
	else if (FIND1("!$ENDCOASTMODEL")			)  Keyword = eENDCOASTMODEL;
	//marker for the diversion channels settings
	else if (FIND1("!$BEGINDVCHANNEL")			)	Keyword = eBEGINDVCHANNEL;
	else if (FIND1("!$ENDDVCHANNEL")				)  Keyword = eENDDVCHANNEL;
	//marker for the coupling river- to floodplain model via hydraulic structure
	else if (FIND1("!$BEGINSTRUCTURECOUPLING")	)	Keyword = eBEGINSTRUCTURECOUPLING;
	else if (FIND1("!$ENDSTRUCTURECOUPLING")		)  Keyword = eENDSTRUCTURECOUPLING;
	//marker for the coupling of a river- to floodplian-model coupling via dikebreak
	else if (FIND1("!$BEGINRV2FPBREAKCOUPLING")	)  Keyword = eBEGINDIKEBREAKCOUPLING;
	else if (FIND1("!$ENDRV2FPBREAKCOUPLING")	)  Keyword = eENDDIKEBREAKCOUPLING;

	//marker for the coupling of a coast- to floodplian-model coupling via dikebreak
	else if (FIND1("!$BEGINFP2COBREAKCOUPLING")	)  Keyword = eBEGINFP2COBREAKCOUPLING;
	else if (FIND1("!$ENDFP2COBREAKCOUPLING")	)  Keyword = eENDFP2COBREAKCOUPLING;




	//general
	else if (FIND("<SET>")				>= 0 && FIND("<SET>")				< Pos)	Keyword = eSET;
	else if (FIND("</SET>")				>= 0 && FIND("</SET>")				< Pos)	Keyword = eSET; 
	else if (FIND("<STANDARD>")			>= 0 && FIND("<STANDARD>")			< Pos)	Keyword = eSTANDARD;

	// Global parameters
	//time settings
	else if (FIND1("!TIME")			) 	Keyword = eTIME;
	else if (FIND1("$TSTART")		)	Keyword = eTSTART;
	else if (FIND1("$TSTEP")		)	Keyword = eTSTEP;
	else if (FIND1("$TNOF")			)	Keyword = eTNOF; 
	else if (FIND1("$NOFITS")		)	Keyword = eNOFITS;
	//models
	else if (FIND1("!NOFFP")		)	Keyword = eNOFFP;
	else if (FIND1("!NOFRV")		)	Keyword = eNOFRV;
	else if (FIND1("!COASTMODEL")	)	Keyword = eCOASTMODEL;
	else if (FIND1("!TEMPMODEL")	)	Keyword = eTEMPMODEL;
	//setted couplings
	else if (FIND1("!NOFDC")			)	Keyword = eNOFDC;
	else if (FIND1("!NOFSC")			)	Keyword = eNOFSC;
	else if (FIND1("!NOFBREAKRV2FP")	)	Keyword = eNOFDBC;
	else if (FIND1("!NOFBREAKFP2CO")	)	Keyword = eNOFFP2COBREAK;
	//solver settings
	else if (FIND1("!INTEGRATION")	)	Keyword = eINTEGRATION;
	else if (FIND1("$MAXNUMSTEPS")	)	Keyword = eMAXNUMSTEPS; 
	else if (FIND1("$MAXSTEPSIZE")	)	Keyword = eMAXSTEPSIZE;
	else if (FIND1("$INISTEPSIZE")	)	Keyword = eINISTEPSIZE;
	//output settings
	else if (FIND1("!OUTPUT"))	Keyword = eOUTPUT;
	else if (FIND1("$TECPLOT_1D"))	Keyword = eTECPLOT1D;
	else if (FIND1("$TECPLOT_2D"))	Keyword = eTECPLOT2D;
	else if (FIND1("$BLUEKENUE_2D"))	Keyword = eBLUEKENUE2D;
	else if (FIND1("$PARAVIEW_EXCEL_1D"))	Keyword = ePARAVIEW1D;
	else if (FIND1("$PARAVIEW_2D"))	Keyword = ePARAVIEW2D;
	else if (FIND1("$DATBASE_INSTAT"))	Keyword = eDATABASE_INSTAT;
	else if (FIND1("$OUTPUT_FOLDER"))	Keyword = eOUTPUT_FOLDER;


	//temperature model
	else if (FIND1("!TEMP_DATA"))	Keyword = eTEMP_INFO;
	else if (FIND1("$GW_TEMP"))	Keyword = eGW_TEMP;
	else if (FIND1("$BRUNT_COEF"))	Keyword = eBRUNT;
	else if (FIND1("$VIEW2SKY"))	Keyword = eVIEW2SKY;
	else if (FIND1("$K_BED"))	Keyword = eHEAT_COND;
	else if (FIND1("$BED_TEMP"))	Keyword = eBED_TEMP;
	else if (FIND1("$BED_WARMING"))	Keyword = eBED_WARM;
	else if (FIND1("$DIFF_SOLAR_RAD"))	Keyword = eSOLAR_RAD;


	//preconditioner settings
	else if (FIND1("!PRECON")		)	Keyword = ePRECON; 
	else if (FIND1("$PRETYPE")		)	Keyword = ePRETYPE; 
	else if (FIND1("$GRAMSCHMIDT")	)	Keyword = eGRAMSCHMIDT;	
	//coupling settings
	else if (FIND1("!SYNCRON")			)	Keyword = eSYNCRON; 
	else if (FIND1("$MAXCHANGE_H_FP")	)	Keyword = eSYNMAXHFP;
	else if (FIND1("$MAXCHANGE_H_RV")	)	Keyword = eSYNMAXHRV;
	else if (FIND1("$MAXCHANGE_V_RV")	)	Keyword =eSYNMAXVRV;
	else if (FIND1("$MIN_INTSTEP")		)	Keyword = eSYNMINTSTEP;
	//global material file
	else if (FIND1("!MATERIALFILE")	)	Keyword = eMATERIALFILE;
	//observation point file
	else if (FIND1("!OBSPOINTFILE")	)	Keyword = eOBSPOINTFILE;

	// Instationary boundary curves used by all models
	else if (FIND1("!INSTATBOUNDFILE"))	Keyword = eINSTATBOUNDFILE;
	else if (FIND1("$NUMBER_CURVE")	)	Keyword = eNUMBER_CURVE;
	else if (FIND1("$FILENAME")		)	Keyword = eFILENAME;
	//noflow polygons (enclosing noflow cells)
	else if (FIND1("!NOFLOWFILE")	)	Keyword = eNOFLOWPOLYFILE;
	else if (FIND1("$NO_POLYGONS")	)	Keyword = eNO_NOFLOWPOLYS;
	//dikeline polysegments
	else if (FIND1("!DIKELINEFILE")	)	Keyword = eDIKELINEFILE;
	else if (FIND1("$NO_POLYLINES")	)	Keyword = eNODIKEPOLYLINES;
	// Floodplain model specific
	else if (FIND1("!GENERAL")		)	Keyword = eGENERAL; 
	else if (FIND1("$NX")			)	Keyword = eNX;
	else if (FIND1("$NY")			)	Keyword = eNY; 
	else if (FIND1("$ELEMWIDTH_X")	)	Keyword = eELEMENTWIDTH_X;
	else if (FIND1("$ELEMWIDTH_Y")	)	Keyword = eELEMENTWIDTH_Y;
	else if (FIND1("$ANGLE")			)	Keyword = eANGLE;	
	else if (FIND1("$LOWLEFTX")		)	Keyword = eLOWLEFTX;
	else if (FIND1("$LOWLEFTY")		)	Keyword = eLOWLEFTY; 
	else if (FIND1("$NOINFOVALUE")	)	Keyword = eNOINFOVALUE; 
	//file with the floodplain elements
	else if (FIND1("!FLOODPLAINFILE"))	Keyword = eFLOODPLAINFILE;
	//Scheme Settings
	else if (FIND1("!SCHEME"))				Keyword = eSCHEME;
	else if (FIND1("$SCHEME_TYPE"))			Keyword = eSCHEME_TYPE;
	else if (FIND1("$SELECTED_DEVICE"))		Keyword = eSELECTED_DEVICE;
	else if (FIND1("$COURANT_NUMBER"))		Keyword = eCOURANT_NUMBER;
	else if (FIND1("$REDUCTION_WAVEFRONTS"))Keyword = eREDUCTION_WAVEFRONTS;
	else if (FIND1("$FRICTION_STATUS"))		Keyword = eFRICTION_STATUS;
	else if (FIND1("$WORKGROUP_SIZE_X"))	Keyword = eWORKGROUP_SIZE_X;
	else if (FIND1("$WORKGROUP_SIZE_Y"))	Keyword = eWORKGROUP_SIZE_Y;
	
	//Limits for the 2d/1d calculation
	else if (FIND1("!LIMITS")		)	Keyword = eLIMITS; 
	else if (FIND1("$ATOL")			)	Keyword = eATOL; 
	else if (FIND1("$RTOL")			)	Keyword = eRTOL;
	else if (FIND1("$WET")			)	Keyword = eWET;
	//just used in 1d
	else if (FIND1("$PROF_INTERFACE")	)	Keyword = ePROF_INTERFACE;
	else if (FIND1("$V_EXPLICIT")		)	Keyword = eV_EXPLICIT;
	//output file
	else if (FIND1("!2DOUTPUT")			)	Keyword = e2DOUTPUT;

	// River Model specific
	//geometry file with the profile data
	else if (FIND1("!GEOMETRYFILE")		)	Keyword = eGEOMETRYFILE;
	//number of profiles
	else if (FIND1("!NOFPROF")			)	Keyword = eNOFPROF;
	//output file
	else if (FIND1("!1DOUTPUT")			)	Keyword = e1DOUTPUT;

	//Coast model specific
	//File for the coastline
	else if (FIND1("!COASTLINEFILE")		)	Keyword = eCOASTLINEFILE;

	//setted coupling
	//here the parameter of the inflow coupling are further specified
	else if (FIND1("!INFLOW_MODEL")		)	Keyword = eINFLOW_MODEL;
	//Specifies the number of the coupled models
	else if (FIND1("$MODEL_NO")			)	Keyword = eMODEL_NO;
	//Specifies the type of coupling structure
	else if (FIND1("$COUPSTRUCT_TYPE")	)	Keyword = eCOUPSTRUCT_TYPE;

	//here the parameters of the outflow model are further specified
	else if (FIND1("!OUTFLOW_MODEL")		)	Keyword = eOUTFLOW_MODEL;

	//coupling structures
	//here the parameters of coupling structure are further specified
	else if (FIND1("!COUPLING_STRUCT")	)	Keyword = eCOUPLING_STRUCT;
	//Specifies if the coupling structure is controlled or not
	else if (FIND1("$CONTROLLED")		)	Keyword = eCONTROLLED;
	//the width of the coupling structure
	else if (FIND1("$WIDTH")				)	Keyword = eWIDTH;
	//the controlled parameter: WEIR the local sill height; GATE the local opening height
	else if (FIND1("$CONTROLLABLE_PARAM"))	Keyword = eCONTROLLABLE_PARAM; 
	//the uncontrolled parameter: WEIR value for the Poleni Factor; GATE value for the local sill height
	else if (FIND1("$UNCONTROLLABLE_PARAM"))Keyword = eUNCONTROLLABLE_PARAM;

	//Coupling via a diversion channel (river to river)
	//number of the river model, which represents the diversion channel
	else if (FIND1("!NORIVER")			)	Keyword = eNORIVER;
	//Coupling via a hydraulic structure (river to floodplain)
	//Distance beginning from the inflow profile of the river along the bank line, where the coupling is allocated
	else if (FIND1("$DISTANCE")			)	Keyword = eDISTANCE;
	//direction of the structure coupling (left bank or right bank)
	else if (FIND1("$LEFTDIRECTION")		)	Keyword = eLEFTDIRECTION;

	//coupling via a dikebreak (RV2FP)
	//here the parameters of the dikebreak location are further specified
	else if (FIND1("!DIKEBREAK_LOCATION"))	Keyword = eDIKEBREAK_LOCATION;
	//the profile number, where the distance is related to
	else if (FIND1("$PROFILE_NO")		)	Keyword = ePROFILE_NO;
	//the point number in a dikeline, where the distance is related to
	else if (FIND1("$POINT_NO")			 )	Keyword = ePOINT_NO;
	//the waterlevel, when the breach begins
	else if (FIND1("$START_WATERH")		)	Keyword = eSTART_WATERH;
	//the maximum breach width
	else if (FIND1("$MAX_BREACHWIDTH")	)	Keyword = eMAX_BREACHWIDTH;
	
	//beginners
	/*else if (FIND("$")					>= 0 && FIND("$")					< Pos)	Keyword = eSKIP;
	else if (FIND("!")					>= 0 && FIND("!")					< Pos)	Keyword = eSKIP;
	else if (FIND("!$")					>= 0 && FIND("!$")					< Pos)	Keyword = eSKIP;*/
	
	else Keyword = eFAIL;

	Command = CommandList;

	//if(Keyword ==eSKIP){
	//	if(Command.find_first_of("!")!=0 || Command.find_first_of("$")!=0){
	//		Keyword =eFAIL;
	//	}
	//}

	// Get right side, if exists
	if (Keyword != eFAIL && Keyword!=eSKIP)
	{
		// If Pos > 0 -> Right Side with new Keywords or values
		if (Pos > 0 && Pos != string::npos) Command.erase(0, Pos+1);
		// Else clear whole content, nothing more to interpret
		else Command.clear();
		// delete white spaces at begin
		Pos=Command.find_first_not_of((char)32, 0);
		if (Command.find((char)9, 0) == Pos) Pos=Command.find_first_not_of((char)9, 0);
		if (Pos >= 0 && Pos != string::npos) Command.erase(0, Pos);
		// Update content of CommandList (Right side only now)
		strcpy(CommandList, Command.c_str());
	}
		
	return Keyword;
}
//remove the delimiters
void _Hyd_Parse_IO::RemoveDelimiters(char *src, char *dest){
	// There could be some spaces in the model text
	char *String;
	String = strchr(src, '\"');
	if (String == NULL)
		strcpy(dest, src);
	else
		strcpy(dest, ++String);

	strtok(dest, "\"#");
}
//add a slash
void _Hyd_Parse_IO::AddSlash(char *str){
	if (str[strlen(str)-1] != '/') strcat(str, "/");
}

//______________
//private
//set the error
Error _Hyd_Parse_IO::set_error(const int err_type){
		string place="_Hyd_Parse_IO::";
		string help;
		string reason;
		int type=0;
		bool fatal=false;
		stringstream info;
		Error msg;

	switch (err_type){
		case 0://File nor open
			place.append("read_values_per_file(void)");
			reason="Can not open file: ";
			help="Check the global file";
			info << "Filename " << this->input_file_name << endl;
			type=5;
			fatal=false;
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
