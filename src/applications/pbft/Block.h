
#ifndef BLOCK_H_
#define BLOCK_H_

#include <iostream>
#include <sstream>
#include <vector>

#include "Operation.h"

using namespace std;

/**
 * This class must represent a block in the blockchain.
 * A block has a max capacity, that is the number of operations contained.
 * It must have a hash, a prevhash, and a list of operations.
 * Also, the block must refer to a view number and a seqNum.
 *
 * It must be able to compute its hash.
 */

class Block {

    public:


        /**
         * Constructor gets the capacity of the block.
         */
        Block(int c);

        //copy constructor
        Block( const Block& block );
        Block();


        // Getters
        int getCapacity(){return capacity;}
        string getHash(){return hash;}
        int getSeqNumber(){return seqNumber;}
        size_t getBlockOpsNumber(){return operations.size();}

        //Setters
        void setSeqNumber(int sn){seqNumber = sn;}
        void setViewNumber(int vn){viewNumber = vn;}
        void setPrevBlockHash(string h){prevBlockHash = h;}

        // Others
        /**
         * Returns true if the block has no more space for other operations.
         */
        bool isFull();

        /**
         * Gets the block's operations
         */
        vector<Operation> getOperations() const;

        /**
         * Adds an operation to the block
         */
        void addOperation(Operation& op);

        /**
         * Concats the hashes of all operations, then adds the previous block hash, then sha256 hash
         */
        string computeHash();

    private:
        string hash; // the block's hash
        string prevBlockHash; // the hash of the previous block appended in the blockchain

        int capacity; // the block's max capacity
        int seqNumber; // the sequence number assigned to this block by the primary
        int viewNumber; // view number

        // The block must contain a list of <capacity> operations
        vector<Operation> operations;

};

#endif
