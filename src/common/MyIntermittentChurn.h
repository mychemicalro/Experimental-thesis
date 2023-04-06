/*
 * MyIntermittentChurn.h
 *
 *  Created on: 08/giu/2022
 *      Author: DinuFC
 */

#ifndef MYINTERMITTENTCHURN_H_
#define MYINTERMITTENTCHURN_H_


#include <ChurnGenerator.h>
#include "GlobalNodeListAccess.h"
#include "ScampConnectivityProbe.h"

/**
 * Churn generating class
 */

class MyIntermittentChurn: public ChurnGenerator {
    public:
        void handleMessage(cMessage* msg);
        void initializeChurn();
        ~MyIntermittentChurn();

    protected:
        void updateDisplayString();

    private:
        double churnInterval;
        double createInterval;
        double endSimulationInterval;
        double intervalBetweenChurnPeriods;

        int churnRate;
        int joiners;
        int leavers;
        int rounds;
        int doneRounds;

        bool stopSimulation;
        int permanentNodes;
        int joinOps;
        bool useLease;

        cMessage* churnTimer;
        cMessage* createTimer;
        cMessage* endSimulationTimer;
        cMessage* measureJoinOpsTimer;

        GlobalStatistics* globalStatistics;
        GlobalNodeList* globalNodesList;

        bool initPeriod;
        bool churnPeriod;
        bool stabilityPeriod;

        cOutVector cOV_JoinerOps;

};


#endif /* MYINTERMITTENTCHURN_H_ */
