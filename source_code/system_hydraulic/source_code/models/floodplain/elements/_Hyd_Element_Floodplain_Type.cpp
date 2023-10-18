#include "Hyd_Headers_Precompiled.h"
//#include "_Hyd_Element_Floodplain_Type.h"

//constructor
_Hyd_Element_Floodplain_Type::_Hyd_Element_Floodplain_Type(void){
	this->z_value=NULL;
	this->was_wet_flag=false;
}
//destructor
_Hyd_Element_Floodplain_Type::~_Hyd_Element_Floodplain_Type(void){
}
//________________
//public
//Connect the element with a instationary boundary curve if requiered (just needed in child-class Hyd_Element_Floodplain_Type_Standard)
void _Hyd_Element_Floodplain_Type::connect_instat_boundarycurve(Hyd_Instationary_Boundary *bound_curve){
	Hyd_Instationary_Boundary *dummy;
	dummy=bound_curve;
}
//Calculate the hydolocigal balance for boundary condition (every syncronisation step)
void _Hyd_Element_Floodplain_Type::calculate_hydrolocigal_balance_boundary(const double ){

}
//Calculate the hydolocigal balance for coupling (every syncronisation step)
void _Hyd_Element_Floodplain_Type::calculate_hydrolocigal_balance_coupling(const double ){

}

