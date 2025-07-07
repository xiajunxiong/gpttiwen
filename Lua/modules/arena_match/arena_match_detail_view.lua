ArenaMatchDetailView = ArenaMatchDetailView or DeclareView("ArenaMatchDetailView", "arena_match/arena_match_detail_view")
function ArenaMatchDetailView:ArenaMatchDetailView()
    self.data = ArenaMatchData.Instance
    self.data_ht = self.data.rank_info_list:Care(BindTool.Bind(self.FlushRankInfo, self))
end

function ArenaMatchDetailView:LoadCallback(param_t)
    param_t = param_t or {}
    self.param_t = param_t
    self.pos = param_t.position or -1
    self.name_list = self.data.arena_list_p[self.pos]
    if not self.name_list then
        return
    end

    self.Bg1:SetActive(param_t.seq == 1)
    self.Bg2:SetActive(param_t.seq == 2)

    local list = self.data:GetOccupyList(param_t.position)
    self.List:SetData(list)

    self:FlushNameList()
end

function ArenaMatchDetailView:CloseCallback()
    self.data.rank_info_list:Uncare(self.data_ht)
    TimeHelper:CancelTimer(self.time_ht)
end

function ArenaMatchDetailView:OnCloseClick()
    ViewMgr:CloseView(ArenaMatchDetailView)
end

function ArenaMatchDetailView:FlushNameList()
    self.NameList:SetData(self.name_list or {})
    self.NameList:SetectItem(1, true)
end

function ArenaMatchDetailView:FlushRankInfo()
    local rank_info_list = self.data:RankInfoList() or {}
    self.RankList:SetData(rank_info_list)
end

function ArenaMatchDetailView:OnNameClick(data)
    ArenaMatchCtrl:SendPlatformBattleReq({oper_type = ArenaMatchConfig.OperType.rank_info, p1 = data.seq})
    self.sel_data = data
end

function ArenaMatchDetailView:OnRewardClick()
    ViewMgr:OpenView(ArenaMatchRewardPreviewView, {sel_index = self.param_t.seq})
end

function ArenaMatchDetailView:OnGotoClick()
    if self.time_ht then
        return
    end
    AudioMgr:PlayEffect(AudioType.UI, CommonAudio.LoginStartClick)
    if SceneData.Instance:GetMode() ~= SceneModeType.ArenaMatch then
        SceneCtrl.Instance:RequestEnterFb(FB_TYPE.ARENA_MATCH, self.param_t.seq)
    end

    self.data.SelMatch.pos = self.sel_data.position
    local func = function ()
        if self.sel_data then
            SceneLogic.Instance:AutoToNpc(self.sel_data.npc_seq, nil, false, false)
        end
        ViewMgr:CloseView(ArenaMatchView)
        ViewMgr:CloseView(ArenaMatchDetailView)
    end

    if LoginData.Instance:IsOnCrossSever() then
        func()
    else
        TimeHelper:CancelTimer(self.time_ht)
        self.time_ht = Invoke(function ()
            func()
        end,3)
    end
end


ArenaMatchDetailNameItem = ArenaMatchDetailNameItem or DeclareMono("ArenaMatchDetailNameItem", UIWidgetBaseItem)
function ArenaMatchDetailNameItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    UH.SetText(self.Name, data.leitai_name)
    local role_list = ArenaMatchData.Instance.role_info_list
    local info = role_list[data.seq]
    self.RankObj:SetActive(info ~= nil)
    if info then
        UH.SetText(self.RankText, info:Rank())
	end
end

ArenaMatchDetailRankItem = ArenaMatchDetailRankItem or DeclareMono("ArenaMatchDetailRankItem", UIWidgetBaseItem)
function ArenaMatchDetailRankItem:SetData(data)
    UIWidgetBaseItem.SetData(self,data)
    UH.SetText(self.Name, data.role_info.name)
    if data.role_info.top_level and data.role_info.top_level > 0 then
        UH.SetText(self.Level, string.format(Language.PeakRoad.Level2, data.role_info.top_level))
	else
		UH.SetText(self.Level, string.format("Lv.%s",data.role_info.level))
	end
    
    UH.SetText(self.FightValue, data:PetsCap() + data:PartnersCap())

    local rank = data.role_info.rank
    if rank <=3 and rank >= 1 then
        self.RankImg:SetObjActive(true)
        UH.SpriteName(self.RankImg, string.format("PaiMing%s",rank))
        UH.SetText(self.RankText, "")
    else
        self.RankImg:SetObjActive(false)
        UH.SetText(self.RankText, rank)
    end
    local bg_name = data.role_info.uid == RoleData.Instance:GetRoleId() and "HongLan" or "XinXiDiBan1"
    UH.SpriteName(self.BG, bg_name)
end

function ArenaMatchDetailRankItem:OnClickInfo()
    ArenaMatchData.Instance.cur_sel_info:Set(ArenaMatchReadyInfo.New(ArenaMatchInfo.CreateInfo{per_info = self.data}))
	ViewMgr:OpenView(ArenaMatchWinnerView)
end


ArenaMatchOccupyDetail = ArenaMatchOccupyDetail or DeclareMono("ArenaMatchOccupyDetail", UIWidgetBaseItem)
function ArenaMatchOccupyDetail:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    UH.SetText(self.Desc, string.format(Language.ArenaMatch.OccupyText, data:ArenaName()))
end