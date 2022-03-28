/*
 * Blockchain.cc
 *
 *  Created on: 14/feb/2021
 *      Author: DinuFC
 */


#define DEBUG false
#include "Blockchain.h"

using namespace std;

Define_Module(Blockchain);


void Blockchain::initialize(int stage) {
    // because of IPAddressResolver, we need to wait until interfaces
    // are registered, address auto-assignment takes place etc.
    if(stage != MIN_STAGE_OVERLAY)
        return;
}

void Blockchain::handleMessage(cMessage* msg){
    error("This module doesn't handle messages, it runs only in initialize()");
}

void Blockchain::initializeChain(const OverlayKey* ok) {

    this->overlayk = ok;
    blockchain_length = 0;
    operations_number = 0;

    WATCH(blockchain_length);
    WATCH(operations_number);

    globalStatistics = GlobalStatisticsAccess().get();

}

void Blockchain::addBlock(Block& b){
    blocks.push_back(b);
    blockchain_length ++;
    operations_number += b.getCapacity();

    if(DEBUG)
        EV << "Added new block at node: " << *overlayk << " with digest:" << b.getHash() << endl;
}

bool Blockchain::isPresent(Block& b){

    for (size_t i=0; i<blocks.size(); i++){
        if(blocks.at(i).getHash() == b.getHash()){
            return true;
        }
    }

    return false;
}

string Blockchain::getLastBlockHash(){
    return blocks.at(blocks.size()-1).getHash();
}

void Blockchain::finish(){

    if(DEBUG)
        EV << "[Blockchain::finish() @ " << *overlayk
        << " length: " << blockchain_length
        << " last hash: " << getLastBlockHash()
        << " ops: " << operations_number
        << endl;

    // globalStatistics->recordHistogram("Blockchain: Number of clients", 1);

    globalStatistics->addStdDev("Blockchain: length", blockchain_length);
    globalStatistics->addStdDev("Blockchain: operations", operations_number);

}

size_t Blockchain::isPresentOp(Operation& op){
    if(DEBUG)
        EV << "[Blockchain::isPresentOp() @ " << *overlayk
           << " Operation: " << op.getHash()<< endl;

    for (size_t i=0; i<blocks.size(); i++){
        EV << "Block: " << blocks.at(i).getHash() << endl;
        if(blocks.at(i).containsOp(op)){
            if (DEBUG)
                EV << "Operation found" << endl;
            return i;
        }
    }

    if (DEBUG)
        EV << "Operation not found" << endl;
    return -1;
}

Block& Blockchain::getBlockByIndex(size_t i){
    return blocks.at(i);
}

void Blockchain::updateBlockchain(vector<Block> otherBlocks){
    blocks.clear();
    blockchain_length = 0;
    for(size_t i=0; i<otherBlocks.size(); i++){
        blocks.push_back(otherBlocks.at(i));
        blockchain_length ++;
        operations_number += otherBlocks.at(i).getCapacity();
    }

    if(DEBUG)
        EV << "Updated blockchain, size -> " << blocks.size() << endl;
}

vector<Block> Blockchain::getBlocks(){
    vector<Block> res;
    for(size_t i=0; i<blocks.size(); i++){
        res.push_back(blocks.at(i));
    }

    if(DEBUG){
        EV << "Get " << res.size() << " blocks" << endl;
    }
    return res;
}




