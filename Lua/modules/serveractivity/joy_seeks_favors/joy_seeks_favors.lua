JoySeeksFavorsView =
    JoySeeksFavorsView or
    DeclareView("JoySeeksFavorsView", "serveractivity/joy_seeks_favors", Mod.RandActivity.JoySeeksFavors)
-- 方向 坐标从左上开始 (0,0)
--       0
--     *****
--     *****
-- 3   *****   1
--     *****
--     *****
--       2
JoySeeksFavorsPanel = JoySeeksFavorsPanel or DeclareMono("JoySeeksFavorsPanel", UIWFlushPanel)
function JoySeeksFavorsPanel:JoySeeksFavorsPanel()
    self.data = JoySeeksFavorsData.Instance
    self.ctrl = JoySeeksFavorsCtrl.Instance
    self.data_cares = {
        {data = self.data:ViewData(), func = self.FlushMatch, keys = {"is_match"}},
        {data = self.data:MySelfMap(), func = self.FlushMySelfMap},
        {data = self.data:EnemyMap(), func = self.FlushEnemyMap},
        {data = self.data:MySelfMap(), func = self.FlushTime},
        {data = self.data:EnemyMap(), func = self.FlushTime},
        {data = self.data:ViewData(), func = self.FlushNotice, keys = {"opera", "is_match"}},
        {data = self.data:ViewData(), func = self.FlushEffect, keys = {"has_find"}},
    }
    self.pre_grid = {}
    self.origin_point = nil
    --点击
    self.change_point = nil
    --最后改变发协议的点
    self.cache_point = nil
    --缓存
    self.origin_point_list = {}
    --当前列表
    self.cache2_list = {}
    --保留当前可旋转方案列表
    self.cache3_list1 = {}
    --中间
    self.cache3_list2 = {}
    --两侧
    --保留
    self.use_item = false
    self.menu_pos = Vector2.zero
    self.item_pos = Vector2.zero

    self.CatchTips:ListenEvent("anim_state",
        function (eve_param,anim_st_info)
            if eve_param == "complete" then 
                self.CatchTipsObj:SetActive(false)
            end 
        end) 

    self.BeCatchTips:ListenEvent("anim_state",
        function (eve_param,anim_st_info)
            if eve_param == "complete" then 
                self.BeCatchTipsObj:SetActive(false)
            end 
        end)    
end
function JoySeeksFavorsPanel:OnClickTip()
   ViewMgr:OpenView(JoySeeksFavorsTip)
end
function JoySeeksFavorsPanel:FlushEffect()
    local view_data = self.data:ViewData()
    -- LogError(view_data)
    if view_data.has_find == 0 then
        return
    end
    if view_data.joy_seq <= 0 then
        return
    end
    if view_data.joy_seq > 3 then
        return
    end
    self.EggGetTimer = TimeHelper:AddDelayFrameTimer(
        function()
            local index = view_data.has_find == 1 and 2 or 1
            if view_data.joy_seq > 0 and view_data.joy_seq <= 3 then
                local pos = self["EggList" .. index][view_data.joy_seq].anchoredPosition
                UH.AnchoredPosition(self.EggGet, pos)
                self.EggGetAnim:SetActive(false)
                self.EggGetAnim:SetActive(true)
            end 
        end,
        5
    )
    self:FlushNum()

    if view_data.has_find == 1 then 
        self.lblCatchTips.text = self.data.catch_tips.val
        self.CatchTipsObj:SetActive(true)
    else 
        self.lblBeCatchTips.text = self.data.catch_tips.val
        self.BeCatchTipsObj:SetActive(true)
    end 
end
function JoySeeksFavorsPanel:OnClickStartGuide()
    self.StartView:SetActive(false)
    self.MasterStart:SetActive(false)
    self.GuideList[1]:SetActive(true)
    self.PlayRewardTips:SetActive(false)
    self.BtnTips:SetActive(false)
    -- self.GuideList[2]:SetActive(true)
    -- self.GuideList[3]:SetActive(true)
    -- self.GuideView:SetActive(true)
end
function JoySeeksFavorsPanel:OnClickGuide(step)
    if step == 1 then
        self.GuideList[1]:SetActive(false)
        self.GuideList[2]:SetActive(true)
    -- elseif step == 2 then
    --     self.GuideList[2]:SetActive(false)
    --     self.GuideList[3]:SetActive(true)
    else
        -- self.GuideView:SetActive(false)
        self.GuideList[1]:SetActive(false)
        self.GuideList[2]:SetActive(false)
        -- self.GuideList[3]:SetActive(false)
        self.StartView:SetActive(true)
    end
end
function JoySeeksFavorsPanel:FlushNotice()
    --在开始阶段
    --[[ local myself = self.data:MySelfMap() ]]
    --local enemy = self.data:EnemyMap()
    --[[ if next(myself) == nil then
        return
    end ]]
    --self.NoticeObj:SetActive(MATCH_TYPE.OPERATION == match)
    local match = self.data:ViewData().is_match
    self.BtnTips:SetActive(match == MATCH_TYPE.SHOW or match == MATCH_TYPE.OPERATION or match == MATCH_TYPE.NONE)
    self.PlayRewardTips:SetActive(match ~= MATCH_TYPE.MATCHING)
    -- LogError("match??",match)
    if match == MATCH_TYPE.NONE or match == MATCH_TYPE.SETTING
    or match == MATCH_TYPE.SHOW or match == MATCH_TYPE.MATCHING then
        return
    end
    local opera = self.data:ViewData().opera
    self.OperaObj1:SetActive(opera)
    self.OperaObj2:SetActive(not opera)

    self.Open1[1]:SetActive(not opera)
    self.Open1[2]:SetActive(not opera)

    self.Open2[1]:SetActive(opera)
    self.Open2[2]:SetActive(opera)

    --[[ UH.SetText(
        self.NoticeTex,
        string.format(
            Language.JoySeeksFavors.OperaText,
            opera == true and myself.role_info.role_name or enemy.role_info.role_name
        )
    )
    UH.SetText(self.TipTex, Language.JoySeeksFavors.PlayState[opera == true and 1 or 2]) ]]
