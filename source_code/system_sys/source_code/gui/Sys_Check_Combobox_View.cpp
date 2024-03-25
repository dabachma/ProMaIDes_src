#include "Sys_Headers_Precompiled.h"
//#include "Sys_Check_Combobox_View.h"

//Default constructor
Sys_Check_Combobox_View::Sys_Check_Combobox_View(QWidget* parent): QListView(parent){
}
//Default destructor
Sys_Check_Combobox_View::~Sys_Check_Combobox_View(void){
}
//________
//public
//Install an event filter on the selection combobox view to handle only clicks over the checkboxes in the combobox.
bool Sys_Check_Combobox_View::eventFilter(QObject* , QEvent* event){
	if (event->type() == QEvent::MouseButtonRelease){
		QMouseEvent* mouse = static_cast<QMouseEvent*>(event);
		QModelIndex index = indexAt(mouse->pos());
		if (index.isValid())
		{
			//Check if the mouse was released over the checkbox
			QStyleOptionButton option;
			option.QStyleOption::operator=(viewOptions());
			option.rect = visualRect(index);
			QRect rect = style()->subElementRect(QStyle::SE_ViewItemCheckIndicator, &option);
			if (rect.contains(mouse->pos()))
			{
				//Mouse was release over a checkbox, bypass combobox and deliver the event just for the listview
				QListView::mouseReleaseEvent(mouse);
				return true;
			}
		}
	}
	return false;
}