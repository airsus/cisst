/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id$

  Author(s):  Peter Kazanzides, Anton Deguet
  Created on: 2008-11-13

  (C) Copyright 2008-2010 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#include <cisstMultiTask/mtsInterfaceRequired.h>

#include <cisstMultiTask/mtsFunctionVoid.h>
#include <cisstMultiTask/mtsFunctionVoidReturn.h>
#include <cisstMultiTask/mtsFunctionWrite.h>
#include <cisstMultiTask/mtsFunctionWriteReturn.h>
#include <cisstMultiTask/mtsFunctionRead.h>
#include <cisstMultiTask/mtsFunctionQualifiedRead.h>
#include <cisstMultiTask/mtsEventReceiver.h>

#include <cisstCommon/cmnSerializer.h>
#include <cisstMultiTask/mtsInterfaceProvided.h>
#include <cisstMultiTask/mtsParameterTypes.h>

mtsInterfaceRequired::mtsInterfaceRequired(const std::string & interfaceName,
                                           mtsMailBox * mailBox,
                                           mtsRequiredType required):
    mtsInterfaceRequiredOrInput(interfaceName, required),
    MailBox(mailBox),
    InterfaceProvided(0),
    MailBoxSize(DEFAULT_MAIL_BOX_AND_ARGUMENT_QUEUES_SIZE),
    ArgumentQueuesSize(DEFAULT_MAIL_BOX_AND_ARGUMENT_QUEUES_SIZE),
    FunctionsVoid("FunctionsVoid"),
    FunctionsVoidReturn("FunctionsVoidReturn"),
    FunctionsWrite("FunctionsWrite"),
    FunctionsWriteReturn("FunctionsWriteReturn"),
    FunctionsRead("FunctionsRead"),
    FunctionsQualifiedRead("FunctionsQualifiedRead"),
    EventReceiversVoid("EventReceiversVoid"),
    EventReceiversWrite("EventReceiversWrite"),
    EventHandlersVoid("EventHandlersVoid"),
    EventHandlersWrite("EventHandlersWrite")
{
    FunctionsVoid.SetOwner(*this);
    FunctionsVoidReturn.SetOwner(*this);
    FunctionsWrite.SetOwner(*this);
    FunctionsWriteReturn.SetOwner(*this);
    FunctionsRead.SetOwner(*this);
    FunctionsQualifiedRead.SetOwner(*this);
    EventReceiversVoid.SetOwner(*this);
    EventReceiversWrite.SetOwner(*this);
    EventHandlersVoid.SetOwner(*this);
    EventHandlersWrite.SetOwner(*this);
}


mtsInterfaceRequired::~mtsInterfaceRequired()
{
    FunctionsVoid.DeleteAll();
    FunctionsVoidReturn.DeleteAll();
    FunctionsWrite.DeleteAll();
    FunctionsWriteReturn.DeleteAll();
    FunctionsRead.DeleteAll();
    FunctionsQualifiedRead.DeleteAll();
}

const mtsInterfaceProvidedOrOutput * mtsInterfaceRequired::GetConnectedInterface(void) const
{
    return InterfaceProvided;
}

bool mtsInterfaceRequired::SetMailBoxSize(size_t desiredSize)
{
    if (!this->MailBox) {
        CMN_LOG_CLASS_INIT_WARNING << "SetMailBoxSize: interface \"" << this->GetName()
                                   << "\" is not queuing commands, calling SetMailBoxSize has no effect"
                                   << std::endl;
        return false;
    }
    if (this->GetConnectedInterface()) {
        CMN_LOG_CLASS_INIT_ERROR << "SetMailBoxSize: interface \"" << this->GetName()
                                 << "\", can't modify mail box size while the interface is connected."
                                 << std::endl;
        return false;
    }
    this->MailBoxSize = desiredSize;
    this->MailBox->SetSize(desiredSize);
    return true;
}


