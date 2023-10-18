#pragma once
/**\class Sys_Status_Bar_Wid
	\author Daniel Bachmann et al.
	\author produced by the Institute of Hydraulic Engineering (IWW), RWTH Aachen University
	\version 0.0.1                                                              
	\date 2009 
*/
#ifndef SYS_STATUS_BAR_WID_H
#define SYS_STATUS_BAR_WID_H

//forms
#include "ui_Sys_Status_Bar_Wid.h"

//qt
#include "qdockwidget.h"

//system sys
#include "_Sys_Common_System.h"

///Widget-class which represents the adapted statusbar of the main-window \ingroup sys
class Sys_Status_Bar_Wid : public QWidget, private Ui::Sys_Status_Bar_Wid
{
//Macro for using signals and slots (Qt)in this class
Q_OBJECT

public:
	///Default constructor
	Sys_Status_Bar_Wid(void);
	///Default destructor
	~Sys_Status_Bar_Wid(void);

	//method


	///Get pointer to the my close button
	QPushButton * get_ptr_my_close(void);
	///Set the database-connection label of the statusbar to "yes" (true) or "no" (false)
	void set_dbcon(const bool flag);

	///Set the total warning number to the statusbar
	void set_warning_number(const int number, const QString tooltip);
	///Set the total error number to the statusbar
	void set_error_number(const int number, const QString tooltip);

	///Set the number of area state and measure state
	void set_system_state(const _sys_system_id id);

	///Set the fpl-status text-label of the statusbar to "yes" (true) or "no" (false)
	void set_fplthread(const bool flag);
	///Set the fpl-status text-label of the statusbar to "yes" (true) or "no" (false)
	void set_hydthread(const bool flag, const string number="");
	///Set the hydraulic-status text-label of the statusbar to "sleep"
	void set_hydthread_sleep(const bool flag);
	///Set the dam-status text-label of the statusbar to "yes" (true) or "no" (false)
	void set_damthread(const bool flag);
	///Set the dam-status text-label of the statusbar "sleep"
	void set_damthread_sleep(const bool flag);
	///Set the risk-status text-label of the statusbar to "yes" (true) or "no" (false)
	void set_riskthread(const bool flag);
	///Set the risk-status text-label of the statusbar to "sleep"
	void set_riskthread_sleep(const bool flag);
	///Set the madm-status text-label of the statusbar to "yes" (true) or "no" (false)
	void set_madmthread(const bool flag);
	///Get a pointer to the risk-state check box 
	QCheckBox* get_ptr_risk_state_check_box(void);
	///Set tooltip for risk-state check box
	void set_tooltip_risk_state_check_box(const bool fpl, const bool dam, const bool hyd);
	///Reset tooltip for risk-state check box
	void reset_tooltip_risk_state_check_box(void);


	///Set the memory-counter label of the statusbar
	void set_required_memory(const QString txt, const QString tooltip);

	
signals:
	///Emeit the close flag
	void emit_close(bool flag);

public slots:
	///Close the widget
	void close_wid(bool flag);


private:

	///Flag if it is closed
	bool close_flag;



};
#endif