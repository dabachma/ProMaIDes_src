//#include "Sys_Base_Expand_Wid.h"
#include "Sys_Headers_Precompiled.h"

//Default constructor
Sys_Base_Expand_Wid::Sys_Base_Expand_Wid(QWidget *parent) : QWidget(parent) {
	//Qt stuff
	this->setupUi(this);
	this->setupRest();

	this->head_pixmap->hide();

	//count the memory
	Sys_Memory_Count::self()->add_mem(sizeof(Sys_Base_Expand_Wid), _sys_system_modules::SYS_SYS);
}

//Default destructor
Sys_Base_Expand_Wid::~Sys_Base_Expand_Wid(void) {
	Sys_Memory_Count::self()->minus_mem(sizeof(Sys_Base_Expand_Wid), _sys_system_modules::SYS_SYS);
}

//______________
//public methods

//Sets the text of the label in the head_layout 
void Sys_Base_Expand_Wid::set_title(const string text) {
	QString qText = text.c_str();
	this->title->setText(qText);
}

//Sets the text of the label in the head_layout 
string Sys_Base_Expand_Wid::get_title(void) {
	return this->title->text().toStdString();
}

//Adds a child widget to the container layout
void Sys_Base_Expand_Wid::add_child(QWidget *child) {
    this->container_layout->addWidget(child);
	child->setParent(this->container_widget);
}
//Adds a child layout to the container layout
//void Sys_Base_Expand_Wid::add_child(QLayout *child) {
//    this->container_layout->addLayout(child);
//    child->setParent(this->container_widget);
//}

//Removes the child widget
void Sys_Base_Expand_Wid::remove_child(QWidget *child) {
	this->container_layout->removeWidget(child);
}

//Sets the frame inside the container widget visible or not visible
void Sys_Base_Expand_Wid::set_frame_visible(const bool state) {
	this->frame->setVisible(state);
}

//Sets the head widget visible or not visible
void Sys_Base_Expand_Wid::set_head_widget_visible(const bool state) {
	this->head_widget->setVisible(state);
}

//Sets the head widgets icon
void Sys_Base_Expand_Wid::set_head_pixmap(const QPixmap pic) {
	this->head_pixmap->setPixmap(pic);
	this->head_pixmap->show();
}

//____________
//public slots

//Sets the container widget visible
void Sys_Base_Expand_Wid::set_expanded(const bool state) {
	this->expand_button->setChecked(state);
	this->container_widget->setVisible(state);
}

//Sets the container widget editable
void Sys_Base_Expand_Wid::set_editable(const bool state) {
	this->container_widget->setEnabled(state);
}

//_______________
//private methods

//Sets up class members and atrributes
void Sys_Base_Expand_Wid::setupRest(void) {
	this->container_widget->setVisible(false);
	QIcon expand_icon;
    expand_icon.addPixmap(style()->standardPixmap(QStyle::SP_ArrowDown), QIcon::Normal, QIcon::Off);
    expand_icon.addPixmap(style()->standardPixmap(QStyle::SP_ArrowUp), QIcon::Normal, QIcon::On);

    //expand_icon.addPixmap(style()->standardPixmap(QStyle::SC_TitleBarMinButton), QIcon::Normal, QIcon::Off);
    //expand_icon.addPixmap(style()->standardPixmap(QStyle::SC_TitleBarMaxButton), QIcon::Normal, QIcon::On);


	this->expand_button->setIcon(expand_icon);
}
