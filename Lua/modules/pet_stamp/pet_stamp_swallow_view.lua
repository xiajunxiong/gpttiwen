
-- 宠物神印吞噬界面
PetStampSwallowView = PetStampSwallowView or DeclareView("PetStampSwallowView", "pet/pet_stamp/pet_stamp_swallow")
VIEW_DECLARE_MASK(PetStampSwallowView, ViewMask.BgBlock)
function PetStampSwallowView:PetStampSwallowView()
    self.data = PetStampData.Instance
end

function PetStampSwallowView:LoadCallback()
    self.Board:SetData(PetStampSwallowView, Language.PetStamp.PetStampSwallow,Vector2.New(816, 606))
end

function PetStampSwallowView:CloseCallback()
    self.data:SetSwallowCo()
    self.data:SetSwallowMain()
    self.data.swallow_smart.attri_index = -1
end

-------吞噬界面panel-------------------------------------
PetStampSwallowPanel = PetStampSwallowPanel or DeclareMono("PetStampSwallowPanel", UIWFlushPanel)
function PetStampSwallowPanel:PetStampSwallowPanel()
    self.data = PetStampData.Instance
    self.main_item = self.data:GetSwallowMain()
    self.data_cares = {
        {data = self.data.swallow_smart, keys = {"main_index"}, func = self.FlushLeftItem},
        {data = self.data.swallow_smart, keys = {"co_index"}, func = self.FlushRightItemInfo}
    }
end

function PetStampSwallowPanel:FlushLeftItem()
    self.LeftStampItem:SetData({stamp = self.main_item})
    self:SetMainAttrList()
    self:SetSwallowCost()
end

function PetStampSwallowPanel:SetMainAttrList()
    self.LeftAttrList:SetData(self.main_item:AttriListInfo())
end

function PetStampSwallowPanel:SetSwallowCost()
    self.cost_num = self.data:GetSwallowCost()
    UH.SetText(self.SwallowCost, Language.PetStamp.SwallowCost..RichTextHelper.SpriteStrAtlas(CommonItemId.Gold, 50, 50) .. self.cost_num)
end

function PetStampSwallowPanel:FlushRightItemInfo()
    if self.data.swallow_smart.co_index == -1 then
        self:ClearRightInfo()
    else
        local item = self.data:GetSwallowCo()
        self.RightStampItem:SetData({stamp = item})
        self.RightAttrList:SetObjActive(true)
        self.RightAttrList:SetData(item:AddAttri())
    end
end

function PetStampSwallowPanel:ClearRightInfo()
    self.RightAttrList:SetObjActive(false)
    self.RightStampItem:ClearData()
end

function PetStampSwallowPanel:OnClickSwallow()
    --无吞噬神印
    if self.data.swallow_smart.co_index == -1 then
        PublicPopupCtrl.Instance:Center(Language.PetStamp.NotHaveCoStamp)

    --未选择吞噬属性
    elseif self.data.swallow_smart.attri_index == -1 then
        PublicPopupCtrl.Instance:Center(Language.PetStamp.NotHaveSwallowAttr)

    --元宝不足
    elseif MallCtrl.IsNotCurrency(CurrencyType.Gold, self.cost_num) then
        PetCtrl.SendPetOp(PET_OP_TYPE.PetGodPrintDevour, self.data.swallow_smart.main_index, self.data.swallow_smart.co_index, self.data.swallow_smart.attri_index)
    end
end

---------吞噬界面被吞噬的神印属性（用于选择属性）--------------------------
PetStampSwallowAttri = PetStampSwallowAttri or DeclareMono("PetStampSwallowAttri", UIWidgetBaseItem)
function PetStampSwallowAttri:PetStampSwallowAttri()
end

function PetStampSwallowAttri:SetData(data)
    UIWidgetBaseItem.SetData(self,data)

    local name = (Language.Common.AttrList2[data.attri_type] or "") .. Language.Common.MaoHao
    UH.SetText(self.AttriName,name)

    local attri_value_str = ""
    if DataHelper.IsPercent(data.attri_type) then
        attri_value_str = string.format("+%s%%",data.attri_value/10)
    else
        attri_value_str = string.format("+%s",data.attri_value)
    end
    UH.SetText(self.AttriValue,attri_value_str)
end

function PetStampSwallowAttri:OnClickToggle()
    PetStampData.Instance.swallow_smart.attri_index = self.data.attri_index
end

--------吞噬界面被吞噬的神印item-------------------
PetStampSwallowCoItem = PetStampSwallowCoItem or DeclareMono("PetStampSwallowCoItem", UIWidgetBaseItem)
function PetStampSwallowCoItem:PetStampSwallowCoItem()
end

