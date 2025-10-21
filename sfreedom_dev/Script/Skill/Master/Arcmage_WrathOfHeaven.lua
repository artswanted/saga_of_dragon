------------------------------------------------
-- Definitions
------------------------------------------------
-- Param
-- 10[int] : 종료 체크
-- 11[float] : 시간
-- 12[int] : timer count
-- 13[int] : hit 이펙트는 한번만 보인다.

function Skill_Arcmage_WrathOfHeaven_OnCheckCanEnter(actor,action)
	
	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
		
	local weapontype = actor:GetEquippedWeaponType();
	
	if weapontype == 0 then
		return	false;
	end

	return true;

end

function Skill_Arcmage_WrathOfHeaven_OnCastingCompleted(actor,action)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
	actor:CutSkillCasting();
end

function Skill_Arcmage_WrathOfHeaven_OnEnter(actor, action)
	
	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
	if actor:IsMyActor() == true then
		if actor:IsMeetFloor() == false then		-- 공중에서는 사용할 수 없다.
			return	false;
		end
	end
	
	action:SetParamInt(10,0);
	if( CheckNil(g_world == nil) ) then return false end
	if( CheckNil(g_world:IsNil()) ) then return false end
	action:SetParamFloat(11,g_world:GetAccumTime());
	action:SetParamInt(13,0);
	
	Skill_Arcmage_WrathOfHeaven_Fire(actor,action)
		
	if actor:IsMyActor() then
	    GetComboAdvisor():OnNewActionEnter(action:GetID());
    end
    
	local effectID = action:GetScriptParam("CASTING_EFFECT_EFFECT_ID");
	local effectNode = action:GetScriptParam("CASTING_EFFECT_TARGET_NODE");
	if effectID ~= "" and effectNode ~= "" then
		actor:AttachParticle(201, effectNode, effectID);
	end
	
	local kSoundID = action:GetScriptParam("FIRE_SOUND_ID");
	if kSoundID~="" then
		actor:AttachSound(2783,kSoundID);
	end	

	if actor:IsMyActor() then
		g_world:SetShowWorldFocusFilterColorAlpha(0x000000, 0.0, 0.7, 0.4,true,true);
	end
	
	
	local iTotalHit = 0;
	local kSkillDef = GetSkillDef(action:GetActionNo());
	if kSkillDef:IsNil() == false then
		iTotalHit  = kSkillDef:GetAbil(AT_COUNT);
	end
	action:SetParamInt(12,iTotalHit);
--	ODS("iTotalHit:"..iTotalHit.."\n",false, 912)
   return true
end

function Skill_Arcmage_WrathOfHeaven_Fire(actor,action)
		
	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
	local kTargetPos = actor:GetPos();
	kTargetPos:SetZ(kTargetPos:GetZ() + 30);
	Skill_Arcmage_WrathOfHeaven_CreateElemental(actor,action,kTargetPos)
end

function Skill_Arcmage_WrathOfHeaven_OnUpdate(actor, accumTime, frameTime)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());

	local	animDone = actor:IsAnimationDone();
	local	action = actor:GetAction()
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());

	local	iState = action:GetParamInt(10);
	if iState == 0 then
		if animDone == true then
			action:SetParamInt(10,1)
		end
	else
		return false
	end

	return true
end

function Skill_Arcmage_WrathOfHeaven_OnCleanUp(actor)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());

	actor:DetachFrom(201);

	if actor:IsMyActor() then
		if( CheckNil(g_world == nil) ) then return true end
		if( CheckNil(g_world:IsNil()) ) then return true end
		g_world:SetShowWorldFocusFilterColorAlpha(0x000000, g_world:GetWorldFocusFilterAlpha(), 0, 1, false,false);
	end	
	
	return true;
end


function Skill_Arcmage_WrathOfHeaven_OnLeave(actor, action)
	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
	if action:GetEnable() == false then
		return	false;
	end
	
	if action:GetActionType()=="EFFECT" then
		return true;
	end	
	
	local	curAction = actor:GetAction();
	if curAction:GetParamInt(10) == 1 then
		return true
	end

	return false
end

function Skill_Arcmage_WrathOfHeaven_OnEvent(actor,textKey,seqID)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
		
	local action = actor:GetAction();
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
	if textKey == "hit" then
		if actor:IsMyActor() == true then
			action:StartTimer(1,0.15,0)			
			-- action:CreateActionTargetList(actor);
			-- action:BroadCastTargetListModify(actor:GetPilot());
			-- action:ClearTargetList();
		end
		local kEffectID = action:GetScriptParam("HIT_TARGET_EFFECT_ID");
		if kEffectID ~= "" then
			local effectPos = actor:GetPos();
			effectPos:SetZ(effectPos:GetZ() - 30);
			local lookAt = actor:GetLookingDir();
			lookAt:Multiply(100);
			effectPos:Add(lookAt);
			actor:AttachParticleToPoint(210, effectPos, kEffectID);
		end
	end
	return	true;
