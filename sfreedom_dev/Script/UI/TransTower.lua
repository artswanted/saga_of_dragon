function CallNpcTRANSPORT( kGuid )
	
	local kUI = nil
	
	if nil == kGuid or kGuid:IsNil() then
		kUI = GetUIWnd("SFRM_TRANSTOWER")
	else
		kUI = ActivateUI("SFRM_TRANSTOWER")
	end
	
	if false == kUI:IsNil() then
	
		local sContinent = CONTINENT_OF_WIND
		
		if nil == kGuid or kGuid:IsNil() then
			kGuid = kUI:GetCustomDataAsGuid()
			sContinent = 0
		else
			kUI:SetCustomDataAsGuid( kGuid )
		end
		
		if false == CallTransTowerList( kGuid, kUI:GetControl("LST_LIST"), sContinent ) then
			kUI:Close()
		end
	end
end

function CallPetTRANSPORT( kPetGuid ) --이곳의 GUID는 타워가 아닌 펫이다.
	
	local kUI = nil
	
	if nil == kPetGuid or kPetGuid:IsNil() or GetUIWnd("SFRM_TRANSTOWER"):IsNil() == false then
		return --GUID가 유효하지 않거나 현재 UI창이 떠있으면 실패시킨다
	end
	
	kUI = ActivateUI("SFRM_TRANSTOWER")
	if kUI:IsNil() or kUI:GetControl("FRM_CALLER_IS_PET"):IsNil() then
		return
	end
	local kPetInfo = kUI:GetControl("FRM_CALLER_IS_PET")
	if kPetInfo:IsNil() then
		return
	end

	kPetInfo:Visible(true) --전송타워가 아닌 펫스킬로 실행 되었다고 알린다.
	kPetInfo:SetCustomDataAsGuid(kPetGuid)

	if false == CallTransTowerList( GUID("7760a860-0379-4298-947a-871ce005ceac"), kUI:GetControl("LST_LIST"), CONTINENT_OF_WIND ) then
		kUI:Close()
	end

end

function CallNpcSAVEPOS( kGuid )
	local packet = NewPacket(8258)		-- PT_C_M_REQ_TRANSTOWER
	packet:PushGuid(kGuid)
	packet:PushInt(0)--SavePos
	Net_Send(packet)
	DeletePacket(packet)
end

function ClickTransTowerWorldTab(kSelf)
	if nil==kSelf or kSelf:IsNil() then return end

	local kParent = kSelf:GetParent():GetParent()
	if kParent:IsNil() then return end
	
	for i = 0, 3 do
		local kWorld = kParent:GetControl("SFRM_WORLD_" .. i)
		if false==kWorld:IsNil() then
			local kSelect = kWorld:GetControl("BTN"):GetControl("IMG_SELECT")
			kSelect:SetUVIndex(1)
		end
	end
	
	kSelf:GetControl("IMG_SELECT"):SetUVIndex(3)
end