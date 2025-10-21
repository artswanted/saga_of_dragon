
function Skill_Santuary_Play_OnEnter(actor, action)
	
	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
--	local actorID = actor:GetID()
--	local actionID = action:GetID()

	action:SetDoNotBroadCast(true)
	
	--	시작 시간 기록
	if( CheckNil(g_world == nil) ) then return false end
	if( CheckNil(g_world:IsNil()) ) then return false end
	action:SetParamFloat(0,g_world:GetAccumTime());
	action:SetParamFloat(1,g_world:GetAccumTime());	
	action:SetParamFloat(2,g_world:GetAccumTime());
	
	-- Looking Dir
	local kLookDir = actor:GetLookingDir();
	action:SetParamFloat( 4, kLookDir:GetX() );
	action:SetParamFloat( 5, kLookDir:GetY() );
	action:SetParamFloat( 6, kLookDir:GetZ() );
	
	action:SetSlot(0);
	actor:PlayCurrentSlot();
	action:SetParamInt(3, 0);
	action:SetParamFloat(8, 70);
	
	Skill_Santuary_Play_OnCastingCompleted(actor, action)
	actor:FreeMove(true)
	return true
end

function Skill_Santuary_Play_OnFindTarget(actor,action,kTargets)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
    local   kParam = FindTargetParam();

	kParam:SetParam_1(actor:GetPos(),actor:GetLookingDir());
	kParam:SetParam_2(0,0,50,1);
	kParam:SetParam_3(true,FTO_NORMAL);
	
	return action:FindTargets(TAT_SPHERE,kParam,kTargets,kTargets);
end

function Skill_Santuary_Play_OnTargetListModified(actor,action,bIsBefore)
	
	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
    if bIsBefore == false then
        SkillHelpFunc_DefaultHitOneTime(actor,action, true);
    end

end

function Skill_Santuary_Play_OnUpdate(actor, accumTime, frameTime)
	
	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
		
--	local actorID = actor:GetID()
	local action = actor:GetAction()
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
	local animDone = actor:IsAnimationDone()
	local iCurrentSlot = action:GetCurrentSlot();
	local iState = action:GetParamInt(3);
	
	local   kMoveDir = Point3(action:GetParamFloat(4),action:GetParamFloat(5),action:GetParamFloat(6));
	local   fMoveSpeed = actor:GetAbil(AT_C_MOVESPEED);
	if fMoveSpeed == 0 then
	    fMoveSpeed = action:GetParamFloat(8);
	end
	
	if iState == 0 and iCurrentSlot == 0 then
		if animDone == true then
			action:SetSlot(1);
			actor:PlayCurrentSlot();
		end
	end

	local   fLifeTime = actor:GetAbil(AT_LIFETIME)/1000.0;
	if fLifeTime == 0 then
	    fLifeTime = 2.0;
    end	

    local   fTotalElapsedTime = accumTime - action:GetParamFloat(0);
	if iState == 0 and iCurrentSlot == 1 then --마지막에 자연스럽게 알파로 사라지도록 처리
		local DieAnimTime = actor:GetAnimationLength(action:GetSlotAnimName(2))
		if 0 ~= DieAnimTime and fLifeTime - fTotalElapsedTime < DieAnimTime then--다이 애니 있으면 다이애니로 바뀌도록
			action:SetSlot(2);
			actor:PlayCurrentSlot();
			action:SetParamInt(3, 1);
		elseif fLifeTime - fTotalElapsedTime < 1.0 then
			action:SetParamInt(3, 1);
			actor:SetTargetAlpha(actor:GetAlpha(),0, 0.5);
			actor:SetHideShadow(true);
		end
	end
 
	if animDone == true 
		and 2 == iCurrentSlot
		and actor:GetAnimationLength(action:GetSlotAnimName(2)) ~= 0 then
		return  false
	end
	if fTotalElapsedTime > fLifeTime then
		return  false
	end

	return true
end

function Skill_Santuary_Play_OnCleanUp(actor, action)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
--	if actor:IsUnderMyControl() then
--		Net_C_M_REQ_DELETE_ENTITY(actor:GetPilotGuid());
--	end
	
	local	kSoundID = nil
	kSoundID = action:GetScriptParam("FIRE_SOUND_ID");
	if kSoundID~="" then
		StopSoundByID(kSoundID)
	end
	
	if( CheckNil(g_world == nil) ) then return true end
	if( CheckNil(g_world:IsNil()) ) then return true end
	g_world:RemoveActorOnNextUpdate(actor:GetPilotGuid());
end

function Skill_Santuary_Play_OnLeave(actor, action)
	return true;
end

function Skill_Santuary_Play_OnCastingCompleted(actor, action)
	local	kSoundID = nil
	kSoundID = action:GetScriptParam("FIRE_SOUND_ID");
	if kSoundID~="" then
		actor:AttachSound(2784, kSoundID);
	end
end