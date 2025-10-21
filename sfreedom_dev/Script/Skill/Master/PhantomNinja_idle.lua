-- [WEAPON_XML] : 무기 외형을 바꾸고 싶을 경우 무기 아이템의 XML 경로.
-- [HIT_SOUND_ID] : HIT 키에서 플레이 할 사운드 ID
-- [HIT_SOUND_ID_MALE] : HIT 키에서 플레이 할 사운드 ID - 남성
-- [HIT_SOUND_ID_FEMALE] : HIT 키에서 플레이 할 사운드 ID - 여성
-- [FIRE_SOUND_ID] : 시전시에 플레이 할 사운드 ID
-- [CASTING_SOUND_ID] : 캐스팅 시에 플레이 할 사운드 ID
-- [MULTI_HIT_NUM] : 여러번 때리는 경우, 때리는 횟수
-- [FIND_TARGET_TYPE] : 타겟 찾는 방법 : RAY(DEFAULT),BAR,SPHERE
-- [HIT_CASTER_EFFECT_TARGET_NODE_TO_WORLD_POS] : HIT 키에서 캐스터에게 이펙트를 붙일때, 붙일 노드
-- [HIT_CASTER_EFFECT_TARGET_NODE_TO_WORLD_POS_WITH_ROTATE] : HIT 키에서 캐스터에게 이펙트를 붙일때, 붙일 노드
-- [HIT_CASTER_EFFECT_TARGET_NODE] : HIT 키에서 캐스터에게 이펙트를 붙일때, 붙일 노드
-- [HIT_CASTER_EFFECT_ID] : HIT 키에서 캐스터에게 붙일 이펙트 ID
-- [FIRE_CASTER_EFFECT_TARGET_NODE_TO_WORLD_POS] : 스킬이 실제 발동될때 캐스터에게 이펙트를 붙일때, 붙일 노드
-- [FIRE_CASTER_EFFECT_TARGET_NODE_TO_WORLD_POS_WITH_ROTATE] : 스킬이 실제 발동될때 캐스터에게 이펙트를 붙일때, 붙일 노드
-- [FIRE_CASTER_EFFECT_TARGET_NODE] : 스킬이 실제 발동될때 캐스터에게 이펙트를 붙일때, 붙일 노드
-- [FIRE_CASTER_EFFECT_ID] : 스킬이 실제 발동될때 캐스터에게 붙일 이펙트 ID
-- [FIRE_CASTER_EFFECT_TARGET_NODE2] : 스킬이 실제 발동될때 캐스터에게 이펙트를 붙일때, 붙일 노드
-- [FIRE_CASTER_EFFECT_ID2] : 스킬이 실제 발동될때 캐스터에게 붙일 이펙트 ID
-- [FIRE_CASTER_EFFECT_ID_DEFAULT_DETACH] : 스킬이 떨어질때 기본 Detach 옵션을 사용할것인가
-- [FIRE_CASTER_EFFECT_ID2_DEFAULT_DETACH] : 스킬이 떨어질때 기본 Detach 옵션을 사용할것인가
-- [CASTING_EFFECT_TARGET_NODE_TO_WORLD_POS] : 캐스팅 시에 이펙트를 붙을때, 붙일 노드
-- [CASTING_EFFECT_TARGET_NODE] : 캐스팅 시에 이펙트를 붙을때, 붙일 노드
-- [CASTING_EFFECT_EFFECT_ID] : 캐스팅 시에 이펙트 붙일 ID
-- [CASTING_EFFECT_ID_DEFAULT_DETACH] : 스킬이 떨어질때 기본 Detach 옵션을 사용할것인가
-- [CASTING_EFFECT_SIZE_TO_SKILLRANGE] : 캐스팅 시에 이펙트 Size를 스킬 범위에 따라 스케일링 할때
-- [DETACH_FIRE_EFFECT_AT_FINISH] : 스킬이 끝날때 FIRE_CASTER_EFFECT 를 제거할 것인가
-- [DETACH_CASTING_EFFECT_AT_FINISH] : 스킬이 끝날때 FIRE_CASTER_EFFECT 를 제거할 것인가
-- [HIT_TARGET_EFFECT_TARGET_NODE] : HIT 키에서 타겟에게 이펙트를 붙일때, 붙일 노드
-- [HIT_TARGET_EFFECT_ID] : HIT 키에서 타겟에게 붙일 이펙트 ID
-- [NO_DEFAULT_HIT_EFFECT] : 기본 타격 이펙트를 사용하지 않을것인가.
-- [NO_WEAPON_TRAIL] : 웨폰 트레일 사용하지 않을것인가
-- [NO_DAMAGE_HOLD] : 대미지 타이밍에 경직효과를 사용하지 않을것인가
-- [TRANSFORM_XML] : 변신 XML ID
-- [TRANSFORM_ID] : 변신 Status Effect ID
-- [MULTI_HIT_RANDOM_OFFSET] : 데미지를 어려번 나누어 때릴 경우 수치가 뜨는곳에 랜덤 오프셋을 준다.
-- [HIT_TARGET_EFFECT_RANDOM_OFFSET] : 타겟에 붙는 Hit이펙트의 위치에 랜덤 오프셋을 준다.
-- [APPLY_EFFECT_ONLY_LAST_HIT] : 마지막 힛트시에만 이펙트 처리를 해줄것인가.
-- [NO_USE_TLM] : Not Use TargetList Modified -> hit 타이밍에 타겟을 잡는 형태를 사용하지 않을것인가(TRUE,FALSE)
-- [HIDE_WEAPON] : 무기를 숨길때
-- [CLEANUP_CASTER_EFFECT_ID] : 클린업시에 붙는 이펙트 ID
-- [CLEANUP_CASTER_EFFECT_TARGET_NODE] : 클린업시에 붙는 이펙트 노드
-- [CLEANUP_CASTER_EFFECT_SCALE] : 클린업시에 붙는 이펙트 크기
-- [SHOW_TEXT_TARGETLIST_COUNT_NUM] : 타겟리스트의 일정 카운트 수가 맞으면
-- [SHOW_TEXT_TARGETLIST_TEXTNO] : 텍스트를 출력

