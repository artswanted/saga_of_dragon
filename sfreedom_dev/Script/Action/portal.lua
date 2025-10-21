-- Portal 

function Act_Portal_OnEnter(actor, action)
	ODS("Param0 : "..action:GetParam(0).."\n")
	ODS("Param1 : "..action:GetParam(1).."\n")
	ODS("Param2 : "..action:GetParam(2).."\n")
	
	if actor:IsRidingPet() then
		action:SetSlot(1)
		actor:PlayCurrentSlot()
		action:SetNextActionName("a_rp_idle")
	end

	
	local	packet = action:GetParamAsPacket()
	if packet~=nil and packet:IsNil() == false then

		local	iParam1 = packet:PopString()
		local	iParam2 = packet:PopSize_t()		
		action:SetParam(1,""..iParam1)
		action:SetParam(2,""..iParam2)
		
	end
	
	action:SetParam(0,"null");	
	actor:Stop()
	
	if actor:IsRidingPet() then
		local kActorPet = actor:GetMountTargetPet()
		if kActorPet:IsNil() == false then
			kActorPet:AttachParticle(289, "char_root", "e_portal");
		end
	else
		actor:AttachParticle(289, "char_root", "e_portal");
	end

	if actor:IsMyActor() == true then
		actor:AttachSound(2000,"Cha_Portal");
	end	
	
	if actor:IsMyActor() then
		LockPlayerInput(21)
	end

	return true
end
function Act_Portal_OnOverridePacket(actor,action,packet)

	packet:PushString(action:GetParam(1))
	packet:PushSize_t(tonumber(action:GetParam(2)))

end

function Act_Portal_OnUpdate(actor, accumTime, frameTime)
	action = actor:GetAction()
	param = action:GetParam(0)
	
	if param == "null" then
		action:SetParam(0, accumTime)
	elseif param~= "done" and accumTime - param > 1.0 then
		action:SetParam(0, "done")
		return false
	end
	
	return true 
end
function Act_Portal_OnCleanUp(actor, action)
	actor:RestoreLockBidirection()
	if actor:IsMyActor() then
		UnLockPlayerInput(21)
	end
end

function Act_Portal_OnLeave(actor, action)
	prevAction = actor:GetAction()
	
	
	ODS("Act_Portal_OnLeave prevAction:"..prevAction:GetID().." action:"..action:GetID().." prevAction:GetParam(0):"..prevAction:GetParam(0).."\n");

	if action:GetID() ~= "a_idle" and action:GetID() ~= "a_step_the_beat_idle" and action:GetID() ~= "a_rp_idle" then
		return	false
	end
	
	if prevAction:GetParam(0) ~= "done" then
		return false
	else
		if actor:IsMyActor() == true then
		
			ODS("Act_Portal_OnLeave Param1:"..prevAction:GetParam(1).." Param2:"..prevAction:GetParam(2).."\n");
		
			local packet = NewPacket(5001)	--PT_C_M_REQ_TRIGGER_ACTION
			packet:PushString(prevAction:GetParam(1))
			packet:PushSize_t(tonumber(prevAction:GetParam(2)))
			Net_Send(packet)
			DeletePacket(packet)
		end
		return true
	end	
end
