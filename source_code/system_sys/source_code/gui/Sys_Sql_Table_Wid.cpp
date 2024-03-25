#include "Sys_Headers_Precompiled.h"
//#include "Sys_Sql_Table_Wid.h"

//Default constructor
Sys_Sql_Table_Wid::Sys_Sql_Table_Wid(QWidget *parent):QTableView(parent){

	this->export_action=new QAction("Export to file...", &this->context_menu);
	this->context_menu.addAction(this->export_action);
	QObject::connect(this->export_action, SIGNAL(triggered()), this, SLOT(export_selected_data()));
	QObject::connect(this, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(context_menu_request(QPoint)));
    QObject::connect(this, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(set_column_filer(const QModelIndex)));




	this->setContextMenuPolicy(Qt::CustomContextMenu);

	this->export_type=_sys_export_type::exp_excel_tab;
	this->filename=label::not_set;
	//This property allows only the selection of rows in the table
	this->setSelectionBehavior(QAbstractItemView::SelectRows);
    this->setSortingEnabled(true);
    this->proxyModel=NULL;
    this->ptr_sqlModel=NULL;

    this->setAlternatingRowColors(true);
    this->horizontalHeader()->setSectionsMovable(true);





	Sys_Memory_Count::self()->add_mem(sizeof(Sys_Sql_Table_Wid),_sys_system_modules::SYS_SYS);


}
//Default destructor
Sys_Sql_Table_Wid::~Sys_Sql_Table_Wid(void){

	Sys_Memory_Count::self()->minus_mem(sizeof(Sys_Sql_Table_Wid),_sys_system_modules::SYS_SYS);
}
//________
//public
//Set the title of the table, which is displayed
void Sys_Sql_Table_Wid::set_table_title(const string title){
	this->table_title=title;
	int pos=0;
	pos=this->table_title.find("from");
	if(pos>=0){
		this->table_title=this->table_title.substr(pos+4);
	}

}
//Set Query model
void Sys_Sql_Table_Wid::set_ptr_model(QSqlQueryModel *ptr){

    this->proxyModel = new QSortFilterProxyModel(this);
    this->proxyModel->setSourceModel(ptr);

    this->setModel(this->proxyModel);
    this->ptr_sqlModel=ptr;


}

