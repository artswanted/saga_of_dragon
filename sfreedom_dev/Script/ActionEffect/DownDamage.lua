-- damage

function Act_Down_Damage_OnEnter(actor, action)
	if( CheckNil(actor == nil) ) then return false end
	if( CheckNil(actor:IsNil()) ) then return false end
			
	if( CheckNil(action == nil) ) then return false end
	if( CheckNil(action:IsNil()) ) then return false end
	
	if actor:IsRidingPet() then --라이딩펫: 탑승 중 피격당하면 강제 하차→피격 액션 진행
		local kActorPet = actor:GetMountTargetPet()
		if kActorPet:IsNil() == false then
			kActorPet:ReserveTransitAction("a_mount")
		end
	end

	actor:Stop()
	actor:ResetAnimation();
	action:SetParam(0,"");
	action:SetParamInt(1,0);	--	State
	actor:StopJump();
	actor:SetNoWalkingTarget(false);
	actor:StartGodTime(0);
	actor:SetMovingDelta(Point3(0,0,0));
	
	actor:SetDownState(true);
	actor:SetBlowUp(false);
	
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
function Act_Down_Damage_OnUpdate(actor, accumTime, frameTime,action)
	if( CheckNil(actor == nil) ) then return false end
	if( CheckNil(actor:IsNil()) ) then return false end
			
	if( CheckNil(action == nil) ) then return false end
	if( CheckNil(action:IsNil()) ) then return false end
	
	local	iState = action:GetParamInt(1);
	
	if iState == 0 then
	
		local	fElapsedTime = accumTime - action:GetActionEnterTime();
		local	fTotalPlayTime = actor:GetTotalDownTime();
		
		if fElapsedTime > fTotalPlayTime then
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
	
	return true
end

function Act_Down_Damage_OnCleanUp(actor, action)
	if( CheckNil(actor == nil) ) then return false end
	if( CheckNil(actor:IsNil()) ) then return false end
			
	-- if( CheckNil(action == nil) ) then return false end
	-- if( CheckNil(action:IsNil()) ) then return false end
	actor:SetDownState(false);
	
	if true == actor:IsMyActor() then
	   CloseUI("FRM_SHOW_EVADE")
	end
	return true
end

function Act_Down_Damage_OnLeave(actor, action)
	if( CheckNil(actor == nil) ) then return true end
	if( CheckNil(actor:IsNil()) ) then return true end
			
	if( CheckNil(action == nil) ) then return false end
	if( CheckNil(action:IsNil()) ) then return false end
	
	local kCurAction = actor:GetAction()
	if( CheckNil(kCurAction == nil) ) then return true end
	if( CheckNil(kCurAction:IsNil()) ) then return true end
	ODS("Act_Down_Damage_OnLeave\n");
	
	if action:GetActionType() ~= "EFFECT" then
		
		--actor:SetSendBlowStatus(false);
		
		local	kEffectDef = GetEffectDef(actor:GetAction():GetActionNo());
		local	fGodTime = 1;
		if kEffectDef:IsNil() == false then
			fGodTime = kEffectDef:GetAbil(AT_GOD_TIME)/1000.0;
		end
		actor:StartGodTime(fGodTime);
		
	end

	return true
end
