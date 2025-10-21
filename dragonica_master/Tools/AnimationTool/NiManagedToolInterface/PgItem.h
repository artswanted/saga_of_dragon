#pragma once

using namespace System::Collections;

class PgItem : public NiMemObject
{
public:

	typedef enum WeaponAttachPos
	{
		UNKNOWN = 0,
		LEFT_HAND = 1,
		RIGHT_HAND = 2,
		LEFT_FOREARM = 3,
		RIGHT_FOREARM = 4
	} WeaponAttachPos;

	class TexturePair
	{
	public:
		TexturePair(std::string kSrcTexture);
		TexturePair(std::string kSrcTexture, std::string kDestTexture);

		std::string m_kSrcTexture;
		std::string m_kDestTexture;

		bool operator ==(const TexturePair &rhs);
	};

	typedef std::set<NiAVObjectPtr> AttachedNodes;
	typedef std::vector<TexturePair> TexturePathContainer;
	typedef std::multimap<std::string, NiTexturingProperty *> TexturePropertyContainer;

	PgItem();
	~PgItem();

	//! Xml을 파싱한다.
	static PgItem *CreateItemFromXmlFile(const char *pcPath, int iGender, int iClass = 0, PgItem *pkItem = 0);

	//! id, nif, kfm를 받아서 PgItem Object를 만든다.
	static PgItem *CreateItem(const char *pcItemID, 
		const char *pcNIFPath, 
		const char *pcKFMPath = 0,
		int iWeapon = 0,
		int iFilter = 0
		);

	//! 초기화 한다.
	bool Initialize();
	
	//! ID를 반환한다.
	const char *GetID();

	//! ID를 셋팅한다.
	void SetID(const char *pcID);

	//! 메쉬 루트 노드를 반환한다.
	NiNodePtr GetMeshRoot();

	//! 무기인가?
	bool IsWeapon();

	//! 무기 종류를 반환한다.
	unsigned int GetWeaponType();

	//! 아이템 타입을 반환한다.
	int ItemPos();

	//! 파츠에 애니가 있는가?
	bool IsAvailableAnimation();

	//! ActorManager를 반환한다.
	NiActorManagerPtr GetActorManager();

	//! 아이템의 Path를 반환한다.
	const char *GetPath();

	//! Attch Point 를 반환한다.
	const char *GetTargetPoint();

	//! KFM의 Path를 반환한다.
	const char *GetKFMPath();

	//! Mesh의 Path를 반환한다.
	const char *GetMeshPath();

	//! 아이템이 Attach되어 있는지 안되어있는지 알려준다.
	bool IsAttached();

	//! 아이템을 Attach했는지 안했는지 설정한다.
	void SetAttached(bool bAttached);

	//! 떼어져야 할 노드들을 추가한다.
	void AddAttachedNode(NiAVObject *pkNode);

	//! 떼어져야 할 노드를 삭제 한다.
	void RemoveAttachedNode(NiAVObject *pkNode);

	//! 떼어져야 할 노드들을 반환한다.
	NiAVObject *GetAttachedNode();

	//! 추가로 붙어 있는 아이템이 있으면 반환한다.
	PgItem *GetAdditionalItem();

	//! SrcTexture를 반환한다.
	ArrayList *GetSrcTexture();

	//! SrsTexture에 해당하는 DestTexture경로를 반환한다.
	String *GetDestTexture(String *pkSrcTexture);

	//! SrcTexture에 대한 DestTexture를 설정한다.
	bool SetDestTexture(String *pkSrcTexture, String *pkDestTexture);

	//! DestTexture가 존재하는지 체크
	bool IsExistsTexture(String *pkDestTexture);

	//! RefCount를 하나 증가시킨다.
	void IncRefCount();

	//! RefCount를 하나 감소시킨다.
	void DecRefCount();

	//! RecCount를 반환한다.
	unsigned int GetRefCount();

	//! 현재 Item을 Xml파일로 쓴다.
	bool WriteToXml(String *pkXmlPath, bool bOverwrite = false);

	//! Actor Manager을 교체한다.
	bool PgItem::ChangeActorManager(const char *pcAMPath);

	//! Mesh를 교체한다.
	bool PgItem::ChangeMesh(const char *pcNIFPath);

	//! Get Gender
	int GetGender();

	//! Get Class
	int GetClass();

private:

	//! 텍스쳐를 적용한다.
	bool ApplyTexture();

	//! Source Texture아서, 그 목록으로 컨테이너를 작성한다.
	bool FindSourceTexture(NiNode *pkRoot);

	//! 메쉬 루트
	NiNodePtr m_spMeshRoot;
	
	//! 무기인가
	bool m_bWeapon;

	//! 무기 종류
	int m_iWeaponType;

	//! 아이템 타입(포션, 포탈, 아이템 등)
	int m_iItemType;

	//! 아이템 착용 위치
	int m_iItemPos;

	//! 아이템 필터 
	int m_iItemFilter;

	//! Gender
	int m_iGender;

	//! Class
	int m_iClass;

	//! Weapon Attach Pos
	WeaponAttachPos m_eWeaponAttachPos;

	//! ID
	std::string m_kID;

	//! KFM Path
	std::string m_kKFMPath;
	
	//! Path
	std::string m_kPath;

	//! 메쉬 패스
	std::string m_kMeshPath;

	//! 텍스쳐 목록
	TexturePathContainer m_kTextureContainer;

	//! Source Texture 목록
	TexturePropertyContainer m_kSourceTextureContainer;

	//! ActorManager
	NiActorManagerPtr m_spAM;
	
	//! 파츠를 붙였을 때, 떼어져야 할 노드 리스트이다.
	AttachedNodes m_kAttachedNodes;

	//! Attached
	bool m_bAttached;

	//! Ref Count
	unsigned int m_uiRefCount;

	//! Additional Item
	PgItem *m_pkAdditionalParts;

	//! Target Point
	std::string m_kTargetPoint;
};