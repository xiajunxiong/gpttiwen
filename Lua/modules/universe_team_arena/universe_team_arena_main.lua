-- ================ 天下第一比武大会入口 =================
UniverseTeamArenaMain = UniverseTeamArenaMain or DeclareView("UniverseTeamArenaMain", "universe_team_arena/universe_team_arena_main", Mod.UniverTeamArena.Main)
VIEW_DECLARE_LEVEL(UniverseTeamArenaMain, ViewLevel.L0)
function UniverseTeamArenaMain:UniverseTeamArenaMain()
    self.data = UniverseTeamArenaData.Instance
    self:InitData()
end

function UniverseTeamArenaMain:LoadCallback(param_t)
    param_t = param_t or {}
    if param_t.tabIndex then
        self.Toggles[param_t.tabIndex].isOn = true
    end
    UH.SetText(self.Desc, Config.language_cfg_auto.textdesc[209].desc)

    self.ht_1 = self.data.self_team_base_data:Care(BindTool.Bind(self.FlushPanel, self))
    self.ht_2 = self.data.ready_scene_data:Care(BindTool.Bind(self.FlushPanel, self))
    self.ht_3 = self.data.remind_data:Care(BindTool.Bind(self.FlushRemind, self))

    -- 当自己的信息为空时请求一下
    local self_team_info = self.data:SelfTeamBaseInfo()
    if not self_team_info then
        UniverseTeamArenaCtrl.Instance:SendCSUTAMyTeamQualificationScheduleReq()
    end
    self:FlushPanel()
end

function UniverseTeamArenaMain:CloseCallback()
    self.data.self_team_base_data:Uncare(self.ht_1)
    self.data.ready_scene_data:Uncare(self.ht_2)
    self.data.remind_data:Uncare(self.ht_3)
end

function UniverseTeamArenaMain:FlushPanel()
    local self_team_info = self.data:SelfTeamBaseInfo()
    local match_act_info = self.data:GetMatchActInfo()
    self.Tog4Inter.Interactable = self_team_info ~= nil and match_act_info.status ~= ActStatusType.Close
    self.GoToMask:SetActive(self.Tog4Inter.Interactable == false)
    local is_open = self.data:UTArena2IsOpen()
    self.Tog3Inter.Interactable = is_open
    self.Toggle3Mask:SetActive(not is_open)
    self:FlushRemind()
end

function UniverseTeamArenaMain:FlushRemind()
    local remind_num = self.data:RemindNum()
    self.RedPoint:SetNum(remind_num)
end

function UniverseTeamArenaMain:UnAllToggle()
    for i = 1, self.Toggles:Length() do
        local toggle = self.Toggles[i]
        toggle.isOn = false
    end
end

function UniverseTeamArenaMain:OnToggleClick(desc_type)
    self:OpenPanel(desc_type)
end

function UniverseTeamArenaMain:OnOpenUTAClick()
    local is_open = self.data:UTArena2IsOpen()
    if not is_open then
        PublicPopupCtrl.Instance:Center(Language.UTAPanel.Tip11)
        return
    end
    self.data.is_jump_match = true
    ViewMgr:OpenView(UniverseTeamArenaView)
    self:UnAllToggle()
end

function UniverseTeamArenaMain:OnGotoMaskClick()
    local is_open = self.data:UTArena2IsOpen()
    if not is_open then
        PublicPopupCtrl.Instance:Center(Language.UTAPanel.Tip11)
        return
    end
    local self_team_info = self.data:SelfTeamBaseInfo()
    if not self_team_info then
        PublicPopupCtrl.Instance:Center(Language.UTAPanel.Tip9)
        return
    end
    local match_act_info = self.data:GetMatchActInfo()
    if match_act_info.status == ActStatusType.Close then
        PublicPopupCtrl.Instance:Center(Language.UTAPanel.Tip10)
        return
    end
end

function UniverseTeamArenaMain:OnToggle3Mask()
    local is_open = self.data:UTArena2IsOpen()
    if not is_open then
        PublicPopupCtrl.Instance:Center(Language.UTAPanel.Tip11)
        return
    end
end


function UniverseTeamArenaMain:OnGoToArenaClick()
    ViewMgr:CloseView(UniverseTeamArenaView)
    ViewMgr:CloseView(UniverseTeamArenaMain)
    SceneLogic.Instance:AutoToNpc(UniverseTeamArenaData.Instance:MainNpcId(), nil, false, true)
end

