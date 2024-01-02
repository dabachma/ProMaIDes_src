/*
 * HIGH-PERFORMANCE INTEGRATED MODELLING SYSTEM (HiPIMS)
 * Copyright (C) 2023 Luke S. Smith, Qiuhua Liang.
 * This code is licensed under GPLv3. See LICENCE for more information.
 * This file has been modified by Alaa Mroue on 04.2023-12.2023.
 * See OriginalSourceCode.zip for changes. (Based on 1e62acf6b9b480e08646b232361b68c1827d91ae from https://github.com/lukeshope/hipims-ocl )
 */

#ifndef HIPIMS_OPENCL_OCLPROGRAM_H
#define HIPIMS_OPENCL_OCLPROGRAM_H

#include "CExecutorControlOpenCL.h"
#include "COCLDevice.h"
#include <unordered_map>

class COCLKernel;
class COCLProgram
{
public:
	COCLProgram(
		CExecutorControlOpenCL*	execController,
		COCLDevice*	device
	);
	~COCLProgram();
	CExecutorControlOpenCL*		getController()							{ return execController; }
	COCLDevice*					getDevice()								{ return device; }
	cl_context					getContext()							{ return clContext; }
	bool						isCompiled()							{ return bCompiled; }
	void						compileProgram( bool = true );
	void						appendCode( OCL_RAW_CODE );
	void						prependCode( OCL_RAW_CODE );
	void						appendCodeFromResource( std::string );
	void						prependCodeFromResource( std::string );
	void						clearCode();
	COCLKernel*					getKernel( const char * );
	std::string					getCompileLog();
	void						addCompileParameter( std::string );
	bool						registerConstant( std::string, std::string );
	bool						removeConstant( std::string );		
	void						clearConstants();		
	void						setForcedSinglePrecision( bool );

protected:
	OCL_RAW_CODE				getConstantsHeader( void );		
	OCL_RAW_CODE				getExtensionsHeader( void );			

	CExecutorControlOpenCL*		execController;
	COCLDevice*					device;
	cl_context					clContext;
	cl_program					clProgram;
	OCL_CODE_STACK				oclCodeStack;
	bool						bCompiled;
	bool						bForceSinglePrecision;
	std::string					sCompileParameters;
	std::unordered_map<std::string,std::string>					
								uomConstants;

friend class COCLKernel;
friend class COCLBuffer;
};

#endif
