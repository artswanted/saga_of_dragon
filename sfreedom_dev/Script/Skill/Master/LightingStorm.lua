-- [WEAPON_XML] : 무기 외형을 바꾸고 싶을 경우 무기 아이템의 XML 경로.
-- [HIT_SOUND_ID] : HIT 키에서 플레이 할 사운드 ID
-- [HIT_SOUND_ID_MALE] : HIT 키에서 플레이 할 사운드 ID - 남성
-- [HIT_SOUND_ID_FEMALE] : HIT 키에서 플레이 할 사운드 ID - 여성
-- [FIRE_SOUND_ID] : 시전시에 플레이 할 사운드 ID
-- [CASTING_SOUND_ID_MALE] : 캐스팅 시에 플레이 할 사운드 ID - 남성
-- [CASTING_SOUND_ID_FEMALE] : 캐스팅 시에 플레이 할 사운드 ID - 여성
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
-- [SUMMON_CLASSNO] : 소환해야하는 엔티티의 ClassNo
-- [SUMMON_RESERVE_ACTION] : 소환해야하는 엔티티의 시작 액션
-- [HIT_TIMER_TIME] : Hit 시에 시작할 타이머의 총 시간
-- [HIT_TIMER_TICK] : Hit 시에 시작할 타이머의 Tick 

-- Melee
function Skill_Lighting_Storm_OnCheckCanEnter(actor,action)

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

function Skill_Lighting_Storm_OnTimer(actor,fAccumTime,action,iTimerID)
	
	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	ODS("Skill_Lighting_Storm_OnTimer:"..iTimerID.."\n", false, 912)
	
	if 0 == iTimerID then
		local	iHitCount = SkillHelpFunc_GetHitCount(action)
		local	iTotalHit = SkillHelpFunc_GetTotalHitCount(action)
		if iHitCount == iTotalHit then
			action:SetParamInt(33, 1);

			return true
		end

		local iCreateLimitCnt = action:GetParamInt(10)
		if(0 < iCreateLimitCnt) then
			ODS("iCreateLimitCnt 들어옴\n", false, 912)
			action:CreateActionTargetList(actor);		
			iCreateLimitCnt  = iCreateLimitCnt-1
			action:SetParamInt(10, iCreateLimitCnt)			
			action:BroadCastTargetListModify(actor:GetPilot());
			action:GetTargetList():SetActionEffectApplied(true)		
		end

		local i = 0;
		local kTargetList = action:GetTargetList();
		local iTargetCount = kTargetList:size();
		while i<iTargetCount do
			local   kTargetInfo = kTargetList:GetTargetInfo(i);
			local   kTargetGUID = kTargetInfo:GetTargetGUID();
			local   kTargetPilot = g_pilotMan:FindPilot(kTargetGUID);
			if kTargetPilot:IsNil() == false then
				if actor:IsMyActor() then
					if( CheckNil(g_world == nil) ) then return false end
					if( CheckNil(g_world:IsNil()) ) then return false end
					g_world:AddDrawActorFilter(kTargetGUID);
				end
			end
			i = i + 1;
		end
		
		if IsSingleMode() then
	    
			SkillHelpFunc_DefaultHitOneTime(actor,action)
			Skill_Lighting_Storm_OnTargetListModified(actor,action,false)
	        
			return true;
		else

			-- 실제는 1방 때린것이지만, 클라이언트에서 여러대 때리는것으로 연출
			ODS("들어옴 iHitCount:"..iHitCount.."\n", false, 912)
			SkillHelpFunc_DoDividedDamage(actor, action, iHitCount)
			if iHitCount == iTotalHit-1 then
				action:SetParam(1, "end")
			end
		end

		SkillHelpFunc_IncHitCount(action, 1)
		
		--ODS("iTotalHit : " .. iTotalHit.." iHitCount: ".. iHitCount.."\n", false, 1509);
		
	elseif 1 == iTimerID then
		local iSkillRange = action:GetParamInt(21);
		local iIndex = action:GetParamInt(20);

		local kPos = actor:GetPos();

		kPos:SetX(kPos:GetX() + math.random(-iSkillRange,iSkillRange));
		kPos:SetY(kPos:GetY() + math.random(-iSkillRange,iSkillRange));
		kPos:SetZ(kPos:GetZ() + 200);

		if( CheckNil(g_world == nil) ) then return false end
		if( CheckNil(g_world:IsNil()) ) then return false end
		local ptcl = g_world:ThrowRay(kPos, Point3(0,0,-1),500);
		if ptcl:GetX() == -1 and ptcl:GetY() == -1 and ptcl:GetZ() == -1 then
			--위로도 쏴 본다.
			ptcl = g_world:ThrowRay(kPos, Point3(0,0,1),500);
			if ptcl:GetX() == -1 and ptcl:GetY() == -1 and ptcl:GetZ() == -1 then
				return true;
			end			
		end

		actor:AttachParticleToPointS(1000 + iIndex, ptcl, "ef_skill_lightingstorm_02_char_root", math.random(-1.0, 1));

		action:SetParamInt(20, iIndex + 1);
	elseif 2 == iTimerID then
		local iIndex = action:GetParamInt(19);
		local kSoundIdTBl = {"LightningStorm_01", "LightningStorm_02", "LightningStorm_03"};
		local iIdx = math.random(1 ,table.getn(kSoundIdTBl))
		actor:AttachSound(200 + iIndex , kSoundIdTBl[iIdx]);
		action:SetParamInt(19, iIndex + 1);
	end

	return true;

