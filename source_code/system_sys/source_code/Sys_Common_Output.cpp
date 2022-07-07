//#include "Sys_Common_Output.h"
#include "Sys_Headers_Precompiled.h"

//init static members
Sys_Output_Division *Sys_Common_Output::output_system=NULL;
Sys_Output_Division *Sys_Common_Output::output_fpl=NULL;
Sys_Output_Division *Sys_Common_Output::output_excep=NULL;
Sys_Output_Division *Sys_Common_Output::output_hyd=NULL;
Sys_Output_Division *Sys_Common_Output::output_madm=NULL;
Sys_Output_Division *Sys_Common_Output::output_dam=NULL;
Sys_Output_Division *Sys_Common_Output::output_risk=NULL;
Sys_Output_Division *Sys_Common_Output::output_alt=NULL;
Sys_Output_Division *Sys_Common_Output::output_cost=NULL;
Sys_Output_Division *Sys_Common_Output::output_hydrol = NULL;

//constructor
Sys_Common_Output::Sys_Common_Output(void){
}

//destructor
Sys_Common_Output::~Sys_Common_Output(void){
}
//____________________
//public
//(static) Allocate the new Sys_Output_Division class for the Sys-modul
 void Sys_Common_Output::new_output_system(QTextEdit *ptr_editor){
	if(output_system==NULL){
		output_system=new Sys_Output_Division(_sys_system_modules::SYS_SYS);
		output_system->set_file("logfile_SYS");
		output_system->set_gui_texteditor(ptr_editor);
	}
}
//(static) Allocate the new Sys_Output_Division class for the Fpl-modul 
void Sys_Common_Output::new_output_fpl(QTextEdit *ptr_editor){
	if(output_fpl==NULL){
		output_fpl=new Sys_Output_Division(_sys_system_modules::FPL_SYS);
		output_fpl->set_file("logfile_FPL");
		output_fpl->set_gui_texteditor(ptr_editor);	
	}
}
//(static) Allocate the new Sys_Output_Division class for the Sys_exception-modul
void Sys_Common_Output::new_output_excep(QTextEdit *ptr_editor){
	if(output_excep==NULL){
		output_excep=new Sys_Output_Division(_sys_system_modules::EXC_SYS);
		output_excep->set_file("exception");
		output_excep->set_gui_texteditor(ptr_editor);	
	}
}
//(static) Allocate the new Sys_Output_Division class for the Hyd-modul
void Sys_Common_Output::new_output_hyd(QTextEdit *ptr_editor){
	if(output_hyd==NULL){
		output_hyd=new Sys_Output_Division(_sys_system_modules::HYD_SYS);
		output_hyd->set_file("logfile_HYD");
		output_hyd->set_gui_texteditor(ptr_editor);	
	}
}
//(static) Allocate the new Sys_Output_Division class for the Madm-modul
void Sys_Common_Output::new_output_madm(QTextEdit *ptr_editor){
	if(output_madm==NULL){
		output_madm=new Sys_Output_Division(_sys_system_modules::MADM_SYS);
		output_madm->set_file("logfile_MADM");
		output_madm->set_gui_texteditor(ptr_editor);	
	}
}
//(static) Allocate the new Sys_Output_Division class for the dam-modul
void Sys_Common_Output::new_output_dam(QTextEdit *ptr_editor){
	if(output_dam==NULL){
		output_dam=new Sys_Output_Division(_sys_system_modules::DAM_SYS);
		output_dam->set_file("logfile_DAM");
		output_dam->set_gui_texteditor(ptr_editor);	
	}
}
//(static) Allocate the new Sys_Output_Division class for the risk-modul
void Sys_Common_Output::new_output_risk(QTextEdit *ptr_editor){
	if(output_risk==NULL){
		output_risk=new Sys_Output_Division(_sys_system_modules::RISK_SYS);
		output_risk->set_file("logfile_RISK");
		output_risk->set_gui_texteditor(ptr_editor);	
	}
}
//Allocate the new Sys_Output_Division class for the alt-modul (static)
void Sys_Common_Output::new_output_alt(QTextEdit *ptr_editor){
	if(Sys_Common_Output::output_alt==NULL){
		Sys_Common_Output::output_alt=new Sys_Output_Division(_sys_system_modules::ALT_SYS);
		Sys_Common_Output::output_alt->set_file("logfile_ALT");
		Sys_Common_Output::output_alt->set_gui_texteditor(ptr_editor);	
	}
}
//Allocate the new Sys_Output_Division class for the cost-modul (static)
void Sys_Common_Output::new_output_cost(QTextEdit *ptr_editor){
	if(Sys_Common_Output::output_cost==NULL){
		Sys_Common_Output::output_cost=new Sys_Output_Division(_sys_system_modules::COST_SYS);
		Sys_Common_Output::output_cost->set_file("logfile_COST");
		Sys_Common_Output::output_cost->set_gui_texteditor(ptr_editor);	
	}
}
//Allocate the new Sys_Output_Division class for the hydrol-modul (static)
void Sys_Common_Output::new_output_hydrol(QTextEdit *ptr_editor) {
	if (Sys_Common_Output::output_hydrol == NULL) {
		Sys_Common_Output::output_hydrol = new Sys_Output_Division(_sys_system_modules::HYDROL_SYS);
		Sys_Common_Output::output_hydrol->set_file("logfile_HYDROL");
		Sys_Common_Output::output_hydrol->set_gui_texteditor(ptr_editor);
	}

}
//(static) Delete the Sys_Output_Division class for the Sys-modul
void Sys_Common_Output::delete_output_system(void){
	if(output_system!=NULL){
		delete output_system;
		output_system=NULL;
	}
}
//(static) Delete the Sys_Output_Division class for the Fpl-modul
void Sys_Common_Output::delete_output_fpl(void){
	if(output_fpl!=NULL){
		delete output_fpl;
		output_fpl=NULL;
	}
}
//(static) Delete the Sys_Output_Division class for the Sys-exception-modul
void Sys_Common_Output::delete_output_excep(void){
	if(output_excep!=NULL){
		delete output_excep;
		output_excep=NULL;
	}
}
//(static)Delete the Sys_Output_Division class for the Hyd-modul
void Sys_Common_Output::delete_output_hyd(void){
	if(output_hyd!=NULL){
		delete output_hyd;
		output_hyd=NULL;
	}
}
//(static) Delete the Sys_Output_Division class for the Madm-modul
void Sys_Common_Output::delete_output_madm(void){
	if(output_madm!=NULL){
		delete output_madm;
		output_madm=NULL;
	}
}
//(static) Delete the Sys_Output_Division class for the dam-modul
void Sys_Common_Output::delete_output_dam(void){
	if(output_dam!=NULL){
		delete output_dam;
		output_dam=NULL;
	}
}
//(static) Delete the Sys_Output_Division class for the risk-modul
void Sys_Common_Output::delete_output_risk(void){
	if(output_risk!=NULL){
		delete output_risk;
		output_risk=NULL;
	}
}
//Delete the Sys_Output_Division class for the alt-modul (static)
void Sys_Common_Output::delete_output_alt(void){
	if(Sys_Common_Output::output_alt!=NULL){
		delete Sys_Common_Output::output_alt;
		Sys_Common_Output::output_alt=NULL;
	}
}
//Delete the Sys_Output_Division class for the cost-modul (static)
void Sys_Common_Output::delete_output_cost(void){
	if(Sys_Common_Output::output_cost!=NULL){
		delete Sys_Common_Output::output_cost;
		Sys_Common_Output::output_cost=NULL;
	}
}
//Delete the Sys_Output_Division class for the hydrol-modul (static)
void Sys_Common_Output::delete_output_hydrol(void) {
	if (Sys_Common_Output::output_hydrol != NULL) {
		delete Sys_Common_Output::output_hydrol;
		Sys_Common_Output::output_hydrol = NULL;
	}
}