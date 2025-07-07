LandsBetweenMainView = LandsBetweenMainView or DeclareView("LandsBetweenMainView", "lands_between/lands_between_main",Mod.LandsBetween.Main)
VIEW_DECLARE_MASK(LandsBetweenMainView,ViewMask.Block)
VIEW_DECLARE_LEVEL(LandsBetweenMainView, ViewLevel.L0)
function LandsBetweenMainView:LandsBetweenMainView() 

    MainOtherCtrl.Instance:CloseKeyUse()
end
function LandsBetweenMainView:LoadCallback(param_t)
    self.delay_init_model_scene = TimeHelper:AddDelayFrameTimer(function ()
        self:InitModelScene()
        self:InitQualitySettings()
        self:InitProtocol()
        LandsBetweenData.Instance:MainOpenCall()
    end,1)

    self.Panel:FlushSlowStart()
    -- if LandsBetweenData.Instance.phasetask_info.flush > 0 then 
    --     ViewMgr:OpenView(LandsBetweenPosterView)
    -- end 
end

function LandsBetweenMainView:OpenCallback()
    ViewMgr:CloseView(ChatView)
    BattleLuaFunction.UIHide()

    self.ActPanel:FlushList()
end

function LandsBetweenMainView:OnCloseClick()
    ViewMgr:CloseView(LandsBetweenMainView)
end

function LandsBetweenMainView:InitProtocol()
    LandsBetweenCtrl.Instance:MainViewUpdateStart()
end 

function LandsBetweenMainView:CloseCallback()
    LandsBetweenData.Instance.draging = false
    LandsBetweenData.Instance:MainCloseCall()

    BattleLuaFunction.UIShow()

    if LandsBetweenCtrl.CloseFunc then
        LandsBetweenCtrl.CloseFunc()
        LandsBetweenCtrl.CloseFunc = nil
    end

    if self.obj then
        UnityGameObj.Destroy(self.obj)
    end
    if self.delay_init_model_scene ~= nil then
        TimeHelper:CancelTimer(self.delay_init_model_scene)
        self.delay_init_model_scene = nil
    end
    if self.delay_closelight ~= nil then
        TimeHelper:CancelTimer(self.delay_closelight)
        self.delay_closelight = nil
    end
    if nil ~= self.scene_care_handle then
        SceneMgr:UncareLoadedSceneIdChange(self.scene_care_handle)
        self.scene_care_handle = nil
    end

    LandsBetweenCtrl.Instance:MainViewUpdateEnd()

    if DayAndNight.obj then
        DayAndNight.Restore()
    end
end 

function LandsBetweenMainView:InitModelScene()
    ResMgr:LoadGameObject("actor/other/LandsBetween.prefab", function (obj)
        DayAndNight.Stop(1)
        self.obj = obj
        self.obj.transform.localPosition = Vector3.New(2000,2000,2000)
        self.obj:SetParent(UnityGameObj.Find("ResidentObjectManager"))
        self.delay_closelight = TimeHelper:AddDelayFrameTimer(function()
            self:CloseLight()
        end,5)
        
        self.Panel:InitComplete()
        self.Panel.RecordList:FlushRecordList()
    end, true, false)
end

function LandsBetweenMainView:InitQualitySettings()
    self:OnQualitySettings()
    LandsBetweenCtrl.CloseFunc = function ()
        local quality = RoleSettingData.Instance:GetData(ROLE_SETTING_TYPE.SET_PICTUER_QUALITY)
        if quality > 0 then
            GpQualityMgr:SetShadowEnable(true)
        end
        if self.light_coms then
            for i=0, self.light_coms.Length - 1 do
                if self.light_coms[i] ~= nil and not self.light_coms[i]:Equals(nil) then
                    self.light_coms[i].enabled = true
                end
            end
        end
        UnityEngine.RenderSettings.fog = self.fog
        UnityEngine.RenderSettings.ambientLight = self.ambientLight
        UnityEngine.QualitySettings.pixelLightCount = self.pixelLightCount
        UnityEngine.QualitySettings.anisotropicFiltering = self.anisotropicFiltering
        UnityEngine.QualitySettings.shadows = self.shadows
        UnityEngine.QualitySettings.shadowResolution = self.shadowResolution
        UnityEngine.QualitySettings.shadowCascades = self.shadowCascades
        UnityEngine.QualitySettings.shadowDistance = self.shadowDistance
        -- if self.scene_sun ~= nil and self.scene_sun.intensity ~= nil then 
        --     self.scene_sun.intensity = self.sun_intensity
        -- end 

        local main_cam_com = SceneMgr:MainCamera()
        if main_cam_com ~= nil then
            main_cam_com.enabled = true
        end

        LandsBetweenData.Instance:EndShowMark()
    end

    self.scene_care_handle = SceneMgr:CareLoadedSceneIdChange(BindTool.Bind(self.OnQualitySettings,self, true))
    LandsBetweenData.Instance:SetShowMark()
end

function LandsBetweenMainView:CloseLight()
    local quality_obj = SceneMgr.FindGameObjectWithTag("Quality")
    if quality_obj then
        self.light_coms = quality_obj:GetComponentsInChildren(typeof(UnityEngine.Light))
        for i = 0, self.light_coms.Length - 1 do
            self.light_coms[i].enabled = false
        end

        -- local quality_csharp = quality_obj:GetComponent(typeof(Crystal.Quality))
        -- self.scene_sun = quality_csharp.SceneSun
        -- if self.scene_sun ~= nil then 
        --     self.sun_intensity = self.scene_sun.intensity
        --     self.scene_sun.intensity = 1
        -- end 
    end

    if self.obj == nil then 
        return 
    end 

    local map_mono = self.obj:GetLuaComponent("LandsBetweenMapModel")
    map_mono.camera.enabled = true

    -- map_mono.Light:SetActive(self.scene_sun == nil)

    local main_cam_com = SceneMgr:MainCamera()
    if main_cam_com ~= nil then
        main_cam_com.enabled = false
    end
    -- if DayAndNight.obj then
    -- end
end

function LandsBetweenMainView:OnQualitySettings(close_light)
    GpQualityMgr:SetShadowEnable(false)
    if close_light then
        self:CloseLight()
    end
    self.fog = UnityEngine.RenderSettings.fog
    self.ambientLight = UnityEngine.RenderSettings.ambientLight
    self.pixelLightCount = UnityEngine.QualitySettings.pixelLightCount
    self.anisotropicFiltering = UnityEngine.QualitySettings.anisotropicFiltering
    self.shadows = UnityEngine.QualitySettings.shadows
    self.shadowResolution = UnityEngine.QualitySettings.shadowResolution
    self.shadowCascades = UnityEngine.QualitySettings.shadowCascades
    self.shadowDistance = UnityEngine.QualitySettings.shadowDistance
    UnityEngine.RenderSettings.fog = false
    UnityEngine.RenderSettings.ambientLight = COLORS.LandsBetween
    UnityEngine.QualitySettings.pixelLightCount = 20
    UnityEngine.QualitySettings.anisotropicFiltering = 2
    UnityEngine.QualitySettings.shadows = 3
    UnityEngine.QualitySettings.shadowDistance = 100
    UnityEngine.QualitySettings.shadowResolution = 2
    UnityEngine.QualitySettings.shadowCascades = 4

    
