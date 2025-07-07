
WenXiangLouInfoView = WenXiangLouInfoView or DeclareView("WenXiangLouInfoView", WenXiangLouConfig.ResPath .. "task_wxl_info", Mod.TaskWXLInfo.Main)
VIEW_DECLARE_MASK(WenXiangLouInfoView, ViewMask.BgBlockNoAnim)

function WenXiangLouInfoView:WenXiangLouInfoView()
	self.data = WenXiangLouCtrl.Instance:Data()
end

function WenXiangLouInfoView:CloseCallback()
	self.data.detail_info.info_sel_seq = -1
end

WenXiangLouInfoViewF = WenXiangLouInfoViewF or DeclareMono("WenXiangLouInfoViewF", UIWFlushPanel)

function WenXiangLouInfoViewF:WenXiangLouInfoViewF()
	self.data = WenXiangLouCtrl.Instance:Data()
	self.language = Language.Task.WXLInfo

	self.data_cares = {
		{data = self.data.detail_info, func = self.FlushBtnConfirm, keys = {"info_sel_seq"}},
    }
end

function WenXiangLouInfoViewF:Awake()
	UIWFlushPanel.Awake(self)
	
    local info_list = self.data:GetInfoList()
	self.InfoList:SetData(info_list)
	self.InfoTips.text = string.format(self.language.info_tips, self.data:GetNpcMinSuspicion())
end

function WenXiangLouInfoViewF:OnClickClose()
    ViewMgr:CloseView(WenXiangLouInfoView)
end

function WenXiangLouInfoViewF:FlushBtnConfirm()
	if WenXiangLouConfig.InfoType.check == self.data.detail_info.info_type then
		self.BtnConfirm.text = self.language.confirm_check
		self.BtnConfirmInter:SetObjActive(true)
	else
		local show_button = self.data.detail_info.info_sel_seq > -1
		self.BtnConfirm.text = self.language.confirm_report
		self.BtnConfirmInter:SetObjActive(show_button)
		if show_button then
			self.BtnConfirmInter.Interactable = self.data:IsNpcCanReport(self.data.detail_info.info_sel_seq)
		end
	end
end

function WenXiangLouInfoViewF:OnClickConfirm()
	if WenXiangLouConfig.InfoType.check == self.data.detail_info.info_type then
		SceneLogic.Instance:AutoToNpc(WenXiangLouConfig.JUDGE_NPC)
		ViewMgr:CloseView(WenXiangLouInfoView)
	else
		local icr, min_suspicion = self.data:IsNpcCanReport(self.data.detail_info.info_sel_seq)
		if icr then
			WenXiangLouCtrl.Instance:SendWenXiangLouIdentity(self.data.detail_info.info_sel_seq)
			ViewMgr:CloseView(WenXiangLouInfoView)
		else
			PublicPopupCtrl.Instance:Center(string.format(self.language.lack_sucspicion_tips, min_suspicion))
		end
	end
end

WenXiangLouInfoViewItem = DeclareMono("WenXiangLouInfoViewItem", UIWidgetBaseItem)

function WenXiangLouInfoViewItem:WenXiangLouInfoViewItem()
	self.wxl_data = WenXiangLouCtrl.Instance:Data()
	self.language = Language.Task.WXLInfo
end

function WenXiangLouInfoViewItem:SetData(data)
	UIWidgetBaseItem.SetData(self, data)

	self:uncareData()
	self.care_handles = {}
	-- self:careData(self.wxl_data.detail_info, BindTool.Bind(self.FlushSucspicion, self), "npc_sucspicion_list")

	self.Name.text = data.name
	self.Desc.text = data.desc
	UH.SetIcon(self.IconSp, data.icon_id, ICON_TYPE.ITEM)
	self:FlushSucspicion()
end

function WenXiangLouInfoViewItem:Click()
	if self.Toggle then
		self.Toggle:Trigger()
	end
end

function WenXiangLouInfoViewItem:OnClickItem()
	self.wxl_data.detail_info.info_sel_seq = self.data.seq
end

function WenXiangLouInfoViewItem:OnDestroy()
    self:uncareData()
end

function WenXiangLouInfoViewItem:FlushSucspicion()
	local cur_val = math.min(self.wxl_data.detail_info.npc_sucspicion_list[self.data.seq] or 0, 100)
	local show_mask = self.wxl_data.detail_info.npc_identity_list[self.data.seq] > 0
	self.Progress:SetProgress(cur_val / 100)
	self.Progress:SetText(string.format(self.language.sucspicion, cur_val))
	self.MaskObj:SetActive(show_mask)
	self.ItemInter.Interactable = not show_mask
end

function WenXiangLouInfoViewItem:OnClickBlock()
	PublicPopupCtrl.Instance:Center(self.language.error_tips)
end