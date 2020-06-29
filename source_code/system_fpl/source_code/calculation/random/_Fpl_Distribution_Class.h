#pragma once
/**\class _Fpl_Distribution_Class
	\author Daniel Bachmann et al.
	\author produced by the Institute of Hydraulic Engineering (IWW), RWTH Aachen University
	\version 0.0.1                                                              
	\date 2009 
*/
#ifndef _FPL_DISTRIBUTION_CLASS_H
#define _FPL_DISTRIBUTION_CLASS_H
//qt libs
#include <QtSql>
//system sys
#include "Common_Const.h"
#include "Tables.h"
#include "Error.h"
#include "Warning.h"

//fpl-libs
#include "Fpl_Random_Generator_Ranmar.h"
#include "Fpl_Random_Generator_Zikurat.h"
#include "Fpl_Random_Generator_MofAll.h"
#include "Fpl_Random_Generator_Sfmt.h"


//enum
///Enumerator for the distribution types \ingroup fpl
enum _fpl_distribution_type :short {
    ///Mean distribution type, e.g. log-normal, uniform, normal etc.
    mean_type,
    ///Triangle distribution type
    triangle_type,
    ///Discrete distribution type
    discret_type,
    ///Combination of two mean type distributions
    mean2mean_type,
    ///Combination of a discrete distribution to a mean type distributions
    discrete2mean_type,
    ///Unknown distribution type
    unknown_dist_type
} ;

//structures
///Data structure for storing the results of the transformation process \ingroup fpl
struct _fpl_result{
	///End results of the whole transformationprocess
	double end_result;
	///Interim result, which is needed by a transformation via cobinated distrubution types, e.g. mean-type to mean-type distribution
	double inter_result;
};

///Data structure for the boundaries of a randomly generated variables  \ingroup fpl
struct _fpl_boundaries{
	///Minimum boundary
	double min;
	///Maximum boundary
	double max;
};

///Base-class for the distribution types for the transformation and generation of randomly distributed variables  \ingroup fpl
/**
This class is a virtual parent class for the distribution types. There are following distribution types:
	- mean distribution type: distribution like normal, log-normal, uniform etc. are grouped in this type. 
	They are characterised by a mean, a standard deviation , a type and a constant. (Fpl_Distribution_Class_Mean)
	- triangle distribution type: it consists of  a minmum, middle and a maximum and a constant value. (Fpl_Distribution_Class_Triangle)
	- discret distribution type: it consists of discret value and their correspondig probabilities. 
	The sum of probabilities have to be 100 %.  (Fpl_Distribution_Class_Discret)
	- a combination of two mean type distributions: the first transformed random value gives the
	mean for the second mean type distribution. (Fpl_Distribution_Class_Mean_Mean)
	-a combination of a discrete and mean-type distributions: First the discret value with a probabilites is calculated.
	This discret value is the mean to a corresponding mean-type distribution. (Fpl_Distribution_Class_Discret_Mean) 

Following steps are performed during the generation process:
	- first the generation of the uniformly distributed variable with the random generator
	- second the transformation of the variable corresponding the distribution type (e.g. mean with log-normal or normal, triangle, discrete etc.

*/
class _Fpl_Distribution_Class
{

public:
	
	///Default constructor
	_Fpl_Distribution_Class(void);
	///Default destructor
	virtual ~_Fpl_Distribution_Class(void);

	//methods

	///Re-initialize the random generator; the seed value is always the same
	static void reinit_random(void);
	///Re-initialize the random generator with a given seed value
	static void reinit_random(const long int seed_val);

	///Set the parameter via console
	virtual void set_input(void)=0;
	///Set the parameter via database table for a user defined variable
	virtual void set_input(const int position, QSqlDatabase *ptr_database, const bool calculated_mean, const _fpl_boundaries boundaries)=0;
	///Set the parameter via database table for a default variable
	virtual void set_input(const string name, const string default_name, QSqlDatabase *ptr_database, const bool calculated_mean, const _fpl_boundaries boundaries)=0;
	///Set the parameter via file for a user defined variable
	virtual void set_input(QFile *ifile, int *line_counter, const bool calculated_mean, const _fpl_boundaries boundaries)=0;


