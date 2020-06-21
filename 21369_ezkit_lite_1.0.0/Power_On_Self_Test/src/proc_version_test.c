/*****************************************************************************
**																			**
**	 Name: 	ProcessorVersion.c											    **
**																			**
******************************************************************************

Pjoject Name:	21369/21375 POST

Hardware:		ADSP-21369/21375 EZ-KIT Lite

Connections:

Purpose:		To test the processor version agaisnt the software build

*****************************************************************************/

#ifdef __ADSP21369__
#include "Cdef21369.h"
#endif

//--------------------------------------------------------------------------//
// Function:	int TestVersion(void)										//
//																			//
// Parameters:	None														//
//																			//
// Return:		0 indicates a failure - the processor does not match the    //
//				  version the software was built for                        //
//				1 indicates a pass - the processor and software versions    //
//				  both match                                                //
//																			//
// Description:	This function compares the version of the processor being   //
//              run on with the version that the software was built against //
//                                                                          //
// Purpose:		The reason for this test is because the build tools work-   //
//              around various silicon anomalies, by not checking to see    //
//              if the software is built for this particular processor      //
//              version, a risk that an anomaly will not be worked around   //
//              correctly.
//--------------------------------------------------------------------------//
int TEST_VERSION(void)
{
#ifdef __ADSP21369__  
	int running_on = ((*pREVPID & 0x00F0) >> 4);	/* check the part */
	int built_for = __SILICON_REVISION__;			/* check what we built against */
	if( built_for != running_on )
#else
	if( __SILICON_REVISION__ != 0x0000 )
#endif
	{
		return 0; // failed
	}
	else
	{
		return 1; // passed
	}
}
