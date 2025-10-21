
function Net_ReqPetState(iState)
	-- PT_C_M_REQ_PETSUMMON_STATE (12702)
	--
	-- State
	-- NONE = 0
	-- SUMMON = 1
	-- RIDING = 2
	-- INROOM = 3

	packet = NewPacket(12702)

	packet:PushInt(iState)

	Net_Send(packet)
	DeletePacket(packet)	
end

function Net_RecvPetState(packet)
	-- PT_M_C_NFY_PETSUMMON_STATE (112703)
	local petGuid = packet:PopGuid()
	local pilot = g_pilotMan:FindPilot(petGuid)
	if pilot:IsNil() == true then
		return true
	end

	local iState = packet:PopInt()
    local iGrade = packet:PopInt()
	local actor = pilot:GetActor()
   
	-- Pet 등급이 기존과 다르다면, Model을 Update한다. 
	if iGrade <= 3 and
		pilot:GetAbil(AT_GRADE) ~= iGrade then
		pilot:SetAbil(AT_GRADE, iGrade)
    	local actorID = actor:GetID()
	    nextID = string.sub(actorID, 1, string.len(actorID)-1) .. abilValue
		ODS(nextID .. "\n")
	    actor:UpdateModel(nextID)

		if actor:IsMyPet() == true then
			GetPetInfo()
		end
	end

	-- 이전 State와 같으면 더 이상 처리하지 않는다.
	local iCurrentPetState = pilot:GetAbil(AT_PETSTATE)	
	if iState == iCurrentPetState then
		return true
	end

	ODS("CurrentState : " .. iCurrentPetState .. "\tPet State : " .. iState .."\n")
	-- 새로운 State를 설정한다.
	pilot:SetAbil(AT_PETSTATE, iState)
	local kPet = pilot:GetActor()
	local masterGuid = actor:GetPetMaster()
	local masterPilot = g_pilotMan:FindPilot(masterGuid)

	if iCurrentPetState == PET_RIDING then
		-- 펫을 내리거나 탄다.
		if masterPilot:IsNil() == false then
			masterActor = masterPilot:GetActor()
			RideMyPet(false, masterActor, kPet)
		end
	elseif iState == PET_RIDING then
		if masterPilot:IsNil() == false then
			masterActor = masterPilot:GetActor()
			RideMyPet(true, masterActor, kPet)
		end
	end

	if iState == PET_NONE then	-- 소환 해제
		if pilot:GetActor():IsMyPet() == true then
			CloseUI("FRM_PetInfo")
		end
		if( CheckNil(g_world == nil) ) then return false end
		if( CheckNil(g_world:IsNil()) ) then return false end
		pilot:GetActor():ReserveTransitAction("a_teleport")
		g_world:FreezeObject(petGuid)
	elseif iState == PET_SUMMON then -- 소환
		if pilot:GetActor():IsMyPet() == true then
			ActivateUI("FRM_PetInfo")
			GetPetInfo()
		end
		if( CheckNil(g_world == nil) ) then return false end
		if( CheckNil(g_world:IsNil()) ) then return false end
		g_world:UnfreezeObject(petGuid)
		pilot:GetActor():ReserveTransitAction("a_teleport")		
	end
end

function Net_RecvPetName(packet)
	-- PT_M_C_NFY_PETNAME_CHANGE (112705)
	local guidPilot = packet:PopGuid()
	--guidPilot:ODS()
	local pilot = g_pilotMan:FindPilot(guidPilot)
	if pilot:IsNil() == true then
		return true
	end

	local petName = packet:PopWString()
	pilot:ChangePetName(petName:GetStr())
	local TempText1 = GetAppendTextW(GetTextW(31),petName);
	local TempText1 = GetAppendTextW(TempText1,GetTextW(32));
	AddWarnDataStr(TempText1, 0)
	--GetPetInfo()
end

function Net_NfyPetDie(packet)
	local petGuid = packet:PopGuid()

	local petPilot = g_pilotMan:FindPilot(petGuid)
	if petPilot:IsNil() == true then
		return true
	end 

	if petPilot:GetActor():IsMyPet() == true then
		string = GetTextW(33)
		DisplayPetText(string , 3000)
	end

	return true
end

function Net_ReqPetRiding(bRiding)
	-- PT_C_M_REQ_PET_RIDING (12706)
	local packet = NewPacket(12706)	
	packet:PushBool(bRiding)

	Net_Send(packet)
	DeletePacket(packet)
end

function Net_ReqPetName(petName)
	-- PT_C_M_REQ_PETNAME_CHANGE (12704)
	local packet = NewPacket(12704)	
	packet:PushWString(petName)

	Net_Send(packet)
	DeletePacket(packet)	
end


function Net_NfyAddPet(packet)
	if( CheckNil(g_world == nil) ) then return false end
	if( CheckNil(g_world:IsNil()) ) then return false end
	local cnt = packet:PopInt()

	--MessageBox(cnt, "NfyAddOtherPet")

	local idx = 0
	while idx < cnt do
		Net_AddOtherPet(g_world, packet)
		idx = idx + 1
	end

end

function RideMyPet(bRide, kActor, kPet)
	if kActor:IsNil() or 
		kPet:IsNil() then
		return true
	end
	if( CheckNil(g_world == nil) ) then return false end
	if( CheckNil(g_world:IsNil()) ) then return false end

	kPet:AttachParticle(289, "char_root", "e_special_transform")
	kActor:RideMyPet(bRide)
	kActor:ClearActionState()

	if bRide == true then
		kActor:HideParts(6, true)
		kActor:GetAction():SetParam(19, "RideMyPet")
		kActor:ReserveTransitAction("a_ride_idle")
		kPet:ReserveTransitAction("a_idle")
		g_world:SetCameraMode(1, kPet)
	else
		kActor:HideParts(6, false)
		kPet:GetAction():SetParam(19, "RideMyPet")
		kActor:ReserveTransitAction("a_idle")
		kPet:ReserveTransitAction("a_idle")
		g_world:SetCameraMode(1, kActor)
	end
