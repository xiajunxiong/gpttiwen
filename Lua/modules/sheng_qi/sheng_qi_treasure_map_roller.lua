ShengQiTreasureMapRollerView = ShengQiTreasureMapRollerView or DeclareView("ShengQiTreasureMapRollerView", "sheng_qi/shengqi_treasure_map_roller")

VIEW_DECLARE_LEVEL(ShengQiTreasureMapRollerView,ViewLevel.L1)

function ShengQiTreasureMapRollerView:ShengQiTreasureMapRollerView()
    if self.mainCanvas == nil then
		self.mainCanvas = UnityGameObj.FindGameObjectWithTag("MainCanvas")
	end
    if self.mainCanvas and self.mainCanvasCom == nil then
        self.mainCanvasCom = self.mainCanvas:GetComponent(typeof(UnityEngine.Canvas))
    end
	self.cur_turn = 1
    self.end_time = 100
    self.speed_time=0.03
    self.round_num = 5  
    self.turn_time = 0.5
    self.turn_timer = 0
    self.start_pos = Vector3.New(537.5,-242,0)
    self.target_pos = Vector3.New(10,-20,0)
end

function ShengQiTreasureMapRollerView:CloseCallback()
    TimeHelper:CancelTimer(self.ShowTimer)
    TimeHelper:CancelTimer(self.AnimTimer)
    MainOtherCtrl.Instance.stop = false
    StarTreasureMapData.Instance:NextShengQiTreasureMap()
end

function ShengQiTreasureMapRollerView:OnClickClose()
    ViewMgr:CloseView(ShengQiTreasureMapRollerView)
end

function ShengQiTreasureMapRollerView:LoadCallback(param_t)
    local config = StarTreasureMapData.Instance:GetConfig2Data()
    for i=1,self.ItemList:Length() do
        self.ItemList[i]:SetData(config[i])
    end
    self.param_t = param_t
    self.BtnClose:SetActive(false)
end

--=========================动画逻辑=========================
function ShengQiTreasureMapRollerView:OnPlayAnimation()
    self.IsUpdataRoller = true
    self.updata_count = 0
    self.index = self.param_t.index + 1
    self.temp_turn = self.round_num * self.ItemList:Length() + self.param_t.index
    self.ShowTimer = TimeHelper:AddCountDownTT(
        BindTool.Bind1(self.UpdataRollerTime,self),
        function()
            self:FlushEndView()
        end
    ,self.end_time,self.speed_time)
end

function ShengQiTreasureMapRollerView:UpdataRollerTime()
    if self.turn_timer > self.turn_time then
        if self.IsUpdataRoller then
            if self.updata_count > self.temp_turn then
                self.turn_time = self.ItemList:Length() * self.speed_time
                self.IsUpdataRoller = false
            end
            self:FlushCellListView(self.turn_time / 1.5)
            self.updata_count = self.updata_count + 1
        elseif math.abs(self.cur_turn - self.index) ~= 0  then
            self:FlushCellListView(self.turn_time)
        else
            self:FlushEndView()
            TimeHelper:CancelTimer(self.ShowTimer)
        end
    end
    self.turn_timer = self.turn_timer + self.speed_time
end

function ShengQiTreasureMapRollerView:FlushCellListView(value)
	self.ItemList[self.cur_turn]:SetActive(false)
	self.cur_turn = self.cur_turn + 1
	if self.cur_turn > self.ItemList:Length() then
		self.cur_turn = 1
    end
    self.Pointer:Rotate(0,0,-30)
    self.ItemList[self.cur_turn]:SetActive(true)
    self.turn_time = math.max(value, 0.001)
    self.turn_timer = 0
end

function ShengQiTreasureMapRollerView:FlushEndView()
    PublicPopupCtrl.Instance:ShowCustom({Item.New(self.param_t.reward)})
    local use_item_id = TreasureMapData.Instance:GetShengQiTreasureMapId()
    BagCtrl.Instance:SendHearsayInfoReq(0,self.param_t.reward.item_id,use_item_id,self.param_t.is_notice)
    self.ItemList[self.cur_turn]:SetEffect(true)
    self.BtnClose:SetActive(true)
end

--=========================拖拽逻辑=========================
function ShengQiTreasureMapRollerView:OnDragBegin()
    if self.IsOnPlayAnimation then return end
    Runner.Instance:RemoveRunUntilTrue(self.time_handle)
    self.Finger:SetActive(false)
end

function ShengQiTreasureMapRollerView:OnDrag()
    if self.IsOnPlayAnimation then return end
    Runner.Instance:RemoveRunUntilTrue(self.time_handle)
    local pos = UH.ScreenPointToViewPoint(self.ContentRect,self.mainCanvasCom)
    UH.AnchoredPosition(self.BtnEnter,pos)
end

function ShengQiTreasureMapRollerView:OnDragEnd()
    if self.IsOnPlayAnimation then return end
    Runner.Instance:RemoveRunUntilTrue(self.time_handle)
    local distance = GameMath.Distance(self.BtnEnter.localPosition,Vector3.zero,true)
    if distance <= 180 then
        self.time_handle = UH.MoveToTarget(self.EnterGame,self.target_pos,10,function()
            self.IsOnPlayAnimation = true
            self.RootAnim:Stop()
            self.RootAnim:Play()
            -- self.AnimTimer = Invoke(function()
                self:OnPlayAnimation()
            -- end,5)
        end)
    else
        self.time_handle = UH.MoveToTarget(self.EnterGame,self.start_pos,40)
    end
end
--[[ 
----------------------------StarTreasureMapRollerItem----------------------------
StarTreasureMapRollerItem = StarTreasureMapRollerItem or DeclareMono("StarTreasureMapRollerItem", UIWidgetBaseItem)

function StarTreasureMapRollerItem:SetData(data)
    self.Call:SetData(Item.New(data.item))
    UIWidgetBaseItem.SetData(self, data)
end

function StarTreasureMapRollerItem:SetActive(active)
    self.Select:SetActive(active)
end

function StarTreasureMapRollerItem:SetEffect(active)
    self.Effect:SetActive(active)
end ]]