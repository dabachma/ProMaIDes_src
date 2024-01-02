/*
 * HIGH-PERFORMANCE INTEGRATED MODELLING SYSTEM (HiPIMS)
 * Copyright (C) 2023 Luke S. Smith, Qiuhua Liang.
 * This code is licensed under GPLv3. See LICENCE for more information.
 * This file has been modified by Alaa Mroue on 04.2023-12.2023.
 * See OriginalSourceCode.zip for changes. (Based on 1e62acf6b9b480e08646b232361b68c1827d91ae from https://github.com/lukeshope/hipims-ocl )
 */


#ifndef HIPIMS_OPENCL_COCLBUFFER_H
#define HIPIMS_OPENCL_COCLBUFFER_H



class COCLProgram;
class COCLDevice;
class COCLBuffer
{
public:
	COCLBuffer( std::string, COCLProgram*, bool Read_Only = false, bool Exists_On_Host = true, cl_ulong Length_In_Bytes = NULL, bool Allocate_Now = false );
	~COCLBuffer();
	std::string		getName()							{ return sName; }
	cl_mem			getBuffer()							{ return clBuffer; }
	cl_ulong		getSize()							{ return ulSize; }
	bool			isReady()							{ return bReady; }
	void			setCallbackRead( void (__stdcall * cb)( cl_event, cl_int, void* ) )
														{ fCallbackRead = cb; }
	void			setCallbackWrite( void (__stdcall * cb)( cl_event, cl_int, void* ) )
														{ fCallbackWrite = cb; }
	template <typename blockType>
	blockType		getHostBlock()						{ return static_cast<blockType>( this->pHostBlock ); }
	bool			createBuffer();
	bool			createBufferAndInitialise();
	void			setPointer( void*, cl_ulong );
	void			allocateHostBlock( cl_ulong );
	void			queueReadAll();
	void			queueReadPartial( cl_ulong, size_t, void* = NULL );
	void			queueWriteAll();
	void			queueWritePartial( cl_ulong, size_t, void* = NULL );

protected:
	cl_uint			uiDeviceID;
	std::string		sName;
	cl_mem_flags	clFlags;
	cl_context		clContext;
	cl_command_queue clQueue;
	cl_mem			clBuffer;
	void*			pHostBlock;
	COCLDevice*		pDevice;
	cl_ulong		ulSize;
	bool			bReady;
	bool			bInternalBlock;
	bool			bReadOnly;
	bool			bExistsOnHost;
	model::CallBackData			callBackData;
	void (__stdcall *fCallbackRead)( cl_event, cl_int, void* );
	void (__stdcall *fCallbackWrite)( cl_event, cl_int, void* );
};

#endif
