LookingForRabbits4 = LookingForRabbits4 or DeclareMono('LookingForRabbits4', UIWFlushPanel)
function LookingForRabbits4:LookingForRabbits4()
    self.data = LookingForRabbitsData.Instance
    self.data_cares = {
        -- {data = self.data.change, func = self.StartChange, keys = {'start'}, init_call = false},
        {data = self.data:Game(), func = self.Prepare, keys = {'state'}}
    }
    self.right_idxs = {}
    self.rabbits = {}
end
function LookingForRabbits4:Awake()
    self.Root:SetActive(false)
    UIWFlushPanel.Awake(self)
    self:CheckRabbit()
end

function LookingForRabbits4:Prepare()
    local level_info = self.data:LevelCfg()
    local rabbit_num = level_info.rabbit_num
    local state = self.data:Game().state
    local unit_num = level_info.unit_num
    if unit_num ~= 4 then
        return
    end
    self.data:SetPosObj(self.PosList)
    if state == -1 then
        if unit_num == 4 then
            self.Root:SetActive(true)
        end
        self.right_idxs = self.data:RightIdxs(rabbit_num, unit_num)
        -- LogError("《-1》随机兔子的位置", self.right_idxs)
        self.data:SetRightIdxs(self.right_idxs)
        self.data:Game().state = -0.9
    elseif state == -0.9 then
        -- LogError("《-0.9》展示兔子过程并倒计时3秒")
        for i = 1, 4 do
            -- self.AnimItems[i]:PlayBoom()
            self:Boom(i, true)
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
    elseif  state == -0.8 then
        -- LogError("《-0.8》隐藏兔子")
        for i = 1, 4 do
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
            -- LogError("《0.1》选择正确展示结果")
            --不管时猜对还是错都是展示
            local right_idxs = self.data:GetRightIdxs()
            for i = 1, 4 do
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
                    for i = 1, 4 do
                        -- self.AnimItems[i]:ShowRoot(false)
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
    elseif state == 0.3 then
        local selects = self.data:SelectIdxs()
        for i, v in ipairs(selects) do
            -- self.AnimItems[v]:PlayBoom()
            self:Boom(v, true)
            self:SetRabbitPos(i, v, true)
        end
    elseif state == 1.1 then
        -- LogError("《1.1》选择错误展示结果")
            --不管时猜对还是错都是展示
            local right_idxs = self.data:GetRightIdxs()
            for i = 1, 4 do
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
    end
end
function LookingForRabbits4:HasRabbit(idx, final)
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
function LookingForRabbits4:SetRabbitPos(idx, pos_idx, show)
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
function LookingForRabbits4:CheckRabbit()
    local need = self.data:NeedRabbit(4)
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
    --[[ local pos_idx
    for i = 1, rabbit_num do
        pos_idx = self.right_idxs[i]
        self:SetRabbitPos(rabbit_num, pos_idx, true)
    end ]]
end

function LookingForRabbits4:OnEnable()
    self:CheckRabbit()
    UIWFlushPanel.OnEnable(self)
end
function LookingForRabbits4:OnDestroy()
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

function LookingForRabbits4:StartChange()
    local level_info = self.data:LevelCfg()
    if level_info.unit_num ~= 4 then
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
    local final = self.data:FinalArray({1, 2, 3, 4}, list2)
    local rights = {}
    for i, v in ipairs(final) do
        if self:HasRabbit(v) then
            table.insert(rights, i)
        end
    end
    -- LogError("转换后正确位置", rights)
    self.data:SetRightIdxs(rights)
    self.change_list = list2


    self.EffectList[self.EffectList:Length()]:SetCompleteCallBack(BindTool.Bind(self.StartPlay, self))
    for i = 1, self.EffectList:Length() do
        self.EffectList[i]:SetData(self.change_list)
    end
end
function LookingForRabbits4:StartPlay()
    self.play_num = 1
    if #self.change_list >= self.play_num then
        self:PlayAnim()
    else
        LogDG('无效播放')
    end
end
function LookingForRabbits4:Show(idx, show)
    self.ShowList[idx]:SetActive(show)
end
function LookingForRabbits4:Boom(idx, boom)
    self.BoomList[idx]:SetActive(false)
    self.BoomList[idx]:SetActive(boom)
