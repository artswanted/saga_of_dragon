function Act_Teleport01_OnEnter(actor, action)
	local	packet = action:GetParamAsPacket()
	if packet~=nil and packet:IsNil() == false then

		local	iParam1 = packet:PopByte()
		local	iParam2 = packet:PopByte()		
		local	iParam3 = packet:PopString()
		local	iParam4 = packet:PopFloat()
		action:SetParam(1,""..iParam1)
		action:SetParam(2,""..iParam2)
		action:SetParam(3,""..iParam3)
		action:SetParam(4,""..iParam4)
	end

	action:SetParam(0,"null");	
	actor:Stop()
	
	if action:GetParam(3) == "Chaos" then
		action:SetSlot(0)
		actor:AttachParticle(289, "char_root", "ef_Chaosportal_01_char_root")
	else
		action:SetSlot(1)
		actor:AttachParticle(289, "char_root", "ef_hiddenportal_01_char_root")
	end
	
	if actor:IsMyActor() == true then
		HidePartUI("FRM_CALL_ORDER", true)
		UIOff()
		LockPlayerInput(23)
	end

	return true
end

function Act_Teleport01_OnOverridePacket(actor,action,packet)
	
	packet:PushByte(action:GetParam(1))
	packet:PushByte(action:GetParam(2))
	packet:PushString(action:GetParam(3))
	packet:PushFloat(action:GetParamFloat(4))

end
function Act_Teleport01_OnUpdate(actor, accumTime, frameTime)
	action = actor:GetAction()
	param = action:GetParam(0)
	local timevalue = 0
	if action:GetParam(3) == "Chaos" then
		timevalue = 4.4
	else
		timevalue = 4.0
	end

	if param == "null" then
		action:SetParam(0, accumTime)
	elseif param~= "done" and accumTime - param > timevalue then
	
		if actor:IsAnimationDone() == true and action:GetParamFloat(4) == 0.0 then
			action:SetParamFloat(4, accumTime - param)
			if actor:IsMyActor() then
				if IsMyAlive() == true then
					if( CheckNil(g_world == nil) ) then return false end
					if( CheckNil(g_world:IsNil()) ) then return false end
					g_world:SetShowWorldFocusFilterColorAlpha( 0x000000, 0.0, 1.0, 1.0, true, false )				
				end
			end
		end
		
		if actor:IsAnimationDone() == true and (accumTime - param) - action:GetParamFloat(4) > 1.0 then
			action:SetParam(0, "done")
			return false
		end				
	end
	
	return true 
end

function Act_Teleport01_OnEvent(actor, textkey)
	CheckNil(nil==actor)
	CheckNil(actor:IsNil())
	
	if textkey == "hit" then
		actor:HideNode("Scene Root",true);
	elseif textkey == "end" then
		--actor:HideNode("Scene Root",false);
	end
	
	return true
end

function Act_Teleport01_OnCleanUp(actor, action)	
	actor:RestoreLockBidirection()
	if action:GetID() ~= "a_teleport01_out" or IsMyAlive() == false then		
		if actor:IsMyActor() == true then
			g_szMapMoveCompleteNextActionName = ""			
		end
		actor:HideNode("Scene Root",false)
		actor:SetHide(false)
	else
		actor:SetHide(true)
	end

	if actor:IsMyActor() == true then	
	if( CheckNil(g_world == nil) ) then return true end
	if( CheckNil(g_world:IsNil()) ) then return true end
		g_world:SetShowWorldFocusFilterColorAlpha(0x000000, g_world:GetWorldFocusFilterAlpha(), 0, 1.0, false,true)	
		UIOn()
		HidePartUI("FRM_CALL_ORDER", false)
		UnLockPlayerInput(23)	
	end	
	if IsMyAlive() == false then
		actor:ReserveTransitAction("a_die")
		return true
	end		
	return true
end
function Act_Teleport01_OnLeave(actor, action)
	prevAction = actor:GetAction()
	
