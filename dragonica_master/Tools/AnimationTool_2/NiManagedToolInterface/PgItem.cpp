#include "stdafx.h"
#include "PgItem.h"
#include "PgAMPool.h"
#include "PgNifMan.h"
#using <System.Xml.dll>

using namespace System::Xml;

PgItem::TexturePair::TexturePair(std::string kSrcTexture) : 
	m_kSrcTexture(kSrcTexture)
{
}

PgItem::TexturePair::TexturePair(std::string kSrcTexture, std::string kDestTexture) :
	m_kSrcTexture(kSrcTexture), 
	m_kDestTexture(kDestTexture)
{
}

bool PgItem::TexturePair::operator ==(const TexturePair &rhs)
{
	return m_kSrcTexture == rhs.m_kSrcTexture;
}

PgItem::PgItem() :
	m_spMeshRoot(0),
	m_iItemType(0),
	m_iItemPos(0),
	m_iItemFilter(0),
	m_spAM(0),
	m_bAttached(0),
	m_iWeaponType(0),
	m_bWeapon(0),
	m_uiRefCount(1),
	m_iGender(0),
	m_iClass(0),
	m_pkAdditionalParts(0)
{
	m_kID.clear();
	m_kPath.clear();
	m_kKFMPath.clear();
}

PgItem::~PgItem()
{
	m_spAM = 0;
	m_spMeshRoot = 0;
	m_kTextureContainer.clear();
	m_kAttachedNodes.clear();
	if(m_pkAdditionalParts)
	{
		NiDelete m_pkAdditionalParts;
	}
}

const char *PgItem::GetID()
{
	if(m_kID.empty())
	{
		return 0;
	}
	return m_kID.c_str();
}

void PgItem::SetID(const char *pcID)
{
	m_kID = std::string(pcID);
}

const char *PgItem::GetKFMPath()
{
	if(m_kKFMPath.empty())
	{
		return 0;
	}
	return m_kKFMPath.c_str();
}

const char *PgItem::GetMeshPath()
{
	if(m_kMeshPath.empty())
	{
		return 0;
	}
	return m_kMeshPath.c_str();
}

const char *PgItem::GetPath()
{
	if(m_kPath.empty())
	{
		return 0;
	}
	return m_kPath.c_str();
}

const char *PgItem::GetTargetPoint()
{
	if(m_kTargetPoint == "")
	{
		return 0;
	}

	return m_kTargetPoint.c_str();
}

int PgItem::ItemPos()
{
	return m_iItemPos;
}

NiNodePtr PgItem::GetMeshRoot()
{
	return m_spMeshRoot;
}

bool PgItem::ChangeActorManager(const char *pcAMPath)
{
	// TODO : m_spMeshRoot와 기존 AM의 RefCount가 어찌 되는지 잘 봐야 함.
	NiActorManager *pkAM = g_kAMPool.LoadActorManager(pcAMPath);
	if(!pkAM)
	{
		return false;
	}
	m_spAM = pkAM;
	m_spAM->ChangeNIFRoot(m_spMeshRoot);
	m_spAM->Update(0.0f);
	m_kKFMPath = pcAMPath;

	return true;
}

bool PgItem::ChangeMesh(const char *pcNIFPath)
{
	NiNodePtr spNode = g_kNifMan.GetNif(pcNIFPath);
	if(spNode)
	{
		m_spMeshRoot = 0;
		m_spMeshRoot = spNode;
		m_kMeshPath = pcNIFPath;

		m_kTextureContainer.clear();
		m_kSourceTextureContainer.clear();

		if(FindSourceTexture(m_spMeshRoot) && ApplyTexture())
		{
			return true;
		}
	}

	return false;
}

