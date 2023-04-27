#pragma once
/**\class _Hyd_Model
	\author Daniel Bachmann et al.
	\author produced by the Institute of Hydraulic Engineering (IWW), RWTH Aachen University
	\version 0.0.1                                                              
	\date 2009 
*/
#ifndef _HYD_MODEL_H
#define _HYD_MODEL_H


//hyd_system classes
//container class the of global parameters
#include "Hyd_Param_Global.h" 
//class of Instationary boundary conditions
#include "Hyd_Instationary_Boundary.h"

//system_sys_ classes
#include "_Sys_Common_System.h"

//CVODE solver includes
//definitions of types realtype (set to double) and integertype (set to int), and the constant FALSE 
#include "sundials_types.h"  
//prototypes for CVodeMalloc, CVode, and CVodeFree,  constants OPT_SIZE, BDF, NEWTON, SV, SUCCESS, NST, NFE, NSETUPS, NNI, NCFN, NETF               
#include "cvode.h"  
// definitions of type N_Vector and macro NV_Ith_S, prototypes for N_VNew, N_VFree
#include "nvector_serial.h"  
 //definitions of type DenseMat, macro DENSE_ELEM  
#include "cvode_dense.h"           
#include "sundials_math.h"
//contains the enum for types of preconditioning
//Preconditioner Includes
#include "sundials_iterative.h"
//use CVSPGMR linear solver each internal step
#include "cvode_spgmr.h" 
//use CVSPTFQMR linear solver each internal step
#include "cvode_sptfqmr.h" 
//use CVSPBCGS linear solver each internal step
#include "cvode_spbcgs.h" 
//band preconditioner function prototypes
#include "cvode_bandpre.h"
#include "cvode_lapack.h"
#include "cvode_band.h"

//struct
///Summarizes the values for a hydrological balance (inflow-, outflow- and totalvolume) \ingroup hyd
struct _hyd_hydrological_balance{
	///Watervolume which flows into the element/segment 
	double volume_in;
	///Watervolume which flows out of the element/segment
	double volume_out;
	///Difference between inflow and outflow
	double volume_total;
};
    
///Base class for the models like river- or floodplain-models; here the solver (cvode) is initialized and solved \ingroup hyd
/**
The base class for the numerical model classes Hy_Model_River (RV) and Hyd_Model_Floodplain (FP)for instationary flow problems. 
These models are the main parts of a hydraulic system (Hyd_Hydraulic_System).
The solver cvode(see for documentation of solver cvode: https://computation.llnl.gov/casc/sundials/documentation/cv_guide.pdf) is 
initialized, the settings are made, and the solver is run to solve the problems (1d-hydraulic/2d-hydraulic).
*/
class _Hyd_Model : public _Sys_Common_System
{
public:
	///Default constructor
	_Hyd_Model(void);
	///Default destructor
	virtual ~_Hyd_Model(void);

	
	//members
	///Instationary boundary curves for river and floodplain models
	Hyd_Instationary_Boundary *instat_boundary_curves;

	//methods
	///Input the model members with the index per file with parser
	virtual void input_members(const string global_file, const int index, const string global_path)=0;
	///Transfer the model members to a database
	virtual void transfer_input_members2database(QSqlDatabase *ptr_database)=0;
	///Transfer a hydraulic boundary szenario from file to a database
	virtual void transfer_hydraulic_boundary_sz2database(QSqlDatabase *ptr_database)=0;
	///Input the relevant models and their members with the index form a database selection
	virtual void input_members(const int index, const QSqlTableModel *query_result, QSqlDatabase *ptr_database, const bool just_elems, const bool relevant, const bool output_flag)=0;


	///Initialize the solver with the given parameters
	virtual void init_solver(Hyd_Param_Global *global_params);

	///Initialize the solver with the given parameters for GPU calculation
	virtual void init_solver_gpu(Hyd_Param_Global *global_params);

	///Reset the solver-tolerances, if they are changed by the dynamic tolerance decreasment; also the warn_counter is reseted
	void reset_solver_tolerances(void);
	///Reinitialize the solver
	virtual void reinit_solver(Hyd_Param_Global *global_params);

	///Reset the model to the state before the calculation after the preproccesing
	virtual void reset_model(Hyd_Param_Global *global_params)=0;

	///Reset the solver of the model
	virtual void reset_solver(void)=0;

	///Clear the boundary condition, which are set; boundary conditions due to interception are not cleared
	virtual void clear_boundary_condition(const bool with_output=true)=0;


	///Get the warncounter for the solver warnings
	int get_warn_counter(void);

	///Set a new hydraulic boundary scenario id to the model
	virtual void set_new_hyd_bound_sz_id(Hyd_Boundary_Szenario new_sz)=0;

	///Solve the model
	virtual void solve_model(const double next_time_point,  const string system_id)=0;

	///Output final statistics of the river model
	virtual void output_final(void)=0;



	///Output the solver errors per internal timestep
	virtual void output_solver_errors(const double time_point, const int step_counter, const string timestring, const string realtime, const double diff_time, const int total_internal, const int internal_steps)=0;							

