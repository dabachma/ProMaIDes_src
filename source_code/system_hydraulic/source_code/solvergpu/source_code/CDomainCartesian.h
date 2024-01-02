/*
 * HIGH-PERFORMANCE INTEGRATED MODELLING SYSTEM (HiPIMS)
 * Copyright (C) 2023 Luke S. Smith, Qiuhua Liang.
 * This code is licensed under GPLv3. See LICENCE for more information.
 * This file has been modified by Alaa Mroue on 04.2023-12.2023.
 * See OriginalSourceCode.zip for changes. (Based on 1e62acf6b9b480e08646b232361b68c1827d91ae from https://github.com/lukeshope/hipims-ocl )
 */

#ifndef HIPIMS_DOMAIN_CARTESIAN_CDOMAINCARTESIAN_H_
#define HIPIMS_DOMAIN_CARTESIAN_CDOMAINCARTESIAN_H_

#include "COCLDevice.h"

 // Public structures
struct DomainSummary
{
	bool			bAuthoritative;
	unsigned int	uiDomainID;
	unsigned int	uiNodeID;
	unsigned int	uiLocalDeviceID;
	double			dResolutionX;
	double			dResolutionY;
	unsigned long	ulRowCount;
	unsigned long	ulColCount;
	unsigned char	ucFloatPrecision;
	unsigned long	ulCouplingArraySize;
	bool			bUseOptimizedBoundary;
};

struct dataProgress
{
	unsigned int 	uiDomainID;
	double			dCurrentTimestep;
	double			dCurrentTime;
	double			dBatchTimesteps;
	cl_uint			uiBatchSkipped;
	cl_uint			uiBatchSuccessful;
	unsigned int	uiBatchSize;
};


enum direction
{
	north = 0,
	east = 1,
	south = 2,
	west = 3
};

// Data structures used in interop
struct DomainCell
{
	unsigned long	ulCellId;
	double			dDepth;
	double			dElevation;
	double			dV_x;
	double			dV_y;
	double			dCalculatedVelocity;
	double			dExpectedTimeStep;
};
/*
 *  DOMAIN CLASS
 *  CDomainCartesian
 *
 *  Stores the relevant information required for a
 *  domain based on a simple cartesian grid.
 */
class CDomainCartesian
{

	public:

		CDomainCartesian( void );												// Constructor
		~CDomainCartesian( void );												// Destructor

		// Public variables
		// ...

		// Public functions
		bool			validateDomain( bool );									// Verify required data is available
		void			prepareDomain();										// Create memory structures etc.
		void			logDetails();											// Log details about the domain

		// - Specific to Cartesian grids
		void			setCellResolution( double, double);						// Set cell resolution
		void			setRows(unsigned long);									// Set number of rows in the domain
		void			setCols(unsigned long);									// Set number of cols in the domain
		void			getCellResolution( double* , double*);					// Returns resolution of cell to two double*
		double			getCellResolutionX();									// Returns cell resolution in X
		double			getCellResolutionY();									// Returns cell resolution in Y
		unsigned long	getRows();												// Get the number of rows in the domain
		unsigned long	getCols();												// Get the number of columns in the domain
		unsigned long	getCellCount();											// X Return the total number of cells
		void			setName(std::string);									// Set the name of the domain
		std::string		getName(void);											// Get the name of the domain

		void			setUseOptimizedCoupling(bool);							// Set the flag for using the optimized coupling (smaller array for boundary conditions)
		void			setOptimizedCouplingSize(unsigned long);				// Set the size of the optimized coupling array
		bool			getUseOptimizedCoupling();								// Get the flag for using the optimized coupling (smaller array for boundary conditions)
		unsigned long	getOptimizedCouplingSize();								// Get the size of the optimized coupling array
		
		//Progress Monitoring
		dataProgress getDataProgress();											// Fetch some data on this domain's progress
		void 		setDataProgress(dataProgress);								// Set some data on this domain's progress


		double			getVolume();											// Calculate the amount of volume in all the cells
		double			getBoundaryVolume();									// Calculate the amount of volume in the boudnary array
		void			readBuffers_h_vx_vy(double*, double*,double*);			// Read GPU Buffers (All three Values)
		void			readBuffers_opt_h(double*);								// Read GPU Buffers (Water Depth: Surface Level - Bed Elevation)
		void			readBuffers_v_x(double*);								// Read GPU Buffers (Velocity in X)
		void			readBuffers_v_y(double*);								// Read GPU Buffers (Velocity in Y)
		void			resetBoundaryCondition();								// Resets boundary condition


		void				createStoreBuffers(void**, void**, void**, void**, void**,
			void**, void**, void**, void**, void**,
			void**, unsigned char);												// Allocates memory and returns pointers to the three arrays
		void				resetAllValues();									// Resets all buffer values
		void				setScheme(CScheme*);								// Set the scheme running for this domain
		CScheme*			getScheme();										// Get the scheme running for this domain
		void				setDevice(COCLDevice*);								// Set the device responsible for running this domain
		COCLDevice*			getDevice();										// Get the device responsible for running this domain

