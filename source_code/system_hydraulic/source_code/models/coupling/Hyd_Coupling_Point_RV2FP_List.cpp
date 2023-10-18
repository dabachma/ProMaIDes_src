#include "Hyd_Headers_Precompiled.h"
//#include "Hyd_Coupling_Point_RV2FP_List.h"

//constructor
Hyd_Coupling_Point_RV2FP_List::Hyd_Coupling_Point_RV2FP_List(void){
	this->number=0;
	this->points=NULL;
	this->left_river_flag=false;
	//count the memory
	Sys_Memory_Count::self()->add_mem(sizeof(Hyd_Coupling_Point_RV2FP_List), _sys_system_modules::HYD_SYS);

}
//destructor
Hyd_Coupling_Point_RV2FP_List::~Hyd_Coupling_Point_RV2FP_List(void){
	this->delete_list();
	//count the memory
	Sys_Memory_Count::self()->minus_mem(sizeof(Hyd_Coupling_Point_RV2FP_List), _sys_system_modules::HYD_SYS);
}
//_____________
//public
//Get the number of coupling points
int Hyd_Coupling_Point_RV2FP_List::get_number_couplings(void){
	return this->number;
}
//Get the total list as a pointer
Hyd_Coupling_Point_RV2FP* Hyd_Coupling_Point_RV2FP_List::get_total_list(void){
	return this->points;
}
//Get a specific coupling pointswith the given index
Hyd_Coupling_Point_RV2FP Hyd_Coupling_Point_RV2FP_List::get_couplingpoint(const int index){
	return this->points[index];
}
//Get a pointer to specific coupling points with the given index
Hyd_Coupling_Point_RV2FP* Hyd_Coupling_Point_RV2FP_List::get_ptr_couplingpoint(const int index){
	Hyd_Coupling_Point_RV2FP* ptr=NULL;
	if(index<0 || index>=this->number){
		return ptr;
	}
	else{
		return &this->points[index];
	}

}
//Get the next coupling point upwards in the list
Hyd_Coupling_Point_RV2FP* Hyd_Coupling_Point_RV2FP_List::get_next_point_upwards(Hyd_Coupling_Point_RV2FP* coupling_point){
	Hyd_Coupling_Point_RV2FP* pointer=NULL;
	for(int i=0; i<this->number-1;i++){
		if(&this->points[i]==coupling_point){
			pointer=&this->points[i+1];
			break;
		}
	}
	return pointer;
}
//Get the next coupling point downwards in the list
Hyd_Coupling_Point_RV2FP* Hyd_Coupling_Point_RV2FP_List::get_next_point_downwards(Hyd_Coupling_Point_RV2FP* coupling_point){
	Hyd_Coupling_Point_RV2FP* pointer=NULL;
	for(int i=1; i<this->number;i++){
		if(&this->points[i]==coupling_point){
			pointer=&this->points[i-1];
			break;
		}
	}

	return pointer;
}
//Get the first coupling points of the list
Hyd_Coupling_Point_RV2FP Hyd_Coupling_Point_RV2FP_List::get_first_couplingpoint(void){
	return this->points[0];
}
//Get the last coupling points of the list
Hyd_Coupling_Point_RV2FP Hyd_Coupling_Point_RV2FP_List::get_last_couplingpoint(void){
	return this->points[this->number-1];
}
//Output the setted members
void Hyd_Coupling_Point_RV2FP_List::output_setted_members(ostringstream *cout){
	*cout  << "No.,";
	Hyd_Coupling_Point_RV2FP::output_header_setted_member(cout);
	for(int i=0; i<this->number; i++){
		*cout << i<<",";
		this->points[i].output_setted_members(cout);
	}
}
//Set a new coupling points
void Hyd_Coupling_Point_RV2FP_List::set_new_couplingpoint(Hyd_Coupling_Point_RV2FP *new_point){
	//check for geometrical same points=> do not count it; is a junction of segments
	for(int i=0; i < this->number; i++){
		if( (*new_point)==this->points[i]){
			return;
		}
	}
	try{
		new_point->set_total_distance_along_polysegment(this->defining_polysegment->get_distance_along_polysegment(new_point));
	}
	catch(Error msg){
		throw msg;
	}

	if(this->number!=0 && this->number % this->block_elems!=0){

		
		this->points[number]=*new_point;
		this->number++;


	}
	else{
		//allocate a buffer
		Hyd_Coupling_Point_RV2FP *buffer=NULL;

		try{
			int full_blocks=(int)((this->number+1)/this->block_elems);
			if(((this->number+1)-full_blocks*this->block_elems)>0){
				full_blocks++;
			}
			buffer=new Hyd_Coupling_Point_RV2FP[full_blocks*this->block_elems];
			//buffer=new Hyd_Coupling_Point_RV2FP[this->number];
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


		//delete the buffer
		//if(buffer!=NULL){
		//	delete []buffer;
		//	buffer=NULL;
		//}

	}
}
//Set if it is the left river bank line or the right one (in flowdirection) 
void Hyd_Coupling_Point_RV2FP_List::set_leftriver_bank_line(const bool flag){
	this->left_river_flag=flag;
}
//Get if it is the left river bank line or the right one (in flowdirection) 
bool Hyd_Coupling_Point_RV2FP_List::get_leftriver_bank_line(void){
	return this->left_river_flag;
}
//Get a point between the coupling points for the given distance from the beginning of the river; if nothing is found a NULL-pointer is returned
Hyd_Coupling_Point_RV2FP* Hyd_Coupling_Point_RV2FP_List::find_coupling_point(const double distance, _Hyd_Coupling_Point *point){
	Hyd_Coupling_Point_RV2FP *pointer=NULL;
	point->set_total_distance_along_polysegment(distance);
	if (this->points[0].distance_along_polysegment >= distance) {
		return pointer;
	}

	for(int i=1; i< this->number; i++){
		if(this->points[i].distance_along_polysegment>=distance){
			if(this->points[i].coupling_flag==true){
				pointer=&this->points[i];
				double diff_dist=distance-this->points[i-1].distance_along_polysegment;
				double y_coor=this->points[i-1].get_ycoordinate()+(diff_dist/this->points[i].get_distance_down())*(this->points[i].get_ycoordinate()-this->points[i-1].get_ycoordinate());
				double x_coor=this->points[i-1].get_xcoordinate()+(diff_dist/this->points[i].get_distance_down())*(this->points[i].get_xcoordinate()-this->points[i-1].get_xcoordinate());
				point->set_point_coordinate(x_coor, y_coor);
				point->set_distance_up(&(this->points[i]));
				point->set_distance_down(&(this->points[i-1]));
				//ostringstream cout;

				//this->points[i].output_coor_members(&cout);
				//this->points[i-1].output_coor_members(&cout);
				//point->output_coor_members(&cout);
				//Sys_Common_Output::output_hyd->output_txt(&cout);


				break;
			}
			else{
				break;
			}
		}
	}

	return pointer;
}
//Find a corresponding coupling point to a given distance along the defining polysegment
Hyd_Coupling_Point_RV2FP* Hyd_Coupling_Point_RV2FP_List::find_coupling_point(const double distance){
	Hyd_Coupling_Point_RV2FP *pointer=NULL;
	for(int i=0; i< this->number; i++){
		if(this->points[i].distance_along_polysegment>=distance){
			pointer=&this->points[i];
			break;
		}
	}

	return pointer;
}
//Insert two coupling point in the list from the given midpoint of a structure or riverjunction up- and downstream and set a stop break flag and a no overflow flag
void Hyd_Coupling_Point_RV2FP_List::insert_constricting_coupling_points(const double width, _Hyd_Coupling_Point *mid_point){
	//pointer to the already existing coupling point related to the upstream inserting point
	Hyd_Coupling_Point_RV2FP *point_upstream;
	//pointer to the already existing coupling point relatedto the downstream inserting point
	Hyd_Coupling_Point_RV2FP *point_downstream;
	//point which will be insert upstream of the structure/junction
	Hyd_Coupling_Point_RV2FP insert_upstream;
	//point which will be insert downstream of the structure/junction
	Hyd_Coupling_Point_RV2FP insert_downstream;

	
	//set and find for the downstream inserting point
	
	point_downstream= this->find_coupling_point((mid_point->get_total_distance_along_polysegment()+width*0.5), &insert_downstream);
	//set and find for the upstream inserting point
	
	point_upstream= this->find_coupling_point((mid_point->get_total_distance_along_polysegment()-width*0.5), &insert_upstream);
	
	//normally not possible
	if(point_upstream==NULL && point_downstream ==NULL){
		return;
	}
	//at the end of the list or there are no coupling points after the upstream point
	else if(point_upstream!=NULL && point_downstream ==NULL){
		//upstream
		//transfer the members to the inserting point from the related existing point
		insert_upstream.set_couplingpoint_members(point_upstream);
		//set flags to the new point
		insert_upstream.set_stop_break_flag();
		//set the flags to the related existing point
		point_upstream->set_stop_break_flag();

		if(abs(point_upstream->get_total_distance_along_polysegment()-insert_upstream.get_total_distance_along_polysegment())>constant::meter_epsilon){
			point_upstream->set_break_flag(false);
			point_upstream->set_overflow_flag(false);
			point_upstream->set_fixed_overflow_flag();
		}
		do{
			point_upstream=this->get_next_point_upwards(point_upstream);
			if(point_upstream==NULL){
				break;
			}
			if(point_upstream->get_coupling_flag()==false){
				break;
			}
			point_upstream->set_stop_break_flag();
			point_upstream->set_break_flag(false);
			point_upstream->set_overflow_flag(false);
			point_upstream->set_fixed_overflow_flag();

		}
		while(point_upstream->get_point_index()>=0 && point_upstream->get_point_index()<this->number);

		//set it to the list
		this->set_new_couplingpoint(&insert_upstream);
	}
	//at the beginning of the list or there are no coupling points before the downstream point
	else if(point_upstream==NULL && point_downstream !=NULL){
		//transfer the members to the inserting point from the related existing point
		insert_downstream.set_couplingpoint_members(point_downstream);
		//set flags to the new point
		insert_downstream.set_stop_break_flag();
		insert_downstream.set_break_flag(false);
		insert_downstream.set_overflow_flag(false);
		insert_downstream.set_fixed_overflow_flag();
		do{
			point_downstream=this->get_next_point_downwards(point_downstream);
			if(point_downstream==NULL){
				break;
			}
			if(point_downstream->get_coupling_flag()==false){
				break;
			}
			point_downstream->set_stop_break_flag();
			point_downstream->set_break_flag(false);
			point_downstream->set_overflow_flag(false);
			point_downstream->set_fixed_overflow_flag();

		}
		while(point_downstream->get_point_index()>=0 && point_downstream->get_point_index()<this->number);
		//set it to the list
		this->set_new_couplingpoint(&insert_downstream);
	}
	
	else{
		//check if the inflow is inbetween two floodplain models; no additional points are set
		if (point_upstream == point_downstream) {
			return;
		}
		//downstream
		insert_downstream.set_couplingpoint_members(point_downstream);
		insert_downstream.set_stop_break_flag();
		insert_downstream.set_break_flag(false);
		insert_downstream.set_overflow_flag(false);
		insert_downstream.set_fixed_overflow_flag();

		//upstream
		insert_upstream.set_couplingpoint_members(point_upstream);
		insert_upstream.set_stop_break_flag();

	
		//there are inbetween points
		if(point_upstream!=point_downstream){

			do{
				//set the flags to the related existing point
				point_upstream->set_stop_break_flag();
				point_upstream->set_break_flag(false);
				point_upstream->set_overflow_flag(false);
				point_upstream->set_fixed_overflow_flag();
				point_upstream=this->get_next_point_upwards(point_upstream);
			}
			while(point_upstream!=point_downstream);

		}
		//set it to the list
		this->set_new_couplingpoint(&insert_downstream);
		this->set_new_couplingpoint(&insert_upstream);
	}

	//sort and calculate distances new
	this->sort_distance_along_polysegment();
	this->calculate_alldistance_up_down();
	this->transfer_informations2points();
}
//Add additional coupling points for more detailed coupling between the models
void Hyd_Coupling_Point_RV2FP_List::add_additional_coupling_points(void){

	int number_start=this->number;
	double dist=0.0;
	int steps=0;
	Hyd_Coupling_Point_RV2FP buffer;

	for(int i=1; i< number_start; i++){

		dist=this->points[i].get_distance_down();
		if(dist > this->min_distance){
			steps=(int)(dist/this->min_distance);
			if(abs(dist-(double)steps*this->min_distance)<0.1){
				steps=steps-1;
			}
			for(int j=0; j<steps; j++){
				if(this->find_coupling_point(this->points[i-1].get_total_distance_along_polysegment()+((double)(j+1)*this->min_distance), &buffer)==&(this->points[i])){;
					if(this->points[i].get_index_downstream_rvprofile()-this->points[i-1].get_index_downstream_rvprofile()<=1){
						buffer.set_couplingpoint_members(&this->points[i]);
						buffer.set_point_name(hyd_label::coupling_point_RV2FP);
						try{
							this->set_new_couplingpoint(&buffer);
						}
						catch(Error msg){
							throw msg;
						}
					}
					else{
						this->points[i].set_coupling_flag(false);
					}
				}
				else{
					//nothing to do
			
				}
			}
		}
	}


	//sort it along the defining line
	this->sort_distance_along_polysegment();
	//calculate the distances
	this->calculate_alldistance_up_down();
	
	//transfer the infos to the coupling points
	this->transfer_informations2points();

}
//Find the distance along the river bank line to a given related profile
bool Hyd_Coupling_Point_RV2FP_List::find_distance2related_profile(const int related_profile_index, double *distance){
	bool found_flag=false;
	//search for the profile index
	for(int i=1; i< this->number; i++){
		if(this->points[i].get_downstream_rvprofile()!=NULL){
			if(this->points[i].get_downstream_rvprofile()->get_profile_number()==related_profile_index){
				*distance=this->points[i].get_total_distance_along_polysegment();
				found_flag=true;
			}
		}
	}


	return found_flag;
}
//Delete the list
void Hyd_Coupling_Point_RV2FP_List::delete_list(void){
	this->delete_couplingpoints();
	this->number=0;
}
//Sort the points after the distance to the beginning of the defining polysegment
void Hyd_Coupling_Point_RV2FP_List::sort_distance_along_polysegment(void){
	this->calculate_distances();
	//make the buffer for sorting
	Hyd_Coupling_Point_RV2FP swap;
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
void Hyd_Coupling_Point_RV2FP_List::calculate_alldistance_up_down(void){
	
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
	}
	else if(this->number==1){
			this->points[0].set_distance_down(0.0);
			this->points[0].set_distance_up(0.0);
	}
}
//Add the relevant coupling points of the defining_polysegment
void Hyd_Coupling_Point_RV2FP_List::add_relevant_polysegment_points(Hyd_Floodplain_Polygon *fp_boundary){
	//sort it before
	this->sort_distance_along_polysegment();
	int old_number=this->number;
	Hyd_Coupling_Point_RV2FP buffer;
	for(int i=1; i<old_number; i++){
		if(abs(this->points[i].river_profile_index_up-this->points[i-1].river_profile_index_up)!=0){
			int start_index=min(this->points[i].river_profile_index_up,this->points[i-1].river_profile_index_up);
			int end_index=max(this->points[i].river_profile_index_up,this->points[i-1].river_profile_index_up);
			for(int j=start_index; j<end_index; j++){
				//check if point is relevant=> it is inside the floodplain boundary
				if(fp_boundary->check_point_inside(&(this->defining_polysegment->my_segment[j].point2))==true){
					buffer.set_point(&(this->defining_polysegment->my_segment[j].point2));
					buffer.set_floodplain_index(this->points[i].get_floodplain_index());
					buffer.set_indices(j,this->points[i].floodplain_elem_index);
					buffer.set_pointer_floodplain_element(this->points[i].floodplain_elem);
					buffer.set_point_name(hyd_label::coupling_point_RV2FP);
					this->set_new_couplingpoint(&buffer);
				}
			}
		}
	}
	//check the beginning of the river dikelinie
	if(this->number>0){
		int end_index=this->points[0].river_profile_index_up;
		int counter=0;
		do{
			//check if point is relevant=> it is inside the floodplain boundary
			if(fp_boundary->check_point_inside(&(this->defining_polysegment->my_segment[counter].point1))==true){
				buffer.set_point(&(this->defining_polysegment->my_segment[counter].point1));
				buffer.set_floodplain_index(this->points[0].get_floodplain_index());
				buffer.set_indices(counter,this->points[0].floodplain_elem_index);
				buffer.set_pointer_floodplain_element(this->points[0].floodplain_elem);
				buffer.set_point_name(hyd_label::coupling_point_RV2FP);
				this->set_new_couplingpoint(&buffer);
			}
			counter++;
		}
		while(counter <= end_index);
	}
}
//Convert the profiles indices to a pointer to the profile
void Hyd_Coupling_Point_RV2FP_List::convert_profiles_index2pointer(Hyd_Model_River *river_model){
	for(int i=0; i<this->number;i++){
		this->points[i].set_pointer_river_profiles(river_model);
	}
}
//Transfer the information of the coupled elements/profiles etc to the coupling points
void Hyd_Coupling_Point_RV2FP_List::transfer_informations2points(void){
	for(int i=0; i<this->number;i++){
		this->points[i].transfer_coupling_characteristics(this->left_river_flag);
	}
}
//Reset the coupling dicharges of the coupling points
void Hyd_Coupling_Point_RV2FP_List::reset_coupling_discharge(void){
	for(int i=0; i<this->number;i++){
		this->points[i].reset_coupling_discharge(this->left_river_flag);
	}
}
//Reset the coupling points
void Hyd_Coupling_Point_RV2FP_List::reset_points(void){
	for(int i=0; i<this->number;i++){
		this->points[i].reset_hydro_balance_max_values();
		this->points[i].reset_smoothing();
	}
}
//Syncronisation of the coupled models with the stored couplingspoints in the list
void Hyd_Coupling_Point_RV2FP_List::syncronisation_models_bylistpoints(const double timepoint, const double delta_t, const bool time_check, const int internal_counter){
	for(int i=0; i<this->number;i++){
		this->points[i].syncronisation_coupled_models(timepoint, delta_t, this->left_river_flag, time_check, internal_counter);	
	}
}
//Get the maximum waterlevel gradient 
double Hyd_Coupling_Point_RV2FP_List::get_maximum_h_grad(void){
	double buffer=0.0;
	double buffer_max=0.0;
	for(int i=0; i<this->number;i++){
		
		buffer=this->points[i].get_maximum_h_grad();;
		if(buffer_max<buffer){
			buffer_max=buffer;
		}
	}

	return buffer_max;
}
//Merge this list with another given list by same defining polysegments; the merged list (result) is the given list
void Hyd_Coupling_Point_RV2FP_List::merge_coupling_list(Hyd_Coupling_Point_RV2FP_List *merging_list){
	if(this->defining_polysegment==NULL || merging_list->get_defining_polysegment()==NULL){
		return;
	}
	if(this->defining_polysegment!=merging_list->get_defining_polysegment()){
		return;
	}
	//return if no point to merge
	if(this->get_number_couplings()==0){
		return;
	}

	//special actions for the first point
	if(merging_list->get_number_couplings()!=0){
		//check if the first point of the merging_list is inbetween this lists start/end-points
		if(merging_list->get_first_couplingpoint().get_total_distance_along_polysegment()>this->get_first_couplingpoint().get_total_distance_along_polysegment() &&
			merging_list->get_first_couplingpoint().get_total_distance_along_polysegment()<this->get_last_couplingpoint().get_total_distance_along_polysegment()){
				//find the point
				Hyd_Coupling_Point_RV2FP *buffer;
				buffer=this->find_coupling_point(merging_list->get_first_couplingpoint().get_total_distance_along_polysegment());
				merging_list->get_ptr_couplingpoint(0)->set_couplingpoint_members(buffer);
		}
		//check if the first point of this list is inbetween the merging lists start/end-points
		else if(this->get_first_couplingpoint().get_total_distance_along_polysegment()>merging_list->get_first_couplingpoint().get_total_distance_along_polysegment() &&
			this->get_first_couplingpoint().get_total_distance_along_polysegment()<merging_list->get_last_couplingpoint().get_total_distance_along_polysegment()){
				//find the point
				Hyd_Coupling_Point_RV2FP *buffer;
				Hyd_Coupling_Point_RV2FP new_point;
				buffer=merging_list->find_coupling_point(this->get_first_couplingpoint().get_total_distance_along_polysegment());
				new_point=this->get_first_couplingpoint();
				new_point.set_couplingpoint_members(buffer);
				merging_list->set_new_couplingpoint(&new_point);
		}
	}


	//put the whole list in the merging_list
	for(int i=0; i< this->number; i++){
		merging_list->set_new_couplingpoint(&this->points[i]);

	}
	//sort it
	merging_list->sort_distance_along_polysegment();
	//calculate the distance
	merging_list->calculate_alldistance_up_down();
	//transfer infos to points
	merging_list->transfer_informations2points();
	
	//check same points
	for(int i=0; i< this->number; i++){
		for(int j=0; j<merging_list->get_number_couplings(); j++){
			if(this->points[i]==merging_list->get_couplingpoint(j) && this->points[i].get_floodplain_index()!=merging_list->get_couplingpoint(j).get_floodplain_index()){
				if(this->points[i].get_coupling_flag()==true && merging_list->get_couplingpoint(j).get_coupling_flag()!=true){
					merging_list->get_ptr_couplingpoint(j)->set_couplingpoint_members(&this->points[i]);
					merging_list->get_ptr_couplingpoint(j)->transfer_coupling_characteristics(merging_list->get_leftriver_bank_line());
				}
				else if(this->points[i].get_coupling_flag()==true && merging_list->get_couplingpoint(j).get_coupling_flag()==true){
					double dist_0=this->points[i].get_distancecoupling2elem_mid();
					double dist_1=merging_list->get_couplingpoint(j).get_distancecoupling2elem_mid();
					if(dist_0>0.0 && dist_1 >0.0){
						if(dist_0<dist_1){
							merging_list->get_ptr_couplingpoint(j)->set_couplingpoint_members(&this->points[i]);
							merging_list->get_ptr_couplingpoint(j)->transfer_coupling_characteristics(merging_list->get_leftriver_bank_line());
						}
					}
				}
				break;
			}

		}
	}
	
	merging_list->verify_merged_list();
	//for development
	//ostringstream cout;
	//merging_list->output_setted_members(&cout);

}
//Check if the connecting segment between coupling point and floodplain element mid_point is intersecting a river bank_line; a coupling is than not possible
void Hyd_Coupling_Point_RV2FP_List::check_intercepting_riverbankline2connecting_segment(Hyd_Floodplain_Polysegment *river_bankline){
	Geo_Segment *buffer=NULL;
	Geo_Interception_Point_List intercept_list;

	for(int i=0; i< this->number-1; i++){
		buffer=this->get_connecting_segment_point_elem(i);
		if(buffer!=NULL){
			intercept_list.delete_list();
			river_bankline->calculate_segment_interception(&intercept_list, buffer);
			if(intercept_list.get_number_points()>1){
				this->points[i+1].set_pointer_floodplain_element(NULL);
				this->points[i+1].set_coupling_flag(false);
				this->points[i+1].set_index_floodplain_element(-1);
			}
			else if(intercept_list.get_number_points()==1){
				if(intercept_list.get_first_point().index_point_is_intercepted==-1){
					this->points[i+1].set_pointer_floodplain_element(NULL);
					this->points[i+1].set_coupling_flag(false);
					this->points[i+1].set_index_floodplain_element(-1);
				}
				else{
					if(intercept_list.get_first_point().index_point_is_intercepted==0 && intercept_list.get_first_point().index_is_intercept==0){
						//nothing happen the first point of the first segment can be intercepted
					}	
					else if(intercept_list.get_first_point().index_point_is_intercepted==1 && intercept_list.get_first_point().index_is_intercept==river_bankline->get_number_segments()-1){
						//nothing happen the last point of the last segment can be intercepted
					}
					else{
						this->points[i+1].set_pointer_floodplain_element(NULL);
						this->points[i+1].set_coupling_flag(false);
						this->points[i+1].set_index_floodplain_element(-1);

					}
					
				}

			}
		}
	}
}
//Clone the coupling point list
void Hyd_Coupling_Point_RV2FP_List::clone_list(Hyd_Coupling_Point_RV2FP_List *list, Hyd_Hydraulic_System *system, const int rv_number){
	_Hyd_Coupling_Point_List::clone_list(list);
	this->left_river_flag=list->left_river_flag;
	this->allocate_couplingpoints();
	for(int i=0; i< this->number; i++){
		this->points[i]=list->points[i];
	}
	Hyd_Model_River *buff_rv=system->get_ptr_river_model(rv_number);
	Hyd_Model_Floodplain *buff_fp=NULL;
	//set the pointer
	for(int i=0; i< this->number; i++){
		this->points[i].river_profile_down=buff_rv->get_ptr_river_profile(list->points[i].river_profile_index_down);
		this->points[i].river_profile_up=buff_rv->get_ptr_river_profile(list->points[i].river_profile_index_up);
		buff_fp=system->get_ptr_floodplain_model_index(list->points[i].floodplain_index);
		if(buff_fp!=NULL){
			this->points[i].set_pointer_floodplain_element(buff_fp->get_ptr_floodplain_elem(list->points[i].floodplain_elem_index));
		}
		else{
			this->points[i].floodplain_elem=NULL;
		}
	}
	this->fpl_sec_ids=*(list->get_ptr_fpl_section_ids());
}
//Get pointer to the coupling point specified by the given
Hyd_Coupling_Point_RV2FP* Hyd_Coupling_Point_RV2FP_List::get_ptr_couplingpoint(Hyd_Coupling_Point_RV2FP coupling_point){
	
	for(int i=0; i< this->number; i++){
		if(this->points[i]==coupling_point){
			return &this->points[i];
		}
	}

	return NULL;
}
//Set the integer list of the different fpl-section ids
void Hyd_Coupling_Point_RV2FP_List::set_list_fpl_sec_id(void){

	for(int i=0; i< this->number; i++){
		if(this->points[i].get_fpl_section_index()>=0){
			if(this->fpl_sec_ids.contains(this->points[i].get_fpl_section_index())==false){
				this->fpl_sec_ids.append(this->points[i].get_fpl_section_index());
			}
		}
	}
}
//Get a pointer to the list of the fpl-section ids
QList<int>* Hyd_Coupling_Point_RV2FP_List::get_ptr_fpl_section_ids(void){
	return &this->fpl_sec_ids;
}
//Get the maximum waterlevel for a coupling of hydraulic and fpl-system and the corresponding coupling point id
void Hyd_Coupling_Point_RV2FP_List::get_max_h2fpl(const int fpl_sec_id, _hyd_max_values *max_h, int *point_id){
	_hyd_max_values h_max_buff;
	h_max_buff.maximum=0.0;
	h_max_buff.time_point=0.0;
	int id_buff=0;

	for(int i=0; i<this->number; i++){
		if(this->points[i].get_fpl_section_index()==fpl_sec_id){
			if(h_max_buff.maximum<this->points[i].get_max_h().maximum){
				h_max_buff=this->points[i].get_max_h();
				id_buff=i;
			}

		}

	}
	*point_id=id_buff;
	*max_h=h_max_buff;
}
//Get the maximum waterlevel for a coupling of river- and floodplain-model via a break
void Hyd_Coupling_Point_RV2FP_List::get_max_h2break(const int fpl_sec_id, _hyd_max_values *max_h, int *point_id){
	double h_max_buff=0.0;
	int id_buff=0;

	for(int i=0; i<this->number; i++){
		if(this->points[i].get_fpl_section_index()==fpl_sec_id){
			if(h_max_buff<this->points[i].get_max_deltah_2break()){
				h_max_buff=this->points[i].get_max_deltah_2break();
				id_buff=i;
			}

		}

	}
	*point_id=id_buff;
	*max_h=this->points[id_buff].get_max_h_2break();
}
//____________
//private
//Delete the allocated coupling points
void Hyd_Coupling_Point_RV2FP_List::delete_couplingpoints(void){
	if(this->points!=NULL){
		delete []this->points;
		this->points=NULL;
		
	}
}
//Allocate the coupling points
void Hyd_Coupling_Point_RV2FP_List::allocate_couplingpoints(void){

	int full_blocks=(int)(this->number/this->block_elems);
	if((this->number-full_blocks*this->block_elems)>0){
		full_blocks++;
	}

	try{
		this->points=new Hyd_Coupling_Point_RV2FP[full_blocks*this->block_elems];
	}
	catch(bad_alloc &){
		Error msg=this->set_error(0);
		throw msg;
	}
}
//Get the connecting Geo_Segment between the mid_point of two coupling points and floodplain element mid_point for the given point index
Geo_Segment * Hyd_Coupling_Point_RV2FP_List::get_connecting_segment_point_elem(const int index){
	Geo_Segment *buffer=NULL;

	if(index<0 || index>= this->number-1){
		return buffer=NULL;
	}
	if(points[index+1].get_coupling_flag()==false){
		return buffer=NULL;
	}
	this->get_mid_point(index);

	if(this->mid_point==*points[index+1].get_floodplain_element()->get_mid_point()){
		return buffer=NULL;
	}
	this->connecting_segment.set_points(&this->mid_point,points[index+1].get_floodplain_element()->get_mid_point()); 

	return &this->connecting_segment;
}
//Get the mid_point between two coupling points
Geo_Point *Hyd_Coupling_Point_RV2FP_List::get_mid_point(const int index){
	Geo_Segment buff_segment;
	buff_segment.set_points(&this->points[index], &this->points[index+1]);
	this->mid_point=buff_segment.calc_mid_point();
	return &this->mid_point;

}
//Calculate the distance to the startpoint 
void Hyd_Coupling_Point_RV2FP_List::calculate_distances(void){
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
//Verify the merged list after merging
void Hyd_Coupling_Point_RV2FP_List::verify_merged_list(void){

	//from end
	for(int i=this->number-1; i>0; i--){
		if(this->points[i].get_floodplain_index()!=this->points[i-1].get_floodplain_index() && 
			this->points[i-1].get_coupling_flag()==false){
				this->points[i-1].set_couplingpoint_members(&this->points[i]);
				this->points[i-1].transfer_coupling_characteristics(this->get_leftriver_bank_line());
		}
	}
}
//Set error(s)
Error Hyd_Coupling_Point_RV2FP_List::set_error(const int err_type){
		string place="Hyd_Coupling_Point_RV2FP_List::";
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
