--星辰宝图 22099
StarTreasureMapView = StarTreasureMapView or DeclareView("StarTreasureMapView", "star_treasure_map/star_treasure_map_view")

VIEW_DECLARE_LEVEL(StarTreasureMapView,ViewLevel.L1)

StarTreasureMapPanel = StarTreasureMapPanel or DeclareMono("StarTreasureMapPanel", UIWFlushPanel)

function StarTreasureMapPanel:Awake()
    UIWFlushPanel.Awake(self)
    local item = TreasureMapData.Instance:MapInfo()
    local param = item.param or {}
    if self.MapTexture:TextureIsNil() then
        self.MapTexture:SetByPath(MapData.Instance:GetMapMiniPath(param.scene_id))
    end
    self.TargetPos.localPosition = MapCtrl.Instance:WorldToTransformUV(self:GetTargetPos(param),self.MapTex,param.scene_id)

    if self.Effect then
        self.Effect.localPosition = self.TargetPos.localPosition
    end
    local scene_cfg = SceneData.Instance:GetSceneCfg(param.scene_id)
    if scene_cfg and scene_cfg.entry_point == nil then
        BuglyReport("star treasure map data is nil uuid = ", param.client_need_uid)
    end
    local AutoToPosFunc = function()
        local target_pos = SceneCtrl.Instance:CurScene():PosToClint(param.x,param.y)
        local obj = SE:Play(SceneEffectConfig.EffectList.StarTreasureMap,target_pos)
        StarTreasureMapData.Instance:AddEffect(param.client_need_uid1,obj)
        SceneLogic.Instance:AutoToPos(nil,param,function()
            StarTreasureMapCtrl.Instance:EndTouchEnded()
            MainOtherCtrl.Instance:OpenKeyUseView(item)
            Invoke(function ()
                local is_auto = StarTreasureMapData.Instance:GetAutoFlag()
                if is_auto and ViewMgr:IsOpen(KeyUseView) then
                    local item = TreasureMapData.Instance:MapInfo()
                    if item ~= nil and item.item_id ~= 0 then
                        -- if item.key_use_func then
                        --     item.key_use_func()
                        --     item.key_use_func = nil
                        -- end
                        KeyUseData.Instance:RemoveItem()
                        TreasureMapCtrl.Instance:KeyUseItem(item:MapType(),TableCopy(item))
                    end
                else
                    StarTreasureMapData.Instance:SetAutoFlag(false)
                end
            end,5)
        end,true)
    end
    self.ClickEnterFunc = function()
        MainOtherCtrl.Instance:CloseKeyUse()
        if SceneLogic.Instance:SceneID() == param.scene_id then
            AutoToPosFunc()
        else
            SceneLogic.Instance:AutoToPos(param.scene_id,nil,function()AutoToPosFunc()end,nil,true)
        end
    end
end

function StarTreasureMapPanel:OnEnable()
    UIWFlushPanel.OnEnable(self)
end

function StarTreasureMapPanel:OnDestroy()
    UIWFlushPanel.OnDestroy(self)
    TimeHelper:CancelTimer(self.time_ht)
end


function StarTreasureMapPanel:OnClickEnter()
    if self.ClickEnterFunc then
        self.ClickEnterFunc()
        StarTreasureMapCtrl.Instance:StartTouchEnded(self.ClickEnterFunc)
    end
    ViewMgr:CloseView(StarTreasureMapView)
    ViewMgr:CloseView(StarTreasureMapAtlaView)
    TimeHelper:CancelTimer(self.time_ht)
end

function StarTreasureMapPanel:GetTargetPos(param)
    if SceneLogic.Instance:SceneID() ~= param.scene_id then
        local cfg = SceneData.Instance:GetSceneCfg(param.scene_id) or {}
        return GameMath.PosToClint(param.x,param.y,cfg.mapid)
    else
        return SceneCtrl.Instance:CurScene():PosToClint(param.x,param.y)
    end
