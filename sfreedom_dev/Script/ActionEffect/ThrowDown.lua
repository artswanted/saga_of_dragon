-- Idle
-- [VELOCITY] : Speed to blow up
-- [VELOCITY2] : Speed to blow up2

function Act_ThrowDown_OnEnter(actor, action)
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

	local	fVelocity = -200;--action:GetScriptParamAsInt("VELOCITY")*2;
	local	fInitVelocity = math.sqrt(2*-g_fGravity*fVelocity);

	action:SetParamFloat(1,fInitVelocity);	-- Initial Velocity
	action:SetParamInt(2,0);	-- Step
	action:SetParamFloat(10,g_world:GetAccumTime());
	action:SetParamFloat(11,0);
	action:SetParamFloat(13,g_world:GetAccumTime());
	actor:FreeMove(true);	
	actor:StopJump();
	actor:StartGodTime(0);
	actor:SetMovingDelta(Point3(0,0,0));
	actor:SetNoWalkingTarget(false);
	actor:Concil(false)
			
	action:SetParamInt(2,1);
	action:SetSlot(1)
	--actor:SetCanHit(false);
	
	actor:StartBodyTrail("../Data/5_Effect/9_Tex/swift_01.tga",500,0,0);
	
	action:SetParamFloat(14,-1);
	if action:GetScriptParam("LIGHTNING_DMG") == "TRUE" then
	
		actor:HideNode("Scene Root",true);
		actor:AttachParticle(513,"char_root","ef_mag_lightning_shock");
		ODS("actor:AttachParticle 12\n");
		
		action:SetParamFloat(14,g_world:GetAccumTime());
		
	end
	
	local	kEffectDef = GetEffectDef(action:GetActionNo());
	local	fTotalDownTime = 3;
	if kEffectDef:IsNil() == false then
		fTotalDownTime = kEffectDef:GetDurationTime()/1000.0;
	end
	actor:SetTotalDownTime(fTotalDownTime);	
	
	local iLevel = actor:GetAbil(AT_LEVEL)
	if true == actor:IsMyActor() and 
	   true == g_bUseBreakFallWhenStandUp and
	   20 >= iLevel then
	   ActivateUI("FRM_SHOW_EVADE")
	end
	
	return true
end

