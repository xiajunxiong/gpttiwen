LandsBetweenMapView = LandsBetweenMapView or
                          DeclareView("LandsBetweenMapView", "lands_between/lands_between_map", Mod.LandsBetween.Map)
function LandsBetweenMapView:LandsBetweenMapView()

end

function LandsBetweenMapView:OnCloseClick()
    ViewMgr:CloseView(LandsBetweenMapView)
end

function LandsBetweenMapView:OpenCallback()
    if LandsBetweenData.Instance.operating_info.type == 0 then
        self.Panel.BtnPageOutClick:Trigger()
    else
        self.Panel.BtnPageInClick:Trigger()
    end
end

LandsBetweenMapPanel = LandsBetweenMapPanel or DeclareMono("LandsBetweenMapPanel", UIWFlushPanel)
function LandsBetweenMapPanel:LandsBetweenMapPanel()
    self.data = LandsBetweenData.Instance

    self.type = LandsBetweenData.Instance.operating_info.type

    self.map_cache = {}
    self.map_temp_cache = {}

    self.StepCount_C = 500
    self.StepFrameSpace_C = nil

    self.step_set_data_timer_C = nil
    self.part_record_C = -1

    self.StepCount_F = 500
    self.StepFrameSpace_F = nil

    self.step_set_data_timer_F = nil
    self.part_record_F = -1

    self.StepCount_T = 300
    self.StepFrameSpace_T = nil

    self.step_set_data_timer_T = nil
    self.part_record_T = -1

    self.total_link_mark = {}
    self.total_index = {}

    self.is_presure = false
    self.DragStepFrameSpace = 11
    self.DragStepSSpace = 1

    self.SpeedCreateLimit = 1000
    self.SpeedCacheLimit = 1000

    self.sp_map_cache = {}
end

-- 调整为更多的创建更少的刷新
function LandsBetweenMapPanel:ChangeToHighCreate()
    self.StepCount_C = 300
    self.StepFrameSpace_C = nil

    self.StepCount_F = 100
    self.StepFrameSpace_F = nil
end

-- 调整为更多的刷新更少的创建
function LandsBetweenMapPanel:ChangeToHighFlush()
    self.StepCount_C = 100
    self.StepFrameSpace_C = nil

    self.StepCount_F = 300
    self.StepFrameSpace_F = nil
end

-- 调整刷新创建彼此相同
function LandsBetweenMapPanel:ChangeToMidUpdate()
    self.StepCount_C = 150
    self.StepFrameSpace_C = nil

    self.StepCount_F = 150
    self.StepFrameSpace_F = nil
end

function LandsBetweenMapPanel:Awake()
    UIWFlushPanel.Awake(self)

    -- self.BtnPageInClick:Trigger()

    local my_app = RoleData.Instance:GetApearance()
    UH.SetAvatar(self.MyHeadOut, my_app.avatar_type, my_app.avatar_id, my_app.fly_flag)
    UH.SetAvatar(self.MyHeadIn, my_app.avatar_type, my_app.avatar_id, my_app.fly_flag)
    UH.SetAvatar(self.MyPosHeadOut, my_app.avatar_type, my_app.avatar_id, my_app.fly_flag)
    UH.SetAvatar(self.MyPosHeadIn, my_app.avatar_type, my_app.avatar_id, my_app.fly_flag)

    self.BtnPageInInter.Interactable = self.data.operating_info.type == 1
    self.BtnPageOutInter.Interactable = self.data.operating_info.type == 0

    self:AddDistributionLabel()
    self:FlushPanel(true)
end

function LandsBetweenMapPanel:AddDistributionLabel()
    for k, v in pairs(Config.divine_domain_system_auto.altar_distribution) do
        local fix_link = self.data.FixLink(v.position)
        local pos = self.data:FixMapViewPicPos(fix_link[1], fix_link[2], v.competition_season_stage)

        local item_obj = self.DistributionCell.gameObject:Clone(
            v.competition_season_stage == 0 and self.OutDParent or self.InDParent)
        item_obj.name = v.position .. "|" .. v.competition_season_stage
        UH.LocalPosG(item_obj, pos)
        local item_mono = item_obj:GetLuaComponent(self.DistributionCell:GetTypeName())
        item_mono:SetData(v)

        item_obj:SetActive(true)
    end
end

