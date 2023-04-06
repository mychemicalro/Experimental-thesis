/*
 * ConnectivityProbe.cc
 *
 *  Created on: 30/dic/2021
 *      Author: DinuFC
 */

#include "ScampConnectivityProbe.h"

Define_Module(ScampConnectivityProbe);

ScampTopologyNode::ScampTopologyNode(int viewID){
    this->viewID = viewID;
    visited = false;
}

View* ScampTopologyNode::getView() const {
    return check_and_cast<View*>(simulation.getModule(viewID));
}

void ScampConnectivityProbe::initialize(){
    globalStatistics = GlobalStatisticsAccess().get();
    globalNodesList = GlobalNodeListAccess().get();

    avgPartialViewSize = 0;
    avgInViewSize = 0;
    leafNodes = 0;

    probeInterval = par("connectivityProbeInterval");
    probeTimer = new cMessage("probeTimer");
    R = 0;
    dag_id = par("dag_id");
    ss << "C:\\Users\\DinuFc\\Desktop\\tesi\\last-run\\" << dag_id << "_DAG";

    WATCH(R);
    WATCH(avgPartialViewSize);
    WATCH(avgInViewSize);
    WATCH(leafNodes);

    if(probeInterval > 0.0){
        scheduleAt(simTime() + probeInterval, probeTimer);

        // initialize output vectors if needed
        cOV_NodeCount.setName("Total node count");
        cOV_MaximumComponent.setName("Largest connected component");
        cOV_AvgConnectivity.setName("Average connectivity");
        cOV_InitialNodes.setName("Percentage of initial nodes still in the overlay");
        cOV_WeakComponents.setName("Number of weak components of the overlay");
        cOV_avgPartialViewSize.setName("Average partialViews size");
        cOV_LeafsNumber.setName("Number of leaf nodes in the overlay");

        // generate external file name
        outfile.open(ss.str().c_str(), std::ios_base::trunc); // append instead of overwrite (app for append)
        outfile.close();
    }
}


ScampConnectivityProbe::~ScampConnectivityProbe(){
    // destroy self timer messages
    cancelAndDelete(probeTimer);
}


void ScampConnectivityProbe::handleMessage(cMessage* msg){

    // catch self timer messages
    if(msg->isName("probeTimer")) {

        //reset timer
        cancelEvent(probeTimer);
        scheduleAt(simTime() + probeInterval, msg);

        computeData();
    }
}

void ScampConnectivityProbe::computeData(){

    // get the partialViews and InViews of all nodes
    extractPartialViewTopology();
    extractInViewTopology();

    if (Topology.size() == 0){
        // Nothing to do now
        return;
    }

    unsigned int maxComponent = 0;
    unsigned int totalCount = 0; // used for the average connectivity
    unsigned int initialNodes = 0; // number of initial nodes still in the overlay
    leafNodes = 0;
    avgPartialViewSize = 0;

    outfile.open(ss.str().c_str(), std::ios_base::app);
    outfile << "Starting run at simTime(): " << simTime() << "\n";

    for (ScampTopology::iterator itTopology = Topology.begin(); itTopology != Topology.end(); ++itTopology) {
        unsigned int count = getComponentSize(itTopology->second.getView()->getOwnerNode().getKey());

        if(count > maxComponent) {
            maxComponent = count;
        }
        resetTopologyNodes();
        totalCount += count;

        // count the number of initial nodes remaining in the overlay
        if(itTopology->second.getView()->getOwnerNode().getKey() <= OverlayKey(Topology.size())){
            initialNodes++;
        }

        // since I am iterating all partialViews ...
        // - Write DAG
        std::vector<NodeHandle> nodes = itTopology->second.getView()->getNodeHandles(); // These are the nodeHandles in the partialView
        for(int i=0; i<(int)nodes.size(); i++){
            outfile << itTopology->second.getView()->getOwnerNode().getKey() << ";" << nodes.at(i).getKey() << "\n";
        }

        // Trace leaf nodes
        if (itTopology->second.getView()->getOverlay().isLeaf()){
            leafNodes ++;
        }

        // Trace avgPartialView size
        avgPartialViewSize += itTopology->second.getView()->getSize();

    }

    outfile.close();


    unsigned int weakComponents = 0;
    for (ScampTopology::iterator inTop = InTopology.begin(); inTop != InTopology.end(); ++inTop) {

        if(inTop->second.visited == false){
            getWeakComponents(inTop->second.getView()->getOwnerNode().getKey());
            weakComponents ++;
        }
    }

    R = (((double)totalCount / ((double)Topology.size())) * 100.0 ) / (double)Topology.size();
    avgPartialViewSize = avgPartialViewSize / Topology.size();

    cOV_NodeCount.record((double)Topology.size());
    cOV_MaximumComponent.record((double)maxComponent);
    cOV_AvgConnectivity.record((((double)totalCount / ((double)Topology.size())) * 100.0 ) / (double)Topology.size());
    cOV_InitialNodes.record((double) initialNodes * 100.0 / (double)Topology.size());
    cOV_WeakComponents.record(weakComponents);
    cOV_avgPartialViewSize.record(avgPartialViewSize);
    cOV_LeafsNumber.record(leafNodes);


    Topology.clear();
    InTopology.clear();

}
void ScampConnectivityProbe::resetTopologyNodes(){
    for(ScampTopology::iterator itTopology = Topology.begin(); itTopology != Topology.end(); ++itTopology) {
        itTopology->second.visited = false;
    }

    for(ScampTopology::iterator inTop = InTopology.begin(); inTop != InTopology.end(); ++inTop) {
        inTop->second.visited = false;
    }
}

