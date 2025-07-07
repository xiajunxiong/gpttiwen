TreasureMapView = TreasureMapView or DeclareView("TreasureMapView", "main_other/treasure_map_view")
function TreasureMapView:TreasureMapView()
end
function TreasureMapView:LoadCallback(data)
end
TreasureMapPanel = TreasureMapPanel or DeclareMono("TreasureMapPanel", UIWFlushPanel)
function TreasureMapPanel:TreasureMapPanel()
    self.data = TreasureMapData.Instance
    self.data_cares = {
        {data = self.data:ViewData(), func = self.FlushPanel, keys = {"map"}--[[ , init_call = true ]]}
    }
    --间隔为1/12 数值越大间隔时间越大 速度越慢
    self.speed = {1,2,3}
end
function TreasureMapPanel:FlushPanel()
    -- self.gameObject:SetActive(false)
    --进行12个物品的
    local data = self.data:ViewData().map
    self.item_list = self.data:ViewData().item_list
    if self.handle_runner then
        Runner.Instance:RemoveRunUntilTrue(self.handle_runner)
    end
    self.handle_runner = Runner.Instance:RunUntilTrue(
        function()
            if table.nums(self.item_list) == 12 then
                self:StartAction(data)
                return true
            end
            return false
        end
    )
end
function TreasureMapPanel:StartAction(data)
    self.need_show_effect = false
    -- self.gameObject:SetActive(true)
    --self.Panel:SetActive(true)
    local circle = GameMath.Ramdon(2, 4)
    local final = GameMath.Ramdon(1, 12)
    -- LogError(final)
    --随机一个正确的位置
    -- 随机
    local info
    for i = 1, self.ItemList:Length() do
        info = self.item_list[i]
        if info and info.type == 1 then
            info.item = {item_id = HuoBi[CurrencyType.CoinBind], num = info.coin}
        end
        local item = Item.Create(info.item)
        self.ItemList[i]:SetData(item)
        UH.SetText(self.NameList[i], item:QuaName())
    end
    local res
    local role_level = RoleData.Instance:GetRoleLevel()
    for i,v in ipairs(self.data.content_list[data.seq]) do
        if role_level >= v.min_level and role_level <= v.max_level and v.index == data.index then
            res = v
            break
        end
    end
    if res == nil then
        ViewMgr:CloseView(TreasureMapView)
        LogError("找不到配置 ：" .. data.seq)
        return
    end
    if res.type == 1 then --1铜币 2 物品 0怪物 策划说普通宝图不会出现怪物
        res.item = {item_id = HuoBi[CurrencyType.CoinBind], num = res.coin}
    end
    if res.is_rare == 1 then
        self.need_show_effect = true
    end
    local item = Item.Create(res.item)
    self.ItemList[final]:SetData(item)
    UH.SetText(self.NameList[final], item:QuaName())
    self.result_item = item
    self.result_info = res

    local interval = 1 / 12
    local final_index = final-- - 1 --结果-1
    -- print("结果位置"..final_index)
    self.last_index = nil
    local index = 1
    self.handle_time =
        TimeHelper:AddCountDownTTA(
        function()
            -- 1-12转一圈
            if self.last_index then
                self.MaskList[self.last_index]:SetActive(false)
            end
            self.last_index = index
            self.MaskList[index]:SetActive(true)
            index = index + 1
            index = index >= 13 and 1 or index
        end,
        function()
            index = 1
            self.handle_time2 =
                TimeHelper:AddCountDownTTA(
                function()
                    if self.last_index then
                        self.MaskList[self.last_index]:SetActive(false)
                    end
                    self.last_index = index
                    self.MaskList[index]:SetActive(true)
                    index = index + 1
                    index = index >= 13 and 1 or index
                end,
                function()
                    index = 1
                    self.handle_time3 =
                        TimeHelper:AddCountDownTTA(
                        function()
                            if self.last_index then
                                self.MaskList[self.last_index]:SetActive(false)
                            end
                            self.last_index = index
                            self.MaskList[index]:SetActive(true)
                            index = index + 1
                            index = index >= 13 and 1 or index
                        end,
                        function()
                            -- print("最后转到的index"..index.."|"..self.last_index)
                            if self.last_index ~= final_index then
                                self.MaskList[self.last_index]:SetActive(false)
                                self.MaskList[final_index]:SetActive(true)
                            end
                            PublicPopupCtrl.Instance.stop = false
                            ChatCtrl.Instance.stop = false
                            PublicPopupCtrl.Instance:CenterI(item)
                            ChatCtrl.Instance:ChannelChatCustom {item_get = {item = item, num = info.num}}
                            self.handle_time4 = TimeHelper:AddDelayTimer(function ()
                                self:OnClickClose()
                            end,2)
                        end,
                        interval * final_index * self.speed[3],
                        interval * self.speed[3],
                        false
                    )
                end,
                12 * interval * self.speed[2],
                interval * self.speed[2],
                false
            )
        end,
        circle * 12 * interval * self.speed[1],
        interval * self.speed[1],
        false
    )
end

function TreasureMapPanel:OnClickClose()
    TimeHelper:CancelTimer(self.handle_time)
    TimeHelper:CancelTimer(self.handle_time2)
    TimeHelper:CancelTimer(self.handle_time3)
    TimeHelper:CancelTimer(self.handle_time4)
    if self.handle_runner then
        Runner.Instance:RemoveRunUntilTrue(self.handle_runner)
    end
    ViewMgr:CloseView(TreasureMapView)
    if PublicPopupCtrl.Instance.stop == true then
        PublicPopupCtrl.Instance.stop = false
        ChatCtrl.Instance.stop = false
        if self.result_item then
            PublicPopupCtrl.Instance:CenterI(self.result_item)
            ChatCtrl.Instance:ChannelChatCustom {item_get = {item = self.result_item, num = self.result_info.item.num}}
            -- BagCtrl.Instance:SendHearsayInfoReq(0,self.result_info.item.item_id,self.data:ViewData().map.item_id,self.result_info.is_notice)
        end
    end
    if MainOtherCtrl.Instance.stop == true then
        MainOtherCtrl.Instance.stop = false
    end
    self.data:ClearResult()
    if self.need_show_effect == false then
        --找下一张宝图继续
        -- TreasureMapData.Instance:ClearItem()
        TreasureMapData.Instance:UseTreasure()
    else
        if self.result_item then
            ViewMgr:OpenView(TreasureMapShow, {item = self.result_item})
        end
    end
end

function TreasureMapPanel:OnDestroy()
    UIWFlushPanel.OnDestroy(self)
    self.data:ClearResult()
    TimeHelper:CancelTimer(self.handle_time)
    TimeHelper:CancelTimer(self.handle_time2)
    TimeHelper:CancelTimer(self.handle_time3)
    TimeHelper:CancelTimer(self.handle_time4)
    if self.handle_runner then
        Runner.Instance:RemoveRunUntilTrue(self.handle_runner)
    end
end