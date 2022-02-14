
/*
 * Block.cc
 *
 *  Created on: 30/nov/2021
 *      Author: DinuFC
 */

#include "Block.h"

Block::Block(){
    hash = computeHash();
}


string Block::computeHash() const {
    cout << "Call in computeHash()" << endl;
    return "ciao";
}
