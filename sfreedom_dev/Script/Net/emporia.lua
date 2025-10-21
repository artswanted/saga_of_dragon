
function ActEmporiaFunc( trigger, actor )

	if true == IsIamGroundOwner() then
	
		local nFuncNo = trigger:GetParam()
		if 1 == nFuncNo then
			if true == AmIGuildOwner() then
				Net_ReqEmporiaFunc( nFuncNo )
			else
				AddWarnDataTT(70096)--길드장만 사용 할 수 있습니다.
			end
		elseif 3 == nFuncNo then
			ActivateUI("SFRM_ITEM_PLUS_UPGRADE")
		else
			Net_ReqEmporiaFunc( nFuncNo )
		end
		
	else
		AddWarnDataTT(70097)
	end
	
end

function Net_ReqEmporiaFunc( nFuncNo )
	local kPacket = NewPacket(30810)--PT_C_M_REQ_USE_EMPORIA_FUNCTION
	kPacket:PushShort(nFuncNo)
	Net_Send(kPacket)
	DeletePacket(kPacket)
end

function Net_ReqEmporiaReserve( kGUID, iExp )
	ODS(" EXPEXPEXP " .. iExp .. "\n")
	local i64Exp = INT64()
	i64Exp:Set( iExp, 0 )
	
	local kPacket = NewPacket(50101)	--PT_C_N_REQ_GUILD_COMMAND
	kPacket:PushByte(28)	-- GC_ReserveBattleForEmporia
	kPacket:PushBool(false)	-- No Throw
	kPacket:PushGuid(kGUID)
	kPacket:PushInt64(i64Exp)
 	Net_Send(kPacket)
	DeletePacket(kPacket)
end

function OnTick_EmporiaRemainReserveTime( kSelf )
	if nil ~= kSelf and false == kSelf:IsNil() then
		local iRemainTimeMin = kSelf:GetCustomDataAsInt()
		if iRemainTimeMin > 0 then
			iRemainTimeMin = iRemainTimeMin - 1
			
			kSelf:SetCustomDataAsInt( iRemainTimeMin )
			local iHour = math.floor(iRemainTimeMin/60)
			local iMin = math.floor(iRemainTimeMin%60)
			kSelf:SetStaticText( string.format(GetTextW(71037):GetStr(), iHour, iMin) )
			
			if 0 == iRemainTimeMin then
			end
		end
	end
end

function Net_Req_PT_C_M_REQ_SELECT_BATTLE_AREA( index )
	local kPlayer = g_pilotMan:GetPlayerUnit()
	if true == IsFullEmporiaBattleArea(index) then
		AddWarnDataTT(201785)
		return
	end
	if false == kPlayer:IsNil() then
		if false == kPlayer:IsDead() then
			local kEffect = kPlayer:GetEffect( 18001 )
			if true == kEffect:IsNil() then
				local kPacket = NewPacket(30580)	-- PT_C_M_REQ_SELECT_BATTLE_AREA
				kPacket:PushInt(index)
				Net_Send(kPacket)
				DeletePacket(kPacket)
			
				CloseUI("FRM_SELECT_LOCATION")
				CloseUI("SFRM_EMBATTLE_PTDRA_TAB")
				return
			end
		end
		
		g_ChatMgrClient:Notice_Show_ByTextTableNo(235)
	end
end
