#include "Sys_Headers_Precompiled.h"
//#include "_Sys_Result_Summary_Wid.h"


//Default constructor
_Sys_Result_Summary_Wid::_Sys_Result_Summary_Wid(QWidget *parent): _Sys_Data_Wid(parent){
	this->setupUi(this);

    //this->area2print=this->tableWidget;
    this->area2print=this->scrollAreaWidgetContents;

	this->export_action=new QAction("Export to file...", &this->context_menu);
	this->context_menu.addAction(this->export_action);
	QObject::connect(this->export_action, SIGNAL(triggered()), this, SLOT(export_selected_data()));
	
	this->context_menu.addSeparator();

	this->sort_action_up=new QAction("Sort after selected column (min)", &this->context_menu);
	this->context_menu.addAction(this->sort_action_up);
	QObject::connect(this->sort_action_up, SIGNAL(triggered()), this, SLOT(sort_widget_by_column_up()));

	this->sort_action_down=new QAction("Sort after selected column (max)", &this->context_menu);
	this->context_menu.addAction(this->sort_action_down);
	QObject::connect(this->sort_action_down, SIGNAL(triggered()), this, SLOT(sort_widget_by_column_down()));

	

	this->export_type=_sys_export_type::exp_excel_tab;
	this->filename=label::not_set;
	//this->sys_id.

	this->tableWidget->setSelectionBehavior(QAbstractItemView::SelectColumns);
	this->tableWidget->setSelectionMode(QAbstractItemView::SingleSelection);
	this->tableWidget->setContextMenuPolicy(Qt::CustomContextMenu);
	QObject::connect(this->tableWidget, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(context_menu_request(QPoint)));

}
//Default destructor
_Sys_Result_Summary_Wid::~_Sys_Result_Summary_Wid(void){

}
//_______
//public
//Set tooltip of the header
void _Sys_Result_Summary_Wid::set_header_tooltip(const string tooltip){
	this->head_label->setToolTip(tooltip.c_str());
	this->setWindowTitle(tooltip.c_str());
	string buffer;
	buffer=_Sys_Data_Wid::get_last_string_string_id(tooltip);
	this->label_id->setText(buffer.c_str());
}
//________
//protected slots
//Sort widget by marked column, ascending order
void _Sys_Result_Summary_Wid::sort_widget_by_column_up(void){
	if(this->tableWidget->columnCount()==0){
		return;
	}
	this->tableWidget->sortItems(this->tableWidget->currentColumn(),Qt::AscendingOrder);
}
///Sort widget by marked column, descending order
void _Sys_Result_Summary_Wid::sort_widget_by_column_down(void){
	if(this->tableWidget->columnCount()==0){
		return;
	}
	this->tableWidget->sortItems(this->tableWidget->currentColumn(),Qt::DescendingOrder);
}
//Open the export dialog
void _Sys_Result_Summary_Wid::export_selected_data(void){

	if(this->tableWidget->columnCount()==0){
		return;
	}
	
	this->my_dia.set_header_txt("Choose the type of export and the file for saving the data of result widget");


	this->my_dia.get_ptr_selectall_checkbox()->setEnabled(false);
	this->my_dia.get_ptr_selectall_groupbox()->hide();
	this->my_dia.get_ptr_tecplot_radio_button()->setEnabled(false);


	if(this->my_dia.start_dialog()){
		this->export_type = this->my_dia.get_export_type();
		this->filename = this->my_dia.get_output_file();

		if(this->export_type == _sys_export_type::exp_excel_comma){
			this->export_selected_data2ascii_comma();
		}
		else if(this->export_type == _sys_export_type::exp_excel_tab){
			this->export_selected_data2ascii_tab();
		} 
	}
}
//________
//privat
//Export selected data to file in ascii-format separated with tabs
void _Sys_Result_Summary_Wid::export_selected_data2ascii_tab(void){
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
	buffer << "Title    " << this->head_label->toolTip().toStdString()<<endl;
	ascii_tab_file << buffer.str() << endl;
	buffer.str("");
	//header
	for(int j=0; j< this->tableWidget->columnCount();j++){
		buffer << (this->tableWidget->horizontalHeaderItem(j)->data(Qt::DisplayRole)).toString().toStdString();
		buffer <<W(5)<< "  \t  ";
	}
	buffer << endl;
	ascii_tab_file << buffer.str();
	buffer.str("");

	//data all
	
	for(int row = 0; row < this->tableWidget->rowCount(); row++){
		for(int j=0; j< this->tableWidget->columnCount(); j++){
			buffer << (this->tableWidget->item(row,j)->data(Qt::DisplayRole)).toString().toStdString();
			buffer <<W(5)<< "  \t  ";
		}
		buffer << endl;
		ascii_tab_file << buffer.str();
		buffer.str("");
	}


	ascii_tab_file.close();
}
//Export selected data to file in ascii-format separated with comma
void _Sys_Result_Summary_Wid::export_selected_data2ascii_comma(void){
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
	buffer << "Title    " << this->head_label->toolTip().toStdString()<<endl;
	ascii_tab_file << buffer.str() << endl;
	buffer.str("");
	//header
	for(int j=0; j< this->tableWidget->columnCount();j++){
		buffer << (this->tableWidget->horizontalHeaderItem(j)->data(Qt::DisplayRole)).toString().toStdString();
		buffer <<W(5)<< "  ,  ";
	}
	buffer << endl;
	ascii_tab_file << buffer.str();
	buffer.str("");

	//data all
	
	for(int row = 0; row < this->tableWidget->rowCount(); row++){
		for(int j=0; j< this->tableWidget->columnCount(); j++){
			buffer << (this->tableWidget->item(row,j)->data(Qt::DisplayRole)).toString().toStdString();
			buffer <<W(5)<< "  ,  ";
		}
		buffer << endl;
		ascii_tab_file << buffer.str();
		buffer.str("");
	}


	ascii_tab_file.close();
}
//Set error(s)
Error _Sys_Result_Summary_Wid::set_error(const int err_type){
		string place="_Sys_Result_Summary_Wid::";
		string help;
		string reason;
		int type=0;
		bool fatal=false;
		stringstream info;
		Error msg;

	switch (err_type){
		case 0://can not open file
			place.append("export_selected_data2...");
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
