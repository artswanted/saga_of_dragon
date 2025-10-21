#ifndef IMMIGRATION_IMMIGRATIONSERVER_MAINFRAME_CONSTANT_H
#define IMMIGRATION_IMMIGRATIONSERVER_MAINFRAME_CONSTANT_H

typedef enum eVar_Kind
{
	EVar_Kind_None = 0,
	EVar_Kind_Login = 1,
} EVar_Kind;

typedef enum
{
	EVKind_None = 0,
	EVAR_Login_Contents_WaitSecond = 1,
	EVAR_Login_Check_HaveExtVarValue = 2,
} EVariable_Login;

#endif // IMMIGRATION_IMMIGRATIONSERVER_MAINFRAME_CONSTANT_H