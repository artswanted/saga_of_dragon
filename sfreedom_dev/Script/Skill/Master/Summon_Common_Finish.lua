-- [CHECK_CAN_PREV_ACTION_ID] : 연속되는 액션이므로 이전 액션을 체크해주어야 한다.
function Skill_Master_Summon_Finish_OnCastingCompleted(actor, action)
end

function Skill_Master_Summon_Finish_OnCheckCanEnter(actor,action)

	ODS("Skill_Master_Summon_Finish_OnCheckCanEnter \n",false, 1509)
	local	kCurrentAction = actor:GetAction();
	if ( false == kCurrentAction:IsNil() ) then
		local   kPrevActionID = action:GetScriptParam("CHECK_CAN_PREV_ACTION_ID");
		return (kCurrentAction:GetID() == kPrevActionID);
	end
	
	return true;	
end

function Skill_Master_Summon_Finish_OnTimer(actor,fAccumTime,action,iTimerID)
	return Skill_Master_Summon_Common_OnTimer(actor,fAccumTime,action,iTimerID);
end

function Skill_Master_Summon_Finish_OnEnter(actor, action)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());

	if "a_Summon_Dragon_Rush2" == action:GetID() then
		action:SetParamInt(21, action:GetSkillRange(0,actor) / 5 * 3) -- 이펙트 범위
		action:StartTimer(2.6, 0.04, 1); -- 타이머 아이디가 1
	end

	return true
end

function Skill_Master_Summon_Finish_OnUpdate(actor, accumTime, frameTime)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	local action = actor:GetAction()
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());

	actor:LockBidirection(false);
	actor:SeeFront(true);

	local animDone = actor:IsAnimationDone()
	local nextActionName = action:GetNextActionName()

	if action:GetParam(1) == "end" then
		return	false;
	end
	
	if animDone == true then
		if(false == actor:PlayNext()) then	-- 더이상 애니메이션이 없으면			
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
	end		

	return true
end

function Skill_Master_Summon_Finish_OnCleanUp(actor)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	local   kCurAction = actor:GetAction();
	
	CheckNil(nil==kCurAction);
	CheckNil(kCurAction:IsNil());
	
	if actor:IsMyActor() then
		if( CheckNil(g_world == nil) ) then return true end
		if( CheckNil(g_world:IsNil()) ) then return true end
		g_world:SetShowWorldFocusFilterColorAlpha(0xFFFFFF, g_world:GetWorldFocusFilterAlpha(), 0, 1, false,false);
		g_world:ClearDrawActorFilter()
	end	

	actor:RestoreLockBidirection();
	
	if "a_groundzero_finish" == kCurAction:GetID() then
	-- 그라운드 제로 일때, 수류탄 제거
		actor:DetachFrom(2004, true)
	end

	actor:SetCanHit(true);

	return true;
end

function Skill_Master_Summon_Finish_OnLeave(actor, action)

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

function Skill_Master_Summon_Finish_OnEvent(actor,textKey)

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
				Skill_Master_Summon_Finish_OnTargetListModified(actor,kAction,false)

				return;
			else
				kAction:BroadCastTargetListModify(actor:GetPilot());
			end
		end	
	
        SkillHelpFunc_DefaultHitOneTime(actor,kAction)
		
		-- 데몰리션 봄버 막타 이펙트
		if "a_Summon_Demolition_Bomber2" == kAction:GetID() then
			kAction:SetParamInt(20);
			kAction:SetParamInt(21, 300);
			local i = 0;
			
			if( CheckNil(g_world == nil) ) then return false end
			if( CheckNil(g_world:IsNil()) ) then return false end
			while i < 20 do
				local bAddEffect = true;
				local iSkillRange = kAction:GetParamInt(21);
				local iIndex = kAction:GetParamInt(20);

				local kPos = actor:GetPos();	

				kPos:SetX(kPos:GetX() + math.random(-iSkillRange,iSkillRange));
				kPos:SetY(kPos:GetY() + math.random(-iSkillRange,iSkillRange));
				kPos:SetZ(kPos:GetZ() + 200);

				local ptcl = g_world:ThrowRay(kPos, Point3(0,0,-1),500);
				if ptcl:GetX() == -1 and ptcl:GetY() == -1 and ptcl:GetZ() == -1 then
					--위로도 쏴 본다.
					ptcl = g_world:ThrowRay(kPos, Point3(0,0,1),500);
					if ptcl:GetX() == -1 and ptcl:GetY() == -1 and ptcl:GetZ() == -1 then
						bAddEffect = false;						
					end			
				end

				if true == bAddEffect then
					actor:AttachParticleToPointS(1000 + iIndex, ptcl, "ef_boom_01", math.random(-0.8, 1.5));
					kAction:SetParamInt(20, iIndex + 1);
					i = i + 1;
				end
				
			end
		end
		
		--  그라운드 제로 막타 이펙트
		if "a_groundzero_finish" == kAction:GetID() then
			kAction:SetParamInt(20);
			kAction:SetParamInt(21, 300);			
			local i = 0;
		
			-- 회전축 구하기
			local iGrenadeNum = 8
			local kRotateAxis = Point3(0,0,1)
			local fRotateAngle = (360/iGrenadeNum)*DEGTORAD
			local i = 0;
			local fRadius = 150
			local kCenterPos = actor:GetPos()
			
			while i<iGrenadeNum do
				local iIndex = kAction:GetParamInt(20)
				local kEffectPos = actor:GetLookingDir()
				kEffectPos:Multiply(fRadius)
				kEffectPos:Rotate(kRotateAxis,fRotateAngle*(i))
				kEffectPos:Add(kCenterPos)
				actor:AttachParticleToPointS(1000 + iIndex, kEffectPos, "ef_boom_01", 1.0)
				kAction:SetParamInt(20, iIndex + 1)
				i=i+1
			end
			QuakeCamera(1, 3, 1, 1, 1)
		end
	end
	
	if textKey == "bomb_start" and "a_groundzero_finish" == kAction:GetID() then
		actor:AttachParticle(2004, "char_root", "ef_Ground_Zero_Bomb")
	end
	
	return	true;
end

function Skill_Master_Summon_Finish_OnTargetListModified(actor,action,bIsBefore)
	return Skill_Master_Summon_Common_OnTargetListModified(actor,action,bIsBefore)
end
