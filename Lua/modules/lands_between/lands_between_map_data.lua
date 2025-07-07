-- 本处存放的data方法主要用于与地图的交互

-- 获取初始化视野(model)
function LandsBetweenData:GetInitVisible()
    self:FindMyPos()
    local looked_pos = self:ChangePosFromWhere(self.cur_role_pos)
    local link_pos = self.cur_role_pos

    if self.cache_init_jump ~= nil then 
        looked_pos = self:ChangePosFromWhere(self.cache_init_jump)
        link_pos = self.cache_init_jump
        
        self.cache_init_jump = nil
    end 

    return {
        look_pos = looked_pos,
        link_pos = link_pos,
    }
end

-- where = {x , y}
-- type =0 外域 1 外域
-- 通过坐标点和地图类型获取视野组
function LandsBetweenData:FindVisibleByWhere(where,type)
    if type == nil then type = self.operating_info.type end 

    local operate = type == 0 and self.static_map_out or self.static_map_in
    local oper_visible = type == 0 and self.visible_group.Out or self.visible_group.In
    local link_mark = self.CfgLink({[1] = where.x,[2] = where.y})
    local static_cfg = operate[link_mark] 
    local visible = oper_visible[static_cfg.visible_area]

    return visible
end

-- 将点格坐标转换为本地坐标
function LandsBetweenData:ChangePosFromWhere(data)
    local real_x = (data.x ) * self.length_pic
    local real_y = (data.y ) * self.length_pic

    return Vector3.New(real_x,0,real_y)
end

-- 设置世界坐标信息
function LandsBetweenData:SetMapParentWorldPos(data)
    self.map_WPos = data
end

-- 将点格坐标转换为世界坐标
function LandsBetweenData:ChangeWPosFormWhere(data)
    local p_pos = self.map_WPos.map_parent
    local real_pos = self:ChangePosFromWhere(data)
    return Vector3.New(real_pos.x + p_pos.x,real_pos.y + p_pos.y,real_pos.z + p_pos.z)
end

-- 将世界坐标转换为本地点格
function LandsBetweenData:ChangePosFormWPos(pos)
    local p_pos = self.map_WPos.map_parent
    local local_pos = Vector3.New(pos.x - p_pos.x,pos.y - p_pos.y,pos.z - p_pos.z)

    local where_x = math.floor(local_pos.x / self.length_pic) + 1
    local where_y = math.floor(local_pos.z / self.length_pic) + 1

    return {x= where_x,y = where_y}
end

-- 将link_mark转化为本地坐标
function LandsBetweenData:ChangePosFromWhereByLinkMark(link_mark)
    local pos_vo = self.FixLink(link_mark)
    return self:ChangePosFromWhere({x = pos_vo[1] , y = pos_vo[2]})
end

-- 从本地格获取视野
function LandsBetweenData:GetVisibleFormWhere(data,type)
    if type == nil then type = self.operating_info.type end 
    local link_mark = self.CfgLink({[1] = data.x,[2] = data.y})
    local oper_map = type == 0 and self.static_map_out or self.static_map_in
    local oper_visible = type == 0 and self.visible_group.Out or self.visible_group.In

    -- LogError("?wahs ?",link_mark)
    -- LogError("?wahs------ ?",type)
    if link_mark == nil or link_mark == "" then 
        LogError("地块link_mark为空！")
        return nil
    end 

    local cfg = oper_map[link_mark]

    if cfg == nil then 
        LogError("本地块信息无法获取视野！请检查配置！",data)
        return nil
    end 

    return oper_visible[cfg.visible_area]
end

-- 获取以该视野为中心的周围的视野
function LandsBetweenData:GetVisibleAround(center,is_res,is_drag)
    local type = self.operating_info.type
    local oper_visible = type == 0 and self.visible_group.Out or self.visible_group.In

    local visible_list = {}

    local around = ((not is_drag or is_drag == nil) and is_res) and GoldLaw.ResAround or GoldLaw.Around
    local limit = type == 0 and GoldLaw.OutLimit or GoldLaw.InLimit

    local pos_x_start = (center.x - around) >= 0 and (center.x - around) or 0
    local pos_y_start = (center.y - around) >= 0 and (center.y - around) or 0

    local pos_x_end = ((center.x + around) <= limit) and (center.x + around) or limit
    local pos_y_end = ((center.y + around) <= limit) and (center.y + around) or limit

    for pos_x = pos_x_start,pos_x_end do 
        for pos_y = pos_y_start,pos_y_end do 
            table.insert(visible_list,oper_visible[self:CheckVisibleIndexByLinkMark(self.CfgLink({[1] = pos_x,[2] = pos_y}),type)])
        end
    end 

    return visible_list
