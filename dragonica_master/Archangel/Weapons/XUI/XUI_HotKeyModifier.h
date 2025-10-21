#ifndef WEAPON_XUI_XUI_HOTKEYMODIFIER_H
#define WEAPON_XUI_XUI_HOTKEYMODIFIER_H

#include <string>

namespace XUI
{
	typedef struct	tagModifierKeyInfo
	{
		tagModifierKeyInfo(DWORD KeyID, std::wstring keyName)
			: m_kKeyID(KeyID), m_kKeyName(keyName), m_kKeyState(false) {}
		~tagModifierKeyInfo()
		{
			m_kKeyName.clear();
		}
		DWORD	m_kKeyID;
		std::wstring m_kKeyName;
		bool	m_kKeyState;
	}SModifierKeyInfo;

	class CXUI_HotKeyModifier
	{
	protected:
		typedef	std::list< SModifierKeyInfo >	kModifierKeyContainer;
		typedef std::map< int, kModifierKeyContainer::iterator > kNumberArrayContainer;
		typedef std::map< std::wstring, kModifierKeyContainer::iterator > kStringArrayContainer;

		kModifierKeyContainer	m_kModifierKeyCont;
		kNumberArrayContainer	m_kNumberArrayCont;
		kStringArrayContainer	m_kStringArrayCont;

	public:
		bool IsModifier(int const iKey);
		bool AddModifierKey(int KeyID, std::wstring const& KeyName);
		bool SetModifierState(std::wstring const& KeyName, bool State);
		bool SetModifierState(int KeyID, bool State);
		std::wstring const GetModifierName();
		void ReSetModifierState();
		bool GetPushModifierState();
		void Clear();

		CXUI_HotKeyModifier(void);
		virtual ~CXUI_HotKeyModifier(void);
	};
}

#endif // WEAPON_XUI_XUI_HOTKEYMODIFIER_H