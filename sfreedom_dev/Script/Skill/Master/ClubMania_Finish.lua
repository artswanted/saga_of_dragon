-- [CHECK_CAN_PREV_ACTION_ID] : 연속되는 액션이므로 이전 액션을 체크해주어야 한다.

function Skill_Club_Mania_Finish_OnCastingCompleted(actor,action)
end

function Skill_Club_Mania_Finish_OnCheckCanEnter(actor,action)

	ODS("Skill_Club_Mania_Finish_OnCheckCanEnter \n",false, 1509)
	local	kCurrentAction = actor:GetAction();

	local   kPrevActionID = action:GetScriptParam("CHECK_CAN_PREV_ACTION_ID");
	if(false == kCurrentAction:IsNil() ) then
		return (kCurrentAction:GetID() == kPrevActionID);
	end
	
	return true;	
end

function Skill_Club_Mania_Finish_OnTimer(actor,fAccumTime,action,iTimerID)
	return Skill_Club_Mania_OnTimer(actor,fAccumTime,action,iTimerID);
end

function Skill_Club_Mania_Finish_OnEnter(actor, action)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());

	return true
end

function Skill_Club_Mania_Finish_OnUpdate(actor, accumTime, frameTime)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	local action = actor:GetAction()
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());

	actor:LockBidirection(false);
	actor:SeeFront(true);

	local animDone = actor:IsAnimationDone()
	local nextActionName = action:GetNextActionName()
	
	if true == animDone then
		if(false == actor:PlayNext()) then	-- 더이상 애니메이션이 없으면
			action:SetParam(1, "end")
			return	false;
		end
	end
	
	return	true;
end

function Skill_Club_Mania_Finish_OnCleanUp(actor)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	local   kCurAction = actor:GetAction();
	
	CheckNil(nil==kCurAction);
	CheckNil(kCurAction:IsNil());
	
	if actor:IsMyActor() then
		if( CheckNil(g_world == nil) ) then return true end
		if( CheckNil(g_world:IsNil()) ) then return true end
		g_world:SetShowWorldFocusFilterColorAlpha(0x000000, g_world:GetWorldFocusFilterAlpha(), 0, 1, false,false);
		g_world:ClearDrawActorFilter()
	end	

	actor:RestoreLockBidirection();

	local	iInstanceID = tonumber(actor:GetParam(200));
	GetStatusEffectMan():RemoveStatusEffectFromActor2(actor:GetPilotGuid(),iInstanceID);

	actor:SetCanHit(true);

	return true;
end

function Skill_Club_Mania_Finish_OnLeave(actor, action)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
		
	local curAction = actor:GetAction();
	local curParam = curAction:GetParam(1)
	local actionID = action:GetID()

	if(nil == string.find(actionID, "a_run")) then
		if curAction:GetNextActionName() ~= actionID then
			ODS("다른게 들어옴:"..actionID.."\n",false, 912)
			return false;
		end
	end
	
		
	if actor:IsMyActor() == false then
		ODS("종료됨1\n", false, 912);
		return true;
	end
	
	
	if action:GetActionType()=="EFFECT" then
		ODS("종료됨2\n", false, 912);
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
		curAction:SetParam(1,actionID);

		return false;
	
	elseif curParam == "end" and (actionID == "a_run_right" or actionID == "a_run_left") then
		return true;
	elseif 	actionID == "a_idle" or
		actionID == "a_handclaps" then
		return true;
	end
	
	return true;
end

function Skill_Club_Mania_Finish_OnEvent(actor,textKey)

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
			kAction:CreateActionTargetList(actor);

			if( CheckNil(g_world == nil) ) then return false end
			if( CheckNil(g_world:IsNil()) ) then return false end
			local i = 0;
			local kTargetList = kAction:GetTargetList();
			local iTargetCount = kTargetList:size();
			while i<iTargetCount do
				local   kTargetInfo = kTargetList:GetTargetInfo(i);
				local   kTargetGUID = kTargetInfo:GetTargetGUID();
				local   kTargetPilot = g_pilotMan:FindPilot(kTargetGUID);
				if kTargetPilot:IsNil() == false then
					if actor:IsMyActor() then
						g_world:AddDrawActorFilter(kTargetGUID);
					end
				end
				i = i + 1;
			end
	
			if IsSingleMode() then
				SkillHelpFunc_DefaultHitOneTime(actor,kAction)
				Skill_Club_Mania_Finish_OnTargetListModified(actor,kAction,false)

				return;
			else
				kAction:BroadCastTargetListModify(actor:GetPilot());
			end
		end	
	
        SkillHelpFunc_DefaultHitOneTime(actor,kAction)
		
		-- 데몰리션 봄버 막타 이펙트		
	end
	
	return	true;
end

function Skill_Club_Mania_Finish_OnTargetListModified(actor,action,bIsBefore)
	return Skill_Club_Mania_OnTargetListModified(actor,action,bIsBefore)
end
