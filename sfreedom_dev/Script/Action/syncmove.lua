-- Action Key

function Act_Sync_Move_OnEnter(actor, action)
	
	actor:FreeMove(true);	
	actor:StopJump();
	actor:SetMovingDelta(Point3(0,0,0));
	actor:SetAnimSpeedInPeriod(3,-1);
	actor:SetNoWalkingTarget(false);
	
	action:SetParamInt(3,0);
			
	return true
end

function Act_Sync_Move_OnUpdate(actor, accumTime, frameTime)

    local movingSpeed = actor:GetAbil(AT_C_MOVESPEED) * 3;
    local action = actor:GetAction();
    
    if movingSpeed == 0 then
        movingSpeed = 300
    end
    
    if action:GetParamInt(3) == 0 then
    
        local   kLookPos = Point3(action:GetParamFloat(0),
        action:GetParamFloat(1),
        action:GetParamFloat(2));
        
        local   kCurPos = actor:GetPos();
        
        ODS("Act_Sync_Move_OnUpdate Actor:"..actor:GetID().." GUID: "..actor:GetPilotGuid():GetString().." CurPos : "..kCurPos:GetX()..","..kCurPos:GetY()..","..kCurPos:GetZ().." kLookPos:"..kLookPos:GetX()..","..
        kLookPos:GetY()..","..kLookPos:GetZ().."\n");
        --SetBreak();
        
        actor:LookAt(kLookPos,true,true);
        
        action:SetParamInt(3,1);
    end
    
    return actor:UpdateSyncMove(movingSpeed,frameTime);
end

function Act_Sync_Move_OnCleanUp(actor, action)

    actor:FreeMove(false);	
    actor:SetAnimSpeedInPeriod(1,0);

end

function Act_Sync_Move_OnLeave(actor, action)

    
	return true
end