bool PgItem::Initialize()
{
	// Item Position을 구한다. 일단은 바뀔 예정이 없으므로 코드에 박아둠.
	const int iNbKindOfItem = 23;
	const char *pcPartName[iNbKindOfItem][2] = {
		"01_helm", "11",
		"02_hair", "1",
		"03_face", "2",
		"04_u_armor", "12",
		"05_d_armor", "13",
		"06_glove", "15",
		"07_shoes", "14",
		"08_back", "4",
		"09_accessory", "5",
		"10_shoulder", "3",
		"01_swd", "6",
		"02_bts", "6",
		"03_stf", "6",
		"04_spr", "6",
		"05_bow", "6",
		"06_csb", "6",
		"07_clw", "6",
		"08_kat", "6",
		"2_shield", "7",
		"10_bxg", "6",
		"11_bot", "14",
		"09_rfe", "6",
		"3_tool", "6"
	};
	
	for(int i=0; i<iNbKindOfItem; ++i)
	{
		std::string::size_type kPos = m_kMeshPath.find(pcPartName[i][0]);
		if(kPos != std::string::npos)
		{
			m_iItemPos = 1 << atoi(pcPartName[i][1]);
			break;
		}
	}

	if(true==m_kTargetPoint.empty())
	{
	switch(m_iItemPos)
	{
	case 1 << 1:
	case 1 << 5:
		m_kTargetPoint = "p_pt_hair";
		break;
	case 1 << 2:
		m_kTargetPoint = "Bip01 Head";
		break;
	case 1 << 3:
	case 1 << 4:
		m_kTargetPoint = "p_pt_chest";
		break;
	case 1 << 6:
		if(m_eWeaponAttachPos == LEFT_HAND)
		{
			m_kTargetPoint = "p_wp_l_hand";
		}
		else if(m_eWeaponAttachPos == RIGHT_HAND)
		{
			m_kTargetPoint = "p_wp_r_hand";
		}
		else if(m_eWeaponAttachPos == LEFT_FOREARM)
		{
			m_kTargetPoint = "Bip01 L Forearm";
		}
		else if(m_eWeaponAttachPos == RIGHT_FOREARM)
		{
			m_kTargetPoint = "Bip01 R Forearm";
		}
		break;
	case 1 << 7:
		m_kTargetPoint = "p_wp_shield";
		break;
	}
	}

	m_spMeshRoot = g_kNifMan.GetNif(m_kMeshPath.c_str());
	if(!m_spMeshRoot)
	{
#ifdef _DEBUG
		char szMsg[500];
		NiSprintf(szMsg, 499, "메쉬를 로딩할 수 없습니다 : %s", m_kMeshPath.c_str());
//		NiMessageBox(szMsg, "Can't Load Mesh");
#endif
		return false;
	}

	bool bRet = FindSourceTexture(m_spMeshRoot);
	if(bRet)
	{
		bRet = ApplyTexture();
		m_spMeshRoot->UpdateProperties();
	}

	// AM이 있다면 AM의 Root에 Mesh를 붙여야 한다.
	if(m_spAM)
	{
		m_spAM->ChangeNIFRoot(m_spMeshRoot);
		m_spAM->Update(0.0f);
	}
			
	return bRet;
}

int PgItem::GetClass()
{
	return m_iClass;
}

int PgItem::GetGender()
{
	return m_iGender;
}

bool PgItem::IsAvailableAnimation()
{
	return (m_spAM != 0);
}

NiActorManagerPtr PgItem::GetActorManager()
{
	return m_spAM;
}

bool PgItem::IsAttached()
{
	return m_bAttached;
}

void PgItem::SetAttached(bool bAttached)
{
	m_bAttached = bAttached;
}

void PgItem::AddAttachedNode(NiAVObject *pkNode)
{
	m_kAttachedNodes.insert(pkNode);
}

void PgItem::RemoveAttachedNode(NiAVObject *pkNode)
{
	AttachedNodes::iterator itr = m_kAttachedNodes.find(pkNode);
	if(itr == m_kAttachedNodes.end())
	{
		return;
	}

	m_kAttachedNodes.erase(itr);
}

