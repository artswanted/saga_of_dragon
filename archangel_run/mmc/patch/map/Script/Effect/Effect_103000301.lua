
-- Archer (언니 이뻐? : Beautiful Girl) : Level 1-5
function Effect_Begin103000301(unit, iEffectNo, actarg)

	return 1
end

function Effect_End103000301(unit, iEffectNo, actarg)

	return 1
end

function Effect_Tick103000301(unit, elapsedtime, effect, actarg)
	local kGround = actarg:Get(ACTARG_GROUND)
	kGround = Ground(kGround)
	
	local kPos = unit:GetPos()
	local kUnitArray = NewUNIT_PTR_ARRAY()
	kGround:GetUnitInRangeZ(kPos, effect:GetAbil(AT_DETECT_RANGE), 30, UT_MONSTER, kUnitArray)
	local iIndex = 0
	local kMonster = kUnitArray:GetUnit(iIndex)
	while kMonster:IsNil() == false do
		kMonster:ClearTargetList()
		kMonster:SetTarget(unit:GetGuid())

		iIndex = iIndex + 1
		kMonster = kUnitArray:GetUnit(iIndex)
	end	-- While
	DeleteUNIT_PTR_ARRAY(kUnitArray)
	return ECT_DOTICK
end

