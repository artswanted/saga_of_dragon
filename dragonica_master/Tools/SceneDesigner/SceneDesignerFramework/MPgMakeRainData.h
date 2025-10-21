#pragma once
namespace Emergent{ namespace Gamebryo{ namespace SceneDesigner{
    namespace Framework
{
	public __gc class MPgMakeRainData
	{
	public:
		MPgMakeRainData(void);
	public:
		virtual ~MPgMakeRainData(void);

	public:
		String *m_pkFilename;
		float m_fDotValue;
		ArrayList *m_pkIgnoreList;
		bool m_bIsBinaryMode;

	public:
		bool Initialize();
		void MakeRainDataToFile();
		void GetAllGeometries(const NiNode *pkNode, NiObjectList &kGeomerties);

	public:
		__property String* get_Filename();
		__property void set_Filename(String *pkFilename);
		__property float get_DotValue();
		__property void set_DotValue(float fRadian);
		__property ArrayList* get_IgnoreList();
		__property void set_IgnoreList(ArrayList *pkList);
		__property bool get_IsBinaryMode();
		__property void set_IsBinaryMode(bool bMode);

	};

}}}}