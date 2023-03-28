#include "Main_Headers_Precompiled.h"
//#include "Sys_Data_Tree_Wid.h"

//Default constructor
Sys_Data_Tree_Wid::Sys_Data_Tree_Wid(QWidget *parent) : QWidget(parent){
	this->setupUi(this);

	this->ptr_data_tab=NULL;
	this->ptr_data_tab_overall=NULL;
	this->ptr_main_wid=NULL;

	this->ptr_data_tab_hyd=NULL;
	this->ptr_data_tab_dam=NULL;
	this->ptr_data_tab_fpl=NULL;
	this->ptr_data_tab_madm=NULL;
	this->ptr_data_tab_risk=NULL;
	this->ptr_data_tab_alt=NULL;
	this->ptr_data_tab_cost=NULL;

    //this->ptr_data_dam=NULL;

	this->current_project=NULL;

	this->data_tree->setColumnWidth(0,190);
	this->data_tree->setColumnWidth(1,15);

	this->system_id=NULL;

	this->clear_total();

	this->previous_db_clicked=NULL;
	this->active_item=NULL;

	this->double_clicked=false;

	this->current_type=_sys_system_modules::SYS_SYS;

	this->current_hor_slider_pos=0;
	this->current_ver_slider_pos=0;

	//action refresh tree
	QAction *refreshAction = new QAction(tr("Refresh"), this->data_tree);
	QObject::connect(refreshAction, SIGNAL(triggered()), SLOT(refresh_tree()));
	this->data_tree->addAction(refreshAction);
	

	//action collapse all
	QAction *act_collapse =new QAction(tr("Collapse all"), this->data_tree);
	QObject::connect(act_collapse, SIGNAL(triggered()), this, SLOT(collapse_all_branch()));
	this->data_tree->addAction(act_collapse);

    this->data_tree->setContextMenuPolicy(Qt::ActionsContextMenu);

	//slot connect
	QObject::connect(this->data_tree, SIGNAL(itemDoubleClicked(QTreeWidgetItem *, int )) , this, SLOT(tree_item_clicked(QTreeWidgetItem *, int )));
	QObject::connect(this->data_tree, SIGNAL(itemClicked(QTreeWidgetItem *, int )) , this, SLOT(change_focus_tab_widget(QTreeWidgetItem *, int )));
	QObject::connect(this->data_tree, SIGNAL(itemActivated(QTreeWidgetItem *, int )) , this, SLOT(set_current_active_item(QTreeWidgetItem *, int )));

	Sys_Memory_Count::self()->add_mem(sizeof(Sys_Data_Tree_Wid), _sys_system_modules::SYS_SYS);
}
//Default destructor
Sys_Data_Tree_Wid::~Sys_Data_Tree_Wid(void){
	this->clear_total();

	Sys_Memory_Count::self()->minus_mem(sizeof(Sys_Data_Tree_Wid), _sys_system_modules::SYS_SYS);
}
//___________
//public
//Set a database connection
void Sys_Data_Tree_Wid::set_db_connection(QSqlDatabase *ptr_database){
	if(ptr_database==NULL){
		this->clear_total();
		this->setEnabled(false);
	}
	else{
		this->clone_database(ptr_database);
		this->set_up_tree_predefinied();
		this->setEnabled(true);
	}
}
//Set-up the data into the tree from the database
void Sys_Data_Tree_Wid::set_up_tree_data(void){
	this->data_tree->setEnabled(false);
	try{
		emit send_txt2statusbar("Load data to data project tree view...", 0);
		//set the data of the project
		if(Sys_Project::get_project_type()==_sys_project_type::proj_all){
			this->set_up_current_data_fpl();
			this->set_up_current_data_hyd();
			this->set_up_current_data_dam();
			this->set_up_current_data_risk();
			this->set_up_current_data_cost();
			this->set_up_current_data_madm();
			this->set_up_current_data_alt();
		}
		else if(Sys_Project::get_project_type()==_sys_project_type::proj_hyd || Sys_Project::get_project_type() == _sys_project_type::proj_hyd_temp){
			this->set_up_current_data_hyd();
		}
		else if(Sys_Project::get_project_type()==_sys_project_type::proj_dam){
			this->set_up_current_data_hyd();
			this->set_up_current_data_dam();
		}
		else if(Sys_Project::get_project_type()==_sys_project_type::proj_risk){
			this->set_up_current_data_fpl();
			this->set_up_current_data_hyd();
			this->set_up_current_data_dam();
			this->set_up_current_data_risk();
		}
		else if(Sys_Project::get_project_type()==_sys_project_type::proj_dam_hyd){
			this->set_up_current_data_hyd();
			this->set_up_current_data_dam();
		}
		else if(Sys_Project::get_project_type()==_sys_project_type::proj_fpl){
			this->set_up_current_data_fpl();
		}
		this->set_number_items();
		emit send_txt2statusbar("Ready", 0);
		this->data_tree->setEnabled(true);
	}
	catch(Error msg){
		this->data_tree->setEnabled(true);
		ostringstream info;
		info << "Try to set up the data-tree view"<< endl;
		msg.make_second_info(info.str());
		throw msg;
	}
}
//Set pointer to the overall data tab-widget, which includes GIS, DATABASE and DATA
void Sys_Data_Tree_Wid::set_ptr_overall_data_tab_wid(QTabWidget *ptr_tab_wid){
	this->ptr_data_tab_overall=ptr_tab_wid;
}
//Set a pointer to the main window
void Sys_Data_Tree_Wid::set_ptr_main_window(QMainWindow *ptr_main){
	this->ptr_main_wid=ptr_main;
}
//Set the pointer to the data-tab widget
void Sys_Data_Tree_Wid::set_ptr_data_tab_wid(QTabWidget *ptr_tab_wid){
	this->ptr_data_tab=ptr_tab_wid;
}
//Set the pointer to the data-tab widget tab hyd
void Sys_Data_Tree_Wid::set_ptr_data_tab_hyd(QWidget *ptr_tab){
	this->ptr_data_tab_hyd=ptr_tab;
}
//Set the pointer to the data-tab widget tab dam
void Sys_Data_Tree_Wid::set_ptr_data_tab_dam(QWidget *ptr_tab){
	this->ptr_data_tab_dam=ptr_tab;
}
//Set the pointer to the data-tab widget tab fpl
void Sys_Data_Tree_Wid::set_ptr_data_tab_fpl(QWidget *ptr_tab){
	this->ptr_data_tab_fpl=ptr_tab;
}
//Set the pointer to the data-tab widget tab madm
void Sys_Data_Tree_Wid::set_ptr_data_tab_madm(QWidget *ptr_tab){
	this->ptr_data_tab_madm=ptr_tab;
}
//Set the pointer to the data-tab widget tab risk
void Sys_Data_Tree_Wid::set_ptr_data_tab_risk(QWidget *ptr_tab){
	this->ptr_data_tab_risk=ptr_tab;
}
//Set the pointer to the data-tab widget tab alt
void Sys_Data_Tree_Wid::set_ptr_data_tab_alt(QWidget *ptr_tab){
	this->ptr_data_tab_alt=ptr_tab;
}
//Set the pointer to the data-tab widget tab cost
void Sys_Data_Tree_Wid::set_ptr_data_tab_cost(QWidget *ptr_tab){
	this->ptr_data_tab_cost=ptr_tab;
}
//Set a pointer to the system id
void Sys_Data_Tree_Wid::set_ptr_sys_system_id(_sys_system_id *id){
	this->system_id=id;
}
//Set a pointer to the current project
void Sys_Data_Tree_Wid::set_ptr_current_project(Sys_Project *ptr_project){
	this->current_project=ptr_project;
}
//____________
//public slots
//Recieve, that a tree item is double clicked
void Sys_Data_Tree_Wid::tree_item_clicked(QTreeWidgetItem *item, int column){
	if(item==NULL){
		return;
	}
	if(item->parent()==NULL){
		return;
	}
	this->double_clicked=true;
	//set the item type and the focus, get the current type
	this->change_focus_tab_widget(item, column);
	//set it to bold
	this->set_doubledclicked2bold(item);
	this->previous_db_clicked=item;
	this->double_clicked=false;

	//set widget to tab widget
	if(item->data(0, Qt::UserRole).toString().length()==0){
		return;
	}
	try{
		emit send_enabled_main(false);
		emit send_txt2statusbar("Widget is loading. Waiting for database....",0);
		//hyd
		if(this->current_type==_sys_system_modules::HYD_SYS){
			this->widget2hyd(item);
		}
		//dam
		else if(this->current_type==_sys_system_modules::DAM_SYS){
			this->widget2dam(item);
		}
		//fpl
		else if(this->current_type==_sys_system_modules::FPL_SYS){
			this->widget2fpl(item);
		}
		//madm
		else if(this->current_type==_sys_system_modules::MADM_SYS){
			this->widget2madm(item);
		}
		//risk
		else if(this->current_type==_sys_system_modules::RISK_SYS){
			this->widget2risk(item);
		}
		//alt
		else if(this->current_type==_sys_system_modules::ALT_SYS){
			this->widget2alt(item);
		}
		//alt
		else if(this->current_type==_sys_system_modules::COST_SYS){
			this->widget2cost(item);
		}

		emit send_txt2statusbar("Ready",0);
		emit send_enabled_main(true);
	}
	catch(Error msg){
		emit send_txt2statusbar("Ready",0);
		emit send_enabled_main(true);
		ostringstream info;
		info <<"Try to set up the data to the data view"<<endl;
		msg.make_second_info(info.str());
		msg.output_msg(0);
		return;
	}
}
//Change the focus of the tab widget, when the item is clicked
void Sys_Data_Tree_Wid::change_focus_tab_widget(QTreeWidgetItem *item, int column){
	if(item->parent()==NULL){
		int buff;//not needed, surpress warnings; just column 0 is used
		buff=column;
		return;
	}

	bool just_module=false;
	string id=item->data(0,Qt::AccessibleDescriptionRole).toString().toStdString();
	//set current module type
	just_module=this->find_module_type(id);
	//decide the focus
	if(this->current_type==_sys_system_modules::HYD_SYS && (just_module==true || this->double_clicked==true)){
		if(this->ptr_data_tab_hyd!=NULL){
			this->ptr_data_tab->setCurrentWidget(this->ptr_data_tab_hyd);
		}
	}
	else if(this->current_type==_sys_system_modules::DAM_SYS && (just_module==true || this->double_clicked==true)){
		if(this->ptr_data_tab_dam!=NULL){
			this->ptr_data_tab->setCurrentWidget(this->ptr_data_tab_dam);
		}
	}
	else if(this->current_type==_sys_system_modules::FPL_SYS && (just_module==true || this->double_clicked==true)){
		if(this->ptr_data_tab_fpl!=NULL){
			this->ptr_data_tab->setCurrentWidget(this->ptr_data_tab_fpl);
		}
	}
	else if(this->current_type==_sys_system_modules::MADM_SYS && (just_module==true || this->double_clicked==true)){
		if(this->ptr_data_tab_madm!=NULL){
			this->ptr_data_tab->setCurrentWidget(this->ptr_data_tab_madm);
		}
	}
	else if(this->current_type==_sys_system_modules::RISK_SYS && (just_module==true || this->double_clicked==true)){
		if(this->ptr_data_tab_risk!=NULL){
			this->ptr_data_tab->setCurrentWidget(this->ptr_data_tab_risk);
		}
	}
	else if(this->current_type==_sys_system_modules::ALT_SYS && (just_module==true || this->double_clicked==true)){
		if(this->ptr_data_tab_alt!=NULL){
			this->ptr_data_tab->setCurrentWidget(this->ptr_data_tab_alt);
		}
	}
	else if(this->current_type==_sys_system_modules::COST_SYS && (just_module==true || this->double_clicked==true)){
		if(this->ptr_data_tab_cost!=NULL){
			this->ptr_data_tab->setCurrentWidget(this->ptr_data_tab_cost);
		}
	}
}
//Refresh the tree
void Sys_Data_Tree_Wid::refresh_tree(void){
	try{
		this->data_tree->setColumnWidth(0,190);
		this->data_tree->setColumnWidth(1,15);
		//clear tree  widget
		this->active_item=NULL;
		string path;
		if(this->previous_db_clicked!=NULL){
			path=this->previous_db_clicked->data(0,Qt::AccessibleDescriptionRole).toString().toStdString();
		}
		this->previous_db_clicked=NULL;
		//delete the layouts
		this->delete_dam_lay();
		this->delete_fpl_lay();
		this->delete_hyd_lay();
		this->delete_madm_lay();
		this->delete_risk_lay();
		//clear tree
		this->data_tree->clear();
		//new setup
		this->set_up_tree_predefinied();
		this->set_up_tree_data();
		//restore old widget in the display view
		QTreeWidgetItem *old_item=NULL;
		if(path.length()!=0){
			if(this->find_module_type(path)==false){
				old_item=this->start_module_tree_search(path, this->current_type);
				if(old_item!=NULL){
					this->tree_item_clicked(old_item,0);
					this->set_doubledclicked2bold(old_item);
					QTreeWidgetItem *child=old_item;
					while(child->parent()!=NULL){
						this->data_tree->expandItem(child->parent());
						child=child->parent();
					}
				}
			}
		}
	}
	catch(Error msg){
		msg.output_msg(0);
		return;
	}
}
//_____________
//private
//Set-up the tree widget with the pre-defined branches corresponding to the project type (_sys_project_type)
void Sys_Data_Tree_Wid::set_up_tree_predefinied(void){
	emit send_txt2statusbar("Set-up project tree view...", 0);
	this->data_tree->clear();

	QTreeWidgetItem *root = new QTreeWidgetItem(this->data_tree);
	root->setText(0, Sys_Project::get_project_name().c_str());
	root->setExpanded(true);
	//set bold font to the first item
	QFont font = root->font(0);
    font.setBold(true);
    root->setFont(0, font);

	if(Sys_Project::get_project_type()==_sys_project_type::proj_all){
		this->set_up_predefined_fpl(root);
		this->set_up_predefined_hyd(root);
		this->set_up_predefined_dam(root);
		this->set_up_predefined_risk(root);

		this->set_up_predefined_cost(root);
		this->set_up_predefined_madm(root);
		this->set_up_predefined_alt(root);
	}
	else if(Sys_Project::get_project_type()==_sys_project_type::proj_hyd || Sys_Project::get_project_type() == _sys_project_type::proj_hyd_temp){
		this->set_up_predefined_hyd(root);
	}
	else if(Sys_Project::get_project_type()==_sys_project_type::proj_dam){
		this->set_up_predefined_hyd(root);
		this->set_up_predefined_dam(root);
	}
	else if(Sys_Project::get_project_type()==_sys_project_type::proj_risk){
		this->set_up_predefined_fpl(root);
		this->set_up_predefined_hyd(root);
		this->set_up_predefined_dam(root);
		this->set_up_predefined_risk(root);
	}
	else if(Sys_Project::get_project_type()==_sys_project_type::proj_dam_hyd){
		this->set_up_predefined_hyd(root);
		this->set_up_predefined_dam(root);
	}
	else if(Sys_Project::get_project_type()==_sys_project_type::proj_fpl){
		this->set_up_predefined_fpl(root);
	}
	emit send_txt2statusbar("Ready", 0);
}
//Set-up predefinied branch for module hydraulic
void Sys_Data_Tree_Wid::set_up_predefined_hyd(QTreeWidgetItem *root){
	//mainfolder
	ostringstream buff_main;
	QTreeWidgetItem *modules = new QTreeWidgetItem(root);
	modules->setText(0, sys_label::str_hyd.c_str());
	buff_main << "/"<<sys_label::str_hyd<<"/";
	modules->setData(0,Qt::AccessibleDescriptionRole, buff_main.str().c_str());
	modules->setToolTip(0, buff_main.str().c_str());
	QIcon icon1;
	icon1.addFile(":hyd_icon");
	modules->setIcon(0,icon1);

		//subfolder
		ostringstream buff_sub;
		QTreeWidgetItem *sub_folder;
		if(Sys_Project::get_project_type()!=_sys_project_type::proj_dam){
			sub_folder=new QTreeWidgetItem(modules);
			sub_folder->setText(0, sys_data_tree_id::id_config.c_str());
			buff_sub<<buff_main.str() << sys_data_tree_id::id_config<<"/";
			sub_folder->setData(0,Qt::AccessibleDescriptionRole, buff_sub.str().c_str());
			sub_folder->setData(0,Qt::UserRole, "1");
			sub_folder->setToolTip(0, buff_sub.str().c_str());
			if(Hyd_Boundary_Szenario_Management::check_base_scenario_is_set(&this->database)==true){
				sub_folder->setData(1, Qt::DisplayRole,1);
			}
			else{
				sub_folder->setData(1, Qt::DisplayRole,0);
			}
		}

		buff_sub.str("");
		sub_folder=new QTreeWidgetItem(modules);
		sub_folder->setText(0, sys_data_tree_id::hyd_bound_sc_id.c_str());
		buff_sub<<buff_main.str() << sys_data_tree_id::hyd_bound_sc_id<<"/";
		sub_folder->setData(0,Qt::AccessibleDescriptionRole, buff_sub.str().c_str());
		sub_folder->setData(0,Qt::UserRole, "1");
		sub_folder->setToolTip(0, buff_sub.str().c_str());
		Hyd_Boundary_Szenario_Management boundary;
		boundary.set_systemid(*this->system_id);
		boundary.set_szenario_per_db(&this->database);
		int number=boundary.get_number_sz();
		sub_folder->setData(1, Qt::DisplayRole,number);

		if(Sys_Project::get_project_type()!=_sys_project_type::proj_dam){
			buff_sub.str("");
			sub_folder=new QTreeWidgetItem(modules);
			sub_folder->setText(0, sys_data_tree_id::hyd_river_id.c_str());
			buff_sub<<buff_main.str() << sys_data_tree_id::hyd_river_id<<"/";
			sub_folder->setData(0,Qt::AccessibleDescriptionRole, buff_sub.str().c_str());
			sub_folder->setToolTip(0, buff_sub.str().c_str());
		}

		buff_sub.str("");
		sub_folder=new QTreeWidgetItem(modules);
		sub_folder->setText(0, sys_data_tree_id::hyd_floodplain_id.c_str());
		buff_sub<<buff_main.str() << sys_data_tree_id::hyd_floodplain_id<<"/";
		sub_folder->setData(0,Qt::AccessibleDescriptionRole, buff_sub.str().c_str());
		sub_folder->setToolTip(0, buff_sub.str().c_str());
}
//Set-up predefined branch for module damage
void Sys_Data_Tree_Wid::set_up_predefined_dam(QTreeWidgetItem *root){
	//mainfolder
	ostringstream buff_main;
	QTreeWidgetItem *modules = new QTreeWidgetItem(root);
	modules->setText(0, sys_label::str_dam.c_str());
	buff_main << "/"<<sys_label::str_dam<<"/";
	modules->setData(0,Qt::AccessibleDescriptionRole, buff_main.str().c_str());
	modules->setToolTip(0, buff_main.str().c_str());
	QIcon icon1;
	icon1.addFile(":dam_icon");
	modules->setIcon(0,icon1);

		//subfolder
		ostringstream buff_sub;
		ostringstream buff_sub_sub_eco;
		ostringstream buff_sub_sub;
		//ecn
		QTreeWidgetItem *sub_folder=new QTreeWidgetItem(modules);
		sub_folder->setText(0, sys_data_tree_id::dam_ecn_id.c_str());
		buff_sub<<buff_main.str() << sys_data_tree_id::dam_ecn_id<<"/";
		sub_folder->setData(0,Qt::AccessibleDescriptionRole, buff_sub.str().c_str());
		sub_folder->setToolTip(0, buff_sub.str().c_str());
		//set icon
		QIcon ecn_icon;
		ecn_icon.addFile(":dam_icon" );
		sub_folder->setIcon(0, ecn_icon);

			//sub_subfolder
			QTreeWidgetItem *sub_sub_folder=new QTreeWidgetItem(sub_folder);
			sub_sub_folder->setText(0, sys_data_tree_id::dam_raster_id.c_str());
			buff_sub_sub<<buff_sub.str() << sys_data_tree_id::dam_raster_id<<"/";
			sub_sub_folder->setData(0,Qt::AccessibleDescriptionRole, buff_sub_sub.str().c_str());
			sub_sub_folder->setToolTip(0, buff_sub_sub.str().c_str());

			buff_sub_sub.str("");
			sub_sub_folder=new QTreeWidgetItem(sub_folder);
			sub_sub_folder->setText(0, sys_data_tree_id::dam_land_use_cat.c_str());
			buff_sub_sub<<buff_sub.str() << sys_data_tree_id::dam_land_use_cat<<"/";
			sub_sub_folder->setData(0,Qt::AccessibleDescriptionRole, buff_sub_sub.str().c_str());
			sub_sub_folder->setToolTip(0, buff_sub_sub.str().c_str());

		//eco
		buff_sub.str("");
		sub_folder=new QTreeWidgetItem(modules);
		sub_folder->setText(0, sys_data_tree_id::dam_eco_id.c_str());
		buff_sub<<buff_main.str() << sys_data_tree_id::dam_eco_id<<"/";
		sub_folder->setData(0,Qt::AccessibleDescriptionRole, buff_sub.str().c_str());
		sub_folder->setToolTip(0, buff_sub.str().c_str());

		//set icon
		QIcon eco_icon;
		eco_icon.addFile(":eco_icon" );
		sub_folder->setIcon(0, eco_icon);

			//btype
			QTreeWidgetItem *sub_sub_folder_eco=new QTreeWidgetItem(sub_folder);
			sub_sub_folder_eco->setText(0, sys_data_tree_id::dam_eco_btype_id.c_str());
			buff_sub_sub_eco<<buff_sub.str() << sys_data_tree_id::dam_eco_btype_id<<"/";
			sub_sub_folder_eco->setData(0,Qt::AccessibleDescriptionRole, buff_sub_sub_eco.str().c_str());
			sub_sub_folder_eco->setToolTip(0, buff_sub_sub_eco.str().c_str());
			//set icon
			QIcon eco_b_icon;
			eco_b_icon.addFile(":eco_btype_icon" );
			sub_sub_folder_eco->setIcon(0, eco_b_icon);

				buff_sub_sub.str("");
				sub_sub_folder=new QTreeWidgetItem(sub_sub_folder_eco);
				sub_sub_folder->setText(0, sys_data_tree_id::dam_raster_id.c_str());
				buff_sub_sub<<buff_sub_sub_eco.str() << sys_data_tree_id::dam_raster_id<<"/";
				sub_sub_folder->setData(0,Qt::AccessibleDescriptionRole, buff_sub_sub.str().c_str());
				sub_sub_folder->setToolTip(0, buff_sub_sub.str().c_str());

				buff_sub_sub.str("");
				sub_sub_folder=new QTreeWidgetItem(sub_sub_folder_eco);
				sub_sub_folder->setText(0, sys_data_tree_id::dam_biotope_type_id.c_str());
				buff_sub_sub<<buff_sub_sub_eco.str() << sys_data_tree_id::dam_biotope_type_id<<"/";
				sub_sub_folder->setData(0,Qt::AccessibleDescriptionRole, buff_sub_sub.str().c_str());
				sub_sub_folder->setToolTip(0, buff_sub_sub.str().c_str());

			//soil-eorsion
			buff_sub_sub_eco.str("");
			sub_sub_folder_eco=new QTreeWidgetItem(sub_folder);
			sub_sub_folder_eco->setText(0, sys_data_tree_id::dam_eco_soil_id.c_str());
			buff_sub_sub_eco<<buff_sub.str() << sys_data_tree_id::dam_eco_soil_id<<"/";
			sub_sub_folder_eco->setData(0,Qt::AccessibleDescriptionRole, buff_sub_sub_eco.str().c_str());
			sub_sub_folder_eco->setToolTip(0, buff_sub_sub_eco.str().c_str());
			//set icon
			QIcon eco_s_icon;
			eco_s_icon.addFile(":eco_soil_icon" );
			sub_sub_folder_eco->setIcon(0, eco_s_icon);

				buff_sub_sub.str("");
				sub_sub_folder=new QTreeWidgetItem(sub_sub_folder_eco);
				sub_sub_folder->setText(0, sys_data_tree_id::dam_raster_id.c_str());
				buff_sub_sub<<buff_sub_sub_eco.str() << sys_data_tree_id::dam_raster_id<<"/";
				sub_sub_folder->setData(0,Qt::AccessibleDescriptionRole, buff_sub_sub.str().c_str());
				sub_sub_folder->setToolTip(0, buff_sub_sub.str().c_str());

				buff_sub_sub.str("");
				sub_sub_folder=new QTreeWidgetItem(sub_sub_folder_eco);
				sub_sub_folder->setText(0, sys_data_tree_id::dam_soil_cost_id.c_str());
				buff_sub_sub<<buff_sub_sub_eco.str() << sys_data_tree_id::dam_soil_cost_id<<"/";
				sub_sub_folder->setData(0,Qt::AccessibleDescriptionRole, buff_sub_sub.str().c_str());
				sub_sub_folder->setToolTip(0, buff_sub_sub.str().c_str());

				buff_sub_sub.str("");
				sub_sub_folder=new QTreeWidgetItem(sub_sub_folder_eco);
				sub_sub_folder->setText(0, sys_data_tree_id::dam_soil_ero_id.c_str());
				buff_sub_sub<<buff_sub_sub_eco.str() << sys_data_tree_id::dam_soil_ero_id<<"/";
				sub_sub_folder->setData(0,Qt::AccessibleDescriptionRole, buff_sub_sub.str().c_str());
				sub_sub_folder->setToolTip(0, buff_sub_sub.str().c_str());

		//pop
		buff_sub.str("");
		sub_folder=new QTreeWidgetItem(modules);
		sub_folder->setText(0, sys_data_tree_id::dam_pop_id.c_str());
		buff_sub<<buff_main.str() << sys_data_tree_id::dam_pop_id<<"/";
		sub_folder->setData(0,Qt::AccessibleDescriptionRole, buff_sub.str().c_str());
		sub_folder->setToolTip(0, buff_sub.str().c_str());
		//set icon
		QIcon pop_icon;
		pop_icon.addFile(":pop_icon" );
		sub_folder->setIcon(0, pop_icon);

			//sub_subfolder
			buff_sub_sub.str("");
			sub_sub_folder=new QTreeWidgetItem(sub_folder);
			sub_sub_folder->setText(0, sys_data_tree_id::dam_raster_id.c_str());
			buff_sub_sub<<buff_sub.str() << sys_data_tree_id::dam_raster_id<<"/";
			sub_sub_folder->setData(0,Qt::AccessibleDescriptionRole, buff_sub_sub.str().c_str());
			sub_sub_folder->setToolTip(0, buff_sub_sub.str().c_str());

			buff_sub_sub.str("");
			sub_sub_folder=new QTreeWidgetItem(sub_folder);
			sub_sub_folder->setText(0, sys_data_tree_id::dam_vul_cat.c_str());
			buff_sub_sub<<buff_sub.str() << sys_data_tree_id::dam_vul_cat<<"/";
			sub_sub_folder->setData(0,Qt::AccessibleDescriptionRole, buff_sub_sub.str().c_str());
			sub_sub_folder->setToolTip(0, buff_sub_sub.str().c_str());

		//pys
		buff_sub.str("");
		sub_folder=new QTreeWidgetItem(modules);
		sub_folder->setText(0, sys_data_tree_id::dam_pys_id.c_str());
		buff_sub<<buff_main.str() << sys_data_tree_id::dam_pys_id<<"/";
		sub_folder->setData(0,Qt::AccessibleDescriptionRole, buff_sub.str().c_str());
		sub_folder->setToolTip(0, buff_sub.str().c_str());
		//set icon
		QIcon pys_icon;
		pys_icon.addFile(":pys_icon" );
		sub_folder->setIcon(0, pys_icon);

			//sub_subfolder
			buff_sub_sub.str("");
			sub_sub_folder=new QTreeWidgetItem(sub_folder);
			sub_sub_folder->setText(0, sys_data_tree_id::dam_raster_id.c_str());
			buff_sub_sub<<buff_sub.str() << sys_data_tree_id::dam_raster_id<<"/";
			sub_sub_folder->setData(0,Qt::AccessibleDescriptionRole, buff_sub_sub.str().c_str());
			sub_sub_folder->setToolTip(0, buff_sub_sub.str().c_str());

			//buff_sub_sub.str("");
			//sub_sub_folder=new QTreeWidgetItem(sub_folder);
			//sub_sub_folder->setText(0, sys_data_tree_id::dam_density_func_id.c_str());
			//buff_sub_sub<<buff_sub.str() << sys_data_tree_id::dam_density_func_id<<"/";
			//sub_sub_folder->setData(0,Qt::AccessibleDescriptionRole, buff_sub_sub.str().c_str());
			//sub_sub_folder->setToolTip(0, buff_sub_sub.str().c_str());

			buff_sub_sub.str("");
			sub_sub_folder=new QTreeWidgetItem(sub_folder);
			sub_sub_folder->setText(0, sys_data_tree_id::dam_category_id.c_str());
			buff_sub_sub<<buff_sub.str() << sys_data_tree_id::dam_category_id<<"/";
			sub_sub_folder->setData(0,Qt::AccessibleDescriptionRole, buff_sub_sub.str().c_str());
			sub_sub_folder->setToolTip(0, buff_sub_sub.str().c_str());

		//sc
		buff_sub.str("");
		sub_folder=new QTreeWidgetItem(modules);
		sub_folder->setText(0, sys_data_tree_id::dam_sc_id.c_str());
		buff_sub<<buff_main.str() << sys_data_tree_id::dam_sc_id<<"/";
		sub_folder->setData(0,Qt::AccessibleDescriptionRole, buff_sub.str().c_str());
		sub_folder->setToolTip(0, buff_sub.str().c_str());
		//set icon
		QIcon sc_icon;
		sc_icon.addFile(":sc_icon" );
		sub_folder->setIcon(0, sc_icon);

			//sub_subfolder
			buff_sub_sub.str("");
			sub_sub_folder=new QTreeWidgetItem(sub_folder);
			sub_sub_folder->setText(0, sys_data_tree_id::dam_point_id.c_str());
			buff_sub_sub<<buff_sub.str() << sys_data_tree_id::dam_point_id<<"/";
			sub_sub_folder->setData(0,Qt::AccessibleDescriptionRole, buff_sub_sub.str().c_str());
			sub_sub_folder->setToolTip(0, buff_sub_sub.str().c_str());
			sub_sub_folder->setData(0,Qt::UserRole, "1");
			QSqlQueryModel raster;
			int number=0;
			number=Dam_Sc_Point::count_relevant_points_database(&raster,&this->database,*this->system_id, false);
			sub_sub_folder->setData(1, Qt::DisplayRole,number);

			buff_sub_sub.str("");
			sub_sub_folder=new QTreeWidgetItem(sub_folder);
			sub_sub_folder->setText(0, sys_data_tree_id::dam_subcategory_id.c_str());
			buff_sub_sub<<buff_sub.str() << sys_data_tree_id::dam_subcategory_id<<"/";
			sub_sub_folder->setData(0,Qt::AccessibleDescriptionRole, buff_sub_sub.str().c_str());
			sub_sub_folder->setToolTip(0, buff_sub_sub.str().c_str());
			sub_sub_folder->setData(0,Qt::UserRole, "1");
			number=Dam_Sc_Subcategory::select_relevant_subcategory_database(&raster,&this->database);
			sub_sub_folder->setData(1, Qt::DisplayRole,number);


			//CI
			buff_sub.str("");
			sub_folder = new QTreeWidgetItem(modules);
			sub_folder->setText(0, sys_data_tree_id::dam_ci_id.c_str());
			buff_sub << buff_main.str() << sys_data_tree_id::dam_ci_id << "/";
			sub_folder->setData(0, Qt::AccessibleDescriptionRole, buff_sub.str().c_str());
			sub_folder->setToolTip(0, buff_sub.str().c_str());

			//set icon
			QIcon ci_icon;
			ci_icon.addFile(":ci_icon");
			sub_folder->setIcon(0, ci_icon);

			//sub_subfolder
			buff_sub_sub.str("");
			sub_sub_folder = new QTreeWidgetItem(sub_folder);
			sub_sub_folder->setText(0, sys_data_tree_id::dam_point_id.c_str());
			buff_sub_sub << buff_sub.str() << sys_data_tree_id::dam_point_id << "/";
			sub_sub_folder->setData(0, Qt::AccessibleDescriptionRole, buff_sub_sub.str().c_str());
			sub_sub_folder->setToolTip(0, buff_sub_sub.str().c_str());
			sub_sub_folder->setData(0, Qt::UserRole, "1");
			raster.clear();
			number = 0;
			number = Dam_CI_Point::count_relevant_points_database(&raster, &this->database, *this->system_id, false);
			sub_sub_folder->setData(1, Qt::DisplayRole, number);

			buff_sub_sub.str("");
			sub_sub_folder = new QTreeWidgetItem(sub_folder);
			sub_sub_folder->setText(0, sys_data_tree_id::dam_polygon_id.c_str());
			buff_sub_sub << buff_sub.str() << sys_data_tree_id::dam_polygon_id << "/";
			sub_sub_folder->setData(0, Qt::AccessibleDescriptionRole, buff_sub_sub.str().c_str());
			sub_sub_folder->setToolTip(0, buff_sub_sub.str().c_str());
			sub_sub_folder->setData(0, Qt::UserRole, "1");
			number = Dam_CI_Polygon::count_relevant_polygon_database(&raster, &this->database, *this->system_id, false);
			sub_sub_folder->setData(1, Qt::DisplayRole, number);

	//result
	buff_sub.str("");
	sub_folder=new QTreeWidgetItem(modules);
	sub_folder->setText(0, sys_data_tree_id::id_results.c_str());
	buff_sub<<buff_main.str() << sys_data_tree_id::id_results<<"/";
	sub_folder->setData(0,Qt::AccessibleDescriptionRole, buff_sub.str().c_str());
	sub_folder->setToolTip(0, buff_sub.str().c_str());

			//sub_subfolder (nobreak)
			buff_sub_sub.str("");
			sub_sub_folder=new QTreeWidgetItem(sub_folder);
			sub_sub_folder->setText(0, risk_label::risk_nobreak.c_str());
			buff_sub_sub<<buff_sub.str() << risk_label::risk_nobreak<<"/";
			sub_sub_folder->setData(0,Qt::AccessibleDescriptionRole, buff_sub_sub.str().c_str());
			sub_sub_folder->setToolTip(0, buff_sub_sub.str().c_str());
			if(Sys_Project::get_project_type()==_sys_project_type::proj_all || Sys_Project::get_project_type()==_sys_project_type::proj_risk ){
				//scenario
				buff_sub_sub.str("");
				sub_sub_folder=new QTreeWidgetItem(sub_folder);
				sub_sub_folder->setText(0, risk_label::risk_scenario.c_str());
				buff_sub_sub<<buff_sub.str() << risk_label::risk_scenario<<"/";
				sub_sub_folder->setData(0,Qt::AccessibleDescriptionRole, buff_sub_sub.str().c_str());
				sub_sub_folder->setToolTip(0, buff_sub_sub.str().c_str());

				//catchment
				buff_sub_sub.str("");
				sub_sub_folder=new QTreeWidgetItem(sub_folder);
				sub_sub_folder->setText(0, risk_label::risk_catchment.c_str());
				buff_sub_sub<<buff_sub.str() << risk_label::risk_catchment<<"/";
				sub_sub_folder->setData(0,Qt::AccessibleDescriptionRole, buff_sub_sub.str().c_str());
				sub_sub_folder->setToolTip(0, buff_sub_sub.str().c_str());
			}
}
//Set-up predefined branch for module fpl
void Sys_Data_Tree_Wid::set_up_predefined_fpl(QTreeWidgetItem *root){
	//mainfolder
	ostringstream buff_main;
	QTreeWidgetItem *modules = new QTreeWidgetItem(root);
	modules->setText(0, sys_label::str_fpl.c_str());
	buff_main << "/"<<sys_label::str_fpl<<"/";
	modules->setData(0,Qt::AccessibleDescriptionRole, buff_main.str().c_str());
	modules->setToolTip(0, buff_main.str().c_str());
	QIcon icon1;
	icon1.addFile(":fpl_icon");
	modules->setIcon(0,icon1);

		//subfolder
		ostringstream buff_sub;
		QTreeWidgetItem *sub_folder=new QTreeWidgetItem(modules);
		sub_folder->setText(0, sys_data_tree_id::id_config.c_str());
		buff_sub<<buff_main.str() << sys_data_tree_id::id_config<<"/";
		sub_folder->setData(0,Qt::AccessibleDescriptionRole, buff_sub.str().c_str());
		sub_folder->setData(0,Qt::UserRole, "1");
		sub_folder->setToolTip(0, buff_sub.str().c_str());
		sub_folder->setData(1, Qt::DisplayRole,1);

		buff_sub.str("");
		sub_folder=new QTreeWidgetItem(modules);
		sub_folder->setText(0, sys_data_tree_id::fpl_fault_tree.c_str());
		buff_sub<<buff_main.str() << sys_data_tree_id::fpl_fault_tree<<"/";
		sub_folder->setData(0,Qt::AccessibleDescriptionRole, buff_sub.str().c_str());
		sub_folder->setToolTip(0, buff_sub.str().c_str());

		//subsubfolder of the section types
		QTreeWidgetItem *subsub_folder;
		ostringstream buff_sub_sub;
		//subsub_folder=new QTreeWidgetItem(sub_folder);
		//subsub_folder->setText(0, fpl_label::sec_highground.c_str());
		//buff_sub_sub<< buff_sub.str()<<fpl_label::sec_highground.c_str()<<"/";
		//subsub_folder->setData(0,Qt::AccessibleDescriptionRole, buff_sub_sub.str().c_str());
		//subsub_folder->setData(0,Qt::UserRole, "1");
		//subsub_folder->setToolTip(0, buff_sub_sub.str().c_str());
		//buff_sub_sub.str("");
		subsub_folder=new QTreeWidgetItem(sub_folder);
		subsub_folder->setText(0, fpl_label::sec_dike.c_str());
		buff_sub_sub<< buff_sub.str()<<fpl_label::sec_dike.c_str()<<"/";
		subsub_folder->setData(0,Qt::AccessibleDescriptionRole, buff_sub_sub.str().c_str());
		subsub_folder->setData(0,Qt::UserRole, "1");
		subsub_folder->setToolTip(0, buff_sub_sub.str().c_str());
		buff_sub_sub.str("");
		//set icon
		QIcon icon3;
		icon3.addFile(":dike_icon");
		subsub_folder->setIcon(0,icon3);

		subsub_folder=new QTreeWidgetItem(sub_folder);
		subsub_folder->setText(0, fpl_label::sec_dune.c_str());
		buff_sub_sub<< buff_sub.str()<<fpl_label::sec_dune.c_str()<<"/";
		subsub_folder->setData(0,Qt::AccessibleDescriptionRole, buff_sub_sub.str().c_str());
		subsub_folder->setData(0,Qt::UserRole, "1");
		subsub_folder->setToolTip(0, buff_sub_sub.str().c_str());
		buff_sub_sub.str("");
		//set icon
		QIcon icon2;
		icon2.addFile(":dune_icon");
		subsub_folder->setIcon(0,icon2);

		subsub_folder=new QTreeWidgetItem(sub_folder);
		subsub_folder->setText(0, fpl_label::sec_gate.c_str());
		buff_sub_sub<< buff_sub.str()<<fpl_label::sec_gate.c_str()<<"/";
		subsub_folder->setData(0,Qt::AccessibleDescriptionRole, buff_sub_sub.str().c_str());
		subsub_folder->setData(0,Qt::UserRole, "1");
		subsub_folder->setToolTip(0, buff_sub_sub.str().c_str());
		buff_sub_sub.str("");
		//set icon
		QIcon icon4;
		icon4.addFile(":gate_icon");
		subsub_folder->setIcon(0,icon4);

		subsub_folder=new QTreeWidgetItem(sub_folder);
		subsub_folder->setText(0, fpl_label::sec_wall.c_str());
		buff_sub_sub<< buff_sub.str()<<fpl_label::sec_wall.c_str()<<"/";
		subsub_folder->setData(0,Qt::AccessibleDescriptionRole, buff_sub_sub.str().c_str());
		subsub_folder->setData(0,Qt::UserRole, "1");
		subsub_folder->setToolTip(0, buff_sub_sub.str().c_str());
		buff_sub_sub.str("");
		subsub_folder=new QTreeWidgetItem(sub_folder);
		subsub_folder->setText(0, fpl_label::sec_mobile_wall.c_str());
		buff_sub_sub<< buff_sub.str()<<fpl_label::sec_mobile_wall.c_str()<<"/";
		subsub_folder->setData(0,Qt::AccessibleDescriptionRole, buff_sub_sub.str().c_str());
		subsub_folder->setData(0,Qt::UserRole, "1");
		subsub_folder->setToolTip(0, buff_sub_sub.str().c_str());
		buff_sub_sub.str("");

		buff_sub.str("");
		sub_folder=new QTreeWidgetItem(modules);
		sub_folder->setText(0, sys_data_tree_id::fpl_section_id.c_str());
		buff_sub<<buff_main.str() << sys_data_tree_id::fpl_section_id<<"/";
		sub_folder->setData(0,Qt::AccessibleDescriptionRole, buff_sub.str().c_str());
		sub_folder->setToolTip(0, buff_sub.str().c_str());

		buff_sub.str("");
		sub_folder=new QTreeWidgetItem(modules);
		sub_folder->setText(0, sys_data_tree_id::id_results.c_str());
		buff_sub<<buff_main.str() << sys_data_tree_id::id_results<<"/";
		sub_folder->setData(0,Qt::AccessibleDescriptionRole, buff_sub.str().c_str());
		sub_folder->setToolTip(0, buff_sub.str().c_str());

		buff_sub.str("");
		sub_folder=new QTreeWidgetItem(modules);
		sub_folder->setText(0, sys_data_tree_id::id_detailed_results.c_str());
		buff_sub<<buff_main.str() << sys_data_tree_id::id_detailed_results<<"/";
		sub_folder->setData(0,Qt::AccessibleDescriptionRole, buff_sub.str().c_str());
		sub_folder->setToolTip(0, buff_sub.str().c_str());
}
//Set-up predefined branch for module madm
void Sys_Data_Tree_Wid::set_up_predefined_madm(QTreeWidgetItem *root){
	//mainfolder
	ostringstream buff_main;
	QTreeWidgetItem *modules = new QTreeWidgetItem(root);
	modules->setText(0, sys_label::str_madm.c_str());
	buff_main << "/"<<sys_label::str_madm<<"/";
	modules->setData(0,Qt::AccessibleDescriptionRole, buff_main.str().c_str());
	modules->setToolTip(0, buff_main.str().c_str());
	QIcon icon1;
	icon1.addFile(":madm_icon");
	modules->setIcon(0,icon1);

		//subfolder
		ostringstream buff_sub;
		buff_sub.str("");
		QTreeWidgetItem *sub_folder=new QTreeWidgetItem(modules);
		sub_folder->setText(0, sys_data_tree_id::madm_weights.c_str());
		buff_sub<<buff_main.str() << sys_data_tree_id::madm_weights<<"/";
		sub_folder->setData(0,Qt::AccessibleDescriptionRole, buff_sub.str().c_str());
		sub_folder->setData(0,Qt::UserRole, "1");
		sub_folder->setToolTip(0, buff_sub.str().c_str());
		QSqlQueryModel model;
		int number=Madm_System::select_data_in_settable(&model, &this->database);
		sub_folder->setData(1, Qt::DisplayRole,number);

		buff_sub.str("");
		sub_folder=new QTreeWidgetItem(modules);
		sub_folder->setText(0, sys_data_tree_id::id_results.c_str());
		buff_sub<<buff_main.str() << sys_data_tree_id::id_results<<"/";
		sub_folder->setData(0,Qt::AccessibleDescriptionRole, buff_sub.str().c_str());
		sub_folder->setToolTip(0, buff_sub.str().c_str());

			//subsubfolder
			ostringstream buff_sub_sub;
			QTreeWidgetItem *sub_sub_folder=new QTreeWidgetItem(sub_folder);
			sub_sub_folder->setText(0, risk_label::risk_nobreak.c_str());
			buff_sub_sub <<buff_sub.str() <<risk_label::risk_nobreak<<"/";
			sub_sub_folder->setData(0,Qt::AccessibleDescriptionRole, buff_sub_sub.str().c_str());
			sub_sub_folder->setToolTip(0, buff_sub_sub.str().c_str());

			buff_sub_sub.str("");
			sub_sub_folder=new QTreeWidgetItem(sub_folder);
			sub_sub_folder->setText(0, risk_label::risk_catchment.c_str());
			buff_sub_sub <<buff_sub.str() <<risk_label::risk_catchment<<"/";
			sub_sub_folder->setData(0,Qt::AccessibleDescriptionRole, buff_sub_sub.str().c_str());
			sub_sub_folder->setToolTip(0, buff_sub_sub.str().c_str());
}
//Set-up predefined branch for module risk
void Sys_Data_Tree_Wid::set_up_predefined_risk(QTreeWidgetItem *root){
	//mainfolder
	ostringstream buff_main;
	QTreeWidgetItem *modules = new QTreeWidgetItem(root);
	modules->setText(0, sys_label::str_risk.c_str());
	buff_main << "/"<<sys_label::str_risk<<"/";
	modules->setData(0,Qt::AccessibleDescriptionRole, buff_main.str().c_str());
	modules->setToolTip(0, buff_main.str().c_str());
	QIcon icon1;
	icon1.addFile(":risk_icon");
	modules->setIcon(0,icon1);

		//subfolder
		ostringstream buff_sub;
		QTreeWidgetItem *sub_folder=new QTreeWidgetItem(modules);
		sub_folder->setText(0, sys_data_tree_id::id_config.c_str());
		buff_sub<<buff_main.str() << sys_data_tree_id::id_config<<"/";
		sub_folder->setData(0,Qt::AccessibleDescriptionRole, buff_sub.str().c_str());
		sub_folder->setData(0,Qt::UserRole, "1");
		sub_folder->setToolTip(0, buff_sub.str().c_str());
		sub_folder->setData(1, Qt::DisplayRole,1);

		//nobreak
		buff_sub.str("");
		sub_folder=new QTreeWidgetItem(modules);
		sub_folder->setText(0, risk_label::risk_nobreak.c_str());
		buff_sub<<buff_main.str() << risk_label::risk_nobreak<<"/";
		sub_folder->setData(0,Qt::AccessibleDescriptionRole, buff_sub.str().c_str());
		sub_folder->setToolTip(0, buff_sub.str().c_str());

			////subsubfolder of the risk approaches
			//QTreeWidgetItem *subsub_folder;
			//ostringstream buff_sub_sub;
			//subsub_folder=new QTreeWidgetItem(sub_folder);
			//subsub_folder->setText(0, sys_data_tree_id::id_detailed_results.c_str());
			//buff_sub_sub<< buff_sub.str()<<sys_data_tree_id::id_detailed_results.c_str()<<"/";
			//subsub_folder->setData(0,Qt::AccessibleDescriptionRole, buff_sub_sub.str().c_str());
			//subsub_folder->setToolTip(0, buff_sub_sub.str().c_str());
			//buff_sub_sub.str("");

		//scenario
		buff_sub.str("");
		sub_folder=new QTreeWidgetItem(modules);
		sub_folder->setText(0, risk_label::risk_scenario.c_str());
		buff_sub<<buff_main.str() << risk_label::risk_scenario<<"/";
		sub_folder->setData(0,Qt::AccessibleDescriptionRole, buff_sub.str().c_str());
		sub_folder->setToolTip(0, buff_sub.str().c_str());
			////subsubfolder of the risk approaches
			//subsub_folder=new QTreeWidgetItem(sub_folder);
			//subsub_folder->setText(0, sys_data_tree_id::id_detailed_results.c_str());
			//buff_sub_sub<< buff_sub.str()<<sys_data_tree_id::id_detailed_results.c_str()<<"/";
			//subsub_folder->setData(0,Qt::AccessibleDescriptionRole, buff_sub_sub.str().c_str());
			//subsub_folder->setToolTip(0, buff_sub_sub.str().c_str());
			//buff_sub_sub.str("");

		//catchment
		buff_sub.str("");
		sub_folder=new QTreeWidgetItem(modules);
		sub_folder->setText(0, risk_label::risk_catchment.c_str());
		buff_sub<<buff_main.str() << risk_label::risk_catchment<<"/";
		sub_folder->setData(0,Qt::AccessibleDescriptionRole, buff_sub.str().c_str());
		sub_folder->setToolTip(0, buff_sub.str().c_str());
			////subsubfolder of the risk approaches
			//subsub_folder=new QTreeWidgetItem(sub_folder);
			//subsub_folder->setText(0, sys_data_tree_id::id_detailed_results.c_str());
			//buff_sub_sub<< buff_sub.str()<<sys_data_tree_id::id_detailed_results.c_str()<<"/";
			//subsub_folder->setData(0,Qt::AccessibleDescriptionRole, buff_sub_sub.str().c_str());
			//subsub_folder->setToolTip(0, buff_sub_sub.str().c_str());
			//buff_sub_sub.str("");

		buff_sub.str("");
}
//Set-up predefined branch for module cost
void Sys_Data_Tree_Wid::set_up_predefined_cost(QTreeWidgetItem *root){
	//mainfolder
	ostringstream buff_main;
	QTreeWidgetItem *modules = new QTreeWidgetItem(root);
	modules->setText(0, sys_label::str_cost.c_str());
	buff_main << "/"<<sys_label::str_cost<<"/";
	modules->setData(0,Qt::AccessibleDescriptionRole, buff_main.str().c_str());
	modules->setToolTip(0, buff_main.str().c_str());
	QIcon icon1;
	icon1.addFile(":cost_icon");
	modules->setIcon(0,icon1);

		//subfolder
		ostringstream buff_sub;
		buff_sub.str("");
		QTreeWidgetItem *sub_folder=new QTreeWidgetItem(modules);
		sub_folder->setText(0, sys_data_tree_id::dam_ecn_id.c_str());
		buff_sub<<buff_main.str() << sys_data_tree_id::dam_ecn_id<<"/";
		sub_folder->setData(0,Qt::AccessibleDescriptionRole, buff_sub.str().c_str());
		sub_folder->setToolTip(0, buff_sub.str().c_str());
}
//Set-up predefined branch for module alt
void Sys_Data_Tree_Wid::set_up_predefined_alt(QTreeWidgetItem *root){
	//mainfolder
	ostringstream buff_main;
	QTreeWidgetItem *modules = new QTreeWidgetItem(root);
	modules->setText(0, sys_label::str_alt.c_str());
	buff_main << "/"<<sys_label::str_alt<<"/";
	modules->setData(0,Qt::AccessibleDescriptionRole, buff_main.str().c_str());
	modules->setToolTip(0, buff_main.str().c_str());
	QIcon icon1;
	icon1.addFile(":alt_icon");
	modules->setIcon(0,icon1);
}
//Set-up current data for the module hydraulic
void Sys_Data_Tree_Wid::set_up_current_data_hyd(void){
	//count and populate the database table model
	QSqlTableModel models(0, this->database);
	QSqlQueryModel profiles;
	int number=0;

	ostringstream buffer_fix;
	ostringstream buffer;
	string name;
	ostringstream id_txt;
	int id;
	QTreeWidgetItem *found=NULL;
	QTreeWidgetItem *child=NULL;

	//hyd/river
	//search for the parent
	buffer_fix<< "/"<<sys_label::str_hyd<<"/"<< sys_data_tree_id::hyd_river_id<<"/";
	found=this->start_module_tree_search(buffer_fix.str(), _sys_system_modules::HYD_SYS);
	if(found!=NULL){
		//select the number of models
		number=Hyd_Model_River::select_relevant_model_database(&models, *this->system_id, false);
		//create the children
		for(int i=0; i< number; i++){
			child=new QTreeWidgetItem(found);
			name=models.record(i).value((Hyd_Model_River::general_param_table->get_column_name(hyd_label::genmod_name)).c_str()).toString().toStdString();
			id=models.record(i).value((Hyd_Model_River::general_param_table->get_column_name(hyd_label::genmod_id)).c_str()).toInt();
			id_txt<<id;
			child->setText(0, name.c_str());
			buffer <<buffer_fix.str()<< name<<"/";
			child->setData(0,Qt::AccessibleDescriptionRole, buffer.str().c_str());
			child->setToolTip(0, buffer.str().c_str());
			child->setData(0,Qt::UserRole, id_txt.str().c_str());

			// set up the profiles
			id_txt.str("");
			ostringstream buffer_prof;
			QTreeWidgetItem *child_profile=NULL;

			int number_prof=0;
			//select number of profiles
			number_prof=_Hyd_River_Profile::select_relevant_profiles_in_database(&profiles,&this->database, *this->system_id, id, false);
			//create children
			for(int j=0; j< number_prof; j++){
				child_profile=new QTreeWidgetItem(child);
				id=profiles.record(j).value((_Hyd_River_Profile::profile_table->get_column_name(hyd_label::profdata_glob_id)).c_str()).toInt();
				name=profiles.record(j).value((_Hyd_River_Profile::profile_table->get_column_name(hyd_label::profdata_name)).c_str()).toString().toStdString();
				name.append("_");
				name.append(profiles.record(j).value((_Hyd_River_Profile::profile_table->get_column_name(hyd_label::profdata_ldist)).c_str()).toString().toStdString());
				id_txt<<id;
				buffer_prof<<buffer.str()<<"/"<<name<<"/";
				child_profile->setText(0, name.c_str());
				child_profile->setData(0,Qt::AccessibleDescriptionRole, buffer_prof.str().c_str());
				child_profile->setToolTip(0, buffer_prof.str().c_str());
				child_profile->setData(0,Qt::UserRole, id_txt.str().c_str());
				//set icon
				QIcon icon1;
				_hyd_profile_types type_buff=_Hyd_River_Profile::convert_string2profiletype(profiles.record(j).value((_Hyd_River_Profile::profile_table->get_column_name(hyd_label::profdata_proftyp)).c_str()).toString().toStdString());
				if(type_buff== _hyd_profile_types::RIVER_TYPE){
					icon1.addFile(":riverprof_icon");
				}
				else if(type_buff== _hyd_profile_types::WEIR_TYPE){
					icon1.addFile(":weir_icon");
				}
				else if(type_buff== _hyd_profile_types::BRIDGE_TYPE){
					icon1.addFile(":bridge_icon");
				}
				child_profile->setIcon(0,icon1);

				id_txt.str("");
				buffer_prof.str("");
			}
			buffer.str("");
		}
	}
	//reset all
	buffer.str("");
	id_txt.str("");
	buffer_fix.str("");
	found=NULL;

	//hyd/floodplain
	//search for the parent
	buffer_fix<< "/"<<sys_label::str_hyd<<"/"<< sys_data_tree_id::hyd_floodplain_id<<"/";
	found=this->start_module_tree_search(buffer_fix.str(), _sys_system_modules::HYD_SYS);
	if(found!=NULL){
		//select the number of models
		number=Hyd_Model_Floodplain::select_relevant_model_database(&models,false);
		//create the children
		for(int i=0; i< number; i++){
			child=new QTreeWidgetItem(found);
			name=models.record(i).value((Hyd_Model_Floodplain::general_param_table->get_column_name(hyd_label::genmod_name)).c_str()).toString().toStdString();
			id=models.record(i).value((Hyd_Model_Floodplain::general_param_table->get_column_name(hyd_label::genmod_id)).c_str()).toInt();
			id_txt<<id;
			child->setText(0, name.c_str());
			buffer <<buffer_fix.str()<< name<<"/";
			child->setData(0,Qt::AccessibleDescriptionRole, buffer.str().c_str());
			child->setToolTip(0, buffer.str().c_str());
			child->setData(0,Qt::UserRole, id_txt.str().c_str());

			buffer.str("");
			id_txt.str("");
		}
	}
	//found->sortChildren(0,Qt::AscendingOrder);
	//reset all
	buffer.str("");
	id_txt.str("");
	buffer_fix.str("");
	found=NULL;
}
//Set-up current data for the module damage
void Sys_Data_Tree_Wid::set_up_current_data_dam(void){
	//count and populate the database table model
	QSqlQueryModel raster;
	QSqlTableModel result(0,this->database);

	int number=0;

	ostringstream buffer_fix;
	ostringstream buffer;
	string name;
	ostringstream id_txt;
	int id;
	QTreeWidgetItem *found=NULL;
	QTreeWidgetItem *child=NULL;

	//dam/ecn/raster
	//search for the parent
	buffer_fix<< "/"<<sys_label::str_dam<<"/"<< sys_data_tree_id::dam_ecn_id<<"/"<<sys_data_tree_id::dam_raster_id<<"/";
	found=this->start_module_tree_search(buffer_fix.str(), _sys_system_modules::DAM_SYS);
	if(found!=NULL){
		//select the number of raster
		number=Dam_Ecn_Raster::select_relevant_raster_database(&raster,&this->database, false);
		//create the children
		for(int i=0; i< number; i++){
			child=new QTreeWidgetItem(found);
			id=raster.record(i).value((Dam_Ecn_Raster::raster_table->get_column_name(dam_label::raster_id)).c_str()).toInt();
			name=raster.record(i).value((Dam_Ecn_Raster::raster_table->get_column_name(dam_label::raster_name_db)).c_str()).toString().toStdString();
			id_txt<<id<<" " <<name;
			child->setText(0, id_txt.str().c_str());
			id_txt.str("");
			id_txt<<id;
			buffer <<buffer_fix.str()<< name<<"/";
			child->setData(0,Qt::AccessibleDescriptionRole, buffer.str().c_str());
			child->setToolTip(0, buffer.str().c_str());
			child->setData(0,Qt::UserRole, id_txt.str().c_str());
			buffer.str("");
			id_txt.str("");
		}
	}
	//found->sortChildren(0,Qt::AscendingOrder);
	//reset all
	buffer_fix.str("");
	found=NULL;

	//dam/ecn/function
	//search for the parent
	buffer_fix<< "/"<<sys_label::str_dam<<"/"<< sys_data_tree_id::dam_ecn_id<<"/"<<sys_data_tree_id::dam_land_use_cat<<"/";
	found=this->start_module_tree_search(buffer_fix.str(), _sys_system_modules::DAM_SYS);
	if(found!=NULL){
		//select the number of raster
		number=Dam_Ecn_Damage_Function::select_relevant_functions_database(&result, false);
		//create the children
		int id_cat=0;
		for(int i=0; i< number; i++){
			child=new QTreeWidgetItem(found);
			id=result.record(i).value((Dam_Ecn_Damage_Function::function_table->get_column_name(dam_label::function_id)).c_str()).toInt();
			name=result.record(i).value((Dam_Ecn_Damage_Function::function_table->get_column_name(dam_label::landuse_name)).c_str()).toString().toStdString();
			id_cat=result.record(i).value((Dam_Ecn_Damage_Function::function_table->get_column_name(dam_label::landuse_id)).c_str()).toInt();
			id_txt<<id_cat<<" " <<name;
			child->setText(0, id_txt.str().c_str());
			id_txt.str("");
			id_txt<<id;
			buffer <<buffer_fix.str()<< name<<"/";
			child->setData(0,Qt::AccessibleDescriptionRole, buffer.str().c_str());
			child->setToolTip(0, buffer.str().c_str());
			child->setData(0,Qt::UserRole, id_txt.str().c_str());
			buffer.str("");
			id_txt.str("");
		}
	}
	//found->sortChildren(0,Qt::AscendingOrder);
	//reset all
	buffer_fix.str("");
	found=NULL;

	//dam/eco/btype/raster
	//search for the parent
	buffer_fix<< "/"<<sys_label::str_dam<<"/"<< sys_data_tree_id::dam_eco_id<<"/"<<sys_data_tree_id::dam_eco_btype_id<<"/"<<sys_data_tree_id::dam_raster_id<<"/";
	found=this->start_module_tree_search(buffer_fix.str(), _sys_system_modules::DAM_SYS);
	if(found!=NULL){
		//select the number of raster
		number=Dam_Eco_Btype_Raster::select_relevant_raster_database(&raster,&this->database, false);
		//create the children
		for(int i=0; i< number; i++){
			child=new QTreeWidgetItem(found);
			id=raster.record(i).value((Dam_Eco_Btype_Raster::raster_table->get_column_name(dam_label::raster_id)).c_str()).toInt();
			name=raster.record(i).value((Dam_Eco_Btype_Raster::raster_table->get_column_name(dam_label::raster_name_db)).c_str()).toString().toStdString();
			id_txt<<id<<" " <<name;
			child->setText(0, id_txt.str().c_str());
			id_txt.str("");
			id_txt<<id;
			buffer <<buffer_fix.str()<< name<<"/";
			child->setData(0,Qt::AccessibleDescriptionRole, buffer.str().c_str());
			child->setToolTip(0, buffer.str().c_str());
			child->setData(0,Qt::UserRole, id_txt.str().c_str());
			buffer.str("");
			id_txt.str("");
		}
	}
	//found->sortChildren(0,Qt::AscendingOrder);
	//reset all
	buffer_fix.str("");
	found=NULL;

	//dam/eco/btype/biotopes
	//search for the parent
	buffer_fix<< "/"<<sys_label::str_dam<<"/"<< sys_data_tree_id::dam_eco_id<<"/"<<sys_data_tree_id::dam_eco_btype_id<<"/"<<sys_data_tree_id::dam_biotope_type_id<<"/";
	found=this->start_module_tree_search(buffer_fix.str(), _sys_system_modules::DAM_SYS);
	if(found!=NULL){
		//select the number of raster
		number=Dam_Eco_Btype::select_relevant_biotope_types_database(&result, false);
		//create the children
		for(int i=0; i< number; i++){
			child=new QTreeWidgetItem(found);
			id=result.record(i).value((Dam_Eco_Btype::table->get_column_name(dam_label::biotope_type_id)).c_str()).toInt();
			name=result.record(i).value((Dam_Eco_Btype::table->get_column_name(dam_label::biotope_type_name)).c_str()).toString().toStdString();
			id_txt<<id<<" " <<name;
			child->setText(0, id_txt.str().c_str());
			id_txt.str("");
			id_txt<<id;
			buffer <<buffer_fix.str()<< name<<"/";
			child->setData(0,Qt::AccessibleDescriptionRole, buffer.str().c_str());
			child->setToolTip(0, buffer.str().c_str());
			child->setData(0,Qt::UserRole, id_txt.str().c_str());
			buffer.str("");
			id_txt.str("");
		}
	}
	//found->sortChildren(0,Qt::AscendingOrder);
	//reset all
	buffer_fix.str("");
	found=NULL;

	//dam/eco/soil/raster
	//search for the parent
	buffer_fix<< "/"<<sys_label::str_dam<<"/"<< sys_data_tree_id::dam_eco_id<<"/"<<sys_data_tree_id::dam_eco_soil_id<<"/"<<sys_data_tree_id::dam_raster_id<<"/";
	found=this->start_module_tree_search(buffer_fix.str(), _sys_system_modules::DAM_SYS);
	if(found!=NULL){
		//select the number of raster
		number=Dam_Eco_Soil_Raster::select_relevant_raster_database(&raster,&this->database, false);
		//create the children
		for(int i=0; i< number; i++){
			child=new QTreeWidgetItem(found);
			id=raster.record(i).value((Dam_Eco_Soil_Raster::raster_table->get_column_name(dam_label::raster_id)).c_str()).toInt();
			name=raster.record(i).value((Dam_Eco_Soil_Raster::raster_table->get_column_name(dam_label::raster_name_db)).c_str()).toString().toStdString();
			id_txt<<id<<" " <<name;
			child->setText(0, id_txt.str().c_str());
			id_txt.str("");
			id_txt<<id;
			buffer <<buffer_fix.str()<< name<<"/";
			child->setData(0,Qt::AccessibleDescriptionRole, buffer.str().c_str());
			child->setToolTip(0, buffer.str().c_str());
			child->setData(0,Qt::UserRole, id_txt.str().c_str());
			buffer.str("");
			id_txt.str("");
		}
	}
	//found->sortChildren(0,Qt::AscendingOrder);
	//reset all
	buffer_fix.str("");
	found=NULL;

	//dam/eco/soil/cost function
	//search for the parent
	buffer_fix<< "/"<<sys_label::str_dam<<"/"<< sys_data_tree_id::dam_eco_id<<"/"<<sys_data_tree_id::dam_eco_soil_id<<"/"<<sys_data_tree_id::dam_soil_cost_id<<"/";
	found=this->start_module_tree_search(buffer_fix.str(), _sys_system_modules::DAM_SYS);
	if(found!=NULL){
		//select the number of raster
		number=Dam_Eco_Soil_Cost_Function::select_relevant_functions_database(&result, false);
		//create the children
		for(int i=0; i< number; i++){
			child=new QTreeWidgetItem(found);
			id=result.record(i).value((Dam_Eco_Soil_Cost_Function::function_table->get_column_name(dam_label::soil_type_id)).c_str()).toInt();
			name=result.record(i).value((Dam_Eco_Soil_Cost_Function::function_table->get_column_name(dam_label::soil_type_name)).c_str()).toString().toStdString();
			id_txt<<id<<" " <<name;
			child->setText(0, id_txt.str().c_str());
			id_txt.str("");
			id_txt<<id;
			buffer <<buffer_fix.str()<< name<<"/";
			child->setData(0,Qt::AccessibleDescriptionRole, buffer.str().c_str());
			child->setToolTip(0, buffer.str().c_str());
			child->setData(0,Qt::UserRole, id_txt.str().c_str());
			buffer.str("");
			id_txt.str("");
		}
	}
	//found->sortChildren(0,Qt::AscendingOrder);
	//reset all
	buffer_fix.str("");
	found=NULL;

	//dam/eco/soil/erosion function
	//search for the parent
	buffer_fix<< "/"<<sys_label::str_dam<<"/"<< sys_data_tree_id::dam_eco_id<<"/"<<sys_data_tree_id::dam_eco_soil_id<<"/"<<sys_data_tree_id::dam_soil_ero_id<<"/";
	found=this->start_module_tree_search(buffer_fix.str(), _sys_system_modules::DAM_SYS);
	if(found!=NULL){
		//select the number of raster
		number=Dam_Eco_Soil_Erosion_Function::select_relevant_functions_database(&result, false);
		//create the children
		for(int i=0; i< number; i++){
			child=new QTreeWidgetItem(found);
			id=result.record(i).value((Dam_Eco_Soil_Erosion_Function::function_table->get_column_name(dam_label::ero_type_id)).c_str()).toInt();
			name=result.record(i).value((Dam_Eco_Soil_Erosion_Function::function_table->get_column_name(dam_label::ero_type_name)).c_str()).toString().toStdString();
			id_txt<<id<<" " <<name;
			child->setText(0, id_txt.str().c_str());
			id_txt.str("");
			id_txt<<id;
			buffer <<buffer_fix.str()<< name<<"/";
			child->setData(0,Qt::AccessibleDescriptionRole, buffer.str().c_str());
			child->setToolTip(0, buffer.str().c_str());
			child->setData(0,Qt::UserRole, id_txt.str().c_str());
			buffer.str("");
			id_txt.str("");
		}
	}
	//found->sortChildren(0,Qt::AscendingOrder);
	//reset all
	buffer_fix.str("");
	found=NULL;

	//dam/pop/raster
	//search for the parent
	buffer_fix<< "/"<<sys_label::str_dam<<"/"<< sys_data_tree_id::dam_pop_id<<"/"<<sys_data_tree_id::dam_raster_id<<"/";
	found=this->start_module_tree_search(buffer_fix.str(), _sys_system_modules::DAM_SYS);
	if(found!=NULL){
		//select the number of raster
		number=Dam_People_Raster::select_relevant_raster_database(&raster,&this->database, false);
		//create the children
		for(int i=0; i< number; i++){
			child=new QTreeWidgetItem(found);
			id=raster.record(i).value((Dam_People_Raster::raster_table->get_column_name(dam_label::raster_id)).c_str()).toInt();
			name=raster.record(i).value((Dam_People_Raster::raster_table->get_column_name(dam_label::raster_name_db)).c_str()).toString().toStdString();
			id_txt<<id<<" " <<name;
			child->setText(0, id_txt.str().c_str());
			id_txt.str("");
			id_txt<<id;
			buffer <<buffer_fix.str()<< name<<"/";
			child->setData(0,Qt::AccessibleDescriptionRole, buffer.str().c_str());
			child->setToolTip(0, buffer.str().c_str());
			child->setData(0,Qt::UserRole, id_txt.str().c_str());
			buffer.str("");
			id_txt.str("");
		}
	}
	//found->sortChildren(0,Qt::AscendingOrder);
	//reset all
	buffer_fix.str("");
	found=NULL;

	//dam/pop/function
	//search for the parent
	buffer_fix<< "/"<<sys_label::str_dam<<"/"<< sys_data_tree_id::dam_pop_id<<"/"<<sys_data_tree_id::dam_vul_cat<<"/";
	found=this->start_module_tree_search(buffer_fix.str(), _sys_system_modules::DAM_SYS);
	if(found!=NULL){
		//select the number of raster
		number=Dam_People_Damage_Function::select_relevant_functions_database(&result, false);
		//create the children
		for(int i=0; i< number; i++){
			child=new QTreeWidgetItem(found);
			id=result.record(i).value((Dam_People_Damage_Function::function_table->get_column_name(dam_label::pop_type_id)).c_str()).toInt();
			name=result.record(i).value((Dam_People_Damage_Function::function_table->get_column_name(dam_label::pop_type_name)).c_str()).toString().toStdString();
			id_txt<<id<<" " <<name;
			child->setText(0, id_txt.str().c_str());
			id_txt.str("");
			id_txt<<id;
			buffer <<buffer_fix.str()<< name<<"/";
			child->setData(0,Qt::AccessibleDescriptionRole, buffer.str().c_str());
			child->setToolTip(0, buffer.str().c_str());
			child->setData(0,Qt::UserRole, id_txt.str().c_str());
			buffer.str("");
			id_txt.str("");
		}
	}
	//found->sortChildren(0,Qt::AscendingOrder);
	//reset all
	buffer_fix.str("");
	found=NULL;

	//dam/pys/raster
	//search for the parent
	buffer_fix<< "/"<<sys_label::str_dam<<"/"<< sys_data_tree_id::dam_pys_id<<"/"<<sys_data_tree_id::dam_raster_id<<"/";
	found=this->start_module_tree_search(buffer_fix.str(), _sys_system_modules::DAM_SYS);
	if(found!=NULL){
		//select the number of raster
		number=Dam_Pys_Raster::select_relevant_raster_database(&raster,&this->database, false);
		//create the children
		for(int i=0; i< number; i++){
			child=new QTreeWidgetItem(found);
			id=raster.record(i).value((Dam_Pys_Raster::raster_table->get_column_name(dam_label::raster_id)).c_str()).toInt();
			name=raster.record(i).value((Dam_Pys_Raster::raster_table->get_column_name(dam_label::raster_name_db)).c_str()).toString().toStdString();
			id_txt<<id<<" " <<name;
			child->setText(0, id_txt.str().c_str());
			id_txt.str("");
			id_txt<<id;
			buffer <<buffer_fix.str()<< name<<"/";
			child->setData(0,Qt::AccessibleDescriptionRole, buffer.str().c_str());
			child->setToolTip(0, buffer.str().c_str());
			child->setData(0,Qt::UserRole, id_txt.str().c_str());
			buffer.str("");
			id_txt.str("");
		}
	}
	//found->sortChildren(0,Qt::AscendingOrder);
	//reset all
	buffer_fix.str("");
	found=NULL;

	//dam/pys/categroy
	//search for the parent
	buffer_fix<< "/"<<sys_label::str_dam<<"/"<< sys_data_tree_id::dam_pys_id<<"/"<<sys_data_tree_id::dam_category_id<<"/";
	found=this->start_module_tree_search(buffer_fix.str(), _sys_system_modules::DAM_SYS);
	if(found!=NULL){
		//select the number of raster
		number=Dam_Pys_Category::select_relevant_category_database(&result, false);
		//create the children
		for(int i=0; i< number; i++){
			child=new QTreeWidgetItem(found);
			id=result.record(i).value((Dam_Pys_Category::table->get_column_name(dam_label::category_id)).c_str()).toInt();
			name=result.record(i).value((Dam_Pys_Category::table->get_column_name(dam_label::category_name)).c_str()).toString().toStdString();
			id_txt<<id<<" " <<name;
			child->setText(0, id_txt.str().c_str());
			id_txt.str("");
			id_txt<<id;
			buffer <<buffer_fix.str()<< name<<"/";
			child->setData(0,Qt::AccessibleDescriptionRole, buffer.str().c_str());
			child->setToolTip(0, buffer.str().c_str());
			child->setData(0,Qt::UserRole, id_txt.str().c_str());
			buffer.str("");
			id_txt.str("");
		}
	}
	//found->sortChildren(0,Qt::AscendingOrder);
	//reset all
	buffer_fix.str("");
	found=NULL;

	Hyd_Boundary_Szenario_Management hyd_sc_manager;
	hyd_sc_manager.set_systemid(*this->system_id);
	hyd_sc_manager.set_szenario_per_db(&this->database);

	//result (nobreak)
	try{
		//select the number of section
		number=Dam_Damage_System::select_data_in_erg_table(&result, &this->database, *this->system_id,"CA");
	}
	catch(Error msg){
		throw msg;
	}

	if(number>0){
		buffer_fix<< "/"<<sys_label::str_dam<<"/"<<sys_data_tree_id::id_results<<"/"<< risk_label::risk_nobreak<<"/";
		found=this->start_module_tree_search(buffer_fix.str(), _sys_system_modules::DAM_SYS);

		if(found!=NULL){
			//create the children
			child=new QTreeWidgetItem(found);
			name=sys_data_tree_id::id_summary_results;
			child->setText(0, name.c_str());
			buffer <<buffer_fix.str()<< name<<"/";
			child->setData(0,Qt::AccessibleDescriptionRole, buffer.str().c_str());
			child->setToolTip(0, buffer.str().c_str());
			child->setData(0,Qt::UserRole, "1");
			buffer.str("");

			int id_sc=0;
			for(int i=0; i< number; i++){
				child=new QTreeWidgetItem(found);
				id_sc=result.record(i).value((Dam_Damage_System::erg_table->get_column_name(hyd_label::sz_bound_id)).c_str()).toInt();
				id=result.record(i).value((Dam_Damage_System::erg_table->get_column_name(dam_label::glob_id)).c_str()).toInt();
				id_txt<<id;
				Hyd_Boundary_Szenario *null_test;
				null_test=hyd_sc_manager.get_ptr_sz_id_given(id_sc);
				if(null_test==NULL){
					continue;
				}

				name=null_test->get_name();
				child->setText(0, name.c_str());
				buffer <<buffer_fix.str()<< name<<"/";
				child->setData(0,Qt::AccessibleDescriptionRole, buffer.str().c_str());
				child->setToolTip(0, buffer.str().c_str());
				child->setData(0,Qt::UserRole, id_txt.str().c_str());
				buffer.str("");
				id_txt.str("");
			}
		}
		//reset all
		buffer.str("");
		id_txt.str("");
		buffer_fix.str("");
		found=NULL;
	}
	//result (scenario)
	try{
		//select the number of section
		number=Dam_Damage_System::select_data_in_erg_table(&result, &this->database, *this->system_id,"SC%");
	}
	catch(Error msg){
		throw msg;
	}

	if(number>0){
		buffer_fix<< "/"<<sys_label::str_dam<<"/"<<sys_data_tree_id::id_results<<"/"<< risk_label::risk_scenario<<"/";
		found=this->start_module_tree_search(buffer_fix.str(), _sys_system_modules::DAM_SYS);

		if(found!=NULL){
			//create the children
			child=new QTreeWidgetItem(found);
			name=sys_data_tree_id::id_summary_results;
			child->setText(0, name.c_str());
			buffer <<buffer_fix.str()<< name<<"/";
			child->setData(0,Qt::AccessibleDescriptionRole, buffer.str().c_str());
			child->setToolTip(0, buffer.str().c_str());
			child->setData(0,Qt::UserRole, "1");
			buffer.str("");

			ostringstream buffer_name;
			int id_sc=0;
			for(int i=0; i< number; i++){
				buffer_name.str("");

				child=new QTreeWidgetItem(found);
				id_sc=result.record(i).value((Dam_Damage_System::erg_table->get_column_name(hyd_label::sz_bound_id)).c_str()).toInt();
				id=result.record(i).value((Dam_Damage_System::erg_table->get_column_name(dam_label::glob_id)).c_str()).toInt();
				id_txt<<id;
				Hyd_Boundary_Szenario *null_test;
				null_test=hyd_sc_manager.get_ptr_sz_id_given(id_sc);
				if(null_test==NULL){
					continue;
				}
				buffer_name<<null_test->get_name()<<"-";
				buffer_name<< result.record(i).value((Dam_Damage_System::erg_table->get_column_name(risk_label::sz_break_id)).c_str()).toString().toStdString();
				name=buffer_name.str();
				child->setText(0, name.c_str());
				buffer <<buffer_fix.str()<< name<<"/";
				child->setData(0,Qt::AccessibleDescriptionRole, buffer.str().c_str());
				child->setToolTip(0, buffer.str().c_str());
				child->setData(0,Qt::UserRole, id_txt.str().c_str());
				buffer.str("");
				id_txt.str("");
			}
		}
		//reset all
		buffer.str("");
		id_txt.str("");
		buffer_fix.str("");
		found=NULL;
	}

	//result (catchment)

	if(this->current_project!=NULL && this->current_project->get_stored_risk_state().catchment_risk_applied==true){
		try{
			//select the number of section
			number=Dam_Damage_System::select_data_in_erg_table(&result, &this->database, *this->system_id,"CA%");
		}
		catch(Error msg){
			throw msg;
		}

		if(number>0){
			buffer_fix<< "/"<<sys_label::str_dam<<"/"<<sys_data_tree_id::id_results<<"/"<< risk_label::risk_catchment<<"/";
			found=this->start_module_tree_search(buffer_fix.str(), _sys_system_modules::DAM_SYS);

			if(found!=NULL){
				//create the children
				child=new QTreeWidgetItem(found);
				name=sys_data_tree_id::id_summary_results;
				child->setText(0, name.c_str());
				buffer <<buffer_fix.str()<< name<<"/";
				child->setData(0,Qt::AccessibleDescriptionRole, buffer.str().c_str());
				child->setToolTip(0, buffer.str().c_str());
				child->setData(0,Qt::UserRole, "1");
				buffer.str("");

				ostringstream buffer_name;
				int id_sc=0;
				for(int i=0; i< number; i++){
					buffer_name.str("");

					child=new QTreeWidgetItem(found);
					id_sc=result.record(i).value((Dam_Damage_System::erg_table->get_column_name(hyd_label::sz_bound_id)).c_str()).toInt();
					id=result.record(i).value((Dam_Damage_System::erg_table->get_column_name(dam_label::glob_id)).c_str()).toInt();
					id_txt<<id;
					Hyd_Boundary_Szenario *null_test;
					null_test=hyd_sc_manager.get_ptr_sz_id_given(id_sc);
					if(null_test==NULL){
						continue;
					}

					buffer_name<<null_test->get_name()<<"-";
					buffer_name<< result.record(i).value((Dam_Damage_System::erg_table->get_column_name(risk_label::sz_break_id)).c_str()).toString().toStdString();
					name=buffer_name.str();
					child->setText(0, name.c_str());
					buffer <<buffer_fix.str()<< name<<"/";
					child->setData(0,Qt::AccessibleDescriptionRole, buffer.str().c_str());
					child->setToolTip(0, buffer.str().c_str());
					child->setData(0,Qt::UserRole, id_txt.str().c_str());
					buffer.str("");
					id_txt.str("");
				}
			}
			//reset all
			buffer.str("");
			id_txt.str("");
			buffer_fix.str("");
			found=NULL;
		}
	}
}
//Set-up current data for the module fpl
void Sys_Data_Tree_Wid::set_up_current_data_fpl(void){
	//count and populate the database table model
	//QSqlTableModel results(0, this->database);
	QSqlQueryModel section;
	int number=0;

	ostringstream buffer_fix;
	ostringstream buffer;
	string name;
	ostringstream id_txt;
	int id;
	QTreeWidgetItem *found=NULL;
	QTreeWidgetItem *child=NULL;

	try{
		//select the number of section
		number=Fpl_Section::select_relevant_section_database(&section, &this->database, *this->system_id, false);
	}
	catch(Error msg){
		throw msg;
	}

	//fpl/section
	//search for the parent
	buffer_fix<< "/"<<sys_label::str_fpl<<"/"<< sys_data_tree_id::fpl_section_id<<"/";
	found=this->start_module_tree_search(buffer_fix.str(), _sys_system_modules::FPL_SYS);
	if(found!=NULL){
		//create the children
		for(int i=0; i< number; i++){
			child=new QTreeWidgetItem(found);
			name=section.record(i).value((Fpl_Section::table->get_column_name(fpl_label::section_name)).c_str()).toString().toStdString();
			id=section.record(i).value((Fpl_Section::table->get_column_name(fpl_label::section_id)).c_str()).toInt();

			id_txt<<id<<" "<<name;
			child->setText(0, id_txt.str().c_str());
			id_txt.str("");
			id_txt<<id;
			buffer <<buffer_fix.str()<< name<<"/";
			child->setData(0,Qt::AccessibleDescriptionRole, buffer.str().c_str());
			child->setToolTip(0, buffer.str().c_str());
			child->setData(0,Qt::UserRole, id_txt.str().c_str());
			buffer.str("");
			id_txt.str("");

			//set icon
			QIcon icon1;
			_fpl_section_types type_buff=_Fpl_Section_Type::convert_txt2section_type(section.record(i).value((Fpl_Section::table->get_column_name(fpl_label::section_type)).c_str()).toString().toStdString());
			if(type_buff== _fpl_section_types::dike_sec){
				icon1.addFile(":dike_icon");
			}
			else if(type_buff== _fpl_section_types::wall_sec){
				icon1.addFile(":wall_icon");
			}
			else if(type_buff== _fpl_section_types::gate_sec){
				icon1.addFile(":gate_icon");
			}
			else if(type_buff== _fpl_section_types::dune_sec){
				icon1.addFile(":dune_icon");
			}
			else if(type_buff== _fpl_section_types::by_hand_sec){
				icon1.addFile(":user_icon");
			}
			child->setIcon(0,icon1);
		}
	}
	//reset all
	buffer.str("");
	id_txt.str("");
	buffer_fix.str("");
	found=NULL;
	//fpl/results
	//search for the parent
	buffer_fix<< "/"<<sys_label::str_fpl<<"/"<< sys_data_tree_id::id_results<<"/";
	found=this->start_module_tree_search(buffer_fix.str(), _sys_system_modules::FPL_SYS);
	if(found!=NULL){
		//create the children
		for(int i=0; i< number; i++){
			child=new QTreeWidgetItem(found);
			name=section.record(i).value((Fpl_Section::table->get_column_name(fpl_label::section_name)).c_str()).toString().toStdString();
			id=section.record(i).value((Fpl_Section::table->get_column_name(fpl_label::section_id)).c_str()).toInt();
			id_txt<<id<<" "<<name;
			child->setText(0, id_txt.str().c_str());
			id_txt.str("");
			id_txt<<id;
			buffer <<buffer_fix.str()<< name<<"/";
			child->setData(0,Qt::AccessibleDescriptionRole, buffer.str().c_str());
			child->setToolTip(0, buffer.str().c_str());
			child->setData(0,Qt::UserRole, id_txt.str().c_str());
			buffer.str("");
			id_txt.str("");

			//set icon
			QIcon icon1;
			_fpl_section_types type_buff=_Fpl_Section_Type::convert_txt2section_type(section.record(i).value((Fpl_Section::table->get_column_name(fpl_label::section_type)).c_str()).toString().toStdString());
			if(type_buff== _fpl_section_types::dike_sec){
				icon1.addFile(":dike_icon");
			}
			else if(type_buff== _fpl_section_types::wall_sec){
				icon1.addFile(":wall_icon");
			}
			else if(type_buff== _fpl_section_types::gate_sec){
				icon1.addFile(":gate_icon");
			}
			else if(type_buff== _fpl_section_types::dune_sec){
				icon1.addFile(":dune_icon");
			}
			else if(type_buff== _fpl_section_types::by_hand_sec){
				icon1.addFile(":user_icon");
			}
			child->setIcon(0,icon1);
		}
	}
	//reset all
	buffer.str("");
	id_txt.str("");
	buffer_fix.str("");
	found=NULL;

	//fpl/results_detailed
	//search for the parent
	buffer_fix<< "/"<<sys_label::str_fpl<<"/"<< sys_data_tree_id::id_detailed_results<<"/";
	found=this->start_module_tree_search(buffer_fix.str(), _sys_system_modules::FPL_SYS);
	if(found!=NULL){
		//create the children
		for(int i=0; i< number; i++){
			child=new QTreeWidgetItem(found);
			name=section.record(i).value((Fpl_Section::table->get_column_name(fpl_label::section_name)).c_str()).toString().toStdString();
			id=section.record(i).value((Fpl_Section::table->get_column_name(fpl_label::section_id)).c_str()).toInt();
			id_txt<<id<<" "<<name;
			child->setText(0, id_txt.str().c_str());
			id_txt.str("");
			id_txt<<id;
			buffer <<buffer_fix.str()<< name<<"/";
			child->setData(0,Qt::AccessibleDescriptionRole, buffer.str().c_str());
			child->setToolTip(0, buffer.str().c_str());
			child->setData(0,Qt::UserRole, id_txt.str().c_str());
			buffer.str("");
			id_txt.str("");

			//set icon
			QIcon icon1;
			_fpl_section_types type_buff=_Fpl_Section_Type::convert_txt2section_type(section.record(i).value((Fpl_Section::table->get_column_name(fpl_label::section_type)).c_str()).toString().toStdString());
			if(type_buff== _fpl_section_types::dike_sec){
				icon1.addFile(":dike_icon");
			}
			else if(type_buff== _fpl_section_types::wall_sec){
				icon1.addFile(":wall_icon");
			}
			else if(type_buff== _fpl_section_types::gate_sec){
				icon1.addFile(":gate_icon");
			}
			else if(type_buff== _fpl_section_types::dune_sec){
				icon1.addFile(":dune_icon");
			}
			else if(type_buff== _fpl_section_types::by_hand_sec){
				icon1.addFile(":user_icon");
			}
			child->setIcon(0,icon1);
		}
	}
	//reset all
	buffer.str("");
	id_txt.str("");
	buffer_fix.str("");
	found=NULL;
}
//Set-up current data for the module madm
void Sys_Data_Tree_Wid::set_up_current_data_madm(void){
	//count and populate the database table model
	//QSqlTableModel results(0, this->database);
	QSqlQueryModel model;
	int number=0;

	ostringstream buffer_fix;
	ostringstream buffer;
	string name;
	ostringstream id_txt;
	int id;
	QTreeWidgetItem *found=NULL;
	QTreeWidgetItem *child=NULL;

	//matrix nobreak
	try{
		///Select the data in the database table for weighting sets
		number=Madm_Decision_Matrix::select_data_in_table_matrix(&model, &this->database, _risk_type::nobreak_risk);
	}
	catch(Error msg){
		throw msg;
	}
	if(number>0){
		buffer_fix<< "/"<<sys_label::str_madm<<"/"<< sys_data_tree_id::id_results<<"/"<<risk_label::risk_nobreak<<"/";
		found=this->start_module_tree_search(buffer_fix.str(), _sys_system_modules::MADM_SYS);
		if(found!=NULL){
			//create the children
			child=new QTreeWidgetItem(found);
			name=sys_data_tree_id::madm_matrix;
			child->setText(0, name.c_str());
			buffer <<buffer_fix.str()<< name<<"/";
			child->setData(0,Qt::AccessibleDescriptionRole, buffer.str().c_str());
			child->setToolTip(0, buffer.str().c_str());
			child->setData(0,Qt::UserRole, "1");
			buffer.str("");
			id_txt.str("");
		}
		buffer_fix.str("");
	}

	//matrix catchment
	try{
		///Select the data in the database table for weighting sets
		number=Madm_Decision_Matrix::select_data_in_table_matrix(&model, &this->database, _risk_type::catchment_risk);
	}
	catch(Error msg){
		throw msg;
	}
	if(number>0){
		buffer_fix<< "/"<<sys_label::str_madm<<"/"<< sys_data_tree_id::id_results<<"/"<<risk_label::risk_catchment<<"/";
		found=this->start_module_tree_search(buffer_fix.str(), _sys_system_modules::MADM_SYS);
		if(found!=NULL){
			//create the children
			child=new QTreeWidgetItem(found);
			name=sys_data_tree_id::madm_matrix;
			child->setText(0, name.c_str());
			buffer <<buffer_fix.str()<< name<<"/";
			child->setData(0,Qt::AccessibleDescriptionRole, buffer.str().c_str());
			child->setToolTip(0, buffer.str().c_str());
			child->setData(0,Qt::UserRole, "1");
			buffer.str("");
			id_txt.str("");
		}
		buffer_fix.str("");
	}

	//rankings
	try{
		///Select the data in the database table for weighting sets
		number=Madm_System::select_data_in_settable(&model, &this->database);
	}
	catch(Error msg){
		throw msg;
	}

	//madm/results
	//nobreak area risk approach
	//search for the parent
	buffer_fix<< "/"<<sys_label::str_madm<<"/"<< sys_data_tree_id::id_results<<"/"<<risk_label::risk_nobreak<<"/";
	found=this->start_module_tree_search(buffer_fix.str(), _sys_system_modules::MADM_SYS);
	if(found!=NULL&& _Madm_Solver::check_data_in_ranking_table(&this->database, this->system_id->area_state, _risk_type::nobreak_risk)==true){
		//create the children
		for(int i=0; i< number; i++){
			child=new QTreeWidgetItem(found);
			name=model.record(i).value((Madm_System::table_sets->get_column_name(madm_label::set_name)).c_str()).toString().toStdString();
			id=model.record(i).value((Madm_System::table_sets->get_column_name(madm_label::set_id)).c_str()).toInt();
			id_txt<<id<<" "<<name;
			child->setText(0, id_txt.str().c_str());
			id_txt.str("");
			id_txt<<id;
			buffer <<buffer_fix.str()<< name<<"/";
			child->setData(0,Qt::AccessibleDescriptionRole, buffer.str().c_str());
			child->setToolTip(0, buffer.str().c_str());
			child->setData(0,Qt::UserRole, id_txt.str().c_str());
			buffer.str("");
			id_txt.str("");
		}
	}

	//catchment area risk approach
	buffer_fix.str("");
	buffer_fix<< "/"<<sys_label::str_madm<<"/"<< sys_data_tree_id::id_results<<"/"<<risk_label::risk_catchment<<"/";
	found=this->start_module_tree_search(buffer_fix.str(), _sys_system_modules::MADM_SYS);
	if(found!=NULL && _Madm_Solver::check_data_in_ranking_table(&this->database, this->system_id->area_state, _risk_type::catchment_risk)==true){
		//create the children
		for(int i=0; i< number; i++){
			child=new QTreeWidgetItem(found);
			name=model.record(i).value((Madm_System::table_sets->get_column_name(madm_label::set_name)).c_str()).toString().toStdString();
			id=model.record(i).value((Madm_System::table_sets->get_column_name(madm_label::set_id)).c_str()).toInt();
			id_txt<<id<<" "<<name;
			child->setText(0, id_txt.str().c_str());
			id_txt.str("");
			id_txt<<id;
			buffer <<buffer_fix.str()<< name<<"/";
			child->setData(0,Qt::AccessibleDescriptionRole, buffer.str().c_str());
			child->setToolTip(0, buffer.str().c_str());
			child->setData(0,Qt::UserRole, id_txt.str().c_str());
			buffer.str("");
			id_txt.str("");
		}
	}
}
//Set-up current data for the module risk
void Sys_Data_Tree_Wid::set_up_current_data_risk(void){
	QSqlQueryModel results;
	int number=0;

	ostringstream buffer_fix;
	ostringstream buffer;
	string name;
	ostringstream id_txt;
	int id;
	QTreeWidgetItem *found=NULL;
	QTreeWidgetItem *child=NULL;

	//load the hydaulic boundary scenarios
	Hyd_Boundary_Szenario_Management hyd_sc_manager;
	hyd_sc_manager.set_systemid(*this->system_id);
	hyd_sc_manager.set_szenario_per_db(&this->database);

	//detailed result (nobreak)
	try{
		//select the number of section
		number=Risk_System::select_data_in_resulttable_detailed(&results, &this->database, *this->system_id,_risk_type::nobreak_risk);
	}
	catch(Error msg){
		throw msg;
	}

	if(number>0){
		buffer_fix<< "/"<<sys_label::str_risk<<"/"<< risk_label::risk_nobreak<<"/";
		found=this->start_module_tree_search(buffer_fix.str(), _sys_system_modules::RISK_SYS);
		if(found!=NULL){
			child=new QTreeWidgetItem(found);
			name=sys_data_tree_id::id_detailed_results;
			child->setText(0, name.c_str());
			buffer <<buffer_fix.str()<< name<<"/";
			child->setData(0,Qt::AccessibleDescriptionRole, buffer.str().c_str());
			child->setToolTip(0, buffer.str().c_str());

			found=child;
			buffer_fix.str("");
			buffer_fix<<buffer.str();
			buffer.str("");
			id_txt.str("");
		}
	}
	else{
		found=NULL;
	}

	if(found!=NULL){
		//create the children
		child=new QTreeWidgetItem(found);
		name=sys_data_tree_id::id_summary_results;
		child->setText(0, name.c_str());
		buffer <<buffer_fix.str()<< name<<"/";
		child->setData(0,Qt::AccessibleDescriptionRole, buffer.str().c_str());
		child->setToolTip(0, buffer.str().c_str());
		child->setData(0,Qt::UserRole, "1");
		buffer.str("");
		int id_sc=0;
		for(int i=0; i< number; i++){
			child=new QTreeWidgetItem(found);
			id_sc=results.record(i).value((Risk_System::table_detailed_results->get_column_name(hyd_label::sz_bound_id)).c_str()).toInt();
			id=results.record(i).value((Risk_System::table_detailed_results->get_column_name(label::glob_id)).c_str()).toInt();
			id_txt<<id;
			Hyd_Boundary_Szenario *null_test;
			null_test=hyd_sc_manager.get_ptr_sz_id_given(id_sc);
			if(null_test==NULL){
				continue;
			}

			name=null_test->get_name();

			child->setText(0, name.c_str());
			buffer <<buffer_fix.str()<< name<<"/";
			child->setData(0,Qt::AccessibleDescriptionRole, buffer.str().c_str());
			child->setToolTip(0, buffer.str().c_str());
			child->setData(0,Qt::UserRole, id_txt.str().c_str());
			buffer.str("");
			id_txt.str("");
		}
	}
	//reset all
	buffer.str("");
	id_txt.str("");
	buffer_fix.str("");
	found=NULL;

	//cumulated result (nobreak)
	try{
		//select the number of section
		number=Risk_System::select_data_in_resulttable_cumulated(&results, &this->database, *this->system_id,_risk_type::nobreak_risk);
	}
	catch(Error msg){
		throw msg;
	}
	//search for the parent
	buffer_fix<< "/"<<sys_label::str_risk<<"/"<< risk_label::risk_nobreak<<"/";
	found=this->start_module_tree_search(buffer_fix.str(), _sys_system_modules::RISK_SYS);
	if(found!=NULL){
		//create the children
		if(number>0){
			child=new QTreeWidgetItem(found);
			name=sys_data_tree_id::risk_cumulated_results;
			child->setText(0, name.c_str());
			buffer <<buffer_fix.str()<< name<<"/";
			child->setData(0,Qt::AccessibleDescriptionRole, buffer.str().c_str());
			child->setToolTip(0, buffer.str().c_str());
			child->setData(0,Qt::UserRole, "1");
			buffer.str("");
			id_txt.str("");
		}
	}
	//reset all
	buffer.str("");
	id_txt.str("");
	buffer_fix.str("");
	found=NULL;

	//total result (nobreak)
	try{
		//select the number of section
		number=Risk_System::select_data_in_resulttable(&results, &this->database, *this->system_id,_risk_type::nobreak_risk);
	}
	catch(Error msg){
		throw msg;
	}
	//search for the parent
	buffer_fix<< "/"<<sys_label::str_risk<<"/"<< risk_label::risk_nobreak<<"/";
	found=this->start_module_tree_search(buffer_fix.str(), _sys_system_modules::RISK_SYS);
	if(found!=NULL){
		//create the children
		if(number>0){
			child=new QTreeWidgetItem(found);
			name=sys_data_tree_id::risk_total_results;
			id=results.record(0).value((Risk_System::table_results->get_column_name(label::glob_id)).c_str()).toInt();
			id_txt<<id;
			child->setText(0, name.c_str());
			buffer <<buffer_fix.str()<< name<<"/";
			child->setData(0,Qt::AccessibleDescriptionRole, buffer.str().c_str());
			child->setToolTip(0, buffer.str().c_str());
			child->setData(0,Qt::UserRole, id_txt.str().c_str());
			buffer.str("");
			id_txt.str("");
		}
	}
	//reset all
	buffer.str("");
	id_txt.str("");
	buffer_fix.str("");
	found=NULL;

	//detailed result (scenario)
	try{
		//select the number of section
		number=Risk_System::select_data_in_resulttable_detailed(&results, &this->database, *this->system_id,_risk_type::scenario_risk);
	}
	catch(Error msg){
		throw msg;
	}
	if(number>0){
		buffer_fix<< "/"<<sys_label::str_risk<<"/"<< risk_label::risk_scenario<<"/";
		found=this->start_module_tree_search(buffer_fix.str(), _sys_system_modules::RISK_SYS);
		if(found!=NULL){
			child=new QTreeWidgetItem(found);
			name=sys_data_tree_id::id_detailed_results;
			child->setText(0, name.c_str());
			buffer <<buffer_fix.str()<< name<<"/";
			child->setData(0,Qt::AccessibleDescriptionRole, buffer.str().c_str());
			child->setToolTip(0, buffer.str().c_str());

			found=child;
			buffer_fix.str("");
			buffer_fix<<buffer.str();
			buffer.str("");
			id_txt.str("");
		}
	}
	else{
		found=NULL;
	}

	if(found!=NULL){
		//create the children
		child=new QTreeWidgetItem(found);
		name=sys_data_tree_id::id_summary_results;
		child->setText(0, name.c_str());
		buffer <<buffer_fix.str()<< name<<"/";
		child->setData(0,Qt::AccessibleDescriptionRole, buffer.str().c_str());
		child->setToolTip(0, buffer.str().c_str());
		child->setData(0,Qt::UserRole, "1");
		buffer.str("");

		int id_sc=0;
		for(int i=0; i< number; i++){
			child=new QTreeWidgetItem(found);
			id_sc=results.record(i).value((Risk_System::table_detailed_results->get_column_name(hyd_label::sz_bound_id)).c_str()).toInt();
			id=results.record(i).value((Risk_System::table_detailed_results->get_column_name(label::glob_id)).c_str()).toInt();
			id_txt<<id;
			ostringstream buffer_name;
			Hyd_Boundary_Szenario *null_test;
			null_test=hyd_sc_manager.get_ptr_sz_id_given(id_sc);
			if(null_test==NULL){
				continue;
			}
			buffer_name<<null_test->get_name()<<"-";
			buffer_name<< results.record(i).value((Risk_System::table_detailed_results->get_column_name(risk_label::sz_break_id)).c_str()).toString().toStdString();
			name=buffer_name.str();
			child->setText(0, name.c_str());
			buffer <<buffer_fix.str()<< name<<"/";
			child->setData(0,Qt::AccessibleDescriptionRole, buffer.str().c_str());
			child->setToolTip(0, buffer.str().c_str());
			child->setData(0,Qt::UserRole, id_txt.str().c_str());
			buffer.str("");
			id_txt.str("");
		}
	}
	//reset all
	buffer.str("");
	id_txt.str("");
	buffer_fix.str("");
	found=NULL;

	//catchment area risk approach
	//scenario catchment
	try{
		//select the number of scenarios
		number=Risk_Break_Scenario_Management::select_data_in_table(&results, &this->database, _risk_type::catchment_risk,*this->system_id);
	}
	catch(Error msg){
		throw msg;
	}

	if(number>0){
		buffer_fix<< "/"<<sys_label::str_risk<<"/"<< risk_label::risk_catchment<<"/";
		found=this->start_module_tree_search(buffer_fix.str(), _sys_system_modules::RISK_SYS);
		if(found!=NULL){
			child=new QTreeWidgetItem(found);
			name=sys_data_tree_id::risk_scenarios;
			child->setText(0, name.c_str());
			buffer <<buffer_fix.str()<< name<<"/";
			child->setData(0,Qt::AccessibleDescriptionRole, buffer.str().c_str());
			child->setToolTip(0, buffer.str().c_str());
			child->setData(0,Qt::UserRole, "1");
			child->setData(1,Qt::DisplayRole, number);
		}
	}
	//reset all
	buffer.str("");
	id_txt.str("");
	buffer_fix.str("");
	found=NULL;

	//make statistics
	if(number>0){
		buffer_fix<< "/"<<sys_label::str_risk<<"/"<< risk_label::risk_catchment<<"/";
		found=this->start_module_tree_search(buffer_fix.str(), _sys_system_modules::RISK_SYS);
		if(found!=NULL){
			child=new QTreeWidgetItem(found);
			name=sys_data_tree_id::risk_ca_statistics;
			child->setText(0, name.c_str());
			buffer <<buffer_fix.str()<< name<<"/";
			child->setData(0,Qt::AccessibleDescriptionRole, buffer.str().c_str());
			child->setToolTip(0, buffer.str().c_str());
			child->setData(0,Qt::UserRole, "1");
			child->setData(1,Qt::DisplayRole, 1);
		}
	}
	//reset all
	buffer.str("");
	id_txt.str("");
	buffer_fix.str("");
	found=NULL;

	try{
		//select the number of scenarios
		number=Risk_Break_Scenario_Management::select_data_in_table_fpl_sec_prob(&results, &this->database, *this->system_id);
	}
	catch(Error msg){
		throw msg;
	}

	//fpl-section probability
	if(number>0){
		buffer_fix<< "/"<<sys_label::str_risk<<"/"<< risk_label::risk_catchment<<"/";
		found=this->start_module_tree_search(buffer_fix.str(), _sys_system_modules::RISK_SYS);
		if(found!=NULL){
			child=new QTreeWidgetItem(found);
			name=sys_data_tree_id::risk_prob_section;
			child->setText(0, name.c_str());
			buffer <<buffer_fix.str()<< name<<"/";
			child->setData(0,Qt::AccessibleDescriptionRole, buffer.str().c_str());
			child->setToolTip(0, buffer.str().c_str());
			child->setData(0,Qt::UserRole, "1");
			child->setData(1,Qt::DisplayRole, number);
		}
	}
	//reset all
	buffer.str("");
	id_txt.str("");
	buffer_fix.str("");
	found=NULL;

	//detailed result (catchment)
	try{
		//select the number of results
		number=Risk_System::select_data_in_resulttable_detailed(&results, &this->database, *this->system_id,_risk_type::catchment_risk);
	}
	catch(Error msg){
		throw msg;
	}

	if(number>0){
		buffer_fix<< "/"<<sys_label::str_risk<<"/"<< risk_label::risk_catchment<<"/";
		found=this->start_module_tree_search(buffer_fix.str(), _sys_system_modules::RISK_SYS);
		if(found!=NULL){
			child=new QTreeWidgetItem(found);
			name=sys_data_tree_id::id_detailed_results;
			child->setText(0, name.c_str());
			buffer <<buffer_fix.str()<< name<<"/";
			child->setData(0,Qt::AccessibleDescriptionRole, buffer.str().c_str());
			child->setToolTip(0, buffer.str().c_str());

			found=child;
			buffer_fix.str("");
			buffer_fix<<buffer.str();
			buffer.str("");
			id_txt.str("");
		}
	}
	else{
		found=NULL;
	}
	if(found!=NULL){
		//create the children
		child=new QTreeWidgetItem(found);
		name=sys_data_tree_id::id_summary_results;
		child->setText(0, name.c_str());
		buffer <<buffer_fix.str()<< name<<"/";
		child->setData(0,Qt::AccessibleDescriptionRole, buffer.str().c_str());
		child->setToolTip(0, buffer.str().c_str());
		child->setData(0,Qt::UserRole, "1");
		buffer.str("");

		int id_sc=0;
		ostringstream buffer_name;
		//create the children
		for(int i=0; i< number; i++){
			buffer_name.str("");
			child=new QTreeWidgetItem(found);
			id_sc=results.record(i).value((Risk_System::table_detailed_results->get_column_name(hyd_label::sz_bound_id)).c_str()).toInt();
			id=results.record(i).value((Risk_System::table_detailed_results->get_column_name(label::glob_id)).c_str()).toInt();
			id_txt<<id;
			Hyd_Boundary_Szenario *null_test;
			null_test=hyd_sc_manager.get_ptr_sz_id_given(id_sc);
			if(null_test==NULL){
				continue;
			}
			buffer_name<<null_test->get_name()<<"_";

			buffer_name<<results.record(i).value((Risk_System::table_detailed_results->get_column_name(risk_label::sz_break_id)).c_str()).toString().toStdString();
			name=buffer_name.str();
			child->setText(0, name.c_str());
			buffer <<buffer_fix.str()<< name<<"/";
			child->setData(0,Qt::AccessibleDescriptionRole, buffer.str().c_str());
			child->setToolTip(0, buffer.str().c_str());
			child->setData(0,Qt::UserRole, id_txt.str().c_str());
			buffer.str("");
			id_txt.str("");
		}
	}
	//reset all
	buffer.str("");
	id_txt.str("");
	buffer_fix.str("");
	found=NULL;

	//cumulated result (catchment)
	try{
		//select the number of section
		number=Risk_System::select_data_in_resulttable_cumulated(&results, &this->database, *this->system_id,_risk_type::catchment_risk);
	}
	catch(Error msg){
		throw msg;
	}
	//search for the parent
	buffer_fix<< "/"<<sys_label::str_risk<<"/"<< risk_label::risk_catchment<<"/";
	found=this->start_module_tree_search(buffer_fix.str(), _sys_system_modules::RISK_SYS);
	if(found!=NULL){
		//create the children
		if(number>0){
			child=new QTreeWidgetItem(found);
			name=sys_data_tree_id::risk_cumulated_results;
			child->setText(0, name.c_str());
			buffer <<buffer_fix.str()<< name<<"/";
			child->setData(0,Qt::AccessibleDescriptionRole, buffer.str().c_str());
			child->setToolTip(0, buffer.str().c_str());
			child->setData(0,Qt::UserRole, "1");
			buffer.str("");
			id_txt.str("");
		}
	}
	//reset all
	buffer.str("");
	id_txt.str("");
	buffer_fix.str("");
	found=NULL;

	//total result (nobreak)
	try{
		//select the number of section
		number=Risk_System::select_data_in_resulttable(&results, &this->database, *this->system_id,_risk_type::catchment_risk);
	}
	catch(Error msg){
		throw msg;
	}
	//search for the parent
	buffer_fix<< "/"<<sys_label::str_risk<<"/"<< risk_label::risk_catchment<<"/";
	found=this->start_module_tree_search(buffer_fix.str(), _sys_system_modules::RISK_SYS);
	if(found!=NULL){
		//create the children
		for(int i=0; i< number; i++){
			child=new QTreeWidgetItem(found);
			name=sys_data_tree_id::risk_total_results;
			id=results.record(i).value((Risk_System::table_results->get_column_name(label::glob_id)).c_str()).toInt();
			id_txt<<id;
			child->setText(0, name.c_str());
			buffer <<buffer_fix.str()<< name<<"/";
			child->setData(0,Qt::AccessibleDescriptionRole, buffer.str().c_str());
			child->setToolTip(0, buffer.str().c_str());
			child->setData(0,Qt::UserRole, id_txt.str().c_str());
			buffer.str("");
			id_txt.str("");
		}
	}
	//reset all
	buffer.str("");
	id_txt.str("");
	buffer_fix.str("");
	found=NULL;
}
//Set-up current data for the module alt
void Sys_Data_Tree_Wid::set_up_current_data_alt(void){
	QSqlQueryModel results;
	int number=0;

	ostringstream buffer_fix;
	ostringstream buffer;
	string name;
	ostringstream id_txt;
	int id;
	QTreeWidgetItem *found=NULL;
	QTreeWidgetItem *child=NULL;

	QStringList buff_list;
	Alt_System::load_stringlist_current_alt(&this->database, this->system_id->area_state, &buff_list);
	number=buff_list.count();

	if(number>0){
		buffer_fix<< "/"<<sys_label::str_alt<<"/";
		found=this->start_module_tree_search(buffer_fix.str(), _sys_system_modules::ALT_SYS);
		if(found!=NULL){
			for(int i=0; i< number; i++){
				stringstream id_stream;
				child=new QTreeWidgetItem(found);
				id_stream << buff_list.at(i).toStdString();
				id_stream >> id;
				id_txt << id;
				name=buff_list.at(i).toStdString();
				int pos_empty;
				pos_empty=name.find(" ");
				name=name.substr(pos_empty);

				ostringstream buffer_name;
				buffer_name<<id<<"-"<<name;
				name=buffer_name.str();
				child->setText(0, name.c_str());
				buffer <<buffer_fix.str()<< name<<"/";
				child->setData(0,Qt::AccessibleDescriptionRole, buffer.str().c_str());
				child->setToolTip(0, buffer.str().c_str());
				child->setData(0,Qt::UserRole, id_txt.str().c_str());
				buffer.str("");
				id_txt.str("");
			}
		}
	}
	//reset all
	buffer.str("");
	id_txt.str("");
	buffer_fix.str("");
	found=NULL;
}
//Set-up current data for the module cost
void Sys_Data_Tree_Wid::set_up_current_data_cost(void){
}
//Set widget to the hydraulic view
void Sys_Data_Tree_Wid::widget2hyd(QTreeWidgetItem *item){
	this->delete_hyd_lay();
	QVBoxLayout *hyd_lay;
	hyd_lay=new QVBoxLayout(this->ptr_data_tab_hyd);

	string str_mod;
	string given=item->data(0,Qt::AccessibleDescriptionRole).toString().toStdString();
	string check;
	int pos=0;
	ostringstream buffer;
	buffer<<"/"<<sys_label::str_hyd<<"/";

	str_mod=buffer.str();
	buffer.str("");

	//hyd system
	buffer << str_mod<<sys_data_tree_id::id_config<<"/";
	check=buffer.str();
	buffer.str("");
	if(check==given){
		HydGui_System_Member_Wid *wid;
		wid=new HydGui_System_Member_Wid;
		wid->setParent(this->ptr_data_tab_hyd);
		if(this->current_project->get_stored_risk_state().risk_state_flag==true || this->system_id->measure_nr>0){
			wid->set_edit_action_disabled(true);
		}
		//set the data
		wid->set_head_spinBox_range(1,1);
		try{
			wid->set_member(&this->database);
		}
		catch(Error msg){
			throw msg;
		}

		hyd_lay->addWidget(wid);
		wid->show();
		return;
	}
	//hyd boundary scenarios
	buffer << str_mod<<sys_data_tree_id::hyd_bound_sc_id<<"/";
	check=buffer.str();
	buffer.str("");
	if(check==given){
		HydGui_Bound_Scenario_Table_Wid *wid;
		wid=new HydGui_Bound_Scenario_Table_Wid;
		wid->setParent(this->ptr_data_tab_hyd);
		//set the data via the boundary scenario manager
		Hyd_Boundary_Szenario_Management my_manager;
		my_manager.set_systemid(*this->system_id);

		try{
			my_manager.set_szenario_per_db(&this->database);
			wid->set_rows(my_manager.get_number_sz());
			wid->set_rows_existing_sc(my_manager.get_number_sz(), my_manager.get_ptr_boundary_scenario());
		}
		catch(Error msg){
			throw msg;
		}
		hyd_lay->addWidget(wid);
		wid->show();
		return;
	}
	//hyd floodplain
	buffer << str_mod<<sys_data_tree_id::hyd_floodplain_id<<"/";
	pos=given.find(buffer.str());
	int fp_nr=-1;
	if(pos>=0){
		fp_nr=item->data(0, Qt::UserRole).toInt();
		HydGui_Floodplain_Member_Wid *wid;
		wid=new HydGui_Floodplain_Member_Wid;
		wid->setParent(this->ptr_data_tab_hyd);
		//set head spin box
		int number_fp=0;
		number_fp=Hyd_Model_Floodplain::check_relevant_model_database(&this->database);
		wid->set_head_spinBox_range(number_fp,1);
		int current_number=0;
		current_number=this->calculate_item_number(item);
		wid->set_head_spinBox_value(current_number+1);
		wid->set_treeItem(item);
		QObject::connect(wid, SIGNAL(send_change_widget(int , QTreeWidgetItem * , int , int )), this, SLOT(change_widget_by_spinbox(int , QTreeWidgetItem *, int, int)));

		//set the data
		try{
			wid->set_member(&this->database, fp_nr);
		}
		catch(Error msg){
			throw msg;
		}

		hyd_lay->addWidget(wid);
		wid->set_current_scroll_bar(this->current_ver_slider_pos, this->current_hor_slider_pos);
		wid->show();
		return;
	}
	buffer.str("");
	//hyd river
	buffer << str_mod<<sys_data_tree_id::hyd_river_id<<"/";
	pos=given.find(buffer.str());
	int rv_id=-1;
	if(pos>=0){
		//river settings
		if(item->childCount()!=0){
			rv_id=item->data(0, Qt::UserRole).toInt();
			HydGui_River_Member_Wid *wid;
			wid=new HydGui_River_Member_Wid;
			wid->setParent(this->ptr_data_tab_hyd);
			//set head spin box
			int number_rv=0;
			number_rv=Hyd_Model_River::check_relevant_model_database(&this->database);
			wid->set_head_spinBox_range(number_rv,1);
			int current_number=0;
			current_number=calculate_item_number(item);
			wid->set_head_spinBox_value(current_number+1);
			wid->set_treeItem(item);
			QObject::connect(wid, SIGNAL(send_change_widget(int , QTreeWidgetItem *, int, int )), this, SLOT(change_widget_by_spinbox(int , QTreeWidgetItem *, int, int)));

			//set the data
			try{
				wid->set_member(&this->database, rv_id);
			}
			catch(Error msg){
				throw msg;
			}

			hyd_lay->addWidget(wid);
			wid->set_current_scroll_bar(this->current_ver_slider_pos, this->current_hor_slider_pos);
			wid->show();
			return;
		}
		//river profile
		else{
			int prof_id=-1;
			rv_id=item->parent()->data(0, Qt::UserRole).toInt();
			prof_id=item->data(0, Qt::UserRole).toInt();

			HydGui_Rv_Profile_General_Wid *wid;
			wid=new HydGui_Rv_Profile_General_Wid;
			wid->setParent(this->ptr_data_tab_hyd);
			//set head spin box
			int number_profs=0;
			int current_number=0;
			QSqlQueryModel buffer;
			number_profs=_Hyd_River_Profile::select_relevant_profiles_in_database(&buffer, &this->database,*this->system_id, rv_id, false);
			current_number=calculate_item_number(item);
			wid->set_head_spinBox_range(number_profs,1);
			wid->set_head_spinBox_value(current_number+1);
			wid->set_treeItem(item);
			QObject::connect(wid, SIGNAL(send_change_widget(int , QTreeWidgetItem *, int, int )), this, SLOT(change_widget_by_spinbox(int , QTreeWidgetItem *, int , int)));

			//set data
			try{
				wid->set_member(&this->database, rv_id, prof_id, *this->system_id);
			}
			catch(Error msg){
				throw msg;
			}

			hyd_lay->addWidget(wid);
			wid->set_current_scroll_bar(this->current_ver_slider_pos, this->current_hor_slider_pos);
			wid->show();
			return;
		}
	}
	buffer.str("");
}
//Set widget to the damage view
void Sys_Data_Tree_Wid::widget2dam(QTreeWidgetItem *item){
	QSqlQueryModel raster;
	QSqlTableModel function(0, this->database);
	this->delete_dam_lay();
	QVBoxLayout *dam_lay;
	dam_lay=new QVBoxLayout(this->ptr_data_tab_dam);

	string str_mod;
	string given=item->data(0,Qt::AccessibleDescriptionRole).toString().toStdString();
	string check;
	int pos=0;
	ostringstream buffer;
	buffer<<"/"<<sys_label::str_dam<<"/";

	str_mod=buffer.str();
	buffer.str("");

	//data
	//ecn raster
	buffer << str_mod<<sys_data_tree_id::dam_ecn_id<<"/"<<sys_data_tree_id::dam_raster_id<<"/";
	pos=given.find(buffer.str());
	int raster_nr=-1;
	if(pos>=0){
		raster_nr=item->data(0, Qt::UserRole).toInt();
		DamGui_Raster_Wid *wid;
		wid=new DamGui_Raster_Wid;
        //this->ptr_data_dam=wid;
        wid->setParent(this->ptr_data_tab_dam);
		//set head spin box
		int number_raster=0;
		number_raster=Dam_Ecn_Raster::select_relevant_raster_database(&raster,&this->database, false);
		wid->set_head_spinBox_range(number_raster,1);
		int current_number=0;
		current_number=calculate_item_number(item);
		wid->set_head_spinBox_value(current_number+1);
		wid->set_treeItem(item);
		QObject::connect(wid, SIGNAL(send_change_widget(int , QTreeWidgetItem * , int , int  )), this, SLOT(change_widget_by_spinbox(int , QTreeWidgetItem * , int , int )));

		//set the data
		try{
			wid->set_member(&this->database, raster_nr, _dam_raster_types::ecn_total);
		}
		catch(Error msg){
			throw msg;
		}
		dam_lay->addWidget(wid);
		wid->set_current_scroll_bar(this->current_ver_slider_pos, this->current_hor_slider_pos);
		wid->show();
		return;
	}
	buffer.str("");

	//ecn damage function
	buffer << str_mod<<sys_data_tree_id::dam_ecn_id<<"/"<<sys_data_tree_id::dam_land_use_cat<<"/";
	pos=given.find(buffer.str());
	if(pos>=0){
		raster_nr=item->data(0, Qt::UserRole).toInt();
		DamGui_Ecn_Func_Wid *wid;
        wid=new DamGui_Ecn_Func_Wid(DisplayRole,this->ptr_data_tab_dam);
        //this->ptr_data_dam=wid;
        //wid->setParent(this->ptr_data_tab_dam);
		//set head spin box
		int number_raster=0;
		number_raster=Dam_Ecn_Damage_Function::select_relevant_functions_database(&function,false);
		wid->set_head_spinBox_range(number_raster,1);
		int current_number=0;
		current_number=calculate_item_number(item);
		wid->set_head_spinBox_value(current_number+1);
		wid->set_treeItem(item);
		QObject::connect(wid, SIGNAL(send_change_widget(int , QTreeWidgetItem * , int , int  )), this, SLOT(change_widget_by_spinbox(int , QTreeWidgetItem * , int , int )));

		//set the data
		try{
			wid->set_member(&this->database, raster_nr);
		}
		catch(Error msg){
			throw msg;
		}
		dam_lay->addWidget(wid);
		wid->set_current_scroll_bar(this->current_ver_slider_pos, this->current_hor_slider_pos);
		wid->show();
		return;
	}
	buffer.str("");

	//eco b_type raster
	buffer << str_mod<<sys_data_tree_id::dam_eco_id<<"/"<<sys_data_tree_id::dam_eco_btype_id<<"/"<<sys_data_tree_id::dam_raster_id<<"/";
	pos=given.find(buffer.str());
	if(pos>=0){
		raster_nr=item->data(0, Qt::UserRole).toInt();
		DamGui_Raster_Wid *wid;
		wid=new DamGui_Raster_Wid;
		wid->setParent(this->ptr_data_tab_dam);
		//set head spin box
		int number_raster=0;
		number_raster=Dam_Eco_Btype_Raster::select_relevant_raster_database(&raster,&this->database, false);
		wid->set_head_spinBox_range(number_raster,1);
		int current_number=0;
		current_number=calculate_item_number(item);
		wid->set_head_spinBox_value(current_number+1);
		wid->set_treeItem(item);
		QObject::connect(wid, SIGNAL(send_change_widget(int , QTreeWidgetItem * , int , int )), this, SLOT(change_widget_by_spinbox(int , QTreeWidgetItem *, int , int )));

		//set the data
		try{
			wid->set_member(&this->database, raster_nr, _dam_raster_types::eco_btype);
		}
		catch(Error msg){
			throw msg;
		}
		dam_lay->addWidget(wid);
		wid->set_current_scroll_bar(this->current_ver_slider_pos, this->current_hor_slider_pos);
		wid->show();
		return;
	}
	buffer.str("");

	//eco b_type function
	buffer << str_mod<<sys_data_tree_id::dam_eco_id<<"/"<<sys_data_tree_id::dam_eco_btype_id<<"/"<<sys_data_tree_id::dam_biotope_type_id<<"/";
	pos=given.find(buffer.str());
	if(pos>=0){
		raster_nr=item->data(0, Qt::UserRole).toInt();
		DamGui_Eco_Btype_Wid *wid;
		wid=new DamGui_Eco_Btype_Wid;
		wid->setParent(this->ptr_data_tab_dam);
		//set head spin box
		int number_raster=0;
		number_raster=Dam_Eco_Btype::select_relevant_biotope_types_database(&function,false);
		wid->set_head_spinBox_range(number_raster,1);
		int current_number=0;
		current_number=calculate_item_number(item);
		wid->set_head_spinBox_value(current_number+1);
		wid->set_treeItem(item);
		QObject::connect(wid, SIGNAL(send_change_widget(int , QTreeWidgetItem * , int , int)), this, SLOT(change_widget_by_spinbox(int , QTreeWidgetItem *, int , int)));

		//set the data
		try{
			wid->set_member(&this->database, raster_nr);
		}
		catch(Error msg){
			throw msg;
		}
		dam_lay->addWidget(wid);
		wid->set_current_scroll_bar(this->current_ver_slider_pos, this->current_hor_slider_pos);
		wid->show();
		return;
	}
	buffer.str("");

	//eco soil erosion raster
	buffer << str_mod<<sys_data_tree_id::dam_eco_id<<"/"<<sys_data_tree_id::dam_eco_soil_id<<"/"<<sys_data_tree_id::dam_raster_id<<"/";
	pos=given.find(buffer.str());
	if(pos>=0){
		raster_nr=item->data(0, Qt::UserRole).toInt();
		DamGui_Raster_Wid *wid;
		wid=new DamGui_Raster_Wid;
		wid->setParent(this->ptr_data_tab_dam);
		//set head spin box
		int number_raster=0;
		number_raster=Dam_Eco_Soil_Raster::select_relevant_raster_database(&raster,&this->database, false);
		wid->set_head_spinBox_range(number_raster,1);
		int current_number=0;
		current_number=calculate_item_number(item);
		wid->set_head_spinBox_value(current_number+1);
		wid->set_treeItem(item);
		QObject::connect(wid, SIGNAL(send_change_widget(int , QTreeWidgetItem * , int , int)), this, SLOT(change_widget_by_spinbox(int , QTreeWidgetItem *, int , int)));

		//set the data
		try{
			wid->set_member(&this->database, raster_nr, _dam_raster_types::eco_soil_total);
		}
		catch(Error msg){
			throw msg;
		}
		dam_lay->addWidget(wid);
		wid->set_current_scroll_bar(this->current_ver_slider_pos, this->current_hor_slider_pos);
		wid->show();
		return;
	}
	buffer.str("");

	//eco soil cost function
	buffer << str_mod<<sys_data_tree_id::dam_eco_id<<"/"<<sys_data_tree_id::dam_eco_soil_id<<"/"<<sys_data_tree_id::dam_soil_cost_id<<"/";
	pos=given.find(buffer.str());
	if(pos>=0){
		raster_nr=item->data(0, Qt::UserRole).toInt();
		DamGui_Eco_Soil_Cost_Wid *wid;
		wid=new DamGui_Eco_Soil_Cost_Wid;
		wid->setParent(this->ptr_data_tab_dam);
		//set head spin box
		int number_raster=0;
		number_raster=Dam_Eco_Soil_Cost_Function::select_relevant_functions_database(&function,false);
		wid->set_head_spinBox_range(number_raster,1);
		int current_number=0;
		current_number=calculate_item_number(item);
		wid->set_head_spinBox_value(current_number+1);
		wid->set_treeItem(item);
		QObject::connect(wid, SIGNAL(send_change_widget(int , QTreeWidgetItem * , int , int )), this, SLOT(change_widget_by_spinbox(int , QTreeWidgetItem *, int , int )));

		//set the data
		try{
			wid->set_member(&this->database, raster_nr);
		}
		catch(Error msg){
			throw msg;
		}
		dam_lay->addWidget(wid);
		wid->set_current_scroll_bar(this->current_ver_slider_pos, this->current_hor_slider_pos);
		wid->show();
		return;
	}
	buffer.str("");

	//eco soil erosion function
	buffer << str_mod<<sys_data_tree_id::dam_eco_id<<"/"<<sys_data_tree_id::dam_eco_soil_id<<"/"<<sys_data_tree_id::dam_soil_ero_id<<"/";
	pos=given.find(buffer.str());
	if(pos>=0){
		raster_nr=item->data(0, Qt::UserRole).toInt();
		DamGui_Eco_Soil_Erosion_Wid *wid;
		wid=new DamGui_Eco_Soil_Erosion_Wid;
		wid->setParent(this->ptr_data_tab_dam);
		//set head spin box
		int number_raster=0;
		number_raster=Dam_Eco_Soil_Erosion_Function::select_relevant_functions_database(&function,false);
		wid->set_head_spinBox_range(number_raster,1);
		int current_number=0;
		current_number=calculate_item_number(item);
		wid->set_head_spinBox_value(current_number+1);
		wid->set_treeItem(item);
		QObject::connect(wid, SIGNAL(send_change_widget(int , QTreeWidgetItem * , int , int )), this, SLOT(change_widget_by_spinbox(int , QTreeWidgetItem *, int , int )));

		//set the data
		try{
			wid->set_member(&this->database, raster_nr);
		}
		catch(Error msg){
			throw msg;
		}
		dam_lay->addWidget(wid);
		wid->set_current_scroll_bar(this->current_ver_slider_pos, this->current_hor_slider_pos);
		wid->show();
		return;
	}
	buffer.str("");

	//people2risk raster
	buffer << str_mod<<sys_data_tree_id::dam_pop_id<<"/"<<sys_data_tree_id::dam_raster_id<<"/";
	pos=given.find(buffer.str());
	if(pos>=0){
		raster_nr=item->data(0, Qt::UserRole).toInt();
		DamGui_Raster_Wid *wid;
		wid=new DamGui_Raster_Wid;
		wid->setParent(this->ptr_data_tab_dam);
		//set head spin box
		int number_raster=0;
		number_raster=Dam_People_Raster::select_relevant_raster_database(&raster,&this->database, false);
		wid->set_head_spinBox_range(number_raster,1);
		int current_number=0;
		current_number=calculate_item_number(item);
		wid->set_head_spinBox_value(current_number+1);
		wid->set_treeItem(item);
		QObject::connect(wid, SIGNAL(send_change_widget(int , QTreeWidgetItem * , int , int )), this, SLOT(change_widget_by_spinbox(int , QTreeWidgetItem *, int , int )));

		//set the data
		try{
			wid->set_member(&this->database, raster_nr, _dam_raster_types::pop_total);
		}
		catch(Error msg){
			throw msg;
		}
		dam_lay->addWidget(wid);
		wid->set_current_scroll_bar(this->current_ver_slider_pos, this->current_hor_slider_pos);
		wid->show();
		return;
	}
	buffer.str("");

	//pop category
	buffer << str_mod<<sys_data_tree_id::dam_pop_id<<"/"<<sys_data_tree_id::dam_vul_cat<<"/";
	pos=given.find(buffer.str());
	if(pos>=0){
		raster_nr=item->data(0, Qt::UserRole).toInt();
		DamGui_Pop_Function_Wid *wid;
		wid=new DamGui_Pop_Function_Wid;
		wid->setParent(this->ptr_data_tab_dam);
		//set head spin box
		int number_raster=0;
		number_raster=Dam_People_Damage_Function::select_relevant_functions_database(&function,false);
		wid->set_head_spinBox_range(number_raster,1);
		int current_number=0;
		current_number=calculate_item_number(item);
		wid->set_head_spinBox_value(current_number+1);
		wid->set_treeItem(item);
		QObject::connect(wid, SIGNAL(send_change_widget(int , QTreeWidgetItem * , int , int )), this, SLOT(change_widget_by_spinbox(int , QTreeWidgetItem *, int , int )));

		//set the data
		try{
			wid->set_member(&this->database, raster_nr);
		}
		catch(Error msg){
			throw msg;
		}
		dam_lay->addWidget(wid);
		wid->set_current_scroll_bar(this->current_ver_slider_pos, this->current_hor_slider_pos);
		wid->show();
		return;
	}
	buffer.str("");

	//psychosocial raster
	buffer << str_mod<<sys_data_tree_id::dam_pys_id<<"/"<<sys_data_tree_id::dam_raster_id<<"/";
	pos=given.find(buffer.str());
	if(pos>=0){
		raster_nr=item->data(0, Qt::UserRole).toInt();
		DamGui_Raster_Wid *wid;
		wid=new DamGui_Raster_Wid;
		wid->setParent(this->ptr_data_tab_dam);
		//set head spin box
		int number_raster=0;
		number_raster=Dam_Pys_Raster::select_relevant_raster_database(&raster,&this->database, false);
		wid->set_head_spinBox_range(number_raster,1);
		int current_number=0;
		current_number=calculate_item_number(item);
		wid->set_head_spinBox_value(current_number+1);
		wid->set_treeItem(item);
		QObject::connect(wid, SIGNAL(send_change_widget(int , QTreeWidgetItem * , int , int )), this, SLOT(change_widget_by_spinbox(int , QTreeWidgetItem *, int , int )));

		//set the data
		try{
			wid->set_member(&this->database, raster_nr, _dam_raster_types::pys_pys_total);
		}
		catch(Error msg){
			throw msg;
		}
		dam_lay->addWidget(wid);
		wid->set_current_scroll_bar(this->current_ver_slider_pos, this->current_hor_slider_pos);
		wid->show();
		return;
	}
	buffer.str("");

	//pys category
	buffer << str_mod<<sys_data_tree_id::dam_pys_id<<"/"<<sys_data_tree_id::dam_category_id<<"/";
	pos=given.find(buffer.str());
	if(pos>=0){
		raster_nr=item->data(0, Qt::UserRole).toInt();
		DamGui_Pys_Categories_Wid *wid;
		wid=new DamGui_Pys_Categories_Wid;
		wid->setParent(this->ptr_data_tab_dam);
		//set head spin box
		int number_raster=0;
		number_raster=Dam_Pys_Category::select_relevant_category_database(&function,false);
		wid->set_head_spinBox_range(number_raster,1);
		int current_number=0;
		current_number=calculate_item_number(item);
		wid->set_head_spinBox_value(current_number+1);
		wid->set_treeItem(item);
		QObject::connect(wid, SIGNAL(send_change_widget(int , QTreeWidgetItem * , int , int )), this, SLOT(change_widget_by_spinbox(int , QTreeWidgetItem *, int , int )));

		//set the data
		try{
			wid->set_member(&this->database, raster_nr);
		}
		catch(Error msg){
			throw msg;
		}
		dam_lay->addWidget(wid);
		wid->set_current_scroll_bar(this->current_ver_slider_pos, this->current_hor_slider_pos);
		wid->show();
		return;
	}
	buffer.str("");

	//sc point
	buffer << str_mod<<sys_data_tree_id::dam_sc_id<<"/"<<sys_data_tree_id::dam_point_id<<"/";
	pos=given.find(buffer.str());
	if(pos>=0){
		DamGui_Sc_Point_Wid *wid;
		wid=new DamGui_Sc_Point_Wid;
		wid->setParent(this->ptr_data_tab_dam);
		wid->set_treeItem(item);
		//set the data
		try{
			wid->set_up_widget(&this->database, *this->system_id);
		}
		catch(Error msg){
			throw msg;
		}
		dam_lay->addWidget(wid);
		wid->show();
		return;
	}
	buffer.str("");

	//sc subcategory
	buffer << str_mod<<sys_data_tree_id::dam_sc_id<<"/"<<sys_data_tree_id::dam_subcategory_id<<"/";
	pos=given.find(buffer.str());
	if(pos>=0){
		DamGui_Sc_Subcategory_Wid *wid;
		wid=new DamGui_Sc_Subcategory_Wid;
		wid->setParent(this->ptr_data_tab_dam);
		wid->set_treeItem(item);

		//set the data
		try{
			wid->set_up_widget(&this->database, *this->system_id);
		}
		catch(Error msg){
			throw msg;
		}
		dam_lay->addWidget(wid);
		wid->show();
		return;
	}
	buffer.str("");


	//ci point 
	buffer << str_mod << sys_data_tree_id::dam_ci_id << "/" << sys_data_tree_id::dam_point_id << "/";
	pos = given.find(buffer.str());
	if (pos >= 0) {
		DamGui_Ci_Point_Wid *wid;
		wid = new DamGui_Ci_Point_Wid;
		wid->setParent(this->ptr_data_tab_dam);
		wid->set_treeItem(item);

		//set the data
		try {
			wid->set_up_widget(&this->database, *this->system_id);
		}
		catch (Error msg) {
			throw msg;
		}
		dam_lay->addWidget(wid);
		wid->show();
		return;
	}
	buffer.str("");

	//ci polygon 
	buffer << str_mod << sys_data_tree_id::dam_ci_id << "/" << sys_data_tree_id::dam_polygon_id << "/";
	pos = given.find(buffer.str());
	if (pos >= 0) {
		DamGui_Ci_Polygon_Wid *wid;
		wid = new DamGui_Ci_Polygon_Wid;
		wid->setParent(this->ptr_data_tab_dam);
		wid->set_treeItem(item);

		//set the data
		try {
			wid->set_up_widget(&this->database, *this->system_id);
		}
		catch (Error msg) {
			throw msg;
		}
		dam_lay->addWidget(wid);
		wid->show();
		return;
	}
	buffer.str("");

	//results
	//summary results nobreak risk approach
	buffer << str_mod<<sys_data_tree_id::id_results<<"/"<< risk_label::risk_nobreak<<"/"<<sys_data_tree_id::id_summary_results<<"/";
	check=buffer.str();
	if(check==given){
		//set widget
		DamGui_Result_Summary_Wid *wid;
		wid=new DamGui_Result_Summary_Wid;
		wid->setParent(this->ptr_data_tab_risk);
		wid->set_header_tooltip(item->data(0,Qt::AccessibleDescriptionRole).toString().toStdString());
		wid->set_risk_approach_id("CA");
		try{
			wid->set_up_widget(&this->database, *this->system_id);
		}
		catch(Error msg){
			throw msg;
		}

		dam_lay->addWidget(wid);
		wid->show();
		return;
	}
	buffer.str("");

	//summary results scenario based risk approach
	buffer << str_mod<<sys_data_tree_id::id_results<<"/"<< risk_label::risk_scenario<<"/"<<sys_data_tree_id::id_summary_results<<"/";
	check=buffer.str();
	if(check==given){
		//set widget
		DamGui_Result_Summary_Wid *wid;
		wid=new DamGui_Result_Summary_Wid;
		wid->setParent(this->ptr_data_tab_risk);
		wid->set_header_tooltip(item->data(0,Qt::AccessibleDescriptionRole).toString().toStdString());
		wid->set_risk_approach_id("SC%");
		try{
			wid->set_up_widget(&this->database, *this->system_id);
		}
		catch(Error msg){
			throw msg;
		}

		dam_lay->addWidget(wid);
		wid->show();
		return;
	}
	buffer.str("");
	//summary results catchment based risk approach
	buffer << str_mod<<sys_data_tree_id::id_results<<"/"<< risk_label::risk_catchment<<"/"<<sys_data_tree_id::id_summary_results<<"/";
	check=buffer.str();
	if(check==given){
		//set widget
		DamGui_Result_Summary_Wid *wid;
		wid=new DamGui_Result_Summary_Wid;
		wid->setParent(this->ptr_data_tab_risk);
		wid->set_header_tooltip(item->data(0,Qt::AccessibleDescriptionRole).toString().toStdString());
		wid->set_risk_approach_id("CA%");
		try{
			wid->set_up_widget(&this->database, *this->system_id);
		}
		catch(Error msg){
			throw msg;
		}

		dam_lay->addWidget(wid);
		wid->show();
		return;
	}
	buffer.str("");

	//damage results (nobreak)
	buffer << str_mod<<sys_data_tree_id::id_results<<"/"<< risk_label::risk_nobreak<<"/";
	pos=given.find(buffer.str());
	if(pos>=0){
		int glob_id=-1;
		glob_id=item->data(0, Qt::UserRole).toInt();

		//set widget
		DamGui_Result_Wid *wid;
		wid=new DamGui_Result_Wid;
		wid->setParent(this->ptr_data_tab_risk);
		wid->set_header_tooltip(item->data(0,Qt::AccessibleDescriptionRole).toString().toStdString(), item);
		//set the data
		wid->set_up_widget(&this->database, glob_id);

		dam_lay->addWidget(wid);
		wid->show();
		return;
	}
	buffer.str("");

	//damage results (scenario)
	buffer << str_mod<<sys_data_tree_id::id_results<<"/"<< risk_label::risk_scenario<<"/";
	pos=given.find(buffer.str());
	if(pos>=0){
		int glob_id=-1;
		glob_id=item->data(0, Qt::UserRole).toInt();

		//set widget
		DamGui_Result_Wid *wid;
		wid=new DamGui_Result_Wid;
		wid->setParent(this->ptr_data_tab_risk);
		wid->set_header_tooltip(item->data(0,Qt::AccessibleDescriptionRole).toString().toStdString(), item);
		//set the data
		wid->set_up_widget(&this->database, glob_id);

		dam_lay->addWidget(wid);
		wid->show();
		return;
	}
	buffer.str("");

	//damage results (catchment)
	buffer << str_mod<<sys_data_tree_id::id_results<<"/"<< risk_label::risk_catchment<<"/";
	pos=given.find(buffer.str());
	if(pos>=0){
		int glob_id=-1;
		glob_id=item->data(0, Qt::UserRole).toInt();

		//set widget
		DamGui_Result_Wid *wid;
		wid=new DamGui_Result_Wid;
		wid->setParent(this->ptr_data_tab_risk);
		wid->set_header_tooltip(item->data(0,Qt::AccessibleDescriptionRole).toString().toStdString(), item);
		//set the data
		wid->set_up_widget(&this->database, glob_id);

		dam_lay->addWidget(wid);
		wid->show();
		return;
	}
	buffer.str("");
}
//Set widget to the fpl view
void Sys_Data_Tree_Wid::widget2fpl(QTreeWidgetItem *item){
	this->delete_fpl_lay();
	QVBoxLayout *fpl_lay;
	fpl_lay=new QVBoxLayout(this->ptr_data_tab_fpl);

	string str_mod;
	string given=item->data(0,Qt::AccessibleDescriptionRole).toString().toStdString();
	string check;
	int pos=0;
	ostringstream buffer;
	buffer<<"/"<<sys_label::str_fpl<<"/";

	str_mod=buffer.str();
	buffer.str("");

	//fpl system
	buffer << str_mod<<sys_data_tree_id::id_config<<"/";
	check=buffer.str();
	buffer.str("");
	if(check==given){
		//set widget (replace this!)
		FplGui_System_Member_Wid *wid;
		wid=new FplGui_System_Member_Wid;
		wid->setParent(this->ptr_data_tab_fpl);
		//set the data
		wid->set_head_spinBox_range(1,1);
		try{
			wid->set_member(&this->database);
		}
		catch(Error msg){
			throw msg;
		}

		fpl_lay->addWidget(wid);
		wid->show();
		return;
	}
	////fpl fault tree (highground) not requiered
	//buffer << str_mod<<sys_data_tree_id::fpl_fault_tree<<"/"<<fpl_label::sec_highground<<"/";
	//check=buffer.str();
	//buffer.str("");
	//if(check==given){
	//	//set widget (replace this!)
	//	FplGui_HighGround_FaultTree_Wid *wid;
	//	wid=new FplGui_HighGround_FaultTree_Wid;
	//	wid->setParent(this->ptr_data_tab_fpl);
	//	////set the data
	//	//ostringstream txt;
	//	//txt <<"Faultree highground";
	//	//wid->setText(txt.str().c_str());

	//	fpl_lay->addWidget(wid);
	//	wid->show();
	//	return;
	//}
	//fpl fault tree (dike)
	buffer << str_mod<<sys_data_tree_id::fpl_fault_tree<<"/"<<fpl_label::sec_dike<<"/";
	check=buffer.str();
	buffer.str("");
	if(check==given){
		//set widget
		FplGui_Faulttree_Dike_Wid *wid;
		wid=new FplGui_Faulttree_Dike_Wid;
		wid->setParent(this->ptr_data_tab_fpl);
		wid->set_head_spinBox_range(1,1);
		//set the data
		wid->set_member(&this->database);

		fpl_lay->addWidget(wid);
		wid->show();
		return;
	}
	//fpl fault tree (dune)
	buffer << str_mod<<sys_data_tree_id::fpl_fault_tree<<"/"<<fpl_label::sec_dune<<"/";
	check=buffer.str();
	buffer.str("");
	if(check==given){
		//set widget
		FplGui_Faulttree_Dune_Wid *wid;
		wid=new FplGui_Faulttree_Dune_Wid;
		wid->setParent(this->ptr_data_tab_fpl);
		wid->set_head_spinBox_range(1,1);
		//set the data
		wid->set_member(&this->database);

		fpl_lay->addWidget(wid);
		wid->show();
		return;
		
	}
	//fpl fault tree (gate)
	buffer << str_mod<<sys_data_tree_id::fpl_fault_tree<<"/"<<fpl_label::sec_gate<<"/";
	check=buffer.str();
	buffer.str("");
	if(check==given){
		//set widget
		FplGui_Faulttree_Gate_Wid *wid;
		wid=new FplGui_Faulttree_Gate_Wid;
		wid->setParent(this->ptr_data_tab_fpl);
		wid->set_head_spinBox_range(1,1);
		//set the data
		wid->set_member(&this->database);

		fpl_lay->addWidget(wid);
		wid->show();
		return;
	}
	//fpl fault tree (wall)
	buffer << str_mod<<sys_data_tree_id::fpl_fault_tree<<"/"<<fpl_label::sec_wall<<"/";
	check=buffer.str();
	buffer.str("");
	if(check==given){
		//set widget (replace this!)
		QLabel *wid;
		wid=new QLabel;
		wid->setParent(this->ptr_data_tab_fpl);
		//set the data
		ostringstream txt;
		txt <<"Faultree wall (not yet available)";
		wid->setText(txt.str().c_str());

		fpl_lay->addWidget(wid);
		wid->show();
		return;
	}
	//fpl fault tree (mobile wall)
	buffer << str_mod<<sys_data_tree_id::fpl_fault_tree<<"/"<<fpl_label::sec_mobile_wall<<"/";
	check=buffer.str();
	buffer.str("");
	if(check==given){
		//set widget (replace this!)
		QLabel *wid;
		wid=new QLabel;
		wid->setParent(this->ptr_data_tab_fpl);
		//set the data
		ostringstream txt;
		txt <<"Faultree mobile wall (not yet available)";
		wid->setText(txt.str().c_str());

		fpl_lay->addWidget(wid);
		wid->show();
		return;
	}

	//fpl section data
	buffer << str_mod<<sys_data_tree_id::fpl_section_id<<"/";
	pos=given.find(buffer.str());
	if(pos>=0){
		int sec_id=-1;
		sec_id=item->data(0, Qt::UserRole).toInt();

		//set widget (replace this!)
		QLabel *wid;
		wid=new QLabel;
		wid->setParent(this->ptr_data_tab_fpl);
		//set the data
		ostringstream txt;
		txt <<"Section data of"<< sec_id;
		wid->setText(txt.str().c_str());

		fpl_lay->addWidget(wid);
		wid->show();
		return;
	}
	buffer.str("");

	//fpl section results
	buffer << str_mod<<sys_data_tree_id::id_results<<"/";
	pos=given.find(buffer.str());
	if(pos>=0){
		int sec_id=-1;
		sec_id=item->data(0, Qt::UserRole).toInt();

		//set widget
		FplGui_Prob_Result_Wid *wid;
		wid=new FplGui_Prob_Result_Wid;
		wid->setParent(this->ptr_data_tab_fpl);
		wid->set_header_tooltip(item->data(0,Qt::AccessibleDescriptionRole).toString().toStdString(), item);
		//set the data
		try{
			wid->set_member(&this->database, sec_id, *this->system_id);
		}
		catch(Error msg){
			throw msg;
		}

		fpl_lay->addWidget(wid);
		wid->show();
		return;
	}
	buffer.str("");

	//fpl section deaitled results
	buffer << str_mod<<sys_data_tree_id::id_detailed_results<<"/";
	pos=given.find(buffer.str());
	if(pos>=0){
		int sec_id=-1;
		sec_id=item->data(0, Qt::UserRole).toInt();

        FplGui_Detailed_Result_Wid *wid;
        wid=new FplGui_Detailed_Result_Wid;
        wid->setParent(this->ptr_data_tab_fpl);
        wid->set_header_tooltip(item->data(0,Qt::AccessibleDescriptionRole).toString().toStdString(), item);
        //set the data
        try{
            wid->set_member(&this->database, sec_id, *this->system_id);
        }
        catch(Error msg){
            throw msg;
        }

		fpl_lay->addWidget(wid);
		wid->show();
		return;
	}
	buffer.str("");
}
//Set widget to the madm view
void Sys_Data_Tree_Wid::widget2madm(QTreeWidgetItem *item){
	this->delete_madm_lay();
	QVBoxLayout *madm_lay;
	madm_lay=new QVBoxLayout(this->ptr_data_tab_madm);
	int pos=0;
	string str_mod;
	string given=item->data(0,Qt::AccessibleDescriptionRole).toString().toStdString();
	string check;

	ostringstream buffer;
	buffer<<"/"<<sys_label::str_madm<<"/";

	str_mod=buffer.str();
	buffer.str("");

	//madm weight sets
	buffer << str_mod<<sys_data_tree_id::madm_weights<<"/";
	check=buffer.str();
	buffer.str("");
	if(check==given){
		//set widget
		MadmGui_Weight_Sets_Wid *wid;
		wid=new MadmGui_Weight_Sets_Wid;
		wid->setParent(this->ptr_data_tab_madm);
		//set the data
		try{
			wid->set_up_widget(&this->database);
		}
		catch(Error msg){
			throw msg;
		}

		madm_lay->addWidget(wid);
		wid->show();
		return;
	}
	buffer.str("");

	//madm matrix nobreak risk approach
	buffer << str_mod<<sys_data_tree_id::id_results<<"/"<<risk_label::risk_nobreak<<"/"<<sys_data_tree_id::madm_matrix<<"/";
	pos=given.find(buffer.str());
	if(pos>=0){
		//set widget
		MadmGui_Matrix_Summary_Wid *wid;
		wid=new MadmGui_Matrix_Summary_Wid;
		wid->setParent(this->ptr_data_tab_madm);
		wid->set_risk_type(_risk_type::nobreak_risk);
		wid->set_header_tooltip(item->data(0,Qt::AccessibleDescriptionRole).toString().toStdString());
		//set the data
		try{
			wid->set_up_widget(&this->database, *this->system_id);
		}
		catch(Error msg){
			throw msg;
		}

		madm_lay->addWidget(wid);
		wid->show();
		return;
	}
	buffer.str("");

	//madm result nobreak risk approach
	buffer << str_mod<<sys_data_tree_id::id_results<<"/"<<risk_label::risk_nobreak<<"/";
	pos=given.find(buffer.str());
	if(pos>=0){
		int set_id=-1;
		set_id=item->data(0, Qt::UserRole).toInt();

		//set widget
		MadmGui_Total_Rank_Wid *wid;
		wid=new MadmGui_Total_Rank_Wid;
		wid->setParent(this->ptr_data_tab_madm);
		wid->set_header_tooltip(item->data(0,Qt::AccessibleDescriptionRole).toString().toStdString(), item);
		//set the data
		try{
			wid->set_up_widget(&this->database, this->system_id->area_state, set_id, _risk_type::nobreak_risk);
		}
		catch(Error msg){
			throw msg;
		}

		madm_lay->addWidget(wid);
		wid->show();
		return;
	}
	buffer.str("");

	//madm matrix catchment risk approach
	buffer << str_mod<<sys_data_tree_id::id_results<<"/"<<risk_label::risk_catchment<<"/"<<sys_data_tree_id::madm_matrix<<"/";
	pos=given.find(buffer.str());
	if(pos>=0){
		//set widget
		MadmGui_Matrix_Summary_Wid *wid;
		wid=new MadmGui_Matrix_Summary_Wid;
		wid->setParent(this->ptr_data_tab_madm);
		wid->set_risk_type(_risk_type::catchment_risk);
		wid->set_header_tooltip(item->data(0,Qt::AccessibleDescriptionRole).toString().toStdString());
		//set the data
		try{
			wid->set_up_widget(&this->database, *this->system_id);
		}
		catch(Error msg){
			throw msg;
		}

		madm_lay->addWidget(wid);
		wid->show();
		return;
	}
	buffer.str("");
	//madm result catchment risk approach
	buffer << str_mod<<sys_data_tree_id::id_results<<"/"<<risk_label::risk_catchment<<"/";
	pos=given.find(buffer.str());
	if(pos>=0){
		int set_id=-1;
		set_id=item->data(0, Qt::UserRole).toInt();

		//set widget
		MadmGui_Total_Rank_Wid *wid;
		wid=new MadmGui_Total_Rank_Wid;
		wid->setParent(this->ptr_data_tab_madm);
		wid->set_header_tooltip(item->data(0,Qt::AccessibleDescriptionRole).toString().toStdString(), item);
		//set the data
		try{
			wid->set_up_widget(&this->database, this->system_id->area_state, set_id, _risk_type::catchment_risk);
		}
		catch(Error msg){
			throw msg;
		}

		madm_lay->addWidget(wid);
		wid->show();
		return;
	}
}
//Set widget to the risk view
void Sys_Data_Tree_Wid::widget2risk(QTreeWidgetItem *item){
	this->delete_risk_lay();
	QVBoxLayout *risk_lay;
	risk_lay=new QVBoxLayout(this->ptr_data_tab_risk);

	string str_mod;
	string given=item->data(0,Qt::AccessibleDescriptionRole).toString().toStdString();
	string check;
	int pos=0;
	ostringstream buffer;
	buffer<<"/"<<sys_label::str_risk<<"/";

	str_mod=buffer.str();
	buffer.str("");

	//risk system
	buffer << str_mod<<sys_data_tree_id::id_config<<"/";
	check=buffer.str();
	if(check==given){
		//set widget (replace this!)
		RiskGui_System_Member_Wid *wid;
		wid=new RiskGui_System_Member_Wid;
		wid->setParent(this->ptr_data_tab_risk);
		if(this->system_id->measure_nr>0){
			wid->set_edit_action_disabled(true);
		}
		wid->set_member(&this->database);

		risk_lay->addWidget(wid);
		wid->show();
		return;
	}
	buffer.str("");

	//risk detailed results (nobreak) summary
	buffer << risk_label::risk_nobreak<<"/"<< sys_data_tree_id::id_detailed_results<<"/"<<sys_data_tree_id::id_summary_results<<"/";
	pos=given.find(buffer.str());
	if(pos>=0){
		//set widget
		RiskGui_Detailed_Risk_Summary_Wid *wid;
		wid=new RiskGui_Detailed_Risk_Summary_Wid;
		wid->setParent(this->ptr_data_tab_risk);
		wid->set_header_tooltip(item->data(0,Qt::AccessibleDescriptionRole).toString().toStdString());
		wid->set_risk_type(_risk_type::nobreak_risk);
		//set the data
		wid->set_up_widget(&this->database, *this->system_id);

		risk_lay->addWidget(wid);
		wid->show();
		return;
	}
	buffer.str("");

	//risk detailed results (nobreak)
	buffer << risk_label::risk_nobreak<<"/"<< sys_data_tree_id::id_detailed_results<<"/";
	pos=given.find(buffer.str());
	if(pos>=0){
		int glob_id=-1;
		glob_id=item->data(0, Qt::UserRole).toInt();

		//set widget
		RiskGui_Detailed_Result_Wid *wid;
		wid=new RiskGui_Detailed_Result_Wid;
		wid->setParent(this->ptr_data_tab_risk);
		wid->set_header_tooltip(item->data(0,Qt::AccessibleDescriptionRole).toString().toStdString(), item);
		//set the data
		wid->set_up_widget(&this->database, glob_id, *this->system_id, _risk_type::nobreak_risk);

		risk_lay->addWidget(wid);
		wid->show();
		return;
	}
	buffer.str("");

	//risk cumulated results (nobreak)
	buffer << risk_label::risk_nobreak<<"/"<< sys_data_tree_id::risk_cumulated_results<<"/";
	pos=given.find(buffer.str());
	if(pos>=0){
		//set widget
		RiskGui_Result_Cumulated_Wid *wid;
		wid=new RiskGui_Result_Cumulated_Wid;
		wid->setParent(this->ptr_data_tab_risk);
		wid->set_header_tooltip(item->data(0,Qt::AccessibleDescriptionRole).toString().toStdString(), item);
		//set the data
		try{
			wid->set_member(&this->database,*this->system_id,_risk_type::nobreak_risk);
		}
		catch(Error msg){
			throw msg;
		}

		risk_lay->addWidget(wid);
		wid->show();
		return;
	}
	buffer.str("");

	//risk total results (nobreak)
	buffer << risk_label::risk_nobreak<<"/"<< sys_data_tree_id::risk_total_results<<"/";
	pos=given.find(buffer.str());
	if(pos>=0){
		int glob_id=-1;
		glob_id=item->data(0, Qt::UserRole).toInt();

		//set widget
		RiskGui_Result_Wid *wid;
		wid=new RiskGui_Result_Wid;
		wid->setParent(this->ptr_data_tab_risk);
		wid->set_header_tooltip(item->data(0,Qt::AccessibleDescriptionRole).toString().toStdString(), item);
		//set the data
		wid->set_up_widget(&this->database, glob_id);

		risk_lay->addWidget(wid);
		wid->show();
		return;
	}
	buffer.str("");

	//risk detailed results (scenario) summary
	buffer << risk_label::risk_scenario<<"/"<< sys_data_tree_id::id_detailed_results<<"/"<<sys_data_tree_id::id_summary_results<<"/";
	pos=given.find(buffer.str());
	if(pos>=0){
		//set widget
		RiskGui_Detailed_Risk_Summary_Wid *wid;
		wid=new RiskGui_Detailed_Risk_Summary_Wid;
		wid->setParent(this->ptr_data_tab_risk);
		wid->set_header_tooltip(item->data(0,Qt::AccessibleDescriptionRole).toString().toStdString());
		wid->set_risk_type(_risk_type::scenario_risk);
		//set the data
		wid->set_up_widget(&this->database, *this->system_id);

		risk_lay->addWidget(wid);
		wid->show();
		return;
	}
	buffer.str("");

	//risk detailed results (scenario)
	buffer << risk_label::risk_scenario<<"/"<< sys_data_tree_id::id_detailed_results<<"/";
	pos=given.find(buffer.str());
	if(pos>=0){
		int glob_id=-1;
		glob_id=item->data(0, Qt::UserRole).toInt();

		//set widget
		RiskGui_Detailed_Result_Wid *wid;
		wid=new RiskGui_Detailed_Result_Wid;
		wid->setParent(this->ptr_data_tab_risk);
		wid->set_header_tooltip(item->data(0,Qt::AccessibleDescriptionRole).toString().toStdString(), item);
		//set the data
		wid->set_up_widget(&this->database, glob_id, *this->system_id, _risk_type::scenario_risk);

		risk_lay->addWidget(wid);
		wid->show();
		return;
	}
	buffer.str("");

	//generated scenarios (catchment)
	buffer << risk_label::risk_catchment<<"/"<< sys_data_tree_id::risk_scenarios<<"/";
	pos=given.find(buffer.str());
	if(pos>=0){
		//set widget
		RiskGui_Result_Scenario_Wid *wid;
		wid=new RiskGui_Result_Scenario_Wid;
		wid->setParent(this->ptr_data_tab_risk);
		wid->set_header_tooltip(item->data(0,Qt::AccessibleDescriptionRole).toString().toStdString());
		//set the data
		wid->set_up_widget(&this->database, *this->system_id);

		risk_lay->addWidget(wid);
		wid->show();
		return;
	}
	buffer.str("");

	//generated statistics (catchment)
	buffer << risk_label::risk_catchment<<"/"<< sys_data_tree_id::risk_ca_statistics<<"/";
	pos=given.find(buffer.str());
	if(pos>=0){
		//set widget
		RiskGui_Catchment_Scenario_Statistic_Wid *wid;
		wid=new RiskGui_Catchment_Scenario_Statistic_Wid;
		wid->setParent(this->ptr_data_tab_risk);
		wid->set_header_tooltip(item->data(0,Qt::AccessibleDescriptionRole).toString().toStdString());
		//set the data
		wid->set_up_widget(&this->database, *this->system_id);

		risk_lay->addWidget(wid);
		wid->show();
		return;
	}
	buffer.str("");

	//generated probabilities for each fpl-section
	buffer << risk_label::risk_catchment<<"/"<< sys_data_tree_id::risk_prob_section<<"/";
	pos=given.find(buffer.str());
	if(pos>=0){
		//set widget
		RiskGui_Probabilty_Fpl_Section_Wid *wid;
		wid=new RiskGui_Probabilty_Fpl_Section_Wid;
		wid->setParent(this->ptr_data_tab_risk);
		wid->set_header_tooltip(item->data(0,Qt::AccessibleDescriptionRole).toString().toStdString());
		//set the data
		wid->set_up_widget(&this->database, *this->system_id);

		risk_lay->addWidget(wid);
		wid->show();
		return;
	}
	buffer.str("");

	//risk detailed results (catchment) summary
	buffer << risk_label::risk_catchment<<"/"<< sys_data_tree_id::id_detailed_results<<"/"<<sys_data_tree_id::id_summary_results<<"/";
	pos=given.find(buffer.str());
	if(pos>=0){
		//set widget
		RiskGui_Detailed_Risk_Summary_Wid *wid;
		wid=new RiskGui_Detailed_Risk_Summary_Wid;
		wid->setParent(this->ptr_data_tab_risk);
		wid->set_header_tooltip(item->data(0,Qt::AccessibleDescriptionRole).toString().toStdString());
		wid->set_risk_type(_risk_type::catchment_risk);
		//set the data
		wid->set_up_widget(&this->database, *this->system_id);

		risk_lay->addWidget(wid);
		wid->show();
		return;
	}
	buffer.str("");

	//risk detailed results (catchment)
	buffer << risk_label::risk_catchment<<"/"<< sys_data_tree_id::id_detailed_results<<"/";
	pos=given.find(buffer.str());
	if(pos>=0){
		int glob_id=-1;
		glob_id=item->data(0, Qt::UserRole).toInt();

		//set widget
		RiskGui_Detailed_Result_Wid *wid;
		wid=new RiskGui_Detailed_Result_Wid;
		wid->setParent(this->ptr_data_tab_risk);
		wid->set_header_tooltip(item->data(0,Qt::AccessibleDescriptionRole).toString().toStdString(), item);
		//set the data
		wid->set_up_widget(&this->database, glob_id, *this->system_id, _risk_type::catchment_risk);

		risk_lay->addWidget(wid);
		wid->show();
		return;
	}
	buffer.str("");

	//risk cumulated results (catchment)
	buffer << risk_label::risk_catchment<<"/"<< sys_data_tree_id::risk_cumulated_results<<"/";
	pos=given.find(buffer.str());
	if(pos>=0){
		//set widget
		RiskGui_Result_Cumulated_Wid *wid;
		wid=new RiskGui_Result_Cumulated_Wid;
		wid->setParent(this->ptr_data_tab_risk);
		wid->set_header_tooltip(item->data(0,Qt::AccessibleDescriptionRole).toString().toStdString(), item);
		//set the data
		try{
			wid->set_member(&this->database,*this->system_id,_risk_type::catchment_risk);
		}
		catch(Error msg){
			throw msg;
		}

		risk_lay->addWidget(wid);
		wid->show();
		return;
	}
	buffer.str("");

	//risk total results (catchment)
	buffer << risk_label::risk_catchment<<"/"<< sys_data_tree_id::risk_total_results<<"/";
	pos=given.find(buffer.str());
	if(pos>=0){
		int glob_id=-1;
		glob_id=item->data(0, Qt::UserRole).toInt();

		//set widget
		RiskGui_Result_Wid *wid;
		wid=new RiskGui_Result_Wid;
		wid->setParent(this->ptr_data_tab_risk);
		wid->set_header_tooltip(item->data(0,Qt::AccessibleDescriptionRole).toString().toStdString(), item);
		//set the data
		wid->set_up_widget(&this->database, glob_id);
		risk_lay->addWidget(wid);
		wid->show();
		return;
	}
	buffer.str("");
}
//Set widget to the alt view
void Sys_Data_Tree_Wid::widget2alt(QTreeWidgetItem *item){
	this->delete_alt_lay();
	QVBoxLayout *alt_lay;
	alt_lay=new QVBoxLayout(this->ptr_data_tab_alt);

	string str_mod;
	string given=item->data(0,Qt::AccessibleDescriptionRole).toString().toStdString();
	string check;
	int pos=0;
	ostringstream buffer;
	buffer<<"/"<<sys_label::str_alt<<"/";

	//alt measures
	pos=given.find(buffer.str());
	if(pos>=0){
		int id=-1;
		id=item->data(0, Qt::UserRole).toInt();

		//set widget
		AltGui_Alternative_Info_Wid *wid;
		wid=new AltGui_Alternative_Info_Wid;
		wid->setParent(this->ptr_data_tab_alt);
		wid->set_identifier(item->data(0,Qt::AccessibleDescriptionRole).toString().toStdString(), item);
		//set head spin box
		int number_alt=0;
		QStringList bufflist;
		Alt_System::load_stringlist_current_alt(&this->database, this->system_id->area_state, &bufflist);
		number_alt=bufflist.count();
		wid->set_head_spinBox_range(number_alt,1);
		int current_number=0;
		current_number=calculate_item_number(item);
		wid->set_head_spinBox_value(current_number+1);
		wid->set_treeItem(item);
		QObject::connect(wid, SIGNAL(send_change_widget(int , QTreeWidgetItem * , int , int )), this, SLOT(change_widget_by_spinbox(int , QTreeWidgetItem * , int , int )));

		//set the data
		wid->set_member(&this->database, id, this->system_id->area_state);

		alt_lay->addWidget(wid);
		wid->set_current_scroll_bar(this->current_ver_slider_pos, this->current_hor_slider_pos);
		wid->show();
		return;
	}
	buffer.str("");
}
//Set widget to the cost view
void Sys_Data_Tree_Wid::widget2cost(QTreeWidgetItem *){
	this->delete_cost_lay();
	QVBoxLayout *cost_lay;
	cost_lay=new QVBoxLayout(this->ptr_data_tab_cost);
}
//Clone the database and open ist
void Sys_Data_Tree_Wid::clone_database(QSqlDatabase *ptr_database){
	this->database=QSqlDatabase::cloneDatabase(*ptr_database, sys_label::str_sys_data_tree.c_str());
	this->database.open();
}
//Clear the tree view
void Sys_Data_Tree_Wid::clear_total(void){
	//clear tree widget
	this->data_tree->clear();
	this->previous_db_clicked=NULL;
	this->active_item=NULL;

	//delete the layouts
	this->delete_dam_lay();
	this->delete_fpl_lay();
	this->delete_hyd_lay();
	this->delete_madm_lay();
	this->delete_risk_lay();

	if(this->database.isOpen()==true){
		this->database.close();
	}
	if(QSqlDatabase::contains(sys_label::str_sys_data_tree.c_str())==true){
		if(QSqlDatabase::database(sys_label::str_sys_data_tree.c_str(),false).isOpen()==true){
			QSqlDatabase::database(sys_label::str_sys_data_tree.c_str(),false).close();
		}
		QSqlDatabase::removeDatabase(sys_label::str_sys_data_tree.c_str());
	}
}
//Find the module type out of the identifier of the item
bool Sys_Data_Tree_Wid::find_module_type(string id){
	bool just_modul=false;
	this->current_type=_sys_system_modules::SYS_SYS;
	int pos;

	ostringstream to_find;
	//hyd
	to_find <<"/"<< sys_label::str_hyd<<"/";
	pos=id.find(to_find.str());
	if(pos>=0){
		this->current_type=_sys_system_modules::HYD_SYS;
		if(to_find.str().length()==id.length()){
			just_modul=true;
			return just_modul;
		}
		else{
			just_modul=false;
			return just_modul;
		}
	}
	to_find.str("");
	//dam
	to_find <<"/"<< sys_label::str_dam<<"/";
	pos=id.find(to_find.str());
	if(pos>=0){
		this->current_type=_sys_system_modules::DAM_SYS;
		if(to_find.str().length()==id.length()){
			just_modul=true;
			return just_modul;
		}
		else{
			just_modul=false;
			return just_modul;
		}
	}

	to_find.str("");
	//fpl
	to_find <<"/"<< sys_label::str_fpl<<"/";
	pos=id.find(to_find.str());
	if(pos>=0){
		this->current_type=_sys_system_modules::FPL_SYS;
		if(to_find.str().length()==id.length()){
			just_modul=true;
			return just_modul;
		}
		else{
			just_modul=false;
			return just_modul;
		}
	}

	to_find.str("");
	//madm
	to_find <<"/"<< sys_label::str_madm<<"/";
	pos=id.find(to_find.str());
	if(pos>=0){
		this->current_type=_sys_system_modules::MADM_SYS;
		if(to_find.str().length()==id.length()){
			just_modul=true;
			return just_modul;
		}
		else{
			just_modul=false;
			return just_modul;
		}
	}
	to_find.str("");
	//risk
	to_find <<"/"<< sys_label::str_risk<<"/";
	pos=id.find(to_find.str());
	if(pos>=0){
		this->current_type=_sys_system_modules::RISK_SYS;
		if(to_find.str().length()==id.length()){
			just_modul=true;
			return just_modul;
		}
		else{
			just_modul=false;
			return just_modul;
		}
	}
	to_find.str("");
	//alt
	to_find <<"/"<< sys_label::str_alt<<"/";
	pos=id.find(to_find.str());
	if(pos>=0){
		this->current_type=_sys_system_modules::ALT_SYS;
		if(to_find.str().length()==id.length()){
			just_modul=true;
			return just_modul;
		}
		else{
			just_modul=false;
			return just_modul;
		}
	}
	to_find.str("");
	//cost
	to_find <<"/"<< sys_label::str_cost<<"/";
	pos=id.find(to_find.str());
	if(pos>=0){
		this->current_type=_sys_system_modules::COST_SYS;
		if(to_find.str().length()==id.length()){
			just_modul=true;
			return just_modul;
		}
		else{
			just_modul=false;
			return just_modul;
		}
	}
	to_find.str("");

	return just_modul;
}
//Start the tree search for the modules
QTreeWidgetItem* Sys_Data_Tree_Wid::start_module_tree_search(const string search_txt, _sys_system_modules module){
	QTreeWidgetItem *found;
	found=NULL;
	QTreeWidgetItem *search;
	search=NULL;
	search=this->data_tree->topLevelItem(0);
	string test=search->text(0).toStdString();
	if(search==NULL){
		return found;
	}
	//search for the module branch
	int number=search->childCount();
	string modul_txt=functions::convert_system_module2txt(module);
	for(int i=0; i< number; i++){
		if(modul_txt==search->child(i)->text(0).toStdString()){
			if(search->child(i)->data(0, Qt::AccessibleDescriptionRole).toString().toStdString()==search_txt){
				found=search->child(i);
				return found;
			}
			else{
				search=search->child(i);
				found=this->item_search(search, search_txt);
				//for development
				//if(found != NULL){
				//	ostringstream cout;
				//	cout << " Search " << search_txt << endl;
				//	cout << " Found " << found->data(0, Qt::AccessibleDescriptionRole).toString().toStdString() << endl;
				//	Sys_Common_Output::output_system->output_txt(&cout);
				//}
				//else{
				//	ostringstream cout;
				//	cout << " Nothing found" << endl;
				//	Sys_Common_Output::output_system->output_txt(&cout);
				//}
				return found;
			}
		}
	}

	return found;
}
//Search for an specific item
QTreeWidgetItem *Sys_Data_Tree_Wid::item_search(const QTreeWidgetItem * parent, const string search_txt){
	QTreeWidgetItem *found;
	found=NULL;
	QTreeWidgetItem *childs;
	childs=NULL;

	//start the search
	int number_child=parent->childCount();
	for(int i=0; i< number_child; i++){
		if(parent->child(i)->data(0, Qt::AccessibleDescriptionRole).toString().toStdString()==search_txt){
			found=parent->child(i);
			return found;
		}
		else{
			//recursive search
			found=this->item_search(parent->child(i), search_txt);
			if(found!=NULL){
				return found;
			}
		}
	}
	return found;
}
//Set the doubled clicked item to bold letters
void Sys_Data_Tree_Wid::set_doubledclicked2bold(QTreeWidgetItem * item2bold){
	QFont font;
	if(this->previous_db_clicked!=NULL){
		font=previous_db_clicked->font(0);
		font.setBold(false);
		previous_db_clicked->setFont(0,font);
	}
	if(item2bold!=NULL){
		font=item2bold->font(0);
		font.setBold(true);
		item2bold->setFont(0,font);
	}
}
//Set number of items in the second column of the tree widget
void Sys_Data_Tree_Wid::set_number_items(void){
	QTreeWidgetItem *search;
	search=NULL;
	search=this->data_tree->topLevelItem(0);

	for(int i=0; i< search->childCount(); i++){
		this->item_count(search->child(i), true);
	}
}
//Count the relevant child items
void Sys_Data_Tree_Wid::item_count(QTreeWidgetItem * parent, const bool first){
	QTreeWidgetItem *childs;
	childs=NULL;

	int counter_no=0;
	int pos1;
	int pos2;
	int pos3;
	int pos4;
	//start the search
	int number_child=parent->childCount();
	for(int i=0; i< number_child; i++){
		childs=parent->child(i);
		this->item_count(childs, false);

		pos1=childs->data(0,Qt::AccessibleDescriptionRole).toString().toStdString().find(sys_data_tree_id::id_summary_results);
		pos2= childs->data(0,Qt::AccessibleDescriptionRole).toString().toStdString().find(sys_data_tree_id::madm_matrix);
		pos3= childs->data(0,Qt::AccessibleDescriptionRole).toString().toStdString().find(sys_data_tree_id::dam_sc_id);
		pos4 = childs->data(0, Qt::AccessibleDescriptionRole).toString().toStdString().find(sys_data_tree_id::dam_ci_id);
		if(childs->data(0,Qt::UserRole).toString()!="" && pos1<0 && pos2<0 && pos3<0 && pos4<0){
			counter_no++;
		}
		if(counter_no>0 && first==false){
			parent->setData(1, Qt::DisplayRole,counter_no);
		}
	}
}
//Delete the widget for the hydraulic view
void Sys_Data_Tree_Wid::delete_hyd_lay(void){
	if(this->ptr_data_tab_hyd!=NULL){
		this->ptr_data_tab_hyd->close();
		if(this->ptr_data_tab_hyd->layout()!=NULL){
			delete this->ptr_data_tab_hyd->layout();
			QObjectList list;
			list=this->ptr_data_tab_hyd->children();
			for(int i=0; i< list.count(); i++){
				delete list.at(i);
			}
		}
		this->ptr_data_tab_hyd->show();
	}
}
//Delete the widget for the hydraulic view
void Sys_Data_Tree_Wid::delete_dam_lay(void){
	if(this->ptr_data_tab_dam!=NULL){
		this->ptr_data_tab_dam->close();
		if(this->ptr_data_tab_dam->layout()!=NULL){
			delete this->ptr_data_tab_dam->layout();

			QObjectList list;
			list=this->ptr_data_tab_dam->children();
			for(int i=0; i< list.count(); i++){

                if(list.at(i)!=NULL){
                    delete list.at(i);
                }
			}
		}
		this->ptr_data_tab_dam->show();
	}
}
//Delete the widget for the fpl view
void Sys_Data_Tree_Wid::delete_fpl_lay(void){
	if(this->ptr_data_tab_fpl!=NULL){
		this->ptr_data_tab_fpl->close();
		if(this->ptr_data_tab_fpl->layout()!=NULL){
			delete this->ptr_data_tab_fpl->layout();
			QObjectList list;
			list=this->ptr_data_tab_fpl->children();
			for(int i=0; i< list.count(); i++){
				delete list.at(i);
			}
		}
		this->ptr_data_tab_fpl->show();
	}
}
//Delete the widget for the madm view
void Sys_Data_Tree_Wid::delete_madm_lay(void){
	if(this->ptr_data_tab_madm!=NULL){
		this->ptr_data_tab_madm->close();
		if(this->ptr_data_tab_madm->layout()!=NULL){
			delete this->ptr_data_tab_madm->layout();
			QObjectList list;
			list=this->ptr_data_tab_madm->children();
			for(int i=0; i< list.count(); i++){
				delete list.at(i);
			}
		}
		this->ptr_data_tab_madm->show();
	}
}
//Delete the layout for the risk view
void Sys_Data_Tree_Wid::delete_risk_lay(void){
	if(this->ptr_data_tab_risk!=NULL){
		this->ptr_data_tab_risk->close();
		if(this->ptr_data_tab_risk->layout()!=NULL){
			delete this->ptr_data_tab_risk->layout();
			QObjectList list;
			list=this->ptr_data_tab_risk->children();
			for(int i=0; i< list.count(); i++){
				delete list.at(i);
			}
		}
		this->ptr_data_tab_risk->show();
	}
}
//Delete the layout for the alt view
void Sys_Data_Tree_Wid::delete_alt_lay(void){
	if(this->ptr_data_tab_alt!=NULL){
		this->ptr_data_tab_alt->close();
		if(this->ptr_data_tab_alt->layout()!=NULL){
			delete this->ptr_data_tab_alt->layout();
			QObjectList list;
			list=this->ptr_data_tab_alt->children();
			for(int i=0; i< list.count(); i++){
				delete list.at(i);
			}
		}
		this->ptr_data_tab_alt->show();
	}
}
//Delete the layout for the cost view
void Sys_Data_Tree_Wid::delete_cost_lay(void){
	if(this->ptr_data_tab_cost!=NULL){
		this->ptr_data_tab_cost->close();
		if(this->ptr_data_tab_cost->layout()!=NULL){
			delete this->ptr_data_tab_cost->layout();
			QObjectList list;
			list=this->ptr_data_tab_cost->children();
			for(int i=0; i< list.count(); i++){
				delete list.at(i);
			}
		}
		this->ptr_data_tab_cost->show();
	}
}
//Calculate the number of the item in the branch
int Sys_Data_Tree_Wid::calculate_item_number(QTreeWidgetItem *item){
	int number=-1;
	QTreeWidgetItem *parent;
	parent=item->parent();
	if(parent==NULL){
		return number;
	}
	else{
		number=parent->indexOfChild(item);
	}

	return number;
}
//Set error(s)
Error Sys_Data_Tree_Wid::set_error(const int err_type){
	string place="Sys_Data_Tree_Wid::";
	string help;
	string reason;
	int type=0;
	bool fatal=false;
	stringstream info;
	Error msg;

	switch (err_type){
		case 0://bad alloc
			place.append("allocate_widget_loader(void)");
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
//__________
//private slots
//Show the gis data widget
void Sys_Data_Tree_Wid::show_gis_widget(void){
	if(this->active_item==NULL){
		return;
	}
	//set the focus on gis-widget
	if(this->ptr_data_tab_overall!=NULL){
		this->ptr_data_tab_overall->setCurrentIndex(0);
	}

	ostringstream cout;
	cout << "Item to show in gis " << this->active_item->text(0).toStdString() << endl;
	Sys_Common_Output::output_system->output_txt(&cout);
}
//Collapse all branches a expand the first one
void Sys_Data_Tree_Wid::collapse_all_branch(void){
	this->data_tree->collapseAll();
	this->data_tree->topLevelItem(0)->setExpanded(true);
}
//Set the current active item
void Sys_Data_Tree_Wid::set_current_active_item(QTreeWidgetItem * item, int column){
	if(item->parent()==NULL){
		int buff;//not needed, surpress warnings; just column 0 is used
		buff=column;
		return;
	}

	this->active_item=item;
}
//Change the widget and the marked tree-leaf when the spin box value of an widget is changed
void Sys_Data_Tree_Wid::change_widget_by_spinbox(int number, QTreeWidgetItem *item, const int ver_pos, const int hor_pos){
	QTreeWidgetItem *parent=NULL;
	parent=item->parent();
	QTreeWidgetItem *new_item=NULL;
	if(parent==NULL){
		return;
	}
	new_item=parent->child(number-1);
	if(new_item==NULL){
		return;
	}
	this->current_hor_slider_pos=hor_pos;
	this->current_ver_slider_pos=ver_pos;
	this->tree_item_clicked(new_item, 0);
	this->current_hor_slider_pos=0;
	this->current_ver_slider_pos=0;

	this->data_tree->clearSelection();
	new_item->setSelected(true);
	this->data_tree->scrollToItem(new_item);
}
//Set the data tree widget to enabled or not (send from the widget loader)
void Sys_Data_Tree_Wid::set_enabled(bool flag){
	this->setEnabled(flag);
}
