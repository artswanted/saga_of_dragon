---------------------------------------------------------------------------
-- Saphire Boss 의 AI Tick
function MonsterAI_200008(ulElapsedTime, monster, arg)
	local pkGround = arg:Get(ACTARG_GROUND)
	pkGround = Ground(pkGround)

	-- 아직 appear이 시작하지 않았으면 tick을 돌리지 말자
	if monster:GetAbil(AT_MONSTER_APPEAR) == 0 then
		return
	end

	local iHP = monster:GetAbil(AT_HP)
	local iHPValue = ( ( monster:GetAbil(AT_C_MAX_HP)) / 4 ) * ( monster:GetAbil(AT_C_HP_GAGE) )

	-- HP가 낮아지면 데미지 입는 연출 출력
	if iHP <= iHPValue and iHPValue > 0 and iHP > 0 then
		monster:SetAbil(AT_C_HP_GAGE,monster:GetAbil(AT_C_HP_GAGE)-1, false)
		SendWorldAction_NoTick(monster,pkGround,WA_SB_DAMAGE)
		InfoLog(9, "SendWorldAction_NoTick ....SB_Damage")
		
		-- 3초간 묶어둬야 한다.
		monster:SetAbil(AT_AI_DELAY,3000,false)

		-- 데미지 입으면 몹을 소환해보자!
--  	for i = 0 , 35 , 6 do
--    	WA_SB_SUMMON_MONSTER_200008(unitmgr,0)
--    end
	end

	local iSPDelay = monster:GetSPData(1) - ulElapsedTime
	if iSPDelay > 0 then
		monster:SetSPData(1,iSPDelay)
	else
		MonsterAI_200008_MakeRandomAttack(monster,arg)
	end

	local iState = monster:GetState()
	if iState == US_IDLE then
		local iDelay = monster:GetAbil(AT_AI_DELAY) - ulElapsedTime
		if iDelay > 0 then
			monster:SetAbil(AT_AI_DELAY,iDelay, false)
			return
		end
		monster:SetAbil(AT_AI_DELAY,0, false)
		MonsterAI_200008_FindTarget(monster,arg)
	elseif iState == US_WATCHPLAYER then
		iDelay = monster:GetAbil(AT_AI_DELAY) - ulElapsedTime
		if iDelay > 0 then
			monster:SetAbil(AT_AI_DELAY,iDelay, false)
			InfoLog(9, "SendWorldAction_NoTick ....Rotation")
			SendWorldAction_NoTick(monster,pkGround,WA_SB_ROTATION)
			return
		end
		monster:SetAbil(AT_AI_DELAY,0, false)
		monster:SetState(US_ATTACK)
		MonsterAI_200008_DoNextAttack(monster, arg)
	elseif iState == US_DEAD then
		return
	elseif iState == US_ATTACK then
		iDelay = monster:GetAbil(AT_AI_DELAY) - ulElapsedTime
		if iDelay > 0 then
		-- Attacking TIME ------
		--InfoLog(9, "MonsterAI_200008     40  DelayTime = " ..iDelay)
			monster:SetAbil(AT_AI_DELAY,iDelay, false)
			return
		end
		monster:SetState(US_IDLE)
		if monster:GetSPData(0) > 0 then
			monster:SetAbil(AT_AI_DELAY,0, false)
		else
			monster:SetAbil(AT_AI_DELAY,300, false)
		end
	else
		monster:SetState(US_ATTACK)
	end
end

