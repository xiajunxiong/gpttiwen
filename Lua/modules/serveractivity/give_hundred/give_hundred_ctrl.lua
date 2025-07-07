--ServerActivityData.Instance:GetScoreData()
--RewardData.Instance:GetScoreRewardFlag(data.seq)

--ServerActivityData.Instance:GetLoginRewardFlag(data.login_days)

--SevenDayData.Instance:GetRewardFlag(self.data.seq) == 0
--local reward_list,end_reward_data = self.data:GetRewardList()

--local show_list, click_item = self.data:GetFightSoulList()
--self.ComObj:SetActive(self.fight_data.fight_soul_info.seq >= self.data.seq)
VIEW_REQ("modules/serveractivity/give_hundred/give_hundred_view")

GiveHundredCtrl = GiveHundredCtrl or BaseClass(BaseCtrl)
function GiveHundredCtrl:__init()
    if GiveHundredCtrl.Instance ~= nil then
		Debuger.LogError("[GiveHundredCtrl] attempt to create singleton twice!")
		return
	end
	GiveHundredCtrl.Instance = self
    self.data = GiveHundredData.New()
end

GiveHundredData = GiveHundredData or BaseClass(BaseCtrl)
function GiveHundredData:__init()
    if GiveHundredData.Instance ~= nil then
		Debuger.LogError("[GiveHundredData] attempt to create singleton twice!")
		return
	end
	GiveHundredData.Instance = self
    self.config = Config.randactivityconfig_1_auto.give_hundred
end
function GiveHundredData:GetConfig()
    return self.config
end
function GiveHundredData:PassNum(type)
    local pass_num = 0
    local curr_num = 0
    if type == 1 then
        pass_num, curr_num = FightData.Instance:GetCanFightAndTotal()
        pass_num = pass_num * 2 --每关固定送两个
    elseif type == 2 then
        local list, _ = SevenDayData.Instance:GetRewardList()
        -- CommonItemId.SingleDraw
        -- CommonItemId.FifthCompany
        if list then
            for i, v in ipairs(list) do
                if SevenDayData.Instance:GetRewardFlag(v.seq) == 1 then
                    if v.reward_item[0].item_id == CommonItemId.SingleDraw then
                        pass_num = pass_num + (1 * v.reward_item[0].num)
                    elseif v.reward_item[0].item_id == CommonItemId.FifthCompany then
                        pass_num = pass_num + (5 * v.reward_item[0].num)
                    end
                end
            end
        end
    elseif type == 3 then
        if ServerActivityData.Instance:GetLoginRewardFlag(4) == -1 then
            pass_num = 5
        end
    elseif type == 4 then
        for i,v in ipairs(ServerActivityData.Instance:GetScoreData()) do
            if RewardData.Instance:GetScoreRewardFlag(v.seq) == 1 then
                for k, v2 in pairs(v.reward_item) do
                    if v2.item_id == CommonItemId.SingleDraw then
                        pass_num = pass_num + (1 * v2.num)
                    elseif v2.item_id == CommonItemId.FifthCompany then
                        pass_num = pass_num + (5 * v2.num)
                    end
                end
            end
        end
    elseif type == 5 then
        local flag = RewardData.Instance.anti_fraud_info.flag
        pass_num = flag == 1 and 1 or 0
    end
    return pass_num, curr_num
end