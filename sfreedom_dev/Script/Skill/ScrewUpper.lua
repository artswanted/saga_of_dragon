
-- Melee
function Skill_ScrewUpper_OnCheckCanEnter(actor,action)
	
	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
	local weapontype = actor:GetEquippedWeaponType();
	
	if weapontype == 0 then
		return	false;
	end

	return		true;
	
end
function Skill_ScrewUpper_OnCastingCompleted(actor, action)
	
	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
	Skill_ScrewUpper_Fire(actor,action);
	action:SetSlot(action:GetCurrentSlot()+1);
	actor:PlayCurrentSlot();

	local	kTargetPos = actor:GetNodeWorldPos("char_root");
	actor:AttachParticleToPointWithRotate(8,kTargetPos,"ef_ScrewUpper_01_char_root", actor:GetRotateQuaternion())

end


function Skill_ScrewUpper_Fire(actor,action)
	
	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
	ODS("RestoreItemModel Skill_ScrewUpper_Fire\n");
	
	if( CheckNil(g_world == nil) ) then return false end
	if( CheckNil(g_world:IsNil()) ) then return false end
	action:SetParamFloat(2,g_world:GetAccumTime()); --  시작 시간
	action:SetParamFloat(6,0);
	action:SetParamInt(9,0)	--	Hit Count

	actor:FreeMove(true);

end
function Skill_ScrewUpper_OnFindTarget(actor,action,kTargets)
	
	CheckNil(nil==actor);	
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
	local  iAttackRange = action:GetSkillRange(0,actor);
	
	ODS("Skill_ScrewUpper_OnFindTarget iAttackRange:"..iAttackRange.."\n");
	if iAttackRange == 0 then
	    iAttackRange = 100
	end

	local	kPos = actor:GetPos();
	local	kDir = actor:GetLookingDir();
	local	fBackDistance = 10;
	kDir:Multiply(-fBackDistance);
	kPos:Add(kDir);

	local kParam = FindTargetParam();
	
    local iFindTargetType = TAT_SPHERE;
	kParam:SetParam_1(actor:GetPos(),actor:GetLookingDir());
	kParam:SetParam_2(0,0,iAttackRange,0);
	
	kParam:SetParam_3(true,action:GetAbil(AT_ATTACK_UNIT_POS));
	local iTargets = action:FindTargets(iFindTargetType,kParam,kTargets,kTargets);
	
	return iTargets;
end

function Skill_ScrewUpper_OnEnter(actor, action)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
		
	SkillFunc_InitUseDmgEffect(action);
--	local actorID = actor:GetID()
	local actionID = action:GetID()
--	local weapontype = actor:GetEquippedWeaponType();
	
	ODS("Skill_ScrewUpper_OnEnter actionID:"..actionID.." GetActionParam:"..action:GetActionParam().."\n");
	
	if action:GetActionParam() == AP_CASTING then	--	캐스팅이라면 그냥 리턴하자.
		return	true;
	end

	Skill_ScrewUpper_OnCastingCompleted(actor,action);
	
	return true
end
function Skill_ScrewUpper_OnTargetListModified(actor,action,bIsBefore)
	
	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
    if bIsBefore == false then	
        Skill_ScrewUpper_HitOneTime(actor,action);    
    end

end

function Skill_ScrewUpper_OnTimer(actor,fAccumTime,action,iTimerID)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
	ODS("Skill_ScrewUpper_OnTimer fAccumTime:"..fAccumTime.."\n");

	action:CreateActionTargetList(actor);
	action:BroadCastTargetListModify(actor:GetPilot());
	
	if actor:IsMyActor() then
		if( CheckNil(g_world == nil) ) then return false end
		if( CheckNil(g_world:IsNil()) ) then return false end
		action:SetParamFloat(10,g_world:GetAccumTime());
		local	iHitCount = action:GetParamInt(9);
		
		ODS("Skill_ScrewUpper_HitOneTime iHitCount:"..iHitCount.."\n");
		
		--local	iSkillLevel = action:GetAbil(AT_LEVEL);
		--	if 0 == iSkillLevel then
		--	iSkillLevel = 1;
		--end

		local	iTotalHit = 5;
		
		if iHitCount == iTotalHit then
			return true
		end		
		iHitCount = iHitCount + 1;
		action:SetParamInt(9,iHitCount);
	
	end
	
	return	true	

end

function Skill_ScrewUpper_OnUpdate(actor, accumTime, frameTime)
	
	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	local actorID = actor:GetID()
	local action = actor:GetAction()
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
	local actionID = action:GetID();
	local movingSpeed = 0
	local bAnimDone = actor:IsAnimationDone();
	local iSlot = action:GetCurrentSlot();

    if iSlot == 1 then

	    if bAnimDone then
	        actor:PlayNext();
	        action:SetParamFloat(2,accumTime);

			if actor:IsMyActor() then
				action:StartTimer(1.0,0.15,0);
			end
	    end
    
    elseif iSlot == 2 then
		
		--local	iSkillLevel = action:GetAbil(AT_LEVEL);
		--	if 0 == iSkillLevel then
		--	iSkillLevel = 1;
		--end

		local	iTotalHit = 5;

		if action:GetParamInt(9) < iTotalHit then
			ODS("\n---------- Cur Hit "..action:GetParamInt(9).."     Total "..iTotalHit.."    ---------\n")
		
			actor:IncRotate(frameTime*8*360.0*3.141592/180.0);
			actor:SetMovingDelta(Point3(0,0,80));
		else
			action:SetParam(1,"end");
			return false;
		end
	end
	
	return true
end

function Skill_ScrewUpper_OnCleanUp(actor)
	
	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	actor:DetachFrom(19002);
	actor:FreeMove(false);

	return true;
end

function Skill_ScrewUpper_OnLeave(actor, action)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
	local curAction = actor:GetAction();
	
	CheckNil(nil==curAction);
	CheckNil(curAction:IsNil());
	
	local curParam = curAction:GetParam(1)
	local actionID = action:GetID()
	
	if actor:IsMyActor() == false then
		return true;
	end
	
	if actionID == "a_jump" then 
		if curParam == "end" then 
			action:SetSlot(2);
			action:SetDoNotBroadCast(true);
			return true;
		end
		return	false;
	end
	
	
	if action:GetActionType()=="EFFECT" then
		
		return true;
	end
	
	if curParam == "end" then 
		return true;
	end

	return false 
end

function Skill_ScrewUpper_OnEvent(actor,textKey)
	
	CheckNil(nil==actor);
	CheckNil(actor:IsNil());

	local kAction = actor:GetAction();
	
	CheckNil(nil==kAction);
	CheckNil(kAction:IsNil());
	
	if kAction:GetActionParam() == AP_CASTING then
		return true;
	end

	return	true;
end

function Skill_ScrewUpper_HitOneTime(actor,action)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());	
		
	SkillHelpFunc_DefaultHitOneTime(actor,action, true);
		
end