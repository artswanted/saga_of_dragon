function SummonThorn_Initialize(actor)
    actor:HideNode("Dummy01",true);
    local kPos = actor:GetNodeWorldPos("Dummy01");
	actor:AttachParticleToPointS(2,kPos,"ef_Umai_skill_03_02_char_root", 1.9)
	actor:AttachSound(50,"Blizzard2");
end

function SummonThorn2_Initialize(actor)
    actor:HideNode("Dummy01",true);
    local kPos = actor:GetNodeWorldPos("Dummy01");
	actor:AttachParticleToPointS(2,kPos,"ef_Aram_skill_01_05_char_root", 1.1)
end

function SummonThorn3_Initialize(actor)
    actor:HideNode("Dummy01",true);
    local kPos = actor:GetNodeWorldPos("Dummy01");
	actor:AttachParticleToPointS(2,kPos,"eff_guardian_gaia_earthquake", 1.1)
end

function SummonHand_Initialize(actor)
    actor:HideNode("Dummy01",true);
    local kPos = actor:GetNodeWorldPos("Dummy01");
	actor:AttachParticleS(2,"Dummy01","ef_ghost_man_02_skill_01_03_char_root", 3)
end

function SummonBone_Initialize(actor)
    --actor:HideNode("Dummy01",true);
    --local kPos = actor:GetNodeWorldPos("Dummy01");
	--actor:AttachParticleToPointS(2,kPos,"ef_Aram_skill_01_05_char_root", 1.1)
end

function SummonStorm_Initialize(actor)
--	actor:AttachParticleS(2,"char_root","ef_Earthquake_skill_02_char_root", 1)
--	actor:AttachParticleS(2,"p_ef_shot_01","ef_Earthquake_skill_02_char_root", 1)
end

function Summonfirefield_Initialize(actor)
    actor:HideNode("Dummy01",true);
    local kPos = actor:GetNodeWorldPos("Dummy01");
	actor:AttachParticleS(2,"Dummy01","ef_Siegedragon_Big2_skill_04_02_char_root", 3)
end


function SummonIce_Initialize(actor)
    actor:HideNode("Dummy01",true);
    local kPos = actor:GetNodeWorldPos("Dummy01");
	actor:AttachParticleS(2,"Dummy01","ef_Blizzard_00003", 3)
	actor:AttachParticleS(3,"Dummy01","EF_Common_Mon_IcePick_03", 2)
end


function SummonIceBoom_Initialize(actor)
    actor:HideNode("Dummy01",true);
    local kPos = actor:GetNodeWorldPos("Dummy01");
	actor:AttachParticleS(2,"Dummy01","EF_Pattern_IceEyeRing_char_root", 1)
		actor:AttachParticleS(3,"Dummy01","EF_Common_Mon_Torus_IcePick_00_char_root", 1)
end

function LightningMargnetSA_Initialize(actor)
	actor:HideNode("Dummy01",true)

	local guid = GUID("123")
	guid:Generate()
	local pilot = g_pilotMan:NewPilot(guid, 1000655, 0) -- sa_lightningmargnetPlay
	if false == pilot:IsNil() then
		pilot:SetUnit(guid,UT_ENTITY,1,1,0);
		local kBird = pilot:GetActor()
		if false == kBird:IsNil() then
			if( CheckNil(g_world == nil) ) then return false end
			if( CheckNil(g_world:IsNil()) ) then return false end
			local kTrans = actor:GetTranslate()
			kTrans:SetZ(kTrans:GetZ()+5)
			g_world:AddActor(guid, kBird, kTrans, 9);
			pilot:GetUnit():SetCaller(actor:GetPilotGuid())
			local	kLookAt = actor:GetLookingDir()
			kBird:SeeFront(true, true)	-- 정면을 보자
			kBird:ReserveTransitAction("a_LightningMargnetPlay")
			pilot:GetActor():ClearActionState();
			if actor:IsMyActor() then
	        	pilot:GetActor():SetUnderMyControl(true);
		    end
		end
	end
end

function Paris_Laser_Initialize(actor)
    actor:HideNode("Dummy01",true);
	actor:AttachParticleS(2,"Dummy01","e_b_palis_skill_03_CylinderBeem", 1.0)
end

function Tree_Rotten_black_Initialize(actor)
    actor:HideNode("Dummy01",true);
	actor:AttachParticleS(2,"Dummy01","efx_tree_rotten_skill_attk_01", 1.0)
end

function iceRock_Initialize(actor)
    actor:HideNode("Dummy01",true);
	actor:AttachParticleS(2,"Dummy01","ef_IceRock_shot_01", 1.0)
end

function goblin01_Initialize(actor)
    actor:HideNode("Dummy01",true);
    actor:AttachParticleS(2,"Dummy01","ef_Blizzard_00003", 2)
	actor:AttachParticleS(3,"Dummy01","EF_Common_Mon_IceEject_00", 0.35)
end


function goblin02_Initialize(actor)
    actor:HideNode("Dummy01",true);
	actor:AttachParticleS(2,"Dummy01","ef_Blizzard_00003", 1.0)
	actor:AttachParticleS(3,"Dummy01","ef_IceRock_shot_01", 1.5)
end

function goblin03_Initialize(actor)
    actor:HideNode("Dummy01",true);
	actor:AttachParticleS(2,"Dummy01","ef_Blizzard_00003", 1.0)
	actor:AttachParticleS(3,"Dummy01","EF_Common_Mon_IceEject_00", 0.4)
end

function elga_Initialize(actor)
    actor:HideNode("Dummy01",true);
	actor:SetHide(true);
	actor:SetHideShadow(true);
	-- actor:AttachParticleS(2,"Dummy01","EF_test", 0.1)
	-- actor:AttachParticleS(3,"Dummy01","EF_test", 0.1)
end

function elgathorn_Initialize(actor)
    actor:HideNode("Dummy01",true);
    local kPos = actor:GetNodeWorldPos("Dummy01");
	actor:AttachParticleToPointS(2,kPos,"eff_elgar_03_summon_thorns_00", 1.1)
end
