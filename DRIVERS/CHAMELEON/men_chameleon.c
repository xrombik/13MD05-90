/*********************  P r o g r a m  -  M o d u l e ***********************/
/*!
 *        \file  men_chameleon.c
 *
 *      \author  kp/ub/ts
 *        $Date: 2013/08/29 16:49:09 $
 *    $Revision: 1.34 $
 *
 *        \brief MEN Chameleon FPGA driver/device registration
 *
 * Derived from men_chameleon.c contained in EM04A Linux BSP by Klaus Popp.
 *
 * Registers each chameleon FPGA found to the PCI subsystem.
 * Provides a driver registration/probe interface similar to the PCI
 * subsystem.
 *
 * Module parameters:
 * usePciIrq : if 1, use the IRQ from the PCI config table for all
 *             devices in the FPGA - default
 *             if 0 use the IRQ from the Chameleon table. Can be different
 *             for every device. Currently makes sense with EM04/A only.
 *
 * Switches: MEN_CHAMV2_IRQ_OFFSET - needed for onboard chameleon FPGAs
 *                                   with IRQs connected to external CPU
 *                                   IRQs instead of PCI interrupt.
 */
/*---------------------------[ Public Functions ]----------------------------
 *
 *-------------------------------[ History ]---------------------------------
 *
 * ------------- from here on maintained in Git -------------------------
 *
 * $Log: men_chameleon.c,v $
 * Revision 1.34  2013/08/29 16:49:09  ts
 * R: 1. qualifier __devinitdata doesnt exist from 3.8.0 on
 *    2. chameleon IP core variant not taken from chameleon table
 * M: 1. undefine qualifier depending on kernel version
 *    2. also copy IP core variant
 *
 * Revision 1.33  2011/04/14 17:36:26  CRuff
 * R: 1. support of pci domains
 *    2. cosmetics
 * M: 1. in pci_init_one, get the corresponding pci domain to the given bus
 *       number and hand it over to the chameleon library
 *    2. added casts of pci_resource_start to U_INT32_OR_64 before casting it
 *       to a pointer
 *
 * Revision 1.32  2011/02/17 15:18:34  CRuff
 * R: 1. support for kernel versions up to 2.6.37
 * M: 1a) include linux/semaphore.h from kernel 1.6.35
 *    1b) use DEFINE_SEMAPHORE instead of DECLARE_MUTEX from kernel 2.6.36
 *
 * Revision 1.31  2010/10/19 11:59:24  CRuff
 * R: Symbol exports are still needed
 * M: undid changes of revision 1.30
 *
 * Revision 1.30  2010/08/23 17:27:38  CRuff
 * R: 1. chameleon lib is now built as core library (ALL_CORE_LIBS)
 *    2. cosmetics
 * M: 1. removed symbol exports for kernel-builtin module; symbols are now
 *       exported in any case in chameleonv2.c
 *    2. takeover of module description from nat_module.c (is not longer used)
 *
 * Revision 1.29  2010/01/27 20:02:42  rt
 * R: 1) Cosmetics.
 * M: 1) Added debug prints to men_chameleonV2_register_driver().
 *
 * Revision 1.28  2010/01/25 16:27:40  rt
 * R: 1) Add support for onboard FPGAs with IRQs connected to external
 *       CPU IRQs instead of PCI IRQs.
 * M: 1) Added MEN_CHAMV2_IRQ_OFFSET parameter.
 *
 * Revision 1.27  2009/08/19 18:06:15  CRuff
 * R: CHAM_InitIo not available on PPC systems
 * M: call CHAM_InitMem on PPC systems, regardless of the cham table mapping
 *
 * Revision 1.26  2009/08/19 17:05:00  CRuff
 * R: pci_device_id(): variable declaration masked out by define
 * M: move variable declaration out of #ifndef block
 *
 * Revision 1.25  2009/08/19 11:02:07  CRuff
 * R: 1. support I/O mapped chameleon tables
 *    2. double termination of cham func table in error case
 * M: 1a) remove CHAM_INIT defines (done in chameleon.h)
 *    1b) for PCI devices, initialize the Cham table as I/O or Mem mapped,
 *        dependent on the BAR mapping
 *    2. remove Term() on cham func table (already done by InitPci)
 *
 * Revision 1.24  2009/02/06 15:02:24  GLeonhardt
 * R:1. header for sysparam not found
 * M:1. change path for include
 *
 * Revision 1.23  2009/01/27 15:48:38  ts
 * R: EXPORT() calls for chameleon-functions in case of kernel builtin driver were missing
 * M: merged latest changes from rt and added symbol exports for CHAM_InitMem,
 *    CHAM_DevIdToName, CHAM_ModCodeToDevId, CHAM_DevIdToModCode
 *
 * Revision 1.4  2008/09/09 11:53:00  acs
 * PR#1923-elinos: Added latest update from MEN (Mr. Schnuerer).
 *
 * Revision 1.22  2008/12/02 18:10:30  rt
 * R:1.FPGA interrupts not working at EM3/EM9/A17
 * M:1.Set usePciIrq=0 at EM3/EM9/A17 since no PCI interrupts available
 *
 * Revision 1.21  2008/09/05 15:22:12  ts
 * R: 1. ElinOS 4.2 EM1 BSP didnt work for a Customer with Framebuffer support,
 *       because only 16 IPcores per FPGA were seeked in ElinOS 4.2
 * M: 1. merged changes from MEN and Sysgo to last recent, updated Sysgo with
 *       this Version
 *
 * Revision 1.20  2008/08/27 17:38:23  aw
 * R: chameleon base address was written false to / false read from flash
 * M: use %X instead of %d
 *
 * Revision 1.19  2008/06/27 12:12:52  aw
 * R: Base address of chameleon table was needed
 * M: Added SysParamGet to get address from system parameter
 *
 * Revision 1.18  2008/03/13 16:58:08  aw
 * initialize pdev with a initial value at case no pci
 *
 * Revision 1.17  2008/03/12 10:51:21  aw
 * changed define NO_PCI to CONFIG_PCI
 *
 * Revision 1.16  2008/02/22 14:48:28  aw
 * bugfix, last checkin changed moduleparam perms to 666 now its 0664 again
 *
 * Revision 1.15  2008/02/21 15:16:42  aw
 * supports NIOS
 *
 * Revision 1.14  2007/12/10 12:11:06  ts
 * module param permissions changed to 0664
 * using pci_driver_register() instead pci_init_module() from 2.6.22 on
 *
 * Revision 1.2  2007/11/21 16:34:33  con
 * PR#1850: Added support for EM1N and chameleon V2 driver.
 *
 * Revision 1.13  2007/10/24 14:15:33  ts
 * changed:
 * search through chamtable until end marker found, not only max. 16 cores/FPGA
 *
 * Revision 1.12  2007/10/08 13:53:10  ts
 * added seamless Chameleon V2 support
 * dump all found Units unconditionally now, not only if DBG defined
 * removed unnecessary oss_slot_pci_devnbrs[] array, already present in oss.c
 * to be used in men_lx_chameleon.ko module and also as BSP built-in driver
 *
 * Revision 1.11  2007/05/08 14:05:46  ts
 * Cosmetics
 *
 * Revision 1.10  2007/03/28 15:55:12  ts
 * Added support for V2 Tables by calling ChameleonV2 functions
 *
 * Revision 1.8  2007/01/10 18:40:05  ts
 * use correct Macro KERNEL_VERSION
 *
 * Revision 1.7  2007/01/09 12:41:29  ts
 * MODULE_PARM is gone in Kernels > 2.6.14
 * make Parameter usePciIrq = 1 the default
 *
 * Revision 1.6  2005/07/22 10:50:40  ts
 * Bugfix in men_chameleon_find_unit: really copy the found unit data to *unit
 *
 * Revision 1.5  2005/01/25 15:59:23  ub
 * added support for new Chameleon table format (magic word 0xcdef)
 *
 * Revision 1.4  2005/01/05 15:58:34  ub
 * Adapted for Linux kernel 2.6
 *
 * Revision 1.3  2004/12/13 12:30:29  ub
 * cosmetics
 *
 * Revision 1.2  2004/11/29 11:34:19  ub
 * Fixed: mem/io region is now released at driver shutdown
 *
 * Revision 1.1  2004/11/23 09:33:57  ub
 * Initial Revision
 *
 *---------------------------------------------------------------------------
 * (c) Copyright 2004-2009 by MEN Mikro Elektronik GmbH, Nuremberg, Germany
 *
 * This program is free software; you can redistribute  it and/or modify it
 * under  the terms of  the GNU General  Public License as published by the
 * Free Software Foundation;  either version 2 of the  License, or (at your
 * option) any later version.
 ****************************************************************************/
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/pci.h>
#include <linux/list.h>
#include <linux/slab.h>
#include <linux/module.h>
#include <linux/kmod.h>
#include <linux/init.h>
#include <linux/version.h>

