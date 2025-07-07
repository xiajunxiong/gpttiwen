LandsBetweenData = LandsBetweenData or BaseClass()

function LandsBetweenData:__init()
    if LandsBetweenData.Instance ~= nil then
        Debuger.LogError("[LandsBetweenData] attempt to create singleton twice!")
        return
    end

    LandsBetweenData.Instance = self

    self.base_info = SmartData.New()-- 5501同步sm
    self.my_info = SmartData.New()-- 5501同步sm

    self.net_map_info = SmartData.New({list = {},flush = 0})-- 5504同步sm -- 地块信息
    self.net_alter_info = SmartData.New({list = {},flush = 0})-- 5504同步sm -- 神坛信息
    self.net_role_info = SmartData.New({list = {},flush = 0})-- 5505同步sm -- 角色信息
    self.net_map_show_info = {list = {},index_list = {},link_mark_list = {}}  -- 小地图的操作列表

    self.brief_info = SmartData.New({list = {},flush = 0})-- 5511同步sm -- 精华简要信息
    self.report_info = SmartData.New({list = {},delete_list = {},flush = 0})-- 5512,5513同步sm -- 战报信息
    self.faction_user = SmartData.New({list = {},flush = 0})-- 5516 同步sm -- 同阵营玩家
    self.faction_base = SmartData.New({})-- 5517 同步sm -- 同阵营信息
    self.phasetask_info = SmartData.New({flush = 0})-- 5523 同步sm -- 阶段任务信息
    -- type == 0 and 外域 or 内域
    self.operating_info = SmartData.New({obj = nil,type = 0,force_flush = 0,force_flush_visible = 0, force_close_operate = 0,jump = nil,flush_extra = 0,draging = 0})
    self.extra_list = {}
    self.draging = false

    self.max_length = {In = 150, Out = 110}
    self.length_pic = 2.6

    -- 具体的静态客户端地图
    self.static_map_out = {}
    self.static_map_in = {}

    -- 静态数位-link_mark 
    self.index_map_out = {}
    self.index_map_in = {}

    -- 根据区块提取可视区域判定点组
    self.visible_group = {
        In = {},
        Out = {},
    }

    self.visible_link_mark_to_index = {
        In = {},
        Out = {},
    }
    self.cache_visible_map = {}
    self.visible_length = {}
    self.map_cahce_info = {}

    self.main_city = {}

    self.cur_role_pos = Vector2.New(0, 0)
    self.extra_mark = {}
    self.report_new = {}
    self.alter_cfg = {}
    -- self.extral_activity = {}
    
    self.total_alter_num = {}
    self.act_show_param = {}

    self.sp_pic_mark = {}
	self.follow_mark = {}
    
    self.force_link_mark_trans = {}

    -- 小地图用视野信息
    self.map_view_visible = {
        In = {},
        Out = {},
    }

    self.open_call_func = {}
    self.close_call_func = {}

    self:InitConfig()
    self:InitExtralAct()
    -- self:InitExtralGrid()

    -- 慢启动标记
    self.slow_start = SmartData.New({flag = false})

    -- 处理中的资源
    self.map_running_mark = 0

    self.irony_tips_pos = {
        [1] = 396,
        [2] = 397,
        [3] = 398,
        [4] = 399,
        [5] = 400,
        [6] = 401,
        [7] = 402,
        [8] = 403,
        [9] = 404,
    }
end

