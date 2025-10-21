--g_indexxxxxxxxxxxx = 1010101
--g_bAddedUIScene = false
--g_iMapMoveStage = 0
--g_MissionWallGroup = PG_PHYSX_GROUP_MISSION_WALL_0		-- 미션맵 벽 통과용

function gi(num)
   Net_GiveItem(num)
end

function Net_ReqMissionInfo(mission_key)
	ODS("Net_ReqMissionInfo mission_key : " ..mission_key.."\n");

--[[	if IsInParty() == true then
		local kPlayerPilot = g_pilotMan:GetPlayerPilot();
		if kPlayerPilot:IsNil() == true or IsPartyMaster(kPlayerPilot:GetGuid()) == false then
			ODS("파장이 아님\n")
			AddWarnDataTT(400230)
			return false;
		end
	end ]]--

	local packet = NewPacket(13400)		-- PT_C_M_REQ_MISSION_INFO
	packet:PushInt(mission_key)

	Net_Send(packet)
	DeletePacket(packet)
end

--------------------------------------------------	xxx
function Net_EnterOtherCharacter(packet)
	if( CheckNil(g_world == nil) ) then return false end
	if( CheckNil(g_world:IsNil()) ) then return false end
	g_world:Recv_PT_M_C_ADD_UNIT(packet)
	return true
end


-------------------------------------------------- 	xxx
function Net_RemoveCharacter(packet)
	local cntChar = packet:PopInt()
	local i = 0
	ODS("Net_RemoveCharacter cntChar:"..cntChar.."\n")
	while i < cntChar do
		local guidPilot = packet:PopGuid()	-- 나간사람 GUID
		--guidPilot:ODS()
		--MessageBox("Remove Character!")
		local	kPilot = g_pilotMan:FindPilot(guidPilot);
		
		if kPilot:IsNil() == false then
			ODS("Net_RemoveCharacter guidPilot:"..guidPilot:GetString().." Name:"..kPilot:GetName():GetStr().." \n")
		else
			ODS("Net_RemoveCharacter guidPilot:"..guidPilot:GetString().."\n")
		end
	
		-- 방어코드
		if guidPilot:IsEqual(g_playerInfo.guidPilot) == false then
			--local pilot = g_pilotMan:FindPilot(guidPilot)
			if kPilot:IsNil() == false then -- 파일럿 있으면.
				if kPilot:GetActor():GetAbil(AT_MANUAL_DIEMOTION) == 0 then	-- 0은 서버에서 죽임, 101은 죽이지 않음(후처리 필요)
					if( CheckNil(g_world == nil) ) then return false end
					if( CheckNil(g_world:IsNil()) ) then return false end
					if kPilot:IsUnitType(UT_GROUNDBOX) == true then -- 아이템박스면.
						local kDrop = PilotToDropBox(kPilot) -- 파일럿을 드랍박스로 바꾸고.
							if kDrop:IsNil() == false then -- 드랍박스로 바뀌면.
								local OwnerGuid = kDrop:GetOwnerGuid() -- 오너 아이디가 
								if OwnerGuid:IsNil() == true then	-- 주인없는 놈이면
								    ODS("Remove DropBox\n");
									g_world:RemoveActorOnNextUpdate(guidPilot)	 -- 다음번에 없에라.
								end
							end
					elseif kPilot:IsUnitType(UT_MONSTER) == false or kPilot:GetAbil(AT_HP) > 0 then --몬스터가 아니면?.
					    ODS("Remove Monster\n");
						g_world:RemoveActorOnNextUpdate(guidPilot)	
					end

				end
			elseif g_pilotMan:IsReservedPilot(guidPilot) == true then
                ODS("Remove Reserved Pilot Guid\n");
   			    g_pilotMan:RemoveReservedPilot(guidPilot);
			    
			end
		end
		
	    i = i + 1
	end
	return true
end

--------------------------------------------------
function Net_RemoveMonster(packet)
	if( CheckNil(g_world == nil) ) then return false end
	if( CheckNil(g_world:IsNil()) ) then return false end
	local cntChar = packet:PopInt()
	local i = 0
	while i < cntChar do
		guidPilot = packet:PopGuid()
		g_world:FreezeObject(guidPilot)
		-- TODO : 진짜로 삭제해야 하는 녀석들도 처리해주자.
		i = i + 1
	end
end

--------------------------------------------------
function Net_RecvMove(packet)
--	guidPilot= packet:PopGuid()--Pilot
--	guidSlot = packet:PopGuid()--Slot
--	loc = Point3(packet:PopFloat(), packet:PopFloat(), packet:PopFloat())	-- POINT3 

--	actor = g_world:FindActor(guidPilot)

--	if actor:IsNil() == false then
--		actor:TransitActionGuid(guidSlot)
--		actor:SetTranslate(loc)
--	end
	--받은정보로 캐릭터 싱크를 한다.
		
	return false
end

--[[
-------------------------------------------------
function Net_RecvChat(packet)
	g_ChatMgrClient:RecvChat(packet)
	return true
end
-------------------------------------------------
function Net_RecvChat_InputNow(packet)
	g_ChatMgrClient:RecvChat_InputNow(packet)
	return true
end
-------------------------------------------------
function Net_RecvFriend_Comamnd(packet)
	g_FriendMgr:RecvFriend_Command(packet)
	Update_FriendList()
	return true
end
]]
function Net_ReqChannelInfo()
	local kMyPlayer = g_pilotMan:GetPlayerPilot()
	if false == kMyPlayer:IsNil() then
		if( CheckNil(g_world == nil) ) then return false end
		if( CheckNil(g_world:IsNil()) ) then return false end
		if g_world:IsHaveWorldAttr(GATTR_INSTANCE) then
			-- 사용 할 수 없음
			g_ChatMgrClient:Notice_Show_ByTextTableNo(98)
			return
		end
		if kMyPlayer:GetAbil(AT_HP) <= 0 then
			-- 사용 할 수 없음
			g_ChatMgrClient:Notice_Show_ByTextTableNo(98)
			return
		end
		local packet = NewPacket(12518)--PT_C_T_REQ_CHANNLE_INFORMATION
		Net_Send(packet)
		DeletePacket(packet)
	end
