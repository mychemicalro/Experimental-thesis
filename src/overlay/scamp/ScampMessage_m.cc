//
// Generated file, do not edit! Created by opp_msgc 4.2 from overlay/scamp/ScampMessage.msg.
//

// Disable warnings about unused variables, empty switch stmts, etc:
#ifdef _MSC_VER
#  pragma warning(disable:4101)
#  pragma warning(disable:4065)
#endif

#include <iostream>
#include <sstream>
#include "ScampMessage_m.h"

// Template rule which fires if a struct or class doesn't have operator<<
template<typename T>
std::ostream& operator<<(std::ostream& out,const T&) {return out;}

// Another default rule (prevents compiler from choosing base class' doPacking())
template<typename T>
void doPacking(cCommBuffer *, T& t) {
    throw cRuntimeError("Parsim error: no doPacking() function for type %s or its base class (check .msg and _m.cc/h files!)",opp_typename(typeid(t)));
}

template<typename T>
void doUnpacking(cCommBuffer *, T& t) {
    throw cRuntimeError("Parsim error: no doUnpacking() function for type %s or its base class (check .msg and _m.cc/h files!)",opp_typename(typeid(t)));
}




EXECUTE_ON_STARTUP(
    cEnum *e = cEnum::find("ScampCommand");
    if (!e) enums.getInstance()->add(e = new cEnum("ScampCommand"));
    e->insert(JOINREQUESTACCEPTED, "JOINREQUESTACCEPTED");
    e->insert(JOINREQUESTFORWARDED, "JOINREQUESTFORWARDED");
    e->insert(ADDREQUESTACCEPTED, "ADDREQUESTACCEPTED");
    e->insert(ADDREQUESTDENIED, "ADDREQUESTDENIED");
    e->insert(JOINREQUESTENDED, "JOINREQUESTENDED");
    e->insert(ADDEDTOPARTIALVIEW, "ADDEDTOPARTIALVIEW");
    e->insert(HEARTBEAT, "HEARTBEAT");
    e->insert(REPLACE_ME, "REPLACE_ME");
    e->insert(REMOVE_ME, "REMOVE_ME");
    e->insert(NEW_NODE, "NEW_NODE");
    e->insert(HEARTBEAT_FROM_LEAF, "HEARTBEAT_FROM_LEAF");
);

Register_Class(ScampMessage);

ScampMessage::ScampMessage(const char *name, int kind) : BaseOverlayMessage(name,kind)
{
    this->command_var = 0;
}

ScampMessage::ScampMessage(const ScampMessage& other) : BaseOverlayMessage(other)
{
    copy(other);
}

ScampMessage::~ScampMessage()
{
}

ScampMessage& ScampMessage::operator=(const ScampMessage& other)
{
    if (this==&other) return *this;
    BaseOverlayMessage::operator=(other);
    copy(other);
    return *this;
}

void ScampMessage::copy(const ScampMessage& other)
{
    this->command_var = other.command_var;
    this->node_var = other.node_var;
}

void ScampMessage::parsimPack(cCommBuffer *b)
{
    BaseOverlayMessage::parsimPack(b);
    doPacking(b,this->command_var);
    doPacking(b,this->node_var);
}

void ScampMessage::parsimUnpack(cCommBuffer *b)
{
    BaseOverlayMessage::parsimUnpack(b);
    doUnpacking(b,this->command_var);
    doUnpacking(b,this->node_var);
}

int ScampMessage::getCommand() const
{
    return command_var;
}

void ScampMessage::setCommand(int command)
{
    this->command_var = command;
}

NodeHandle& ScampMessage::getNode()
{
    return node_var;
}

void ScampMessage::setNode(const NodeHandle& node)
{
    this->node_var = node;
}

class ScampMessageDescriptor : public cClassDescriptor
{
  public:
    ScampMessageDescriptor();
    virtual ~ScampMessageDescriptor();

    virtual bool doesSupport(cObject *obj) const;
    virtual const char *getProperty(const char *propertyname) const;
    virtual int getFieldCount(void *object) const;
    virtual const char *getFieldName(void *object, int field) const;
    virtual int findField(void *object, const char *fieldName) const;
    virtual unsigned int getFieldTypeFlags(void *object, int field) const;
    virtual const char *getFieldTypeString(void *object, int field) const;
    virtual const char *getFieldProperty(void *object, int field, const char *propertyname) const;
    virtual int getArraySize(void *object, int field) const;

    virtual std::string getFieldAsString(void *object, int field, int i) const;
    virtual bool setFieldAsString(void *object, int field, int i, const char *value) const;

    virtual const char *getFieldStructName(void *object, int field) const;
    virtual void *getFieldStructPointer(void *object, int field, int i) const;
};

Register_ClassDescriptor(ScampMessageDescriptor);

ScampMessageDescriptor::ScampMessageDescriptor() : cClassDescriptor("ScampMessage", "BaseOverlayMessage")
{
}

ScampMessageDescriptor::~ScampMessageDescriptor()
{
}

