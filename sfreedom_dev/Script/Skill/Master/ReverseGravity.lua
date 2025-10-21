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
-- [HIT_TARGET_EFFECT_TARGET_NODE_TO_GROUND] : HIT 키에서 타겟에게 이펙트를 붙일때, 바닥에 붙여준다.
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
-- [QUAKE_TIME] : 화면 흔들 시간
-- [QUAKE_POWER] : 화면 흔들 양
-- Melee
function Skill_Reverse_Gravity_OnCheckCanEnter(actor,action)

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

function Skill_Reverse_Gravity_OnCastingCompleted(actor, action)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
	Skill_Reverse_Gravity_Fire(actor,action);
	action:SetSlot(action:GetCurrentSlot()+1);
	actor:PlayCurrentSlot();
	action:SetParam(3,"0");
end

function Skill_Reverse_Gravity_Fire(actor,action)
	ODS("RestoreItemModel Skill_Reverse_Gravity_Fire\n");
	
	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
	local	kSoundID = action:GetScriptParam("FIRE_SOUND_ID");
	if kSoundID~="" then
		actor:AttachSound(2783,kSoundID);
	end	
	
	--	무기 바꾸기 처리
	if action:GetScriptParam("WEAPON_XML") ~= "" then
		actor:ChangeItemModel(ITEM_WEAPON, action:GetScriptParam("WEAPON_XML"));
		ODS("ChangeItemModel "..action:GetScriptParam("WEAPON_XML").."\n");
	end
	
	if action:GetScriptParam("HIDE_WEAPON") == "TRUE" then
		actor:HideParts(EQUIP_POS_WEAPON, true);
	end

	actor:AttachParticleToPointS(7212,actor:GetNodeWorldPos("char_root"),"ef_Reversegravity_01_char_root",1.3);
	actor:AttachParticleS(7213,"char_root","ef_Reversegravity_02_char_root",2.5);

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

	local iTotalHit = 0;
	local kSkillDef = GetSkillDef(action:GetActionNo());
	if kSkillDef:IsNil() == false then
		iTotalHit  = kSkillDef:GetAbil(AT_COUNT);
	end

	action:SetParamInt(8,0);
	action:SetParamInt(9,0)	--	Hit Count
	action:SetParamInt(30,iTotalHit);
	action:SetParamInt(31,0); -- hit_timer를 사용하는지 Check

	action:SetParamInt(20,0);
end

