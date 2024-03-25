#include "Hyd_Headers_Precompiled.h"
//#include "Hyd_Floodplain_Dikeline_Point_List.h"

//constructor
Hyd_Floodplain_Dikeline_Point_List::Hyd_Floodplain_Dikeline_Point_List(void):block_elems(50){

	this->points=NULL;
	this->number=0;
	//count the memory
	Sys_Memory_Count::self()->add_mem(sizeof(Hyd_Floodplain_Dikeline_Point_List), _sys_system_modules::HYD_SYS);
}
//destructor
Hyd_Floodplain_Dikeline_Point_List::~Hyd_Floodplain_Dikeline_Point_List(void){
	this->delete_points();
	//count the memory
	Sys_Memory_Count::self()->minus_mem(sizeof(Hyd_Floodplain_Dikeline_Point_List), _sys_system_modules::HYD_SYS);
}
//____________
//public
//Get the number of points
int Hyd_Floodplain_Dikeline_Point_List::get_number_points(void){
	return this->number;
}
//Get the total list as a pointer
Hyd_Floodplain_Dikeline_Point_List* Hyd_Floodplain_Dikeline_Point_List::get_total_list(void){
	return this;
}
//Get a specific  points with the given index
Hyd_Floodplain_Dikeline_Point Hyd_Floodplain_Dikeline_Point_List::get_point(const int index){
	return this->points[index];
}
//Get a specific point pointer with the given index
Hyd_Floodplain_Dikeline_Point* Hyd_Floodplain_Dikeline_Point_List::get_ptr_point(const int index){
	return &(this->points[index]);
}
//Set a new points
void Hyd_Floodplain_Dikeline_Point_List::set_new_point(Hyd_Floodplain_Dikeline_Point *new_point){
	//check for geometrical same points=> do not count it;
	for(int i=0; i < this->number; i++){
		if( (*new_point)==this->points[i]){	

			if(this->points[i].get_abs_height()<=-99.0){
				this->points[i].set_abs_height(new_point->get_abs_height());
			}
			return;
		}
	}

	if(this->number!=0 && this->number % this->block_elems!=0){

		this->points[number]=*new_point;
		this->number++;

	}
	else{
		//allocate a buffer
		Hyd_Floodplain_Dikeline_Point *buffer=NULL;
		try{
			int full_blocks=(int)((this->number+1)/this->block_elems);
			if(((this->number+1)-full_blocks*this->block_elems)>0){
				full_blocks++;
			}
			buffer=new Hyd_Floodplain_Dikeline_Point[full_blocks*this->block_elems];
			
		}
		catch(bad_alloc &){
			Error msg=this->set_error(0);
			throw msg;
		}
		//copy it
		for(int i=0; i< this->number;i++){
			buffer[i]=this->points[i];
		}
		//delete
		this->delete_points();
		//count it
		this->number++;

		this->points=buffer;
		//set the new one
		this->points[this->number-1]=*new_point;
	}
	
}
//Delete the list
void Hyd_Floodplain_Dikeline_Point_List::delete_list(void){
	this->number=0;
	this->delete_points();

}
//Output the setted members
void Hyd_Floodplain_Dikeline_Point_List::output_setted_members(ostringstream *cout){
	*cout << "No.";
	Hyd_Floodplain_Dikeline_Point::output_header_setted_member(cout);
	for(int i=0; i<this->number; i++){
		*cout << i;
		this->points[i].output_setted_members(cout);
	}
}
//Use offset of coordinates
void Hyd_Floodplain_Dikeline_Point_List::use_offset_coordinates(const double x_off, const double y_off){
	for(int i=0; i< this->number; i++){
		this->points[i].set_point_coordinate(this->points[i].get_xcoordinate()+x_off,this->points[i].get_ycoordinate()+y_off);
	}
}
//____________
//private
///Delete the allocated points
void Hyd_Floodplain_Dikeline_Point_List::delete_points(void){
	if(this->points!=NULL){
		delete []this->points;
		this->points=NULL;
	}
}
///Set error(s)
Error Hyd_Floodplain_Dikeline_Point_List::set_error(const int err_type){
		string place="Hyd_Floodplain_Dikeline_Point_List::";
		string help;
		string reason;
		int type=0;
		bool fatal=false;
		stringstream info;
		Error msg;

	switch (err_type){
		case 0://bad alloc
			place.append("set_new_point(Hyd_Floodplain_Dikeline_Point *new_point)");
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