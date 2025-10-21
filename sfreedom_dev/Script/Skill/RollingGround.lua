
-- Melee
function Skill_RollingGround_OnCheckCanEnter(actor,action)
	CheckNil(nil==actor);
	CheckNil(actor:IsNil());

	local weapontype = actor:GetEquippedWeaponType();
	
	if weapontype == 0 then
		return	false;
	end

	return true;	
end

function Skill_RollingGround_OnCastingCompleted(actor, action)
	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());	

	Skill_RollingGround_Fire(actor,action);
	action:SetSlot(action:GetCurrentSlot()+1);
	actor:PlayCurrentSlot();
end

function Skill_RollingGround_Fire(actor,action)
	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
	if( CheckNil(g_world == nil) ) then return false end
	if( CheckNil(g_world:IsNil()) ) then return false end
	action:SetParamFloat(2, g_world:GetAccumTime()); --  시작 시간
	action:SetParamFloat(6,0);	--  이동 거리
	action:SetParamInt(9,0);	--	Hit Count
	action:SetParamInt(30, action:GetAbil(AT_COUNT))	--	Max Count
	action:SetParamAsPoint(11, actor:GetLookingDir()); -- 바라보는 방향
	action:SetParamAsPoint(12, actor:GetPos()); -- 이동 시작 위치
	actor:AttachSound(99, "RollingGround");
	
	if actor:IsMyActor() == true then
		--actor:AttachParticle(29913, "char_root", "ef_skill_rollingground_02_char_root");
	end
end

function Skill_RollingGround_OnEnter(actor, action)
	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
--	local actorID = actor:GetID()
	local actionID = action:GetID()
--	local weapontype = actor:GetEquippedWeaponType();
	
	if action:GetActionParam() == AP_CASTING then
	--	캐스팅중이다
		return	true;
	end

	Skill_RollingGround_OnCastingCompleted(actor,action);

	return true
end

function Skill_RollingGround_OnUpdate(actor, accumTime, frameTime)
	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	local action = actor:GetAction()
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
	local bAnimDone = actor:IsAnimationDone();
	local iSlot = action:GetCurrentSlot();
	local nextActionName = action:GetNextActionName();

	--ODS("Skill_NineteenNinety_OnUpdate NextActionName : "..nextActionName.." slot :" ..iSlot .. "\n");

	if iSlot == 1 then
	    if bAnimDone then
			action:SetSlot(action:GetCurrentSlot()+1);
	        actor:PlayCurrentSlot();
			if actor:IsMyActor() then
				-- 타격은 타이머로 한다. 총 몇초동안 몇초 간격으로 때리는가.
				local fLimitDist = action:GetAbil(AT_MOVE_RANGE);
				local fSpeed = action:GetAbil(AT_MOVESPEED);
				local fMoveTime = fLimitDist / fSpeed;
				action:StartTimer(fMoveTime,fMoveTime / action:GetParamInt(30),0);
			end
	    end    
    elseif iSlot == 2 then	
		actor:AttachParticle(29912, "char_root", "ef_skill_rollingground_01_char_root");
		--actor:AttachParticle(29914, "char_root", "ef_skill_rollingground_03_char_root");
		actor:AttachParticleToPointWithRotate(29914, actor:GetNodeWorldPos("char_root"), "ef_skill_rollingground_03_char_root", actor:GetRotateQuaternion());
		local StartPos = action:GetParamAsPoint(12);
		local NowPos = actor:GetPos();
		StartPos:SetZ(0);
		NowPos:SetZ(0);
		
		-- 현제 위치와 지난 시간
		local fNowDist = NowPos:Distance(StartPos);
		local fElapsedTime = accumTime - action:GetParamFloat(2);
		
		-- 최종 거리, 스피드, 총 진행 시간.
		local fLimitDist = action:GetAbil(AT_MOVE_RANGE);
		local fSpeed = action:GetAbil(AT_MOVESPEED);
		local fMoveTime = fLimitDist / fSpeed;

		-- 아직 목적지 도착 안했다.
		if ( fNowDist < fLimitDist ) then
			-- 현제 있어야 하는 위치
			local fMoveDist = fLimitDist / fMoveTime * fElapsedTime;
			if fMoveDist > fLimitDist then
				fMoveDist = fLimitDist - fNowDist -- 최종 거리는 벗어 날 수 없다.
			else
				fMoveDist = fMoveDist - fNowDist -- 현제 위치에서 다음 위치사이에 간격.
			end
			local kMovingDir = action:GetParamAsPoint(11)
			kMovingDir:Multiply(fMoveDist);
			actor:SetMovingDelta(kMovingDir);
		end
		
		-- 시간 다됬다. 다음 액션으로 변경.
		if fElapsedTime > fMoveTime then
			actor:DetachFrom(29912);
			actor:DetachFrom(29914);
			action:SetSlot(action:GetCurrentSlot()+1);
			actor:PlayCurrentSlot();
		end
    else
        if bAnimDone then        
            action:SetParam(1,"end");
            return false;        
        end    
    end	

	return true
end

function Skill_RollingGround_OnCleanUp(actor)
	CheckNil(nil==actor);
	CheckNil(actor:IsNil());

	actor:DetachFrom(29912);
	actor:DetachFrom(29914);
	--actor:DetachFrom(29913);
	return true;
end

function Skill_RollingGround_OnLeave(actor, action)
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
	
	if actionID == "a_jump" then 
		return false;
	end
	
	if action:GetActionType()=="EFFECT" then
		
		return true;
	end
	
	if curParam == "end" then 
		return true;
	end

	return false 
end

function Skill_RollingGround_OnEvent(actor,textKey)
	return true;
end

function Skill_RollingGround_OnTargetListModified(actor,action,bIsBefore)
	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
    if bIsBefore == false then
        --  때린다.
        SkillHelpFunc_DefaultHitOneTime(actor,action, false);
    end
end


function Skill_RollingGround_OnTimer(actor,fAccumTime,action,iTimerID)
	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
	--ODS("Skill_RollingGround fAccumTime:"..fAccumTime.."\n");

	local	iHitCount = action:GetParamInt(9);
	local	iTotalHit = action:GetParamInt(30);	
	if iHitCount == iTotalHit then
		return true
	end

	action:CreateActionTargetList(actor);
	
	local	kTargetList = action:GetTargetList();
	local	iTargetCount = kTargetList:size();
	if 0 < iTargetCount then
		action:BroadCastTargetListModify(actor:GetPilot());
	end
	
	iHitCount = iHitCount + 1;
	action:SetParamInt(9,iHitCount);
	
	return	true	
end