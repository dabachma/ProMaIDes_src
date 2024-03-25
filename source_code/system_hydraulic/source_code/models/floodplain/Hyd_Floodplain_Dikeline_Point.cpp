#include "Hyd_Headers_Precompiled.h"
//#include "Hyd_Floodplain_Dikeline_Point.h"


//Default constructor
Hyd_Floodplain_Dikeline_Point::Hyd_Floodplain_Dikeline_Point(void){

	this->abs_height=0.0;
	this->base_height=0.0;
	this->set_point_name(hyd_label::dikeline_point);
	this->poleni_fac=constant::poleni_const;
	this->overflow_flag=true;
	this->break_flag=false;
	this->break_param.abrupt_fails_flag=false;
	this->break_param.abrupt_opening=0.0;
	this->break_param.resistance=1e10;
	this->distance2begin=-1.0;
	this->point2fpl=false;
	this->fpl_sec_id=-1;

	//count the memory
	Sys_Memory_Count::self()->add_mem(sizeof(Hyd_Floodplain_Dikeline_Point), _sys_system_modules::HYD_SYS);
}
//copy constructor
Hyd_Floodplain_Dikeline_Point::Hyd_Floodplain_Dikeline_Point(const Hyd_Floodplain_Dikeline_Point& object){

	this->abs_height=object.abs_height;
	this->base_height=object.base_height;
	this->poleni_fac=object.poleni_fac;
	this->overflow_flag=object.overflow_flag;
	this->break_flag=object.break_flag;
	this->break_param=object.break_param;
	this->distance2begin=object.distance2begin;
	this->point2fpl=object.point2fpl;
	this->system_id=object.system_id;
	this->fpl_sec_id=object.fpl_sec_id;
	//count the memory
	Sys_Memory_Count::self()->add_mem(sizeof(Hyd_Floodplain_Dikeline_Point), _sys_system_modules::HYD_SYS);
	
}
//Default destructor
Hyd_Floodplain_Dikeline_Point::~Hyd_Floodplain_Dikeline_Point(void){
	//count the memory
	Sys_Memory_Count::self()->minus_mem(sizeof(Hyd_Floodplain_Dikeline_Point), _sys_system_modules::HYD_SYS);
}
//____________
//public
//Set the absolute height of the point (crest)
void Hyd_Floodplain_Dikeline_Point::set_abs_height(const double h){
	this->abs_height=h;
}
//Get the absolute height of the point (crest)
double Hyd_Floodplain_Dikeline_Point::get_abs_height(void){
	return this->abs_height;
}
//Set the absolute height of the point (base)
void Hyd_Floodplain_Dikeline_Point::set_base_height(const double h){
	this->base_height=h;
}
//Get the absolute base height of the point (base)
double Hyd_Floodplain_Dikeline_Point::get_base_height(void){
	return this->base_height;
}
//Set the poleni factor of the point
void Hyd_Floodplain_Dikeline_Point::set_poleni_fac(const double pol){
	this->poleni_fac=pol;
}
//Get the poleni factor of the point
double Hyd_Floodplain_Dikeline_Point::get_poleni_fac(void){
	return this->poleni_fac;
}
//Set the overflow flag of the point
void Hyd_Floodplain_Dikeline_Point::set_overflow_flag(const bool flag){
	this->overflow_flag=flag;
}
//Get the overflow flag of the point
bool Hyd_Floodplain_Dikeline_Point::get_overflow_flag(void){
	return this->overflow_flag;
}
//Set the break flag of the point
void Hyd_Floodplain_Dikeline_Point::set_break_flag(const bool flag){
	this->break_flag=flag;
}
//Get the break flag of the point
bool Hyd_Floodplain_Dikeline_Point::get_break_flag(void){
	return this->break_flag;
}
//Set the break parameters of the point
void Hyd_Floodplain_Dikeline_Point::set_break_param(const _hyd_break_parameters param){
	this->break_param=param;
}
//Get the break parameters of the point
_hyd_break_parameters Hyd_Floodplain_Dikeline_Point::get_break_param(void){
	return this->break_param;
}
//Get a pointer to the break parameters of the point
_hyd_break_parameters* Hyd_Floodplain_Dikeline_Point::get_ptr_break_param(void){
	return &this->break_param;
}
//Check the dikeline point
void Hyd_Floodplain_Dikeline_Point::check_dikeline_point(void){
	//check poleni
	if(this->poleni_fac<0.35 || this->poleni_fac>1.0){
		Warning msg=this->set_warning(0);
		ostringstream info;
		info << "Given poleni factor : "<< this->poleni_fac<<label::no_unit<<endl;
		info << "New poleni factor   : "<< constant::poleni_const<<label::no_unit<<endl;
		info << "Point id            : "<< this->number << endl;
		msg.make_second_info(info.str());
		msg.output_msg(2);
		this->poleni_fac=constant::poleni_const;
	}

	//check break parameters
	if(this->break_param.resistance<0.0){
		Error msg=this->set_error(0);
		ostringstream info;
		info << "Given resistance  : "<< this->break_param.resistance <<endl;
		info << "Point id          : "<< this->number << endl;
		msg.make_second_info(info.str());
		throw msg;

	}
	if(this->break_param.abrupt_opening<0.0){
		Error msg=this->set_error(1);
		ostringstream info;
		info << "Given abrupt opening : "<< this->break_param.abrupt_opening<<label::m <<endl;
		info << "Point id             : "<< this->number << endl;
		msg.make_second_info(info.str());
		throw msg;
	}

	if(this->base_height>this->abs_height){
		Error msg=this->set_error(2);
		ostringstream info;
		info << "Absolute base point height : "<< this->base_height<<label::m <<endl;
		info << "Absolute crest height      : "<< this->abs_height<<label::m <<endl;
		info << "Point id                   : "<< this->number << endl;
		msg.make_second_info(info.str());
		throw msg;

	}
	if(this->distance2begin<0.0){
		Warning msg=this->set_warning(1);
		ostringstream info;
		info << "Distance to begin   : "<< this->distance2begin<<label::m<<endl;
		info << "Point id            : "<< this->number << endl;
		msg.make_second_info(info.str());
		msg.output_msg(2);
		this->distance2begin=0.0;
	}
}
//Set the distance to the beginning of a polysegment from the dikelinie point
void Hyd_Floodplain_Dikeline_Point::set_distance2begin(const double distance){
	this->distance2begin=distance;
}
//Get the distance to the beginning of a polysegment from the dikelinie point
double Hyd_Floodplain_Dikeline_Point::get_distance2begin(void){
	return this->distance2begin;
}
//Get the fpl-section id (if existing )of the dikeline point
int Hyd_Floodplain_Dikeline_Point::get_fpl_section_id(void){
	return this->fpl_sec_id;
}
//Idealise the dikeline points with given parameters of a fpl-section
void Hyd_Floodplain_Dikeline_Point::idealise_points_fpl(_hyd_idealisation_parameters ideal_param,  const int section_id, const bool last_flag){

	//ouput to the fpl-display
	ostringstream cout;
	cout << "Idealise dikeline point " <<this->number<<"..."<<endl;
	Sys_Common_Output::output_fpl->output_txt(&cout);

	//check if it is already idealised
	if(this->point2fpl==true){
		Error msg=this->set_error(3);
		ostringstream info;
		info <<"Point index              : " << this->number<<endl;
		info <<"Station                  : " << this->distance2begin<<label::m << endl;
		info <<"FPL-section index        : " << ideal_param.index << endl;
		info <<"FPL-section name         : " << ideal_param.name << endl;
		msg.make_second_info(info.str());
		throw msg;

	}
	if(this->break_flag==false){
		this->break_flag=true;
	}
	if(this->overflow_flag==false){
		this->overflow_flag=true;
	}

	//check the heights
	if((abs(ideal_param.struct_height-(this->abs_height-this->base_height))/ideal_param.struct_height)>0.5 && this->system_id.area_state==0
		&& this->system_id.measure_nr==0){
		Warning msg=this->set_warning(2);
		ostringstream info;
		info <<"Point index              : " << this->number<<endl;
		info <<"Station                  : " << this->distance2begin<<label::m << endl;
		info <<"Relative height          : " << this->abs_height-this->base_height << label::m << endl;
		info <<"FPL-section index        : " << ideal_param.index << endl;
		info <<"FPL-section name         : " << ideal_param.name << endl;
		info <<"Structure height         : " << ideal_param.struct_height <<label::m<< endl;
		msg.make_second_info(info.str());
		//warning to fpl!!
		msg.output_msg(1);
	}
	//set the ideal height
	this->abs_height=this->base_height+ideal_param.struct_height;

	//check the breach type
	if(this->break_param.abrupt_fails_flag!=ideal_param.break_params.abrupt_fails_flag && this->system_id.area_state==0
		&& this->system_id.measure_nr==0){
		Warning msg=this->set_warning(3);
		ostringstream info;
		info <<"Point index              : " << this->number<<endl;
		info <<"Station                  : " << this->distance2begin<<label::m << endl;
		info <<"Abrupt breach profile    : " << functions::convert_boolean2string(this->break_param.abrupt_fails_flag) << endl;
		info <<"FPL-section index        : " << ideal_param.index << endl;
		info <<"FPL-section name         : " << ideal_param.name << endl;
		info <<"Abrupt breach section    : " << functions::convert_boolean2string(ideal_param.break_params.abrupt_fails_flag) << endl;
		msg.make_second_info(info.str());
		msg.output_msg(1);
	}	
	//set the parameters
	this->break_param=ideal_param.break_params;

	this->point2fpl=true;
	if(last_flag==false){
		this->fpl_sec_id=section_id;
	}
}
//Output the header for the setted member (static)
void Hyd_Floodplain_Dikeline_Point::output_header_setted_member(ostringstream *cout){
	*cout <<W(15) << "Abs. height " <<label::m<< W(15) << "Base height " <<label::m << W(15) << "Poleni Factor "<<label::no_unit<< W(15) <<"Overflow " <<W(15)<< "Break " ;
	*cout << W(15)<< "x" << label::m << W(17) << "y" <<label::m;
	*cout<< endl;
	Sys_Common_Output::output_hyd->output_txt(cout,true);
}
//Output the setted members
void Hyd_Floodplain_Dikeline_Point::output_setted_members(ostringstream *cout){

	
	
	this->overflow_flag=true;
	this->break_flag=false;

	*cout <<W(15) <<P(5)<< this->abs_height << W(15) << this->base_height;
	*cout <<W(15) << this->poleni_fac ;
	*cout <<W(15) << functions::convert_boolean2string(this->overflow_flag);
	*cout <<W(19) <<functions::convert_boolean2string(this->break_flag);
	*cout <<W(21) <<P(2)<< FORMAT_FIXED_REAL << this->x_coordinate ;
	*cout <<W(21) <<P(2)<< FORMAT_FIXED_REAL << this->y_coordinate ;
	*cout<< endl;
	Sys_Common_Output::output_hyd->output_txt(cout,true);	
}

