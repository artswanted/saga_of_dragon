-- 베틀메이지[전투마법사] (블리자드 : Blizzard) : Level 1-10
function Effect_Begin108000101(unit, iEffectNo, actarg)

	return 1
end

function Effect_End108000101(unit, iEffectNo, actarg)

	return 1
end

function Effect_Tick108000101(unit, elapsedtime, effect, actarg)
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
		--unit:SetAbil(AT_HP, 0)
		unit:SetHP(0, E_SENDTYPE_NONE, effect:GetCaster())
		return ECT_DOTICK
	end
	
	local iCount = 0
	local target = kUnitArray:GetUnit(iCount)
	while target:IsNil() == false do
		if caster:IsEnemy(target, false) == true then
			DoFinalDamage( caster, target, iDamage, 0, actarg )
			target:AddEffect(effect:GetAbil(AT_EFFECTNUM1), 0, arg, unit)
		end
		
		iCount = iCount + 1
		if ( iCount > 5 )	then break end
		
		target = kUnitArray:GetUnit(iCount)
	end -- while
	
	DeleteUNIT_PTR_ARRAY(kUnitArray)
	return ECT_DOTICK
end
