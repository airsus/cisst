/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: mtsTaskManagerProxyClient.cpp 145 2009-03-18 23:32:40Z mjung5 $

  Author(s):  Min Yang Jung
  Created on: 2009-03-17

  (C) Copyright 2009 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#include <cisstOSAbstraction/osaSleep.h>
#include <cisstMultiTask/mtsTask.h>
#include <cisstMultiTask/mtsTaskManager.h>
#include <cisstMultiTask/mtsTaskManagerProxyClient.h>
#include <cisstMultiTask/mtsDeviceProxy.h>

CMN_IMPLEMENT_SERVICES(mtsTaskManagerProxyClient);

#define TaskManagerProxyClientLogger(_log) BaseType::Logger->trace("mtsTaskManagerProxyClient", _log)
#define TaskManagerProxyClientLoggerError(_log1, _log2) {\
        std::string s("mtsTaskManagerProxyClient: ");\
        s += _log1; s+= _log2;\
        BaseType::Logger->error(s); }

mtsTaskManagerProxyClient::mtsTaskManagerProxyClient(
    const std::string & propertyFileName, const std::string & propertyName) :
        BaseType(propertyFileName, propertyName)
{
}

mtsTaskManagerProxyClient::~mtsTaskManagerProxyClient()
{
}

void mtsTaskManagerProxyClient::Start(mtsTaskManager * callingTaskManager)
{
    // Initialize Ice object.
    // Notice that a worker thread is not created right now.
    Init();
    
    if (InitSuccessFlag) {
        // Client configuration for bidirectional communication
        // (see http://www.zeroc.com/doc/Ice-3.3.1/manual/Connections.38.7.html)
        Ice::ObjectAdapterPtr adapter = IceCommunicator->createObjectAdapter("");
        Ice::Identity ident;
        ident.name = GetGUID();
        ident.category = "";    // not used currently.

        mtsTaskManagerProxy::TaskManagerClientPtr client = 
            new TaskManagerClientI(IceCommunicator, Logger, GlobalTaskManagerProxy, this);
        adapter->add(client, ident);
        adapter->activate();
        GlobalTaskManagerProxy->ice_getConnection()->setAdapter(adapter);

        // Set an implicit context (per proxy context)
        // (see http://www.zeroc.com/doc/Ice-3.3.1/manual/Adv_server.33.12.html)
        IceCommunicator->getImplicitContext()->put(CONNECTION_ID, 
            IceCommunicator->identityToString(ident));

        // Generate an event so that the global task manager register this task manager.
        GlobalTaskManagerProxy->AddClient(ident);

        // Create a worker thread here and returns immediately.
        ThreadArgumentsInfo.argument = callingTaskManager;
        ThreadArgumentsInfo.proxy = this;        
        ThreadArgumentsInfo.Runner = mtsTaskManagerProxyClient::Runner;

        WorkerThread.Create<ProxyWorker<mtsTaskManager>, ThreadArguments<mtsTaskManager>*>(
            &ProxyWorkerInfo, &ProxyWorker<mtsTaskManager>::Run, &ThreadArgumentsInfo, "S-PRX");
    }
}

void mtsTaskManagerProxyClient::StartClient()
{
    Sender->Start();

    // This is a blocking call that should run in a different thread.
    IceCommunicator->waitForShutdown();
}

void mtsTaskManagerProxyClient::Runner(ThreadArguments<mtsTaskManager> * arguments)
{
    mtsTaskManager * TaskManager = reinterpret_cast<mtsTaskManager*>(arguments->argument);

    mtsTaskManagerProxyClient * ProxyClient = 
        dynamic_cast<mtsTaskManagerProxyClient*>(arguments->proxy);

    ProxyClient->GetLogger()->trace("mtsTaskManagerProxyClient", "Proxy client starts.");

    try {
        ProxyClient->StartClient();        
    } catch (const Ice::Exception& e) {
        ProxyClient->GetLogger()->trace("mtsTaskManagerProxyClient exception: ", e.what());
    } catch (const char * msg) {
        ProxyClient->GetLogger()->trace("mtsTaskManagerProxyClient exception: ", msg);
    }

    ProxyClient->OnThreadEnd();
}

void mtsTaskManagerProxyClient::Stop()
{
    OnThreadEnd();
}