//_________
//private slots
//Open the export dialog
void Sys_Sql_Table_Wid::export_selected_data(void){
	
	this->my_dia.set_header_txt("Choose the type of export and the file for saving the data of sql-table");

	if(this->my_dia.start_dialog()){
		this->export_type = this->my_dia.get_export_type();
		this->filename = this->my_dia.get_output_file();

		if(this->export_type == _sys_export_type::exp_excel_comma){
			this->export_selected_data2ascii_comma();
		}
		else if(this->export_type == _sys_export_type::exp_excel_tab){
			this->export_selected_data2ascii_tab();
		} 
		else if(this->export_type == _sys_export_type::exp_tecplot){
			this->export_selected_data2tecplot();
		}
	}
}
//Handle a context menu request (Right Mouse Click)
void Sys_Sql_Table_Wid::context_menu_request(QPoint point){
	QPoint trans;
	trans = this->mapToGlobal(point);
	if(this->model()!=NULL && this->model()->rowCount()!=0 && this->model()->columnCount()!=0){
		this->context_menu.popup(trans);
	}
}
//Set filter per column
void Sys_Sql_Table_Wid::set_column_filer(const QModelIndex & index){

    this->proxyModel->setFilterKeyColumn(index.column());




    QRegExp buff_reg=this->proxyModel->filterRegExp();

    if(buff_reg.isEmpty()==true){
         QString buff_str;
         buff_str="^";
         buff_str.append(this->proxyModel->data(index,Qt::DisplayRole).toString());
         QRegExp buffer;
         buffer.setPattern(buff_str); 
        this->proxyModel->setFilterRegExp(buffer);


    }
    else{
        QRegExp buffer;
        this->proxyModel->setFilterRegExp(buffer);
    }
}
//________
//private
//Export selected data to file in ascii-format separated with tab
void Sys_Sql_Table_Wid::export_selected_data2ascii_tab(void){

	ofstream ascii_tab_file;
	ascii_tab_file.open (this->filename.c_str());

	if( ascii_tab_file.is_open() == false ){
		Error msg = this->set_error(0);
		ostringstream info;
		info <<"File name : " << this->filename << endl;
		msg.make_second_info(info.str());
	}

	ostringstream buffer;
	//table title
	buffer << "Title    " << this->table_title<<endl;
	ascii_tab_file << buffer.str() << endl;
	buffer.str("");
	//header
	for(int j=0; j< this->model()->columnCount();j++){
		buffer << (this->model()->headerData(j, Qt::Horizontal, Qt::DisplayRole)).toString().toStdString();
		buffer <<W(5)<< "  \t  ";
	}
	buffer << endl;
	ascii_tab_file << buffer.str();
	buffer.str("");

	//data all
	if((this->my_dia.getCheckBoxState()) == true){
		for(int row = 0; row < this->model()->rowCount(); row++){
			for(int j=0; j< this->model()->columnCount();j++){
				buffer << (this->model()->data(this->model()->index(row,j), Qt::DisplayRole)).toString().toStdString();
				buffer <<W(5)<< "  \t  ";
			}
			buffer << endl;
			ascii_tab_file << buffer.str();
			buffer.str("");
		}
	}
	//data selected
	else if((this->my_dia.getCheckBoxState()) == false){
		for(int i = 0; i < this->model()->rowCount(); i++){
			if(this->selectionModel()->isRowSelected(i, QModelIndex())){
				for(int j=0; j< this->model()->columnCount();j++){
					buffer << (this->model()->data(this->model()->index(i,j), Qt::DisplayRole)).toString().toStdString();
					buffer <<W(5)<< "  \t  ";
				}
				buffer << endl;
				ascii_tab_file << buffer.str();
				buffer.str("");
			}
		}
	}
	ascii_tab_file.close();
}
//Export selected data to file in ascii-format separated with comma
void Sys_Sql_Table_Wid::export_selected_data2ascii_comma(void){

	ofstream ascii_comma_file;
	ascii_comma_file.open (this->filename.c_str());

	if( ascii_comma_file.is_open() == false ){
		Error msg = this->set_error(0);
		ostringstream info;
		info <<"File name : " << this->filename << endl;
		msg.make_second_info(info.str());
	}

	ostringstream buffer;
	//table title
	buffer << "Title    " << this->table_title<<endl;
	ascii_comma_file << buffer.str() << endl;
	buffer.str("");
	//header
	for(int j=0; j< this->model()->columnCount();j++){
		if(j>0){
			buffer <<W(5)<< " , ";
		}
		buffer << (this->model()->headerData(j, Qt::Horizontal, Qt::DisplayRole)).toString().toStdString();
	}
	buffer << endl;
	ascii_comma_file << buffer.str();
	buffer.str("");

	//data all
	if((this->my_dia.getCheckBoxState()) == true){
		for(int row = 0; row < this->model()->rowCount(); row++){
			for(int j=0; j< this->model()->columnCount();j++){
				if(j>0){
					buffer <<W(5)<< " , ";
				}
				buffer << (this->model()->data(this->model()->index(row,j), Qt::DisplayRole)).toString().toStdString();
				
			}
			buffer << endl;
			ascii_comma_file << buffer.str();
			buffer.str("");
		}
	}
	//data selected
	else if((this->my_dia.getCheckBoxState()) == false){
		for(int i = 0; i < this->model()->rowCount(); i++){
			if(this->selectionModel()->isRowSelected(i, QModelIndex())){
				for(int j=0; j< this->model()->columnCount();j++){
					if(j>0){
						buffer <<W(5)<< " , ";
					}
					buffer << (this->model()->data(this->model()->index(i,j), Qt::DisplayRole)).toString().toStdString();
				}
				buffer << endl;
				ascii_comma_file << buffer.str();
				buffer.str("");
			}
		}
	}
	ascii_comma_file.close();
}
//Export selected data to file in tecplot-format
void Sys_Sql_Table_Wid::export_selected_data2tecplot(void){

	ofstream ascii_tec_file;
	ascii_tec_file.open (this->filename.c_str());

	if( ascii_tec_file.is_open() == false ){
		Error msg = this->set_error(0);
		ostringstream info;
		info <<"File name : " << this->filename << endl;
		msg.make_second_info(info.str());
	}


	ostringstream buffer;
	//header
	buffer << "TITLE = \" " <<this->table_title<<"\""<<endl;
	buffer << "VARIABLES = ";
	for(int j=0; j< this->model()->columnCount();j++){
		buffer << " \" ";
		buffer << (this->model()->headerData(j, Qt::Horizontal, Qt::DisplayRole)).toString().toStdString();
		buffer << " \" "<<W(5);
	}

	buffer << endl;
	buffer << "ZONE T = \" " <<this->table_title<<"\""<<endl;
	if((this->my_dia.getCheckBoxState()) == true){
		buffer << "I = " << this->model()->rowCount() <<endl;
	}
	else{
		int number=this->selectionModel()->selectedRows().count();
		buffer << "I = " << number <<endl;

	}


	ascii_tec_file << buffer.str();
	buffer.str("");

	//data all
	if((this->my_dia.getCheckBoxState()) == true){
		for(int row = 0; row < this->model()->rowCount(); row++){
			for(int j=0; j< this->model()->columnCount();j++){
				buffer << (this->model()->data(this->model()->index(row,j), Qt::DisplayRole)).toString().toStdString();
				buffer <<W(5)<< "  \t  ";
			}
			buffer << endl;
			ascii_tec_file << buffer.str();
			buffer.str("");
		}
	}
	//data selected
	else if((this->my_dia.getCheckBoxState()) == false){
		for(int i = 0; i < this->model()->rowCount(); i++){
			if(this->selectionModel()->isRowSelected(i, QModelIndex())){
				for(int j=0; j< this->model()->columnCount();j++){
					buffer << (this->model()->data(this->model()->index(i,j), Qt::DisplayRole)).toString().toStdString();
					buffer <<W(5)<< "  \t  ";
				}
				buffer << endl;
				ascii_tec_file << buffer.str();
				buffer.str("");
			}
		}
	}
	ascii_tec_file.close();
}
//Set error(s)
Error Sys_Sql_Table_Wid::set_error(const int err_type){
		string place="Sys_Sql_Table_Wid::";
		string help;
		string reason;
		int type=0;
		bool fatal=false;
		stringstream info;
		Error msg;

	switch (err_type){
		case 0://can not open file
			place.append("export_selected_data2...(void)");
			reason="Can not open file";
			help="Check the file name";
			type=5;
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