end

function Skill_Arcmage_WrathOfHeaven_OnTargetListModified(actor,action,bIsBefore)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	CheckNil(action:IsNil());
	
	if bIsBefore == false then
		Skill_Arcmage_WrathOfHeaven_HitOneTime(actor,action)
    end

end

function Skill_Arcmage_WrathOfHeaven_CreateElemental(actor,action,kTargetPos)
	
	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
	local guid = GUID("123")
	guid:Generate()
	
	local pilot = g_pilotMan:NewPilot(guid, 1000705, 0)
	if pilot:IsNil() == false then
		local kNewActor = pilot:GetActor();
		if( CheckNil(g_world == nil) ) then return false end
		if( CheckNil(g_world:IsNil()) ) then return false end
		g_world:AddActor(guid, kNewActor, kTargetPos, 9);
		kNewActor:FreeMove(true);
		kNewActor:SetTranslate(kTargetPos);
		kNewActor:ReserveTransitAction("a_WrathOfHeavenSA");
		kNewActor:ClearActionState()
		local	kLookAt = actor:GetLookingDir()
		kNewActor:LookAt(actor:GetTranslate():_Add(kLookAt), true)	--캐릭터랑 같은 방향으로
		kNewActor:PlayCurrentSlot();
	end
end

function Skill_Arcmage_WrathOfHeaven_OnTimer(actor, accumTime, action,iTimerID)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
	local count = action:GetParamInt(12);
	
	if count == 0 then
		return true
	end

	action:SetParamInt(12, count-1);
	
	action:CreateActionTargetList(actor);
	action:BroadCastTargetListModify(actor:GetPilot());
	action:ClearTargetList();
	
	return true;
end

function Skill_Arcmage_WrathOfHeaven_HitOneTime(actor,action)
	
	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
	local	iHitCount = action:GetParamInt(2);
		local	iTotalHit = action:GetParamInt(12)
	if iTotalHit == nil or iTotalHit == 0 then
	    iTotalHit = 1;
	end
--	ODS("히트시 iTotalHit"..iTotalHit.."iHitCount:"..iHitCount.."\n",false, 912)
	
	if iHitCount == iTotalHit then
--		ODS("리턴됨\n", false,912)
		return
	end
	
	if iHitCount == 0 then
	
	    local   kHitEffectID = action:GetScriptParam("HIT_CASTER_EFFECT_ID");
	    local   kHitEffectTargetNodeID = action:GetScriptParam("HIT_CASTER_EFFECT_TARGET_NODE");
		local   kHitEffectTargetNodeIDToPos = action:GetScriptParam("HIT_CASTER_EFFECT_TARGET_NODE_TO_WORLD_POS");
		local   kHitEffectTargetNodeIDToPosWithRotate = action:GetScriptParam("HIT_CASTER_EFFECT_TARGET_NODE_TO_WORLD_POS_WITH_ROTATE");

	    if kHitEffectID ~= "" and kHitEffectTargetNodeID ~= "" or kHitEffectTargetNodeIDToPos ~= ""  or kHitEffectTargetNodeIDToPosWithRotate ~= "" then
			if kHitEffectID ~= "" and kHitEffectTargetNodeID ~= "" then
				actor:AttachParticle(7214,kHitEffectTargetNodeID,kHitEffectID);
			elseif kHitEffectTargetNodeID ~= "" then
	            actor:AttachParticle(7214,kHitEffectTargetNodeID,kHitEffectID);
			elseif kHitEffectTargetNodeIDToPos ~= "" then
				actor:AttachParticleToPoint(7214,actor:GetNodeWorldPos(kHitEffectTargetNodeIDToPos),kHitEffectID);			
			else
			    actor:AttachParticleToPointWithRotate(7214,actor:GetNodeWorldPos(kHitEffectTargetNodeIDToPosWithRotate), kHitEffectID, actor:GetRotateQuaternion())
		    end
		end
	end

	
	local	kSoundID = action:GetScriptParam("HIT_SOUND_ID");
	if kSoundID~="" then
		actor:AttachSound(2784,kSoundID);
	end	
	
	--남성용 사운드
	if 1 == actor:GetAbil(AT_GENDER) then
		local	kSoundID2 = action:GetScriptParam("HIT_SOUND_ID_MALE");
		if kSoundID2~="" then
			actor:AttachSound(2785,kSoundID2);
		end	
	--여성용 사운드
	else
		local	kSoundID2 = action:GetScriptParam("HIT_SOUND_ID_FEMALE");
		if kSoundID2~="" then
			actor:AttachSound(2785,kSoundID2);
		end	
	end
		
	local iTargetCount = action:GetTargetCount();
	local i =0;
	if iTargetCount>0 then
		
		local bWeaponSoundPlayed = false;
		
		while i<iTargetCount do
		
			local actionResult = action:GetTargetActionResult(i);
			if actionResult:IsNil() == false then
			
				local kTargetGUID = action:GetTargetGUID(i);
				local kTargetPilot = g_pilotMan:FindPilot(kTargetGUID);
				if kTargetPilot:IsNil() == false and actionResult:IsMissed() == false then
					
					local actorTarget = kTargetPilot:GetActor();
					
                    if bWeaponSoundPlayed == false then
                        bWeaponSoundPlayed = true;
                        -- 피격 소리 재생
						local actionName = action:GetID();
						if actionResult:GetCritical() then
							actionName = "Critical_dmg"
						end
                        actorTarget:PlayWeaponSound(WST_HIT, actor, actionName, 0, action:GetTargetInfo(i));
                    end
					Skill_Arcmage_WrathOfHeaven_DoDamage(actor,actorTarget,actionResult, action:GetTargetInfo(i));
				end
				
			end
			
			i=i+1;
		
		end
	end

	action:GetTargetList():ApplyActionEffects();
	actor:ClearIgnoreEffectList();
	
	iHitCount = iHitCount + 1;
	action:SetParamInt(2,iHitCount);
	action:SetParamInt(13,1);
