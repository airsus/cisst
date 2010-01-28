/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: mtsComponentProxy.cpp 291 2009-04-28 01:49:13Z mjung5 $

  Author(s):  Min Yang Jung
  Created on: 2009-12-18

  (C) Copyright 2009-2010 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#include <cisstMultiTask/mtsComponentProxy.h>

#include <cisstCommon/cmnUnits.h>
#include <cisstOSAbstraction/osaSleep.h>
#include <cisstMultiTask/mtsManagerLocal.h>
#include <cisstMultiTask/mtsRequiredInterface.h>

#include <cisstMultiTask/mtsFunctionReadOrWriteProxy.h>
#include <cisstMultiTask/mtsFunctionQualifiedReadOrWriteProxy.h>
//#include <cisstMultiTask/mtsCommandVoidProxy.h>
//#include <cisstMultiTask/mtsCommandWriteProxy.h>
//#include <cisstMultiTask/mtsCommandReadProxy.h>
//#include <cisstMultiTask/mtsCommandQualifiedReadProxy.h>
//#include <cisstMultiTask/mtsMulticastCommandVoid.h>
//#include <cisstMultiTask/mtsMulticastCommandWriteProxy.h>

CMN_IMPLEMENT_SERVICES(mtsComponentProxy);

mtsComponentProxy::mtsComponentProxy(const std::string & componentProxyName) 
: mtsDevice(componentProxyName), ProvidedInterfaceProxyInstanceID(0)
{
}

mtsComponentProxy::~mtsComponentProxy()
{
    // Stop all the provided interface proxies running.
    ProvidedInterfaceNetworkProxyMapType::MapType::iterator itProvidedProxy = ProvidedInterfaceNetworkProxies.GetMap().begin();
    const ProvidedInterfaceNetworkProxyMapType::MapType::const_iterator itProvidedProxyEnd = ProvidedInterfaceNetworkProxies.end();
    for (; itProvidedProxy != itProvidedProxyEnd; ++itProvidedProxy) {
        itProvidedProxy->second->Stop();
    }

    // Stop all required interface proxies running.
    //RequiredInterfaceNetworkProxyMapType::MapType::iterator it2 =
    //    RequiredInterfaceNetworkProxies.GetMap().begin();
    //for (; it2 != RequiredInterfaceNetworkProxies.GetMap().end(); ++it2) {
    //    it2->second->Stop();
    //}

    osaSleep(500 * cmn_ms);

    // Deallocation
    ProvidedInterfaceNetworkProxies.DeleteAll();
    RequiredInterfaceNetworkProxies.DeleteAll();

    FunctionProxyAndEventHandlerProxyMap.DeleteAll();
}

