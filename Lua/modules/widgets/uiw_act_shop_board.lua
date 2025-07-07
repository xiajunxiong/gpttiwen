------Title====UI.Text
ActShopBoard1 = ActShopBoard1 or DeclareMono("ActShopBoard1")

function ActShopBoard1:ActShopBoard1()
	self.title_str = ""
	self.view_type = nil
end

function ActShopBoard1:OnClickClose()
	if self.view_type ~= nil then
		ViewMgr:CloseView(self.view_type)
	end
end

function ActShopBoard1:SetData(view_type, title, style_type)
    self.view_type = view_type
    UH.SetText(self.Title, title)
	self:SetStyleData(style_type or 0)
end

function ActShopBoard1:OnRelease()
end

function ActShopBoard1:SetStyleData(style_type)
	local special_config = {[2] = true,[3] = true}
	local special_lr_config = {[6] = true}

	UH.SpriteName(self.ImgBG,[[DaDi]] .. style_type)
	UH.SpriteName(self.ImgTitle,[[Zuo]] .. style_type)
	local top_conver = {[5] = 1}
	UH.SpriteName(self.ImgDescLT,[[DingBuZhuangShiZuo]] .. (top_conver[style_type] or style_type))
	UH.SpriteName(self.ImgDescRT,[[DingBuZhuangShiYou]] .. (top_conver[style_type] or style_type))
	UH.SpriteName(self.ImgDescLB,[[DiBuZhuangShiZuo]] .. style_type)
	UH.SpriteName(self.ImgDescRB,[[DiBuZhuangShiYou]] .. style_type)
	local close_conver = {[5] = 1}
	UH.SpriteName(self.ImgLampRight,[[DengLongYou]] .. (top_conver[style_type] or style_type))

	local is_special = special_config[style_type]
	UH.SpriteName(self.ImgLampLeft,is_special == nil and [[DengLongZuo]] .. (top_conver[style_type] or style_type) or nil)
	self.ImgLampRight:SetObjActive(is_special == nil)
	self.BtnClose:SetActive(is_special ~= nil)

	if special_lr_config[style_type] == true then
		self.BtnClose:SetActive(true)
		self.ImgLampLeft:SetObjActive(false)
		self.ImgLampRight:SetObjActive(false)
	end
	self.ImgDescLT:SetObjActive(special_lr_config[style_type] ~= true)
	self.ImgDescRT:SetObjActive(special_lr_config[style_type] ~= true)
	self.ImgDescLB:SetObjActive(special_lr_config[style_type] ~= true)
	self.ImgDescRB:SetObjActive(special_lr_config[style_type] ~= true)
	self.ObjSp6:SetActive(special_lr_config[style_type] == true)
end

function ActShopBoard1.GetShowStyle(config)
	for k,v in pairs(config) do
		if v.is_show == 1 then
			return v.interface
		end
	end
	return 0
end

ActShopBoard2 = ActShopBoard2 or DeclareMono("ActShopBoard2")

function ActShopBoard2:ActShopBoard2()
	self.title_str = ""
	self.view_type = nil
end

function ActShopBoard2:OnClickClose()
	if self.view_type ~= nil then
		ViewMgr:CloseView(self.view_type)
	end
end

function ActShopBoard2:SetData(view_type, title)
	self.view_type = view_type
	UH.SetText(self.Title, title)
end

function ActShopBoard2:OnRelease()
end

ActShopBoard3 = ActShopBoard3 or DeclareMono("ActShopBoard3")
function ActShopBoard3:ActShopBoard3()
	self.title_str = ""
	self.view_type = nil
end

function ActShopBoard3:OnClickClose()
	if self.view_type ~= nil then
		ViewMgr:CloseView(self.view_type)
	end
end

function ActShopBoard3:SetData(view_type, title, show_style)
	self.view_type = view_type
	for i = 1,self.BoardList:Length() do
		self.BoardList[i]:SetActive(i == show_style + 1)
	end
	if self.TextList[show_style + 1] then
		UH.SetText(self.TextList[show_style + 1], title)
	end
end

function ActShopBoard3:OnRelease()
end

------Title====UI.Text
ActShopBoard4 = ActShopBoard4 or DeclareMono("ActShopBoard4")

function ActShopBoard4:ActShopBoard4()
	self.title_str = ""
	self.view_type = nil
end

function ActShopBoard4:OnClickClose()
	if self.view_type ~= nil then
		ViewMgr:CloseView(self.view_type)
	end
end
function ActShopBoard4:SetData(view_type,title,show_style)
	self.view_type = view_type
	for i = 1,self.BoradList:Length() do
		self.BoradList[i]:SetActive(i==show_style+1)
	end
	if self.TextList[show_style+1] then
		UH.SetText(self.TextList[show_style+1],title)
	end
end

function ActShopBoard4:OnRelease()
end

------Title====UI.Text
ActShopBoard5 = ActShopBoard5 or DeclareMono("ActShopBoard5")

function ActShopBoard5:ActShopBoard5()
	self.title_str = ""
	self.view_type = nil
end

function ActShopBoard5:OnClickClose()
	if self.view_type ~= nil then
		ViewMgr:CloseView(self.view_type)
	end
end
function ActShopBoard5:SetData(view_type,title,show_style)
	self.view_type = view_type
	for i = 1,self.BoradList:Length() do
		self.BoradList[i]:SetActive(i==show_style+1)
	end
	if self.TextList[show_style+1] then
		UH.SetText(self.TextList[show_style+1],title)
	end
end

function ActShopBoard5:OnRelease()
end


ActSpringBoard = ActSpringBoard or DeclareMono("ActSpringBoard")
function ActSpringBoard:ActSpringBoard()
	self.view_type = nil
end

function ActSpringBoard:OnClickClose()
	if self.view_type ~= nil then
		ViewMgr:CloseView(self.view_type)
	end
end

function ActSpringBoard:SetData(view_type)
	self.view_type = view_type
end


ActShopBoard6 = ActShopBoard6 or DeclareMono("ActShopBoard6")
function ActShopBoard6:ActShopBoard6()
    self.view_type = nil
end

function ActShopBoard6:OnClickClose()
    if self.view_type ~= nil then
        ViewMgr:CloseView(self.view_type)
    end
end

function ActShopBoard6:SetData(view_type)
    self.view_type = view_type
end

