-- SwiftShot
-- SKILL_RANGE : 어느 범위 안에 있는 몬스터들이 맞을 것인가?
-- SKILL_TIME : Skill 총 시간(필요 없을 수도)
-- SKILL_MAX_MONSTER : 총 몇마리의 몬스터가 맞을 것인가?
-- SKILL_MAX_HIT : 총 몇대를 때릴 수 있는가?
-- HIT_EFFECT_ID : 몬스터를 때릴때의 HitEffectID
-- SKILL_ADJUST : 잔상 이펙트를 보여주는 범위를 조정한다. 값이 크면 클 수록 좀 더 과장 되어져 보인다.

function Skill_SwiftShot_OnEnter(actor, action)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
--	local actorID = actor:GetID()
--	local actionID = action:GetID()
	local skillRange = action:GetScriptParam("SKILL_RANGE")
	local skillTime = action:GetScriptParam("SKILL_TIME")
	local skillMaxMonster = action:GetScriptParam("SKILL_MAX_MONSTER")
	local skillMaxHit = action:GetScriptParam("SKILL_MAX_HIT")
--	local iCastingTime = action:GetAbil(AT_CAST_TIME)
	local fSkillAdjust = action:GetScriptParam("SKILL_ADJUST")
	
	if actor:IsMyActor() == true then
		actor:StopNormalAttackFreeze();
		action:LockCurrentTarget(skillRange, skillMaxMonster, true);

		if action:GetTargetCount() <= 0 then
			return false;
		end
		
		-- 보스일 경우에도 쓴다.
		--if action:GetTargetCount() == 1 then
		--	targetGUID = action:GetTargetGUID(0)
		--	if targetGUID:IsNil() == false then				
		--		target = g_pilotMan:FindPilot(targetGUID)
		--		if target:IsNil() == false and target:GetMonsterNo() == 23 then
		--			return false
		--		end
		--	end
		--end
		
		if( CheckNil(g_world == nil) ) then return false end
		if( CheckNil(g_world:IsNil()) ) then return false end
		g_world:SetShowWorldFocusFilter(true, "../Data/5_Effect/9_Tex/EF_blackBG.tga", 0.8)

		action:SetParamFloat(1, GetCameraTrn():GetX())
		action:SetParamFloat(2, GetCameraTrn():GetY())
		action:SetParamFloat(3, GetCameraTrn():GetZ())
		action:SetParamFloat(4, GetCameraRot():GetZ())
		local cameraTrn = GetCameraTrn()
		local cameraRot = GetCameraRot()
		cameraTrn:SetY(cameraTrn:GetY() - 80)
		cameraTrn:SetZ(cameraTrn:GetZ() + 100)
		cameraRot:SetZ(cameraRot:GetZ() -60)
		--SetCameraSetting(cameraTrn, cameraRot)		
	end
	
	action:GenerateSwiftShotSkillInfo(skillMaxHit, skillTime, fSkillAdjust)
	
	-- 첫번째 타켓 방향으로 틀고
	local targetGUID = action:GetTargetGUID(0)
	local target = g_pilotMan:FindPilot(targetGUID);
	if target:IsNil() == false then
		local targetobjpos = target:GetActor():GetTranslate();
		actor:LookAt(targetobjpos);
	end
	
	action:SetParam(6, actor:GetTranslate():GetX())
	action:SetParam(7, actor:GetTranslate():GetY())
	action:SetParam(8, actor:GetTranslate():GetZ())
	
	-- Target으로 잡힌 모든 몬스터 멈춤
	if action:GetTargetCount() > 0 then
		for iIndex = 0, action:GetTargetCount() - 1 do
			targetGUID = action:GetTargetGUID(iIndex)
			target = g_pilotMan:FindPilot(targetGUID)
			if target:IsNil() == false then
				target:GetActor():ReserveTransitAction("a_freeze")
				target:GetActor():SetAnimSpeedInPeriod(0.00001, (skillTime + 0.1) * 1000)
			end
		end
	end
	
	actor:SetComboCount(0)
	action:SetParam(0, "ready")
	action:SetSlot(1) -- 잠깐 달릴려는 듯한 포즈 Ani	
	action:SetSkillTime(skillTime + 0.1)
	action:SkillStart()
	action:SetParam(0, "start")
	return true
end

