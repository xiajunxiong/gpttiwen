FightFieldBossView = FightFieldBossView or DeclareView("FightFieldBossView", "fight/fight_hill_sea",Mod.FightFieldBoss.Main)

function FightFieldBossView:FightFieldBossView()
    self.data = FightFieldBossData.Instance
	FightFieldBossCtrl.Instance:SendWildBossReq(1)
	FightFieldBossCtrl.Instance:SendShanHaiBossInfoReq(1)
    SocietyCtrl.Instance:SendServerFirstKillReq(4,24)
end

function FightFieldBossView:LoadCallback(param_t)
    local open_param = param_t.open_param or 1
    local data_list = self.data:GetBtnList()
    for i=1,self.BtnList:Length() do
        self.BtnList[i]:SetData(data_list[i])
        self.BtnList[i]:Toggle(open_param == i)
    end
end

function FightFieldBossView:CloseCallback()
    self.data:Initialization()
    self.data.auto_to_boss_smdata.boss_pos = {}
    self.data.auto_to_boss_smdata.auto_to_seq = -1
end

function FightFieldBossView:OnClickClose()
    ViewMgr:CloseView(FightFieldBossView)
end

function FightFieldBossView:OnClickHelp()
    PublicPopupCtrl.Instance:HelpTip(186)
end

--===========================FightFieldBossPanel===========================
FightFieldBossPanel = FightFieldBossPanel or DeclareMono("FightFieldBossPanel", UIWFlushPanel)

function FightFieldBossPanel:FightFieldBossPanel()
    self.data = FightFieldBossData.Instance
    self.data_cares = {
        {data = self.data.wild_boss_data,func = self.FlushTimesView,init_call = true},
        {data = self.data.wild_boss_data,func = self.FlushListView,init_call = false},
        {data = self.data.field_boss_data,func = self.FlushListView,init_call = false},
    }
end

function FightFieldBossPanel:Awake()
    UIWFlushPanel.Awake(self)
    self.TabsList:SetDataList(self.data:GetFieldBossPageList())
end

function FightFieldBossPanel:FlushListView()
    self.List:SetDataList(self.data:GetFieldBossList(self.data:GetFieldBossType() or 1))
    SocietyCtrl.Instance:SendServerFirstKillReq(3,self.data:GetFieldBossType() + 6)
    self.WildBossTip:SetObjActive(self.data.field_boss_data.wild_number ~= 0)
    self.Tips:SetObjActive(self.data.field_boss_data.wild_number ~= 0)
end

function FightFieldBossPanel:FlushTimesView()
    local TimesTip = Language.FightFieldBoss.TimesTip[1]
    local times,target_num = self.data:GetFieldBossTimes()
    local color = times >= target_num and COLORSTR.Red7 or COLORSTR.Red15
    local all_wild_rate = Config.monster_yewaiboss_auto.other[1].all_wild_rate
    UH.SetText(self.Times,Format(TimesTip.Times,color,Valve(times,target_num),target_num))
    UH.SetText(self.Tips,Format(TimesTip.Tips,all_wild_rate))
    UH.SetText(self.WildBossTip,Language.FightFieldBoss.WildBossTip)
end

----------------------------FightFieldBossPage----------------------------
FightFieldBossPage = FightFieldBossPage or DeclareMono("FightFieldBossPage", UIWidgetBaseItem)

function FightFieldBossPage:SetData(data)
    UH.SetText(self.Name,data.name)
    self.BtnInter.Interactable = FunOpen.Instance:GetFunIsOpened(data.mod)
    self.Block:SetActive(not self.BtnInter.Interactable)
    UIWidgetBaseItem.SetData(self, data)
end

function FightFieldBossPage:Toggle(toggle)
    self.BtnToggle.isOn = toggle
end

function FightFieldBossPage:OnClickItem()
    local _,open_tip = FunOpen.Instance:GetFunIsOpened(self.data.mod)
    PublicPopupCtrl.Instance:Center(open_tip)
end

----------------------------FightFieldBossBtn----------------------------
FightFieldBossBtn = FightFieldBossBtn or DeclareMono("FightFieldBossBtn", UIWidgetBaseItem)

function FightFieldBossBtn:SetData(data)
    UH.SetText(self.Name,Format("%s-%s",data.min_level_show,data.max_level_show).. Language.Common.Level)
    UIWidgetBaseItem.SetData(self, data)