end


-- 获取全视野（取当前位置
function LandsBetweenData:GetFullVisible()
    local type = self.operating_info.type
    return type == 0 and self.visible_group.Out or self.visible_group.In 
end 

-- 根据当前协议信息修正初始角色位置（兼注视点）
function LandsBetweenData:FindMyPos()
    self.operating_info.type = self:GetCurMapType()
    
    if self.my_info.row == nil or self.my_info.column == nil then 
        local cur_max_length = self.operating_info.type == 0 and self.max_length.Out or self.max_length.In
        self.cur_role_pos = Vector3.New(11,56)--(cur_max_length/2,cur_max_length/2)    
    else 
        self.cur_role_pos = Vector3.New(self.my_info.row,self.my_info.column)
    end 

end

-- 获取当前角色的位置
function LandsBetweenData:GetRolePos()
    self:FindMyPos()

    return self.cur_role_pos
end

-- -- 保存的是包含transform的list
-- function LandsBetweenData:SetVisibleMark(list)
--     self.marked_visible_list = list
-- end

-- 获取视野
function LandsBetweenData:GetVisibleSize(cam)
    if self.marked_visible_list == nil then return end 

    local visible_list = {}
    for k,v in pairs(self.marked_visible_list) do
        local vp_pos = {}
        vp_pos[1] = cam:WorldToViewportPoint(Vector3.New(v.trans.position.x-(self.length_pic/2),v.trans.position.y,v.trans.position.z-(self.length_pic/2)) )
        vp_pos[2] = cam:WorldToViewportPoint(Vector3.New(v.trans.position.x-(self.length_pic/2),v.trans.position.y,v.trans.position.z+(self.length_pic/2)) )
        vp_pos[3] = cam:WorldToViewportPoint(Vector3.New(v.trans.position.x+(self.length_pic/2),v.trans.position.y,v.trans.position.z-(self.length_pic/2)) )
        vp_pos[4] = cam:WorldToViewportPoint(Vector3.New(v.trans.position.x+(self.length_pic/2),v.trans.position.y,v.trans.position.z+(self.length_pic/2)) )
        for i = 1,4 do
            if vp_pos[i].x > - 0.2 and vp_pos[i].x < 1.2 and vp_pos[i].y > 0 and vp_pos[i].y < 1 then 
                if visible_list[v.group_id] == nil then 
                    visible_list[v.group_id] = v.group_id
                end 
                break
            end 
        end 
    end 

    local visibled_list = {}
    for k,v in pairs(visible_list) do 
        for i,j in pairs(self.visible_group.Out) do 
            if v == j.group_id then 
                table.insert(visibled_list,j)
            end
        end 
    end 

    -- local vp_pos1 = self.map_mono.camera:WorldToViewportPoint(self.map_mono.PosCheck1.position)
    -- local vp_pos2 = self.map_mono.camera:WorldToViewportPoint(self.map_mono.PosCheck2.position)
    return visibled_list
end 

-- 解压——将视野信息转换为具体的格子组
function LandsBetweenData:ReleaseVisible(visible_cell)
    if self.cache_visible_map[visible_cell.type] ~= nil and self.cache_visible_map[visible_cell.type][visible_cell.group_id] ~= nil then 
        return self.cache_visible_map[visible_cell.type][visible_cell.group_id]
    end 

    -- LogError("?hjajs  x",visible_cell.marked_list[1].x,visible_cell.marked_list[2].x)
    -- LogError("?hjajs  y",visible_cell.marked_list[1].y,visible_cell.marked_list[3].y)
    -- LogError("type",visible_cell.type," group_id ",visible_cell.group_id)
    local map_list = {}
    for x = visible_cell.marked_list[1].x, visible_cell.marked_list[2].x do 
        for y = visible_cell.marked_list[1].y,visible_cell.marked_list[3].y do 
            local link_mark = self.CfgLink({[1] = x,[2] = y})
            local vo = {}
            vo.pos = self:ChangePosFromWhere({x=x,y=y})
            vo.link_mark = link_mark
            vo.map_show = visible_cell.type == 0 and self.static_map_out[link_mark] or self.static_map_in[link_mark]

            if visible_cell.type == 0 and self.static_map_out[link_mark].map_stamp == 401 then
                vo.map_show.map_stamp = 801 + self.my_info.faction * 1
            end

            if vo.map_show ~= nil then 
                table.insert(map_list, vo)
            end 
        end 
    end 

    if self.cache_visible_map[visible_cell.type] == nil then 
        self.cache_visible_map[visible_cell.type] = {}
    end 
    if self.cache_visible_map[visible_cell.type][visible_cell.group_id] == nil then 
        self.cache_visible_map[visible_cell.type][visible_cell.group_id] = map_list
    end 
    return map_list
