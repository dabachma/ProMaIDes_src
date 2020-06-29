#pragma once
#ifndef FPL_SECTION_DIA_H
#define FPL_SECTION_DIA_H

#include <QObject>
#include "ui_Fpl_Section_Dia.h"
#include "Fpl_Data_Section.h"

class Fpl_Section_Dia : public QDialog
{
Q_OBJECT
public:
	//constructor
	Fpl_Section_Dia(void);

	//destructor
	~Fpl_Section_Dia(void);
	
	//make the combo values
	void sectionnumber_to_combo(const string section_type_name, Fpl_Data_Section *section, const int text_number);

	//get the section number from the combo dialog
	int get_section_number(void);

private:
	//made with the qt designer
	Ui::Fpl_Section_Dia fpl_section_dia;

	//choose which actiontext hast to be set
	void set_dia_text(int const text_number, const string section_type_name);
};
#endif