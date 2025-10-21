-- [WEAPON_XML] : 무기 외형을 바꾸고 싶을 경우 무기 아이템의 XML 경로.
-- [HIT_SOUND_ID] : HIT 키에서 플레이 할 사운드 ID
-- [MULTI_HIT_NUM] : 여러번 때리는 경우, 때리는 횟수
-- [FIND_TARGET_TYPE] : 타겟 찾는 방법 : RAY(DEFAULT),BAR,SPHERE
-- [HIT_CASTER_EFFECT_TARGET_NODE] : HIT 키에서 캐스터에게 이펙트를 붙일때, 붙일 노드
-- [HIT_CASTER_EFFECT_ID] : HIT 키에서 캐스터에게 붙일 이펙트 ID
-- [FIRE_CASTER_EFFECT_TARGET_NODE] : 스킬이 실제 발동될때 캐스터에게 이펙트를 붙일때, 붙일 노드
-- [FIRE_CASTER_EFFECT_ID] : 스킬이 실제 발동될때 캐스터에게 붙일 이펙트 ID
-- [FIRE_CASTER_EFFECT_TARGET_NODE2] : 스킬이 실제 발동될때 캐스터에게 이펙트를 붙일때, 붙일 노드
-- [FIRE_CASTER_EFFECT_ID2] : 스킬이 실제 발동될때 캐스터에게 붙일 이펙트 ID
-- [FIRE_CASTER_EFFECT_ID_DEFAULT_DETACH] : 스킬이 떨어질때 기본 Detach 옵션을 사용할것인가
-- [FIRE_CASTER_EFFECT_ID2_DEFAULT_DETACH] : 스킬이 떨어질때 기본 Detach 옵션을 사용할것인가
-- [DETACH_FIRE_EFFECT_AT_FINISH] : 스킬이 끝날때 FIRE_CASTER_EFFECT 를 제거할 것인가
-- [HIT_TARGET_EFFECT_TARGET_NODE] : HIT 키에서 타겟에게 이펙트를 붙일때, 붙일 노드
-- [HIT_TARGET_EFFECT_ID] : HIT 키에서 타겟에게 붙일 이펙트 ID
-- [NO_DEFAULT_HIT_EFFECT] : 기본 타격 이펙트를 사용하지 않을것인가.
-- [NO_WEAPON_TRAIL] : 웨폰 트레일 사용하지 않을것인가
-- [NO_DAMAGE_HOLD] : 대미지 타이밍에 경직효과를 사용하지 않을것인가
-- [TRANSFORM_XML] : 변신 XML ID
-- [TRANSFORM_ID] : 변신 Status Effect ID
-- [MULTI_HIT_RANDOM_OFFSET] : 데미지를 어려번 나누어 때릴 경우 수치가 뜨는곳에 랜덤 오프셋을 준다.
-- [HIT_TARGET_EFFECT_RANDOM_OFFSET] : 타겟에 붙는 Hit이펙트의 위치에 랜덤 오프셋을 준다.
-- [WEAPON_HIDE] : 무기를 그냥 숨기기만 할때

-- Melee
function Act_Melee_Use_TargetListModified_OnCheckCanEnter(actor,action)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	local weapontype = actor:GetEquippedWeaponType();
	
	if weapontype == 0 then
		return	false;
	end

	if actor:IsMeetFloor() == false then
			return	false;
	end

	return		true;
	
end
function Act_Melee_Use_TargetListModified_OnCastingCompleted(actor, action)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
	Act_Melee_Use_TargetListModified_Fire(actor,action);
	action:SetSlot(action:GetCurrentSlot()+1);
	actor:PlayCurrentSlot();

