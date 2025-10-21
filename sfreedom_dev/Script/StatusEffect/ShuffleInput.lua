g_kQuickInvDoActionOrig = QuickInvDoAction
g_kQuickInvRandIdx = 1

function ShuffleQuickInvDoAction(uiUKey)
	local uiUNewKey = (uiUKey + g_kQuickInvRandIdx) % 15
	ODS("Shuffle Key: " .. uiUNewKey .. "\n")
	g_kQuickInvDoActionOrig(uiUNewKey)
end

function Effect_ShuffleInput_Begin(actor, iEffectID, iEffectKey)
	if false == actor:IsMyActor() then
		return
	end

	g_kQuickInvRandIdx = math.random(1, 6)
	QuickInvDoAction = ShuffleQuickInvDoAction
	ODS("Effect_ShuffleInput_Begin\n")

	if( CheckNil(g_world == nil) ) then return false end
	if( CheckNil(g_world:IsNil()) ) then return false end

	local fade = actor:GetStatusEffectParam(iEffectID, "FADEIN")
	if nil~=fade and "TRUE"==fade:GetStr() then
		g_world:SetShowWorldFocusFilterColorAlpha( 0x000000, 0.0, 0.5, 0.5, true, false )
	end
end

function Effect_ShuffleInput_End(actor, iEffectID, iEffectKey)	
	if false == actor:IsMyActor() then
		return
	end

	ODS("Effect_ShuffleInput_End\n")
	QuickInvDoAction = g_kQuickInvDoActionOrig

	if( CheckNil(g_world == nil) ) then return false end
	if( CheckNil(g_world:IsNil()) ) then return false end

	local fade = actor:GetStatusEffectParam(iEffectID, "FADEIN")
	if nil~=fade and "TRUE"==fade:GetStr() then
		g_world:SetShowWorldFocusFilterColorAlpha( 0x000000, 0.5, 0.0, 0.5, true, false )
	end
end

function Effect_ShuffleInput_OnTick(actor, iEffectID, fAccumTime)

end
