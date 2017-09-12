/*********************  P r o g r a m  -  M o d u l e ***********************/
/*!
 *        \file  vme4l_rwex.c
 *
 *      \author  klaus.popp@men.de
 *        $Date: 2009/06/03 19:19:53 $
 *    $Revision: 1.3 $
 *
 *  	 \brief  Example program for VME4L_Read/Write
 *
 *     Switches: -
 *     Required: libraries: vme4l_api
 */
/*-------------------------------[ History ]---------------------------------
 *
 * $Log: vme4l_rwex.c,v $
 * Revision 1.3  2009/06/03 19:19:53  rt
 * R: 1.) No support for VME addresses > 0xFFFFFFFF.
 * M: 1.) Use strtoull for VME address.
 *
 * Revision 1.2  2009/04/30 21:18:05  rt
 * R: 1.) Print better error message.
 * M: 1.) Added comment.
 *
 * Revision 1.1  2003/12/15 15:02:28  kp
 * Initial Revision
 *
 *---------------------------------------------------------------------------
 * (c) Copyright 2017 by MEN mikro elektronik GmbH, Nuremberg, Germany
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

static const char RCSid[]="$Id: vme4l_rwex.c,v 1.3 2009/06/03 19:19:53 rt Exp $";

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <signal.h>
#include <errno.h>
#include <MEN/vme4l.h>
#include <MEN/vme4l_api.h>
#include <MEN/men_typs.h>
#include <MEN/usr_utl.h>
#include <MEN/usr_oss.h>

/*--------------------------------------+
|   DEFINES                             |
+--------------------------------------*/
#define CHK(expression) \
 if( !(expression)) {\
	 printf("\n*** Error during: %s\nfile %s\nline %d\n", \
      #expression,__FILE__,__LINE__);\
      printf("%s\n",strerror(errno));\
     goto ABORT;\
 }

/*--------------------------------------+
|   TYPDEFS                             |
+--------------------------------------*/

/*--------------------------------------+
|   GLOBALS                             |
+--------------------------------------*/


/*--------------------------------------+
|   PROTOTYPES                          |
+--------------------------------------*/
static void MemDump(
    uint8_t *buf,
    uint32_t n,
    uint32_t fmt
	);


static void usage(int excode)
{
	printf("Syntax:   vme4l_rwex [<opts>] <vmeaddr> <size> [<opts>]\n");
	printf("Function: perform basic VME read/write from and to VME spaces\n");
	printf("Options:\n\n");
	printf("-s=<spc>      VME4L space number\n");
	printf("-a=<width>    access width in bytes (1/2/4)        [4]\n");
	printf("-v=<init.Val> initial value for write buffer fill  [0]\n");
	printf("-x            use swap mode                        [0]\n");
	printf("-r            read from VME space into CPU\n");
	printf("-w            write from CPU to VME space\n");
	exit(excode);
}

static void SigHandler( int sigNum )
{

}

/**********************************************************************/
/** Program entry point
 *
 * \return success (0) always
 */
int main( int argc, char *argv[] )
{
	int fd, rv, i;
	uint8_t *buf;
	VME4L_SPACE spc=0;
	vmeaddr_t vmeAddr;
	u_int32 startaddr= 0xffffffff;
	int accWidth=4;
	size_t size;
	char *optp=NULL;
	int doRead=-1;
	int swapMode = 0;
	int startVal = 0;

	if( UTL_TSTOPT("?") || (argc == 1) )
		usage(0);

	/*-----------------------------+
    |  Parse command line options  |
    +-----------------------------*/
	for(i=1; i<argc; i++ )
		if( *argv[i]!='-'){
			if( startaddr == 0xffffffff )
				sscanf( argv[i], "%lx", &startaddr );
			else
				sscanf( argv[i], "%lx", &size );
		}

	if( startaddr==0xffffffff || size==0xffffffff ){
		printf("*** missing vmeaddr or size!\n");
		usage(1);
	}

	signal( SIGUSR1, SigHandler ); /* catch sig 10 (typical) */

	vmeAddr = startaddr;

	if (UTL_TSTOPT("r"))
		doRead = 1;
	else if (UTL_TSTOPT("w"))
		doRead = 0;

	if( (optp=UTL_TSTOPT("a=")))
		sscanf(optp, "%d", &accWidth);

	if( (optp=UTL_TSTOPT("s=")))
		spc=atoi(optp);

	swapMode = (optp=UTL_TSTOPT("x")) ? 1 : 0;

	if( (optp=UTL_TSTOPT("v=")))
		startVal=atoi(optp);


	if (doRead == -1) {
		printf("*** specify either -r or -w!\n");
		usage(1);
	}


	CHK( (buf = malloc( size )) != NULL);

	printf("Open space %s, user buffer @ %p \n", VME4L_SpaceName(spc), buf );

	CHK( (fd = VME4L_Open( spc )) >= 0 /*node /dev/vme4l_<spc> must exist*/ );

	CHK( VME4L_SwapModeSet( fd, swapMode ) == 0 );

	if( doRead )
	{
		CHK( (rv = VME4L_Read( fd, vmeAddr, accWidth, size, buf, VME4L_RW_NOFLAGS )) >=0);
		MemDump( buf, rv, 1 );
	}
	else {
		uint8_t *p = buf;
		int i;

		for( i=0; i < size; i++ )
			*p++ = (i + startVal) & 0xff;

		CHK( (rv = VME4L_Write( fd, vmeAddr, accWidth, size, buf, VME4L_RW_NOFLAGS )) >=0);
	}

	return 0;

 ABORT:
	return 1;
}

static void MemDump(
    uint8_t *buf,
    uint32_t n,
    uint32_t fmt
)
{
    uint8_t *k, *k0, *kmax = buf+n;
    int32_t i;

    for (k=k0=buf; k0<kmax; k0+=16)
    {
        printf("%04x: ", (unsigned int)(k-buf));

        switch(fmt)                                        /* dump hex: */
        {
           case 4 : for (k=k0,i=0; i<16; i+=4, k+=4)       /* long aligned */
                       if (k<kmax)  printf("%08lx ",*(uint32_t*)k);
                       else         printf("         ");
                    break;
           case 2 : for (k=k0,i=0; i<16; i+=2, k+=2)       /* word aligned */
                       if (k<kmax)  printf("%04x ",*(uint16_t*)k & 0xffff);
                       else         printf("     ");
                    break;
           default: for (k=k0,i=0; i<16; i++, k++)         /* byte aligned */
                       if (k<kmax)  printf("%02x ",*k & 0xff);
                       else         printf("   ");
        }

        for (k=k0,i=0; i<16 && k<kmax; i++, k++)           /* dump ascii */
            if ( *(uint8_t*)k>=32 && *(uint8_t*)k<=127 )
               printf("%c", *k);
            else
               printf(".");

        printf("\n");
    }
}