		//Setting Domain Data
		void			setBedElevation(unsigned long, double);					// Sets the bed elevation for a cell
		void			setManningCoefficient(unsigned long, double);			// Sets the manning coefficient for a cell
		void			setBoundaryCondition(unsigned long, double);			// Sets the boundary coefficient for a cell
		void			setOptimizedCouplingCondition(unsigned long, double);	// Sets the optimized coupling boundary coefficient for a cell
		void			setZxmax(unsigned long, double);						// Sets the Zx value for a cell
		void			setcx(unsigned long, double);							// Sets the Cx value for a cell 
		void			setZymax(unsigned long, double);						// Sets the Zy value for a cell
		void			setcy(unsigned long, double);							// Sets the Cx value for a cell 
		void			setFSL(unsigned long, double);							// Sets the water surface level (water height + bed elevation) for a cell 
		void			setMaxFSL(unsigned long, double);						// Sets the max water surface level (across time) for a cell 
		void			setDischargeX(unsigned long, double);					// Sets the discharge value in X for a cell 
		void			setDischargeY(unsigned long, double);					// Sets the discharge value in Y for a cell 
		void			setOptimizedCouplingID(unsigned long, unsigned long);	// Sets the cell id of a cell in order to map the coupling array to cell values
		void			setPoleniConditionX(unsigned long, bool);				// Sets the poleni condition in x for a cell
		void			setPoleniConditionY(unsigned long, bool);				// Sets the poleni condition in y for a cell
		double			getStateValue(unsigned long , unsigned char);			// Gets a state variable for a given cell
		double			getBedElevation(unsigned long);							// Gets the bed elevation for a given cell
		double			getZxmax(unsigned long);								// Gets the Zx value for a given cell
		double			getZymax(unsigned long);								// Gets the Zy value for a given cell
		double			getBoundaryCondition(unsigned long ulCellID);			// Gets the boundary condition for a given cell
		bool			isDoublePrecision() { return (ucFloatSize == 8); };		// Are we using double-precision?

		//HelperFunctions
		unsigned long		getCellID(unsigned long, unsigned long);										// Get the cell ID using an X and Y index
		void				getCellIndices(unsigned long ulID, unsigned long* lIdxX, unsigned long* lIdxY); // Fetch the X and Y indices for a cell using its ID
		unsigned long		getNeighbourID(unsigned long ulCellID, unsigned char  ucDirection);				// Fetch the ID for a neighboring cell in the domain
		void				memoryDump();																	// Dumps memory for debugging
		void				output_to_vtk_file(std::string path, double time, std::string rasterName,		// Output various data into a ParaView file (useful for debugging)
									int sizeX, int sizeY, double* opt_z, double* opt_zx_max, double* opt_zy_max,
										double* opt_h, double* opt_s, double* opt_v_x, double* opt_v_y);

	private:

		// Private General Domain Variables
		unsigned long	ulRows;						// Number of cells in the y-direction
		unsigned long	ulCols;						// Number of cells in the x-direction
		double			dCellResolutionX;			// Size of cell in the x-direction
		double			dCellResolutionY;			// Size of cell in the y-direction
		bool			bUseOptimizedBoundary;		// Show boundary condition be optimized
		unsigned long	ulCouplingArraySize;		// If boundary condition is optimized, what is then the size of the boundary condition array
		dataProgress	sDataProgress;				// Data on this domain's progress
		CScheme* pScheme;							// Scheme we are running for this particular domain
		std::string		domainName;					// Custom name of the domain (For outputting)
		COCLDevice* pDevice;						// Device responsible for running this domain

		// Private Domain GPU heaps
		unsigned int uiRounding;			// Significant value to round to
		unsigned char		ucFloatSize;	// Size of floats used for cell data (bytes)

		cl_double4* dCellStates;			// Heap for cell state data
		cl_double* dBedElevations;			// Heap for bed elevations
		cl_double* dManningValues;			// Heap for manning values
		cl_double* dBoundaryValues;			// Heap for boundary values
		cl_double* dOpt_zxmaxValues;		// Heap for opt_zxmax values
		cl_double* dOpt_cxValues;			// Heap for opt_cx values
		cl_double* dOpt_zymaxValues;		// Heap for opt_zymax values
		cl_double* dOpt_cyValues;			// Heap for opt_cy values
		cl_double* dCouplingValues;			// Heap for optimized coupling values

		cl_float4* fCellStates;				// Heap for cell state date (single)
		cl_float* fBedElevations;			// Heap for bed elevations (single)
		cl_float* fManningValues;			// Heap for manning values (single)
		cl_float* fBoundaryValues;			// Heap for boundary values (single)
		cl_float* fOpt_zxmaxValues;			// Heap for opt_zxmax values (single)
		cl_float* fOpt_cxValues;			// Heap for opt_cx values (single)
		cl_float* fOpt_zymaxValues;			// Heap for opt_zymax values (single)
		cl_float* fOpt_cyValues;			// Heap for opt_cy values (single)
		cl_float* fCouplingValues;			// Heap for optimized coupling values (single)

		sUsePoleni* bPoleniValues;			// Heap for Struct of Poleni values
		cl_ulong* ulCouplingIDs;			// Heap for optimized coupling IDs

};

#endif
