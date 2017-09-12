/*********************  P r o g r a m  -  M o d u l e ***********************
 *
 *         Name: regfile.c
 *      Project: descgen
 *
 *       Author: ds
 *        $Date: 2000/07/04 09:21:24 $
 *    $Revision: 1.1 $
 *
 *  Description: Output descriptor as .reg-file for Windows NT
 *
 *
 *     Required: -
 *     Switches: -
 *
 *---------------------------[ Public Functions ]----------------------------
 *
 *
 *-------------------------------[ History ]---------------------------------
 *
 * $Log: regfile.c,v $
 * Revision 1.1  2000/07/04 09:21:24  Schmidt
 * Initial Revision
 *
 *---------------------------------------------------------------------------
 * (c) Copyright 2000 by MEN mikro elektronik GmbH, Nuremberg, Germany
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

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <time.h>

#include <MEN/men_typs.h>
#include <MEN/desctyps.h>
#include "descgen.h"

/*--------------------------------------+
|   DEFINES                             |
+--------------------------------------*/
#define MAX_REG_DIR		200
#define MAX_SUBREG_DIR	100
#define MAX_DEV			100
#define MAX_DRV			100
#define SERVICES_DIR	"HKEY_LOCAL_MACHINE\\SYSTEM\\CurrentControlSet\\Services"

/*--------------------------------------+
|   TYPDEFS                             |
+--------------------------------------*/
typedef struct{
	char		*drvName;
	u_int32		devCount;
	DESCR_TAG	*devTag[MAX_DEV];
} DRV_LIST;

/*--------------------------------------+
|   GLOBALS                             |
+--------------------------------------*/
DRV_LIST	G_devDrvList[MAX_DRV];
DRV_LIST	G_brdDrvList[MAX_DRV];
u_int32		G_devDrvNbr;
u_int32		G_brdDrvNbr;
char		G_regDrvDir[MAX_REG_DIR];	/* registry dir for driver entry */

/*--------------------------------------+
|   PROTOTYPES                          |
+--------------------------------------*/
static int32 GetParam( DESCR_TAG *topTag, u_int32 *descType, char **drvName );
static int32 RegDevTag( DESCR_TAG *topTag, char *drvName );
static int32 RegBrdTag( DESCR_TAG *topTag, char *drvName );
static void OutDevEntries( FILE *fp );
static void OutBrdEntries( FILE *fp );
static void OutTagParam( FILE *fp, DESCR_TAG *topTag, int32 level );


/********************************* OutRegFile *******************************
 *
 *  Description: Output internal data base as .reg-File
 *			
 *---------------------------------------------------------------------------
 *  Input......: fname		output filename for source file
 *				 topTag		root directory of objects
 *  Output.....: return		error (>0) or success (0) 		
 *  Globals....: several
 ****************************************************************************/