end

LandsBetweenMainPanel = LandsBetweenMainPanel or DeclareMono("LandsBetweenMainPanel", UIWFlushPanel)
function LandsBetweenMainPanel:LandsBetweenMainPanel()
    self.data = LandsBetweenData.Instance

	self.map_x = 0
	self.map_y = 0

    self.data_cares = {
        {data = self.data.operating_info, func = self.FlushOperatingObj,keys = {"obj"},init_call = false},
        -- {data = self.data.operating_info , func = self.FlushCurMapShow,keys = {"map_load_mark"},init_call = false},
        -- {data = self.data.net_role_info , func = self.FlushCurMapShow,init_call = false},
        {data = self.data.base_info , func = self.FlushStatus},
        -- {data = self.data.my_info , func = self.FlushMyRole, init_call = false }, 
        -- {data = self.data.operating_info , func = self.FlushSpShow, keys = {"map_load_mark"},init_call = false},
        {data = self.data.operating_info , func = self.FlushMyRole, keys = {"map_load_mark"},init_call = false},
        -- {data = self.data.operating_info , func = self.FlushOtherRole, keys = {"map_load_mark","force_flush"},init_call = false},
        -- {data = self.data.net_role_info , func = self.FlushOtherRole,init_call = false},
        {data = self.data.net_alter_info , func = self.FlushAlterShow},
        {data = self.data.net_alter_info , func = self.FlushVisible,init_call = false},
        {data = self.data.net_map_info , func = self.FlushVisible,init_call = false},
        {data = self.data.operating_info , func = self.FlushVisible,keys = {"force_flush_visible"},init_call = false},
        -- {data = self.data.operating_info , func = self.ResetOperShow,keys = {"force_flush_visible"},init_call = false},
        -- {data = self.data.operating_info , func = self.ResetOperShow,keys = {"force_close_operate"},init_call = false},
        {data = self.data.operating_info , func = self.FlushExtraList,keys = {"flush_extra"},init_call = false},

        {data = LBShopData.Instance.buff_info , func = self.FlushBuff,init_call = false},
        {data = GodPosData.Instance.info_data , func = self.FlushAlterShow},
        -- 强制款
        {data = self.data.my_info , func = self.ForceFlushMyRole,init_call = false},
        {data = self.data.operating_info , func = self.ForceFlushMyRole, keys = {"map_load_mark"},init_call = false},
        {data = self.data.operating_info , func = self.ForceFlushOtherRole, keys = {"draging"},init_call = false},
        {data = self.data.net_role_info , func = self.ForceFlushOtherRole,init_call = false},
        {data = self.data.net_alter_info , func = self.ForceFlushOtherRole,init_call = false},
        {data = self.data.operating_info , func = self.ForceFlushOtherRole,init_call = false},
        {data = self.data.operating_info , func = self.ForceFlushSpShow,init_call = false},

        {data = self.data.slow_start , func = self.FlushSlowStart,init_call = false},
    }

    self.sensitivity = 0.1

    -- self.cache_roles = {}

    self.role_obj = UIChDrawer.CopyMainRoleData()
	local show_t = {
		ui_obj = self.role_obj,
		view = self
	}
    self.RoleModel:SetShowData(show_t)

    self.role_cache = {}
    self.sp_cache = {}

    self.force_role_cache = {}
    self.force_altar_cache = {}
    self.force_sp_cache = {}
end 

function LandsBetweenMainPanel:Awake()
    UIWFlushPanel.Awake(self)

    local my_app = RoleData.Instance:GetApearance()
    UH.SetAvatar(self.RoleHead, my_app.avatar_type ,my_app.avatar_id,my_app.fly_flag)
end

-- function LandsBetweenMainPanel:ForceInit()
--     self:ForceFlushMyRole()
--     self:ForceFlushOtherRole()
--     self:ForceFlushSpShow()
-- end

function LandsBetweenMainPanel:OnDestroy()
    UIWFlushPanel.OnDestroy(self)

    if self.drag_end_timer ~= nil then
        TimeHelper:CancelTimer(self.drag_end_timer)
        self.drag_end_timer = nil
    end

    if self.role_obj ~= nil then
        self.role_obj:DeleteMe()
    end
    self.role_obj = nil
    self.map_mono = nil

    -- 被隐藏的extra不会主动执行删除操作
    if self.BtnGroupShow.activeSelf then 
        for k,v in pairs(self.ExtralList.item_list) do 
            v:OnRelease()
        end 
    else
        for k,v in pairs(self.ExtralOutList.item_list) do 
            v:OnRelease()
        end 
    end 
    
    for k,v in pairs(self.BuffList.item_list) do 
        v:OnRelease()
    end 
end

function LandsBetweenMainPanel:InitComplete()
    -- LogError("?InitComplete")
    self.map_mono = self.View.obj:GetLuaComponent("LandsBetweenMapModel")
    self.map_camera = self.map_mono.camera.gameObject

    self.data:SetMapParentWorldPos({
        map_parent = self.map_mono.OperParent.transform.position,
        root = self.View.obj.transform.position
    })
    
    self:FlushExtraList()
    TimeHelper:AddDelayFrameTimer(function()
        if self.map_mono ~= nil then 
            self.map_mono:InitVisible()
        end 
    end,2)
end

-- 额外系统
function LandsBetweenMainPanel:FlushExtraList()
    local list = self.data:GetExtralList()
    local out_list = self.data:GetExtralOutList()
    self.ExtralList:SetData(list)
    self.ExtralOutList:SetData(out_list)    
end

-- 点击聊天栏
function LandsBetweenMainPanel:OnClickList()
    ViewMgr:OpenView(ChatView)
end

-- 点击我的头像
function LandsBetweenMainPanel:OnClickMyHead()
    self.data:SetMapClick(self.data:GetRolePos())
end

function LandsBetweenMainPanel:OnClickChatArrow()
    self.HeadObj:SetActive(not self.HeadObj.activeSelf)
    self.GodPosObj:SetActive(not self.GodPosObj.activeSelf)
end

function LandsBetweenMainPanel:OnDrag()
    -- if self.is_Occupying then 
        -- PublicPopupCtrl.Instance:Center(Language.LandsBetween.Desc.ClickInOccupying)
    --     return 
    -- end 
    self:MapDrag() 
end

function LandsBetweenMainPanel:OnClickShow()
    self.BtnGroupShow:SetActive(false)
end

function LandsBetweenMainPanel:OnClickHide()
    self.BtnGroupShow:SetActive(true)
end

function LandsBetweenMainPanel:OnClickMap()
    ViewMgr:OpenView(LandsBetweenMapView)
end

