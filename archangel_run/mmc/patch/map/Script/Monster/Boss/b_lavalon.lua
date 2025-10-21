-------------------------------------------------------------------------
-- Lavalon Boss : field에 생성되었을 때 호출 되는 함수
function MonsterAI_6000900(ulElapsedTime, monster, arg)

end

function MonsterGen_6000900(monster, arg)
	monster:SetAbil(AT_CURRENT_ACTION, 0, false)
	local iGage = monster:GetAbil(AT_HP_GAGE)
	monster:SetAbil(AT_C_HP_GAGE, iGage-1, false)

	local kGround = Ground(arg:Get(ACTARG_GROUND))

	--monster:SetSPData(0,1) -- 공중 공격 확율 증가
	--monster:SetSPData(1,0) -- 공중 공격 횟수

	-- Sending Boss monster
	local kPacket = NewPacket(PT_M_C_NFY_BOSSMONSTER)
	kPacket:PushGuid(monster:GetGuid())
	local emptyGuid = GUID()
	emptyGuid:Empty()
	kGround:Broadcast(kPacket,monster,emptyGuid)
	DeletePacket(kPacket)

	local kDecorator = kGround:GetDecorator()
	if kDecorator:IsNil() == false then
		kDecorator:SetActorSlot("BOSS", monster:GetGuid())
		kDecorator:ReserveStage(0)
	end

	-- begin the first level. (Appear)
	local kUnitItor = NewObjectMgr_UnitIterator()
	kGround:GetFirstUnitType(UT_PLAYER, kUnitItor)
	local kUnit = kGround:GetNextUnitType(UT_PLAYER, kUnitItor) 
	while kUnit:IsNil() == false do
		kUnit:SetAbil(AT_GOD_TIME, 0)
		kUnit = kGround:GetNextUnitType(UT_PLAYER, kUnitItor)
	end --while문 끝
	DeleteObjectMgr_UnitIterator(kUnitItor)
end

function MonsterDie_6000900(monster, arg)

end

function Init_Lavalon(Decorator)
end

function Enter_Lavalon()

end

function Leave_Lavalon()
	-- Decorator 정리.
end


function Stage_Lavalon_Level0_OnEnter(kStage)
	kStage:ForceNextAction("a_lavalon_appear")
	return true
end

function Stage_Lavalon_Level0_OnUpdate(frameTime, kStage)
	local curAction = kStage:GetAction()
	if curAction:IsNil() then
		return true
	end

	local kGround = kStage:GetGround()
	local actionID = curAction:GetID()

	if curAction:IsDone() and
		actionID == "a_lavalon_appear" then
		return false
	end

	return true
end


function Stage_Lavalon_Level0_OnLeave(frameTime, kStage)
	return true
end

function Stage_Lavalon_Level1_OnEnter(kStage)
	kStage:SetParamInt(0, 0)	-- dmg cnt
	kStage:SetParamInt(1, 0)	-- dead
	kStage:SetParamInt(2, 0)	-- accumtime
	kStage:SetParamInt(3, 0)	-- last meteor time
	return true
end

function Stage_Lavalon_Level1_OnUpdate(frameTime, kStage)
	local curAction = kStage:GetAction()
	if curAction:IsNil() then
		return true
	end

	local kGround = kStage:GetGround()
	local actionID = curAction:GetID()
	local actorGUID = kStage:GetActorGuid(curAction:GetActorID())
	local iDmgCnt = kStage:GetParamInt(0)
	local iDead = kStage:GetParamInt(1)
	local iAccumTime = kStage:GetParamInt(2) + frameTime * 1000
	local iLastMeteorTime = kStage:GetParamInt(3)

	kStage:SetParamInt(2, iAccumTime)
