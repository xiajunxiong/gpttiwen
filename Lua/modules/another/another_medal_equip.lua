AnotherMedalEquipView = AnotherMedalEquipView or DeclareView("AnotherMedalEquipView", "another/another_medal_equip")
VIEW_DECLARE_MASK(AnotherMedalEquipView,ViewMask.BgBlockClose)
function AnotherMedalEquipView:AnotherMedalEquipView()

end

function AnotherMedalEquipView:OpenCallback()

end 

function AnotherMedalEquipView:LoadCallback(param_t)
    self.SortCell1:Trigger()

    self.panel:FlushRight(param_t)
end  

AnotherMedalEquipFlushPanel = AnotherMedalEquipFlushPanel or DeclareMono("AnotherMedalEquipFlushPanel", UIWFlushPanel)

function AnotherMedalEquipFlushPanel:AnotherMedalEquipFlushPanel()
    self.data = AnotherData.Instance
    self.data_cares = {
        {data = self.data.equip_data,func = self.FlushSortCell ,keys = {"medal_info"}},
        -- {data = self.data:GetColumnGrids(),func = self.FlushSortList},
        {data = self.data.equip_data,func = self.FlushBagChange ,keys = {"medal_take_off"},init_call = false},
    }

    self.send_data = {}
    self.effect_play = {}
    self.send_data.select_index = 0
    self.init_flag = false
end 

function AnotherMedalEquipFlushPanel:FlushSortCell()
    for i = 1,3 do 
        local flag = true
        for k,v in pairs(self.data.equip_data.medal_info) do 
            if v.grid_index == i - 1 then 
                self["SortCell"..i]:SetData(v)
                flag = false
            end 
        end 
        if flag then 
            self["SortCell"..i]:SetData({item_id = 0})
        end 
    end 

    if not self.init_flag then 
        self.init_flag = true
    else 
        self.effect_play[self.send_data.select_index + 1] = MedalEquipEffect.New(self, self.send_data.select_index + 1)
        self.effect_play[self.send_data.select_index + 1]:Play()
    end 

    self:FlushPutButton()
end

function AnotherMedalEquipFlushPanel:OnDestroy()
    UIWFlushPanel.OnDestroy(self)
    for index, value in ipairs(self.effect_play) do
        value:DeleteMe()
    end
end 

-- function AnotherMedalEquipFlushPanel:FlushSortList()
--     local data_list = self.data:GetGridForMedal() 
--     self.SortList:SetData(data_list)
-- end

function AnotherMedalEquipFlushPanel:OnClickPutOn()
    local param_t = {
        req_type = MEDAL_REQ_TYPE.PUT_ON,
        param_1 = self.send_data.select_index,
        param_2 = self.send_data.grid_item.index, 
        -- param3 = 
    }

    AnotherCtrl.Instance:SendMedalReq(param_t)

    -- 同步操作
    local real_item = BagData.Instance:SearchEquipByUniqueId(self.send_data.grid_item)
    if real_item == nil then return end 
    local param_t_together = {
        req_type = MEDAL_REQ_TYPE.PUT_ON,
        param1 = self.send_data.select_index,
        param2 = real_item.column_type,
        param3 = real_item.index, 
    }
    BagCtrl.Instance:SendMedalReq(param_t_together)
end

function AnotherMedalEquipFlushPanel:FlushBagChange()
    -- local data = self.data.medal_take_off_str
    -- local split_list = Split(data,"|")
    local bag_cell = self.data:GetItemByIndex(self.data.equip_data.medal_take_off)

    self:FlushRight(bag_cell)
end

-- function AnotherMedalEquipFlushPanel:EffectComplete()
--     self:FlushRight(BagData.Instance:GetItemByIndex(ItemColumnType.Item,self.old_grid_index))
-- end

function AnotherMedalEquipFlushPanel:OnClickTakeOff()
	local param_t = {
        req_type = MEDAL_REQ_TYPE.TAKE_OFF,
        param_1 = self.send_data.select_index
    }
	AnotherCtrl.Instance:SendMedalReq(param_t)
end

function AnotherMedalEquipFlushPanel:OnClickMedalSort(index)
    self.send_data.select_index = index
end

function AnotherMedalEquipFlushPanel:OnClickSortCell(data)
    self.send_data.grid_item = data
end

function AnotherMedalEquipFlushPanel:FlushPutButton( )
    if self.send_data.select_index == nil then return end 
    local index = self.send_data.select_index + 1
    local flag = self["SortCell"..index]:GetData() ~= nil and self["SortCell"..index]:GetData().item_id or 0
    if flag == 0 or flag == nil then 
        self.BtnPutOnText.text = Language.ItemInfo.ItemButtonName.Equip
    else 
        self.BtnPutOnText.text = Language.ItemInfo.ItemButtonName.MedalEquip
    end 
end

function AnotherMedalEquipFlushPanel:FlushRight(data)
    self.Name.text = string.format( Language.Medal.MedalNameColor,QualityColorStr[ItemColor.Green],data:Name() ) 
    self.Cell:SetData(data)
    self.Level.text = data:LimitLv()..Language.Common.Level
    if data.param.sp_id > 0 then 
        local sp_cfg = BagData.Instance:GetEquipEffectCfg(data.param.sp_id)
        self.Special.text = string.format(Language.Common.Xstr, QualityColorStr[data:GetMedalQua()], sp_cfg.description)
        
        self.Name.text = data:QuaName()-- string.format( Language.Medal.MedalNameColor,QualityColorStr[data:GetMedalQua()],data:Name() ) 
    else 
        self.Special.text = Language.ItemInfo.None
    end 

    if data.param.second_sp_id and data.param.second_sp_id > 0 then
        local temp = Format(Language.Common.Xstr, QualityColorStr[data:GetMedalSecondQua()], data:Effect2Desc())
        self.Special2.text = temp
        self.Special2.gameObject:SetActive(true)
    else
        self.Special2.gameObject:SetActive(false)
    end 


    self.Desc.text = data:Desc()

    local color = data:Color() or 1
    self.Quality.SpriteName = ItemInfoConfig.QualitySp[color]

    for k, v in ipairs(data.param.attr_list) do
        if v.attr_value ~= 0 then
            local str = string.format(Language.ItemInfo.ItemTitles.EquipAttr2, 
            Language.Common.AttrList[v.attr_type], 
            DataHelper.IsPercent(v.attr_type) and Percent(v.attr_value) or v.attr_value)

            self.AttrObjList[k]:SetActive(true)
            UH.SetText(self.AttrList[k], str)
        else 
            self.AttrObjList[k]:SetActive(false)
        end 
    end 

    self.send_data.grid_item = data

    self.old_grid_index = data.index
end
