
function Skill_Moon_Dance_OnCheckCanEnter(actor,action)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());

	local weapontype = actor:GetEquippedWeaponType();
	
	if weapontype == 0 then
		return	false;
	end

	return		true;
	
end
function Skill_Moon_Dance_OnCastingCompleted(actor, action)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());

	Skill_Moon_Dance_Fire(actor,action);
	action:SetSlot(action:GetCurrentSlot()+1);
	actor:PlayCurrentSlot();

end
function Skill_Moon_Dance_Fire(actor,action)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
	if( CheckNil(g_world == nil) ) then return false end
	if( CheckNil(g_world:IsNil()) ) then return false end
	ODS("RestoreItemModel Skill_Moon_Dance_Fire\n");
	
	action:SetParamFloat(0,g_world:GetAccumTime()); --  시작 시간 기록
	action:CreateActionTargetList(actor);
--[[	
	if LOCALE.NC_JAPAN ~= GetLocale() then
	
		--  마이클 잭슨으로 변신!
		local   kMan = GetStatusEffectMan();
		
		local	iInstanceID = kMan:AddStatusEffectToActor(actor:GetPilotGuid(),"se_transform_to_moon_dance",0,0);
		action:SetParamInt(2,iInstanceID);
	else
		action:SetSlot(2);
		actor:PlayCurrentSlot();
		actor:HideParts(EQUIP_POS_WEAPON, true);
	end
]]	
	action:SetSlot(2);
	actor:PlayCurrentSlot();
	actor:HideParts(EQUIP_POS_WEAPON, true);
		
	action:SetParamFloat(6,actor:GetLookingDir():GetX());
	action:SetParamFloat(7,actor:GetLookingDir():GetY());
	action:SetParamFloat(8,actor:GetLookingDir():GetZ());
	
	actor:AttachParticle(59901,"char_root","ef_Moonwalk_01");
	actor:StopJump();

	local	kSoundID = action:GetScriptParam("HIT_SOUND_ID");
	if kSoundID~="" then
		actor:AttachSound(2784,kSoundID);
	end
		
end
function Skill_Moon_Dance_OnFindTarget(actor,action,kTargets)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
	local  iAttackRange = action:GetSkillRange(0,actor);
	
	--ODS("Skill_WindMill_OnFindTarget iAttackRange:"..iAttackRange.."\n");
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
	
	kParam:SetParam_3(true,FTO_NORMAL);
	local iTargets = action:FindTargets(iFindTargetType,kParam,kTargets,kTargets);
	
	return iTargets;
end

function Skill_Moon_Dance_OnTargetListModified(actor,action,bIsBefore)
	
	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
    ODS("Skill_WindMill_OnTargetListModified\n");
    
    if bIsBefore == false then
        --  때린다.
        SkillHelpFunc_DefaultHitOneTime(actor,action, true);
    end
end

function Skill_Moon_Dance_OnEnter(actor, action)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
--	local actorID = actor:GetID()
	local actionID = action:GetID()
--	local weapontype = actor:GetEquippedWeaponType();
	
	ODS("Skill_Moon_Dance_OnEnter actionID:"..actionID.." GetActionParam:"..action:GetActionParam().."\n");
	
	if action:GetActionParam() == AP_CASTING then	--	캐스팅이라면 그냥 리턴하자.
		return	true;
	end

	Skill_Moon_Dance_OnCastingCompleted(actor,action);
	
	return true
end

function Skill_Moon_Dance_OnUpdate(actor, accumTime, frameTime)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
		
--	local actorID = actor:GetID()
	local action = actor:GetAction()
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
--	local animDone = actor:IsAnimationDone()
--	local nextActionName = action:GetNextActionName()
--	local actionID = action:GetID();

    local   fElasepdTime = accumTime - action:GetParamFloat(0);
    if 	fElasepdTime> 2 then
		action:SetParam(1, "end")
		return false    
    end
    
	local fVelocity = 50;
	
	local kMovingDir = Point3(action:GetParamFloat(6),
	                            action:GetParamFloat(7),
	                            action:GetParamFloat(8));
	                            
	kMovingDir:Multiply(-fVelocity)
	actor:SetMovingDelta(kMovingDir);    

	return true
end

function Skill_Moon_Dance_OnCleanUp(actor)
	
	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
		
    actor:DetachFrom(59901);

	local	kAction = actor:GetAction();
    
	CheckNil(nil==kAction);
	CheckNil(kAction:IsNil());

--[[	if LOCALE.NC_JAPAN ~= GetLocale() then
		--  변신 해제
		local   kMan = GetStatusEffectMan();

		local	iInstanceID = kAction:GetParamInt(2)
		kMan:RemoveStatusEffectFromActor2(actor:GetPilotGuid(),iInstanceID);	
	else
		actor:HideParts(EQUIP_POS_WEAPON, false);
	end
	]]
	actor:HideParts(EQUIP_POS_WEAPON, false);
	return true;
end

function Skill_Moon_Dance_OnLeave(actor, action)

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

