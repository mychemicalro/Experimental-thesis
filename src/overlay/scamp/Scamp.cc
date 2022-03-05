/*
 * Scamp.cc
 *
 *  Created on: 30/nov/2021
 *      Author: DinuFC
 */

#include <iostream>
#include <UnderlayConfigurator.h>
#include <GlobalStatistics.h>
#include <View.h>
#include <BootstrapList.h>
#include "Scamp.h"


// To convert between IP addresses (which have bit 24 active), and keys (which don't), we'll need to set or remove this bit.
#define BIGBIT (1 << 24)
#define DEBUG false

// Important! This line must be present for each module you extend (see BaseApp)
Define_Module(Scamp);

Scamp::Scamp() {
    inViewModule = NULL;
    partialViewModule = NULL;
    seenNodesModule = NULL;
}

Scamp::~Scamp() {
    if(DEBUG)
        EV << "[Scamp::DESTRUCTOR() @ " << thisNode.getIp()
           << " (" << thisNode.getKey().toString(16) << ")]\n"
           << "    Destroying" << endl;

    cancelAndDelete(join_timer);
    cancelAndDelete(rejoinTimer);

    if(useHeartbeats){
        cancelAndDelete(heartbeat_timer);
        cancelAndDelete(pruneInView_timer);
    }
}

// Called when the module is being initialized
void Scamp::initializeOverlay(int stage) {
    // see BaseApp.cc
    if (stage != MIN_STAGE_OVERLAY) return;

    // get our key from our IP address
    // this was for myoverlay example
    myKey = thisNode.getIp().get4().getInt() & ~BIGBIT;

    // get input parameters with par
    joinDelay = par("joinDelay");
    joinRetry = par("joinRetry");
    heartbeatDelay = par("heartbeatDelay");
    heartbeatTimeout = par("heartbeatTimeout");
    pruneDelay = par("pruneDelay");

    // how many copies of a join request received a node will forward
    joinRequestCopies = par("joinRequestCopies");

    useCwhenLeaving = par("useCwhenLeaving");

    // initialize other parameters
    // if true, timers will be used.
    useHeartbeats = par("useHeartbeats");

    // ok
    findFriendModules();

    // self messages
    join_timer = new cMessage("join_timer");
    rejoinTimer = new cMessage("rejoinTimer");
    leasing = false;

    if(useHeartbeats){
        heartbeat_timer = new cMessage("heartbeat_timer");
        pruneInView_timer = new cMessage("pruneInView_timer");
    }

    // statistics
    totalHeartbeatsSent = 0;
    heartbeatsSentLastPeriod = 0;

    // watch variables
    WATCH(bootstrapNode);
    WATCH(joinRetry);
    WATCH(joinDelay);
    WATCH(joinRequestCopies);
    WATCH(totalHeartbeatsSent);
    WATCH(heartbeatsSentLastPeriod);
    WATCH(useCwhenLeaving);
    WATCH(leasing);
}

// Called to set our own overlay key (optional)
void Scamp::setOwnNodeID() {
    // create the corresponding overlay key
    if (DEBUG)
        EV << "setOwnNodeID()" << endl;
    thisNode.setKey(OverlayKey(myKey));
    initializeFriendModules();
    seenNodesModule->addMember(thisNode);
}

void Scamp::joinOverlay() {
    changeState(INIT);
    changeState(JOIN);
}

