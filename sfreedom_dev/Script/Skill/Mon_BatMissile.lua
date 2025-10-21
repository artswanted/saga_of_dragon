-- [PROJECTILE_ID] : 발사할 발사체 ID, "" 일 경우 무기에 붙어있는 발사체 ID 를 사용한다.
-- [ATTACH_NODE_ID] : 발사체를 붙일 액터의 노드 ID, "" 일 경우 캐릭터 좌표에서 발사된다.
-- [HIT_SOUND_ID] : HIT 키에서 플레이 할 사운드 ID
-- [NO_AUTO_CONNECT] : 다음 액션으로 부드럽게 연결해줄 것인가

-- Melee
function Skill_Mon_BatMissle_OnCheckCanEnter(actor,action)
	return true
end
function Skill_Mon_BatMissle_OnCastingCompleted(actor, action)

	if( CheckNil(nil==actor) ) then return false end
	if( CheckNil(actor:IsNil()) ) then return false end
	
	if( CheckNil(nil==action) ) then return false end
	if( CheckNil(action:IsNil()) ) then return false end
	
	action:SetSlot(action:GetCurrentSlot()+1)
	actor:ResetAnimation()
	actor:PlayCurrentSlot()
	Skill_Mon_BatMissle_Fire(actor,action)
	Act_Mon_Melee_SetState(actor,action,"FIRE")
	actor:DetachFrom(123)	--캐스팅 이펙트 제거
end

function Skill_Mon_BatMissle_Fire(actor,action)
	
	action:CreateActionTargetList(actor);
	
end

function Skill_Mon_BatMissle_OnEnter(actor, action)

	if( CheckNil(nil==actor) ) then return false end
	if( CheckNil(actor:IsNil()) ) then return false end
	
	if( CheckNil(nil==action) ) then return false end
	if( CheckNil(action:IsNil()) ) then return false end
	
--	local actorID = actor:GetID()
--	local actionID = action:GetID()
--	local weapontype = actor:GetEquippedWeaponType();
	
	actor:ResetAnimation();

	if action:GetActionParam() == AP_FIRE then
		Skill_Mon_BatMissle_OnCastingCompleted(actor,action)
	else
		if( Act_Mon_Melee_SetState(actor,action,"BATTLEIDLE_START") == false) then
			Act_Mon_Melee_SetState(actor,action,"BATTLEIDLE_LOOP")
		end	
		local effect = actor:GetAnimationInfoFromAction("FIRE_EFFECT", action, 1000212)
		if nil ~= effect then
			local node = actor:GetAnimationInfoFromAction("FIRE_EFFECT_NODE", action, 1000212)
			if nil == node then
				node = "char_root"
			end

			actor:AttachParticle(123, node, effect)
		end
	end
	
	return true
end

function Skill_Mon_BatMissle_OnUpdate(actor, accumTime, frameTime)

	if( CheckNil(nil==actor) ) then return false end
	if( CheckNil(actor:IsNil()) ) then return false end
		
--	local actorID = actor:GetID()
	local action = actor:GetAction()
	
	if( CheckNil(nil==action) ) then return false end
	if( CheckNil(action:IsNil()) ) then return false end
	
	local animDone = actor:IsAnimationDone()
--	local nextActionName = action:GetNextActionName()
--	local actionID = action:GetID()

	local	iState = action:GetParamInt(1)

	if action:GetActionParam() == AP_FIRE then
		if animDone == true then
			if iState == 2 then
				if actor:GetAnimationLength(action:GetSlotAnimName(4)) == 0 then
					return false
				else
					Act_Mon_Melee_SetState(actor,action,"RETURN")
					return true
				end
			end
			
			return false
		end
	elseif action:GetActionParam() == AP_CASTING then
		if iState == 0 then
			if animDone then
				Act_Mon_Melee_SetState(actor,action,"BATTLEIDLE_LOOP")
			end
		end
	end

	return true
end

