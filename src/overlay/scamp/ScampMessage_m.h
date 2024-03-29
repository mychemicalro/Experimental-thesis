//
// Generated file, do not edit! Created by opp_msgc 4.2 from overlay/scamp/ScampMessage.msg.
//

#ifndef _SCAMPMESSAGE_M_H_
#define _SCAMPMESSAGE_M_H_

#include <omnetpp.h>

// opp_msgc version check
#define MSGC_VERSION 0x0402
#if (MSGC_VERSION!=OMNETPP_VERSION)
#    error Version mismatch! Probably this file was generated by an earlier version of opp_msgc: 'make clean' should help.
#endif

// cplusplus {{
#include <IPvXAddress.h>
#include <NodeHandle.h>
#include <TransportAddress.h>
#include <CommonMessages_m.h>
#include <OverlayKey.h>


static const int SCAMPCOMMAND_L = 4;
static const int STEP_L = 8;
static const int SIMTIME_L = 4;

#define JOINCALL_L(msg) (BASECALL_L(msg) + IPADDR_L + SIMTIME_L)
#define JOINRESPONSE_L(msg) (BASERESPONSE_L(msg) + SCAMPCOMMAND_L)

#define SCAMP_MESSAGE_L(msg) (BASECALL_L(msg) + SCAMPCOMMAND_L + NODEHANDLE_L)
#define FORWARD_MESSAGE_L(msg) (SCAMP_MESSAGE_L(msg) + STEP_L + NODEHANDLE_L)
#define LEAVE_MESSAGE_L(msg) (SCAMP_MESSAGE_L(msg) + NODEHANDLE_L)
#define NEW_NODE_L(msg) (SCAMP_MESSAGE_L(msg))
// }}



/**
 * Enum generated from <tt>overlay/scamp/ScampMessage.msg</tt> by opp_msgc.
 * <pre>
 * enum ScampCommand
 * {
 *     JOINREQUESTACCEPTED = 0;
 *     JOINREQUESTFORWARDED = 1;
 *     ADDREQUESTACCEPTED = 2;
 *     ADDREQUESTDENIED = 3;
 *     JOINREQUESTENDED = 4;
 *     ADDEDTOPARTIALVIEW = 5;
 *     HEARTBEAT = 6;
 *     REPLACE_ME = 7;
 *     REMOVE_ME = 8;
 *     NEW_NODE = 9;
 *     HEARTBEAT_FROM_LEAF = 10;
 * }
 * </pre>
 */
enum ScampCommand {
    JOINREQUESTACCEPTED = 0,
    JOINREQUESTFORWARDED = 1,
    ADDREQUESTACCEPTED = 2,
    ADDREQUESTDENIED = 3,
    JOINREQUESTENDED = 4,
    ADDEDTOPARTIALVIEW = 5,
    HEARTBEAT = 6,
    REPLACE_ME = 7,
    REMOVE_ME = 8,
    NEW_NODE = 9,
    HEARTBEAT_FROM_LEAF = 10
};

/**
 * Class generated from <tt>overlay/scamp/ScampMessage.msg</tt> by opp_msgc.
 * <pre>
 * packet ScampMessage extends BaseOverlayMessage{
 *     int command enum(ScampCommand);  
 *     NodeHandle node;  
 * }
 * </pre>
 */
class ScampMessage : public ::BaseOverlayMessage
{
  protected:
    int command_var;
    NodeHandle node_var;

  private:
    void copy(const ScampMessage& other);

  protected:
    // protected and unimplemented operator==(), to prevent accidental usage
    bool operator==(const ScampMessage&);

  public:
    ScampMessage(const char *name=NULL, int kind=0);
    ScampMessage(const ScampMessage& other);
    virtual ~ScampMessage();
    ScampMessage& operator=(const ScampMessage& other);
    virtual ScampMessage *dup() const {return new ScampMessage(*this);}
    virtual void parsimPack(cCommBuffer *b);
    virtual void parsimUnpack(cCommBuffer *b);

