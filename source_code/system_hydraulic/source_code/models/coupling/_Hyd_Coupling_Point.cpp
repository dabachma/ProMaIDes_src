
#include "Hyd_Headers_Precompiled.h"
//#include "_Hyd_Coupling_Point.h"

//constructor
_Hyd_Coupling_Point::_Hyd_Coupling_Point(void): discharge_boundary(1e-10){
	this->point_index=-1;
	this->distance_up=0.0;
	this->distance_down=0.0;
	this->distance_along_polysegment=0.0;
	this->coupling_flag=false;
	this->current_q=0.0;
	this->coupling_v=0.0;
	this->max_coupling_v.maximum=0.0;
	this->max_coupling_v.time_point=0.0;
	this->old_q=0.0;
	this->store_old_q=0.0;
	this->coupling_volume.volume_in=0.0;
	this->coupling_volume.volume_out=0.0;
	this->coupling_volume.volume_total=0.0;
	this->delta_t=0.0;

	this->predicted_h_one=0.0;
	this->gradient_list_h_one.clear();
	for(int i=0; i< constant::no_stored_grad; i++){
		this->gradient_list_h_one.append(0.0);
	}
	this->gradient_h_one=0.0;
	this->calc_h_one=0.0;
	this->old_calc_h_one=0.0;
	this->time_check=false;

	this->min_area=0.0;


}
//Copy constructor
_Hyd_Coupling_Point::_Hyd_Coupling_Point(const _Hyd_Coupling_Point& object):discharge_boundary(1e-10){
	Geo_Point::operator =(object);
	this->point_index=object.point_index;
	this->distance_up=object.distance_up;
	this->distance_down=object.distance_down;
	this->distance_along_polysegment=object.distance_along_polysegment;
	this->coupling_flag=object.coupling_flag;
	this->current_q=object.current_q;
	this->coupling_v=object.coupling_v;
	this->max_coupling_v=object.max_coupling_v;
	this->old_q=object.old_q;
	this->store_old_q=object.store_old_q;
	this->coupling_volume=object.coupling_volume;
	this->delta_t=object.delta_t;
	this->predicted_h_one=object.predicted_h_one;
	this->gradient_h_one=object.gradient_h_one;
	this->calc_h_one=object.calc_h_one;
	this->old_calc_h_one=object.old_calc_h_one;
	this->gradient_list_h_one.clear();
	this->time_check=object.time_check;
	for(int i=0; i< constant::no_stored_grad; i++){
		this->gradient_list_h_one.append(0.0);
	}
	this->min_area=object.min_area;
}
//destructor
_Hyd_Coupling_Point::~_Hyd_Coupling_Point(void){
}
//_______________
//public
//Set the point index
void _Hyd_Coupling_Point::set_point_index(const int index){
	this->point_index=index;
}
//Get the point index
int _Hyd_Coupling_Point::get_point_index(void){
	return this->point_index;
}
//Set distance to the upward point with a given point
void _Hyd_Coupling_Point::set_distance_up(_Hyd_Coupling_Point *up_point){
	this->distance_up=this->distance(up_point);
}
//Set distance to the upward point with a given distance
void _Hyd_Coupling_Point::set_distance_up(const double distance){
	this->distance_up=distance;
}
//Get distance to the upward point
double _Hyd_Coupling_Point::get_distance_up(void){
	return this->distance_up;
}
//Set distance to the downward point with a given point
void _Hyd_Coupling_Point::set_distance_down(_Hyd_Coupling_Point *down_point){
	this->distance_down=this->distance(down_point);
}
//Set distance to the downward point with a given distance
void _Hyd_Coupling_Point::set_distance_down(const double distance){
	this->distance_down=distance;
}
//Get distance to the downward point
double _Hyd_Coupling_Point::get_distance_down(void){
	return this->distance_down;
}
//Set total distance along the defining polysegment
void _Hyd_Coupling_Point::set_total_distance_along_polysegment(const double total_distance){
	this->distance_along_polysegment=total_distance;
}

