UIWRoller = UIWRoller or DeclareMono("UIWRoller", UIWidgetBaseItem)
-- 一定要勾选OnDestroy
-- 这个转盘会帮你转到你定下的位置
-- 所以就相当于是收到转到哪的协议才来调用这个转盘来转动展示的
-- 例子在UIWRoller预设上

---ItemList---------UIWidgetBaseItem[] item要自己摆上去

---IsAutoShowItem----bool	    是否自动展示获得的道具，为空默认展示
---IsSkipAnim----bool	        是否跳过动画，为空默认不跳过
---Toggle----Toggle	            是否跳过动画的Toggle，可为空
---PrefabsString----string	    数据持久化记录是否跳过动画的字符串，用于区分不同的转盘，不为空时必须要有Trigger

---StartSpeed----string	        开始速度（取值范围0~1，超过1相当于每帧执行），可为空，默认值0
---MinSpeed----string	        最小速度（取值范围0~1，超过1相当于每帧执行），可为空，默认值0.15
---MaxSpeed----string	        最大速度（取值范围0~1，超过1相当于每帧执行），可为空，默认值0.5
---AcceleratedSpeed----string	加速度，用于刚开始加速和后续减速（取值范围0~1），可为空，默认值0.01
---ProcessTime----string	    开始减速前的过程时间，可为空，默认值3
---AverageSpeed---string        是否用平均速度（取值范围0~1，超过1相当于每帧执行），可为空，默认值0，0的话就不用平均速度

---Pointer----Transform         指针，可为空
---PointerStartAngle----string	指针初始偏转角，如：UI出的图指针朝正上方，可为空，这里就写90


function UIWRoller:UIWRoller()
    -- 当前高亮的item，0就是没有
    self.now_item_index = 0
    -- 转到目标的方法（可为空）
    self.end_func = nil
    -- 每次转动到下一个时执行的方法，多用于播放音效（可为空）
    self.rolling_func = nil
    -- 当前目标格子，0就是没有
    self.now_target = 0
    -- 用于存储指针指向每一个item的角度（存储后不用每次去计算）
    self.pointer_angle = {}
    -- 是否正在转动
    self.is_playing = false
    -- 用于转盘发放的特殊道具，比如角色属性丹礼盒这种，是服务端随机给发的
    self.roller_sp_reward = {}
end

function UIWRoller:CheckReady()
    if self.StartSpeed == nil then
        self.StartSpeed = 0
    end
    self.StartSpeed = tonumber(self.StartSpeed)

    if self.MinSpeed == nil then
        self.MinSpeed = 0.15
    end
    self.MinSpeed = tonumber(self.MinSpeed)

    if self.MaxSpeed == nil then
        self.MaxSpeed = 0.5
    end
    self.MaxSpeed = tonumber(self.MaxSpeed)

    if self.AcceleratedSpeed == nil then
        self.AcceleratedSpeed = 0.01
    end
    self.AcceleratedSpeed = tonumber(self.AcceleratedSpeed)

    if self.AverageSpeed == nil then
        self.AverageSpeed = 0
    end
    self.AverageSpeed = tonumber(self.AverageSpeed)

    if self.ProcessTime == nil then
        self.ProcessTime = 3
    end
    self.ProcessTime = tonumber(self.ProcessTime)

    if self.PointerStartAngle == nil then
        self.PointerStartAngle = 0
    end
    self.PointerStartAngle = tonumber(self.PointerStartAngle)

    if self.IsSkipAnim == nil then
        self.IsSkipAnim = false
    end

    if self.IsAutoShowItem == nil then
        self.IsAutoShowItem = true
    end

    if self.Toggle then
        self.skip_key = ""
        if self.PrefabsString ~= nil and self.PrefabsString ~= "" then
            self.skip_key = RoleId() .. "_" .. tostring(self.PrefabsString)
            self.IsSkipAnim = UnityPlayerPrefs.GetInt(self.skip_key) == 1
        end
        self.Toggle.isOn = self.IsSkipAnim
        self:SetIsSkipAnim(self.IsSkipAnim)
    end
