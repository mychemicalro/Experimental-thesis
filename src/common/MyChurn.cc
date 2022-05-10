/*
 * MyChurn.cc
 *
 *  Created on: 27/dic/2021
 *      Author: DinuFC
 */

#include <assert.h>

#include <TransportAddress.h>
#include <GlobalStatistics.h>
#include <GlobalStatisticsAccess.h>
#include <UnderlayConfigurator.h>

#include <MyChurn.h>

Define_Module(MyChurn);

/**
 * I need to simulate the paper experiment.
 * Need to have three phases, init, churn and stability.
 * In the init phase I will create the overlay reaching N nodes.
 * Then I will apply the churning with some churnRate C. Then, optionally I have the lease period.
 * Finally, I will have a stability period.
 */

void MyChurn::initializeChurn(){

    Enter_Method_Silent();
    churnRate = par("churnRate");
    churnInterval = par("churnInterval");
    createInterval = par("createInterval");
    endSimulationInterval = par("endSimulationInterval");
    joiners = par("joiners");
    leavers = par("leavers");
    stopSimulation = par("stopSimulation");
    permanentNodes = par("permanentNodes");
    useLease = par("useLease");

    churnTimer = new cMessage("churnTimer");

    createTimer = new cMessage("createTimer");
    scheduleAt(simTime() + createInterval, createTimer);

    endSimulationTimer = new cMessage("endSimulationTimer");

    measureJoinOpsTimer = new cMessage("measureJoinOpsTimer");
    scheduleAt(simTime() + 5, measureJoinOpsTimer);

    leaseTimer = new cMessage("leaseTimer");

    cOV_JoinerOps.setName("Number of join operations completed");

    globalStatistics = GlobalStatisticsAccess().get();
    globalNodesList = GlobalNodeListAccess().get();


    // logic parameters
    initPeriod = true;
    churnPeriod = false;
    leasePeriod = false;
    leaseStarted = false;
    stabilityPeriod = false;


    joinOps = 0;

    WATCH(leavers);
    WATCH(joiners);
    WATCH(initPeriod);
    WATCH(churnPeriod);
    WATCH(leasePeriod);
    WATCH(stabilityPeriod);
}


