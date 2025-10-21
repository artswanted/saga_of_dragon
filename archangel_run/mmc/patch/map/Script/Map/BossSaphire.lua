function Ground_Init9010600(arg)
	InfoLog(9, "Ground_Init9010600     00")
	local mng = arg:Get(ACTARG_PUPPETMNG)
	mng = PuppetMng(mng)  
	local kGroupPuppet = mng:NewPuppet(PUPPET_T_PUPPETGROUP, PUPPET_OT_NONE)
	mng:Add(kGroupPuppet)
	
	if kGroupPuppet:IsNil() == true then
		InfoLog(4, "Ground_Init9010600....AddPuppet(PUPPET_T_PUPPETGROUP) failed")
		return
	end

	local ptZero = Point3(0.0, 0.0, 0.0)
	
	local fRadius = 460.0
	local fX = 460.0
	local fY = 0.0
	local fRadian = math.pi / 180 

	local fDegree = 0.0
	for iIndex = 0, 35 do
		local kSubPuppet = mng:NewPuppet(PUPPET_T_TRANSFORMCIRCLE, PUPPET_OT_BOSSZONE_TILE)
		if kSubPuppet:IsNil() == true then
			InfoLog(4, "Ground_Init9010600....AddPuppet(PUPPET_T_TRANSFORM) failed")
			return
		end
		fX = math.cos( 10 * iIndex * fRadian ) * fRadius
		fY = math.sin( 10 * iIndex * fRadian ) * fRadius
		local kQuat = Quaternion( fDegree / math.pi, ptZero)
		kSubPuppet:SetTransformQuaternion(kQuat:GetW(), kQuat:GetX(), kQuat:GetY(), kQuat:GetZ())
		kSubPuppet:SetTransformTranslate(ptZero)
		kSubPuppet:SetTransformScale(1.0)
		kGroupPuppet:AddSubpuppet(2, kSubPuppet)
		local CirclePos = Point3(fX,fY,0.0)
		kSubPuppet:SetPosition(CirclePos,60,35)
		fDegree = fDegree + 10
	end
	local kBeginTrigger = mng:NewPuppet(PUPPET_T_TRANSFORMCIRCLE, PUPPET_OT_NONE)
	kBeginTrigger:SetScript("SaphireBoss_BeginTrigger")
	local ptPos = Point3(-8.0, -460.0, 5.0) -- 27번 인덱스

	kBeginTrigger:SetPosition(ptPos, 60,0)

	mng:Add(kBeginTrigger)
	local act2 = NewActArg()
	act2:Set(ACTARG_PUPPET, kBeginTrigger:GetObject())
	local pkGround = Ground(arg:Get(ACTARG_GROUND))
	act2:Set(ACTARG_GROUND, pkGround:GetObject())
	act2:Set(ACTARG_PUPPETMNG, mng:GetObject())
	mng:BeginPuppetTick2(kBeginTrigger, ETICK_INTERVAL_100MS, act2)
	DeleteActArg(act2)
	InfoLog(7, "Ground Init lua   (Ground_Init9010600) success..")
end

--------------------------------------------------------------------------------
function SaphireBoss_BeginTrigger(dwElapsedTime, arg)
	--InfoLog(9, "SaphireBoss_BeginTrigger  ....00")
	local kPuppet = arg:Get(ACTARG_PUPPET)
	kPuppet = Puppet(kPuppet)
	local ptPos = kPuppet:GetPosition()
	local iSize = kPuppet:GetSize()
	
	--InfoLog(9, "SaphireBoss_BeginTrigger  ....10")
	local pkGround = Ground(arg:Get(ACTARG_GROUND))
	local player = pkGround:FindUnit(UT_PLAYER, ptPos, iSize)
	if player:IsNil() == true then
		return false
	end

	local monster = pkGround:GetBossMonster(M_SAPHIREBOSS)
	-- Boss monster Gen.....
	if monster:IsNil() == true then
		local ptPos = Point3(0, 0, 80)
		InfoLog(9, "Ground:MonsterGenerate")
		pkGround:MonsterGenerate(M_SAPHIREBOSS,ptPos,3000)
	end
	
	-- Remove Self
	--InfoLog(9, "SaphireBoss_BeginTrigger  ....40")
	local kGuid = kPuppet:GetID()
	local mng = arg:Get(ACTARG_PUPPETMNG)
	mng = PuppetMng(mng)
	mng:EndPuppetTick(ETICK_INTERVAL_100MS, kGuid)
	mng:DeletePuppet(kGuid)
	--InfoLog(9, "SaphireBoss_BeginTrigger  ....99")
	return true
end

--------------------------------------------------------------------------------
function WA_SB_MakePacket(kWAction, kPacket, arg)
	-- kWAction : WorldAction
	-- kUnitMgr : UnitMgr
	-- kPacket : packet to send

	local iType = kWAction:GetType()
	kPacket:PushInt(iType)
	InfoLog(4, "WA_SB_MakePacket  ....WA Type  " ..iType) 
	kPacket:PushInt(timeGetTime() - kWAction:GetBeginTime())
	if iType == WA_SAPHIREBOSS_APPEAR then
	elseif iType == WA_SAPHIREBOSS_DIE then
	elseif iType == WA_SB_CRYSTAL_APPEAR then
		-- GUID of Crystal (Sended outer)
	elseif iType == WA_SB_ATTACK1 then
		kPacket:PushChar(arg:GetInt(ACT_ARG_CUSTOMDATA1))
		kPacket:PushInt(arg:GetInt(ACT_ARG_CUSTOMDATA1+1))
	elseif iType == WA_SB_ATTACK2 then
		kPacket:PushChar(arg:GetInt(ACT_ARG_CUSTOMDATA1))
		kPacket:PushInt(arg:GetInt(ACT_ARG_CUSTOMDATA1+1))
		kPacket:PushInt(arg:GetInt(ACT_ARG_CUSTOMDATA1+2))
	elseif iType == WA_SB_ATTACK3 then
		kPacket:PushInt(arg:GetInt(ACT_ARG_CUSTOMDATA1))
	elseif iType == WA_SB_ATTACK4 then
	elseif iType == WA_SB_ATTACK5 then
	elseif iType == WA_SB_ATTACK6 then
		kPacket:PushChar(arg:GetInt(ACT_ARG_CUSTOMDATA1))
		kPacket:PushInt(arg:GetInt(ACT_ARG_CUSTOMDATA1+1))
	elseif iType == WA_SB_ATTACK7 then
		kPacket:PushChar(arg:GetInt(ACT_ARG_CUSTOMDATA1))
		kPacket:PushInt(arg:GetInt(ACT_ARG_CUSTOMDATA1+1))
	elseif iType == WA_SB_ATTACK8 then
		kPacket:PushInt(arg:GetInt(ACT_ARG_CUSTOMDATA1))
	elseif iType == WA_SB_ATTACK9 then
		kPacket:PushChar(arg:GetInt(ACT_ARG_CUSTOMDATA1))
		kPacket:PushInt(arg:GetInt(ACT_ARG_CUSTOMDATA1+1))
		kPacket:PushInt(arg:GetInt(ACT_ARG_CUSTOMDATA1+2))
	elseif iType == WA_SB_ATTACK10 then
		iMaxNum = arg:GetInt(ACT_ARG_CUSTOMDATA1)
		kPacket:PushChar(iMaxNum)
		
		for i = 0 , iMaxNum do
			kPacket:PushChar(arg:GetInt(ACT_ARG_CUSTOMDATA1 + ( i * 3 ) + 1))
			kPacket:PushInt(arg:GetInt(ACT_ARG_CUSTOMDATA1 + ( i * 3 ) + 2))
			kPacket:PushChar(arg:GetInt(ACT_ARG_CUSTOMDATA1 + ( i * 3 ) + 3))
		end
	end
end

--------------------------------------------------------------------------------
function WA_SAPHIREBOSS_APPEAR_OnEnter(dwElapsedTime, arg)
	InfoLog(9, "WA_SAPHIREBOSS_APPEAR_OnEnter ....00")
	local pkGround = Ground(arg:Get(ACTARG_GROUND))
	local monster = pkGround:GetBossMonster(M_SAPHIREBOSS)
	if monster:IsNil() == true then
		InfoLog(4, "WA_SAPHIREBOSS_APPEAR_OnEnter Monster is Nill....00")
		return false
	end

	local kAction = arg:Get(ACTARG_WORLDACTION)
	kAction = WorldAction(kAction)
	local kPacket = NewPacket(PT_M_C_NFY_BEGIN_WORLDACTION)
	WA_SB_MakePacket(kAction, kPacket, arg)
	local emptyGuid = GUID()
	emptyGuid:Empty()
	pkGround:Broadcast(kPacket,monster,emptyGuid)
	DeletePacket(kPacket)

	monster:SetSPData(1,20000)
 	monster:SetAbil(AT_MONSTER_APPEAR,1, false)

	return true