function LandsBetweenData:InitConfig()
    -- 静态地图注入
    local index = 1
    local cfg_1 = Config.divine_domain_terrainwy_auto
    for i = 0,#cfg_1-1 do 
        for j = 0,109 do 
            local link_mark = self.CfgLink({[1] = i,[2] = j})
            local vo = {}
            vo.map_stamp = cfg_1[i+1]["c"..j]
            vo.link_mark = link_mark
            for k,v in pairs(Config.divine_domain_system_auto.other) do 
                if v.lot_number == vo.map_stamp then 
                    vo.map_cfg = v
                    break
                end 
            end 

            vo.type = 0
            vo.can_operate = GoldLaw.Operate[vo.map_stamp] ~= nil

            self.static_map_out[link_mark] = vo 
            self.index_map_out[index] = link_mark
            index = index +1
        end 
    end 

    index = 1
    local cfg_2 = Config.divine_domain_terrainny_auto
    for i = 0,#cfg_2-1 do 
        for j = 0,149 do 
            local link_mark = self.CfgLink({[1] = i,[2] = j})
            local vo = {}
            vo.map_stamp = cfg_2[i+1]["c"..j]
            vo.link_mark = link_mark
            for k,v in pairs(Config.divine_domain_system_auto.other) do 
                if v.lot_number == vo.map_stamp then 
                    vo.map_cfg = v
                    break
                end 
            end 

            vo.type = 1
            vo.can_operate = GoldLaw.Operate[vo.map_stamp] ~= nil
            self.static_map_in[link_mark] = vo
            self.index_map_in[index] = link_mark
            index = index +1
        end 
    end

    local index = 1
    local extra_pos = math.floor(GoldLaw.OutLimit % GoldLaw.VisiLength) > 0 
    local v_length = extra_pos and math.floor(GoldLaw.OutLimit / GoldLaw.VisiLength) + 1 or math.floor(GoldLaw.OutLimit / GoldLaw.VisiLength)

    for i = 0,v_length-1 do 
        for j = 0,v_length-1 do 
            local vo = {}
            vo.group_id = index
            vo.x = j
            vo.y = i
            vo.type = 0
            vo.marked_list = {}
            table.insert(vo.marked_list,  {x = GoldLaw.VisiLength * vo.x ,y = GoldLaw.VisiLength * vo.y})
            table.insert(vo.marked_list,  {x = GoldLaw.VisiLength * (vo.x+1)-1 ,y = GoldLaw.VisiLength * vo.y})
            table.insert(vo.marked_list,  {x = GoldLaw.VisiLength * vo.x ,y = GoldLaw.VisiLength * (vo.y+1)-1})
            table.insert(vo.marked_list,  {x = GoldLaw.VisiLength * (vo.x+1)-1 ,y = GoldLaw.VisiLength * (vo.y+1)-1})

            -- 为静态地图信息追加视野id
            for y = vo.marked_list[1].y , vo.marked_list[3].y do 
                for x = vo.marked_list[1].x , vo.marked_list[2].x do 
                    local link_mark = self.CfgLink({[1] = x,[2] = y})
                    if self.static_map_out[link_mark] then
                        self.static_map_out[link_mark].visible_area = index

                        self:InitMainCity(self.static_map_out[link_mark])
                        self:InitAlterNum(link_mark,vo.type)
                    end
                end 
            end 

            -- 存在多出来的格子
            if extra_pos then end 
            table.insert(self.visible_group.Out,vo)
            local link_mark = self.CfgLink({[1] = vo.x,[2] = vo.y})
            self.visible_link_mark_to_index.Out[link_mark] = index

            local check_poses = {}
            for check_index = 1,4 do 
                check_poses[check_index] = self:FixMapViewPicPos(vo.marked_list[check_index].x,vo.marked_list[check_index].y,0)
            end 
            self.map_view_visible.Out[link_mark] = {
                marked_list = vo.marked_list,
                marked_poses = check_poses,
                x = vo.x,
                y = vo.y,
            }

            index = 1 + index
        end 
    end 

    self.visible_length[0] = v_length

    index = 1
    extra_pos = math.floor(GoldLaw.InLimit % GoldLaw.VisiLength) > 0 
    v_length = extra_pos and math.floor(GoldLaw.InLimit / GoldLaw.VisiLength) + 1 or math.floor(GoldLaw.InLimit / GoldLaw.VisiLength)

    for i = 0,v_length-1 do 
        for j = 0,v_length-1 do 
            local vo = {}
            vo.group_id = index
            vo.x = j
            vo.y = i
            vo.type = 1
            vo.marked_list = {}
            table.insert(vo.marked_list,  {x = GoldLaw.VisiLength * vo.x ,y = GoldLaw.VisiLength * vo.y})
            table.insert(vo.marked_list,  {x = GoldLaw.VisiLength * (vo.x+1)-1 ,y = GoldLaw.VisiLength * vo.y})
            table.insert(vo.marked_list,  {x = GoldLaw.VisiLength * vo.x ,y = GoldLaw.VisiLength * (vo.y+1)-1})
            table.insert(vo.marked_list,  {x = GoldLaw.VisiLength * (vo.x+1)-1 ,y = GoldLaw.VisiLength * (vo.y+1)-1})

            -- 为静态地图信息追加视野id
            for y = vo.marked_list[1].y , vo.marked_list[3].y do 
                for x = vo.marked_list[1].x , vo.marked_list[2].x do 
                    local link_mark = self.CfgLink({[1] = x,[2] = y})
                    if self.static_map_in[link_mark] then
                        self.static_map_in[link_mark].visible_area = index

                        self:InitMainCity(self.static_map_in[link_mark])
                        self:InitAlterNum(link_mark,vo.type)
                    end 
                end 
            end 

            -- 存在多出来的格子
            if extra_pos then end 

            table.insert(self.visible_group.In,vo)
            local link_mark = self.CfgLink({[1] = vo.x,[2] = vo.y})
            self.visible_link_mark_to_index.In[link_mark] = index

            local check_poses = {}
            for check_index = 1,4 do 
                check_poses[check_index] = self:FixMapViewPicPos(vo.marked_list[check_index].x,vo.marked_list[check_index].y,1)
            end 
            self.map_view_visible.In[link_mark] = {
                marked_list = vo.marked_list,
                marked_poses = check_poses,
                x = vo.x,
                y = vo.y,
            }

            index = 1 + index
        end 
    end 
    

    self.visible_length[1] = v_length

    for k,v in pairs(Config.divine_domain_system_auto.altar) do 
        self.alter_cfg[v.altar_level] = v
    end 

    self.map_resources_list = KeyList(Config.divine_domain_system_auto.map_resources,"lot_number")

end

function LandsBetweenData:SetMainCallFunc(type,mark,func)
    if type == GoldLaw.CallBackType.Open then 
        self.open_call_func[mark] = func
    else 
        self.close_call_func[mark] = func
    end 
end

function LandsBetweenData:MainOpenCall()
    for k,v in pairs(self.open_call_func) do 
        v()
    end 
end

function LandsBetweenData:MainCloseCall()
    for k,v in pairs(self.close_call_func) do 
        v()
    end 
end

function LandsBetweenData:FlushExtralList()
    self.extra_list = {}

    for k,v in pairs(Config.divine_domain_function_auto.main_screen_button) do 
        local act_cfg = nil
        for i,j in pairs(self.act_show_param) do 
            local temp_cfg = self:GetLandsBetweenActCfgByActType(j.act_type)
            if temp_cfg.mod_key== v.open then
                act_cfg = j
                break 
            end
        end 

        if FunOpen.Instance:GetFunIsOpened(v.open) then 
            -- and ((act_cfg == nil) 
            -- or (act_cfg ~= nil and act_cfg.act_show_func == nil) 
            -- or (act_cfg ~= nil and act_cfg.act_show_func ~= nil and act_cfg.act_show_func()) ) then
            local vo = {}
            vo.place = v.place
            vo.name = v.text
            vo.sprite = v.sprite
            vo.mod = v.mod
            vo.open = v.open
    
            table.insert(self.extra_list,vo)
        end 
    end 

    self.extra_out_list = {}
    for k,v in pairs(Config.divine_domain_function_auto.main_screen_button) do 
        local act_cfg = nil
        -- for i,j in pairs(self.act_show_param) do 
        --     local temp_cfg = self:GetLandsBetweenActCfgByActType(j.act_type)
        --     if temp_cfg.mod_key== v.open then
        --         act_cfg = j
        --         break 
        --     end
        -- end 

        if v.out_show == 1 and FunOpen.Instance:GetFunIsOpened(v.open) then
            -- and ((act_cfg == nil) 
            -- or (act_cfg ~= nil and act_cfg.act_show_func == nil) 
            -- or (act_cfg ~= nil and act_cfg.act_show_func ~= nil and act_cfg.act_show_func()) ) then

            local vo = {}
            vo.place = v.place
            vo.name = v.text
            vo.sprite = v.sprite
            vo.mod = v.mod
            vo.open = v.open

            table.insert(self.extra_out_list,vo)
        end 
    end 
end

-- 通过视野的link_mark获取index
function LandsBetweenData:CheckVisibleIndexByLinkMark(link_mark,type)
    if type == 0 then 
        return self.visible_link_mark_to_index.Out[link_mark]
    else 
        return self.visible_link_mark_to_index.In[link_mark]
    end 
    -- local fix_link = self.FixLink(link_mark)
    -- local link_mark = self.CfgLink({[1] = x,[2] = y})
end

-- 获取地图最大格数
function LandsBetweenData:SimpleGetMaxMap(type)
    if type == 0 then 
        return self.max_length.Out *self.max_length.Out
    else 
        return self.max_length.In *self.max_length.In
    end 