#include <MEN/men_chameleon.h>
#include <MEN/oss.h>
#include <MEN/chameleon.h>

#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,35)
#include <linux/semaphore.h>
#endif

/* __devinit qualifiers are removed */
#if LINUX_VERSION_CODE >= KERNEL_VERSION(3,8,0)
# define __devinit
# define __devinitdata
#endif

#undef DEBUG_CHAM_LX

#ifdef DEBUG_CHAM_LX
#define DBGOUT(x...) printk(KERN_DEBUG x)
#else
#define DBGOUT(x...)
#endif /* DBG */

/* length of Cham. table file */
#define CHAM_TBL_FILE_LEN    12

/* module parameters */
static int usePciIrq = 1; /* true for all ESMs except EM8, ... */

#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,14)
MODULE_PARM( usePciIrq, "i" );
#else
module_param( usePciIrq, int, 0664 );
#endif
MODULE_PARM_DESC( usePciIrq, "usePciIrq=1: IRQ# from PCI header. usePciIrq=0: Use IP Core IRQ#");

/*--------------------------------------+
|   DEFINES & CONSTS                    |
+--------------------------------------*/
#define CFGTABLE_READWORD(tbl,idx)   (h->ioMapped ? \
    inw((u16)((u32)tbl + (idx)*4)) : readw((u16*)((u32)tbl + (idx)*4)))

