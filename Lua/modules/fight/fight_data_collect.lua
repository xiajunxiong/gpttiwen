
function FightData:FightDataCollect()
	self.scene_gathers = {}
	self.collect_gather_list = SmartData.New()
	-- self.collect_result_start_time = 0
end

function FightData:ClearSceneGathers()
	self.scene_gathers = {}
end

function FightData:SetGatherFbSceneInfo(protocol)
	local gather_info = protocol.gather_info
	self.scene_gathers[gather_info.obj_id] = gather_info
end

function FightData:SetGatherFbGatherRemoveInfo(protocol)
	self.scene_gathers[protocol.obj_id] = nil
end

function FightData:SetGatherFbGatherListInfo(protocol)
	self.collect_gather_list:Set({})
	for i = 1, protocol.list_num do
		local info = protocol.gather_list[i]
		local item = Item.New()
		item.item_id = info.item_id
		item.num = info.num
		self.collect_gather_list[i] = item
	end
end

function FightData:GetCollectGatherList()
	return self.collect_gather_list
end

function FightData:SetCollectGatherList(data)
	return self.collect_gather_list:Set(data)
end

function FightData:AutoCollecting()
	local main_point = MainPlayer:GetPos()
	local x, y = SceneCtrl.Instance:CurScene():PosToServer(main_point)
	local min_info, min_dis = {}
	for k, v in pairs(self.scene_gathers) do
		local distance = GameMath.GetDistance(x, y, v.x, v.y, false)
		if nil == min_dis or distance < min_dis then
			min_dis = distance
			min_info = v
		end
	end
	local scene_id = SceneData.Instance:SceneId()
	min_info.scene_id = scene_id
	min_info.id = self.gather_scene_list[scene_id].gather_id
	SceneCtrl.Instance:Logic():AutoToNpc(min_info, nil, true, false)
end