--	InfoLog(9, "________Accum : " .. iAccumTime .. " frame : " .. frameTime .. "_______________")

	-- 용암탄을 랜덤하게 떨어뜨림
	if iDead == 0 then
		if math.random(1000) < 100 then
			-- 100ms당 10%확률
			WorldAct_Meteor(kGround)	-- 용암탄은 랜덤.
		end

		if iAccumTime - iLastMeteorTime > 6000 + math.random(-1000, 1000) then
			kStage:SetParamInt(3, iAccumTime)

			--WorldAct_Meteor_Blue(kGround)
			local kUnitItor = NewObjectMgr_UnitIterator()
			kGround:GetFirstUnitType(UT_PLAYER,kUnitItor)

			local kUnit = kGround:GetNextUnitType(UT_PLAYER, kUnitItor)
			while kUnit:IsNil() == false do
				if kUnit:IsUnitType(UT_PLAYER) and math.random() > 0.2 then
					local kPos = kUnit:GetPos()
					-- 메테오를 떨어지게 하자.
					WorldAct_Meteor_Blue(kGround, kPos:GetX(), kPos:GetY(), kPos:GetZ())
				end -- 존 캐릭터 가져오기 끝
				kUnit = kGround:GetNextUnitType(UT_PLAYER,kUnitItor)
			end --while문 끝
			DeleteObjectMgr_UnitIterator(kUnitItor)
		end	
	end

	if kStage:IsCurrentActionDone() and actionID == "a_lavalon_die" then
		InfoLog(9, "______________Start Result__________")
		kGround:StartResult(500)
		return false 
	end

	local kBoss = kGround:GetUnit(actorGUID)
	if kBoss:IsNil() then
		return true 
	end

	local iMaxHP = kBoss:GetAbil(AT_C_MAX_HP)
	local iCurrentHP = kBoss:GetAbil(AT_HP)
	-- ForceNextAction을 하면 이전 액션을 끊어 먹는다.
	if iCurrentHP <= 0 and iDead == 0 then
		kStage:SetParamInt(1, 1)	-- 죽었다는거 표시.
		kStage:ForceNextAction("a_lavalon_die")
		InfoLog(9, "______________Start Die__________")
		return true
	elseif kStage:IsCurrentActionDone() then
		if actionID == "a_lavalon_dmg_01" then
			kStage:ForceNextAction("a_lavalon_powerbreak")
		end
	end

	local fDamagedPer = (iMaxHP - iCurrentHP) / iMaxHP * 100
	local fThreshold = 30	-- 30퍼센트 마다 데미지 줌.
	if fDamagedPer - (iDmgCnt * fThreshold) > fThreshold and iDead == 0 then
		kStage:SetParamInt(0, iDmgCnt + 1)
		if actionID ~= "a_lavalon_dmg_01" then
			kStage:ForceNextAction("a_lavalon_dmg_01")
		end
	end

	return true
end

function Stage_Lavalon_Level1_OnLeave(kDecorator, kNextStage)
	return true
end

----------------------------------------------------------------
-- Lavalon Action
----------------------------------------------------------------
function Act_Lavalon_Appear_OnEnter(kAction, kGround, kGuid, kActArg)
	--InfoLog(9, "Lavalon_Appear_OnEnter  ....00")

	local kLavalon = kGround:GetUnit(kGuid)
	kLavalon:SetState(US_FIRE)

	local kSkill = kLavalon:GetSkill()
	kSkill:Reserve(130)

	return true
end

function Act_Lavalon_Appear_OnUpdate(kAction, elapsedTime, durationTime, kGround, kGuid, kActArg)
	return true
end

function Act_Lavalon_Appear_OnLeave(kAction, kGround, kGuid)
	--InfoLog(8, "Start Result 0")
	kGround:StartResult(0)
	return true
end

function Act_Lavalon_Idle_OnEnter(kAction, kGround, kGuid, kActArg)
	return true
end

function Act_Lavalon_Idle_OnUpdate(kAction, elapsedTime, durationTime, kGround, kGuid, kActArg)
	return true
end

function Act_Lavalon_Idle_OnLeave(kAction, kGround, kGuid)
	return true
end

function Act_Lavalon_Breath_Center_OnEnter(kAction, kGround, kGuid, kActArg)
	local kLavalon = kGround:GetUnit(kGuid)
	kLavalon:SetState(US_SKILL_CAST)

	local kSkill = kLavalon:GetSkill()
	kSkill:Reserve(900000001)
	kActArg:SetInt(ACT_ARG_CUSTOMDATA1, 0)	-- Breath Count
	return true
end