bool mtsInterfaceRequired::SetArgumentQueuesSize(size_t desiredSize)
{
    if (!this->MailBox) {
        CMN_LOG_CLASS_INIT_WARNING << "SetArgumentQueuesSize: interface \"" << this->GetName()
                                   << "\" is not queuing commands, calling SetArgumentQueuesSize has no effect"
                                   << std::endl;
        return false;
    }
    if (this->GetConnectedInterface()) {
        CMN_LOG_CLASS_INIT_ERROR << "SetArgumentQueuesSize: interface \"" << this->GetName()
                                 << "\", can't modify argument queues size while the interface is connected."
                                 << std::endl;
        return false;
    }
    if (desiredSize > this->MailBoxSize) {
        CMN_LOG_CLASS_INIT_WARNING << "SetArgumentQueuesSize: interface \"" << this->GetName()
                                   << "\" new size (" << desiredSize
                                   << ") is smaller than command mail box size (" << this->MailBoxSize
                                   << "), the extra space won't be used" << std::endl;
    }
    this->ArgumentQueuesSize = desiredSize;
    return true;
}


bool mtsInterfaceRequired::SetMailBoxAndArgumentQueuesSize(size_t desiredSize)
{
    return (this->SetMailBoxSize(desiredSize)
            && this->SetArgumentQueuesSize(desiredSize));
}


bool mtsInterfaceRequired::UseQueueBasedOnInterfacePolicy(mtsEventQueueingPolicy queueingPolicy,
                                                          const std::string & methodName,
                                                          const std::string & eventName)
{
    bool interfaceHasMailbox = (this->MailBox != 0);
    if (interfaceHasMailbox) {
        if (queueingPolicy == MTS_EVENT_NOT_QUEUED) {
            CMN_LOG_CLASS_INIT_DEBUG << methodName << ": event handler for \"" << eventName
                                     << "\" will not be queued while the corresponding required interface \""
                                     << this->GetName() << "\" uses queued event handlers by default." << std::endl;
            return false;
        } else {
            return true;
        }
    } else {
        if (queueingPolicy == MTS_EVENT_QUEUED) {
            CMN_LOG_CLASS_INIT_ERROR  << methodName << ": event handler for \"" << eventName
                                      << "\" has been added as queued while the corresponding required interface \""
                                      << this->GetName() << "\" has been created without a mailbox." << std::endl;
            return true;
        } else {
            return false;
        }
    }
}


bool mtsInterfaceRequired::AddEventHandlerToReceiver(const std::string & eventName, mtsCommandVoid * handler) const
{
    bool result = false;
    ReceiverVoidInfo * receiverInfo = EventReceiversVoid.GetItem(eventName, CMN_LOG_LEVEL_RUN_VERBOSE);
    if (receiverInfo) {
        receiverInfo->Pointer->SetHandlerCommand(handler);
        result = true;
    }
    return result;
}


bool mtsInterfaceRequired::AddEventHandlerToReceiver(const std::string & eventName, mtsCommandWriteBase * handler) const
{
    bool result = false;
    ReceiverWriteInfo *receiverInfo = EventReceiversWrite.GetItem(eventName, CMN_LOG_LEVEL_RUN_VERBOSE);
    if (receiverInfo) {
        receiverInfo->Pointer->SetHandlerCommand(handler);
        result = true;
    }
    return result;
}


std::vector<std::string> mtsInterfaceRequired::GetNamesOfFunctions(void) const
{
    std::vector<std::string> commands = GetNamesOfFunctionsVoid();
    std::vector<std::string> tmp = GetNamesOfFunctionsVoidReturn();
    commands.insert(commands.begin(), tmp.begin(), tmp.end());
    tmp.clear();
    tmp = GetNamesOfFunctionsWrite();
    commands.insert(commands.begin(), tmp.begin(), tmp.end());
    tmp.clear();
    tmp = GetNamesOfFunctionsWriteReturn();
    commands.insert(commands.begin(), tmp.begin(), tmp.end());
    tmp.clear();
    tmp = GetNamesOfFunctionsRead();
    commands.insert(commands.begin(), tmp.begin(), tmp.end());
    tmp.clear();
    tmp = GetNamesOfFunctionsQualifiedRead();
    commands.insert(commands.begin(), tmp.begin(), tmp.end());
    return commands;
}


std::vector<std::string> mtsInterfaceRequired::GetNamesOfFunctionsVoid(void) const
{
    return FunctionsVoid.GetNames();
}


std::vector<std::string> mtsInterfaceRequired::GetNamesOfFunctionsVoidReturn(void) const
{
    return FunctionsVoidReturn.GetNames();
}


std::vector<std::string> mtsInterfaceRequired::GetNamesOfFunctionsWrite(void) const
{
    return FunctionsWrite.GetNames();
}


