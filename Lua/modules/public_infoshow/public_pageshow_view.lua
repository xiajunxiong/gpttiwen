PublicPageShowView = PublicPageShowView or DeclareView("PublicPageShowView", "public_infoshow/public_pageshow")

function PublicPageShowView:PublicPageShowView()

end

function PublicPageShowView:LoadCallback()
    self.data = PublicInfoShowCtrl.Instance.page_show_data
    self.Board:SetData(self:GetType(), self.data.title_name or "", Vector2.New(873, 505))

    self:FlushTabData()

    self.cache_list = {}
end

function PublicPageShowView:TabDataChange()
    local data = {}
    
    for k,v in pairs(self.data.tab_list) do 
        for i,j in pairs(self.data.show_list) do 
            if v.tab_index == j.tab_index then 
                table.insert( data, v )
                break
            end 
        end 
    end 

    return data
end

function PublicPageShowView:CloseCallback()
    self.cache_list = nil
end

function PublicPageShowView:FlushTabData()

    self.PageList:SetData(self:TabDataChange())
    self.PageList:ClickItem(1)
    -- self.ShowList:SetData(self.data.show_list)

    self.Empty.text = self.data.empty_lab 
end

function PublicPageShowView:FlushShowData(Param)
    local data_list = {}

    if self.cache_list[Param.tab_index] ~= nil then 
        self.ShowList:SetData(self.cache_list[Param.tab_index])

        self.EmptyObj:SetActive(#self.cache_list[Param.tab_index] == 0 and self.data.empty_lab and self.data.empty_lab ~= "") 
        return 
    end 

    for k,v in pairs(self.data.show_list) do 
        if v.tab_index == Param.tab_index then 
            table.insert( data_list, v )
        end 
    end 

    self.ShowList:SetData(data_list)
    self.EmptyObj:SetActive(#data_list == 0 and self.data.empty_lab and self.data.empty_lab ~= "") 
    self.cache_list[Param.tab_index] = data_list
end

function PublicPageShowView:OnClickTab(data)
    self:FlushShowData(data)
end

function PublicPageShowView:OnClickShow(data)
    if self.data.cell_func then 
        self.data.cell_func(data)
    end 
end

InfoPageShowTabCell = DeclareMono("InfoPageShowTabCell", UIWidgetBaseItem)
function InfoPageShowTabCell:InfoPageShowTabCell()
    -- body
end

function InfoPageShowTabCell:SetData(data)
    UIWidgetBaseItem.SetData(self, data)

    self.Name.text = data.name
end

function InfoPageShowTabCell:Click()
    if self.ItemClick then
        self.ItemClick:Trigger()
    end 
end

InfoPageShowCell = DeclareMono("InfoPageShowCell", UIWidgetBaseItem)
function InfoPageShowCell:InfoPageShowCell()
    -- body
end

function InfoPageShowCell:SetData(data)
    UIWidgetBaseItem.SetData(self, data)

    UH.SetIcon(self.Icon,data.icon,data.icon_type)
    UH.SpriteName(self.Qua, MedalConfig.item_qua_sprite[data.qua])
    self.Desc.text = data.desc
    self.Name.text = data.name
end