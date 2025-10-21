/*****************************************************************

    MODULE    : FxStudioFxObjectInterfaceBase.h

    PURPOSE   : Implements most of an Emergent component which wraps an FxStudio Fx.

    CREATED   : 9/24/2008 3:46:22 PM

    COPYRIGHT : (C) 2008 Aristen, Inc. 

*****************************************************************/

#ifndef FXSTUDIOFXOBJECTINTERFACEBASE_H
#define FXSTUDIOFXOBJECTINTERFACEBASE_H

#include "FxStudioReferenceLibType.h"

#include <NiRefObject.h>
#include <NiSmartPointer.h>
#include <NiEntityComponentInterface.h>
#include <NiFlags.h>

NiSmartPointer(FxStudioFxObjectInterfaceBase);

class FXSTUDIOREFERENCE_ENTRY FxStudioFxObjectInterfaceBase : public NiRefObject, public NiEntityComponentInterface
{
    NiDeclareFlags(unsigned char);

public :

	static const NiUniqueID ms_kUniqueID;

public:

	// These functions should be called before using this class.
	static void InitializeStatics();
	static void TerminateStatics();

    FxStudioFxObjectInterfaceBase();
	virtual ~FxStudioFxObjectInterfaceBase() {}

    virtual void SetBankFilename(const NiFixedString& kBankFilename);
    const NiFixedString& GetBankFilename() const;

    const NiFixedString& GetBankName() const;

    void SetFxId(unsigned int uiFxId);
    unsigned int GetFxId() const;

	void SetStartPlaying(bool bPlaying);
	bool GetStartPlaying() const;

	void SetRandomizeStartTime(bool bRandomize);
	bool GetRandomizeStartTime() const;

	void SetAutoRestart(bool bAutoRestart);
	bool GetAutoRestart() const;

	void SetMinRestartDelay(float fDelay);
	float GetMinRestartDelay() const;

	void SetMaxRestartDelay(float fDelay);
	float GetMaxRestartDelay() const;

	virtual NiObject* GetRootScenePointer() const = 0;

public:
    // NiEntityComponentInterface overrides.
    virtual NiEntityComponentInterface* Clone(bool bInheritProperties) = 0;
    virtual NiEntityComponentInterface* GetMasterComponent();
    virtual NiEntityComponentInterface* GetMasterComponent() const;
    virtual void SetMasterComponent(NiEntityComponentInterface* pkMasterComponent);
    virtual void GetDependentPropertyNames(NiTObjectSet<NiFixedString>& kDependentPropertyNames);

    // NiEntityPropertyInterface overrides.
    virtual void AddReference();
    virtual void RemoveReference();
    virtual NiFixedString GetClassName() const;
    virtual NiUniqueID GetTemplateID();
    virtual NiBool SetTemplateID(const NiUniqueID& kID);
    virtual NiFixedString GetName() const;
    virtual NiBool SetName(const NiFixedString& kName);
    virtual NiBool IsAnimated() const;

	// These two functions handle the behavior of the component and should be overridden for the appropriate class.
    virtual void Update(NiEntityPropertyInterface* pkParentEntity, float fTime, NiEntityErrorInterface* pkErrors, NiExternalAssetManager* pkAssetManager) = 0;
    virtual void BuildVisibleSet(NiEntityRenderingContext* pkRenderingContext, NiEntityErrorInterface* pkErrors) = 0;

    virtual void GetPropertyNames(NiTObjectSet<NiFixedString>& kPropertyNames) const;
    virtual NiBool CanResetProperty(const NiFixedString& kPropertyName, bool& bCanReset) const;
    virtual NiBool ResetProperty(const NiFixedString& kPropertyName);
    virtual NiBool MakePropertyUnique(const NiFixedString& kPropertyName, bool& bMadeUnique);
    virtual NiBool GetDisplayName(const NiFixedString& kPropertyName, NiFixedString& kDisplayName) const;
    virtual NiBool SetDisplayName(const NiFixedString& kPropertyName, const NiFixedString& kDisplayName);
    virtual NiBool GetPrimitiveType(const NiFixedString& kPropertyName, NiFixedString& kPrimitiveType) const;
    virtual NiBool SetPrimitiveType(const NiFixedString& kPropertyName, const NiFixedString& kPrimitiveType);
    virtual NiBool GetSemanticType(const NiFixedString& kPropertyName, NiFixedString& kSemanticType) const;
    virtual NiBool SetSemanticType(const NiFixedString& kPropertyName, const NiFixedString& kSemanticType);
    virtual NiBool GetDescription(const NiFixedString& kPropertyName, NiFixedString& kDescription) const;
    virtual NiBool SetDescription(const NiFixedString& kPropertyName, const NiFixedString& kDescription);
    virtual NiBool GetCategory(const NiFixedString& kPropertyName, NiFixedString& kCategory) const;
    virtual NiBool IsPropertyReadOnly(const NiFixedString& kPropertyName, bool& bIsReadOnly);
    virtual NiBool IsPropertyUnique(const NiFixedString& kPropertyName, bool& bIsUnique);
    virtual NiBool IsPropertySerializable(const NiFixedString& kPropertyName, bool& bIsSerializable);
    virtual NiBool IsPropertyInheritable(const NiFixedString& kPropertyName, bool& bIsInheritable);
    virtual NiBool IsExternalAssetPath(const NiFixedString& kPropertyName, unsigned int uiIndex, bool& bIsExternalAssetPath) const;
    virtual NiBool GetElementCount(const NiFixedString& kPropertyName, unsigned int& uiCount) const;
    virtual NiBool SetElementCount(const NiFixedString& kPropertyName, unsigned int uiCount, bool& bCountSet);
    virtual NiBool IsCollection(const NiFixedString& kPropertyName, bool& bIsCollection) const;

