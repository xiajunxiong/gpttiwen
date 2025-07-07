SanXianGuiDong3 = SanXianGuiDong3 or DeclareMono('SanXianGuiDong3', UIWFlushPanel)
function SanXianGuiDong3:SanXianGuiDong3()
    self.data = SanXianGuiDongData.Instance
    self.data_cares = {
        -- {data = self.data.change, func = self.StartChange, keys = {'start'}, init_call = false},
        {data = self.data:Game(), func = self.Prepare, keys = {'state'}}
    }
    self.right_idxs = {}
    self.rabbits = {}
end
function SanXianGuiDong3:Awake()
    self.Root:SetActive(false)
    UIWFlushPanel.Awake(self)
    self:CheckRabbit()
end
function SanXianGuiDong3:HasRabbit(idx, final)
    local idxs = self.right_idxs
    if final then
        idxs = self.data:GetRightIdxs()
    end
    for i, v in ipairs(idxs) do
        if v == idx then
            return true
        end
    end
    return false
end
function SanXianGuiDong3:Prepare()
    local level_info = self.data:LevelCfg()
    local rabbit_num = level_info.rabbit_num
    local state = self.data:Game().state
    local unit_num = level_info.unit_num
    if unit_num ~= 3 then
        for i, v in ipairs(self.rabbits) do
            self:SetRabbitPos(i, nil, false)
        end
        return
    end
    self.data:SetPosObj(self.PosList)
    if state == -1 then
        -- LogError('《-1》随机兔子的位置')
        if unit_num == 3 then
            self.Root:SetActive(true)
        end
        self.right_idxs = self.data:RightIdxs(rabbit_num, unit_num)
        self.data:SetRightIdxs(self.right_idxs)
        self.data:Game().state = -0.9
    elseif state == -0.9 then
        -- LogError('《-0.9》展示兔子过程并倒计时3秒')
        for i = 1, 3 do
            self:Boom(i, true)
            -- self.AnimItems[i]:PlayBoom()
            self:Show(i, not self:HasRabbit(i))
            --[[ if self:HasRabbit(i) then
                self.AnimItems[i]:ShowRoot(false)
            else
                self.AnimItems[i]:ShowRoot(true)
            end ]]
        end
        for i, v in ipairs(self.right_idxs) do
            self:SetRabbitPos(i, v, true)
        end
    elseif state == -0.8 then
        -- LogError('《-0.8》隐藏兔子')
        for i = 1, 3 do
            if self:HasRabbit(i) then
                -- self.AnimItems[i]:PlayBoom()
                self:Boom(i, true)
            end
            -- self.AnimItems[i]:ShowRoot(true)
            self:Show(i, true)
        end
        for i, v in ipairs(self.right_idxs) do
            self:SetRabbitPos(i, nil, false)
        end
        TimeHelper:CancelTimer(self.handle_hide)
        self.handle_hide =
            TimeHelper:AddDelayTimer(
            function()
                self.data:Game().state = -0.5
                self:StartChange()
            end,
            1
        )
    elseif state == 0.1 then
        -- LogError('《0.1》选择正确展示结果')
        --不管时猜对还是错都是展示
        local right_idxs = self.data:GetRightIdxs()
        for i = 1, 3 do
            -- self.AnimItems[i]:ShowRoot(false)
            self:Show(i, false)
            if self:HasRabbit(i, true) then
                -- self.AnimItems[i]:PlayBoom()
                self:Boom(i, true)
            end
        end
        for i, v in ipairs(right_idxs) do
            self:SetRabbitPos(i, v, true)
        end
        TimeHelper:CancelTimer(self.handle_next)
        self.handle_next =
            TimeHelper:AddDelayTimer(
            function()
                for i = 1, 3 do
                    --self.AnimItems[i]:ShowRoot(false)
                    self:Show(i, false)
                end
                for i, v in ipairs(self.rabbits) do
                    self:SetRabbitPos(i, nil, false)
                end
                --这个暂时合在一起
                if self.data.cache_pass_idx > self.data:Game().pass_idx then
                    self.data:Game().pass_idx = self.data.cache_pass_idx
                end
                self.data:Game().state = -1
            end,
            2
        )
    elseif state == 1.1 then
        -- LogError('《1.1》选择错误展示结果')
        --不管时猜对还是错都是展示
        local right_idxs = self.data:GetRightIdxs()
        for i = 1, 3 do
            self:Show(i, false)
            -- self.AnimItems[i]:ShowRoot(false)
            if self:HasRabbit(i, true) then
                -- self.AnimItems[i]:PlayBoom()
                self:Boom(i, true)
            end
        end
        for i, v in ipairs(right_idxs) do
            self:SetRabbitPos(i, v, true)
        end
    end
