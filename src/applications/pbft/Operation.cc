
/*
 * Operation.cc
 *
 *  Created on: 16/feb/2021
 *      Author: DinuFC
 */

#include "Operation.h"
#include "sha256.h"

Operation::Operation(OverlayKey ok, IPvXAddress ip, simtime_t ts){
    op = "Ciao";
    timestamp = ts;
    originator = make_pair(ok, ip);
}

//copy constructor
Operation::Operation( const Operation& operation ) {
    op = operation.op;
    timestamp = operation.timestamp;
}

Operation::Operation(){

}

// TODO Adjust the hash computation
const char* Operation::computeHash() {
    stringstream ss;
    ss << op << timestamp << originator.first.toString() << originator.second.str();
    const char* c = sha256(ss.str()).c_str();
    return c;
}

Operation& Operation::getOp(){
    return *this;
}