end

function Net_AddOtherPet(world, packet)
	local petGuid = packet:PopGuid()
	local petNo = packet:PopInt()
	local petGrade = packet:PopChar()
	local petLv = packet:PopInt()
	--petState = packet:PopShort()
--	MessageBox("Net_AddOtherPet : ["..petNo.."")

	local PetResNo = GetPetResNoFromLevel(petNo, petLv)
	if PetResNo == 0 then
		return false
	end

	local xmlPath = GetActorPath(PetResNo)
	
	if xmlPath == "" then
		return true
	end

	local pilot = g_pilotMan:NewPilot(petGuid, xmlPath, "pet")
	if pilot:IsNil() == true then
		return true
	end

	-- 패킷으로 파일럿을 초기화
	pilot:ReadPacket_Pet(petGuid, packet,petNo,false,petGrade)	-- OtherPet 

	-- 스폰 위치 결정
	local spawnLoc = pilot:GetPos()
	world:AddActor(petGuid, pilot:GetActor(), spawnLoc, 1) -- 펫도 그룹1번을 쓴다.

	local masterGuid = pilot:GetActor():GetPetMaster()
	if masterGuid:IsNil() == false then
		masterPilot = g_pilotMan:FindPilot(masterGuid);
		if masterPilot:IsNil() == false then
			-- 펫 주인에게 자기 펫임을 알려준다.
			masterPilot:SetMyPet(petGuid)
		else
			--MessageBox("펫이 주인을 잃어버렸어요........")
		end
	end
	
	-- Action State 초기화
	pilot:GetActor():ClearActionState()
	local petState = pilot:GetAbil(AT_PETSTATE)

	-- 소환 상태가 아니면, 오브젝트를 냉동시킨다.
	if petState == PET_SUMMON then -- 소환중
		pilot:GetActor():ReserveTransitAction("a_run_right")
		pilot:GetActor():ReserveTransitAction("a_idle")
	elseif petState == PET_RIDING then -- 탑승중
		
	else
		world:FreezeObject(petGuid)
	end
end

function Net_AddMyPet(packet)
	if( CheckNil(g_world == nil) ) then return false end
	if( CheckNil(g_world:IsNil()) ) then return false end
	local cntPet = packet:PopInt()
	--MessageBox(cntPet, "Pet Count")
	if cntPet ~= 1 then
		--MessageBox("저에게 펫을 왜 이리도 많이 주시나이까!!!!\n", "OTL")
	end
	return Net_AddPet(g_world, packet)
end


function Net_AddPet(world, packet)
	local petLv = packet:PopInt()
	local petGuid = packet:PopGuid()
	local petNo = packet:PopInt()
	local petGrade = packet:PopChar()
	--petState = packet:PopShort()
	
--	MessageBox("Net_AddPet :"..petNo,"")
	
	local PetResNo = GetPetResNoFromLevel(petNo, petLv)
	if PetResNo == 0 then
		return false
	end

	local pilot = g_pilotMan:NewPilot(petGuid, GetActorPath(PetResNo), "pet")
	
	if pilot:IsNil() == true then
		pilot = g_pilotMan:FindPilot(petGuid)
		pilot:ReadPacket_Pet(petGuid, packet,petNo,true,petGrade)	-- My Pet
		return true
	end

	-- 패킷으로 파일럿을 초기화
	pilot:ReadPacket_Pet(petGuid, packet,petNo,true,petGrade)	-- My Pet
	
	-- 스폰 위치 결정
	local spawnLoc = pilot:GetPos()
	-- 좌표가 없다면, 주인의 위치에 놓는다.
	if spawnLoc:GetX() == 0 and spawnLoc:GetY() == 0 and spawnLoc:GetZ() == 0 then
		spawnLoc = playerPilot:GetPos()
	end
	
	world:AddActor(petGuid, pilot:GetActor(), spawnLoc, 4) -- 펫은 그룹 4번을 쓴다.
	local petActor = pilot:GetActor()
	petActor:ClearActionState()

	playerPilot = g_pilotMan:FindPilot(g_playerInfo.guidPilot);
	if playerPilot:IsNil() == false then
		-- 플레이어에게 자기 펫임을 알려준다.
		playerPilot:SetMyPet(petGuid)
	end

	local petState = pilot:GetAbil(AT_PETSTATE)

	-- 소환 상태가 아니면, 오브젝트를 냉동시킨다.
	if petState == PET_NONE then
		return true
	elseif petState == PET_SUMMON then -- 소환중
		ActivateUI("FRM_PetInfo")
		GetPetInfo()
		petActor:ReserveTransitAction("a_run_right")
		petActor:ReserveTransitAction("a_idle")
	elseif petState == PET_RIDING then -- 탑승중
		local playerActor = playerPilot:GetActor()
		RideMyPet(true, playerActor, petActor) 
	else
		world:FreezeObject(petGuid)
	end
end

function Net_RemovePet(packet)
	if( CheckNil(g_world == nil) ) then return false end
	if( CheckNil(g_world:IsNil()) ) then return false end
	local cntChar = packet:PopInt()
	i = 0
	--MessageBox(cntChar, "Net_RemovePet")
	while i < cntChar do
		guidPet = packet:PopGuid()	-- 지워질 펫 GUID
		g_world:RemoveActorOnNextUpdate(guidPet)
		i = i + 1
	end
	return true
end