void Scamp::changeState(int toState) {

    // Defines tasks to be executed when a state change occurs.
    switch(toState){
        case INIT:
            state = INIT;

            // see BaseOverlay.h
            setOverlayReady(false);

            if(DEBUG)
                EV << "[Scamp::changeState() @ " << thisNode.getIp()
                    << " (" << thisNode.getKey().toString(16) << ")]\n"
                    << "    Entered INIT stage" << endl;


            getParentModule()->getParentModule()->bubble("Enter INIT state.");
            break;

        case JOIN:
            state = JOIN;

            if(DEBUG)
                EV << "[Scamp::changeState() @ " << thisNode.getIp()
                    << " (" << thisNode.getKey().toString(16) << ")]\n"
                    << "    Entered JOIN stage"
                    << endl;

            // initiate join process



            cancelEvent(join_timer);
            // workaround: prevent notificationBoard from taking ownership of join_timer message
            // take(join_timer);
            scheduleAt(simTime() + joinDelay, join_timer);


            getParentModule()->getParentModule()->bubble("Enter JOIN state.");

            if(!leasing || partialViewModule->isEmpty()){
                // find a new bootstrap node and enroll to the bootstrap list
                bootstrapNode = bootstrapList->getBootstrapNode(overlayId);

                // is this the first node?
                if (bootstrapNode.isUnspecified()) {
                    bootstrapNode = thisNode;
                    changeState(READY);
                }

            } else {
                // get a random node from the partialView
                int pos = (int) uniform(0, partialViewModule->getSize());
                bootstrapNode = partialViewModule->getMember(pos);
            }

            break;

        case READY:
            state = READY;
            // send CompReadyMessage to all registered components and modifies GUI

            if(DEBUG)
                EV << "[Scamp::changeState() @ " << thisNode.getIp()
                    << " (" << thisNode.getKey().toString(16) << ")]\n"
                    << "    Entered READY stage"
                    << endl;

            setOverlayReady(true);
            getParentModule()->getParentModule()->bubble("Enter READY state.");

            // Now I can send imAlive messages and pruneInView

            if(useHeartbeats){

                if(heartbeat_timer->isScheduled()){
                    cancelEvent(heartbeat_timer);
                }
                if(pruneInView_timer->isScheduled()){
                    cancelEvent(pruneInView_timer);
                }

                scheduleAt(simTime() + heartbeatDelay, heartbeat_timer);
                scheduleAt(simTime() + pruneDelay, pruneInView_timer);
            }

            if (leasing)
                leasing = false;

            break;

        case DISCONNECTED:{
            state = DISCONNECTED;
            if(DEBUG)
                EV << "[Scamp::changeState() @ " << thisNode.getIp()
                    << " (" << thisNode.getKey().toString(16) << ")]\n"
                    << "    Entered DISCONNECTED stage"
                    << endl;

            getParentModule()->getParentModule()->bubble("Enter DISCONNECTED state.");

            if (!leasing){
                // delete other arrows
                std::vector<NodeHandle> nodes = partialViewModule->getNodeHandles();
                for(int i=0; i<(int)nodes.size(); i++){
                    deleteOverlayNeighborArrow(nodes.at(i));
                }

                partialViewModule->clearView();
            }

            inViewModule->clearView(); // Not useful
            joinRetry = par("joinRetry");

            // take(join_timer);
            scheduleAt(simTime() + joinDelay, join_timer);
            setOverlayReady(false);

            break;
        }

        case SHUTDOWN:
            state = SHUTDOWN;

            if(DEBUG)
                EV << "[Scamp::changeState() @ " << thisNode.getIp()
                    << " (" << thisNode.getKey().toString(16) << ")]\n"
                    << "    Entered SHUTDOWN stage"
                    << endl;

            getParentModule()->getParentModule()->bubble("Enter SHUTDOWN state.");

            setOverlayReady(false);

            if(useHeartbeats){

                if(heartbeat_timer->isScheduled()){
                    cancelEvent(heartbeat_timer);
                }
                if(pruneInView_timer->isScheduled()){
                    cancelEvent(pruneInView_timer);
                }
            }

            if (join_timer->isScheduled()){
               cancelEvent(join_timer);
            }
            if (rejoinTimer->isScheduled()){
               cancelEvent(rejoinTimer);
            }

            break;
    }
}

void Scamp::handleTimerEvent(cMessage* msg) {

    // catch JOIN timer
    if (msg == join_timer) {
        handleJoinTimerExpired(msg);

    } else if (msg == heartbeat_timer){
        handleHeartbeatTimerExpired(msg);
        // schedule next imAlive process
        cancelEvent(heartbeat_timer);
        scheduleAt(simTime() + heartbeatDelay, heartbeat_timer);

    } else if (msg == pruneInView_timer){
        handlePruneInViewTimerExpired(msg);
        // schedule next pruning process
        cancelEvent(pruneInView_timer);
        scheduleAt(simTime() + pruneDelay, pruneInView_timer);

    } else if(msg == rejoinTimer) {
        changeState(DISCONNECTED);

    } else if (msg->hasPar("RemoveNode")){
        cArray pars = msg->getParList();

        cObject *obj = pars[1]; // Get the overlay key of the nodes
        std::vector<NodeHandle> nodes = partialViewModule->getNodeHandles();

        int i;
        for(i=0; i<(int)nodes.size(); i++){
            if (nodes.at(i).getKey() == OverlayKey(obj->getName())){
                partialViewModule->removeMember(nodes.at(i));
                deleteOverlayNeighborArrow(nodes.at(i));
            }
        }
    }
    // unknown self message
    else {
        error("Scamp::handleTimerEvent(): received self message of "
              "unknown type!");
    }
}