function Skill_Phantom_Ninja_Idle_OnCastingCompleted(actor, action)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
	Skill_Phantom_Ninja_Idle_Fire(actor,action);
--	action:SetSlot(action:GetCurrentSlot()+1);
	actor:PlayCurrentSlot();
	action:SetParam(3,"0");
end
function Skill_Phantom_Ninja_Idle_Fire(actor,action)
	ODS("RestoreItemModel Skill_Phantom_Ninja_Idle_Fire\n");
	
	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());

	local	kSoundID = action:GetScriptParam("FIRE_SOUND_ID");
	if kSoundID~="" then
		actor:AttachSound(2783,kSoundID);
	end		
end
function Skill_Phantom_Ninja_Idle_OnEnter(actor, action)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());

	local actionID = action:GetID()
	
	ODS("Skill_Phantom_Ninja_Idle_OnEnter actionID:"..actionID.." GetActionParam:"..action:GetActionParam().."\n");

	action:SetParam(3,"0");

	local	kSoundID = action:GetScriptParam("CASTING_SOUND_ID");
	if kSoundID~="" then
		actor:AttachSound(2785,kSoundID);
	end	
	
	action:SetParamInt(2,0);
	
	if action:GetActionParam() == AP_CASTING then	--	캐스팅이라면 그냥 리턴하자.
		return	true;
	end

	Skill_Phantom_Ninja_Idle_OnCastingCompleted(actor,action);
	
	local kGuidString = actor:GetParam("TargetGUID");
	if nil == kGuidString then
		actor:AttachSound(2780, "Ninja_Poof");
		actor:SetParam("SummonEnd", "false");
		actor:AttachParticleToPoint(7220, actor:GetNodeWorldPos("char_root"), "ef_smog_03_char_root");
		local kCallerGUID = actor:GetCallerGuid();
		local kCallerPilot = g_pilotMan:FindPilot(kCallerGUID);
		local kCallerActor = kCallerPilot:GetActor();
		if kCallerActor:IsNil() then return true end;
		
		if kCallerActor:IsMyActor() then
			if( CheckNil(g_world == nil) ) then return false end
			if( CheckNil(g_world:IsNil()) ) then return false end
			g_world:AddDrawActorFilter(actor:GetPilotGuid());
		end

		-- Caller에게 Caller가 소환한 놈이라고 알려준다.
		kCallerActor:AddSummonUnit(actor:GetPilotGuid());

		actor:SetParam("TargetGUID", "");

		local kCallerAction = kCallerActor:GetAction();
		if kCallerAction:IsNil() then return true end;

		local kTargetList = kCallerAction:GetTargetList();
		local iTargetCount = kTargetList:size();
		if 0 < iTargetCount then
			local kTargetInfo = kTargetList:GetTargetInfo(0);
			local kTargetGUID = kTargetInfo:GetTargetGUID();
			local kTargetPilot = g_pilotMan:FindPilot(kTargetGUID);

			if kTargetPilot:IsNil() == false then
				local   kTargetActor = kTargetPilot:GetActor();
				if kTargetActor:IsNil() == false then
					actor:SetParam("TargetGUID", kTargetActor:GetPilotGuid():GetString());

					-- 타겟을 잡은 놈은 지워준다.
					kTargetList:DeleteTargetInfo(0);
				end
			end
		else
			actor:SetParam("TargetGUID", "");
		end

		actor:SetParam("CurHit", tostring(0)); -- Current Hit
		actor:SetParam("TotalHit",tostring(8)); -- Total Hit	
	end

	return true
