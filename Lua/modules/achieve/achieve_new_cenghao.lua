AchieveNewCenghao = AchieveNewCenghao or DeclareView("AchieveNewCenghao","achieve/achieve_new_cenghao")
---TitleTxt=======Text
---BtnTxt=========Text
---TitleImg=======UIImageSpriteSet
---DescTxt========Text
---Stars==========GameObject[5]
function AchieveNewCenghao:AchieveNewCenghao()

end

function AchieveNewCenghao:LoadCallback()
    self.data = AchieveNewData.Instance:GetCurStarData()
    if self.data == nil then return end

    self.can_get_reward = AchieveNewData.Instance:CanGetStageReward()

    self.BtnTxt.text = self.can_get_reward and Language.NewAchieve.GetReward or Language.NewAchieve.Knowed
    local cur_stage = AchieveNewData.Instance:GetCurStage(true)
    local cur_star = AchieveNewData.Instance:GetCurStar(true)
    local stage_full_name = AchieveNewData.GetStageName(cur_stage,cur_star)
    self.TitleImg.SpriteName = AchieveNewData.GetTitleSpriteName(cur_stage)
    self.TitleTxt.text = stage_full_name
    local award_getted = false
    local hint_txt
    if self.can_get_reward then
        hint_txt = string.format(Language.NewAchieve.CenghaoRewardable,stage_full_name)
    else
        local next_star_cfg = AchieveNewData.Instance:GetNextStarData()
        if next_star_cfg == nil then
            hint_txt = Language.NewAchieve.DontBeGreed
            award_getted = true
        else
            hint_txt = string.format(Language.NewAchieve.CenghaoRewarddisable,
                AchieveNewData.GetStageName(next_star_cfg.stage_id,next_star_cfg.star_id)) 
        end
    end
    local data_list = {
        {item_id = CommonItemId.Exp ,num = self.data.reward_exp,award_getted = award_getted},
        {item_id = CommonItemId.Coin ,num = self.data.reward_coin,award_getted = award_getted},
    }
    for _,v in pairs(self.data.reward_item) do
        table.insert(data_list,{item_id = v.item_id,num = v.num,award_getted = award_getted})
    end
    self.list:SetData(data_list)
    self.DescTxt.text = hint_txt
    local cur_star = AchieveNewData.Instance:GetCurStar(true)
    for i=1,cur_star do
        self.Stars[i]:SetActive(true)
    end
end

function AchieveNewCenghao:OnGetAwardClick()
    if self.can_get_reward then
        AchieveCtrl.Instance:ReqeustBossAward()
    end
    self:OnClose()
end

function AchieveNewCenghao:OnClose()
    ViewMgr:CloseView(AchieveNewCenghao)
end

AchieveNewCenghaoItem = AchieveNewCenghaoItem or DeclareMono("AchieveNewCenghaoItem",UIWidgetBaseItem)
---got_oj===GameObject
function AchieveNewCenghaoItem:SetData(data)
    local item_data = Item.New()
    item_data.item_id = data.item_id
    item_data.num = tostring(data.num)
	self.cell:SetData(item_data)
    self.got_oj:SetActive(data.award_getted)
end