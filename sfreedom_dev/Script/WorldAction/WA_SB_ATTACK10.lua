-- 크리스탈 낙하

CrystalPos = {}
CrystalTime = {}
CrystalType = {}
CrystalExlpo = {}
CrystalTypeString = {}
CrystalTypeString[0] = "e_ef_trap_broken"
CrystalTypeString[1] = "e_ef_trap_explosion"

i10AniStartTime = -1
iNowCrystalIndex = 0

function WA_SB_ATTACK10_OnReceivePacket(wa_obj,Packet)

	wa_obj:SetParamInt(0,Packet:PopChar())	-- 	갯수

	iTotalCrystalNum = wa_obj:GetParamInt(0)
	iNowCrystalIndex = 0

--	ODS("크리스탈 갯수 : " .. iTotalCrystalNum .. "\n")
	
	for i = 0, iTotalCrystalNum-1 do		
		CrystalPos[i] = Packet:PopChar()	--	위치*갯수
		CrystalTime[i] = Packet:PopInt()	--	시간*갯수
		CrystalType[i] = Packet:PopChar()	--	종류*갯수
		CrystalExlpo[i] = 0			-- 	초기화
	end

	for i = 0, iTotalCrystalNum-1 do		
--		ODS("위치 : " .. CrystalPos[i] .. " 시간 : " .. CrystalTime[i] .. " 종류 : " .. CrystalType[i] .. "\n" )
	end

	if( CheckNil(g_world == nil) ) then return false end
	if( CheckNil(g_world:IsNil()) ) then return false end
	BossActor = g_world:FindActor(g_world:GetBossGUID())	--보스 액터 갖고오기
	return true
end

function WA_SB_ATTACK10_OnEnter(wa_obj,ElapsedTimeAtStart)
	if BossActor:IsNil() then
		return false
	end
	i10AniStartTime = ElapsedTimeAtStart
end

function WA_SB_ATTACK10_OnUpdate(wa_obj,ElapsedTime)
	if BossActor == nil or bSB_Boss_is_Die == true then
		return false
	end
  
	for i = 0, iTotalCrystalNum-1 do		
		if CrystalExlpo[i] == 0 and ElapsedTime - i10AniStartTime >= CrystalTime[i] then
			BossActor:AttachParticleToPoint(42345+iNowCrystalIndex, Point3(PuppetPosX[CrystalPos[i]+1], PuppetPosY[CrystalPos[i]+1], 5), CrystalTypeString[CrystalType[i]] )
--			ODS("크리스탈 터짐 : " .. CrystalPos[i] .. " iNowCrystalIndex : " .. iNowCrystalIndex .. "\n")
			iNowCrystalIndex = iNowCrystalIndex + 1
--			i10AniStartTime = ElapsedTime
			CrystalExlpo[i] = 1	-- 폭팔 했다고 체크함
		end
	end

	if iNowCrystalIndex >= iTotalCrystalNum then
--		ODS("return false-> iNowCrystalIndex : " .. iNowCrystalIndex .. " iTotalCrystalNum : " .. iTotalCrystalNum .."\n")
		return false
	end
	
	return true
end

function WA_SB_ATTACK10_OnLeave(wa_obj)
  iNowCrystalIndex = 0
	ODS("WA_SB_ATTACK10_OnLeave\n")
	return true
end