end

-- 设置当前的操作对象——点击地图格
function LandsBetweenData:SetOperatingObj(obj)
    if obj == self.operating_info.obj then 
        self.operating_info.obj = nil
        return 
    end 

    self.operating_info.obj =  obj
end

-- 设置当前的操作对象——点击地图格(测试)
function LandsBetweenData:SetOperatingTestObj(obj)
    self.operating_info.test_obj =  obj
end

-- 检查是不是别的界面也在打开中
-- 仅限客户端配置 LandsLimitClickView
function LandsBetweenData:IsOtherViewOpening()
    for k,v in pairs(LandsLimitClickView) do 
        if ViewMgr:IsOpen(v) then 

            return true
        end 
    end 

    return false
end

-- 获取该位置是否在缓存视野中
-- param = {x , y}
function LandsBetweenData:IsGridCanBeVisible(param)
    local visible = self.map_cahce_info.map_visible

    if visible == nil then return false end 
    
    for k,v in pairs(visible ) do 
        local max_x_limit = math.max(v.marked_list[1].x,v.marked_list[2].x,v.marked_list[3].x,v.marked_list[4].x)
		local min_x_limit = math.min(v.marked_list[1].x,v.marked_list[2].x,v.marked_list[3].x,v.marked_list[4].x)

        local max_y_limit = math.max(v.marked_list[1].y,v.marked_list[2].y,v.marked_list[3].y,v.marked_list[4].y)
		local min_y_limit = math.min(v.marked_list[1].y,v.marked_list[2].y,v.marked_list[3].y,v.marked_list[4].y)

        if param.x >= min_x_limit and param.x <= max_x_limit 
            and param.y >= min_y_limit and param.y <= max_y_limit then
                return true
        end
    end 
    return false
end

-- 获取地图界面的视野信息
-- 本方法的type不能为空
function LandsBetweenData:GetMapVisibleCellList(type)
    local visible = type == 0 and self.visible_group.Out or self.visible_group.In
    local static_map = type == 0 and self.static_map_out or self.static_map_in
    local visible_list = {}
    local v_length = self.visible_length[type]
    local rect_length = math.floor(100 / GoldLaw.VisiLength)

    for k,v in pairs(visible) do 
        local vo = {}
        vo.group_id = v.group_id 
        vo.x = v.x
        vo.y = v.y
        vo.list = {}
        local map_list = self:ReleaseVisible(v)

        local map_simple = map_list[1]
        local simple_pos = self.FixLink(map_simple.link_mark)
        -- 将visible的rect大小限制为100X100,
        -- 每一个rect的具体大小和坐标在本方法内直接计算所得
        for i,j in pairs(map_list) do 
            local vpo = {}
            vpo.link_mark = j.link_mark
            local pos = self.FixLink(j.link_mark)
            vpo.x = tonumber(pos[1]) - tonumber(simple_pos[1]) + 1
            vpo.y = tonumber(pos[2]) - tonumber(simple_pos[2]) + 1

            vpo.pos = Vector3.New(math.floor(rect_length/2)+(vpo.x-1)*rect_length,-math.floor(rect_length/2)-(vpo.y-1)*rect_length,0)
            vpo.marked = (vpo.x == 1 and vpo.y == 1) or (vpo.x == GoldLaw.VisiLength and vpo.y == 1) or (vpo.x == 1 and vpo.y == GoldLaw.VisiLength) or (vpo.x == GoldLaw.VisiLength and vpo.y == GoldLaw.VisiLength)
            vpo.rect_length = rect_length
            
            vpo.map_show = j.map_show
            table.insert(vo.list,vpo)
        end 
        table.insert(visible_list,vo)
    end 
    
    return {
        v_length = v_length,
        visible_list = visible_list
    }
end

-- 地图点击（跳转使用
-- 快速跳跃方案1 {x ,y}
function LandsBetweenData:SetMapClick(pos)
    self.operating_info.map_click = self:ChangePosFromWhere(pos)
end

-- 地图跳跃
-- 快速跳跃方案2 {x ,y}
-- 本方法联动操作轮展开！
function LandsBetweenData:SetMapJumpClick(pos)
    self.operating_info.jump = pos
end

-- 清除地图点击
function LandsBetweenData:ClearMapClick()
    self.operating_info.map_click = nil
end

-- 设置操作缓存的地图格信息
function LandsBetweenData:SetMapPicClick(data)
    self.operating_info.map_pic = data
