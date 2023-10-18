#include "Hyd_Headers_Precompiled.h"
//#include "_Hyd_Coupling_Structure.h"

//constructor
_Hyd_Coupling_Structure::_Hyd_Coupling_Structure(void): discharge_boundary(1e-10){	
	this->global_h_sill=0.0;
	this->width=0.0;
	this->controlled_flag=false;
	this->discharge=0.0;
	this->coupling_v=0.0;
	this->control_curve_is_found=false;
	this->local_h_sill=0.0;
	this->old_discharge=0.0;
}
//destructor
_Hyd_Coupling_Structure::~_Hyd_Coupling_Structure(void){
}
//________________
//public
//Set the global sill height
void _Hyd_Coupling_Structure::set_global_sill_height(double max_z_min){
	this->global_h_sill=this->local_h_sill+max_z_min;
}
//Set the coupling structure member with a structure where the parser has gathered the informations (_hyd_coupling_structure_data)
void _Hyd_Coupling_Structure::set_coupling_structure_data(_hyd_coupling_structure_data data){
	this->controlled_flag=data.controlled_flag;
	//development error remove it after implementation
	if(this->controlled_flag==true){
		Error msg=this->set_error(3);
		throw msg;
	}
	this->width=data.width;
}
//(static) Convert a string to _1d_outflow_types of the coupling structure
_hyd_1d_outflow_types _Hyd_Coupling_Structure::convert_string2coupling_type(string type){

	_hyd_1d_outflow_types choosen_type;
	//convert string to lower letters
	_Hyd_Parse_IO::string2lower(&type);
	_Hyd_Parse_IO::erase_leading_whitespace_tabs(&type);
	_Hyd_Parse_IO::erase_end_whitespace_tabs(&type);

	if(type==hyd_label::weir_coupling){
		choosen_type=_hyd_1d_outflow_types::WEIR_OUT;	
	}
	else if(type==hyd_label::gate_coupling){
		choosen_type=_hyd_1d_outflow_types::GATE_OUT;
	}
	else{
		choosen_type=_hyd_1d_outflow_types::UNKNOWN_OUT;
	}
	return choosen_type;
}
//(static) Convert a _1d_outflow_types of the coupling structure to a string
string _Hyd_Coupling_Structure::convert_coupling_type2string(_hyd_1d_outflow_types type){
	string txt;
	switch(type){
		case (_hyd_1d_outflow_types::WEIR_OUT):
			txt=hyd_label::weir_coupling;
			break;
		case (_hyd_1d_outflow_types::GATE_OUT):
			txt=hyd_label::gate_coupling;
			break;
		default:
			txt=label::unknown_type;

	}
 return txt;
}
//Reset the coupling structure
void _Hyd_Coupling_Structure::reset_structure(void){
	this->coupling_v=0.0;
	this->old_discharge=0.0;
}
//Get if the control curve is found
bool _Hyd_Coupling_Structure::get_control_curve_is_found(void){
	return this->control_curve_is_found;
}
//Get the width of the structure
double _Hyd_Coupling_Structure::get_width(void){
	return this->width;
}
//Clone the coupling structure
void _Hyd_Coupling_Structure::clone_coupling_structure(_Hyd_Coupling_Structure *structure){
	this->global_h_sill=structure->global_h_sill;
	this->width=structure->width;
	this->controlled_flag=structure->controlled_flag;
	this->discharge=structure->discharge;
	this->coupling_v=structure->coupling_v;
	this->control_curve_is_found=structure->control_curve_is_found;
	this->local_h_sill=structure->local_h_sill;
	this->old_discharge=structure->old_discharge;
}
//____________________
//protected
//Allocate the control function
void _Hyd_Coupling_Structure::allocate_control_function(int const ){
	if(this->controlled_flag==true){

		//allocate here the control function
		try{

		}
		catch(bad_alloc &){
			Error msg=this->set_error(0);
			throw msg;
		}
	}
}
//Smooth the coupling discharge with the coupling discharge calculated one internal timestep before
double _Hyd_Coupling_Structure::smooth_coupling_discharge(const double q_buff){
	double smooth_q=0.0;
	smooth_q=q_buff*(1.0-constant::smoothing_coupling_factor)+this->old_discharge*constant::smoothing_coupling_factor;
	//smooth_q = q_buff * (1.0 - 0.7) + this->old_discharge*0.3;
	smooth_q=q_buff;
	this->old_discharge=q_buff;
	return smooth_q;
}

//check the parameters
void _Hyd_Coupling_Structure::check_parameters(void){
	if(this->width<0.0){
		Error msg=this->set_error(1);
		ostringstream info;
		info << "Given width is " << this->width << label::m << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
	if(this->local_h_sill<0.05){
		Error msg=this->set_error(2);
		ostringstream info;
		info << "Given local sill height is " << this->local_h_sill << label::m << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
}
//Set error(s)
Error _Hyd_Coupling_Structure::set_error(const int err_type){
	string place="_Hyd_Coupling_Structure::";
		string help;
		string reason;
		int type=0;
		bool fatal=false;
		stringstream info;
		Error msg;

	switch (err_type){
		case 0://bad alloc
			place.append("allocate_control_function(int const index)");
			reason="Can not allocate the memory";
			help="Check the memory";
			type=10;
			break;
		case 1://width is below 0.0
			place.append("check_parameters(void)");
			reason="The width of the structure is < 0.0 m";
			help="Check the coupling structure settings";
			type=3;
			break;
		case 2://the local sill height is below 0.0
			place.append("check_parameters(void)");
			reason="The local sill height of the structure is < 0.05 m";
			help="Check the coupling structure settings";
			type=3;
			break;
		case 3://development error 
			place.append("set_coupling_structure_data(_hyd_coupling_structure_data data)");
			reason="The controllable structure is not yet implemented";
			help="Structures must be not controllable";
			info <<"Remove this error after implementation"<< endl;
			type=19;
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