-------------------------------------------------------------------
-- Saphire Boss가 다음 공격을 위해 타겟을 찾는다!
function MonsterAI_200008_FindTarget(monster,arg)
	InfoLog(9, "MonsterAI_200008_FindTarget     00")

	-- Finding Target
	local pkGround = arg:Get(ACTARG_GROUND)
	pkGround = Ground(pkGround)
	local kTGuid = monster:GetTarget()
	local kTarget = nil
	local randtarget = RAND(2)
	if kTGuid:IsNil() == false then
		kTarget = pkGround:GetUnit(kTGuid)
		if kTarget:IsNil() == false then
			if (pkGround:IsAttackable(AT_C_ATTACK_RANGE, monster, kTarget,0)  == false) then
				InfoLog(9, "IsAttackable is false")
				kTarget:Empty()
			elseif randtarget == 1 then  -- 0.5 확률로 Target 재설정
				InfoLog(9, "IsAttackable is ture and random target is changed ....["..randtarget.."]")
				kTarget:Empty()
			end
		end
	end
	--InfoLog(9, "MonsterAI_200008_DoNextAttack     20")
	if kTarget == nil or kTarget:IsNil() == true then
		kTarget = pkGround:FindRandomUnit(monster:GetPos(),-1,UT_PLAYER)
		if kTarget:IsNil() == true then
			if pkGround:GetPlayerNum() <= 0 then
				---------------------------------------------------
				------------  Player가 없으면 SB가 스스로 죽는 시나리오
				InfoLog(9, "------Monster DieDieDie")
				monster:OnDamaged(ALL_UNIT_MAX_HP, pkGround,kTarget)	-- 무조건 죽이기
				---------------------------------------------------
				return
			else
				-----------------------------------------
				---------- 원래 시나리오 소스
				-- Cannot find Target player
				monster:SetState(US_IDLE)
				monster:SetAbil(AT_AI_DELAY,200, false)
				InfoLog(9, "PlayerNum is true and taget is NULL")
				--------------------------------------------
				return
			end
		end
	end
	

	if kTarget:IsNil() == false then
		--InfoLog(9, "MonsterAI_200008_DoNextAttack     30")
		monster:SetTarget(kTarget:GetGuid())
		-- Sending Target Lock ....
		kPacket = NewPacket(PT_M_C_NFY_TARGETUNIT)
		kPacket:PushGuid(monster:GetGuid())
		kPacket:PushGuid(kTarget:GetGuid())
		local emptyGuid = GUID()
		emptyGuid:Empty()
		pkGround:Broadcast(kPacket,monster,emptyGuid)
		DeletePacket(kPacket)

		monster:SetState(US_WATCHPLAYER)
		if monster:GetSPData(0) > 0 then
			monster:SetAbil(AT_AI_DELAY,0, false)
		else
			monster:SetAbil(AT_AI_DELAY,500, false)
		end
	end