function LandsBetweenMapPanel:FlushPanel(need_move_visible)
    self.map_info = self.data:CurMapInfo()

    UH.SetText(self.MyCityIn, self.map_info.my_info[1].city_name)
    UH.SetText(self.MyCityOut, self.map_info.my_info[0].city_name)

    if need_move_visible then
        local lock_pos = self.data:GetMapLockPos()

        local oper_rect = self.type == 0 and self.OutPosArea or self.InPosArea
        local oper_view = self.type == 0 and self.OutViewArea or self.InViewArea

        local move_pos = Vector2.New(-(lock_pos.x - (oper_view.sizeDelta.x / 2)),
            -(lock_pos.y + (oper_view.sizeDelta.y / 2)))

        UH.LocalPos(oper_rect, move_pos)
    end

    self:TryFlushMap()
    self:FlushMyPos()
    self:FlushSpPos()
    self:ChangeInfoType()
end

-- 刷新我自己的位置
function LandsBetweenMapPanel:FlushMyPos()
    local oper_mine = self.type == 0 and self.MyPosOut or self.MyPosIn

    self.MyPosOut.gameObject:SetActive(self.type == 0)
    self.MyPosIn.gameObject:SetActive(self.type == 1)
    local pos = self.data:FixMapViewPicPos(self.data.my_info.row, self.data.my_info.column, self.type)

    UH.LocalPos(oper_mine, pos)
end

-- 刷新特殊地块
function LandsBetweenMapPanel:FlushSpPos()
    local oper_parent = self.type == 0 and self.MapSpParentOut or self.MapSpParentIn

    for k, v in pairs(self.data.sp_pic_mark) do

        if v.map_sp_name ~= nil then
            if self.sp_map_cache[k] == nil then
                if v.enable_func ~= nil and v.enable_func() then
                    local fix_link = self.data.FixLink(k)
                    local pos = self.data:FixMapViewPicPos(fix_link[1], fix_link[2], self.type)
                    local item_obj = self.MapSpPic.gameObject:Clone(oper_parent)

                    item_obj.name = v.map_sp_name
                    UH.LocalPosG(item_obj, pos)

                    local item_mono = item_obj:GetLuaComponent(self.MapSpPic:GetTypeName())
                    item_mono:SetData({
                        show_pic = v.map_sp_name
                    })

                    self.sp_map_cache[k] = item_obj
                    self.sp_map_cache[k]:SetActive(true)

                    if v.map_sp_fix ~= nil then
                        UH.LocalPosG(item_obj, Vector2.New(v.map_sp_fix.position_x, v.map_sp_fix.position_y))

                        UH.SizeDelta(item_obj, Vector2.New(v.map_sp_fix.width, v.map_sp_fix.height))
                    end
                end
            else
                self.sp_map_cache[k]:SetActive(v.enable_func ~= nil and v.enable_func())
            end
        end
    end
end

function LandsBetweenMapPanel:TryFlushMap()
    if self.step_set_data_timer_C ~= nil then
        Runner.Instance:RemoveRunUntilTrue(self.step_set_data_timer_C)
        self.step_set_data_timer_C = nil
    end

    if self.step_set_data_timer_F ~= nil then
        Runner.Instance:RemoveRunUntilTrue(self.step_set_data_timer_F)
        self.step_set_data_timer_F = nil
    end

    if self.step_set_data_timer_T ~= nil then
        Runner.Instance:RemoveRunUntilTrue(self.step_set_data_timer_T)
        self.step_set_data_timer_T = nil
    end

    if self.type ~= LandsBetweenData.Instance.operating_info.type then
        return
    end

    local oper_rect = self.type == 0 and self.OutPosArea or self.InPosArea
    local oper_view = self.type == 0 and self.OutViewArea or self.InViewArea
    self.map_input = {
        type = self.type,
        visible = {
            start = {
                x = oper_rect.localPosition.x,
                y = oper_rect.localPosition.y
            },
            limit = {
                x = oper_rect.localPosition.x - oper_view.sizeDelta.x,
                y = oper_rect.localPosition.y + oper_view.sizeDelta.y
            }
        }
    }

    -- local map_pos_list = self.data:GetMapVisibleList()
    -- self.OutTest 
    local cur_center_pos = Vector2.New((-oper_rect.localPosition.x + (oper_view.sizeDelta.x / 2)),
        (-oper_rect.localPosition.y - (oper_view.sizeDelta.y / 2)))
    -- UH.LocalPosG(self.InTest,cur_center_pos)
    local map_pos_list = self.data:GetMapVisibleList(cur_center_pos, self.type, self.drag_runner ~= nil)
    self.init_cache = table.nums(self.total_index) == 0
    if table.nums(map_pos_list) == 0 then
        return
    end
    -- if self.start_time == nil then
    --     self.start_time = os.time()
    -- end
    -- LogError("神域小地图 已生成有色地块", Time.realtimeSinceStartup - self.start_time)
    self:FlushPicTotal(map_pos_list)
    -- self.start_time = Time.realtimeSinceStartup
