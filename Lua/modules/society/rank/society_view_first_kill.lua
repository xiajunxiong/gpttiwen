SocietyViewFirstKill = SocietyViewFirstKill or DeclareMono("SocietyViewFirstKill", UIWFlushPanel)

function SocietyViewFirstKill:SocietyViewFirstKill()
    self.data = SocietyCtrl.Instance:Data()

    self.data_cares = {
        {data = self.data.first_flood_sm, func = self.FlushLevelList, keys = {"type"}, init_call = false},
        {data = self.data.first_flood_sm, func = self.FlushItemList, keys = {"level_id"}, init_call = false}
    }
end

function SocietyViewFirstKill:Awake()
    UIWFlushPanel.Awake(self)
    self:FlushTypeList()
end

function SocietyViewFirstKill:FlushTypeList()
    self.data.first_flood_sm.type = 0
    self.TypeList:SetDataList(Config.first_kill_auto.challengetask)
end

function SocietyViewFirstKill:FlushLevelList()
    self.data.first_flood_sm.level_id = 0
    self.LevelToggleGroup:SetAllTogglesOff()
    self.LevelList:SetDataList(self.data:FirstKillLevelList())
end

function SocietyViewFirstKill:FlushItemList()
    self.ItemList:SetData(self.data:FirstKillItemList())
end

SocietyViewFirstKillTypeItem = DeclareMono("SocietyViewFirstKillTypeItem", UIWidgetBaseItem)

function SocietyViewFirstKillTypeItem:SocietyViewFirstKillTypeItem()
    self.society_data = SocietyCtrl.Instance:Data()
end

function SocietyViewFirstKillTypeItem:SetData(data)
    UH.SetText(self.TypeName,data.name)
    UIWidgetBaseItem.SetData(self, data)
end

function SocietyViewFirstKillTypeItem:Click()
    self.Toggle:Trigger()
end

function SocietyViewFirstKillTypeItem:OnSelect(selected)
    UIWidgetBaseItem.OnSelect(self, selected)
    if selected then
        AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ClickGeneral2)
        self.society_data.first_flood_sm.type = self.data.type
    end
end

SocietyViewFirstKillLevelItem = DeclareMono("SocietyViewFirstKillLevelItem", UIWidgetBaseItem)

function SocietyViewFirstKillLevelItem:SocietyViewFirstKillLevelItem()
    self.society_data = SocietyCtrl.Instance:Data()
end

function SocietyViewFirstKillLevelItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    if data.name and data.name ~= "" then
        UH.SetText(self.LevelDesc,data.name)
    else
        UH.SetText(self.LevelDesc,data.level_desc)
    end
end

function SocietyViewFirstKillLevelItem:Click()
    self.Toggle:Trigger()
end

function SocietyViewFirstKillLevelItem:OnSelect(selected)
    UIWidgetBaseItem.OnSelect(self, selected)
    if selected then
        AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ClickGeneral1)
        self.society_data.first_flood_sm.level_id = self.data.level_id
        SocietyCtrl.Instance:SendServerFirstKillReq()
    end
end

SocietyViewFirstKillItem = DeclareMono("SocietyViewFirstKillItem", UIWidgetBaseItem)

function SocietyViewFirstKillItem:SocietyViewFirstKillItem()
    self.data = SocietyCtrl.Instance:Data()
end

function SocietyViewFirstKillItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)

    self:uncareData()
    self.care_handles = {}

    self.BossName.text = data:BossName()
    UH.SetIcon(self.BossIcon, data:BossIcon(), ICON_TYPE.ITEM)
    self.RewardList:SetData(data:RewardList())
    self.Killers.text = data:Killers()
    self.Round.text = data:Round()
    self.Time.text = data:Time()
    self.Killed:SetActive(data:IsFirstKill())
    local battle_mode = SocietyData.GetFirstKillMode(data.config.type)
    local video = BattleVideo.Instance:GetVideoConfig(battle_mode, data.config.monster_group)
	if video then
        self:careData(BattleVideo.Instance:SimpleInfo(), BindTool.Bind(self.FlushGray, self, video))
        BattleCtrl.BattleReportSimpleInfo(video.monster_group_id, 1)
    end
end

function SocietyViewFirstKillItem:FlushGray(video)
	local can_play = BattleVideo.Instance:GetSimpleInfo(video.monster_group_id, 1)
	self.Gray.enabled = can_play == 0
end

function SocietyViewFirstKillItem:OnDestroy()
    self:uncareData()
end

function SocietyViewFirstKillItem:OnClickPlayVideo()
    local battle_mode = SocietyData.GetFirstKillMode(self.data.config.type)
    local video = BattleVideo.Instance:GetVideoConfig(battle_mode, self.data.config.monster_group)
    if video then
        BattleVideo.Instance:PlayVideoCfg(video)
        BattleCtrl.BattleReportDetailInfo(video.monster_group_id, 1)
    end    
end