std::vector<std::string> mtsInterfaceRequired::GetNamesOfFunctionsWriteReturn(void) const
{
    return FunctionsWriteReturn.GetNames();
}


std::vector<std::string> mtsInterfaceRequired::GetNamesOfFunctionsRead(void) const
{
    return FunctionsRead.GetNames();
}


std::vector<std::string> mtsInterfaceRequired::GetNamesOfFunctionsQualifiedRead(void) const
{
    return FunctionsQualifiedRead.GetNames();
}

mtsFunctionVoid * mtsInterfaceRequired::GetFunctionVoid(const std::string & functionName) const
{
    return dynamic_cast<mtsFunctionVoid *>(FunctionsVoid.GetItem(functionName, CMN_LOG_LOD_INIT_ERROR)->Pointer);
}

mtsFunctionVoidReturn * mtsInterfaceRequired::GetFunctionVoidReturn(const std::string & functionName) const
{
    return dynamic_cast<mtsFunctionVoidReturn *>(FunctionsVoidReturn.GetItem(functionName, CMN_LOG_LOD_INIT_ERROR)->Pointer);
}

mtsFunctionWrite * mtsInterfaceRequired::GetFunctionWrite(const std::string & functionName) const
{
    return dynamic_cast<mtsFunctionWrite *>(FunctionsWrite.GetItem(functionName, CMN_LOG_LOD_INIT_ERROR)->Pointer);
}

mtsFunctionWriteReturn * mtsInterfaceRequired::GetFunctionWriteReturn(const std::string & functionName) const
{
    return dynamic_cast<mtsFunctionWriteReturn *>(FunctionsWriteReturn.GetItem(functionName, CMN_LOG_LOD_INIT_ERROR)->Pointer);
}

mtsFunctionRead * mtsInterfaceRequired::GetFunctionRead(const std::string & functionName) const
{
    return dynamic_cast<mtsFunctionRead *>(FunctionsRead.GetItem(functionName, CMN_LOG_LOD_INIT_ERROR)->Pointer);
}

mtsFunctionQualifiedRead * mtsInterfaceRequired::GetFunctionQualifiedRead(const std::string & functionName) const
{
    return dynamic_cast<mtsFunctionQualifiedRead *>(FunctionsQualifiedRead.GetItem(functionName, CMN_LOG_LOD_INIT_ERROR)->Pointer);
}


std::vector<std::string> mtsInterfaceRequired::GetNamesOfEventHandlersVoid(void) const
{
    return EventHandlersVoid.GetNames();
}


std::vector<std::string> mtsInterfaceRequired::GetNamesOfEventHandlersWrite(void) const
{
    return EventHandlersWrite.GetNames();
}


mtsCommandVoid * mtsInterfaceRequired::GetEventHandlerVoid(const std::string & eventName) const
{
    return EventHandlersVoid.GetItem(eventName);
}


mtsCommandWriteBase * mtsInterfaceRequired::GetEventHandlerWrite(const std::string & eventName) const
{
    return EventHandlersWrite.GetItem(eventName);
}


bool mtsInterfaceRequired::ConnectTo(mtsInterfaceProvidedOrOutput * interfaceProvidedOrOutput)
{
    CMN_LOG_CLASS_INIT_ERROR << "ConnectTo is OBSOLETE" << std::endl;
    return false;
}


bool mtsInterfaceRequired::DetachCommands(void)
{
    // Set pointer to interface provided or input to 0.
    // We do this first so that if the component is still running (e.g., this required interface
    // is MTS_OPTIONAL), it will detect that the required interface is disconnected before trying
    // to invoke any function objects that may be in the process of being detached.
    InterfaceProvided = 0;

    // In the future, it may be better to set the pointers to NOPVoid, NOPRead, etc., which can
    // be static members of the corresponding command classes.
    FunctionInfoMapType::iterator iter;
    for (iter = FunctionsVoid.begin(); iter != FunctionsVoid.end(); iter++) {
        iter->second->Detach();
    }
    for (iter = FunctionsVoidReturn.begin(); iter != FunctionsVoidReturn.end(); iter++) {
        iter->second->Detach();
    }
    for (iter = FunctionsWrite.begin(); iter != FunctionsWrite.end(); iter++) {
        iter->second->Detach();
    }
    for (iter = FunctionsWriteReturn.begin(); iter != FunctionsWriteReturn.end(); iter++) {
        iter->second->Detach();
    }
    for (iter = FunctionsRead.begin(); iter != FunctionsRead.end(); iter++) {
        iter->second->Detach();
    }
    for (iter = FunctionsQualifiedRead.begin(); iter != FunctionsQualifiedRead.end(); iter++) {
        iter->second->Detach();
    }
    return true;
}


