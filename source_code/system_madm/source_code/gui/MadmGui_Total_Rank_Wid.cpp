#include "Madm_Headers_Precompiled.h"
//#include "MadmGui_Total_Rank_Wid.h"


//Default constructor
MadmGui_Total_Rank_Wid::MadmGui_Total_Rank_Wid(QWidget *parent): _Sys_Data_Wid(parent){
	this->setupUi(this);
	this->area2print=this->scrollAreaWidgetContents;
	//count the memory
	Sys_Memory_Count::self()->add_mem(sizeof(MadmGui_Total_Rank_Wid), _sys_system_modules::MADM_SYS);
}
//Default destructor
MadmGui_Total_Rank_Wid::~MadmGui_Total_Rank_Wid(void){

	//count the memory
	Sys_Memory_Count::self()->minus_mem(sizeof(MadmGui_Total_Rank_Wid), _sys_system_modules::MADM_SYS);
}
//______
//public
//Set tooltip of the header
void MadmGui_Total_Rank_Wid::set_header_tooltip(const string tooltip, QTreeWidgetItem *item){
	this->head_label->setToolTip(tooltip.c_str());
	this->set_identifier(tooltip, item);
	string buffer;
	buffer=_Sys_Data_Wid::get_last_string_string_id(tooltip);
	this->label_id->setText(buffer.c_str());
}
///Set-up the widget
void MadmGui_Total_Rank_Wid::set_up_widget(QSqlDatabase *ptr_database, const int areastate, const int set_id, const _risk_type risktype){

	//set header
	string buffer;
	if(risktype==_risk_type::nobreak_risk){
		buffer="Ranking (Nobreak risk approach)";
	}
	else if(risktype==_risk_type::catchment_risk){
		buffer="Ranking (Catchment risk approach)";
	}
	else{
		buffer="Ranking (UNKNOWN risk approach)";
	}
	this->head_label->setText(buffer.c_str());

	//total
	tableWidget_total->set_up_widget(ptr_database, areastate, set_id, risktype, _madm_approach_type::approach_total);
	//saw standard
	tableWidget_saw_standard->set_up_widget(ptr_database, areastate, set_id, risktype, _madm_approach_type::approach_saw_standard);
	//saw range
	tableWidget_saw_range->set_up_widget(ptr_database, areastate, set_id, risktype, _madm_approach_type::approach_saw_range);
	//topsis
	tableWidget_topsis->set_up_widget(ptr_database, areastate, set_id, risktype, _madm_approach_type::approach_topsis);

	//electre
	tableWidget_electre->set_up_widget(ptr_database, areastate, set_id, risktype, _madm_approach_type::approach_electre);

}
