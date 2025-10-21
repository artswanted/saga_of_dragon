------------------------------------------------
-- Definitions
------------------------------------------------
-- Param
-- 10[int] : 종료 체크
-- 11[float] : 시간
-- 12[int] : 이동량
-- Param Point
-- 101 : 시전자 초기 위치

function Skill_Striker_SelfBomber2_OnCastingCompleted(actor,action)
end

function Skill_Striker_SelfBomber2_OnEnter(actor, action)
	
	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
	action:SetParamInt(10,0)
	if( CheckNil(g_world == nil) ) then return false end
	if( CheckNil(g_world:IsNil()) ) then return false end
	action:SetParamFloat(11,g_world:GetAccumTime());
	action:SetParamInt(12,action:GetAbil(AT_DISTANCE));
	action:SetParamAsPoint(101,actor:GetPos());

	actor:StopJump();

	local kSoundID = action:GetScriptParam("FIRE_SOUND_ID");
	if kSoundID~="" then
		actor:AttachSound(2783,kSoundID);
	end	
    return true
end

function Skill_Striker_SelfBomber2_Fire(actor,action)
end

function Skill_Striker_SelfBomber2_OnUpdate(actor, accumTime, frameTime)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());

	local	animDone = actor:IsAnimationDone();
	local	action = actor:GetAction()
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
	local	curSlot = action:GetCurrentSlot();
	local	iState = action:GetParamInt(10);
	
	if iState == 0 then
		if animDone == true then
			action:SetParamInt(10,1);
		else
			local distance = action:GetParamInt(12);
			local kMovingVec = actor:GetLookingDir();
			kMovingVec:Multiply(-distance);
			actor:SetMovingDelta(kMovingVec);
		end
	else
		return false;
	end

	return true;
end

function Skill_Striker_SelfBomber2_OnCleanUp(actor)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());

	local kAction = actor:GetAction()
	
	CheckNil(nil==kAction);
	CheckNil(kAction:IsNil());
	return true;
end


function Skill_Striker_SelfBomber2_OnLeave(actor, action)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
	local curAction = actor:GetAction();
	
	CheckNil(nil==curAction);
	CheckNil(curAction:IsNil());
	

	local	curAction = actor:GetAction();
	if curAction:GetParamInt(10) == 1 then
		return true
	end

	return false
end

function Skill_Striker_SelfBomber2_OnEvent(actor,textKey,seqID)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());

	local action = actor:GetAction()
			
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
	return	true;
end

function Skill_Striker_SelfBomber2_OnTargetListModified(actor,action,bIsBefore)
end


