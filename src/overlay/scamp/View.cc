/*
 * View.cc
 *
 *  Created on: 30/nov/2021
 *      Author: DinuFC
 */


#include "Scamp.h"
#include "View.h"
#include "vector"
#include <algorithm>

#define DEBUG false

using namespace std;

Define_Module(View);

std::ostream& operator<<(std::ostream& os, const NodeEntry& e) {
    os << e.nodeHandle;
    return os;
};

void View::initialize(int stage) {
    // because of IPAddressResolver, we need to wait until interfaces
    // are registered, address auto-assignment takes place etc.
    if(stage != MIN_STAGE_OVERLAY)
        return;

    WATCH_MAP(members);
}

void View::handleMessage(cMessage* msg){
    error("This module doesn't handle messages, it runs only in initialize()");
}

void View::initializeView(const NodeHandle& owner, Scamp* overlay) {
    this->overlay = overlay;
    maxSize = par("maxSize");
    thisNode = owner;
    members.clear();
    if(maxSize == 0){
        t = INVIEW;
    } else if(maxSize == 42){
        t = SEENVIEW;
    } else {
        t = PARTIALVIEW;
    }
}

NodeHandle& View::getOwnerNode(){
    return thisNode;
}


uint32_t View::getSize(){
    return members.size();
}

uint32_t View::getMaxSize(){
    return maxSize;
}

bool View::room(){
    if ((int) getSize() < maxSize)
        return true;
    return false;
}

bool View::addMember(const NodeHandle& node, bool force){
    //TODO is this necessary? Make a CommonAPI update() upcall to inform application
    // about our new neighbor in the successor list

    // if this is an inView or seenView, behave differently
    if(getMaxSize() == 0 || getMaxSize() == 42){
        if(!isPresent(node)){
            OverlayKey k = node.getKey();
            NodeEntry entry;
            entry.nodeHandle = node;
            entry.lastHeartbeatArrival = simTime().dbl();
            members.insert(make_pair(k, entry));

            if (getMaxSize() == 0){ // If inView, then inform PBFT
                this->overlay->callUpdate(node, true);
            }

            return true;
        }
    }

    // if there is space in this partialView and the node is not already in it
    if(room() && !isPresent(node)){

        if(force){
            // add the node no matter what
            OverlayKey k = node.getKey();
            NodeEntry entry;
            entry.nodeHandle = node;
            members.insert(make_pair(k, entry));

            // inform PBFT
            this->overlay->callUpdate(node, true);

            return true;
        }

        // check probability
        double addProbability = (double) 1/(1 + getSize());
        double randomNumber = uniform(0, 1);

        if(DEBUG)
            EV << "    RANDOM NUMBER: " << randomNumber << "\n"
               << "    ADD PROBABILITY: " << addProbability
               << endl;

        if (addProbability > randomNumber){
            OverlayKey k = node.getKey();
            NodeEntry entry;
            entry.nodeHandle = node;
            members.insert(make_pair(k, entry));

            // inform PBFT
            this->overlay->callUpdate(node, true);

            return true;
        }
    }
    return false;

}

const NodeHandle& View::getMember(uint32_t pos){
    // check boundaries
    if (pos == 0 && members.size() == 0)
        return NodeHandle::UNSPECIFIED_NODE;

    if (pos >= members.size()) {
        error("Index out of bound (View, getMember())");
    }

    std::map<OverlayKey, NodeEntry>::iterator it = members.begin();
    for (uint32_t i= 0; i < pos; i++) {
        it++;
        if (i == (pos-1))
            return it->second.nodeHandle;
    }
    return it->second.nodeHandle;

}


bool View::handleFailedNode(const TransportAddress& failed){
    // Notify the application about this event
    overlay->callUpdate(failed, false);
    return true;
}

void View::removeMember(const NodeHandle& node){
    for (std::map<OverlayKey, NodeEntry>::iterator it = members.begin(); it != members.end();){
        if (it->second.nodeHandle.getIp() == node.getIp()){
            if(DEBUG)
                EV << "Node -> DELETED" << endl;
            members.erase(it++);

            // inform PBFT
            this->overlay->callUpdate(node, false);

        } else {
            it ++;
        }
    }
}

