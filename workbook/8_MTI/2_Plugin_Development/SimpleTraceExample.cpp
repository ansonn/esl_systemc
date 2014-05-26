/*!
 *  Work book session 8
 *  
 */

#include "MTI/PluginInterface.h"
#include "MTI/PluginFactory.h"
#include "MTI/PluginInstance.h"
#include "MTI/ModelTraceInterface.h"

#include <list>
#include <string>
#include <algorithm>
#include <cassert>
#include <cstdio>

using namespace std;
using namespace eslapi;
using namespace MTI;

/////////////////////////////////////////////////////////////////////////////

class SimpleTraceExample :
    public PluginInstance
{
public:
    virtual CAInterface * ObtainInterface(if_name_t    ifName,
                                          if_rev_t     minRev,
                                          if_rev_t *   actualRev);

    SimpleTraceExample(const char *instance_name,
                       uint32_t num_parameters,
                       CADIParameterValue_t *parameter_values);


    /** This is to associate a plugin with a simulation instance. Exactly one simulation must be registered. */
    virtual CADIReturn_t RegisterSimulation(CAInterface *simulation);

    // This is called before the plugin .dll/.so is unloaded and should allow the plugin to do it's cleanup.
    virtual void Release();

    virtual const char *GetName() const;

public: // methods
    string instance_name;
    ValueIndex inst_pc_index;

    void
    TracePC(const MTI::EventClass *event_class,
            const struct MTI::EventRecord *record);

    static void
    TracePC_Thunk(void * user_data,
                  const MTI::EventClass *event_class,
                  const struct MTI::EventRecord *record);

    CADIReturn_t Error(const char *message) const;
};

CAInterface * 
SimpleTraceExample::ObtainInterface(if_name_t    ifName,
                              if_rev_t     minRev,
                              if_rev_t *   actualRev)
{
    if((strcmp(ifName,PluginInstance::IFNAME()) == 0) && // if someone is asking for the matching interface
       (minRev <= PluginInstance::IFREVISION())) // and the revision of this interface implementation is
        // at least what is being asked for
    {
        if (actualRev) // make sure this is not a NULL pointer
            *actualRev = PluginInstance::IFREVISION();
        return static_cast<PluginInstance *>(this);
    }

    if((strcmp(ifName, CAInterface::IFNAME()) == 0) &&
       minRev <= CAInterface::IFREVISION())
    {
        if (actualRev != NULL)
            *actualRev = CAInterface::IFREVISION();
        return static_cast<PluginInstance *>(this);
    }
    return NULL;
}


SimpleTraceExample::SimpleTraceExample(const char *instance_name,
                           uint32_t /*number_parameters*/,
                           CADIParameterValue_t * /*parameter_values*/) :
    instance_name(instance_name),
    inst_pc_index(-1)
{
}

CADIReturn_t
SimpleTraceExample::Error(const char *message) const
{
    fprintf(stderr, "Error: Trace plugin %s: %s\n", instance_name.c_str(), message);
    return CADI_STATUS_GeneralError;
}

CADIReturn_t
SimpleTraceExample::RegisterSimulation(CAInterface *ca_interface)
{
    if (!ca_interface)
        return CADI_STATUS_IllegalArgument;

    // Get System Trace Interface
    ca_interface = ca_interface->ObtainInterface(SystemTraceInterface::IFNAME(),
                                                 SystemTraceInterface::IFREVISION(), 0);
    SystemTraceInterface *sti = static_cast<SystemTraceInterface *>(ca_interface);


    // Check for components supporting trace
    // 8.2: ADD YOUR CODE HERE - get the number of components with trace interfaces
    SystemTraceInterface::TraceComponentIndex num_trace_components =

    if (num_trace_components == 0)
        return Error("No components provide trace.");


    // Connect to component 1
    // 8.2: ADD YOUR CODE HERE - get the trace interface of component 1
    ca_interface =
    
    ca_interface = ca_interface->ObtainInterface(ComponentTraceInterface::IFNAME(),
                                                 ComponentTraceInterface::IFREVISION(), 0);
    ComponentTraceInterface *cti = static_cast<ComponentTraceInterface *>(ca_interface);

    printf("Attached %s to component: %s\n", instance_name.c_str(), sti->GetComponentTracePath(0));


    // For this example we will assume that the only component
    // in our system with a trace interface is the processor.  ARM's
    // processor models include a trace source called "INST".  This
    // trace source has a field called "PC".

    // Get trace source named "INST"
    // 8.2: ADD YOU CODE HERE - get trace source "INST"
    TraceSource *source =
    if (!source)
        return Error("No trace source \"INST\" found.");


    // Get trace field "PC"
    // 8.2: ADD YOUR CODE HERE - get the "PC" field
    const EventFieldType *field =
    if (!field)
        return Error("No field named \"PC\" found in \"INST\" trace source.");
    FieldMask mask = 1 << field->GetIndex();


    // Now we need to register an event class, so our call-back
    // function can be called whenever new trace data is available

    // Instiate and register an event class
    // 8.2: ADD YOU CODE HERE - create an event class from the INST trace source
    EventClass *event_class =
    if (!event_class)
        return Error("Problem when creating EventClass.");
    inst_pc_index = event_class->GetValueIndex("PC");


    // Register the call-back function "TracePC_Thunk" with the event
    // class
    // 8.2: ADD YOU CODE HERE - register TracePC_Thunk as a call-back passing 'this' as 2nd argument
    Status status =
    if (status != MTI_OK)
        return Error("EventClass::RegisterCallback() returned error.");


    inst_pc_index = event_class->GetValueIndex("PC");
    if (inst_pc_index == -1)
        return Error("EventClass::GetValueIndex(\"PC\") returned error.");

    return CADI_STATUS_OK;
}

