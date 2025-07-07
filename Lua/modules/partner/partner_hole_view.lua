
PartnerHoleView = PartnerHoleView or DeclareView("PartnerHoleView", PartnerConfig.ResPath .. "partner_hole")

function PartnerHoleView:PartnerHoleView()
	self.data = PartnerCtrl.Instance:Data()
	self.language = Language.Partner.Hole
	self.cost_item = Item.Init(CommonItemId.LuckCoin)
end

function PartnerHoleView:OnClickClose()
	ViewMgr:CloseView(PartnerHoleView)
	AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ViewCloseL1)
end

function PartnerHoleView:OnFlush()
	local cur_sel_pe_hole = self.data.cur_sel_pe_hole
	local data = self.data:GetPartnerInfoById(cur_sel_pe_hole.partner_id)
	-- local hole_desc_co = Cfg.PartnerHoleDescCoByHoleLevel(cur_sel_pe_hole.hole_level)
	-- if nil == item_co or nil == hole_desc_co then return end
	self.cost_name = self.cost_item:Name()
	local have_num = RoleData.Instance:GetCurrency(cur_sel_pe_hole.price_type)
	self.Title.text = string.format(self.language.Title, data.info.name, self.language.HolePostion[cur_sel_pe_hole.hole_postion])
	-- self.Desc.text = string.format(self.language.Desc, cur_sel_pe_hole.hole_level, hole_desc_co.hole_desc)
	self.CostName.text = self.cost_item:Name()
	self.CostNum.text = string.format(self.language.CostNum, RichTextHelper.ColorStr(have_num, have_num < cur_sel_pe_hole.price_num and COLORSTR.Red7 or COLORSTR.Green6), cur_sel_pe_hole.price_num)
	self.CostCell:SetData(self.cost_item)
	-- self.Tips.text = string.format(self.language.Tips, item_co.name, cur_sel_pe_hole.hole_num)
	-- UH.SetIcon(self.CostIcon, cur_sel_pe_hole.hole_need)
end

function PartnerHoleView:OnClickUnlock()
	AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ClickGeneral2)
	local cur_sel_pe_hole = self.data.cur_sel_pe_hole
	local data = self.data:GetPartnerInfoById(cur_sel_pe_hole.partner_id)
	local have_num = RoleData.Instance:GetCurrency(cur_sel_pe_hole.price_type)
	if have_num >= cur_sel_pe_hole.price_num then
		PartnerCtrl.Instance:SendPartnerEqOperatorReq(PartnerConfig.PEOperType.unlock, cur_sel_pe_hole.partner_id, cur_sel_pe_hole.hole_postion - 1)
		if PartnerConfig.LockState.unlock == data.vo.is_unlocked then
			ViewMgr:CloseView(PartnerHoleView)
			self.data.partner_life_effect.pos = cur_sel_pe_hole.hole_postion
		end
	else
		-- PublicPopupCtrl.Instance:DialogTips{
		-- 	content = string.format(self.language.MallTips, self.cost_name or ""),
		-- 	confirm = {
		-- 		func = function()
		-- 			MallCtrl.Instance:Jump(cur_sel_pe_hole.hole_need)
		-- 			ViewMgr:CloseView(DialogTipsView)
		-- 			-- ViewMgr:CloseView(PartnerView)
		-- 		end
		-- 	}
		-- }
		ViewMgr:OpenView(GetWayView, {item = self.cost_item})
		ViewMgr:CloseView(PartnerHoleView)
	end
end