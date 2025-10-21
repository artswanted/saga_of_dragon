
-- Wizard ( HP 리스토어: a_HP Restore) : Level 1-10
function Effect_Begin109001601(unit, iEffectNo, actarg)
	local kGround = actarg:Get(ACTARG_GROUND)
	kGround = Ground(kGround)
	if kGround:IsNil() then
		InfoLog(5, "Effect_Begin109001601 Ground is NILL")
		return 0
	end

	local kEffectMgr = unit:GetEffectMgr()
	local kEffect = kEffectMgr:GetEffect(iEffectNo, false)

	local caster = kGround:GetUnit(kEffect:GetCaster())

	if caster:IsNil() then
		InfoLog(5, "Effect_Begin109001601 Caster is NILL")
		return 0
	end

	local iMagicPower = caster:GetAbil(AT_C_MAGIC_ATTACK)
	iMagicPower = iMagicPower * effect:GetAbil(AT_R_MAGIC_ATTACK_MAX) / ABILITY_RATE_VALUE;
	
	local iMaxHP = unit:GetAbil(AT_C_MAX_HP)
	local iHP = unit:GetAbil(AT_HP)
	if iHP == iMaxHP then
		return 0
	end
	
	local iNewHP = math.min(iMaxHP, iHP+iMagicPower)

	unit:SetHP(iNewHP, E_SENDTYPE_BROADALL+E_SENDTYPE_EFFECTABIL, effect:GetCaster());
	unit:NftChangedAbil(AT_HP, E_SENDTYPE_BROADALL+E_SENDTYPE_EFFECTABIL);
	
	return 1
end
                                                                                                                          
function Effect_End109001601(unit, iEffectNo, actarg)
	return 1
end

function Effect_Tick109001601(unit, elapsedtime, effect, actarg)
	--InfoLog(5, "Effect_Tick109001601 1")
	local kGround = actarg:Get(ACTARG_GROUND)
	if kGround == nil then
		InfoLog(5, "Effect_Tick109001601 Ground is NILL")
		return ECT_MUSTDELETE
	end

	kGround = Ground(kGround)
	if kGround:IsNil() then
		InfoLog(5, "Effect_Tick109001601 Ground is NILL")
		return ECT_MUSTDELETE
	end

	--InfoLog(5, "Effect_Tick109001601 2")
	local caster = kGround:GetUnit(effect:GetCaster())

	if caster:IsNil() then
		InfoLog(5, "Effect_Tick109001601 Caster is NILL")
		return ECT_MUSTDELETE
	end
	
	--InfoLog(5, "Effect_Tick109001601 3")
	local iMagicPower = caster:GetAbil(AT_C_MAGIC_ATTACK)
	iMagicPower = iMagicPower * effect:GetAbil(AT_R_MAGIC_ATTACK_MAX) / ABILITY_RATE_VALUE;
	
	--InfoLog(5, "Effect_Tick109001601 4")
	local iMaxHP = unit:GetAbil(AT_C_MAX_HP)
	local iHP = unit:GetAbil(AT_HP)
	if iHP == iMaxHP then
		return ECT_MUSTDELETE
	end
	
	--InfoLog(5, "Effect_Tick109001601 5 iMaxHP : "..iMaxHP.." iHP+iMagicPower : "..iHP+iMagicPower)
	local iNewHP = math.min(iMaxHP, iHP+iMagicPower)

	--InfoLog(5, "Effect_Tick109001601 5-1 iNewHP : "..iNewHP)

	unit:SetHP(iNewHP, E_SENDTYPE_BROADALL+E_SENDTYPE_EFFECTABIL, effect:GetCaster());
	--InfoLog(5, "Effect_Tick109001601 6")
	
	return ECT_DOTICK
end