function Act_Lavalon_Breath_Center_OnUpdate(kAction, elapsedTime, durationTime, kGround, kGuid, kActArg)
	local kLavalon = kGround:GetUnit(kGuid)
	local kSkillDef = GetSkillDef(900000001)
	if kSkillDef:IsNil() then
		--InfoLog(5, "Skill_Fire900000001....SkillDef is Nil")
		return true 
	end

	if elapsedTime >= durationTime - 1000 then
		return true
	end

	local iBreathCnt = kActArg:GetInt(ACT_ARG_CUSTOMDATA1)
	local iCastTime = kSkillDef:GetAbil(AT_CAST_TIME)

	if (elapsedTime - iCastTime) - (iBreathCnt * 800) > 800 then
		-- Fired를 리셋 해줌으로써, 데미지를 다시 보낸다.
		kActArg:SetInt(ACT_ARG_CUSTOMDATA1, iBreathCnt + 1)

		--InfoLog(9, "Skill_Fire900000001 1.0")
		kUnitArray = NewUNIT_PTR_ARRAY()
		local iDetectRange = 120
	
		--InfoLog(9, "Skill_Fire900000001 1.1")
		local kTopLeft = kGround:GetNodePosition("pt_blaze_01")		-- Top Left
		local kTopRight = kGround:GetNodePosition("pt_blaze_02")	-- Top Right
		local kBottomLeft = kGround:GetNodePosition("pt_blaze_03")	-- Bottom Left
		local kBottomRight = kGround:GetNodePosition("pt_blaze_04")	-- Bottm Right
	
		--InfoLog(9, "Skill_Fire900000001 1.2")
		local kUpperBase = kTopRight:Subtract(kTopLeft)
		local kRightBase = kTopRight:Subtract(kBottomRight)
		local kBottomBase = kBottomRight:Subtract(kBottomLeft)
		local kLeftBase = kTopLeft:Subtract(kBottomLeft)
	
		--InfoLog(9, "Skill_Fire900000001 2.0")
		local akBasePos = {kTopLeft, kBottomRight, kBottomLeft, kBottomLeft}
		local akAlpha = {kUpperBase, kRightBase, kBottomBase, kLeftBase}
	
		--InfoLog(9, "Skill_Fire900000001 2.1")
		local fMetaForStart = 0.5
		local fMetaForEnd = 0.5
		
		--InfoLog(9, "Skill_Fire900000001 2.2")
		local kStartPos = kUpperBase:Multiply(fMetaForStart)
		kStartPos = kStartPos:Add(kTopLeft)
		kStartPos:SetZ(0)
	
		--InfoLog(9, "Skill_Fire900000001 2.3")
		local kEndPos = kBottomBase:Multiply(fMetaForEnd)
		kEndPos = kEndPos:Add(kBottomLeft)
		kEndPos:SetZ(0)
	
		--InfoLog(9, "Skill_Fire900000001 2.4")
		kGround:GetUnitInWidthFromLine(kStartPos, kEndPos, iDetectRange, UT_PLAYER, kUnitArray)
	
		local iIndex = 0
		local target = kUnitArray:GetUnit(iIndex)
		while target:IsNil() == false do
			--InfoLog(9, "Skill_Fire900000001 3.0")
			if kLavalon:IsEnemy(target, false) then	-- Enemy에게만 적용시킨다.
				--InfoLog(9, "Skill_Fire900000001 3.1")
				--WA_ATTACKDAMAGE_UNIT(kSkillDef:GetAbil(AT_PHY_ATTACK_MAX), kSkillDef:GetAbil(AT_PHY_ATTACK_MIN), kSkillDef:GetAbil(AT_SKILLPOWER_RATE), kLavalon, target, kGround)
				WA_ATTACKDAMAGE_UNIT(300, 400, kLavalon, target, kGround)
			end
		
			iIndex = iIndex + 1
			target = kUnitArray:GetUnit(iIndex)
			--InfoLog(9, "Skill_Fire102000301 3.0 [" .. iIndex .. "]")
		end
	
		DeleteUNIT_PTR_ARRAY(kUnitArray)
	end

	return true
end

function Act_Lavalon_Breath_Center_OnLeave(kAction, kGround, kGuid)
	local kLavalon = kGround:GetUnit(kGuid)
	kLavalon:SetState(US_IDLE)
	return true
end

function Act_Lavalon_Breath_Left_OnEnter(kAction, kGround, kGuid, kActArg)
	local kLavalon = kGround:GetUnit(kGuid)
	kLavalon:SetState(US_SKILL_CAST)

	local kSkill = kLavalon:GetSkill()
	kSkill:Reserve(900000011)
	kActArg:SetInt(ACT_ARG_CUSTOMDATA1, 0)	-- Breath Count
	return true
end