/* pci_module_init became pci_register_driver in 2.6.22, just renamed */
#ifndef pci_module_init
# define pci_module_init pci_register_driver
#endif

/*
 * IP cores per FPGA (v0Unit[] and v2Unit[] arrays), mind that by Spec its
 * unlimited! TODO: use chained List instead v0Unit[] and v2Unit[] !
 */
#define CORES_PER_FPGA		 	256

/** data structure for early access to core FPGAs */
typedef struct {
	u32 bar[4];					/**< base address regs  */
	void *cfgTbl;				/**< mapped config table in bar0 */
	int numUnits;				/**< number of chameleon units of this FPGA */
	int valid;					/**< early_init has been called  */
} CHAMELEON_EARLY_T;

/** data structure to handle version differences between driver and FPGA */
typedef struct {
    int fpga;   /**< version of FPGA unit */
    int unit;    /**< version of the assigned unit */
} CAMELEON_API_VERSION;

/** data structure that handles one instance of a chameleon FPGA */
typedef struct {
    struct list_head node; /**< node in list of chameleon FPGAs 		*/
    struct pci_dev *pdev;  /**< corresponding pci device 				*/
    int numUnits;          /**< number of chameleon units of this FPGA 	*/
    void *cfgTbl;          /**< mapped config table in bar0 			*/
    u32 cfgTblPhys;        /**< phys addr of config table in bar0 		*/
    int ioMapped;          /**< config table in io-mapped bar 			*/
    char variant;
    int revision;
    int chamNum;
    CHAMELEON_UNIT_T v0Unit[CORES_PER_FPGA];
    CHAMELEONV2_UNIT_T v2Unit[CORES_PER_FPGA];
} CHAMELEON_HANDLE_T;

static int G_chamInit=0;              /**< men_chameleon_init was called  	*/
static LIST_HEAD(G_chamLst);          /**< list of chameleon FPGAs 			*/
static LIST_HEAD(G_drvLst);           /**< list of registered drivers 		*/
static LIST_HEAD(G_drvV2Lst);         /**< list of registered V2 drivers  	*/

static CHAM_FUNCTBL G_chamFctTable;     /**< Chameleon function table       */

#ifdef MEN_EARLY_ACCESS
static CHAMELEON_EARLY_T G_early; 		/**< early access vars 				*/
#endif

#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,36)
static DEFINE_SEMAPHORE(cham_probe_sem);
#else
static DECLARE_MUTEX(cham_probe_sem);
#endif

static void cham_probe_lock(void)
{
    down(&cham_probe_sem);
}

static void cham_probe_unlock(void)
{
    up(&cham_probe_sem);
}

/*******************************************************************/
/** Probes driver if it can handle the new unit
 *
 * - does nothing if the unit has already a driver attached
 * - checks if the unit's module code is to be handled by \a drv
 * - if so, calls the driver's probe function
 * - on success, assigns the driver to the unit
 *
 * \return 1 if \a drv assigned to unit, 0 if not
 */
static int chameleon_announce(CHAMELEON_UNIT_T *unit, CHAMELEON_DRIVER_T *drv )
{
    const u16 *modCodeP = drv->modCodeArr;
    int rv = 0;

    if( unit->driver )
        return 0;                   /* already a driver attached */

    while( *modCodeP != CHAMELEON_MODCODE_END ){
        if( *modCodeP == unit->modCode ) {

            /* code match, call driver probe */
            cham_probe_lock();
            if( drv->probe( unit ) >= 0 ){
                unit->driver = drv;
                rv = 1;
                cham_probe_unlock();
                break;
            }
            cham_probe_unlock();
        }
        modCodeP++;
    }
    return rv;
}

/*************************************************************************/
/** Probes driver if it can handle the new unit
 *
 * - does nothing if the unit has already a driver attached
 * - checks if the unit's module code is to be handled by \a drv
 * - if so, calls the driver's probe function
 * - on success, assigns the driver to the unit
 *
 * \return 1 if \a drv assigned to unit, 0 if not
 */