//-----------------------------------------------------------------------------
//  Methods for Server Components
//-----------------------------------------------------------------------------
bool mtsComponentProxy::CreateRequiredInterfaceProxy(const RequiredInterfaceDescription & requiredInterfaceDescription)
{    
    const std::string requiredInterfaceName = requiredInterfaceDescription.RequiredInterfaceName;

    // Check if the interface name is unique
    mtsRequiredInterface * requiredInterface = GetRequiredInterface(requiredInterfaceName);
    if (requiredInterface) {
        CMN_LOG_CLASS_RUN_ERROR << "CreateRequiredInterfaceProxy: can't create required interface proxy: "
            << "duplicate name: " << requiredInterfaceName << std::endl;
        return false;
    }

    // Create a required interface proxy
    mtsRequiredInterface * requiredInterfaceProxy = new mtsRequiredInterface(requiredInterfaceName);

    // Store function proxy pointers and event handler proxy pointers to assign
    // command proxies' id at server side.
    FunctionProxyAndEventHandlerProxyMapElement * mapElement = new FunctionProxyAndEventHandlerProxyMapElement;

    // Populate the new required interface
    mtsFunctionVoid * functionVoidProxy;
    mtsFunctionWrite * functionWriteProxy;
    mtsFunctionRead * functionReadProxy;
    mtsFunctionQualifiedRead * functionQualifiedReadProxy;

    bool success;

    // Create void function proxies
    const std::vector<std::string> namesOfFunctionVoid = requiredInterfaceDescription.FunctionVoidNames;
    for (unsigned int i = 0; i < namesOfFunctionVoid.size(); ++i) {
        functionVoidProxy = new mtsFunctionVoid();
        success = requiredInterfaceProxy->AddFunction(namesOfFunctionVoid[i], *functionVoidProxy);
        success &= mapElement->FunctionVoidProxyMap.AddItem(namesOfFunctionVoid[i], functionVoidProxy);
        if (!success) {
            delete requiredInterfaceProxy;
            CMN_LOG_CLASS_RUN_ERROR << "CreateRequiredInterfaceProxy: failed to add void function proxy: " << namesOfFunctionVoid[i] << std::endl;
            return false;
        }
    }

    // Create write function proxies
    const std::vector<std::string> namesOfFunctionWrite = requiredInterfaceDescription.FunctionWriteNames;
    for (unsigned int i = 0; i < namesOfFunctionWrite.size(); ++i) {
        functionWriteProxy = new mtsFunctionWriteProxy();
        success = requiredInterfaceProxy->AddFunction(namesOfFunctionWrite[i], *functionWriteProxy);
        success &= mapElement->FunctionWriteProxyMap.AddItem(namesOfFunctionWrite[i], functionWriteProxy);
        if (!success) {
            delete requiredInterfaceProxy;
            CMN_LOG_CLASS_RUN_ERROR << "CreateRequiredInterfaceProxy: failed to add write function proxy: " << namesOfFunctionWrite[i] << std::endl;
            return false;
        }
    }

    // Create read function proxies
    const std::vector<std::string> namesOfFunctionRead = requiredInterfaceDescription.FunctionReadNames;
    for (unsigned int i = 0; i < namesOfFunctionRead.size(); ++i) {
        functionReadProxy = new mtsFunctionReadProxy();
        success = requiredInterfaceProxy->AddFunction(namesOfFunctionRead[i], *functionReadProxy);
        success &= mapElement->FunctionReadProxyMap.AddItem(namesOfFunctionRead[i], functionReadProxy);
        if (!success) {
            delete requiredInterfaceProxy;
            CMN_LOG_CLASS_RUN_ERROR << "CreateRequiredInterfaceProxy: failed to add read function proxy: " << namesOfFunctionRead[i] << std::endl;
            return false;
        }
    }

    // Create QualifiedRead function proxies
    const std::vector<std::string> namesOfFunctionQualifiedRead = requiredInterfaceDescription.FunctionQualifiedReadNames;
    for (unsigned int i = 0; i < namesOfFunctionQualifiedRead.size(); ++i) {
        functionQualifiedReadProxy = new mtsFunctionQualifiedReadProxy();
        success = requiredInterfaceProxy->AddFunction(namesOfFunctionQualifiedRead[i], *functionQualifiedReadProxy);
        success &= success &= mapElement->FunctionQualifiedReadProxyMap.AddItem(namesOfFunctionQualifiedRead[i], functionQualifiedReadProxy);
        if (!success) {
            delete requiredInterfaceProxy;
            CMN_LOG_CLASS_RUN_ERROR << "CreateRequiredInterfaceProxy: failed to add qualified read function proxy: " << namesOfFunctionQualifiedRead[i] << std::endl;
            return false;
        }
    }

    // Create event handler proxies
    std::string eventName;

    // Create event handler proxies with CommandID set to zero and disable them
    // by default. If UpdateEventHandlerID() is called later, CommandID will be
    // set to pointer to event generator pointers and event handler proxies will
    // be enabled.

    // Create void event handler proxy
    mtsCommandVoidProxy * newEventVoidHandlerProxy = NULL;
    for (unsigned int i = 0; i < requiredInterfaceDescription.EventHandlersVoid.size(); ++i) {
        eventName = requiredInterfaceDescription.EventHandlersVoid[i].Name;
        newEventVoidHandlerProxy = new mtsCommandVoidProxy(eventName);
        if (!requiredInterfaceProxy->EventHandlersVoid.AddItem(eventName, newEventVoidHandlerProxy)) {
            // return without adding the required interface proxy to the component
            delete newEventVoidHandlerProxy;
            delete requiredInterfaceProxy;
            CMN_LOG_CLASS_RUN_ERROR << "CreateRequiredInterfaceProxy: failed to add void event handler proxy: " << eventName << std::endl;
            return false;
        }
    }

    // Create write event handler proxies
    std::stringstream streamBuffer;
    cmnDeSerializer deserializer(streamBuffer);

    mtsCommandWriteProxy * newEventWriteHandlerProxy = NULL;
    mtsGenericObject * argumentPrototype = NULL;
    for (unsigned int i = 0; i < requiredInterfaceDescription.EventHandlersWrite.size(); ++i) {
        eventName = requiredInterfaceDescription.EventHandlersWrite[i].Name;
        newEventWriteHandlerProxy = new mtsCommandWriteProxy(eventName);
        if (!requiredInterfaceProxy->EventHandlersWrite.AddItem(eventName, newEventWriteHandlerProxy)) {
            // return without adding the required interface proxy to the component
            delete newEventWriteHandlerProxy;
            delete requiredInterfaceProxy;
            CMN_LOG_CLASS_RUN_ERROR << "CreateRequiredInterfaceProxy: failed to add write event handler proxy: " << eventName << std::endl;
            return false;
        }

        // argument deserialization
        streamBuffer.str("");
        streamBuffer << requiredInterfaceDescription.EventHandlersWrite[i].ArgumentPrototypeSerialized;
        try {
            argumentPrototype = dynamic_cast<mtsGenericObject *>(deserializer.DeSerialize());
        } catch (std::exception e) {
            CMN_LOG_CLASS_RUN_ERROR << "CreateRequiredInterfaceProxy: write command argument deserialization failed: " << e.what() << std::endl;
            argumentPrototype = NULL;
        }

        if (!argumentPrototype) {
            // return without adding the required interface proxy to the component
            delete requiredInterfaceProxy;
            
            CMN_LOG_CLASS_RUN_ERROR << "CreateRequiredInterfaceProxy: failed to create event write handler proxy: " << eventName << std::endl;
            return false;
        }
        newEventWriteHandlerProxy->SetArgumentPrototype(argumentPrototype);
    }

    // Add the required interface proxy to the component
    if (!AddRequiredInterface(requiredInterfaceName, requiredInterfaceProxy)) {
        CMN_LOG_CLASS_RUN_ERROR << "CreateRequiredInterfaceProxy: failed to add required interface proxy: " << requiredInterfaceName << std::endl;
        delete requiredInterfaceProxy;
        return false;
    }

    // Add to function proxy and event handler proxy map
    if (!FunctionProxyAndEventHandlerProxyMap.AddItem(requiredInterfaceName, mapElement)) {
        CMN_LOG_CLASS_RUN_ERROR << "CreateRequiredInterfaceProxy: failed to add proxy map: " << requiredInterfaceName << std::endl;
        return false;
    }

    CMN_LOG_CLASS_RUN_VERBOSE << "CreateRequiredInterfaceProxy: added required interface proxy: " << requiredInterfaceName << std::endl;

    return true;
}

bool mtsComponentProxy::RemoveRequiredInterfaceProxy(const std::string & requiredInterfaceProxyName)
{
    if (!RequiredInterfaces.FindItem(requiredInterfaceProxyName)) {
        CMN_LOG_CLASS_RUN_ERROR << "RemoveRequiredInterfaceProxy: cannot find required interface proxy: " << requiredInterfaceProxyName << std::endl;
        return false;
    }

    // Get a pointer to the provided interface proxy
    mtsRequiredInterface * requiredInterfaceProxy = RequiredInterfaces.GetItem(requiredInterfaceProxyName);
    if (!requiredInterfaceProxy) {
        CMN_LOG_CLASS_RUN_ERROR << "RemoveRequiredInterfaceProxy: This should not happen" << std::endl;
        return false;
    }

    // Remove the provided interface proxy from map
    if (!RequiredInterfaces.RemoveItem(requiredInterfaceProxyName)) {
        CMN_LOG_CLASS_RUN_ERROR << "RemoveRequiredInterfaceProxy: cannot remove required interface proxy: " << requiredInterfaceProxyName << std::endl;
        return false;
    }

    delete requiredInterfaceProxy;

    CMN_LOG_CLASS_RUN_VERBOSE << "RemoveRequiredInterfaceProxy: removed required interface proxy: " << requiredInterfaceProxyName << std::endl;
    return true;
}

