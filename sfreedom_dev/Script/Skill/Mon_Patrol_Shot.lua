-- skill
--
function	Act_Mon_PATROL_SHOT_LoadToWeapon(actor,action,iProjectileIndex)

	if( CheckNil(nil==actor) ) then return nil end
	if( CheckNil(actor:IsNil()) ) then return nil end
	
	if( CheckNil(nil==action) ) then return nil end
	if( CheckNil(action:IsNil()) ) then return nil end
	
	local	kProjectileMan = GetProjectileMan();
	local	kProjectileID = actor:GetAnimationInfo("PROJECTILE_ID")
	
	if kProjectileID == nil or kProjectileID == "" then
		kProjectileID = "PROJECTILE_Raum_Egg"
	end
	
	local	kNewArrow = kProjectileMan:CreateNewProjectile(kProjectileID,action,actor:GetPilotGuid());
	
	local	kTargetNodeID = actor:GetAnimationInfo("FIRE_START_NODE");
	if kTargetNodeID == nil or kTargetNodeID == "" then
		kTargetNodeID = "p_ef_heart"
	end
	
	kNewArrow:LoadToHelper(actor,kTargetNodeID);	--	장전
	return kNewArrow;

end

function Act_Mon_PATROL_SHOT_OnCheckCanEnter(actor, action)
	return true
end

function Act_Mon_PATROL_SHOT_SetState(actor,action,kState)

	if( CheckNil(nil==actor) ) then return false end
	if( CheckNil(actor:IsNil()) ) then return false end
	
	if( CheckNil(nil==action) ) then return false end
	if( CheckNil(action:IsNil()) ) then return false end
	
	local	iNewState = -1;

	if kState == "BATTLEIDLE_START" then
		actor:ToLeft(true, true)
		if actor:GetAnimationLength(action:GetSlotAnimName(0)) == 0 then
			return	false	
		end
				
		action:SetSlot(0)
		iNewState = 0;
	
	elseif kState == "BATTLEIDLE_LOOP" then
		actor:ToLeft(true, true)		
		action:SetSlot(1)
		if actor:GetAnimationLength(action:GetSlotAnimName(1)) == 0 then
		
			ODS("Act_Mon_PATROL_SHOT_SetState SlotName["..action:GetSlotAnimName(1).."] Not Exist -> SetSlot To Next\n");
		
			action:SetSlot(5);	--	배틀 Idle 모션이 없을 때는 그냥 Idle 모션을 하자.
		end
		iNewState = 1;

		--actor:AttachParticle(129,"char_root","ef_Mahalka_skill_03_01_char_root");		
	elseif kState == "FIRE" then
		actor:ToLeft(true, true)
		action:SetSlot(2)
		iNewState = 2;
	elseif kState == "RETURN1" then
		actor:ToLeft(true, true)
		action:SetSlot(3)
		if actor:GetAnimationLength(action:GetSlotAnimName(3)) == 0 then	
			action:SetSlot(5)	--	Finish 모션이 없을 때는 그냥 Idle 모션을 하자.
		end

		iNewState = 3;
	elseif kState == "RETURN2" then
		actor:ToLeft(true, true)
		action:SetSlot(4)
		if actor:GetAnimationLength(action:GetSlotAnimName(4)) == 0 then	
			action:SetSlot(5)	--	Finish 모션이 없을 때는 그냥 Idle 모션을 하자.
		end

		iNewState = 4
	end
	
	actor:ResetAnimation();
	actor:PlayCurrentSlot();
	action:SetParamInt(0,iNewState)

	return	true;

end

function Act_Mon_PATROL_SHOT_OnEnter(actor, action)

	if( CheckNil(nil==actor) ) then return false end
	if( CheckNil(actor:IsNil()) ) then return false end
	
	if( CheckNil(nil==action) ) then return false end
	if( CheckNil(action:IsNil()) ) then return false end
	
	ODS("Act_Mon_PATROL_SHOT_OnEnter actor : "..actor:GetID().." action: "..action:GetID().." ActionParam : "..action:GetActionParam().."\n")

	actor:SetNoWalkingTarget(false)
	actor:FreeMove(true)
