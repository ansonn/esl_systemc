/*
 * Copyright 2011 ARM Limited. All rights reserved.
 * 
 * SimpleCADI.h - convenience layer around the CADI debug interface
 */

#ifndef _SimpleCADI_h_
#define _SimpleCADI_h_

#include <eslapi/CADI.h>
#include <eslapi/CADIDisassembler.h>
#include <string>
#include <map>
#include <vector>

// simple disassembler callback class to reveive one line of disassembly
class SimpleCADIDisassemblerCB: public eslapi::CADIDisassemblerCB
{
public:
    virtual eslapi::CAInterface *ObtainInterface(eslapi::if_name_t ifName, eslapi::if_rev_t minRev, eslapi::if_rev_t *actualRev)
    {
        if ((strcmp(ifName, eslapi::CAInterface::IFNAME()) == 0) && (minRev <= eslapi::CAInterface::IFREVISION()))
        {
            if (actualRev) *actualRev = eslapi::CAInterface::IFREVISION();
            return static_cast<eslapi::CAInterface*>(this);
        }
        else if ((strcmp(ifName, eslapi::CADIDisassemblerCB::IFNAME()) == 0) && (minRev <= eslapi::CADIDisassemblerCB::IFREVISION()))
        {
            if (actualRev) *actualRev = eslapi::CADIDisassemblerCB::IFREVISION();
            return static_cast<eslapi::CADIDisassemblerCB*>(this);
        }
        else
            return 0;
    }
    virtual void ReceiveModeName(uint32_t mode, const char *modename) {}
    virtual void ReceiveSourceReference(const eslapi::CADIAddr_t &addr, const char *sourceFile, uint32_t sourceLine) {}
    virtual void ReceiveDisassembly(const eslapi::CADIAddr_t &addr, const char *opcodes, const char *disassembly)
    {
        disAddress = addr;
        disOpcodes = opcodes;
        disDisassembly = disassembly;
    }
    
    // public data
    eslapi::CADIAddr_t disAddress;
    std::string disOpcodes;
    std::string disDisassembly;
};


// SimpleCADI is a convenience wrapper around a CADI interface
// from the client (debugger) point of view.
// 
// This is NOT an official interface between binary modules. Binary
// compatibility will generally not be maintained between any version.
// Source code compatibility will be maintained as far as possible.
// 
// Some functions intentionally have the same signature as the 
// corresponding functions in Model Debuggers mxscript implementation.
class SimpleCADI
{
public:
    // constructor and destructor
    // (you can also use setCADI() any time)
    SimpleCADI(eslapi::CADI *cadi_ = 0);
    ~SimpleCADI();

    // set/get CADI interface
    void setCADI(eslapi::CADI *cadi_);
    eslapi::CADI *getCADI();

    // --- main API -------------------------------------------------------------
    
    // global configuration
    void hideETFRRegisters(bool hide = true);
    
    // register read/write
    uint64_t regRead(const std::string& regName);
    void regWrite(const std::string& regName, uint64_t value);
    bool hasRegister(const std::string& regName);
    std::vector<std::string> getRegisterNames();

    // memory read/write
    uint64_t memRead(uint32_t memorySpace, uint64_t address, uint32_t sizeInBytes = 1, uint32_t numAccesses = 1);
    void memWrite(uint32_t memorySpace, uint64_t address, uint64_t value, uint32_t sizeInBytes = 1, uint32_t numAccesses = 1);
    
    // disassembly
    std::string disassemble(uint64_t address, uint32_t memorySpaceId, uint32_t disassemblyMode);
    
    // get instruction count
    uint64_t getInstCount();

    // breakpoints
    uint32_t bpAdd(uint64_t address, uint32_t memorySpace); // returns bptId
    void bpRemove(uint32_t bptId);
    
    // execution control
    void run();
    void stop();
    
    // static info about the target
    const char *getTargetName();
    const char *getTargetVersion();
    const char *getInstanceName();

    // return true if last call failed in some way, else false
    bool gotError() const;

    // public helpers
    static uint64_t lebytes2u64(const uint8_t *lebytes);
    static void u642lebytes(uint64_t, uint8_t *lebytes_out);

               
private:
    // private data
    eslapi::CADI *cadi;
    eslapi::CADIReturn_t lastError;
    eslapi::CADIDisassemblerStatus lastDisassemblerStatus;
    bool haveStaticRegInfo;
    typedef std::map<std::string,uint32_t> RegIDMap;
    RegIDMap regName2regid; // register name (R0 or Group.R0) to register id (regNumber)
    bool hideETFRRegisters_;
    bool haveTargetFeatures;
    eslapi::CADITargetFeatures_t targetFeatures;
    bool haveTargetInfo;
    eslapi::CADITargetInfo_t targetInfo;
    