//copy operator
Hyd_Floodplain_Dikeline_Point& Hyd_Floodplain_Dikeline_Point::operator=(const Hyd_Floodplain_Dikeline_Point& object){
	Geo_Point::operator =(object);
	this->abs_height=object.abs_height;
	this->base_height=object.base_height;
	this->poleni_fac=object.poleni_fac;
	this->overflow_flag=object.overflow_flag;
	this->break_flag=object.break_flag;
	this->break_param=object.break_param;
	this->distance2begin=object.distance2begin;
	this->point2fpl=object.point2fpl;
	this->system_id=object.system_id;
	this->fpl_sec_id=object.fpl_sec_id;

	return *this;
}
//___________
//private
//Set warning(s)
Warning Hyd_Floodplain_Dikeline_Point::set_warning(const int warn_type){
		string place="Hyd_Floodplain_Dikeline_Point::";
		string help;
		string reason;
		string reaction;
		int type=0;
		Warning msg;
		stringstream info;

	switch (warn_type){
		case 0://poleni
			place.append("check_dikeline_point(void)");
			reason="The poleni factor is not in a physical interval (0.35-1.0)";
			help="Check the poleni factor";
			reaction="The factor is set to the default factor";
			type=3;
			break;
		case 1://distance
			place.append("check_dikeline_point(void)");
			reason="The distance to the begin of the dikeline is smaller than zero";
			help="Check the distance calculation";
			reaction="The distance is set to zero";
			type=3;
			break;
		case 2://structure height differs from the given fpl-section height
			place.append("idealise_points_fpl(_hyd_idealisation_parameters ideal_param,  const int section_id)");
			reason="The structure height of the FPL-section differs more than 50% of the dikeline height";
			help="Check the height of the dikeline point and the structure height of the FPL-section";
			reaction="The structure height is relevant for a hydraulic coupling";
			type=23;
			break;
		case 3://abrupt fails setting differs from the given fpl-section setting 
			place.append("idealise_points_fpl(_hyd_idealisation_parameters ideal_param,  const int section_id");
			reason="The dikeline point abrupt fail setting differs from the FPL-section settings";
			help="Check the abrupt fails setting of the dikeline point and the FPL-section";
			reaction="The abrupt fail setting of the FPL-section is relevant";
			type=23;
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
};
//Set error(s)
Error Hyd_Floodplain_Dikeline_Point::set_error(const int err_type){
	string place="Hyd_Floodplain_Dikeline_Point::";
		string help;
		string reason;
		int type=0;
		bool fatal=false;
		stringstream info;
		Error msg;

	switch (err_type){
		case 0://break parameter resistance is under 0.0
			place.append("check_dikeline_point(void)");
			reason="The break parameter of the resistance is smaller than 0.0";
			help="Check the break parameters of the dike line point";
			type=3;
			break;
		case 1://break parameter abrupt opening width is under 0.0
			place.append("check_dikeline_point(void)");
			reason="The break parameter of the abrupt opening width is smaller than 0.0";
			help="Check the break parameters of the dike line point";
			type=3;
			break;
		case 2://base height higher than crest height
			place.append("check_dikeline_point(void)");
			reason="The base height is above the absolute crest height";
			help="Check the geometric parameters of the dike line point";
			type=3;
			break;
		case 3://the point is already idealised
			place.append("idealise_points_fpl(_hyd_idealisation_parameters ideal_param,  const int section_id)");
			reason="The dikeline point is already idealised by another FPL-section. The FPL-section are intercepting each other";
			help="Check the hydraulic connection parameters of the FPL-section";
			type=28;
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