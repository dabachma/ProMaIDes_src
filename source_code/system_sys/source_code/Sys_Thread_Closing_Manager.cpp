#include "Sys_Headers_Precompiled.h"
//#include "Sys_Thread_Closing_Manager.h"

//Default constructor
Sys_Thread_Closing_Manager::Sys_Thread_Closing_Manager(void){

	this->ptr_dam_module=NULL;
	this->ptr_hyd_module=NULL;
	this->ptr_fpl_module=NULL;
	this->ptr_madm_module=NULL;
	this->ptr_risk_module=NULL;
	this->ptr_cost_module=NULL;
	this->ptr_alt_module=NULL;
	this->ptr_table_loader=NULL;
	this->ptr_project_copy=NULL;
	this->waiting_loop_flag=false;

	this->close_type=_sys_close_types::app_close;
	//count the memory
	Sys_Memory_Count::self()->add_mem(sizeof(Sys_Thread_Closing_Manager), _sys_system_modules::SYS_SYS);
}
//Default destructor
Sys_Thread_Closing_Manager::~Sys_Thread_Closing_Manager(void){
		//count the memory
	Sys_Memory_Count::self()->minus_mem(sizeof(Sys_Thread_Closing_Manager), _sys_system_modules::SYS_SYS);
}
//___________
//public
//Set the pointer to the dam-module thread
void Sys_Thread_Closing_Manager::set_ptr_dam_thread(QThread *ptr_dam_thread){
	this->ptr_dam_module=ptr_dam_thread;
}
//Set the pointer to the hyd-module thread
void Sys_Thread_Closing_Manager::set_ptr_hyd_thread(QThread *ptr_hyd_thread){
	this->ptr_hyd_module=ptr_hyd_thread;
}
//Set the pointer to the fpl-module thread
void Sys_Thread_Closing_Manager::set_ptr_fpl_thread(QThread *ptr_fpl_thread){
	this->ptr_fpl_module=ptr_fpl_thread;
}
//Set the pointer to the madm-module thread
void Sys_Thread_Closing_Manager::set_ptr_madm_thread(QThread *ptr_madm_thread){
	this->ptr_madm_module=ptr_madm_thread;
}
//Set the pointer to the risk-module thread
void Sys_Thread_Closing_Manager::set_ptr_risk_thread(QThread *ptr_risk_thread){
	this->ptr_risk_module=ptr_risk_thread;
}
//Set the pointer to the alt-module thread
void Sys_Thread_Closing_Manager::set_ptr_alt_thread(QThread *ptr_alt_thread){
	this->ptr_alt_module=ptr_alt_thread;
}
//Set the pointer to the cost-module thread
void Sys_Thread_Closing_Manager::set_ptr_cost_thread(QThread *ptr_cost_thread){
	this->ptr_cost_module=ptr_cost_thread;
}
//Set the pointer to the table loader thread thread
void Sys_Thread_Closing_Manager::set_ptr_table_loader_thread(QThread *ptr_table_loader){
	this->ptr_table_loader=ptr_table_loader;
}
//Set the pointer to the project copy thread thread
void Sys_Thread_Closing_Manager::set_ptr_project_copy_thread(QThread *ptr_project_copy){
	this->ptr_project_copy=ptr_project_copy;
}
//Set the closing type
void Sys_Thread_Closing_Manager::set_closing_type(_sys_close_types close_type){
	this->close_type=close_type;
}
//Set the flag for just using a waitng loop 
void Sys_Thread_Closing_Manager::set_jusing_waiting_loop(const bool flag){
	this->waiting_loop_flag=flag;
}
//Start the thread for managing the closing of the threads
void Sys_Thread_Closing_Manager::run(void){
	if(this->waiting_loop_flag==false){
		while(this->check_thread_running()==true){

		}
		emit send_threads_closed(this->close_type);
	}
	else{
		QTimer timer;
		timer.setSingleShot(true);
		timer.start(500);
	}
}
//__________
//private
//Check if the threads are still running
bool Sys_Thread_Closing_Manager::check_thread_running(void){
	bool flag=false;
	if(this->ptr_dam_module!=NULL && this->ptr_dam_module->isRunning()==true){
		flag=true;
		return flag;
	}
	if(this->ptr_hyd_module!=NULL && this->ptr_hyd_module->isRunning()==true){
		flag=true;
		return flag;
	}
	if(this->ptr_fpl_module!=NULL && this->ptr_fpl_module->isRunning()==true){
		flag=true;
		return flag;
	}
	if(this->ptr_madm_module!=NULL && this->ptr_madm_module->isRunning()==true){
		flag=true;
		return flag;
	}
	if(this->ptr_risk_module!=NULL && this->ptr_risk_module->isRunning()==true){
		flag=true;
		return flag;
	}
	if(this->ptr_alt_module!=NULL && this->ptr_alt_module->isRunning()==true){
		flag=true;
		return flag;
	}
	if(this->ptr_cost_module!=NULL && this->ptr_cost_module->isRunning()==true){
		flag=true;
		return flag;
	}
	if(this->ptr_table_loader!=NULL && this->ptr_table_loader->isRunning()==true){
		flag=true;
		return flag;
	}
	if(this->ptr_project_copy!=NULL && this->ptr_project_copy->isRunning()==true){
		flag=true;
		return flag;
	}


	return flag;
}