end
function LookingForRabbits4:PlayAnim()
    if self.play_num > #self.change_list then
        self:PlayEnd()
        return
    end
    --[[ local info = self.change_list[self.play_num]
    local a, b = tonumber(info[1]), tonumber(info[2])
    self.AnimItems[a]:PlayAnim(b, nil, self)
    self.AnimItems[b]:PlayAnim(a, BindTool.Bind(self.AnimaEnd, self, a, b), self) ]]

    local info = self.change_list[self.play_num]
   --  LogDG("行动方案", info)
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
        self.EffectList[d]:GetItem(self.play_num):PlayAnim(d, c, BindTool.Bind(self.AnimaEnd, self, a, b, c, d), self.play_num == #self.change_list)
    else
        local a, b = tonumber(info[1]), tonumber(info[2])
        self:Show(a, false)
        self:Show(b, false)
        self.EffectList[a]:GetItem(self.play_num):PlayAnim(a, b, nil, self.play_num == #self.change_list)
        self.EffectList[b]:GetItem(self.play_num):PlayAnim(b, a, BindTool.Bind(self.AnimaEnd, self, a, b), self.play_num == #self.change_list)
        -- self.AnimItems[a]:PlayAnim(b, nil, self)
        -- self.AnimItems[b]:PlayAnim(a, BindTool.Bind(self.AnimaEnd, self, a, b), self)
    end
end
function LookingForRabbits4:AnimaEnd(a, b, c, d)
    for i = 1, self.ShowList:Length() do
        self:Show(i, true)
    end

    self.play_num = self.play_num + 1
    self:PlayAnim()
end
function LookingForRabbits4:PlayEnd()
    for i = 1, self.ShowList:Length() do
        self:Show(i, true)
    end
    LookingForRabbitsCtrl.StartGame()
    -- LogDG('请求开始10秒倒计时')
end
function LookingForRabbits4:SelectEnd()
    -- 打开兔子关闭特效
end

Rabbits4Item = Rabbits4Item or DeclareMono('Rabbits4Item', UIWidgetBaseItem)
function Rabbits4Item:Rabbits4Item()
end
function Rabbits4Item:SetData(data)
    self.Root:SetActive(false)    
    self.Root:SetLocalScale(Vector3.one)
    self.Son:SetLocalScale(Vector3.one)
    self.Son:SetLocalPosition(Vector3.zero)
    if self.from then
        self.Root:SetLocalRotation(Quaternion.Euler(0, (self.from - 1) * 90, 0))
    end
    -- self.func_count = 1
end
function Rabbits4Item:PlayAnim(from, to, end_func, is_end)

    self.from = from
    self.to = to
    self.end_func = end_func
    self.is_end = is_end

    self.Root:SetActive(true)
    self.Root:SetLocalRotation(Quaternion.Euler(0, (self.from - 1) * 90, 0))
    local diff = self.from - to
    if diff == 1 or diff == -3 then
        self.Root:SetLocalRotation(Quaternion.Euler(0, self.from * 90, 0))
    end
    local mult = math.abs(diff)
    if mult == 2 then
        mult = 2 * math.cos(math.pi / 4)
        self.Root:SetLocalRotation(Quaternion.Euler(0, (self.from - 0.5) * 90, 0))
        self.Root:SetLocalScale(Vector3.New(mult, 1, mult))
        self.Son:SetLocalScale(Vector3.New(1 / mult, 1, 1 / mult))
    end

    self.Anima:Play()
end
function Rabbits4Item:OnAnimationEvent()
    -- if self.func_count % 2 ~= 0 then
        if self.end_func then
            self.end_func()
        end
    -- end
    -- if self.func_count % 2 == 0 then
    -- end
    -- self.func_count = self.func_count + 1
end

-- Rabbits4Item = Rabbits4Item or DeclareMono('Rabbits4Item', UIWidgetBaseItem)
-- function Rabbits4Item:Rabbits4Item()
--     -- 原角度为 （index - 1） * 90
--     -- 反方向 = 原角度 + 90
--     -- 对角 = 原角度 + 45  长度变化 =  2 * cos45 (x, z)
-- end
-- function Rabbits4Item:OnEnable()
--     self.Root:SetLocalScale(Vector3.one)
--     self.Root:SetLocalRotation(Quaternion.Euler(0, (self.Index - 1) * 90, 0)) 
-- end
-- function Rabbits4Item:PlayAnim(to, end_func, parent)
--     -- LogDG("PlayAnim index", self.Index, Time.time)
--     self.parent = parent
--     self.end_func = end_func
--     self.to = to
    
--     local diff = self.Index - to
--     if diff == 1 or diff == -3 then
--         self.Root:SetLocalRotation(Quaternion.Euler(0, self.Index * 90, 0))
--     end
--     local mult = math.abs(diff)
--     if mult == 2 then
--         mult = 2 * math.cos(math.pi / 4)
--         self.Root:SetLocalRotation(Quaternion.Euler(0, (self.Index - 0.5) * 90, 0))
--         self.Root:SetLocalScale(Vector3.New(mult, 1, mult))
--         self.Son:SetLocalScale(Vector3.New(1 / mult, 1, 1 / mult))
--     end
--     self.Anima:Play()
--     self:ClearTimer()
--     self.handle_listen = AnimaHelper.ListenStop(self.Anima, BindTool.Bind(self.AnimaEnd, self))
-- end
-- function Rabbits4Item:AnimaEnd()
--     -- self.Root:SetActive(false)
--     self:ClearTimer()
--     self:OnRelese()
--     if self.end_func then
--         self.end_func()
--     end
-- end
-- function Rabbits4Item:ClearTimer()
--     Runner.Instance:RemoveRunUntilTrue(self.handle_listen)
-- end
-- function Rabbits4Item:OnRelese()
--     -- LogDG("relese index", self.Index, Time.time)
--     self.Root:SetLocalRotation(Quaternion.Euler(0, (self.Index - 1) * 90, 0))
--     self.Root:SetLocalScale(Vector3.one)
--     self.Son:SetLocalScale(Vector3.one)
--     self.Son:SetLocalPosition(Vector3.zero)
-- end

-- function Rabbits4Item:OnDestroy()
--     self:ClearTimer()
-- end
-- function Rabbits4Item:Init()
--     self.Root:SetLocalScale(Vector3.one)
--     -- self.Effect:SetLocalScale(Vector3.zero)
-- end
-- function Rabbits4Item:ShowRoot(show)
--     self.Root:SetActive(show)
-- end
-- function Rabbits4Item:PlayBoom()
--     self.BoomEffect:SetActive(false)
--     self.BoomEffect:SetActive(true)
-- end