end

function FightFieldBossBtn:OnClickItem()
    FightFieldBossData.Instance:SetFieldBossType(self.data)
end

function FightFieldBossBtn:Click()
    self.Toggle:Trigger()
end

----------------------------FightFieldBossItem----------------------------
FightFieldBossItem = FightFieldBossItem or DeclareMono("FightFieldBossItem", UIWidgetBaseItem)

function FightFieldBossItem:FightFieldBossItem()
    self.data_instance = FightFieldBossData.Instance
end

function FightFieldBossItem:SetData(data)
    local number = FightFieldBossData.Instance:GetWildNumber(data.seq)
    number = number == 0 and data.ordinary_number or number
    local config = FightFieldBossData.Instance:GetBossConfig(number)
    local npc_config = Cfg.NpcById(config.npc_id)
    self.monster_group = npc_config.param1
    local is_wild = number > FightFieldBossData.Instance:GetMaxNumber()

    UH.SetText(self.Name,npc_config.name)
    UH.SetIcon(self.Icon,npc_config.npc_icon)
    UH.SetText(self.Level,MonsterData.GetLevel(self.monster_group) .. Language.Common.Level)
    UH.SpriteName(self.TitleImg,is_wild and [[ShenHongMoJiDiXin]] or [[ShenHuangMoJiDiXin]])
    UH.SpriteName(self.BGImg,is_wild and [[KuangBao]] or [[Tiao]])
    self.Grid:SetData(DataHelper.FormatItemList(config.reward))
    self.WildImg:SetActive(is_wild)

    UIWidgetBaseItem.SetData(self, data)
    self:FlushTiemView(data)
    self:SetVideoView(data)
end

function FightFieldBossItem:FlushTiemView(data)
    self.Time:CloseCountDownTime()
    local refresh_time = FightFieldBossData.Instance:GetNextRefreshTime(data.seq)
    if refresh_time ~= 0 then
        self.Time:StampTime(refresh_time,TimeType.Type_Special_0,Language.FightFieldBoss.TimeTip)
        self.Time:SetCallBack(function()
            self:FlushDescView(self.data)
        end)
        self.IHandler.Interactable = false
    else
        self:FlushDescView(data)
        self.IHandler.Interactable = true
    end
end

function FightFieldBossItem:FlushDescView(data)
    local scene_config = MapData.Instance:GetMapShowData(data.scene_id) or {}
    self.Time:SetTime(Format(Language.FightFieldBoss.Desc,scene_config.name or ""))
end

function FightFieldBossItem:OnClickEnter()
    local refresh_time = FightFieldBossData.Instance:GetNextRefreshTime(self.data.seq)
    if refresh_time ~= 0 then
        PublicPopupCtrl.Instance:Center(Language.FightFieldBoss.TimeTip2)
        return
    end
    if self.care_handle then
        LoginData.Instance:BaseData():Uncare(self.care_handle)
        self.care_handle = nil
    end
    if TeamCtrl.Instance:Data():InTeam() then
        if not TeamCtrl.Instance:Data():IsLeader() then
            PublicPopupCtrl.Instance:Center(ErrorText[57])
            return
        end
    end
    if SceneData.Instance:SceneId() ~= self.data.scene_id then
        SceneLogic.Instance:AutoToPos(self.data.scene_id,nil,function()
            self:OnOnCrossSever()
        end,nil,true)
    else
        self:OnOnCrossSever()
    end
end

function FightFieldBossItem:OnOnCrossSever()
    if not LoginData.Instance:IsOnCrossSever() then
        LoginCtrl.Instance:SendCrossStartReq()
        self.care_handle = LoginData.Instance:BaseData():Care(BindTool.Bind(self.SendCrossStartReq, self),"is_on_crosssever")
    else
        self:SendCrossStartReq()
    end
end

function FightFieldBossItem:SendCrossStartReq()
    if self.auto_pos_care_handle == nil then
        self.auto_pos_care_handle = FightFieldBossData.Instance.auto_to_boss_smdata:Care(function ()
            self:AutoToFieldBossPos()
        end, "boss_pos")
    end
    self.delay_send_get_pos = TimeHelper:AddDelayTimer(function ()
        FightFieldBossData.Instance.auto_to_boss_smdata.auto_to_seq = self.data.seq
        FightFieldBossCtrl.Instance:SendWildBossReq(2, self.data.seq)
    end, 1)

    -- PublicPopupCtrl.Instance:Center(Language.FightFieldBoss.DescTip)
    -- ViewMgr:CloseView(FightFieldBossView)