//Calculate the boundary value to a given time point
void _Hyd_Element_Floodplain_Type::calculate_boundary_value(const double ){

}
//Calculate the maximum values and the wet duration
void _Hyd_Element_Floodplain_Type::calc_max_values(const double , const double ){

}
//Set the result value (just needed in child-class Hyd_Element_Floodplain_Type_Standard)
void _Hyd_Element_Floodplain_Type::set_solver_result_value(const double ){

}
//Set a value to the the variation of the global waterlevel over time
void _Hyd_Element_Floodplain_Type::set_ds2dt_value(const double ){

}
//Calculate the variation of the global waterlevel over time(just needed in child-class Hyd_Element_Floodplain_Type_Standard)
void _Hyd_Element_Floodplain_Type::calculate_ds_dt(void){

}
///Calculate the flow velocities over time (just needed in child-class Hyd_Element_Floodplain_Type_Standard)
void _Hyd_Element_Floodplain_Type::calculate_v_out(const double , const double ) {

}
//Distribute the flows (ds_dt) to the neighbouring x- and y-elements (just needed in child-class Hyd_Element_Floodplain_Type_Standard)
//Add a value to the the variation of the global waterlevel over time (just needed in child-class Hyd_Element_Floodplain_Type_Standard)
void _Hyd_Element_Floodplain_Type::add_ds_dt(const double ){

}
//Get the global waterlevel (s_value); here the z_value (here the global geodetic height (z_value) is return, because the cell is always dry) 
double _Hyd_Element_Floodplain_Type::get_s_value(void){
	return *this->z_value;
}
///Get the local waterlevel (h_value)
double _Hyd_Element_Floodplain_Type::get_h_value(void){
	return 0.0;
}
//Get the relevant area of the element
double _Hyd_Element_Floodplain_Type::get_relevant_area(void){
	return 0.0;
}
//Get the ds2dt value due to coupling
double _Hyd_Element_Floodplain_Type::get_coupling_ds2dt(void){
	return 0.0;
}
//Rewind the element
void _Hyd_Element_Floodplain_Type::rewind_element(const double ){


}
//Get the variation of the global waterlevel over time as result value ds_dt_value
double _Hyd_Element_Floodplain_Type::get_ds2dt_value(void){
	return 0.0;
}
//Get the flow velocity in x-direction
double _Hyd_Element_Floodplain_Type::get_flowvelocity_vx(void){
	return 0.0;
}
//Get the flow velocity in y-direction
double _Hyd_Element_Floodplain_Type::get_flowvelocity_vy(void){
	return 0.0;
}
//Get the flow velocity in x-direction for output
double _Hyd_Element_Floodplain_Type::get_flowvelocity_vx_out(void) {
	return 0.0;
}
//Get the flow velocity in y-direction for output
double _Hyd_Element_Floodplain_Type::get_flowvelocity_vy_out(void) {
	return 0.0;
}
//Get the total flow velocity
double _Hyd_Element_Floodplain_Type::get_flowvelocity_vtotal(void){
	return 0.0;
}
//Get the if the element is wet
double _Hyd_Element_Floodplain_Type::get_wet_flag(void){
	return false;
}
//Add the hydrological balance value of the boundary condition discharges to the given pointer
void _Hyd_Element_Floodplain_Type::add_hydro_balance_boundary(_hyd_hydrological_balance *given){
	_hyd_hydrological_balance *buffer;
	buffer=given;
}
//Add the hydrological balance value of the coupling discharges to the elements due to a coupling between two floodplain models
void _Hyd_Element_Floodplain_Type::add_hydro_balance_coupling_fp(_hyd_hydrological_balance *given){
	_hyd_hydrological_balance *buffer;
	buffer=given;
}
//Add the hydrological balance value of the coupling discharges to the elements due to river overflow
void _Hyd_Element_Floodplain_Type::add_hydro_balance_overflow_coupling_rv(_hyd_hydrological_balance *given){
	_hyd_hydrological_balance *buffer;
	buffer=given;
}
//Add the hydrological balance value of the coupling discharges to the elements due to river coupling via a hydraulic structure
void _Hyd_Element_Floodplain_Type::add_hydro_balance_structure_coupling_rv(_hyd_hydrological_balance *given){
	_hyd_hydrological_balance *buffer;
	buffer=given;
}
//Add the hydrological balance value of the coupling discharges to the elements due to river coupling via a dikebreak
void _Hyd_Element_Floodplain_Type::add_hydro_balance_dikebreak_coupling_rv(_hyd_hydrological_balance *given){
	_hyd_hydrological_balance *buffer;
	buffer=given;
}
//Add the hydrological balance value of the coupling discharges to the elements due to costal overflow
void _Hyd_Element_Floodplain_Type::add_hydro_balance_overflow_coupling_co(_hyd_hydrological_balance *given){
	_hyd_hydrological_balance *buffer;
	buffer=given;
}
//Add the hydrological balance value of the coupling discharges to the elements due to coast-model coupling via a dikebreak
void _Hyd_Element_Floodplain_Type::add_hydro_balance_dikebreak_coupling_co(_hyd_hydrological_balance *given){
	_hyd_hydrological_balance *buffer;
	buffer=given;
}
//Add the hydrological balance value of the coupling discharges from a coupled river model directly from the outflow profile to the given pointer
void _Hyd_Element_Floodplain_Type::add_hydro_balance_direct_coupling_rv(_hyd_hydrological_balance *given){
	_hyd_hydrological_balance *buffer;
	buffer=given;
}
//Get the hydrological balance value of the coupling discharges to the element due to a coupling between two floodplain models 
_hyd_hydrological_balance _Hyd_Element_Floodplain_Type::get_hydro_balance_coupling_fp(void){
	_hyd_hydrological_balance buff;
	buff.volume_in=0.0;
	buff.volume_out=0.0;
	buff.volume_total=0.0;
	return buff;
}
//Get the hydrological balance value of the coupling discharges to the element due to river overflow 
_hyd_hydrological_balance _Hyd_Element_Floodplain_Type::get_hydro_balance_overflow_coupling_rv(void){
	_hyd_hydrological_balance buff;
	buff.volume_in=0.0;
	buff.volume_out=0.0;
	buff.volume_total=0.0;
	return buff;
}
//Get the hydrological balance value of the coupling discharges to the element due to river coupling via a hydraulic structure 
_hyd_hydrological_balance _Hyd_Element_Floodplain_Type::get_hydro_balance_structure_coupling_rv(void){
	_hyd_hydrological_balance buff;
	buff.volume_in=0.0;
	buff.volume_out=0.0;
	buff.volume_total=0.0;
	return buff;
}
//Get the hydrological balance value of the coupling discharges to the element due to river coupling via a dikebreak 
_hyd_hydrological_balance _Hyd_Element_Floodplain_Type::get_hydro_balance_dikebreak_coupling_rv(void){
	_hyd_hydrological_balance buff;
	buff.volume_in=0.0;
	buff.volume_out=0.0;
	buff.volume_total=0.0;
	return buff;
}
//Get the hydrological balance value of the coupling discharges to the element due to costal overflow 
_hyd_hydrological_balance _Hyd_Element_Floodplain_Type::get_hydro_balance_overflow_coupling_co(void){
	_hyd_hydrological_balance buff;
	buff.volume_in=0.0;
	buff.volume_out=0.0;
	buff.volume_total=0.0;
	return buff;
}
//Get the hydrological balance value of the coupling discharges to the element due to coast-model coupling via a dikebreak 
 _hyd_hydrological_balance _Hyd_Element_Floodplain_Type::get_hydro_balance_dikebreak_coupling_co(void){
	_hyd_hydrological_balance buff;
	buff.volume_in=0.0;
	buff.volume_out=0.0;
	buff.volume_total=0.0;
	return buff;
}
//Get the hydrological balance value of the coupling discharges from a coupled river model directly from the outflow profile (coupling_q_rv_direct)
_hyd_hydrological_balance _Hyd_Element_Floodplain_Type::get_hydro_balance_direct_coupling_rv(void){
	_hyd_hydrological_balance buff;
	buff.volume_in=0.0;
	buff.volume_out=0.0;
	buff.volume_total=0.0;
	return buff;
}
//Get the total coupling inflow volume (without boundary conditions)
double _Hyd_Element_Floodplain_Type::get_total_coupling_inflow_volume(void){
	double buff=0.0;
	return buff;
}
//Get the total coupling outflow volume (without boundary conditions)
double _Hyd_Element_Floodplain_Type::get_total_coupling_outflow_volume(void){
	double buff=0.0;
	return buff;
}
//Get the watervolume in the element
double _Hyd_Element_Floodplain_Type::get_watervolume_element(){
	return 0.0;
}
//Get the watervolume in of the init-conditions
double _Hyd_Element_Floodplain_Type::get_watervolume_init(void){
	return 0.0;
}
//Get the conveance in x-direction
double _Hyd_Element_Floodplain_Type::get_cx(void){
	return 0.0;
}
//Get the conveance in y-direction
double _Hyd_Element_Floodplain_Type::get_cy(void){
	return 0.0;
}
//Get the maximum of geodetic height in x-direction
double _Hyd_Element_Floodplain_Type::get_smax_x(void){
	return 0.0;
}
//Get the maximum of geodetic height in y-direction
double _Hyd_Element_Floodplain_Type::get_smax_y(void){
	return 0.0;
}
//Get the boundary discharge
double _Hyd_Element_Floodplain_Type::get_bound_discharge(void){
	return 0.0;

}
//Get the coupling discharge
double _Hyd_Element_Floodplain_Type::get_coupling_discharge(void){
	return 0.0;

}
//Set the maximum results values directly
void _Hyd_Element_Floodplain_Type::set_max_results_directly(const double , const double , const double , const double ){
		
}
//Get the maximum local waterlevel (h_value)
_hyd_max_values _Hyd_Element_Floodplain_Type::get_max_h_value(void){
	_hyd_max_values buffer;
	buffer.maximum=0.0;
	buffer.time_point=-1.0;
	return buffer;
}
//Get the maximal variation of the global waterlevel over time as result value (ds_dt_value)
_hyd_max_values _Hyd_Element_Floodplain_Type::get_max_ds2dt(void){
	_hyd_max_values buffer;
	buffer.maximum=0.0;
	buffer.time_point=-1.0;
	return buffer;
}
//Get the maximum flow velocity in x-direction
_hyd_max_values _Hyd_Element_Floodplain_Type::get_max_v_x(void){
	_hyd_max_values buffer;
	buffer.maximum=0.0;
	buffer.time_point=-1.0;
	return buffer;
}
//Get the maximum flow velocity in y-direction
_hyd_max_values _Hyd_Element_Floodplain_Type::get_max_v_y(void){
	_hyd_max_values buffer;
	buffer.maximum=0.0;
	buffer.time_point=-1.0;
	return buffer;
}
//Get the maximum total flow velocity
_hyd_max_values _Hyd_Element_Floodplain_Type::get_max_hv(void){
	_hyd_max_values buffer;
	buffer.maximum=0.0;
	buffer.time_point=-1.0;
	return buffer;
}
//Get the maximum product of folow velocity times waterlevel
_hyd_max_values _Hyd_Element_Floodplain_Type::get_max_v_total(void){
	_hyd_max_values buffer;
	buffer.maximum=0.0;
	buffer.time_point=-1.0;
	return buffer;
}
//Get the overall was_wet_flag
bool _Hyd_Element_Floodplain_Type::get_was_wet_flag(void){
	return false;
}
//Get the arrivial time (time when first wet)
double _Hyd_Element_Floodplain_Type::get_arrival_time(void){
	return -1.0;
}
//Get the duration of overflooding
double _Hyd_Element_Floodplain_Type::get_wet_duration(void){
	return 0.0;
}
//Reset the discharge, which comming from coupled floodplain models (coupling_q_fp)
void _Hyd_Element_Floodplain_Type::reset_coupling_discharge_fp(void){

}
//Add coupling discharge, which comming from coupled floodplain models (coupling_q_fp) during syncronisation 
void _Hyd_Element_Floodplain_Type::add_coupling_discharge_fp(const double discharge){
	double dummy;
	dummy=discharge;
}
//Reset the discharge, which comming from coupled river models due to overflow (coupling_q_rv_overflow)
void _Hyd_Element_Floodplain_Type::reset_coupling_discharge_rv_overflow(void){

}
//Add coupling discharge, which comming from coupled river models due to overflow (coupling_q_rv_overflow) during syncronisation 
void _Hyd_Element_Floodplain_Type::add_coupling_discharge_rv_overflow(const double discharge){
	double dummy;
	dummy=discharge;
}
//Reset the discharge, which comming from coupled river models due to hydraulic structure coupling (coupling_q_rv_structure)
void _Hyd_Element_Floodplain_Type::reset_coupling_discharge_rv_structure(void){

}
//Add coupling discharge, which comming from coupled river models due to hydraulic structure coupling (coupling_q_rv_structure) during syncronisation 
void _Hyd_Element_Floodplain_Type::add_coupling_discharge_rv_structure(const double discharge){
	double dummy;
	dummy=discharge;
}
//Reset the discharge, which comming from coupled river models due to dikebreak coupling (coupling_q_rv_dikebreak)
void _Hyd_Element_Floodplain_Type::reset_coupling_discharge_rv_dikebreak(void){

}
//Add coupling discharge, which comming from coupled river models due to dikebreak coupling (coupling_q_rv_dikebreak) during syncronisation 
void _Hyd_Element_Floodplain_Type::add_coupling_discharge_rv_dikebreak(const double discharge){
	double dummy;
	dummy=discharge;
}
//Reset the discharge, which comming from a coupled coast model due to overflow (coupling_q_co_overflow)
void _Hyd_Element_Floodplain_Type::reset_coupling_discharge_co_overflow(void){

}
//Add coupling discharge, which comming from a coupled coast model due to overflow (coupling_q_co_overflow) during syncronisation 
void _Hyd_Element_Floodplain_Type::add_coupling_discharge_co_overflow(const double discharge){
	double dummy;
	dummy=discharge;
}
//Reset the discharge, which comming from a coupled coast model due to dikebreak coupling (coupling_q_co_dikebreak)
void _Hyd_Element_Floodplain_Type::reset_coupling_discharge_co_dikebreak(void){

}
//Add coupling discharge, which comming from a coupled coast model due to dikebreak coupling (coupling_q_co_dikebreak) during syncronisation 
void _Hyd_Element_Floodplain_Type::add_coupling_discharge_co_dikebreak(const double discharge){
	double dummy;
	dummy=discharge;
}
//Reset the discharge, which comming from a coupled river model directly from the outflow profile (coupling_q_rv_direct)
void _Hyd_Element_Floodplain_Type::reset_coupling_discharge_rv_direct(void){
}
//Add coupling discharge, which comming from a coupled river model directly from the outflow profile (coupling_q_rv_direct)
void _Hyd_Element_Floodplain_Type::add_coupling_discharge_rv_direct(const double discharge){
	double dummy;
	dummy=discharge;
}
//Set the maximum result values of an element to an query string to transfer them into a database table (Hyd_Element_Floodplain
void _Hyd_Element_Floodplain_Type::set_maximum_value2querystring(ostringstream *query_string){
	//max-values
	*query_string << 0.0 << " , " ;
	*query_string << (*this->z_value) << " , " ;
	*query_string << 0.0 << " , " ;
	*query_string << 0.0 << " , " ;
	*query_string << 0.0 << " , " ;
	*query_string << 0.0 << " , " ;
	*query_string << 0.0 << " , " ;
	*query_string << -1.0 << " , " ;
	*query_string << 0.0 << " , " ;
	*query_string << 0.0 << " , " ;
	//volumes
	*query_string << 0.0 << " , " ;
	*query_string << 0.0 << " , " ;

	*query_string << 0.0 << " , " ;
	*query_string << 0.0 << " , " ;
	*query_string << 0.0 << " , " ;
	*query_string << 0.0 << " , " ;

	*query_string << 0.0 << " , " ;
	*query_string << 0.0 << " , " ;
	*query_string << 0.0 << " , " ;
	*query_string << 0.0 << " , " ;

	*query_string << 0.0 << " , " ;
	*query_string << 0.0 << " , " ;
	*query_string << 0.0 << " , " ;
	*query_string << 0.0 << " , " ;

	*query_string << 0.0 << " , " ;
	*query_string << 0.0 << " , " ;

}
///Set the instationary result values of an element to an query string to transfer them into a database table (Hyd_Element_Floodplain)
void _Hyd_Element_Floodplain_Type::set_instat_value2querystring(ostringstream *query_string) {
	//instat-values
	*query_string << 0.0 << " , ";
	*query_string << 0.0 << " , ";
	*query_string << 0.0 << " , ";
	*query_string << 0.0 << " , ";
	*query_string << 0.0 << " , ";
	*query_string << 0.0 << " , ";
	*query_string << 0.0 << " , ";
}
//Get the next _hyd_all_element_direction to the given direction counterclockwise (static)
_hyd_all_element_direction _Hyd_Element_Floodplain_Type::get_next_element_direction_counterclockwise(const _hyd_all_element_direction dir){
	_hyd_all_element_direction next_dir;
	switch(dir){
		case _hyd_all_element_direction::X_DIRECT:
			next_dir=_hyd_all_element_direction::XY_DIRECT;
			break;
		case _hyd_all_element_direction::XY_DIRECT:
			next_dir=_hyd_all_element_direction::Y_DIRECT;
			break;
		case _hyd_all_element_direction::Y_DIRECT:
			next_dir=_hyd_all_element_direction::YMINUS_X_DIRECT;
			break;
		case _hyd_all_element_direction::YMINUS_X_DIRECT:
			next_dir=_hyd_all_element_direction::MINUS_X_DIRECT;
			break;
		case _hyd_all_element_direction::MINUS_X_DIRECT:
			next_dir=_hyd_all_element_direction::MINUS_X_MINUS_Y_DIRECT;
			break;
		case _hyd_all_element_direction::MINUS_X_MINUS_Y_DIRECT:
			next_dir=_hyd_all_element_direction::MINUS_Y_DIRECT;
			break;
		case _hyd_all_element_direction::MINUS_Y_DIRECT:
			next_dir=_hyd_all_element_direction::MINUS_Y_X_DIRECT;
			break;
		case _hyd_all_element_direction::MINUS_Y_X_DIRECT:
			next_dir=_hyd_all_element_direction::X_DIRECT;
			break;
		default:
			break;
	}

	return next_dir;
}
///Get the next _hyd_all_element_direction to the given direction clockwise (static)
_hyd_all_element_direction _Hyd_Element_Floodplain_Type::get_next_element_direction_clockwise(const _hyd_all_element_direction dir){
	_hyd_all_element_direction next_dir;
	switch(dir){
		case _hyd_all_element_direction::X_DIRECT:
			next_dir=_hyd_all_element_direction::MINUS_Y_X_DIRECT;
			break;
		case _hyd_all_element_direction::XY_DIRECT:
			next_dir=_hyd_all_element_direction::X_DIRECT;
			break;
		case _hyd_all_element_direction::Y_DIRECT:
			next_dir=_hyd_all_element_direction::XY_DIRECT;
			break;
		case _hyd_all_element_direction::YMINUS_X_DIRECT:
			next_dir=_hyd_all_element_direction::Y_DIRECT;
			break;
		case _hyd_all_element_direction::MINUS_X_DIRECT:
			next_dir=_hyd_all_element_direction::YMINUS_X_DIRECT;
			break;
		case _hyd_all_element_direction::MINUS_X_MINUS_Y_DIRECT:
			next_dir=_hyd_all_element_direction::MINUS_X_DIRECT;
			break;
		case _hyd_all_element_direction::MINUS_Y_DIRECT:
			next_dir=_hyd_all_element_direction::MINUS_X_MINUS_Y_DIRECT;
			break;
		case _hyd_all_element_direction::MINUS_Y_X_DIRECT:
			next_dir=_hyd_all_element_direction::MINUS_Y_DIRECT;
			break;
		default:
			break;
	}


	return next_dir;
}
//Reset the hydrological balance value and the maximum values
void _Hyd_Element_Floodplain_Type::reset_hydrobalance_maxvalues(void){
	this->was_wet_flag=false;
}
//Set the connected element (just relevant for river element types)
void _Hyd_Element_Floodplain_Type::set_connected_element(_hyd_neighbouring_elems ){

}
//Set the pointer to the sea waterlevel
void _Hyd_Element_Floodplain_Type::set_ptr_sea_waterlevel(double *){
/**This method is just important for the coastal elements.
*/
}
//Get the maximum change in a element 
void _Hyd_Element_Floodplain_Type::get_max_changes_elements(double *max_change_h, const bool ){
	*max_change_h=0.0;
}
///Get a pointer to the flow data
_hyd_flowdata_floodplain_elem _Hyd_Element_Floodplain_Type::get_flow_data(void){
	_hyd_flowdata_floodplain_elem buffer;
	this->init_flowdata_structures(&buffer);
	return buffer;
}
///Get a pointer to the boundary data
_hyd_boundarydata_floodplain_elem _Hyd_Element_Floodplain_Type::get_boundary_data(void){
	_hyd_boundarydata_floodplain_elem  buffer;
	Hyd_Element_Bound_Members::init_boundarydata_structures(&buffer);
	return buffer;
}
//Get the coupling flag
bool _Hyd_Element_Floodplain_Type::get_coupling_flag(void){
	return false;
}
//Get boundary condition flag
bool _Hyd_Element_Floodplain_Type::get_bound_flag(void){
	return false;
}
//Delete the data of boundary condition
void _Hyd_Element_Floodplain_Type::delete_bound_data(void){
}
//Set coupling data
void _Hyd_Element_Floodplain_Type::set_coupling_data(void){

}
//_____________
//protected
//Initialize the data structures of _hyd_flowdata_floodplain_elem
void _Hyd_Element_Floodplain_Type::init_flowdata_structures(_hyd_flowdata_floodplain_elem * data){
	data->height_border_x_abs=0.0;
	data->height_border_y_abs=0.0;
	data->init_condition=0.0;
	data->mat_type=-1;
	data->n_value=0.0;
	data->no_flow_x_flag=false;
	data->no_flow_y_flag=false;
	data->poleni_flag_x=false;
	data->poleni_flag_y=false;
	data->poleni_x=constant::poleni_const;
	data->poleni_y=constant::poleni_const;
}
//Get the element index of the connected element (important for Hyd_Element_Floodplian_Type_River because here the connected element number is returned)
int _Hyd_Element_Floodplain_Type::get_coupling_relevant_element_number(void){
	return -1;
}
//_____________________
//private
//Set the data buffers of _hyd_boundarydata_floodplain_elem, _hyd_flowdata_floodplain_elem and the global geodetic height from class Hyd_Floodplain_Element  
void _Hyd_Element_Floodplain_Type::set_data_buffers(const _hyd_boundarydata_floodplain_elem boundary, const _hyd_flowdata_floodplain_elem , double *z_value){
	this->z_value=z_value;
	this->set_boundary_condition_data(boundary);

}
//Set new boundary condition data
void _Hyd_Element_Floodplain_Type::set_boundary_condition_data(const _hyd_boundarydata_floodplain_elem ){

}
///Set element flag for flow in x-direction (just needed in child-class Hyd_Element_Floodplain_Type_Standard)
void _Hyd_Element_Floodplain_Type::set_x_noflow_flag(const bool ){
	
}
///Set element flag for flow in y-direction (just needed in child-class Hyd_Element_Floodplain_Type_Standard)
void _Hyd_Element_Floodplain_Type::set_y_noflow_flag(const bool ){

}
///Set the relative element border height z (m) in x-direction (just needed in child-class Hyd_Element_Floodplain_Type_Standard)
void _Hyd_Element_Floodplain_Type::set_x_border_z(const double , const double ){
;
}
///Set the relative element border height z (m) in y-direction (just needed in child-class Hyd_Element_Floodplain_Type_Standard)
void _Hyd_Element_Floodplain_Type::set_y_border_z(const double , const double ){

}
//(static) Ouput the header for the maximum calculated values output
void _Hyd_Element_Floodplain_Type::output_header_maximum_calculated_values(ostringstream *cout){

	*cout <<W(10) << "was_wet" ;
	*cout <<W(10) << "t(wet)"<< label::sec;
	*cout <<W(8)  << "h" << label::m ;
	*cout <<W(8)  << "s" << label::m ;
	*cout << W(10)<< "t"<<label::sec;
	*cout <<W(8)  << "ds_dt" << label::m_per_min ;
	*cout << W(6)<< "t"<<label::sec;
	*cout <<W(8)  << "v_x" << label::m_per_sec ;
	*cout << W(6)<< "t"<<label::sec;
	*cout <<W(8)  << "v_y" << label::m_per_sec ;
	*cout << W(6)<< "t"<<label::sec;
	*cout <<W(8)  << "v_tot" << label::m_per_sec ;
	*cout<< W(6)<< "t"<<label::sec;
	*cout <<W(8) << "hv" << label::sqm_per_sec ;
	*cout<< W(6)<< "t"<<label::sec;
	*cout <<W(11) << "ariv.time" << label::sec;
	*cout <<W(13) << "CVol_FP_in" << label::cubicmeter;
	*cout <<W(13) << "CVol_FP_out" << label::cubicmeter;
	*cout <<W(13) << "CVol_RV_Sc_in" << label::cubicmeter;
	*cout <<W(13) << "CVol_RV_Sc_out" << label::cubicmeter;
	*cout <<W(13) << "CVol_RV_Di_in" << label::cubicmeter;
	*cout <<W(13) << "CVol_RV_Di_out" << label::cubicmeter;
	*cout <<W(13) << "CVol_RV_OV_in" << label::cubicmeter;
	*cout <<W(13) << "CVol_RV_OV_out" << label::cubicmeter;
	*cout <<W(13) << "CVol_RV_DB_in" << label::cubicmeter;
	*cout <<W(13) << "CVol_RV_DB_out" << label::cubicmeter;
	*cout <<W(13) << "CVol_CO_OV_in" << label::cubicmeter;
	*cout <<W(13) << "CVol_CO_OV_out" << label::cubicmeter;
	*cout <<W(13) << "CVol_CO_DB_in" << label::cubicmeter;
	*cout <<W(13) << "CVol_CO_DB_out" << label::cubicmeter;

	*cout<< endl;
}
//(Static) Ouput the header for the maximum calculated values output
void _Hyd_Element_Floodplain_Type::output_header_result_values(ostringstream *cout){
	*cout <<W(10) << "is_wet" ;
	*cout <<W(10) << "h" << label::m ;
	*cout <<W(10) << "s" << label::m ;
	*cout <<W(10) << "ds_dt" << label::m_per_sec;
	*cout <<W(10) << "v_x" << label::m_per_sec ;
	*cout <<W(10) << "v_y" << label::m_per_sec ;
	*cout <<W(10) << "v_tot" << label::m_per_sec ;
	*cout<< endl;
}
