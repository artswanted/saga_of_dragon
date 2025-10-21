
function Skill_Blink_OnCheckCanEnter(kActor,kAction)

	CheckNil(nil==kActor)
	CheckNil(kActor:IsNil())

	local weapontype = kActor:GetEquippedWeaponType()
	
	if weapontype == 0 then
		return	false
	end

	return		true
	
end

function Skill_Blink_OnCastingCompleted(kActor, kAction)

	CheckNil(nil==kActor)
	CheckNil(kActor:IsNil())
	
	CheckNil(nil==kAction)
	CheckNil(kAction:IsNil())

	Skill_Blink_Fire(kActor,kAction)
end

function Skill_Blink_Fire(kActor,kAction)

	CheckNil(nil==kActor)
	CheckNil(kActor:IsNil())
	
	CheckNil(nil==kAction)
	CheckNil(kAction:IsNil())
	
	ODS("RestoreItemModel Skill_Blink_Fire\n")

	kAction:CreateActionTargetList(kActor)

	kAction:SetParamAsPoint(6, kActor:GetLookingDir())
	
	kActor:StopJump()	
	local	kSoundID = kAction:GetScriptParam("FIRE_SOUND_ID")
	if kSoundID~="" then
		kActor:AttachSound(2784,kSoundID)
	end
	--kActor:AttachParticleToPointWithRotate(7212,kActor:GetNodeWorldPos("p_ef_heart"), "ef_show_01_p_ef_heart", kActor:GetRotateQuaternion())
end

function Skill_Blink_OnFindTarget(kActor,kAction,kTargets)

	CheckNil(nil==kActor)
	CheckNil(kActor:IsNil())
	
	CheckNil(nil==kAction)
	CheckNil(kAction:IsNil())
	
	local  iAttackRange = kAction:GetSkillRange(0,kActor)
	
	--ODS("Skill_WindMill_OnFindTarget iAttackRange:"..iAttackRange.."\n")
	if iAttackRange == 0 then
	    iAttackRange = 100
	end

	local	kPos = kActor:GetPos()
	local	kDir = kActor:GetLookingDir()
	local	fBackDistance = 10
	kDir:Multiply(-fBackDistance)
	kPos:Add(kDir)

	local kParam = FindTargetParam()
	
    local iFindTargetType = TAT_SPHERE
	kParam:SetParam_1(kActor:GetPos(),kActor:GetLookingDir())
	kParam:SetParam_2(0,0,iAttackRange,0)
	
	kParam:SetParam_3(true,FTO_NORMAL)
	local iTargets = kAction:FindTargets(iFindTargetType,kParam,kTargets,kTargets)
	
	return iTargets
end

function Skill_Blink_OnTargetListModified(kActor,kAction,bIsBefore)
	
	CheckNil(nil==kActor)
	CheckNil(kActor:IsNil())
	
	CheckNil(nil==kAction)
	CheckNil(kAction:IsNil())
	
    ODS("Skill_WindMill_OnTargetListModified\n")
    
    if bIsBefore == false then
        --  때린다.
        SkillHelpFunc_DefaultHitOneTime(kActor,kAction, true);
    end
end

function Skill_Blink_OnEnter(kActor, kAction)

	CheckNil(nil==kActor)
	CheckNil(kActor:IsNil())
	
	CheckNil(nil==kAction)
	CheckNil(kAction:IsNil())
	
--	local actorID = kActor:GetID()
	local actionID = kAction:GetID()
--	local weapontype = kActor:GetEquippedWeaponType()
	
	ODS("Skill_Blink_OnEnter actionID:"..actionID.." GetActionParam:"..kAction:GetActionParam().."\n")	
	kAction:SetParamInt(0, 0)
	kAction:SetParamAsPoint(100, kActor:GetPos())
	
	local fMaxMoveDist = kAction:GetAbil(AT_C_MOVESPEED)
	if(0 == fMaxMoveDist) then 
		fMaxMoveDist = 100
	end
	kAction:SetParamFloat(101, fMaxMoveDist)
	
	local fSpeed = kAction:GetAbil(AT_SKILL_MAXIMUM_SPEED)
	if(0 == fSpeed) then 
		fSpeed = 500
	end
	kAction:SetParamFloat(300, fSpeed)
	local fHeightLimit = jumpForce * 0.7
	kAction:SetParamFloat(301, fHeightLimit) --위로 이동 가능한 크기
	
	
	if kAction:GetActionParam() == AP_CASTING then	--	캐스팅이라면 그냥 리턴하자.
		return	true
	end
	kActor:PlayCurrentSlot()
	
	return true
