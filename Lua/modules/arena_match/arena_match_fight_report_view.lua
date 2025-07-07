ArenaMatchFightReportView = ArenaMatchFightReportView or DeclareView("ArenaMatchFightReportView","arena_match/arena_match_fight_report")

function ArenaMatchFightReportView:ArenaMatchFightReportView()
	self.Board:SetData(self:GetType(),Language.ArenaMatch.FightReport,Vector2.New(1177, 636))
	self.data = ArenaMatchData.Instance
	self.report_handle = self.data.record_infos:Care(BindTool.Bind(self.FlushRank,self))
    self.btn_handle = self.data.platform_notice_data:Care(BindTool.Bind(self.FlushRank,self))
    self.btn_list_data = TableCopy(self.data.config_pos)
    table.sort(self.btn_list_data,function(a,b)
        return a.position < b.position
    end)
    self.select_index = 1
    if self.data.platform_notice_data.has_lose_record then
        local len = #self.data.platform_notice_data.has_lose_record
        for i = len,1,-1 do
            if self.data.platform_notice_data.has_lose_record[i] > 0 then
                self.select_index = len - i + 1
                break
            end
        end
    end
end

function ArenaMatchFightReportView:LoadCallback()
    AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ViewOpenL2)
	ArenaMatchCtrl.Instance:SendPlatformBattleReq({oper_type = ArenaMatchConfig.OperType.record})
	local actInfo = ActivityData.Instance:GetActivityInfo(ActType.ArenaMatch)
    if actInfo then
        actInfo:ActRedPoint()
    end
	self:FlushRank()
    self.BtnList:SetectItem(self.select_index,true)
end

function ArenaMatchFightReportView:FlushRank()
    self.BtnList:SetData(self.btn_list_data)
    self:SetReport(self.btn_list_data[self.select_index])
end

function ArenaMatchFightReportView:CloseCallback()
    AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ViewCloseL1)
	self.data.record_infos:Uncare(self.report_handle)
    self.data.platform_notice_data:Uncare(self.btn_handle)
end

function  ArenaMatchFightReportView:SetReport(btn_data)
    if btn_data == nil then
        return
    end
    if btn_data.remind_num > 0 then
        ArenaMatchCtrl.Instance:SendPlatformBattleReq({oper_type = ArenaMatchConfig.OperType.set_notice2,p1 = btn_data.position})
    end
    local report_data = self.data:GetRecordInfos(btn_data.position)
	if report_data == nil or next(report_data) == nil then
		self.NorecordOj:SetActive(true)
	else
		self.NorecordOj:SetActive(false)
	end
    self.RecordList:SetData(report_data or {})
end

function ArenaMatchFightReportView:OnSelectedClick(data)
    -- 首次不播放切换标签音效
    if self.play_audio then
        AudioMgr:PlayEffect(AudioType.UI, CommonAudio.QieHuanBiaoQian)
    end
    self.play_audio = true
    self.select_index = self.BtnList:GetSelectedIndex(data)
    self:SetReport(data)
end


ArenaMatchFightReportItem = ArenaMatchFightReportItem or DeclareMono("ArenaMatchFightReportItem",UIWidgetBaseItem)

function ArenaMatchFightReportItem:SetData(data)
	local result = false
    if data.is_win == 1 then
        result = true
    end
    self.WinOj:SetActive(result)
    local my_info
    local rival_info
    if RoleData.Instance:GetRoleId() == data.role_info1.role_id then
    	my_info = data.role_info1
    	rival_info = data.role_info2
    else
    	my_info = data.role_info2
    	rival_info = data.role_info1
    end

    self.MyAvatar:SetAvatar(my_info.avatar_type,my_info.avatar_id,my_info.avatar_quality)
    self.Avatar:SetAvatar(rival_info.avatar_type,rival_info.avatar_id,rival_info.avatar_quality)
    
    UH.SetText(self.MyName,my_info.role_name)
    UH.SetText(self.Name,rival_info.role_name)

    UH.SetText(self.MyLv,my_info.role_level)
    UH.SetText(self.TargetLv,rival_info.role_level)
    
    UH.SpriteName(self.MyProfImg, ProfSettingB[my_info.prof])
    UH.SpriteName(self.TargetProfImg, ProfSettingB[rival_info.prof])

    local rank_str = data.curr_rank == 0 and Language.Arena.rank2 or data.curr_rank
    self.rank.text = string.format(Language.Arena.rank,rank_str)

    local platform_data = ArenaMatchData.Instance:GetArenaBySeq(data.platform_number)
    UH.SetText(self.PlatformName,platform_data.leitai_name)

    if data.last_rank == 0 then
    	data.last_rank = data.curr_rank
    end
    local rank_change_num = (data.curr_rank - data.last_rank) or 0
    local rank_change_num_abs = math.abs(rank_change_num)
    if data.curr_rank == 0 then
        rank_change_num = 1
        rank_change_num_abs = 11 - data.last_rank
    end
    if data.curr_rank == 0 and data.last_rank == 0 then
        rank_change_num = 0
        rank_change_num_abs = 0
    end
    if rank_change_num > 0 then
    	UH.SetText(self.RankChangeNum,ColorStr(rank_change_num_abs,COLORSTR.Red10))
    elseif rank_change_num < 0 then
    	UH.SetText(self.RankChangeNum,ColorStr(rank_change_num_abs,COLORSTR.Green10))
    else
    	UH.SetText(self.RankChangeNum,ColorStr("",COLORSTR.Yellow13))
    end
    self.UpFlagObj:SetActive(rank_change_num < 0)
    self.DownFlagObj:SetActive(rank_change_num > 0)
    self.NoRankObj:SetActive(rank_change_num == 0)
end

ArenaMatchFightReportBtnItem = ArenaMatchFightReportBtnItem or DeclareMono("ArenaMatchFightReportBtnItem",UIWidgetBaseItem)
function ArenaMatchFightReportBtnItem:SetData(data)
    UIWidgetBaseItem.SetData(self,data)
    UH.SetText(self.BtnName,data.desc)
    local remind_num = ArenaMatchData.Instance:BtnRemind(data.position)
    data.remind_num = remind_num
    self.RedPoint:SetNum(remind_num)
end