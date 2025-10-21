// EMERGENT GAME TECHNOLOGIES PROPRIETARY INFORMATION
//
// This software is supplied under the terms of a license agreement or
// nondisclosure agreement with Emergent Game Technologies and may not 
// be copied or disclosed except in accordance with the terms of that 
// agreement.
//
//      Copyright (c) 1996-2006 Emergent Game Technologies.
//      All Rights Reserved.
//
// Emergent Game Technologies, Chapel Hill, North Carolina 27517
// http://www.emergent.net

#ifndef NIENTITYSTREAMING_H
#define NIENTITYSTREAMING_H

#include "NiEntityLibType.h"
#include "NiRefObject.h"
#include "NiScene.h"
#include <NiTArray.h>

// Abstract class for dealing with various streaming methods
class NIENTITY_ENTRY NiEntityStreaming : public NiRefObject
{
public:
    NiEntityStreaming();
    virtual ~NiEntityStreaming();

    // Errors
    static NiFixedString ERR_OBJECT_CREATION_FAILED;
    static NiFixedString ERR_FILE_LOAD_FAILED;
    static NiFixedString ERR_FILE_PARSE_FAILED;
    static NiFixedString ERR_FILE_SAVE_FAILED;

	// Scene Management Functions.
	void InsertScene(NiScene* pkScene);
	void RemoveScene(NiScene* pkScene);
	void RemoveAllScenes();

	// 팔레트 폴더 리스트 관리.
	void InsertPaletteDirectory(NiFixedString strDir);
	void RemovePaletteDir(NiFixedString strDir);
	void RemoveAllPaletteDir();
    unsigned int GetPaletteDirectoryCount() const;
	const NiFixedString& GetPaletteDirectory(unsigned int ui) const;
	// 팔레트 이름.
	void InsertPaletteName(NiFixedString strName);
	void RemovePaletteName(NiFixedString strName);
	void RemoveAllPaletteName();
    unsigned int GetPaletteNameCount() const;
	const NiFixedString& GetPaletteName(unsigned int ui) const;
	// 팔레트 이름과 연동되는 인덱스
	void InsertPaletteIdx(NiFixedString strIdx);
	void RemovePaletteIdx(NiFixedString strIdx);
	void RemoveAllPaletteIdx();
    unsigned int GetPaletteIdxCount() const;
	const NiFixedString& GetPaletteIdx(unsigned int ui) const;


    unsigned int GetSceneCount() const;
	unsigned int GetEffectiveSceneCount() const;
	NiScene* GetSceneAt(unsigned int ui) const;

	// I/O Functions.
	virtual NiBool Load(const char* pcFileName) = 0;
	virtual NiBool Save(const char* pcFileName) = 0;

	// ProjectG GetProperty
	virtual NiFixedString GetPGProperty(const char* pcFileName, const char* pcEntityName) = 0;

    // Set Error Handler
    void SetErrorHandler(NiEntityErrorInterface* pkErrorIntf);

	// File Extension of format that can be load/saved.
	// Also used for registering factory so that different files
	// can be read and saved appropriately.
	virtual NiFixedString GetFileExtension() const = 0;
	virtual NiFixedString GetFileDescription() const = 0;

    // *** begin Emergent internal use only ***
    static void _SDMInit();
    static void _SDMShutdown();
    // *** end Emergent internal use only ***

protected:
    class NIENTITY_ENTRY IDLinkMaps
    {
    public:
        void RemoveLinks();

        void AssignID(NiEntityPropertyInterface* pkPropIntf);
        void AssignID(NiEntityPropertyInterface* pkPropIntf,
            unsigned int uiID);

        NiEntityPropertyInterface* GetAt(unsigned int uiID);
        NiBool GetID(NiEntityPropertyInterface* pkPropIntf,
            unsigned int& uiID);
        unsigned int GetIDCount();

    protected:
        NiTMap<NiEntityPropertyInterface*, unsigned int> m_kPropToIDMap;
        NiTMap<unsigned int, NiEntityPropertyInterfaceIPtr> m_kIDMap;
    };
   
    void RecurseEntityPointerProperties(IDLinkMaps& kMaps,
        NiEntityPropertyInterface* pkPropIntf);
    void AssignUniqueLinkIDs(IDLinkMaps& kMaps);

    void ReportError(const NiFixedString& kErrorMessage,
        const NiFixedString& kErrorDescription,
        const NiFixedString& kEntityName, const NiFixedString& kPropertyName);

    void MapToID(IDLinkMaps& kMaps, NiEntityPropertyInterface* pkPropIntf);
    void RecursiveMapToID(IDLinkMaps& kMaps, NiEntityInterface* pkEntity);
    void RecursiveMapToID(IDLinkMaps& kMaps, 
        NiEntityComponentInterface* pkComp);

	NiTObjectArray<NiScenePtr> m_kTopScenes;
    NiEntityErrorInterface* m_pkErrorHandler;
	NiTObjectArray<NiFixedString> m_kPaletteDirList;
	NiTObjectArray<NiFixedString> m_kPaletteNameList;
	NiTObjectArray<NiFixedString> m_kPaletteIdxList;
};

#endif // NIENTITYSTREAMING_H