//-----------------------------------------------------------------------------
//  Methods for Client Components
//-----------------------------------------------------------------------------
bool mtsComponentProxy::CreateProvidedInterfaceProxy(const ProvidedInterfaceDescription & providedInterfaceDescription)
{
    const std::string providedInterfaceName = providedInterfaceDescription.ProvidedInterfaceName;

    // Create a local provided interface (a provided interface proxy) but it
    // is not immediately added to the component. The interface proxy can be 
    // added to the component proxy only after all proxy objects (command 
    // proxies and event proxies) are confirmed to be successfully created.
    mtsProvidedInterface * providedInterfaceProxy = new mtsDeviceInterface(providedInterfaceName, this);

    // Create command proxies according to the information about the original
    // provided interface. CommandId is initially set to zero and will be 
    // updated later by GetCommandId() which assigns command IDs to command
    // proxies.

    // Note that argument prototypes in the interface description was serialized
    // so they should be deserialized in order to be used to recover orignial 
    // argument prototypes.
    std::string commandName;
    mtsGenericObject * argumentPrototype = NULL,
                     * argument1Prototype = NULL, 
                     * argument2Prototype = NULL;

    std::stringstream streamBuffer;
    cmnDeSerializer deserializer(streamBuffer);

    // Create void command proxies
    mtsCommandVoidProxy * newCommandVoid = NULL;
    CommandVoidVector::const_iterator itVoid = providedInterfaceDescription.CommandsVoid.begin();
    const CommandVoidVector::const_iterator itVoidEnd = providedInterfaceDescription.CommandsVoid.end();
    for (; itVoid != itVoidEnd; ++itVoid) {
        commandName = itVoid->Name;
        newCommandVoid = new mtsCommandVoidProxy(commandName);
        if (!providedInterfaceProxy->GetCommandVoidMap().AddItem(commandName, newCommandVoid)) {
            // return without adding the provided interface proxy to the component
            delete newCommandVoid;
            delete providedInterfaceProxy;
            
            CMN_LOG_CLASS_RUN_ERROR << "CreateProvidedInterfaceProxy: failed to add void command proxy: " << commandName << std::endl;
            return false;
        }
    }

    // Create write command proxies
    mtsCommandWriteProxy * newCommandWrite = NULL;
    CommandWriteVector::const_iterator itWrite = providedInterfaceDescription.CommandsWrite.begin();
    const CommandWriteVector::const_iterator itWriteEnd = providedInterfaceDescription.CommandsWrite.end();
    for (; itWrite != itWriteEnd; ++itWrite) {
        commandName = itWrite->Name;
        newCommandWrite = new mtsCommandWriteProxy(commandName);
        if (!providedInterfaceProxy->GetCommandWriteMap().AddItem(commandName, newCommandWrite)) {
            // return without adding the provided interface proxy to the component
            delete newCommandWrite;
            delete providedInterfaceProxy;            
            
            CMN_LOG_CLASS_RUN_ERROR << "CreateProvidedInterfaceProxy: failed to add write command proxy: " << commandName << std::endl;
            return false;
        }

        // argument deserialization
        streamBuffer.str("");
        streamBuffer << itWrite->ArgumentPrototypeSerialized;
        try {
            argumentPrototype = dynamic_cast<mtsGenericObject *>(deserializer.DeSerialize());
        } catch (std::exception e) {
            CMN_LOG_CLASS_RUN_ERROR << "CreateProvidedInterfaceProxy: write command argument deserialization failed: " << e.what() << std::endl;
            argumentPrototype = NULL;
        }

        if (!argumentPrototype) {
            // return without adding the provided interface proxy to the component
            delete providedInterfaceProxy;
            
            CMN_LOG_CLASS_RUN_ERROR << "CreateProvidedInterfaceProxy: failed to create write command proxy: " << commandName << std::endl;
            return false;
        }
        newCommandWrite->SetArgumentPrototype(argumentPrototype);
    }

    // Create read command proxies
    mtsCommandReadProxy * newCommandRead = NULL;
    CommandReadVector::const_iterator itRead = providedInterfaceDescription.CommandsRead.begin();
    const CommandReadVector::const_iterator itReadEnd = providedInterfaceDescription.CommandsRead.end();
    for (; itRead != itReadEnd; ++itRead) {
        commandName = itRead->Name;
        newCommandRead = new mtsCommandReadProxy(commandName);
        if (!providedInterfaceProxy->GetCommandReadMap().AddItem(commandName, newCommandRead)) {
            // return without adding the provided interface proxy to the component
            delete newCommandRead;
            delete providedInterfaceProxy;
            
            CMN_LOG_CLASS_RUN_ERROR << "CreateProvidedInterfaceProxy: failed to add read command proxy: " << commandName << std::endl;
            return false;
        }

        // argument deserialization
        streamBuffer.str("");
        streamBuffer << itRead->ArgumentPrototypeSerialized;
        try {
            argumentPrototype = dynamic_cast<mtsGenericObject *>(deserializer.DeSerialize());
        } catch (std::exception e) {
            CMN_LOG_CLASS_RUN_ERROR << "CreateProvidedInterfaceProxy: read command argument deserialization failed: " << e.what() << std::endl;
            argumentPrototype = NULL;
        }

        if (!argumentPrototype) {
            // return without adding the provided interface proxy to the component
            delete providedInterfaceProxy;
            
            CMN_LOG_CLASS_RUN_ERROR << "CreateProvidedInterfaceProxy: failed to create read command proxy: " << commandName << std::endl;
            return false;
        }
        newCommandRead->SetArgumentPrototype(argumentPrototype);
    }

    // Create qualified read command proxies
    mtsCommandQualifiedReadProxy * newCommandQualifiedRead = NULL;
    CommandQualifiedReadVector::const_iterator itQualifiedRead = providedInterfaceDescription.CommandsQualifiedRead.begin();
    const CommandQualifiedReadVector::const_iterator itQualifiedReadEnd = providedInterfaceDescription.CommandsQualifiedRead.end();
    for (; itQualifiedRead != itQualifiedReadEnd; ++itQualifiedRead) {
        commandName = itQualifiedRead->Name;
        newCommandQualifiedRead = new mtsCommandQualifiedReadProxy(commandName);
        if (!providedInterfaceProxy->GetCommandQualifiedReadMap().AddItem(commandName, newCommandQualifiedRead)) {
            // return without adding the provided interface proxy to the component
            delete newCommandQualifiedRead;
            delete providedInterfaceProxy;
            
            CMN_LOG_CLASS_RUN_ERROR << "CreateProvidedInterfaceProxy: failed to add qualified read command proxy: " << commandName << std::endl;
            return false;
        }

        // argument1 deserialization
        streamBuffer.str("");
        streamBuffer << itQualifiedRead->Argument1PrototypeSerialized;
        try {
            argument1Prototype = dynamic_cast<mtsGenericObject *>(deserializer.DeSerialize());
        } catch (std::exception e) {
            CMN_LOG_CLASS_RUN_ERROR << "CreateProvidedInterfaceProxy: qualified read command argument 1 deserialization failed: " << e.what() << std::endl;
            argument1Prototype = NULL;
        }
        // argument2 deserialization
        streamBuffer.str("");
        streamBuffer << itQualifiedRead->Argument2PrototypeSerialized;
        try {
            argument2Prototype = dynamic_cast<mtsGenericObject *>(deserializer.DeSerialize());
        } catch (std::exception e) {
            CMN_LOG_CLASS_RUN_ERROR << "CreateProvidedInterfaceProxy: qualified read command argument 2 deserialization failed: " << e.what() << std::endl;
            argument2Prototype = NULL;
        }

        if (!argument1Prototype || !argument2Prototype) {
            // return without adding the provided interface proxy to the component
            delete providedInterfaceProxy;
            
            CMN_LOG_CLASS_RUN_ERROR << "CreateProvidedInterfaceProxy: failed to create qualified read command proxy: " << commandName << std::endl;
            return false;
        }
        newCommandQualifiedRead->SetArgumentPrototype(argument1Prototype, argument2Prototype);
    }

    // Create event generator proxies
    std::string eventName;
    
    FunctionProxyAndEventHandlerProxyMapElement * mapElement
        = FunctionProxyAndEventHandlerProxyMap.GetItem(providedInterfaceName);
    if (!mapElement) {
        mapElement = new FunctionProxyAndEventHandlerProxyMapElement;
    }

    // Create void event generator proxies
    mtsFunctionVoid * eventVoidGeneratorProxy = NULL;
    EventVoidVector::const_iterator itEventVoid = providedInterfaceDescription.EventsVoid.begin();
    const EventVoidVector::const_iterator itEventVoidEnd = providedInterfaceDescription.EventsVoid.end();
    for (; itEventVoid != itEventVoidEnd; ++itEventVoid) {
        eventName = itEventVoid->Name;
        eventVoidGeneratorProxy = new mtsFunctionVoid();
        if (!mapElement->EventGeneratorVoidProxyMap.AddItem(eventName, eventVoidGeneratorProxy)) {
            delete eventVoidGeneratorProxy;
            delete providedInterfaceProxy;
            CMN_LOG_CLASS_RUN_ERROR << "CreateProvidedInterfaceProxy: failed to create event generator proxy: " << eventName << std::endl;
            return false;
        }
        if (!eventVoidGeneratorProxy->Bind(providedInterfaceProxy->AddEventVoid(eventName))) {
            delete providedInterfaceProxy;
            CMN_LOG_CLASS_RUN_ERROR << "CreateProvidedInterfaceProxy: failed to create event generator proxy: " << eventName << std::endl;
            return false;
        }
    }

    // Create write event generator proxies
    //mtsFunctionWriteProxy * eventWriteGeneratorProxy;
    mtsFunctionWrite * eventWriteGeneratorProxy;
    mtsMulticastCommandWriteProxy * eventMulticastCommandProxy;

    EventWriteVector::const_iterator itEventWrite = providedInterfaceDescription.EventsWrite.begin();
    const EventWriteVector::const_iterator itEventWriteEnd = providedInterfaceDescription.EventsWrite.end();
    for (; itEventWrite != itEventWriteEnd; ++itEventWrite) {
        eventName = itEventWrite->Name;
        eventWriteGeneratorProxy = new mtsFunctionWrite();//new mtsFunctionWriteProxy();
        if (!mapElement->EventGeneratorWriteProxyMap.AddItem(eventName, eventWriteGeneratorProxy)) {
            delete eventWriteGeneratorProxy;
            delete providedInterfaceProxy;
            CMN_LOG_CLASS_RUN_ERROR << "CreateProvidedInterfaceProxy: failed to add event write generator proxy pointer: " << eventName << std::endl;
            return false;
        }

        eventMulticastCommandProxy = new mtsMulticastCommandWriteProxy(eventName);
        
        // event argument deserialization
        streamBuffer.str("");
        streamBuffer << itEventWrite->ArgumentPrototypeSerialized;
        try {
            argumentPrototype = dynamic_cast<mtsGenericObject *>(deserializer.DeSerialize());
        } catch (std::exception e) {
            CMN_LOG_CLASS_RUN_ERROR << "CreateProvidedInterfaceProxy: argument deserialization for event write generator failed: " << e.what() << std::endl;
            argumentPrototype = NULL;
        }
        if (!argumentPrototype) {
            delete eventMulticastCommandProxy;
            delete providedInterfaceProxy;
            CMN_LOG_CLASS_RUN_ERROR << "CreateProvidedInterfaceProxy: failed to create write event proxy: " << eventName << std::endl;
            return false;
        }
        eventMulticastCommandProxy->SetArgumentPrototype(argumentPrototype);

        if (!providedInterfaceProxy->AddEvent(eventName, eventMulticastCommandProxy)) {
            delete eventMulticastCommandProxy;
            delete providedInterfaceProxy;
            CMN_LOG_CLASS_RUN_ERROR << "CreateProvidedInterfaceProxy: failed to add event multicast write command proxy: " << eventName << std::endl;
            return false;
        }

        if (!eventWriteGeneratorProxy->Bind(eventMulticastCommandProxy)) {
            delete eventMulticastCommandProxy;
            delete providedInterfaceProxy;
            CMN_LOG_CLASS_RUN_ERROR << "CreateProvidedInterfaceProxy: failed to bind with event multicast write command proxy: " << eventName << std::endl;
            return false;
        }
    }

    // Add the provided interface proxy to the component
    if (!ProvidedInterfaces.AddItem(providedInterfaceName, providedInterfaceProxy)) {
        CMN_LOG_CLASS_RUN_ERROR << "CreateProvidedInterfaceProxy: failed to add provided interface proxy: " << providedInterfaceName << std::endl;
        delete providedInterfaceProxy;
        return false;
    }

    CMN_LOG_CLASS_RUN_VERBOSE << "CreateProvidedInterfaceProxy: added provided interface proxy: " << providedInterfaceName << std::endl;

    return true;
}