end

function LandsBetweenData:GetStaticGridLinkMarkToIndex(index,type)
    if type == 0 then 
        return self.index_map_out[index]
    else
        return self.index_map_in[index]
    end 
end

-- mod_key 
-- target 刷新检查data 
-- func 更新需要的func 返回0 or 1
function LandsBetweenData:ExtraRemindMark(param)
    self.extra_mark[param.mod_key] = param
end

function LandsBetweenData:GetExtraRemindMark(mod_key)
    return self.extra_mark[mod_key]
end

function LandsBetweenData:GetExtralList()
    self:FlushExtralList()

    return self.extra_list
end

function LandsBetweenData:GetExtralOutList()
    self:FlushExtralList()

    return self.extra_out_list
end

-- 根据活动类型与阵营获取资源配置
function LandsBetweenData:GetLandsBetweenResCfgByActType(act_type,faction)
    for k,v in pairs(self.map_resources_list) do 
        if v.act_type ~= "" and act_type == v.act_type and ((faction ~= nil and faction == v.camp_type) or  faction == nil) then 
            return v
        end 
    end 

    return nil
end

-- 通过活动类型获取活动配置
function LandsBetweenData:GetLandsBetweenActCfgByActType(act_type)
    for k,v in pairs(Config.divine_domain_function_auto.daily) do 
        if act_type == v.act_type then 
            return v
        end 
    end  
end

-- 初始化活动
-- 对非自定义的活动进行整合生成
-- 最基础的初始化方案不生成红点！
function LandsBetweenData:InitExtralAct()
    for k,v in pairs(Config.divine_domain_function_auto.daily) do 
        if not LandsBetweenExtralCustom[v.act_type] then 
            -- 注册活动图标
            local param = {}
            param.act_type = v.act_type
            param.act_seq = v.act_seq
            param.listen_data = self.base_info
            param.remin_data = BindTool.Bind(self.ExtralActRemindData, self, v)  -- 不注册红点
            param.remind_func = BindTool.Bind(self.ExtralActRemindFunc, self, v)
            param.click_func = BindTool.Bind(self.ExtralActClick, self,v.act_type)
            param.time_show_func = BindTool.Bind(self.ExtralActTimeShow, self, v.act_type)
            param.act_show_func = BindTool.Bind(self.ExtralActShow, self,v.act_type)
            param.act_status_func = BindTool.Bind(self.ExtralActStatusShow, self,v.act_type)
            param.show_sp_sub = nil
            param.with_jump = false

            -- 注册活动指定地块
            local need_faction = v.quantity > 1
            for i = 1 , v.quantity do 
                local param_t = {}
                -- 补齐了再谈
                param_t.act_type = v.act_type
                param_t.act_seq = v.act_seq

                local res_cfg = self:GetLandsBetweenResCfgByActType(v.act_type,need_faction and i-1 or nil)
                local res_path = res_cfg.route ~= "" and res_cfg.route.."/"..res_cfg.corresponding_model..".prefab" 
                    or string.format(GoldLaw.ResPath,res_cfg.corresponding_model)
                param_t.res_path = res_path

                param_t.name = v.name_text

                param_t.link_mark = "0|0"
                param_t.center_link_mark = "0|0"
                param_t.around = {}
                local need_grid = false
                for index, pos_cfg in pairs(Config.divine_domain_function_auto.position) do
                    if pos_cfg.position_id == v["position_"..i] then  
                        -- 目前客户端针对活动静态模型生成点取中心点！（3X3 or 5X5)
                        local generate_pos = self.FixLink(pos_cfg.position_generate)
                        local center_core = self.FixLink(pos_cfg.position_core)
                        local center_link_mark = LandsBetweenData.CfgLink({[1] = generate_pos[1] + center_core[1] -1 
                            ,[2] = generate_pos[2] + center_core[2] -1})    

                        param_t.link_mark = center_link_mark
                        param_t.center_link_mark = center_link_mark

                        local around = {}
                        for i = generate_pos[1] ,generate_pos[1] + center_core[1] do 
                            for j = generate_pos[2] ,generate_pos[2] + center_core[2]  do 
                                local link_mark = LandsBetweenData.CfgLink({[1] = i ,[2] = j})    
                                table.insert(around,link_mark)
                            end 
                        end 

                        param_t.around = around
                        need_grid = true
                    end 
                end 

                param_t.enable_func = BindTool.Bind(self.ExtralSpGridEnable, self,v.act_type)
                if not StringIsEmpty(v.ui_show_sp_sub) then
                    param_t.ui_show_sp_sub = v.ui_show_sp_sub
                else
                    param_t.ui_show_sp_sub = nil
                end
                param_t.big_size = not (v.act_type == ActType.Ruins or v.act_type == ActType.Palaces or v.act_type == ActType.Animals)
                param_t.extral_info = {
                    name = Language.LandsBetween.Desc.DefaultOperate,
                    func = function()
                        if not StringIsEmpty(v.act_go_click) then
                            self[v.act_go_click]()
                            return
                        end
                        ViewMgr:OpenViewByKey(v.mod_key)
                    end,
                    share_func = function()
                        self:ExtralShareClick(v.act_type)
                    end
                }

                param_t.faction = need_faction and i - 1 or -1

                param_t.map_sp_name = v["ui_model_decals_"..i] ~= "" and v["ui_model_decals_"..i] or nil
                param_t.map_sp_fix = v["map_position_"..i] == "" and nil or Config.divine_domain_function_auto.map_location[v["map_position_"..i]]
                if need_grid then 
                    self:SetResOfSpPicByLinkMark(param_t,param_t.center_link_mark)
                end 

                param.with_jump = true
            end 

            self:SetExtralActShowParam(param)
        end 
    end 
end
function LandsBetweenData:ExtralActRemindData(info)
    local data = self:GetExtraRemindMark(info.mod_key)
    if data then
         return data.target
    end
    --[[ if info.act_type == ActType.Ruins then
        return GodAreasActsData.Instance.boss_info
    end ]]
    if info.act_type == ActType.Animals then
        return GodAreasActsData.Instance:GetBossInfo()
    end
    return nil
end
function LandsBetweenData:ExtralActRemindFunc(info)
   local data = self:GetExtraRemindMark(info.mod_key)
   if data then
        return data.func
   end
   --[[ if info.act_type == ActType.Ruins then
        return function ()
            return 1
        end
   end ]]
   if info.act_type == ActType.Animals then
        return BindTool.Bind(self.CheckAnimalsRemind, self)
   end
   return nil