end
function JoySeeksFavorsPanel:FlushMatch()
    local match = self.data:ViewData().is_match
    local match_show = match == 1
    local start_show = match == 0
    self.MatchView:SetActive(match_show)
    -- self.StartView:SetActive(start_show and not self:CheckIsStart())
    self.MasterStart:SetActive(start_show)
    self.BtnTip:SetActive(match == MATCH_TYPE.NONE)
    if match == MATCH_TYPE.NONE or match == MATCH_TYPE.SHOW
    or match == MATCH_TYPE.SETTING or match == MATCH_TYPE.MATCHING then
        self.OperaObj1:SetActive(false)
        self.OperaObj2:SetActive(false)
        self.Open2[1]:SetActive(false)
        self.Open2[2]:SetActive(false)
        self.SelectItem:SetActive(false)
        if match ~= MATCH_TYPE.SHOW then
            self.Open1[1]:SetActive(false)
            self.Open1[2]:SetActive(false)
        end
        if match == MATCH_TYPE.SETTING then
            self.Open1[1]:SetActive(true)
            self.Open1[2]:SetActive(true)
        end
    end
    --进行初始化

    --[[ if show then
        UH.SetText(self.Name1, "")
        UH.SetText(self.Name2, "")
        UH.SetText(self.Name11, "")
        UH.SetText(self.Name22, "")
        self:FlushNum()
    end
    if match > 2 then
        return
    end
    UH.SetText(self.MatchText, Language.JoySeeksFavors.MatchText[match]) ]]
    if match_show then 
        TimeHelper:CancelTimer(self.match_time_func)
        self.match_start_time = 0

        self.match_time_func = TimeHelper:AddRunTimer(function()
            self.match_start_time = self.match_start_time + 1
            local time_ = TimeManager.FormatDHMS(self.match_start_time)
            if time_.hour < 10 then time_.hour = "0".. time_.hour end 
            if time_.min < 10 then time_.min = "0".. time_.min end 
            if time_.s < 10 then time_.s = "0".. time_.s end 
            self.MatchTimer.text = time_.hour..":"..time_.min..":"..time_.s
        end,1)
    else 
        TimeHelper:CancelTimer(self.match_time_func)
        self.match_start_time = 0
        self.MatchTimer.text = ""
    end 
end
function JoySeeksFavorsPanel:FlushTime()
    self:FlushNum()
    TimeHelper:CancelTimer(self.count_time)
    self.RoundCount.text = ""
    local match = self.data:ViewData().is_match
    local ready_tamp = self.data:ViewData().ready_tamp
    local next_move_tamp = self.data:ViewData().next_move_tamp
    if MATCH_TYPE.SETTING == match and ready_tamp > 0 then
        local time = math.floor(ready_tamp - TimeHelper.GetServerTime())
        self:PlayCountDown(time)
        self.SettingTex.gameObject:SetActive(true)
        self.TimeMeter[3]:StampTime(ready_tamp, TimeType.Type_Special_2, Language.JoySeeksFavors.TimeDesc)
    else
        self.SettingTex.gameObject:SetActive(false)
    end
    if MATCH_TYPE.OPERATION == match then
        self.CountDownObj:SetActive(false)
        self.CountDownAnim:SetTrigger("tri_default")
        self.TimeMeter[1]:StampTime(next_move_tamp, TimeType.Type_Special_2,Language.JoySeeksFavors.TimeDesc)
        self.TimeMeter[2]:StampTime(next_move_tamp, TimeType.Type_Special_2,Language.JoySeeksFavors.TimeDesc)
        self:ClearPre()
        self.MenuObj.gameObject:SetActive(false)
    end
end
function JoySeeksFavorsPanel:OnDestroy()
    UIWFlushPanel.OnDestroy(self)
    TimeHelper:CancelTimer(self.count_time)
    TimeHelper:CancelTimer(self.EggGetTimer)
    TimeHelper:CancelTimer(self.match_time_func)
end
function JoySeeksFavorsPanel:PlayCountDown(end_time)
    TimeHelper:CancelTimer(self.count_time)
    local show_num = end_time
    self.count_time = TimeHelper:AddCountDownTT(function()
        if show_num <= 3 then
            self.CountDownObj:SetActive(true)
            self.RoundCount.text = show_num
            self.CountDownAnim:SetTrigger("tri_321")
            self.SettingTex.gameObject:SetActive(false)
        end
        show_num = show_num - 1
	end,
    function()
        self.RoundCount.text = 0
        -- self.CountDownAnim:SetTrigger("tri_go")
        self.CountDownAnim:SetTrigger("tri_default")
        self.CountDownObj:SetActive(false)
	end,end_time,1,true)
end
function JoySeeksFavorsPanel:onFlush()
    self:FlushMySelfMap()
    self:FlushNum()
    -- self.Reward:SetData(Item.Create(self.data:Reward()))
    local list = {}
    table.insert( list, Item.Create(self.data:Reward()))
    self.RewardList:SetData(list)
    self.RewardTopList:SetData(list)
    self.RewardMatchList:SetData(list)
    for i=1,11 do
        UH.SetText(self.WordList[i], Language.JoySeeksFavors.WordText[i])
    end
end
function JoySeeksFavorsPanel:FlushNum()
    local num_list1 = self.data:MyEggInfo()
    for i = 1, 3 do
        UH.SetText(self.NumList1[i], num_list1[i])
    end
    local num_list2 = self.data:EyEggInfo()
    for i = 1, 3 do
        UH.SetText(self.NumList2[i], num_list2[i])
    end
end
function JoySeeksFavorsPanel:FlushEnemyMap()
    --刷新敌人地图 游玩时除了打开的显示其他都不显示
    local enemy_map = self.data:EnemyMap()
    if next(enemy_map) == nil then
        for i = 1, 3 do
            self.EggList2[i].gameObject:SetActive(false)
        end
        for i = 1, 5 do
            for j = 1, 5 do
                local index = (i - 1) * 5 + j
                UH.SpriteName(self.GridList2[index], "gezi")
            end
        end
        return
    end
    --在操作阶段只显示open的
    UH.SetAvatar(self.Avatar2, enemy_map.role_info.avatar_type,enemy_map.role_info.avatar_id,enemy_map.role_info.avatar_quality)
    UH.SetText(self.Name2, enemy_map.role_info.role_name)

    -- UH.SetText(self.Name22, enemy_map.role_info.role_name)
    local match = self.data:ViewData().is_match
    local player_joy_map = enemy_map.player_joy_map
    if MATCH_TYPE.OPERATION == match or MATCH_TYPE.SHOW == match then
        for i = 1, 5 do
            for j = 1, 5 do
                local index = (i - 1) * 5 + j
                local grid = player_joy_map[i][j]
                if grid.is_open ~= 0 then
                    if grid.joy_seq ~= 0 then
                        self.EggList2[grid.joy_seq].gameObject:SetActive(true)
                        local pos = self.ClickList2[index].anchoredPosition
                        pos.y = pos.y + 20
                        UH.AnchoredPosition(self.EggList2[grid.joy_seq], pos)
                    end
                    UH.SpriteName(self.GridList2[index], "gezi2")
                end
            end
        end
    else
        for i = 1, 3 do
            self.EggList2[i].gameObject:SetActive(false)
        end
        for i = 1, 5 do
            for j = 1, 5 do
                local index = (i - 1) * 5 + j
                local grid = player_joy_map[i][j]
                if grid.joy_index ~= 0 then
                    UH.SpriteName(self.GridList2[index], "gezi")
                end
            end
        end
    end
    self:FlushMySelfMap()
    if enemy_map.role_info.find_item > 0 then
        self.use_item = false
        --[[ self.Select1:SetActive(false)
        self.Select2:SetActive(true) ]]
        self.SelectItem:SetActive(false)
    end
end

function JoySeeksFavorsPanel:CheckIsStart()
    return self.MasterStart.activeSelf