end

function LandsBetweenMapPanel:FlushPicTotalComplete()
    -- for i = #self.data.net_map_show_info.list, #self.map_cache do 
    --     if self.map_cache[i] ~= nil then 
    --         self.map_cache[i]:SetActive(false)
    --     end 
    -- end 

    self:FlushTotalCache()

    local nums_create = table.nums(self.totaled.create_list)
    local nums_cache = table.nums(self.totaled.cache_list)
    if nums_create > self.SpeedCreateLimit and nums_create > nums_cache then
        self:ChangeToHighCreate()
    elseif nums_cache > self.SpeedCacheLimit and nums_cache > nums_create then
        self:ChangeToHighFlush()
    else
        self:ChangeToMidUpdate()
    end
    if nums_create > 0 then
        self:FlushPicCreate(self.totaled.create_list)
    end
    if nums_cache > 0 then
        self:FlushPicCache(self.totaled.cache_list)
    end

end

function LandsBetweenMapPanel:FlushTotalCache()
    -- LogError("?total cache",table.nums(self.total_index),table.nums(self.total_link_mark))
    if self.init_cache then
        return
    end

    --[[ for k,v in pairs(self.total_index) do 
        if v.times == 0 then
            self.total_link_mark[v.link_mark] = nil
            self.total_index[k] = nil
        end
    end  ]]
    for k, v in pairs(self.map_cache) do
        if self.total_link_mark[v.name] == nil then
            -- UnityObj.Destroy(v.obj)
            v.obj:SetActive(false)
            self.map_cache[v.name] = nil
            table.insert(self.map_temp_cache, v.obj)
        end
    end
    for k, v in pairs(self.total_index) do
        v.times = 0
    end
end

function LandsBetweenMapPanel:FlushPicTotal(create_list)

    self.part_record_T = -1
    self.setuped_index_T = 0

    self.cache_index = 1

    self.total_list = create_list
    self.totaled = {
        create_list = {},
        cache_list = {}
    }
    -- LogError("删除runner")
    -- 进行刷新前会删除之前发起的资源加载
    if self.step_set_data_timer_T then
        Runner.Instance:RemoveRunUntilTrue(self.step_set_data_timer_T)
        self.step_set_data_timer_T = nil
    end

    if self.step_set_data_timer_F then
        Runner.Instance:RemoveRunUntilTrue(self.step_set_data_timer_F)
        self.step_set_data_timer_F = nil
    end

    if self.step_set_data_timer_C then
        Runner.Instance:RemoveRunUntilTrue(self.step_set_data_timer_C)
        self.step_set_data_timer_C = nil
    end
    -- LogError("神域小地图开始创建")
    self.total_link_mark = {}
    self.step_set_data_timer_T = Runner.Instance:RunUntilTrue(BindTool.Bind(self.stepSetupItemsTotal, self))
end