end

function Net_ReqChannelChange(nChannelNo)
	local kChannelChangeUI = GetUIWnd("FRM_CHANNEL_CHANGING")
	if kChannelChangeUI:IsNil() then
		local packet = NewPacket(12520)--PT_C_M_REQ_CHANNEL_CHANGE
		packet:PushShort(nChannelNo)
		Net_Send(packet)
		DeletePacket(packet)
	end
end

-------------------------------------------------
g_bNowGmChat = {}
g_bNowGmChat.kGmGuid = nil
g_bNowGmChat.kPetitionGuid = nil
g_bNowGmChat.kHistoryCount = 0
g_bNowGmChat.kHistory = {}

function Net_PT_O_C_NFY_GMCHAT(packet) 	--Recv Gm Chat
	local kGuid = packet:PopGuid() 				--GM Unique Guid
	g_bNowGmChat.kPetitionGuid = packet:PopGuid()	-- PetitionGoud
--	ODS("g_bNowGmChat.kPetitionGuid:"..g_bNowGmChat.kPetitionGuid:GetString().."\n");	
	local kGmName = packet:PopWString() 	--GmName
	local kChat = packet:PopWString() 		--Comment/Contents
	
--	if 0 == kChat:Length() then--comment Length is 0 'Close connection Gm Chat'
--		CloseUI("SFRM_PATITION_CHAT_DLG")
--		g_bNowGmChat.kHistoryCount = 0
--		g_bNowGmChat.kHistory = {}
--		return
--	end
	
	g_bNowGmChat.kGmGuid = kGuid
	local kWnd = GetUIWnd("SFRM_PATITION_CHAT_DLG")
	if kWnd:IsNil() then
		kWnd = CallUI("SFRM_PATITION_CHAT_DLG")
	end
	
	local kChatList = kWnd:GetControl("LIST_CHAT_HISTORY")
	if kChatList:IsNil() then ODS("[Net_PT_T_C_NFY_GMCHAT] LIST_CHAT_HISTORY is nil\n") return end
	
	local kContents = WideString(kGmName:GetStr() .. " : " .. kChat:GetStr())
	local kItem = kChatList:AddNewListItem(kContents)
	if kItem:IsNil() then return true end
	local kItemWnd = kItem:GetWnd()
	if kItemWnd:IsNil() then return true end
	kItemWnd:SetFontColorRGBA(146, 37, 142, 255)
	
	--Add Chat Log
	g_bNowGmChat.kHistoryCount = g_bNowGmChat.kHistoryCount + 1
	g_bNowGmChat.kHistory[g_bNowGmChat.kHistoryCount] = kContents
	return true
end

function Net_ReqGMChat(kSelf)--Send GM Chat
	if not kSelf:IsFocus() then
		return false
	end
	
	if nil == g_bNowGmChat.kGmGuid then
		return false
	end
	
	if kSelf:IsNil() then return false end
	local kChat = kSelf:GetEditText()
	local kMyName = g_pilotMan:GetPlayerPilot():GetName()
	if 0 == kChat:Length() then
		return
	end
	kSelf:SetEditText("")
	--kChat:SetEditFocus(false)
	
	--local kMyMemGuid = :GetGuid()--Get My MemberGuid
	packet = NewPacket(50602)--PT_C_O_REQ_GMCHAT
	packet:PushGuid(g_bNowGmChat.kGmGuid)
	ODS("g_bNowGmChat.kPetitionGuid:"..g_bNowGmChat.kPetitionGuid:GetString().."\n");	
	packet:PushGuid(g_bNowGmChat.kPetitionGuid)
	packet:PushWString(kMyName) --MyName
	packet:PushWString(kChat)--Chat
	Net_Send(packet)
	DeletePacket(packet)
	
	local kWnd = GetUIWnd("SFRM_PATITION_CHAT_DLG")
	if kWnd:IsNil() then
		kWnd = CallUI("SFRM_PATITION_CHAT_DLG")
	end
	local kChatList = kWnd:GetControl("LIST_CHAT_HISTORY")
	if kChatList:IsNil() then ODS("[Net_PT_T_C_NFY_GMCHAT] LIST_CHAT_HISTORY is nil\n") return end
	
	kChat = WideString(kMyName:GetStr() .. " : " .. kChat:GetStr())	
	local kItem = kChatList:AddNewListItem(kChat)
	if kItem:IsNil() then return true end
	local kItemWnd = kItem:GetWnd()
	if kItemWnd:IsNil() then return true end
	kItemWnd:SetFontColorRGBA(30, 30, 30, 255)
	
	--Add Chat Log
	g_bNowGmChat.kHistoryCount = g_bNowGmChat.kHistoryCount + 1
	g_bNowGmChat.kHistory[g_bNowGmChat.kHistoryCount] = kChat
	
	return true
end

function Net_ClosePetitionChatWindow(kSelf)
	local kWnd = GetUIWnd("SFRM_PATITION_CHAT_DLG")
	if kWnd:IsNil() then
		kWnd = CallUI("SFRM_PATITION_CHAT_DLG")
	end
	local kChatList = kWnd:GetControl("LIST_CHAT_HISTORY")
	if kChatList:IsNil() then ODS("[Net_PT_T_C_NFY_GMCHAT] LIST_CHAT_HISTORY is nil\n") return end
	kChatList:ClearAllListItem()
	CloseUI("SFRM_PATITION_CHAT_DLG")
	g_bNowGmChat.kHistoryCount = 0
	g_bNowGmChat.kHistory = {}		
