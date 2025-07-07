-- ============= 天下第一比武大会界面 ============
UniverseTeamArenaView = UniverseTeamArenaView or DeclareView("UniverseTeamArenaView", "universe_team_arena/universe_team_arena")
VIEW_DECLARE_LEVEL(UniverseTeamArenaView, ViewLevel.L0)
function UniverseTeamArenaView:UniverseTeamArenaView()
    self:InitReqData()
end

function UniverseTeamArenaView:LoadCallback(param_t)
    
end

function UniverseTeamArenaView:CloseCallback()
    UniverseTeamArenaData.Instance.is_jump_match = false
end

-- 请求需要的数据
function UniverseTeamArenaView:InitReqData()
    -- 请求我的战队基本信息
    UniverseTeamArenaCtrl.Instance:SendCSUTAMyTeamQualificationScheduleReq()
end

function UniverseTeamArenaView:OnCloseClick()
    ViewMgr:CloseView(UniverseTeamArenaView)
end

function UniverseTeamArenaView:OnHelpClick()
    PublicPopupCtrl.Instance:HelpTip(Config.language_cfg_auto.textdesc[205].desc)
end



UniverseTeamArenaPanel = UniverseTeamArenaPanel or DeclareMono("UniverseTeamArenaPanel", UIWFlushPanel)
function UniverseTeamArenaPanel:UniverseTeamArenaPanel()
    self.data = UniverseTeamArenaData.Instance
end

function UniverseTeamArenaPanel:Awake()
    UIWFlushPanel.Awake(self)
    self:FlushTabBtn()
    local index = self.data.base_data.season_idx
    UH.SetText(self.Title, string.format(Language.UTAPanel.Text11, DataHelper.GetDaXie(index)))

    if self.data.is_jump_match then
        local state = self.data:ActState()
        if state == UTACfg.ActStatus.Knockout then
            self.TabToggles[2].isOn = true
        elseif state == UTACfg.ActStatus.Second then
            self.TabToggles[3].isOn = true
        end
    else
        self.TabToggles[1].isOn = true
    end
end

function UniverseTeamArenaPanel:FlushTabBtn()
    self.TabItem3:SetShow(self.data:IsSecond())
end

UniverseTeamArenaTabBtn = UniverseTeamArenaTabBtn or DeclareMono("UniverseTeamArenaTabBtn")
function UniverseTeamArenaTabBtn:SetShow(value)
    if self.IsGray then
        self.Interactor.Interactable = value
    end
    self.Mask:SetActive(not value)
end

function UniverseTeamArenaTabBtn:OnMaskClick(_type)
    if _type == 3 then
        PublicPopupCtrl.Instance:Center(Language.UTAPanel.Tip6)
    elseif _type == 4 then
        PublicPopupCtrl.Instance:Center(Language.UTAPanel.Tip7)
    end
end


UniverseTeamArenaSceneMenu = UniverseTeamArenaSceneMenu or DeclareMono("UniverseTeamArenaSceneMenu", UIWFlushPanel)
function UniverseTeamArenaSceneMenu:OnEnable()
    UIWFlushPanel.OnEnable(self)
    self.info = UniverseTeamArenaData.Instance.ready_scene_data
    UH.SetText(self.SideName, self.info.side_team_name or "")
    self.Time:StampTime(self.info.start_battle_timestamp or 0,TimeType.Type_Special_8)

    local match_type = self.info.match_type
    local round_index = self.info.round_index
    if match_type == UTACfg.MatchType.Out then
        round_index = UniverseTeamArenaData.Instance:ConvertRoundIndex(round_index)
    end
    local match_name = UniverseTeamArenaData.Instance:FormatMatchName(match_type, round_index)
    UH.SetText(self.MatchName, match_name)
end

function UniverseTeamArenaSceneMenu:OnDestroy()
    UIWFlushPanel.OnDestroy(self)
    self.Time:CloseCountDownTime()
end

function UniverseTeamArenaSceneMenu:OnLookInfoClick()
    local side_team_id_t = self.info.side_team_id_t
    if not TableIsEmpty(side_team_id_t) then
        ViewMgr:OpenView(UTABattleArray,{team_id_t = side_team_id_t, team_name = self.info.side_team_name or ""})
    end
end

function UniverseTeamArenaSceneMenu:OnHelpClick()
    ViewMgr:OpenView(UniverseTeamArenaMain, {tabIndex = 1})
end