function UniverseTeamArenaMain:OpenPanel(desc_type)
    self.Content:SetActive(true)
    local list_data = self.desc_cfg[desc_type] or {}
    if desc_type == 1 then
        for i = 1, self.TipList:Length() do
            local data = list_data[i]
            self.TipList[i]:SetObjActive(data ~= nil)
            if data then
                self.TipList[i]:SetData(data)
            end
        end
    else
        for i = 1, self.TipList:Length() do
            self.TipList[i]:SetObjActive(false)
        end
        local data = list_data[1]
        if data then
            self.TipList[2]:SetObjActive(true)
            self.TipList[2]:SetData(data)
        end
    end
    local title = Language.ArenaContestNotice.DescTitle[desc_type] or ""
    UH.SetText(self.Title, title)

    for i = 1, self.RewardTipList:Length() do
        self.RewardTipList[i]:SetActive(desc_type == 2)
    end
end

function UniverseTeamArenaMain:ClosePanel()
    self.Content:SetActive(false)
    self:UnAllToggle()
end

function UniverseTeamArenaMain:InitData()
    local desc_cfg = {}
    local cfg = Config.daily_activity_auto.match_notice
    for i,v in pairs(cfg) do
        if v.seq == 1 then
            if not desc_cfg[v.type] then
                desc_cfg[v.type] = {}
            end
            table.insert(desc_cfg[v.type], v)
        end
    end
    self.desc_cfg = desc_cfg
end

function UniverseTeamArenaMain:OnCloseClick()
    ViewMgr:CloseView(UniverseTeamArenaMain)
end

UniverseTeamArenaMainTipItem = UniverseTeamArenaMainTipItem or DeclareMono("UniverseTeamArenaMainTipItem", UIWidgetBaseItem)
function UniverseTeamArenaMainTipItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    UH.SetText(self.Title, data.name)
    local desc_list = Split(tostring(data.language_seq), "|") or {}
    UH.SetText(self.Desc, Config.language_cfg_auto.textdesc[tonumber(desc_list[1])].desc)
    if self.Desc2 and desc_list[2] then
        UH.SetText(self.Desc2, Config.language_cfg_auto.textdesc[tonumber(desc_list[2])].desc)
    end
    local h = self.SelfH + self.Desc.preferredHeight
    self.Tansform:SetSizeWithCurrentAnchors(1,h)
end

UniverseTeamArenaMainTipReward1 = UniverseTeamArenaMainTipReward1 or DeclareMono("UniverseTeamArenaMainTipReward1")
function UniverseTeamArenaMainTipReward1:UniverseTeamArenaMainTipReward1()
    self.cfg = Config.universe_arena_auto
end
function UniverseTeamArenaMainTipReward1:Awake()
    local list_data = self:GetListData()
    for i = 1, self.InfoList:Length() do
        self.InfoList[i]:SetData(list_data[i])
    end
end

function UniverseTeamArenaMainTipReward1:GetListData()
    if self.list_data then
        return self.list_data
    end
    local list_data = {}
    self.list_data = list_data
    local data1 = {
        text = Language.UTAPanel.Text20,
        item_list = DataHelper.FormatItemList(self.cfg.other[1].join_reward)
    }
    table.insert(list_data,data1)
    for i,v in pairs(self.cfg.victory_reward) do
        local data = {}
        data.text = string.format(Language.UTAPanel.Text21,v.victory_times)
        data.item_list = DataHelper.FormatItemList(v.reward)
        table.insert(list_data, data)
    end
    return list_data
end

UniverseTeamArenaMainTipReward1Item = UniverseTeamArenaMainTipReward1Item or DeclareMono("UniverseTeamArenaMainTipReward1Item", UIWidgetBaseItem)
function UniverseTeamArenaMainTipReward1Item:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    data = data or {}
    UH.SetText(self.Text, data.text)
    self.List:SetData(data.item_list or {})
end

UniverseTeamArenaMainTipReward2 = UniverseTeamArenaMainTipReward2 or DeclareMono("UniverseTeamArenaMainTipReward2")
function UniverseTeamArenaMainTipReward2:UniverseTeamArenaMainTipReward2()
    self.data = UniverseTeamArenaData.Instance
end
function UniverseTeamArenaMainTipReward2:Awake()
    local list_data = self.data:GetRankRewardList(1)
    local h = 80 + #list_data * 109
    self.Trans:SetSizeWithCurrentAnchors(1,h)
    self.List:SetData(list_data)
end

