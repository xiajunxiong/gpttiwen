SpiritResolveView = SpiritResolveView or DeclareView("SpiritResolveView", "equip_view/spirit_resolve")
function SpiritResolveView:SpiritResolveView()
    
end
function SpiritResolveView:LoadCallback()
    self.Board:SetData(self:GetType(), Language.Equip.SpiritResolveTitle)
end

function SpiritResolveView:OnHelpClick()
    PublicPopupCtrl.Instance:HelpTip(Config.language_cfg_auto.textdesc[143].desc)
end


--------------------------SpiritResolvePanel--------------------------
SpiritResolvePanel = SpiritResolvePanel or DeclareMono("SpiritResolvePanel", UIWFlushPanel)
function SpiritResolvePanel:SpiritResolvePanel()
    self.Data = EquipData.Instance
    self.data = self.Data.spirit_resolve_data
    self.language = Language.Equip
    self.data_cares = {
        {data = BagData.Instance:GetColumnGrids(ItemColumnType.Item), func = self.onFlush, init_call = false},
        {data = self.data, func = self.OnSelect, init_call = false},
    }
end

function SpiritResolvePanel:onFlush()
    self.data.now_select_seq = -1
    self.data.select_list = {}
    self.spirit_resolve_list_data = self.Data:GetSpiritResolveListData()
    self.SpiritList:SetData(self.spirit_resolve_list_data)
    UH.SetText(self.TxtNowDust, RoleData.Instance:GetCurrency(CurrencyType.DustCoin))
    self:OnSelect()
end

function SpiritResolvePanel:OnSelect()
    self.ObjCount:SetActive(self.data.now_select_seq ~= -1)
    if self.data.now_select_seq ~= -1 then
        UH.SetText(self.TxtNum, self.data.select_list[self.data.now_select_seq].now_select_num)
    end
    self.TxtAddDust:SetObjActive(not TableIsEmpty(self.data.select_list))
    local dust_num = EquipData.Instance:GetCanGetDustNum(self.data.select_list)
    UH.SetText(self.TxtAddDust, Format(self.language.SpiritResolveAdd, dust_num))
end

function SpiritResolvePanel:OnResolveClick()
    if self.is_playing == true then
        return
    end
    if TableIsEmpty(self.data.select_list) then
        PublicPopupCtrl.Instance:Center(self.language.SpiritResolveNoSelectTip)
        return
    end
    self.is_playing = true
    AudioMgr:PlayEffect(AudioType.UI, CommonAudio.FaBaoShengJie)
    --做个动画做完再发协议
    for k, v in pairs(self.data.select_list) do
        local mono = self.SpiritList:GetItem(v.seq)
        mono:PlayAnim()
    end
    self.handle_delay =
        TimeHelper:AddDelayTimer(
        function()
            self.EffectTool:Play(30000825)
        end,
        0.4
    )
    self.handle_delay1 =
        TimeHelper:AddDelayTimer(
        function()
            local tab = {}
            for k, v in pairs(self.data.select_list) do
                local t = {}
                t.column = v.column
                t.index = v.index
                t.num = v.now_select_num
                table.insert(tab, t)
            end
            EquipCtrl.Instance:SendSpiritResolveReq(tab)
            self.is_playing = false
        end,
        2.2
    )
end

function SpiritResolvePanel:OnDestroy()
    UIWFlushPanel.OnDestroy(self)
    TimeHelper:CancelTimer(self.handle_delay)
    TimeHelper:CancelTimer(self.handle_delay1)
    TimeHelper:CancelTimer(self.time_func)
    self.data.now_select_seq = -1
    self.data.select_list = {}
end

function SpiritResolvePanel:OnClickAddNum(num)
    if self.data.now_select_seq == -1 then
        return
    end
    local mono = self.SpiritList:GetItem(self.data.now_select_seq)
    local mono_data = mono:GetData()

    if mono_data.now_select_num + num < 1 then
        PublicPopupCtrl.Instance:Center(self.language.SpiritResolveMinTip)
        mono_data.now_select_num = 1
        return
    elseif mono_data.now_select_num + num > mono_data.item.num then
        PublicPopupCtrl.Instance:Center(self.language.SpiritResolveMaxTip)
        mono_data.now_select_num = mono_data.item.num
        return
    else
        mono_data.now_select_num = mono_data.now_select_num + num
    end
    mono:ChangeCellShowNum()
    self:OnSelect()