	///Set the folder for the file-output in case of an database based calculation
	virtual void set_output_folder(const string folder)=0;

	///Get the diferences of solver steps per interal time step
	int get_diff_solver_step_internal(void);



protected:
	
	// members
	///CVode memory Pointer for the solver data
	void *cvode_mem;	


	///N_Vector for the results;
	N_Vector results;

	///Pointer to the N_Vector to work with the data (it is connected to results)
	realtype *results_real;

	///Number of equation to solve
	int NEQ;
    ///Number of equation to solve in reality (without noflow etc)
    int NEQ_real;
    ///Reduced index reduced to the equation to solve
    int *id_reduced;
	///Half bandwidth of the system, which is to solve
	int half_bandwidth;
	///Reached time of the solver
	realtype reached_time;
	///Maximum value of the N_Vector of errors
	double max_estim_error;
	/// Minimum value of the N_Vector of errors
	double min_estim_error;
	///Norm of the N_Vector of errors
	double norm_estim_error;
	///Difference of solversteps per outputtime step
	int diff_solver_steps;
	///Old number of solver steps per internal time step
	int old_solver_steps;

	///Maximum solver step size
	double max_solver_step;

	///Setted absolute tolerances for the solver
	double setted_abs_tol;
	///Changed absolute tolerances for the solver
	double change_abs_tol;
	///Setted absolute tolerances for the solver
	double setted_rel_tol;
	///Changed relative tolerances for the solver
	double change_rel_tol;
	///Counter for warnings by solving the function
	int warn_counter;
	///Tolerance reduction counter
	int tol_reduce_counter;

	///Total volume left in the floodplain
	double total_watervolume;
	///Total watervolume set by the init-condition
	double total_watervolume_init;
	///Sum of volumes 
	double sum_volumes;
	///Error of the hydrological balance as the sum of volumes related to the sum of all absolute volumes
	double error_hydrological_balance;

	///Path of the output folder for the use of the models with the database
	string folder_output_db;

	///Output file for the file output of the model
	ofstream tecplot_output;
	///Output file for the file output of the model to bluekenue
    ofstream bluekenue_output;
    ///Output file for the file output of the model to bluekenue
    QFile bluekenue_output1;

	///Number of reseting the solver
	int no_reset;

    ///Number for preconditioner ilu (-2 1D; 3 2D)
     int ilu_number;

	//methods
	///Clone the model; the solver have to be initialised after
	void clone_model(_Hyd_Model *model);
	///Make the syncronisation between the models and the boundaries
	virtual void make_syncronisation(const double time_point)=0;

	///Run the solver
	void run_solver(const double next_time_point,  const string system_id);
	///Run the solver GPU
	void run_solver_gpu(const double next_time_point, const string system_id);

	//get the number of solver timesteps
	long int get_number_solversteps(void);

	///General output of the endstatistics of the solver
	void output_solver_statistics(ostringstream *cout);

	///Set the solver statistics before a solver reset
	void set_solver_statistics(void);

	///Output the hydrological balance of the floodplain model
	virtual void output_hydrological_balance(void)=0;
	///Calculate the hydrological balance of the floodplain model
	virtual void calculate_total_hydrological_balance(void)=0;

	///CVode-Solver init error-flags to string
	string cvode_init_flags2string(const int flag);
	///CVode-Solver solver error-flags to string
	string cvode_solv_flags2string(const int flag);
	///CVode-Linearsolver error-flags to string
	string cvode_linear_flags2string(const int flag);
	///Delete the instationary boundary curves
	void delete_instationary_boundary_curve(void);

	///Get the model description
	virtual string get_model_description(void)=0;

private:

	//members
	///Memory needed for the solver
    long long int solver_mem;
	///Number of solversteps per internal time step
	long int number_solversteps;

	///Number cvode function evaluation
	long int no_cvode;
	///Number SPGMR function evaluation
	long int no_spgmr;
	///Number Prec function evaluation
	long int no_prec;
	///Number Prec-solve function evaluation
	long int no_prec_solve;
	///Number of non-linear iterations
	long int no_non_lin_iters;
	///Number of non-linear convergence failure
	long int no_non_lin_failure;
	///Number of linear iterations
	long int no_lin_iters;
	///Number of linear convergence failure
	long int no_lin_failure;



	///Vector of the errors of the solver
	N_Vector estimated_error;
	///CVode memory pointer for the preconditioner data
	void *bp_data;



	//method
	///Set the init condition to the result vector of the solver
	virtual void set_initcond2resultvector(void)=0;
	///Set function to solve to the solver
	virtual void set_function2solver(void)=0;
	
	///Allocate the n_vectors of the results-data and linked realtype arrays results and estimated_error
	void allocate_solver_vector(void);
	///Calculate the errors of the solver per step
	void calculate_solver_errors(void);

	///Close the solver
	void close_solver(void);
	///Count solver memory
	void count_solver_memory(void);

	///Additional functions for N_Vector data type: Returns maximum value of a N_Vector
	realtype N_VMax(N_Vector x);

	///Set error(s)
	Error set_error(const int err_type);

	///Set warning(s)
	Warning set_warning(const int warn_type);

};
#endif