// This is called before the plugin .dll/.so is unloaded and should allow the plugin to do it's cleanup.
void 
SimpleTraceExample::Release()
{
    delete this;
}

const char *
SimpleTraceExample::GetName() const
{
    return instance_name.c_str();
}

/////////////////////////////////////////////////////////////////////////////

void SimpleTraceExample::TracePC(const MTI::EventClass *event_class,
                                 const struct MTI::EventRecord *record)
{
    uint32_t pc = record->Get<uint32_t>(event_class, inst_pc_index);
    printf("PC: 0x%08x\n", pc);
}

// The callback interface is a C interface, so we need a short thunk
// function to call into a C++ method. This is done via a static C++
// method, which behaves like a C function:
void
SimpleTraceExample::TracePC_Thunk(void * user_data,
                                  const MTI::EventClass *event_class,
                                  const struct MTI::EventRecord *record)
{
    if (user_data)
    {
       SimpleTraceExample *simple_trace = reinterpret_cast<SimpleTraceExample *>(user_data);
       simple_trace->TracePC(event_class, record);
    }
    else
    {
      fprintf(stderr, "No userdata defined in callback, did you include 'this' when registering the callback?\n");
      while(1);
    }
}

/////////////////////////////////////////////////////////////////////////////

class ThePluginFactory :
    public PluginFactory
{
public:
    virtual CAInterface * ObtainInterface(if_name_t    ifName,
                                          if_rev_t     minRev,
                                          if_rev_t *   actualRev);

    virtual uint32_t GetNumberOfParameters();

    virtual CADIReturn_t GetParameterInfos(CADIParameterInfo_t *parameter_info_list);

    virtual CAInterface *Instantiate(const char *instance_name,
                                     uint32_t number_of_parameters,
                                     CADIParameterValue_t *parameter_values);

    virtual void Release();

    virtual const char *GetType() const { return "SimpleTraceExample"; }
    virtual const char *GetVersion() const { return "$Revision$"; }
};

CAInterface *
ThePluginFactory::ObtainInterface(if_name_t    ifName,
                                  if_rev_t     minRev,
                                  if_rev_t *   actualRev)
{
    if((strcmp(ifName,IFNAME()) == 0) && // if someone is asking for the matching interface
       (minRev <= IFREVISION())) // and the revision of this interface implementation is
        // at least what is being asked for
    {
        if (actualRev) // make sure this is not a NULL pointer
            *actualRev = IFREVISION();
        return static_cast<ThePluginFactory *>(this);
    }

    if((strcmp(ifName, CAInterface::IFNAME()) == 0) &&
       minRev <= CAInterface::IFREVISION())
    {
        if (actualRev) // make sure this is not a NULL pointer
            *actualRev = CAInterface::IFREVISION();
        return static_cast<CAInterface *>(this);
    }
    return NULL;
}

uint32_t
ThePluginFactory::GetNumberOfParameters()
{
    return 0;
}


CADIReturn_t
ThePluginFactory::GetParameterInfos(CADIParameterInfo_t* /*parameter_info_list*/)
{
    return CADI_STATUS_CmdNotSupported;
}


CAInterface *
ThePluginFactory::Instantiate(const char *instance_name,
                              uint32_t    number_of_parameters,
                              CADIParameterValue_t *param_values)
{
    return static_cast<PluginInstance *>(new SimpleTraceExample(instance_name, number_of_parameters, param_values));
}

void
ThePluginFactory::Release()
{
    // nothing to do since factory is a static instance
}

ThePluginFactory factory_instance;

CAInterface *
GetCAInterface()
{
    return &factory_instance;
}

// End of file SimpleTraceExample.cpp
