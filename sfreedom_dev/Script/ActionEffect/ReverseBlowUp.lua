-- Idle
-- [VELOCITY] : Speed to blow up
-- [NO_HORI_MOVE] : 횡 이동을 할 것인가.
-- [LIGHTNING_DMG] : 라이트닝 대미지 인가

function Act_Reverse_BlowUp_OnEnter(actor, action)
	if( CheckNil(actor == nil) ) then return false end
	if( CheckNil(actor:IsNil()) ) then return false end
	
	if( CheckNil(action == nil) ) then return false end
	if( CheckNil(action:IsNil()) ) then return false end
	if( CheckNil(g_world == nil) ) then return false end
	if( CheckNil(g_world:IsNil()) ) then return false end
	
	local	actorID = actor:GetID()
	ODS("Act_Reverse_BlowUp_OnEnter actionID : "..action:GetID().." LIGHTNING:"..action:GetScriptParam("LIGHTNING_DMG").." \n");

	local	fVelocity = action:GetScriptParamAsInt("VELOCITY");

	action:SetParamFloat(1,fVelocity);	-- Initial Velocity
	action:SetParamInt(2,0);	-- Step
	action:SetParamFloat(5,fVelocity) 		-- action:GetScriptParamAsInt("VELOCITY")
	action:SetParamFloat(10,g_world:GetAccumTime());
	action:SetParamFloat(11,0);
	action:SetParamFloat(13,g_world:GetAccumTime());
	actor:FreeMove(true);	
	actor:StopJump();
	actor:SetNoWalkingTarget(false);
	
	actor:StartGodTime(0);
	actor:SetMovingDelta(Point3(0,0,0));
	
	action:SetParamInt(2,1);
	action:SetSlot(1);	
	--actor:SetCanHit(false);
	actor:SetBlowUp(true);
	actor:Concil(false)
	
	action:SetParamFloat(14,-1);
	if action:GetScriptParam("LIGHTNING_DMG") == "TRUE" then
	
		actor:HideNode("Scene Root",true);
		actor:AttachParticle(513,"char_root","ef_mag_lightning_shock");
		
		action:SetParamFloat(14,g_world:GetAccumTime());
		
	end
	
	local	kEffectDef = GetEffectDef(action:GetActionNo());
	local	fTotalDownTime = 3;
	if kEffectDef:IsNil() == false then
		fTotalDownTime = kEffectDef:GetDurationTime()/1000.0;
	end
	actor:SetTotalDownTime(fTotalDownTime);	
	
	return true
end

