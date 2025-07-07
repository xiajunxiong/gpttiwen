
TopLayerActTipsData = TopLayerActTipsData or BaseClass()

function TopLayerActTipsData:__init()
    if TopLayerActTipsData.Instance ~= nil then
        Debuger.LogError("[TopLayerActTipsData] attempt to create singleton twice!")
        return
    end
    TopLayerActTipsData.Instance = self
    self.cur_show_act = SmartData.New()
    self.cur_show_act_time = SmartData.New {num = 0}
end

function TopLayerActTipsData:CurShowAct(act_info)
    if act_info then self.cur_show_act:Set() end
    act_info = act_info or self.cur_show_act:Val()
    self.cur_show_act:Set(act_info)
    return act_info
end

function TopLayerActTipsData:ResetShowActTime(time_s)
    self.cur_show_act_time.num = time_s
end

--获取当前推送活动倒计时
function TopLayerActTipsData:GetShowActStampTime()
    return self.cur_show_act_time.num
end

function TopLayerActTipsData:GetCurShowAct()
    return self.cur_show_act:Val()
end