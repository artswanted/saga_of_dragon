local iExplosionParticleTargetRadius = 79
local iExplosionParticleRadius = 100--110
local iExplosionTerm = 1

function Act_Mon_DarkBreath_SetState(actor,action,kState)
	
	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	actor:FreeMove(true)
	--ODS("State : "..kState.."\n",false,987)
	local	iNewState = -1;
	if kState == "START" then
	
		if actor:GetAnimationLength(action:GetSlotAnimName(0)) == 0 then
			return	false	
		end
				
		action:SetSlot(0)
		iNewState = 0
	
	elseif kState == "IDLE" then
		
		action:SetSlot(1)
		if actor:GetAnimationLength(action:GetSlotAnimName(1)) == 0 then
			action:SetSlot(6);	--	배틀 Idle 모션이 없을 때는 그냥 Idle 모션을 하자.
		end
		iNewState = 1
		
	elseif kState == "RUN" then
		action:SetSlot(2)
		iNewState = 2
		
	elseif kState == "FIREREADY" then
		action:SetSlot(3)
		iNewState = 3
		
	elseif kState == "BATTLEIDLE" then
		action:SetSlot(4)
		iNewState = 4
	
	elseif kState == "FINISH" then
		action:SetSlot(5)
		iNewState = 5
	elseif kState == "END_IDLE" then
		action:SetSlot(6)
		iNewState = 6
	elseif kState == "FIRE" then
		action:SetSlot(7)
		iNewState = 7
	
		
	end
	
	actor:ResetAnimation();
	actor:PlayCurrentSlot();
	action:SetParamInt(0,iNewState)
	
	return	true;

end

function Act_Mon_DarkBreath_OnEnter(actor, action)
	local kPacket = action:GetParamAsPacket()
	if kPacket:IsNil() then
		return true
	end
	
	action:SetParamAsPoint(1, actor:GetPos())
	actor:FreeMove(true)
	
	actor:ResetAnimation()
	
	--g_world:SetCameraAdjustInfo(50,-300,50,0,0)
	
	if action:GetActionParam() == AP_FIRE then
		Act_Mon_DarkBreath_SetState(actor,action,"FIRE")
		--ODS("Act_Mon_DarkBreath_OnEnter FIRE\n",false,987)
	else
		if( Act_Mon_DarkBreath_SetState(actor,action,"START") == false) then
			Act_Mon_DarkBreath_SetState(actor,action,"IDLE")
		end	
	end

	if( CheckNil(g_world == nil) ) then return false end
	if( CheckNil(g_world:IsNil()) ) then return false end
	local kTopLeft = g_world:GetObjectPosByName("breath_L_top")		-- left top
	local kTopRight = g_world:GetObjectPosByName("breath_R_top")	-- right top
	local kBottomLeft = g_world:GetObjectPosByName("breath_L_bottom")	-- left bottom
	local kBottomRight = g_world:GetObjectPosByName("breath_R_bottom")	-- right bottom

	local kUpperBase = Point3((kTopRight:GetX()+kTopLeft:GetX())/2, (kTopRight:GetY()+kTopLeft:GetY())/2, (kTopRight:GetZ()+kTopLeft:GetZ())/2)
	local kRightBase = Point3((kTopRight:GetX()+kBottomRight:GetX())/2, (kTopRight:GetY()+kBottomRight:GetY())/2, (kTopRight:GetZ()+kBottomRight:GetZ())/2)
	local kBottomBase = Point3((kBottomRight:GetX()+kBottomLeft:GetX())/2, (kBottomRight:GetY()+kBottomLeft:GetY())/2, (kBottomRight:GetZ()+kBottomLeft:GetZ())/2)
	local kLeftBase = Point3((kTopLeft:GetX()+kBottomLeft:GetX())/2, (kTopLeft:GetY()+kBottomLeft:GetY())/2, (kTopLeft:GetZ()+kBottomLeft:GetZ())/2)
	
	local fHorLen =  kUpperBase:GetY() - kBottomBase:GetY()
	local fVerLen = kRightBase:GetX() - kLeftBase:GetX()
	
	local kStartPoint = Point3(kLeftBase:GetX(), kBottomBase:GetY(), kBottomBase:GetZ())

	action:SetParamAsPoint(111, kStartPoint)
	
	-- ODS("Pos X : "..kUpperBase:GetX().." Y : "..kUpperBase:GetY().." Z : "..kUpperBase:GetZ().."\n",false,987)
	-- ODS("Pos X : "..kBottomBase:GetX().." Y : "..kBottomBase:GetY().." Z : "..kBottomBase:GetZ().."\n",false,987)
	-- ODS("Pos X : "..kRightBase:GetX().." Y : "..kRightBase:GetY().." Z : "..kRightBase:GetZ().."\n",false,987)
	-- ODS("Pos X : "..kLeftBase:GetX().." Y : "..kLeftBase:GetY().." Z : "..kLeftBase:GetZ().."\n",false,987)
	
	local iHorMaxCount = 4
	local iVerMaxCount = 20
	
	local fLen = fHorLen / iHorMaxCount;
	local fHalfLen = fLen / 2;
	action:SetParamFloat(200, fLen)
	
	--ODS("a:"..fHorLen.." "..fVerLen.."\n",false,987)

	local iHor = kPacket:PopInt()
	ODS("iHor : "..iHor.."\n",false,987)
	--action:SetParamInt(100, iHor)
	for it=1,80 do
		action:SetParamInt(i, 0)
	end
	for ih=1,iHor do
		local iPos = kPacket:PopInt() - 1
		--ODS("iPos : "..iPos.."\n",false,987)
		for i=1,20 do
			action:SetParamInt(iPos*20 + i, 1)
		end
	end
	local iVer = kPacket:PopInt()
	--ODS("iVer : "..iVer.."\n",false,987)
	for iv=1,iVer do
		local iPos = kPacket:PopInt()
		--ODS("iPos : "..iPos.."\n",false,987)
		for i=0,3 do
			action:SetParamInt(i*20 + iPos, 1)
		end
	end
	
	action:DeleteParamPacket()

	return true
