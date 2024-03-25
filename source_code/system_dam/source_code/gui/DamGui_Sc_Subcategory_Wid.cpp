#include "Dam_Headers_Precompiled.h"
//#include "DamGui_Sc_Subcategory_Wid.h"

//Default constructor
DamGui_Sc_Subcategory_Wid::DamGui_Sc_Subcategory_Wid(QWidget *parent): _Sys_Result_Summary_Wid(parent){

		//set number columns
	this->tableWidget->setColumnCount(3);


	QStringList my_list;
	string buffer;
	buffer="No";
	my_list.append(buffer.c_str());
	buffer="Subcategory id";
	my_list.append(buffer.c_str());
	buffer="Subcategory name";
	my_list.append(buffer.c_str());

	this->tableWidget->setHorizontalHeaderLabels(my_list);

	this->head_label->setText("Simple counting subcategory");
	this->label_id->setText("SC");

	//count the memory
	Sys_Memory_Count::self()->add_mem(sizeof(DamGui_Sc_Subcategory_Wid), _sys_system_modules::DAM_SYS);
}
//Default destructor
DamGui_Sc_Subcategory_Wid::~DamGui_Sc_Subcategory_Wid(void){

	//count the memory
	Sys_Memory_Count::self()->minus_mem(sizeof(DamGui_Sc_Subcategory_Wid), _sys_system_modules::DAM_SYS);
}
//_______
//public
//Set-up the widget
void DamGui_Sc_Subcategory_Wid::set_up_widget(QSqlDatabase *ptr_database , const _sys_system_id ){
	//detailed results
	QSqlQueryModel model;
	int number=0;

	number=Dam_Sc_Subcategory::select_relevant_subcategory_database(&model, ptr_database);

	if(number==0){
		this->setEnabled(false);
		this->setToolTip("No results found!");
		return;
	}

	this->tableWidget->setRowCount(number);

	//init table widget
	QTableWidgetItem *item;
	Qt::ItemFlags my_flag;
	my_flag=my_flag|(Qt::ItemIsEnabled);
	my_flag=my_flag|(Qt::ItemIsSelectable);
	my_flag=my_flag|(Qt::ItemIsDragEnabled);

	for(int i=0; i<this->tableWidget->rowCount(); i++){
		for(int j=0; j<this->tableWidget->columnCount(); j++){
			item=new QTableWidgetItem;
			item->setFlags(my_flag);
			this->tableWidget->setItem(i,j,item);
			this->tableWidget->item(i,j)->setData(Qt::DisplayRole, "-");
	
		}
	}



	QVariant buffer;
	for(int i=0; i< number; i++){
		buffer=i+1;
		this->tableWidget->item(i,0)->setData(Qt::DisplayRole, buffer);

		buffer=model.record(i).value((Dam_Sc_Subcategory::table->get_column_name(dam_label::subcategory_id)).c_str()).toInt();
		this->tableWidget->item(i,1)->setData(Qt::DisplayRole, buffer);

		buffer=model.record(i).value((Dam_Sc_Subcategory::table->get_column_name(dam_label::category_name)).c_str()).toString();
		this->tableWidget->item(i,2)->setData(Qt::DisplayRole, buffer);
	}

	this->tableWidget->resizeColumnsToContents();
}
