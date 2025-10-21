function Effect_SafetyPopo_Begin(actor, iEffectID, iEffectKey)
	if false == actor:IsMyActor() then
		return
	end
	
	local kPilot = actor:GetPilot();
	if false == kPilot:IsNil() then
		local kUnit = kPilot:GetUnit();
		if false == kUnit:IsNil() then
		
			local kPacket = NewPacket(12608);--PT_C_M_REQ_MSGBOX_CALL
			kPacket:PushBool(false);
			kPacket:PushInt(iEffectKey);
			CommonMsgBoxCancel( GetTextW(450052):GetStr(), kPacket, false );
			DeletePacket(kPacket);
			
			local kEffect = kUnit:FindEffect( iEffectID );
			if false == kEffect:IsNil() then
				local kUI = GetUIWnd("SFRM_MSG_COMMON_CANCEL");
				if false == kUI:IsNil() then
					kUI = kUI:GetControl("SFRM_TIMER_COUNT");
					if false == kUI:IsNil() then
						local kNowTime = GetServerElapsedTime32(false);
						local kEndRemainTime = 0;
						if kNowTime > kEffect:GetEndTime() then
							kEndRemainTime = kNowTime - kEffect:GetEndTime();
						else
							kEndRemainTime = kEffect:GetEndTime() - kNowTime;
						end
					
						local fAccumTime = GetAccumTime() + ( kEndRemainTime / 1000 );
						kUI:SetCustomDataAsFloat(fAccumTime);
					end
				end
			end
		end
	end
	
	LockPlayerInput(100);--EPLT_StatusEffect1
end

function Effect_SafetyPopo_End(actor, iEffectID, iEffectKey)
	if false == actor:IsMyActor() then
		return
	end
	
	CloseUI("SFRM_MSG_COMMON_CANCEL");
	
	Net_PT_C_M_REQ_FOLLOWING( actor:FollowingHeadGuid(), 4 );--EFollow_Cancel
	
	UnLockPlayerInput(100);--EPLT_StatusEffect1
end

function Effect_SafetyPopo_OnTick(actor, iEffectID, fAccumTime)
	return
end