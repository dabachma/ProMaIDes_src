#include "Sys_Headers_Precompiled.h"
//#include "Geo_Point_List.h"

//constructor
Geo_Point_List::Geo_Point_List(void):block_elems(5){
	this->number=0;
	this->points=NULL;
	this->double_allowed=false;
	this->number_points_alloc=0;
	//count the memory
	Sys_Memory_Count::self()->add_mem(sizeof(Geo_Point_List), _sys_system_modules::GEOSYS_SYS);
}
//destructor
Geo_Point_List::~Geo_Point_List(void){
	this->delete_points();
	//count the memory
	Sys_Memory_Count::self()->minus_mem(sizeof(Geo_Point_List), _sys_system_modules::GEOSYS_SYS);
} 
//___________
//public
//Get the number of points
int Geo_Point_List::get_number_points(void){
	return this->number;
}
//Get the total point list as a pointer
Geo_Point* Geo_Point_List::get_total_list(void){
	return this->points;
}
//Get a specific point with the given index
Geo_Point Geo_Point_List::get_point(const int index){
	return this->points[index];
}
//Get the first point of the list
Geo_Point Geo_Point_List::get_first_point(void){
	return this->points[0];
}
//Get the point with the maximum y-coordinate
Geo_Point *Geo_Point_List::get_ptr_point_max_y(void){
	double y_max=-9999.0;
	int max_index=-1;
	for(int i=0; i< this->number; i++){
		if(i==0){
			y_max=this->points[i].get_ycoordinate();
			max_index=i;
		}
		else{
			if(this->points[i].get_ycoordinate()>y_max){
				y_max=this->points[i].get_ycoordinate();
				max_index=i;
			}
		}
	}

	return this->get_ptr_point(max_index);
}
//Get the last point of the list
Geo_Point Geo_Point_List::get_last_point(void){
	return this->points[this->number-1];
}
//Get a pointer to specific point with the given index
Geo_Point *Geo_Point_List::get_ptr_point(const int index){
	if(index<0 || index > this->number){
		return NULL;
	}
	else{
		return &this->points[index];
	}

}
//Output members
void Geo_Point_List::output_members(ostringstream *cout){
	*cout <<"Number of points " << this->number<< endl;
	*cout <<"No. " << W(8) << "x" <<W(12)<< "y" <<endl;
	for(int i=0; i<this->number; i++){
		*cout << i << W(14) << P(5) << FORMAT_FIXED_REAL<< this->points[i].get_xcoordinate();
		*cout << W(14) << P(5) << FORMAT_FIXED_REAL<< this->points[i].get_ycoordinate();
		*cout << endl;
	}

}
//Set a new point
void Geo_Point_List::set_new_point(Geo_Point *new_point){

	//no double points allowed
	if(this->double_allowed==false){
		for(int i=0; i < this->number; i++){
			if( (*new_point)==this->points[i]){
				return;
			}
		}
	}
	else{
		//check if the point is the same than the last=> do not count it; is a junction of segments
		if( this->number>0 && (*new_point)==this->points[this->number-1]){
			return;
		}
	}

	if((this->number<this->number_points_alloc-1) ||(this->number!=0 && this->number % this->block_elems!=0)){

		
		this->points[this->number]=*new_point;
		this->number++;


	}
	else{

		//allocate a buffer
		Geo_Point *buffer=NULL;
		try{
			int full_blocks=(int)((this->number+1)/this->block_elems);
			if(((this->number+1)-full_blocks*this->block_elems)>0){
				full_blocks++;
			}
			buffer=new Geo_Point[full_blocks*this->block_elems];
			this->number_points_alloc=full_blocks*this->block_elems;
			//buffer=new Geo_Point[this->number];
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
		int buff_alloc=this->number_points_alloc;
		this->delete_points();
		//count it
		this->number++;

		this->points=buffer;
		this->number_points_alloc=buff_alloc;

		//set the new one
		this->points[this->number-1]=*new_point;
	}
}
//Set a new points with a polygon
void Geo_Point_List::set_new_points(_Geo_Polygon *new_points){
	for(int i=0; i< new_points->get_number_segments(); i++){
		this->set_new_point(&new_points->get_segment(i)->point1);
	}
}
//Set a new points with a polysegment
void Geo_Point_List::set_new_points(Geo_Polysegment *new_points){
	for(int i=0; i< new_points->get_number_segments(); i++){
		this->set_new_point(&new_points->get_segment(i)->point1);
	}
	//last point
	if(new_points->get_number_segments()>0){
		this->set_new_point(&new_points->get_segment(new_points->get_number_segments()-1)->point2);
	}
}
//Get the index of the point with the smallest y-coordinate in the list
int Geo_Point_List::get_index_smallest_y(void){
	double y_min=0.0;
	int min_index=-1;
	for(int i=0; i< this->number; i++){
		if(i==0){
			y_min=this->points[i].get_ycoordinate();
			min_index=i;
		}
		else{
			if(this->points[i].get_ycoordinate()<y_min){
				y_min=this->points[i].get_ycoordinate();
				min_index=i;
			}
		}
	}

	return min_index;
}
//Get the the smallest y-coordinate in the list
double Geo_Point_List::get_smallest_y(void){
	double y_min=0.0;
	int min_index=-1;
	for(int i=0; i< this->number; i++){
		if(i==0){
			y_min=this->points[i].get_ycoordinate();
			min_index=i;
		}
		else{
			if(this->points[i].get_ycoordinate()<y_min){
				y_min=this->points[i].get_ycoordinate();
				min_index=i;
			}
		}
	}

	return y_min;
}
//Delete the list
void Geo_Point_List::delete_list(void){
	this->number=0;
	this->delete_points();
}
//Reset the list
void Geo_Point_List::reset_list(void){
	this->number=0;

}
//Sort the point list after the x-coordinates of the points 
void Geo_Point_List::sort_list_x_coordinates(const bool ascending){


	//make the buffer for sorting
	Geo_Point swap;
	int j=0;

	for (int i = 1; i < this->number; i++) {
		swap=points[i];
		//the higher value goes first => ">" ??
		//the smaller value goes first => "<" 
		j = i-1;
		if(ascending==true){
			while ( j >= 0 && swap.get_xcoordinate() < this->points[j].get_xcoordinate()){
				this->points[j+1]=this->points[j];
				j--;
			}
		}
		else{
			while ( j >= 0 && swap.get_xcoordinate() > this->points[j].get_xcoordinate()){
				this->points[j+1]=this->points[j];
				j--;
			}
		}
		this->points[j+1]=swap;
	}

}
//Set the flag if double points are allowed
void Geo_Point_List::set_double_point_allowed(const bool flag){
	this->double_allowed=flag;
}
//Calculate area
double Geo_Point_List::calculate_area(void){
	double area=0.0;

	if(this->number<3){
		return 0.0;
	}

	for(int i=0; i<this->number-1; i++){
		area=area+(this->points[i].get_xcoordinate()-this->points[i+1].get_xcoordinate())*(this->points[i].get_ycoordinate()+this->points[i+1].get_ycoordinate());
	}
	area = area + (this->points[this->number-1].get_xcoordinate()-this->points[0].get_xcoordinate())*(this->points[this->number-1].get_ycoordinate()+this->points[0].get_ycoordinate());

	return abs(area/2.0);

}

//____________
//private
//Delete the allocated points
void Geo_Point_List::delete_points(void){
	if(this->points!=NULL){
		delete []this->points;
		this->points=NULL;
		this->number_points_alloc=0;
	}	
}
///Allocate the points
void Geo_Point_List::allocate_points(void){

	int full_blocks=(int)(this->number/this->block_elems);
	if((this->number-full_blocks*this->block_elems)>0){
		full_blocks++;
	}

	try{
		this->points=new Geo_Point[full_blocks*this->block_elems];
		this->number_points_alloc=full_blocks*this->block_elems;
	}
	catch(bad_alloc &){
		Error msg=this->set_error(0);
		throw msg;
	}
}

//Set error(s)
Error Geo_Point_List::set_error(const int err_type){
		string place="Geo_Point_List::";
		string help;
		string reason;
		int type=0;
		bool fatal=false;
		stringstream info;
		Error msg;

	switch (err_type){
		case 0://bad alloc
			place.append("allocate_points(void)/set_new_point(Geo_point *new_point)");
			reason="Can not allocate the memory";
			help="Check the memory";
			type=10;
			break;
		case 1://bad alloc
			place.append("set_enclosing_polygon_points(Geo_Point_List *outer_list)");
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