end

function Skill_Arcmage_WrathOfHeaven_DoDamage(actor,actorTarget,kActionResult,kActionTargetInfo)
	
	CheckNil(nil==kActionResult);
	CheckNil(nil==kActionTargetInfo);
	
	local kAction = actor:GetAction()
	local kActionID = kAction:GetID()
	
	if actor:GetPilotGuid():IsEqual(actorTarget:GetPilotGuid()) then
		return
	end
	
	local	iABVIndex = kActionTargetInfo:GetABVIndex();
	local pt = actorTarget:GetABVShapeWorldPos(iABVIndex);
	
	local disableEffect = kAction:GetParamInt(13);
	if disableEffect == 0 then
		local   kHitTargetEffectID = kAction:GetScriptParam("HIT_TARGET_EFFECT_ID");
		local   kHitTargetEffectTargetNodeID = kAction:GetScriptParam("HIT_TARGET_EFFECT_TARGET_NODE");
		if kHitTargetEffectID ~= "" and kHitTargetEffectTargetNodeID ~= "" then
			actorTarget:AttachParticle(7211,kHitTargetEffectTargetNodeID,kHitTargetEffectID);
		end	
	end
	
	if kAction:GetScriptParam("NO_DEFAULT_HIT_EFFECT") ~= "TRUE" then
		local   kHitTargetEffectOffset = tonumber(kAction:GetScriptParam("HIT_TARGET_EFFECT_RANDOM_OFFSET"));
		
		if kHitTargetEffectOffset ~= nil then
			pt:SetX(pt:GetX() + math.random(-kHitTargetEffectOffset,kHitTargetEffectOffset))
			pt:SetY(pt:GetY() + math.random(-kHitTargetEffectOffset,kHitTargetEffectOffset))
			pt:SetZ(pt:GetZ() + math.random(-kHitTargetEffectOffset,kHitTargetEffectOffset))
		end

		local	iHitCount = actor:GetAction():GetParamInt(2);

	    if kActionResult:GetCritical() then
		    actorTarget:AttachParticleToPoint(12 + iHitCount, pt, "e_dmg_cri")
	    else
		    actorTarget:AttachParticleToPoint(12 + iHitCount, pt, "e_dmg")
	    end	
	    
	    
	    if kAction:GetScriptParam("NO_DAMAGE_HOLD") ~= "TRUE" then
    	    
	        -- 충격 효과 적용
	        local iShakeTime = g_fAttackHitHoldingTime * 1000;
	        actor:SetAnimSpeedInPeriod(0.01, iShakeTime);
	        actor:SetShakeInPeriod(5, iShakeTime/2);
	        actorTarget:SetShakeInPeriod(5, iShakeTime);		
	    end
	end
end