end

function Act_Mon_DarkBreath_OnUpdate(actor, accumTime, frameTime)
	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	local	action = actor:GetAction()
	CheckNil(nil==action);
	CheckNil(action:IsNil());
	if( CheckNil(g_world == nil) ) then return false end
	if( CheckNil(g_world:IsNil()) ) then return false end
	local	iState = action:GetParamInt(0)
	--ODS(iState.." \n",false,987)
	
	
	--if action:GetActionParam() == AP_FIRE then
		if iState == 7 then
			if actor:IsAnimationDone() then
				Act_Mon_DarkBreath_SetState(actor,action,"FINISH")
				actor:SetTranslate(action:GetParamAsPoint(1))
				--g_world:RecoverCameraAdjustInfo()
			end
		end
	--elseif action:GetActionParam() == AP_CASTING then
		if iState == 0 then
			if actor:IsAnimationDone() then
				--g_world:SetCameraAdjustInfo(143,-96,50,0,0)
				
				Act_Mon_DarkBreath_SetState(actor,action,"RUN")
				--g_world:SetParamAsPoint(112, actor:GetPos())
				
				action:SetParamFloat(88, g_world:GetAccumTime())
				
				-- local ptLookat = Point3(actor:GetPos():GetX()-2000, actor:GetPos():GetY(), actor:GetPos():GetZ())
				-- actor:LookAt(ptLookat)
				--ODS("Lookat x : "..ptLookat:GetX().." y : "..ptLookat:GetY().." z : "..ptLookat:GetZ().."\n",false,987)
			end

		elseif iState == 2 then
			local kMoveDir = Point3(-1,0,0)
			local fMoveSpeed = 2000
			local OldPos = action:GetParamAsPoint(112)
			local CurPos = actor:GetPos()
			
			-- ODS("Old "..OldPos:GetX().." Old "..OldPos:GetY().." Old "..OldPos:GetZ().."\n",false,987)
			-- ODS("Cur "..CurPos:GetX().." Cur "..CurPos:GetY().." Cur "..CurPos:GetZ().."\n",false,987)
			
			-- if CurPos:GetX() ~= OldPos:GetX() then
				kMoveDir:Multiply(fMoveSpeed)
				--kMoveDir:Add(Point3(0,0,100))
				actor:SetMovingDelta(kMoveDir)
			-- else
				-- CurPos:SetZ(CurPos:GetZ() + 500)
				-- actor:SetTranslate(CurPos,true)
			-- end
						
			--action:SetParamAsPoint(112, actor:GetPos())
			
			local fCurTime = g_world:GetAccumTime()
			if fCurTime - action:GetParamFloat(88) > 1.8 then
				local ptMiddle = g_world:GetObjectPosByName("fly_end")		-- left top
				actor:SetTranslate(ptMiddle)
				Act_Mon_DarkBreath_SetState(actor,action,"FIREREADY")
			end
		elseif iState == 3 then
			if actor:IsAnimationDone() then
				Act_Mon_DarkBreath_SetState(actor,action,"BATTLEIDLE")
			end
		elseif iState == 4 then
			-- local ptMiddle = g_world:GetObjectPosByName("fly_end")		-- left top
			-- actor:SetTranslate(ptMiddle)
		elseif iState == 5 then
			if actor:IsAnimationDone() then
				Act_Mon_DarkBreath_SetState(actor,action,"END_IDLE")
				--g_world:RecoverCameraAdjustInfo()
			end
		end

	--end

	-- actor:SeeFront(true, true)

	-- local action = actor:GetAction()		-- 지금 엑션 객체를 반환
	-- local actionParam = action:GetActionParam()
	-- local curSlot = action:GetCurrentSlot()	-- 엑션이 플레이하고 있는 에니 순번
	-- local curStage = action:GetCurrentStage()

	-- local iNbExp = action:GetParamInt(0)
	-- if actor:IsAnimationDone() == true then
		-- if curSlot == 0 then
			-- actor:PlayNext()
		-- elseif curSlot == 2 and
			-- curStage > iNbExp and
			-- action:GetParam(219857) == "TheEnd" then
			-- -- 끝나는 조건.
			-- return false
		-- end
	-- elseif actionParam == AP_CASTING and
		-- curSlot == 1 then
		-- return true
	-- end

	-- 화염 폭발 발사 조건.
	-- if curStage >= 1 and actionParam == AP_FIRE then

		-- -- 순차적으로 터져주면 된다.
		-- -- 얼마의 시간 차이로 터질 것인가!
		-- local kLastExplosionTime = action:GetParamInt(982389);
		-- local iNbExp = action:GetParamInt(0)
		-- local iNbIdx = 0

		-- local iStartParticleIDX = action:GetParamInt(90000)
		-- if kLastExplosionTime == 0 or
			-- iExplosionTerm < math.floor(accumTime * 1000 - kLastExplosionTime) then
			
			-- while iNbIdx < iNbExp and curStage <= iNbExp do
				-- local kStartPos = action:GetParamAsPoint(iNbIdx * 2 + 1)
				-- local kEndPos = action:GetParamAsPoint(iNbIdx * 2 + 2)

				-- local kProgress = 0.0
				-- while kProgress < 1.0 do
					-- local kExplosionVector = kEndPos:_Subtract(kStartPos)
					-- local kExpPos = kExplosionVector:_Multiply(kProgress)
					-- kExpPos:Add(kStartPos)
					-- kExpPos:SetZ(kExpPos:GetZ() + 100)
					
					-- local kFloorPos = g_world:ThrowRay(kExpPos, Point3(0,0,-1), 200)
					-- if -1 ~= kFloorPos:GetX() and -1 ~= kFloorPos:GetY() and -1 ~= kFloorPos:GetZ() then
						-- GetPlayer():AttachParticleToPointS(90000+iStartParticleIDX, kExpPos, "e_ef_fossilearth_explosion", 1.0)
					-- end
					
					-- kProgress = math.min(kProgress + iExplosionParticleRadius / kExplosionVector:Length(), 1.0)
					-- iStartParticleIDX = iStartParticleIDX + 1
				-- end
				-- action:NextStage()
				-- iNbIdx = iNbIdx + 1
			-- end
		
			-- action:SetParamInt(982389, math.floor(accumTime * 1000))

			-- if curStage > iNbExp then
				-- -- Action 끝내자.
				-- action:SetParam(219857 , "TheEnd")
			
				-- local kTargetList = action:GetTargetList()
				-- kTargetList:ApplyActionEffects()
			-- end
		-- end
		-- action:SetParamInt(90000, iStartParticleIDX)
	-- end

	return true
