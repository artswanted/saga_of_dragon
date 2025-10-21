#ifndef NICHANGEPGRANDOMANICOMMAND_H
#define NICHANGEPGRANDOMANICOMMAND_H

#include <NiRefObject.h>
#include <NiSmartPointer.h>
#include "NiEntityCommandInterface.h"
#include "NiEntityInterface.h"

class NIENTITY_ENTRY NiChangePGRandomAniCommand : public NiRefObject,
    public NiEntityCommandInterface
{
public:
    NiChangePGRandomAniCommand(NiEntityInterface* pkEntity, 
        bool bPGRandomAni);

    NiEntityInterface* GetEntity() const;
    NiBool GetPGRandomAni() const;	

private:
    NiEntityInterfaceIPtr m_spEntity;
    bool m_bPGRandomAni;

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

NiSmartPointer(NiChangePGRandomAniCommand);

#include "NiChangePGRandomAniCommand.inl"

#endif