end
function Act_Melee_Use_TargetListModified_Fire(actor,action)
	ODS("RestoreItemModel Act_Melee_Use_TargetListModified_Fire\n");
	
	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
	--	무기 바꾸기 처리
	if action:GetScriptParam("WEAPON_XML") ~= "" then
		actor:ChangeItemModel(ITEM_WEAPON, action:GetScriptParam("WEAPON_XML"));
		ODS("ChangeItemModel "..action:GetScriptParam("WEAPON_XML").."\n");
	end

	-- 무기 숨기기 처리
	if action:GetScriptParam("WEAPON_HIDE") == "TRUE" then
		actor:HideParts(6,true); -- 6번이 무기
	end
	
    local   kFireEffectID = action:GetScriptParam("FIRE_CASTER_EFFECT_ID");
    local   kFireEffectTargetNodeID = action:GetScriptParam("FIRE_CASTER_EFFECT_TARGET_NODE");
    
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

    kFireEffectID = action:GetScriptParam("FIRE_CASTER_EFFECT_ID2");
    kFireEffectTargetNodeID = action:GetScriptParam("FIRE_CASTER_EFFECT_TARGET_NODE2");
    
    if kFireEffectID ~= "" and kFireEffectTargetNodeID ~= "" then
    
        if kFireEffectTargetNodeID == "__FRONT_ACTOR" then
            
            local   kDir = actor:GetLookingDir();
            local   fDistance = 30;
            kDir:Multiply(fDistance);
            kDir:Add(actor:GetPos());
            kDir:SetZ(kDir:GetZ()-28);
            
            actor:AttachParticleToPoint(7213,kDir,kFireEffectID);
            
        else
            actor:AttachParticle(7213,kFireEffectTargetNodeID,kFireEffectID);
        end
    
    end	
		
	if action:GetScriptParam("NO_WEAPON_TRAIL") ~= "TRUE" then	
	    actor:StartWeaponTrail();
	end
	
	--  변신 적용
    local kTransformXMLID = action:GetScriptParam("TRANSFORM_XML");
    if kTransformXMLID ~= "" then
    
        local   iTransformEffectID = tonumber(action:GetScriptParam("TRANSFORM_ID"));
        if iTransformEffectID ~= nil then
        
            local   kMan = GetStatusEffectMan();
            kMan:AddStatusEffectToActor(actor:GetPilotGuid(),kTransformXMLID,iTransformEffectID,0);
        
        end
    
    end    
end
function Act_Melee_Use_TargetListModified_OnFindTarget(actor,action,kTargets)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
	action:SetParam(0,"");
	
	local  iAttackRange = action:GetSkillRange(0,actor);
	if iAttackRange == 0 then
	    iAttackRange = 100
	end
	
	return  Act_Melee_Use_TargetListModified_FindTargetEx(actor,action,action:GetActionNo(),iAttackRange,kTargets);
	
end

function    Act_Melee_Use_TargetListModified_FindTargetEx(actor,action,iActionNo,iAttackRange,kTargets)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
	local	kPos = actor:GetPos();
	local	kDir = actor:GetLookingDir();
	local	fBackDistance = 10;
	kDir:Multiply(-fBackDistance);
	kPos:Add(kDir);

	local kParam = FindTargetParam();
	
	local   strFindTargetType = action:GetScriptParam("FIND_TARGET_TYPE");
	if strFindTargetType == "" then
	    strFindTargetType = "RAY"
	end
	
	local iFindTargetType = TAT_RAY_DIR;
	
	if strFindTargetType == "RAY" then
	
	    iFindTargetType = TAT_RAY_DIR;
    	kParam:SetParam_1(kPos,actor:GetLookingDir());
    	kParam:SetParam_2(iAttackRange,30,0,0);
	
	elseif strFindTargetType == "BAR" then
	    iFindTargetType = TAT_BAR;
    	kParam:SetParam_1(kPos,actor:GetLookingDir());
    	kParam:SetParam_2(iAttackRange,60,0,0);
	
	elseif strFindTargetType == "SPHERE" then
	    iFindTargetType = TAT_SPHERE;
    	kParam:SetParam_1(actor:GetPos(),actor:GetLookingDir());
    	kParam:SetParam_2(0,30,iAttackRange,0);
	
	end
	
	kParam:SetParam_3(true,FTO_NORMAL);
	local iTargets = action:FindTargetsEx(iActionNo,iFindTargetType,kParam,kTargets,kTargets);
	
	return iTargets;
end

function Act_Melee_Use_TargetListModified_OnEnter(actor, action)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());

--	local actorID = actor:GetID()
	local actionID = action:GetID()
--	local weapontype = actor:GetEquippedWeaponType();
	
	ODS("Act_Melee_Use_TargetListModified_OnEnter actionID:"..actionID.." GetActionParam:"..action:GetActionParam().."\n");
	
	if actor:IsMyActor() == true then
		if actor:IsMeetFloor() == false then	
			
			-- 공중에서 공격을 했다면 종료
			action:ChangeToNextActionOnNextUpdate(true,true);
			return	true;	

		end
	end
	
	if action:GetActionParam() == AP_CASTING then	--	캐스팅이라면 그냥 리턴하자.
		return	true;
	end

	Act_Melee_Use_TargetListModified_OnCastingCompleted(actor,action);
	
	if actor:IsMyActor() and actionID == "a_archer_melee" then
	    GetComboAdvisor():OnNewActionEnter(action:GetID());
	end
	
	return true
end