function LandsBetweenMapPanel:TotalRes(vo)
    local oper_parent = self.type == 0 and self.OutMapPicParents or self.InMapPicParents
    vo.parent = oper_parent

    self.total_link_mark[vo.name] = vo.index

    if self.map_cache[vo.link_mark] == nil then
        local item_obj
        if table.nums(self.map_temp_cache) > 0 then
            vo.obj = table.remove(self.map_temp_cache)
            table.insert(self.totaled.cache_list, vo)
        else
            table.insert(self.totaled.create_list, vo)
        end
        vo.func = function()
            local item_obj = vo.obj
            if item_obj == nil then
                item_obj = self.MapPic.gameObject:Clone(vo.parent)
            end
            item_obj.name = vo.name
            UH.LocalPosG(item_obj, vo.pos)

            local item_mono = item_obj:GetLuaComponent(self.MapPic:GetTypeName())
            item_mono:SetData(vo.item_data)
            vo.obj = item_obj
            vo.obj:SetActive(true)
            -- self.map_cache[vo.index] = {obj = item_obj,data = vo.item_data}
            -- self.map_cache[vo.index].obj:SetActive(true)
            self.map_cache[vo.link_mark] = vo

            -- 明确的信息更新会刷新缓存
            self.total_link_mark[vo.name] = vo.index
            self.total_index[vo.index] = {
                link_mark = vo.name,
                times = 0
            }
        end
    else
        vo.func = function()
            local item_obj = vo.obj
            if item_obj then
                local item_mono = item_obj:GetLuaComponent(self.MapPic:GetTypeName())
                item_mono:SetData(vo.item_data)
                vo.obj = item_obj
                vo.obj:SetActive(true)
            end
            -- -- self.map_cache[vo.index]:SetParent(vo.parent)
            -- -- 只有在现缓存与刷新目标不相同的场合下进行数据更新
            -- if self.map_cache[vo.index].obj.name == vo.name then return end 

            -- self.map_cache[vo.index].obj.name = vo.name
            -- UH.LocalPosG(self.map_cache[vo.index].obj,vo.pos)

            -- -- 明确的信息更新会刷新缓存
            -- self.total_link_mark[vo.name] = vo.index
            -- self.total_index[vo.index] = {link_mark = vo.name,times = 0}

            -- if self.map_cache[vo.index].data.color_enum == vo.item_data.color_enum then return end

            -- local item_mono = self.map_cache[vo.index].obj:GetLuaComponent(self.MapPic:GetTypeName())
            -- item_mono:SetData(vo.item_data)        

            -- self.map_cache[vo.index].data = vo.item_data
            -- -- self.map_cache[cp_o.index]:SetActive(true)
        end

        table.insert(self.totaled.cache_list, vo)
    end
end
function LandsBetweenMapPanel:ExeData()
    self.setuped_index_T = self.setuped_index_T + 1
    local res_vo = self.total_list[self.setuped_index_T]
    if res_vo ~= nil then
        local map_list = self.data:ReleaseVisible(res_vo)
        -- LogError("小地图设置个数", table.nums(map_list))
        local check_pos
        local color_m
        local pos
        local fix_link
        for i, v in ipairs(map_list) do
            check_pos = self.data.net_map_show_info.list[v.link_mark]
            if check_pos ~= nil and check_pos.faction >= 0 then
                fix_link = self.data.FixLink(v.link_mark)
                color_m = self.map_input.type == 0 and 0 or check_pos.faction
                pos = self.data:FixMapViewPicPos(fix_link[1], fix_link[2], self.map_input.type)
                local ready = {}
                ready.index = self.cache_index
                ready.link_mark = v.link_mark
                ready.item_data = {
                    type = self.map_input.type,
                    color_enum = color_m
                }
                ready.pos = pos
                ready.name = v.link_mark
                self:TotalRes(ready)
            end
        end
        self.cache_index = self.cache_index + 1
    end
