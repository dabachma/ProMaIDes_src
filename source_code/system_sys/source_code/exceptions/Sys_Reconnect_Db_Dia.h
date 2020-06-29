#ifndef SYS_RECONNECT_DB_DIA_H
#define SYS_RECONNECT_DB_DIA_H
/**\class Sys_Reconnect_Db_Dia
	\author Daniel Bachmann et al.
	\author produced by the Institute of Hydraulic Engineering (IWW), RWTH Aachen University
	\version 0.0.1                                                              
	\date 2009 
*/


//system-sys
#include "Sys_Diverse_Text_Dia.h"

///Dialog-class if the database connection breaks during calculation  \ingroup sys
/**

*/
class Sys_Reconnect_Db_Dia : public Sys_Diverse_Text_Dia
{
//Macro for using signals and slots (Qt)in this class	
Q_OBJECT

public:
	///Default constructor
	Sys_Reconnect_Db_Dia(QWidget *parent = 0);
	///Default destructor
	~Sys_Reconnect_Db_Dia(void);

	///Start the dialog and return the user's decision (ok:=true, cancel:=false)
	bool start_dialog(void);


public slots:

	///Close the dialog
	void close_dialog(void);

	///Ok-button pressed actions
	void ok_button_is_clicked(void);

private:

	//methods
	///Handles the close event by the close-button of the dialog
	void closeEvent(QCloseEvent *close);

	///Flag if a close event is sent
	bool close_event;

	///Flag if the closing event is acceptable
	bool good_close;

};
#endif
