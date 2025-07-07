AchieveNewDetail = AchieveNewDetail or DeclareView("AchieveNewDetail","achieve/achieve_new_detail_popup")
---BtnTxt=========Text
--CompleteBgObj==GameObject
--UncomBgObj=====GameObject
--RewardedBgObj==GameObject
--EffectPlayer===EffectTool
function AchieveNewDetail:AchieveNewDetail()
    AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ViewOpenL2)
end

function AchieveNewDetail:LoadCallback()
    self.data = AchieveNewData.Instance:GetClickAchieve()
    if self.data == nil then return end
    self.name.text = self.data.name
    self.desc.text = self.data.desc
    local complete_flag = AchieveNewData.Instance:CheckAchieveIsComplete(self.data.index)
    local award_getted = AchieveNewData.Instance:IsAchieveAwardGetted(self.data.index)
    self.can_get_reward = complete_flag and not award_getted
    self.not_complete = not complete_flag and not award_getted
    if award_getted then
        self.BtnTxt.text = Language.NewAchieve.Knowed
    else
        self.BtnTxt.text = complete_flag and Language.NewAchieve.GetReward or self.data.button_name
    end
    if award_getted then
        self.RewardedBgObj:SetActive(true)
        self.UncomBgObj:SetActive(false)
        self.CompleteBgObj:SetActive(false)
        self.IconSprite:SetObjActive(false)
        self.name.color = COLORS.Red1
    elseif complete_flag then
        self.RewardedBgObj:SetActive(false)
        self.UncomBgObj:SetActive(false)
        self.CompleteBgObj:SetActive(true)
        self.IconSprite:SetObjActive(true)
        self.IconSprite.SpriteName = self.data.icon
        self.name.color = COLORS.Red1
        self.EffectPlayer:Play("10010292")
    else
        self.RewardedBgObj:SetActive(false)
        self.UncomBgObj:SetActive(true)
        self.CompleteBgObj:SetActive(false)
        self.IconSprite:SetObjActive(true)
        self.IconSprite.SpriteName = self.data.icon
        self.name.color = COLORS.Gray1
        local value = AchieveNewData.Instance:GetAchieveCurProg(self.data.index)
        local max = AchieveNewData.Instance:GetAchieveMaxProg(self.data.index)
        if max ~= "" and max ~= 0 then self.prog:SetProgress(value / tonumber(max)) end
        self.prog:SetText(tostring(value).." / "..tostring(max))
    end


    local data_list = {}
    if self.data.reward_exp > 0 then
        table.insert(data_list,{item_id = CommonItemId.Exp ,num = self.data.reward_exp,award_getted = award_getted})
    end
    if self.data.reward_coin > 0 then
        table.insert(data_list,{item_id = CommonItemId.Coin ,num = self.data.reward_coin,award_getted = award_getted})
    end

    for _,v in pairs(self.data.reward_item or RO_EMTPY_TAB) do
        table.insert(data_list,{item_id = v.item_id,num = v.num,award_getted = award_getted})
    end
    self.data__item_list = DataHelper.FormatItemList(data_list,1)
    self.list:SetData(data_list)
end

function AchieveNewDetail:OnGetAwardClick()
    if self.can_get_reward then
        AchieveCtrl.Instance:ReqeustRecvAward(self.data.seq)
        --PublicPopupCtrl.Instance:RewardExpand2(self.data__item_list, 1)
    elseif self.not_complete then
        if self.data.open_panel ~= nil and self.data.open_panel ~= "" then
            ViewMgr:OpenViewByKey(self.data.open_panel)
        end
        if self.data.npc ~= nil and self.data.npc ~= "" then
            SceneLogic.Instance:AutoToNpc(self.data.npc,nil,nil,true)
        end
        ViewMgr:CloseView(AchieveNewView)
    end
    self:OnClose()
end

function AchieveNewDetail:OnClose()
    ViewMgr:CloseView(AchieveNewDetail)
end

AchieveNewDetailItem = AchieveNewDetailItem or DeclareMono("AchieveNewDetailItem",UIWidgetBaseItem)
function AchieveNewDetailItem:SetData(data)
    self.got_oj:SetActive(data.award_getted)

    local item_data = Item.New()
    item_data.item_id = data.item_id
    item_data.num = tostring(data.num)
	self.cell:SetData(item_data)
end