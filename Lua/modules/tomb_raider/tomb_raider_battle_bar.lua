
--===========================TombLiveMap=====================================
TombLiveMap = TombLiveMap or DeclareMono("TombLiveMap", UIWFlushPanel)
function TombLiveMap:TombLiveMap()
    self.on_scene_data_inited = EventSys:Bind(GlobalEventKeys.SceneDataInited,BindTool.Bind(self.OnSceneDataInited, self))
    self.on_scene_data_disposed = EventSys:Bind(GlobalEventKeys.SceneDataDisposed,BindTool.Bind(self.OnSceneDataDisposed, self))

    self:LoadMapInfo()

    self.data = TombRaiderData.Instance
    self.time_ctrl = TimeCtrl.Instance
    self.scene_data = SceneData.Instance
    self.login_data = LoginData.Instance:BaseData()

    self.cur_time = 0
    self.cur_wifi = 2
    Runner.Instance:AddRunObj(self, 16)
    self.data_cares = {
        {data = self.data.map_obj_info, func = self.LoadMapInfo},
        {data = self.time_ctrl.delay_time, func = self.FlushWIFI},
        {data = self.scene_data:Base(), func = self.FlushMapName},
        {data = self.scene_data.scene_custom_names, func = self.FlushMapName},
        {data = self.login_data, func = self.FlushMapName, keys = {"is_on_crosssever"}, init_call = false},
    }

    self.mark_data = {}
    self.timers = 0
    self.is_first = true
    self.mark_timer_list = {}
end

function TombLiveMap:OnDestroy()
    UIWFlushPanel.OnDestroy(self)
    Runner.Instance:RemoveRunObj(self)
    EventSys:UnBind(self.on_scene_data_disposed)
    EventSys:UnBind(self.on_scene_data_inited)

    TimeHelper:CancelTimer(self.status_flush_timer)
    self.status_flush_timer = nil
end 

-- 启动加载
function TombLiveMap:OnSceneDataInited()
    self:LoadMapInfo()
end 

function TombLiveMap:OnSceneDataDisposed()

end 

function TombLiveMap:OnEnable()
    UIWFlushPanel.OnEnable(self)
    self.status_flush_timer = TimeHelper:AddRunTimer(function() 
        local lv = Nirvana.DeviceTool.GetBatteryLevel()
        self.PowerImg.fillAmount = Nirvana.DeviceTool.GetBatteryLevel() / 100
    end,10,-1,true)
end

function TombLiveMap:OnDisable()
    UIWFlushPanel.OnDisable(self)
    TimeHelper:CancelTimer(self.status_flush_timer)
    self.status_flush_timer = nil
end


function TombLiveMap:LoadMapInfo()
    local scene_id = SceneData.Instance:SceneId()
    --加载地图
    if not self.is_first then
        self.Texture:SetByPath(MapData.Instance:GetMapMiniPath(scene_id))
        self.ShowTexture:SetByPath(MapData.Instance:GetMapMiniPath(scene_id))
        self.is_first = true
    end 

    self:FlushMapIcon()
end

function TombLiveMap:FlushMapIcon()
    local scene_id = SceneData.Instance:SceneId()
        --加载地图图标
        local icon_list = TombRaiderData.Instance:GetMapMiniIconList()
        for k,v in pairs(icon_list) do
            if v.is_mark then 
                v.pos = v.c_pos
            else 
                local pos = Vector3.New(v.c_pos.x,v.c_pos.y,v.c_pos.z)
                v.pos = MapCtrl.Instance:WorldToTransformUV(pos,self.MapTex,scene_id)
            end 
        end
        self.IconList.StepCount = #icon_list
        self.IconList:SetData(icon_list)
    
        local Showicon_list = TombRaiderData.Instance:GetMapMiniIconList()
        for k,v in pairs(Showicon_list) do
            if v.is_mark then 
                v.pos = v.c_pos
            else 
                local pos = Vector3.New(v.c_pos.x,v.c_pos.y,v.c_pos.z)
                v.pos = MapCtrl.Instance:WorldToTransformUV(pos,self.ShowMapTex,scene_id)
            end 
        end
        self.ShowIconList.StepCount = #Showicon_list
        self.ShowIconList:SetData(Showicon_list)
end

-- 实时刷新
function TombLiveMap:FlushMapAllTime()
    if MainPlayer == nil then return end
    local scene_id = SceneData.Instance:SceneId()
    local pos = MainPlayer:GetPos()
    local main_pos = MapCtrl.Instance:WorldToTransformUV(pos,self.MapTex, scene_id)
    UH.LocalPos(self.MainRolePos,main_pos)
   
    local cam_main_pos = MapCtrl.Instance:WorldToTransformUV(pos,self.ShowMapTex, scene_id)
    UH.LocalPos(self.ShowMainRolePos,cam_main_pos)

    local cam_pos = Vector3.New(-cam_main_pos.x,-cam_main_pos.y,-cam_main_pos.z)
    UH.LocalPos(self.MainShowPos,cam_pos)

    if  self.timers > 100 then 
        -- self.data:CheckBoxHere()
        -- self:CheckMapMark()
        self.data:ReverseFlushSkillItem()
        self.timers = 0
    else 
        self.timers =  self.timers + 1
    end 

    self:FlushMapIcon()
end

function TombLiveMap:SetMarkData(type)
    self.mark_data = {
        mark_type =  type,
    }
end

-- mark系统已被暂时移除
function TombLiveMap:CheckMapMark()
    self.data:FlushMarkInfo()
    self:LoadMapInfo()
end

function TombLiveMap:IsShowing()
    return self.wholeMap.activeSelf == true
end 

function TombLiveMap:OnClickMap(type)
    if type == 0 and self:IsShowing() then return end 
    local operate = type == 1 and "hide" or "show"

    if operate == "hide" then self:MapMark() end 
    self.MapAnim:SetTrigger(APH(operate))
end

