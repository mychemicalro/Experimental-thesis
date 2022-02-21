/*
 * View.h
 *
 *  Created on: 14/feb/2021
 *      Author: DinuFC
 */

#ifndef BLOCKCHAIN_H_
#define BLOCKCHAIN_H_

#include <NodeVector.h>
#include <omnetpp.h>
#include <map>
#include <InitStages.h>
#include "Block.h"

// class BaseOverlay;

/**
 * This class must store a node's blockchain.
 */

class Blockchain : public cSimpleModule {

public:
    virtual void initialize(int stage);
    virtual void handleMessage(cMessage* msg);

    // don't know what it is
    virtual int numInitStages() const {
        return MAX_STAGE_OVERLAY + 1;
    }

    // initialize parameters and data structure
    virtual void initializeChain();

    /**
     * Adds block to the blockchain
     */
    void addBlock(Block& b);

    /**
     * Checks if block is present in the blockchain
     */
    bool isPresent(Block& b);

    /**
     * Get lash block hash
     */
    string getLastBlockHash();


protected:

private:
    vector<Block> blocks;

};


#endif /* BLOCKCHAIN_H_ */
