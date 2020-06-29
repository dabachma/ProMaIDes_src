//#include "Sys_DoubleSpinBox_Delegate.h"
#include "Sys_Headers_Precompiled.h"

//Default Constructor
Sys_DoubleSpinBox_Delegate::Sys_DoubleSpinBox_Delegate(QObject *parent) : QItemDelegate(parent) {

	this->max_value=100000.0;
	this->min_value=-100000.0;
	this->interval_value=10.0;
	this->precison=2;

	Sys_Memory_Count::self()->add_mem(sizeof(Sys_DoubleSpinBox_Delegate), _sys_system_modules::SYS_SYS);
}

//default destructor
Sys_DoubleSpinBox_Delegate::~Sys_DoubleSpinBox_Delegate(void) {
	Sys_Memory_Count::self()->minus_mem(sizeof(Sys_DoubleSpinBox_Delegate), _sys_system_modules::SYS_SYS);
}
//________
//public
//Create the editor widget
QWidget *Sys_DoubleSpinBox_Delegate::createEditor(QWidget *parent, const QStyleOptionViewItem &/*option*/, const QModelIndex &/*index*/) const {
    QDoubleSpinBox *editor = new QDoubleSpinBox(parent);
	editor->setMinimum(this->min_value);
	editor->setMaximum(this->max_value);
	editor->setSingleStep(this->interval_value);
	editor->setDecimals(this->precison);

    return editor;
}
//Set the editors data
void Sys_DoubleSpinBox_Delegate::setEditorData(QWidget *editor, const QModelIndex &index) const {
	double value = index.model()->data(index, Qt::EditRole).toDouble();

    QDoubleSpinBox *doubleSpinBox = static_cast<QDoubleSpinBox*>(editor);
    doubleSpinBox->setValue(value);
}
//Set the models data
void Sys_DoubleSpinBox_Delegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const {
    QDoubleSpinBox *doubleSpinBox = static_cast<QDoubleSpinBox*>(editor);
    doubleSpinBox->interpretText();
    double value = doubleSpinBox->value();

    model->setData(index, value, Qt::EditRole);
}
//Update the editors geometry
void Sys_DoubleSpinBox_Delegate::updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &/*index*/) const {
    editor->setGeometry(option.rect);
}
//Set the spefic values for a spin-box
void Sys_DoubleSpinBox_Delegate::set_spin_box_values(const double max, const double min, const double interval, const int precision){
	this->max_value=max;
	this->min_value=min;
	this->interval_value=interval;
	this->precison=precision;
}
