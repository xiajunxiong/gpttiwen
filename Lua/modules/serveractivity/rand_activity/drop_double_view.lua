DropDoubleView = DropDoubleView or DeclareView("DropDoubleView","serveractivity/drop_double_view",Mod.RandActivity.DropDouble)
VIEW_DECLARE_LEVEL(DropDoubleView, ViewLevel.L1)
function DropDoubleView:DropDoubleView()
    self.Board:SetData(self:GetType(), ActivityRandData.GetName(ACTIVITY_TYPE.RAND_DROP_DOUBLE))
end

function DropDoubleView:LoadCallback()
    self.Time:CreateActTime(ACTIVITY_TYPE.RAND_DROP_DOUBLE,TimeType.Type_Special_4)
    local config = Config.drop_double_auto.burst_rate_double
    local reward_list = ServerActivityData.Instance:GetActOpenDayCfg(ACTIVITY_TYPE.RAND_DROP_DOUBLE,config,"seq")
    self.RewardList:SetData(reward_list)
end

function DropDoubleView:OnClickClose()
    ViewMgr:CloseView(DropDoubleView)
end

function DropDoubleView:OnClickEnter()
    ViewMgr:OpenView(FightView)
    ViewMgr:CloseView(DropDoubleView)
end

function DropDoubleView:OnClickHelp()
    PublicPopupCtrl.Instance:HelpTip(Config.language_cfg_auto.textdesc[44].desc)
end

----------------------------DropDoubleItem----------------------------
DropDoubleItem = DropDoubleItem or DeclareMono("DropDoubleItem", UIWidgetBaseItem)

function DropDoubleItem:SetData(data)
    local config = self:GetFbList(data.fb_id + 1)
    local map_png = MapData.MapIdToIcon(config.scene_id)
    local cfg = SceneData.Instance:GetSceneCfg(config.scene_id) or {}
    local list = {[0] = 1,[1] = 3,[2] = 2}
    local type_name = Language.Fight.TypeNames[2][list[config.type] or 1]
    UH.SetIcon(self.MapIcon, map_png, ICON_TYPE.MAP)
    UH.SetText(self.Name,Format("%s-%s",cfg.name,type_name))
    UIWidgetBaseItem.SetData(self, data)
end

function DropDoubleItem:GetFbList(seq)
    return Config.crystal_scene_auto.crystal_config[seq] or {}
end