function Act_Lavalon_Breath_Left_OnUpdate(kAction, elapsedTime, durationTime, kGround, kGuid, kActArg)
	local kLavalon = kGround:GetUnit(kGuid)
	local kSkillDef = GetSkillDef(900000011)
	if kSkillDef:IsNil() then
		--InfoLog(5, "Skill_Fire900000011....SkillDef is Nil")
		return true 
	end

	if elapsedTime >= durationTime - 900 then
		return true
	end
	
	local iBreathCnt = kActArg:GetInt(ACT_ARG_CUSTOMDATA1)
	local iCastTime = kSkillDef:GetAbil(AT_CAST_TIME)

	if (elapsedTime - iCastTime) - (iBreathCnt * 800) > 800 then
		-- Fired를 리셋 해줌으로써, 데미지를 다시 보낸다.
		kActArg:SetInt(ACT_ARG_CUSTOMDATA1, iBreathCnt + 1)

		--InfoLog(9, "Skill_Fire900000011 1.0")
		kUnitArray = NewUNIT_PTR_ARRAY()
		local iDetectRange = 120
	
		--InfoLog(9, "Skill_Fire900000011 1.1")
		local kTopLeft = kGround:GetNodePosition("pt_blaze_01")		-- Top Left
		local kTopRight = kGround:GetNodePosition("pt_blaze_02")	-- Top Right
		local kBottomLeft = kGround:GetNodePosition("pt_blaze_03")	-- Bottom Left
		local kBottomRight = kGround:GetNodePosition("pt_blaze_04")	-- Bottm Right
	
		--InfoLog(9, "Skill_Fire900000011 1.2")
		local kUpperBase = kTopRight:Subtract(kTopLeft)
		local kRightBase = kTopRight:Subtract(kBottomRight)
		local kBottomBase = kBottomRight:Subtract(kBottomLeft)
		local kLeftBase = kTopLeft:Subtract(kBottomLeft)
	
		--InfoLog(9, "Skill_Fire900000011 2.0")
		local akBasePos = {kTopLeft, kBottomRight, kBottomLeft, kBottomLeft}
		local akAlpha = {kUpperBase, kRightBase, kBottomBase, kLeftBase}
	
		--InfoLog(9, "Skill_Fire900000011 2.1")
		local fMetaForStart = 0.55
		local fMetaForEnd = 0.83
		
		--InfoLog(9, "Skill_Fire900000011 2.2")
		local kStartPos = kUpperBase:Multiply(fMetaForStart)
		kStartPos = kStartPos:Add(kTopLeft)
		kStartPos:SetZ(0)
	
		--InfoLog(9, "Skill_Fire900000011 2.3")
		local kEndPos = kBottomBase:Multiply(fMetaForEnd)
		kEndPos = kEndPos:Add(kBottomLeft)
		kEndPos:SetZ(0)
	
		--InfoLog(9, "Skill_Fire900000011 2.4")
		kGround:GetUnitInWidthFromLine(kStartPos, kEndPos, iDetectRange, UT_PLAYER, kUnitArray)
	
		local iIndex = 0
		local target = kUnitArray:GetUnit(iIndex)
		while target:IsNil() == false do
			--InfoLog(9, "Skill_Fire900000011 3.0")
			if kLavalon:IsEnemy(target, false) then	-- Enemy에게만 적용시킨다.
				--InfoLog(9, "Skill_Fire900000011 3.1")
				--WA_ATTACKDAMAGE_UNIT(kSkillDef:GetAbil(AT_PHY_ATTACK_MAX), kSkillDef:GetAbil(AT_PHY_ATTACK_MIN), kSkillDef:GetAbil(AT_SKILLPOWER_RATE), kLavalon, target, kGround)
				WA_ATTACKDAMAGE_UNIT(240, 365, kLavalon, target, kGround)
			end
		
			iIndex = iIndex + 1
			target = kUnitArray:GetUnit(iIndex)
			--InfoLog(9, "Skill_Fire102000301 3.0 [" .. iIndex .. "]")
		end
	
		DeleteUNIT_PTR_ARRAY(kUnitArray)
	end

	return true
end

function Act_Lavalon_Breath_Left_OnLeave(kAction, kGround, kGuid)
	local kLavalon = kGround:GetUnit(kGuid)
	kLavalon:SetState(US_IDLE)
	return true
end

function Act_Lavalon_Breath_Right_OnEnter(kAction, kGround, kGuid, kActArg)
	local kLavalon = kGround:GetUnit(kGuid)
	kLavalon:SetState(US_SKILL_CAST)

	local kSkill = kLavalon:GetSkill()
	kSkill:Reserve(900000021)
	kActArg:SetInt(ACT_ARG_CUSTOMDATA1, 0)	-- Breath Count
	return true
end

