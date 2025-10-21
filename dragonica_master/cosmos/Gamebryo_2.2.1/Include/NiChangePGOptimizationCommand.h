#ifndef NICHANGEPGOPTIMIZATIONCOMMAND_H
#define NICHANGEPGOPTIMIZATIONCOMMAND_H

#include <NiRefObject.h>
#include <NiSmartPointer.h>
#include "NiEntityCommandInterface.h"
#include "NiEntityInterface.h"

class NIENTITY_ENTRY NiChangePGOptimizationCommand : public NiRefObject,
    public NiEntityCommandInterface
{
public:
    NiChangePGOptimizationCommand(NiEntityInterface* pkEntity, 
        const NiFixedString &kPGOptimization);

    NiEntityInterface* GetEntity() const;
	NiFixedString GetPGOptimization() const;

private:
    NiEntityInterfaceIPtr m_spEntity;
	NiFixedString m_kPGOptimization;

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

NiSmartPointer(NiChangePGOptimizationCommand);

#include "NiChangePGOptimizationCommand.inl"

#endif
