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
-- [FIRE_CASTER_EFFECT_SCALE_NORMALIZE] : 스킬이 실제 발동될때 캐스터에게 이펙트의 크기를 실제 범위와 맞추기 위해 리소스 이펙트를 보정하는값
-- [FIRE_CASTER_EFFECT_ID2] : 스킬이 실제 발동될때 캐스터에게 붙일 이펙트 ID
-- [FIRE_CASTER_EFFECT_ID_DEFAULT_DETACH] : 스킬이 떨어질때 기본 Detach 옵션을 사용할것인가
-- [FIRE_CASTER_EFFECT_ID2_DEFAULT_DETACH] : 스킬이 떨어질때 기본 Detach 옵션을 사용할것인가
-- [FFIRE_CASTER_EFFECT_SCALE_NORMALIZE2] : 스킬이 실제 발동될때 캐스터에게 이펙트의 크기를 실제 범위와 맞추기 위해 리소스 이펙트를 보정하는값
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
-- [ROTATION_ANGLE_VALUE_TICK] : tick당 회전할 각도
-- [MOVING_ATTACK_TICK] : 이동공격시 공격할 틱 없으면 디폴트값 0.3
-- [PROJECTILE_ID] : 발사할 발사체 ID, "" 일 경우 무기에 붙어있는 발사체 ID 를 사용한다.
-- [ATTACH_NODE_ID] : 발사체를 붙일 액터의 노드 ID, "" 일 경우 캐릭터 좌표에서 발사된다.
-- [HIT_RUN_STOP] : hit 시 이동 정지

-- Melee
function Act_Pet_Melee_Default_OnCheckCanEnter(actor, kNextAction)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==kNextAction);
	CheckNil(kNextAction:IsNil());
	
	if("USING" == actor:GetParam("SKILL_STATE")) then
		--ODS("스킬 사용중이라 다른 스킬 못씀\n",false, 912)
		return false
	end
	
	local kMyPilot = actor:GetPilot()
	if kMyPilot:IsNil() then return false end
	local kMyUnit = kMyPilot:GetUnit()
	if kMyUnit:IsNil() then return false end
	local kCallerPilot = g_pilotMan:FindPilot(kMyUnit:GetCaller())
	if kCallerPilot:IsNil() then return false end
	local kCallerUnit = kCallerPilot:GetUnit()
	if kCallerUnit:IsNil() then return false end
	if(true == kCallerUnit:IsDead()) then return false end
	--	local weapontype = actor:GetEquippedWeaponType();	
	--	if weapontype == 0 then
	--		return	false;
	--	end

	local kCallerActor = kCallerPilot:GetActor()
	if kCallerActor:IsNil() then return true end
	local kPlayerPos = kCallerActor:GetPos()
	local kPetPosToPlayer = kPlayerPos:_Subtract(actor:GetPos())
	local fDist = kPetPosToPlayer:Length()
	--ODS("거리:"..fDist.."\n",false, 912)
	if(200 < fDist) then --스킬을 쓰기에 너무 멀다		
		return false
	end

	return true
end

function Act_Pet_Melee_Default_OnCastingCompleted(actor, action)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
	Act_Pet_Melee_Default_Fire(actor,action);
	action:SetSlot(action:GetCurrentSlot()+1);
	actor:PlayCurrentSlot();
	action:SetParam(3,"0");

end