end
function LandsBetweenMapPanel:stepSetupItemsTotal()
    if self:Destroyed() then
        return true
    end
    -- LogError("小地图数据解析中", self.setuped_index_T)
    -- 完成 结束循环
    if self.setuped_index_T > table.nums(self.total_list) then -- 分步加载完成后延一帧再通知外部加载完成
        -- LogError("小地图数据解析结束")
        self:FlushPicTotalComplete()
        self.step_set_data_timer_T = nil
        return true
    end

    -- 隔帧操作
    -- if self.StepFrameSpace_T ~= nil and self.StepFrameSpace_T > 1 then
    -- 	if self.part_record_T >= 0 then
    -- 		if self.part_record_T >= self.StepFrameSpace_T then
    -- 			self.part_record_T = 0
    -- 		else
    -- 			self.part_record_T = self.part_record_T + 1
    -- 			return false
    -- 		end
    -- 	else
    -- 		self.part_record_T = 0
    -- 	end
    -- end 

    -- local count = 0
    -- while true do
    -- 	count = count + 1
    -- 	self.setuped_index_T = self.setuped_index_T + 1
    -- 	if self.setuped_index_T > table.nums(self.total_list) then
    -- 		return false
    -- 	end
    -- 生成逻辑
    -- local res_vo = self.total_list[self.data:GetLinkMarkIndex(self.setuped_index_T)]

    -- local function TotalRes(vo)
    --     local oper_parent = self.type == 0 and self.OutMapPicParents or self.InMapPicParents
    --     local cp_o = {}
    --     cp_o.parent = oper_parent
    --     cp_o.index = vo.index
    --     cp_o.name = vo.link_mark
    --     cp_o.item_data = vo.show_param
    --     cp_o.pos = {x = vo.x , y = vo.y}

    --     if self.map_cache[vo.index] == nil then 
    --         cp_o.func = function()
    --             local item_obj = self.MapPic.gameObject:Clone(cp_o.parent)
    --             item_obj.name = cp_o.name
    --             UH.LocalPosG(item_obj,cp_o.pos)

    --             local item_mono = item_obj:GetLuaComponent(self.MapPic:GetTypeName())
    --             item_mono:SetData(cp_o.item_data)

    --             self.map_cache[cp_o.index] = {obj = item_obj,data = cp_o.item_data}
    --             self.map_cache[cp_o.index].obj:SetActive(true)

    --             -- 明确的信息更新会刷新缓存
    --             self.total_link_mark[cp_o.name] = cp_o.index
    --             self.total_index[cp_o.index] = {link_mark = cp_o.name,times = 0}
    --         end
    --         table.insert(self.totaled.create_list,cp_o)
    --     else 
    --         cp_o.func = function()
    --             -- self.map_cache[cp_o.index]:SetParent(cp_o.parent)
    --             -- 只有在现缓存与刷新目标不相同的场合下进行数据更新
    --             if self.map_cache[cp_o.index].obj.name == cp_o.name then return end 

    --             self.map_cache[cp_o.index].obj.name = cp_o.name
    --             UH.LocalPosG(self.map_cache[cp_o.index].obj,cp_o.pos)

    --             -- 明确的信息更新会刷新缓存
    --             self.total_link_mark[cp_o.name] = cp_o.index
    --             self.total_index[cp_o.index] = {link_mark = cp_o.name,times = 0}

    --             if self.map_cache[cp_o.index].data.color_enum == cp_o.item_data.color_enum then return end

    --             local item_mono = self.map_cache[cp_o.index].obj:GetLuaComponent(self.MapPic:GetTypeName())
    --             item_mono:SetData(cp_o.item_data)        

    --             self.map_cache[cp_o.index].data = cp_o.item_data
    --             -- self.map_cache[cp_o.index]:SetActive(true)
    --         end 

    --         table.insert(self.totaled.cache_list,cp_o)
    --     end 
    -- end
    -- if res_vo ~= nil then 
    --     local fix_link = self.data.FixLink(res_vo.link_mark)
    --     if res_vo ~= nil and self.data:CheckMapVisibleShow(
    --         {x = fix_link[1],y = fix_link[2]},self.map_input) then --,map_stamp = res_vo.map_stamp

    --         local pos = self.data:FixMapViewPicPos(fix_link[1],fix_link[2],self.map_input.type)

    --         local ready = {}
    --         ready.index =  self.cache_index
    --         ready.link_mark = res_vo.link_mark
    --         ready.x = pos.x
    --         ready.y = pos.y
    --         local color_m = self.map_input.type == 0 and 0 or res_vo.faction
    --         ready.show_param = {
    --             type = self.map_input.type,
    --             color_enum = color_m,
    --         }
    --         if res_vo.faction >= 0 then 
    --             TotalRes(ready)
    --             self.cache_index = self.cache_index + 1
    --         end 
    --     end 
    -- end 

    --  math.floor(math.random(0, 3))
    -- UnityProfiler.BeginSample("check cell")
    local num = math.ceil(table.nums(self.total_list) / 10)
    for i = 1, num, 1 do
        self:ExeData()
    end

    -- for k,v in pairs(map_list) do 
    --     local check_pos = self.data.net_map_show_info.list[v.link_mark]
    --     local fix_link = self.data.FixLink(v.link_mark)
    --     -- 存在缓存记录则标记一次缓存
    --     -- LogError("??",v.link_mark,self.total_link_mark[v.link_mark])
    --     if self.total_link_mark[v.link_mark] ~= nil and self.total_index[self.total_link_mark[v.link_mark]] ~= nil then 
    --         if self.total_link_mark[v.link_mark] == self.cache_index then 
    --             self.total_index[self.total_link_mark[v.link_mark]].times = self.total_index[self.total_link_mark[v.link_mark]].times + 1
    --         else
    --             self.total_index[self.total_link_mark[v.link_mark]] = nil 
    --             self.total_link_mark[v.link_mark] = nil
    --         end 
    --     end 
    --     -- 不存在缓存记录则列入处理阵列
    --     if (self.is_presure or (check_pos ~= nil and check_pos.faction >= 0)) and GoldLaw.Operate[v.map_show.map_stamp] 
    --         and self.total_link_mark[v.link_mark] == nil then -- and self.data:CheckMapVisibleShow({x = fix_link[1],y = fix_link[2]},self.map_input) then 

    --         local pos = self.data:FixMapViewPicPos(fix_link[1],fix_link[2],self.map_input.type)

    --         local ready = {}
    --         ready.index =  self.cache_index
    --         ready.link_mark = v.link_mark
    --         ready.x = pos.x
    --         ready.y = pos.y
    --         local color_m = self.is_presure and 0 or (self.map_input.type == 0 and 0 or check_pos.faction) -- (self.is_presure or self.map_input.type == 0) and 0 or check_pos.faction
    --             ready.show_param = {
    --                 type = self.map_input.type,
    --                 color_enum = color_m,
    --             }
    --         TotalRes(ready)
    --     end 
    --     self.cache_index = self.cache_index + 1
    -- end 
    -- end
    -- UnityProfiler.EndSample()
    -- if self.StepCount_T > 0 and count >= self.StepCount_T then
    -- 	return false
    -- end

