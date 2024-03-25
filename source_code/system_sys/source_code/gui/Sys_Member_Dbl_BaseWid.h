#pragma once
/**\class Sys_Member_Dbl_BaseWid
	\author Tobias Schruff, Daniel Bachmann et al.
	\author produced by the Institute of Hydraulic Engineering (IWW), RWTH Aachen University
	\version 0.0.1                                                              
	\date 2009 
*/
#ifndef SYS_MEMBER_DBL_BASEWID_H
#define SYS_MEMBER_DBL_BASEWID_H

//forms
#include "ui_Sys_Member_Dbl_BaseWid.h"
#include "Sys_Member_Dbl_Prec_BaseWid.h"

//qt-libs
#include <QMenu>
#include <QPoint>
#include <QIcon>


//system sys
#include "Common_Const.h"
#include "Sys_Memory_Count.h"
#include "_Sys_Abstract_Base_Wid.h"

///Base-widget for displaying a double variable with a text label \ingroup sys
/**

*/
class Sys_Member_Dbl_BaseWid : public QWidget, public Ui::dblForm
{
	Q_OBJECT //Macro for using signals and slots (Qt)in this class

public:

	///Default Constructor
	Sys_Member_Dbl_BaseWid(QWidget *parent = 0);
	///Default Destructor
	~Sys_Member_Dbl_BaseWid(void);

	///Changes the text of the QLabel
	void set_label_text(const string);
	///Changes tooltip text
	void set_tooltip(const string);
	///Set the range of the QSpinBox
	void set_range(const double min, const double);
	///Set the Increment of the QSpinBox
	void set_increment(const double);
	///Set the QSpinBox value
	void set_value(const double);
	///Get the QSpinBox value
	double get_value(void);
	///Returns editable status
	bool is_editable(void);
	///Sets the Suffix of the QSpinBox
	void set_suffix(string);
	///Changes the width of the combo box
	void set_box_width(const double);

private:

	//member

	///Custom context_menu
	QMenu *context_menu;
	///Custom context_menu precision icon
	QIcon prec_icon;
	///Custom context_menu precision action
	QAction *prec_action;
	///Size of an Increment Step [%]
	const int inc_step;

public slots:

	///Show the Custom PopUp Menus
	void contextMenu_request(QPoint);
	///Change editable status of the Widget
	void set_editable(bool);
	///Gain Information To Show Precision Dialog
	void precDiaRequest(void);
	///Execute and Show Precision Dialog
	void execPrecDia(int);
	///Set Precision of The DoubleSpinBox
	void set_precision(int);
	///Change the current datarole | edit or display
	void set_dataRole(DataRole role);

signals:

	///Connected to precDiaRequest, gains addictional information like number of decimals of the current spinBox widget
	void showPrecDia(int);
};

#endif // SYS_MEMBER_DBL_BASEWID_H