end
function LandsBetweenData:CheckAnimalsRemind()
    return GodAreasActsData.Instance:AnimalsRemind()
end

-- -- 注入活动特殊地块
-- function LandsBetweenData:InitExtralGrid()
--     self.extral_activity = {}
--     for k,v in pairs(Config.divine_domain_function_auto.daily) do 
--         self.extral_activity[v.act_seq] = TableCopy(v)
--         self.extral_activity[v.act_seq].links = {}
--         self.extral_activity[v.act_seq].is_rand = v.quantity > 0 
--         for i = 1,v.quantity do 
--             local split = self.FixLink(v["position_"..i])

--             local link_mark = self.CfgLink({[1] = split[1],[2] = split[2]})
--             local vo = {link_mark = link_mark,faction = split[3] or 0}
--             table.insert(self.extral_activity[v.act_seq].links,vo)
--         end 
--     end 
-- end

-- -- 注入特殊地块的启动func
-- -- param_t = {act_seq , name ,func}
-- -- act_seq 活动seq
-- -- name 操作的名字
-- -- func 操作的func
-- -- 静止的特殊地块
-- function LandsBetweenData:SetExtralOperateFunc(param_t)
--     self.extral_activity[param_t.act_seq].operate = param_t
-- end

-- 先用着吧，复杂就复杂
-- 注册图标规则
-- param_t = {act_seq,listen_data,remin_data,remind_func,click_func,time_show_func,act_show_func,act_status_func,is_show_sp}
-- act_seq         活动id
-- listen_data     主要监听 smart_data
-- remin_data      红点监听 smart_data
-- remind_func     红点显隐规则 0 不显示 1 显示
-- click_func      点击逻辑
-- time_show_func  倒计时显示规则 用时间戳
-- act_show_func   活动展示逻辑 false 不展示 true 展示
-- act_status_func 活动状态展示 1 等待中 2 启动中 其他 不展示
-- show_sp_sub     需要启动特殊逻辑界面吗
function LandsBetweenData:SetExtralActShowParam(param_t)
    self.act_show_param[param_t.act_type] = param_t
end

-- 获取活动图标显隐逻辑组
function LandsBetweenData:GetExtralActShowParam(act_type)
    return self.act_show_param[act_type]
end

-- 单独更换活动图标某个参数
function LandsBetweenData:SetExtralActShowParamDeatil(act_type,sub,param)
    if self.act_show_param[act_type] == nil then 
        return 
    end 

    self.act_show_param[act_type].sub = param
end

-- 获取现在应该要展示活动icon
-- 只有当前的阶段应该展示时才会被生成
function LandsBetweenData:GetLandsBetweenActList()
    local list = {}
    for  k,v in pairs(Config.divine_domain_function_auto.daily) do 
        if self.act_show_param[v.act_type] ~= nil then 
            -- if (self:GetCurMapType() + 1) == v.divine_realm_stage then 
                table.insert(list,v)
            -- end 
        end 
    end 
    return list
end

-- 设置特殊资源地图快
-- param {res_path,name,link_mark}
-- act_type         活动类型 
-- act_seq          活动编号
-- res_path         资源地址，就按这个生成！
-- name             名字，主要测试用，可以没有
-- link_mark        地块位置
-- ui_show_sp_sub   主界面上的地块附加信息界面 = 预设lands_between_act_map_show中的sub名字
-- around           同效地块的link_mark
-- center_link_mark 中心地块 
-- extral_info      特殊地块逻辑 name = 操作的名字 func 操作的函数 pic = 操作图标 tips_func 说明func share_func分享func
-- faction          该块区的阵营 -1 为无阵营限制 0~3 4阵营
-- enable_func      该块区的有效判定
function LandsBetweenData:SetResOfSpPicByLinkMark(param,link_mark,is_flush)
    -- LogDG("link_mark",link_mark,param)
    self.sp_pic_mark[link_mark] = param

    if is_flush and ViewMgr:IsOpen(LandsBetweenMainView) then         
        self:ForceFlushVisibleOperate()
    end 
end

-- 设置特殊资源地图块，列表型
-- 注意，key = link_mark value = param
function LandsBetweenData:SetResOfSpPicByLinkMarkListVer(param_list,is_flush)
    for k,v in pairs(param_list) do
        self.sp_pic_mark[k] = v
    end

    if is_flush and ViewMgr:IsOpen(LandsBetweenMainView) then         
        self:ForceFlushVisibleOperate()
    end 
end 

function LandsBetweenData:GetResOfSpPicByLinkMark(link_mark)
    return self.sp_pic_mark[link_mark]
end

-- 通过link_mark检索是否为有效的活动特殊地块
function LandsBetweenData:CheckSpPicAroundByLinkMark(link_mark)
    for k,v in pairs(self.sp_pic_mark) do 
        for i,j in pairs(v.around or {}) do 
            if j == link_mark then 
                return v
            end 
        end 
    end 

    return nil
end

-- function LandsBetweenData:Get()
    
-- end

-- 通过活动类型获取特殊地块信息组
function LandsBetweenData:CheckSpPicAroundByActType(act_type, faction)
    for k,v in pairs(self.sp_pic_mark) do
        if act_type == ActType.Animals then
            if v.act_type == act_type and v.faction == faction then 
                return v
            end 
        else
            if v.act_type == act_type then 
                return v
            end 
        end
    end 

    return nil
end

-- 活动展示逻辑
function LandsBetweenData:ExtralActShow(act_type)
    local act_cfg = self:GetLandsBetweenActCfgByActType(act_type)

    local start,complete = self:GetExtralActTimeStamp(act_type)
    local cur = TimeHelper.GetServerTime()

    return act_cfg.divine_realm_stage == self:GetCurMapType() + 1 and complete > cur
end

-- 获取活动开启和结束的时间戳
function LandsBetweenData:GetExtralActTimeStamp(act_type)
    if self.base_info.season_index == nil then 
        return 0,0
    end 

    local act_cfg = self:GetLandsBetweenActCfgByActType(act_type)

    local area_start = TimeManager.DayStartAny(
        act_cfg.divine_realm_stage == 1 and self.base_info.ourter_space_timestamp  or 
        self.base_info.inner_space_timestamp) 

    if area_start == nil then 
        return 0,0
    end 

    local split_start = Split( act_cfg.begin_time ,":")
    local hour_second = tonumber(split_start[1]) * TIME_HOUR_SECOND_NUM
    local min_second = tonumber(split_start[2]) * TIME_MINUTE_SECOND_NUM
    local day_second = (act_cfg.activity_time-1) * TIME_DAY_SECOND_NUM
    
    local stamp_start = area_start + day_second + hour_second + min_second
    -- LogError("?DSA",act_type,os.date("*t",stamp_start))

    local split_end = Split( act_cfg.end_time ,":")
    hour_second = tonumber(split_end[1]) * TIME_HOUR_SECOND_NUM
    min_second = tonumber(split_end[2]) * TIME_MINUTE_SECOND_NUM
    day_second = (act_cfg.open_day+act_cfg.activity_time-2) * TIME_DAY_SECOND_NUM

    local stamp_end = area_start + day_second + hour_second + min_second
    -- LogError("?DSA end",act_type,os.date("*t",stamp_end))

    return stamp_start,stamp_end