end
function JoySeeksFavorsPanel:FlushMySelfMap()
    --通过服务端发来的信息显示
    --准备阶段只显示左边的地图(给玩家放置)
    local myself_map = self.data:MySelfMap()
    if next(myself_map) == nil then
        for i = 1, 3 do
            self.EggList1[i].gameObject:SetActive(false)
        end
        for i = 1, 5 do
            for j = 1, 5 do
                local index = (i - 1) * 5 + j
                UH.SpriteName(self.GridList1[index], "gezi")
            end
        end
        return
    end
    local match = self.data:ViewData().is_match
    local player_joy_map = myself_map.player_joy_map
    if MATCH_TYPE.SETTING == match then
        --显示蛋和格子3
        --设置阶段显示蛋和深色块
        --头像
        for i = 1, 5 do
            for j = 1, 5 do
                local index = (i - 1) * 5 + j
                local grid = player_joy_map[i][j]
                if grid.joy_index ~= 0 then
                    self.EggList1[grid.joy_seq].gameObject:SetActive(true)
                    local pos = self.ClickList1[index].anchoredPosition
                    pos.y = pos.y + 20
                    UH.AnchoredPosition(self.EggList1[grid.joy_seq], pos)
                    UH.SpriteName(self.GridList1[index], "gezi3")
                else
                    UH.SpriteName(self.GridList1[index], "gezi")
                end
            end
        end
    elseif MATCH_TYPE.SHOW == match or MATCH_TYPE.OPERATION then --展示阶段
        for i = 1, 3 do
            self.EggList1[i].gameObject:SetActive(false)
        end
        --操作阶段
        --我的这边就显示就好了
        for i = 1, 5 do
            for j = 1, 5 do
                local index = (i - 1) * 5 + j
                local grid = player_joy_map[i][j]
                if grid.is_open ~= 0 then
                    if grid.joy_seq ~= 0 then
                        self.EggList1[grid.joy_seq].gameObject:SetActive(true)
                        local pos = self.ClickList1[index].anchoredPosition
                        pos.y = pos.y + 20
                        UH.AnchoredPosition(self.EggList1[grid.joy_seq], pos)
                    end
                    UH.SpriteName(self.GridList1[index], "gezi2")
                else
                    UH.SpriteName(self.GridList1[index], "gezi")
                end
            end
        end
    else
        for i = 1, 3 do
            self.EggList1[i].gameObject:SetActive(false)
        end
        for i = 1, 5 do
            for j = 1, 5 do
                local index = (i - 1) * 5 + j
                local grid = player_joy_map[i][j]
                if grid.joy_index ~= 0 then
                    UH.SpriteName(self.GridList1[index], "gezi")
                end
            end
        end
    end
end
function JoySeeksFavorsPanel:OnClickClose()
    ViewMgr:CloseView(JoySeeksFavorsView)
end
--[[ function JoySeeksFavorsPanel:OnClickTips()
    ViewMgr:OpenView(JoySeeksFavorsTip)
end ]]
function JoySeeksFavorsPanel:OnClickMatch()
    local match = self.data:ViewData().is_match == 0 and 1 or 0
    self.ctrl:JoySeeksFavorsMatchReq(match)

    self.StartView:SetActive(false)
    -- self.MasterStart:SetActive(false)
end

function JoySeeksFavorsPanel:OnClickGrid(x, y)
    --放置阶段
    local match = self.data:ViewData().is_match
    if MATCH_TYPE.SETTING ~= match then
        return
    end

    --判断当前是否选中了蛋，没有就return
    --有就显示菜单和并记录，当点击其他位置时进行 解析并更换位置
    local myself_map = self.data:MySelfMap()
    if next(myself_map) == nil then
        return
    end
    local player_joy_map = myself_map.player_joy_map
    local index = self:CalcIndex(x, y)
    local point = player_joy_map[x + 1][y + 1]
    point = self:CreatePoint(point, x, y, index)
    if self.origin_point then
        --我需要知道你是啥蛋才能给你安排位置
        --判断新点的是否有
        --比如是1,我只要判断更换位置是否已有就可以了
        if self.origin_point.joy_seq == 1 then
            if point.joy_seq > 0 then
                return
            else
                self:ClearPre()
                self.change_point = point
                self:FlushMenu(point)
                UH.SpriteName(self.GridList1[index], "gezi3")
                table.insert(self.pre_grid, self.GridList1[index])
                return
            end
        elseif self.origin_point.joy_seq == 2 then
            if point.joy_seq == 0 then
                self:ClearPre()
                local point2, dir = self:Grid2(point)
                self:Grid2All(point)
                if point2 == nil then
                    return
                end
                UH.SpriteName(self.GridList1[index], "gezi3")
                UH.SpriteName(self.GridList1[point2.index], "gezi3")
                table.insert(self.pre_grid, self.GridList1[index])
                table.insert(self.pre_grid, self.GridList1[point2.index])

                point.dir = dir
                self.change_point = point
                self:FlushMenu(point)
            end
        elseif self.origin_point.joy_seq == 3 then
            if point.joy_seq == 0 then
                self:ClearPre()
                local point2, dir = self:Grid3(point)
                --这个点就是中间点
                self:Grid3All(point)
                if point2 == nil then
                    return
                end
                -- point.dir = dir
                -- point2.dir = dir
                self.change_point = point2
                --得到了准确的点
                self.cache_point = point
                self:FlushMenu(point)
            end
        end
    end
    if point.joy_index == 0 then
        --二次点击和一次点击无事件格子
        return
    end
    local temp = {}
    for i = 1, 5 do
        for j = 1, 5 do
            if player_joy_map[i][j].joy_seq == point.joy_seq then
                local p = player_joy_map[i][j]
                p.x = i - 1
                p.y = j - 1
                p.index = (p.x * 5) + (p.y + 1)
                table.insert(temp, p)
            end
        end
    end

    if self.origin_point == nil then
        self.origin_point = point
    end
    --无二次点击时
    self.change_point = point
    self.origin_point_list = temp

    if point == self.origin_point_list[2] then
        --判断是否是中心
        self.is_click_middle = true
    else
        self.is_click_middle = false
    end
    self.cache_point = point

    self.save_point_list = temp
    self:Grid2All(point)
    self:Grid3All(point)
    --菜单按钮也有需求
    self:FlushMenu(point)
end
function JoySeeksFavorsPanel:Grid2(point)
    local myself_map = self.data:MySelfMap()
    if next(myself_map) == nil then
        return
    end
    local player_joy_map = myself_map.player_joy_map
    --用于计算格子索引
    local x = point.x
    local y = point.y
    --point坐标是0开始 转换为数据索引
    local x1 = point.x + 1
    local y1 = point.y + 1
    --检查四周是否有可放预览
    if player_joy_map[x1 - 1] then
        if player_joy_map[x1 - 1][y1].joy_seq == 0 then
            local index2 = self:CalcIndex(x - 1, y)
            return self:CreatePoint(player_joy_map[x1 - 1][y1], x - 1, y, index2), 0
        end
    end
    if player_joy_map[x1][y1 + 1] then
        if player_joy_map[x1][y1 + 1].joy_seq == 0 then
            local index2 = self:CalcIndex(x, y + 1)
            return self:CreatePoint(player_joy_map[x1][y1 + 1], x, y + 1, index2), 1
        end
    end
    if player_joy_map[x1 + 1] then
        if player_joy_map[x1 + 1][y1].joy_seq == 0 then
            local index2 = self:CalcIndex(x + 1, y)
            return self:CreatePoint(player_joy_map[x1 + 1][y1], x - 1, y, index2), 2
        end
    end
    if player_joy_map[x1][y1 - 1] then
        if player_joy_map[x1][y1 - 1].joy_seq == 0 then
            local index2 = self:CalcIndex(x, y - 1)
            return self:CreatePoint(player_joy_map[x1][y1 - 1], x, y - 1, index2), 3
        end
    end
    return nil
