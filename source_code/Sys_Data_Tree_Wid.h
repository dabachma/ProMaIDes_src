#pragma once
#ifndef SYS_DATA_TREE_WID_H
#define SYS_DATA_TREE_WID_H

/**\class Sys_Data_Tree_Wid
	\author Daniel Bachmann et al.
	\author produced by the Institute of Hydraulic Engineering (IWW), RWTH Aachen University
	\version 0.0.1
	\date 2009
*/

//QT libs
#include <QWidget>
#include <QTreeWidgetItem>
#include <QMainWindow>
//forms
#include "ui_Sys_Data_Tree_Wid.h"

//system hyd
#include "Hyd_Multiple_Hydraulic_Systems.h"
#include "HydGui_River_Member_Wid.h"
#include "HydGui_System_Member_Wid.h"
#include "HydGui_Floodplain_Member_Wid.h"
#include "HydGui_Rv_Profile_General_Wid.h"
#include "HydGui_Bound_Scenario_Table_Wid.h"
#include "Hyd_Boundary_Szenario_Management.h"

//system dam
#include "Dam_Damage_System.h"
#include "DamGui_Result_Summary_Wid.h"
#include "DamGui_Result_Wid.h"
#include "DamGui_Raster_Wid.h"
#include "DamGui_Ecn_Func_Wid.h"
#include "DamGui_Eco_Btype_Wid.h"
#include "DamGui_Eco_Soil_Cost_Wid.h"
#include "DamGui_Eco_Soil_Erosion_Wid.h"
#include "DamGui_Pop_Function_Wid.h"
#include "DamGui_Pys_Categories_Wid.h"
#include "DamGui_Sc_Point_Wid.h"
#include "DamGui_Sc_Subcategory_Wid.h"
#include "DamGui_Ci_Point_Wid.h"
#include "DamGui_Ci_Polygon_Wid.h"

//system fpl
#include "Fpl_Calculation.h"
#include "FplGui_Prob_Result_Wid.h"
#include "FplGui_Detailed_Result_Wid.h"
#include "FplGui_System_Member_Wid.h"
//#include "FplGui_HighGround_FaultTree_Wid.h"
//#include "FplGui_Dike_FaultTree_Wid.h"

#include "FplGui_Faulttree_Dune_Wid.h"
#include "FplGui_Faulttree_Gate_Wid.h"
#include "FplGui_Faulttree_Dike_Wid.h"

//system sys
#include "Common_Const.h"
#include "Sys_One_Filechooser_Wid.h"

//system risk
#include "Risk_System.h"
#include "RiskGui_Result_Cumulated_Wid.h"
#include "RiskGui_Result_Wid.h"
#include "RiskGui_Detailed_Result_Wid.h"
#include "RiskGui_Detailed_Risk_Summary_Wid.h"
#include "RiskGui_Result_Scenario_Wid.h"
#include "RiskGui_Probabilty_Fpl_Section_Wid.h"
#include "RiskGui_Catchment_Scenario_Statistic_Wid.h"
#include "RiskGui_System_Member_Wid.h"

//system madm
#include "MadmGui_Weight_Sets_Wid.h"
#include "MadmGui_Total_Rank_Wid.h"
#include "MadmGui_Matrix_Summary_Wid.h"

//system alt
#include "AltGui_Alternative_Info_Wid.h"

///Namespace for to integrate the text identifier for the data tree \see Sys_Data_Tree_Wid \ingroup sys
namespace sys_data_tree_id{
	//general
	///String-id for the configuration of a modul
	const string id_config("CONFIG");
	///String-id for the results of a modul
	const string id_results("RESULT");
	///String-id for the detailed results of a modul
	const string id_detailed_results("RESULT_DETAILED");
	///String-id for the summary of results of a modul
	const string id_summary_results("SUMMARY");

	//hyd
	///String-id for the river model
	const string hyd_river_id("RIVER");
	///String-id for the floodplain model
	const string hyd_floodplain_id("FLOODPLAIN");
	///String-id for the boundary scenatios
	const string hyd_bound_sc_id("BOUNDARY_SCENARIO");

	//dam
	///String-id for the economic damage model
	const string dam_ecn_id("ECN");
	///String-id for the ecologic damage model
	const string dam_eco_id("ECO");
	///String-id for the ecologic damage model for biotope-type
	const string dam_eco_btype_id("BIOTOPE");
	///String-id for the ecologic damage model for soil-erosion damages
	const string dam_eco_soil_id("SOIL_EROSION");
	///String-id for the psycho-social damage model
	const string dam_pys_id("PYS");
	///String-id for the people2risk damage model
	const string dam_pop_id("POP");
	///String-id for the simple counting damage model
	const string dam_sc_id("SC");
	///String-id for the CI damage model
	const string dam_ci_id("CI");

	///String-id for the damage raster
	const string dam_raster_id("RASTER");
	///String-id for the damage function (ecn)
	const string dam_land_use_cat("LAND_USE_CATEGORIES");
	///String-id for the damage function (pop)
	const string dam_vul_cat("VULNERABILITY_CATEGORIES");