function Act_Pet_Melee_Default_Fire(actor,action)
	
	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
	local	bUseTLM = (action:GetScriptParam("NO_USE_TLM") ~= "TRUE");		
	
	if bUseTLM == false then
		action:CreateActionTargetList(actor);
	end

	local	kSoundID = action:GetScriptParam("FIRE_SOUND_ID");
	if kSoundID~="" then
		actor:AttachSound(2783,kSoundID);
	end	
	
	--	무기 바꾸기 처리
	if action:GetScriptParam("WEAPON_XML") ~= "" then
		actor:ChangeItemModel(ITEM_WEAPON, action:GetScriptParam("WEAPON_XML"));
		--ODS("ChangeItemModel "..action:GetScriptParam("WEAPON_XML").."\n");
	end
	
	if action:GetScriptParam("HIDE_WEAPON") == "TRUE" then
		actor:HideParts(EQUIP_POS_WEAPON, true);
	end

    local   kFireEffectID = action:GetScriptParam("FIRE_CASTER_EFFECT_ID");
    local   kFireEffectTargetNodeID = action:GetScriptParam("FIRE_CASTER_EFFECT_TARGET_NODE");
	local   kFireEffectTargetNodeIDToPos = action:GetScriptParam("FIRE_CASTER_EFFECT_TARGET_NODE_TO_WORLD_POS");
	local   kFireEffectTargetNodeIDToPosWithRotate = action:GetScriptParam("FIRE_CASTER_EFFECT_TARGET_NODE_TO_WORLD_POS_WITH_ROTATE");
		
	local fEffectScale = 1
	local   kFireEffectNomalize = action:GetScriptParam("FIRE_CASTER_EFFECT_SCALE_NORMALIZE");	
	if("" ~= kFireEffectNomalize) then
		local iAttackRange = action:GetSkillRange(0,actor);
		kFireEffectNomalize = tonumber(kFireEffectNomalize)
		fEffectScale = iAttackRange/kFireEffectNomalize
	end
	
    if kFireEffectID ~= "" and kFireEffectTargetNodeID ~= "" or kFireEffectTargetNodeIDToPos ~= ""  or kFireEffectTargetNodeIDToPosWithRotate ~= "" then
    
        if kFireEffectTargetNodeID == "__FRONT_ACTOR" then
            
            local   kDir = actor:GetLookingDir();
            local   fDistance = 30;
            kDir:Multiply(fDistance);
            kDir:Add(actor:GetPos());
            kDir:SetZ(kDir:GetZ()-28);
            
            actor:AttachParticleToPoint(7212,kDir,kFireEffectID);            
        elseif kFireEffectTargetNodeID ~= "" then
            actor:AttachParticleS(7212,kFireEffectTargetNodeID,kFireEffectID, fEffectScale);
		elseif kFireEffectTargetNodeIDToPos ~= "" then
			actor:AttachParticleToPointS(7212,actor:GetNodeWorldPos(kFireEffectTargetNodeIDToPos),kFireEffectID,fEffectScale);			
		else
		    actor:AttachParticleToPointWithRotate(7212,actor:GetNodeWorldPos(kFireEffectTargetNodeIDToPosWithRotate), kFireEffectID, actor:GetRotateQuaternion(),fEffectScale)
        end
    
    end	

    kFireEffectID = action:GetScriptParam("FIRE_CASTER_EFFECT_ID2");
    kFireEffectTargetNodeID = action:GetScriptParam("FIRE_CASTER_EFFECT_TARGET_NODE2");
	fEffectScale = 1
	kFireEffectNomalize = action:GetScriptParam("FIRE_CASTER_EFFECT_SCALE_NORMALIZE2");	
	if("" ~= kFireEffectNomalize) then
		local iAttackRange = action:GetSkillRange(0,actor);
		kFireEffectNomalize = tonumber(kFireEffectNomalize)
		fEffectScale = iAttackRange/kFireEffectNomalize		
	end
	
    if kFireEffectID ~= "" and kFireEffectTargetNodeID ~= "" then
    
        if kFireEffectTargetNodeID == "__FRONT_ACTOR" then
            
            local   kDir = actor:GetLookingDir();
            local   fDistance = 30;
            kDir:Multiply(fDistance);
            kDir:Add(actor:GetPos());
            kDir:SetZ(kDir:GetZ()-28);
            
            actor:AttachParticleToPointS(7213,kDir,kFireEffectID,kFireEffectNomalize);
            
        else
            actor:AttachParticleS(7213,kFireEffectTargetNodeID,kFireEffectID, fEffectScale,kFireEffectNomalize);
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
	
	-- 스킬 시전시 Player 위치로 이동한다.	
	local kMyPilot = actor:GetPilot()
	if kMyPilot:IsNil() then return true end
	local kMyUnit = kMyPilot:GetUnit()
	if kMyUnit:IsNil() then return true end
	local kCallerPilot = g_pilotMan:FindPilot(kMyUnit:GetCaller())
	if kCallerPilot:IsNil() then return true end
	local kCallerActor = kCallerPilot:GetActor()
	if kCallerActor:IsNil() then return true end
	local kPlayerPos = kCallerActor:GetPos()
	--Pos
	local kMoveDir = kCallerActor:GetMovingDir()
	local kTarget = kMyPilot:GetTarget()

	if 0~=actor:GetAbil(AT_AUTO_PET_SKILL) and false==kTarget:IsNil() then
		if( CheckNil(g_world == nil) ) then return false end
		if( CheckNil(g_world:IsNil()) ) then return false end
		local kTargetActor = g_world:FindActor(kTarget)
		if false==kTargetActor:IsNil() then
			local kTargetPos = kTargetActor:GetPos()
			actor:LookAt(kTargetPos, true)
--			ODS("타겟방향으로 쐇음 "..action:GetID().." X : "..kTargetPos:GetX().." Y : "..kTargetPos:GetY().."\n", false, 912)
			kMoveDir = kTargetPos:_Subtract(actor:GetPos())
			kMoveDir:Unitize()
