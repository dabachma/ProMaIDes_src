#include "Sys_Headers_Precompiled.h"
//#include "Sys_Status_Bar_Wid.h"

//constructor
Sys_Status_Bar_Wid::Sys_Status_Bar_Wid(void){
	this->setupUi(this);
	string buff;
	buff ="FPL-thread is not running";
	this->label_status_fpl->setToolTip(buff.c_str());
	buff ="HYD-thread is not running";
	this->label_status_hyd->setToolTip(buff.c_str());
	buff ="DAM-thread is not running";
	this->label_status_dam->setToolTip(buff.c_str());
	buff ="RISK-thread is not running";
	this->label_status_risk->setToolTip(buff.c_str());
	buff ="MADM-thread is not running";
	this->label_status_madm->setToolTip(buff.c_str());
	buff = "No floodplains are running";
	this->label_status_cgpu->setToolTip(buff.c_str());

	this->close_flag=false;

	this->my_close->setToolTip("Press to reduce the information");
	QObject::connect(this->my_close,SIGNAL(clicked(bool )), this, SLOT(close_wid(bool )));

	Sys_Memory_Count::self()->add_mem(sizeof(Sys_Status_Bar_Wid), _sys_system_modules::SYS_SYS);
}

//destructor
Sys_Status_Bar_Wid::~Sys_Status_Bar_Wid(void){
	Sys_Memory_Count::self()->minus_mem(sizeof(Sys_Status_Bar_Wid), _sys_system_modules::SYS_SYS);
}
//________________________________
//public
//Get pointer to the my close button
QPushButton * Sys_Status_Bar_Wid::get_ptr_my_close(void){
	return this->my_close;
}
//set the db connection to true "yes" or "no" (false)
void Sys_Status_Bar_Wid::set_dbcon(const bool flag){
	if(flag==true){
		this->label_status_dbcon->setText("yes");
	}
	else if(flag==false){
		this->label_status_dbcon->setText("no");
	}
}
//set the warning number
void Sys_Status_Bar_Wid::set_warning_number(const int number, const QString tooltip){
	this->label_status_warningnum->setNum(number);
	this->label_status_warningnum->setToolTip(tooltip);
}
//set the error number
void Sys_Status_Bar_Wid::set_error_number(const int number, const QString tooltip){
	this->label_status_errornum->setNum(number);
	this->label_status_errornum->setToolTip(tooltip);
}
//Set the number of area state and measure state
void Sys_Status_Bar_Wid::set_system_state(const _sys_system_id id){
	ostringstream buffer;
	buffer<< id.measure_nr;
	this->label_measure_state->setText(buffer.str().c_str());
	buffer.str("");
	buffer<< id.area_state;
	this->label_area_state->setText(buffer.str().c_str());
}
//set the fpl_thread to true "yes" or "no" (false)
void Sys_Status_Bar_Wid::set_fplthread(const bool flag){
	ostringstream buff;
	if(flag==true){
		if(this->label_status_fpl->text()=="no"){
			buff << "FPL-thread starts at " << Sys_Output_Division::set_time();
			this->label_status_fpl->setToolTip(buff.str().c_str());
		}
		this->label_status_fpl->setText("yes");

	}
	else if(flag==false){
		this->label_status_fpl->setText("no");
		buff << "FPL-thread finished at " << Sys_Output_Division::set_time();
		this->label_status_fpl->setToolTip(buff.str().c_str());
	}
}
//set the hyd_thread to true "yes" or "no" (false)
void Sys_Status_Bar_Wid::set_hydthread(const bool flag, const string number){
	ostringstream buff;
	if(flag==true){
		if(this->label_status_hyd->text()=="no"){
			buff << "HYD-thread starts at " << Sys_Output_Division::set_time();
			this->label_status_hyd->setToolTip(buff.str().c_str());
		}
		if(number.empty()==true){
			this->label_status_hyd->setText("yes");
		}
		else{
			buff<<"yes ("<<number<<")";
			this->label_status_hyd->setText(buff.str().c_str());
			buff.str("");
		}

	}
	else if(flag==false){
		this->label_status_hyd->setText("no");
		buff << "HYD-thread finished at " << Sys_Output_Division::set_time();
		this->label_status_hyd->setToolTip(buff.str().c_str());

		this->set_cpu_gpu_count(0, 0);
	}
}
//set the Cpu and Gpu fps
void Sys_Status_Bar_Wid::set_cpu_gpu_count(unsigned int cpu_count, unsigned int gpu_count) {

	ostringstream buff;
	buff << cpu_count << "C " << gpu_count << "G";
	this->label_status_cgpu->setText(buff.str().c_str());
	buff.str("");

	buff << "The simulation is set to run " << cpu_count << " FP/s on the CPU, and " << gpu_count;
	buff << " FP/s on the GPU (not necessarily in parallel)";

	this->label_status_cgpu->setToolTip(buff.str().c_str());
}
//Set the hydraulic-status text-label of the statusbar to "sleep"
void Sys_Status_Bar_Wid::set_hydthread_sleep(const bool flag){
	if(flag==true){
		this->label_status_risk->setText("sleep");
	}
	else{
		this->label_status_risk->setText("yes");
	}
}
//set the dam_thread to true "yes" or "no" (false)
void Sys_Status_Bar_Wid::set_damthread(const bool flag){
	ostringstream buff;
	if(flag==true){
		if(this->label_status_dam->text()=="no"){
			buff << "DAM-thread starts at " << Sys_Output_Division::set_time();
			this->label_status_dam->setToolTip(buff.str().c_str());
		}
		this->label_status_dam->setText("yes");

	}
	else if(flag==false){
		this->label_status_dam->setText("no");
		buff << "DAM-thread finished at " << Sys_Output_Division::set_time();
		this->label_status_dam->setToolTip(buff.str().c_str());
	}
}
//Set the dam-status text-label of the statusbar "sleep"
void Sys_Status_Bar_Wid::set_damthread_sleep(const bool flag){
	if(flag==true){
		this->label_status_dam->setText("sleep");
	}
	else{
		this->label_status_dam->setText("yes");
	}
}
//Set the risk-status text-label of the statusbar to "yes" (true) or "no" (false)
void Sys_Status_Bar_Wid::set_riskthread(const bool flag){
	ostringstream buff;
	if(flag==true){
		if(this->label_status_risk->text()=="no"){
			buff << "RISK-thread starts at " << Sys_Output_Division::set_time();
			this->label_status_risk->setToolTip(buff.str().c_str());
		}
		this->label_status_risk->setText("yes");

	}
	else if(flag==false){
		this->label_status_risk->setText("no");
		buff << "RISK-thread finished at " << Sys_Output_Division::set_time();
		this->label_status_risk->setToolTip(buff.str().c_str());
	}
}
//Set the risk-status text-label of the statusbar to "sleep"
void Sys_Status_Bar_Wid::set_riskthread_sleep(const bool flag){
	if(flag==true){
		this->label_status_risk->setText("sleep");
	}
	else{
		this->label_status_risk->setText("yes");
	}
}
//Set the madm-status text-label of the statusbar to "yes" (true) or "no" (false)
void Sys_Status_Bar_Wid::set_madmthread(const bool flag){
	ostringstream buff;
	if(flag==true){
		if(this->label_status_madm->text()=="no"){
			buff << "MADM-thread starts at " << Sys_Output_Division::set_time();
			this->label_status_madm->setToolTip(buff.str().c_str());
		}
		this->label_status_madm->setText("yes");

	}
	else if(flag==false){
		this->label_status_madm->setText("no");
		buff << "MADM-thread finished at " << Sys_Output_Division::set_time();
		this->label_status_madm->setToolTip(buff.str().c_str());
	}
}
//Get a pointer to the risk-state check box 
QCheckBox* Sys_Status_Bar_Wid::get_ptr_risk_state_check_box(void){
	return this->checkBox_risk_state;
}
//Set tooltip for risk-state check box
void Sys_Status_Bar_Wid::set_tooltip_risk_state_check_box(const bool fpl, const bool dam, const bool hyd){
	ostringstream buffer;

	buffer<<"Reliability of:"<<endl;
	buffer<<" FPL "<<functions::convert_boolean2string(fpl)<<endl;
	buffer<<" HYD "<<functions::convert_boolean2string(hyd)<<endl; 
	buffer<<" DAM "<<functions::convert_boolean2string(dam);
	this->checkBox_risk_state->setToolTip(buffer.str().c_str());
}
//Reset tooltip for risk-state check box
void Sys_Status_Bar_Wid::reset_tooltip_risk_state_check_box(void){
	this->checkBox_risk_state->setToolTip("");	
}
//Set the memory-counter label of the statusbar
void Sys_Status_Bar_Wid::set_required_memory(const QString txt, const QString tooltip){
	this->label_memnumber->setText(txt);
	this->label_memnumber->setToolTip(tooltip);
}
//_____
//public slot
//Close the widget
void Sys_Status_Bar_Wid::close_wid(bool flag){
	if(flag==false){
		emit_close(this->close_flag);
		if(this->close_flag==true){
			
			this->close_flag=false;
			this->my_close->setText("-");
			this->my_close->setToolTip("Press to reduce the information");
			this->widget_close->show();
		}
		else{
			this->widget_close->close();
			this->close_flag=true;
			this->my_close->setText("+");
			this->my_close->setToolTip("Press to expand the information");

		}

	}
}
