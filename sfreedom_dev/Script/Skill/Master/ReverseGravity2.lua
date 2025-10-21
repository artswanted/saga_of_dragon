-- [CHECK_CAN_PREV_ACTION_ID] : 연속되는 액션이므로 이전 액션을 체크해주어야 한다.
-- [HIT_TARGET_EFFECT_TARGET_NODE] : HIT 키에서 타겟에게 이펙트를 붙일때, 붙일 노드
-- [HIT_TARGET_EFFECT_ID] : HIT 키에서 타겟에게 붙일 이펙트 ID

function Skill_Reverse_Gravity2_OnCastingCompleted(kActor,kAction)
end

function Skill_Reverse_Gravity2_OnCheckCanEnter(actor,action)

	local	kCurrentAction = actor:GetAction();
	if( false == kCurrentAction:IsNil() ) then
		local   kPrevActionID = action:GetScriptParam("CHECK_CAN_PREV_ACTION_ID");
		return (kCurrentAction:GetID() == kPrevActionID);
	end
	
	return true;
end

function Skill_Reverse_Gravity2_OnTimer(actor,fAccumTime,action,iTimerID)
	return Skill_Reverse_Gravity_OnTimer(actor,fAccumTime,action,iTimerID);
end

function Skill_Reverse_Gravity2_OnEnter(actor, action)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());

	action:SetParamInt(21, action:GetSkillRange(0,actor) / 5 * 3) -- 이펙트 범위
	--action:StartTimer(0.9, 0.1, 1); -- 타이머 아이디가 1

	return true
end

function Skill_Reverse_Gravity2_OnUpdate(actor, accumTime, frameTime)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	local action = actor:GetAction()
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());

	actor:LockBidirection(false);
	actor:SeeFront(true);

	local animDone = actor:IsAnimationDone()
	local nextActionName = action:GetNextActionName()
	local iSlot = action:GetCurrentSlot();

	if action:GetParam(1) == "end" then
		return	false;
	end

	if animDone == true then
		local curActionParam = action:GetParam(0)
		if curActionParam == "GoNext" then
			action:SetParam(0, "null")
			action:SetParam(1, "end")
			return false;
		else
			action:SetParam(1, "end")
			return false
		end
	end		
		
	return	true;
end

function Skill_Reverse_Gravity2_OnCleanUp(actor)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());

	local action = actor:GetAction()
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());

	local kEndAction;
	
	if false == action:IsNil() then
		kEndAction = action:GetScriptParam("END_ACTION");
	end

	if actor:IsMyActor() and kEndAction == "TRUE" then
		if( CheckNil(g_world == nil) ) then return true end
		if( CheckNil(g_world:IsNil()) ) then return true end
		g_world:SetShowWorldFocusFilterColorAlpha(0x000000, g_world:GetWorldFocusFilterAlpha(), 0, 1, false,false);
		g_world:ClearDrawActorFilter()
	end	

	if kEndAction == "TRUE" then
		actor:DetachFrom(7212,true)
		actor:DetachFrom(7213,true)
		actor:SetCanHit(true);
	end
	actor:RestoreLockBidirection();
	
	return true;
end

function Skill_Reverse_Gravity2_OnLeave(actor, action)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
	local curAction = actor:GetAction();
	local curParam = curAction:GetParam(1)
	local actionID = action:GetID()
	
	if actor:IsMyActor() == false then
		return true;
	end
	
	if action:GetActionType()=="EFFECT" then
		
		return true;
	end
	
	if curParam == "end" then 
		return true;
	end
	
	if action:GetEnable() == false then
		if curParam == actionID then
			curAction:SetParam(0,"");
		end
	end
	
	if action:GetEnable() == true then
		curAction:SetParam(0,"GoNext");
		return false;
	end

	return false 
end

function Skill_Reverse_Gravity2_OnEvent(actor,textKey)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
		
	local kAction = actor:GetAction();
		
	CheckNil(nil==kAction);
	CheckNil(kAction:IsNil());

	if kAction:GetActionParam() == AP_CASTING then
		return true;
	end
	
	if textKey == "hit" or textKey == "fire" then

		if actor:IsMyActor() then
			if actor:IsMyActor() then
				local iQuakeTime = kAction:GetScriptParamAsFloat("QUAKE_TIME");
				local iQuakePower = kAction:GetScriptParamAsFloat("QUAKE_POWER");
				if 0 < iQuakeTime and 0 < iQuakePower then
					QuakeCamera(iQuakeTime, iQuakePower, 1, 1, 1);
				end
			end

			kAction:CreateActionTargetList(actor);

			local i = 0;
			local kTargetList = kAction:GetTargetList();
			local iTargetCount = kTargetList:size();
			while i<iTargetCount do
				local   kTargetInfo = kTargetList:GetTargetInfo(i);
				local   kTargetGUID = kTargetInfo:GetTargetGUID();
				local   kTargetPilot = g_pilotMan:FindPilot(kTargetGUID);
				if kTargetPilot:IsNil() == false then
					if actor:IsMyActor() then
						if( CheckNil(g_world == nil) ) then return false end
						if( CheckNil(g_world:IsNil()) ) then return false end
						g_world:AddDrawActorFilter(kTargetGUID);
					end
				end
				i = i + 1;
			end
	
			if IsSingleMode() then
				Skill_Reverse_Gravity_HitOneTime(actor,kAction)
				Skill_Reverse_Gravity2_OnTargetListModified(actor,kAction,false)

				return;
			else
				kAction:BroadCastTargetListModify(actor:GetPilot());
				kAction:ClearTargetList();
			end
		end	
	
        --Skill_Reverse_Gravity_HitOneTime(actor,kAction)

	end

	return	true;
end

function Skill_Reverse_Gravity2_OnTargetListModified(actor,action,bIsBefore)
	return Skill_Reverse_Gravity_OnTargetListModified(actor,action,bIsBefore)
end
