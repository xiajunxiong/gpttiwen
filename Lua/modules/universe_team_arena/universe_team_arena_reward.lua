-- ========== 天下第一比武大会：：赛事奖励 ==========
UniverseTeamArenaReward = UniverseTeamArenaReward or DeclareView("UniverseTeamArenaReward", "universe_team_arena/universe_team_arena_reward")
function UniverseTeamArenaReward:OnCloseClick()
    ViewMgr:CloseView(UniverseTeamArenaReward)
end

function UniverseTeamArenaReward:OnTipClick()
    PublicPopupCtrl.Instance:HelpTip(Config.language_cfg_auto.textdesc[207].desc)
end

UniverseTeamArenaRewardPanel = UniverseTeamArenaRewardPanel or DeclareMono("UniverseTeamArenaRewardPanel", UIWFlushPanel)
function UniverseTeamArenaRewardPanel:UniverseTeamArenaRewardPanel()
    self.data = UniverseTeamArenaData.Instance
    self.tab_index = 1
end

function UniverseTeamArenaRewardPanel:Awake()
    UIWFlushPanel.Awake(self)
    local desc = Config.language_cfg_auto.textdesc[207].desc
    UH.SetText(self.Desc, desc)
    self:FlushPanel()
end

function UniverseTeamArenaRewardPanel:FlushPanel()
    local list_data = {}
    if self.tab_index == 1 then
        list_data = self.data:GetMainRewardList()
    else
        list_data = self.data:GetRankRewardList(1)
    end
    self.List:SetData(list_data)
end

function UniverseTeamArenaRewardPanel:OnTabClick(tab_index)
    self.tab_index = tab_index
    self:FlushPanel()
end



UniverseTeamArenaRewardItem = UniverseTeamArenaRewardItem or DeclareMono("UniverseTeamArenaRewardItem", UIWidgetBaseItem)
function UniverseTeamArenaRewardItem:SetData(data)
    UIWidgetBaseItem.SetData(self,data)
    UH.SetText(self.Text, data.text)
    self.RewardList:SetData(data.reward_list)
end



UniverseTeamArenaRewardModelInfo = UniverseTeamArenaRewardModelInfo or DeclareMono("UniverseTeamArenaRewardModelInfo", UIWFlushPanel)
function UniverseTeamArenaRewardModelInfo:UniverseTeamArenaRewardModelInfo()
    self.cfg = UTACfg.ModelShowCfg

    self.is_animtion = {
        true,
        true,
    }
    self.index = 1
end

function UniverseTeamArenaRewardModelInfo:Awake()
    UIWFlushPanel.Awake(self)
    if self.ui_obj == nil then
		self.ui_obj = UIObjDrawer.New()
        self.ModelShow:SetShowData(
			{
				ui_obj = self.ui_obj,
                view = self,
                show_shadow = true,
			}
		)
    end
    self.LimitSign:SetType(PetData.LimitType.Champion)
    self:FlushModel()

    self.time_default_num = 3
    self.time_flush_num = self.time_default_num     -- 隔多少秒刷新一次模型
    self.time_ht = TimeHelper:AddRunTimer(function ()
        self.time_flush_num = self.time_flush_num - 1
        if self.time_flush_num == 0 then
            self:AddIndex(1)
            self.time_flush_num = 3
        end
    end,1)
end

function UniverseTeamArenaRewardModelInfo:OnDestroy()
    UIWFlushPanel.OnDestroy(self)
    TimeHelper:CancelTimer(self.time_ht)
    if self.ui_obj ~= nil then
		self.ui_obj:DeleteMe()
    end
end

function UniverseTeamArenaRewardModelInfo:FlushModel()
    local cfg = self.cfg[self.index]
    local path
    if cfg.reward_type == 1 then
        local ride = IllusionData.Instance:GetFashionInfoById(cfg.reward_id)
        path = DrawerHelper.GetRidePath(ride.res_id)
    else
        local pet_cfg = PetData.Instance:GetPetCfg(cfg.reward_id)
        path = DrawerHelper.GetNpcPath(pet_cfg.res_id)
    end
    self.ui_obj:SetData(path or "")
    if self.is_animtion[self.index] then
        --self.ui_obj:PlayAnim(ChAnimType.Show)
        self.is_animtion[self.index] = false
    end

    if self.Types[self.index] then
        self.Types[self.index].isOn = true
    end
end

function UniverseTeamArenaRewardModelInfo:AddIndex(value)
    local index = self.index + value
    if index < 1 then
        index = #self.cfg
    end
    if index > #self.cfg then
        index = 1
    end
    self.index = index
    self:FlushModel()
end

function UniverseTeamArenaRewardModelInfo:OnBtnClick(value)
    self:AddIndex(value)
    self.time_flush_num = 10
end