void mtsTaskManagerProxyClient::OnThreadEnd()
{
    TaskManagerProxyClientLogger("Proxy client ends.");

    BaseType::OnThreadEnd();

    Sender->Stop();
}

//-----------------------------------------------------------------------------
//  Processing Methods
//-----------------------------------------------------------------------------
mtsDeviceInterface * mtsTaskManagerProxyClient::GetProvidedInterfaceProxy(
    const std::string & resourceTaskName, const std::string & providedInterfaceName,
    const std::string & userTaskName, const std::string & requiredInterfaceName,
    mtsTask * clientTask)
{
    // Ask the global task manager if it has the information about the task named
    // as 'resourceTaskName' that provides the provided interface of which name is
    // 'providedInterfaceName.'
    if (!SendIsRegisteredProvidedInterface(resourceTaskName, providedInterfaceName)) {
        TaskManagerProxyClientLoggerError(
            "[GetProvidedInterfaceProxy] Cannot find the information: ", 
            "task = " + resourceTaskName + ", provided interface = " + providedInterfaceName);
        return NULL;
    }

    // If (server task, provided interface) exists,
    //
    // 1. From the global task manager, retrieve the information about the provided
    //    interface used to create a provided interface proxy at client side.
    mtsTaskManagerProxy::ProvidedInterfaceAccessInfo info;
    if (!SendGetProvidedInterfaceAccessInfo(resourceTaskName, providedInterfaceName, info)) {
        TaskManagerProxyClientLoggerError(
            "[GetProvidedInterfaceProxy] Cannot get provided interface information: ", 
            "task = " + resourceTaskName + ", provided interface = " + providedInterfaceName);
        return NULL;
    }

    // 2. Using the information, start device interface proxy client(s).
    clientTask->RunRequiredInterfaceProxy(requiredInterfaceName, 
                                          info.endpointInfo, 
                                          info.communicatorID);

    //
    // TODO: Does ICE allow a user to register a callback function? (e.g. OnConnect())
    //       If it does, we can remove the following line.
    //
    osaSleep(1 * cmn_s);

    // 3. From the server task (not the global task manager), try to get the complete
    //    information on the provided interface as a set of string. The name of the 
    //    interface is 'providedInterfaceName.'
    //    Note that this is done at the task layer, not at the task manager layer.
    mtsDeviceInterfaceProxy::ProvidedInterfaceInfo providedInterfaceInfo;
    if ( !clientTask->SendGetProvidedInterfaceInfo(
            requiredInterfaceName, providedInterfaceName, providedInterfaceInfo)) {
        TaskManagerProxyClientLoggerError(
            "[GetProvidedInterfaceProxy] Failed to retrieve provided interface information: ",
            providedInterfaceName);
        return NULL;
    }
    
    CMN_ASSERT(providedInterfaceName == providedInterfaceInfo.InterfaceName);
    mtsDeviceInterfaceProxyClient * requiredInterfaceProxy = 
        clientTask->GetRequiredInterfaceProxy(requiredInterfaceName);
    CMN_ASSERT(requiredInterfaceProxy);

    // 4. Create the server task proxy. Note that the proxy is of mtsDevice type, not
    // of mtsTask (see mtsDeviceProxy.h).
    std::string serverTaskProxyName = mtsDeviceProxy::GetServerTaskProxyName(
        resourceTaskName, providedInterfaceName, userTaskName, requiredInterfaceName);
    mtsDeviceProxy * serverTaskProxy = new mtsDeviceProxy(serverTaskProxyName);

    // Add the proxy task to the task manager
    mtsTaskManager * taskManager = mtsTaskManager::GetInstance();
    if (!taskManager->AddDevice(serverTaskProxy)) {
        TaskManagerProxyClientLoggerError(
            "[GetProvidedInterfaceProxy] Failed to add a server task proxy: ",
            serverTaskProxyName);
        return NULL;
    }

    // Create a provided interface proxy using the information received from the 
    // server task.
    mtsDeviceInterface * providedInterfaceProxy = 
        serverTaskProxy->CreateProvidedInterfaceProxy(
        clientTask->GetRequiredInterfaceProxy(requiredInterfaceName), providedInterfaceInfo);
    if (!providedInterfaceProxy) {
        TaskManagerProxyClientLoggerError(
            "[GetProvidedInterfaceProxy] Failed to create provided interface proxy: ",
            providedInterfaceName);
        return NULL;
    }
    
    // A pointer to the provided interface proxy is returned as if the interface 
    // were created at the client's local memory space.
    return providedInterfaceProxy;
}