end

function WA_SAPHIREBOSS_APPEAR_OnTick(dwElapsedTime, arg)
--	InfoLog(9, "WA_SAPHIREBOSS_APPEAR ....00")
	-- Saphire Boss Appear....
	local ulSBTime = 20000
	local kAction = arg:Get(ACTARG_WORLDACTION)
	kAction = WorldAction(kAction)

	local pkGround = Ground(arg:Get(ACTARG_GROUND))

	local bEnd = false
	local monster = pkGround:GetBossMonster(M_SAPHIREBOSS)

	if monster:IsNil() == true then
		bEnd = true
	elseif monster:GetAbil(AT_HP) <= 0 then
		bEnd = true
	end

	if (timeGetTime() - kAction:GetBeginTime()) > ulSBTime or bEnd == true then
		-- Self Delete
		local mng = arg:Get(ACTARG_PUPPETMNG)
		mng = PuppetMng(mng)
		local kGuid = kAction:GetID()
		mng:EndWorldActionTick(ETICK_INTERVAL_100MS, kGuid)
		mng:DeleteWorldAction(kGuid)
		
		if monster:IsNil() == false then
			monster:SetState(US_IDLE)
		end

		InfoLog(4, "Appear End")
	end
	return true
end

--------------------------------------------------------------------------------
function WA_SAPHIREBOSS_DIE_OnEnter(dwElapsedTime, arg)
	InfoLog(9, "WA_SAPHIREBOSS_DIE_OnEnter  ....00")
	local kAction = arg:Get(ACTARG_WORLDACTION)
	kAction = WorldAction(kAction)
	local kPacket = NewPacket(PT_M_C_NFY_BEGIN_WORLDACTION)
	WA_SB_MakePacket(kAction, kPacket, arg)
	local pkGround = Ground(arg:Get(ACTARG_GROUND))
	local monster = Unit(arg:Get(ACTARG_CUSTOM_UNIT1))
	local emptyGuid = GUID()
	emptyGuid:Empty()
	pkGround:Broadcast(kPacket,monster,emptyGuid)
	DeletePacket(kPacket)
	return true
end

function WA_SAPHIREBOSS_DIE_OnTick(dwElapsedTime, arg)
	-- Saphire Boss Die
	local ulSBTime = 15000
	local kAction = arg:Get(ACTARG_WORLDACTION)
	kAction = WorldAction(kAction)

	if (timeGetTime() - kAction:GetBeginTime()) < ulSBTime then
		return true
	end

	-- Self Delete
	local mng = arg:Get(ACTARG_PUPPETMNG)
	mng = PuppetMng(mng)
	local kGuid = kAction:GetID()
	mng:EndWorldActionTick(ETICK_INTERVAL_100MS, kGuid)
	mng:DeleteWorldAction(kGuid)
	
	-- Begin : WA_SB_CRYSTAL_APPEAR
	mng = arg:Get(ACTARG_PUPPETMNG)
	mng = PuppetMng(mng)
	kWAction = mng:NewWorldAction(WATYPE_BASE, WA_SB_CRYSTAL_APPEAR)
	mng:AddWorldAction(kWAction)
	local act2 = NewActArg()
	act2:Set(ACTARG_WORLDACTION, kWAction:GetObject())
	act2:Set(ACTARG_PUPPETMNG, mng:GetObject())
	act2:Set(ACTARG_GROUND, arg:Get(ACTARG_GROUND))
	mng:BeginWorldActionTick2(kWAction, ETICK_INTERVAL_100MS, act2)
	kWAction:Begin()
	DeleteActArg(act2)  

	local pkGround = Ground(arg:Get(ACTARG_GROUND))
	local monster = Unit(arg:Get(ACTARG_CUSTOM_UNIT1))
	if monster:IsNil() == true then
	  return false
	end

	-- Packet : Boss Monster Delete
--	local kBossPacket = NewPacket(PT_M_C_NFY_REMOVE_MONSTER)
--	kBossPacket:PushInt(1)
--	kBossPacket:PushGuid(monster:GetGuid())
--	local emptyGuid = GUID()
--	emptyGuid:Empty()
--	pkGround:Broadcast(kBossPacket,monster,emptyGuid)
--	DeletePacket(kBossPacket)

	return true
end

--------------------------------------------------------------------------------
function WA_SB_CRYSTAL_APPEAR_OnEnter(dwElapsedTime, arg)
	InfoLog(9, "WA_SB_CRYSTAL_APPEAR_OnEnter  ....00")
	local pkGround = Ground(arg:Get(ACTARG_GROUND))

	-- Create Crystal (as NPC)
--	local kCrystalGuid = GUID()
--	kCrystalGuid:Generate()
--	pkGround:AddNPC("100038", "c_crystal", "sb_crystal_portal", "0, 0, 10",  kCrystalGuid, 1942)

--	local kCrystal = pkGround:GetUnit(kCrystalGuid)
--	if kCrystal:IsNil() == true then
--		return false
--	end
--	arg:Set(ACTARG_CUSTOM_UNIT1, kCrystal:GetObject())

	local kAction = arg:Get(ACTARG_WORLDACTION)
	kAction = WorldAction(kAction)
	-- Sending World Action
	kPacket = NewPacket(PT_M_C_NFY_BEGIN_WORLDACTION)
	WA_SB_MakePacket(kAction, kPacket, arg)
--	kCrystal:WriteToPacket(kPacket)
	local emptyGuid = GUID()
	emptyGuid:Empty()
	local kCrystal = Unit()
	kCrystal:Empty()
	pkGround:Broadcast(kPacket,kCrystal,emptyGuid)
	DeletePacket(kPacket)

	InfoLog(9, "CRYSTAL_APPEAR_SendPacket....99")

	return true
end

function WA_SB_CRYSTAL_APPEAR_OnTick(dwElapsedTime, arg)
	-- Crystal Appear (after Saphire Boss die)
	-- Saphire Boss Die
	local ulSBTime = 60000
	local kAction = arg:Get(ACTARG_WORLDACTION)
	kAction = WorldAction(kAction)

	if (timeGetTime() - kAction:GetBeginTime()) < ulSBTime then
		return true
	end
	
	local pkGround = Ground(arg:Get(ACTARG_GROUND))

	-- Self Delete
	local mng = arg:Get(ACTARG_PUPPETMNG)
	mng = PuppetMng(mng)
	local kGuid = kAction:GetID()
	mng:EndWorldActionTick(ETICK_INTERVAL_100MS, kGuid)
	mng:DeleteWorldAction(kGuid)  

	---------------------------------------------------------------------------------
-- SB가 죽고 나서 다시 Regen 될 수 있도록 설정한 부분
	---------------------------------------------------------------------------------
	-- Crystal Delete
--	kCrystal = arg:Get(ACTARG_CUSTOM_UNIT1)
--	kCrystal = Unit(kCrystal)
--	pkGround:ReleaseUnit(kCrystal)

	-- Puppet Trigger generate  
--  local kBeginTrigger = mng:NewPuppet(PUPPET_T_TRANSFORMCIRCLE, PUPPET_OT_NONE)
--  kBeginTrigger:SetScript("SaphireBoss_BeginTrigger")
-- 	local ptPos = Point3(-8.0, -460.0, 5.0) -- 27번 인덱스
--  kBeginTrigger:SetPosition(ptPos, 60)
--  mng:Add(kBeginTrigger)
--  act2 = NewActArg()
--  act2:Set(ACTARG_PUPPET, kBeginTrigger:GetObject())
--  act2:Set(ACTARG_GROUND, pkGround:GetObject())
--  act2:Set(ACTARG_PUPPETMNG, mng:GetObject())
--  mng:BeginPuppetTick2(kBeginTrigger, ETICK_INTERVAL_100MS, act2)
--  DeleteActArg(act2)
	---------------------------------------------------------------------------------
	---------------------------------------------------------------------------------
	return true
end

---------------------------------------------------------
function WA_SB_ATTACK1_OnEnter(dwElapsedTime, arg)
	local kAction = arg:Get(ACTARG_WORLDACTION)
	kAction = WorldAction(kAction)
	local kPacket = NewPacket(PT_M_C_NFY_BEGIN_WORLDACTION)
	WA_SB_MakePacket(kAction, kPacket, arg)
	local pkGround = Ground(arg:Get(ACTARG_GROUND))
	local monster = pkGround:GetBossMonster(M_SAPHIREBOSS)
	local emptyGuid = GUID()
	emptyGuid:Empty()
	pkGround:Broadcast(kPacket,monster,emptyGuid)

	DeletePacket(kPacket)

	InfoLog(9, "WA_SB_ATTACK1_OnEnter ....00")

	return true
