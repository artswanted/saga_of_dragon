#pragma once

class IFBase
{
public:
	virtual void ChangeIF(std::wstring const& Contents, std::wstring const& Static1, std::wstring const& Static2, std::wstring const& Static3) = 0;
	virtual bool Input(WPARAM wParam, LPARAM lParam) = 0;
	virtual DWORD GetOption(){ return 0x00000000; };

	virtual ~IFBase(void){};

protected:
	explicit IFBase(HWND hWnd) : m_hWnd(hWnd){};

protected:
	HWND	m_hWnd;
};

class IFDefault
	: public IFBase
{
public:
	virtual void ChangeIF(std::wstring const& Contents, std::wstring const& Static1, std::wstring const& Static2, std::wstring const& Static3);
	virtual bool Input(WPARAM wParam, LPARAM lParam);

	explicit IFDefault(HWND hWnd) : IFBase(hWnd){};
	virtual ~IFDefault(void){};
};

class IFDiff
	: public IFBase
{
public:
	virtual void ChangeIF(std::wstring const& Contents, std::wstring const& Static1, std::wstring const& Static2, std::wstring const& Static3);
	virtual bool Input(WPARAM wParam, LPARAM lParam);
	virtual DWORD GetOption();

	explicit IFDiff(HWND hWnd) : IFBase(hWnd){};
	virtual ~IFDiff(void){};
};

class IFPack
	: public IFBase
{
public:
	virtual void ChangeIF(std::wstring const& Contents, std::wstring const& Static1, std::wstring const& Static2, std::wstring const& Static3);
	virtual bool Input(WPARAM wParam, LPARAM lParam);

	explicit IFPack(HWND hWnd) : IFBase(hWnd){};
	virtual ~IFPack(void){};
};

class IFCreateList
	: public IFBase
{
public:
	virtual void ChangeIF(std::wstring const& Contents, std::wstring const& Static1, std::wstring const& Static2, std::wstring const& Static3);
	virtual bool Input(WPARAM wParam, LPARAM lParam);
	virtual DWORD GetOption();

	explicit IFCreateList(HWND hWnd) : IFBase(hWnd){};
	virtual ~IFCreateList(void){};
};

class IFHeaderMerge
	: public IFBase
{
public:
	virtual void ChangeIF(std::wstring const& Contents, std::wstring const& Static1, std::wstring const& Static2, std::wstring const& Static3);
	virtual bool Input(WPARAM wParam, LPARAM lParam);

	explicit IFHeaderMerge(HWND hWnd) : IFBase(hWnd){};
	virtual ~IFHeaderMerge(void){};
};

class IFBind
	: public IFBase
{
public:
	virtual void ChangeIF(std::wstring const& Contents, std::wstring const& Static1, std::wstring const& Static2, std::wstring const& Static3);
	virtual bool Input(WPARAM wParam, LPARAM lParam);

	explicit IFBind(HWND hWnd) : IFBase(hWnd){};
	virtual ~IFBind(void){};
};

class IFUnBind
	: public IFBase
{
public:
	virtual void ChangeIF(std::wstring const& Contents, std::wstring const& Static1, std::wstring const& Static2, std::wstring const& Static3);
	virtual bool Input(WPARAM wParam, LPARAM lParam);

	explicit IFUnBind(HWND hWnd) : IFBase(hWnd){};
	virtual ~IFUnBind(void){};
};

class IFDMakeINB
	: public IFBase
{
public:
	virtual void ChangeIF(std::wstring const& Contents, std::wstring const& Static1, std::wstring const& Static2, std::wstring const& Static3);
	virtual bool Input(WPARAM wParam, LPARAM lParam);

	explicit IFDMakeINB(HWND hWnd) : IFBase(hWnd){};
	virtual ~IFDMakeINB(void){};
};

class IFExportList
	: public IFBase
{
public:
	virtual void ChangeIF(std::wstring const& Contents, std::wstring const& Static1, std::wstring const& Static2, std::wstring const& Static3);
	virtual bool Input(WPARAM wParam, LPARAM lParam);

	explicit IFExportList(HWND hWnd) : IFBase(hWnd){};
	virtual ~IFExportList(void){};
};

class IFCreateID
	: public IFBase
{
public:
	virtual void ChangeIF(std::wstring const& Contents, std::wstring const& Static1, std::wstring const& Static2, std::wstring const& Static3);
	virtual bool Input(WPARAM wParam, LPARAM lParam);

	explicit IFCreateID(HWND hWnd) : IFBase(hWnd){};
	virtual ~IFCreateID(void){};
};

class IFDatVersionUp
	: public IFBase
{
public:
	virtual void ChangeIF(std::wstring const& Contents, std::wstring const& Static1, std::wstring const& Static2, std::wstring const& Static3);
	virtual bool Input(WPARAM wParam, LPARAM lParam);

	explicit IFDatVersionUp(HWND hWnd) : IFBase(hWnd){};
	virtual ~IFDatVersionUp(void){};
};

class IFDatVersionCheck
	: public IFBase
{
public:
	virtual void ChangeIF(std::wstring const& Contents, std::wstring const& Static1, std::wstring const& Static2, std::wstring const& Static3);
	virtual bool Input(WPARAM wParam, LPARAM lParam);

	explicit IFDatVersionCheck(HWND hWnd) : IFBase(hWnd){};
	virtual ~IFDatVersionCheck(void){};
};

class IFDatConvert
	: public IFBase
{
public:
	virtual void ChangeIF(std::wstring const& Contents, std::wstring const& Static1, std::wstring const& Static2, std::wstring const& Static3);
	virtual bool Input(WPARAM wParam, LPARAM lParam);

	explicit IFDatConvert(HWND hWnd) : IFBase(hWnd){};
	virtual ~IFDatConvert(void){};
};

class IFMakeManualPatch
	: public IFBase
{
public:
	virtual void ChangeIF(std::wstring const& Contents, std::wstring const& Static1, std::wstring const& Static2, std::wstring const& Static3);
	virtual bool Input(WPARAM wParam, LPARAM lParam);

	explicit IFMakeManualPatch(HWND hWnd) : IFBase(hWnd){};
	virtual ~IFMakeManualPatch(void){};
};

class IFMakeAutoPatch
	: public IFBase
{
public:
	virtual void ChangeIF(std::wstring const& Contents, std::wstring const& Static1, std::wstring const& Static2, std::wstring const& Static3);
	virtual bool Input(WPARAM wParam, LPARAM lParam);

	explicit IFMakeAutoPatch(HWND hWnd) : IFBase(hWnd){};
	virtual ~IFMakeAutoPatch(void){};
};