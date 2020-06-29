//#include "Sys_Member_ScientificDbl_BaseWid.h"
#include "Sys_Headers_Precompiled.h"

//Default Constructor
Sys_Member_ScientificDbl_BaseWid::Sys_Member_ScientificDbl_BaseWid(QWidget *parent) : QWidget(parent), inc_step(5) {
	//Qt Stuff
	this->setupUi(this);

	//CONTEXT MENU AREA
	this->context_menu = new QMenu();

	//-------- Change Precision
	this->prec_icon = QIcon(":/hydro/Preferences");
	this->prec_action = new QAction(this->prec_icon, "Change Precision", this->context_menu);
	this->context_menu->addAction(this->prec_action);

	//SELF
	this->set_editable(false);
	this->base_spinBox->setDecimals(2);
	this->base_spinBox->setRange(1.0, 9.99);
	
	//count the memory
	Sys_Memory_Count::self()->add_mem((sizeof(Sys_Member_ScientificDbl_BaseWid)), _sys_system_modules::SYS_SYS);
}
//Default Destructor
Sys_Member_ScientificDbl_BaseWid::~Sys_Member_ScientificDbl_BaseWid(void) {
	Sys_Memory_Count::self()->minus_mem(sizeof(Sys_Member_ScientificDbl_BaseWid), _sys_system_modules::SYS_SYS);
}

//____________
//public

//Changes the text of the QLabel
void Sys_Member_ScientificDbl_BaseWid::set_label_text(const string text) {
	QString qText = text.c_str();
	this->label->setText(qText);
}

//Changes tooltip text
void Sys_Member_ScientificDbl_BaseWid::set_tooltip(const string tooltip) {
	QString qTooltip = tooltip.c_str();
	this->label->setToolTip(qTooltip);
}

//Set the range of the QSpinBox
void Sys_Member_ScientificDbl_BaseWid::set_range(const double min, const double max) {
	this->exp_spinBox->setRange(min, max);
	this->set_increment(max-min);
}

//Set the Increment of the QSpinBox
void Sys_Member_ScientificDbl_BaseWid::set_increment(const double range) {
	double inc = range * (this->inc_step / 100.0);

	if (inc < 1) {
		inc = 1;
	}
	this->base_spinBox->setSingleStep(inc);
}

//Set the QSpinBox value
void Sys_Member_ScientificDbl_BaseWid::set_value(double val) {
	int exp = 0;
	if (val < 1 && val > 0) {
		while (val < 1) {
			val = val * 10.0;
			exp++;
		}
		this->base_spinBox->setValue(val);
		this->exp_spinBox->setValue(-exp);
	}
	else if (val > 1) {
		while (val > 10) {
			val = val / 10.0;
			exp++;
		}
		this->base_spinBox->setValue(val);
		this->exp_spinBox->setValue(exp);
	}
	else if (val > -1 && val < 0) {
		while (val > -1) {
			val = val * 10.0;
			exp++;
		}
		this->base_spinBox->setValue(val);
		this->exp_spinBox->setValue(-exp);
	}
	else if (val < -1) {
		while (val < -10) {
			val = val / 10.0;
			exp++;
		}
		this->base_spinBox->setValue(val);
		this->exp_spinBox->setValue(exp);
	}
}

//Get the QSpinBox value
double Sys_Member_ScientificDbl_BaseWid::get_value(void) {
	return this->base_spinBox->value() * pow(10.0, this->exp_spinBox->value());
}

//Change editable status
void Sys_Member_ScientificDbl_BaseWid::set_editable(bool edit) {
	this->base_spinBox->setReadOnly(!edit);
	this->exp_spinBox->setReadOnly(!edit);
}

//Returns editable status
bool Sys_Member_ScientificDbl_BaseWid::is_editable(void) {
	if (this->base_spinBox->isReadOnly() && this->exp_spinBox->isReadOnly()) {
		return false;
	}
	else return true;
}

//Sets the Suffix of the QSpinBox
void Sys_Member_ScientificDbl_BaseWid::set_suffix(string suffix) {
	QString qText = suffix.c_str();
	this->base_spinBox->setSuffix(qText);
}

//Show the Custom PopUp Menus
void Sys_Member_ScientificDbl_BaseWid::context_menu_request(QPoint point) {
	QPoint trans;
	trans = this->mapToGlobal(point);
	this->context_menu->popup(trans);
}

//Show Precision Dialog
void Sys_Member_ScientificDbl_BaseWid::precDiaRequest(bool show) {
	show = true;
	emit showPrecDia(this->base_spinBox->decimals());
}

//Execute and Show Precision Dialog
void Sys_Member_ScientificDbl_BaseWid::execPrecDia(int prec) {
	Sys_Member_Dbl_Prec_BaseWid prec_dia(prec, this);
	QObject::connect(&prec_dia, SIGNAL(precApplied(int)), this, SLOT(set_precision(int)));
	prec_dia.exec();
	QObject::disconnect(&prec_dia, SIGNAL(precApplied(int)), this, SLOT(set_precision(int)));
}

//Set Precision of the base SpinBox
void Sys_Member_ScientificDbl_BaseWid::set_precision(int prec) {
	this->base_spinBox->setDecimals(prec);
}

void Sys_Member_ScientificDbl_BaseWid::set_dataRole(DataRole role) {
	switch(role) {
		case 0: //DisplayRole
			//Disconnect Signals from Slots
			this->setContextMenuPolicy(Qt::DefaultContextMenu);
			QObject::disconnect(this->prec_action, SIGNAL(triggered(bool)), this, SLOT(precDiaRequest(bool)));
			QObject::disconnect(this, SIGNAL(showPrecDia(int)), this, SLOT(execPrecDia(int)));
			QObject::disconnect(this, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(context_menu_request(QPoint)));
			break;
		case 1: //EditRole
			//Connect Signals To Slots
			this->setContextMenuPolicy(Qt::CustomContextMenu);
			QObject::connect(this->prec_action, SIGNAL(triggered(bool)), this, SLOT(precDiaRequest(bool)));
			QObject::connect(this, SIGNAL(showPrecDia(int)), this, SLOT(execPrecDia(int)));
			QObject::connect(this, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(context_menu_request(QPoint)));
	}
}