	virtual NiBool GetPropertyData(const NiFixedString& kPropertyName, NiFixedString& kData, unsigned int uiIndex = 0) const;
	virtual NiBool SetPropertyData(const NiFixedString& kPropertyName, const NiFixedString& kData, unsigned int uiIndex = 0);

    virtual NiBool GetPropertyData(const NiFixedString& kPropertyName, bool& bData, unsigned int uiIndex = 0) const;
    virtual NiBool SetPropertyData(const NiFixedString& kPropertyName, bool  bData, unsigned int uiIndex = 0);

    virtual NiBool GetPropertyData(const NiFixedString& kPropertyName, unsigned int& uiData, unsigned int uiIndex = 0) const;
    virtual NiBool SetPropertyData(const NiFixedString& kPropertyName, unsigned int uiData, unsigned int uiIndex = 0);

    virtual NiBool GetPropertyData(const NiFixedString& kPropertyName, float& fData, unsigned int uiIndex = 0) const;
    virtual NiBool SetPropertyData(const NiFixedString& kPropertyName, float  fData, unsigned int uiIndex = 0);

	virtual NiBool GetPropertyData(const NiFixedString& kPropertyName, NiObject*& pkData, unsigned int uiIndex) const;

protected:

	// Construction from a master component should only be done
	// internally.
    FxStudioFxObjectInterfaceBase(FxStudioFxObjectInterfaceBase* pkMasterComponent);

	// This function should only be called from internal code
	// to so that the bank name is always set from the bank filename.  
	void SetBankName(const NiFixedString& kBankName);

	// Used to copy all member variables from the source object.
	void CopyProperties(const FxStudioFxObjectInterfaceBase& source);

    // Error strings.
    static NiFixedString ERR_TRANSLATION_NOT_FOUND;
    static NiFixedString ERR_ROTATION_NOT_FOUND;
	static NiFixedString ERR_FXCREATION_FAILED;

	// Class name.
    static NiFixedString ms_kClassName;

    // Component name.
    static NiFixedString ms_kComponentName;

    // Property names.
	static NiFixedString ms_kBankFilenameProperty;
    static NiFixedString ms_kBankNameProperty;
    static NiFixedString ms_kFxIdProperty;
    static NiFixedString ms_kStartPlayingProperty;
	static NiFixedString ms_kRandomizeStartTimeProperty;
	static NiFixedString ms_kAutoRestartProperty;
	static NiFixedString ms_kMinRestartDelayProperty;
	static NiFixedString ms_kMaxRestartDelayProperty;
	static NiFixedString ms_kSceneRootPointerProperty;

    // Property descriptions.
	static NiFixedString ms_kBankFilenameDescription;
    static NiFixedString ms_kBankNameDescription;
    static NiFixedString ms_kFxIdDescription;
	static NiFixedString ms_kStartPlayingDescription;
	static NiFixedString ms_kRandomizeStartTimeDescription;
	static NiFixedString ms_kAutoRestartDescription;
	static NiFixedString ms_kMinRestartDelayDescription;
	static NiFixedString ms_kMaxRestartDelayDescription;

    // Dependent property names.
    static NiFixedString ms_kRotationProperty;
	static NiFixedString ms_kTranslationProperty;
	static NiFixedString ms_kScaleProperty;

private :

    // Flags.
    enum
    {
		BANK_FILENAME_UNIQUE_MASK = 1 << 0,
        FX_ID_UNIQUE_MASK     = 1 << 1,
		START_PLAYING_UNIQUE_MASK = 1 << 2,
		RANDOMIZE_START_TIME_UNIQUE_MASK = 1 << 3,
		AUTO_RESTART_UNIQUE_MASK = 1 << 4,
		MIN_RESTART_DELAY_UNIQUE_MASK = 1 << 5,
		MAX_RESTART_DELAY_UNIQUE_MASK = 1 << 6
    };
    bool GetBankFilenameUnique() const;
    bool GetFxIdUnique() const;

    // Master component.
    FxStudioFxObjectInterfaceBasePtr m_spMasterComponent;

    // Properties.
	NiFixedString	m_kBankFilename;
	NiFixedString	m_kBankName;
	unsigned int	m_kFxId;
	bool			m_bStartPlaying;
	bool			m_bRandomizeStartTime;
	bool			m_bAutoRestart;
	float			m_fMinRestartDelay;
	float			m_fMaxRestartDelay;

};

#endif  // #ifndef FXSTUDIOFXOBJECTINTERFACEBASE_H