bool ScampMessageDescriptor::doesSupport(cObject *obj) const
{
    return dynamic_cast<ScampMessage *>(obj)!=NULL;
}

const char *ScampMessageDescriptor::getProperty(const char *propertyname) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    return basedesc ? basedesc->getProperty(propertyname) : NULL;
}

int ScampMessageDescriptor::getFieldCount(void *object) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    return basedesc ? 2+basedesc->getFieldCount(object) : 2;
}

unsigned int ScampMessageDescriptor::getFieldTypeFlags(void *object, int field) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldTypeFlags(object, field);
        field -= basedesc->getFieldCount(object);
    }
    static unsigned int fieldTypeFlags[] = {
        FD_ISEDITABLE,
        FD_ISCOMPOUND,
    };
    return (field>=0 && field<2) ? fieldTypeFlags[field] : 0;
}

const char *ScampMessageDescriptor::getFieldName(void *object, int field) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldName(object, field);
        field -= basedesc->getFieldCount(object);
    }
    static const char *fieldNames[] = {
        "command",
        "node",
    };
    return (field>=0 && field<2) ? fieldNames[field] : NULL;
}

int ScampMessageDescriptor::findField(void *object, const char *fieldName) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    int base = basedesc ? basedesc->getFieldCount(object) : 0;
    if (fieldName[0]=='c' && strcmp(fieldName, "command")==0) return base+0;
    if (fieldName[0]=='n' && strcmp(fieldName, "node")==0) return base+1;
    return basedesc ? basedesc->findField(object, fieldName) : -1;
}

const char *ScampMessageDescriptor::getFieldTypeString(void *object, int field) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldTypeString(object, field);
        field -= basedesc->getFieldCount(object);
    }
    static const char *fieldTypeStrings[] = {
        "int",
        "NodeHandle",
    };
    return (field>=0 && field<2) ? fieldTypeStrings[field] : NULL;
}

const char *ScampMessageDescriptor::getFieldProperty(void *object, int field, const char *propertyname) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldProperty(object, field, propertyname);
        field -= basedesc->getFieldCount(object);
    }
    switch (field) {
        case 0:
            if (!strcmp(propertyname,"enum")) return "ScampCommand";
            return NULL;
        default: return NULL;
    }
}

int ScampMessageDescriptor::getArraySize(void *object, int field) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getArraySize(object, field);
        field -= basedesc->getFieldCount(object);
    }
    ScampMessage *pp = (ScampMessage *)object; (void)pp;
    switch (field) {
        default: return 0;
    }
}

std::string ScampMessageDescriptor::getFieldAsString(void *object, int field, int i) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldAsString(object,field,i);
        field -= basedesc->getFieldCount(object);
    }
    ScampMessage *pp = (ScampMessage *)object; (void)pp;
    switch (field) {
        case 0: return long2string(pp->getCommand());
        case 1: {std::stringstream out; out << pp->getNode(); return out.str();}
        default: return "";
    }
}

bool ScampMessageDescriptor::setFieldAsString(void *object, int field, int i, const char *value) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->setFieldAsString(object,field,i,value);
        field -= basedesc->getFieldCount(object);
    }
    ScampMessage *pp = (ScampMessage *)object; (void)pp;
    switch (field) {
        case 0: pp->setCommand(string2long(value)); return true;
        default: return false;
    }
}

const char *ScampMessageDescriptor::getFieldStructName(void *object, int field) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldStructName(object, field);
        field -= basedesc->getFieldCount(object);
    }
    static const char *fieldStructNames[] = {
        NULL,
        "NodeHandle",
    };
    return (field>=0 && field<2) ? fieldStructNames[field] : NULL;
}

void *ScampMessageDescriptor::getFieldStructPointer(void *object, int field, int i) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldStructPointer(object, field, i);
        field -= basedesc->getFieldCount(object);
    }
    ScampMessage *pp = (ScampMessage *)object; (void)pp;
    switch (field) {
        case 1: return (void *)(&pp->getNode()); break;
        default: return NULL;
    }
}

Register_Class(ForwardMessage);

ForwardMessage::ForwardMessage(const char *name, int kind) : ScampMessage(name,kind)
{
    this->steps_var = 0;
}

ForwardMessage::ForwardMessage(const ForwardMessage& other) : ScampMessage(other)
{
    copy(other);
}

ForwardMessage::~ForwardMessage()
{
}

ForwardMessage& ForwardMessage::operator=(const ForwardMessage& other)
{
    if (this==&other) return *this;
    ScampMessage::operator=(other);
    copy(other);
    return *this;
}

void ForwardMessage::copy(const ForwardMessage& other)
{
    this->steps_var = other.steps_var;
    this->sourceNode_var = other.sourceNode_var;
}

void ForwardMessage::parsimPack(cCommBuffer *b)
{
    ScampMessage::parsimPack(b);
    doPacking(b,this->steps_var);
    doPacking(b,this->sourceNode_var);
}

void ForwardMessage::parsimUnpack(cCommBuffer *b)
{
    ScampMessage::parsimUnpack(b);
    doUnpacking(b,this->steps_var);
    doUnpacking(b,this->sourceNode_var);
}

