#ifndef _SCAMP_
#define _SCAMP_

#include "BaseOverlay.h"
#include "ScampMessage_m.h"
#include <fstream>

class View;


class Scamp : public BaseOverlay {
  public:
    Scamp();
    virtual ~Scamp();

    // Routing parameters
    int myKey;               // our overlay key
    NodeHandle prevNode;     // next node in chain
    NodeHandle nextNode;     // previous node in chain

    //module parameters
    double dropChance;       // we'll store the "dropChance" parameter here


    int joinRequestCopies;
    bool useCwhenLeaving;

    // overlay routines
    void initializeOverlay(int stage);              // called when the overlay is being initialized
    void setOwnNodeID();                            // (optional) called to set the key of this node (random otherwise)
    void joinOverlay();                             // called when the node is ready to join the overlay
    void finishOverlay();                           // called when the module is about to be destroyed

    // obligatory: called when we need the next hop to route a packet to the given key
    NodeVector* findNode(const OverlayKey& key,     // key to route to
                         int numRedundantNodes,     // next hop candidates to return if we're not responsible for "key"
                         int numSiblings,           // how many siblings to return if we're responsible for "key"
                         BaseOverlayMessage* msg);  // message being routed

    // obligatory: In general, called when we need to know whether "node" is amongst numSiblings closest nodes to "key".
    // But normally it is called with "node" set to "thisNode", and asking whether we are responsible for "key"
    bool isSiblingFor(const NodeHandle& node,       // which node (usually thisNode) we're referring to
                      const OverlayKey& key,        // key in question
                      int numSiblings,              // how many siblings we're querying about
                      bool* err);                   // set to false when we couldn't determine the range

    // obligatory: Set the maximum number of siblings that can be queried about in isSiblingFor(usually 1)
    int getMaxNumSiblings();

    // obligatory: Set the maximum number of redundant nodes that can be queried about in isSiblingFor (usually 1)
    int getMaxNumRedundantNodes();

    virtual void handleUDPMessage(BaseOverlayMessage* msg);

    virtual int getHeartbeatsSentLastPeriod();

    virtual void resetHeartbeatsSentLastPeriod();

    virtual bool inViewEmpty();
    virtual bool partialViewEmpty();

    /**
     * Method called by MyChurn that sets the lease for this node.
     * It will schedule a timer that will make it rejoin through a random
     * node from its partialView. The partialView will stay intact. What about the inView?
     */
    virtual void setLease(NodeHandle& node, double tm);

    /**
     * Returns true if the lease operations of this node are completed.
     * The lease operations are: remove the nodes from its partialView and rejoin
     */
    virtual bool completedLease();

    /**
     * Set the timer for the rejoin after the leasePeriod
     */
    virtual void rejoin(double tm);

    /**
     * Returns ready if state==READY
     */
    virtual bool isReady();

    /**
     * Returns true if overlay is a leave
     */
    virtual bool isLeaf();



  protected:
    double joinDelay; /**< */
    double heartbeatDelay; /**< */
    double pruneDelay;
    double heartbeatTimeout; /**< */

    int joinRetry;/**< how many times a node can try to join the overlay*/
    int joinForwardSteps; /**< how many times a join request can be forwarded through the network*/

    // module references
    View* inViewModule; /**< pointer to this node's finger table */
    View* partialViewModule; /**< pointer to this node's successor list */
    View* seenNodesModule; /**< pointer to the list of all nodes seen by this node */

    TransportAddress bootstrapNode; /**< node used to bootstrap */

    // timer messages
    cMessage* join_timer; /**< Timer to trigger the join action */
    cMessage* heartbeat_timer; /**< Timer to trigger the ImAlive messages */
    cMessage* pruneInView_timer;
    cMessage* rejoinTimer;

    bool leasing;

    bool useHeartbeats;

    // statistics stuff
    // cOutVector joinCallsReceived;
    int totalHeartbeatsSent;
    int heartbeatsSentLastPeriod;

    std::ofstream outfile;


    /**
     * handle a expired join timer
     *
     * @param msg the timer self-message
     */
    virtual void handleTimerEvent(cMessage* msg);

    /**
     * handle a expired join timer
     *
     * @param msg the timer self-message
     */
    virtual void handleJoinTimerExpired(cMessage* msg);

    virtual void handleHeartbeatTimerExpired(cMessage* msg);

    virtual void handlePruneInViewTimerExpired(cMessage* msg);

    /**
     * Handles an RPC Join Request.
     * With probability 1/(1+size(partialView)) it will add the requester to its partialView,
     * if not present already, and tells the requester the positive outcome.
     * If the request is discarded, the request must be forwarded to a random node of the partialView.
     *
     * Furthermore, if this is the contact node, it will broadcast the requester ID to
     * all nodes in its partialView, will create c copies of the request and send
     * them to c random nodes in the partialView.
     *
     *
     * @param call RPC Parameter Message
     */
    virtual void rpcJoin(ScampJoinCall* call);


    virtual bool handleRpcCall(BaseCallMessage* msg);

    // see BaseOverlay.h
    virtual void handleRpcResponse(BaseResponseMessage* msg,
                                   cPolymorphic* context, int rpcId,
                                   simtime_t rtt);

    /**
     * Handles the response for a join request.
     *
     */
    virtual void handleRpcJoinResponse(ScampJoinResponse* joinResponse);

    /**
     * Assigns the inView and partialView to our reference
     */
    virtual void findFriendModules();

    /**
     * initializes finger table and successor list
     */
    virtual void initializeFriendModules();

    /**
     * changes node state
     *
     * @param toState state to change to
     */
    virtual void changeState(int toState);

    /**
     * Forwards a join request to a random node different from the sender
     * @param request
     */
    virtual void forwardJoinRequest(NodeHandle sender, NodeHandle requester, int steps, int howMany = 1, bool useRequester = false);

    virtual void handlePartialViewAdd(ScampMessage* m);

    virtual void handleForwardedJoinRequest(ForwardMessage* m);

    virtual void handleHeartbeatMessage(ScampMessage* m);

    virtual void handleRemoveMeMessage(LeaveMessage* m);

    virtual void handleReplaceMeMessage(LeaveMessage* m);


    /**
     * Sends LeaveMessages to some nodes in its inView,
     * telling them to replace this node with some nodes
     * from this partialView. Other nodes in this inView will
     * delete this node from their partialView.
     */
    virtual void gracefulLeave();

    /**
     * Return up to k random nodes from the partialView
     */
    virtual NodeVector* neighborSet(int k);

    /**
     * Handle message from app through sendDirect
     * TODO maybe not need virtual
     *
     */
    void handleAppMessage(cMessage* msg);

    virtual void handleNodeGracefulLeaveNotification();
    virtual void route(const OverlayKey& key, CompType destComp,
                       CompType srcComp, cPacket* msg,
                       const std::vector<TransportAddress>& sourceRoute
                           = TransportAddress::UNSPECIFIED_NODES,
                       RoutingType routingType = DEFAULT_ROUTING);


    friend class View;


  };

  #endif
