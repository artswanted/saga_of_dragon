-- [WEAPON_XML] : 무기 외형을 바꾸고 싶을 경우 무기 아이템의 XML 경로.
-- [DEATH_BOUND] : 데스바운드일경우
-- [HIT_CASTER_EFFECT_TARGET_NODE] : HIT 키에서 캐스터에게 이펙트를 붙일때, 붙일 노드
-- [HIT_CASTER_EFFECT_ID] : HIT 키에서 캐스터에게 붙일 이펙트 ID

-- Melee
function EarthQuake_OnCheckCanEnter(actor,action)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	local weapontype = actor:GetEquippedWeaponType();
	
	if weapontype == 0 then
		return	false;
	end
	return		true;
	
end
function EarthQuake_OnCastingCompleted(actor, action)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
	EarthQuake_Fire(actor,action);
	action:SetSlot(action:GetCurrentSlot()+1);
	actor:PlayCurrentSlot();
	local	kMovingDir = actor:GetLookingDir();
	local	kTargetPos = actor:GetNodeWorldPos("char_root");
	kTargetPos:Add(kMovingDir);

	if action:GetScriptParam("DEATH_BOUND") ~= "TRUE" then
		actor:AttachParticleToPointWithRotate(8,kTargetPos,"ef_Earthquake_02_char_root", actor:GetRotateQuaternion())
	else		
		actor:AttachParticle(7215, "p_ef_heart", "ef_Deathbound_01_p_ef_heart");	
	end
end
function EarthQuake_Fire(actor,action)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
	--	무기 바꾸기 처리
	if action:GetScriptParam("WEAPON_XML") ~= "" then
		actor:ChangeItemModel(ITEM_WEAPON, action:GetScriptParam("WEAPON_XML"));
	end

	if action:GetScriptParam("DEATH_BOUND") ~= "TRUE" then
		local   kFireEffectID = "ef_Earthquake_01_p_wp_r_hand01";
		local   kFireEffectTargetNodeID = "p_wp_r_hand";

		actor:AttachSound(105,"LandAnger_Start");
    
		if kFireEffectID ~= "" and kFireEffectTargetNodeID ~= "" then
	    
			if kFireEffectTargetNodeID == "__FRONT_ACTOR" then
	            
				local   kDir = actor:GetLookingDir();
				local   fDistance = 30;
				kDir:Multiply(fDistance);
				kDir:Add(actor:GetPos());
				kDir:SetZ(kDir:GetZ()-28);
	            
				actor:AttachParticleToPoint(7212,kDir,kFireEffectID);
	            
			else
				actor:AttachParticle(7212,kFireEffectTargetNodeID,kFireEffectID);
			end
		end	
	else
		actor:AttachSound(105,"DeathBound_Start");
	end

end

function EarthQuake_OnFindTarget(actor,action,kTargets)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
	action:SetParam(0,"");
	return	EarthQuake_FindTargetEx(actor,action,action:GetActionNo(),action:GetSkillRange(0,actor),kTargets);
end

function EarthQuake_FindTargetEx(actor,action,iActionNo,iAttackRange,kTargets)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());	

	local	kMovingDir = actor:GetLookingDir();

	if action:GetScriptParam("DEATH_BOUND") ~= "TRUE" then
		kMovingDir:Multiply(iAttackRange);
	end

	local	kTargetPos = actor:GetNodeWorldPos("char_root");
	kTargetPos:Add(kMovingDir);
	
	local iParam2 = action:GetAbil(AT_1ST_AREA_PARAM2)
	if 0==iParam2 then
		iParam2 = iAttackRange
	end
	
	local kParam = FindTargetParam();
	kParam:SetParam_1(kTargetPos,actor:GetLookingDir());
	kParam:SetParam_2(0,30,iParam2,0);
	kParam:SetParam_3(true,FTO_NORMAL+FTO_DOWN);
	
	return	action:FindTargetsEx(iActionNo,TAT_SPHERE,kParam,kTargets,kTargets);
end