bool mtsComponentProxy::RemoveProvidedInterfaceProxy(const std::string & providedInterfaceProxyName)
{
    if (!ProvidedInterfaces.FindItem(providedInterfaceProxyName)) {
        CMN_LOG_CLASS_RUN_ERROR << "RemoveProvidedInterfaceProxy: cannot find provided interface proxy: " << providedInterfaceProxyName << std::endl;
        return false;
    }

    // Get a pointer to the provided interface proxy
    mtsProvidedInterface * providedInterfaceProxy = ProvidedInterfaces.GetItem(providedInterfaceProxyName);
    if (!providedInterfaceProxy) {
        CMN_LOG_CLASS_RUN_ERROR << "RemoveProvidedInterfaceProxy: This should not happen" << std::endl;
        return false;
    }

    // Remove the provided interface proxy from map
    if (!ProvidedInterfaces.RemoveItem(providedInterfaceProxyName)) {
        CMN_LOG_CLASS_RUN_ERROR << "RemoveProvidedInterfaceProxy: cannot remove provided interface proxy: " << providedInterfaceProxyName << std::endl;
        return false;
    }

    delete providedInterfaceProxy;

    CMN_LOG_CLASS_RUN_VERBOSE << "RemoveProvidedInterfaceProxy: removed provided interface proxy: " << providedInterfaceProxyName << std::endl;
    return true;
}