function Act_Lavalon_Breath_Right_OnUpdate(kAction, elapsedTime, durationTime, kGround, kGuid, kActArg)
	local kLavalon = kGround:GetUnit(kGuid)
	local kSkillDef = GetSkillDef(900000031)
	if kSkillDef:IsNil() then
		--InfoLog(5, "Skill_Fire900000021....SkillDef is Nil")
		return true 
	end

	if elapsedTime >= durationTime - 900 then
		return true
	end

	local iBreathCnt = kActArg:GetInt(ACT_ARG_CUSTOMDATA1)
	local iCastTime = kSkillDef:GetAbil(AT_CAST_TIME)

	if (elapsedTime - iCastTime) - (iBreathCnt * 800) > 800 then
		-- Fired를 리셋 해줌으로써, 데미지를 다시 보낸다.
		kActArg:SetInt(ACT_ARG_CUSTOMDATA1, iBreathCnt + 1)

		--InfoLog(9, "Skill_Fire900000021 1.0")
		kUnitArray = NewUNIT_PTR_ARRAY()
		local iDetectRange = 90 
	
		--InfoLog(9, "Skill_Fire900000021 1.1")
		local kTopLeft = kGround:GetNodePosition("pt_blaze_01")		-- Top Left
		local kTopRight = kGround:GetNodePosition("pt_blaze_02")	-- Top Right
		local kBottomLeft = kGround:GetNodePosition("pt_blaze_03")	-- Bottom Left
		local kBottomRight = kGround:GetNodePosition("pt_blaze_04")	-- Bottm Right
	
		--InfoLog(9, "Skill_Fire900000021 1.2")
		local kUpperBase = kTopRight:Subtract(kTopLeft)
		local kRightBase = kTopRight:Subtract(kBottomRight)
		local kBottomBase = kBottomRight:Subtract(kBottomLeft)
		local kLeftBase = kTopLeft:Subtract(kBottomLeft)
	
		--InfoLog(9, "Skill_Fire900000021 2.0")
		local akBasePos = {kTopLeft, kBottomRight, kBottomLeft, kBottomLeft}
		local akAlpha = {kUpperBase, kRightBase, kBottomBase, kLeftBase}
	
		--InfoLog(9, "Skill_Fire900000021 2.1")
		local fMetaForStart = 0.28
		local fMetaForEnd = 0.25
		
		--InfoLog(9, "Skill_Fire900000021 2.2")
		local kStartPos = kUpperBase:Multiply(fMetaForStart)
		kStartPos = kStartPos:Add(kTopLeft)
		kStartPos:SetZ(0)
	
		--InfoLog(9, "Skill_Fire900000021 2.3")
		local kEndPos = kBottomBase:Multiply(fMetaForEnd)
		kEndPos = kEndPos:Add(kBottomLeft)
		kEndPos:SetZ(0)
	
		--InfoLog(9, "Skill_Fire900000021 2.4")
		kGround:GetUnitInWidthFromLine(kStartPos, kEndPos, iDetectRange, UT_PLAYER, kUnitArray)
	
		local iIndex = 0
		local target = kUnitArray:GetUnit(iIndex)
		while target:IsNil() == false do
			--InfoLog(9, "Skill_Fire900000021 3.0")
			if kLavalon:IsEnemy(target, false) then	-- Enemy에게만 적용시킨다.
				--InfoLog(9, "Skill_Fire900000021 3.1")
				--WA_ATTACKDAMAGE_UNIT(kSkillDef:GetAbil(AT_PHY_ATTACK_MAX), kSkillDef:GetAbil(AT_PHY_ATTACK_MIN), kSkillDef:GetAbil(AT_SKILLPOWER_RATE), kLavalon, target, kGround)
				WA_ATTACKDAMAGE_UNIT(240, 365, kLavalon, target, kGround)
			end
		
			iIndex = iIndex + 1
			target = kUnitArray:GetUnit(iIndex)
			--InfoLog(9, "Skill_Fire900000021 3.0 [" .. iIndex .. "]")
		end
	
		DeleteUNIT_PTR_ARRAY(kUnitArray)
	end

	return true
end

function Act_Lavalon_Breath_Right_OnLeave(kAction, kGround, kGuid)
	local kLavalon = kGround:GetUnit(kGuid)
	kLavalon:SetState(US_IDLE)
	return true
end

function Act_Lavalon_Blaze_OnOverridePacket(kGround, kPacket, kUnit, kActArg)
	--				1
	--     	---------------------
	--     	|					|
	--	4	|					|    2
	--		|					|
	--		|					|
	--		|					|
	--     	---------------------
	--				3
	
	local kStartLoc = {1, 4, 2}
	local kEndLoc = {3, 2, 4}

	local iMaxHP = kUnit:GetAbil(AT_C_MAX_HP)
	local iCurrentHP = kUnit:GetAbil(AT_HP)

	local fPercent = iCurrentHP / iMaxHP

	-- Line 개수는 보스 피가 작을 수록 많게 하자.
	local iNbLine = 5 - math.floor((2 * fPercent))
	local iLineIdx = 0
	kPacket:PushInt(iNbLine)
	kActArg:SetInt(ACT_ARG_CUSTOMDATA1, iNbLine)

	while iLineIdx < iNbLine do
		local iSelectedIndex = math.random(1,3)
		local fStartPos = kStartLoc[iSelectedIndex] + math.random()
		local fEndPos = kEndLoc[iSelectedIndex] + math.random()

		kPacket:PushFloat(fStartPos)
		kPacket:PushFloat(fEndPos)
		kActArg:SetInt(ACT_ARG_CUSTOMDATA1 + iLineIdx * 2 + 1, fStartPos * 1000)
		kActArg:SetInt(ACT_ARG_CUSTOMDATA1 + iLineIdx * 2 + 2, fEndPos * 1000)
		--InfoLog(9, "_____________Blaze,  Start : " .. fStartPos .. " , End : " .. fEndPos)
		iLineIdx = iLineIdx + 1
	end

	return true
end

function Act_Lavalon_Blaze_OnEnter(kAction, kGround, kGuid, kActArg)
	local kLavalon = kGround:GetUnit(kGuid)
	kLavalon:SetState(US_SKILL_CAST)

	local kSkill = kLavalon:GetSkill()
	kSkill:Reserve(900000031)
	return true
end

function Act_Lavalon_Blaze_OnUpdate(kAction, elapsedTime, durationTime, kGround, kGuid)
	local kLavalon = kGround:GetUnit(kGuid)
	local kActArg = NewActArg()

	local kSkillDef = GetSkillDef(900000031)
	if kSkillDef:IsNil() then
		--InfoLog(5, "Skill_Fire900000031....SkillDef is NIl" ..skillnum)
		return true 
	end

	return true
end