end

-- config：列表
-- end_func：结束调用方法
-- rolling_func：每次转动到下一个时执行的方法
-- now_select：刚开始选中哪个，不传就没有选中，转的时候从1开始
function UIWRoller:SetData(data)
    self:CheckReady()
    self.end_func = data.end_func
    self.rolling_func = data.rolling_func
    self.item_count = self.ItemList:Length()
    self.data = data
    if self.item_timer ~= nil then
        Runner.Instance:RemoveRunUntilTrue(self.item_timer)
    end
    -- 如果在转动中突然设置数据，就要直接展示原来没转完的目标道具
    if self.is_playing then
        self:SetNowItemIndex(self.now_target)
        self:RotateEnd()
    end
    self:CalculatePointerAngle()
    for i = 1, self.item_count do
        if data.config then
            self.ItemList[i]:SetData(data.config[i] or {})
        else
            self.ItemList[i]:SetData({})
        end
    end
    if data.now_select then
        self.now_item_index = data.now_select
        self:SetNowItemIndex(self.now_item_index)
    end
    -- 重新设置数据后，如果当前item_index已经超过新转盘数量，就会重置回1
    if self.now_item_index > self.item_count or self.now_item_index == 0 then
        self.now_item_index = 1
        self:SetNowItemIndex(1)
    end
end

function UIWRoller:OnDestroy()
    if self.item_timer ~= nil then
        Runner.Instance:RemoveRunUntilTrue(self.item_timer)
    end
    -- 在转盘转动时被销毁了，就会直接展示道具
    if self.is_playing then
        self:ShowTargetItem()
    end
end

-- 计算并存储指针指向角度
function UIWRoller:CalculatePointerAngle()
    self.pointer_angle = {}
    if not self.Pointer then
        return
    end
    for i = 1, self.item_count do
        local toward = self.ItemList[i].gameObject.transform.localPosition - self.Pointer.localPosition
        local angle = Vector3.Angle(toward, Vector3.right)
        angle = self.ItemList[i].gameObject.transform.localPosition.y > self.Pointer.localPosition.y and angle or -angle
        self.pointer_angle[i] = angle - self.PointerStartAngle
    end
end

function UIWRoller:GetIsPlaying()
    return self.is_playing
end

function UIWRoller:SetRollerSpReward(item_list)
    self.roller_sp_reward = item_list
end

function UIWRoller:SetIsSkipAnim(flag)
    if flag == nil then
        return
    end
    UnityPlayerPrefs.SetInt(self.skip_key, flag and 1 or 0)
    if self.is_playing then
        -- 如果在转动的过程中改变了是否跳过动画的状态，就要先记录下来
        self.temporary_is_skip_anim = flag
    else
        self.IsSkipAnim = flag
        self.temporary_is_skip_anim = nil
    end
end

function UIWRoller:PlayRoller(index)
    if self.is_playing then
        return
    end
    
    self.AcceleratedSpeed = math.abs(self.AcceleratedSpeed)
    -- 当前速度
    self.now_speed = self.StartSpeed
    -- 累计的速度累加，每次加到1会清空
    self.cumulative_speed = self.StartSpeed
    self.start_time = Time.time
    -- 开始减速的标志
    self.is_start_slow = false
    self.is_playing = true
    self.now_target = index
    if self.item_timer ~= nil then
        Runner.Instance:RemoveRunUntilTrue(self.item_timer)
    end
    if self.ItemList[self.now_item_index].SetEffect then
        self.ItemList[self.now_item_index]:SetEffect(false)
    end
    if self.IsSkipAnim then
        self:SetNowItemIndex(index, true)
        self:RotateEnd()
    else
        self.item_timer = Runner.Instance:RunUntilTrue(BindTool.Bind(self.Rotating, self))
    end
end