    // helpers
    uint64_t noInterfaceError();
    uint64_t illegalArgumentError();
    void initStaticRegInfo();
    void initStaticRegInfoForGroup(uint32_t groupID, uint32_t numRegsInGroup, const std::string &prefix, eslapi::CADITargetFeatures_t *targetFeatures);
    void initTargetFeatures();
    void initTargetInfo();
    void init(); // initialize (clear) all data
};

// --- implementation --------------------------------------------------------------------------------

inline SimpleCADI::SimpleCADI(eslapi::CADI *cadi_)    
{
    setCADI(cadi_);
}
    
inline void SimpleCADI::setCADI(eslapi::CADI *cadi_) 
{
    init();
    cadi = cadi_;
}    

inline eslapi::CADI *SimpleCADI::getCADI()
{
    return cadi;
}

inline SimpleCADI::~SimpleCADI()
{
}

inline void SimpleCADI::hideETFRRegisters(bool hide)
{
    init();
    hideETFRRegisters_ = hide;
}

inline void SimpleCADI::init()
{
    cadi = 0;
    lastError = eslapi::CADI_STATUS_OK;
    lastDisassemblerStatus = eslapi::CADI_DISASSEMBLER_STATUS_OK;
    haveStaticRegInfo = false;
    haveTargetFeatures = false;
    haveTargetInfo = false;
    regName2regid.clear();
    hideETFRRegisters_ = true;
}

inline uint64_t SimpleCADI::lebytes2u64(const uint8_t *lebytes)
{
    uint32_t lo = lebytes[0] | (lebytes[1] << 8) | (lebytes[2] << 16) | (lebytes[3] << 24);
    uint32_t hi = lebytes[4] | (lebytes[5] << 8) | (lebytes[6] << 16) | (lebytes[7] << 24);
    return (uint64_t(hi) << 32) | lo;
}

inline void SimpleCADI::u642lebytes(uint64_t value, uint8_t *lebytes_out)
{
    uint32_t lo = (uint32_t)value;
    uint32_t hi = (uint32_t)(value >> 32);
    lebytes_out[0] = (uint8_t)lo;
    lebytes_out[1] = (uint8_t)(lo >> 8);
    lebytes_out[2] = (uint8_t)(lo >> 16);
    lebytes_out[3] = (uint8_t)(lo >> 24);
    lebytes_out[4] = (uint8_t)hi;
    lebytes_out[5] = (uint8_t)(hi >> 8);
    lebytes_out[6] = (uint8_t)(hi >> 16);
    lebytes_out[7] = (uint8_t)(hi >> 24);
}

inline uint64_t SimpleCADI::getInstCount()
{
    if (!cadi)
        return noInterfaceError();
    uint64_t count = 0;
    lastError = cadi->CADIGetInstructionCount(count);
    return count;
}

inline void SimpleCADI::run()
{
    if (!cadi)
    {
        noInterfaceError();
        return; 
    }
    lastError = cadi->CADIExecContinue();
}

inline void SimpleCADI::stop()
{
    if (!cadi)
    {
        noInterfaceError();
        return;
    }
    lastError = cadi->CADIExecStop();
}

inline uint64_t SimpleCADI::regRead(const std::string& regName)
{
    if (!cadi)
        return noInterfaceError();
    initStaticRegInfo();

    // get regid
    RegIDMap::const_iterator i = regName2regid.find(regName);
    if (i == regName2regid.end())
    {
        lastError = eslapi::CADI_STATUS_IllegalArgument;
        return 0;
    }
    uint32_t regid = i->second;

    // read reg value
    eslapi::CADIReg_t reg;
    reg.regNumber = regid;
    uint32_t actualCount = 0;
    lastError = cadi->CADIRegRead(1, &reg, &actualCount, 0);
    return lebytes2u64(reg.bytes);
}

inline void SimpleCADI::regWrite(const std::string& regName, uint64_t value)
{
    if (!cadi)
    {
        noInterfaceError();
        return;
    }
    initStaticRegInfo();

    // get regid
    RegIDMap::const_iterator i = regName2regid.find(regName);
    if (i == regName2regid.end())
    {
        lastError = eslapi::CADI_STATUS_IllegalArgument;
        return;
    }
    uint32_t regid = i->second;

    // write reg value
    eslapi::CADIReg_t reg;
    reg.regNumber = regid;
    u642lebytes(value, reg.bytes);
    uint32_t actualCount = 0;
    lastError = cadi->CADIRegWrite(1, &reg, &actualCount, 0);
}

