
-- Magician (마나실드 : Mana Shield) : Level 1-5
function Effect_Begin102000701(unit, iEffectNo, actarg)
	local kEffectMgr = unit:GetEffectMgr()
	local kEffect = kEffectMgr:GetEffect(iEffectNo, false)
	if kEffect:IsNil() == true then
		InfoLog(5, "Effect_Begin102000701 GetEffect(" .. iEffectNo ..")");
		return 0
	end

	local iPhy = kEffect:GetAbil(AT_PHY_DMG_DEC)
	InfoLog(9, "Effect_Begin102000701 before:PHY:".. iPhy .. ", DEC:" ..unit:GetAbil(AT_PDMG_DEC_RATE_2ND))
	unit:AddAbil(AT_PDMG_DEC_RATE_2ND, iPhy)
	InfoLog(9, "Effect_Begin102000701 after:DEC:".. unit:GetAbil(AT_PDMG_DEC_RATE_2ND))
	local iMagic = kEffect:GetAbil(AT_MAGIC_DMG_DEC)
	InfoLog(9, "Effect_Begin102000701 before:MAGIC:".. iMagic .. ", DEC:" .. unit:GetAbil(AT_MDMG_DEC_RATE_2ND))	
	unit:AddAbil(AT_MDMG_DEC_RATE_2ND, iMagic)
	InfoLog(9, "Effect_Begin102000701 after:MAGIC:".. unit:GetAbil(AT_MDMG_DEC_RATE_2ND))	
	local iMana = kEffect:GetAbil(AT_DMG_CONSUME_MP)
	unit:AddAbil(AT_DMG_CONSUME_MP, iMana)
	InfoLog(9, "Effect_Begin102000701 after:AT_DMG_CONSUME_MP:".. unit:GetAbil(AT_DMG_CONSUME_MP))
	unit:SetAbil(AT_DMG_DEC_RATE_COUNT, -1)	-- 데미지 감소 효과 회수 무제한
	
	local kActarg = NewActArg()
	kActarg:SetInt(ACT_ARG_CUSTOMDATA1, AT_PDMG_DEC_RATE_2ND)
	kActarg:SetInt(ACT_ARG_CUSTOMDATA1+1, AT_MDMG_DEC_RATE_2ND)
	kActarg:SetInt(ACT_ARG_CUSTOMDATA1+2, AT_DMG_CONSUME_MP)
	kActarg:SetInt(ACT_ARG_CUSTOMDATA1+3, AT_DMG_DEC_RATE_COUNT)
	unit:SendAbiles(4, kActarg, E_SENDTYPE_BROADALL)
	DeleteActArg(kActarg)
	
	return 1
end

function Effect_End102000701(unit, iEffectNo, actarg)
	local kEffectMgr = unit:GetEffectMgr()
	local kEffect = kEffectMgr:GetEffect(iEffectNo, false)
	if kEffect:IsNil() == true then
		InfoLog(5, "Effect_End102000701 GetEffect(" .. iEffectNo ..")");
		return 0
	end

	local iPhy = kEffect:GetAbil(AT_PHY_DMG_DEC)
	InfoLog(9, "Effect_End102000701 before:PHY:".. iPhy .. ", DEC:" ..unit:GetAbil(AT_PDMG_DEC_RATE_2ND))
	unit:AddAbil(AT_PDMG_DEC_RATE_2ND, 0-iPhy)
	InfoLog(9, "Effect_End102000701 after:DEC:".. unit:GetAbil(AT_PDMG_DEC_RATE_2ND))
	local iMagic = kEffect:GetAbil(AT_MAGIC_DMG_DEC)
	InfoLog(9, "Effect_End102000701 before:MAGIC:".. iMagic .. ", DEC:" .. unit:GetAbil(AT_MDMG_DEC_RATE_2ND))	
	unit:AddAbil(AT_MDMG_DEC_RATE_2ND, 0-iMagic)
	InfoLog(9, "Effect_End102000701 after:MAGIC:".. unit:GetAbil(AT_MDMG_DEC_RATE_2ND))	
	local iMana = kEffect:GetAbil(AT_DMG_CONSUME_MP)
	unit:AddAbil(AT_DMG_CONSUME_MP, 0-iMana)
	InfoLog(9, "Effect_End102000701 after:AT_DMG_CONSUME_MP:".. unit:GetAbil(AT_DMG_CONSUME_MP))	
	unit:SetAbil(AT_DMG_DEC_RATE_COUNT, 0)
	
	local kActarg = NewActArg()
	kActarg:SetInt(ACT_ARG_CUSTOMDATA1, AT_PDMG_DEC_RATE_2ND)
	kActarg:SetInt(ACT_ARG_CUSTOMDATA1+1, AT_MDMG_DEC_RATE_2ND)
	kActarg:SetInt(ACT_ARG_CUSTOMDATA1+2, AT_DMG_CONSUME_MP)
	kActarg:SetInt(ACT_ARG_CUSTOMDATA1+3, AT_DMG_DEC_RATE_COUNT)
	unit:SendAbiles(4, kActarg, E_SENDTYPE_BROADALL)
	DeleteActArg(kActarg)
	
	return 1
end

function Effect_Tick102000701(unit, elapsedtime, effect, actarg)
	--InfoLog(6, "Don't call me Effect_Tick102000701 Effect=" .. effect:GetEffectNo())
	if unit:GetAbil(AT_MP) == 0 then
		return ECT_MUSTDELETE
	end
	return ECT_DOTICK
end