function Act_Melee_Use_TargetListModified_OnUpdate(actor, accumTime, frameTime)
	
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

function Act_Melee_Use_TargetListModified_OnCleanUp(actor)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
		
    local   kCurAction = actor:GetAction();
    
	ODS("RestoreItemModel Act_Melee_Use_TargetListModified_ReallyLeave\n");
	--	무기 원래대로
	if kCurAction:GetScriptParam("WEAPON_XML") ~= "" then
		actor:RestoreItemModel(ITEM_WEAPON);
	end

	-- 무기를 숨겼을 경우 원래대로
	if kCurAction:GetScriptParam("WEAPON_HIDE") == "TRUE" then
		actor:HideParts(6,false); -- 6번이 무기
	end


	if kCurAction:GetScriptParam("NO_WEAPON_TRAIL") ~= "TRUE" then	
	    actor:EndWeaponTrail();
	end
	
	if kCurAction:GetScriptParam("DETACH_FIRE_EFFECT_AT_FINISH") == "TRUE" then
		local DefaultDetaChID = false;
		local DefaultDetaChID2 = false;
		if kCurAction:GetScriptParam("FIRE_CASTER_EFFECT_ID_DEFAULT_DETACH") == "TRUE" then
			DefaultDetaChID = true;
		end

		if kCurAction:GetScriptParam("FIRE_CASTER_EFFECT_ID2_DEFAULT_DETACH") == "TRUE" then
			DefaultDetaChID2 = true;
		end		
	
	    actor:DetachFrom(7212,DefaultDetaChID)
	    actor:DetachFrom(7213,DefaultDetaChID2)
		actor:DetachFrom(7214)	
	end
	
	-- 모습 원래대로(이걸 사용한후 curAction을 얻어와 쓰면 절대로 안됨!!!)
    local kTransformXMLID = kCurAction:GetScriptParam("TRANSFORM_XML");
    if kTransformXMLID ~= "" then
    
        local   iTransformEffectID = tonumber(kCurAction:GetScriptParam("TRANSFORM_ID"));
        if iTransformEffectID ~= nil then
        
            local   kMan = GetStatusEffectMan();
            kMan:RemoveStatusEffectFromActor(actor:GetPilotGuid(),iTransformEffectID);
        
        end
    
    end
    	
	return true;
end

function Act_Melee_Use_TargetListModified_OnLeave(actor, action)

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

	return false 
end

function Act_Melee_Use_TargetListModified_OnTargetListModified(actor,action,bIsBefore)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
    if bIsBefore == false then

        if action:GetParam(2) == "HIT" then
        
            --  hit 키 이후라면
            --  바로 대미지 적용시켜준다.
            if actor:IsMyActor()  then
				action:GetTargetList():ApplyActionEffects(true,true);
				--Act_Melee_Use_TargetListModified_HitOneTime(actor,action);				
            else
                Act_Melee_Use_TargetListModified_HitOneTime(actor,action);
            end        
        end
    end
end

function Act_Melee_Use_TargetListModified_DoDamage(actor,actorTarget,kActionResult,kActionTargetInfo)
	
	CheckNil(nil==kActionResult);
	CheckNil(nil==kActionTargetInfo);	
	
	local kAction = actor:GetAction()
	
	CheckNil(nil==kAction);
	CheckNil(kAction:IsNil());
	
--	local kActionID = kAction:GetID()
	
	if actor:GetPilotGuid():IsEqual(actorTarget:GetPilotGuid()) then
		return
	end
	
	local	iABVIndex = kActionTargetInfo:GetABVIndex();
	local pt = actorTarget:GetABVShapeWorldPos(iABVIndex);
	
    local   kHitTargetEffectID = kAction:GetScriptParam("HIT_TARGET_EFFECT_ID");
    local   kHitTargetEffectTargetNodeID = kAction:GetScriptParam("HIT_TARGET_EFFECT_TARGET_NODE");
    
    if kHitTargetEffectID ~= "" and kHitTargetEffectTargetNodeID ~= "" then
    
        actorTarget:AttachParticle(7211,kHitTargetEffectTargetNodeID,kHitTargetEffectID);
    
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

