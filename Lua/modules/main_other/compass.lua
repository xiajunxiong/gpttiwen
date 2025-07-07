--罗盘界面
Compass = Compass or DeclareView("Compass", "main_other/compass", Mod.SuperMap.Compass, ViewLayer.Normal - 1)
-- VIEW_DECLARE_LEVEL(Compass, ViewLevel.L0)
VIEW_DECLARE_MASK(Compass, ViewMask.None)

function Compass:Compass()
    self.data = TreasureMapData.Instance
end

function Compass:LoadCallback()
    self.data.view_data.val = true
end

function Compass:OnClickClose()
    AutoInvokerCtrl.Instance:ClearCommands()
    ViewMgr:CloseView(Compass)
end

function Compass:CloseCallback()
    self.item = self.data:MapInfo()
    if self.item ~= nil and self.item.param.client_need_uid1 ~= nil then
        TreasureMapData.Instance:DeleteEffect(self.item.param.client_need_uid1)
    end
    TreasureMapData.Instance:ClearItem()
    self.data.view_data.val = false
end

CompassPanel = CompassPanel or DeclareMono("CompassPanel", UIWFlushPanel)
function CompassPanel:CompassPanel()
    self.data = TreasureMapData.Instance

    self.data_cares = {}
    self.anim_events = {}
    self.is_play_anim = false
    self.last_index = nil
end

function CompassPanel:onFlush()
    -- 两个监听
    --监听人物移动到达一定范围 开始挖宝图
    self.item = self.data:MapInfo()
    if self.item == nil then
        ViewMgr:CloseView(Compass)
        return
    end
    self.map_uid = self.item.param.client_need_uid1
    self.map_item_id = self.item.item_id
    --先转一圈
    if not SmartData.Check(self.item) then
        ViewMgr:CloseView(Compass)
        BuglyReport("罗盘收到的检查宝图物品不是SMdata")
        return
    end
    self.map_item_care = self:CareData(self.map_item_care, self.item, self.MapItemChange)
    local target_pos = SceneCtrl.Instance:CurScene():PosToClint(self.item.param.x, self.item.param.y)
    local obj = SE:Play(SceneEffectConfig.EffectList.SuperMapEffect, target_pos)
    -- LogDG(obj)
    if self.item.param.client_need_uid1 then        
        TreasureMapData.Instance:AddEffect(self.item.param.client_need_uid1, obj)
    else
        ViewMgr:CloseView(Compass)
    end

    for i = 1, self.LightObjs:Length() do
        local last_index = i
        if self.anim_events[last_index] == nil then
            self.anim_events[last_index] =
                self.LightAnims[last_index]:ListenEvent(
                "AttrFloat",
                function(eve_param)
                    if eve_param == "wait_end" then
                        -- self.LightAnims[last_index]:SetTrigger(APH("Hide"))
                    elseif eve_param == "hide_end" then
                        self.LightObjs[last_index]:SetActive(false)
                    end
                end
            )
        end
    end

    self:StartAction()
end
function CompassPanel:FlushLight(angle)
    if angle <0 then
        angle = 360 + angle
    end
    local x, y = math.modf(angle / 45)
    x = x < 0 and (8 + x) or x
    local index = 8 - x
    index = y < 0.5 and index + 1 or index
    index = index == 0 and 1 or index
    index = index > 8 and 1 or index
    --[[ for i = 1, self.LightObjs:Length() do
        self.LightObjs[i]:SetActive(index == i)
    end ]]
    self.LightObjs[index]:SetActive(true)
    if self.last_index and self.last_index ~= index then
        self.LightAnims[self.last_index]:SetTrigger(APH("Hide"))
    end
    self.last_index = index
    --[[ local last_index = index - 1
    last_index = last_index == 0 and 1 or last_index
    last_index = last_index > 8 and 1 or last_index
    -- self.LightObjs[last_index]:SetActive(false)
    self.LightAnims[last_index]:SetTrigger(APH("Hide")) ]]