--	if action:GetID() ~= "a_idle" then
--		return	false
--	end
	g_szMapMoveCompleteNextActionName = ""
	if prevAction:GetParam(0) ~= "done" then
		return false
	else
		if actor:IsMyActor() == true then		
			if IsMyAlive() == false then
				return true
			end
			if prevAction:GetParam(3) == "Chaos" then
				g_szMapMoveCompleteNextActionName = "Chaos"
			else
				g_szMapMoveCompleteNextActionName = "Hidden"
			end				
			
			local packet = NewPacket(12422)	--PT_C_M_REQ_ITEM_ACTION
			packet:PushByte(prevAction:GetParam(1))
			packet:PushByte(prevAction:GetParam(2))
			packet:PushInt(GetServerElapsedTime32())
			Net_Send(packet)
			DeletePacket(packet)			
		end
		return true
	end	
end

-- Teleport Out
function Act_Teleport01_Out_OnEnter(actor, action)	
	local	packet = action:GetParamAsPacket()
	if packet~=nil and packet:IsNil() == false then
		local	iParam3 = packet:PopString()
		action:SetParam(3,""..iParam3)
	end
	
	if IsMyAlive() == false then
		actor:ReserveTransitAction("a_die")
		return false
	end
	
	action:SetParam(0,"null");	
	actor:Stop()	
	if action:GetParam(3) == "Chaos" then
		action:SetSlot(0)
		actor:AttachParticle(289, "char_root", "ef_Chaosportal_02_char_root")
	else
		action:SetSlot(1)
		actor:AttachParticle(289, "char_root", "ef_hiddenportal_01_char_root")	
	end			
	--action:BroadCast(actor:GetPilot())	
	if actor:IsMyActor() == true then	
		HidePartUI("FRM_CALL_ORDER", true)
		UIOff()
		actor:HideNode("Scene Root",true)
		LockPlayerInput(23)
	end	
	return true
end
function Act_Teleport01_Out_OnOverridePacket(actor,action,packet)
		
	packet:PushString(action:GetParam(3))		

end
function Act_Teleport01_Out_OnUpdate(actor, accumTime, frameTime)
	action = actor:GetAction()
	param = action:GetParam(0)
	
	local timevalue = 0
	if action:GetParam(3) == "Chaos" then
		timevalue = 3.6
	else
		timevalue = 3.2
	end
	
	if param == "null" then
		action:SetParam(0, accumTime)
	elseif param~= "done" and accumTime - param > timevalue then
		action:SetParam(0, "done")
		return false
	end
	
	return true 
end

function Act_Teleport01_Out_OnEvent(actor, textkey)
	CheckNil(nil==actor)
	CheckNil(actor:IsNil())
	
	if textkey == "hit" then
		actor:HideNode("Scene Root",false);
		actor:SetHide(false)
	elseif textkey == "end" then
		actor:HideNode("Scene Root",false);
		actor:SetHide(false)
	end
	
	return true
end

function Act_Teleport01_Out_OnCleanUp(actor, action)
	actor:RestoreLockBidirection()
	actor:HideNode("Scene Root",false);
	actor:SetHide(false)
	if actor:IsMyActor() == true then
		UIOn()
		HidePartUI("FRM_CALL_ORDER", false)
		UnLockPlayerInput(23)
	end
	if IsMyAlive() == false then
		actor:ReserveTransitAction("a_die")
		return true
	end	
	return true
end

function Act_Teleport01_Out_OnLeave(actor, action)
	prevAction = actor:GetAction()
	
--	if action:GetID() ~= "a_idle" then
--		return	false
--	end
	
	if prevAction:GetParam(0) ~= "done" then
		return false
	else
		return true
	end	
end

function IsMyAlive()
	if g_pilotMan ~= nil then
		local kMyPilot = g_pilotMan:GetPlayerPilot()
		if nil ~= kMyPilot and false == kMyPilot:IsNil() then
			if false == kMyPilot:IsAlive() then
				return false
			end
		end
	end	
	return true
end
