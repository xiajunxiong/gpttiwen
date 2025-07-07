----------------------------IllustrateRareView----------------------------
IllustrateRareView = IllustrateRareView or DeclareMono("IllustrateRareView", UIWFlushPanel)
function IllustrateRareView:IllustrateRareView()
    self.Data = IllustrateData.Instance
    self.language = Language.Illustrate.Rare
    self.data = self.Data.rare_data
    self.data_cares = {
        {data = self.data, func = self.FlushAll, keys = {"info"}, init_call = false},
        {data = self.data, func = self.FlushItem, keys = {"remind"}, init_call = false},
    }
end

function IllustrateRareView:Awake()
    UIWFlushPanel.Awake(self)
    self:FlushAll()
end

function IllustrateRareView:FlushAll()
    self.all_attr_data = self.Data:GetRareAllAttr(true)
    self.AllAttrList:SetData(self:DealAllAttr())
    self:FlushItem()
end

function IllustrateRareView:DealAllAttr()
    local list = {}
    for k, v in pairs(self.all_attr_data) do
        local t = {}
        t.txt = Format(self.language.Attr1, v.attr_name, v.attr_percent_value)
        table.insert(list, t)
    end
    return list
end

function IllustrateRareView:FlushItem()
    self.List:SetData(self.Data:GetRareListData())
end

function IllustrateRareView:OnAllAttrShowClick()
    self.ObjAllAttr:SetActive(true)
end

function IllustrateRareView:OnAllAttrCloseClick()
    self.ObjAllAttr:SetActive(false)
end

function IllustrateRareView:OnDestroy()
    UIWFlushPanel.OnDestroy(self)
end

--------------IllustrateRareListItem--------------
IllustrateRareListItem = IllustrateRareListItem or DeclareMono("IllustrateRareListItem", UIWidgetBaseItem)
function IllustrateRareListItem:IllustrateRareListItem()
    self.Data = IllustrateData.Instance
    self.language = Language.Illustrate.Rare
end

function IllustrateRareListItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    self.List:SetData(data.list)
    local num1, num2 = self.Data:GeRareGroupNum(data.list)
    local color1 = num1 == num2 and COLORSTR.Green9 or COLORSTR.Red1
    local color2 = num1 == num2 and COLORSTR.Green9 or COLORSTR.Red20
    UH.SetText(self.TxtNum, Format(self.language.Num, color1, color2, num1, num2))
    UH.SpriteName(self.ImgTitle, IllustrateConfig.Rare.Title..data.color)
end

function IllustrateRareListItem:OnDestroy()
    
end

--------------IllustrateRareCardItem--------------
IllustrateRareCardItem = IllustrateRareCardItem or DeclareMono("IllustrateRareCardItem", UIWidgetBaseItem)
function IllustrateRareCardItem:IllustrateRareCardItem()
    self.Data = IllustrateData.Instance
    self.language = Language.Illustrate.Rare
    self.is_first = true
    self.now_lv = 0
end

function IllustrateRareCardItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    self.color = Item.GetColor(data.id)
    self.level = self.Data:GetRareInfo(data.seq)
    self.is_max = self.level == self.Data:RareMaxLevel()
    self:FirstDo()
    UH.SetText(self.TxtLevel, self.level)
    -- local score = self.Data:RareScore(data.seq, self.level == 0 and 1 or self.level)
    -- UH.SetText(self.TxtScore, Format(self.language.Score, score))
    UH.SetText(self.TxtScore, ColorStr(self.language.Card[self.color], QualityColorStrShallow[self.color]))
    self.is_can, self.btn_tip = self.Data:RareIsCanLvUp(data.seq)
    self.Remind1:SetNum(self.is_can and 1 or 0)
    self.ObjMask:SetActive(self.level == 0)
    self.ObjCannotActive:SetActive(not self.is_can)
    self.ObjCanActive:SetActive(self.is_can)
    UH.SetText(self.TxtLv2, Format(self.language.Lv, self.level, self.Data:RareMaxLevel()))
    self.AttrList:SetData(self:GetAttrData())
    self.ObjNormal:SetActive(not self.is_max)
    self.ObjMax:SetActive(self.is_max)
    self.Remind2:SetNum(self.is_can and 1 or 0)
    self.ObjCanUpEffect:SetActive(self.is_can)

    local num = DataHelper.ConsumeNum(Item.GetNum(data.id), self.Data:RareNeedCard(data.seq))
    self.Cell:SetData(Item.Create({item_id = data.id, is_bind = true, num = num}))
    
    if self.level == 0 and self.is_can then
        self:OnTurnClick(true)
    end

    self:PlayEffect()