bool mtsComponentProxy::CreateInterfaceProxyServer(const std::string & providedInterfaceProxyName,
                                                   std::string & adapterName, 
                                                   std::string & endpointAccessInfo,
                                                   std::string & communicatorID)
{
    mtsManagerLocal * managerLocal = mtsManagerLocal::GetInstance();

    const std::string adapterNameBase = "ComponentInterfaceServerAdapter";
    const std::string endpointInfoBase = "tcp -p ";
    const std::string portNumber = GetNewPortNumberAsString(ProvidedInterfaceNetworkProxies.size());
    const std::string endpointInfo = endpointInfoBase + portNumber;

    // Generate parameters to initialize server proxy    
    adapterName = adapterNameBase + "_" + providedInterfaceProxyName;
    endpointAccessInfo = ":default -h " + managerLocal->GetIPAddress() + " " + "-p " + portNumber;
    communicatorID = mtsComponentInterfaceProxyServer::InterfaceCommunicatorID;

    // Create an instance of mtsComponentInterfaceProxyServer
    mtsComponentInterfaceProxyServer * providedInterfaceProxy =
        new mtsComponentInterfaceProxyServer(adapterName, endpointInfo, communicatorID);

    // Add it to provided interface proxy object map
    if (!ProvidedInterfaceNetworkProxies.AddItem(providedInterfaceProxyName, providedInterfaceProxy)) {
        CMN_LOG_CLASS_RUN_ERROR << "CreateInterfaceProxyServer: "
            << "Cannot register provided interface proxy: " << providedInterfaceProxyName << std::endl;
        return false;
    }

    // Run provided interface proxy (i.e., component interface proxy server)
    if (!providedInterfaceProxy->Start(this)) {
        CMN_LOG_CLASS_RUN_ERROR << "CreateInterfaceProxyServer: Proxy failed to start: " << providedInterfaceProxyName << std::endl;
        return false;
    }

    providedInterfaceProxy->GetLogger()->trace(
        "mtsComponentProxy", "Provided interface proxy starts: " + providedInterfaceProxyName);

    return true;
}

bool mtsComponentProxy::CreateInterfaceProxyClient(const std::string & requiredInterfaceProxyName,
                                                   const std::string & serverEndpointInfo,
                                                   const std::string & communicatorID,
                                                   const unsigned int providedInterfaceProxyInstanceId)
{
    // Create an instance of mtsComponentInterfaceProxyClient
    mtsComponentInterfaceProxyClient * requiredInterfaceProxy = 
        new mtsComponentInterfaceProxyClient(serverEndpointInfo, communicatorID, providedInterfaceProxyInstanceId);

    // Add it to required interface proxy object map
    if (!RequiredInterfaceNetworkProxies.AddItem(requiredInterfaceProxyName, requiredInterfaceProxy)) {
        CMN_LOG_CLASS_RUN_ERROR << "CreateInterfaceProxyClient: "
            << "Cannot register required interface proxy: " << requiredInterfaceProxyName << std::endl;
        return false;
    }

    // Run required interface proxy (i.e., component interface proxy client)
    if (!requiredInterfaceProxy->Start(this)) {
        CMN_LOG_CLASS_RUN_ERROR << "CreateInterfaceProxyClient: Proxy failed to start: " << requiredInterfaceProxyName << std::endl;
        return false;
    }

    requiredInterfaceProxy->GetLogger()->trace(
        "mtsComponentProxy", "Required interface proxy starts: " + requiredInterfaceProxyName);

    return true;
}

const std::string mtsComponentProxy::GetNewPortNumberAsString(const unsigned int id) const
{
    unsigned int newPortNumber = mtsProxyBaseCommon<mtsComponentProxy>::GetBasePortNumberForComponentInterface() + (id * 5);

    std::stringstream newPortNumberAsString;
    newPortNumberAsString << newPortNumber;

    return newPortNumberAsString.str();
}

bool mtsComponentProxy::IsActiveProxy(const std::string & proxyName, const bool isProxyServer) const
{
    if (isProxyServer) {
        mtsComponentInterfaceProxyServer * providedInterfaceProxy = ProvidedInterfaceNetworkProxies.GetItem(proxyName);
        if (!providedInterfaceProxy) {
            CMN_LOG_CLASS_RUN_ERROR << "IsActiveProxy: Cannot find provided interface proxy: " << proxyName << std::endl;
            return false;
        }
        return providedInterfaceProxy->IsActiveProxy();
    } else {
        mtsComponentInterfaceProxyClient * requiredInterfaceProxy = RequiredInterfaceNetworkProxies.GetItem(proxyName);
        if (!requiredInterfaceProxy) {
            CMN_LOG_CLASS_RUN_ERROR << "IsActiveProxy: Cannot find required interface proxy: " << proxyName << std::endl;
            return false;
        }
        return requiredInterfaceProxy->IsActiveProxy();
    }
}