bool mtsInterfaceRequired::BindCommands(const mtsInterfaceProvided *interfaceProvided)
{
    bool success = true;
    bool result;
    if (!interfaceProvided) {
        CMN_LOG_CLASS_INIT_ERROR << "BindCommands: required interface \""
                                 << this->GetName() << "\" is not connected to a valid provided interface" << std::endl;
        return false;
    }

    // Bind the command pointers. This may not be thread-safe if the client component is active. We could execute
    // this in the thread of the client component to achieve thread-safety (see, for example, the way that GetEndUserInterface
    // and AddObserverList are executed in the thread of the server component). For now, we assume that the client component (with
    // required interface) will not be active during the connection process, though the server component (with provided interface)
    // might be active. Note also that we set the InterfaceProvided member data at the end of this function, so that the
    // required interface is not considered connected until after the command binding is done (though event receivers/handlers
    // have yet to be set up).

    FunctionInfoMapType::iterator iter;
    mtsFunctionVoid * functionVoid;
    for (iter = FunctionsVoid.begin();
         iter != FunctionsVoid.end();
         iter++) {
        if (!iter->second->Pointer) {
            CMN_LOG_CLASS_INIT_ERROR << "BindCommands: found null function pointer for void command \""
                                     << iter->first << "\" in interface \"" << this->GetName() << "\"" << std::endl;
            result = false;
        } else {
            functionVoid = dynamic_cast<mtsFunctionVoid *>(iter->second->Pointer);
            if (!functionVoid) {
                CMN_LOG_CLASS_INIT_ERROR << "BindCommands: incorrect function pointer for void command \""
                                         << iter->first << "\" in interface \"" << this->GetName() << "\" (got \""
                                         << typeid(iter->second->Pointer).name() << "\")" << std::endl;
                result = false;
            } else {
                result = functionVoid->Bind(interfaceProvided->GetCommandVoid(iter->first));
                if (!result) {
                    CMN_LOG_CLASS_INIT_WARNING << "BindCommands: failed for void command \""
                                               << iter->first << "\" (connecting \""
                                               << this->GetName() << "\" to \""
                                               << interfaceProvided->GetName() << "\")"<< std::endl;
                } else {
                    CMN_LOG_CLASS_INIT_DEBUG << "BindCommands: succeeded for void command \""
                                             << iter->first << "\" (connecting \""
                                             << this->GetName() << "\" to \""
                                             << interfaceProvided->GetName() << "\")"<< std::endl;
                }
            }
        }
        if (iter->second->Required == MTS_OPTIONAL) {
            result = true;
        }
        success &= result;
    }

    mtsFunctionVoidReturn * functionVoidReturn;
    for (iter = FunctionsVoidReturn.begin();
         iter != FunctionsVoidReturn.end();
         iter++) {
        if (!iter->second->Pointer) {
            CMN_LOG_CLASS_INIT_ERROR << "BindCommands: found null function pointer for void with return command \""
                                     << iter->first << "\" in interface \"" << this->GetName() << "\"" << std::endl;
            result = false;
        } else {
            functionVoidReturn = dynamic_cast<mtsFunctionVoidReturn *>(iter->second->Pointer);
            if (!functionVoidReturn) {
                CMN_LOG_CLASS_INIT_ERROR << "BindCommands: incorrect function pointer for void with return command \""
                                         << iter->first << "\" in interface \"" << this->GetName() << "\" (got \""
                                         << typeid(iter->second->Pointer).name() << "\")" << std::endl;
                result = false;
            } else {
                result = functionVoidReturn->Bind(interfaceProvided->GetCommandVoidReturn(iter->first));
                if (!result) {
                    CMN_LOG_CLASS_INIT_WARNING << "BindCommands: failed for void with return command \""
                                               << iter->first << "\" (connecting \""
                                               << this->GetName() << "\" to \""
                                               << interfaceProvided->GetName() << "\")"<< std::endl;
                } else {
                    CMN_LOG_CLASS_INIT_DEBUG << "BindCommands: succeeded for void with return command \""
                                             << iter->first << "\" (connecting \""
                                             << this->GetName() << "\" to \""
                                             << interfaceProvided->GetName() << "\")"<< std::endl;
                }
            }
        }
        if (iter->second->Required == MTS_OPTIONAL) {
            result = true;
        }
        success &= result;
    }

    mtsFunctionWrite * functionWrite;
    for (iter = FunctionsWrite.begin();
         iter != FunctionsWrite.end();
         iter++) {
        if (!iter->second->Pointer) {
            CMN_LOG_CLASS_INIT_ERROR << "BindCommands: found null function pointer for write command \""
                                     << iter->first << "\" in interface \"" << this->GetName() << "\"" << std::endl;
            result = false;
        } else {
            functionWrite = dynamic_cast<mtsFunctionWrite *>(iter->second->Pointer);
            if (!functionWrite) {
                CMN_LOG_CLASS_INIT_ERROR << "BindCommands: incorrect function pointer for write command \""
                                         << iter->first << "\" in interface \"" << this->GetName() << "\" (got \""
                                         << typeid(iter->second->Pointer).name() << "\")" << std::endl;
                result = false;
            } else {
                result = functionWrite->Bind(interfaceProvided->GetCommandWrite(iter->first));
                if (!result) {
                    CMN_LOG_CLASS_INIT_WARNING << "BindCommands: failed for write command \""
                                               << iter->first << "\" (connecting \""
                                               << this->GetName() << "\" to \""
                                               << interfaceProvided->GetName() << "\")"<< std::endl;
                } else {
                    CMN_LOG_CLASS_INIT_DEBUG << "BindCommands: succeeded for write command \""
                                             << iter->first << "\" (connecting \""
                                             << this->GetName() << "\" to \""
                                             << interfaceProvided->GetName() << "\")"<< std::endl;
                }
            }
        }
        if (iter->second->Required == MTS_OPTIONAL) {
            result = true;
        }
        success &= result;
    }

    mtsFunctionWriteReturn * functionWriteReturn;
    for (iter = FunctionsWriteReturn.begin();
         iter != FunctionsWriteReturn.end();
         iter++) {
        if (!iter->second->Pointer) {
            CMN_LOG_CLASS_INIT_ERROR << "BindCommands: found null function pointer for write with result command \""
                                     << iter->first << "\" in interface \"" << this->GetName() << "\"" << std::endl;
            result = false;
        } else {
            functionWriteReturn = dynamic_cast<mtsFunctionWriteReturn *>(iter->second->Pointer);
            if (!functionWriteReturn) {
                CMN_LOG_CLASS_INIT_ERROR << "BindCommands: incorrect function pointer for write with result command \""
                                         << iter->first << "\" in interface \"" << this->GetName() << "\" (got \""
                                         << typeid(iter->second->Pointer).name() << "\")" << std::endl;
                result = false;
            } else {
                result = functionWriteReturn->Bind(interfaceProvided->GetCommandWriteReturn(iter->first));
                if (!result) {
                    CMN_LOG_CLASS_INIT_WARNING << "BindCommands: failed for write with result command \""
                                               << iter->first << "\" (connecting \""
                                               << this->GetName() << "\" to \""
                                               << interfaceProvided->GetName() << "\")"<< std::endl;
                } else {
                    CMN_LOG_CLASS_INIT_DEBUG << "BindCommands: succeeded for write with result command \""
                                             << iter->first << "\" (connecting \""
                                             << this->GetName() << "\" to \""
                                             << interfaceProvided->GetName() << "\")"<< std::endl;
                }
            }
        }
        if (iter->second->Required == MTS_OPTIONAL) {
            result = true;
        }
        success &= result;
    }

    mtsFunctionRead * functionRead;
    for (iter = FunctionsRead.begin();
         iter != FunctionsRead.end();
         iter++) {
        if (!iter->second->Pointer) {
            CMN_LOG_CLASS_INIT_ERROR << "BindCommands: found null function pointer for read command \""
                                     << iter->first << "\" in interface \"" << this->GetName() << "\"" << std::endl;
            result = false;
        } else {
            functionRead = dynamic_cast<mtsFunctionRead *>(iter->second->Pointer);
            if (!functionRead) {
                CMN_LOG_CLASS_INIT_ERROR << "BindCommands: incorrect function pointer for read command \""
                                         << iter->first << "\" in interface \"" << this->GetName() << "\" (got \""
                                         << typeid(iter->second->Pointer).name() << "\")" << std::endl;
                result = false;
            } else {
                result = functionRead->Bind(interfaceProvided->GetCommandRead(iter->first));
                if (!result) {
                    CMN_LOG_CLASS_INIT_WARNING << "BindCommands: failed for read command \""
                                               << iter->first << "\" (connecting \""
                                               << this->GetName() << "\" to \""
                                               << interfaceProvided->GetName() << "\")"<< std::endl;
                } else {
                    CMN_LOG_CLASS_INIT_DEBUG << "BindCommands: succeeded for read command \""
                                             << iter->first  << "\" (connecting \""
                                             << this->GetName() << "\" to \""
                                             << interfaceProvided->GetName() << "\")"<< std::endl;
                }
            }
        }
        if (iter->second->Required == MTS_OPTIONAL) {
            result = true;
        }
        success &= result;
    }

    mtsFunctionQualifiedRead * functionQualifiedRead;
    for (iter = FunctionsQualifiedRead.begin();
         iter != FunctionsQualifiedRead.end();
         iter++) {
        if (!iter->second->Pointer) {
            CMN_LOG_CLASS_INIT_ERROR << "BindCommands: found null function pointer for qualified read command \""
                                     << iter->first << "\" in interface \"" << this->GetName() << "\"" << std::endl;
            result = false;
        } else {
            functionQualifiedRead = dynamic_cast<mtsFunctionQualifiedRead *>(iter->second->Pointer);
            if (!functionQualifiedRead) {
                CMN_LOG_CLASS_INIT_ERROR << "BindCommands: incorrect function pointer for qualified read command \""
                                         << iter->first << "\" in interface \"" << this->GetName() << "\" (got \""
                                         << typeid(iter->second->Pointer).name() << "\")" << std::endl;
                result = false;
            } else {
                result = functionQualifiedRead->Bind(interfaceProvided->GetCommandQualifiedRead(iter->first));
                if (!result) {
                    CMN_LOG_CLASS_INIT_WARNING << "BindCommands: failed for qualified read command \""
                                               << iter->first << "\" (connecting \""
                                               << this->GetName() << "\" to \""
                                               << interfaceProvided->GetName() << "\")"<< std::endl;
                } else {
                    CMN_LOG_CLASS_INIT_DEBUG << "BindCommands: succeeded for qualified read command \""
                                             << iter->first << "\" (connecting \""
                                             << this->GetName() << "\" to \""
                                             << interfaceProvided->GetName() << "\")"<< std::endl;
                }
            }
        }
        if (iter->second->Required == MTS_OPTIONAL) {
            result = true;
        }
        success &= result;
    }

    if (!success) {
        CMN_LOG_CLASS_INIT_ERROR << "BindCommands: required commands missing (connecting \""
                                 << this->GetName() << "\" to \""
                                 << interfaceProvided->GetName() << "\")"<< std::endl;
    }
    // Save pointer to provided interface in class
    InterfaceProvided = interfaceProvided;
    return success;
}

