#ifndef WEAPON_XUI_XUI_MANAGER_INL
#define WEAPON_XUI_XUI_MANAGER_INL

	class Align_Func
	{
	public:
		Align_Func(){}
		
		template< typename T1 >
		void operator ()(T1 &t1)
		{
			(t1.second)->VAlign();
		}
	};

	class DisplayActive_Func//Active 목록의 for_each
	{
	public:
		DisplayActive_Func(){}
		
		template< typename T1 >
		void operator ()(T1 &t1)
		{
			t1->VDisplay();
		}
	};

	class Init_Func
	{
	public:
		Init_Func(){}
		
		template< typename T1 >
		void operator ()(T1 &t1)
		{
			(t1.second)->VInit();
		}
	};

	class Scale_Func
	{
	public:
		Scale_Func(float const fScale):m_fScale(fScale){}
		
		template< typename T1 >
		void operator ()(T1 &t1)
		{
			(t1.second)->VScale(m_fScale);
		}
		float const m_fScale;
	};

#endif // WEAPON_XUI_XUI_MANAGER_INL