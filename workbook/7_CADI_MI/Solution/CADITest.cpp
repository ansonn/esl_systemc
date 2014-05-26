/*

  This confidential and proprietary software may be used only as
  authorized by a licensing agreement from ARM Limited.

  Copyright (c) 2008 ARM Limited
  All rights reserved.

  The entire notice above must be reproduced on all authorised copies
  and copies may only be made to the extent permitted by a licensing
  agreement from ARM Limited.

*/

#ifndef WIN32
# include <unistd.h> 
# include <dlfcn.h>
#include <cstdlib>
#include <cstdio>
#include "pthread.h"
#else
//# include "libdlWin.hpp"
# include <wtypes.h>
# include <winbase.h>
#include <windows.h>
#endif

#include <string>
#include <assert.h>

#include "eslapi/CADIFactory.h"
#include "eslapi/CADI.h"
#include "eslapi/CADITypes.h"

#include <sg/SGComponentRegistry.h>
#include <scx/scx.h>

#include "MyCADICallback.h"

#include "CADITest.h"

/////////////////////////////////////////////////////////////////////////////
// Globals
eslapi::CADISimulation *simulation;
eslapi::CADI           *cadi;
MyCADICallback         *cadi_callback;
/////////////////////////////////////////////////////////////////////////////



void connect(bool verbose)
{

    uint32_t simulation_num=0;
    uint32_t target_num = 1;
    eslapi::CAInterface *ca_interface;

    // get global interface
    ca_interface = scx::scx_get_global_interface();

    // get access to CADISimulation
    const char *errorMessage = "(no error)";
    simulation = sg::obtainComponentInterfacePointer<eslapi::CADISimulation>(ca_interface, "cadiSimulation", &errorMessage);
    if (!simulation)
    {
        printf("dump_cadi_targets(): error: cannot get CADISimulation interface for component cadiSimulation: %s\n",
               errorMessage);
        return;
    } 
    // Now get the info an all targets:
    // Simplification: Use fixed upper limit for number of targets:
    const uint32_t           desiredNumberOfTargetInfos = 100;
    eslapi::CADITargetInfo_t target_infos[desiredNumberOfTargetInfos];
    uint32_t                 actualNumberOfTargetInfos = 0;
    eslapi::CADIReturn_t         status;

    // Getting information on targets available for this CADISimulation;
    // based on these information a caller may decide which target
    // to connect to

    // 7.1: ADD YOUR CODE HERE - Get list of the avilable simulation targets
    status = simulation->GetTargetInfos(0, // startTargetInfoIndex
                                        desiredNumberOfTargetInfos,
                                        target_infos,
                                        &actualNumberOfTargetInfos);
    if (status != eslapi::CADI_STATUS_OK)
    {
        printf("ERROR: Getting target infos for simulation #%u failed.\n",
               simulation_num);
        exit(-1);
    }


    // 7.1: ADD YOUR CODE HERE - Print a list of targets
    for(unsigned int i=0; i < actualNumberOfTargetInfos; ++i)
    {
        printf("    Target #%u: ID:%u   Name:%s   Instance:%s\n",
             i,
             target_infos[i].id,
             target_infos[i].targetName,
             target_infos[i].instanceName);
    }


    printf("\n***Connecting to target #%u of simulation #%u:\n",
           target_num,
           simulation_num);

    // This call returns a pointer to the CADI interface of a target; actually, the returned pointer is of type
    // CAInterface* which is a base class for the CADI class. Its ObtainInterface() method has to be called
    // before casting it to the desired CADI* type.
    // 7.1: ADD YOUR CODE HERE - Connect to target number "target_num"
    ca_interface = simulation->GetTarget(target_infos[target_num].id);
    if (ca_interface == 0)
    {
        printf("ERROR: Getting pointer to target #%u failed.\n",
               target_num);
        exit(-1);
    }

    // Now cast the CAInterface-pointer to a CADI-pointer which allows debug access to the desired target
    printf("    Successfully obtained a CADI 2.0 interface pointer.\n");
      cadi = (static_cast<eslapi::CADI *>(ca_interface));
}


// ----------------------------------------------------------------------------------------


