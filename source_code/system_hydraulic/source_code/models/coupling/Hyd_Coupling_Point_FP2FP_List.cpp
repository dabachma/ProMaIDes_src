#include "Hyd_Headers_Precompiled.h"
//#include "Hyd_Coupling_Point_FP2FP_List.h"

//constructor
Hyd_Coupling_Point_FP2FP_List::Hyd_Coupling_Point_FP2FP_List(void){
	this->number=0;
	this->points=NULL;

	//count the memory
	Sys_Memory_Count::self()->add_mem(sizeof(Hyd_Coupling_Point_FP2FP_List), _sys_system_modules::HYD_SYS);
}
//destructor
Hyd_Coupling_Point_FP2FP_List::~Hyd_Coupling_Point_FP2FP_List(void){
	this->delete_list();
	//count the memory
	Sys_Memory_Count::self()->minus_mem(sizeof(Hyd_Coupling_Point_FP2FP_List), _sys_system_modules::HYD_SYS);
}
//_____________
//public
//Get the number of coupling points
int Hyd_Coupling_Point_FP2FP_List::get_number_couplings(void){
	return this->number;
}
//Get the total list as a pointer
Hyd_Coupling_Point_FP2FP* Hyd_Coupling_Point_FP2FP_List::get_total_list(void){
	return this->points;
}
//Get a specific coupling points with the given index
Hyd_Coupling_Point_FP2FP Hyd_Coupling_Point_FP2FP_List::get_couplingpoint(const int index){
	return this->points[index];
}
//Get a specific coupling points pointer with the given index
Hyd_Coupling_Point_FP2FP* Hyd_Coupling_Point_FP2FP_List::get_ptr_couplingpoint(const int index){
	return &(this->points[index]);
}
//Get the first coupling points of the list
Hyd_Coupling_Point_FP2FP Hyd_Coupling_Point_FP2FP_List::get_first_couplingpoint(void){
	return this->points[0];
}
//Get the last coupling points of the list
Hyd_Coupling_Point_FP2FP Hyd_Coupling_Point_FP2FP_List::get_last_couplingpoint(void){
	return this->points[this->number-1];
}
//Output the setted members
void Hyd_Coupling_Point_FP2FP_List::output_setted_members(ostringstream *cout){
	*cout << "No.";
	Hyd_Coupling_Point_FP2FP::output_header_setted_member(cout);
	for(int i=0; i<this->number; i++){
		*cout << i;
		this->points[i].output_setted_members(cout);
	}
}
//Set a new coupling points
void Hyd_Coupling_Point_FP2FP_List::set_new_couplingpoint(Hyd_Coupling_Point_FP2FP *new_point){
	//check for geometrical same points=> do not count it; is a junction of segments
	for(int i=0; i < this->number; i++){
		if( (*new_point)==this->points[i]){
			//transfer the information of the intersected elements
			if(new_point->fp1_elem_is_setted==true){
				this->points[i].set_first_fpelem_index(new_point->fp1_elem_index);
				this->points[i].set_first_fpelem_pointer(new_point->fp1_elem);
				this->points[i].set_direction_coupling(new_point->x_width_flag);
			}
			if(new_point->fp2_elem_is_setted==true){
				this->points[i].set_second_fpelem_index(new_point->fp2_elem_index);
				this->points[i].set_second_fpelem_pointer(new_point->fp2_elem);
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
		Hyd_Coupling_Point_FP2FP *buffer=NULL;
		try{
			int full_blocks=(int)((this->number+1)/this->block_elems);
			if(((this->number+1)-full_blocks*this->block_elems)>0){
				full_blocks++;
			}
			buffer=new Hyd_Coupling_Point_FP2FP[full_blocks*this->block_elems];
			//buffer=new Hyd_Coupling_Point_FP2FP[this->number];
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
		this->delete_couplingpoints();
		//count it
		this->number++;

		this->points=buffer;

		//try{
		//	//reallocate
		//	this->allocate_couplingpoints();
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


		////delete the buffer
		//if(buffer!=NULL){
		//	delete []buffer;
		//	buffer=NULL;
		//}
	}
}
//Delete the list
void Hyd_Coupling_Point_FP2FP_List::delete_list(void){
	this->delete_couplingpoints();
	this->number=0;
}
//Sort the points after the distance to the beginning of the defining polysegment
void Hyd_Coupling_Point_FP2FP_List::sort_distance_along_polysegment(void){
	this->calculate_distances();
	//make the buffer for sorting
	Hyd_Coupling_Point_FP2FP swap;
	int j=0;

	for (int i = 1; i < this->number; i++) {
		swap=this->points[i];
		//the higher value goes first => ">" ??
		//the smaller value goes first => "<" 
		j = i-1;
		while ( j >= 0 && swap.get_total_distance_along_polysegment() < this->points[j].get_total_distance_along_polysegment()){
			this->points[j+1]= this->points[j];
			j--;
		}
		this->points[j+1] = swap;
	}

	//set the point index
	for(int i=0; i< this->number; i++){
		this->points[i].set_point_index(i);
	}

}
//Calculate for all points the distance to the next upward point and downward point
void Hyd_Coupling_Point_FP2FP_List::calculate_alldistance_up_down(void){
	
	if(this->number>1){
		for(int i=0; i<this->number;i++){
			//begin
			if(i==0){
				this->points[i].set_distance_down(0.0);
				this->points[i].set_distance_up(&(this->points[i+1]));
			}
			//end
			else if(i==this->number-1){
				this->points[i].set_distance_down(&(this->points[i-1]));
				this->points[i].set_distance_up(0.0);
			}
			//inbetween
			else{
				this->points[i].set_distance_down(&(this->points[i-1]));
				this->points[i].set_distance_up(&(this->points[i+1]));
			}
		}
		//if the floodplain 1 is inside floodplain 2
		if(this->defining_polysegment->get_is_closed()==true){
			this->points[0].set_distance_down(&(this->points[this->number-1]));
			this->points[this->number-1].set_distance_up(&(this->points[0]));
		}
	}
	else if(this->number==1){
			this->points[0].set_distance_down(0.0);
			this->points[0].set_distance_up(0.0);
	}

}
//Complete the coupling points with the coupled element indices
void Hyd_Coupling_Point_FP2FP_List::complete_couplingpoints(Hyd_Model_Floodplain *fp1, Hyd_Model_Floodplain *fp2){

	int point_no=-1;
	//complete the points with the first floodplain elements
	for(int i=this->number-1; i>=0; i--){
		if(this->points[i].fp1_elem_is_setted==true){
			point_no=i;
		}
		else{
			if(point_no>=0 && this->points[i].fp1_elem_is_setted==false){
				this->points[i].set_first_fpelem_index(this->points[point_no].fp1_elem_index);
				this->points[i].set_first_fpelem_pointer(this->points[point_no].fp1_elem);
				this->points[i].set_direction_coupling(this->points[point_no].x_width_flag);
			}
		}
	}
	//complete the points with the second floodplain elements
	point_no=-1;
	for(int i=this->number-1; i>=0; i--){
		if(this->points[i].fp2_elem_is_setted==true){
			point_no=i;
		}
		else{
			if(point_no>=0 && this->points[i].fp2_elem_is_setted==false){
				this->points[i].set_second_fpelem_index(this->points[point_no].fp2_elem_index);
				this->points[i].set_second_fpelem_pointer(this->points[point_no].fp2_elem);
			}
		}
	}
	//if the floodplain 1 is inside floodplain 2
	if(this->defining_polysegment->get_is_closed()==true){
		this->points[this->number-1].set_second_fpelem_index(this->points[0].fp2_elem_index);
		this->points[this->number-1].set_second_fpelem_pointer(this->points[0].fp2_elem);
	}
	//check if the points are in both floodplains
	for(int i=0; i<this->number-1;i++){
		if(fp1->raster.geometrical_bound.check_point_outside(&(this->points[i]))==true || fp2->raster.geometrical_bound.check_point_outside(&(this->points[i]))==true){
			this->points[i].set_first_fpelem_index(-1);
			this->points[i].set_first_fpelem_pointer(NULL);
			this->points[i].set_second_fpelem_index(-1);
			this->points[i].set_second_fpelem_pointer(NULL);
			this->points[i+1].set_first_fpelem_index(-1);
			this->points[i+1].set_first_fpelem_pointer(NULL);
			this->points[i+1].set_second_fpelem_index(-1);
			this->points[i+1].set_second_fpelem_pointer(NULL);
		}
	}
	//set the distances for the flow direction
	for(int i=0; i<this->number;i++){
		this->points[i].set_flow_distance(fp1);
	}
}
//Transfer the information of the coupled elements/profiles etc to the coupling points
void Hyd_Coupling_Point_FP2FP_List::transfer_informations2points(void){
	for(int i=0; i<this->number;i++){
		this->points[i].transfer_coupling_characteristics();
	}
}
//Reset the coupling dicharges of the coupling points
void Hyd_Coupling_Point_FP2FP_List::reset_coupling_discharge(void){
	for(int i=0; i<this->number;i++){
		this->points[i].reset_coupling_discharge();
	}
}
//Reset the coupling points
void Hyd_Coupling_Point_FP2FP_List::reset_points(void){
	for(int i=0; i<this->number;i++){
		this->points[i].reset_hydro_balance_max_values();
		this->points[i].reset_smoothing();
	}
}
//Syncronisation of the coupled models with the stored couplingspoints in the list
void Hyd_Coupling_Point_FP2FP_List::syncronisation_models_bylistpoints(const double timepoint, const double delta_t, const bool time_check, const int internal_counter){
	for(int i=0; i<this->number;i++){
		this->points[i].syncronisation_coupled_models(timepoint,delta_t, time_check, internal_counter);	
	}
}
//Clone the coupling point list
void Hyd_Coupling_Point_FP2FP_List::clone_list(Hyd_Coupling_Point_FP2FP_List *list, Hyd_Model_Floodplain *fp1, Hyd_Model_Floodplain *fp2){
	_Hyd_Coupling_Point_List::clone_list(list);
	if(fp1==NULL || fp2==NULL){
		return;
	}

	this->allocate_couplingpoints();
	for(int i=0; i< this->number; i++){
		this->points[i]=list->points[i];
	}
	//set the pointer
	for(int i=0; i< this->number; i++){
		this->points[i].set_first_fpelem_pointer(fp1->get_ptr_floodplain_elem(list->points[i].fp1_elem_index));
		this->points[i].set_second_fpelem_pointer(fp2->get_ptr_floodplain_elem(list->points[i].fp2_elem_index));
	}
}
//Output the discharge list to file
//void Hyd_Coupling_Point_FP2FP_List::output_discharge_lists(void){
//	for(int i=0; i< this->number; i++){
//
//		this->points[i].output_discharge_list();
//	}
//}
//____________
//private
//Delete the allocated coupling points
void Hyd_Coupling_Point_FP2FP_List::delete_couplingpoints(void){
	if(this->points!=NULL){
		delete []this->points;
		this->points=NULL;
	}
}
//Allocate the coupling points
void Hyd_Coupling_Point_FP2FP_List::allocate_couplingpoints(void){

	int full_blocks=(int)(this->number/this->block_elems);
	if((this->number-full_blocks*this->block_elems)>0){
		full_blocks++;
	}
	try{
		this->points=new Hyd_Coupling_Point_FP2FP[full_blocks*this->block_elems];
	}
	catch(bad_alloc &){
		Error msg=this->set_error(0);
		throw msg;
	}
}
//Calculate the distance to the startpoint
void Hyd_Coupling_Point_FP2FP_List::calculate_distances(void){
	if(this->defining_polysegment==NULL){
		Error msg=this->set_error(0);
		throw msg;
	}
	//calculate the total distance to the start point
	if(this->number>0){
		for(int i=0; i< this->number; i++){
			try{
				this->points[i].set_total_distance_along_polysegment(this->defining_polysegment->get_distance_along_polysegment(&(this->points[i])));
			}
			catch(Error msg){
				ostringstream info;
				info <<"Pointno. in list " << i << endl;
				msg.make_second_info(info.str());
				throw msg;
			}
		}
	}
}
//Set error(s)
Error Hyd_Coupling_Point_FP2FP_List::set_error(const int err_type){
		string place="Hyd_Coupling_Point_FP2FP_List::";
		string help;
		string reason;
		int type=0;
		bool fatal=false;
		stringstream info;
		Error msg;

	switch (err_type){
		case 0://bad alloc
			place.append("allocate_couplingpoints(void))/set_new_couplingpoint(Hyd_Coupling_Point_FP2CO *new_point)");
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