--	actor:ToLeft(true, true)
	
	local	prevAction = actor:GetAction()
	if( false == prevAction:IsNil() ) then
		if prevAction:GetID() ~= "a_jump" then
			actor:Stop()
		end
	end
		
	actor:ResetAnimation()
	
	if action:GetActionParam() == AP_FIRE then
		Act_Mon_PATROL_SHOT_OnCastingCompleted(actor,action)
	else
		if( Act_Mon_PATROL_SHOT_SetState(actor,action,"BATTLEIDLE_START") == false) then	--스타트가 없으면 루프로
			Act_Mon_PATROL_SHOT_SetState(actor,action,"BATTLEIDLE_LOOP")
		end	
	end
	
	action:SetParamFloat(1,0)	--날기 시작 시간 초기화
	action:SetParamFloat(2,0)	--포탄 떨어뜨리는 딜레이 시간 저장
	action:SetParamInt(3,0)	--프로젝틸 카운트
	action:SetParamFloat(5,0)	--내려오기 시작하는 시간 저장
	action:SetParamAsPoint(7,actor:GetPos())--시작 좌표
	action:SetParamAsPoint(8,actor:GetPos())--시작 좌표
	action:SetParamFloat(9,0)	--ADD_Z값
	return true
end
function Act_Mon_PATROL_SHOT_OnCastingCompleted(actor,action)	

	Act_Mon_PATROL_SHOT_SetState(actor,action,"FIRE")
	
	Act_Mon_PATROL_SHOT_Fire(actor,action);	
end

function Act_Mon_PATROL_SHOT_Fire(actor,action)

	if( CheckNil(nil==action) ) then return false end
	if( CheckNil(action:IsNil()) ) then return false end
	
	--actor:AttachParticle(129,"char_root","ef_Mahalka_skill_03_02_char_root");
	if 0>=action:GetParamFloat(1) then
		if( CheckNil(g_world == nil) ) then return false end
		if( CheckNil(g_world:IsNil()) ) then return false end
		action:SetParamFloat(1, g_world:GetAccumTime())--날기 시작하는 시간 저장
		local rand = Random()%5*0.1+0.1
		action:SetParamFloat(2, g_world:GetAccumTime()+rand)--다음 포탄 떨어뜨릴 시간 저장
	end
end

function Act_Mon_PATROL_SHOT_OnUpdate(actor, accumTime, frameTime)

	if( CheckNil(nil==actor) ) then return false end
	if( CheckNil(actor:IsNil()) ) then return false end
		
	local	action = actor:GetAction()
		
	if( CheckNil(nil==action) ) then return false end
	if( CheckNil(action:IsNil()) ) then return false end
	
	local	iState = action:GetParamInt(0)
	
	if action:GetActionParam() == AP_FIRE then
--		local delta = accumTime - action:GetParamFloat(1)
		local bRet = true
		local param = action:GetParamInt(0)
		if 2==param then
			if actor:IsAnimationDone() then
				action:SetParamAsPoint(7,actor:GetPos():_Subtract(action:GetParamAsPoint(8)))-- 내려 가야될 벡터
				Act_Mon_PATROL_SHOT_SetState(actor,action,"RETURN1")
				action:SetParamFloat(5, accumTime)
			end
		elseif 3==param then	--return1
			if actor:IsAnimationDone() then
				Act_Mon_PATROL_SHOT_SetState(actor,action,"RETURN2")
			else	-- 올라갔으면 내려오자
				local deltareturn = accumTime - action:GetParamFloat(5)
				local anilen = actor:GetAnimationLength(action:GetSlotAnimName(action:GetParamInt(0)))
				local rate = 1 - deltareturn/anilen	--주의. 루프 애니일 경우 시간 구하면 어떻하지??
				if 0 < rate then
					local loc = action:GetParamAsPoint(8):_Add(action:GetParamAsPoint(7):_Multiply(rate))
					actor:SetTranslate(loc)	--원래 좌표로 복구
				end
			end
		elseif 4==param then	--return2
			if actor:IsAnimationDone() then
				actor:SetTranslate(action:GetParamAsPoint(8))	--원래 좌표로 복구
				bRet = false
			end
		end
		return bRet
		
	elseif action:GetActionParam() == AP_CASTING then
		local fAddZ = actor:GetAnimationInfo("ADD_Z")
		
		if nil ~= fAddZ then
			fAddZ = tonumber(fAddZ)
			local pos = actor:GetPos()
			pos:SetZ(pos:GetZ() + fAddZ*frameTime)
			actor:SetTranslate(pos)
			action:SetParamFloat(9,fAddz)	--ADD_Z값
		end
		
		if iState == 0 then
			if actor:IsAnimationDone() then
				Act_Mon_PATROL_SHOT_SetState(actor,action,"BATTLEIDLE_LOOP")
			end
		end
	
		if IsSingleMode() then
			if accumTime - action:GetActionEnterTime() > action:GetAbil(AT_CAST_TIME)/1000.0 then
			
				action:SetActionParam(AP_FIRE)
				Act_Mon_PATROL_SHOT_OnCastingCompleted(actor,action)
			
			end
		end
	else
		ODS("actionparam : " .. action:GetActionParam() .. "\n")
	end

	return true
