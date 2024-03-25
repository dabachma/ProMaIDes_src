//#include "FplGui_HighGround_FallTree_Wid.h"
#include "Fpl_Headers_Precompiled.h"

//Default constructor
FplGui_HighGround_FallTree_Wid::FplGui_HighGround_FallTree_Wid(DataRole role, QWidget *parent) : _Sys_Abstract_Base_Wid(parent) {
	//Qt stuff
	QWidget *widget = new QWidget(parent);
	ui.setupUi(widget);
	this->set_child(widget);
	this->scene = new FplGui_FaultTree_GraphicsScene();
	ui.graphicsView->setScene(this->scene);

	this->set_headLabel_text("Faulttree Highground");

#ifndef DEBUG
	ui.statusLabel->hide();
	ui.panButton->hide();
	ui.zoomButton->hide();
#endif //DEBUG

	this->overflowItem = new Sys_FallTree_BaseWid(QPixmap(":/wave_overtopping"), role);
	this->overflowItem->set_position(0,200);
	this->scene->addItem(this->overflowItem);

	this->overflowItem->add_dependent_line(QPoint(0, 0), this->overflowItem->tc_pos());
	QObject::connect(this->scene, SIGNAL(mousePositionChanged(QPointF)), this, SLOT(onMousePositionChanged(QPointF)));

	//Self
	switch(role) {
		case 0: //DisplayRole
			this->set_editable(false);
			this->head_pixmap->setPixmap(style()->standardPixmap(QStyle::SP_FileDialogInfoView));
			break;
		case 1: //EditRole
			this->set_editable(true);
			this->headWidget->hide();
			QObject::disconnect(this, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(contextMenu_request(QPoint)));
	}

	this->set_default_values();
	//count the memory
	Memory_Count::self()->add_mem(sizeof(FplGui_HighGround_FallTree_Wid), _system_modules::FPL_SYS);
}

//Default destructor
FplGui_HighGround_FallTree_Wid::~FplGui_HighGround_FallTree_Wid(void) {
	delete this->overflowItem;
	//count the memory
	Memory_Count::self()->minus_mem(sizeof(FplGui_HighGround_FallTree_Wid), _system_modules::FPL_SYS);
}

//______________
//public methods

//method to set the whole widget editable
void FplGui_HighGround_FallTree_Wid::set_editable(const bool state) {
	this->editable = state;
}

//Set all members of the widget, similar to a copy constructor
void FplGui_HighGround_FallTree_Wid::set_member(_Sys_Abstract_Base_Wid *ptr) {
	//upcast to derived object
	FplGui_HighGround_FallTree_Wid *other = (FplGui_HighGround_FallTree_Wid *) ptr;
	if (other == NULL) {
		std::cerr << "reinterpret_cast error: could not cast object\n";
		return;
	}
	this->overflowItem->set_active(other->overflowItem->is_active());
}

//Set default values
void FplGui_HighGround_FallTree_Wid::set_default_values(void) {
	//head widget
	this->set_head_spinBox_range(0);
	this->set_head_spinBox_text("Set Nr.:");
	//general parameters
	this->overflowItem->set_active(true);
}

//____________
//public slots

//Open the dialog to edit values in the widget
void FplGui_HighGround_FallTree_Wid::show_as_dialog(void) {
	Sys_Base_Edit_Dia dialog(NULL, this);
	FplGui_HighGround_FallTree_Wid inDiaWid(DataRole::EditRole, &dialog);
	inDiaWid.set_member(this);
	dialog.add_child(&inDiaWid);
	dialog.setWindowTitle(this->head_label->text());
	QIcon icon;
	icon.addPixmap(*this->head_pixmap->pixmap());
	dialog.setWindowIcon(icon);
	dialog.exec();
}

#ifdef DEBUG
//
void FplGui_HighGround_FallTree_Wid::onMousePositionChanged(QPointF pos) {
	QString qtext, xpos, ypos;
	qtext = "x = " + xpos.setNum(pos.x()) + "; y = " + ypos.setNum(pos.y());
	ui.statusLabel->setText(qtext);
}
#endif //DEBUG

//
void FplGui_HighGround_FallTree_Wid::on_item_state_changed(const bool state) {

}
