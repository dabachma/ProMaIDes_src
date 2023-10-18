//#include "DamGui_Eco_Btype_Wid.h"
#include "Dam_Headers_Precompiled.h"

//default constructor
DamGui_Eco_Btype_Wid::DamGui_Eco_Btype_Wid(DataRole role, QWidget *parent) : _Sys_Abstract_Base_Wid(parent) {
	// Qt Stuff
	QWidget *widget = new QWidget(parent);
	ui.setupUi(widget);
	this->set_child(widget);

	this->area2print=ui.scrollAreaWidgetContents;
	this->scroll_area=ui.scrollArea;
	this->set_headLabel_text("Biotope Type");

	//biotope_category
	ui.biotope_category->set_label_text("Biotope category");
	ui.biotope_category->set_tooltip("Identifier of the biotope category");
	ui.biotope_category->set_range(0,999999);

	//biotope_name
	ui.biotope_name->set_label_text("Biotope name");
	ui.biotope_name->set_tooltip("The name of the biotope category");

	//stockvalue
	ui.stockvalue->set_label_text("Stockvalue [Monetary/m²]");
	ui.stockvalue->set_tooltip("The default stock value for the biotope category [Monetary/m²]");
	ui.stockvalue->spinBox->setMinimum(0);
	ui.stockvalue->set_dataRole(role);

	// SELF
	switch(role) {
		case 0: //DISPLAY_ROLE
			this->set_editable(false);
			this->head_pixmap->setPixmap(style()->standardPixmap(QStyle::SP_FileDialogInfoView));
			break;
		case 1: //EDIT_ROLE
			this->set_editable(true);
			this->headWidget->hide();
			QObject::disconnect(this, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(contextMenu_request(QPoint)));
	}

	this->role = role;

	this->impact_widgets = NULL;
	this->number_of_impact_widgets = 0;

	this->set_default_values();
	//count the memory
	Sys_Memory_Count::self()->add_mem(sizeof(DamGui_Eco_Btype_Wid), _sys_system_modules::DAM_SYS);
}

//default destructor
DamGui_Eco_Btype_Wid::~DamGui_Eco_Btype_Wid(void) {
	this->delete_impact_widgets();

	Sys_Memory_Count::self()->minus_mem(sizeof(DamGui_Eco_Btype_Wid), _sys_system_modules::DAM_SYS);
}