int32 OutRegFile( char *fname, DESCR_TAG *topTag )
{
	FILE		*fp;
	DESCR_TAG	*tag;
	char		regFile[MAX_FNAME_LENGTH], *p;
	char		fbody[MAX_FNAME_LENGTH];
	char		*drvName;
	u_int32		descType;
	time_t		ltime;
	int32		error;

	time(&ltime);

	/*------------------------------------+
	|  create file name                   |
	+------------------------------------*/	
	/* remove input path */
	if( p = strrchr( fname, MEN_PATHSEP ))
		strcpy( fbody, p+1 );
	else
		strcpy( fbody, fname );
	
	/* remove old file extension */
	if( p = strrchr( fbody, '.' ))
		*p = '\0';

	/* add dir and new file extension */
	sprintf(regFile,"%s%c%s.reg",G_outputDir,MEN_PATHSEP,fbody);

	/*------------------------------------+
	|  check if output file already exist |
	+------------------------------------*/	
	if (FileExist(regFile))
		if (!G_overwrite) {
			fprintf(stderr,"*** output file %s already exists\n",
					regFile);
			return(1);
		}
		else
			printf("overwriting %s\n",regFile);
	else
		printf("creating %s\n",regFile);

	/*------------------------------------+
    |  open output file                   |
	+------------------------------------*/	
	if( (fp = fopen( regFile, "w" )) == NULL ){
		fprintf( stderr, "*** error %d opening output file %s\n",
				errno, regFile );
		return errno;
	}

	/*------------------------------------+
    |  output header                      |
	+------------------------------------*/	
	fprintf( fp, "REGEDIT4\n" );	/* must be the first line */
	fprintf( fp, "\n" );			/* second line must be blank */
	fprintf( fp, "# ------------------------------------------------------------------------\n" );
	fprintf( fp, "# Generated by descgen %s (MDIS descriptor generator)\n",G_version );
	fprintf( fp, "#   Input File : %s\n",fname );
	fprintf( fp, "#   Date       : %s",ctime(&ltime) );
	fprintf( fp, "#\n" );
	fprintf( fp, "# This file can simply be double clicked or run to enter the information\n" );
	fprintf( fp, "# into the registry.\n" );
	fprintf( fp, "# You can edit this file on your own risk, but it is better to edit the\n" );
	fprintf( fp, "# descgen input file. Note that all numbers in this file are expressed\n" );
	fprintf( fp, "# in hex, and should never include a '0x' prefix.\n" );
	fprintf( fp, "# ------------------------------------------------------------------------\n" );
	fprintf( fp, "\n" );

	/*------------------------------------+
    |  top-tag loop (e.g. m_34, d201, ..) |
	+------------------------------------*/	
	for( tag=topTag->children; tag; tag=tag->next ){

		/*------------------------------------+
        |  register boards and devices        |
		+------------------------------------*/	
		if( error = GetParam( tag, &descType, &drvName ) )
			return error;
		
		switch( descType ){
			/* device descriptor */
			case 1:
				if( error = RegDevTag( tag, drvName ) )
					return error;
				break;
			/* board descriptor */
			case 2:
				if( error = RegBrdTag( tag, drvName ) )
					return error;
				break;
			/* unknown descriptor */
			default:
				return 1;
		}

	}

	/*------------------------------------+
    |  output descriptor entries          |
    +------------------------------------*/
	if( G_brdDrvNbr )
		OutBrdEntries( fp );
	
	if( G_devDrvNbr )
		OutDevEntries( fp );

	fprintf( fp, "\n" );	/* last line must be blank */

	fclose(fp);
	return(0);
}

/********************************* GetParam *********************************
 *
 *  Description: Get the following parameters from the current top-tag:
 *                 - DESC_TYPE
 *                 - HW_TYPE
 *			
 *---------------------------------------------------------------------------
 *  Input......: topTag		top-tag
 *  Output.....: *descType	descriptor type (DESC_TYPE)
 *				 *drvName	driver name for device (HW_TYPE)
 *               return		error (>0) or success (0) 		
 *  Globals....: -
 ****************************************************************************/
static int32 GetParam( DESCR_TAG *topTag, u_int32 *descType, char **drvName )
{
	DESCR_TAG	*tag;
	char		*p;

	*descType = 0;
	*drvName = NULL;

	/*------------------------------------+
    |  parameter loop (DESC_TYPE-...)     |
    +------------------------------------*/	
	for( tag=topTag->children; tag; tag=tag->next ){
		
		if( strcmp( tag->name, "DESC_TYPE" ) == 0 )
			*descType = tag->val.uInt32;
			
		if( strcmp( tag->name, "HW_TYPE" ) == 0 )
			*drvName = tag->val.string;
	}

	/* DESC_TYPE found ? */
	if( !descType ){
		fprintf(stderr, "*** DESC_TYPE not found\n" );
		return 1;
	}

	/* HW_TYPE found ? */
	if( !drvName ){
		fprintf(stderr, "*** HW_TYPE not found\n" );
		return 1;
	}
	/* convert driver name (M00x->Mx, M0xx->Mxx) */
	else{
		p = *drvName;
		if( (*p == 'M') && (*(p+1) == '0') ){
			/* M00 -> remove 00 */
			if( *(p+2) == '0' )
				strcpy( p+1, p+3 );
			/* M0 -> remove 0 */
			else
				strcpy( p+1, p+2 );
		}
	}

	return 0;
}

/********************************* RegDevTag ********************************
 *
 *  Description: Register device tag in device driver list.
 *			
 *---------------------------------------------------------------------------
 *  Input......: topTag		top-tag to register
 *				 drvName	driver name
 *  Output.....: return		error (>0) or success (0) 		
 *  Globals....: several
 ****************************************************************************/
