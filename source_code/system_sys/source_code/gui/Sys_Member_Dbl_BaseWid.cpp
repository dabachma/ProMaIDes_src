//#include "Sys_Member_Dbl_BaseWid.h"
#include "Sys_Headers_Precompiled.h"

//Default Constructor
Sys_Member_Dbl_BaseWid::Sys_Member_Dbl_BaseWid(QWidget *parent) : QWidget(parent), inc_step(5) {
	//Qt Stuff
	this->setupUi(this);

	this->context_menu = new QMenu(this);
	this->prec_icon = QIcon(":hydro/Preferences");
	this->prec_action = new QAction(this->prec_icon, tr("Change precision"), this->context_menu);
	this->context_menu->addAction(this->prec_action);

	//SELF
	this->set_editable(false);
	this->spinBox->setDecimals(4);

	//count the memory
	Sys_Memory_Count::self()->add_mem((sizeof(Sys_Member_Dbl_BaseWid)), _sys_system_modules::SYS_SYS);
}
//Default Destructor
Sys_Member_Dbl_BaseWid::~Sys_Member_Dbl_BaseWid(void) {
	Sys_Memory_Count::self()->minus_mem(sizeof(Sys_Member_Dbl_BaseWid), _sys_system_modules::SYS_SYS);
}
//____________
//public
//Changes the text of the QLabel
void Sys_Member_Dbl_BaseWid::set_label_text(const string text) {
	QString qText = text.c_str();
	this->label->setText(qText);
}

//Changes tooltip text
void Sys_Member_Dbl_BaseWid::set_tooltip(const string tooltip) {
	QString qTooltip = tooltip.c_str();
	this->label->setToolTip(qTooltip);
}

//Set the range of the QSpinBox
void Sys_Member_Dbl_BaseWid::set_range(const double min, const double max) {
	this->spinBox->setRange(min, max);
	
	double buffer=(max-min)/10.0;
	this->set_increment(buffer);
}

//Set the Increment of the QSpinBox
void Sys_Member_Dbl_BaseWid::set_increment(const double range) {
	//double inc = range * (this->inc_step / 100.0);

	//if (inc < 1) {
	//	inc = 1;
	//}
	this->spinBox->setSingleStep(range);
}

//Set the QSpinBox value
void Sys_Member_Dbl_BaseWid::set_value(const double val) {
	this->spinBox->setValue(val);
}

//Get the QSpinBox value
double Sys_Member_Dbl_BaseWid::get_value(void) {
	return this->spinBox->value();
}

//Change editable status
void Sys_Member_Dbl_BaseWid::set_editable(bool edit) {
	this->spinBox->setReadOnly(!edit);
}

//Returns editable status
bool Sys_Member_Dbl_BaseWid::is_editable(void) {
	return this->spinBox->isReadOnly();
}

//Sets the Suffix of the QSpinBox
void Sys_Member_Dbl_BaseWid::set_suffix(string suffix) {
	QString qText = suffix.c_str();
	this->spinBox->setSuffix(qText);
}

//Show the Custom PopUp Menus
void Sys_Member_Dbl_BaseWid::contextMenu_request(QPoint point) {
	QPoint trans;
	trans = this->mapToGlobal(point);
	this->context_menu->popup(trans);
}

//Show Precision Dialog
void Sys_Member_Dbl_BaseWid::precDiaRequest(void) {
	emit showPrecDia(this->spinBox->decimals());
}

//Execute and Show Precision Dialog
void Sys_Member_Dbl_BaseWid::execPrecDia(int prec) {
	Sys_Member_Dbl_Prec_BaseWid prec_dia(prec, this);
	QObject::connect(&prec_dia, SIGNAL(precApplied(int)), this, SLOT(set_precision(int)));
	prec_dia.exec();
	QObject::disconnect(&prec_dia, SIGNAL(precApplied(int)), this, SLOT(set_precision(int)));
}

//Set Precision of the base SpinBox
void Sys_Member_Dbl_BaseWid::set_precision(int prec) {
	this->spinBox->setDecimals(prec);
}

//Change the current datarole | edit or display
void Sys_Member_Dbl_BaseWid::set_dataRole(DataRole role) {
	switch(role) {
		case 0: //DisplayRole
			//Disconnect Signals from Slots
			this->setContextMenuPolicy(Qt::DefaultContextMenu);
			QObject::disconnect(this, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(contextMenu_request(QPoint)));
			QObject::disconnect(this->prec_action, SIGNAL(triggered()), this, SLOT(precDiaRequest()));
			QObject::disconnect(this, SIGNAL(showPrecDia(int)), this, SLOT(execPrecDia(int)));
			break;
		case 1: //EditRole
			//Connect Signals To Slots
			this->setContextMenuPolicy(Qt::CustomContextMenu);
			QObject::connect(this, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(contextMenu_request(QPoint)));
			QObject::connect(this->prec_action, SIGNAL(triggered()), this, SLOT(precDiaRequest()));
			QObject::connect(this, SIGNAL(showPrecDia(int)), this, SLOT(execPrecDia(int)));
	}
}
