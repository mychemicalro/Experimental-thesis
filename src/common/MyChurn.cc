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
 * Then I will apply the churning with some churnRate C.
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
    churnTimer = new cMessage("churnTimer");
    createTimer = new cMessage("createTimer");
    scheduleAt(simTime() + createInterval, createTimer);
    endSimulationTimer = new cMessage("endSimulationTimer");
    measureJoinOpsTimer = new cMessage("measureJoinOpsTimer");
    scheduleAt(simTime() + 2, measureJoinOpsTimer);
    cOV_JoinerOps.setName("Number of join operations completed");
    globalStatistics = GlobalStatisticsAccess().get();
    globalNodesList = GlobalNodeListAccess().get();


    // logic parameters
    initPeriod = true;
    churnPeriod = false;
    stabilityPeriod = false;

    joinOps = 0;

    WATCH(leavers);
    WATCH(joiners);
    WATCH(initPeriod);
    WATCH(churnPeriod);
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

            if (joiners > 0 && leavers > 0){
                scheduleAt(simTime() + churnInterval, churnTimer);

            } else {
                globalStatistics->addStdDev("MyChurn: Churn period ended -> ", simTime().dbl());
                churnPeriod = false;
                scheduleAt(simTime() + endSimulationInterval, endSimulationTimer);
            }

        } else if(joiners > 0 && leavers > 0){

            // kill a node iff not in the permanents one
            int killed = 0;
            while (killed <= leavers){

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

            for(i=0; i <= joiners; i++){
                // create nodes
                TransportAddress* ta = underlayConfigurator->createNode(type);
                delete ta; // Address not needed in this churn model
                joiners--;
                joinOps++;
            }

            // scheduleAt(simTime() + churnInterval, churnTimer);
            churnPeriod = false;
            globalStatistics->addStdDev("MyChurn: Churn period ended -> ", simTime().dbl());
            scheduleAt(simTime() + endSimulationInterval, endSimulationTimer);
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
            scheduleAt(simTime(), churnTimer);  // TODO: Schedule right away the first churn? -> Yes, for now.
            globalStatistics->addStdDev("MyChurn: Initialization period ended -> ", simTime().dbl());
        }

    } else if (msg == endSimulationTimer){
        cancelEvent(endSimulationTimer);
        endSimulation();

    } else if(msg == measureJoinOpsTimer){

        cancelEvent(measureJoinOpsTimer);
        cOV_JoinerOps.record(joinOps);
        scheduleAt(simTime() + 2, measureJoinOpsTimer);

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
}