static int chameleonV2_announce(CHAMELEONV2_UNIT_T *unit,
								CHAMELEONV2_DRIVER_T *drv )
{
    const u16 *devIdP = drv->devIdArr;
    int rv = 0;

    if( unit->driver )
        return 0;                   /* already a driver attached */

    while( *devIdP != CHAMELEONV2_DEVID_END ){
        if( *devIdP == unit->unitFpga.devId ){

            /* code match, call driver probe */
            cham_probe_lock();
            if( drv->probe( unit ) >= 0 ){
                unit->driver = drv;
                rv = 1;
                cham_probe_unlock();
                break;
            }
            cham_probe_unlock();
        }
        devIdP++;
    }
    return rv;
}


/*******************************************************************/
/** probes all units of new FPGA against all registered drivers
 *
 */
static void chameleon_announce_fpga(CHAMELEON_HANDLE_T *h)
{
    struct list_head *pos;
    int i;

    for( i=0; i < h->numUnits; i++ ){
        list_for_each( pos, &G_drvLst ) {
            CHAMELEON_DRIVER_T *drv = list_entry( pos,
												  CHAMELEON_DRIVER_T,
												  node );

            chameleon_announce( &h->v0Unit[i], drv );
        }
        list_for_each( pos, &G_drvV2Lst ) {
            CHAMELEONV2_DRIVER_T *drv = list_entry( pos,
													CHAMELEONV2_DRIVER_T,
													node );

            chameleonV2_announce( &h->v2Unit[i], drv );
        }
    }
}

/*******************************************************************/
/** register a new chameleon driver
 *
 * Adds the driver structure to the list of registered drivers.
 * Immediately checks if any known FPGA unit can be handled by the
 * new driver and calls it probe() function if so.
 *
 * \param drv       \IN the driver structure to register. Must be kept
 *                      intact by caller.
 *
 * \return the number of chameleon units which were claimed by the driver
 *          during registration.  The driver remains registered even if the
 *          return value is zero.
 */
int men_chameleon_register_driver( CHAMELEON_DRIVER_T *drv )
{
    struct list_head *pos;
    int i;
    int count=0;
    CHAMELEON_HANDLE_T *h;

    list_add_tail(&drv->node, &G_drvLst);

    list_for_each( pos, &G_chamLst ) {
        h = list_entry( pos, CHAMELEON_HANDLE_T, node );

        for( i=0; i < h->numUnits; i++ ){
/* --leo 27.08.2007 10:07 */
            /* DBGOUT("reg_driver, modCode: %d\n", h->v0Unit[i].modCode); */
            if (chameleon_announce( &h->v0Unit[i], drv )) {
/* --leo 27.08.2007 10:15 */
                /* DBGOUT("\t-->\t accepted\n"); */
                count++;
            }
        }
    }
    return count;
}

/*******************************************************************/
/** register a new chameleon V2 driver
 *
 * Adds the driver structure to the list of registered drivers.
 * Immediately checks if any known FPGA unit can be handled by the
 * new driver and calls it probe() function if so.
 *
 * \param drv       \IN the driver structure to register. Must be kept
 *                      intact by caller.
 *
 * \return the number of chameleon units which were claimed by the driver
 *          during registration.  The driver remains registered even if the
 *          return value is zero.
 */
int men_chameleonV2_register_driver( CHAMELEONV2_DRIVER_T *drv )
{
    struct list_head *pos;
    int i;
    int count=0;

    list_add_tail(&drv->node, &G_drvV2Lst);

    list_for_each( pos, &G_chamLst ) {
        CHAMELEON_HANDLE_T *h = list_entry( pos, CHAMELEON_HANDLE_T, node );

        for( i=0; i < h->numUnits; i++ ){
            DBGOUT("reg_driver, devId: %d\n", h->v2Unit[i].unitFpga.devId);
            if (chameleonV2_announce( &h->v2Unit[i], drv )) {
            	DBGOUT("\t-->\t accepted\n");
                count++;
            }
        }
    }
    return count;
}

/*******************************************************************/
/** unregister a chameleon driver
 *
 * Deletes the driver structure from the list of registered chameleon drivers,
 * gives it a chance to clean up by calling its remove() function for
 * each device it was responsible for, and marks those devices as
 * driverless.
 *
 *
 * \param drv       \IN the driver structure to register. Must be kept
 *                      intact by caller.
 */
void men_chameleon_unregister_driver( CHAMELEON_DRIVER_T *drv )
{
    struct list_head *pos;
    int i;

    list_del(&drv->node );

    list_for_each( pos, &G_chamLst ) {
        CHAMELEON_HANDLE_T *h = list_entry( pos, CHAMELEON_HANDLE_T, node );

        for( i=0; i < h->numUnits; i++ ){
            if( (h->v0Unit[i].driver == drv)
             && (drv->remove) ){
                drv->remove( &h->v0Unit[i] );
            }
            h->v0Unit[i].driver = NULL;
            h->v0Unit[i].driver_data = NULL;
        }
    }
}

