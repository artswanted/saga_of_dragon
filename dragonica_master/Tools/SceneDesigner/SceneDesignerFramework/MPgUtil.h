#pragma once

#include "IComponentService.h"

namespace Emergent{ namespace Gamebryo{ namespace SceneDesigner{
    namespace Framework
{
	public __gc class MPgUtil
	{
	public:
		MPgUtil(void);
		virtual ~MPgUtil(void);

	public:
		bool Initialize();

	public:
		//Math func
		static bool IsEqualF(float const fLhs, float const fRhs, float const fTol);
		static String* ConvertAbsPathToRelPath(String* pStartPath, String* pEndPath);
		static void SetTimeToAniObj(NiObjectNET* pkObj, float &rfTime);
		static void ApplyPropertyData(MEntity* pmEntity, String* strPropertyName,
			Object* pmData);
		bool HasTimeController(NiAVObject *pkAVObject);
		bool AddAniComponentToPalettes();
		bool AddAniComponentToEntity(MEntity* pmEntity, MComponent* pmComponent, bool bIsPalettesEntity);
		bool AddHideObjectComponentToEntity(MEntity* pmEntity, MComponent* pmComponent, bool bIsPalettesEntity);
		bool SyncObjectToPalettes();
		void GetAllTriangleCount(int& _nTriangleCount, int& _nObjectCount);
		void UpdateTriangleCount();

		void RecursiveAnimationType(NiAVObject* pkObject);
		void ChangeAnimationType();
		NiCamera* FindCamera(NiAVObject* pObject);

		std::wstring ConvToUnicode(std::string const& strSrc);
		errno_t static ConvToUnicode(std::string const &strSrc, std::wstring& wstrTgt);
		std::string ConvToMultiByte(std::wstring const &wstrSrc);
		errno_t ConvToMultiByte(std::wstring const& wstrSrc, std::string &strTgt);

	protected:
		__property static IComponentService* get_ComponentService();
		static IComponentService* ms_pmComponentService;
	};
}}}}
