#include "Sys_Headers_Precompiled.h"
//#include "Sys_Database_Project_Tree_Wid.h"

///Default constructor
Sys_Database_Project_Tree_Wid::Sys_Database_Project_Tree_Wid(QWidget *parent) : QWidget(parent){
	QVBoxLayout *layout = new QVBoxLayout(this);
    tree = new QTreeWidget(this);
    tree->setObjectName(QLatin1String("tree"));
    tree->setHeaderLabels(QStringList(tr("Tables Explorer")));
    tree->header()->setSectionResizeMode(QHeaderView::Stretch);
    QAction *refreshAction = new QAction(tr("Refresh"), tree);
    metaDataAction = new QAction(tr("Show Schema"), tree);
	QObject::connect(refreshAction, SIGNAL(triggered()), SLOT(refresh()));
	QObject::connect(metaDataAction, SIGNAL(triggered()), SLOT(show_metadata()));
    tree->addAction(refreshAction);
    tree->addAction(metaDataAction);
    tree->setContextMenuPolicy(Qt::ActionsContextMenu);

	this->previous_activated=NULL;

    layout->addWidget(tree);

    QMetaObject::connectSlotsByName(this);
	Sys_Memory_Count::self()->add_mem(sizeof(Sys_Database_Project_Tree_Wid), _sys_system_modules::SYS_SYS);
}
///Default destructor
Sys_Database_Project_Tree_Wid::~Sys_Database_Project_Tree_Wid(void){
	Sys_Memory_Count::self()->minus_mem(sizeof(Sys_Database_Project_Tree_Wid), _sys_system_modules::SYS_SYS);
}
//____________
//public
//Clear the tree-view
void Sys_Database_Project_Tree_Wid::clear_project_tree(void){
	this->tree->clear();
}
//____________
//public slots
//Refresh the view
void Sys_Database_Project_Tree_Wid::refresh(void){
	emit send_txt2statusbar("Load database project tree view...", 0);
	this->tree->clear();
	this->previous_activated=NULL;
    QStringList connectionNames = QSqlDatabase::connectionNames();

    bool gotActiveDb = false;

    for (int i = 0; i < connectionNames.count(); ++i) {
		if(connectionNames.at(i)==sys_label::str_sys.c_str()){
			QTreeWidgetItem *root = new QTreeWidgetItem(tree);
			QSqlDatabase db = QSqlDatabase::database(connectionNames.at(i), false);
			root->setText(0, Sys_Project::get_project_name().c_str());
			root->setExpanded(true);

			if (connectionNames.at(i) == activeDb) {
				gotActiveDb = true;
				setActive(root);
			}
			if (db.isOpen()) {
				this->create_folder_modules(root);
				QStringList tables = db.tables();
				this->create_tables_folder(&tables);
			}
		}
    }
    if (!gotActiveDb) {
        activeDb = connectionNames.value(0);
        setActive(tree->topLevelItem(0));
    }

    tree->doItemsLayout(); 
	emit send_txt2statusbar("Ready", 0);
}
//Show the metadata to the view
void Sys_Database_Project_Tree_Wid::show_metadata(void){
	 QTreeWidgetItem *cItem = tree->currentItem();
	 if (!cItem || !cItem->parent() || cItem->childCount()!=0){
			return;
	}
    setActive(cItem->parent());
	string buffer=this->get_table_name_sql(cItem);
	emit metaDataRequested(buffer.c_str());

}
//Recieve, that a tree item is activated
void Sys_Database_Project_Tree_Wid::on_tree_itemActivated(QTreeWidgetItem *item, int column){

	if (!item){
        int buff; //not needed, surpress warnings; just column 0 is used
        buff=column;
        return;
	}

    if (!item->parent()) {
        setActive(item);
    } 
	else if(item->childCount()!=0){
		//setActive(item);
	}
	else {
		string buffer=this->get_table_name_sql(item);
        emit tableActivated(buffer.c_str());
    }

}
//Recieve, that a tree item is clicked
void Sys_Database_Project_Tree_Wid::on_tree_itemClicked(QTreeWidgetItem *item, int column){

	if (!item){
        int buff;//not needed, surpress warnings; just column 0 is used
        buff=column;
        return;
	}

    if (!item->parent()) {
        setActive(item);
    }
	else if(item->childCount()!=0){
		//setActive(item);
	}
	else {
        this->qSetBold(this->previous_activated, false);
		this->qSetBold(item,true);
		this->previous_activated=item;

		string buffer=this->get_table_name_sql(item);
	
        emit tableClicked(buffer.c_str());
    }

}
//Recieve, that activated tree item is changed to another one
void Sys_Database_Project_Tree_Wid::on_tree_currentItemChanged(QTreeWidgetItem *current, QTreeWidgetItem *previous){
	//not needed
	QTreeWidgetItem *buff;
	buff=previous;

	metaDataAction->setEnabled(current && current->parent());
}
//_______
//private
//Set a item as the active/non-active item of the tree
void Sys_Database_Project_Tree_Wid::setActive(QTreeWidgetItem *item){
    for (int i = 0; i < tree->topLevelItemCount(); ++i) {
		if (tree->topLevelItem(i)->font(0).bold()){
            this->qSetBold(tree->topLevelItem(i), false);
		}
    }

    if (!item)
        return;

    this->qSetBold(item, true);
    activeDb = QSqlDatabase::connectionNames().value(tree->indexOfTopLevelItem(item));

}
//Set the active item with bold font or reset it to normal font
void Sys_Database_Project_Tree_Wid::qSetBold(QTreeWidgetItem *item, bool bold){
	if(item==NULL){
		return;
	}
    QFont font = item->font(0);
    font.setBold(bold);
    item->setFont(0, font);
}
//Create folders for the modules
void Sys_Database_Project_Tree_Wid::create_folder_modules(QTreeWidgetItem *root_item){
		if(Sys_Project::get_project_type()==_sys_project_type::proj_all){
				QTreeWidgetItem *modules = new QTreeWidgetItem(root_item);
				modules->setText(0, sys_label::str_fpl.c_str());
				QIcon icon1;
				icon1.addFile(":fpl_icon");
				modules->setIcon(0,icon1);

				modules = new QTreeWidgetItem(root_item);
				modules->setText(0, sys_label::str_hyd.c_str());
				QIcon icon2;
				icon2.addFile(":hyd_icon");
				modules->setIcon(0,icon2);

				modules = new QTreeWidgetItem(root_item);
				modules->setText(0, sys_label::str_dam.c_str());
				QIcon icon3;
				icon3.addFile(":dam_icon");
				modules->setIcon(0,icon3);

				modules = new QTreeWidgetItem(root_item);
				modules->setText(0, sys_label::str_risk.c_str());
				QIcon icon4;
				icon4.addFile(":risk_icon");
				modules->setIcon(0,icon4);

				modules = new QTreeWidgetItem(root_item);
				modules->setText(0, sys_label::str_cost.c_str());
				QIcon icon6;
				icon6.addFile(":cost_icon");
				modules->setIcon(0,icon6);

				modules = new QTreeWidgetItem(root_item);
				modules->setText(0, sys_label::str_madm.c_str());
				QIcon icon7;
				icon7.addFile(":madm_icon");
				modules->setIcon(0,icon7);

				modules = new QTreeWidgetItem(root_item);
				modules->setText(0, sys_label::str_alt.c_str());
				QIcon icon5;
				icon5.addFile(":alt_icon");
				modules->setIcon(0,icon5);

		}
		else if(Sys_Project::get_project_type()==_sys_project_type::proj_hyd || Sys_Project::get_project_type() == _sys_project_type::proj_hyd_temp){
				QTreeWidgetItem *modules = new QTreeWidgetItem(root_item);
				modules->setText(0, sys_label::str_hyd.c_str());
				QIcon icon1;
				icon1.addFile(":hyd_icon");
				modules->setIcon(0,icon1);
		}
		else if(Sys_Project::get_project_type()==_sys_project_type::proj_risk){
				QTreeWidgetItem *modules = new QTreeWidgetItem(root_item);
				modules->setText(0, sys_label::str_fpl.c_str());
				QIcon icon1;
				icon1.addFile(":fpl_icon");
				modules->setIcon(0,icon1);

				modules = new QTreeWidgetItem(root_item);
				modules->setText(0, sys_label::str_hyd.c_str());
				QIcon icon2;
				icon2.addFile(":hyd_icon");
				modules->setIcon(0,icon2);

				modules = new QTreeWidgetItem(root_item);
				modules->setText(0, sys_label::str_dam.c_str());
				QIcon icon3;
				icon3.addFile(":dam_icon");
				modules->setIcon(0,icon3);

				modules = new QTreeWidgetItem(root_item);
				modules->setText(0, sys_label::str_risk.c_str());
				QIcon icon4;
				icon4.addFile(":risk_icon");
				modules->setIcon(0,icon4);

		}
		else if(Sys_Project::get_project_type()==_sys_project_type::proj_dam_hyd || Sys_Project::get_project_type()==_sys_project_type::proj_dam ){
				QTreeWidgetItem *modules = new QTreeWidgetItem(root_item);
				modules->setText(0, sys_label::str_hyd.c_str());
				QIcon icon1;
				icon1.addFile(":hyd_icon");
				modules->setIcon(0,icon1);

				modules = new QTreeWidgetItem(root_item);
				modules->setText(0, sys_label::str_dam.c_str());
				QIcon icon2;
				icon2.addFile(":dam_icon");
				modules->setIcon(0,icon2);
		}
		else if(Sys_Project::get_project_type()==_sys_project_type::proj_fpl){
				QTreeWidgetItem *modules = new QTreeWidgetItem(root_item);
				modules->setText(0, sys_label::str_fpl.c_str());
				QIcon icon1;
				icon1.addFile(":fpl_icon");
				modules->setIcon(0,icon1);
		}
}
//Create tables in folders
void Sys_Database_Project_Tree_Wid::create_tables_folder(QStringList *table_list){
	int pos=-1;
	string buffer;

	table_list->sort();

	for (int i = 0; i < table_list->count(); i++) {
		if(Data_Base::get_driver_type()==_sys_driver_type::POSTGRESQL){
			buffer=table_list->at(i).toStdString();
			pos=buffer.find(Sys_Project::get_complete_project_database_schemata_name());
			if(pos==0){
				this->sort_tables2folder(buffer);
			}

		}
		else{
			buffer=table_list->at(i).toStdString();
			pos=buffer.find(Sys_Project::get_complete_project_database_table_prefix_name());
			if(pos==0){
				this->sort_tables2folder(buffer);
			}
		}
	}//end for-loop
}
//Sort tables to folder
void Sys_Database_Project_Tree_Wid::sort_tables2folder(string table_name){
	QList<QTreeWidgetItem *> item_list;
	QTreeWidgetItem *item;
	QTreeWidgetItem *new_item;
	int pos=-1;
	ostringstream buff;

	//find the module
	//hyd-module
	buff << functions::convert_string2lower_case(sys_label::str_hyd)<<"_";
	pos=table_name.find(buff.str());
	if(pos>=0){
		table_name=table_name.substr(pos+1+sys_label::str_hyd.length(), table_name.length());
		item_list.clear();
		item_list=this->tree->findItems(sys_label::str_hyd.c_str(),Qt::MatchRecursive);
		if(item_list.length()==0){
			return;
		}
		item=item_list.first();
		new_item = new QTreeWidgetItem(item);
		new_item->setText(0,table_name.c_str());
		return;
	}
	//dam-modul
	buff.str("");
	buff << functions::convert_string2lower_case(sys_label::str_dam)<<"_";
	pos=table_name.find(buff.str());
	if(pos>=0){
		table_name=table_name.substr(pos+1+sys_label::str_dam.length(), table_name.length());
		item_list.clear();
		item_list=this->tree->findItems(sys_label::str_dam.c_str(),Qt::MatchRecursive);
		if(item_list.length()==0){
			return;
		}
		item=item_list.first();
		new_item = new QTreeWidgetItem(item);
		new_item->setText(0,table_name.c_str());
		return;
	}

	//fpl-module
	buff.str("");
	buff << functions::convert_string2lower_case(sys_label::str_fpl)<<"_";
	pos=table_name.find(buff.str());
	if(pos>=0){
		table_name=table_name.substr(pos+1+sys_label::str_fpl.length(), table_name.length());
		item_list.clear();
		item_list=this->tree->findItems(sys_label::str_fpl.c_str(),Qt::MatchRecursive);
		if(item_list.length()==0){
			return;
		}
		item=item_list.first();
		new_item = new QTreeWidgetItem(item);
		new_item->setText(0,table_name.c_str());
		return;
	}

	//risk-module
	buff.str("");
	buff << functions::convert_string2lower_case(sys_label::str_risk)<<"_";
	pos=table_name.find(buff.str());
	if(pos>=0){
		table_name=table_name.substr(pos+1+sys_label::str_risk.length(), table_name.length());
		item_list.clear();
		item_list=this->tree->findItems(sys_label::str_risk.c_str(),Qt::MatchRecursive);
		if(item_list.length()==0){
			return;
		}
		item=item_list.first();
		new_item = new QTreeWidgetItem(item);
		new_item->setText(0,table_name.c_str());
		return;
	}

	//alt-module
	buff.str("");
	buff << functions::convert_string2lower_case(sys_label::str_alt)<<"_";
	pos=table_name.find(buff.str());
	if(pos>=0){
		table_name=table_name.substr(pos+1+sys_label::str_alt.length(), table_name.length());
		item_list.clear();
		item_list=this->tree->findItems(sys_label::str_alt.c_str(),Qt::MatchRecursive);
		if(item_list.length()==0){
			return;
		}
		item=item_list.first();
		new_item = new QTreeWidgetItem(item);
		new_item->setText(0,table_name.c_str());
		return;
	}

	//cost-module
	buff.str("");
	buff << functions::convert_string2lower_case(sys_label::str_cost)<<"_";
	pos=table_name.find(buff.str());
	if(pos>=0){
		table_name=table_name.substr(pos+1+sys_label::str_cost.length(), table_name.length());
		item_list.clear();
		item_list=this->tree->findItems(sys_label::str_cost.c_str(),Qt::MatchRecursive);
		if(item_list.length()==0){
			return;
		}
		item=item_list.first();
		new_item = new QTreeWidgetItem(item);
		new_item->setText(0,table_name.c_str());
		return;
	}

	//madm-module
	buff.str("");
	buff << functions::convert_string2lower_case(sys_label::str_madm)<<"_";
	pos=table_name.find(buff.str());
	if(pos>=0){
		table_name=table_name.substr(pos+1+sys_label::str_madm.length(), table_name.length());
		item_list.clear();
		item_list=this->tree->findItems(sys_label::str_madm.c_str(),Qt::MatchRecursive);
		if(item_list.length()==0){
			return;
		}
		item=item_list.first();
		new_item = new QTreeWidgetItem(item);
		new_item->setText(0,table_name.c_str());
		return;
	}




}
//Get complete table name for the sql-query
string Sys_Database_Project_Tree_Wid::get_table_name_sql(QTreeWidgetItem *item){
	ostringstream buff;
	//postgre-sql
	if(Data_Base::get_driver_type()==_sys_driver_type::POSTGRESQL){
		buff << Sys_Project::get_complete_project_database_schemata_name()<<".";
		//buff << Sys_Project::get_complete_project_database_table_prefix_name();
		buff << functions::convert_string2lower_case(item->parent()->text(0).toStdString()) << "_";
		buff << item->text(0).toStdString();
	}
	else{
		buff << Sys_Project::get_complete_project_database_table_prefix_name();
		buff << functions::convert_string2lower_case(item->parent()->text(0).toStdString()) << "_";
		buff << item->text(0).toStdString();
	}
	return buff.str();
}

//Get complete table name for the sql-query without schema name
string Sys_Database_Project_Tree_Wid::get_table_name_no_schema(QTreeWidgetItem *item) {
	ostringstream buff;

	buff << functions::convert_string2lower_case(item->parent()->text(0).toStdString()) << "_";
	buff << item->text(0).toStdString();

	return buff.str();
}