function Act_Reverse_BlowUp_OnUpdate(actor, accumTime, frameTime,action)
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
	local	fTotalDownTime = actor:GetTotalDownTime();
	
	
	if action:GetParamFloat(14)>=0 and action:GetScriptParam("LIGHTNING_DMG") == "TRUE" then
	
		local	fLightningElapsedTime = accumTime - action:GetParamFloat(14);
		if fLightningElapsedTime > 0.4 then
			
			actor:HideNode("Scene Root",false);
			actor:DetachFrom(513);
						
			action:SetParamFloat(14,-1);
		end
		
	end	
	
	if iStep == 0 or iStep == 1 then
	
		local	kMovingDir = actor:GetLookingDir();
		local	fZ_Velocity = action:GetParamFloat(1);
		
		--kMovingDir:SetX(-kMovingDir:GetX());
		--kMovingDir:SetY(-kMovingDir:GetY());
		
		kMovingDir:SetX(0);
		kMovingDir:SetY(0);
		kMovingDir:SetZ(fZ_Velocity);

		if actor:IsMyActor() then
			if fZ_Velocity<0 and action:GetParamInt(7) == 0 then
			
				ODS("Set LastPressed Time To 0\n");
				action:SetParamInt(7,1)
				action:SetParamFloat(6,0)
				
			end
		end		

		ODS("Step : " .. iStep .. " MovingDir X : " .. kMovingDir:GetX().. " Y : " .. kMovingDir:GetY() .. " Z : " .. kMovingDir:GetZ() .. "\n")
	
		actor:SetMovingDelta(kMovingDir);

		-- 일정 시간이 지나면 자동으로 떨어지도록 해주어야 한다.
		if 2.0 < fTotalElapsedTime then
			action:SetParamInt(2, iStep + 1);
		end
		
	elseif iStep == 2 then
		
		local	kMovingDir = actor:GetLookingDir();
		local	fZ_Velocity_Init = action:GetParamFloat(1);
		local	fZ_Velocity = g_fGravity*(0.5*frameTime+fTotalElapsedTime)+fZ_Velocity_Init;
		kMovingDir:SetZ(fZ_Velocity);
		
		if actor:IsMyActor() then
			if fZ_Velocity<0 and action:GetParamInt(7) == 0 then
			
				ODS("Set LastPressed Time To 0\n");
				action:SetParamInt(7,1)
				action:SetParamFloat(6,0)
				
			end
		end				
		
		actor:SetMovingDelta(kMovingDir);
	
	end
	if iStep == 4 then
	
		local	fElapsedDownTime = accumTime - action:GetParamFloat(3);
		if fElapsedDownTime >= fTotalDownTime then
			action:SetParam(0,"end");
			actor:SetDownState(false);

			if actor:IsUnitType(UT_PLAYER) then	--플레이어일 경우에만
				return true
			else
				actor:PlayNext();
			end

			return	false;
		end
	end

	if iStep == 3 and actor:IsAnimationDone() == true then
	
		action:SetParamInt(2,4);
		action:SetParamFloat(3,accumTime);	--	다운 시작 시간 기록

	end
	
	if actor:IsMeetFloor() == true then
	
		if iStep == 1 then
		
			local	fVelocity = action:GetParamFloat(5)*g_fBlowUpSecondJumpPower;
			local	fInitVelocity = math.sqrt(2*-g_fGravity*fVelocity);		
			
			action:SetParamFloat(1,fInitVelocity);	-- Initial Velocity
			action:SetParamFloat(11,0);
			action:SetParamInt(2,2);
			action:SetParamInt(4,0);
			local pt = actor:GetTranslate()
			pt:SetZ(pt:GetZ() - 30)
			actor:AttachParticleToPoint(200, pt, "e_jump")
			actor:PlayNext();
			actor:SetBlowUp(false);	
			actor:SetSendBlowStatus(false, true);
			actor:SetDownState(true);
			actor:FreeMove(false);
			
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
				actor:SetSendBlowStatus(false);

				actor:SetDownState(true);
				actor:SetCanHit(true);				
			
			end
		end
	
	end

	return true
end

function Act_Reverse_BlowUp_OnCleanUp(actor, action)
	
	if( CheckNil(actor == nil) ) then return false end
	if( CheckNil(actor:IsNil()) ) then return false end
	
	if( CheckNil(action == nil) ) then return false end
	if( CheckNil(action:IsNil()) ) then return false end
	
	actor:Concil(true)
	actor:SetBlowUp(false);
	actor:SetCanHit(true);				
	
	actor:SetDownState(false);

	actor:FreeMove(false);
end

function Act_Reverse_BlowUp_OnLeave(actor, action)
	ODS("Act_Reverse_BlowUp_OnLeave action:"..action:GetID().."\n");
	
	if( CheckNil(actor == nil) ) then return true end
	if( CheckNil(actor:IsNil()) ) then return true end
	
	if( CheckNil(action == nil) ) then return false end
	if( CheckNil(action:IsNil()) ) then return false end
	
	local	kCurAction = actor:GetAction();
	if( CheckNil(kCurAction == nil) ) then return true end
	if( CheckNil(kCurAction:IsNil()) ) then return true end
	
	
	
	if action:GetActionType() ~= "EFFECT" then
		
		local	kEffectDef = GetEffectDef(actor:GetAction():GetActionNo());
		local	fGodTime = 1;
		if kEffectDef:IsNil() == false then
			fGodTime = kEffectDef:GetAbil(AT_GOD_TIME)/1000.0;
		end
		actor:StartGodTime(fGodTime);
		
	end
	
	
	
	if kCurAction:GetParamFloat(14)>=0 and kCurAction:GetScriptParam("LIGHTNING_DMG") == "TRUE" then
	
		actor:HideNode("Scene Root",false);
		actor:DetachFrom(513);

		kCurAction:SetParamFloat(14,-1);
		
	end	
	


	return	true;

end
