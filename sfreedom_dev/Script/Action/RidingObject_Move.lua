function Act_RidingObject_Move_OnEnter(actor, action)

	actor:FreeMove(true);	
	actor:StopJump();
	actor:SetNoWalkingTarget(false);

	return true
end


function Act_RidingObject_Move_OnUpdate(actor, accumTime, frameTime)

	local	kPilot = actor:GetPilot();
	local	kRidingObject = kPilot:GetObject();
	
	local	fElapsedTime = accumTime;
	if IsSingleMode() == false then
		if( CheckNil(g_world == nil) ) then return false end
		if( CheckNil(g_world:IsNil()) ) then return false end
		fElapsedTime = g_world:GetServerElapsedTime()
	end
	
	local	kPosition = kRidingObject:GetRidingObjectPosition(fElapsedTime);
	actor:SetTranslate(kPosition);
	
	return	true;

end

function Act_RidingObject_Move_OnCleanUp(actor, action)
end

function Act_RidingObject_Move_OnLeave(actor, action)
	return	false;
end

