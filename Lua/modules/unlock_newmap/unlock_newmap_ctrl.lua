VIEW_REQ("modules/unlock_newmap/unlock_newmap_view")

UnlockNewMapCtrl = UnlockNewMapCtrl or BaseClass(BaseCtrl)

function UnlockNewMapCtrl:__init()
    if UnlockNewMapCtrl.Instance ~= nil then
		Debuger.LogError("[UnlockNewMapCtrl] attempt to create singleton twice!")
        
        return
    end
    UnlockNewMapCtrl.Instance = self
    self.active_id = 0

    self:RegisterProtocol(SCMapUnlockFristNotice,"OnMapUnlockFristNotice")
end

function UnlockNewMapCtrl:__delete()
    if UnlockNewMapCtrl.Instance ~= nil then
        UnlockNewMapCtrl.Instance = nil
    end
end

function UnlockNewMapCtrl:OnMapUnlockFristNotice(protocol)
    -- Debuger.LogError("unlock...id"..protocol.active_id)
    -- self.active_id = protocol.active_id
    -- local cfg_data = self:FindDataByID(self.active_id)
    -- local cfg_data_is_pet = cfg_data.is_pet
    -- local is_show = cfg_data.is_show_new_scene == 1
    -- if is_show then
    --     ViewMgr:OpenView(UnlockNewMapView)
    -- end
end

function UnlockNewMapCtrl:FindDataByID(id)
	local config = Config.world_map_auto.word_type_show
	for k, v in pairs(config)do
		if v.id == id then
			return v
		end
	end
	Debuger.LogError("[ERROR] UnlockNewMapView:FindDataByID   can't not find id:"..id)
	return nil
end
function UnlockNewMapCtrl:FindDataBySceneID(id)
	local config = Config.world_map_auto.word_type_show
	for k, v in pairs(config)do
		if v.scene_id == id then
			return v
		end
	end
	Debuger.LogError("[ERROR] UnlockNewMapView:FindDataByID   can't not find scene_id:"..id)
	return nil
end
function UnlockNewMapCtrl:GetIrregularID(id)
	local config = self:FindDataBySceneID(id)
    if config then
        return config.irregular_id
    end
	Debuger.LogError("[ERROR] UnlockNewMapView:FindDataByID   can't not find id:"..id)
	return nil
end