end

function SpiritResolvePanel:OnClickDown(num)
    TimeHelper:CancelTimer(self.time_func)
    self.start_time = 0
    if self.data.now_select_seq == -1 then
        return
    end
    self.time_func = TimeHelper:AddRunTimer(function()
        self.start_time = self.start_time + 0.1
        if self.start_time > 0.2 then
            self:OnClickAddNum(num)
        end
	end, 0.1)
end

function SpiritResolvePanel:OnClickUp(num)
    TimeHelper:CancelTimer(self.time_func)
    self.start_time = 0
end


--------------------------SpiritResolveEquipItem--------------------------
SpiritResolveEquipItem = SpiritResolveEquipItem or DeclareMono("SpiritResolveEquipItem", UIWidgetBaseItem)
function SpiritResolveEquipItem:SpiritResolveEquipItem()
    self.is_long_click = false
end

function SpiritResolveEquipItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    self.is_first = true
    TimeHelper:CancelTimer(self.handle_delay)
    TimeHelper:CancelTimer(self.time_func)
    UH.LocalScale(self.AnimRect, Vector3.one)
    self.Cell:SetData(Item.Create(self.data.item))
    self:ChangeCellShowNum()
    self.SpiritName:SetData({item_id = data.item.item_id})
    self.Cell.MengBan.transform:SetAsLastSibling()
    self:OnSelect(false)
    self.is_first = false
end

function SpiritResolveEquipItem:OnSelect(selected)
    if self.panel.is_playing == true then
        return
    end
    if not self.is_long_click then
        local total = self.List:GetSelectedData()
        if table.nums(total) > EQUIP_SPIRIT_RESOLVE_MAX then
            PublicPopupCtrl.Instance:Center(Format(Language.Equip.SpiritResolveCountMaxTip, EQUIP_SPIRIT_RESOLVE_MAX))
            selected = false
        end
        if not self.is_first then
            self.panel:OnSelect()
            if selected then
                AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ItemInfoOpen)
            else
                AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ItemInfoClose)
            end
        end
        if selected then
            if self.data.now_select_num == 0 then
                self.data.now_select_num = self.data.item.num
            end
            EquipData.Instance.spirit_resolve_data.select_list[self.data.seq] = self.data
        else
            if self.data.now_select_seq == self.data.seq then
                self.data.now_select_num = 0
            end
            EquipData.Instance.spirit_resolve_data.select_list[self.data.seq] = nil
        end
        self:SetSelected(selected)
        self.Cell.MengBan:SetActive(selected)
        self.ObjSelect:SetActive(selected)
        self:ChangeCellShowNum()
        EquipData.Instance.spirit_resolve_data.now_select_seq = selected and self.data.seq or -1
    end
end

-- 改变道具格子文本显示
function SpiritResolveEquipItem:ChangeCellShowNum()
    UH.SetText(self.TxtCount, Format(Language.Common.Slash, self.data.now_select_num, self.data.item.num))
end

function SpiritResolveEquipItem:OnPressUp()
    TimeHelper:CancelTimer(self.time_func)
    self.is_long_click = false
end

function SpiritResolveEquipItem:OnDestroy()
    TimeHelper:CancelTimer(self.handle_delay)
    TimeHelper:CancelTimer(self.time_func)
end

function SpiritResolveEquipItem:PlayAnim()
    TimeHelper:CancelTimer(self.handle_delay)
    self.Anim:Play()
    self.handle_delay =
        TimeHelper:AddDelayTimer(
        function()
            self.EffectTool:Play(30000813)
        end,
        0.3336
    )
end

function SpiritResolveEquipItem:OnPressDown()
    self.is_long_click = false
    TimeHelper:CancelTimer(self.time_func)
    local time = 0
    self.time_func =
        TimeHelper:AddRunTimer(
        function()
            time = time + 0.2
            if time > 1 then
                self.is_long_click = true
                CellClicks.BagGridNoButton(self.Cell)
            end
        end,
        0.2
    )
end
