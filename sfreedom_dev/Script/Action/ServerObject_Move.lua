function Act_ServerObject_Move_OnEnter(actor, action)

	actor:FreeMove(true);	
	actor:StopJump();
	actor:SetNoWalkingTarget(false);
	return true
end


function Act_ServerObject_Move_OnUpdate(actor, accumTime, frameTime)
	local	kPilot = actor:GetPilot();
    local	kRidingObject = kPilot:GetObject();

    local	kPosition = kRidingObject:GetServerObjectPosition(frameTime);
	actor:SetTranslate(kPosition);

	return	true;

end

function Act_ServerObject_Move_OnCleanUp(actor, action)
end

function Act_ServerObject_Move_OnLeave(actor, action)
	return	false;
end

