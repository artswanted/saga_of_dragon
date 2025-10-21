
function Skill_BoobyTrap_Play_OnEnter(actor, action)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
--	local actorID = actor:GetID()
--	local actionID = action:GetID()
	
	actor:StopJump();
	actor:FreeMove(true);
	actor:SetMovingDelta(Point3(0,0,0));
	
	action:SetParam(4,"MOVING");
	
	if( CheckNil(g_world == nil) ) then return false end
	if( CheckNil(g_world:IsNil()) ) then return false end
	action:SetParamFloat(5,g_world:GetAccumTime());
	
	action:SetParamFloat(6,actor:GetPos():GetX());
	action:SetParamFloat(7,actor:GetPos():GetY());
	action:SetParamFloat(8,actor:GetPos():GetZ()+30);
	
	action:SetParamFloat(9,0.7);  --  Total Move Time
	actor:FreeMove(true)
	return true
end

function Skill_BoobyTrap_Play_OnUpdate(actor, accumTime, frameTime)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
    local   action = actor:GetAction();
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
    if  action:GetParam(4) == "MOVING" then
        
        --  원래는 목표좌표로 곡선을 그리며 날아가야하지만
        --  일단은 그냥 목표좌표로 순간이동하자.
        local   kTargetPos = Point3(action:GetParamFloat(0),action:GetParamFloat(1),action:GetParamFloat(2));
        local   kStartPos = Point3(action:GetParamFloat(6),action:GetParamFloat(7),action:GetParamFloat(8));
        
        local   fElapsedTime = accumTime - action:GetParamFloat(5);
        local   fMaxTime = action:GetParamFloat(9);
        local   fRate = fElapsedTime/fMaxTime;
        
        local   kCenterPos1 = kTargetPos:_Subtract(kStartPos);
        kCenterPos1:Multiply(0.3);
        kCenterPos1:Add(kStartPos);
        
        kCenterPos1:SetZ(kStartPos:GetZ()+50);
        
        local   kCenterPos2 = kTargetPos:_Subtract(kStartPos);
        kCenterPos2:Multiply(0.6);
        kCenterPos2:Add(kStartPos);
        kCenterPos2:SetZ(kTargetPos:GetZ()+50);
        

        if fElapsedTime>fMaxTime then
            fRate = 1
        end
        
        
        local   kNextPos = Bezier4(kStartPos,kCenterPos1,kCenterPos2,kTargetPos,fRate);
        actor:SetTranslate(kNextPos);
            
            
        if fRate == 1 then
            --  이동을 완료했을 경우
            action:SetParam(4,"ARRIVED");
        end
    
    end
    
    if action:GetParam(4) == "BLOW" then
        local   kTargetPos = Point3(action:GetParamFloat(0),action:GetParamFloat(1),action:GetParamFloat(2));
    
        actor:AttachParticleToPoint(1,kTargetPos,"ef_Magnumbreak_01_char_root");
        action:SetParamFloat(5,accumTime);
        action:SetParam(4,"BLOWING");
        
        actor:HideNode("Scene Root",true);
    
    end
    
    if action:GetParam(4) == "BLOWING" then
    
        local   fElapsedTime = accumTime - action:GetParamFloat(5);
        if fElapsedTime > 3 then
        
            return  false;
        
        end
    
    end
    
    
	return true
end


function Skill_BoobyTrap_Play_OnCleanUp(actor, action)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	if( CheckNil(g_world == nil) ) then return true end
	if( CheckNil(g_world:IsNil()) ) then return true end
	g_world:RemoveActorOnNextUpdate(actor:GetPilotGuid())
end

function Skill_BoobyTrap_Play_OnLeave(actor, action)
	return true;
end

function Skill_BoobyTrap_Play_OnCastingCompleted(actor, action)
end