	///String-id for the biotope-type (eco)
	const string dam_biotope_type_id("BIOTOPE_TYPES");
	///String-id for the soil-cost function (eco)
	const string dam_soil_cost_id("COST_FUNCTION");
	///String-id for the soil-erosion function (eco)
	const string dam_soil_ero_id("EROSION_FUNCTION");
	///String-id for the categories (pys)
	const string dam_category_id("CATEGORY");
	///String-id for the density function (pys)
	const string dam_density_func_id("DENSITYFUNTION");

	///String-id for the subcategories (sc)
	const string dam_subcategory_id("SUBCATEGORY");
	///String-id for the points (sc/CI)
	const string dam_point_id("POINTS");

	///String-id for the points (sc/CI)
	const string dam_polygon_id("POLYGONS");

	//fpl
	///String-id for the fpl section
	const string fpl_section_id("SECTION");
	///String-id for the fpl fault tree
	const string fpl_fault_tree("FAULT-TREE");

	//risk
	///String-id for the cumulated results of the risk modul
	const string risk_cumulated_results("RESULT_CUMULATED");
	///String-id for the total results of the risk modul
	const string risk_total_results("RESULT_TOTAL");
	///String-id for the generated scenarios
	const string risk_scenarios("LIST_SCENARIO");
	///String-id for the fpl-section probability
	const string risk_prob_section("LIST_PROB_SECTION");
	///String-id for the statistic of the catchment risk approach
	const string risk_ca_statistics("STATISTICS");

	//madm
	///String-id for the weighting sets
	const string madm_weights("WEIGHT_SET");
	///String-id for the matrix
	const string madm_matrix("MATRIX");
};

///Data-viewer as a tree widget for displaying the project data for each module \ingroup sys
/**

*/
class Sys_Data_Tree_Wid : public QWidget, public Ui::Sys_Data_Tree_Wid
{
//thread for loading the widgets
friend class Sys_Data_Widget_Loader;

//Macro for using signals and slots (Qt)in this class
Q_OBJECT

public:
	///Default constructor
	Sys_Data_Tree_Wid(QWidget *parent = 0);
	///Default destructor
	~Sys_Data_Tree_Wid(void);

	//method

	///Set a database connection
	void set_db_connection(QSqlDatabase *ptr_database);
	///Set-up the data into the tree from the database
	void set_up_tree_data(void);

	///Set pointer to the overall data tab-widget, which includes GIS, DATABASE and DATA
	void set_ptr_overall_data_tab_wid(QTabWidget *ptr_tab_wid);
	///Set a pointer to the main window
	void set_ptr_main_window(QMainWindow *ptr_main);
	///Set the pointer to the data-tab widget
	void set_ptr_data_tab_wid(QTabWidget *ptr_tab_wid);

	///Set the pointer to the data-tab widget tab hyd
	void set_ptr_data_tab_hyd(QWidget *ptr_tab);
	///Set the pointer to the data-tab widget tab dam
	void set_ptr_data_tab_dam(QWidget *ptr_tab);
	///Set the pointer to the data-tab widget tab fpl
	void set_ptr_data_tab_fpl(QWidget *ptr_tab);
	///Set the pointer to the data-tab widget tab madm
	void set_ptr_data_tab_madm(QWidget *ptr_tab);
	///Set the pointer to the data-tab widget tab risk
	void set_ptr_data_tab_risk(QWidget *ptr_tab);
	///Set the pointer to the data-tab widget tab cost
	void set_ptr_data_tab_cost(QWidget *ptr_tab);
	///Set the pointer to the data-tab widget tab alt
	void set_ptr_data_tab_alt(QWidget *ptr_tab);
	//implement further modules

	///Set a pointer to the system id
	void set_ptr_sys_system_id(_sys_system_id *id);

	///Set a pointer to the current project
	void set_ptr_current_project(Sys_Project *ptr_project);

public slots:
	///Recieve, that a tree item is double clicked
    void tree_item_clicked(QTreeWidgetItem *item, int column);
	///Change the focus of the tab widget, when the item is clicked
	void change_focus_tab_widget(QTreeWidgetItem *item, int column);
	///Refresh the tree
	void refresh_tree(void);

signals:
	///Send a text to the status bar
	void send_txt2statusbar(QString txt, int time);

	///Send to enabled/disabled the main widget
	void send_enabled_main(bool flag);

private:
	//members

	///Database connection for the data tree
	QSqlDatabase database;

	///Pointer to the data tab-widget of the main-window
	QTabWidget *ptr_data_tab_overall;
	///Pointer to the data tab-widget of the overall data-tab widget
	QTabWidget *ptr_data_tab;
	///Pointer to the main window
	QMainWindow *ptr_main_wid;

	///Pointer to the data tab-widget of the main-window, tab hyd
	QWidget *ptr_data_tab_hyd;
	///Pointer to the data tab-widget of the main-window, tab dam
	QWidget *ptr_data_tab_dam;
    ///Pointer to the data widget, dam
    //QWidget *ptr_data_dam;
	///Pointer to the data tab-widget of the main-window, tab fpl
	QWidget *ptr_data_tab_fpl;
	///Pointer to the data tab-widget of the main-window, tab madm
	QWidget *ptr_data_tab_madm;
	///Pointer to the data tab-widget of the main-window, tab madm
	QWidget *ptr_data_tab_risk;
	///Pointer to the data tab-widget of the main-window, tab alt
	QWidget *ptr_data_tab_alt;
	///Pointer to the data tab-widget of the main-window, tab cost
	QWidget *ptr_data_tab_cost;
	//implement further modules

