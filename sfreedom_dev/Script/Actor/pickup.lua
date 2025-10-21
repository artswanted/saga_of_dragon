function Actor_Pickup(who)
	if( CheckNil(g_world == nil) ) then return false end
	if( CheckNil(g_world:IsNil()) ) then return false end
	actor = ToActor(who)

	if actor:GetPilotGuid():IsEqual(g_playerInfo.guidPilot) then
		return
	end
	
	if GetHelperGuid():IsEqual( actor:GetPilotGuid() ) then
		return
	end
	
	local MyActor = GetMyActor()
	if false == CheckActorDist(MyActor, actor, 110) then
	    return 
    end
	
	local bHaveParty = false
	local kPilot = actor:GetPilot()
	if nil ~= kPilot and not kPilot:IsNil() then
		local kUnit = kPilot:GetUnit()
		if nil ~= kUnit and not kUnit:IsNil() then
			local kPartyGuid = kUnit:GetPartyGuid()
			bHaveParty = not kPartyGuid:IsNil()
		end
	end
	
	--TempOhterSet(actor)
	local kGuid = actor:GetPilotGuid()
	local kPopupItem = {}
	kPopupItem[1] = "VIEW_INFO"
	kPopupItem[2] = "WHISPER"
	if not g_world:IsHaveWorldAttr(GATTR_FLAG_PVP) then
		kPopupItem[3] = "TRADE"
	end
	if not bHaveParty and IsCanPartyArea() then
		kPopupItem[4] = "INVITE_PARTY"
	end
	if bHaveParty and IsCanPartyArea() and not IsInParty() and IsOtherPartyMaster(kGuid) then
		kPopupItem[5] = "JOIN_PARTYFIND"
	end
	kPopupItem[6] = "ADD_FRIEND"
	
	ODS("\n지역이름 : "..GetLocalName():GetStr().."\n")
	
	-- if IsCanPvPArea() then
		 kPopupItem[7] = "INVITE_PVP"
	-- end

	if  false == IsHaveGuild(kGuid) and 
		true  == HaveGuild() 		and 
		true  == AmIGuildMaster() 	then
		kPopupItem[10] = "REQ_GUILD_GUID"
	end
	
	kPopupItem[9] = "COPY_PCNAME_TO_CLIPBOARD"
	
	
	if true == CheckExistCharacterCard(kGuid) then
		kPopupItem[11] = "ORDER_CARD_VIEW"
		if true == CheckRecommendLevel() then
			kPopupItem[12] = "RECOMMEND"
		end
	end
	
	local Menu = false
	local MyPilot = actor:GetPilot()
	if nil ~= MyPilot and not MyPilot:IsNil() then
		local MyUnit = MyPilot:GetUnit()
		if nil ~= MyUnit and not MyUnit:IsNil() then
			Menu = MyUnit:IsEffect(402001, true)			--402001, 98000010
		end
	end
		
	if Menu == false then 
		if not MyActor:IsNowFollowing() then	
			if actor:FindFollowingMeActor(MyActor:GetPilotGuid()) == false then
				kPopupItem[8] = "FOLLOW"
			end
		else
			kPopupItem[8] = "CANCEL_FOLLOW"
		end		
	else
		local bRet = actor:FindFollowingMeActor(MyActor:GetPilotGuid())
		
		if bRet == false then
			kPopupItem[8] = "TAKE_PERSON"
		else
			kPopupItem[8] = "CANCEL_TAKE_PERSON"
		end
	end
	
	--쓸수 있을경우에 여기에 등록----------------------------------------------------------------------------------------------------	
	if(IsMyPlayerLearnInteractiveEmotion()) then -- 상호 이모션을 쓸수 있다면
		kPopupItem[20] = "INTERACTIVE_EMOTION"
	end	
	------------------------------------------------------------------------------------------------------------------------
	
	if GetEventScriptSystem():IsNowActivate() then return end
	
	CallCommonPopup(kPopupItem, kGuid, actor)

	local pilot = g_pilotMan:FindPilot( actor:GetPilotGuid() )
	if pilot:IsNil() == true then
		return
	end

	AttachCircle(pilot)

	g_PickUpPilot = pilot
end

--pet_base.lua의 Act_Pet_Pickup()과 동일한 함수. pet_base.lua를 사용하지 않아 따로 떼어서 사용함. 2010.01.06 조현건
function Pet_Pickup()
	charform = CallUI("CharInfo")
	if charform:IsNil() == true then
		return true
	end
	petform = charform:GetControl("Pet")
	characterForm = charform:GetControl("Charactor")
	characterForm:Visible(false)
	petform:Visible(true)

	GetPetInfo()
	CallPetUI()
end

function AttachCircle(Pickpilot)
	if Pickpilot ~= nil and Pickpilot:IsNil() == false then
		local actor = Pickpilot:GetActor()
		local class = Pickpilot:GetAbil(AT_CLASS)
		ODS("이사람 직업은 : " .. class .. "\n")
		if class == 2 or class == 5 or (class >= 11 and class <= 14) then
			actor:AttachParticle(9191, "char_root", "e_ef_colorshadow_battler")
		elseif class == 3 or class == 7 or class == 22 or (class >= 15 and class <= 17) then
			actor:AttachParticle(9191, "char_root", "e_ef_colorshadow_magician")
		elseif class == 4 or class == 9 or class == 19 or class == 20 then
			actor:AttachParticle(9191, "char_root", "e_ef_colorshadow_archer")
		elseif class == 10 or class == 21 then
			actor:AttachParticle(9191, "char_root", "e_ef_colorshadow_thief")
		end
	end
end

function GroundItemBox_Pickup(who)

	dropbox = ToDropBox(who)

	local bIsMoney = false;
	local IsBearItem = 0
	
	if dropbox:IsNil() == false then
		local	kMyActor= GetMyActor();
		if kMyActor:IsNil() then
			return;
		end
		
		local	kMyPos = kMyActor:GetPos();
		local	kBoxPos = dropbox:GetPos();
		local	fDistance = kBoxPos:Distance(kMyPos);
		bIsMoney = dropbox:IsMoney();
	--	if fDistance>GetItemPickUpLimitDistance() then
	--		ODS("The box is too far from you\n");
	--		return	true
	--	end
		IsBearItem = dropbox:GetAbil(AT_BEAR_ITEM)
	end

	if IsGroundItemBox_Pickup() or bIsMoney then
		if IsBearItem then
		elseif false == bIsMoney then
			PlaySound("../Sound/PC_Sound/Commonaction/Com_Item_Pickup.wav")
		end
		ODS("GroundItemBox_Pickup\n")
		packet = NewPacket(12368)
		packet:PushGuid(GetPilotGUID(who))
		packet:PushGuid(dropbox:GetOwnerGuid())
   		Net_Send(packet)
   		DeletePacket(packet)
	else
		ODS("GroundItemBox_Pickup Failed\n")
	end
end

function Entity_Pickup(who)
	actor = ToActor(who)

	if nil == actor or actor:IsNil() then
		return
	end
	
	local MyActor = GetMyActor()
	if false == CheckActorDist(MyActor, actor, 110) then
	    return 
    end	
	
	local kGuid = actor:GetPilotGuid()
	
	if not IsOpenUIMode7() then
		return
	end

	if IsGuardianInstall(kGuid) then
		CallGuardianUI(kGuid)
		return
	end

	local kPilot = actor:GetPilot()
	if nil == kPilot or kPilot:IsNil() then
		return
	end
	g_PickUpPilot = kPilot
end