end
function JoySeeksFavorsPanel:Grid2All(point)
    self.cache2_list = {}
    local myself_map = self.data:MySelfMap()
    if next(myself_map) == nil then
        return
    end
    local player_joy_map = myself_map.player_joy_map
    --用于计算格子索引
    local x = point.x
    local y = point.y
    --point坐标是0开始 转换为数据索引
    local x1 = point.x + 1
    local y1 = point.y + 1
    --检查四周是否有可放预览
    if player_joy_map[x1 - 1] then
        if player_joy_map[x1 - 1][y1].joy_seq == 0 then
            local index2 = self:CalcIndex(x - 1, y)
            self.cache2_list[0] = self:CreatePoint(player_joy_map[x1 - 1][y1], x - 1, y, index2)
        end
    end
    if player_joy_map[x1][y1 + 1] then
        if player_joy_map[x1][y1 + 1].joy_seq == 0 then
            local index2 = self:CalcIndex(x, y + 1)
            self.cache2_list[1] = self:CreatePoint(player_joy_map[x1][y1 + 1], x, y + 1, index2)
        end
    end
    if player_joy_map[x1 + 1] then
        if player_joy_map[x1 + 1][y1].joy_seq == 0 then
            local index2 = self:CalcIndex(x + 1, y)
            self.cache2_list[2] = self:CreatePoint(player_joy_map[x1 + 1][y1], x - 1, y, index2)
        end
    end
    if player_joy_map[x1][y1 - 1] then
        if player_joy_map[x1][y1 - 1].joy_seq == 0 then
            local index2 = self:CalcIndex(x, y - 1)
            self.cache2_list[3] = self:CreatePoint(player_joy_map[x1][y1 - 1], x, y - 1, index2)
        end
    end
end
function JoySeeksFavorsPanel:Grid3(point)
    --当前位置处于什么位置
    local myself_map = self.data:MySelfMap()
    if next(myself_map) == nil then
        return
    end
    local player_joy_map = myself_map.player_joy_map
    --用于计算格子索引
    local x = point.x
    local y = point.y
    --point坐标是0开始 转换为数据索引
    local x1 = point.x + 1
    local y1 = point.y + 1
    --中间位置就是 只有两个方向
    if self.is_click_middle then
        --当前位置是否可以组成|——
        if player_joy_map[x1 - 1] and player_joy_map[x1 + 1] then --|
            if player_joy_map[x1 - 1][y1].joy_seq == 0 and player_joy_map[x1 + 1][y1].joy_seq == 0 then
                local index = self:CalcIndex(x - 1, y)
                local index2 = self:CalcIndex(x + 1, y)
                local point1 = self:CreatePoint(player_joy_map[x1 - 1][y1], x - 1, y, index)
                local point2 = self:CreatePoint(player_joy_map[x1 + 1][y1], x + 1, y, index2)
                UH.SpriteName(self.GridList1[point.index], "gezi3")
                UH.SpriteName(self.GridList1[point1.index], "gezi3")
                UH.SpriteName(self.GridList1[point2.index], "gezi3")
                table.insert(self.pre_grid, self.GridList1[point.index])
                table.insert(self.pre_grid, self.GridList1[point1.index])
                table.insert(self.pre_grid, self.GridList1[point2.index])
                point1.dir = 2
                return point1, 2
            end
        end
        if player_joy_map[x1][y1 - 1] and player_joy_map[x1][y1 + 1] then --——
            if player_joy_map[x1][y1 - 1].joy_seq == 0 and player_joy_map[x1][y1 + 1].joy_seq == 0 then
                local index = self:CalcIndex(x, y - 1)
                local index2 = self:CalcIndex(x, y + 1)
                local point1 = self:CreatePoint(player_joy_map[x1][y1 - 1], x, y - 1, index)
                local point2 = self:CreatePoint(player_joy_map[x1][y1 + 1], x, y + 1, index2)
                UH.SpriteName(self.GridList1[point.index], "gezi3")
                UH.SpriteName(self.GridList1[point1.index], "gezi3")
                UH.SpriteName(self.GridList1[point2.index], "gezi3")
                table.insert(self.pre_grid, self.GridList1[point.index])
                table.insert(self.pre_grid, self.GridList1[point1.index])
                table.insert(self.pre_grid, self.GridList1[point2.index])
                point1.dir = 1
                return point1, 1
            end
        end
    else
        --根据该点进行上下左右计算
        if player_joy_map[x1 - 1] and player_joy_map[x1 - 2] then
            if player_joy_map[x1 - 1][y1].joy_seq == 0 and player_joy_map[x1 - 2][y1].joy_seq == 0 then
                local index = self:CalcIndex(x - 1, y)
                local index2 = self:CalcIndex(x - 2, y)
                local point1 = self:CreatePoint(player_joy_map[x1 - 1][y1], x - 1, y, index)
                local point2 = self:CreatePoint(player_joy_map[x1 - 2][y1], x - 2, y, index2)
                UH.SpriteName(self.GridList1[point.index], "gezi3")
                UH.SpriteName(self.GridList1[point1.index], "gezi3")
                UH.SpriteName(self.GridList1[point2.index], "gezi3")
                table.insert(self.pre_grid, self.GridList1[point.index])
                table.insert(self.pre_grid, self.GridList1[point1.index])
                table.insert(self.pre_grid, self.GridList1[point2.index])
                point.dir = 0
                return point, 0
            end
        end
        if player_joy_map[x1][y1 + 1] and player_joy_map[x1][y1 + 2] then
            if player_joy_map[x1][y1 + 1].joy_seq == 0 and player_joy_map[x1][y1 + 2].joy_seq == 0 then
                local index = self:CalcIndex(x, y + 1)
                local index2 = self:CalcIndex(x, y + 2)
                local point1 = self:CreatePoint(player_joy_map[x1][y1 + 1], x, y + 1, index)
                local point2 = self:CreatePoint(player_joy_map[x1][y1 + 2], x, y + 2, index2)
                UH.SpriteName(self.GridList1[point.index], "gezi3")
                UH.SpriteName(self.GridList1[point1.index], "gezi3")
                UH.SpriteName(self.GridList1[point2.index], "gezi3")
                table.insert(self.pre_grid, self.GridList1[point.index])
                table.insert(self.pre_grid, self.GridList1[point1.index])
                table.insert(self.pre_grid, self.GridList1[point2.index])
                point.dir = 1
                return point, 1
            end
        end
        if player_joy_map[x1 + 1] and player_joy_map[x1 + 2] then
            if player_joy_map[x1 + 1][y1].joy_seq == 0 and player_joy_map[x1 + 2][y1].joy_seq == 0 then
                local index = self:CalcIndex(x + 1, y)
                local index2 = self:CalcIndex(x + 2, y)
                local point1 = self:CreatePoint(player_joy_map[x1 + 1][y1], x + 1, y, index)
                local point2 = self:CreatePoint(player_joy_map[x1 + 2][y1], x + 2, y, index2)
                UH.SpriteName(self.GridList1[point.index], "gezi3")
                UH.SpriteName(self.GridList1[point1.index], "gezi3")
                UH.SpriteName(self.GridList1[point2.index], "gezi3")
                table.insert(self.pre_grid, self.GridList1[point.index])
                table.insert(self.pre_grid, self.GridList1[point1.index])
                table.insert(self.pre_grid, self.GridList1[point2.index])
                point.dir = 2
                return point, 2
            end
        end
        if player_joy_map[x1][y1 - 1] and player_joy_map[x1][y1 - 2] then
            if player_joy_map[x1][y1 - 1].joy_seq == 0 and player_joy_map[x1][y1 - 2].joy_seq == 0 then
                local index = self:CalcIndex(x, y - 1)
                local index2 = self:CalcIndex(x, y - 2)
                local point1 = self:CreatePoint(player_joy_map[x1][y1 - 1], x, y - 1, index)
                local point2 = self:CreatePoint(player_joy_map[x1][y1 - 2], x, y - 2, index2)
                UH.SpriteName(self.GridList1[point.index], "gezi3")
                UH.SpriteName(self.GridList1[point1.index], "gezi3")
                UH.SpriteName(self.GridList1[point2.index], "gezi3")
                table.insert(self.pre_grid, self.GridList1[point.index])
                table.insert(self.pre_grid, self.GridList1[point1.index])
                table.insert(self.pre_grid, self.GridList1[point2.index])
                point.dir = 3
                return point, 3
            end
        end
    end
    return nil