function Act_Lavalon_Blaze_OnLeave(kAction, kGround, kGuid)
	local kLavalon = kGround:GetUnit(kGuid)
	kLavalon:SetState(US_IDLE)
	return true
end

function Act_Lavalon_Damage_OnEnter(kAction, kGround, kGuid, kActArg)
	return true
end

function Act_Lavalon_Damage_OnUpdate(kAction, elapsedTime, durationTime, kGround, kGuid, kActArg)
	return true
end

function Act_Lavalon_Damage_OnLeave(kAction, kGround, kGuid)
	return true
end

function Act_Lavalon_Die_OnEnter(kAction, kGround, kGuid, kActArg)
--	local iIndex = 1
--	local emptyGuid = GUID()
--	emptyGuid:Empty()
--
--	local kUnitItor = NewObjectMgr_UnitIterator()
--	kGround:GetFirstUnitType(UT_PLAYER, kUnitItor)
--
--	local kUnit = kGround:GetNextUnitType(UT_PLAYER, kUnitItor)
--	while kUnit:IsNil() == false do
--		if kUnit:IsUnitType(UT_PLAYER) then
--			local kNodeName = string.format('char_spawn_%d', iIndex)
--			local kPos = kGround:GetNodePosition(kNodeName)
--			local kPacket = NewPacket(PT_M_C_UNIT_POS_CHANGE)
--			kPacket:PushGuid(kUnit:GetGuid())
--			kPacket:PushPoint3(kPos)
--			kPacket:PushBool(false)
--			kGround:Broadcast(kPacket, GetDummyUnit(), emptyGuid)
--			InfoLog(9, "[WA_Die_On_Enter] char spawn : " .. kPos:GetX() .. " , " .. kPos:GetY() .. " , " .. kPos:GetZ())
--			DeletePacket(kPacket)
--		end 
--		kUnit = kGround:GetNextUnitType(UT_PLAYER, kUnitItor)
--		iIndex = iIndex + 1
--	end --while문 끝
--	DeleteObjectMgr_UnitIterator(kUnitItor)

	return true
end

function Act_Lavalon_Die_OnUpdate(kAction, elapsedTime, durationTime, kGround, kGuid, kActArg)
	return true
end

function Act_Lavalon_Die_OnLeave(kAction, kGround, kGuid)
	return true
end

function Act_Lavalon_Powerbreak_OnEnter(kAction, kGround, kGuid, kActArg)
	local kLavalon = kGround:GetUnit(kGuid)
	kLavalon:SetState(US_SKILL_CAST)

	local kSkill = kLavalon:GetSkill()
	kSkill:Reserve(141)
	return true
end

function Act_Lavalon_Powerbreak_OnUpdate(kAction, elapsedTime, durationTime, kGround, kGuid, kActArg)
	return true
end

function Act_Lavalon_Powerbreak_OnLeave(kAction, kGround, kGuid)
	local kLavalon = kGround:GetUnit(kGuid)
	kLavalon:SetState(US_IDLE)
	return true
end

function Act_Lavalon_Powerbreak_OnOverridePacket(kGround, kPacket, kUnit, kActArg)
	local iMaxHP = kUnit:GetAbil(AT_C_MAX_HP)
	local iCurrentHP = kUnit:GetAbil(AT_HP)
	local fDamagedPer = (iMaxHP - iCurrentHP) / iMaxHP * 100

	local iIndex = math.max(math.floor(fDamagedPer / 30) - 1, 0)
	kPacket:PushInt(iIndex)
	kActArg:SetInt(ACT_ARG_CUSTOMDATA1, iIndex)

	return true
end


----------------------------------------------------------------------------
-- 라발론 용암탄 World Action
----------------------------------------------------------------------------

function WorldAct_Meteor(kGround)
	local kActArg = NewActArg()
	kActArg:Set(ACTARG_GROUND, kGround:GetObject())

	local kGuid = kGround:AddWorldAction(WATYPE_ATTACKUNIT, WA_METEOR, ETICK_INTERVAL_100MS, kActArg)
	kGround:BeginWorldActionTick(kGuid)
	DeleteActArg(kActArg)
end