end
function Act_Mon_PATROL_SHOT_OnCleanUp(actor, action)

	if( CheckNil(nil==actor) ) then return false end
	if( CheckNil(actor:IsNil()) ) then return false end
	
--	actor:DetachFrom(129)
	actor:FreeMove(false)
end

function Act_Mon_PATROL_SHOT_OnLeave(actor, action)

	if( CheckNil(nil==actor) ) then return true end
	if( CheckNil(actor:IsNil()) ) then return true end
	
	if( CheckNil(nil==action) ) then return true end
	if( CheckNil(action:IsNil()) ) then return true end
	
	local	kCurAction = actor:GetAction();
	local	actionID = action:GetID()
	ODS("Act_Mon_PATROL_SHOT_OnLeave nextaction : "..actionID.." CurAction : "..kCurAction:GetID().." CurAction AP:"..kCurAction:GetActionParam().." \n");
	if action:GetActionType()=="EFFECT" or
		actionID == "a_freeze" or
		actionID == "a_reverse_gravity" or
		actionID == "a_blow_up" or
		actionID == "a_blow_up_small" or
		actionID == "a_knock_back" or
		actionID == "a_die" or
		actionID == "a_idle" or
		actionID == "a_run_right" or
		actionID == "a_run_left" or
		actionID == "a_handclaps" then
		return true
	end
	return false 
end
function Act_Mon_PATROL_SHOT_OnTargetListModified(actor,action,bIsBefore)

end

function Act_Mon_PATROL_SHOT_OnEvent(actor,textKey)

	if( CheckNil(nil==actor) ) then return false end
	if( CheckNil(actor:IsNil()) ) then return false end
		
	local kAction = actor:GetAction()
	
	if( CheckNil(nil==kAction) ) then return end
	if( CheckNil(kAction:IsNil()) ) then return false end
	
	if kAction:GetActionParam() == AP_CASTING then
		return true
	end

	if textKey == "hit" or textKey == "fire" then
		local kFireDir = Point3(0,0,-1)
		local	kTargetNodeID = actor:GetAnimationInfo("FIRE_START_NODE");
		if kTargetNodeID == nil or kTargetNodeID == "" then
			kTargetNodeID = "p_ef_heart"
		end
		local nodepos = actor:GetNodeWorldPos(kTargetNodeID)
		if( CheckNil(g_world == nil) ) then return false end
		if( CheckNil(g_world:IsNil()) ) then return false end
		local kFireTargetPos = g_world:ThrowRay(nodepos,kFireDir,1000)
		if kFireTargetPos:GetX() ~= -1 and kFireTargetPos:GetY() ~= -1 and kFireTargetPos:GetZ() ~= -1 then
			local kNewArrow = Act_Mon_PATROL_SHOT_LoadToWeapon(actor,kAction,kAction:GetParamInt(3))
			kAction:SetParamInt(3, kAction:GetParamInt(3)+1)
			if nil~=kNewArrow and false == kNewArrow:IsNil() then
				kNewArrow:SetParentActionInfo(kAction:GetActionNo(),kAction:GetActionInstanceID(),kAction:GetTimeStamp())
				kNewArrow:SetTargetLoc(kFireTargetPos)
				kNewArrow:Fire(true)
			end
		end
	end
	return true
end