end
function JoySeeksFavorsPanel:Grid3All(point)
    self.cache3_list1 = {}
    self.cache3_list2 = {}
    local myself_map = self.data:MySelfMap()
    if next(myself_map) == nil then
        return
    end
    local player_joy_map = myself_map.player_joy_map
    --用于计算格子索引
    local x = point.x
    local y = point.y
    --point坐标是0开始 转换为数据索引
    local x1 = point.x + 1
    local y1 = point.y + 1
    --中间位置就是 只有两个方向
    if self.is_click_middle == true then --中间点
        --当前位置是否可以组成|——
        if player_joy_map[x1 - 1] and player_joy_map[x1 + 1] then --|
            if player_joy_map[x1 - 1][y1].joy_seq == 0 and player_joy_map[x1 + 1][y1].joy_seq == 0 then
                local index = self:CalcIndex(x - 1, y)
                local index2 = self:CalcIndex(x + 1, y)
                local point1 = self:CreatePoint(player_joy_map[x1 - 1][y1], x - 1, y, index)
                local point2 = self:CreatePoint(player_joy_map[x1 + 1][y1], x + 1, y, index2)
                point1.dir = 2
                self.cache3_list1[2] = {point1, point, point2}
            end
        end
        if player_joy_map[x1][y1 - 1] and player_joy_map[x1][y1 + 1] then --——
            if player_joy_map[x1][y1 - 1].joy_seq == 0 and player_joy_map[x1][y1 + 1].joy_seq == 0 then
                local index = self:CalcIndex(x, y - 1)
                local index2 = self:CalcIndex(x, y + 1)
                local point1 = self:CreatePoint(player_joy_map[x1][y1 - 1], x, y - 1, index)
                local point2 = self:CreatePoint(player_joy_map[x1][y1 + 1], x, y + 1, index2)
                point1.dir = 1
                self.cache3_list1[1] = {point1, point, point2}
            end
        end
    else
        --根据该点进行上下左右计算
        if player_joy_map[x1 - 1] and player_joy_map[x1 - 2] then
            if player_joy_map[x1 - 1][y1].joy_seq == 0 and player_joy_map[x1 - 2][y1].joy_seq == 0 then
                local index = self:CalcIndex(x - 1, y)
                local index2 = self:CalcIndex(x - 2, y)
                local point1 = self:CreatePoint(player_joy_map[x1 - 1][y1], x - 1, y, index)
                local point2 = self:CreatePoint(player_joy_map[x1 - 2][y1], x - 2, y, index2)
                point1.dir = 0
                self.cache3_list2[0] = {point1, point, point2}
            end
        end
        if player_joy_map[x1][y1 + 1] and player_joy_map[x1][y1 + 2] then
            if player_joy_map[x1][y1 + 1].joy_seq == 0 and player_joy_map[x1][y1 + 2].joy_seq == 0 then
                local index = self:CalcIndex(x, y + 1)
                local index2 = self:CalcIndex(x, y + 2)
                local point1 = self:CreatePoint(player_joy_map[x1][y1 + 1], x, y + 1, index)
                local point2 = self:CreatePoint(player_joy_map[x1][y1 + 2], x, y + 2, index2)
                point1.dir = 1
                self.cache3_list2[1] = {point1, point, point2}
            end
        end
        if player_joy_map[x1 + 1] and player_joy_map[x1 + 2] then
            if player_joy_map[x1 + 1][y1].joy_seq == 0 and player_joy_map[x1 + 2][y1].joy_seq == 0 then
                local index = self:CalcIndex(x + 1, y)
                local index2 = self:CalcIndex(x + 2, y)
                local point1 = self:CreatePoint(player_joy_map[x1 + 1][y1], x + 1, y, index)
                local point2 = self:CreatePoint(player_joy_map[x1 + 2][y1], x + 2, y, index2)
                point1.dir = 2
                self.cache3_list2[2] = {point1, point, point2}
            end
        end
        if player_joy_map[x1][y1 - 1] and player_joy_map[x1][y1 - 2] then
            if player_joy_map[x1][y1 - 1].joy_seq == 0 and player_joy_map[x1][y1 - 2].joy_seq == 0 then
                local index = self:CalcIndex(x, y - 1)
                local index2 = self:CalcIndex(x, y - 2)
                local point1 = self:CreatePoint(player_joy_map[x1][y1 - 1], x, y - 1, index)
                local point2 = self:CreatePoint(player_joy_map[x1][y1 - 2], x, y - 2, index2)
                point1.dir = 3
                self.cache3_list2[3] = {point1, point, point2}
            end
        end
    end
end
function JoySeeksFavorsPanel:CreatePoint(point, x, y, index)
    local new_point = point
    new_point.x = x
    new_point.y = y
    new_point.index = index
    return new_point
end
function JoySeeksFavorsPanel:CalcIndex(x, y)
    return (x * 5) + (y + 1)
end
function JoySeeksFavorsPanel:FlushMenu(point, enemy)
    local pos
    if enemy == true then
        -- self.MenuList[2]:SetActive(self.use_item == false)
        pos = self.ClickList2[point.index].anchoredPosition
    else
        pos = self.ClickList1[point.index].anchoredPosition
        self.MenuList[2]:SetActive(point.joy_seq ~= 1)
    end
    self.menu_pos:Set(pos.x, pos.y + 75)
    UH.AnchoredPosition(self.MenuObj, self.menu_pos)
    
    local match = self.data:ViewData().is_match
    if MATCH_TYPE.SETTING ~= match then 
        self.MenuObj.gameObject:SetActive(true)
    end 
    --操作判断如果点了取消