function TombLiveMap:MapMark()
    if self.mark_data == nil then return false end 

    local scene_id = SceneData.Instance:SceneId()
    -- 获得世界坐标
    local view_pos = MapCtrl.Instance:TransformUVPos(self.ShowMapTex, scene_id)

    -- 获得场景实际坐标
    local local_pos = MapCtrl.Instance:TransformUVToWorld(view_pos)
    local y = SceneCtrl.Instance:CurScene():GetPosHeight(local_pos.x, local_pos.z)

    -- 获得相对场景的地图坐标
    local pos = MapCtrl.Instance:WorldToTransformUV(Vector3.New(local_pos.x, y ,local_pos.z),self.ShowMapTex,scene_id)

    -- self.MarkTest:SetData({
    --     pos = pos,
    --     type = self.mark_data.mark_type ,
    -- })
    TombRaiderCtrl.Instance:SendSkillOperate({
        type = TombRaiderConfig.OperType.MarkMap,
        param1 = self.mark_data.mark_type ,
        param2 = pos.x,
        param3 = pos.y,
    })
    self.mark_data = nil 
end

function TombLiveMap:FlushWIFI()
    local delay = self.time_ctrl:GetDelayTime()
    local index = 4
    if delay == nil then
        index = 2
    end
    if delay and delay > Delay_Type.Nomal then
        index = 3
    end
    if delay and delay > Delay_Type.Bad then
        index = 2
        PublicPopupCtrl.Instance:Center("网络不稳定")
    end
    if self.cur_wifi ~= index then
        UH.SpriteName(self.WifiSp, "XiTongXinXi_XinHao" .. index)
        self.cur_wifi = index
    end
end

function TombLiveMap:Update()
    -- LogError("?TombLiveMap!")
    local server_time = TimeManager:GetServerTime()
    if self.cur_time ~= tonumber(os.date("%M", server_time)) then
        UH.SetText(self.TimeTex, os.date("%H:%M", server_time))
        self.cur_time = tonumber(os.date("%M", server_time))
    end

    self:FlushMapAllTime()
end

function TombLiveMap:FlushMapName()
    local custom_scene_name = SceneData.Instance:GetSceneNameCustomBySceneId(SceneData.Instance:SceneId())
    local temp_name = custom_scene_name or SceneData.Instance:Base().scene_name
    -- if self.login_data.is_on_crosssever then
    --     UH.SetText(self.NameTex, string.format("%s(%s)", temp_name, Language.Login.CrossRealm))
    -- else
        UH.SetText(self.NameTex, temp_name)
    -- end
end

--===========================TombLiveMapItem=====================================
TombLiveMapItem = TombLiveMapItem or DeclareMono("TombLiveMapItem", UIWidgetBaseItem)
function TombLiveMapItem:SetData(data)
    if data.pos then
        UH.LocalPos(self.Item,data.pos)
    end

    -- local type = TombRaiderConfig.BattleMapIconType[data.type]
    self:ShowIconView(data.type)
    UIWidgetBaseItem.SetData(self, data)
end 

function TombLiveMapItem:ShowIconView(type)
    for i = 1, 8 do
        if self.IconList[i] then 
            self.IconList[i]:SetActive(i == type)
        end
    end
end
function TombLiveMapItem:OnClickItem()
    
end

--=================================TombJoyStick=======================================
TombJoyStick = TombJoyStick or DeclareMono("TombJoyStick", UIWFlushPanel)
function TombJoyStick:TombJoyStick()
    self.joystick_angle = 0
    self.joystick_last_target_pos = Vector2.New(0, 0)	

    self.stick_off = Vector3.zero

    self.main_camera = UnityGameObj.FindGameObjectWithTag("MainCamera")
end 

function TombJoyStick:OnDestroy()
    UIWFlushPanel.OnDestroy(self)

    if self.stick_update ~= nil then
        TimeHelper:CancelTimer(self.stick_update)
        self.stick_update = nil
    end 

end

function TombJoyStick:OnJoyStickDrag()
    if TombRaiderData.Instance:IsLimitedMove() then return end 
    self.stick_off = self:getStickOffset()	
end

function TombJoyStick:getStickOffset()
	if self.stick_init_pos == nil then         --初始化取到的位置会有问题，改在第一次收到事件的时候获取
		self.stick_init_pos = self.joystick:GetLocalPosition()
	end

    -- 输入方案：unity的屏幕鼠标输入/当前屏幕宽 * 当前界面尺寸
	self.input_t = Vector2(UnityEngine.Input.mousePosition.x / UnityEngine.Screen.width * self.main.rect.width,
        UnityEngine.Input.mousePosition.y / UnityEngine.Screen.height * self.main.rect.height)

	local touch_off = self.input_t - self.stick_init_pos

	local max_off_sqr = 10000
	if (touch_off:SqrMagnitude()) > max_off_sqr then
		touch_off = touch_off:Normalize() * 100
	end

    return touch_off
end

function TombJoyStick:OnJoyStickPressUp()
    if TombRaiderData.Instance:IsLimitedMove() then return end 
    -- if self.stick_update ~= nil then
    --     TimeHelper:CancelTimer(self.stick_update)
    --     self.stick_update = nil
    -- end
    local timer = 4 
    local mark_x = self.stick_off.x/timer
    local mark_y = self.stick_off.y/timer
    self.ShowTimer = TimeHelper:AddRunFrameTimer(function()
        self.stick_off.x = self.stick_off.x - mark_x
        self.stick_off.y = self.stick_off.y - mark_y
        timer = timer - 1
        if timer == 0 then
            self.stick_off = Vector3.zero
        end 
    end, 1,timer)

    -- self.stick_off = Vector3.zero
    -- self.show:SetLocalPosition(Vector3.zero)
    -- MainPlayer:MoveTo(MainPlayer:GetPos(),true)
    -- MainPlayer:StopMove()
end

