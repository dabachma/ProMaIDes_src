#include "Sys_Headers_Precompiled.h"
//#include "Sys_Sql_Help_Row_Wid.h"


//Default constructor
Sys_Sql_Help_Row_Wid::Sys_Sql_Help_Row_Wid(QWidget *parent): QWidget(parent){
	this->setupUi(this);
    if(Data_Base::get_driver_type()==_sys_driver_type::MYSQL){
        this->pushButton_std->setText("STD(");
     }
    else{

        this->pushButton_std->setText("stddev(");
    }

	ostringstream tip;
	tip<<"Choose the column(s), which should be selected:"<<endl;
	tip<<"eg.1  *                    all columns are selected"<<endl;
	tip<<"eg.2  col1, col2           col1 and col2 are selected"<<endl;
	tip<<"eg.3  col1+col2, col2      sum of col1+col2 and col3 are selected"<<endl;
	tip<<"eg.4  ST_AsText(Geo_col)      Geometrical data are slected as text"<<endl;
	tip<<"eg.5  SUM(col1), AVG(col2) Aggregation of col1 as sum and col2 as average"<<endl;

	this->textEdit_sql->setToolTip(tip.str().c_str());

	QPalette p;
	p.setColor(QPalette::Active,QPalette::Base, Qt::lightGray);
	p.setColor(QPalette::Inactive,QPalette::Base, Qt::lightGray);
    this->textEdit_sql->setPalette(p);

	this->listWidget->setAlternatingRowColors(true);


	QObject::connect(this->close_button, SIGNAL(clicked()), this, SLOT(close_window()));
	QObject::connect(this->clear_button, SIGNAL(clicked()), this, SLOT(clear_display()));
	QObject::connect(this->send_button, SIGNAL(clicked()), this, SLOT(slot2send_txt()));

	//connect the buttons
	QObject::connect(this->pushButton_plus, SIGNAL(send_button_txt(QString )), this, SLOT(set_text2display(QString )));
	QObject::connect(this->pushButton_minus, SIGNAL(send_button_txt(QString )), this, SLOT(set_text2display(QString )));
	QObject::connect(this->pushButton_div, SIGNAL(send_button_txt(QString )), this, SLOT(set_text2display(QString )));
	QObject::connect(this->pushButton_mult, SIGNAL(send_button_txt(QString )), this, SLOT(set_text2display(QString )));

	QObject::connect(this->pushButton_open_brack, SIGNAL(send_button_txt(QString )), this, SLOT(set_text2display(QString )));
	QObject::connect(this->pushButton_close_brack, SIGNAL(send_button_txt(QString )), this, SLOT(set_text2display(QString )));

	QObject::connect(this->pushButton_komma, SIGNAL(send_button_txt(QString )), this, SLOT(set_text2display(QString )));

	QObject::connect(this->pushButton_sum, SIGNAL(send_button_txt(QString )), this, SLOT(set_text2display(QString )));
	QObject::connect(this->pushButton_count, SIGNAL(send_button_txt(QString )), this, SLOT(set_text2display(QString )));
	QObject::connect(this->pushButton_avg, SIGNAL(send_button_txt(QString )), this, SLOT(set_text2display(QString )));
	QObject::connect(this->pushButton_std, SIGNAL(send_button_txt(QString )), this, SLOT(set_text2display(QString )));
	QObject::connect(this->pushButton_max, SIGNAL(send_button_txt(QString )), this, SLOT(set_text2display(QString )));
	QObject::connect(this->pushButton_min, SIGNAL(send_button_txt(QString )), this, SLOT(set_text2display(QString )));
	QObject::connect(this->pushButton_astext, SIGNAL(send_button_txt(QString )), this, SLOT(set_text2display(QString )));

	

	QObject::connect(this->listWidget, SIGNAL(itemDoubleClicked(QListWidgetItem *)), this, SLOT(set_itemtext2display(QListWidgetItem *)));


	//count the memory
	Sys_Memory_Count::self()->add_mem(sizeof(Sys_Sql_Help_Row_Wid), _sys_system_modules::SYS_SYS);
}
//Default destructor
Sys_Sql_Help_Row_Wid::~Sys_Sql_Help_Row_Wid(void){


	//count the memory
	Sys_Memory_Count::self()->minus_mem(sizeof(Sys_Sql_Help_Row_Wid), _sys_system_modules::SYS_SYS);
}
//__________
//public
//Set the list widget with the column names
void Sys_Sql_Help_Row_Wid::set_list_column_name(QStringList *list){
	this->listWidget->clear();
	
	this->listWidget->addItem("*");

	this->listWidget->addItems(*list);

}
//__________
//public slots
//Set a text to the display, which is send by the Sys_Letter_Button
void Sys_Sql_Help_Row_Wid::set_text2display(QString txt){
	this->textEdit_sql->moveCursor(QTextCursor::End, QTextCursor::MoveAnchor);
	this->textEdit_sql->insertPlainText(txt);
	

}
//Set a text to the display, which is send by the list widget
void Sys_Sql_Help_Row_Wid::set_itemtext2display(QListWidgetItem *item){
	this->textEdit_sql->moveCursor(QTextCursor::End, QTextCursor::MoveAnchor);
	this->textEdit_sql->insertPlainText(item->text());
	
}
//Clear the display
void Sys_Sql_Help_Row_Wid::clear_display(void){
	this->textEdit_sql->clear();
	this->textEdit_sql->setText("");

}
//Close the window
void Sys_Sql_Help_Row_Wid::close_window(void){
	this->clear_display();
	this->close();

}
//A slot to send the display text
void Sys_Sql_Help_Row_Wid::slot2send_txt(void){
	if(this->textEdit_sql->toPlainText().isEmpty()==false){
		emit send_display_txt(this->textEdit_sql->toPlainText());
	}
	else{
		emit send_display_txt("*");
	}
}
