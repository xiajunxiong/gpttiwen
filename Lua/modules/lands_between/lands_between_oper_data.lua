-- 本处存放的data方法主要用于与服务器交互

-- 基础信息 5501
function LandsBetweenData:SetBaseInfo(protocol)
    self.base_info.season_index = protocol.season_index
    self.base_info.cur_status = protocol.cur_status
    self.base_info.is_open = protocol.is_open
    self.base_info.bigcross_available = protocol.bigcross_available
    self.base_info.ourter_space_timestamp = protocol.ourter_space_timestamp
    self.base_info.inner_space_timestamp = protocol.inner_space_timestamp
    self.base_info.season_end_timestamp = protocol.season_end_timestamp
end 

-- 用户信息 5503
function LandsBetweenData:SetUserInfo(protocol)
    self.my_info.plat_type = protocol.plat_type
    self.my_info.row = protocol.row
    self.my_info.column = protocol.column
    self.my_info.faction = protocol.faction
    self.my_info.action_point  = protocol.action_point
    self.my_info.today_gold_buy_ap_times  = protocol.today_gold_buy_ap_times
    self.my_info.today_jade_buy_ap_times  = protocol.today_jade_buy_ap_times
    self.my_info.ap_item_num = protocol.ap_item_num
    self.my_info.next_recover_ap_timestamp = protocol.next_recover_ap_timestamp
    self.my_info.recover_all_ap_timestamp = protocol.recover_all_ap_timestamp
    self.my_info.role_uuid = protocol.role_uuid

    local account_key = LoginData.Instance:GetAccountKey()
    local last_login_server = LoginData.Instance:getAccountCacheData(account_key).last_login_server
    local server_info = LoginData.Instance:GetServerInfoById(last_login_server)

    local t = {}
    t.uid = RoleData.Instance:GetRoleId()
    t.plat_type = self.my_info.plat_type
    t.user_name = RoleData.Instance:GetRoleName()
	-- t.server_id = server_info.id
	t.row = self.my_info.row
	t.column = self.my_info.column 
	t.faction = self.my_info.faction 
	t.avatar_type = RoleData.Instance:GetRoleAvatarType()
	t.profession = RoleData.Instance:GetProfession()

    local link_mark = self.CfgLink({[1] = t.uid ,[2] = t.plat_type})
    self.net_role_info.list[link_mark] = t
end 

-- 移动信息 5506
function LandsBetweenData:FlushRoleMovedInfo(protocol)
    local link_mark = self.CfgLink({[1] = protocol.uid ,[2] = protocol.plat_type})
    if self.net_role_info.list[link_mark] ~= nil then 
        self.net_role_info.list[link_mark].row = protocol.row
        self.net_role_info.list[link_mark].column = protocol.column
    else 
        
    end 
    
    self.net_role_info.flush = self.net_map_info.flush +1
end 

-- 地图变化 5507
function LandsBetweenData:FlushMapGridInfo(protocol)
    local link_mark = self.CfgLink({[1] = protocol.info.row ,[2] = protocol.info.column})

    local static = self:GetMapGridStaticInfo(link_mark)
    if self:IsAltar(static) then 
        self.net_alter_info.list[link_mark] = protocol.info
        self.net_map_show_info.list[link_mark] = {
            link_mark = link_mark,
            faction = protocol.info.occupied_faction
        }
        self:SetLinkMarkIndex(link_mark)
        self.net_alter_info.flush = self.net_alter_info.flush +1
    else 
        self.net_map_info.list[link_mark] = protocol.info
        self.net_map_show_info.list[link_mark] = {
            link_mark = link_mark,
            faction = protocol.info.occupied_faction
        }
        self:SetLinkMarkIndex(link_mark)
        self.net_map_info.flush = self.net_map_info.flush +1
    end
end

-- 神坛祈祷信息变化 5508
function LandsBetweenData:FlushAltarPrayGridInfo(protocol)
    local link_mark = self.CfgLink({[1] = protocol.info.row ,[2] = protocol.info.column})

    self.net_alter_info.list[link_mark] = protocol.info
    self.net_alter_info.flush = self.net_alter_info.flush +1
    
    self.net_map_show_info.list[link_mark] = {
        link_mark = link_mark,
        faction = protocol.info.faction
    }
    self:SetLinkMarkIndex(link_mark)
