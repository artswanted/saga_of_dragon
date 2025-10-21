#include "stdafx.h"

int const MAX_UNIT_CHAR_SIZE = 16000;		// 한번에 들어갈 최대 문자열 (16380를 넘어가면 컴파일 오류)
int const BUF_SIZE = 1000;
BM::vstring const DQ("\"");	// 쌍따옴표 문자
BM::vstring const BS("\\");	// 백스페이스 문자
BM::vstring const NL("\n");	// 개행 문자

// 스크립트
BM::vstring const SCRIPT_BS("#SC_BS#");			// 스크립트의 백스페이스 문자, 임시 교체 문자
BM::vstring const SCRIPT_BS_ON_CPP("\\\\");		// CPP에서 적혀질, 스크립트 백스페이스 문자

BM::vstring const SCRIPT_NL("#SC_NL#");			// 스크립트의개행 문자, 임시 교체 문자
BM::vstring const SCRIPT_NL_ON_CPP("\\n\\\n");		// CPP에서 적혀질, 스크립트 개행 문자

BM::vstring const SCRIPT_DQ("#SC_DQ#");			// 스크립트의 쌍따옴표, 임시 교체 문자
BM::vstring const SCRIPT_DQ_ON_CPP("\\\"");		// CPP에서 적혀질, 스크립트 쌍따옴표

BM::vstring const CPP_DQ("#CODE_DQ#");			// 코드에서 보여질 쌍따옴표 임시 교체 문자

// Action XML의 ID, PATH를 등록할때 반복 사용되는 문장
std::string const INSERT_ACTION_XML_PATH_BEGIN("	m_kContPath.insert( std::make_pair(");
std::string const INSERT_ACTION_XML_PATH_END(") );\n");

//Action XML의 PATH, XML 내용을 등록 할때 반복 사용되는 문장
std::string const INSERT_ACTION_XMLDATA_BEGIN("	m_kContXmlData.insert( std::make_pair(");
std::string const INSERT_ACTION_XMLDATA_END(") );\n");

