FILE_REQ("modules/fish/fish_data")
VIEW_REQ("modules/fish/fish_view")
VIEW_REQ("modules/fish/fish_ground_menu")
VIEW_REQ("modules/fish/fish_area")
VIEW_REQ("modules/fish/fish_explore")
FishCtrl = FishCtrl or BaseClass(BaseCtrl)
FishCtrl.OpType = {
    Start = 0,
    GetFish = 1,
    Stop = 2,
}
FishCtrl.NoticeType = {
    Fail = 0,
    Get = 1,
    Limit = 2,
}
function FishCtrl:__init()
	if FishCtrl.Instance ~= nil then
		Debuger.LogError("[FishCtrl] attempt to create singleton twice!")
		return
	end
	FishCtrl.Instance = self
	self.data = FishData.New()
    self:RegisterAllProtocols()
end
function FishCtrl:__delete()

end
function FishCtrl:RegisterAllProtocols()
    self:RegisterProtocol(CSFishGroundReq)--0开始钓鱼 1结束钓鱼(调到鱼) 2停止钓鱼
    self:RegisterProtocol(SCFishingNotify, "SCFishingNotify")
    self:RegisterProtocol(SCStopFishingNotify, "SCStopFishingNotify")--0钓鱼失败 1钓鱼成功 2钓鱼失败，时间不足
end
function FishCtrl.CSFishGroundReq(op_type, param1)
    local protocol = GetProtocol(CSFishGroundReq)
    protocol.op_type = op_type
    protocol.param1 = param1 or 0
    SendProtocol(protocol)
end
function FishCtrl:SCFishingNotify(protocol)
    if MainPlayer and MainPlayer:ObjId() == protocol.role_obj_id then
        self.data:Fish().state = FishData.State.Fishing
    elseif  MainPlayer and MainPlayer:ObjId() ~= protocol.role_obj_id then
        local obj = SceneCtrl.Instance:Objs():GetObjectByObjId(protocol.role_obj_id)
        if obj then
            obj:PlayAnim(ChAnimType.FishIdle)
            obj:PlayAnim(ChAnimType.Lagan)
            obj:PlayAnim(ChAnimType.LaganIdle)
        end
    end
end
function FishCtrl:SCStopFishingNotify(protocol)
    if MainPlayer and MainPlayer:ObjId() == protocol.role_obj_id then
        if protocol.stop_reason == FishCtrl.NoticeType.Fail then
            --收杆
        elseif protocol.stop_reason == FishCtrl.NoticeType.Get then
            --缓存钓到的列表
            local list = self.data:FishList()
            local had = false
            for index, value in ipairs(list) do
                if value.item_id == protocol.param1 then
                    had = true
                    value.num = value.num + protocol.param2
                end
            end
            if had == false then
                table.insert(list, Item.Create({item_id = protocol.param1, num = protocol.param2}))
            end
        else
        end
        TimeHelper:CancelTimer(self.handle_delay)
        self.handle_delay = TimeHelper:AddDelayTimer(function ()
            self.data:Fish().state = FishData.State.Init
        end, 0.6)
    elseif MainPlayer and MainPlayer:ObjId() ~= protocol.role_obj_id then
        local obj = SceneCtrl.Instance:Objs():GetObjectByObjId(protocol.role_obj_id)
        if obj then
            obj:PlayAnim(ChAnimType.ShouGan)
            obj:PlayAnim(ChAnimType.FishIdle)
        end
    end
end