end

function Net_ReqDamage(iActionID,iInstanceID,kTargetGUID)
--	ODS("Net_ReqDamage ActionID:"..iActionID.." InstanceID:"..iInstanceID.."\n");
	packet = NewPacket(12340) 				--PT_C_M_REQ_DAMAGE
	packet:PushInt(iActionID)
	packet:PushInt(iInstanceID);
	packet:PushGuid(kTargetGUID)	--맞을놈
	Net_Send(packet)
	DeletePacket(packet)
end

function Net_ReqDamageD(iActionID,iInstanceID,kTargetGUID)
	packet = NewPacket(12341) 				--PT_C_M_REQ_DAMAGED
	packet:PushInt(iActionID)
	packet:PushInt(iInstanceID);
	packet:PushGuid(kTargetGUID)	--맞을놈
	Net_Send(packet)
	DeletePacket(packet)
end

function Net_C_M_REQ_MON_BLOWSTATUS(kMonGUID,kPosition,bFirstDown)--bFirstDown : 땅에 처음 닿았을 때
	ODS("Net_C_M_REQ_MON_BLOWSTATUS kMonGUID:"..kMonGUID:GetString().." kPosition:"..kPosition:GetX()..","..kPosition:GetY()..","..kPosition:GetZ().."\n");
	packet = NewPacket(13200) 				--PT_C_M_REQ_DAMAGED
	packet:PushGuid(kMonGUID)
	packet:PushFloat(kPosition:GetX())
	packet:PushFloat(kPosition:GetY())
	packet:PushFloat(kPosition:GetZ())
	if nil==bFirstDown then
		bFirstDown = false
	end
	packet:PushBool(bFirstDown)
	Net_Send(packet)
	DeletePacket(packet)
end
function Net_C_M_REQ_LEARN_SKILL(iNextSkillNo)
	--MessageBox("Net_C_M_REQ_LEARN_SKILL iNextSkillNo:"..iNextSkillNo,"");
	packet = NewPacket(12560) 				--PT_C_M_REQ_LEARN_SKILL
	packet:PushInt(iNextSkillNo)
	Net_Send(packet)
	DeletePacket(packet)
end

function NET_M_C_RES_LEARN_SKILL(packet)
    ODS("NET_M_C_RES_LEARN_SKILL \n")

	local iNewSkillNo = packet:PopInt();
	local iResult = packet:PopInt();
	local sRemainSkillPoint = packet:PopShort();
	--MessageBox("NET_M_C_RES_LEARN_SKILL iNewSkillNo:"..iNewSkillNo.." iResult:"..iResult.." sRemainSkillPoint:"..sRemainSkillPoint,"");
	
	if iResult == 1 then
	
		local kSkillTree = GetSkillTree();
		kSkillTree:NewSkillLearned(iNewSkillNo,sRemainSkillPoint);
		--UI_SKILLWND_ON_NEW_SKILL_LEARNED(iNewSkillNo);
      UI_SKILLWND_ON_NEW_SKILL_LEARNED_NEW(iNewSkillNo);
	end
	local kQuick = GetUIWnd("QuickInv")
	if false == kQuick:IsNil() then
	    kQuick:SetInvalidate();
	end
end

function NET_M_C_RES_DELETE_SKILL(packet)
    ODS("NET_M_C_RES_DELETE_SKILL \n")
	local iNewSkillNo = packet:PopInt();
	local iResult = packet:PopInt();
	--MessageBox("NET_M_C_RES_DELETE_SKILL iNewSkillNo:"..iNewSkillNo.." iResult:"..iResult.." sRemainSkillPoint:"..sRemainSkillPoint,"");
	
	if iResult == 1 then
	
		local kSkillTree = GetSkillTree();
		kSkillTree:DeleteSkill(iNewSkillNo);
		--UI_SKILLWND_ON_NEW_SKILL_LEARNED(iNewSkillNo);
      UI_SKILLWND_ON_NEW_SKILL_LEARNED_NEW(iNewSkillNo);
	end
	local kQuick = GetUIWnd("QuickInv")
	if false == kQuick:IsNil() then
	    kQuick:SetInvalidate();
	end
end

function Net_C_M_REQ_BEGINCAST(iSkillNo)
	local packet = NewPacket(12573)	
	packet:PushInt(iSkillNo)
	packet:PushInt(GetServerElapsedTime32())
	Net_Send(packet)
	DeletePacket(packet)
	
	--ODS("Net_C_M_REQ_BEGINCAST iSkillNo : "..iSkillNo.."\n", false, 3851);
	
	local	kActor = GetMyActor();
	if IsSingleMode() == true then
		kActor:SkillCastingConfirmed(iSkillNo,0);
	end
	
end

function Net_M_C_RES_BEGINCAST(packet)
	local	iSkillNo = packet:PopInt();
	local	sErrorCode = packet:PopShort();
	
	--ODS("Net_M_C_RES_BEGINCAST iSkillNo : "..iSkillNo.." sErrorCode : "..sErrorCode.."\n", false, 3851);

	local	kActor = GetMyActor();
	if kActor:IsNil() == false then
		kActor:SkillCastingConfirmed(iSkillNo,sErrorCode);
	end
end

