
-- Lavalon(BOSS) lv_lavalon_breath_c,l,r
function Skill_Begin6033201(caster, skillnum, iStatus,arg,kTailPacket)
	
	local kGround = Ground(arg:Get(ACTARG_GROUND))

	-- Meteor가 뿌려질 좌표..
	local kTopLeft = kGround:GetNodePosition("pt_blaze_01")		-- Top Left
	local kTopRight = kGround:GetNodePosition("pt_blaze_02")	-- Top Right
	local kBottomLeft = kGround:GetNodePosition("pt_blaze_03")	-- Bottom Left
	local kBottomRight = kGround:GetNodePosition("pt_blaze_04")	-- Bottm Right

	-- 위 좌표는 반드시 의미에 부합해야 한다.
	local firstPos = kTopRight:Subtract(kTopLeft)
	firstPos = firstPos:Multiply(math.random())
	firstPos = firstPos:Add(kTopLeft)

	local secondPos = kBottomRight:Subtract(kBottomLeft)
	secondPos = secondPos:Multiply(math.random())
	secondPos = secondPos:Add(kBottomLeft)

	local finalPos = firstPos:Subtract(secondPos)
	finalPos = finalPos:Multiply(math.random())
	finalPos = finalPos:Add(secondPos)
		
	caster:SetAbil(AT_SKILL_CUSTOM_DATA_01, finalPos:GetX())
	caster:SetAbil(AT_SKILL_CUSTOM_DATA_01+1, finalPos:GetY())
	caster:SetAbil(AT_SKILL_CUSTOM_DATA_01+2, finalPos:GetZ())

	kTailPacket:PushInt(finalPos:GetX())
	kTailPacket:PushInt(finalPos:GetY())
	kTailPacket:PushInt(finalPos:GetZ())
	kTailPacket:PushInt(0)
	
	return true
--	return Skill_BeginDefault(caster, skillnum, iStatus,arg)
end

function Skill_Fire6033201(caster, kTargetArray, skillnum, result, arg)

	local kSkillDef = GetSkillDef(skillnum)
	if kSkillDef:IsNil() then
		InfoLog(5, "Skill_Fire6033201....SkillDef is Nil " ..skillnum)
		return -1
	end
	
	local kGround = Ground(arg:Get(ACTARG_GROUND))
	local kPosX = caster:GetAbil(AT_SKILL_CUSTOM_DATA_01)
	local kPosY = caster:GetAbil(AT_SKILL_CUSTOM_DATA_01+1)
	local kPosZ = caster:GetAbil(AT_SKILL_CUSTOM_DATA_01+2)

	local kCollisionPos = Point3(kPosX, kPosY, kPosZ)
	kCollisionPos:SetZ(0)
		
	kUnitArray = NewUNIT_PTR_ARRAY()
	
	kGround:GetUnitInRange(kCollisionPos,55,UT_PLAYER,kUnitArray)
	
	CS_GetSkillResultDefault(skillnum, caster, kUnitArray, result)
	
	iIndex = 0
	local target = kUnitArray:GetUnit(iIndex)
	
	while target:IsNil() == false do

		if caster:IsEnemy(target, false) then	-- Enemy에게만 적용시킨다.
		
			local aresult = result:GetResult(target:GetGuid())
			
			if aresult:IsNil() == false and aresult:GetInvalid() == false then
				DoFinalDamage(caster, target, aresult:GetValue(), skillnum, arg)
			end
		end
		
		iIndex = iIndex + 1
		target = kUnitArray:GetUnit(iIndex)
	end
	
	DeleteUNIT_PTR_ARRAY(kUnitArray)
	
	return true

end

function Skill_CanFire6033201(caster, kTargetArray, skillnum, actarg)
	return true
end

function Skill_CanReserve6033201(caster, kTargetArray, skillnum, actarg)
	return Skill_CanReserveDefault(caster, kTargetArray, skillnum, actarg)
end

function Skill_Fail6033201(caster, kTargetArray, skillnum, result, arg)
	return false
end