int ForwardMessage::getSteps() const
{
    return steps_var;
}

void ForwardMessage::setSteps(int steps)
{
    this->steps_var = steps;
}

NodeHandle& ForwardMessage::getSourceNode()
{
    return sourceNode_var;
}

void ForwardMessage::setSourceNode(const NodeHandle& sourceNode)
{
    this->sourceNode_var = sourceNode;
}

class ForwardMessageDescriptor : public cClassDescriptor
{
  public:
    ForwardMessageDescriptor();
    virtual ~ForwardMessageDescriptor();

    virtual bool doesSupport(cObject *obj) const;
    virtual const char *getProperty(const char *propertyname) const;
    virtual int getFieldCount(void *object) const;
    virtual const char *getFieldName(void *object, int field) const;
    virtual int findField(void *object, const char *fieldName) const;
    virtual unsigned int getFieldTypeFlags(void *object, int field) const;
    virtual const char *getFieldTypeString(void *object, int field) const;
    virtual const char *getFieldProperty(void *object, int field, const char *propertyname) const;
    virtual int getArraySize(void *object, int field) const;

    virtual std::string getFieldAsString(void *object, int field, int i) const;
    virtual bool setFieldAsString(void *object, int field, int i, const char *value) const;

    virtual const char *getFieldStructName(void *object, int field) const;
    virtual void *getFieldStructPointer(void *object, int field, int i) const;
};

Register_ClassDescriptor(ForwardMessageDescriptor);

ForwardMessageDescriptor::ForwardMessageDescriptor() : cClassDescriptor("ForwardMessage", "ScampMessage")
{
}

ForwardMessageDescriptor::~ForwardMessageDescriptor()
{
}

bool ForwardMessageDescriptor::doesSupport(cObject *obj) const
{
    return dynamic_cast<ForwardMessage *>(obj)!=NULL;
}

const char *ForwardMessageDescriptor::getProperty(const char *propertyname) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    return basedesc ? basedesc->getProperty(propertyname) : NULL;
}

int ForwardMessageDescriptor::getFieldCount(void *object) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    return basedesc ? 2+basedesc->getFieldCount(object) : 2;
}

unsigned int ForwardMessageDescriptor::getFieldTypeFlags(void *object, int field) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldTypeFlags(object, field);
        field -= basedesc->getFieldCount(object);
    }
    static unsigned int fieldTypeFlags[] = {
        FD_ISEDITABLE,
        FD_ISCOMPOUND,
    };
    return (field>=0 && field<2) ? fieldTypeFlags[field] : 0;
}

const char *ForwardMessageDescriptor::getFieldName(void *object, int field) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldName(object, field);
        field -= basedesc->getFieldCount(object);
    }
    static const char *fieldNames[] = {
        "steps",
        "sourceNode",
    };
    return (field>=0 && field<2) ? fieldNames[field] : NULL;
}

int ForwardMessageDescriptor::findField(void *object, const char *fieldName) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    int base = basedesc ? basedesc->getFieldCount(object) : 0;
    if (fieldName[0]=='s' && strcmp(fieldName, "steps")==0) return base+0;
    if (fieldName[0]=='s' && strcmp(fieldName, "sourceNode")==0) return base+1;
    return basedesc ? basedesc->findField(object, fieldName) : -1;
}

const char *ForwardMessageDescriptor::getFieldTypeString(void *object, int field) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldTypeString(object, field);
        field -= basedesc->getFieldCount(object);
    }
    static const char *fieldTypeStrings[] = {
        "int",
        "NodeHandle",
    };
    return (field>=0 && field<2) ? fieldTypeStrings[field] : NULL;
}

const char *ForwardMessageDescriptor::getFieldProperty(void *object, int field, const char *propertyname) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldProperty(object, field, propertyname);
        field -= basedesc->getFieldCount(object);
    }
    switch (field) {
        default: return NULL;
    }
}

int ForwardMessageDescriptor::getArraySize(void *object, int field) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getArraySize(object, field);
        field -= basedesc->getFieldCount(object);
    }
    ForwardMessage *pp = (ForwardMessage *)object; (void)pp;
    switch (field) {
        default: return 0;
    }
}

std::string ForwardMessageDescriptor::getFieldAsString(void *object, int field, int i) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldAsString(object,field,i);
        field -= basedesc->getFieldCount(object);
    }
    ForwardMessage *pp = (ForwardMessage *)object; (void)pp;
    switch (field) {
        case 0: return long2string(pp->getSteps());
        case 1: {std::stringstream out; out << pp->getSourceNode(); return out.str();}
        default: return "";
    }
}

bool ForwardMessageDescriptor::setFieldAsString(void *object, int field, int i, const char *value) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->setFieldAsString(object,field,i,value);
        field -= basedesc->getFieldCount(object);
    }
    ForwardMessage *pp = (ForwardMessage *)object; (void)pp;
    switch (field) {
        case 0: pp->setSteps(string2long(value)); return true;
        default: return false;
    }
}