end

function Skill_Phantom_Ninja_Idle_OnUpdate(actor, accumTime, frameTime)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	local action = actor:GetAction()
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
	local animDone = actor:IsAnimationDone()
	local nextActionName = action:GetNextActionName()

	local kGuidString = actor:GetParam("TargetGUID");
	local kGuid = GUID("123");
	if nil ~= kGuidString then
		kGuid:Set(kGuidString);
	else
		local kMyPilot = actor:GetPilot()
		if kMyPilot:IsNil() then return true end
		local kMyUnit = kMyPilot:GetUnit()
		if kMyUnit:IsNil() then return true end
		local kCallerGUID = kMyUnit:GetCaller();
		if nil ~= kCallerGUID then
			kGuid:Set(kMyUnit:GetCaller());
		end
	end

	local kParamValue = actor:GetParam("SummonEnd");
	if kParamValue == "true" then
		return false;
	end

	local kTargetPilot = g_pilotMan:FindPilot(kGuid)
	if kTargetPilot:IsNil() then return true end
	local kTargetActor = kTargetPilot:GetActor()
	if kTargetActor:IsNil() then return true end
	local kTargetPos = kTargetActor:GetPos()

	local iNextSlot = 0;
	local bForceNextSlot = false
	--Pos
	actor:SetTraceFlyTargetLoc( kTargetPos )
	local fDistance = Trace(actor, 2.5, frameTime, 40.0, 15.0, true )
	if fDistance < 50.0 then
		iNextSlot = 1;
		bForceNextSlot = true;
		if nil ~= kGuidString then
			local kGuid = GUID("123");
			kGuid:Set(kGuidString);
			local kTargetPilot = g_pilotMan:FindPilot(kGuid);
			if false == kTargetPilot:IsNil() then
				local kTargetUnit = kTargetPilot:GetUnit()
				if false == kTargetUnit:IsNil() then
					if false == kTargetUnit:IsDead() then
						
						local iTotalHit =  tonumber(actor:GetParam("TotalHit"));
						local iCurHit = tonumber(actor:GetParam("CurHit"));

						if iTotalHit < iCurHit then
							iNextSlot = 0;
							bForceNextSlot = false;
						elseif iTotalHit > iCurHit then
							iCurHit = iCurHit + 1;
							actor:SetParam("CurHit", tostring(iCurHit));

							actor:ReserveTransitAction("a_Phantom_Ninja_melee_01");
						elseif iTotalHit == iCurHit then
							iCurHit = iCurHit + 1;
							actor:SetParam("CurHit", tostring(iCurHit));
							actor:ReserveTransitAction("a_Phantom_Ninja_melee_finish");
						end

					else
						iNextSlot = 0;
						bForceNextSlot = false;
					end
				end				
			end
		end		
	else
		iNexSlot = 1;
		bForceNextSlot = true;
	end	

	if actor:IsAnimationDone() or bForceNextSlot then
		action:SetSlot(iNextSlot)
		actor:PlayCurrentSlot(false)
	end
	
	if action:GetParam(1) == "end" then
		return	false;
	end
	
	return true
