/*
 * ReplicaState.cc
 *
 *  Created on: 16/feb/2021
 *      Author: DinuFC
 */


#define DEBUG true
#include "ReplicaState.h"

using namespace std;

Define_Module(ReplicaState);


Checkpoint::Checkpoint(int sn, string d){
    this->sn = sn;
    this->proof = false;
    this->digest = d;
}

/*
View* ScampTopologyNode::getView() const {
    return check_and_cast<View*>(simulation.getModule(viewID));
}
*/




void ReplicaState::initialize(int stage) {
    // because of IPAddressResolver, we need to wait until interfaces
    // are registered, address auto-assignment takes place etc.
    if(stage != MIN_STAGE_OVERLAY)
        return;
}

void ReplicaState::handleMessage(cMessage* msg){
    error("This module doesn't handle messages, it runs only in initialize()");
}

void ReplicaState::initializeState(const OverlayKey* ok) {


    currentView = 0;
    primary = false;
    requests.clear();
    preprepares.clear();
    prepares.clear();
    commits.clear();
    replies.clear();
    checkpoints.clear();
    candidateBlocks.clear();
    timestamps.clear();
    checkpoints_data.clear();
    clientRequests.clear();

    f = par("f");
    nodesNumber = 3*f + 1;

    this->overlayk = ok;

    WATCH(primary);
    WATCH(currentView);
    WATCH(f);

    if(DEBUG){
        EV << "ReplicaState initialized" << endl;
        EV << "Replica with overlay key: " << *overlayk << endl;
    }

    // TODO
    // WATCH_VECTOR(requests);
    // WATCH_VECTOR(preprepares);
    // WATCH_VECTOR(prepares);

}

void ReplicaState::addToRequestsLog(PBFTRequestMessage* msg){

    if (seenMessage(msg)) return;
    requests.push_back(*msg);

    if(DEBUG)
        EV << "Added a PBFTRequestMessage" << endl;
}

bool ReplicaState::digestInRequestsLog(const char* digest){

    for(size_t i=0; i<requests.size(); i++){
        if (strcmp(requests.at(i).getOp().getHash().c_str(), digest) == 0){
            if(DEBUG)
                EV << "Digest found - returning true" << endl;
            return true;
        }
    }

    if(DEBUG)
        EV << "Digest not found - returning false" << endl;
    return false;
}


void ReplicaState::addToPrepreparesLog(PBFTPreprepareMessage* msg){

    if (seenMessage(msg)) return;
    preprepares.push_back(*msg);

    if(DEBUG)
        EV << "Added a PBFTPreprepareMessage with block hash: " << msg->getBlock().getHash() << endl;

}

void ReplicaState::addToPreparesLog(PBFTPrepareMessage* msg){

    if (seenMessage(msg)) return;
    prepares.push_back(*msg);

    if(DEBUG)
        EV << "Added a PBFTPrepareMessage" << endl;

}

void ReplicaState::addToCommitsLog(PBFTCommitMessage* msg){

    if (seenMessage(msg)) return;
    commits.push_back(*msg);

    if(DEBUG)
        EV << "Added a PBFTCommitMessage to plain vector with creatorkey: " << msg->getCreatorKey() << endl;
}

void ReplicaState::addToRepliesLog(PBFTReplyMessage* msg){

    if (seenMessage(msg)) return;
    replies.push_back(*msg);

    if(DEBUG)
        EV << "Added a PBFTReplyMessage to plain vector with block hash: " << msg->getBlock().getHash() << endl;
}

void ReplicaState::addToCheckpointsLog(PBFTCheckpointMessage* msg){
    if (seenMessage(msg)) return;
    checkpoints.push_back(*msg);

    // If there is not a Checkpoint object in checkpoints_data, create and add it
    map<int, Checkpoint>::iterator mi = checkpoints_data.find(msg->getSeqNumber());
    if(mi == checkpoints_data.end()){
        Checkpoint chkp(msg->getSeqNumber(), msg->getDigest());
        checkpoints_data.insert(make_pair(msg->getSeqNumber(), chkp));
    }
    if(DEBUG)
        EV << "Added a PBFTCheckpointMessage to plain vector" << endl;
        EV << "Checkpoints data len: " << checkpoints_data.size() << endl;
}