-- 转动中
function UIWRoller:Rotating()
    if self.AverageSpeed > 0 then
        self.is_start_slow = true
        self.cumulative_speed = self.cumulative_speed + self.AverageSpeed
    else
        -- 减速处理
        if Time.time - self.start_time >= self.ProcessTime and not self.is_start_slow and self.AcceleratedSpeed > 0 then
            self.AcceleratedSpeed = -self.AcceleratedSpeed
        end
        -- 这边计算速度
        if not self.is_start_slow then
            self.now_speed = self.now_speed + self.AcceleratedSpeed
            if self.AcceleratedSpeed < 0 then
                -- 减到最小速度后，开始锁定目标吧
                if self.now_speed <= self.MinSpeed then
                    self.now_speed = self.MinSpeed
                    self.is_start_slow = true
                end
            else
                self.now_speed = self.now_speed > self.MaxSpeed and self.MaxSpeed or self.now_speed
            end
        end
        self.cumulative_speed = self.cumulative_speed + self.now_speed
    end
    
    if self.cumulative_speed >= 1 then
        self:SetNowItemIndex(self.now_item_index % self.item_count + 1, true)
        self.cumulative_speed = self.StartSpeed
        -- 转盘停止
        if self.is_start_slow == true and self.now_item_index == self.now_target then
            self:RotateEnd()
            return true
        end
    end
end
-- 设置当前转到的item
function UIWRoller:SetNowItemIndex(index, is_rolling)
    if self.now_item_index ~= 0 and self.ItemList[self.now_item_index] then
        if self.ItemList[self.now_item_index].SetActive then
            self.ItemList[self.now_item_index]:SetActive(false)
        end
    end
    self.now_item_index = index
    if self.Pointer and self.now_item_index and self.pointer_angle[self.now_item_index] then
        UH.LocalEulerAngles(self.Pointer, Vector3.New(0, 0, self.pointer_angle[self.now_item_index]))
    end
    if self.now_item_index ~= 0 and self.ItemList[self.now_item_index] then
        if self.ItemList[self.now_item_index].SetActive then
            self.ItemList[self.now_item_index]:SetActive(true)
        end
    end
    if is_rolling and type(self.rolling_func) == "function" then
        self.rolling_func()
    end
end

-- 转动结束
function UIWRoller:RotateEnd()
    self.is_playing = false
    self:SetIsSkipAnim(self.temporary_is_skip_anim)
    self:ShowTargetItem()
    if self.ItemList[self.now_target].SetEffect then
        self.ItemList[self.now_target]:SetEffect(true)
    end
    if type(self.end_func) == "function" then
        self.end_func()
    end
    self.now_target = 0
end

-- 如果是自动展示获得的道具，就会展示
function UIWRoller:ShowTargetItem()
    if self.IsAutoShowItem and self.now_target ~= 0 then
        if not TableIsEmpty(self.data.config[self.now_target].reward) then
            -- 判断展示的物品是否能快速使用
            local item_id = self.data.config[self.now_target].reward.item_id
            -- 角色属性丹礼盒特殊处理
            if Item.IsPlayerDanGift(item_id) then
                if #self.roller_sp_reward > 0 then
                    item_id = self.roller_sp_reward[1].item_id
                    PublicPopupCtrl.Instance:ShowCustom(self.roller_sp_reward)
                end
            else
                PublicPopupCtrl.Instance:ShowCustom({self.data.config[self.now_target].reward})
            end
            if Item.IsQuick(item_id) then
                local item_list = BagData.Instance:GetItemsByItemId(item_id)
                if #item_list > 0 then
                    MainOtherCtrl.Instance:OpenKeyUseView(item_list[1])
                end
            end
        end
    end
end


-- 转盘中的道具类，基本方法已有，可以自己继承重写
-- 重写的话data.reward不建议改命名，因为上面自动展示道具中有用到
----------------------------UIWRollerItem----------------------------
UIWRollerItem = UIWRollerItem or DeclareMono("UIWRollerItem", UIWidgetBaseItem)

function UIWRollerItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    if self.Call and data.reward then
        self.Call:SetData(Item.Create(data.reward))
    end
end

function UIWRollerItem:SetActive(active)
    if self.Select then
        self.Select:SetActive(active)
    end
end

function UIWRollerItem:SetEffect(active)
    if self.Effect then
        self.Effect:SetActive(active)
    end
end