#pragma once
/**\class _Hyd_Parse_IO
	\author Daniel Bachmann et al.
	\author produced by the Institute of Hydraulic Engineering (IWW), RWTH Aachen University
	\version 0.0.1                                                              
	\date 2009 
*/
#ifndef _HYD_PARSE_IO_H
#define _HYD_PARSE_IO_H


//defines
#define FIND(A) (int)Command.find(A , 0)
#define FIND1(A) (buffer.assign(A), buffer.length()==posc && (int)Command.find(A , 0)>=0 && (int)Command.find(A , 0)<Pos)

//system_sys_ classes
#include "_Sys_Common_System.h"

using namespace std;
///Enumerator for the general language keywords for the parser of the main-file of the hydraulic system \ingroup hyd
enum _hyd_keyword_file{	
	///Overall begin marker
	eBEGINDESCRIPTION,
	///Overall end marker
	eENDDESCRIPTION,
	///Marker for the global settings (begin)
	eBEGINGLOBAL, 
	///Marker for the global settings (end)
	eENDGLOBAL, 
	///Marker for the floodplain model settings (begin)
	eBEGINFPMODEL,
	///Marker for the floodplain model settings (end)
	eENDFPMODEL,
	///Marker for the river model settings (begin)
	eBEGINRVMODEL, 
	///Marker for the river model settings (end)
	eENDRVMODEL,
	///Marker for the river model settings (begin)
	eBEGINCOASTMODEL,
	///Marker for the river model settings (end)
	eENDCOASTMODEL,
	///Marker for the diversion channels settings (begin)
	eBEGINDVCHANNEL, 
	///Marker for the diversion channels settings (end)
	eENDDVCHANNEL,
	///marker for the coupling of a river- to floodplian-model coupling via a hydraulic structure (begin)
	eBEGINSTRUCTURECOUPLING,
	///Marker for the coupling of a river- to floodplian-model coupling via a hydraulic structure (end)
	eENDSTRUCTURECOUPLING,
	///Marker for the coupling of a river- to floodplian-model coupling via dikebreak (begin)
	eBEGINDIKEBREAKCOUPLING,
	///Marker for the coupling of a river- to floodplian-model coupling via dikebreak (end)
	eENDDIKEBREAKCOUPLING,

	///Marker for the coupling of a coast- to floodplian-model coupling via dikebreak (begin)
	eBEGINFP2COBREAKCOUPLING,
	///Marker for the coupling of a coast- to floodplian-model coupling via dikebreak (end)
	eENDFP2COBREAKCOUPLING,
	
	///Keyword for failing
	eFAIL,
	///Marker for user defined settings
    eSET, 
	///Marker for the standard settings
	eSTANDARD, 
	///Keyword for skipping
	eSKIP,
		
	///Marker for the time settings (main key word)
	eTIME, 
	///Marker for the length of the output time step
	eTSTEP, 
	///Marker for the number of output timesteps
	eTNOF,
	///Marker for the start time
	eTSTART, 
	///Marker for the number of internal time steps
	eNOFITS,
	///Marker for the number of floodplain models (2d)
	eNOFFP,
	///Marker for the number of river model (1d)
	eNOFRV,
	///Marker for the flag for a applied coast model (true/false)
	eCOASTMODEL,
	///Marker for the flag for a applied coast model (true/false)
	eTEMPMODEL,
	///Marker for the number of diversion channels
	eNOFDC,
	///Marker for the number of structure couplings (1d-2d)
	eNOFSC,
	///Marker for the number of dikebreak couplings river to floodplain (1d-2d)
	eNOFDBC,
	///Marker for the number of dikebreak couplings coast to floodplain (co-2d)
	eNOFFP2COBREAK,
	///Marker for the solver settings (main key word)
	eINTEGRATION,

	///Marker for the output settings (main key word)
	eOUTPUT,
	///Marker for the Tecplot 1d output
	eTECPLOT1D,
	///Marker for the Tecplot 2d output
	eTECPLOT2D,
	///Marker for the Bluekenue 2d output
	eBLUEKENUE2D,
	///Marker for the Paraview / Excel 1d output
	ePARAVIEW1D,
	///Marker for the Paraview 2d output
	ePARAVIEW2D,
	///Marker for the database instationary output
	eDATABASE_INSTAT,
	///Marker for the out folder
	eOUTPUT_FOLDER,

