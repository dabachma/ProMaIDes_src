#include "Risk_Headers_Precompiled.h"
//#include "RiskGui_Result_Reliability_Dia.h"

//Default constructor
RiskGui_Result_Reliability_Dia::RiskGui_Result_Reliability_Dia(QWidget *parent) : Sys_Multi_CheckBox_Dia(parent){
	QIcon icon;
	icon.addFile(":risk_icon");

	this->set_number_check_boxes(3, icon);
	stringstream buffer;
	buffer << "Choose which already generated results are reliable \n for a further use in the risk calculation";
	this->set_main_text_label(buffer.str());
	buffer.str("");
	this->set_window_title("Set reliability of the results");
	this->get_ptr_check_box(0)->setText("Results of the fpl calculation are reliable");
	this->get_ptr_check_box(1)->setText("Results of the hydraulic calculation are reliable");
	this->get_ptr_check_box(2)->setText("Results of the damage calculation are reliable");

	


	QObject::connect(this->get_ptr_check_box(1), SIGNAL(clicked()), this, SLOT(check_flag_setting()));
	QObject::connect(this->get_ptr_check_box(2), SIGNAL(clicked()), this, SLOT(check_flag_setting()));


	Sys_Memory_Count::self()->add_mem(sizeof(RiskGui_Result_Reliability_Dia),_sys_system_modules::RISK_SYS);//count the memory
}
//Default destructor
RiskGui_Result_Reliability_Dia::~RiskGui_Result_Reliability_Dia(void){

	Sys_Memory_Count::self()->minus_mem(sizeof(RiskGui_Result_Reliability_Dia),_sys_system_modules::RISK_SYS);//count the memory

}
//__________
//public
//Start the dialog
bool RiskGui_Result_Reliability_Dia::start_dialog(QSqlDatabase *ptr_database, const _sys_system_id id, const bool fpl_important){
	this->adjustSize();
	

	this->get_ptr_check_box(0)->setEnabled(fpl_important);
	this->get_ptr_check_box(2)->setEnabled(false);

	if(fpl_important==false){
		this->get_ptr_check_box(0)->setChecked(true);
	}
	else{
		this->get_ptr_check_box(1)->setChecked(false);
		this->get_ptr_check_box(1)->setEnabled(false);
		this->get_ptr_check_box(2)->setChecked(false);
		this->get_ptr_check_box(2)->setEnabled(false);

	}
	
	int decision=this->exec();
	//rejected
	if(decision ==0){
		return false;
	}
	//accepted; read the path and file name out via get_file_path(const int index) 
	else{
		if(this->check_second_dialog_required(ptr_database, id)==true){
			Sys_Diverse_Text_Dia dialog2;
			
			dialog2.set_dialog_question(this->generate_question_txt(ptr_database, id));
			dialog2.setIcon(QMessageBox::Warning);
			bool flag2=dialog2.start_dialog();
			if(flag2==true){
				return true;
			}
			else{
				return false;
			}
		}
		else{
			return true;
		}
	}
}
//Get the risk results reliability flags
_risk_state_flags RiskGui_Result_Reliability_Dia::get_risk_state_flags(void){
	_risk_state_flags buffer;
	buffer.risk_state_flag=true;
	buffer.reliability_fpl_result=this->get_ptr_check_box(0)->isChecked();
	buffer.reliability_hyd_result=this->get_ptr_check_box(1)->isChecked();
	buffer.reliability_dam_result=this->get_ptr_check_box(2)->isChecked();
	buffer.catchment_risk_applied=true;
	buffer.nobreak_risk_applied=true;

	return buffer;

}
//_______
//private
//Generate question text
string RiskGui_Result_Reliability_Dia::generate_question_txt(QSqlDatabase *ptr_database, const _sys_system_id id){
	ostringstream txt;
	txt<<"All generated risk relevant results of this system state will be deleted in database:"<< endl;
	if(Risk_System::check_detailed_risk_results_available(ptr_database, id)==true){
		txt << "   The results of the risk-modul"<<endl;
	}
	if(this->get_ptr_check_box(0)->isChecked()==false){
		txt<<"   The results of the fpl-modul" << endl;
	}
	if(this->get_ptr_check_box(1)->isChecked()==false){
		txt<<"   The results of the hyd-modul" << endl;
	}
	if(this->get_ptr_check_box(2)->isChecked()==false){
		txt<<"   The results of the dam-modul" << endl;
	}
	txt<<"Do you want to continue?" << endl;
		
	return txt.str();

}
///Check if a second warning dialog is required
bool RiskGui_Result_Reliability_Dia::check_second_dialog_required(QSqlDatabase *ptr_database, const _sys_system_id id){

	if(this->get_ptr_check_box(2)->isChecked()==true && this->get_ptr_check_box(1)->isChecked()==true &&
		this->get_ptr_check_box(0)->isChecked()==true &&
		Risk_System::check_detailed_risk_results_available(ptr_database, id)==false){
		return false;
	}
	else{
		return true;
	}

}
//________
//private slots
//Check the settings of the flags
void RiskGui_Result_Reliability_Dia::check_flag_setting(void){
	if(this->get_ptr_check_box(1)->isChecked()==false){
		this->get_ptr_check_box(2)->setChecked(false);
		this->get_ptr_check_box(2)->setEnabled(false);
	}
	else{
		this->get_ptr_check_box(2)->setEnabled(true);
	}
	if(this->get_ptr_check_box(2)->isChecked()==true){
		this->get_ptr_check_box(1)->setChecked(true);
	}


}