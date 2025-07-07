ShengDanMiLingView = ShengDanMiLingView or DeclareView("ShengDanMiLingView","serveractivity/sheng_dan_mi_ling/sheng_dan_mi_ling",Mod.Act_Halloween.ShanDanMiLing)
function ShengDanMiLingView:OnCloseClick()
    ViewMgr:CloseView(ShengDanMiLingView)
end


ShengDanMiLingPanel = ShengDanMiLingPanel or DeclareMono("ShengDanMiLingPanel", UIWFlushPanel)
function ShengDanMiLingPanel:ShengDanMiLingPanel()
    self.data = ShengDanMiLingData.Instance
    self.data_cares = {
        {data = self.data.act_data, func = self.FlushPanel, init_call = false}
    }
end

function ShengDanMiLingPanel:Awake()
    UIWFlushPanel.Awake(self)
    local cfg = self.data:GetCfg()
    self.list_data1 = {}
    self.list_data2 = {}
    for i,v in pairs(cfg) do
        table.insert(self.list_data1, {seq = v.seq, type = 1, cfg = v})
        table.insert(self.list_data2, {seq = v.seq, type = 2, cfg = v})
    end
    UH.SetText(self.Tip, Language.ShengDanMiLing.Tip)
    self.ActTime:CreateActTime(ACTIVITY_TYPE.RAND_SHENG_DAN_MI_LING,TimeType.Type_Time_3,Language.ShengDanMiLing.ActTime)
    self:FlushPanel()
end

function ShengDanMiLingPanel:FlushPanel()
    self.List1:SetData(self.list_data1)
    self.List2:SetData(self.list_data2)
end


ShengDanMiLingItem = ShengDanMiLingItem or DeclareMono("ShengDanMiLingItem", UIWidgetBaseItem)
function ShengDanMiLingItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    local flag
    if data.type == 1 then
        flag = ShengDanMiLingData.Instance:GetBaseRewardFlag(data.seq)
        self.RewardItem:SetData(Item.Create(data.cfg.reward_item))
        --UH.SetText(self.StateText, string.format(Language.ShengDanMiLing.Text1, DataHelper.GetDaXie(data.seq + 1)))
    else
        flag = ShengDanMiLingData.Instance:GetMiLingRewardFlag(data.seq)
        self.RewardItem:SetData(Item.Create(data.cfg.reward1_item))
        --UH.SetText(self.StateText, Language.ShengDanMiLing.Text2)
    end
    self.On:SetActive(flag == -1)
    self.Off:SetActive(flag ~= -1)
    self.GetBtn:SetActive(flag == 1)
    self.State:SetActive(flag ~= 1)
    UH.SetInter(self.Inter, flag >= 0)
    if flag == -1 then
        UH.SetText(self.StateText, Language.ShengDanMiLing.Text3)
    elseif flag == -2 then
        UH.SetText(self.StateText, "密令已失效")
    else
        UH.SetText(self.StateText, string.format(Language.ShengDanMiLing.Text1, DataHelper.GetDaXie(data.seq + 1)))
    end
end

function ShengDanMiLingItem:OnBoxClick()
    self.RewardInfo:SetActive(not self.RewardInfo.activeSelf)
end

function ShengDanMiLingItem:OnGetClick()
    local req_type = self.data.type == 1 and ShengDanMiLingCtrl.ReqType.BaseReward or ShengDanMiLingCtrl.ReqType.MiLingReward
    ServerActivityCtrl.Instance:SendActivityReq(ACTIVITY_TYPE.RAND_SHENG_DAN_MI_LING,req_type,self.data.seq)
end