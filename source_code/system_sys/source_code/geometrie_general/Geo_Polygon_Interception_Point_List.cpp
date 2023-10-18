#include "Sys_Headers_Precompiled.h"
//#include "Geo_Polygon_Interception_Point_List.h"

//Default constructor
Geo_Polygon_Interception_Point_List::Geo_Polygon_Interception_Point_List(void):block_elems(5){
	this->number=0;
	this->points=NULL;
	this->number_points_alloc=0;
	Sys_Memory_Count::self()->add_mem(sizeof(Geo_Polygon_Interception_Point_List), _sys_system_modules::GEOSYS_SYS);//count the memory

}
//Default destructor
Geo_Polygon_Interception_Point_List::~Geo_Polygon_Interception_Point_List(void){
	this->delete_points();
	Sys_Memory_Count::self()->minus_mem(sizeof(Geo_Polygon_Interception_Point_List), _sys_system_modules::GEOSYS_SYS);//count the memory

}
//_____________________________
//public
//Get the number of points
int Geo_Polygon_Interception_Point_List::get_number_points(void){
	return this->number;
}
//Get the total point list as a pointer
Geo_Polygon_Interception_Point* Geo_Polygon_Interception_Point_List::get_total_list(void){
	return this->points;
}
//Get a specific point with the given index
Geo_Polygon_Interception_Point Geo_Polygon_Interception_Point_List::get_point(const int index){
	return this->points[index];
}
//Get the first point of the list
Geo_Polygon_Interception_Point Geo_Polygon_Interception_Point_List::get_first_point(void){
	return this->points[0];
}
//Get the last point of the list
Geo_Polygon_Interception_Point Geo_Polygon_Interception_Point_List::get_last_point(void){
	return this->points[this->number-1];
}
//Get a pointer to specific point with the given index
Geo_Polygon_Interception_Point *Geo_Polygon_Interception_Point_List::get_ptr_point(const int index){
	if(index<0 || index > this->number){
		return NULL;
	}
	else{
		return &this->points[index];
	}

}
//Get a pointer to the next point with the given index
Geo_Polygon_Interception_Point *Geo_Polygon_Interception_Point_List::get_ptr_next_point(int *index){
	if(*index<0 || *index > this->number){
		return NULL;
	}
	else{
		if(*index+1>=this->number){
			*index=0;
		}
		else{
			(*index)++;
		}
		return &this->points[*index];
	}

}
//Get the number of inside points
int Geo_Polygon_Interception_Point_List::get_number_inside_points(void){
	int counter=0;
	for(int i=0; i< this->number; i++){
		if(this->points[i].get_inside_flag()==true){
			counter++;
		}
	}
	return counter;
}
//Get the number of at boundary points
int Geo_Polygon_Interception_Point_List::get_number_atboundary_points(void){
	int counter=0;
	for(int i=0; i< this->number; i++){
		if(this->points[i].get_polygon_point_flag()==true && this->points[i].get_interception_point_flag()==true){
			counter++;
		}
	}
	return counter;
}
//Get the number of corresponding points
int Geo_Polygon_Interception_Point_List::get_number_corresponding_points(void){
	int counter=0;
	for(int i=0; i< this->number; i++){
		if(this->points[i].get_index_corresponding_point()>=0){
			counter++;
		}
	}
	return counter;
}
//Output members
void Geo_Polygon_Interception_Point_List::output_members(ostringstream *cout){
	*cout <<"Number of points " << this->number<< endl;
	*cout <<"No. " << W(8) << "x" <<W(12)<< "y" <<W(19)<< "Polypoint"<<W(19)<<"Interceptpoint"<<W(10)<<"Inside"<<W(12)<< "Index"<<W(12)<< "Is_used"<<endl;
	for(int i=0; i<this->number; i++){
		*cout << i << W(14) << P(5) << FORMAT_FIXED_REAL<< this->points[i].get_xcoordinate();
		*cout << W(11) << P(5) << FORMAT_FIXED_REAL<< this->points[i].get_ycoordinate();
		*cout << W(14) << functions::convert_boolean2string(this->points[i].get_polygon_point_flag());
		*cout << W(16) << functions::convert_boolean2string(this->points[i].get_interception_point_flag());
		*cout << W(14) << functions::convert_boolean2string(this->points[i].get_inside_flag());
		*cout << W(14) << this->points[i].get_index_corresponding_point();
		*cout << W(14) << functions::convert_boolean2string(this->points[i].get_is_used_flag());
		*cout << endl;
	}

}
//Set a new point
void Geo_Polygon_Interception_Point_List::set_new_point(Geo_Polygon_Interception_Point *new_point){

	//no same points in list
	for(int i=0; i<this->number; i++){
		if(this->points[i].check_same_point(new_point)==true ){
			if(new_point->get_interception_point_flag()==true){
				if(this->points[i].get_interception_point_flag()==false){
					this->points[i].set_point_coordinate(new_point->get_xcoordinate(), new_point->get_ycoordinate());
				}
				this->points[i].set_interception_point_flag(true);	
			}
			if(new_point->get_polygon_point_flag()==true){
				this->points[i].set_polygon_point_flag(true);	
			}
			if(new_point->get_inside_flag()==true){
				this->points[i].set_inside_flag(true);
			}
			return;
		}
	}



	if((this->number<this->number_points_alloc-1) || (this->number!=0 && this->number % this->block_elems!=0)){

		
		this->points[number]=*new_point;
		this->number++;


	}
	else{

		//allocate a buffer
		Geo_Polygon_Interception_Point *buffer=NULL;
		try{
			int full_blocks=(int)((this->number+1)/this->block_elems);
			if(((this->number+1)-full_blocks*this->block_elems)>0){
				full_blocks++;
			}
			buffer=new Geo_Polygon_Interception_Point[full_blocks*this->block_elems];
			this->number_points_alloc=full_blocks*this->block_elems;
			//buffer=new Geo_Polygon_Interception_Point[this->number];
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

		//try{
		//	//reallocate
		//	this->allocate_points();
		//}
		//catch(Error msg){
		//	throw msg;
		//}

		////copy it back
		//for(int i=0; i< this->number-1;i++){
		//	this->points[i]=buffer[i];
		//}
		//set the new one
		this->points[this->number-1]=*new_point;


		//delete the buffer
		//if(buffer!=NULL){
		//	delete []buffer;
		//	buffer=NULL;
		//}
	}

}
//Delete the list
void Geo_Polygon_Interception_Point_List::delete_list(void){
	this->number=0;
	this->delete_points();
}
//Reset the list
void Geo_Polygon_Interception_Point_List::reset_list(void){
	this->number=0;

}
//Set the indices for the corresponding points in the other listand vice versa
void Geo_Polygon_Interception_Point_List::set_corresponding_indices(Geo_Polygon_Interception_Point_List *list){

	for(int i=0; i< this->number; i++){
		if(this->points[i].get_interception_point_flag()==true){
			for(int j=0; j< list->get_number_points(); j++){
				if(*this->get_ptr_point(i)==*list->get_ptr_point(j) && this->get_ptr_point(i)->get_index_corresponding_point()<0 && list->get_ptr_point(j)->get_index_corresponding_point()<0){
					this->points[i].set_index_corresponding_point(j);
					list->get_ptr_point(j)->set_index_corresponding_point(i);
					break;
				}
			}
		}
	}

	//check the list
	for(int i=0; i< this->number; i++){
		if(this->points[i].get_interception_point_flag()==true && this->points[i].get_index_corresponding_point()<0){
			this->points[i].set_interception_point_flag(false);
		}
	}

	for(int i=0; i< list->get_number_points(); i++){
		if(list->get_ptr_point(i)->get_interception_point_flag()==true && list->get_ptr_point(i)->get_index_corresponding_point()<0){
			list->get_ptr_point(i)->set_interception_point_flag(false);
		}
	}
}
//Set the direction of pure interception points
void Geo_Polygon_Interception_Point_List::set_interception_point_direction(_Geo_Polygon *poly){

	int start=-1;
	//find start index
	for(int i=0; i< this->number; i++){
		if(this->points[i].get_polygon_point_flag()==true && this->points[i].get_interception_point_flag()==false){
			start=i;
			break;
		}
	}


	//set the direction
	int counter_total=0;
	int counter=start;
	bool inside=false;
	if(counter>=0){
		//set the starting flag
		if(this->points[counter].get_inside_flag()==true){
			inside=true;
		}
		else{
			inside=false;
		}
	}
	//no start index found
	else{
		for(int i=0; i< this->number; i++){
			int index_next=i;
			Geo_Polygon_Interception_Point *ptr_next=this->get_ptr_next_point(&index_next);
			Geo_Segment buff_seg;
			Geo_Point buff_point;
			buff_seg.set_points(&this->points[i], ptr_next);
			buff_point=buff_seg.calc_mid_point();
			if(poly->check_point_inside(&buff_point)==true){
				this->points[i].set_inside_flag(true);
				inside=true;
				counter=i;
				break;
			}
		}	
	}

	if(counter<0){
		return;
	}

	do{
		counter++;
		if(counter>=this->number){
			counter=0;
		}
		counter_total++;
		//is a pure interception point
		if(this->points[counter].get_polygon_point_flag()==false && this->points[counter].get_interception_point_flag()==true){
			if(inside==true){
				this->points[counter].set_inside_flag(false);
				inside=false;
			}
			else{
				this->points[counter].set_inside_flag(true);
				inside=true;
			}
		}
		//is a polygon point and an interception point
		else if(this->points[counter].get_polygon_point_flag()==true && this->points[counter].get_interception_point_flag()==true){
			int index_next=counter;
			Geo_Polygon_Interception_Point *ptr_next=this->get_ptr_next_point(&index_next);
			//check the next point
			if(ptr_next->get_polygon_point_flag()==true && ptr_next->get_interception_point_flag()==false){
				if(ptr_next->get_inside_flag()==true){
					this->points[counter].set_inside_flag(true);
					inside=true;
				}
				else{
					this->points[counter].set_inside_flag(false);
					inside=false;
				}
			}
			//next point is also not distinct
			else{
				Geo_Segment buff_seg;
				Geo_Point buff_point;
				buff_seg.set_points(&this->points[counter], ptr_next);
				buff_point=buff_seg.calc_mid_point();
				if(poly->check_point_inside(&buff_point)==true){
					this->points[counter].set_inside_flag(true);
					inside=true;
				}
				else{
					//this->points[counter].set_interception_point_flag(false);
					this->points[counter].set_inside_flag(false);
					inside=false;
				}	
			}
		}
		else if(this->points[counter].get_polygon_point_flag()==true && this->points[counter].get_interception_point_flag()==false){
			inside=this->points[counter].get_inside_flag();

		}
			


	}
	while(counter_total < this->number);

}
//____________
//private
//Delete the allocated points
void Geo_Polygon_Interception_Point_List::delete_points(void){
	if(this->points!=NULL){
		delete []this->points;
		this->points=NULL;
		this->number_points_alloc=0;
	}	
}
///Allocate the points
void Geo_Polygon_Interception_Point_List::allocate_points(void){

	int full_blocks=(int)(this->number/this->block_elems);
	if((this->number-full_blocks*this->block_elems)>0){
		full_blocks++;
	}

	try{
		this->points=new Geo_Polygon_Interception_Point[full_blocks*this->block_elems];
		this->number_points_alloc=full_blocks*this->block_elems;
	}
	catch(bad_alloc &){
		Error msg=this->set_error(0);
		throw msg;
	}
}

//Set error(s)
Error Geo_Polygon_Interception_Point_List::set_error(const int err_type){
		string place="Geo_Polygon_Interception_Point_List::";
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