const char *ForwardMessageDescriptor::getFieldStructName(void *object, int field) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldStructName(object, field);
        field -= basedesc->getFieldCount(object);
    }
    static const char *fieldStructNames[] = {
        NULL,
        "NodeHandle",
    };
    return (field>=0 && field<2) ? fieldStructNames[field] : NULL;
}

void *ForwardMessageDescriptor::getFieldStructPointer(void *object, int field, int i) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldStructPointer(object, field, i);
        field -= basedesc->getFieldCount(object);
    }
    ForwardMessage *pp = (ForwardMessage *)object; (void)pp;
    switch (field) {
        case 1: return (void *)(&pp->getSourceNode()); break;
        default: return NULL;
    }
}

Register_Class(LeaveMessage);

LeaveMessage::LeaveMessage(const char *name, int kind) : ScampMessage(name,kind)
{
}

LeaveMessage::LeaveMessage(const LeaveMessage& other) : ScampMessage(other)
{
    copy(other);
}

LeaveMessage::~LeaveMessage()
{
}

LeaveMessage& LeaveMessage::operator=(const LeaveMessage& other)
{
    if (this==&other) return *this;
    ScampMessage::operator=(other);
    copy(other);
    return *this;
}

void LeaveMessage::copy(const LeaveMessage& other)
{
    this->newNode_var = other.newNode_var;
}

void LeaveMessage::parsimPack(cCommBuffer *b)
{
    ScampMessage::parsimPack(b);
    doPacking(b,this->newNode_var);
}

void LeaveMessage::parsimUnpack(cCommBuffer *b)
{
    ScampMessage::parsimUnpack(b);
    doUnpacking(b,this->newNode_var);
}

NodeHandle& LeaveMessage::getNewNode()
{
    return newNode_var;
}

void LeaveMessage::setNewNode(const NodeHandle& newNode)
{
    this->newNode_var = newNode;
}

class LeaveMessageDescriptor : public cClassDescriptor
{
  public:
    LeaveMessageDescriptor();
    virtual ~LeaveMessageDescriptor();

    virtual bool doesSupport(cObject *obj) const;
    virtual const char *getProperty(const char *propertyname) const;
    virtual int getFieldCount(void *object) const;
    virtual const char *getFieldName(void *object, int field) const;
    virtual int findField(void *object, const char *fieldName) const;
    virtual unsigned int getFieldTypeFlags(void *object, int field) const;
    virtual const char *getFieldTypeString(void *object, int field) const;
    virtual const char *getFieldProperty(void *object, int field, const char *propertyname) const;
    virtual int getArraySize(void *object, int field) const;

    virtual std::string getFieldAsString(void *object, int field, int i) const;
    virtual bool setFieldAsString(void *object, int field, int i, const char *value) const;

    virtual const char *getFieldStructName(void *object, int field) const;
    virtual void *getFieldStructPointer(void *object, int field, int i) const;
};

Register_ClassDescriptor(LeaveMessageDescriptor);

LeaveMessageDescriptor::LeaveMessageDescriptor() : cClassDescriptor("LeaveMessage", "ScampMessage")
{
}

LeaveMessageDescriptor::~LeaveMessageDescriptor()
{
}

bool LeaveMessageDescriptor::doesSupport(cObject *obj) const
{
    return dynamic_cast<LeaveMessage *>(obj)!=NULL;
}

const char *LeaveMessageDescriptor::getProperty(const char *propertyname) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    return basedesc ? basedesc->getProperty(propertyname) : NULL;
}

int LeaveMessageDescriptor::getFieldCount(void *object) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    return basedesc ? 1+basedesc->getFieldCount(object) : 1;
}

unsigned int LeaveMessageDescriptor::getFieldTypeFlags(void *object, int field) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldTypeFlags(object, field);
        field -= basedesc->getFieldCount(object);
    }
    static unsigned int fieldTypeFlags[] = {
        FD_ISCOMPOUND,
    };
    return (field>=0 && field<1) ? fieldTypeFlags[field] : 0;
}

const char *LeaveMessageDescriptor::getFieldName(void *object, int field) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldName(object, field);
        field -= basedesc->getFieldCount(object);
    }
    static const char *fieldNames[] = {
        "newNode",
    };
    return (field>=0 && field<1) ? fieldNames[field] : NULL;
}

int LeaveMessageDescriptor::findField(void *object, const char *fieldName) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    int base = basedesc ? basedesc->getFieldCount(object) : 0;
    if (fieldName[0]=='n' && strcmp(fieldName, "newNode")==0) return base+0;
    return basedesc ? basedesc->findField(object, fieldName) : -1;
}

const char *LeaveMessageDescriptor::getFieldTypeString(void *object, int field) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldTypeString(object, field);
        field -= basedesc->getFieldCount(object);
    }
    static const char *fieldTypeStrings[] = {
        "NodeHandle",
    };
    return (field>=0 && field<1) ? fieldTypeStrings[field] : NULL;
}

const char *LeaveMessageDescriptor::getFieldProperty(void *object, int field, const char *propertyname) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldProperty(object, field, propertyname);
        field -= basedesc->getFieldCount(object);
    }
    switch (field) {
        default: return NULL;
    }
}

