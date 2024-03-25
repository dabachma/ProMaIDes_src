/*
 * HIGH-PERFORMANCE INTEGRATED MODELLING SYSTEM (HiPIMS)
 * Copyright (C) 2023 Luke S. Smith, Qiuhua Liang.
 * This code is licensed under GPLv3. See LICENCE for more information.
 * This file has been modified by Alaa Mroue on 04.2023-12.2023.
 * See OriginalSourceCode.zip for changes. (Based on 1e62acf6b9b480e08646b232361b68c1827d91ae from https://github.com/lukeshope/hipims-ocl )
 */

#ifndef HIPIMS_OPENCL_COCLKERNEL_H
#define HIPIMS_OPENCL_COCLKERNEL_H

#include "CL/opencl.h"

class COCLProgram;
class COCLBuffer;
class COCLDevice;
class COCLKernel
{
public:
	COCLKernel(
		COCLProgram*,
		std::string
	);
	~COCLKernel();

	COCLProgram*	getProgram()									{ return program; }
	std::string		getName()										{ return sName; }
	bool			isReady()										{ return bReady; }
	void			setCallback( void (__stdcall *cb)( cl_event, cl_int, void* ) )
																	{ fCallback = cb; }

	void			scheduleExecution();
	void			scheduleExecutionAndFlush();
	bool			assignArguments( COCLBuffer* Buffer_Arguments[] );
	bool			assignArgument( unsigned char Index, COCLBuffer* Buffer_Argument );
	void			setGlobalSize( cl_ulong = 1, cl_ulong = 1, cl_ulong = 1 );
	void			setGlobalOffset( cl_ulong = 0, cl_ulong = 0, cl_ulong = 0 );
	void			setGroupSize( cl_ulong = 1, cl_ulong = 1, cl_ulong = 1 );

protected:
	void			prepareKernel();

	cl_uint			uiDeviceID;
	cl_kernel		clKernel;
	cl_program		clProgram;
	cl_command_queue clQueue;
	COCLDevice*		pDevice;
	COCLProgram*	program;
	COCLBuffer**	arguments;
	size_t			szGlobalSize[3];
	size_t			szGlobalOffset[3];
	size_t			szGroupSize[3];
	cl_uint			uiArgumentCount;
	cl_ulong		ulMemPrivate;
	cl_ulong		ulMemLocal;
	std::string		sName;
	bool			bReady;
	bool			bGroupSizeForced;
	model::CallBackData			callBackData;
	void (__stdcall *fCallback)( cl_event, cl_int, void* );
};

#endif