void mtsTaskManagerProxyClient::UpdateCommandId(
    mtsDeviceInterfaceProxy::FunctionProxySet functionProxies)
{
    /*
    const std::string serverTaskProxyName = functionProxies.ServerTaskProxyName;
    mtsDevice * serverTaskProxy = GetDevice(serverTaskProxyName);
    CMN_ASSERT(serverTaskProxy);

    mtsProvidedInterface * providedInterfaceProxy = 
        serverTaskProxy->GetProvidedInterface(functionProxies.ProvidedInterfaceProxyName);
    CMN_ASSERT(providedInterfaceProxy);

    //mtsCommandVoidProxy * commandVoid = NULL;
    //mtsDeviceInterfaceProxy::FunctionProxySequence::const_iterator it = 
    //    functionProxies.FunctionVoidProxies.begin();
    //for (; it != functionProxies.FunctionVoidProxies.end(); ++it) {
    //    commandVoid = dynamic_cast<mtsCommandVoidProxy*>(
    //        providedInterfaceProxy->GetCommandVoid(it->Name));
    //    CMN_ASSERT(commandVoid);
    //    commandVoid->SetCommandId(it->FunctionProxyPointer);
    //}

    // Replace a command id value with an actual pointer to the function
    // pointer at server side (this resolves thread synchronization issue).
#define REPLACE_COMMAND_ID(_commandType)\
    mtsCommand##_commandType##Proxy * command##_commandType = NULL;\
    mtsDeviceInterfaceProxy::FunctionProxySequence::const_iterator it##_commandType = \
        functionProxies.Function##_commandType##Proxies.begin();\
    for (; it##_commandType != functionProxies.Function##_commandType##Proxies.end(); ++it##_commandType) {\
        command##_commandType = dynamic_cast<mtsCommand##_commandType##Proxy*>(\
            providedInterfaceProxy->GetCommand##_commandType(it##_commandType->Name));\
        if (command##_commandType)\
            command##_commandType->SetCommandId(it##_commandType->FunctionProxyPointer);\
    }

    REPLACE_COMMAND_ID(Void);
    REPLACE_COMMAND_ID(Write);
    REPLACE_COMMAND_ID(Read);
    REPLACE_COMMAND_ID(QualifiedRead);
    */
}

//-------------------------------------------------------------------------
//  Send Methods
//-------------------------------------------------------------------------
bool mtsTaskManagerProxyClient::SendAddProvidedInterface(
    const std::string & newProvidedInterfaceName,
    const std::string & adapterName,
    const std::string & endpointInfo,
    const std::string & communicatorID,
    const std::string & taskName)
{
    mtsTaskManagerProxy::ProvidedInterfaceAccessInfo info;
    info.adapterName = adapterName;
    info.endpointInfo = endpointInfo;
    info.communicatorID = communicatorID;
    info.taskName = taskName;
    info.interfaceName = newProvidedInterfaceName;

    GetLogger()->trace("TMClient", ">>>>> SEND: AddProvidedInterface: " 
        + info.taskName + ", " + info.interfaceName);

    return GlobalTaskManagerProxy->AddProvidedInterface(info);
}

bool mtsTaskManagerProxyClient::SendAddRequiredInterface(
    const std::string & newRequiredInterfaceName, const std::string & taskName)
{
    mtsTaskManagerProxy::RequiredInterfaceAccessInfo info;
    info.taskName = taskName;
    info.interfaceName = newRequiredInterfaceName;

    GetLogger()->trace("TMClient", ">>>>> SEND: AddRequiredInterface: " 
        + info.taskName + ", " + info.interfaceName);

    return GlobalTaskManagerProxy->AddRequiredInterface(info);
}

bool mtsTaskManagerProxyClient::SendIsRegisteredProvidedInterface(
    const std::string & taskName, const std::string & providedInterfaceName) const
{
    GetLogger()->trace("TMClient", ">>>>> SEND: IsRegisteredProvidedInterface: " 
        + taskName + ", " + providedInterfaceName);

    return GlobalTaskManagerProxy->IsRegisteredProvidedInterface(
        taskName, providedInterfaceName);
}

