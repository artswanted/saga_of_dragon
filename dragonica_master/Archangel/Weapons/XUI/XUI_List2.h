#ifndef WEAPON_XUI_XUI_LIST2_H
#define WEAPON_XUI_XUI_LIST2_H

#include "XUI_Form.h"
#include "BM/ObjectPool.h"

namespace XUI
{
	class CXUI_List2_Item
	{
		friend class CXUI_List2;

	public:
		explicit CXUI_List2_Item( BM::GUID const& kOwnerGuid )
			:	m_kOwnerGuid(kOwnerGuid)
			,	m_pWnd(NULL)
		{}

		virtual ~CXUI_List2_Item()
		{
			SAFE_DELETE( m_pWnd );
		}

		CXUI_Wnd* GetWnd()const{return m_pWnd;}
		BM::GUID const OwnerGuid()const{return m_kOwnerGuid;}

		void Close()
		{
			if ( m_pWnd && !m_pWnd->IsClosed() )
			{
				m_pWnd->Close();
			}
		}

		bool IsClosed()const
		{
			if ( m_pWnd )
			{
				return m_pWnd->IsClosed();
			}
			return false;
		}

		void Visible( bool const bVisible )
		{	
			if ( m_pWnd )
				m_pWnd->Visible( bVisible );
		}

		virtual void Refresh(){}
		virtual void SetSelect(){}
		virtual void DelSelect(){}

	protected:
		BM::GUID	m_kOwnerGuid;
		CXUI_Wnd*	m_pWnd;

	private:
		CXUI_List2_Item();
	};

	template< typename T >
	class CXUI_List2_CreatePolicy
	{
	public:
		static T* Create( BM::GUID const &kOwnerGuid )
		{
			return new T(kOwnerGuid);
		}

		virtual void Set( T* pItem ){}
	};

	class CXUI_List2
		:	public CXUI_Form
	{
	public:
		typedef std::list<CXUI_List2_Item*>				CONT_LIST;
		typedef std::map< BM::GUID, CXUI_List2_Item* >	CONT_MAP;

	public:
		class CExport
		{
			friend class CXUI_List2;
		public:
			CExport(void){}
			~CExport()
			{
				CONT_LIST::iterator display_itr = m_kContList.begin();
				for ( ; display_itr!=m_kContList.end() ; ++display_itr )
				{
					SAFE_DELETE( *display_itr );
				}
			}

		private:
			CONT_LIST	m_kContList;
			CONT_MAP	m_kContMap;
			BM::GUID	m_kSelectedItemGuid;
		};

	public:
		CXUI_List2();
		virtual ~CXUI_List2();
		virtual E_XUI_TYPE VType(){return E_XUI_LIST2;}

		virtual CXUI_Wnd* VCreate()const{return new CXUI_List2;}
		virtual void VRegistAttr(std::wstring const& wstrName, std::wstring const& wstrValue);

		virtual void VInit();

		virtual bool VOnTick( DWORD const dwCurTime );	
		virtual bool VDisplay();
		virtual bool VDisplayEnd(){return true;}
		virtual bool VPeekEvent(E_INPUT_EVENT_INDEX const& rET, POINT3I const& rPT, DWORD const& dwValue);
		virtual void VRefresh();
		virtual void RemoveAllControls(bool const bIsClosed = false);

		void ClearList();

		bool AdjustMiddleBtnPos();
		bool AdjustMiddleBarSize();

		CLASS_DECLARATION_S(std::wstring, ScrUpName);
		CLASS_DECLARATION_S(std::wstring, ScrDownName);
		CLASS_DECLARATION_S(std::wstring, ScrMiddleName);
		CLASS_DECLARATION_S(std::wstring, ScrBarName);

		CLASS_DECLARATION_S( int, ScrollValue );
		CLASS_DECLARATION( POINT2, m_ItemPos, ItemPos );
		CLASS_DECLARATION( POINT2, m_ItemSize, ItemSize );

		CLASS_DECLARATION_S(std::wstring, ElementID);//리스트에 들어가는 컨트롤의 아이디

		CLASS_DECLARATION_S( int, ItemHeight );

	public:
		template< typename T, template<typename> class CreationPolicy>
		T* AddItem( BM::GUID const& kOwnerGuid, CreationPolicy<T> CreatePolicy )
		{
			T *pItem = NULL;
			CONT_MAP::iterator itr = m_kContMap.find( kOwnerGuid );
			if ( itr != m_kContMap.end() )
			{
				pItem = dynamic_cast<T*>(itr->second);
				if ( pItem )
				{
					CreatePolicy.Set( pItem );
					pItem->Refresh();
				}

				return pItem;
			}

			pItem = CreationPolicy<T>::Create( kOwnerGuid );
			if ( pItem )
			{
				CreatePolicy.Set( pItem );
				m_kContList.push_back( pItem );
				m_kContMap.insert( std::make_pair( kOwnerGuid, pItem ) );

				InitItem( pItem );
				AdjustMiddleBtnPos();
				m_bUpdateStartPos = true;
			}
			assert(pItem);
			return pItem;
		}

		template<class T>
		void Sort( T _Pred )
		{
			m_kContList.sort( _Pred );
			m_bUpdateStartPos = true;
		}

		CXUI_List2_Item* GetItem( BM::GUID const& kOwnerGuid );
		bool DeleteItem( BM::GUID const& kOwnerGuid );
		bool SetSelect( BM::GUID const& kOwnerGuid );
		BM::GUID const GetSelecteGuid()const{return m_kSelectedItemGuid;}

		CONT_LIST::const_iterator Begin(void)const{return m_kContList.begin();}
		CONT_LIST::const_iterator End(void)const{return m_kContList.end();}

		const size_t GetSize()const{return m_kContMap.size();}

		void Export( CExport &kExport );
		void Import( CExport &kExport );

		void SetDisplayStartItem( size_t iIndex );

	private:
		CXUI_Wnd* InitItem( CXUI_List2_Item* pItem );
		void SetDisplayStartPos( int const iStartPos );

	protected:
		virtual void InvalidateChild(){}
		virtual	void SetBoundingBox(POINT2 const& ptLocation, POINT2 const& ptSize);

	protected:
		CXUI_Wnd*			m_pWndUp;
		CXUI_Wnd*			m_pWndDown;
		CXUI_Wnd*			m_pWndMiddle;
		CXUI_Wnd*			m_pWndBar;

		CONT_LIST			m_kContList;
		CONT_CONTROL		m_kContDrawList;
		CONT_MAP			m_kContMap;
		BM::GUID			m_kSelectedItemGuid;

		int					m_iDisplayStartPos;//픽셀단위 디스플레이 시작 포지션
		bool				m_bUpdateStartPos;//
	};
};

#endif // WEAPON_XUI_XUI_LIST2_H