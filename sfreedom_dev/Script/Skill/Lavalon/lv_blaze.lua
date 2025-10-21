local iExplosionParticleTargetRadius = 79
local iExplosionParticleRadius = 100--110
local iExplosionTerm = 1

function Act_LAVALON_Blaze_OnEnter(actor, action)
	local kPacket = action:GetParamAsPacket()
	if kPacket:IsNil() then
		return true
	end

	if( CheckNil(g_world == nil) ) then return false end
	if( CheckNil(g_world:IsNil()) ) then return false end
	local kTopLeft = g_world:GetObjectPosByName("pt_blaze_01")		-- Top Left
	local kTopRight = g_world:GetObjectPosByName("pt_blaze_02")	-- Top Right
	local kBottomLeft = g_world:GetObjectPosByName("pt_blaze_03")	-- Bottom Left
	local kBottomRight = g_world:GetObjectPosByName("pt_blaze_04")	-- Bottm Right

	--				1
	--     	---------------------
	--     	|					|
	--	4	|					|    2
	--		|					|
	--		|					|
	--		|					|
	--     	---------------------
	--				3

	local kUpperBase = kTopRight:_Subtract(kTopLeft)
	local kRightBase = kTopRight:_Subtract(kBottomRight)
	local kBottomBase = kBottomRight:_Subtract(kBottomLeft)
	local kLeftBase = kTopLeft:_Subtract(kBottomLeft)

	local akBasePos = {kTopLeft, kBottomRight, kBottomLeft, kBottomLeft}
	local akAlpha = {kUpperBase, kRightBase, kBottomBase, kLeftBase}
	
	local iNbLine = kPacket:PopInt()
	--ODS("________________NbLine ___________ : " ..  iNbLine .."\n")
--	local iNbLine = 3
	action:SetParamInt(0, iNbLine)
	local iLineIndex = 0

	local iCount = 1
	while iNbLine > iLineIndex do
		local fMetaForStart = kPacket:PopFloat()
		local fMetaForEnd = kPacket:PopFloat()
		--ODS("________________Start___________ : " ..  fMetaForStart .."\n")
		--ODS("________________End ___________ : " ..  fMetaForEnd .."\n")
		--local fMetaForStart = math.random() + math.random(1, 2) 
		--local fMetaForEnd = math.random() + math.random(3, 4)

		local kStartPos = akAlpha[math.floor(fMetaForStart)]:_Multiply(fMetaForStart - math.floor(fMetaForStart))
		kStartPos = kStartPos:_Add(akBasePos[math.floor(fMetaForStart)])
		kStartPos:SetZ(0)

		local kEndPos = akAlpha[math.floor(fMetaForEnd)]:_Multiply(fMetaForEnd - math.floor(fMetaForEnd))
		kEndPos = kEndPos:_Add(akBasePos[math.floor(fMetaForEnd)])
		kEndPos:SetZ(0)

		action:SetParamAsPoint(iLineIndex * 2 + 1, kStartPos)
		action:SetParamAsPoint(iLineIndex * 2 + 2, kEndPos)
		
		local kWarnVector = kEndPos:_Subtract(kStartPos)
		local fIncValue = iExplosionParticleTargetRadius / kWarnVector:Length()

		local kProgress = 0
		while kProgress < 1.0 do
			local kWarnPos = kWarnVector:_Multiply(kProgress)
			kWarnPos:Add(kStartPos)
			kWarnPos:SetZ(kWarnPos:GetZ() + 100)
			
			local kFloorPos = g_world:ThrowRay(kWarnPos, Point3(0,0,-1), 200)
			if -1 ~= kFloorPos:GetX() and -1 ~= kFloorPos:GetY() and -1 ~= kFloorPos:GetZ() then
				GetPlayer():AttachParticleToPointS(90000 + iCount, kFloorPos, "e_ef_fossilearth_target_01", 1.0)
			end
			kProgress = kProgress + fIncValue
			iCount = iCount + 1
		end
		
		
		iLineIndex = iLineIndex + 1
	end

	action:DeleteParamPacket()

	return true
end

