#pragma once
#ifndef SYS_COMMON_OUTPUT_H
#define SYS_COMMON_OUTPUT_H
#include "Sys_Output_Division.h"



class Sys_Common_Output
{
public:
//constructor
Sys_Common_Output(void);
//destructor
~Sys_Common_Output(void);

//members
///Class for all output for the Sys-modul (general)
static Sys_Output_Division *output_system;
///Class for all output for the Fpl-modul (probability modul)
static Sys_Output_Division *output_fpl;
///Class for all output for the Sys_exception-modul (extern part of the sys-core)
static Sys_Output_Division *output_excep;
///Class for all output for the Hyd-modul (hydraulic modul)
static Sys_Output_Division *output_hyd;
///Class for all output for the Madm-modul (decision support modul)
static Sys_Output_Division *output_madm;
///Class for all output for the dam-modul (damage modul)
static Sys_Output_Division *output_dam;
///Class for all output for the risk-modul (risk modul)
static Sys_Output_Division *output_risk;
///Class for all output for the alt-modul (alt modul)
static Sys_Output_Division *output_alt;
///Class for all output for the cost-modul (cost modul)
static Sys_Output_Division *output_cost;
///Class for all output for the hydrol-modul (hydrol modul)
static Sys_Output_Division *output_hydrol;

//methods
///Allocate the new Sys_Output_Division class for the Sys-modul
static void new_output_system(QTextEdit *ptr_editor=NULL);
///Allocate the new Sys_Output_Division class for the Fpl-modul 
static void new_output_fpl(QTextEdit *ptr_editor=NULL);
///Allocate the new Sys_Output_Division class for the Sys_exception-modul
static void new_output_excep(QTextEdit *ptr_editor=NULL);
///Allocate the new Sys_Output_Division class for the Hyd-modul
static void new_output_hyd(QTextEdit *ptr_editor=NULL);
///Allocate the new Sys_Output_Division class for the Madm-modul
static void new_output_madm(QTextEdit *ptr_editor=NULL);
///Allocate the new Sys_Output_Division class for the dam-modul
static void new_output_dam(QTextEdit *ptr_editor=NULL);
///Allocate the new Sys_Output_Division class for the risk-modul
static void new_output_risk(QTextEdit *ptr_editor=NULL);
///Allocate the new Sys_Output_Division class for the alt-modul
static void new_output_alt(QTextEdit *ptr_editor=NULL);
///Allocate the new Sys_Output_Division class for the cost-modul
static void new_output_cost(QTextEdit *ptr_editor=NULL);
///Allocate the new Sys_Output_Division class for the hydrol-modul
static void new_output_hydrol(QTextEdit *ptr_editor = NULL);

///Delete the Sys_Output_Division class for the sys-modul
static void delete_output_system(void);
///Delete the Sys_Output_Division class for the fpl-modul
static void delete_output_fpl(void);
///Delete the Sys_Output_Division class for the sys-exception-modul
static void delete_output_excep(void);
///Delete the Sys_Output_Division class for the hyd-modul
static void delete_output_hyd(void);
///Delete the Sys_Output_Division class for the madm-modul
static void delete_output_madm(void);
///Delete the Sys_Output_Division class for the dam-modul
static void delete_output_dam(void);
///Delete the Sys_Output_Division class for the risk-modul
static void delete_output_risk(void);
///Delete the Sys_Output_Division class for the alt-modul
static void delete_output_alt(void);
///Delete the Sys_Output_Division class for the cost-modul
static void delete_output_cost(void);
///Delete the Sys_Output_Division class for the hydrol-modul
static void delete_output_hydrol(void);

};
#endif