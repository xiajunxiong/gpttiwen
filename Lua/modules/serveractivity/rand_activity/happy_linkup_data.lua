----------------------------HappyLinkupData----------------------------
HappyLinkupData = HappyLinkupData or BaseClass()
HappyLinkupData.GridSize = {
    [30] = {col = 6,row = 5},
    [40] = {col = 8,row = 5},
    [50] = {col = 10,row = 5},
    [60] = {col = 10,row = 6},
}
HappyLinkupData.LinkSize = {
    [30] = {col = 8,row = 7},
    [40] = {col = 10,row = 7},
    [50] = {col = 12,row = 7},
    [60] = {col = 12,row = 8},
}
HappyLinkupData.N_Dir = {
    [1] = 3,
    [2] = 4,
    [3] = 1,
    [4] = 2,
}
HappyLinkupData.test_log = {
    type = {
        [1] = "1.同向外圈检查",
        [2] = "2.内圈直接连接检查",
        [3] = "3.内圈延伸相交检查",
        [4] = "4.内圈同向折线，反向折线检查",
    }
}

function HappyLinkupData:__init()
	if HappyLinkupData.Instance ~= nil then
		Debuger.LogError("[HappyLinkupData] attempt to create singleton twice!")
		return
	end
    HappyLinkupData.Instance = self


    -- 递归过程中的静态数据
    self.link_cacahe = {start_pos = nil,end_pos = nil,way_list = nil}

    self.game_master = {status = 0,record = {}}
end

function HappyLinkupData:ClearCache()
    self.link_cacahe = {start_pos = nil,end_pos = nil,way_list = nil}
end 

function HappyLinkupData:GetCurGameStatus()
    return self.game_master.status
end

function HappyLinkupData:IsGamePlaying()
    return self.game_master.status == 1
end

function HappyLinkupData:IsRewardWait()
    return self.game_master.status == 2
end

function HappyLinkupData:IsGameWait()
    return self.game_master.status == 0
end

-- function HappyLinkupData:MarkGameComplete(seq)
--     if seq == nil then return 
--     for k,v in pairs() do 

--     end 
-- end

-- function HappyLinkupData:StopGame()
--     self.game_master.status = 0
-- end

-- function HappyLinkupData:StartGame()
--     self.game_master.status = 1
-- end

function HappyLinkupData:ReadyMark(size)
    local mark = {}
    local icon = 1
    for j = 1,2 do 
        for i = 1,size /2 do 
            if icon > 10 then icon = 1 end 
            table.insert(mark,icon)
            icon = icon + 1
        end  
        icon = 1
    end 

    return mark
end

function HappyLinkupData:MatchGridData(row,col)
    local data = {}

    local size = row * col
    local marks = self:ReadyMark(size)

    local index = 0
    for i = 1, row do
        for j = 1,col do 
            index = index + 1
            local vo = {}
            local mark_pos = GameMath.Ramdon(1,#marks) 
            vo.icon = marks[mark_pos]
            vo.row = i
            vo.col = j
            vo.index = index
            vo.disapper = false
            vo.pre = false
            -- 1 左边 2 上边 3 右边 4 下边
            -- vo.is_out = i == 1 and 1 or (j == 1 and 2 or (i == col and 3 or (j == row and 4 or 0)) )) -- or j == 1 or i == col or j == row
            -- vo.is_corn = self:LinkCellIsOutCorn(j,i,col,row)
            table.insert(data,vo)
            table.remove( marks, mark_pos)
        end 
    end 

    return data
end

function HappyLinkupData:PreViewGridData(row,col)
    local data = {}
    local size = row * col
    for i = 1, row do
        for j = 1,col do 
            local vo = {}
            vo.row = i
            vo.col = j
            vo.is_pre = true
            table.insert(data,vo)
        end 
    end 

    return data
end

function HappyLinkupData:MatchLinkData(type)
    local grid_size = HappyLinkupData.GridSize[type]
    local link_size = HappyLinkupData.LinkSize[type]
    local data = {}
    for i = 0,link_size.row -1 do 
        for j = 0,link_size.col -1 do 
            local vo = {}
            vo.col = j
            vo.row = i
            vo.t_row = link_size.row -1
            vo.t_col = link_size.col -1
            vo.is_out = i == 0 or j == 0 or i == link_size.row-1 or j == link_size.col -1
            vo.disapper = vo.is_out
            table.insert(data,vo)
        end 
    end 
    return data
