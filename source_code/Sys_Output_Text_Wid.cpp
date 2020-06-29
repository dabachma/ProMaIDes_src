//#include "Sys_Output_Text_Wid.h"
#include "Main_Headers_Precompiled.h"

//Default constructor
Sys_Output_Text_Wid::Sys_Output_Text_Wid(QWidget *parent):QTextEdit(parent){
	this->createActions();
	this->connectActions();
	//this->setReadOnly(true);
	this->document()->setMaximumBlockCount(10000);
	QFont myfont("Courier", 8);
	this->setFont(myfont);
	this->setLineWrapMode(QTextEdit::NoWrap);
	this->flag_clear_required=false;
	this->flag_detailed_set_required=false;
	this->ensureCursorVisible();

	Sys_Memory_Count::self()->add_mem((sizeof(Sys_Output_Text_Wid)), _sys_system_modules::SYS_SYS);
}
//Default destructor
Sys_Output_Text_Wid::~Sys_Output_Text_Wid(void){
	Sys_Memory_Count::self()->minus_mem((sizeof(Sys_Output_Text_Wid)), _sys_system_modules::SYS_SYS);
}
//________
//public
//Custom connect function, provides option to clear QTextEdit
void Sys_Output_Text_Wid::connect_clear_act(Main_Wid *receiver, const _sys_system_modules type){
	switch (type){
		case _sys_system_modules::SYS_SYS:
			QObject::connect(clear_act, SIGNAL(triggered()), receiver, SLOT(reset_system_outputtxt()) );
			QObject::connect(short_cut_clear, SIGNAL(activated()), receiver, SLOT(reset_system_outputtxt()) );
			this->flag_clear_required=true;
			break;
		case _sys_system_modules::EXC_SYS:
			QObject::connect(clear_act, SIGNAL(triggered()), receiver, SLOT(reset_error_warning()) );
			QObject::connect(short_cut_clear, SIGNAL(activated()), receiver, SLOT(reset_error_warning()) );
			this->flag_clear_required=true;
			break;
		case _sys_system_modules::FPL_SYS:
			QObject::connect(clear_act, SIGNAL(triggered()), receiver, SLOT(reset_fpl_outputtxt()) );
			QObject::connect(short_cut_clear, SIGNAL(activated()), receiver, SLOT(reset_fpl_outputtxt()) );
			this->flag_clear_required=true;
			break;
		case _sys_system_modules::HYD_SYS:
			QObject::connect(clear_act, SIGNAL(triggered()), receiver, SLOT(reset_hyd_outputtxt()) );
			QObject::connect(short_cut_clear, SIGNAL(activated()), receiver, SLOT(reset_hyd_outputtxt()) );
			this->flag_clear_required=true;
			break;
		case _sys_system_modules::DAM_SYS:
			QObject::connect(clear_act, SIGNAL(triggered()), receiver, SLOT(reset_dam_outputtxt()) );
			QObject::connect(short_cut_clear, SIGNAL(activated()), receiver, SLOT(reset_dam_outputtxt()) );
			this->flag_clear_required=true;
			break;
		case _sys_system_modules::MADM_SYS:
			QObject::connect(clear_act, SIGNAL(triggered()), receiver, SLOT(reset_madm_outputtxt()) );
			this->flag_clear_required=true;
			break;
		case _sys_system_modules::RISK_SYS:
			QObject::connect(clear_act, SIGNAL(triggered()), receiver, SLOT(reset_risk_outputtxt()) );
			QObject::connect(short_cut_clear, SIGNAL(activated()), receiver, SLOT(reset_risk_outputtxt()) );
			this->flag_clear_required=true;
			break;
		case _sys_system_modules::COST_SYS:
			QObject::connect(clear_act, SIGNAL(triggered()), receiver, SLOT(reset_cost_outputtxt()) );
			QObject::connect(short_cut_clear, SIGNAL(activated()), receiver, SLOT(reset_cost_outputtxt()) );
			this->flag_clear_required=true;
			break;
		case _sys_system_modules::ALT_SYS:
			QObject::connect(clear_act, SIGNAL(triggered()), receiver, SLOT(reset_alt_outputtxt()) );
			QObject::connect(short_cut_clear, SIGNAL(activated()), receiver, SLOT(reset_alt_outputtxt()) );
			this->flag_clear_required=true;
			break;
		default:
			this->flag_clear_required=false;
			break;
	}
}
//Custom connect function, provides option to set some details on QTextEdit
void Sys_Output_Text_Wid::connect_setDetailed_act(Main_Wid *receiver, const _sys_system_modules type){
	switch (type){
		case _sys_system_modules::SYS_SYS:
			QObject::connect(setDetailed_act, SIGNAL(triggered()), receiver, SLOT(set_system_outputflag()) );
			this->flag_detailed_set_required=true;
			break;
		case _sys_system_modules::FPL_SYS:
			QObject::connect(setDetailed_act, SIGNAL(triggered()), receiver, SLOT(set_fpl_outputflag()) );
			this->flag_detailed_set_required=true;
			break;
		case _sys_system_modules::HYD_SYS:
			QObject::connect(setDetailed_act, SIGNAL(triggered()), receiver, SLOT(set_hyd_outputflag()) );
			this->flag_detailed_set_required=true;
			break;
		case _sys_system_modules::DAM_SYS:
			QObject::connect(setDetailed_act, SIGNAL(triggered()), receiver, SLOT(set_dam_outputflag()) );
			this->flag_detailed_set_required=true;
			break;
		case _sys_system_modules::MADM_SYS:
			QObject::connect(setDetailed_act, SIGNAL(triggered()), receiver, SLOT(set_madm_outputflag()) );
			this->flag_detailed_set_required=true;
			break;
		case _sys_system_modules::RISK_SYS:
			QObject::connect(setDetailed_act, SIGNAL(triggered()), receiver, SLOT(set_risk_outputflag()) );
			this->flag_detailed_set_required=true;
			break;
		case _sys_system_modules::COST_SYS:
			QObject::connect(setDetailed_act, SIGNAL(triggered()), receiver, SLOT(set_cost_outputflag()) );
			this->flag_detailed_set_required=true;
			break;
		case _sys_system_modules::ALT_SYS:
			QObject::connect(setDetailed_act, SIGNAL(triggered()), receiver, SLOT(set_alt_outputflag()) );
			this->flag_detailed_set_required=true;
			break;
		default:
			this->flag_detailed_set_required=false;
			break;
	}
}
//_________
//private slots
//Start the text-searcher dialog
void Sys_Output_Text_Wid::start_text_searcher(void){
	this->my_searcher.set_ptr_text_edit(this);
	this->my_searcher.start_dialog();
}
//________
//private
//Set up all the actions in constructor
void Sys_Output_Text_Wid::createActions(void){
	this->selectAll_act = new QAction(tr("&Select All"), this);
	this->selectAll_act->setShortcut(tr("Ctrl+A"));
	this->copy_act = new QAction(tr("&Copy"), this);
	this->copy_act->setShortcut(tr("Ctrl+C"));
    this->clear_act = new QAction(tr("&Clear"), this);
	this->short_cut_clear=new QShortcut(tr("Ctrl+Del"),this);
	this->clear_act->setShortcut(tr("Ctrl+DEL"));
    this->setDetailed_act = new QAction(tr("&Set Detailed..."), this);
	this->short_cut_clear->setContext(Qt::WidgetShortcut);

    this->searching_act = new QAction(tr("&Search..."), this);
	this->short_cut_search = new QShortcut(Qt::Key_F3,this);
	this->short_cut_search->setContext(Qt::WidgetShortcut);
	this->searching_act->setShortcut(Qt::Key_F3);
}
//Connect all the signals to all the slots
void Sys_Output_Text_Wid::connectActions(void){
	QObject::connect(selectAll_act, SIGNAL(triggered()), this, SLOT(selectAll()));
	QObject::connect(copy_act, SIGNAL(triggered()), this, SLOT(copy()));
	QObject::connect(searching_act, SIGNAL(triggered()), &this->my_searcher, SLOT(start_dialog()));
	QObject::connect(short_cut_search, SIGNAL(activated()), this, SLOT(start_text_searcher()));
}
//Context menu, provides some functionalities by mouse right click
void Sys_Output_Text_Wid::contextMenuEvent(QContextMenuEvent *event){
	this->my_searcher.set_ptr_text_edit(this);
	QMenu *contextMenu = new QMenu(this);
	contextMenu->addAction(this->selectAll_act);
    contextMenu->addAction(this->copy_act);
	 contextMenu->addAction( this->searching_act);
	if(this->flag_clear_required==true){
		contextMenu->addAction(this->clear_act);
	}
	if(this->flag_detailed_set_required==true){
		contextMenu->addAction(this->setDetailed_act);
	}

	contextMenu->exec(event->globalPos());
}