function TombJoyStick:OnJoyStickPressDown()
    if TombRaiderData.Instance:IsLimitedMove() then return end 
    if self.stick_update ~= nil then return end 

    self.stick_update = TimeHelper:AddRunTimer(function()
        -- 被限制移动了的话就算在挪也不给刷
        if TombRaiderData.Instance:IsLimitedMove(false) then 
            self.show:SetLocalPosition(Vector3.zero)
            return 
        end 
        self.show:SetLocalPosition(self.stick_off)

        local dir_vec = Quaternion.Euler(0,135,0) * Vector3.New(self.stick_off.x,0,self.stick_off.y)
        local dir = dir_vec:Normalize()

        local distance = self.stick_off.x *self.stick_off.x + self.stick_off.y * self.stick_off.y
        if distance < 1000 then return end 

        local dir_ho = Vector3.New(self.stick_off.x,0,self.stick_off.y)
        local qua = self.main_camera:GetLocalEulerAngles()

        -- 话说主角都能丢失的吗
        if MainPlayer == nil then return end 

        local role_pos =  MainPlayer:GetPos()
        local pos_x = role_pos.x + dir.x * 2 * (distance/10000)
        local pos_z = role_pos.z + dir.z * 2 * (distance/10000)

        local param_t = nil
        local end_y,succes = SceneCtrl.Instance:CurScene():GetPosHeight(pos_x, pos_z)
        local end_point = Vector3.New(pos_x, end_y,pos_z)

        local flag_pos = not succes -- 
        if MainPlayer.nav_agent.isOnNavMesh and MainPlayer.nav_mesh_path ~= nil then 
            flag_pos = MainPlayer.nav_agent:CalculatePath(end_point, MainPlayer.nav_mesh_path)
        end 
        local adjust = false
        local angle = math.atan2(dir_ho.z, dir_ho.x)
        if not flag_pos then 
            end_point,param_t = JoyStickData.Instance:AdjustMovePoint(
                {
                    stick_off = self.stick_off,
                    camera_qua = qua,
                    distance = distance,
                    is_ant = not (math.deg(angle) >= -90 and math.deg(angle) < 90),
                    default = Vector3.New(pos_x,end_y,pos_z),
                }
            )
            adjust = true
        end 

        local dis = Vector3.Distance2(MainPlayer:GetPos(), MainPlayer:GetMoveEndRealPos() or MainPlayer:GetPos())
        local an
        if self.joystick_angle == 0 then
            an = true
        else
            an = math.abs(angle - self.joystick_angle) >= 0.26
        end
        local exe_stick = ((dis <= 4) or an)
    
        if exe_stick then
            self.joystick_angle = angle

            if (self.joystick_last_target_pos.x ~= end_point.x or self.joystick_last_target_pos.z ~= end_point.z) then
				self.joystick_last_target_pos = end_point
                MainPlayer:MoveTo(end_point,true)
            end 
        end 

    end,0.01)
end

--===========================TombLiveMapItem=====================================
TombTopPointBar = TombTopPointBar or DeclareMono("TombTopPointBar", UIWFlushPanel)
function TombTopPointBar:TombTopPointBar()
    self.data = TombRaiderData.Instance
    self.data_cares = {
        {data = self.data.game_start_state, func = self.FlushTimer, },--init_call = false},
        {data = self.data.tomb_team, func = self.OnFlush, },--init_call = false},
        {data = self.data.map_obj_info, func = self.OnFlush, },--init_call = false},
    }
end 

function TombTopPointBar:OnClickCampInfo()
    ViewMgr:OpenView(TombRaiderCampView)
end

function TombTopPointBar:OnFlush()
    local my_info = self.data:CacheMyTeamInfo()
    local my_team_type = my_info.team_type

    UH.SetText(self.emenyTex,Language.TombRaider.TeamTopName[my_team_type == 0 and 1 or 0])
    UH.SetText(self.ourTex, Language.TombRaider.TeamTopName[my_team_type])

    local team_num,emeny_num = self.data:GetMemberNum()

    -- LogError("??",team_num,emeny_num)
    UH.SetText(self.emenyPoint,emeny_num)
    UH.SetText(self.outPoint,team_num)
    UH.SetText(self.totalPoint,self.data:GetWinBoxNum())
    UH.SetText(self.featsTex,my_info.gongxian)
end 

function TombTopPointBar:FlushTimer()
    self.Time:StampTime(self.data.game_start_state.time_stamp,TimeType.Type_Time_0)
end

--==========================
TombViewRbBtnsRoot = TombViewRbBtnsRoot or DeclareMono("TombViewRbBtnsRoot", UIWFlushPanel)
function TombViewRbBtnsRoot:TombViewRbBtnsRoot()
    self.data_cares = {
        {data = TombRaiderData.Instance.battle_view_data, func = self.HideRb, keys = {"hide_rb"}, init_call = false},
        {data = TombRaiderData.Instance.battle_view_data, func = self.ShowRb, keys = {"show_all"}, init_call = false},
        {data = TombRaiderData.Instance.rb_toggle, func = self.FlushRbToggle, init_call = false},
    }
end

function TombViewRbBtnsRoot:OnToggleRbBtns(flag)
     if flag == true then
        self.RbRedPoint:SetNum(0)
     else
         self:FlushRbRedPoint()
     end
     TombRaiderData.Instance.rb_toggle.isOn = flag
 end

function TombViewRbBtnsRoot:FlushRbRedPoint()
    local nums = MainViewData.Instance:RbRmindNums()
    for key, value in pairs(nums) do
        if value > 0 then
            self.RbRedPoint:SetNum(1)
            break
        end
    end
end
function TombViewRbBtnsRoot:HideRb()
    self.RbBtnAnim:SetBool("state", false)
    self:OnToggleRbBtns(false)
end
function TombViewRbBtnsRoot:ShowRb()
    self.RbBtnAnim:SetBool("state", true)
    self:OnToggleRbBtns(true)
end

function TombViewRbBtnsRoot:FlushRbToggle()
    if self.RbToggle.isOn == TombRaiderData.Instance.rb_toggle.isOn then return end
    self.RbToggle.isOn = TombRaiderData.Instance.rb_toggle.isOn
end

