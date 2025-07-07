--===========================HillSeaPanel===========================
HillSeaPanel = HillSeaPanel or DeclareMono("HillSeaPanel", UIWFlushPanel)

function HillSeaPanel:HillSeaPanel()
    self.data = FightFieldBossData.Instance
    self.data_cares = {
        {data = self.data.hill_sea_data,func = self.FlushTimesView},
        {data = self.data.hill_sea_data,func = self.FlushListView},
    }
    self.Time:SetCallBack(function()
	    FightFieldBossCtrl.Instance:SendShanHaiBossInfoReq(1)
    end)
end

function HillSeaPanel:FlushListView()
    self.List:SetData(self.data:GetHillSeaList())
end

function HillSeaPanel:FlushTimesView()
    local TimesTip = Language.FightFieldBoss.TimesTip[2]
    local times,target_num = self.data:GetHillSeaTimes()
    UH.SetText(self.Tips,Format(TimesTip.Tips,target_num))
    local color = times >= target_num and COLORSTR.Red7 or COLORSTR.Red15
    UH.SetText(self.Times,Format(TimesTip.Times,color,Valve(times,target_num),target_num))
    self.Time:StampTime(self.data:GetHillSeaRefreshTime(),TimeType.Type_Special_1,Language.FightFieldBoss.HillSeaFlushTime,"")
end

----------------------------FightHillSeaItem----------------------------
FightHillSeaItem = FightHillSeaItem or DeclareMono("FightHillSeaItem", UIWidgetBaseItem)

function FightHillSeaItem:SetData(data)
    local npc_config = Cfg.NpcById(data.npc_id)
    local is_deeath = FightFieldBossData.Instance:IsDeeathNum(data)
    local info = FightFieldBossData.Instance:GetSingleBossInfo(data.npc_id)

    UH.SetText(self.Name,npc_config.name)
    UH.SetIcon(self.Icon,npc_config.npc_icon)
    UH.SpriteName(self.Image,[[_LocGradeType]] .. data.grade_type)
    self.Grid:SetData(self:GetGridList(DataHelper.FormatItemList(data.reward),is_deeath))
    UH.SetText(self.Times,Format(Language.FightFieldBoss.DeathNum,info.death_num,data.fight_times))

    self.BtnInter.Interactable = not is_deeath
    self.monster_group = npc_config.param1
    self.Complete:SetActive(is_deeath)

    local limit_level = Config.monster_shxyboss_auto.initiate_level[data.grade_type].initiate_level
    UH.SetText(self.NotOpenTip,Format(Language.Common.LevelLimit,limit_level))
    self.NotOpenTip:SetObjActive(RoleData.Instance:GetRoleLevel() < limit_level)

    UIWidgetBaseItem.SetData(self, data)
    self:SetVideoView(data)
end

function FightHillSeaItem:GetGridList(data_list,is_deeath)
    for i,v in ipairs(data_list) do
        v.is_grey = is_deeath
    end
    return data_list
end

function FightHillSeaItem:OnClickEnter()
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
    local scene_id = FightFieldBossData.Instance:GetSceneId(self.data.npc_id)
    if SceneData.Instance:SceneId() ~= scene_id then
        SceneLogic.Instance:AutoToPos(scene_id,nil,function()
            self:OnOnCrossSever()
        end,nil,true)
    else
        self:OnOnCrossSever()
    end
end

function FightHillSeaItem:OnOnCrossSever()
    if not LoginData.Instance:IsOnCrossSever() then
        LoginCtrl.Instance:SendCrossStartReq()
        self.care_handle = LoginData.Instance:BaseData():Care(BindTool.Bind(self.SendCrossStartReq, self),"is_on_crosssever")
    else
        self:SendCrossStartReq()
    end
end

function FightHillSeaItem:SendCrossStartReq()
    PublicPopupCtrl.Instance:Center(Language.FightFieldBoss.DescTip)
    ViewMgr:CloseView(FightFieldBossView)
end

function FightHillSeaItem:OnDestroy()
    TimeHelper:CancelTimer(self.enter_time)
    if self.handle_care then
        BattleVideo.Instance:SimpleInfo():Uncare(self.handle_care)
        self.handle_care = nil
    end
    if self.care_handle then
        LoginData.Instance:BaseData():Uncare(self.care_handle)
        self.care_handle = nil
    end
end
--===============================录像相关===============================
function FightHillSeaItem:SetVideoView(data)
    local video = BattleVideo.Instance:GetVideoConfig(BATTLE_MODE.BATTLE_MODE_SHXY,self.monster_group)
	if video then
		if self.handle_care then
            BattleVideo.Instance:SimpleInfo():Uncare(self.handle_care)
        end
        self.handle_care = BattleVideo.Instance:SimpleInfo():Care(BindTool.Bind(self.FlushGrayView, self, video))
        self:FlushGrayView(video)
        BattleCtrl.BattleReportSimpleInfo(video.monster_group_id, 0)
	end
end

function FightHillSeaItem:FlushGrayView(video)
	self.ViewGray.enabled = BattleVideo.Instance:GetSimpleInfo(video.monster_group_id, 0) == 0
end

function FightHillSeaItem:OnClickPlayVideo()
    if self.ViewGray.enabled == true then
        PublicPopupCtrl.Instance:Center(Language.DreamNotes.NotHasPlayVideo)
        return
    end
    local video = BattleVideo.Instance:GetVideoConfig(BATTLE_MODE.BATTLE_MODE_SHXY,self.monster_group)
	if video ~= nil then
        BattleVideo.Instance:PlayVideoCfg(video)
        BattleCtrl.BattleReportDetailInfo(video.monster_group_id, 0)
	end
end