	///Marker for the maximum number of steps
	eMAXNUMSTEPS,
	///Marker for the maximum stepsize
	eMAXSTEPSIZE,
	///Marker for the initial stepsize
	eINISTEPSIZE,
	///Marker for the preconditioner settings (main key word)
	ePRECON,
	///Marker for the preconditioner type
	ePRETYPE,
	///Marker for the Graham-Schmidt-scheme
	eGRAMSCHMIDT,
	///Marker for the syncronisation settings (main key word)
	eSYNCRON,
	///Marker for the maximum of h change in a floodplain element per syncronisation step 
	eSYNMAXHFP,
	///Marker for the maximum of h change in a river element per syncronisation step 
	eSYNMAXHRV,
	///Marker for the minimum internatl time step 
	eSYNMINTSTEP,
	///Marker for the maximum change of the explicit velocity head in a river model per syncronisation step 
	eSYNMAXVRV,

	///Marker for the path and name of the global material file
	eMATERIALFILE, 

	///Marker for the path and name of the observation point file
	eOBSPOINTFILE,

	///Marker for the instationary boundary curves (main key word)
	eINSTATBOUNDFILE,
	///Marker for the number of instationary boundary curves in file
	eNUMBER_CURVE,
	///Marker for the path and name of the instationary boundary curve file
	eFILENAME,

	///Marker for the path and name of the noflow-polygon file
	eNOFLOWPOLYFILE,
	///Marker for the number of noflow-polygons in file
	eNO_NOFLOWPOLYS,
	///Marker for the path and name of the dikeline-polysegment file
	eDIKELINEFILE,
	///Marker for the number of the dikeline-polysegment in file
	eNODIKEPOLYLINES,

	///Marker for the general floodplain settings (main keyword)
	eGENERAL,
	///Marker for the number of 2d-elements in x-direction
	eNX,
	///Marker for the number of 2d-elements in y-direction
	eNY,
	///Marker for the 2d-elements width in x-direction
	eELEMENTWIDTH_X,
	///Marker for the 2d-elements width in y-direction
	eELEMENTWIDTH_Y,
	///Marker for the angle of the 2d-raster
	eANGLE,
	///Marker for the low left x-coordinate of the 2d-raster
	eLOWLEFTX,
	///Marker for the low left y-coordinate of the 2d-raster
	eLOWLEFTY,
	///Marker for the noinfo-value
	eNOINFOVALUE,
	///Marker for the path and name of the file with the floodplain elements data
	eFLOODPLAINFILE,
	///Marker for the scheme settings of the floodplain (main keyword)
	eSCHEME,
	///Marker for the scheme to use when solving
	eSCHEME_TYPE,
	///Marker for the device selected to run the simulation on
	eSELECTED_DEVICE,
	///Marker for the CFL condition courant number
	eCOURANT_NUMBER,
	///Marker for the number of wavefront
	eREDUCTION_WAVEFRONTS,
	///Marker for the friction effect
	eFRICTION_STATUS,
	///Marker for the workgroup size in the X-direction when working with a gpu scheme
	eWORKGROUP_SIZE_X, 
	///Marker for the workgroup size in the Y-direction when working with a gpu scheme
	eWORKGROUP_SIZE_Y,
	///Marker for the limits for the 1d/2d calculation (main keyword)
	eLIMITS,
    ///Marker for the temperature model (main keyword)
	///Marker for the relative tolerance
	eRTOL,
	///Marker for the absolute tolerance
	eATOL,
	///Marker for the wet-height
	eWET,

	///Marker for the flag for taken the v-energy explicitily into account (1d)
	eV_EXPLICIT,
	///Marker for the flag of the interface of foreland and main river (1d)
	ePROF_INTERFACE,
	///Marker for the path and name of the 2d-output file 
	e2DOUTPUT,
	
	///Marker for the temperature model (main keyword)
	eTEMP_INFO,
	///Groundwater temperature
	eGW_TEMP,
	///Brunt coefficient
	eBRUNT,	
	///View-to-sky coefficient
	eVIEW2SKY,
	///Coefficient of heat conductivity multiplied by the distance
	eHEAT_COND,
	///Bed temperature
	eBED_TEMP,
	///Coefficient bed warming
	eBED_WARM,
	///Coefficient diffusive solar radiation
	eSOLAR_RAD,




	// River Model specific
	///Marker for the path and name of the geometry file with the profile data
	eGEOMETRYFILE,
	///Marker for the number of profiles in file
	eNOFPROF,
	///Marker for the path and name of the 1d-output file
	e1DOUTPUT,

