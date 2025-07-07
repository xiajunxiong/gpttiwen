GuildTrialRewardGet = GuildTrialRewardGet or DeclareView("GuildTrialRewardGet", "guild/guild_trial_reward_get")
-- RewardList = Array(GuildTrialRewardGetItem)
function GuildTrialRewardGet:GuildTrialRewardGet()
    self.data = GuildData.Instance
end

function GuildTrialRewardGet:LoadCallback()

end
------------------------------------
GuildTrialRewardGetPanel = GuildTrialRewardGetPanel or DeclareMono("GuildTrialRewardGetPanel",UIWFlushPanel)
function GuildTrialRewardGetPanel:GuildTrialRewardGetPanel()
    self.data = GuildData.Instance
    self.data_cares = {
        {data = GuildData.Instance.guild_trial_reward_item, func = self.ShowItemGet,init_call = false}
    }
end

function GuildTrialRewardGetPanel:Awake()
    UIWFlushPanel.Awake(self)
    self:FlushReward()
end

function GuildTrialRewardGetPanel:OnDestroy()
    GuildData.Instance.boss.flushRewardRedPoint = not GuildData.Instance.boss.flushRewardRedPoint
    UIWFlushPanel.OnDestroy(self)
    TimeHelper:CancelTimer(self.roll_run)
    self.roll_run = nil
    TimeHelper:CancelTimer(self.stop_delay)
    self.stop_delay = nil
    self.state = nil
end

function GuildTrialRewardGetPanel:OnClickGet()
    if self.state == nil then
        GuildCtrl.SendGuildBossReq(GUILD_BOSS_REQ_TYPE.GET_REWARD)
        self.state = "sended"
    elseif self.state == "got" then
        PublicPopupCtrl.Instance:Center(Language.Guild.TrialRewardGot)
    else
        PublicPopupCtrl.Instance:Center(Language.Guild.WaitForReward)
    end
end

function GuildTrialRewardGetPanel:OnClickClose()
    ViewMgr:CloseView(GuildTrialRewardGet)
end

function GuildTrialRewardGetPanel:FlushReward()
    local boss_info = self.data:GuildBossInfo()
    self.reward_data_list = self.data:GetGuildBossReward(boss_info.guild_boss_chapter_id, boss_info.guild_boss_seq - 1)

    for i,v in ipairs(self.reward_data_list) do
        self.RewardList[i]:SetData(v)
    end
end

function GuildTrialRewardGetPanel:ShowItemGet(param)
    for k,v in pairs(self.reward_data_list) do
        if v.item_id == self.data.guild_trial_reward_item.item.item_id then
            self.end_index = k
            break
        end
    end
    if self.end_index ~= nil then
        self:StartRoll(0.1)
        self.stop_delay = TimeHelper:AddDelayTimer(function ()
            TimeHelper:CancelTimer(self.roll_run)
            self.roll_run = nil
            self:StartRoll(0.2, self.pos_index, function()
                if self.pos_index == self.end_index then
                    TimeHelper:CancelTimer(self.roll_run)
                    self.state = "got"
                    GuildCtrl.SendGuildBossReq()
                    PublicPopupCtrl.Instance:CenterI({item_id = self.data.guild_trial_reward_item.item.item_id, num = self.data.guild_trial_reward_item.item.num})
                end
            end)
        end, 2)
    end
end


function GuildTrialRewardGetPanel:StartRoll(speed_rate, start_pos, jump_func)
    if self.roll_run == nil then
        self.pos_index = start_pos or 1
        self.roll_run = TimeHelper:AddRunTimer(function()
            if self.cur_select_obj ~= nil then
                self.cur_select_obj:SetActive(false)
            end
            self.pos_index = self.pos_index > 12 and 1 or self.pos_index
            self.RewardList[self.pos_index].SelectObj:SetActive(true)
            self.cur_select_obj = self.RewardList[self.pos_index].SelectObj
            if jump_func ~= nil then
                jump_func()
            end
            self.pos_index = self.pos_index + 1
        end,speed_rate)
    end
end
----------------------------------------------------------------
GuildTrialRewardGetItem = GuildTrialRewardGetItem or DeclareMono("GuildTrialRewardGetItem", UIWidgetBaseItem)
-- ItemCell = UIWItemCell
-- ItemName = Text
function GuildTrialRewardGetItem:GuildTrialRewardGetItem()
end

function GuildTrialRewardGetItem:SetData(data)
    local item = Item.Create(data)
    self.ItemCell:SetData(item)
    UH.SetText(self.ItemName, item:Name())
end