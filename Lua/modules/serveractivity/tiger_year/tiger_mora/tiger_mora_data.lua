TigerMoraData = TigerMoraData or BaseClass()
function TigerMoraData:__init()
    if TigerMoraData.Instance ~= nil then
        Debuger.LogError("[TigerMoraData] attempt to create singleton twice!")
        return
    end
    TigerMoraData.Instance = self

    self.act_info = SmartData.New({times = 0, is_win = 0})
    self.pk_result = SmartData.New({flush = false})
    self.last_game_info = nil
end

function TigerMoraData:__delete()
    TigerMoraData.Instance = nil
end

function TigerMoraData:ClearAllInfo()
    self.last_game_info = nil
end

function TigerMoraData:Config()
    return Config.yinhudouxinchun_auto
end
function TigerMoraData:OtherConfig()
    return self:Config().other[1]
end

function TigerMoraData:GetPetId()
    return 50230
end

function TigerMoraData:GetRewardCfg()
    local base_list = ServerActivityData.Instance:GetActOpenDayCfg(ACTIVITY_TYPE.RAND_TIGER_MORA, self:Config().basic_configure)
    return base_list[1]
end

-----次数
function TigerMoraData:GetMaxTimes()
    return self:OtherConfig().day_num
end
function TigerMoraData:GetRemainTimes()
    return self:OtherConfig().day_num - self.act_info.times
end

function TigerMoraData:GetIsWin()
    return self.act_info.is_win
end

-----奖励组
function TigerMoraData:GetRewardList(reward_group)
    local show_list = {}
    for i, v in ipairs(self:Config().gift_group) do
        if v.reward_group == reward_group and v.win_reward_item[0] ~= nil then
            table.insert(show_list, v)
        end
    end
    table.sort(show_list, DataHelper.SortFunc("seq"))
    return show_list
end

-----红点
--有次数时需有红点提示，次数用完则红点消失
function TigerMoraData:GetRemindNum()
    if self:GetRemainTimes() > 0 then
        return 1
    end
    return 0
end

function TigerMoraData:GetLastGameInfo()
    return self.last_game_info or {is_win = 0, left_pic = -1, right_pic = -1}
end

function TigerMoraData:SetLastGameInfo(is_win, left_pic, right_pic)
    self.last_game_info = {is_win = is_win, left_pic = left_pic, right_pic = right_pic}
end

-----协议
function TigerMoraData:SetActInfo(protocol)
    self.act_info.is_win = protocol.is_win
    self.act_info.times = protocol.times
    if protocol.is_win ~= 0 then
        self.pk_result.flush = not self.pk_result.flush
    end
end

function TigerMoraData:SaveNeedDelayItem(item_list)
    self.need_delay_item = item_list
end

function TigerMoraData:GetDelayItems()
    return self.need_delay_item
end

function TigerMoraData:ReleaseNeedDelayItem()
    self.need_delay_item = nil
end