end

-- 活动时间展示逻辑
function LandsBetweenData:ExtralActTimeShow(act_type)
    local act_cfg = self:GetLandsBetweenActCfgByActType(act_type)

    if act_type == ActType.Probe then
        return 0
    end

    local start,complete = self:GetExtralActTimeStamp(act_type)
    local cur = TimeHelper.GetServerTime()

    if act_cfg.divine_realm_stage == self:GetCurMapType() + 1 then 
        if cur > start then 
            return complete
        elseif cur < start then 
            return start
        end 
    else 
        return 0
    end 
end

-- 活动状态展示逻辑
function LandsBetweenData:ExtralActStatusShow(act_type)
    local act_cfg = self:GetLandsBetweenActCfgByActType(act_type)

    local start,complete = self:GetExtralActTimeStamp(act_type)
    local cur = TimeHelper.GetServerTime()

    if act_type == ActType.Probe then
        return true
    end
    if act_type == ActType.Animals then
        return GodAreasActsData.Instance:AnimalsIsOpen()
    end
    if act_cfg.divine_realm_stage == self:GetCurMapType() + 1 then 
        if cur > start and cur < complete then 
            return GoldLaw.ActOpen
        elseif cur < start then 
            return GoldLaw.ActWait
        end 
    else 
        return 0
    end 
end

-- 活动图标点击逻辑
function LandsBetweenData:ExtralActClick(act_type)
    local act_cfg = self:GetLandsBetweenActCfgByActType(act_type)
    local param = self:GetExtralActShowParam(act_type)
    if act_type == ActType.Palaces or act_type == ActType.Ruins then
        ViewMgr:OpenViewByKey(act_cfg.mod_key)
        return
    end
    if not StringIsEmpty(act_cfg.act_icon_click) then
        self[act_cfg.act_icon_click]()
        return
    end

    if self:ExtralActStatusShow(act_type) ~=  GoldLaw.ActOpen 
        and act_type ~= ActType.Probe then 
        PublicPopupCtrl.Instance:Center(Language.LandsBetween.Desc.BtnActUnOpen)
        return 
    end 

    if param.with_jump then 
        local pos_info =self:CheckSpPicAroundByActType(act_type, self.my_info.faction or -1)
        local fix_link = self.FixLink(pos_info.center_link_mark)

        local pos = {x = fix_link[1],y = fix_link[2]}
        self:SetMapClick(pos)
        self:SetMapJumpClick(pos)
        -- TimeHelper:AddDelayTimer(function()
            
        -- end,1)
    else 
        ViewMgr:OpenViewByKey(act_cfg.mod_key)
    end 
end

function LandsBetweenData:ExtralShareClick(act_type)
    local str = self:GetActShareStr(act_type)
    ChatCtrl.Instance:SendChannelChatText(str,nil, ChatConfig.ChannelType.landsbetween)
end

---------------------------------------配置活动配表方法-----------------------------
function LandsBetweenData:ruins_act_icon_click()
    ViewMgr:OpenViewByKey(Mod.GodAreasActs.Ruins)
end
function LandsBetweenData:palaces_act_icon_click()
    ViewMgr:OpenViewByKey(Mod.GodAreasActs.Palaces)
end

function LandsBetweenData:ruins_act_go_click()
    if RuinsData.Instance:IsOpen()then
        RuinsCtrl.Instance:EnterScene()
    else
        PublicPopupCtrl.Instance:Center(Language.GodAreasActs.ActTip)
    end
end
function LandsBetweenData:palaces_act_go_click()
    if PalacesData.Instance:IsOpen()then
        PalacesCtrl.Instance:EnterScene()
    else
        PublicPopupCtrl.Instance:Center(Language.GodAreasActs.ActTip)
    end
end

function LandsBetweenData:ruins_model_show()
    local is_open = RuinsData.Instance:ModelOpen()
    return is_open
end
function LandsBetweenData:palaces_model_show()
    local is_open = PalacesData.Instance:ModelOpen()
    return is_open
end
function LandsBetweenData:animals_model_show()
    local base_info = LandsBetweenData.Instance:GetBaseInfo()
    return base_info.cur_status == GOD_SPACE_STATUS.InnerOpen
end
----------------------------------------------------------------------------------------

-- 活动地块有效检查
function LandsBetweenData:ExtralSpGridEnable(act_type)
    local start,complete = self:GetExtralActTimeStamp(act_type)
    local cur = TimeHelper.GetServerTime()    
    if act_type == ActType.Probe then
        return true
    end
    local act_cfg = self:GetLandsBetweenActCfgByActType(act_type)
    if act_cfg and not StringIsEmpty(act_cfg.act_model_show) then
        return self[act_cfg.act_model_show]()
    end
    return cur >= start and cur <= complete
end

-- 测试代码
function LandsBetweenData:CacheMap(index)
    local length = index == 2 and 149 or 109
    local cfg = Config.divine_domain_system_auto["map_point_"..index]
    
    local str = ""
    for i = 0,#cfg-1 do 
        for j = 0,length do 
            str = str .. cfg[i]["c"..j]..(j == length and "\n" or "|")
        end 
        str = str .."@"
    end 

    UnityEngine.GUIUtility.systemCopyBuffer = str
end

-- 获取神坛信息列表
function LandsBetweenData:GetAlterInfoList()
    local list = self:GetMyNetAlter() -- 先提取属于我的协议神坛，然后补充静态信息并输出
    local fix_list = {}
    for k,v in pairs(list) do
        local link_mark = self.CfgLink({[1] = v.row ,[2] = v.column})
        local vo = TableCopy(self:GetMapGridStaticInfo(link_mark))
        vo.net_info = TableCopy(v)
        vo.level = self:GetAlterGridLevel({x = v.row, y = v.column})
        table.insert(fix_list,vo)
    end 
    return fix_list
end

