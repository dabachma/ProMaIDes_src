#pragma once
#ifndef FPL_DATA_SECTION_H
#define FPL_DATA_SECTION_H
//libs


//class
#include "Fpl_Section.h"
#include "_Common_System.h"




class Fpl_Data_Section : public _Common_System
{
public:
	//constructor
	Fpl_Data_Section(void);
	//destructor
	~Fpl_Data_Section(void);
	
	//members
	int *number_specif_sectiontype;
	
	
	//decide per console which action is desired
	void action(void);
	//show the section table
	QTableView* show_section_table(void);
	//show one section
	QWidget* show_one_section(const int actuel_secnum);
	//statistics of the section data
	void statistics_of_section(void);
	
	//ask which section
	int ask_section(const string section_specifier);
	//numbers of specific section type to calc
	int calc_quant_spec_section(const string section_type_name);

private:
	//members
	Database_Viewer *brows_distclass;
	const int nr_distclass_table;
	Database_Viewer *brows_distclass_default;
	const int nr_distclass_default;

	Database_Viewer *brows_section;

	QWidget *win_section;
	QBoxLayout *my_layout;

	

	//the tables from the distribution_class are set and show
	void set_distributionclass_view(const int number, const int actuel_secnum);
	//the tables from the distribution_class (default values) are set and show
	void set_distributionclass_default_view(const int number, const int actuel_secnum);

	
	//set the error
	Error set_error(const int err_type);

};
#endif