bool ReplicaState::seenMessage(cMessage* msg){

    if (dynamic_cast<PBFTRequestMessage*>(msg)){
        PBFTRequestMessage *myMsg = dynamic_cast<PBFTRequestMessage*>(msg);

        for(size_t i=0; i<requests.size(); i++){
            if (requests.at(i).getOp().getHash() == myMsg->getOp().getHash()){
                if (requests.at(i).getRetryNumber() == myMsg->getRetryNumber()){
                    if(DEBUG)
                        EV << "Request found - returning true" << endl;
                    return true;
                }
            }
        }
    } else if (dynamic_cast<PBFTPreprepareMessage*>(msg)){
        PBFTPreprepareMessage *myMsg = dynamic_cast<PBFTPreprepareMessage*>(msg);

        for(size_t i=0; i<preprepares.size(); i++){

            if (strcmp(preprepares.at(i).getDigest(), myMsg->getDigest()) == 0){
                return true;
            }
        }
    } else if (dynamic_cast<PBFTPrepareMessage*>(msg)){
        PBFTPrepareMessage *myMsg = dynamic_cast<PBFTPrepareMessage*>(msg);

        /**
         * This is different from the others. I will have many PREPARE messages for the same request.
         * I need to be able to count these messages, for each request.
         * Messages are equal if:
         *      - same digest
         *      - same sender
         */
        for(size_t i=0; i<prepares.size(); i++){
            if (strcmp(prepares.at(i).getDigest(), myMsg->getDigest()) == 0 && prepares.at(i).getCreatorKey() == myMsg->getCreatorKey()){
                return true;
            }
        }
    } else if (dynamic_cast<PBFTCommitMessage*>(msg)){
        PBFTCommitMessage *myMsg = dynamic_cast<PBFTCommitMessage*>(msg);

        for(size_t i=0; i<commits.size(); i++){
            if (strcmp(commits.at(i).getDigest(), myMsg->getDigest()) == 0 && commits.at(i).getCreatorKey() == myMsg->getCreatorKey()){
                if (commits.at(i).getSeqNumber() == myMsg->getSeqNumber() && commits.at(i).getView() == myMsg->getView()){
                    return true;
                }
            }
        }
    } else if (dynamic_cast<PBFTReplyMessage*>(msg)){
        PBFTReplyMessage* myMsg = dynamic_cast<PBFTReplyMessage*>(msg);

        for(size_t i=0; i<replies.size(); i++){
                if(replies.at(i).getView() == myMsg->getView()){
                        if(replies.at(i).getReplicaNumber() == myMsg->getReplicaNumber()){
                                if(replies.at(i).getRetryNumber() == myMsg->getRetryNumber()){
                                    if(replies.at(i).getBlock().getHash() == myMsg->getBlock().getHash()){
                                        return true;
                                    }
                                }
                        }
                }
        }
    } else if (dynamic_cast<PBFTCheckpointMessage*>(msg)){
        PBFTCheckpointMessage *myMsg = dynamic_cast<PBFTCheckpointMessage*>(msg);
        for(size_t i=0; i<checkpoints.size(); i++){
            if(checkpoints.at(i).getSeqNumber() == myMsg->getSeqNumber()){
                if(checkpoints.at(i).getCreatorKey() == myMsg->getCreatorKey()){
                    return true;
                }
            }
        }
    }

    if(DEBUG)
        EV << "Message not found - returning false" << endl;

    return false;
}


bool ReplicaState::searchPreparedCertificate(PBFTPrepareMessage* m){
    int prepare_c = 0;
    int preprepare_c = 0;
    bool minePresent = false;  // Not specified in the paper if needed.

    for(size_t i=0; i<prepares.size(); i++){
        if(strcmp(prepares.at(i).getDigest(), m->getDigest()) == 0){
            if(prepares.at(i).getSeqNumber() == m->getSeqNumber() && prepares.at(i).getView() == m->getView()){
                prepare_c ++;
                if(prepares.at(i).getCreatorKey() == *overlayk){
                    minePresent = true;
                }
            }
        }
    }

    for(size_t i=0; i<preprepares.size(); i++){
        if(strcmp(preprepares.at(i).getDigest(), m->getDigest()) == 0){
            if(preprepares.at(i).getSeqNumber() == m->getSeqNumber() && preprepares.at(i).getView() == m->getView()){
                preprepare_c ++;
            }
        }
    }

    if (prepare_c >= 2*f && preprepare_c > 0/* && minePresent*/){
        EV << "Found Prepared Certificate! " << endl;
        return true;
    }

    return false;
}


