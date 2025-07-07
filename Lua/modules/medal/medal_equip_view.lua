MedalEquipView = MedalEquipView or DeclareView("MedalEquipView", "medal/medal_equip")
VIEW_DECLARE_MASK(MedalEquipView,ViewMask.BgBlockClose)
function MedalEquipView:MedalEquipView()

end

function MedalEquipView:OpenCallback()

end 

function MedalEquipView:LoadCallback(param_t)
    self.SortCell1:Trigger()

    self.panel:FlushRight(param_t)
end  

MedalEquipFlushPanel = MedalEquipFlushPanel or DeclareMono("MedalEquipFlushPanel", UIWFlushPanel)

function MedalEquipFlushPanel:MedalEquipFlushPanel()
    self.data = BagData.Instance
    self.data_cares = {
        {data = self.data:ViewData(),func = self.FlushSortCell ,keys = {"medal_info"}},
        -- {data = self.data:GetColumnGrids(ItemColumnType.Item),func = self.FlushSortList},
        {data = self.data:ViewData(),func = self.FlushBagChange ,keys = {"medal_take_off"},init_call = false},
    }

    self.send_data = {}
    self.effect_play = {}
    self.send_data.select_index = 0
    self.init_flag = false
end 

function MedalEquipFlushPanel:FlushSortCell()
    for i = 1,3 do 
        local flag = true
        for k,v in pairs(self.data:ViewData().medal_info) do 
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

function MedalEquipFlushPanel:OnDestroy()
    UIWFlushPanel.OnDestroy(self)
    for index, value in ipairs(self.effect_play) do
        value:DeleteMe()
    end
end 

function MedalEquipFlushPanel:FlushSortList()
    local data_list = self.data:GetGridForMedal() 
    self.SortList:SetData(data_list)
end

function MedalEquipFlushPanel:OnClickPutOn()
    local param_t = {
        req_type = MEDAL_REQ_TYPE.PUT_ON,
        param1 = self.send_data.select_index,
        param2 = self.send_data.grid_item.column_type, 
        param3 = self.send_data.grid_item.index
    }
    BagCtrl.Instance:SendMedalReq(param_t)

end

function MedalEquipFlushPanel:FlushBagChange()
    local data = self.data.medal_take_off_str
    local split_list = Split(data,"|")
    local bag_cell = BagData.Instance:GetItemByIndex(tonumber(split_list[1]),tonumber(split_list[2]))

    if bag_cell ~= nil then 
        self:FlushRight(bag_cell)
    else 
        ViewMgr:CloseView(MedalEquipView)
    end 
end

function MedalEquipFlushPanel:EffectComplete()

    self:FlushRight(BagData.Instance:GetItemByIndex(ItemColumnType.Item,self.old_grid_index))
    -- ViewMgr:CloseView(MedalEquipView)
end

function MedalEquipFlushPanel:OnClickTakeOff()
	local param_t = {
        req_type = MEDAL_REQ_TYPE.TAKE_OFF,
        param1 = self.send_data.select_index
    }
	BagCtrl.Instance:SendMedalReq(param_t)
end

function MedalEquipFlushPanel:OnClickMedalSort(index)
    self.send_data.select_index = index

    -- self.BtnTakeOff:SetActive(true)
    
    -- self:FlushPutButton( )
end

function MedalEquipFlushPanel:OnClickSortCell(data)
    self.send_data.grid_item = data
    
    -- self.BtnPutOn:SetActive(true)

    -- self:FlushPutButton( )
end

function MedalEquipFlushPanel:FlushPutButton( )
    if self.send_data.select_index == nil then return end 
    local index = self.send_data.select_index + 1
    local flag = self["SortCell"..index]:GetData() ~= nil and self["SortCell"..index]:GetData().item_id or 0
    if flag == 0 or flag == nil then 
        self.BtnPutOnText.text = Language.ItemInfo.ItemButtonName.Equip
    else 
        self.BtnPutOnText.text = Language.ItemInfo.ItemButtonName.MedalEquip
    end 
end

function MedalEquipFlushPanel:FlushRight(data)
    self.Name.text = string.format( Language.Medal.MedalNameColor,QualityColorStr[ItemColor.Green],data:Name() ) 
    self.Cell:SetData(data)
    self.Level.text = data:LevelDesc()-- ..Language.Common.Level
    if data.param.sp_id > 0 then 
        local sp_cfg = BagData.Instance:GetEquipEffectCfg(data.param.sp_id)
        if sp_cfg then 
            self.Special.text = string.format(Language.Common.Xstr, QualityColorStr[data:GetMedalQua()], sp_cfg.description)
        else 
            self.Special.text = Language.ItemInfo.None
        end 
        
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