void Scamp::handleJoinTimerExpired(cMessage* msg) {
    // only process timer, if node is not joined yet
    if (state == READY)
        return;

    if (state != JOIN)
        changeState(JOIN);

    if (joinRetry == 0) {
        if(leasing){ // I have already tried different times to join, but I cannot through my partialView
            partialViewModule->clearView();
        }
        changeState(DISCONNECTED);
        return;
    }

    joinRetry--;

    // call JOIN CALL RPC
    ScampJoinCall* call = new ScampJoinCall("JoinCall");
    call->setTime(simTime());
    call->setAddress(thisNode.getIp());
    call->setBitLength(JOINCALL_L(call));

    RoutingType routingType = (defaultRoutingType == FULL_RECURSIVE_ROUTING ||
                               defaultRoutingType == RECURSIVE_SOURCE_ROUTING) ?
                              SEMI_RECURSIVE_ROUTING : defaultRoutingType;

    // Each time get a new bootstrapNode
    bootstrapNode = bootstrapList->getBootstrapNode(overlayId);

    sendRouteRpcCall(OVERLAY_COMP, bootstrapNode, thisNode.getKey(), call, NULL, routingType);

    // schedule next join process in the case this one fails
    cancelEvent(join_timer);
    scheduleAt(simTime() + joinDelay, msg);

    joinDelay += uniform(0, 3);
}

void Scamp::handleHeartbeatTimerExpired(cMessage* msg){
    if(DEBUG)
        EV << "[Scamp::handleHeartbeatTimerExpired() @ " << thisNode.getIp()
            << " (" << thisNode.getKey().toString(16) << ")]\n"
            << endl;
    if (state != READY){
        if(DEBUG)
            EV << "Not ready yet to send heartbeat messages"
               << endl;
        return;
    }

    // send heartbeat to every neighbor
    for(int i=0; i<(int)partialViewModule->getSize(); i++){
        NodeHandle dest = partialViewModule->getMember(i);
        ScampMessage* heartbeat = new ScampMessage("Heartbeat");

        if (isLeaf() && 1==0){ // TODO
            heartbeat->setCommand(HEARTBEAT_FROM_LEAF);
            if(DEBUG)
                EV << "Sent HEARTBEAT_FROM_LEAF" << endl;

        } else {
            heartbeat->setCommand(HEARTBEAT);
        }

        heartbeat->setNode(thisNode);
        // notify it about this
        sendMessageToUDP(dest, heartbeat);

        totalHeartbeatsSent++;
        heartbeatsSentLastPeriod++;
    }
}

void Scamp::handlePruneInViewTimerExpired(cMessage* msg){
    if(DEBUG)
        EV << "[Scamp::handlePruneInViewTimerExpired() @ " << thisNode.getIp()
            << " (" << thisNode.getKey().toString(16) << ")]\n"
            << endl;

    if (state != READY){
        if(DEBUG)
            EV << "Not ready yet to prune InView nodes"
               << endl;
        return;
    }

    // prune the old nodes and get the deleted ones, in order to remove them also from the partialView
    std::vector<NodeHandle> deletedNodes = inViewModule->prune(simTime().dbl(), heartbeatTimeout);

    // I won't delete the nodes from the partialView for now --> need to delete them instead!!
    // 1. Because otherwise the partialView gets full
    // 2. I do not have to send them heartbeats anymore

    for(int i=0; i<(int)deletedNodes.size(); i++){
        if(partialViewModule->isPresent(deletedNodes.at(i))){
            partialViewModule->removeMember(deletedNodes.at(i));
            if(DEBUG)
                EV << "Node deleted also from the partialView"
                   << endl;
        }
    }

    // check if inView is empty or partialViewModule is empty. If yes, change state to DISCONNECTED. For now, I won't consider the partialView being empty

    if(inViewModule->isEmpty() && !leasing){

        if(DEBUG)
            EV << "Empty inView -> DISCONNECTED"
               << endl;

        changeState(DISCONNECTED);

    }

    /* else if (partialViewModule->isEmpty()){
        // Connect to a random node from my inView
        if(DEBUG)
            EV << "Empty partialView -> Join some node from the inView"
               << endl;

        std::vector<NodeHandle> nodes = inViewModule->getRandomNodes(joinRequestCopies);

        ScampMessage* message = new ScampMessage("ScampMessage");
        message->setCommand(ADDEDTOPARTIALVIEW);
        message->setNode(thisNode);

        // notify it about this
        sendMessageToUDP(nodes[0], message);
        partialViewModule->addMember(nodes[0]);
        showOverlayNeighborArrow(nodes[0], false, "m=m,50,0,50,0;ls=red,1");

    }*/ // TODO
}