void registers(bool verbose)
{
    uint32_t group_to_test = 0;
    uint32_t register_to_test = 16;

    //
    // Get register groups
    //

    printf("\n***Retrieving information for register %d in group %d\n", register_to_test, group_to_test);

    const uint32_t         desiredNumberOfRegisterGroups = 20;
    eslapi::CADIRegGroup_t register_groups[desiredNumberOfRegisterGroups];
    uint32_t               actualNumberOfRegisterGroups = 0;
    eslapi::CADIReturn_t   status;

    // 7.2: ADD YOUR CODE HERE - Get the list of register groups
     status = cadi->CADIRegGetGroups(0, //groupIndex
                                    desiredNumberOfRegisterGroups,
                                    &actualNumberOfRegisterGroups,
                                    register_groups);

    if (status != eslapi::CADI_STATUS_OK)
        printf("ERROR: Getting register group information from target failed.\n");

    assert(eslapi::CADI_STATUS_OK == status);

    //
    // Get registers in group
    //

    printf("      Group %d     : %s\n", register_groups[group_to_test].groupID, register_groups[group_to_test].name);

    eslapi::CADIRegInfo_t* registers              = new eslapi::CADIRegInfo_t[40]();
    uint32_t               actualNumberOfRegInfos = 0;

    // 7.2: ADD YOUR CODE HERE - Get register map for group 'group_to_test'
    status = cadi->CADIRegGetMap(group_to_test,//register_groups[i].groupID,
                                 0, //startRegisterIndex
                                 40,//register_groups[i].numRegsInGroup,
                                 &actualNumberOfRegInfos,
                                 registers);

    if (status != eslapi::CADI_STATUS_OK) 
        printf("ERROR: Getting register information from target failed.\n");

    assert(eslapi::CADI_STATUS_OK == status);

    printf("      Register %d : %s  0x", registers[register_to_test].regNumber, registers[register_to_test].name);

 
    //
    // Read a register
    //

    uint32_t          numRegsRead = 0;
    eslapi::CADIReg_t reg;

    // The register to be tested is given in 'register_to_test'
    reg.regNumber = registers[register_to_test].regNumber;
    memset(reg.bytes, 0, (sizeof(uint8_t) * 16));
    reg.isUndefined = false;
    reg.attribute = registers[0].attribute;

    // 7.2: ADD YOUR CODE HERE - read a register
    status = cadi->CADIRegRead(1, //regCount
                               &reg,
                               &numRegsRead,
                               0); //doSideEffects

    if (status != eslapi::CADI_STATUS_OK)
        printf("ERROR: Getting register value from target failed.\n");

    assert(eslapi::CADI_STATUS_OK == status);

    for (unsigned int k = 1; k <= 4; ++k)
    {
         printf("%02x", reg.bytes[4 - k]);
    }
    printf("\n");
}

// ----------------------------------------------------------------------------------------

void callbacks(void)
{
    cadi_callback = new MyCADICallback();
    eslapi::CADIReturn_t status;

    // This vector is used to enable callback methods of the CADICallback object;
    // disabled callbacks may not be called for the corresponding target
    char cadi_callback_enables[eslapi::CADI_CB_Count] = {0};
    cadi_callback_enables[eslapi::CADI_CB_AppliOutput] = 1;
    cadi_callback_enables[eslapi::CADI_CB_String] = 1;
    cadi_callback_enables[eslapi::CADI_CB_ModeChange] = 1;
    cadi_callback_enables[eslapi::CADI_CB_Reset] = 1;

    printf("***Adding callback object to target...\n");

    // Adding a callback object to the target enables the target to issue valuable
    // information from inside to cause an appropriate reaction of the caller (e.g.
    // refreshing a debugger's views or control buttons)

    // 7.3: ADD YOUR CODE HERE - Register the call back object with the simulation
    status = cadi->CADIXfaceAddCallback(static_cast<eslapi::CADICallbackObj*>(cadi_callback), cadi_callback_enables);

    if (status != eslapi::CADI_STATUS_OK)
    {
        printf("ERROR: Adding CADI callback object to target failed.\n");
        exit(-1);
    }

    printf("    Successfully added callback object.\n");
}


// ----------------------------------------------------------------------------------------