function PetStampSwallowCoItem:SetData(data)
    UIWidgetBaseItem.SetData(self,data)
    local pos_index = data.stamp:PosIndex()
    local stamp = data.stamp
    self.AddImg:SetActive(false)
    self.LevelObj:SetActive(true)
    UH.SetIcon(self.Icon, stamp:Icon())
    self.Effect:SetActive(true)
    UH.SetText(self.Pos, DataHelper.GetDaXie(pos_index, true))
    UH.SetText(self.Level, string.format(Language.PetStamp.Level,stamp:Level()))
end

function PetStampSwallowCoItem:ClearData()
    self.data = nil
    self.LevelObj:SetActive(false)
    UH.SetIcon(self.Icon, nil)
    self.AddImg:SetActive(true)
    self.Effect:SetActive(false)
end

function PetStampSwallowCoItem:OnClick()
    -- LogError("OnClick ",self.data == nil)
    -- if self.data == nil then
        ViewMgr:OpenView(PetStampSwallowSelectView)
    -- end
end

--------吞噬选择界面----------------------------------
PetStampSwallowSelectView = PetStampSwallowSelectView or DeclareView("PetStampSwallowSelectView","pet/pet_stamp/pet_stamp_swallow_select")
VIEW_DECLARE_MASK(PetStampSwallowSelectView, ViewMask.BgBlockClose)
function PetStampSwallowSelectView:PetStampSwallowSelectView()
    self.data = PetStampData.Instance
end

function PetStampSwallowSelectView:LoadCallback()
    local select_list = self.data:GetSwallowSelectList()
    if next(select_list) == nil then
        self.EmptyState:SetActive(true)
    else
        self.EmptyState:SetActive(false)
    end
    self.StampList:SetData(select_list)
end

------吞噬选择界面item-------------------------
PetStampSwallowSelectItemInfo = PetStampSwallowSelectItemInfo or DeclareMono("PetStampSwallowSelectItemInfo", UIWidgetBaseItem)
--StampItem PetStampBagItem
function PetStampSwallowSelectItemInfo:PetStampSwallowSelectItemInfo()
end

function PetStampSwallowSelectItemInfo:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    self.StampItem:SetData(data)
    self.AttrList:SetData(data:AddAttri())
end

function PetStampSwallowSelectItemInfo:OnSelectToggle()
    PetStampData.Instance:SetSwallowCo(self.data)
    ViewMgr:CloseView(PetStampSwallowSelectView)
end

-----吞噬成功-------------------------------
PetStampSwallowSucc = PetStampSwallowSucc or DeclareView("PetStampSwallowSucc","pet/pet_stamp/pet_stamp_swallow_succ")
VIEW_DECLARE_MASK(PetStampSwallowSucc, ViewMask.BgBlockClose)
function PetStampSwallowSucc:PetStampSwallowSucc()
    self.data = PetStampData.Instance
end

function PetStampSwallowSucc:LoadCallback(param)
    self.stamp = self.data:GetPetStampByIndex(param.index)
    self.PetStampItem:SetObjActive(false)
    self.PetStampItem:SetData({stamp = self.stamp})
    self.AttrList:SetObjActive(false)
    UH.SetText(self.StampName, self.stamp:Name())
    local attr_list = TableCopy(self.stamp:AttriList(), 1)
    attr_list = table.rearrange2(attr_list)
    table.insert(attr_list,self.stamp:SwallowAttr())
    table.sort(attr_list, function (a,b)
        if a.is_main ~= nil or b.is_main ~= nil then
            return a.is_main ~= nil
        elseif a.attri_index ~= nil and b.attri_index ~= nil then
            return a.attri_index < b.attri_index
        else
            return a.attri_index ~= nil
        end
    end)
    self.AttrList:SetData(attr_list)
end

function PetStampSwallowSucc:OnEffectEnd()
    self.PetStampItem:SetObjActive(true)
    self.ItemEffect:SetActive(true)
    self.AttrList:SetObjActive(true)
end

------吞噬特效------------------------------------
PetStampSwallowSuccEffectPanel = PetStampSwallowSuccEffectPanel or DeclareMono("PetStampSwallowSuccEffectPanel",UIWFlushPanel)
function PetStampSwallowSuccEffectPanel:PetStampSwallowSuccEffectPanel()
    self.data = PetStampData.Instance
    self.stamp = self.data:GetPetStampByIndex(self.data.swallow_smart.show_succ_index)
end

function PetStampSwallowSuccEffectPanel:OnEnable()
    UH.SetIcon(self.Icon, self.stamp:Icon())
end