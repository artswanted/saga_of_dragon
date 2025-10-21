-- 배틀메이지 (파이어 엠블렘 : a_Fire Emblem) : Level 1-5
function Effect_Begin109000901(unit, iEffectNo, actarg)

	return 1
end

function Effect_End109000901(unit, iEffectNo, actarg)
	
	return 1
end

function Effect_Tick109000901(unit, elapsedtime, effect, actarg)
	local kGround = actarg:Get(ACTARG_GROUND)
	kGround = Ground(kGround)
	
	local kPos = unit:GetPos()
	local kUnitArray = NewUNIT_PTR_ARRAY()
	
	local iRange = effect:GetAbil(AT_DETECT_RANGE)
	kGround:GetUnitInRangeZ(kPos, iRange, 30, UT_MONSTER, kUnitArray)
	kGround:GetUnitInRangeZ(kPos, iRange, 30, UT_PLAYER, kUnitArray)
	
	--공격력은 설치자의 마법공격력 + 지뢰공격력
	local iDamage = unit:GetAbil( AT_ATTR_ATTACK ) + effect:GetAbil(AT_PHY_ATTACK_MAX)
	local caster = kGround:GetUnit( unit:GetCaller() )
	if caster:IsNil() == true then
		--unit:SetAbil(AT_HP, 0)
		unit:SetHP(0, E_SENDTYPE_NONE, effect:GetCaster())
		return ECT_DOTICK
	end
	
	local iCount = 0
	local target = kUnitArray:GetUnit(iCount)
	local bFired = false
	while target:IsNil() == false do
		if caster:IsEnemy(target, false) == true then
			DoFinalDamage( caster, target, iDamage, 0, actarg )
			bFired = true
		end
		
		iCount = iCount + 1
		if ( iCount > 5 )	then break end
		
		target = kUnitArray:GetUnit(iCount)
	end -- while
	
	DeleteUNIT_PTR_ARRAY(kUnitArray)
	if bFired == true then
		return ECT_MUSTDELETE	-- 지뢰가 터졌으므로 삭제되어야 한다.
		-- unit(지뢰개체)는 언제 삭제되어야 하나?
		-- Life Time이란것이 있으니, 그것을 기대해 봐도 될것 같기는 한데 흠~
	end
	return ECT_DOTICK
end