function MedalEquipFlushPanel:PlayEffectEquiped()

end

MedalEquipEffect = MedalEquipEffect or BaseClass()
function MedalEquipEffect:__init(mono, index)
    self.WI = mono
    self.index = index
end
function MedalEquipEffect:__delete()
    if self.handle_run then
        Runner.Instance:RemoveRunUntilTrue(self.handle_run)
        self.handle_run = nil
    end
    if self.handle_delay then
        TimeHelper:CancelTimer(self.handle_delay)
        self.handle_delay = nil
    end
end
function MedalEquipEffect:Play()
    local time = 0
    local effectParent = self.WI.CellEffect[self.index]
    local effectendparent = self.WI.CellEndEffect[self.index]
    local effectNode = self.WI.EffectNode
    local startPos = self.WI.EffectStartObj.localPosition
    local endPos = effectendparent.transform.localPosition
    local crossPos = self.WI.CrossObj.localPosition
    local shoot_curve = Game.CurveParabola(startPos, endPos, crossPos, 0.5)
    
    UH.LocalPos(effectNode.transform, startPos)
    if self.WI.EffectTool.isActiveAndEnabled then 
        self.WI.EffectTool:Play(10010254, effectNode)
    end 
    self.handle_run = Runner.Instance:RunUntilTrue(
        function()
            if shoot_curve then
                time = time + Time.deltaTime * 3
                if time >= 1 then
                    if self.WI and self.WI.EffectTool and self.WI.EffectTool.isActiveAndEnabled then 
                        self.WI.EffectTool:Play(10010863, effectParent)
                    end 
                    AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ZhuangBeiFuShuJia)
                    return true
                end
                if effectNode == nil or not ViewMgr:IsOpen(MedalEquipView) then return true end 
                UH.LocalPos(effectNode.transform, shoot_curve:GetPos(time))
            end
            return false
        end
    )
end 

MedalEquipSortCell = MedalEquipSortCell or DeclareMono("MedalEquipSortCell",UIWidgetBaseItem)

function MedalEquipSortCell:MedalEquipSortCell()

end

function MedalEquipSortCell:SetData(data)
    self.Info:SetActive( data.item_id ~= 0 )
    self.Empty:SetActive( data.item_id == 0 )

    if data.item_id == 0 then 
        return 
    end 

    local item_ = Item.Create(data)
    local color = self.isDark and QualityColorStr or QualityColorStrShallow

    self.Name.text = string.format( Language.Medal.MedalNameColor,color[ItemColor.Green],item_:Name() )  
    self.Cell:SetData(item_)
    self.LimitedLevel.text = item_:LevelDesc()

    if data.param.sp_id > 0 then 
        local sp_cfg = BagData.Instance:GetEquipEffectCfg(data.param.sp_id)
        if sp_cfg then 
            local temp = Format(Language.Common.Xstr, color[item_:GetMedalQua()], item_:EffectDesc())
            
            self.Special.text = temp

            self.Name.text = item_:QuaName(true)
        else 
            self.Special.text = Language.Medal.SpecialSolo..Language.Medal.SpecialNone
        end 
    else
        if not self.isDark then 
            self.Special.text = Language.Medal.SpecialSolo..Language.Medal.SpecialNone
        else 
            self.Special.text = Language.Medal.SpecialSoloDark..Language.Medal.SpecialNoneDark
        end 
    end

    if item_.param.second_sp_id and item_.param.second_sp_id > 0 then
        local temp = Format(Language.Common.Xstr, color[item_:GetMedalSecondQua()], item_:Effect2Desc())
        self.Special2.text = temp
        self.Special2.gameObject:SetActive(true)
    else
        self.Special2.gameObject:SetActive(false)
    end 

    for i = 1,4 do self.AttrObjList[i]:SetActive(false) end 
    for k, v in ipairs(item_:BaseAttr()) do
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

    UIWidgetBaseItem.SetData(self,data)

    local effect_actives = MedalData.Instance:GetActiveEquipMedal()

    if self.Fail then self.Fail:SetActive(effect_actives[data.grid_index+1][1]>1) end 
    if self.Fail2 then self.Fail2:SetActive(effect_actives[data.grid_index+1][2]>1) end 
end

function MedalEquipSortCell:Click()
    if self.ItemClick then
        self.ItemClick:Trigger()
    end
end