bool View::isPresent(const NodeHandle& node){
    std::map<OverlayKey, NodeEntry>::iterator it = members.begin();
    if (isEmpty())
        return false;

    while (it != members.end()){
        if (node == it->second.nodeHandle){
            return true;
        } else {
            it++;
        }
    }
    return false;
}

vector<NodeHandle> View::getRandomNodesExcept(const NodeHandle& exception, int howMany){
    vector<NodeHandle> result;

    int m = 0;
    int s = (int) getSize();

    if (howMany < s){
        m = howMany;
    } else {
        m = s;
    }
    // pick m from members
    int pos = (int) uniform(0, s);

    // case in which the exception is the only node in the partialView
    if (s == 1 && isPresent(exception)){
        return result;
    }
    int i;
    for (i=0; i<m; i++){
        NodeHandle nh = getMember((pos + i)%s);
        if (nh.getIp() != exception.getIp()){
            result.push_back(nh);
            if(DEBUG)
                EV << nh.getIp() << " Added to result of random nodes " << endl;
        }
    }
    // Picked the exception, one missing
    if ((int)result.size() < m){
        // There are unpicked nodes
        if(DEBUG)
            EV << "Missing a node to return " << endl;
        if (s-1 >= m){
            // Pick the next one
            NodeHandle nh = getMember((pos + i)%s);
            if (nh.getIp() != exception.getIp()){
                result.push_back(nh);
                if(DEBUG)
                    EV << nh.getIp() << " Added to result of random nodes " << endl;
            }
        }
    }
    return result;
}


void View::clearView(){
    members.clear();
}

bool View::isEmpty(){
    if (members.size() == 0)
        return true;
    else
        return false;
}

void View::setHeartbeatArrival(const NodeHandle& node, double arrival){

    if (isEmpty())
        return;

    std::map<OverlayKey, NodeEntry>::iterator it = members.begin();
    while (it != members.end()){
        if (node == it->second.nodeHandle){
            if(DEBUG)
                EV << "Heartbeat for node " << node.getIp() << " is: " << it->second.lastHeartbeatArrival << endl;
            it->second.lastHeartbeatArrival = arrival;
            if(DEBUG)
                EV << "Heartbeat for node " << node.getIp() << " is NOW: " << it->second.lastHeartbeatArrival << endl;
            return;
        } else {
            it++;
        }
    }
}

std::vector<NodeHandle> View::prune(double now, double aliveTimeout){
    int res = 0;
    std::vector<NodeHandle> deletedNodes;

    if (isEmpty())
        return deletedNodes;

    for (std::map<OverlayKey, NodeEntry>::iterator it = members.begin(); it != members.end();){
        if (it->second.lastHeartbeatArrival + aliveTimeout < now){
            if(DEBUG)
                EV << "Heartbeat for node " << it->second.nodeHandle.getIp() << " is too old --> DELETED" << endl;

            // inform PBFT
            this->overlay->callUpdate(it->second.nodeHandle, true);

            deletedNodes.push_back(it->second.nodeHandle);
            handleFailedNode(it->second.nodeHandle);
            members.erase(it++);
            res ++;



        } else {
            it ++;
        }
    }

    return deletedNodes;
}

vector<NodeHandle> View::getNodeHandles(){
    std::vector<NodeHandle> nodes;

    for (int i=0; i<(int)getSize(); i++){
        NodeHandle nh = getMember(i);
        nodes.push_back(nh);
    }
    std::sort(nodes.begin(), nodes.end());
    return nodes;
}

ViewType View::getType(){
    return t;
}

vector<NodeHandle> View::getRandomNodes(int howMany){
    vector<NodeHandle> result;

    int m = 0;
    int s = (int) getSize();

    if (howMany < s){
        m = howMany;
    } else {
        m = s;
    }

    // pick m from members
    int pos = (int) uniform(0, s);

    int i;
    for (i=0; i<m; i++){
        NodeHandle nh = getMember((pos + i)%s);
        result.push_back(nh);
    }

    return result;
}

Scamp& View::getOverlay(){
    return *overlay;
}