void Scamp::rpcJoin(ScampJoinCall* joinCall) {

    NodeHandle requester = joinCall->getSrcNode();

    if(DEBUG)
        EV << "[Scamp::rpcJoin() @ " << thisNode.getIp() << "\n"
           << "    Received JoinCall request from node: " << requester.getIp() << "\n"
           << endl;

    if(state != READY){
        if(DEBUG)
            EV << "Cannot take JOIN requests --> DISCARDED"
               << endl;
        return;
    }

    // broadcast this nodeHandle
    std::vector<NodeHandle> nodes = partialViewModule->getNodeHandles();
    int i;
    for(i=0; i<(int)nodes.size(); i++){
        NewNodeMessage* message = new NewNodeMessage("NewNodeMessage");
        message->setCommand(NEW_NODE);
        message->setNode(requester);
        // notify it about this
        sendMessageToUDP(nodes.at(i), message);
    }

    seenNodesModule->addMember(requester);

    // Log the join message
    outfile.open("results\\JoinMessages.log", std::ios_base::app); // append instead of overwrite
    outfile << joinCall->getAddress() << ";JOIN_CALL;" << simTime() << ";" << thisNode.getIp() << "\n";
    outfile.close();

    ScampJoinResponse* joinResponse = new ScampJoinResponse("JoinResponse");

    if (partialViewModule->addMember(requester) || partialViewModule->isPresent(requester)){
        // send accepted to the requester
        joinResponse->setCommand(JOINREQUESTACCEPTED);
        joinResponse->setBitLength(JOINRESPONSE_L(joinResponse));
        sendRpcResponse(joinCall, joinResponse);

        showOverlayNeighborArrow(requester, false, "m=m,50,0,50,0;ls=red,1");

    } else {
        joinResponse->setCommand(JOINREQUESTFORWARDED);
        joinResponse->setBitLength(JOINRESPONSE_L(joinResponse));
        sendRpcResponse(joinCall, joinResponse);
    }

    /**
     * since this is got by a contact node, forward the request.
     * Need to send joinRequestCopies to some random nodes that are not the one that is requesting to join
     *
     */
    forwardJoinRequest(requester, requester, 1, joinRequestCopies, true);
}


void Scamp::forwardJoinRequest(NodeHandle sender, NodeHandle requester, int steps, int howMany, bool useRequester){
    if(DEBUG)
        EV << "[Scamp::forwardJoinRequest() @ " << thisNode.getIp()
           << " (" << thisNode.getKey().toString(16) << ")]\n"
           << "    Trying to forward call for requester: "
           << requester.getIp()
           << endl;

    if (useRequester){
        sender = requester;
    }

    if (requester.getIp() == thisNode.getIp()){
        // My old request got to me
        return;
    }

    std::vector<NodeHandle> nodes = partialViewModule->getRandomNodes(howMany);

    if (!nodes.empty()){
        if(DEBUG)
            EV << "Can forward to: " << nodes.size() << " nodes" << endl;

        for(std::vector<NodeHandle>::iterator it = nodes.begin(); it != nodes.end(); ++it) {
            ForwardMessage* message = new ForwardMessage("ForwardMessage");
            message->setSteps(steps);
            message->setCommand(JOINREQUESTFORWARDED);
            message->setNode(requester);
            message->setSourceNode(thisNode);
            // notify it about this
            sendMessageToUDP(*it, message);
        }
    }
}

bool Scamp::handleRpcCall(BaseCallMessage* msg){

    if (state != READY && state != JOIN) {
        EV << "[Scamp::handleRpcCall() @ " << thisNode.getIp()
           << " (" << thisNode.getKey().toString(16) << ")]\n"
           << "    Received RPC call and state != READY"
           << endl;
        return false;
    }

    EV << "[Scamp::handleRpcCall() @ " << thisNode.getIp()
       << " (" << thisNode.getKey().toString(16) << ")]\n"
       << "    Received RPC call and state == READY"
       << endl;

    // delegate messages
    RPC_SWITCH_START( msg )
    RPC_DELEGATE( ScampJoin, rpcJoin );
    RPC_SWITCH_END( )

    return RPC_HANDLED;
}


void Scamp::handleRpcResponse(BaseResponseMessage* msg, cPolymorphic* context, int rpcId, simtime_t rtt) {
    RPC_SWITCH_START(msg)
    RPC_ON_RESPONSE( ScampJoin ) {
        handleRpcJoinResponse(_ScampJoinResponse);
        EV << "[Scamp::handleRpcResponse() @ " << thisNode.getIp()
        << " (" << thisNode.getKey().toString(16) << ")]\n"
        << "    Received a Join RPC Response: id=" << rpcId << "\n"
        << "    msg=" << *_ScampJoinResponse << " rtt=" << rtt
        << endl;
        break;
    }
    RPC_SWITCH_END( )
}