function Net_RecvDamage(packet)
	-- DAMAGE RESULT    12325
	local guidPlayer = packet:PopGuid()			-- 공격한 Character GUID
	local guidPilot = packet:PopGuid()	-- 맞은 Monster GUID
	local dmg = packet:PopInt()
	local remainHP = packet:PopInt()
	
	ODS("Net_RecvDamage dmg:"..dmg.." remainHP :"..remainHP.."\n");
	
	local iState = packet:PopInt()
	local iBackDist = packet:PopInt()
	local iFlyDist = packet:PopInt()

	if IsVisible("FRM_WAIT_FOR_SIEGE") == true then	-- 공성전 준비중
		CloseUI("FRM_WAIT_FOR_SIEGE")
		SiegeMsgBox(GetTextW(19011):GetStr())
	end

	local pilot = g_pilotMan:FindPilot(guidPilot)
	if pilot:IsNil() == false then
		local actor = pilot:GetActor()
		local attacker_pilot = g_pilotMan:FindPilot(guidPlayer)
		
		if attacker_pilot:IsNil() == true then
			actor:ShowDamageNum(actor:GetPos(),actor:GetPos(),dmg) 
		else
			local attacker_actor = attacker_pilot:GetActor()
			local loc = attacker_actor:GetTranslate()
			actor:ShowDamageNum(loc,actor:GetPos(),dmg)
		end
		
      local iDamageAction = actor:GetAbil(AT_DAMAGEACTION_TYPE)
		if iDamageAction ~= 1 and iDamageAction ~= 101 then
         actor:ReserveTransitAction("a_dmg")
      end
		pilot:SetHP(remainHP) --HP 박아짐.
				
		if actor:IsNil() == false and remainHP == 0 then
			ODS("monster Die GUID = " .. pilot:GetGuid():GetString().."\n")
			if pilot:GetMonsterNo() ~= 23 then
				--actor:ReserveTransitAction("a_die")
				actor:DetachFrom(9191)
			end

			CloseUI("DmgInfo")
--			if guidPilot:IsEqual(g_playerInfo.guidPilot) == true and g_world:IsWorldType(WDT_PVP_KTH) ~= true then
--				ODS("Call DeadDlg[1]\n");
--				CallUI("DeadDlg")
--			end
		end
	end
	
	--SetLastAttackTarget(guidPilot)
end

function Net_RecvAddMonster(packet)
	if( CheckNil(g_world == nil) ) then return false end
	if( CheckNil(g_world:IsNil()) ) then return false end
	g_world:Recv_PT_M_C_ADD_UNIT(packet)
	return true
end

function Net_RecvChangeWeapon(packet)
-- PT_M_C_NFY_WEAPON_CHANGE 12346
	-- 무기교체

	return true
end

--[[
function Net_RecvLvUp(packet)
-- PT_M_C_NFY_LV_CHANGE 12350
	-- 레벨업
	local guidPilot = packet:PopGuid()
	local NewLv = packet:PopInt()
--	local afterExp = packet:PopInt64()

	local pilot = g_pilotMan:FindPilot(guidPilot)
	if pilot:IsNil() == true then
		return true
	end

	pilot:SetAbil(AT_LEVEL, NewLv)

	local actor = g_world:FindActor(guidPilot)
	if actor:IsNil() == true then
		return true
	end

	GetHelpSystem():ActivateByConditionInt("LevelUp",NewLv);
	
	actor:AttachParticle(8382, "char_root", "e_level_up")
	
	--Add EventMessage
	local MyGuid = g_pilotMan:GetPlayerPilot():GetGuid()
	if MyGuid:IsEqual(guidPilot) then
		g_ChatMgrClient:AddEventMessage(700027, true, 2)--Level Up
	end
	return true
end
]]

function Net_AttackDamage(packet)
-- PT_M_C_NFY_ATTACKDAMAGE (12353)
-- 몬스터 공격으로 인한 Character 맞음
-- SDmgInfo structure
	local attacker_guid = packet:PopGuid()	-- 공격한 Monster GUID
	local guidPilot = packet:PopGuid()		-- 맞은캐릭터 GUID
	local distHP = packet:PopInt()			-- 타격치
	local leftHP = packet:PopInt()			-- 캐릭터의 남은 HP
	local iEffectNo = packet:PopInt()	-- 이펙트 ID
	local iParam = packet:PopInt() -- 밀리는 길이
	local bPushback = packet:PopBool() -- 뒤로 밀리는거.
--	packet:PopInt()			-- 캐릭터의 상태 (무시해도 좋은 값)
--	packet:PopInt()			-- BackDist (무시해도 좋은값)
--	packet:PopInt()			-- FlyDist (무시해도 좋은값)

	-- 혹시나 플레이어의 것이 아니면 무시하자
   if guidPilot:IsNil() == true then
      return true
   end
--	if guidPilot:IsEqual(g_playerInfo.guidPilot) == false then
	--	return true
	--end
	
	-- 없는 Pilot에 대한 요청이면 무시하자
	local pilot = g_pilotMan:FindPilot(guidPilot)
	if pilot:IsNil() == true then
		return true
	end
	
	local actor = pilot:GetActor()
   if actor:IsNil() == true then
      return true
   end
   
   local attacker_pilot = g_pilotMan:FindPilot(attacker_guid)
   local attacker_actor = attacker_pilot:GetActor()
   
	if actor:GetAction():GetID() ~= "a_dmg" and actor:GetCanHit() then
      pilot:GetActor():AddEffect(100001901,0,pilot:GetGuid(),0,0,true)
	  	if iEffectNo ~= 0 then
			ODS("_____________EffectNo : " .. iEffectNo .. "\n")
			local kEffectActor = pilot:GetActor()
			local kCurrentAction = kEffectActor:GetAction()
			if kCurrentAction:IsNil() == false and
				kCurrentAction:GetActionNo() ~= iEffectNo then
				kEffectActor:AddEffect(iEffectNo, 0, pilot:GetGuid(), 0, 0, true)
			end
		elseif bPushback == true and iParam ~= 0 then
			local iLeft = 1
			if attacker_actor:IsNil() == false then
				if attacker_actor:IsToLeft() then
					iLeft = 2
				else
					iLeft = 1
				end
			end
			actor:Walk( iLeft, -iParam, 1, true)
		end
		--actor:ReserveTransitAction("a_dmg")

		--if math.random() > 0.5 then
		--	actor:AttachParticle(12, "char_root", "e_dmg")
		--else
		--	actor:AttachParticle(12, "char_root", "e_dmg_cri")
		--end			

		-- 피격 소리 재생
		--actor:PlayWeaponSound(1, actor)
		-- 카메라 흔들기
		--if actor:GetPilotGuid():IsEqual(g_playerInfo.guidPilot) == true then
		--	QuakeCamera(0.6, 1.0)
		--end
	end
	
	if attacker_pilot:IsNil() == true then
		actor:ShowDamageNum(actor:GetPos(),actor:GetPos(),distHP) 
	else
		attacker_actor = attacker_pilot:GetActor()
		local loc = attacker_actor:GetTranslate()
		actor:ShowDamageNum(loc,actor:GetPos(),distHP)
	end	

	-- 플레이어의 HP를 갱신한다.
	pilot:SetAbil(AT_HP, leftHP)

	-- 죽는 모션으로 전이하고
	-- 마을 포탈 창을 연다