-- 拖拽主逻辑
function LandsBetweenMainPanel:MapDrag()
    if not self.map_mono then return end 
    LandsBetweenData.Instance.draging = true

    local mouse_pos_x = UnityEngine.Input.mousePosition.x
	local mouse_pos_y = UnityEngine.Input.mousePosition.y

    self.camera_x = self.map_camera.transform.localPosition.x
    self.camera_y = self.map_camera.transform.localPosition.z

    if self.mouse_pos_x and self.mouse_pos_x ~= mouse_pos_x then 
        local delta = self.mouse_pos_x - mouse_pos_x 
        self.delta_x = delta + self.delta_x
        self.map_x = delta * self.sensitivity 
    end 

    if self.mouse_pos_y and self.mouse_pos_y ~= mouse_pos_y then 
        local delta = self.mouse_pos_y - mouse_pos_y 
        self.delta_y = delta + self.delta_y
        self.map_y = delta  * self.sensitivity 
    end 

    local pos_y = self.map_camera.transform.localPosition.y

    -- local pos_x = self.camera_x - self.map_x
    -- local pos_z = self.camera_y - self.map_y
    -- LogError("?",self.camera_x,self.camera_y,self.map_x,self.map_y)
    local pos_x,pos_z = self.map_mono:CheckLimit(self.camera_x,self.camera_y,self.map_x,self.map_y)

    UH.LocalPosG(self.map_camera, Vector3.New(pos_x,pos_y,pos_z))

    self.mouse_pos_x = mouse_pos_x
    self.mouse_pos_y = mouse_pos_y

    self.camera_x = self.map_camera.transform.localPosition.x
    self.camera_y = self.map_camera.transform.localPosition.z

    self.map_mono.Vcamera.Follow = nil
end

-- 点击屏幕
function LandsBetweenMainPanel:OnClickOperate()
    if not self.map_mono then return end 

    local input_t = {
		x = UnityEngine.Input.mousePosition.x / UnityEngine.Screen.width * self.OperateSize.rect.width,
		y = UnityEngine.Input.mousePosition.y / UnityEngine.Screen.height * self.OperateSize.rect.height,
	}
    
    local view_port = Vector3.New(
        input_t.x /self.OperateSize.rect.width,
        input_t.y /self.OperateSize.rect.height,
        1
    )

    self.map_mono:TryTouchTouch(view_port)
end 

-- 刷新视野 
function LandsBetweenMainPanel:FlushVisible()
    if not self.map_mono then return end 

    self.map_mono:FlushVisible()
end

-- 清除操作缓存——开始拖动
function LandsBetweenMainPanel:ClearBeginOperate()
    self:ResetOperShow()
end

-- 强制刷新行为重置操作轮
function LandsBetweenMainPanel:ResetOperShow()
    self.data:SetOperatingObj()
    if self.OperateFollow.Target ~= nil then 
        local oper = self.OperateFollow.Target.gameObject:GetLuaComponent(LandsBetweenMapModelPic:GetTypeName())
        oper:SelectItem(false)
    end 
    self.OperateFollow.Target = nil
    self.OperateFollow.gameObject:SetActive(false)
    self.data:ClearMapClick()
end 

-- 清除操作缓存——结束拖动
function LandsBetweenMainPanel:ClearEndOperate()
    self.data:SetOperatingObj()
    self.OperateFollow.Target = nil
    self.OperateFollow.gameObject:SetActive(false)
    self.data:ClearMapClick()
    -- self.RoleModelCan.alpha = 0
    
    for k,v in pairs(self.role_cache) do 
        local item_mono = v:GetLuaComponent(self.RolePref:GetTypeName())
        item_mono.TargetFollow.Target = nil
        v:SetActive(false)
        -- item_mono.Can.alpha = 0
    end 

    for k,v in pairs(self.sp_cache) do 
        local item_mono = v:GetLuaComponent(self.ItemActSp:GetTypeName())
        item_mono.TargetFollow.Target = nil
        v:SetActive(false)
        -- item_mono.Can.alpha = 0
    end 
end

function LandsBetweenMainPanel:OnDragBegin()
    if not self.map_mono then return end 

    self.camera_x = self.map_camera.transform.localPosition.x
    self.camera_y = self.map_camera.transform.localPosition.z

    self.delta_x = 0
    self.delta_y = 0

    self:ClearBeginOperate()

    self.map_mono:SetDraging(true)
end

function LandsBetweenMainPanel:OnDragEnd()
    LandsBetweenData.Instance.draging = false

    if not self.map_mono then return end 

    self.mouse_pos_x = nil
    self.mouse_pos_y = nil
    self.map_x = 0
    self.map_y = 0
    
    self:ClearEndOperate()

    self:FlushVisible()
    -- self.drag_end_timer = TimeHelper:AddDelayTimer(function()
    self.map_mono:SetDraging(false)
    -- end, 1)
    self.data.operating_info.draging = 0
end

-- 刷新操作地块
function LandsBetweenMainPanel:FlushOperatingObj()
    if self.data.operating_info.obj == nil then 
        self.OperateFollow.Target = nil
        self.OperateFollow.gameObject:SetActive(false)
        self.data:ClearMapClick()
        return 
    end 

    -- if self.is_Occupying then 
    --     PublicPopupCtrl.Instance:Center(Language.LandsBetween.Desc.ClickInOccupying)
    --     return 
    -- end 
    -- LogError("?Zdas FlushOperatingObj")

    local map_pic = self.data:GetMapPicClick(true)
    local sp_grid_param = self.data:CheckSpPicAroundByLinkMark(map_pic.static.link_mark)
    if sp_grid_param ~= nil then 
        if sp_grid_param.enable_func ~= nil and sp_grid_param.enable_func() then 
            if sp_grid_param.faction ~= -1 and self.data.my_info.faction ~= sp_grid_param.faction then 
                PublicPopupCtrl.Instance:Center(Language.LandsBetween.Desc.ClickOtherGrid)
                return 
            end 

            if sp_grid_param.center_link_mark ~= map_pic.static.link_mark then 
                local pos_str = LandsBetweenData.FixLink(sp_grid_param.center_link_mark)
                LandsBetweenData.Instance:SetMapClick({x = tonumber(pos_str[1]),y = tonumber(pos_str[2])})
    
                -- TimeHelper:AddDelayTimer(function()
                    LandsBetweenData.Instance:SetMapJumpClick({x = tonumber(pos_str[1]),y = tonumber(pos_str[2])})
                -- end,1)
                return 
            end 
            if sp_grid_param.extral_info ~= nil then 
                self.OperatePos:SetData({is_nomal = sp_grid_param.big_size,type = 3,extra = sp_grid_param.extral_info,show_pos = sp_grid_param.show_pos,pos_str = self.data:GetMapPicPosStr()})
                self.extra_func = sp_grid_param.extral_info.func
                self.share_func = sp_grid_param.extral_info.share_func
                self.oper_tips_func = sp_grid_param.extral_info.tips_func
            else 
                self.OperatePos:SetData({type = map_pic.linking_city and 1 or 0})
            end 
            
            self.OperateFollow.Target = self.data.operating_info.obj.transform
            self.OperateFollow.gameObject:SetActive(true)
            return 
        end 
    end 

    if self.data:IsMainCity(map_pic.static) then 
        if not self.data:GetMainCityIsMyCity(map_pic.static) then 
            PublicPopupCtrl.Instance:Center(Language.LandsBetween.Desc.ClickOtherCity)
            return 
        end 
        
        self.OperateFollow.Target = nil
        self.OperateFollow.gameObject:SetActive(false)
        self.data:ClearMapClick()
        ViewMgr:OpenView(LandsBetweenCityView)
        return
    end 

    local is_our_grid = self.data:IsOurGridByNet(map_pic.net_info) 
    local is_altar = self.data:IsAltar(map_pic.static.map_show) 
    local is_link = map_pic.linking_city
    local is_Adjoin = map_pic.adjoining_grid

    local show_operate = (not is_our_grid and is_link and self.data:GetCurMapType() == 1) or (not is_our_grid and is_Adjoin and self.data:GetCurMapType() == 0) or (is_our_grid and is_altar)

    if is_our_grid and is_altar then 
        self.OperatePos:SetData({type = 2 ,is_mine = self.data:IsMyNetAlter(map_pic.static.link_mark),show_pos = true,pos_str = self.data:GetMapPicPosStr()})
    elseif not is_our_grid and is_link and self.data:GetCurMapType() == 1 then 
        self.OperatePos:SetData({type = 1 ,show_pos = true,pos_str = self.data:GetMapPicPosStr()})
    elseif not is_our_grid and is_Adjoin and self.data:GetCurMapType() == 0 then 
        self.OperatePos:SetData({type = 1 ,show_pos = true,pos_str = self.data:GetMapPicPosStr()})
    elseif is_our_grid and not is_altar then 
        PublicPopupCtrl.Instance:Center(Language.LandsBetween.Desc.PlaceOccupy)

        self:ResetOperShow()
    end 

    if not is_Adjoin and not is_our_grid then 
        PublicPopupCtrl.Instance:Center(Language.LandsBetween.Desc.AdjoinError)
        self:ResetOperShow()
        return 
    end 

    if not is_link and not is_our_grid and self.data:GetCurMapType() == 1 then 
        PublicPopupCtrl.Instance:Center(Language.LandsBetween.Desc.LinkError)
        self:ResetOperShow()
        return 
    end 

    self.OperateFollow.gameObject:SetActive(show_operate)
    if show_operate then 
        self.OperateFollow.Target = self.data.operating_info.obj.transform
    else  
        self.data:ClearMapClick()
    end 