--			ODS("\nDIr X : "..kMoveDir:GetX().." Y : "..kMoveDir:GetY().." ActorPos X : "..actor:GetPos():GetX().." Y : "..actor:GetPos():GetY().."\n\n", false, 912)
		end
	end
	--actor:SetTraceFlyTargetLoc( kPlayerPos )
	actor:SetMovingDir(kMoveDir);
	actor:FindPathNormal();

	action:SetParamFloat(15, kMoveDir:GetX());
	action:SetParamFloat(16, kMoveDir:GetY());
	action:SetParamFloat(17, kMoveDir:GetZ());
	action:SetParamFloat(18, tonumber(action:GetScriptParam("ROTATION_ANGLE_VALUE_TICK")));
	
	local fAttackTick = 0.3;

	if nil ~= tonumber(action:GetScriptParam("MOVING_ATTACK_TICK")) then
		fAttackTick = tonumber(action:GetScriptParam("MOVING_ATTACK_TICK"))
	end

	action:SetParamAsPoint(7, actor:GetPos());
	action:SetParamFloat(8, timeGetTime());

	local kSkillDef = GetSkillDef(action:GetActionNo());

	action:SetParamFloat(9, 2000); -- MaxMoveTime
	if false == kSkillDef:IsNil() then
		local iMoveSpeed = kSkillDef:GetAbil(AT_MOVESPEED);
		local iMaxDistance = action:GetParamInt(30)
		action:SetParamFloat(9, iMaxDistance / iMoveSpeed * 1000); -- MaxMoveTime
		if 0 < iMoveSpeed and 0 < iMaxDistance then
			if actor:IsUnderMyControl() then
				if 0.0 < fAttackTick then
					action:StartTimer(iMaxDistance / iMoveSpeed, fAttackTick, 0);
					if 0 == action:GetParamInt(20) then
						local iMaxCount = (iMaxDistance / iMoveSpeed) / fAttackTick;
						action:SetParamInt(iMaxCount);
					end
				end
			end
		end
	end

	if actor:IsUnderMyControl() then
		if (nil ~= string.find(action:GetID(), "a_blizzard_pet")) then
			--action:GetID() == "a_blizzard_pet_a" or action:GetID() == "a_blizzard_pet_b" or action:GetID() == "a_blizzard_pet_c" then			
			local iTotalProjectile = 0;

			local	kSkillDef = GetSkillDef(action:GetActionNo());
			if kSkillDef:IsNil() == false then
				iTotalProjectile = kSkillDef:GetAbil(AT_PROJECTILE_NUM);
			end

			action:SetParamInt(100, iTotalProjectile);
			
			local kPos = actor:GetNodeWorldPos("char_root");

			local fRange = 75;

			local i = 0;

			-- 2개의 좌표를 저장한다. 시작 / 끝
			iTotalProjectile = iTotalProjectile * 2;
			while i < iTotalProjectile do
				local fRandomX = math.random(-fRange,fRange);
				local fRandomY = math.random(-fRange,fRange);

				if 2 > iTotalProjectile then -- 발사체 개수가 1개이면 랜덤 사용안함
					fRandomX = 0;
					fRandomY = 0;
				end
				local kStartPos = Point3(kPos:GetX() + fRandomX,
										 kPos:GetY() + fRandomY,
										 kPos:GetZ() + 150);
								
				local kEndPos = Point3(kStartPos:GetX(), kStartPos:GetY(), kPos:GetZ());
				
				action:SetParamAsPoint(101 + i, kStartPos);
				action:SetParamAsPoint(101 + i + 1, kEndPos);

				i=i+2;
			end			

		elseif (nil ~= string.find(action:GetID(), "a_meteor_pet")) then 
			--action:GetID() == "a_meteor_pet_a" or action:GetID() == "a_meteor_pet_b" or action:GetID() == "a_meteor_pet_c" then

			-- 메테오는 한발이다.. 
			-- 지금은 타겟팅 된 적 중 하나의 위치에 떨어지도록 되어있음.
			-- 여러발이라면 새로운 규칙이 되어야 하므로 밑에 코드를 다시 작성해주어야한다.
			
			local iTotalProjectile = 1; 
			action:SetParamInt(100, iTotalProjectile);
			
			local kPos = actor:GetNodeWorldPos("char_root");

			local fRange = 75;

			local i = 0;
			local j = 0;

			-- 사정거리 안의 적의 위치를 타겟팅
			action:CreateActionTargetList(actor, false);	-- 임시적으로 사용하기 위한 타게팅
			local kTargetList = action:GetTargetList();
			local iTargetCount = kTargetList:size();
			local findTarget = false;
			local i=0;
			local j=0;

			local kStartPos;
			local kEndPos;
			if iTargetCount > 0 then
				while i < iTargetCount do
					local targetInfo = kTargetList:GetTargetInfo(i);
					local targetGUID = targetInfo:GetTargetGUID();
					local targetPilot = g_pilotMan:FindPilot(targetGUID);
					if targetPilot:IsNil() == false then
						local targetActor = targetPilot:GetActor();
						if targetActor:IsNil() == false then
							local targetPos = targetActor:GetPos();

							kStartPos = Point3(targetPos:GetX(),
										 targetPos:GetY(),
										 targetPos:GetZ() + 150);
								
							kEndPos = Point3(kStartPos:GetX(), kStartPos:GetY(), targetPos:GetZ());						

							break;
						end
					end

					i = i + 1;
				end

				if (nil ~= string.find(action:GetID(), "a_meteor_pet")) then 
					--action:GetID() == "a_meteor_pet_a" or action:GetID() == "a_meteor_pet_b" or action:GetID() == "a_meteor_pet_c" then -- 메테오 비스듬하게 떨어지게
					local kMovingDir = actor:GetLookingDir();
					local iAttackRange = action:GetSkillRange(0,actor);
					iAttackRange = iAttackRange * 0.7; -- 2/3 정도 기울기에서 떨어지게
					kMovingDir:Multiply(iAttackRange);
					kStartPos:Add(kMovingDir);
				end

				action:SetParamAsPoint(101 + j, kStartPos);
				action:SetParamAsPoint(101 + j + 1, kEndPos);

				i=i+2;
			else -- 타겟이 잡히지 않았으면 사정거리 위치에 떨어뜨린다.
				local kPos = actor:GetNodeWorldPos("char_root");
				local kMovingDir = actor:GetLookingDir();
				local iAttackRange = action:GetSkillRange(0,actor);
				kMovingDir:Multiply(iAttackRange);
				kPos:Add(kMovingDir);

				local kStartPos = Point3(kPos:GetX(), kPos:GetY(),kPos:GetZ() + 150);
				local kEndPos = Point3(kStartPos:GetX(), kStartPos:GetY(), kPos:GetZ());

				action:SetParamAsPoint(101, kStartPos);
				action:SetParamAsPoint(102, kEndPos);
			end
			action:ClearTargetList();	-- 임시적으로 사용한것 타게팅 클리어
			
		elseif (nil ~= string.find(action:GetID(), "a_bite_off")) or (nil ~= string.find(action:GetID(), "a_body_bump")) then 
			--action:GetID() == "a_bite_off" or action:GetID() == "a_body_bump_a" or action:GetID() == "a_body_bump_b" or action:GetID() == "a_body_bump_c"  then
			-- 물어뜯기, 몸통 박치기
			action:SetParamAsPoint(7, actor:GetPos())
			
			local kStartPos = actor:GetPos()
			action:CreateActionTargetList(actor, false);
			local kTargetList = action:GetTargetList();
			local iTargetCount = kTargetList:size();
			local findTarget = false;
			local i=0;
			local kEndPos = actor:GetPos()
			if iTargetCount > 0 then
				while i < iTargetCount do
					local targetInfo = kTargetList:GetTargetInfo(i);
					local targetGUID = targetInfo:GetTargetGUID();
					local targetPilot = g_pilotMan:FindPilot(targetGUID);
					if targetPilot:IsNil() == false then
						local targetActor = targetPilot:GetActor();
						if targetActor:IsNil() == false then
							kEndPos = targetActor:GetPos();	
						end
					end
					i = i + 1;
				end
				
				action:SetParamAsPoint(500, kEndPos); -- 달려가서 멈출 위치
				--ODS("타겟이 잡힘 저장했음:"..kEndPos:GetX()..","..kEndPos:GetY()..","..kEndPos:GetZ().."\n", false, 912)
			else
				--ODS("타겟이 안잡혔음\n", false, 912)
			end
			action:ClearTargetList();			
		end
	else -- 타인일 경우
		local kPacket = action:GetParamAsPacket()
		if nil==kPacket or kPacket:IsNil() then
			return false
		end
		--ODS("패킷 받으러 들어왔음\n", false, 912)
		
		if (nil ~= string.find(action:GetID(), "a_bite_off")) or (nil ~= string.find(action:GetID(), "a_body_bump"))
			--action:GetID() == "a_bite_off" or action:GetID() == "a_body_bump_a" or action:GetID() == "a_body_bump_b" or action:GetID() == "a_body_bump_c" -- 몸통 박치기
		then
			action:SetParamAsPoint(500, kPacket:PopPoint3());		-- 달려가서 멈출 위치
			
			local kEndPos = action:GetParamAsPoint(500)
			actor:LookAt(kEndPos, true)
			--ODS("패킷 받은것:"..kEndPos:GetX()..","..kEndPos:GetY()..","..kEndPos:GetZ().."\n", false, 912)
		else   
			local iTotalProjectile = kPacket:PopInt();
			action:SetParamInt(100, iTotalProjectile);

			local i = 0;
			-- 2개의 좌표를 저장한다. 시작 / 끝
			iTotalProjectile = iTotalProjectile * 2;
			while i < iTotalProjectile do

				action:SetParamAsPoint(101 + i, kPacket:PopPoint3());		-- 시작 위치
				action:SetParamAsPoint(101 + i + 1, kPacket:PopPoint3());	-- 끝 위치 	

				i=i+2;	-- 두개가 한 묶음이니까
			end
		end
		
	end
