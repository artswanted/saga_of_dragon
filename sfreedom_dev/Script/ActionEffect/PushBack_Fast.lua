-- damage
--
function Act_PushBack_Fast_OnCheckCanEnter(actor,action)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil())
	local CurAction = actor:GetAction()
	CheckNil(nil==CurAction);
	CheckNil(CurAction:IsNil())

	action:SetParamFloat(6, 0)
	action:SetParamInt(9, 0)
	if action:GetScriptParam("IS_OVERLAP")~="TRUE" and CurAction:GetID()==action:GetID() then
		action:SetParamInt(9, 1)
	end

	return	true
	
end

function Act_PushBack_Fast_OnEnter(actor, action)
	--ODS("Act_PushBack_Fast_OnEnter:"..action:GetActionNo().."\n", false, 912)

	actor:StopJump();
	actor:StartGodTime(0);
	actor:SetMovingDelta(Point3(0,0,0));
	actor:Stop()
	actor:SetNoWalkingTarget(false);

	if 1==action:GetParamInt(9) then
		return true
	end

	local	fDistance = tonumber(action:GetScriptParam("DISTANCE"));
	if fDistance == nil then
		local iDistanceFromDB = action:GetAbil(AT_DISTANCE) --DB에서 읽어올수 있는 값이 있는가
		if(0 == iDistanceFromDB) then
			fDistance = 120
		else
			fDistance = iDistanceFromDB
		end
	end	

	local	fSpeed = tonumber(action:GetScriptParam("VELOCITY"));
	if fSpeed == nil then
		--fSpeed = 300
		fSpeed = fDistance * 2.5 -- 기존 120거리, 300 속력 이던것을 같은 비율로 사용
	end	

	local	ePushType = tonumber(action:GetScriptParam("PUSH_TYPE"));	-- 밀리는 옵션
	if nil == ePushType then
		-- 1 : 공격자 위치에서 피격자 방향으로 밀리는 옵션
		ePushType = 0
	end	

	local AttackActor = action:GetEffectCaster()
	if true==AttackActor:IsNil() then
		actor:PushActor(not actor:IsToLeft(),fDistance,fSpeed,1);
	else
		local kMovingDir = AttackActor:GetLookingDir()
		--ODS("action:GetAbil(AT_CUSTOMDATA1):"..action:GetAbil(AT_CUSTOMDATA1).."\n", false, 912)		
		if(0 < action:GetAbil(AT_CUSTOMDATA1) or 0 < ePushType) then -- 공격자 위치에서 피격자 방향으로 밀리는 옵션이 있는가
			kMovingDir = actor:GetPos()
			kMovingDir:Subtract( AttackActor:GetPos() )
			kMovingDir:Unitize()
		end
		actor:PushActorDir(kMovingDir,fDistance,fSpeed,1);		
	end	
	action:SetSlot(0);
	return true
end

function Act_PushBack_Fast_OnUpdate(actor, accumTime, frameTime)
	local action = actor:GetAction()
	if actor:IsAnimationDone() and actor:GetNowPush() == false then
		if 0==action:GetCurrentSlot() and 0==action:GetParamInt(9) then
			actor:SetSendBlowStatus(false, true);
		end

		local iNextSlot = action:GetCurrentSlot()+1
		if false==action:NextSlot() or actor:GetAnimationLength(action:GetSlotAnimName( iNextSlot )) == 0 then
			return	false
		end
		actor:PlayCurrentSlot()

		if actor:IsMyActor() then
			local	fLastPressTime = action:GetParamFloat(6)
			if accumTime - fLastPressTime < g_fBreakFallLimitTime then			
				actor:ReserveTransitAction("a_break_fall");
				action:SetParam(0,"end");
				return	false;
			end
		end
	end
	return true
end

function Act_PushBack_Fast_OnCleanUp(actor, action)
end

function Act_PushBack_Fast_OnLeave(actor, action)

	if 0==action:GetParamInt(9) then
	if action:GetID() ~= "a_die" then
		local kEffectDef = GetEffectDef(actor:GetAction():GetActionNo());
		local fGodTime = 1;
		if kEffectDef:IsNil() == false then
			fGodTime = kEffectDef:GetAbil(AT_GOD_TIME)/1000.0;
		end
		actor:StartGodTime(fGodTime);
	end
	
	actor:SetMovingDir(Point3(0, 0, 0))

		actor:SetSendBlowStatus(false, false);
	end
	return true
end