-- 获取神坛奖励信息
function LandsBetweenData:GetAlterInfoRewardList()
    local list = {}
    for k,v in pairs(self.brief_info.unfetch_list or {}) do 
        local item = Item.Init(v.item_id,v.num)
        table.insert(list,item)
    end 
    if self.brief_info.unfetch_essence_num and self.brief_info.unfetch_essence_num > 0 then 
        table.insert(list,Item.Create({item_id = CommonItemId.GodSpaceEssence,num = self.brief_info.unfetch_essence_num}))
    end 

    if self.brief_info.unfetch_guaji_essence and self.brief_info.unfetch_guaji_essence > 0 then 
        table.insert(list,Item.Create({item_id = CommonItemId.GodSpaceEssence,num = self.brief_info.unfetch_guaji_essence}))
    end 

    return list
end

function LandsBetweenData:GetCityParam()
    local lands = 0
    for k,v in pairs(self.net_map_info.list) do 
        if v.faction == self.my_info.faction then 
            lands = lands + 1
        end 
    end 

    for k,v in pairs(self.net_alter_info.list) do 
        if v.faction == self.my_info.faction then 
            lands = lands + 1
        end 
    end 

    return {
        name = string.format(Language.LandsBetween.Desc.CityName[self:GetCurMapType()],Language.LandsBetween.FactionName[self.my_info.faction]),
        member =  self.faction_user.total_num,
        group_name = self.faction_base.server_num or 0,
        empty_lands = (self.faction_base.occupy_glade_num or 0).."/"..(self.faction_base.max_glade_num or 0 ),
        hold_on =  self.brief_info.guaji_essence_per_minute..Language.LandsBetween.Desc.PointPreMin ,
        icon_str = self:GetMyCityIconStr()
    }
end

function LandsBetweenData:GetMemberList()
    -- 
    local list = {}
    for i = 1,#self.faction_user.list do 
        local vo = {}
        vo.index = i
        vo.name = self.faction_user.list[i].user_name

        local server_name = ""
        -- 测试代码
        -- local account_key = LoginData.Instance:GetAccountKey()
        -- local last_login_server = LoginData.Instance:getAccountCacheData(account_key).last_login_server
        local server_info = LoginData.Instance:GetServerInfoById(self.faction_user.list[i].server_id)
        if IS_EDITOR then
            server_name = string.format(Language.Login.ServerNameEditor, server_info.id, server_info.name)
        else
            server_name = string.format(Language.Login.ServerName, server_info.name)
        end
        vo.guild = server_name
        vo.score = self.faction_user.list[i].capability
        vo.avatar_type = self.faction_user.list[i].avatar_type
        vo.achievement = self.faction_user.list[i].exploits
        vo.headshot_id = self.faction_user.list[i].headshot_id

        table.insert(list,vo)
    end 

    return list 
end 

function LandsBetweenData:CurMapInfo()
    local num_list =self:GetTotalGridList()
    local type = self:GetCurMapType()

    -- 最大神坛等级为9
    local out_list = {}
    for i = 0 ,9 do 
        local vo = {}
        if self.total_alter_num[0][i] ~= nil then 
            vo.str = (i == 0 and Language.LandsBetween.Desc.Empty or string.format(Language.LandsBetween.Desc.Alter,i))..": "..
                ColorStr((type == 0 and num_list[i] or 0).."/"..self.total_alter_num[0][i],COLORSTR.Green9)
            table.insert(out_list,vo)
        end 
    end 

    local in_list = {}
    for i = 0,9 do 
        local vo = {}
        if self.total_alter_num[1][i] ~= nil then 
            vo.str = (i == 0 and Language.LandsBetween.Desc.Empty or string.format(Language.LandsBetween.Desc.Alter,i))..": "..
                ColorStr((type == 1 and num_list[i] or 0).."/"..self.total_alter_num[1][i],COLORSTR.Green9)
            table.insert(in_list,vo)
        end 
    end 
    return {
        info_list = { 
            [0] = out_list,
            [1] = in_list,
        },
        my_info = {
            [0] = {
                city_name = Language.LandsBetween.Desc.MyFaction..string.format(Language.LandsBetween.Desc.CityName[0],
                Language.LandsBetween.FactionName[self.my_info.faction]),
            },
            [1] = {
                city_name = Language.LandsBetween.Desc.MyFaction..string.format(Language.LandsBetween.Desc.CityName[1],
                Language.LandsBetween.FactionName[self.my_info.faction]),
            },
        },
    }
end

function LandsBetweenData:GetMeetGiftReward()
    local base_list =  BagData.Instance:ItemListBuild(Config.divine_domain_system_auto.other[1].season_reward)
    local quintessence_num = Config.divine_domain_system_auto.other[1].quintessence
    local item_quintessence = Item.Init(CommonItemId.GodSpaceEssence,quintessence_num)

    if quintessence_num > 0 then
        table.insert(base_list,item_quintessence)
    end

    local military_exploits_num = Config.divine_domain_system_auto.other[1].military_exploits
    local item_military_exploits = Item.Init(CommonItemId.GodSpaceExploits,military_exploits_num)

    if military_exploits_num > 0 then
        table.insert(base_list,item_military_exploits)
    end

    return base_list 
end

-- 截取当前的行动值信息
function LandsBetweenData:CacheMovingTemp()
    local ap_cfg = Config.divine_domain_system_auto.action_force[1]
    return {
        cur_num = self.my_info.action_point or 0,
        total =GodPosData.Instance:GodPosCfg().ap_max,
        next_temp = self.my_info.next_recover_ap_timestamp,
        all_temp = self.my_info.recover_all_ap_timestamp,
    }
end