void run_control(void)
{
    eslapi::CADIReturn_t status;
/*  this is the begining of hello.axf     single step takes pc from 0x8000 to 0x8008
    __main
        0x00008000:    eb000000    ....    BL       __scatterload ; 0x8008
        0x00008004:    eb000022    "...    BL       __rt_entry ; 0x8094
    !!!scatter
    __scatterload
    __scatterload_rt2
        0x00008008:    e28f002c    ,...    ADR      r0,{pc}+0x34 ; 0x803c
        0x0000800c:    e8900c00    ....    LDM      r0,{r10,r11}
        0x00008010:    e08aa000    ....    ADD      r10,r10,r0
*/

    printf("\n**** Executing single step..... PC = 0x%llx \n", cadi->CADIGetPC());

   // 7.3: ADD YOUR CODE HERE - single step the processor
    status = cadi->CADIExecSingleStep(1, true, false);

    if (status != eslapi::CADI_STATUS_OK)
    {
        printf("CADIExecSingleStep returned an error\n");
        exit(1);
    }

    // 7.3: ADD YOUR CODE HERE - Wait for the simulation to report the current mode as running
    while (cadi_callback->GetCurrentMode() != eslapi::CADI_EXECMODE_Run){};

    // 7.3: ADD YOUR CODE HERE - Wait for the simulation to report the current mode as stopped
    while (cadi_callback->GetCurrentMode() != eslapi::CADI_EXECMODE_Stop){};

    printf("---- Step successful, current PC = 0x%llx \n", cadi->CADIGetPC());

    eslapi::CADIBptRequest_t *bpt = new eslapi::CADIBptRequest_t;
    eslapi::CADIBptNumber_t *bptnum = new eslapi::CADIBptNumber_t;
    bpt->type = eslapi::CADI_BPT_PROGRAM;
    bpt->address.location.addr = 0x80B8;
    bpt->enabled = 1;

    printf("\n\n**** Setting program breakpoint at address 0x%llx ....\n",bpt->address.location.addr);

    // 7.3: ADD YOUR CODE HERE - Set a breakpoint on address 0x8010
    status = cadi->CADIBptSet(bpt, bptnum);

    if (status != eslapi::CADI_STATUS_OK)
    {
        printf("unable to set breakpoint\n");
        exit(1);
    }

    printf("---- Breakpoint set successfully: %d \n\n", *bptnum);
    printf("**** CADIExecContinue\n");

    // 7.3: ADD YOUR CODE HERE - Run the simulation
    status = cadi->CADIExecContinue();

    if (status != eslapi::CADI_STATUS_OK)
    {
        printf("CADIExecContinue returned an error\n");
        exit(1);
    }
    // 7.3: ADD YOUR CODE HERE - Wait for the simulation to report the current mode as running
    while (cadi_callback->GetCurrentMode() != eslapi::CADI_EXECMODE_Run){};

    // 7.3: ADD YOUR CODE HERE - Wait for the simulation to report the current mode as stopped on breakpoint
    while (cadi_callback->GetCurrentMode() != eslapi::CADI_EXECMODE_Bpt){};

    // 7.3: ADD YOUR CODE HERE - Wait for the simulation to report the current mode as EXECMODE stop
    while (cadi_callback->GetCurrentMode() != eslapi::CADI_EXECMODE_Stop){};  // wait until stop

    printf("---- Breakpoint hit,   PC=0x%llx\n", cadi->CADIGetPC());

}

// ----------------------------------------------------------------------------------------


void close(bool verbose)
{
    // Before shutting down the connetion to the target, all registerd callback objects
    // need to be removed; otherwise the following may happen:
    //   In case of closing a debugger which is not the only connected one, a target
    //   does not necessarily have to shut down. If a closed debugger misses to remove
    //   its callback objects, the target might try to trigger those callbacks. This 
    //   will most likely end up in an corrupted memory access.
    printf("    Successfully removed callback object from target.\n");
#if 1
    // This Release()-call shall inform the simulation that the caller will NOT access
    // the corresponding object anymore. Hence, it may be safely destroyed.
    // (The boolean parameter indicates whether to shut the simulation down or to leave
    // it running in order to allow other callers to connect to it or to finishe their job).
    printf("\n***Releasing simulation...\n");
    simulation->Release(true); // release and shut down the simulation
#endif

#ifdef WIN32
    ExitThread(0);
#else
    pthread_exit(NULL);
#endif


}


// ----------------------------------------------------------------------------------------



void* cadi_thread_fxn(void* target_num_)
{

    connect(true);
    
    // workaround for SDDKW-9565
    eslapi::CADITargetFeatures_t target_features;
    eslapi::CADIReturn_t status = cadi->CADIXfaceGetFeatures(&target_features);

    if (status != eslapi::CADI_STATUS_OK)
    {
        printf("ERROR: Getting CADI target features from target failed.\n");
        exit(-1);
    }
  /////////////////////

    // Un-comment in 7.2
    registers(true);

    // Un-comment in 7.3
    callbacks();
    run_control();

    close(false);

    return 0;
}

void cadi_start_thread(void)
{
    uint32_t target_num = 1;

#ifdef WIN32
    DWORD threadID; // Win32 thread ID
    HANDLE thread = CreateThread(NULL, 16*1024*1024, (LPTHREAD_START_ROUTINE)cadi_thread_fxn,
                                 (void *) target_num, 0, &threadID);
    if (thread == NULL){
        printf("ERROR; CreateThread failed\n");
        exit(-1);
    }
#else
    pthread_t cadi_thread;
    int rc = pthread_create(&cadi_thread, NULL, cadi_thread_fxn, (void *) target_num);
    if (rc){
        printf("ERROR; return code from pthread_create() is %d\n", rc);
        exit(-1);
    }
#endif
}

// End of file