end

function Skill_Phantom_Ninja_Idle_OnCleanUp(actor)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
    local   kCurAction = actor:GetAction();
	
	ODS("RestoreItemModel Skill_Phantom_Ninja_Idle_ReallyLeave\n");

	local kCallerGUID = actor:GetCallerGuid();
	local kCallerPilot = g_pilotMan:FindPilot(kCallerGUID);
	local kCallerActor = kCallerPilot:GetActor();
	if kCallerActor:IsNil() then return true end;
	
	local kParamValue = actor:GetParam("SummonEnd");
	if kParamValue == "true" then
		kCallerActor:AttachSound(2780, "Ninja_Poof");
		kCallerActor:DeleteSummonUnit(actor:GetPilotGuid());
		kCallerActor:AttachParticleToPoint(7220, actor:GetNodeWorldPos("char_root"), "ef_smog_03_char_root");
		if( CheckNil(g_world == nil) ) then return true end
		if( CheckNil(g_world:IsNil()) ) then return true end
		g_world:RemoveActorOnNextUpdate(actor:GetPilotGuid())
	end
		
	return true
end

function Skill_Phantom_Ninja_Idle_OnLeave(actor, action)

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
		if( false == actor:PlayNext()) then 
			actor:GetAction():SetNextActionName("a_battle_idle");
		end
		actor:GetAction():ChangeToNextActionOnNextUpdate(true,true);
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

function Skill_Phantom_Ninja_Idle_DoDamage(actor,actorTarget,kActionResult,kActionTargetInfo)
	
	CheckNil(nil==kActionResult);
	CheckNil(nil==kActionTargetInfo);
	
	local kAction = actor:GetAction()
	local kActionID = kAction:GetID()
	
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

function Skill_Phantom_Ninja_Idle_HitOneTime(actor,action)
	
	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
	local	iHitCount = action:GetParamInt(2);
	
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
					Skill_Phantom_Ninja_Idle_DoDamage(actor,actorTarget,actionResult, action:GetTargetInfo(i));
				end
				
			end
			
			i=i+1;
		
		end
	end

	action:GetTargetList():ApplyActionEffects();
	actor:ClearIgnoreEffectList();
	
	iHitCount = iHitCount + 1;
	action:SetParamInt(2,iHitCount);
	
end
function Skill_Phantom_Ninja_Idle_OnTargetListModified(actor,action,bIsBefore)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
    if bIsBefore == false then

        if actor:IsMyActor()  then
			action:GetTargetList():ApplyActionEffects(true,true);
			return;
		end

		local	iTotalHit = tonumber(action:GetScriptParam("MULTI_HIT_NUM"));
		if iTotalHit == nil then
			iTotalHit = 1
		end
		
        if iTotalHit>1 or action:GetParamInt(2) > 0  then
        
            --  hit 키 이후라면
            --  바로 대미지 적용시켜준다.
            Skill_Phantom_Ninja_Idle_HitOneTime(actor,action);
        
        end        
    
    end

