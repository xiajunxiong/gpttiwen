YouMingReQuaPanel = YouMingReQuaPanel or DeclareMono("YouMingReQuaPanel", UIWFlushPanel)

function YouMingReQuaPanel:YouMingReQuaPanel()
    self.data = YouMingData.Instance
    self.data_cares = {{
        data = self.data:GetRemodelYouMing(),
        func = self.FlushBaseInfo
    }, {
        data = self.data:GetRemodelUse(),
        func = self.FlushFruitUse,
        keys = {"requa_item"}
    }}
    self.select_item = nil
end
function YouMingReQuaPanel:FlushBaseInfo()
    self.youming = self.data:GetRemodelYouMing()
    if self.youming.Aptitude == nil then
        return
    end
    local item_id = self.data:GetReQuaItem()
    self.requ_use_item_id = item_id
    self.Cell:SetDataByItemIdWithNeed(item_id, 1)
    UH.SetText(self.Name, Item.GetName(item_id))
    UH.SetText(self.Desc, Format(Language.YouMingPractice.ReQuaTip, self.youming:TotalRemodelCount()))
    local attr_data = self.youming:AptitudeList()
    self.ZiZhi1:SetData(attr_data)
    local requa_data = self.youming:ReQuaAptitudeList()
    self.ZiZhi2:SetData(requa_data)
    local is_has = self.youming:IsHasReshape()
    self.SaveRoot:SetActive(is_has)
    self.BtnReQua:SetActive(not is_has)
    -- 选择果实
    self:FlushFruitUse()
end
function YouMingReQuaPanel:OnClickGiveup()
    YouMingCtrl.Instance:SendReq(YouMingCfg.ReqType.SAVE_OR_CANCEL_REMODELING,self.youming.index, 0)
end
function YouMingReQuaPanel:OnClickSave()
    YouMingCtrl.Instance:SendReq(YouMingCfg.ReqType.SAVE_OR_CANCEL_REMODELING,self.youming.index, 1)
end
function YouMingReQuaPanel:FlushFruitUse()
    self.select_item = self.data:GetRemodelUse().requa_item
    if self.select_item == nil or self.select_item.item_id == 0 then
        self.Cell2:SetObjActive(false)
        self.AddItem:SetActive(true)
        UH.SetText(self.Name2, "")
    else
        self.Cell2:SetObjActive(true)
        self.AddItem:SetActive(false)
        self.Cell2:SetDataByItemIdWithZeroNum(self.select_item.item_id)
        UH.SetText(self.Name2, self.select_item:Name())
    end
end
function YouMingReQuaPanel:OnClickUse()
    if self.youming:IsHasReshape() then
        PublicPopupCtrl.Instance:Center(Language.YouMingPractice.UnSaveOrCancel)
        return
    end
    if self.select_item ~= nil then
        if Item.GetNum(self.select_item.item_id) == 0 then
            PublicPopupCtrl.Instance:Center(Language.YouMingPractice.ReQuaZero)
            return
        elseif not self.youming:HasPerfectRemodel() then
            PublicPopupCtrl.Instance:Center(Language.YouMingPractice.OnlyPerfectCanUseFruit)
            return
        end
        -- 只有存在完美改造才能进行果实重置点数
        YouMingCtrl.Instance:SendReq(YouMingCfg.ReqType.USE_FRUIT, self.youming.index, self.select_item.item_id)
    else
        PublicPopupCtrl.Instance:Center(Language.YouMingPractice.ReQuaZero2)
    end
end
function YouMingReQuaPanel:OnClickAdd()
    ViewMgr:OpenView(YouMingReQuaUse)
end
function YouMingReQuaPanel:OnClickReQua()
    if not self.youming:IsRemodeled(1) then
        PublicPopupCtrl.Instance:Center(Language.YouMingPractice.Text9)
        return
    end
    local is_has, _ = self.youming:IsHasTemp()
    if is_has then
        PublicPopupCtrl.Instance:Center(Language.YouMingPractice.UnSaveOrCancel2)
        return
    end
    if self.requ_use_item_id then
        if Item.GetNum(self.requ_use_item_id) <= 0 then
            PublicPopupCtrl.Instance:Center(Language.YouMingPractice.Text5)
            return
        end
    end
    
    YouMingCtrl.Instance:SendReq(YouMingCfg.ReqType.REMODELING, self.youming.index)
end
function YouMingReQuaPanel:OnClickSet()
    ViewMgr:OpenView(YouMingReQuaSetPoint)
end

-- ==果实使用
YouMingReQuaUse = YouMingReQuaUse or DeclareView("YouMingReQuaUse", "youming_remodel/youming_remodel_fruit")
function YouMingReQuaUse:YouMingReQuaUse()
    self.data = YouMingData.Instance
    self.select_item = nil
end
function YouMingReQuaUse:LoadCallback()
    self.Board:SetData(self:GetType(), Language.YouMingPractice.FruitUse, Vector2.New(560, 440))
    self.data:SetReQuaUse(nil)
    -- self.EmptyTip
    local list = self.data:RemodelFruits()
    self.List:SetData(list)
    self.EmptyTip:SetActive(table.nums(list) == 0)