	///Current module type
	_sys_system_modules current_type;

	///Identifier of the object, namely id-areastate and id-measure number
	_sys_system_id *system_id;

	///Current project
	Sys_Project *current_project;

	///Flag if a item is double-clicked
	bool double_clicked;

	///Previous double clicked item
	QTreeWidgetItem *previous_db_clicked;
	///The currently active item of the tree
	QTreeWidgetItem *active_item;

	///Current horizontal slider position of a scrollarea (if existent)
	int current_hor_slider_pos;
	///Current vertical slider position of a scrollarea (if existent)
	int current_ver_slider_pos;

	//method
	///Set-up the tree widget with the pre-defined branches corresponding to the project type (_sys_project_type)
	void set_up_tree_predefinied(void);
	///Set-up predefinied branch for module hydraulic
	void set_up_predefined_hyd(QTreeWidgetItem *root);
	///Set-up predefined branch for module damage
	void set_up_predefined_dam(QTreeWidgetItem *root);
	///Set-up predefined branch for module fpl
	void set_up_predefined_fpl(QTreeWidgetItem *root);
	///Set-up predefined branch for module madm
	void set_up_predefined_madm(QTreeWidgetItem *root);
	///Set-up predefined branch for module risk
	void set_up_predefined_risk(QTreeWidgetItem *root);
	///Set-up predefined branch for module cost
	void set_up_predefined_cost(QTreeWidgetItem *root);
	///Set-up predefined branch for module alt
	void set_up_predefined_alt(QTreeWidgetItem *root);
	//set up for further modules

	///Set-up current data for the module hydraulic
	void set_up_current_data_hyd(void);
	///Set-up current data for the module damage
	void set_up_current_data_dam(void);
	///Set-up current data for the module fpl
	void set_up_current_data_fpl(void);
	///Set-up current data for the module madm
	void set_up_current_data_madm(void);
	///Set-up current data for the module risk
	void set_up_current_data_risk(void);
	///Set-up current data for the module alt
	void set_up_current_data_alt(void);
	///Set-up current data for the module cost
	void set_up_current_data_cost(void);
	//set up for further modules

	///Set widget to the hydraulic view
	void widget2hyd(QTreeWidgetItem *item);
	///Set widget to the damage view
	void widget2dam(QTreeWidgetItem *item);
	///Set widget to the fpl view
	void widget2fpl(QTreeWidgetItem *item);
	///Set widget to the madm view
	void widget2madm(QTreeWidgetItem *item);
	///Set widget to the risk view
	void widget2risk(QTreeWidgetItem *item);
	///Set widget to the alt view
	void widget2alt(QTreeWidgetItem *item);
	///Set widget to the cost view
	void widget2cost(QTreeWidgetItem *item);
	//set up for further modules

	///Clone the database and open ist
	void clone_database(QSqlDatabase *ptr_database);

	///Clear the tree view
	void clear_total(void);

	///Find the module type out of the identifier of the item
	bool find_module_type(string id);

	///Start the tree search for the modules
	QTreeWidgetItem* start_module_tree_search(const string search_txt, _sys_system_modules module);
	///Search for an specific item
	QTreeWidgetItem* item_search(const QTreeWidgetItem * parent, const string search_txt);

	///Set the doubled clicked item to bold letters
	void set_doubledclicked2bold(QTreeWidgetItem * item2bold);

	///Set number of items in the second column of the tree widget
	void set_number_items(void);

	///Count the relevant child items
	void item_count(QTreeWidgetItem * parent, const bool first);

	///Delete the layout for the hydraulic view
	void delete_hyd_lay(void);
	///Delete the layout for the hydraulic view
	void delete_dam_lay(void);
	///Delete the layout for the fpl view
	void delete_fpl_lay(void);
	///Delete the layout for the madm view
	void delete_madm_lay(void);
	///Delete the layout for the risk view
	void delete_risk_lay(void);
	///Delete the layout for the alt view
	void delete_alt_lay(void);
	///Delete the layout for the cost view
	void delete_cost_lay(void);
	//set up for further modules

	//Calculate the number of the item in the branch
	int calculate_item_number(QTreeWidgetItem *item);

	///Set error(s)
	Error set_error(const int err_type);

private slots:
	///Show the gis data widget
	void show_gis_widget(void);

	///Collapse all branches a expand the first one
	void collapse_all_branch(void);

	///Set the current active item
	void set_current_active_item(QTreeWidgetItem * item, int column);

	///Change the widget and the marked tree-leaf when the spin box value of an widget is changed
	void change_widget_by_spinbox(int number, QTreeWidgetItem *item, const int ver_pos, const int hor_pos);

	///Set the data tree widget to enabled or not (send from the widget loader)
	void set_enabled(bool flag);
};
#endif