-- 行动丹及其恢复界面
function LandsBetweenData:GetMovingParam()
    local ap_cfg = Config.divine_domain_system_auto.action_force[1]
    local ap_items = GoldLaw.ApTemp
    local list = {}
    for i = 1,#ap_items do 
        local vo = {}
        vo.use_item = true
        vo.target_id = ap_items[i]
        vo.num = self.my_info.ap_item_num[i-1]
        vo.oper_index = i -1 
        table.insert(list, vo)
    end 

    local sp_gold_vo = {}
    sp_gold_vo.is_gold = true
    sp_gold_vo.use_item = false
    sp_gold_vo.target_id = CommonItemId.Gold
    sp_gold_vo.num = 0
    sp_gold_vo.name_desc = string.format(Language.LandsBetween.Desc.APBuyByCurrency,
        self.my_info.today_gold_buy_ap_times or 0,ap_cfg.gold_buy_times)
    sp_gold_vo.main_desc = string.format(Language.LandsBetween.Desc.APBuyDesc,ap_cfg.gold_add_ap_num)
    sp_gold_vo.price = ap_cfg.gold_buy_ap_cost
    sp_gold_vo.is_limit = self.my_info.today_gold_buy_ap_times < ap_cfg.gold_buy_times
    table.insert(list,sp_gold_vo)

    local sp_silver_vo = {}
    sp_silver_vo.is_gold = false
    sp_silver_vo.use_item = false
    sp_silver_vo.target_id = CommonItemId.ImmortalCoin
    sp_silver_vo.num = 0
    sp_silver_vo.name_desc = string.format(Language.LandsBetween.Desc.APBuyByCurrency,
        self.my_info.today_jade_buy_ap_times or 0 ,ap_cfg.silver_coin_buy_times)
    sp_silver_vo.main_desc = string.format(Language.LandsBetween.Desc.APBuyDesc,ap_cfg.silver_coin_add_ap_num)
    sp_silver_vo.price = ap_cfg.silver_coin_buy_ap_cost
    sp_silver_vo.is_limit = self.my_info.today_jade_buy_ap_times < ap_cfg.silver_coin_buy_times
    table.insert(list,sp_silver_vo)

    return list
end

-- 获取产出简要信息
function LandsBetweenData:GetProduceInfo()
    local temp_list = { }

    local main_guaji = self.brief_info.guaji_essence_per_minute or 0
    local alter_guaji = self.brief_info.altar_essence_per_minute or 0

    local main_info = {}
    main_info.index = 1
    main_info.main_str = string.format(Language.LandsBetween.Desc.MainCityGuaji, main_guaji)
    main_info.tips = Language.LandsBetween.Desc.MainCityGuajiTips
    table.insert(temp_list,main_info)

    local alter_info = {}
    alter_info.index = 2
    alter_info.main_str = string.format(Language.LandsBetween.Desc.AlterGuaji, alter_guaji)
    alter_info.tips = Language.LandsBetween.Desc.AlterGuajiTips
    table.insert(temp_list,alter_info)

    return {
        list = temp_list,
        main_tips = string.format(Language.LandsBetween.Desc.ProducePer,(main_guaji+alter_guaji)),
    }
end

-- 获取战报列表
function LandsBetweenData:GetReportList()

    local list = {}
    if #self.report_info.list == 0 then return list end 

    for  i = #self.report_info.list, 1 , -1 do 
        -- if self.report_info.delete_list[i] == nil then 
        local vo = TableCopy(self.report_info.list[i])
        table.insert(list,vo)
        -- end 
    end 

    return list
end

-- function LandsBetweenData:ReportDeleteMark(list)
--     for k,v in pairs(list) do 
--         self.report_info.delete_list[v.index] = 1
--     end 
-- end 

function LandsBetweenData:ReadReportDetail(index)
    if index == nil then return nil end 
    self:ReadReport(index)
    ViewMgr:OpenView(LandsBetweenReportDetailView, self.report_info.list[index])
end 

function LandsBetweenData:ReadReport(index)

    self.report_info.list[index].is_read = 1
    LandsBetweenCtrl.Instance:SendReadReport({
        time_stamp =  self.report_info.list[index].timestamp,
    })
    self.report_info.flush = self.report_info.flush + 1
end

function LandsBetweenData:GetPosterInfo()
    local status = self:GetCurMapType() == 0 and 1 or 2
    local t_start = os.date("*t",self.base_info.ourter_space_timestamp)
    local t_end = os.date("*t",self.base_info.season_end_timestamp)
    local role_level = RoleData.Instance:GetRoleLevel()

    local status_str = self:GetCurStatus() < 3 and Language.LandsBetween.Desc.PosterReadyStr
        or (self:GetCurStatus() <5 and string.format(Language.LandsBetween.Desc.PosterStatusStr,DataHelper.GetDaXie(status)) or 
           (self.base_info.season_index > 0 and Language.LandsBetween.Desc.StatusCounting or Language.LandsBetween.Desc.PosterReadyStr))
    return {
        season = self.base_info.season_index,
        status_str = status_str,
        is_running = (self:GetCurStatus() < 3 or self:GetCurStatus() > 4) and role_level >= Config.divine_domain_system_auto.other[1].level_limit,
        time_show = string.format(Language.LandsBetween.Desc.PosterDayShow,t_start.month,t_start.day,t_end.month,t_end.day),
        time_stamp = self.base_info.ourter_space_timestamp,
    }
end

function LandsBetweenData:GetSeasonParam()
    local t_cfg = Config.divine_domain_system_auto
    local season_cfg = {}
    local t_start = os.date("*t",self.base_info.ourter_space_timestamp)
    local t_end = os.date("*t",self.base_info.season_end_timestamp)

    for season_stage = 1,2 do 
        season_cfg[season_stage] = {}

        local task_list = {}
        local oper_tasks = season_stage == 1 and self.phasetask_info.outer_tasks or self.phasetask_info.inner_tasks
        local oper_progress = season_stage == 1 and self.phasetask_info.outer_progress or self.phasetask_info.inner_progress
        local oper_flag = season_stage == 1 and self.phasetask_info.outer_task_flag or self.phasetask_info.inner_task_flag

        for task_index = 1,2 do 
            for index,cfg in pairs(t_cfg.competition_season_task) do 
                if cfg.seq == oper_tasks[task_index] then 
                    local vo = {}
                    vo.task_desc = cfg.way_explain
                    vo.task_total = cfg.param1
                    vo.task_progress = oper_progress[task_index]
                    vo.task_id = oper_tasks[task_index]
                    vo.task_flag = oper_flag[task_index-1] -- vo.task_total == vo.task_progress and 1 or 0 -- 
                    vo.open = cfg.open
                    table.insert(task_list,vo)
                end 
            end 
        end

        season_cfg[season_stage].task_list = task_list

        local reward_list =  season_stage == 1 and t_cfg.competition_season_stage_reward[1].stage_one_preview or 
            t_cfg.competition_season_stage_reward[1].stage_two_preview

        season_cfg[season_stage].reward_list = BagData.Instance:ItemListBuild(reward_list)
        season_cfg[season_stage].reward_flag = season_stage == 1 and self.phasetask_info.outer_reward_flag or self.phasetask_info.inner_reward_flag
    end 

    return {
        season_cfg = season_cfg,
        time_show =  string.format(Language.LandsBetween.Desc.PosterDayShow,t_start.month,t_start.day,t_end.month,t_end.day),
        cur_status = self:GetCurMapType() == 0 and 1 or 2,
        -- 写死了
        -- time_str = "06月09日 09:00--06月09日10:30"
    }