function WA_METEOR_OnEnter(kAction, dwElapsedTime, arg)
	local kGround = Ground(arg:Get(ACTARG_GROUND))

	-- Meteor가 뿌려질 좌표..
	local kTopLeft = kGround:GetNodePosition("pt_blaze_01")		-- Top Left
	local kTopRight = kGround:GetNodePosition("pt_blaze_02")	-- Top Right
	local kBottomLeft = kGround:GetNodePosition("pt_blaze_03")	-- Bottom Left
	local kBottomRight = kGround:GetNodePosition("pt_blaze_04")	-- Bottm Right

	-- 위 좌표는 반드시 의미에 부합해야 한다.
	local firstPos = kTopRight:Subtract(kTopLeft)
	firstPos = firstPos:Multiply(math.random())
	firstPos = firstPos:Add(kTopLeft)

	local secondPos = kBottomRight:Subtract(kBottomLeft)
	secondPos = secondPos:Multiply(math.random())
	secondPos = secondPos:Add(kBottomLeft)

	local finalPos = firstPos:Subtract(secondPos)
	finalPos = finalPos:Multiply(math.random())
	finalPos = finalPos:Add(secondPos)
		
	-- Argument 설정.
	arg:SetInt(ACT_ARG_CUSTOMDATA1, finalPos:GetX())
	arg:SetInt(ACT_ARG_CUSTOMDATA1+1, finalPos:GetY())
	arg:SetInt(ACT_ARG_CUSTOMDATA1+2, finalPos:GetZ())

	-- Packet 생성.
	local kPacket = NewPacket(PT_M_C_NFY_BEGIN_WORLDACTION)
	kPacket:PushInt(kAction:GetType())
	kPacket:PushInt(timeGetTime() - kAction:GetBeginTime())
	kPacket:PushFloat(finalPos:GetX())
	kPacket:PushFloat(finalPos:GetY())
	kPacket:PushFloat(finalPos:GetZ())
	kPacket:PushInt(0)

	--InfoLog(9, "[WA_Meteor_OnEnter] Meteor Pos : " .. finalPos:GetX() .. " , " .. finalPos:GetY() .. " , " .. finalPos:GetZ() .. "\n")

	-- Packet을 보내자.
	local emptyGuid = GUID()
	emptyGuid:Empty()
	kGround:Broadcast(kPacket, GetDummyUnit(), emptyGuid)
	DeletePacket(kPacket)
	
	return true
end

function WA_METEOR_OnTick(kAction, dwElapsedTime, arg)
	-- 시간 잰 뒤에..
	local bEnd = false
	local kGround = Ground(arg:Get(ACTARG_GROUND))
	local iCurrentTime = timeGetTime() -- 현재 시간
	local iBeginTime = kAction:GetBeginTime() -- 액션 시작한 시간
	local iTotalElapsedTime = iCurrentTime - iBeginTime -- 진행된 시간
	local iDuration = kAction:GetDuration() 

	if iTotalElapsedTime > iDuration - 130 then
		-- 캐릭터가 맞았는지 안 맞았는지 체크..
		-- 패킷 보내고 종료.
		--InfoLog(9, "[WA_Meteor_OnTick] Meteor is done now, start collision check\n")
		local kPosX = arg:GetInt(ACT_ARG_CUSTOMDATA1)
		local kPosY = arg:GetInt(ACT_ARG_CUSTOMDATA1+1)
		local kPosZ = arg:GetInt(ACT_ARG_CUSTOMDATA1+2)
	
		--InfoLog(9, "[WA_Meteor_OnTick] Pos X : " .. kPosX .. " , Y : " .. kPosY .. "\n")
		local kCollisionPos = Point3(kPosX, kPosY, kPosZ)
		kCollisionPos:SetZ(0)
		--InfoLog(9, "[WA_Meteor_OnTick] Final Pos X : " .. kCollisionPos:GetX() .. " , Y : " .. kCollisionPos:GetY())
		
		local kUnitItor = NewObjectMgr_UnitIterator()
		kGround:GetFirstUnitType(UT_PLAYER,kUnitItor)

		local kUnit = kGround:GetNextUnitType(UT_PLAYER, kUnitItor)
		while kUnit:IsNil() == false do
			if kUnit:IsUnitType(UT_PLAYER) then
				local kPos = kUnit:GetPos()
				kPos:SetZ(0)
				local fDist = GetDistance(kPos, kCollisionPos)
				--InfoLog(9, "[WA_Meteor_OnTick] Check Distance : " .. fDist)
				if fDist <= 55 then
					if kAction:AddAttackUnit(kUnit:GetGuid(), iCurrentTime, 1200) == true then
						--InfoLog(9, "[WA_Meteor_OnTick] occur ths collision[1.1] !!")
						WA_ATTACKDAMAGE_UNIT(150, 250, GetDummyUnit(), kUnit, kGround)
					end
				end
			end -- 존 캐릭터 가져오기 끝
			kUnit = kGround:GetNextUnitType(UT_PLAYER,kUnitItor)
		end --while문 끝
		DeleteObjectMgr_UnitIterator(kUnitItor)
	end
	
	return true
end

function WA_METEOR_OnLeave(kAction, dwElapsedTime, arg)
	return true
end


function WorldAct_Meteor_Blue(kGround, x, y, z)
	local kActArg = NewActArg()
	kActArg:Set(ACTARG_GROUND, kGround:GetObject())
	kActArg:SetInt(ACT_ARG_CUSTOMDATA1 + 1001, x)
	kActArg:SetInt(ACT_ARG_CUSTOMDATA1 + 1002, y)
	kActArg:SetInt(ACT_ARG_CUSTOMDATA1 + 1003, z)

	local kGuid = kGround:AddWorldAction(WATYPE_ATTACKUNIT, WA_METEOR_BLUE, ETICK_INTERVAL_100MS, kActArg)
	kGround:BeginWorldActionTick(kGuid)
	DeleteActArg(kActArg)
