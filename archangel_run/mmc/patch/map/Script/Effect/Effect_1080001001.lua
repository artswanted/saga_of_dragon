	
-- 배틀메이지 (블리자드 공격 : a_Blizzard_Attack) : Level 1-5
function Effect_Begin1080001001(unit, iEffectNo, actarg)
	local kEffectMgr = unit:GetEffectMgr()
	local kEffect = kEffectMgr:GetEffect(iEffectNo, false)
	if kEffect:IsNil() == true then
		InfoLog(5, "Effect_Begin1080001001...Cannot GetEffect(" .. iEffectNo ..")");
		return 0
	end
	-- 동빙시키기
	local iAdd = kEffect:GetAbil(AT_R_MOVESPEED)
	kEffectMgr:AddAbil(AT_R_MOVESPEED, iAdd)
	unit:NftChangedAbil(AT_R_MOVESPEED, E_SENDTYPE_BROADALL+E_SENDTYPE_EFFECTABIL)
	return 1
end

function Effect_End1080001001(unit, iEffectNo, actarg)
	local kEffectMgr = unit:GetEffectMgr()
	local kEffect = kEffectMgr:GetEffect(iEffectNo, false)
	if kEffect:IsNil() == true then
		InfoLog(5, "Effect_End1080001001...Cannot GetEffect(" .. iEffectNo ..")");
		return 0
	end
	-- 동빙시키기
	local iAdd = kEffect:GetAbil(AT_R_MOVESPEED)
	kEffectMgr:AddAbil(AT_R_MOVESPEED, 0-iAdd)
	unit:NftChangedAbil(AT_R_MOVESPEED, E_SENDTYPE_BROADALL+E_SENDTYPE_EFFECTABIL)
	return 1
end

function Effect_Tick1080001001(unit, elapsedtime, effect, actarg)
	InfoLog(6, "Don't call me Effect_Tick1080001001 Effect=" .. effect:GetEffectNo())
	--[[InfoLog(5, "Effect_Tick1080001001");
	local kGround = actarg:Get(ACTARG_GROUND)
	kGround = Ground(kGround)
	
	local kPos = unit:GetPos()
	local kUnitArray = NewUNIT_PTR_ARRAY()
	
	local iRange = effect:GetAbil(AT_DETECT_RANGE)
	kGround:GetUnitInRangeZ(kPos, iRange, 30, UT_MONSTER, kUnitArray)
	kGround:GetUnitInRangeZ(kPos, iRange, 30, UT_PLAYER, kUnitArray)
	
	local iDamage = unit:GetAbil( AT_ATTR_ATTACK )
	local caster = kGround:GetUnit( unit:GetCaller() )
	if caster:IsNil() == true then
		InfoLog(5, "Effect_Tick1080001001-2");
		--unit:SetAbil(AT_HP, 0)
		unit:SetHP(0, E_SENDTYPE_NONE, effect:GetCaster())
		return ECT_DOTICK
	end
	
	local iCount = 0
	local target = kUnitArray:GetUnit(iCount)
	while target:IsNil() == false do
		InfoLog(5, "Effect_Tick1080001001-3");
		if caster:IsEnemy(target, false) == true then
			InfoLog(5, "Effect_Tick1080001001-4");
			DoFinalDamage( caster, target, iDamage, 0, actarg )
			target:AddEffect(effect:GetAbil(AT_EFFECTNUM1), 0, arg, unit)
		end
		
		iCount = iCount + 1
		if ( iCount > 5 )	then break end
		
		target = kUnitArray:GetUnit(iCount)
	end -- while
	
	InfoLog(5, "Effect_Tick1080001001-5");
	DeleteUNIT_PTR_ARRAY(kUnitArray)]]--
	return ECT_DOTICK
end