int _tmain(int argc, wchar_t* argv[])
{	
	if(4 > argc)
	{
		return -1;
	}
	FILE * fpPgActionXmlDataFile = NULL;	// PgActionXMLDataFile.cpp
	std::string const kResultFileName = BM::vstring(argv[3]);
	fopen_s(&fpPgActionXmlDataFile, kResultFileName.c_str(), "wb");
	if(!fpPgActionXmlDataFile)
	{
		return -1;
	}

	{// Dragonica_Exe/XML/path.xml의 ACTION 태그에 적힌 path의 XML들을 읽어, CPP에서 그 내용을 등록하게끔 하는 구문을 만듬.
		// path.xml의 fullpath를 얻어와
		std::string const kPathXmlFullPath = BM::vstring(argv[1]);
		std::string kXmlDirectoryPath = kPathXmlFullPath;
		{// Dragonica_Exe/XML의 경로를 파악해두고
			std::string::size_type stPos = kXmlDirectoryPath.rfind("/");
			if(std::string::npos == stPos
				|| stPos == kXmlDirectoryPath.size() 
				)
			{
				return -1;
			}
			std::string kTemp(kXmlDirectoryPath);
			kTemp.replace(stPos+1, kTemp.size(), "");
			kXmlDirectoryPath = kTemp;
		}
		// XML에서 CPP로의 import 메세지를 표기해 두고
		std::cout<<" Import Action XML from \""<< kPathXmlFullPath.c_str()<<std::endl;

		CONT_ACTION_PATH kContActionPath;
		{// Path.xml 에서 Action만 골라내어 ID와 Path를 뽑아내서 m_kContPath.insert 하는 구문을 만든다.
			{// CPP의 시작 부분의 내용을 파일에 쓰고
				std::string kCppBeginStr("#include \"stdafx.h\"\n#include \"PgActionXmlData.h\"\n\nbool PgActionXmlData::Init()\n{\n");
				WriteToFile( fpPgActionXmlDataFile, kCppBeginStr);
			}
			{// CPP에서 ActionID, Path 등록 하는 부분
				ParsePathXML(kPathXmlFullPath, kContActionPath);	// path.xml의 <ACTION ID="#action_id#"> #action_xml_path# </ACTION>
																	// #action_id#, #action_xml_path# 이 두 가지를 가져와 kContActionPath에 저장하고
				
				CONT_ACTION_PATH::const_iterator kItor = kContActionPath.begin();
				while(kContActionPath.end() != kItor)
				{// kContActionPath에 저장된 내용을 바탕으로, CPP에서 ID, PATH 등록하는 구문을 작성하고
					std::string kStr(INSERT_ACTION_XML_PATH_BEGIN); // m_kContPath.insert( std::make_pair(
					kStr+=DQ; kStr+= kItor->first; kStr+=DQ;	//										"#action_id#",
					kStr+=", ";										
					kStr+=DQ; kStr+= kItor->second; kStr+=DQ;	//													 "#action_xml_path#") );
					kStr+=INSERT_ACTION_XML_PATH_END;
					WriteToFile( fpPgActionXmlDataFile, kStr);	// PgActionXmlDataFile에 쓰고
					++kItor;
				}
				WriteToFile( fpPgActionXmlDataFile, "\n" );
			}
		}
		{// 이제 각 ActionID가 어느 XML을 사용하는지 확인 되었으므로
			CONT_ACTION_PATH::const_iterator kItor = kContActionPath.begin();
			while(kContActionPath.end() != kItor)
			{// Action Xml Path
				std::string kText;
				
				std::string kXmlFileName(kXmlDirectoryPath);		// Dragonica_Exe/XML
				kXmlFileName+= kItor->second;						//					/해당 ActionXML 경로로
				FILE * fpActionXML = NULL;
				fopen_s(&fpActionXML, kXmlFileName.c_str(), "rt");	// XML File의 내용을 읽어와
				if(fpActionXML)
				{
					char acBuf[BUF_SIZE] = {};
					while( !feof(fpActionXML) )
					{// 내용을 저장하고 
						::memset(acBuf, 0, sizeof(acBuf));
						fgets(acBuf, sizeof(acBuf), fpActionXML);
						kText += acBuf;
					}
					fclose(fpActionXML);
				}
				
				if( !kText.empty() )
				{// 얻어온 내용은 CPP에서 표기 될것이므로
					BM::vstring vStr(kText);
					vStr.Replace(NL, "");							// 불필요한 개행문자는 제거하고
					vStr.Replace(DQ, SCRIPT_DQ);					// "를 
					vStr.Replace(SCRIPT_DQ, SCRIPT_DQ_ON_CPP);		// \"로 바꿔준다(바로 바꾸면 무한 루프에 빠짐)
					kText = vStr;
					
					std::string kStr(INSERT_ACTION_XMLDATA_BEGIN);	// m_kContXmlData.insert( std::make_pair(
					kStr+=DQ; kStr+= kItor->second; kStr+=DQ;		//										   "#action_xml_path#",
					kStr+=", ";
					kStr+=DQ; kStr+= kText; kStr+=DQ;				//																"XML내용") );
					kStr+=INSERT_ACTION_XMLDATA_END;
					WriteToFile(fpPgActionXmlDataFile, kStr);		// PgActionXmlDataFile에 쓰고
				}
				++kItor;
			}
			WriteToFile( fpPgActionXmlDataFile, "\n" );
		}
		std::cout<<"complete."<<std::endl;
	}

	{// Dragonica_Exe/Script/action.lua를 읽어와, 그곳에 포함되어있는 Skill lua의 내용을 CPP에 등록 하는 구문을 만듬
		//Script/action.lua의 fullpath를 얻어와
		std::string const kActionLuaFileFullPath = BM::vstring(argv[2]);
		std::string kScriptDirectoryPath;
		
		{// Dragonica_Exe/Script/ 위치를 저장해두고
			kScriptDirectoryPath = kActionLuaFileFullPath;
			std::string::size_type stPos = kActionLuaFileFullPath.rfind("/");
			if(std::string::npos == stPos
				|| stPos == kActionLuaFileFullPath.size() 
				)
			{
				return -1;
			}
			std::string kTemp(kScriptDirectoryPath);
			kTemp.replace(stPos+1, kTemp.size(), "");
			kScriptDirectoryPath = kTemp;
		}
		std::cout<<" Import Skill lua from \""<< kActionLuaFileFullPath.c_str()<<"\" -> ";

		std::string kText;
		{//action.lua  내용 얻어와
			FILE * pFile;
			fopen_s(&pFile, kActionLuaFileFullPath.c_str(), "rt");
			if(pFile)
			{
				char acBuf[BUF_SIZE] = {};
				while(!feof(pFile)) 
				{
					::memset(acBuf, 0, sizeof(acBuf));
					fgets(acBuf, sizeof(acBuf), pFile);
					kText += acBuf;
				}
				fclose(pFile);
			}
		}

		{// lua 주석 제거을 제거하고
			std::string::size_type stPos1 = kText.find("--[[");
			std::string::size_type stPos2 = kText.find("--");
			while(std::string::npos != stPos1 
				|| std::string::npos != stPos2
				)
			{
				bool bMultyLine = false;
				if(stPos1 != stPos2)
				{// 긴 주석에 포함되는 -- 이 아닌 다른 주석이라면
					if(stPos1 < stPos2)
					{// 긴주석이 먼저 있으면 긴주석을 먼저 제거 하고
						bMultyLine = true;
					}
					else if(stPos1 > stPos2)
					{// 한줄 주석이 먼저 있으면 한줄 주석 부터 제거 한다
						bMultyLine = false;
					}
				}
				else
				{
					bMultyLine = true;
				}

				if(bMultyLine)
				{
					ReplaceStringOneAtaTime(kText, "", "--[[", "]]", kText);
				}
				else
				{
					ReplaceStringOneAtaTime(kText, "", "--", "\n", kText);
				}
				stPos1 = kText.find("--[[");
				stPos2 = kText.find("--");
			}
		}

		{//DoFile(, ) 문자 제거해
			CONT_STR kCont;
			GetBetweenStringOneAtaTime(kText,"DoFile(\"","\")", kCont);
			CONT_STR::const_iterator kItor = kCont.begin();
			std::string kLuaText;
			while( kCont.end() != kItor )
			{// Action Lua의 File 이름을 얻어낸 다음
				std::string kFilePath = kScriptDirectoryPath + (*kItor);	// Action Lua의 path를 얻어
				
				FILE * fpActionLuaFile = NULL;
				fopen_s(&fpActionLuaFile, kFilePath.c_str(), "rt");			// File을 읽은 다음
				if(fpActionLuaFile)
				{
					kLuaText+= "	m_kContLuaText.push_back(std::string(\"";	//m_kContLuaText.push_back( 의 구문으로 저장하는데
					int iTempCnt = 1;
					int iAccSize = 0;
					std::string kText;
					char acBuf[BUF_SIZE] = {};
					while( !feof(fpActionLuaFile) )
					{// 파일을 읽다가
						::memset(acBuf, 0, sizeof(acBuf));
						fgets(acBuf, sizeof(acBuf), fpActionLuaFile);
						kText += acBuf;
						if( (MAX_UNIT_CHAR_SIZE+iAccSize) < static_cast<int>(kText.size()) )
						{// 그 사이즈가 한번에 넣을수 없는 크기라면 분리해서 넣어 주고,  std::string() +std::string() 이런식으로.
							iAccSize += MAX_UNIT_CHAR_SIZE;
							kText+=CPP_DQ;
							kText+=") + std::string(";
							kText+=CPP_DQ;
							std::cout<<"\n        내용이 너무 길어 분리합니다 -"<<kFilePath.c_str();
						}
					}

					{// CPP에 등록 되어야 하므로
						BM::vstring vStr(kText);
						vStr.Replace(BS, SCRIPT_BS);	// 백스페이스 문자
						vStr.Replace(NL, SCRIPT_NL);	// 개행 문자
						vStr.Replace(DQ, SCRIPT_DQ);	// 쌍따옴표 문자를
						
						// CPP에 등록될 수 있게 적절히 수정해서 
						vStr.Replace(SCRIPT_BS, SCRIPT_BS_ON_CPP);
						vStr.Replace(SCRIPT_NL, SCRIPT_NL_ON_CPP);
						vStr.Replace(SCRIPT_DQ, SCRIPT_DQ_ON_CPP);
						vStr.Replace(CPP_DQ, DQ);
						kText = vStr;
					}
					kLuaText += kText;
					kLuaText+= "\") );\n";	// 담아 둔다.
					fclose(fpActionLuaFile);
				}
				++kItor;
			}

			{// 모든 action lua의 내용을 담았으므로
				std::string kTemp = kLuaText;
				kTemp+= "\n";
				WriteToFile( fpPgActionXmlDataFile, kTemp);	// File에 쓰고
			}
		}
		std::cout<<"\ncomplete."<<std::endl<<std::endl;	// 완료 되었음을 알린다.
	}
	
	std::string kCppEndStr("	return true;\n}\n");	// CPP에 마무리 구문을 등록하고
	WriteToFile( fpPgActionXmlDataFile, kCppEndStr);	// 최종적으로 PgActionXmlDataFile.cpp를 저장한다
	fclose(fpPgActionXmlDataFile);
	return 0;
}