end

function WA_SB_ATTACK1_OnTick(dwElapsedTime, arg)
	-- Saphire Boss Attack Type 1 ( 손바닥치기 )
	local iPlayingTime = 18500 --15초동안 실행된다!
	local iDamagedTime = 1900 -- 1.9초 후에 데미지 들어간다
	local iBlockTime = 4100 -- 블록이 데미지 주는 시간
	local iHandTime = 14000 -- 손이 내려와 있는 시간
	local iCurrentTime = timeGetTime() -- 현재 시간

	local kAction = arg:Get(ACTARG_WORLDACTION)
	kAction = WorldAction(kAction)
	
	local iBeginTime = kAction:GetBeginTime() -- 액션 시작한 시간
	
	local iPlayedTime = iCurrentTime - iBeginTime
	
	local iCurrentIndex = arg:GetInt(ACT_ARG_CUSTOMDATA1+1)
	
	local iBeforeIndex = iCurrentIndex - 1
	if iBeforeIndex < 0 then
		iBeforeIndex = 35
	end
	local iNextIndex = iCurrentIndex + 1
	if iNextIndex > 35 then
		iNextIndex = 0
	end

	local pkGround = Ground(arg:Get(ACTARG_GROUND))
	
	local bEnd = false
	local monster = pkGround:GetBossMonster(M_SAPHIREBOSS)

	if monster:IsNil() == true then
		bEnd = true
	elseif monster:GetAbil(AT_HP) <= 0 then
		bEnd = true
	end
	
	if iPlayedTime < iPlayingTime and bEnd == false then

		-- 손이 내려온 뒤에 데미지 들어간다.
		if iPlayedTime < iDamagedTime  then
			return true
		end
		
		-- 손이 올라가기 전까지 데미지 들어간다.
		if iPlayedTime >= (iDamagedTime + iBlockTime) then
			return true
		end

		local iIndex = 0
		local kUnitItor = NewObjectMgr_UnitIterator()
		pkGround:GetFirstUnitType(UT_PLAYER,kUnitItor)
		local kUnit = pkGround:GetNextUnitType(UT_PLAYER,kUnitItor)
		while kUnit:IsNil() == false do
		-- 존에 있는 캐릭터를 가져와서 그 캐릭터가 있는 위치를 알아낸다
			if kUnit:IsUnitType(UT_PLAYER) then
				iIndex = WA_GET_UNIT_INDEX(kUnit,36)
				
				local bDamage = false
				local bJump = kUnit:IsJump()
				-- 실행 시간이 현재 손 내려오고 블럭이 끝나기 전이다.
				if iPlayedTime >= iDamagedTime and iPlayedTime < iDamagedTime + iBlockTime then
					if ( ( iIndex == iCurrentIndex ) or ( iIndex == iBeforeIndex ) or ( iIndex == iNextIndex ) ) or ( ( iIndex % 6 ) ~= 0 ) then
						bDamage = true
					end
				end
				
				-- 데미지 받는 인덱스이고 점프중이지 않다.
				if  (bDamage == true) and (bJump == false) then
					-- 블록이 존재하는 것이고 이 유저가 맞은 적이 없거나 맞은지 4초가 지난 후라면!
					if kAction:AddAttackUnit(kUnit:GetGuid(),iCurrentTime,2000) == true then
						WA_ATTACKDAMAGE_UNIT(12,60,36,monster,kUnit,pkGround)
					end -- 활성화 블럭 안에서의 동작 끝
				end -- 블럭 인덱스가 활성화 블럭인가 검사 끝
			end
			kUnit = pkGround:GetNextUnitType(UT_PLAYER,kUnitItor)
		end --while문 끝
		DeleteObjectMgr_UnitIterator(kUnitItor)
		return true -- 아직 시간이 남았으니 끝내지 않는다.
	end

	-- Self Delete
	local mng = arg:Get(ACTARG_PUPPETMNG)
	mng = PuppetMng(mng)
	local kGuid = kAction:GetID()
	mng:EndWorldActionTick(ETICK_INTERVAL_100MS, kGuid)
	mng:DeleteWorldAction(kGuid)  
	return true 
end

--------------------------------------------------------------------------------
function WA_SB_ATTACK2_OnEnter(dwElapsedTime, arg)
	local kAction = arg:Get(ACTARG_WORLDACTION)
	kAction = WorldAction(kAction)
	local kPacket = NewPacket(PT_M_C_NFY_BEGIN_WORLDACTION)
	WA_SB_MakePacket(kAction, kPacket, arg)
	local pkGround = Ground(arg:Get(ACTARG_GROUND))
	local monster = pkGround:GetBossMonster(M_SAPHIREBOSS)
	local emptyGuid = GUID()
	emptyGuid:Empty()
	pkGround:Broadcast(kPacket,monster,emptyGuid)

	DeletePacket(kPacket)

	InfoLog(9, "WA_SB_ATTACK2_OnEnter ....00")

	return true
end

function WA_SB_ATTACK2_OnTick(dwElapsedTime, arg)
	-- Saphire Boss Attack Type 2 ( 손바닥 긁기 )
	local iHandDownTime = 1600 -- 손이 내려오는 시간
	local iBlockPassTime = 400 --블럭 하나 긁는데 0.6초 든다고 가정
	local iBlockEndTime = 5000 -- 손 움직임 끝나고 블럭 복귀되는 시간
	local iCurrentTime = timeGetTime() -- 현재 시간

	local iRotate = arg:GetInt(10000) --왼쪽이냐 오른쪽이냐. 1이면 왼쪽 0이면 오른쪽
	local iStartIndex = arg:GetInt(10001) --시작 블록
	local iBlockNum = arg:GetInt(10002) --총 긁을 블록 수

	local iPlayingTime = iHandDownTime + ( iBlockPassTime * iBlockNum )-- 총 플레이 시간

	local kAction = arg:Get(ACTARG_WORLDACTION)
	kAction = WorldAction(kAction)

	local iBeginTime = kAction:GetBeginTime() -- 액션 시작한 시간
	local iPlayedTime = iCurrentTime - iBeginTime -- 액션 진행된 시간

	local iHandIndex = iStartIndex
	local iCurrentBlock = ( ( iPlayedTime - iHandDownTime ) / iBlockPassTime )
	iCurrentBlock = math.floor(iCurrentBlock) -- 현재 진행된 블록의 수
	if iCurrentBlock > iBlockNum then
		iCurrentBlock = iBlockNum
	end

	if iRotate == 0 then -- 오른쪽으로 움직이는 것이면 인덱스를 더해준다.
		iHandIndex = iHandIndex + iCurrentBlock
		if iHandIndex >= 36 then
			iHandIndex = iHandIndex - 36
		end
	else -- 왼쪽으로 움직이는 것이면 인덱스를 빼준다.
		iHandIndex = iHandIndex - iCurrentBlock
		if iHandIndex < 0 then
			iHandIndex = 36 + iHandIndex
		end
	end
	
	local pkGround = Ground(arg:Get(ACTARG_GROUND))

	local bEnd = false
	local monster = pkGround:GetBossMonster(M_SAPHIREBOSS)

	if monster:IsNil() == true then
		bEnd = true
	elseif monster:GetAbil(AT_HP) <= 0 then
		bEnd = true
	end

	if iPlayedTime < iPlayingTime + iBlockEndTime and bEnd == false then
	-- 손이 내려오기 전까지는 데미지 안 준다.
		if iPlayedTime < iHandDownTime then
			return true
		end
		
		local iIndex = 0
		local ptPos
		local kUnitItor = NewObjectMgr_UnitIterator()
		pkGround:GetFirstUnitType(UT_PLAYER,kUnitItor)
		local kUnit = pkGround:GetNextUnitType(UT_PLAYER,kUnitItor)
		while kUnit:IsNil() == false do
		-- 존에 있는 캐릭터를 가져와서 그 캐릭터가 있는 위치를 알아낸다
			if kUnit:IsUnitType(UT_PLAYER) then
				iIndex = WA_GET_UNIT_INDEX(kUnit,36)

				local bJump = kUnit:IsJump()
				local bDamage = false
				if iRotate == 0 then -- 오른쪽으로 움직인 것이면
					if iStartIndex + iCurrentBlock >= 36 then
						if ( iIndex >= iStartIndex and iIndex <= 35 ) or ( iIndex >= 0 and iIndex <= iHandIndex ) then
							bDamage = true
						end
					else
						if iIndex >= iStartIndex and iIndex <= iHandIndex then
							bDamage = true
						end
					end
				else -- 왼쪽으로 움직인 것이면
					if iStartIndex - iCurrentBlock < 0 then
						if ( iIndex <= iStartIndex and iIndex >= 0 ) or ( iIndex <= 35 and iIndex >= iHandIndex ) then
							bDamage = true
						end
					else
						if iIndex <= iStartIndex and iIndex >= iHandIndex then
							bDamage = true
						end
					end
				end

				if iPlayedTime > iPlayingTime and iIndex == iHandIndex then
					bDamage = false
				end

				if  bDamage == true and bJump == false then
					-- 블록이 존재하는 것이고 이 유저가 맞은 적이 없거나 맞은지 2초가 지난 후라면!
					if kAction:AddAttackUnit(kUnit:GetGuid(),iCurrentTime,2000) == true then
						WA_ATTACKDAMAGE_UNIT(12,60,36,monster,kUnit,pkGround)
					end -- 활성화 블럭 안에서의 동작 끝
				end -- 블럭 인덱스가 활성화 블럭인가 검사 끝
			end -- 존 캐릭터 가져오기 끝
			kUnit = pkGround:GetNextUnitType(UT_PLAYER,kUnitItor)
		end --while문 끝
		DeleteObjectMgr_UnitIterator(kUnitItor)
		return true -- 아직 시간이 남았으니 끝내지 않는다.
	end

	-- Self Delete
	local mng = arg:Get(ACTARG_PUPPETMNG)
	mng = PuppetMng(mng)
	local kGuid = kAction:GetID()
	mng:EndWorldActionTick(ETICK_INTERVAL_100MS, kGuid)
	mng:DeleteWorldAction(kGuid)  
	return true 