static int32 RegDevTag( DESCR_TAG *topTag, char *drvName )
{
	u_int32		n;

	/*------------------------------------+
    |  search for driver entry            |
    +------------------------------------*/	
	for( n=0; n<G_devDrvNbr; n++ ){
		if( strcmp( drvName, G_devDrvList[n].drvName ) == 0 )
			break;
	}

	/*------------------------------------+
    |  driver entry not exist ?           |
    +------------------------------------*/	
	if( n >= G_devDrvNbr ){
		
		/* list full ? */
		if( G_devDrvNbr >= MAX_DRV ){
			fprintf(stderr, "*** device driver list full\n" );
			return 1;
		}

		/* add driver */
		n=G_devDrvNbr;
		G_devDrvList[n].drvName = drvName;
		G_devDrvNbr++;
	}

	/*------------------------------------+
    |  add device tag                     |
    +------------------------------------*/	
	/* list full ? */
	if( G_devDrvList[n].devCount >= MAX_DEV ){
		fprintf(stderr, "*** too many devices for driver %s\n",
			G_devDrvList[n].drvName );
		return 1;
	}

	G_devDrvList[n].devTag[G_devDrvList[n].devCount] = topTag;
	G_devDrvList[n].devCount++;	/* device added */

	return 0;
}

/********************************* RegBrdTag ********************************
 *
 *  Description: Register board tag in board driver list.
 *			
 *---------------------------------------------------------------------------
 *  Input......: topTag		top-tag to register
 *				 drvName	driver name
 *  Output.....: return		error (>0) or success (0) 		
 *  Globals....: several
 ****************************************************************************/
static int32 RegBrdTag( DESCR_TAG *topTag, char *drvName )
{
	u_int32		n;

	/*------------------------------------+
    |  search for driver entry            |
    +------------------------------------*/	
	for( n=0; n<G_brdDrvNbr; n++ ){
		if( strcmp( drvName, G_brdDrvList[n].drvName ) == 0 )
			break;
	}

	/*------------------------------------+
    |  driver entry not exist ?           |
    +------------------------------------*/	
	if( n >= G_brdDrvNbr ){
		
		/* list full ? */
		if( G_brdDrvNbr >= MAX_DRV ){
			fprintf(stderr, "*** board driver list full\n" );
			return 1;
		}

		/* add driver */
		n=G_brdDrvNbr;
		G_brdDrvList[n].drvName = drvName;
		G_brdDrvNbr++;
	}

	/*------------------------------------+
    |  add board tag                      |
    +------------------------------------*/	
	/* list full ? */
	if( G_brdDrvList[n].devCount >= MAX_DEV ){
		fprintf(stderr, "*** too many boards for driver %s\n",
			G_brdDrvList[n].drvName );
		return 1;
	}

	G_brdDrvList[n].devTag[G_brdDrvList[n].devCount] = topTag;
	G_brdDrvList[n].devCount++;	/* board added */

	return 0;
}

/********************************* OutDevEntries ****************************
 *
 *  Description: Output parameters for device descriptor
 *			
 *---------------------------------------------------------------------------
 *  Input......: fp			outfile stream
 *  Output.....: -
 *  Globals....: several
 ****************************************************************************/