function Act_LAVALON_Blaze_OnUpdate(actor, accumTime, frameTime)
	if actor == nil or actor:IsNil() then
		return false
	end

	actor:SeeFront(true, true)

	local action = actor:GetAction()		-- 지금 엑션 객체를 반환
	local actionParam = action:GetActionParam()
	local curSlot = action:GetCurrentSlot()	-- 엑션이 플레이하고 있는 에니 순번
	local curStage = action:GetCurrentStage()

	local iNbExp = action:GetParamInt(0)
	if actor:IsAnimationDone() == true then
		if curSlot == 0 then
			actor:PlayNext()
		elseif curSlot == 2 and
			curStage > iNbExp and
			action:GetParam(219857) == "TheEnd" then
			-- 끝나는 조건.
			return false
		end
	elseif actionParam == AP_CASTING and
		curSlot == 1 then
		return true
	end

	-- 화염 폭발 발사 조건.
	if curStage >= 1 and actionParam == AP_FIRE then

		-- 순차적으로 터져주면 된다.
		-- 얼마의 시간 차이로 터질 것인가!
		local kLastExplosionTime = action:GetParamInt(982389);
		local iNbExp = action:GetParamInt(0)
		local iNbIdx = 0

		local iStartParticleIDX = action:GetParamInt(90000)
		if kLastExplosionTime == 0 or
			iExplosionTerm < math.floor(accumTime * 1000 - kLastExplosionTime) then
	
			if( CheckNil(g_world == nil) ) then return false end
			if( CheckNil(g_world:IsNil()) ) then return false end
			while iNbIdx < iNbExp and curStage <= iNbExp do
				local kStartPos = action:GetParamAsPoint(iNbIdx * 2 + 1)
				local kEndPos = action:GetParamAsPoint(iNbIdx * 2 + 2)

				local kProgress = 0.0
				while kProgress < 1.0 do
					local kExplosionVector = kEndPos:_Subtract(kStartPos)
					local kExpPos = kExplosionVector:_Multiply(kProgress)
					kExpPos:Add(kStartPos)
					kExpPos:SetZ(kExpPos:GetZ() + 100)
					
					local kFloorPos = g_world:ThrowRay(kExpPos, Point3(0,0,-1), 200)
					if -1 ~= kFloorPos:GetX() and -1 ~= kFloorPos:GetY() and -1 ~= kFloorPos:GetZ() then
						local Effect = actor:GetAnimationInfo("FIRE_EFFECT")
						if Effect == nil or Effect == "" then
							Effect = "e_ef_fossilearth_explosion"
						end

						if Effect ~= "NONE" then
							GetPlayer():AttachParticleToPointS(90000+iStartParticleIDX, kFloorPos, Effect, 1.0)
						end
					end
					
					kProgress = math.min(kProgress + iExplosionParticleRadius / kExplosionVector:Length(), 1.0)
					iStartParticleIDX = iStartParticleIDX + 1
				end
				action:NextStage()
				iNbIdx = iNbIdx + 1
			end
		
			action:SetParamInt(982389, math.floor(accumTime * 1000))

			if curStage > iNbExp then
				-- Action 끝내자.
				action:SetParam(219857 , "TheEnd")
			
				local kTargetList = action:GetTargetList()
				kTargetList:ApplyActionEffects()
			end
		end
		action:SetParamInt(90000, iStartParticleIDX)
	end

	return true
end
function Act_LAVALON_Blaze_OnCleanUp(actor, action)
	return true
end

function Act_LAVALON_Blaze_OnLeave(actor, action)
	return true
end


function Act_LAVALON_Blaze_OnCastingCompleted(actor, action)
	--ODS("____________Lavalon Blaze OnCastingComplete \n")
	action:SetSlot(2)
	actor:PlayCurrentSlot()
	return true
end

function Act_LAVALON_Blaze_OnEvent(actor, textKey, seqID)

	if textKey == "hit" then
		--ODS("____________Lavalon Blaze OnEvent : hit " .. textKey .. "\n")
		local kAction = actor:GetAction()
		kAction:NextStage()
		QuakeCamera(1,1,1,1,1)
	end

	return	true;
end

function Act_LAVALON_Blaze_OnTargetListModified(actor,action,bIsBefore)
	Act_Melee_Default_OnTargetListModified(actor,action,bIsBefore)
end
