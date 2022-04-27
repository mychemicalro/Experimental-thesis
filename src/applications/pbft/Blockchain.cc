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
    iter = par("iter");
}

void Blockchain::handleMessage(cMessage* msg){
    error("This module doesn't handle messages, it runs only in initialize()");
}

void Blockchain::initializeChain(const OverlayKey* ok) {

    this->overlayk = ok;
    blockchain_length = 0;
    operations_number = 0;
    creation_time = simTime().dbl();

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

    globalStatistics->recordHistogram("Blockchain: Length", blockchain_length);

    // globalStatistics->addStdDev("Blockchain: length", blockchain_length);
    globalStatistics->addStdDev("Blockchain: operations", operations_number);

    printChain();

}

size_t Blockchain::isPresentOp(Operation& op){
    if(DEBUG)
        EV << "[Blockchain::isPresentOp() @ " << *overlayk
           << " Operation: " << op.getHash()<< endl;

    for (size_t i=0; i<blocks.size(); i++){
        // EV << "Block: " << blocks.at(i).getHash() << endl;
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

void Blockchain::printChain(){

    stringstream ss;
    ss << iter << "_" << overlayk->toString();

    // generate external file name
    outfile.open(ss.str().c_str(), std::ios_base::trunc); // append instead of overwrite (app for append)

    outfile << "Creation time: " << creation_time << "\n";
    outfile << "Length: " << blockchain_length << "\n";
    outfile << "Operations: " << operations_number << "\n";
    if(blocks.size() > 0){
        outfile << "Blocks latency: " << blocks.at(blocks.size()-1).getCreationTimestamp() / blockchain_length << "\n";
    }
    // print the blocks data
    for(size_t i=0; i<blocks.size(); i++){
        outfile << blocks.at(i).getHash() << ":" << blocks.at(i).getCreationTimestamp() << ":" << blocks.at(i).getInsertionTimestamp() << ":" << blocks.at(i).getSeqNumber() << "\n";
    }
    outfile << "Shutdown time: " << simTime().dbl() << "\n";

    outfile.close();
}

bool Blockchain::missingBlocks(int seqNum){
    size_t n = blocks.size();
    if(seqNum - blocks.at(n-1).getSeqNumber() > 1){
        return true;
    }

    for(size_t i=0; i<n-1; i++){
        if(blocks.at(i+1).getSeqNumber() - blocks.at(i).getSeqNumber() > 1){
            return true;
        }
    }
    return false;
}



