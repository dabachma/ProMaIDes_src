#pragma once
/**\class Sys_Exception_Sender
	\author Daniel Bachmann et al.
	\author produced by the Institute of Hydraulic Engineering (IWW), RWTH Aachen University
	\version 0.0.1                                                              
	\date 2009 
*/
#ifndef SYS_EXCEPTION_SENDER
#define SYS_EXCEPTION_SENDER
//qtclasse
#include <QObject>
//libs


///Sender-class for sending a fatal error text (Qstring) to the main-application and  sending the number of exception to the statusbar \ingroup sys
/** This class sends a fatal error text (Qstring) to the main-application, where it is handled as a dialog. Moreover, the number of errors/warnings is
send to the statusbar widget of the main-window. It is written as a singleton pattern.
\see Error.h and child-classes, Sys_Status_Bar_Wid.h
*/
class Sys_Exception_Sender: public QObject
{
	//Macro for using signals and slots (Qt)in this class
	Q_OBJECT
public:
	///It returns the static pointer to this unique object (singleton pattern); if it is not yet allocated, it will be allocated
	static Sys_Exception_Sender* self(void);
	
	///Default destructor
	~Sys_Exception_Sender(void);

	//method
	///Set the error text in case of afatal error and send it
	void set_fatal_errortxt(const QString txt);
	///Set the error number and send it
	void set_error_number(const int number, const QString tooltip);
	///Set the warning number and send it
	void set_warning_number(const int number, const QString tooltip);

	///Set to open the reconnection dialog
	void set_open_reconnection_dia(QString time);
	///Set to close the reconnection dialog
	void set_close_reconnection_dia(void);

signals:
	///Sender for the warning number
	void send_warning_number(const int number, const QString tooltip);
	///Sender for the errornumber
	void send_error_number(const int number, const QString tooltip);
	///Sender for the fatal error text
	void send_fatal_error(const QString txt);

	///Sender to open the reconnection tries dialog
	void send_reconnection_try_dia_open(QString time);
	///Sender to close the reconnection tries dialog
	void send_reconnection_try_dia_close(void);

private:
	///Default constructor as private (singleton pattern)
	Sys_Exception_Sender(void);

	//member
	///Pointer to this unique object (singleton pattern)
	static Sys_Exception_Sender *ptr_class;
};
#endif