end

--------------------------------------------------------------------------------
function WA_SB_ATTACK3_OnEnter(dwElapsedTime, arg)
	local kAction = arg:Get(ACTARG_WORLDACTION)
	kAction = WorldAction(kAction)
	local kPacket = NewPacket(PT_M_C_NFY_BEGIN_WORLDACTION)
	WA_SB_MakePacket(kAction, kPacket, arg)
	local pkGround = Ground(arg:Get(ACTARG_GROUND))
	local monster = pkGround:GetBossMonster(M_SAPHIREBOSS)
	local emptyGuid = GUID()
	emptyGuid:Empty()
	pkGround:Broadcast(kPacket,monster,emptyGuid)

	DeletePacket(kPacket)

	InfoLog(9, "WA_SB_ATTACK3_OnEnter ....00")

	return true
end

function WA_SB_ATTACK3_OnTick(dwElapsedTime, arg)
	-- Saphire Boss Attack Type 1 ( 물어뜯기 )
	local iBossAttackTime = 1300 --물어버리는 시간
	local iBlockRollingTime = 400 --블럭이 돌아가는 시간
	local iBlockHitTime = 2300 -- 블럭이 공격상태 유지하는 시간
	local iHeadTime = 13800 -- 머리가 유지되는 시간
	local iCurrentTime = timeGetTime() -- 현재 시간
	local iBlockNum = 5 --5개를 넘긴다
	local iTotalBlockTime = (iBlockRollingTime*(iBlockNum*2)) + iBlockHitTime -- 블럭 데미지 시간

	local iCurrentBlock = arg:GetInt(10000) --무는 블록
	local iPlayTime = iBossAttackTime + iHeadTime -- 총 플레이 시간

	local kAction = arg:Get(ACTARG_WORLDACTION)
	kAction = WorldAction(kAction)
	
	local iBeginTime = kAction:GetBeginTime() -- 액션 시작한 시간
	local iPlayedTime = iCurrentTime - iBeginTime -- 액션 진행된 시간
	
	local pkGround = Ground(arg:Get(ACTARG_GROUND))

	local bEnd = false
	local monster = pkGround:GetBossMonster(M_SAPHIREBOSS)

	if monster:IsNil() == true then
		bEnd = true
	elseif monster:GetAbil(AT_HP) <= 0 then
		bEnd = true
	end

	local iDamagedTime = iPlayedTime - iBossAttackTime

	if iPlayedTime < iPlayTime and bEnd == false then
		-- 머리 물기 전까지는 체크하지 않는다.
		if iPlayedTime < iBossAttackTime then
			return true
		end

		local iIndex = 0
		local ptPos
		local kUnitItor = NewObjectMgr_UnitIterator()
		pkGround:GetFirstUnitType(UT_PLAYER,kUnitItor)
		local kUnit = pkGround:GetNextUnitType(UT_PLAYER,kUnitItor)
		while kUnit:IsNil() == false do
		-- 존에 있는 캐릭터를 가져와서 그 캐릭터가 있는 위치를 알아낸다
			if kUnit:IsUnitType(UT_PLAYER) then
				iIndex = WA_GET_UNIT_INDEX(kUnit,36)

				local bDamage = false
				local bJump = kUnit:IsJump()

				local value = 0
				local valuetime = 0
				if iIndex == iCurrentBlock then
					if iPlayedTime < ( iBossAttackTime + iBlockRollingTime ) or iPlayedTime >= ( iTotalBlockTime + iBossAttackTime ) then
						return true
					end
					bDamage = true
				else
					if iCurrentBlock + iBlockNum >= 36 then
						if iIndex > iCurrentBlock then
							value = iIndex - iCurrentBlock - 1
						elseif iIndex >= 0 and iIndex < iBlockNum then
							value = 35 - iCurrentBlock + iIndex
						else
							value = iCurrentBlock - iIndex - 1
						end
					elseif iCurrentBlock - iBlockNum < 0 then
						if iIndex < iCurrentBlock then
							value = iCurrentBlock - iIndex - 1
						elseif iIndex <= 35 and iIndex >= 35 - iBlockNum then
							value = 35 - iIndex + iCurrentBlock
						else
							value = iIndex - iCurrentBlock - 1
						end
					else
						if iIndex > iCurrentBlock then
							value = iIndex - iCurrentBlock - 1
						else
							value = iCurrentBlock - iIndex - 1
						end
					end

					if value < iBlockNum then
						valuetime = iBlockNum - value - 1
	
						local minValue = ( value + 1 ) * iBlockRollingTime
						local maxValue = minValue+(valuetime*iBlockRollingTime*2)+iBlockHitTime
	
						if iDamagedTime >= minValue and iDamagedTime < maxValue  then
							bDamage = true 
						end
					end
				end

				if bDamage == true and bJump == false then
					-- 블록이 존재하는 것이고 이 유저가 맞은 적이 없거나 맞은지 정해진 지난 후라면!
					if kAction:AddAttackUnit(kUnit:GetGuid(),iCurrentTime,2000) == true then
						WA_ATTACKDAMAGE_UNIT(12,60,36,monster,kUnit,pkGround)
					end -- 활성화 블럭 안에서의 동작 끝
				end -- 히트 인텍스가 있나 검사 끝
			end -- 존 캐릭터 가져오기 끝
			kUnit = pkGround:GetNextUnitType(UT_PLAYER,kUnitItor)
		end --while문 끝
		DeleteObjectMgr_UnitIterator(kUnitItor)
		return true -- 아직 시간이 남았으니 끝내지 않는다.
	end
	
	local mng = arg:Get(ACTARG_PUPPETMNG)
	mng = PuppetMng(mng)
	local kGuid = kAction:GetID()
	mng:EndWorldActionTick(ETICK_INTERVAL_100MS, kGuid)
	mng:DeleteWorldAction(kGuid)  
	return true 
end

--------------------------------------------------------------------------------
function WA_SB_ATTACK4_OnEnter(dwElapsedTime, arg)
	local kAction = arg:Get(ACTARG_WORLDACTION)
	kAction = WorldAction(kAction)
	local kPacket = NewPacket(PT_M_C_NFY_BEGIN_WORLDACTION)
	WA_SB_MakePacket(kAction, kPacket, arg)
	local pkGround = Ground(arg:Get(ACTARG_GROUND))
	local monster = pkGround:GetBossMonster(M_SAPHIREBOSS)
	local emptyGuid = GUID()
	emptyGuid:Empty()
	pkGround:Broadcast(kPacket,monster,emptyGuid)

	DeletePacket(kPacket)

	InfoLog(9, "WA_SB_ATTACK4_OnEnter ....00")

	return true
end

