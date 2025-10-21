---- lua_tinker에서 에러 메세지를 출력할 때 사용하는 함수
function _ALERT(msg)
	MessageBox(msg, "lua error")
end

-- Checker
function LoadingChecker()
	return false
end

--Login Checker
function LoginChecker()
	return g_bLogined_User
end

g_bLogined_User = false	--don't open inventory for not login user <070610 Naturallaw>
g_bEnableShortCutKey = true --Can activate short cut Key

--Can Short Cut Key ?
function EnableShortCutKey()
	if not g_bLogined_User then
		return false--Can't
	end
	if not g_bEnableShortCutKey then
		return false--Can't
	end
	return true--Can Short Cut Key
end

---- Include 
DoFile("constant.lua")
DoFile("SystemDefinition.lua")

DoFile("actor.lua")
DoFile("Action/idle.lua")
DoFile("Action/PvP.lua")
DoFile("Action/Emotion.lua")
DoFile("Actioneffect/die.lua")
DoFile("Action/intro.lua")
DoFile("Action/moving_tree.lua")
DoFile("Action/lock_move.lua")	-- 지울거임......

-- Network packet handler
DoFile("Net/net_basic.lua")
DoFile("Net/login.lua")

-- UI
DoFile("UI/loading.lua")
DoFile("UI/logo.lua")
DoFile("UI/ui.lua")
DoFile("UI/inv.lua")
DoFile("UI/BookUI.lua")
DoFile("UI/MiningTimer.lua")

--Helper
DoFile("Helper.lua")

UI_ColorSet(Config_GetValue(HEADKEY_OPTION, SUBKEY_THEME_COLOR_SET))

-- Checker
function LoadingChecker()
	return true
end

-- 전역 변수 설정
g_mapNo = 0
g_renderMan = RenderMan()
g_pilotMan = GetPilotMan()
g_particleMan = GetParticleMan()
g_cameraTrn = nil 
g_cameraRot = nil 
SetSingleMode(false);
g_bAddedUIScene = false
g_bInitScript = false
g_reloadSelf = false
g_MovieMgr = GetMovieMgr()

-- 시작 씬을 로딩한다.
g_renderMan:AddScene("s_login")

SetEreaseBackGroundMessage(false);

function SelfReload()
	g_reloadSelf = true 
	DoFile("ReloadScript_F9.lua")
	g_reloadSelf = false
end

DoFileInitLua() -- 채널 입장시 실행 되던것을 옮겨옴