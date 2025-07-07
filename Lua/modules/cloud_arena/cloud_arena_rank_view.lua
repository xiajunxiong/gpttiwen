CloudArenaRankView = CloudArenaRankView or DeclareView("CloudArenaRankView","cloud_arena/cloud_arena_rank")
VIEW_DECLARE_MASK(CloudArenaRankView, ViewMask.BgBlock)
function CloudArenaRankView:CloudArenaRankView()
	self.data = CloudArenaData.Instance
end

function CloudArenaRankView:OnClickClose()
    ViewMgr:CloseView(CloudArenaRankView)
end

function CloudArenaRankView:CloseCallback()
	self.data.notice_smart.rank_type = 0
end

function CloudArenaRankView:OnClickToggle(param)
	self.data.notice_smart.rank_type = param
end
-------------------------------------
CloudArenaRankPanel = CloudArenaRankPanel or DeclareMono("CloudArenaRankPanel", UIWFlushPanel)
function CloudArenaRankPanel:CloudArenaRankPanel()
	self.data = CloudArenaData.Instance
	self.data_cares = {
		{data = self.data.notice_smart, func = self.FlushRankList, keys = {"rank_type"}},
	}
end

function CloudArenaRankPanel:FlushRankList()
	local data_list = self.data:GetCurRankList()
	self.ScrollView.enabled = false
	self.RankList:SetData(data_list)
	self.RankList:JumpVerticalIndex(1,50)
	self.MyRankInfoItem:SetData(self.data:GetCurMyRankInfo())
	TimeHelper:AddRunFrameTimer(function ()
		self.ScrollView.enabled = true
	end)
end

------------------------------------------
CloudArenaRankItem = CloudArenaRankItem or DeclareMono("CloudArenaRankItem", UIWidgetBaseItem)
function CloudArenaRankItem:CloudArenaRankItem()
	self.data_instance = CloudArenaData.Instance
end
function CloudArenaRankItem:SetData(data)
	if data ~= nil then
		if data.is_robot == 1 then
			UH.SetText(self.NameText, data.name.."_s"..LoginData.Instance:GetLastLoginServer())
		else
			UH.SetText(self.NameText, data.name)
		end
		UH.SetText(self.ScoreText, data.score)
		if data.top_level and data.top_level > 0 then
			UH.SetText(self.LevelText, string.format(Language.PeakRoad.Level2, data.top_level))
		else
			UH.SetText(self.LevelText, data.level)
		end
		if data.rank < 0 then
			UH.SpriteName(self.RankImg, nil)
			UH.SpriteName(self.RankBgImg, nil)
			UH.SetText(self.RankText, Language.CloudArena.NoInRank)
		elseif data.rank <= 2 then
			UH.SpriteName(self.RankImg, "paiming_"..(data.rank + 1))
			UH.SpriteName(self.RankBgImg, "paihangdi"..(data.rank + 1))
			UH.SetText(self.RankText, data.rank <= 2 and "" or data.rank + 1)
		else
			UH.SpriteName(self.RankImg, nil)
			UH.SpriteName(self.RankBgImg, nil)
			UH.SetText(self.RankText, data.rank <= 2 and "" or data.rank + 1)
		end
		local rank_cfg = self.data_instance:GetRankConfig(data.score)
		if rank_cfg ~= nil then
			UH.SetText(self.RankingText, string.format(Language.Common.Xstr, QualityColorStr[rank_cfg.rank], rank_cfg.ranking_title))
		else
			UH.SetText(self.RankingText, Language.CloudArena.NoRankTemp)
		end
		UH.SpriteName(self.ProfImg, ProfSettingB[math.floor(data.profession / 100)])
		if data.my_rank then
			UH.SetText(self.NameText, RoleData.Instance:GetRoleName())
			UH.SpriteName(self.ProfImg, ProfSettingB[RoleData.Instance:ProfType()])
		end
		self.MyBgObj:SetActive(data.rank >= 3 and data.rank == self.data_instance:GetCurMyRankInfo().rank and not data.my_rank)
		self.BgObj:SetActive(data.rank%2 == 0 and not data.my_rank)
	end
	self.data = data
end