end
function Act_Mon_DarkBreath_OnCleanUp(actor, action)
	CheckNil(nil==actor);
	CheckNil(actor:IsNil());
	
	actor:FreeMove(false);
	--g_world:RecoverCameraAdjustInfo()
	return true
end

function Act_Mon_DarkBreath_OnLeave(actor, action)
	local	iState = action:GetParamInt(0)
	if iState == 5 then
		if actor:IsAnimationDone() then
			ODS("Act_Mon_DarkBreath_OnLeave\n",false,987)
			return true
		end
	end
	return false
end


function Act_Mon_DarkBreath_OnCastingCompleted(actor, action)
	ODS("Act_Mon_DarkBreath_OnCastingCompleted\n",false,987)
	Act_Mon_DarkBreath_SetState(actor, action, "FIRE")
	return true
end

function Act_Mon_DarkBreath_OnEvent(actor, textKey, seqID)
	--ODS("Key: "..textKey.."\n",false,987)
	if textKey == "set" then
		local	action = actor:GetAction()
		CheckNil(nil==action);
		CheckNil(action:IsNil());
		
		local iHorMaxCount = 4
		local iVerMaxCount = 20
		
		local fLen = action:GetParamFloat(200)
		local fHalfLen = fLen / 2
		
		local kStartPoint = action:GetParamAsPoint(111)
		
		for it=1,80 do
			local ibool = action:GetParamInt(it)
			if ibool == 1 then
				if( CheckNil(g_world == nil) ) then return false end
				if( CheckNil(g_world:IsNil()) ) then return false end
				local vPos = ((it-1) / iVerMaxCount) + 1
				local hPos = ((it-1) % iVerMaxCount) + 1
				local Offset = Point3(hPos * fLen - fHalfLen, vPos * fLen - fHalfLen, 0)
				local kPoint = Point3(kStartPoint:GetX(), kStartPoint:GetY(), kStartPoint:GetZ())
				kPoint:Add(Offset)
				local kWarnPos = Point3(kPoint:GetX(), kPoint:GetY(), kPoint:GetZ()+100)
				local kFloorPos = g_world:ThrowRay(kWarnPos, Point3(0,0,-1), 200)
				
				if -1 ~= kFloorPos:GetX() and -1 ~= kFloorPos:GetY() and -1 ~= kFloorPos:GetZ() then
					GetPlayer():AttachParticleToPointS(90000 + it, kFloorPos, "e_ef_fossilearth_target_01", 1.0)
				end
			end
		end
	end
	
	if textKey == "begin" then
		-- local kAction = actor:GetAction()
		-- kAction:NextStage()
		-- QuakeCamera(1,1,1,1,1)
		local	action = actor:GetAction()
		CheckNil(nil==action);
		CheckNil(action:IsNil());
		
		local iHorMaxCount = 4
		local iVerMaxCount = 20
		
		local fLen = action:GetParamFloat(200)
		local fHalfLen = fLen / 2
		
		local kStartPoint = action:GetParamAsPoint(111)
		
		if( CheckNil(g_world == nil) ) then return false end
		if( CheckNil(g_world:IsNil()) ) then return false end
		for it=1,80 do
			local ibool = action:GetParamInt(it)
			if ibool == 1 then
			
				local vPos = ((it-1) / iVerMaxCount) + 1
				local hPos = ((it-1) % iVerMaxCount) + 1
				local Offset = Point3(hPos * fLen - fHalfLen, vPos * fLen - fHalfLen, 0)
				local kPoint = Point3(kStartPoint:GetX(), kStartPoint:GetY(), kStartPoint:GetZ())
				kPoint:Add(Offset)
				local kWarnPos = Point3(kPoint:GetX(), kPoint:GetY(), kPoint:GetZ()+100)
				local kFloorPos = g_world:ThrowRay(kWarnPos, Point3(0,0,-1), 200)
				
				if -1 ~= kFloorPos:GetX() and -1 ~= kFloorPos:GetY() and -1 ~= kFloorPos:GetZ() then
					--ODS("Pos X : "..kFloorPos:GetX().." Y : "..kFloorPos:GetY().." Z : "..kFloorPos:GetZ().."\n",false,987)
					GetPlayer():AttachParticleToPointS(80000 + it, kFloorPos, "ef_BoneDragon_skill_03_02_char_root", 1.0)
				end
			end
		end
	end
	
	if textKey == "fire" or textKey == "hit" then
		local	action = actor:GetAction()
		if action:GetTargetList():IsNil() == false then
			action:GetTargetList():ApplyActionEffects();
		end
		action:ClearTargetList();
	end
	
	if textKey == "quake" then
		Act_Mon_QuakeCamera(actor)
	end

	return	true;
end

function Act_Mon_QuakeCamera(actor)
	local QuakeTime = actor:GetAnimationInfo("QUAKE_TIME")
	if nil ~= QuakeTime and "" ~= QuakeTime then
		local QuakeFactor = actor:GetAnimationInfo("QUAKE_FACTOR")	
		if  nil == QuakeFactor then
			QuakeFactor = 1.0
		else
			QuakeFactor = tonumber(QuakeFactor)
		end
		QuakeCamera(QuakeTime, QuakeFactor)
	end
end

function Act_Mon_DarkBreath_OnTargetListModified(actor,action,bIsBefore)
	Act_Melee_Default_OnTargetListModified(actor,action,bIsBefore)
end
