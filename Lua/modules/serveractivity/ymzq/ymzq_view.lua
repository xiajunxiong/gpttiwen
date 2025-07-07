-- 月满中秋 中秋活动汇总界面
YmzqView = YmzqView or DeclareView('YmzqView', 'serveractivity/ymzq/ymzq', Mod.MidAutumnFestival.Main)
VIEW_DECLARE_LEVEL(YmzqView, ViewLevel.L1)
VIEW_DECLARE_MASK(YmzqView, ViewMask.BgBlock)

function YmzqView:YmzqView()
end

function YmzqView:LoadCallback(param_t)
end

function YmzqView:OnCloseClick()
    ViewMgr:CloseView(YmzqView)
end

----------------------------YmzqPanel----------------------------
YmzqPanel = YmzqPanel or DeclareMono('YmzqPanel', UIWFlushPanel)
function YmzqPanel:YmzqPanel()
    self.handle = ActivityData.Instance:GetActivityEvent():Care(BindTool.Bind(self.FlushShow, self))
    self:FlushShow()
end

function YmzqPanel:Awake()
    UIWFlushPanel.Awake(self)
end

function YmzqPanel:OnFlush()
end

function YmzqPanel:OnDestroy()
    UIWFlushPanel.OnDestroy(self)
    ActivityData.Instance:GetActivityEvent():Uncare(self.handle)
end

function YmzqPanel:FlushShow()
    local list = ServerActivityData.Instance:GetYmzqList()
    if #list > 2 then
        self.ListLayout.spacing = 153
        self.List.x = 44
    else
        self.ListLayout.spacing = 234
        self.List.x = 47
    end
    self.List:SetData(list)
end

----------------------------YmzqItem----------------------------
YmzqItem = YmzqItem or DeclareMono('YmzqItem', UIWidgetBaseItem)

function YmzqItem:Awake()
    self.is_enable = true
    self:ShowEffect()
end

function YmzqItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    UH.SetText(self.Title1, System.String.New(data.text):Substring(0, 2))
    UH.SetText(self.Title2, System.String.New(data.text):Substring(2))
    local descs = Split(data.desc, ',')
    UH.SetText(self.Desc1, descs[1])
    UH.SetText(self.Desc2, descs[2])
    UH.SetText(self.TxtBtn, Language.Common.GoTo)
    self:FlushRemindView(data.mod)
    --UH.PlayEffect(self.EffectTool, GANEN_EFFEXT_ID[self.data.act_type], self.Effect)
    self:ShowEffect()
    UH.SpriteName(self.Icon, YMZQ_ICON_IMG[self.data.act_type])
end

function YmzqItem:ShowEffect()
    if self.is_enable and self.data then
        if not self.effect_ht then
            self.effect_ht = UH.PlayEffect(self.EffectTool, GANEN_EFFEXT_ID[self.data.act_type])
        end
    end
end

function YmzqItem:FlushRemindView(mod)
    if self.handle == nil and self.gameObject.activeSelf then
        self.handle = Remind.Instance:RegisterGroup(mod, function()
            local num = Remind.Instance:GetGroupNum(mod)
            if self.RedPos == nil then
                if self.handle then
                    Remind.Instance:UnRegisterGroup(self.data.mod, self.handle)
                end
                return
            end
            self.RedPos:SetNum(num)
        end, true)
    end
end

function YmzqItem:GoToAct()
    if (self.data.is_close == 0) then
        ViewMgr:CloseView(YmzqView)
    end
    ViewMgr:OpenViewByKey(self.data.mod_key)
end

function YmzqItem:OnDisable()
    Remind.Instance:UnRegisterGroup(self.data.mod, self.handle)
end

function YmzqItem:OnDestroy()
    Remind.Instance:UnRegisterGroup(self.data.mod, self.handle)
end

YMZQ_ICON_IMG = {
    [ACTIVITY_TYPE.RAND_YUE_XI_DENG_MI] = "WenDaZhuangShi",
    [ACTIVITY_TYPE.RAND_YUE_XI_LEI_CHONG] = "LeiChongZhuangShi",
    [ACTIVITY_TYPE.RAND_ZHUI_YUE_SHANG_CHENG] = "ShangChengZhuangShi",
}

-- 感恩活动特效id
GANEN_EFFEXT_ID = {
    [ACTIVITY_TYPE.RAND_YUE_XI_DENG_MI] = 4164006,
    [ACTIVITY_TYPE.RAND_YUE_XI_LEI_CHONG] = 4164004,
    [ACTIVITY_TYPE.RAND_ZHUI_YUE_SHANG_CHENG] = 4164005,
}

-----特效id
YMZQ_EFFEXT_ID = {[ACTIVITY_TYPE.RAND_YUE_XI_DENG_MI] = 6165313, [ACTIVITY_TYPE.RAND_YUE_XI_LEI_CHONG] = 6165311, [ACTIVITY_TYPE.RAND_ZHUI_YUE_SHANG_CHENG] = 6165312}