end
function JoySeeksFavorsPanel:OnClickGrid2(x, y)
    local match = self.data:ViewData().is_match
    local opera = self.data:ViewData().opera
    if MATCH_TYPE.SHOW == math then
        PublicPopupCtrl.Instance:Center("展示阶段")
    end
    if MATCH_TYPE.OPERATION == match and opera == false then
        PublicPopupCtrl.Instance:Center("轮到对方")
    end
    if MATCH_TYPE.OPERATION == match and opera == true then
        --判断是否用了耙子
        if self.use_item then
            self.is_click_middle = true
            local enemy_map = self.data:EnemyMap()
            local player_joy_map = enemy_map.player_joy_map
            local index = self:CalcIndex(x, y)
            local point = player_joy_map[x + 1][y + 1]
            point = self:CreatePoint(point, x, y, index)
            --判断原点是否符合挖掘特点
            if point.joy_seq == 0 then
                self:ClearPre()
                local point2, dir = self:Grid32(point)
                --这个点就是中间点
                self:Grid32All(point)
                if point2 == nil then
                    return
                end
                self.change_point = point2
                --得到了准确的点
                self.cache_point = point
                self:FlushMenu(point, true)
                local pos = self.ClickList2[point.index].anchoredPosition
                self.item_pos:Set(pos.x + 25, pos.y + 25)
                UH.AnchoredPosition(self.Item2, self.item_pos)
                self.Item2.gameObject:SetActive(true)
                self:HideItem()
            else
                self:ClearPre()
            end
        else
            local enemy_map = self.data:EnemyMap()
            local player_joy_map = enemy_map.player_joy_map
            local index = self:CalcIndex(x, y)
            local point = player_joy_map[x + 1][y + 1]
            point = self:CreatePoint(point, x, y, index)
            if point.is_open == 0 then
                local pos = self.ClickList2[point.index].anchoredPosition
                self.item_pos:Set(pos.x + 25, pos.y + 25)
                UH.AnchoredPosition(self.Item1, self.item_pos)
                self.Item1.gameObject:SetActive(true)

                self.ctrl:Open(x, y)
                -- self.data:ViewData().opera = false
                self:HideItem()
            else
                PublicPopupCtrl.Instance:Center("已经挖过了")
            end
        end
    end
end
function JoySeeksFavorsPanel:HideItem()
    TimeHelper:CancelTimer(self.handle_delay)
    self.handle_delay =
        TimeHelper:AddDelayTimer(
        function()
            self.Item1.gameObject:SetActive(false)
            self.Item2.gameObject:SetActive(false)
        end,
        1
    )
end
function JoySeeksFavorsPanel:Grid32(point)
    --当前位置处于什么位置
    local myself_map = self.data:EnemyMap()
    if next(myself_map) == nil then
        return
    end
    local player_joy_map = myself_map.player_joy_map
    --用于计算格子索引
    local x = point.x
    local y = point.y
    --point坐标是0开始 转换为数据索引
    local x1 = point.x + 1
    local y1 = point.y + 1
    --中间位置就是 只有两个方向
    if self.is_click_middle then
        --当前位置是否可以组成|——
        if player_joy_map[x1 - 1] and player_joy_map[x1 + 1] then --|
            if player_joy_map[x1 - 1][y1].joy_seq == 0 and player_joy_map[x1 + 1][y1].joy_seq == 0 then
                local index = self:CalcIndex(x - 1, y)
                local index2 = self:CalcIndex(x + 1, y)
                local point1 = self:CreatePoint(player_joy_map[x1 - 1][y1], x - 1, y, index)
                local point2 = self:CreatePoint(player_joy_map[x1 + 1][y1], x + 1, y, index2)
                UH.SpriteName(self.GridList2[point.index], "gezi3")
                UH.SpriteName(self.GridList2[point1.index], "gezi3")
                UH.SpriteName(self.GridList2[point2.index], "gezi3")
                table.insert(self.pre_grid, self.GridList2[point.index])
                table.insert(self.pre_grid, self.GridList2[point1.index])
                table.insert(self.pre_grid, self.GridList2[point2.index])
                point1.dir = 2
                return point1, 2
            end
        end
        if player_joy_map[x1][y1 - 1] and player_joy_map[x1][y1 + 1] then --——
            if player_joy_map[x1][y1 - 1].joy_seq == 0 and player_joy_map[x1][y1 + 1].joy_seq == 0 then
                local index = self:CalcIndex(x, y - 1)
                local index2 = self:CalcIndex(x, y + 1)
                local point1 = self:CreatePoint(player_joy_map[x1][y1 - 1], x, y - 1, index)
                local point2 = self:CreatePoint(player_joy_map[x1][y1 + 1], x, y + 1, index2)
                UH.SpriteName(self.GridList2[point.index], "gezi3")
                UH.SpriteName(self.GridList2[point1.index], "gezi3")
                UH.SpriteName(self.GridList2[point2.index], "gezi3")
                table.insert(self.pre_grid, self.GridList2[point.index])
                table.insert(self.pre_grid, self.GridList2[point1.index])
                table.insert(self.pre_grid, self.GridList2[point2.index])
                point1.dir = 1
                return point1, 1
            end
        end
    else
        --根据该点进行上下左右计算
        --[[ if player_joy_map[x1 - 1] and player_joy_map[x1 - 2] then
            if player_joy_map[x1 - 1][y1].joy_seq == 0 and player_joy_map[x1 - 2][y1].joy_seq == 0 then
                local index = self:CalcIndex(x - 1, y)
                local index2 = self:CalcIndex(x - 2, y)
                local point1 = self:CreatePoint(player_joy_map[x1 - 1][y1], x - 1, y, index)
                local point2 = self:CreatePoint(player_joy_map[x1 - 2][y1], x - 2, y, index2)
                UH.SpriteName(self.GridList1[point.index], "gezi3")
                UH.SpriteName(self.GridList1[point1.index], "gezi3")
                UH.SpriteName(self.GridList1[point2.index], "gezi3")
                table.insert(self.pre_grid, self.GridList1[point.index])
                table.insert(self.pre_grid, self.GridList1[point1.index])
                table.insert(self.pre_grid, self.GridList1[point2.index])
                point.dir = 0
                return point, 0
            end
        end
        if player_joy_map[x1][y1 + 1] and player_joy_map[x1][y1 + 2] then
            if player_joy_map[x1][y1 + 1].joy_seq == 0 and player_joy_map[x1][y1 + 2].joy_seq == 0 then
                local index = self:CalcIndex(x, y + 1)
                local index2 = self:CalcIndex(x, y + 2)
                local point1 = self:CreatePoint(player_joy_map[x1][y1 + 1], x, y + 1, index)
                local point2 = self:CreatePoint(player_joy_map[x1][y1 + 2], x, y + 2, index2)
                UH.SpriteName(self.GridList1[point.index], "gezi3")
                UH.SpriteName(self.GridList1[point1.index], "gezi3")
                UH.SpriteName(self.GridList1[point2.index], "gezi3")
                table.insert(self.pre_grid, self.GridList1[point.index])
                table.insert(self.pre_grid, self.GridList1[point1.index])
                table.insert(self.pre_grid, self.GridList1[point2.index])
                point.dir = 1
                return point, 1
            end
        end
        if player_joy_map[x1 + 1] and player_joy_map[x1 + 2] then
            if player_joy_map[x1 + 1][y1].joy_seq == 0 and player_joy_map[x1 + 2][y1].joy_seq == 0 then
                local index = self:CalcIndex(x + 1, y)
                local index2 = self:CalcIndex(x + 2, y)
                local point1 = self:CreatePoint(player_joy_map[x1 + 1][y1], x + 1, y, index)
                local point2 = self:CreatePoint(player_joy_map[x1 + 2][y1], x + 2, y, index2)
                UH.SpriteName(self.GridList1[point.index], "gezi3")
                UH.SpriteName(self.GridList1[point1.index], "gezi3")
                UH.SpriteName(self.GridList1[point2.index], "gezi3")
                table.insert(self.pre_grid, self.GridList1[point.index])
                table.insert(self.pre_grid, self.GridList1[point1.index])
                table.insert(self.pre_grid, self.GridList1[point2.index])
                point.dir = 2
                return point, 2
            end
        end
        if player_joy_map[x1][y1 - 1] and player_joy_map[x1][y1 - 2] then
            if player_joy_map[x1][y1 - 1].joy_seq == 0 and player_joy_map[x1][y1 - 2].joy_seq == 0 then
                local index = self:CalcIndex(x, y - 1)
                local index2 = self:CalcIndex(x, y - 2)
                local point1 = self:CreatePoint(player_joy_map[x1][y1 - 1], x, y - 1, index)
                local point2 = self:CreatePoint(player_joy_map[x1][y1 - 2], x, y - 2, index2)
                UH.SpriteName(self.GridList1[point.index], "gezi3")
                UH.SpriteName(self.GridList1[point1.index], "gezi3")
                UH.SpriteName(self.GridList1[point2.index], "gezi3")
                table.insert(self.pre_grid, self.GridList1[point.index])
                table.insert(self.pre_grid, self.GridList1[point1.index])
                table.insert(self.pre_grid, self.GridList1[point2.index])
                point.dir = 3
                return point, 3
            end
        end ]]
    end
    return nil
