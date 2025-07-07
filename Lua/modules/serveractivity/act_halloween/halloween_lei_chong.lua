HalloweenLeiChong = HalloweenLeiChong or DeclareView("HalloweenLeiChong", "serveractivity/act_halloween/halloween_lei_chong", Mod.Act_Halloween.LeiChong)
VIEW_DECLARE_LEVEL(HalloweenLeiChong, ViewLevel.Lt)
VIEW_DECLARE_MASK(HalloweenLeiChong, ViewMask.BgBlock)
function HalloweenLeiChong:HalloweenLeiChong()
end

function HalloweenLeiChong:LoadCallback(param_t)
    ActHalloweenData.Instance:ClearLeiChongFirstRemind()
end

function HalloweenLeiChong:OnCloseClick()
    ViewMgr:CloseView(HalloweenLeiChong)
end

--Panel 
HalloweenLeiChongPanel = HalloweenLeiChongPanel or DeclareMono("HalloweenLeiChongPanel", UIWFlushPanel)

function HalloweenLeiChongPanel:HalloweenLeiChongPanel()
    self.language=Language.Halloween_Lei_Chong
    self.data = ActHalloweenData.Instance
    self.data_cares = {
        {data = self.data.lei_chong_data, func = self.OnFlush, init_call = false }
    }
    self:CheckOutView()
    self:OnFlush()
end

function HalloweenLeiChongPanel:CheckOutView()
    local cfg = self.data:GetShopCfg()
    self.Npart:SetActive(Config.wanshengleichong_auto.activity_interface[1].is_show == 1)
    self.Cpart:SetActive(Config.wanshengleichong_auto.activity_interface[2].is_show == 1)
    self.c_target = Config.wanshengleichong_auto.activity_interface[1].is_show == 1
end 

function HalloweenLeiChongPanel:OnFlush()
    if self.c_target then 
        self.List:SetData(self.data:GetLeiChongCfg())
    else
        self.CList:SetData(self.data:GetLeiChongCfg())
    end     
end

function HalloweenLeiChongPanel:Awake()
    UIWFlushPanel.Awake(self)
    self.Time:SetShowCallBack(function(t)
        return Format(self.language.time, t.day, t.hour, t.min)
    end)
    self.Time:CreateActTime(ACTIVITY_TYPE.RAND_WAN_SHENG_LEI_CHONG, TimeType.Type_Time_3)

    self.CTime:SetShowCallBack(function(t)
        return Format(self.language.time, t.day, t.hour, t.min)
    end)
    self.CTime:CreateActTime(ACTIVITY_TYPE.RAND_WAN_SHENG_LEI_CHONG, TimeType.Type_Time_3)
end

--Item
HalloweenLeiChongItem = HalloweenLeiChongItem or DeclareMono("HalloweenLeiChongItem", UIWidgetBaseItem)

function HalloweenLeiChongItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    local num = data.acc_recharge_down / 10
    local charge_num = ActHalloweenData.Instance.lei_chong_data.chong_zhi_value
    local fecth_flag = ActHalloweenData.Instance.lei_chong_data.fetch_flag[data.seq]
    UH.SetText(self.TxtNum, num .. Language.Halloween_Lei_Chong.yuan)
    local rewards = {}
    local is_ke_ling_qu = charge_num >= num and fecth_flag == 0
    if data.gold > 0 then
        table.insert(rewards,{item=Item.Init(HuoBi[0],data.gold,1),is_ke_ling_qu=is_ke_ling_qu})
    end
    for k, v in pairs(data.reward_item) do
        table.insert(rewards, {item=Item.Create(v),is_ke_ling_qu=is_ke_ling_qu})
    end
    self.List:SetData(rewards)
    local value = charge_num / num
    self.Pro:SetProgress(value)
    self.Pro:SetText(charge_num .. "/" .. num)
    self.Block:SetActive(is_ke_ling_qu)
    self.ObjFetched:SetActive(fecth_flag == 1)
end

function HalloweenLeiChongItem:GetReward()
    if not BagData.Instance:TryFindEmptyForList(ItemColumnType.Item, self.data.reward_item) then
        PublicPopupCtrl.Instance:Center(Language.Activity.BagFullTip)
        return
    end
    ServerActivityCtrl.Instance:SendActivityReq(ACTIVITY_TYPE.RAND_WAN_SHENG_LEI_CHONG, WAN_SHENG_LEI_CHONG_REQ_TYPE.FETCH, self.data.seq)
end

HalloweenLeiChongRewardItem = HalloweenLeiChongRewardItem or DeclareMono("HalloweenLeiChongRewardItem", UIWidgetBaseItem)
function HalloweenLeiChongRewardItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    self.Cell:SetData(data.item)
    if data.is_ke_ling_qu then
        if self.EffectHandle == nil then
            self.EffectHandle = UH.PlayEffect(self.UiEffect, 3164016)
        end
    else
        if self.EffectHandle ~= nil then
            UH.StopEffect(self.UiEffect, self.EffectHandle)
        end
    end
end

function HalloweenLeiChongRewardItem:OnDestroy()
    UIWidgetBaseItem.OnDestroy(self)
    if self.EffectHandle ~= nil then
        UH.StopEffect(self.UiEffect, self.EffectHandle)
    end
end


