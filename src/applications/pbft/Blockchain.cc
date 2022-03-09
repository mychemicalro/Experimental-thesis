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
}

void Blockchain::addBlock(Block& b){
    blocks.push_back(b);
    blockchain_length ++;
    operations_number += b.getCapacity();

    if(DEBUG){
        EV << "Added new block at node: " << *overlayk << " with digest:" << b.getHash() << endl;
/*
        vector<Operation> const & ops = b.getOperations();
        for(size_t i=0; i<ops.size(); i++){
            EV << "Operation hash: " << ops.at(i).cHash() << endl;
        }
*/
        EV << "New blockchain length: " << blocks.size() << endl;
    }
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

    EV << "[Blockchain::finish() @ " << *overlayk
    << " length: " << blockchain_length
    << " last hash: " << getLastBlockHash()
    << " ops: " << operations_number
    << endl;

}



