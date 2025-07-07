DaZhanHongTuView = DaZhanHongTuView or DeclareView("DaZhanHongTuView", "serveractivity/DaZhanHongTuView",Mod.RandActivity.DaZhanHongTu)
function DaZhanHongTuView:DaZhanHongTuView()
    self.data = DaZhanHongTuData.Instance
    --活动界面打开时申请一下信息
    DaZhanHongTuCtrl.GetDaZhanHongTuInfo()
end

function DaZhanHongTuView:LoadCallback()
    UH.SetText(self.ConditionText, Format(Language.DaZhanHongTu.ConditionFormat, RechargeData.Instance:GoldToMoney(Config.dazhanhongtu_auto.reward[1].acc_price)))
    local item_cfg = Config.dazhanhongtu_auto.reward[1].reward_item[0]
    local item = Item.Init(item_cfg.item_id, item_cfg.num, item_cfg.is_bind)
    self.ItemCell:SetData(item)
    
    self.LeftTimeMeter:SetCallBack(function ()
        self:OnClickClose()
    end)
    self.LeftTimeMeter:SetShowCallBack(function (t)
        if t.day > 0 then
            return Format(Language.DaZhanHongTu.CountDownFormatDay, t.day, t.hour, t.min)
        else
            return Format(Language.DaZhanHongTu.CountDownFormatHour, t.hour, t.min, t.s)
        end
    end)
    self.LeftTimeMeter:CreateActTime(ACTIVITY_TYPE.RAND_DA_ZHAN_HONG_TU)
end

function DaZhanHongTuView:OnClickClose()
    ViewMgr:CloseView(DaZhanHongTuView)
end
-----------------------------------------
DaZhanHongTuPanel = DaZhanHongTuPanel or DeclareMono("DaZhanHongTuPanel", UIWFlushPanel)
function DaZhanHongTuPanel:DaZhanHongTuPanel()
    self.data = DaZhanHongTuData.Instance
    self.data_cares = {
        {data = self.data.sm_data, func = self.FlushView, keys = {"flush"}}
    }
end

function DaZhanHongTuPanel:FlushView()
    UH.SetText(self.RechargeNum, Format(Language.DaZhanHongTu.RechargedNum, RechargeData.Instance:GoldToMoney(self.data:GetHasRechargeNum())))
    if self.data:DaZhanHongTuRewardCanGet() then
        if self.red_point == nil then
            self.red_point = UiRedPoint.New(self.RedPointPar)
        end
        self.red_point:SetNum(1)
    elseif self.red_point ~= nil then
        self.red_point:SetNum(0)
    end
end