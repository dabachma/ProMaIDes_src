#include "Hyd_Headers_Precompiled.h"
//#include "Hyd_Element_Floodplain_Type_Standard.h"

//constructor
Hyd_Element_Floodplain_Type_Standard::Hyd_Element_Floodplain_Type_Standard(void): infiltration_rate(2.0e-6){
	//boundary condtion and coupling members members

	this->coupling_flag=false;
	this->coup_mem=NULL;
	this->boundary_flag=false;
	this->bound_mem=NULL;

	//calculation members
	this->init_flowdata_structures(&(this->flow_data));
	this->x_width=NULL;
	this->y_width=NULL;
	this->area=NULL;
	this->s_max_x=0.0;
	this->s_max_y=0.0;
	this->c_x=0.0;
	this->c_y=0.0;
	this->element_x=NULL;
	this->element_y=NULL;
	this->element_x_minus=NULL;
	this->element_y_minus=NULL;


	//result members
	this->s_value=0.0;
	this->h_value=0.0;
	this->ds_dt_value=0.0;
	this->v_x=0.0;
	this->v_y=0.0;
	this->v_x_out = 0.0;
	this->v_y_out = 0.0;
	this->v_total=0.0;
	this->wet_flag=false;
	this->time_wet_start=0.0;
	this->wet_duration=0.0;
	//maximum result variables
	this->max_h_value.maximum=0.0;
	this->max_ds_dt.maximum=0.0;	
	this->max_v_x.maximum=0.0;
	this->max_v_y.maximum=0.0;
	this->max_v_total.maximum=0.0;
	this->max_h_value.time_point=-1.0;
	this->max_ds_dt.time_point=-1.0;	
	this->max_v_x.time_point=-1.0;
	this->max_v_y.time_point=-1.0;
	this->max_v_total.time_point=-1.0;
	this->max_hv.time_point=-1.0;
	this->max_hv.maximum=0.0;
	this->first_arrival_time=-1.0;

	
	this->ds_dt_coup_old=0.0;
	this->ds_dt_coup=0.0;

	this->watervolume=0.0;
	this->watervolume_init=0.0;

	

	//count the memory
	Sys_Memory_Count::self()->add_mem(sizeof(Hyd_Element_Floodplain_Type_Standard), _sys_system_modules::HYD_SYS);
}
//destructor
Hyd_Element_Floodplain_Type_Standard::~Hyd_Element_Floodplain_Type_Standard(void){
	
	this->delete_bound_data();
	this->delete_coup_data();
	//count the memory
	Sys_Memory_Count::self()->minus_mem(sizeof(Hyd_Element_Floodplain_Type_Standard), _sys_system_modules::HYD_SYS);
}
//____________
//public
//Initialize the element (call it backward: not from the origin; start at the diagonal corner of the origin)
void Hyd_Element_Floodplain_Type_Standard::init_element(_hyd_neighbouring_elems neigh_elem, double *width_x, double *width_y, double *area, Hyd_Param_Material *mat_table, const bool clone){
	//set the width
	this->x_width=width_x;
	this->y_width=width_y;
	this->area=area;
	

	//set the data for the manning claculation and if requiered for the poleni in x-direction
	this->flow_data.n_value=mat_table->get_mat_value(this->flow_data.mat_type, _hyd_mat_coefficient_types::manning_coefficient);
	//x-direction
	if(neigh_elem.x_direction!=NULL){
		this->element_x=neigh_elem.x_direction;
		if(this->element_x->get_elem_type()==_hyd_elem_type::DIKELINE_ELEM || this->element_x->get_elem_type()==_hyd_elem_type::STANDARD_ELEM){
			//manning
			this->c_x=(1.0/this->flow_data.n_value+1.0/this->element_x->get_flow_data().n_value)*0.5;
			this->c_x=this->c_x/pow(*this->x_width,1.5);
			this->s_max_x=max((*this->z_value),this->element_x->get_z_value());
			//poleni (set a boundary)
			if(this->flow_data.poleni_flag_x==true){
				//set the absolut weir height
				this->flow_data.height_border_x_abs=max(this->s_max_x, this->flow_data.height_border_x_abs);
				if (clone == false) {
					this->flow_data.poleni_x = this->flow_data.poleni_x / (*this->x_width);
				}
			}
		}
		else{
			this->flow_data.no_flow_x_flag=true;
		}
	}
	else{
		this->flow_data.no_flow_x_flag=true;
	}
	//set the data for the manning calculation and if requiered for the poleni in y-direction
	if(neigh_elem.y_direction!=NULL){
		this->element_y=neigh_elem.y_direction;
		if(this->element_y->get_elem_type()==_hyd_elem_type::DIKELINE_ELEM || this->element_y->get_elem_type()==_hyd_elem_type::STANDARD_ELEM){
			//manning
			this->c_y=(1.0/this->flow_data.n_value+1.0/this->element_y->get_flow_data().n_value)*0.5;
			this->c_y=this->c_y/pow(*this->y_width,1.5);
			this->s_max_y=max((*this->z_value),this->element_y->get_z_value());
			//poleni (set a boundary)
			if(this->flow_data.poleni_flag_y==true){
				//set the absolut weir height
				this->flow_data.height_border_y_abs=max(this->s_max_y, this->flow_data.height_border_y_abs);
				if (clone == false) {
					this->flow_data.poleni_y = this->flow_data.poleni_y / (*this->y_width);
				}
			}
		}
		else{
			this->flow_data.no_flow_y_flag=true;
		}
	}
	else{
		this->flow_data.no_flow_y_flag=true;
	}

	//set the other element pointer
	this->element_x_minus=neigh_elem.minus_x_direction;
	this->element_y_minus=neigh_elem.minus_y_direction;

	//calculate the initial watervolume for a hydrological balance
	this->watervolume_init=this->flow_data.init_condition*(*this->area);
}
//Connect the element with a instationary boundary curve if requiered
void Hyd_Element_Floodplain_Type_Standard::connect_instat_boundarycurve(Hyd_Instationary_Boundary *bound_curve){
	if(this->boundary_flag==false){
		return;	
	}
	
	if(this->bound_mem->boundary_data.stationary_flag==true){
		this->bound_mem->boundary_data.found_flag=true;
		return;
	}
	else{
		//check if it is the right curve
		if(this->bound_mem->boundary_data.curve_number==bound_curve->get_curve_number()){
			try{
				bound_curve->set_type(this->bound_mem->boundary_data.boundary_type);
			}
			catch(Error msg){
				ostringstream info;
				info <<"Curve Number : "<< this->bound_mem->boundary_data.curve_number << endl;
				msg.make_second_info(info.str());
				throw msg;
			}
			this->bound_mem->boundary_data.ptr_boundarycurve=bound_curve;
			this->bound_mem->boundary_data.found_flag=true;
		}
	}
}
//Calculate the hydolocigal balance for boundary condition (every syncronisation step)
void Hyd_Element_Floodplain_Type_Standard::calculate_hydrolocigal_balance_boundary(const double delta_t){
	//boundary
	if(this->boundary_flag==true){
		this->bound_mem->calculate_hydro_balance(delta_t);
	}
}
//Calculate the hydolocigal balance for coupling (every syncronisation step)
void Hyd_Element_Floodplain_Type_Standard::calculate_hydrolocigal_balance_coupling(const double delta_t){
	//coupling
	if(this->coupling_flag==true){
		this->coup_mem->calculate_hydro_balance(delta_t);
	}
}
//Calculate the boundary value to a given time point
void Hyd_Element_Floodplain_Type_Standard::calculate_boundary_value(const double time_point){
	
	if(this->boundary_flag==false){//no condition
		return;
	}
	else{
		this->bound_mem->calculate_bound_value(time_point, *this->area);
	}
}
//Calculate the maximum values and the wet duration
void Hyd_Element_Floodplain_Type_Standard::calc_max_values(const double time_point, const double wet_boundary){
	//calculate the maximum values
	this->calculate_maximum_values(time_point);
	//calculate the wet or dry flags
	this->calculate_wet_dry_duration(time_point, wet_boundary);
	//calculate volume
	this->calculate_watervolume_element();
}
//Set the result value
void Hyd_Element_Floodplain_Type_Standard::set_solver_result_value(const double result){
	
	//this->check_unphysical_waterlevels();
	if(result<=constant::dry_hyd_epsilon){
		
		this->h_value=0.0;
		this->s_value=(*this->z_value);
	}
	else{
		this->h_value=result;
		this->s_value=(*this->z_value)+this->h_value;
	}
}
//Get the global waterlevel (s_value)
double Hyd_Element_Floodplain_Type_Standard::get_s_value(void){
	return this->s_value;
}
//Get the local waterlevel (h_value)
double Hyd_Element_Floodplain_Type_Standard::get_h_value(void){
	return this->h_value;
}
//Calculate the variation of the global waterlevel over time
void Hyd_Element_Floodplain_Type_Standard::calculate_ds_dt(void){
	//TODO TIME???
	//flow in x-direction
	if(this->flow_data.no_flow_x_flag!=true){
		if(this->flow_data.poleni_flag_x==false){
			this->calculate_ds_dt_manning_x();
		}
		else{
			this->calculate_ds_dt_poleni_x();
		}
	}

	//flow in y-direction
	if(this->flow_data.no_flow_y_flag!=true){
		if(this->flow_data.poleni_flag_y==false){
			this->calculate_ds_dt_manning_y();
		}
		else{
			this->calculate_ds_dt_poleni_y();
		}

	}

}
///Calculate the flow velocities over time 
void Hyd_Element_Floodplain_Type_Standard::calculate_v_out(const double sin_value, const double cos_value) {
	//x-direction
	if (this->element_x_minus != NULL){
		this->v_x_out = (this->v_x + this->element_x_minus->element_type->get_flowvelocity_vx())*0.5;
	}
	else {
		this->v_x_out = (this->v_x + 0.0)*0.5;
	}

	//y-direction
	if (this->element_y_minus != NULL) {
		this->v_y_out = (this->v_y + this->element_y_minus->element_type->get_flowvelocity_vy())*0.5;
	}
	else {
		this->v_y_out = (this->v_y + 0.0)*0.5;
	}

	//turning
	this->v_x_out = cos_value * this->v_x_out + sin_value * this->v_y_out;
	this->v_y_out = cos_value * this->v_y_out - sin_value * this->v_x_out;


}
//Set a value to the the variation of the global waterlevel over time
void Hyd_Element_Floodplain_Type_Standard::set_ds2dt_value(const double value){
	this->ds_dt_value=value;
}
//Get the variation of the global waterlevel over time as result value ds_dt_value
double Hyd_Element_Floodplain_Type_Standard::get_ds2dt_value(void){
	return this->ds_dt_value;
}
//Get the flow velocity in x-direction
double Hyd_Element_Floodplain_Type_Standard::get_flowvelocity_vx(void){
	return this->v_x;
}
//Get the flow velocity in y-direction
double Hyd_Element_Floodplain_Type_Standard::get_flowvelocity_vy(void){
	return this->v_y;
}
//Get the flow velocity in x-direction for output
double Hyd_Element_Floodplain_Type_Standard::get_flowvelocity_vx_out(void) {
	return this->v_x_out;
}
//Get the flow velocity in y-direction for output
double Hyd_Element_Floodplain_Type_Standard::get_flowvelocity_vy_out(void) {
	return this->v_y_out;
}
//Get the total flow velocity
double Hyd_Element_Floodplain_Type_Standard::get_flowvelocity_vtotal(void){
	return this->v_total;
}
//Get the if the element is wet
double Hyd_Element_Floodplain_Type_Standard::get_wet_flag(void){
	return this->wet_flag;
}
//Get the relevant area of the element
double Hyd_Element_Floodplain_Type_Standard::get_relevant_area(void){
	return (*this->area);
}
//Get the ds2dt value due to coupling
double Hyd_Element_Floodplain_Type_Standard::get_coupling_ds2dt(void){
	return 0.0; //this->ds_dt_coup/(*this->area);
}
//Add the hydrological balance value of the boundary condition discharges to the given pointer
void Hyd_Element_Floodplain_Type_Standard::add_hydro_balance_boundary(_hyd_hydrological_balance *given){
	if(this->boundary_flag==false){
		return;
	}
	given->volume_in=given->volume_in+this->bound_mem->boundary_volume.volume_in;
	given->volume_out=given->volume_out+this->bound_mem->boundary_volume.volume_out;
	given->volume_total=given->volume_total+this->bound_mem->boundary_volume.volume_total;
}
//Add the hydrological balance value of the coupling discharges to the element due to a coupling between two floodplain models
void Hyd_Element_Floodplain_Type_Standard::add_hydro_balance_coupling_fp(_hyd_hydrological_balance *given){
	if(this->coupling_flag==false){
		return;
	}
	given->volume_in=given->volume_in+this->coup_mem->coupling_volume_fp.volume_in;
	given->volume_out=given->volume_out+this->coup_mem->coupling_volume_fp.volume_out;
	given->volume_total=given->volume_total+this->coup_mem->coupling_volume_fp.volume_total;
}
//Add the hydrological balance value of the coupling discharges to the element due to river overflow
void Hyd_Element_Floodplain_Type_Standard::add_hydro_balance_overflow_coupling_rv(_hyd_hydrological_balance *given){
	if(this->coupling_flag==false){
		return;
	}
	given->volume_in=given->volume_in+this->coup_mem->coupling_volume_overflow_rv.volume_in;
	given->volume_out=given->volume_out+this->coup_mem->coupling_volume_overflow_rv.volume_out;
	given->volume_total=given->volume_total+this->coup_mem->coupling_volume_overflow_rv.volume_total;
}
//Add the hydrological balance value of the coupling discharges to the element due to river coupling via a hydraulic structure
void Hyd_Element_Floodplain_Type_Standard::add_hydro_balance_structure_coupling_rv(_hyd_hydrological_balance *given){
	if(this->coupling_flag==false){
		return;
	}
	given->volume_in=given->volume_in+this->coup_mem->coupling_volume_structure_rv.volume_in;
	given->volume_out=given->volume_out+this->coup_mem->coupling_volume_structure_rv.volume_out;
	given->volume_total=given->volume_total+this->coup_mem->coupling_volume_structure_rv.volume_total;
}
//Add the hydrological balance value of the coupling discharges to the element due to river coupling via a dikebreak
void Hyd_Element_Floodplain_Type_Standard::add_hydro_balance_dikebreak_coupling_rv(_hyd_hydrological_balance *given){
	if(this->coupling_flag==false){
		return;
	}
	given->volume_in=given->volume_in+this->coup_mem->coupling_volume_dikebreak_rv.volume_in;
	given->volume_out=given->volume_out+this->coup_mem->coupling_volume_dikebreak_rv.volume_out;
	given->volume_total=given->volume_total+this->coup_mem->coupling_volume_dikebreak_rv.volume_total;
}
//Add the hydrological balance value of the coupling discharges to the element due to costal overflow
void Hyd_Element_Floodplain_Type_Standard::add_hydro_balance_overflow_coupling_co(_hyd_hydrological_balance *given){
	if(this->coupling_flag==false){
		return;
	}
	given->volume_in=given->volume_in+this->coup_mem->coupling_volume_overflow_co.volume_in;
	given->volume_out=given->volume_out+this->coup_mem->coupling_volume_overflow_co.volume_out;
	given->volume_total=given->volume_total+this->coup_mem->coupling_volume_overflow_co.volume_total;
}
//Add the hydrological balance value of the coupling discharges to the element due to coast-model coupling via a dikebreak
void Hyd_Element_Floodplain_Type_Standard::add_hydro_balance_dikebreak_coupling_co(_hyd_hydrological_balance *given){
	if(this->coupling_flag==false){
		return;
	}
	given->volume_in=given->volume_in+this->coup_mem->coupling_volume_dikebreak_co.volume_in;
	given->volume_out=given->volume_out+this->coup_mem->coupling_volume_dikebreak_co.volume_out;
	given->volume_total=given->volume_total+this->coup_mem->coupling_volume_dikebreak_co.volume_total;
}
//Add the hydrological balance value of the coupling discharges from a coupled river model directly from the outflow profile (coupling_q_rv_direct)
void Hyd_Element_Floodplain_Type_Standard::add_hydro_balance_direct_coupling_rv(_hyd_hydrological_balance *given){
	if(this->coupling_flag==false){
		return;
	}
	given->volume_in=given->volume_in+this->coup_mem->coupling_volume_direct_rv.volume_in;
	given->volume_out=given->volume_out+this->coup_mem->coupling_volume_direct_rv.volume_out;
	given->volume_total=given->volume_total+this->coup_mem->coupling_volume_direct_rv.volume_total;
}
//Get the hydrological balance value of the coupling discharges to the element due to a coupling between two floodplain models 
_hyd_hydrological_balance Hyd_Element_Floodplain_Type_Standard::get_hydro_balance_coupling_fp(void){
	if(this->coupling_flag==false){
		_hyd_hydrological_balance buffer;
		buffer.volume_in=0.0;
		buffer.volume_out=0.0;
		buffer.volume_total=0.0;
		return buffer;
	}
	return this->coup_mem->coupling_volume_fp;
}
//Get the hydrological balance value of the coupling discharges to the element due to river overflow 
_hyd_hydrological_balance Hyd_Element_Floodplain_Type_Standard::get_hydro_balance_overflow_coupling_rv(void){
	if(this->coupling_flag==false){
		_hyd_hydrological_balance buffer;
		buffer.volume_in=0.0;
		buffer.volume_out=0.0;
		buffer.volume_total=0.0;
		return buffer;
	}
	return this->coup_mem->coupling_volume_overflow_rv;
}
//Get the hydrological balance value of the coupling discharges to the element due to river coupling via a hydraulic structure 
_hyd_hydrological_balance Hyd_Element_Floodplain_Type_Standard::get_hydro_balance_structure_coupling_rv(void){
	if(this->coupling_flag==false){
		_hyd_hydrological_balance buffer;
		buffer.volume_in=0.0;
		buffer.volume_out=0.0;
		buffer.volume_total=0.0;
		return buffer;
	}
	return this->coup_mem->coupling_volume_structure_rv;
}
//Get the hydrological balance value of the coupling discharges to the element due to river coupling via a dikebreak 
_hyd_hydrological_balance Hyd_Element_Floodplain_Type_Standard::get_hydro_balance_dikebreak_coupling_rv(void){
	if(this->coupling_flag==false){
		_hyd_hydrological_balance buffer;
		buffer.volume_in=0.0;
		buffer.volume_out=0.0;
		buffer.volume_total=0.0;
		return buffer;
	}
	return this->coup_mem->coupling_volume_dikebreak_rv;
}
///Get the hydrological balance value of the coupling discharges to the element due to costal overflow 
_hyd_hydrological_balance Hyd_Element_Floodplain_Type_Standard::get_hydro_balance_overflow_coupling_co(void){
	if(this->coupling_flag==false){
		_hyd_hydrological_balance buffer;
		buffer.volume_in=0.0;
		buffer.volume_out=0.0;
		buffer.volume_total=0.0;
		return buffer;
	}
	return this->coup_mem->coupling_volume_overflow_co;
}
//Get the hydrological balance value of the coupling discharges to the element due to coast-model coupling via a dikebreak 
 _hyd_hydrological_balance Hyd_Element_Floodplain_Type_Standard::get_hydro_balance_dikebreak_coupling_co(void){
	if(this->coupling_flag==false){
		_hyd_hydrological_balance buffer;
		buffer.volume_in=0.0;
		buffer.volume_out=0.0;
		buffer.volume_total=0.0;
		return buffer;
	}
	 return this->coup_mem->coupling_volume_dikebreak_co;
}
//Get the hydrological balance value of the coupling discharges from a coupled river model directly from the outflow profile (coupling_q_rv_direct)
_hyd_hydrological_balance Hyd_Element_Floodplain_Type_Standard::get_hydro_balance_direct_coupling_rv(void){
	if(this->coupling_flag==false){
		_hyd_hydrological_balance buffer;
		buffer.volume_in=0.0;
		buffer.volume_out=0.0;
		buffer.volume_total=0.0;
		return buffer;
	}
	return this->coup_mem->coupling_volume_direct_rv;
}
//Get the total coupling inflow volume (without boundary conditions)
double Hyd_Element_Floodplain_Type_Standard::get_total_coupling_inflow_volume(void){
	if(this->coupling_flag==false){
		return 0.0;
	}
	double sum=this->coup_mem->get_sum_inflow();
	return sum;
}
//Get the total coupling outflow volume (without boundary conditions)
double Hyd_Element_Floodplain_Type_Standard::get_total_coupling_outflow_volume(void){
	if(this->coupling_flag==false){
		return 0.0;
	}
	double sum=this->coup_mem->get_sum_outflow();
	return sum;
}
//Reset all hydrologicla balance value
void Hyd_Element_Floodplain_Type_Standard::reset_hydro_balance(void){
	if(this->boundary_flag==true){
		this->bound_mem->reset_vol_q();
	}
	if(this->coupling_flag==true){
		this->coup_mem->reset_vol_q();
	}

	this->watervolume=0.0;
	this->watervolume_init=0.0;

	this->ds_dt_coup_old=0.0;
	this->ds_dt_coup=0.0;
}
//Get the watervolume in the element
double Hyd_Element_Floodplain_Type_Standard::get_watervolume_element(void){
	//update it
	this->calculate_watervolume_element(true);
	return this->watervolume;
}
//Get the watervolume in of the init-conditions
double Hyd_Element_Floodplain_Type_Standard::get_watervolume_init(void){
	return this->watervolume_init;
}
//Set the maximum results values directly
void Hyd_Element_Floodplain_Type_Standard::set_max_results_directly(const double waterlevel, const double max_v, const double duration, const double dh_dt){
	if(waterlevel>0.0){
		this->was_wet_flag=true;
		this->wet_flag=true;
		this->h_value=waterlevel;
	}
	else{
		return;	
	}

	this->max_h_value.maximum=waterlevel;
	this->max_ds_dt.maximum=dh_dt;	
	this->max_v_x.maximum=0.0;
	this->max_v_y.maximum=0.0;
	this->max_v_total.maximum=max_v;
	
	this->max_h_value.time_point=-1.0;
	this->max_ds_dt.time_point=-1.0;	
	this->max_v_x.time_point=-1.0;
	this->max_v_y.time_point=-1.0;
	this->max_v_total.time_point=-1.0;

	this->max_hv.time_point=-1.0;
	this->max_hv.maximum=waterlevel*max_v;

	this->wet_duration=duration;
}
//Get the maximum local waterlevel (h_value)
_hyd_max_values Hyd_Element_Floodplain_Type_Standard::get_max_h_value(void){
	return this->max_h_value;
}
//Get the maximal variation of the global waterlevel over time as result value (ds_dt_value)
_hyd_max_values Hyd_Element_Floodplain_Type_Standard::get_max_ds2dt(void){
	return this->max_ds_dt;
}
//Get the maximum flow velocity in x-direction
_hyd_max_values Hyd_Element_Floodplain_Type_Standard::get_max_v_x(void){
	return this->max_v_x;
}
//Get the maximum flow velocity in y-direction
_hyd_max_values Hyd_Element_Floodplain_Type_Standard::get_max_v_y(void){
	return this->max_v_y;
}
//Get the maximum total flow velocity
_hyd_max_values Hyd_Element_Floodplain_Type_Standard::get_max_v_total(void){
	return this->max_v_total;
}
//Get the maximum of the product flow velocity time waterlevel (hv)
_hyd_max_values Hyd_Element_Floodplain_Type_Standard::get_max_hv(void){
	return this->max_hv;
}
//Get the overall was_wet_flag
bool Hyd_Element_Floodplain_Type_Standard::get_was_wet_flag(void){
	return this->was_wet_flag;
}
//Get the arrivial time (time when first wet)
double Hyd_Element_Floodplain_Type_Standard::get_arrival_time(void){
	return this->first_arrival_time;
}
//Get the duration of overflooding
double Hyd_Element_Floodplain_Type_Standard::get_wet_duration(void){
	return this->wet_duration;
}
//Get the conveance in x-direction
double Hyd_Element_Floodplain_Type_Standard::get_cx(void){
	return this->c_x;
}
//Get the conveance in y-direction
double Hyd_Element_Floodplain_Type_Standard::get_cy(void){
	return this->c_y;
}
//Get the maximum of geodetic height in x-direction
double Hyd_Element_Floodplain_Type_Standard::get_smax_x(void){
	return this->s_max_x;
}
//Get the maximum of geodetic height in y-direction
double Hyd_Element_Floodplain_Type_Standard::get_smax_y(void){
	return this->s_max_y;
}
//Get the boundary discharge
double Hyd_Element_Floodplain_Type_Standard::get_bound_discharge(void){
	if(this->boundary_flag==false){
		return 0.0;
	}
	return this->bound_mem->q_boundary;
}
//Get the coupling discharge
double Hyd_Element_Floodplain_Type_Standard::get_coupling_discharge(void){
	if(this->coupling_flag==false){
		return 0.0;
	}
	return this->coup_mem->sum_coupling_discharge();
}
//Reset the discharge, which comming from coupled floodplain models (coupling_q_fp)
void Hyd_Element_Floodplain_Type_Standard::reset_coupling_discharge_fp(void){
	if(this->coupling_flag==false){
		return;
	}
	this->coup_mem->coupling_q_fp=0.0;
}
//Add coupling discharge, which comming from coupled floodplain models (coupling_q_fp) during syncronisation 
void Hyd_Element_Floodplain_Type_Standard::add_coupling_discharge_fp(const double discharge){
	if(this->coupling_flag==false){
		return;
	}
	this->coup_mem->coupling_q_fp=this->coup_mem->coupling_q_fp+discharge;
}
//Reset the discharge, which comming from coupled river models due to overflow (coupling_q_rv_overflow)
void Hyd_Element_Floodplain_Type_Standard::reset_coupling_discharge_rv_overflow(void){
	if(this->coupling_flag==false){
		return;
	}
	this->coup_mem->coupling_q_rv_overflow=0.0;
}
//Add coupling dicharge, which comming from coupled river models due to overflow (coupling_q_rv_overflow) during syncronisation 
void Hyd_Element_Floodplain_Type_Standard::add_coupling_discharge_rv_overflow(const double discharge){
	if(this->coupling_flag==false){
		return;
	}
	this->coup_mem->coupling_q_rv_overflow=this->coup_mem->coupling_q_rv_overflow+discharge;
}
//Reset the discharge, which comming from coupled river models due to hydraulic structure coupling (coupling_q_rv_structure)
void Hyd_Element_Floodplain_Type_Standard::reset_coupling_discharge_rv_structure(void){
	if(this->coupling_flag==false){
		return;
	}
	this->coup_mem->coupling_q_rv_structure=0.0;
}
//Add coupling discharge, which comming from coupled river models due to hydraulic structure coupling (coupling_q_rv_structure) during syncronisation 
void Hyd_Element_Floodplain_Type_Standard::add_coupling_discharge_rv_structure(const double discharge){
	if(this->coupling_flag==false){
		return;
	}
	this->coup_mem->coupling_q_rv_structure=this->coup_mem->coupling_q_rv_structure+discharge;
}
//Reset the discharge, which comming from coupled river models due to dikebreak coupling (coupling_q_rv_dikebreak)
void Hyd_Element_Floodplain_Type_Standard::reset_coupling_discharge_rv_dikebreak(void){
	if(this->coupling_flag==false){
		return;
	}
	this->coup_mem->coupling_q_rv_dikebreak=0.0;
}
//Add coupling discharge, which comming from coupled river models due to dikebreak coupling (coupling_q_rv_dikebreak) during syncronisation 
void Hyd_Element_Floodplain_Type_Standard::add_coupling_discharge_rv_dikebreak(const double discharge){
	if(this->coupling_flag==false){
		return;
	}
	this->coup_mem->coupling_q_rv_dikebreak=this->coup_mem->coupling_q_rv_dikebreak+discharge;
}
//Reset the discharge, which comming from a coupled coast model due to overflow (coupling_q_co_overflow)
void Hyd_Element_Floodplain_Type_Standard::reset_coupling_discharge_co_overflow(void){
	if(this->coupling_flag==false){
		return;
	}
	this->coup_mem->coupling_q_co_overflow=0.0;
}
//Add coupling discharge, which comming from a coupled coast model due to overflow (coupling_q_co_overflow) during syncronisation 
void Hyd_Element_Floodplain_Type_Standard::add_coupling_discharge_co_overflow(const double discharge){
	if(this->coupling_flag==false){
		return;
	}
	this->coup_mem->coupling_q_co_overflow=this->coup_mem->coupling_q_co_overflow+discharge;
}
//Reset the discharge, which comming from a coupled coast model due to dikebreak coupling (coupling_q_co_dikebreak)
void Hyd_Element_Floodplain_Type_Standard::reset_coupling_discharge_co_dikebreak(void){
	if(this->coupling_flag==false){
		return;
	}
	this->coup_mem->coupling_q_co_dikebreak=0.0;
}
//Add coupling discharge, which comming from a coupled coast model due to dikebreak coupling (coupling_q_co_dikebreak) during syncronisation 
void Hyd_Element_Floodplain_Type_Standard::add_coupling_discharge_co_dikebreak(const double discharge){
	if(this->coupling_flag==false){
		return;
	}
	this->coup_mem->coupling_q_co_dikebreak=this->coup_mem->coupling_q_co_dikebreak+discharge;

}
//Reset the discharge, which comming from a coupled river model directly from the outflow profile (coupling_q_rv_direct)
void Hyd_Element_Floodplain_Type_Standard::reset_coupling_discharge_rv_direct(void){
	if(this->coupling_flag==false){
		return;
	}
	this->coup_mem->coupling_q_rv_direct=0.0;
}
//Add coupling discharge, which comming from a coupled river model directly from the outflow profile (coupling_q_rv_direct)
void Hyd_Element_Floodplain_Type_Standard::add_coupling_discharge_rv_direct(const double discharge){
	if(this->coupling_flag==false){
		return;
	}
	this->coup_mem->coupling_q_rv_direct=this->coup_mem->coupling_q_rv_direct+discharge;
}
//Set the maximum result values of an element to an query string to transfer them into a database table (Hyd_Element_Floodplain
void Hyd_Element_Floodplain_Type_Standard::set_maximum_value2querystring(ostringstream *query_string){
	//max-values
	*query_string << this->max_h_value.maximum << " , " ;
	*query_string << this->max_h_value.maximum + (*this->z_value)<< " , " ;
	*query_string << this->max_ds_dt.maximum<< " , " ;
	*query_string << this->max_v_x.maximum << " , " ;
	*query_string << this->max_v_y.maximum << " , " ;
	*query_string << this->max_v_total.maximum << " , " ;
	*query_string << this->max_hv.maximum << " , " ;
	*query_string << this->first_arrival_time << " , " ;
	*query_string << this->wet_duration << " , " ;
	*query_string << this->watervolume  << " , " ;
	//volumes
	if(this->boundary_flag==true){
		*query_string << this->bound_mem->boundary_volume.volume_in << " , " ;
		*query_string << this->bound_mem->boundary_volume.volume_out << " , " ;
	}
	else{
		*query_string << 0.0 << " , " ;
		*query_string << 0.0 << " , " ;
	}
	if(this->coupling_flag==true){
		*query_string << this->coup_mem->coupling_volume_structure_rv.volume_in << " , " ;
		*query_string << this->coup_mem->coupling_volume_structure_rv.volume_out << " , " ;
		*query_string << this->coup_mem->coupling_volume_direct_rv.volume_in << " , " ;
		*query_string << this->coup_mem->coupling_volume_direct_rv.volume_out << " , " ;

		*query_string << this->coup_mem->coupling_volume_overflow_rv.volume_in << " , " ;
		*query_string << this->coup_mem->coupling_volume_overflow_rv.volume_out << " , " ;
		*query_string << this->coup_mem->coupling_volume_dikebreak_rv.volume_in << " , " ;
		*query_string <<this->coup_mem->coupling_volume_dikebreak_rv.volume_out << " , " ;

		*query_string << this->coup_mem->coupling_volume_overflow_co.volume_in << " , " ;
		*query_string << this->coup_mem->coupling_volume_overflow_co.volume_out << " , " ;
		*query_string << this->coup_mem->coupling_volume_dikebreak_co.volume_in << " , " ;
		*query_string << this->coup_mem->coupling_volume_dikebreak_co.volume_out << " , " ;

		*query_string << this->coup_mem->coupling_volume_fp.volume_in << " , " ;
		*query_string << this->coup_mem->coupling_volume_fp.volume_out << " , " ;
	}
	else{
		*query_string << 0.0 << " , " ;
		*query_string << 0.0  << " , " ;
		*query_string << 0.0  << " , " ;
		*query_string << 0.0  << " , " ;

		*query_string << 0.0  << " , " ;
		*query_string << 0.0  << " , " ;
		*query_string << 0.0  << " , " ;
		*query_string << 0.0  << " , " ;

		*query_string << 0.0  << " , " ;
		*query_string << 0.0  << " , " ;
		*query_string << 0.0  << " , " ;
		*query_string << 0.0 << " , " ;

		*query_string << 0.0  << " , " ;
		*query_string << 0.0  << " , " ;

	}
	
}
//Set the instationary result values of an element to an query string to transfer them into a database table (Hyd_Element_Floodplain
void Hyd_Element_Floodplain_Type_Standard::set_instat_value2querystring(ostringstream *query_string) {
	//instat-values
	*query_string << this->get_h_value()<< " , ";
	*query_string << this->get_s_value() << " , ";
	*query_string << this->get_ds2dt_value() << " , ";
	*query_string << this->get_flowvelocity_vx_out() << " , ";
	*query_string << this->get_flowvelocity_vy_out() << " , ";
	*query_string << this->get_flowvelocity_vtotal() << " , ";
	*query_string << this->get_h_value() *this->get_flowvelocity_vtotal() << " , ";
}
//Reset the hydrological balance value and the maximum values
void Hyd_Element_Floodplain_Type_Standard::reset_hydrobalance_maxvalues(void){

	_Hyd_Element_Floodplain_Type::reset_hydrobalance_maxvalues();

	this->s_value=(*this->z_value);
	this->h_value=0.0;
	this->v_total=0.0;
	this->v_x=0.0;
	this->v_y=0.0;
	this->v_x_out = 0.0;
	this->v_y_out = 0.0;
	this->ds_dt_value=0.0;
	this->wet_flag=false;
	this->time_wet_start=0.0;
	this->wet_duration=0.0;
	//maximum result variables
	this->max_h_value.maximum=0.0;
	this->max_ds_dt.maximum=0.0;	
	this->max_v_x.maximum=0.0;
	this->max_v_y.maximum=0.0;
	this->max_v_total.maximum=0.0;
	this->max_h_value.time_point=-1.0;
	this->max_ds_dt.time_point=-1.0;	
	this->max_v_x.time_point=-1.0;
	this->max_v_y.time_point=-1.0;
	this->max_v_total.time_point=-1.0;
	this->max_hv.maximum=0.0;
	this->max_hv.time_point=-1.0;
	this->first_arrival_time=-1.0;

	//coupling and boundary members
	this->reset_hydro_balance();

}
//Get the maximum change in a element 
void Hyd_Element_Floodplain_Type_Standard::get_max_changes_elements(double *max_change_h, const bool timecheck){

	if(this->coupling_flag == true || this->boundary_flag ==true){
		this->ds_dt_coup=0.0;
		if(this->coupling_flag==true){
			this->ds_dt_coup=this->coup_mem->sum_coupling_discharge();
		}
		if(this->boundary_flag==true){
			this->ds_dt_coup=this->ds_dt_coup+this->bound_mem->q_boundary;
		}

		this->ds_dt_coup=this->ds_dt_coup/(*this->area);
		*max_change_h=abs(this->ds_dt_coup_old-this->ds_dt_coup);

		if(timecheck==false){
			this->ds_dt_coup_old=this->ds_dt_coup;
		}

	}
	else{
		*max_change_h=0.0;
	}
	
}
//Get the coupling flag
bool Hyd_Element_Floodplain_Type_Standard::get_coupling_flag(void){
	return this->coupling_flag;
}
//Get boundary condition flag
bool Hyd_Element_Floodplain_Type_Standard::get_bound_flag(void){
	return this->boundary_flag;
}
//Delete the data of boundary condition
void Hyd_Element_Floodplain_Type_Standard::delete_bound_data(void){
	if(this->bound_mem!=NULL){
		delete this->bound_mem;
		this->bound_mem=NULL;
		this->boundary_flag=false;
	}
}
//Set coupling data
void Hyd_Element_Floodplain_Type_Standard::set_coupling_data(void){
	if(this->coupling_flag==true){
		return;
	}
	this->allocate_coup_data();
}
//________________
//private
//Get the flow data structure
_hyd_flowdata_floodplain_elem Hyd_Element_Floodplain_Type_Standard::get_flow_data(void){
	return this->flow_data;
}
//Get the boundary data structure
_hyd_boundarydata_floodplain_elem Hyd_Element_Floodplain_Type_Standard::get_boundary_data(void){
	if(this->boundary_flag==true){
		return this->bound_mem->boundary_data;
	}
	else{
		_hyd_boundarydata_floodplain_elem buffer;
		Hyd_Element_Bound_Members::init_boundarydata_structures(&buffer);
		return buffer;

	}
}
//Output the setted members
void Hyd_Element_Floodplain_Type_Standard::output_setted_members(ostringstream *cout){

	*cout << W(14) << P(2) << FORMAT_FIXED_REAL << (*this->z_value);
	//x-direction
	if(this->flow_data.no_flow_x_flag==true){
		*cout << W(14) << " NO-flow " << W(7) << "-" ;
	}
	else if(this->flow_data.no_flow_x_flag==false && this->flow_data.poleni_flag_x==true){
		*cout << W(14) << " POL-flow " << W(7) << P(2) << FORMAT_FIXED_REAL << this->flow_data.height_border_x_abs;
	}
	else if(this->flow_data.no_flow_x_flag==false && this->flow_data.poleni_flag_x==false){
		*cout << W(14) << " MAN-flow " << W(7) <<"-" ;
	}
	//y-direction
	if(this->flow_data.no_flow_y_flag==true){
		*cout << W(18) << " NO-flow " << W(7) << "-" ;
	}
	else if(this->flow_data.no_flow_y_flag==false && this->flow_data.poleni_flag_y==true){
		*cout << W(18) << " POL-flow " << W(7) << P(2) << FORMAT_FIXED_REAL << this->flow_data.height_border_y_abs;
	}
	else if(this->flow_data.no_flow_y_flag==false && this->flow_data.poleni_flag_y==false){
		*cout << W(18) << " MAN-flow " << W(7) <<"-" ;
	}
	//boundary
	if(this->boundary_flag==false){
		*cout << W(16) << " NO-bound " << W(10) <<"-" ;
	}
	else if(this->bound_mem->boundary_data.boundary_flag==true && this->bound_mem->boundary_data.stationary_flag==true){
		*cout << W(16) << " STAT " << W(10) << P(5) << FORMAT_FIXED_REAL << this->bound_mem->boundary_data.discharge_value ;
	}
	else if(this->bound_mem->boundary_data.boundary_flag==true && this->bound_mem->boundary_data.stationary_flag==false){
		*cout << W(16) << " INSTAT " << W(10) << this->bound_mem->boundary_data.curve_number;
	}
	//material values
	*cout << W(10) << this->flow_data.mat_type << W(12) << P(5) <<FORMAT_FIXED_REAL << this->flow_data.n_value;
	//init and connection cell
	*cout << W(12) << P(2) << FORMAT_FIXED_REAL<< this->flow_data.init_condition<< W(10) << "-";


}
//Ouput the result members per internal timestep
void Hyd_Element_Floodplain_Type_Standard::output_result_values(ostringstream *cout){
	if(this->h_value<=constant::dry_hyd_epsilon){
		*cout  << W(12)<<functions::convert_boolean2string(false);
		*cout  << W(15)<< P(4) << FORMAT_FIXED_REAL<< 0.0 ; 
		*cout  << W(15)<< P(4) << FORMAT_FIXED_REAL<< this->s_value ;
		*cout  << W(15)<< P(4) << FORMAT_FIXED_REAL<< this->ds_dt_value;
		*cout  << W(17)<< P(4) << FORMAT_FIXED_REAL<< 0.0;
		*cout  << W(15)<< P(4) << FORMAT_FIXED_REAL<< 0.0;
		*cout  << W(15)<< P(4) << FORMAT_FIXED_REAL<< 0.0;
		*cout<< endl;
	}
	else{
		*cout  << W(12)<<functions::convert_boolean2string(this->wet_flag);
		*cout  << W(15)<< P(4) << FORMAT_FIXED_REAL<< this->h_value ; 
		*cout  << W(15)<< P(4) << FORMAT_FIXED_REAL<< this->s_value ;
		*cout  << W(15)<< P(4) << FORMAT_FIXED_REAL<< this->ds_dt_value;
		*cout  << W(17)<< P(4) << FORMAT_FIXED_REAL<< this->v_x;
		*cout  << W(15)<< P(4) << FORMAT_FIXED_REAL<< this->v_y;
		*cout  << W(15)<< P(4) << FORMAT_FIXED_REAL<< this->v_total;
		*cout<< endl;
	}
}
//Ouput the maximum calculated values
void Hyd_Element_Floodplain_Type_Standard::output_maximum_calculated_values(ostringstream *cout){
	if(this->was_wet_flag==true){
		*cout  << W(12)<<functions::convert_boolean2string(this->was_wet_flag)<< W(12)<< P(0) << FORMAT_FIXED_REAL<< this->wet_duration;
		*cout  << W(15)<< P(4) << FORMAT_FIXED_REAL<< this->max_h_value.maximum << W(12) << this->max_h_value.maximum+(*this->z_value) <<W(12) << P(0) << FORMAT_FIXED_REAL <<this->max_h_value.time_point; 
		*cout  << W(15)<< P(4) << FORMAT_FIXED_REAL<< this->max_ds_dt.maximum*60.0 << W(12)<< P(0) << FORMAT_FIXED_REAL <<this->max_ds_dt.time_point; 
		*cout  << W(15)<< P(4) << FORMAT_FIXED_REAL<< this->max_v_x.maximum << W(12)<< P(0) << FORMAT_FIXED_REAL <<this->max_v_x.time_point;
		*cout  << W(15)<< P(4) << FORMAT_FIXED_REAL<< this->max_v_y.maximum << W(12)<< P(0) << FORMAT_FIXED_REAL <<this->max_v_y.time_point;
		*cout  << W(15)<< P(4) << FORMAT_FIXED_REAL<< this->max_v_total.maximum << W(12)<< P(0) << FORMAT_FIXED_REAL <<this->max_v_total.time_point;
		*cout  << W(15)<< P(4) << FORMAT_FIXED_REAL<< this->max_hv.maximum << W(12)<< P(0) << FORMAT_FIXED_REAL <<this->max_hv.time_point;
		*cout  << W(12)<< P(0) << FORMAT_FIXED_REAL <<this->first_arrival_time;
		*cout << P(1) << FORMAT_SCIENTIFIC_REAL;
		if(this->coupling_flag==true){
			*cout <<W(20) << this->coup_mem->coupling_volume_fp.volume_in;
			*cout <<W(20) << this->coup_mem->coupling_volume_fp.volume_out;
			*cout <<W(20) << this->coup_mem->coupling_volume_structure_rv.volume_in;
			*cout <<W(20) << this->coup_mem->coupling_volume_structure_rv.volume_out;
			*cout <<W(20) << this->coup_mem->coupling_volume_direct_rv.volume_in;
			*cout <<W(20) << this->coup_mem->coupling_volume_direct_rv.volume_out;
			*cout <<W(20) << this->coup_mem->coupling_volume_overflow_rv.volume_in;
			*cout <<W(20) << this->coup_mem->coupling_volume_overflow_rv.volume_out;
			*cout <<W(22) << this->coup_mem->coupling_volume_dikebreak_rv.volume_in;
			*cout <<W(22) << this->coup_mem->coupling_volume_dikebreak_rv.volume_out;
			*cout <<W(22) << this->coup_mem->coupling_volume_overflow_co.volume_in;
			*cout <<W(20) << this->coup_mem->coupling_volume_overflow_co.volume_out;
			*cout <<W(20) << this->coup_mem->coupling_volume_dikebreak_co.volume_in;
			*cout <<W(20) << this->coup_mem->coupling_volume_dikebreak_co.volume_out;
		}
		else{
			*cout <<W(20) << 0.0;
			*cout <<W(20) << 0.0;
			*cout <<W(20) << 0.0;
			*cout <<W(20) << 0.0;
			*cout <<W(20) << 0.0;
			*cout <<W(20) << 0.0;
			*cout <<W(20) << 0.0;
			*cout <<W(20) << 0.0;
			*cout <<W(22) << 0.0;
			*cout <<W(22) << 0.0;
			*cout <<W(22) << 0.0;
			*cout <<W(20) << 0.0;
			*cout <<W(20) << 0.0;
			*cout <<W(20) << 0.0;
		}
		*cout<< endl;
	}
	else{
		*cout << "    Not affected " << endl;
	}
}
//Set the data buffers of _hyd_boundarydata_floodplain_elem, _hyd_flowdata_floodplain_elem from class Hyd_Floodplain_Element
void Hyd_Element_Floodplain_Type_Standard::set_data_buffers(const _hyd_boundarydata_floodplain_elem boundary, const _hyd_flowdata_floodplain_elem flow, double *z_value){
	this->set_boundary_condition_data(boundary);

	this->flow_data=flow;
	this->z_value=z_value;
	this->s_value=(*this->z_value);
}
//Set new boundary condition data
void Hyd_Element_Floodplain_Type_Standard::set_boundary_condition_data(const _hyd_boundarydata_floodplain_elem boundary){
	if(boundary.boundary_flag==true){
		this->allocate_bound_data();
		this->bound_mem->boundary_data=boundary;
		if(this->bound_mem->boundary_data.stationary_flag==false){
			this->bound_mem->boundary_data.found_flag=false;
		}
		//check the boundary type
		if(this->bound_mem->boundary_data.boundary_type==_hyd_bound_type::length_type || this->bound_mem->boundary_data.boundary_type==_hyd_bound_type::waterlevel_type){
			Error msg=this->set_error(0);
			ostringstream info;
			info << "Given type: " << Hyd_Instationary_Boundary::transfrom_instatboundtype2txt(this->bound_mem->boundary_data.boundary_type) << endl;
			msg.make_second_info(info.str());
			throw msg;
		}
	}
}
//Set element flag for flow in x-direction
void Hyd_Element_Floodplain_Type_Standard::set_x_noflow_flag(const bool flag){
	this->flow_data.no_flow_x_flag=flag;
}
//Set element flag for flow in y-direction
void Hyd_Element_Floodplain_Type_Standard::set_y_noflow_flag(const bool flag){
	this->flow_data.no_flow_y_flag=flag;
}
//Set the absolute element border height z (m) in x-direction and the Poleni-factor
void Hyd_Element_Floodplain_Type_Standard::set_x_border_z(const double abs_height, const double pol_factor){
	if(this->flow_data.poleni_flag_x==false){
		this->flow_data.height_border_x_abs=abs_height;
		this->flow_data.poleni_x=pol_factor;
		this->flow_data.poleni_flag_x=true;
	}
	else{//take the mid value
		this->flow_data.poleni_x=(pol_factor+this->flow_data.poleni_x)*0.5;
		this->flow_data.height_border_x_abs=(abs_height+this->flow_data.height_border_x_abs)*0.5;
	}

}
//Set the absolute element border height z (m) in y-direction and the Poleni-factor
void Hyd_Element_Floodplain_Type_Standard::set_y_border_z(const double abs_height, const double pol_factor){

	if(this->flow_data.poleni_flag_y==false){
		this->flow_data.height_border_y_abs=abs_height;
		this->flow_data.poleni_y=pol_factor;
		this->flow_data.poleni_flag_y=true;
	}
	else{//take the mid value
		this->flow_data.poleni_y=(pol_factor+this->flow_data.poleni_y)*0.5;
		this->flow_data.height_border_y_abs=(abs_height+this->flow_data.height_border_y_abs)*0.5;
	}
	
}
//Calculate the variation of the global waterlevel (s-value) over time for flow in x-direction with the Manning-formula
void Hyd_Element_Floodplain_Type_Standard::calculate_ds_dt_manning_x(void){
	double flow_depth=0.0;
	double flow_depth_neigh=0.0;
	double delta_h=0.0;
	double ds_dt_buff=0.0;
	if(this->h_value<=constant::flow_epsilon && this->element_x->element_type->get_h_value()<=constant::flow_epsilon){
		return;
	}
	
	//calculate the mid of the flow depth
	flow_depth=this->s_value-this->s_max_x;
	if(flow_depth <0.0){
		flow_depth=0.0;
	}
	flow_depth_neigh=this->element_x->element_type->get_s_value()-this->s_max_x;
	if(flow_depth_neigh<0.0){
		flow_depth_neigh=0.0;
	}
	//flow_depth=flow_depth*0.5+flow_depth_neigh*0.5;
	flow_depth=max(flow_depth,flow_depth_neigh);
	//set a limit to the flow depth
	
	if(flow_depth<=constant::flow_epsilon){
		this->v_x=0.0;
		ds_dt_buff=0.0;
	}
	else{
		delta_h=this->element_x->element_type->get_s_value()-this->s_value;
		double abs_delta_h=abs(delta_h);
		
		if(abs_delta_h<=constant::flow_epsilon){ 
			this->v_x=0.0;
			ds_dt_buff=0.0;
		}
		else{
			ds_dt_buff=this->c_x*pow(flow_depth, (5.0/3.0));
			//replace the manning strickler function by a tangens- function by a given boundary; this functions is the best fit to the square-root
			//functions between 0.001 m and 0.02 m; the boundary is set, where the functions (arctan/square root) are identically)
			if(abs_delta_h<=0.005078){
				ds_dt_buff=ds_dt_buff*0.10449968880528*atan(159.877741951379*delta_h); //0.0152
			}
			//if(abs_delta_h<=0.009541142){
			//	ds_dt_buff=ds_dt_buff*0.10489968880528*atan(140.877741951379*delta_h); //0.0152
			//}
			////if(abs_delta_h<=0.000246925){
			////	ds_dt_buff=ds_dt_buff*0.02304416717797*atan(3287.68191373281*delta_h); //0.0152
			////}
			////if(abs_delta_h<=0.007581357){
			////	ds_dt_buff=ds_dt_buff*0.07370400068147*atan(321.40326964290100*delta_h); //0.0152
			////}
			
			//if(abs_delta_h<=0.01){
			//	ds_dt_buff=ds_dt_buff*(delta_h*1.0/(pow(0.01,0.5))); //0.0152
			//}
			else{
				ds_dt_buff=ds_dt_buff*(delta_h/pow(abs_delta_h,0.5));
			}
			
			//calculate the flow-velocity
			this->v_x=-1.0*ds_dt_buff*(*this->x_width)/flow_depth;
		}
	}
}
//Calculate the variation of the global waterlevel (s-value) over time for flow in y-direction with the Manning-formula
void Hyd_Element_Floodplain_Type_Standard::calculate_ds_dt_manning_y(void){
	double flow_depth=0.0;
	double flow_depth_neigh=0.0;
	double delta_h=0.0;
	double ds_dt_buff=0.0;

	if(this->h_value<=constant::flow_epsilon && this->element_y->element_type->get_h_value()<=constant::flow_epsilon){
		return;
	}

	flow_depth=this->s_value-this->s_max_y;
	if(flow_depth <0.0){
		flow_depth=0.0;
	}
	flow_depth_neigh=this->element_y->element_type->get_s_value()-this->s_max_y;
	if(flow_depth_neigh<0.0){
		flow_depth_neigh=0.0;
	}
	//flow_depth=flow_depth*0.5+flow_depth_neigh*0.5;
	flow_depth=max(flow_depth,flow_depth_neigh);

	//set a limit to the flow depth
	if(flow_depth<=constant::flow_epsilon){
		this->v_y=0.0;
		ds_dt_buff=0.0;
	}
	else{
		delta_h=this->element_y->element_type->get_s_value()-this->s_value;
		double abs_delta_h=abs(delta_h);
		if(abs_delta_h<=constant::flow_epsilon){ 
			ds_dt_buff=0.0;
			this->v_y=0.0;
		}
		else{
			ds_dt_buff=this->c_y*pow(flow_depth, (5.0/3.0));
			//replace the manning strickler function by a tangens- function by a given boundary
			if(abs_delta_h<=0.005078){
				ds_dt_buff=ds_dt_buff*0.10449968880528*atan(159.877741951379*delta_h); //0.0152
			}
			//if(abs_delta_h<=0.009541142){
			//	ds_dt_buff=ds_dt_buff*0.10489968880528*atan(140.877741951379*delta_h); //0.0152
			//}
			//if(abs_delta_h<=0.000246925){
			//	ds_dt_buff=ds_dt_buff*0.02304416717797*atan(3287.68191373281*delta_h); //0.0152
			//}
			//if(abs_delta_h<=0.007581357){
			//	ds_dt_buff=ds_dt_buff*0.07370400068147*atan(321.40326964290100*delta_h); //0.0152
			//}
			

			//if(abs_delta_h<=0.01){
			//	ds_dt_buff=ds_dt_buff*(delta_h*1.0/(pow(0.01,0.5))); //0.0152
			//}
			else{
				ds_dt_buff=ds_dt_buff*(delta_h/pow(abs_delta_h,0.5));
			}

			//calculate the flow-velocity
			this->v_y=-1.0*ds_dt_buff*(*this->y_width)/flow_depth;
		}

	}

}
//Calculate the variation of the global waterlevel (s-value) over time for flow in x-direction with the Poleni-formula
void Hyd_Element_Floodplain_Type_Standard::calculate_ds_dt_poleni_x(void){
	double delta_h_this=0.0;
	double delta_h_xelem=0.0;
	double ds_dt_buff=0.0;

	
	delta_h_this=this->s_value-this->flow_data.height_border_x_abs;
	

	delta_h_xelem=this->element_x->element_type->get_s_value()-this->flow_data.height_border_x_abs;

	//same waterlevels=> no flow
	if(abs(delta_h_this-delta_h_xelem)<=0.0){
		this->v_x=0.0;
		ds_dt_buff=0.0;
	}
	//both waterlevel are under the weirsill=> no flow
	else if(delta_h_this<=0.0 && delta_h_xelem<=0.0){
		this->v_x=0.0;
		ds_dt_buff=0.0;
	}
	//flow out of this element without submerged weirflow reduction into the neihgbouring element
	else if(delta_h_this> 0.0 && delta_h_xelem <= 0.0){
		ds_dt_buff=-1.0*constant::Cfacweir*this->flow_data.poleni_x*pow(delta_h_this,(3.0/2.0));
		//calculate the velocity
		this->v_x=-1.0*ds_dt_buff*(*this->x_width)/delta_h_this;
	}
	//flow out of the neighbouring element without submerged weirflow reduction into this element
	else if(delta_h_this<= 0.0 && delta_h_xelem > 0.0){
		ds_dt_buff=constant::Cfacweir*this->flow_data.poleni_x*pow(delta_h_xelem,(3.0/2.0));
		//calculate the velocity
		this->v_x=-1.0*ds_dt_buff*(*this->x_width)/delta_h_xelem;
	}
	//submerged weirflow with reduction
	else if(delta_h_this > 0.0 && delta_h_xelem > 0.0){
		//flow out of this element into the neihgbouring element
		if(delta_h_this>delta_h_xelem){
			ds_dt_buff=constant::Cfacweir*this->flow_data.poleni_x*pow(delta_h_this,(3.0/2.0));
			//reduction of the discharge (submerged weirflow)
			double reduction_term=(1.0-delta_h_xelem/delta_h_this);
			//replace the ^(1/3) by a fitted arctan-function; at the boundary they have the same values
			if(reduction_term<=0.000463529){
				ds_dt_buff=-1.0*ds_dt_buff*0.057965266895*atan(8984.365582471040*reduction_term);
			}
			else{
				ds_dt_buff=-1.0*ds_dt_buff*pow(reduction_term,(1.0/3.0));
			}
			
			//calculate the velocity
			this->v_x=-1.0*ds_dt_buff*(*this->x_width)/delta_h_this;
		}
		//flow out of the neighbouring element into this element
		else{
			ds_dt_buff=constant::Cfacweir*this->flow_data.poleni_x*pow(delta_h_xelem,(3.0/2.0));
			//reduction of the discharge (submerged weirflow)
			double reduction_term=(1.0-delta_h_this/delta_h_xelem);
			//replace the ^(1/3) by a fitted arctan-function; at the boundary they have the same values
			if(reduction_term<=0.000463529){
				ds_dt_buff=ds_dt_buff*0.057965266895*atan(8984.365582471040*reduction_term);
			}
			else{
				ds_dt_buff=ds_dt_buff*pow(reduction_term,(1.0/3.0));
			}
			
			//calculate the velocity
			this->v_x=-1.0*ds_dt_buff*(*this->x_width)/delta_h_xelem;
		}
	}
}
//Calculate the variation of the global waterlevel (s-value) over time for flow in y-direction with the Poleni-formula
void Hyd_Element_Floodplain_Type_Standard::calculate_ds_dt_poleni_y(void){
	double delta_h_this=0.0;
	double delta_h_yelem=0.0;
	double ds_dt_buff=0.0;

	delta_h_this=this->s_value-this->flow_data.height_border_y_abs;
	delta_h_yelem=this->element_y->element_type->get_s_value()-this->flow_data.height_border_y_abs;

	//same waterlevels=> no flow
	if(abs(delta_h_this-delta_h_yelem)<=0.0){
		this->v_y=0.0;
		ds_dt_buff=0.0;
	}
	//both waterlevel are under the weirsill=> no flow
	else if(delta_h_this<=0.0 && delta_h_yelem<=0.0){
		this->v_y=0.0;
		ds_dt_buff=0.0;
	}
	//flow out of this element without submerged weirflow reduction into the neihgbouring element
	else if(delta_h_this> 0.0 && delta_h_yelem <= 0.0){
		ds_dt_buff=-1.0*constant::Cfacweir*this->flow_data.poleni_y*pow(delta_h_this,(3.0/2.0));
		//calculate the velocity
		this->v_y=-1.0*ds_dt_buff*(*this->y_width)/delta_h_this;
	}
	//flow out of the neighbouring element without submerged weirflow reduction into this element
	else if(delta_h_this<= 0.0 && delta_h_yelem > 0.0){
		ds_dt_buff=constant::Cfacweir*this->flow_data.poleni_y*pow(delta_h_yelem,(3.0/2.0));
		//calculate the velocity
		this->v_y=-1.0*ds_dt_buff*(*this->y_width)/delta_h_yelem;
	}
	//submerged weirflow with reduction
	else if(delta_h_this > 0.0 && delta_h_yelem > 0.0){
		//flow out of this element into the neihgbouring element
		if(delta_h_this>delta_h_yelem){
			ds_dt_buff=constant::Cfacweir*this->flow_data.poleni_y*pow(delta_h_this,(3.0/2.0));
			//reduction of the discharge (submerged weirflow)
			double reduction_term=(1.0-delta_h_yelem/delta_h_this);
			//replace the ^(1/3) by a fitted arctan-function; at the boundary they have the same values
			if(reduction_term<=0.000463529){
				ds_dt_buff=-1.0*ds_dt_buff*0.057965266895*atan(8984.365582471040*reduction_term);
			}
			else{
				ds_dt_buff=-1.0*ds_dt_buff*pow(reduction_term,(1.0/3.0));
			}
			//calculate the velocity
			this->v_y=-1.0*ds_dt_buff*(*this->y_width)/delta_h_this;
		}
		//flow out of the neighbouring element into this element
		else{
			ds_dt_buff=constant::Cfacweir*this->flow_data.poleni_y*pow(delta_h_yelem,(3.0/2.0));
			//reduction of the discharge (submerged weirflow)
			double reduction_term=(1.0-delta_h_this/delta_h_yelem);
			//replace the ^(1/3) by a fitted arctan-function; at the boundary they have the same values
			if(reduction_term<=0.000463529){
				ds_dt_buff=ds_dt_buff*0.057965266895*atan(8984.365582471040*reduction_term);
			}
			else{
				ds_dt_buff=ds_dt_buff*pow(reduction_term,(1.0/3.0));
			}
			//calculate the velocity
			this->v_y=-1.0*ds_dt_buff*(*this->y_width)/delta_h_yelem;
		}
	}
}
//Calculate the maximum values to a given time point
void Hyd_Element_Floodplain_Type_Standard::calculate_maximum_values(const double time_point){
	if(this->h_value<constant::dry_hyd_epsilon){
		return;
	}

	//calculation of the maximum
	if(this->h_value>this->max_h_value.maximum){
		this->max_h_value.maximum=this->h_value;
		this->max_h_value.time_point=time_point;
	}
	if(abs(this->ds_dt_value)>this->max_ds_dt.maximum){
		this->max_ds_dt.maximum=abs(this->ds_dt_value);
		this->max_ds_dt.time_point=time_point;
	}
	if(abs(this->v_x_out)>abs(this->max_v_x.maximum)){
		this->max_v_x.maximum=this->v_x_out;
		this->max_v_x.time_point=time_point;
	}
	if(abs(this->v_y_out)>abs(this->max_v_y.maximum)){
		this->max_v_y.maximum=this->v_y_out;
		this->max_v_y.time_point=time_point;
	}
	//calculate the total velocity
	this->v_total=pow((pow(this->v_x_out,2.0)+pow(this->v_y_out,2.0)),0.5);
	//maximum of total velocity
	if(abs(this->v_total)>this->max_v_total.maximum){
		this->max_v_total.maximum=abs(this->v_total);
		this->max_v_total.time_point=time_point;
	}
	if(abs(this->v_total*this->h_value)>this->max_hv.maximum){
		this->max_hv.maximum=abs(this->v_total*this->h_value);
		this->max_hv.time_point=time_point;
	}
}
//Calculate if the element gets wet or dry and the duration of being wet
void Hyd_Element_Floodplain_Type_Standard::calculate_wet_dry_duration(const double time_point, const double wet_boundary){

	//set was_wet_flag
	if(this->was_wet_flag==false && this->h_value>wet_boundary){
		this->was_wet_flag=true;
	}
	if(this->wet_flag==false && this->h_value>wet_boundary){
		this->wet_flag=true;
		this->time_wet_start=time_point;
		if(this->first_arrival_time<0.0){
			this->first_arrival_time=time_point;
		}
	}
	else if(this->wet_flag==true && this->h_value>wet_boundary){
		this->wet_duration=this->wet_duration+(time_point-this->time_wet_start);
		this->time_wet_start=time_point;
	}
	//set the duration
	if(this->wet_flag==true && this->h_value<wet_boundary){
		this->wet_flag=false;
		this->wet_duration=this->wet_duration+(time_point-this->time_wet_start);
	}
}
//Calculate the watervolume in the element
void Hyd_Element_Floodplain_Type_Standard::calculate_watervolume_element(const bool infiltration){
	this->watervolume=this->h_value*(*this->area);

	if(infiltration==true){
		//add time to the wet duration
		this->wet_duration=this->wet_duration+this->h_value/this->infiltration_rate;
	}
}
//Check if there are unphysical waterlevel calculated by the solver
void Hyd_Element_Floodplain_Type_Standard::check_unphysical_waterlevels(void){
	if(abs(this->h_value)>19882.0){
		Error msg=this->set_error(1);
		throw msg;
	}
	if(functions::check_infinite(this->h_value)==true){
		Error msg=this->set_error(2);
		throw msg;
	}
}
//Allocate the data of coupling
void Hyd_Element_Floodplain_Type_Standard::allocate_coup_data(void){
	this->delete_coup_data();
	try{
		this->coup_mem=new Hyd_Element_Coup_Members;
	}
	catch(bad_alloc&t){
		Error msg=this->set_error(3);
		ostringstream info;
		info<< "Info bad alloc: " << t.what() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
	this->coupling_flag=true;
}
//Delete the data of coupling
void Hyd_Element_Floodplain_Type_Standard::delete_coup_data(void){
	if(this->coup_mem!=NULL){
		delete this->coup_mem;
		this->coup_mem=NULL;
		this->coupling_flag=false;
	}
}
//Allocate the data of boundary condition
void Hyd_Element_Floodplain_Type_Standard::allocate_bound_data(void){
	this->delete_bound_data();
	try{
		this->bound_mem=new Hyd_Element_Bound_Members;
	}
	catch(bad_alloc&t){
		Error msg=this->set_error(4);
		ostringstream info;
		info<< "Info bad alloc: " << t.what() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
	this->boundary_flag=true;
	this->bound_mem->boundary_data.boundary_flag=true;
}
//set the error
Error Hyd_Element_Floodplain_Type_Standard::set_error(const int err_type){
		string place="Hyd_Element_Floodplain_Type_Standard::";
		string help;
		string reason;
		int type=0;
		bool fatal=false;
		stringstream info;
		Error msg;

	switch (err_type){
		case 0://not suitable boundary type
			place.append("set_data_buffers(const _hyd_boundarydata_floodplain_elem boundary, const _hyd_flowdata_floodplain_elem flow, const double z_value)");
			reason="There is not a suitable boundary type given for this element";
			help="Check the given boundary type; possible boundary type are: area and point";
			type=18;
			break;
		case 1://emergency stop of the solver
			place.append("check_unphysical_waterlevels(void)");
			reason="The waterlevel has no physical sense; the absolute value are > 19882.0 m (Marian Gap + Mont Everest)";
			help="Check the model and solver settings";
			type=17;
			break;
		case 2://the waterlevels are not defined
			place.append("check_unphysical_waterlevels(void)");
			reason="The waterlevel is not defined";
			help="Check the model and solver settings";
			type=17;
			break;
		case 3://bad alloc
			place.append("allocate_coup_data(void)");
			reason="Can not allocate the memory";
			help="Check the memory";
			type=10;
			break;
		case 4://bad alloc
			place.append("allocate_bound_data(void)");
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
