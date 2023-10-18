#include "Madm_Headers_Precompiled.h"
//#include "MadmGui_Matrix_Summary_Wid.h"

//Default constructor
MadmGui_Matrix_Summary_Wid::MadmGui_Matrix_Summary_Wid(QWidget *parent): _Sys_Result_Summary_Wid(parent){
	


	//set number columns
	this->tableWidget->setColumnCount(15);

	QStringList my_list;
	string buffer;
	buffer="No";
	my_list.append(buffer.c_str());
	buffer="Areastate";
	my_list.append(buffer.c_str());
	buffer="Measurestate";
	my_list.append(buffer.c_str());
	buffer="Measure name";
	my_list.append(buffer.c_str());
	buffer="Delta risk ECN [monteray/a]";
	my_list.append(buffer.c_str());
	buffer="Delta risk EC0 [monteray/a]";
	my_list.append(buffer.c_str());
	buffer="Delta risk POP affected [person/a]";
	my_list.append(buffer.c_str());
	buffer="Delta risk POP endangered [person/a]";
	my_list.append(buffer.c_str());
	buffer="Delta risk PYS [score/a]";
	my_list.append(buffer.c_str());

	buffer="Delta risk SC Public buildings [score/a]";
	my_list.append(buffer.c_str());
	buffer="Delta risk SC Ecologic perilous sites/buildings [score/a]";
	my_list.append(buffer.c_str());
	buffer="Delta risk SC Cultural heritage [score/a]";
	my_list.append(buffer.c_str());
	buffer="Delta risk SC Buildings with highly vulnerable person [score/a]";
	my_list.append(buffer.c_str());


	buffer="Delta risk OUTFLOW [(m³/s)/a]";
	my_list.append(buffer.c_str());
	buffer="Cost ECN [monetary]";
	my_list.append(buffer.c_str());

	this->tableWidget->setHorizontalHeaderLabels(my_list);
	this->risk_type=_risk_type::nobreak_risk;

	this->head_label->setText("Decision matrix");

	//count the memory
	Sys_Memory_Count::self()->add_mem(sizeof(MadmGui_Matrix_Summary_Wid), _sys_system_modules::MADM_SYS);

}
MadmGui_Matrix_Summary_Wid::~MadmGui_Matrix_Summary_Wid(void){
	//count the memory
	Sys_Memory_Count::self()->minus_mem(sizeof(MadmGui_Matrix_Summary_Wid), _sys_system_modules::MADM_SYS);
}
//_______
//public
//Set-up the widget
void MadmGui_Matrix_Summary_Wid::set_up_widget(QSqlDatabase *ptr_database, const _sys_system_id){


	//detailed results
	QSqlQueryModel model;
	int number=0;

	number=Madm_Decision_Matrix::select_data_in_table_matrix(&model, ptr_database, this->risk_type);

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

	QColor my_color1(Qt::red);
	QColor my_color2(Qt::green);
	QColor my_color3(Qt::magenta);
	QColor my_color4(Qt::darkMagenta);
	QColor my_color5(Qt::cyan);
	QColor my_color6(Qt::yellow);
	QColor my_color7(Qt::lightGray);

	for(int i=0; i<this->tableWidget->rowCount(); i++){
		for(int j=0; j<this->tableWidget->columnCount(); j++){
			item=new QTableWidgetItem;
			item->setFlags(my_flag);
			this->tableWidget->setItem(i,j,item);

			if(j==4){
				this->tableWidget->item(i,j)->setBackgroundColor(my_color1);
			}
			else if(j==5){
				this->tableWidget->item(i,j)->setBackgroundColor(my_color2);
			}

			else if(j>=6 && j<=7){
				this->tableWidget->item(i,j)->setBackgroundColor(my_color3);	

			}
			else if(j==8){
				this->tableWidget->item(i,j)->setBackgroundColor(my_color4);
			}
			else if(j>=9 && j<=12){
				this->tableWidget->item(i,j)->setBackgroundColor(my_color7);
			}
			else if(j==13){
				this->tableWidget->item(i,j)->setBackgroundColor(my_color5);
			}
			else if(j==14){
				this->tableWidget->item(i,j)->setBackgroundColor(my_color6);
			}
		}
	}

	QVariant buffer;
	_sys_system_id buff_id;
	buff_id.area_state=0;
	buff_id.measure_nr=0;


	for(int i=0; i< number; i++){
		buffer=i+1;
		this->tableWidget->item(i,0)->setData(Qt::DisplayRole, buffer);	

		buff_id.area_state=model.record(i).value((Madm_Decision_Matrix::table_matrix->get_column_name(label::areastate_id)).c_str()).toInt();
		buffer=buff_id.area_state;
		this->tableWidget->item(i,1)->setData(Qt::DisplayRole, buffer);
		
		buff_id.measure_nr=model.record(i).value((Madm_Decision_Matrix::table_matrix->get_column_name(label::measure_id)).c_str()).toInt();
		buffer=buff_id.measure_nr;
		this->tableWidget->item(i,2)->setData(Qt::DisplayRole, buffer);
		string name;
		name=label::not_defined;
		string descr;
		descr=label::not_defined;
		if(buff_id.measure_nr!=0){
			Alt_System::get_name_descript_measure_table(ptr_database, buff_id, &name, &descr);
			buffer=name.c_str();
			this->tableWidget->item(i,3)->setData(Qt::DisplayRole, buffer);
		}
		else{
			buffer="base-state";
			this->tableWidget->item(i,3)->setData(Qt::DisplayRole, buffer);
		}

		buffer=model.record(i).value((Madm_Decision_Matrix::table_matrix->get_column_name(madm_label::crit_risk_ecn)).c_str());
		this->tableWidget->item(i,4)->setData(Qt::DisplayRole, buffer);

		buffer=model.record(i).value((Madm_Decision_Matrix::table_matrix->get_column_name(madm_label::crit_risk_eco)).c_str());
		this->tableWidget->item(i,5)->setData(Qt::DisplayRole, buffer);

		buffer=model.record(i).value((Madm_Decision_Matrix::table_matrix->get_column_name(madm_label::crit_risk_pop_aff)).c_str());
		this->tableWidget->item(i,6)->setData(Qt::DisplayRole, buffer);

		buffer=model.record(i).value((Madm_Decision_Matrix::table_matrix->get_column_name(madm_label::crit_risk_pop_dan)).c_str());
		this->tableWidget->item(i,7)->setData(Qt::DisplayRole, buffer);

		buffer=model.record(i).value((Madm_Decision_Matrix::table_matrix->get_column_name(madm_label::crit_risk_pys)).c_str());
		this->tableWidget->item(i,8)->setData(Qt::DisplayRole, buffer);

		buffer=model.record(i).value((Madm_Decision_Matrix::table_matrix->get_column_name(madm_label::crit_risk_sc_pub)).c_str());
		this->tableWidget->item(i,9)->setData(Qt::DisplayRole, buffer);
		buffer=model.record(i).value((Madm_Decision_Matrix::table_matrix->get_column_name(madm_label::crit_risk_sc_eco)).c_str());
		this->tableWidget->item(i,10)->setData(Qt::DisplayRole, buffer);
		buffer=model.record(i).value((Madm_Decision_Matrix::table_matrix->get_column_name(madm_label::crit_risk_sc_cult)).c_str());
		this->tableWidget->item(i,11)->setData(Qt::DisplayRole, buffer);
		buffer=model.record(i).value((Madm_Decision_Matrix::table_matrix->get_column_name(madm_label::crit_risk_sc_person)).c_str());
		this->tableWidget->item(i,12)->setData(Qt::DisplayRole, buffer);	


		buffer=model.record(i).value((Madm_Decision_Matrix::table_matrix->get_column_name(madm_label::crit_risk_outflow)).c_str());
		this->tableWidget->item(i,13)->setData(Qt::DisplayRole, buffer);

		buffer=model.record(i).value((Madm_Decision_Matrix::table_matrix->get_column_name(madm_label::crit_cost_ecn)).c_str());
		this->tableWidget->item(i,14)->setData(Qt::DisplayRole, buffer);

	}




	this->tableWidget->resizeColumnsToContents();
	
}
//Set the risk type
void MadmGui_Matrix_Summary_Wid::set_risk_type(const _risk_type type){
	this->risk_type=type;
}