end

function StarTreasureMapPanel:AutoEnter()
    local is_auto = StarTreasureMapData.Instance:GetAutoFlag()
    if is_auto then
        TimeHelper:CancelTimer(self.time_ht)
        self.time_ht = Invoke(function ()
            local _is_auto = StarTreasureMapData.Instance:GetAutoFlag()
            if _is_auto then
                self:OnClickEnter()
            end
        end,7)
    end
end

--===========================StarTreasureMapExchangeView===========================

StarTreasureMapExchangeView = StarTreasureMapExchangeView or DeclareView("StarTreasureMapExchangeView", "star_treasure_map/star_treasure_map_exchange",Mod.StarTreasureMap.Exchange)

function StarTreasureMapExchangeView:StarTreasureMapExchangeView()
    self.Board:SetData(self:GetType(),nil,Vector2.New(759, 457))
end

function StarTreasureMapExchangeView:LoadCallback(param_t)
    local target_id = TreasureMapData.Instance:GetStarTreasureMapId()
    local ex_item = StarTreasureMapData.Instance:GetExchangeMaterial()
    local map_type = 3
    if ex_item then
        map_type = TreasureMapData.Instance:MapType(ex_item.item_id)
    end
    if map_type == 3 then
        target_id = TreasureMapData.Instance:GetStarTreasureMapId()
    else
        target_id = TreasureMapData.Instance:GetShengQiTreasureMapId()
    end
    self.config = WorkshopData.Instance:GetForgeComposeCfg(target_id)
    local stuff_list = WorkshopData.Instance:GetStuffList(self.config)
    for i=1,self.StuffList:Length() do
        if stuff_list[i] ~= nil then
            self.StuffList[i]:SetData(Item.Init(stuff_list[i].item_id))
            CellFlushs.SetNum(self.StuffList[i],self:GetConsume(stuff_list[i]))
        else
            self.StuffList[i]:SetData(Item.New())
        end
    end
    self.stuff_list = stuff_list
    local target_item = Item.Init(target_id)
    self.TargetCall:SetData(target_item)
    UH.SetText(self.Name,target_item:Name())
    UH.SetText(self.Desc,target_item:Desc())
    UH.SetText(self.Title, Language.StarTreasureMap.TitleList[map_type])
end

function StarTreasureMapExchangeView:GetConsume(data)
    return DataHelper.ConsumeNum(Item.GetNum(data.item_id),data.num,true)
end

function StarTreasureMapExchangeView:IsExchange(stuff_list)
    for k,v in pairs(stuff_list) do
        if Item.GetNum(v.item_id) < v.num then
            return false
        end
    end
    return true
end

function StarTreasureMapExchangeView:OnClickExchange()
    if not self:IsExchange(self.stuff_list) then
        PublicPopupCtrl.Instance:Center(ErrorText[42])
        return
    end
    WorkshopCtrl.Instance:SendCompoundReq(self.config.seq,1)
    ViewMgr:CloseView(StarTreasureMapExchangeView)
end

--===========================StarTreasureMapAtlaView===========================

StarTreasureMapAtlaView = StarTreasureMapAtlaView or DeclareView("StarTreasureMapAtlaView", "star_treasure_map/star_treasure_map_atlas")

VIEW_DECLARE_LEVEL(StarTreasureMapAtlaView,ViewLevel.L1)

function StarTreasureMapAtlaView:StarTreasureMapAtlaView()
    if self.mainCanvas == nil then
		self.mainCanvas = UnityGameObj.FindGameObjectWithTag("MainCanvas")
	end
    if self.mainCanvas and self.mainCanvasCom == nil then
        self.mainCanvasCom = self.mainCanvas:GetComponent(typeof(UnityEngine.Canvas))
    end
    self.index_list = {}
    self.pos = nil
    self.index = nil
    self.effect_handle = {}
    self.line_max_num = 200
    self.is_show_auto = true
end