NiAVObject *PgItem::GetAttachedNode()
{
	AttachedNodes::iterator itr = m_kAttachedNodes.begin();
	if(itr == m_kAttachedNodes.end())
	{
		return 0;
	}

	return *itr;		
}

void PgItem::IncRefCount()
{
	++m_uiRefCount;
}

void PgItem::DecRefCount()
{
	--m_uiRefCount;
}

unsigned int PgItem::GetRefCount()
{
	return m_uiRefCount;
}

bool PgItem::IsWeapon()
{
	return m_bWeapon;
}

unsigned int PgItem::GetWeaponType()
{
	return m_iWeaponType;
}

PgItem *PgItem::GetAdditionalItem()
{
	return m_pkAdditionalParts;
}

ArrayList *PgItem::GetSrcTexture()
{
	m_kSourceTextureContainer.clear();
	if(!FindSourceTexture(m_spMeshRoot))
	{
		return 0;
	}

	ArrayList *pkSrcTextures = NiExternalNew ArrayList();
	
	TexturePropertyContainer::iterator itr = m_kSourceTextureContainer.begin();
	while(itr != m_kSourceTextureContainer.end())
	{
		String *kPath = NiExternalNew String(itr->first.c_str());
		pkSrcTextures->Add(kPath);
		++itr;
	}

	if(pkSrcTextures->Count == 0)
	{
		return 0;
	}

	return pkSrcTextures;
}

String *PgItem::GetDestTexture(String *pkSrcTexture)
{
	const char *pcSrcTexture = MStringToCharPointer(pkSrcTexture);
	TexturePair kTexturePair(pcSrcTexture);
	TexturePathContainer::iterator itrTexturePath = std::find(m_kTextureContainer.begin(), m_kTextureContainer.end(), kTexturePair);
	MFreeCharPointer(pcSrcTexture);

	if(itrTexturePath == m_kTextureContainer.end())
	{
		return 0;		
	}

	return itrTexturePath->m_kDestTexture.c_str();
}

bool PgItem::SetDestTexture(String *pkSrcTexture, String *pkDestTexture)
{
	if(!IsExistsTexture(pkDestTexture))
	{
		return false;
	}

	const char *pcSrcTexture = MStringToCharPointer(pkSrcTexture);
	const char *pcDestTexture = MStringToCharPointer(pkDestTexture);

	TexturePair kTexturePair(pcSrcTexture, pcDestTexture);

	TexturePathContainer::iterator itrTexturePath = std::find(m_kTextureContainer.begin(), m_kTextureContainer.end(), kTexturePair);
	if(itrTexturePath != m_kTextureContainer.end())
	{
		// 기존의 Texture Set을 변경
		itrTexturePath->m_kDestTexture = kTexturePair.m_kDestTexture;
	}
	else
	{
		// 새로 추가되는 Texture Set
		m_kTextureContainer.push_back(kTexturePair);
	}

	MFreeCharPointer(pcSrcTexture);
	MFreeCharPointer(pcDestTexture);

	return true;
}

bool PgItem::IsExistsTexture(String *pkDestTexture)
{
	const char *pcDestTexture = MStringToCharPointer(pkDestTexture);

	std::string kDestPath;
	//PgUtility::ToAbsolutePath(pcDestTexture, "d:/work/projectg/sfreedom_dev", kDestPath);
	PgUtility::ToSuitableRelativePath(pcDestTexture, RELATIVE_PATH, kDestPath);
	MFreeCharPointer(pcDestTexture);

	// Dest Texture가 없으면, 텍스쳐 정보를 갱신하지 않는다.
	if(!NiFile::Access(kDestPath.c_str(), NiFile::READ_ONLY))
	{
		return false;
	}

	return true;
}