end

--[[
function Skill_Blink_CalcResultPos(kActor, kBeginPos, kMoveDir, fDist)
	CheckNil(nil==kActor)
	CheckNil(kActor:IsNil())	
	local kAction = kActor:GetAction()	
	CheckNil(nil==kAction)
	CheckNil(kAction:IsNil())

	kMoveDir:Multiply(fDist)
	kMoveDir:Add(kBeginPos)	
	local kResult = kMoveDir
	
	return kResult
end
]]

function Skill_Blink_OnUpdate(kActor, accumTime, frameTime)

	CheckNil(nil==kActor)
	CheckNil(kActor:IsNil())
		
--	local actorID = kActor:GetID()
	local kAction = kActor:GetAction()
	
	CheckNil(nil==kAction)
	CheckNil(kAction:IsNil())
	
--	ODS("값:"..kAction:GetParamInt(0).."\n", false, 912)
	
--	local animDone = kActor:IsAnimationDone()
	local fSpeed = kAction:GetParamFloat(300)
	--if(animDone == true) then
		if(kAction:GetParamInt(0) == 1) then
			Skill_Blink_OnCastingCompleted(kActor,kAction)
			if( CheckNil(g_world == nil) ) then return false end
			if( CheckNil(g_world:IsNil()) ) then return false end
			kAction:SetParamFloat(200, g_world:GetAccumTime()) --  시작 시간 기록
			kAction:SetParamInt(0,2)
			kActor:SetHide(true)
		elseif(kAction:GetParamInt(0) == 2) then 
			local kBeginPos = kAction:GetParamAsPoint(100)
			
			local kNewPos = kActor:GetPos()
			local fDistance = kNewPos:Distance(kBeginPos)
			local fMaxMoveDist = kAction:GetParamFloat(101)
			local kMoveDir = kAction:GetParamAsPoint(6)
			kMoveDir:Multiply(-1)
			
			if(0 < fDistance) then	-- fSpeed 보정
				local fRemainDistance = fMaxMoveDist - fDistance
				fSpeed = fRemainDistance/frameTime -- 속력 = 남은거리/시간
			end

			local fDist = kNewPos:Distance(kBeginPos)			
			ODS("dist:"..fDist.." MaxDist:"..fMaxMoveDist.."\n",false, 912)
						
			local   fElasepdTime = accumTime - kAction:GetParamFloat(200)
			if(fDist >= fMaxMoveDist or fElasepdTime >= 0.2) then -- 시간의 오차가 있으므로 0.1초 달성 스피드지만 0.2로 해준다
				ODS("걸림"..fElasepdTime.."\n",false, 912)
				kAction:SetParam(1, "end")
				kActor:AttachParticleToPointWithRotate(7213,kActor:GetNodeWorldPos("p_ef_heart"), "ef_show_01_p_ef_heart", kActor:GetRotateQuaternion())
				return false
			end
			
			kMoveDir:Multiply(fSpeed)
			kActor:SetMovingDelta(kMoveDir)
			
		elseif(kAction:GetParamInt(0) == 3) then
			ODS("언덕이동~!!!\n", false, 912)
			--언덕길이라 바로 위치 이동
			local kTargetPos = kAction:GetParamAsPoint(7)
			kActor:SetTranslate(kTargetPos, false)
			kAction:SetParam(1, "end")
			kActor:AttachParticleToPointWithRotate(7213,kTargetPos, "ef_show_01_p_ef_heart", kActor:GetRotateQuaternion())
			return false			
		end	
	--end

	return true
end

