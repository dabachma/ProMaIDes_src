#include "Alt_Headers_Precompiled.h"
//#include "AltGui_Alternative_Info_Dia.h"


//Default constructor
AltGui_Alternative_Info_Dia::AltGui_Alternative_Info_Dia(QWidget *parent): QDialog(parent){

	this->setupUi(this);

	this->just_editing=false;
	this->measure_data.name=label::not_set;
	this->measure_data.replacing=true;
	this->measure_data.measurestate_based_on=0;

	QObject::connect(this->cancelButton, SIGNAL(clicked()), this, SLOT(reject()));
	QObject::connect(this->lineEdit_alt_name, SIGNAL(textChanged(QString )), this, SLOT(check_name(QString )));
	
	//count the memory
	Sys_Memory_Count::self()->add_mem(sizeof(AltGui_Alternative_Info_Dia), _sys_system_modules::ALT_SYS);

}
//Default destructor
AltGui_Alternative_Info_Dia::~AltGui_Alternative_Info_Dia(void){

	//count the memory
	Sys_Memory_Count::self()->minus_mem(sizeof(AltGui_Alternative_Info_Dia), _sys_system_modules::ALT_SYS);
}
//_______
//public
//Set the dialog measure identifier
void AltGui_Alternative_Info_Dia::set_dialog(const _sys_system_id id, const bool for_editing, const _alt_general_info data, const bool fpl_required){
	if(fpl_required==false){
		this->radioButton_fpl->setEnabled(false);
	}
	
	if(for_editing==false){
		this->measure_data.id.area_state=id.area_state;
		this->measure_data.id.measure_nr=id.measure_nr;
		this->just_editing=false;
		ostringstream buffer;
		buffer << this->measure_data.id.measure_nr;
		this->lineEdit_alt_id->setText(buffer.str().c_str());
		buffer.str("");
		buffer << "measure_"<<this->measure_data.id.area_state<<"_"<<this->measure_data.id.measure_nr;
		this->lineEdit_alt_name->setText(buffer.str().c_str());

		this->radioButton_hyd->setChecked(true);
		QObject::connect(this->okButton, SIGNAL(clicked()), this, SLOT(start_detailed_infodialog()));
	}
	else{
		this->setWindowTitle("Edit the measure info");
		this->just_editing=true;
		this->measure_data=data;
		ostringstream buffer;
		buffer << this->measure_data.id.measure_nr;
		this->lineEdit_alt_id->setText(buffer.str().c_str());
		this->lineEdit_alt_name->setText(data.name.c_str());
		this->textEdit_alt_description->setText(data.description.c_str());
		this->groupBox_alt_type->setEnabled(false);
		if(data.category==_alt_measure_category::category_dam){
			this->radioButton_dam->setChecked(true);
		}
		else if(data.category==_alt_measure_category::category_fpl){
			this->radioButton_fpl->setChecked(true);
		}
		else if(data.category==_alt_measure_category::category_hyd){
			this->radioButton_hyd->setChecked(true);
		}
		this->okButton->setText("Ok");
		QObject::connect(this->okButton, SIGNAL(clicked()), this, SLOT(accept()));

	}
}
//Start the dialog it returns true by acception, false by rejection
bool AltGui_Alternative_Info_Dia::start_dialog(QSqlDatabase *ptr_database, const _sys_system_id id){
	int decision =this->exec();
	 //rejected
	if(decision ==0){
		return false;
	}
	//accepted
	else{
		//set the data
		this->measure_data.name=this->lineEdit_alt_name->text().toStdString();
		this->measure_data.description=this->textEdit_alt_description->toPlainText().toStdString();


		if(this->radioButton_fpl->isChecked()==true){
			this->measure_data.category=_alt_measure_category::category_fpl;
		}
		else if(this->radioButton_hyd->isChecked()==true){
			this->measure_data.category=_alt_measure_category::category_hyd;
		}
		else if(this->radioButton_dam->isChecked()==true){
			this->measure_data.category=_alt_measure_category::category_dam;
		}
		if(this->just_editing==true){
			return true;
		}
		else{
			//start the measure-state base on dia if the measure type is "changing paramater of the fpl-sections"
			if(this->measure_data.detailed_type==alt_replace_fpl::section_param){
				AltGui_Alternative_Base_On_Dia my_dia_2;
			
				my_dia_2.set_dialog(ptr_database, id);

				if(my_dia_2.start_dialog()==false){
					return false;
				}
				else{
					this->measure_data.measurestate_based_on=my_dia_2.get_measurestate_base_on();
				}
			}
			//set the files for replacing and adding a measure
			//dam-resettlement measure
			if(this->measure_data.detailed_type==alt_replace_dam::dam_resettlement){
				AltGui_Dam_Resettlement_Dia my_dia_2;

				my_dia_2.set_dialog(ptr_database);

				if(my_dia_2.start_dialog()==false){
					return false;
				}
				else{
					my_dia_2.get_selected_information(&this->replacing_files, &this->replacing_ids);
					return true;
				}
				


			}
			//dam-change damage- function/categories measure
			else if(this->measure_data.detailed_type==alt_replace_dam::dam_ecn_function_polygon || this->measure_data.detailed_type==alt_replace_dam::dam_eco_btype_function_polygon 
				|| this->measure_data.detailed_type==alt_replace_dam::dam_eco_soil_function_polygon || this->measure_data.detailed_type==alt_replace_dam::dam_pop_categories_polygon ||
				this->measure_data.detailed_type==alt_replace_dam::dam_pys_categories_polygon){
				AltGui_Dam_Change_Function_Dia my_dia_2;

				if(my_dia_2.set_dialog(ptr_database, this->measure_data.detailed_type)==false){
					return this->start_dialog(ptr_database, id);
				}

				if(my_dia_2.start_dialog()==false){
					return false;
				}
				else{
					my_dia_2.get_selected_information(&this->replacing_files, &this->current_ids, &this->replacing_ids);
					return true;
				}


			}
			//other measures uses the same dialog
			else if(this->start_measures_filedialog(ptr_database)==true){
				return true;
			}
			else{
				return false;
			}

		}
	}

}
//Get the data of the measure
_alt_general_info AltGui_Alternative_Info_Dia::get_measure_general_data(void){
	return this->measure_data;
}
//______
//private slot
//Check the given name before
void AltGui_Alternative_Info_Dia::check_name(QString name){

	if(name.isEmpty()==true){
		okButton->setEnabled(false);
	}
	else{
		okButton->setEnabled(true);
	}

}
//Start the dialog for the detailed measure information
void AltGui_Alternative_Info_Dia::start_detailed_infodialog(void){

	this->measure_data.name=this->lineEdit_alt_name->text().toStdString();
	this->measure_data.description=this->textEdit_alt_description->toPlainText().toStdString();


	if(this->radioButton_fpl->isChecked()==true){
		this->measure_data.category=_alt_measure_category::category_fpl;
	}
	else if(this->radioButton_hyd->isChecked()==true){
		this->measure_data.category=_alt_measure_category::category_hyd;
	}
	else if(this->radioButton_dam->isChecked()==true){
		this->measure_data.category=_alt_measure_category::category_dam;
	}

	AltGui_Alternative_Type_Dia my_dia(this);
	my_dia.set_dialog(this->measure_data);

	bool reaction=my_dia.start_dialog();

	if(my_dia.get_back_button_is_clicked()==true){
		this->measure_data=my_dia.get_measure_general_data();
		return;
	}
	else if(reaction==true ){
		this->measure_data=my_dia.get_measure_general_data();

		this->accept();
	}
	else if(reaction==false){
		this->reject();
	}
}
//Start the dialog for the file information for measures
bool AltGui_Alternative_Info_Dia::start_measures_filedialog(QSqlDatabase *ptr_database){

	AltGui_Measure_Replacing_File_Dia my_dia(this);
	my_dia.set_ptr_database(ptr_database);
	
	my_dia.set_dialog(this->measure_data);

	this->replacing_files.clear();
	this->replacing_ids.clear();

	if(my_dia.start_dialog()==true){
		my_dia.get_selected_information(&this->replacing_files, &this->replacing_ids);
		return true;
	}
	else{
		return false;
	}


}
