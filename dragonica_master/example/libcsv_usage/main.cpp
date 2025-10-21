#include <libcsv/csv.h>
#include <string.h>
#include <iostream>
#include <fstream>

static void TestCommon()
{
	static const char *Data = "test,lol,kek,string\n"
				   "1,2,3,\"abc,2\"\n"
				   "8,7,6,kek\n";
	CsvParser csv;
	if (!csv.load_from_buffer(Data, strlen(Data)))
		return;
	do
	{
		for(int i = 0; i < csv.col_count(); i++)
			std::cout << csv.col(i) << " " << std::flush;
		std::cout << "\n";
	} while (csv.next_row());
}

static void TestFile()
{
	static const char* path = "Z:/Work/Dragonica/Advent/Dragonica_Exe/Archangel_run/MMC/Patch/Contents/Table/DR2_Def/TB_DefItemAbil.csv";
	std::ifstream ff(path);
	if (!ff.is_open())
		return;
	std::string str;

	ff.seekg(0, std::ios::end);   
	str.reserve(ff.tellg());
	ff.seekg(0, std::ios::beg);

	str.assign((std::istreambuf_iterator<char>(ff)),
				std::istreambuf_iterator<char>());
	CsvParser csv;
	if (!csv.load_from_buffer(str.c_str(), str.length()))
		return;
	do
	{
		for(int i = 0; i < csv.col_count(); i++)
		{
			const char *a = csv.col(i);
			//std::cout << a << " ";
		}	
		//std::cout << "\n";
	} while (csv.next_row());
}


static void TestCommon1()
{
	static const char *Data = "\"test\",\"lol\",\"kek\",\"string\"\n"
				   "\"new\nline\",2,3,\"abc,2\"\n"
				   "1,2,3,\"abc,2\"\n"
				   "8,7,6,kek\n";
	CsvParser csv;
	if (!csv.load_from_buffer(Data, strlen(Data)))
		return;
	do
	{
		for(int i = 0; i < csv.col_count(); i++)
			std::cout << csv.col(i) << " " << std::flush;
		std::cout << "\n";
	} while (csv.next_row());
}


int main()
{
	TestCommon1();
	TestCommon();
	TestFile();
	system("pause");
    return 0;
}