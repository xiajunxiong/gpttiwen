AllSoulsStorybookStart =
    AllSoulsStorybookStart or DeclareView('AllSoulsStorybookStart', 'all_souls_storybook/all_souls_start')
function AllSoulsStorybookStart:AllSoulsStorybookStart()
    self.click_time = 0
    self.data = AllSoulsStorybookData.Instance
end
function AllSoulsStorybookStart:LoadCallback(param)
    --点一次展示所有文字
    local info = self.data:TaskInfo()
    if param and param.use_cur then
        info = self.data:CurTask()
    end
    -- LogError("start view task_info",info)
    if info then
        UH.SetText(self.Desc, info.mb_dec)
        local task_list = self.data:TaskList()
        local task_seq = self.data:SceneInfo().seq
        self.task_info = task_list[task_seq]
    else
        UH.SetText(self.Desc, "")
    end
    self.has_not_play = false
    self.delay_play = TimeHelper:AddDelayTimer(function ()
        self.has_not_play = true
        self.WordAnim:Play()
    end,1.5)
    self.delay_close = TimeHelper:AddDelayTimer(function ()
        ViewMgr:CloseView(AllSoulsStorybookStart)
    end, 7)
end

function AllSoulsStorybookStart:CloseCallback()
    if self.delay_play ~= nil then
        TimeHelper:CancelTimer(self.delay_play)
        self.delay_play = false
    end
    if self.delay_close ~= nil then
        TimeHelper:CancelTimer(self.delay_close)
        self.delay_close = nil
    end
    local cur_task = self.data:CurTask()
    self.data:doTask(cur_task)
    UH.SizeDelta(self.WordRect, Vector2.New(835, 0))
    self.click_time = 0
end
function AllSoulsStorybookStart:OnClickClose()
    if self.WordAnim.isPlaying == false and self.has_not_play then
        if self.task_info then
            self.data:doTask(self.task_info)
        end
        ViewMgr:CloseView(AllSoulsStorybookStart)
        return
    end
    if self.click_time == 0 then
        if self.delay_play then
            TimeHelper:CancelTimer(self.delay_play)
        end
        self.WordAnim:Stop()
        UH.SizeDelta(self.WordRect, Vector2.New(835, 400))
        self.click_time = self.click_time + 1
    else
        if self.task_info then
            self.data:doTask(self.task_info)
        end
        ViewMgr:CloseView(AllSoulsStorybookStart)
    end
end
