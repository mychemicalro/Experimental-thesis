/*
 * MyChurn.h
 *
 *  Created on: 27/dic/2021
 *      Author: DinuFC
 */


#ifndef __MYCHURN_H_
#define __MYCHURN_H_

#include <ChurnGenerator.h>
#include "GlobalNodeListAccess.h"
#include "ScampConnectivityProbe.h"

/**
 * Churn generating class
 */

class MyChurn: public ChurnGenerator{
    public:
        void handleMessage(cMessage* msg);
        void initializeChurn();
        ~MyChurn();

    protected:
        void updateDisplayString();

    private:
        double churnInterval;
        double createInterval;
        double endSimulationInterval;
        int churnRate;
        int joiners;
        int leavers;
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

#endif