int LeaveMessageDescriptor::getArraySize(void *object, int field) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getArraySize(object, field);
        field -= basedesc->getFieldCount(object);
    }
    LeaveMessage *pp = (LeaveMessage *)object; (void)pp;
    switch (field) {
        default: return 0;
    }
}

std::string LeaveMessageDescriptor::getFieldAsString(void *object, int field, int i) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldAsString(object,field,i);
        field -= basedesc->getFieldCount(object);
    }
    LeaveMessage *pp = (LeaveMessage *)object; (void)pp;
    switch (field) {
        case 0: {std::stringstream out; out << pp->getNewNode(); return out.str();}
        default: return "";
    }
}

bool LeaveMessageDescriptor::setFieldAsString(void *object, int field, int i, const char *value) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->setFieldAsString(object,field,i,value);
        field -= basedesc->getFieldCount(object);
    }
    LeaveMessage *pp = (LeaveMessage *)object; (void)pp;
    switch (field) {
        default: return false;
    }
}

const char *LeaveMessageDescriptor::getFieldStructName(void *object, int field) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldStructName(object, field);
        field -= basedesc->getFieldCount(object);
    }
    static const char *fieldStructNames[] = {
        "NodeHandle",
    };
    return (field>=0 && field<1) ? fieldStructNames[field] : NULL;
}

void *LeaveMessageDescriptor::getFieldStructPointer(void *object, int field, int i) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldStructPointer(object, field, i);
        field -= basedesc->getFieldCount(object);
    }
    LeaveMessage *pp = (LeaveMessage *)object; (void)pp;
    switch (field) {
        case 0: return (void *)(&pp->getNewNode()); break;
        default: return NULL;
    }
}

Register_Class(NewNodeMessage);

NewNodeMessage::NewNodeMessage(const char *name, int kind) : ScampMessage(name,kind)
{
}

NewNodeMessage::NewNodeMessage(const NewNodeMessage& other) : ScampMessage(other)
{
    copy(other);
}

NewNodeMessage::~NewNodeMessage()
{
}

NewNodeMessage& NewNodeMessage::operator=(const NewNodeMessage& other)
{
    if (this==&other) return *this;
    ScampMessage::operator=(other);
    copy(other);
    return *this;
}

void NewNodeMessage::copy(const NewNodeMessage& other)
{
}

void NewNodeMessage::parsimPack(cCommBuffer *b)
{
    ScampMessage::parsimPack(b);
}

void NewNodeMessage::parsimUnpack(cCommBuffer *b)
{
    ScampMessage::parsimUnpack(b);
}

class NewNodeMessageDescriptor : public cClassDescriptor
{
  public:
    NewNodeMessageDescriptor();
    virtual ~NewNodeMessageDescriptor();

    virtual bool doesSupport(cObject *obj) const;
    virtual const char *getProperty(const char *propertyname) const;
    virtual int getFieldCount(void *object) const;
    virtual const char *getFieldName(void *object, int field) const;
    virtual int findField(void *object, const char *fieldName) const;
    virtual unsigned int getFieldTypeFlags(void *object, int field) const;
    virtual const char *getFieldTypeString(void *object, int field) const;
    virtual const char *getFieldProperty(void *object, int field, const char *propertyname) const;
    virtual int getArraySize(void *object, int field) const;

    virtual std::string getFieldAsString(void *object, int field, int i) const;
    virtual bool setFieldAsString(void *object, int field, int i, const char *value) const;

    virtual const char *getFieldStructName(void *object, int field) const;
    virtual void *getFieldStructPointer(void *object, int field, int i) const;
};

Register_ClassDescriptor(NewNodeMessageDescriptor);

NewNodeMessageDescriptor::NewNodeMessageDescriptor() : cClassDescriptor("NewNodeMessage", "ScampMessage")
{
}

NewNodeMessageDescriptor::~NewNodeMessageDescriptor()
{
}

bool NewNodeMessageDescriptor::doesSupport(cObject *obj) const
{
    return dynamic_cast<NewNodeMessage *>(obj)!=NULL;
}

const char *NewNodeMessageDescriptor::getProperty(const char *propertyname) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    return basedesc ? basedesc->getProperty(propertyname) : NULL;
}

int NewNodeMessageDescriptor::getFieldCount(void *object) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    return basedesc ? 0+basedesc->getFieldCount(object) : 0;
}

unsigned int NewNodeMessageDescriptor::getFieldTypeFlags(void *object, int field) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldTypeFlags(object, field);
        field -= basedesc->getFieldCount(object);
    }
    return 0;
}

const char *NewNodeMessageDescriptor::getFieldName(void *object, int field) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldName(object, field);
        field -= basedesc->getFieldCount(object);
    }
    return NULL;
}

int NewNodeMessageDescriptor::findField(void *object, const char *fieldName) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    return basedesc ? basedesc->findField(object, fieldName) : -1;
}