inline bool SimpleCADI::hasRegister(const std::string& regName)
{
    if (!cadi)
    {
        noInterfaceError();
        return false;
    }
    initStaticRegInfo();
    RegIDMap::const_iterator i = regName2regid.find(regName);
    return i != regName2regid.end();
}

inline std::vector<std::string> SimpleCADI::getRegisterNames()
{
    std::vector<std::string> reglist;
    if (!cadi)
    {
        noInterfaceError();
        return reglist;
    }
    initStaticRegInfo();
    for (RegIDMap::const_iterator i = regName2regid.begin(); i != regName2regid.end(); i++)
        reglist.push_back(i->first);
    return reglist;
}

// memory read/write
inline uint64_t SimpleCADI::memRead(uint32_t memorySpace, uint64_t address, uint32_t sizeInBytes, uint32_t numAccesses)
{
    if (!cadi)
    {
        return noInterfaceError();
    }
    if ((sizeInBytes == 0) || (numAccesses == 0) || ((sizeInBytes * numAccesses) > 8))
        return illegalArgumentError();
    eslapi::CADIAddrComplete_t startAddress;
    startAddress.location.addr = address;
    startAddress.location.space = memorySpace;
    uint32_t actualNum = 0;
    uint8_t data[8] = {0};
    lastError = cadi->CADIMemRead(startAddress, numAccesses, sizeInBytes, data, &actualNum, 0);
    if (lastError != eslapi::CADI_STATUS_OK)
        return 0;
    if (actualNum != numAccesses)
    {
        lastError = eslapi::CADI_STATUS_GeneralError;
        return 0;
    }
    return lebytes2u64(data);
}

inline void SimpleCADI::memWrite(uint32_t memorySpace, uint64_t address, uint64_t value, uint32_t sizeInBytes, uint32_t numAccesses)
{
    if (!cadi)
    {
        noInterfaceError();
        return;
    }
    if ((sizeInBytes == 0) || (numAccesses == 0) || ((sizeInBytes * numAccesses) > 8))
    {
        illegalArgumentError();
        return; 
    }
    eslapi::CADIAddrComplete_t startAddress;
    startAddress.location.addr = address;
    startAddress.location.space = memorySpace;
    uint32_t actualNum = 0;
    uint8_t data[8] = {0};
    u642lebytes(value, data);
    lastError = cadi->CADIMemWrite(startAddress, numAccesses, sizeInBytes, data, &actualNum, 0);
    if (actualNum != numAccesses)
        lastError = eslapi::CADI_STATUS_GeneralError;
}

// disassembly
inline std::string SimpleCADI::disassemble(uint64_t address, uint32_t memorySpaceId, uint32_t disassemblyMode)
{
    if (!cadi)
    {
        noInterfaceError();
        return "";
    }
    eslapi::CADIDisassembler *dis = cadi->CADIGetDisassembler();
    if (!dis)
    {
        noInterfaceError();
        return "";
    }
    eslapi::CADIAddr_t addr(memorySpaceId, address);
    eslapi::CADIAddr_t nextAddr;
    SimpleCADIDisassemblerCB callback;
    lastDisassemblerStatus = dis->GetDisassembly(&callback, addr, nextAddr, disassemblyMode);
    lastError = (lastDisassemblerStatus == eslapi::CADI_DISASSEMBLER_STATUS_OK) ? eslapi::CADI_STATUS_OK : eslapi::CADI_STATUS_GeneralError;
    return callback.disDisassembly;
}

inline const char *SimpleCADI::getTargetName()
{
    initTargetFeatures();
    return targetFeatures.targetName;
}

inline const char *SimpleCADI::getTargetVersion()
{
    initTargetFeatures();
    return targetFeatures.targetVersion;
}

inline const char *SimpleCADI::getInstanceName()
{
    initTargetInfo();
    return targetInfo.instanceName;
}

inline uint64_t SimpleCADI::noInterfaceError()
{
    // this is probably strange enough to tell any client that something is severely wrong
    lastError = eslapi::CADI_STATUS_InsufficientResources;
    return 0;
}

inline uint64_t SimpleCADI::illegalArgumentError()
{
    // this is probably strange enough to tell any client that something is severely wrong
    lastError = eslapi::CADI_STATUS_IllegalArgument;
    return 0;
}

