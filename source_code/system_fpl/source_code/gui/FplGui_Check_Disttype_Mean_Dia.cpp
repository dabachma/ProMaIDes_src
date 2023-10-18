#include "Fpl_Headers_Precompiled.h"
//#include "FplGui_Check_Disttype_Mean_Dia.h"

//Default constructor
FplGui_Check_Disttype_Mean_Dia::FplGui_Check_Disttype_Mean_Dia(QWidget *parent): QDialog(parent){

	this->setupUi(this);

	this->doubleSpinBox_std_dev->setMinimum(0.0);
	this->doubleSpinBox_std_dev->setDecimals(9);
	this->doubleSpinBox_mean->setDecimals(9);
	this->doubleSpinBox_constant->setDecimals(3);

	this->comboBox_mean_type->addItem(fpl_label::dist_mean_determ.c_str());
	this->comboBox_mean_type->addItem(fpl_label::dist_mean_normal.c_str());
	this->comboBox_mean_type->addItem(fpl_label::dist_mean_log_normal.c_str());
	this->comboBox_mean_type->addItem(fpl_label::dist_mean_uniform.c_str());
	this->comboBox_mean_type->addItem(fpl_label::dist_mean_exponentiel.c_str());
	this->comboBox_mean_type->addItem(fpl_label::dist_mean_gumbel.c_str());
	this->comboBox_mean_type->addItem(fpl_label::dist_mean_weibull.c_str());

	QObject::connect(this->okButton, SIGNAL(clicked()), this, SLOT(accept()));
	QObject::connect(this->cancelButton, SIGNAL(clicked()), this, SLOT(reject()));
	QObject::connect(this->comboBox_mean_type, SIGNAL(currentIndexChanged(int )), this, SLOT(change_dialog_by_disttype(int )));

	//count the memory
	Sys_Memory_Count::self()->add_mem(sizeof(FplGui_Check_Disttype_Mean_Dia), _sys_system_modules::FPL_SYS);
}
//Default destructor
FplGui_Check_Disttype_Mean_Dia::~FplGui_Check_Disttype_Mean_Dia(void){

	//count the memory
	Sys_Memory_Count::self()->minus_mem(sizeof(FplGui_Check_Disttype_Mean_Dia), _sys_system_modules::FPL_SYS);
}
//__________
//public
//Start the dialog; it returns true by acception, false by rejection
bool FplGui_Check_Disttype_Mean_Dia::start_dialog(void){
	int decision =this->exec();
	 //rejected
	if(decision ==0){
		return false;
	}
	//accepted
	else{
		return true;
	}
}
//Get the mean value
double FplGui_Check_Disttype_Mean_Dia::get_mean(void){
	return this->doubleSpinBox_mean->value();
}
//Get the standard deviation
double FplGui_Check_Disttype_Mean_Dia::get_std_dev(void){
	return this->doubleSpinBox_std_dev->value();
}
//Get the constant
double FplGui_Check_Disttype_Mean_Dia::get_constant(void){
	return this->doubleSpinBox_constant->value();
}
//Get mean distribution type
_fpl_mean_distribution_type FplGui_Check_Disttype_Mean_Dia::get_mean_dist_type(void){
	string buffer;
	buffer=this->comboBox_mean_type->currentText().toStdString();

	return Fpl_Distribution_Class_Mean::convert_txt2meandist_type(buffer);
}
//__________
//private slots
//Check the distribution type and change dialog dynamically
void FplGui_Check_Disttype_Mean_Dia::change_dialog_by_disttype(const int){

	this->doubleSpinBox_std_dev->setMinimum(0.0);
	this->doubleSpinBox_mean->setMinimum(-1.0e30);

	if(this->get_mean_dist_type()==_fpl_mean_distribution_type::log_normal_type){
		this->doubleSpinBox_mean->setMinimum(0.0);
	}

	if(this->get_mean_dist_type()==_fpl_mean_distribution_type::weibull_type){
		this->label_mean->setText("Alpha");
		this->doubleSpinBox_mean->setToolTip("Alpha value of the weibull distribution.");
		this->label_std_dev->setText("Beta");
		this->doubleSpinBox_std_dev->setToolTip("Beta value of the weibull distribution.");
		this->doubleSpinBox_std_dev->setMinimum(0.0000000000000001);
		this->doubleSpinBox_mean->setMinimum(0.00000000000000001);
	}
	else{
		this->label_mean->setText("Mean Value");
		this->doubleSpinBox_mean->setToolTip("Mean value of the distribution.");
		this->label_std_dev->setText("Standard deviation");
		this->doubleSpinBox_std_dev->setToolTip("The standard deviation of the distribution. It can not be negative.");
	}
}