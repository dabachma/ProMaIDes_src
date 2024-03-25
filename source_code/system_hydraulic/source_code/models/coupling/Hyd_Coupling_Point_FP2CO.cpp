#include "Hyd_Headers_Precompiled.h"
//#include "Hyd_Coupling_Point_FP2CO.h"

//constructor
Hyd_Coupling_Point_FP2CO::Hyd_Coupling_Point_FP2CO(void){
	this->coastdikeline_index=-1;
	this->coastdikeline_height=0.0;
	this->floodplain_elem_index=-1;
	this->floodplain_elem=NULL;
	this->floodplain_index=-1;
	this->overflow_flag=true;
	this->ptr_upstream_point=NULL;
	this->ptr_downstream_point=NULL;

	this->fpl_section_id=-1;
	this->mid_basepoint=0.0;
	this->mid_basepoint_point=0.0;

	this->poleni_coefficient=0.577;

	this->current_q_break=0.0;
	this->coupling_v_break=0.0;

	this->break_flag=false;
	this->index_break=-1;
	this->stop_break_flag=false;
	this->break_height=0.0;
	this->break_width=0.0;

	this->total_flow_width=0.0;
	this->overflow_width=0.0;
	this->delta_h_fp2co_break=0.0;
	this->max_h_2break.maximum=0.0;
	this->max_h_2break.time_point=0.0;
	this->max_h.maximum=0.0;
	this->max_h.time_point=0.0;
	this->max_deltah2break=0.0;

	this->old_q_break=0.0;


	this->grad_q_current=0.0;
	this->grad_q_before=0.0;
	this->oscilation_smoother=1.0;
	this->no_osci_counter=0.0;
	this->number_osci_counter=2.0;

	this->grad_q_break_current=0.0;
	this->grad_q_break_before=0.0;
	this->oscilation_smoother_break=1.0;
	this->no_osci_counter_break=0.0;
	this->number_osci_counter_break=2.0;
	//this->break_q_list.relevant=false;
	this->store_grad_q_before=0.0;
	this->store_oscilation_smoother=1.0;
	this->store_no_osci_counter=0.0;
	this->store_number_osci_counter=2.0;
	this->store_grad_q_break_before=0.0;
	this->store_oscilation_smoother_break=1.0;
	this->store_no_osci_counter_break=0.0;
	this->store_number_osci_counter_break=2.0;
	this->store_old_q_break=0.0;






	//count the memory
	Sys_Memory_Count::self()->add_mem(sizeof(Hyd_Coupling_Point_FP2CO), _sys_system_modules::HYD_SYS);

}
//Copy constructor
Hyd_Coupling_Point_FP2CO::Hyd_Coupling_Point_FP2CO(const Hyd_Coupling_Point_FP2CO& object){
	_Hyd_Coupling_Point::operator =(object);
	this->coastdikeline_index=object.coastdikeline_index;
	this->coastdikeline_height=object.coastdikeline_height;
	this->floodplain_elem_index=object.floodplain_elem_index;
	this->floodplain_elem=object.floodplain_elem;
	this->floodplain_index=object.floodplain_index;
	this->overflow_flag=object.overflow_flag;

	this->ptr_upstream_point=object.ptr_upstream_point;
	this->ptr_downstream_point=object.ptr_downstream_point;

	this->break_flag=object.break_flag;
	this->index_break=object.index_break;
	this->stop_break_flag=object.stop_break_flag;
	this->break_height=object.break_height;
	this->break_width=object.break_width;

	this->fpl_section_id=object.fpl_section_id;
	this->mid_basepoint=object.mid_basepoint;
	this->mid_basepoint_point=object.mid_basepoint_point;

	this->poleni_coefficient=object.poleni_coefficient;

	this->total_flow_width=object.total_flow_width;
	this->overflow_width=object.overflow_width;
	this->delta_h_fp2co_break=object.delta_h_fp2co_break;
	this->max_h_2break=object.max_h_2break;
	this->max_h=object.max_h;
	this->max_deltah2break=object.max_deltah2break;


	this->grad_q_current=object.grad_q_current;
	this->grad_q_before=object.grad_q_before;
	this->oscilation_smoother=object.oscilation_smoother;
	this->no_osci_counter=object.no_osci_counter;
	this->number_osci_counter=object.number_osci_counter;

	this->grad_q_break_current=object.grad_q_break_current;
	this->grad_q_break_before=object.grad_q_break_before;
	this->oscilation_smoother_break=object.oscilation_smoother_break;
	this->no_osci_counter_break=object.no_osci_counter_break;
	this->number_osci_counter_break=object.number_osci_counter_break;

	//this->break_q_list.relevant=object.break_q_list.relevant;

	this->store_grad_q_before=object.store_grad_q_before;
	this->store_oscilation_smoother=object.store_oscilation_smoother;
	this->store_no_osci_counter=object.store_no_osci_counter;
	this->store_number_osci_counter=object.store_number_osci_counter;
	this->store_grad_q_break_before=object.store_grad_q_break_before;
	this->store_oscilation_smoother_break=object.store_oscilation_smoother_break;
	this->store_no_osci_counter_break=object.store_no_osci_counter_break;
	this->store_old_q_break=object.store_old_q_break;
	this->store_number_osci_counter_break=object.store_number_osci_counter_break;
	

	//count the memory
	Sys_Memory_Count::self()->add_mem(sizeof(Hyd_Coupling_Point_FP2CO), _sys_system_modules::HYD_SYS);
}
//destructor
Hyd_Coupling_Point_FP2CO::~Hyd_Coupling_Point_FP2CO(void){

	//count the memory
	Sys_Memory_Count::self()->minus_mem(sizeof(Hyd_Coupling_Point_FP2CO), _sys_system_modules::HYD_SYS);
}
//_________
//public
//Set the pointer/index members (without the geometrical members) from the given point
void Hyd_Coupling_Point_FP2CO::set_couplingpoint_members(Hyd_Coupling_Point_FP2CO *object){

	//from this class
	this->floodplain_elem_index=object->floodplain_elem_index;
	this->set_pointers(object->floodplain_elem);
	this->floodplain_index=object->floodplain_index;
	this->coastdikeline_index=object->coastdikeline_index;


}
//Output the header for the setted member (static)
void Hyd_Coupling_Point_FP2CO::output_header_setted_member(ostringstream *cout){
	*cout <<W(10) << "I_co-line" << W(10) << "I_fp" << W(15) << "I_fp_elem" << W(15)<< "Coupling"<< W(15) << "Overflow"<< W(15)<<"Dist_up" << label::m <<W(15)<< "Dist_down" << label::m;
	*cout << W(15)<< "abs_Height" << label::m << W(15) << "Dist_begin" <<label::m ;
	*cout << W(15)<< "x" << label::m << W(17) << "y" <<label::m << W(15) << "mid base point height" << label::m << W(17) << "fixed base point height" << label::m << W(17) << "Coastline index" << label::no_unit;
	*cout<< endl;
	Sys_Common_Output::output_hyd->output_txt(cout,true);
}
//Output the setted members
void Hyd_Coupling_Point_FP2CO::output_setted_members(ostringstream *cout){
	*cout <<W(10) << this->coastdikeline_index << W(12) << this->floodplain_index;
	*cout << W(10) <<this->floodplain_elem_index;
	*cout <<W(16) << functions::convert_boolean2string(this->coupling_flag);
	*cout <<W(16) << functions::convert_boolean2string(this->overflow_flag);
	*cout <<W(17) <<P(2)<< FORMAT_FIXED_REAL << this->distance_up ;
	*cout <<W(18) <<P(2)<< FORMAT_FIXED_REAL << this->distance_down ;
	*cout <<W(20) <<P(2)<< FORMAT_FIXED_REAL << this->coastdikeline_height ;
	*cout <<W(21) <<P(2)<< FORMAT_FIXED_REAL << this->distance_along_polysegment ;
	*cout <<W(21) <<P(2)<< FORMAT_FIXED_REAL << this->x_coordinate ;
	*cout <<W(21) <<P(2)<< FORMAT_FIXED_REAL << this->y_coordinate ;
	*cout << W(21) << P(2) << FORMAT_FIXED_REAL << this->get_mid_basepoint_height();
	*cout << W(21) << P(2) << FORMAT_FIXED_REAL << this->get_fixed_basepoint_height();
	*cout << W(21) << P(2) << FORMAT_FIXED_REAL << this->coastdikeline_index;
	*cout<< endl;
	Sys_Common_Output::output_hyd->output_txt(cout,true);
}
//Set the indices
void Hyd_Coupling_Point_FP2CO::set_indices(const int coast_dikeline_index, const int floodplain_model_elem){
	this->coastdikeline_index=coast_dikeline_index;
	this->floodplain_elem_index=floodplain_model_elem;
}
//Set the pointer to the dikeline points
void Hyd_Coupling_Point_FP2CO::set_ptr_dikeline_points(Hyd_Floodplain_Dikeline_Point *upstream, Hyd_Floodplain_Dikeline_Point *downstream){
	this->ptr_upstream_point=upstream;
	this->ptr_downstream_point=downstream;
}
//Get the pointer to upwards dikeline point
Hyd_Floodplain_Dikeline_Point *Hyd_Coupling_Point_FP2CO::get_ptr_upstream_point(void){
	return this->ptr_upstream_point;
}
//Get the pointer to downwards dikeline point
Hyd_Floodplain_Dikeline_Point *Hyd_Coupling_Point_FP2CO::get_ptr_downstream_point(void){
	return this->ptr_downstream_point;
}
//Set the flag if a coupling due to a overflow is applicable
void Hyd_Coupling_Point_FP2CO::set_overflow_flag(const bool flag){
	this->overflow_flag=flag;
}
//Set the index of the coupled floodplain
void Hyd_Coupling_Point_FP2CO::set_floodplain_index(const int index){
	this->floodplain_index=index;
}
//Get the index of the coupled floodplain
int Hyd_Coupling_Point_FP2CO::get_floodplain_index(void){
	return this->floodplain_index;
}
//Set the pointers to the elements to the floodplain element
void Hyd_Coupling_Point_FP2CO::set_pointers(Hyd_Element_Floodplain *floodplain){
	this->floodplain_elem=floodplain;
	if(this->floodplain_elem!=NULL && this->floodplain_elem_index>=0){
		this->floodplain_elem->element_type->set_coupling_data();
	}
}
//Get the pointer to the floodplain element
Hyd_Element_Floodplain* Hyd_Coupling_Point_FP2CO::get_floodplain_element(void){
	return this->floodplain_elem;
}
//Transfer the coupling characteristics of the coupled elements
void Hyd_Coupling_Point_FP2CO::transfer_coupling_characteristics(void){

	//no coupling applied
	if(this->coastdikeline_index<0 || this->floodplain_elem_index<0 || this->ptr_downstream_point==NULL || this->ptr_upstream_point==NULL){
		this->coupling_flag=false;
		this->coastdikeline_height=0.0;
		this->break_flag=false;
		this->overflow_flag=false;
		return;
	}
	if(this->floodplain_elem->get_elem_type()==_hyd_elem_type::STANDARD_ELEM || this->floodplain_elem->get_elem_type()==_hyd_elem_type::DIKELINE_ELEM){
		this->coupling_flag=true;
	}
	//no coupling applied
	else{
		this->coupling_flag=false;
		this->coastdikeline_height=0.0;
		this->break_flag=false;
		this->overflow_flag=false;
		return;
	}

	//set minimal area
	this->min_area=this->floodplain_elem->element_type->get_relevant_area();

	//set the fpl index, if available
	this->fpl_section_id=this->ptr_upstream_point->get_fpl_section_id();
	
	//the height
	double total_point_distance=0.0;
	double distance_point_up=0.0;
	double distance_point_down=0.0;

	//calculate the distances to the dikeline points for a point between the coupling points => hmid
	total_point_distance=this->ptr_upstream_point->distance(this->ptr_downstream_point);
	distance_point_up=this->distance(this->ptr_upstream_point);
	distance_point_up=distance_point_up-this->distance_down*0.5;
	distance_point_down=this->distance(this->ptr_downstream_point);
	distance_point_down=distance_point_down+this->distance_down*0.5;

	//set the mid of the height
	this->coastdikeline_height=(1.0-distance_point_down/total_point_distance)*this->ptr_downstream_point->get_abs_height();
	this->coastdikeline_height=this->coastdikeline_height+(1.0-distance_point_up/total_point_distance)*this->ptr_upstream_point->get_abs_height();



	//set absolute height
	if(this->coastdikeline_height<=this->floodplain_elem->get_z_value()){
		this->coastdikeline_height=this->floodplain_elem->get_z_value();
	}


	//the other params
	this->poleni_coefficient=this->ptr_upstream_point->get_poleni_fac();
	this->overflow_flag=this->ptr_upstream_point->get_overflow_flag();
	this->break_flag=this->ptr_upstream_point->get_break_flag();

	//set the width
	this->total_flow_width=this->distance_down;
	this->overflow_width=this->total_flow_width;

	if(this->break_flag==true){
		this->mid_basepoint=(1.0-distance_point_down/total_point_distance)*this->ptr_downstream_point->get_base_height();
		this->mid_basepoint=this->mid_basepoint+(1.0-distance_point_up/total_point_distance)*this->ptr_upstream_point->get_base_height();
		this->mid_basepoint_point=this->mid_basepoint;
		if (abs(distance_point_up + distance_point_down - total_point_distance +0.001)> 0.1 ) {
			//Add Warning


		}

		if(this->mid_basepoint<=this->floodplain_elem->get_z_value()){
			this->mid_basepoint=this->floodplain_elem->get_z_value();
		}
		//no break in case base_point > dikeline
		if(this->mid_basepoint>this->coastdikeline_height){
			this->break_flag=false;
		}
	}
}
//Reset the current coupling discharge of the points and the coupled element
void Hyd_Coupling_Point_FP2CO::reset_coupling_discharge(void){
	_Hyd_Coupling_Point::reset_coupling_discharge();
	if(this->coupling_flag==true){
		if(this->overflow_flag==true){
			this->floodplain_elem->element_type->reset_coupling_discharge_co_overflow();
		}
		if(this->break_flag==true){
			this->floodplain_elem->element_type->reset_coupling_discharge_co_dikebreak();
		}
	}

}
//Syncronisation of the coupled models with the couplingspoint
void Hyd_Coupling_Point_FP2CO::syncronisation_coupled_models(const double timepoint, const double delta_t, const double coast_water_level, const bool time_check, const int internal_counter){
	_Hyd_Coupling_Point::syncronisation_coupled_models();
	this->delta_t=delta_t;
	this->time_check=time_check;


	if(this->coupling_flag==false){
		return;
	}

	double h_one_buff=0.0;
	this->predict_values(internal_counter);
	h_one_buff=0.5*(this->predicted_h_one+this->calc_h_one);
	
	if(this->time_check==true){
		this->store_grad_q_before=this->grad_q_before;
		this->store_oscilation_smoother=this->oscilation_smoother;
		this->store_no_osci_counter=this->no_osci_counter;
		this->store_grad_q_break_before=this->grad_q_break_before;
		this->store_oscilation_smoother_break=this->oscilation_smoother_break;
		this->store_no_osci_counter_break=this->no_osci_counter_break;
		this->store_old_q_break=this->old_q_break;
		this->store_old_q=this->old_q;
		this->store_number_osci_counter=this->number_osci_counter;
		this->store_number_osci_counter_break=this->number_osci_counter_break;
	}
	else{
		this->grad_q_before=this->store_grad_q_before;
		this->oscilation_smoother=this->store_oscilation_smoother;
		this->no_osci_counter=this->store_no_osci_counter;
		this->grad_q_break_before=this->store_grad_q_break_before;
		this->oscilation_smoother_break=this->store_oscilation_smoother_break;
		this->no_osci_counter_break=this->store_no_osci_counter_break;
		this->old_q_break=this->store_old_q_break;
		this->old_q=this->store_old_q;
		this->number_osci_counter=this->store_number_osci_counter;
		this->number_osci_counter_break=this->store_number_osci_counter_break;
	}

	if(this->overflow_flag==true){
		this->syncronisation_coupled_models_overflow(timepoint, coast_water_level, h_one_buff);
	}
	if(this->break_flag==true){
		if(this->time_check==true){
			//calculate maximum waterlevel for a break
			this->calculate_max_h2break(timepoint, coast_water_level);
		}
		this->syncronisation_coupled_models_break(timepoint, coast_water_level, h_one_buff);
	}

}
//Get the distance from the coupling point to the element mid point
double Hyd_Coupling_Point_FP2CO::get_distancecoupling2elem_mid(void){
	double distance=-1.0;
	if(this->coupling_flag==false || this->floodplain_elem==NULL){
		return distance;
	}
	else{
		distance=this->distance(this->floodplain_elem->get_mid_point());
	}

	return distance;
}
//Get the index of the coupled fpl-section
int Hyd_Coupling_Point_FP2CO::get_fpl_section_index(void){
	return this->fpl_section_id;
}
//Get the flag if a coupling due to a break is applicable
bool Hyd_Coupling_Point_FP2CO::get_break_flag(void){
	return this->break_flag;
}
//Get a flag if the break width is as same as the total overflow width => break is total
bool Hyd_Coupling_Point_FP2CO::get_break_is_total(void){
	if(abs(this->break_width-this->total_flow_width)<=constant::meter_epsilon){
		return true;
	}
	else{
		return false;
	}
}
//Set the flag if a coupling due to a break is applicable
void Hyd_Coupling_Point_FP2CO::set_break_flag(const bool flag){
	this->break_flag=flag;
}
//Set the break width; the break height is related to the base points
bool Hyd_Coupling_Point_FP2CO::add_break_width(const double delta_break_width){
	bool is_full_flag=true;
	if(this->break_flag==true){

		if(abs(this->break_width-this->total_flow_width)<=0.0){
			is_full_flag=true;
		}
		else{
			is_full_flag=false;
			this->break_width=this->break_width+delta_break_width;
			if(this->break_width>=this->total_flow_width){
				this->break_width=this->total_flow_width;
				this->overflow_width=this->total_flow_width-this->break_width;
			}
			else{
				this->overflow_width=this->total_flow_width-this->break_width;
			}
		}
	}
	else{
		is_full_flag=true;
	}

	return is_full_flag;
}
//Get the mid basepoint height for a break coupling (global)
double Hyd_Coupling_Point_FP2CO::get_mid_basepoint_height(void){
	return this->mid_basepoint;
}
//Get the fixed basepoint height for a break coupling (global)
double Hyd_Coupling_Point_FP2CO::get_fixed_basepoint_height(void){
	return this->mid_basepoint_point;
}
//Calculate the current delta h between coast and floodplain in case of an break
void Hyd_Coupling_Point_FP2CO::calculate_delta_h_break(const double co_waterlevel){
	if(this->coupling_flag==true && this->break_flag==true){
		double delta_fp=this->floodplain_elem->element_type->get_s_value()-this->break_height;
		double delta_co=co_waterlevel-this->break_height;
		if(delta_fp <= 0.0 && delta_co<=0.0){
			this->delta_h_fp2co_break=0.0;
		}
		else if(delta_fp > 0.0  && delta_co<=0.0){
			this->delta_h_fp2co_break=-1.0*delta_fp;
		}
		else if(delta_fp <= 0.0  && delta_co>0.0){
			this->delta_h_fp2co_break=delta_co;
		}
		else if(delta_fp > 0.0  && delta_co>0.0){
			this->delta_h_fp2co_break=delta_co-delta_fp;
		}

		//set boundary
		if(abs(this->delta_h_fp2co_break)<=constant::flow_epsilon){
			this->delta_h_fp2co_break=0.0;
		}
	}
	else{
		this->delta_h_fp2co_break=0.0;
	}

}
//Get a pointer to the current delta h between coast and floodplain in case of an break
double *Hyd_Coupling_Point_FP2CO::get_delta_h_break(void){
	return &this->delta_h_fp2co_break;
}
//Set the starting break width
void Hyd_Coupling_Point_FP2CO::set_begin_break_width(const double begin_break_width){
	if(this->break_flag==true){
		this->break_height=this->coastdikeline_height;
		this->break_width=begin_break_width;
		this->overflow_width=this->total_flow_width-this->break_width;
	}
}
//Set a reduction of the overflow height in the starting breach (slow reduction of the height due to numerical reasons)
bool Hyd_Coupling_Point_FP2CO::set_overflow_height_reduction(const double height_reduction){
	if(this->break_flag==true){
		bool break_init_finished=false;
		this->break_height=this->break_height-height_reduction;
		if(this->break_height<=this->mid_basepoint){
			this->break_height=this->mid_basepoint;
			break_init_finished=true;
			return break_init_finished;
		}
		else{
			break_init_finished=false;
			return break_init_finished;
		}
	}
	else{
		return false;
	}

}
//Set the mid basepoint height as the break height
void Hyd_Coupling_Point_FP2CO::set_mid_basepoint2breakheight(void){
	if(this->break_flag==true){
		this->break_height=this->mid_basepoint;
	}
}
//Set the flag, that a breach is stopped here to true
void Hyd_Coupling_Point_FP2CO::set_stop_break_flag(void){
	this->stop_break_flag=true;
}
//Get the flag, that a breach is stopped here
bool Hyd_Coupling_Point_FP2CO::get_stop_break_flag(void){
	return this->stop_break_flag;
}
//Set the index of the break class
void Hyd_Coupling_Point_FP2CO::set_index_break_class(const int index){
	this->index_break=index;
}
//Get the index of the break class
int Hyd_Coupling_Point_FP2CO::get_index_break_class(void){
	return this->index_break;
}
//Get the current discharge through a break
double Hyd_Coupling_Point_FP2CO::get_q_break(void){
	return this->current_q_break;
}
//Get the current velocity through a break
double Hyd_Coupling_Point_FP2CO::get_v_break(void){
	return this->coupling_v_break;
}
//Get the maximum waterlevel at this coupling point by the maximum delta waterlevel relevant for a break  
_hyd_max_values Hyd_Coupling_Point_FP2CO::get_max_h_2break(void){
	return this->max_h_2break;
}
//Get the delta waterlevel between river and floodplain at this coupling point
double Hyd_Coupling_Point_FP2CO::get_max_deltah_2break(void){
	return this->max_deltah2break;
}
//Get maximum waterlevel at this coupling point related to the mid base points of the profiles 
_hyd_max_values Hyd_Coupling_Point_FP2CO::get_max_h(void){
	return this->max_h;
}
//Reset the break width
void Hyd_Coupling_Point_FP2CO::reset_break_width(void){
	
	this->break_width=0.0;
	this->overflow_width=this->total_flow_width-this->break_width;
	this->index_break=-1;

}
//Reset the hydrological balance values and the maximum calculated values
void Hyd_Coupling_Point_FP2CO::reset_hydro_balance_max_values(void){
	_Hyd_Coupling_Point::reset_hydro_balance_max_values();

	this->max_coupling_v_break.maximum=0.0;
	this->max_coupling_v_break.time_point=0.0;

	this->max_h_2break.maximum=0.0;
	this->max_h_2break.time_point=0.0;
	this->max_deltah2break=0.0;
	this->max_h.maximum=0.0;
	this->max_h.time_point=0.0;
	this->reset_break_width();
	
	this->coupling_volume_break.volume_in=0.0;
	this->coupling_volume_break.volume_out=0.0;
	this->coupling_volume_break.volume_total=0.0;

	/*this->break_q_list.current_discharge.clear();
	this->break_q_list.smooth_discharge.clear();
	this->break_q_list.timepoint.clear();
	this->break_q_list.relevant=false;*/
}
//Reset the smoothing calculation members
void Hyd_Coupling_Point_FP2CO::reset_smoothing(void){
	_Hyd_Coupling_Point::reset_smoothing();


	this->old_q_break=0.0;


	this->grad_q_current=0.0;
	this->grad_q_before=0.0;
	this->oscilation_smoother=1.0;
	this->no_osci_counter=0.0;
	this->number_osci_counter=2.0;

	this->grad_q_break_current=0.0;
	this->grad_q_break_before=0.0;
	this->oscilation_smoother_break=1.0;
	this->no_osci_counter_break=0.0;
	this->number_osci_counter_break=2.0;
	//this->break_q_list.relevant=false;
	this->store_grad_q_before=0.0;
	this->store_oscilation_smoother=1.0;
	this->store_no_osci_counter=0.0;
	this->store_number_osci_counter=2.0;
	this->store_grad_q_break_before=0.0;
	this->store_oscilation_smoother_break=1.0;
	this->store_no_osci_counter_break=0.0;
	this->store_number_osci_counter_break=2.0;
	this->store_old_q_break=0.0;
	
}
////Output the discharges of break to file
//void Hyd_Coupling_Point_FP2CO::output_discharge_break_list(void){
//
//	if(this->break_q_list.relevant==false){
//		return;
//	}
//	ostringstream cout;
//	//set prefix for output
//	ostringstream prefix;
//	
//	prefix << "DB_CO2FP_"<<this->point_index<<"> ";
//	Sys_Common_Output::output_hyd->set_userprefix(prefix.str());
//	cout<<"  Id  "<<"   time  "<< "    q_current  "<< "   q_smooth   "<<endl;
//	for(int i=0; i< this->break_q_list.timepoint.count(); i++){
//		cout << "  " <<i<<"   " << this->break_q_list.timepoint.at(i)<< "  "<<this->break_q_list.current_discharge.at(i);
//		cout << "   "<< this->break_q_list.smooth_discharge.at(i)<<endl;
//
//	}
//
//	Sys_Common_Output::output_hyd->output_txt(&cout,true);
//	Sys_Common_Output::output_hyd->rewind_userprefix();
//
//}
//Get the maximum waterlevel gradient 
double Hyd_Coupling_Point_FP2CO::get_maximum_h_grad(void){
	double buffer=0.0;
	double buffer_max=0.0;

	buffer=abs(this->grad_q_break_current/this->min_area);
	if(buffer_max<buffer){
		buffer_max=buffer;
	}
	buffer=abs(this->grad_q_current/this->min_area);
	if(buffer_max<buffer){
		buffer_max=buffer;
	}
	

	return buffer_max;
}
//Copy operator
Hyd_Coupling_Point_FP2CO& Hyd_Coupling_Point_FP2CO::operator=(const Hyd_Coupling_Point_FP2CO& object){
	_Hyd_Coupling_Point::operator =(object);
	this->coastdikeline_index=object.coastdikeline_index;
	this->coastdikeline_height=object.coastdikeline_height;
	this->floodplain_elem_index=object.floodplain_elem_index;
	this->floodplain_elem=object.floodplain_elem;
	this->floodplain_index=object.floodplain_index;
	this->overflow_flag=object.overflow_flag;
	this->ptr_upstream_point=object.ptr_upstream_point;
	this->ptr_downstream_point=object.ptr_downstream_point;


	this->break_flag=object.break_flag;
	this->index_break=object.index_break;
	this->stop_break_flag=object.stop_break_flag;
	this->break_height=object.break_height;
	this->break_width=object.break_width;

	this->fpl_section_id=object.fpl_section_id;
	this->mid_basepoint=object.mid_basepoint;
	this->mid_basepoint_point=object.mid_basepoint_point;

	this->poleni_coefficient=object.poleni_coefficient;

	this->total_flow_width=object.total_flow_width;
	this->overflow_width=object.overflow_width;
	this->delta_h_fp2co_break=object.delta_h_fp2co_break;
	this->max_h_2break=object.max_h_2break;
	this->max_h=object.max_h;
	this->max_deltah2break=object.max_deltah2break;


	this->grad_q_current=object.grad_q_current;
	this->grad_q_before=object.grad_q_before;
	this->oscilation_smoother=object.oscilation_smoother;
	this->no_osci_counter=object.no_osci_counter;
	this->number_osci_counter=object.number_osci_counter;

	this->grad_q_break_current=object.grad_q_break_current;
	this->grad_q_break_before=object.grad_q_break_before;
	this->oscilation_smoother_break=object.oscilation_smoother_break;
	this->no_osci_counter_break=object.no_osci_counter_break;
	this->number_osci_counter_break=object.number_osci_counter_break;

	//this->break_q_list.relevant=object.break_q_list.relevant;

	this->store_grad_q_before=object.store_grad_q_before;
	this->store_oscilation_smoother=object.store_oscilation_smoother;
	this->store_no_osci_counter=object.store_no_osci_counter;
	this->store_number_osci_counter=object.store_number_osci_counter;
	this->store_grad_q_break_before=object.store_grad_q_break_before;
	this->store_oscilation_smoother_break=object.store_oscilation_smoother_break;
	this->store_no_osci_counter_break=object.store_no_osci_counter_break;
	this->store_old_q_break=object.store_old_q_break;
	this->store_number_osci_counter_break=object.store_number_osci_counter_break;

	return *this;
}
//_________
//private
//Syncronisation of the coupled models with the couplingspoint for overflow
void Hyd_Coupling_Point_FP2CO::syncronisation_coupled_models_overflow(const double timepoint, const double coast_water_level, const double h_one){
	double delta_h_coast=0.0;
	double delta_h_elem=0.0;
	double q_buff=0.0;

	delta_h_coast=coast_water_level-(this->coastdikeline_height);
	delta_h_elem=(this->floodplain_elem->get_z_value()+h_one)-(this->coastdikeline_height);

	//same waterlevels=> no flow
	if(abs(delta_h_coast-delta_h_elem)<constant::flow_epsilon){
		this->coupling_v=0.0;
		q_buff=0.0;
	}
	//both waterlevel are under the weirsill=> no flow
	else if(delta_h_coast<=0.0 && delta_h_elem<=0.0){
		this->coupling_v=0.0;
		q_buff=0.0;
	}
	//flow out of the coast model into the floodplain without submerged reduction
	else if(delta_h_coast> 0.0 && delta_h_elem <= 0.0){
		q_buff=-1.0*constant::Cfacweir*this->poleni_coefficient*pow(delta_h_coast,(3.0/2.0))*this->overflow_width;
		//calculate the velocity
		this->coupling_v=-1.0*q_buff/(this->overflow_width*delta_h_coast);
	}
	//flow out of the floodplain element into the coast model without submerged reduction
	else if(delta_h_coast<= 0.0 && delta_h_elem > 0.0){
		q_buff=constant::Cfacweir*this->poleni_coefficient*pow(delta_h_elem,(3.0/2.0))*this->overflow_width;
		//calculate the velocity
		this->coupling_v=-1.0*q_buff/(this->overflow_width*delta_h_elem);
	}
	//weirflow with submerged reduction
	else if(delta_h_coast > 0.0 && delta_h_elem > 0.0){
		//flow out of the coast model into the floodplain 
		if(delta_h_coast>delta_h_elem){
			q_buff=constant::Cfacweir*this->poleni_coefficient*pow(delta_h_coast,(3.0/2.0))*this->overflow_width;
			//reduction of the discharge (submerged weirflow)
			double reduction_term=(1.0-delta_h_elem/delta_h_coast);
			//replace the ^(1/3) by a fitted arctan-function; at the boundary they have the same values
			if(reduction_term<=0.000463529){
				q_buff=-1.0*q_buff*0.057965266895*atan(8984.365582471040*reduction_term);
			}
			else{
				q_buff=-1.0*q_buff*pow(reduction_term,(1.0/3.0));
			}
			//calculate the velocity
			this->coupling_v=-1.0*q_buff/(this->overflow_width*delta_h_coast);
		}
		//flow out of the floodplain element into the coast model
		else{
			q_buff=constant::Cfacweir*this->poleni_coefficient*pow(delta_h_elem,(3.0/2.0))*this->overflow_width;
			//reduction of the discharge (submerged weirflow)
			double reduction_term=(1.0-delta_h_coast/delta_h_elem);
			//replace the ^(1/3) by a fitted arctan-function; at the boundary they have the same values
			if(reduction_term<=0.000463529){
				q_buff=q_buff*0.057965266895*atan(8984.365582471040*reduction_term);
			}
			else{
				q_buff=q_buff*pow(reduction_term,(1.0/3.0));
			}
			//calculate the velocity
			this->coupling_v=-1.0*q_buff/(this->overflow_width*delta_h_elem);
		}
	}
	//set a boundary to the discharge
	if(abs(q_buff)<=this->discharge_boundary){
		q_buff=0.0;
		this->coupling_v=0.0;
	}
	//smooth it for numerical reasons
	q_buff=this->smooth_coupling_discharge(q_buff, &this->old_q);
	
	this->current_q=q_buff;
	this->calculate_maximum_values(timepoint, this->current_q, &this->max_coupling_v);
	this->calculate_hydrological_balance(this->current_q, &this->coupling_volume);
	//discharge to element
	this->floodplain_elem->element_type->add_coupling_discharge_co_overflow((-1.0)*this->current_q);

}
//Syncronisation of the coupled models with the couplingspoint for the discharge throught the break
void Hyd_Coupling_Point_FP2CO::syncronisation_coupled_models_break(const double timepoint, const double coast_water_level, const double h_one){
	double delta_h_co=0.0;
	double delta_h_elem=0.0;
	double q_buff=0.0;
	this->current_q_break=0.0;
	this->coupling_v_break=0.0;
	

	delta_h_co=coast_water_level-this->break_height;
	delta_h_elem=(this->floodplain_elem->get_z_value()+h_one)-this->break_height;

	//no width
	if(abs(this->break_width)<constant::flow_epsilon){
		this->coupling_v_break=0.0;
		q_buff=0.0;
	}
	//same waterlevels=> no flow
	else if(abs(delta_h_co-delta_h_elem)<constant::flow_epsilon){
		this->coupling_v_break=0.0;
		q_buff=0.0;
	}
	//both waterlevel are under the weirsill=> no flow
	else if(delta_h_co<=0.0 && delta_h_elem<=0.0){
		this->coupling_v_break=0.0;
		q_buff=0.0;
	}
	//flow out of the coast model into the floodplain without submerged reduction, without sideweir reduction 
	else if(delta_h_co> 0.0 && delta_h_elem <= 0.0){
		q_buff=-1.0*constant::Cfacweir*constant::poleni_const*pow(delta_h_co,(3.0/2.0))*this->break_width;
		//calculate the velocity
		this->coupling_v_break=-1.0*q_buff/(this->break_width*delta_h_co);
	}
	//flow out of the floodplain element into the coast model without submerged reduction, withtout sideweir reduction
	else if(delta_h_co<= 0.0 && delta_h_elem > 0.0){
		q_buff=constant::Cfacweir*constant::poleni_const*pow(delta_h_elem,(3.0/2.0))*this->break_width;
		//calculate the velocity
		this->coupling_v_break=-1.0*q_buff/(this->break_width*delta_h_elem);
	}
	//weir flow with submerged reduction
	else if(delta_h_co > 0.0 && delta_h_elem > 0.0){
		//flow out of the coast model into the floodplain without sideweir reduction 
		if(delta_h_co>delta_h_elem){
			q_buff=constant::Cfacweir*constant::poleni_const*pow(delta_h_co,(3.0/2.0))*this->break_width;
			//reduction of the discharge (submerged weirflow)
			double reduction_term=(1.0-delta_h_elem/delta_h_co);
			//replace the ^(1/3) by a fitted arctan-function; at the boundary they have the same values
			if(reduction_term<=0.000463529){
				q_buff=-1.0*q_buff*0.057965266895*atan(8984.365582471040*reduction_term);
			}
			else{
				q_buff=-1.0*q_buff*pow(reduction_term,(1.0/3.0));
			}
			//calculate the velocity
			this->coupling_v_break=-1.0*q_buff/(this->break_width*delta_h_co);
		}
		//flow out of the floodplain element into the river model without sideweir reduction 
		else{
			q_buff=constant::Cfacweir*constant::poleni_const*pow(delta_h_elem,(3.0/2.0))*this->break_width;
			//reduction of the discharge (submerged weirflow)
			double reduction_term=(1.0-delta_h_co/delta_h_elem);
			//replace the ^(1/3) by a fitted arctan-function; at the boundary they have the same values
			if(reduction_term<=0.000463529){
				q_buff=q_buff*0.057965266895*atan(8984.365582471040*reduction_term);
			}
			else{
				q_buff=q_buff*pow(reduction_term,(1.0/3.0));
			}
			//calculate the velocity
			this->coupling_v_break=-1.0*q_buff/(this->break_width*delta_h_elem);
		}
	}
	//set a boundary to the discharge
	if(abs(q_buff)<=this->discharge_boundary){
		q_buff=0.0;
		this->coupling_v_break=0.0;
	}
	


	//just for testing
	//if(q_buff!=0.0){
	//	this->break_q_list.relevant=true;
	//}
	//if(this->break_q_list.relevant==true){
	//	this->break_q_list.timepoint.append(timepoint);
	//	this->break_q_list.current_discharge.append(q_buff);
	//}
	
	//smooth it for numerical reasons
	q_buff=this->smooth_coupling_dikebreak_discharge(q_buff, &this->old_q_break);
	//if(this->break_q_list.relevant==true){
	//	this->break_q_list.smooth_discharge.append(q_buff);
	//}
	this->current_q_break=q_buff;


	this->calculate_maximum_values(timepoint, this->current_q_break, &this->max_coupling_v_break);
	this->calculate_hydrological_balance(this->current_q_break, &this->coupling_volume_break);
	//discharge to element
	this->floodplain_elem->element_type->add_coupling_discharge_co_dikebreak((-1.0)*this->current_q_break);
	

}
//Smooth the coupling discharge with the coupling discharge calculated one internal timestep before
double Hyd_Coupling_Point_FP2CO::smooth_coupling_discharge(const double q_current, double *old_q){
	double smooth_q=0.0;

	if(this->delta_t>0.0){
		this->grad_q_current=(q_current-*old_q)/(this->delta_t);
		if((this->grad_q_before>=0.0 && this->grad_q_current<0.0)|| (this->grad_q_before<=0.0 && this->grad_q_current>0.0)){
			this->oscilation_smoother=this->oscilation_smoother+this->number_osci_counter;
			this->number_osci_counter=2.0;
			this->no_osci_counter=0.0;
			if(this->oscilation_smoother>=100.0){
				this->oscilation_smoother=100.0;
			}

		}
		else{
			this->oscilation_smoother=this->oscilation_smoother-pow(this->no_osci_counter,0.75);
			this->no_osci_counter++;
			this->number_osci_counter=2.0;
			if(this->oscilation_smoother<1.0){
				this->oscilation_smoother=1.0;
			}
			
		}
		this->grad_q_before=this->grad_q_current;
	}

	

	
	smooth_q=q_current*(1.0/this->oscilation_smoother)+(*old_q)*(1.0-(1.0/this->oscilation_smoother));
	if(this->delta_t>0.0){
		this->grad_q_current=(smooth_q-*old_q)/(this->delta_t);
		this->grad_q_before=this->grad_q_current;
	}
	

	*old_q=smooth_q;
	return smooth_q;
}
//Smooth the coupling discharge with the coupling dikebreak discharge calculated one internal timestep before
double Hyd_Coupling_Point_FP2CO::smooth_coupling_dikebreak_discharge(const double q_current, double *old_q){
	double smooth_q=0.0;
	
	

	if(this->delta_t>0.0){
		this->grad_q_break_current=(q_current-*old_q)/(this->delta_t);
		if((this->grad_q_break_before>=0.0 && this->grad_q_break_current<0.0)|| (this->grad_q_break_before<=0.0 && this->grad_q_break_current>0.0)){
			this->oscilation_smoother_break=this->oscilation_smoother_break+this->number_osci_counter_break;
			this->number_osci_counter_break=2.0;
			this->no_osci_counter_break=0.0;
			if(this->oscilation_smoother_break>=100.0){
				this->oscilation_smoother_break=100.0;
			}

		}
		else{
			this->oscilation_smoother_break=this->oscilation_smoother_break-pow(this->no_osci_counter_break,0.75);
			this->no_osci_counter_break++;
			this->number_osci_counter_break=2.0;
			if(this->oscilation_smoother_break<1.0){
				this->oscilation_smoother_break=1.0;
			}
			
		}
		
		this->grad_q_break_before=this->grad_q_break_current;
	}


	smooth_q=q_current*(1.0/this->oscilation_smoother_break)+(*old_q)*(1.0-(1.0/this->oscilation_smoother_break));


	if(this->delta_t>0.0){
		this->grad_q_break_current=(smooth_q-*old_q)/(this->delta_t);
		this->grad_q_break_before=this->grad_q_break_current;
	}

	*old_q=smooth_q;

	
	return smooth_q;

}
//Calculate the maximum waterlevel at the maximim waterlevel difference between coast and floodplain
void Hyd_Coupling_Point_FP2CO::calculate_max_h2break(const double timepoint, const double coast_water_level){

	double buffer=0.0;
	if(this->floodplain_elem ==NULL){
		return;
	}
	if(this->break_width>0.0){
		return;
	}
	if(this->break_flag==false){
		return;
	}

	buffer=coast_water_level-this->mid_basepoint;
	
	if(buffer>0.0){
		

		if(coast_water_level-this->mid_basepoint>this->max_deltah2break){
			this->max_deltah2break=coast_water_level-this->mid_basepoint;
			this->max_h_2break.maximum=coast_water_level-this->mid_basepoint_point;
			this->max_h_2break.time_point=timepoint;
		}
		if(coast_water_level-this->mid_basepoint_point>this->max_h.maximum){
			this->max_h.maximum=coast_water_level-this->mid_basepoint_point;
			this->max_h.time_point=timepoint;
		}
	}

}
//Predict the values
void Hyd_Coupling_Point_FP2CO::predict_values(const int int_counter){
	if(this->delta_t>0.0){
		

		this->calc_h_one=this->floodplain_elem->element_type->get_h_value();
		this->gradient_h_one=(this->calc_h_one-this->old_calc_h_one)/this->delta_t;
		if(this->time_check==false){
			this->swap_grad_list(&this->gradient_list_h_one, this->gradient_h_one);		
			this->old_calc_h_one=this->calc_h_one;
		
			
		}
		else{
			
			this->replace_first_grad_list(&this->gradient_list_h_one, this->gradient_h_one);
		}

		//this->predicted_h_one=this->calc_h_one+(1.0/12.0)*(23.0*this->gradient_list_h_one.at(0)-16.0*this->gradient_list_h_one.at(1)+5.0*this->gradient_list_h_one.at(2))*this->delta_t;
		//this->predicted_h_one=this->calc_h_one+(0.5)*(3.0*this->gradient_list_h_one.at(0)-1.0*this->gradient_list_h_one.at(1))*this->delta_t;
		if(int_counter>3){
			//this->predicted_h_one=this->calc_h_one+(this->gradient_list_h_one.at(0))*this->delta_t;
			//explicit
			this->predicted_h_one = this->calc_h_one;
			//this->predicted_h_one=this->calc_h_one+(0.5)*(3.0*this->gradient_list_h_one.at(0)-1.0*this->gradient_list_h_one.at(1))*this->delta_t;
		}
		else{
			this->predicted_h_one=this->calc_h_one;
		}
	}

}