//________
//public
//method to set the whole widget editable
void DamGui_Eco_Btype_Wid::set_editable(const bool state) {
	this->editable = state;

	ui.biotope_category->set_editable(state);
	ui.biotope_name->set_editable(state);
	ui.stockvalue->set_editable(state);
}
//Set the number of impact widgets
void DamGui_Eco_Btype_Wid::set_number_of_impact_widgets(int number) {
	if (number > 0) {
		this->number_of_impact_widgets = number;
		this->allocate_impact_widgets();

		for (int i = 0; i < number; i++) {
			this->impact_widgets[i] = new DamGui_Eco_Btype_Func_Wid(this->role, this);
			ui.impact_layout->addWidget(this->impact_widgets[i]);
		}
	}
}
//Common getter the number of impactWidgets
int DamGui_Eco_Btype_Wid::get_number_of_impact_widgets(void) {
	return this->number_of_impact_widgets;
}
//Set impact widget properties
void DamGui_Eco_Btype_Wid::set_impact_widget_properties(int index, int impact_type_index, double weight, bool stepwise_flag) {
	if (index < this->number_of_impact_widgets) {
		this->impact_widgets[index]->impact_type->set_current_value(impact_type_index);
		this->impact_widgets[index]->weight->set_value(weight);
		this->impact_widgets[index]->stepwise_flag->set_value(stepwise_flag);
	}
}
//Set all members of the widget, similar to a copy constructor
void DamGui_Eco_Btype_Wid::set_member(_Sys_Abstract_Base_Wid *ptr) {
	//upcast to derived object
	DamGui_Eco_Btype_Wid *other = (DamGui_Eco_Btype_Wid *) ptr;
	if (other == NULL) {
		std::cerr << "reinterpret_cast error: could not cast object\n";
		return;
	}
	ui.biotope_category->set_value(other->ui.biotope_category->get_value());
	ui.biotope_name->set_text(other->ui.biotope_name->get_text());
	ui.stockvalue->set_value(other->ui.stockvalue->get_value());

	if (this->impact_widgets != NULL) {
		if (this->number_of_impact_widgets == other->get_number_of_impact_widgets()) {
			for (int i = 0; i < this->number_of_impact_widgets; i++) {
				this->impact_widgets[i]->impact_type->set_current_value(other->impact_widgets[i]->impact_type->get_current_index());
				this->impact_widgets[i]->weight->set_value(other->impact_widgets[i]->weight->get_value());
				this->impact_widgets[i]->stepwise_flag->set_value(other->impact_widgets[i]->stepwise_flag->get_value());
			}
		}
	}
}
//Set default values
void DamGui_Eco_Btype_Wid::set_default_values(void) {
	//head widget
	this->set_head_spinBox_range(0);
	this->set_head_spinBox_text("Set Nr.:");
	//general settings
	ui.biotope_category->set_value(0);
	ui.biotope_name->set_text("");
	ui.stockvalue->set_value(0);

	if (this->impact_widgets != NULL) {
		for (int i = 0; i < this->number_of_impact_widgets; i++) {
			this->impact_widgets[i]->set_default_values();
		}
	}
}
//Set the member of the widget per database
void DamGui_Eco_Btype_Wid::set_member(QSqlDatabase *ptr_database, const int btype_no){
	try{	
		QSqlTableModel results(0, *ptr_database);
		this->ptr_database=ptr_database;

		if(Dam_Eco_Btype::select_relevant_biotope_types_database(&results, btype_no)==0){
			this->set_headLabel_text("Unknown Biotope Type");
			this->setEnabled(false);
		}

		Dam_Eco_Btype buffer;
		buffer.input_biotope_type_perdatabase(&results, 0);
		ui.biotope_category->set_value(buffer.id);
		ui.biotope_name->set_text(buffer.name);
		ui.stockvalue->set_value(buffer.cost);

		this->set_function2plot(&buffer);

		//set icon
		QPixmap icon;
		icon.load(":eco_btype_icon" );
		this->set_headPixmap(icon);
	}
	catch(Error msg){
		msg.output_msg(0);
	}
}
//____________
//public slots
//Open the dialog to edit values in the widget
void DamGui_Eco_Btype_Wid::show_as_dialog(void) {
	Sys_Base_Edit_Dia dialog(NULL, this);
	DamGui_Eco_Btype_Wid inDiaWid(DataRole::EditRole, &dialog);
	inDiaWid.set_number_of_impact_widgets(this->number_of_impact_widgets);
	inDiaWid.set_member(this);
	dialog.add_child(&inDiaWid);
	for (int i = 0; i < this->number_of_impact_widgets; i++) {
		
	}
	dialog.setWindowTitle(this->head_label->text());
	QIcon icon;
	icon.addPixmap(*this->head_pixmap->pixmap());
	dialog.setWindowIcon(icon);
	dialog.exec();
}
//______________
//private mehods
//Allocate impact widgets
void DamGui_Eco_Btype_Wid::allocate_impact_widgets(void) {
	this->delete_impact_widgets();

	this->impact_widgets = new DamGui_Eco_Btype_Func_Wid*[this->number_of_impact_widgets];
}
//Delete impact widgets
void DamGui_Eco_Btype_Wid::delete_impact_widgets(void) {

	//delete if necessary
	if (this->impact_widgets != NULL) {

		for (int i = 0; i < this->number_of_impact_widgets; i++) {
		
			if (this->impact_widgets[i] != NULL) {
				delete [] this->impact_widgets[i];
				this->impact_widgets[i] = NULL;
			}
		}

		delete [] this->impact_widgets;
		this->impact_widgets = NULL;
	}
}

//Set the damage function(s) to the plot(s) of the widget
void DamGui_Eco_Btype_Wid::set_function2plot(Dam_Eco_Btype *btype){

	this->set_number_of_impact_widgets(btype->number_dam_function);

	for(int i=0; i< this->number_of_impact_widgets; i++){
		this->impact_widgets[i]->set_member2widget(btype->get_ptr_btype_function(i));
	}
}