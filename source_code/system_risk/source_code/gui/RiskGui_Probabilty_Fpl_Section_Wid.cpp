#include "Risk_Headers_Precompiled.h"
//#include "RiskGui_Probabilty_Fpl_Section_Wid.h"


//Default constructor
RiskGui_Probabilty_Fpl_Section_Wid::RiskGui_Probabilty_Fpl_Section_Wid(QWidget *parent): _Sys_Result_Summary_Wid(parent){

	//set number columns
	this->tableWidget->setColumnCount(6);

	QStringList my_list;
	string buffer;
	buffer="No";
	my_list.append(buffer.c_str());
	buffer="HYD-boundary id";
	my_list.append(buffer.c_str());
	buffer="HYD-boundary name";
	my_list.append(buffer.c_str());
	buffer="FPL-section id";
	my_list.append(buffer.c_str());
	buffer="FPL-section name";
	my_list.append(buffer.c_str());
	buffer="Probability [-]";
	my_list.append(buffer.c_str());
	

	this->tableWidget->setHorizontalHeaderLabels(my_list);
	this->head_label->setText("Integrated probability of FPL-section failure");


	//count the memory
	Sys_Memory_Count::self()->add_mem(sizeof(RiskGui_Probabilty_Fpl_Section_Wid), _sys_system_modules::RISK_SYS);
}
//Default destructor
RiskGui_Probabilty_Fpl_Section_Wid::~RiskGui_Probabilty_Fpl_Section_Wid(void){

	//count the memory
	Sys_Memory_Count::self()->minus_mem(sizeof(RiskGui_Probabilty_Fpl_Section_Wid), _sys_system_modules::RISK_SYS);
}
//_______
//public
//Set-up the widget
void RiskGui_Probabilty_Fpl_Section_Wid::set_up_widget(QSqlDatabase *ptr_database, const _sys_system_id id){
	//detailed results
	QSqlQueryModel model;
	
	//set the hyd-boundary scenarios
	Hyd_Boundary_Szenario_Management hyd_sc;
	hyd_sc.set_systemid(id);
	hyd_sc.set_szenario_per_db(ptr_database);


	//set the fpl-section
	QSqlQueryModel fpl_result;
	int number_sec=0;
	Fpl_Section *my_section=NULL;
	double *sum_section=NULL;

	
	number_sec=Fpl_Section::select_relevant_section_database(&fpl_result, ptr_database, id, false);
		
	

	try{
		my_section=new Fpl_Section[number_sec];
		sum_section=new double[number_sec];
		for(int i=0; i< number_sec; i++){
			my_section[i].set_systemid(id);
			sum_section[i]=0.0;
		}
	}
	catch(bad_alloc &t){
		Error msg=this->set_error(0);
		ostringstream info;
		info<< "Info bad alloc: " << t.what() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
	
	//set the section
	for(int i=0; i< number_sec;i++){
		my_section[i].input_section_perdatabase(&fpl_result, i, ptr_database);
	}


	//set rows
	this->tableWidget->setRowCount(number_sec*(hyd_sc.get_number_sz()+1));

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
	int counter=0;
	QColor my_color1(Qt::lightGray);
	for(int i=0; i< hyd_sc.get_number_sz(); i++){
		for(int j=0; j< number_sec; j++){
			
			Risk_Break_Scenario_Management::select_data_in_table_fpl_sec_prob(&model, ptr_database, id, hyd_sc.get_ptr_sz(i)->get_id(), my_section[j].get_id_section());
			//set data to table wid
			buffer=counter+1;
			this->tableWidget->item(counter,0)->setData(Qt::DisplayRole, buffer);
			if(i % 2!=0){
				this->tableWidget->item(counter,0)->setBackgroundColor(my_color1);
			}

			buffer=hyd_sc.get_ptr_sz(i)->get_id();
			this->tableWidget->item(counter,1)->setData(Qt::DisplayRole, buffer);
			if(i % 2!=0){
				this->tableWidget->item(counter,1)->setBackgroundColor(my_color1);
			}
			
			buffer=hyd_sc.get_ptr_sz(i)->get_name().c_str();
			this->tableWidget->item(counter,2)->setData(Qt::DisplayRole, buffer);
			if(i % 2!=0){
				this->tableWidget->item(counter,2)->setBackgroundColor(my_color1);
			}

			buffer=my_section[j].get_id_section();
			this->tableWidget->item(counter,3)->setData(Qt::DisplayRole, buffer);
			if(i % 2!=0){
				this->tableWidget->item(counter,3)->setBackgroundColor(my_color1);
			}

			buffer=my_section[j].get_name_section().c_str();
			this->tableWidget->item(counter,4)->setData(Qt::DisplayRole, buffer);
			if(i % 2!=0){
				this->tableWidget->item(counter,4)->setBackgroundColor(my_color1);
			}

			buffer=model.record(0).value((Risk_Break_Scenario_Management::table_fpl_sec_prob->get_column_name(risk_label::sz_break_prob)).c_str()).toDouble();;
			this->tableWidget->item(counter,5)->setData(Qt::DisplayRole, buffer);
			if(i % 2!=0){
				this->tableWidget->item(counter,5)->setBackgroundColor(my_color1);
			}
			sum_section[j]=sum_section[j]+buffer.toDouble()*hyd_sc.get_ptr_sz(i)->get_prob_year()*hyd_sc.get_ptr_sz(i)->get_prob_occurence();

			counter++;
		}
	}

	QColor my_color2(Qt::yellow);
	//set total probability
	for(int i=0; i< number_sec; i++){
		buffer=counter+1;
		this->tableWidget->item(counter,0)->setData(Qt::DisplayRole, buffer);
		this->tableWidget->item(counter,0)->setBackgroundColor(my_color2);

		buffer="-";
		this->tableWidget->item(counter,1)->setData(Qt::DisplayRole, buffer);
		this->tableWidget->item(counter,1)->setBackgroundColor(my_color2);
		buffer="total";
		this->tableWidget->item(counter,2)->setData(Qt::DisplayRole, buffer);
		this->tableWidget->item(counter,2)->setBackgroundColor(my_color2);

		buffer=my_section[i].get_id_section();
		this->tableWidget->item(counter,3)->setData(Qt::DisplayRole, buffer);
		this->tableWidget->item(counter,3)->setBackgroundColor(my_color2);
		

		buffer=my_section[i].get_name_section().c_str();
		this->tableWidget->item(counter,4)->setData(Qt::DisplayRole, buffer);
		this->tableWidget->item(counter,4)->setBackgroundColor(my_color2);

		buffer=sum_section[i];
		this->tableWidget->item(counter,5)->setData(Qt::DisplayRole, buffer);
		this->tableWidget->item(counter,5)->setBackgroundColor(my_color2);
		

		counter++;
	}

	//delete fpl_section
	if(my_section!=NULL){
		delete []my_section;
		my_section=NULL;
	}
	if(sum_section!=NULL){
		delete []sum_section;
		sum_section=NULL;
	}

	this->tableWidget->resizeColumnsToContents();

}
//_________
//private
//Set error(s)
Error RiskGui_Probabilty_Fpl_Section_Wid::set_error(const int err_type){
		string place="RiskGui_Probabilty_Fpl_Section_Wid::";
		string help;
		string reason;
		int type=0;
		bool fatal=false;
		stringstream info;
		Error msg;

	switch (err_type){
		case 0://bad_alloc
			place.append("set_up_widget(QSqlDatabase *ptr_database, const _sys_system_id id)");
			reason="Can not allocate the memory";
			help="Check the memory";
			type=10;
			break;
		default:
			place.append("set_error(const int err_type)");
			reason ="Unknown flag!";
			help="Check the flags";
			type=6;
	}
	msg.set_msg(place, reason, help, type, fatal);
	msg.make_second_info(info.str());
	return msg;
}