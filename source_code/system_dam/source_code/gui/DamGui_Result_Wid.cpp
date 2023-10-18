#include "Dam_Headers_Precompiled.h"
//#include "DamGui_Result_Wid.h"


//Default constructor
DamGui_Result_Wid::DamGui_Result_Wid(QWidget *parent): _Sys_Data_Wid(parent){
	this->setupUi(this);
	this->area2print=this->scrollAreaWidgetContents;
	//count the memory
	Sys_Memory_Count::self()->add_mem(sizeof(DamGui_Result_Wid), _sys_system_modules::DAM_SYS);
}
//Default destructor
DamGui_Result_Wid::~DamGui_Result_Wid(void){

	//count the memory
	Sys_Memory_Count::self()->minus_mem(sizeof(DamGui_Result_Wid), _sys_system_modules::DAM_SYS);


}
//_______
//public
//Set tooltip of the header
void DamGui_Result_Wid::set_header_tooltip(const string tooltip, QTreeWidgetItem *item){
	this->head_label->setToolTip(tooltip.c_str());

	this->set_identifier(tooltip, item);
	string buffer;
	buffer=_Sys_Data_Wid::get_last_string_string_id(tooltip);
	this->label_id->setText(buffer.c_str());
}
//Set-up the widget
void DamGui_Result_Wid::set_up_widget(QSqlDatabase *ptr_database,  const int glob_id){

	QSqlQueryModel model;
	int number=0;

	number=Dam_Damage_System::select_data_in_erg_table(&model, ptr_database,  glob_id);

	if(number==0){
		this->setEnabled(false);
		this->setToolTip("No results found!");
		return;
	}

	double sum=0.0;
	double sum_total=0.0;
	ostringstream buff_stream;
	buff_stream<< FORMAT_FIXED_REAL<< P(1);
	//ecn
	buff_stream<<model.record(0).value((Dam_Damage_System::erg_table->get_column_name(dam_label::immob_dam)).c_str()).toString().toStdString();
	sum=sum+model.record(0).value((Dam_Damage_System::erg_table->get_column_name(dam_label::immob_dam)).c_str()).toDouble();
	this->lineEdit_immobil->setText(buff_stream.str().c_str());
	buff_stream.str("");
	buff_stream<<model.record(0).value((Dam_Damage_System::erg_table->get_column_name(dam_label::mob_dam)).c_str()).toString().toStdString();
	sum=sum+model.record(0).value((Dam_Damage_System::erg_table->get_column_name(dam_label::mob_dam)).c_str()).toDouble();
	this->lineEdit_mobil->setText(buff_stream.str().c_str());
	buff_stream.str("");
	buff_stream<<model.record(0).value((Dam_Damage_System::erg_table->get_column_name(dam_label::immob_5_quant)).c_str()).toString().toStdString();
	this->lineEdit_immobil_5->setText(buff_stream.str().c_str());
	buff_stream.str("");
	buff_stream<<model.record(0).value((Dam_Damage_System::erg_table->get_column_name(dam_label::immob_95_quant)).c_str()).toString().toStdString();
	this->lineEdit_immobil_95->setText(buff_stream.str().c_str());
	buff_stream.str("");
	buff_stream<<model.record(0).value((Dam_Damage_System::erg_table->get_column_name(dam_label::mob_5_quant)).c_str()).toString().toStdString();
	this->lineEdit_mobil_5->setText(buff_stream.str().c_str());
	buff_stream.str("");
	buff_stream<<model.record(0).value((Dam_Damage_System::erg_table->get_column_name(dam_label::mob_95_quant)).c_str()).toString().toStdString();
	this->lineEdit_mobil_95->setText(buff_stream.str().c_str());
	buff_stream.str("");
	buff_stream << sum;
	this->lineEdit_total_ecn->setText(buff_stream.str().c_str());
	buff_stream.str("");
	sum_total=sum_total+sum;
	sum=0.0;

	//eco
	buff_stream<<model.record(0).value((Dam_Damage_System::erg_table->get_column_name(dam_label::btype_cost)).c_str()).toString().toStdString();
	sum=sum+model.record(0).value((Dam_Damage_System::erg_table->get_column_name(dam_label::btype_cost)).c_str()).toDouble();
	this->lineEdit_btype->setText(buff_stream.str().c_str());
	buff_stream.str("");
	buff_stream<<model.record(0).value((Dam_Damage_System::erg_table->get_column_name(dam_label::soil_cost)).c_str()).toString().toStdString();
	sum=sum+model.record(0).value((Dam_Damage_System::erg_table->get_column_name(dam_label::soil_cost)).c_str()).toDouble();
	this->lineEdit_soil_ero->setText(buff_stream.str().c_str());
	buff_stream.str("");
	buff_stream << sum;
	this->lineEdit_total_eco->setText(buff_stream.str().c_str());
	buff_stream.str("");
	sum_total=sum_total+sum;
	sum=0.0;

	//monetary
	buff_stream << sum_total;
	this->lineEdit_total_sum->setText(buff_stream.str().c_str());
	buff_stream.str("");
	sum_total=0.0;

	//pop
	buff_stream<<model.record(0).value((Dam_Damage_System::erg_table->get_column_name(dam_label::pop_affected)).c_str()).toString().toStdString();
	this->lineEdit_pop_aff->setText(buff_stream.str().c_str());
	buff_stream.str("");
	buff_stream<<model.record(0).value((Dam_Damage_System::erg_table->get_column_name(dam_label::pop_endangered)).c_str()).toString().toStdString();
	this->lineEdit_pop_end->setText(buff_stream.str().c_str());
	buff_stream.str("");
	//pys
	double dens=0.0;
	double dens_sum=0.0;
	double without=0.0;
	double without_sum=0.0;
	without=model.record(0).value((Dam_Damage_System::erg_table->get_column_name(dam_label::crit_age_50_59)).c_str()).toDouble();
	dens=model.record(0).value((Dam_Damage_System::erg_table->get_column_name(dam_label::denscrit_age50_59)).c_str()).toDouble();
	dens_sum=dens_sum+dens;
	without_sum=without_sum+without;
	buff_stream<<without<<" ("<<dens<<")";
	this->lineEdit_pys_50->setText(buff_stream.str().c_str());
	buff_stream.str("");

	without=model.record(0).value((Dam_Damage_System::erg_table->get_column_name(dam_label::crit_age_80)).c_str()).toDouble();
	dens=model.record(0).value((Dam_Damage_System::erg_table->get_column_name(dam_label::denscrit_age_80)).c_str()).toDouble();
	dens_sum=dens_sum+dens;
	without_sum=without_sum+without;
	buff_stream<<without<<" ("<<dens<<")";
	this->lineEdit_pys_80->setText(buff_stream.str().c_str());
	buff_stream.str("");

	without=model.record(0).value((Dam_Damage_System::erg_table->get_column_name(dam_label::crit_female)).c_str()).toDouble();
	dens=model.record(0).value((Dam_Damage_System::erg_table->get_column_name(dam_label::denscrit_female)).c_str()).toDouble();
	dens_sum=dens_sum+dens;
	without_sum=without_sum+without;
	buff_stream<<without<<" ("<<dens<<")";
	this->lineEdit_pys_female->setText(buff_stream.str().c_str());
	buff_stream.str("");

	without=model.record(0).value((Dam_Damage_System::erg_table->get_column_name(dam_label::crit_owner)).c_str()).toDouble();
	dens=model.record(0).value((Dam_Damage_System::erg_table->get_column_name(dam_label::denscrit_owner)).c_str()).toDouble();
	dens_sum=dens_sum+dens;
	without_sum=without_sum+without;
	buff_stream<<without<<" ("<<dens<<")";
	this->lineEdit_pys_owner->setText(buff_stream.str().c_str());
	buff_stream.str("");

	buff_stream<<without_sum<<" ("<<dens_sum<<")";
	this->lineEdit_pys_total->setText(buff_stream.str().c_str());
	buff_stream.str("");

	//sc
	buff_stream<<model.record(0).value((Dam_Damage_System::erg_table->get_column_name(dam_label::sc_pub_build)).c_str()).toString().toStdString();
	this->lineEdit_sc_pub->setText(buff_stream.str().c_str());
	buff_stream.str("");
	buff_stream<<model.record(0).value((Dam_Damage_System::erg_table->get_column_name(dam_label::sc_eco_build)).c_str()).toString().toStdString();
	this->lineEdit_sc_eco->setText(buff_stream.str().c_str());
	buff_stream.str("");
	buff_stream<<model.record(0).value((Dam_Damage_System::erg_table->get_column_name(dam_label::sc_cult_build)).c_str()).toString().toStdString();
	this->lineEdit_sc_cult->setText(buff_stream.str().c_str());
	buff_stream.str("");
	buff_stream<<model.record(0).value((Dam_Damage_System::erg_table->get_column_name(dam_label::sc_person_build)).c_str()).toString().toStdString();
	this->lineEdit_sc_person->setText(buff_stream.str().c_str());
	buff_stream.str("");

	buff_stream << model.record(0).value((Dam_Damage_System::erg_table->get_column_name(dam_label::ci_elect_pt)).c_str()).toString().toStdString();
	this->lineEdit_elect->setText(buff_stream.str().c_str());
	buff_stream.str("");

	buff_stream << model.record(0).value((Dam_Damage_System::erg_table->get_column_name(dam_label::ci_info_tec_pt)).c_str()).toString().toStdString();
	this->lineEdit_info_tec->setText(buff_stream.str().c_str());
	buff_stream.str("");

	buff_stream << model.record(0).value((Dam_Damage_System::erg_table->get_column_name(dam_label::ci_water_sup_pt)).c_str()).toString().toStdString();
	this->lineEdit_water_sup->setText(buff_stream.str().c_str());
	buff_stream.str("");

	buff_stream << model.record(0).value((Dam_Damage_System::erg_table->get_column_name(dam_label::ci_water_treat_pt)).c_str()).toString().toStdString();
	this->lineEdit_water_treat->setText(buff_stream.str().c_str());
	buff_stream.str("");

	buff_stream << model.record(0).value((Dam_Damage_System::erg_table->get_column_name(dam_label::ci_energy_pt)).c_str()).toString().toStdString();
	this->lineEdit_energy->setText(buff_stream.str().c_str());
	buff_stream.str("");

	buff_stream << model.record(0).value((Dam_Damage_System::erg_table->get_column_name(dam_label::ci_health_pt)).c_str()).toString().toStdString();
	this->lineEdit_health->setText(buff_stream.str().c_str());
	buff_stream.str("");


	buff_stream << model.record(0).value((Dam_Damage_System::erg_table->get_column_name(dam_label::ci_social_pt)).c_str()).toString().toStdString();
	this->lineEdit_social->setText(buff_stream.str().c_str());
	buff_stream.str("");

	buff_stream << model.record(0).value((Dam_Damage_System::erg_table->get_column_name(dam_label::ci_mat_pt)).c_str()).toString().toStdString();
	this->lineEdit_mat->setText(buff_stream.str().c_str());
	buff_stream.str("");



}