void mtsInterfaceRequired::GetEventList(mtsEventHandlerList &eventList)
{
    // Make sure event list is empty
    eventList.VoidEvents.clear();
    eventList.WriteEvents.clear();

    EventReceiverVoidMapType::iterator iterReceiverVoid;
    for (iterReceiverVoid = EventReceiversVoid.begin();
         iterReceiverVoid != EventReceiversVoid.end();
         iterReceiverVoid++) {
        eventList.VoidEvents.push_back(mtsEventHandlerList::InfoVoid(
                             iterReceiverVoid->first, iterReceiverVoid->second->Pointer->GetCommand(), iterReceiverVoid->second->Required));
    }

    EventReceiverWriteMapType::iterator iterReceiverWrite;
    for (iterReceiverWrite = EventReceiversWrite.begin();
         iterReceiverWrite != EventReceiversWrite.end();
         iterReceiverWrite++) {
        eventList.WriteEvents.push_back(mtsEventHandlerList::InfoWrite(
                              iterReceiverWrite->first, iterReceiverWrite->second->Pointer->GetCommand(), iterReceiverWrite->second->Required));
    }

    EventHandlerVoidMapType::iterator iterEventVoid;
    for (iterEventVoid = EventHandlersVoid.begin();
         iterEventVoid != EventHandlersVoid.end();
         iterEventVoid++) {
        // First, attempt to add it to the event receiver (if one is present).
        // If there is no event receiver, add it directly to the provided interface.
        // Note that event handlers are considered optional.
        if (!AddEventHandlerToReceiver(iterEventVoid->first, iterEventVoid->second))
            eventList.VoidEvents.push_back(mtsEventHandlerList::InfoVoid(
                                 iterEventVoid->first, iterEventVoid->second, MTS_OPTIONAL));
    }

    EventHandlerWriteMapType::iterator iterEventWrite;
    for (iterEventWrite = EventHandlersWrite.begin();
         iterEventWrite != EventHandlersWrite.end();
         iterEventWrite++) {
        // First, attempt to add it to the event receiver (if one is present)
        // If there is no event receiver, add it directly to the provided interface
        // Note that event handlers are considered optional.
        if (!AddEventHandlerToReceiver(iterEventWrite->first, iterEventWrite->second))
            eventList.WriteEvents.push_back(mtsEventHandlerList::InfoWrite(
                                  iterEventWrite->first, iterEventWrite->second, MTS_OPTIONAL));
    }
}

