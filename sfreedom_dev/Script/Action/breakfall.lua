function Act_BreakFall_OnCheckCanEnter(actor,action)
	
	if( CheckNil(actor == nil) ) then return false end
	if( CheckNil(actor:IsNil()) ) then return false end
	
	if actor:IsMeetFloor() == false then
		return	false
	end
	
	return	true;

end
function Act_BreakFall_OnEnter(actor, action)
		
	if( CheckNil(actor == nil) ) then return false end
	if( CheckNil(actor:IsNil()) ) then return false end
	
	if( CheckNil(action == nil) ) then return false end
	if( CheckNil(action:IsNil()) ) then return false end
	
	actor:ResetAnimation();
	actor:SetCanHit(false);
	
	local	byDir = DIR_NONE;
	
	if actor:IsMyActor() then
		--	누르고 있는 키에 따라 구를 방향을 바꾼다.
		local	bKeyUp = KeyIsDown(KEY_UP);
		local	bKeyDown = KeyIsDown(KEY_DOWN);
		local	bKeyLeft = KeyIsDown(KEY_LEFT);
		local	bKeyRight = KeyIsDown(KEY_RIGHT);
		
		if 	bKeyUp then 
			byDir = DIR_UP
		elseif bKeyDown then
			byDir = DIR_DOWN
		elseif bKeyLeft then
			byDir = DIR_LEFT
		elseif bKeyRight then
			byDir = DIR_RIGHT
		end

	else 
	
		local	packet = action:GetParamAsPacket()
		if packet~=nil and packet:IsNil() == false then
		
			byDir = packet:PopByte();
			
		end
	
	end		
	
	action:SetParamInt(2,byDir);

	local	vDepth = actor:GetPathNormal();
	local	vLeft = Point3(0,0,1);
	vLeft:Cross(vDepth);
	
	local	bIsToleft = actor:IsToLeft();
	local	iSlot = 0;
		
	if byDir == DIR_UP then
		action:SetParamAsPoint(0,vDepth);
		
		if bIsToleft then 
			iSlot = 3
		else 
			iSlot = 2
		end
		
	elseif byDir == DIR_DOWN then
		action:SetParamAsPoint(0,vDepth:_Multiply(-1));
		
		if bIsToleft then 
			iSlot = 2
		else 
			iSlot = 3
		end
				
	elseif byDir == DIR_LEFT then
		action:SetParamAsPoint(0,vLeft);
		
		if bIsToleft then 
			iSlot = 1
		else 
			iSlot = 0
		end
				
	elseif byDir == DIR_RIGHT then
		action:SetParamAsPoint(0,vLeft:_Multiply(-1));
		
		if bIsToleft then 
			iSlot = 0
		else 
			iSlot = 1
		end
				
	elseif byDir == DIR_NONE then
		iSlot = 0;
		action:SetParamAsPoint(0,actor:GetLookingDir():_Multiply(-1));
	end
	
	action:SetSlot(iSlot);
	
	return true
end

function Act_BreakFall_OnOverridePacket(actor,action,packet)
	
	if( CheckNil(actor == nil) ) then return false end
	if( CheckNil(actor:IsNil()) ) then return false end
			
	if( CheckNil(action == nil) ) then return false end
	if( CheckNil(action:IsNil()) ) then return false end
	
	packet:PushByte(action:GetParamInt(2));

end

function Act_BreakFall_OnUpdate(actor, accumTime, frameTime)
	
	if( CheckNil(actor == nil) ) then return false end
	if( CheckNil(actor:IsNil()) ) then return false end

	local actorID = actor:GetID()
	
	local action = actor:GetAction()	
	if( CheckNil(action == nil) ) then return false end
	if( CheckNil(action:IsNil()) ) then return false end
	
	local fOriginalMoveSpeed = actor:GetAbil(AT_MOVESPEED)
	local movingSpeed = actor:GetAbil(AT_C_MOVESPEED)
	
	if IsSingleMode() == true then
		movingSpeed = 120 * 0.6
	end

	local	iRoundCount = action:GetParamInt(1)		
	
	if iRoundCount == 2 then
		action:SetParam(3,"end");
		return	false
	end
	
	if action:GetParam(4) == "Move" then
		movingSpeed = movingSpeed*2;
	end

	if fOriginalMoveSpeed == 0 then
		fOriginalMoveSpeed = movingSpeed
	end
		
	local	fAnimSpeed = 0.0;
	
	if fOriginalMoveSpeed>0 then
		fAnimSpeed = movingSpeed/fOriginalMoveSpeed
	end
	
	actor:SetAnimSpeed(fAnimSpeed);

	local	kMovingDir = action:GetParamAsPoint(0);
	kMovingDir:Multiply(movingSpeed);
	actor:SetMovingDelta(kMovingDir);
	
	return true
end
function Act_BreakFall_OnCleanUp(actor, action)
	if( CheckNil(actor == nil) ) then return false end
	if( CheckNil(actor:IsNil()) ) then return false end
	actor:SetCanHit(true);
	return true;
end

function Act_BreakFall_OnLeave(actor, action)
	
	if( CheckNil(actor == nil) ) then return true end
	if( CheckNil(actor:IsNil()) ) then return true end
	
	local kCurAction = actor:GetAction();
	if( CheckNil(kCurAction == nil) ) then return true end
	if( CheckNil(kCurAction:IsNil()) ) then return true end
	
	if kCurAction:GetParam(3) ~= "end" then
		return	false
	end
	return true
end

function Act_BreakFall_OnEvent(actor,textKey)
	
	if( CheckNil(actor == nil) ) then return false end
	if( CheckNil(actor:IsNil()) ) then return false end
	
	local kCurAction = actor:GetAction();
	if( CheckNil(kCurAction == nil) ) then return false end
	if( CheckNil(kCurAction:IsNil()) ) then return false end
	
	if textKey == "Move" then
		kCurAction:SetParam(4,"Move")
	end
	
	if textKey == "stand" then
	
		local	iRoundCount = kCurAction:GetParamInt(1)
		iRoundCount = iRoundCount+1
		kCurAction:SetParamInt(1,iRoundCount);
		
		kCurAction:SetParam(4,"")
	
	end

end