bool mtsTaskManagerProxyClient::SendGetProvidedInterfaceAccessInfo(
    const ::std::string & taskName, const std::string & providedInterfaceName,
    mtsTaskManagerProxy::ProvidedInterfaceAccessInfo & info) const
{
    GetLogger()->trace("TMClient", ">>>>> SEND: GetProvidedInterfaceAccessInfo: " 
        + taskName + ", " + providedInterfaceName);

    return GlobalTaskManagerProxy->GetProvidedInterfaceAccessInfo(
        taskName, providedInterfaceName, info);
}

//void mtsTaskManagerProxyClient::SendNotifyInterfaceConnectionResult(
//    const bool isServerTask, const bool isSuccess,
//    const std::string & userTaskName,     const std::string & requiredInterfaceName,
//    const std::string & resourceTaskName, const std::string & providedInterfaceName)
//{
//    GetLogger()->trace("TMClient", ">>>>> SEND: NotifyInterfaceConnectionResult: " +
//        resourceTaskName + " : " + providedInterfaceName + " - " +
//        userTaskName + " : " + requiredInterfaceName);
//
//    return GlobalTaskManagerProxy->NotifyInterfaceConnectionResult(
//        isServerTask, isSuccess, 
//        userTaskName, requiredInterfaceName, resourceTaskName, providedInterfaceName);
//}

//-------------------------------------------------------------------------
//  Definition by mtsTaskManagerProxy.ice
//-------------------------------------------------------------------------
mtsTaskManagerProxyClient::TaskManagerClientI::TaskManagerClientI(
    const Ice::CommunicatorPtr& communicator,                           
    const Ice::LoggerPtr& logger,
    const mtsTaskManagerProxy::TaskManagerServerPrx& server,
    mtsTaskManagerProxyClient * taskManagerClient)
    : Runnable(true), 
      Communicator(communicator), Logger(logger),
      Server(server), TaskManagerClient(taskManagerClient),      
      Sender(new SendThread<TaskManagerClientIPtr>(this))      
{
}

void mtsTaskManagerProxyClient::TaskManagerClientI::Start()
{
    CMN_LOG_RUN_VERBOSE << "TaskManagerProxyClient: Send thread starts" << std::endl;

    Sender->start();
}

void mtsTaskManagerProxyClient::TaskManagerClientI::Run()
{
    bool flag = true;

    while(Runnable)
    {
#ifdef _COMMUNICATION_TEST_
        static int num = 0;
        std::cout << "client send: " << ++num << std::endl;
        Server->ReceiveDataFromClient(num);
#endif

        if (flag) {
            // Send a set of task names
            mtsTaskManagerProxy::TaskList localTaskList;
            std::vector<std::string> myTaskNames;
            mtsTaskManager::GetInstance()->GetNamesOfTasks(myTaskNames);

            localTaskList.taskNames.insert(
                localTaskList.taskNames.end(),
                myTaskNames.begin(),
                myTaskNames.end());

            localTaskList.taskManagerID = TaskManagerClient->GetGUID();

            Server->UpdateTaskManager(localTaskList);

            flag = false;
        }

        timedWait(IceUtil::Time::milliSeconds(10));
    }
}

void mtsTaskManagerProxyClient::TaskManagerClientI::Stop()
{
    CMN_LOG_RUN_VERBOSE << "TaskManagerProxyClient: Send thread is terminating." << std::endl;

    IceUtil::ThreadPtr callbackSenderThread;

    {
        //IceUtil::Monitor<IceUtil::Mutex>::Lock lock(*this);

        CMN_LOG_RUN_VERBOSE << "TaskManagerProxyClient: Destroying sender." << std::endl;
        Runnable = false;

        notify();

        callbackSenderThread = Sender;
        Sender = 0; // Resolve cyclic dependency.
    }

    callbackSenderThread->getThreadControl().join();
}

// for test purpose
void mtsTaskManagerProxyClient::TaskManagerClientI::ReceiveData(
    ::Ice::Int num, const ::Ice::Current&)
{
    std::cout << "------------ client recv data " << num << std::endl;
}