end

-- 设置操作缓存的地图格信息(测试
function LandsBetweenData:SetMapPicTestClick(data)
    self.operating_info.test_map_pic = data
end

-- 提取操作缓存的地图格信息
-- 本操作同时提取联网信息和静态信息
-- 提取完成后删除缓存信息
-- 警告 根据需要选择是否需要判断连接
function LandsBetweenData:GetMapPicClick(need_check)
    local vo = {}
    vo.static = self.operating_info.map_pic
    vo.net_info = self:GetMapGridNetInfo( vo.static.link_mark )
    -- vo.linking_city = true
    if need_check then
        vo.linking_city = self:GetMapGridLinkingWithCity(self.operating_info.map_pic.map_show)
        vo.adjoining_grid = self:GetMapGridAdjoinWithUs(self.operating_info.map_pic.map_show)    
    end

    -- self.operating_info.map_pic = nil

    return vo
end

-- 测试用信息
function LandsBetweenData:GetMapPicTestClick()
    local vo = {}
    vo.static = self.operating_info.test_map_pic
    vo.net_info = self:GetMapGridNetInfo( vo.static.link_mark )
    -- vo.linking_city = true
    vo.linking_city = self:GetMapGridLinkingWithCity(self.operating_info.test_map_pic.map_show)

    return vo
end

-- 获取神坛名字（注意放入的是静态配置
function LandsBetweenData:GetAlterName(data)
    local level = math.floor(data.map_stamp % 100)
    local title = string.format(Language.LandsBetween.Desc.Alter,  DataHelper.GetDaXie(level))

    return title
end 

-- 初始化主城
function LandsBetweenData:InitMainCity(map_info)
    local map_cfg = map_info.map_cfg
    if map_cfg == nil then 
        map_cfg = {}
        map_cfg.lot_number = map_info.map_stamp
    end 
    -- 百位为4则为主城
    if map_cfg.lot_number == "" then 
        LogError("?警告！ 地块",map_info.link_mark,"为空地块！")
        return 
    end 

    local city_mark = math.floor(tonumber(map_cfg.lot_number)/100)
    if city_mark ~= 4 then 
        return 
    end 
    local faction_mark = math.floor(math.floor(map_cfg.lot_number % 100)/10)
    local type_mark = math.floor(math.floor(map_cfg.lot_number % 100)%10)

    if self.main_city[map_info.type] == nil then 
        self.main_city[map_info.type] = {}
    end 

    -- 外域地图不讲究，只讲究是不是
    if map_info.type == 0 then
        self.main_city[map_info.type][map_info.link_mark] = map_info
    -- 内域地图讲究，讲究是哪个阵营，需要追加上去
    else 
        if self.main_city[map_info.type][faction_mark] == nil then 
            self.main_city[map_info.type][faction_mark] = {}
        end 
        self.main_city[map_info.type][faction_mark][map_info.link_mark] = map_info
    end 
end

function LandsBetweenData:InitAlterNum(link_mark,type)
    local oper_map = type == 0 and self.static_map_out or self.static_map_in
    local cfg = oper_map[link_mark]
    if cfg.map_stamp == "" then 
        LogError("?警告! "..(type == 0 and "外域" or "内域" ).." 的地块",link_mark,"为空地块！")
        return 
    end 
    local map_type = math.floor(cfg.map_stamp/100)
    local level = 0
    if map_type == 1 then 
        level = math.floor(cfg.map_stamp % 100)
    end 
    local is_empty = map_type == 2

    if self.total_alter_num[type] == nil then 
        self.total_alter_num[type] = {}
    end 

    if level > 0 then 
        if self.total_alter_num[type][level] == nil then 
            self.total_alter_num[type][level] = 1
        else 
            self.total_alter_num[type][level] = 1 + self.total_alter_num[type][level]
        end 
    end 

    if is_empty then 
        if self.total_alter_num[type][0] == nil then
            self.total_alter_num[type][0] = 1
        else 
            self.total_alter_num[type][0] = 1 + self.total_alter_num[type][0]
        end
    end 
end

-- 返回当前的域的总占领信息
-- 闲了得优化一下……
function LandsBetweenData:GetTotalGridList()
    local list = {}
    for k,v in pairs(self.net_map_info.list) do 
        local cfg = self:GetMapGridStaticInfo(k)
        if cfg.map_stamp == "" then 
            LogError("?警告！ 地块",cfg.link_mark,"为空地块！")
            return 
        end 
        local map_type = math.floor(cfg.map_stamp/100)
        local level = 0
        if map_type == 1 then 
            level = math.floor(cfg.map_stamp % 100)
        end 
        local is_empty = map_type == 2

        if level > 0 then 
            if list[level] == nil then 
                list[level] = 1
            else 
                list[level] = 1 + list[level]
            end 
        elseif is_empty then 
            if list[0] == nil then
                list[0] = 1
            else 
                list[0] = 1 + list[0]
            end
        end 
    end 

    for k,v in pairs(self.net_alter_info.list) do 
        local cfg = self:GetMapGridStaticInfo(k)

        local map_type = math.floor(cfg.map_stamp/100)
        local level = 0
        if map_type == 1 then 
            level = math.floor(cfg.map_stamp % 100)
        end 
        local is_empty = map_type == 2

        if level > 0 then 
            if list[level] == nil then 
                list[level] = 1
            else 
                list[level] = 1 + list[level]
            end 
        elseif is_empty then 
            if list[0] == nil then
                list[0] = 1
            else 
                list[0] = 1 + list[0]
            end
        end 
    end 

    return list
end

-- 获取该地图格是否为主城
function LandsBetweenData:GetIsMainCity(type,faction,link_mark)
    if type == nil then 
        return false
    end 

    if link_mark == nil then 
        return false
    end 

    local checker = self.main_city[type]

    if type == 1 and checker[faction] == nil then 
        LogError("?警告！",Language.LandsBetween.FactionName[faction],"主城信息报空")
        return false
    end 

    local sp_map_pic = self:CheckSpPicAroundByLinkMark(link_mark)
    if sp_map_pic ~= nil then 
        if sp_map_pic.faction == faction 
            and (sp_map_pic.act_type == ActType.Animals or sp_map_pic.act_type == ActType.DragonPluse ) then 
        return true
        end 
    end 

    if type == 0 then 
        return checker[link_mark] ~= nil
    else 
        return checker[faction][link_mark] ~= nil
    end 
end

-- 地图格是否在占领CD中
-- 放入的是协议地图格信息
function LandsBetweenData:IsInOccupyCD(grid)
    if grid == nil then 
        return false ,0
    end 
    local link_mark = self.CfgLink({[1] = grid.row ,[2] = grid.column})
    local sp_mark = self:GetResOfSpPicByLinkMark(link_mark)
    -- 有活动配置且活动配置为有阵营且信息也是有阵营的情况下
    if sp_mark ~= nil and sp_mark.faction ~= -1 and ((grid.faction and grid.faction ~= -1 ) or (grid.occupied_faction and grid.occupied_faction ~= -1 )) then
        return false,0
    end 

    local timestamp = grid.can_occupy_timestamp 
    local flag =  grid.can_occupy_timestamp - TimeManager:GetServerTime()
    return flag > 0 , timestamp
end

-- 是否已被我方占领
-- 放入的是协议地图格信息
function LandsBetweenData:IsOurGridByNet(grid)
    return grid ~= nil and (grid.faction == self.my_info.faction or grid.occupied_faction == self.my_info.faction)
end

-- 是否神坛
-- 放入的是静态地图格信息
function LandsBetweenData:IsAltar(grid)
    if grid.map_stamp == nil then return false end 

    return GoldLaw.Operate[grid.map_stamp] == 2
end

-- 获取该地图格是否为主城
-- 注入信息为静态配置信息
function LandsBetweenData:IsMainCity(grid)
    if grid.map_show.map_stamp == nil then return false end 
    return math.floor(grid.map_show.map_stamp/100) == 4 or math.floor(grid.map_show.map_stamp/100) == 8
end

-- 地图格刷新变化
function LandsBetweenData:SetFlushPicComplete(flag)
    self.operating_info.map_load_mark = flag
end

-- 地图格刷新是否完成
function LandsBetweenData:IsFlushPicComplete()
    return self.operating_info.map_load_mark
end

-- 存一份当前的有效视野
function LandsBetweenData:SetCurVisibleList(list)
    self.map_cahce_info.map_visible = list
end

-- 将角色信息转换为带XY的格子信息
function LandsBetweenData:GetOtherRolePos(data)
    return Vector3.New(data.row,data.column,0) 
end

function LandsBetweenData:GetMapPicPosStr()
    local map_pic = self:GetMapPicClick()
    local fix_link = self.FixLink(map_pic.static.link_mark)
    local name = (self:IsAltar(map_pic.static.map_show) and Language.LandsBetween.Desc.PosStrType[1] or Language.LandsBetween.Desc.PosStrType[0])    
    local pos = "("..fix_link[1]..","..fix_link[2]..")"

    return name..pos
end

-- 将协议格子信息修正为地图头像格信息
-- 注意注入的是地图协议和角色协议的信息哦
function LandsBetweenData:FixMapRoleHead(info)
    local vo = {}
    vo.link_mark = self.CfgLink({[1] = info.row ,[2] = info.column})
    vo.level = self:GetAlterGridLevel({x = info.row,y = info.column})
    vo.is_altar = vo.level > 0
    vo.time_stamp = info.pray_end_timestamp or 0
    vo.role_name = info.user_name
    vo.avatar_type = info.avatar_type
    vo.head_id = info.head_id
    vo.is_ironying = vo.time_stamp > 0
    return vo
end

-- 将协议格子信息修正为地图其他玩家模型信息
-- 注意注入的是地图协议和角色协议的信息哦
function LandsBetweenData:FixMapRoleModel(info)
    local vo = {}
    vo.link_mark = self.CfgLink({[1] = info.row ,[2] = info.column})
    vo.role_name = info.user_name
    vo.faction = info.faction
    vo.avatar_type = info.avatar_type
    local appearance = CommonStruct.AppearanceParam({
        avatar_type = info.avatar_type,
        prof_base = info.profession,
    })

    vo.role_app = appearance
    return vo
end

-- 强制刷新主界面（对operating_info有监听的部分
function LandsBetweenData:ForceFlushOperate()
    self.operating_info.force_flush = self.operating_info.force_flush +1
end

-- 强制刷新视野（对operating_info有监听的部分
function LandsBetweenData:ForceFlushVisibleOperate()
    LogDG("强制刷新视野————————————请留意您的调用频率·")
    self.operating_info.force_flush_visible = self.operating_info.force_flush_visible +1
end

-- 强制关闭操作轮（目前只针对操作轮
function LandsBetweenData:ForceCloseOperate()
    LogDG("强制关闭操作轮————————————请留意您的调用频率·")
    self.operating_info.force_close_operate = self.operating_info.force_close_operate +1
end

-- 强制刷新额外list
function LandsBetweenData:ForceFlushExtra()
    self.operating_info.flush_extra = self.operating_info.flush_extra + 1
end

-- 获取地图格的神坛等级（若不是神坛则为0级
-- 要求是x,y
function LandsBetweenData:GetAlterGridLevel(data)
    local link_mark = self.CfgLink({[1] = data.x,[2] = data.y})
    local cfg = self:GetMapGridStaticInfo(link_mark)

    local map_type = math.floor(cfg.map_stamp/100)
    if map_type == 1 then 
        return math.floor(cfg.map_stamp % 100)
    end 

    return 0
end 

-- 检查该格子能否被操作
-- 判断为，主城，神坛，空地块返回有效
-- 注意注入的是静态配置哦！
function LandsBetweenData:CheckCanOperate(data)
    if GoldLaw.Operate[data.map_show.map_stamp] then
        return true
    end 

    if self:IsMainCity(data) then 
        return true
    end 

    return false
end

-- 检查该主城格子是否为我方格子
-- 注意注入的是静态配置哦！
function LandsBetweenData:GetMainCityIsMyCity(data)
    if self:GetCurMapType() == 1 then 
        local map_faction = math.floor(math.floor(data.map_show.map_stamp % 100)/10)
        return map_faction == self.my_info.faction
    else 
        return self:GetIsMainCity(self:GetCurMapType(),self.my_info.faction,data.map_show.link_mark)
    end 

end

-- 强制跳跃到指定活动的地图格、
-- 快速跳跃方案2
function LandsBetweenData:JumpToYourActSeqGrid(seq) 
    for k,v in pairs(self.extral_activity) do 
        if k == seq then 
            local pos = nil
            if v.divine_realm_stage == 1 then 
                local split = self.FixLink(v.links[1].link_mark)
                pos = {x = split[1],y = split[2]}
            else
                for i,j in pairs(v.links) do
                    if j.faction == self.my_info.faction then 
                        local split = self.FixLink(j.link_mark)
                        pos = {x = split[1],y = split[2]}
                        break
                    end 
                end 
            end 
            if pos ~= nil then 
                self:SetMapClick(pos)

                -- TimeHelper:AddDelayTimer(function()
                    self:SetMapJumpClick(pos)
                -- end,1)
            else 
                LogError("表有问题")
            end 
        end 
    end 
end 

-- 获取该地图格是否为特殊格
function LandsBetweenData:GetIsExtral(link_mark)
    for k,v in pairs(self.extral_activity) do 
        for i,j in pairs(v.links)do 
            if j.link_mark == link_mark then 
                return true,v.operate
            end 
        end 
    end 

    return false,nil
end


function LandsBetweenData:GetMapResInfo(map_stamp)
    return self.map_resources_list[map_stamp]
end

-- 通过现实地块坐标转换成ui坐标
function LandsBetweenData:FixMapViewPicPos(x,y,type)
    local out_x = { a1 = 61,a2 =1761, a3 = 2296,a4 = 3997 }
    local out_y = { a1 = -981,a2 =-2163, a3 = -46 ,a4 = -1228 }

    local in_x = { a1 = 4040,a2 =2291, a3 = 1784 ,a4 = 34 }
    local in_y = { a1 = -1236,a2 =-21, a3 = -2183 ,a4 = -966 }

    local function out_change_pos(x,y,param)
        return (1/100)*(param.a2-param.a1)*x +(1/102)*(param.a3-param.a1)*y+param.a4-((105/100)*(param.a2-param.a1)+(106/102)*(param.a3-param.a1))
    end

    local function in_change_pos(x,y,param)
        return (1/141)*((param.a2-param.a1)*x +(param.a3-param.a1)*y)+param.a4-(145/141)*(param.a3+param.a2-2*param.a1)
    end

    local x_input = type == 0 and out_x or in_x
    local y_input = type == 0 and out_y or in_y

    local pos_x = type == 0 and out_change_pos(x,y,x_input) or in_change_pos(x,y,x_input)
    local pos_y = type == 0 and out_change_pos(x,y,y_input) or in_change_pos(x,y,y_input)

    return {x = pos_x , y = pos_y}
end

-- 根据主动公式逆推公式得出的近似位置
-- 误差过大，放弃
function LandsBetweenData:MapChangePosFormWPos(click_pos,type)
    local out_x = { a1 = 61,a2 =1761, a3 = 2296,a4 = 3997 }
    local out_y = { a1 = -981,a2 =-2163, a3 = -46 ,a4 = -1228 }
    local in_x = { a1 = 1784,a2 =34, a3 = 4040 ,a4 = 2291 }
    local in_y = { a1 = -2183 ,a2 =-966, a3 = -1236 ,a4 = -21 }

    local function out_change_pos(pox,poy,px,py)
        local pa1 = (1/100)*(px.a2-px.a1)
        local pa2 = (1/100)*(py.a2-py.a1)
        local pb1 = (1/102)*(px.a3-px.a1)
        local pb2 = (1/102)*(py.a3-py.a1)
        local pz1 = (105/100)*(px.a2-px.a1)+(106/102)*(px.a3-px.a1)
        local pz2 = (105/100)*(py.a2-py.a1)+(106/102)*(py.a3-py.a1)

        local x = (poy - pz2 - pb2*((pox - pz1)/pb1))/(pa2 - (pa1*pb2)/pb1)
        local y = (pox - pz1)/pb1 - (pa1/pb1) * x 

        return {x = x,y = y}
    end

    local function in_change_pos(pox,poy,px,py)
        local pa1 = (1/141)*(px.a2-px.a1)
        local pa2 = (1/141)*(py.a2-py.a1)
        local pb1 = (1/141)*(px.a3-px.a1)
        local pb2 = (1/141)*(py.a3-py.a1)
        local pz1 = px.a4-(145/141)*(px.a3+px.a2-2*px.a1)
        local pz2 = py.a4-(145/141)*(py.a3+py.a2-2*py.a1)

        local x = (poy - pz2 - pb2*((pox - pz1)/pb1))/(pa2 - (pa1*pb2)/pb1)
        local y = (pox - pz1)/pb1 - (pa1/pb1) * x 

        return {x = x,y = y}
    end

    local x_input = type == 0 and out_x or in_x
    local y_input = type == 0 and out_y or in_y

    local v_pos = type == 0 and out_change_pos(click_pos.x,click_pos.y,x_input,y_input) or in_change_pos(click_pos.x,click_pos.y,x_input,y_input)

    return {x = math.floor(v_pos.x) ,y = math.floor(v_pos.y)}
end

function LandsBetweenData:GetMapVisibleList(center_pos,type,is_draging)

    -- local v_pos = self:MapChangePosFormWPos(center_pos)
    -- local visible = self:GetVisibleFormWhere(v_pos)
    -- LogError("?asd ",visible)
    local lock_link_mark = nil 
    local lock_pos = nil
    local oper_list = type == 0 and self.map_view_visible.Out or self.map_view_visible.In
    for k,v in pairs(oper_list) do
        local max_x_limit = math.max(v.marked_poses[1].x,v.marked_poses[2].x,v.marked_poses[3].x,v.marked_poses[4].x)
		local min_x_limit = math.min(v.marked_poses[1].x,v.marked_poses[2].x,v.marked_poses[3].x,v.marked_poses[4].x)

        local max_y_limit = math.max(v.marked_poses[1].y,v.marked_poses[2].y,v.marked_poses[3].y,v.marked_poses[4].y)
		local min_y_limit = math.min(v.marked_poses[1].y,v.marked_poses[2].y,v.marked_poses[3].y,v.marked_poses[4].y)

        if center_pos.x > min_x_limit and center_pos.x < max_x_limit then 
            if center_pos.y > min_y_limit and center_pos.y < max_y_limit then 
                lock_link_mark = k
                lock_pos = {x = v.x,y= v.y}
                break
            end 
        end 
    end 

    if lock_link_mark == nil then
        return {}
    end

    -- LogError("?ASd GR",lock_pos)
    -- local center = self:GetVisibleFormWhere(lock_pos)
    local oper_visible = type == 0 and self.visible_group.Out or self.visible_group.In
    local visible_list = {}
    local around = is_draging and (type == 0 and 4 or 5) or (type == 0 and 5 or 6)
    local limit = type == 0 and GoldLaw.OutLimit or GoldLaw.InLimit

    local pos_x_start = (lock_pos.x - around) >= 0 and (lock_pos.x - around) or 0
    local pos_y_start = (lock_pos.y - around) >= 0 and (lock_pos.y - around) or 0

    local pos_x_end = ((lock_pos.x + around) <= limit) and (lock_pos.x + around) or limit
    local pos_y_end = ((lock_pos.y + around) <= limit) and (lock_pos.y + around) or limit

    for pos_x = pos_x_start,pos_x_end do 
        for pos_y = pos_y_start,pos_y_end do 
            table.insert(visible_list,oper_visible[self:CheckVisibleIndexByLinkMark(self.CfgLink({[1] = pos_x,[2] = pos_y}),type)])
        end
    end 
    
    return visible_list

end

function LandsBetweenData:CheckMapVisibleShow(map_pic,map_input)
    -- 总数量1k以下直接生成！
    -- if table.nums(self.net_map_show_info.list) < 1000 then 
    --     return true
    -- end 
    local pos = self:FixMapViewPicPos(map_pic.x,map_pic.y,map_input.type)
    if - pos.x <= map_input.visible.start.x and -pos.x >= map_input.visible.limit.x 
        and pos.y <= -map_input.visible.start.y and pos.y >= -map_input.visible.limit.y then
        -- and GoldLaw.Operate[map_pic.map_stamp] ~= nil then
            
        return true
    end 

    return false
end

function LandsBetweenData:TryJumpPos(pos)
    
    local area = {
        [0] = {x = 20,y=11},
        [1] = {x = 14,y=8},
    }

    local type = self.operating_info.type
    local operate = type == 0 and self.static_map_out or self.static_map_in
    for k,v in pairs(operate) do 
        local fix_link = self.FixLink(k)
        local fix_pos = self:FixMapViewPicPos(fix_link[1],fix_link[2],type)
        if pos.x >= fix_pos.x - area[type].x and pos.x <= fix_pos.x + area[type].x 
            and pos.y >= fix_pos.y - area[type].y and pos.y <= fix_pos.y + area[type].y then 
                self:SetMapClick({x=fix_link[1],y = fix_link[2]})
                self:SetMapJumpClick({x=fix_link[1],y = fix_link[2]})
                ViewMgr:CloseView(LandsBetweenMapView)

                break
        end 
    end 
end

function LandsBetweenData:GetMapLockPos()
    local checked_pos = self:FixMapViewPicPos(self.cur_role_pos.x,self.cur_role_pos.y,self.operating_info.type)

    return checked_pos
end

-- 设置跳跃缓存
function LandsBetweenData:SetInitJumpCache(pos_cache)
    self.cache_init_jump = pos_cache
end

-- -- 甚至强制链接（link_mark,和地图上生成的transform）
-- -- 注意强制链接的时机，要在完成创建后再进行连接
-- 强制链接方案不成了，放弃
-- function LandsBetweenData:SetForceMark(sp_mark,transform)
--     self.force_link_mark_trans[sp_mark] = transform
-- end

-- function LandsBetweenData:GetForceMark(sp_mark)
--     return self.force_link_mark_trans[sp_mark]
-- end

function LandsBetweenData:OperRunningMark(oper)
    self.map_running_mark = self.map_running_mark + oper

    if self.map_running_mark < 0 then 
        self.map_running_mark = 0
    end 
    -- LogError("?cur mark!",self.map_running_mark,oper)
end

function LandsBetweenData:GetRunningMark()
    return self.map_running_mark 
end