function Act_ThrowDown_OnUpdate(actor, accumTime, frameTime,action)
	if( CheckNil(actor == nil) ) then return false end
	if( CheckNil(actor:IsNil()) ) then return false end
	
	if( CheckNil(action == nil) ) then return false end
	if( CheckNil(action:IsNil()) ) then return false end
	
	local	currentSlot = action:GetCurrentSlot()
	local	param = action:GetParam(0)
	
	local	iStep = action:GetParamInt(2);
	
	local	fTotalElapsedTime = action:GetParamFloat(11);
	action:SetParamFloat(11,fTotalElapsedTime+frameTime);	

	if action:GetParamFloat(14)>=0 and action:GetScriptParam("LIGHTNING_DMG") == "TRUE" then
	
		local	fLightningElapsedTime = accumTime - action:GetParamFloat(14);
		if fLightningElapsedTime > 0.4 then
			
			actor:HideNode("Scene Root",false);
			actor:DetachFrom(513);
			ODS("actor:DetachFrom 12\n");
			
			
			action:SetParamFloat(14,-1);
		end
		
	end	

	if iStep == 0 or iStep == 1 then
		
		local	kMovingDir = actor:GetLookingDir();
		local	fZ_Velocity_Init = action:GetParamFloat(1);
		local	fZ_Velocity = g_fGravity*(0.5*frameTime+fTotalElapsedTime)*15+fZ_Velocity_Init;

		if actor:IsMyActor() then
			if fZ_Velocity<0 and action:GetParamInt(7) == 0 then
			
				ODS("Set LastPressed Time To 0\n");
				action:SetParamInt(7,1)
				action:SetParamFloat(6,0)
				
			end
		end
			
		kMovingDir:SetX(kMovingDir:GetX()*-1*200);
		kMovingDir:SetY(kMovingDir:GetY()*-1*200);
			
		kMovingDir:SetZ(fZ_Velocity);
		actor:SetMovingDelta(kMovingDir);
		
	elseif iStep == 2 then

		local	fVelocity2 = action:GetScriptParamAsInt("VELOCITY");
		if fVelocity2 == nil then
			fVelocity2 = 0;
		end		
		
		local	kMovingDir = actor:GetLookingDir();
		local	fZ_Velocity_Init = action:GetParamFloat(1);
		local	fZ_Velocity = g_fGravity*(0.5*frameTime+fTotalElapsedTime)+fZ_Velocity_Init + fVelocity2;

		kMovingDir:SetZ(fZ_Velocity);
		kMovingDir:SetX(kMovingDir:GetX()*-50);
		kMovingDir:SetY(kMovingDir:GetY()*-50);

		kMovingDir:Multiply(actor:GetAnimSpeed());
		actor:SetMovingDelta(kMovingDir);
		
		if actor:IsMyActor() then
			if fZ_Velocity<0 and action:GetParamInt(7) == 0 then
			
				ODS("Set LastPressed Time To 0\n");
				action:SetParamInt(7,1)
				action:SetParamFloat(6,0)
				
			end
		end				
	
	end

	if iStep == 4 then

		local	fElapsedDownTime = accumTime - action:GetParamFloat(3);

		if fElapsedDownTime>=actor:GetTotalDownTime() then			
			action:SetParam(0,"end");
			actor:SetDownState(false);
	
			if actor:IsMyActor() then
				if KeyIsDown(KEY_LEFT) or 
					KeyIsDown(KEY_RIGHT) or 
					KeyIsDown(KEY_UP) or
					KeyIsDown(KEY_DOWN) then
					if g_bUseBreakFallWhenStandUp == true then
						actor:ReserveTransitAction("a_break_fall");
					else
						actor:ReserveTransitAction("a_stand_up");
					end
				else
					actor:ReserveTransitAction("a_stand_up");
				end
			else				actor:PlayNext();
			end
			
			return	false;
		end
	
	end
	if iStep == 3 and actor:IsAnimationDone() == true then
	
		actor:SetSendBlowStatus(false, false, true);
		action:SetParamInt(2,4);
		action:SetParamFloat(3,accumTime);	--	다운 시작 시간 기록

	
	end
	
	if actor:IsMeetFloor() == true then
	
		if iStep == 1 then
		
			local	fVelocity = 30;
			local	fInitVelocity = math.sqrt(2*-g_fGravity*fVelocity);

			action:SetParamFloat(1,fInitVelocity);	-- Initial Velocity
			action:SetParamFloat(11,0);
			action:SetParamInt(2,2);
			action:SetParamInt(4,0);
			local pt = actor:GetTranslate()
			pt:SetZ(pt:GetZ() - 30)
			actor:AttachParticleToPoint(200, pt, "ef_monster_down")
			actor:PlayNext();
			
			if actor:IsMyActor() then
			
				local	fLastPressTime = action:GetParamFloat(6)
				if accumTime - fLastPressTime < g_fBreakFallLimitTime then
				
					actor:ReserveTransitAction("a_break_fall");
					action:SetParam(0,"end");
					return	false;
				
				end
			
			end					
			
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

function Act_ThrowDown_OnCleanUp(actor, action)
	if( CheckNil(actor == nil) ) then return false end
	if( CheckNil(actor:IsNil()) ) then return false end
	
	if( CheckNil(action == nil) ) then return false end
	if( CheckNil(action:IsNil()) ) then return false end
	
	actor:EndBodyTrail();
	actor:SetCanHit(true);				
	actor:Concil(true)
	
	actor:SetDownState(false);

	actor:FreeMove(false);
	
	if true == actor:IsMyActor() then
	   CloseUI("FRM_SHOW_EVADE")
	end
end


function Act_ThrowDown_OnLeave(actor, action)
	if( CheckNil(actor == nil) ) then return true end
	if( CheckNil(actor:IsNil()) ) then return true end
	
	if( CheckNil(action == nil) ) then return false end
	if( CheckNil(action:IsNil()) ) then return false end
	
	local	kCurAction = actor:GetAction();
	if( CheckNil(kCurAction == nil) ) then return true end
	if( CheckNil(kCurAction:IsNil()) ) then return true end
	
	if kCurAction:GetParamFloat(14)>=0 and kCurAction:GetScriptParam("LIGHTNING_DMG") == "TRUE" then
	
		actor:HideNode("Scene Root",false);
		actor:DetachFrom(513);
		ODS("actor:DetachFrom 12\n");

		kCurAction:SetParamFloat(14,-1);
		
	end	
	
	if action:GetActionType() ~= "EFFECT" then
		

	
	end

	return	true;

end