--	if leftHP == 0 then
--		--pilot:GetActor():ReserveTransitAction("a_die")
--		pilot:GetActor():AddEffect(pilot:GetActor(),0,0,true,100002101, 0)
--		if g_world:IsWorldType(WDT_PVP_KTH) ~= true then
--			ODS("Call DeadDlg[0]\n");
--			CallUI("DeadDlg")
--		end
--	end

	if IsVisible("FRM_WAIT_FOR_SIEGE") == true then	-- 공성전 준비중
		CloseUI("FRM_WAIT_FOR_SIEGE")
		SiegeMsgBox(GetTextW(19011):GetStr())
	end

	return true
end
--[[
function Net_AnsGBoxInfo(packet)
	-- PT_M_C_ANS_GBOXINFO (12363)
	-- SGroundBoxInfo kGBoxInfo
	-- if (kGBoxInfo.eRet == GITEM_ANS_OK(1))
	--	 {
	--		BYTE byItemNumber
	--		Loop(byItemNumber)
	--		{
	--			SITEM_INBOX sItemInfo
	--		}
	--	 }

	--MessageBox("Net_AnsGBoxInfo", "a")
	local iAnswer = packet:PopInt()			--  Answer (1=OK)
	local boxGuid = packet:PopGuid()			--  Box ID

	-- Find DropBox
	local dropBox = g_world:FindObject(boxGuid)
	dropBox = ToDropBox(dropBox)

	if dropBox:IsNil() == true then
		return true
	end

	if iAnswer == 1 then
		boxGuid:ODS()
		if dropBox:AddItems(packet)	== true then
			local pilot = g_pilotMan:FindPilot(dropBox:GetOwnerGuid())
			if not pilot:IsNil() then
				pilot:SetDropBox(dropBox)
				--pilot:ReserveTransitAction("open")
			end
			
			if dropBox:IsMine() then
				packet = NewPacket(12368) 	-- 모든 아이템 가져오기
				packet:PushGuid(boxGuid)
				Net_Send(packet)
				DeletePacket(packet)
			-- CallUI("UI_Pickup_DropBox")
			end
		end
	else
		ODS("Fail : Net_AnsGBoxInfo : " .. iAnswer .. "\n")
	end
end
]]
function	Net_M_C_NFY_ABILCHANGED(packet)

	local wAbil = packet:PopShort();
	local iNewValue = packet:PopInt();	--	변화된 결과값
	local iChanged = packet:PopInt();	-- 변화량
	
	ODS("Net_M_C_NFY_ABILCHANGED wAbil:"..wAbil.." iNewValue:"..iNewValue.." iChanged:"..iChanged.."\n");

	local kPlayerPilot = g_pilotMan:GetPlayerPilot();
	if  kPlayerPilot:IsNil() == true then
		return
	end
	
	kPlayerPilot:SetAbil(wAbil, iNewValue)
	
	if wAbil == AT_HP then
	
		local kPlayerActor = kPlayerPilot:GetActor();
		if kPlayerActor:IsNil() == false then
		
			kPlayerActor:ShowDamageNum(kPlayerActor:GetPos(),kPlayerActor:GetPos(),-iChanged);
		
		end
		
	
	end
end

function Net_M_C_NFY_ADD_PUPPET(packet)

	local sPuppetNum = packet:PopShort();
	local i =0 ;
	while i<sPuppetNum do
	
		Puppet_GUID = packet:PopGUID();
		Puppet_Type = packet:PopInt();
		Puppet_Quat_w = packet:PopFloat();
		Puppet_Quat_x = packet:PopFloat();
		Puppet_Quat_y = packet:PopFloat();
		Puppet_Quat_z = packet:PopFloat();
		Puppet_Trans_x = packet:PopFloat();
		Puppet_Trans_y = packet:PopFloat();
		Puppet_Trans_z = packet:PopFloat();
		Puppet_Scale = packet:PopFloat();
	
		i = i+1;	
	end
end

function Net_M_C_NFY_REMOVE_PUPPET(packet)

	local sPuppetNum = packet:PopShort();

	local i =0 ;
	while i<sPuppetNum do
	
		Puppet_GUID = packet:PopGUID();
		i = i+1;
	end

end

function Net_M_C_NFY_BEGIN_WORLDACTION(packet)

	if( CheckNil(g_world == nil) ) then return false end
	if( CheckNil(g_world:IsNil()) ) then return false end
	local iWorldActionType = packet:PopInt()
	local ulElapsedTime = packet:PopInt()
	
	local world_action_man = g_world:GetWorldActionMan()
	if world_action_man:IsNil() == true then
	
		MessageBox("g_world:GetWorldActionMan() Failed. world_action_man is nil","Net_M_C_NFY_BEGIN_WORLDACTION Error")
		return
		
	end
	
	local new_world_action = world_action_man:CreateNewWorldActionObject(iWorldActionType)
	if new_world_action:IsNil() == false then
	
		new_world_action:OnPacketReceive(packet)
		new_world_action:Enter(ulElapsedTime)
	
	end
  
  	ODS("Net_M_C_NFY_BEGIN_WORLDACTION:"..iWorldActionType.."\n")
