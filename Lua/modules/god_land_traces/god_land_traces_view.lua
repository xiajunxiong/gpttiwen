
GodLandTracesView = GodLandTracesView or DeclareView("GodLandTracesView", GodLandTracesConfig.ResPath .. "god_land_traces", Mod.GodLandTraces.Main)
VIEW_DECLARE_LEVEL(GodLandTracesView,ViewLevel.L1)

function GodLandTracesView:GodLandTracesView()
	self.data = GodLandTracesCtrl.Instance:Data()
	self.language = Language.GodLandTraces
end

function GodLandTracesView:LoadCallback(param_t)
	local ti = GodLandTracesConfig.TipsIcons
	local ta = GodLandTracesConfig.TipsAtlas
	self.GetTips.text = string.format(self.language.GetTips, RichTextHelper.SpriteStrAtlas(ti[1].n, ti[1].w, ti[1].h, nil, ta), 
		RichTextHelper.SpriteStrAtlas(ti[2].n, ti[2].w, ti[2].h, nil, ta), RichTextHelper.SpriteStrAtlas(ti[3].n, ti[3].w, ti[3].h, nil, ta))

	if 0 == UnityPlayerPrefs.GetInt(PrefKeys.GodLandTracesGuide(RoleData.Instance:GetRoleId())) then
		ViewMgr:OpenView(GodLandTracesGuideView)
		UnityPlayerPrefs.SetInt(PrefKeys.GodLandTracesGuide(RoleData.Instance:GetRoleId()), 1)
	end
end

function GodLandTracesView:CloseCallback()

end

function GodLandTracesView:OnClickClose()
	ViewMgr:CloseView(GodLandTracesView)
end

GodLandTracesViewF = GodLandTracesViewF or DeclareMono("GodLandTracesViewF", UIWFlushPanel)

function GodLandTracesViewF:GodLandTracesViewF()
	self.data = GodLandTracesCtrl.Instance:Data()
	self.language = Language.GodLandTraces

	self.data_cares = {
		{data = self.data.info_sm, func = self.FlushInfo},
		{data = self.data.info_sm_open, func = self.FlushTime},
		{data = BagData.Instance:GetColumnGrids(ItemColumnType.Material), func = self.FlushShow, init_call = false},
	}
end

function GodLandTracesViewF:OnDestroy()
	UIWFlushPanel.OnDestroy(self)
	TimeHelper:CancelTimer(self.timer_ct_open)
end

function GodLandTracesViewF:FlushInfo()
	local reward_list, value1, value2 = self.data:GetRewardList()
	self.BlockGet:SetActive(value1)
	self.RewardList:SetData(reward_list)
	self:FlushShow()
	self.State.text = not value2 and self.language.State3 or (value1 and self.language.State2 or self.language.State1)
	self.StateGrey.enabled = not value1 and value2
end

function GodLandTracesViewF:FlushTime()
	local iso = self.data.info_sm_open
	TimeHelper:CancelTimer(self.timer_ct_open)
	if 1 == iso.is_open then
		self.timer_ct_open = TimeHelper:AddCountDownCT(function ()
			local next_time = iso.next_state_time - TimeHelper.GetServerTime()
			local ft = TimeHelper.FormatDHMS(next_time)
			self.RemainTime.text = string.format(self.language.RemainTime, ft.day, ft.hour)
		end, function ()
			self.RemainTime.text = string.format(self.language.RemainTime, 0, 0)
		end,
		iso.next_state_time, 1)
	else
		ViewMgr:CloseView(GodLandTracesView)
	end
end

function GodLandTracesViewF:FlushShow()
	self.ShowList:SetData(self.data.show_list)
end

function GodLandTracesViewF:OnClickGet()
	GodLandTracesCtrl.Instance:SendTianDiDaoHenReqFetch()
end

GodLandTracesViewShowItem = DeclareMono("GodLandTracesViewShowItem", UIWidgetBaseItem)

function GodLandTracesViewShowItem:GodLandTracesViewShowItem()
	self.glt_data = GodLandTracesCtrl.Instance:Data()
	self.society_data = SocietyCtrl.Instance:Data()
	self.language = Language.GodLandTraces
end

function GodLandTracesViewShowItem:SetData(data)
	UIWidgetBaseItem.SetData(self, data)
	
	local have_num = BagData.Instance:GetNumByItemId(data.item_id)
	data.item.num = have_num

	local show_get = false
	local show_help = false
	local show_send = false

	self.red_point_item = self.red_point_item or UiRedPoint.New(self.BtnGetObj, Vector3.New(40,15,0))
	local red_point_num = 0
	if data.flag then
		show_send = have_num > 0
		self.Desc.text = self.language.Finish
	else
		if have_num >= data.need_num then
			red_point_num = 1
			show_get = true
		else
			show_help = true
		end
		self.Desc.text = string.format(self.language.NeedNum, data.need_num)
	end
	self.red_point_item:SetNum(red_point_num)
	self.BtnGetObj:SetActive(show_get)
	self.BtnHelpObj:SetActive(show_help)
	self.BtnSendObj:SetActive(show_send)
	
	self.Cell:SetData(data.item)
end

function GodLandTracesViewShowItem:OnClickGet()
	AudioMgr:PlayEffect(AudioType.UI, CommonAudio.TreasureHuntOpen)
	GodLandTracesCtrl.Instance:SendTianDiDaoHenReqActive(self.data.index - 1)
end

function GodLandTracesViewShowItem:OnClickHelp()
	self.society_data:SetShareInfo(SocietyShareInfoGodLandTrace.New(self.data.item))
	ViewMgr:OpenView(SocietyShareView)
end

function GodLandTracesViewShowItem:OnClickSend()
	self.society_data:SetSocietySendInfo(SocietyConfig.ShareType.GodLandTrace, 0, self.data.item_id)
	ViewMgr:OpenView(SocietySendView)
end

GodLandTracesViewRewardItem = DeclareMono("GodLandTracesViewRewardItem", UIWidgetBaseItem)

function GodLandTracesViewRewardItem:GodLandTracesViewRewardItem()
	self.glt_data = GodLandTracesCtrl.Instance:Data()
end

function GodLandTracesViewRewardItem:SetData(data)
	UIWidgetBaseItem.SetData(self, data)
	self.Cell:SetData(data.item)
	self.RandomObj:SetActive(data.index > 0)
	self.MustObj:SetActive(0 == data.index)
	self.red_point_item = self.red_point_item or UiRedPoint.New(self.RemindObj, Vector3.New(30,30,0))
	self.red_point_item:SetNum(data.flag and 1 or 0)
end