function WA_SB_ATTACK4_OnTick(dwElapsedTime, arg)
	-- Saphire Boss Attack Type 1 ( 마법발동 )
	local iLightTime = 3300 -- 빛이 나는 시간
	local iBurstTime = 1000 -- 터지는 시간
	local iCurrentTime = timeGetTime() -- 현재 시간

	local kAction = arg:Get(ACTARG_WORLDACTION)
	kAction = WorldAction(kAction)
	
	local iBeginTime = kAction:GetBeginTime() -- 액션 시작한 시간
	local iPlayedTime = iCurrentTime - iBeginTime -- 액션 진행된 시간

	local pkGround = Ground(arg:Get(ACTARG_GROUND))
	
	local bEnd = false
	local monster = pkGround:GetBossMonster(M_SAPHIREBOSS)

	if monster:IsNil() == true then
		bEnd = true
	elseif monster:GetAbil(AT_HP) <= 0 then
		bEnd = true
	end

	if iPlayedTime < ( iBurstTime + iLightTime ) and bEnd == false then
		if iPlayedTime < iLightTime then
			return true
		end
		local iIndex = 0
		local ptPos
		local kUnitItor = NewObjectMgr_UnitIterator()
		pkGround:GetFirstUnitType(UT_PLAYER,kUnitItor)
		local kUnit = pkGround:GetNextUnitType(UT_PLAYER,kUnitItor)
		while kUnit:IsNil() == false do
		-- 존에 있는 캐릭터를 가져와서 그 캐릭터가 있는 위치를 알아낸다
			if kUnit:IsUnitType(UT_PLAYER) then
				iIndex = WA_GET_UNIT_INDEX(kUnit,36)

			-- 뽑아낸 위치가 현재 활성화된 블럭인지 검사한다. 안전구역은 0 6 12 18 24 30
				if ( iIndex % 6 ) ~= 0 then
					-- 블록이 존재하는 것이고 이 유저가 맞은 적이 없거나 맞은지 2초가 지난 후라면!
					if kAction:AddAttackUnit(kUnit:GetGuid(),iCurrentTime,2000) == true then
						WA_ATTACKDAMAGE_UNIT(36,180,36,monster,kUnit,pkGround)
					end -- 활성화 블럭 안에서의 동작 끝
				end -- 블럭 인덱스가 활성화 블럭인가 검사 끝
			end -- 존 캐릭터 가져오기 끝
			kUnit = pkGround:GetNextUnitType(UT_PLAYER,kUnitItor)
		end --while문 끝
		DeleteObjectMgr_UnitIterator(kUnitItor)
		return true -- 아직 시간이 남았으니 끝내지 않는다.
	end

	-- Self Delete
	local mng = arg:Get(ACTARG_PUPPETMNG)
	mng = PuppetMng(mng)
	local kGuid = kAction:GetID()
	mng:EndWorldActionTick(ETICK_INTERVAL_100MS, kGuid)
	mng:DeleteWorldAction(kGuid)  
	return true 
end

--------------------------------------------------------------------------------
function WA_SB_ATTACK5_OnEnter(dwElapsedTime, arg)
	local kAction = arg:Get(ACTARG_WORLDACTION)
	kAction = WorldAction(kAction)
	local kPacket = NewPacket(PT_M_C_NFY_BEGIN_WORLDACTION)
	WA_SB_MakePacket(kAction, kPacket, arg)
	local pkGround = Ground(arg:Get(ACTARG_GROUND))
	local monster = pkGround:GetBossMonster(M_SAPHIREBOSS)
	local emptyGuid = GUID()
	emptyGuid:Empty()
	pkGround:Broadcast(kPacket,monster,emptyGuid)

	DeletePacket(kPacket)

	InfoLog(9, "WA_SB_ATTACK5_OnEnter ....00")

	return true
end

function WA_SB_ATTACK5_OnTick(dwElapsedTime, arg)
	-- Saphire Boss Attack Type 1 ( 마법발동 )
	local iLightTime = 3300 -- 빛이 나는 시간
	local iBurstTime = 1000 -- 터지는 시간
	local iCurrentTime = timeGetTime() -- 현재 시간

	local kAction = arg:Get(ACTARG_WORLDACTION)
	kAction = WorldAction(kAction)
	
	local iBeginTime = kAction:GetBeginTime() -- 액션 시작한 시간
	local iPlayedTime = iCurrentTime - iBeginTime -- 액션 진행된 시간

	local pkGround = Ground(arg:Get(ACTARG_GROUND))

	local bEnd = false
	local monster = pkGround:GetBossMonster(M_SAPHIREBOSS)

	if monster:IsNil() == true then
		bEnd = true
	elseif monster:GetAbil(AT_HP) <= 0 then
		bEnd = true
	end

	
	if iPlayedTime < ( iBurstTime + iLightTime ) and bEnd == false then
		if iPlayedTime < iLightTime then
			return true
		end
		local iIndex = 0
		local ptPos
		local kUnitItor = NewObjectMgr_UnitIterator()
		pkGround:GetFirstUnitType(UT_PLAYER,kUnitItor)
		local kUnit = pkGround:GetNextUnitType(UT_PLAYER,kUnitItor)
		while kUnit:IsNil() == false do
		-- 존에 있는 캐릭터를 가져와서 그 캐릭터가 있는 위치를 알아낸다
			if kUnit:IsUnitType(UT_PLAYER) then
				iIndex = WA_GET_UNIT_INDEX(kUnit,36)

			-- 뽑아낸 위치가 현재 활성화된 블럭인지 검사한다. 미안전구역은 0 6 12 18 24 30
				if ( iIndex % 6 ) == 0 then
					-- 블록이 존재하는 것이고 이 유저가 맞은 적이 없거나 맞은지 2초가 지난 후라면!
					if kAction:AddAttackUnit(kUnit:GetGuid(),iCurrentTime,2000) == true then
						WA_ATTACKDAMAGE_UNIT(36,180,36,monster,kUnit,pkGround)
					end -- 활성화 블럭 안에서의 동작 끝
				end -- 블럭 인덱스가 활성화 블럭인가 검사 끝
			end -- 존 캐릭터 가져오기 끝
			kUnit = pkGround:GetNextUnitType(UT_PLAYER,kUnitItor)
		end --while문 끝
		DeleteObjectMgr_UnitIterator(kUnitItor)
		return true -- 아직 시간이 남았으니 끝내지 않는다.
	end

	-- Self Delete
	local mng = arg:Get(ACTARG_PUPPETMNG)
	mng = PuppetMng(mng)
	local kGuid = kAction:GetID()
	mng:EndWorldActionTick(ETICK_INTERVAL_100MS, kGuid)
	mng:DeleteWorldAction(kGuid)  
	return true 
end

---------------------------------------------------------
function WA_SB_ATTACK6_OnEnter(dwElapsedTime, arg)
	local kAction = arg:Get(ACTARG_WORLDACTION)
	kAction = WorldAction(kAction)
	local kPacket = NewPacket(PT_M_C_NFY_BEGIN_WORLDACTION)
	WA_SB_MakePacket(kAction, kPacket, arg)
	local pkGround = Ground(arg:Get(ACTARG_GROUND))
	local monster = pkGround:GetBossMonster(M_SAPHIREBOSS)
	local emptyGuid = GUID()
	emptyGuid:Empty()
	pkGround:Broadcast(kPacket,monster,emptyGuid)

	DeletePacket(kPacket)

	InfoLog(9, "WA_SB_ATTACK6_OnEnter ....00")

	return true
end

function WA_SB_ATTACK6_OnTick(dwElapsedTime, arg)
	-- Saphire Boss Attack Type 1 ( 손바닥치기 )
	local iPlayingTime = 900 
	local iDamagedTime = 400 -- 0.4초 후에 데미지 들어간다

	local iHandTime = 200 -- 손이 내려와 있는 시간
	local iCurrentTime = timeGetTime() -- 현재 시간

	local kAction = arg:Get(ACTARG_WORLDACTION)
	kAction = WorldAction(kAction)

	local iBeginTime = kAction:GetBeginTime() -- 액션 시작한 시간
	local iPlayedTime = iCurrentTime - iBeginTime

	local iCurrentIndex = arg:GetInt(ACT_ARG_CUSTOMDATA1+1)

	local pkGround = Ground(arg:Get(ACTARG_GROUND))

	local bEnd = false
	local monster = pkGround:GetBossMonster(M_SAPHIREBOSS)

	if monster:IsNil() == true then
		bEnd = true
	elseif monster:GetAbil(AT_HP) <= 0 then
		bEnd = true
	end
	
	if iPlayedTime < iPlayingTime and bEnd == false then
		local iIndex = 0
		local kUnitItor = NewObjectMgr_UnitIterator()
		pkGround:GetFirstUnitType(UT_PLAYER,kUnitItor)
		local kUnit = pkGround:GetNextUnitType(UT_PLAYER,kUnitItor)
		while kUnit:IsNil() == false do
			if kUnit:IsUnitType(UT_PLAYER) then
				iIndex = WA_GET_UNIT_INDEX(kUnit,36)
				
				local bDamage = false
				if iPlayedTime >= iDamagedTime then
					if iIndex == iCurrentIndex then
						bDamage = true
					end
				end
				
				if  bDamage == true then
					-- 블록이 존재하는 것이고 이 유저가 맞은 적이 없거나 맞은지 0.8초가 지난 후라면!
					if kAction:AddAttackUnit(kUnit:GetGuid(),iCurrentTime,800) == true then
						WA_ATTACKDAMAGE_UNIT(24,120,72,monster,kUnit,pkGround)
					end -- 활성화 블럭 안에서의 동작 끝
				end -- 블럭 인덱스가 활성화 블럭인가 검사 끝
			end
			kUnit = pkGround:GetNextUnitType(UT_PLAYER,kUnitItor)
		end --while문 끝
		DeleteObjectMgr_UnitIterator(kUnitItor)
		return true -- 아직 시간이 남았으니 끝내지 않는다.
	end

	-- Self Delete
	local mng = arg:Get(ACTARG_PUPPETMNG)
	mng = PuppetMng(mng)
	local kGuid = kAction:GetID()
	mng:EndWorldActionTick(ETICK_INTERVAL_100MS, kGuid)
	mng:DeleteWorldAction(kGuid)  
	return true 
