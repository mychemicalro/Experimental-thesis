/*
 * ConnectivityProbe.h
 *
 *  Created on: 30/dic/2021
 *      Author: DinuFC
 */

#ifndef __CONNECTIVITYPROBE_H__
#define __CONNECTIVITYPROBE_H__

#include <omnetpp.h>
#include <NodeHandle.h>
#include <fstream>
#include <sstream>
#include <iostream>
#include "GlobalStatisticsAccess.h"
#include "GlobalNodeListAccess.h"
#include "View.h"
#include "Scamp.h"

class ScampTopologyNode{
    public:
        ScampTopologyNode(int viewID);
        View* getView() const;
        bool visited;
        int viewID;
};

typedef std::map<OverlayKey, ScampTopologyNode> ScampTopology;

/**
 * This module has to compute the network connectivity periodically.
 * As for now, it will have to check how many nodes a node can reach.
 */
class ScampConnectivityProbe : public cSimpleModule
{
    public:
        void initialize();
        void handleMessage(cMessage* msg);
        void finish();
        ~ScampConnectivityProbe();

    private:
        GlobalStatistics* globalStatistics;
        GlobalNodeList* globalNodesList;

        ScampTopology Topology;
        ScampTopology InTopology;

        simtime_t probeInterval;
        cMessage* probeTimer;

        double avgPartialViewSize;
        double avgInViewSize;
        int leafNodes;

        void resetTopologyNodes();
        void traceViews();

        /**
         * Trace the average size of partialViews and inViews
         */

        void recordInViewsSize();

        /**
         * Mega-method that does the following thisngs:
         *  - writes DAG to file
         *  - counts heartbeats
         *  - computes avgPartialViews size
         *  - counts connected components
         *  - counts initial nodes still in the overlay
         *  - counts leaf nodes
         *  - counts weak components
         */
        void computeData();

        void extractPartialViewTopology();
        void extractInViewTopology();
        unsigned int getComponentSize(OverlayKey key);
        int getWeakComponents(OverlayKey key);
        double R;
        int dag_id;

        // statistics
        cOutVector cOV_NodeCount;
        cOutVector cOV_MaximumComponent;
        cOutVector cOV_AvgConnectivity;
        cOutVector cOV_InitialNodes;
        cOutVector cOV_WeakComponents;
        cOutVector cOV_TotalHeartbeats;
        cOutVector cOV_avgPartialViewSize;
        cOutVector cOV_LeafsNumber;

        std::ofstream outfile;
        std::stringstream ss;
};

#endif
