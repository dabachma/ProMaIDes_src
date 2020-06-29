//#include "Fpl_Cub_Dike_Crest.h"
#include "Fpl_Headers_Precompiled.h"

//Default constructor
Fpl_Cub_Dike_Crest::Fpl_Cub_Dike_Crest(void){
	Sys_Memory_Count::self()->add_mem(sizeof(Fpl_Cub_Dike_Crest)-sizeof(Geo_Segment), _sys_system_modules::FPL_SYS);//count the memory
}
//Default destructor
Fpl_Cub_Dike_Crest::~Fpl_Cub_Dike_Crest(void){
	Sys_Memory_Count::self()->minus_mem(sizeof(Fpl_Cub_Dike_Crest)-sizeof(Geo_Segment), _sys_system_modules::FPL_SYS);//count the memory
}
//_____________
//public
//Set start- and the end-point of the crest to generate the segment
void Fpl_Cub_Dike_Crest::set_segment(Geo_Point *crestwater_point, Geo_Point *crestland_point){
	this->crest_segment.set_points(crestwater_point, crestland_point);
	if(crestwater_point->check_left_of(crestland_point)==false){
		Error msg=this->set_error(1);
		throw msg;
	}
	try{
		//check the gradient of the segment
		this->check_crestsegment();
	}
	catch(Error msg){
		throw msg;
	}

}//______________
//Get the crest width
double Fpl_Cub_Dike_Crest::get_crest_width(void){
	return this->crest_segment.get_distance();
}
//Get crest height
double Fpl_Cub_Dike_Crest::get_crest_height(void){
	return this->crest_segment.point1.get_ycoordinate();
}
//Get the x coordiante of the crest mid point
double Fpl_Cub_Dike_Crest::get_crest_mid_x(void){
	return 0.5*(this->crest_segment.point1.get_xcoordinate()+this->crest_segment.point2.get_xcoordinate());
}
//Output the geometrie of the dike crest to display/console
void Fpl_Cub_Dike_Crest::output_geometrie(void){
	ostringstream cout;
	ostringstream prefix;
	prefix << "CREST> ";
	Sys_Common_Output::output_fpl->set_userprefix(&prefix);
	this->crest_segment.output_members(&cout);
	Sys_Common_Output::output_fpl->output_txt(&cout,true);
	Sys_Common_Output::output_fpl->rewind_userprefix();
}
//Get the number of segments
int Fpl_Cub_Dike_Crest::get_number_segments(void){
	return 1;
}
//Get the dike crest segment
Geo_Segment* Fpl_Cub_Dike_Crest::get_segment(void){
	return &this->crest_segment;
}
//_____________
//private
//Check the dike crest for gradient (max grad=1.0/19.0)
void Fpl_Cub_Dike_Crest::check_crestsegment(void){
	if(abs(this->crest_segment.get_gradient())>=1.0/19.0){
		Error msg=this->set_error(0);
		throw msg;
	}
}
//Set the error(s)
Error Fpl_Cub_Dike_Crest::set_error(const int err_type){
	string place="Fpl_Cub_Dike_Crest::";
		string help;
		string reason;
		int type=0;
		bool fatal=false;
		stringstream info;
		Error msg;

	switch (err_type){
		case 0://do not find any interception point
			place.append("check_crestsegment(void)");
			reason="The absolute value of the crest's gradient is greater than 1:19; it is not corresponding to the weir formula";
			help="Check the waterside and landside crest point";
			type=11;
			break;
		case 1://do not find any interception point
			place.append("set_segment(Geo_Point *crestwater_point, Geo_Point *crestland_point)");
			reason="The landside crest point is left of the water side crest point";
			help="Check the waterside and landside crest point";
			type=11;
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