const char *NewNodeMessageDescriptor::getFieldTypeString(void *object, int field) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldTypeString(object, field);
        field -= basedesc->getFieldCount(object);
    }
    return NULL;
}

const char *NewNodeMessageDescriptor::getFieldProperty(void *object, int field, const char *propertyname) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldProperty(object, field, propertyname);
        field -= basedesc->getFieldCount(object);
    }
    switch (field) {
        default: return NULL;
    }
}

int NewNodeMessageDescriptor::getArraySize(void *object, int field) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getArraySize(object, field);
        field -= basedesc->getFieldCount(object);
    }
    NewNodeMessage *pp = (NewNodeMessage *)object; (void)pp;
    switch (field) {
        default: return 0;
    }
}

std::string NewNodeMessageDescriptor::getFieldAsString(void *object, int field, int i) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldAsString(object,field,i);
        field -= basedesc->getFieldCount(object);
    }
    NewNodeMessage *pp = (NewNodeMessage *)object; (void)pp;
    switch (field) {
        default: return "";
    }
}

bool NewNodeMessageDescriptor::setFieldAsString(void *object, int field, int i, const char *value) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->setFieldAsString(object,field,i,value);
        field -= basedesc->getFieldCount(object);
    }
    NewNodeMessage *pp = (NewNodeMessage *)object; (void)pp;
    switch (field) {
        default: return false;
    }
}

const char *NewNodeMessageDescriptor::getFieldStructName(void *object, int field) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldStructName(object, field);
        field -= basedesc->getFieldCount(object);
    }
    return NULL;
}

void *NewNodeMessageDescriptor::getFieldStructPointer(void *object, int field, int i) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldStructPointer(object, field, i);
        field -= basedesc->getFieldCount(object);
    }
    NewNodeMessage *pp = (NewNodeMessage *)object; (void)pp;
    switch (field) {
        default: return NULL;
    }
}

Register_Class(ScampJoinCall);

ScampJoinCall::ScampJoinCall(const char *name, int kind) : BaseCallMessage(name,kind)
{
    this->time_var = 0;
}

ScampJoinCall::ScampJoinCall(const ScampJoinCall& other) : BaseCallMessage(other)
{
    copy(other);
}

ScampJoinCall::~ScampJoinCall()
{
}

ScampJoinCall& ScampJoinCall::operator=(const ScampJoinCall& other)
{
    if (this==&other) return *this;
    BaseCallMessage::operator=(other);
    copy(other);
    return *this;
}

void ScampJoinCall::copy(const ScampJoinCall& other)
{
    this->address_var = other.address_var;
    this->time_var = other.time_var;
}

void ScampJoinCall::parsimPack(cCommBuffer *b)
{
    BaseCallMessage::parsimPack(b);
    doPacking(b,this->address_var);
    doPacking(b,this->time_var);
}

void ScampJoinCall::parsimUnpack(cCommBuffer *b)
{
    BaseCallMessage::parsimUnpack(b);
    doUnpacking(b,this->address_var);
    doUnpacking(b,this->time_var);
}

IPvXAddress& ScampJoinCall::getAddress()
{
    return address_var;
}

void ScampJoinCall::setAddress(const IPvXAddress& address)
{
    this->address_var = address;
}

simtime_t ScampJoinCall::getTime() const
{
    return time_var;
}

void ScampJoinCall::setTime(simtime_t time)
{
    this->time_var = time;
}

class ScampJoinCallDescriptor : public cClassDescriptor
{
  public:
    ScampJoinCallDescriptor();
    virtual ~ScampJoinCallDescriptor();

    virtual bool doesSupport(cObject *obj) const;
    virtual const char *getProperty(const char *propertyname) const;
    virtual int getFieldCount(void *object) const;
    virtual const char *getFieldName(void *object, int field) const;
    virtual int findField(void *object, const char *fieldName) const;
    virtual unsigned int getFieldTypeFlags(void *object, int field) const;
    virtual const char *getFieldTypeString(void *object, int field) const;
    virtual const char *getFieldProperty(void *object, int field, const char *propertyname) const;
    virtual int getArraySize(void *object, int field) const;

    virtual std::string getFieldAsString(void *object, int field, int i) const;
    virtual bool setFieldAsString(void *object, int field, int i, const char *value) const;

    virtual const char *getFieldStructName(void *object, int field) const;
    virtual void *getFieldStructPointer(void *object, int field, int i) const;
};

Register_ClassDescriptor(ScampJoinCallDescriptor);

ScampJoinCallDescriptor::ScampJoinCallDescriptor() : cClassDescriptor("ScampJoinCall", "BaseCallMessage")
{
}

ScampJoinCallDescriptor::~ScampJoinCallDescriptor()
{
}

bool ScampJoinCallDescriptor::doesSupport(cObject *obj) const
{
    return dynamic_cast<ScampJoinCall *>(obj)!=NULL;
}

const char *ScampJoinCallDescriptor::getProperty(const char *propertyname) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    return basedesc ? basedesc->getProperty(propertyname) : NULL;
}