end

function Skill_Lighting_Storm_OnCastingCompleted(actor, action)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
	Skill_Lighting_Storm_Fire(actor,action);
	action:SetSlot(action:GetCurrentSlot()+1);
	actor:PlayCurrentSlot();
	SkillHelpFunc_SetNoneCastingState(action)			-- 캐스팅 완료
end

function Skill_Lighting_Storm_Fire(actor,action)
	ODS("RestoreItemModel Skill_Lighting_Storm_Fire\n");
	
	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
	local	kSoundID = action:GetScriptParam("FIRE_SOUND_ID");
	if kSoundID~="" then
		actor:AttachSound(2783,kSoundID);
	end	
	
	SkillHelpFunc_PlayFireSound(actor, action)	-- 공격시 사운드		
	SkillHelpFunc_HideWeapon(actor, action)		-- 무기 감추기 
	SkillHelpFunc_ChangeWeapon(actor, action)	-- 무기 바꾸기 처리	
	SkillHelpFunc_ExpressFireEffect(actor, action)	-- 공격시 파티클 표현
    SkillHelpFunc_AttachWeaponTrail(actor, action)	-- 웨폰 트레일 
	SkillHelpFunc_TransformShape(actor, action)		--  변신 적용
	
	SkillHelpFunc_InitHitCount(action) -- Hit Count
	action:SetParamInt(31,0); -- hit_timer를 사용하는지 Check
	action:SetParamInt(33,0);

	action:SetParamInt(19,0);
	action:SetParamInt(20,0);
end

function Skill_Lighting_Storm_OnEnter(actor, action)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());

	local actionID = action:GetID()
	
	ODS("Skill_Lighting_Storm_OnEnter actionID:"..actionID.." GetActionParam:"..action:GetActionParam().."\n");
	SkillHelpFunc_InitHitCount(action)					-- HitCount 초기화	
	SkillHelpFunc_ExpressCastingEffect(actor, action)	-- 캐스팅때 파티클이 붙어있는지 확인하기 위해서	
	SkillHelpFunc_PlayCastingSound(actor, action) 		-- 캐스팅시 사운드	
	
	if action:GetActionParam() == AP_CASTING then	--	캐스팅이라면 그냥 리턴하자.
		return	true;
	end
	Skill_Lighting_Storm_OnCastingCompleted(actor,action);

	if actor:IsMyActor() then
		if( CheckNil(g_world == nil) ) then return false end
		if( CheckNil(g_world:IsNil()) ) then return false end
		g_world:SetShowWorldFocusFilterColorAlpha(0x000000, 0.0, 1.0, 0.4,true,true);
		g_world:AddDrawActorFilter(actor:GetPilotGuid());
	end

	action:CreateActionTargetList(actor,true);
	local i = 0;
	local kTargetList = action:GetTargetList();
	local iTargetCount = kTargetList:size();
	while i<iTargetCount do
		local   kTargetInfo = kTargetList:GetTargetInfo(i);
		local   kTargetGUID = kTargetInfo:GetTargetGUID();
		local   kTargetPilot = g_pilotMan:FindPilot(kTargetGUID);
		if kTargetPilot:IsNil() == false then
			if actor:IsMyActor() then
				if( CheckNil(g_world == nil) ) then return false end
				if( CheckNil(g_world:IsNil()) ) then return false end
				g_world:AddDrawActorFilter(kTargetGUID);
			end
		end
		i = i + 1;
	end	
	action:ClearTargetList();	
	action:SetParamInt(21, action:GetSkillRange(0,actor)) -- 이펙트 범위		
	actor:SetCanHit(true);
	
	local kMulptiple = action:GetScriptParam("MULTIPLE_ATTACK")
	if kMulptiple~="" then
		action:SetParamInt(100, 1)
		action:SetParamInt(10,1) -- Timer 사용시 CreateTargetList를 1번만 사용하게 하기 위한 값
	end
	
	return true