function EarthQuake_OnEnter(actor, action)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
--	local actorID = actor:GetID()
--	local actionID = action:GetID()
--	local weapontype = actor:GetEquippedWeaponType();

	if action:GetActionParam() == AP_CASTING then	--	캐스팅이라면 그냥 리턴하자.
		return	true;
	end

	EarthQuake_OnCastingCompleted(actor,action);
	
	return true
end

function EarthQuake_OnUpdate(actor, accumTime, frameTime)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
--	local actorID = actor:GetID()
	local action = actor:GetAction()
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
	local animDone = actor:IsAnimationDone()
	local nextActionName = action:GetNextActionName()
--	local actionID = action:GetID();
--	local iComboCount = actor:GetComboCount();
	
	if action:GetParam(1) == "end" then
		return	false;
	end
	
	if animDone == true then
		
	
		local curActionParam = action:GetParam(0)
		if curActionParam == "GoNext" then

			nextActionName = action:GetParam(1);

			action:SetParam(0, "null")
			action:SetParam(1, "end")
			
			action:SetNextActionName(nextActionName)
			return false;
		else
			action:SetParam(1, "end")
			return false
		end
	end

	return true
end

function EarthQuake_OnCleanUp(actor,action)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
	--	무기 원래대로
	if actor:GetAction():GetScriptParam("WEAPON_XML") ~= "" then
		actor:RestoreItemModel(ITEM_WEAPON);
	end

	if action:GetParam(1) ~= "end" then
		actor:DetachFrom(7215,true);	
		actor:DetachFrom(7212,true);
	end

	actor:DetachFrom(8);
	actor:DetachFrom(7212);	

	return true;
end

function EarthQuake_OnLeave(actor, action)

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
	
	if action:GetEnable() == false then
		
		if curParam==actionID then
			curAction:SetParam(0,"");
		end
		
	end	
	
	if action:GetEnable() == true then
	

		curAction:SetParam(0,"GoNext");
		curAction:SetParam(1,actionID);

		return false;

	end

	return false 
end


function EarthQuake_OnTargetListModified(actor,action,bIsBefore)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
    if bIsBefore == false then

        if action:GetParam(2) == "HIT" then
        
            --  hit 키 이후라면
            --  바로 대미지 적용시켜준다.
            SkillHelpFunc_DefaultHitOneTime(actor,action, false);
        end        
    
    end
    
    
end

function EarthQuake_OnEvent(actor,textKey)
	
	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	local kAction = actor:GetAction();
	
	CheckNil(nil==kAction);
	CheckNil(kAction:IsNil());
	
	if kAction:GetActionParam() == AP_CASTING then
		return true;
	end	
	
	if textKey == "hit" then
		
	    kAction:SetParam(2,"HIT");
	
		if kAction:GetScriptParam("DEATH_BOUND") == "TRUE" then
--			local	fDistance = kAction:GetSkillRange(0,actor);
			local	kMovingDir = actor:GetLookingDir();
			local	kTargetPos = actor:GetNodeWorldPos("char_root");
			kTargetPos:Add(kMovingDir);
			actor:AttachParticleToPointWithRotate(7214,kTargetPos,"ef_Deathbound_01_char_root", actor:GetRotateQuaternion());
			actor:AttachSound(106,"DeathBound");
		else
			actor:AttachSound(106,"LandAnger");
		end

	    if actor:IsMyActor() then
    	    kAction:CreateActionTargetList(actor);   --  타겟 잡기
	    
	        if IsSingleMode() then
	            EarthQuake_OnTargetListModified(actor,kAction,false)
	        else
	            kAction:BroadCastTargetListModify(actor:GetPilot());
	        end
	    
	    end
				
		if actor:IsMyActor() then
			if kAction:GetScriptParam("DEATH_BOUND") ~= "TRUE" then
				QuakeCamera(g_fMeleeDropQuakeValue[1]
				, g_fMeleeDropQuakeValue[2]
				, g_fMeleeDropQuakeValue[3]
				, g_fMeleeDropQuakeValue[4]
				, g_fMeleeDropQuakeValue[5])
			else
				QuakeCamera(0.5, 1.5, 1, 1, 1)
			end
		end		
		
	end

	return	true;
end
