#include "Hyd_Headers_Precompiled.h"
//#include "_Hyd_Coupling_Point_List.h"

//constructor
_Hyd_Coupling_Point_List::_Hyd_Coupling_Point_List(void):block_elems(50), min_distance(10.0){
	this->number=0;
	this->defining_polysegment=NULL;
}
//destructor
_Hyd_Coupling_Point_List::~_Hyd_Coupling_Point_List(void){
	
}
//____________
//public
//Set the defining line
void _Hyd_Coupling_Point_List::set_defining_polysegment(Hyd_Floodplain_Polysegment *polysegment){
	this->defining_polysegment=polysegment;
	
}
//Get a pointer of the defining line
Hyd_Floodplain_Polysegment* _Hyd_Coupling_Point_List::get_defining_polysegment(void){
	return this->defining_polysegment;
}
//Add the relevant coupling points of the defining_polysegment
void _Hyd_Coupling_Point_List::add_relevant_polysegment_points(Hyd_Floodplain_Polygon *fp_boundary){
	Hyd_Floodplain_Polygon *buffer;
	buffer=fp_boundary;
}
//Clone the coupling point list
void _Hyd_Coupling_Point_List::clone_list(_Hyd_Coupling_Point_List *list){
	this->number=list->number;
}
//______________________
//private
//Set error(s)
Error _Hyd_Coupling_Point_List::set_error(const int err_type){
		string place="_Hyd_Coupling_Point_List::";
		string help;
		string reason;
		int type=0;
		bool fatal=false;
		stringstream info;
		Error msg;

	switch (err_type){
		case 0://bad alloc
			place.append("calculate_distances(void)");
			reason="No defining polysegment is set";
			help="Set it before (set_defining_polysegment(Hyd_Floodplain_Polysegment *polysegment)) using this method";
			type=6;
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