bool mtsComponentProxy::UpdateEventHandlerProxyID(const std::string & clientComponentName, const std::string & requiredInterfaceName)
{
    // Note that this method is only called by a server process.

    // Get the network proxy client connected to the required interface proxy 
    // of which name is 'requiredInterfaceName.'
    mtsComponentInterfaceProxyClient * interfaceProxyClient = RequiredInterfaceNetworkProxies.GetItem(requiredInterfaceName);
    if (!interfaceProxyClient) {
        CMN_LOG_CLASS_RUN_ERROR << "UpdateEventHandlerProxyID: no interface proxy client found: " << requiredInterfaceName << std::endl;
        return false;
    }

    // Fetch pointers of event generator proxies from the connected provided 
    // interface proxy at the client side.
    mtsComponentInterfaceProxy::EventGeneratorProxyPointerSet eventGeneratorProxyPointers;
    if (!interfaceProxyClient->SendFetchEventGeneratorProxyPointers(clientComponentName, requiredInterfaceName, eventGeneratorProxyPointers))
    {
        CMN_LOG_CLASS_RUN_ERROR << "UpdateEventHandlerProxyID: failed to fetch event generator proxy pointers: " << requiredInterfaceName << std::endl;
        return false;
    }

    mtsComponentInterfaceProxy::EventGeneratorProxySequence::const_iterator it;
    mtsComponentInterfaceProxy::EventGeneratorProxySequence::const_iterator itEnd;

    mtsRequiredInterface * requiredInterface = GetRequiredInterface(requiredInterfaceName);
    if (!requiredInterface) {
        CMN_LOG_CLASS_RUN_ERROR << "UpdateEventHandlerProxyID: no required interface found: " << requiredInterfaceName << std::endl;
        return false;
    }

    mtsCommandVoidProxy * eventHandlerVoid;
    it = eventGeneratorProxyPointers.EventGeneratorVoidProxies.begin();
    itEnd = eventGeneratorProxyPointers.EventGeneratorVoidProxies.end();
    for (; it != itEnd; ++it) {
        // Get event handler proxy of which id is current zero and which is disabled
        eventHandlerVoid = dynamic_cast<mtsCommandVoidProxy*>(requiredInterface->EventHandlersVoid.GetItem(it->Name));
        if (!eventHandlerVoid) {
            CMN_LOG_CLASS_RUN_ERROR << "UpdateEventHandlerProxyID: cannot find event void handler proxy: " << it->Name << std::endl;
            return false;
        }
        // Set client ID and network proxy. Note that SetNetworkProxy() should 
        // be called before SetCommandID().
        if (!eventHandlerVoid->SetNetworkProxy(interfaceProxyClient)) {
            CMN_LOG_CLASS_RUN_ERROR << "UpdateEventHandlerProxyID:: failed to set network proxy: " << it->Name << std::endl;
            return false;
        }
        eventHandlerVoid->SetCommandID(it->EventGeneratorProxyId);
        eventHandlerVoid->Enable();
    }

    mtsCommandWriteProxy * eventHandlerWrite;
    it = eventGeneratorProxyPointers.EventGeneratorWriteProxies.begin();
    itEnd = eventGeneratorProxyPointers.EventGeneratorWriteProxies.end();
    for (; it != itEnd; ++it) {
        // Get event handler proxy which is disabled and of which id is current zero
        eventHandlerWrite = dynamic_cast<mtsCommandWriteProxy*>(requiredInterface->EventHandlersWrite.GetItem(it->Name));
        if (!eventHandlerWrite) {
            CMN_LOG_CLASS_RUN_ERROR << "UpdateEventHandlerProxyID: cannot find event Write handler proxy: " << it->Name << std::endl;
            return false;
        }
        // Set client ID and network proxy. Note that SetNetworkProxy() should 
        // be called before SetCommandID().
        if (!eventHandlerWrite->SetNetworkProxy(interfaceProxyClient)) {
            CMN_LOG_CLASS_RUN_ERROR << "UpdateEventHandlerProxyID:: failed to set network proxy: " << it->Name << std::endl;
            return false;
        }
        eventHandlerWrite->SetCommandID(it->EventGeneratorProxyId);
        eventHandlerWrite->Enable();
    }

    return true;
}

