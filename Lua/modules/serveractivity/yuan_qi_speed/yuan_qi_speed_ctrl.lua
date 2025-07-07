FILE_REQ("modules/serveractivity/yuan_qi_speed/yuan_qi_speed_data")
VIEW_REQ("modules/serveractivity/yuan_qi_speed/yuan_qi_speed_view")
VIEW_REQ("modules/serveractivity/yuan_qi_speed/yuan_qi_speed_game_view")

YuanQiSpeedCtrl = YuanQiSpeedCtrl or BaseClass(BaseCtrl)
function YuanQiSpeedCtrl:__init()
    if YuanQiSpeedCtrl.Instance ~= nil then
        Debuger.LogError("[YuanQiSpeedCtrl] attempt to create singleton twice!")
        return
    end
    YuanQiSpeedCtrl.Instance = self
    self.data = YuanQiSpeedData.New()
    self:RegisterAllProtocols()
end

function YuanQiSpeedCtrl:__delete()
    YuanQiSpeedCtrl.Instance = nil
    Delete(self, "data")
end

function YuanQiSpeedCtrl:OnInit()
    PeriodActivityCtrl.Instance:RegisterRemind(Mod.MayDay.YuanQiSpeed, self.data.act_info, function()
        local red_num = ActivityData.IsOpen(ACTIVITY_TYPE.RAND_YUAN_QI_SPEED) and self.data:GetRemindNum() or 0
    	return red_num
    end)
end

function YuanQiSpeedCtrl:OnUnloadGameLogic()
end

function YuanQiSpeedCtrl:RegisterAllProtocols()
    self:RegisterProtocol(SCYuanQiSpeedInfo, "OnSCYuanQiSpeedInfo")
    self:RegisterProtocol(SCRAYuanQiSpeedPlayInfo, "OnSCRAYuanQiSpeedPlayInfo")
end

function YuanQiSpeedCtrl:OnSCYuanQiSpeedInfo(protocol)
    self.data:SetActInfo(protocol)
end

function YuanQiSpeedCtrl:OnSCRAYuanQiSpeedPlayInfo(protocol)
    self.data:SetGameInfo(protocol)
    if protocol.game_type == 0 or protocol.game_type == 1 then
        YuanQiSpeedCtrl.Instance:ShowResultView(protocol.game_type == 1)
    end
end

--0信息     
function YuanQiSpeedCtrl:SendInfo()
    ServerActivityCtrl.Instance:SendActivityReq(ACTIVITY_TYPE.RAND_YUAN_QI_SPEED, 0)
end

--1开始游戏 param1=seq  游戏难度
function YuanQiSpeedCtrl:SendBegin(seq)
    ServerActivityCtrl.Instance:SendActivityReq(ACTIVITY_TYPE.RAND_YUAN_QI_SPEED, 1, seq)
end

--2是否胜利 param1  (0失败/1胜利)
function YuanQiSpeedCtrl:SendResult(result)
    ServerActivityCtrl.Instance:SendActivityReq(ACTIVITY_TYPE.RAND_YUAN_QI_SPEED, 2, result)
end

--3领取首通奖励 param1 seq
function YuanQiSpeedCtrl:SendFirstReward(seq)
    ServerActivityCtrl.Instance:SendActivityReq(ACTIVITY_TYPE.RAND_YUAN_QI_SPEED, 3, seq)
    self:SendInfo()
end

function YuanQiSpeedCtrl:ShowResultView(is_win)
    local seq = self.data:GetSelect()
    local config = self.data:GetShowCfg(seq)
    if config == nil then
        return
    end
    local item_list, gift_type, title_name = nil, nil, nil
    if is_win then
        item_list = DataHelper.FormatItemList(config.reward1_item)
        title_name = "_LocYouXiChengGong"
        gift_type = GIFT_OPEN_TYPE.CONGRATULATIONS
    else
        item_list = DataHelper.FormatItemList(config.reward2_item)
        title_name = "_LocYouXiShiBai"
        gift_type = GIFT_OPEN_TYPE.FAILBG
    end
    local function func_close()
        ViewMgr:CloseView(YuanQiSpeedGameView)
        ViewMgr:OpenView(YuanQiSpeedView)
    end
    MainOtherCtrl.Instance:OpenGiftView(item_list, gift_type, true, title_name, nil, nil, func_close)
end