end
-------------------------------------------------------------------------
-- Saphire Boss 의 다음공격 패턴을 결정하고 World Action 을 실행시킨다.
function MonsterAI_200008_DoNextAttack(monster, arg)
	InfoLog(9, "MonsterAI_200008_DoNextAttack     00")

	-- Finding Target
	local pkGround = Ground(arg:Get(ACTARG_GROUND))

	local kTGuid = monster:GetTarget()
	local kTarget = nil
	if kTGuid:IsNil() == false then
	InfoLog(9, "MonsterAI_200008_DoNextAttack     Player is not Nill")
		kTarget = pkGround:GetUnit(kTGuid)
		if kTarget:IsNil() == false then
			InfoLog(9, "MonsterAI_200008_DoNextAttack     GetUnit is true")
			if (pkGround:IsAttackable(AT_C_ATTACK_RANGE, monster, kTarget,0)  == false) then
				InfoLog(9, "MonsterAI_200008_DoNextAttack     IsAttackable is false")
				kTarget:Empty()
			end
		end
	end
	--InfoLog(9, "MonsterAI_200008_DoNextAttack     20")
	if kTarget:IsNil() == true then
		InfoLog(9, "MonsterAI_200008_DoNextAttack     kTarget is Nill")
		monster:SetState(US_IDLE)
		monster:SetAbil(AT_AI_DELAY,300, false)
		return
	end

	local mng = arg:Get(ACTARG_PUPPETMNG)
	mng = PuppetMng(mng)

	-- 일단은 Random 하게 다음 공격 선택
	local bEnd = false
	local iIndex = 0
	local BeforeAttack = monster:GetAbil(AT_CURRENT_ACTION)

	local kWAction
	local iSPData = monster:GetSPData(0)
	local irand = 0
	if iSPData > 0 then
		kWAction = mng:NewWorldAction(WATYPE_ATTACKUNIT, WA_SB_ATTACK6)
		monster:SetAbil(AT_CURRENT_ACTION,WA_SB_ATTACK6)
		monster:SetSPData(0,iSPData-1, false)
	else
		gage = monster:GetAbil(AT_C_HP_GAGE)
		iAttackNum = 4 + ( 5 - gage )
		while bEnd == false do
			iChoose = RAND(10000)

			bAttack6 = false
			irand = RAND(10000)
			if irand <= ( ( 5 - gage ) * 800 ) then
				bAttack6 = true -- 강제적으로 Attack6을 행하게 한다.
			end

			InfoLog(7, "MonsterAI_200008_DoNextAttack....iChoose[" .. iChoose .."]")
			if iChoose < ( 10000 / iAttackNum ) and bAttack6 == false then
				if BeforeAttack ~= WA_SB_ATTACK1 or iIndex == 2 then
					kWAction = mng:NewWorldAction(WATYPE_ATTACKUNIT, WA_SB_ATTACK1)
					monster:SetAbil(AT_CURRENT_ACTION,WA_SB_ATTACK1, false)
					bEnd = true
				end
			elseif iChoose < ( ( 10000 / iAttackNum ) * 2 ) and bAttack6 == false then
				if BeforeAttack ~= WA_SB_ATTACK3 or iIndex == 2 then
					kWAction = mng:NewWorldAction(WATYPE_ATTACKUNIT, WA_SB_ATTACK2)
					monster:SetAbil(AT_CURRENT_ACTION,WA_SB_ATTACK2, false)
					bEnd = true
				end
			elseif iChoose < ( ( 10000 / iAttackNum ) * 3 ) and bAttack6 == false then
				if BeforeAttack ~= WA_SB_ATTACK3 or iIndex == 2 then
					kWAction = mng:NewWorldAction(WATYPE_ATTACKUNIT, WA_SB_ATTACK3)
					monster:SetAbil(AT_CURRENT_ACTION,WA_SB_ATTACK3, false)
					bEnd = true
				end
			elseif iChoose < ( ( 10000 / iAttackNum ) * 4 ) and bAttack6 == false then
				if BeforeAttack ~= WA_SB_ATTACK4 or iIndex == 2 then
					if RAND(5) < 1 then
						kWAction = mng:NewWorldAction(WATYPE_ATTACKUNIT, WA_SB_ATTACK5)
					else
						kWAction = mng:NewWorldAction(WATYPE_ATTACKUNIT, WA_SB_ATTACK4)
					end
					monster:SetAbil(AT_CURRENT_ACTION,WA_SB_ATTACK4, false)
					bEnd = true
				end
			elseif iChoose < ( ( 10000 / iAttackNum ) * 5 ) or bAttack6 == true then
				if BeforeAttack ~= WA_SB_ATTACK6 or iIndex == 2 or bAttack6 == true then
					kWAction = mng:NewWorldAction(WATYPE_ATTACKUNIT, WA_SB_ATTACK6)
					monster:SetAbil(AT_CURRENT_ACTION,WA_SB_ATTACK6, false)
					bEnd = true
					monster:SetSPData(0,5)
				end
			elseif iChoose < ( ( 10000 / iAttackNum ) * 6 ) and bAttack6 == false then
				if BeforeAttack ~= WA_SB_ATTACK7 or iIndex == 2 then
					kWAction = mng:NewWorldAction(WATYPE_ATTACKUNIT, WA_SB_ATTACK7)
					monster:SetAbil(AT_CURRENT_ACTION,WA_SB_ATTACK7, false)
					bEnd = true
				end
			elseif iChoose < ( ( 10000 / iAttackNum ) * 7 ) then
				if BeforeAttack ~= WA_SB_ATTACK8 or iIndex == 2 then
					kWAction = mng:NewWorldAction(WATYPE_ATTACKUNIT, WA_SB_ATTACK8)
					monster:SetAbil(AT_CURRENT_ACTION,WA_SB_ATTACK8, false)
					bEnd = true
				end
			elseif iChoose < ( ( 10000 / iAttackNum ) * 8 ) and bAttack6 == false then
				if BeforeAttack ~= WA_SB_ATTACK9 or iIndex == 2 then
					kWAction = mng:NewWorldAction(WATYPE_ATTACKUNIT, WA_SB_ATTACK9)
					monster:SetAbil(AT_CURRENT_ACTION,WA_SB_ATTACK9, false)
					bEnd = true
				end
			end
			iIndex = iIndex + 1
	--  else
			--kWAction = mng:NewWorldAction(WATYPE_ATTACKUNIT, WA_SB_ATTACK8)
			--monster:SetAbil(AT_AI_DELAY,150000, false)
	--  end
		end
	end

	--InfoLog(9, "MonsterAI_200008_DoNextAttack     30")
	mng:AddWorldAction(kWAction)--
	local act2 = NewActArg()
	act2:Set(ACTARG_WORLDACTION, kWAction:GetObject())
	act2:Set(ACTARG_PUPPETMNG, mng:GetObject())
	act2:Set(ACTARG_GROUND, pkGround:GetObject())
	MonsterAI_200008_MakeActArg(kWAction, kTarget,monster, act2)
	mng:BeginWorldActionTick2(kWAction, ETICK_INTERVAL_100MS, act2)--
	kWAction:Begin()
	DeleteActArg(act2)

	InfoLog(7, "MonsterAI_200008_DoNextAttack....Choosed Attack[" .. kWAction:GetType() .."]")

	------------  DEBUGGING  -------------------------------
	--if iChoose > 600 then
	--  InfoLog(9, "------Monster DieDieDie")
	--  monster:OnDamaged(ALL_UNIT_MAX_HP, pkGround)	-- 무조건 죽이기
	--end
	--------------------------------------------------------
