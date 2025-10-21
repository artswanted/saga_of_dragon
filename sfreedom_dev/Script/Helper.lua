function IsCanAutoAddHelper(iGroundNo, kPlayer)
	if kPlayer:IsNil() then return false end
	if( CheckNil(g_world == nil) ) then return false end
	if( CheckNil(g_world:IsNil()) ) then return false end
	if g_world:GetID() == "w_char_sel" then
		return false
	end
	
	if kPlayer:GetAbil(AT_LEVEL) > 14 then
		return false
	end
	
	if IsIngQuest(910) or IsEndedQuest(910) then
		return false
	end
	
	if iGroundNo == 9910100 then return false end
	if iGroundNo == 9910200 then return false end
	if iGroundNo == 9910300 then return false end
	if iGroundNo == 9910400 then return false end
	return true
end

function CreateHelper()
	local	kNameTTID = 505485--kCmdObj:GetAttr("NameTTID");
	local	iClassNo = 1028159 --1028159 --kCmdObj:GetAttrInt("ClassNo")
	
	local kPlayerPilot = g_pilotMan:GetPlayerPilot()
	if kPlayerPilot:IsNil() then
		return
	end
	local kPlayerActor = kPlayerPilot:GetActor()
	if kPlayerActor:IsNil() then
		return
	end
	
	local iBaseClass = kPlayerActor:GetAbil(AT_BASE_CLASS)
	if iBaseClass == CT_SHAMAN or
		iBaseClass == CT_DOUBLE_FIGHTER then
		return		--신종족은 포로링 없다.
	end
	
	local kPos = kPlayerActor:GetPos() --kCmdObj:GetAttrPoint3("SpawnPos");
	local kName = GetTT(kNameTTID)
	
	--	같은 이름의 액터가 이미 있는지 체크하자.
	if not GetHelperGuid():IsNil() then
		local kFindPilot = g_pilotMan:FindPilot( GetHelperGuid() )
		if not kFindPilot:IsNil() then
			return
		end
	end
	
	local kGuid = GUID("123")
	kGuid:Generate()
	
	RegistHelperGuid( kGuid )
	
	local pilot = g_pilotMan:NewPilot(kGuid, iClassNo, 0)
	if pilot:IsNil() then
		RemoveHelperGuid()
		return
	end
	if IsSingleMode() == false then
		g_pilotMan:InsertPilot(kGuid,pilot);
	end
	
	local kActor = pilot:GetActor()
	if kActor:IsNil() then
		RemoveHelperGuid()
		return
	end
	
	if not g_world:AddActor(kGuid, kActor, kPos, 1) then
		RemoveHelperGuid()
		return
	end
	
	pilot:SetName( kName )
	
	local action = kActor:ReserveTransitAction("a_trace_idle")
	if nil ~= action and not action:IsNil() then
		action:SetSlot(6)
	end
	kActor:ClearActionState()
	pilot:SetAbil(AT_HP, 1000000)
	pilot:SetAbil(AT_UNIT_SIZE,2)
	pilot:SetAbil(AT_DAMAGEACTION_TYPE,0)
	pilot:SetAbil(AT_NOT_SEE_PLAYER, 0)
	pilot:SetAbil(AT_DAMAGEACTION_TYPE, 0)
	
	kActor:SetHideShadow(false)
end

function RemoveHelper()
	local kFindPilot = g_pilotMan:FindPilot( GetHelperGuid() )
	if kFindPilot:IsNil() then
		return
	end
	if( CheckNil(g_world == nil) ) then return false end
	if( CheckNil(g_world:IsNil()) ) then return false end
	g_world:RemoveActorOnNextUpdate( GetHelperGuid() )
	
	RemoveHelperGuid()
end

--[[
function TestHelper()
	RemoveHelper()
	DoFile("helper.lua")
	DoFile("action/traceidle.lua")
	CreateHelper()
end
]]

function TalkHelper(iTalkID)
	local kFindPilot = g_pilotMan:FindPilot( GetHelperGuid() )
	if kFindPilot:IsNil() then
		return
	end
	local kActor = kFindPilot:GetActor()
	local kAction = kActor:GetAction()
	kAction:SetSlot(5)
	kActor:PlayCurrentSlot(true)
	-- 포로링 말풍선 지속 시간(단위:밀리초)
	if(GetLocale() == LOCALE.NC_USA) then	--AMERICA
		kActor:Talk(iTalkID, 5000)
	else
		kActor:Talk(iTalkID, 15000)
	end
end