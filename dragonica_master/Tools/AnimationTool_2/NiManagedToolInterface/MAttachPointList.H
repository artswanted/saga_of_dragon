// Barunson Interactive Propritary Information

#pragma once
#include <vector>
#include <string>

namespace NiManagedToolInterface
{
	public __gc class MAttachPointList
	{
	public:
		MAttachPointList()
		{
		}

		void	Init(String* kPath)
		{
			m_pkAttachPointCont = NiExternalNew AttachPointVector();
			m_kPath = kPath;
			Load(kPath);
		}
		void	Save()
		{
			Save(m_kPath);
		}
		void	Reload()
		{
			m_pkAttachPointCont = NiExternalNew AttachPointVector();
			Load(m_kPath);

		}

		void	Shutdown()
		{
			Save(m_kPath);

			m_pkAttachPointCont = NULL;
			m_kPath = NULL;
		}

	public:

		unsigned int GetNumAttachPoint()	{	return	m_pkAttachPointCont->size();	}
		String*	GetAttachPointAt(unsigned int uiPos)
		{
			return	NiExternalNew String(m_pkAttachPointCont->at(uiPos).c_str());
		}

		bool	AddAttachPoint(String* kAttachPointName);
		bool	AddAttachPoint(std::string const &kAttachPointName);
		bool	RemoveAttachPoint(String* kAttachPointName);

	private:

		bool	Load(String* kPath);
		bool	Save(String* kPath);

		bool	Load(FILE *fp);
		bool	Save(FILE *fp);
		std::string ReadNextToken(FILE *fp);
		std::string GetFilePath(String* kPath);

	private:

		typedef std::vector<std::string> AttachPointVector;
		AttachPointVector	*m_pkAttachPointCont;

		String*	m_kPath;
	};
}