end

function YouMingReQuaUse:OnClickOk()
    self.data:SetReQuaUse(self.select_item)
    ViewMgr:CloseView(YouMingReQuaUse)
end

function YouMingReQuaUse:OnGetWayClick()
    local get_way_info = {}
    local get_way_t = Split(Config.dim_pet_cfg_auto.other[1].fruit_get_way,"|")
    for k,v in pairs(get_way_t) do
        table.insert(get_way_info, BagData.Instance:GetWayCfg(tonumber(v)))
    end
    ViewMgr:OpenView(PetGuideTipView, {list = get_way_info, tip = Language.YouMingPractice.Text3})
end
function YouMingReQuaUse:OnClickItem(data)
    self.select_item = data
end

YouMingReQuaUseItem = YouMingReQuaUseItem or DeclareMono("YouMingReQuaUseItem", UIWidgetBaseItem)
function YouMingReQuaUseItem:YouMingReQuaUseItem()

end
function YouMingReQuaUseItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    self.Cell:SetData(data)
end
function YouMingReQuaUseItem:Click()
    self.EventClick:Trigger()
end

function YouMingReQuaUseItem:OnSelect(selected)
    if selected then
        self:Click()
    end
end

-- ==果实使用
YouMingReQuaSetPoint = YouMingReQuaSetPoint or
                           DeclareView("YouMingReQuaSetPoint", "youming_remodel/youming_remodel_point")
function YouMingReQuaSetPoint:YouMingReQuaSetPoint()
    self.data = YouMingData.Instance
    self.select_item = nil
    self.free_point = 0
    self.point_map = {}
    self.handle_care = self.data:GetRemodelYouMing():Care(BindTool.Bind(self.FlushSetPoint, self))
end
function YouMingReQuaSetPoint:CloseCallback()
    if self.handle_care then
        self.data:GetRemodelYouMing():Uncare(self.handle_care)
        self.handle_care = nil
    end
end
function YouMingReQuaSetPoint:FlushSetPoint()
    LogDG("FlushSetPoint")
    self.youming = self.data:GetRemodelYouMing()
    if self.youming.Aptitude == nil then
        return
    end
    local attr_data = self.youming:AptitudeList()
    self.ZiZhi:SetData(attr_data)
    self.free_point = self.youming:FreePoint()
    -- self.free_point = 10
    for i = 1, 5 do
        self.PointItems[i]:SetData({index = i})
    end
    UH.SetText(self.Count, self.free_point)
end
function YouMingReQuaSetPoint:LoadCallback()
    self.Board:SetData(self:GetType(), Language.YouMingPractice.SetPoint, Vector2.New(776, 516))
    self.youming = self.data:GetRemodelYouMing()
    if self.youming.Aptitude == nil then
        return
    end
    local attr_data = self.youming:AptitudeList()
    self.ZiZhi:SetData(attr_data)
    self.free_point = self.youming:FreePoint()
    -- self.free_point = 10
    for i = 1, 5 do
        self.PointItems[i]:SetData({index = i})
    end
    UH.SetText(self.Count, self.free_point)
end

function YouMingReQuaSetPoint:OnClickOk()
    -- ADD_POINT_TYPE_MAX 0开始
    -- YouMingCtrl.Instance:SendReq(YouMingCfg.ReqType.ADD_GROWTH)
    for k, v in pairs(self.point_map) do
        for i = 1, v do
            YouMingCtrl.Instance:SendReq(YouMingCfg.ReqType.ADD_GROWTH, self.youming.index, k - 1)
        end
    end
end
function YouMingReQuaSetPoint:OnClickAdd(index)
    if self.free_point == 0 then
        PublicPopupCtrl.Instance:Center(Language.YouMingPractice.PointZero)
        return
    end    
    self.free_point = self.free_point - 1
    self.point_map[index] = (self.point_map[index] or 0) + 1
    self.PointItems[index]:SetCount(self.point_map[index])
    UH.SetText(self.Count, self.free_point)
end
function YouMingReQuaSetPoint:OnClickSub(index)
    --[[ if self.free_point == 0 then
        PublicPopupCtrl.Instance:Center(Language.YouMingPractice.PointZero)
        return
    end ]]
    if self.point_map[index] == nil or self.point_map[index] == 0 then
        PublicPopupCtrl.Instance:Center(Language.YouMingPractice.PointZero)
        return
    end
    self.free_point = self.free_point + 1
    self.point_map[index] = (self.point_map[index] or 0) - 1
    self.PointItems[index]:SetCount(self.point_map[index])
    UH.SetText(self.Count, self.free_point)
end

YouMingPointItem = YouMingPointItem or DeclareMono("YouMingPointItem", UIWidgetBaseItem)
function YouMingPointItem:YouMingPointItem()

end
function YouMingPointItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    UH.SetText(self.Count, 0)
end
function YouMingPointItem:SetCount(num)
    UH.SetText(self.Count, num > 0 and Format(Language.YouMingPractice.PointColor, num) or num)
end