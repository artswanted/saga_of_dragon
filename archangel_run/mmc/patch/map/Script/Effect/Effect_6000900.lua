
-- lavalon(BOSS) -- Lavalon(BOSS) lv_lavalon_breath_c
function Effect_Begin6000900(lavalon, iEffectNo, actarg)
	local kEffectMgr = lavalon:GetEffectMgr()
	local kEffect = kEffectMgr:GetEffect(iEffectNo, false)
	if kEffect:IsNil() == true then
		InfoLog(5, "Effect_Begin" .. iEffectNo .. "Cannot GetEffect")
		return 0
	end

	return 1
end

function Effect_End6000900(lavalon, iEffectNo, actarg)
	local kEffectMgr = lavalon:GetEffectMgr()
	local kEffect = kEffectMgr:GetEffect(iEffectNo, false)
	if kEffect:IsNil() == true then
		InfoLog(5, "Effect_Begin" .. iEffectNo .. "Cannot GetEffect")
		return 0
	end
	
	return 1
end
	
function Effect_Tick6000900( lavalon, elapsedtime, effect, actarg)
	
	local kGround = actarg:Get(ACTARG_GROUND)
	kGround = Ground(kGround)
	if kGround:IsNil() then
		InfoLog(5,"EffectTick 6000900 Ground Nil")
	end
	
	local iDetactRange = 120
	local fMetaForStart = 0.5
	local fMetaForEnd = 0.5
	if 6000902 == effect:GetEffectNo() then	-- 왼쪽
		fMetaForStart = 0.45
		fMetaForEnd = 0.17
	elseif 6000901 == effect:GetEffectNo() then -- 오른쪽
		fMetaForStart = 0.55
		fMetaForEnd = 0.83
	end
	
	--InfoLog(9, "Skill_Fire900000001 1.1")
	local kTopLeft = kGround:GetNodePosition("pt_blaze_01")		-- Top Left
	local kTopRight = kGround:GetNodePosition("pt_blaze_02")	-- Top Right
	local kBottomLeft = kGround:GetNodePosition("pt_blaze_03")	-- Bottom Left
	local kBottomRight = kGround:GetNodePosition("pt_blaze_04")	-- Bottm Right
	
	--InfoLog(9, "Skill_Fire900000001 1.2")
	local kUpperBase = kTopRight:Subtract(kTopLeft)
	local kRightBase = kTopRight:Subtract(kBottomRight)
	local kBottomBase = kBottomRight:Subtract(kBottomLeft)
	local kLeftBase = kTopLeft:Subtract(kBottomLeft)
	
	--InfoLog(9, "Skill_Fire900000001 2.0")
	local akBasePos = {kTopLeft, kBottomRight, kBottomLeft, kBottomLeft}
	local akAlpha = {kUpperBase, kRightBase, kBottomBase, kLeftBase}
		
	--InfoLog(9, "Skill_Fire900000001 2.2")
	local kStartPos = kUpperBase:Multiply(fMetaForStart)
	kStartPos = kStartPos:Add(kTopLeft)
	kStartPos:SetZ(0)
	
	--InfoLog(9, "Skill_Fire900000001 2.3")
	local kEndPos = kBottomBase:Multiply(fMetaForEnd)
	kEndPos = kEndPos:Add(kBottomLeft)
	kEndPos:SetZ(0)
	
	local kTargetUnitArray = NewUNIT_PTR_ARRAY()
	kGround:GetUnitInWidthFromLine(kStartPos, kEndPos, iDetactRange, UT_PLAYER, kTargetUnitArray)
	
	local result = NewActionResultVector()
	local bReturn = GetSkillResult6000900(lavalon, kTargetUnitArray, iSkillNo, result)
	local iIndex = 0
	local target = kTargetUnitArray:GetUnit(iIndex)
	while target:IsNil() == false do
		--InfoLog(8, "Skill_Fire1  10")
		local aresult = result:GetResult(target:GetGuid())
		if aresult:IsNil() == false and aresult:GetInvalid() == false then
			--InfoLog(8, "Skill_Fire1  Damaged:" .. aresult:GetValue())
			DoFinalDamage(lavalon,target,aresult:GetValue(),iSkillNo,actarg)
		end
		
		iIndex = iIndex + 1
		target = kTargetUnitArray:GetUnit(iIndex)
	end
	
	DeleteActionResultVector(result)
	DeleteUNIT_PTR_ARRAY(kTargetUnitArray)
end