UniverseTeamArenaMainTipReward2Item = UniverseTeamArenaMainTipReward2Item or DeclareMono("UniverseTeamArenaMainTipReward2Item", UIWidgetBaseItem)
function UniverseTeamArenaMainTipReward2Item:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    self.List:SetData(data.reward_list)
    UH.SetText(self.Text, data.r_text)
end

UniverseTeamArenaMainTipReward3 = UniverseTeamArenaMainTipReward3 or DeclareMono("UniverseTeamArenaMainTipReward3")
function UniverseTeamArenaMainTipReward3:UniverseTeamArenaMainTipReward3()
    self.data = UniverseTeamArenaData.Instance
end
function UniverseTeamArenaMainTipReward3:Awake()
    local list_data = self.data:GetMainRewardList()
    local h = 80 + #list_data * 109
    self.Trans:SetSizeWithCurrentAnchors(1,h)
    self.List:SetData(list_data)
end

UniverseTeamArenaMainTipReward4 = UniverseTeamArenaMainTipReward4 or DeclareMono("UniverseTeamArenaMainTipReward4")
function UniverseTeamArenaMainTipReward4:UniverseTeamArenaMainTipReward4()
    self.cfg = UTACfg.ModelShowCfg
end
function UniverseTeamArenaMainTipReward4:Awake()
    if self.ui_obj_1 == nil then
		self.ui_obj_1 = UIObjDrawer.New()
        self.ModelShow1:SetShowData(
			{
				ui_obj = self.ui_obj_1,
                view = self,
                show_shadow = true,
			}
		)
    end
    if self.ui_obj_2 == nil then
		self.ui_obj_2 = UIObjDrawer.New()
        self.ModelShow2:SetShowData(
			{
				ui_obj = self.ui_obj_2,
                view = self,
                show_shadow = true,
			}
		)
    end

    local pet_cfg = PetData.Instance:GetPetCfg(self.cfg[2].reward_id)
    local path1 = DrawerHelper.GetNpcPath(pet_cfg.res_id)

    local ride = IllusionData.Instance:GetFashionInfoById(self.cfg[1].reward_id)
    local path2 = DrawerHelper.GetRidePath(ride.res_id)

    self.ui_obj_1:SetData(path1)
    self.ui_obj_2:SetData(path2)
end

function UniverseTeamArenaMainTipReward4:OnDestroy()
    if self.ui_obj_1 ~= nil then
		self.ui_obj_1:DeleteMe()
    end
    if self.ui_obj_2 ~= nil then
		self.ui_obj_2:DeleteMe()
	end
end

UniverseTeamArenaMainTipReward5 = UniverseTeamArenaMainTipReward5 or DeclareMono("UniverseTeamArenaMainTipReward5")
function UniverseTeamArenaMainTipReward5:UniverseTeamArenaMainTipReward5()

end
function UniverseTeamArenaMainTipReward5:Awake()
    local title_list = Split(Config.universe_arena_auto.other[1].title_of_arena, "|")
    local list_data = {}
    for i,v in pairs(title_list) do
        local cfg = Config.titleconfig_auto.title_list[tonumber(v)]
        if cfg then
            table.insert(list_data, cfg)
        end
    end
    self.List:SetData(list_data)
end

UniverseTeamArenaMainTipReward5Item = UniverseTeamArenaMainTipReward5Item or DeclareMono("UniverseTeamArenaMainTipReward5Item", UIWidgetBaseItem)
function UniverseTeamArenaMainTipReward5Item:SetData(data)
    UH.SetIcon(self.Title1, data.res_id, ICON_TYPE.TITLE)
    UH.SetIcon(self.Title2, data.res_id, ICON_TYPE.TITLE)
end

UniverseTeamArenaMainTipReward6 = UniverseTeamArenaMainTipReward6 or DeclareMono("UniverseTeamArenaMainTipReward6")
function UniverseTeamArenaMainTipReward6:UniverseTeamArenaMainTipReward6()
    self.data = UniverseTeamArenaData.Instance
end
function UniverseTeamArenaMainTipReward6:Awake()
    local list_data = {}
    local data = {
        text = Language.UTAPanel.Text23,
        r_text = Language.UTAPanel.Text23,
        reward_list = DataHelper.FormatItemList(self.data.other_cfg.service_reward_show)
    }
    table.insert(list_data, data)

    local h = 80 + #list_data * 109
    self.Trans:SetSizeWithCurrentAnchors(1,h)
    self.List:SetData(list_data)
end