function Net_C_M_GodCmd(packet, ground, unit, actarg)
	InfoLog(8, "Net_C_M_GodCmd --")

	local cmd = packet:PopInt()
	InfoLog(8, "Net_C_M_GodCmd cmd:" .. cmd)
	if unit:CheckGmCmd(cmd) == false then
		InfoLog(5, "Net_C_M_GodCmd CheckGmCmd failed")
		return true
	end

	-- Give Item -----------------------------------------------------------------
	if cmd == 1 then
		itemNo = packet:PopInt()
		iCount = packet:PopInt()
		ground:GiveItem(unit, itemNo, iCount)
--		local kVector = Vector()
--	kVector:PushInt(itemNo)
--		ground:GiveItemSet(unit, kVector)

	-- Pet Level Change ----------------------------------------------------------
	--elseif cmd == 2 then
		--InfoLog(4, "Not implemented GOD CMD 2")
		--PetLevel = packet:PopInt()
		--if unit:SetPetLevel(PetLevel) == true then
    	--kPetLvUpPacket = NewPacket(12350)
  		--kPetLvUpPacket:PushGuid(guid)
  		--i64Exp = unit:GetAbil64(AT_EXPERIENCE)
  		--kPetLvUpPacket:PushInt64(i64Exp)
  		--kPetLvUpPacket:PushInt64(i64Exp)
  		--unitmgr:Send(kPetLvUpPacket)
  		--DeletePacket(kPetLvUpPacket)

	-- Monster Regen ---------------------------------------------------------------
	elseif cmd == 3 then
		InfoLog(4, "Not implemented GOD CMD 3")
		--MonID = packet:PopInt()
		--Delay = packet:PopInt()
		--Delay = Delay * 1000
		--unitmgr:MonsterGenerate(MonID , unit:GetPos() , Delay)

	-- Player Class Change ---------------------------------------------------------
	elseif cmd == 4 then
		local newClass = packet:PopInt()
		local newLevel = packet:PopInt()
		local bGod = packet:PopBool()

		local changedClassNo = unit:God_ChangeClass(newClass, newLevel, bGod, ground)

		if changedClassNo == 0 then
			return false
		end
		
	-- Skill Point Edit ----------------------------------------------------------------
	elseif cmd == 5 then
		 reqPoint = packet:PopShort()
	  	unit:God_SkillPoint(reqPoint)
		
	 -- let me be invisibl -----------------------------------------------------------------
	elseif cmd == 6 then
		iAbilNo = packet:PopInt()
		byAble = packet:PopChar()
		if byAble == 0 then
			unit:SetGmAbil(iAbilNo, false)
		else
			unit:SetGmAbil(iAbilNo, true);
		end
		
	-- Heal me plz
	elseif cmd == 7 then
		unit:SetAbil(AT_HP, unit:GetAbil(AT_C_MAX_HP), false)
		InfoLog(9, "HEALME : AT_HP:" .. unit:GetAbil(AT_HP))
		unit:SetAbil(AT_MP, unit:GetAbil(AT_C_MAX_MP), false)
		InfoLog(9, "HEALME : AT_MP:" .. unit:GetAbil(AT_MP))
		local kActArg = NewActArg()
		kActArg:SetInt(ACT_ARG_CUSTOMDATA1, AT_HP)
		kActArg:SetInt(ACT_ARG_CUSTOMDATA1+1, AT_MP)
		unit:SendAbiles(2, kActArg, E_SENDTYPE_SELF)
		DeleteActArg(kActArg)
		
	-- Set Quest UnComplete / Complete -----------------------------------------------------------------
	elseif cmd == 8 then
		local iStartID = packet:PopShort()
		local iEndID = packet:PopShort()
		local bComplete = packet:PopBool()
		unit:GodQuest(iStartID, iEndID, bComplete)
	-- SetAbil64          -----------------------------------------------------------------
	elseif cmd == 9 then
		InfoLog(9, "cmd9 --")
		local wWord = packet:PopShort()
		InfoLog(9, "cmd9 10")
		local kValue = packet:PopInt64()
		InfoLog(9, "cmd9 20")
		unit:God_SetAbil64(wWord, kValue, ground)
		InfoLog(9, "cmd9 30")
	elseif cmd == 10 then
		InfoLog(9, "cmd10 --")
		ground:GstarInvClear(unit)	
	elseif cmd == 11 then
		local kValue = packet:PopInt64()
		ground:ResetMoney(unit, kValue)
	-- All Server Notice	
	elseif cmd == 12 then
		InfoLog(9, "cmd12 --")
		local strNotice = packet:PopString()
		InfoLog(9, "Notice : " .. strNotice)
		ground:AllServerNotice(strNotice);
	elseif cmd == 13 then
		InfoLog(9, "cmd13 --")
		local strUserName = packet:PopString()
		InfoLog(9, "KickUser : " .. strUserName)
		ground:KickUser(strUserName);
	elseif cmd == 14 then
		InfoLog(9, "cmd14 --")
		local strUserName = packet:PopString()
		ground:Gm_CallUser(unit, strUserName)
	elseif cmd == 15 then
		InfoLog(9, "cmd15 --")
		local strUserName = packet:PopString()
		local iValue = packet:PopInt();
		ground:BlockUser(strUserName, iValue)		
	elseif cmd == 16 then
		InfoLog(9, "Cmd16 --")
		local iQuestID = packet:PopInt()
		local iPramNo = packet:PopInt()
		local iCount = packet:PopInt()
		unit:GodQuestState(iQuestID, iPramNo, iCount)
		
	-- Give ItemSet -----------------------------------------------------------------
	elseif cmd == 17 then
		itemNo = packet:PopInt()
		local kVector = Vector()
		kVector:PushInt(itemNo)
		ground:GiveItemSet(unit, kVector)
	-- Move Speed Change
	elseif cmd == 18 then
		iSpeed = packet:PopInt()
		unit:GodSpeedChange(iSpeed)