end

function Net_M_C_NFY_BOSSMONSTER(packet)
	if( CheckNil(g_world == nil) ) then return false end
	if( CheckNil(g_world:IsNil()) ) then return false end
	ODS("Boss ")
	local BossGUID = packet:PopGuid()
	if BossGUID:IsNil() == true then
		ODS("BossGUID is Nil\n")
		return
	end
	g_world:SetBossGUID(BossGUID)
	BossGUID:ODS()
end

function Net_M_C_NFY_TARGETUNIT(packet)
	if( CheckNil(g_world == nil) ) then return false end
	if( CheckNil(g_world:IsNil()) ) then return false end

	local MonsterGUID = packet:PopGuid()
	local TargetGUID = packet:PopGuid()
	
	g_world:SetMonsterTarget(MonsterGUID,TargetGUID)
	
end

function Net_M_C_COMMON_MESSAGE(packet)
	local msgNo = packet:PopInt()

	--if msgNo == 1 then
	--	g_world:Show_Party_Message_Box(GetTextW(5), 5000)
	--end
end

---------------------------------------------------------------------
function Net_M_C_NFY_UPDATE_EFFECT(packet)
	packet:PopGuid()
	packet:PopBool()
	packet:PopInt()
end

function Net_PT_M_C_ANS_INDUN_TREE(packet)

	if IsVisible("FRM_OPEN_DUNGEUN_INIT") == false then
		wnd = CallUI("FRM_OPEN_DUNGEUN_INIT")
	else
		wnd = GetUIWnd("FRM_OPEN_DUNGEUN_INIT")
	end

	if wnd:IsNil() == true then
		return true
	end

	local wndList = wnd:GetControl("LST_DUNGEUN_LIST")
	if wndList:IsNil() == true then
		return true
	end

	wndList:ClearAllListItem()
	iCount = packet:PopChar() -- 맵 갯수
	ODS("맵 갯수 : " .. iCount .. "\n")
	for i = 1,iCount do
		local iReqMapNum = packet:PopInt() -- 맵 넘버
		ODS("맵 넘버 : " .. iReqMapNum .. "\n")
		local iMinlv = packet:PopShort()
		local iMaxlv = packet:PopShort()

		local iReqItem = packet:PopInt()	-- 필요 아이템 번호(아마도 열쇠)
		local wstrMapName = packet:PopWString() -- 맵 이름 스트링
		ODS(wstrMapName:GetStr() .. "\n")
		local wndItem = wndList:AddNewListItem(wstrMapName):GetWnd()
		wndItem:SetCustomDataAsInt(iReqMapNum)
		local wndIcon = wndItem:GetControl("ICN_KEY")
		if wndIcon:IsNil() == false then
			wndIcon:SetIconKey(iReqItem)
		end
		
		wndItem:GetControl("FRM_TEXT2"):SetStaticText(tostring(iMinlv) .. GetTextW(46):GetStr() .. " ~ " .. tostring(iMaxlv) .. GetTextW(46):GetStr())
		
	end
	
end

function Net_PT_C_M_REQ_INDUN_TREE(iMapNum)
	local packet = NewPacket(13301)	-- 패킷 번호 바꿔야 함
	packet:PushInt(iMapNum)
	Net_Send(packet)
	DeletePacket(packet)
end
function Net_PT_M_C_NFY_AREA_DATA(packet)
	if( CheckNil(g_world == nil) ) then return false end
	if( CheckNil(g_world:IsNil()) ) then return false end
	g_world:Recv_PT_M_C_NFY_AREA_DATA(packet)
	return true
end

function Net_PT_M_C_ADD_UNIT(packet)
	if( CheckNil(g_world == nil) ) then return false end
	if( CheckNil(g_world:IsNil()) ) then return false end
	g_world:Recv_PT_M_C_ADD_UNIT(packet)
	return true
end

function Net_M_C_RES_ACTION2(packet)
	local iActionID = packet:PopInt();
	local iActionInstanceID = packet:PopInt();
	local sErrorCode = packet:PopShort();
	
	ODS("Net_M_C_RES_ACTION2 iActionID:"..iActionID.." iActionInstanceID:"..iActionInstanceID.." sErrorCode:"..sErrorCode.."\n");
	
	if GetMyActor():IsNil() == false then
		GetMyActor():CancelAction(iActionID,iActionInstanceID,"a_battle_idle"); -- 임시로 주석 처리함 leesg213
	end
	
end


-- Recive Goal Pos & Target Guid
function Net_M_C_NFY_MONSTERGOAL(packet)
	local guidPilot = packet:PopGuid()		-- Character GUID
--	local iElapsedTime = packet:PopInt()	-- Elapsed Time 
--	local kCurrentX = packet:PopFloat()		-- Current Position (On Server)
--	local kCurrentY = packet:PopFloat()
--	local kCurrentZ = packet:PopFloat()
	local x = packet:PopFloat()				-- Goal Position
	local y = packet:PopFloat()
	local z = packet:PopFloat()
	--local guidTarget = packet:PopGuid()
--	local ptCurrentPos = Point3(kCurrentX, kCurrentY, kCurrentZ)
	local pt3Goal = Point3(x,y,z)
	
	ODS("\n__________________________Net_M_C_NFY_MONSTERGOAL guidPilot:"..guidPilot:GetString().." Loc:"..x..","..y..","..z.."\n");
	
	local pilot = g_pilotMan:FindPilot(guidPilot)
	if pilot:IsNil() == true then
		return true
	end

	local actor = pilot:GetActor()
	if actor:IsNil() == true then
		return true
	end

	pilot:SetGoalPos(pt3Goal)
	actor:SetWalkingTargetLoc(pt3Goal, 1, "a_idle", 1)
	actor:ReserveTransitAction("a_walk_left")
