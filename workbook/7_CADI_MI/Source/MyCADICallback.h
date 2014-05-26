// Class providing callbacks which may be registered to the target. Typically, the
// implementation of these callbacks needs to guarantee thread-safety(!!). That has been
// omitted here due to the simplicity of the example provided with this package.



class MyCADICallback :
    public eslapi::CADICallbackObj
{
public:
    MyCADICallback()
        : currentMode(0),
          modeChangeCallbackCounter(0)
    {}
    
    //used to check if the target provides the correct interface of a feasible revision
    virtual eslapi::CAInterface * ObtainInterface(eslapi::if_name_t    ifName,
                                                  eslapi::if_rev_t     minRev,
                                                  eslapi::if_rev_t *   actualRev);

    virtual uint32_t appliOpen(const char * /*sFileName*/,
                               const char * /*mode*/)
    {
        return (uint32_t)-1;
    }

    virtual void appliInput(uint32_t /*streamId*/,
                            uint32_t /*maxCount*/,
                            uint32_t* /*actualCount*/,
                            char* /*buffer*/)
    {
    }

    virtual void appliOutput(uint32_t streamId,
                             uint32_t maxCount,
                             uint32_t* actualCount,
                             const char* buffer)
    {
        if(actualCount)
            *actualCount = 0;

        if(!buffer)
        {
            printf("appliOutput callback: buffer is NULL");
            return;
        }

        uint32_t count = (uint32_t) strnlen(buffer, maxCount);
        if(count > 0)
        {
            switch(streamId)
            {
                case 1:
                case 2: {
                    char* tmp_buffer = new char[count+1];
                    memcpy(tmp_buffer,buffer,count);
                    tmp_buffer[count] = '\0';
                    fputs(tmp_buffer, streamId==1?stdout:stderr);
                    delete[] tmp_buffer;
                }
                    break;
                default:
                    printf("appliOutput callback: unsupported stream id %u\n", streamId);
            }
        } 

        if(actualCount)
            *actualCount = count;
    }

    virtual uint32_t appliClose(uint32_t /*streamID*/)
    {
        return (uint32_t)-1;
    }

    virtual void doString(const char * stringArg)
    {
        printf("...doString(\"%s\") callback received.\n\n", stringArg);
    }

    virtual void modeChange(uint32_t newMode,
                            eslapi::CADIBptNumber_t bptNumber)
    {
        const char *mode2string[6] = 
            {
                "Stop",
                "Run",
                "Bpt",
                "Error",
                "HighLevelStep",
                "RunUnconditionally"
            };

        if (newMode > 5)
        {
            printf("ERROR: Unknown execution mode (%u) received!\n",
                   newMode);
        }
        
        assert(newMode < 6);
        if (newMode == eslapi::CADI_EXECMODE_Bpt)
        {
            printf("...modeChange(%s, %d) callback received.\n\n",
                   mode2string[newMode],
                   bptNumber);
        }
        else
        {
            printf("...modeChange(%s) callback received.\n\n",
                   mode2string[newMode]);
        }


        currentMode = newMode;
        modeChangeCallbackCounter++;
    }

    virtual void reset(uint32_t resetLevel)
    {
        printf("...reset occured, level = %u\n\n", resetLevel);
    }

    virtual void cycleTick(void) // deprecated
    {}

    virtual void killInterface(void)
    {}

    virtual uint32_t bypass(uint32_t /*commandLength*/,
                            const char * /*command*/,
                            uint32_t /*maxResponseLength*/,
                            char * /*response*/)
    {
        return 0;
    }

    virtual uint32_t lookupSymbol (uint32_t /*symbolLength*/,
                                   const char * /*symbol*/,
                                   uint32_t /*maxResponseLength*/,
                                   char * /*response*/)
    {
        return 0;
    }

    virtual void refresh(uint32_t /*refreshReason*/)
    {}

public:
    // small auxiliary methods to check the state of the target as indicated by callbacks
    uint32_t GetCurrentMode()
    {
        return currentMode;
    };

    uint32_t GetModeChangeCallbackCounter()
    {
        return modeChangeCallbackCounter;
    };

    void ClearModeChangeCallbackCounter()
    {
        modeChangeCallbackCounter = 0;
    };

private:
    uint32_t currentMode;
    uint32_t modeChangeCallbackCounter;
};

// Used to check if the target provides the correct interface of a feasible revision
eslapi::CAInterface *
MyCADICallback::ObtainInterface(eslapi::if_name_t    ifName,
                                eslapi::if_rev_t     minRev,
                                eslapi::if_rev_t *   actualRev)
{
    if((strcmp(ifName,IFNAME()) == 0) && // if someone is asking for the matching interface
       (minRev <= IFREVISION())) // and the revision of this interface implementation is
        // at least what is being asked for
    {
        if (actualRev) // make sure this is not a NULL pointer
        {
            *actualRev = IFREVISION();
        }
        return this;
    }
    if((strcmp(ifName, CAInterface::IFNAME()) == 0) &&
       minRev <= CAInterface::IFREVISION())
    {
        if (actualRev != NULL)
        {
            *actualRev = CAInterface::IFREVISION();
        }
        return this;
    }
    return NULL;
}