end

-- 神坛守军信息下发 5509
function LandsBetweenData:FlushAltarDefenderInfo(protocol)
    self.cache_defendr = protocol

    local map_pic = self:GetMapPicClick()
    local title = self:GetAlterName(map_pic.static.map_show)
    ViewMgr:OpenView(LandsBetweenSingleAltarView,{holded = false,city_name = title})
end

function LandsBetweenData:GetDefenderInfo()
    return self.cache_defendr
end 

-- 阵营分配 5510
function LandsBetweenData:SetUserAssignFaction(protocol)
    self.my_info.faction = protocol.faction

    ViewMgr:OpenView(LandsBetweenCityMatchView)
end 

-- 精华简要信息 5511
function LandsBetweenData:SetUserEssemceBriefInfo(protocol)
    self.brief_info.guaji_essence_per_minute = protocol.guaji_essence_per_minute
    self.brief_info.altar_essence_per_minute = protocol.altar_essence_per_minute
    self.brief_info.unfetch_essence_num = protocol.unfetch_essence_num
    self.brief_info.unfetch_essence_item_num = protocol.unfetch_essence_item_num
    self.brief_info.unfetch_guaji_essence = protocol.unfetch_guaji_essence
end 

-- 新战报 5512
function LandsBetweenData:SetUserNewReportArrived(protocol)
    -- self.report_info.list[#self.report_info.list+1] = protocol.info
    table.insert(self.report_info.list,protocol.info)
    self.report_info.list[#self.report_info.list].index = #self.report_info.list
    
    self.report_info.flush = self.report_info.flush + 1
end  

-- 战报列表 5513
function LandsBetweenData:SetUserReportList(protocol)
    self.report_info.list = {}
    for i = 1, #protocol.report_list do 
        local vo = TableCopy(protocol.report_list[i])
        vo.index = i

        table.insert(self.report_info.list,vo)
    end 
    self.report_info.flush = self.report_info.flush + 1
end  

-- 全地图信息 5504
function LandsBetweenData:SetAllMapInfo(protocol)
    self.net_map_info.list = {}
    self.net_alter_info.list = {}
    self.net_map_show_info.list = {}
    self.net_map_show_info.index_list = {}
    self.net_map_show_info.link_mark_list = {}

    for k,v in pairs(protocol.occupy_grids) do 
        local link_mark = self.CfgLink({[1] = v.row  ,[2] = v.column})
        local static = self:GetMapGridStaticInfo(link_mark)
        if self:IsAltar(static) then 
            self.net_alter_info.list[link_mark] = v
        else 
            self.net_map_info.list[link_mark] = v    
        end

        self.net_map_show_info.list[link_mark] = {
            link_mark = link_mark,
            faction = v.occupied_faction
        }
        self:SetLinkMarkIndex(link_mark)
    end 
    
    for k,v in pairs(protocol.praying_altars) do 
        local link_mark = self.CfgLink({[1] = v.row ,[2] = v.column})
        self.net_alter_info.list[link_mark] = v

        self.net_map_show_info.list[link_mark] = {
            link_mark = link_mark,
            faction = v.faction
        }
        self:SetLinkMarkIndex(link_mark)
    end 
end

-- 全角色信息 5505
function LandsBetweenData:SetAllUserInfo(protocol)
    self.net_role_info.list = {}
    for k,v in pairs(protocol.users) do 
        local link_mark = self.CfgLink({[1] = v.uid ,[2] = v.plat_type})
        self.net_role_info.list[link_mark] = v
    end
end

-- 同阵营玩家列表 5516
function LandsBetweenData:SetFactionUser(protocol)
    local fix_list = self.faction_user.list

    for i = 1,protocol.user_num do 
        table.insert(fix_list,protocol.users[i])
    end 
    self.faction_user.list = fix_list
    self.faction_user.wait_num = protocol.remain_num
    self.faction_user.total_num = protocol.remain_num + protocol.user_num
    self.faction_user.flush = self.faction_user.flush + 1
end 

-- 同阵营信息 5517
function LandsBetweenData:SetFactionBaseInfo(protocol)
    self.faction_base.user_num = protocol.user_num
    self.faction_base.server_num = protocol.server_num
    self.faction_base.occupy_glade_num = protocol.occupy_glade_num
    self.faction_base.max_glade_num = protocol.max_glade_nummax_glade_num
end 

-- 神域未领取精华奖励信息 5522
function LandsBetweenData:SetUnfetchEssenceInfo(protocol)
    self.brief_info.unfetch_essence_num = protocol.unfetch_essence_num
    self.brief_info.unfetch_guaji_essence = protocol.unfetch_guaji_essence
    self.brief_info.unfetch_essence_item_num = protocol.unfetch_essence_item_num
    self.brief_info.unfetch_list = protocol.unfetch_list
    self.brief_info.flush = self.brief_info.flush + 1
end 

-- 神域阶段任务信息 5523
function LandsBetweenData:SetPhaseTaskInfo(protocol)
    self.phasetask_info.outer_tasks = protocol.outer_tasks
    self.phasetask_info.outer_progress = protocol.outer_progress
    self.phasetask_info.inner_tasks = protocol.inner_tasks
    self.phasetask_info.inner_progress = protocol.inner_progress
    self.phasetask_info.outer_task_flag = protocol.outer_task_flag
    self.phasetask_info.inner_task_flag = protocol.inner_task_flag
    self.phasetask_info.outer_reward_flag = protocol.outer_reward_flag
    self.phasetask_info.inner_reward_flag = protocol.inner_reward_flag
    
    self.phasetask_info.flush = self.phasetask_info.flush + 1
end

-- 玩家能否进入神域 5534
function LandsBetweenData:SetCanAccess(protocol)
    self.my_info.can_access = protocol.can_enter
end

function LandsBetweenData:GetCanAccess()
    return self.my_info.can_access == 1
end

-- 获取当前的地图状态
function LandsBetweenData:GetCurMapType()
    if self.base_info.cur_status == nil then 
        return 0
    end 

    if self.base_info.cur_status <= 3 then 
        return 0
    elseif self.base_info.cur_status >= 4 then  
        return 1
    end 
end

-- 设置 index - link_mark 对net_info的操作位置
function LandsBetweenData:SetLinkMarkIndex(link_mark)
    if self.net_map_show_info.link_mark_list[link_mark] == nil then 
        table.insert(self.net_map_show_info.index_list,link_mark)
        self.net_map_show_info.link_mark_list[link_mark] = #self.net_map_show_info.index_list
    end 
end

function LandsBetweenData:GetLinkMarkIndex(index)
    return self.net_map_show_info.index_list[index]
end

-- 获取当前的活动状态
function LandsBetweenData:GetCurStatus()
    if self.base_info.cur_status == nil then 
        return 6
    end 
    return self.base_info.cur_status
end

-- 获取协议地图格信息
function LandsBetweenData:GetMapGridNetInfo(link_mark)
    local map_pos = self.net_map_info.list[link_mark]
    if map_pos == nil then 
        map_pos = self.net_alter_info.list[link_mark]
    end 
    return map_pos
end

-- 获取协议地图格是否空白
function LandsBetweenData:GetMapGridNetIsEmpty(link_mark)
    return self:GetMapGridNetInfo(link_mark) == nil
end

-- 获取静态地图格配置
function LandsBetweenData:GetMapGridStaticInfo(link_mark)
    local oper_map = self:GetCurMapType() == 0 and self.static_map_out or self.static_map_in
    return oper_map[link_mark]
end

-- 获取该地图格的周边是否存在相连的我方已占领格子
-- link_mark-受检查格子，limit_mark-不检查的格子
function LandsBetweenData:GetMapGridLinkingUs(checker)
    if self.limit_mark[checker.link_mark] == 1 then
        -- LogError("神域不可占用检查过了")
        return false
    end
    local fix_link = self.FixLink(checker.link_mark)
    local check_pos = {}
    check_pos[1] = {[1] = fix_link[1]-1, [2] = fix_link[2]}
    check_pos[2] = {[1] = fix_link[1]+1, [2] = fix_link[2]}
    check_pos[3] = {[1] = fix_link[1], [2] = fix_link[2]-1}
    check_pos[4] = {[1] = fix_link[1], [2] = fix_link[2]+1}

    local cells = {}
    for i = 1,#check_pos do 
        local link_mark = self.CfgLink(check_pos[i])

        local static_cell = self:GetMapGridStaticInfo(link_mark)
        local net_cell = self:GetMapGridNetInfo(link_mark)

        if net_cell ~= nil and static_cell ~= nil then 
            if (net_cell.occupied_faction ~= nil and self.my_info.faction == net_cell.occupied_faction) or
               (net_cell.faction ~= nil and self.my_info.faction == net_cell.faction) then 

                local link_mark = self.CfgLink({[1] = net_cell.row ,[2] = net_cell.column})
                if self:LimitMarking(link_mark) then 
                    table.insert(cells,static_cell)
                end 

            -- if self:GetIsMainCity(static_cell.,net_cell.faction,net_cell.link_mark)
            end 
        elseif static_cell ~= nil then 
            if self:GetIsMainCity(static_cell.type,self.my_info.faction,static_cell.link_mark) then
                if self:LimitMarking(static_cell.link_mark) then 
                    table.insert(cells,static_cell)
                end  
            end 
        end 
    end 

    -- 完成检查后将自己装入限制列表
    self.limit_mark[checker.link_mark] = 1 

    return #cells >0 , cells
end

-- 检查是否被限制的格子
function LandsBetweenData:LimitMarking(link_mark)
    return self.limit_mark[link_mark] == nil
end


-- 获取该地图格是否与主城相连
-- 注入的必须是整个格子
function LandsBetweenData:GetMapGridLinkingWithCity(cell)
    if self.draging == true then
        return
    end
    -- if self.operating_info.type == 0 then 
    --     return true
    -- end 
    -- 清空限制器
    self.check_cell_list = self.check_cell_list or {}
    self.limit_times = 0
    self.limit_mark = {}
    self.is_link_city = false
    self.check_cell_list[cell] = self.check_cell_list[cell] or {}
    if self.check_cell_list[cell].check_time ~= nil and (os.time() - self.check_cell_list[cell].check_time) < 2 then
        -- LogError("神域",self.check_cell_list[cell].result)
        return self.check_cell_list[cell].result
    end
    self.check_cell_list[cell].check_time = os.time()
    self:GetMapGridLinkingIsCity(cell)

    -- LogError("神域递归次数", self.limit_times, self.is_link_city)
    self.check_cell_list[cell].result = self.is_link_city
    return self.is_link_city
end


-- 该地图格的连接格是否为主城格
function LandsBetweenData:GetMapGridLinkingIsCity(cell)
    -- 强制终止
    if self.is_link_city == true or self.limit_times > 3000 then 
        -- LogDG("递归次数",self.limit_times,"参与连线",self.limit_mark)
        return self.is_link_city
    end 
    -- 该格子就是主城格
    if self:GetIsMainCity(cell.type,self.my_info.faction,cell.link_mark) then 
        self.is_link_city = true
        -- LogDG("递归次数",self.limit_times,"参与连线",self.limit_mark)
        return self.is_link_city
    end

    local is_link,cells = self:GetMapGridLinkingUs(cell)
    -- 已经没有非限制的与该格子相连的格子
    if not is_link then 
        -- LogError("神域不可占用")
        return self.is_link_city
    end 

    self.limit_times = self.limit_times + 1
    for k,v in pairs(cells) do 
        self:GetMapGridLinkingIsCity(v)
    end
end

-- 直接取周围四格是否有我方阵营地块即可
function LandsBetweenData:GetMapGridAdjoinWithUs(map_pic)
    local function checker(map_pic,delta_x,delta_y)
        local fix_link =  LandsBetweenData.FixLink(map_pic.link_mark)
        local checker_link_mark = LandsBetweenData.CfgLink({[1] = fix_link[1] + delta_x,[2] = fix_link[2] + delta_y})
    
        if LandsBetweenData.Instance:GetIsMainCity(LandsBetweenData.Instance:GetCurMapType(),LandsBetweenData.Instance.my_info.faction,checker_link_mark) then 
            return true
        end 

        local static = self:GetMapGridStaticInfo(checker_link_mark)
        if LandsBetweenData.Instance:IsAltar(static) then 
            local check_alter_info = self.net_alter_info.list[checker_link_mark]
            if check_alter_info == nil then 
                return false
            end 
            if check_alter_info.occupied_faction ~= nil then 
                return check_alter_info.occupied_faction == LandsBetweenData.Instance.my_info.faction
            end 

            if check_alter_info.faction ~= nil then 
                return check_alter_info.faction == LandsBetweenData.Instance.my_info.faction
            end 
        else
            local check_grid_info = self.net_map_info.list[checker_link_mark]
            if check_grid_info == nil then 
                return false
            end 
            if check_grid_info.occupied_faction ~= nil then 
                return check_grid_info.occupied_faction == LandsBetweenData.Instance.my_info.faction
            end 
        end 

        return false
    end
    

    return checker(map_pic,-1,0) or checker(map_pic,1,0) or checker(map_pic,0,1) or checker(map_pic,0,-1)
    
end

-- 获得当前我方阵营
function LandsBetweenData:GetMyFaction()
    return self.my_info.faction
end

-- 名字不 改 了
function LandsBetweenData:GetMyCityIconStr()
    return self:GetCityIconStr(self.my_info.faction)
end

function LandsBetweenData:GetCityIconStr(faction)
    if faction == 0 then 
        return "ShenYuZhenYing_Shui"
    elseif faction == 1 then 
        return "ShenYuZhenYing_Huo"
    elseif faction == 2 then 
        return "ShenYuZhenYing_Feng"
    elseif faction == 3 then 
        return "ShenYuZhenYing_Di"
    end 
end

-- 获得当前隶属我方阵营的神坛
function LandsBetweenData:GetMyFactionNetAlter()
    local list = {}
    for k,v in pairs(self.net_alter_info.list) do 
        if v.faction == self.my_info.faction then 
            table.insert(list,v)
        end 
    end 
    return list
end 

-- 获得当前隶属我的神坛
function LandsBetweenData:GetMyNetAlter()
    local role_id = RoleData.Instance:GetRoleId()
    local list = {}
    for k,v in pairs(self.net_alter_info.list) do 
        if v.occupy_uid == role_id 
         and v.occupy_plat_type == self.my_info.plat_type then 
            table.insert(list,v)
        end 
    end 
    return list
end 

function LandsBetweenData:GetMyNetAlterStr()
    local num = #self:GetMyNetAlter()
    local max=GodPosData.Instance:GodPosCfg().occupy
    return num.."/"..max
end 

function LandsBetweenData:IsMyNetAlter(link_mark) 
    local mark = self.net_alter_info.list[link_mark]

    return mark ~= nil and mark.occupy_uid == RoleData.Instance:GetRoleId() 
        and self.my_info.plat_type == mark.occupy_plat_type
end 

-- 输入link_mark(uid+plat_type)获取是不是我自己
function LandsBetweenData:IsMyRole(uid,plat_type)
    local role_id = RoleData.Instance:GetRoleId()
    return uid == role_id and plat_type == self.my_info.plat_type
end

-- 获取神域是否开启
function LandsBetweenData:IsOpen()
    return self:GetCurStatus() <=4  and self:GetCurStatus() >= 3--self.base_info.is_open == 1 
end
-- 返回基础信息
function LandsBetweenData:GetBaseInfo()
    return self.base_info
end

-- 复合link
function LandsBetweenData.CfgLink(param)
	local str = ""
	for i = 1,#param do 
		str = str..param[i]..(i == #param and "" or "|")
	end  
	return str
end

-- 解link
function LandsBetweenData.FixLink(str)
	local list = Split(str,"|")
	local result = {}
	for i = 1, #list do 
		table.insert(result,tonumber(list[i]))
	end 
	return result
end


function LandsBetweenData:TestOne()
    local res_path = ShenYuProbeData.Instance:GetRockResPath()
    local center_link_mark = LandsBetweenData.CfgLink({[1] = 141,[2] = 6})

            
    LandsBetweenData.Instance:SetResOfSpPicByLinkMark({
        res_path = res_path,
        name = "测试矿石2",
        link_mark = center_link_mark,
        ui_show_sp_sub = "shen_yu_probe_sub",
        around = {center_link_mark},
        center_link_mark = center_link_mark,
        extral_info = nil,
        faction = -1,
        task_type = ShenYuProbeCtrl.TaskType.Role,
        enable_func = function ()
            return true
        end
    },center_link_mark)
end

function LandsBetweenData:TestTwo()
    local center_link_mark = LandsBetweenData.CfgLink({[1] = 141,[2] = 6})
    LandsBetweenData.Instance:SetResOfSpPicByLinkMark(nil,center_link_mark)
end

function LandsBetweenData:TestThree()
    local center_link_mark = LandsBetweenData.CfgLink({[1] = 140,[2] = 5})
    LandsBetweenData.Instance:SetResOfSpPicByLinkMark(nil,center_link_mark)
    -- self:JumpToYourActSeqGrid(1)
end

function LandsBetweenData:TestFour()
    self:SetMapClick({x = 140,y = 6})
end

function LandsBetweenData:TestFive()
    return "57|14"
end

-- 记录并进行激进的屏蔽操作
function LandsBetweenData:SetShowMark()
    self.follow_mark.role_setting = {
		is_show =RoleSettingData.Instance:GetData(ROLE_SETTING_TYPE.SET_SHOW_OTHER_PLAYER),
		show_num = RoleSettingData.Instance:GetData(ROLE_SETTING_TYPE.SET_NUMBER_OF_SCREENS),
    }

    RoleSettingData.Instance:SetData(ROLE_SETTING_TYPE.SET_NUMBER_OF_SCREENS, 10)
    RoleSettingData.Instance:SetData(ROLE_SETTING_TYPE.SET_SHOW_OTHER_PLAYER, 1)
	RoleSettingCtrl.Instance:SaveRoleSettingInfo()
end

-- 解除屏蔽操作
function LandsBetweenData:EndShowMark()
    RoleSettingData.Instance:SetData(ROLE_SETTING_TYPE.SET_NUMBER_OF_SCREENS,self.follow_mark.role_setting.show_num)
    RoleSettingData.Instance:SetData(ROLE_SETTING_TYPE.SET_SHOW_OTHER_PLAYER, self.follow_mark.role_setting.is_show)
	RoleSettingCtrl.Instance:SaveRoleSettingInfo()
	self.follow_mark.role_setting = nil
end

function LandsBetweenData:GetShareStr()
    local map_pic = self:GetMapPicClick()
    local pos_name = (self:IsAltar(map_pic.static.map_show) and Language.LandsBetween.Desc.PosStrType[1] or Language.LandsBetween.Desc.PosStrType[0])    
    local pos_mark = pos_name.."{landsbetween_share_pos;"..map_pic.static.link_mark..";"..self.operating_info.type.."}"
    local my_name = RoleData.Instance:GetRoleName()

    return string.format(Language.LandsBetween.Desc.ShareFrontStr,my_name)..pos_mark
end

function LandsBetweenData:GetActShareStr(act_type)
    local map_pic = self:GetMapPicClick()
    local config = Config.divine_domain_function_auto.daily[act_type]
    local pos_name = (self:IsAltar(map_pic.static.map_show) and Language.LandsBetween.Desc.PosStrType[1] or Language.LandsBetween.Desc.PosStrType[0])    
    if config ~= nil then 
        pos_name = config.name_text
    end 

    local pos_mark = pos_name.."{landsbetween_share_pos;"..map_pic.static.link_mark..";"..self.operating_info.type.."}"
    local my_name = RoleData.Instance:GetRoleName()

    return string.format(Language.LandsBetween.Desc.ShareFrontStr,my_name)..pos_mark
end