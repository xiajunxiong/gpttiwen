CourageChallengeRewardView = CourageChallengeRewardView or DeclareView("CourageChallengeRewardView", "courage_challenge/courage_challenge_reward")

function CourageChallengeRewardView:OnClickClose()
    ViewMgr:CloseView(CourageChallengeRewardView)
    ViewMgr:FlushView(CourageChallengeView)
end

--===============================CourageChallengeRewardPanel===============================
CourageChallengeRewardPanel = CourageChallengeRewardPanel or DeclareMono("CourageChallengeRewardPanel", UIWFlushPanel)

function CourageChallengeRewardPanel:CourageChallengeRewardPanel()
	self.data = CourageChallengeData.Instance
	self.data_cares = {
		{data = self.data.reward_red_piont,func = self.FlushListView,init_call = true},
    }
end

function CourageChallengeRewardPanel:FlushListView()
    self.layer_level = self.data:GetSelectData().layer_level
    self.List:SetData(self.data:GetLayerRewardList(self.layer_level))
    UH.SetText(self.StarNum,self.data:GetLayerNum(self.layer_level))
    self:FlushStarRewardView()
end

function CourageChallengeRewardPanel:FlushStarRewardView()
    local star_num = self.data:GetLayerNum(self.layer_level)
    local reward_config = self.data:GetLayerRewardList(self.layer_level)
    local config = self.data:GetStarRewardConfig(self.layer_level)
    
    self.Progress:SetProgress(star_num / config.max_star + 0.02)
    UH.SpriteName(self.GiftIcon,"_LocBtnGift" .. config.icon)

    local reward_list,count = self:GetFillAmountList(reward_config)
    self.StarList:SetData(reward_list)
    local spacing = (576 - 55 * count) / (count - 1)
    self.StarLayout.spacing = Vector2.New(spacing,0)

    local flag = self.data:GetLayerRewardEndFlag(self.layer_level)
    self.Effect:SetActive(flag == 0 and star_num >= config.max_star)

    self.GiftFinish:SetActive(flag == 1)
end

function CourageChallengeRewardPanel:GetFillAmountList(config)
    local list = {{is_hide = true,star = 0}}
    for k,v in pairs(config) do
        table.insert(list,v)
    end
    list[#list].is_hide = true
    --判断数量
    local count = #list - 6
    if count > 0 then
        for i=1,count do
            table.remove(list,#list - i)
        end
    end
    return list,#list
end

function CourageChallengeRewardPanel:OnClickGift()
    if not self.Effect.activeSelf then
        local config = self.data:GetStarRewardConfig(self.layer_level)
        local reward_list = {}
        if config then
            reward_list = DataHelper.FormatItemList(config.reward)
        end
        if #reward_list == 1 then
            CellClicks.BagGridNoButton({GetData = function()
                return reward_list[1]
            end})
        else
            ViewMgr:OpenView(SilkRoadPreview,{pos = Vector3.New(280,-70,0),reward_list = reward_list})
        end
    else
        CourageChallengeCtrl.Instance:SendCourageChallengeReq(2,self.layer_level)
        self.Effect:SetActive(false)
    end
end

--===============================CourageChallengeRewardItem===============================
CourageChallengeRewardItem = DeclareMono("CourageChallengeRewardItem", UIWidgetBaseItem)

function CourageChallengeRewardItem:SetData(data)
    UH.SetText(self.StarNum,data.star)
    data.item = Item.New(data.reward[0])
    self.Call:SetData(data.item)
    UH.SetText(self.Name,data.item:QuaName())
    local cur_num = CourageChallengeData.Instance:GetLayerNum(data.layer_level)
    local flag = CourageChallengeData.Instance:GetLayerRewardFlag(data.layer_level,data.seq) or 0
    if self.red_point == nil then
        self.red_point = UiRedPoint.New(self.BtnInter.gameObject, Vector3.New(74,21,0))
    end
    self.BtnInter.Interactable = cur_num >= data.star
    self.Finish:SetActive(flag == 1)
    self.BtnReward:SetActive(flag == 0)
    self.red_point:SetNum(self.BtnInter.Interactable and 1 or 0)
    UH.SetText(self.BtnName,self.BtnInter.Interactable and Language.Recharge.BtnState[0] or Language.Achieve.NotReach)
    UIWidgetBaseItem.SetData(self, data)
end

function CourageChallengeRewardItem:OnClickReward()
    CourageChallengeCtrl.Instance:SendCourageChallengeReq(1,self.data.layer_level,self.data.seq)
end

--===============================CourageChallengeRewardStarItem===============================
CourageChallengeRewardStarItem = DeclareMono("CourageChallengeRewardStarItem", UIWidgetBaseItem)

function CourageChallengeRewardStarItem:SetData(data)
    UH.SetText(self.Num,data.star)
    if data.is_hide ~= nil then
        self.Image:SetActive(false)
    end
end