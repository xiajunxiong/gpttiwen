MainTaskData = MainTaskData or BaseClass()

MainTaskData.ChapterStatus = {
    Lock = "lock",
    None = "none",
    Nomal_Complete = "nomal_complete",
    Elite_Complete = "elite_complete",
}

MainTaskData.RewardStatus = {
    Wait = 1,
    Got = 2,
}

function MainTaskData:__init()
    if MainTaskData.Instance ~= nil then
        Debuger.LogError("[MainTaskData] attempt to create singleton twice!")
        return
    end 
    MainTaskData.Instance = self

    self:MainTaskData()
    self:MainTaskConfig()
end 

function MainTaskData:__delete()

end 

function MainTaskData:Clear()

end 

function MainTaskData:MainTaskConfig()
    self.config = Config.tasklist_auto
    self.config_monster_grow = self.config.monster_grow
    self.config_chapter_award = self.config.chapter_award

    self.monster_elite_unlocks = {}
    self.monster_group = {}
    for i = 1, #self.config_monster_grow do 
        if self.monster_group[self.config_monster_grow[i].seq] == nil then 
            self.monster_group[self.config_monster_grow[i].seq] = {}
        end 

        self.monster_group[self.config_monster_grow[i].seq][self.config_monster_grow[i].step] = self.config_monster_grow[i]
        if self.config_monster_grow[i].step == 1 then 
            self.monster_elite_unlocks[self.config_monster_grow[i].seq] = self.config_monster_grow[i].elite_level
        end 
    end 

    self.max_monster_page = #self.monster_group
end 

function MainTaskData:GetMonsterEliteUnlock()
    return self.monster_elite_unlocks
end

function MainTaskData:MainTaskData()
    self.chapter_info = SmartData.New()
    self.cur_view_data = SmartData.New()

    self.cur_view_data.is_flush = 0
    self.battle_data = {}
end

function MainTaskData:SetBossChapterInfo(protocol)
    self.chapter_info.reason = protocol.reason
    self.chapter_info.chapter_reward_flag = protocol.chapter_reward_flag
    -- self.chapter_info.info_list = protocol.info_list
    -- 0 初始化 1 新增 2 更新信息 3 更新其他信息
    if self.chapter_info.reason == 0 then
        self.chapter_info.info_list = protocol.info_list
    elseif self.chapter_info.reason == 1 then  
        table.insert( self.chapter_info.info_list, protocol.info_list[1])
    elseif self.chapter_info.reason == 2 then 
        for k,v in pairs(self.chapter_info.info_list) do 
            if v.monster_group_id == protocol.info_list[1].monster_group_id then
                v.reward_status = protocol.info_list[1].reward_status
            end 
        end 
    -- elseif self.chapter_info.reason == 3 then 
    end 


    self.cur_view_data.is_flush = self.cur_view_data.is_flush + 1
end

function MainTaskData:ClearBossChapterReason()
    self.chapter_info.reason = -1
end 

