//#include "_Hyd_Model.h"
#include "Hyd_Headers_Precompiled.h"

// constructor
_Hyd_Model::_Hyd_Model(void) {

	//solver data space
	this->cvode_mem = NULL;
	this->bp_data = NULL;
	this->NEQ=0;
    this->NEQ_real=0;
    this->id_reduced=NULL;
	this->half_bandwidth=0;
	this->number_solversteps=0;


	this->no_cvode=0;
	this->no_spgmr=0;
	this->no_prec=0;
	this->no_prec_solve=0;
	this->no_non_lin_iters=0;
	this->no_non_lin_failure=0;
	this->no_lin_iters=0;
	this->no_lin_failure=0;
	this->no_reset=0;

	this->solver_mem=0;
	this->reached_time=0.0;
	//result vector
	this->results=NULL;
	this->results_real=NULL;
	this->estimated_error=NULL;
	this->norm_estim_error=0.0;
	this->max_estim_error=0.0;
	this->min_estim_error=0.0;
	this->diff_solver_steps=0;
	this->old_solver_steps=0;
	this->max_solver_step=0.0;
	this->setted_abs_tol=0.0;
	this->setted_rel_tol=0.0;
	this->change_rel_tol=0.0;
	this->change_abs_tol=0.0;
    this->ilu_number=-2;
	

	this->total_watervolume=0.0;
	this->total_watervolume_init=0.0;
	this->sum_volumes=0.0;
	this->error_hydrological_balance=0.0;
	this->folder_output_db=label::not_set;

	this->warn_counter=0;
	this->tol_reduce_counter=0;
	
	this->instat_boundary_curves=NULL;

	this->gpu_in_use = false;
	this->pManager = nullptr;
}
//destructor
_Hyd_Model::~_Hyd_Model(void){

	this->delete_instationary_boundary_curve();

	//close the solver
	this->close_solver();
	if(this->tecplot_output.is_open()==true){
		this->tecplot_output.close();
	}
}
//___________________
//public
//initialize the solver with the given parameters
void _Hyd_Model::init_solver(Hyd_Param_Global *global_params){
	//documentation solver cvode https://computation.llnl.gov/casc/sundials/documentation/cv_guide.pdf
	int flag=-1;
	//allocate the solver vectors
	try{
		this->allocate_solver_vector();
	}
	catch(Error msg){
		throw msg;
	}
	//set the init condition
	//2DGPU
	this->set_initcond2resultvector();

	//allocate the solver object 
	this->cvode_mem = CVodeCreate(CV_BDF, CV_NEWTON);
	if(this->cvode_mem==NULL){//alloc was not successfull
		Error msg=this->set_error(0);
		throw msg;
	}

	//set the maximum order of bdf
	flag=CVodeSetMaxOrd(this->cvode_mem,3);
	if(flag<0){
		Error msg=this->set_error(1);
		ostringstream info;
		info <<"CVodeSetMaxOrd(this->cvode_mem,3)"<< endl;
		info <<"CVode: " << this->cvode_init_flags2string(flag) << endl;
		msg.make_second_info(info.str());
		throw msg;
	}

    //Set maximum number of nonlinear iteration steps (default 3/4)
    flag=CVodeSetMaxNonlinIters(this->cvode_mem, 4);
	if(flag<0){
		Error msg=this->set_error(1);
		ostringstream info;
		info <<"CVodeSetMaxNonlinIters(this->cvode_mem, 5)"<< endl;
		info <<"CVode: " << this->cvode_init_flags2string(flag) << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
	


	//set the function to the solver
	this->set_function2solver();

	//Set optional inputs
	//  Call CVodeSet* functions to change from their default values any optional inputs that control
	// the behavior of cvode. See x5.5.4 for details. 
	
	//Maximum no. of internal steps before tout CVodeSetMaxNumSteps
	flag = CVodeSetMaxNumSteps(this->cvode_mem, global_params->GlobMaxNumSteps);
	if(flag<0){
		Error msg=this->set_error(1);
		ostringstream info;
		info <<"Solver function: CVodeSetMaxNumSteps(this->cvode_mem, global_params->GlobMaxNumSteps)"<< endl;
		info <<"CVode: " << this->cvode_init_flags2string(flag) << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
    //nolinear solver safty factor (default 0.1/0.2)
    flag = CVodeSetNonlinConvCoef(this->cvode_mem, 0.1);
	if(flag<0){
		Error msg=this->set_error(1);
		ostringstream info;
        info <<"Solver function: CVodeSetNonlinConvCoef(this->cvode_mem, 0.1)"<< endl;
		info <<"CVode: " << this->cvode_init_flags2string(flag) << endl;
		msg.make_second_info(info.str());
		throw msg;
	}

	//Set the maximum of error test failure
	flag=CVodeSetMaxErrTestFails(this->cvode_mem, 10);
	if(flag<0){
		Error msg=this->set_error(1);
		ostringstream info;
		info <<"Solver function: CVodeSetMaxErrTestFails(this->cvode_mem, 10)"<< endl;
		info <<"CVode: " << this->cvode_init_flags2string(flag) << endl;
		msg.make_second_info(info.str());
		throw msg;
	}

	//Flag to activate stability limit detection  
	/*flag = CVodeSetStabLimDet(this->cvode_mem, FALSE);
	if(flag<0){
		Error msg=this->set_error(1);
		ostringstream info;
		info <<"Solver function: CVodeSetStabLimDet(this->cvode_mem, FALSE)"<< endl;
		info <<"CVode: " << this->cvode_init_flags2string(flag) << endl;
		msg.make_second_info(info.str());
		throw msg;
	}*/
	//Initial step size
	flag  = CVodeSetInitStep(this->cvode_mem, global_params->GlobInitStepSize);
	if(flag<0){
		Error msg=this->set_error(1);
		ostringstream info;
		info <<"Solver function: CVodeSetInitStep(this->cvode_mem, global_params->GlobInitStepSize)"<< endl;
		info <<"CVode: " << this->cvode_init_flags2string(flag) << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
	//Maximum absolute step size
	flag = CVodeSetMaxStep(this->cvode_mem, global_params->GlobMaxStepSize);
	if(flag<0){
		Error msg=this->set_error(1);
		ostringstream info;
		info <<"Solver function: CVodeSetMaxStep(this->cvode_mem, global_params->GlobMaxStepSize)"<< endl;
		info <<"CVode: " << this->cvode_init_flags2string(flag) << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
	
	this->max_solver_step=global_params->GlobMaxStepSize;
	


	//see the cvode manual to set other specifications
	//Set the linear solver
	//flag = CVSpgmr(this->cvode_mem, global_params->GlobPreType, constant::krylow_subspace);
    flag = CVSpgmr(this->cvode_mem, global_params->GlobPreType, 3);
    //flag = CVSpgmr(this->cvode_mem, PREC_NONE, 3);
	//other solver types
	//flag = CVSpbcg(this->cvode_mem, global_params->GlobPreType, 5);
	//flag = CVSptfqmr(this->cvode_mem, global_params->GlobPreType, 5);
    //flag = CVBand(this->cvode_mem, this->NEQ, this->half_bandwidth, this->half_bandwidth);
	if(flag<0){
		Error msg=this->set_error(3);
		ostringstream info;
		info <<"CVSpgmr(this->cvode_mem, global_params->GlobPreType, constant::krylow_subspace)"<< endl;
		info <<"CVode: " << this->cvode_linear_flags2string(flag) << endl;
		msg.make_second_info(info.str());
		throw msg;
	}



	//Initialize the cvbandpre preconditioner module; Specify the upper and lower half-bandwidths mu and ml and call
   flag=CVBandPrecInit(this->cvode_mem, this->NEQ_real, this->half_bandwidth, this->half_bandwidth, this->NEQ, this->id_reduced,this->ilu_number);
	if(flag<0){
		Error msg=this->set_error(3);
		ostringstream info;
		info <<"Solver function: CVBandPrecInit(this->cvode_mem, this->NEQ, this->half_bandwidth, this->half_bandwidth)"<< endl;
		info <<"CVode: " << this->cvode_linear_flags2string(flag) << endl;
		msg.make_second_info(info.str());
		throw msg;
	}


	//CVSpgmrSetGSType specifies the type of Gram-Schmidt orthogonalization to be used. This must be one of the two enumeration constants MODIFIED_GS or CLASSICAL_GS
    flag = CVSpilsSetGSType(this->cvode_mem, global_params->GlobGramSchmidt);
	if(flag<0){
		Error msg=this->set_error(3);
		ostringstream info;
		info <<"Solver function: CVSpgmrSetGSType(this->cvode_mem, global_params->GlobGramSchmidt)"<< endl;
		info <<"CVode: " << this->cvode_linear_flags2string(flag) << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
	//Set the relation between linear and nonlinear error weights (default 0.05);0,35
    flag=CVSpilsSetEpsLin(this->cvode_mem, 0.05);
	if(flag<0){
		Error msg=this->set_error(3);
		ostringstream info;
		info <<"Solver function: CVSpilsSetEpsLin(this->cvode_mem, 0.10)"<< endl;
		info <<"CVode: " << this->cvode_linear_flags2string(flag) << endl;
		msg.make_second_info(info.str());
		throw msg;
	}

	//count the required memory for the solver
	this->count_solver_memory();
}
//Initialize the solver with the given parameters for GPU calculation
void _Hyd_Model::init_solver_gpu(Hyd_Param_Global* global_params) {

	this->gpu_in_use = true;
	Hyd_Model_Floodplain* myFloodplain = (Hyd_Model_Floodplain*) this;

	//Collect Floodplain Information
	_hyd_floodplain_scheme_info scheme_info = myFloodplain->Param_FP.get_scheme_info();
	double outputFrequency = global_params->GlobTStep / global_params->GlobNofITS;
	double simulationLength = global_params->GlobTNof * global_params->GlobTStep;

	//Create the GPU model with our own logger attached
	Hyd_SolverGPU_LoggingWrapper* hyd_SolverGPU_LoggingWrapper = new Hyd_SolverGPU_LoggingWrapper(); //Deleted by pManager destructor
	pManager = new CModel(hyd_SolverGPU_LoggingWrapper, false); //Deleted by _Hyd_Model destructor

	//Set up the Manager Settings
	pManager->setSelectedDevice(scheme_info.selected_device);							// Set GPU device to Use. Important: Has to be called after setExecutor. Default is the faster one.
	pManager->setSimulationLength(simulationLength);									// Set Simulation Length
	pManager->setOutputFrequency(outputFrequency);										// Set Output Frequency
	pManager->setFloatPrecision(model::floatPrecision::kDouble);						// Set Precision
	
	//Create the domain
	CDomainCartesian* ourCartesianDomain = pManager->getDomain();
	ourCartesianDomain->setCellResolution(*myFloodplain->Param_FP.get_ptr_width_x(), *myFloodplain->Param_FP.get_ptr_width_y());
	ourCartesianDomain->setCols(myFloodplain->Param_FP.get_no_elems_x());
	ourCartesianDomain->setRows(myFloodplain->Param_FP.get_no_elems_y());
	ourCartesianDomain->setUseOptimizedCoupling(myFloodplain->get_number_boundary_conditions() == 0 && myFloodplain->get_number_coupling_conditions() > 0 );
	ourCartesianDomain->setOptimizedCouplingSize(myFloodplain->get_number_coupling_conditions());
	ourCartesianDomain->setName(myFloodplain->Param_FP.get_floodplain_name());
	
	//Create the Scheme,
	model::SchemeSettings schemeSettings;
	schemeSettings.scheme_type = scheme_info.scheme_type;
	schemeSettings.CourantNumber = scheme_info.courant_number;
	schemeSettings.DryThreshold = 1E-10;
	schemeSettings.Timestep = 0.1;
	schemeSettings.ReductionWavefronts = scheme_info.reduction_wavefronts;
	schemeSettings.FrictionStatus = scheme_info.friction_status;
	schemeSettings.NonCachedWorkgroupSize[0] = scheme_info.workgroup_size_x;
	schemeSettings.NonCachedWorkgroupSize[1] = scheme_info.workgroup_size_y;
	schemeSettings.debuggerOn = false;
	CScheme::createScheme(pManager, schemeSettings);


	pManager->log->logInfo("Setting Data...");
	unsigned long ulCellID;
	unsigned char	ucRounding = 6;
	for (unsigned long iRow = 0; iRow < myFloodplain->Param_FP.get_no_elems_y(); iRow++) {
		for (unsigned long iCol = 0; iCol < myFloodplain->Param_FP.get_no_elems_x(); iCol++) {
			ulCellID = ourCartesianDomain->getCellID(iCol, ourCartesianDomain->getRows() - iRow - 1);
			//Elevations
			if (myFloodplain->floodplain_elems[ulCellID].get_elem_type() == _hyd_elem_type::STANDARD_ELEM || myFloodplain->floodplain_elems[ulCellID].get_elem_type() == _hyd_elem_type::DIKELINE_ELEM) {
				//Bed Elevation
				ourCartesianDomain->setBedElevation(ulCellID, myFloodplain->floodplain_elems[ulCellID].get_z_value());
			}else {
				//Bed Elevation
				ourCartesianDomain->setBedElevation(ulCellID, -9999.0);
			}
			//Manning Coefficient
			ourCartesianDomain->setManningCoefficient(ulCellID, myFloodplain->floodplain_elems[ulCellID].element_type->get_flow_data().n_value);
			//Depth
			ourCartesianDomain->setFSL(ulCellID, myFloodplain->floodplain_elems[ulCellID].element_type->get_flow_data().init_condition + myFloodplain->floodplain_elems[ulCellID].get_z_value());
			//MaxDepth
			ourCartesianDomain->setMaxFSL(ulCellID, myFloodplain->floodplain_elems[ulCellID].element_type->get_flow_data().init_condition + myFloodplain->floodplain_elems[ulCellID].get_z_value());
			//VelocityX
			ourCartesianDomain->setDischargeX(ulCellID, 0.0);
			//VelocityY
			ourCartesianDomain->setDischargeY(ulCellID, 0.0);
			//Boundary Condition
			if (ourCartesianDomain->getUseOptimizedCoupling() == false) {
				ourCartesianDomain->setBoundaryCondition(ulCellID, 0.0);
			}
			//Poleni Conditions
			if (!myFloodplain->floodplain_elems[ulCellID].element_type->get_flow_data().no_flow_x_flag) {
				ourCartesianDomain->setPoleniConditionX(ulCellID, myFloodplain->floodplain_elems[ulCellID].element_type->get_flow_data().poleni_flag_x);
			}
			//Poleni Conditions
			if (!myFloodplain->floodplain_elems[ulCellID].element_type->get_flow_data().no_flow_y_flag) {
				ourCartesianDomain->setPoleniConditionY(ulCellID, myFloodplain->floodplain_elems[ulCellID].element_type->get_flow_data().poleni_flag_y);
			}
			//Zxmax
			ourCartesianDomain->setZxmax(ulCellID, myFloodplain->floodplain_elems[ulCellID].get_flow_data().height_border_x_abs);
			//cx
			ourCartesianDomain->setcx(ulCellID, myFloodplain->floodplain_elems[ulCellID].get_flow_data().poleni_x);
			//Zymax
			ourCartesianDomain->setZymax(ulCellID, myFloodplain->floodplain_elems[ulCellID].get_flow_data().height_border_y_abs);
			//cy
			ourCartesianDomain->setcy(ulCellID, myFloodplain->floodplain_elems[ulCellID].get_flow_data().poleni_y);
			//Coupling Condition
			//ourCartesianDomain->setCouplingCondition(ulCellID, 0.0);

		}
	}
	if (ourCartesianDomain->getUseOptimizedCoupling() ) {
		//set id array
		for (int i = 0; i < ourCartesianDomain->getOptimizedCouplingSize(); i++) {
			ourCartesianDomain->setOptimizedCouplingCondition(i, 0.0);
			ourCartesianDomain->setOptimizedCouplingID(i, myFloodplain->get_optimized_coupling_id(i));
		}
	}

	pManager->log->logInfo("The computational engine is now ready.");

	pManager->ValidateAndPrepareModel();

	//TODO: Alaa: Calculate memory     this->count_solver_memory();
}
//Reset the solver-tolerances, if they are changed by the dynamic tolerance decreasment; also the warn_counter is reseted
void _Hyd_Model::reset_solver_tolerances(void){
	//if the tolerances are changed, then they are reseted here
	if(this->tol_reduce_counter>0){
		this->reset_solver();
		this->change_rel_tol=this->setted_rel_tol;
		this->change_abs_tol=this->setted_abs_tol;
		//CVodeSetTolerances(this->cvode_mem, CV_SS, this->setted_rel_tol,this->ptr_setted_abstol_model);
		CVode_change_SStolerances(this->cvode_mem, this->setted_rel_tol,this->setted_abs_tol);
		this->tol_reduce_counter=0;
	}
}
//Reinitialize the solver
void _Hyd_Model::reinit_solver(Hyd_Param_Global *global_params){
	this->close_solver();
    //CVodeReInit(this->cvode_mem,0.0, this->results);
	this->diff_solver_steps=0;
	this->old_solver_steps=0;
	
	this->number_solversteps=0;
	this->no_cvode=0;
	this->no_spgmr=0;
	this->no_prec=0;
	this->no_prec_solve=0;
	this->no_non_lin_iters=0;
	this->no_non_lin_failure=0;
	this->no_lin_iters=0;
	this->no_lin_failure=0;
	this->no_reset=0;

	this->init_solver(global_params);

}
///Get the warncounter for the solver warnings
int _Hyd_Model::get_warn_counter(void){
	return this->warn_counter;
}
//Get the diferences of solver steps per interal time step
int _Hyd_Model::get_diff_solver_step_internal(void){
	int buffer=this->get_number_solversteps()-this->old_solver_steps;
	this->old_solver_steps=this->get_number_solversteps();
	return buffer;
}
//_________________
//protected
//Clone the model; the solver have to be initialised after
void _Hyd_Model::clone_model(_Hyd_Model *model){

	this->NEQ=model->NEQ;
    this->NEQ_real=model->NEQ_real;
	this->half_bandwidth=model->half_bandwidth;

	this->number_solversteps=model->number_solversteps;
	this->no_cvode=model->no_cvode;
	this->no_spgmr=model->no_spgmr;
	this->no_prec=model->no_prec;
	this->no_prec_solve=model->no_prec_solve;
	this->no_non_lin_iters=model->no_non_lin_iters;
	this->no_non_lin_failure=model->no_non_lin_failure;
	this->no_lin_iters=model->no_lin_iters;
	this->no_lin_failure=model->no_lin_failure;
	this->no_reset=model->no_reset;


	this->solver_mem=model->solver_mem;
	this->reached_time=model->reached_time;
	this->norm_estim_error=model->norm_estim_error;
	this->max_estim_error=model->max_estim_error;
	this->min_estim_error=model->min_estim_error;

	this->setted_abs_tol=model->setted_abs_tol;
	this->setted_rel_tol=model->setted_rel_tol;
	this->change_rel_tol=model->change_rel_tol;
	this->change_abs_tol=model->change_abs_tol;

	this->total_watervolume=model->total_watervolume;
	this->total_watervolume_init=model->total_watervolume_init;
	this->sum_volumes=model->sum_volumes;
	this->error_hydrological_balance=model->error_hydrological_balance;
	this->folder_output_db=model->folder_output_db;

	this->warn_counter=model->warn_counter;
	this->tol_reduce_counter=model->tol_reduce_counter;
	
	
}
//run the solver
void _Hyd_Model::run_solver(const double next_time_point, const string system_id){

	//documentation solver cvode https://computation.llnl.gov/casc/sundials/documentation/cv_guide.pdf
	int okflag=-1;

	int converg_failure=0;
	//The function CVode integrates the ODE over an interval in t.
	okflag=CVodeSetStopTime(this->cvode_mem, next_time_point);
	if(okflag<0){
		Error msg=this->set_error(1);
		ostringstream info;
		info <<"CVodeSetStopTime(this->cvode_mem, next_time_point)"<< endl;
		info <<"CVode: " << this->cvode_init_flags2string(okflag) << endl;
		msg.make_second_info(info.str());
		throw msg;
	}

	do{
		//ostringstream out;
		//out << "   RV " << next_time_point << endl;
		//Sys_Common_Output::output_hyd->output_txt(&out);

		okflag=CVode(this->cvode_mem, next_time_point, this->results, &this->reached_time, CV_NORMAL);
		//out << "   RV solve finsihed "  <<okflag<< endl;
		//Sys_Common_Output::output_hyd->output_txt(&out);

		//Error
		if(okflag<-4){
			Error msg=this->set_error(6);
			ostringstream info;
			info <<"Hydraulic system                    : "<<system_id << endl;
			info << this->get_model_description();
			info <<"CVode                               : " << this->cvode_solv_flags2string(okflag) << endl;
			info<< "Time to reached                     : " <<next_time_point << label::sec <<endl;
			info<< "Time reached                        : " << this->reached_time<< label::sec<<endl;
			msg.make_second_info(info.str());
			throw msg;
		}
		
		//warning to much work/to much accurancy
		if(okflag>-3 && okflag<0 && this->tol_reduce_counter<3){
			Warning msg=this->set_warning(4);
			this->warn_counter++;
			this->tol_reduce_counter++;
			ostringstream info;
			info <<"Hydraulic system                    : "<<system_id << endl;
			info << this->get_model_description();
			info <<"CVode                               : " << this->cvode_solv_flags2string(okflag) << endl;
			info<< "Time to reached                     : "<<next_time_point << label::sec <<endl;
			info<< "Time reached                        : " << this->reached_time<< label::sec<<endl;
			info<< "Number of tolerance reduction       : "<< this->tol_reduce_counter<<endl;  

			//reaction: increase the tolerances automatically
			this->change_rel_tol=this->setted_rel_tol*pow(10.0,this->tol_reduce_counter)*0.25;
			//set a boundary to rel_tol
            if(this->change_rel_tol>=1e-3){
                this->change_rel_tol=1e-3;
			}
			this->change_abs_tol=this->setted_abs_tol*pow(10.0,this->tol_reduce_counter)*0.25;
			//set a boundary to abs_tol
            if(this->change_abs_tol>=1e-2){
                this->change_abs_tol=1e-2;
			}
			//set the new tolerances to the solver
			CVode_change_SStolerances(this->cvode_mem, this->change_rel_tol,this->change_abs_tol);
			info<< "New relative tolerance              : "<< this->change_rel_tol << endl;
			info<< "New absolute tolerance              : "<< this->change_abs_tol <<endl;
			msg.make_second_info(info.str());
			msg.output_msg(2);
		}
		else if(okflag>-3 && okflag<0 && this->tol_reduce_counter>=3){
			Error msg=this->set_error(5);
			ostringstream info;
			info <<"Hydraulic system                    : "<<system_id << endl;
			info << this->get_model_description();
			info <<"CVode                               : " << this->cvode_solv_flags2string(okflag) << endl;
			info<< "Time to reached                     : "<<next_time_point << label::sec <<endl;
			info<< "Time reached                        : " << this->reached_time<< label::sec<<endl;
			info<< "Number of tolerance reduction       : "<< this->tol_reduce_counter<<endl;  
			msg.make_second_info(info.str());
			throw msg;
		}
		if(okflag<=-3 && okflag>=-4 && converg_failure==0){
			converg_failure++;
			//Warning msg=this->set_warning(5);
			//this->warn_counter++;
			//ostringstream info;
			//info <<"Hydraulic system                    : "<<system_id << endl;
			//info << this->get_model_description();
			//info <<"CVode                               : " << this->cvode_solv_flags2string(okflag) << endl;
			//msg.make_second_info(info.str());
			//msg.output_msg(2);
		}
		else if(okflag<=-3 && okflag>=-4 && converg_failure>0){
			Error msg=this->set_error(7);
			ostringstream info;
			info <<"Hydraulic system                    : "<<system_id << endl;
			info << this->get_model_description();
			info <<"CVode                               : " << this->cvode_solv_flags2string(okflag) << endl;
			info<< "Time to reached                     : "<< next_time_point << label::sec <<endl;
			info<< "Time reached                        : " << this->reached_time<< label::sec<<endl;
			msg.make_second_info(info.str());
			throw msg;
		}

		Hyd_Multiple_Hydraulic_Systems::check_stop_thread_flag();
	}
	while(okflag<0);

		
	this->calculate_solver_errors();
}
//get the number of solver timesteps
long int _Hyd_Model::get_number_solversteps(void){
	//calculate them
	int flag=-1;
	long int buff=0;
	if (this->gpu_in_use) {
		//TODO: Alaa Get Gpu Timesteps
	}
	else {
	flag=CVodeGetNumSteps(this->cvode_mem, &buff);
	 if(flag<0){
		Warning msg=this->set_warning(3);
		ostringstream info;
		info <<"Solver function: CVodeGetNumSteps(this->cvode_mem, &this->number_solversteps)"<< endl;
		info <<"CVode: " << this->cvode_init_flags2string(flag) << endl;
		msg.make_second_info(info.str());
		msg.output_msg(2);
	}

	}
	return this->number_solversteps+buff;
}
//output final statistics of the solver
void _Hyd_Model::output_solver_statistics(ostringstream *cout){
	long int nst, nfe, nni, ncfn;
	long int nli, ncfl, nfeSPGMR;
	long int n_pre=0;
	long int n_pre_solv=0;
	//long int bdf_order_reduction=0;
	//realtype suggested_scale_factor=0.0;
	//long int no_error_test_fails=0;
	int flag=-1;

	flag = CVodeGetNumSteps(cvode_mem, &nst);
 	if(flag<0){
		Warning msg=this->set_warning(2);
		ostringstream info;
		info <<"Solver function: CVodeGetNumSteps(cvode_mem, &nst)"<< endl;
		info <<"CVode: " << this->cvode_init_flags2string(flag) << endl;
		msg.make_second_info(info.str());
		msg.output_msg(2);
	}
	this->number_solversteps=this->number_solversteps+nst;
	*cout << " Number of timesteps                      :"<< P(0) << W(15) << this->number_solversteps << endl;
	*cout << " Number of reset                          :"<< P(0) << W(15) << this->no_reset << endl;


	flag = CVodeGetNumRhsEvals(cvode_mem, &nfe);
	 if(flag<0){
		Warning msg=this->set_warning(2);
		ostringstream info;
		info <<"Solver function: CVodeGetNumRhsEvals(cvode_mem, &nfe)"<< endl;
		info <<"CVode: " << this->cvode_init_flags2string(flag) << endl;
		msg.make_second_info(info.str());
		msg.output_msg(2);
	}
	 this->no_cvode=this->no_cvode+nfe;
	*cout << " Number of function evaluations (CVode)   :" << P(0) << W(15) << this->no_cvode << endl;
	Sys_Common_Output::output_hyd->output_txt(cout);


	flag = CVSpilsGetNumRhsEvals(cvode_mem, &nfeSPGMR);
	 if(flag<0){
		Warning msg=this->set_warning(2);
		ostringstream info;
		info <<"Solver function: CVSpgmrGetNumRhsEvals(cvode_mem, &nfeSPGMR)"<< endl;
		info <<"CVode: " << this->cvode_init_flags2string(flag) << endl;
		msg.make_second_info(info.str());
		msg.output_msg(2);
	}
	 this->no_spgmr=this->no_spgmr+nfeSPGMR;
	 *cout << " Number of function evaluations (Spgmr)   :"<< P(0) << W(15) << this->no_spgmr << endl;
	 Sys_Common_Output::output_hyd->output_txt(cout);


	flag=CVSpilsGetNumPrecEvals(this->cvode_mem, &n_pre);
	if(flag<0){
		Warning msg=this->set_warning(2);
		ostringstream info;
		info <<"CVSpgmrGetNumPrecEvals(this->bp_data, &n_pre)"<< endl;
		info <<"CVode: " << this->cvode_init_flags2string(flag) << endl;
		msg.make_second_info(info.str());
		msg.output_msg(2);
	}
	this->no_prec=this->no_prec+n_pre;
	*cout << " Number of function evaluations (Prec)    :"<< P(0) << W(15) << this->no_prec << endl;
	Sys_Common_Output::output_hyd->output_txt(cout);


	flag=CVSpilsGetNumPrecSolves(this->cvode_mem, &n_pre_solv);
	if(flag<0){
		Warning msg=this->set_warning(2);
		ostringstream info;
		info <<"CVSpgmrGetNumPrecSolves(this->bp_data, &n_pre_solv)"<< endl;
		info <<"CVode: " << this->cvode_init_flags2string(flag) << endl;
		msg.make_second_info(info.str());
		msg.output_msg(2);
	}
	this->no_prec_solve=this->no_prec_solve+n_pre_solv;
	*cout << " Number of function evaluations (Precsolv):"<< P(0) << W(15) << this->no_prec_solve << endl;
	Sys_Common_Output::output_hyd->output_txt(cout);



	flag = CVodeGetNumNonlinSolvIters(cvode_mem, &nni);
	 if(flag<0){
		Warning msg=this->set_warning(2);
		ostringstream info;
		info <<"Solver function: CVodeGetNumNonlinSolvIters(cvode_mem, &nni)"<< endl;
		info <<"CVode: " << this->cvode_init_flags2string(flag) << endl;
		msg.make_second_info(info.str());
		msg.output_msg(2);
	}
	 this->no_non_lin_iters=this->no_non_lin_iters+nni;
	 *cout << " Number of nonlinear iterations           :"<< P(0) << W(15) << this->no_non_lin_iters << endl;
	 Sys_Common_Output::output_hyd->output_txt(cout,true);

	flag = CVodeGetNumNonlinSolvConvFails(cvode_mem, &ncfn);
	 if(flag<0){
		Warning msg=this->set_warning(2);
		ostringstream info;
		info <<"Solver function: CVodeGetNumNonlinSolvConvFails(cvode_mem, &ncfn)"<< endl;
		info <<"CVode: " << this->cvode_init_flags2string(flag) << endl;
		msg.make_second_info(info.str());
		msg.output_msg(2);
	}
	 this->no_non_lin_failure=this->no_non_lin_failure+ncfn;
	 *cout << " Number of nonlinear convergence failures :"<< P(0) << W(15) << this->no_non_lin_failure << endl;
	  Sys_Common_Output::output_hyd->output_txt(cout,true);

	  flag = CVSpilsGetNumLinIters(cvode_mem, &nli);
	 if(flag<0){
		Warning msg=this->set_warning(2);
		ostringstream info;
		info <<"Solver function: CVSpgmrGetNumLinIters(cvode_mem, &nli)"<< endl;
		info <<"CVode: " << this->cvode_init_flags2string(flag) << endl;
		msg.make_second_info(info.str());
		msg.output_msg(2);
	}
	 this->no_lin_iters=this->no_lin_iters+nli;
	 *cout << " Number of linear iterations              :"<< P(0) << W(15) << this->no_lin_iters << endl;
	 Sys_Common_Output::output_hyd->output_txt(cout,true);
	
	flag = CVSpilsGetNumConvFails(cvode_mem, &ncfl);
	 if(flag<0){
		Warning msg=this->set_warning(2);
		ostringstream info;
		info <<"Solver function: CVSpgmrGetNumConvFails(cvode_mem, &ncfl)"<< endl;
		info <<"CVode: " << this->cvode_init_flags2string(flag) << endl;
		msg.make_second_info(info.str());
		msg.output_msg(2);
	}
	 this->no_lin_failure=this->no_lin_failure+ncfl;
	 *cout << " Number of linear convergence failures    :"<< P(0) << W(15) << this->no_lin_failure << endl;
	 Sys_Common_Output::output_hyd->output_txt(cout,true);
	 
}
//Set the solver statistics before a solver reset
void _Hyd_Model::set_solver_statistics(void){
	long int nst, nfe, nni, ncfn;

	int flag=-1;

	flag = CVodeGetNumSteps(cvode_mem, &nst);
 	if(flag<0){
		Warning msg=this->set_warning(2);
		ostringstream info;
		info <<"Solver function: CVodeGetNumSteps(cvode_mem, &nst)"<< endl;
		info <<"CVode: " << this->cvode_init_flags2string(flag) << endl;
		msg.make_second_info(info.str());
		msg.output_msg(2);
	}
	this->number_solversteps=this->number_solversteps+nst;

	
	flag = CVodeGetNumRhsEvals(cvode_mem, &nfe);
	 if(flag<0){
		Warning msg=this->set_warning(2);
		ostringstream info;
		info <<"Solver function: CVodeGetNumRhsEvals(cvode_mem, &nfe)"<< endl;
		info <<"CVode: " << this->cvode_init_flags2string(flag) << endl;
		msg.make_second_info(info.str());
		msg.output_msg(2);
	}
	 this->no_cvode=this->no_cvode+nfe;



    //flag = CVSpilsGetNumRhsEvals(cvode_mem, &nfeSPGMR);
    // if(flag<0){
    //	Warning msg=this->set_warning(2);
    //	ostringstream info;
    //	info <<"Solver function: CVSpgmrGetNumRhsEvals(cvode_mem, &nfeSPGMR)"<< endl;
    //	info <<"CVode: " << this->cvode_init_flags2string(flag) << endl;
    //	msg.make_second_info(info.str());
    //	msg.output_msg(2);
    //}
    // this->no_spgmr=this->no_spgmr+nfeSPGMR;



    //flag=CVSpilsGetNumPrecEvals(this->cvode_mem, &n_pre);
    //if(flag<0){
    //	Warning msg=this->set_warning(2);
    //	ostringstream info;
    //	info <<"CVSpgmrGetNumPrecEvals(this->bp_data, &n_pre)"<< endl;
    //	info <<"CVode: " << this->cvode_init_flags2string(flag) << endl;
    //	msg.make_second_info(info.str());
    //	msg.output_msg(2);
    //}
    //this->no_prec=this->no_prec+n_pre;



    //flag=CVSpilsGetNumPrecSolves(this->cvode_mem, &n_pre_solv);
    //if(flag<0){
    //	Warning msg=this->set_warning(2);
    //	ostringstream info;
    //	info <<"CVSpgmrGetNumPrecSolves(this->bp_data, &n_pre_solv)"<< endl;
    //	info <<"CVode: " << this->cvode_init_flags2string(flag) << endl;
    //	msg.make_second_info(info.str());
    //	msg.output_msg(2);
    //}
    //this->no_prec_solve=this->no_prec_solve+n_pre_solv;




	flag = CVodeGetNumNonlinSolvIters(cvode_mem, &nni);
	 if(flag<0){
		Warning msg=this->set_warning(2);
		ostringstream info;
		info <<"Solver function: CVodeGetNumNonlinSolvIters(cvode_mem, &nni)"<< endl;
		info <<"CVode: " << this->cvode_init_flags2string(flag) << endl;
		msg.make_second_info(info.str());
		msg.output_msg(2);
	}
	 this->no_non_lin_iters=this->no_non_lin_iters+nni;


	flag = CVodeGetNumNonlinSolvConvFails(cvode_mem, &ncfn);
	 if(flag<0){
		Warning msg=this->set_warning(2);
		ostringstream info;
		info <<"Solver function: CVodeGetNumNonlinSolvConvFails(cvode_mem, &ncfn)"<< endl;
		info <<"CVode: " << this->cvode_init_flags2string(flag) << endl;
		msg.make_second_info(info.str());
		msg.output_msg(2);
	}
	 this->no_non_lin_failure=this->no_non_lin_failure+ncfn;


	//  flag = CVSpilsGetNumLinIters(cvode_mem, &nli);
	// if(flag<0){
	//	Warning msg=this->set_warning(2);
	//	ostringstream info;
	//	info <<"Solver function: CVSpgmrGetNumLinIters(cvode_mem, &nli)"<< endl;
	//	info <<"CVode: " << this->cvode_init_flags2string(flag) << endl;
	//	msg.make_second_info(info.str());
	//	msg.output_msg(2);
	//}
	// this->no_lin_iters=this->no_lin_iters+nli;

	//
	//flag = CVSpilsGetNumConvFails(cvode_mem, &ncfl);
	// if(flag<0){
	//	Warning msg=this->set_warning(2);
	//	ostringstream info;
	//	info <<"Solver function: CVSpgmrGetNumConvFails(cvode_mem, &ncfl)"<< endl;
	//	info <<"CVode: " << this->cvode_init_flags2string(flag) << endl;
	//	msg.make_second_info(info.str());
	//	msg.output_msg(2);
	//}
	// this->no_lin_failure=this->no_lin_failure+ncfl;

}
//CVode Solver init error-flags to string
string _Hyd_Model::cvode_init_flags2string(const int flag){
	string txt="CVODE_init: ";
	switch(flag){
		case (-1):
			txt.append("The cvode mem argument was NULL");
			break;
		case (-2):
			txt.append("One of the inputs to CVode is illegal");
			break;
		case (-3):
			txt.append("The cvode memory was not allocated by a call to CVodeMalloc");
			break;
		default:
			txt.append("Unknown solver error/warning");
			break;
	}
	return txt;
}
//CVode-Solver solver error-flags to string
string _Hyd_Model::cvode_solv_flags2string(const int flag){
	string txt="CVODE_solv: ";
	switch(flag){
		case (-1):
			txt.append("The solver took mxstep internal steps but could not reach tout");
			break;
		case (-2):
			txt.append("The solver could not satisfy the accuracy demanded by the user for some internal step");
			break;
		case (-3):
			txt.append("Error test failures occurred too many times during one internal time step or occurred with |h| = hmin");
			break;
		case (-4):
			txt.append("Convergence test failures occurred too many times during one internal time step or occurred with |h| = hmin");
			break;
		case (-5):
			txt.append("The linear solver's initialization function failed");
			break;
		case (-6):
			txt.append("The linear solver's setup function failed in an unrecoverable manner");
			break;
		case (-7):
			txt.append("The linear solver's solve function failed in an unrecoverable manner");
			break;
		default:
			txt.append("Unknown solver error/warning");
			break;
	}
	return txt;
}
//CVode-Linearsolver error-flags to string
string _Hyd_Model::cvode_linear_flags2string(const int flag){
string txt="CVBPSpgmr: ";
	switch(flag){
		case (-1):
			txt.append("The cvode mem argument was NULL");
			break;
		case (-2):
			txt.append("The cv linear solver has not been initialized");
			break;
		case (-3):
			txt.append("One of the inputs to CVBPSpgmr is illegal");
			break;
		case (-4):
			txt.append("Allocation failure of the preconditioner");
			break;
		default:
			txt.append("Unknown solver error/warning");
			break;
	}
	return txt;
}
//Delete the instationary boundary curves
void _Hyd_Model::delete_instationary_boundary_curve(void){
	if(this->instat_boundary_curves!=NULL){
		delete [] this->instat_boundary_curves;
		this->instat_boundary_curves=NULL;
	}

}
//_________________
//private
//allocate the n_vectors link results and estimated_error
void _Hyd_Model::allocate_solver_vector(void){

	//allocate the result vector
    this->results=N_VNew_Serial(this->NEQ_real);
	if(this->results==NULL){
		Error msg=this->set_error(4);
		throw msg;
	}
	//connect the results-nvector with the real results
	this->results_real=NV_DATA_S(this->results);
	// Initialize result vector with zero with zero
	N_VConst(0.0, this->results);


	//allocate the vector for the estimated errors
    this->estimated_error=N_VNew_Serial(this->NEQ_real);
	// Initialize error vector with zero with zero
	N_VConst(0.0, this->estimated_error);
	if(this->estimated_error==NULL){
		Error msg=this->set_error(4);
		throw msg;
	}
}
//calculate the errors
void _Hyd_Model::calculate_solver_errors(void){
	int flag=-1;
	flag = CVodeGetEstLocalErrors(this->cvode_mem, this->estimated_error);
	if(flag<0){
		Warning msg=this->set_warning(0);
		ostringstream info;
		info <<"Solver function: CVodeGetEstLocalErrors(this->cvode_mem, this->estimated_error)"<< endl;
		info <<"CVode: " << this->cvode_init_flags2string(flag) << endl;
		msg.make_second_info(info.str());
		msg.output_msg(2);
	}
	//get the norm of the vector
	this->norm_estim_error=N_VL1Norm(this->estimated_error);
	//make the all values positiv
	N_VAbs(this->estimated_error, this->estimated_error);
	//get the max value
	this->max_estim_error=N_VMax(this->estimated_error);
	//get the minvalue
	this->min_estim_error=N_VMin(this->estimated_error);
}
//close the solver
void _Hyd_Model::close_solver(void){
	//For Cvode
	// Deallocate memory for solution vector 
	if(this->results!=NULL){
		N_VDestroy_Serial(this->results);
		this->results=NULL;
		this->results_real=NULL;
	}
	// Deallocate memory for solution vector 
	if(this->estimated_error!=NULL){
		N_VDestroy_Serial(this->estimated_error);
		this->estimated_error=NULL;
	}

	//For GPU Solver
	if (this->pManager != NULL)
		delete this->pManager;
	
	//Free solver memory: CVodeFree(cvode mem); to free the memory allocated for cvode.
	CVodeFree(&this->cvode_mem);

	//count the memory
	Sys_Memory_Count::self()->minus_mem(this->solver_mem, _sys_system_modules::HYD_SYS);
}
//count solver memory
void _Hyd_Model::count_solver_memory(void){
    long int number_real=0;
    long int number_int=0;
	this->solver_mem=0;
	int flag=-1;


	if(this->NEQ>0){
		// Calculate CVODE Memory
		//The function *GetWorkSpace returns the cvode, CVSpgmr, CVBandPrec integer and real workspace sizes
		flag=CVodeGetWorkSpace(this->cvode_mem, &number_real, &number_int);
		number_real=number_real;
		number_int=number_int;

		if(flag<0){
			Warning msg=this->set_warning(0);
			ostringstream info;
			info <<"Solver function: CVodeGetWorkSpace(this->cvode_mem, &number_real, &number_int)"<< endl;
			info <<"CVode: " << this->cvode_init_flags2string(flag) << endl;
			msg.make_second_info(info.str());
			msg.output_msg(2);
		}
		this->solver_mem = number_real * sizeof(realtype) + number_int * sizeof(int);
        flag=CVSpilsGetWorkSpace(this->cvode_mem, &number_real, &number_int);
		if(flag<0){
			Warning msg=this->set_warning(0);
			ostringstream info;
			info <<"Solver function: CVSpgmrGetWorkSpace(this->cvode_mem, &number_real, &number_int)"<< endl;
			info <<"CVode: " << this->cvode_init_flags2string(flag) << endl;
			msg.make_second_info(info.str());
			msg.output_msg(2);
		}
		this->solver_mem=this->solver_mem+ number_real * sizeof(realtype) + number_int * sizeof(int);

        flag=CVBandPrecGetWorkSpace(this->cvode_mem, &number_real, &number_int);
		if(flag<0){
			Warning msg=this->set_warning(0);
			ostringstream info;
			info <<"Solver function: CVBandPrecGetWorkSpace(this->bp_data, &number_real, &number_int)"<< endl;
			info <<"CVode: " << this->cvode_init_flags2string(flag) << endl;
			msg.make_second_info(info.str());
			msg.output_msg(2);
		}
		this->solver_mem=this->solver_mem+number_real * sizeof(realtype) + number_int * sizeof(int);

		//the realtype vector of the result and estimated_error =>2
		this->solver_mem=this->solver_mem+this->NEQ*sizeof(realtype)*2;

		//count the memory
		Sys_Memory_Count::self()->add_mem(this->solver_mem, _sys_system_modules::HYD_SYS);

	}
}
//Additional functions for N_Vector data type: Returns max element of a Vector
realtype _Hyd_Model::N_VMax(N_Vector x){
  long int i, N;
  realtype max, *xd;

  N  = NV_LENGTH_S(x);
  xd = NV_DATA_S(x);

  max = xd[0];

  xd++;
  for (i=1; i < N; i++, xd++) {
    if ((*xd) > max) max = *xd;
  }

  return(max);
}
//Set the warning
Warning _Hyd_Model::set_warning(const int warn_type){
	string place="_Hyd_Model::";
		string help;
		string reason;
		string reaction;
		int type=0;
		Warning msg;
		stringstream info;

	switch (warn_type){
		case 0://solver get-functions
			place.append("count_solver_memory(void)") ;
			reason="Problem with the get-functions of the solver; can not count properly the memory requirement";
			reaction="No reaction";
			help= "Check the solver settings in the source code; check the solver manual";
			type=11;	
			break;
		case 1://solver get-functions
			place.append("calculate_solver_errors(void)") ;
			reason="Problem with the get-functions of the solver; can not calculate properly the solver errors";
			reaction="No reaction";
			help= "Check the solver settings in the source code; check the solver manual";
			type=11;	
			break;
		case 2://solver get-functions
			place.append("output_solver_statistics(ostringstream *cout)") ;
			reason="Problem with the get-functions of the solver; can not calculate properly the solver statistic";
			reaction="No reaction";
			help= "Check the solver settings in the source code; check the solver manual";
			type=11;	
			break;
		case 3://solver get-functions
			place.append("get_number_solversteps(void)") ;
			reason="Problem with the get-functions of the solver; can not calculate properly the number of solver steps";
			reaction="No reaction";
			help= "Check the solver settings in the source code; check the solver manual";
			type=11;	
			break;
		case 4://solver solve function
			place.append("run_solver(const double next_time_point)") ;
			reason="Problem with solving the function; if this problems occurs 3 times consecutivly, the calculation is automaticaly aborted";
			reaction="The tolerances are increased dynamically and the time step is recalculated";
			help= "Check/increase the setted tolerances or the maximum number of solver steps";
			type=11;	
			break;
		case 5://problems with converge
			place.append("run_solver(const double next_time_point)") ;
			reason="Problems with the converging of the solver";
			reaction="The time step is recalculated";
			type=11;	
			break;
		default:
			place.append("set_warning(const int warn_type)");
			reason ="Unknown flag!";
			help="Check the flags";
			type=5;
	}
	msg.set_msg(place,reason,help,reaction,type);
	msg.make_second_info(info.str());
	return msg;
}
//set the error
Error _Hyd_Model::set_error(const int err_type){
	string place="_Hyd_Model::";
		string help;
		string reason;
		int type=0;
		bool fatal=false;
		stringstream info;
		Error msg;

	switch (err_type){
		case 0://can not allocate the solver memory
			place.append("init_solver(Hyd_Param_Global *global_params)");
			reason="Can not allocate the memory for the solver";
			help="Check the memory and solver settings";
			type=17;
			break;
		case 1://problem by the solver settings
			place.append("init_solver(Hyd_Param_Global *global_params)");
			reason="There are problems by the solver settings with a solver settings-function";
			help="Check the solver settings";
			type=17;
			break;
		case 2://can not allocate the preconditioner memory
			place.append("init_solver(Hyd_Param_Global *global_params)");
			reason="Can not allocate the memory for the preconditioner";
			help="Check the memory and preconditioner settings";
			type=17;
			break;
		case 3://problem by the linear solver settings
			place.append("init_solver(Hyd_Param_Global *global_params)");
			reason="There are problems by the linear solver settings";
			help="Check the linear solver settings";
			type=17;
			break;
		case 4://can not allocate the vectors for the solver
			place.append("allocate_solver_vector(void)");
			reason="Can not allocate the memory for the result-vector/estimated_error-vector";
			help="Check the memory";
			type=17;
			break;
		case 5://to much consecutively warnings by solving the function
			place.append("run_solver(const double next_time_point)") ;
			reason="There occurrs more then 5 consecutively warnings by solving the function; calculation is aborted";
			help="Check the calculation settings. Try to increase the number of internal timesteps";
			type=17;
			break;
		case 6://can not solve the system
			place.append("run_solver(const double next_time_point)") ;
			reason="Problems to solve the system";
			help="Check the system and the solver setting";
			type=17;
			break;
		case 7://convergence problems
			place.append("run_solver(const double next_time_point)") ;
			reason="Problems with the converging of the solver";
			help="Check the system and the solver setting";
			type=17;
			break;
		case 8://problem by the solver reinitialisation
			place.append("rewind_solver(const double time)");
			reason="There are problems by the solver reinitialisation";
			help="Check the solver";
			type=17;
			break;
		case 9://problem by the solver copy 
			place.append("store_data_model(void)");
			reason="There are problems by the solver storing";
			help="Check the solver";
			type=17;
			break;
		default:
			place.append("set_error(const int err_type)");
			reason ="Unknown flag!";
			help="Check the flags";
			type=6;
	}
	msg.set_msg(place, reason, help, type, fatal);
	msg.make_second_info(info.str());
	return msg;

}