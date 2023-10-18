#include "Hyd_Headers_Precompiled.h"
//#include "Hyd_Element_Floodplain_Type_River.h"

//constructor
Hyd_Element_Floodplain_Type_River::Hyd_Element_Floodplain_Type_River(void){
	this->connected_elem=NULL;
	this->connected_elem_dir=_hyd_neighbouring_direction::Y_DIR;
	this->x_width=NULL;
	this->y_width=NULL;
	//result members
	this->s_value=0.0;
	this->h_value=0.0;
	this->wet_flag=false;	
	this->time_wet_start=0.0;
	this->wet_duration=0.0;
	//maximum result variables
	this->max_h_value.maximum=0.0;
	this->max_h_value.time_point=-1.0;

	//count the memory
	Sys_Memory_Count::self()->add_mem(sizeof(Hyd_Element_Floodplain_Type_River), _sys_system_modules::HYD_SYS);
}
//destructor
Hyd_Element_Floodplain_Type_River::~Hyd_Element_Floodplain_Type_River(void){
	//count the memory
	Sys_Memory_Count::self()->minus_mem(sizeof(Hyd_Element_Floodplain_Type_River), _sys_system_modules::HYD_SYS);
}
//________
//public
//Initialize the element (here it is not needed)
void Hyd_Element_Floodplain_Type_River::init_element(_hyd_neighbouring_elems neigh_elem, double *width_x, double *width_y, double *, Hyd_Param_Material *, const bool ){
	
	//set the width
	this->x_width=width_x;
	this->y_width=width_y;
	




	//set boundaries
	if(neigh_elem.x_direction==0){
		this->flow_data.no_flow_x_flag=true;
	}
	else{
		if(neigh_elem.x_direction->get_elem_type()!=_hyd_elem_type::STANDARD_ELEM && neigh_elem.x_direction->get_elem_type()!=_hyd_elem_type::DIKELINE_ELEM){
			this->flow_data.no_flow_x_flag=true;
		}
		else{
			//poleni (set a boundary)
			if(this->flow_data.poleni_flag_x==true){
				//set the absolut weir height
				this->flow_data.height_border_x_abs=max((*this->z_value), this->flow_data.height_border_x_abs);
			}
		}
	}
	if(neigh_elem.y_direction==0){
		this->flow_data.no_flow_y_flag=true;
	}
	else{
		if(neigh_elem.y_direction->get_elem_type()!=_hyd_elem_type::STANDARD_ELEM && neigh_elem.y_direction->get_elem_type()!=_hyd_elem_type::DIKELINE_ELEM){
			this->flow_data.no_flow_y_flag=true;
		}
		else{
			//poleni (set a boundary)
			if(this->flow_data.poleni_flag_y==true){
				//set the absolut weir height
				this->flow_data.height_border_y_abs=max((*this->z_value), this->flow_data.height_border_y_abs);
			}
		}
	}


}
//Set the connected element 
void Hyd_Element_Floodplain_Type_River::set_connected_element(_hyd_neighbouring_elems neigh_elem){
	double height_diff=0.0;
	double height_diff_min=9999999.9;
	//check the most similar element: it have to be a standard_type element and can not have any no_flow condition in direction of this cell; 
	//y-direction
	if(this->find_convenient_neigh_element(neigh_elem.y_direction, _hyd_neighbouring_direction::Y_DIR, &height_diff)){
		if(height_diff<height_diff_min){
			height_diff_min=height_diff;
			this->connected_elem=neigh_elem.y_direction;
			this->connected_elem_dir=_hyd_neighbouring_direction::Y_DIR;
		}
	}
	//x-direction
	if(this->find_convenient_neigh_element(neigh_elem.x_direction, _hyd_neighbouring_direction::X_DIR, &height_diff)){
		if(height_diff<height_diff_min){
			height_diff_min=height_diff;
			this->connected_elem=neigh_elem.x_direction;
			this->connected_elem_dir=_hyd_neighbouring_direction::X_DIR;
		}
	}
	//minus-y-direction
	if(this->find_convenient_neigh_element(neigh_elem.minus_y_direction, _hyd_neighbouring_direction::MINUS_Y_DIR, &height_diff)){
		if(height_diff<height_diff_min){
			height_diff_min=height_diff;
			this->connected_elem=neigh_elem.minus_y_direction;
			this->connected_elem_dir=_hyd_neighbouring_direction::MINUS_Y_DIR;
		}
	}
	//minus-x-direction
	if(this->find_convenient_neigh_element(neigh_elem.minus_x_direction, _hyd_neighbouring_direction::MINUS_X_DIR, &height_diff)){
		if(height_diff<height_diff_min){
			height_diff_min=height_diff;
			this->connected_elem=neigh_elem.minus_x_direction;
			this->connected_elem_dir=_hyd_neighbouring_direction::MINUS_X_DIR;
		}
	}

	//reset all if height difference is to much
	if (height_diff_min > constant::max_height_diff) {
		this->connected_elem = NULL;
		this->connected_elem_dir = _hyd_neighbouring_direction::Y_DIR;
	}
}
//Calculate the maximum values and the wet duration
void Hyd_Element_Floodplain_Type_River::calc_max_values(const double time_point, const double wet_boundary){
	//get the waterlevel from the connected element
	this->set_connected_waterlevel();
	//calculate the maximum values
	this->calculate_maximum_values(time_point);
	//claulate the wet or dry flags
	this->calculate_wet_dry_duration(time_point, wet_boundary);
}
//Get the local waterlevel h-value
double Hyd_Element_Floodplain_Type_River::get_h_value(void){
	return this->h_value;
}
//Get the global waterlevel s-value
double Hyd_Element_Floodplain_Type_River::get_s_value(void){
	return this->s_value;
}
//Get the if the element is wet
double Hyd_Element_Floodplain_Type_River::get_wet_flag(void){
	return this->wet_flag;
}
//Get the maximum local waterlevel (h_value)
_hyd_max_values Hyd_Element_Floodplain_Type_River::get_max_h_value(void){
	return this->max_h_value;
}
//Get the overall was_wet_flag
bool Hyd_Element_Floodplain_Type_River::get_was_wet_flag(void){
	return this->was_wet_flag;
}
//Get the duration of overflooding
double Hyd_Element_Floodplain_Type_River::get_wet_duration(void){
	return this->wet_duration;
}
//Set the maximum result values of an element to an query string to transfer them into a database table (Hyd_Element_Floodplain
void Hyd_Element_Floodplain_Type_River::set_maximum_value2querystring(ostringstream *query_string){
	//max-values
	*query_string << this->max_h_value.maximum << " , " ;
	*query_string << this->max_h_value.maximum + (*this->z_value)<< " , " ;
	*query_string << 0.0 << " , " ;
	*query_string << 0.0 << " , " ;
	*query_string << 0.0 << " , " ;
	*query_string << 0.0 << " , " ;
	*query_string << 0.0 << " , " ;
	*query_string << this->time_wet_start << " , " ;
	*query_string << this->wet_duration << " , " ;
	*query_string << this->h_value*(*this->x_width)*(*this->y_width)  << " , " ;
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
//Set the instationary result values of an element to an query string to transfer them into a database table (Hyd_Element_Floodplain
void Hyd_Element_Floodplain_Type_River::set_instat_value2querystring(ostringstream *query_string) {
	//instat-values
	*query_string << this->get_h_value() << " , ";
	*query_string << this->get_s_value() << " , ";
	*query_string << 0.0 << " , ";
	*query_string << 0.0 << " , ";
	*query_string << 0.0 << " , ";
	*query_string << 0.0 << " , ";
	*query_string << 0.0 << " , ";
}
//Reset the hydrological balance value and the maximum values
void Hyd_Element_Floodplain_Type_River::reset_hydrobalance_maxvalues(void){
	_Hyd_Element_Floodplain_Type::reset_hydrobalance_maxvalues();
	//result members
	this->s_value=0.0;
	this->h_value=0.0;
	this->wet_flag=false;	
	this->time_wet_start=0.0;
	this->wet_duration=0.0;
	//maximum result variables
	this->max_h_value.maximum=0.0;
	this->max_h_value.time_point=-1.0;
}
//__________
//private
//Set the data buffers of _hyd_boundarydata_floodplain_elem, _hyd_flowdata_floodplain_elem from class Hyd_Floodplain_Element
void Hyd_Element_Floodplain_Type_River::set_data_buffers(const _hyd_boundarydata_floodplain_elem , const _hyd_flowdata_floodplain_elem flow, double *z_value){

	this->flow_data=flow;
	this->z_value=z_value;
	this->s_value=(*this->z_value);
}
//Get the flow data structure
_hyd_flowdata_floodplain_elem Hyd_Element_Floodplain_Type_River::get_flow_data(void){
	return this->flow_data;
}
//Output the setted members
void Hyd_Element_Floodplain_Type_River::output_setted_members(ostringstream *cout){
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
	*cout << W(16) << " NO-bound " << W(10) <<"-" ;
	//material values
	*cout << W(10) << -1 << W(12) << "-";
	//init and connection cell
	if(this->connected_elem!=NULL){
		*cout << W(12) << "-" << W(10) << this->connected_elem->get_elem_number();
	}
	else{
		//init and connection cell
		*cout << W(12) << "-" << W(10) << "-";
	}
	
}
//Ouput the result members per internal timestep
void Hyd_Element_Floodplain_Type_River::output_result_values(ostringstream *cout){
	*cout  << W(12)<< functions::convert_boolean2string(this->wet_flag);
	*cout  << W(15)<< P(4) << FORMAT_FIXED_REAL<< this->h_value ; 
	*cout  << W(15)<< P(4) << FORMAT_FIXED_REAL<< this->s_value ;
	*cout  << W(15)<< P(4) << FORMAT_FIXED_REAL<< 0.0;
	*cout  << W(17)<< P(4) << FORMAT_FIXED_REAL<< 0.0;
	*cout  << W(15)<< P(4) << FORMAT_FIXED_REAL<< 0.0;
	*cout  << W(15)<< P(4) << FORMAT_FIXED_REAL<< 0.0;
	*cout<< endl;
}
//Ouput the maximum calculated values
void Hyd_Element_Floodplain_Type_River::output_maximum_calculated_values(ostringstream *cout){
	if(this->was_wet_flag==true){
		*cout  << W(12)<<functions::convert_boolean2string(this->was_wet_flag)<< W(12)<< P(0) << FORMAT_FIXED_REAL<< this->wet_duration;
		*cout  << W(15)<< P(4) << FORMAT_FIXED_REAL<< this->max_h_value.maximum<< W(12) << this->max_h_value.maximum+(*this->z_value) << W(12)<< P(0) << FORMAT_FIXED_REAL <<this->max_h_value.time_point; 
		*cout  << W(15)<< P(4) << FORMAT_FIXED_REAL<< 0.0 << W(12)<< P(0) << FORMAT_FIXED_REAL <<-1.0; 
		*cout  << W(15)<< P(4) << FORMAT_FIXED_REAL<< 0.0 << W(12)<< P(0) << FORMAT_FIXED_REAL <<-1.0;
		*cout  << W(15)<< P(4) << FORMAT_FIXED_REAL<< 0.0 << W(12)<< P(0) << FORMAT_FIXED_REAL <<-1.0;
		*cout  << W(15)<< P(4) << FORMAT_FIXED_REAL<< 0.0 << W(12)<< P(0) << FORMAT_FIXED_REAL <<-1.0;
		*cout  << W(15)<< P(4) << FORMAT_FIXED_REAL<< 0.0 << W(12) << P(0) << FORMAT_FIXED_REAL<<-1.0;
		*cout  << W(12)<< P(0) << FORMAT_FIXED_REAL <<-1.0;
		*cout<< endl;
	}
	else{
		*cout << "    Not affected " << endl;
	}
}
//Set element flag for flow in x-direction
void Hyd_Element_Floodplain_Type_River::set_x_noflow_flag(const bool flag){
	this->flow_data.no_flow_x_flag=flag;
}
//Set element flag for flow in y-direction
void Hyd_Element_Floodplain_Type_River::set_y_noflow_flag(const bool flag){
	this->flow_data.no_flow_y_flag=flag;
}
//Set the absolute element border height z (m) in x-direction and the Poleni-factor
void Hyd_Element_Floodplain_Type_River::set_x_border_z(const double abs_height, const double pol_factor){
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
void Hyd_Element_Floodplain_Type_River::set_y_border_z(const double abs_height, const double pol_factor){
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
//Set the waterlevel from the connected element
void Hyd_Element_Floodplain_Type_River::set_connected_waterlevel(void){

	this->h_value=0.0;
	this->s_value=(*this->z_value);

	if(this->connected_elem!=NULL){
		if(this->connected_elem->element_type->get_h_value()>constant::flow_epsilon){
			//y-direction
			if(this->connected_elem_dir==_hyd_neighbouring_direction::Y_DIR){
				//poleni-boundary 
				if(this->flow_data.poleni_flag_y==true){
					if(this->connected_elem->element_type->get_s_value()>this->flow_data.height_border_y_abs){
						this->s_value=this->connected_elem->element_type->get_s_value();
						this->h_value=this->s_value-(*this->z_value);
					}
				}
				else{
					this->s_value=this->connected_elem->element_type->get_s_value();
					if(this->s_value>(*this->z_value)){
						this->h_value=this->s_value-(*this->z_value);
					}
				}
			}
			//x-direction
			else if(this->connected_elem_dir==_hyd_neighbouring_direction::X_DIR){
				//poleni-boundary
				if(this->flow_data.poleni_flag_x==true){
					if(this->connected_elem->element_type->get_s_value()>this->flow_data.height_border_x_abs){
						this->s_value=this->connected_elem->element_type->get_s_value();
						this->h_value=this->s_value-(*this->z_value);
					}
				}
				else{
					this->s_value=this->connected_elem->element_type->get_s_value();
					if(this->s_value>(*this->z_value)){
						this->h_value=this->s_value-(*this->z_value);
					}
				}
			}
			//minus-y-direction
			else if(this->connected_elem_dir==_hyd_neighbouring_direction::MINUS_Y_DIR){ 
				//poleni-boundary
				if(this->connected_elem->get_flow_data().poleni_flag_y==true){
					if(this->connected_elem->element_type->get_s_value()>this->connected_elem->get_flow_data().height_border_y_abs){
						this->s_value=this->connected_elem->element_type->get_s_value();
						this->h_value=this->s_value-(*this->z_value);
					}
				}
				else{
					this->s_value=this->connected_elem->element_type->get_s_value();
					if(this->s_value>(*this->z_value)){
						this->h_value=this->s_value-(*this->z_value);
					}
				}
			}
			//minus-x-direction
			else if(this->connected_elem_dir==_hyd_neighbouring_direction::MINUS_X_DIR){
				//poleni-boundary
				if(this->connected_elem->get_flow_data().poleni_flag_x==true){
					if(this->connected_elem->element_type->get_s_value()>this->connected_elem->get_flow_data().height_border_x_abs){
						this->s_value=this->connected_elem->element_type->get_s_value();
						this->h_value=this->s_value-(*this->z_value);
					}
				}
				else{
					this->s_value=this->connected_elem->element_type->get_s_value();
					if(this->s_value>(*this->z_value)){
						this->h_value=this->s_value-(*this->z_value);
					}
				}
			}
		}
	}
}
//Calculate the maximum values to a given time point
void Hyd_Element_Floodplain_Type_River::calculate_maximum_values(const double time_point){
	
	//calculation of the maximum
	if(this->h_value>this->max_h_value.maximum){
		this->max_h_value.maximum=this->h_value;
		this->max_h_value.time_point=time_point;
	}
}
//Calculate if the element gets wet or dry and the duration of being wet
void Hyd_Element_Floodplain_Type_River::calculate_wet_dry_duration(const double time_point, const double wet_boundary){

	//set was_wet_flag
	if(this->was_wet_flag==false && this->h_value>wet_boundary){
		this->was_wet_flag=true;
	}
	if(this->wet_flag==false && this->h_value>wet_boundary){
		this->wet_flag=true;
		this->time_wet_start=time_point;
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
//Check if the neighbouring element is convenient to connect it to this element
bool Hyd_Element_Floodplain_Type_River::find_convenient_neigh_element(Hyd_Element_Floodplain *neigh_elem, _hyd_neighbouring_direction dir, double *height_diff){
	bool conven_flag=false;
	if(neigh_elem!=NULL){
		if(neigh_elem->get_elem_type()==_hyd_elem_type::STANDARD_ELEM){
			if(dir==_hyd_neighbouring_direction::Y_DIR && this->flow_data.no_flow_y_flag==false){
				if(this->flow_data.poleni_flag_y==true){
					*height_diff=abs(this->flow_data.height_border_y_abs-(*this->z_value));
				}
				else{
					*height_diff=abs((*this->z_value)-neigh_elem->get_z_value());
				}
				conven_flag=true;
			}
			else if(dir==_hyd_neighbouring_direction::X_DIR && this->flow_data.no_flow_x_flag==false){
				if(this->flow_data.poleni_flag_x==true){
					*height_diff=abs(this->flow_data.height_border_x_abs-(*this->z_value));
				}
				else{
					*height_diff=abs((*this->z_value)-neigh_elem->get_z_value());
				}
				conven_flag=true;
			}
			else if(dir==_hyd_neighbouring_direction::MINUS_Y_DIR && neigh_elem->get_flow_data().no_flow_y_flag==false){		
				if(neigh_elem->get_flow_data().poleni_flag_y==true){
                    *height_diff=abs(neigh_elem->get_flow_data().height_border_y_abs+max((*this->z_value), neigh_elem->get_z_value())-(*this->z_value));
				}
				else{
					*height_diff=abs((*this->z_value)-neigh_elem->get_z_value());
				}
				conven_flag=true;
			}
			else if(dir==_hyd_neighbouring_direction::MINUS_X_DIR && neigh_elem->get_flow_data().no_flow_x_flag==false){
				if(neigh_elem->get_flow_data().poleni_flag_x==true){
                    *height_diff=abs(neigh_elem->get_flow_data().height_border_x_abs+max((*this->z_value), neigh_elem->get_z_value())-(*this->z_value));
				}
				else{
					*height_diff=abs((*this->z_value)-neigh_elem->get_z_value());
				}
				conven_flag=true;
			}
		}
	}

	return conven_flag;
}
//Get the element index of the connected element (important for Hyd_Element_Floodplian_Type_River because here the connected element number is returned)
int Hyd_Element_Floodplain_Type_River::get_coupling_relevant_element_number(void){
	if(this->connected_elem==NULL){
		return -1;
	}
	else{
		return this->connected_elem->get_elem_number();
	}
}
