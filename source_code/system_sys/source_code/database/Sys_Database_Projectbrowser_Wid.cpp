#include "Sys_Headers_Precompiled.h"
//#include "Sys_Database_Projectbrowser_Wid.h"

//Default constructor
Sys_Database_Projectbrowser_Wid::Sys_Database_Projectbrowser_Wid(QWidget *parent) : QWidget(parent){
	this->ptr_database=NULL;
	this->setupUi(this);
	this->loader_thread=NULL;
	this->allocate_table_loader();
	this->ptr_table_view=NULL;
	this->ptr_status_msg=NULL;
	this->clearer.setColumnCount(1);
	this->system_id=NULL;
	QStringList hhLabels;
	hhLabels << tr("Loading...");
	this->clearer.setHorizontalHeaderLabels(hhLabels);

	QPalette p;
	p.setColor(QPalette::Active,QPalette::Base, Qt::lightGray);
	p.setColor(QPalette::Inactive,QPalette::Base, Qt::lightGray);
    this->sqlEdit_column->setPalette(p);

	this->stopbutton_clicked=false;


	this->sql_help_cond_wid.close();
	this->sql_help_row_wid.close();

	QObject::connect(this, SIGNAL(statusMessage(const QString )), this, SLOT(display_status_msg(const QString )));
	QObject::connect(this->projecttreewid, SIGNAL(tableClicked(const QString)), this, SLOT(set_table_txt_label(const QString )));
	emit statusMessage(tr("Disconnected from database"));

	QObject::connect(this->projecttreewid, SIGNAL(tableClicked(const QString)), this, SLOT(set_row_name2helper(const QString )));


	QObject::connect(this->pushButton_help_row, SIGNAL(clicked()), this, SLOT(show_sql_row_helper()));
	QObject::connect(this->pushButton_help_condition, SIGNAL(clicked()), this, SLOT(show_sql_condition_helper()));
	

	QObject::connect(&this->sql_help_row_wid, SIGNAL(send_display_txt(QString)), this, SLOT(set_text2row_display(QString)));
	QObject::connect(&this->sql_help_cond_wid, SIGNAL(send_display_txt(QString)), this, SLOT(set_text2cond_display(QString)));

	this->stopButton->setEnabled(false);

	Sys_Memory_Count::self()->add_mem(sizeof(Sys_Database_Projectbrowser_Wid), _sys_system_modules::SYS_SYS);
}
//Default destructor
Sys_Database_Projectbrowser_Wid::~Sys_Database_Projectbrowser_Wid(void){
	
	this->sql_help_cond_wid.close();
	this->sql_help_row_wid.close();
	this->delete_table_loader();
	Sys_Memory_Count::self()->minus_mem(sizeof(Sys_Database_Projectbrowser_Wid), _sys_system_modules::SYS_SYS);
}
//_____________
//public
//Set a database connection
void Sys_Database_Projectbrowser_Wid::set_db_connection(QSqlDatabase *ptr_database){
	if(ptr_database!=NULL){
		this->ptr_database=ptr_database;
		this->projecttreewid->refresh();
		emit statusMessage(tr("Connected to database"));
		this->setEnabled(true);
	}	 
}
//Disconnect the browser from the database and clear all views
void Sys_Database_Projectbrowser_Wid::disconnect_db(void){
	//send to clear the view
	emit send_db_clear();
	this->delete_table_loader();
	this->allocate_table_loader();
	//clear the sql-edit
	this->sqlEdit_column->clear();
	this->sqlEdit_column->setText("*");
	this->sqlEdit_rest->clear();
	//clear the tree view
	this->projecttreewid->clear_project_tree();
	emit statusMessage(tr("Disconnected from database"));
	this->ptr_database=NULL;
	//clear the table text-label
	this->label_table->clear();
	this->label_table->setText("from ");
	this->setEnabled(false);
}
//Refresh the project-tree view
void Sys_Database_Projectbrowser_Wid::refresh_project_tree(void){
	this->projecttreewid->refresh();
}
//Set the pointer to the table model
void Sys_Database_Projectbrowser_Wid::set_ptr_table_model(Sys_Sql_Table_Wid *ptr_table_view){
	this->ptr_table_view=ptr_table_view;
	
}
//Set the pointer to the status bar message line
void Sys_Database_Projectbrowser_Wid::set_ptr_status_msg(QTextEdit *ptr_status_msg){
	this->ptr_status_msg=ptr_status_msg;
}
//Get the pointer to the table loader thread
QThread* Sys_Database_Projectbrowser_Wid::get_ptr_table_loader_thread(void){
	return this->loader_thread;
}
//Get the pointer to the tree widget
Sys_Database_Project_Tree_Wid * Sys_Database_Projectbrowser_Wid::get_ptr_tree_wid(void){
	return this->projecttreewid;
}
//Set a pointer to the system id 
void Sys_Database_Projectbrowser_Wid::set_ptr_sys_system_id(_sys_system_id *id){
	this->system_id=id;
}
//_____________
//public slots
//Set and show the sql-query
void Sys_Database_Projectbrowser_Wid::exec(void){
	if(this->ptr_database==NULL){
		return;
	}
	//set inbfos to the thread
	loader_thread->set_query_string(this->get_select_statement().c_str());
	loader_thread->set_ptr2database(this->ptr_database);
	QObject::connect(this->loader_thread, SIGNAL(finished()), this, SLOT(loading_is_finished()));
	//begin time recording	
	time(&this->start_time);
	this->ptr_table_view->setModel(&this->clearer);
	//start the loading thread
	this->loader_thread->start();
	//send message
	emit statusMessage(tr("Selection is loading..."));
	//disable windows during thread
	//this->setEnabled(false);
	this->stopbutton_clicked=false;
	this->ptr_table_view->setEnabled(false);
	this->ptr_status_msg->setEnabled(false);
	this->projecttreewid->setEnabled(false);
	this->sqlEdit_column->setEnabled(false);
	this->sqlEdit_rest->setEnabled(false);
	this->pushButton_help_condition->setEnabled(false);
	this->pushButton_help_row->setEnabled(false);
	this->submitButton->setEnabled(false);
	this->clearButton->setEnabled(false);
	this->stopButton->setEnabled(true);
	this->radioButton_all->setEnabled(false);
	this->radioButton_areastate->setEnabled(false);
	this->radioButton_measure->setEnabled(false);
}
//Show the sql table
void Sys_Database_Projectbrowser_Wid::showTable(const QString &table_str){
	if(this->ptr_database==NULL){
		return;
	}
	//set inbfos to the thread
	loader_thread->set_query_string(this->get_select_statement().c_str());
	loader_thread->set_ptr2database(this->ptr_database);
	QObject::connect(this->loader_thread, SIGNAL(finished()), this, SLOT(loading_is_finished()));
	//begin time recording	
	time(&this->start_time);
	this->ptr_table_view->setModel(&this->clearer);
	//start the loading thread
	this->loader_thread->start();
	//send message
	ostringstream buffer;
	buffer << "Table is loading...("<<table_str.toStdString()<<")";
	emit statusMessage(tr(buffer.str().c_str()));
	//disable windows during thread
	this->stopbutton_clicked=false;
	this->ptr_table_view->setEnabled(false);
	this->ptr_status_msg->setEnabled(false);
	this->projecttreewid->setEnabled(false);
	this->sqlEdit_column->setEnabled(false);
	this->sqlEdit_rest->setEnabled(false);
	this->pushButton_help_condition->setEnabled(false);
	this->pushButton_help_row->setEnabled(false);
	this->submitButton->setEnabled(false);
	this->clearButton->setEnabled(false);
	this->stopButton->setEnabled(true);
	this->radioButton_all->setEnabled(false);
	this->radioButton_areastate->setEnabled(false);
	this->radioButton_measure->setEnabled(false);

}
//Show the meta data of the database table
void Sys_Database_Projectbrowser_Wid::showMetaData(const QString &table_str){
	if(this->ptr_database==NULL){
		return;
	}
	QSqlRecord rec = this->ptr_database->record(table_str);
	QStandardItemModel *model = new QStandardItemModel(this->ptr_table_view);

    model->insertRows(0, rec.count());
    model->insertColumns(0, 7);

    model->setHeaderData(0, Qt::Horizontal, "Fieldname");
    model->setHeaderData(1, Qt::Horizontal, "Type");
    model->setHeaderData(2, Qt::Horizontal, "Length");
    model->setHeaderData(3, Qt::Horizontal, "Precision");
    model->setHeaderData(4, Qt::Horizontal, "Required");
    model->setHeaderData(5, Qt::Horizontal, "AutoValue");
    model->setHeaderData(6, Qt::Horizontal, "DefaultValue");


    for (int i = 0; i < rec.count(); ++i) {
        QSqlField fld = rec.field(i);
        model->setData(model->index(i, 0), fld.name());
        model->setData(model->index(i, 1), fld.typeID() == -1
                ? QString(QVariant::typeToName(fld.type()))
                : QString("%1 (%2)").arg(QVariant::typeToName(fld.type())).arg(fld.typeID()));
        model->setData(model->index(i, 2), fld.length());
        model->setData(model->index(i, 3), fld.precision());
        model->setData(model->index(i, 4), fld.requiredStatus() == -1 ? QVariant("?")
                : QVariant(bool(fld.requiredStatus())));
        model->setData(model->index(i, 5), fld.isAutoValue());
        model->setData(model->index(i, 6), fld.defaultValue());
    }

	this->ptr_table_view->setModel(model);

}
//Recieve if table is activated in the table-tree view
void Sys_Database_Projectbrowser_Wid::on_projecttreewid_tableActivated(const QString &table){
	this->showTable(table);
}
//Reciev if a metadata is requested in the table-tree view
void Sys_Database_Projectbrowser_Wid::on_projecttreewid_metaDataRequested(const QString &table){
	this->showMetaData(table); 
}
//Recieve the submit-command from the button
void Sys_Database_Projectbrowser_Wid::on_submitButton_clicked(void){
    this->exec();
    this->groupBox_sql->setFocus();
}
//Recieve the clear-command from the button
void Sys_Database_Projectbrowser_Wid::on_clearButton_clicked(void){
    //clear the sql-edit
	this->sqlEdit_column->clear();
	this->sqlEdit_column->setText("*");
	this->sqlEdit_rest->clear();
    this->groupBox_sql->setFocus();
	//send to clear the view
	emit send_db_clear();
	this->delete_table_loader();
	this->allocate_table_loader();
	//send a message
	emit statusMessage(tr("All is cleared"));
}
//Recieve the stop-command from the stop-button
void Sys_Database_Projectbrowser_Wid::on_stopButton_clicked(void){
	if(this->loader_thread!=NULL){
		if(this->loader_thread->isRunning()==true){
			if(this->loader_thread->ptr_model!=NULL){
				
				this->loader_thread->ptr_model->clear();
				this->stopbutton_clicked=true;
				emit statusMessage(tr("Loading is stopped..."));
				this->loader_thread->terminate();
				this->loader_thread->qsqldatabase.close();
				this->loader_thread->qsqldatabase.open();
			}
			
		}
	}
}
//Recieve a satus message and display it to the status text-line
void Sys_Database_Projectbrowser_Wid::display_status_msg(const QString message){
	if(this->ptr_status_msg!=NULL){
		this->ptr_status_msg->append(message);
		this->ptr_status_msg->moveCursor(QTextCursor::End, QTextCursor::MoveAnchor);
	}
}
//Recieve a marked table name and set it to the table-text label
void Sys_Database_Projectbrowser_Wid::set_table_txt_label(const QString &table){
	ostringstream buffer;
	buffer <<"from " << table.toStdString();
	this->label_table->clear();
	this->label_table->setText(buffer.str().c_str());
	this->sqlEdit_column->clear();
	this->sqlEdit_column->setText("*");
	this->sqlEdit_rest->clear();
}
//Set text to the row display
void Sys_Database_Projectbrowser_Wid::set_text2row_display(QString txt){
	this->sqlEdit_column->clear();
	this->sqlEdit_column->setText(txt);
}
//Set text to the condition display
void Sys_Database_Projectbrowser_Wid::set_text2cond_display(QString txt){
	this->sqlEdit_rest->clear();
	this->sqlEdit_rest->setText(txt);
}
//Set table row names to helper widgets
void Sys_Database_Projectbrowser_Wid::set_row_name2helper(QString table_name){
		
	QSqlRecord column;
	if(table_name.isEmpty()==false){
		//Data_Base::database_table_columns(&column, table_name, this->ptr_database);
		QString buff = table_name.split('.').at(1);
		Data_Base::database_table_columns_query(&column, Sys_Project::get_complete_project_database_schemata_name(), buff.toStdString(), this->ptr_database);
	}

	QStringList buffer;
	for(int i=0; i< column.count(); i++){
		buffer.append(column.fieldName(i));
	}
	this->sql_help_row_wid.set_list_column_name(&buffer);
	this->sql_help_cond_wid.set_list_column_name(&buffer);

}
//_____________
//private 
///Get select statement
string Sys_Database_Projectbrowser_Wid::get_select_statement(void){
	string str;
	ostringstream buffer;


	buffer <<"Select " << this->sqlEdit_column->toPlainText().toStdString() << " ";
	buffer <<this->label_table->text().toStdString() << " " ;
	


	//use filter for areastate and measurestate
	string buff_table_name;
	string buff_where_statement;
	QSqlRecord column;
	buff_table_name=this->label_table->text().toStdString();
	buff_table_name=buff_table_name.substr(5);
	column=(ptr_database->record(buff_table_name.c_str()));
	buff_where_statement=this->sqlEdit_rest->toPlainText().toStdString();

	ostringstream buff1;
	buff1 << " WHERE ";
	bool where_found=false;
	bool adding_required=false;


	if(buff_where_statement.empty()==false){
		int pos=0;
		QString buff2=buff_where_statement.c_str();
		pos=buff2.indexOf("where",0,Qt::CaseInsensitive);
		if(pos>=0){
			where_found=true;
			buff2=buff2.remove(pos, 5);
		}
		buff_where_statement=buff2.toStdString();
	}




	//current areastate
	if(this->radioButton_areastate->isChecked()==true){
		for(int i=0; i< column.count(); i++){
			string small_letters=functions::convert_string2lower_case(label::areastate_id);
			if(column.fieldName(i).toStdString()==small_letters){
				
				buff1<<small_letters << " = "<< this->system_id->area_state << " ";
				if(where_found==true){
					buff1<< " AND ";
				}
				adding_required=true;
				break;
			}
		}
		//append the where statement
		if(where_found==true || adding_required==true){
			buffer<< buff1.str() << " " <<buff_where_statement;
		}
	}
	//current measure state
	else if(radioButton_measure->isChecked()==true){
		bool measure_state_field=false;
		string small_letters_measure=functions::convert_string2lower_case(label::measure_id);
		bool applied_flag_filed=false;
		string small_letters_applied=functions::convert_string2lower_case(label::applied_flag);
		for(int i=0; i< column.count(); i++){
			if(column.fieldName(i).toStdString()==small_letters_measure){
				measure_state_field=true;
			}
			else if(column.fieldName(i).toStdString()==small_letters_applied){
				applied_flag_filed=true;
			}
			if(measure_state_field==true && applied_flag_filed==true){
				break;
			}

		}
				
		if(measure_state_field==true && applied_flag_filed==false){
			buff1<<functions::convert_string2lower_case(label::areastate_id) << " = "<< this->system_id->area_state;
			buff1<<" AND "<<small_letters_measure << " = "<< this->system_id->measure_nr;
			if(where_found==true){
				buff1<< " AND ";
			}
			adding_required=true;
		}
		else if(measure_state_field==true && applied_flag_filed==true){
			buff1<<functions::convert_string2lower_case(label::areastate_id) << " = "<< this->system_id->area_state;
			buff1<<" AND "<<small_letters_applied << " = true " << " AND ( ";
			buff1<<small_letters_measure << " = "<< this->system_id->measure_nr<< " OR ";
			buff1<<small_letters_measure << " = "<< 0 << " ) ";
			if(where_found==true){
				buff1<< " AND ";
			}
			adding_required=true;
		}



		//append the where statement
		if(where_found==true || adding_required==true){
			buffer<< buff1.str() << " " <<buff_where_statement;
		}


	}
	else{
		//append the where statement
		if(where_found==true){
			buffer<< buff1.str() << " " <<buff_where_statement;
		}
		else{
			buffer << this->sqlEdit_rest->toPlainText().toStdString();
		}

	}



	str=buffer.str();

	emit statusMessage(tr(buffer.str().c_str())); 

	return str;

}
//Allocate the table loader thread
void Sys_Database_Projectbrowser_Wid::allocate_table_loader(void){
	if(this->loader_thread==NULL){
		this->loader_thread=new Sys_Sqlmodel_Loader;
	}
}
//Delete the table loader thread
void Sys_Database_Projectbrowser_Wid::delete_table_loader(void){
	if(this->loader_thread!=NULL){
		delete this->loader_thread;
		this->loader_thread=NULL;
	}
}
//_________________
//private slots
//Recieve the signal from the loader thread, that loading the data is finished
void Sys_Database_Projectbrowser_Wid::loading_is_finished(void){
	//model to view




    this->ptr_table_view->set_ptr_model(this->loader_thread->ptr_model);

    //this->ptr_table_view->setModel(this->loader_thread->ptr_model);
	this->ptr_table_view->set_table_title(this->label_table->text().toStdString());
	//set the actual time
	time(&this->actual_time);
	//send message
	if(this->stopbutton_clicked==false){
		if (this->loader_thread->ptr_model->lastError().type() != QSqlError::NoError){
			emit statusMessage(this->loader_thread->ptr_model->lastError().text());
			this->ptr_database->close();
			this->ptr_database->open();
		}
		else{
			ostringstream buffer;
			buffer <<"Query OK, number of rows: "<< this->loader_thread->ptr_model->rowCount() <<" in " <<functions::convert_seconds2string(this->actual_time-this->start_time) << label::time_unit_output;
			emit statusMessage(tr(buffer.str().c_str()));
		}
	}
	//enable the window after dinishing the thread
	this->ptr_table_view->setEnabled(true);
	this->ptr_status_msg->setEnabled(true);
	this->projecttreewid->setEnabled(true);
	this->sqlEdit_column->setEnabled(true);
	this->sqlEdit_rest->setEnabled(true);
	this->pushButton_help_condition->setEnabled(true);
	this->pushButton_help_row->setEnabled(true);
	this->submitButton->setEnabled(true);
	this->clearButton->setEnabled(true);
	this->stopButton->setEnabled(false);
	this->radioButton_all->setEnabled(true);
	this->radioButton_areastate->setEnabled(true);
	this->radioButton_measure->setEnabled(true);
	QObject::disconnect(this->loader_thread, SIGNAL(finished()), this, SLOT(loading_is_finished()));
}
//Show the sql-row helper widget
void Sys_Database_Projectbrowser_Wid::show_sql_row_helper(void){
	this->sql_help_row_wid.show();
	this->sql_help_row_wid.activateWindow();
	this->sql_help_row_wid.setFocus();

}
//Show the sql-condition helper widget
void Sys_Database_Projectbrowser_Wid::show_sql_condition_helper(void){
	this->sql_help_cond_wid.show();
	this->sql_help_cond_wid.activateWindow();
	this->sql_help_cond_wid.setFocus();
}
