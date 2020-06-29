#include "Sys_Headers_Precompiled.h"
//#include "Geo_Interception_Point_List.h"

//constructor
Geo_Interception_Point_List::Geo_Interception_Point_List(void):block_elems(30){
	this->intercept_points=NULL;
	this->number_interception=0;
	this->number_points_alloc=0;
	//count the memory
	Sys_Memory_Count::self()->add_mem(sizeof(Geo_Interception_Point_List), _sys_system_modules::GEOSYS_SYS);
}
//destructor
Geo_Interception_Point_List::~Geo_Interception_Point_List(void){
	if(this->intercept_points!=NULL){
		delete []this->intercept_points;
		this->intercept_points=NULL;
	}
	//count the memory
	Sys_Memory_Count::self()->minus_mem(sizeof(Geo_Interception_Point_List), _sys_system_modules::GEOSYS_SYS);
}
//_____________
//public
//Get the number of interception_points
int Geo_Interception_Point_List::get_number_points(void){
	return this->number_interception;
}
//Get the total point list as a pointer
_geo_multisegment_interception* Geo_Interception_Point_List::get_total_list(void){
	return this->intercept_points;
}
//Get a specific point with the given index
_geo_multisegment_interception Geo_Interception_Point_List::get_point(const int index){
	return this->intercept_points[index];
}
//Change the Index of the geometrical structure, which is intercepted for a given point
void Geo_Interception_Point_List::change_index_is_intercepted(const int index, const int is_intercepted_index){
	this->intercept_points[index].index_is_intercept=is_intercepted_index;
}
//Chane the index of a point if one is hit for the structure which is intercepted 
void Geo_Interception_Point_List::change_point_index_is_intercepted(const int index, const int is_intercepted_point_index){
	//check if a point is hit; if -1 than no point is hit0> it can not be changed
	if(this->intercept_points[index].index_point_is_intercepted!=-1){
		this->intercept_points[index].index_point_is_intercepted=is_intercepted_point_index;
	}
}
//Set a new point (no indefinite interception are taken; it is a point list)
void Geo_Interception_Point_List::set_new_point(_geo_multisegment_interception *new_interception_point){

	//check if there are same points => do not count it; is a junction of segments
	for(int i=0; i < this->number_interception; i++){
		if((*new_interception_point).line_interception.interception_point==this->intercept_points[i].line_interception.interception_point){
			return;
		}
	}
	//no indefinite interception are taken 
	if(new_interception_point->line_interception.indefinite_flag==true){
			return;
	}


	if((this->number_interception<this->number_points_alloc-1) ||(this->number_interception!=0 && this->number_interception % this->block_elems!=0)){

		
		this->intercept_points[this->number_interception]=*new_interception_point;
		this->number_interception++;


	}
	else{
		//allocate a buffer
		_geo_multisegment_interception *buffer=NULL;
		try{
			int full_blocks=(int)((this->number_interception+1)/this->block_elems);
			if(((this->number_interception+1)-full_blocks*this->block_elems)>0){
				full_blocks++;
			}
			buffer=new _geo_multisegment_interception[full_blocks*this->block_elems];
			this->number_points_alloc=full_blocks*this->block_elems;

			//buffer=new _geo_multisegment_interception[this->number_interception+1];
		}
		catch(bad_alloc &){
			Error msg=this->set_error(0);
			throw msg;
		}
		//copy it
		for(int i=0; i< this->number_interception;i++){
			buffer[i]=this->intercept_points[i];
		}
		//delete
		int buff_alloc=this->number_points_alloc;
		this->delete_points();
		//count it
		this->number_interception++;
		this->intercept_points=buffer;
		this->number_points_alloc=buff_alloc;

		buffer[this->number_interception-1]=*new_interception_point;
		

		
	}
}
//Sort the interception points after the distance to the beginning of the structure which intercepts
void Geo_Interception_Point_List::sort_distance(void){
	//make the buffer for sorting
	_geo_multisegment_interception swap;
	int j=0;

	for (int i = 1; i < this->number_interception; i++) {
		swap=this->intercept_points[i];
		//the higher value goes first => ">" ??
		//the smaller value goes first => "<" 
		j = i-1;
		while ( j >= 0 && swap.distance < this->intercept_points[j].distance){
			this->intercept_points[j+1]= this->intercept_points[j];
			j--;
		}
		this->intercept_points[j+1] = swap;
	}

}
//Output members
void Geo_Interception_Point_List::output_members(void){
	//output the elements
	ostringstream prefix1;
	prefix1 << "IC_LIST> ";
	Sys_Common_Output::output_hyd->set_userprefix(prefix1.str());
	ostringstream cout;

	cout <<"Number of points " << this->number_interception<< endl;
	cout <<"No. " << W(8) << "x" <<W(12)<< "y" << W(20) << "Seg_intercept" << W(20) << "Seg_is_intercepted" << W(20)<< "Po_is_intercepted" << W(20) << "distance begin" << label::m  <<endl;
	for(int i=0; i<this->number_interception; i++){
		cout << i << W(14) << P(2) << FORMAT_FIXED_REAL<< this->intercept_points[i].line_interception.interception_point.get_xcoordinate();
		cout << W(14) << P(2) << FORMAT_FIXED_REAL<< this->intercept_points[i].line_interception.interception_point.get_ycoordinate();
		cout << W(14) << this->intercept_points[i].index_intercepts;
		cout << W(14) << this->intercept_points[i].index_is_intercept;
		cout << W(14) << this->intercept_points[i].index_point_is_intercepted;
		cout << W(14) << this->intercept_points[i].distance<< endl;
	}
	Sys_Common_Output::output_hyd->output_txt(&cout);
	//rewind the prefix
	Sys_Common_Output::output_hyd->rewind_userprefix();

}
//Get the first point of the list
_geo_multisegment_interception Geo_Interception_Point_List::get_first_point(void){
	return this->intercept_points[0];
}
//Get the last point of the list
_geo_multisegment_interception Geo_Interception_Point_List::get_last_point(void){
	return this->intercept_points[this->number_interception-1];
}
//Delete the list
void Geo_Interception_Point_List::delete_list(void){
	this->number_interception=0;
	this->delete_points();
}
//Reset the list
void Geo_Interception_Point_List::reset_list(void){
	this->number_interception=0;

}
//Get the maximum y-coordinate
double Geo_Interception_Point_List::get_max_y_coordinate(void){

	double buff_max=0.0;
	for(int i=0; i< this->number_interception; i++){
		if(i==0){
			buff_max=this->intercept_points[i].line_interception.interception_point.get_ycoordinate();
		}
		else if(buff_max<this->intercept_points[i].line_interception.interception_point.get_ycoordinate()){
			buff_max=this->intercept_points[i].line_interception.interception_point.get_ycoordinate();
		}
	}

	return buff_max;
}
//Get the minimum y-coordinate
double Geo_Interception_Point_List::get_min_y_coordinate(void){
	double buff_min=0.0;
	for(int i=0; i< this->number_interception; i++){
		if(i==0){
			buff_min=this->intercept_points[i].line_interception.interception_point.get_ycoordinate();
		}
		else if(buff_min>this->intercept_points[i].line_interception.interception_point.get_ycoordinate()){
			buff_min=this->intercept_points[i].line_interception.interception_point.get_ycoordinate();
		}
	}

	return buff_min;

}
//_____________________
//private
//Delete the alloacated interception points
void Geo_Interception_Point_List::delete_points(void){
	if(this->intercept_points!=NULL){
		delete []this->intercept_points;
		this->intercept_points=NULL;
		this->number_points_alloc=0;
	}
}
//Allocate the points
void Geo_Interception_Point_List::allocate_points(void){

	int full_blocks=(int)(this->number_interception/this->block_elems);
	if((this->number_interception-full_blocks*this->block_elems)>0){
		full_blocks++;
	}

	try{
		this->intercept_points=new _geo_multisegment_interception[full_blocks*this->block_elems];
		this->number_points_alloc=full_blocks*this->block_elems;
	}
	catch(bad_alloc &){
		Error msg=this->set_error(0);
		throw msg;
	}
	//init
	for(int i=0; i< full_blocks*this->block_elems; i++){
		this->intercept_points[i].line_interception.indefinite_flag=false;
		this->intercept_points[i].line_interception.interception_flag=false;
		this->intercept_points[i].line_interception.name_1=label::not_set;
		this->intercept_points[i].line_interception.name_2=label::not_set;
		this->intercept_points[i].index_intercepts=-1;
		this->intercept_points[i].index_is_intercept=-1;
		this->intercept_points[i].index_point_is_intercepted=-1;
	}
}
//set the error
Error Geo_Interception_Point_List::set_error(const int err_type){
	string place="Geo_Interception_Point_List::";
		string help;
		string reason;
		int type=0;
		bool fatal=false;
		stringstream info;
		Error msg;

	switch (err_type){
		case 0://bad alloc
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
