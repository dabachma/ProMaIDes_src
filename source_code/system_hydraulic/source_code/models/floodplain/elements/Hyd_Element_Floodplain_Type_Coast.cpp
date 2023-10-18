#include "Hyd_Headers_Precompiled.h"
//#include "Hyd_Element_Floodplain_Type_Coast.h"


//constructor
Hyd_Element_Floodplain_Type_Coast::Hyd_Element_Floodplain_Type_Coast(void){
	this->h_value=0.0;
	this->wet_flag=false;		
	this->max_h_value.maximum=0.0;
	this->max_h_value.time_point=-1.0;	
	this->time_wet_start=0.0;		
	this->wet_duration=0.0;
	this->s_value=0.0;
	this->ptr_sea_waterlevel=NULL;
	this->first_arrival_time=-1.0;
	//count the memory
	Sys_Memory_Count::self()->add_mem(sizeof(Hyd_Element_Floodplain_Type_Coast), _sys_system_modules::HYD_SYS);
}
//destructor
Hyd_Element_Floodplain_Type_Coast::~Hyd_Element_Floodplain_Type_Coast(void){
	//count the memory
	Sys_Memory_Count::self()->minus_mem(sizeof(Hyd_Element_Floodplain_Type_Coast), _sys_system_modules::HYD_SYS);
}
//________
//public
//Initialize the element (here it is not needed)
void Hyd_Element_Floodplain_Type_Coast::init_element(_hyd_neighbouring_elems , double *, double *, double *, Hyd_Param_Material *, const bool ){

}
//Set the pointer to the sea waterlevel
void Hyd_Element_Floodplain_Type_Coast::set_ptr_sea_waterlevel(double *ptr){
	this->ptr_sea_waterlevel=ptr;
}
//Make the syncronistation of the elements to a given time point (
void Hyd_Element_Floodplain_Type_Coast::calculate_boundary_value(const double ){

	if(this->ptr_sea_waterlevel!=NULL){
		this->s_value=*this->ptr_sea_waterlevel;	
	}
	else{
		this->s_value=*this->z_value;	
	}


	this->h_value=this->s_value-(*this->z_value);
	if(this->h_value<0.0){
		this->h_value=0.0;
	}

}
//Calculate the maximum values and the wet duration
void Hyd_Element_Floodplain_Type_Coast::calc_max_values(const double time_point, const double wet_boundary){


	this->calculate_maximum_values(time_point);
	this->calculate_wet_dry_duration(time_point, wet_boundary);
}
//Get the local waterlevel h-value
double Hyd_Element_Floodplain_Type_Coast::get_h_value(void){
	return this->h_value;
}
//Get the global waterlevel s-value
double Hyd_Element_Floodplain_Type_Coast::get_s_value(void){
	return this->s_value;
}
//Get the if the element is wet
double Hyd_Element_Floodplain_Type_Coast::get_wet_flag(void){
	return this->wet_flag;
}
//Get the maximum local waterlevel (h_value)
_hyd_max_values Hyd_Element_Floodplain_Type_Coast::get_max_h_value(void){
	return this->max_h_value;
}
//Get the overall was_wet_flag
bool Hyd_Element_Floodplain_Type_Coast::get_was_wet_flag(void){
	return this->was_wet_flag;
}
//Get the duration of overflooding
double Hyd_Element_Floodplain_Type_Coast::get_wet_duration(void){
	return this->wet_duration;
}
//Set the maximum result values of an element to an query string to transfer them into a database table (Hyd_Element_Floodplain
void Hyd_Element_Floodplain_Type_Coast::set_maximum_value2querystring(ostringstream *query_string){
	//max-values
	*query_string << this->max_h_value.maximum << " , " ;
	*query_string << this->max_h_value.maximum + (*this->z_value)<< " , " ;
	*query_string << 0.0 << " , " ;
	*query_string << 0.0 << " , " ;
	*query_string << 0.0 << " , " ;
	*query_string << 0.0 << " , " ;
	*query_string << 0.0 << " , " ;
	*query_string << this->first_arrival_time << " , " ;
	*query_string << this->wet_duration << " , " ;
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
//Set the instationary result values of an element to an query string to transfer them into a database table (Hyd_Element_Floodplain
void Hyd_Element_Floodplain_Type_Coast::set_instat_value2querystring(ostringstream *query_string) {
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
void Hyd_Element_Floodplain_Type_Coast::reset_hydrobalance_maxvalues(void){
	_Hyd_Element_Floodplain_Type::reset_hydrobalance_maxvalues();
	this->h_value=0.0;
	this->wet_flag=false;		
	this->max_h_value.maximum=0.0;
	this->max_h_value.time_point=-1.0;	
	this->time_wet_start=0.0;		
	this->wet_duration=0.0;
	this->first_arrival_time=-1.0;
}
//__________
//private
//Calculate the maximum values to a given time point
void Hyd_Element_Floodplain_Type_Coast::calculate_maximum_values(const double time_point){
	
	//calculation of the maximum
	if(this->h_value>this->max_h_value.maximum){
		this->max_h_value.maximum=this->h_value;
		this->max_h_value.time_point=time_point;
	}
}
//Calculate if the element gets wet or dry and the duration of being wet
void Hyd_Element_Floodplain_Type_Coast::calculate_wet_dry_duration(const double time_point, const double wet_boundary){

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
//Output the setted members
void Hyd_Element_Floodplain_Type_Coast::output_setted_members(ostringstream *cout){
	*cout << W(14) << P(2) << FORMAT_FIXED_REAL << (*this->z_value);
	//x-direction
	*cout << W(14) << "NO-flow" << W(7) << "-" ;
	//y-direction
	*cout << W(18) << "NO-flow" << W(7) << "-" ;
	//boundary
	*cout << W(16) << "NO-bound" << W(7) <<"-" ;
	//material values
	*cout << W(10) << -1 << W(12) << "-";
	//init and connection cell
	*cout << W(12) << "-" << W(10) << "-";

}
//Ouput the result members per internal timestep
void Hyd_Element_Floodplain_Type_Coast::output_result_values(ostringstream *cout){
	*cout  << W(12)<<functions::convert_boolean2string(this->wet_flag);
	*cout  << W(15)<< P(4) << FORMAT_FIXED_REAL<< this->h_value ; 
	*cout  << W(15)<< P(4) << FORMAT_FIXED_REAL<< this->s_value ;
	*cout  << W(15)<< P(4) << FORMAT_FIXED_REAL<< 0.0;
	*cout  << W(17)<< P(4) << FORMAT_FIXED_REAL<< 0.0;
	*cout  << W(15)<< P(4) << FORMAT_FIXED_REAL<< 0.0;
	*cout  << W(15)<< P(4) << FORMAT_FIXED_REAL<< 0.0;
	*cout<< endl;
}
//Ouput the maximum calculated values
void Hyd_Element_Floodplain_Type_Coast::output_maximum_calculated_values(ostringstream *cout){
	if(this->was_wet_flag==true){
		*cout  << W(12)<<functions::convert_boolean2string(this->was_wet_flag)<< W(12)<< P(0) << FORMAT_FIXED_REAL<< this->wet_duration;
		*cout  << W(15)<< P(4) << FORMAT_FIXED_REAL<< this->max_h_value.maximum << W(12) << this->max_h_value.maximum+(*this->z_value)<< W(12) << P(0) << FORMAT_FIXED_REAL<<this->max_h_value.time_point; 
		*cout  << W(15)<< P(4) << FORMAT_FIXED_REAL<< 0.0 << W(12)<< P(0) << FORMAT_FIXED_REAL <<-1.0; 
		*cout  << W(15)<< P(4) << FORMAT_FIXED_REAL<< 0.0 << W(12) << P(0) << FORMAT_FIXED_REAL<<-1.0;
		*cout  << W(15)<< P(4) << FORMAT_FIXED_REAL<< 0.0 << W(12) << P(0) << FORMAT_FIXED_REAL<<-1.0;
		*cout  << W(15)<< P(4) << FORMAT_FIXED_REAL<< 0.0 << W(12) << P(0) << FORMAT_FIXED_REAL<<-1.0;
		*cout  << W(15)<< P(4) << FORMAT_FIXED_REAL<< 0.0 << W(12) << P(0) << FORMAT_FIXED_REAL<<-1.0;
		*cout  << W(12)<< P(0) << FORMAT_FIXED_REAL <<this->first_arrival_time;
		*cout<< endl;
	}
	else{
		*cout << "    Not affected " << endl;
	}
}