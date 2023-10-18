#include "Dam_Headers_Precompiled.h"
//#include "DamGui_Ci_Point_Wid.h"


//Default constructor
DamGui_Ci_Point_Wid::DamGui_Ci_Point_Wid(QWidget *parent): _Sys_Data_Wid(parent){
	this->setupUi(this);
	this->area2print=this->scrollAreaWidgetContents;
	//count the memory
	Sys_Memory_Count::self()->add_mem(sizeof(DamGui_Ci_Point_Wid), _sys_system_modules::DAM_SYS);


}
//Default destructor
DamGui_Ci_Point_Wid::~DamGui_Ci_Point_Wid(void){

	//count the memory
	Sys_Memory_Count::self()->minus_mem(sizeof(DamGui_Ci_Point_Wid), _sys_system_modules::DAM_SYS);
}
//_______
//public
//Set-up the widget
void DamGui_Ci_Point_Wid::set_up_widget(QSqlDatabase *ptr_database,  const _sys_system_id id){

	QSqlQueryModel model;
	int number=0;
	int number_elect=0;
	int number_info_tec=0;
	int number_wat_sup=0;
	int number_wat_treat=0;
	int number_energy = 0;

	int number_emerg = 0;
	int number_health = 0;
	int number_trans_good = 0;
	int number_trans_per= 0;
	int number_govern = 0;
	int number_haz= 0;
	int number_indus= 0;
	int number_cult = 0;
	int number_edu = 0;
	int number_pri = 0;

	number=Dam_CI_Point::select_relevant_points_database(&model, ptr_database,  id, false);

	for(int i=0; i< number; i++){
		if(model.record(i).value((Dam_CI_Point::point_table->get_column_name(dam_label::sector_id)).c_str()).toInt()==1){
			number_elect++;	
		}
		else if(model.record(i).value((Dam_CI_Point::point_table->get_column_name(dam_label::sector_id)).c_str()).toInt()==2){
			number_info_tec++;	
		}
		else if(model.record(i).value((Dam_CI_Point::point_table->get_column_name(dam_label::sector_id)).c_str()).toInt()==3){
			number_wat_sup++;	
		}
		else if(model.record(i).value((Dam_CI_Point::point_table->get_column_name(dam_label::sector_id)).c_str()).toInt()==4){
			number_wat_treat++;	
		}
		else if (model.record(i).value((Dam_CI_Point::point_table->get_column_name(dam_label::sector_id)).c_str()).toInt() == 5) {
			number_energy++;
		}


		if (model.record(i).value((Dam_CI_Point::point_table->get_column_name(dam_label::sector_id)).c_str()).toInt() == 10) {
			number_emerg++;
		}
		else if (model.record(i).value((Dam_CI_Point::point_table->get_column_name(dam_label::sector_id)).c_str()).toInt() == 11) {
			number_health++;
		}
		else if (model.record(i).value((Dam_CI_Point::point_table->get_column_name(dam_label::sector_id)).c_str()).toInt() == 12) {
			number_trans_good++;
		}
		else if (model.record(i).value((Dam_CI_Point::point_table->get_column_name(dam_label::sector_id)).c_str()).toInt() == 13) {
			number_trans_per++;
		}
		else if (model.record(i).value((Dam_CI_Point::point_table->get_column_name(dam_label::sector_id)).c_str()).toInt() == 14) {
			number_govern++;
		}
		else if (model.record(i).value((Dam_CI_Point::point_table->get_column_name(dam_label::sector_id)).c_str()).toInt() == 15) {
			number_haz++;
		}
		else if (model.record(i).value((Dam_CI_Point::point_table->get_column_name(dam_label::sector_id)).c_str()).toInt() == 16) {
			number_indus++;
		}
		else if (model.record(i).value((Dam_CI_Point::point_table->get_column_name(dam_label::sector_id)).c_str()).toInt() == 17) {
			number_cult++;
		}
		else if (model.record(i).value((Dam_CI_Point::point_table->get_column_name(dam_label::sector_id)).c_str()).toInt() == 18) {
			number_edu++;
		}
		else if (model.record(i).value((Dam_CI_Point::point_table->get_column_name(dam_label::sector_id)).c_str()).toInt() == 19) {
			number_pri++;
		}

	}


	ostringstream buff_stream;
	buff_stream<<number;
	this->lineEdit_tot_no->setText(buff_stream.str().c_str());
	buff_stream.str("");
	buff_stream<<number_elect;
	this->lineEdit_elec_no->setText(buff_stream.str().c_str());
	buff_stream.str("");
	buff_stream<<number_info_tec;
	this->lineEdit_info_tec_no->setText(buff_stream.str().c_str());
	buff_stream.str("");
	buff_stream<<number_wat_sup;
	this->lineEdit_water_sup_no->setText(buff_stream.str().c_str());
	buff_stream.str("");
	buff_stream<<number_wat_treat;
	this->lineEdit_water_treat_no->setText(buff_stream.str().c_str());
	buff_stream.str("");
	buff_stream << number_energy;
	this->lineEdit_energy_no->setText(buff_stream.str().c_str());
	buff_stream.str("");






	buff_stream << number_emerg;
	this->lineEdit_emerg_no->setText(buff_stream.str().c_str());
	buff_stream.str("");
	buff_stream << number_health;
	this->lineEdit_care_no->setText(buff_stream.str().c_str());
	buff_stream.str("");
	buff_stream << number_trans_good;
	this->lineEdit_trans_go_no->setText(buff_stream.str().c_str());
	buff_stream.str("");
	buff_stream << number_trans_per;
	this->lineEdit_trans_pe_no->setText(buff_stream.str().c_str());
	buff_stream.str("");


	buff_stream << number_govern;
	this->lineEdit_govern_no->setText(buff_stream.str().c_str());
	buff_stream.str("");
	buff_stream << number_haz;
	this->lineEdit_haz_no->setText(buff_stream.str().c_str());
	buff_stream.str("");
	buff_stream << number_indus;
	this->lineEdit_ind_no->setText(buff_stream.str().c_str());
	buff_stream.str("");
	buff_stream << number_cult;
	this->lineEdit_cult_no->setText(buff_stream.str().c_str());
	buff_stream.str("");

	buff_stream << number_edu;
	this->lineEdit_edu_no->setText(buff_stream.str().c_str());
	buff_stream.str("");
	buff_stream << number_pri;
	this->lineEdit_pri_no->setText(buff_stream.str().c_str());
	buff_stream.str("");



	
}