end
function CompassPanel:StartAction()
    TimeHelper:CancelTimer(self.handle_time1)
    TimeHelper:CancelTimer(self.handle_time2)
    local angle = 360
    self.handle_time1 =
        TimeHelper:AddCountDownTT(
        function()
            self:FlushLight(angle)
            local ang = Quaternion.AngleAxis(angle, Vector3.up)
            ang.z = ang.y
            ang.y = 0
            self.Arrow.localRotation = ang
            angle = angle - 360 / 24
            --15度
        end,
        function()
            angle = 360
            local final = self:FinalAngle()
            local frame = (angle - final) / 8
            --math.floor(final * 2 / 16) --（1+15） --(360 - final) / (360 / 24)
            local temp = frame
            self.handle_time2 =
                TimeHelper:AddCountDownTT(
                function()
                    if angle > final then
                        self:FlushLight(angle)
                        local ang = Quaternion.AngleAxis(angle, Vector3.up)
                        ang.z = ang.y
                        ang.y = 0
                        self.Arrow.localRotation = ang
                        angle = angle - ((temp / frame) * 15)
                        temp = temp - 1
                    end
                end,
                function()
                    self.QuanObj:SetActive(true)
                    self.role_pos_event =
                        EventSys:Bind(GlobalEventKeys.RolePosChange, BindTool.Bind(self.onMainRolePosChange, self))
                        self:onMainRolePosChange()
                end,
                (1 / 24) * frame,
                (1 / 24),
                false
            )
        end,
        1,
        (1 / 24),
        false
    )
    --[[ TimeHelper:AddRunFrameTimer(
        function()
            self:FlushLight(angle)
            local ang = Quaternion.AngleAxis(angle, Vector3.up)
            ang.z = ang.y
            ang.y = 0
            self.Arrow.localRotation = ang
            angle = angle - 360 / 24
            if angle == 0 then
                angle = 360
                --temp
                -- 转完一圈
                -- 算出需要的帧数
                local final = self:FinalAngle()
                local frame =  (360 - final) / 8--math.floor(final * 2 / 16) --（1+15） --(360 - final) / (360 / 24)
                local temp = frame
                TimeHelper:AddRunFrameTimer(
                    function()
                        if angle > final then
                            self:FlushLight(angle)
                            local ang = Quaternion.AngleAxis(angle, Vector3.up)
                            ang.z = ang.y
                            ang.y = 0
                            self.Arrow.localRotation = ang
                            angle = angle - ((temp / frame) * 15)
                            temp = temp - 1
                        else
                            self.QuanObj:SetActive(true)
                        end
                    end,
                    1,
                    frame
                )
            end
        end,
        1,
        24
    ) ]]
end
function CompassPanel:OnClickA()
    local state = self.Anim:get_Item("long1")
    state.speed = 2
end
function CompassPanel:OnClickB()
    --[[ local state = self.Anim:get_Item("long1")
    state.speed = 1.5 ]]
    SceneLogic.Instance:AutoToPos(self.item.param.scene_id, self.item.param, nil, true, true, nil)
end
function CompassPanel:OnClickC()
    local state = self.Anim:get_Item("long1")
    state.speed = 1
end
function CompassPanel:MapItemChange()
    self:onMainRolePosChange()
end
function CompassPanel:OnDestroy()
    self.anim_events = {}
    self.last_index = nil
    UIWFlushPanel.OnDestroy(self)
    EventSys:UnBind(self.role_pos_event)
    TimeHelper:CancelTimer(self.handle_time1)
    TimeHelper:CancelTimer(self.handle_time2)
