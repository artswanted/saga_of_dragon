function WA_SB_CRYSTAL_APPEAR_OnReceivePacket(wa_obj,Packet)
	if( CheckNil(g_world == nil) ) then return false end
	if( CheckNil(g_world:IsNil()) ) then return false end
	local guid = GUID("")
	g_world:SetBossGUID(guid)
	BossActor = nil
	guid:Generate()
	local pilot = g_pilotMan:NewPilot(guid, 1000103, 0)
	local actor = pilot:GetActor()
   if actor:IsNil() == true then
        ODS("크리스탈 액터가 nil\n")
      return false
   end
	g_world:AddActor(guid, actor, Point3(0,0,100), 6) -- NPC Create
	actor:SetPickupScript("sb_crystal_portal")
	actor:SeeFront(true)
	actor:ClearActionState()
   actor:SetTargetAnimation("appear")
   ODS("WA_SB_CRYSTAL_APPEAR_OnReceivePacket \n")
	--actor:ReserveTransitAction("a_appear")
	return true
end

function WA_SB_CRYSTAL_APPEAR_OnEnter(wa_obj,ElapsedTimeAtStart)
	return true
end

function WA_SB_CRYSTAL_APPEAR_OnUpdate(wa_obj,ElapsedTime)
  return false
end

function WA_SB_CRYSTAL_APPEAR_OnLeave(wa_obj)
	if( CheckNil(g_world == nil) ) then return true end
	if( CheckNil(g_world:IsNil()) ) then return true end
	g_world:Show_Party_Message_Box(GetTextW(19934),15000)
	return true
end
