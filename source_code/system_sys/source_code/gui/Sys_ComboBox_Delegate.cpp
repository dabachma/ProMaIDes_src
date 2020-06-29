//#include "Sys_ComboBox_Delegate.h"
#include "Sys_Headers_Precompiled.h"

//default construcor
Sys_ComboBox_Delegate::Sys_ComboBox_Delegate(QObject *parent) : QItemDelegate(parent) {
	Sys_Memory_Count::self()->add_mem(sizeof(Sys_ComboBox_Delegate), _sys_system_modules::SYS_SYS);
}
//default destructor
Sys_ComboBox_Delegate::~Sys_ComboBox_Delegate(void) {
	Sys_Memory_Count::self()->minus_mem(sizeof(Sys_ComboBox_Delegate), _sys_system_modules::SYS_SYS);
}
//______
//public
//Create the editor widget
QWidget *Sys_ComboBox_Delegate::createEditor(QWidget *parent, const QStyleOptionViewItem &, const QModelIndex &) const {
	QComboBox *combo = new QComboBox(parent);
	
	combo->addItems(this->items);

	connect(combo, SIGNAL(activated(int)), this, SLOT(emitCommitData()));
	return combo;
}
//Set the editors data
void Sys_ComboBox_Delegate::setEditorData( QWidget *editor, const QModelIndex &index ) const {

	QComboBox *combo = qobject_cast<QComboBox*>(editor);
	if (!combo) {
		QItemDelegate::setEditorData(editor, index); // Extrem wichtig, sonst kommt man nicht mehr zurück.
		return;
	}
	int pos = combo->findText(index.model()->data(index).toString(), Qt::MatchExactly);
	combo->setCurrentIndex(pos);
}
//Set the models data
void Sys_ComboBox_Delegate::setModelData( QWidget *editor, QAbstractItemModel *model, const QModelIndex &index ) const {

	QComboBox *combo = qobject_cast<QComboBox*>(editor);
	if (!combo) {
		QItemDelegate::setModelData(editor, model, index); // Extrem wichtig, sonst kommt man nicht mehr zurück.
		return;
	}
	model->setData(index, combo->currentText());
}
//Update the editors geometry
void Sys_ComboBox_Delegate::updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &) const {
    editor->setGeometry(option.rect);
}
//Set the string list for the combobox items
void Sys_ComboBox_Delegate::set_string_list(QStringList items){
	this->items=items;
}
//________
//private slot
//Emit the commited data
void Sys_ComboBox_Delegate::emitCommitData() { 
	emit commitData(qobject_cast<QWidget*>(sender())); 
}
