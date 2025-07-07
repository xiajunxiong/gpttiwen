GuildEmbattleMsgPanelZone = GuildEmbattleMsgPanelZone or DeclareMono("GuildEmbattleMsgPanelZone", UIWFlushPanel)
function GuildEmbattleMsgPanelZone:GuildEmbattleMsgPanelZone()
    self.embattle_data = GuildEmbattleData.Instance
    self.data_cares = {
        {data = self.embattle_data:BattleData(), func = self.onFlush},
        {data = self.embattle_data:ViewData(), func = self.onFlush, keys = {"flush"}}
    }
    --根据信息刷新，是否可观战是否已胜利
    self.time = 1
end
function GuildEmbattleMsgPanelZone:Awake()
    UIWFlushPanel.Awake(self)
    self:onFlush()
end
function GuildEmbattleMsgPanelZone:OnEnable()
    UIWFlushPanel.OnEnable(self)
end
function GuildEmbattleMsgPanelZone:OnDisable()
    --关闭计时器
end
function GuildEmbattleMsgPanelZone:onFlush()
    local battle_data = self.embattle_data:BattleData()
    for i = 1, 3 do
        self.SuccObj[i]:SetActive(false)
        self.FightingObj[i]:SetActive(true)
        self.ZoneToggle[i].isOn = battle_data.area == i
        UH.SetText(self.NameTex, "")
    end
    self:FlushShow()
    BattleData.Instance:ViewInfo().embattle_watching = false
end
function GuildEmbattleMsgPanelZone:FlushShow()
    local battle_data = self.embattle_data:BattleData()
    -- local list = self.embattle_data:FighterInfo()
    -- if list then
        --local info = list[battle_data.index]
        local data = battle_data.data
        local info = nil
        if data then
            info = data[1]
        end
        if info and info.battle_result then
            local oppo = self.embattle_data:OpponetInfo(info.guild_id)
            if oppo then
                LogDG("对战双方信息", oppo)
                LogDG("当前选择信息", info)
                for i, v in ipairs(info.battle_result) do
                    UH.SetText(self.DescTex[i], Language.GuildEmbattle.Fighting)
                    if oppo[1] and v == oppo[1].guild_id then
                        UH.SetText(self.NameTex[i], oppo[1].name)
                        self.SuccObj[i]:SetActive(true)
                        self.FightingObj[i]:SetActive(false)
                    elseif oppo[2] and v == oppo[2].guild_id then
                        UH.SetText(self.NameTex[i], oppo[2].name)
                        self.SuccObj[i]:SetActive(true)
                        self.FightingObj[i]:SetActive(false)
                    elseif v == -1 then
                        UH.SetText(self.DescTex[i], Language.GuildEmbattle.TieGame)
                    end
                end
            else
                LogError("对战信息空")
            end
        else
            LogError("初始观战信息空")
        end
    -- end
end
function GuildEmbattleMsgPanelZone:OnToggleArrow()
    -- body
end
function GuildEmbattleMsgPanelZone:ClickZone(area)
    local battle_data = self.embattle_data:BattleData()
    local is_can_watch, battle_id = self.embattle_data:IsCanWatch(area, battle_data.data)
    if area == battle_data.area then
        return
    end
    if is_can_watch then
        BattleData.Instance:ViewInfo().embattle_watching = true
        -- BattleCtrl.BattleReportStopWatching()
        BattleCtrl.Instance:AudienceLeaveBattle()
        TimeHelper:AddDelayTimer(
            function()
                -- BattleCtrl.BattleReportDetailInfo(5000)
                BattleCtrl.Instance:SendWatchBattleReq(battle_id, 1)
                battle_data.area = area
            end,
            self.time
        )
    else
        PublicPopupCtrl.Instance:Center(Language.GuildEmbattle.NoWatchBattle)
        self.ZoneToggle[battle_data.area].isOn = true
        LogError("不满足观战条件")
    end
end
function GuildEmbattleMsgPanelZone:OnClickZoneA()
    self:ClickZone(1)
end
function GuildEmbattleMsgPanelZone:OnClickZoneB()
    self:ClickZone(2)
end
function GuildEmbattleMsgPanelZone:OnClickZoneC()
    self:ClickZone(3)
end

---------------------------------------GuildEmbattleHonorPanel-------------------------------------
GuildEmbattleHonorPanel = GuildEmbattleHonorPanel or DeclareMono("GuildEmbattleHonorPanel", UIWFlushPanel)

function GuildEmbattleHonorPanel:OnEnable()
	UIWFlushPanel.OnEnable(self)
    local process = GuildEmbattleData.Instance:BattleProcess().process
    if GUILD_HONOR_BATTLE_PROCESS.WEEK_START == process or GUILD_HONOR_BATTLE_PROCESS.FINISH == process then
        self.HonorInter.Interactable = not (GuildEmbattleData.Instance:Base().fighter_num == 0)
        UH.SetText(self.LabelHonor, Language.GuildEmbattle.CheckInfo)
    else
        UH.SetText(self.LabelHonor, Language.GuildEmbattle.CheckNameList)
    end
end

function GuildEmbattleHonorPanel:OnClickHonor()
    AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ClickGeneral1)
    local process = GuildEmbattleData.Instance:BattleProcess().process
    if GUILD_HONOR_BATTLE_PROCESS.WEEK_START == process or GUILD_HONOR_BATTLE_PROCESS.FINISH == process then
        -- local lastinfo = GuildEmbattleData.Instance:LastInfo()
        if GuildEmbattleData.Instance:Base().fighter_num == 0 then
            PublicPopupCtrl.Instance:Center(Language.GuildEmbattle.ErrorLast)
        else
            ViewMgr:OpenView(GuildEmbattleMsg)
        end
    else
        ViewMgr:OpenView(GuildEmbattleNameList)
    end 
end