end

function Act_Pet_Melee_Default_OnOverridePacket(actor, action, packet)

	if (nil ~= string.find(action:GetID(), "a_blizzard_pet")) or (nil ~= string.find(action:GetID(), "a_meteor_pet")) then
		--action:GetID() == "a_blizzard_pet_a" or action:GetID() == "a_blizzard_pet_b" or action:GetID() == "a_blizzard_pet_c" 
		--or action:GetID() == "a_meteor_pet_a" or action:GetID() == "a_meteor_pet_b" or action:GetID() == "a_meteor_pet_c" then
		
		local iTotalProjectile = action:GetParamInt(100)
		packet:PushInt(iTotalProjectile)
		
		-- 2개의 좌표를 저장한다. 시작 / 끝
		local i = 0;
		iTotalProjectile = iTotalProjectile * 2;
		while i < iTotalProjectile do
			packet:PushPoint3(action:GetParamAsPoint(101 + i));
			packet:PushPoint3(action:GetParamAsPoint(101 + i + 1));
			i=i+2;
		end	
		
	elseif (nil ~= string.find(action:GetID(), "a_bite_off")) or (nil ~= string.find(action:GetID(), "a_body_bump")) then
		--action:GetID() == "a_bite_off" or action:GetID() == "a_body_bump_a" or action:GetID() == "a_body_bump_b" or action:GetID() == "a_body_bump_c" then
		-- 물어 뜯기, 몸통 박치기
		local kEndPos = action:GetParamAsPoint(500)
		packet:PushPoint3(kEndPos)
		--ODS("타겟 패킷 저장함:"..kEndPos:GetX()..","..kEndPos:GetY()..","..kEndPos:GetZ().."\n", false, 912)
	end
