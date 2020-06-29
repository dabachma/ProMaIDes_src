//#include "Sys_SpinBox_Delegate.h"
#include "Sys_Headers_Precompiled.h"

//default constructor
Sys_SpinBox_Delegate::Sys_SpinBox_Delegate(QObject *parent) : QItemDelegate(parent) {

	this->max_value=100000;
	this->min_value=-100000;
	this->interval_value=10;

	Sys_Memory_Count::self()->add_mem(sizeof(Sys_SpinBox_Delegate), _sys_system_modules::SYS_SYS);
}

//default destructor
Sys_SpinBox_Delegate::~Sys_SpinBox_Delegate(void) {
	Sys_Memory_Count::self()->minus_mem(sizeof(Sys_SpinBox_Delegate), _sys_system_modules::SYS_SYS);
}

QWidget *Sys_SpinBox_Delegate::createEditor(QWidget *parent, const QStyleOptionViewItem &/*option*/, const QModelIndex &/*index*/) const {
    QSpinBox *editor = new QSpinBox(parent);
    editor->setMinimum(this->min_value);
    editor->setMaximum(this->max_value);
	editor->setSingleStep(this->interval_value);

    return editor;
}

void Sys_SpinBox_Delegate::setEditorData(QWidget *editor, const QModelIndex &index) const {
    int value = index.model()->data(index, Qt::EditRole).toInt();

    QSpinBox *spinBox = static_cast<QSpinBox*>(editor);
    spinBox->setValue(value);
}

void Sys_SpinBox_Delegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const {
    QSpinBox *spinBox = static_cast<QSpinBox*>(editor);
    spinBox->interpretText();
    int value = spinBox->value();

    model->setData(index, value, Qt::EditRole);
}

void Sys_SpinBox_Delegate::updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &/*index*/) const {
    editor->setGeometry(option.rect);
}

//Set the spefic values for a spin-box
void Sys_SpinBox_Delegate::set_spin_box_values(const int max, const int min, const int interval){
	this->max_value=max;
	this->min_value=min;
	this->interval_value=interval;

}