function MainTaskData:GetChapterRewardIsGot(_seq)
    local flag_t = bit:d2b(self.chapter_info.chapter_reward_flag)
    
    return flag_t[#flag_t - _seq ] == 0 
end

function MainTaskData:SetViewPage(value)
    self.cur_view_data.page = value
end 

function MainTaskData:GetViewPage()
    return self.cur_view_data.page
end 

function MainTaskData:GetChapterAward()
    local seq = self:GetViewPage()
    for k,v in pairs(self.config_chapter_award) do 
        if seq == v.seq then 
            return v
        end 
    end 
end 

-- true 上一页
-- false 下一页
function MainTaskData:GetAdjacentChapter(is_pre)
    -- body
    local seq = self:GetViewPage()
    local select_seq = is_pre and seq - 1 or seq + 1
    if is_pre ~= nil and select_seq > 0 and select_seq <= #self.config_chapter_award then 
        for k,v in pairs(self.config_chapter_award) do 
            if select_seq == v.seq then 
                return v
            end 
        end 
    else 
        return nil
    end 
end

function MainTaskData:GetChapeterMonster()
    local seq = self:GetViewPage()
    local result_list = {}
    for k,v in pairs(self.monster_group[seq]) do 
        local vo = {}
        vo.config = v
        vo.status_info,vo.status = self:GetMonsterStatus(v.ordinary_monster,v.elite_monster,v.task_monster)

        table.insert(result_list,vo)
    end 
    return result_list
end 

function MainTaskData:GetMonsterStatus(nomal_id,elite_id,task_id)

    if not TaskData.Instance:GetTaskIsAccepted(task_id) and not TaskData.Instance:GetTaskIsCompleted(task_id) then 
        local should_can = false
        for k,v in pairs(TaskData.Instance:GetTaskListAcceptedFollowMenu()) do 
            if v.task_id == task_id then 
                should_can = true
            end 
        end 

        if not should_can then 
            return nil,MainTaskData.ChapterStatus.Lock
        end 
    end 

    if #self.chapter_info.info_list == 0 then 
        return nil,MainTaskData.ChapterStatus.None
    end 

    local nomal_catch = nil
    local elite_catch = nil

    -- 搜索结果 普通
    for k,v in pairs(self.chapter_info.info_list) do 
        if v.monster_group_id == nomal_id then 
            nomal_catch = v --, MainTaskData.ChapterStatus.Nomal_Complete
        end 
    end 

    -- 搜索结果 精英
    for k,v in pairs(self.chapter_info.info_list) do 
        if v.monster_group_id == elite_id then 
            elite_catch = v
            --return v, MainTaskData.ChapterStatus.Elite_Complete
        end 
    end 

    if self.cur_view_data.mode_type == 1 and nomal_catch == nil then 
        return nil,MainTaskData.ChapterStatus.None
    elseif self.cur_view_data.mode_type == 1 and nomal_catch then 
        return nomal_catch,MainTaskData.ChapterStatus.Nomal_Complete
    elseif self.cur_view_data.mode_type == 2 and elite_catch == nil and nomal_catch == nil then 
        return nil,MainTaskData.ChapterStatus.None
    elseif self.cur_view_data.mode_type == 2 and elite_catch == nil and nomal_catch then 
        return nil,MainTaskData.ChapterStatus.Nomal_Complete
    elseif  self.cur_view_data.mode_type == 2 and elite_catch then 
        return elite_catch,MainTaskData.ChapterStatus.Elite_Complete
    end 

    -- if nomal_catch == nil and elite_catch == nil then 
    --     return nil,MainTaskData.ChapterStatus.None
    -- elseif nomal_catch ~= nil and elite_catch == nil then 
    --     return nomal_catch, MainTaskData.ChapterStatus.Nomal_Complete
    -- elseif nomal_catch ~= nil and elite_catch ~= nil then 
    --     if nomal_catch.reward_status == MainTaskData.RewardStatus.Wait then 
    --         return nomal_catch, MainTaskData.ChapterStatus.Nomal_Complete
    --     else 
    --         return elite_catch, MainTaskData.ChapterStatus.Elite_Complete
    --     end 
    -- end 
end 

function MainTaskData:SetViewMode(mode_type)
    self.cur_view_data.mode_type = mode_type
end

function MainTaskData:GetViewMode()
    return self.cur_view_data.mode_type
end

function MainTaskData:GetShouldBeChange(_page)
    local cur_page = _page == nil and self.cur_view_data.page or _page
    local cur_mode = self.cur_view_data.mode_type

    if cur_page == 0 then return false end 

    local killed_num = 0
    for k,v in pairs(self.monster_group[cur_page] or {}) do 
        for i,j in pairs(self.chapter_info.info_list) do 
            if j.monster_group_id == v.ordinary_monster then 
                killed_num = killed_num + 1
                break
            end 
        end 
    end 
    local monster_group = self.monster_group[cur_page] or {}
    -- 尽数击杀
    if killed_num == #monster_group then
        return true
    else 
        return false
    end 
end

function MainTaskData:GetShouldBeUnlockElite()
    local cur_page = self.cur_view_data.page
    local level = RoleData.Instance:GetRoleLevel()

    return self.monster_elite_unlocks[cur_page] <= level ,self.monster_elite_unlocks[cur_page]
end

function MainTaskData:GetIsWaitReward(monster_group_id)
    for k,v in pairs(self.chapter_info.info_list) do 
        if monster_group_id == v.monster_group_id then 
            return false
        end 
    end 
    return true
end

function MainTaskData:SetRewardPreView(flag,data_list)
    self.cur_view_data.show_pre_flag = flag

    if data_list == nil then return end 
    self.cur_view_data.pre_data_list = {}
    for k,v in pairs(data_list) do 
        local item_vo = Item.New()
        item_vo.item_id = v.item_id
        item_vo.num = v.num 
        item_vo.is_bind = v.is_bind

        table.insert( self.cur_view_data.pre_data_list, item_vo )
    end 
end

function MainTaskData:IsShowRewardPreView()
    return self.cur_view_data.show_pre_flag
end

function MainTaskData:GetRewardPreViewList()
    return self.cur_view_data.pre_data_list
end

function MainTaskData:CheckWithChapterReward()
    -- LogError("enter!!!")
    local flag_t = bit:d2b(self.chapter_info.chapter_reward_flag or 0)
    local flag = 0

    if self.chapter_info.info_list == nil then return flag end 
    for i = 1,self.max_monster_page do 
        local killed_num = 0
        for k,v in pairs(self.monster_group[i]) do 
            for i,j in pairs(self.chapter_info.info_list) do 
                if j.monster_group_id == v.ordinary_monster then 
                    killed_num = killed_num + 1
                    break
                end 
            end 
        end 

        if flag_t[#flag_t - i ] == 0 and killed_num == #self.monster_group[i] then 
            flag = 1 
        end 

        if flag > 0 then 
            break
        end 
    end 

    return flag
end

--外部预定位功能
function MainTaskData:SetQuickLocal(_xuhao,_seq)
    self.cur_view_data.local_seq = _seq
    self.cur_view_data.local_xuhao = _xuhao

    if self.cur_view_data.local_seq == nil then 
        for k,v in pairs(self.config_monster_grow) do 
            if _xuhao == v.xuhao then 
                self.cur_view_data.local_seq = v.seq
                break
            end 
        end 
    end 
end

function MainTaskData:GetQuickLoclPage()
    if self.cur_view_data.local_seq ~= nil then 
        return self.cur_view_data.local_seq
    end 

    -- 检查是否有未了之事
    for m = 1, self.max_monster_page do
        local killed_num = 0 
        for k,v in pairs(self.monster_group[m]) do
            for i,j in pairs(self.chapter_info.info_list) do 
                if j.monster_group_id == v.ordinary_monster and #v.ordinary_award > 0
                    and j.reward_status == MainTaskData.RewardStatus.Got then 
                    killed_num = killed_num + 1
                    break
                end 
                if  j.monster_group_id == v.ordinary_monster and #v.ordinary_award == 0 then 
                    killed_num = killed_num + 1
                    break
                end 
            end 
        end 

        if killed_num < #self.monster_group[m] or self:GetChapterRewardIsGot(m) then 
            return m
        end 
    end 
end

function MainTaskData:GetQuickLoclMode()
    return self.cur_view_data.local_mode
end

function MainTaskData:SetQuickLoclMode(_mode)
    self.cur_view_data.local_mode = _mode
end

function MainTaskData:ClearQuickLocal()
    self.cur_view_data.local_seq = nil
    self.cur_view_data.local_xuhao = nil
    self.cur_view_data.is_show_unlock = nil
    self.cur_view_data.local_mode = nil
end

function MainTaskData:GetSelectedPos()
    return self.cur_view_data.local_xuhao
end

function MainTaskData:SetPreViewLocal(_xuhao)
    self.cur_view_data.local_pre_view = _xuhao
end

function MainTaskData:GetPreViewLocal()
    return self.cur_view_data.local_pre_view
end

function MainTaskData:SetIsUnLock(flag)
    self.cur_view_data.is_show_unlock = flag
end

function MainTaskData:SetEliteUnLockSave(chapter_)
    local role_id = RoleData.Instance:GetBaseData().role_id
    if FunOpen.Instance:GetFunIsOpened(Mod.MainTask.EliteLock) then 
        UnityPlayerPrefs.SetInt(PrefKeys.MainTaskEliteFirst(chapter_,role_id), 1)
    else 
        UnityPlayerPrefs.SetInt(PrefKeys.MainTaskEliteFirst(chapter_,role_id), 2)
    end 
end

function MainTaskData:ReflushEliteUnLockSave(chapter_)
    local role_id = RoleData.Instance:GetBaseData().role_id
    local lock_flag = UnityPlayerPrefs.GetInt(PrefKeys.MainTaskEliteFirst(chapter_,role_id))
    if lock_flag == 2 and FunOpen.Instance:GetFunIsOpened(Mod.MainTask.EliteLock) then 
        UnityPlayerPrefs.SetInt(PrefKeys.MainTaskEliteFirst(chapter_,role_id), 1)
    end 
end

function MainTaskData:GetEliteUnLockSave(chapter_)
    local role_id = RoleData.Instance:GetBaseData().role_id
    return UnityPlayerPrefs.GetInt(PrefKeys.MainTaskEliteFirst(chapter_,role_id)) == 1 
        and FunOpen.Instance:GetFunIsOpened(Mod.MainTask.EliteLock) and self:GetEliteModeUnLockByLevel(chapter_)
end

function MainTaskData:CancelEliteUnLockSave(chapter_)
    local role_id = RoleData.Instance:GetBaseData().role_id
	UnityPlayerPrefs.SetInt(PrefKeys.MainTaskEliteFirst(chapter_,role_id), 0)
end

function MainTaskData:ReflushEliteUnLockComplete(chapter_) 
    local role_id = RoleData.Instance:GetBaseData().role_id
	UnityPlayerPrefs.SetInt(PrefKeys.MainTaskEliteFirstShow(role_id), chapter_)
end 

function MainTaskData:GetEliteUnLockComplete()
    local role_id = RoleData.Instance:GetBaseData().role_id
    return UnityPlayerPrefs.GetInt(PrefKeys.MainTaskEliteFirstShow(role_id))
end

-- 返回目前精英按钮是否解锁（等级
function MainTaskData:GetEliteModeUnLockByLevel(chapter_)
    local level = RoleData.Instance:GetRoleLevel()
    return self:GetMonsterEliteUnlock()[chapter_] == level 
end

function MainTaskData:GetIsUnLock()
    return self.cur_view_data.is_show_unlock
end

function MainTaskData:SetIsEliteBattle(flag,map_id)
    self.battle_data.is_elite_battle = flag
    self.battle_data.map_id = map_id
end

function MainTaskData:SetIsNomalBattle(flag,map_id)
    self.battle_data.is_nomal_battle = flag
    self.battle_data.map_id = map_id
end

function MainTaskData:GetIsEliteBattle()
    return self.battle_data.is_elite_battle 
end

function MainTaskData:GetIsNomalBattle()
    return self.battle_data.is_nomal_battle 
end

function MainTaskData:GetBattleMapId()
    return self.battle_data.map_id
end

function MainTaskData:ClearBattleData()
    self.battle_data.is_elite_battle = nil
    self.battle_data.is_nomal_battle = nil
    self.battle_data.map_id = nil
end

function MainTaskData:CheckMainTaskElite()
    -- 新增时检查是否完成了一整个章节的怪的击杀
    if self.chapter_info.reason == 1 then 
         -- 自动定位到最新的击杀位置
        for m = 1, self.max_monster_page do
            local killed_num = 0 
            local monster_group = self.monster_group[m]

            -- 最新击杀的是该章的最终怪
            if monster_group[#monster_group].ordinary_monster 
                == self.chapter_info.info_list[#self.chapter_info.info_list].monster_group_id then 
                return true , m
            end 

            -- for k,v in pairs(self.monster_group[m]) do
            --     for i,j in pairs(self.chapter_info.info_list) do 
            --         if j.monster_group_id == v.ordinary_monster then
            --             killed_num = killed_num + 1
            --             break
            --         end 
            --     end 
            -- end 

            -- --击杀是顺序执行且不可逆的，所以当前章若未完成击杀的话就返回前一章
            -- if killed_num < #self.monster_group[m] and m - 1 >0 then 
            --     return true,m - 1
            -- end 
        end 

        -- 新增功能 返回的是击杀精英的话标记位置
        for m = 1, self.max_monster_page do
            local killed_num = 0 
            for k,v in pairs(self.monster_group[m]) do 
                if v.elite_monster == self.chapter_info.info_list[#self.chapter_info.info_list].monster_group_id then 
                    return false,m
                end 
            end 
        end 

        return false,nil
    else 
        return false,nil
    end 
end