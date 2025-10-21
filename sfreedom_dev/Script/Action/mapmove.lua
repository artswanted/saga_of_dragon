-- Portal 

function Act_MapMove_OnEnter(actor, action)
	actor:AttachParticle(289, "char_root", "e_portal")
	actor:SetTargetAlpha(actor:GetAlpha(), 0, 1)
	return true
end

function Act_MapMove_OnUpdate(actor, accumTime, frameTime)
	local action = actor:GetAction()
	local param = action:GetParam(0)
	local param2= action:GetParam(1)
	
	if param == "null" then
		action:SetParam(0, accumTime)
		return true
	end

	if param2 ~= "done" and accumTime - param > 1.0 then
		action:SetParam(1, "done")
		return false
	end

	return true
end
function Act_MapMove_OnCleanUp(actor, action)
end

function Act_MapMove_OnLeave(actor, action)

	if( CheckNil(nil==actor) ) then return false end
	if( CheckNil(actor:IsNil()) ) then return false end
	
	if( CheckNil(nil==action) ) then return false end
	if( CheckNil(action:IsNil()) ) then return false end
		
	local prevAction = actor:GetAction()
	if prevAction:GetParam(1) ~= "done" then
		return false
	elseif actor:IsMyActor() == true then
		if( CheckNil(nil==g_world) ) then return false end
		if( CheckNil(g_world:IsNil()) ) then return false end
		--ODS("____________Net_MapMoveReally_____________\n")
		g_world:RemoveActorOnNextUpdate(actor:GetPilotGuid())
		Net_MapMoveReally(prevAction:GetParamAsPacket())
		return true
	end
end
