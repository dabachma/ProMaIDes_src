#include "Risk_Headers_Precompiled.h"
//#include "Risk_Dam_Results.h"

//Default constructor
Risk_Dam_Results::Risk_Dam_Results(void){

	this->number_res=0;

	///Pointer to the damage results
	this->results=NULL;

	Sys_Memory_Count::self()->add_mem(sizeof(Risk_Dam_Results),_sys_system_modules::RISK_SYS);//count the memory

}
//Default destructor
Risk_Dam_Results::~Risk_Dam_Results(void){
	this->delete_results();
	Sys_Memory_Count::self()->minus_mem(sizeof(Risk_Dam_Results),_sys_system_modules::RISK_SYS);//count the memory

}
//_________
//public
//Set and allocate the number of available results
void Risk_Dam_Results::set_number_results(const int number){
	this->delete_results();
	this->number_res=number;
	this->allocate_results();
}
//Set the result of one element
void Risk_Dam_Results::set_elem_result(const int index, const int raster_id, const int elem_id, const double value){
	if(index>=0 && index<this->number_res){
		this->results[index].dam_result=value;
		this->results[index].elem_id=elem_id;
		this->results[index].raster_id=raster_id;
	}
}
//Get the pointer to the result of one element
_risk_dam_elem_result *Risk_Dam_Results::get_ptr_elem_result(const int index){
	if(index<0 || index>=this->number_res){
		return NULL;
	}
	else{
		return &this->results[index];
	}
}
//Compare to result classes
void Risk_Dam_Results::compare_results(Risk_Dam_Results *compare){
	bool found=false;
	int old_index=0;

	//this->output_dam_result();
	//compare->output_dam_result();


	for(int i=0; i< this->number_res; i++){
		found=false;
		for(int j=old_index; j<compare->number_res; j++){
			if(this->results[i].raster_id==compare->results[j].raster_id){
				if(this->results[i].elem_id==compare->results[j].elem_id){
					found=true;
					old_index=j;
					if(this->results[i].dam_result>=compare->results[j].dam_result){
						compare->results[j].dam_result=0.0;
					}
					else{
						this->results[i].dam_result=0.0;
					}
				}
				else if(this->results[i].elem_id<compare->results[j].elem_id){
					break;
				}
			}
			else if(this->results[i].raster_id<compare->results[j].raster_id){
				break;
			}
			if(found==true){
				break;
			}
		}
	}

	//this->output_dam_result();
	//compare->output_dam_result();


}
//Get the sum of all values
double Risk_Dam_Results::get_sum_value(void){
	double sum=0.0;
	for(int i=0; i< this->number_res; i++){
		sum=sum+this->results[i].dam_result;
	
	}
	return sum;
}
//Output the result structure
void Risk_Dam_Results::output_dam_result(void){

	ostringstream cout;

	cout <<this->number_res<<" stored DAM-results"<<endl; 
	cout << "No. "<<" Raster-Id "<<" Elem-Id " << " Value "<<endl;
	Sys_Common_Output::output_risk->output_txt(&cout);
	for(int i=0; i<this->number_res; i++){
		cout << i<<" "<<W(5)<< this->results[i].raster_id << " " <<W(5)<<this->results[i].elem_id<<" " << W(5)<<this->results[i].dam_result<<endl;
		Sys_Common_Output::output_risk->output_txt(&cout);
	}
}
//________
//private
//Allocate the damage result structure
void Risk_Dam_Results::allocate_results(void){
	try{
		this->results=new _risk_dam_elem_result[this->number_res];
		Sys_Memory_Count::self()->add_mem(sizeof(_risk_dam_elem_result)*this->number_res,_sys_system_modules::RISK_SYS);//count the memory

	}
	catch(bad_alloc &t){
		Error msg=this->set_error(0);
		ostringstream info;
		info<< "Info bad alloc: " << t.what() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}

	for(int i=0; i< this->number_res; i++){
		this->results[i].dam_result=0.0;
		this->results[i].elem_id=-1;
		this->results[i].raster_id=-1;
	}
}
//Delete the damage result structure
void Risk_Dam_Results::delete_results(void){
	if(this->results!=NULL){
		delete []this->results;
		this->results=NULL;
		Sys_Memory_Count::self()->minus_mem(sizeof(_risk_dam_elem_result)*this->number_res,_sys_system_modules::RISK_SYS);//count the memory
	}
}
//Set error(s)
Error Risk_Dam_Results::set_error(const int err_type){
	string place="Risk_Dam_Results::";
	string help;
	string reason;
	int type=0;
	bool fatal=false;
	stringstream info;
	Error msg;

	switch (err_type){
		case 0://bad alloc
			place.append("allocate_results(void)");
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