end

-- 获取当前创建进度
function LandsBetweenMapPanel:GetCurCreateProgress()
    return (self.setuped_index_C / #self.res_list)
end

function LandsBetweenMapPanel:FlushPicCreate(create_list)

    self.part_record_C = -1
    self.setuped_index_C = 0

    self.res_list = create_list

    -- 进行刷新前会删除之前发起的资源加载
    if self.step_set_data_timer_C then
        Runner.Instance:RemoveRunUntilTrue(self.step_set_data_timer_C)
        self.step_set_data_timer_C = nil
    end

    self.step_set_data_timer_C = Runner.Instance:RunUntilTrue(BindTool.Bind(self.stepSetupItemsCreate, self))
end

function LandsBetweenMapPanel:stepSetupItemsCreate()
    if self:Destroyed() then
        return true
    end

    -- 完成 结束循环
    if self.setuped_index_C > #self.res_list then -- 分步加载完成后延一帧再通知外部加载完成
        -- LogError("?complete create!")
        self.step_set_data_timer_C = nil
        return true
    end

    -- 隔帧操作
    if self.StepFrameSpace_C ~= nil and self.StepFrameSpace_C > 1 then
        if self.part_record_C >= 0 then
            if self.part_record_C >= self.StepFrameSpace_C then
                self.part_record_C = 0
            else
                self.part_record_C = self.part_record_C + 1
                return false
            end
        else
            self.part_record_C = 0
        end
    end

    local count = 0
    while true do
        count = count + 1
        self.setuped_index_C = self.setuped_index_C + 1
        if self.setuped_index_C > #self.res_list then
            return false
        end
        -- 生成逻辑
        local res_vo = self.res_list[self.setuped_index_C]
        if res_vo.func ~= nil then
            res_vo.func()
        end

        if self.StepCount_C > 0 and count >= self.StepCount_C then
            return false
        end
    end
end

-- 获取当前刷新进度
function LandsBetweenMapPanel:GetCurFlushProgress()
    return (self.setuped_index_F / #self.flush_list)
end

function LandsBetweenMapPanel:FlushPicCache(create_list)

    self.part_record_F = -1
    self.setuped_index_F = 0

    self.flush_list = create_list

    -- 进行刷新前会删除之前发起的资源加载
    if self.step_set_data_timer_F then
        Runner.Instance:RemoveRunUntilTrue(self.step_set_data_timer_F)
        self.step_set_data_timer_F = nil
    end

    self.step_set_data_timer_F = Runner.Instance:RunUntilTrue(BindTool.Bind(self.stepSetupItemsFlush, self))
end

function LandsBetweenMapPanel:stepSetupItemsFlush()
    if self:Destroyed() then
        return true
    end

    -- 完成 结束循环
    if self.setuped_index_F > #self.flush_list then -- 分步加载完成后延一帧再通知外部加载完成
        self.step_set_data_timer_F = nil
        -- LogError("?complete flush!")
        return true
    end

    -- 隔帧操作
    if self.StepFrameSpace_F ~= nil and self.StepFrameSpace_F > 1 then
        if self.part_record_F >= 0 then
            if self.part_record_F >= self.StepFrameSpace_F then
                self.part_record_F = 0
            else
                self.part_record_F = self.part_record_F + 1
                return false
            end
        else
            self.part_record_F = 0
        end
    end

    local count = 0
    while true do
        count = count + 1
        self.setuped_index_F = self.setuped_index_F + 1
        if self.setuped_index_F > #self.flush_list then
            return false
        end
        -- 生成逻辑
        local res_vo = self.flush_list[self.setuped_index_F]
        if res_vo.func ~= nil then
            res_vo.func()
        end

        if self.StepCount_F > 0 and count >= self.StepCount_F then
            return false
        end
    end
end

function LandsBetweenMapPanel:OnDestroy()
    UIWFlushPanel.OnDestroy(self)

    if self.step_set_data_timer_C ~= nil then
        Runner.Instance:RemoveRunUntilTrue(self.step_set_data_timer_C)
        self.step_set_data_timer_C = nil
    end

    if self.step_set_data_timer_F ~= nil then
        Runner.Instance:RemoveRunUntilTrue(self.step_set_data_timer_F)
        self.step_set_data_timer_F = nil
    end

    if self.step_set_data_timer_T ~= nil then
        Runner.Instance:RemoveRunUntilTrue(self.step_set_data_timer_T)
        self.step_set_data_timer_T = nil
    end

    if self.drag_runner then
        TimeHelper:CancelTimer(self.drag_runner)
        self.drag_runner = nil
    end

end

function LandsBetweenMapPanel:OnDragBegin()
    self.wait_timer = 0
    TimeHelper:CancelTimer(self.drag_runner)
    -- self.drag_runner = TimeHelper:AddRunTimer(function ()
    self.drag_runner = TimeHelper:AddRunFrameTimer(function()
        -- LogError("拖动检查中11")
        -- self:FlushVisible()
        -- self:FlushForce()
        -- LogError("? in runner",self.DragStepFrameSpace,Time.frameCount)
        -- 没做完的话尽量做完再开始下一次更新
        -- LogError("?loop！！！！！",(self.step_set_data_timer_C ~= nil or self.step_set_data_timer_F ~= nil),self.wait_timer )
        -- if (self.step_set_data_timer_T ~= nil or self.step_set_data_timer_C ~= nil or self.step_set_data_timer_F ~= nil) and self.wait_timer < 10 then
        --     self.wait_timer = self.wait_timer + 1
        --     return 
        -- elseif self.wait_timer >= 10 then 
        --     -- LogError("?重试等待次数已经超过"..self.wait_timer.."次！！！！")
        --     self.wait_timer = 0 
        -- end 

        -- 统计分布要做完
        -- 创建分布也要做完
        -- 刷新分布可以少一点
        if self.step_set_data_timer_T ~= nil or self.step_set_data_timer_C ~= nil or self.step_set_data_timer_F ~= nil then -- self:GetCurFlushProgress() < 0.8 then 
            return
        end
        self:TryFlushMap()
    end, self.DragStepFrameSpace,60000)
    -- end, self.DragStepSSpace)
end

function LandsBetweenMapPanel:OnDragEndOfOut()
    if self.drag_runner then
        TimeHelper:CancelTimer(self.drag_runner)
        self.drag_runner = nil
    end
    self:TryFlushMap()
end

function LandsBetweenMapPanel:OnDragEndOfIn()
    if self.drag_runner then
        TimeHelper:CancelTimer(self.drag_runner)
        self.drag_runner = nil
    end

    self:TryFlushMap()
end

function LandsBetweenMapPanel:OnClickJump()

    local oper_rect = self.type == 0 and self.OutPosArea or self.InPosArea
    local oper_view = self.type == 0 and self.OutViewArea or self.InViewArea

    local click_pos = UnityEngine.Input.mousePosition

    UH.AnchoredPosition(self.ClickMarkRect,{x=click_pos.x,y=click_pos.y})

    local lock_pos = self.ClickMarkRect:InverseTransformPoint(oper_view.position)

    self.ClickMark2:SetParent(oper_view)
    self.ClickMark2.transform.anchorMax = Vector2.New(0,1)
    self.ClickMark2.transform.anchorMin = Vector2.New(0,1)
    UH.LocalPosG(self.ClickMark2, {x = -lock_pos.x ,y = -lock_pos.y})

    local jump_pos = {x = -oper_rect.localPosition.x+self.ClickMark2.transform.anchoredPosition.x,
        y = -oper_rect.localPosition.y+self.ClickMark2.transform.anchoredPosition.y}

    self.data:TryJumpPos(jump_pos)

end

function LandsBetweenMapPanel:ChangeInfoType()
    self.info_list:SetData(self.map_info.info_list[self.type])
end

function LandsBetweenMapPanel:OnClickShowInfo()
    self.BtnMapInfoShowIn:SetActive(false)
    self.BtnMapInfoShowOut:SetActive(false)
end

function LandsBetweenMapPanel:OnMapDrag()

end

function LandsBetweenMapPanel:OnClickHideInfo()
    self.BtnMapInfoShowIn:SetActive(true)
    self.BtnMapInfoShowOut:SetActive(true)
end

function LandsBetweenMapPanel:OnClickDistributionPos()
    self.BtnDistributionPos:SetActive(false)
    self.OutDParent:SetActive(true)
    self.InDParent:SetActive(true)
end

function LandsBetweenMapPanel:OnClickAlterPos()
    self.BtnDistributionPos:SetActive(true)
    self.OutDParent:SetActive(false)
    self.InDParent:SetActive(false)
    ViewMgr:OpenView(LandsBetweenAltarInfoView, {
        click_type = 1
    })
    ViewMgr:CloseView(LandsBetweenMapView)
end

-- 内域
function LandsBetweenMapPanel:OnClickPageIn()
    if self.data.operating_info.type == 0 then
        PublicPopupCtrl.Instance:Center(Language.LandsBetween.Desc.OutAreaStatusComplete)
        return
    end

    self.type = 1
    self.TotalOut:SetActive(false)
    self.TotalIn:SetActive(true)

    self:ChangeInfoType()
end

-- 外域
function LandsBetweenMapPanel:OnClickPageOut()
    if self.data.operating_info.type == 1 then
        PublicPopupCtrl.Instance:Center(Language.LandsBetween.Desc.InAreaStatusComplete)
        return
    end

    self.type = 0
    self.TotalOut:SetActive(true)
    self.TotalIn:SetActive(false)

    self:ChangeInfoType()
end

LandsBetweenMapInfoCell = LandsBetweenMapInfoCell or DeclareMono("LandsBetweenMapInfoCell", UIWidgetBaseItem)
function LandsBetweenMapInfoCell:SetData(data)
    UIWidgetBaseItem.SetData(self, data)

    UH.SetText(self.str, data.str)
end

LandsBetweenMapGridCell = LandsBetweenMapGridCell or DeclareMono("LandsBetweenMapGridCell", UIWidgetBaseItem)
function LandsBetweenMapGridCell:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    -- UH.SizeDelta2(self.Trans, Vector2.New(data.rect_length,data.rect_length))
    if data.type == nil or data.color_enum == nil then
        return
    end

    UH.SpriteName(self.ShowSprite, data.type == 0 and "origin_103" or "origin_141")
    UH.Color(self.Show, GoldLaw.MapColorShow[data.type][data.color_enum])
end

LandsBetweenMapDCell = LandsBetweenMapDCell or DeclareMono("LandsBetweenMapDCell", UIWidgetBaseItem)
function LandsBetweenMapDCell:SetData(data)
    UIWidgetBaseItem.SetData(self, data)

    UH.SetText(self.Str, data.distribution)
end

LandsBetweenMapGridSpCell = LandsBetweenMapGridSpCell or DeclareMono("LandsBetweenMapGridSpCell", UIWidgetBaseItem)
function LandsBetweenMapGridSpCell:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    UH.SpriteName(self.SpPic, data.show_pic)
end