end

function Act_Pet_Melee_Default_OnEnter(actor, action)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
	actor:SetParam("SKILL_STATE", "USING")
	
--	local actorID = actor:GetID()
	local actionID = action:GetID()
--	local weapontype = actor:GetEquippedWeaponType();
	
	ODS("Act_Pet_Melee_Default_OnEnter actionID:"..actionID.." GetActionParam:"..action:GetActionParam().."\n");

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
	
	if action:GetActionParam() == AP_CASTING then	--	캐스팅이라면 그냥 리턴하자.
		return	true;
	end

	action:SetParamInt(2, 0);

	local iTotalHit = tonumber(action:GetScriptParam("MULTI_HIT_NUM"));

	action:SetParamInt(20, iTotalHit); -- MaxHitCount 실제로 때릴 수 있는 Max
	action:SetParamInt(21,0); -- HitCount
	action:SetParamInt(22,0); -- Hit 한 수 DashStop ( 즉 때린 위치에서 멈추기 )

	local iHitRunEnd = 0;
	if action:GetScriptParam("HIT_RUN_STOP") == "TRUE" then
		iHitRunEnd = 1;		
	end

	action:SetParamInt(23,iHitRunEnd); -- 이동후 히트 하도록
	action:SetParamInt(24,0); --  되었을 경우
	action:SetParamInt(25,0); --  목표점까지 이동

	local iMaxDistance = 0;
	local kSkillDef = GetSkillDef(action:GetActionNo());
	if false == kSkillDef:IsNil() then
		iMaxDistance = kSkillDef:GetAbil(AT_DISTANCE);		
	end	

	action:SetParamInt(30, iMaxDistance); -- Max Distance
	
	Act_Pet_Melee_Default_OnCastingCompleted(actor,action);
	
	return true
end

function Act_Pet_Melee_Default_OnUpdate(actor, accumTime, frameTime)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	local action = actor:GetAction()
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
	local animDone = actor:IsAnimationDone()
	local nextActionName = action:GetNextActionName()
	
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

	if 1 == action:GetParamInt(22) and action:GetParamInt(20) == action:GetParamInt(21) then
		return false
	end

	local kSkillDef = GetSkillDef(action:GetActionNo());

	local iRotValue = action:GetParamFloat(18);
	if nil ~= iRotValue and 0 ~= iRotValue then
		actor:IncRotate(frameTime*15*360.0*math.pi/180.0);
	end

	if false == kSkillDef:IsNil() then
		local iMoveSpeed = kSkillDef:GetAbil(AT_MOVESPEED);

		if 0 < iMoveSpeed and 0 == action:GetParamInt(25) then
			local kNewPos = actor:GetPos();
			local kOldPos = action:GetParamAsPoint(7);
			kOldPos:SetZ(0)
			kNewPos:SetZ(0); --Z를 무시하고 계산한다. 떨어지는 것은 중력에 의한 것

			local iMaxDistance = action:GetParamInt(30);
			local iHitRunEnd = action:GetParamInt(23)
			--ODS("GetParamInt(23):"..iHitRunEnd.."\n", false, 912)
			local kMovingDir = nil;
			if 1 == iHitRunEnd then
				local kStartPos = action:GetParamAsPoint(7);
					
				if actor:IsUnderMyControl() then					
					action:CreateActionTargetList(actor, false);
					local kTargetList = action:GetTargetList();
					local iTargetCount = kTargetList:size();
					local findTarget = false;
					local i=0;
					if iTargetCount > 0 then
						while i < iTargetCount do
							local targetInfo = kTargetList:GetTargetInfo(i);
							local targetGUID = targetInfo:GetTargetGUID();
							local targetPilot = g_pilotMan:FindPilot(targetGUID);
							if targetPilot:IsNil() == false then
								local targetActor = targetPilot:GetActor();
								if targetActor:IsNil() == false then
									local targetPos = targetActor:GetPos();
									local distance = kStartPos:Distance(targetPos);
									iMaxDistance = distance;
									-- 이동 중 타겟팅이 되면 타겟팅의 위치가 목표 위치가 되어야 한다.
									ODS("내맞음 Dist:"..distance.."\n", false, 912)
									break;
								end
							end
							i = i + 1;
						end
					end
					action:ClearTargetList();					
				else
					local targetPos = action:GetParamAsPoint(500)
					local distance = kStartPos:Distance(targetPos);
					iMaxDistance = distance;
