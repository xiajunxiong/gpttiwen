ActHalloweenView = ActHalloweenView or DeclareView("ActHalloweenView", "serveractivity/act_halloween/act_halloween",Mod.Act_Halloween.Main)
VIEW_DECLARE_LEVEL(ActHalloweenView, ViewLevel.L1)
VIEW_DECLARE_MASK(ActHalloweenView, ViewMask.BgBlock)

function ActHalloweenView:ActHalloweenView()
end

function ActHalloweenView:LoadCallback(param_t)
    self.Panels[ ActHalloweenData.Instance:GetShowStyle()]:SetActive(true)
end

function  ActHalloweenView:OnCloseClick()
    ViewMgr:CloseView(ActHalloweenView)
end

--Panel
ActHalloweenPanel = ActHalloweenPanel or DeclareMono("ActHalloweenPanel", UIWFlushPanel)
function ActHalloweenPanel:ActHalloweenPanel()
end

function ActHalloweenPanel:Awake()
    local cfg=Config.activity_main_auto.spring_festival
    for k, v in pairs(cfg) do
        if self.Acts[k] ~=nil then
            self.Acts[k]:SetData(v)
        end
    end 
end

--Item
ActHalloweenItem = ActHalloweenItem or DeclareMono("ActHalloweenItem", UIWidgetBaseItem)

function ActHalloweenItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    UH.SetText(self.TxtDesc,data.desc)
    self:FlushRemindView(data.mod_key)
end

function ActHalloweenItem:FlushRemindView(mod_key)
    local mod=TabMod(mod_key)
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

function ActHalloweenItem:OnGoTo()
    ViewMgr:OpenViewByKey(self.data.mod_key)
end

function ActHalloweenItem:OnDisable()
    Remind.Instance:UnRegisterGroup(TabMod(self.data.mod_key), self.handle)
end

function ActHalloweenItem:OnDestroy()
    UIWFlushPanel.OnDestroy(self)
    Remind.Instance:UnRegisterGroup(TabMod( self.data.mod_key), self.handle)
end
