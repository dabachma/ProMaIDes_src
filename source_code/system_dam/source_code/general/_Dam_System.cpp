#include "Dam_Headers_Precompiled.h"
//#include "_Dam_System.h"

//Default constructor
_Dam_System::_Dam_System(void){
	this->number_error=0;
	this->number_warning_begin=0;
	this->number_warning=0;

	this->number_raster_dia=NULL;
	this->mulit_raster_import_dia=NULL;

}
//Default destructor
_Dam_System::~_Dam_System(void){

	this->delete_multi_raster_path_dia();
	this->delete_number_raster_dia();

}
//__________
//public
//Get the error number
int _Dam_System::get_error_number(void){
	return this->number_error;
}
//__________
//protected
//Set the number of warnings before an action
void _Dam_System::set_start_warnings_number(void){
	this->number_warning_begin=Warning::get_number_dam_warnings();
}
//Set the warning number, which occured during an action
void _Dam_System::set_warning_number(void){
	this->number_warning=Warning::get_number_dam_warnings()-this->number_warning_begin;
}
//Get the warning number, which occurred during the action
int _Dam_System::get_occured_warning(void){
	return this->number_warning;
}
//Allocate the dialog for the number of raster
void _Dam_System::allocate_number_raster_dia(void){
	try{
		this->number_raster_dia=new DamGui_Number_Raster_Dia;
	}
	catch(bad_alloc &t){
		Error msg=this->set_error(0);
		ostringstream info;
		info<< "Info bad alloc: " << t.what() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
}
//Delete the dialog for the number of raster
void _Dam_System::delete_number_raster_dia(void){
	if(this->number_raster_dia!=NULL){
		delete this->number_raster_dia;
		this->number_raster_dia=NULL;
	}
}
//Allocate the dialog of the raster path for a multiple raster import
void _Dam_System::allocate_multi_raster_path_dia(void){
	if(this->number_raster_dia==NULL){
		return;
	}
	try{
		this->mulit_raster_import_dia=new Sys_Multi_Filechooser_Dia[this->number_raster_dia->get_number_raster()];
	}
	catch(bad_alloc &t){
		Error msg=this->set_error(1);
		ostringstream info;
		info<< "Info bad alloc: " << t.what() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}

}
//Delete the dialog of the raster path for a multiple raster import
void _Dam_System::delete_multi_raster_path_dia(void){
	if(this->mulit_raster_import_dia!=NULL){
		delete []this->mulit_raster_import_dia;
		this->mulit_raster_import_dia=NULL;
	}
}
//Allocate and sort the floodplian models after the size of the elements (from big to small elements)
void _Dam_System::sort_fp_models_elem_size(int **rank, Hyd_Model_Floodplain *fp_models, const int number_fp){
	if(*rank!=NULL){
		delete [](*rank);
	}
	try{
		(*rank)=new int[number_fp];
	}
	catch(bad_alloc &t){
		Error msg=this->set_error(2);
		ostringstream info;
		info<< "Info bad alloc: " << t.what() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
	//set them

	for(int i=0; i< number_fp; i++){
		(*rank)[i]=i;
	}

	//sort
	int swap;
	int j=0;
	double area1=0.0;
	double area2=0.0;
	
	for (int i = 1; i < number_fp; i++) {
		swap=(*rank)[i];
		//the higher value goes first => ">" ??
		//the smaller value goes first => "<" 
		j = i-1;
		area1=fp_models[swap].Param_FP.get_geometrical_info().width_x*fp_models[swap].Param_FP.get_geometrical_info().width_y;
		area2=fp_models[j].Param_FP.get_geometrical_info().width_x*fp_models[j].Param_FP.get_geometrical_info().width_y;

		while ( j >= 0 && area1 > area2){
			(*rank)[j+1]= (*rank)[j];
			j--;
		}
		(*rank)[j+1] = swap;
	}



}
//____________
//private
//Set error(s)
Error _Dam_System::set_error(const int err_type){
		string place="_Dam_System::";
		string help;
		string reason;
		int type=0;
		bool fatal=false;
		stringstream info;
		Error msg;

	switch (err_type){
		case 0://bad alloc
			place.append("allocate_number_raster_dia(QWidget *parent)");
			reason="Can not allocate the memory";
			help="Check the memory";
			type=10;
			break;
		case 1://bad alloc
			place.append("allocate_multi_raster_path_dia(QWidget *parent)");
			reason="Can not allocate the memory";
			help="Check the memory";
			type=10;
			break;
		case 2://bad alloc
			place.append("sort_fp_models_elem_size(double **rank, Hyd_Model_Floodplain *fp_models, const int number_fp)");
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