function Skill_SwiftShot_OnUpdate(actor, accumTime, frameTime)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	local action = actor:GetAction();
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());	
	
	if action:GetParam(0) == "start" and actor:IsAnimationDone() == true then
		-- 이제 진짜로 달리기 시작
		actor:SetHide(true)
		actor:FreeMove(true)		
		action:SetParam(0, "swift")		
		actor:StartSwiftShotSkillTrail(150, 40, 0, "../Data/5_Effect/9_Tex/swift_01.tga", "../Data/5_Effect/9_Tex/swift_02.tga")
		actor:AttachSound(7, "../Data/1_Cha/1_PC/C_Sound/Skill/Swift_Shot.wav", 30, 10, 100)
	end
	
	local hitTargetIndex = action:GetHitTargetIndex(accumTime, frameTime)
	local kTargetGUID
	local kTargetPilot
	local HitEffectID = action:GetScriptParam("HIT_EFFECT_ID");
	local HitEffectTarget = action:GetScriptParam("HIT_EFFECT_TARGET");
	
	if hitTargetIndex >= 0 then		
		kTargetGUID = action:GetTargetGUID(hitTargetIndex);
		kTargetPilot = g_pilotMan:FindPilot(kTargetGUID);		
	
		if kTargetPilot:IsNil() == false then			
			local actorTarget = kTargetPilot:GetActor();
			if HitEffectID ~= "" then				
				if HitEffectTarget=="" then				
					local pt = actorTarget:GetHitPoint()
					actorTarget:AttachParticleToPoint(12, pt, HitEffectID)
				else					
					actorTarget:AttachParticle(12,HitEffectTarget,HitEffectID);
				end				
			end
			actorTarget:PlayWeaponSound(WST_HIT, actor, "a_SwiftShot", 0, action:GetTargetInfo(hitTargetIndex))
			QuakeCamera(0.2, 2.0)
			Net_ReqDamage(action:GetActionNo(),action:GetActionInstanceID(),kTargetGUID);			
		end
	else
		local showPos = action:GetSwiftShotActorPos(accumTime, frameTime)
		showPos:SetZ(action:GetParamFloat(8))
		actor:SetTranslate(showPos)
		if hitTargetIndex == -2 then -- -2가 오면 전환점이다.
			actor:SetHide(false)			
			--action:SetParam(9, showPos:GetX())
			--action:SetParam(10, showPos:GetY()) -- Rotation을 위해 방향 전환시의 x,y를 저장
			action:SetParam(12, accumTime + 0.1)
			--originPos = Point3(action:GetParamFloat(6) - showPos:GetX(), action:GetParamFloat(7) - showPos:GetY(), action:GetParamFloat(8))
			--originPos = Point3(action:GetParamFloat(6), action:GetParamFloat(7), action:GetParamFloat(8))
			--originPos:Unitize()
			--degree = math.acos(originPos:Dot(Point3(0, -1, 0)))
			--actor:SetRotation(math.random(3), Point3(0, 0, 1))
			--actor:LookAt(originPos)
		else
			actor:SetHide(false)
		end
	end
	
	
	if action:GetParamFloat(12) < accumTime then
		actor:SetHide(true)
	end
	
	if action:IsSkillEnd() == true then
		action:SetParam(0, "end")
		return false
	end
	
	return true
end

function Skill_SwiftShot_ReallyLeave(actor)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	local current_action = actor:GetAction();
	
	CheckNil(nil==current_action);
	CheckNil(current_action:IsNil());
	
	local targetGUID
	local target
	-- Target으로 잡힌 모든 몬스터 애니 멈춤을 풀어줌	
	if current_action:IsNil() == false and current_action:GetTargetCount() > 0 then
		for iIndex = 0, current_action:GetTargetCount() - 1 do		
			targetGUID = current_action:GetTargetGUID(iIndex)
			target = g_pilotMan:FindPilot(targetGUID)			
			if target:IsNil() == false then
				--target:GetActor():ReserveTransitAction("a_idle") -- Action/freeze.lua의 OnLeave에서 막아놨기 때문에 바꿔준다. (별로 좋은거 같지는 않다.)
				target:GetActor():SetAnimOriginalSpeed()
				--target:GetActor():SetTargetAnimation("float_04")
				target:GetActor():ReserveTransitAction("a_knock_back")
				target:GetActor():PlayWeaponSound(WST_HIT, actor, "a_melee_knock_back", 0, current_action:GetTargetInfo(iIndex))
			end
		end
	end

	if actor:IsMyActor() == true then
		local cameraTrn = GetCameraTrn()
		local cameraRot = GetCameraRot()
		cameraTrn:SetX(current_action:GetParamFloat(1))
		cameraTrn:SetY(current_action:GetParamFloat(2))
		cameraTrn:SetZ(current_action:GetParamFloat(3))
		cameraRot:SetZ(current_action:GetParamFloat(4))
		--SetCameraSetting(cameraTrn, cameraRot)
		--actor:AttachParticle(4, "char_root", "e_skill_frost_start")
	end
	local curPos = actor:GetTranslate()
	curPos:SetX(current_action:GetParamFloat(6))
	curPos:SetY(current_action:GetParamFloat(7))
	curPos:SetZ(current_action:GetParamFloat(8))
	actor:SetTranslate(curPos)
	
	actor:DetachFrom(7) -- 사운드를 끈다.
	if( CheckNil(g_world == nil) ) then return false end
	if( CheckNil(g_world:IsNil()) ) then return false end
	g_world:SetShowWorldFocusFilter(false)
	actor:SetHide(false)
	actor:FreeMove(false)
	--actor:Concil(true)
	actor:EndSwiftShotSkillTrail();
	return true;
end

function Skill_SwiftShot_OnLeave(actor, action)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	
	local actionID = action:GetID()
	local current_action = actor:GetAction();
	
	CheckNil(nil==current_action);
	CheckNil(current_action:IsNil());		
	
	if actor:IsMyActor() == false then
		return Skill_SwiftShot_ReallyLeave(actor);
	end
	
	if 	action:GetActionType()=="EFFECT" or
		current_action:GetParam(0) == "end" then
		
		-- 여기에서 GetParam(1) == "levelup" 체크해야 한다.
		return Skill_SwiftShot_ReallyLeave(actor);
	end
	
	return false
end

function Skill_SwiftShot_OnEvent(actor, textKey)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
		
	if textKey == "end" then
		local action = actor:GetAction()
		
		CheckNil(nil==action);
		CheckNil(action:IsNil());
	
		if action:GetParam(0) == "start" then
			--actor:SetHide(true)
			--action:SetSkillTime(skillTime + 0.1)
			--action:SkillStart()	
			--actor:StartSwiftShotSkillTrail("../Data/5_Effect/9_Tex/swift1.dds", 1000, 50, 2)
			--action:SetParam(0, "swift")
		end
	end
end

function Skill_SwiftShot_OnCastingCompleted(actor, action)
end