end
function CompassPanel:FinalAngle()
    if MainPlayer == nil then
        return 0
    end
    if not ViewMgr:IsOpen(Compass) then
        return 0
    end
    local pos = MainPlayer:GetPos()
    local target, need_samp = SceneCtrl.Instance:CurScene():PosToClint(self.item.param.x, self.item.param.y)

    if (target.y == 0 or need_samp == true) and self.item.param.scene_id == SceneData.Instance:SceneId() then
        BagCtrl.Instance:SendActiveSpecialLogicItem(1, self.item.column_type, self.item.index)
        local column = self.item.column_type
        local index = self.item.index
        PublicPopupCtrl.Instance:DialogTips(
            {
                content = Language.StarTreasureMap.ErrorPos,
                cancel = {
                    func = function()
                        TreasureMapData.Instance:ClearItem()
                        ViewMgr:CloseView(DialogTipsView)
                    end
                },
                confirm = {
                    name = Language.StarTreasureMap.ReUse,
                    func = function()
                        local item = BagData.Instance:GetItemByIndex(column, index)
                        if item then
                            TreasureMapData.Instance:SetMapInfo(item)
                            SceneLogic.Instance:AutoToPos(
                                item.param.scene_id,
                                nil,
                                function()
                                    ViewMgr:OpenView(Compass)
                                    SceneLogic.Instance:ClearMoveCache()
                                end,
                                true,
                                true
                            )
                        end
                        ViewMgr:CloseView(DialogTipsView)
                    end
                }
            }
        )
        ViewMgr:CloseView(Compass)
        return 0
    end
    local xrad = math.atan2(target.z - pos.z, target.x - pos.x)
     --math.atan2(target.x - pos.x, target.z - pos.z)
    local angle = xrad * (180 / math.pi)
     --xrad / math.pi * 180
    angle = angle + 45
    --[[ angle = 360 - angle + 90
    angle = angle < 0 and 360 + angle or angle
    angle = angle % 360 ]]
    return angle
end
-----0-----
--90---270-
----180----
function CompassPanel:onMainRolePosChange()
    if self.item == nil then
        return
    end
    if self.item.item_id == 0 and not self.item.is_task then
        self.data:ClearItem()
        ViewMgr:CloseView(Compass)
        return
    end
    if MainPlayer == nil then
        return
    end
    if self.item.param == nil or self.item.param.x == nil or self.item.param.y == nil then
        return
    end
    local pos = MainPlayer:GetPos()
    local target = SceneCtrl.Instance:CurScene():PosToClint(self.item.param.x, self.item.param.y)
    local angle = self:FinalAngle()
    self:FlushLight(angle)
    local ang = Quaternion.AngleAxis(angle, Vector3.up)
    ang.z = ang.y
    ang.y = 0
    self.Arrow.localRotation = ang
    local dis = GameMath.GetDistance(pos.x, pos.z, target.x, target.z)
    self.LengthDesc.text = string.format(Language.Common.Distance, math.floor(math.sqrt(dis)))
    if dis < 1000 and dis > 200 then
        for i = 1, 3 do
            self.LongObjs[i]:SetActive(i == 1)
        end
    elseif dis < 200 and dis > 20 then
        for i = 1, 3 do
            self.LongObjs[i]:SetActive(i == 2)
        end
    elseif dis < 20 then
        for i = 1, 3 do
            self.LongObjs[i]:SetActive(i == 3)
        end
    end
    if dis < 6 and self.item.is_task then
        ViewMgr:CloseView(Compass)
        BehaviorCtrl.Exe(self.item.param.beh_id)
    elseif dis < 6 and not ViewMgr:IsOpen(KeyUseView) then
        local list = BagData.Instance:GetItemsByItemId(self.map_item_id)
        for index, value in ipairs(list) do
            if value.param.client_need_uid1 == self.map_uid then
                self.item = value
                break
            end
        end
        MainOtherCtrl.Instance:OpenKeyUseView(self.item)
    elseif dis < 6 and ViewMgr:IsOpen(KeyUseView) then
    else
        MainOtherCtrl.Instance:CloseKeyUse()
    end
    if self.item.param.scene_id ~= SceneData.Instance:SceneId() then
        ViewMgr:CloseView(Compass)
    end
end
