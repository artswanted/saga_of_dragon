playerInfo = {} 

function Pilot_FindXmlPath(gender, class)
	xmlPath = ""
	if gender == 1 then			-- 남자
		xmlPath = "pilot/fighter_male.xml"
	elseif gender == 2 then		-- 여자
		xmlPath = "pilot/fighter_female.xml"
	elseif gender == 3 then		-- 펫
		petXmlPath = GetActorPath(class)
		if petXmlPath ~= nil then
			xmlPath = petXmlPath
		end
		
	elseif gender == 4 then		-- 몬스터
		monXmlPath = GetActorPath(class)
		if monXmlPath ~= nil then
			xmlPath = monXmlPath
		end
	else		
		MessageBox("unknown g ender = " .. gender, "actor.lua")
	end

	return xmlPath 
end

function Actor_OnMeetFloor(actor,action,bMeetFloor)
	if bMeetFloor then
		actor:SetComboCount(0);
		actor:SetParam("FLOAT_ATTACK_ENABLE","TRUE");
		actor:SetParam("JUMP_DOWN_ATTACK_ENABLE","TRUE");
		actor:SetParam("RANGER_FLOATDOWNSHOT_COUNT","0");
		actor:SetParam("DOUBLE_JUMP","TRUE");
		
		if actor:GetParam("FLOAT_EVASION") == "FALSE" then
			actor:SetCanHit(true);
		end
		
		actor:SetParam("FLOAT_EVASION","TRUE");
	end
end

function Actor_OnPlayerPilotSet(kPilotGUID,bSet)

	if bSet then
		GetHelpSystem():LoadHelpInfoFile(kPilotGUID);
	else
		GetHelpSystem():SaveHelpInfoFile(kPilotGUID);
	end

end

function GetEffectAutoScale(actor)
	local nEffectScale = 1.0
	if nil~=actor and false==actor:IsNil() then
		local nUnitSize = actor:GetAbil(AT_UNIT_SIZE)
		if nUnitSize == 0 then
			nUnitSize = 1 --유닛 크기가 설정되어있지 않으면 기본으로 지정한다.
		end

		if 1 == nUnitSize then --1:Small
			nEffectScale = 1.0
		elseif 2 == nUnitSize then --2:Middle
			nEffectScale = 1.5
		elseif 3 == nUnitSize then --3:Large
			nEffectScale = 2.0
		elseif 4 == nUnitSize then --4:BigLarge
			nEffectScale = 2.5
		elseif 5 == nUnitSize then --5:Giant
			nEffectScale = 3.0
		end
	end
	return nEffectScale
end


function TestHeadSize(size)
 GetMyActor():SetDefaultHeadSize(size)
 GetMyActor():SetTargetHeadSize(1.0,1.0)
end
