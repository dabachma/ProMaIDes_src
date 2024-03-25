#include "Risk_Headers_Precompiled.h"
//#include "RiskGui_Risk_Scenario_Dia.h"


//Default constructor
RiskGui_Risk_Scenario_Dia::RiskGui_Risk_Scenario_Dia(QWidget *parent): QDialog(parent){
	this->setupUi(this);
	this->id_hyd_bound_sc=-1;
	this->break_scenario.break_height=0.0;
	this->break_scenario.id_fpl_sec=-1;
	this->break_scenario.id_interval="N";

	this->listWidget_fpl->setSelectionMode(QAbstractItemView::SingleSelection);
	this->listWidget_hyd->setSelectionMode(QAbstractItemView::SingleSelection);

	QObject::connect(this->okButton, SIGNAL(clicked()), this, SLOT(check_selections_correct()));
	QObject::connect(this->cancelButton, SIGNAL(clicked()), this, SLOT(reject()));

	this->type=_risk_type::nobreak_risk;


	Sys_Memory_Count::self()->add_mem(sizeof(RiskGui_Risk_Scenario_Dia)-sizeof(Hyd_Boundary_Szenario_Management)-
		sizeof(Risk_Break_Scenario),_sys_system_modules::RISK_SYS);//count the memory

}
//Default destructor
RiskGui_Risk_Scenario_Dia::~RiskGui_Risk_Scenario_Dia(void){
	Sys_Memory_Count::self()->minus_mem(sizeof(RiskGui_Risk_Scenario_Dia)-sizeof(Hyd_Boundary_Szenario_Management)-
		sizeof(Risk_Break_Scenario),_sys_system_modules::RISK_SYS);//count the memory
}
//_________
//public
//Set the dialog, the list widgets etc.
void RiskGui_Risk_Scenario_Dia::set_dialog(QSqlDatabase *ptr_database, _sys_system_id sys_id, const _risk_type type){
	
	this->type=type;
	if(this->type==_risk_type::nobreak_risk){
		this->label_header->setText("Wrong risk type! Nobreak risk approach is choosen!");
	}
	else if(this->type==_risk_type::scenario_risk){
		this->label_header->setText("Combine one FPL-section with one HYD-boundary scenario to one RISK-break scenario \nof the risk scenario risk approach");
	}
	else if(this->type==_risk_type::catchment_risk){
		this->label_header->setText("Wrong risk type! Nobreak risk approach is choosen!");
	}


	QSqlQueryModel result;
	int number=0;
	QListWidgetItem *item;

	//fpl-section
	Fpl_Section *my_section=NULL;

	number=Fpl_Section::select_relevant_section_database(&result, ptr_database, sys_id, false);

	try{
		my_section=new Fpl_Section[number];
	}
	catch(bad_alloc &t){
		Error msg=this->set_error(0);
		ostringstream info;
		info<< "Info bad alloc: " << t.what() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}

	try{
		//set the section
		for(int i=0; i< number;i++){
			my_section[i].set_systemid(sys_id);
			my_section[i].input_section_perdatabase(&result, i, ptr_database);
			if(my_section[i].get_type_section()!=_fpl_section_types::high_ground_sec){
				item=new QListWidgetItem;
				item->setText(my_section[i].get_string_selection().c_str());
				this->listWidget_fpl->addItem(item);

			}
		}

	}
	catch(Error msg){
		if(my_section!=NULL){
			delete []my_section;
			my_section=NULL;
		}
		throw msg;
	}
	if(my_section!=NULL){
		delete []my_section;
		my_section=NULL;
	}

	//hyd-boundary scenarios
	hyd_bound_sc.set_systemid(sys_id);

	hyd_bound_sc.set_szenario_per_db(ptr_database);
	for(int i=0; i< hyd_bound_sc.get_number_sz(); i++){
		item=new QListWidgetItem;
		item->setText(hyd_bound_sc.get_ptr_sz(i)->get_sz_text().c_str());
		this->listWidget_hyd->addItem(item);	
	}

}
//Start the dialog; it returns true by acception, false by rejection
bool RiskGui_Risk_Scenario_Dia::start_dialog(void){
	int decision =this->exec();
	 //rejected
	if(decision ==0){
		return false;
	}
	//accepted
	else{

		//set the break scenario
		//get selected fpl-section
		for(int i=0; i<this->listWidget_fpl->count(); i++){
			if(this->listWidget_fpl->item(i)->isSelected()==true){
				int id_buff=0;
				stringstream buff_str; 
				buff_str << this->listWidget_fpl->item(i)->text().toStdString();
				buff_str >> id_buff;
				this->break_scenario.id_fpl_sec=id_buff;
				break;	
			}
		}
		


		//get selected hyd-boundary scenario
		for(int i=0; i<this->listWidget_hyd->count(); i++){
			if(this->listWidget_hyd->item(i)->isSelected()==true){
				int id_buff=0;
				stringstream buff_str; 
				buff_str << this->listWidget_hyd->item(i)->text().toStdString();
				buff_str >> id_buff;
				this->id_hyd_bound_sc=id_buff;
				break;	
			}
		}


		

		return true;
	}
}
//Get the data of the break scenario, which was choosen
_fpl_break_info RiskGui_Risk_Scenario_Dia::get_break_scenario_data(void){
	return this->break_scenario;
}
//Get the data of the hydraulic boundary scenario, which was choosen
Hyd_Boundary_Szenario* RiskGui_Risk_Scenario_Dia::get_hyd_boundary_scenario_data(void){
	return this->hyd_bound_sc.get_ptr_sz_id_given(this->id_hyd_bound_sc);
}
//________
//private slot
//Check if the selction are correctly set
bool RiskGui_Risk_Scenario_Dia::check_selections_correct(void){
	bool ok_flag=true;

	QList<QListWidgetItem *> list;
	list=this->listWidget_hyd->selectedItems();
	if(list.count()==0){
		ok_flag=false;
		Sys_Diverse_Text_Dia dialog2(true);
		ostringstream txt;
		txt<<"One HYD-base scenario must be selected for a break scenario calculation!"<< endl;
		dialog2.set_dialog_question(txt.str());
		dialog2.start_dialog();
		return ok_flag;
	}

	list.clear();
	list=this->listWidget_fpl->selectedItems();
	if(this->type==_risk_type::scenario_risk){
		if(list.count()==0){
			ok_flag=false;
			Sys_Diverse_Text_Dia dialog2(true);
			ostringstream txt;
			txt<<"One FPL-section must be selected for a break scenario calculation!"<< endl;
			dialog2.set_dialog_question(txt.str());
			dialog2.start_dialog();
			return ok_flag;

		}	
	}



	if(ok_flag==true){
		this->accept();
	}

	return ok_flag;
}
//________
//private
//Set error(s)
Error RiskGui_Risk_Scenario_Dia::set_error(const int err_type){
		string place="RiskGui_Risk_Scenario_Dia::";
		string help;
		string reason;
		int type=0;
		bool fatal=false;
		stringstream info;
		Error msg;

	switch (err_type){
		case 0://bad_alloc
			place.append("set_dialog(QSqlDatabase *ptr_database, _sys_system_id sys_id, const const _risk_type type)");
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