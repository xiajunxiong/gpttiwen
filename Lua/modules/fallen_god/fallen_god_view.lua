FallenGodView = FallenGodView or DeclareView("FallenGodView", "fallen_god/fallen_god", Mod.Challenge.FallenGod)
VIEW_DECLARE_LEVEL(FallenGodView, ViewLevel.L0)

function FallenGodView:FallenGodView()
    self.AudioOpen = CommonAudio.ViewOpenL1
    self.AudioClose = CommonAudio.ViewCloseL1
end

function FallenGodView:LoadCallback()
    SocietyCtrl.Instance:SendCrossGetPersonAllRankList(PERSON_RANK_TYPE.FALLEN_GOD)
end

function FallenGodView:OnClickClose()
    ViewMgr:CloseView(FallenGodView)
end

FallenGodPanel = FallenGodPanel or DeclareMono("FallenGodPanel", UIWFlushPanel)
function FallenGodPanel:FallenGodPanel()
    self.data_cares = {
        {data = FallenGodData.Instance.base_info, func = self.FlushView, init_call = false}
    }
    self.select_sn = 0
end

function FallenGodPanel:Awake()
    UIWFlushPanel.Awake(self)

    local boss_list = FallenGodData.Instance:GetBossList()
    self.BossList:SetData(boss_list)
    self.RewardList:SetData(boss_list)
    self:FlushInfo()
    for i, v in ipairs(boss_list) do
        if i == #boss_list or not FallenGodData.Instance:BossIsPassBySn(v.sn) then
            self.BossList:ClickItem(i)
            break
        end
    end

    self.ActTimer:SetShowCallBack(function(t)
        if tonumber(t.day) > 0 then
            return string.format(Language.FallenGod.TimeFormat1, tonumber(t.day), tonumber(t.hour))
        elseif tonumber(t.hour) > 0 then
            return string.format(Language.FallenGod.TimeFormat2, tonumber(t.hour))
        elseif tonumber(t.min) > 0 or tonumber(t.s) > 0 then
            local min = tonumber(t.min) > 0 and tonumber(t.min) or 1
            return string.format(Language.FallenGod.TimeFormat3, min)
        end
        return ""
    end)
    self.ActTimer:SetCallBack(BindTool.Bind1(self.OnTimeOver, self), nil)
    self:FlushTime()
end

function FallenGodPanel:OnDestroy()
    UIWFlushPanel.OnDestroy(self)
    Delete(self, "ui_obj")
end

function FallenGodPanel:FlushView()
    local boss_list = FallenGodData.Instance:GetBossList()
    self.BossList:SetData(boss_list)
    self.RewardList:SetData(boss_list)
    self:FlushInfo()
end

function FallenGodPanel:OnClickRank()
    ViewMgr:OpenView(FallenGodRankView)
end

function FallenGodPanel:OnClickTips()
    PublicPopupCtrl.Instance:HelpTip(FallenGodData.Instance:GetHelpId())
end

function FallenGodPanel:ShowModel(npc_seq)
    local npc_cfg = FallenGodData.NpcBySeq(npc_seq)
    if self.ui_obj == nil then
        self.ui_obj = UIChDrawer.New()
        self.ModelShow:SetShowData({ui_obj = self.ui_obj, view = self})
    end
    self.ui_obj:SetMainSolo(DrawerHelper.GetNpcPath(npc_cfg.res_id))
end

function FallenGodPanel:FlushInfo()
    local boss_cfg = FallenGodData.Instance:GetBossCfg(self.select_sn)
    --解锁状态
    local is_unlock = FallenGodData.Instance:IsBossUnlock(boss_cfg.sn)
    self.Inactive:SetActive(not is_unlock)
    UH.SetInter(self.BtnGoInter, is_unlock)
    if not is_unlock then --未解锁
        if boss_cfg.pass_type == 0 then
            UH.SetText(self.InactiveText, Format(Language.FallenGod.UnlockStr2, boss_cfg.name))
        else
            local last_cfg = FallenGodData.Instance:GetBossCfg(boss_cfg.pass_sn)
            UH.SetText(self.InactiveText, Format(Language.FallenGod.UnlockStr1, last_cfg.name, boss_cfg.name))
        end
    end
    --助战次数
    local join_times = FallenGodData.Instance:GetHelpTimes(boss_cfg.sn)
    local show_str = boss_cfg.help_name .. ColorStr(join_times .. "/" .. boss_cfg.help_times, COLORSTR.Green4)
    UH.SetText(self.BoxText, show_str)
    UH.SetText(self.BossName, boss_cfg.name)
    self:ShowModel(boss_cfg.npc_seq)

    self.HelpReward:SetData(DataHelper.FormatItemList(boss_cfg.help_reward))