end

function LandsBetweenMainPanel:FlushStatus()
    local status = self.data:GetCurMapType() == 0 and 1 or 2
    UH.SetText(self.CurStatus,string.format(Language.LandsBetween.Desc.MainStatusStr,DataHelper.GetDaXie(status) ))

    local cfg = Config.divine_domain_system_auto.other[1]
    local time_str= status == 1 and cfg.stage_one_time or cfg.stage_two_time
    local cur_month= os.date("%m", os.time())
    UH.SetText(self.OpenShowTime,string.format(time_str,cur_month,cur_month))
end

-- 动态视野方案与格子跟踪方案冲突！ 需要优化
function LandsBetweenMainPanel:FlushMyRole()
    --LogError("flush?!FlushMyRole")
    if not self.data:IsFlushPicComplete() then return end 

    if not self.map_mono then return end 
    -- LogError("role_flush_yime",self.RoleModel.gameObject.transform.localPosition)
    -- if self.RoleModelFollow.Target ~= nil then 
    --     return 
    -- end 
    local role_pos = self.data:GetRolePos()
    local obj = self.map_mono:CatchFollowTarget(role_pos)

    self.HeadObj:SetActive(obj == nil)
    -- local link_mark = self.data.CfgLink({[1] = role_pos.x ,[2] = role_pos.y})
    -- local cfg = self.data:GetMapGridStaticInfo(link_mark)
    -- self.RoleModelCan.alpha = self.data:IsMainCity({map_show = cfg}) and 0 or 1
    -- if obj ~= nil then 
    --     self.RoleModelFollow.Target = obj.transform
    -- end 

    -- LogError("role_flush_yime",self.RoleModel.gameObject.transform.localPosition)
    -- 不在视野里的话就移动到屏幕外

    -- if obj == nil then 
    --     self.RoleModelFollow.Target = nil
    --     UH.LocalPosG(self.RoleModel.gameObject, Vector3.New(1000,1000))
    -- end 


end

-- 刷新其他人的地图头像信息
function LandsBetweenMainPanel:FlushOtherRole()
    if not self.data:IsFlushPicComplete() then return end 
    if not self.map_mono then return end 

    for k,v in pairs(self.role_cache) do 
        local item_mono = v:GetLuaComponent(self.RolePref:GetTypeName())
        item_mono.TargetFollow.Target = nil
        v:SetActive(false)
    end 

    local index = 1
    local is_un_cahce = #self.role_cache == 0
    -- 生成角色头·普通款
    -- 这种是不带精华的
    -- 本功能暂时屏蔽·后改成模型
    -- for k,v in pairs(self.data.net_role_info.list) do 
    --     local role_grid = self.data:GetOtherRolePos(v)
    --     local follow_obj = self.map_mono:CatchFollowTarget(role_grid)
    --     if follow_obj ~= nil then 
    --         local fix_data = self.data:FixMapRoleHead(v)
    --         local link_mark = self.data.CfgLink({[1] = role_grid.x,[2] = role_grid.y})
    --         if is_un_cahce or self.role_cache[index] == nil then
    --             local item_obj = self.RolePref.gameObject:Clone(self.RoleParent)
    --             item_obj.name = k.."|"..link_mark
        
    --             item_obj:SetActive(true)
    --             local item_mono = item_obj:GetLuaComponent(self.RolePref:GetTypeName())
    --             item_mono:SetData(fix_data)
    --             item_mono.TargetFollow.Target = follow_obj.transform
    --             table.insert(self.role_cache,item_obj)
    --         else 
    --             self.role_cache[index].name = k.."|"..link_mark

    --             local item_mono = self.role_cache[index]:GetLuaComponent(self.RolePref:GetTypeName())
    --             item_mono:SetData(fix_data)
    --             item_mono.TargetFollow.Target = follow_obj.transform
    --         end
    --         index = index + 1 
    --     end 
    -- end

    -- 生成神坛·祈祷款
    -- 这种是带精华的
    for k,v in pairs(self.data.net_alter_info.list) do 
        if v.pray_end_timestamp ~= nil and v.occupy_uid ~= nil and v.occupy_plat_type ~= nil then 
            local fix_info = {x = v.row,y = v.column}
            local follow_obj = self.map_mono:CatchFollowTarget(fix_info)
            if follow_obj ~= nil then 
                local fix_data = self.data:FixMapRoleHead(v)
                local link_mark = self.data.CfgLink({[1] = v.row,[2] = v.column})

                if is_un_cahce or self.role_cache[index] == nil then
                    local item_obj = self.RolePref.gameObject:Clone(self.RoleParent)
                    item_obj.name = k.."|"..link_mark
            
                    item_obj:SetActive(true)
                    local item_mono = item_obj:GetLuaComponent(self.RolePref:GetTypeName())
                    item_mono:SetData(fix_data)
                    item_mono.TargetFollow.Target = follow_obj.transform
                    table.insert(self.role_cache,item_obj)
                else
                    self.role_cache[index].name = k.."|"..link_mark
    
                    local item_mono = self.role_cache[index]:GetLuaComponent(self.RolePref:GetTypeName())
                    item_mono:SetData(fix_data)
                    item_mono.TargetFollow.Target = follow_obj.transform
                end 
                index = index + 1  
            else

                -- item_mono.TargetFollow.Target = nil
            end
        end 
    end 

    -- for k,v in pairs(self.role_cache) do 
    if #self.role_cache > 0 then 
        for i= 1,index - 1 do 
            local item_mono = self.role_cache[i]:GetLuaComponent(self.RolePref:GetTypeName())
            -- item_mono.TargetFollow.Target = nil
            self.role_cache[i]:SetActive(true)
        end 
    end 