end

function Skill_Phantom_Ninja_Idle_OnEvent(actor,textKey)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
		
	local kAction = actor:GetAction();
		
	CheckNil(nil==kAction);
	CheckNil(kAction:IsNil());

	
	if kAction:GetActionParam() == AP_CASTING then
		return true;
	end
	
	if textKey == "hit" or textKey == "fire" then
	
		if actor:IsUnderMyControl() then
			local	bUseTLM = (kAction:GetScriptParam("NO_USE_TLM") ~= "TRUE");
			if bUseTLM then
			
				kAction:CreateActionTargetList(actor);
				if kAction:GetScriptParam("SHOW_TEXT_TARGETLIST_COUNT_NUM") ~= "" then
					local iTargetCount = kAction:GetTargetCount();
					if iTargetCount == tonumber(kAction:GetScriptParam("SHOW_TEXT_TARGETLIST_COUNT_NUM")) then
						AddWarnDataStr(GetTextW(tonumber(kAction:GetScriptParam("SHOW_TEXT_TARGETLIST_TEXTNO"))),2);		
					end
				end

				if IsSingleMode() then
		        
					Skill_Phantom_Ninja_Idle_HitOneTime(actor,kAction)
					Skill_Phantom_Ninja_Idle_OnTargetListModified(actor,kAction,false)
		            
					return;
				else
		        
					kAction:BroadCastTargetListModify(actor:GetPilot());
		            
				end			
			
			end	
		end
	
        Skill_Phantom_Ninja_Idle_HitOneTime(actor,kAction)
	end

	return	true;
end

