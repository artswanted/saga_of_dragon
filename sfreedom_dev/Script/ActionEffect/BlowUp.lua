-- Idle
-- [VELOCITY] : Speed to blow up
-- [NO_HORI_MOVE] : 횡 이동을 할 것인가.
-- [LIGHTNING_DMG] : 라이트닝 대미지 인가

g_fGravity = -800.0
g_meleedrop = 0

function Act_BlowUp_OnEnter(actor, action)
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
	ODS("Act_BlowUp_OnEnter actionID : "..action:GetID().." LIGHTNING:"..action:GetScriptParam("LIGHTNING_DMG").." \n");

	local	fVelocity = action:GetScriptParamAsInt("VELOCITY");
	local	fInitVelocity = math.sqrt(2*-g_fGravity*fVelocity);

	action:SetParamFloat(1,fInitVelocity);	-- Initial Velocity
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
		ODS("actor:AttachParticle 12\n");
		
		action:SetParamFloat(14,g_world:GetAccumTime());
		
	end
	
	local	kEffectDef = GetEffectDef(action:GetActionNo());
	local	fTotalDownTime = 3;
	if kEffectDef:IsNil() == false then
		fTotalDownTime = kEffectDef:GetDurationTime()/1000.0;
	end

	if action:GetNextActionName()=="a_stand_up" then
		fTotalDownTime = 1.5
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

function Act_BlowUp_OnUpdate(actor, accumTime, frameTime,action)
	
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
			--ODS("actor:DetachFrom 12\n");
			
			
			action:SetParamFloat(14,-1);
		end
		
	end	

	if iStep == 0 or iStep == 1 then
	
		local	kMovingDir = actor:GetLookingDir();
		local MonVelo = actor:GetAnimationInfo("VELOCITY")
		if nil~=MonVelo then
			MonVelo = tonumber(MonVelo)
			action:SetParamFloat(5,MonVelo)
			action:SetParamFloat(1, math.sqrt(2*-g_fGravity*MonVelo))
		end
		local	fZ_Velocity_Init = action:GetParamFloat(1);
		
		local Param_Init = actor:GetAnimationInfo("VELOCITY_INIT")
		if nil~=Param_Init then
			if nil~=string.find(action:GetID(), "a_float_dmg") then
				fZ_Velocity_Init = tonumber(Param_Init)
			end
		end
		local	fZ_Velocity = g_fGravity*(0.5*frameTime+fTotalElapsedTime)+fZ_Velocity_Init;
		kMovingDir:SetZ(fZ_Velocity);
		
		if actor:IsMyActor() then
			if fZ_Velocity<0 and action:GetParamInt(7) == 0 then
			
				--ODS("Set LastPressed Time To 0\n");
				action:SetParamInt(7,1)
				action:SetParamFloat(6,0)
				
			end
		end		

		--ODS("Step : " .. iStep .. " MovingDir Z : " .. kMovingDir:GetZ() .." fZ_Velocity_Init :"..fZ_Velocity_Init.." "..action:GetID().. "\n")
		
		if kMovingDir:GetX() > 0 then
			g_meleedrop = g_meleedrop+1
			if g_meleedrop == 2 then
			--	SetBreak()
				g_meleedrop = 0
			end
		end

		if action:GetScriptParam("NO_HORI_MOVE") ~= "TRUE" then
			local AttackActor = action:GetEffectCaster()
			if false==AttackActor:IsNil() then
				local kAttackMovingDir = AttackActor:GetLookingDir()
				kMovingDir:SetX(kAttackMovingDir:GetX()*g_fBlowUpHorizonSpeed)
				kMovingDir:SetY(kAttackMovingDir:GetY()*g_fBlowUpHorizonSpeed)
			else
				kMovingDir:SetX(kMovingDir:GetX()*-1*g_fBlowUpHorizonSpeed);
				kMovingDir:SetY(kMovingDir:GetY()*-1*g_fBlowUpHorizonSpeed);
			end

		end
		actor:SetMovingDelta(kMovingDir);
		
	elseif iStep == 2 then
		
		local	kMovingDir = actor:GetLookingDir();
		local	fZ_Velocity_Init = action:GetParamFloat(1);
		local	fZ_Velocity = g_fGravity*(0.5*frameTime+fTotalElapsedTime)+fZ_Velocity_Init;
		kMovingDir:SetZ(fZ_Velocity);
		
		if actor:IsMyActor() then
			if fZ_Velocity<0 and action:GetParamInt(7) == 0 then
			
				--ODS("Set LastPressed Time To 0\n");
				action:SetParamInt(7,1)
				action:SetParamFloat(6,0)
				
			end
		end				
		
		if action:GetScriptParam("NO_HORI_MOVE") ~= "TRUE" then
			local AttackActor = action:GetEffectCaster()
			if false==AttackActor:IsNil() then
				local kAttackMovingDir = AttackActor:GetLookingDir()
				kMovingDir:SetX(kAttackMovingDir:GetX()*g_fBlowUpHorizonSpeed*0.2)
				kMovingDir:SetY(kAttackMovingDir:GetY()*g_fBlowUpHorizonSpeed*0.2)
			else
				kMovingDir:SetX(kMovingDir:GetX()*-1*g_fBlowUpHorizonSpeed*0.2);
				kMovingDir:SetY(kMovingDir:GetY()*-1*g_fBlowUpHorizonSpeed*0.2);
			end
			
		end
		
		actor:SetMovingDelta(kMovingDir);
	
	end
	if iStep == 4 then
	
		local	fElapsedDownTime = accumTime - action:GetParamFloat(3);

		if 0.3 > fTotalDownTime-fElapsedDownTime then
			actor:SetSendBlowStatus(false);
		end

		if fElapsedDownTime >= fTotalDownTime then
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

			else
				if actor:IsUnitType(UT_PLAYER) then	--플레이어일 경우에만
					return true
				else
					actor:PlayNext();
				end
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

				actor:SetDownState(true);
				actor:SetCanHit(true);				
			
			end
		end
	
	end

	return true