end

---------------------------------------------------------
function WA_SB_ATTACK7_OnEnter(dwElapsedTime, arg)
	local kAction = arg:Get(ACTARG_WORLDACTION)
	kAction = WorldAction(kAction)
	local kPacket = NewPacket(PT_M_C_NFY_BEGIN_WORLDACTION)
	WA_SB_MakePacket(kAction, kPacket, arg)
	local pkGround = Ground(arg:Get(ACTARG_GROUND))
	local monster = pkGround:GetBossMonster(M_SAPHIREBOSS)
	local emptyGuid = GUID()
	emptyGuid:Empty()
	pkGround:Broadcast(kPacket,monster,emptyGuid)

	DeletePacket(kPacket)

	InfoLog(9, "WA_SB_ATTACK7_OnEnter ....00")

	return true
end

function WA_SB_ATTACK7_OnTick(dwElapsedTime, arg)
	local iPlayingTime = 7000 -- 5초동안 실행된다!
	local iDamagedTime = 1900 -- 1.9초 후에 데미지 들어간다
	local iBlockTime = 4400 -- 블록이 데미지 주는 시간
	local iHandTime = 200 -- 손이 내려와 있는 시간
	local iCurrentTime = timeGetTime() -- 현재 시간

	local kAction = arg:Get(ACTARG_WORLDACTION)
	kAction = WorldAction(kAction)
	
	local iBeginTime = kAction:GetBeginTime() -- 액션 시작한 시간
	
	local iPlayedTime = iCurrentTime - iBeginTime
	
	local iCurrentIndex = arg:GetInt(ACT_ARG_CUSTOMDATA1+1)
	
	local pkGround = Ground(arg:Get(ACTARG_GROUND))

	local bEnd = false
	local monster = pkGround:GetBossMonster(M_SAPHIREBOSS)

	if monster:IsNil() == true then
		bEnd = true
	elseif monster:GetAbil(AT_HP) <= 0 then
		bEnd = true
	end

	if iPlayedTime < iPlayingTime and bEnd == false then
		-- 손이 내려온 뒤에 데미지 들어간다.
		if iPlayedTime < iDamagedTime  then
			return true
		end

		local iIndex = 0
		local kUnitItor = NewObjectMgr_UnitIterator()
		pkGround:GetFirstUnitType(UT_PLAYER,kUnitItor)
		local kUnit = pkGround:GetNextUnitType(UT_PLAYER,kUnitItor)
		while kUnit:IsNil() == false do
		-- 존에 있는 캐릭터를 가져와서 그 캐릭터가 있는 위치를 알아낸다
			if kUnit:IsUnitType(UT_PLAYER) then
				iIndex = WA_GET_UNIT_INDEX(kUnit,36)
				
				local bDamage = false
				local bJump = kUnit:IsJump()
				-- 실행 시간이 현재 손이 올라가기 전이다!
				if iPlayedTime >= iDamagedTime then
					if ( ( iIndex == iCurrentIndex ) or ( ( iIndex % 6 ) == 0 ) )then
						bDamage = true
					end
				end
				
				-- 데미지 받는 인덱스이고 점프중이지 않다.
				if  (bDamage == true) and (bJump == false) then
					-- 블록이 존재하는 것이고 이 유저가 맞은 적이 없거나 맞은지 4초가 지난 후라면!
					if kAction:AddAttackUnit(kUnit:GetGuid(),iCurrentTime,2000) == true then
						WA_ATTACKDAMAGE_UNIT(12,60,36,monster,kUnit,pkGround)
					end -- 활성화 블럭 안에서의 동작 끝
				end -- 블럭 인덱스가 활성화 블럭인가 검사 끝
			end
			kUnit = pkGround:GetNextUnitType(UT_PLAYER,kUnitItor)
		end --while문 끝
		DeleteObjectMgr_UnitIterator(kUnitItor)
		return true -- 아직 시간이 남았으니 끝내지 않는다.
	end

	-- Self Delete
	local mng = arg:Get(ACTARG_PUPPETMNG)
	mng = PuppetMng(mng)
	local kGuid = kAction:GetID()
	mng:EndWorldActionTick(ETICK_INTERVAL_100MS, kGuid)
	mng:DeleteWorldAction(kGuid)  
	return true 
end

---------------------------------------------------------
function WA_SB_ATTACK8_OnEnter(dwElapsedTime, arg)
	local kAction = arg:Get(ACTARG_WORLDACTION)
	kAction = WorldAction(kAction)
	local kPacket = NewPacket(PT_M_C_NFY_BEGIN_WORLDACTION)
	WA_SB_MakePacket(kAction, kPacket, arg)
	local pkGround = Ground(arg:Get(ACTARG_GROUND))
	local monster = pkGround:GetBossMonster(M_SAPHIREBOSS)
	local emptyGuid = GUID()
	emptyGuid:Empty()
	pkGround:Broadcast(kPacket,monster,emptyGuid)

	DeletePacket(kPacket)

	InfoLog(9, "WA_SB_ATTACK8_OnEnter ....00")

	return true
end

function WA_SB_ATTACK8_OnTick(dwElapsedTime, arg)
	local iHandTime = 2400
	local iBlockPassTime = 400
	local iBlockNum = 9
	local iPlayingTime = iBlockNum * iBlockPassTime + iHandTime

	local kAction = arg:Get(ACTARG_WORLDACTION)
	kAction = WorldAction(kAction)

	local iBeginTime = kAction:GetBeginTime() -- 액션 시작한 시간
	local iCurrentTime = timeGetTime() -- 현재 시간
	local iPlayedTime = iCurrentTime - iBeginTime

	local iTargetBlock = arg:GetInt(ACT_ARG_CUSTOMDATA1)
	local iRollingBlockNum = ( ( iPlayedTime - iHandTime ) / iBlockPassTime )
	iRollingBlockNum = math.floor(iRollingBlockNum) -- 현재 진행된 블록의 수

	local pkGround = Ground(arg:Get(ACTARG_GROUND))

	local bEnd = false
	local monster = pkGround:GetBossMonster(M_SAPHIREBOSS)

	if monster:IsNil() == true then
		bEnd = true
	elseif monster:GetAbil(AT_HP) <= 0 then
		bEnd = true
	end
	
	if iPlayedTime < iPlayingTime and bEnd == false then
		if iPlayedTime < iHandTime then
			return true
		end
		
		if iPlayedTime >= iPlayingTime - iHandTime then
			return true
		end

		local iIndex = 0
		local kUnitItor = NewObjectMgr_UnitIterator()
		pkGround:GetFirstUnitType(UT_PLAYER,kUnitItor)
		local kUnit = pkGround:GetNextUnitType(UT_PLAYER,kUnitItor)
		while kUnit:IsNil() == false do
		-- 존에 있는 캐릭터를 가져와서 그 캐릭터가 있는 위치를 알아낸다
			if kUnit:IsUnitType(UT_PLAYER) then
				iIndex = WA_GET_UNIT_INDEX(kUnit,36)
				
				local bDamage = false
				local bJump = kUnit:IsJump()

				local blocknum = iBlockNum
				while blocknum > ( iBlockNum - iRollingBlockNum + 1 ) do
					-- 일단 왼쪽부터 검사. 왼쪽은 상위니까 +이다
					iBlockIndex = iTargetBlock + blocknum
					if iBlockIndex >= 36 then
						iBlockIndex = blocknum - ( 36 - iTargetBlock )
					end
					
					if iIndex == iBlockIndex then
						bDamage = true
						break
					end
					
					-- 오른쪽 검사하자. 오른쪽은 하위니까 -이다.
					local iBlockIndex = iTargetBlock - blocknum
					if iBlockIndex < 0 then
						iBlockIndex = 36 + ( iTargetBlock - blocknum )
					end

					if iIndex == iBlockIndex then
						bDamage = true
						break
					end
					
					blocknum = blocknum - 1
				end

				-- 데미지 받는 인덱스이고 점프중이지 않다.
				if  (bDamage == true) and (bJump == false) then
					-- 블록이 존재하는 것이고 이 유저가 맞은 적이 없거나 맞은지 4초가 지난 후라면!
					if kAction:AddAttackUnit(kUnit:GetGuid(),iCurrentTime,2000) == true then
						WA_ATTACKDAMAGE_UNIT(12,60,36,monster,kUnit,pkGround)
					end -- 활성화 블럭 안에서의 동작 끝
				end -- 블럭 인덱스가 활성화 블럭인가 검사 끝
			end
			kUnit = pkGround:GetNextUnitType(UT_PLAYER,kUnitItor)
		end --while문 끝
		DeleteObjectMgr_UnitIterator(kUnitItor)
		return true
	end

	-- Self Delete
	local mng = arg:Get(ACTARG_PUPPETMNG)
	mng = PuppetMng(mng)
	local kGuid = kAction:GetID()
	mng:EndWorldActionTick(ETICK_INTERVAL_100MS, kGuid)
	mng:DeleteWorldAction(kGuid)  
	return true 
