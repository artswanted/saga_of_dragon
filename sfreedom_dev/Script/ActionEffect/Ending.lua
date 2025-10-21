g_Ending_Emporia = {}
g_Ending_Emporia["On"] = false
g_Ending_Emporia["startTime"] = 0.0
function Ending_Emporia_OnEnter()
	if( CheckNil(g_world == nil) ) then return false end
	if( CheckNil(g_world:IsNil()) ) then return false end
	g_world:SetShowWorldFocusFilterColorAlpha(g_fChargeFocusFilterValue[1]
											, g_fChargeFocusFilterValue[2]
											, g_fChargeFocusFilterValue[3]
											, 0.2
											,true
											,true)
	g_world:SetUpdateSpeed(0.0008)
	PlaySoundByID( "PVP_Battle_Hit" )

	g_Ending_Emporia["On"] = true
	g_Ending_Emporia["startTime"] = GetAccumTime()
end

function Ending_Emporia_OnUpdate()
	if( CheckNil(g_world == nil) ) then return false end
	if( CheckNil(g_world:IsNil()) ) then return false end
	if g_Ending_Emporia["On"] then
		local 	fElapsedTime = GetAccumTime() - g_Ending_Emporia["startTime"]
		local	fMaxSpeedTime = 4.0
		local	fRate = fElapsedTime/fMaxSpeedTime;
		if fRate> 1 then
			fRate = 1
		end
		
		local fSpeed = fRate;
		
		if fRate<3.5 then
			fSpeed = fSpeed*0.3
		end
		
		if fRate == 1.0 then
			g_Ending_Emporia["On"] = false
			
			g_world:SetUpdateSpeed(1)			
			g_world:SetShowWorldFocusFilterColorAlpha(g_fChargeFocusFilterValue[1]
													, g_world:GetWorldFocusFilterAlpha()
													, 0
													, 0.2
													,false
													,true)
		else
			g_world:SetUpdateSpeed(fSpeed*0.400)
		end
	end
	
	return true
end

function Ending_Emporia_OnCleanUp()
	if( CheckNil(g_world == nil) ) then return true end
	if( CheckNil(g_world:IsNil()) ) then return true end
	g_world:SetShowWorldFocusFilterColorAlpha(g_fChargeFocusFilterValue[1]
											, g_world:GetWorldFocusFilterAlpha()
											, 0
											, 0.2
											,false
											,true)
	g_world:SetUpdateSpeed(1)
end
