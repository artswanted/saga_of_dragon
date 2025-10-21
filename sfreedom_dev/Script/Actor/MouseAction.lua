g_MouseOverGuid = nil

function Actor_MouseOver(who)
	local actor = ToActor(who)
	if nil==actor and actor:IsNil()==true then
		--ODS("Actor_MouseOver : nil==actor\n",false, 3851)
		return false
	end
	
	local pilot = g_pilotMan:FindPilot( actor:GetPilotGuid() )
	if pilot:IsNil() == true then
		--ODS("Actor_MouseOver : pilot:IsNil()\n",false, 3851)
		return false
	end
	if( CheckNil(g_world == nil) ) then return false end
	if( CheckNil(g_world:IsNil()) ) then return false end
	
	local kOldGuid = g_world:GetOldMouseOverObject()
	--[[if nil~=kOldGuid and false == kOldGuid:IsNil() then
		local pilot = g_pilotMan:FindPilot( kOldGuid )
		if false == pilot:IsNil() then
			pilot:GetActor():DetachFrom(49023854, true)
		end
	end]]
	--[[local kSelectGuid = g_selectStage:GetSelectedActor()
	local kMouseOverGuid = actor:GetPilotGuid()
	if kMouseOverGuid:GetStr() == kSelectGuid:GetStr() then
		return;
	end]]
	
	pilot:GetActor():AttachParticleS(49023854, "p_ef_heart", "eff_char_select04", 1)
	
	local strSound = "cha-cursor0"
	local spawnSlot = g_selectStage:GetSpawnSlot(actor:GetPilotGuid())
	PlaySoundByID( strSound..(spawnSlot) )
	--ODS("PlaySound "..strSound..(spawnSlot).."\n", false, 3851)
	
	return true
end


function Actor_MouseOut(ActorGuid)
	--ODS("Actor_MouseOut\n",false, 3851)
	
	local pilot = g_pilotMan:FindPilot( ActorGuid )
	if pilot:IsNil() == true then
		ODS("Actor_MouseOut : pilot:IsNil()\n",false, 3851)
		return false
	end
	--ODS("Actor_MouseOut - "..kActorID:GetStr().."\n",false, 3851)
	
	pilot:GetActor():DetachFrom(456)
	local spawnSlot = g_selectStage:GetSpawnSlot(ActorGuid)
	local kAction = pilot:GetActor():ReserveTransitAction("a_intro_idle")
	kAction:SetSlot(spawnSlot-1)

	return true
end
