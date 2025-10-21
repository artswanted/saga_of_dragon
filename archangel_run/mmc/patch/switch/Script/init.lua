function _ALERT(msg)
	InfoLog(1, "lua error : " .. msg)
end

--Init Script Confirm
g_ConfirmInitScript = 0
function ConfirmInitScript()
	return g_ConfirmInitScript
end

g_reloadSelf = false
function SelfReload()
	g_reloadSelf = true
	DoFile("Script/ReloadScript_F9.lua")
	g_reloadSelf = false
end

-- include core scripts
InfoLog(5, "...........LUA Loading..............")
-- Load any files
InfoLog(5, ".............Complete...............")
g_ConfirmInitScript = 1 --Complete
DoFile("./Script/ProtoFilter.lua")