--					ODS("내아님 Dist:"..distance.."\n", false, 912)
					kMovingDir = targetPos:_Subtract(kStartPos)
					kMovingDir:Unitize()
				end
			end

			action:SetParamInt(30, iMaxDistance); -- Max Distance

			local fDistance = kNewPos:Distance(kOldPos);
			
			--이동한 거리가 있을 경우
			if 0.0 < fDistance then
				--남은 이동 거리
				local fDistance2 = iMaxDistance - fDistance;

				--두 벡터를 이용하여 방향 벡터를 구하고
				--방향 벡터를 이용하여 실제로 이동하는 곳의 위치를 구한다.
				local kDirection = kNewPos:_Subtract(kOldPos);
				kDirection:Unitize();
				kDirection:Multiply(iMoveSpeed * frameTime);
				kDirection:Add(kNewPos);

				--이번 프레임에 이동 하는 거리
				local fDistance3 = kDirection:Distance(kOldPos);

				--이미 최대 대쉬 이동거리 거리를 벗어난 경우
				if iMaxDistance - fDistance3 < 0 then
					--ODS("최대거리 이동했음 \n",false, 912)
					action:SetParamInt(25,1)					
					return Act_Pet_Melee_Default_PlayNextAction(actor,action,iHitRunEnd);
				end

				if action:GetParamFloat(9) < timeGetTime() - action:GetParamFloat(8) then
					--ODS("최대거리 이동 시간 초과 \n",false, 912)
					action:SetParamInt(25,1)
					return Act_Pet_Melee_Default_PlayNextAction(actor,action,iHitRunEnd);
				end
			end

			local fAnimSpeed = iMoveSpeed / 150;			
			actor:SetAnimSpeed(fAnimSpeed);
			
			if nil ~= iRotValue and 0 ~= iRotValue then
--				ODS("이동방향을 저장된걸로 씀\n", false, 912)
				kMovingDir = Point3(action:GetParamFloat(15),action:GetParamFloat(16),action:GetParamFloat(17));
			elseif (nil == kMovingDir) then
--				ODS("이동 방향을 바라보는 방향으로씀\n", false, 912)
				kMovingDir = actor:GetLookingDir();
			end
			

			kMovingDir:Multiply(iMoveSpeed);
			
			actor:SetMovingDelta(kMovingDir);
			--ODS("이동중\n",false, 912)
		end
	end	

	return true
end

function Act_Pet_Melee_Default_OnCleanUp(actor)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	actor:SetParam("SKILL_STATE", "END" )
	
    local   kCurAction = actor:GetAction();
	
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
			kCurAction:SetNextActionName("a_trace_idle");			
		end
		kCurAction:ChangeToNextActionOnNextUpdate(true,true);
	end
	
--  모습 원래대로 (이걸 사용한후 curAction을 얻어와 쓰면 절대로 안됨!!!)
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

function Act_Pet_Melee_Default_OnLeave(actor, action)
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
			actor:GetAction():SetNextActionName("a_trace_idle");
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

function Act_Pet_Melee_Default_DoDamage(actor,actorTarget,kActionResult,kActionTargetInfo)
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

