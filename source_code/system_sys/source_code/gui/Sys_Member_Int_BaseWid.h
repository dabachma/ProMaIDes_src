/** \class Sys_Member_Int_BaseWid
	\author Tobias Schruff, Daniel Bachmann et al.
	\author produced by the Institute of Hydraulic Engineering (IWW), RWTH Aachen University
	\version 0.0.1                                                              
	\date 2009 
*/
#ifndef SYS_MEMBER_INT_BASEWID_H
#define SYS_MEMBER_INT_BASEWID_H

//forms
#include "ui_Sys_Member_Int_BaseWid.h"

//qt-libs
#include <QMenu>
#include <QPoint>
#include <QIcon>

//system sys
#include "Common_Const.h"
#include "Sys_Memory_Count.h"

///Base-widget for displaying an integer variable with a text label \ingroup sys
/**

*/
class Sys_Member_Int_BaseWid : public QWidget, public Ui::intForm
{
	Q_OBJECT //Macro for using signals and slots (Qt)in this class

public:

	///Default Constructor
	Sys_Member_Int_BaseWid(QWidget *parent = 0);
	///Default Destructor
	~Sys_Member_Int_BaseWid(void);

	///Changes the text of the QLabel
	void set_label_text(const string text);
	///Changes tooltip text
	void set_tooltip(const string tooltip);
	///Set the range of the QSpinBox
	void set_range(const int min, const int max);
	///Set the incerement of QSpinBox
	void set_increment(int range);
	///Set the QSpinBox value
	void set_value(const int val);
	///Get the QSpinBox value
	int get_value(void);
	///Returns editable status
	bool is_editable(void);

private:

	///Size of an Increment Step [%]
	const int inc_step;

public slots:

	///Change Edit Status (Read Only)
	void set_editable(bool edit);
};

#endif // SYS_MEMBER_INT_BASEWID_H
