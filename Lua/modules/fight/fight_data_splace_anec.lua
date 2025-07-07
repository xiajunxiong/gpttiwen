function FightData:FightDataSplaceAnecdote()
	self.fight_splace_anec_info = SmartData.New()
    self.fight_splace_anec_objs = SmartData.New()

end 

function FightData:SetSplaceAnecInfo(protocol)
	self.fight_splace_anec_info:Set(protocol.fight_fantasy_info)
	self.fight_splace_anec_objs:Set(protocol.obj_list)
end


function FightData:GetSplaceAnecInfo()
	return self.fight_splace_anec_info
end

function FightData:GetSplaceAnecObjsInfo()
	return self.fight_splace_anec_objs
end