end

function Act_BlowUp_OnCleanUp(actor, action)
	if( CheckNil(actor == nil) ) then return false end
	if( CheckNil(actor:IsNil()) ) then return false end
	
	actor:Concil(true)
	actor:SetBlowUp(false);
	actor:SetCanHit(true);				
	
	actor:SetDownState(false);
	
	if true == actor:IsMyActor() then
	   CloseUI("FRM_SHOW_EVADE")
	end

	actor:FreeMove(false);
end

function Act_BlowUp_OnLeave(actor, action)
	
	if( CheckNil(actor == nil) ) then return true end
	if( CheckNil(actor:IsNil()) ) then return true end
	
	if( CheckNil(action == nil) ) then return false end
	if( CheckNil(action:IsNil()) ) then return false end
	
	ODS("Act_BlowUp_OnLeave action:"..action:GetID().."\n");

	if action:GetActionType() ~= "EFFECT" then
		
		local	kEffectDef = GetEffectDef(actor:GetAction():GetActionNo());
		local	fGodTime = 1;
		if kEffectDef:IsNil() == false then
			fGodTime = kEffectDef:GetAbil(AT_GOD_TIME)/1000.0;
		end
		actor:StartGodTime(fGodTime);
		
	end
	
	local	kCurAction = actor:GetAction();
	if( CheckNil(kCurAction == nil) ) then return true end
	if( CheckNil(kCurAction:IsNil()) ) then return true end
	
	if kCurAction:GetParamFloat(14)>=0 and kCurAction:GetScriptParam("LIGHTNING_DMG") == "TRUE" then
	
		actor:HideNode("Scene Root",false);
		actor:DetachFrom(513);
		ODS("actor:DetachFrom 12\n");

		kCurAction:SetParamFloat(14,-1);
		
	end	
	


	return	true;

end