end

-- 生成活动特殊信息框
function LandsBetweenMainPanel:FlushSpShow()
    if not self.data:IsFlushPicComplete() then return end 
    if not self.map_mono then return end 

    for k,v in pairs(self.sp_cache) do 
        local item_mono = v:GetLuaComponent(self.ItemActSp:GetTypeName())
        -- item_mono.Can.alpha = 0
        item_mono.TargetFollow.Target = nil
        v:SetActive(false)
    end 

    local index = 1
    local is_un_cahce = #self.sp_cache == 0
    for k,v in pairs(self.data.sp_pic_mark) do
        local fix_link = self.data.FixLink(k)
        local follow_obj = self.map_mono:CatchFollowTarget({x = fix_link[1],y = fix_link[2]})

        if follow_obj ~= nil then 
            local link_mark = k

            if is_un_cahce or self.sp_cache[index] == nil then
                local item_obj = self.ItemActSp.gameObject:Clone(self.SpParent)
                item_obj.name = v.name.."|"..link_mark
            
                item_obj:SetActive(true)
                local item_mono = item_obj:GetLuaComponent(self.ItemActSp:GetTypeName())
                item_mono:SetData(v)
                item_mono.TargetFollow.Target = follow_obj.transform
                table.insert(self.sp_cache,item_obj)
            else
                self.sp_cache[index].name = k.."|"..link_mark
    
                local item_mono = self.sp_cache[index]:GetLuaComponent(self.ItemActSp:GetTypeName())
                item_mono:SetData(v)
                item_mono.TargetFollow.Target = follow_obj.transform
            end 
            index = index + 1  
        end
    end

    if #self.sp_cache > 0 then 
        for i = 1,index - 1 do 
            local item_mono = self.sp_cache[i]:GetLuaComponent(self.ItemActSp:GetTypeName())

            local sp_grid_param = LandsBetweenData.Instance:GetResOfSpPicByLinkMark(item_mono:GetData().link_mark)

            if sp_grid_param.enable_func ~= nil and sp_grid_param.enable_func() then 
                
                self.sp_cache[i]:SetActive(true)
            else 
                self.sp_cache[i]:SetActive(false)
            end 
            -- LogError("?????",sp_grid_param)
            -- item_mono.Can.alpha = (sp_grid_param.enable_func ~= nil and sp_grid_param.enable_func()) and 1 or 0 
        end 
    end 

end

-- 强制生成我的模型
-- 强制生成不会检查视野！
function LandsBetweenMainPanel:ForceFlushMyRole()
    if not self.map_mono then return end 
    
    local role_pos = self.data:GetRolePos()
    local link_mark = self.data.CfgLink({[1] = role_pos.x ,[2] = role_pos.y})
    local obj = self.map_mono:LockTargetPos(link_mark,"my_role")

    local cfg = self.data:GetMapGridStaticInfo(link_mark)
    self.RoleModelCan.alpha = self.data:IsMainCity({map_show = cfg}) and 0 or 1

    if self.RoleModelFollow.Target == nil then 
        self.RoleModelFollow.Target = obj ~= nil and obj.transform or nil
    end 
end 

-- 强制生成别人的模型和神坛祷告（包括我自己
-- 强制生成不会检查视野！(现在会了 20221109)
function LandsBetweenMainPanel:ForceFlushOtherRole()
    -- if not self.data:IsFlushPicComplete() then return end 
    if not self.map_mono then return end 

    -- 是否为有效的神坛信息
    local function CheckIsPray(vo)
        local flag_in_visible = self.data:IsGridCanBeVisible({x = vo.row , y = vo.column})
        return flag_in_visible and vo.pray_end_timestamp ~= nil and vo.pray_end_timestamp > 0 and vo.occupy_uid ~= nil and vo.occupy_plat_type ~= nil and vo.occupy_uid >0 and vo.prayer_uid > 0
    end

    local alter_flush_list = {}
    for k,v in pairs(self.data.net_alter_info.list) do 
        if CheckIsPray(v) then
            local link_mark = self.data.CfgLink({[1] =v.row ,[2] = v.column})
            if self.force_altar_cache[link_mark] == nil then 
                local obj = self.map_mono:LockTargetPos(link_mark,"alter|"..link_mark)

                local item_obj = self.RolePref.gameObject:Clone(self.RoleParent)
                item_obj.name = "alter|"..link_mark
                item_obj:SetActive(true)

                local item_mono = item_obj:GetLuaComponent(self.RolePref:GetTypeName())
                item_mono.TargetFollow.Target = obj ~= nil and obj.transform or nil
                item_mono:SetData(self.data:FixMapRoleHead(v))

                self.force_altar_cache[link_mark] = item_obj
            else 
                local obj = self.map_mono:LockTargetPos(link_mark,"alter|"..link_mark)
                
                local item_mono = self.force_altar_cache[link_mark]:GetLuaComponent(self.RolePref:GetTypeName())
                item_mono.TargetFollow.Target = obj ~= nil and obj.transform or nil
                item_mono:SetData(self.data:FixMapRoleHead(v))

                self.force_altar_cache[link_mark]:SetActive(true)
            end
            alter_flush_list[link_mark] = 1
        end
    end 

    -- 对 多出来的锚定神坛（被放弃或者是不再持有netinfo的神坛） 进行解绑
    -- 解绑动作不会丢失强制锚定，可以走正常更新途径重新接绑
    for link_mark,obj in pairs(self.force_altar_cache) do 
        if alter_flush_list[link_mark] == nil then 
            local item_mono = self.force_altar_cache[link_mark]:GetLuaComponent(self.RolePref:GetTypeName())
            item_mono.TargetFollow.Target = nil
            self.force_altar_cache[link_mark]:SetActive(false)
        end 
    end 

        
    -- 是否为有效的玩家信息
    local function CheckIsEffRole(vo)
        local role_id = RoleData.Instance:GetRoleId()
        local flag_in_visible = self.data:IsGridCanBeVisible({x = vo.row , y = vo.column})
        return flag_in_visible and vo.avatar_type ~= nil and vo.avatar_type > 0 and vo.uid ~= nil and vo.uid ~= role_id
    end

    local role_flush_list = {}
    for k,v in pairs(self.data.net_role_info.list) do 
        if CheckIsEffRole(v) then
            local link_mark = self.data.CfgLink({[1] =v.row ,[2] = v.column})
            if self.force_role_cache[link_mark] == nil then 
                local obj = self.map_mono:LockTargetPos(link_mark,"role|"..link_mark)

                local item_obj = self.OtherRolePref.gameObject:Clone(self.RoleParent)
                item_obj.name = "role|"..link_mark
                item_obj:SetActive(true)

                local item_mono = item_obj:GetLuaComponent(self.OtherRolePref:GetTypeName())
                item_mono.TargetFollow.Target = obj ~= nil and obj.transform or nil
                item_mono:SetData(self.data:FixMapRoleModel(v))

                self.force_role_cache[link_mark] = item_obj
            else 
                local item_mono = self.force_role_cache[link_mark]:GetLuaComponent(self.OtherRolePref:GetTypeName())
                item_mono:SetData(self.data:FixMapRoleModel(v))
            end
            role_flush_list[link_mark] = 1
        end
    end 

    -- 对 多出来的锚定神坛（被放弃或者是不再持有netinfo的神坛） 进行解绑
    -- 解绑动作不会丢失强制锚定，可以走正常更新途径重新接绑
    for link_mark,obj in pairs(self.force_role_cache) do 
        if role_flush_list[link_mark] == nil then 
            local item_mono = self.force_role_cache[link_mark]:GetLuaComponent(self.OtherRolePref:GetTypeName())
            item_mono.TargetFollow.Target = nil
            self.force_role_cache[link_mark]:SetActive(false)
        end 
    end 