TombBattleRbBtnsSub = TombBattleRbBtnsSub or DeclareMono("TombBattleRbBtnsSub", UIWFlushPanel)
function TombBattleRbBtnsSub:OnEnable()
    UIWFlushPanel.OnEnable(self)
    self.RbBtnAnim:Rebind()
    self.RbBtnAnim:SetBool("state", false)

    self.RbBtnpartAnim:Rebind()
    self.RbBtnpartAnim:SetBool("state", false)
end

function TombBattleRbBtnsSub:OnDestroy()
    UIWFlushPanel.OnDestroy(self)
    self.BtnList:OnDestroy()
    for i = 1,8 do 
        self.Second.BtnList[i]:OnRelease()
    end 
end

TombBattleViewSecondBtns = TombBattleViewSecondBtns or DeclareMono("TombBattleViewSecondBtns", UIWFlushPanel)
function TombBattleViewSecondBtns:TombBattleViewSecondBtns()
    self.bag_data = BagData.Instance
    -- EventSys:Bind(GlobalEventKeys.RoleLevelChange, BindTool.Bind(self.onFlush, self))
    self.data_cares = {
        -- {data = LoveData.Instance.marry_data, func = self.onFlush, keys = {"love_uid"}},--情缘屏蔽
        {data = self.bag_data:GetColumnGrids(ItemColumnType.Temporary), func = self.onFlush, init_call = false},
        {data = self.bag_data.better_equip, func = self.FlushEquipBetter, keys = {"is_better"}},
        {data = TreasureHuntData.Instance.smart_data, func = self.onFlush, keys = {"FlushBtn"}, init_call = false},
        {data = LoginData.Instance.base_data, func = self.onFlush, keys = {"login_state"}},
    }
end
function TombBattleViewSecondBtns:OnDestroy()
    UIWFlushPanel.OnDestroy(self)
end 

function TombBattleViewSecondBtns:OnEnable()
    UIWFlushPanel.OnEnable(self)
    -- GuideManager.Instance:RegisterGetGuideUi(
    --     "BagViewOpenBtn",
    --     function()
    --         if ViewMgr:IsOpen(KeyUseView) then
    --             local item = KeyUseData.Instance:GetItem()
    --             MainOtherCtrl.Instance:SetWaitViewFunc(function()
    --                 MainOtherCtrl.Instance:OpenKeyUseView(item)
    --             end)
    --             ViewMgr:CloseView(KeyUseView)
    --         end
    --         MainViewData.Instance:ShowRbButton()
    --         return self.BtnObjList[1], function()
    --             ViewMgr:OpenView(BagView)
    --         end
    --     end
    -- )
    -- GuideManager.Instance:RegisterGetGuideUi(
    --     "TreasureHuntViewBtn",
    --     function()
    --         MainViewData.Instance:ShowRbButton()
    --         for i = 1, 8, 1 do
    --             if self.BtnList[i]:GetData() and self.BtnList[i]:GetData().sprite == "XunBao" then
    --                 return self.BtnObjList[i],function ()
    --             ViewMgr:OpenView(TreasureHuntView)
    --         end
    --     end
    --         end
    --         --[[ return self.BtnObjList[5], function()
    --             ViewMgr:OpenView(TreasureHuntView)
    --         end ]]
    --     end
    -- )
    -- GuideManager.Instance:RegisterGetGuideUi(
    --     "WorkshopBtn",
    --     function()
    --         MainViewData.Instance:ShowRbButton()
    --         for i = 1, 8, 1 do
    --             if self.BtnList[i]:GetData() and self.BtnList[i]:GetData().sprite == "GongFang" then
    --                 return self.BtnObjList[i],function ()
    --                     ViewMgr:OpenView(Workshop)
    --                 end
    --             end
    --         end
    --     end
    -- )
    -- if IS_IPHONEX or EditorSettings:SimularIphoneX() then
    --     GuideManager.Instance:RegisterGetGuideUi(
    --         "IllustratedBtn",
    --         function()
    --             MainViewData.Instance:ShowRbButton()
    --             for i = 1, 9 do
    --                 if self.BtnList[i] and self.BtnList[i]:GetData() and self.BtnList[i]:GetData().sprite == "TuJian" then
    --                     return self.BtnObjList[i],function ()
    --                         ViewMgr:OpenView(IllustrateView)
    --                     end
    --                 end
    --             end
    --         end
    --     )
    -- end
    self:onFlush()
end

local second_btn_config
local function get_second_btn_config()
    if second_btn_config == nil then
        second_btn_config =
        {
            {view = BagView, text = Language.Main.BeiBao, sprite = "BeiBao", pos = 1,mod = Mod.Bag},
            {view = Workshop, text = Language.Main.GongFang, sprite = "GongFang", mod = Mod.Workshop, open = Mod.Workshop, pos = 1},
            {view = FormationView, text = Language.Main.Formation, sprite = "BuZhen", mod = Mod.Formation, open = Mod.Formation, pos = 1},
            {view = GuildList, text = Language.Main.JiaZu, sprite = "JiaZu", mod = Mod.Guild, open = Mod.Guild, pos = 1},
            {view = LiveView, text = Language.Main.ShengHuo, sprite = "ShengHuo", mod = Mod.Live, open = Mod.Live, pos = 1},
            {view = AchieveNewView, text = Language.Main.ChengJiu, sprite = "ChengJiu", mod = Mod.Achieve, open = Mod.Achieve, pos = 1},
            {view = TreasureHuntView, text = Language.Main.XunBao, sprite = "XunBao", mod = Mod.TreasureHunt, open = Mod.TreasureHunt, pos = 1,
                lock_func = function()
                    local list = TreasureHuntData.Instance:GetOpenedTreasureID()
                    return next(list) ~= nil end},
            {view = KnightlyView, text = Language.Main.XiaYi, sprite = "XiaYi", mod = Mod.Knight, open = Mod.Knight.Main, pos = 1,
            },
        }
    end
    return second_btn_config
end

