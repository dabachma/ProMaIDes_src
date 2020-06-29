//#include "_Sys_Abstract_Base_Wid.h"
#include "Sys_Headers_Precompiled.h"
#include <QScrollBar>

//Default constructor
_Sys_Abstract_Base_Wid::_Sys_Abstract_Base_Wid(QWidget *parent): _Sys_Data_Wid(parent) {
	//Qt stuff
	this->setupUi(this);
	
	this->ptr_database=NULL;

	this->edit_icon = QIcon(":hydro/Preferences");
	this->edit_action = new QAction(this->edit_icon, "Edit", &this->context_menu);
	this->context_menu.addAction(this->edit_action);
	this->context_menu.insertSeparator(this->edit_action);

	this->set_spinBox->setKeyboardTracking(false);
	
	QObject::connect(this->edit_action, SIGNAL(triggered()), this, SLOT(show_as_dialog()));
}
//Default destructor
_Sys_Abstract_Base_Wid::~_Sys_Abstract_Base_Wid(void) {

}
//__________
//public
//Set the possibility to edit the data to the given state
void _Sys_Abstract_Base_Wid::set_edit_action_disabled(const bool state) {
	this->edit_action->setDisabled(state);
}
//Common getter for editable state
bool _Sys_Abstract_Base_Wid::is_editable(void) {
	return this->editable;
}
//Set the spinbox range in the head widget
void _Sys_Abstract_Base_Wid::set_head_spinBox_range(const int max, const int min) {
	if (max > 0) {
		this->set_spinBox->setEnabled(true);
		this->set_spinBox->setRange(min, max);
		QString qtext;
		QString qnum;
		qtext = "(" + qnum.setNum(max) + ")";
		this->behind_spinBox_label->setText(qtext);
	}
	else {
		this->set_spinBox->setEnabled(false);
	}
}
//Set the head spin box value
void _Sys_Abstract_Base_Wid::set_head_spinBox_value(const int value){
	this->set_spinBox->setValue(value);
	QObject::connect(this->set_spinBox, SIGNAL(valueChanged(int )), this, SLOT(recieve_head_spin_box_changed(int )), Qt::QueuedConnection);

}
//Get a pointer to the head spin box
QSpinBox* _Sys_Abstract_Base_Wid::get_ptr_head_spinbox(void){
	return this->set_spinBox;
}
//Set the spinbox text in the head widget
void _Sys_Abstract_Base_Wid::set_head_spinBox_text(const string text) {
	QString qtext = text.c_str();
	this->spinBox_label->setText(qtext);
}
//this method sets the text of the big label on top of the widget
void _Sys_Abstract_Base_Wid::set_headLabel_text(const string title) {
	QString qtext = title.c_str();
	this->head_label->setText(qtext);
}
//this method sets the icon on top of the widget (left)
void _Sys_Abstract_Base_Wid::set_headPixmap(const QPixmap pic) {
	this->head_pixmap->setPixmap(pic);
}
//Set the child widget
void _Sys_Abstract_Base_Wid::set_child(QWidget *child) {
	this->mainLayout->addWidget(child);
}
//Set current scroll bars position (vertical, horizontal)
void _Sys_Abstract_Base_Wid::set_current_scroll_bar(const int ver_pos, const int hor_pos){
	if(this->scroll_area!=NULL){
        this->scroll_area->verticalScrollBar()->setSliderPosition(ver_pos);
		this->scroll_area->horizontalScrollBar()->setSliderPosition(hor_pos);
	}
}
//_____________
//private slots
//Recieve if the head spin box value is changed
void _Sys_Abstract_Base_Wid::recieve_head_spin_box_changed(int index){

	if(this->scroll_area==NULL){
		emit send_change_widget(index, this->id_item, 0, 0);
	}
	else{
		emit send_change_widget(index, this->id_item, this->scroll_area->verticalScrollBar()->sliderPosition(), this->scroll_area->horizontalScrollBar()->sliderPosition());
		
	}

}
