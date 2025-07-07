FILE_REQ("modules/serveractivity/dragon_boat/act_dragon_boat_data")
FILE_REQ("modules/serveractivity/dragon_boat/luck_draw_box/act_luck_draw_box_ctrl")
FILE_REQ("modules/serveractivity/dragon_boat/dragon_collect/act_dragon_collect_ctrl")

VIEW_REQ("modules/serveractivity/dragon_boat/act_dragon_boat_view")

ActDragonBoatCtrl = ActDragonBoatCtrl or BaseClass(BaseCtrl)

function ActDragonBoatCtrl:__init()
	if ActDragonBoatCtrl.Instance ~= nil then
		Debuger.LogError("[ActDragonBoatCtrl] attempt to create singleton twice!")
		return
	end
    ActDragonBoatCtrl.Instance = self
    ActDragonBoatData.New()
    ActLuckDrawBoxCtrl.New()
    ActDragonCollectCtrl.New()
    self.data = ActDragonBoatData.Instance

    -- 活动开启注册
    ActivityRandData.Instance:Register(Mod.DragonBoat.Main,function()
        return self.data:IsOpen()
    end)

    -- 倒计时
    ActivityRandData.Instance:RegisterActCountDown(Mod.DragonBoat.Main, function (TimeMeter)
        if TimeMeter ~= nil then
            local end_time = self.data:EndTime()
            TimeMeter:StampTime(end_time or 0,TimeType.Type_Special_9, "")
        end
    end)

    self:RegisterAllProtocols()
end

function ActDragonBoatCtrl:__delete()
    ActDragonBoatCtrl.Instance = nil
end

function ActDragonBoatCtrl:OnUnloadGameLogic()
    self.data:ClearData()
end

function ActDragonBoatCtrl:RegisterAllProtocols()
   
end