function Skill_Reverse_Gravity_OnEnter(actor, action)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());

	local actionID = action:GetID()
	
	ODS("Skill_Reverse_Gravity_OnEnter actionID:"..actionID.." GetActionParam:"..action:GetActionParam().."\n");

	action:SetParam(3,"0");
	local   kFireEffectID = action:GetScriptParam("CASTING_EFFECT_EFFECT_ID");
    local   kFireEffectTargetNodeID = action:GetScriptParam("CASTING_EFFECT_TARGET_NODE");
	local   kFireEffectTargetNodeIDToPos = action:GetScriptParam("CASTING_EFFECT_TARGET_NODE_TO_WORLD_POS");
    
    if kFireEffectID ~= "" and kFireEffectTargetNodeID ~= "" or kFireEffectTargetNodeIDToPos ~= "" then
    
		local iSize = 1.0
		if "TRUE" == action:GetScriptParam("CASTING_EFFECT_SIZE_TO_SKILLRANGE") then
			iSize = iSize * (action:GetSkillRange(0,actor) / 100)
			iSize = iSize * 2 
		end
		
        if kFireEffectTargetNodeID == "__FRONT_ACTOR" then
            
            local   kDir = actor:GetLookingDir();
            local   fDistance = 30;
            kDir:Multiply(fDistance);
            kDir:Add(actor:GetPos());
            kDir:SetZ(kDir:GetZ()-28);
            
            actor:AttachParticleToPointS(7215,kDir,kFireEffectID, iSize);
            
        elseif kFireEffectTargetNodeID ~= "" then
            actor:AttachParticleS(7215,kFireEffectTargetNodeID,kFireEffectID,iSize);
		else
			actor:AttachParticleToPointS(7215,actor:GetNodeWorldPos(kFireEffectTargetNodeIDToPos),kFireEffectID,iSize);
        end
		action:SetParam(3,"1");
    end	
	
	local	kSoundID = action:GetScriptParam("CASTING_SOUND_ID");
	if kSoundID~="" then
		actor:AttachSound(2785,kSoundID);
	end	
	
	action:SetParamInt(2,0);
	
	if action:GetActionParam() == AP_CASTING then	--	캐스팅이라면 그냥 리턴하자.
		return	true;
	end

	Skill_Reverse_Gravity_OnCastingCompleted(actor,action);

	if actor:IsMyActor() then
		if( CheckNil(g_world == nil) ) then return false end
		if( CheckNil(g_world:IsNil()) ) then return false end
		g_world:SetShowWorldFocusFilterColorAlpha(0x000000, 0.0, 1.0, 0.4,true,true);
		g_world:AddDrawActorFilter(actor:GetPilotGuid());
	end

	action:CreateActionTargetList(actor,true);

	if( CheckNil(g_world == nil) ) then return false end
	if( CheckNil(g_world:IsNil()) ) then return false end
	local i = 0;
	local kTargetList = action:GetTargetList();
	local iTargetCount = kTargetList:size();
	while i<iTargetCount do
		local   kTargetInfo = kTargetList:GetTargetInfo(i);
		local   kTargetGUID = kTargetInfo:GetTargetGUID();
		local   kTargetPilot = g_pilotMan:FindPilot(kTargetGUID);
		if kTargetPilot:IsNil() == false then
			if actor:IsMyActor() then
				g_world:AddDrawActorFilter(kTargetGUID);
			end
		end
		i = i + 1;
	end

	action:ClearTargetList();
	
	action:SetParamInt(21, action:GetSkillRange(0,actor)) -- 이펙트 범위
	
	if actor:IsMyActor() then
		QuakeCamera(4, 1, 1, 1, 1);
	end
	
	--남성용 사운드
	if 1 == actor:GetAbil(AT_GENDER) then
		local	kSoundID2 = action:GetScriptParam("CASTING_SOUND_ID_MALE");
		if kSoundID2~="" then
			actor:AttachSound(2786,kSoundID2);
			ODS("남자사운드\n", false, 912)
		end	
	--여성용 사운드
	else
		local	kSoundID2 = action:GetScriptParam("CASTING_SOUND_ID_FEMALE");
		if kSoundID2~="" then
			actor:AttachSound(2786,kSoundID2);
			ODS("여자사운드\n", false, 912)
		end	
	end

	actor:SetCanHit(false);
	
	return true
end

function Skill_Reverse_Gravity_OnUpdate(actor, accumTime, frameTime)

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
		
	return	true;
end

function Skill_Reverse_Gravity_OnCleanUp(actor)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
    local   kCurAction = actor:GetAction();
	
	ODS("RestoreItemModel Skill_Reverse_Gravity_ReallyLeave\n");
	--	무기 원래대로
	if kCurAction:GetScriptParam("WEAPON_XML") ~= "" then
		actor:RestoreItemModel(ITEM_WEAPON);
	end

	if kCurAction:GetScriptParam("HIDE_WEAPON") == "TRUE" then
		actor:HideParts(EQUIP_POS_WEAPON, false);
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
	
	if kCurAction:GetScriptParam("DETACH_CASTING_EFFECT_AT_FINISH") == "TRUE" then
		local DefaultDetaChID = false;
		if kCurAction:GetScriptParam("CASTING_EFFECT_ID_DEFAULT_DETACH") == "TRUE" then
			DefaultDetaChID = true;
		end
		actor:DetachFrom(7215,DefaultDetaChID)
	end
		
	local   kCleanUpEffectID = kCurAction:GetScriptParam("CLEANUP_CASTER_EFFECT_ID");
    local   kCleanUpEffectTargetNodeID = kCurAction:GetScriptParam("CLEANUP_CASTER_EFFECT_TARGET_NODE");
	local	fCleanUpScale = tonumber(kCurAction:GetScriptParam("CLEANUP_CASTER_EFFECT_SCALE"));

	if fCleanUpScale == 0 or fCleanUpScale == nil then
		fCleanUpScale = 1.0;
	end
    
    if kCleanUpEffectID ~= "" and kCleanUpEffectTargetNodeID ~= "" then
		 actor:AttachParticleS(7216,kCleanUpEffectTargetNodeID,kCleanUpEffectID, fCleanUpScale);
	end

	if "1" == kCurAction:GetParam(3) then
		--캐스팅 중에 캔슬된 상태
		actor:DetachFrom(7215,true)
		if( false == actor:PlayNext()) then 
			kCurAction:SetNextActionName("a_battle_idle");			
		end
		kCurAction:ChangeToNextActionOnNextUpdate(true,true);
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

