#pragma once
/**\class Sys_Memory_Count
	\author Daniel Bachmann et al.
	\author produced by the Institute of Hydraulic Engineering (IWW), RWTH Aachen University
	\version 0.0.1                                                              
	\date 2009 
*/
#ifndef SYS_MEMORY_COUNT_H
#define SYS_MEMORY_COUNT_H
//libs
//qtclasse
#include <QObject>
#include <QWidget>
#include <QMutex>

//qt
#include <QMessageBox>

//system_sys
#include "Common_Const.h"


///Class for counting the memory of the modules \ingroup sys
/** The counting is not 100% exacte, because memory requirements of extern classes,
e.g. Qt-classes are not counted. The counting is in generally done in each constructor (add)
and destructor (minus) of each class. The counting is divided into the different modules (fpl, hyd etc.)

For this class a singleton pattern is used, it means that just one object can instantiated of this class. 
In software engineering, the singleton pattern is a design pattern that is used to restrict instantiation of a class to one object. 
This is useful when exactly one object is needed to coordinate actions across the system.

*/
class Sys_Memory_Count : public QObject
{
//Macro for using signals and slots (Qt) in this class
Q_OBJECT

public:

	///Default destructor
	~Sys_Memory_Count(void);

	//members


	//methods
	///Main method of the singleton pattern; it instantiate the class and returns a static pointer to the single object 
	static Sys_Memory_Count* self(void);

	///Add the memory to the total memory 
    void add_mem(const long long int mem);
	///Add the memory to the total memory and to the given memory counter of the moduls
    void add_mem(const long long int mem, const _sys_system_modules modul);
	///Subtract the memory of the total memory 
    void minus_mem(const long long int mem);
	///Subtract the memory of the total memory and of the given memory counter of the moduls
    void minus_mem(const long long int mem, const _sys_system_modules modul);

	///Set a pointer to the output widget
	void set_output_widget(QWidget *ptr_output);
	///Output the momory to the console; just used in case of an application without a gui
	void output_mem(void);

	///Make a widget for warning
	void make_warning_end_widget(void);

signals:
	///Send the memory as a string to the statusbar widget (signal)
	void send_memorytxt(const QString mem_txt, const QString tooltip);

private:
	//member
	///Counter for the memory of the total system
    static long long int mem_count;
	///Counter for the memory of the sys_system
    static long long int mem_count_sys;
	///Counter for the memory of the sys_system geometry
    static long long int mem_count_geosys;
	///Counter for the memory of the hyd_system
    static long long int mem_count_hyd;
	///Counter for the memory of the fpl_system
    static long long int mem_count_fpl;
	///Counter for the memory of the madm_system
    static long long int mem_count_madm;
	///Counter for the memory of the dam_system
    static long long int mem_count_dam;
	///Counter for the memory of the risk_system
    static long long int mem_count_risk;
	///Counter for the memory of the alt_system
    static long long int mem_count_alt;
	///Counter for the memory of the cost_system
    static long long int mem_count_cost;
	//..introduce further modules

	///Static pointer to this class; (singleton pattern)
	static Sys_Memory_Count *ptr_class;
	///Widget pointer for the output
	QWidget *ptr_output;
	///The used memory as a string
	QString memory_txt;

	///Count the difference of the new memory
	int delta_mem;

	///Member to avoid simultaneous use of the counter by multi-threading
	QMutex my_locker;

	//method
	///Private constructor for the singleton pattern
	Sys_Memory_Count(void);
	///Generate a string for the tooltip
	string generate_tooltip(void);

};
#endif