bool PgItem::FindSourceTexture(NiNode *pkRoot)
{
	if(!pkRoot)
	{
		return false;
	}

	unsigned int uiArrayCount = pkRoot->GetArrayCount();

	for (unsigned int i = 0; i < uiArrayCount; i++)
	{
		NiAVObject* pkChild = pkRoot->GetAt(i);

		if(!pkChild)
		{
			continue;
		}

		if(NiIsKindOf(NiGeometry, pkChild) && !pkChild->GetAppCulled())
		{
			NiGeometry *pkGeometry =(NiGeometry*)pkChild;

			// 기존의 TextureProperty를 가져온다.
			NiTexturingProperty *pkTextureProp = NiDynamicCast(NiTexturingProperty, pkGeometry->GetProperty(NiTexturingProperty::GetType()));
			if(!pkTextureProp)
			{
				continue;
			}

			// BaseMap의 텍스쳐를 가져온다.
			NiTexture *pkTex = pkTextureProp->GetBaseMap()->GetTexture();
			NiSourceTexture *pkSrcTex = (NiSourceTexture*)pkTex;

			// BaseMap의 텍스쳐 파일 이름을 알아온다.
			NiString kSrcTexture(pkSrcTex->GetFilename());
			unsigned int iPos = kSrcTexture.FindReverse('\\');
			if(iPos != NiString::INVALID_INDEX)
			{
				++iPos;
				kSrcTexture = kSrcTexture.Mid(iPos);
			}
			kSrcTexture.ToLower();

			m_kSourceTextureContainer.insert(std::make_pair(kSrcTexture, pkTextureProp));
		}
		else if(NiIsKindOf(NiNode, pkChild))
		{
			FindSourceTexture((NiNode *)pkChild);
		}
	}

	return true;
}

bool PgItem::ApplyTexture()
{
	TexturePropertyContainer::iterator itr = m_kSourceTextureContainer.begin();
	while(itr != m_kSourceTextureContainer.end())
	{
		NiTexturingProperty *pkTextureProp = itr->second;
		if(!pkTextureProp)
		{
			++itr;
			continue;
		}

		// BaseMap의 텍스쳐를 가져온다.
		NiTexture *pkTex = pkTextureProp->GetBaseMap()->GetTexture();
		NiSourceTexture *pkSrcTex = (NiSourceTexture*)pkTex;

		// 기존에 붙어 있는 텍스쳐가 변경해야 하는 텍스쳐 리스트에 있는가?
		TexturePair kTexturePair(itr->first.c_str());
		TexturePathContainer::iterator itrTexturePath = std::find(m_kTextureContainer.begin(), m_kTextureContainer.end(), kTexturePair);

		if(m_kTextureContainer.end() == itrTexturePath)
		{
			++itr;
			continue;
		}
		
		//TexturePath를 알맞게 고쳐서 로딩한다.
		std::string kDestPath;
		//PgUtility::ToAbsolutePath(itrTexturePath->m_kDestTexture.c_str(), "d:/work/projectg/sfreedom_dev", kDestPath);
		PgUtility::ToSuitableRelativePath(itrTexturePath->m_kDestTexture.c_str(), RELATIVE_PATH, kDestPath);
	
		if(!NiFile::Access(kDestPath.c_str(), NiFile::READ_ONLY))
		{
			assert(!"Can't create New Texture : Invalid Path!");
			++itr;
			continue;
		}
		
		// 바꿀 Texture의 경로로 Texture를 새로 만든다.
		NiSourceTexture *pkNewTex = NiSourceTexture::Create(kDestPath.c_str());

		// 새로운 텍스쳐를 설정한다.
		// TODO : 기존의 텍스쳐를 버리고 그냥 Set해도 Memory Leak이 없는가? 체크하자.
		pkSrcTex->DestroyAppPixelData();
		pkTextureProp->GetBaseMap()->SetTexture(pkNewTex);

		++itr;
	}

	return true;
}

