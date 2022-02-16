//
// Generated file, do not edit! Created by opp_msgc 4.2 from applications/pbft/PBFTMessage.msg.
//

// Disable warnings about unused variables, empty switch stmts, etc:
#ifdef _MSC_VER
#  pragma warning(disable:4101)
#  pragma warning(disable:4065)
#endif

#include <iostream>
#include <sstream>
#include "PBFTMessage_m.h"

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
    cEnum *e = cEnum::find("MessageType");
    if (!e) enums.getInstance()->add(e = new cEnum("MessageType"));
    e->insert(REQUEST, "REQUEST");
);

Register_Class(PBFTMessage);

PBFTMessage::PBFTMessage(const char *name, int kind) : cPacket(name,kind)
{
    this->type_var = 0;
    this->timestamp_var = 0;
}

PBFTMessage::PBFTMessage(const PBFTMessage& other) : cPacket(other)
{
    copy(other);
}

PBFTMessage::~PBFTMessage()
{
}

PBFTMessage& PBFTMessage::operator=(const PBFTMessage& other)
{
    if (this==&other) return *this;
    cPacket::operator=(other);
    copy(other);
    return *this;
}

void PBFTMessage::copy(const PBFTMessage& other)
{
    this->type_var = other.type_var;
    this->senderAddress_var = other.senderAddress_var;
    this->oKey_var = other.oKey_var;
    this->timestamp_var = other.timestamp_var;
    this->op_var = other.op_var;
}

void PBFTMessage::parsimPack(cCommBuffer *b)
{
    cPacket::parsimPack(b);
    doPacking(b,this->type_var);
    doPacking(b,this->senderAddress_var);
    doPacking(b,this->oKey_var);
    doPacking(b,this->timestamp_var);
    doPacking(b,this->op_var);
}

void PBFTMessage::parsimUnpack(cCommBuffer *b)
{
    cPacket::parsimUnpack(b);
    doUnpacking(b,this->type_var);
    doUnpacking(b,this->senderAddress_var);
    doUnpacking(b,this->oKey_var);
    doUnpacking(b,this->timestamp_var);
    doUnpacking(b,this->op_var);
}

int PBFTMessage::getType() const
{
    return type_var;
}

void PBFTMessage::setType(int type)
{
    this->type_var = type;
}

TransportAddress& PBFTMessage::getSenderAddress()
{
    return senderAddress_var;
}

void PBFTMessage::setSenderAddress(const TransportAddress& senderAddress)
{
    this->senderAddress_var = senderAddress;
}

OverlayKey& PBFTMessage::getOKey()
{
    return oKey_var;
}

void PBFTMessage::setOKey(const OverlayKey& oKey)
{
    this->oKey_var = oKey;
}

simtime_t PBFTMessage::getTimestamp() const
{
    return timestamp_var;
}

void PBFTMessage::setTimestamp(simtime_t timestamp)
{
    this->timestamp_var = timestamp;
}

Operation& PBFTMessage::getOp()
{
    return op_var;
}

void PBFTMessage::setOp(const Operation& op)
{
    this->op_var = op;
}

class PBFTMessageDescriptor : public cClassDescriptor
{
  public:
    PBFTMessageDescriptor();
    virtual ~PBFTMessageDescriptor();

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

Register_ClassDescriptor(PBFTMessageDescriptor);

PBFTMessageDescriptor::PBFTMessageDescriptor() : cClassDescriptor("PBFTMessage", "cPacket")
{
}

PBFTMessageDescriptor::~PBFTMessageDescriptor()
{
}

bool PBFTMessageDescriptor::doesSupport(cObject *obj) const
{
    return dynamic_cast<PBFTMessage *>(obj)!=NULL;
}

const char *PBFTMessageDescriptor::getProperty(const char *propertyname) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    return basedesc ? basedesc->getProperty(propertyname) : NULL;
}

int PBFTMessageDescriptor::getFieldCount(void *object) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    return basedesc ? 5+basedesc->getFieldCount(object) : 5;
}

unsigned int PBFTMessageDescriptor::getFieldTypeFlags(void *object, int field) const
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
        FD_ISCOMPOUND,
        FD_ISEDITABLE,
        FD_ISCOMPOUND,
    };
    return (field>=0 && field<5) ? fieldTypeFlags[field] : 0;
}

