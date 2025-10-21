-- Magician (너스 콜 : Nurse Call) : Level 1-10
function Effect_Begin102000601(unit, iEffectNo, actarg)

	return 1
end

function Effect_End102000601(unit, iEffectNo, actarg)

	return 1
end

function Effect_Tick102000601(unit, elapsedtime, effect, actarg)
	local kGround = actarg:Get(ACTARG_GROUND)
	kGround = Ground(kGround)
	
	local kPos = unit:GetPos()
	local kUnitArray = NewUNIT_PTR_ARRAY()
	
	-- 파티검사
	local bParty = true
	local kCheckGuid = unit:GetPartyGuid()
	if true == kCheckGuid:IsNil() then
		kCheckGuid = unit:GetCaller()
		bParty = false
	end
	
	kGround:GetUnitInRange(kPos, effect:GetAbil(AT_DETECT_RANGE), UT_PLAYER, kUnitArray)
	local iRecoveryMP = effect:GetAbil(AT_MP_RECOVERY_INTERVAL)
	local iIndex = 0
	local kPlayer = kUnitArray:GetUnit(iIndex)
	local iNowMP, iMaxMP, iNewMP
	while kPlayer:IsNil() == false do
		
		local bOK = false
		if true == bParty then
			if true == kCheckGuid:IsEqual( kPlayer:GetPartyGuid()) then
				bOK = true
			end
		else
			if true == kCheckGuid:IsEqual( kPlayer:GetGuid() ) then
				bOK = true
			end
		end
		
		if true == bOK then
			iNowMP = kPlayer:GetAbil(AT_MP)
			iMaxMP = kPlayer:GetAbil(AT_C_MAX_MP)
			if iNowMP ~= iMaxMP then
				iNewMP = math.min(iMaxMP, iNowMP+iRecoveryMP)
				kPlayer:SetAbil(AT_MP, iNewMP, true)
--				InfoLog(6, "Nurse Call Add Magic " .. iNowMP .. "->" .. iNewMP )

				-- Send
				local kPacket = NewPacket(PT_M_C_NFY_ABILCHANGED)
				kPacket:PushGuid(kPlayer:GetGuid())
				kPacket:PushShort(AT_MP)
				kPacket:PushInt(iNewMP)
				kPacket:PushGuid(effect:GetCaster())
				kPacket:PushInt(effect:GetEffectNo())
				kPlayer:Send(kPacket, E_SENDTYPE_BROADALL)
				DeletePacket(kPacket)
			end
		end
	
		iIndex = iIndex + 1
		kPlayer = kUnitArray:GetUnit(iIndex)
		
	end	-- While
	
	DeleteUNIT_PTR_ARRAY(kUnitArray)
	return ECT_DOTICK
end