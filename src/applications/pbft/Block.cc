
/*
 * Block.cc
 *
 *  Created on: 14/feb/2021
 *      Author: DinuFC
 */

#include "Block.h"
#include "sha256.h"

Block::Block(int c){
    capacity = c;
    seqNumber = 0;
    EV << "Created block with capacity: " << capacity << endl;
}

Block::Block(){

}


string Block::computeHash() {
    cout << "Call in computeHash()" << endl;
    stringstream ss;

    for(size_t i=0; i<operations.size(); i++){
        ss << operations.at(i).cHash();
    }

    ss << prevBlockHash;

    hash = sha256(ss.str());

    EV << "BLOCK HASH:" << hash << endl;

    return hash;
}

void Block::addOperation(Operation& op){
    EV << "trying to add operation" << op.getHash() << " to block: " << capacity << endl;
    operations.push_back(op);
    EV << "operations size: " << operations.size() << endl;
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
}