//Get total distance along the defining polysegment
double _Hyd_Coupling_Point::get_total_distance_along_polysegment(void){
	return this->distance_along_polysegment;
}
//Get if a coupling is applied
bool _Hyd_Coupling_Point::get_coupling_flag(void){
	return this->coupling_flag;
}
//Set the coupling flag
void _Hyd_Coupling_Point::set_coupling_flag(const bool flag){
	this->coupling_flag=flag;
}
//Reset the coupling discharge of the points 
void _Hyd_Coupling_Point::reset_coupling_discharge(void){
	this->current_q=0.0;
	
}
//Reset the hydrological balance values and the maximum calculated values
void _Hyd_Coupling_Point::reset_hydro_balance_max_values(void){
	this->delta_t=0.0;
	this->coupling_volume.volume_in=0.0;
	this->coupling_volume.volume_out=0.0;
	this->coupling_volume.volume_total=0.0;

	this->max_coupling_v.maximum=0.0;
	this->max_coupling_v.time_point=0.0;

}
//Reset the smoothing calculation members
void _Hyd_Coupling_Point::reset_smoothing(void){
	this->old_q=0.0;
	this->store_old_q=0.0;
	this->current_q=0.0;


	this->predicted_h_one=0.0;
	this->gradient_list_h_one.clear();
	for(int i=0; i< constant::no_stored_grad; i++){
		this->gradient_list_h_one.append(0.0);
	}
	this->gradient_h_one=0.0;
	this->calc_h_one=0.0;
	this->old_calc_h_one=0.0;
	this->time_check=false;

}

//Get the predicted waterlvel one (FP1)
double _Hyd_Coupling_Point::get_predicted_h_one(void){
	return this->predicted_h_one;
}
//Copy operator
_Hyd_Coupling_Point& _Hyd_Coupling_Point::operator=(const _Hyd_Coupling_Point& object){
	Geo_Point::operator =(object);
	this->point_index=object.point_index;
	this->distance_up=object.distance_up;
	this->distance_down=object.distance_down;
	this->distance_along_polysegment=object.distance_along_polysegment;
	this->coupling_flag=object.coupling_flag;
	this->current_q=object.current_q;
	this->coupling_v=object.coupling_v;
	this->max_coupling_v=object.max_coupling_v;
	this->old_q=object.old_q;
	this->store_old_q=object.store_old_q;
	this->coupling_volume=object.coupling_volume;
	this->delta_t=object.delta_t;
	this->predicted_h_one=object.predicted_h_one;
	this->gradient_h_one=object.gradient_h_one;
	this->calc_h_one=object.calc_h_one;
	this->old_calc_h_one=object.old_calc_h_one;
	this->time_check=object.time_check;
	this->min_area=object.min_area;

	return *this;
}
//__________________
//protected
//Syncronisation of the coupled models with the couplingspoint
void _Hyd_Coupling_Point::syncronisation_coupled_models(void){
	this->current_q=0.0;
}
//Calculate the maximum values to a given time point
void _Hyd_Coupling_Point::calculate_maximum_values(const double time_point, const double value,_hyd_max_values *max_values){
	
	//calculation of the maximum
	if(abs(value)>max_values->maximum){
		max_values->maximum=abs(value);
		max_values->time_point=time_point;
	}
}
//Calculate the hydrological balance of this coupling
void _Hyd_Coupling_Point::calculate_hydrological_balance(const double q_current, _hyd_hydrological_balance *balance_value){
	
	if(q_current>0.0){
		//outflow of the floodplain
		balance_value->volume_out=balance_value->volume_out+abs(q_current)*this->delta_t;
	}
	else if(q_current<0.0){
		//inflow of the flooplain
		balance_value->volume_in=balance_value->volume_in+abs(q_current)*this->delta_t;
	}
	//total
	balance_value->volume_total=balance_value->volume_in-balance_value->volume_out;

}
//Smooth the coupling discharge with the coupling discharge calculated one internal timestep before
double _Hyd_Coupling_Point::smooth_coupling_discharge(const double q_current, double *old_q){
	double smooth_q=0.0;
	smooth_q=q_current*(1.0-constant::smoothing_coupling_factor)+(*old_q)*constant::smoothing_coupling_factor;
	
	*old_q=smooth_q;
	return smooth_q;
}
//Swap the gradient list
void _Hyd_Coupling_Point::swap_grad_list(QList<double> *list, const double new_grad){

	for(int i=constant::no_stored_grad-1; i>0; i--){
		list->swap(i, i-1);
	}
	list->replace(0, new_grad);
}
//Replace first of the gradient list
void _Hyd_Coupling_Point::replace_first_grad_list(QList<double> *list, const double new_grad){
	list->replace(0, new_grad);
}