end

---------------------------------------------------------
function WA_SB_ATTACK9_OnEnter(dwElapsedTime, arg)
	local kAction = arg:Get(ACTARG_WORLDACTION)
	kAction = WorldAction(kAction)
	local kPacket = NewPacket(PT_M_C_NFY_BEGIN_WORLDACTION)
	WA_SB_MakePacket(kAction, kPacket, arg)
	local pkGround = Ground(arg:Get(ACTARG_GROUND))
	local monster = pkGround:GetBossMonster(M_SAPHIREBOSS)
	local emptyGuid = GUID()
	emptyGuid:Empty()
	pkGround:Broadcast(kPacket,monster,emptyGuid)

	DeletePacket(kPacket)

	InfoLog(9, "WA_SB_ATTACK9_OnEnter ....00")

	return true
end

function WA_SB_ATTACK9_OnTick(dwElapsedTime, arg)
	local iHandDownTime = 1600
	local iBlockPassTime = 400
	local iBlockEndTime = 750
	local iCurrentTime = timeGetTime() -- 현재 시간

	local kAction = arg:Get(ACTARG_WORLDACTION)
	kAction = WorldAction(kAction)

	local iBlockNum = arg:GetInt(10002)

	local iFirstPlayingTime = iHandDownTime + ( iBlockPassTime * iBlockNum ) + iBlockEndTime

	local pkGround = Ground(arg:Get(ACTARG_GROUND))

	local monster = pkGround:GetBossMonster(M_SAPHIREBOSS)

	if monster:IsNil() == true then
		return false
	elseif monster:GetAbil(AT_HP) <= 0 then
		return false
	end

	local bFirstAttack = WA_SB_ATTACK9_1st_OnTick(dwElapsedTime,arg)
	local bSecondAttack = false

	if bFirstAttack == false then
		bSecondAttack = WA_SB_ATTACK9_2nd_OnTick(dwElapsedTime,arg,iFirstPlayingTime)
	end

	if bFirstAttack == false and bSecondAttack == false then
		-- Self Delete
		local mng = arg:Get(ACTARG_PUPPETMNG)
		mng = PuppetMng(mng)
		local kGuid = kAction:GetID()
		mng:EndWorldActionTick(ETICK_INTERVAL_100MS, kGuid)
		mng:DeleteWorldAction(kGuid)  
	end

	return true 
end

function WA_SB_ATTACK9_1st_OnTick(dwElapsedTime, arg)
	-- Saphire Boss Attack Type 2 ( 손바닥 긁기 )
	local iHandDownTime = 1600 -- 손이 내려오는 시간
	local iBlockPassTime = 400 --블럭 하나 긁는데 0.6초 든다고 가정
	local iBlockEndTime = 800 -- 손 움직임 끝나고 블럭 복귀되는 시간
	local iCurrentTime = timeGetTime() -- 현재 시간

	local iRotate = arg:GetInt(10000) --왼쪽이냐 오른쪽이냐. 1이면 왼쪽 0이면 오른쪽
	local iStartIndex = arg:GetInt(10001) --시작 블록
	local iBlockNum = arg:GetInt(10002) --총 긁을 블록 수

	local iPlayingTime = iHandDownTime + ( iBlockPassTime * iBlockNum )-- 총 플레이 시간

	local kAction = arg:Get(ACTARG_WORLDACTION)
	kAction = WorldAction(kAction)

	local iBeginTime = kAction:GetBeginTime() -- 액션 시작한 시간
	local iPlayedTime = iCurrentTime - iBeginTime -- 액션 진행된 시간

	local iHandIndex = iStartIndex
	local iCurrentBlock = ( ( iPlayedTime - iHandDownTime ) / iBlockPassTime )
	iCurrentBlock = math.floor(iCurrentBlock) -- 현재 진행된 블록의 수
	if iCurrentBlock > iBlockNum then
		iCurrentBlock = iBlockNum
	end

	if iRotate == 0 then -- 오른쪽으로 움직이는 것이면 인덱스를 더해준다.
		iHandIndex = iHandIndex + iCurrentBlock
		if iHandIndex >= 36 then
			iHandIndex = iHandIndex - 36
		end
	else -- 왼쪽으로 움직이는 것이면 인덱스를 빼준다.
		iHandIndex = iHandIndex - iCurrentBlock
		if iHandIndex < 0 then
			iHandIndex = 36 + iHandIndex
		end
	end
	
	local pkGround = Ground(arg:Get(ACTARG_GROUND))

	local monster = pkGround:GetBossMonster(M_SAPHIREBOSS)

	if monster:IsNil() == true then
		return false
	elseif monster:GetAbil(AT_HP) <= 0 then
		return false
	end

	if iPlayedTime < iPlayingTime + iBlockEndTime then
	-- 손이 내려오기 전까지는 데미지 안 준다.
		if iPlayedTime < iHandDownTime then
			return true
		end
		
		local iIndex = 0
		local ptPos
		local kUnitItor = NewObjectMgr_UnitIterator()
		pkGround:GetFirstUnitType(UT_PLAYER,kUnitItor)
		local kUnit = pkGround:GetNextUnitType(UT_PLAYER,kUnitItor)
		while kUnit:IsNil() == false do
		-- 존에 있는 캐릭터를 가져와서 그 캐릭터가 있는 위치를 알아낸다
			if kUnit:IsUnitType(UT_PLAYER) then
				iIndex = WA_GET_UNIT_INDEX(kUnit,36)

				local bJump = kUnit:IsJump()
				local bDamage = false
				if iRotate == 0 then -- 오른쪽으로 움직인 것이면
					if iStartIndex + iCurrentBlock >= 36 then
						if ( iIndex >= iStartIndex and iIndex <= 35 ) or ( iIndex >= 0 and iIndex <= iHandIndex ) then
							bDamage = true
						end
					else
						if iIndex >= iStartIndex and iIndex <= iHandIndex then
							bDamage = true
						end
					end
				else -- 왼쪽으로 움직인 것이면
					if iStartIndex - iCurrentBlock < 0 then
						if ( iIndex <= iStartIndex and iIndex >= 0 ) or ( iIndex <= 35 and iIndex >= iHandIndex ) then
							bDamage = true
						end
					else
						if iIndex <= iStartIndex and iIndex >= iHandIndex then
							bDamage = true
						end
					end
				end

				if iPlayedTime > iPlayingTime and iIndex == iHandIndex then
					bDamage = false
				end

				if  bDamage == true and bJump == false then
					-- 블록이 존재하는 것이고 이 유저가 맞은 적이 없거나 맞은지 2초가 지난 후라면!
					if kAction:AddAttackUnit(kUnit:GetGuid(),iCurrentTime,2000) == true then
						WA_ATTACKDAMAGE_UNIT(12,60,36,monster,kUnit,pkGround)
					end -- 활성화 블럭 안에서의 동작 끝
				end -- 블럭 인덱스가 활성화 블럭인가 검사 끝
			end -- 존 캐릭터 가져오기 끝
			kUnit = pkGround:GetNextUnitType(UT_PLAYER,kUnitItor)
		end --while문 끝
		DeleteObjectMgr_UnitIterator(kUnitItor)
		return true -- 아직 시간이 남았으니 끝내지 않는다.
	end

	return false
end

