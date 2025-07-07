
GodLandTracesGuideView = GodLandTracesGuideView or DeclareView("GodLandTracesGuideView", GodLandTracesConfig.ResPath .. "god_land_traces_guide")

function GodLandTracesGuideView:GodLandTracesGuideView()
	self.data = GodLandTracesCtrl.Instance:Data()
	self.language = Language.GodLandTraces
end

function GodLandTracesGuideView:LoadCallback()
	local ti = GodLandTracesConfig.TipsIcons
	local ta = GodLandTracesConfig.TipsAtlas
	self.GetTips.text = string.format(self.language.GetTips2, RichTextHelper.SpriteStrAtlas(ti[1].n, ti[1].w, ti[1].h, nil, ta), 
		RichTextHelper.SpriteStrAtlas(ti[2].n, ti[2].w, ti[2].h, nil, ta), RichTextHelper.SpriteStrAtlas(ti[3].n, ti[3].w, ti[3].h, nil, ta))
	
	for i = 1, self.ShowItems:Length() do
		self.ShowItems[i]:SetData(self.data.show_list[i], i)
	end

	self.Page1Obj:SetActive(true)
	self.Page2Obj:SetActive(false)
end

function GodLandTracesGuideView:CloseCallback()

end

function GodLandTracesGuideView:OnClickArrow()
	self.Page1Obj:SetActive(false)
	self.Page2Obj:SetActive(true)
end

function GodLandTracesGuideView:OnClickBlock()
	ViewMgr:CloseView(GodLandTracesGuideView)
end

GodLandTracesViewGuideShowItem = DeclareMono("GodLandTracesViewGuideShowItem", UIWidgetBaseItem)

function GodLandTracesViewGuideShowItem:GodLandTracesViewGuideShowItem()
	self.glt_data = GodLandTracesCtrl.Instance:Data()
	self.language = Language.GodLandTraces
	self.config = GodLandTracesConfig.GuideShow
end

function GodLandTracesViewGuideShowItem:SetData(data, index)
	UIWidgetBaseItem.SetData(self, data)
	
	local have_num = self.config.have_num[index]
	local need_num = self.config.need_num[index]
	data.item_guide.num = have_num

	local show_get = false
	local show_help = false
	local show_send = false

	if 3 == index then
		show_send = have_num > 0
		self.Desc.text = self.language.Finish
	else
		if have_num >= need_num then
			show_get = true
		else
			show_help = true
		end
		self.Desc.text = string.format(self.language.NeedNum, need_num)
	end

	self.BtnGetObj:SetActive(show_get)
	self.BtnHelpObj:SetActive(show_help)
	self.BtnSendObj:SetActive(show_send)
	
	self.Cell:SetData(data.item_guide)
end