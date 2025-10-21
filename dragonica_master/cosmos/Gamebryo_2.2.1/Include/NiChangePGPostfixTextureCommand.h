#ifndef NICHANGEPGPOSTFIXTEXTURECOMMAND_H
#define NICHANGEPGPOSTFIXTEXTURECOMMAND_H

#include <NiRefObject.h>
#include <NiSmartPointer.h>
#include "NiEntityCommandInterface.h"
#include "NiEntityInterface.h"

class NIENTITY_ENTRY NiChangePGPostfixTextureCommand : public NiRefObject,
    public NiEntityCommandInterface
{
public:
    NiChangePGPostfixTextureCommand(NiEntityInterface* pkEntity, 
        const NiFixedString &kPGPostfixTexture);

    NiEntityInterface* GetEntity() const;
	NiFixedString GetPGPostfixTexture() const;

private:
    NiEntityInterfaceIPtr m_spEntity;
	NiFixedString m_kPGPostfixTexture;

    NiFixedString m_kCommandName;
    bool m_bOldDataValid;

public:
    // NiEntityCommandInterface overrides.
    virtual void AddReference();
    virtual void RemoveReference();
    virtual NiFixedString GetName();
    virtual void DoCommand(NiEntityErrorInterface* pkErrors, bool bUndoable);
    virtual void UndoCommand(NiEntityErrorInterface* pkErrors);
};

NiSmartPointer(NiChangePGPostfixTextureCommand);

#include "NiChangePGPostfixTextureCommand.inl"

#endif