end 

-- 强制生成活动特殊格子
-- 强制生成不会检查视野！
function LandsBetweenMainPanel:ForceFlushSpShow()
    if not self.data:IsFlushPicComplete() then return end 
    if not self.map_mono then return end 

    -- 
    local flush_list = {}
    for k,v in pairs(self.data.sp_pic_mark) do
        local fix_link = self.data.FixLink(k)
        if v.enable_func == nil or (v.enable_func ~= nil and v.enable_func()) then 
            if self.force_sp_cache[k] == nil then 
                local obj = self.map_mono:LockTargetPos(k,"sp_"..k)
                local item_obj = self.ItemActSp.gameObject:Clone(self.SpParent)
                item_obj.name = v.name.."|"..k
                item_obj:SetActive(true)

                local item_mono = item_obj:GetLuaComponent(self.ItemActSp:GetTypeName())
                item_mono:SetData(v)
                item_mono.TargetFollow.Target = obj ~= nil and obj.transform or nil

                self.force_sp_cache[k] = item_obj
            else
                local item_mono = self.force_sp_cache[k]:GetLuaComponent(self.ItemActSp:GetTypeName())
                item_mono:SetData(v) 
            end 
            flush_list[k] = 1
        end 
    end    

    -- 对 多出来的锚定特殊格（已删除） 进行解绑
    -- 解绑动作不会丢失强制锚定，可以走正常更新途径重新接绑
    for link_mark,obj in pairs(self.force_sp_cache) do 
        if flush_list[link_mark] == nil then 
            local item_mono = self.force_sp_cache[link_mark]:GetLuaComponent(self.ItemActSp:GetTypeName())
            item_mono.TargetFollow.Target = nil
            item_mono.SpCalls:HideSpSub()
            self.force_sp_cache[link_mark]:SetActive(false)
        end 
    end 
end 

function LandsBetweenMainPanel:FlushAlterShow()
    local str = self.data:GetMyNetAlterStr()
    UH.SetText(self.AlterNum,str)    
end 