bool mtsComponentProxy::UpdateCommandProxyID(
    const std::string & serverProvidedInterfaceName, const std::string & clientComponentName, 
    const std::string & clientRequiredInterfaceName, const unsigned int providedInterfaceProxyInstanceId)
{
    const unsigned int clientID = providedInterfaceProxyInstanceId;

    // Note that this method is only called by a client process.

    // Get a network proxy server that corresponds to 'serverProvidedInterfaceName'
    mtsComponentInterfaceProxyServer * interfaceProxyServer = 
        ProvidedInterfaceNetworkProxies.GetItem(serverProvidedInterfaceName);
    if (!interfaceProxyServer) {
        CMN_LOG_CLASS_RUN_ERROR << "UpdateCommandProxyID: no interface proxy server found: " << serverProvidedInterfaceName << std::endl;
        return false;
    }

    // Fetch function proxy pointers from a connected required interface proxy
    // at server side, which will be used to set command proxies' IDs.
    mtsComponentInterfaceProxy::FunctionProxyPointerSet functionProxyPointers;
    if (!interfaceProxyServer->SendFetchFunctionProxyPointers(
            providedInterfaceProxyInstanceId, clientRequiredInterfaceName, functionProxyPointers))
    {
        CMN_LOG_CLASS_RUN_ERROR << "UpdateCommandProxyID: failed to fetch function proxy pointers: " 
            << clientRequiredInterfaceName << " @ " << providedInterfaceProxyInstanceId << std::endl;
        return false;
    }
    
    // Get a provided interface proxy instance of which command proxies are updated.
    ProvidedInterfaceProxyInstanceMapType::const_iterator it = 
        ProvidedInterfaceProxyInstanceMap.find(providedInterfaceProxyInstanceId);
    if (it == ProvidedInterfaceProxyInstanceMap.end()) {
        CMN_LOG_CLASS_RUN_ERROR << "UpdateCommandProxyID: failed to fetch provided interface proxy instance: " 
            << providedInterfaceProxyInstanceId << std::endl;
        return false;
    }
    mtsProvidedInterface * instance = it->second;

    // Set command proxy IDs in the provided interface proxy instance as the 
    // function proxies' pointers fetched from server process.

    // Void command
    mtsCommandVoidProxy * commandVoid = NULL;
    mtsComponentInterfaceProxy::FunctionProxySequence::const_iterator itVoid = functionProxyPointers.FunctionVoidProxies.begin();
    const mtsComponentInterfaceProxy::FunctionProxySequence::const_iterator itVoidEnd= functionProxyPointers.FunctionVoidProxies.end();
    for (; itVoid != itVoidEnd; ++itVoid) {
        commandVoid = dynamic_cast<mtsCommandVoidProxy*>(instance->GetCommandVoid(itVoid->Name));
        if (!commandVoid) {
            CMN_LOG_CLASS_RUN_ERROR << "UpdateCommandProxyID:: failed to update command void proxy id: " << itVoid->Name << std::endl;
            return false;
        }
        // Set client ID and network proxy. Note that SetNetworkProxy() should 
        // be called before SetCommandID().
        if (!commandVoid->SetNetworkProxy(interfaceProxyServer, clientID)) {
            CMN_LOG_CLASS_RUN_ERROR << "UpdateCommandProxyID:: failed to set network proxy: " << itVoid->Name << std::endl;
            return false;
        }
        // Set command void proxy's id and enable this command
        commandVoid->SetCommandID(itVoid->FunctionProxyId);
        commandVoid->Enable();
    }

    // Write command
    mtsCommandWriteProxy * commandWrite = NULL;
    mtsComponentInterfaceProxy::FunctionProxySequence::const_iterator itWrite = functionProxyPointers.FunctionWriteProxies.begin();
    const mtsComponentInterfaceProxy::FunctionProxySequence::const_iterator itWriteEnd = functionProxyPointers.FunctionWriteProxies.end();
    for (; itWrite != itWriteEnd; ++itWrite) {
        commandWrite = dynamic_cast<mtsCommandWriteProxy*>(instance->GetCommandWrite(itWrite->Name));
        if (!commandWrite) {
            CMN_LOG_CLASS_RUN_ERROR << "UpdateCommandProxyID:: failed to update command write proxy id: " << itWrite->Name << std::endl;
            return false;
        }
        // Set client ID and network proxy
        if (!commandWrite->SetNetworkProxy(interfaceProxyServer, clientID)) {
            CMN_LOG_CLASS_RUN_ERROR << "UpdateCommandProxyID:: failed to set network proxy: " << itWrite->Name << std::endl;
            return false;
        }
        // Set command write proxy's id and enable this command
        commandWrite->SetCommandID(itWrite->FunctionProxyId);
        commandWrite->Enable();
    }

    // Read command
    mtsCommandReadProxy * commandRead = NULL;
    mtsComponentInterfaceProxy::FunctionProxySequence::const_iterator itRead = functionProxyPointers.FunctionReadProxies.begin();
    const mtsComponentInterfaceProxy::FunctionProxySequence::const_iterator itReadEnd = functionProxyPointers.FunctionReadProxies.end();
    for (; itRead != itReadEnd; ++itRead) {
        commandRead = dynamic_cast<mtsCommandReadProxy*>(instance->GetCommandRead(itRead->Name));
        if (!commandRead) {
            CMN_LOG_CLASS_RUN_ERROR << "UpdateCommandProxyID:: failed to update command read proxy id: " << itRead->Name << std::endl;
            return false;
        }
        // Set client ID and network proxy
        if (!commandRead->SetNetworkProxy(interfaceProxyServer, clientID)) {
            CMN_LOG_CLASS_RUN_ERROR << "UpdateCommandProxyID:: failed to set network proxy: " << itRead->Name << std::endl;
            return false;
        }
        // Set command read proxy's id and enable this command
        commandRead->SetCommandID(itRead->FunctionProxyId);
        commandRead->Enable();
    }

    // QualifiedRead command
    mtsCommandQualifiedReadProxy * commandQualifiedRead = NULL;
    mtsComponentInterfaceProxy::FunctionProxySequence::const_iterator itQualifiedRead = functionProxyPointers.FunctionQualifiedReadProxies.begin();
    const mtsComponentInterfaceProxy::FunctionProxySequence::const_iterator itQualifiedReadEnd = functionProxyPointers.FunctionQualifiedReadProxies.end();
    for (; itQualifiedRead != itQualifiedReadEnd; ++itQualifiedRead) {
        commandQualifiedRead = dynamic_cast<mtsCommandQualifiedReadProxy*>(instance->GetCommandQualifiedRead(itQualifiedRead->Name));
        if (!commandQualifiedRead) {
            CMN_LOG_CLASS_RUN_ERROR << "UpdateCommandProxyID:: failed to update command qualifiedRead proxy id: " << itQualifiedRead->Name << std::endl;
            return false;
        }
        // Set client ID and network proxy
        if (!commandQualifiedRead->SetNetworkProxy(interfaceProxyServer, clientID)) {
            CMN_LOG_CLASS_RUN_ERROR << "UpdateCommandProxyID:: failed to set network proxy: " << itQualifiedRead->Name << std::endl;
            return false;
        }
        // Set command qualified read proxy's id and enable this command
        commandQualifiedRead->SetCommandID(itQualifiedRead->FunctionProxyId);
        commandQualifiedRead->Enable();
    }

    return true;
}

mtsProvidedInterface * mtsComponentProxy::CreateProvidedInterfaceInstance(
    const mtsProvidedInterface * providedInterfaceProxy, unsigned int & instanceID)
{
    // Create a new instance of provided interface proxy
    mtsProvidedInterface * providedInterfaceInstance = new mtsProvidedInterface;

    // Clone command object proxies
    mtsDeviceInterface::CommandVoidMapType::const_iterator itVoidBegin =
        providedInterfaceProxy->CommandsVoid.begin();
    mtsDeviceInterface::CommandVoidMapType::const_iterator itVoidEnd =
        providedInterfaceProxy->CommandsVoid.end();
    providedInterfaceInstance->CommandsVoid.GetMap().insert(itVoidBegin, itVoidEnd);

    mtsDeviceInterface::CommandWriteMapType::const_iterator itWriteBegin =
        providedInterfaceProxy->CommandsWrite.begin();
    mtsDeviceInterface::CommandWriteMapType::const_iterator itWriteEnd =
        providedInterfaceProxy->CommandsWrite.end();
    providedInterfaceInstance->CommandsWrite.GetMap().insert(itWriteBegin, itWriteEnd);

    mtsDeviceInterface::CommandReadMapType::const_iterator itReadBegin =
        providedInterfaceProxy->CommandsRead.begin();
    mtsDeviceInterface::CommandReadMapType::const_iterator itReadEnd =
        providedInterfaceProxy->CommandsRead.end();
    providedInterfaceInstance->CommandsRead.GetMap().insert(itReadBegin, itReadEnd);

    mtsDeviceInterface::CommandQualifiedReadMapType::const_iterator itQualifiedReadBegin =
        providedInterfaceProxy->CommandsQualifiedRead.begin();
    mtsDeviceInterface::CommandQualifiedReadMapType::const_iterator itQualifiedReadEnd =
        providedInterfaceProxy->CommandsQualifiedRead.end();
    providedInterfaceInstance->CommandsQualifiedRead.GetMap().insert(itQualifiedReadBegin, itQualifiedReadEnd);

    mtsDeviceInterface::EventVoidMapType::const_iterator itEventVoidGeneratorBegin = 
        providedInterfaceProxy->EventVoidGenerators.begin();
    mtsDeviceInterface::EventVoidMapType::const_iterator itEventVoidGeneratorEnd = 
        providedInterfaceProxy->EventVoidGenerators.end();
    providedInterfaceInstance->EventVoidGenerators.GetMap().insert(itEventVoidGeneratorBegin, itEventVoidGeneratorEnd);

    mtsDeviceInterface::EventWriteMapType::const_iterator itEventWriteGeneratorBegin = 
        providedInterfaceProxy->EventWriteGenerators.begin();
    mtsDeviceInterface::EventWriteMapType::const_iterator itEventWriteGeneratorEnd = 
        providedInterfaceProxy->EventWriteGenerators.end();
    providedInterfaceInstance->EventWriteGenerators.GetMap().insert(itEventWriteGeneratorBegin, itEventWriteGeneratorEnd);

    // Assign a new provided interface proxy instance id
    instanceID = ++ProvidedInterfaceProxyInstanceID;
    ProvidedInterfaceProxyInstanceMap.insert(std::make_pair(instanceID, providedInterfaceInstance));

    return providedInterfaceInstance;
}