const char *PBFTMessageDescriptor::getFieldName(void *object, int field) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldName(object, field);
        field -= basedesc->getFieldCount(object);
    }
    static const char *fieldNames[] = {
        "type",
        "senderAddress",
        "oKey",
        "timestamp",
        "op",
    };
    return (field>=0 && field<5) ? fieldNames[field] : NULL;
}

int PBFTMessageDescriptor::findField(void *object, const char *fieldName) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    int base = basedesc ? basedesc->getFieldCount(object) : 0;
    if (fieldName[0]=='t' && strcmp(fieldName, "type")==0) return base+0;
    if (fieldName[0]=='s' && strcmp(fieldName, "senderAddress")==0) return base+1;
    if (fieldName[0]=='o' && strcmp(fieldName, "oKey")==0) return base+2;
    if (fieldName[0]=='t' && strcmp(fieldName, "timestamp")==0) return base+3;
    if (fieldName[0]=='o' && strcmp(fieldName, "op")==0) return base+4;
    return basedesc ? basedesc->findField(object, fieldName) : -1;
}

const char *PBFTMessageDescriptor::getFieldTypeString(void *object, int field) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldTypeString(object, field);
        field -= basedesc->getFieldCount(object);
    }
    static const char *fieldTypeStrings[] = {
        "int",
        "TransportAddress",
        "OverlayKey",
        "simtime_t",
        "Operation",
    };
    return (field>=0 && field<5) ? fieldTypeStrings[field] : NULL;
}

const char *PBFTMessageDescriptor::getFieldProperty(void *object, int field, const char *propertyname) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldProperty(object, field, propertyname);
        field -= basedesc->getFieldCount(object);
    }
    switch (field) {
        case 0:
            if (!strcmp(propertyname,"enum")) return "MessageType";
            return NULL;
        default: return NULL;
    }
}

int PBFTMessageDescriptor::getArraySize(void *object, int field) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getArraySize(object, field);
        field -= basedesc->getFieldCount(object);
    }
    PBFTMessage *pp = (PBFTMessage *)object; (void)pp;
    switch (field) {
        default: return 0;
    }
}

std::string PBFTMessageDescriptor::getFieldAsString(void *object, int field, int i) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldAsString(object,field,i);
        field -= basedesc->getFieldCount(object);
    }
    PBFTMessage *pp = (PBFTMessage *)object; (void)pp;
    switch (field) {
        case 0: return long2string(pp->getType());
        case 1: {std::stringstream out; out << pp->getSenderAddress(); return out.str();}
        case 2: {std::stringstream out; out << pp->getOKey(); return out.str();}
        case 3: return double2string(pp->getTimestamp());
        case 4: {std::stringstream out; out << pp->getOp(); return out.str();}
        default: return "";
    }
}

bool PBFTMessageDescriptor::setFieldAsString(void *object, int field, int i, const char *value) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->setFieldAsString(object,field,i,value);
        field -= basedesc->getFieldCount(object);
    }
    PBFTMessage *pp = (PBFTMessage *)object; (void)pp;
    switch (field) {
        case 0: pp->setType(string2long(value)); return true;
        case 3: pp->setTimestamp(string2double(value)); return true;
        default: return false;
    }
}

const char *PBFTMessageDescriptor::getFieldStructName(void *object, int field) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldStructName(object, field);
        field -= basedesc->getFieldCount(object);
    }
    static const char *fieldStructNames[] = {
        NULL,
        "TransportAddress",
        "OverlayKey",
        NULL,
        "Operation",
    };
    return (field>=0 && field<5) ? fieldStructNames[field] : NULL;
}

void *PBFTMessageDescriptor::getFieldStructPointer(void *object, int field, int i) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldStructPointer(object, field, i);
        field -= basedesc->getFieldCount(object);
    }
    PBFTMessage *pp = (PBFTMessage *)object; (void)pp;
    switch (field) {
        case 1: return (void *)(&pp->getSenderAddress()); break;
        case 2: return (void *)(&pp->getOKey()); break;
        case 4: return (void *)(&pp->getOp()); break;
        default: return NULL;
    }
}


