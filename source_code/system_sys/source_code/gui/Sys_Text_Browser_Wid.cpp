#include "Sys_Headers_Precompiled.h"
#include <QPrintPreviewDialog>
//#include "Sys_Text_Browser_Wid.h"

//Default constructor
Sys_Text_Browser_Wid::Sys_Text_Browser_Wid(QWidget *parent) : QWidget(parent){
	this->setupUi(this);
	QFont myfont("Courier", 9);
	myfont.setBold(false);
	this->textEdit->setFont(myfont);
	this->textEdit->setLineWrapMode(QTextEdit::NoWrap);
	this->textEdit->setTabStopWidth(10);
	

	this->file_read_only=true;
	this->groupBox_title->setTitle("No document");
	this->textEdit->setReadOnly(this->file_read_only);
	this->create_action_for_context_menu();
	this->textEdit->setContextMenuPolicy(Qt::CustomContextMenu);
	QObject::connect(this->textEdit, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(context_menu_request(QPoint)));
	QObject::connect(this->textEdit, SIGNAL(textChanged()), this, SLOT(check_for_modification()));

	Sys_Memory_Count::self()->add_mem(sizeof(Sys_Text_Browser_Wid), _sys_system_modules::SYS_SYS);
}
//Default destructor
Sys_Text_Browser_Wid::~Sys_Text_Browser_Wid(void){
	Sys_Memory_Count::self()->minus_mem(sizeof(Sys_Text_Browser_Wid), _sys_system_modules::SYS_SYS);
}
//__________
//public
//Load a file
bool Sys_Text_Browser_Wid::load(const QFileInfo file){
	if(this->textEdit->document()->isEmpty()==false){
		this->close_document();
	}
	if(QFile::exists(file.absoluteFilePath())==false){
		return false;
	}
	QFile ifile(file.absoluteFilePath());
	if (ifile.open(QFile::ReadOnly)==false){
		return false;
	}

	QByteArray data;
	if(ifile.size()>114857600){
		Sys_Diverse_Text_Dia dialog2(true);
		ostringstream txt;
		txt<<"The file requires to much memory to open!"<< endl;
		dialog2.set_dialog_question(txt.str());
		dialog2.start_dialog();
		ifile.close();
		return false;
	}
	
	data= ifile.readAll();
	this->textEdit->setPlainText(data);
	
	

	this->set_current_file_name(file);
	ifile.close();
	return true;

}
//Ask for saving by closing a changed file
bool Sys_Text_Browser_Wid::maybe_save(void){
	if(this->textEdit->document()->isModified()!=true){
         return true;
	}
	if(this->file_name.startsWith(QLatin1String(":/"))){
         return true;
	}
	QMessageBox::StandardButton ret;
	ret = QMessageBox::warning(this, tr("Application"),
							tr("The document has been modified.\n"
							   "Do you want to save your changes?"),
							QMessageBox::Save | QMessageBox::Discard
							| QMessageBox::Cancel);
	if (ret == QMessageBox::Save){
		return this->file_save();
	}
	else if (ret == QMessageBox::Cancel){
		return false;
	}
	return true;

}
//Set the current file name
void Sys_Text_Browser_Wid::set_current_file_name(const QFileInfo file){
	 this->file_name = file.absoluteFilePath();
	 this->pure_file_name=file.baseName();
	 this->groupBox_title->setTitle(file.fileName());
     this->textEdit->document()->setModified(false);
	 if(file.suffix()=="prm"){
		 this->file_read_only=true;
		 this->textEdit->setReadOnly(this->file_read_only);
		
		 this->label_path->setText(file.absoluteFilePath());
		 this->label_path->setToolTip(".prm-file is not editable");
	 }
	 else{
		 this->file_read_only=false;
		 this->textEdit->setReadOnly(this->file_read_only);
		 this->label_path->setText(file.absoluteFilePath());
		 this->label_path->setToolTip("");
	 }
	 this->file_info=file;
}
//Get the current file info
QFileInfo Sys_Text_Browser_Wid::get_current_file_info(void){

	return this->file_info;
}
//____________
//public slots
//Close document
bool Sys_Text_Browser_Wid::close_document(void){
	if(this->textEdit->document()->isModified()==true){
		if(this->maybe_save()==false){
			return false;
		}
	}		
	//this->textEdit->clear();
	this->textEdit->document()->clear();
	this->groupBox_title->setTitle("No document");
    this->label_path->setText("");
	this->label_path->setToolTip("");
	this->file_read_only=true;
	this->textEdit->setReadOnly(this->file_read_only);
	this->my_searcher.close();
	this->file_name = "";
	this->pure_file_name="";
	this->file_info.setFile("");
	this->textEdit->document()->setModified(false);
	this->check_for_modification();
	return true;
}
//Context menu, provides some functionalities by mouse right click
void Sys_Text_Browser_Wid::context_menu_request(QPoint point){
	this->my_searcher.set_ptr_text_edit(this->textEdit);
	this->my_searcher.set_window_title(this->pure_file_name.toStdString());
	this->my_searcher.set_upwards_search_flag(false);
	QMenu *contextMenu = new QMenu(this);
	if(this->file_read_only==true){
		contextMenu->addAction(this->new_act);
		contextMenu->addSeparator();
		contextMenu->addAction(this->searching_act);
		contextMenu->addSeparator();
		contextMenu->addAction(this->print_act);
		contextMenu->addAction(this->print_preview_act);
		contextMenu->addAction(this->print2pdf_act);
		contextMenu->addSeparator();
		contextMenu->addAction(this->close_act);

	}
	else{
		contextMenu->addAction(this->new_act);
		contextMenu->addAction(this->save_act);
		contextMenu->addAction(this->save_as_act);
		contextMenu->addSeparator();
		contextMenu->addAction(this->selectAll_act);
		contextMenu->addAction(this->copy_act);
		contextMenu->addAction(this->paste_act);
		contextMenu->addAction(this->undo_act);
		contextMenu->addAction(this->redo_act);
		contextMenu->addSeparator();
		contextMenu->addAction(this->searching_act);
		contextMenu->addSeparator();
		contextMenu->addAction(this->print_act);
		contextMenu->addAction(this->print_preview_act);
		contextMenu->addAction(this->print2pdf_act);
		contextMenu->addSeparator();
		contextMenu->addAction(this->close_act);

	}
	QPoint trans;
	trans = this->mapToGlobal(point);
	contextMenu->popup(trans);

}
//__________
//private slots:
 //Save a file