local rb_btn_config
local function get_rb_btn_config()
    -- 上下两行 由下到上排 pos=1右下
    if rb_btn_config == nil then
        rb_btn_config = {
            {view = RolePromoteView, text = Language.Main.Promote, sprite = "JinSheng", mod = Mod.RolePromote, open = Mod.RolePromote, pos = 1},
            {view = EquipView, text = Language.Main.ZhuangBei, sprite = "ZhuangBei", mod = Mod.Equip, open = Mod.Equip, pos = 1},
            {view = DivineCostumeView, text = Language.Main.DivineCostume, sprite = "ShenZhuang", mod = Mod.DivineCostume, open = Mod.DivineCostume, pos = 1},
            {view = PartnerView, text = Language.Main.HuoBan, sprite = "HuoBan", mod = Mod.Partner, open = Mod.Partner, pos = 1},
            {view = BicycleView, text = Language.Main.LingQi, sprite = "LingQi", mod = Mod.Bicycle, open = Mod.Bicycle, pos = 1},
            {view = FabaoView, text = Language.Main.FaBao, sprite = "FaBao", open = Mod.Fabao, mod = Mod.Fabao, pos = 1},
            {view = IllustrateView, text = Language.Main.Illustrate, sprite = "TuJian", mod = Mod.Illustrate, open = Mod.Illustrate, pos = 1},
            {view = AthleticsView, text = Language.Main.JingJi, sprite = "JingJi", mod = Mod.Athletics, open = Mod.Athletics, pos = 1},
            {view = ChallengPopupView, text = Language.Main.TiaoZhan, sprite = "TiaoZhan",open = Mod.Challenge, pos = 1, mod = Mod.Challenge},
        }
    end
    
    return rb_btn_config
end

function TombBattleViewSecondBtns:onFlush()
    if LoginData.Instance:BaseData().login_state ~= LoginStateType.Logined then
        return
    end
    local btn_config = DataHelper.TableCopy(get_second_btn_config())
    if BagData.Instance:CheckTempBagEmpty() then
        table.insert(btn_config, 2, {view = TempBag, text = Language.Main.LinShiBeiBao, sprite = "LinShiBeiBao"})
    end
    local first_line_btn_config = get_rb_btn_config()
    local count = #first_line_btn_config--get_rb_btn_open_num()
    if count > 8 and (IS_IPHONEX or EditorSettings:SimularIphoneX())then
        table.insert(btn_config, first_line_btn_config[count - 2])
    end
    local num = table.nums(btn_config)
    for i = 1, 9, 1 do
        local data = btn_config[i]
        self.BtnList[i]:OnRelease()
        if i <= num then
            self.BtnList[i]:SetData(data)
        else
            self.BtnObjList[i]:SetActive(false)
        end
    end
end

function TombBattleViewSecondBtns:FlushEquipBetter()
    self.Is_equip_better:SetActive(self.bag_data.better_equip.is_better)
end

TombBattleViewRbBtns = TombBattleViewRbBtns or DeclareMono("TombBattleViewRbBtns", UIWFlushPanel)
function TombBattleViewRbBtns:TombBattleViewRbBtns()
    -- GuideManager.Instance:RegisterGetGuideUi(
    --     "EquipBtn",
    --     function()
    --         MainViewData.Instance.view_data.show_all = not MainViewData.Instance.view_data.show_all
    --         for k, v in pairs(self.BtnList:GetData()) do
    --             if v.sprite == "ZhuangBei" then
    --                 return self.BtnList.item_list[self.BtnList:GetSelectedIndex(v)],function ()
    --                     ViewMgr:OpenView(EquipView)
    --                 end
    --             end
    --         end
    --     end
    -- )
    -- GuideManager.Instance:RegisterGetGuideUi(
    --     "PartnerBtn",
    --     function()
    --         MainViewData.Instance:ShowRbButton()
    --         for k, v in pairs(self.BtnList:GetData()) do
    --             if v.sprite == "HuoBan" then
    --                 return self.BtnList.item_list[self.BtnList:GetSelectedIndex(v)],function ()
    --                     ViewMgr:OpenView(PartnerView)
    --                 end
    --             end
    --         end
    --     end
    -- )
    -- if not (IS_IPHONEX or EditorSettings:SimularIphoneX()) then 
    --     GuideManager.Instance:RegisterGetGuideUi(
    --         "IllustratedBtn",
    --         function()
    --             MainViewData.Instance:ShowRbButton()
    --             for k, v in pairs(self.BtnList:GetData()) do
    --                 --LogError("sprite",v.sprite)
    --                 if v.sprite == "TuJian" then
    --                     return self.BtnList.item_list[self.BtnList:GetSelectedIndex(v)],function ()
    --                         ViewMgr:OpenView(IllustrateView)
    --                     end
    --                 end
    --             end
    --         end
    --     )
    -- end
    -- GuideManager.Instance:RegisterGetGuideUi(
    --     "ChallengeBtn",
    --     function()
    --         MainViewData.Instance:ShowRbButton()
    --         for k, v in pairs(self.BtnList:GetData()) do
    --             if v.sprite == "TiaoZhan" then
    --                 return self.BtnList.item_list[self.BtnList:GetSelectedIndex(v)],function ()
    --                     ViewMgr:OpenView(ChallengeNaviView)
    --                 end
    --             end
    --         end
    --     end
    -- )
    -- GuideManager.Instance:RegisterGetGuideUi(
    --     "FabaoBtn",
    --     function()
    --         MainViewData.Instance:ShowRbButton()
    --         for k, v in pairs(self.BtnList:GetData()) do
    --             if v.sprite == "FaBao" then
    --                 return self.BtnList.item_list[self.BtnList:GetSelectedIndex(v)],function ()
    --                     ViewMgr:OpenView(FabaoNeoView)
    --                 end
    --             end
    --         end
    --     end
    -- )
    -- GuideManager.Instance:RegisterGetGuideUi(
    --     "BicycleMain",
    --     function()
    --         MainViewData.Instance:ShowRbButton()
    --         for k, v in pairs(self.BtnList:GetData()) do
    --             if v.sprite == "LingQi" then
    --                 return self.BtnList.item_list[self.BtnList:GetSelectedIndex(v)],function ()
    --                     ViewMgr:OpenView(BicycleView)
    --                 end
    --             end
    --         end
    --     end
    -- )
    -- GuideManager.Instance:RegisterGetGuideUi(
    --     "AthleticsBtn",
    --     function()
    --         MainViewData.Instance:ShowRbButton()
    --         for k, v in pairs(self.BtnList:GetData()) do
    --             if v.sprite == "JingJi" then
    --                 return self.BtnList.item_list[self.BtnList:GetSelectedIndex(v)],function ()
    --                     ViewMgr:OpenView(AthleticsView)
    --                 end
    --             end
    --         end
    --     end
    -- )
    -- GuideManager.Instance:RegisterGetGuideUi(
    --     "RolePromoteBtn",
    --     function()
    --         MainViewData.Instance:ShowRbButton()
    --         for k, v in pairs(self.BtnList:GetData()) do
    --             if v.sprite == "JinSheng" then
    --                 return self.BtnList.item_list[self.BtnList:GetSelectedIndex(v)],function ()
    --                     ViewMgr:OpenView(RolePromoteView)
    --                 end
    --             end
    --         end
    --     end
    -- )
    -- EventSys:Bind(GlobalEventKeys.RoleLevelChange, BindTool.Bind(self.onFlush, self))
    self.data_cares = {
        {data = MainViewData.Instance.view_data, func = self.onFlush, keys = {"flush_rb"}},
    }
