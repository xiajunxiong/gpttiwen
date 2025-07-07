SanXianGuiDong5 = SanXianGuiDong5 or DeclareMono('SanXianGuiDong5', UIWFlushPanel)
function SanXianGuiDong5:SanXianGuiDong5()
    self.data = SanXianGuiDongData.Instance
    self.data_cares = {
        {data = self.data:Game(), func = self.Prepare, keys = {'state'}}
    }
    self.right_idxs = {}
    self.rabbits = {}
    -- self.pos_list = {1, 2, 3, 4, 5}
    self.grid_num = 5
end
function SanXianGuiDong5:Awake()
    self.Root:SetActive(false)
    UIWFlushPanel.Awake(self)
    self:CheckRabbit()
end

function SanXianGuiDong5:Prepare()
    local level_info = self.data:LevelCfg()
    local rabbit_num = level_info.rabbit_num
    local state = self.data:Game().state
    local unit_num = level_info.unit_num
    if unit_num ~= self.grid_num then
        for i, v in ipairs(self.rabbits) do
            self:SetRabbitPos(i, nil, false)
        end
        return
    end
    self.data:SetPosObj(self.PosList)
    if state == -1 then
        if unit_num == self.grid_num then
            self.Root:SetActive(true)
        end
        self.right_idxs = self.data:RightIdxs(rabbit_num, unit_num)
        -- LogError('《-1》随机兔子的位置', self.right_idxs)
        self.data:SetRightIdxs(self.right_idxs)
        self.data:Game().state = -0.9
    elseif state == -0.9 then
        -- LogError('《-0.9》展示兔子过程并倒计时3秒')
        for i = 1, self.grid_num do
            self:Boom(i, true)
            self:Show(i, not self:HasRabbit(i))
        end
        for i, v in ipairs(self.right_idxs) do
            self:SetRabbitPos(i, v, true)
        end
    elseif state == -0.8 then
        -- LogError('《-0.8》隐藏兔子')
        for i = 1, self.grid_num do
            if self:HasRabbit(i) then
                self:Boom(i, true)
            end
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
        for i = 1, self.grid_num do
            self:Show(i, false)
            if self:HasRabbit(i, true) then
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
                for i = 1, self.grid_num do
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
    elseif state == 0.2 or state == 1.1 then
        -- LogError('《0.2/1.1》展示结果')
        --不管时猜对还是错都是展示
        local right_idxs = self.data:GetRightIdxs()
        for i = 1, self.grid_num do
            self:Show(i, false)
            if self:HasRabbit(i, true) then
                self:Boom(i, true)
            end
        end
        for i, v in ipairs(right_idxs) do
            self:SetRabbitPos(i, v, true)
        end
    elseif state == 0.3 then
        local selects = self.data:SelectIdxs()
        for i, v in ipairs(selects) do
            self:Boom(v, true)
            self:SetRabbitPos(i, v, true)
        end
    end
end
function SanXianGuiDong5:HasRabbit(idx, final)
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
function SanXianGuiDong5:SetRabbitPos(idx, pos_idx, show)
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
function SanXianGuiDong5:CheckRabbit()
    local need = self.data:NeedRabbit(self.grid_num)
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
end

function SanXianGuiDong5:OnEnable()
    self:CheckRabbit()
    UIWFlushPanel.OnEnable(self)
end
function SanXianGuiDong5:OnDestroy()
    UIWFlushPanel.OnDestroy(self)
    TimeHelper:CancelTimer(self.handle_hide)
    TimeHelper:CancelTimer(self.handle_next)
    self.data:SetPosObj(nil)
    self.data:SetRightIdxs({})
    for i, v in ipairs(self.rabbits) do
        v:DeleteMe()
    end
    self.rabbits = nil
end
function SanXianGuiDong5:StartChange()
    local level_info = self.data:LevelCfg()
    if level_info.unit_num ~= self.grid_num then
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
    local final = self.data:FinalArray({1, 2, 3, 4, 5}, list2)
    local rights = {}
    for i, v in ipairs(final) do
        if self:HasRabbit(v) then
            table.insert(rights, i)
        end
    end

    -- LogError('转换后正确位置', rights)
    self.data:SetRightIdxs(rights)
    self.change_list = list2

    self.EffectList[self.EffectList:Length()]:SetCompleteCallBack(BindTool.Bind(self.StartPlay, self))
    for i = 1, self.EffectList:Length() do
        self.EffectList[i]:SetData(self.change_list)
    end