end

function SanXianGuiDong3:SetRabbitPos(idx, pos_idx, show)
    local rabbit = self.rabbits[idx]
    if rabbit then
        if pos_idx then
            rabbit:SetPos(self.PosList[pos_idx]:GetPosition())
        end
        rabbit:SetDirByEulerY(-90)
        rabbit:SetVisible(show)
        rabbit:PlayAnim(ChAnimType.Show)
    end
end
function SanXianGuiDong3:CheckRabbit()
    local need = self.data:NeedRabbit(3)
    if #self.rabbits >= need then
        return
    end
    local level_info = self.data:LevelCfg()
    local rabbit_num = level_info.rabbit_num
    local rabbit
    local vo = CommonStruct.NpcVo({id = 73713, obj_type = SceneObjType.Npc, not_affect_yinyang = true})
    need = need - #self.rabbits
    for i = 1, need do
        rabbit = SceneObj.CreateObj(vo)
        table.insert(self.rabbits, rabbit)
    end
    -- local pos_idx = self.right_idxs[rabbit_num]
    -- self:SetRabbitPos(rabbit_num, pos_idx, true)
end

function SanXianGuiDong3:OnEnable()
    self:CheckRabbit()
    UIWFlushPanel.OnEnable(self)
end
function SanXianGuiDong3:OnDestroy()
    UIWFlushPanel.OnDestroy(self)
    TimeHelper:CancelTimer(self.handle_hide)
    TimeHelper:CancelTimer(self.handle_next)
    self.data:SetPosObj(nil)
    self.data:SetRightIdxs({})
    for i, v in ipairs(self.rabbits) do
        v:DeleteMe()
    end
    self.rabbits = nil
    self.data:Clear()
end
function SanXianGuiDong3:StartChange()
    local level_info = self.data:LevelCfg()
    if level_info.unit_num ~= 3 then
        return
    end
    local rabbit_num = level_info.rabbit_num
    local list = self.data:ChangeList()
    local list2 = {}
    for i, v in ipairs(list) do
        temp = tostring(v)
        temp = DataHelper.GetStringWordToTable(temp)
        table.insert(list2, temp)
    end
    local final = self.data:FinalArray({1, 2, 3}, list2)
    local rights = {}
    for i, v in ipairs(final) do
        if v == self.right_idxs[rabbit_num] then
            table.insert(rights, i)
        end
    end
    -- LogError('兔子交换后正确位置', rights)
    self.data:SetRightIdxs(rights)
    self.change_list = list2
    

    self.EffectList[self.EffectList:Length()]:SetCompleteCallBack(BindTool.Bind(self.StartPlay, self))
    for i = 1, self.EffectList:Length() do
        self.EffectList[i]:SetData(self.change_list)
    end
end

function SanXianGuiDong3:StartPlay()
    self.play_num = 1
    if #self.change_list >= self.play_num then
        self:PlayAnim()
    else
        LogDG('无效播放')
    end
end
function SanXianGuiDong3:Show(idx, show)
    self.ShowList[idx]:SetActive(show)
end
function SanXianGuiDong3:Boom(idx, boom)
    self.BoomList[idx]:SetActive(false)
    self.BoomList[idx]:SetActive(boom)
