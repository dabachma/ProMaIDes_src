//#include "HydGui_Diversion_Channel_Wid.h"
#include "Hyd_Headers_Precompiled.h"

//Default constructor
HydGui_Diversion_Channel_Wid::HydGui_Diversion_Channel_Wid(DataRole role, QWidget *parent) : _Sys_Abstract_Base_Wid(parent) {
	// Qt Stuff
	QWidget *widget = new QWidget(parent);
	ui.setupUi(widget);
	this->set_child(widget);

	//number
	ui.number->set_label_text("River model number [-]");
	ui.number->set_tooltip("River model number, which represents the diversion channel");
	//name
	ui.name->set_label_text("River model name");
	ui.name->set_tooltip("");
	//inflow_number
	ui.inflow_number->set_label_text("Inflow river number [-]");
	ui.inflow_number->set_tooltip("River model number for the inflow to the diversion channel");
	//inflow_name
	ui.inflow_name->set_label_text("Inflow river name");
	ui.inflow_name->set_tooltip("");
	//inflow_type
	ui.inflow_type->set_label_text("Inflow type [-]");
	ui.inflow_type->set_tooltip("Type of the hydraulic structure ('WEIR' or 'GATE' is possible)");
	string types[] = {"WEIR", "GATE"};
	ui.inflow_type->set_items(types, 2);
	//widht
	ui.coupling_width->set_label_text("Sill width [m]");
	ui.coupling_width->set_tooltip("Sill-width of the coupling structure [m]");
	//controlled
	ui.controlled->set_label_text("Controlled structure");
	ui.controlled->set_tooltip("Controlled or uncontrolled structure (true:= controlled [is not yet implemented]; false:= uncontrolled)");
	//controllable_param
	ui.controllable_param->set_label_text("Local sill height [m]");
	ui.controllable_param->set_tooltip("WEIR type: the local sill height [m]");
	//uncontrollable_param
	ui.uncontrollable_param->set_label_text("Poleni factor [-]");
	ui.uncontrollable_param->set_tooltip("WEIR type: the Poleni Factor [-]");
	ui.uncontrollable_param->spinBox->setDecimals(0);
	//ouflow_number
	ui.outflow_number->set_label_text("Outflow river number");
	ui.outflow_number->set_tooltip("River model number for the outflow of the diversion channel");
	//outflow_name
	ui.outflow_name->set_label_text("Outflow river name");
	ui.outflow_name->set_tooltip("");

	//SELF
	switch(role) {
		case 0: //DisplayRole
			this->set_editable(false);
			this->head_pixmap->setPixmap(style()->standardPixmap(QStyle::SP_FileDialogInfoView));
			break;
		case 1: //EditRole
			this->set_editable(true);
			this->headWidget->hide();
			QObject::disconnect(this, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(context_menu_request(QPoint)));
			QObject::connect(ui.inflow_type->comboBox, SIGNAL(currentIndexChanged(QString)), this, SLOT(type_changed(QString)));
	}

	this->prev_prec = 4;
	this->set_default_values();
	//count the memory
	Sys_Memory_Count::self()->add_mem(sizeof(HydGui_Diversion_Channel_Wid), _sys_system_modules::HYD_SYS);
}

//Default destructor
HydGui_Diversion_Channel_Wid::~HydGui_Diversion_Channel_Wid(void) {
	Sys_Memory_Count::self()->minus_mem(sizeof(HydGui_Diversion_Channel_Wid), _sys_system_modules::HYD_SYS);
}

//______________
//public methods

//method to set the whole widget editable
void HydGui_Diversion_Channel_Wid::set_editable(bool state) {
	this->editable = state;
	//general
	ui.number->set_editable(state);
	ui.name->set_editable(state);
	//inflow
	ui.inflow_number->set_editable(state);
	ui.inflow_name->set_editable(state);
	ui.inflow_type->set_editable(state);
	//coupling structure
	ui.coupling_width->set_editable(state);
	ui.controlled->set_editable(state);
	ui.controllable_param->set_editable(state);
	ui.uncontrollable_param->set_editable(state);
	//outflow model
	ui.outflow_number->set_editable(state);
	ui.outflow_name->set_editable(state);
}

