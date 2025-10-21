-- damage

function Act_PushBack_OnEnter(actor, action)
	if( CheckNil(g_world == nil) ) then return false end
	if( CheckNil(g_world:IsNil()) ) then return false end
	actor:Stop()

	local	fAccel = 400.0;
	local	fInitSpeed = -g_fPushBackMoveSpeed;
	local	fZeroMoveTime = (-fInitSpeed)/(2*fAccel);

	action:SetParamFloat(0,g_world:GetAccumTime());
	action:SetParamFloat(1,fInitSpeed);
	action:SetParamFloat(3,fZeroMoveTime);
	action:SetParamFloat(4,fAccel);
	action:SetParamFloat(6,g_world:GetAccumTime());
	
	action:SetParamInt(5,0)
	actor:SetCanHit(false);
	
	actor:SetNoWalkingTarget(false);
	
	action:SetParamInt(2,actor:GetLastDirection());

	return true
end

function Act_PushBack_OnUpdate(actor, accumTime, frameTime)
	ODS("Act_PushBack_OnUpdate\n");
	local action = actor:GetAction()
	
	if frameTime == 0 then
		return	true
	end
	
	local	kEffectDef = GetEffectDef(action:GetActionNo());
	local	fTotalPlayTime = 1;
	if kEffectDef:IsNil() == false then
		--fTotalPlayTime = kEffectDef:GetDurationTime()/1000.0;
	end
	
	local	fZeroMoveTime = action:GetParamFloat(3);
	
	local	fDeltaTime = accumTime - action:GetParamFloat(6);
	action:SetParamFloat(6,accumTime);
	
	local	fElapsedTime = accumTime -  action:GetParamFloat(0);

	if (fElapsedTime-fDeltaTime) > fZeroMoveTime then
		fDeltaTime = 0

	elseif  fElapsedTime > fZeroMoveTime then
		fDeltaTime = fDeltaTime-(fElapsedTime - fZeroMoveTime);
		fElapsedTime = fZeroMoveTime;
	end
	
	if fDeltaTime ~= 0 then
	
		local	fAccel = action:GetParamFloat(4);
		local	fInitSpeed = action:GetParamFloat(1);
		local	fMoveDistance = fInitSpeed*fDeltaTime+fAccel*(fElapsedTime*fElapsedTime-(fElapsedTime-fDeltaTime)*(fElapsedTime-fDeltaTime));
		local	fMoveSpeed = fMoveDistance / frameTime;
		
		if fMoveSpeed>0 then
			fMoveSpeed = 0
		end

		local	kDir = action:GetParamInt(2);
		actor:Walk(kDir,fMoveSpeed,frameTime)
		
		if math.abs(fMoveSpeed) < 1 and 0==action:GetParamInt(5) then
			actor:SetSendBlowStatus(false, false, true);
			action:SetParamInt(5,1)
		end
	
	end
	
	if (accumTime - action:GetActionEnterTime()) > fTotalPlayTime then
		return	false
	end

	return true
end

function Act_PushBack_OnCleanUp(actor, action)
	actor:SetCanHit(true);
end

function Act_PushBack_OnLeave(actor, action)
	ODS("Act_PushBack_OnLeave\n");
	
	if action:GetID() ~= "a_die" then
		local kEffectDef = GetEffectDef(actor:GetAction():GetActionNo());
		local fGodTime = 1;
		if kEffectDef:IsNil() == false then
			fGodTime = kEffectDef:GetAbil(AT_GOD_TIME)/1000.0;
		end
		actor:StartGodTime(fGodTime);
	end
	
	actor:SetMovingDir(Point3(0, 0, 0))
	actor:SetSendBlowStatus(false, false, true)

	return true
end
