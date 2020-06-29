//#include "Fpl_Fail_Overflow_HiGround.h"
#include "Fpl_Headers_Precompiled.h"

//Default constructor
Fpl_Fail_Overflow_HiGround::Fpl_Fail_Overflow_HiGround(void){
	Sys_Memory_Count::self()->add_mem(sizeof(Fpl_Fail_Overflow_HiGround), _sys_system_modules::FPL_SYS);
}
//Default destructor
Fpl_Fail_Overflow_HiGround::~Fpl_Fail_Overflow_HiGround(void){
	Sys_Memory_Count::self()->minus_mem(sizeof(Fpl_Fail_Overflow_HiGround), _sys_system_modules::FPL_SYS);
}
//_____________
//public
//Calculate the failure mechanism
int Fpl_Fail_Overflow_HiGround::calc_mechanism(const double height, const double waterlevel){

	//stress
	this->stress=waterlevel;
	//resistance
	this->resistance=height;
	//reliability
	return this->calc_reliability();

}
//Output the result to display/console
void Fpl_Fail_Overflow_HiGround::output_reliabilty(void){
	ostringstream cout;
	cout <<"Failure mechanism overtopping (highground)" << endl;
	cout <<" Reliability  : " << this->reliability << endl;
	cout <<" Stress       : " << this->stress<< endl;
	cout <<" Resistance   : " << this->resistance <<endl;
	cout <<" Failure      : " << functions::convert_boolean2string(this->failure_flag)<<endl;
	Sys_Common_Output::output_fpl->output_txt(&cout,false);
	
}
//__________
//privat