end

-------------------------------------------------------------------------
-- Saphire Boss : field에 생성되었을 때 호출 되는 함수
function MonsterGen_200008(monster, arg)
	monster:SetAbil(AT_CURRENT_ACTION, 0, false)
	monster:SetAbil(AT_C_HP_GAGE,4-1, false)

	local pkGround = Ground(arg:Get(ACTARG_GROUND))

	-- Sending Boss monster
	local kPacket = NewPacket(PT_M_C_NFY_BOSSMONSTER)
	kPacket:PushGuid(monster:GetGuid())
	local emptyGuid = GUID()
	emptyGuid:Empty()
	pkGround:Broadcast(kPacket,monster,emptyGuid)
	DeletePacket(kPacket)
	monster:SetAbil(AT_AI_DELAY,20000, false)

	-- Begin : WA_SAPHIREBOSS_APPEAR
	local mng = arg:Get(ACTARG_PUPPETMNG)
	mng = PuppetMng(mng)
	local kWAction = mng:NewWorldAction(WATYPE_BASE, WA_SAPHIREBOSS_APPEAR)
	mng:AddWorldAction(kWAction)
	local act2 = NewActArg()
	act2:Set(ACTARG_WORLDACTION, kWAction:GetObject())
	act2:Set(ACTARG_PUPPETMNG, mng:GetObject())
	act2:Set(ACTARG_GROUND, pkGround:GetObject())
	mng:BeginWorldActionTick2(kWAction, ETICK_INTERVAL_100MS, act2)
	kWAction:Begin()
	DeleteActArg(act2)
end

-------------------------------------------------------------------------
-- Saphire Boss : 죽었을 때 호출되는 함수
function MonsterDie_200008(monster,ground, arg)
	InfoLog(9, "MonsterDie_200008 ....00")
	-- Begin : WA_SAPHIREBOSS_DIE
	local mng = arg:Get(ACTARG_PUPPETMNG)
	mng = PuppetMng(mng)
	local kWAction = mng:NewWorldAction(WATYPE_BASE, WA_SAPHIREBOSS_DIE)
	mng:AddWorldAction(kWAction)
	local act2 = NewActArg()
	act2:Set(ACTARG_WORLDACTION, kWAction:GetObject())
	act2:Set(ACTARG_PUPPETMNG, mng:GetObject())
	act2:Set(ACTARG_GROUND, ground:GetObject())
	act2:Set(ACTARG_CUSTOM_UNIT1,monster:GetObject())
	mng:BeginWorldActionTick2(kWAction, ETICK_INTERVAL_100MS, act2)
	kWAction:Begin()
	DeleteActArg(act2)
end

