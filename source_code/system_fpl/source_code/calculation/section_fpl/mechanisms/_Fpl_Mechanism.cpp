//#include "_Fpl_Mechanism.h"
#include "Fpl_Headers_Precompiled.h"

//Default constructor
_Fpl_Mechanism::_Fpl_Mechanism(void){
	this->calculate_flag=false;
	this->in_tree_flag=false;
	this->counter_single_prob=0;
	this->counter_tree_prob=0;
	this->wave2zero=false;
}
//Default destructor
_Fpl_Mechanism::~_Fpl_Mechanism(void){
}
//_________
//public
//Set the flag, if the mechanism should be calculated
void _Fpl_Mechanism::set_2calc_flag(const bool flag){
	this->calculate_flag=flag;
}
//Get the flag, if the mechanism should be calculated
bool _Fpl_Mechanism::get_2calc_flag(void){
	return this->calculate_flag;
}
//Set the flag, if the mechanism should be used in the failure tree
void _Fpl_Mechanism::set_in_tree_flag(const bool flag){
	this->in_tree_flag=flag;
}
//Get the flag, if the mechanism should be used in the failure tree
bool _Fpl_Mechanism::get_in_tree_flag(void){
	return this->in_tree_flag;
}
//Sum up the counter of the single occurence probability by 1
void _Fpl_Mechanism::sum_up_counter_single_prob(void){
	this->counter_single_prob=this->counter_single_prob+1;
}
//Sum up the counter of the fault-tree occurence probability by 1
void _Fpl_Mechanism::sum_up_counter_tree_prob(void){
	this->counter_tree_prob=this->counter_tree_prob+1;
}
//Get the counter of the single occurence probability
double _Fpl_Mechanism::get_counter_single_prob(void){
	return (double)this->counter_single_prob;
}
//Get the counter of the fault-tree occurence probability
double _Fpl_Mechanism::get_counter_tree_prob(void){
	return (double)this->counter_tree_prob;
}
//Reset the counter of the single occurence probability
void _Fpl_Mechanism::reset_counter_single_prob(void){
	this->counter_single_prob=0;
	
}
//Reset the counter of the fault-tree occurence probability
void _Fpl_Mechanism::reset_counter_tree_prob(void){
	this->counter_tree_prob=0;
}
//Reset the counter of the fault-tree occurence probability and of the single occurence probability
void _Fpl_Mechanism::reset_counter_prob(void){
	this->counter_single_prob=0;
	this->counter_tree_prob=0;
}
//_______________
//protected
//Calculate the angle of wave attack, from which the wave height is reduced
void _Fpl_Mechanism::calculate_wavedirection_reduction(const double wind_direction, const double section_direction, double *wave_height, double *wave_period){
	//accoording to TAW (Wave-Runup and wave overtopping at dikes 2002; Eurotop-Manual 2007)
	//calculate the angle of wave attack
	//0° is orthogonal to the section!
	
	this->wave2zero=false;
	double local_wave_direction=abs(wind_direction-section_direction);
	if(abs(local_wave_direction) <=80.0){
		return;
	}
	else if(abs(local_wave_direction)>80.0 && abs(local_wave_direction)<=110.0){
		*wave_height=*wave_height*(110.0-abs(local_wave_direction))/30.0;
		*wave_period=*wave_period*pow(((110.0-abs(local_wave_direction))/30.0),0.5);
	}
	else{//wave height is zero
		*wave_height=0.0;
		this->wave2zero=true;
	}
	
}