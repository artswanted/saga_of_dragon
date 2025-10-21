// Barunson Interactive Propritary Information

#pragma once

using namespace System::Collections;

class PgItem;
class PgItemMan;

namespace NiManagedToolInterface
{
    public __gc class MItem
    {
    public:
        MItem(PgItem *pkItem, PgItemMan *pkItemMan);
		MItem(String *pkXmlPath, PgItemMan *pkItemMan);
		MItem(String *pkItemID, String *pkNIFPath);

		~MItem();
		
		bool ParseItemXml(int iGender, int iClass, bool bForceParsing);

        // Properties.
        __property String *get_Name();
		__property void set_Name(String *kID);
        __property String *get_Path();
		__property String *get_NIFPath();
		__property String *get_KFMPath();
		__property int get_ItemPos();

		__property bool get_Attached();
		__property void set_Attached(bool bAttached);

		ArrayList *GetSrcTexture();
		String *GetDestTexture(String *pkSrcTexture);
		bool SetDestTexture(String *pkSrcTexture, String *pkDestTexture);
		bool IsExistsTexture(String *pkDestTexture);

		bool WriteToXml(String *pkXmlPath, bool bOverwrite);
		bool ChangeMesh(String *pkNifPath);
		bool ChangeActorManager(String *pkKfmPath);

		PgItem *GetItem(int iGender, int iClass);
		void SetItem(PgItem *pkItem);
		

    protected:
		PgItem *m_pkItem;
		PgItemMan *m_pkItemMan;
		String *m_pkXmlPath;
    };
}