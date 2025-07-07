IllustrateCollectView = IllustrateCollectView or DeclareMono("IllustrateCollectView", UIWFlushPanel)

function IllustrateCollectView:IllustrateCollectView()
    self.data = IllustrateData.Instance
    self.data_cares = {
        {data = self.data.included_red_piont,func = self.FlushTabList,init_call = false},
        {data = self.data.select_data,func = self.FlushListView,init_call = true,keys = {"page"}},
    }
end

function IllustrateCollectView:Awake()
    UIWFlushPanel.Awake(self)
    local list = {}
    for i,v in ipairs(self.data:GetCollectPageData()) do
        local is_can_show = true
        if v.page == 6 then
            is_can_show = ShengQiData.Instance:IsOpen()
        end

        if self.PageList[v.page] and is_can_show then
            table.insert(list,{page = v.page,page_name = v.page_name,tab_objs = {self.PageList[v.page].gameObject}})
        end
    end
    self.TabList:SetDataList(list)
end

function IllustrateCollectView:FlushTabList()
    for k,item in pairs(self.TabList.item_list) do
        if item then
            item:FlushRedNumView()
        end
    end
    self:FlushPageList()
end

function IllustrateCollectView:FlushPageList()
    local page = self.data:GetSelectPage()
    local page_data = self.data:GetCollectPageList()

    local role_level = RoleData.Instance:GetRoleLevel()
    local target_num = page_data.target_num
    for i,page in pairs(page_data.page_list) do
        for _,v in pairs(page.item_list) do
            if role_level < v.show_condition_level then
                target_num = target_num - 1
            end
        end
    end

    self:FlushProgressView(target_num)
    for k,item in pairs(self.PageList[page].item_list) do
        if item then
            item:FlushRedPageView()
        end
    end
end

function IllustrateCollectView:FlushListView()
    local page = self.data:GetSelectPage()
    local page_data = self.data:GetCollectPageList()

    local role_level = RoleData.Instance:GetRoleLevel()
    local target_num = page_data.target_num
    for i,page in pairs(page_data.page_list) do
        for _,v in pairs(page.item_list) do
            if role_level < v.show_condition_level then
                target_num = target_num - 1
            end
        end
    end

    self.PageList[page]:SetData(page_data.page_list)
    self:FlushProgressView(target_num)
end