function Skill_Blink_OnCleanUp(kActor)
	
	CheckNil(nil==kActor)
	CheckNil(kActor:IsNil())

	local	kAction = kActor:GetAction()	
	CheckNil(nil==kAction)
	CheckNil(kAction:IsNil())
--	kActor:DetachFrom(7212)
--	kActor:DetachFrom(7213)
	kActor:SetHide(false)
	return true
end

function Skill_Blink_OnLeave(kActor, kAction)

	CheckNil(nil==kActor)
	CheckNil(kActor:IsNil())
	
	CheckNil(nil==kAction)
	CheckNil(kAction:IsNil())
	
	local curAction = kActor:GetAction()
	local curParam = curAction:GetParam(1)
	local actionID = kAction:GetID()
	
	if kActor:IsMyActor() == false then
		return true
	end
	
	if actionID == "a_jump" then 
		return false
	end
	
	if kAction:GetActionType()=="EFFECT" then
		return true
	end
	
	if curParam == "end" then 
		return true
	end
	
	return false 
end

function Skill_Blink_OnEvent(kActor,textKey)

	CheckNil(nil==kActor)
	CheckNil(kActor:IsNil())
		
	local kAction = kActor:GetAction()
		
	CheckNil(nil==kAction)
	CheckNil(kAction:IsNil())
	
	
	if kAction:GetActionParam() == AP_CASTING then
		return true
	end
	local fHeightLimit = kAction:GetParamFloat(301) --위로 이동 가능한 높이
	
	if textKey == "hit" or textKey == "fire" then
		kActor:AttachParticleToPointWithRotate(7212,kActor:GetNodeWorldPos("p_ef_heart"), "ef_show_01_p_ef_heart", kActor:GetRotateQuaternion())
		local	kSoundID = kAction:GetScriptParam("FIRE_SOUND_ID");
		if kSoundID~="" then
			kActor:AttachSound(2783,kSoundID);
		end	
	
		-- 여기서 거리 검사 하고 
		local fMaxMoveDist = kAction:GetParamFloat(101)	--최대 이동거리
		local kReverseDir = kActor:GetLookingDir()	kReverseDir:Multiply(-1)	--플레이어 바라보는 반대방향
		local kPos = kActor:GetPos()
		kPos:SetZ(kPos:GetZ()+fHeightLimit)
		
		if( CheckNil(g_world == nil) ) then return false end
		if( CheckNil(g_world:IsNil()) ) then return false end
		local ptcl = g_world:ThrowRay(kPos, kReverseDir, fMaxMoveDist,3) --뒤로 레이를 쏴서 
		if ptcl:GetX() == -1 and ptcl:GetY() == -1 and ptcl:GetZ() == -1 then	-- 이동이 가능하면			
			local kTemp = kReverseDir
			kTemp:Multiply(fMaxMoveDist)
			kTemp:Add(kActor:GetPos())		 -- 뒤로 이동 시키고
			kTemp:SetZ(kTemp:GetZ()+fHeightLimit) -- 조금 위로 올린후에			
			ptcl = g_world:ThrowRay(kTemp, Point3(0,0,-1), 500,3)	-- 레이를 아래로 쏴서
			if ptcl:GetX() == -1 and ptcl:GetY() == -1 and ptcl:GetZ() == -1 then
				--실패하면 위로도 쏴 보고
				ptcl = g_world:ThrowRay(kTemp, Point3(0,0,1),500,3);
				if ptcl:GetX() == -1 and ptcl:GetY() == -1 and ptcl:GetZ() == -1 then
				--모두 실패 하면 MoveDelta형식으로 이동
					kAction:SetParamInt(0, 1)
					return true
				end
			end
			--바닥을 찾았으면
			ptcl:SetZ(ptcl:GetZ()+30) -- 캐릭터가 서있을 위치를
			kAction:SetParamAsPoint(7, ptcl) -- 설정하고
			kAction:SetParamInt(0, 3) -- 강제 이동 시킨다
		else
			-- 플레이어 뒤로 fMaxMoveDist 거리 이내 뭔가 있으면 MoveDelta형식으로 이동
			kAction:SetParamInt(0, 1)
			return true
		end
	end
	
	return	true
end
