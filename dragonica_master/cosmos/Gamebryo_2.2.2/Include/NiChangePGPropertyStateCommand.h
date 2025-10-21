#ifndef NICHANGEPGPROPERTYSTATECOMMAND_H
#define NICHANGEPGPROPERTYSTATECOMMAND_H

#include <NiRefObject.h>
#include <NiSmartPointer.h>
#include "NiEntityCommandInterface.h"
#include "NiEntityInterface.h"

class NIENTITY_ENTRY NiChangePGPropertyStateCommand : public NiRefObject,
    public NiEntityCommandInterface
{
public:
    NiChangePGPropertyStateCommand(NiEntityInterface* pkEntity, 
        NiFixedString &kPGPropertyName);

    NiEntityInterface* GetEntity() const;
	NiFixedString GetPGProperty() const;

private:
    NiEntityInterfaceIPtr m_spEntity;
	NiFixedString m_kPGPropertyName;

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

NiSmartPointer(NiChangePGPropertyStateCommand);

#include "NiChangePGPropertyStateCommand.inl"

#endif
