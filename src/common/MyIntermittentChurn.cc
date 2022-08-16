/*
 * MyIntermittentChurn.cc
 *
 *  Created on: 08/giu/2022
 *      Author: DinuFC
 */



#include <assert.h>
#include <TransportAddress.h>
#include <GlobalStatistics.h>
#include <GlobalStatisticsAccess.h>
#include <UnderlayConfigurator.h>
#include <MyIntermittentChurn.h>

Define_Module(MyIntermittentChurn);


void MyIntermittentChurn::initializeChurn(){

    Enter_Method_Silent();
    churnRate = par("churnRate");
    churnInterval = par("churnInterval");
    createInterval = par("createInterval");
    endSimulationInterval = par("endSimulationInterval");
    intervalBetweenChurnPeriods = par("intervalBetweenChurnPeriods");

    joiners = par("joiners");
    leavers = par("leavers");
    rounds = par("rounds");
    doneRounds = 0;

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


void MyIntermittentChurn::handleMessage(cMessage* msg){

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

                                    break;
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
                if (doneRounds < rounds - 1){
                    scheduleAt(simTime() + churnInterval, churnTimer);
                    doneRounds ++;
                } else {
                    scheduleAt(simTime() + intervalBetweenChurnPeriods, churnTimer);
                    doneRounds = 0;
                }
            } else {
                churnPeriod = false;
                globalStatistics->addStdDev("MyIntermittentChurn: Churn period ended -> ", simTime().dbl());
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

            for(i=0; i<=joiners; i++){
                // create nodes
                TransportAddress* ta = underlayConfigurator->createNode(type);
                delete ta; // Address not needed in this churn model
                joiners--;
                joinOps++;
            }

            churnPeriod = false;
            globalStatistics->addStdDev("MyIntermittentChurn: Churn period ended -> ", simTime().dbl());
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
            globalStatistics->addStdDev("MyIntermittentChurn: Initialization period ended -> ", simTime().dbl());
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

void MyIntermittentChurn::updateDisplayString(){
    char buf[80];
    sprintf(buf, "MyIntermittentChurn");
    getDisplayString().setTagArg("t", 0, buf);
}

MyIntermittentChurn::~MyIntermittentChurn(){
    cancelAndDelete(churnTimer);
    cancelAndDelete(createTimer);
    cancelAndDelete(endSimulationTimer);
    cancelAndDelete(measureJoinOpsTimer);
}