void Scamp::handleRpcJoinResponse(ScampJoinResponse* joinResponse) {
    if(DEBUG)
        EV << "[Scamp::handleRpcJoinResponse() @ " << thisNode.getIp() << "\n"
           << "    Command: " << joinResponse->getCommand()
           << endl;
    NodeHandle responder = joinResponse->getSrcNode();

    outfile.open("results\\JoinMessages.log", std::ios_base::app); // append instead of overwrite
    outfile << thisNode.getIp() << ";JOIN_RESPONSE;" << simTime() << ";" << responder.getIp() <<"\n";
    outfile.close();

    // Handle the response from my bootstrapNode
    if(bootstrapNode.getIp() != thisNode.getIp() && bootstrapNode.getIp() == joinResponse->getSrcNode().getIp()){

        // if already in the partialView
        if(partialViewModule->isPresent(joinResponse->getSrcNode())){
            if(DEBUG)
                EV << "Bootstrap node already in this partialView"
                   << endl;
            if (joinResponse->getCommand() == JOINREQUESTACCEPTED){
                inViewModule->addMember(joinResponse->getSrcNode());
                if(state != READY)
                    changeState(READY);
            }
        }

        else  {
            if(partialViewModule->addMember(joinResponse->getSrcNode())){

                showOverlayNeighborArrow(joinResponse->getSrcNode(), false, "m=m,50,0,50,0;ls=red,1");

                outfile.open("results\\JoinMessages.log", std::ios_base::app); // append instead of overwrite
                outfile << thisNode.getIp() << ";ADDEDTOPARTIALVIEW;" << simTime() << ";" << joinResponse->getSrcNode().getIp() << "\n";
                outfile.close();

                ScampMessage* message = new ScampMessage("ScampMessage");
                message->setCommand(ADDEDTOPARTIALVIEW);
                message->setNode(thisNode);
                // notify it about this
                sendMessageToUDP(joinResponse->getSrcNode(), message);

            }
            if (joinResponse->getCommand() == JOINREQUESTACCEPTED){
                inViewModule->addMember(joinResponse->getSrcNode());
                if(state != READY)
                    changeState(READY);
            }
        }
        // just to be sure
        seenNodesModule->addMember(joinResponse->getSrcNode());
    }
}

// Return whether the given node is responsible for the key
bool Scamp::isSiblingFor(const NodeHandle& node, const OverlayKey& key, int numSiblings, bool* err) {
    return true;
}

// Return the next step for the routing of the given message
NodeVector *Scamp::findNode(const OverlayKey& key, int numRedundantNodes, int numSiblings, BaseOverlayMessage* msg) {
    NodeVector* nextHops;
    nextHops = new NodeVector(0);

    return nextHops;
}

// Called when the module is about to be destroyed
void Scamp::finishOverlay() {
    // remove this node from the overlay
    setOverlayReady(false);

    // statistics that are aggregated
    // recordScalar("SCAMP: Total number of heartbeats sent", totalHeartbeatsSent);
}

// Return the max amount of siblings that can be queried about
int Scamp::getMaxNumSiblings() {
    return 1;
}

// Return the max amount of redundant that can be queried about
int Scamp::getMaxNumRedundantNodes() {
    return 1;
}


void Scamp::findFriendModules(){
    // these are modules
    inViewModule = check_and_cast<View*> (getParentModule()->getSubmodule("inView"));
    partialViewModule = check_and_cast<View*> (getParentModule()->getSubmodule("partialView"));
    seenNodesModule = check_and_cast<View*> (getParentModule()->getSubmodule("seenNodesView"));
}

void Scamp::initializeFriendModules(){
    inViewModule->initializeView(thisNode, this);
    partialViewModule->initializeView(thisNode, this);
    seenNodesModule->initializeView(thisNode, this);
}


void Scamp::handleUDPMessage(BaseOverlayMessage* msg){

    ScampMessage* m = check_and_cast<ScampMessage*>(msg);
    if(DEBUG)
        EV << "[Scamp::handleUDPMessage() @ " << thisNode.getIp()
           << " (" << thisNode.getKey().toString(16) << ")]\n"
           << "    Received message with command: "
           << m->getCommand()
           << endl;

    if(state != READY && m->getCommand() != ADDEDTOPARTIALVIEW){
        EV << "Not READY --> DISCARD"
           << endl;
        delete m;
        return;
    }

    switch(m->getCommand()) {
        case ADDEDTOPARTIALVIEW:

            handlePartialViewAdd(m);
            delete m;
            break;

        case JOINREQUESTFORWARDED:{
            ForwardMessage* fm = check_and_cast<ForwardMessage*>(m);

            outfile.open("results\\JoinMessages.log", std::ios_base::app); // append instead of overwrite
            outfile << fm->getNode().getIp() << ";JOINREQUESTFORWARDED;" << simTime() << ";" << thisNode.getIp() << "\n";
            outfile.close();

            handleForwardedJoinRequest(fm);
            break;
        }
        case HEARTBEAT_FROM_LEAF:
        case HEARTBEAT:
            handleHeartbeatMessage(m);
            break;

        case REMOVE_ME:{
            LeaveMessage* lm = check_and_cast<LeaveMessage*>(m);

            outfile.open("results\\JoinMessages.log", std::ios_base::app); // append instead of overwrite
            outfile << lm->getNode().getIp() << ";REMOVE_ME;" << simTime() << ";" << thisNode.getIp() << "\n";
            outfile.close();

            handleRemoveMeMessage(lm);
            break;
        }

        case REPLACE_ME:{
            LeaveMessage* lm = check_and_cast<LeaveMessage*>(m);

            outfile.open("results\\JoinMessages.log", std::ios_base::app); // append instead of overwrite
            outfile << lm->getNode().getIp() << ";REPLACE_ME;" << simTime() << ";" << thisNode.getIp() << "\n";
            outfile.close();

            handleReplaceMeMessage(lm);
            break;
        }

        case NEW_NODE:{
            NewNodeMessage* nnm = check_and_cast<NewNodeMessage*>(m);

            outfile.open("results\\JoinMessages.log", std::ios_base::app); // append instead of overwrite
            outfile << nnm->getNode().getIp() << ";NEW_NODE;" << simTime() << ";" << thisNode.getIp() << "\n";
            outfile.close();

            seenNodesModule->addMember(nnm->getNode());
            delete nnm;
            break;
        }

        default:
            error("handleUDPMessage(): Unknown message type!");
            break;
        }

    // delete m;
}

