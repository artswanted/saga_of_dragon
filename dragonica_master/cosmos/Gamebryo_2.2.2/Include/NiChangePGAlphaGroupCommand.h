#ifndef NICHANGEPGALPHAGROUPCOMMAND_H
#define NICHANGEPGALPHAGROUPCOMMAND_H

#include <NiRefObject.h>
#include <NiSmartPointer.h>
#include "NiEntityCommandInterface.h"
#include "NiEntityInterface.h"

class NIENTITY_ENTRY NiChangePGAlphaGroupCommand : public NiRefObject,
    public NiEntityCommandInterface
{
public:
    NiChangePGAlphaGroupCommand(NiEntityInterface* pkEntity, 
        const NiFixedString &kPGAlphaGroup);

    NiEntityInterface* GetEntity() const;
	NiFixedString GetPGAlphaGroup() const;

private:
    NiEntityInterfaceIPtr m_spEntity;
	NiFixedString m_kPGAlphaGroup;

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

NiSmartPointer(NiChangePGAlphaGroupCommand);

#include "NiChangePGAlphaGroupCommand.inl"

#endif