function WA_SB_ATTACK9_2nd_OnTick(dwElapsedTime, arg,iTime)
	local iPlayingTime = 16500 --15초동안 실행된다!
	local iBlockTime = 5700 -- 블록이 데미지 주는 시간
	local iHandTime = 14600 -- 손이 내려와 있는 시간
	local iCurrentTime = timeGetTime() -- 현재 시간

	local kAction = arg:Get(ACTARG_WORLDACTION)
	kAction = WorldAction(kAction)
	
	local iBeginTime = kAction:GetBeginTime() + iTime -- 액션 시작한 시간
	local iPlayedTime = iCurrentTime - iBeginTime

	local iRotate = arg:GetInt(10000) --왼쪽이냐 오른쪽이냐. 1이면 왼쪽 0이면 오른쪽
	local iStartIndex = arg:GetInt(10001) --시작 블록
	local iBlockNum = arg:GetInt(10002) --총 긁을 블록 수
	
	local iHandIndex = iStartIndex

	if iRotate == 0 then -- 오른쪽으로 움직이는 것이면 인덱스를 더해준다.
		iHandIndex = iHandIndex + iBlockNum
		if iHandIndex >= 36 then
			iHandIndex = iHandIndex - 36
		end
	else -- 왼쪽으로 움직이는 것이면 인덱스를 빼준다.
		iHandIndex = iHandIndex - iBlockNum
		if iHandIndex < 0 then
			iHandIndex = 36 + iHandIndex
		end
	end
	
	local iCurrentIndex = iHandIndex

	local iBeforeIndex = iCurrentIndex - 1
	if iBeforeIndex < 0 then
		iBeforeIndex = 35
	end
	local iNextIndex = iCurrentIndex + 1
	if iNextIndex > 35 then
		iNextIndex = 0
	end

	local pkGround = Ground(arg:Get(ACTARG_GROUND))

	local monster = pkGround:GetBossMonster(M_SAPHIREBOSS)

	if monster:IsNil() == true then
		return false
	elseif monster:GetAbil(AT_HP) <= 0 then
		return false
	end

	if iPlayedTime < iPlayingTime then
		if iPlayedTime >= iBlockTime then
			return true
		end

		local iIndex = 0
		local kUnitItor = NewObjectMgr_UnitIterator()
		pkGround:GetFirstUnitType(UT_PLAYER,kUnitItor)
		local kUnit = pkGround:GetNextUnitType(UT_PLAYER,kUnitItor)
		while kUnit:IsNil() == false do
		-- 존에 있는 캐릭터를 가져와서 그 캐릭터가 있는 위치를 알아낸다
			if kUnit:IsUnitType(UT_PLAYER) then
				iIndex = WA_GET_UNIT_INDEX(kUnit,36)
				
				local bDamage = true
				local bJump = kUnit:IsJump()

				if iRotate == 0 then -- 오른쪽으로 움직인 것이면
					if iStartIndex + iCurrentIndex >= 36 then
						if ( iIndex >= iStartIndex and iIndex <= 35 ) or ( iIndex >= 0 and iIndex <= iHandIndex ) then
							bDamage = false
						end
					else
						if iIndex >= iStartIndex and iIndex <= iHandIndex then
							bDamage = false
						end
					end
				else -- 왼쪽으로 움직인 것이면
					if iStartIndex - iCurrentIndex < 0 then
						if ( iIndex <= iStartIndex and iIndex >= 0 ) or ( iIndex <= 35 and iIndex >= iHandIndex ) then
							bDamage = false
						end
					else
						if iIndex <= iStartIndex and iIndex >= iHandIndex then
							bDamage = false
						end
					end
				end

				if ( ( iIndex == iCurrentIndex ) or ( iIndex == iBeforeIndex ) or ( iIndex == iNextIndex ) ) then
					bDamage = true
				end
				
				if ( iIndex % 6 ) == 0 then
					bDamage = false
				end
				
				-- 데미지 받는 인덱스이고 점프중이지 않다.
				if  (bDamage == true) and (bJump == false) then
					if kAction:AddAttackUnit(kUnit:GetGuid(),iCurrentTime,2000) == true then
						WA_ATTACKDAMAGE_UNIT(12,60,36,monster,kUnit,pkGround)
					end -- 활성화 블럭 안에서의 동작 끝
				end -- 블럭 인덱스가 활성화 블럭인가 검사 끝
			end
			kUnit = pkGround:GetNextUnitType(UT_PLAYER,kUnitItor)
		end --while문 끝
		DeleteObjectMgr_UnitIterator(kUnitItor)
		return true -- 아직 시간이 남았으니 끝내지 않는다.
	end

	return false
end

-- 몬스터 소환. 이 보스 전용이다!
function WA_SB_SUMMON_MONSTER_200008(ground,iBlockIndex)
	if ground:IsNil() == true then
		return false
	end

	local fRadius = 415.0
	fRadian = math.pi / 180 

	local fX = math.cos( 10 * iBlockIndex * fRadian ) * fRadius
	local fY = math.sin( 10 * iBlockIndex * fRadian ) * fRadius
	local fZ = 5.0

	local ptPoint = Point3(460,-173,fZ)
	ground:MonsterGenerate(29,ptPoint,200)
	
	InfoLog(4, "WA_SB_SUMMON_MONSTER_200008 ...["..iBlockIndex.."]["..fX.."]["..fY.."]["..fZ.."]")
end

---------------------------------------------------------
function WA_SB_ATTACK10_OnEnter(dwElapsedTime, arg)
	local kAction = arg:Get(ACTARG_WORLDACTION)
	kAction = WorldAction(kAction)
	local kPacket = NewPacket(PT_M_C_NFY_BEGIN_WORLDACTION)
	WA_SB_MakePacket(kAction, kPacket, arg)
	local pkGround = Ground(arg:Get(ACTARG_GROUND))
	local monster = pkGround:GetBossMonster(M_SAPHIREBOSS)
	local emptyGuid = GUID()
	emptyGuid:Empty()
	pkGround:Broadcast(kPacket,monster,emptyGuid)

	DeletePacket(kPacket)

	InfoLog(9, "WA_SB_ATTACK10_OnEnter ....00")

	return true
end

function WA_SB_ATTACK10_OnTick(dwElapsedTime, arg)
	local iDamagedTime = 800 -- 낙석이 데미지 주는 시간
	local iPlayingTime = 6500

	local pkGround = Ground(arg:Get(ACTARG_GROUND))

	local bEnd = false
	local monster = pkGround:GetBossMonster(M_SAPHIREBOSS)

	if monster:IsNil() == true then
		bEnd = true
	elseif monster:GetAbil(AT_HP) <= 0 then
		bEnd = true
	end
	local iCurrentTime = timeGetTime() -- 현재 시간

	local kAction = arg:Get(ACTARG_WORLDACTION)
	kAction = WorldAction(kAction)
	
	local iBeginTime = kAction:GetBeginTime() -- 액션 시작한 시간
	local iPlayedTime = iCurrentTime - iBeginTime

	local iBlockNum = arg:GetInt(10000)

	if iBlockNum > 0 and iPlayedTime < iPlayingTime and bEnd == false then
		for i = 1 , iBlockNum do
			local iBlockIndex = arg:GetInt(ACT_ARG_CUSTOMDATA1 + ( (i-1) * 3 ) + 1)
			iBlockTime = arg:GetInt(ACT_ARG_CUSTOMDATA1 + ( (i-1) * 3 ) + 2)
			iBlockType = arg:GetInt(ACT_ARG_CUSTOMDATA1 + ( (i-1) * 3 ) + 3)

			if iPlayedTime >= iBlockTime + 1000 and iPlayedTime < iBlockTime + iDamagedTime + 1000 then
				local iIndex = 0
				local kUnitItor = NewObjectMgr_UnitIterator()
				pkGround:GetFirstUnitType(UT_PLAYER,kUnitItor)
				local kUnit = pkGround:GetNextUnitType(UT_PLAYER,kUnitItor)
				while kUnit:IsNil() == false do
					if kUnit:IsUnitType(UT_PLAYER) then
						iIndex = WA_GET_UNIT_INDEX(kUnit,36)

						if iIndex == iBlockIndex then
							if kAction:AddAttackUnit(kUnit:GetGuid(),iCurrentTime,2000) == true then
								WA_ATTACKDAMAGE_UNIT(12,60,36,monster,kUnit,pkGround)
							end
						end
					end
					kUnit = pkGround:GetNextUnitType(UT_PLAYER,kUnitItor)
				end --while문 끝
				DeleteObjectMgr_UnitIterator(kUnitItor)
			end
		end
		return true
	end

	-- Self Delete
	local mng = arg:Get(ACTARG_PUPPETMNG)
	mng = PuppetMng(mng)
	local kGuid = kAction:GetID()
	mng:EndWorldActionTick(ETICK_INTERVAL_100MS, kGuid)
	mng:DeleteWorldAction(kGuid)  
	return true 
end