end

function Skill_Lighting_Storm_OnUpdate(actor, accumTime, frameTime)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	local action = actor:GetAction()
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());

	actor:LockBidirection(false);
	actor:SeeFront(true);
		
	local animDone = actor:IsAnimationDone()
	local nextActionName = action:GetNextActionName()
	local iSlot = action:GetCurrentSlot();

	if action:GetParam(1) == "end" then
		return	false;
	end

	if animDone == true then
		if 1 == iSlot then
			actor:PlayNext();
		end
	end

	local kHitEnd = action:GetParamInt(33);
	if 1 == kHitEnd then
		local curActionParam = action:GetParam(0)
		if curActionParam == "GoNext" then
			action:SetParam(0, "null")
			action:SetParam(1, "end")
			return false;
		else
			action:SetParam(1, "end")
			return false
		end
	end		
	
	return true
end

function Skill_Lighting_Storm_OnCleanUp(actor)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
    local   kCurAction = actor:GetAction();

	SkillHelpFunc_RestoreWeapon(actor, kCurAction)					-- 무기를 변경했으면 복구하고	
	SkillHelpFunc_ShowWeapon(actor, kCurAction)						-- 무기가 감춰져있으면 복구하고	
	SkillHelpFunc_DetachWeaponTrail(actor, kCurAction)				-- 무기 꼬리 파티클 제거	
    SkillHelpFunc_RestoreShape(actor, kCurAction)					-- 모습 원래대로	
	SkillHelpFunc_DetachFireEffectWhenSkillEnd(actor, kCurAction)	-- 스킬이 끝날때 공격 동작 파티클 제거	
	SkillHelpFunc_DetachCastingEffectWhenSkillEnd(actor, kCurAction)-- 스킬이 끝날때 캐스팅 이펙트 제거	
	SkillHelpFunc_ExpressCleanUpEffect(actor, kCurAction)			-- CleanUp시(스킬 종료시) 표현할 파티클이 있으면 붙여주고	
	SkillHelpFunc_IfCastingCancelThenDoBattleIdle(actor)			-- 캐스팅 하다 캔슬 되었으면 공격 대기 상태로 바꿈	
	SkillHelpFunc_DetachHitCasterEffect(actor)						-- 때릴때 캐스터에게 붙은 파티클 제거

	return true;
end

function Skill_Lighting_Storm_OnLeave(actor, action)

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
	
	if curAction:GetNextActionName() ~= actionID then
		ODS("다른게 들어옴:"..actionID.."\n",false, 912)
		return false;
	end
	
	if action:GetActionType()=="EFFECT" then
		if actor:IsMyActor() then
			if( CheckNil(g_world == nil) ) then return true end
			if( CheckNil(g_world:IsNil()) ) then return true end
			g_world:SetShowWorldFocusFilterColorAlpha(0xFFFFFF, g_world:GetWorldFocusFilterAlpha(), 0, 1, false,false);
			g_world:ClearDrawActorFilter()
		end
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
		return false;
	end
	
	return false 
end

function Skill_Lighting_Storm_OnTargetListModified(actor,action,bIsBefore)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
	if(0 == action:GetParamInt(100) ) then
		if bIsBefore == false then

			--  hit 키 이후라면
			--  바로 대미지 적용시켜준다.
			SkillHelpFunc_DefaultHitOneTime(actor,action);	 
		end
	end

end

function Skill_Lighting_Storm_OnEvent(actor,textKey)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
		
	local kAction = actor:GetAction();
		
	CheckNil(nil==kAction);
	CheckNil(kAction:IsNil());

	
	if kAction:GetActionParam() == AP_CASTING then
		return true;
	end
	
	if textKey == "hit" or textKey == "fire" then

		local iTimerTime = kAction:GetScriptParamAsFloat("HIT_TIMER_TIME");
		local iTimerick = kAction:GetScriptParamAsFloat("HIT_TIMER_TICK");
		kAction:StartTimer(iTimerTime, iTimerick,0);
		
		if actor:IsUnderMyControl() then
			QuakeCamera(5.5, 2, 1, 1, 1);
		end

		kAction:StartTimer(4.0, 0.1,1); -- 타이머 아이디가 1
		kAction:StartTimer(4.0, 0.3,2); -- 타이머 아이디가 2
		
		actor:AttachParticleS(7218, "p_ef_heart","ef_skill_raceofheaven_01_p_ef_r_hand", 5.0);
	end

	return	true;
end