bool mtsComponentProxy::GetFunctionProxyPointers(const std::string & requiredInterfaceName, 
    mtsComponentInterfaceProxy::FunctionProxyPointerSet & functionProxyPointers)
{
    // Get required interface proxy
    mtsRequiredInterface * requiredInterfaceProxy = GetRequiredInterface(requiredInterfaceName);
    if (!requiredInterfaceProxy) {
        CMN_LOG_CLASS_RUN_ERROR << "GetFunctionProxyPointers: failed to get required interface proxy: " << requiredInterfaceName << std::endl;
        return false;
    }

    // Get function proxy and event handler proxy map element
    FunctionProxyAndEventHandlerProxyMapElement * mapElement = FunctionProxyAndEventHandlerProxyMap.GetItem(requiredInterfaceName);
    if (!mapElement) {
        CMN_LOG_CLASS_RUN_ERROR << "GetFunctionProxyPointers: failed to get proxy map element: " << requiredInterfaceName << std::endl;
        return false;
    }

    mtsComponentInterfaceProxy::FunctionProxyInfo function;

    // Void function proxy
    FunctionVoidProxyMapType::const_iterator itVoid = mapElement->FunctionVoidProxyMap.begin();
    const FunctionVoidProxyMapType::const_iterator itVoidEnd = mapElement->FunctionVoidProxyMap.end();
    for (; itVoid != itVoidEnd; ++itVoid) {
        function.Name = itVoid->first;
        function.FunctionProxyId = reinterpret_cast<CommandIDType>(itVoid->second);
        functionProxyPointers.FunctionVoidProxies.push_back(function);
    }

    // Write function proxy
    FunctionWriteProxyMapType::const_iterator itWrite = mapElement->FunctionWriteProxyMap.begin();
    const FunctionWriteProxyMapType::const_iterator itWriteEnd = mapElement->FunctionWriteProxyMap.end();
    for (; itWrite != itWriteEnd; ++itWrite) {
        function.Name = itWrite->first;
        function.FunctionProxyId = reinterpret_cast<CommandIDType>(itWrite->second);
        functionProxyPointers.FunctionWriteProxies.push_back(function);
    }

    // Read function proxy
    FunctionReadProxyMapType::const_iterator itRead = mapElement->FunctionReadProxyMap.begin();
    const FunctionReadProxyMapType::const_iterator itReadEnd = mapElement->FunctionReadProxyMap.end();
    for (; itRead != itReadEnd; ++itRead) {
        function.Name = itRead->first;
        function.FunctionProxyId = reinterpret_cast<CommandIDType>(itRead->second);
        functionProxyPointers.FunctionReadProxies.push_back(function);
    }

    // QualifiedRead function proxy
    FunctionQualifiedReadProxyMapType::const_iterator itQualifiedRead = mapElement->FunctionQualifiedReadProxyMap.begin();
    const FunctionQualifiedReadProxyMapType::const_iterator itQualifiedReadEnd = mapElement->FunctionQualifiedReadProxyMap.end();
    for (; itQualifiedRead != itQualifiedReadEnd; ++itQualifiedRead) {
        function.Name = itQualifiedRead->first;
        function.FunctionProxyId = reinterpret_cast<CommandIDType>(itQualifiedRead->second);
        functionProxyPointers.FunctionQualifiedReadProxies.push_back(function);
    }

    return true;
}

bool mtsComponentProxy::GetEventGeneratorProxyPointer(
    const std::string & clientComponentName, const std::string & requiredInterfaceName,
    mtsComponentInterfaceProxy::EventGeneratorProxyPointerSet & eventGeneratorProxyPointers)
{
    mtsManagerLocal * localManager = mtsManagerLocal::GetInstance();
    mtsDevice * clientComponent = localManager->GetComponent(clientComponentName);
    if (!clientComponent) {
        CMN_LOG_CLASS_RUN_ERROR << "GetEventGeneratorProxyPointer: no client component found: " << clientComponentName << std::endl;
        return false;
    }

    mtsRequiredInterface * requiredInterface = clientComponent->GetRequiredInterface(requiredInterfaceName);
    if (!requiredInterface) {
        CMN_LOG_CLASS_RUN_ERROR << "GetEventGeneratorProxyPointer: no required interface found: " << requiredInterfaceName << std::endl;
        return false;
    }
    mtsProvidedInterface * providedInterface = requiredInterface->GetConnectedInterface();
    if (!providedInterface) {
        CMN_LOG_CLASS_RUN_ERROR << "GetEventGeneratorProxyPointer: no connected provided interface found" << std::endl;
        return false;
    }
    
    mtsComponentInterfaceProxy::EventGeneratorProxyElement element;
    mtsCommandBase * eventGenerator = NULL;

    std::vector<std::string> namesOfEventHandlersVoid = requiredInterface->GetNamesOfEventHandlersVoid();
    for (unsigned int i = 0; i < namesOfEventHandlersVoid.size(); ++i) {
        element.Name = namesOfEventHandlersVoid[i];
        eventGenerator = providedInterface->EventVoidGenerators.GetItem(element.Name);
        if (!eventGenerator) {
            CMN_LOG_CLASS_RUN_ERROR << "GetEventGeneratorProxyPointer: no event void generator found: " << element.Name << std::endl;
            return false;
        }
        element.EventGeneratorProxyId = reinterpret_cast<CommandIDType>(eventGenerator);
        eventGeneratorProxyPointers.EventGeneratorVoidProxies.push_back(element);
    }

    std::vector<std::string> namesOfEventHandlersWrite = requiredInterface->GetNamesOfEventHandlersWrite();
    for (unsigned int i = 0; i < namesOfEventHandlersWrite.size(); ++i) {
        element.Name = namesOfEventHandlersWrite[i];
        eventGenerator = providedInterface->EventWriteGenerators.GetItem(element.Name);
        if (!eventGenerator) {
            CMN_LOG_CLASS_RUN_ERROR << "GetEventGeneratorProxyPointer: no event write generator found: " << element.Name << std::endl;
            return false;
        }
        element.EventGeneratorProxyId = reinterpret_cast<CommandIDType>(eventGenerator);
        eventGeneratorProxyPointers.EventGeneratorWriteProxies.push_back(element);
    }

    return true;
}