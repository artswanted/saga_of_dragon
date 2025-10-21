#ifndef NICHANGEPGUSEPHYSXCOMMAND_H
#define NICHANGEPGUSEPHYSXCOMMAND_H

#include <NiRefObject.h>
#include <NiSmartPointer.h>
#include "NiEntityCommandInterface.h"
#include "NiEntityInterface.h"

class NIENTITY_ENTRY NiChangePGUsePhysXCommand : public NiRefObject,
    public NiEntityCommandInterface
{
public:
    NiChangePGUsePhysXCommand(NiEntityInterface* pkEntity, 
        bool bPGUsePhysX);

    NiEntityInterface* GetEntity() const;
    NiBool GetPGUsePhysX() const;	

private:
    NiEntityInterfaceIPtr m_spEntity;
    bool m_bPGUsePhysX;

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

NiSmartPointer(NiChangePGUsePhysXCommand);

#include "NiChangePGUsePhysXCommand.inl"

#endif
