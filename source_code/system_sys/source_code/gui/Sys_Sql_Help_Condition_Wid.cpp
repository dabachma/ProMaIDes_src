#include "Sys_Headers_Precompiled.h"
//#include "Sys_Sql_Help_Condition_Wid.h"


//Default constructor
Sys_Sql_Help_Condition_Wid::Sys_Sql_Help_Condition_Wid(QWidget *parent): QWidget(parent){
	this->setupUi(this);

	ostringstream tip;
	tip<<"Choose a condition for data selection:"<<endl;
	tip<<"eg.1  where col1=7                all data are selected, where col1 equals 7"<<endl;
	tip<<"eg.2  where col1!=7               all data are selected, where col1 not equals 7"<<endl;
	tip<<"eg.3  where col1=7 and col2>0     all data are selected, where col1 equals 7 and col2 is positiv" <<endl;
	tip<<"eg.4  where col1=7 or col2>=0     all data are selected, where col1 equals 7 or col2 is positiv or zero" <<endl;
	tip<<"eg.5  where col1=7 order by(col2) all data are selected, where col1 equals 7; all is order by col2 ascending"<<endl;
	tip<<"eg.6  where col1='text'           all data are selected, where col1 equals the text"<<endl;
	tip<<"eg.7  where col1 like 'text%'     all data are selected, where col1 is like text...."<<endl;
	
	this->textEdit_sql->setToolTip(tip.str().c_str());

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
	QObject::connect(this->pushButton_strich, SIGNAL(send_button_txt(QString )), this, SLOT(set_text2display(QString )));
	QObject::connect(this->pushButton_perc, SIGNAL(send_button_txt(QString )), this, SLOT(set_text2display(QString )));

	QObject::connect(this->pushButton_like, SIGNAL(send_button_txt(QString )), this, SLOT(set_text2display(QString )));

	QObject::connect(this->pushButton_order, SIGNAL(send_button_txt(QString )), this, SLOT(set_text2display(QString )));


	QObject::connect(this->pushButton_equal, SIGNAL(send_button_txt(QString )), this, SLOT(set_text2display(QString )));
	QObject::connect(this->pushButton_unequal, SIGNAL(send_button_txt(QString )), this, SLOT(set_text2display(QString )));
	QObject::connect(this->pushButton_higher, SIGNAL(send_button_txt(QString )), this, SLOT(set_text2display(QString )));
	QObject::connect(this->pushButton_smaller, SIGNAL(send_button_txt(QString )), this, SLOT(set_text2display(QString )));

	QObject::connect(this->pushButton_and, SIGNAL(send_button_txt(QString )), this, SLOT(set_text2display(QString )));
	QObject::connect(this->pushButton_or, SIGNAL(send_button_txt(QString )), this, SLOT(set_text2display(QString )));

	QObject::connect(this->listWidget, SIGNAL(itemDoubleClicked(QListWidgetItem *)), this, SLOT(set_itemtext2display(QListWidgetItem *)));

	//count the memory
	Sys_Memory_Count::self()->add_mem(sizeof(Sys_Sql_Help_Condition_Wid), _sys_system_modules::SYS_SYS);
}
//Default destructor
Sys_Sql_Help_Condition_Wid::~Sys_Sql_Help_Condition_Wid(void){



	//count the memory
	Sys_Memory_Count::self()->minus_mem(sizeof(Sys_Sql_Help_Condition_Wid), _sys_system_modules::SYS_SYS);
}
//__________
//public
//Set the list widget with the column names
void Sys_Sql_Help_Condition_Wid::set_list_column_name(QStringList *list){
	this->listWidget->clear();
	
	
	this->listWidget->addItems(*list);

}
//__________
//public slots
//Set a text to the display, which is send by the Sys_Letter_Button
void Sys_Sql_Help_Condition_Wid::set_text2display(QString txt){
	this->textEdit_sql->moveCursor(QTextCursor::End, QTextCursor::MoveAnchor);
	this->textEdit_sql->insertPlainText(txt);
	

}
//Set a text to the display, which is send by the list widget
void Sys_Sql_Help_Condition_Wid::set_itemtext2display(QListWidgetItem *item){
	this->textEdit_sql->moveCursor(QTextCursor::End, QTextCursor::MoveAnchor);
	this->textEdit_sql->insertPlainText(item->text());

}
//Clear the display
void Sys_Sql_Help_Condition_Wid::clear_display(void){
	this->textEdit_sql->clear();
	this->textEdit_sql->setText("");

}
//Close the window
void Sys_Sql_Help_Condition_Wid::close_window(void){
	this->clear_display();
	this->close();

}
//A slot to send the display text
void Sys_Sql_Help_Condition_Wid::slot2send_txt(void){
	QString buff;
	if(this->textEdit_sql->toPlainText().isEmpty()==false){
		buff="where ";
		buff.append(this->textEdit_sql->toPlainText());
	
	}
	emit send_display_txt(buff);
}