	///Transfer the data of the distribution type to database table
	virtual void transfer_disttype2database(const int position, QSqlDatabase *ptr_database, const bool calculated_mean)=0;

	///Set the input of the distribution type via dialog for checking the random generator and the transformation
	virtual bool set_input_check(void);

	///Output the members of the distribution types to display/console
	virtual void output_member(bool detailed=true)=0; 
	
	///Reset the values for checking the statistics
	virtual void reset_statistic(void);
	///Check the statistic of the random generation process
	virtual void check_statistic(void);
	///Output the statistics to display/console; the statistics have to be checked before
	virtual void output_statistic(const bool calc_flag, bool detailed=true)=0;

	
	///Set a new middle for the distribution classes: mean, mid (triangle), middle (discret), mean_1 (mean_mean), middle (discret_mean)
	virtual void set_new_middle(const double new_middle)=0;
	///Return the last generated result without any generation
	_fpl_result get_result(void);
	///Set the random result
	void set_result(const _fpl_result new_res);

	///Get calculated exact probability for the distribution for the random generator check
	virtual double get_calulculated_exact_prob(const double value, const double stepsize);

	///Generate a random distributed variable
	virtual _fpl_result make_random(void)=0;
	///Generate a determinstic variable (mean, mid or a middle of the discret values)
	virtual _fpl_result make_deterministic(void)=0;

	///Set the boundaries for the result of the variable
	void set_boundaries(const double min, const double max);

	///Get the calculated mean value
	double get_calculated_mean(void);

	///Get mean distribution type as text
	virtual string get_mean_distribution_type(void);


	///Convert the distribution type (_fpl_distribution_type) to a string
	static string convert_dist_type2txt(_fpl_distribution_type type);
	///Convert a string to the distribution type (_fpl_distribution_type)
	static _fpl_distribution_type convert_txt2dist_type(const string txt);
	///Convert an integer to the distribution type (_fpl_distribution_type)
	static _fpl_distribution_type convert_int2dist_type(const int type);

	///Get the index of the result (relevant by discret and discret2mean distribution type)
	virtual int get_result_index(void);

	///Get the number of points (relevant by discret and discret2mean distribution type)
	virtual int get_number_point(void);
	///Get the value per given point index  (relevant by discret and discret2mean distribution type)
	virtual double get_point_value(const int index);

protected:
	//members
	///Result of the random generation and transformation process
	_fpl_result random_result;
	///Boundaries (min/max) of each random variable
	_fpl_boundaries boundaries;
	
	///Statistics of multiple the random generation and transformation process: mean value
	double mean_calc;
	///Statistics of multiple the random generation and transformation process: standard deviation
	double std_dev_calc;
	///Statistics of multiple the random generation and transformation process: minimum calculated
	double min_calc;
	///Statistics of multiple the random generation and transformation process: maximum calculated
	double max_calc;
	///Statistics of multiple the random generation and transformation process: number of generation steps
	int counter_calc;
	///Statistics of multiple the random generation and transformation process: mean value the generation step before
	double mean_calc_old;
	///Statistics of multiple the random generation and transformation process: variance
	double varianz_calc;
	///Calculated the mean of the mean value, in case of the calculation of the mean value in each generation
	double mid_calc;
	///Count the number, how often the mean value is set in case of the calculation of the mean value in each generation
	int counter_set_mid;

	///Description of the default distribution types
	string description;
	
	//methods
	///Generation of uniformly-distributed numbers between [0...1]
	double ran1(void);

private:
	//members
	_Fpl_Random_Generator *random_generator;
	
	//method
	///Allocate the random generator
	void allocate_random_generator(void);
	///Delete the random generator
	void delete_random_generator(void);

	///Set error(s)
	Error set_error(const int err_type);


};
#endif