end 

function TombBattleViewRbBtns:OnRelease()
    
end

function TombBattleViewRbBtns:onFlush()
    if LoginData.Instance:BaseData().login_state ~= LoginStateType.Logined then
        return
    end
    local rb_config = DataHelper.TableCopy(get_rb_btn_config())
    local count = #rb_config--get_rb_btn_open_num()  --#48090 缺陷 G6-LineR（版本发布线） 升到60级后出现两个竞技按钮 的问题
    if count > 8 and (IS_IPHONEX or EditorSettings:SimularIphoneX())then
        table.remove(rb_config, #rb_config - 2)
    end
    self.BtnList:SetData(rb_config) --因为BtnItem里有N多监听，还会自己开关自己，必需每次传进来的table数一样，如果比原来少就会出问题
end

TombViewModBtn = TombViewModBtn or DeclareMono("TombViewModBtn", UIWidgetBaseItem)
function TombViewModBtn:SetData(data)
    self.ModRedPoint:SetNum(0, true)
    UH.SetText(self.Label, data.text)
    UH.SpriteName(self.Icon, data.sprite)
    if data.view == ChallengPopupView then
        -- self.challenge_sub:SetParent(self.gameObject)
        self.courage_challenge_sub:SetParent(self.gameObject)
        -- UH.LocalPosG(self.challenge_sub, Vector3.zero)
        UH.LocalPosG(self.courage_challenge_sub, Vector3.zero)
        self.courage_challenge_sub:SetActive(true)
    elseif data.view == IllusionPopupView then
        self.illusion_sub:SetParent(self.gameObject)
        UH.LocalPosG(self.illusion_sub, Vector3.zero)
    elseif data.view == BicycleView then
        self.BicycleTipSub:SetParent(self.gameObject)
        UH.LocalPosG(self.BicycleTipSub, Vector3.zero)
        self.BicycleTipSub:SetActive(true)
    elseif data.view == PetRefining and self.LianYaoSub then
        self.LianYaoSub:SetParent(self.gameObject)
        UH.LocalPosG(self.LianYaoSub, Vector3.zero)
        self.LianYaoSub:SetActive(true)
    end
    
    if data.view == TempBag then
        self.ModRedPoint:SetNum(1, true)
    end

    if data.view == TreasureHuntView then
        if self.XianShi then
            local list, show_corner = TreasureHuntData.Instance:GetOpenedTreasureID()
            self.XianShi:SetActive(show_corner)
        end
    elseif self.XianShi then
        self.XianShi:SetActive(false)
    end

    --工坊监听 到某等级后仍有低级装备 提示该换装备了
    -- if data.view == Workshop or data.view == ChallengPopupView then
    --     self:InitRoleLevelGuide()
    --     self.handle_role_level = RoleData.Instance:GetBaseData():Care(BindTool.Bind(self.RoleLevelGuide, self), "level")
    -- end

    if self.EffectTool and data.effect then
        Runner.Instance:RunUntilTrue(
            function()
                if self.handle_effect == nil then
                    self.handle_effect = self.EffectTool:Play(data.effect, self.gameObject)
                end
                return true
            end
        )
    end
    if data.mod then
        local num = RemindCtrl.Instance:Remind():GetGroupNum(data.mod)
        if data.pos and data.pos == 1 then
            MainViewData.Instance:FlushRbRedPoint(data.mod, num)
        end
        if self.ModRedPoint then self.ModRedPoint:SetNum(num, true) end 
    else
        self.ModRedPoint:SetNum(0, true)
    end
    UIWidgetBaseItem.SetData(self, data)
    if data.open then
            --当从false变到true的时候显示new图标
        self.gameObject:SetActive(FunOpen.Instance:CheckOpen(data.open))
        if data.lock_func then
            self.gameObject:SetActive(data.lock_func())
        end
    else
        if data.lock_func then
            self.gameObject:SetActive(data.lock_func())
        else
            self.gameObject:SetActive(not data.close)
        end
    end
end 
function TombViewModBtn:InitRoleLevelGuide()
    if self.handle_role_level then
        RoleData.Instance:GetBaseData():Uncare(self.handle_role_level)
        self.handle_role_level = nil
    end
    if self.guide_tip then
        self.guide_tip:DeleteMe()
        self.guide_tip = nil
    end
end
function TombViewModBtn:OnClick(data)
    if self.data == nil then return end
    if GuildCtrl.IsHasGuild(self.data.view) then --家族的处理
        return
    end
    if self.data.view == ChallengPopupView then
        AudioMgr:PlayEffect(AudioType.UI, CommonAudio.QieHuanZhanKai)
        -- self.challenge_sub:SetActive(not self.challenge_sub.activeSelf)
        ViewMgr:OpenView(ChallengeNaviView)
        MainOtherData.Instance:MutualViewFunc("challenge_sub")
    elseif self.data.view == IllusionPopupView then
        AudioMgr:PlayEffect(AudioType.UI, CommonAudio.QieHuanZhanKai)
        self.illusion_sub:SetActive(not self.illusion_sub.activeSelf)
        MainOtherData.Instance:MutualViewFunc("illusion_sub")
    -- elseif self.data.view == StoreView then
    --     StoreData.Instance:SetJumpIndex({conver_type = 1})
    --     ViewMgr:OpenViewByKey(Mod.Store.Main)
    else
        MainOtherData.Instance:MutualViewFunc()
        ViewMgr:OpenView(self.data.view or GetView(self.data.view_name))
    end
end

function TombViewModBtn:OnRelease()
    if self.data == nil then return end
    TimeHelper:CancelTimer(self.handle_delay)
    self:InitRoleLevelGuide()
    UIWidgetBaseItem.OnRelease(self)
end 

function TombViewModBtn:GetActiveSelf()
    return self.gameObject.activeSelf
end

TombStartCount = TombStartCount or DeclareMono("TombStartCount", UIWFlushPanel)
function TombStartCount:TombStartCount()
    self.data = TombRaiderData.Instance
    self.data_cares = {
        {data = self.data.game_start_state, func = self.FlushTimer, },
        {data = self.data.tomb_team, func = self.FlushStartInfo, },
    }
end 

function TombStartCount:FlushStartInfo() 
    local data = self.data:CacheMyTeamInfo()
    local my_team_type = data.team_type
    UH.SpriteName(self.team_title, my_team_type == 0 and "Loc_ShouWeiZhe" or "Loc_DaoBaoZhe")
    UH.SetText(self.team_target, Language.TombRaider.TeamTarget[my_team_type])
end 

function TombStartCount:FlushTimer()
    -- LogError("!?@!@", self.data.game_start_state.reason_type )
    if self.data.game_start_state.reason_type == 1 then 
        local show_num = 3 --self.data.game_start_state.time_stamp
        self.count_time = TimeHelper:AddCountDownTT(function()
            self.total:SetActive(true)
            if show_num <= 3 then
                self.countnum.text = show_num
            end
            show_num = show_num - 1
        end,
        function()
            self.total:SetActive(false)
            self.countnum.text = 0
        end,3,1,true)
    -- else 
    --     self.total:SetActive(false)
    end 
end

TombBattleGuide = TombBattleGuide or DeclareMono("TombBattleGuide", UIWFlushPanel)
function TombBattleGuide:TombBattleGuide()
    Runner.Instance:AddRunObj(self, 16)

    self.data = TombRaiderData.Instance

    self.guide_list = {}
    self.special_list = {}
    self.guide_list,self.special_list = self.data:GetGuideList()

    self.guide_index = 0
    self.guide_total = #self.guide_list
    self.guide_pass = {}

    self.guide_timer = nil
    self.delay_timer = nil

    self.special_guide_timer = nil
end 

function TombBattleGuide:OnDestroy()
    UIWFlushPanel.OnDestroy(self)
    Runner.Instance:RemoveRunObj(self)

    TimeHelper:CancelTimer(self.delay_timer)
    TimeHelper:CancelTimer(self.guide_timer)
    TimeHelper:CancelTimer(self.special_guide_timer)
end 

function TombBattleGuide:Update()
    if not self.data:GetIsNeedGuide() then 
        return 
    end 

    if self:IsGameReadying() then 
        return 
    end 
    self:GuideUpdate()
    self:SpecialGuideUpdate()
   --  if self.
end

--
function TombBattleGuide:IsGameReadying()
    return self.CountDown.activeSelf == true or self.data.game_start_state.reason_type < 2
end

-- 是否展示普通引导展示
function TombBattleGuide:CanShowGuide()
    return self.MapShowObj.activeSelf == false
end

-- 普通引导是否已经结束
function TombBattleGuide:IsCompleteGuide()
    return self.guide_index == self.guide_total
end

-- 特殊引导是否已经结束
function TombBattleGuide:IsSpecialCompleteGuide()
    local total = #self.special_list
    local index = 0
    for k,v in pairs(self.special_list) do 
        if self:IsGuidePass(v.step) then index = index + 1 end 
    end 

    return total <= index
end

function TombBattleGuide:IsGuidePass(step_id)
    return self.guide_pass[step_id]
end

function TombBattleGuide:IsGuideIndexPass()
    for k,v in pairs(self.guide_list[self.guide_index+1]) do 
        if self:IsGuidePass(v.step) then 
            return true
        end 
    end 
    return false
end

function TombBattleGuide:GuideUpdate()
    if not self:CanShowGuide() then return end 
    if self:IsCompleteGuide() then return end 
    if self.guide_timer ~= nil or self.delay_timer ~= nil then return end 
    if self.guide_list == nil then return end 
    -- 获取信息
    local cfg = self.guide_list[self.guide_index+1]
    if cfg == nil then 
        LogDG("?!报空 ",self.guide_index+1,self.guide_list)
        return 
    end 

    if self:IsGuideIndexPass() then return end 
    -- 执行引导操作

    for i=1,self.poses:Length() do
        if cfg[i] ~= nil then 
            self.poses[i]:FlushGuide(cfg[i])
        else 
            self.poses[i]:StopGuide()
        end 
    end 

    -- 执行引导结束操作
    self.guide_timer = TimeHelper:AddDelayTimer(function ()
        for i=1,self.poses:Length() do 
            self.poses[i]:StopGuide() 
            if cfg[i] then self.guide_pass[cfg[i].step] = cfg[i] end 
        end 
        self.guide_index = self.guide_index + 1
        self.delay_timer = TimeHelper:AddDelayTimer(function ()
            self.guide_timer = nil 
            self.delay_timer = nil 
        end,cfg[1].end_delay_time)
    end,cfg[1].last_time)
end 

function TombBattleGuide:SpecialGuideUpdate()
    if #self.special_list == 0 then return end 
    if self:IsSpecialCompleteGuide() then return end
    if self.special_guide_timer ~= nil then 
        -- 若当前指引中的为地图宝箱，在地图界面关闭时需要直接终止指引
        if self:CanShowGuide() and self.pos_special.cfg.guide_param == "MapShowGuide" then 
            self.pos_special:StopGuide()
            self.mark_pos:SetParent(self.total.gameObject)
        -- 若当前指引中的为道具拾取，在地图界面开启时需要直接终止指引
        elseif not self:CanShowGuide() and self.pos_special.cfg.guide_param == "PickGuide" then 
            self.pos_special:StopGuide()
            self.mark_pos:SetParent(self.total.gameObject)
        end 
        -- 注意 特殊指引只能同时存在一个
        self.special_guide_timer = nil
        return 
    end 

    for k,v in pairs(self.special_list) do 
        -- 本指引未指引
        if not self:IsGuidePass(v.step) then
            -- 地图宝箱指引
            if v.guide_param == "MapShowGuide" then 
                -- 在展示地图界面时
                if not self:CanShowGuide() then 
                    for i,j in pairs(self.MapItemList.item_list) do 
                        -- 对象为地图宝箱
                        if j:GetData().type == TombRaiderConfig.BattleMapIconType.Box or
                           j:GetData().type == TombRaiderConfig.BattleMapIconType.Ruins then 

                            -- 锚定对象
                            self.mark_pos:SetParent(j.gameObject,false)
                            UH.LocalPosG(self.mark_pos, Vector3.zero)
                            local lock_pos = self.total.gameObject.transform:InverseTransformPoint(self.mark_pos.transform.position)

                            -- 修正位置，开始指引
                            local paramt = TableCopy(v)
                            paramt.pos_x =  lock_pos.x + v.pos_x
                            paramt.pos_y =  lock_pos.y + v.pos_y
                            self.pos_special:FlushGuide(paramt)

                            self.special_guide_timer = TimeHelper:AddDelayTimer(function ()
                                if not ViewMgr:IsOpen(TombRaiderBattleView) then return end 

                                self.pos_special:StopGuide() 
                                self.mark_pos:SetParent(self.total.gameObject)
                                self.guide_pass[paramt.step]= paramt
                            end,paramt.last_time)

                            break
                        end 
                    end 
                end 
            -- 拾取按钮指引
            elseif v.guide_param == "PickGuide" then 
                -- 在地图界面没有打开时
                if self:CanShowGuide() and self.PickItemObj.activeSelf == true then
                    -- 修正位置 开始指引
                    self.pos_special:FlushGuide(v) 
                    self.special_guide_timer = TimeHelper:AddDelayTimer(function ()
                        if not ViewMgr:IsOpen(TombRaiderBattleView) then return end 

                        self.pos_special:StopGuide() 
                        self.guide_pass[v.step]= v
                    end,v.last_time)

                    break
                end 
            elseif v.guide_param == "PointGuide" then 
                if self:CanShowGuide() and self.data:CacheMyTeamInfo() ~= nil and self.data:CacheMyTeamInfo().gongxian ~= nil and self.data:CacheMyTeamInfo().gongxian > 0 then 
                    -- 修正位置 开始指引
                    self.pos_special:FlushGuide(v) 
                    self.special_guide_timer = TimeHelper:AddDelayTimer(function ()
                        if not ViewMgr:IsOpen(TombRaiderBattleView) then return end 

                        self.pos_special:StopGuide() 
                        self.guide_pass[v.step]= v
                    end,v.last_time)

                    break
                end 
            end 
        end 
    end 
end

TombBattleGuidePosCell = TombBattleGuidePosCell or DeclareMono("TombBattleGuidePosCell", UIWidgetBaseItem)
function TombBattleGuidePosCell:TombBattleGuidePosCell() end 
function TombBattleGuidePosCell:FlushGuide(cfg)
    self.cfg = cfg
    self.guide_pos:SetActive(true)
    if self.cfg.advance == 0 or self.guide_master[self.cfg.guide_param] == nil then 
        UH.LocalPos(self.pos, Vector2.New(cfg.pos_x, cfg.pos_y))
    elseif self.cfg.advance == 1 then  
        -- 抛掷锚点
        self.mark_pos:SetParent(self.guide_master[self.cfg.guide_param],false)
        UH.LocalPosG(self.mark_pos, Vector3.zero)
        UH.AnchoredPosition(self.mark_pos:GetComponent(typeof(UnityEngine.RectTransform)), Vector2.zero)

        -- 锚点位置定位
        self.pos:SetParent(self.mark_pos:GetComponent(typeof(UnityEngine.RectTransform)),false)
        UH.LocalPos(self.pos,  Vector2.New(cfg.pos_x2, cfg.pos_y2))

        -- 归位
        self.pos:SetParent(self.guide_master.GuideSelf:GetComponent(typeof(UnityEngine.RectTransform)),true)
        if self.pos ~= nil then UH.LocalScale(self.pos, Vector3.New(1, 1, 1)) end 
    end 

    UH.SetText(self.guide_desc, TombRaiderData.Instance:CacheMyTeamInfo().team_type == 0 and cfg.guide_dec_shouwei or cfg.guide_dec_daobao)
    UH.SizeDelta(self.guide_bg, Vector2.New(cfg.bottom_long, cfg.bottom_wide))
    UH.LocalPos(self.guide_arrow,Vector2.New(cfg.arrow_x, cfg.arrow_y))
    UH.LocalRotate(self.guide_arrow,Quaternion.Euler(0,cfg.is_mirror*180,cfg.arrow_direction*(90)))
end

function TombBattleGuidePosCell:StopGuide()
    if not ViewMgr:IsOpen(TombRaiderBattleView) then return end 
    if self.guide_pos == nil or self.mark_pos == nil then return end 
    self.guide_pos:SetActive(false)
    self.mark_pos:SetParent(self.guide_master.GuideSelf.gameObject)
end
