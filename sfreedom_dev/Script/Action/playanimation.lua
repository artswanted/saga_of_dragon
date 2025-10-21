-- Action Key
function Act_PlayAnimation_OnEnter(actor, action)

	action:SetDoNotBroadCast(true);
	action:SetDoNotPlayAnimation(true);
	action:SetParamInt(0,0);
	
	return true
end

function Act_PlayAnimation_OnUpdate(actor, accumTime, frameTime)

	local	action = actor:GetAction();

	if action:GetParamInt(0) == 0 then
	
		action:SetParamInt(0,1);
		actor:SetTargetAnimation(action:GetParam(1));
	
		return	true;
	end
	
	if actor:IsAnimationDone() and action:GetParam(3) ~= "end" then
		
		if action:GetParamInt(2) == 0 then
			return	false
		end
		
		action:SetParam(3,"end");
	end

	return true
end

function Act_PlayAnimation_OnCleanUp(actor, action)
	return true
end

function Act_PlayAnimation_OnLeave(actor, action)
	return true
end