// PK: This could be moved out of mtsInterfaceRequired (perhaps to mtspEventHandlerList?)
bool mtsInterfaceRequired::CheckEventList(mtsEventHandlerList &eventList) const
{
    bool success = true;
    // Now, check the results
    unsigned int i;
    for (i = 0; i < eventList.VoidEvents.size(); i++) {
        if (!eventList.VoidEvents[i].Result) {
            CMN_LOG_CLASS_INIT_WARNING << "CheckeventList: failed to add observer for void event \""
                                       << eventList.VoidEvents[i].EventName << "\" (connecting \""
                                       << this->GetName() << "\" to \""
                                       << eventList.Provided->GetName() << "\")"<< std::endl;
            if (eventList.VoidEvents[i].Required == MTS_REQUIRED)
                success = false;
        } else {
            CMN_LOG_CLASS_INIT_DEBUG << "CheckeventList: succeeded to add observer for void event \""
                                     << eventList.VoidEvents[i].EventName << "\" (connecting \""
                                     << this->GetName() << "\" to \""
                                     << eventList.Provided->GetName() << "\")"<< std::endl;
        }
    }

    for (i = 0; i < eventList.WriteEvents.size(); i++) {
        if (!eventList.WriteEvents[i].Result) {
            CMN_LOG_CLASS_INIT_WARNING << "CheckeventList: failed to add observer for write event \""
                                       << eventList.WriteEvents[i].EventName << "\" (connecting \""
                                       << this->GetName() << "\" to \""
                                       << eventList.Provided->GetName() << "\")"<< std::endl;
            if (eventList.WriteEvents[i].Required == MTS_REQUIRED)
                success = false;
        } else {
            CMN_LOG_CLASS_INIT_DEBUG << "CheckeventList: succeeded to add observer for write event \""
                                     << eventList.WriteEvents[i].EventName << "\" (connecting \""
                                     << this->GetName() << "\" to \""
                                     << eventList.Provided->GetName() << "\")"<< std::endl;
        }
    }
    return success;
}


