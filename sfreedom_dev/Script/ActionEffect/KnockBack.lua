
function Act_KnockBack_OnEnter(actor, action)
	if( CheckNil(actor == nil) ) then return false end
	if( CheckNil(actor:IsNil()) ) then return false end
			
	if( CheckNil(action == nil) ) then return false end
	if( CheckNil(action:IsNil()) ) then return false end
	if( CheckNil(g_world == nil) ) then return false end
	if( CheckNil(g_world:IsNil()) ) then return false end
	
	if actor:IsRidingPet() then --라이딩펫: 탑승 중 피격당하면 강제 하차→피격 액션 진행
		local kActorPet = actor:GetMountTargetPet()
		if kActorPet:IsNil() == false then
			kActorPet:ReserveTransitAction("a_mount")
		end
	end

	local	actorID = actor:GetID()

	ODS("Act_KnockBack_OnEnter actor:"..actor:GetID().." action : "..action:GetID().."\n");
	
	action:SetParamFloat(1,200);	-- Initial Velocity
	
	action:SetParamInt(2,0);	-- Step
	action:SetParamFloat(10,g_world:GetAccumTime());
	action:SetParamFloat(11,0);
	action:SetParamFloat(13,g_world:GetAccumTime());
	actor:FreeMove(true);	
	actor:StopJump();
	actor:StartGodTime(0);
	actor:SetMovingDelta(Point3(0,0,0));
	actor:SetNoWalkingTarget(false);
			
	action:SetParamInt(2,1);
	action:SetSlot(1);
	
	actor:SetCanHit(false);

	return true
end

function Act_KnockBack_OnUpdate(actor, accumTime, frameTime,action)
	if( CheckNil(actor == nil) ) then return false end
	if( CheckNil(actor:IsNil()) ) then return false end
			
	if( CheckNil(action == nil) ) then return false end
	if( CheckNil(action:IsNil()) ) then return false end
	
	local	currentSlot = action:GetCurrentSlot()
	local	param = action:GetParam(0)
	
	local	iStep = action:GetParamInt(2);

	local	fTotalElapsedTime = action:GetParamFloat(11);
	action:SetParamFloat(11,fTotalElapsedTime+frameTime);
	
	local	fTotalElapsedTime2 = accumTime - action:GetParamFloat(13);
	local	kEffectDef = GetEffectDef(action:GetActionNo());
	local	fTotalPlayTime = 3;
	if kEffectDef:IsNil() == false then
		fTotalPlayTime = kEffectDef:GetDurationTime()/1000.0;
	end
	
	if fTotalElapsedTime2>fTotalPlayTime then
		return	 false;
	end
	
	local	fHorizSpeed = action:GetScriptParamAsInt("VELOCITY");
	
	local	fBlowUp_Gravity = -600;
	if iStep == 0 or iStep == 1 then
		
		local	kMovingDir = actor:GetLookingDir();
		local	fZ_Velocity_Init = action:GetParamFloat(1);
		local	fZ_Velocity = 2*fBlowUp_Gravity*fTotalElapsedTime+fZ_Velocity_Init;
		kMovingDir:SetZ(fZ_Velocity);
		kMovingDir:SetX(kMovingDir:GetX()*-1*fHorizSpeed);
		kMovingDir:SetY(kMovingDir:GetY()*-1*fHorizSpeed);
		actor:SetMovingDelta(kMovingDir);
		
	elseif iStep == 2 then
		
		local	kMovingDir = actor:GetLookingDir();
		local	fZ_Velocity_Init = action:GetParamFloat(1);
		local	fZ_Velocity = 2*fBlowUp_Gravity*fTotalElapsedTime+fZ_Velocity_Init;
		kMovingDir:SetZ(fZ_Velocity);
		kMovingDir:SetX(kMovingDir:GetX()*-1*fHorizSpeed*0.5);
		kMovingDir:SetY(kMovingDir:GetY()*-1*fHorizSpeed*0.5);
		actor:SetMovingDelta(kMovingDir);
	
	end
	if iStep == 5 and actor:IsAnimationDone() == true then
		action:SetParam(0,"end");
		actor:SetDownState(false);
		return	false;
	end 	
	if iStep == 4 then
	
		local	fElapsedDownTime = accumTime - action:GetParamFloat(3);
		if fElapsedDownTime>=fTotalPlayTime-(1.5+(action:GetParamFloat(3)-action:GetParamFloat(13))) then
			action:SetParamInt(2,5);
			actor:PlayNext();
		end
	
	end
	if iStep == 3 and actor:IsAnimationDone() == true then
	
		action:SetParamInt(2,4);
		action:SetParamFloat(3,accumTime);	--	다운 시작 시간 기록

	
	end
	
	if actor:IsMeetFloor() == true then
	
		if iStep == 1 then
		
			ODS("To Step 2\n");
		
			action:SetParamFloat(1,200);	-- Initial Velocity
			action:SetParamFloat(11,0);
			action:SetParamInt(2,2);
			action:SetParamInt(4,0);
			local pt = actor:GetTranslate()
			pt:SetZ(pt:GetZ() - 30)
			actor:AttachParticleToPoint(200, pt, "e_jump")
			actor:PlayNext();
			
		elseif iStep == 2 then
		
			if action:GetParamInt(4) == 0 then
				action:SetParamInt(4,1);
			else
		
				local pt = actor:GetTranslate()
				pt:SetZ(pt:GetZ() - 30)
				actor:AttachParticleToPoint(201, pt, "e_jump")
				action:SetParamInt(2,3);
				actor:ResetAnimation();
				action:SetSlot(2);
				actor:SetTargetAnimation(action:GetCurrentSlotAnimName());
				actor:SetSendBlowStatus(false, true);
				actor:FreeMove(false);
					
				actor:SetDownState(true);
				actor:SetCanHit(true);			
			end
		end
	
	end

	return true
end

function Act_KnockBack_OnCleanUp(actor, action)
	if( CheckNil(actor == nil) ) then return false end
	if( CheckNil(actor:IsNil()) ) then return false end
			
	if( CheckNil(action == nil) ) then return false end
	if( CheckNil(action:IsNil()) ) then return false end
	
	actor:SetDownState(false)
	actor:SetCanHit(true)		
	
	actor:SetSendBlowStatus(false)

	actor:FreeMove(false)
	actor:SetMeetFloor(true)
end

function Act_KnockBack_OnLeave(actor, action)
	if( CheckNil(actor == nil) ) then return true end
	if( CheckNil(actor:IsNil()) ) then return true end
			
	if( CheckNil(action == nil) ) then return false end
	if( CheckNil(action:IsNil()) ) then return false end
	
	if action:GetID() ~= "a_die" then
		
		local	kEffectDef = GetEffectDef(actor:GetAction():GetActionNo())
		local	fGodTime = 1
		if kEffectDef:IsNil() == false then
			fGodTime = kEffectDef:GetAbil(AT_GOD_TIME)/1000.0
		end
		actor:StartGodTime(fGodTime)
	end


	return	true;

end
