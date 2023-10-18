#include "Hyd_Headers_Precompiled.h"
//#include "Hyd_Floodplainraster_Point_List.h"

//constructor
Hyd_Floodplainraster_Point_List::Hyd_Floodplainraster_Point_List(void):block_elems(50){
	this->number_points=0;
	this->points=NULL;
}
//destructor
Hyd_Floodplainraster_Point_List::~Hyd_Floodplainraster_Point_List(void){
	this->delete_points();
}
//___________
//public
//Set a new point in the list
void Hyd_Floodplainraster_Point_List::set_new_point(_hyd_value2pointindex *new_point){


	if(this->number_points!=0 && this->number_points % this->block_elems!=0){

		this->points[number_points]=*new_point;
		this->number_points++;

	}
	else{

		//allocate a buffer
		_hyd_value2pointindex *buffer=NULL;
		try{
			int full_blocks=(int)((this->number_points+1)/this->block_elems);
			if(((this->number_points+1)-full_blocks*this->block_elems)>0){
				full_blocks++;
			}
			buffer=new _hyd_value2pointindex[full_blocks*this->block_elems];
			
		}
        catch(bad_alloc &t){
			Error msg=this->set_error(0);
            ostringstream info;
            info<< "Info bad alloc: " << t.what() << endl;
            msg.make_second_info(info.str());
			throw msg;
		}
		//copy it
		for(int i=0; i< this->number_points;i++){
			buffer[i]=this->points[i];
		}
		//delete
		this->delete_points();
		//count it
		this->number_points++;

		this->points=buffer;
		//set the new one
		this->points[this->number_points-1]=*new_point;
	}
	

	
		
	//allocate a buffer
	//_hyd_value2pointindex *buffer=NULL;
	//try{
	//	buffer=new _hyd_value2pointindex[this->number_points];
	//}
	//catch(bad_alloc &){
	//	Error msg=this->set_error(0);
	//	throw msg;
	//}
	////copy it
	//for(int i=0; i< this->number_points;i++){
	//	buffer[i]=this->points[i];
	//}
	////delete
	//this->delete_points();
	////count it
	//this->number_points++;

	//try{
	//	//reallocate
	//	this->allocate_points();
	//}
	//catch(Error msg){
	//	throw msg;
	//}

	////copy it back
	//for(int i=0; i< this->number_points-1;i++){
	//	this->points[i]=buffer[i];
	//}
	////set the new one
	//this->points[this->number_points-1]=*new_point;


	////delete the buffer
	//if(buffer!=NULL){
	//	delete []buffer;
	//	buffer=NULL;
	//}
	
}
//Get the total point list as a pointer
_hyd_value2pointindex* Hyd_Floodplainraster_Point_List::get_total_list(void){
	return this->points;
}
//Get a point with a given index
_hyd_value2pointindex Hyd_Floodplainraster_Point_List::get_point(const int index){
	return this->points[index];
}
//Get the number of points stored in the list
int Hyd_Floodplainraster_Point_List::get_number_points(void){
	return this->number_points;
}
//Delete the list
void Hyd_Floodplainraster_Point_List::delete_list(void){
	this->number_points=0;
	this->delete_points();
}
//Output members
void Hyd_Floodplainraster_Point_List::output_members(void){
	ostringstream prefix1;
	prefix1 << "RASTP_LIST> ";
	Sys_Common_Output::output_hyd->set_userprefix(prefix1.str());
	ostringstream cout;

	cout <<"Number of points " << this->number_points<< endl;
	cout <<" Index " << W(8) << " value_h " <<W(8)<<" value_pol "<<W(8)<<  " Flag "  <<endl;
	for(int i=0; i<this->number_points; i++){
		cout << this->points[i].index ;
		cout << W(8) << P(2) << FORMAT_FIXED_REAL<< this->points[i].value_h;
		cout << W(8) << P(2) << FORMAT_FIXED_REAL<< this->points[i].value_pol;
		cout << W(8) << functions::convert_boolean2string(this->points[i].flag);
		cout << endl;
	}
	Sys_Common_Output::output_hyd->output_txt(&cout);
	//rewind the prefix
	Sys_Common_Output::output_hyd->rewind_userprefix();

}
//__________
//private
//Delete the points
void Hyd_Floodplainraster_Point_List::delete_points(void){
	if(this->points!=NULL){
		delete [] this->points;
		this->points=NULL;
	}
}
//Allocate the points
void Hyd_Floodplainraster_Point_List::allocate_points(void){
	try{
		this->points=new _hyd_value2pointindex[this->number_points];

	}
	catch(bad_alloc &t){
		Error msg=this->set_error(0);
		ostringstream info;
		info<< "Info bad alloc: " << t.what() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
	for(int i=0; i< this->number_points;i++){
		this->points[i].flag=false;
		this->points[i].index=-1;
		this->points[i].value_h=0.0;
		this->points[i].value_pol=constant::poleni_const;
	}
}
//Set error(s)
Error Hyd_Floodplainraster_Point_List::set_error(const int err_type){
	string place="Hyd_Floodplainraster_Point_List::";
		string help;
		string reason;
		int type=0;
		bool fatal=false;
		stringstream info;
		Error msg;

	switch (err_type){
		case 1://bad alloc
			place.append("allocate_points(void)/set_new_point(_geo_interception_point *new_interception_point)");
			reason="Can not allocate the memory";
			help="Check the memory";
			type=10;
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
