#include "Hyd_Headers_Precompiled.h"
//#include "Hyd_Coupling_Point_RV2FP.h"

//constructor
Hyd_Coupling_Point_RV2FP::Hyd_Coupling_Point_RV2FP(void){
	this->river_profile_index_up=-1;
	this->river_profile_up=NULL;
	this->river_profile_index_down=-1;
	this->river_profile_down=NULL;
	this->floodplain_elem_index=-1;
	this->floodplain_elem=NULL;
	this->floodplain_index=-1;

	this->fpl_section_id=-1;

	this->first_found_elem_index=-1;
	this->mid_height=0.0;
	this->mid_basepoint=0.0;
	this->mid_basepoint_profile=0.0;
	this->mid_fac_up=0.0;
	this->mid_fac_down=0.0;
	this->mid_waterlevel=0.0;

	this->overflow_flag=true;
	this->overflow_flag_fixed=false;

	this->horizontal_backwater_flag=false;
	this->horizontal_backwater_flag_upstream=false;
	this->current_q_break=0.0;
	this->old_q_break = 0.0;
	this->coupling_v_break=0.0;

	this->break_flag=false;
	this->index_break=-1;
	this->stop_break_flag=false;
	this->break_height=0.0;
	this->break_width=0.0;

	this->total_flow_width=0.0;
	this->overflow_width=0.0;
	this->delta_h_rv2fp_break=0.0;
	this->max_h_2break.maximum=0.0;
	this->max_h_2break.time_point=0.0;
	this->max_h.maximum=0.0;
	this->max_h.time_point=0.0;
	this->max_deltah2break=0.0;


	this->grad_q_current=0.0;
	this->grad_q_before=0.0;
	this->oscilation_smoother=1.0;
	this->number_osci_counter=2.0;
	this->no_osci_counter=0.0;

	this->grad_q_break_current=0.0;
	this->grad_q_break_before=0.0;
	this->oscilation_smoother_break=1.0;
	this->number_osci_counter_break=2.0;
	this->no_osci_counter_break=0.0;

	this->predicted_h_two=0.0;
	this->corrected_h_two=0.0;
	this->gradient_list_h_two.clear();
	for(int i=0; i< constant::no_stored_grad; i++){
		this->gradient_list_h_two.append(0.0);
	}
	this->gradient_h_two=0.0;
	this->calc_h_two=0.0;
	this->old_calc_h_two=0.0;

	this->store_grad_q_before=0.0;
	this->store_oscilation_smoother=1.0;
	this->store_number_osci_counter=2.0;
	this->store_no_osci_counter=0.0;
	this->store_grad_q_break_before=0.0;
	this->store_oscilation_smoother_break=1.0;
	this->store_number_osci_counter_break=2.0;
	this->store_no_osci_counter_break=0.0;
	this->store_old_q_break=0.0;


	//count the memory
	Sys_Memory_Count::self()->add_mem(sizeof(Hyd_Coupling_Point_RV2FP), _sys_system_modules::HYD_SYS);

}
//Copy constructor
Hyd_Coupling_Point_RV2FP::Hyd_Coupling_Point_RV2FP(const Hyd_Coupling_Point_RV2FP& object){
	_Hyd_Coupling_Point::operator =(object);
	this->river_profile_index_up=object.river_profile_index_up;
	this->river_profile_up=object.river_profile_up;
	this->river_profile_index_down=object.river_profile_index_down;
	this->river_profile_down=object.river_profile_down;
	this->floodplain_index=object.floodplain_index;
	this->floodplain_elem_index=object.floodplain_elem_index;
	this->first_found_elem_index=object.first_found_elem_index;
	this->floodplain_elem=object.floodplain_elem;
	this->fpl_section_id=object.fpl_section_id;

	this->mid_basepoint=object.mid_basepoint;
	this->mid_basepoint_profile=object.mid_basepoint_profile;
	this->mid_height=object.mid_height;
	this->mid_fac_up=object.mid_fac_up;
	this->mid_fac_down=object.mid_fac_down;
	this->mid_waterlevel=object.mid_waterlevel;

	this->overflow_flag=object.overflow_flag;
	this->overflow_flag_fixed=object.overflow_flag_fixed;

	this->horizontal_backwater_flag=object.horizontal_backwater_flag;
	this->horizontal_backwater_flag_upstream=object.horizontal_backwater_flag_upstream;
	this->current_q_break = object.current_q_break;
	this->old_q_break = object.old_q_break;
	this->coupling_v_break = object.coupling_v_break;
	

	this->break_flag=object.break_flag;
	this->index_break=object.index_break;
	this->stop_break_flag=object.stop_break_flag;
	this->break_height=object.break_height;
	this->break_width=object.break_width;

	this->total_flow_width=object.total_flow_width;
	this->overflow_width=object.overflow_width;
	this->delta_h_rv2fp_break=object.delta_h_rv2fp_break;
	this->max_h_2break=object.max_h_2break;
	this->max_h=object.max_h;
	this->max_deltah2break=object.max_deltah2break;


	this->grad_q_current=object.grad_q_current;
	this->grad_q_before=object.grad_q_before;
	this->oscilation_smoother=object.oscilation_smoother;
	this->number_osci_counter=object.number_osci_counter;
	this->no_osci_counter=object.no_osci_counter;

	this->grad_q_break_current=object.grad_q_break_current;
	this->grad_q_break_before=object.grad_q_break_before;
	this->oscilation_smoother_break=object.oscilation_smoother_break;
	this->no_osci_counter_break=object.no_osci_counter_break;
	this->number_osci_counter_break=object.number_osci_counter_break;

	this->corrected_h_two=object.corrected_h_two;
	this->predicted_h_two=object.predicted_h_two;
	this->gradient_h_two=object.gradient_h_two;
	this->calc_h_two=object.calc_h_two;
	this->old_calc_h_two=object.old_calc_h_two;
	this->gradient_list_h_two.clear();
	for(int i=0; i< constant::no_stored_grad; i++){
		this->gradient_list_h_two.append(0.0);
	}
	this->store_grad_q_before=object.store_grad_q_before;
	this->store_oscilation_smoother=object.store_oscilation_smoother;
	this->store_number_osci_counter=object.store_number_osci_counter;
	this->store_no_osci_counter=object.store_no_osci_counter;
	this->store_grad_q_break_before=object.store_grad_q_break_before;
	this->store_oscilation_smoother_break=object.store_oscilation_smoother_break;
	this->store_no_osci_counter_break=object.store_no_osci_counter_break;
	this->store_number_osci_counter_break=object.store_number_osci_counter_break;
	this->store_old_q_break=object.store_old_q_break;

	//count the memory
	Sys_Memory_Count::self()->add_mem(sizeof(Hyd_Coupling_Point_RV2FP), _sys_system_modules::HYD_SYS);
	
}
//destructor
Hyd_Coupling_Point_RV2FP::~Hyd_Coupling_Point_RV2FP(void){
	//count the memory
	Sys_Memory_Count::self()->minus_mem(sizeof(Hyd_Coupling_Point_RV2FP), _sys_system_modules::HYD_SYS);

}
//_________
//public
//Set the pointer/index members (without the geometrical members) from the given point
void Hyd_Coupling_Point_RV2FP::set_couplingpoint_members(Hyd_Coupling_Point_RV2FP *object){

	//from this class
	this->river_profile_index_up=object->river_profile_index_up;
	this->river_profile_up=object->river_profile_up;
	this->river_profile_index_down=object->river_profile_index_down;
	this->river_profile_down=object->river_profile_down;
	this->floodplain_index=object->floodplain_index;
	this->floodplain_elem_index=object->floodplain_elem_index;
	this->set_pointer_floodplain_element(object->floodplain_elem);
	this->first_found_elem_index=object->first_found_elem_index;
	this->fpl_section_id=object->fpl_section_id;


}
//Output the header for the setted member (static))
void Hyd_Coupling_Point_RV2FP::output_header_setted_member(ostringstream *cout){
	*cout <<W(10) << "Id_rv_up," << W(10) << "Id_rv_down," << W(7) << "Id_fp," << W(11) << "Id_fp_elem,";
	*cout <<W(15) << "Coupling,"<< W(15) << "Overflow," << W(15) <<"Dist_upstr" << label::m<<"," <<W(15)<< "Dist_downstr" << label::m << ",";
	*cout << W(15)<< "abs_Height" <<label::m << ",";
	*cout << W(10)<< "Break," << W(16) << "Stop_break,";
	*cout << W(15)<< "Dist_inflow" <<label::m << ",";
	*cout << W(15)<< "x" << label::m << "," << W(17) << "y" <<label::m << ",";
	*cout << W(17)<<"Id_fpl_sec" << "," << W(7) << "mid_fac_down" << "," << W(7) << "mid_fac_up";
	*cout<< endl;
	Sys_Common_Output::output_hyd->output_txt(cout,true);
}
//Output the setted members
void Hyd_Coupling_Point_RV2FP::output_setted_members(ostringstream *cout){
	*cout <<W(8) << this->river_profile_index_up << "," << W(10) << this->river_profile_index_down << ",";
	*cout <<W(10) << this->floodplain_index << W(10)<<"," << this->floodplain_elem_index << ",";
	*cout <<W(15) << functions::convert_boolean2string(this->coupling_flag) << ",";
	*cout <<W(15) << functions::convert_boolean2string(this->overflow_flag) << ",";
	*cout <<W(15) <<P(2)<< FORMAT_FIXED_REAL << this->distance_down << ",";
	*cout <<W(21) <<P(2)<< FORMAT_FIXED_REAL << this->distance_up << ",";
	*cout <<W(17) <<P(2)<< FORMAT_FIXED_REAL << this->mid_height << ",";
	*cout <<W(17) << functions::convert_boolean2string(this->break_flag) << ",";
	*cout <<W(15) << functions::convert_boolean2string(this->stop_break_flag) << ",";
	*cout <<W(17) << this->distance_along_polysegment << ",";
	*cout <<W(21) <<P(2)<< FORMAT_FIXED_REAL << this->x_coordinate << ",";
	*cout <<W(21) <<P(2)<< FORMAT_FIXED_REAL << this->y_coordinate << ",";
	*cout <<W(21) <<P(0)<< FORMAT_FIXED_REAL << this->fpl_section_id << ",";
	*cout << W(21) << P(10) << FORMAT_FIXED_REAL << this->mid_fac_down << ",";
	*cout << W(21) << P(10) << FORMAT_FIXED_REAL << this->mid_fac_up;

	*cout<< endl;
	Sys_Common_Output::output_hyd->output_txt(cout, true);
}
//Set the indices for the upwards river profile (corresponding to the index of the polysegment of the river bank line) and the element index of the Hyd_Floodplain_Element, which is used for calculation
void Hyd_Coupling_Point_RV2FP::set_indices(const int up_river_profile, const int floodplain_model_elem){
	this->river_profile_index_up=up_river_profile;	
	this->river_profile_index_down=this->river_profile_index_up+1;	
	this->floodplain_elem_index=floodplain_model_elem;
}
//Set the pointers to the river profiles with help of the indices
void Hyd_Coupling_Point_RV2FP::set_pointer_river_profiles(Hyd_Model_River *river_model){

	if(this->river_profile_index_up<0 || this->river_profile_index_up>river_model->Param_RV.get_number_profiles()){
		this->river_profile_up=NULL;
		this->river_profile_down=NULL;
	}
	//inflow profile upwards
	else if(this->river_profile_index_up==0){
		this->river_profile_up=&(river_model->inflow_river_profile);
		this->river_profile_down=&(river_model->river_profiles[this->river_profile_index_down-1]);
	}
	//outflow profile downwards
	else if(this->river_profile_index_up==river_model->Param_RV.get_number_profiles()-2){
		this->river_profile_up=&(river_model->river_profiles[this->river_profile_index_up-1]);
		this->river_profile_down=&(river_model->outflow_river_profile);
	}
	//just inbetween profiles
	else{
		this->river_profile_up=&(river_model->river_profiles[this->river_profile_index_up-1]);
		this->river_profile_down=&(river_model->river_profiles[this->river_profile_index_down-1]);
	}

}
//Set the indexof the first found Hyd_Floodplain_Element; it is not necessarily used for calculation (e.g. river elements)
void Hyd_Coupling_Point_RV2FP::set_first_found_elem_index(const int index){
	this->first_found_elem_index=index;
}
//Set the pointers to the floodplain elements
void Hyd_Coupling_Point_RV2FP::set_pointer_floodplain_element(Hyd_Element_Floodplain *floodplain){
	this->floodplain_elem=floodplain;
	if(this->floodplain_elem!=NULL && this->floodplain_elem_index>=0){
		this->floodplain_elem->element_type->set_coupling_data();
	}
}
//Set the index of the floodplain element
void Hyd_Coupling_Point_RV2FP::set_index_floodplain_element(const int index){
	this->floodplain_elem_index=index;
}
//Get the index of the floodplain element
int Hyd_Coupling_Point_RV2FP::get_index_floodplain_element(void){
	return this->floodplain_elem_index;
}
//Set the index of the coupled floodplain
void Hyd_Coupling_Point_RV2FP::set_floodplain_index(const int index){
	this->floodplain_index=index;
}
//Get the index of the coupled floodplain
int Hyd_Coupling_Point_RV2FP::get_floodplain_index(void){
	return this->floodplain_index;
}
//Get the index of the coupled fpl-section
int Hyd_Coupling_Point_RV2FP::get_fpl_section_index(void){
	return this->fpl_section_id;
}
//Get the mid of waterlevel of the two profiles in the river
double Hyd_Coupling_Point_RV2FP::get_mid_waterlevel(void){
	return this->mid_waterlevel;
}
//Calculate the current mid-waterlevel via the factors in the river
void Hyd_Coupling_Point_RV2FP::calculate_mid_waterlevel(void){
	if(this->coupling_flag==true){
		this->horizontal_backwater_flag=false;
		this->horizontal_backwater_flag_upstream=false;
		//set the mid_waterlevel via the factors
		if(this->river_profile_down->typ_of_profile->get_actual_local_waterlevel_h()>constant::dry_hyd_epsilon && this->river_profile_up->typ_of_profile->get_actual_local_waterlevel_h()>constant::dry_hyd_epsilon){
			this->mid_waterlevel=this->mid_fac_down*this->river_profile_down->get_actual_global_waterlevel()+this->mid_fac_up*this->river_profile_up->get_actual_global_waterlevel();	
		}
		else if(this->river_profile_down->typ_of_profile->get_actual_local_waterlevel_h()<=constant::dry_hyd_epsilon&& this->river_profile_up->typ_of_profile->get_actual_local_waterlevel_h()>constant::dry_hyd_epsilon){
			//this->mid_waterlevel=this->river_profile_up->get_actual_global_waterlevel();
			this->mid_waterlevel = this->mid_fac_down*this->river_profile_down->get_actual_global_waterlevel() + this->mid_fac_up*this->river_profile_up->get_actual_global_waterlevel();

			this->horizontal_backwater_flag_upstream=true;
		}
		else if(this->river_profile_down->typ_of_profile->get_actual_local_waterlevel_h()>constant::dry_hyd_epsilon&& this->river_profile_up->typ_of_profile->get_actual_local_waterlevel_h()<=constant::dry_hyd_epsilon){
			//this->mid_waterlevel=this->river_profile_down->get_actual_global_waterlevel();
			this->mid_waterlevel = this->mid_fac_down*this->river_profile_down->get_actual_global_waterlevel() + this->mid_fac_up*this->river_profile_up->get_actual_global_waterlevel();

			this->horizontal_backwater_flag=true;
		}
		else if(this->river_profile_down->typ_of_profile->get_actual_local_waterlevel_h()<=constant::dry_hyd_epsilon && this->river_profile_up->typ_of_profile->get_actual_local_waterlevel_h()<=constant::dry_hyd_epsilon){
			this->mid_waterlevel=this->mid_fac_down*this->river_profile_down->typ_of_profile->get_global_z_min()+this->mid_fac_up*this->river_profile_up->typ_of_profile->get_global_z_min();
		}
		
	}
}
//Calculate the current delta h between river and floodplain in case of an break
void Hyd_Coupling_Point_RV2FP::calculate_delta_h_break(void){
	if(this->coupling_flag==true && this->break_flag==true){
		double delta_fp=this->floodplain_elem->element_type->get_s_value()-this->break_height;
		double delta_rv=this->mid_waterlevel-this->break_height;
		if(delta_fp <= 0.0 && delta_rv<=0.0){
			this->delta_h_rv2fp_break=0.0;
		}
		else if(delta_fp > 0.0  && delta_rv<=0.0){
			this->delta_h_rv2fp_break=-1.0*delta_fp;
		}
		else if(delta_fp <= 0.0  && delta_rv>0.0){
			this->delta_h_rv2fp_break=delta_rv;
		}
		else if(delta_fp > 0.0  && delta_rv>0.0){
			this->delta_h_rv2fp_break=delta_rv-delta_fp;
		}

		//set boundary
		if(abs(this->delta_h_rv2fp_break)<=constant::flow_epsilon){
			this->delta_h_rv2fp_break=0.0;
		}
	}
	else{
		this->delta_h_rv2fp_break=0.0;
	}

}
//Get a pointer to the current delta h between river and floodplain in case of an break
double *Hyd_Coupling_Point_RV2FP::get_delta_h_break(void){
	return &this->delta_h_rv2fp_break;
}
//Get the mid height for the overflow-coupling (global)
double Hyd_Coupling_Point_RV2FP::get_mid_height(void){
	return this->mid_height;
}
//Get the mid basepoint height for a break coupling (global)
double Hyd_Coupling_Point_RV2FP::get_mid_basepoint_height(void){
	return this->mid_basepoint;
}
//Get the mid basepoint height (just profile) for a break coupling (global)
double Hyd_Coupling_Point_RV2FP::get_mid_basepoint_height_profile(void){
	return this->mid_basepoint_profile;
}
//Get the pointer to the downstream river profile
_Hyd_River_Profile* Hyd_Coupling_Point_RV2FP::get_downstream_rvprofile(void){
	return this->river_profile_down;
}
//Get the pointer to the upstream river profile
_Hyd_River_Profile* Hyd_Coupling_Point_RV2FP::get_upstream_rvprofile(void){
	return this->river_profile_up;
}
//Get the index of the downstream river profile
int Hyd_Coupling_Point_RV2FP::get_index_downstream_rvprofile(void){
	if(this->river_profile_down==NULL){
		return -1;
	}
	else{
		return this->river_profile_down->get_profile_number();
	}
}
//Get the index to the upstream river profile
int Hyd_Coupling_Point_RV2FP::get_index_upstream_rvprofile(void){
	if(this->river_profile_up==NULL){
		return -1;
	}
	else{
		return this->river_profile_up->get_profile_number();
	}
}
//Get the pointer to the floodplain element
Hyd_Element_Floodplain* Hyd_Coupling_Point_RV2FP::get_floodplain_element(void){
	return this->floodplain_elem;
}
//Get the upwards factor for the mid-value calculation of the mid_height and the mid_waterlevel depending on the distances
double Hyd_Coupling_Point_RV2FP::get_upwards_mid_factor(void){
	return this->mid_fac_up;
}
//Get the downwards factor for the mid-value calculation of the mid_height and the mid_waterlevel depending on the distances
double Hyd_Coupling_Point_RV2FP::get_downwards_mid_factor(void){
	return this->mid_fac_down;
}
//Get the distance from the coupling point to the element mid point
double Hyd_Coupling_Point_RV2FP::get_distancecoupling2elem_mid(void){
	double distance=-1.0;
	if(this->coupling_flag==false || this->floodplain_elem==NULL){
		return distance;
	}
	else{
		distance=this->distance(this->floodplain_elem->get_mid_point());
	}

	return distance;
}
//Transfer the coupling characteristics of the coupled elements
void Hyd_Coupling_Point_RV2FP::transfer_coupling_characteristics(const bool left_river_flag){


	if(this->river_profile_index_up<0 || this->floodplain_elem_index<0 || this->river_profile_index_down<0){
		this->coupling_flag=false;
		return;
	}
	if(this->floodplain_elem->get_elem_type()==_hyd_elem_type::STANDARD_ELEM || this->floodplain_elem->get_elem_type()==_hyd_elem_type::DIKELINE_ELEM){
		this->coupling_flag=true;
		//left river bank
		if(left_river_flag==true){
			this->transfer_coupling_characteristics_leftbank();
		}
		//right river bank
		else{
			this->transfer_coupling_characteristics_rightbank();
		}
		//Daniel ok? If this is used no direct in coupling is possible
		//if(abs(this->distance_down)<=constant::meter_epsilon){
			//this->coupling_flag=false;
		//}
	}
	else{
		this->coupling_flag=false;
		return;
	}
}
//Reset the current coupling discharge of the points and the coupled element
void Hyd_Coupling_Point_RV2FP::reset_coupling_discharge(const bool left_river_flag){
	_Hyd_Coupling_Point::reset_coupling_discharge();

	if(this->coupling_flag==true){
		if(this->overflow_flag==true){
			this->floodplain_elem->element_type->reset_coupling_discharge_rv_overflow();
			if(left_river_flag==true){
				this->river_profile_up->reset_coupling_discharge_left_bank();
				this->river_profile_down->reset_coupling_discharge_left_bank();
			}
			else{
				this->river_profile_up->reset_coupling_discharge_right_bank();
				this->river_profile_down->reset_coupling_discharge_right_bank();
			}
		}
		if(this->break_flag==true){
			this->floodplain_elem->element_type->reset_coupling_discharge_rv_dikebreak();
			if(left_river_flag==true){
				this->river_profile_up->reset_dikebreak_coupling_discharge_left_bank();
				this->river_profile_down->reset_dikebreak_coupling_discharge_left_bank();
			}
			else{
				this->river_profile_up->reset_dikebreak_coupling_discharge_right_bank();
				this->river_profile_down->reset_dikebreak_coupling_discharge_right_bank();
			}
	

		}
	}
}
//Syncronisation of the coupled models with the couplingspoint
void Hyd_Coupling_Point_RV2FP::syncronisation_coupled_models(const double timepoint, const double delta_t, const bool left_river_flag, const bool time_check, const int internal_counter){

	_Hyd_Coupling_Point::syncronisation_coupled_models();
	this->delta_t=delta_t;
	this->time_check=time_check;

	if(this->coupling_flag==false){
		return;
	}
	
	else{
		//calculate the current midwaterlevel in the river
		this->calculate_mid_waterlevel();
		

		double h_one_buff=0.0;
		double h_two_buff=0.0;

		this->predict_values(internal_counter);
		h_one_buff=0.5*(this->predicted_h_one+this->calc_h_one);
		h_two_buff=0.5*(this->predicted_h_two+this->calc_h_two);

		if(this->time_check==true){
			this->store_grad_q_before=this->grad_q_before;
			this->store_oscilation_smoother=this->oscilation_smoother;
			this->store_number_osci_counter=this->number_osci_counter;
			this->store_no_osci_counter=this->no_osci_counter;
			this->store_grad_q_break_before=this->grad_q_break_before;
			this->store_oscilation_smoother_break=this->oscilation_smoother_break;
			this->store_number_osci_counter_break=this->number_osci_counter_break;
			this->store_no_osci_counter_break=this->no_osci_counter_break;
			this->store_old_q_break=this->old_q_break;
			this->store_old_q=this->old_q;
		
		}
		else{
			this->grad_q_before=this->store_grad_q_before;
			this->oscilation_smoother=this->store_oscilation_smoother;
			this->number_osci_counter=this->store_number_osci_counter;
			this->no_osci_counter=this->store_no_osci_counter;
			this->grad_q_break_before=this->store_grad_q_break_before;
			this->oscilation_smoother_break=this->store_oscilation_smoother_break;
			this->number_osci_counter_break=this->store_number_osci_counter_break;
			this->no_osci_counter_break=this->store_no_osci_counter_break;
			this->old_q_break=this->store_old_q_break;
			this->old_q=this->store_old_q;
		

		//left river bank
			if(left_river_flag==true){
				if(this->overflow_flag==true){
					this->syncronisation_coupled_models_overflow(timepoint, this->river_profile_up->get_overflow_poleni_left(), h_one_buff, h_two_buff);


					if(this->horizontal_backwater_flag==false && this->horizontal_backwater_flag_upstream==false){
						//discharge to river profile segment: division to the profiles weighted with the factors 
						this->river_profile_up->add_coupling_discharge_left_bank(this->current_q*this->mid_fac_up);
						this->river_profile_down->add_coupling_discharge_left_bank(this->current_q*this->mid_fac_down);
					}
					else if(this->horizontal_backwater_flag==true){
						//discharge to river profile segment: just to/out of the downstream profile 
						this->river_profile_down->add_coupling_discharge_left_bank(this->current_q);
					}
					else if(this->horizontal_backwater_flag_upstream==true){
						//discharge to river profile segment: just to/out of the upstream profile 
						this->river_profile_up->add_coupling_discharge_left_bank(this->current_q);
					}
				}
				if(this->break_flag==true && this->index_break>=0){
					this->syncronisation_coupled_models_break(timepoint, h_one_buff, h_two_buff);
					//discharge to river profile segment: division to the profiles weighted with the factors 
					if(this->horizontal_backwater_flag==false && this->horizontal_backwater_flag_upstream==false){	
						this->river_profile_up->add_dikebreak_coupling_discharge_left_bank(this->current_q_break*this->mid_fac_up);
						this->river_profile_down->add_dikebreak_coupling_discharge_left_bank(this->current_q_break*this->mid_fac_down);
					}
					else if(this->horizontal_backwater_flag==true){
						//discharge to river profile segment: just to/out of the downstream profile 
						this->river_profile_down->add_dikebreak_coupling_discharge_left_bank(this->current_q_break);

					}
					else if(this->horizontal_backwater_flag_upstream==true){
						//discharge to river profile segment: just to/out of the upstream profile 
						this->river_profile_up->add_dikebreak_coupling_discharge_left_bank(this->current_q_break);
					}

				}
				if(this->time_check==false){
					//calculate maximum waterlevel for a break
					this->calculate_max_h2break(true, timepoint);
				}
			}
			//right river bank
			else{
				if(this->overflow_flag==true){
					this->syncronisation_coupled_models_overflow(timepoint, this->river_profile_up->get_overflow_poleni_right(), h_one_buff, h_two_buff);
			
					//if (this->floodplain_elem_index == 285692) {
					//	//	
					//	ostringstream cout;
					//	cout << this->current_q << " mid_w " << this->mid_waterlevel << " pre " << this->predicted_h_two << " aver  " << h_two_buff << " mid_h " << this->mid_height << endl;
					//	Sys_Common_Output::output_hyd->output_txt(&cout, true);


					//}

					//discharge to river profile segment: division to the profiles weighted with the factors 
					if(this->horizontal_backwater_flag==false && this->horizontal_backwater_flag_upstream==false){
						this->river_profile_up->add_coupling_discharge_right_bank(this->current_q*this->mid_fac_up);
						this->river_profile_down->add_coupling_discharge_right_bank(this->current_q*this->mid_fac_down);
					}
					else if(this->horizontal_backwater_flag==true){
						//discharge to river profile segment: just to/out of the downstream profile 
						this->river_profile_down->add_coupling_discharge_right_bank(this->current_q);
					}
					else if(this->horizontal_backwater_flag_upstream==true){
						//discharge to river profile segment: just to/out of the upstream profile 
						this->river_profile_up->add_coupling_discharge_right_bank(this->current_q);
					}
				}

	


				if(this->break_flag==true && this->index_break>=0){
					this->syncronisation_coupled_models_break(timepoint, h_one_buff, h_two_buff);
					//discharge to river profile segment: division to the profiles weighted with the factors 
					if(this->horizontal_backwater_flag==false && this->horizontal_backwater_flag_upstream==false){
						this->river_profile_up->add_dikebreak_coupling_discharge_right_bank(this->current_q_break*this->mid_fac_up);
						this->river_profile_down->add_dikebreak_coupling_discharge_right_bank(this->current_q_break*this->mid_fac_down);
					}
					else if(this->horizontal_backwater_flag==true){
						//discharge to river profile segment: just to/out of the downstream profile 
						this->river_profile_down->add_dikebreak_coupling_discharge_right_bank(this->current_q_break);
					}
					else if(this->horizontal_backwater_flag_upstream==true){
						//discharge to river profile segment: just to/out of the upstream profile 
						this->river_profile_up->add_dikebreak_coupling_discharge_right_bank(this->current_q_break);
					}
				}
			}
		}
		if(this->time_check==false){
			//calculate maximum waterlevel for a break
			this->calculate_max_h2break(false, timepoint);
		}
	}
}
//Set the flag if a coupling due to a overflow is applicable
void Hyd_Coupling_Point_RV2FP::set_overflow_flag(const bool flag){
	this->overflow_flag=flag;
}
//Set the fixed overflow flag to true; it is not more dependent of the profiles
void Hyd_Coupling_Point_RV2FP::set_fixed_overflow_flag(void){
	this->overflow_flag_fixed=true;
}
//Get the flag if a coupling due to a break is applicable
bool Hyd_Coupling_Point_RV2FP::get_break_flag(void){
	return this->break_flag;
}
//Get a flag if the break width is as same as the total overflow width => break is total
bool Hyd_Coupling_Point_RV2FP::get_break_is_total(void){
	if(this->break_width==this->total_flow_width){
		return true;
	}
	else{
		return false;
	}
}
//Set the flag if a coupling due to a break is applicable
void Hyd_Coupling_Point_RV2FP::set_break_flag(const bool flag){
	this->break_flag=flag;
}
//Set the break width; the break height is related to the base points
bool Hyd_Coupling_Point_RV2FP::add_break_width(const double delta_break_width){
	bool is_full_flag=true;
	if(this->break_flag==true){

		if(abs(this->break_width-this->total_flow_width)<constant::meter_epsilon){
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
//Set the starting break width
void Hyd_Coupling_Point_RV2FP::set_begin_break_width(const double begin_break_width){
	if(this->break_flag==true){
		this->break_height=this->mid_height;
		this->break_width=begin_break_width;
		this->overflow_width=this->total_flow_width-this->break_width;
	}
}
//Set a reduction of the overflow height in the starting breach (slow reduction of the height due to numerical reasons)
bool Hyd_Coupling_Point_RV2FP::set_overflow_height_reduction(const double height_reduction){
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
void Hyd_Coupling_Point_RV2FP::set_mid_basepoint2breakheight(void){
	if(this->break_flag==true){
		this->break_height=this->mid_basepoint;
	}
}
//Set the flag, that a breach is stopped here to true
void Hyd_Coupling_Point_RV2FP::set_stop_break_flag(void){
	this->stop_break_flag=true;
}
//Get the flag, that a breach is stopped here
bool Hyd_Coupling_Point_RV2FP::get_stop_break_flag(void){
	return this->stop_break_flag;
}
//Set the index of the break class
void Hyd_Coupling_Point_RV2FP::set_index_break_class(const int index){
	this->index_break=index;
}
//Get the index of the break class
int Hyd_Coupling_Point_RV2FP::get_index_break_class(void){
	return this->index_break;
}
//Get the current discharge through a break
double Hyd_Coupling_Point_RV2FP::get_q_break(void){
	return this->current_q_break;
}
//Get the current velocity through a break
double Hyd_Coupling_Point_RV2FP::get_v_break(void){
	return this->coupling_v_break;
}
//Get the maximum waterlevel at this coupling point by the maximum delta waterlevel relevant for a break  
_hyd_max_values Hyd_Coupling_Point_RV2FP::get_max_h_2break(void){
	return this->max_h_2break;
}
//Get the delta waterlevel between river and floodplain at this coupling point
double Hyd_Coupling_Point_RV2FP::get_max_deltah_2break(void){
	return this->max_deltah2break;
}
//Get maximum waterlevel at this coupling point related to the mid base points of the profiles 
_hyd_max_values Hyd_Coupling_Point_RV2FP::get_max_h(void){
	return this->max_h;
}
//Reset the break width
void Hyd_Coupling_Point_RV2FP::reset_break_width(void){
	
	this->break_width=0.0;
	this->overflow_width=this->total_flow_width-this->break_width;
	this->index_break=-1;

}
//Reset the hydrological balance values and the maximum calculated values
void Hyd_Coupling_Point_RV2FP::reset_hydro_balance_max_values(void){
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
}
//Reset the smoothing calculation members
void Hyd_Coupling_Point_RV2FP::reset_smoothing(void){
	_Hyd_Coupling_Point::reset_smoothing();

	this->grad_q_current=0.0;
	this->grad_q_before=0.0;
	this->oscilation_smoother=1.0;
	this->number_osci_counter=2.0;
	this->no_osci_counter=0.0;

	this->grad_q_break_current=0.0;
	this->grad_q_break_before=0.0;
	this->oscilation_smoother_break=1.0;
	this->number_osci_counter_break=2.0;
	this->no_osci_counter_break=0.0;

	this->predicted_h_two=0.0;
	this->corrected_h_two=0.0;
	this->gradient_list_h_two.clear();
	for(int i=0; i< constant::no_stored_grad; i++){
		this->gradient_list_h_two.append(0.0);
	}
	this->gradient_h_two=0.0;
	this->calc_h_two=0.0;
	this->old_calc_h_two=0.0;

	this->store_grad_q_before=0.0;
	this->store_oscilation_smoother=1.0;
	this->store_number_osci_counter=2.0;
	this->store_no_osci_counter=0.0;
	this->store_grad_q_break_before=0.0;
	this->store_oscilation_smoother_break=1.0;
	this->store_number_osci_counter_break=2.0;
	this->store_no_osci_counter_break=0.0;
	this->store_old_q_break=0.0;

}
//Get the maximum waterlevel gradient 
double Hyd_Coupling_Point_RV2FP::get_maximum_h_grad(void){
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
Hyd_Coupling_Point_RV2FP& Hyd_Coupling_Point_RV2FP::operator=(const Hyd_Coupling_Point_RV2FP& object){
	_Hyd_Coupling_Point::operator =(object);
	this->river_profile_index_up=object.river_profile_index_up;
	this->river_profile_up=object.river_profile_up;
	this->river_profile_index_down=object.river_profile_index_down;
	this->river_profile_down=object.river_profile_down;
	this->floodplain_index=object.floodplain_index;
	this->fpl_section_id=object.fpl_section_id;
	this->floodplain_elem_index=object.floodplain_elem_index;
	this->first_found_elem_index=object.first_found_elem_index;
	this->floodplain_elem=object.floodplain_elem;
	this->mid_height=object.mid_height;
	this->mid_basepoint=object.mid_basepoint;
	this->mid_basepoint_profile=object.mid_basepoint_profile;
	this->mid_fac_up=object.mid_fac_up;
	this->mid_fac_down=object.mid_fac_down;
	this->mid_waterlevel=object.mid_waterlevel;


	this->overflow_flag=object.overflow_flag;
	this->overflow_flag_fixed=object.overflow_flag_fixed;
	this->break_flag=object.break_flag;
	this->index_break=object.index_break;
	this->stop_break_flag=object.stop_break_flag;
	this->break_height=object.break_height;
	this->break_width=object.break_width;
	this->total_flow_width=object.total_flow_width;
	this->overflow_width=object.overflow_width;
	this->delta_h_rv2fp_break=object.delta_h_rv2fp_break;
	this->max_h_2break=object.max_h_2break;
	this->max_deltah2break=object.max_deltah2break;
	this->max_h=object.max_h;

	this->horizontal_backwater_flag=object.horizontal_backwater_flag;
	this->horizontal_backwater_flag_upstream=object.horizontal_backwater_flag_upstream;
	this->current_q_break = object.current_q_break;
	this->coupling_v_break = object.coupling_v_break;
	this->grad_q_current=object.grad_q_current;
	this->old_q_break = object.old_q_break;
	this->grad_q_before=object.grad_q_before;
	this->oscilation_smoother=object.oscilation_smoother;
	this->number_osci_counter=object.number_osci_counter;
	this->no_osci_counter=object.no_osci_counter;

	this->grad_q_break_current=object.grad_q_break_current;
	this->grad_q_break_before=object.grad_q_break_before;
	this->oscilation_smoother_break=object.oscilation_smoother_break;
	this->no_osci_counter_break=object.no_osci_counter_break;
	this->number_osci_counter_break=object.number_osci_counter_break;

	this->corrected_h_two=object.corrected_h_two;
	this->predicted_h_two=object.predicted_h_two;
	this->gradient_h_two=object.gradient_h_two;
	this->calc_h_two=object.calc_h_two;
	this->old_calc_h_two=object.old_calc_h_two;

	this->store_grad_q_before=object.store_grad_q_before;
	this->store_oscilation_smoother=object.store_oscilation_smoother;
	this->store_number_osci_counter=object.store_number_osci_counter;
	this->store_no_osci_counter=object.store_no_osci_counter;
	this->store_grad_q_break_before=object.store_grad_q_break_before;
	this->store_oscilation_smoother_break=object.store_oscilation_smoother_break;
	this->store_no_osci_counter_break=object.store_no_osci_counter_break;
	this->store_number_osci_counter_break=object.store_number_osci_counter_break;
	this->store_old_q_break=object.store_old_q_break;

	return *this;
}
//______________
//private
//Transfer the coupling characteristics of the coupled elements for a left river bank (in flow direction)
void Hyd_Coupling_Point_RV2FP::transfer_coupling_characteristics_leftbank(void){
		//check if a overflow coupling is given by the profiles
		if(this->overflow_flag_fixed==false){
			if(this->river_profile_up->get_overflow_flag_left()==false ){
				this->overflow_flag=false;
			}
			else{
				this->overflow_flag=true;
			}
		}
		//check if a break is applied
		if(this->river_profile_down->get_break_flag_left()==true && this->river_profile_up->get_break_flag_left()==true){
			this->break_flag=true;
		}
		else{
			this->break_flag=false;
		}

		//set the fpl index, if available
		this->fpl_section_id=this->river_profile_up->get_fpl_section_id_left();
		

		//this->coupling_flag=true;
		//set the factors for the mid-value calculation depending on the distances
		double total_prof_distance=0.0;
		double distance_prof_up=0.0;
		double distance_prof_down=0.0;
		//calculate the distances to the profiles for a point between the coupling points => hmid
		total_prof_distance=abs(this->river_profile_down->typ_of_profile->get_first_point()->get_global_x_y_coordinates().distance(&(this->river_profile_up->typ_of_profile->get_first_point()->get_global_x_y_coordinates())));
		distance_prof_up=abs(this->distance(&(this->river_profile_up->typ_of_profile->get_first_point()->get_global_x_y_coordinates())));
		distance_prof_up=distance_prof_up-this->distance_down*0.5;
		distance_prof_down=abs(this->distance(&(this->river_profile_down->typ_of_profile->get_first_point()->get_global_x_y_coordinates())));
		//check for stop flags by special types and coupling points, which are caused by a riverprofile
		if(abs(distance_prof_down)<=constant::meter_epsilon && (this->river_profile_down->get_profile_type()==_hyd_profile_types::BRIDGE_TYPE || this->river_profile_down->get_profile_type()==_hyd_profile_types::WEIR_TYPE 
			|| this->river_profile_down->get_connection_type()==_hyd_connection_types::INFLOW_CONN || this->river_profile_down->get_connection_type()==_hyd_connection_types::OUTFLOW_CONN)){
			this->set_stop_break_flag();
		}
		distance_prof_down=distance_prof_down+this->distance_down*0.5;
		//set the factors via the distances
		this->mid_fac_down=(1.0-distance_prof_down/total_prof_distance);
		this->mid_fac_up=(1.0-distance_prof_up/total_prof_distance);

		//check for a weir profile
		if(this->river_profile_up->get_profile_type()==_hyd_profile_types::WEIR_TYPE){
			//full weight to the downstream profile
			this->mid_fac_down=1.0;
			this->mid_fac_up=0.0;
		}
		if(this->river_profile_down->get_profile_type()==_hyd_profile_types::WEIR_TYPE){
			//full weight to the upstream profile
			this->mid_fac_down=0.0;
			this->mid_fac_up=1.0;
		}
		//set the mid_height via the factors
		//this->mid_height=this->mid_fac_down*this->river_profile_down->typ_of_profile->get_first_point()->get_global_z_coordinate()+this->mid_fac_up*this->river_profile_up->typ_of_profile->get_first_point()->get_global_z_coordinate();
		this->mid_height=this->mid_fac_down*this->river_profile_down->get_height_left_bank_abs()+this->mid_fac_up*this->river_profile_up->get_height_left_bank_abs();
		//if the height of the element is above the river bank line set the element height
		this->mid_height=max(this->mid_height, this->floodplain_elem->get_z_value());


		//set the width
		this->total_flow_width=this->distance_down;
		this->overflow_width=this->total_flow_width;

		//set minimal area
		this->min_area=this->floodplain_elem->element_type->get_relevant_area();


		//special settings for a breaking
		if(this->break_flag==true){	
			this->mid_basepoint_profile=this->mid_fac_down*this->river_profile_down->get_left_basepoint()->get_global_z_coordinate()+this->mid_fac_up*this->river_profile_up->get_left_basepoint()->get_global_z_coordinate();
			this->mid_basepoint=this->mid_fac_down*this->river_profile_down->get_left_basepoint()->get_global_z_coordinate()+this->mid_fac_up*this->river_profile_up->get_left_basepoint()->get_global_z_coordinate();
			this->mid_basepoint=max(this->mid_basepoint, this->floodplain_elem->get_z_value());
			//check if the mid_base point is equal to the mid of the global z_min values; than increase the height by 0.05 m 
			double mid_zmin=this->mid_fac_down*this->river_profile_down->typ_of_profile->get_global_z_min()+this->mid_fac_up*this->river_profile_up->typ_of_profile->get_global_z_min();
			if(this->mid_basepoint<=mid_zmin+0.05){
				//increase the mid_basepoint, otherwise the riversgements gets numerical problems with to much outflow before getting dry
				this->mid_basepoint=this->mid_basepoint+0.05;
			}
			
			//there is no break, if floodplain is heigher than floodprotection line
			if(this->mid_basepoint>=this->mid_height){
				this->break_flag=false;
			}		
		}
}
//Transfer the coupling characteristics of the coupled elements for a right river bank (in flow direction)
void Hyd_Coupling_Point_RV2FP::transfer_coupling_characteristics_rightbank(void){
		//check if a overflow coupling is given by the profiles
		if(this->overflow_flag_fixed==false){
			if(this->river_profile_up->get_overflow_flag_right()==false){
				this->overflow_flag=false;
			}
			else{
				this->overflow_flag=true;
			}
		}

		//check if a break is applied
		if(this->river_profile_down->get_break_flag_right()==true && this->river_profile_up->get_break_flag_right()==true){
			this->break_flag=true;
		}
		else{
			this->break_flag=false;
		}

		//set the fpl index, if available
		this->fpl_section_id=this->river_profile_up->get_fpl_section_id_right();

		//this->coupling_flag=true;
		//set the factors for the mid-value calculation depending on the distances
		double total_prof_distance=0.0;
		double distance_prof_up=0.0;
		double distance_prof_down=0.0;
		//calculate the distances to the profiles
		total_prof_distance=abs(this->river_profile_down->typ_of_profile->get_last_point()->get_global_x_y_coordinates().distance(&(this->river_profile_up->typ_of_profile->get_last_point()->get_global_x_y_coordinates())));
		distance_prof_up=abs(this->distance(&(this->river_profile_up->typ_of_profile->get_last_point()->get_global_x_y_coordinates())));
		distance_prof_up=distance_prof_up-0.5*this->distance_down;
		distance_prof_down=abs(this->distance(&(this->river_profile_down->typ_of_profile->get_last_point()->get_global_x_y_coordinates())));
		//check for stop flags by special types and coupling points, which are caused by a riverprofile
		if(abs(distance_prof_down)<=constant::meter_epsilon && (this->river_profile_down->get_profile_type()==_hyd_profile_types::BRIDGE_TYPE || this->river_profile_down->get_profile_type()==_hyd_profile_types::WEIR_TYPE 
			|| this->river_profile_down->get_connection_type()==_hyd_connection_types::INFLOW_CONN || this->river_profile_down->get_connection_type()==_hyd_connection_types::OUTFLOW_CONN)){
			this->set_stop_break_flag();
		}
		distance_prof_down=distance_prof_down+0.5*this->distance_down;
		//set the factors via the distances
		this->mid_fac_down=(1.0-distance_prof_down/total_prof_distance);
		this->mid_fac_up=(1.0-distance_prof_up/total_prof_distance);
		//set the mid_height via the factors
		//this->mid_height=this->mid_fac_down*this->river_profile_down->typ_of_profile->get_last_point()->get_global_z_coordinate()+this->mid_fac_up*this->river_profile_up->typ_of_profile->get_last_point()->get_global_z_coordinate();
		this->mid_height=this->mid_fac_down*this->river_profile_down->get_height_right_bank_abs()+this->mid_fac_up*this->river_profile_up->get_height_right_bank_abs();
		//if the height of the element is above the river bank line set the element height
		this->mid_height=max(this->mid_height, this->floodplain_elem->get_z_value());

		//check for a weir profile
		if(this->river_profile_up->get_profile_type()==_hyd_profile_types::WEIR_TYPE || this->river_profile_up->get_profile_type()==_hyd_profile_types::BRIDGE_TYPE){
			//full weight to the downstream profile
			this->mid_fac_down=1.0;
			this->mid_fac_up=0.0;
		}
		if(this->river_profile_down->get_profile_type()==_hyd_profile_types::WEIR_TYPE || this->river_profile_down->get_profile_type()==_hyd_profile_types::BRIDGE_TYPE){
			//full weight to the upstream profile
			this->mid_fac_down=0.0;
			this->mid_fac_up=1.0;
		}

		//set the width
		this->total_flow_width=this->distance_down;
		this->overflow_width=this->total_flow_width;

		//set minimal area
		this->min_area=this->floodplain_elem->element_type->get_relevant_area();

		//special settings for a breaking
		if(this->break_flag==true){
			this->mid_basepoint_profile=this->mid_fac_down*this->river_profile_down->get_right_basepoint()->get_global_z_coordinate()+this->mid_fac_up*this->river_profile_up->get_right_basepoint()->get_global_z_coordinate();
			this->mid_basepoint=this->mid_fac_down*this->river_profile_down->get_right_basepoint()->get_global_z_coordinate()+this->mid_fac_up*this->river_profile_up->get_right_basepoint()->get_global_z_coordinate();
			this->mid_basepoint=max(this->mid_basepoint, this->floodplain_elem->get_z_value());
			//check if the mid_base point is equal to the mid of the global z_min values; than increase the height by 0.05 m 
			double mid_zmin=this->mid_fac_down*this->river_profile_down->typ_of_profile->get_global_z_min()+this->mid_fac_up*this->river_profile_up->typ_of_profile->get_global_z_min();
			if(this->mid_basepoint<=mid_zmin+0.05){
				//increase the mid_basepoint, otherwise the riversgements gets numerical problems with to much outflow before getting dry
				this->mid_basepoint=this->mid_basepoint+0.05;
			}
			//there is no break, if floodplain is heigher than floodprotection line
			if(this->mid_basepoint>=this->mid_height){
				this->break_flag=false;
			}
		}
}
//Syncronisation of the coupled models with the couplingspoint for overflow
void Hyd_Coupling_Point_RV2FP::syncronisation_coupled_models_overflow(const double timepoint, const double poleni, const double h_one, const double h_two){
	double delta_h_river=0.0;
	double delta_h_elem=0.0;
	double q_buff=0.0;
	
	delta_h_river=h_two-this->mid_height;
	//if(delta_h_river>0.0){
	//	cout <<"Test"<<endl;
	//}
	delta_h_elem=(this->floodplain_elem->get_z_value()+h_one)-this->mid_height;

	//no width
	if(abs(this->overflow_width)<constant::flow_epsilon){
		this->coupling_v=0.0;
		q_buff=0.0;
	}
	//same waterlevels=> no flow
	else if(abs(delta_h_river-delta_h_elem)<constant::dry_hyd_epsilon){
		this->coupling_v=0.0;
		q_buff=0.0;
	}
	//both waterlevel are under the weirsill=> no flow
	else if(delta_h_river<=0.0 && delta_h_elem<=0.0){
		this->coupling_v=0.0;
		q_buff=0.0;
	}
	//flow out of the river model into the floodplain without submerged reduction
	else if(delta_h_river> 0.0 && delta_h_elem <= 0.0){
		q_buff=-1.0*constant::Cfacweir*poleni*pow(delta_h_river,(3.0/2.0))*this->overflow_width;
		//factor for a side weir overflow (after "Technische Hydromechanik I", Bollrich [2000])
		q_buff=constant::side_weir_reduction*q_buff;
		//calculate the velocity
		this->coupling_v=-1.0*q_buff/(this->overflow_width*delta_h_river);
	}
	//flow out of the floodplain element into the river model without submerged reduction
	else if(delta_h_river<= 0.0 && delta_h_elem > 0.0){
		q_buff=constant::Cfacweir*poleni*pow(delta_h_elem,(3.0/2.0))*this->overflow_width;
		//calculate the velocity
		this->coupling_v=-1.0*q_buff/(this->overflow_width*delta_h_elem);
	}
	//flow without submerged reduction
	else if(delta_h_river > 0.0 && delta_h_elem > 0.0){
		//flow out of the river model into the floodplain with sideweir reduction
		if(delta_h_river>delta_h_elem){
			q_buff=constant::Cfacweir*poleni*pow(delta_h_river,(3.0/2.0))*this->overflow_width;
			//reduction of the discharge (submerged weirflow)
			double reduction_term=(1.0-delta_h_elem/delta_h_river);
			//replace the ^(1/3) by a fitted arctan-function; at the boundary they have the same values
			if(reduction_term<=0.000463529){
				q_buff=-1.0*q_buff*0.057965266895*atan(8984.365582471040*reduction_term);
			}
			else{
				q_buff=-1.0*q_buff*pow(reduction_term,(1.0/3.0));
			}		
			//factor for a side weir overflow (after "Technische Hydromechanik I", Bollrich [2000])
			q_buff=constant::side_weir_reduction*q_buff;
			//calculate the velocity
			this->coupling_v=-1.0*q_buff/(this->overflow_width*delta_h_river);
		}
		//flow out of the floodplain element into the river model without sideweir reduction 
		else{
			q_buff=constant::Cfacweir*poleni*pow(delta_h_elem,(3.0/2.0))*this->overflow_width;
			//reduction of the discharge (submerged weirflow)
			double reduction_term=(1.0-delta_h_river/delta_h_elem);
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
	this->floodplain_elem->element_type->add_coupling_discharge_rv_overflow((-1.0)*this->current_q);
}
//Syncronisation of the coupled models with the couplingspoint for the discharge throught the break
void Hyd_Coupling_Point_RV2FP::syncronisation_coupled_models_break(const double timepoint, const double h_one, const double h_two){
	double delta_h_river=0.0;
	double delta_h_elem=0.0;
	double q_buff=0.0;
	this->current_q_break=0.0;
	this->coupling_v_break=0.0;

	delta_h_river=h_two-this->break_height;
	delta_h_elem=(this->floodplain_elem->get_z_value()+h_one)-this->break_height;

	//no width
	if(abs(this->break_width)<constant::flow_epsilon){
		this->coupling_v_break=0.0;
		q_buff=0.0;
	}
	//same waterlevels=> no flow
	else if(abs(delta_h_river-delta_h_elem)<constant::flow_epsilon){
		this->coupling_v_break=0.0;
		q_buff=0.0;
	}
	//both waterlevel are under the weirsill=> no flow
	else if(delta_h_river<=0.0 && delta_h_elem<=0.0){
		this->coupling_v_break=0.0;
		q_buff=0.0;
	}
	//flow out of the river model into the floodplain without submerged reduction, with sideweir reduction
	else if(delta_h_river> 0.0 && delta_h_elem <= 0.0){
		q_buff=-1.0*constant::Cfacweir*constant::poleni_const*pow(delta_h_river,(3.0/2.0))*this->break_width;
		//factor for a side weir overflow (after "Technische Hydromechanik I", Bollrich [2000])
		q_buff=constant::side_weir_reduction*q_buff;
		//calculate the velocity
		this->coupling_v_break=-1.0*q_buff/(this->break_width*delta_h_river);
	}
	//flow out of the floodplain element into the river model without submerged reduction, withtout sideweir reduction
	else if(delta_h_river<= 0.0 && delta_h_elem > 0.0){
		q_buff=constant::Cfacweir*constant::poleni_const*pow(delta_h_elem,(3.0/2.0))*this->break_width;
		//calculate the velocity
		this->coupling_v_break=-1.0*q_buff/(this->break_width*delta_h_elem);
	}
	//weir flow with submerged reduction
	else if(delta_h_river > 0.0 && delta_h_elem > 0.0){
		//flow out of the river model into the floodplain with sideweir reduction
		if(delta_h_river>delta_h_elem){
			q_buff=constant::Cfacweir*constant::poleni_const*pow(delta_h_river,(3.0/2.0))*this->break_width;
			//reduction of the discharge (submerged weirflow)
			double reduction_term=(1.0-delta_h_elem/delta_h_river);
			//replace the ^(1/3) by a fitted arctan-function; at the boundary they have the same values
			if(reduction_term<=0.000463529){
				q_buff=-1.0*q_buff*0.057965266895*atan(8984.365582471040*reduction_term);
			}
			else{
				q_buff=-1.0*q_buff*pow(reduction_term,(1.0/3.0));
			}
			//factor for a side weir overflow (after "Technische Hydromechanik I", Bollrich [2000])
			q_buff=constant::side_weir_reduction*q_buff;
			//calculate the velocity
			this->coupling_v_break=-1.0*q_buff/(this->break_width*delta_h_river);
		}
		//flow out of the floodplain element into the river model without sideweir reduction 
		else{
			q_buff=constant::Cfacweir*constant::poleni_const*pow(delta_h_elem,(3.0/2.0))*this->break_width;
			//reduction of the discharge (submerged weirflow)
			double reduction_term=(1.0-delta_h_river/delta_h_elem);
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

	//smooth it for numerical reasons
	q_buff=this->smooth_coupling_dikebreak_discharge(q_buff, &this->old_q_break);
	this->current_q_break=q_buff;


	this->calculate_maximum_values(timepoint, this->current_q_break, &this->max_coupling_v_break);
	this->calculate_hydrological_balance(this->current_q_break, &this->coupling_volume_break);
	//discharge to element
	this->floodplain_elem->element_type->add_coupling_discharge_rv_dikebreak((-1.0)*this->current_q_break);
}
//Smooth the coupling discharge with the coupling discharge calculated one internal timestep before
double Hyd_Coupling_Point_RV2FP::smooth_coupling_discharge(const double q_current, double *old_q){
	double smooth_q=0.0;

	if(this->delta_t>0.0){
		this->grad_q_current=(q_current-*old_q)/(this->delta_t);
		if((this->grad_q_before>=0.0 && this->grad_q_current<0.0)|| (this->grad_q_before<=0.0 && this->grad_q_current>0.0)){
			this->oscilation_smoother=this->oscilation_smoother+this->number_osci_counter;
			//this->oscilation_smoother=this->oscilation_smoother+1.0;
			this->no_osci_counter=0.0;
			this->number_osci_counter=2.0;
			if(this->oscilation_smoother>=100.0){
				this->oscilation_smoother=100.0;
			}

		}
		else{
			this->oscilation_smoother=this->oscilation_smoother-pow(this->no_osci_counter,0.75);
			//this->oscilation_smoother=this->oscilation_smoother-pow(this->no_osci_counter,1.5);
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
double Hyd_Coupling_Point_RV2FP::smooth_coupling_dikebreak_discharge(const double q_current, double *old_q){
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
//Calculate the maximum waterlevel at the maximim waterlevel difference between river and floodplain
void Hyd_Coupling_Point_RV2FP::calculate_max_h2break(const bool left_flag, const double timepoint){

	double buffer=0.0;
	if(this->river_profile_up==NULL || this->floodplain_elem ==NULL || this->river_profile_up->typ_of_profile->get_actual_local_waterlevel_h()<=constant::dry_hyd_epsilon){
		return;
	}
	if(this->break_width>0.0){
		return;
	}
	if(this->break_flag==false){
		return;
	}

	buffer=this->mid_waterlevel-this->mid_basepoint;
	
	if(buffer>0.0){
		this->river_profile_up->set_delta_waterlevel_rv2fp2break(this->mid_waterlevel, buffer, left_flag);

		if(this->mid_waterlevel-this->mid_basepoint>this->max_deltah2break){
			this->max_deltah2break=this->mid_waterlevel-this->mid_basepoint;
			this->max_h_2break.maximum=this->mid_waterlevel-this->mid_basepoint_profile;
			this->max_h_2break.time_point=timepoint;
		}
		if(this->mid_waterlevel-this->mid_basepoint_profile>this->max_h.maximum){
			this->max_h.maximum=this->mid_waterlevel-this->mid_basepoint_profile;
			this->max_h.time_point=timepoint;
		}
	}

}
//Predict the values
void Hyd_Coupling_Point_RV2FP::predict_values(const int int_counter){
	if(this->delta_t>0.0){
		//one
		this->calc_h_one=this->floodplain_elem->element_type->get_h_value();

		this->gradient_h_one=(this->calc_h_one-this->old_calc_h_one)/this->delta_t;
		if(this->time_check==false){
			this->old_calc_h_one=this->calc_h_one;
			this->replace_first_grad_list(&this->gradient_list_h_one, this->gradient_h_one);
			
		}
		else{
			this->swap_grad_list(&this->gradient_list_h_one, this->gradient_h_one);
		}

		//this->predicted_h_one=this->calc_h_one+(1.0/12.0)*(23.0*this->gradient_list_h_one.at(0)-16.0*this->gradient_list_h_one.at(1)+5.0*this->gradient_list_h_one.at(2))*this->delta_t;
		//this->predicted_h_one=this->calc_h_one+(0.5)*(3.0*this->gradient_list_h_one.at(0)-1.0*this->gradient_list_h_one.at(1))*this->delta_t;
		//this->predicted_h_one=this->calc_h_one+(this->gradient_list_h_one.at(0))*this->delta_t;

		//two
		this->calc_h_two=this->mid_waterlevel;

		this->gradient_h_two=(this->calc_h_two-this->old_calc_h_two)/this->delta_t;
		if(this->time_check==false){
			this->old_calc_h_two=this->calc_h_two;
			this->replace_first_grad_list(&this->gradient_list_h_two, this->gradient_h_two);
		}
		else{
			this->swap_grad_list(&this->gradient_list_h_two, this->gradient_h_two);
			
		}
		if(int_counter>3){
			//this->predicted_h_two=this->calc_h_two+(1.0/12.0)*(23.0*this->gradient_list_h_two.at(0)-16.0*this->gradient_list_h_two.at(1)+5.0*this->gradient_list_h_two.at(2))*this->delta_t;
			//this->predicted_h_two=this->calc_h_two+(0.5)*(3.0*this->gradient_list_h_two.at(0)-1.0*this->gradient_list_h_two.at(1))*this->delta_t;
			//this->predicted_h_two=this->calc_h_two+(this->gradient_list_h_two.at(0))*this->delta_t;
			//this->predicted_h_one=this->calc_h_one+(this->gradient_list_h_one.at(0))*this->delta_t;
			//changed to explicit!!!
			this->predicted_h_two = this->calc_h_two;
			this->predicted_h_one = this->calc_h_one;
		}
		else{
			this->predicted_h_two=this->calc_h_two;
			this->predicted_h_one=this->calc_h_one;
		}

	}
}