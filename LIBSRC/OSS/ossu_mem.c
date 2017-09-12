/*********************  P r o g r a m  -  M o d u l e ***********************/
/*!
 *        \file  ossu_mem.c
 *
 *      \author  christian.schuster@men.de
 *        $Date: 2005/07/08 11:32:57 $
 *    $Revision: 2.1 $
 *
 *	   \project  MDIS4Linux
 *  	 \brief  Memory handling functions of the OSS module for user space
 *
 *    \switches
 */
/*-------------------------------[ History ]---------------------------------
 *
 * $Log: ossu_mem.c,v $
 * Revision 2.1  2005/07/08 11:32:57  cs
 * Initial Revision
 *
 *
 *---------------------------------------------------------------------------
 * (c) Copyright 2005 by MEN Mikro Elektronik GmbH, Nuremberg, Germany
 ******************************************************************************/
/*
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#include "oss_intern.h"


/**********************************************************************/
/** Allocates general memory block.	
 *
 * \copydoc oss_specification.c::OSS_MemGet()
 *
 * \linux Puts calling process to sleep if no free pages available (i.e.
 *	should never return NULL).
 *
 * \sa OSS_MemFree
 */
void* OSS_MemGet(
    OSS_HANDLE  *oss,
    u_int32     size,
    u_int32     *gotsizeP
)
{

	void *mem = malloc( size );
	if( mem != NULL )
		*gotsizeP = size;
	else
		*gotsizeP = 0;

	DBGWRT_1((DBH,"OSS_MemGet: size=0x%lx allocated addr=0x%p\n",
			  size, mem ));
	return mem;
}/*OSS_MemGet*/

/**********************************************************************/
/** Return memory block.
 *
 * \copydoc oss_specification.c::OSS_MemFree()
 *
 * \sa OSS_MemGet
 */
int32 OSS_MemFree(
    OSS_HANDLE *oss,
    void       *addr,
    u_int32    size
)
{
	DBGWRT_1((DBH,"OSS_MemFree: addr=0x%p size=0x%lx\n", addr, size));
	free(addr);
    return(0);
}/*OSS_MemFree*/


/**********************************************************************/
/** Check if memory block accessible by caller
 *
 * \copydoc oss_specification.c::OSS_MemChk()
 *
 * \linux This function is a no-op.
 */
int32 OSS_MemChk(
    OSS_HANDLE *oss,
    void       *addr,
    u_int32    size,
    int32      mode)
{
	return 0;
}