function StarTreasureMapAtlaView:CloseCallback()
    TimeHelper:CancelTimer(self.ShowTimer)
    TimeHelper:CancelTimer(self.auto_time_ht)
end

function StarTreasureMapAtlaView:OnClickClose()
    TimeHelper:CancelTimer(self.ShowTimer)
    ViewMgr:CloseView(StarTreasureMapAtlaView)
    TreasureMapData.Instance:ClearItem()
    StarTreasureMapData.Instance:SetAutoFlag(false)
end

function StarTreasureMapAtlaView:LoadCallback()
    for i=1,self.BGLine:Length() do
        local pos1 = self.ItemList[i].localPosition
        local pos2 = self.ItemList[i+1].localPosition
        self.BGLine[i]:SetObjActive(true)
        UH.AnchoredPosition(self.BGLine[i],pos1)
        local distance = GameMath.Distance(pos1,pos2,true)
        self.BGLine[i].sizeDelta = Vector2.New(distance,6)
        self.BGLine[i].eulerAngles = Vector3.New(0,0,self:GetAngle(pos1,pos2))
    end
    self:SetAuto()
end

function StarTreasureMapAtlaView:OnClickDown()
    if not self.IsOnClickDown then
        self:OnDragBegin()
        self.IsOnClickDown = true
    end
    self.AutoTip:SetActive(false)
end

function StarTreasureMapAtlaView:OnDragBegin()
    self.is_show_auto = false
    if self.auto_time_ht then
        self:Reset()
        TimeHelper:CancelTimer(self.auto_time_ht)
        self.auto_time_ht = nil
    end
    if self.IsOnClickDown == true then
        self:OnDrag()
        return
    end
    local pos,index = self:GetItemPoint()
    if index ~= 1 then
        self.pos = nil
        return
    end
    if pos ~= nil then
        self.pos,self.index = pos,index
        self:SetLineShow(index,pos)
        self:SetSelectEffect()
        if self.ItemList[index + 1] then
            self.next_angle = self:GetAngle(pos,self.ItemList[index + 1].localPosition)
        end
    end
end

function StarTreasureMapAtlaView:OnDrag()
    if self.pos == nil then
        return
    end
    local pos,index = self:GetItemPoint()
    if pos ~= nil and index ~= self.index and self:IsNotHasItemList(index)then
        self:DrawLine(self.pos,pos)
        self.LineList[self.index]:SetActive(true)
        self.pos,self.index = pos,index
        self:SetLineShow(index,pos)
        self:SetSelectEffect()
        if #self.index_list == self.ItemList:Length() then
            self:SetSelectEffect()
            self.ItemLine[index]:SetObjActive(false)
            return
        end
        if self.ItemList[index + 1] then
            self.next_angle = self:GetAngle(pos,self.ItemList[index + 1].localPosition)
        end
    end
    self:DrawLine(self.pos,UH.ScreenPointToViewPoint(self.ContentRect,self.mainCanvasCom))
end

function StarTreasureMapAtlaView:OnDragEnd()
    if self:IsHasLock() then
        for i=1,self.ItemList:Length() do
            self:Play(i,100108223)
        end
        self.MainAnim:Play()
        self.Panel:AutoEnter()
        self.ShowTimer = Invoke(function()
            local item = TreasureMapData.Instance:MapInfo()
            if item == nil then return end
            BagCtrl.Instance:SendActiveSpecialLogicItem(0,item.column_type,item.index)
        end,4.5)
        return
    end
    self:Reset()
end

function StarTreasureMapAtlaView:Reset()
    self.index_list = {}
    for i=1,self.ItemList:Length() do
        self.ItemLine[i].sizeDelta = Vector2.zero
        self.ItemLine[i]:SetObjActive(false)
        self.LineShow[i]:SetActive(true)
        self.LineList[i]:SetActive(false)
        self.Toggle[i].isOn = false
    end
    for i=1,self.BGLine:Length() do
        self.BGLine[i]:SetObjActive(true)
    end
    self.index = nil
    self.pos = nil
    self.IsOnClickDown = nil
    --self:SetAuto()