bool Sys_Text_Browser_Wid::file_save(void){
	if (this->pure_file_name.isEmpty()==true){
         return this->file_save_as();
	}

	QFile my_file;
	my_file.setFileName(this->file_name);
	QIODevice::OpenMode my_flag;
	my_flag=my_flag|(QIODevice::WriteOnly);
	my_flag=my_flag|(QIODevice::Text);
	bool flag=my_file.open(my_flag);
	if(flag==true){
        my_file.write(this->textEdit->document()->toPlainText().toLatin1());
		this->textEdit->document()->setModified(false);
		this->groupBox_title->setTitle(this->file_info.fileName());
		this->check_for_modification();
		my_file.close();
	}

	return flag;
}
//New a file
void Sys_Text_Browser_Wid::file_new(void){
	if (this->close_document()==true) {
		this->textEdit->clear();
		this->groupBox_title->setTitle("untitled");

	}
}
 //Save a file with a new name
bool Sys_Text_Browser_Wid::file_save_as(void){
	QString fn = QFileDialog::getSaveFileName(this, tr("Save as..."), Sys_Project::get_current_path().c_str());
	if (fn.isEmpty()==true){
         return false;
	}
	QFileInfo my_info;
	my_info.setFile(fn);

	this->set_current_file_name(my_info);

    return this->file_save();
}
 //Print a file
void Sys_Text_Browser_Wid::file_print(void){
	QPrinter printer(QPrinter::HighResolution);
	QPrintDialog *dlg = new QPrintDialog(&printer, this);
	if (this->textEdit->textCursor().hasSelection()==true){
		dlg->addEnabledOption(QAbstractPrintDialog::PrintSelection);
	}
	dlg->setWindowTitle(tr("Print Document"));
	if (dlg->exec() == QDialog::Accepted) {
		this->textEdit->print(&printer);
	}
	delete dlg;
}
 //Print preview
void Sys_Text_Browser_Wid::file_print_preview(void){
	 QPrinter printer(QPrinter::HighResolution);
     QPrintPreviewDialog preview(&printer, this);
	 QObject::connect(&preview, SIGNAL(paintRequested(QPrinter *)), SLOT(print_preview(QPrinter *)));
     preview.exec();
}
//Print the preview
void Sys_Text_Browser_Wid::print_preview(QPrinter *printer){
	this->textEdit->print(printer);
}
 //Print a file to pdf
void Sys_Text_Browser_Wid::file_print_pdf(void){
	QString fileName = QFileDialog::getSaveFileName(this, "Export PDF", Sys_Project::get_current_path().c_str(), "*.pdf");
	if (fileName.isEmpty()==false) {
		if (QFileInfo(fileName).suffix().isEmpty()==true){
			fileName.append(".pdf");
		}
		QPrinter printer(QPrinter::HighResolution);
		printer.setOutputFormat(QPrinter::PdfFormat);
		printer.setOutputFileName(fileName);
		this->textEdit->document()->print(&printer);
	}
}
 ///Check if the file is modified
