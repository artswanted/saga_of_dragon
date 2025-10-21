#include "stdAfx.h"
#include "PgAMPool.h"

PgAMPool::PgAMPool(void)
{
}

PgAMPool::~PgAMPool(void)
{
}
void PgAMPool::Destroy()
{
	for(AMContainer::iterator itr = m_kContainer.begin();
		itr != m_kContainer.end();
		itr++)
	{
		NiActorManager *pkAM = (NiActorManager*)itr->second;
		NiDelete pkAM;
	}
}
NiActorManager *PgAMPool::LoadActorManager(const char *pcKFMPath)
{
	AMContainer::iterator itr = m_kContainer.find(pcKFMPath);
	
	if(itr == m_kContainer.end())
	{
		std::string kKFMPath;
		//PgUtility::ToAbsolutePath(pcKFMPath, "d:/work/projectg/sfreedom_dev", kKFMPath);
		PgUtility::ToSuitableRelativePath(pcKFMPath, RELATIVE_PATH, kKFMPath);
				
		NiActorManager *pkAM = NiActorManager::Create(kKFMPath.c_str());

		if(!pkAM)
		{
			return 0;
		}

		itr = m_kContainer.insert(std::make_pair(pcKFMPath, pkAM)).first;
	}

	NiActorManager *pkAM = itr->second;

	return pkAM->Clone();
}