-- Chat Mute
	elseif cmd == 23 then		
		local strUserName = packet:PopString()
		local iTime = packet:PopInt()
		InfoLog(9, "Mute Chat : " .. strUserName)
		InfoLog(9, "Mute Time : " .. iTime)
		ground:MuteUserChat(strUserName, iTime)		
	elseif cmd == 24 then
		local iQuestID = packet:PopShort()
		unit:GodBeginQuest(iQuestID)
	elseif cmd == 25 then
		local wWord = packet:PopShort()
		local kValue = packet:PopInt()
		unit:God_SetAbil(wWord, kValue, ground)
	elseif cmd == 26 then
		local InvPos = packet:PopChar()
		local ModifyType = packet:PopInt()
		local Value1 = packet:PopInt()
		local Value2 = packet:PopInt()
		unit:God_SetEnchant(ModifyType,InvPos,Value1,Value2,ground)
	end
	
	return true
end

function Net_C_M_ReqLearnSkill(packet, ground, unit, actarg)
	local iNewSkill = packet:PopInt()
	local iResult;
	local iResult = unit:LearnSkill(iNewSkill)
	local kSPacket = NewPacket(PT_M_C_RES_LEARN_SKILL)
	kSPacket:PushInt(iNewSkill)
	kSPacket:PushInt(iResult)
	kSPacket:PushShort(unit:GetAbil(AT_SP))
	unit:Send(kSPacket)
	DeletePacket(kSPacket)
	return true
end

function Net_C_M_GOD_ADDMONSTER(packet, ground, unit, actarg)
	InfoLog(8, "Net_C_M_GOD_ADDMONSTER cmd:" .. 101)
	if unit:CheckGmCmd(101) == false then
		InfoLog(5, "Net_C_M_GodCmd CheckGmCmd failed")
		return true
	end

	local iAddMonster = packet:PopInt()
	local kPos = ground:GetUnitFrontPos(unit, 100)
	ground:InsertMonster(iAddMonster, kPos)
end

function Net_C_M_GodCmd2(packet, ground, unit, actarg)
	local cmd = packet:PopInt()
	InfoLog(8, "Net_C_M_GodCmd cmd:" .. cmd)
	if unit:CheckGmCmd(cmd) == false then
		InfoLog(5, "Net_C_M_GodCmd CheckGmCmd failed")
		return true
	end

	-- Net_SetItemCount(InvType, InvPos, Count)
	if cmd == 17 then
		local sInvType = packet:PopShort()
		local sInvPos = packet:PopShort()
		local iCount = packet:PopInt()
		ground:SetItemCount(unit, sInvType, sInvPos, iCount)

	-- Net_MissionMove(iMissionKey,iLevel)
	elseif 18 == cmd then
	    local iMissionKey = packet:PopInt()
	    local iMissionLevel = packet:PopInt()
	    ground:SetMissionMove(unit,iMissionKey,iMissionLevel)
	
	-- Net_MissionStage(iStageNo)
	elseif 19 == cmd then
	    local iStageNo = packet:PopInt()
	    ground:SetChangeMissionStage(iStageNo)
	elseif 20 == cmd then
		local kSkill = unit:GetSkill()
		if kSkill:IsNil() == false then
			kSkill:ClearCoolTime()
			local kSPacket = NewPacket(PT_M_C_NFY_COOLTIME_INIT)
			kSPacket:PushGuid(unit:GetGuid())
			unit:Send(kSPacket)
			DeletePacket(kSPacket)
		end
	
	-- Net_KillUnit(kUnitGuid)
	elseif 21 == cmd then
	    local kGuid = packet:PopGuid()
	    if kGuid:IsNil() then
	        ground:RemoveAllMonster(true)
		else
			local unit = ground:GetUnit(kGuid)
			if false == unit:IsNil() then
			    unit:Kill(ground)
			end
	    end
	elseif 22 == cmd then
		local iTeam = packet:PopInt()
		unit:SetAbil(AT_TEAM,iTeam,true,true)
	end		
end

function Net_C_NT_GODCMD(packet, ground, unit, actarg)
	local cmd = packet:PopInt()
	InfoLog(8, "Net_C_N_GODCMD cmd:" .. cmd)
	
	if unit:CheckGmCmd(cmd) == false then
		InfoLog(5, "Net_C_N_GODCMD CheckGmCmd failed. cmd:" .. cmd)
		return true
	end
	RecvContentsGodCmd(packet,cmd,unit)
end


