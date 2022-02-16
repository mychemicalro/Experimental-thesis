/*
 * ReplicaState.h
 *
 *  Created on: 16/feb/2021
 *      Author: DinuFC
 */

#ifndef REPLICASTATE_H_
#define REPLICASTATE_H_

#include <NodeVector.h>
#include <omnetpp.h>
#include <InitStages.h>

/**
 * Class for storing the replica state and the message log, that will have to contain
 * most of the client requests. TODO
 *
 */

class ReplicaState : public cSimpleModule {

public:
    virtual void initialize(int stage);
    virtual void handleMessage(cMessage* msg);

    // don't know what it is
    virtual int numInitStages() const {
        return MAX_STAGE_OVERLAY + 1;
    }

    // initialize parameters and data structure
    virtual void initializeState();

    // Getters
    int getCurrentView(){ return currentView; }

    // and setters
    void setCurrentView(int cv){ currentView = cv; }

protected:

private:
    // Class variables
    int currentView;
    // TODO Message log
};


#endif /* REPLICASTATE_H_ */
