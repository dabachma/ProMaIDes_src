#pragma once
/** 
	\class Sys_Letter_Button 
	\author Daniel Bachmann et al.
	\author produced by the Institute of Hydraulic Engineering (IWW), RWTH Aachen University
	\version 0.0.1                                                              
	\date 2011 
*/

#ifndef SYS_LETTER_BUTTON_H
#define SYS_LETTER_BUTTON_H


//qt-libs
 #include <QPushButton>

///Button class for sending the button text to a text display in case of pushing  \ingroup sys
/**

*/
class Sys_Letter_Button : public QPushButton
{
	//Macro for using signals and slots (Qt)in this class
	Q_OBJECT

public:
	///Default constructor
	Sys_Letter_Button(QWidget *parent=NULL);
	///Default destructor
	~Sys_Letter_Button(void);


public slots:
	///A slot to send the button text
	void slot2send_txt(void);

signals:

	///Send the button text
	void send_button_txt(const QString button_txt);



};
#endif
