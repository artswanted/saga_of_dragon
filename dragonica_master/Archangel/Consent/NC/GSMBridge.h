/*
	
*/
#pragma once

#ifdef GSMBRIDGE_EXPORTS
#define GSMBRIDGE_API __declspec(dllexport)
#else
#define GSMBRIDGE_API __declspec(dllimport)
#endif

// This class is exported from the GSMBridge.dll

#ifdef __cplusplus
extern "C" {
#endif

/*! 통합인증 서버로 부터 받는 정보로, 포털게임에서의 각종 Flag정보를 가지고 있습니다.*/
struct  GSMBRIDGE_API PortalFlag 
{
	/*!로그인 실패 이유가  게임 관련 제재 일 경우 GameFlag 값중 innerBlock, outerBlock값을 확인하면 됩니다. 그 이외의 나머지 값은 0 으로 전달됩니다. (표 PortalWarning코드 참조)*/	
	int roleFlag;	
	/*!사용자의 경고 Flag값으로 로그인은 할 수 있습니다. (표 Portal Role 코드 참조)*/		
	int warnFlag;	
	/*!엔씨코인 과 관련된 즉 결제와 관련된 제재 Flag. 해당 값이 1로 Setting되어 있다면 로그인은 불가능 합니다. (표 Portal CoinBlock 코드 참조)*/
	int coinBlockFlag;	
	/*!포털 게임에 적용되는 제재 Flag. 해당 값이 Setting되어 있다면 로그인은 불가능 합니다. (표 Portal GameBlock 코드 참조)*/		
	int gameBlockFlag;	
	/*!사용자 인증 Flag값으로 auth값에 따라 로그인이 가능, 불가능이 정해집니다. (표 Portal AuthBlock 코드 참조)*/
	int	authBlockFlag;	
};

/*! 통합인증 서버로 부터 받는 정보로, 게임에서만 적용되는 각종 Flag정보를 가지고 있습니다.*/
struct  GSMBRIDGE_API GameFlag 
{
	/*!사용자의 로그인 권한 Flag값으로 해당 값으로 사내 계정, GM 계정, PC방 업주 등 로그인 권한과 관련된 값입니다. (표 Game Warning 코드 참조)*/
	int roleFlag;	
	/*!사용자의 경고 Flag값으로 로그인은 할 수 있습니다. (표 Game Role 코드 참조)*/
	int warnFlag;	
	/*!게임에서만 적용되는 게임 내 결제 Flag. 해당 값이 Setting되어 있다면 로그인은 불가능 합니다. 
	즉 A라는 게임에서 innerBlock 값이 setting된 사용자라도 
	B라는 게임에서는 B에서의 innerBlock값이 Setting되어 있지 않는다면 로그인이 가능합니다. (표 Game InnerBlock 코드 참조)*/
	int innerBlockFlag;	
	/*!게임에만 적용되는 게임 외 제재 Flag. 해당 값이 Setting되어 있다면 로그인은 불가능 합니다.
	즉 A라는 게임에서 outerBlock 값이 setting된 사용자라도 
	B라는 게임에서는 B에서의 outerBlock 값이 Setting되어 있지 않는다면 로그인이 가능합니다. (표 Game OuterBlock 코드 참조)*/
	int outerBlockFlag;	
};

/*!
	개별 사용자의 종류별 point를 manipulate하는 서버(이하 point서버)에 대한 operation 요청,응답에 사용되는 base 객체입니다.<br>
	point ID와 수량 정보를 담고 있습니다. 조회, 추가, 차감 operation에 사용됩니다.
*/
#pragma pack ( push, 1 )
struct GSMBRIDGE_API PointInfo
{
	/*!개별 Point의 구분 ID입니다.*/
	unsigned short mPointID;					
	/*!사용자가 가진 잔여 Point의 양을 의미합니다.*/
	__int64 mPointAmount;	//	Send시에 내부적으로 4 byte 형변환 요구(japan)
};

/*!
	PointInfo를 상속한 객체로 특정 point에 대해서 operation을 요청할 때 추가적인 정보를 제공합니다.<br>
	현재 지원되는 추가 정보는 일일 변동 제한 값과 추가 action code입니다.
*/

struct GSMBRIDGE_API PointUsageInfo : public PointInfo
{
	/*!일일 변동 제한치, 0인 경우 무제한 */
	unsigned int mDailyLimit;
	/*!적립에 대한 추가 Action Code, default: 0*/
	unsigned int mPointActionID;
};
#pragma pack ( pop )


/*!
	Point관련 Interface에서 공통으로 사용되는 구조체입니다. 내부적으로 n개의 개별 point에 대한 정보를 포함합니다.
*/
struct GSMBRIDGE_API ServerPointsInfo
{
	/*!사용자의 ID입니다.*/
	unsigned int mAccountID;	
	/*!리니지, 리니지2, Portal 계정 등급을 분류하는 계정 상태의 특이성을 명시하는 코드로 한국 전용입니다.
	default constructor에서 13(portal)로 세팅합니다.*/
	int mAffiliatedGameID;
	/*!해당하는 서비스의 서비스 코드를 명시합니다.*/
	unsigned int mGameID;	
	/*!적용될 pointArray에 포함되어 있는 PointInfo base 객체의 개수입니다.*/
	unsigned short mPointCount;
	/*!
		operation의 대상이 되는 Point들의 Array입니다.<br>
		각 Point들은 PointInfo 상속 객체이어야 하며, array는 모두 같은 종류의 PointInfo 상속 객체로 구성되어야 합니다.
	*/
	PointInfo* mpPoints;
	/*!PointInfo base 객체의 byte size입니다. <b>RTTI를 사용하지 않기 때문에 size는 명확한 Instance 판별에 사용됩니다.</b>*/
	size_t mSizeOfPointInfo;

	/*!
		default constructor입니다. 대부분의 국가에서 Affiliated Game ID는 13(portal service)로 설정되므로, 초기화를 수행합니다.<br>
		만약 다른 값으로 처리해야 하는 경우에는 명시적으로 값을 바꾸어 주어야 합니다.
	*/
	ServerPointsInfo ( void ) { mAffiliatedGameID = 13; };
};


/*GSMBRIDGE_API int  AllocTest();*/

/*!
	<summary>
	GSMBridge.dll의 최초 Load시 반드시 호출해야하는 메소드이며, Load에 실패하면 재호출이 필요합니다.<br>
	본 메소드는 다음과 같은 작업을 처리합니다.
	<list type="bullet">
	<item>Config.xml설정사항 Load</item>
	<item>Log 및 암호화 관련 설정 작업</item>
	<item>GSM Connection 처리</item>
	<item>Packet처리를 위한 설정 작업</item>
	
	<b><i>remark</i></b><br>
	<list type="bullet">
	<item>로그를 남기지 않고 false를 리턴했을 경우, 확인해봐야하는 사항들은 다음과 같습니다.
		<list type="number">
		<item>callerDirPath가 없는 경우</item>
		<item>callerDirPathLength<0 혹은 callerDirPathLength>512인 경우</item>
		<item><callerDirPath>\BridgeConfig\Config.xml 파일을 읽을 수 없는 경우</item>
		<item>Config.xml의 파일 내용이 잘못된 경우</item>
		<item>Config.xml의 <Log><Folder>에 설정된 디렉토리가 생성되어있지 않은 경우</item>
		</list>
	</item>
	</list>
	<list type="bullet">
	<item>로그를 남기고 false를 리턴했을 경우, 확인해봐야하는 사항들은 다음과 같습니다.
		<list type="number">
		<item>Packet Cipher관련 에러일 경우</item>
		<item><callerDirPath>\\BridgeConfig\\Error.xml이 없을 경우</item>
		<item><callerDirPath>\\BridgeConfig\\BlockMsg.xml이 없을 경우</item>
		<item><callerDirPath>\\BridgeConfig\\CommerceError.xml이 없을 경우</item>
		<item><callerDirPath>\\BridgeConfig\\GSMError.xml이 없을 경우</item>
		</list>
	</item>
	</list>
	</summary>
	
	<param name="callerDirPath">[in]DLL이 존재하는 폴더의 전체 경로명, 최대 512자 까지 허용합니다. 즉 DLL을 링크하는 Application 프로그램 위치에 같이 존재한다면 Application 프로그램 Directory와 같습니다.</param>
	<param name="callerDirPathLength">[in]DLL이 존재하는 폴더의 전체 경로명의 길이를 의미합니다.</param>
	
	<returns>
	<list>
		<item><c>true</c> -DLL Load 성공</item>
		<item><c>false</c> -DLL Load 실패(재시도해야 함)</item>
	</list>
	</returns>

	<list type="table">
	<listheader>
	<description>Sample Code</description>
	</listheader>
	<item>
	<description><pre><code>

	wchar_t DirPath[256] = {0};
	GetCurrentDirectory(sizeof(DirPath), DirPath);

	BOOL Started = FALSE;
	Started = BridgeDLLStartup(szDirPath, sizeof(szDirPath));
	if (Started == FALSE)
		return FALSE;
	else
		return TRUE;

	</code></pre></description>
	</item>
	</list>
	<seealso cref="BridgeDLLTerminate"/>
*/
GSMBRIDGE_API bool BridgeDLLStartup(wchar_t *callerDirPath,int callerDirPathLength );

/*!
	<summary>
	GSMBridge.dll의 종료시 반드시 호출해야하는 메소드입니다.<br>
	본 메소드는 다음과 같은 작업을 처리합니다.<br>
	<list type="bullet">
	<item>소켓 및 로그 메모리 정리 작업</item>
	</summary>
	
	<returns>
	<list>
		<item><c>void</c></item>
	</list>
	</returns>

	<list type="table">
	<listheader>
	<description>Sample Code</description>
	</listheader>
	<item>
	<description><pre><code>

	if(BridgeDLLStarted)
		BridgeDLLTerminaate();

	</code></pre></description>
	</item>
	</list>
	<seealso cref="BridgeDLLStartup"/>
*/
GSMBRIDGE_API void BridgeDLLTerminate(void);

/*!
	<summary>
	웹에서의 로그인을 통해 게임에 별도의 로그인 없이, 로그인을 요청하는 메소드입니다.<br>
	<b><i>remark</i></b><br>
	<list type="bullet">
	<item>PC방 관련 과금 혹은 이벤트를 하기 위해서는 LoginWithWebSID2를 이용해야 합니다.</item>
	<item>MacAddr의 경우, 기본값을 0으로 넘기도록 합니다.(현재는 mac체크를 하지 않음)</item>
	<item>WSM으로부터 전달받은 WebSessionID는 BSTR타입의 HexString이므로, 이를 Byte Array로 변환해서 넘겨줘야 합니다.</item>
	<item>[out]인자 중, gusID는 게임세션ID로, GSM-GSMBridgeDll간의 모든 통신에 사용됩니다. 그러므로, 게임인증 중계서버에서는 반드시 <gusID>-<userID>를 키값으로 하는 해쉬테이블을 관리해야합니다.</item>
	<item>GSM으로부터 비동기로 받은 AccountKick이나 AlarmCoin메시지의 경우, 내부에 gusID만을 가지고 있기 때문에, 해쉬테이블을 통해 UserID를 찾을 수 있어야 합니다.</item>
	</list>
	</summary>
	
	<param name="webSID">[in]웹에서 로그인 할 때의 Web Session ID값 입니다.</param>
	<param name="macAddr">[in]웹에서 로그인 할 때의 사용자의 Mac Address 입니다. (반드시 0을 넘김)</param>
	<param name="gusID">[out]사용자의 게임SessionID 입니다. 실패하면 16자 전부 0인 값으로 전달되며, 성공할 경우에만 정확한 값이 전달됩니다. 로그인 이후 모든 패킷은 이 gusID를 통해 사용자의 인증을 체크합니다.</param>
	<param name="account">[out]사용자의 account 입니다. 실패하면 NULL값이 전달됩니다. NULL을 포함한 최대 51자(102Byte) 길이를 갖습니다.</param>
	<param name="userID">[out]사용자의 userID값 입니다. 실패하면 0값이 전달됩니다.</param>
	<param name="pPortalFlag">[out]포털 게임에서의 각종Flag 정보를 가지고 있는 PortalFlag객체의 Pointer입니다.</param>
	<param name="pGameFlag">[out]게임에서만 적용되는 각종Flag 정보를 가지고 있는 GameFlag객체의 Pointer입니다. 로그인 실패 이유가  게임 관련 제재 일 경우 GameFlag 값중 innerBlock, outerBlock값을 확인하면 됩니다. 그 이외의 나머지 값은 0 으로 전달됩니다.</param>
	<param name="birthDay">[out]사용자의 생년월일(YYYY*10000 + mm*100 + dd) 입니다.</param>
	
	<returns>
	<list>
		<item>0 성공</item>
		<item>not 0 실패, 해당 return code에 대한 정확한 error message는 GetErrorMsg 함수를 사용하여 얻을 수 있습니다.</item>
		<item>ErrorCode가 5번일 경우에는 PortalFlag값 중 gameBlock에 상세정보가 전달됩니다.</item>
		<item>ErrorCode가 6번일 경우에는 GameFlag값 중 innerBlock에 상세정보가 전달됩니다.</item>
		<item>ErrorCode가 7번일 경우에는 GameFlag값 중 outerBlock에 상세정보가 전달됩니다.</item>
		<item>[참고]BlockCode의 우선순위는 gameBlock > innerBlock > outerBlock 입니다.</item>
	</list>

	<list type="table">
	<listheader>
	<description>Sample Code</description>
	</listheader>
	<item>
	<description><pre><code>

	WCHAR webHexaString[WSID_HEXA_LEN] = {0};
	BYTE webSessionId[WSID_LEN] = {0};
	BYTE gameSessionId[GSID_LEN] = {0};
	WCHAR account[ACCOUNT_LEN] = {0};
	PortalFlag portalFlag = {0};
	GameFlag gameFlag = {0};
	int UserID = 0;
	int birthDay = 0;

	BOOL bResult = LoginByWSM(webHexaString);
	ConvertHexaToBinary(webSessionId, sizeof(webSessionId), webHexaString);

	if (bResult == 0)
	{		
		int result = LoginWithWebSID(webSessionId, 0, gameSessionId, account, &UserID, &portalFlag, &gameFlag, &birthDay);
		if (result == 0)
			//LoginWithWebSID Success
		else
			//LoginWithWebSID Fail

	}
	
	</code></pre></description>
	</item>
	</list>
	<seealso cref="LoginWithWebSID2"/>
	<seealso cref="LoginWithIDPWD"/>
	<seealso cref="LogOut"/>
*/
// 웹세션(WSM서버에서 생성) 아이디로 로그인
GSMBRIDGE_API int  LoginWithWebSID(unsigned char* webSID,__int64 macAddr ,unsigned char* gusID, wchar_t* account, 
								   int* userID, PortalFlag * pPortalFlag, GameFlag *pGameFlag, int *birthDay);
/*!
	<summary>
	PC방 과금 혹은 이벤트를 위하여 웹에서의 로그인을 통해 게임에 별도의 로그인 없이 로그인을 요청하는 메서드 입니다.<br>
	인자로 넘기는 IP를 체크하여 PC방에서 로그인하는지를 확인합니다.<br>
	<b><i>remark</i></b><br>
	<list type="bullet">
	<item>PC방 관련 과금 혹은 이벤트를 하기 위해서는 LoginWithWebSID2를 이용해야 합니다.</item>
	<item>WSM으로부터 전달받은 WebSessionID는 BSTR타입의 HexString이므로, 이를 Byte Array로 변환해서 넘겨줘야 합니다.</item>
	<item>userIP로 PC방 이용자인지를 체크하고 과금하게 되므로 반드시 로그인하려는 사용자의 IP를 넘겨야 합니다.</item>
	<item>[out]인자 중, gusID는 게임세션ID로, GSM-GSMBridgeDll간의 모든 통신에 사용됩니다. 그러므로, 게임인증 중계서버에서는 반드시 <gusID>-<userID>를 키값으로 하는 해쉬테이블을 관리해야합니다.</item>
	<item>GSM으로부터 비동기로 받은 AccountKick이나 AlarmCoin메시지의 경우, 내부에 gusID만을 가지고 있기 때문에, 해쉬테이블을 통해 UserID를 찾을 수 있어야 합니다.</item>
	</list>
	</summary>
	
	<param name="webSID">[in]웹에서 로그인 할 때의 Web Session ID값 입니다.</param>
	<param name="userIP">[in]웹에서 로그인 할 때의 사용자의 접속IP 입니다. (PC방 과금, 혹은 이벤트를 위해서 반드시 필요)</param>
	<param name="gusID">[out]사용자의 게임SessionID 입니다. 실패하면 16자 전부 0인 값으로 전달되며, 성공할 경우에만 정확한 값이 전달됩니다. 로그인 이후 모든 패킷은 이 gusID를 통해 사용자의 인증을 체크합니다. </param>
	<param name="account">[out]사용자의 account 입니다. 실패하면 NULL값이 전달됩니다. NULL을 포함한 최대 51자(102Byte) 길이를 갖습니다.</param>
	<param name="userID">[out]사용자의 userID값 입니다. 실패하면 0값이 전달됩니다.</param>
	<param name="pPortalFlag">[out]포털 게임에서의 각종Flag 정보를 가지고 있는 PortalFlag객체의 Pointer입니다.</param>
	<param name="pGameFlag">[out]게임에서만 적용되는 각종Flag 정보를 가지고 있는 GameFlag객체의 Pointer입니다. 로그인 실패 이유가  게임 관련 제재 일 경우 GameFlag 값중 innerBlock, outerBlock값을 확인하면 됩니다. 그 이외의 나머지 값은 0 으로 전달됩니다.</param>
	<param name="birthDay">[out]사용자의 생년월일(YYYY*10000 + mm*100 + dd) 입니다.</param>
	
	<returns>
	<list>
		<item>0 성공</item>
		<item>not 0 실패, 해당 return code에 대한 정확한 error message는 GetErrorMsg 함수를 사용하여 얻을 수 있습니다.</item>
		<item>ErrorCode가 5번일 경우에는 PortalFlag값 중 gameBlock에 상세정보가 전달됩니다.</item>
		<item>ErrorCode가 6번일 경우에는 GameFlag값 중 innerBlock에 상세정보가 전달됩니다.</item>
		<item>ErrorCode가 7번일 경우에는 GameFlag값 중 outerBlock에 상세정보가 전달됩니다.</item>
		<item>[참고]BlockCode의 우선순위는 gameBlock > innerBlock > outerBlock 입니다.</item>
	</list>

	<list type="table">
	<listheader>
	<description>Sample Code</description>
	</listheader>
	<item>
	<description><pre><code>

	WCHAR webHexaString[WSID_HEXA_LEN] = {0};
	BYTE webSessionId[WSID_LEN] = {0};
	BYTE gameSessionId[GSID_LEN] = {0};
	WCHAR account[ACCOUNT_LEN] = {0};
	PortalFlag portalFlag = {0};
	GameFlag gameFlag = {0};
	int UserID = 0;
	int birthDay = 0;
	char ipAddress[IP_LEN] = {0};

	BOOL bResult = LoginByWSM(webHexaString, sizeof(webHexString));
	GetWSMIPAddress(ipAddress, sizeof(ipAddress));
	ConvertHexaToBinary(webSessionId, sizeof(webSessionId), webHexaString);
	if (bResult == 0)
	{		
		GetUserIPAdress(ipAddress, sizeof(ipAddress));
		int result = LoginWithWebSID(webSessionId, ipAddress, gameSessionId, account, &UserID, &portalFlag, &gameFlag, &birthDay);
		if (result == 0)
			//LoginWithWebSID2 Success
		else
			//LoginWithWebSID2 Fail
	}

	</code></pre></description>
	</item>
	</list>
	<seealso cref="LoginWithWebSID"/>
	<seealso cref="LoginWithIDPWD"/>
	<seealso cref="LogOut"/>
*/
// 웹세션(WSM서버에서 생성) 아이디로 로그인
GSMBRIDGE_API int  LoginWithWebSID2(unsigned char* webSID,unsigned long userIP,unsigned char* gusID, wchar_t* account, 
								   int* userID, PortalFlag * pPortalFlag, GameFlag *pGameFlag, int *birthDay);
/*!
	<summary>
	웹 로그인의 결과로 받아온 WebSessionID를 사용하여 게임로그인을 하지 않고, 직접 게임 클라이언트로부터 로그인을 할 경우에 호출되는 메서드 입니다.<br>
	웹 로그인이 불가능한 환경에서 테스트용으로 사용됩니다.<br>
	<b><i>remark</i></b><br>
	<list type="bullet">
	<item>실서비스에서는 사용하지 않는 테스트용 인터페이스 입니다.</item>
	<item>MacAddr의 경우, 기본값을 0으로 넘기도록 합니다.(현재는 mac체크를 하지 않음)</item>
	<item>userIP는 0보다 큰 값을 넘겨야 합니다.</item>
	<item>[out]인자 중, gusID는 게임세션ID로, GSM-GSMBridgeDll간의 모든 통신에 사용됩니다. 그러므로, 게임인증 중계서버에서는 반드시 <gusID>-<userID>를 키값으로 하는 해쉬테이블을 관리해야합니다.</item>
	<item>GSM으로부터 비동기로 받은 AccountKick이나 AlarmCoin메시지의 경우, 내부에 gusID만을 가지고 있기 때문에, 해쉬테이블을 통해 UserID를 찾을 수 있어야 합니다.</item>
	</list>
	</summary>
	
	<param name="account">[in]사용자의 account값 입니다. NULL을 포함한 최대 16자(32Bytes) 길이를 갖습니다.</param>
	<param name="password">[in]로그인 하려는 account의 password값 입니다. NULL을 포함한 최대 17자(34Bytes) 길이를 갖습니다.</param>
	<param name="macAddr">[in]사용자의 MacAddress.</param>
	<param name="userIP">[in]사용자의 접속IP 입니다.</param>
	<param name="gusID">[out]사용자의 게임SessionID 입니다. 실패하면 16자 전부 0인 값으로 전달되며, 성공할 경우에만 정확한 값이 전달됩니다. 로그인 이후 모든 Packet은 gusID를 통해 사용자의 인증을 체크합니다.</param>
	<param name="userID">[out]사용자의 userID값 입니다. 로그인 실패할 경우 0값이 전달됩니다.</param>
	<param name="pPortalFlag">[out]포털 게임에서의 각종Flag 정보를 가지고 있는 PortalFlag객체의 Pointer입니다. 로그인 실패 이유가 포털 관련 제재 일 경우 PortalFlag값 중 coinBlock, gameBlock 혹은 authBlockFlag값을 확인하면 됩니다. 그 이외의 나머지 값은 0으로 전달됩니다.</param>
	<param name="pGameFlag">[out]게임에서만 적용되는 각종Flag 정보를 가지고 있는 GameFlag객체의 Pointer입니다. 로그인 실패 이유가 게임 관련 제재 일 경우 GameFlag값중 innerBlock, outerBlock 값을 확인하면 됩니다. 그 이외의 나머지 값은 0으로 전달됩니다.</param>
	<param name="birthDay">[out]사용자의 생년월일(YYYY*10000 + mm*100 + dd) 입니다.</param>
	
	<returns>
	<list>
		<item>0 성공</item>
		<item>not 0 실패, 해당 return code에 대한 정확한 error message는 GetErrorMsg 함수를 사용하여 얻을 수 있습니다.</item>
		<item>ErrorCode가 5번일 경우에는 PortalFlag값 중 gameBlock에 상세정보가 전달됩니다.</item>
		<item>ErrorCode가 6번일 경우에는 GameFlag값 중 innerBlock에 상세정보가 전달됩니다.</item>
		<item>ErrorCode가 7번일 경우에는 GameFlag값 중 outerBlock에 상세정보가 전달됩니다.</item>
		<item>[참고]BlockCode의 우선순위는 gameBlock > innerBlock > outerBlock 입니다.</item>
	</list>

	<list type="table">
	<listheader>
	<description>Sample Code</description>
	</listheader>
	<item>
	<description><pre><code>

	WCHAR account[ACCOUNT_LEN] = {0};
	WCHAR pwd[PASSWORD_LEN] = {0};
	BYTE gameSessionId[GSID_LEN] = {0};
	PortalFlag portalFlag = {0};
	GameFlag gameFlag = {0};
	int userId = 0;
	int birthDay = 0;

	unsigned long userIP = GetUserIP(account);
	wcscpy ( account, L"testAccount" );
	wcscpy ( pwd, L"abc123" );	

	int result = LoginWithIDPWD(account, pwd, 0, userIP, gameSessionId, &userId, &portalFlag, &gameFlag, &birthDay);
	if (result == 0)
		//LoginWithIDPWD Success
	else
		//LoginWithIDPWD Fail

	</code></pre></description>
	</item>
	</list>
	<seealso cref="LoginWithWebSID"/>
	<seealso cref="LoginWithWebSID2"/>
	<seealso cref="LogOut"/>
*/
// 패스워드 아이디로 직접 로그인
GSMBRIDGE_API int  LoginWithIDPWD(wchar_t *account,wchar_t* pwd ,__int64 macAddr, unsigned long userIP,unsigned char* gusID, int* userID,
								   PortalFlag * pPortalFlag, GameFlag *pGameFlag, int *birthDay);

/*!
	<summary>
	로그인한 사용자의 부가정보를 얻어오기 위한 함수로, Login을 통해 얻어온 gusID를 인자로 넘겨 해당 사용자의 부가 정보를 확인하는 메소드 입니다.<br>
	<b><i>remark</i></b><br>
	<list type="bullet">
	<item>사용자의 정보를 얻어올 때 당사자임을 증명하기 위해서는 반드시 userID가 아닌 gusID를 넘겨줘야 합니다.</item>
	<item>사용자의 정보를 얻어오기 위해서는 GetUserOneInfo가 아닌, 이 인터페이스를 사용해야 합니다.</item>
	<item>GetUserOneInfo는 하위호환성을 위한 인터페이스로, 실제 사용되지 않습니다.</item>
	</list>
	</summary>
	
	<param name="gusID">[in]사용자의 게임SessionID 입니다.</param>
	<param name="nickName">[out]포털 전체에서 사용하는 사용자의 NickName 입니다. NULL을 포함한 최대 51자(102Bytes) 길이를 갖습니다.</param>
	<param name="gender">[out]사용자의 성별(1:남성, 2:여성) 입니다.</param>
	<param name="city">[out]해당 사용자의 사는 도시(포털 가입 당시 사용자가 입력한 도시) 입니다. </param>
	<param name="userGrade">[out]사용자의 포털등급 입니다. </param>
	
	<returns>
	<list>
		<item>0 성공</item>
		<item>not 0 실패, 해당 return code에 대한 정확한 error message는 GetErrorMsg 함수를 사용하여 얻을 수 있습니다.</item>
		<item>Error.xml에 있는 에러목록에 없을 경우 기본 에러 값으로 -1 입니다.</item>
	</list>

	<list type="table">
	<listheader>
	<description>Sample Code</description>
	</listheader>
	<item>
	<description><pre><code>
	
	BYTE gameSessionId[GSID_LEN] = {0};
	wchar_t NickName[NICKNAME_LEN] = {0};
	BYTE Gender = 0;
	BYTE City = 0;
	int UserGrade = 0;

	BOOL bResult = GetGameSession(gameSessionId, sizeof(gameSessionId));
	if (bResult == 0)
	{
		int result = GetUserInfo(gameSessionId, NickName, &Gender, &City, &UserGrade);

		if (result == 0)
			//GetUserInfo Success
		else 
			//GetUserInfo Fail
	}

	</code></pre></description>
	</item>
	</list>
*/
GSMBRIDGE_API int GetUserInfo(unsigned char* gusID, wchar_t* nickName, BYTE *gender, BYTE* city, int *userGrade);

/*!
사용되지 않는 메서드 입니다.
*/
GSMBRIDGE_API int GetUserOneInfo(unsigned char* gusID,BYTE infoType, short bufferSize, BYTE *dataType,char*  oneData,int * dataLength);

/*!
	<summary>
	사용자의 게임 로그아웃을 요청하는 메소드 입니다.<br>
	사용자가 게임에 로그인 후 발급받은 GameSessionID를 통해서만 요청할 수 있습니다.<br>
	
	<b><i>remark</i></b><br>
	<list type="bullet">
	<item>시간제Base과금의 게임일 경우에는 반드시 과금 종료를 요청하고 Logout함수를 호출해야 합니다.</item>
	<item>해당 게임세션을 종료하기 위해서는 반드시 gusID를 넘겨줘야 합니다.</item>
	<item>게임 로그아웃 시에 호출하지 않을 경우, 기존 세션이 남아 다음 로그인시 이중로그인 처리가 될 수 있으므로 반드시 호출해줘야 합니다.</item>
	<item>만약, 이 인터페이스의 호출이 실패했을 경우엔 별도의 처리를 하지 않습니다.</item>
	<item>이 경우엔 이중로그인이 되어 다음번 최초로그인은 Kick되고 두번째 시도시엔 성공하게되므로 큰 영향은 없습니다.</item>
	</list>
	<list type="bullet">
	<item>이중 로그인 시 내부 처리방식은 다음과 같습니다.
		<list type="bullet">
		<item>기존 세션은 삭제하고, 게임인증 중계서버로 사용자 Kick을 보냅니다.</item>
		<item>이중로그인 시도 실패를 리턴합니다.(다음 로그인 시도는 성공)</item>
		<item>내부적으로 로그아웃이 실패하는 경우는 다음과 같습니다. 아래 경우에 대해서는, 0(성공)을 리턴합니다.</item>
		<item>잘못된 게임아이디</item>
		<item>이중 로그인인 경우</item>
		<item>잘못된 게임세션인 경우</item>
		</list>
	</item>
	</list>
	</summary>
	
	<param name="gusID">[in]사용자의 게임SessionID 입니다.</param>
	
	<returns>
	<list>
		<item>0 성공</item>
		<item>not 0 실패, 해당 return code에 대한 정확한 error message는 GetErrorMsg 함수를 사용하여 얻을 수 있습니다.</item>
		<item>실패 리턴의 의미가 없기 때문에 무조건 0(성공)을 리턴합니다.</item>
	</list>

	<list type="table">
	<listheader>
	<description>Sample Code</description>
	</listheader>
	<item>
	<description><pre><code>

	BYTE gameSessionId[GSID_LEN] = {0};

	BOOL bResult = GetGameSession(gameSessionId, sizeof(gameSessionId));
	if (bResult == 0)
	{
		int result = LogOut(gameSessionId);

		if (result == 0)
			//Logout Success
		else 
			//Logout Fail
	}

	</code></pre></description>
	</item>
	</list>
	<seealso cref="LoginWithWebSID"/>
	<seealso cref="LoginWithWebSID2"/>
	<seealso cref="LoginWithIDPWD"/>
*/
GSMBRIDGE_API int LogOut(unsigned char* gusID);

/*!
	<summary>
	사용자의 남은 유료, 무료 코인을 알아오기위해 요청하는 메소드 입니다.<br>
	사용자가 게임에 로그인 후 발급받은 GameSessionID를 통해서만 요청할 수 있습니다.<br>
	
	<b><i>remark</i></b><br>
	userID가 아닌 gusID를 사용해 잔여코인을 요청하기 때문에 인증중계서버에서는 반드시 gusID-userID 쌍을 가지고 있어야합니다.<br>
	</summary>
	
	<param name="gusID">[in]사용자의 게임SessionID 입니다.</param>
	<param name="paidCoin">[out]사용자의 남은 유료코인 입니다.</param>
	<param name="freeCoin">[out]사용자의 남은 포털 무료코인 입니다.</param>
	<param name="gameFreeCoin">[out]사용자가 접속한 게임에서만 사용 가능한 무료코인 입니다.</param>
	<param name="Mileage">[out]사용자의 사용과 관련된 마일리지 입니다.</param>
	
	<returns>
	<list>
		<item>0 성공</item>
		<item>not 0 실패, 해당 return code에 대한 정확한 error message는 GetErrorMsg 함수를 사용하여 얻을 수 있습니다.</item>
		<item>Error.xml에 있는 에러목록에 없을 경우 기본 에러 값으로 -1 입니다.</item>
	</list>

	<list type="table">
	<listheader>
	<description>Sample Code</description>
	</listheader>
	<item>
	<description><pre><code>

	BYTE gameSessionId[GSID_LEN] = {0};
	int PaidCoin = 0;
	int FreeCoin = 0;
	int GameFreeCoin = 0;
	int Mileage = 0;

	BOOL bResult = GetGameSession(gameSessionId, sizeof(gameSessionId));
	if (bResult == 0)
	{
		int result = GetRemainCoin(gameSessionId, &PaidCoin, &FreeCoin, &GameFreeCoin, &Mileage);

		if (result == 0)
			//GetRemainCoin Success
		else 
			//GetRemainCoin Fail
	}

	</code></pre></description>
	</item>
	</list>
*/
GSMBRIDGE_API int GetRemainCoin(unsigned char* gusID, int *paidCoin,int * freeCoin,int *gameFreeCoin, int *mileage);

// 1) GSM에서 Admin소켓을 통해 패킷요청을 받음
// 2) GSM이 게임서버로 패킷(예,Kick)을 보낸다.
// 3) 게임서버는 OnRecv를 호출해 패킷을 처리	OnRecvPacketFromGSM()
/*!
	<summary>
	ListenPort를 이용해서 Async한 데이터를 받아 BridgeDLL로 전달하는 메소드 입니다.<br>
	GSMBridgeDLL은 해당 정보를 받아 Parsing해서 게임서버에서 결과를 알 수 있도록 특정 Type을 돌려줍니다.<br>
	데이터를 받은 게임 서버는 해당 데이터를 적절한 타입으로 casting 해서 결과를 돌려받아 사용하면 됩니다.<br>
	본 메소드는 다음과 같은 작업을 처리합니다.<br>
	<list type="bullet">
	<item>파라미터(packetLength<2 || 포인터변수==NULL) 체크합니다. Invalid할 경우 false를 리턴(현재는 false를 리턴하게 되어 있어서, 0이 리턴되고 있습니다. 추후 수정될 예정)</item>
	<item>Config.xml에 Cipher부분이 0보다 큰 값일 경우, 패킷 암호화가 적용된 상태이기 때문에, 암호화된 패킷을 복호화하는 과정을 거칩니다. (그러나 현재 패킷암호화는 사용되고 있지 않음)</item>
	<item>패킷으로부터 프로토콜번호를 얻어옵니다</item>
	<item>프로토콜 번호에 따라 패킷 파싱을 합니다. 현재 처리하는 패킷의 종류는 다음과 같습니다.
		<list type="bullet">
		<item>사용자 Kick (castType = 1)</item>
		<item>코인변화 알림 (castType = 6)</item>
		</list>
	</item>
	</list>
	<list type="bullet">
	<item>GSM서버로부터 받은 패킷의 종류가 위의 두 가지에 포함되어 있지 않을 경우, 16번 에러를 리턴합니다.</item>
	<item>그 다음부터는 개발자가 아래 “사용법” 에서 안내하는대로, 넘겨받은 패킷을 적절하게 type casting하여 사용하면 됩니다.<br>
		  사용법<br>
		  ListenPort (10008번 포트)로부터 받은 패킷을, OnRecv 함수에 packet과 packetLength로 나누어 인자로 넣어 호출합니다.<br>
		  OnRecv함수에서는 다음의 세 가지를 넘겨줍니다.<br>
		  <list type="bullet">
		  <item>castType에는 cast해야 될 Type을 integer로 넘김 </item>
		  <item>resultData에 파싱된 패킷을 넘김</item>
		  <item>resultSize에 패킷사이즈를 넘김</item>
		  </list>
	</item>
	</list>
	
	<b><i>remark</i></b><br>
	OnRecv함수를 사용하기 위한 상세한 전제조건은 {{LINK|EXAMPLE|method.onrecv|ListenPort를 이용한 데이터 인터페이스}}를 참조하시기 바랍니다.<br>
	다음은 예제에서 사용된 castType별 구조체입니다.<br> 
	GSMBridgeDll에서는 현재 해당 구조체를 별도의 헤더파일로 지원하고 있지 않으므로, 아래의 내용을 헤더에 추가하여 사용하여야 합니다.<br>
	아래 내용은 [표 OnRecv 함수의 CastType]에도 명시되어 있습니다.<br>
	[표 OnRecv 함수의 CastType]에서는 castType을 0~6까지 명시하고 있으나, 실제로 GSM에서 보내는 메시지는 1, 6 두가지입니다. 
	나머지 경우는 현재는 사용되고 있지 않으나, 앞으로 추가될 가능성이 있는 경우이므로, 구현할 필요는 없지만, 참고가 필요합니다.<br>
	<list type="table">
	<listheader>
	<description>castType별 구조체 Sample Code</description>
	</listheader>
	<item>
	<description><pre><code>

	#pragma pack(push, 1)

	struct AccountKick
	{
		BYTE gusID[16];
		BYTE reason;
		int userID;
	};

	struct AlarmCoin
	{
		BYTE gusID[16];
		int paidCoin;
		int freeCoin;
	};

	#pragma pack(pop)

	</code></pre></description>
	</item>
	</list>

	</summary>
	
	<param name="paacket">[in]Listen Port를 통해 GSMBridge로 들어온 데이터로 Packet의 Length를 제외한 Payload만을 의미합니다.(최대 8K정도의 Buffer)</param>
	<param name="packetLength">[in]Listen Port를 통해 GSMBridge로 들어온 데이터 packet의 Length입니다. Packet의 Length에서 Length Size인 2bytes를 제외한 Payload Size만을 의미합니다.</param>
	<param name="castType">[out]Packet을 Parsing한 결과값에 대한 Type 입니다. 응답결과가 실패라면 해당 데이터는 0값으로 전송됩니다.(표OnRecv 함수의 CastType 참조)</param>
	<param name="resultData">[out]Packet을 Parsing한 결과값 입니다. castType값으로 reinterpret_cast를 이용해서 결과값을 전송 받습니다. 응답 결과가 실패라면 해당 데이터는 NULL값으로 전송됩니다.(표OnRecv 함수의 CastType 참조)</param>
	<param name="resultDataSize">[out]Packet을 Parsing한 결과값에 대한 size 입니다. 응답 결과가 실패라면 해당 데이터는 0값으로 전송됩니다.(표OnRecv 함수의 CastType 참조)</param>
	
	<returns>
	<list>
		<item>0 성공</item>
		<item>not 0 실패, 해당 return code에 대한 정확한 error message는 GetErrorMsg 함수를 사용하여 얻을 수 있습니다.</item>
		<item>Error.xml에 있는 에러목록에 없을 경우 기본 에러 값으로 -1 입니다.</item>
	</list>

	<list type="table">
	<listheader>
	<description>Sample Code</description>
	</listheader>
	<item>
	<description><pre><code>

	ListenPort로부터 데이터를 받았을 경우,

	char* resultData = new char[nPacketLength];
	memset(resultData, 0, nPacketLength);

	int castType = 0;
	int resultDataSize = 0;

	OnRecv함수 호출
	int nResult = OnRecv((unsigned char*)(IOData->wsaBuf.buf + 2), nPayloadLength, &castType, resultData, &resultDataSize);

	switch (castType)
	{
		case 0:
					 break;
		case 1:
		{
				//OnRecv의 output parameter인 castType이 1인 경우, AccountKick 메시지임.
				//resultData를 AccountKick Structure로 casting한 후 
				//AccountKick에 있는 사용자 정보 및 Kick 이유를 사용하여, 게임 Kick을 구현하면 됨
				AccountKick* pAccountKick = reinterpret_cast<AccountKick*>(resultData);
		}
					break;
		case 6:
		{
				//OnRecv의 output parameter인 castType이 6인 경우, 코인변화알림 메시지임.
				//resultData를 AlarmCoin Structure로 casting한 후 
				//AlarmCoin에 있는 사용자 게임세션과 유무료 코인값을 이용하여, 
				//코인변화를 적용할 필요가 있는 곳에 적용하면 됨
				AlarmCoin* pAlarmCoin = reinterpret_cast<AlarmCoin*>(resultData);
		}
					break;
	}

	delete[] resultData;

	</code></pre></description>
	</item>
	</list>
*/
GSMBRIDGE_API int OnRecv(unsigned char*  packet, int packetLength, int * castType,void * resultData,int * resultDataSize);

/*!
	<summary>
	사용자가 접속한 IP가 PCCafe에서의 요청인지 확인하는 메소드 입니다.<br>
	<b><i>remark</i></b><br>
	PCCafeCode > 0 이면 PCCafe에서 접속 요청을 한 것입니다.<br>
	PCCafeCode가 0 이면 해당 사용자는 PC Cafe에서 접속하지 않았음을 의미합니다.<br>
	</summary>
	
	<param name="gusID">[in]사용자의 게임SessionID 입니다.</param>
	<param name="userIP">[in]사용자의 접속IP 입니다.</param>
	<param name="PCCafeCode">[out]PC Cafe Code(0이면 PC Cafe에서 접속하지 않았음을 의미) 입니다.</param>
	<param name="PCCafeGrade">[out]PC Cafe Grade(현재는 모든값이 0) 입니다.</param>
	
	<returns>
	<list>
		<item>0 성공</item>
		<item>not 0 실패, 해당 return code에 대한 정확한 error message는 GetErrorMsg 함수를 사용하여 얻을 수 있습니다.</item>
		<item>Error.xml에 있는 에러목록에 없을 경우 기본 에러 값으로 -1 입니다.</item>
	</list>

	<list type="table">
	<listheader>
	<description>Sample Code</description>
	</listheader>
	<item>
	<description><pre><code>

	BYTE gameSessionId[GSID_LEN] = {0};
	int UserIP = 0;
	int PCCafeCode = 0;
	int PCCafeGrade = 0;

	BOOL bResult = GetGameSession(gameSessionId, sizeof(gameSessionId));
	UserIP = inet_addr (GetUserIP(gameSessionId));
	if (bResult == 0)
	{
		int result = CheckPCCafeIP(gameSessionId, UserIP, &PCCafeCode, &PCCafeGrade);

		if (result == 0)
			//CheckPCCafeIP Success
		else 
			//CheckPCCafeIP Fail
	}

	</code></pre></description>
	</item>
	</list>
*/
GSMBRIDGE_API int CheckPCCafeIP(unsigned char *gusID, unsigned long userIP, int *pccafeCode , int *pccafeGrade);

/*!
	<summary>
	GameSessionID로 WebSessionID를 알아오기 위한 메소드 입니다.<br>
	<b><i>remark</i></b><br>
	보통, 웹로그인 후 받아온 웹세션ID로 게임로그인을 시도해서 게임세션ID를 받아왔을 때 게임인증서버에서는 웹세션ID를 저장해두지 않습니다.<br>
	그러나 게임클라이언트에서 웹과 연동이 필요할 경우,(ex, 게임클라이언트의 인게임 쇼핑몰을 웹으로 구현했을 경우) 웹세션ID를 넘겨야 할 경우가
	생기기 때문에, 그러한 경우 이 인터페이스를 사용하여 웹세션ID를 알아오도록 합니다.<br>
	</summary>
	
	<param name="gusID">[in]사용자의 게임SessionID 입니다.</param>
	<param name="webSessionID">[out]사용자의 WebSessionID 입니다.</param>
	<param name="macAddr">[out]사용자의 macAddress(현재는 모두 0) 입니다.</param>
	
	<returns>
	<list>
		<item>0 성공</item>
		<item>not 0 실패, 해당 return code에 대한 정확한 error message는 GetErrorMsg 함수를 사용하여 얻을 수 있습니다.</item>
		<item>Error.xml에 있는 에러목록에 없을 경우 기본 에러 값으로 -1 입니다.</item>
	</list>

	<list type="table">
	<listheader>
	<description>Sample Code</description>
	</listheader>
	<item>
	<description><pre><code>

	BYTE gameSessionId[GSID_LEN] = {0};
	BYTE webSessionID[WSID_LEN] = {0};
	INT64 MacAddr = 0;
	
	BOOL bResult = GetGameSession(gameSessionId, sizeof(gameSessionId));
	if (bResult == 0)
	{
		int result = GetWebSessionID(gameSessionId, webSessionID, &MacAddr);

		if (result == 0)
			//GetWebSessionID Success
		else 
			//GetWebSessionID Fail
	}

	</code></pre></description>
	</item>
	</list>
*/
GSMBRIDGE_API int GetWebSessionID(unsigned char* gusID, unsigned char* webSessionID,  __int64 *macAddr);

/*!
	<summary>
	API호출 결과에 대한 에러코드의 메시지를 출력하는 메소드 입니다.<br>
	클라이언트로 에러에 해당하는 메시지를 출력하고자 할 때, 에러코드에 해당되는 메시지를 받아와 출력하면 됩니다.<br> 
	</summary>
	
	<param name="errCode">[in]API호출 에러 결과 코드 입니다.</param>
	<param name="errMsgSize">[in]에러 메시지 Buffer의 size(wchar_t기준) 입니다.</param>
	<param name="errMessage">[out]에러 메시지 Buffer(최소 250 Length 확보) 입니다.</param>
	<param name="errMsgLength">[out]출력 에러 메시지의 길이(NULL포함) 입니다.</param>
	
	<returns>
	<list>
		<item><c>true</c>성공</item>
		<item><c>false</c>실패</item>
	</list>

	<list type="table">
	<listheader>
	<description>Sample Code</description>
	</listheader>
	<item>
	<description><pre><code>

	WCHAR errBuff[ERR_BUFF_LEN] = {0};
	int msgLen = 0;
	
	int result = GetUserInfo(gameSessionId, NickName, Gender, City, UserGrade);
	GetErrorMsg(result, sizeof(errBuff), errBuff, msgLen);
	MessageBox(hWnd, errbuff, L"오류", MB_OK);

	</code></pre></description>
	</item>
	</list>
*/
GSMBRIDGE_API bool GetErrorMsg(int errCode,int errMsgSize, wchar_t* errMessage, int &errMsgLength);

/*!
	<summary>
	통합제제 코드에 대한 메시지를 출력하는 메소드 입니다.<br>
	클라이언트로 코드에 해당하는 메시지를 출력하고자 할 때, 통합제제코드에 해당되는 메시지를 받아와 출력하면 됩니다.<br> 
	</summary>
	
	<param name="blockCode">[in]통합제제 코드 입니다.</param>
	<param name="errMsgSize">[in]에러 메시지 Buffer의 size(wchar_t기준) 입니다.</param>
	<param name="errMessage">[out]에러 메시지 Buffer(최소 2049 Length 확보) 입니다. 각 메시지는 NULL값(2bytes)으로 구분됩니다.</param>
	<param name="errMsgLength">[out]출력 에러 메시지의 길이(NULL포함) 입니다.</param>
	
	<returns>
	<list>
		<item><c>true</c>성공</item>
		<item><c>false</c>실패</item>
	</list>

	<list type="table">
	<listheader>
	<description>Sample Code</description>
	</listheader>
	<item>
	<description><pre><code>

	WCHAR Buff[ERR_BUFF_LEN] = {0};
	int msgLen = 0;
	
	int result = LoginWithWebSID(webSessionId, ipAddress, gameSessionId, account, &UserID, &portalFlag, &gameFlag, &birthDay);
	if (result == 5)
	{
		GetPortalBlockMsg(pPortalFlag->gameBlockFlag, sizeof(Buff), Buff, msgLen);
		MessageBox(hWnd, Buff, L"오류", MB_OK);
	}

	</code></pre></description>
	</item>
	</list>
*/
GSMBRIDGE_API bool GetPortalBlockMsg(int blockCode, int errMsgSize, wchar_t* errMessage, int &errMsgLength);

/*!
	<summary>
	게임제제 코드에 대한 메시지를 출력하는 메소드 입니다.<br>
	클라이언트로 코드에 해당하는 메시지를 출력하고자 할 때, 게임제제코드에 해당되는 메시지를 받아와 출력하면 됩니다.<br> 
	</summary>
	
	<param name="innerblockCode">[in]게임 내 제제 코드 입니다.</param>
	<param name="outerblockCode">[in]게임 외 제제 코드 입니다.</param>
	<param name="errMsgSize">[in]에러 메시지 Buffer의 size(wchar_t기준) 입니다.</param>
	<param name="errMessage">[out]에러 메시지 Buffer(최소 2049 Length 확보) 입니다. 각 메시지는 NULL값(2bytes)으로 구분됩니다.</param>
	<param name="errMsgLength">[out]출력 에러 메시지의 길이(NULL포함) 입니다.</param>
	
	<returns>
	<list>
		<item><c>true</c>성공</item>
		<item><c>false</c>실패</item>
	</list>

	<list type="table">
	<listheader>
	<description>Sample Code</description>
	</listheader>
	<item>
	<description><pre><code>

	WCHAR Buff[ERR_BUFF_LEN] = {0};
	int msgLen = 0;
	
	int result = LoginWithWebSID(webSessionId, ipAddress, gameSessionId, account, &UserID, &portalFlag, &gameFlag, &birthDay);
	if (result == 6)
	{
		GetGameBlockMsg(pGameFlag->innerBlockFlag, 0, sizeof(Buff), Buff, msgLen);
		MessageBox(hWnd, Buff, L"오류", MB_OK);
	}
	else if (result == 7)
	{
		status = GetGameBlockMsg(0, pGameFlag->outerBlockFlag, sizeof(Buff), Buff, msgLen);
		MessageBox(hWnd, Buff, L"오류", MB_OK);
	}

	</code></pre></description>
	</item>
	</list>
*/
GSMBRIDGE_API bool GetGameBlockMsg(int innerblockCode, int outerblockCode, int errMsgSize, wchar_t* errMessage, int &errMsgLength);

/*!
	<summary>
	한 사용자의 지정된 Point의 잔여량을 조회합니다.<br>
	Point를 관리하는 하나의 Commerce 서버에 속한 복수의 Point를 한 번에 조회할 수 있지만, 복수의 Commerce 서버가 관리하는 point는 조회할 수 없습니다.<br>
	<br>
	<b><i>remark</i></b><br>
	조회시에는 각 point에 대한 action 정보가 필요 없기 때문에, ServerPointsInfo의 mpPoints member를 구성하는 PointInfo상속 객체에는 PointInfo를 그대로 사용하면 됩니다.<br>
	</summary>
	<param name="PointServer">[in]질의하고자 하는 Commerce 서버의 ID입니다.</param>
	<param name="pPointsInfo">[in, out]질의하고자 하는 Point에 대한 정보를 총괄하는 ServerPointsInfo객체의 Pointer입니다.</param>
	<param name="sizeofPointsInfo">[in]ServerPointsInfo객체의 전체 사이즈입니다.</param>
	<returns>
	<list>
		<item>0 성공</item>
		<item>not 0 실패, 해당 return code에 대한 정확한 error message는 GetErrorMsg 함수를 사용하여 얻을 수 있습니다.</item>
	</list>
	</returns>
	<list type="table">
	<listheader>
	<description>Sample Code</description>
	</listheader>
	<item>
	<description><pre><code>
	//질의하고자 하는 대상이 되는 point 관리 서버의 ID : 1
	//ID 1로 설정된 point관리 서버가 관리하는 세 가지 종류의 Point의 각각의 ID : 1, 7, 13
	//사용자의 UID: 231
	//사용 게임 서비스의 서비스 코드 : 4

	PointInfo* pPointsToGet = new PointInfo[3];
	pPointsToGet[0].mPointID = 1;
	pPointsToGet[0].mPointAmount = 0;
	pPointsToGet[1].mPointID = 7;
	pPointsToGet[1].mPointAmount = 0;
	pPointsToGet[2].mPointID = 13;
	pPointsToGet[2].mPointAmount = 0;

	ServerPointsInfo* pServerPoints = new ServerPointsInfo;
	pServerPoints.mAccountID = (__int64)231;
	pServerPoints.mGameID = 4;
	pServerPoints.mPointCount = 3;
	pServerPoints.mpPoints = pPointsToGet;
	pServerPoints.mSizeOfPointInfo = sizeof(PointInfo);

	int getResult = GetPoint ( (short)1, pServerPoints, sizeof(ServerPointsInfo) );
	if ( getResult == 0 )	//	Success
	{
		pUser->mpPointsInfo->SetPointAmount ( 1, pServerPoints->mpPoints[0].mPointAmount );
		pUser->mpPointsInfo->SetPointAmount ( 7, pServerPoints->mpPoints[1].mPointAmount );
		pUser->mpPointsInfo->SetPointAmount ( 13, pServerPoints->mpPoints[2].mPointAmount );
	}
	else
	{
		wchar_t errMsg[256];
		int errMsgLen = 0;
		bool bGetErrMsg = GetErrorMsg ( getResult, 256, errMsg, &errMsgLen );
	}

	delete []pPointsToGet;
	delete pServerPoints;
	</code></pre></description>
	</item>
	</list>
	<seealso cref="AddPoint"/>
	<seealso cref="SubPoint"/>
*/
GSMBRIDGE_API int GetPoint ( short PointServer, ServerPointsInfo* pPointsInfo, size_t sizeofPointsInfo );

/*!
	<summary>
	한 사용자의 한 개 혹은 여러 개의 point에 일정 양을 추가합니다.<br>
	Point를 관리하는 하나의 commerce 서버에 속한 복수의 Point를 한 번에 증가시킬 수 있지만, 복수의 서버가 관리하는 point는 적용되지 않습니다.<br>
	<br><b><i>remark</i></b><br>
	AddPoint와 SubPoint에서는 GetPoint와 달리, 사용하고자 하는 Point에 사용에 대한 추가 정보를 추가할 수 있습니다.<br>
	point서버들과의 표준 protocol에 의거해 pPointsInfo parameter의 mpPoints를 구성하는 객체는 PointUsageInfo를 사용하는 것을 표준으로 합니다.<br>
	만약 이 mpPoints array를 PointInfo객체로 구성하는 경우에는 DLL이 자동으로 모든 Point종류에 대해서 mDailyLimit와 mPointActionID를 0으로 처리합니다.
	</summary>
	<param name="PointServer">[in]추가하고자 하는 point(들)의 관리 서버의 ID입니다.</param>
	<param name="pPointsInfo">[in, out]추가하고자 하는 Point에 대한 정보를 총괄하는 ServerPointsInfo객체의 Pointer입니다.</param>
	<param name="sizeofPointsInfo">[in]ServerPointsInfo객체의 전체 사이즈입니다.</param>
	<returns>
	<list>
		<item>0 성공</item>
		<item>not 0 실패, 해당 return code에 대한 정확한 error message는 GetErrorMsg 함수를 사용하여 얻을 수 있습니다.</item>
	</list>
	</returns>
	<list type="table">
	<listheader>
	<description>Sample Code</description>
	</listheader>
	<item>
	<description><pre><code>
	//질의하고자 하는 대상이 되는 point 관리 서버의 ID : 1
	//ID 1로 설정된 point관리 서버가 관리하는 세 가지 종류의 Point의 각각의 ID : 1, 7, 13
	//사용자의 UID: 231
	//사용 게임 서비스의 서비스 코드 : 4
	//7번 Point에 100을, 13번 Point에 500을 추가하는 예제입니다. 일일 제한과 ActionCode를 사용하지 않는 경우입니다.

	PointInfo* pPointsToAdd = new PointInfo[2];
	pPointsToAdd[0].mPointID = 7;
	pPointsToAdd[0].mPointAmount = 100;
	pPointsToAdd[1].mPointID = 13;
	pPointsToAdd[1].mPointAmount = 500;

	ServerPointsInfo* pServerPoints = new ServerPointsInfo;
	pServerPoints.mAccountID = (__int64)231;
	pServerPoints.mGameID = 4;
	pServerPoints.mPointCount = 2;
	pServerPoints.mpPoints = pPointsToAdd;
	pServerPoints.mSizeOfPointInfo = sizeof(PointInfo);	//	일일 제한과 ActionCode는 모두 0으로 내부 처리됩니다.

	int addResult = AddPoint ( (short)1, pServerPoints, sizeof(ServerPointsInfo) );
	if ( addResult == 0 )
	{
		pUser->mpPointsInfo->SetPointAmount ( 7, pServerPoints->mpPoints[0].mPointAmount );
		pUser->mpPointsInfo->SetPointAmount ( 13, pServerPoints->mpPoints[1].mPointAmount );
	}
	else
	{
		wchar_t errMsg[256];
		int errMsgLen = 0;
		bool bGetErrMsg = GetErrorMsg ( addResult, 256, errMsg, &errMsgLen );
	}

	delete []pPointsToAdd;
	delete pServerPoints;

	</code></pre></description>
	</item>
	</list>
	<seealso cref="GetPoint"/>
	<seealso cref="SubPoint"/>
*/
GSMBRIDGE_API int AddPoint ( short PointServer, ServerPointsInfo* pPointsInfo, size_t sizeofPointsInfo );

/*!
	<summary>
	한 사용자의 한 개 혹은 여러 개의 point에 대해서 지정된 양을 삭감합니다.<br>
	Point를 관리하는 하나의 commerce 서버에 속한 복수의 Point를 한 번에 삭감시킬 수 있지만, 복수의 서버가 관리하는 point는 적용되지 않습니다.<br>
	<br><b><i>remark</i></b><br>
	AddPoint와 SubPoint에서는 GetPoint와 달리, 사용하고자 하는 Point에 사용에 대한 추가 정보를 추가할 수 있습니다.<br>
	point서버들과의 표준 protocol에 의거해 pPointsInfo parameter의 mpPoints를 구성하는 객체는 PointUsageInfo를 사용하는 것을 표준으로 합니다.<br>
	만약 이 mpPoints array를 PointInfo객체로 구성하는 경우에는 DLL이 자동으로 모든 Point종류에 대해서 mDailyLimit와 mPointActionID를 0으로 처리합니다.
	</summary>
	<param name="PointServer">[in]적용하고자 하는 point(들)의 관리 서버의 ID입니다.</param>
	<param name="pPointsInfo">[in, out]삭감하고자 하는 Point에 대한 정보를 총괄하는 ServerPointsInfo객체의 Pointer입니다.</param>
	<param name="sizeofPointsInfo">[in]ServerPointsInfo객체의 전체 사이즈입니다.</param>
	<returns>
	<list>
		<item>0 성공</item>
		<item>not 0 실패, 해당 return code에 대한 정확한 error message는 GetErrorMsg 함수를 사용하여 얻을 수 있습니다.</item>
	</list>
	</returns>

	<list type="table">
	<listheader>
	<description>Sample Code</description>
	</listheader>
	<item>
	<description><pre><code>
	//질의하고자 하는 대상이 되는 point 관리 서버의 ID : 1
	//ID 1로 설정된 point관리 서버가 관리하는 세 가지 종류의 Point의 각각의 ID : 1, 7, 13
	//사용자의 UID: 231
	//사용 게임 서비스의 서비스 코드 : 4
	//1번 Point를 100만큼, 13번 point를 200만큼 삭감합니다.
	//1번 point의 일일 변동 제한은 1,000이며 13번 point의 일일 변동 제한은 2,000입니다.
	//ActionCode는 둘 다 3번이라 가정합니다.

	PointUsageInfo* pPointsToSub = new PointUsageInfo[2];
	pPointsToSub[0].mPointID = 1;
	pPointsToSub[0].mPointAmount = 100;
	pPointsToSub[0].mDailyLimit = 1000;
	pPointsToSub[0].mPointActionID = 3;
	pPointsToSub[1].mPointID = 13;
	pPointsToSub[1].mPointAmount = 200;
	pPointsToSub[1].mDailyLimit = 2000;
	pPointsToSub[1].mPointActionID = 3;

	ServerPointsInfo* pServerPoints = new ServerPointsInfo;
	pServerPoints.mAccountID = (__int64)231;
	pServerPoints.mGameID = 4;
	pServerPoints.mPointCount = 2;
	pServerPoints.mpPoints = (PointInfo*)pPointsToSub;
	pServerPoints.mSizeOfPointInfo = sizeof(PointUsageInfo);	

	int subResult = SubPoint ( (short)1, pServerPoints, sizeof(ServerPointsInfo) );
	if ( subResult == 0 )
	{
		pUser->mpPointsInfo->SetPointAmount ( 1, pServerPoints->mpPoints[0].mPointAmount );
		pUser->mpPointsInfo->SetPointAmount ( 13, pServerPoints->mpPoints[1].mPointAmount );
	}
	else
	{
		wchar_t errMsg[256];
		int errMsgLen = 0;
		bool bGetErrMsg = GetErrorMsg ( subResult, 256, errMsg, &errMsgLen );
	}

	delete []pPointsToSub;
	delete pServerPoints;
	</code></pre></description>
	</item>
	</list>
	<seealso cref="GetPoint"/>
	<seealso cref="AddPoint"/>
*/
GSMBRIDGE_API int SubPoint ( short PointServer, ServerPointsInfo* pPointsInfo, size_t sizeofPointsInfo );

#ifdef __cplusplus
}
#endif
