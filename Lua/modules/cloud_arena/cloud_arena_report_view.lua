CloudArenaReportView = CloudArenaReportView or DeclareView("CloudArenaReportView","cloud_arena/cloud_arena_report")
VIEW_DECLARE_MASK(CloudArenaReportView, ViewMask.BgBlock)
function CloudArenaReportView:CloudArenaReportView()
	self.data = CloudArenaData.Instance
end

function CloudArenaReportView:LoadCallback()
	self.RecordList:SetData(self.data:GetRecordList())
    if self.data.notice_smart.new_record then
        self.data.notice_smart.new_record = false
    end
	self.NoRecord:SetActive(self.data:GetRecordList() == nil or next(self.data:GetRecordList()) == nil)
end

function CloudArenaReportView:OnClickClose()
	ViewMgr:CloseView(CloudArenaReportView)
end

------------------------------
CloudArenaReportItem = CloudArenaReportItem or DeclareMono("CloudArenaReportItem", UIWidgetBaseItem)
function CloudArenaReportItem:CloudArenaReportItem()
	self.data_instance = CloudArenaData.Instance
end

function CloudArenaReportItem:SetData(data)
	local main_role_id = RoleData.Instance:GetRoleId()
	local arrow_name = "LvJianTou"
	if main_role_id == data.defender_info.uid then
		UH.SpriteName(self.ResultIcon, data.is_attacker_win > 1 and "_loc_ping" or data.is_attacker_win == 0 and "_loc_sheng" or "_loc_bai")
		UH.SetText(self.RankChangeText, Language.CloudArena.RankTitle..(data.defender_info.new_rank + 1))
		self.RankNotChange:SetActive(data.defender_info.old_rank == data.defender_info.new_rank)
		if data.defender_info.change_score > 0 then
			UH.SetText(self.ScoreText, string.format(Language.Common.Xstr,  COLORSTR.Green7 , "+"..data.defender_info.change_score))
			arrow_name = data.defender_info.old_rank == data.defender_info.new_rank and "" or arrow_name
		else
			UH.SetText(self.ScoreText, string.format(Language.Common.Xstr, COLORSTR.Red8, "--"))
			arrow_name = data.defender_info.old_rank == data.defender_info.new_rank and "" or "HongJianTou"
		end
	else
		UH.SpriteName(self.ResultIcon, data.is_attacker_win > 1 and "_loc_ping" or data.is_attacker_win == 0 and "_loc_bai" or "_loc_sheng")
		UH.SetText(self.RankChangeText, Language.CloudArena.RankTitle..(data.attacker_info.new_rank + 1))
		self.RankNotChange:SetActive(data.attacker_info.old_rank == data.attacker_info.new_rank)
		if data.attacker_info.change_score > 0 then
			UH.SetText(self.ScoreText, string.format(Language.Common.Xstr,  COLORSTR.Green7 , "+"..data.attacker_info.change_score))
			arrow_name = data.attacker_info.old_rank == data.attacker_info.new_rank and "" or arrow_name
		else
			UH.SetText(self.ScoreText, string.format(Language.Common.Xstr, COLORSTR.Red8, "--"))
			arrow_name = data.attacker_info.old_rank == data.attacker_info.new_rank and "" or "HongJianTou"
		end

		
	end
	UH.SpriteName(self.ScoreArrow, arrow_name)
	UH.SpriteName(self.DefProfImg, ProfSettingB[math.floor(data.defender_info.profession / 100)])
	UH.SpriteName(self.AttProfImg, ProfSettingB[math.floor(data.attacker_info.profession / 100)])
	self.DefAvatar:SetData({appearance = {avatar_type = data.defender_info.avatar, avatar_id = data.defender_info.avatar_id,avatar_id = data.defender_info.avatar_quality,prof_base = math.floor(data.defender_info.profession / 100)}})
	self.AttAvatar:SetData({appearance = {avatar_type = data.attacker_info.avatar, avatar_id = data.attacker_info.avatar_id,avatar_id = data.attacker_info.avatar_quality,prof_base = math.floor(data.attacker_info.profession / 100)}})
	UH.SetText(self.DefNameText, data.defender_info.is_robot == 1 and data.defender_info.name.."_s"..LoginData.Instance:GetLastLoginServer() or data.defender_info.name)
	UH.SetText(self.AttNameText, data.attacker_info.is_robot == 1 and data.attacker_info.name.."_s"..LoginData.Instance:GetLastLoginServer() or data.attacker_info.name)
	local def_rank_cfg = self.data_instance:GetRankConfig(data.defender_info.old_score)
	local att_rank_cfg = self.data_instance:GetRankConfig(data.attacker_info.old_score)
	if def_rank_cfg then
		UH.SetText(self.DefRankingText, string.format(Language.Common.Xstr, QualityColorStr[def_rank_cfg.rank], def_rank_cfg.ranking_title))
	end
	if att_rank_cfg then
		UH.SetText(self.AttRankingText, string.format(Language.Common.Xstr, QualityColorStr[att_rank_cfg.rank], att_rank_cfg.ranking_title))
	end
end