end
function SanXianGuiDong3:PlayAnim()
    if self.play_num > #self.change_list then
        self:PlayEnd()
        return
    end
    local info = self.change_list[self.play_num]
    local a, b = tonumber(info[1]), tonumber(info[2])
    self:Show(a, false)
    self:Show(b, false)
    self.EffectList[a]:GetItem(self.play_num):PlayAnim(a, b, nil, self.play_num == #self.change_list)
    self.EffectList[b]:GetItem(self.play_num):PlayAnim(b, a, BindTool.Bind(self.AnimaEnd, self, a, b), self.play_num == #self.change_list)
    -- self.AnimItems[a]:PlayAnim(b, nil, self, self.play_num == #self.change_list)
    -- self.AnimItems[b]:PlayAnim(a, BindTool.Bind(self.AnimaEnd, self, a, b), self, self.play_num == #self.change_list)
end
function SanXianGuiDong3:AnimaEnd(a, b)
    self:Show(a, true)
    self:Show(b, true)
    --[[ local effect = self.AnimItems[a].FlyEffect
    self.AnimItems[a].FlyEffect = self.AnimItems[b].FlyEffect
    self.AnimItems[b].FlyEffect = effect ]]
    self.play_num = self.play_num + 1
    self:PlayAnim()
end
function SanXianGuiDong3:PlayEnd()
    for i = 1, self.ShowList:Length() do
        self:Show(i, true)
    end
    --[[ for i = 1, 3 do
        self.AnimItems[i]:Hide()
    end ]]
    SanXianGuiDongCtrl.StartGame()
    -- LogDG('请求开始10秒倒计时')
end
function SanXianGuiDong3:SelectEnd()
    -- 打开兔子关闭特效
end

Rabbits3Item = Rabbits3Item or DeclareMono('Rabbits3Item', UIWidgetBaseItem)
function Rabbits3Item:Rabbits3Item()
end
function Rabbits3Item:SetData(data)
    self.Root:SetActive(false)
    self.Root:SetLocalScale(Vector3.one)
    self.Son:SetLocalScale(Vector3.one)
    self.Son:SetLocalPosition(Vector3.zero)
    -- self.func_count = 1
end
function Rabbits3Item:PlayAnim(from, to, end_func, is_end)

    self.from = from
    self.to = to
    self.end_func = end_func
    self.is_end = is_end

    self.Root:SetActive(true)
    self.Root:SetLocalRotation(Quaternion.Euler(0, 0, 0))
    local diff = self.from - self.to
    if diff > 0 then
        self.Root:SetLocalRotation(Quaternion.Euler(0, 180, 0))
    end
    local mult = math.abs(diff)
    if mult > 1 then
        self.Root:SetLocalScale(Vector3.New(mult, 1, 1))
        self.Son:SetLocalScale(Vector3.New(1 / mult, 1, 1))
    end
    self.Anima:Play()
end
function Rabbits3Item:OnAnimationEvent()
    -- if self.func_count % 2 ~= 0 then
        if self.end_func then
            self.end_func()
        end
    -- end
    -- if self.func_count % 2 == 0 then
    -- end
    -- self.func_count = self.func_count + 1
end

-- Rabbits3Item = Rabbits3Item or DeclareMono('Rabbits3Item', UIWidgetBaseItem)
-- function Rabbits3Item:Rabbits3Item()
-- end
-- function Rabbits3Item:OnEnable()
--     self.func_count = 1
-- end
-- function Rabbits3Item:PlayAnim(to, end_func, parent, is_end)
--     -- self.FlyEffect:SetActive(true)
--     -- self.SubEffect:SetActive(false)
--     self.parent = parent
--     self.end_func = end_func
--     self.to = to
--     self.is_end = is_end
--     LogDG(self.Index, "|", self.to, "|", self.is_end, "|", self.Anima.isPlaying)
--     if self.Anima.isPlaying then
--         -- Runner.Instance:RemoveRunUntilTrue(self.handle_listen2)
--         -- self.handle_listen2 = AnimaHelper.ListenStop(self.AnimaNext, BindTool.Bind(self.AnimaEnd, self, true))
--         self.SubEffectNext:SetActive(false)
--         self.SubEffect:SetActive(false)
--         self:WillPlay2()
--         self.AnimaNext:Play()
--     else
--         -- Runner.Instance:RemoveRunUntilTrue(self.handle_listen1)
--         -- self.handle_listen1 = AnimaHelper.ListenStop(self.Anima, BindTool.Bind(self.AnimaEnd, self, false))
--         self.SubEffect:SetActive(false)
--         self.SubEffectNext:SetActive(false)
--         self:WillPlay()
--         self.Anima:Play()
--     end
--     -- self:ClearTimer()
-- end
-- function Rabbits3Item:WillPlay()
--     self.Root:SetActive(true)
--     self.Root:SetLocalScale(Vector3.one)
--     -- self.Effect:SetActive(false)
--     -- self.Effect:SetLocalScale(Vector3.zero)
--     local diff = self.Index - self.to
--     if diff > 0 then
--         self.Root:SetLocalRotation(Quaternion.Euler(0, 180, 0))
--     end
--     local mult = math.abs(diff)
--     if mult > 1 then
--         self.Root:SetLocalScale(Vector3.New(mult, 1, 1))
--         self.Son:SetLocalScale(Vector3.New(1 / mult, 1, 1))
--     --当需要进行变化时使用速度较快的动画
--     end
--     --[[ if self.FlyEffect.activeSelf then
--         self.FlyEffect:SetLocalPosition(Vector3.zero)
--     else
--         self.SubEffect:SetLocalPosition(Vector3.zero)
--     end ]]
-- end
-- function Rabbits3Item:WillPlay2()
--     self.RootNext:SetActive(true)
--     -- self.RootNext:SetLocalScale(Vector3.one)
--     -- self.Effect:SetActive(false)
--     -- self.Effect:SetLocalScale(Vector3.zero)
--     local diff = self.Index - self.to
--     if diff > 0 then
--         self.RootNext:SetLocalRotation(Quaternion.Euler(0, 180, 0))
--     end
--     local mult = math.abs(diff)
--     if mult > 1 then
--         self.RootNext:SetLocalScale(Vector3.New(mult, 1, 1))
--         self.SonNext:SetLocalScale(Vector3.New(1 / mult, 1, 1))
--     --当需要进行变化时使用速度较快的动画
--     end
--     --[[ if self.FlyEffect.activeSelf then
--         self.FlyEffect:SetLocalPosition(Vector3.zero)
--     else
--         self.SubEffect:SetLocalPosition(Vector3.zero)
--     end ]]
-- end
-- function Rabbits3Item:AnimaEnd(is_next)
--     --[[ TimeHelper:AddDelayTimer(function ()
--         if is_next then
--             if not self.is_end then
--                 self:OnRelese2()
--             end
--         else
--             if not self.is_end then
--                 self:OnRelese()
--             end
--         end
--     end,0.1) ]]
--     if is_next then
--         if not self.is_end then
--             self:OnRelese2()
--         end
--     else
--         if not self.is_end then
--             self:OnRelese()
--         end
--     end

--     --[[ if self.FlyEffect.activeSelf then
--         self.SubEffect:SetActive(true)
--         self.FlyEffect:SetActive(false)
--         self.FlyEffect:SetParent(self.Root, false)
--         self.SubEffect:SetParent(self.Son, false)
--     else
--         self.FlyEffect:SetActive(true)
--         self.SubEffect:SetActive(false)
--         self.SubEffect:SetParent(self.Root, false)
--         self.FlyEffect:SetParent(self.Son, false)
--     end ]]
--     -- self:ClearTimer()
--     -- self:OnRelese()--位置复原
--     --[[ if self.end_func then
--         self.end_func()
--     end ]]
-- end
-- function Rabbits3Item:ClearTimer()
--     TimeHelper:CancelTimer(self.handle_delay1)
--     TimeHelper:CancelTimer(self.handle_delay2)
--     -- Runner.Instance:RemoveRunUntilTrue(self.handle_listen)
-- end
-- function Rabbits3Item:OnRelese()
--     self.Root:SetActive(false)
--     TimeHelper:AddDelayFrameTimer(function ()
--         self.Son:SetLocalPosition(Vector3.zero)
--     end,3)
--     self.Root:SetLocalRotation(Quaternion.Euler(0, 0, 0))
--     self.Son:SetLocalScale(Vector3.one)
--     -- self.Root:SetLocalPosition(Vector3.zero)
-- end
-- function Rabbits3Item:OnRelese2()
--     self.RootNext:SetActive(false)
--     TimeHelper:AddDelayFrameTimer(function ()
--         self.SonNext:SetLocalPosition(Vector3.zero)
--     end, 3)
--     self.RootNext:SetLocalRotation(Quaternion.Euler(0, 0, 0))
--     self.SonNext:SetLocalScale(Vector3.one)
--     -- self.Root:SetLocalPosition(Vector3.zero)
-- end

-- function Rabbits3Item:OnDestroy()
--     self:ClearTimer()
-- end
-- function Rabbits3Item:Init()
--     self.Root:SetLocalScale(Vector3.one)
--     -- self.Effect:SetLocalScale(Vector3.zero)
-- end
-- function Rabbits3Item:ShowRoot(show)
--     self.Root:SetActive(show)
--     if show == false then
--         self.RootNext:SetActive(show)
--     end
-- end
-- function Rabbits3Item:PlayBoom()
--     self.BoomEffect:SetActive(false)
--     self.BoomEffect:SetActive(true)
-- end
-- function Rabbits3Item:OnAnimationEvent(is_next)
--     LogDG('OnAnimationEvent', self.Index, Time.time)
--     -- self.SubEffectNext:SetActive(true)
--     if self.func_count % 2 ~= 0 then
--         self.SubEffect:SetActive(true)
--         if not self.is_end then
--             if self.end_func then
--                 self.end_func()
--             end
--         end
--     end
--     if self.func_count % 2 == 0 then
--         if is_next then
--             if not self.is_end then
--                 self:OnRelese2()
--             else
--                 if self.end_func then
--                     self.end_func()
--                 end
--             end
--         else
--             if not self.is_end then
--                 self:OnRelese()
--             else
--                 if self.end_func then
--                     self.end_func()
--                 end
--             end
--         end
--     end

--     self.func_count = self.func_count + 1
-- end
-- function Rabbits3Item:Hide()
--     self.SubEffect:SetActive(false)
-- end