-- 天下第一结算界面
UTAFinishPassView = UTAFinishPassView or DeclareView("UTAFinishPassView", "universe_team_arena/uta_finish_pass")
function UTAFinishPassView:LoadCallback(param_t)
    UH.SetText(self.Tip1, param_t.tip or "")
    self.time_count = 6
    self.time_ht = TimeHelper:AddRunTimer(function ()
        self.time_count = self.time_count - 1
        UH.SetText(self.BtnText, string.format(Language.UTAPanel.Text12, self.time_count))
        if self.time_count == 0 then
            self:OnOkClick()
        end
    end,1,6)
    self.WinEffect:SetActive(param_t.is_win == 1)
    self.FailEffect:SetActive(param_t.is_win == 0)
    self.DrawEffect:SetActive(param_t.is_win == 2)
end

function UTAFinishPassView:OnOkClick()
    ViewMgr:CloseView(UTAFinishPassView)
end

function UTAFinishPassView:CloseCallback()
    TimeHelper:CancelTimer(self.time_ht)
end

-- 天下第一活动图标sub
UTAIconSub = UTAIconSub or DeclareMono("UTAIconSub", UIWFlushPanel)
function UTAIconSub:UTAIconSub()
    self.data = UniverseTeamArenaData.Instance
    self.data_cares = {
        {data = self.data.base_data, func = self.FlushPanel, init_call = false},
    }
end

function UTAIconSub:Awake()
    UIWFlushPanel.Awake(self)
    self:FlushPanel()
end

function UTAIconSub:FlushPanel()
    local status = self.data:ActState()
    local tip2 = ""
    local round_index = self.data.base_data.round_index
    if status == UTACfg.ActStatus.Knockout then
        tip2 = Language.UTAPanel.Text13
        round_index = self.data:ConvertRoundIndex(round_index)
        tip2 = tip2 .. tostring(Language.UTA.OutRoundName2[round_index])
    elseif status == UTACfg.ActStatus.Second then
        tip2 = Language.UTAPanel.Text14
        tip2 = tip2 .. tostring(Language.UTA.WinRoundName[round_index])
    else
        tip2 = tostring(Language.UTA.ActStatus[status])
    end
    UH.SetText(self.Tips2, tip2)

    local tip1 = self.data:IsInProgress() and Language.UTAPanel.Text15 or Language.UTAPanel.Text16
    if status > 3 then
        tip1 = Language.UTAPanel.Text17
    end
    UH.SetText(self.Tips1, tip1)
end

function UTAIconSub:OnIconClick()
    UniverseTeamArenaCtrl.Instance:OpenUta()
end


-- 冠军雕像
UTAStatueCh = UTAStatueCh or BaseClass(MainCh)

function UTAStatueCh:InteractionBehavior()
    --LogError("点击了天下第一冠军雕像",self.vo)

    local btn_list = {}
    local button = NpcDialogBehBtnData.FuncCreate(
		Language.UTA.NPCBtn.Btn5,function()
		ViewMgr:OpenView(UtaChampionTeam)
    end)
	button:SetIconType(NpcIconType.Act)
    table.insert(btn_list, button)

    local button = NpcDialogBehBtnData.FuncCreate(
        Language.UTA.NPCBtn.Btn6,function()
        UniverseTeamArenaData.Instance.is_look_last_all_match = true
        ViewMgr:OpenView(UTAAllMatch)
    end)
    table.insert(btn_list, button)
    
    local role_info = self.vo.role_info -- ==== SCGetUTAChampionRecordAck协议
    local season_idx = ColorStr(DataHelper.GetDaXie(role_info.season_idx + 1),COLORSTR.Yellow1)
    local team_name = ColorStr(role_info.team_name, COLORSTR.Yellow1)
    local role_name = ColorStr(role_info.role_name, COLORSTR.Yellow1)
    local server_name = ColorStr(UniverseTeamArenaData.Instance:GetServerName(role_info.server_id, role_info.plat_id), COLORSTR.Yellow1)
    local talk_text = string.format(Language.UTAPanel.Tip8, season_idx,team_name,role_name,server_name)

    local appearance = UniverseTeamArenaCtrl.Instance:ChampionFormatAppearance(role_info)
    appearance.ride_id = 0
    appearance.bicycle_index = -1
    appearance.model_card_flag = 0
    appearance.is_use_huanshou = 0

    NpcDialogCtrl.Instance:NpcDialogView({
        talk_list = {
            [1] = {
                talker = 0,
                talk_text = talk_text,
                type = NpcDialogConfig.TalkSpecial.UTAShow,
                is_npc_talk = true,
                appearance = appearance,
                name = role_info.role_name,
            },
        },
        beh_btn_list = btn_list,
    })
end


function UTAStatueCh:GetTargetPos()
    local pos = self:GetPos()
    return GameScene.SamplePos(pos)
end

function UTAStatueCh:Type()
    return SceneObjType.UTAStatue
end