function Act_Pet_Melee_Default_HitOneTime(actor,action)
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
		local fHitEffectScale = 1
		if(nil ~= string.find(action:GetID(), "a_bite_off")) then			
			local iSkillLevel = action:GetAbil(AT_LEVEL);
			local fIncScale = 0.05*iSkillLevel	--1LV 75%, 2Lv 80%, 3Lv 85%, 4Lv 90%, 5Lv 100%
			fHitEffectScale = 0.7 + fIncScale
			ODS("Level:"..iSkillLevel.." Scale:"..fHitEffectScale.."\n",false, 912)
			if (1<fHitEffectScale) then fHitEffectScale = 1 end
		end
		
	    if kHitEffectID ~= "" and kHitEffectTargetNodeID ~= "" or kHitEffectTargetNodeIDToPos ~= ""  or kHitEffectTargetNodeIDToPosWithRotate ~= "" then
			if kHitEffectID ~= "" and kHitEffectTargetNodeID ~= "" then
				actor:AttachParticleS(7214,kHitEffectTargetNodeID,kHitEffectID,fHitEffectScale);
			elseif kHitEffectTargetNodeID ~= "" then
	            actor:AttachParticleS(7214,kHitEffectTargetNodeID,kHitEffectID,fHitEffectScale);
			elseif kHitEffectTargetNodeIDToPos ~= "" then
				actor:AttachParticleToPointS(7214,actor:GetNodeWorldPos(kHitEffectTargetNodeIDToPos),kHitEffectID,fHitEffectScale);			
			else
			    actor:AttachParticleToPointWithRotate(7214,actor:GetNodeWorldPos(kHitEffectTargetNodeIDToPosWithRotate), kHitEffectID, actor:GetRotateQuaternion(), fHitEffectScale)
		    end
		end
	end

	local	iTotalHit = action:GetParamInt(20)
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
					Act_Pet_Melee_Default_DoDamage(actor,actorTarget,actionResult, action:GetTargetInfo(i));
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
function Act_Pet_Melee_Default_OnTargetListModified(actor,action,bIsBefore)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());

    if bIsBefore == false then

		if actor:IsUnderMyControl()  then
			action:GetTargetList():ApplyActionEffects(true,true);
			return;
		end
        		
		--  hit 키 이후라면
        --  바로 대미지 적용시켜준다.
        Act_Pet_Melee_Default_HitOneTime(actor,action);
		action:SetParamInt(24,action:GetParamInt(24) + 1);	
        
    end

end

function Act_Pet_Melee_Default_OnEvent(actor,textKey)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
		
	local kAction = actor:GetAction();
		
	CheckNil(nil==kAction);
	CheckNil(kAction:IsNil());

	
	if kAction:GetActionParam() == AP_CASTING then
		return true;
	end
	
	if textKey == "hit" or textKey == "fire" then

		if 0 < kAction:GetParamInt(23) and 0 == kAction:GetParamInt(25) then
			-- 이동 후 hit를 해야하는 경우
			-- 아직 이동이 끝나지 않은 경우
			return true;
		end

		local kProjectileID = kAction:GetScriptParam("PROJECTILE_ID");
	
		if (nil ~= string.find(kAction:GetID(), "a_blizzard_pet")) or (nil ~= string.find(kAction:GetID(), "a_meteor_pet")) then
			--kAction:GetID() == "a_blizzard_pet_a" or kAction:GetID() == "a_blizzard_pet_b" or kAction:GetID() == "a_blizzard_pet_c" or 
			--kAction:GetID() == "a_meteor_pet_a" or kAction:GetID() == "a_meteor_pet_b" or kAction:GetID() == "a_meteor_pet_c" then
			local kProjectileMan = GetProjectileMan();

			local kPos = actor:GetNodeWorldPos("char_root");

			local iTotalProjectile = kAction:GetParamInt(100);
			
			if 0 == iTotalProjectile then
				return true;
			end

			local i = 0;
			local j = 0;
			local k = 0;
			local fFireDelay = 0.5;
			local fRange = 75;

			local	kProjectileID2 = kAction:GetScriptParam("PROJECTILE_ID");

			if "" ~= kProjectileID2 then
				while i < iTotalProjectile do
					local kNewArrow = kProjectileMan:CreateNewProjectile(kProjectileID2, actor:GetAction(), actor:GetPilotGuid());
					if false == kNewArrow:IsNil() then
						kNewArrow:SetMultipleAttack(true)
						local kStartPos = kAction:GetParamAsPoint(101 + k);
						local kEndPos = kAction:GetParamAsPoint(101 + k + 1);

						if (nil ~= string.find(kAction:GetID(), "a_meteor_pet")) then -- 메테오 비스듬하게 떨어지게
							--kAction:GetID() == "a_meteor_pet_a" or kAction:GetID() == "a_meteor_pet_b" or kAction:GetID() == "a_meteor_pet_c" then 
							local kMovingDir = actor:GetLookingDir();
							local iAttackRange = kAction:GetSkillRange(0,actor);
							iAttackRange = iAttackRange * 0.7; -- 2/3 정도 기울기에서 떨어지게
							kMovingDir:Multiply(iAttackRange);
							kStartPos:Add(kMovingDir);
						end

						kNewArrow:LoadToPosition(kStartPos);	--	장전			
						kNewArrow:SetParamValue("index", tostring(i))
						kNewArrow:SetTargetLoc(kEndPos);
						-- 한발에 랜덤값을 적용해서 틱 당 발사되는 발사체들이 덜 어색하도록
						kNewArrow:DelayFire(fFireDelay * j + (math.random(-3, 3) / 10) ,true);
						kNewArrow:SetParentPilotGuid(actor:GetPilotGuid());
						kNewArrow:SetHide(true);
					end
					i=i+1;
					k=k+2;

					-- 4발이 같은 틱에 발사되도록 
					if i % 4 == 0 then
						j=j+1;
					end
				end
			end
		elseif kProjectileID ~= nil and "" ~= kProjectileID then
			local	kProjectileMan = GetProjectileMan();
			local	kArrow = Projectile_Default_LoadToWeapon(actor,kAction);				

			if kArrow:IsNil() == false then
				local	kMovingDir = actor:GetLookingDir();	-- 액터가 바라보고 있는 방향
				local	arrow_worldpos = kArrow:GetWorldPos();	--	화살의 위치

				kArrow:SetParentActionInfo(kAction:GetActionNo(),kAction:GetActionInstanceID(),kAction:GetTimeStamp());

				local	iAttackRange = kAction:GetSkillRange(0,actor);
			
				kMovingDir:Multiply(iAttackRange);
				kMovingDir:Add(arrow_worldpos);
				kArrow:SetTargetLoc(kMovingDir);

				local iPentrationCount = kAction:GetScriptParam("PENETRATION_COUNT");
				if iPentrationCount == nil then
					iPentrationCount = "0";
				end

				kArrow:SetPenetrationCount(tonumber(iPentrationCount))-- 몇번을 관통할것인지 세팅
				if iPentrationCount ~= 0 then
					kArrow:SetMultipleAttack(true);
				end
				
				kArrow:Fire();
			end
		else
			local	bUseTLM = (kAction:GetScriptParam("NO_USE_TLM") ~= "TRUE");
			if bUseTLM then

				local	iHitCount = kAction:GetParamInt(21);
				local	iTotalHit = kAction
					:GetParamInt(20);

				-- 최대 히트수가 정의되어 있을때 최대 수만큼 때렸을 경우
				if 0 ~= iTotalHit and iHitCount == iTotalHit then 
					return false
				end
			
				if actor:IsUnderMyControl() then
					kAction:CreateActionTargetList(actor);

					if IsSingleMode() then			        
						Act_Pet_Melee_Default_HitOneTime(actor,kAction)
						Act_Pet_Melee_Default_OnTargetListModified(actor,kAction,false)
			            
						return;
					else
						if(0 == kAction:GetAbil(AT_CLASS)) then
							kAction:BroadCastTargetListModify(actor:GetPilot());
						end
					end

				end	
			end
		end


		Act_Pet_Melee_Default_HitOneTime(actor,kAction);

		--kAction:ClearTargetList();	

		if kProjectileID ~= nil and "" ~= kProjectileID then
			actor:ClearIgnoreEffectList();			
		end
	end

	return	true;
