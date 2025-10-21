-- 대각선아래방향으로 세발의 화살을 동시에 발사!
-- [PROJECTILE_ID] : 발사할 프로젝틸 ID. 빈문자열일 경우 기본 프로젝틸이 발사된다.
-- [ATTACH_NODE] : 프로젝틸을 붙일 노드. 빈문자열일 경우 무기에 붙이게 된다.
function Skill_Nuclear_OnCastingCompleted(kActor,kAction)
end

function Skill_Nuclear_OnCheckCanEnter(actor,action)
	ODS("Skill_Nuclear_OnCheckCanEnter\n", false, 912)
	CheckNil(nil==actor)
	CheckNil(actor:IsNil())
	
	local kCurAction = actor:GetAction()
	if( false == kCurAction:IsNil() ) then
		local weapontype = actor:GetEquippedWeaponType()
		if weapontype == 0 then
			kCurAction:SetNextActionName("a_idle")
			return false
		end
	end

	if actor:IsMyActor() == true then
		if actor:GetFloatHeight(70)<50  then
			action:ChangeToNextActionOnNextUpdate(true,true)
			return	false	
		end	
	end
	
	return true	
end

function Skill_Nuclear_OnEnter(actor, action)

	CheckNil(nil==actor)
	CheckNil(actor:IsNil())
	
	CheckNil(nil==action)
	CheckNil(action:IsNil())
	
	if actor:IsMyActor() then
		if( CheckNil(g_world == nil) ) then return false end
		if( CheckNil(g_world:IsNil()) ) then return false end
		g_world:SetShowWorldFocusFilterColorAlpha(0x000000, 0.0, 0.7, 0.4,true,true);
	end
	
--	local	actorID = actor:GetID()
--	local	actionID = action:GetID()
	local	weapontype = actor:GetEquippedWeaponType()
	
	ODS("Skill_Nuclear_OnEnter weapontype : "..weapontype.."\n")

	if actor:IsMyActor() == true then
		Skill_Nuclear_FindTargets(actor,action)
	else
		GetActionResultCS(action, actor)
	end
	
	action:SetParamInt(10, 175) -- 미사일 날아갈 거리
	actor:StopJump()
	actor:FreeMove(true)
	actor:AttachParticleS(7216,"char_root","ef_nuclear_pickout", 1)
	return true
end
function Skill_Nuclear_OnFindTarget(actor,action,kTargets)

	CheckNil(nil==actor)
	CheckNil(actor:IsNil())
	
	CheckNil(nil==action)
	CheckNil(action:IsNil())	

	local iAttackRange = action:GetSkillRange(0,actor)
	local	arrow_worldpos = actor:GetPos()--kNewArrow:GetWorldPos()	--	화살의 위치
	
-- 회전축 구하기
--[[
	local	kRotateAxis = actor:GetLookingDir()
	kRotateAxis:Cross(Point3(0,0,1))
	kRotateAxis:Unitize()
	local	fRotateAngle = 20.0*3.141592/180.0

	local kParam = FindTargetParam()
	kParam:SetParam_1(actor:GetTranslate(),actor:GetLookingDir())
	kParam:SetParam_2(iAttackRange,20,0,1)
	kParam:SetParam_3(true,FTO_NORMAL+FTO_DOWN)		

	local	kFireTargetDir = Point3(0.1,0.1,-1)
	kFireTargetDir:Rotate(kRotateAxis,fRotateAngle)
	kParam:SetParam_1(arrow_worldpos,kFireTargetDir)
	action:FindTargets(TAT_RAY_DIR,kParam,kTargets,kTargets)
	
	kFireTargetDir = Point3(0.1,0.1,-1)
	kFireTargetDir:Rotate(kRotateAxis,fRotateAngle*2)
	kParam:SetParam_1(arrow_worldpos,kFireTargetDir)
	action:FindTargets(TAT_RAY_DIR,kParam,kTargets,kTargets)
	
	kFireTargetDir = Point3(0.1,0.1,-1)
	kFireTargetDir:Rotate(kRotateAxis,fRotateAngle*3)
	kParam:SetParam_1(arrow_worldpos,kFireTargetDir)
	action:FindTargets(TAT_RAY_DIR,kParam,kTargets,kTargets)
	]]
	return	kTargets:size()