function Skill_Mon_BatMissle_OnCleanUp(actor)
    return true;
end

function Skill_Mon_BatMissle_OnLeave(actor, action)
	
	if( CheckNil(nil==actor) ) then return true end
	if( CheckNil(actor:IsNil()) ) then return true end
	
	if( CheckNil(nil==action) ) then return true end
	if( CheckNil(action:IsNil()) ) then return true end
	
	local	kCurAction = actor:GetAction();
	local	actionID = action:GetID()
	--ODS("Act_Mon_Melee_OnLeave nextaction : "..actionID.." CurAction : "..kCurAction:GetID().." CurAction AP:"..kCurAction:GetActionParam().." \n");
	if action:GetActionType()=="EFFECT" or
		actionID == "a_freeze" or
		actionID == "a_reverse_gravity" or
		actionID == "a_blow_up" or
		actionID == "a_blow_up_small" or
		actionID == "a_knock_back" or
		actionID == "a_die" or
		actionID == "a_idle" or
		actionID == "a_run_right" or
		actionID == "a_run_left" or
		actionID == "a_handclaps" then
		return true
	end
	return false 
end

function Skill_Mon_BatMissle_OnEvent(actor,textKey)

	if( CheckNil(nil==actor) ) then return false end
	if( CheckNil(actor:IsNil()) ) then return false end
		
	local kAction = actor:GetAction();
	
	if( CheckNil(nil==kAction) ) then return end
	if( CheckNil(kAction:IsNil()) ) then return false end
	
	if kAction:GetActionParam() == AP_CASTING then
		return true;
	end

	ODS("Skill_Mon_BatMissle_OnEvent textKey:"..textKey.."\n");
	
	if textKey == "hit" or textKey == "fire" then
        actor:ClearIgnoreEffectList();
		local	kSoundID = kAction:GetScriptParam("HIT_SOUND_ID");
		if kSoundID~="" then
			actor:AttachSound(2784,kSoundID);
		end
		
--		local	kProjectileMan = GetProjectileMan();
		
		local	iAttackRange = kAction:GetSkillRange(0,actor);
		if iAttackRange == 0 then
			iAttackRange = 200; 
		end
		
		local   kTargetList = kAction:GetTargetList()
		local	iTargetCount = kTargetList:size()
