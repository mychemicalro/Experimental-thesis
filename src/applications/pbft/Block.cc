
/*
 * Block.cc
 *
 *  Created on: 14/feb/2021
 *      Author: DinuFC
 */

#include "Block.h"
#include "sha256.h"

#define DEBUG true

Block::Block(int c){
    stringstream ss;
    ss << "pre_hash";
    prevBlockHash = sha256(ss.str());

    capacity = c;
    seqNumber = 0;
    creationTimestamp = simTime().dbl();
    insertionTimestamp = 0;
    hash = computeHash(); // -> initial hash, won't be the final one.
}

Block::Block(){}


string Block::computeHash() {

    stringstream ss;

    for(size_t i=0; i<operations.size(); i++){
        ss << operations.at(i).cHash();
    }

    ss << prevBlockHash << creationTimestamp;
    hash = sha256(ss.str());

    return hash;
}

void Block::addOperation(Operation& op){

    if(DEBUG)
        EV << "Trying to add operation: " << op.getHash() << " to block: " << this->hash << endl;

    if(!containsOp(op)){
        operations.push_back(op);
        computeHash();
    }
}

bool Block::isFull(){

    if(operations.size() == capacity){
        return true;
    }

    return false;
}

vector<Operation> Block::getOperations() const{
    vector<Operation> res;
    for(size_t i=0; i<operations.size(); i++){
        Operation o = operations.at(i);
        res.push_back(o);
    }

    return res;
}

Block::Block( const Block& block ){
    capacity = block.capacity;
    seqNumber = block.seqNumber;
    hash = block.hash;
    prevBlockHash = block.prevBlockHash;
    operations = block.operations;
    creationTimestamp = block.creationTimestamp;
    insertionTimestamp = block.insertionTimestamp;
}

bool Block::containsOp(Operation& op){
    for(size_t i=0; i<operations.size(); i++){
        if(op.getHash() == operations.at(i).getHash()){
            return true;
        }
    }
    return false;
}

vector<Operation> Block::getOpsByCreator(OverlayKey creator){
    vector<Operation> res;

    for(size_t i=0; i<operations.size(); i++){
        if (operations.at(i).getOriginatorKey() == creator){
            res.push_back(operations.at(i));
        }
    }

    if (DEBUG)
        EV << "Operations found by creator in this block: " << res.size() << endl;

    return res;
}