end

function FightFieldBossItem:AutoToFieldBossPos()
    if self.data_instance.auto_to_boss_smdata.auto_to_seq == self.data.seq and not TableIsEmpty(self.data_instance.auto_to_boss_smdata.boss_pos) then
        local number = FightFieldBossData.Instance:GetWildNumber(self.data.seq)
        number = number == 0 and self.data.ordinary_number or number
        local config = FightFieldBossData.Instance:GetBossConfig(number)
        
        local beh = function ()
            local npc_vo_list = VoMgr:MonsterVoList()
            for key, value in pairs(npc_vo_list) do
                if value.id == config.npc_id then
                    local scene_obj = SceneCtrl.Instance:Objs():GetObjectByObjId(key)
                    if scene_obj ~= nil then
                        scene_obj:InteractionBehavior()
                        break
                    end
                end
            end
        end
        
        SceneLogic.Instance:AutoToPos(nil, self.data_instance.auto_to_boss_smdata.boss_pos, beh, true)

        ViewMgr:CloseView(FightFieldBossView)
    end
end

function FightFieldBossItem:OnDestroy()
    TimeHelper:CancelTimer(self.enter_time)
    if self.handle_care then
        BattleVideo.Instance:SimpleInfo():Uncare(self.handle_care)
        self.handle_care = nil
    end
    if self.care_handle then
        LoginData.Instance:BaseData():Uncare(self.care_handle)
        self.care_handle = nil
    end
    if self.auto_pos_care_handle then
        FightFieldBossData.Instance.auto_to_boss_smdata:Uncare(self.auto_pos_care_handle)
        self.auto_pos_care_handle = nil
    end
    if self.delay_send_get_pos then
        TimeHelper:CancelTimer(self.delay_send_get_pos)
        self.delay_send_get_pos = nil
    end
end
--===============================录像相关===============================
function FightFieldBossItem:SetVideoView(data)
    local video = BattleVideo.Instance:GetVideoConfig(BATTLE_MODE.BATTLE_MODE_WORLD_BOSS,self.monster_group)
	if video then
		if self.handle_care then
            BattleVideo.Instance:SimpleInfo():Uncare(self.handle_care)
        end
        self.handle_care = BattleVideo.Instance:SimpleInfo():Care(BindTool.Bind(self.FlushGrayView, self, video))
        self:FlushGrayView(video)
        BattleCtrl.BattleReportSimpleInfo(video.monster_group_id, 0)
	end
end

function FightFieldBossItem:FlushGrayView(video)
	self.ViewGray.enabled = BattleVideo.Instance:GetSimpleInfo(video.monster_group_id, 0) == 0
end

function FightFieldBossItem:OnClickPlayVideo()
    if self.ViewGray.enabled == true then
        PublicPopupCtrl.Instance:Center(Language.DreamNotes.NotHasPlayVideo)
        return
    end
    local video = BattleVideo.Instance:GetVideoConfig(BATTLE_MODE.BATTLE_MODE_WORLD_BOSS,self.monster_group)
	if video ~= nil then
        BattleVideo.Instance:PlayVideoCfg(video)
        BattleCtrl.BattleReportDetailInfo(video.monster_group_id, 0)
	end
end

--这个不要改动，n多地方用到
----------------------------FightFieldBossRewardItem----------------------------
FightFieldBossRewardItem = FightFieldBossRewardItem or DeclareMono("FightFieldBossRewardItem", UIWidgetBaseItem)

function FightFieldBossRewardItem:SetData(data)
    local item = Item.Create(data.item_data or data)
    self.Call:SetData(item)
    if self.Name ~= nil then
        if self.IsQuality then
            UH.SetText(self.Name,item:QuaName(self.IsShallow))
        else
            UH.SetText(self.Name,item:Name())
        end
    end
    if self.Rare then
        self.Rare:SetActive(BagData.IsRare(item.item_id))
    end
    if self.Treasures then
        self.Treasures:SetActive(BagData.IsTreasures(item.item_id))
    end
    UIWidgetBaseItem.SetData(self, data)
end