	///Marker for the path and name of the coast-line file
	eCOASTLINEFILE,

	//Coupling
	///Marker for the parameters of the inflow coupling (main keyword)
	eINFLOW_MODEL,
	///Marker for the number of the coupled models
	eMODEL_NO,
	///Marker for the type of the coupling structure
	eCOUPSTRUCT_TYPE,

	///Marker for the parameters of the outflow model (main keyword)
	eOUTFLOW_MODEL,

	//couplingstructures
	///Marker for the parameters of coupling structure (main keyword)
	eCOUPLING_STRUCT,
	///Marker for the controlled flag
	eCONTROLLED,
	///Marker for the width of the coupling structure
	eWIDTH,
	///Marker for the controlled parameter of the structure (WEIR the local sill height / GATE the local opening height)
	eCONTROLLABLE_PARAM, 
	///Marker for the uncontrolled parameter of the structuer (WEIR value for the Poleni Factor / GATE value for the local sill height)
	eUNCONTROLLABLE_PARAM,

	//Coupling via a diversion channel (river to river)
	///Marker for the river model, which represents the diversion channel
	eNORIVER,
	///Marker for the distance beginning from the inflow profile of the river along the bank line, where the coupling is allocated
	eDISTANCE,
	///Marker for the direction of the coupling (left bank or right bank in flow direction)
	eLEFTDIRECTION,

	///Marker for specifing the parameters of the dikebreak location (main keyword)
	eDIKEBREAK_LOCATION,
	///Marker for the profile number, where the distance is related to
	ePROFILE_NO,
	///Marker for the point number in a dikeline, where the distance is related to
	ePOINT_NO,
	///Marker for the the starting waterlevel, when the breach begins
	eSTART_WATERH,
	///Marker for the maximum breach width
	eMAX_BREACHWIDTH,

};


///Base class for the file input foor reading the properties of the hydraulic system \ingroup hyd
/**
The bass class for the file input for the hydraulic system. Here the keywords for the data files are defined.
The keywords are searched in the data file and the corresponding data are extracted.
*/
class _Hyd_Parse_IO
{	
public:
	///Default constructor
	_Hyd_Parse_IO(void);
	///Constructor where the file name of global parameter is given
	_Hyd_Parse_IO(const string file_name);
	///Default destructor
	virtual ~_Hyd_Parse_IO(void);

	//methods
	///Set the input file name of the global file
	void set_input_filename(const string file_name);
	///Open the global input file 
	void open_input_file(void);
	///Reopen the global input file
	void reopen_input_file(void);
	///Close the input file
	void close_input_file(void);

	///Transform string to boolean
	static bool transform_string2boolean(string my_string_flag);

	///Erase carriage return "\r" from end of file (using when parsing text files with Windows Encoding on a Unix System)
	static void erase_carriageReturn(string* my_string);

	///Erase leading whitespaces and tabs before the first sign
	static void erase_leading_whitespace_tabs(string *my_string);

	///Erase end whitespaces and tabs after the last sign
	static void erase_end_whitespace_tabs(string *my_string);

	///Convert keywords to lower case to compare them 
	static void string2lower(string *convert_string);

	///Erase all text behind a comment "#"
	static void erase_comment(string *erase_str);

	//(static) Erase all text behind a comment """
	static void erase_quote(string *erase_str);

	///Use the unix slash and insert it in str
	static void UseLinuxSlash(char *str);

	///Use the unix slash and insert it in the string object, which is returned
	static string insert_linux_slash2string(string str);

protected:
	//members
	///Name of Input File
	string input_file_name;	
	///Stream of Input File
	ifstream input_file;	
	///Counter for the line in the input file
	int line_counter;

	//methods
	///Parse the next line to a keyword begin 
	bool GetLine(char* Return);
	///Rewind the file
	void Rewind(void);
	///Search for the next keyword. Here all keywords in the main data file (.ilm) are listed
	_hyd_keyword_file ParseNextKeyword(char *CommandList);

	///Remove the delimiters 
	void RemoveDelimiters(char *src, char *dest);
	///Add a slash to the given string
	void AddSlash(char *str);

private:
	//methods

	///Check if the requiered paramaters were found
	virtual void check_parameters_found(void)=0;

	///Set error(s)
	Error set_error(const int err_type);


	
};
#endif