end

function LandsBetweenData:GetAlterAttackInfo()
    local partner_cfg = KeyList(PartnerData.Instance:GetOriginalPartnerInfo(),"partner_id")

    local net_info = self:GetDefenderInfo()

    local map_pic = self:GetMapPicClick()
    local fix_link = self.FixLink(map_pic.static.link_mark)
    local alter_cfg = self:GetAlterCfgByLevel(self:GetAlterGridLevel({x = fix_link[1],y = fix_link[2]}))

    local temp_list = { }
    local single_point = 0
    local group_point = 0
    if alter_cfg ~= nil then 
        for k,v in pairs(alter_cfg.reward) do 
            local vo = Item.Create({item_id = v.item_id ,is_bind = 1,num = 1})
            table.insert(temp_list,vo)
        end 
        single_point = alter_cfg.pray_produce
        group_point = alter_cfg.essence_team
    end 

    local model_path = nil
    if net_info.is_monster == 1 then 
       
        local monster_id = Config.monster_group_auto.monster_group[net_info.monster_group_id].show_monster
        model_path = DrawerHelper.GetNpcPath(ResHelper.MonsterResId(monster_id))
    else 
        local appearance = CommonStruct.AppearanceParam({
            avatar_type = net_info.avatar_type,
            prof_base = net_info.profession,
        })
        model_path = appearance-- UIChDrawer.CreateByAppearance(appearance)
    end


    local emeny_score = net_info.capability
    local diffcult = Language.LandsBetween.Diffcult.Easy
    local role_score,score_level = RoleData.Instance:GetRoleScore()
    if emeny_score == 0 and alter_cfg ~= nil then 
        emeny_score = alter_cfg.score
    end 
    local score_cfg = Config.divine_domain_system_auto.score
    local checker_num = role_score - emeny_score
    local diffect_cfg = nil
    for k,v in pairs(score_cfg) do 
        local real = math.abs(checker_num)
        -- boss比我大的场合下
        if v.attr_max == 0 and checker_num > v.attr_min then  
            diffect_cfg = v 
            break
        -- 我比boss大的场合下
        elseif v.attr_min == 0 and checker_num < v.attr_min then  
            diffect_cfg = v 
            break
        -- 我和boss差不多大的场合下
        else 
            diffect_cfg = v 
            break
        end 
    end 

    diffcult = diffect_cfg == nil and diffcult or diffect_cfg.score_name
    return {
        qua_tips = alter_cfg.altar_task == 1 and alter_cfg.altar_task_2 or alter_cfg.altar_task_2,
        irony_type = alter_cfg.altar_task,
        is_monster =  net_info.is_monster  == 1,
        obj_path = model_path,
        role_app = model_path,
        icon_sp = partner_cfg[5].icon_id,
        progress_rate = 0.5,
        progress_num = "5084/59874",
        single_point = ColorStr(single_point..Language.LandsBetween.Desc.PointPreMin,COLORSTR.Green7),
        group_point = ColorStr(group_point..Language.LandsBetween.Desc.PointPreMin,COLORSTR.Green7),

        emeny_score = emeny_score,
        diffcult = diffcult,
        reward_list = temp_list,
        show_icon = alter_cfg.icon
    }
end

function LandsBetweenData:GetAlterIronyInfo()
    local temp_member_list = {}

    local map_pic = self:GetMapPicClick()
    local fix_link = self.FixLink(map_pic.static.link_mark)
    local grid_level = self:GetAlterGridLevel({x = fix_link[1],y = fix_link[2]})
    local alter_cfg = self:GetAlterCfgByLevel(grid_level)
    
    if alter_cfg.altar_task == 1 then     
        for k,v in pairs(PartnerData.Instance:GetPartnerAllInfo()) do 
            if PartnerConfig.LockState.unlock == v.vo.is_unlocked and v.quality.quality >= alter_cfg.altar_task_2 
                and not ShenYuProbeData.Instance:IsPartnerFight(v, ShenYuSystemHelpType.ShenTan) then
                table.insert(temp_member_list,v)
            end 
        end 
    else 
        for k,v in pairs(PetData.Instance:GetPetList()) do 
            if v:Quality() >= alter_cfg.altar_task_2 
                and not ShenYuProbeData.Instance:IsPetFight(v, ShenYuSystemHelpType.ShenTan) then 
                table.insert(temp_member_list,v)
            end 
        end 
    end 

    -- qua_tips
    local single_point = 0
    local group_point = 0
    local temp_list = { }
    if alter_cfg ~= nil then 
        for k,v in pairs(alter_cfg.reward) do 
            local vo = Item.Create({item_id = v.item_id ,is_bind = 1,num = 1})
            table.insert(temp_list,vo)
        end 
        single_point = alter_cfg.pray_produce
        group_point = alter_cfg.essence_team
    end 

    local map_pic = self:GetMapPicClick()
    local is_empty = map_pic.net_info == nil or map_pic.net_info.pray_end_timestamp == 0 or map_pic.net_info.pray_end_timestamp == nil
    
    -- LogError("/asd gv",alter_cfg.altar_task_2)
    return {
        should_start = is_empty,
        qua_tips = alter_cfg.altar_task == 1 and alter_cfg.altar_task_2 or alter_cfg.altar_task_2,
        irony_type = alter_cfg.altar_task,
        irony_list = temp_member_list,
        reward_list = temp_list,
        single_point = ColorStr(single_point..Language.LandsBetween.Desc.PointPreMin,COLORSTR.Green7),
        group_point = ColorStr(group_point..Language.LandsBetween.Desc.PointPreMin,COLORSTR.Green7),
        cost_num = Config.divine_domain_system_auto.action_force[1].occupy_ap_consume,
        irony_tips = self:GetInoryTipsByGridLevel(grid_level),
    }
end

function LandsBetweenData:GetAlterCfgByLevel(grid_level)
    return self.alter_cfg[grid_level]
end

function LandsBetweenData:GetInoryTipsByGridLevel(grid_level)
    return self.irony_tips_pos[grid_level]
end

function LandsBetweenData:GetExplainList(type)
    local list = { }
    for k,v in pairs(Config.divine_domain_system_auto.explain_button) do
        if v.act == type then 
            local vo = {}
            vo.title =v.small_title
            vo.desc = v.paging_dec
            vo.icon = v.paging_pic
            table.insert(list,vo)
        end
    end

    return list
end

function LandsBetweenData:TryChangeMainInitType(flag)
    self.slow_start.flag = flag
end