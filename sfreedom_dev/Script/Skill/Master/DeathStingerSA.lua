
function Skill_DeathStingerSA_OnCheckCanEnter(actor,action)
	return true
end
function Skill_DeathStingerSA_OnEnter(actor, action)
	CheckNil(nil==actor)
	CheckNil(actor:IsNil())
	CheckNil(nil==action)
	CheckNil(action:IsNil())

	action:SetDoNotBroadCast(true)
	action:SetParamAsPoint(1, actor:GetTranslate())
	return true
end

function Skill_DeathStingerSA_OnCastingCompleted(actor, action)

	CheckNil(nil==actor);
	CheckNil(actor:IsNil());

	actor:PlayCurrentSlot()
end

function Skill_DeathStingerSA_Shot_Fire(actor,action)

end


function Skill_DeathStingerSA_OnUpdate(actor, accumTime, frameTime)

	CheckNil(nil==actor)
	CheckNil(actor:IsNil())
	local action = actor:GetAction()
	CheckNil(nil==action)
	CheckNil(action:IsNil())

	local iSlot = action:GetCurrentSlot()
	local bDone = actor:IsAnimationDone()
	if true==bDone then
		if 2==iSlot	then
			return false == bDone
		elseif 0==iSlot then
			actor:StartBodyTrail("../Data/5_Effect/9_Tex/swift_01.tga",500,0,0);
		end

		actor:PlayNext()
	end

--	if 2>iSlot then
		local guid = actor:GetCallerGuid()
		if false==guid:IsNil() then
			local Caller = g_pilotMan:FindPilot(guid)
			if false==Caller:IsNil() then		
				local CallerActor = Caller:GetActor()
				if false == CallerActor:IsNil() then
					actor:SetTranslate(CallerActor:GetTranslate())
				end
			else
				return false
			end
		end
--	end
	
	return true
end
function Skill_DeathStingerSA_OnCleanUp(actor,action)
	
	CheckNil(nil==actor);
	CheckNil(actor:IsNil());

	actor:EndBodyTrail();	--혹시 모르니까
	g_world:RemoveActorOnNextUpdate(actor:GetPilotGuid())

	CheckNil(nil==action);
	CheckNil(action:IsNil());
	action:SetDoNotBroadCast(true)

	return true;
end
function Skill_DeathStingerSA_OnLeave(actor, action)
	return true
end

function Skill_DeathStingerSA_OnEvent(actor, textKey)

	CheckNil(nil==actor)
	CheckNil(actor:IsNil())
		
	if textKey == "alpha_end" then
		actor:SetTargetAlpha(actor:GetAlpha(),0.0, 0.25); -- 일단 처음엔 투명하게
	end
end
