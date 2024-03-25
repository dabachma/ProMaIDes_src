#include "Sys_Headers_Precompiled.h"
//#include "Sys_Text_Searcher_Wid.h"


//Default constructor
Sys_Text_Searcher_Wid::Sys_Text_Searcher_Wid(QWidget *parent):QWidget(parent){

	this->setupUi(this);
	this->ptr_text_editor=NULL;

	QObject::connect(this->pushButton_find, SIGNAL(clicked()), this, SLOT(perform_search()));
	Sys_Memory_Count::self()->add_mem(sizeof(Sys_Text_Searcher_Wid),_sys_system_modules::SYS_SYS);

}
//Default destructor
Sys_Text_Searcher_Wid::~Sys_Text_Searcher_Wid(void){
	Sys_Memory_Count::self()->minus_mem(sizeof(Sys_Text_Searcher_Wid),_sys_system_modules::SYS_SYS);
}
//__________
//public
//Set a pointer to the text editor, where the search should be performed
void Sys_Text_Searcher_Wid::set_ptr_text_edit(QTextEdit *ptr){
	this->ptr_text_editor=ptr;

}
//Set the title of the window
void Sys_Text_Searcher_Wid::set_window_title(const _sys_system_modules type){
	ostringstream buffer;
	buffer << "Search for text in "<<functions::convert_system_module2txt(type) << " display";
	this->setWindowTitle(buffer.str().c_str());

	QStringList buff;
	ostringstream buff_str;

	if(type==_sys_system_modules::EXC_SYS){
		buff_str<<sys_label::str_sys<<"> ";
		buff.append(buff_str.str().c_str());
		buff_str.str("");
		buff_str<<sys_label::str_fpl<<"> ";
		buff.append(buff_str.str().c_str());
		buff_str.str("");
		buff_str<<sys_label::str_hyd<<"> ";
		buff.append(buff_str.str().c_str());
		buff_str.str("");
		buff_str<<sys_label::str_dam<<"> ";
		buff.append(buff_str.str().c_str());
		buff_str.str("");
		buff_str<<sys_label::str_risk<<"> ";
		buff.append(buff_str.str().c_str());
		buff_str.str("");
		buff_str<<sys_label::str_madm<<"> ";
		buff.append(buff_str.str().c_str());
		buff_str.str("");
		buff_str<<sys_label::str_alt<<"> ";
		buff.append(buff_str.str().c_str());
		buff_str.str("");
		buff_str<<sys_label::str_cost<<"> ";
		buff.append(buff_str.str().c_str());
		buff_str.str("");
		this->lineEdit_word->addItems(buff);
		this->lineEdit_word->insertSeparator(this->lineEdit_word->count());
	}
	else{
		buff_str<<"Errornumber ";
		buff.append(buff_str.str().c_str());
		buff_str.str("");
		buff_str<<"Warningnumber ";
		buff.append(buff_str.str().c_str());
		buff_str.str("");
		if(type==_sys_system_modules::HYD_SYS){
			buff_str<<"Wetted area";
			buff.append(buff_str.str().c_str());
			buff_str.str("");	
			buff_str<<"Modeltime";
			buff.append(buff_str.str().c_str());
			buff_str.str("");
		}
		this->lineEdit_word->addItems(buff);
		this->lineEdit_word->insertSeparator(this->lineEdit_word->count());
	}
}
//Set the title of the window
void Sys_Text_Searcher_Wid::set_window_title(const string txt){
	ostringstream buffer;
	buffer << "Search for text in "<<txt;
	this->setWindowTitle(buffer.str().c_str());
}
//Set start search at begin-flag 
void Sys_Text_Searcher_Wid::set_start_at_begin_flag(const bool flag){
	this->checkBox_start_begin->setChecked(flag);
}
//Set upwards search at flag 
void Sys_Text_Searcher_Wid::set_upwards_search_flag(const bool flag){
	this->checkBox_direction->setChecked(flag);
}
//____________
//public slots
//Start the dialog
void Sys_Text_Searcher_Wid::start_dialog(void){
	
	this->label_status->setText("Ok");
	this->show();
	this->activateWindow();
}
//________
//private slot
//Perform a search
void Sys_Text_Searcher_Wid::perform_search(void){
	this->lineEdit_word->addItem(this->lineEdit_word->currentText());
	
	if(this->ptr_text_editor!=NULL){
		this->setEnabled(true);
		this->label_status->setText("Search...");
		QTextDocument::FindFlags my_flag;

		if(this->checkBox_start_begin->isChecked()==true){
			this->ptr_text_editor->moveCursor(QTextCursor::Start, QTextCursor::MoveAnchor);
		}
		if(this->checkBox_direction->isChecked()==true){
			my_flag=my_flag|(QTextDocument::FindBackward);
		}
		if(this->checkBox_word_search->isChecked()==true){
			my_flag=my_flag|(QTextDocument::FindWholeWords);
		}

		if(this->checkBox_case_sens->isChecked()==true){
			my_flag=my_flag|(QTextDocument::FindCaseSensitively);
		}

		if(this->ptr_text_editor->find(this->lineEdit_word->currentText(), my_flag)==true){
			this->label_status->setText("ok");	
			
		}
		else{
			this->label_status->setText("Nothing found");
		}
		
	}
	else{
		this->label_status->setText("No editor is set");
	}


}