/*******************************************************************/
/** unregister a chameleon V2 driver
 *
 * Deletes the driver structure from the list of registered chameleon drivers,
 * gives it a chance to clean up by calling its remove() function for
 * each device it was responsible for, and marks those devices as
 *
 *
 * \param drv       \IN the driver structure to register. Must be kept
 *                      intact by caller.
 */
void men_chameleonV2_unregister_driver( CHAMELEONV2_DRIVER_T *drv )
{
    struct list_head *pos;
    int i;

    list_del(&drv->node );

    list_for_each( pos, &G_chamLst ) {
        CHAMELEON_HANDLE_T *h = list_entry( pos, CHAMELEON_HANDLE_T, node );

        for( i=0; i < h->numUnits; i++ ){
            if( (h->v2Unit[i].driver == drv)
             && (drv->remove) ){
                drv->remove( &h->v2Unit[i] );
            }
            h->v2Unit[i].driver = NULL;
            h->v2Unit[i].driver_data = NULL;
        }
    }
}

#ifdef MEN_EARLY_ACCESS
/*******************************************************************/
/** Initialize early access to MEN chameleon FPGA
 *
 * This should be called from the platform specific init code if required
 *
 * Supports only one instance of a chameleon FPGA.
 *
 * initializes the global variable G_early so that routines
 * men_chameleon_early_unit_find and men_chameleon_early_unit_ident can work
 *
 * \param hose		\IN early pci access controller structure
 * \param devNo		\IN pci device number of FPGA
 *
 * \return 0 on success or negative linux error number
 */
int men_chameleon_early_init(
	struct pci_controller *hose,
	int devNo)
{
	int i;
    int dev_fun = PCI_DEVFN( devNo, 0 );
    u16 venId;

    G_early.valid = 0;
	/* make sure device is present */
	if( early_read_config_word( hose, 0, dev_fun, PCI_VENDOR_ID, &venId ) ||
		(venId == 0xffff))
		return -ENODEV;

	/*-------------------------+
	|  Read base address regs  |
	+-------------------------*/
    for( i=0; i<4; ++i ) {
    	if( early_read_config_dword( hose, 0, dev_fun, PCI_BASE_ADDRESS_0+i*4,
									 &G_early.bar[i] )) {
            return -EIO;
        }
		/* printk( "BAR%d: 0x%08x\n", i, G_early.bar[i] ); */
    }

    G_early.cfgTbl = ioremap_nocache( G_early.bar[0], CHAMELEON_CFGTABLE_SZ );

#if 0 /* for debug purposes */
	printk( "Dumping Chameleon Table:");
    for( i=0; i<256; ++i ) {
		/* linefeed every 16 values */
		if (!(i%16))
			printk( "\n");
		printk( "%04x ", CFGTABLE_READWORD( G_early.cfgTbl,i));
    }

#endif


	/*--------------------------------------------+
	|  Check if config table contains sync word   |
	+--------------------------------------------*/
    if( (CFGTABLE_READWORD( G_early.cfgTbl,1) != FPGA_SYNC_MAGIC_ABCD) && \
        (CFGTABLE_READWORD( G_early.cfgTbl,1) != FPGA_SYNC_MAGIC_CDEF)) {
		printk( "unknown Magic word 0x%04x from G_early.bar[0]=0x%08x\n",
				CFGTABLE_READWORD(G_early.cfgTbl,1), G_early.bar[0] );
		iounmap( G_early.cfgTbl );
        return -ENODEV;
    }

	/*--------------------------------------------+
	|  Determine number of units in config table  |
	+--------------------------------------------*/
	for( i=0; i<CHAMELEON_MAX_UNITS; i++ ){
		/* check for end marker (device is all 1's) */
		if((CFGTABLE_READWORD( G_early.cfgTbl, (i+1)*2 ) & 0x3f0 ) == 0x3f0)
			break;
	}

	G_early.numUnits = i;
	G_early.valid = 1;

    printk( "MEN Chameleon early number modules: %d\n", G_early.numUnits);

    return 0;
}


/*******************************************************************/
/** Identify nth unit of chameleon FPGA initialized by men_chameleon_early_init
 */
int
men_chameleon_early_unit_ident(
    int idx,
    CHAMELEON_UNIT_T *unit )
{
    if( ! G_early.valid )
        return -ENODEV;

	if( idx >= G_early.numUnits )
		return -EINVAL;

    /* fill unit info */
	fill_unit_info( NULL, G_early.cfgTbl, unit, idx );

	unit->phys 		= (void *)(G_early.bar[unit->bar] + unit->offset);
	unit->chamNum	= 0;
	unit->pdev		= NULL;

/*     printk( "modCode: %d  bar:%d  offset:0x%x  addr:0x%p\n",  */
/* 	  unit->modCode, unit->bar, unit->offset, unit->phys ); */

    return 0;
}


/*******************************************************************/
/** Find unit in chameleon FPGA initialized by men_chameleon_early_init
 */