function IllustrateCollectView:FlushProgressView(target_num)
    local page = self.data:GetSelectPage()
    local active_num = Valve(self.data:GetIncludePageNum(page),target_num) 
    self.Progress:SetProgress(active_num / target_num)
    self.Progress:SetText(Format("%s/%s",active_num,target_num))
    local page_name = Language.Illustrate.CollectName[page]
    UH.SetText(self.Included,Format(Language.Illustrate.Included,page_name))
    local attr_list = self.data:GetIncludedAttrData(page)
    self.BaseList:SetData(attr_list)
    self.AttrTips:SetObjActive(#attr_list == 0)
    UH.SetText(self.AttrTips,Format(Language.Illustrate.AttrTips,page_name))
end

----------------------------IllustrateCollectBtn----------------------------
IllustrateCollectBtn = IllustrateCollectBtn or DeclareMono("IllustrateCollectBtn", UIWidgetBaseItem)

function IllustrateCollectBtn:SetData(data)
    UH.SetText(self.Name,data.page_name)
    self.TabActivator.Actives:Clear()
    if data.tab_objs ~= nil then
		for _, item in pairs(data.tab_objs) do
			self.TabActivator.Actives:Add(item)
		end
    end
    UIWidgetBaseItem.SetData(self, data)
    self:FlushRedNumView()
end

function IllustrateCollectBtn:FlushRedNumView()
    if self.red_point == nil then
        self.red_point = UiRedPoint.New(self.Root, Vector3.New(68,24,0))
        self.red_point:SetNum(0)
    end
    self.red_point:SetNum(IllustrateData.Instance:GetIncludedPageRedNum(self.data.page))
end

function IllustrateCollectBtn:OnClickItem()
    IllustrateData.Instance:SetSelectPage(self.data.page)
end

function IllustrateCollectBtn:Click()
	if self.Toggle then
		self.Toggle:Trigger()
	end
end

----------------------------IllustrateCollectPage----------------------------
IllustrateCollectPage = IllustrateCollectPage or DeclareMono("IllustrateCollectPage",UIWidgetBaseItem)
function IllustrateCollectPage:SetData(data)
    local list_data = {}
    local role_level = RoleData.Instance:GetRoleLevel()
    for i,v in pairs(data.item_list or {}) do
        if not v.show_condition_level or role_level >= v.show_condition_level then
            table.insert(list_data,v)
        end
    end
    self.List:SetData(list_data)
    self.list_data = list_data
    local page_name = Language.Illustrate.CollectName[data.page]
    UH.SetText(self.PageName,IllustrateConfig.PageName[data.page][data.color])
    self:FlushTitleView(IllustrateConfig.PageColor[data.page][data.color])
    UIWidgetBaseItem.SetData(self, data)
    self:FlushRedPageView()
end

function IllustrateCollectPage:FlushTitleView(color)
    self.Gradient.topColor = color[1]
    self.Gradient.buttomColor = color[2]
    self.Outline.effectColor = color[3]
    self.Shadow.effectColor = color[3]
end

function IllustrateCollectPage:FlushRedPageView()
    local active_num = IllustrateData.Instance:GetPageActiveNum(self.data.item_list)
    UH.SetText(self.PageNum,Format("（%s/%s）",active_num,#self.list_data))
    for k,item in pairs(self.List.item_list) do
        if item then
            item:FlushRedNumView()
        end
    end
end

----------------------------IllustrateCollectItem----------------------------
IllustrateCollectItem = IllustrateCollectItem or DeclareMono("IllustrateCollectItem",UIWidgetBaseItem)
function IllustrateCollectItem:SetData(data)
    self.Effect:SetActive(false)
    local item,name = IllustrateData.Instance:GetIncludedItem(data.page,data.id)
    self.Call:SetData(item)
    UH.SetText(self.Name,name)
    for i=1,self.AttrList:Length() do
        UH.SetText(self.AttrList[i],data.attr_list[i] and data.attr_list[i].name or "")
    end
    UIWidgetBaseItem.SetData(self, data)
    self:FlushRedNumView()
end

function IllustrateCollectItem:OnClickItem()
    if self.RedPos.activeSelf then
        self.red_point:SetNum(0)
        self.Effect:SetActive(true)
        AudioMgr:PlayEffect(AudioType.UI, CommonAudio.QiangHuaChengGong)
        IllustrateCtrl.Instance:SendCollectSeq(IllustrateConfig.Req.Included, self.data.seq)
        TimeHelper:CancelTimer(self.effect_time)
        self.effect_time = TimeHelper:AddDelayTimer(function()
            TimeHelper:CancelTimer(self.effect_time)
            self.Effect:SetActive(false)
        end, 3)
    end
end

function IllustrateCollectItem:OnDestroy()
	TimeHelper:CancelTimer(self.effect_time)
end

function IllustrateCollectItem:FlushRedNumView()
    local can_active = IllustrateData.Instance:GetActiveCanFalg(self.data.seq) == 1
    local has_active = IllustrateData.Instance:GetActiveHasFalg(self.data.seq) == 1
    if not has_active and can_active then
        if self.red_point == nil then
            self.red_point = UiRedPoint.New(self.RedPos, Vector3.zero)
        end
        self.red_point:SetNum(1)
    end 
    self.Call:MakeGrey(not has_active)
    self.RedPos:SetActive(not has_active and can_active)
    UH.SpriteName(self.Image,has_active and IllustrateConfig.ImageName[self.data.page]..self.data.color or IllustrateConfig.None)
end

----------------------------IllustrateCollectPartnerItem----------------------------
IllustrateCollectPartnerItem = IllustrateCollectPartnerItem or DeclareMono("IllustrateCollectPartnerItem",UIWidgetBaseItem)
function IllustrateCollectPartnerItem:SetData(data)
    local config = Config.partner_cfg_auto.partner_info[data.id]
    if config then
        UH.SetIcon(self.Icon, config.icon_id, ICON_TYPE.ITEM)
        PartnerInfo.SetQuality(self.Quality, config.partner_quality)
    end
end

function IllustrateCollectPartnerItem:MakeGrey(grey)
    self.Interactor.Interactable = not grey
end

----------------------------IllustrateCollectAttrItem----------------------------
IllustrateCollectAttrItem = IllustrateCollectAttrItem or DeclareMono("IllustrateCollectAttrItem",UIWidgetBaseItem)
function IllustrateCollectAttrItem:SetData(data)
    UH.SetText(self.Value,data.attr_name .. "  +" .. data.attr_value)
end