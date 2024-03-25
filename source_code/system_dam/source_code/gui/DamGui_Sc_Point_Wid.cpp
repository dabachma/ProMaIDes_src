#include "Dam_Headers_Precompiled.h"
//#include "DamGui_Sc_Point_Wid.h"


//Default constructor
DamGui_Sc_Point_Wid::DamGui_Sc_Point_Wid(QWidget *parent): _Sys_Data_Wid(parent){
	this->setupUi(this);
	this->area2print=this->scrollAreaWidgetContents;
	//count the memory
	Sys_Memory_Count::self()->add_mem(sizeof(DamGui_Sc_Point_Wid), _sys_system_modules::DAM_SYS);


}
//Default destructor
DamGui_Sc_Point_Wid::~DamGui_Sc_Point_Wid(void){

	//count the memory
	Sys_Memory_Count::self()->minus_mem(sizeof(DamGui_Sc_Point_Wid), _sys_system_modules::DAM_SYS);
}
//_______
//public
//Set-up the widget
void DamGui_Sc_Point_Wid::set_up_widget(QSqlDatabase *ptr_database,  const _sys_system_id id){

	QSqlQueryModel model;
	int number=0;
	int number_pub=0;
	int number_eco=0;
	int number_cult=0;
	int number_person=0;

	number=Dam_Sc_Point::select_relevant_points_database(&model, ptr_database,  id, false);

	for(int i=0; i< number; i++){
		if(model.record(i).value((Dam_Sc_Point::point_table->get_column_name(dam_label::category_id)).c_str()).toInt()==1){
			number_pub++;	
		}
		else if(model.record(i).value((Dam_Sc_Point::point_table->get_column_name(dam_label::category_id)).c_str()).toInt()==2){
			number_eco++;	
		}
		else if(model.record(i).value((Dam_Sc_Point::point_table->get_column_name(dam_label::category_id)).c_str()).toInt()==3){
			number_cult++;	
		}
		else if(model.record(i).value((Dam_Sc_Point::point_table->get_column_name(dam_label::category_id)).c_str()).toInt()==4){
			number_person++;	
		}
	}


	ostringstream buff_stream;
	buff_stream<<number;
	this->lineEdit_tot_no->setText(buff_stream.str().c_str());
	buff_stream.str("");
	buff_stream<<number_pub;
	this->lineEdit_pub_no->setText(buff_stream.str().c_str());
	buff_stream.str("");
	buff_stream<<number_eco;
	this->lineEdit_eco_no->setText(buff_stream.str().c_str());
	buff_stream.str("");
	buff_stream<<number_cult;
	this->lineEdit_cult_no->setText(buff_stream.str().c_str());
	buff_stream.str("");
	buff_stream<<number_person;
	this->lineEdit_person_no->setText(buff_stream.str().c_str());
	buff_stream.str("");

	
}