int men_chameleon_early_unit_find(
	int modCode,
	int instance,
	CHAMELEON_UNIT_T *unit)
{
	int idx = 0;
    if( !G_early.valid )
        return -ENODEV;

	while( men_chameleon_early_unit_ident( idx, unit ) == 0 ){

		if( ((unit->modCode == modCode) || (modCode == -1 )) &&
			((unit->instance == instance) || (instance == -1)))

			return 0;

		idx++;
	}

    printk( "unit not found. searched %d units\n", idx );
	dump_stack();
    return -ENODEV;
}

EXPORT_SYMBOL(men_chameleon_early_unit_find);
EXPORT_SYMBOL(men_chameleon_early_unit_ident);
EXPORT_SYMBOL(men_chameleon_early_init);


#endif /* MEN_EARLY_ACCESS */

/*******************************************************************/
/** Find the system wide nth occurrance of a chameleon module \a modCode
 *
 * \param modCode   \IN  module code to search
 * \param idx       \IN  nth occurance of module (system wide!)
 * \param unit      \OUT filled with unit information
 * \return 0 on success or negative linux error number
 */
int men_chameleon_unit_find(
    int modCode,
    int idx,
    CHAMELEON_UNIT_T *unit)
{
    struct list_head *pos;
    int count=0, i;

/* 	ChameleonUnitFind(CHAMELEON_HANDLE *h, modCode, idx,  */
/* 					  CHAMELEON_UNIT *info); */

    list_for_each( pos, &G_chamLst ) {
     CHAMELEON_HANDLE_T *h = list_entry( pos, CHAMELEON_HANDLE_T, node );
        for( i=0; i < h->numUnits; i++ ){
            if( h->v0Unit[i].modCode == modCode ){
                if( idx == count ){
					/* found, copy unit data */
                    memcpy(unit, &(h->v0Unit[i]), sizeof(CHAMELEON_UNIT_T));
                    return 0;
                }
                count++;
            }
        }
    }
    return -ENODEV;
}

/*******************************************************************/
/** Find the system wide nth occurrance of a chameleon module \a devId
 *
 * \param devId     \IN  device id to search
 * \param idx       \IN  nth occurance of module (system wide!)
 * \param unit      \OUT filled with unit information
 * \return 0 on success or negative linux error number
 */
int men_chameleonV2_unit_find(
    int devId,
    int idx,
    CHAMELEONV2_UNIT_T *unit)
{
    struct list_head *pos;
    int count=0, i;


    list_for_each( pos, &G_chamLst ) {
     CHAMELEON_HANDLE_T *h = list_entry( pos, CHAMELEON_HANDLE_T, node );
        for( i=0; i < h->numUnits; i++ ){
            if( h->v2Unit[i].unitFpga.devId == devId ){
                if( idx == count ){
                    /* found, copy unit data */
                    memcpy(unit, &(h->v2Unit[i]), sizeof(CHAMELEONV2_UNIT_T));
                    return 0;
				}
				count++;
            }
        }
    }
    return -ENODEV;
}

/*******************************************************************/
/** Probe/initialize Chameleon FPGA.
 *
 * Gets called (during execution of pci_register_driver for already
 * existing devices or later if a new device gets inserted) for all
 * PCI devices which match the ID table and are not handled by the
 * other drivers yet. This function gets passed a pointer to the
 * pci_dev structure representing the device and also which entry in
 * the ID table did the device match. It returns zero when the driver
 * has accepted the device or an error code (negative number)
 * otherwise.
 *
 *  \param pdev     pci_dev structure representing the device
 *  \param ent      entry in PCI table that matches
 *
 *  \return     0 when the driver has accepted the device or
 *              an error code (negative number) otherwise.
 */

