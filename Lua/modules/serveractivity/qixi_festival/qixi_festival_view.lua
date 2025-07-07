QixiFestivalView = QixiFestivalView or DeclareView("QixiFestivalView", "serveractivity/qixi_festival/qixi_festival", Mod.QixiFestival.Main)
VIEW_DECLARE_LEVEL(QixiFestivalView, ViewLevel.L1)
VIEW_DECLARE_MASK(QixiFestivalView, ViewMask.BgBlock)
function QixiFestivalView:QixiFestivalView()

end

function QixiFestivalView:OnCloseClick()
    ViewMgr:CloseView(QixiFestivalView)
end

----------------------------QixiFestivalPanel----------------------------
QixiFestivalPanel = QixiFestivalPanel or DeclareMono("QixiFestivalPanel", UIWFlushPanel)
function QixiFestivalPanel:QixiFestivalPanel()
    self.Data = QixiFestivalData.Instance
    self.language = Language.QixiFestival
    self.data_cares = {
        {data = ActivityData.Instance:GetActivityEvent(), func = self.FlushAll},
    }
end

function QixiFestivalPanel:Awake()
    UIWFlushPanel.Awake(self)
end

function QixiFestivalPanel:FlushAll()
    self.QixiFestivalItem1:SetData(self.Data:Config(1))
    self.QixiFestivalItem2:SetData(self.Data:Config(2))
end

function QixiFestivalPanel:OnDestroy()
    UIWFlushPanel.OnDestroy(self)
end

--------------QixiFestivalItem--------------
QixiFestivalItem = QixiFestivalItem or DeclareMono("QixiFestivalItem", UIWidgetBaseItem)
function QixiFestivalItem:QixiFestivalItem()
    self.Data = QixiFestivalData.Instance
    self.language = Language.QixiFestival
end

function QixiFestivalItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    local is_open = ActivityData.IsOpen(data.act_type)
    local open_time = ActivityData.GetStartStampTime(data.act_type)
    if is_open and open_time > 0 then
        local open_t = os.date("*t", open_time)
        local end_t = os.date("*t", ActivityData.GetSwitchStampTime(data.act_type) - 1)
        UH.SetText(self.TxtTime, Format(self.language.Time, open_t.month, open_t.day, end_t.month, end_t.day))
        self:FlushRemindView(TabMod(data.mod_key))
    else
        UH.SetText(self.TxtTime, self.language.Btn[2])
        self.Remind:SetNum(0)
    end
    UH.SetText(self.TxtGo, self.language.Btn[is_open and 1 or 2])
end

function QixiFestivalItem:OnGoClick()
    local is_open = ActivityData.IsOpen(self.data.act_type)
    if is_open then
        ViewMgr:OpenViewByKey(self.data.mod_key)
    else
        PublicPopupCtrl.Instance:Center(Language.Activity.NotOpenTip)
    end
end

function QixiFestivalItem:FlushRemindView(mod)
    if self.handle == nil then
        self.handle = Remind.Instance:RegisterGroup(mod, function()
            self.Remind:SetNum(Remind.Instance:GetGroupNum(mod))
        end, true)
    else
        self.Remind:SetNum(Remind.Instance:GetGroupNum(mod))
    end
end

function QixiFestivalItem:OnDestroy()
    Remind.Instance:UnRegisterGroup(TabMod(self.data.mod_key), self.handle)
end