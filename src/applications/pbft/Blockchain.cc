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

// override <<
/*
std::ostream& operator<<(std::ostream& os, const NodeEntry& e) {
    os << e.nodeHandle;
    return os;
};
*/

void Blockchain::initialize(int stage) {
    // because of IPAddressResolver, we need to wait until interfaces
    // are registered, address auto-assignment takes place etc.
    if(stage != MIN_STAGE_OVERLAY)
        return;
}

void Blockchain::handleMessage(cMessage* msg){
    error("This module doesn't handle messages, it runs only in initialize()");
}

void Blockchain::initializeChain() {
    EV << "Blockchain initialized" << endl;
}

void Blockchain::addBlock(Block& b){
    blocks.push_back(b);
    EV << "Initial block added, hash: " << b.getHash() << endl;
    EV << "Blockchain length: " << blocks.size() << endl;

}