static int __devinit pci_init_one (
    struct pci_dev *pdev,
    const struct pci_device_id *ent)
{

    OSS_HANDLE *osH 		= NULL;
    CHAMELEON_HANDLE_T *h	= NULL;  	/* this is the native struct */
    CHAMELEONV2_UNIT 		info;
    CHAMELEON_UNIT_T 		*v0unit;
    CHAMELEONV2_UNIT_T 		*v2unit;
    CHAMELEONV2_TABLE   	table;
    CHAMELEONV2_HANDLE 		*chamHdl;
    char name[21]; 		/* buffer to keep unit name/index */
    char tblfile[CHAM_TBL_FILE_LEN+1];
    int rv=-ENOMEM, err = 0, idx, i;
    int32 chamResult;
    u32 value32;

	if ( ( err = OSS_Init( "men_chameleon", &osH ) ) ) {
		printk( KERN_ERR "*** Error during OSS_Init!()\n");
		return err;
	}

	/* allocate internal handles */
    if( (h = kmalloc( sizeof(*h), GFP_KERNEL )) == NULL )
        goto CLEANUP;
    memset( h, 0, sizeof(*h));  /* clear handle */

    printk( KERN_INFO "\nFound MEN Chameleon FPGA at bus %d dev %02x\n", pdev->bus->number, pdev->devfn >> 3);

    /* initialize Chameleon handle depending on IO or mem mapped BAR0 */
    pci_read_config_dword( pdev, PCI_BASE_ADDRESS_0, &value32 );

    if ( value32 & PCI_BASE_ADDRESS_SPACE_IO ) {  /* io mapped */
#ifndef CONFIG_PPC
    	chamResult = CHAM_InitIo( &G_chamFctTable );
#else
		/* for PPCs, no I/O Mapping is supported; init as Mem mapped */
		chamResult = CHAM_InitMem( &G_chamFctTable );
#endif
    } else {
    	chamResult = CHAM_InitMem( &G_chamFctTable );
    }

    if(chamResult != CHAMELEON_OK){
        printk(KERN_ERR "*** Error during Chameleon_Init!\n");
        return chamResult;
    }

    /* Initialize Chameleon library */
    chamResult = G_chamFctTable.InitPci( osH,
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,11)
					 OSS_MERGE_BUS_DOMAIN(pdev->bus->number, pci_domain_nr(pdev->bus)),
#else
										pdev->bus->number,
#endif
										pdev->devfn>>3, 0, &chamHdl );
	if( chamResult != CHAMELEON_OK ) {
        printk( KERN_ERR "*** Error during Chameleon_Init (InitPci)!\n");
        return chamResult;
    }

    /* Ident Table */
    chamResult = G_chamFctTable.TableIdent( chamHdl, 0, &table );
    h->revision = table.revision;
    h->variant  = table.model;

    for (i=0; i < CHAM_TBL_FILE_LEN; i++)
    	tblfile[i] = (table.file[i] != 0) ? table.file[i] : ' ';

    tblfile[CHAM_TBL_FILE_LEN]='\0';

    printk(KERN_INFO "FPGA File='%s' table model=0x%02x('%c') "
	   "Revision %d Magic 0x%04X\n", tblfile , table.model, table.model, table.revision, table.magicWord );

    printk( KERN_INFO " Unit                devId   Grp Rev  Var  Inst IRQ   BAR  Offset       Addr\n");
    for (i=0; i < 79; i++)
        printk("-");
    printk("\n");

    /* gather all IP cores until end marker found */
    idx = 0;

	while ((chamResult=G_chamFctTable.UnitIdent(chamHdl,idx,&info)) != CHAMELEONV2_NO_MORE_ENTRIES ) {

        v0unit = &h->v0Unit[idx];
        v2unit = &h->v2Unit[idx];

        /* format Unit index and Name so tab spacing never breaks */
        sprintf(name, "%02d %-17s", idx, CHAM_DevIdToName( info.devId) );
        printk(KERN_INFO " %s"  					/* name 			*/
			             "0x%04x %2d   %2d   %2d"	/* devId/Group/Rev/Var. */
                         "   0x%02x"				/* instance 		*/
                         "\t0x%02x"					/* interrupt 		*/
                         "   %d   0x%08x"			/* BAR / offset  	*/
                         "   0x%p\n",				/* addr 			*/
	       name, info.devId, info.group, info.revision, info.variant, info.instance, info.interrupt, info.bar, (unsigned int)info.offset, info.addr);

        /* Copy the Units info */
        v0unit->modCode   			= CHAM_DevIdToModCode( info.devId );
        v0unit->revision  			= info.revision;
        v0unit->instance  			= info.instance;
        v0unit->irq       			= info.interrupt;
        v0unit->bar       			= info.bar;
        v0unit->offset    			= info.offset;
        v0unit->phys      			= (void *)(U_INT32_OR_64)(pci_resource_start(pdev,info.bar) + info.offset);
        v2unit->unitFpga.devId     	= info.devId;
        v2unit->unitFpga.group     	= info.group;
        v2unit->unitFpga.revision  	= info.revision;
        v2unit->unitFpga.instance  	= info.instance;
        v2unit->unitFpga.variant   	= info.variant;
        v2unit->unitFpga.interrupt 	= info.interrupt;
        v2unit->unitFpga.bar       	= info.bar;
        v2unit->unitFpga.offset    	= info.offset;
        v2unit->unitFpga.size      	= info.size;
        v2unit->unitFpga.addr  		= (void *)(U_INT32_OR_64)(pci_resource_start(pdev,info.bar) + info.offset);

        /* if specified use IRQ given by PCI config space */
        if( usePciIrq ) {
            v0unit->irq = pdev->irq;
            v2unit->unitFpga.interrupt = pdev->irq;
        }

        v0unit->pdev      = pdev;
        v0unit->chamNum   = h->chamNum;
        v2unit->pdev      = pdev;
        v2unit->chamNum   = h->chamNum;

        idx++;
    }

    h->numUnits = idx;

    list_add_tail( &h->node, &G_chamLst);

    /*--------------------------------+
    |  Inform all registered drivers  |
    +--------------------------------*/
    chameleon_announce_fpga( h );

    rv = 0;

	if (pci_enable_device( pdev )) {
		printk(KERN_ERR "failed to pci_enable_device(). Something is very wrong...\n");
		return -ENODEV;
	}

    return rv;

 CLEANUP:
    if( h ){
        if( h->cfgTbl )
            iounmap( h->cfgTbl );

        if( h->cfgTblPhys ) {
            if( h->ioMapped )
                release_region( h->cfgTblPhys, CHAMELEON_CFGTABLE_SZ );
            else
                release_mem_region( h->cfgTblPhys, CHAMELEON_CFGTABLE_SZ );
        }

        kfree( h );
    }
    return rv;
}