function Act_Melee_Use_TargetListModified_HitOneTime(actor,action)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());

	local	iHitCount = action:GetParamInt(2);
	
	if iHitCount == 0 then
	
	    local   kHitEffectID = action:GetScriptParam("HIT_CASTER_EFFECT_ID");
	    local   kHitEffectTargetNodeID = action:GetScriptParam("HIT_CASTER_EFFECT_TARGET_NODE");
	    
	    if kHitEffectID ~= "" and kHitEffectTargetNodeID ~= "" then
	    
	        actor:AttachParticle(7214,kHitEffectTargetNodeID,kHitEffectID);
	    
	    end
	
	
	end

	local	iTotalHit = tonumber(action:GetScriptParam("MULTI_HIT_NUM"));
	if iTotalHit == nil or iTotalHit == 0 then
	    iTotalHit = 1;
	end
	
	if iHitCount == iTotalHit then
		return
	end
	
	local	kSoundID = action:GetScriptParam("HIT_SOUND_ID");
	if kSoundID~="" then
		actor:AttachSound(2784,kSoundID);
	end	
		
	if iHitCount == iTotalHit-1 then
	
		local iTargetCount = action:GetTargetCount();
		local i =0;
		if iTargetCount>0 then
			
			local bWeaponSoundPlayed = false;
			
			while i<iTargetCount do
			
				local actionResult = action:GetTargetActionResult(i);
				if actionResult:IsNil() == false then
				
					action:SetParamInt(3,actionResult:GetValue()/(iTotalHit));
					local	iOneDmg = action:GetParamInt(3);
				
					actionResult:SetValue(
						actionResult:GetValue() - iOneDmg*(iTotalHit-1));
						
					local kTargetGUID = action:GetTargetGUID(i);
					local kTargetPilot = g_pilotMan:FindPilot(kTargetGUID);
					if kTargetPilot:IsNil() == false and actionResult:IsMissed() == false then
						
						local actorTarget = kTargetPilot:GetActor();
						
	                    if bWeaponSoundPlayed == false then
	                        bWeaponSoundPlayed = true;
                            -- 피격 소리 재생
                            local actionName = action:GetID()
							if actionResult:GetCritical() then
								actionName = "Critical_dmg"
							end
							actorTarget:PlayWeaponSound(WST_HIT, actor, actionName, 0, action:GetTargetInfo(i));
                        end

						Act_Melee_Use_TargetListModified_DoDamage(actor,actorTarget,actionResult,action:GetTargetInfo(i));

						local iHoldingTime = g_fAttackHitHoldingTime * 1000;
						local iShakeTime = g_fAttackHitHoldingTime * 1000;
						
						-- 충격 효과 적용
						if bNoHold == false then
							actor:SetAnimSpeedInPeriod(0.00001,iHoldingTime);
						end
						
						actor:SetShakeInPeriod(5,iShakeTime/2);
						actorTarget:SetShakeInPeriod(5,iShakeTime);
					end
					
				end
				
				i=i+1;
			
			end
		end

		action:GetTargetList():ApplyActionEffects();
		actor:ClearIgnoreEffectList();
	
	else
		local kRandomPos = action:GetScriptParam("MULTI_HIT_RANDOM_OFFSET");

		if kRandomPos == nil then
			kRandomPos = 0;
		end

		action:GetTargetList():ApplyOnlyDamage(iTotalHit,true, kRandomPos);
		
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
                            local actionName = action:GetID()
							if actionResult:GetCritical() then
								actionName = "Critical_dmg"
							end
							actorTarget:PlayWeaponSound(WST_HIT, actor, actionName, 0, action:GetTargetInfo(i));
                        end							
						
						Act_Melee_Use_TargetListModified_DoDamage(actor,actorTarget,actionResult,action:GetTargetInfo(i));
					end
				end
				
				i=i+1;
			
			end
		end
		
	end
		
	iHitCount = iHitCount + 1;
	action:SetParamInt(2,iHitCount);
	
end

function Act_Melee_Use_TargetListModified_OnEvent(actor,textKey)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
		
	local kAction = actor:GetAction();
	
	CheckNil(nil==kAction);
	CheckNil(kAction:IsNil());
	
	if kAction:GetActionParam() == AP_CASTING then
		return true;
	end
	
	if textKey == "hit" or textKey == "fire" then
	
	    kAction:SetParam(2,"HIT");
	
		local	kSoundID = kAction:GetScriptParam("HIT_SOUND_ID");
		if kSoundID~="" then
			actor:AttachSound(106,kSoundID);
		end	

	    if actor:IsMyActor() then
    	    kAction:CreateActionTargetList(actor);   --  타겟 잡기
	    
	        if IsSingleMode() then
	            Act_Melee_Use_TargetListModified_OnTargetListModified(actor,kAction,false)
	        else
	            kAction:BroadCastTargetListModify(actor:GetPilot());
	        end	    
	    end
			
		Act_Melee_Use_TargetListModified_HitOneTime(actor,kAction);
	
	end

	return	true;
end