void Scamp::handlePartialViewAdd(ScampMessage* m){

    inViewModule->addMember(m->getNode());

    if(state != READY)
        changeState(READY);
    
    // add node to seenNodes
    seenNodesModule->addMember(m->getNode());
    //delete m;
    return;
}

void Scamp::handleForwardedJoinRequest(ForwardMessage* m){
    if(DEBUG)
        EV << "[Scamp::handleForwardedJoinRequest() @ " << thisNode.getIp()
           << " (" << thisNode.getKey().toString(16) << ")]\n"
           << "    Received forwarded join request "
           << endl;

    NodeHandle requester = m->getNode();
    NodeHandle sender = m->getSourceNode();

    seenNodesModule->addMember(m->getNode());


    if (requester.getIp() != thisNode.getIp() && partialViewModule->addMember(requester)){
        // send accepted to the requester

        outfile.open("results\\JoinMessages.log", std::ios_base::app); // append instead of overwrite
        outfile << requester.getIp() << ";ADDEDTOPARTIALVIEW;" << simTime() << ";" << thisNode.getIp() << "\n";
        outfile.close();

        ScampMessage* message = new ScampMessage("ScampMessage");
        message->setCommand(ADDEDTOPARTIALVIEW);
        message->setNode(thisNode);
        // notify it about this
        sendMessageToUDP(requester, message);

        //showOverlayNeighborArrow(requester);
        showOverlayNeighborArrow(requester, false, "m=m,50,0,50,0;ls=red,1");

    } else if (partialViewModule->isPresent(requester)){
        // In case it gets back to me, I keep forwarding it
        forwardJoinRequest(sender, requester, m->getSteps() + 1);

    } else {
        forwardJoinRequest(sender, requester, m->getSteps() + 1);
    }

    delete m;
}

void Scamp::handleHeartbeatMessage(ScampMessage* m){
    if(DEBUG)
        EV << "[Scamp::handleHeartbeatMessage() @ " << thisNode.getIp()
           << " (" << thisNode.getKey().toString(16) << ")]\n"
           << "    Received " << m->getCommand() << " message "
           << endl;

    if(!inViewModule->isPresent(m->getNode())){
        if(DEBUG)
            EV << "Sender not in this inView anymore --> DISCARDED"
            << endl;

    } else {
        inViewModule->setHeartbeatArrival(m->getNode(), simTime().dbl());

        // Need to understand if messages I send are reached by nodes that are not in my partialView
        // If I get a heartbeat from a leaf node, and only this node is in my partialView, then I should connect to some other node
        // from my inView
        if (m->getCommand() == HEARTBEAT_FROM_LEAF){
            if (partialViewModule->getSize() == 1){
                if (partialViewModule->isPresent(m->getNode())){
                    // Connect to someone else, that is not m->getNode

                    if (isLeaf()){
                        if (DEBUG)
                            EV << "I am also leaf --> send another request to my bootstrapNode" << endl;
                        // need to connect to bootstrap node
                        ScampJoinCall* call = new ScampJoinCall("JoinCall");
                        call->setTime(simTime());
                        call->setAddress(thisNode.getIp());
                        call->setBitLength(JOINCALL_L(call));

                        RoutingType routingType = (defaultRoutingType == FULL_RECURSIVE_ROUTING ||
                                                   defaultRoutingType == RECURSIVE_SOURCE_ROUTING) ?
                                                  SEMI_RECURSIVE_ROUTING : defaultRoutingType;

                        // Each time get a new bootstrapNode
                        bootstrapNode = bootstrapList->getBootstrapNode(overlayId);

                        sendRouteRpcCall(OVERLAY_COMP, bootstrapNode, thisNode.getKey(), call, NULL, routingType);

                    } else {
                        std::vector<NodeHandle> node = inViewModule->getRandomNodesExcept(m->getNode(), 1);
                        ScampMessage* message = new ScampMessage("ScampMessage");
                        message->setCommand(ADDEDTOPARTIALVIEW);
                        message->setNode(thisNode);

                        // notify it about this
                        sendMessageToUDP(node[0], message);
                        partialViewModule->addMember(node[0]);
                        showOverlayNeighborArrow(node[0], false, "m=m,50,0,50,0;ls=red,1");
                        if (DEBUG)
                            EV << "Connected to some other node because I have only a leaf node in my partialView" << endl;
                    }
                }
            }
        }
    }

    delete m;
}

