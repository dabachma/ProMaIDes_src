#pragma once
/**\class AltGui_Alternative_Type_Dia
	\author Daniel Bachmann et al.
	\author produced by the Institute of Hydraulic Engineering (IWW), RWTH Aachen University
	\version 0.0.1                                                              
	\date 2009 
*/
#ifndef ALTGUI_ALTERNATIVE_TYPE_DIA_H
#define ALTGUI_ALTERNATIVE_TYPE_DIA_H

//Qt-libs
#include <QtGui>
#include <QDialog>
//forms
#include "ui_AltGui_Alternative_Type_Dia.h"

#include "AltGui_Alternative_Info_Dia.h"
#include "_Sys_Common_System.h"

///Namespace for the identfier-strings for replacing measure in the HYD-system
namespace alt_replace_hyd{

	///Replacing hydraulic boundary scenarios
	const string boundary_sc("Replace HYD-Boundary scenario(s)");
	///Replacing dike-lines
	const string dike_lines("Replace a 2-d dike-line");
	///Change river profiles
	const string river_profiles("Change river-profile(s)");
	//add further measures...
};
///Namespace for the identfier-strings for adding measure in the HYD-system
namespace alt_add_hyd{

	///Replacing dike-lines
	const string dike_lines("Add a 2-d dike-line");
	///Add river profiles
	const string river_profiles("Add river-profile(s)");
	//add further measures..
};

///Namespace for the identfier-strings for replacing measure in the FPL-system
namespace alt_replace_fpl{

	///Replacing a complete FPL-section
	const string section("Replace a FPL-section (HYD-relevant parameters are changed)");
	///Change parameters of a FPL-section
	const string section_param("Change FPL-section parameter (HYD-relevant parameters stay unchanged)");
	//add further measures...
};
///Namespace for the identfier-strings for adding measure in the FPL-system
namespace alt_add_fpl{

	///Add a FPL-section
	const string section("Add a FPL-section");
	//add further measures..
};
///Namespace for the identfier-strings for combination of measure in the FPL-system
namespace alt_combi_fpl{

	///Add a FPL-section
	const string fpl_line("Add multiple FPL-section(s) as protection line");
	//add further measures..
};

///Namespace for the identfier-strings for replacing measure in the DAM-system
namespace alt_replace_dam{

	///Replace ecn-damage function(s) for the elements specified by a polygon
	const string dam_ecn_function_polygon("Change ECN-damage function(s) of the elements specified in a polygon(s)");
	///Replace eco-biotope-type function(s) for the elements specified by a polygon
	const string dam_eco_btype_function_polygon("Change ECO-biotope-type function(s) of the elements specified in a polygon(s)");
	///Replace eco-soil-cost function(s) for the elements specified by a polygon
	const string dam_eco_soil_function_polygon("Change ECO-soil-cost function(s) of the elements specified in a polygon(s)");
	///Replace people2risk categories(s) for the elements specified by a polygon
	const string dam_pop_categories_polygon("Change POP categories of the elements specified in a polygon(s)");
	///Replace people2risk categories(s) for the elements specified by a polygon
	const string dam_pys_categories_polygon("Change PYS categories of the elements specified in a polygon(s)");

	//add the rest of the damage types

	///Resettlement for a specific polygon(s); economic stockvalues are set to zero, the population is set to zero as well as the pyscho-social criteria; the bitope-type is selectable  
	const string dam_resettlement("Resettlement of the elements specified in a polygon(s)");



	//add further measures...
};
///Namespace for the identfier-strings for adding measure in the DAM-system
namespace alt_add_dam{

};





///Dialog-class for setting the detailed data concerning the type of a specific alternative \ingroup alt
/**
*/
class AltGui_Alternative_Type_Dia : public QDialog, public Ui::AltGui_Alternative_Type_Dia
{
	//Macro for using signals and slots (Qt)in this class
	Q_OBJECT

public:
	///Default constructor
	AltGui_Alternative_Type_Dia(QWidget *parent = 0);
	///Default destructor
	~AltGui_Alternative_Type_Dia(void);

	///Set for the dialog the measure identifier
	void set_dialog(const _alt_general_info data);
	///Start the dialog it returns true by acception, false by rejection
	bool start_dialog(void);
	///Get the data of the measure
	_alt_general_info get_measure_general_data(void);

	///Get if the back-button is clicked
	bool get_back_button_is_clicked(void);

	///Check if the detailed measure type is valid
	static bool check_measure_type_valid(const string type);

private:
	//members
	///Data of the measure
	_alt_general_info measure_data;

	///Flag if the back-button is clocked
	bool back_pressed;


	///Stringlist for the replacing measures of the hydraulic system
	QStringList replace_hyd;
	///Stringlist for the adding measures of the hydraulic system
	QStringList add_hyd;
	///Stringlist for the replacing measures of the fpl-system
	QStringList replace_fpl;
	///Stringlist for the adding measures of the fpl-system
	QStringList add_fpl;
	///Stringlist for a combination of measures of the fpl-system
	QStringList combi_fpl;
	///Stringlist for the replacing measures of the damage system
	QStringList replace_dam;
	///Stringlist for the adding measures of the damage system
	QStringList add_dam;

	//method
	///Set up measure string lists
	void set_up_measure_string_list(void);


private slots:
	///Back-button is clicked
	void back_button_is_clicked(void);
	///Set up the combo-box
	void set_up_combo_box(void);
};
#endif