end

function Act_Pet_Melee_Default_OnTimer(actor,fAccumTime,action,iTimerID)
	
	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
	local	iHitCount = action:GetParamInt(21);
	local	iTotalHit = action:GetParamInt(20);

	-- 최대 히트수가 정의되어 있을때 최대 수만큼 때렸을 경우
	if 0 ~= iTotalHit and iHitCount == iTotalHit then 
		return true
	end

	action:CreateActionTargetList(actor);
	
	if IsSingleMode() then
    
		Act_Pet_Melee_Default_HitOneTime(actor,action)
		Act_Pet_Melee_Default_OnTargetListModified(actor,action,false)
        
		return true;
	else

		local	kTargetList = action:GetTargetList();
		local	iTargetCount = kTargetList:size();

		if 0 < iTargetCount then
			if(0 == action:GetAbil(AT_CLASS)) then
				action:BroadCastTargetListModify(actor:GetPilot());
			end			
		end

		Act_Pet_Melee_Default_HitOneTime(actor,action)
	end

	iHitCount = iHitCount + 1;
	action:SetParamInt(21,iHitCount);
	
	return	true

end

function Act_Pet_Melee_Default_PlayNextAction(actor, action,iHitRunEnd)

	if 2 < action:GetSlotCount() and action:GetSlotCount() - 1 > action:GetCurrentSlot() then -- 카운터는 개수이고, 현재 슬롯은 0부터 시작
		action:SetSlot(action:GetCurrentSlot()+1);
		actor:PlayCurrentSlot();
		return true;
	elseif 1 == iHitRunEnd and 2 == action:GetSlotCount() and action:GetSlotCount() - 1 == action:GetCurrentSlot() then -- 멈춘 후 데미지 + 모션은 2개 인경우
		if true == actor:IsUnderMyControl() then
			local	iHitCount = action:GetParamInt(21);
			local	iTotalHit = action:GetParamInt(20);

			-- 최대 히트수가 정의되어 있을때 최대 수만큼 때렸을 경우
			if 0 ~= iTotalHit and iHitCount == iTotalHit then 
				return false
			end

			action:CreateActionTargetList(actor);
			if(0 == action:GetAbil(AT_CLASS)) then
				action:BroadCastTargetListModify(actor:GetPilot());
			end
			Act_Pet_Melee_Default_HitOneTime(actor,action)

			return false;
		end
	end	

	return false;
end