PgItem *PgItem::CreateItem(const char *pcItemID, 
						   const char *pcNIFPath, 
						   const char *pcKFMPath,
						   int iWeapon,
						   int iFilter)
{
	if(!pcItemID || !pcNIFPath)
	{
		return 0;
	}

	PgItem *pkThis = NiNew PgItem();
	pkThis->m_kID = std::string(pcItemID);
	pkThis->m_kMeshPath = std::string(pcNIFPath);
	pkThis->m_iWeaponType = iWeapon;
	pkThis->m_iItemFilter = iFilter;
	if(pcKFMPath)
	{
		pkThis->m_kKFMPath = std::string(pcKFMPath);
		pkThis->m_spAM = g_kAMPool.LoadActorManager(pcKFMPath);
	}

	if(!pkThis->Initialize())
	{
		NiDelete pkThis;
		return 0;
	}

	return pkThis;
}

bool PgItem::WriteToXml(String *pkXmlPath, bool bOverwrite)
{
	XmlTextWriter *pkWriter = new XmlTextWriter(pkXmlPath, 0);

	// <ITEM NAME = "" WEAPON = "" FILTER = "">
	pkWriter->WriteStartElement(S"ITEM");
	pkWriter->WriteAttributeString(S"NAME", m_kID.c_str());
	if(m_iWeaponType != 0)
	{
		pkWriter->WriteAttributeString(S"WEAPON", XmlConvert::ToString(m_iWeaponType));
	}
	pkWriter->WriteWhitespace("\n\t");

	// <KFMPATH> ..... </KFMPATH>
	if(!m_kKFMPath.empty())
	{
		pkWriter->WriteElementString(S"KFMPATH", m_kKFMPath.c_str());
		pkWriter->WriteWhitespace("\n\t");
	}

	// <MESH> ..... </MESH>
	pkWriter->WriteElementString(S"MESH", m_kMeshPath.c_str());
	
	// <TEXTURE SRC=""> ..... </TEXTURE>
	TexturePathContainer::iterator itr = m_kTextureContainer.begin();
	while(itr != m_kTextureContainer.end())
	{
		pkWriter->WriteWhitespace("\n\t");
		pkWriter->WriteStartElement(S"TEXTURE");
		pkWriter->WriteAttributeString(S"SRC", itr->m_kSrcTexture.c_str());
		pkWriter->WriteString(itr->m_kDestTexture.c_str());
		pkWriter->WriteEndElement();
		
		++itr;
	}

	pkWriter->WriteWhitespace("\n");
	pkWriter->WriteFullEndElement();

	pkWriter->Close();

	return true;
}

