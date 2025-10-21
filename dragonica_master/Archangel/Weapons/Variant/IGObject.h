#ifndef VARIANTE_BASIC_IGOBJECT_H
#define VARIANTE_BASIC_IGOBJECT_H

#include "Lohengrin/ActArg.h"

class IGObject
{
public:
	IGObject(){}
	virtual ~IGObject(){}

public:
	virtual int ObjectType()const{return 0;}
	virtual int Enter(SActArg *pActArg) = 0;
	virtual int Leave(SActArg *pActArg) = 0;

	virtual int CallAction(WORD wActCode, SActArg *pActArg) = 0;
};

#endif // VARIANTE_BASIC_IGOBJECT_H