end 

function HappyLinkupData:LinkCellIsOutCorn(col,row,t_col,t_row)
    -- 左上角
    if col == 1 and row == 1 then 
        return 1
    end 
    -- 右上角
    if col == t_col and row == 1 then 
        return 2
    end 
    -- 右下角
    if col == t_col and row == t_row then 
        return 3
    end 
    -- 左下角
    if col == 1 and row == t_row then 
        return 4
    end

    -- 都不是
    return 0
end

function HappyLinkupData:FlushGamePlayMatch(matching,linking,group)
    local success,line_way = self:LinkGamePlayCheck(matching,linking,group)
    if success then 
        matching[group[1].index].disapper = true
        matching[group[2].index].disapper = true 

        local is_gameover = self:CheckIsGameFinish(matching)
        local change_link = self:LinkGridMatch(linking,line_way,group)
        return matching, change_link,is_gameover
    else 
        matching[group[1].index].fail = true
        matching[group[2].index].fail = true 
    end 

    for k,v in pairs(linking) do
        v.link_info = nil
    end 
    return matching,linking,false
end

function HappyLinkupData:LinkGridMatch(linking,way_list,group)
    for k,v in pairs(linking) do
        v.link_info = nil
        for z = 1,2 do 
            if self:IsSameCell(group[z],v) then 
                v.disapper = true
            end 
        end 
        for i,j in pairs(way_list) do 
            if self:IsSameCell(v,j) then 
                v.link_info = j
            end 
        end 
    end 

    self:ClearCache()
    return linking
end 

function HappyLinkupData:CheckIsGameFinish(matching)
    local is_game_over = true
    for k,v in pairs(matching) do 
        -- 若不存在非消失状态的格子则为游戏胜利
        if not v.disapper then 
            is_game_over = false
        end 
    end 
    return is_game_over
end