end

function IllustrateRareCardItem:GetAttrData()
    local list = {}
    -- 拿满级的属性去取0，然后在拿当前属性去赋值
    local zero_attr
    if self.is_max then
        zero_attr = self.Data:GetRareAttrBySeq(self.data.seq, self.Data:RareMaxLevel())
    else
        zero_attr = self.Data:GetRareAttrBySeq(self.data.seq, self.Data:RareMaxLevel(), true)
    end
    local next_attr = {}
    if self.level > 0 and (not self.is_max) then
        local now_attr = self.Data:GetRareAttrBySeq(self.data.seq, self.level, false, true)
        for k, v in pairs(zero_attr) do
            if now_attr[v.attr_type] then
                zero_attr[k] = CommonStruct.AttrStruct(v.attr_type, now_attr[v.attr_type].attr_value)
            end
        end
    end
    if (not self.is_max) then
        next_attr = self.Data:GetRareAttrBySeq(self.data.seq, self.level + 1, false, true)
    end
    for k, v in pairs(zero_attr) do
        local t = {}
        t.now = v
        if next_attr[v.attr_type] then
            t.next = next_attr[v.attr_type]
        end
        table.insert(list, t)
    end
    return list
end

function IllustrateRareCardItem:FirstDo()
    if self.is_first then
        local bg_path = self.Data:RareIconPath(self.data)
        ResMgr:LoadObject(bg_path, function(tex)
            if not self:Destroyed() then
                if self.ImgCard then
                    self.ImgCard.texture = tex
                end
            end
        end, false)

        UH.SpriteName(self.ImgBg, IllustrateConfig.Rare.Bg .. self.color)
        UH.SpriteName(self.ImgFrame, IllustrateConfig.Rare.Frame .. self.color)
        UH.SpriteName(self.ImgLv, IllustrateConfig.Rare.Name .. self.color)
        UH.SetText(self.TxtName, self.data.name)
        self.now_lv = self.level
        self.is_first = false
    end
end

function IllustrateRareCardItem:PlayEffect()
    if self.now_lv ~= self.level then
        if self.ht then
            UH.StopEffect(self.EffectTool, self.ht)
        end
        self.ht = UH.PlayEffect(self.EffectTool, "4165112")
        self.now_lv = self.level
    end
end

function IllustrateRareCardItem:OnTurnClick(value)
    self.Anim:SetBool("state", value)
end

function IllustrateRareCardItem:OnLvUpClick()
    if self.is_can then
        IllustrateCtrl.Instance:SendCollectSeq(IllustrateConfig.Req.Treasure, self.data.seq)
    elseif self.btn_tip then
        PublicPopupCtrl.Instance:Center(self.language.Tip[self.btn_tip])
        if self.btn_tip == 2 then
            ViewMgr:OpenView(GetWayView, {item = Item.Create({item_id = self.data.id})})
        end
    end
end

function IllustrateRareCardItem:OnDestroy()

end

--------------IllustrateRareAttrItem--------------
IllustrateRareAttrItem = IllustrateRareAttrItem or DeclareMono("IllustrateRareAttrItem", UIWidgetBaseItem)
function IllustrateRareAttrItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    UH.SetText(self.TxtName, data.attr_name)
    UH.SetText(self.TxtValue, data.attr_percent_value)
end

--------------IllustrateRareTxtItem--------------
IllustrateRareTxtItem = IllustrateRareTxtItem or DeclareMono("IllustrateRareTxtItem", UIWidgetBaseItem)
function IllustrateRareTxtItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    UH.SetText(self.Txt, data.txt)
end

--------------IllustrateRareAttrItem2--------------
IllustrateRareAttrItem2 = IllustrateRareAttrItem2 or DeclareMono("IllustrateRareAttrItem2", UIWidgetBaseItem)
function IllustrateRareAttrItem2:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    UH.SetText(self.TxtName, data.now.attr_name)
    UH.SetText(self.TxtValue, data.now.attr_percent_value)
    if data.next then
        self.ObjCanAdd:SetActive(true)
        UH.SetText(self.TxtAddValue, DataHelper.Percent(data.now.attr_type, data.next.attr_value - data.now.attr_value))
    else
        self.ObjCanAdd:SetActive(false)
    end
end