void MyChurn::handleMessage(cMessage* msg){

    if(!msg->isSelfMessage()){
        delete msg;
        return;
    }

    if(msg == churnTimer){
        // time to delete and create some nodes
        cancelEvent(churnTimer);

        if(joiners - churnRate >= 0 && leavers - churnRate >= 0){

            // kill a node iff not in the permanents one
            int killed = 0;
            while (killed < churnRate){

                try {
                    TransportAddress* addr = globalNodesList->getRandomAliveNode(type.typeID);
                    NodeHandle* nh = globalNodesList->getNodeHandle(addr->getIp());

                    for(int i=0; i<=simulation.getLastModuleId(); i++){
                        // For a more efficient way I could create
                        cModule* module = simulation.getModule(i);
                        if(module && dynamic_cast<Scamp*>(module)) {
                            Scamp* v = check_and_cast<Scamp*>(module);

                            if (nh->getKey() == v->getThisNode().getKey() && v->isReady()){
                                // Node's overlay found and is ready

                                if (nh->getKey() > OverlayKey(permanentNodes)){
                                    // I am allowed to kill
                                    int oldTerminalCount = terminalCount;
                                    underlayConfigurator->preKillNode(type, addr);
                                    assert ((oldTerminalCount - 1) == terminalCount);
                                    leavers--;
                                    killed++;

                                    break; // break the for
                                }
                            }
                        }
                    }

                } catch (std::exception& e){
                    EV << "Something really bad happened" << e.what() << endl;
                }
            }

            int i;

            for(i=0; i<churnRate; i++){
                // create nodes
                TransportAddress* ta = underlayConfigurator->createNode(type);
                delete ta; // Address not needed in this churn model
                joiners--;
                joinOps++;
            }

            scheduleAt(simTime() + churnInterval, churnTimer);

        } else if(joiners > 0 && leavers > 0){

            // kill a node iff not in the permanents one
            int killed = 0;
            while (killed < leavers){

                try {
                    TransportAddress* addr = globalNodesList->getRandomAliveNode(type.typeID);
                    NodeHandle* nh = globalNodesList->getNodeHandle(addr->getIp());

                    for(int i=0; i<=simulation.getLastModuleId(); i++){
                        // For a more efficient way I could create
                        cModule* module = simulation.getModule(i);
                        if(module && dynamic_cast<Scamp*>(module)) {
                            Scamp* v = check_and_cast<Scamp*>(module);

                            if (nh->getKey() == v->getThisNode().getKey() && v->isReady()){
                                // Node's overlay found and is ready

                                if (nh->getKey() > OverlayKey(permanentNodes)){
                                    // I am allowed to kill
                                    int oldTerminalCount = terminalCount;
                                    underlayConfigurator->preKillNode(type, addr);
                                    assert ((oldTerminalCount - 1) == terminalCount);
                                    leavers--;
                                    killed++;

                                    break; // break the for
                                }
                            }
                        }
                    }

                } catch (std::exception& e){
                    EV << "Something really bad happened" << e.what() << endl;
                }
            }

            int i;

            for(i=0; i<joiners; i++){
                // create nodes
                TransportAddress* ta = underlayConfigurator->createNode(type);
                delete ta; // Address not needed in this churn model
                joiners--;
                joinOps++;
            }

            scheduleAt(simTime() + churnInterval, churnTimer);

        } else {

            churnPeriod = false;

            globalStatistics->addStdDev("MyChurn: Churn period ended -> ", simTime().dbl());

            if (useLease){
                leasePeriod = true;
                scheduleAt(simTime() + 20, leaseTimer);

            } else {
                scheduleAt(simTime() + endSimulationInterval, endSimulationTimer);
            }
        }

    } else if (msg == createTimer){
        // time to add a node

        cancelEvent(createTimer);

        if(terminalCount < targetOverlayTerminalNum){
            TransportAddress* ta = underlayConfigurator->createNode(type);
            delete ta; // Address not needed in this churn model

            scheduleAt(simTime()+ createInterval, msg);

        } else {
            // I'm done with the init phase
            initPeriod = false;
            underlayConfigurator->initFinished();

            // switch to churn period
            churnPeriod = true;
            scheduleAt(simTime()+ churnInterval, churnTimer);
            globalStatistics->addStdDev("MyChurn: Initialization period ended -> ", simTime().dbl());
        }

    } else if (msg == endSimulationTimer){
        // When to stop the simulation? -> joiners and leavers were processed -> churn period ended

        cancelEvent(endSimulationTimer);
        endSimulation();

    } else if(msg == measureJoinOpsTimer){

        cancelEvent(measureJoinOpsTimer);
        cOV_JoinerOps.record(joinOps);
        scheduleAt(simTime() + 5, measureJoinOpsTimer);

    } else if(msg == leaseTimer) {

            if (!leaseStarted){
                // let the games begin
                leaseStarted = true;

                // It will contain N pairs, with the leaseDelay of each node.
                std::map<NodeHandle, double> data;

                for(int i=0; i<=simulation.getLastModuleId(); i++){
                    cModule* module = simulation.getModule(i);
                    if(module && dynamic_cast<View*>(module)) {
                        View* v = check_and_cast<View*>(module);

                        // need only the inView
                        if(v->getMaxSize() == 0 && v->getOwnerNode().getKey().isUnspecified() == 0){
                            double randomDelay = uniform(0, 1000); // decide the upper value of the interval
                            // EV << "Node: " << v->getOwnerNode().getIp() << " lease expires in: " << randomDelay << " seconds" << endl;
                            data.insert(std::pair<NodeHandle, double>(v->getOwnerNode(), randomDelay));
                            Scamp& ol = v->getOverlay();
                            ol.rejoin(randomDelay);
                        }
                    }
                }

                /**
                 * For each partialView
                 *      I get the overlay
                 *      for each node in the partialView
                 *          set the leaseDelay invoking an overlay method
                 */
                for(int i=0; i<=simulation.getLastModuleId(); i++){
                    cModule* module = simulation.getModule(i);
                    if(module && dynamic_cast<View*>(module)) {
                        View* v = check_and_cast<View*>(module);

                        // now I consider partialViews
                        if(v->getMaxSize() > 0 && v->getMaxSize() < 42 && v->getOwnerNode().getKey().isUnspecified() == 0){
                            Scamp& ol = v->getOverlay();

                            std::vector<NodeHandle> nodes = v->getNodeHandles();
                            for(int i=0; i<(int)nodes.size(); i++){
                                // It may be that a node that is in someone's partialView does not exists anymore, not having an inView
                                // therefore not being present in data.
                                try {
                                   ol.setLease(nodes.at(i), data.at(nodes.at(i)));

                                } catch (const std::out_of_range& oor) {

                                }

                            }
                        }
                    }
                }

            } else {
                // Check if lease is done
                // For each overlay
                //      call the completedLease() method
                bool completed = true;

                for(int i=0; i<=simulation.getLastModuleId(); i++){
                    cModule* module = simulation.getModule(i);
                    if(module && dynamic_cast<Scamp*>(module)) {
                        Scamp* v = check_and_cast<Scamp*>(module);
                        if (!v->completedLease()){
                            completed = false;
                            break;
                        }
                    }
                }

                if (completed){
                    leasePeriod = false;
                    globalStatistics->addStdDev("MyChurn: Lease period ended -> ", simTime().dbl());
                }
            }

            if (!leasePeriod){
                // when leasePeriod ends
                stabilityPeriod = true;

                // set the end of the simulation
                if(stopSimulation){
                    scheduleAt(simTime() + endSimulationInterval, endSimulationTimer);
                }

            } else {
                // reschedule this
                scheduleAt(simTime() + 5, leaseTimer);
            }


    } else {
        EV << "Message type not supported" << endl;
    }
}

void MyChurn::updateDisplayString(){
    char buf[80];
    sprintf(buf, "MyChurn");
    getDisplayString().setTagArg("t", 0, buf);
}

MyChurn::~MyChurn(){
    cancelAndDelete(churnTimer);
    cancelAndDelete(createTimer);
    cancelAndDelete(endSimulationTimer);
    cancelAndDelete(measureJoinOpsTimer);
    cancelAndDelete(leaseTimer);
}


