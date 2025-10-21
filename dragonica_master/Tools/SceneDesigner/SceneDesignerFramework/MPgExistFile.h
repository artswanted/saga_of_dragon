#pragma once

#include <string>

namespace Emergent{ namespace Gamebryo{ namespace SceneDesigner{
    namespace Framework
{
	public __gc class ExistFiles : public Object
	{
	public:
		String *m_pkFilePath;
		MEntity* m_pkEntity;

	public:
		ExistFiles(){}
		~ExistFiles(){}
	};
}}}}

namespace Emergent{ namespace Gamebryo{ namespace SceneDesigner{
    namespace Framework
{
	public __gc class MPgExistFile
	{
	public:
		bool m_bToRefreshList;
		ArrayList *m_pmNotExistFiles;		// 화면에 찍힌 물체들 중..
		ArrayList *m_pmNotExistFiles2;		// 팔레트 물체들 중..

	public:
		MPgExistFile(void);
		virtual ~MPgExistFile(void);

	public:
		bool Initialize();
		void AddNotExistFile(String *pkFilePath, MEntity* pkEntity);
		void AddNotExistFile2(String *pkFilePath, MEntity* pkEntity);
		unsigned int GetNotExistFileCount();
		unsigned int GetNotExistFile2Count();
		ExistFiles* GetNotExistFile(unsigned int uiIdx, bool bPop);
		ExistFiles* GetNotExistFile2(unsigned int uiIdx, bool bPop);
		void ClearNotExistFile();
		bool FindNotExistFile();

		// PG Property에서 설정된 항목을 Refresh(PostfixTexture)
		void RefreshPostfixTexture();

	};
}}}}
