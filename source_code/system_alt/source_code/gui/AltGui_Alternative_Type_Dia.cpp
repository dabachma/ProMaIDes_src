#include "Alt_Headers_Precompiled.h"
//#include "AltGui_Alternative_Type_Dia.h"


//Default constructor
AltGui_Alternative_Type_Dia::AltGui_Alternative_Type_Dia(QWidget *parent): QDialog(parent){

	this->setupUi(this);
	this->back_pressed=false;
	this->measure_data.name=label::not_set;
	this->measure_data.replacing=true;
	this->set_up_measure_string_list();


	QObject::connect(this->okButton, SIGNAL(clicked()), this, SLOT(accept()));
	QObject::connect(this->cancelButton, SIGNAL(clicked()), this, SLOT(reject()));
	QObject::connect(this->backButton, SIGNAL(clicked()), this, SLOT(back_button_is_clicked()));

	QObject::connect(this->radioButton_replace, SIGNAL(clicked()), this, SLOT(set_up_combo_box()));
	QObject::connect(this->radioButton_adding, SIGNAL(clicked()), this, SLOT(set_up_combo_box()));
	QObject::connect(this->radioButton_combi, SIGNAL(clicked()), this, SLOT(set_up_combo_box()));
	
	//count the memory
	Sys_Memory_Count::self()->add_mem(sizeof(AltGui_Alternative_Type_Dia), _sys_system_modules::ALT_SYS);
}
//Default destructor
AltGui_Alternative_Type_Dia::~AltGui_Alternative_Type_Dia(void){

	//count the memory
	Sys_Memory_Count::self()->minus_mem(sizeof(AltGui_Alternative_Type_Dia), _sys_system_modules::ALT_SYS);
}
//_______
//public
//Set the dialog measure identifier
void AltGui_Alternative_Type_Dia::set_dialog(const _alt_general_info data){
	
		this->measure_data=data;
		this->back_pressed=false;

		ostringstream buffer;
		buffer << this->measure_data.id.measure_nr;
		this->lineEdit_alt_id->setText(buffer.str().c_str());
		buffer.str("");
		buffer << this->measure_data.name;
		this->lineEdit_alt_name->setText(buffer.str().c_str());
		
		radioButton_fpl->setEnabled(false);
		radioButton_hyd->setEnabled(false);
		radioButton_dam->setEnabled(false);
		if(this->measure_data.category==_alt_measure_category::category_fpl){
			radioButton_fpl->setCheckable(true);
			radioButton_fpl->setChecked(true);
			radioButton_fpl->setEnabled(true);
		}
		else if(this->measure_data.category==_alt_measure_category::category_dam){
			radioButton_dam->setCheckable(true);
			radioButton_dam->setChecked(true);
			radioButton_dam->setEnabled(true);
		}
		else if(this->measure_data.category==_alt_measure_category::category_hyd){
			radioButton_hyd->setCheckable(true);
			radioButton_hyd->setChecked(true);
			radioButton_hyd->setEnabled(true);
		}

		if(this->measure_data.replacing==true){
			this->radioButton_replace->setChecked(true);
		}
		else if(this->measure_data.replacing==false){
			this->radioButton_adding->setChecked(true);
		}
	
		this->set_up_combo_box();

}
//Start the dialog it returns true by acception, false by rejection
bool AltGui_Alternative_Type_Dia::start_dialog(void){
	this->back_pressed=false;
	int decision =this->exec();
	 //rejected
	if(decision ==0){
		return false;
	}
	//accepted
	else{
		//set the data
		if(this->radioButton_replace->isChecked()==true){
			this->measure_data.replacing=true;
		}
		else if(this->radioButton_adding->isChecked()==true){
			this->measure_data.replacing=false;
		}
		int last_index=this->comboBox_type->currentIndex();
		this->measure_data.detailed_type=this->comboBox_type->itemText(last_index).toStdString();

		return true;
	}

}
//Get the data of the measure
_alt_general_info AltGui_Alternative_Type_Dia::get_measure_general_data(void){
	return this->measure_data;
}
//Get if the back-button is clicked
bool AltGui_Alternative_Type_Dia::get_back_button_is_clicked(void){
	return this->back_pressed;
}
//Check if the detailed measure type is valid (static)
bool AltGui_Alternative_Type_Dia::check_measure_type_valid(const string type){

	//hyd
	if(type==alt_replace_hyd::boundary_sc){
		return true;
	}
	else if(type==alt_replace_hyd::dike_lines){
		return true;
	}
	else if(type==alt_add_hyd::dike_lines){
		return true;
	}
	else if(type==alt_replace_hyd::river_profiles){
		return true;
	}
	//fpl
	else if(type==alt_combi_fpl::fpl_line){
		return true;
	}
	else if(type==alt_add_fpl::section){
		return true;
	}
	else if(type==alt_replace_fpl::section){
		return true;
	}
	else if(type==alt_replace_fpl::section_param){
		return true;
	}
	else if(type==alt_replace_dam::dam_resettlement){
		return true;
	}
	else if(type==alt_replace_dam::dam_ecn_function_polygon){
		return true;
	}
	else if(type==alt_replace_dam::dam_eco_btype_function_polygon){
		return true;
	}
	else if(type==alt_replace_dam::dam_eco_soil_function_polygon){
		return true;
	}
	else if(type==alt_replace_dam::dam_pop_categories_polygon){
		return true;
	}
	else if(type==alt_replace_dam::dam_pys_categories_polygon){
		return true;
	}
	else{
		return false;
	}
	//implement the other measures...
}
//_____
//private
//Set up measure string lists
void AltGui_Alternative_Type_Dia::set_up_measure_string_list(void){

	//replace hyd
	this->replace_hyd.append(alt_replace_hyd::boundary_sc.c_str());
	this->replace_hyd.append(alt_replace_hyd::dike_lines.c_str());
	this->replace_hyd.append(alt_replace_hyd::river_profiles.c_str());

	//add hyd
	this->add_hyd.append(alt_add_hyd::dike_lines.c_str());
	this->add_hyd.append(alt_add_hyd::river_profiles.c_str());

	//replace fpl
	this->replace_fpl.append(alt_replace_fpl::section.c_str());
	this->replace_fpl.append(alt_replace_fpl::section_param.c_str());

	//add fpl
	this->add_fpl.append(alt_add_fpl::section.c_str());

	//combi fpl
	this->combi_fpl.append(alt_combi_fpl::fpl_line.c_str());

	//replace dam
	
	this->replace_dam.append(alt_replace_dam::dam_ecn_function_polygon.c_str());
	this->replace_dam.append(alt_replace_dam::dam_eco_btype_function_polygon.c_str());
	this->replace_dam.append(alt_replace_dam::dam_eco_soil_function_polygon.c_str());
	this->replace_dam.append(alt_replace_dam::dam_pop_categories_polygon.c_str());
	this->replace_dam.append(alt_replace_dam::dam_pys_categories_polygon.c_str());
	this->replace_dam.append(alt_replace_dam::dam_resettlement.c_str());
	

	//add dam
	//this->add_dam.append(alt_add_dam::dam_ecn_function_polygon.c_str());

}
//______
//private slot
//Back-button is clicked
void AltGui_Alternative_Type_Dia::back_button_is_clicked(void){
	this->back_pressed=true;
	this->accept();
}
//Set up the combo-box
void AltGui_Alternative_Type_Dia::set_up_combo_box(void){
	this->comboBox_type->setEnabled(true);
	this->comboBox_type->clear();
	if(this->radioButton_fpl->isChecked()==true){

		if(this->radioButton_replace->isChecked()==true){
			this->comboBox_type->addItems(this->replace_fpl);
			
		}
		else if(this->radioButton_adding->isChecked()==true){
			this->comboBox_type->addItems(this->add_fpl);
		}
		else if(this->radioButton_combi->isChecked()==true){
			this->comboBox_type->addItems(this->combi_fpl);
		}
	}
	else if(this->radioButton_hyd->isChecked()==true){
		if(this->radioButton_replace->isChecked()==true){
			this->comboBox_type->addItems(this->replace_hyd);
		}
		else if(this->radioButton_adding->isChecked()==true){
			this->comboBox_type->addItems(this->add_hyd);
		}
		else if(this->radioButton_combi->isChecked()==true){
			this->comboBox_type->setEnabled(false);
		}

	}
	else if(this->radioButton_dam->isChecked()==true){
		if(this->radioButton_replace->isChecked()==true){
			this->comboBox_type->addItems(this->replace_dam);
			this->comboBox_type->insertSeparator(5);
		}
		else if(this->radioButton_adding->isChecked()==true){
			this->comboBox_type->addItems(this->add_dam);
		}
		else if(this->radioButton_combi->isChecked()==true){
			this->comboBox_type->setEnabled(false);
		}

	}
}