unsigned int ScampConnectivityProbe::getComponentSize(OverlayKey key){
    ScampTopology::iterator itEntry = Topology.find(key);
    if(itEntry != Topology.end() && itEntry->second.visited == false) {
        int count = 1;
        itEntry->second.visited = true;
        View* v = itEntry->second.getView();
        std::vector<NodeHandle> nodes = v->getNodeHandles();
        int i;
        for (i=0; i<(int)nodes.size(); i++){
            count += getComponentSize(nodes.at(i).getKey());
        }
        return count;
    }
    return 0;
}

int ScampConnectivityProbe::getWeakComponents(OverlayKey key){
    ScampTopology::iterator itEntry = InTopology.find(key);
    if(itEntry != InTopology.end() && itEntry->second.visited == false) {
        itEntry->second.visited = true;
        View* inv = itEntry->second.getView();
        std::vector<NodeHandle> InNodes = inv->getNodeHandles();

        ScampTopology::iterator En = Topology.find(key);
        View* pv = En->second.getView();
        std::vector<NodeHandle> OutNodes = pv->getNodeHandles();

        int i;
        int c = 1;
        for (i=0; i<(int)InNodes.size(); i++){
            c += getWeakComponents(InNodes.at(i).getKey());
        }

        for (i=0; i<(int)OutNodes.size(); i++){
            c += getWeakComponents(OutNodes.at(i).getKey());
        }

        return c;
    }

    return 0;
}

void ScampConnectivityProbe::extractInViewTopology(){
    for(int i=0; i<=simulation.getLastModuleId(); i++){
        cModule* module = simulation.getModule(i);
        if(module && dynamic_cast<View*>(module)) {
            View* v = check_and_cast<View*>(module);
            if(v->getMaxSize() == 0 && v->getOwnerNode().getKey().isUnspecified() == 0){
                if(v->getOverlay().isReady()){
                    ScampTopologyNode temp(i);
                    InTopology.insert(std::make_pair(v->getOwnerNode().getKey(), temp));

                }
            }
        }
    }
}

void ScampConnectivityProbe::extractPartialViewTopology(){
    for(int i=0; i<=simulation.getLastModuleId(); i++){
        cModule* module = simulation.getModule(i);
        if(module && dynamic_cast<View*>(module)) {
            View* v = check_and_cast<View*>(module);
            if(v->getMaxSize() > 0 && v->getMaxSize() < 42 && v->getOwnerNode().getKey().isUnspecified() == 0){
                if(v->getOverlay().isReady()){
                    ScampTopologyNode temp(i);
                    Topology.insert(std::make_pair(v->getOwnerNode().getKey(), temp));
                }
            }
        }
    }
}

void ScampConnectivityProbe::finish(){
    computeData();
}







