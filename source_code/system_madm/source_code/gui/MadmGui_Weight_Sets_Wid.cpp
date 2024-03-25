#include "Madm_Headers_Precompiled.h"
//#include "MadmGui_Weight_Sets_Wid.h"


//Default constructor
MadmGui_Weight_Sets_Wid::MadmGui_Weight_Sets_Wid(QWidget *parent): _Sys_Data_Wid(parent){
	this->setupUi(this);
	this->area2print=this->tableWidget;

	//set number columns
	this->tableWidget->setColumnCount(22);

	QStringList my_list;
	string buffer;
	buffer="Id";
	my_list.append(buffer.c_str());
	buffer="Name";
	my_list.append(buffer.c_str());
	buffer="Delta risk ECN";
	my_list.append(buffer.c_str());
	buffer="Delta risk EC0";
	my_list.append(buffer.c_str());
	buffer="Delta risk POP affected";
	my_list.append(buffer.c_str());
	buffer="Delta risk POP endangered";
	my_list.append(buffer.c_str());
	buffer="Delta risk PYS";
	my_list.append(buffer.c_str());
	buffer="Delta risk SC Public buildings";
	my_list.append(buffer.c_str());
	buffer="Delta risk SC Ecologic perilous sites/buildings";
	my_list.append(buffer.c_str());
	buffer="Delta risk SC Cultural heritages";
	my_list.append(buffer.c_str());
	buffer="Delta risk SC Buildings with highly vulnerable person";
	my_list.append(buffer.c_str());


	buffer = "Delta risk CI Population time Sector Electricity";
	my_list.append(buffer.c_str());
	buffer = "Delta risk CI Population time Sector Information technology";
	my_list.append(buffer.c_str());

	buffer = "Delta risk CI Population time Sector Water supply";
	my_list.append(buffer.c_str());
	buffer = "Delta risk CI Population time Sector Water treatment";
	my_list.append(buffer.c_str());
	buffer = "Delta risk CI Population time Sector Energy";
	my_list.append(buffer.c_str());

	buffer = "Delta risk CI Population time Cum-Sector Health";
	my_list.append(buffer.c_str());

	buffer = "Delta risk CI Population time Cum-Sector Social";
	my_list.append(buffer.c_str());

	buffer = "Delta risk CI Population time Cum-Sector Material";
	my_list.append(buffer.c_str());










	buffer="Delta risk OUTFLOW";
	my_list.append(buffer.c_str());
	buffer="Cost ECN";
	my_list.append(buffer.c_str());
	buffer="Description";
	my_list.append(buffer.c_str());

	this->tableWidget->setHorizontalHeaderLabels(my_list);
	this->tableWidget->setSelectionMode(QAbstractItemView::ExtendedSelection);
	this->tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
	this->tableWidget->setContextMenuPolicy(Qt::DefaultContextMenu);

	//count the memory
	Sys_Memory_Count::self()->add_mem(sizeof(MadmGui_Weight_Sets_Wid), _sys_system_modules::MADM_SYS);
}
//Default destructor
MadmGui_Weight_Sets_Wid::~MadmGui_Weight_Sets_Wid(void){

	//count the memory
	Sys_Memory_Count::self()->minus_mem(sizeof(MadmGui_Weight_Sets_Wid), _sys_system_modules::MADM_SYS);
}
//______
//public
//Set-up the widget
void MadmGui_Weight_Sets_Wid::set_up_widget(QSqlDatabase *ptr_database){

	int number_rows=0;
	QSqlQueryModel model;
	///Select the data in the database table for weighting sets 
	number_rows=Madm_System::select_data_in_settable(&model, ptr_database);


	this->tableWidget->setRowCount(number_rows);
	//init table widget
	QTableWidgetItem *item;
	Qt::ItemFlags my_flag;
	my_flag=my_flag|(Qt::ItemIsEnabled);
	my_flag=my_flag|(Qt::ItemIsSelectable);
	my_flag=my_flag|(Qt::ItemIsDragEnabled);

	QColor my_color1(Qt::red);
	QColor my_color2(Qt::green);
	QColor my_color3(Qt::magenta);
	QColor my_color4(Qt::darkMagenta);
	QColor my_color5(Qt::cyan);
	QColor my_color6(Qt::yellow);
	QColor my_color7(Qt::lightGray);
	QColor my_color8(Qt::darkYellow);

	for(int i=0; i<this->tableWidget->rowCount(); i++){
		for(int j=0; j<this->tableWidget->columnCount(); j++){
			item=new QTableWidgetItem;
			item->setFlags(my_flag);
			
			this->tableWidget->setItem(i,j,item);
			
			if(j==2){
				this->tableWidget->item(i,j)->setBackgroundColor(my_color1);
			}
			else if(j==3){
				this->tableWidget->item(i,j)->setBackgroundColor(my_color2);
			}
			else if(j>=4 && j<=5){
				this->tableWidget->item(i,j)->setBackgroundColor(my_color3);
			}
			else if(j==6){
				this->tableWidget->item(i,j)->setBackgroundColor(my_color4);
			}
			else if(j>=7 && j<=10){
				this->tableWidget->item(i,j)->setBackgroundColor(my_color7);
			}
			else if (j >= 11 && j <= 18) {
				this->tableWidget->item(i, j)->setBackgroundColor(my_color8);
			}

			else if(j==19){
				this->tableWidget->item(i,j)->setBackgroundColor(my_color5);
			}
			else if(j==20){
				this->tableWidget->item(i,j)->setBackgroundColor(my_color6);
			}
		}
	}


	QVariant buffer;
	for(int i=0; i<this->tableWidget->rowCount(); i++){
		buffer=model.record(i).value((Madm_System::table_sets->get_column_name(madm_label::set_id)).c_str());
		this->tableWidget->item(i,0)->setData(Qt::DisplayRole, buffer);
		//this->tableWidget->item(i,0)->setFlags(Qt::ItemIsEditable);
		buffer=model.record(i).value((Madm_System::table_sets->get_column_name(madm_label::set_name)).c_str());
		this->tableWidget->item(i,1)->setData(Qt::DisplayRole, buffer);
		buffer=model.record(i).value((Madm_System::table_sets->get_column_name(madm_label::crit_risk_ecn)).c_str());
		this->tableWidget->item(i,2)->setData(Qt::DisplayRole, buffer);
		buffer=model.record(i).value((Madm_System::table_sets->get_column_name(madm_label::crit_risk_eco)).c_str());
		this->tableWidget->item(i,3)->setData(Qt::DisplayRole, buffer);
		buffer=model.record(i).value((Madm_System::table_sets->get_column_name(madm_label::crit_risk_pop_aff)).c_str());
		this->tableWidget->item(i,4)->setData(Qt::DisplayRole, buffer);
		buffer=model.record(i).value((Madm_System::table_sets->get_column_name(madm_label::crit_risk_pop_dan)).c_str());
		this->tableWidget->item(i,5)->setData(Qt::DisplayRole, buffer);
		buffer=model.record(i).value((Madm_System::table_sets->get_column_name(madm_label::crit_risk_pys)).c_str());
		this->tableWidget->item(i,6)->setData(Qt::DisplayRole, buffer);
		
		buffer=model.record(i).value((Madm_System::table_sets->get_column_name(madm_label::crit_risk_sc_pub)).c_str());
		this->tableWidget->item(i,7)->setData(Qt::DisplayRole, buffer);
		buffer=model.record(i).value((Madm_System::table_sets->get_column_name(madm_label::crit_risk_sc_eco)).c_str());
		this->tableWidget->item(i,8)->setData(Qt::DisplayRole, buffer);
		buffer=model.record(i).value((Madm_System::table_sets->get_column_name(madm_label::crit_risk_sc_cult)).c_str());
		this->tableWidget->item(i,9)->setData(Qt::DisplayRole, buffer);
		buffer=model.record(i).value((Madm_System::table_sets->get_column_name(madm_label::crit_risk_sc_person)).c_str());
		this->tableWidget->item(i,10)->setData(Qt::DisplayRole, buffer);

		buffer = model.record(i).value((Madm_System::table_sets->get_column_name(madm_label::crit_risk_ci_elect_pt)).c_str());
		this->tableWidget->item(i, 11)->setData(Qt::DisplayRole, buffer);
		buffer = model.record(i).value((Madm_System::table_sets->get_column_name(madm_label::crit_risk_ci_info_tec_pt)).c_str());
		this->tableWidget->item(i, 12)->setData(Qt::DisplayRole, buffer);
		buffer = model.record(i).value((Madm_System::table_sets->get_column_name(madm_label::crit_risk_ci_water_sup_pt)).c_str());
		this->tableWidget->item(i, 13)->setData(Qt::DisplayRole, buffer);
		buffer = model.record(i).value((Madm_System::table_sets->get_column_name(madm_label::crit_risk_ci_water_treat_pt)).c_str());
		this->tableWidget->item(i, 14)->setData(Qt::DisplayRole, buffer);
		buffer = model.record(i).value((Madm_System::table_sets->get_column_name(madm_label::crit_risk_ci_energy_pt)).c_str());
		this->tableWidget->item(i, 15)->setData(Qt::DisplayRole, buffer);
		buffer = model.record(i).value((Madm_System::table_sets->get_column_name(madm_label::crit_risk_ci_health_pt)).c_str());
		this->tableWidget->item(i, 16)->setData(Qt::DisplayRole, buffer);
		buffer = model.record(i).value((Madm_System::table_sets->get_column_name(madm_label::crit_risk_ci_social_pt)).c_str());
		this->tableWidget->item(i, 17)->setData(Qt::DisplayRole, buffer);
		buffer = model.record(i).value((Madm_System::table_sets->get_column_name(madm_label::crit_risk_ci_mat_pt)).c_str());
		this->tableWidget->item(i, 18)->setData(Qt::DisplayRole, buffer);




		buffer=model.record(i).value((Madm_System::table_sets->get_column_name(madm_label::crit_risk_outflow)).c_str());
		this->tableWidget->item(i,19)->setData(Qt::DisplayRole, buffer);
		buffer=model.record(i).value((Madm_System::table_sets->get_column_name(madm_label::crit_cost_ecn)).c_str());
		this->tableWidget->item(i,20)->setData(Qt::DisplayRole, buffer);
		buffer=model.record(i).value((Madm_System::table_sets->get_column_name(label::description)).c_str());
		this->tableWidget->item(i,21)->setData(Qt::DisplayRole, buffer);
	}

	this->tableWidget->resizeColumnsToContents();
}