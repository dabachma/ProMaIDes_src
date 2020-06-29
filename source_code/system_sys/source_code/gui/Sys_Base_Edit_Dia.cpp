//#include "Sys_Base_Edit_Dia.h"
#include "Sys_Headers_Precompiled.h"

//Default constructor
Sys_Base_Edit_Dia::Sys_Base_Edit_Dia(_Sys_Abstract_Base_Wid *child, QWidget *parent) : QDialog(parent) {
	this->setupUi(this);

	if (child != NULL) {
		this->mainLayout->addWidget(child);
		this->resize(880, 700);
	}

	QObject::connect(this->cancelButton, SIGNAL(clicked()), this, SLOT(reject()));
	QObject::connect(this->okButton, SIGNAL(clicked()), this, SLOT(accept()));

	this->child = child;
	//count the memory
	Sys_Memory_Count::self()->add_mem(sizeof(Sys_Base_Edit_Dia), _sys_system_modules::SYS_SYS);
}

//Default destructor
Sys_Base_Edit_Dia::~Sys_Base_Edit_Dia(void) {
	Sys_Memory_Count::self()->minus_mem(sizeof(Sys_Base_Edit_Dia), _sys_system_modules::SYS_SYS);
}
//___________
//public
//Start the dialog
bool Sys_Base_Edit_Dia::start_dialog(void){
	this->resize(780, 600);
	int decision = this->exec();
	//rejected
	if(decision == 0){
		return false;
	}
	//accepted; 
	else{
		return true;
	}
}
//____________
//public slots
//Show the given Base_Wid as Dialog to make editing data possible
void Sys_Base_Edit_Dia::add_child(_Sys_Abstract_Base_Wid *wid) {
	this->mainLayout->addWidget(wid);
}

////Close the dialog when the cancel button has been pressed
//void Sys_Base_Edit_Dia::cancel_pressed(void) {
//	this->done(0);
//}
//
////Listen for events that make the apply button enabled
//void Sys_Base_Edit_Dia::set_applyButton_enable(QEvent *key_event) {
//	if (key_event->type() == QEvent::KeyPress) {
//		this->applyButton->setEnabled(true);
//	}
//}
//_____________
//private slots

////Close the dialog and assume the changed parameters
//void Sys_Base_Edit_Dia::ok_pressed(void) {
//	this->done(0);
//	emit dialog_applied(this->child); 
//}
