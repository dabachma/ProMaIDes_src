//#include "Fpl_Section_Dia.h"
#include "Fpl_Headers_Precompiled.h"

//constructor
Fpl_Section_Dia::Fpl_Section_Dia(void){

	fpl_section_dia.setupUi(this);
	Memory_Count::self()->add_mem(sizeof(Fpl_Section_Dia));//count the memory
}

//destructor
Fpl_Section_Dia::~Fpl_Section_Dia(void){
	Memory_Count::self()->minus_mem((sizeof(Fpl_Section_Dia)));//count the memory
}
//___________________________________________
//public
//make the combo values
void Fpl_Section_Dia::sectionnumber_to_combo(const string section_type_name, Fpl_Data_Section *section, const int text_number){
	int number=0;
	number=section->calc_quant_spec_section(section_type_name);
	fpl_section_dia.comboBox->clear();
	for(int i=0; i<number; i++){
		ostringstream numbers;
		numbers << section->number_specif_sectiontype[i];
		fpl_section_dia.comboBox->addItem(numbers.str().c_str());
	}
	//set the text of the dia
	this->set_dia_text(text_number, section_type_name);
	
}
//get the section number from the combo dialog
int Fpl_Section_Dia::get_section_number(void){
	int decision =exec();
	if(decision ==0){
		return 0; //rejected
	}
	else if(decision!=0){//accepted
		return fpl_section_dia.comboBox->currentText().toInt(); //accepted
	}
	else{
		return 0;
	}
}
//_____________________________________________
//private
//choose which actiontext hast to be set
void Fpl_Section_Dia::set_dia_text(int const text_number, const string section_type_name){
	ostringstream question;
	switch(text_number){
		case 0: question << " You choose a deterministic analysis for one section!"<< endl;
				break;
		case 1: question << " You choose a MC analysis for one section!"<< endl;
				break;
		case 2: question << " You choose a FRC analysis for one section!"<< endl;
				break;
		case 3: question << " You want to see the FRC-result in table and graph for one section!"<< endl;
				break;
		case 4: question << " You want to see the input parameter for one section! "<< endl;
				break;
		case 5: question << " You want to see the FRC by hand for one section! "<< endl;
				break;
	}
	question << endl;
	question << " For which section?" << endl;
	question << endl;
	question<<" Choose a section from the combo box " << endl;
	question<<" The selection is from all section which have a section type : " << section_type_name; 
	fpl_section_dia.text_label->clear();
	fpl_section_dia.text_label->setText(question.str().c_str());
}