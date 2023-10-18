#include "Hyd_Headers_Precompiled.h"
//#include "Hyd_Element_Bound_Members.h"

//constructor
Hyd_Element_Bound_Members::Hyd_Element_Bound_Members(void){

	this->reset_vol_q();
	this->init_boundarydata_structures(&this->boundary_data);

	//count the memory
	Sys_Memory_Count::self()->add_mem(sizeof(Hyd_Element_Bound_Members), _sys_system_modules::HYD_SYS);
}

//destructor
Hyd_Element_Bound_Members::~Hyd_Element_Bound_Members(void){


	//count the memory
	Sys_Memory_Count::self()->minus_mem(sizeof(Hyd_Element_Bound_Members), _sys_system_modules::HYD_SYS);
}
//_____________
//public
//Initialize the data structures of _hyd_boundarydata_floodplain_elem (static
void Hyd_Element_Bound_Members::init_boundarydata_structures(_hyd_boundarydata_floodplain_elem * data){
	data->boundary_flag=false;
	data->curve_number=-1;
	data->found_flag=true;
	data->stationary_flag=false;
	data->discharge_value=0.0;
	data->ptr_boundarycurve=NULL;
	data->boundary_type=_hyd_bound_type::point_type;
}
//Reset volumes and discharge
void Hyd_Element_Bound_Members::reset_vol_q(void){
	this->q_boundary=0.0;
	this->boundary_volume.volume_in=0.0;
	this->boundary_volume.volume_out=0.0;
	this->boundary_volume.volume_total=0.0;
}
//Calculate boundary value
void Hyd_Element_Bound_Members::calculate_bound_value(const double time, const double area){
	//stationar
	if(this->boundary_data.stationary_flag==true){
		this->q_boundary=this->boundary_data.discharge_value;
		
	}
	//instat
	else if(this->boundary_data.stationary_flag==false){
		this->q_boundary=this->boundary_data.ptr_boundarycurve->calculate_actuel_boundary_value(time);
	}
	//for area boundary type 
	if(this->boundary_data.boundary_type==_hyd_bound_type::area_type){
		this->q_boundary=this->q_boundary*(area);
	}

}
//Calculate hydrological balance
void Hyd_Element_Bound_Members::calculate_hydro_balance(const double delta_t){


	//boundary volume
	if(this->q_boundary>0.0){
		this->boundary_volume.volume_in=this->boundary_volume.volume_in+this->q_boundary*delta_t;
	}
	else if(this->q_boundary<0.0){
		this->boundary_volume.volume_out=this->boundary_volume.volume_out-this->q_boundary*delta_t;
	}
	//total boundary volume
	this->boundary_volume.volume_total=this->boundary_volume.volume_in-this->boundary_volume.volume_out;

}