void mtsInterfaceRequired::DisableAllEvents(void)
{
    EventHandlersVoid.ForEachVoid(&mtsCommandBase::Disable);
    EventHandlersWrite.ForEachVoid(&mtsCommandBase::Disable);
}


void mtsInterfaceRequired::EnableAllEvents(void)
{
    EventHandlersVoid.ForEachVoid(&mtsCommandBase::Enable);
    EventHandlersWrite.ForEachVoid(&mtsCommandBase::Enable);
}


size_t mtsInterfaceRequired::ProcessMailBoxes(void)
{
    // one of constructor of mtsInterfaceRequired allows a null
    // pointer to be passed as the second argument.
    if (!MailBox) {
        CMN_LOG_CLASS_RUN_WARNING << "ProcessMailBoxes: called on interface \""
                                  << this->GetName() << "\" which doesn't have a mail box." << std::endl;
        return 0;
    }

    size_t numberOfCommands = 0;
    while (MailBox->ExecuteNext()) {
        numberOfCommands++;
    }
    return numberOfCommands;
}


void mtsInterfaceRequired::ToStream(std::ostream & outputStream) const
{
    outputStream << "Required Interface name: " << Name << std::endl;
    FunctionsVoid.ToStream(outputStream);
    FunctionsRead.ToStream(outputStream);
    FunctionsWrite.ToStream(outputStream);
    FunctionsQualifiedRead.ToStream(outputStream);
    EventHandlersVoid.ToStream(outputStream);
    EventHandlersWrite.ToStream(outputStream);
}


