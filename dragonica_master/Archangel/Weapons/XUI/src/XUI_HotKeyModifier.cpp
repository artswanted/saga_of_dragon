#include "StdAfx.h"
#include "XUI_HotKeyModifier.h"

using namespace XUI;

CXUI_HotKeyModifier::CXUI_HotKeyModifier(void)
{
}

CXUI_HotKeyModifier::~CXUI_HotKeyModifier(void)
{
	Clear();
}

bool CXUI_HotKeyModifier::AddModifierKey(int KeyID, std::wstring const& KeyName)
{
	tagModifierKeyInfo	Info(KeyID, KeyName);

	kNumberArrayContainer::iterator	id_iter = m_kNumberArrayCont.find(KeyID);
	kStringArrayContainer::iterator	str_iter = m_kStringArrayCont.find(KeyName);
	
	if( m_kNumberArrayCont.end() == id_iter
	&&	m_kStringArrayCont.end() == str_iter )
	{
		kModifierKeyContainer::iterator rst_iter = m_kModifierKeyCont.insert(m_kModifierKeyCont.end(), Info);
		m_kNumberArrayCont.insert(std::make_pair( KeyID, rst_iter ));
		m_kStringArrayCont.insert(std::make_pair( KeyName, rst_iter ));
		return true;
	}

	return false;
}

void CXUI_HotKeyModifier::Clear()
{
	m_kNumberArrayCont.clear();
	m_kStringArrayCont.clear();
	m_kModifierKeyCont.clear();
}

std::wstring const CXUI_HotKeyModifier::GetModifierName()
{
	std::wstring Name(L"");

	kModifierKeyContainer::iterator iter = m_kModifierKeyCont.begin();
	while( iter != m_kModifierKeyCont.end() )
	{
		if( (*iter).m_kKeyState )
		{
			Name += iter->m_kKeyName;
		}
		++iter;
	}

	return Name;
}

bool CXUI_HotKeyModifier::SetModifierState(int KeyID, bool State)
{
	kNumberArrayContainer::iterator iter = m_kNumberArrayCont.find(KeyID);
	if( m_kNumberArrayCont.end() != iter )
	{
		iter->second->m_kKeyState = State;
		return true;
	}
	return false;
}

bool CXUI_HotKeyModifier::SetModifierState(std::wstring const& KeyName, bool State)
{
	kStringArrayContainer::iterator	iter = m_kStringArrayCont.find(KeyName);
	if( m_kStringArrayCont.end() != iter )
	{
		iter->second->m_kKeyState = State;
		return true;
	}
	return false;
}

void CXUI_HotKeyModifier::ReSetModifierState()
{
	kModifierKeyContainer::iterator	iter = m_kModifierKeyCont.begin();
	while( m_kModifierKeyCont.end() != iter )
	{
		iter->m_kKeyState = false;
		++iter;
	}
}

bool CXUI_HotKeyModifier::GetPushModifierState()
{
	kModifierKeyContainer::iterator iter = m_kModifierKeyCont.begin();
	while( iter != m_kModifierKeyCont.end() )
	{
		if( (*iter).m_kKeyState )
		{
			return true;
		}
		++iter;
	}
	return false;	
}

bool CXUI_HotKeyModifier::IsModifier(int const iKey)
{
	kNumberArrayContainer::iterator iter = m_kNumberArrayCont.find(iKey);
	return ( m_kNumberArrayCont.end() != iter )?(true):(false);
}