function Skill_Reverse_Gravity_OnLeave(actor, action)

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
--		ODS("다른게 들어옴:"..actionID.."\n",false, 912)
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

function Skill_Reverse_Gravity_DoDamage(actor,actorTarget,kActionResult,kActionTargetInfo)
	
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
	local   kHitTargetEffectTargetNodeIDToGround = kAction:GetScriptParam("HIT_TARGET_EFFECT_TARGET_NODE_TO_GROUND");
    
    if kHitTargetEffectID ~= "" and kHitTargetEffectTargetNodeID ~= "" or kHitTargetEffectTargetNodeIDToGround ~= "" then
		if kHitTargetEffectTargetNodeID ~= "" then
			actorTarget:AttachParticle(7211,kHitTargetEffectTargetNodeID,kHitTargetEffectID);
		elseif kHitTargetEffectTargetNodeIDToGround == "TRUE" then
    			local kPos = actorTarget:GetPos();
			kPos:SetZ(kPos:GetZ() + 200);
			if( CheckNil(g_world == nil) ) then return false end
			if( CheckNil(g_world:IsNil()) ) then return false end
			local ptcl = g_world:ThrowRay(kPos, Point3(0,0,-1),500);
			if ptcl:GetX() == -1 and ptcl:GetY() == -1 and ptcl:GetZ() == -1 then
				--위로도 쏴 본다.
				ptcl = g_world:ThrowRay(kPos, Point3(0,0,1),500);
				if ptcl:GetX() == -1 and ptcl:GetY() == -1 and ptcl:GetZ() == -1 then
					return false;
				end			
			end

			actor:AttachParticleToPoint(100 + kAction:GetParamInt(8), ptcl, kHitTargetEffectID);
	        
			kAction:SetParamInt(8,kAction:GetParamInt(8) + 1);

			actorTarget:AttachParticleToPointS(200, ptcl, "ef_Reversegravity_02_char_root", 0.5);
			--actor:AttachParticleS(7213,"char_root","ef_Reversegravity_02_char_root",2.5);
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
	end
end

function Skill_Reverse_Gravity_HitOneTime(actor,action)
	
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
					Skill_Reverse_Gravity_DoDamage(actor,actorTarget,actionResult, action:GetTargetInfo(i));
				end
				
			end
			
			i=i+1;
		
		end
	end

	action:GetTargetList():ApplyActionEffects();
	
end
function Skill_Reverse_Gravity_OnTargetListModified(actor,action,bIsBefore)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());	
	
    if bIsBefore == false then

		--  hit 키 이후라면
        --  바로 대미지 적용시켜준다.
        Skill_Reverse_Gravity_HitOneTime(actor,action);

        if actor:IsMyActor()  then
			action:GetTargetList():ApplyActionEffects(true,true);
			return;
		end       
 
    end
end

function Skill_Reverse_Gravity_OnEvent(actor,textKey)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
		
	local kAction = actor:GetAction();
		
	CheckNil(nil==kAction);
	CheckNil(kAction:IsNil());

	
	if kAction:GetActionParam() == AP_CASTING then
		return true;
	end
	
	if textKey == "hit" or textKey == "fire" then

		if actor:IsMyActor() then
			local iQuakeTime = kAction:GetScriptParamAsFloat("QUAKE_TIME");
			local iQuakePower = kAction:GetScriptParamAsFloat("QUAKE_POWER");
			if 0 < iQuakeTime and 0 < iQuakePower then
				QuakeCamera(iQuakeTime, iQuakePower, 1, 1, 1);
			end
		end

		if actor:IsUnderMyControl() then
			if 0 == kAction:GetParamInt(31) then -- hit이 여러번 있을 수 있기 때문에 처음 한번만 발동 시켜줘야 한다.
				kAction:CreateActionTargetList(actor);

				local i = 0;
				local kTargetList = kAction:GetTargetList();
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
		        
					Skill_Reverse_Gravity_HitOneTime(actor,kAction)
					Skill_Reverse_Gravity_OnTargetListModified(actor,kAction,false)
		            
					return;
				else
					kAction:BroadCastTargetListModify(actor:GetPilot());
					kAction:ClearTargetList();
				end

				kAction:SetParamInt(31,1);
			end
		end
	end

	return	true;
end