function HappyLinkupData:LinkGamePlayCheck(matching,linking,group)
    if group[1].icon ~= group[2].icon then 
        return false,nil
    end 

    local init_link = {}
    local size = HappyLinkupData.LinkSize[#matching]
    local real_size = HappyLinkupData.GridSize[#matching]
    for i = 0,size.col - 1 do 
        if init_link[i] == nil then init_link[i] = {} end 
        for j = 0,size.row - 1 do 
            for k, v in pairs(linking) do 
                if i == v.col and j == v.row then 
                    init_link[i][j] = v
                    break
                end 
            end 
        end 
    end 

    local way_list = {}

    -- 最大伸展
    local max_extend_1 = {}
    local max_extend_2 = {}

    -- 1 左边 2 上边 3 右边 4 下边
    -- 初始化，把自己算进去
    for i = 1,4 do 
        max_extend_1[i] = group[1]
        max_extend_2[i] = group[2]
    end 
    -- 横向伸展
    for i = group[1].col,size.col-1 do
        if init_link[i][group[1].row].disapper then 
            max_extend_1[3] = init_link[i][group[1].row]
        elseif not self:IsSameCell(group[1],init_link[i][group[1].row]) then 
            break
        end 
    end 

    for i = group[1].col,0,-1 do
        if init_link[i][group[1].row].disapper then 
            max_extend_1[1] = init_link[i][group[1].row]
        elseif not self:IsSameCell(group[1],init_link[i][group[1].row]) then 
            break
        end 
    end 

    for i = group[2].col,size.col-1 do
        if init_link[i][group[2].row].disapper then 
            max_extend_2[3] = init_link[i][group[2].row]
        elseif not self:IsSameCell(group[2],init_link[i][group[2].row]) then 
            break
        end 
    end 

    for i = group[2].col,0,-1 do
        if init_link[i][group[2].row].disapper then 
            max_extend_2[1] = init_link[i][group[2].row]
        elseif not self:IsSameCell(group[2],init_link[i][group[2].row]) then 
            break
        end 
    end 

    -- 纵向伸展
    for i = group[1].row,size.row-1 do
        if init_link[group[1].col][i].disapper then 
            max_extend_1[4] = init_link[group[1].col][i]
        elseif not self:IsSameCell(group[1],init_link[group[1].col][i]) then 
            break
        end 
    end 

    for i = group[1].row,0,-1 do
        if init_link[group[1].col][i].disapper then 
            max_extend_1[2] = init_link[group[1].col][i]
        elseif not self:IsSameCell(group[1],init_link[group[1].col][i]) then  
            break
        end 
    end 

    for i = group[2].row,size.row-1 do
        if init_link[group[2].col][i].disapper then 
            max_extend_2[4] = init_link[group[2].col][i]
        elseif not self:IsSameCell(group[2],init_link[group[2].col][i]) then  
            break
        end 
    end 

    for i = group[2].row,0,-1 do
        if init_link[group[2].col][i].disapper then 
            max_extend_2[2] = init_link[group[2].col][i]
        elseif not self:IsSameCell(group[2],init_link[group[2].col][i]) then 
            break
        end 
    end 

    -- 同向外圈检查
    -- 外圈检查只存在同向
    -- 外圈已经用完规则的全部弯折了
    for step = 1,4 do 
        if max_extend_1[step].is_out and max_extend_2[step].is_out then 
            local way = {cell_list = {},step = 0,group = {}}
            if step == 1 then 
                for i = 1,2 do 
                    for j = group[i].col,0,-1 do
                        table.insert( way.cell_list, init_link[j][group[i].row])
                        way.step = way.step + 1
                    end 
                end 

                for i = math.min(group[1].row,group[2].row) + 1,math.max(group[1].row,group[2].row) - 1 do 
                    table.insert( way.cell_list, init_link[max_extend_1[step].col][i])
                    way.step = way.step + 1
                end 
                -- 去掉自己
                way.step = way.step - 2
                way.type = 1
                way.group = group
            end
            if step == 3 then 
                for i = 1,2 do 
                    for j = group[i].col,size.col-1 do
                        table.insert( way.cell_list, init_link[j][group[i].row])
                        way.step = way.step + 1
                    end 
                end 

                for i = math.min(group[1].row,group[2].row) + 1,math.max(group[1].row,group[2].row) - 1 do 
                    table.insert( way.cell_list, init_link[max_extend_1[step].col][i])
                    way.step = way.step + 1
                end                
                -- 去掉自己
                way.step = way.step - 2
                way.type = 1
                way.group = group
            end 
            if step == 2 then 
                for i =1,2 do 
                    for j = group[i].row,0,-1 do
                        table.insert( way.cell_list, init_link[group[i].col][j])
                        way.step = way.step + 1
                    end 
                end 

                for i = math.min(group[1].col,group[2].col) + 1,math.max(group[1].col,group[2].col) - 1 do 
                    table.insert( way.cell_list, init_link[i][max_extend_1[step].row])
                    way.step = way.step + 1
                end                 
                -- 去掉自己
                way.step = way.step - 2
                way.type = 1
                way.group = group
            end 
            if step == 4 then 
                for i = 1,2 do 
                    for j = group[i].row,size.row -1 do
                        table.insert( way.cell_list, init_link[group[i].col][j])
                        way.step = way.step + 1
                    end 
                end 

                for i = math.min(group[1].col,group[2].col) + 1,math.max(group[1].col,group[2].col) - 1 do 
                    table.insert( way.cell_list, init_link[i][max_extend_1[step].row])
                    way.step = way.step + 1
                end                 
                -- 去掉自己
                way.step = way.step - 2
                way.type = 1
                way.group = group
            end 

            table.insert(way_list,way)
        end 
    end 

    -- 内圈检查
    -- 直接连接检查
    -- 直连规则不需要讨论规则
    if group[1].row == group[2].row then 
        if self:GetStepLink(group[1],max_extend_2[1]) > 0 or self:GetStepLink(group[1],max_extend_2[3]) > 0 then 
            local way = {cell_list = {},step = 0}
            for i = math.min(group[1].col,group[2].col),math.max(group[1].col,group[2].col) do 
                table.insert( way.cell_list, init_link[i][group[1].row])
                way.step = way.step + 1
            end                  
            -- 去掉自己
            way.step = way.step - 2
            way.group = group
            way.type = 2
            table.insert(way_list,way)
        end 
    end 

    if group[1].col == group[2].col then 
        if self:GetStepLink(group[1],max_extend_2[2]) > 0 or self:GetStepLink(group[1],max_extend_2[4]) > 0 then 
            local way = {cell_list = {},step = 0}
            for i = math.min(group[1].row,group[2].row),math.max(group[1].row,group[2].row) do 
                table.insert( way.cell_list, init_link[group[1].col][i])
                way.step = way.step + 1
            end                  
            -- 去掉自己
            way.step = way.step - 2
            way.group = group
            way.type = 2
            table.insert(way_list,way)
        end 
    end 

    -- 折线问题
    -- 内圈相交折线
    for step = 1,4 do 
        if step == 1 and max_extend_2[step].col <= group[1].col and group[1].col <= group[2].col then 
            local way = {cell_list = {},step = 0}
            -- 2的左边延伸与1的上方延伸或下方延伸相交
            if (max_extend_1[2].row <= group[2].row and group[2].row < group[1].row) or
              (max_extend_1[4].row >= group[2].row and group[2].row > group[1].row) then 
                for i = math.min(group[1].row,group[2].row),math.max(group[1].row,group[2].row) do 
                    table.insert(way.cell_list,init_link[group[1].col][i])
                end 
                -- 防止相交点算两次
                for i = math.min(group[1].col,group[2].col),math.max(group[1].col,group[2].col)  do
                    table.insert(way.cell_list,init_link[i][group[2].row])
                end 
                way.group = group
                --去掉双方自己的格子
                way.step = #way.cell_list - 3
                way.type = 3
                table.insert(way_list,way)
            end 
        end 
        if step == 2 and max_extend_2[step].row <= group[1].row and group[1].row <= group[2].row then 
            local way = {cell_list = {},step = 0}
            -- 2的上边延伸与1的左边延伸或右边延伸相交
            if (max_extend_1[1].col <= group[2].col and group[2].col < group[1].col) 
               or ( max_extend_1[3].col >= group[2].col and group[2].col > group[1].col ) then 
                for i = math.min(group[1].col,group[2].col),math.max(group[1].col,group[2].col) do 
                    table.insert(way.cell_list,init_link[i][group[1].row])
                end 
                -- 防止相交点算两次
                for i = math.min(group[1].row,group[2].row),math.max(group[1].row,group[2].row)  do
                    table.insert(way.cell_list,init_link[group[2].col][i])
                end 
                way.group = group
                --去掉双方自己的格子
                way.step = #way.cell_list - 3
                way.type = 3
                table.insert(way_list,way)
            end 
        end 
        if step == 3 and max_extend_2[step].col >= group[1].col and group[1].col >= group[2].col then 
            local way = {cell_list = {},step = 0}
            -- 2的右边延伸与1的上方延伸或下方延伸相交
            if (max_extend_1[2].row <= group[2].row and group[2].row < group[1].row) or
              (max_extend_1[4].row >= group[2].row and group[2].row > group[1].row) then 
                for i = math.min(group[1].row,group[2].row),math.max(group[1].row,group[2].row) do 
                    table.insert(way.cell_list,init_link[group[1].col][i])
                end 
                -- 防止相交点算两次
                for i = math.min(group[1].col,group[2].col),math.max(group[1].col,group[2].col) do
                    table.insert(way.cell_list,init_link[i][group[2].row])
                end 
                way.group = group
                --去掉双方自己的格子
                way.step = #way.cell_list - 3
                way.type = 3
                table.insert(way_list,way)
            end 
        end 
        if step == 4 and max_extend_2[step].row >= group[1].row and group[1].row >= group[2].row then 
            local way = {cell_list = {},step = 0}
            -- 2的下边延伸与1的左边延伸或右边延伸相交
            if (max_extend_1[1].col <= group[2].col and group[2].col < group[1].col) 
               or ( max_extend_1[3].col >= group[2].col and group[2].col > group[1].col ) then 
                for i = math.min(group[1].col,group[2].col),math.max(group[1].col,group[2].col) do 
                    table.insert(way.cell_list,init_link[i][group[1].row])
                end 
                -- 防止相交点算两次
                for i = math.min(group[1].row,group[2].row),math.max(group[1].row,group[2].row) do
                    table.insert(way.cell_list,init_link[group[2].col][i])
                end 
                way.group = group
                --去掉双方自己的格子
                way.step = #way.cell_list - 3
                way.type = 3
                table.insert(way_list,way)
            end 
        end 
    end 

    -- 内圈同向折线
    -- 内圈反向折线
    for ex2,ex2_vo in pairs(max_extend_2) do 
        for ex1,ex1_vo in pairs(max_extend_1) do
            if (ex2 == 1 or ex2 == 3) and (ex1 == 1 or ex1 == 3) then 
                for step_move2 = math.min(group[2].col,ex2_vo.col),math.max(group[2].col,ex2_vo.col) do 
                    for step_move1 = math.min(group[1].col,ex1_vo.col),math.max(group[1].col,ex1_vo.col) do 
                        if step_move1 == step_move2 then
                            local link_success = true
                            for move = math.min(ex1_vo.row,ex2_vo.row),math.max(ex1_vo.row,ex2_vo.row) do 
                                if not init_link[step_move1][move].disapper then 
                                    link_success  =false
                                end 
                            end 
    
                            if link_success then 
                                local way = {cell_list = {},step = 0}
    
                                for i = math.min(group[1].col,step_move1),math.max(group[1].col,step_move1) do
                                    table.insert(way.cell_list,init_link[i][group[1].row])
                                end 
    
                                for i = math.min(group[2].col,step_move2),math.max(group[2].col,step_move2) do
                                    table.insert(way.cell_list,init_link[i][group[2].row])
                                end 
    
                                for i = math.min(ex1_vo.row,ex2_vo.row),math.max(ex1_vo.row,ex2_vo.row) do 
                                    table.insert(way.cell_list,init_link[step_move2][i])
                                end 
    
                                way.group = group
                                --去掉双方自己的格子
                                way.step = #way.cell_list - 4
                                way.type = 4
                                table.insert(way_list,way)
                                break
                            end 
                        end 
                    end
                end 
            end 
            if (ex2 == 2 or ex2 == 4) and (ex1 == 2 or ex1 == 4) then 
                for step_move2 = math.min(group[2].row,ex2_vo.row),math.max(group[2].row,ex2_vo.row) do 
                    for step_move1 = math.min(group[1].row,ex1_vo.row),math.max(group[1].row,ex1_vo.row) do 
                        if step_move1 == step_move2 then
                            local link_success = true
                            for move = math.min(ex1_vo.col,ex2_vo.col),math.max(ex1_vo.col,ex2_vo.col) do 
                                if not init_link[move][step_move1].disapper then 
                                    link_success  = false
                                end 
                            end 
        
                            if link_success then 
                                local way = {cell_list = {},step = 0}
        
                                for i = math.min(group[1].row,step_move1),math.max(group[1].row,step_move1) do
                                    table.insert(way.cell_list,init_link[group[1].col][i])
                                end 
        
                                for i = math.min(group[2].row,step_move2),math.max(group[2].row,step_move2) do
                                    table.insert(way.cell_list,init_link[group[2].col][i])
                                end 
        
                                for i = math.min(ex1_vo.col,ex2_vo.col),math.max(ex1_vo.col,ex2_vo.col) do 
                                    table.insert(way.cell_list,init_link[i][step_move2])
                                end 
        
                                way.group = group
                                --去掉双方自己的格子
                                way.step = #way.cell_list - 4
                                way.type = 4
                                table.insert(way_list,way)
                                break
                            end 
                        end 
                    end 
                end
            end 
        end 
    end 

    local result_way = nil
    local test_log = ""
    for k,v in pairs(way_list) do 
        local test_way = "拟路径"..k.." 步数 "..v.step.." 方案"..HappyLinkupData.test_log.type[v.type]
            .." 起点"..v.group[1].col.."-"..v.group[1].row.." 终点"..v.group[2].col.."-"..v.group[2].row.."\n"
        test_log = test_log .. test_way
        if ( result_way == nil or v.step < result_way.step ) and v.step >= 0 then 
            result_way = v
        end 
    end 

    -- LogError("路径检查完成：\n"..test_log)
    local pass_list = {}
    if result_way ~= nil then 
        -- LogError("最短路径方案",HappyLinkupData.test_log.type[result_way.type],"步数 ",result_way.step)
        self.link_cacahe.start_pos = group[1]
        self.link_cacahe.end_pos = group[2]
        self.link_cacahe.way_list = result_way.cell_list
    
        pass_list = self:FindNextCellLink(self.link_cacahe.start_pos,{})
    end 
    

    return result_way ~= nil, pass_list


end

-- 检查是否同一个格子 
-- 判断是否纵列是否一致
function HappyLinkupData:IsSameCell(cell_1,cell_2)
    if cell_1 == nil or cell_2 == nil then return false end
    return cell_1.row == cell_2.row and cell_1.col == cell_2.col
end 

function HappyLinkupData:AllIsSameCell(cell_1,list)
    for k,v in pairs(list) do 
        if self:IsSameCell(v,cell_1) then 
            return true
        end 
    end 
    return false
end 

-- 找该格子的附近的格子
function HappyLinkupData:GetPassLink(cell,list)
    for k,v in pairs(list) do 
        local dir = self:GetStepLink(v,cell)
        if dir > 0 then 
            return v,dir
        end 
    end 
    return nil,nil
end

-- 获取格子A相对格子B的方向
function HappyLinkupData:GetStepLink(cell_A,cell_B)

    if (cell_A.row == cell_B.row + 1 and cell_A.col == cell_B.col) then 
        return 4
    end 
    if (cell_A.row == cell_B.row - 1 and cell_A.col == cell_B.col) then
        return 2
    end
    if (cell_A.row == cell_B.row and cell_A.col == cell_B.col - 1) then
        return 1
    end
    if (cell_A.row == cell_B.row and cell_A.col == cell_B.col + 1) then 
        return 3
    end 
    return 0
end 

function HappyLinkupData:FindNextCellLink(c_cell,pass_list)
    -- 如果当前格子是终点，返回pass_list，结束递归
    if self:IsSameCell(c_cell,self.link_cacahe.end_pos) then 
        local vo = {}
        vo.row = c_cell.row
        vo.col = c_cell.col
        vo.disapper = c_cell.disapper
        vo.is_start = false
        vo.is_end = true
        table.insert(pass_list,vo)

        return pass_list
    end 

    local next_cell = nil
    local pass_cell,pass_dir = self:GetPassLink(c_cell,pass_list)

    local vo = {}
    vo.row = c_cell.row
    vo.col = c_cell.col
    vo.disapper = c_cell.disapper
    vo.is_start = pass_cell == nil
    vo.is_end = false

    -- 如果不是起点则记录前一个点的信息
    -- 并根据前一个点的情况获取连接类型

    vo.next_info = {row = 0,col = 0,is_out = false,disapper = false}

    for k,v in pairs(self.link_cacahe.way_list) do 
        local dir = self:GetStepLink(v,c_cell)
        -- 不是前一个格子
        if not self:IsSameCell(v,pass_cell) and dir > 0 then          
            vo.next_info = {
                row = v.row,
                col = v.col,
                is_out = v.is_out,
                disapper = v.disapper,
            }
            vo.dir = dir
            next_cell = v
            break
        end
    end 

    if not vo.is_start then 
        vo.pass_info = pass_cell
    end 

    table.insert(pass_list,vo)

    return self:FindNextCellLink(next_cell,pass_list)

end 

function HappyLinkupData:GetCurTimer()
    local data = ServerActivityData.Instance:GetHappyLookInfo()
    local s_time = Config.happy_look_auto.other[1].frequency

    return s_time - data.ra_frequency_num
end

-- 0 未开始 1 开始中 2 结束等待领取奖励
function HappyLinkupData:FlushGameStatus()
    local data = ServerActivityData.Instance:GetHappyLookInfo()

    if data.chanllenge_index == -1 and data.challenge_is_succ == -1 then 
        self.game_master.status = 0
    elseif data.chanllenge_index > -1 and data.challenge_is_succ == -1 and data.challenge_end_timestamp > 0 then 
        self.game_master.status = 1
    elseif data.chanllenge_index > -1 and data.challenge_is_succ > -1 and  data.challenge_end_timestamp == 0 then 
        self.game_master.status = 2
    end  
end

function HappyLinkupData:GetGameSuccess()
    local data = ServerActivityData.Instance:GetHappyLookInfo()
    return data.chanllenge_index,data.challenge_is_succ
end

function HappyLinkupData:ResetLinkMacth(matching)
    local icon_list = {}
    for k,v in pairs(matching) do 
        if not v.disapper then 
            table.insert(icon_list,v.icon)
        end 
    end 

    for k,v in pairs(matching) do 
        if not v.disapper then 
            local mark_pos = GameMath.Ramdon(1,#icon_list) 
            v.icon = icon_list[mark_pos]
            table.remove( icon_list, mark_pos)
        end 
    end 

    return matching
end