PgItem *PgItem::CreateItemFromXmlFile(const char *pcPath, int iGender, int iClass, PgItem *pkItem)
{
	XmlReaderSettings *pkSettings = new XmlReaderSettings();

	pkSettings->ConformanceLevel = ConformanceLevel::Fragment;
	pkSettings->IgnoreWhitespace = true;
	pkSettings->IgnoreComments = true;
	
	XmlReader *pkReader = XmlReader::Create(pcPath, pkSettings);
	if(!pkReader)
	{
		assert(!"Can't load Xml File");
		return 0;
	}

	// 최상위 노드를 찾는다.
	while(pkReader->Read())
	{
		if(pkReader->NodeType == XmlNodeType::Element)
		{
			// 최상위 노드가 <ITEM>이 아니면 종료
			if(pkReader->Name->CompareTo(S"ITEM") != 0)
			{
				pkReader->Close();
				assert(!"Element is not <ITEM>!!");
				return 0;		
			}
			else
			{
				break;
			}
		}
	}

	// 새 아이템을 생성 또는 기존의 아이템을 변경
	PgItem *pkThis = 0;
	if(!pkItem)
	{
		pkThis = NiNew PgItem;
	}
	else
	{
		pkItem->m_spAM = 0;
		pkItem->m_spMeshRoot = 0;
		pkItem->m_iItemType = 0;
		pkItem->m_iItemPos = 0;
		pkItem->m_iItemFilter = 0;
		pkItem->m_uiRefCount = 1;
		pkThis->m_kTargetPoint.clear();
		pkThis = pkItem;
	}

	if(!pkThis)
	{
		assert(!"Can't create PgItem Object!");
		return 0;
	}

	pkThis->m_kPath = std::string(pcPath);

	// ITEM Element의 첫 번째 Attribute를 찾는다. (ITEM 태그)
	while(pkReader->MoveToNextAttribute())
	{		
		String *pkTagName = pkReader->Name;
		if(pkTagName->CompareTo(S"NAME") == 0)
		{
			const char *pcName = MStringToCharPointer(pkReader->Value);
			pkThis->m_kID = pcName;
			MFreeCharPointer(pcName);
		}
		else if(pkTagName->CompareTo(S"ATTACH_TO") == 0)
		{
			pkThis->m_kTargetPoint = MStringToCharPointer(pkReader->Value);
		}
		else if(pkTagName->CompareTo(S"FILTER") == 0)
		{
			pkThis->m_iItemFilter = Int32::Parse(pkReader->Value);
		}
		else if(pkTagName->CompareTo(S"WEAPON") == 0)
		{
			pkThis->m_bWeapon = true;
			pkThis->m_iWeaponType = Int32::Parse(pkReader->Value);
		}
		else if(pkTagName->CompareTo(S"ANIM_FOLDER_NUM") == 0)
		{
		}
		else if(pkTagName->CompareTo(S"WEAPON_ATTACH_POS") == 0)
		{
			String *pkAttrValue = pkReader->Value;
			if(pkAttrValue->CompareTo(S"LEFT") == 0)
			{
				pkThis->m_eWeaponAttachPos = PgItem::LEFT_HAND;
			}
			else if(pkAttrValue->CompareTo(S"RIGHT") == 0)
			{
				pkThis->m_eWeaponAttachPos = PgItem::RIGHT_HAND;
			}
			else if(pkAttrValue->CompareTo(S"LEFT_FOREARM") == 0)
			{
				pkThis->m_eWeaponAttachPos = PgItem::LEFT_FOREARM;
			}
			else if(pkAttrValue->CompareTo(S"RIGHT_FOREARM") == 0)
			{
				pkThis->m_eWeaponAttachPos = PgItem::RIGHT_FOREARM;
			}
		}
		else if(pkTagName->CompareTo(S"ADDITIANL_ITEM") == 0)
		{
			String *pkPath = pkReader->Value;
			const char *pcPath = MStringToCharPointer(pkPath);
			std::string kDestPath(RELATIVE_PATH);
			kDestPath+="/XML/";
			kDestPath+=pcPath;	
			//PgUtility::ToSuitableRelativePath(sz, RELATIVE_PATH, kDestPath);//왜 이렇게 했는지 이해 불가.
			MFreeCharPointer(pcPath);
			pkThis->m_pkAdditionalParts = PgItem::CreateItemFromXmlFile(kDestPath.c_str(), iGender, iClass);			
		}
		else
		{
			assert(!"Unknown attribute");
		}
	}

	bool bFindGender = false;
	String *kReqGender = (iGender == 2 ? S"FEMALE" : 
								iGender == 1 ? S"MALE" :
								iGender == 0 ? S"COMMON" :
								S"UNKNOWN");

	while(pkReader->Read() && !bFindGender)
	{
		String *kTagName = pkReader->Name;

		// 같은 성별의 매쉬 정보를 찾았다면
		if(kTagName->CompareTo(kReqGender) == 0)
		{
			bFindGender = true;
		}
		// 공통 성별의 아이템일 경우, 공통 성별의 매쉬 정보를 찾았다면
		else if(kTagName->CompareTo(S"COMMON") == 0)
		{
			kReqGender = S"COMMON";
			bFindGender = true;
		}
	}

	// 알맞은 매쉬 정보가 없음.
	if(!bFindGender)
	{
		MAssert(iGender >= 0 && iGender <= 2);
		return 0;
	}

	// Class정보를 찾는다.
	bool bFindClass = false;
	String *kReqClass = (iClass == 1 ? S"FIGHTER" : 
						iClass == 2 ? S"MAGICIAN" :
						iClass == 3 ? S"ARCHER" : 
						iClass == 4 ? S"THIEF" :
						iClass == 5 ? S"DR_SUMMONER" :
						S"DR_TWINS");
	
	while(pkReader->Read())
	{
		String *kCurClass = pkReader->Name;

		// 같은 클래스의 매쉬 정보를 찾았다면
		if(kCurClass->CompareTo(kReqClass) == 0)
		{
			bFindClass = true;
			break;
		}
	}

	// 이미 읽은 노드로 되돌아 가는 방법을 모르겠다........
	pkReader->Close();
	pkReader = XmlReader::Create(pcPath, pkSettings);

	if(bFindGender)
	{
		pkReader->ReadToFollowing(kReqGender);
	}
	if(bFindClass)
	{
		pkReader->ReadToFollowing(kReqClass);
	}
	
	while(pkReader->Read())
	{
		int iClass = 0;
		int iGender = 0;
		if(pkReader->NodeType == XmlNodeType::Element)
		{
			String *pkTagName = pkReader->Name;
			if(pkTagName->CompareTo(S"KFMPATH") == 0)
			{
				if(pkReader->Read() && pkReader->NodeType == XmlNodeType::Text)
				{
					const char *pcKFMRelativePath = MStringToCharPointer(pkReader->Value->ToLower());
					pkThis->m_spAM = g_kAMPool.LoadActorManager(pcKFMRelativePath);
					pkThis->m_kKFMPath = std::string(pcKFMRelativePath);
					MFreeCharPointer(pcKFMRelativePath);
				}
			}
			else if(pkTagName->CompareTo("MESH") == 0)
			{
				if(false==pkThis->m_kMeshPath.empty())
				{
					break;//나 이미 읽었음.
				}
				if(pkReader->Read() && pkReader->NodeType == XmlNodeType::Text)
				{
					const char *pcMeshRelativePath = MStringToCharPointer(pkReader->Value->ToLower());
					pkThis->m_kMeshPath = pcMeshRelativePath;
					MFreeCharPointer(pcMeshRelativePath);
				}
			}
			else if(pkTagName->CompareTo(S"TEXTURE") == 0)
			{
				if(pkReader->MoveToAttribute(S"SRC"))
				{
					std::string kSrc;
					std::string kDest;

					const char *pcSrcPath = MStringToCharPointer(pkReader->Value->ToLower());
					kSrc = pcSrcPath;
					MFreeCharPointer(pcSrcPath);
					
					if(pkReader->Read() && pkReader->NodeType == XmlNodeType::Text)
					{
						const char *pcDestPath = MStringToCharPointer(pkReader->Value->ToLower());
						kDest = std::string(pcDestPath);
						MFreeCharPointer(pcDestPath);
					}

					if(!kSrc.empty() && !kDest.empty())
					{
						TexturePair kTexturePair(kSrc.c_str(), kDest.c_str());
						pkThis->m_kTextureContainer.push_back(kTexturePair);
					}
				}
			}
			else if(pkTagName->CompareTo(S"TRAIL") == 0)
			{
			}
			else
			{
#ifdef _DEBUG
				char szMsg[512];
				const char *pcTagName = MStringToCharPointer(pkTagName);
				sprintf(szMsg, "Unknown Tag : %s", pkTagName);
				MFreeCharPointer(pcTagName);
				NiMessageBox(szMsg, "Error");
				break;
#endif				
			}
		}
	}

	pkReader->Close();

	if(!pkThis->Initialize())
	{
#ifdef _DEBUG
		char szMsg[512];
		sprintf(szMsg, "Failed Init Item : %s", pkThis->GetPath());
//		NiMessageBox(szMsg, "Error");
#endif
		NiDelete pkThis;
		return 0;
	}
	
	pkThis->m_iGender = iGender;
	pkThis->m_iClass = iClass;
	return pkThis;
}