end


function StarTreasureMapAtlaView:DrawLine(pos1,pos2)
    local distance = GameMath.Distance(pos1,pos2,true)
    local angle = self:GetAngle(pos1,pos2)
    distance = Valve(distance,400)
    self.ItemLine[self.index].sizeDelta = Vector2.New(distance,6)
    self.ItemLine[self.index].eulerAngles = Vector3.New(0,0,angle)
    if self.next_angle ~= nil and distance > self.line_max_num then
        if math.abs(math.floor(self.next_angle - angle)) > 5 then
            self.ItemLine[self.index].sizeDelta = Vector2.New(self.line_max_num,6)
        end
    end
end

function StarTreasureMapAtlaView:GetItemPoint()
    local localPoint = UH.ScreenPointToViewPoint(self.ContentRect,self.mainCanvasCom)
    for i=1,self.ItemList:Length() do
        if GameMath.Distance(self.ItemList[i].localPosition,localPoint,true) < 50 then
            if self.pos and self.ItemLine[self.index].sizeDelta.x == self.line_max_num then
                return
            end
            return self.ItemList[i].localPosition,i
        end
    end
end

function StarTreasureMapAtlaView:IsHasLock()
    if #self.index_list ~= self.ItemList:Length() then
        return false
    end
    for i=1,self.ItemList:Length() do
        if self.index_list[i] ~= i then
            return false
        end
    end
    return true
end

function StarTreasureMapAtlaView:IsNotHasItemList(index)
    for k,v in pairs(self.index_list) do
        if v == index then
            return false
        end
    end
    return true
end

function StarTreasureMapAtlaView:Play(index,key)
    UH.StopEffect(self.Effect[index],self.effect_handle[index])
    self.effect_handle[index] = UH.PlayEffect(self.Effect[index],key)
end

function StarTreasureMapAtlaView:SetSelectEffect()
    self.Toggle[self.index].isOn = true
    self:Play(self.index,10010360)
end

function StarTreasureMapAtlaView:SetLineShow(index,pos)
    UH.AnchoredPosition(self.ItemLine[index],pos)
    table.insert(self.index_list,index)
    self.ItemLine[index]:SetObjActive(true)
end

function StarTreasureMapAtlaView:GetAngle(pos1,pos2)
    local vecRotation = (pos2 - pos1)
    return Mathf.Atan2(vecRotation.y, vecRotation.x) * Mathf.Rad2Deg
end

function StarTreasureMapAtlaView:SetAuto()
    self.AutoTip:SetActive(true)
    self.AutoTime:TotalTime(5,TimeType.Type_Special_5)
    self.AutoTime:SetCallBack(function ()
        if not self.IsOnClickDown and self.is_show_auto then
            self:StartAuto()
            self.AutoTip:SetActive(false)
        end
    end)
end

function StarTreasureMapAtlaView:AutoFunc()
    if not self.index then
        self.index = 0
    end
    self.index = self.index + 1
    if self.index > self.Toggle:Length() then
        TimeHelper:CancelTimer(self.auto_time_ht)
        self.auto_time_ht = nil
        StarTreasureMapData.Instance:SetAutoFlag(true)
        self:OnDragEnd()
        return
    end
    self:SetSelectEffect()
    local pos_1 = self.ItemList[self.index].localPosition
    self:SetLineShow(self.index, pos_1)
    if self.index < self.Toggle:Length() then
        local poe_2 = self.ItemList[self.index + 1].localPosition
        self:DrawLine(pos_1,poe_2)
        self.LineList[self.index]:SetActive(true)
    end
end

function StarTreasureMapAtlaView:StartAuto()
    TimeHelper:CancelTimer(self.auto_time_ht)
    self.auto_time_ht = TimeHelper:AddRunTimer(function ()
        self:AutoFunc()
    end,0.2)
end