end
function SanXianGuiDong5:StartPlay()
    self.play_num = 1
    if #self.change_list >= self.play_num then
        self:PlayAnim()
    else
        LogDG('无效播放')
    end
end
function SanXianGuiDong5:Show(idx, show)
    self.ShowList[idx]:SetActive(show)
end
function SanXianGuiDong5:Boom(idx, boom)
    self.BoomList[idx]:SetActive(false)
    self.BoomList[idx]:SetActive(boom)
end

function SanXianGuiDong5:PlayAnim()
    if self.play_num > #self.change_list then
        self:PlayEnd()
        return
    end
    --[[  local info = self.change_list[self.play_num]
    local a, b = tonumber(info[1]), tonumber(info[2])
    self.AnimItems[a]:PlayAnim(b, nil, self)
    self.AnimItems[b]:PlayAnim(a, BindTool.Bind(self.AnimaEnd, self, a, b), self) ]]
    local info = self.change_list[self.play_num]
    if #info > 2 then
        local a, b = tonumber(info[1]), tonumber(info[2])
        local c, d = tonumber(info[3]), tonumber(info[4])
        self:Show(a, false)
        self:Show(b, false)
        self:Show(c, false)
        self:Show(d, false)
        self.EffectList[a]:GetItem(self.play_num):PlayAnim(a, b, nil, self.play_num == #self.change_list)
        self.EffectList[b]:GetItem(self.play_num):PlayAnim(b, a, nil, self.play_num == #self.change_list)
        self.EffectList[c]:GetItem(self.play_num):PlayAnim(c, d, nil, self.play_num == #self.change_list)
        self.EffectList[d]:GetItem(self.play_num):PlayAnim(
            d,
            c,
            BindTool.Bind(self.AnimaEnd, self, a, b, c, d),
            self.play_num == #self.change_list
        )
    else
        -- self.AnimItems[a]:PlayAnim(b, nil, self)
        -- self.AnimItems[b]:PlayAnim(a, BindTool.Bind(self.AnimaEnd, self, a, b), self)
        local a, b = tonumber(info[1]), tonumber(info[2])
        self:Show(a, false)
        self:Show(b, false)
        self.EffectList[a]:GetItem(self.play_num):PlayAnim(a, b, nil, self.play_num == #self.change_list)
        self.EffectList[b]:GetItem(self.play_num):PlayAnim(
            b,
            a,
            BindTool.Bind(self.AnimaEnd, self, a, b),
            self.play_num == #self.change_list
        )
    end
end
function SanXianGuiDong5:AnimaEnd(a, b)
    for i = 1, self.ShowList:Length() do
        self:Show(i, true)
    end
    self.play_num = self.play_num + 1
    self:PlayAnim()
end
function SanXianGuiDong5:PlayEnd()
    for i = 1, self.ShowList:Length() do
        self:Show(i, true)
    end
    SanXianGuiDongCtrl.StartGame()
    -- LogDG('请求开始10秒倒计时')
end
function SanXianGuiDong5:SelectEnd()
    -- 打开兔子关闭特效
end

Rabbits5Item = Rabbits5Item or DeclareMono('Rabbits5Item', UIWidgetBaseItem)
function Rabbits5Item:Rabbits5Item()
end
function Rabbits5Item:SetData(data)
    self.Root:SetActive(false)
    self.Root:SetLocalScale(Vector3.one)
    self.Son:SetLocalScale(Vector3.one)
    self.Son:SetLocalPosition(Vector3.zero)
    if self.from then
        self.Root:SetLocalRotation(Quaternion.Euler(0, (self.from - 1) * 72, 0))
    end
    -- self.func_count = 1
end
function Rabbits5Item:PlayAnim(from, to, end_func, is_end)
    self.from = from
    self.to = to
    self.end_func = end_func
    self.is_end = is_end

    self.Root:SetActive(true)
    self.Root:SetLocalRotation(Quaternion.Euler(0, (self.from - 1) * 72, 0))
    local diff = self.from - to
    if diff == 1 or diff == -4 then -- (self.from - 1) * 72 + 1.5 * 72
        self.Root:SetLocalRotation(Quaternion.Euler(0, (self.from + 0.5) * 72, 0))
    end
    local mult = diff < 0 and (diff + 5) or diff
    if mult == 2 or mult == 3 then
        local add = mult == 3 and 36 or 72
        local length = 2 * math.cos(math.pi / 5)
        self.Root:SetLocalRotation(Quaternion.Euler(0, (self.from - 1) * 72 + add, 0))
        self.Root:SetLocalScale(Vector3.New(length, 1, length))
        self.Son:SetLocalScale(Vector3.New(1 / length, 1, 1 / length))
    end
    self.Anima:Play()
end
function Rabbits5Item:OnAnimationEvent()
    -- if self.func_count % 2 ~= 0 then
    if self.end_func then
        self.end_func()
    end
    -- end
    -- if self.func_count % 2 == 0 then
    -- end
    -- self.func_count = self.func_count + 1
end

-- Rabbits5Item = Rabbits5Item or DeclareMono('Rabbits5Item', UIWidgetBaseItem)
-- function Rabbits5Item:Rabbits5Item()
--     --原角度 =   (self.Index - 1) * 72
--     -- 反方向 =  (self.Index - 1) * 72 + 1.5 * 72 | 原角度 + 108
--     -- 对角 =  原角度 + 36/72  长度 2* cos36
-- end
-- function Rabbits5Item:OnEnable()
--     self.Root:SetLocalRotation(Quaternion.Euler(0, (self.Index - 1) * 72, 0))
-- end
-- function Rabbits5Item:PlayAnim(to, end_func)
--     self.end_func = end_func
--     local diff = self.Index - to
--     if diff == 1 or diff == -4 then -- (self.Index - 1) * 72 + 1.5 * 72
--         self.Root:SetLocalRotation(Quaternion.Euler(0, (self.Index + 0.5) * 72, 0))
--     end
--     local mult = diff < 0 and (diff + 5) or diff
--     if mult == 2 or mult == 3 then
--         local add = mult == 3 and 36 or 72
--         local length = 2 * math.cos(math.pi / 5)
--         self.Root:SetLocalRotation(Quaternion.Euler(0, (self.Index - 1) * 72 + add, 0))
--         self.Root:SetLocalScale(Vector3.New(length, 1, length))
--         self.Son:SetLocalScale(Vector3.New(1 / length, 1, 1 / length))
--     end
--     self.Anima:Play()
--     self:ClearTimer()
--     self.handle_listen = AnimaHelper.ListenStop(self.Anima, BindTool.Bind(self.AnimaEnd, self))
-- end
-- function Rabbits5Item:AnimaEnd()
--     -- self.Root:SetActive(false)
--     self:ClearTimer()
--     self:OnRelese()
--     if self.end_func then
--         self.end_func()
--     end
-- end
-- function Rabbits5Item:ClearTimer()
--     Runner.Instance:RemoveRunUntilTrue(self.handle_listen)
-- end
-- function Rabbits5Item:OnRelese()
--     self.Root:SetLocalRotation(Quaternion.Euler(0, (self.Index - 1) * 72, 0))
--     self.Root:SetLocalScale(Vector3.one)
--     self.Son:SetLocalScale(Vector3.one)
--     self.Son:SetLocalPosition(Vector3.zero)
-- end

-- function Rabbits5Item:OnDestroy()
--     self:ClearTimer()
-- end

-- function Rabbits5Item:Init()
--     self.Root:SetLocalScale(Vector3.one)
--     -- self.Effect:SetLocalScale(Vector3.zero)
-- end
-- function Rabbits5Item:ShowRoot(show)
--     self.Root:SetActive(show)
-- end
-- function Rabbits5Item:PlayBoom()
--     self.BoomEffect:SetActive(false)
--     self.BoomEffect:SetActive(true)
-- end