//Set all members of the widget, similar to a copy constructor
void HydGui_Diversion_Channel_Wid::set_member(_Sys_Abstract_Base_Wid *ptr) {
	//upcast to derived object
	HydGui_Diversion_Channel_Wid *other = (HydGui_Diversion_Channel_Wid *) ptr;
	if (other == NULL) {
		std::cerr << "reinterpret_cast error: could not cast object\n";
		return;
	}
	//general parameters
	ui.number->set_value(other->ui.number->get_value());
	ui.name->set_text(other->ui.name->get_text());
	//inflow parameters
	ui.inflow_number->set_value(other->ui.inflow_number->get_value());
	ui.inflow_name->set_text(other->ui.inflow_name->get_text());
	ui.inflow_type->set_current_value(other->ui.inflow_type->get_current_index());
	//coupling structure parameters
	ui.coupling_width->set_value(other->ui.coupling_width->get_value());
	ui.controlled->set_value(other->ui.controlled->get_value());
	ui.controllable_param->set_value(other->ui.controllable_param->get_value());
	ui.uncontrollable_param->set_value(other->ui.uncontrollable_param->get_value());
	//outflow model parameters
	ui.outflow_number->set_value(other->ui.outflow_number->get_value());
	ui.outflow_name->set_text(other->ui.outflow_name->get_text());
}

//Set default values
void HydGui_Diversion_Channel_Wid::set_default_values(void) {
	//head widget
	this->set_head_spinBox_range(0);
	this->set_head_spinBox_text("Set Nr.:");
	//general parameters
	ui.number->set_value(0);
	ui.name->set_text("-");
	//inflow parameters
	ui.inflow_number->set_value(0);
	ui.inflow_name->set_text("-");
	ui.inflow_type->set_current_value(0);
	//coupling structure parameters
	ui.coupling_width->set_value(0);
	ui.controlled->set_value(false);
	ui.controllable_param->set_value(0);
	ui.uncontrollable_param->set_value(0);
	//outflow model parameters
	ui.outflow_number->set_value(0);
	ui.outflow_name->set_text("-");
}

//____________
//public slots

//Open the dialog to edit values in the widget
void HydGui_Diversion_Channel_Wid::show_as_dialog(void) {
	Sys_Base_Edit_Dia dialog(NULL, this);
	HydGui_Diversion_Channel_Wid inDiaWid(DataRole::EditRole, &dialog);
	inDiaWid.set_member(this);
	dialog.add_child(&inDiaWid);
	dialog.setWindowTitle(this->head_label->text());
	QIcon icon;
	icon.addPixmap(*this->head_pixmap->pixmap());
	dialog.setWindowIcon(icon);

	QObject::connect(&dialog, SIGNAL(dialog_applied(_Sys_Abstract_Base_Wid*)), this, SLOT(set_member(_Sys_Abstract_Base_Wid*)));
	dialog.exec();
	QObject::disconnect(&dialog, SIGNAL(dialog_applied(_Sys_Abstract_Base_Wid*)), this, SLOT(set_member(_Sys_Abstract_Base_Wid*)));
}

//_____________
//private slots

//Change the label text and tooltip for controllable- and uncontrollable param
void HydGui_Diversion_Channel_Wid::type_changed(QString item) {
	if (item == "WEIR") {
		ui.controllable_param->set_label_text("Local sill height [m]");
		ui.controllable_param->set_tooltip("WEIR type: the local sill height [m]");
		ui.uncontrollable_param->set_label_text("Poleni factor [-]");
		ui.uncontrollable_param->set_tooltip("WEIR type: the Poleni Factor [-]");
		ui.uncontrollable_param->spinBox->setDecimals(0);
	}
	if (item == "GATE") {
		ui.controllable_param->set_label_text("Local opening height [m]");
		ui.controllable_param->set_tooltip("GATE type: the local opening height [m]");
		ui.uncontrollable_param->set_label_text("Local sill height [m]");
		ui.uncontrollable_param->set_tooltip("GATE type: the local sill height [m]");
		ui.uncontrollable_param->spinBox->setDecimals(this->prev_prec);
	}
}

//Save the precision of the uncontrollable_param widget`s spinbox
void HydGui_Diversion_Channel_Wid::precision_changed(const int prec) {
	this->prev_prec = prec;
}
