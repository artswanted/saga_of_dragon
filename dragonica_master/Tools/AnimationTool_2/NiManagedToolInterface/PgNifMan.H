#pragma once

#include "NiMain.H"

//	NIF 파일을 로딩 및 관리를 해준다.
class	PgNifMan
{
	typedef std::map<std::string, NiNodePtr> NifContainer;

	NifContainer	m_NifContainer;

public:

	PgNifMan()
	{
		Init();
	}
	virtual	~PgNifMan()
	{
		Destroy();
	}

	void	Init();
	void	Destroy();

public:

	NiNodePtr	GetNif(const std::string NifFilePath);	//	Nif 파일을 로딩한 후, 클론 한 오브젝트를 리턴한다.
	NiNodePtr	GetNif_DeepCopy(const std::string NifFilePath);	//	Nif 파일을 로딩한 후, 클론 한 오브젝트를 리턴한다.(이때에는 DeepCopy를 사용한다)
	bool	DeleteNif(const std::string NifFilePath);	//	Nif 파일을 컨테이너에서 제거한다.
	bool	IsExist(const std::string NifFilePath);	//	Nif 파일이 컨테이너에 존재하는가?
	bool	DeleteAllNif();	//	컨테이너 클리어

};

#define g_kNifMan Loki::SingletonHolder< PgNifMan >::Instance()