--	actor:ShowChatBalloon("I'm going to goal pos(" .. x .. ", " .. y .. ", " .. z)
--	local kAction = actor:GetReservedTransitAction()
--	kAction:SetActionTerm(iElapsedTime)

--	pt3Goal:SetZ(actor:GetPos():GetZ())
--	actor:AttachParticleToPoint(g_indexxxxxxxxxxxx + 1, pt3Goal, "ef_thi_charge_01_loop_p_wp_l_hand")
--	g_indexxxxxxxxxxxx = g_indexxxxxxxxxxxx + 1
--	actor:SetWalkingTarget(true,pt3Goal,true)
--	pilot:SetTarget(guidTarget)
end

function CallDmgInfo(guid)
	-- 더이상 이 기능을 사용하지 않음 leesg213
	--local bossguid = g_world:GetBossGUID()

	--if guid:IsNil() == true then
	--	return
	--elseif bossguid:IsNil() == false and guid:IsEqual(bossguid) == true then
    --  return
   --end

	--ActivateUI("DmgInfo")
	--SetLastAttackTarget(guid)
end

function Net_C_M_SYNC_MOVE(byIsBegin,byAction,kCurPos)

	local packet = NewPacket(12337)		-- PT_C_M_REQ_MISSION_INFO
	packet:PushChar(byIsBegin);
	packet:PushChar(byAction);
	packet:PushFloat(kCurPos:GetX());
	packet:PushFloat(kCurPos:GetY());
	packet:PushFloat(kCurPos:GetZ());

	Net_Send(packet)
	DeletePacket(packet)

end

function Net_M_C_SYNC_MOVE_BEGIN(packet)

end

function Net_M_C_SYNC_MOVE_TICK(packet)

end

--[[
function    Net_C_M_REQ_DELETE_ENTITY(kEntityGuid)

    ODS("Net_C_M_REQ_DELETE_ENTITY kEntityGuid:"..kEntityGuid:GetString());

	local kPacket = NewPacket(76)
    
    kPacket:PushGuid(kEntityGuid)

	Net_Send(kPacket)
	DeletePacket(kPacket)
	
end]]--

function    Net_M_C_NFY_TOGGLESKILL_ON_OFF(packet)

    local   iSkillNo = packet:PopInt()
    local   bOn = packet:PopBool();
    local 	kGuid = packet:PopGuid()
    if bOn then
        ODS("Net_M_C_NFY_TOGGLSKILL_ON_OFF iSkillNo:"..iSkillNo.." ON\n")
    else
        ODS("Net_M_C_NFY_TOGGLSKILL_ON_OFF iSkillNo:"..iSkillNo.." OFF\n")
    end
    
	
	if kGuid:IsNil() then return end	
	local	kPilot = g_pilotMan:FindPilot(kGuid)
	if kPilot:IsNil() then return end		
	local	kActor = kPilot:GetActor();
	
    if kActor:IsNil() == false then    
        kActor:ActionToggleStateChange(iSkillNo,bOn);   
		kActor:StartSkillCoolTime(iSkillNo);    
		if( kActor:IsUnitType(UT_PLAYER)
			or  kActor:IsUnitType(UT_SUB_PLAYER)
			) then
			local kAction = kActor:GetAction()
			if(false == kAction:IsNil() and kAction:GetActionNo() == iSkillNo) then
				local iLastAniSlot = kAction:GetSlotCount()-1
				local iCurSlot = kAction:GetCurrentSlot()
				if(iCurSlot == iLastAniSlot) then 
					kAction:SetParam(1, "end")		-- 마무리 애니는 없거나
				else
					kAction:SetSlot(iLastAniSlot)	-- 무조건 한개 애니만 허용함
					kActor:PlayCurrentSlot()
				end
			end
		end
    end

end

function Net_PT_C_M_REQ_VIEW_OTHER_EQUIP(kGuid)
	ODS("Net_PT_C_M_REQ_VIEW_OTHER_EQUIP 시작\n")
	if false==ViewOtherEquip(kGuid) then
		AddWarnDataTT(400667)
	end
	SetOtherGuid(kGuid)

--	local packet = NewPacket(13220)
--	packet:PushGuid(kGuid)
--	Net_Send(packet)
--	DeletePacket(packet)
end

function Net_SendPacketByNo(no)
	if 0<=no then return end

	local kPacket = NewPacket(no)
	Net_Send(kPacket)
	DeletePacket(kPacket)
end

function Net_PT_C_M_REQ_FOLLOWING(kTargetGUID,byMode)
	local bFriendCheck = false
	if IsMyFriend(kTargetGUID) then
		bFriendCheck = true
	end
	
	local	kPacket = NewPacket(12365)
	kPacket:PushByte(byMode)
	kPacket:PushGuid(kTargetGUID)
	kPacket:PushBool(bFriendCheck)
	Net_Send(kPacket)
	DeletePacket(kPacket)
end

function Net_PT_M_C_RES_FOLLOWING(packet)

	ODS("Net_PT_M_C_RES_FOLLOWING\n");

	local	byMode = packet:PopByte()
	local	kHeadGUID = packet:PopGuid();
	local	kFollowGUID = packet:PopGuid();
	
	local	kPilot = g_pilotMan:FindPilot(kFollowGUID)
	if kPilot:IsNil() then
		return
	end
	
	local	kActor = kPilot:GetActor();
	if kActor:IsNil() then
		return
	end
		
	--g_SafeModeTargetFollowGuid = kHeadGUID
	
	kActor:ResponseFollowActor(kHeadGUID,byMode);