bool ReplicaState::searchCommittedCertificate(PBFTCommitMessage* m){
    int commits_c = 0;
    bool minePresent = false;

    for(size_t i=0; i<commits.size(); i++){
        if(strcmp(commits.at(i).getDigest(), m->getDigest()) == 0){
            if(commits.at(i).getSeqNumber() == m->getSeqNumber() && commits.at(i).getView() == m->getView()){
                commits_c ++;
                if(commits.at(i).getCreatorKey() == *overlayk){
                    minePresent = true;
                }
            }
        }
    }

    if (commits_c >= 2*f +1 && minePresent){
        return true;
    }

    return false;
}


bool ReplicaState::otherPreprepareAccepted(PBFTPreprepareMessage* msg){
    // TODO Very strange method ...
    for(size_t i=0; i<preprepares.size(); i++){
        if (preprepares.at(i).getSeqNumber() == msg->getSeqNumber() && preprepares.at(i).getView() == msg->getView()){
            if(strcmp(preprepares.at(i).getDigest(), msg->getDigest()) != 0){
                return true;
            }
        }
    }

    return false;
}

bool ReplicaState::searchReplyCertificate(PBFTReplyMessage* msg){
    int replies_c = 0;
    for(size_t i=0; i<replies.size(); i++){
        if(replies.at(i).getBlock().getHash() == msg->getBlock().getHash()){
            replies_c ++;
        }
    }

    if (replies_c >= f + 1){
        return true;
    }

    return false;
}


void ReplicaState::addCandidateBlock(PBFTPreprepareMessage* preprep){
    if(DEBUG)
        EV << "candidates block length: " << candidateBlocks.size() << endl;

    candidateBlocks.insert(make_pair(preprep->getBlock().getHash(), preprep->getBlock()));

    if(DEBUG){
        EV << "Added a new candidate block." << endl;
        EV << "candidates block length: " << candidateBlocks.size() << endl;
    }
}

size_t ReplicaState::getCandidateBlocksLength(){
    return candidateBlocks.size();
}

void ReplicaState::addTimestamp(PBFTPreprepareMessage* preprep){
    map<string, double>::iterator it = timestamps.find(preprep->getBlock().getHash());
    if (it != timestamps.end()){
        if(DEBUG)
            EV << "Timestamp already added for block: " << preprep->getBlock().getHash() << endl;

    } else {
        timestamps.insert(make_pair(preprep->getBlock().getHash(), simTime().dbl()));
        if(DEBUG)
            EV << "Timestamp added for block: " << preprep->getBlock().getHash() << endl;
    }

}

double ReplicaState::getTimestamp(string digest){
    if(DEBUG)
        EV << "Get timestamp for digest: " << digest << endl;

    map<string, double>::iterator it = timestamps.find(digest);
    return it->second;
}

bool ReplicaState::checkIfCanPrepare(PBFTRequestMessage* msg){
    if(DEBUG)
        EV << "Check if there are new candidate blocks for which I can send a PREPARE message." << endl;

    // find the block with this operation. If I have the block, check if I have all the operations locally. If yes, return true
    bool canPrepare = false;
    for(size_t i=0; i<preprepares.size(); i++){

        if(operationPrepPrepared(msg->getOp())){
            // The replica has a preprepare message with this operation.
            canPrepare = true;
            vector<Operation> const & ops = preprepares.at(i).getBlock().getOperations();

            for(size_t j=0; j<ops.size(); j++){
                if(!digestInRequestsLog(ops.at(j).cHash().c_str())){
                    canPrepare = false;
                    if(DEBUG)
                        EV << "Operation: " << ops.at(j).cHash().c_str() << " not received by this node" << endl;
                }
            }
        }
    }

    return canPrepare;
}


bool ReplicaState::isPresentCandidateBlock(string dig){

    map<string, Block>::iterator it = candidateBlocks.find(dig);

    if (it != candidateBlocks.end()){
        if(DEBUG)
            EV << "Candidate block found" << endl;
        return true;
    }
    if(DEBUG)
        EV << "Candidate block not found! " << endl;

    return false;
}

map<string,Block> ReplicaState::getCandidateBlocks(){
    return candidateBlocks;
}

Block& ReplicaState::getBlock(const char* digest){
    map<string, Block>::iterator it = candidateBlocks.find(digest);
    if (it != candidateBlocks.end()){
        return it->second;
    }
}


PBFTPreprepareMessage& ReplicaState::getPreprepareMessage(Operation& op){
    // If I do not have the preprepare? Safe because I check first if I have a block with the operation.
    for(size_t i=0; i<preprepares.size(); i++){
        if(preprepares.at(i).getBlock().containsOp(op)){
            return preprepares.at(i);
        }
    }
}

bool ReplicaState::operationPrepPrepared(Operation& op){
    for(size_t i=0; i<preprepares.size(); i++){
        if(preprepares.at(i).getBlock().containsOp(op)){
            return true;
        }
    }

    return false;
}

