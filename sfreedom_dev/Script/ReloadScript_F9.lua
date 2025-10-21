
function Reload()
	if g_reloadSelf == true then
		DoFile("ReloadScript_F9.lua")
	else
		DoFile("action.lua");
		DoFile("skill.lua");
		DoFile("World/bigmaze.lua");
		DoFile("Net/net.lua");
		DoFile("Net/net_basic.lua");
		DoFile("Actor/pet_base.lua");
		DoFile("Actor/pc_base.lua");
		DoFile("Actor/mob_base.lua");
		DoFile("UI/PvPUI.lua");
	end
end