end

function WA_METEOR_BLUE_OnEnter(kAction, dwElapsedTime, arg)
	local kGround = Ground(arg:Get(ACTARG_GROUND))

	--local ptTarget = string.format('pt_meteor_%02d', math.random(1,2))
	--local finalPos = kGround:GetNodePosition(ptTarget)

	local finalPos = Point3(0, 0, 0)
	finalPos:SetX(arg:GetInt(ACT_ARG_CUSTOMDATA1 + 1001))
	finalPos:SetY(arg:GetInt(ACT_ARG_CUSTOMDATA1 + 1002))
	finalPos:SetZ(arg:GetInt(ACT_ARG_CUSTOMDATA1 + 1003))

	-- Argument 설정.
	arg:SetInt(ACT_ARG_CUSTOMDATA1, finalPos:GetX())
	arg:SetInt(ACT_ARG_CUSTOMDATA1+1, finalPos:GetY())
	arg:SetInt(ACT_ARG_CUSTOMDATA1+2, finalPos:GetZ())

	-- Packet 생성.
	local kPacket = NewPacket(PT_M_C_NFY_BEGIN_WORLDACTION)
	kPacket:PushInt(kAction:GetType())
	kPacket:PushInt(timeGetTime() - kAction:GetBeginTime())
	kPacket:PushFloat(finalPos:GetX())
	kPacket:PushFloat(finalPos:GetY())
	kPacket:PushFloat(finalPos:GetZ())
	kPacket:PushInt(1)

	--InfoLog(9, "[WA_Meteor_Blue_OnEnter] Meteor Pos : " .. finalPos:GetX() .. " , " .. finalPos:GetY() .. " , " .. finalPos:GetZ() .. "\n")

	-- Packet을 보내자.
	local emptyGuid = GUID()
	emptyGuid:Empty()
	kGround:Broadcast(kPacket, GetDummyUnit(), emptyGuid)
	DeletePacket(kPacket)

	return true
end

function WA_METEOR_BLUE_OnTick(kAction, dwElapsedTime, arg)
	-- 시간 잰 뒤에..
	local bEnd = false
	local kGround = Ground(arg:Get(ACTARG_GROUND))
	local iCurrentTime = timeGetTime() -- 현재 시간
	local iBeginTime = kAction:GetBeginTime() -- 액션 시작한 시간
	local iTotalElapsedTime = iCurrentTime - iBeginTime -- 진행된 시간
	local iDuration = kAction:GetDuration() 

	if iTotalElapsedTime > iDuration - 130 then
		-- 캐릭터가 맞았는지 안 맞았는지 체크..
		-- 패킷 보내고 종료.
		--InfoLog(9, "[WA_Meteor_Blue_OnTick] Meteor is done now, start collision check\n")
		local kPosX = arg:GetInt(ACT_ARG_CUSTOMDATA1)
		local kPosY = arg:GetInt(ACT_ARG_CUSTOMDATA1+1)
		local kPosZ = arg:GetInt(ACT_ARG_CUSTOMDATA1+2)
	
		--InfoLog(9, "[WA_Meteor_OnTick] Pos X : " .. kPosX .. " , Y : " .. kPosY .. "\n")
		local kCollisionPos = Point3(kPosX, kPosY, kPosZ)
		kCollisionPos:SetZ(0)
		--InfoLog(9, "[WA_Meteor_OnTick] Final Pos X : " .. kCollisionPos:GetX() .. " , Y : " .. kCollisionPos:GetY())
		
		local kUnitItor = NewObjectMgr_UnitIterator()
		kGround:GetFirstUnitType(UT_PLAYER,kUnitItor)

		local kUnit = kGround:GetNextUnitType(UT_PLAYER, kUnitItor)
		while kUnit:IsNil() == false do
			if kUnit:IsUnitType(UT_PLAYER) then
				local kPos = kUnit:GetPos()
				kPos:SetZ(0)
				local fDist = GetDistance(kPos, kCollisionPos)
				--InfoLog(9, "[WA_Meteor_OnTick] Check Distance : " .. fDist)
				if fDist <= 55 then
					if kAction:AddAttackUnit(kUnit:GetGuid(), iCurrentTime, 1200) == true then
						--InfoLog(9, "[WA_Meteor_OnTick] occur ths collision[1.1] !!")
						WA_ATTACKDAMAGE_UNIT(200, 350, GetDummyUnit(), kUnit, kGround)
					end
				end
			end -- 존 캐릭터 가져오기 끝
			kUnit = kGround:GetNextUnitType(UT_PLAYER,kUnitItor)
		end --while문 끝
		DeleteObjectMgr_UnitIterator(kUnitItor)
	end
	
	return true
end

function WA_METEOR_BLUE_OnLeave(kAction, dwElapsedTime, arg)
	return true
end
