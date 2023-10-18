//#include "FplGui_HighGround_FaultTree_Wid.h"
#include "Fpl_Headers_Precompiled.h"
//Default constructor
FplGui_HighGround_FaultTree_Wid::FplGui_HighGround_FaultTree_Wid(DataRole role, QWidget *parent) : FplGui_FaultTree_BaseWid(parent) {
	//Sys_Abstract_BaseWid stuff
	//Set the widgets header
	set_headLabel_text("Highground Faulttree");
	set_child(view);
	//FplGui_FaultTree_BaseWid stuff
	setup_items();
	set_item_position_to_defaults();
	connect(arrangeAction, SIGNAL(triggered()), this, SLOT(set_item_position_to_defaults()));
	
	//Macros defined in _Sys_Abstract_Base_Wid.h
	DATA_ROLE_CONSTRUCTOR_STUFF();
	ADD_MEM(FplGui_HighGround_FaultTree_Wid, _sys_system_modules::FPL_SYS);
}
//Default destructor
FplGui_HighGround_FaultTree_Wid::~FplGui_HighGround_FaultTree_Wid(void) {
	MIN_MEM(FplGui_HighGround_FaultTree_Wid, _sys_system_modules::FPL_SYS);
}
//______________
//public methods
//
void FplGui_HighGround_FaultTree_Wid::set_overflow_active(const bool active) {
	if (active)
	{
		overflowItem->set_active_button(tr("Overflow"));
	}
	else if (active == false && is_overflow_active())
	{
		overflowItem->set_active_button(-1);
	}
}
//
bool FplGui_HighGround_FaultTree_Wid::is_overflow_active() const {
	return overflowItem->is_active();
}
//
void FplGui_HighGround_FaultTree_Wid::reset(void)
{
	this->set_overflow_active(false);
}
//method to set the whole widget editable
void FplGui_HighGround_FaultTree_Wid::set_editable(const bool state) {
	editable = state;
	set_items_checkable(state);
}
//Set all members of the widget, similar to a copy constructor
void FplGui_HighGround_FaultTree_Wid::set_member(_Sys_Abstract_Base_Wid *ptr) {
	//upcast to derived object
	FplGui_HighGround_FaultTree_Wid *other = (FplGui_HighGround_FaultTree_Wid *) ptr;
	if (other == NULL) {
		std::cerr << "reinterpret_cast error: could not cast object\n";
		return;
	}
	//
	reset();
	//
	set_overflow_active(other->is_overflow_active());
}
//Set default values
void FplGui_HighGround_FaultTree_Wid::set_default_values(void) {
	//head widget
	set_head_spinBox_range(0);
	set_head_spinBox_text("Set Nr.:");
	//
	reset();
}
//____________
//public slots
//Open the dialog to edit values in the widget
void FplGui_HighGround_FaultTree_Wid::show_as_dialog(void) {
	SHOW_AS_DIALOG(FplGui_HighGround_FaultTree_Wid);
}
//
void FplGui_HighGround_FaultTree_Wid::set_item_position_to_defaults()
{
	failureItem->set_pos(0, -100);
	overflowItem->set_pos(0, 100);
}
//_______________
//private methods
//
void FplGui_HighGround_FaultTree_Wid::setup_items()
{
	failureItem = new Sys_Elastic_GraphicsJunction(scene, this);
	failureItem->set_custom_pixmap(QPixmap(":/failure_icon"));
	failureItem->set_type(Sys_Elastic_GraphicsJunction::Custom);
	failureItem->set_size(102.0, 58.0);

	overflowItem = new Sys_Elastic_GraphicsPixmapItem(scene, this);
	overflowItem->add_pixmap(tr("Overflow"), QPixmap(":/highground_overflow"));
	overflowItem->set_size(80.0, 80.0);

	failureItem->add_bottomLevel_object(overflowItem);

	//this->set_overflow_active(false);

	items.append(overflowItem);
}
//
void FplGui_HighGround_FaultTree_Wid::set_items_checkable(const bool checkable)
{
	overflowItem->set_enabled(checkable);
}