vector<PBFTCommitMessage> ReplicaState::getCommitMessages(int sn){
    vector<PBFTCommitMessage> res;
    for(size_t i=0; i<commits.size(); i++){
        if(commits.at(i).getSeqNumber() > sn){
            res.push_back(commits.at(i));
        }
    }
    return res;
}

void ReplicaState::clearDataStructures(){

    requests.clear();
    preprepares.clear();
    prepares.clear();
    commits.clear();
    replies.clear();
    candidateBlocks.clear();
    timestamps.clear();
    checkpoints.clear();
    checkpoints_data.clear();
    clientRequests.clear();

}

void ReplicaState::throwGarbage(int sn){

    // TODO Can I delete also requests? -> 30% less messages in omnet
    vector <PBFTRequestMessage>::iterator mir;
    for(mir = requests.begin(); mir != requests.end();){
        if(mir->getOp().getTimestamp() < simTime() - 15){ // TODO It was 50
            mir = requests.erase(mir);
        }
        else{
            mir++;
        }
    }

    vector <PBFTPreprepareMessage>::iterator mit;
    for(mit = preprepares.begin(); mit != preprepares.end();){
        if(mit->getSeqNumber() < sn){
            mit = preprepares.erase(mit);
        }
        else{
            mit++;
        }
    }

    vector <PBFTPrepareMessage>::iterator mitt;
    for(mitt = prepares.begin(); mitt != prepares.end();){
        if(mitt->getSeqNumber() < sn){
            mitt = prepares.erase(mitt);
        }
        else{
            mitt++;
        }
    }

    std::map<string, double>::iterator tmi;
    std::map<string, Block>::iterator cmi;

    vector <PBFTCommitMessage>::iterator mittt;
    for(mittt = commits.begin(); mittt != commits.end();){
        if(mittt->getSeqNumber() < sn){
            tmi = timestamps.find(mittt->getDigest());
            if(tmi != timestamps.end()){
                timestamps.erase(tmi);
            }
            cmi = candidateBlocks.find(mittt->getDigest());
            if(cmi != candidateBlocks.end()){
                // TODO Do not delete for now -> problems
                // candidateBlocks.erase(cmi);
            }
            mittt = commits.erase(mittt);
        }
        else{
            mittt++;
        }
    }

    // TODO Check if I can delete replies -> 80% less messages in omnet
    vector <PBFTReplyMessage>::iterator mirr;
    for(mirr = replies.begin(); mirr != replies.end();){
        if(mirr->getBlock().getSeqNumber() < sn){
            mirr = replies.erase(mirr);
        }
        else{
            mirr++;
        }
    }
}


void ReplicaState::checkpointProcedure(int sn){
    int chkp_count = 0;
    for(size_t i=0; i<checkpoints.size(); i++){
        if(checkpoints.at(i).getSeqNumber() == sn){
            chkp_count++;
        }
    }

    if (chkp_count >= 2*f+1){
        if(DEBUG)
            EV << "Found stable checkpoint for sn: " << sn << endl;

        // If the checkpoint with sn is stable, then throw some data away.
        map<int, Checkpoint>::iterator mi = checkpoints_data.find(sn);

        if(mi != checkpoints_data.end()){
            if(mi->second.proof == false){
                if(DEBUG)
                    EV << "Throwing garbage" << endl;
                throwGarbage(sn);
            }

            mi->second.proof = true;
            // TODO Delete previous checkpoints

        }

    } else {
        if(DEBUG)
            EV << "Checkpoint for sn: " << sn << " still not stable"<< endl;
    }

}

void ReplicaState::deleteRequestFromClientRequests(Operation& op){

    vector <PBFTRequestMessage>::iterator reqs;
    for(reqs = clientRequests.begin(); reqs != clientRequests.end();){
        if(op.getHash() == reqs->getOp().getHash()){
            reqs = clientRequests.erase(reqs);

            if(DEBUG)
                EV << "Deleted request from client requests" << endl;

        } else {
            reqs++;
        }
    }
}

vector<PBFTRequestMessage> ReplicaState::getClientRequests(){
    vector<PBFTRequestMessage> res;
    for(size_t i=0; i<clientRequests.size(); i++){
        res.push_back(clientRequests.at(i));
    }
    return res;
}

void ReplicaState::addClientRequest(PBFTRequestMessage* req){
    clientRequests.push_back(*req);
}

int ReplicaState::getClientRequestSize(){
    return clientRequests.size();
}