end

function Skill_Nuclear_FindTargets(actor,action)

    action:CreateActionTargetList(actor)
	
end

function Skill_Nuclear_LoadToWeapon(actor,action)

	CheckNil(nil==actor)
	CheckNil(actor:IsNil())
	
	CheckNil(nil==action)
	CheckNil(action:IsNil())
	

	local	kProjectileID = actor:GetEquippedWeaponProjectileID()
	local	kAttackNodeName = ""
	
	if action:GetScriptParam("PROJECTILE_ID")~="" then
		kProjectileID = action:GetScriptParam("PROJECTILE_ID")
	end
	
--	kAttackNodeName = "p_ef_heart"
	
	local	kPilotGuid = actor:GetPilotGuid()
	local	kProjectileMan = GetProjectileMan()
	local	kNewArrow = kProjectileMan:CreateNewProjectile(kProjectileID,action,kPilotGuid)
	
	local kProjectileOption = kNewArrow:GetOptionInfo() -- 타겟이 0이어도 삭제 되지 않게 설정
	if(false == kProjectileOption:IsNil()) then
		kProjectileOption:SetAutoRemoveIfNoneTarget(false)
		kNewArrow:SetOptionInfo(kProjectileOption)
	end
	
	if kAttackNodeName == "" then
		--강제로 위치 세팅(애니메이션과 비슷한 위치에서 발사체 생성
		kNewArrow:LoadToWeapon(actor)
		local kPos = kNewArrow:GetWorldPos()
		kPos:SetZ(kPos:GetZ()+12)
		local kDir = actor:GetLookingDir()
		kDir:Multiply(-33)
		kPos:Add(kDir)
		kNewArrow:LoadToPosition(kPos)
	else
		kNewArrow:LoadToHelper(actor,kAttackNodeName)
	end
	return kNewArrow
end

function Skill_Nuclear_OnUpdate(actor, accumTime, frameTime)

	CheckNil(nil==actor)
	CheckNil(actor:IsNil())
	
--	local	actorID = actor:GetID()
	local	action = actor:GetAction()
	
	CheckNil(nil==action)
	CheckNil(action:IsNil())
	
	local	animDone = actor:IsAnimationDone()
	local	nextActionName = action:GetNextActionName()
--	local	iMaxActionSlot = action:GetSlotCount()
--	local	weapontype = actor:GetEquippedWeaponType()
--	local	actionID = action:GetID()

	if animDone == true then
	
		if(actor:IsMeetFloor()) then
			action:SetParam(1, "end")
			actor:ReserveTransitAction("a_battle_idle")
			return false
		end
	
--		local	curActionSlot = action:GetCurrentSlot()
		local	curActionParam = action:GetParam(0)
		if curActionParam == "GoNext" then
			action:SetParam(0, "null")
			action:SetParam(1, "end")
			action:SetNextActionName(nextActionName)
			return false
		else

			action:SetParam(1, "end")
			return false
		end
		
	elseif(action:GetParam(1) =="end") then	
		return false
	end
	
	if(actor:IsMeetFloor()) then
		action:SetParam(1, "end")
		actor:ReserveTransitAction("a_battle_idle")
		return false
	end

	return true
end
function Skill_Nuclear_OnCleanUp(actor,action)
	
	CheckNil(nil==actor)
	CheckNil(actor:IsNil())
	
	actor:FreeMove(false)

	if actor:IsMyActor() then
		if( CheckNil(g_world == nil) ) then return true end
		if( CheckNil(g_world:IsNil()) ) then return true end
		g_world:SetShowWorldFocusFilterColorAlpha(0x000000, g_world:GetWorldFocusFilterAlpha(), 0, 1, false,false);
	end	
	
	return true
end
function Skill_Nuclear_OnLeave(actor, action)

	CheckNil(nil==actor)
	CheckNil(actor:IsNil())
	
	CheckNil(nil==action)
	CheckNil(action:IsNil())
	
	local	curAction = actor:GetAction()
		
	CheckNil(nil==curAction)
	CheckNil(curAction:IsNil())
	
	local	curParam = curAction:GetParam(1)
	local	actionID = action:GetID()
	if actor:IsMyActor() == false then
		return true
	end
	if action:GetActionType()=="EFFECT"  then
		return true
	end	
	if actionID == "a_jump" then 
		return true
	end	
	
	if action:GetEnable() == false then
		
		if curAction:GetParam(1)==actionID then
			curAction:SetParam(0,"")
		end
		
	end	
	if action:GetEnable() == true then
		
		if curParam == "end" then 
			return true
		end
	
		curAction:SetParam(0,"GoNext")
		curAction:SetParam(1,actionID)
		
		return false
	
	elseif curParam == "end" and
		(actionID == "a_run_right" or
		actionID == "a_run_left") then
		return true
	elseif 	actionID == "a_idle" or
		actionID == "a_die" or
		actionID == "a_handclaps" then
		return true
	end

	return false 
end

function Skill_Nuclear_OnEvent(actor, textKey)

	CheckNil(nil==actor)
	CheckNil(actor:IsNil())
	
	local	action = actor:GetAction()		
	CheckNil(nil==action)
	CheckNil(action:IsNil())
		
	if textKey == "fire" or textKey=="hit" then
		
		local kSoundID = action:GetScriptParam("FIRE_SOUND_ID");
		if kSoundID~="" then
			actor:AttachSound(2783,kSoundID);
		end	
	
		--local	iAttackRange = action:GetSkillRange(0,actor)
		local iMissileMoveDist = action:GetParamInt(10)
		
		local kMoveVec = actor:GetLookingDir()
		kMoveVec:Multiply(iMissileMoveDist)
		local kActorPos = actor:GetPos()
		local kTargetPos = kActorPos
		kTargetPos:Add(kMoveVec)		
		if( CheckNil(g_world == nil) ) then return false end
		if( CheckNil(g_world:IsNil()) ) then return false end
		kTargetPos = g_world:ThrowRay(kTargetPos, Point3(0,0,-1),500)
		if kTargetPos:GetX() == -1 and kTargetPos:GetY() == -1 and kTargetPos:GetZ() == -1 then
			kTargetPos = kActorPos	-- Ray가 실패할 경우
		end
		if(kTargetPos:GetZ() > kActorPos:GetZ()) then 
			kTargetPos:SetZ(0)
		end
		
		kTargetPos:SetZ(kTargetPos:GetZ()+40)
		
--		local	kProjectileMan = GetProjectileMan()		
--		local	iTargetCount = action:GetTargetCount()
		local	iTargetABVShapeIndex = 0
	
		-- 회전축 구하기

		local	kActionTargetList = action:GetTargetList()
		
			local	kTargetActor = nil
			local	kActionTargetInfo = kActionTargetList:GetTargetInfo(0)
			iTargetABVShapeIndex = 0
			
			if kActionTargetInfo:IsNil() == false then
			
				iTargetABVShapeIndex = kActionTargetInfo:GetABVIndex()
				local	kTargetPilot = g_pilotMan:FindPilot(kActionTargetInfo:GetTargetGUID())
				if kTargetPilot:IsNil() == false then
					kTargetActor = kTargetPilot:GetActor()
				end			
				
			end

			local	kArrow = Skill_Nuclear_LoadToWeapon(actor,action)
			local	arrow_worldpos = kArrow:GetWorldPos()	--	화살의 위치
		
			if kTargetActor ~= nil and kTargetActor:IsNil() == false and kActionTargetInfo~=nil and kActionTargetInfo:IsNil() == false then
				kArrow:SetTargetObject(kActionTargetInfo)	
				kActionTargetList:DeleteTargetInfo(0)
			end
				
			kArrow:SetMovingType(1)
			--kArrow:SetParam_SinCurveLineType(math.random(50, 70), math.random(-45, 45)*DEGTORAD)
			kArrow:SetParam_SinCurveLineType(110, 0)
			kArrow:SetParamValue("TIME", tostring(GetWorldAccumTime()))
			kArrow:SetTargetLoc(kTargetPos)
			kArrow:Fire(true)--	발사!

	
		actor:GetAction():SetParam(7,"FIRED")
		actor:DetachFrom(7216, true)	-- 주머니에서 꺼낸 미사일 연출 제거
		actor:ClearIgnoreEffectList()
	elseif ("free" == textKey) then
		actor:FreeMove(false)
		action:SetParam(1, "end")
	end
end
