#include "source_code\Main_Headers_Precompiled.h"
//#include "Sys_Data_Widget_Loader.h"


Sys_Data_Widget_Loader::Sys_Data_Widget_Loader(void){

	this->ptr_datatree=NULL;
	this->item2load=NULL;
	Memory_Count::self()->add_mem(sizeof(Sys_Data_Widget_Loader), _system_modules::SYS_SYS);
}
Sys_Data_Widget_Loader::~Sys_Data_Widget_Loader(void){

	Memory_Count::self()->minus_mem(sizeof(Sys_Data_Widget_Loader), _system_modules::SYS_SYS);
}
//____________
//public
//Set the pointer to the data tree widget
void Sys_Data_Widget_Loader::set_ptr_data_tree(Sys_Data_Tree_Wid *ptr){
	this->ptr_datatree=ptr;
}
//Run the thread; the pointer to the data tree widget has to be set before running the thread
void Sys_Data_Widget_Loader::run(void){
	if(this->item2load==NULL){
		return;
	}
	if(this->ptr_datatree==NULL){
		return;
	}
	try{
		emit enable_data_tree(false);

		//hyd
		if(this->ptr_datatree->current_type==_system_modules::HYD_SYS){
			this->widget2hyd(this->item2load);
		}
		//dam
		else if(this->ptr_datatree->current_type==_system_modules::DAM_SYS){
			this->widget2dam(this->item2load);
		}
		//fpl
		else if(this->ptr_datatree->current_type==_system_modules::FPL_SYS){
			this->widget2fpl(this->item2load);
		}
		//madm
		else if(this->ptr_datatree->current_type==_system_modules::MADM_SYS){
			this->widget2madm(this->item2load);
		}
		//risk
		else if(this->ptr_datatree->current_type==_system_modules::RISK_SYS){
			this->widget2risk(this->item2load);
		}

		emit enable_data_tree(true);

	}
	catch(Error msg){
		emit enable_data_tree(true);
		ostringstream info;
		info <<"Try to set up the data to the data view"<<endl;
		msg.make_second_info(info.str());
		msg.output_msg(0);
		return;
	}
}
//Set the tree widget item, which should be loaded as widget
void Sys_Data_Widget_Loader::set_ptr_item2load(QTreeWidgetItem *load){
	this->item2load=load;
}
void Sys_Data_Widget_Loader::set_widget(QWidget *widget){
	this->widget=widget;
}
//__________
//private
//Set widget to the hydraulic view
void Sys_Data_Widget_Loader::widget2hyd(QTreeWidgetItem *item){
	
	//this->ptr_datatree->delete_hyd_lay();
	QVBoxLayout *hyd_lay;
	//hyd_lay=new QVBoxLayout(this->ptr_datatree->ptr_data_tab_hyd);


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
		//wid=HydGui_System_Member_Wid
		//HydGui_System_Member_Wid *wid=(HydGui_System_Member_Wid *)widget;
		//Hyd_River_Profile_Type_Bridge *buffer=(Hyd_River_Profile_Type_Bridge *)prof_type;
		HydGui_System_Member_Wid *wid;
		wid=new HydGui_System_Member_Wid;
		//wid->setParent(this->ptr_datatree->ptr_data_tab_hyd);
		//set the data
		wid->set_head_spinBox_range(1,1);
		try{
			wid->set_member(&this->ptr_datatree->database);
		}
		catch(Error msg){
			throw msg;
		}

		
		//hyd_lay->addWidget(wid);
		//wid->show();
		//emit widget2show(wid);
		return;
	}
	//hyd boundary scenarios
	buffer << str_mod<<sys_data_tree_id::hyd_bound_sc_id<<"/";
	check=buffer.str();
	buffer.str("");
	if(check==given){
		HydGui_Bound_Scenario_Table_Wid *wid;
		wid=new HydGui_Bound_Scenario_Table_Wid;
		wid->setParent(this->ptr_datatree->ptr_data_tab_hyd);
		//set the data via the boundary scenario manager
		Hyd_Boundary_Szenario_Management my_manager;
		my_manager.set_systemid(*this->ptr_datatree->system_id);

		try{
			my_manager.set_szenario_per_db(&this->ptr_datatree->database);
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
		wid->setParent(this->ptr_datatree->ptr_data_tab_hyd);
		//set head spin box
		int number_fp=0;
		number_fp=Hyd_Model_Floodplain::check_relevant_model_database(&this->ptr_datatree->database);
		wid->set_head_spinBox_range(number_fp,1);
		int current_number=0;
		current_number=this->ptr_datatree->calculate_item_number(item);
		wid->set_head_spinBox_value(current_number+1);
		wid->set_treeItem(item);
		QObject::connect(wid, SIGNAL(send_change_widget(int , QTreeWidgetItem * )), this->ptr_datatree, SLOT(change_widget_by_spinbox(int , QTreeWidgetItem *)));

		//set the data
		try{
			wid->set_member(&this->ptr_datatree->database, fp_nr);
		}
		catch(Error msg){
			throw msg;
		}


		hyd_lay->addWidget(wid);
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
			wid->setParent(this->ptr_datatree->ptr_data_tab_hyd);
			//set head spin box
			int number_rv=0;
			number_rv=Hyd_Model_River::check_relevant_model_database(&this->ptr_datatree->database);
			wid->set_head_spinBox_range(number_rv,1);
			int current_number=0;
			current_number=this->ptr_datatree->calculate_item_number(item);
			wid->set_head_spinBox_value(current_number+1);
			wid->set_treeItem(item);
			QObject::connect(wid, SIGNAL(send_change_widget(int , QTreeWidgetItem * )), this->ptr_datatree, SLOT(change_widget_by_spinbox(int , QTreeWidgetItem *)));

			//set the data
			try{
				wid->set_member(&this->ptr_datatree->database, rv_id);
			}
			catch(Error msg){
				throw msg;
			}

			hyd_lay->addWidget(wid);
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
			wid->setParent(this->ptr_datatree->ptr_data_tab_hyd);
			//set head spin box
			int number_profs=0;
			int current_number=0;
			QSqlQueryModel buffer;
			number_profs=_Hyd_River_Profile::select_relevant_profiles_in_database(&buffer, &this->ptr_datatree->database,*this->ptr_datatree->system_id, rv_id, false);
			current_number=this->ptr_datatree->calculate_item_number(item);
			wid->set_head_spinBox_range(number_profs,1);
			wid->set_head_spinBox_value(current_number+1);
			wid->set_treeItem(item);
			QObject::connect(wid, SIGNAL(send_change_widget(int , QTreeWidgetItem * )), this->ptr_datatree, SLOT(change_widget_by_spinbox(int , QTreeWidgetItem *)));

			
			//set data
			try{
				wid->set_member(&this->ptr_datatree->database, rv_id, prof_id, *this->ptr_datatree->system_id);
			}
			catch(Error msg){
				throw msg;
			}


			hyd_lay->addWidget(wid);
			wid->show();
			return;

		}
	}
	buffer.str("");


}
//Set widget to the damage view
void Sys_Data_Widget_Loader::widget2dam(QTreeWidgetItem *item){

	QSqlQueryModel raster;
	QSqlTableModel function(0, this->ptr_datatree->database);
	this->ptr_datatree->delete_dam_lay();
	QVBoxLayout *dam_lay;
	dam_lay=new QVBoxLayout(this->ptr_datatree->ptr_data_tab_dam);

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
		wid->setParent(this->ptr_datatree->ptr_data_tab_dam);
		//set head spin box
		int number_raster=0;
		number_raster=Dam_Ecn_Raster::select_relevant_raster_database(&raster,&this->ptr_datatree->database, false);
		wid->set_head_spinBox_range(number_raster,1);
		int current_number=0;
		current_number=this->ptr_datatree->calculate_item_number(item);
		wid->set_head_spinBox_value(current_number+1);
		wid->set_treeItem(item);
		QObject::connect(wid, SIGNAL(send_change_widget(int , QTreeWidgetItem * )), this->ptr_datatree, SLOT(change_widget_by_spinbox(int , QTreeWidgetItem *)));

		//set the data
		try{
			wid->set_member(&this->ptr_datatree->database, raster_nr, _dam_raster_types::ecn_total);
		}
		catch(Error msg){
			throw msg;
		}
		dam_lay->addWidget(wid);
		wid->show();
		return;
	}
	buffer.str("");

	//ecn damage function
	buffer << str_mod<<sys_data_tree_id::dam_ecn_id<<"/"<<sys_data_tree_id::dam_damage_func_id<<"/";
	pos=given.find(buffer.str());
	if(pos>=0){
		raster_nr=item->data(0, Qt::UserRole).toInt();
		DamGui_Ecn_Func_Wid *wid;
		wid=new DamGui_Ecn_Func_Wid;
		wid->setParent(this->ptr_datatree->ptr_data_tab_dam);
		//set head spin box
		int number_raster=0;
		number_raster=Dam_Ecn_Damage_Function::select_relevant_functions_database(&function,false);
		wid->set_head_spinBox_range(number_raster,1);
		int current_number=0;
		current_number=this->ptr_datatree->calculate_item_number(item);
		wid->set_head_spinBox_value(current_number+1);
		wid->set_treeItem(item);
		QObject::connect(wid, SIGNAL(send_change_widget(int , QTreeWidgetItem * )), this->ptr_datatree, SLOT(change_widget_by_spinbox(int , QTreeWidgetItem *)));

		//set the data
		try{
			wid->set_member(&this->ptr_datatree->database, raster_nr);
		}
		catch(Error msg){
			throw msg;
		}
		dam_lay->addWidget(wid);
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
		wid->setParent(this->ptr_datatree->ptr_data_tab_dam);
		//set head spin box
		int number_raster=0;
		number_raster=Dam_Eco_Btype_Raster::select_relevant_raster_database(&raster,&this->ptr_datatree->database, false);
		wid->set_head_spinBox_range(number_raster,1);
		int current_number=0;
		current_number=this->ptr_datatree->calculate_item_number(item);
		wid->set_head_spinBox_value(current_number+1);
		wid->set_treeItem(item);
		QObject::connect(wid, SIGNAL(send_change_widget(int , QTreeWidgetItem * )), this->ptr_datatree, SLOT(change_widget_by_spinbox(int , QTreeWidgetItem *)));

		//set the data
		try{
			wid->set_member(&this->ptr_datatree->database, raster_nr, _dam_raster_types::eco_btype);
		}
		catch(Error msg){
			throw msg;
		}
		dam_lay->addWidget(wid);
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
		wid->setParent(this->ptr_datatree->ptr_data_tab_dam);
		//set head spin box
		int number_raster=0;
		number_raster=Dam_Eco_Btype::select_relevant_biotope_types_database(&function,false);
		wid->set_head_spinBox_range(number_raster,1);
		int current_number=0;
		current_number=this->ptr_datatree->calculate_item_number(item);
		wid->set_head_spinBox_value(current_number+1);
		wid->set_treeItem(item);
		QObject::connect(wid, SIGNAL(send_change_widget(int , QTreeWidgetItem * )), this->ptr_datatree, SLOT(change_widget_by_spinbox(int , QTreeWidgetItem *)));

		//set the data
		try{
			wid->set_member(&this->ptr_datatree->database, raster_nr);
		}
		catch(Error msg){
			throw msg;
		}
		dam_lay->addWidget(wid);
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
		wid->setParent(this->ptr_datatree->ptr_data_tab_dam);
		//set head spin box
		int number_raster=0;
		number_raster=Dam_Eco_Soil_Raster::select_relevant_raster_database(&raster,&this->ptr_datatree->database, false);
		wid->set_head_spinBox_range(number_raster,1);
		int current_number=0;
		current_number=this->ptr_datatree->calculate_item_number(item);
		wid->set_head_spinBox_value(current_number+1);
		wid->set_treeItem(item);
		QObject::connect(wid, SIGNAL(send_change_widget(int , QTreeWidgetItem * )), this->ptr_datatree, SLOT(change_widget_by_spinbox(int , QTreeWidgetItem *)));

		//set the data
		try{
			wid->set_member(&this->ptr_datatree->database, raster_nr, _dam_raster_types::eco_soil_total);
		}
		catch(Error msg){
			throw msg;
		}
		dam_lay->addWidget(wid);
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
		wid->setParent(this->ptr_datatree->ptr_data_tab_dam);
		//set head spin box
		int number_raster=0;
		number_raster=Dam_Eco_Soil_Cost_Function::select_relevant_functions_database(&function,false);
		wid->set_head_spinBox_range(number_raster,1);
		int current_number=0;
		current_number=this->ptr_datatree->calculate_item_number(item);
		wid->set_head_spinBox_value(current_number+1);
		wid->set_treeItem(item);
		QObject::connect(wid, SIGNAL(send_change_widget(int , QTreeWidgetItem * )), this->ptr_datatree, SLOT(change_widget_by_spinbox(int , QTreeWidgetItem *)));

		//set the data
		try{
			wid->set_member(&this->ptr_datatree->database, raster_nr);
		}
		catch(Error msg){
			throw msg;
		}
		dam_lay->addWidget(wid);
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
		wid->setParent(this->ptr_datatree->ptr_data_tab_dam);
		//set head spin box
		int number_raster=0;
		number_raster=Dam_Eco_Soil_Erosion_Function::select_relevant_functions_database(&function,false);
		wid->set_head_spinBox_range(number_raster,1);
		int current_number=0;
		current_number=this->ptr_datatree->calculate_item_number(item);
		wid->set_head_spinBox_value(current_number+1);
		wid->set_treeItem(item);
		QObject::connect(wid, SIGNAL(send_change_widget(int , QTreeWidgetItem * )), this->ptr_datatree, SLOT(change_widget_by_spinbox(int , QTreeWidgetItem *)));

		//set the data
		try{
			wid->set_member(&this->ptr_datatree->database, raster_nr);
		}
		catch(Error msg){
			throw msg;
		}
		dam_lay->addWidget(wid);
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
		wid->setParent(this->ptr_datatree->ptr_data_tab_dam);
		//set head spin box
		int number_raster=0;
		number_raster=Dam_People_Raster::select_relevant_raster_database(&raster,&this->ptr_datatree->database, false);
		wid->set_head_spinBox_range(number_raster,1);
		int current_number=0;
		current_number=this->ptr_datatree->calculate_item_number(item);
		wid->set_head_spinBox_value(current_number+1);
		wid->set_treeItem(item);
		QObject::connect(wid, SIGNAL(send_change_widget(int , QTreeWidgetItem * )), this->ptr_datatree, SLOT(change_widget_by_spinbox(int , QTreeWidgetItem *)));

		//set the data
		try{
			wid->set_member(&this->ptr_datatree->database, raster_nr, _dam_raster_types::pop_total);
		}
		catch(Error msg){
			throw msg;
		}
		dam_lay->addWidget(wid);
		wid->show();
		return;
	}
	buffer.str("");

	//pop category
	buffer << str_mod<<sys_data_tree_id::dam_pop_id<<"/"<<sys_data_tree_id::dam_damage_func_id<<"/";
	pos=given.find(buffer.str());
	if(pos>=0){
		raster_nr=item->data(0, Qt::UserRole).toInt();
		DamGui_Pop_Function_Wid *wid;
		wid=new DamGui_Pop_Function_Wid;
		wid->setParent(this->ptr_datatree->ptr_data_tab_dam);
		//set head spin box
		int number_raster=0;
		number_raster=Dam_People_Damage_Function::select_relevant_functions_database(&function,false);
		wid->set_head_spinBox_range(number_raster,1);
		int current_number=0;
		current_number=this->ptr_datatree->calculate_item_number(item);
		wid->set_head_spinBox_value(current_number+1);
		wid->set_treeItem(item);
		QObject::connect(wid, SIGNAL(send_change_widget(int , QTreeWidgetItem * )), this->ptr_datatree, SLOT(change_widget_by_spinbox(int , QTreeWidgetItem *)));

		//set the data
		try{
			wid->set_member(&this->ptr_datatree->database, raster_nr);
		}
		catch(Error msg){
			throw msg;
		}
		dam_lay->addWidget(wid);
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
		wid->setParent(this->ptr_datatree->ptr_data_tab_dam);
		//set head spin box
		int number_raster=0;
		number_raster=Dam_Pys_Raster::select_relevant_raster_database(&raster,&this->ptr_datatree->database, false);
		wid->set_head_spinBox_range(number_raster,1);
		int current_number=0;
		current_number=this->ptr_datatree->calculate_item_number(item);
		wid->set_head_spinBox_value(current_number+1);
		wid->set_treeItem(item);
		QObject::connect(wid, SIGNAL(send_change_widget(int , QTreeWidgetItem * )), this->ptr_datatree, SLOT(change_widget_by_spinbox(int , QTreeWidgetItem *)));

		//set the data
		try{
			wid->set_member(&this->ptr_datatree->database, raster_nr, _dam_raster_types::pys_pys_total);
		}
		catch(Error msg){
			throw msg;
		}
		dam_lay->addWidget(wid);
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
		wid->setParent(this->ptr_datatree->ptr_data_tab_dam);
		//set head spin box
		int number_raster=0;
		number_raster=Dam_Pys_Category::select_relevant_category_database(&function,false);
		wid->set_head_spinBox_range(number_raster,1);
		int current_number=0;
		current_number=this->ptr_datatree->calculate_item_number(item);
		wid->set_head_spinBox_value(current_number+1);
		wid->set_treeItem(item);
		QObject::connect(wid, SIGNAL(send_change_widget(int , QTreeWidgetItem * )), this->ptr_datatree, SLOT(change_widget_by_spinbox(int , QTreeWidgetItem *)));

		//set the data
		try{
			wid->set_member(&this->ptr_datatree->database, raster_nr);
		}
		catch(Error msg){
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
		wid->setParent(this->ptr_datatree->ptr_data_tab_risk);
		wid->set_header_tooltip(item->data(0,Qt::AccessibleDescriptionRole).toString().toStdString());
		wid->set_risk_approach_id("");
		try{
			wid->set_up_widget(&this->ptr_datatree->database, *this->ptr_datatree->system_id);
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
		wid->setParent(this->ptr_datatree->ptr_data_tab_risk);
		wid->set_header_tooltip(item->data(0,Qt::AccessibleDescriptionRole).toString().toStdString());
		wid->set_risk_approach_id("SC%");
		try{
			wid->set_up_widget(&this->ptr_datatree->database, *this->ptr_datatree->system_id);
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
		wid->setParent(this->ptr_datatree->ptr_data_tab_risk);
		wid->set_header_tooltip(item->data(0,Qt::AccessibleDescriptionRole).toString().toStdString());
		wid->set_risk_approach_id("CA%");
		try{
			wid->set_up_widget(&this->ptr_datatree->database, *this->ptr_datatree->system_id);
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
		wid->setParent(this->ptr_datatree->ptr_data_tab_risk);
		wid->set_header_tooltip(item->data(0,Qt::AccessibleDescriptionRole).toString().toStdString(), item);
		//set the data
		wid->set_up_widget(&this->ptr_datatree->database, glob_id);

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
		wid->setParent(this->ptr_datatree->ptr_data_tab_risk);
		wid->set_header_tooltip(item->data(0,Qt::AccessibleDescriptionRole).toString().toStdString(), item);
		//set the data
		wid->set_up_widget(&this->ptr_datatree->database, glob_id);

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
		wid->setParent(this->ptr_datatree->ptr_data_tab_risk);
		wid->set_header_tooltip(item->data(0,Qt::AccessibleDescriptionRole).toString().toStdString(), item);
		//set the data
		wid->set_up_widget(&this->ptr_datatree->database, glob_id);

		dam_lay->addWidget(wid);
		wid->show();
		return;
	}
	buffer.str("");


}
//Set widget to the fpl view
void Sys_Data_Widget_Loader::widget2fpl(QTreeWidgetItem *item){
	this->ptr_datatree->delete_fpl_lay();
	QVBoxLayout *fpl_lay;
	fpl_lay=new QVBoxLayout(this->ptr_datatree->ptr_data_tab_fpl);

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
		wid->setParent(this->ptr_datatree->ptr_data_tab_fpl);
		//set the data
		wid->set_head_spinBox_range(1,1);
		try{
			wid->set_member(&this->ptr_datatree->database);
		}
		catch(Error msg){
			throw msg;
		}

		fpl_lay->addWidget(wid);
		wid->show();
		return;
	}
	//fpl fault tree (highground)
	buffer << str_mod<<sys_data_tree_id::fpl_fault_tree<<"/"<<fpl_label::sec_highground<<"/";
	check=buffer.str();
	buffer.str("");
	if(check==given){
		//set widget (replace this!)
		FplGui_HighGround_FaultTree_Wid *wid;
		wid=new FplGui_HighGround_FaultTree_Wid;
		wid->setParent(this->ptr_datatree->ptr_data_tab_fpl);
		////set the data
		//ostringstream txt;
		//txt <<"Faultree highground";
		//wid->setText(txt.str().c_str());

		fpl_lay->addWidget(wid);
		wid->show();
		return;
	}
	//fpl fault tree (dike)
	buffer << str_mod<<sys_data_tree_id::fpl_fault_tree<<"/"<<fpl_label::sec_dike<<"/";
	check=buffer.str();
	buffer.str("");
	if(check==given){
		//set widget (replace this!)
		QLabel *wid;
		wid=new QLabel;
		wid->setParent(this->ptr_datatree->ptr_data_tab_fpl);
		//set the data
		ostringstream txt;
		txt <<"Faultree dike";
		wid->setText(txt.str().c_str());

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
		wid->setParent(this->ptr_datatree->ptr_data_tab_fpl);
		//set the data
		ostringstream txt;
		txt <<"Faultree wall";
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
		wid->setParent(this->ptr_datatree->ptr_data_tab_fpl);
		//set the data
		ostringstream txt;
		txt <<"Faultree mobile wall";
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
		wid->setParent(this->ptr_datatree->ptr_data_tab_fpl);
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
		wid->setParent(this->ptr_datatree->ptr_data_tab_fpl);
		wid->set_header_tooltip(item->data(0,Qt::AccessibleDescriptionRole).toString().toStdString(), item);
		//set the data
		try{
			wid->set_member(&this->ptr_datatree->database, sec_id, *this->ptr_datatree->system_id);
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
	
		//set widget (replace this!)
		QLabel *wid;
		wid=new QLabel;
		wid->setParent(this->ptr_datatree->ptr_data_tab_fpl);
		//set the data
		ostringstream txt;
		txt <<"Section detailed result data of"<< sec_id;
		wid->setText(txt.str().c_str());

		fpl_lay->addWidget(wid);
		wid->show();
		return;
	}
	buffer.str("");

}
//Set widget to the madm view
void Sys_Data_Widget_Loader::widget2madm(QTreeWidgetItem *item){
	this->ptr_datatree->delete_madm_lay();
	QVBoxLayout *madm_lay;
	madm_lay=new QVBoxLayout(this->ptr_datatree->ptr_data_tab_madm);
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
		wid->setParent(this->ptr_datatree->ptr_data_tab_madm);
		//set the data
		try{
			wid->set_up_widget(&this->ptr_datatree->database);
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
		wid->setParent(this->ptr_datatree->ptr_data_tab_madm);
		wid->set_risk_type(_risk_type::nobreak_risk);
		wid->set_header_tooltip(item->data(0,Qt::AccessibleDescriptionRole).toString().toStdString());
		//set the data
		try{
			wid->set_up_widget(&this->ptr_datatree->database, *this->ptr_datatree->system_id);
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
		wid->setParent(this->ptr_datatree->ptr_data_tab_madm);
		wid->set_header_tooltip(item->data(0,Qt::AccessibleDescriptionRole).toString().toStdString(), item);
		//set the data
		try{
			wid->set_up_widget(&this->ptr_datatree->database, this->ptr_datatree->system_id->area_state, set_id, _risk_type::nobreak_risk);
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
		wid->setParent(this->ptr_datatree->ptr_data_tab_madm);
		wid->set_risk_type(_risk_type::catchment_risk);
		wid->set_header_tooltip(item->data(0,Qt::AccessibleDescriptionRole).toString().toStdString());
		//set the data
		try{
			wid->set_up_widget(&this->ptr_datatree->database, *this->ptr_datatree->system_id);
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
		wid->setParent(this->ptr_datatree->ptr_data_tab_madm);
		wid->set_header_tooltip(item->data(0,Qt::AccessibleDescriptionRole).toString().toStdString(), item);
		//set the data
		try{
			wid->set_up_widget(&this->ptr_datatree->database, this->ptr_datatree->system_id->area_state, set_id, _risk_type::catchment_risk);
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
void Sys_Data_Widget_Loader::widget2risk(QTreeWidgetItem *item){
	this->ptr_datatree->delete_risk_lay();
	QVBoxLayout *risk_lay;
	risk_lay=new QVBoxLayout(this->ptr_datatree->ptr_data_tab_risk);

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
		QLabel *wid;
		wid=new QLabel;
		wid->setParent(this->ptr_datatree->ptr_data_tab_risk);
		//set the data
		ostringstream txt;
		txt <<"Configuration of the risk caluclation";
		wid->setText(txt.str().c_str());

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
		wid->setParent(this->ptr_datatree->ptr_data_tab_risk);
		wid->set_header_tooltip(item->data(0,Qt::AccessibleDescriptionRole).toString().toStdString());
		wid->set_risk_type(_risk_type::nobreak_risk);
		//set the data
		wid->set_up_widget(&this->ptr_datatree->database, *this->ptr_datatree->system_id);

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
		wid->setParent(this->ptr_datatree->ptr_data_tab_risk);
		wid->set_header_tooltip(item->data(0,Qt::AccessibleDescriptionRole).toString().toStdString(), item);
		//set the data
		wid->set_up_widget(&this->ptr_datatree->database, glob_id, *this->ptr_datatree->system_id, _risk_type::nobreak_risk);

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
		wid->setParent(this->ptr_datatree->ptr_data_tab_risk);
		wid->set_header_tooltip(item->data(0,Qt::AccessibleDescriptionRole).toString().toStdString(), item);
		//set the data
		try{
			wid->set_member(&this->ptr_datatree->database,*this->ptr_datatree->system_id,_risk_type::nobreak_risk);
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
		wid->setParent(this->ptr_datatree->ptr_data_tab_risk);
		wid->set_header_tooltip(item->data(0,Qt::AccessibleDescriptionRole).toString().toStdString(), item);
		//set the data
		wid->set_up_widget(&this->ptr_datatree->database, glob_id);

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
		wid->setParent(this->ptr_datatree->ptr_data_tab_risk);
		wid->set_header_tooltip(item->data(0,Qt::AccessibleDescriptionRole).toString().toStdString());
		wid->set_risk_type(_risk_type::scenario_risk);
		//set the data
		wid->set_up_widget(&this->ptr_datatree->database, *this->ptr_datatree->system_id);

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
		wid->setParent(this->ptr_datatree->ptr_data_tab_risk);
		wid->set_header_tooltip(item->data(0,Qt::AccessibleDescriptionRole).toString().toStdString(), item);
		//set the data
		wid->set_up_widget(&this->ptr_datatree->database, glob_id, *this->ptr_datatree->system_id, _risk_type::scenario_risk);

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
		wid->setParent(this->ptr_datatree->ptr_data_tab_risk);
		wid->set_header_tooltip(item->data(0,Qt::AccessibleDescriptionRole).toString().toStdString());
		//set the data
		wid->set_up_widget(&this->ptr_datatree->database, *this->ptr_datatree->system_id);

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
		wid->setParent(this->ptr_datatree->ptr_data_tab_risk);
		wid->set_header_tooltip(item->data(0,Qt::AccessibleDescriptionRole).toString().toStdString());
		wid->set_risk_type(_risk_type::catchment_risk);
		//set the data
		wid->set_up_widget(&this->ptr_datatree->database, *this->ptr_datatree->system_id);

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
		wid->setParent(this->ptr_datatree->ptr_data_tab_risk);
		wid->set_header_tooltip(item->data(0,Qt::AccessibleDescriptionRole).toString().toStdString(), item);
		//set the data
		wid->set_up_widget(&this->ptr_datatree->database, glob_id, *this->ptr_datatree->system_id, _risk_type::catchment_risk);

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
		wid->setParent(this->ptr_datatree->ptr_data_tab_risk);
		wid->set_header_tooltip(item->data(0,Qt::AccessibleDescriptionRole).toString().toStdString(), item);
		//set the data
		try{
			wid->set_member(&this->ptr_datatree->database,*this->ptr_datatree->system_id,_risk_type::catchment_risk);
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
		wid->setParent(this->ptr_datatree->ptr_data_tab_risk);
		wid->set_header_tooltip(item->data(0,Qt::AccessibleDescriptionRole).toString().toStdString(), item);
		//set the data
		wid->set_up_widget(&this->ptr_datatree->database, glob_id);
		risk_lay->addWidget(wid);
		wid->show();
		return;
	}
	buffer.str("");
}
//Set widget to the alt view
void Sys_Data_Widget_Loader::widget2alt(QTreeWidgetItem *item){
	this->ptr_datatree->delete_alt_lay();
	QVBoxLayout *alt_lay;
	alt_lay=new QVBoxLayout(this->ptr_datatree->ptr_data_tab_alt);
}
//Set widget to the cost view
void Sys_Data_Widget_Loader::widget2cost(QTreeWidgetItem *item){
	this->ptr_datatree->delete_cost_lay();
	QVBoxLayout *cost_lay;
	cost_lay=new QVBoxLayout(this->ptr_datatree->ptr_data_tab_cost);

}