end

function JoySeeksFavorsPanel:Grid32All(point)
    self.cache3_list1 = {}
    self.cache3_list2 = {}
    local myself_map = self.data:EnemyMap()
    if next(myself_map) == nil then
        return
    end
    local player_joy_map = myself_map.player_joy_map
    --用于计算格子索引
    local x = point.x
    local y = point.y
    --point坐标是0开始 转换为数据索引
    local x1 = point.x + 1
    local y1 = point.y + 1
    --中间位置就是 只有两个方向
    if self.is_click_middle == true then --中间点
        --当前位置是否可以组成|——
        if player_joy_map[x1 - 1] and player_joy_map[x1 + 1] then --|
            if player_joy_map[x1 - 1][y1].joy_seq == 0 and player_joy_map[x1 + 1][y1].joy_seq == 0 then
                local index = self:CalcIndex(x - 1, y)
                local index2 = self:CalcIndex(x + 1, y)
                local point1 = self:CreatePoint(player_joy_map[x1 - 1][y1], x - 1, y, index)
                local point2 = self:CreatePoint(player_joy_map[x1 + 1][y1], x + 1, y, index2)
                point1.dir = 2
                self.cache3_list1[2] = {point1, point, point2}
            end
        end
        if player_joy_map[x1][y1 - 1] and player_joy_map[x1][y1 + 1] then --——
            if player_joy_map[x1][y1 - 1].joy_seq == 0 and player_joy_map[x1][y1 + 1].joy_seq == 0 then
                local index = self:CalcIndex(x, y - 1)
                local index2 = self:CalcIndex(x, y + 1)
                local point1 = self:CreatePoint(player_joy_map[x1][y1 - 1], x, y - 1, index)
                local point2 = self:CreatePoint(player_joy_map[x1][y1 + 1], x, y + 1, index2)
                point1.dir = 1
                self.cache3_list1[1] = {point1, point, point2}
            end
        end
    --[[ else ]]
    --[[ --根据该点进行上下左右计算
        if player_joy_map[x1 - 1] and player_joy_map[x1 - 2] then
            if player_joy_map[x1 - 1][y1].joy_seq == 0 and player_joy_map[x1 - 2][y1].joy_seq == 0 then
                local index = self:CalcIndex(x - 1, y)
                local index2 = self:CalcIndex(x - 2, y)
                local point1 = self:CreatePoint(player_joy_map[x1 - 1][y1], x - 1, y, index)
                local point2 = self:CreatePoint(player_joy_map[x1 - 2][y1], x - 2, y, index2)
                point1.dir = 0
                self.cache3_list2[0] = {point1, point, point2}
            end
        end
        if player_joy_map[x1][y1 + 1] and player_joy_map[x1][y1 + 2] then
            if player_joy_map[x1][y1 + 1].joy_seq == 0 and player_joy_map[x1][y1 + 2].joy_seq == 0 then
                local index = self:CalcIndex(x, y + 1)
                local index2 = self:CalcIndex(x, y + 2)
                local point1 = self:CreatePoint(player_joy_map[x1][y1 + 1], x, y + 1, index)
                local point2 = self:CreatePoint(player_joy_map[x1][y1 + 2], x, y + 2, index2)
                point1.dir = 1
                self.cache3_list2[1] = {point1, point, point2}
            end
        end
        if player_joy_map[x1 + 1] and player_joy_map[x1 + 2] then
            if player_joy_map[x1 + 1][y1].joy_seq == 0 and player_joy_map[x1 + 2][y1].joy_seq == 0 then
                local index = self:CalcIndex(x + 1, y)
                local index2 = self:CalcIndex(x + 2, y)
                local point1 = self:CreatePoint(player_joy_map[x1 + 1][y1], x + 1, y, index)
                local point2 = self:CreatePoint(player_joy_map[x1 + 2][y1], x + 2, y, index2)
                point1.dir = 2
                self.cache3_list2[2] = {point1, point, point2}
            end
        end
        if player_joy_map[x1][y1 - 1] and player_joy_map[x1][y1 - 2] then
            if player_joy_map[x1][y1 - 1].joy_seq == 0 and player_joy_map[x1][y1 - 2].joy_seq == 0 then
                local index = self:CalcIndex(x, y - 1)
                local index2 = self:CalcIndex(x, y - 2)
                local point1 = self:CreatePoint(player_joy_map[x1][y1 - 1], x, y - 1, index)
                local point2 = self:CreatePoint(player_joy_map[x1][y1 - 2], x, y - 2, index2)
                point1.dir = 3
                self.cache3_list2[3] = {point1, point, point2}
            end
        end ]]
    end
end

