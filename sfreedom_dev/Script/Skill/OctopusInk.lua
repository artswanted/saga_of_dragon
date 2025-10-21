-- [HIT_CASTER_EFFECT_TARGET_NODE] : HIT 키에서 캐스터에게 이펙트를 붙일때, 붙일 노드
-- [HIT_CASTER_EFFECT_ID] : HIT 키에서 캐스터에게 붙일 이펙트 ID


-- Melee
function Skill_OctopusInk_OnCheckCanEnter(actor,action)
	
	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	local weapontype = actor:GetEquippedWeaponType();
	
	if weapontype == 0 then
		return	false;
	end
	return		true;
	
end
function Skill_OctopusInk_OnCastingCompleted(actor, action)
	
	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
	Skill_OctopusInk_Fire(actor,action);
	action:SetSlot(action:GetCurrentSlot()+1);
	actor:PlayCurrentSlot();

end

function Skill_OctopusInk_Fire(actor,action)
end

function Skill_OctopusInk_OnTargetListModified(actor,action,bIsBefore)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
    if bIsBefore == false then
    
        --  타격 이펙트
		SkillHelpFunc_DefaultHitOneTime(actor,action, true);        
        action:SetParam(1,"fire");
        
    end

end

function Skill_OctopusInk_OnFindTarget(actor,action,kTargets)
	
	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
	local  iAttackRange = action:GetSkillRange(0,actor);
	if iAttackRange == 0 then
	    iAttackRange = 100
	end
	
	local	kPos = actor:GetPos();
	local	kDir = actor:GetLookingDir();
	local	fBackDistance = 10;
	kDir:Multiply(-fBackDistance);
	kPos:Add(kDir);

	local kParam = FindTargetParam();
	
	local iParam2 = action:GetAbil(AT_1ST_AREA_PARAM2)
	if 0==iParam2 then
		iParam2 = 30
	end	
	
	kParam:SetParam_1(kPos,actor:GetLookingDir());
	kParam:SetParam_2(iAttackRange,iParam2,0,0);
	kParam:SetParam_3(true,FTO_NORMAL);
	local iTargets = action:FindTargets(TAT_RAY_DIR,kParam,kTargets,kTargets);
	
	return iTargets;
end

function Skill_OctopusInk_OnEnter(actor, action)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
--	local actorID = actor:GetID()
	local actionID = action:GetID()
--	local weapontype = actor:GetEquippedWeaponType();
	
	ODS("Skill_OctopusInk_OnEnter actionID:"..actionID.." GetActionParam:"..action:GetActionParam().."\n");

	if action:GetActionParam() == AP_CASTING then	--	캐스팅이라면 그냥 리턴하자.
		return	true;
	end

	Skill_OctopusInk_OnCastingCompleted(actor,action);
	
	return true
end

function Skill_OctopusInk_OnUpdate(actor, accumTime, frameTime)
	
	CheckNil(nil==actor);
	CheckNil(actor:IsNil());

--	local actorID = actor:GetID()
	local action = actor:GetAction()
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
	local animDone = actor:IsAnimationDone()
--	local nextActionName = action:GetNextActionName()
--	local actionID = action:GetID();
	
	if action:GetParam(1) == "wait" and animDone then
	
	    action:SetParam(1,"end");
	    return  false
	end
	if action:GetParam(1) == "fire" and accumTime - action:GetParamFloat(3)>2 then
	
	    action:SetParam(1,"wait");
	    actor:PlayNext();
	    
	    return  true;
	end
	
	
	if actor:IsMyActor() then
	
	    if action:GetParamFloat(2)>0 then
    	
	        local   fElapsedTime = accumTime - action:GetParamFloat(2);
	        if fElapsedTime>0.5 then
    	    
    	        action:CreateActionTargetList(actor);
                
                if IsSingleMode() == false and action:GetTargetCount()>0 then
                    action:BroadCastTargetListModify(actor:GetPilot());    -- 서버로 브로드캐스팅
                    action:ClearTargetList();
                else
                    Skill_OctopusInk_OnTargetListModified(actor,action,false);
                end    	 
                
                action:SetParamFloat(2,0);   
                
	        end
    	
	    end
	
	end
	
	

	return true
end

function Skill_OctopusInk_OnCleanUp(actor)
    
	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
    actor:DetachFrom(7213);
	return true;
end

function Skill_OctopusInk_OnLeave(actor, action)

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

function Skill_OctopusInk_OnEvent(actor,textKey)
	
	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	local kAction = actor:GetAction();
	
	CheckNil(nil==kAction);
	CheckNil(kAction:IsNil());
	
	if kAction:GetActionParam() == AP_CASTING then
		return true;
	end
	
	if textKey == "hit" or textKey == "fire" then
	    
	    local   kHitEffectID = kAction:GetScriptParam("HIT_CASTER_EFFECT_ID");
	    local   kHitEffectTargetNodeID = kAction:GetScriptParam("HIT_CASTER_EFFECT_TARGET_NODE");
	    
	    if kHitEffectID ~= "" and kHitEffectTargetNodeID ~= "" then
	        --actor:AttachParticle(7213,kHitEffectTargetNodeID,kHitEffectID);			
			actor:AttachParticleToPointWithRotate(7213,actor:GetNodeWorldPos("char_root"), kHitEffectID, actor:GetRotateQuaternion());	
	    end

		local	kSoundID = kAction:GetScriptParam("HIT_SOUND_ID");
		if kSoundID~="" then
			actor:AttachSound(2784,kSoundID);
		end
		if( CheckNil(g_world == nil) ) then return false end
		if( CheckNil(g_world:IsNil()) ) then return false end
	    
	    kAction:SetParamFloat(2,g_world:GetAccumTime());
        kAction:SetParamFloat(3,g_world:GetAccumTime());
	    
	end

	return	true;
end
