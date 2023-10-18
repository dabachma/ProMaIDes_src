#include "Madm_Headers_Precompiled.h"
//#include "MadmGui_Rank_Wid.h"


//Default constructor
MadmGui_Rank_Wid::MadmGui_Rank_Wid(QWidget *parent): _Sys_Data_Wid(parent){


	this->setupUi(this);
	this->area2print=this->tableWidget;

	//set number columns
	this->tableWidget->setColumnCount(4);

	QStringList my_list;
	string buffer;
	buffer="Rank";
	my_list.append(buffer.c_str());
	buffer="Id";
	my_list.append(buffer.c_str());
	buffer="Name";
	my_list.append(buffer.c_str());
	buffer="Score";
	my_list.append(buffer.c_str());

	this->tableWidget->setHorizontalHeaderLabels(my_list);

	this->tableWidget->setSelectionMode(QAbstractItemView::ExtendedSelection);
	this->tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
	this->tableWidget->setContextMenuPolicy(Qt::DefaultContextMenu);

	//count the memory
	Sys_Memory_Count::self()->add_mem(sizeof(MadmGui_Rank_Wid), _sys_system_modules::MADM_SYS);
}
//Default destructor
MadmGui_Rank_Wid::~MadmGui_Rank_Wid(void){

	//count the memory
	Sys_Memory_Count::self()->minus_mem(sizeof(MadmGui_Rank_Wid), _sys_system_modules::MADM_SYS);
}
//______
//public
//Set-up the widget
void MadmGui_Rank_Wid::set_up_widget(QSqlDatabase *ptr_database, const int areastate, const int set_id, const _risk_type risktype, const _madm_approach_type ana_type){

	QSqlQueryModel model;

	int number_row;
	number_row=_Madm_Solver::select_data_in_ranking_table(&model, ptr_database, areastate, set_id, risktype, ana_type);
	
	this->tableWidget->setRowCount(number_row);
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
		}
	}


	Alt_System my_alt;
	my_alt.load_list_current_alt(ptr_database, areastate);


	QVariant buffer;
	for(int i=0; i<this->tableWidget->rowCount(); i++){
		buffer=model.record(i).value((_Madm_Solver::table_ranking->get_column_name(madm_label::rank)).c_str());
		this->tableWidget->item(i,0)->setData(Qt::DisplayRole, buffer);
		buffer=model.record(i).value((_Madm_Solver::table_ranking->get_column_name(label::measure_id)).c_str());
		this->tableWidget->item(i,1)->setData(Qt::DisplayRole, buffer);
		if(buffer.toInt()!=0){
			buffer=my_alt.get_ptr_alt_info_by_id(buffer.toInt())->name.c_str();
		}
		else{
			buffer="base-state";
		}
		this->tableWidget->item(i,2)->setData(Qt::DisplayRole, buffer);
		buffer=model.record(i).value((_Madm_Solver::table_ranking->get_column_name(madm_label::score)).c_str());
		this->tableWidget->item(i,3)->setData(Qt::DisplayRole, buffer);
		
	}

	this->tableWidget->resizeColumnsToContents();
}
