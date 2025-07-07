AllSoulsStorybookMenu = AllSoulsStorybookMenu or DeclareMono("AllSoulsStorybookMenu", UIWFlushPanel)
function AllSoulsStorybookMenu:AllSoulsStorybookMenu()
    self.data = AllSoulsStorybookData.Instance
    self.data_cares = {{
        data = self.data:SceneInfo(),
        func = self.FlushTask
    }}
    self.npc_list = {}
end
function AllSoulsStorybookMenu:OnEnable()
    UIWFlushPanel.OnEnable(self)
    self.npc_list = {}
    self:FlushTask()
end
function AllSoulsStorybookMenu:FlushTask()
    -- LogError("k176k176k176k176")
    -- 获取当前任务信息
    -- 并显示NPC等
    local task_list = self.data:TaskList()
    local task_seq = self.data:SceneInfo().seq
    local info = task_list[task_seq]
    self.task_info = info
    if info then
        UH.SetText(self.Name, info.task_name)
        UH.SetText(self.Desc, info.task_dec)
    else
        UH.SetText(self.Name, "")
        UH.SetText(self.Desc, "")
    end
    self:FlushProgress()
    local rewards = self.data:GetCacheRewards()
    self.RewardList:SetData(rewards)
    self.NoneObj:SetActive(#rewards == 0)
end
function AllSoulsStorybookMenu:OnClickTeam()
    ViewMgr:OpenView(TeamView)
end
function AllSoulsStorybookMenu:OnClickTask()
    self.data:doTask(self.task_info)
    BehaviorData.Instance.auto_behavior = 5
end

function AllSoulsStorybookMenu:FlushProgress()
    local task_list = AllSoulsStorybookData.Instance:TaskList()
    local total_task = 0
    local boss_index = 0
    local passed_boss = 0
    local boss_space = {}
    local pro_num = 0
    for _, v in pairs(task_list) do
        total_task = total_task + 1
        if v.task_type == 1 then -- 已经完成的战斗任务
            if self.task_info.task_num > v.task_num then
                boss_index = boss_index + 1
                passed_boss = passed_boss + 1
                self:FlushBossIcon(boss_index, false)
            else
                boss_index = boss_index + 1
                self:FlushBossIcon(boss_index, true)
            end
            boss_space[boss_index] = v.task_num
        end
    end
    local pre_v = 0
    local complete_num = self.task_info.task_num - 1
    for i, v in ipairs(boss_space) do
        if complete_num >= v then
            pro_num = pro_num + 0.3
            pre_v = v
        else
            local temp_prog = (complete_num - pre_v) / (v - pre_v) * 0.3
            pro_num = pro_num + temp_prog
            break
        end
    end
    if passed_boss >= 3 then
        pro_num = (complete_num - boss_space[3]) / (total_task - 1 - boss_space[3]) * 0.1 + pro_num
    end
    self.Progress:SetProgress(pro_num)
end

function AllSoulsStorybookMenu:FlushBossIcon(boss_index, interactor)
    if self.BossInteractor[boss_index] then
        self.BossInteractor[boss_index].Interactable = interactor
        self.BossCha[boss_index]:SetActive(not interactor)
    end
end

function AllSoulsStorybookMenu:OnClickRewards()
    local rewards = self.data:GetCacheRewards()
    ViewMgr:OpenView(AllSoulsStorybookRewardsView, rewards)
end