    // field getter/setter methods
    virtual int getCommand() const;
    virtual void setCommand(int command);
    virtual NodeHandle& getNode();
    virtual const NodeHandle& getNode() const {return const_cast<ScampMessage*>(this)->getNode();}
    virtual void setNode(const NodeHandle& node);
};

inline void doPacking(cCommBuffer *b, ScampMessage& obj) {obj.parsimPack(b);}
inline void doUnpacking(cCommBuffer *b, ScampMessage& obj) {obj.parsimUnpack(b);}

/**
 * Class generated from <tt>overlay/scamp/ScampMessage.msg</tt> by opp_msgc.
 * <pre>
 * packet ForwardMessage extends ScampMessage{
 * 	int steps;
 * 	NodeHandle sourceNode;
 * }
 * </pre>
 */
class ForwardMessage : public ::ScampMessage
{
  protected:
    int steps_var;
    NodeHandle sourceNode_var;

  private:
    void copy(const ForwardMessage& other);

  protected:
    // protected and unimplemented operator==(), to prevent accidental usage
    bool operator==(const ForwardMessage&);

  public:
    ForwardMessage(const char *name=NULL, int kind=0);
    ForwardMessage(const ForwardMessage& other);
    virtual ~ForwardMessage();
    ForwardMessage& operator=(const ForwardMessage& other);
    virtual ForwardMessage *dup() const {return new ForwardMessage(*this);}
    virtual void parsimPack(cCommBuffer *b);
    virtual void parsimUnpack(cCommBuffer *b);

    // field getter/setter methods
    virtual int getSteps() const;
    virtual void setSteps(int steps);
    virtual NodeHandle& getSourceNode();
    virtual const NodeHandle& getSourceNode() const {return const_cast<ForwardMessage*>(this)->getSourceNode();}
    virtual void setSourceNode(const NodeHandle& sourceNode);
};

inline void doPacking(cCommBuffer *b, ForwardMessage& obj) {obj.parsimPack(b);}
inline void doUnpacking(cCommBuffer *b, ForwardMessage& obj) {obj.parsimUnpack(b);}

/**
 * Class generated from <tt>overlay/scamp/ScampMessage.msg</tt> by opp_msgc.
 * <pre>
 * packet LeaveMessage extends ScampMessage{
 *     
 *     NodeHandle newNode; 
 * }
 * </pre>
 */
class LeaveMessage : public ::ScampMessage
{
  protected:
    NodeHandle newNode_var;

  private:
    void copy(const LeaveMessage& other);

  protected:
    // protected and unimplemented operator==(), to prevent accidental usage
    bool operator==(const LeaveMessage&);

  public:
    LeaveMessage(const char *name=NULL, int kind=0);
    LeaveMessage(const LeaveMessage& other);
    virtual ~LeaveMessage();
    LeaveMessage& operator=(const LeaveMessage& other);
    virtual LeaveMessage *dup() const {return new LeaveMessage(*this);}
    virtual void parsimPack(cCommBuffer *b);
    virtual void parsimUnpack(cCommBuffer *b);

    // field getter/setter methods
    virtual NodeHandle& getNewNode();
    virtual const NodeHandle& getNewNode() const {return const_cast<LeaveMessage*>(this)->getNewNode();}
    virtual void setNewNode(const NodeHandle& newNode);
};

inline void doPacking(cCommBuffer *b, LeaveMessage& obj) {obj.parsimPack(b);}
inline void doUnpacking(cCommBuffer *b, LeaveMessage& obj) {obj.parsimUnpack(b);}

/**
 * Class generated from <tt>overlay/scamp/ScampMessage.msg</tt> by opp_msgc.
 * <pre>
 * packet NewNodeMessage extends ScampMessage{
 * }
 * </pre>
 */
class NewNodeMessage : public ::ScampMessage
{
  protected:

  private:
    void copy(const NewNodeMessage& other);

  protected:
    // protected and unimplemented operator==(), to prevent accidental usage
    bool operator==(const NewNodeMessage&);

  public:
    NewNodeMessage(const char *name=NULL, int kind=0);
    NewNodeMessage(const NewNodeMessage& other);
    virtual ~NewNodeMessage();
    NewNodeMessage& operator=(const NewNodeMessage& other);
    virtual NewNodeMessage *dup() const {return new NewNodeMessage(*this);}
    virtual void parsimPack(cCommBuffer *b);
    virtual void parsimUnpack(cCommBuffer *b);

    // field getter/setter methods
};

inline void doPacking(cCommBuffer *b, NewNodeMessage& obj) {obj.parsimPack(b);}
inline void doUnpacking(cCommBuffer *b, NewNodeMessage& obj) {obj.parsimUnpack(b);}

/**
 * Class generated from <tt>overlay/scamp/ScampMessage.msg</tt> by opp_msgc.
 * <pre>
 * packet ScampJoinCall extends BaseCallMessage{
 *     IPvXAddress address;
 *     simtime_t time;
 * }
 * </pre>
 */
class ScampJoinCall : public ::BaseCallMessage
{
  protected:
    IPvXAddress address_var;
    simtime_t time_var;

  private:
    void copy(const ScampJoinCall& other);

  protected:
    // protected and unimplemented operator==(), to prevent accidental usage
    bool operator==(const ScampJoinCall&);

  public:
    ScampJoinCall(const char *name=NULL, int kind=0);
    ScampJoinCall(const ScampJoinCall& other);
    virtual ~ScampJoinCall();
    ScampJoinCall& operator=(const ScampJoinCall& other);
    virtual ScampJoinCall *dup() const {return new ScampJoinCall(*this);}
    virtual void parsimPack(cCommBuffer *b);
    virtual void parsimUnpack(cCommBuffer *b);

    // field getter/setter methods
    virtual IPvXAddress& getAddress();
    virtual const IPvXAddress& getAddress() const {return const_cast<ScampJoinCall*>(this)->getAddress();}
    virtual void setAddress(const IPvXAddress& address);
    virtual simtime_t getTime() const;
    virtual void setTime(simtime_t time);
};

inline void doPacking(cCommBuffer *b, ScampJoinCall& obj) {obj.parsimPack(b);}
inline void doUnpacking(cCommBuffer *b, ScampJoinCall& obj) {obj.parsimUnpack(b);}

/**
 * Class generated from <tt>overlay/scamp/ScampMessage.msg</tt> by opp_msgc.
 * <pre>
 * packet ScampJoinResponse extends BaseResponseMessage{
 *     int command enum(ScampCommand);
 * }
 * </pre>
 */
class ScampJoinResponse : public ::BaseResponseMessage
{
  protected:
    int command_var;

  private:
    void copy(const ScampJoinResponse& other);

  protected:
    // protected and unimplemented operator==(), to prevent accidental usage
    bool operator==(const ScampJoinResponse&);

  public:
    ScampJoinResponse(const char *name=NULL, int kind=0);
    ScampJoinResponse(const ScampJoinResponse& other);
    virtual ~ScampJoinResponse();
    ScampJoinResponse& operator=(const ScampJoinResponse& other);
    virtual ScampJoinResponse *dup() const {return new ScampJoinResponse(*this);}
    virtual void parsimPack(cCommBuffer *b);
    virtual void parsimUnpack(cCommBuffer *b);

    // field getter/setter methods
    virtual int getCommand() const;
    virtual void setCommand(int command);
};

inline void doPacking(cCommBuffer *b, ScampJoinResponse& obj) {obj.parsimPack(b);}
inline void doUnpacking(cCommBuffer *b, ScampJoinResponse& obj) {obj.parsimUnpack(b);}


#endif // _SCAMPMESSAGE_M_H_
