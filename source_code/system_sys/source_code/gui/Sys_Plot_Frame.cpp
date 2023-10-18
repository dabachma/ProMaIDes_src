#include "Sys_Headers_Precompiled.h"
//#include "Sys_Plot_Frame.h"


//Default constructor
Sys_Plot_Frame::Sys_Plot_Frame(QWidget *parent) : QFrame(parent){
	this->ui.setupUi(this);
    //this->ui.plot->setFramePtr(this);
    //this->getPlotPtr()->setParent(parent);
   this->ui.zoomPushButton->setToolTip(
        tr("Zoom In or Out with Mouse Wheel\nPress Mouse Wheel to restore size\nZoom selected region with Ctrl + Left Mouse Button\nPan the plot with Left Mouse Button"));

   // this->my_plot=NULL;
    //this->my_plot=new Sys_Plot_Wid(this);
    //this->my_plot->setFramePtr(this);
   // this->ui.verticalLayout->addWidget(this->my_plot);
    this->getPlotPtr()->setFramePtr(this);
    this->connectButtons();

   //this->ui.expander->add_child(this->ui.widget_button);


    this->getPlotPtr()->canvas()->setCursor(Qt::PointingHandCursor);

	//Set selection combobox properties
	this->ui.selectionComboBox->setEditable(true);
	this->ui.selectionComboBox->lineEdit()->setReadOnly(true);
	Sys_Check_Combobox_Model* model = new Sys_Check_Combobox_Model(this);
	Sys_Check_Combobox_View*  view  = new Sys_Check_Combobox_View(this);
	this->ui.selectionComboBox->setModel(model); 
	this->ui.selectionComboBox->setView(view);
	QObject::connect(this->ui.selectionComboBox->view(), SIGNAL(clicked(const QModelIndex&)), this, SLOT(editComboBoxTextChanged(const QModelIndex&)));
	QObject::connect(this->ui.selectionComboBox, SIGNAL(editTextChanged(const QString )), this, SLOT(change_visible_text(const QString )));
	QObject::connect(this->ui.selectionComboBox->view(), SIGNAL(clicked(const QModelIndex&)), this->getPlotPtr(), SLOT(showCurves(const QModelIndex&)));
	//Install the event filter on the selection combobox to handle mouse clicks and releases
	this->ui.selectionComboBox->view()->viewport()->installEventFilter(view);	
	this->ui.selectionComboBox->setMinimumWidth(150);
	//Checkable items are added in the selection combobox. These are default all checked.
    this->getPlotPtr()->addItem(true);
	this->ui.selectionComboBox->view()->setAlternatingRowColors(true);

	this->ui.groupBox->setTitle("");

	Sys_Memory_Count::self()->add_mem((sizeof(Sys_Plot_Frame)), _sys_system_modules::SYS_SYS);
}
//Default destructor
Sys_Plot_Frame::~Sys_Plot_Frame(void){

	Sys_Memory_Count::self()->minus_mem((sizeof(Sys_Plot_Frame)), _sys_system_modules::SYS_SYS);
}
//_______
//public
//Return pointer on the plot. This method provides an interface to access Sys_Plot_Wid functions
Sys_Plot_Wid* Sys_Plot_Frame::getPlotPtr(void){
    //return qobject_cast<Sys_Plot_Wid* >(this->ui.plot);
    return this->ui.plot;
    //return this->my_plot;
}
//Set data to the model stored in the selection combobox
void Sys_Plot_Frame::setComboBoxData(int index, bool checked){
	int counter = this->ui.selectionComboBox->count();
	if((index < 0) || (index >= counter)){
		return;
	}
	this->ui.selectionComboBox->setItemData(index, Qt::Checked, Qt::CheckStateRole);
	
	//Set the curve names in the selection combobox if items are checked
	if(checked){
		//Iteration over the items in the selection combobox
		if(map.find(index) == map.end()){
			QString str = this->ui.selectionComboBox->itemText(index);
			map.insert(index, str);
		}
	}
}
// Check whether the checkboxes in the ComboBox are checked or not
bool Sys_Plot_Frame::isChecked(int index){
	int counter = this->ui.selectionComboBox->count();
	if((index < 0) || (index >= counter)){ 
		return false;
	}

	QVariant var = this->ui.selectionComboBox->itemData(index, Qt::CheckStateRole);
	if(var == Qt::Checked){
		return true;
	}
	else{
		return false;
	}
}
//Set the curve names in the combobox as default text
void Sys_Plot_Frame::editComboBoxTextChanged(void){
	int counter = this->ui.selectionComboBox->count();
	QString str;
	str.append("Choose visible graphs...");
	/*for (int i = 0; i < counter; ++i){
		if(isChecked(i)){
			if(map.value(i)!=""){
				str += map.value(i);
				str += "|";
			}
		}
	}*/
	this->ui.selectionComboBox->setEditText(str);
}
//Set title for the groupbox containing tool buttons and plot
void Sys_Plot_Frame::set_groupbox_title(const string title){
	this->ui.groupBox->setTitle(title.c_str());
}
//_______
//private
//Connect all the buttons in one method, that should be call once in the Sys_Plot_Frame constructor
void Sys_Plot_Frame::connectButtons(void){
    QObject::connect(this->ui.printPushButton, SIGNAL(clicked()), this->getPlotPtr(), SLOT(printPlot()));
    QObject::connect(this->ui.pdfPushButton, SIGNAL(clicked()), this->getPlotPtr(), SLOT(printPDF()));
    QObject::connect(this->ui.savePushButton, SIGNAL(clicked()), this->getPlotPtr(), SLOT(saveAS()));
    QObject::connect(this->ui.zoomPushButton, SIGNAL(toggled(bool)), this->getPlotPtr(), SLOT(enableZoomMode(bool)));
    QObject::connect(this->ui.zoomPushButton, SIGNAL(toggled(bool)), this, SLOT(checkActions(bool)));
    QObject::connect(this->ui.restoresizePushButton, SIGNAL(clicked()), this->getPlotPtr(), SLOT(restoreSize()));
    QObject::connect(this->ui.exportPushButton, SIGNAL(clicked()), this->getPlotPtr(), SLOT(export_curve_data()));
}
//______
//private slots
//Ensure that Zoom Action is checked as soon as Zoom Button is being checked
void Sys_Plot_Frame::checkActions(bool on){
    this->getPlotPtr()->zoomAct->setChecked(on);
}
//Update the displayed text of the Combobox after every user selections
void Sys_Plot_Frame::editComboBoxTextChanged(const QModelIndex & index){
/**Text is the name of the selected curves separated by |
Overloaded member function. See editComboBoxTextChanged() method
*/
	int i = index.row();
	QVariant var = this->ui.selectionComboBox->itemData(i, Qt::CheckStateRole);
	if(var==Qt::Checked){
		this->ui.selectionComboBox->setItemData(i, Qt::Unchecked, Qt::CheckStateRole);
	}
	else{
		this->ui.selectionComboBox->setItemData(i, Qt::Checked, Qt::CheckStateRole);
	}
	int counter = this->ui.selectionComboBox->count();
	QString str;
	str.append("Choose visible graphs...");
	//for (i = 0; i< counter; ++i){
	//	if(isChecked(i)){
	//		if(map.value(i)!=""){
	//			str += map.value(i);
	//			str += "|";
	//		}
	//	}
	//}
	this->ui.selectionComboBox->setEditText(str);
}
//Change the visible text after a item is just activated
void Sys_Plot_Frame::change_visible_text(const QString ){
	QObject::disconnect(this->ui.selectionComboBox, SIGNAL(editTextChanged(const QString )), this, SLOT(change_visible_text(const QString )));
	QString str;
	str.append("Choose visible graphs...");
	this->ui.selectionComboBox->setEditText(str);
	QObject::connect(this->ui.selectionComboBox, SIGNAL(editTextChanged(const QString )), this, SLOT(change_visible_text(const QString )));
}