bool mtsInterfaceRequired::AddFunction(const std::string & functionName, mtsFunctionVoid & function,
                                       mtsRequiredType required)
{
    return FunctionsVoid.AddItem(functionName, new FunctionInfo(function, required));
}


bool mtsInterfaceRequired::AddFunction(const std::string & functionName, mtsFunctionVoidReturn & function,
                                       mtsRequiredType required)
{
    return FunctionsVoidReturn.AddItem(functionName, new FunctionInfo(function, required));
}


bool mtsInterfaceRequired::AddFunction(const std::string & functionName, mtsFunctionWrite & function,
                                       mtsRequiredType required)
{
    return FunctionsWrite.AddItem(functionName, new FunctionInfo(function, required));
}


bool mtsInterfaceRequired::AddFunction(const std::string & functionName, mtsFunctionWriteReturn & function,
                                       mtsRequiredType required)
{
    return FunctionsWriteReturn.AddItem(functionName, new FunctionInfo(function, required));
}


bool mtsInterfaceRequired::AddFunction(const std::string & functionName, mtsFunctionRead & function,
                                       mtsRequiredType required)
{
    return FunctionsRead.AddItem(functionName, new FunctionInfo(function, required));
}


bool mtsInterfaceRequired::AddFunction(const std::string & functionName, mtsFunctionQualifiedRead & function,
                                       mtsRequiredType required)
{
    return FunctionsQualifiedRead.AddItem(functionName, new FunctionInfo(function, required));
}


bool mtsInterfaceRequired::AddEventReceiver(const std::string & eventName, mtsEventReceiverVoid & receiver, mtsRequiredType required)
{
    receiver.SetRequired(eventName, this);
    return EventReceiversVoid.AddItem(eventName, new ReceiverVoidInfo(receiver, required));
}


bool mtsInterfaceRequired::AddEventReceiver(const std::string & eventName, mtsEventReceiverWrite & receiver, mtsRequiredType required)
{
    receiver.SetRequired(eventName, this);
    return EventReceiversWrite.AddItem(eventName, new ReceiverWriteInfo(receiver, required));
}


mtsCommandVoid * mtsInterfaceRequired::AddEventHandlerVoid(mtsCallableVoidBase * callable,
                                                           const std::string & eventName,
                                                           mtsEventQueueingPolicy queueingPolicy)
{
    CMN_ASSERT(callable);
    bool queued = this->UseQueueBasedOnInterfacePolicy(queueingPolicy, "AddEventHandlerVoid", eventName);
    if (queued) {
        if (MailBox) {
            EventHandlersVoid.AddItem(eventName, new mtsCommandQueuedVoid(callable, eventName, MailBox, this->ArgumentQueuesSize));
        } else {
            CMN_LOG_CLASS_INIT_ERROR << "No mailbox for queued event handler void \"" << eventName << "\"" << std::endl;
        }
    } else {
        EventHandlersVoid.AddItem(eventName, new mtsCommandVoid(callable, eventName));
    }
    mtsCommandVoid * handler = EventHandlersVoid.GetItem(eventName);
    AddEventHandlerToReceiver(eventName, handler);  // does nothing if event receiver does not exist
    return handler;
}


bool mtsInterfaceRequired::RemoveEventHandlerVoid(const std::string &eventName)
{
    mtsCommandVoid * handler = 0;
    AddEventHandlerToReceiver(eventName, handler);
    return EventHandlersVoid.RemoveItem(eventName, CMN_LOG_LEVEL_INIT_WARNING);
}


bool mtsInterfaceRequired::RemoveEventHandlerWrite(const std::string &eventName)
{
    mtsCommandWriteBase * handler = 0;
    AddEventHandlerToReceiver(eventName, handler);
    return EventHandlersWrite.RemoveItem(eventName, CMN_LOG_LEVEL_INIT_WARNING);
}
