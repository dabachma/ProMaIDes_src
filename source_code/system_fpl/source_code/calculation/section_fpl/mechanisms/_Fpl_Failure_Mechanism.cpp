//#include "_Fpl_Failure_Mechanism.h"
#include "Fpl_Headers_Precompiled.h"

//Default constructor
_Fpl_Failure_Mechanism::_Fpl_Failure_Mechanism(void){
	this->stress=0.0;
	this->resistance=0.0;
	this->reliability=0.0;
	this->failure_flag=false;
	this->calculate_flag=false;
}
//Default destructor
_Fpl_Failure_Mechanism::~_Fpl_Failure_Mechanism(void){
}
//__________
//public
//Set the flag, if the failure mechanism should be calculated
void _Fpl_Failure_Mechanism::set_2calc_flag(const bool flag){
	this->calculate_flag=flag;
}
//Get the flag, if the failure mechanism should be calculated
bool _Fpl_Failure_Mechanism::get_2calc_flag(void){
	return this->calculate_flag;
}
//____________
//protected
//Calculate the reliability
int _Fpl_Failure_Mechanism::calc_reliability(void){
	//reliability
	this->reliability=this->stress-this->resistance;
	
	if(this->reliability>=0.0){//failure
		this->failure_flag=true;
		return 1;
	}
	else{//no failure
		this->failure_flag=false;
		return 0;
	}
}