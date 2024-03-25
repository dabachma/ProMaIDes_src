//#include "DamGui_Raster_Wid.h"
#include "Dam_Headers_Precompiled.h"

//default constructor
DamGui_Raster_Wid::DamGui_Raster_Wid(DataRole role, QWidget *parent) : _Sys_Abstract_Base_Wid(parent) {
	// Qt Stuff
	QWidget *widget = new QWidget(parent);
	ui.setupUi(widget);
	this->set_child(widget);

	this->area2print=ui.scrollAreaWidgetContents;
	this->scroll_area=ui.scrollArea;

	this->set_headLabel_text("Damage Raster");

	//raster_name
	ui.raster_name->set_label_text("Raster name");
	ui.raster_name->set_tooltip("Name of the raster");

	//raster_type
	ui.raster_type->set_label_text("Raster type");
	string types[] = {"ECN", "ECO biotope-type", "ECO soil-erosion", "POP", "PYS"};
	ui.raster_type->set_items(types, 5);
	ui.raster_type->set_tooltip("Type of the raster");

	//ncols
	ui.ncols->set_label_text("Number of columns [-]");
	ui.ncols->set_tooltip("Number of columns or number of elements in x-direction");
	ui.ncols->set_range(0, 9999999);

	//nrows
	ui.nrows->set_label_text("Number of rows [-]");
	ui.nrows->set_tooltip("Number of rows or number of elements in y-direction");
	ui.nrows->set_range(0, 9999999);

	//xllcorner
	ui.xllcorner->set_label_text("x-Coordinate of left/low corner [m]");
	ui.xllcorner->set_tooltip("x-ccordinate of the raster origin in [m]");
	ui.xllcorner->set_dataRole(role);
	ui.xllcorner->set_range(-999999999999.0, 999999999999.0);
	ui.xllcorner->set_precision(2);

	//yllcorner
	ui.yllcorner->set_label_text("y-Coordinate of left/low corner [m]");
	ui.yllcorner->set_tooltip("y-ccordinate of the raster origin in [m]");
	ui.yllcorner->set_dataRole(role);
	ui.yllcorner->set_range(-9999999999.0, 99999999999.0);
	ui.yllcorner->set_precision(2);

	//cellsize
	ui.cellsize->set_label_text("Cellsize [m]");
	ui.cellsize->set_tooltip("Width of the quadratic element [m]");
	ui.cellsize->set_dataRole(role);
	ui.cellsize->set_range(0.0, 999999.0);
	ui.cellsize->set_precision(2);

	//nodata
	ui.nodata->set_label_text("Nodata value [-]");
	ui.nodata->set_tooltip("Value for representing a no-information element");
	ui.nodata->set_range(-99999999, 999999999);

	// SELF
	switch(role) {
		case 0: //DISPLAY_ROLE
			this->set_editable(false);
			this->head_pixmap->setPixmap(QPixmap(":/32x32/DamGui_Raster"));
			break;
		case 1: //EDIT_ROLE
			this->set_editable(true);
			this->headWidget->hide();
			QObject::disconnect(this, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(contextMenu_request(QPoint)));
	}

	this->set_default_values();
	//count the memory
	Sys_Memory_Count::self()->add_mem(sizeof(DamGui_Raster_Wid), _sys_system_modules::DAM_SYS);
}
//default destructor
DamGui_Raster_Wid::~DamGui_Raster_Wid(void) {
	Sys_Memory_Count::self()->minus_mem(sizeof(DamGui_Raster_Wid), _sys_system_modules::DAM_SYS);
}
//_________
//public
//method to set the whole widget editable
void DamGui_Raster_Wid::set_editable(const bool state) {
	this->editable = state;

	ui.raster_name->set_editable(state);
	ui.raster_type->set_editable(state);
	ui.ncols->set_editable(state);
	ui.nrows->set_editable(state);
	ui.xllcorner->set_editable(state);
	ui.yllcorner->set_editable(state);
	ui.cellsize->set_editable(state);
	ui.nodata->set_editable(state);
}
//Set all members of the widget, similar to a copy constructor
void DamGui_Raster_Wid::set_member(_Sys_Abstract_Base_Wid *ptr) {
	//upcast to derived object
	DamGui_Raster_Wid *other = (DamGui_Raster_Wid *) ptr;
	if (other == NULL) {
		std::cerr << "reinterpret_cast error: could not cast object\n";
		return;
	}
	//general settings
	ui.raster_name->set_text(other->ui.raster_name->get_text());
	ui.raster_type->set_current_value(other->ui.raster_type->get_current_index());
	//file settings
	ui.ncols->set_value(other->ui.ncols->get_value());
	ui.nrows->set_value(other->ui.nrows->get_value());
	ui.xllcorner->set_value(other->ui.xllcorner->get_value());
	ui.yllcorner->set_value(other->ui.yllcorner->get_value());
	ui.cellsize->set_value(other->ui.cellsize->get_value());
	ui.nodata->set_value(other->ui.nodata->get_value());
}
//Set default values
void DamGui_Raster_Wid::set_default_values(void) {
	//head widget
	this->set_head_spinBox_range(0);
	this->set_head_spinBox_text("Set Nr.:");
	//general settings
	ui.raster_name->set_text("");
	ui.raster_type->set_current_value(0);
	//file settings
	ui.ncols->set_value(0);
	ui.nrows->set_value(0);
	ui.xllcorner->set_value(0);
	ui.yllcorner->set_value(0);
	ui.cellsize->set_value(0);
	ui.nodata->set_value(0);
}
//Set the member of the widget per database
void DamGui_Raster_Wid::set_member(QSqlDatabase *ptr_database, const int raster_no, const _dam_raster_types type){
	try{
		//QPixmap icon;
		QSqlQueryModel results;
		this->ptr_database=ptr_database;

		ui.raster_type->set_editable(true);

		if(type==_dam_raster_types::ecn_total){
			//picture
			//icon.load(":dam_icon" );
			ui.raster_type->set_current_value(0);
			Dam_Ecn_Raster::select_relevant_raster_database(&results, this->ptr_database, raster_no);
			Dam_Ecn_Raster buffer;
			buffer.input_raster_perdatabase_general_data(&results, 0);
			this->set_raster_member(&buffer);
		}
		else if(type==_dam_raster_types::eco_btype){
			//picture
			//icon.load(":eco_btype_icon" );
			ui.raster_type->set_current_value(1);
			Dam_Eco_Btype_Raster::select_relevant_raster_database(&results, this->ptr_database, raster_no);
			Dam_Eco_Btype_Raster buffer;
			buffer.input_raster_perdatabase_general_data(&results, 0);
			this->set_raster_member(&buffer);
		}
		else if(type==_dam_raster_types::eco_soil_total){
			//picture
			//icon.load(":eco_soil_icon" );
			ui.raster_type->set_current_value(2);
			Dam_Eco_Soil_Raster::select_relevant_raster_database(&results, this->ptr_database, raster_no);
			Dam_Eco_Soil_Raster buffer;
			buffer.input_raster_perdatabase_general_data(&results, 0);
			this->set_raster_member(&buffer);
		}
		else if(type==_dam_raster_types::pop_total){
			//picture
			//icon.load(":pop_icon" );
			ui.raster_type->set_current_value(3);
			Dam_People_Raster::select_relevant_raster_database(&results, this->ptr_database, raster_no);
			Dam_People_Raster buffer;
			buffer.input_raster_perdatabase_general_data(&results, 0);
			this->set_raster_member(&buffer);
		}
		else if(type==_dam_raster_types::pys_pys_total){
			//picture
			//icon.load(":pys_icon" );
			ui.raster_type->set_current_value(4);
			Dam_Pys_Raster::select_relevant_raster_database(&results, this->ptr_database, raster_no);
			Dam_Pys_Raster buffer;
			buffer.input_raster_perdatabase_general_data(&results, 0);
			this->set_raster_member(&buffer);
		}
		else{
			this->set_headLabel_text("Unknown Damage Raster");
			this->setEnabled(false);
		}

		ui.raster_type->set_editable(false);
		//this->set_headPixmap(icon);
	}
	catch(Error msg){
		msg.output_msg(0);
	}
}
//____________
//private slots
//Open the dialog to edit values in the widget
void DamGui_Raster_Wid::show_as_dialog(void) {
	Sys_Base_Edit_Dia dialog(NULL, this);
	DamGui_Raster_Wid inDiaWid(DataRole::EditRole, &dialog);
	inDiaWid.set_member(this);
	dialog.add_child(&inDiaWid);
	dialog.setWindowTitle(this->head_label->text());
	QIcon icon;
	icon.addPixmap(*this->head_pixmap->pixmap());
	dialog.setWindowIcon(icon);
	dialog.exec();
}
//___________
//private
//Set the raster member 
void DamGui_Raster_Wid::set_raster_member(Dam_Raster *raster){

	ui.raster_name->set_text(raster->name);
	ui.ncols->set_value(raster->no_elems_x);
	ui.nrows->set_value(raster->no_elems_y);
	ui.xllcorner->set_value(raster->x_coor_origin);
	ui.yllcorner->set_value(raster->y_coor_origin);
	ui.cellsize->set_value(raster->width_x);
	ui.nodata->set_value(raster->no_info_value);

}
