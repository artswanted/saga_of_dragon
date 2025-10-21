function NPC_LardBerkshire(actor, keyText)
	if keyText == "end" then
		chance = math.random(1, 1000)
		if chance > 700 then
			actor:SetTargetAnimation("electricshok_01")
		elseif chance > 500 then
			actor:SetTargetAnimation("idle_02")
		else
			actor:SetTargetAnimation("idle_01")
		end
	end
end