int ScampJoinCallDescriptor::getFieldCount(void *object) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    return basedesc ? 2+basedesc->getFieldCount(object) : 2;
}

unsigned int ScampJoinCallDescriptor::getFieldTypeFlags(void *object, int field) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldTypeFlags(object, field);
        field -= basedesc->getFieldCount(object);
    }
    static unsigned int fieldTypeFlags[] = {
        FD_ISCOMPOUND,
        FD_ISEDITABLE,
    };
    return (field>=0 && field<2) ? fieldTypeFlags[field] : 0;
}

const char *ScampJoinCallDescriptor::getFieldName(void *object, int field) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldName(object, field);
        field -= basedesc->getFieldCount(object);
    }
    static const char *fieldNames[] = {
        "address",
        "time",
    };
    return (field>=0 && field<2) ? fieldNames[field] : NULL;
}

int ScampJoinCallDescriptor::findField(void *object, const char *fieldName) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    int base = basedesc ? basedesc->getFieldCount(object) : 0;
    if (fieldName[0]=='a' && strcmp(fieldName, "address")==0) return base+0;
    if (fieldName[0]=='t' && strcmp(fieldName, "time")==0) return base+1;
    return basedesc ? basedesc->findField(object, fieldName) : -1;
}

const char *ScampJoinCallDescriptor::getFieldTypeString(void *object, int field) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldTypeString(object, field);
        field -= basedesc->getFieldCount(object);
    }
    static const char *fieldTypeStrings[] = {
        "IPvXAddress",
        "simtime_t",
    };
    return (field>=0 && field<2) ? fieldTypeStrings[field] : NULL;
}

const char *ScampJoinCallDescriptor::getFieldProperty(void *object, int field, const char *propertyname) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldProperty(object, field, propertyname);
        field -= basedesc->getFieldCount(object);
    }
    switch (field) {
        default: return NULL;
    }
}

int ScampJoinCallDescriptor::getArraySize(void *object, int field) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getArraySize(object, field);
        field -= basedesc->getFieldCount(object);
    }
    ScampJoinCall *pp = (ScampJoinCall *)object; (void)pp;
    switch (field) {
        default: return 0;
    }
}

std::string ScampJoinCallDescriptor::getFieldAsString(void *object, int field, int i) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldAsString(object,field,i);
        field -= basedesc->getFieldCount(object);
    }
    ScampJoinCall *pp = (ScampJoinCall *)object; (void)pp;
    switch (field) {
        case 0: {std::stringstream out; out << pp->getAddress(); return out.str();}
        case 1: return double2string(pp->getTime());
        default: return "";
    }
}

bool ScampJoinCallDescriptor::setFieldAsString(void *object, int field, int i, const char *value) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->setFieldAsString(object,field,i,value);
        field -= basedesc->getFieldCount(object);
    }
    ScampJoinCall *pp = (ScampJoinCall *)object; (void)pp;
    switch (field) {
        case 1: pp->setTime(string2double(value)); return true;
        default: return false;
    }
}

const char *ScampJoinCallDescriptor::getFieldStructName(void *object, int field) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldStructName(object, field);
        field -= basedesc->getFieldCount(object);
    }
    static const char *fieldStructNames[] = {
        "IPvXAddress",
        NULL,
    };
    return (field>=0 && field<2) ? fieldStructNames[field] : NULL;
}

void *ScampJoinCallDescriptor::getFieldStructPointer(void *object, int field, int i) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldStructPointer(object, field, i);
        field -= basedesc->getFieldCount(object);
    }
    ScampJoinCall *pp = (ScampJoinCall *)object; (void)pp;
    switch (field) {
        case 0: return (void *)(&pp->getAddress()); break;
        default: return NULL;
    }
}

Register_Class(ScampJoinResponse);

ScampJoinResponse::ScampJoinResponse(const char *name, int kind) : BaseResponseMessage(name,kind)
{
    this->command_var = 0;
}

ScampJoinResponse::ScampJoinResponse(const ScampJoinResponse& other) : BaseResponseMessage(other)
{
    copy(other);
}

ScampJoinResponse::~ScampJoinResponse()
{
}

ScampJoinResponse& ScampJoinResponse::operator=(const ScampJoinResponse& other)
{
    if (this==&other) return *this;
    BaseResponseMessage::operator=(other);
    copy(other);
    return *this;
}

void ScampJoinResponse::copy(const ScampJoinResponse& other)
{
    this->command_var = other.command_var;
}

void ScampJoinResponse::parsimPack(cCommBuffer *b)
{
    BaseResponseMessage::parsimPack(b);
    doPacking(b,this->command_var);
}

void ScampJoinResponse::parsimUnpack(cCommBuffer *b)
{
    BaseResponseMessage::parsimUnpack(b);
    doUnpacking(b,this->command_var);
}

int ScampJoinResponse::getCommand() const
{
    return command_var;
}

void ScampJoinResponse::setCommand(int command)
{
    this->command_var = command;
}

class ScampJoinResponseDescriptor : public cClassDescriptor
{
  public:
    ScampJoinResponseDescriptor();
    virtual ~ScampJoinResponseDescriptor();

