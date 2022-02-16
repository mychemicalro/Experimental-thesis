
/*
 * Operation.cc
 *
 *  Created on: 16/feb/2021
 *      Author: DinuFC
 */

#include "Operation.h"

Operation::Operation(simtime_t ts){
    op = "Ciao";
    timestamp = ts;
}

//copy constructor
Operation::Operation( const Operation& handle ) {
    op = handle.op;
    timestamp = handle.timestamp;
}
Operation::Operation(){

}