inline void SimpleCADI::initStaticRegInfo()
{
    if (haveStaticRegInfo)
        return;
    if (!cadi)
    {
        noInterfaceError();
        return;
    }
    
    regName2regid.clear();
    
    // get number of reg groups from target features
    initTargetFeatures();   
    uint32_t nrRegisterGroups = targetFeatures.nrRegisterGroups;
    
    // get register groups
    uint32_t actualGroups = 0;
    eslapi::CADIRegGroup_t* regGroups = new eslapi::CADIRegGroup_t[nrRegisterGroups];
    lastError = cadi->CADIRegGetGroups(0, nrRegisterGroups, &actualGroups, regGroups);
    if ((lastError != eslapi::CADI_STATUS_OK) || (actualGroups != nrRegisterGroups))
    {
        delete[] regGroups;
        return;
    }

    // get total number of regs in all groups
    uint32_t numAllRegs = 0;
    for (uint32_t i = 0; i < actualGroups; i++)
        numAllRegs += regGroups[i].numRegsInGroup;
    
    // get regs from pseudogroup CADI_REG_ALLGROUPS (without group prefix)
    initStaticRegInfoForGroup(eslapi::CADI_REG_ALLGROUPS, numAllRegs, "", &targetFeatures);
    
    // get regs from all groups
    for (uint32_t i = 0; i < actualGroups; i++)
        initStaticRegInfoForGroup(regGroups[i].groupID, regGroups[i].numRegsInGroup, std::string(regGroups[i].name) + ".", &targetFeatures);
    delete[] regGroups;

    // done
    haveStaticRegInfo = true;
}

inline void SimpleCADI::initStaticRegInfoForGroup(uint32_t groupID, uint32_t numRegsInGroup, const std::string &prefix, eslapi::CADITargetFeatures_t *targetFeatures)
{
    // get register infos
    uint32_t actualRegs = 0;
    eslapi::CADIRegInfo_t* regInfos = new eslapi::CADIRegInfo_t[numRegsInGroup];
    lastError = cadi->CADIRegGetMap(groupID, 0, numRegsInGroup, &actualRegs, regInfos);
    if ((lastError != eslapi::CADI_STATUS_OK) || (actualRegs != numRegsInGroup))
    {
        delete[] regInfos;
        return;
    }
    
    // fill map
    for (uint32_t i = 0; i < actualRegs; i++)
    {
        std::string name = prefix + regInfos[i].name;
        uint32_t regid = regInfos[i].regNumber;
        
        // hide ETFR registers (extended target features registers)
        if (hideETFRRegisters_ &&
            targetFeatures->nExtendedTargetFeaturesRegNumValid &&
            (targetFeatures->nExtendedTargetFeaturesRegNum == regid))
            continue;
        
        // we always give the first occurrence the priority in case registers in the same group (or globally)
        // have the same name
        RegIDMap::const_iterator it = regName2regid.find(name);
        if (it == regName2regid.end())
            regName2regid[name] = regid;
    }
    
    delete[] regInfos;
}

inline void SimpleCADI::initTargetFeatures()
{
    if (haveTargetFeatures)
        return;

    targetFeatures = eslapi::CADITargetFeatures_t();
    lastError = cadi->CADIXfaceGetFeatures(&targetFeatures);
    if (lastError != eslapi::CADI_STATUS_OK)        
        targetFeatures = eslapi::CADITargetFeatures_t();
    else
        haveTargetFeatures = true;
}

inline void SimpleCADI::initTargetInfo()
{
    if (haveTargetInfo)
        return;

    targetInfo = eslapi::CADITargetInfo_t();
    lastError = cadi->CADIGetTargetInfo(&targetInfo);
    if (lastError != eslapi::CADI_STATUS_OK)        
        targetInfo = eslapi::CADITargetInfo_t();
    else
        haveTargetInfo = true;
}

// breakpoints

// returns bptId
inline uint32_t SimpleCADI::bpAdd(uint64_t address, uint32_t memorySpace)
{
    if (!cadi)
    {
        noInterfaceError();
        return 0xffffffffu;
    }

    // prepare breakpoint request
    eslapi::CADIBptRequest_t request;
    request.address.location.addr = address;
    request.address.location.space = memorySpace;
    request.type = eslapi::CADI_BPT_PROGRAM;
    request.enabled = true;

    // set breakpoint and return id
    eslapi::CADIBptNumber_t id = 0;
    lastError = cadi->CADIBptSet(&request, &id);
    if (lastError != eslapi::CADI_STATUS_OK)
        return 0xffffffffu;
    return uint32_t(id);
}

inline void SimpleCADI::bpRemove(uint32_t bptId)
{
    if (!cadi)
    {
        noInterfaceError();
        return;
    }
    
    // remove vreakpoint
    lastError = cadi->CADIBptClear(bptId);
}

inline bool SimpleCADI::gotError() const
{
    return lastError != eslapi::CADI_STATUS_OK;
}

#endif