void Scamp::gracefulLeave(){

    std::vector<NodeHandle> inNodes = inViewModule->getNodeHandles();
    std::vector<NodeHandle> outNodes = partialViewModule->getNodeHandles();

    int m = 0;
    int in = (int)inNodes.size(); // inView size
    int out = (int)outNodes.size(); // partialView size

    if (useCwhenLeaving){

        // It doesn't make sense very much to me, for now I will do it the other way around
        if (in - joinRequestCopies - 1 >= out){
            m = out;
        } else {
            m = in - joinRequestCopies - 1;
        }

        // tell the other nodes to remove this node from their partialView, since I can only do this
        if(m <= 0){
            int j;
            for(j=0; j<in; j++){
                LeaveMessage* lm = new LeaveMessage("LeaveMessage");
                lm->setCommand(REMOVE_ME);
                lm->setNode(thisNode);
                lm->setNewNode(NodeHandle::UNSPECIFIED_NODE);

                sendMessageToUDP(inNodes.at(j), lm);
            }

        } else {

            int i;
            for(i=0; i<m; i++ ){
                // notify node that it has to replace me
                LeaveMessage* lm = new LeaveMessage("LeaveMessage");
                lm->setCommand(REPLACE_ME);
                lm->setNode(thisNode);
                lm->setNewNode(outNodes.at(i));

                sendMessageToUDP(inNodes.at(i), lm);
            }

            for(i=m; i<in; i++ ){
                LeaveMessage* lm = new LeaveMessage("LeaveMessage");
                lm->setCommand(REMOVE_ME);
                lm->setNode(thisNode);
                lm->setNewNode(NodeHandle::UNSPECIFIED_NODE);

                sendMessageToUDP(inNodes.at(i), lm);
            }
        }
    }


    else {
        if (in > out){
            m = out;
        } else {
            m = in;
        }

        int i;
        for(i=0; i<m; i++ ){
            // notify node that it has to replace me
            LeaveMessage* lm = new LeaveMessage("LeaveMessage");
            lm->setCommand(REPLACE_ME);
            lm->setNode(thisNode);
            lm->setNewNode(outNodes.at(i));

            sendMessageToUDP(inNodes.at(i), lm);
        }

        // send REMOVE_ME to the inView nodes exceeding the partialView nodes
        for(i=m; i<in; i++){
            LeaveMessage* lm = new LeaveMessage("LeaveMessage");
            lm->setCommand(REMOVE_ME);
            lm->setNode(thisNode);
            lm->setNewNode(NodeHandle::UNSPECIFIED_NODE);

            sendMessageToUDP(inNodes.at(i), lm);
        }
    }
}

void Scamp::handleNodeGracefulLeaveNotification(){

    if (state != READY){
        changeState(SHUTDOWN);
        recordScalar("SCAMP: Node had to leave but was not ready", 42);
        return;
    }

    if(DEBUG)
        EV << "[Scamp::handleNodeLeaveNotification() @ " << thisNode.getIp()
           << " (" << thisNode.getKey().toString(16) << ")]\n"
           << "    Leaving gracefully "
           << endl;

    outfile.open("results\\JoinMessages.log", std::ios_base::app); // append instead of overwrite
    outfile << thisNode.getIp() << ";LEAVE;" << simTime() << ";" << thisNode.getIp() << "\n";
    outfile.close();

    // notify others
    gracefulLeave();

    // need to delete the arrows
    std::vector<NodeHandle> nodes = partialViewModule->getNodeHandles();
    for(int i=0; i<(int)nodes.size(); i++){
        deleteOverlayNeighborArrow(nodes.at(i));
    }

    // delete views
    inViewModule->clearView();
    partialViewModule->clearView();

    changeState(SHUTDOWN);

}

void Scamp::handleRemoveMeMessage(LeaveMessage* m){
    if(DEBUG)
        EV << "[Scamp::handleRemoveMeMessage() @ " << thisNode.getIp()
           << " (" << thisNode.getKey().toString(16) << ")]\n"
           << "    Received RemoveMe message "
           << endl;

    if(partialViewModule->isPresent(m->getNode())){
        partialViewModule->removeMember(m->getNode());
        if(DEBUG)
            EV << "Removed node upon remove me message"
               << endl;

        deleteOverlayNeighborArrow(m->getNode());
        inViewModule->removeMember(m->getNode()); // Just to be sure

    } else {
        if(DEBUG)
            EV << "ERROR - Cannot remove this node from the partialView"
               << endl;
    }

    delete m;
}