function Trace(actor, fSpeed, fFrameTime, fLimitDistance, fAccelateScale, bCanRotate)
	
	if actor:IsNil() then
		return 0
	end

	local rkCurPos = actor:GetPos();
	local kTempTargetLoc = Point3(actor:GetTraceFlyTargetLoc():GetX(), actor:GetTraceFlyTargetLoc():GetY(), actor:GetTraceFlyTargetLoc():GetZ());
	local kTempCurLoc = Point3(rkCurPos:GetX(), rkCurPos:GetY(), rkCurPos:GetZ());

	kTempTargetLoc:SetZ(0);
	kTempCurLoc:SetZ(0);

	local kDiffPos = kTempTargetLoc:_Subtract(kTempCurLoc);
	local fDistance = kDiffPos:Length();

	kDiffPos:Unitize();

	local kTempNextPos = actor:GetTranslate();
	local kNextPos = Point3(kTempNextPos:GetX(), kTempNextPos:GetY(), kTempNextPos:GetZ());

	actor:FreeMove(false);
	local bZDist = 250 < math.abs(rkCurPos:GetZ() - actor:GetTraceFlyTargetLoc():GetZ());

	local bIsFloor = true;
	local iActionNo = 0;
	local bIsRiding = false;

	local kGuidString = actor:GetParam("TargetGUID");
	local kGuid = GUID("123");
	if nil ~= kGuidString then
		kGuid:Set(kGuidString);
	else
		local kMyPilot = actor:GetPilot()
		if kMyPilot:IsNil() then return 0 end
		local kMyUnit = kMyPilot:GetUnit()
		if kMyUnit:IsNil() then return 0 end
		local kCallerGUID = kMyUnit:GetCaller();
		if nil ~= kCallerGUID then
			kGuid:Set(kMyUnit:GetCaller());
		end
	end

	local kTargetPilot = g_pilotMan:FindPilot(kGuid)
	if kTargetPilot:IsNil() then return 0 end
	local kTargetActor = kTargetPilot:GetActor()
	if kTargetActor:IsNil() then return 0 end
	local kTargetPos = kTargetActor:GetPos()

	if false == kTargetActor:IsNil() then
		bIsFloor = kTargetActor:IsMeetFloor();
		if false == kTargetActor:GetAction():IsNil() then
			iActionNo = kTargetActor:GetAction():GetActionNo();
			bIsRiding = kTargetActor:IsOnRidingObject();
		end
	end

	local bTelePort = false;

	if 150 < fDistance or bZDist then
		if bIsFloor and false == bIsRiding then --주인이 바닥에 붙어 잇을 때만 펑하자
			local kTempNextPos = actor:GetTraceFlyTargetLoc();
			kNextPos = Point3(kTempNextPos:GetX(), kTempNextPos:GetY(), kTempNextPos:GetZ());
			
			bTelePort = true;
			actor:AttachParticle(1010, "p_ef_heart", "ef_show_01_p_ef_heart");
		end
	end
	
	if fLimitDistance < fDistance and false == bZDist and false == bTelePort then
		-- 열심히 쫒아가자
		--kNextPos = rkCurPos + (kDiffPos * fSpeed * fFrameTime) + (kDiffPos * (fDistance-fLimitDistance) * fAccelateScale * fFrameTime);
		kNextPos = kDiffPos:_Multiply(fSpeed * fFrameTime);
		kNextPos:Add(rkCurPos);

		local kTempNewPos = kDiffPos:_Multiply((fDistance - fLimitDistance) * fAccelateScale * fFrameTime);		
		kNextPos:Add(kTempNewPos);

		if( CheckNil(g_world == nil) ) then return false end
		if( CheckNil(g_world:IsNil()) ) then return false end
		local kTempDown = g_world:ThrowRay(Point3(kNextPos:GetX(), kNextPos:GetY(), kNextPos:GetZ()+50.0), Point3(0.0,0.0,-1.0), 200.0);
		local kTempPos = g_world:ThrowRay(Point3(kNextPos:GetX(), kNextPos:GetY(), actor:GetTraceFlyTargetLoc():GetZ()+10), Point3(0.0,0.0,-1.0), 1000.0);	--타겟이 어느정도 높이에 떠 있나?

		if (-1 == kTempPos:GetX() and -1 and kTempPos:GetY() and -1 == kTempPos:GetZ()) or 95.0 < rkCurPos:GetZ() - kTempPos:GetZ() then
			local kFrontDir = kDiffPos;
			kFrontDir:Unitize();
			local kTempFront = g_world:ThrowRay(actor:GetLastFloorPos(), kFrontDir, 40.0);	--앞으로 레이
			if -1 == kTempFront:GetX() and -1 and kTempFront:GetY() and -1 == kTempFront:GetZ() then
				actor:SetTraceFlyTargetLoc(rkCurPos); --멈추자 일단
				fDistance = 0.0;
				return fDistance;
			end
		else
			kNextPos:SetZ(kTempPos:GetZ() + 25.0)
		end

		if -1 == kTempPos:GetX() and -1 and kTempPos:GetY() and -1 == kTempPos:GetZ() then --바닥이 허공
			if false == bIsFloor then
				if 100005440 == iActionNo then --텔레점프
					actor:SetFreeMove(true);--이놈은 진짜 대점프를 탄 상태일 때만 해 주자
					kNextPos:SetZ(actor:GetTraceFlyTargetLoc():GetZ());
				else -- 바닥이 허공. 난 바닥. 주인은 허공
					actor:SetTraceFlyTargetLoc(rkCurPos); -- 멈추자 일단
					kNextPos = actor:GetTranslate();
					fDistance = 0.0;
				end
			end
		end

		kNextPos:SetZ(kTempPos:GetZ() + 25.0)
	end

	if false == actor:GetTranslate():IsEqual(kNextPos) then
		actor:SetTranslate(kNextPos);
		actor:SetTranslate(kNextPos,true);
		actor:SetWorldTranslate(kNextPos);

		if bCanRotate then
			actor:SetMovingDir(kDiffPos);
			actor:ConcilDirection(kDiffPos, true);
		end
	end

	return fDistance;
end