function JoySeeksFavorsPanel:OnClickItem()
end
function JoySeeksFavorsPanel:OnClickCancel()
    self:ClearPre()
    --清除所有预览
    -- self:FlushMySelfMap()
    ---------------
    self.change_dir = nil
    ---------------
    self.origin_point = nil
    self.change_point = nil
    self.origin_point_list = {}
    ---------------
    self.MenuObj.gameObject:SetActive(false)
end
function JoySeeksFavorsPanel:OnClickChange()
    if self.use_item == true then
        self:UseChange()
    else
        self:SetChange()
    end
    self.MenuObj.gameObject:SetActive(true)
end
function JoySeeksFavorsPanel:UseChange()
    local point2, new_dir = self:Grid32(self.cache_point)
    if self.change_point.dir == nil then
        self.change_point = point2
    end
    if point2 ~= nil then
        if self.is_click_middle == true then
            local list = self.cache3_list1[self.change_point.dir == 2 and 1 or 2]
            --如果存在就转个方向
            if list then
                self:ClearPre()
                for i = 1, 3 do
                    UH.SpriteName(self.GridList2[list[i].index], "gezi3")
                    table.insert(self.pre_grid, self.GridList2[list[i].index])
                end
                self.change_point = list[1]
            end
        --[[ else ]]
        --[[ if point2 ~= nil then
                    if self.change_dir == nil then
                        self.change_dir = new_dir
                    end
                    if self.change_dir > 3 then
                        --转回初始方向
                        self.change_dir = new_dir - 1
                    elseif self.change_dir < 0 then
                        self.change_dir = 0
                    end
                    self.change_dir = self.change_dir + 1
                    if self.change_dir ~= new_dir then
                        local list = self.cache3_list2[self.change_dir]
                        if list == nil then
                            self:OnClickChange()
                            return
                        else
                            new_dir = self.change_dir
                        end
                    else
                        self.change_dir = new_dir
                    end
                    local list = self.cache3_list2[new_dir]
                    point2.dir = new_dir
                    self.change_point = point2
                    self:ClearPre()
                    for i = 1, 3 do
                        UH.SpriteName(self.GridList1[list[i].index], "gezi3")
                        table.insert(self.pre_grid, self.GridList1[list[i].index])
                    end
                end ]]
        end
    end
end
function JoySeeksFavorsPanel:SetChange()
    --只有2和3能够旋转 使用道具时能旋转
    if self.origin_point.joy_seq == 2 then
        --方向如果=3那么就已经是最后一种选择了没得选了。
        local point2, new_dir = self:Grid2(self.change_point)
        --二次点击点
        --跳过某个方向
        if point2 ~= nil then
            if self.change_dir == nil then
                self.change_dir = new_dir
            end
            if self.change_dir > 3 then
                --转回初始方向
                self.change_dir = new_dir - 1
            elseif self.change_dir < 0 then
                self.change_dir = 0
            end
            self.change_dir = self.change_dir + 1
            if self.change_dir ~= new_dir then
                point2 = self.cache2_list[self.change_dir]
                --这里获取不同方向的方案
                if point2 == nil then
                    self:OnClickChange()
                    return
                else
                    new_dir = self.change_dir
                end
            else
                self.change_dir = new_dir
            end
            point2.dir = new_dir
            self.change_point.dir = new_dir
            self:ClearPre()
            local index = self.change_point.index
            UH.SpriteName(self.GridList1[index], "gezi3")
            UH.SpriteName(self.GridList1[point2.index], "gezi3")
            table.insert(self.pre_grid, self.GridList1[index])
            table.insert(self.pre_grid, self.GridList1[point2.index])
        end
    elseif self.origin_point.joy_seq == 3 then
        local point2, new_dir = self:Grid3(self.cache_point)
        if self.change_point.dir == nil then
            self.change_point = point2
        end
        if point2 ~= nil then
            if self.is_click_middle == true then
                local list = self.cache3_list1[self.change_point.dir == 2 and 1 or 2]
                --如果存在就转个方向
                if list then
                    self:ClearPre()
                    for i = 1, 3 do
                        UH.SpriteName(self.GridList1[list[i].index], "gezi3")
                        table.insert(self.pre_grid, self.GridList1[list[i].index])
                    end
                    self.change_point = list[1]
                end
                return
            else
                if point2 ~= nil then
                    if self.change_dir == nil then
                        self.change_dir = new_dir
                    end
                    if self.change_dir > 3 then
                        --转回初始方向
                        self.change_dir = new_dir - 1
                    elseif self.change_dir < 0 then
                        self.change_dir = 0
                    end
                    self.change_dir = self.change_dir + 1
                    if self.change_dir ~= new_dir then
                        local list = self.cache3_list2[self.change_dir]
                        if list == nil then
                            self:OnClickChange()
                            return
                        else
                            new_dir = self.change_dir
                        end
                    else
                        self.change_dir = new_dir
                    end
                    local list = self.cache3_list2[new_dir]
                    point2.dir = new_dir
                    self.change_point = point2
                    self:ClearPre()
                    for i = 1, 3 do
                        UH.SpriteName(self.GridList1[list[i].index], "gezi3")
                        table.insert(self.pre_grid, self.GridList1[list[i].index])
                    end
                end
            end
        end
    end
end
function JoySeeksFavorsPanel:OnClickOK()
    --清理预览 发送协议
    self:ClearPre()
    if self.use_item == true then
        self.ctrl:Item(self.change_point.x, self.change_point.y, self.change_point.dir)
    else
        --如果没有就发协议 关菜单
        if self.origin_point.joy_seq == 1 then
            self.ctrl:Set(self.change_point.x, self.change_point.y, 0, self.origin_point.joy_seq)
        else
            if self.change_point.dir then --无方向保持原状
                self.ctrl:Set(
                    self.change_point.x,
                    self.change_point.y,
                    self.change_point.dir,
                    self.origin_point.joy_seq
                )
            end
        end
    end
    self.change_dir = nil
    ---------------
    self.origin_point = nil
    self.change_point = nil
    self.origin_point_list = {}
    ---------------
    self.MenuObj.gameObject:SetActive(false)
end

function JoySeeksFavorsPanel:ClearPre()
    for _, value in pairs(self.pre_grid) do
        UH.SpriteName(value, "gezi")
    end
    self.pre_grid = {}
    self.MenuObj.gameObject:SetActive(false)
end
--铲子
--[[ function JoySeeksFavorsPanel:OnClickUseItem2()
    --取消是用物品的状态
    self.Select1:SetActive(false)
    self.Select2:SetActive(true)
    self.use_item = false
end ]]
--耙子
function JoySeeksFavorsPanel:OnClickUseItem()
    local map = self.data:EnemyMap()
    if next(map) == nil then
        return
    end
    if map.role_info.find_item > 0 then
        PublicPopupCtrl.Instance:Center("每场比赛只能使用一次")
        return
    end
    --只能使用一次
    self.SelectItem:SetActive(not self.SelectItem.activeSelf)
    self.use_item = self.SelectItem.activeSelf
    --[[ self.Select2:SetActive(false) ]]
end
--[[ 
function JoySeeksFavorsPanel:OnClickChat()
    ViewMgr:OpenView(ChatView)
end
function JoySeeksFavorsPanel:OnClickSo()
    ViewMgr:OpenView(SocietyView)
end
 ]]
