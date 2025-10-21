
-- 보쓰 
function Skill_Begin900000031(caster, skillnum, iStatus,arg)
	return Skill_BeginDefault(caster, skillnum, iStatus,arg)
end

function Skill_Fire900000031(caster, kTargetArray, skillnum, result, arg)
	--InfoLog(9, "Skill_Fire900000031 -.-")
	--GetSkillResult102000301(caster, kTargetArray, skillnum, result)
	local kSkillDef = GetSkillDef(skillnum)
	if kSkillDef:IsNil() then
		InfoLog(5, "Skill_Fire102000301....SkillDef is Nil" ..skillnum)
		return -1
	end
	
	--InfoLog(9, "Skill_Fire900000031 1.0")
	local kGround = arg:Get(ACTARG_GROUND)
	kGround = Ground(kGround)
	kUnitArray = NewUNIT_PTR_ARRAY()
	local iDetectRange = 60
	
	--InfoLog(9, "Skill_Fire900000031 1.1")
	local kTopLeft = kGround:GetNodePosition("pt_blaze_01")		-- Top Left
	local kTopRight = kGround:GetNodePosition("pt_blaze_02")	-- Top Right
	local kBottomLeft = kGround:GetNodePosition("pt_blaze_03")	-- Bottom Left
	local kBottomRight = kGround:GetNodePosition("pt_blaze_04")	-- Bottm Right

	--InfoLog(9, "Skill_Fire900000031 1.2")
	local kUpperBase = kTopRight:Subtract(kTopLeft)
	local kRightBase = kTopRight:Subtract(kBottomRight)
	local kBottomBase = kBottomRight:Subtract(kBottomLeft)
	local kLeftBase = kTopLeft:Subtract(kBottomLeft)

	--InfoLog(9, "Skill_Fire900000031 2.0")
	local akBasePos = {kTopLeft, kBottomRight, kBottomLeft, kBottomLeft}
	local akAlpha = {kUpperBase, kRightBase, kBottomBase, kLeftBase}

	local iIndex = 0
	local iNbLine = arg:GetInt(ACT_ARG_CUSTOMDATA1)
	while iIndex < iNbLine do
		--InfoLog(9, "Skill_Fire900000031 2.1")
		local fMetaForStart = arg:GetInt(ACT_ARG_CUSTOMDATA1 + iIndex * 2 + 1) / 1000
		local fMetaForEnd = arg:GetInt(ACT_ARG_CUSTOMDATA1 + iIndex * 2 + 2) / 1000
	
		--InfoLog(9, "Skill_Fire900000031 2.2")
		local kStartPos = akAlpha[math.floor(fMetaForStart)]:Multiply(fMetaForStart - math.floor(fMetaForStart))
		kStartPos = kStartPos:Add(akBasePos[math.floor(fMetaForStart)])
		kStartPos:SetZ(0)

		--InfoLog(9, "Skill_Fire900000031 2.3")
		local kEndPos = akAlpha[math.floor(fMetaForEnd)]:Multiply(fMetaForEnd - math.floor(fMetaForEnd))
		kEndPos = kEndPos:Add(akBasePos[math.floor(fMetaForEnd)])
		kEndPos:SetZ(0)

		--InfoLog(9, "Skill_Fire900000031 2.4")
		kGround:GetUnitInWidthFromLine(kStartPos, kEndPos, iDetectRange, UT_PLAYER, kUnitArray)

		iIndex = iIndex + 1
	end
	
	CS_GetSkillResultDefault(skillnum, caster, kUnitArray, result)

	iIndex = 0
	local target = kUnitArray:GetUnit(iIndex)
	while target:IsNil() == false do
		--InfoLog(9, "Skill_Fire102000301 3.0")
		if caster:IsEnemy(target, false) then	-- Enemy에게만 적용시킨다.
			--InfoLog(9, "Skill_Fire102000301 3.1")
			local aresult = result:GetResult(target:GetGuid())
			if aresult:IsNil() == false and aresult:GetInvalid() == false then
				--InfoLog(9, "Skill_Fire102000301 3.2")
				DoFinalDamage(caster, target, aresult:GetValue(), skillnum, arg)
			end
		end
		
		iIndex = iIndex + 1
		target = kUnitArray:GetUnit(iIndex)
		--InfoLog(9, "Skill_Fire102000301 3.0 [" .. iIndex .. "]")
	end
	
	DeleteUNIT_PTR_ARRAY(kUnitArray)
end

function Skill_Fail900000031(caster, kTargetArray, skillnum, result, arg)
	return false
end
