-- 周期活动第三期活动信息展示

PeriodActivityInfoView = PeriodActivityInfoView or DeclareView("PeriodActivityInfoView", "period_activity/period_activity_info_view")
function PeriodActivityInfoView:LoadCallback(param_t)
    if TableIsEmpty(param_t) or TableIsEmpty(param_t.config) then
        return
    end
    local panel_name = "ShowPanel" .. param_t.config.show_type
    local panel = self[panel_name]
    if panel then
        panel:SetObjActive(true)
        panel:SetInfo(param_t)
    end
end 






PeriodActivityInfoPanel = PeriodActivityInfoPanel or DeclareMono("PeriodActivityInfoPanel", UIWFlushPanel)

function PeriodActivityInfoPanel:SetInfo(param_t)
    local data = param_t.config
    self.data = data
    self.RedPoint:SetNum(0)
    UH.SetText(self.Desc,data.desc)
    self.Grid:SetData(DataHelper.FormatItemList(data.reward_item))
    local times_func = PeriodActivityData.Instance:GetTimesHandle(data.mod)
    if times_func ~= nil then
        UH.SetText(self.Times,Format(Language.PeriodActivity.TimesTip,times_func()))
    else
        UH.SetText(self.Times,Language.PeriodActivity.TimesTitle)
    end
    if param_t.is_open then
        self:FlushRemindView(data.mod)
    end
    UH.SetInter(self.BtnInter,param_t.is_open)
end

function PeriodActivityInfoPanel:FlushRemindView(mod)
    if self.handle == nil then
        self.handle = RegisterRemind(mod,function(num)
            self.RedPoint:SetNum(num)
        end, true)
    end
end

function PeriodActivityInfoPanel:OnDestroy()
    UIWFlushPanel.OnDestroy(self)
    if self.data ~= nil then
        UnRegisterRemind(self.data.mod,self.handle)
    end
end

function PeriodActivityInfoPanel:OnClickEnter()
    if IsEmpty(self.data.mod) then
        return
    end
    if FightCtrl.IsActFB() then
		return
	end
    if not PeriodActivityData.Instance:IsOpen(self.data.mod) then
        PublicPopupCtrl.Instance:Error(Language.PeriodActivity.EnterError1)
        return
    end
    if ActivityRandData.Instance:OnClickHandle(self.data.mod) then
        ViewMgr:CloseView(PeriodActivityView)
    else
        ViewMgr:OpenViewByKey(self.data.mod)
    end
    ViewMgr:CloseView(PeriodActivityInfoPanel)
end

function PeriodActivityInfoPanel:OnClickClose()
    ViewMgr:CloseView(PeriodActivityInfoView)
end