end

function Net_PT_M_C_NFY_REMOVE_ALL_MONSTER(packet)
	if( CheckNil(g_world == nil) ) then return false end
	if( CheckNil(g_world:IsNil()) ) then return false end
	g_world:ReserveTransitActionToActors("a_die",UT_MONSTER)
end

function Net_PT_M_C_NFY_REMOVE_ALL_OBJECT(packet)
	if( CheckNil(g_world == nil) ) then return false end
	if( CheckNil(g_world:IsNil()) ) then return false end
	g_world:ReserveTransitActionToActors("a_die",UT_OBJECT)
end

--[[function Net_PT_C_M_CHECK_WORLDEVENT_CONDITION(EventID)
	
	ODS("Net_PT_C_M_CHECK_WORLDEVENT_CONDITION EventID :"..EventID.."\n");

	local packet = NewPacket(18501)
	packet:PushShort(EventID)
	Net_Send(packet)
	DeletePacket(packet)
	
end]]

function Net_PT_M_C_ANS_EVENT_QUEST_TALK(kPacket)
	local iRet = kPacket:PopInt()
	local kNpcGuid = kPacket:PopGuid()
	
	local kNpcFaceID = "Tw_event_girl_normal"
	local iDuplicateTTW = 450218
	local iTimeTTW = 450217
--[[	if GetLocale() == LOCALE.NC_TAIWAN then
		kNpcFaceID = "Tw_event_girl_normal"
		iDuplicateTTW = 450218
		iTimeTTW = 450217
	else
		kNpcFaceID = "Colin_normal"
		iDuplicateTTW = 450227
		iTimeTTW = 450226
	end
]]
	--[[	1 --> Time
			2 --> NotEnd
			3 --> End
			4 --> Duplicate	]]
	if 2 == iRet then -- Not end
		CallNpcFunctionUI2(1001, kNpcGuid, kNpcFaceID, GetEventQuest_NpcTalkNotEnd())
	elseif 3 == iRet then -- Ended
		CallNpcFunctionUI2(1001, kNpcGuid, kNpcFaceID, GetEventQuest_NpcTalkEnd())
	elseif 4 == iRet then -- Duplicate End......
		CallNpcFunctionUI(1001, kNpcGuid, kNpcFaceID, iDuplicateTTW)
	else -- Time
		CallNpcFunctionUI(1001, kNpcGuid, kNpcFaceID, iTimeTTW)
	end
end

function SetVisitFirstMap(bFirst)
	g_VisitFirstMap = bFirst
end

function GetVisitFirstMap()
	return g_VisitFirstMap
end

function OnCloseTownName(bFirst)
	if bFirst then
		OnCallCenterMiniMap(GUID(""), 5000)
	end

	SetVisitFirstMap(false)
end

function Net_PT_C_M_REQ_INTERACTIVE_EMOTION_REQUEST(kMyGUID, kTargetGUID, iActionNo)
	ODS("Net_PT_C_M_REQ_INTERACTIVE_EMOTION_REQUEST", false, 912)
	local	kPacket = NewPacket(20301)
	ODS("iActionNo:"..iActionNo.."\n", false, 912)
	kPacket:PushInt(iActionNo)
	kPacket:PushGuid(kMyGUID)
	kPacket:PushGuid(kTargetGUID)	
	Net_Send(kPacket)
	DeletePacket(kPacket)
end

g_iIncAlphaState = 2
g_iBlinkCount = 3
g_fSecPerCycleUp = 0.4
g_fSecPerCycleDown = 0.2
g_fBeginTime = 0.1

function InitLowHPWarnAlpha()
	g_iIncAlphaState = 0
	g_iBlinkCount = -1
	g_fSecPerCycleUp = 0.4
	g_fSecPerCycleDown = 0.2
	g_fBeginTime = GetAccumTime()
end

function SetLowHPWarnAlpha(iCount, fCycleUpSec, fCycleDownSec)
	InitLowHPWarnAlpha()
	g_fSecPerCycleUp = fCycleUpSec
	g_fSecPerCycleDown = fCycleDownSec
	g_iBlinkCount = iCount
end

function UpdateLowHPWarnAlpha(kWnd)
	if kWnd:IsNil() then
		return
	end
	
	local fAlpha = 0
	local fAccumTime = GetAccumTime()
	local fElapsedTime = fAccumTime - g_fBeginTime
	
	if g_iIncAlphaState == 2 then
		if (fElapsedTime > g_fSecPerCycleUp) or (fElapsedTime == g_fSecPerCycleUp) then
			PlaySoundByID("heartbeat_fast")
			fElapsedTime = g_fSecPerCycleUp
			g_fBeginTime = fAccumTime
			g_iIncAlphaState = 1
		end
		fAlpha = fElapsedTime / g_fSecPerCycleUp
	elseif g_iIncAlphaState == 1 then
		if (fElapsedTime > g_fSecPerCycleDown) or (fElapsedTime == g_fSecPerCycleDown) then
			fElapsedTime = g_fSecPerCycleDown
			g_fBeginTime = fAccumTime
			g_iIncAlphaState = 2
			
			if g_iBlinkCount == 0 then
				kWnd:Close()
				return
			elseif g_iBlinkCount > -1 then
				g_iBlinkCount = g_iBlinkCount - 1
			end
		end
		fAlpha = 1 - (fElapsedTime / g_fSecPerCycleDown)
	else -- 최초 딜레이 (0일때)
		kWnd:SetAlpha(0)
		kWnd:SetMaxAlpha(0)
		if fElapsedTime > 0.1 then
			g_iIncAlphaState = 2
			g_fBeginTime = fAccumTime
		end
		return
	end
	
	kWnd:SetMaxAlpha(math.sin(fAlpha))
end
