//#include "HydGui_Coupling_Structure_Wid.h"
#include "Hyd_Headers_Precompiled.h"

//Default constructor
HydGui_Coupling_Structure_Wid::HydGui_Coupling_Structure_Wid(DataRole role, QWidget *parent) : _Sys_Abstract_Base_Wid(parent) {
	// Qt Stuff
	QWidget *widget = new QWidget(parent);
	ui.setupUi(widget);
	this->set_child(widget);

	//number
	ui.number->set_label_text("River model number [-]");
	ui.number->set_tooltip("River model number, where the hydraulic structure is located");
	//name
	ui.name->set_label_text("River model name");
	ui.name->set_tooltip("");
	//type
	ui.type->set_label_text("Structure type [-]");
	ui.type->set_tooltip("Type of the hydraulic structure ('Weir' or 'Gate' is possible)");
	string types[] = {"WEIR", "GATE"};
	ui.type->set_items(types, 2);
	//profile_number
	ui.profile_number->set_label_text("Profile number [-]");
	ui.profile_number->set_tooltip("Profile number, to which the given distance (to the location) is related");
	//distance
	ui.distance->set_label_text("Related distance [m]");
	ui.distance->set_tooltip("Related distance [m] to the given profile number; here the mid of the structure is located");
	//left_dir
	ui.left_dir->set_label_text("Left direction");
	ui.left_dir->set_tooltip("Side of the river (in flow direction), where the structure is located (true:= left bank; false:= right bank)");
	//width
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
			QObject::connect(ui.type->comboBox, SIGNAL(currentIndexChanged(QString)), this, SLOT(type_changed(QString)));
	}

	this->prev_prec = 4;
	this->set_default_values();
	//count the memory
	Sys_Memory_Count::self()->add_mem(sizeof(HydGui_Coupling_Structure_Wid), _sys_system_modules::HYD_SYS);
}

//Default destructor
HydGui_Coupling_Structure_Wid::~HydGui_Coupling_Structure_Wid(void) {
	Sys_Memory_Count::self()->minus_mem(sizeof(HydGui_Coupling_Structure_Wid), _sys_system_modules::HYD_SYS);
}

//______________
//public methods

//method to set the whole widget editable
void HydGui_Coupling_Structure_Wid::set_editable(bool state) {
	this->editable = state;
	//general
	ui.number->set_editable(state);
	ui.name->set_editable(state);
	ui.type->set_editable(state);
	ui.profile_number->set_editable(state);
	ui.distance->set_editable(state);
	ui.left_dir->set_editable(state);
	//coupling structure
	ui.coupling_width->set_editable(state);
	ui.controlled->set_editable(state);
	ui.controllable_param->set_editable(state);
	ui.uncontrollable_param->set_editable(state);
}

//Set all members of the widget, similar to a copy constructor
void HydGui_Coupling_Structure_Wid::set_member(_Sys_Abstract_Base_Wid *ptr) {
	//upcast to derived object
	HydGui_Coupling_Structure_Wid *other = (HydGui_Coupling_Structure_Wid *) ptr;
	if (other == NULL) {
		std::cerr << "reinterpret_cast error: could not cast object\n";
		return;
	}
	//general parameters
	ui.number->set_value(other->ui.number->get_value());
	ui.name->set_text(other->ui.name->get_text());
	ui.type->set_current_value(other->ui.type->get_current_index());
	ui.profile_number->set_value(other->ui.profile_number->get_value());
	ui.distance->set_value(other->ui.distance->get_value());
	ui.left_dir->set_value(other->ui.left_dir->get_value());
	//coupling structure parameters
	ui.coupling_width->set_value(other->ui.coupling_width->get_value());
	ui.controlled->set_value(other->ui.controlled->get_value());
	ui.controllable_param->set_value(other->ui.controllable_param->get_value());
	ui.uncontrollable_param->set_value(other->ui.uncontrollable_param->get_value());
}

//Set default values
void HydGui_Coupling_Structure_Wid::set_default_values(void) {
	//head widget
	this->set_head_spinBox_range(0);
	this->set_head_spinBox_text("Set Nr.:");
	//general parameters
	ui.number->set_value(0);
	ui.name->set_text("-");
	ui.type->set_current_value(0);
	ui.profile_number->set_value(0);
	ui.distance->set_value(0);
	ui.left_dir->set_value(false);
	//coupling structure parameters
	ui.coupling_width->set_value(0);
	ui.controlled->set_value(false);
	ui.controllable_param->set_value(0);
	ui.uncontrollable_param->set_value(0);
}

//____________
//public slots

//Open the dialog to edit values in the widget
void HydGui_Coupling_Structure_Wid::show_as_dialog(void) {
	Sys_Base_Edit_Dia dialog(NULL, this);
	HydGui_Coupling_Structure_Wid inDiaWid(DataRole::EditRole, &dialog);
	inDiaWid.set_member(this);
	dialog.add_child(&inDiaWid);
	dialog.setWindowTitle(this->head_label->text());
	QIcon icon;
	icon.addPixmap(*this->head_pixmap->pixmap());
	dialog.setWindowIcon(icon);
	dialog.exec();
}

//_____________
//private slots

//Change the label text and tooltip for controllable- and uncontrollable param
void HydGui_Coupling_Structure_Wid::type_changed(QString item) {
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
void HydGui_Coupling_Structure_Wid::precision_changed(const int prec) {
	this->prev_prec = prec;
}