/*
 * PCI Vendor/Device ID table.
 * Driver will handle all devices that have these codes
 */
static struct pci_device_id G_pci_tbl[] __devinitdata = {
    { CHAMELEON_PCI_VENID_ALTERA, 0x5104 ,  PCI_ANY_ID, PCI_ANY_ID },   /* EM04 				*/
    { CHAMELEON_PCI_VENID_ALTERA, 0x454d ,  PCI_ANY_ID, 	0x0441 },   /* EM04A MEN PCI core	*/
    { CHAMELEON_PCI_VENID_ALTERA, 0x0008 ,  PCI_ANY_ID, PCI_ANY_ID },   /* EM07  				*/
    { CHAMELEON_PCI_VENID_ALTERA, 0x000a ,  PCI_ANY_ID, PCI_ANY_ID },   /* F401  				*/
    { CHAMELEON_PCI_VENID_ALTERA, 0x000b ,  PCI_ANY_ID, PCI_ANY_ID },   /* F206  				*/
    { CHAMELEON_PCI_VENID_ALTERA, 0x0013 ,  PCI_ANY_ID, PCI_ANY_ID },   /* F206i 				*/
    { CHAMELEON_PCI_VENID_ALTERA, 0x0009 ,  PCI_ANY_ID, PCI_ANY_ID },   /* F206 Trainguard 		*/
    { CHAMELEON_PCI_VENID_ALTERA, 0x4d45 ,  PCI_ANY_ID, PCI_ANY_ID },   /* Chameleon general ID */
	/* care for future devices with new MEN own PCI vendor ID. Look at every PCI device with this ID. */
	{ CHAMELEON_PCI_VENID_MEN, PCI_ANY_ID, PCI_ANY_ID, PCI_ANY_ID },
    { 0,}
};

static struct pci_driver G_pci_driver = {
    .name       =       "men-chameleon",
    .id_table   =       G_pci_tbl,
    .probe      =       pci_init_one,
};

int men_chameleon_init(void)
{
    int rv;

    if( G_chamInit )
        return 0;               /* already initialized */

    printk( KERN_INFO "Init MEN Chameleon PNP subsystem\n" );

    if( (rv =  pci_module_init( &G_pci_driver )) < 0 )
        return rv;

    G_chamInit++;
    return 0;
}


void men_chameleon_cleanup( void )
{
    struct list_head *pos = NULL;

    printk( KERN_INFO "men_chameleon_cleanup\n" );

    list_for_each( pos, &G_chamLst ) {
        CHAMELEON_HANDLE_T *h = list_entry( pos, CHAMELEON_HANDLE_T, node );
        if( h->cfgTblPhys ) {
            if( h->ioMapped )
                release_region( h->cfgTblPhys, CHAMELEON_CFGTABLE_SZ );
            else
                release_mem_region( h->cfgTblPhys, CHAMELEON_CFGTABLE_SZ );
        }
        kfree( h );
    }
    G_chamInit--;
    pci_unregister_driver( &G_pci_driver );
}

EXPORT_SYMBOL(men_chameleon_unit_find);
EXPORT_SYMBOL(men_chameleon_register_driver);
EXPORT_SYMBOL(men_chameleon_unregister_driver);
EXPORT_SYMBOL(men_chameleonV2_unit_find);
EXPORT_SYMBOL(men_chameleonV2_register_driver);
EXPORT_SYMBOL(men_chameleonV2_unregister_driver);

/* are we kernel-builtin? export V2 funcs or BBIS drivers cant find them. */
#ifdef CONFIG_MEN_CHAMELEON
EXPORT_SYMBOL(CHAM_InitMem);
EXPORT_SYMBOL(CHAM_DevIdToName);
EXPORT_SYMBOL(CHAM_ModCodeToDevId);
EXPORT_SYMBOL(CHAM_DevIdToModCode);
#endif

MODULE_LICENSE( "GPL" );
module_init(men_chameleon_init);
module_exit(men_chameleon_cleanup);
