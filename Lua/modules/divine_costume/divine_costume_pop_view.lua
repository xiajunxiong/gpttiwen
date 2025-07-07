DivineCostumePopupView = DivineCostumePopupView or DeclareView("DivineCostumePopupView","divine_costume/divine_costume_popup")
-- Board = UIWSubstitute(UIWBoard3)
function DivineCostumePopupView:DivineCostumePopupView()
    self.data = DivineCostumeData.Instance
end

function DivineCostumePopupView:LoadCallback()
    self.Board:SetData(self:GetType(), Language.DivineCostume.PopDetail, Vector2.New(725, 550))
	self.TabList:SetDataList(self.data:GetBtnList(),1)
end

function DivineCostumePopupView:CloseCallback()
	DivineCostumeData.Instance.pop_tab_index_data.index  = 0
end


DivineCostumePopupPanel = DivineCostumePopupPanel or DeclareMono("DivineCostumePopupPanel", UIWFlushPanel)
function DivineCostumePopupPanel:DivineCostumePopupPanel()
	self.data = DivineCostumeData.Instance
	self.data_cares = {
		{data = self.data.pop_tab_index_data, func = self.FlushShowPanel, init_call = false},
    }
end 

function DivineCostumePopupPanel:FlushShowPanel()
	if self.data.pop_tab_index_data.index == 1 then
		self.shuomingobj:SetActive(false)
		self.shuxingobj:SetActive(true)
		self.shuxinglist:SetDataList(self.data:GetAllAttrByEquipType(self.data.now_select_equiptype))
	else
		self.shuomingobj:SetActive(true)
		self.shuxingobj:SetActive(false)
	end
end


----------------------------DivineCostumePopupBtnItem----------------------------
DivineCostumePopupBtnItem = DivineCostumePopupBtnItem or DeclareMono("DivineCostumePopupBtnItem", UIWidgetBaseItem)

function DivineCostumePopupBtnItem:SetData(data)
	UIWidgetBaseItem.SetData(self, data)
    UH.SetText(self.Name,data.page_name)
    local is_open = true
    self.Lock:SetActive(not is_open)
    self.BtnInter.Interactable = is_open
end

function DivineCostumePopupBtnItem:OnClickItem()
	DivineCostumeData.Instance.pop_tab_index_data.index = self.data.page_index
end

function DivineCostumePopupBtnItem:OnClickLock()
    --PublicPopupCtrl.Instance:Center(Format(ErrorText[90],self.data.level))
end

function DivineCostumePopupBtnItem:Click()
	if self.Toggle then
		self.Toggle:Trigger()
	end
end

PopShuXingItem = PopShuXingItem or DeclareMono("PopShuXingItem", UIWidgetBaseItem)
function PopShuXingItem:PopShuXingItem()
	
end

function PopShuXingItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
	if data.level == 0 then
		self.attrobj:SetActive(false)
		self.titleobj:SetActive(true)
		UH.SetText(self.jieduan,Language.DivineCostume.JieDuanDetail[data.index])
	else
		self.attrobj:SetActive(true)
		self.titleobj:SetActive(false)	
		UH.SetText(self.grade,string.format(Language.DivineCostume.Grade,tostring(data.level)))
		local attr_one = data.attr[1]
		local arrt_name = Language.Common.AttrList[attr_one.attr_type]
		local num = DataHelper.IsPercent(attr_one.attr_type) and Percent(attr_one.attr_value) or attr_one.attr_value
		UH.SetText(self.showattr1,arrt_name.."+"..num)
		local attr_two = data.attr[2]
		if attr_two then
			local arrt_name_add = Language.Common.AttrList[attr_two.attr_type]
			local num_add = DataHelper.IsPercent(attr_two.attr_type) and Percent(attr_two.attr_value) or attr_two.attr_value
			UH.SetText(self.showattr1,arrt_name.."+"..num.."     "..arrt_name_add.."+"..num_add)
		end
		local level_dec = Language.DivineCostume.NotLimit
		if data.need_rolelevel > 0 then
			level_dec = string.format(Language.DivineCostume.LevelLimit,tostring(data.need_rolelevel))
		end
		UH.SetText(self.level,level_dec)
		local tiao_show = data.level % 2
		if tiao_show == 1 then
			UH.SpriteName(self.tiao,"tiao1")
		else
			UH.SpriteName(self.tiao,"tiao2")
		end
		local grade_color = COLORS.Yellow13
		local attr_color = COLORS.Yellow13
		local level_color = COLORS.Yellow13
		if data.level == DivineCostumeData.Instance.now_select_equiptype_strengthen_level and data.color == DivineCostumeData.Instance.now_select_equiptype_color then
			grade_color = COLORS.Yellow6
			attr_color = COLORS.Green3
			level_color = COLORS.Green3
		else
			if data.color > DivineCostumeData.Instance.now_select_equiptype_color then
				grade_color = COLORS.Yellow15
				attr_color = COLORS.Yellow15
				level_color = COLORS.Yellow15
			else
				if data.color == DivineCostumeData.Instance.now_select_equiptype_color and data.level > DivineCostumeData.Instance.now_select_equiptype_strengthen_level then
					grade_color = COLORS.Yellow15
					attr_color = COLORS.Yellow15
					level_color = COLORS.Yellow15
				end
			end
		end
		UH.Color(self.grade,grade_color)
		UH.Color(self.showattr1,attr_color)
		UH.Color(self.level,level_color)
	end
end
