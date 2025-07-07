FishData = FishData or BaseClass()
FishData.State = {
    None = 0,
    Init = 1,--点击开始钓鱼进入该状态，拿鱼竿
    Wait = 2,--点击抛竿，开始抛竿然后 垂钓等待
    Fishing = 3--点击咬钩进入该状态， 播放咬钩挣扎
}
function FishData:__init()
    if FishData.Instance ~= nil then
		Debuger.LogError("[FishData] attempt to create singleton twice!")
		return
	end	
	FishData.Instance = self
    self.fish = SmartData.New({state = FishData.State.None, fish_area = nil})
    self.collect_scene = Config.collect_scene_auto
    self.gather_scene = self.collect_scene.gather_scene
    self.other = self.collect_scene.other[1]
    self.fish_pos = self.collect_scene.fish_pos

    self.fish_map = {}
    self.fish_list = {}
    self.pos_list = {}
    self:initConfig()
end
function FishData:initConfig()
    for index, value in ipairs(self.gather_scene) do
        if value.limit_skill == 1 then
            self.fish_map[value.scene_id] = value
        end
    end
    for index, value in ipairs(self.fish_pos) do
        self.pos_list[value.pos_group] = self.pos_list[value.pos_group] or {}
        table.insert(self.pos_list[value.pos_group], value)
    end
end
function FishData:__delete()
    
end
function FishData:Fish()
    return self.fish
end
function FishData:WaitTime()
    return GameMath.Ramdon(self.other.time_down, self.other.time_up)
end
function FishData:InFishMap(scene_id)
    scene_id = scene_id or SceneData.Instance:SceneId()
    return self.fish_map[scene_id] ~= nil
end
function FishData:FishList()
    return self.fish_list
end
function FishData:MapInfo(scene_id)
    scene_id = scene_id or SceneData.Instance:SceneId()
    return self.fish_map[scene_id]
end
function FishData:ClearSetting()
    if MainPlayer then
        MainPlayer:PlayAnim(ChAnimType.Run)
        MainPlayer:PlayAnim(ChAnimType.Idle)
    end
    FishCtrl.CSFishGroundReq(FishCtrl.OpType.Stop)
    self:Fish().fish_area = nil
    self:Fish().state = FishData.State.None
    local vir_cam = SceneMgr:GetFreeLookCamera(SceneManager.FreeLookCamType.FishSceneCamera)
    local FreeLook = typeof(Cinemachine.CinemachineFreeLook)
    local FreeLookCom = vir_cam:GetComponent(FreeLook)
    FreeLookCom.Priority = 4
end
function FishData:PosGroup()
    local map = self:MapInfo()
    return self.pos_list[map.pos_1_group or 1]
end
--随机钓鱼点
function FishData:RamdonPoint()
    local list = self:PosGroup()
    local num = table.nums(list)
    return list[GameMath.Ramdon(1, num)]
end