LavPuppets = {}

function Init_020200(world)
	guid=GUID("1")
	guid:Generate()
	LavPuppets[0] =  world:AddPuppet(guid, "p_front", Point3(0, 0, 0), Quaternion(0, Point3(0, 0, 1)))
	
	guid:Generate()
	LavPuppets[1] =  world:AddPuppet(guid, "p_left", Point3(0, 0, 0), Quaternion(0, Point3(0, 0, 1)))

	guid:Generate()
	LavPuppets[2] =  world:AddPuppet(guid, "p_right", Point3(0, 0, 0), Quaternion(0, Point3(0, 0, 1)))
	
	--CallUI("LavalonStart")

	ODS("____________________Add Puppets!!!!!!!__________________\n")


end

function pt()
	LavPuppets[0]:TransitAction("dmg")
	LavPuppets[1]:TransitAction("dmg")
	LavPuppets[2]:TransitAction("dmg")
end

function ptt()
--	LavPuppets[0]:ReloadNif()
--	LavPuppets[1]:ReloadNif()
--	LavPuppets[2]:ReloadNif()
	if( CheckNil(g_world == nil) ) then return false end
	if( CheckNil(g_world:IsNil()) ) then return false end

	guid = GUID("1")
	guid:Generate()
	LavPuppets[0] = g_world:AddPuppet(guid, "p_front", Point3(0, 0, 0), Quaternion(0, Point3(0, 0, 1)))
	guid:Generate()
	LavPuppets[1] = g_world:AddPuppet(guid, "p_left", Point3(0, 0, 0), Quaternion(0, Point3(0, 0, 1)))
	guid:Generate()
	LavPuppets[2] = g_world:AddPuppet(guid, "p_right", Point3(0, 0, 0), Quaternion(0, Point3(0, 0, 1)))
end

function pttt()
	if( CheckNil(g_world == nil) ) then return false end
	if( CheckNil(g_world:IsNil()) ) then return false end
	g_world:RemoveObject(LavPuppets[0]:GetGuid())
	g_world:RemoveObject(LavPuppets[1]:GetGuid())
	g_world:RemoveObject(LavPuppets[2]:GetGuid())
end

function Start_020200(world)
	ClearESCScript()
	OnRecoveryMiniQuest(false)
end

--[[
function Ready_020200(world)
	-- ESC Script 등록
	RegistESCScript("OnEsc_LavalonOpening")
end

function Update_020200()
	return true
end

function Start_020200(world)
	ClearESCScript()
	OnRecoveryMiniQuest(false)
end

function OnEsc_LavalonOpening()
	local kPacket = NewPacket(13201) -- PT_C_M_REQ_INDUN_START
	Net_Send(kPacket)
	DeletePacket(kPacket)
end]]
