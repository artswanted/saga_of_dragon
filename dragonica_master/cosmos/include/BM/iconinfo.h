#pragma once
#pragma pack(1)

typedef struct stSIconInfo
{
	stSIconInfo() 
	{
		Clear(); 
	}

	explicit stSIconInfo(unsigned int iGroup, unsigned int iKey) 
	{
		Clear(); 
		iIconGroup = iGroup; 
		iIconKey = iKey; 
	}
	
	~stSIconInfo() 
	{
	}

	void Clear() 
	{ 
		iIconKey = 0;		// 예전의 y, IconKey는 0는 없다는 뜻이다.
		iIconGroup = 0;			// 예전의 x, Item.h의 eKindUIIconGroup 를 넣는다. default값은 KUIG_NONE 값과 같아야 한다.
		iIconResNumber = 0;

		// 안쓸지도 모르지만, 성능 개선을 위해서 만든 변수
		pkData = NULL;
		pkItemInfo = NULL;
		bIconNeedToUpdate = false;
		bIconShow = false;
		bIconAttachtedToCursor = false;
	}

	unsigned int iIconKey;		// 예전의 y, IconKey는 0는 없다는 뜻이다.
	unsigned int iIconGroup;			// 예전의 x, Item.h의 eKindUIIconGroup 를 넣는다. default값은 KUIG_NONE 값과 같아야 한다.
	unsigned int iIconResNumber;

	// 안쓸지도 모르지만, 성능 개선을 위해서 만든 변수
	void* pkData;
	void* pkItemInfo;
	bool bIconNeedToUpdate;
	bool bIconShow;
	bool bIconAttachtedToCursor;
}SIconInfo;

#pragma pack()