    virtual bool doesSupport(cObject *obj) const;
    virtual const char *getProperty(const char *propertyname) const;
    virtual int getFieldCount(void *object) const;
    virtual const char *getFieldName(void *object, int field) const;
    virtual int findField(void *object, const char *fieldName) const;
    virtual unsigned int getFieldTypeFlags(void *object, int field) const;
    virtual const char *getFieldTypeString(void *object, int field) const;
    virtual const char *getFieldProperty(void *object, int field, const char *propertyname) const;
    virtual int getArraySize(void *object, int field) const;

    virtual std::string getFieldAsString(void *object, int field, int i) const;
    virtual bool setFieldAsString(void *object, int field, int i, const char *value) const;

    virtual const char *getFieldStructName(void *object, int field) const;
    virtual void *getFieldStructPointer(void *object, int field, int i) const;
};

Register_ClassDescriptor(ScampJoinResponseDescriptor);

ScampJoinResponseDescriptor::ScampJoinResponseDescriptor() : cClassDescriptor("ScampJoinResponse", "BaseResponseMessage")
{
}

ScampJoinResponseDescriptor::~ScampJoinResponseDescriptor()
{
}

bool ScampJoinResponseDescriptor::doesSupport(cObject *obj) const
{
    return dynamic_cast<ScampJoinResponse *>(obj)!=NULL;
}

const char *ScampJoinResponseDescriptor::getProperty(const char *propertyname) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    return basedesc ? basedesc->getProperty(propertyname) : NULL;
}

int ScampJoinResponseDescriptor::getFieldCount(void *object) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    return basedesc ? 1+basedesc->getFieldCount(object) : 1;
}

unsigned int ScampJoinResponseDescriptor::getFieldTypeFlags(void *object, int field) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldTypeFlags(object, field);
        field -= basedesc->getFieldCount(object);
    }
    static unsigned int fieldTypeFlags[] = {
        FD_ISEDITABLE,
    };
    return (field>=0 && field<1) ? fieldTypeFlags[field] : 0;
}

const char *ScampJoinResponseDescriptor::getFieldName(void *object, int field) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldName(object, field);
        field -= basedesc->getFieldCount(object);
    }
    static const char *fieldNames[] = {
        "command",
    };
    return (field>=0 && field<1) ? fieldNames[field] : NULL;
}

int ScampJoinResponseDescriptor::findField(void *object, const char *fieldName) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    int base = basedesc ? basedesc->getFieldCount(object) : 0;
    if (fieldName[0]=='c' && strcmp(fieldName, "command")==0) return base+0;
    return basedesc ? basedesc->findField(object, fieldName) : -1;
}

const char *ScampJoinResponseDescriptor::getFieldTypeString(void *object, int field) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldTypeString(object, field);
        field -= basedesc->getFieldCount(object);
    }
    static const char *fieldTypeStrings[] = {
        "int",
    };
    return (field>=0 && field<1) ? fieldTypeStrings[field] : NULL;
}

const char *ScampJoinResponseDescriptor::getFieldProperty(void *object, int field, const char *propertyname) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldProperty(object, field, propertyname);
        field -= basedesc->getFieldCount(object);
    }
    switch (field) {
        case 0:
            if (!strcmp(propertyname,"enum")) return "ScampCommand";
            return NULL;
        default: return NULL;
    }
}

int ScampJoinResponseDescriptor::getArraySize(void *object, int field) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getArraySize(object, field);
        field -= basedesc->getFieldCount(object);
    }
    ScampJoinResponse *pp = (ScampJoinResponse *)object; (void)pp;
    switch (field) {
        default: return 0;
    }
}

std::string ScampJoinResponseDescriptor::getFieldAsString(void *object, int field, int i) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldAsString(object,field,i);
        field -= basedesc->getFieldCount(object);
    }
    ScampJoinResponse *pp = (ScampJoinResponse *)object; (void)pp;
    switch (field) {
        case 0: return long2string(pp->getCommand());
        default: return "";
    }
}

bool ScampJoinResponseDescriptor::setFieldAsString(void *object, int field, int i, const char *value) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->setFieldAsString(object,field,i,value);
        field -= basedesc->getFieldCount(object);
    }
    ScampJoinResponse *pp = (ScampJoinResponse *)object; (void)pp;
    switch (field) {
        case 0: pp->setCommand(string2long(value)); return true;
        default: return false;
    }
}

const char *ScampJoinResponseDescriptor::getFieldStructName(void *object, int field) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldStructName(object, field);
        field -= basedesc->getFieldCount(object);
    }
    static const char *fieldStructNames[] = {
        NULL,
    };
    return (field>=0 && field<1) ? fieldStructNames[field] : NULL;
}

void *ScampJoinResponseDescriptor::getFieldStructPointer(void *object, int field, int i) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldStructPointer(object, field, i);
        field -= basedesc->getFieldCount(object);
    }
    ScampJoinResponse *pp = (ScampJoinResponse *)object; (void)pp;
    switch (field) {
        default: return NULL;
    }
}