-- Building Argument
function MonsterAI_200008_MakeActArg(kAction, kUnit,kMonster, arg)
	-- kAction : WorldAction
	-- kUnit : Target Player
	-- kMonster : Boss Monster
	-- arg :[OUT] making argument
	local iType = kAction:GetType()
	InfoLog(9, "MonsterAI_200008_MakeActArg  ....iType = " ..iType)
	if iType == WA_SB_ATTACK1 then
		arg:SetInt(ACT_ARG_CUSTOMDATA1, RAND(2))
		arg:SetInt(ACT_ARG_CUSTOMDATA1+1, MonsterAI_200008_GetBlockIndex(kUnit:GetPos()))
		kMonster:SetAbil(AT_AI_DELAY,18500+1200, false)
	elseif iType == WA_SB_ATTACK2 then
		arg:SetInt(ACT_ARG_CUSTOMDATA1, RAND(2))
		arg:SetInt(ACT_ARG_CUSTOMDATA1+1, MonsterAI_200008_GetBlockIndex(kUnit:GetPos()))
		iCount = RAND(14) + 6
		arg:SetInt(ACT_ARG_CUSTOMDATA1+2, iCount)
		kMonster:SetAbil(AT_AI_DELAY,1600+(400*iCount)+5000+500, false)
	elseif iType == WA_SB_ATTACK3 then
		arg:SetInt(ACT_ARG_CUSTOMDATA1, MonsterAI_200008_GetBlockIndex(kUnit:GetPos()))
		kMonster:SetAbil(AT_AI_DELAY,1300+13800+1200, false)
	elseif iType == WA_SB_ATTACK4 then
		kMonster:SetAbil(AT_AI_DELAY,7000+500, false)
	elseif iType == WA_SB_ATTACK5 then
		kMonster:SetAbil(AT_AI_DELAY,7000+500, false)
	elseif iType == WA_SB_ATTACK6 then
		iBlock = MonsterAI_200008_GetBlockIndex(kUnit:GetPos())
		arg:SetInt(ACT_ARG_CUSTOMDATA1, RAND(2))
		arg:SetInt(ACT_ARG_CUSTOMDATA1+1, iBlock)
		kMonster:SetAbil(AT_AI_DELAY,850+300, false)

		InfoLog(4, "MonsterAI_200008_MakeActArg....GetBlockIndex[" .. iBlock .."]")
	elseif iType == WA_SB_ATTACK7 then
		arg:SetInt(ACT_ARG_CUSTOMDATA1, RAND(2))
		arg:SetInt(ACT_ARG_CUSTOMDATA1+1, MonsterAI_200008_GetBlockIndex(kUnit:GetPos()))
		kMonster:SetAbil(AT_AI_DELAY,7000+500, false)
	elseif iType == WA_SB_ATTACK8 then
		arg:SetInt(ACT_ARG_CUSTOMDATA1, MonsterAI_200008_GetBlockIndex(kUnit:GetPos()))
		kMonster:SetAbil(AT_AI_DELAY,2400+(400*9)+2000+500, false)
	elseif iType == WA_SB_ATTACK9 then
		arg:SetInt(ACT_ARG_CUSTOMDATA1, RAND(2))
		arg:SetInt(ACT_ARG_CUSTOMDATA1+1, MonsterAI_200008_GetBlockIndex(kUnit:GetPos()))
		iCount = RAND(14) + 6
		arg:SetInt(ACT_ARG_CUSTOMDATA1+2, iCount)
		kMonster:SetAbil(AT_AI_DELAY,1600+(400*iCount)+800+500+16500, false)
	end
end

function MonsterAI_200008_GetBlockIndex(kPos)
	InfoLog(9, "MonsterAI_200008_GetBlockIndex ....00")
	local fX = kPos:GetX()
	local fY = kPos:GetY()
	local fHodo = math.atan2(fY,fX) / math.pi * 180
	if fHodo <= 0 then
		fHodo = fHodo + 360
	end

	local iIndex = ( fHodo + 5 ) / 10
	iIndex = math.floor(iIndex)
	if iIndex >= 36 then
		iIndex = iIndex - 36
	end
	InfoLog(9, "MonsterAI_200008_GetBlockIndex ....99   RETURN " ..iIndex)
	return iIndex
end

function MonsterAI_200008_MakeRandomAttack(monster, arg)
	local pkGround = Ground(arg:Get(ACTARG_GROUND))

	local kTarget = monster:GetTarget()
	if kTarget:IsNil() == true then
		return
	end

	local mng = arg:Get(ACTARG_PUPPETMNG)
	mng = PuppetMng(mng)

	local kWAction = mng:NewWorldAction(WATYPE_ATTACKUNIT, WA_SB_ATTACK10)

	mng:AddWorldAction(kWAction)--
	local act2 = NewActArg()
	act2:Set(ACTARG_WORLDACTION, kWAction:GetObject())
	act2:Set(ACTARG_PUPPETMNG, mng:GetObject())
	act2:Set(ACTARG_GROUND, pkGround:GetObject())

	local MaxIndex = 36
	local Index = {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32,33,34,35}

	local iTotalNum = 6 + RAND(3)
	act2:SetInt(ACT_ARG_CUSTOMDATA1, iTotalNum)
	for i = 1 , iTotalNum do
		local index = RAND(MaxIndex)
		local block = Index[index]
		for j = index , (iTotalNum-1) do
			Index[j] = Index[j+1]
		end
		act2:SetInt(ACT_ARG_CUSTOMDATA1 + ( (i-1) * 3 ) + 1, block)
		local blocktime = (1+RAND(4))*1000
		act2:SetInt(ACT_ARG_CUSTOMDATA1 + ( (i-1) * 3 ) + 2, blocktime)
		local blocktype = RAND(2)
		act2:SetInt(ACT_ARG_CUSTOMDATA1 + ( (i-1) * 3 ) + 3, blocktype)

		iTotalNum = iTotalNum - 1
	end

	mng:BeginWorldActionTick2(kWAction, ETICK_INTERVAL_100MS, act2)
	kWAction:Begin()
	DeleteActArg(act2)

	monster:SetSPData(1,7000)

	return
end

