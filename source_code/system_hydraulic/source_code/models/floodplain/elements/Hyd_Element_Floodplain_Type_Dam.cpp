#include "Hyd_Headers_Precompiled.h"
//#include "Hyd_Element_Floodplain_Type_Dam.h"


//constructor
Hyd_Element_Floodplain_Type_Dam::Hyd_Element_Floodplain_Type_Dam(void){


	//maximum result variables
	this->max_h_value=0.0;
	this->max_ds_dt=0.0;	
	this->max_v_total=0.0;
	this->max_hv=0.0;
	this->wet_duration=0.0;



	//count the memory
	Sys_Memory_Count::self()->add_mem(sizeof(Hyd_Element_Floodplain_Type_Dam), _sys_system_modules::HYD_SYS);

}
//destructor
Hyd_Element_Floodplain_Type_Dam::~Hyd_Element_Floodplain_Type_Dam(void){

	//count the memory
	Sys_Memory_Count::self()->minus_mem(sizeof(Hyd_Element_Floodplain_Type_Dam), _sys_system_modules::HYD_SYS);

}
//____________
//public
//Initialize the element (here it is not needed)
void Hyd_Element_Floodplain_Type_Dam::init_element(_hyd_neighbouring_elems , double *, double *, double *, Hyd_Param_Material *, const bool ){
	
}
//Set the maximum results values directly
void Hyd_Element_Floodplain_Type_Dam::set_max_results_directly(const double waterlevel, const double max_v, const double duration, const double dh_dt){
	if(waterlevel>0.0){
		this->was_wet_flag=true;
	}
	else{
		return;	
	}
	
	this->max_h_value=waterlevel;
	this->max_ds_dt=dh_dt;	
	this->max_v_total=max_v;
	this->max_hv=waterlevel*max_v;
	this->wet_duration=duration;
}
//Set the maximum result values of an element to an query string to transfer them into a database table (Hyd_Element_Floodplain
void Hyd_Element_Floodplain_Type_Dam::set_maximum_value2querystring(ostringstream *query_string){
	//max-values
	*query_string << this->max_h_value << " , " ;
	*query_string << this->max_h_value + (*this->z_value)<< " , " ;
	*query_string << this->max_ds_dt<< " , " ;
	*query_string << 0.0 << " , " ;
	*query_string << 0.0 << " , " ;
	*query_string << this->max_v_total << " , " ;
	*query_string << this->max_hv << " , " ;
	*query_string << -1.0 << " , " ;
	*query_string << this->wet_duration << " , " ;
	*query_string << 0.0  << " , " ;
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
//Get the overall was_wet_flag
bool Hyd_Element_Floodplain_Type_Dam::get_was_wet_flag(void){
	return this->was_wet_flag;
}
//____________
//private
//Output the setted members
void Hyd_Element_Floodplain_Type_Dam::output_setted_members(ostringstream *cout){
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
//Ouput the maximum calculated values
void Hyd_Element_Floodplain_Type_Dam::output_maximum_calculated_values(ostringstream *cout){
	if(this->was_wet_flag==true){
		*cout  << W(12)<<functions::convert_boolean2string(this->was_wet_flag)<< W(12)<< P(0) << FORMAT_FIXED_REAL<< this->wet_duration;
		*cout  << W(15)<< P(4) << FORMAT_FIXED_REAL<< this->max_h_value << W(12) << this->max_h_value+(*this->z_value)<< W(12) << P(0) << FORMAT_FIXED_REAL <<-1.0; 
		*cout  << W(15)<< P(4) << FORMAT_FIXED_REAL<< this->max_ds_dt*60.0 << W(12)<< P(0) << FORMAT_FIXED_REAL <<-1.0; 
		*cout  << W(15)<< P(4) << FORMAT_FIXED_REAL<< 0.0 << W(12) << P(0) << FORMAT_FIXED_REAL<<-1.0;
		*cout  << W(15)<< P(4) << FORMAT_FIXED_REAL<< 0.0 << W(12) << P(0) << FORMAT_FIXED_REAL<<-1.0;
		*cout  << W(15)<< P(4) << FORMAT_FIXED_REAL<< this->max_v_total << W(12) << P(0) << FORMAT_FIXED_REAL<<-1.0;
		*cout  << W(15)<< P(4) << FORMAT_FIXED_REAL<< this->max_hv << W(12) << P(0) << FORMAT_FIXED_REAL<<-1.0;
		*cout  << W(12)<< P(0) << FORMAT_FIXED_REAL <<-1.0;
		*cout<< endl;
	}
	else{
		*cout << "    Not affected " << endl;
	}
}
//Ouput the result members per internal timestep
void Hyd_Element_Floodplain_Type_Dam::output_result_values(ostringstream *cout){
	*cout  << W(12)<<functions::convert_boolean2string(false);
	*cout  << W(15)<< P(4) << FORMAT_FIXED_REAL<< 0.0 ; 
	*cout  << W(15)<< P(4) << FORMAT_FIXED_REAL<< (*this->z_value) ;
	*cout  << W(15)<< P(4) << FORMAT_FIXED_REAL<< 0.0;
	*cout  << W(17)<< P(4) << FORMAT_FIXED_REAL<< 0.0;
	*cout  << W(15)<< P(4) << FORMAT_FIXED_REAL<< 0.0;
	*cout  << W(15)<< P(4) << FORMAT_FIXED_REAL<< 0.0;
	*cout<< endl;
}