static void OutDevEntries( FILE *fp )
{
	DESCR_TAG	*tag;
	u_int32		drv, dev;

	fprintf( fp, "\n" );
	fprintf( fp, "#######################################\n" );
	fprintf( fp, "#        Device Driver Entries        #\n" );
	fprintf( fp, "#######################################\n" );

	/*---------------------+
    |  handle all drivers  |
    +---------------------*/	
	for( drv=0; drv<G_devDrvNbr; drv++ ){

		/* new driver */
		fprintf( fp, "\n" );
		fprintf( fp, "# ------------------------------ DRIVER: %s ------------------------------\n",
				 G_devDrvList[drv].drvName );

		/* NT specific parameters for event log service */
		fprintf( fp, "[%s\\EventLog\\System\\men_%s]\n", SERVICES_DIR, strlwr(G_devDrvList[drv].drvName) );
		fprintf( fp, "\"TypesSupported\"=dword:7\n" );
		/* Note: hex(2) = REG_EXPAND_SZ */
		fprintf( fp, "\"EventMessageFile\"=hex(2):25,53,79,73,74,65,6d,52,6f,6f,74,25,5c,53,79,73,74,\\\n"
					 "65,6d,33,32,5c,49,6f,4c,6f,67,4d,73,67,2e,64,6c,6c,3b,25,53,79,73,74,65,6d,\\\n"
					 "52,6f,6f,74,25,5c,53,79,73,74,65,6d,33,32,5c,6d,65,6e,5f,65,76,6c,67,2e,64,\\\n"
					 "6c,6c,00\n" );

		/* build driver dir */
		sprintf( G_regDrvDir, "%s\\men_%s", SERVICES_DIR, G_devDrvList[drv].drvName );

		/* NT specific parameters for device drivers */
		fprintf( fp, "\n" );
		fprintf( fp, "[%s]\n", G_regDrvDir );
		fprintf( fp, "\"Type\"=dword:1\n" );
		fprintf( fp, "\"Start\"=dword:3\n" );
		fprintf( fp, "\"Group\"=\"MEN_MODULE_DRIVER\"\n" );
		/* Note: hex(7) = REG_MULTI_SZ */
		fprintf( fp, "\"DependOnGroup\"=hex(7):4d,45,4e,5f,42,4f,41,52,44,5f,44,52,49,56,45,52,00,00\n" );
		fprintf( fp, "\"ErrorControl\"=dword:1\n" );
		fprintf( fp, "\"DEBUG_LEVEL_ENTRY\"=dword:c0008000\n" );

		/*---------------------+
		|  handle all devices  |
		+---------------------*/	
		for( dev=0; dev<G_devDrvList[drv].devCount; dev++ ){

			/* new device */
			tag = G_devDrvList[drv].devTag[dev];
			fprintf( fp, "\n" );
			fprintf( fp, "# --------------- DEVICE: %s ---------------\n", tag->name );

			/* NT specific device parameters */
			fprintf( fp, "[%s\\%s]\n", G_regDrvDir, strlwr(tag->name) );
			fprintf( fp, "\"VALID\"=dword:1\n" );

			/* common device parameters */
			OutTagParam( fp, tag, 0 );
		}

	}
}

/********************************* OutBrdEntries ****************************
 *
 *  Description: Output parameters for board descriptor
 *			
 *---------------------------------------------------------------------------
 *  Input......: fp			outfile stream
 *  Output.....: -
 *  Globals....: several
 ****************************************************************************/
static void OutBrdEntries( FILE *fp )
{
	DESCR_TAG	*topTag;
	u_int32		drv, dev;

	fprintf( fp, "\n" );
	fprintf( fp, "#######################################\n" );
	fprintf( fp, "#        Board Driver Entries         #\n" );
	fprintf( fp, "#######################################\n" );

	/*---------------------+
    |  handle all drivers  |
    +---------------------*/	
	for( drv=0; drv<G_brdDrvNbr; drv++ ){

		/* new driver */
		fprintf( fp, "\n" );
		fprintf( fp, "# ------------------------------ DRIVER: %s ------------------------------\n",
				 G_brdDrvList[drv].drvName );

		/* NT specific parameters for event log service */
		fprintf( fp, "[%s\\EventLog\\System\\men_%s]\n", SERVICES_DIR, strlwr(G_brdDrvList[drv].drvName) );
		fprintf( fp, "\"TypesSupported\"=dword:7\n" );
		/* Note: hex(2) = REG_EXPAND_SZ */
		fprintf( fp, "\"EventMessageFile\"=hex(2):25,53,79,73,74,65,6d,52,6f,6f,74,25,5c,53,79,73,74,\\\n"
					 "65,6d,33,32,5c,49,6f,4c,6f,67,4d,73,67,2e,64,6c,6c,3b,25,53,79,73,74,65,6d,\\\n"
					 "52,6f,6f,74,25,5c,53,79,73,74,65,6d,33,32,5c,6d,65,6e,5f,65,76,6c,67,2e,64,\\\n"
					 "6c,6c,00\n" );

		/* build driver dir */
		sprintf( G_regDrvDir, "%s\\men_%s", SERVICES_DIR, G_brdDrvList[drv].drvName );

		/* NT specific parameters for board drivers */
		fprintf( fp, "\n" );
		fprintf( fp, "[%s]\n", G_regDrvDir );
		fprintf( fp, "\"Type\"=dword:1\n" );
		fprintf( fp, "\"Start\"=dword:3\n" );
		fprintf( fp, "\"Group\"=\"MEN_BOARD_DRIVER\"\n" );
		fprintf( fp, "\"ErrorControl\"=dword:1\n" );
		fprintf( fp, "\"DEBUG_LEVEL_ENTRY\"=dword:c0008000\n" );

		/*---------------------+
		|  handle all boards   |
		+---------------------*/	
		for( dev=0; dev<G_brdDrvList[drv].devCount; dev++ ){

			/* new board */
			topTag = G_brdDrvList[drv].devTag[dev];
			fprintf( fp, "\n" );
			fprintf( fp, "# --------------- BOARD: %s ---------------\n", topTag->name );

			/* NT specific board parameters */
			fprintf( fp, "[%s\\%s]\n", G_regDrvDir, strlwr(topTag->name) );
			fprintf( fp, "\"VALID\"=dword:1\n" );

			/* common board parameters */
			OutTagParam( fp, topTag, 0 );
		}

	}
}