end

function FallenGodPanel:OnClickGo()
    if not FallenGodData.Instance:IsActOpen() then
        PublicPopupCtrl.Instance:Center(Language.FallenGod.Error4)
        return
    end
    local boss_cfg = FallenGodData.Instance:GetBossCfg(self.select_sn)
    if not FallenGodData.Instance:IsBossUnlock(boss_cfg.sn) then --未解锁
        if boss_cfg.pass_type == 0 then
            -- PublicPopupCtrl.Instance:Center(Language.FallenGod.Error3)
            local info = {
                content = Language.FallenGod.GoToTips1,
                confirm = {
                    func = function()
                        BigDipperData.Instance:SetOpenIndex(8)
                        ViewMgr:OpenViewByKey(Mod.Challenge.BigDipper)
                        ViewMgr:CloseView(FallenGodView)
                        PublicPopupCtrl.Instance:CloseDialogTips()
                    end
                },
            }
            PublicPopupCtrl.Instance:DialogTips(info)
        else
            PublicPopupCtrl.Instance:Center(Language.FallenGod.Error1)
        end
        return
    end
    SceneLogic.Instance:AutoToNpc(boss_cfg.npc_seq, nil, nil, true)
    ViewMgr:CloseView(FallenGodView)
end

function FallenGodPanel:OnSelectBoss(data)
    self.select_sn = data.sn
    self:FlushInfo()
end

function FallenGodPanel:OnShowBlock()
    self.HelpShow:SetActive(false)
end

function FallenGodPanel:OnClickBox()
    self.HelpShow:SetActive(true)
end

function FallenGodPanel:FlushTime()
    local act_status, next_time = FallenGodData.Instance:CheckNextWeekTime()
    local time_str = act_status == 1 and Language.FallenGod.TimeStr2 or Language.FallenGod.TimeStr1
    self.ActTimer:StampTime(next_time, TimeType.Type_Special_4, time_str)
end

function FallenGodPanel:OnTimeOver()
    self:FlushTime()
end

FallenGodBossItem = FallenGodBossItem or DeclareMono("FallenGodBossItem", UIWidgetBaseItem)
function FallenGodBossItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)

    local npc_cfg = FallenGodData.NpcBySeq(data.npc_seq)
    UH.SetText(self.Name, npc_cfg.name)
    UH.SetIcon(self.Icon, npc_cfg.npc_icon)

    local is_unlock = FallenGodData.Instance:IsBossUnlock(data.sn)
    local is_pass = FallenGodData.Instance:BossIsPassBySn(data.sn)
    self.Lock:SetActive(not is_unlock)
    UH.SetInter(self.Inter, is_unlock)
    self.Pass:SetActive(is_unlock and is_pass)
end

function FallenGodBossItem:Click()
    if self.Selector ~= nil then
        self.Selector.isOn = true
    end
end

FallenGodRewardItem = FallenGodRewardItem or DeclareMono("FallenGodRewardItem", UIWidgetBaseItem)
function FallenGodRewardItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    UH.SetText(self.Title, data.reward_name)
    local show_list = {}
    local is_pass = FallenGodData.Instance:BossIsPassBySn(data.sn)
    for i, v in ipairs(DataHelper.FormatItemList(data.item_list_1)) do
        table.insert(show_list, {is_pass = is_pass, item = v})
    end
    self.ItemList:SetData(show_list)
end

FallenGodCellItem = FallenGodCellItem or DeclareMono("FallenGodCellItem", UIWidgetBaseItem)
function FallenGodCellItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    self.Cell:SetData(data.item)
    self.HasGet:SetActive(data.is_pass)
    self.Cell:MakeGrey(data.is_pass)
end