function LandsBetweenMainPanel:FlushBuff()
    local list = LBShopData.Instance:GetBuffShowList()
    self.BuffList:SetData(list)
    UH.SetText(self.BuffText,string.format(Language.LandsBetween.Desc.BuffShowStr,#list))
end 

function LandsBetweenMainPanel:FlushSlowStart()
    -- self.Black:SetActive(LandsBetweenData.Instance.slow_start.flag) 
end

function LandsBetweenMainPanel:OnClickONE()
    self.data:TestOne()
end

function LandsBetweenMainPanel:OnClickTWO()
    self.data:TestTwo()    
end

function LandsBetweenMainPanel:OnClickTHREE()
    self.data:TestThree()    
end

function LandsBetweenMainPanel:OnClickFOUR()
    self.data:TestFour()    
end

function LandsBetweenMainPanel:OnClickFive()
    self.data:TestFive()    
end

function LandsBetweenMainPanel:OnClickOperateTips()
    -- PublicPopupCtrl.Instance:Center("点击说明")
    if self.oper_tips_func ~= nil then 
        self.oper_tips_func()
        return         
    end 

    local map_pic = self.data:GetMapPicClick()

    local cur_grid = self.data:IsAltar(map_pic.static.map_show) and 1 or 0

    ViewMgr:OpenView(LandsBetweenExplainView,{type = self.data:GetCurMapType(),grid = cur_grid})
end

function LandsBetweenMainPanel:OnOccEffComplete()
    LandsBetweenCtrl.Instance:OperateOccupy({param_1 =self.cache_pos.x,param_2 =self.cache_pos.y,param_3 = 0})
    self.is_Occupying = false
    self.EffPos.Target = nil
end

function LandsBetweenMainPanel:OnClickOperateOccupy()
    if self.is_Occupying then 
        PublicPopupCtrl.Instance:Center(Language.LandsBetween.Desc.ClickInOccupying)
        return 
    end 

    -- if self.data.my_info.action_point < Config.divine_domain_system_auto.action_force[1].occupy_ap_consume then 
    --     PublicPopupCtrl.Instance:Center(Language.LandsBetween.Desc.MovingIsLack)
    --     ViewMgr:OpenView(LandsBetweenMovingView)         
    --     return
    -- end 

    local map_pic = self.data:GetMapPicClick()
    local is_cd,cd_stamp = self.data:IsInOccupyCD(map_pic.net_info) 

    if is_cd then 
        local last = cd_stamp - TimeManager:GetServerTime()
        local t0 = TimeManager.FormatUnixTime2Date(last)
        local t_str = string.format(Language.UiTimeMeter.Specia2, t0.min ,t0.s)
        if t0.min == 0 then 
            local t_str = string.format(Language.UiTimeMeter.Special5,t0.s)
        end 
        PublicPopupCtrl.Instance:Center(string.format(Language.LandsBetween.Desc.GridCDtiming,t_str))     
        return 
    end 

    local fix_link = self.data.FixLink(map_pic.static.link_mark)
    if self.data:IsAltar(map_pic.static.map_show) then 
        LandsBetweenCtrl.Instance:ReqDefenderInfo({param_1 = fix_link[1],param_2 = fix_link[2]})
    else
        self.map_mono:TryEffPos()
        self.EffPos.Target = self.map_mono.EffPos
        -- UH.LocalPosG(self.EffPos,self.OperateFollow.transform.localPosition)
        self.cache_pos = {x = fix_link[1],y = fix_link[2]}
        self.EffectTool:Play("3165307")
        self.is_Occupying = true
    end

    self:ResetOperShow()
    -- PublicPopupCtrl.Instance:Center("点击占领")
end

function LandsBetweenMainPanel:OnClickOperateShare()
    -- PublicPopupCtrl.Instance:Center("点击分享")
    if self.share_func ~= nil then 
        self.share_func()
        return
    end 

    local str = self.data:GetShareStr()
    ChatCtrl.Instance:SendChannelChatText(str,nil, ChatConfig.ChannelType.landsbetween)
end

function LandsBetweenMainPanel:OnClickOperateIrony()
    -- PublicPopupCtrl.Instance:Center("点击祈愿")
    
    local role_id = RoleData.Instance:GetRoleId()
    local map_pic = self.data:GetMapPicClick()

    -- LogError("haops?",role_id)
    -- LogError("haops2?",map_pic.net_info)
    if map_pic.net_info.pray_end_timestamp ~= nil and map_pic.net_info.pray_end_timestamp ~= 0 
        and (role_id ~= map_pic.net_info.prayer_uid and map_pic.net_info.prayer_uid ~= 0) then 
        PublicPopupCtrl.Instance:Center(Language.LandsBetween.Desc.SingleAlterPraying)
        self.data:ClearMapClick()
        return 
    end 
    local title = self.data:GetAlterName(map_pic.static.map_show)
    ViewMgr:OpenView(LandsBetweenSingleAltarView,{holded = true,city_name = title})
end


function LandsBetweenMainPanel:OnClickOperateCancel()
    -- PublicPopupCtrl.Instance:Center("取消")

    local info = {
		content =Language.LandsBetween.Desc.CancelAltar, 
		cancel = {
			name = nil,
			func = nil
		},
		confirm = {
			name = nil,
			func = function()
                local map_pic = self.data:GetMapPicClick()
                local fix_link = self.data.FixLink(map_pic.static.link_mark)
                LandsBetweenCtrl:OperateCancelAltar({param_1 = fix_link[1],param_2 = fix_link[2]})
                PublicPopupCtrl.Instance:CloseDialogTips()
                self:ResetOperShow()
            end,
		}
	}
    self.data:ClearMapClick()
	PublicPopupCtrl.Instance:DialogTips(info)
end

function LandsBetweenMainPanel:OnClickOperateExtral()
    if self.extra_func ~= nil then 
        self.extra_func()
    end 
end

function LandsBetweenMainPanel:OnClickBuffShow()
    self.BuffContent:SetActive(not self.BuffContent.activeSelf)
end

function LandsBetweenMainPanel:OnClickIrony()
    ViewMgr:OpenView(LandsBetweenAltarInfoView,{click_type = 2})
end 

function LandsBetweenMainPanel:OnClickReport()
    ViewMgr:OpenView(LandsBetweenReportView)
end 

function LandsBetweenMainPanel:OnClickAlter()
    ViewMgr:OpenView(LandsBetweenAltarInfoView,{click_type = 1})
end 

LandsBetweenExtraModCell = LandsBetweenExtraModCell or DeclareMono("LandsBetweenExtraModCell", UIWidgetBaseItem)
function LandsBetweenExtraModCell:SetData(data)
    UIWidgetBaseItem.SetData(self, data)

    UH.SetText(self.name,data.name)
    UH.SpriteName(self.icon,data.sprite)

    local param = LandsBetweenData.Instance:GetExtraRemindMark(data.open)

    if param == nil then 
        return 
    end 
    self.remin_data = param.target
    self.remind_func = param.func
    --注册红点
    if self.remin_data then 
        if not self.is_care then
            self.is_care = true
            if self.red_pos ~= nil then 
                self.red_point = UiRedPoint.New(self.red_pos, Vector3.New(0,0,0))
                self:careData(self.remin_data, BindTool.Bind(self.ActRedPoint, self))
            end 
        else 
            self:ActRedPoint()
        end
    end 
end 

function LandsBetweenExtraModCell:ActRedPoint()

    if self.red_pos == nil then return end 
    if self.red_point ~= nil and ViewMgr:IsOpen(LandsBetweenMainView) then 
    	self.red_point:SetNum(self.remind_func ~= nil and self.remind_func() or 0)
    end 
end

function LandsBetweenExtraModCell:OnRelease()
    UIWidgetBaseItem.OnRelease(self)
	if not self.is_care then return end
    self:uncareData()
    self.red_point = nil
end

function LandsBetweenExtraModCell:OnClickExtra()
    if self.data.open == "" then 
        PublicPopupCtrl.Instance:Center("嘉泽，你该补配置了，这是神域功能表")
    end 
    ViewMgr:OpenViewByKey(self.data.open)
end

-- buff
LandsBetweenBattleBuffCell = LandsBetweenBattleBuffCell or DeclareMono("LandsBetweenBattleBuffCell", UIWidgetBaseItem)
function LandsBetweenBattleBuffCell:SetData(data)
    UIWidgetBaseItem.SetData(self, data)    
    self.Calls:SetData(data.item_show)
    UH.SetText(self.Name,data.item_show:QuaName())

    self.Timer.gameObject:SetActive(not data.is_running)
    self.RunTimer.gameObject:SetActive(data.is_running)

    if data.is_running then 
        self.RunTimer:StampTime(data.time_stamp,TimeType.Type_Time_0)
    else 
        UH.SetText(self.Timer,data.timer_str)
    end 
end

-- 角色头信息
LandsBetweenRoleHeadShow = LandsBetweenRoleHeadShow or DeclareMono("LandsBetweenRoleHeadShow", UIWidgetBaseItem)
function LandsBetweenRoleHeadShow:LandsBetweenRoleHeadShow()
    self.Timer:SetCallBack(function()self:TimerComplete() end)
end
function LandsBetweenRoleHeadShow:SetData(data)
    UIWidgetBaseItem.SetData(self, data)    

    UH.SetAvatar(self.head, self.data.avatar_type , self.data.head_id ,0)

    local link_mark = LandsBetweenData.Instance.CfgLink({[1] = data.row ,[2] = data.column})
    local grid_cfg = LandsBetweenData.Instance:GetMapGridStaticInfo(link_mark)
    local grid_name = data.level > 0 and string.format(Language.LandsBetween.Desc.Alter,data.level) .. "\n" or ""
    local grid_irony = ""
    if data.is_ironying then 
        grid_irony = Language.LandsBetween.Desc.IronyDesc .. ":%s\n"
        local link_str = grid_name..grid_irony..data.role_name
        self.Timer:StampTime(data.time_stamp,TimeType.Type_Special_11,link_str)
    else 
        self.Timer:CloseCountDownTime()
        UH.SetText(self.PlayerInfo, grid_name..grid_irony..data.role_name)
    end
    self.Reward:SetActive(data.is_ironying)

    UH.SetIcon(self.RewardIcon, Item.GetIconId(CommonItemId.GodSpaceEssence))    
end
function LandsBetweenRoleHeadShow:TimerComplete()
    LandsBetweenData.Instance:ForceFlushOperate()
end
-- 角色模型信息
LandsBetweenOtherRoleShow = LandsBetweenOtherRoleShow or DeclareMono("LandsBetweenOtherRoleShow", UIWidgetBaseItem)
function LandsBetweenOtherRoleShow:LandsBetweenOtherRoleShow()
    if self.ui_role == nil then
        self.ui_role = UIChDrawer.New()
        self.Model:SetShowData({ui_obj = self.ui_role,view = self})
    end
end
function LandsBetweenOtherRoleShow:OnRelease()
    UIWidgetBaseItem.OnRelease(self)
    if self.ui_role ~= nil then
		self.ui_role:DeleteMe()
	end
	self.ui_role = nil
end
function LandsBetweenOtherRoleShow:SetData(data)
    UIWidgetBaseItem.SetData(self, data)    

    self.ui_role:SetData(data.role_app)
    local city_name = string.format(Language.LandsBetween.Desc.CityName[LandsBetweenData.Instance:GetCurMapType()],Language.LandsBetween.FactionName[self.data.faction])
    UH.SetText(self.Name, city_name.."\n" ..self.data.role_name)
end 

-- 地图操作轮
LandsBetweenMapOperate = LandsBetweenMapOperate or DeclareMono("LandsBetweenMapOperate", UIWidgetBaseItem)
function LandsBetweenMapOperate:SetData(data)
    UIWidgetBaseItem.SetData(self, data)    

    self.BtnTips:SetActive(true)
    self.BtnShare:SetActive(true)
 
    self.BtnOccupy:SetActive(data.type == 1)
    self.BtnMyIrony:SetActive(data.type == 2 and data.is_mine)
    self.BtnOtherIrony:SetActive(data.type == 2 and not data.is_mine)
    self.BtnCancel:SetActive(data.type == 2 and data.is_mine)

    self.BtnExtral:SetActive(data.type == 3)
    if data.type == 3 then 
        UH.SetText(self.BtnExtralName,data.extra.name)
        UH.SetText(self.BtnExtralNameBig,data.extra.name)
    end 

    self.TotalNomal:SetActive(data.is_nomal == true or data.is_nomal == nil)
    self.TotalBig:SetActive(data.is_nomal == false)

    local flag_show_pos = data.show_pos
    self.BtnPos.gameObject:SetActive(flag_show_pos)
    UH.SetText(self.BtnPos,data.pos_str)
end 

-- 顶部货币显示区域
-- 行动值
LandsBetweenCurrencyMove = LandsBetweenCurrencyMove or DeclareMono("LandsBetweenCurrencyMove", UIWFlushPanel)
function LandsBetweenCurrencyMove:LandsBetweenCurrencyMove()
    self.data = LandsBetweenData.Instance
    self.data_cares = {
        {data = self.data.my_info , func = self.FlushValue,keys = {"action_point"} },
    }    
end

function LandsBetweenCurrencyMove:FlushValue()
    local point = self.data.my_info == nil and 0 or self.data.my_info.action_point
    UH.SetText(self.Value,point)
    UH.SetIcon(self.Icon,Item.GetIconId(CommonItemId.GodSpaceMovition))
end

function LandsBetweenCurrencyMove:OnClickOperate()
    ViewMgr:OpenView(LandsBetweenMovingView) 
end

-- 战功
LandsBetweenCurrencyExploits = LandsBetweenCurrencyExploits or DeclareMono("LandsBetweenCurrencyExploits", UIWFlushPanel)
function LandsBetweenCurrencyExploits:LandsBetweenCurrencyExploits()
    self.data = LandsBetweenData.Instance
    self.data_cares = {
        {data = RoleData.Instance:GetCurrencyData() , func = self.FlushValue},
    }    
end

function LandsBetweenCurrencyExploits:FlushValue()
    local point = RoleData.Instance:GetCurrency(CurrencyType.GodSpaceExploits)
    UH.SetText(self.Value,point)
    UH.SetIcon(self.Icon,Item.GetIconId(CommonItemId.GodSpaceExploits))
end

function LandsBetweenCurrencyExploits:OnClickOperate()
    ViewMgr:OpenView(GetWayView, {item = Item.Create({item_id = CommonItemId.GodSpaceExploits})})
end

-- 精华
LandsBetweenCurrencyEssence = LandsBetweenCurrencyEssence or DeclareMono("LandsBetweenCurrencyEssence", UIWFlushPanel)
function LandsBetweenCurrencyEssence:LandsBetweenCurrencyEssence()
    self.data = LandsBetweenData.Instance
    self.data_cares = {{data = RoleData.Instance:GetCurrencyData() , func = self.FlushValue},}    
end

function LandsBetweenCurrencyEssence:FlushValue()
    local point = RoleData.Instance:GetCurrency(CurrencyType.GodSpaceEssence)
    UH.SetText(self.Value,point)
    UH.SetIcon(self.Icon,Item.GetIconId(CommonItemId.GodSpaceEssence))
end

function LandsBetweenCurrencyEssence:OnClickOperate()
    ViewMgr:OpenView(LandsBetweenProduceView) 
end

-- 神域主界面按钮
LandsBetweenIconSub = LandsBetweenIconSub or DeclareMono("LandsBetweenIconSub", UIWFlushPanel)
function LandsBetweenIconSub:LandsBetweenIconSub()
    self.data = LandsBetweenData.Instance
    self.data_cares = {
        {data = self.data.base_info, func = self.FlushPanel},
    }
end

function LandsBetweenIconSub:Awake()
    UIWFlushPanel.Awake(self)

    self.Timer:SetShowCallBack(function (t)

        local remain_time = self.data.base_info.ourter_space_timestamp - TimeManager:GetServerTime()

        if TIME_DAY_SECOND_NUM < remain_time then 
            return string.format(Language.LandsBetween.Desc.OpenWaitShow[1], DataHelper.GetDaXie(t.day))
        elseif TIME_HOUR_SECOND_NUM <= remain_time and TIME_DAY_SECOND_NUM >= remain_time then 
            return string.format(Language.LandsBetween.Desc.OpenWaitShow[2],tonumber(t.hour))
        elseif TIME_MINUTE_SECOND_NUM <= remain_time and TIME_HOUR_SECOND_NUM >= remain_time then 
            return string.format(Language.LandsBetween.Desc.OpenWaitShow[3],tonumber(t.min))
        else
            return string.format(Language.UiTimeMeter.Special, t.min ,t.s)
        end 

    end)

    self.Timer:SetCallBack(function()
        self:FlushPanel()
    end)
end

function LandsBetweenIconSub:FlushPanel()
    self.Timer.gameObject:SetActive(false)
    self.Tips2.gameObject:SetActive(false)

    if self.data:GetCurStatus() and self.data:GetCurStatus() < 3 then 
        self.Timer.gameObject:SetActive(true)

        self.Timer:StampTime(self.data.base_info.ourter_space_timestamp or 0)
    elseif self.data:GetCurStatus() == 5 then 
        self.Tips2.gameObject:SetActive(true)
        UH.SetText(self.Tips2,Language.LandsBetween.Desc.StatusCounting)
    elseif self.data:GetCurStatus() == 6 then 
        -- 同临时代码 
        self.Timer.gameObject:SetActive(true)
        self.Timer:StampTime(self.data.base_info.ourter_space_timestamp or 0)
    else 
        self.Tips2.gameObject:SetActive(true)
        UH.SetText(self.Tips2,Language.LandsBetween.Desc.Status[self.data:GetCurMapType()])
    end 
    -- 
end

function LandsBetweenIconSub:OnIconClick()
    LandsBetweenCtrl.Instance:OpenMainView()
end