/********************************* OutTagParam ******************************
 *
 *  Description: Output .reg-parameters for a single tag directory
 *			
 *---------------------------------------------------------------------------
 *  Input......: fp			outfile stream
 *				 topTag		top-tag to generate
 *				 level		recursion level
 *  Output.....: -
 *  Globals....: several
 ****************************************************************************/
static void OutTagParam( FILE *fp, DESCR_TAG *topTag, int32 level )
{
	DESCR_TAG	*tag;
	int32		i, n, line, nameLen;
	static char	subDir[MAX_SUBREG_DIR];
	char		*p;

	/* first level -> set new sub-dir */
	if( level == 0 )
		sprintf( subDir, "\\%s\\Parameters", strlwr(topTag->name) );

	/* out new dir */
	fprintf( fp, "\n" );
	fprintf( fp, "[%s%s]\n", G_regDrvDir, subDir );

	/*------------------------------------+
	|  handle all tags of current dir     |
	+-------------------------------------*/	
	for( tag=topTag->children; tag; tag=tag->next ){

		switch( tag->type ){
		
		/*-------------------+
		|  new sub-directory |
		+-------------------*/	
		case DESC_DIR:
					
			/* add subdir */
			strcat( subDir, "\\" );
			strcat( subDir, tag->name );
			level++;
			
			/* handle subdir entries (recursive call) */
			OutTagParam( fp, tag, level );

			/* remove subdir */
			level--;
			if( p = strrchr( subDir, '\\' ))
				*p = '\0';

			break;
			
		/*-------------------+
		|  out string        |
		+-------------------*/	
		case DESC_STRING:

			/* suppress HW_TYPE */
			if( strcmp( tag->name, "HW_TYPE" ) == 0 )
				break;
			
			/* BOARD_NAME must be lowercase */
			if( strcmp( tag->name, "BOARD_NAME" ) == 0 )
				strlwr( tag->val.string );
			
			fprintf(fp, "\"%s\"=\"%s\"\n", tag->name, tag->val.string );
			break;
		
		/*-------------------+
		|  out u_int32       |
		+-------------------*/	
		case DESC_U_INT32:

			/* suppress DESC_TYPE */
			if( strcmp( tag->name, "DESC_TYPE" ) == 0 )
				break;
			
			fprintf(fp, "\"%s\"=dword:%x\n", tag->name, tag->val.uInt32 );
			break;

		/*-------------------+
		|  out binary        |
		+-------------------*/	
		case DESC_BINARY:

			nameLen = strlen( tag->name );
			fprintf(fp, "\"%s\"=hex:", tag->name );

			line = 0;
			n = 0;

			/* out all values */
			for( i=0; i<tag->val.uInt8.entries; i++ ){
				fprintf( fp, "%02x%s", tag->val.uInt8.arr[i],
						i==tag->val.uInt8.entries-1 ? "" : "," );
				
				/* first line length */
				if( line == 0 ){
					if( ( (nameLen+12)/3 + (i)) >= 24 ){
						fprintf( fp, "\\\n" );
						line++;
					}
				}
				/* further line length */
				else{
					n++;
					if( (n%24 == 0) && (i != tag->val.uInt8.entries - 1) ){
						fprintf( fp, "\\\n" );
						n=0;
					}
				}

			}
			fprintf( fp, "\n" );
			break;

		} /* switch */
	} /* for */
}