void Scamp::handleReplaceMeMessage(LeaveMessage* m){
    if(DEBUG)
        EV << "[Scamp::handleReplaceMeMessage() @ " << thisNode.getIp()
           << " (" << thisNode.getKey().toString(16) << ")]\n"
           << "    Received ReplaceMe message "
           << endl;

    if(partialViewModule->isPresent(m->getNode())){

        partialViewModule->removeMember(m->getNode());
        deleteOverlayNeighborArrow(m->getNode());
        inViewModule->removeMember(m->getNode()); // Just to be sure

    } else {
        if(DEBUG)
            EV << "ERROR - Cannot remove this node from the partialView"
               << endl;
    }

    // add the other node. The other node must be different from this node
    if(m->getNewNode().getIp() != thisNode.getIp()){

        if(partialViewModule->addMember(m->getNewNode(), true)){
            showOverlayNeighborArrow(m->getNewNode(), false, "m=m,50,0,50,0;ls=red,1");

            outfile.open("results\\JoinMessages.log", std::ios_base::app); // append instead of overwrite
            outfile << m->getNode().getIp() << ";ADDEDTOPARTIALVIEW_REPLACEMENT;" << simTime() << ";" << m->getNewNode().getIp() << "\n";
            outfile.close();

            ScampMessage* message = new ScampMessage("ScampMessage");
            message->setCommand(ADDEDTOPARTIALVIEW);
            message->setNode(thisNode);

            // notify it about this
            sendMessageToUDP(m->getNewNode(), message);
        }
    }

    delete m;
}


int Scamp::getHeartbeatsSentLastPeriod(){
    return heartbeatsSentLastPeriod;
}


void Scamp::resetHeartbeatsSentLastPeriod(){
    heartbeatsSentLastPeriod = 0;
}

bool Scamp::inViewEmpty(){
    return inViewModule->isEmpty();
}

bool Scamp::partialViewEmpty(){
    return partialViewModule->isEmpty();
}

void Scamp::setLease(NodeHandle& node, double tm){
    Enter_Method_Silent();
    if(DEBUG)
        EV << "[Scamp::setLease() @ " << thisNode.getIp()
           << " (" << thisNode.getKey().toString(16) << ")]\n"
           << " Got in input node: " << node.getKey()
           << " Delay: " << tm
           << endl;

    // Launch a timer each time I am being called
    cMessage* leaseTimer = new cMessage("Remove from partial view");
    leaseTimer->addPar("RemoveNode");
    leaseTimer->addPar(node.getKey().toString().c_str());
    scheduleAt(simTime() + tm, leaseTimer);

}

bool Scamp::completedLease(){
    return !leasing;
}

void Scamp::rejoin(double tm){
    Enter_Method_Silent();
    if(DEBUG)
        EV << "[Scamp::rejoin() @ " << thisNode.getIp()
           << " (" << thisNode.getKey().toString(16) << ")]\n"
           << " Rejoin after delay: " << tm
           << endl;

    scheduleAt(simTime() + tm, rejoinTimer);
    leasing = true;
}

bool Scamp::isReady(){
    if (state == READY){
        return true;
    }

    return false;
}

bool Scamp::isLeaf(){
    if (partialViewModule->getSize() == 1 && inViewModule->getSize() == 1){
        if (partialViewModule->getNodeHandles().at(0) == inViewModule->getNodeHandles().at(0)){
            return true;
        }
    }

    return false;
}

void Scamp::route(const OverlayKey& key, CompType destComp, CompType srcComp, cPacket* msg, const std::vector<TransportAddress>& sourceRoute, RoutingType routingType){
    if(DEBUG)
        EV << "[Scamp::route() @ " << thisNode.getIp()
           << " (" << thisNode.getKey().toString(16) << ")]\n"
           << "Got here in the overlay when sending app message with callRoute()"
           << endl;
}


NodeVector* Scamp::neighborSet(int num){
    if(DEBUG)
        EV << "[Scamp::neighborSet() @ " << thisNode.getIp()
           << " (" << thisNode.getKey().toString(16) << ")]"
           << endl;

    NodeVector* nodes = new NodeVector();
    std::vector<NodeHandle> out = partialViewModule->getRandomNodes(num);

    for(int i=0; i<(int)out.size(); i++){
        if(!nodes->contains(out.at(i).getKey())){
            nodes->add(out.at(i));
        }
    }
    // Add myself to the result, since this will be used for broadcasting
    nodes->add(thisNode);
    return nodes;
}

void Scamp::handleAppMessage(cMessage* msg){
    if(DEBUG)
        EV << "[Scamp::handleAppMessage() @ " << thisNode.getIp()
           << " (" << thisNode.getKey().toString(16) << ")]\n"
           << endl;

    // TODO add delay?
    send(msg, "appOut");

}



