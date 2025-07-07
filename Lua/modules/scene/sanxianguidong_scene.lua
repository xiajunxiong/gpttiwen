SXGD_Scene = SXGD_Scene or BaseClass(BaseScene)
function SXGD_Scene:__init()
    
end
function SXGD_Scene:StartLoading()
    self.scene_loaded = false
    -- ViewMgr:OpenView(TowerAdventureLevelView)
    SceneMgr:LoadScene("SXGD_1",function(error)
		if self:Disposed() then
			return
		end
		if not StringIsEmpty(error) then
			Debuger.LogError("GameScene Load Scene failed===SXGD_1")
		end
        ViewMgr:MainViewVisible(false)
        ViewMgr:OpenView(SanXianGuiDongView)
        self.rabbit_list = {}
        for i = 3, 5, 1 do
            self:LoadPrefab(i, function ()
                self.scene_loaded = true        
            end)
        end
		-- self.scene_loaded = true
	end)
end
function SXGD_Scene:LoadPrefab(index, func)
    local res_path = string.format("ui/views/sanxianguidong/rabbits%d.prefab", index)
    ResMgr:LoadGameObject(res_path, function (obj)
        if self:Disposed() then
            if obj then
				UnityGameObj.Destroy(obj)
			end
            return
        end
        obj.transform.localPosition = Vector3.New(400.51,125.76,293.87)
        obj:SetParent(UnityGameObj.Find("ResidentObjectManager"))
        table.insert(self.rabbit_list, obj)
        if index == 5 then
            func()
        end
    end, true, false)
end
function SXGD_Scene:LoadingComplete()
    local complete = self.scene_loaded and ViewMgr:IsOpen(SanXianGuiDongView)
    if complete then
        PreloadViewStateMachine.Instance:StopAll()
    end
    return complete
end
function SXGD_Scene:Dispose()
    BaseScene.Dispose(self)
    for i, v in ipairs(self.rabbit_list) do
		UnityGameObj.Destroy(v)
	end
end