void Sys_Text_Browser_Wid::check_for_modification(void){
	if(this->textEdit->document()->isModified()==true){
		string buffer=this->groupBox_title->title().toStdString();
		buffer=buffer.at(buffer.length()-1);
		if(buffer!="*"){
			ostringstream buff;
			buff<<this->groupBox_title->title().toStdString()<<"*";
			this->groupBox_title->setTitle(buff.str().c_str());	
		}

	}
}
//Start the text-searcher dialog
void Sys_Text_Browser_Wid::start_text_searcher(void){
	this->my_searcher.set_ptr_text_edit(this->textEdit);
	this->my_searcher.set_upwards_search_flag(false);
	this->my_searcher.start_dialog();
}
//_________
//private
//Set up all the actions for the context menu
void Sys_Text_Browser_Wid::create_action_for_context_menu(void){

	this->new_act = new QAction(tr("&New"), this);
	QObject::connect(new_act, SIGNAL(triggered()), this, SLOT(file_new()));
	
	this->selectAll_act = new QAction(tr("&Select All"), this);
	this->selectAll_act->setShortcut(tr("Ctrl+A"));
	//this->selectAll_act->setShortcut(Qt::Key_F1);
	this->selectAll_act->setShortcutContext(Qt::WidgetWithChildrenShortcut);
	
	QObject::connect(selectAll_act, SIGNAL(triggered()), this->textEdit, SLOT(selectAll()));
	this->copy_act = new QAction(tr("Copy"), this);
	this->copy_act->setShortcut(tr("Ctrl+C"));
	QObject::connect(copy_act, SIGNAL(triggered()), this->textEdit, SLOT(copy()));
    this->paste_act = new QAction(tr("&Paste"), this);
	this->paste_act->setShortcut(tr("Ctrl+V"));
	QObject::connect(paste_act, SIGNAL(triggered()), this->textEdit, SLOT(paste()));

    this->searching_act = new QAction(tr("&Find..."), this);
	this->searching_act->setShortcut(Qt::Key_F3);
	this->short_cut_search = new QShortcut(Qt::Key_F3,this->textEdit);
	this->short_cut_search->setContext(Qt::WidgetShortcut);
	QObject::connect(short_cut_search, SIGNAL(activated()), this, SLOT(start_text_searcher()));
	QObject::connect(searching_act, SIGNAL(triggered()), &this->my_searcher, SLOT(start_dialog()));

	this->save_act = new QAction(tr("&Save"), this);
	this->save_act->setShortcut(tr("Ctrl+S"));
	QObject::connect(save_act, SIGNAL(triggered()), this, SLOT(file_save()));
	this->short_cut_save =new QShortcut(tr("Ctrl+S"),this->textEdit);
	QObject::connect(short_cut_save, SIGNAL(activated()), this, SLOT(file_save()));
	this->short_cut_save->setContext(Qt::WidgetShortcut);
	
	this->save_as_act = new QAction(tr("Save &as..."), this);
	QObject::connect(save_as_act, SIGNAL(triggered()), this, SLOT(file_save_as()));

	this->close_act = new QAction(tr("&Close"), this);
	QObject::connect(close_act, SIGNAL(triggered()), this, SLOT(close_document()));

	this->print_act = new QAction(tr("Print..."), this);
	QObject::connect(print_act, SIGNAL(triggered()), this, SLOT(file_print()));
	this->print_preview_act = new QAction(tr("&Print preview..."), this);
	QObject::connect(print_preview_act, SIGNAL(triggered()), this, SLOT(file_print_preview()));
	this->print2pdf_act = new QAction(tr("&Export to pdf..."), this);
	QObject::connect(print2pdf_act, SIGNAL(triggered()), this, SLOT(file_print_pdf()));


	this->undo_act = new QAction(tr("&Undo"), this);
	QObject::connect(undo_act, SIGNAL(triggered()), this->textEdit, SLOT(undo()));
	this->redo_act = new QAction(tr("&Redo"), this);
	QObject::connect(redo_act, SIGNAL(triggered()), this->textEdit, SLOT(redo()));

	//set to buttons
	QObject::connect(this->toolButton_save, SIGNAL(clicked()), this, SLOT(file_save()));
	QObject::connect(this->toolButton_close, SIGNAL(clicked()), this, SLOT(close_document()));
	QObject::connect(this->toolButton_print, SIGNAL(clicked()), this, SLOT(file_print()));
}