--		local   iTargetNum = 0
		local   kTargetInfo = nil

		local fire_delay = actor:GetAnimationInfo("FIRE_DELAY")
		if nil == fire_delay or "" == Projectile_cnt then
			fire_delay = 0 -- 기본 딜레이 없음
		else
			fire_delay = tonumber(fire_delay)
		end

		local Projectile_cnt = actor:GetAnimationInfo("PROJECTILE_COUNT")
		if nil == Projectile_cnt or "" == Projectile_cnt then
			Projectile_cnt = 5 -- 기본 5개
		else
			Projectile_cnt = tonumber(Projectile_cnt)
		end
		
		if iTargetCount>0 then
				--	타겟을 찾아 바라본다
			local	kTargetGUID = kAction:GetTargetGUID(0);
			local   iTargetABVShapeIndex = kAction:GetTargetABVShapeIndex(0);
			local   targetobject = g_pilotMan:FindPilot(kTargetGUID);
			if targetobject:IsNil() == false then
				local   pt = targetobject:GetActor():GetABVShapeWorldPos(iTargetABVShapeIndex);
				actor:LookAt(pt);
			end	
		end		

		local   kLookDir = actor:GetLookingDir();
		local kUp = Point3(0,0,1)
		local kRight = kLookDir:Cross(kUp)	-- 업벡터랑 외적하여 right벡터 얻자
		kRight:Unitize()	-- 단위벡터로
		local iRound = kAction:GetAbil(AT_1ST_AREA_PARAM2)

		if iRound == 0 then
			iRound = 40
		end

		local kAddZ = actor:GetAnimationInfo("PROJECTILE_ADD_Z")
		local no = actor:GetAnimationInfo("NO_VERICAL_RANDOM")
		local   iArrowCount = 0
		while   iArrowCount<Projectile_cnt do
		    --  프로젝틸 생성
			local	kNewArrow = Skill_Mon_BatMissle_LoadToWeapon(actor,kAction,0,kAddZ);
			if kNewArrow == nil or kNewArrow:IsNil() then
				ODS("Arrow is nil\n")
				return
			end

		    kTargetInfo = kTargetList:GetTargetInfo(0)
		    
		    kNewArrow:SetParentActionInfo(kAction:GetActionNo(),kAction:GetActionInstanceID(),kAction:GetTimeStamp())
		    local	arrow_worldpos = kNewArrow:GetWorldPos()	--	화살의 위치
	        local   kTargetPos = actor:GetLookingDir()
			kTargetPos:Multiply(iAttackRange)
			kTargetPos:Add(arrow_worldpos)

			if nil==no then
				kTargetPos:Add(Point3(0,0,math.random(-iRound, iRound)))
			end
			local addx = kRight:_Multiply(math.random(-iRound, iRound))

			if nil~=kAddZ and 0~=kAddZ then
				addx:SetZ(addx:GetZ()+kAddZ)
			end

			kTargetPos:Add(addx)

			kNewArrow:SetTargetLoc(kTargetPos)    
		    
		    kNewArrow:DelayFire(iArrowCount*fire_delay)
		    
		    iArrowCount=iArrowCount+1
		end

		local dmg = actor:GetAnimationInfo("DAMAGE_ON_EVENT")
		if nil~=dmg then
			Act_Mon_Melee_DoDamage(actor, kAction)
		end
	end

	return	true
end

function Skill_Mon_BatMissle_LoadToWeapon(actor,action,iProjectileIndex,kAddZ)
	if( CheckNil(nil==actor) ) then return nil end
	if( CheckNil(actor:IsNil()) ) then return nil end
	
	if( CheckNil(nil==action) ) then return nil end
	if( CheckNil(action:IsNil()) ) then return nil end
	
	local	kAttachID=""
	
	if iProjectileIndex>0 then
		kAttachID = ""..iProjectileIndex;
	end
	
	local	kProjectileMan = GetProjectileMan()
	local	kProjectileID = actor:GetAnimationInfo("PROJECTILE_ID"..kAttachID)
	local	fProjetileScale = tonumber(actor:GetAnimationInfo("PROJECTILE_SCALE"..kAttachID))
	if fProjetileScale == nil then
		fProjetileScale = 1.0
	end
	
	if iProjectileIndex>0 and kProjectileID==nil then
		return	nil
	end
	
	if kProjectileID == nil then
		kProjectileID = "Projectile_arrow_02_arrow_head"
	end
	
	local	kNewArrow = kProjectileMan:CreateNewProjectile(kProjectileID,action,actor:GetPilotGuid())
	if kNewArrow:IsNil() then
		return nil
	end
	
	local	kTargetNodeID = actor:GetAnimationInfo("FIRE_START_NODE"..kAttachID)
	if kTargetNodeID == nil then
		kTargetNodeID = "p_ef_heart"
	end

	local	fProjectileSpeed = actor:GetAnimationInfo("PROJECTILE_SPEED"..kAttachID)
	if fProjectileSpeed ~= nil then
		fProjectileSpeed = tonumber(fProjectileSpeed)
		kNewArrow:SetSpeed(fProjectileSpeed)
	end

	kNewArrow:SetScale(fProjetileScale*kNewArrow:GetScale())	

	local kNodePos = actor:GetNodeTranslate(kTargetNodeID)
	if nil~=kAddZ then
		kNodePos:SetZ(kNodePos:GetZ()+kAddZ)
	end
	kNewArrow:LoadToPosition(kNodePos)
--	kNewArrow:LoadToHelper(actor,kTargetNodeID)	--	장전
	return kNewArrow
end
