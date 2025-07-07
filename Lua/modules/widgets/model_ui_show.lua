local camera_path = "ui/widgets/modelCamera.prefab"
PreloadCtrl.RegisterPreload(
    function()
        PreloadCtrl.RegisterLoading()
        GameObjPool:RegisterPool(
            camera_path,
            function(error)
                if not StringIsEmpty(error) then
                    Debuger.LogError("modelCamera pool register error:%s", error)
                end
                PreloadCtrl.UnregisterLoading()
            end,
            false
        )
    end
)

local load_waiting_path = "ui/widgets/load_waiting.prefab"
PreloadCtrl.RegisterPreload(
    function()
        PreloadCtrl.RegisterLoading()
        GameObjPool:RegisterPool(
            load_waiting_path,
            function(error)
                if not StringIsEmpty(error) then
                    Debuger.LogError("load_waiting pool register error:%s", error)
                end
                PreloadCtrl.UnregisterLoading()
            end,
            false
        )
    end
)

local RenderTexture = UnityEngine.RenderTexture
local RenderTextureQualityConfig = Game.RenderTextureQualityConfig

local ModelCameraCount = 0
-- local CameraFieldofView = 12.5 -- 摄像机夹角 field of view
ModelUIShow = ModelUIShow or DeclareMono("ModelUIShow")
ModelUIShow.count_list = {}
--LoadWaitColor=====int 0:浅色，1：深色 ，默认浅色
function ModelUIShow:ModelUIShow()
    self.wait_loading_timer = nil
    self.wait_circle = nil
    self.loaded = false
    self.camera_loaded_delay_handle = nil
    self.another_ui_obj_list = {}
    self.adaptive_offset_pos_cache = Vector3.zero
    self.block_default_layer = false
end

function ModelUIShow:OnEnable()
    if self.model_camera == nil then
        self.model_camera = GameObjPool:Get(camera_path)
        self.model_camera_mono = self.model_camera:GetLuaComponent("ModelCamera")
        self.model_camera_mono.Camera.gameObject:SetActive(false)
        --Image的RectSize在自适配时无法在enable的时候取到正确的宽高，延一帧
        self.camera_loaded_delay_handle = TimeHelper:AddRunFrameTimer(function() 
            self.camera_loaded_delay_handle = nil
            self:OnCameraLoaded()
        end)
    end
    if self.model_camera ~= nil then
        self.model_camera:SetActive(true)
    end
    if self.loaded == false then        --如果控件被激活但还没有任何加载的话，先把贴图控件关掉，防止界面闪白
        if self.ShowTexture == nil then
            self.ShowTexture = self.gameObject:GetComponent(typeof(UnityUI.RawImage))
            self.RectTransform = self.gameObject:GetComponent(typeof(UnityEngine.RectTransform))
        end
        self.ShowTexture.enabled = false
    end
end
function ModelUIShow:ReCalcModelCameraCount(count)
    return self:CalcModelCameraCount(count)
end
function ModelUIShow:CalcModelCameraCount(count)
    if ModelUIShow.count_list[count] == 1 then
        count = count + 1
        return self:ReCalcModelCameraCount(count)
    end
    return count
end
function ModelUIShow:OnDisable()
    if self.model_camera ~= nil then
        self.model_camera:SetActive(false)
    end
    -- LogDG("ModelCameraCount", "OnDisable", ModelCameraCount)
    -- ModelCameraCount = ModelCameraCount + 1
end

function ModelUIShow:OnDestroy()
    -- if self.is_loaded then
    --     LogError("OnDestroy ModelCameraCount ",ModelCameraCount)
    -- ModelCameraCount = ModelCameraCount - 2
    if self.model_camera_count then
        -- LogDG("ModelCameraCount", "OnDestroy", self.model_camera_count)
            ModelUIShow.count_list[self.model_camera_count] = 0
            ModelCameraCount = ModelCameraCount - 1
        end
    --     self.is_loaded = false
    -- end
    if self.camera_loaded_delay_handle ~= nil then
        TimeHelper:CancelTimer(self.camera_loaded_delay_handle)
        self.camera_loaded_delay_handle = nil
    end
    if self.wait_loading_timer ~= nil then
        TimeHelper:CancelTimer(self.wait_loading_timer)
        self.wait_loading_timer = nil
    end
    if self.wait_circle ~= nil then
        GameObjPool:Release(self.wait_circle)
        self.wait_circle = nil
    end

    if self.show_shadow and self.model_camera_mono then
        self.model_camera_mono.Shadow:SetActive(false)
    end
    if self.model_camera and self.model_camera_mono then
        self.model_camera_mono:Release()
        GameObjPool:Release(self.model_camera)
    end

    self:ReleaseUIObj()
    for _,v in pairs(self.another_ui_obj_list) do
        v:DeleteMe()
    end
    self.another_ui_obj_list = {}
    self.model_camera = nil
end

function ModelUIShow:ReleaseUIObj()
    if self.ui_obj then
        self.show_obj = nil
        self.ui_obj:DeleteMe()
        self.ui_obj = nil
    end
end
--

ModelUIShow.ShadowCameraDir = {
    Default = Vector3.New(35, 160, -8.9),
    Pet = Vector3.New(57, 168.8, -1.6)
}

--[[ -- 外部显示接口
    show_data =
    {
        ui_obj = UIObjDrawer及其子类UIChDrawer等,
        view = self,（用于生成key来读取一个独立的镜头配置）
        ！！！！！seq = 1、2、3...，（同一界面的不同镜头可能需要不同的配置，此时需传入不同的seq）
        show_shadow = 是否显示阴影
        shadow_dir = Vector3 阴影方向，使用ModelUIShow.ShadowCameraDir枚举设置，为空表示ModelUIShow.ShadowCameraDir.Default
        not_show_anim = 模型加载完不播放动画（屏蔽obj:SampleMainAnim）
    }
    ！！！一个ui_obj只需要用一次SetShowData来绑定，之后更改模型只需要对ui_obj本身用SetData或其它方式更改即可
]]
function ModelUIShow:SetShowData(show_data)
    if self.ShowTexture == nil then
        self.ShowTexture = self.gameObject:GetComponent(typeof(UnityUI.RawImage))
        self.RectTransform = self.gameObject:GetComponent(typeof(UnityEngine.RectTransform))
    end
    if show_data.show_shadow == nil then --默认都显示阴影
        show_data.show_shadow = true
    end
    if show_data.shadow_dir == nil then
        show_data.shadow_dir = ModelUIShow.ShadowCameraDir.Default
    end
    self.show_data = show_data
    if self.ui_obj ~= nil then
        self:ReleaseUIObj()
    end
    self.ui_obj = show_data.ui_obj
    self.show_obj = self.ui_obj:GetRoot()
    self.additional_click_event = show_data.additional_click_event
    -- self.default_angle = show_data.default_angle
    -- self.show_model_type = show_data.show_model_type
    self.show_shadow = show_data.show_shadow
    if self.is_loaded then
        self:ShowObj()
    else
        self.delay_show = true
    end
end
--多模型展示
function ModelUIShow:AddShowData(params)
    if self.model_camera_mono then
        local parent = self.model_camera_mono.Adaptive
        for _, data in ipairs(params) do
            data.ui_obj:Enable()
            local show_obj = data.ui_obj:GetRoot()
            show_obj:SetParent(parent, false)
            show_obj:SetLocalPosition(data.pos)
            table.insert(self.another_ui_obj_list, data.ui_obj)
        end
    else
        self.delay_show_params = params
    end
end

function ModelUIShow:SetAdaptiveOffSet(off_pos)
    self.adaptive_offset_pos_cache = off_pos
    if self.model_camera_mono ~= nil then
        self.model_camera_mono:setAdaptiveOffset(self.adaptive_offset_pos_cache)
    end
end

function ModelUIShow:getShowObj()
    if not self.show_obj then
        self.show_obj = self.ui_obj:GetRoot()
    end
    return self.show_obj
end

function ModelUIShow:OnCameraLoaded()
    self.model_camera_mono = self.model_camera:GetLuaComponent("ModelCamera")
    self.model_camera_mono.Camera.gameObject:SetActive(false)
    if self.block_default_layer then
        self.model_camera_mono.Camera.cullingMask = LayerMask.GetMask("QualityRole")
    else
        self.model_camera_mono.Camera.cullingMask = LayerMask.GetMask("QualityRole","Default")
    end
    if ModelCameraCount > 50 then
        ModelCameraCount = 0
    end
    ModelCameraCount = ModelCameraCount + 1
    ModelCameraCount = self:CalcModelCameraCount(ModelCameraCount)
    ModelUIShow.count_list[ModelCameraCount] = 1
    self.model_camera_count = ModelCameraCount

    -- LogDG("ModelCameraCount", "Add", ModelCameraCount)
    -- LogError("OnCameraLoaded ModelCameraCount ",ModelCameraCount)
    self.model_camera:SetPosition(Vector3.New(1000 + 100 * ModelCameraCount, 1000, 1000))
    self.is_loaded = true
    if self.delay_show then
        self:ShowObj()
    end
    self.model_camera_mono:setAdaptiveOffset(self.adaptive_offset_pos_cache)
end

function ModelUIShow:ShowObj()
    self.model_camera_mono.ModelUIShow = self
    self.model_camera_mono.ShowTexture = self.ShowTexture
    self.model_camera_mono.RectTransform = self.RectTransform
    if self.show_shadow then
        self.model_camera_mono.Shadow:SetActive(true)
    end
    self.ShowTexture.gameObject:SetActive(true)
    -- local size = self.RectTransform.sizeDelta
    local imageSize = self.RectTransform.rect.size
    -- LogError("SIZE=====",imageSize)
    -- TimeHelper:AddDelayFrameTimer(function() 
    --     LogError("111----SIZE=====",self.RectTransform.rect.size)
    -- end)
    local render_size_x = imageSize.x * 1.3 -- 这里乘于2不然会模糊
    local render_size_y = imageSize.y * 1.3
    self.render_texture = self.model_camera_mono:GetRenderTexture(render_size_x, render_size_y)
    self.ShowTexture.texture = self.render_texture
    self.model_camera_mono:SetShowObj(self.show_data)
    if self.delay_show_params then
        self:AddShowData(self.delay_show_params)
        self.delay_show_params = nil
    end
end

function ModelUIShow:UiObj()
    return self.ui_obj
end

function ModelUIShow:OnClickDrag()
    if self:getShowObj() then
        local mouse_pos_x = UnityEngine.Input.mousePosition.x
        if self.mouse_pos_x and self.mouse_pos_x ~= mouse_pos_x then
            local obj_rot = self:getShowObj():GetLocalEulerAngles()
            obj_rot.y = obj_rot.y + self.mouse_pos_x - mouse_pos_x
            self:getShowObj():SetLocalEulerAngles(obj_rot)
        end
        self.mouse_pos_x = mouse_pos_x
    end
    self.is_click = false
end

function ModelUIShow:ResetAngle()
    if self:getShowObj() then
        local obj_rot = self:getShowObj():GetLocalEulerAngles()
        obj_rot.y = 0
        self:getShowObj():SetLocalEulerAngles(obj_rot)
    end
end

function ModelUIShow:OnClickDown()
    self.mouse_pos_x = nil
    self.is_click = true
end

function ModelUIShow:OnClick()
    if self.is_click then
        self.additional_click_event()
    end
end

function ModelUIShow:BeginWaitLoading()
    self.ShowTexture.enabled = false
    if self.wait_loading_timer ~= nil then
        return
    end
    self.loaded = false
    local circle_time = IS_EDITOR and 0.0 or 0.1
    self.wait_loading_timer = TimeHelper:AddDelayTimer(function()
        self.wait_circle = GameObjPool:Get(load_waiting_path)
        self.wait_circle:SetParent(self.gameObject)
        self.wait_circle:ResetTransform()
        local txt_col = self.LoadWaitColor == 1 and COLORS.Yellow8 or COLORS.Yellow3
        local circle_txt_obj = self.wait_circle:FindChild("txt")
        local circle_txt = circle_txt_obj:GetComponent(typeof(UnityUI.Text))
        circle_txt.color = txt_col
    end,circle_time)
    
end

function ModelUIShow:EndWaitLoading()
    self.loaded = true
    self.ShowTexture.enabled = true
    if self.wait_loading_timer ~= nil then
        TimeHelper:CancelTimer(self.wait_loading_timer)
        self.wait_loading_timer = nil
    end
    if self.wait_circle ~= nil then
        GameObjPool:Release(self.wait_circle)
        self.wait_circle = nil
    end
end


ModelCamera = ModelCamera or DeclareMono("ModelCamera")

function ModelCamera:ModelCamera()
    self.parent = nil
    self.render_texture = nil
    self.main_path = ""
    -- self.show_data = {}
    -- self.fov = self.Camera.fieldOfView
    self.is_loading = false
    self.loaded_delay_timer = nil
    self.adaptive_orginal_pos = self.Adaptive:GetLocalPosition()
    self.adaptive_offset_pos = Vector3.zero
end

-- function ModelCamera:SetShowData(show_data)
--     self.show_data = show_data
-- end

function ModelCamera:GetRenderTexture(width, height)
    if self.render_texture ~= nil then
        return self.render_texture
    end
    self.render_texture = RenderTexture.GetTemporary(width, height, 1, 0)
    --self.render_texture.autoGenerateMips = false
    -- self.render_texture.useMipMap = false
    -- self.render_texture.autoGenerateMips = false
    self.Camera.targetTexture = self.render_texture
    return self.render_texture
end

ModelCameraAnchor = {
    --头顶贴上
    Top = 0,
    --中心对准
    Center = 1,
    --脚底贴下
    Bottom = 2
}

ModelCameraInteractable = {
    --跟随原始设定
    DontChange = 0,
    --强行可旋转
    ForceTrue = 1,
    --强行不可旋转
    ForceFalse = 2
}

ModelCameraAdaption = {
    --每次都自适应
    Always = 0,
    --只自适应一次
    Once = 1,
    --从不自适应
    Never = 2
}

ModelCameraFill = {
    --不填充
    DontFill = 0,
    --以横为标准填充
    BasedOnWidth = 1,
    --以纵为标准填充
    BasedOnHeight = 2
}

function ModelCamera:SetShowObj(show_data)
    local key = string.format("%s_%d", show_data.view._type_name, show_data.seq or 1)
    self.CameraBrain.Key = key
    local ui_obj = show_data.ui_obj
    ui_obj.model_camera = self
    ui_obj:Enable()
    self.show_shadow = show_data.show_shadow
    self.shadow_dir = show_data.shadow_dir
    self.not_show_anim = show_data.not_show_anim
    -- self.adapte_all = show_data.adapte_all
    local show_obj = ui_obj:GetRoot()

    local parent = self.Adaptive
    if self.rtq_config == nil then
        self.rtq_config = parent:GetComponent(typeof(RenderTextureQualityConfig))
    end

    -- self.parent = parent

    show_obj:SetParent(parent, false)
    self.ModelUIShow.ShowTexture.enabled = false
    -- show_obj:SetLocalScale(Vector3.zero)
    -- self.RectTransform.localScale = Vector3.one * 0.01
    -- self.RectTransform.localScale = Vector3.zero

    if self.load_event_node then
        return
    end

    local adapted = false
    self.load_event_node =
        EventNode.New(
        function(loaded)
            if loaded then
                --模型切换时有些特效需要等待销毁
                -- show_obj:SetLocalScale(Vector3.one)
                -- self.RectTransform.localScale = Vector3.one
                -- Game.MyTool.Pause()
                -- end
                -- if show_data.delay_frame then
                -- TimeHelper:AddDelayFrameTimer(on_load, 10)
                -- else
                --     on_load()
                -- end
                -- local function on_load()
                if not self.not_show_anim then
                     ui_obj:SampleMainAnim(ChAnimType.Idle, 0.3)
                 end
                -- self.ModelUIShow.ShowTexture.enabled = false
                if self.loaded_delay_timer ~= nil then
                    TimeHelper:CancelTimer(self.loaded_delay_timer)
                end
                --延一帧显示，防止动画闪动(坐骑入场)
                self.ShowTexture.enabled = false
                self.loaded_delay_timer = TimeHelper:AddDelayFrameTimer(
                    function()
                        self.loaded_delay_timer = nil
                        self.ModelUIShow:EndWaitLoading()
                        local path_changed = ui_obj.main_path ~= self.main_path
                        if path_changed then
                            self.CameraBrain.CurMainResPath = ui_obj.main_path
                            self.ModelUIShow:ResetAngle()
                        end
                        self.main_path = ui_obj.main_path
                        self:Reposition(show_obj)
                        self.rtq_config:SetupRendererMaterialProperty(show_obj)
                        self.Camera.gameObject:SetActive(true)
                        if show_data.load_callback then
                            show_data.load_callback()
                        end
                    end
                )
            else
                -- self.Camera.gameObject:SetActive(true)
                -- show_obj:SetLocalScale(Vector3.one * 0.01)
                -- show_obj:SetLocalScale(Vector3.one)
                -- LogError("Not Loaded")
                self.ModelUIShow:BeginWaitLoading()
            end
        end
    )
    if self.load_event_node ~= nil then
        local node = ui_obj:GetLoadEventNode()
        if node ~= nil then
            self.load_event_node:AddChild(node)
        end
    end

    if IS_EDITOR then
        self.CameraBrain.OnDataChanged = BindTool.Bind(self.Reposition, self, show_obj)
    end
end

local function transformReference(point, source, target)
    local w = source:TransformPoint(point)
    local l = target:InverseTransformPoint(w)
    return l
end

local axes = {"x", "y", "z"}

function ModelCamera:Reposition(show_obj, model_camera_data)
    local model_camera_data = self.CameraBrain:GetData()

    local get_off = model_camera_data:GetResOffset(self.main_path);
    local offset = model_camera_data.offset + get_off
    -- LogError("set offset y 11111====",offset,model_camera_data.offset,get_off)

    local offset_z = 0

    local adaption = model_camera_data.adaption:ToInt()
    if adaption == ModelCameraAdaption.Always or (adaption == ModelCameraAdaption.Once and not self.adapted) then
        if adaption == ModelCameraAdaption.Once then
            self.adapted = true
        end
        -- local props = show_obj:GetComponentsInChildren(typeof(Game.ModelCameraProperty), false)
        -- if props.Length == 0 then
        --     -- 开发阶段有的资源不全
        --     return true
        -- end
        local max = Vector3.New()
        local min = Vector3.New()
        -- for i = 0, props.Length - 1 do
        --     local prop = props[i]
        if show_obj.transform.childCount <= 1 then
            return true
        end
        local skin_tran = show_obj.transform:GetChild(1)
        --[[ if skin_tran == nil then
            return true
        end ]]
        local prop = skin_tran:GetComponent(typeof(Game.ModelCameraProperty))
        if prop == nil then
            return true
        end
            local boundMax = transformReference(prop.boundMax, prop.transform, show_obj.transform)
            local boundMin = transformReference(prop.boundMin, prop.transform, show_obj.transform)
            -- LogError("boundMax", boundMax)
            -- LogError("boundMin", boundMin)
            for _, axis in pairs(axes) do
                if not max[axis] or boundMax[axis] > max[axis] then
                    max[axis] = boundMax[axis]
                end
                if not min[axis] or boundMin[axis] < min[axis] then
                    min[axis] = boundMin[axis]
                end
            end
        -- end

        -- x = offset.x
        -- y = offset.y
        -- z = -extent_z
        -- LogError("!!!!!!!", max)
        -- LogError("!!!!!!!", min)

        -- local height = max.y - min.y

        local camera_tan = math.tan(math.rad(self.Camera.fieldOfView / 2))

        -- local extent_y = (max.y - min.y) / 2
        -- local fov_y = math.abs(offset.z) * camera_tan
        -- local extent_z = (max.z - min.z) / 2

        -- show_obj:SetLocalPosition(-(max - min) / 2)
        -- x轴、z轴本已居中，不需要更改
        -- local child = show_obj.transform:GetChild(0)
        -- show_obj:SetLocalPosition(Vector3.New(sp_x[tonumber(child.gameObject.name)] or 0, -extent_y, 0))
        -- show_obj:SetLocalPosition(Vector3.New(0, -(max.y - min.y) / 2, 0))
        -- LogError("model_camera_data.anchor", model_camera_data.anchor)

        local fill = model_camera_data.fill:ToInt()
        if fill == ModelCameraFill.BasedOnWidth then
            -- local extent_x = (max.x - min.x) / 2
            -- local fov_x = math.abs(offset.z) * camera_tan
            -- local ratio = extent_x / fov_x
            -- offset.z = offset.z * ratio
            -- --相机坐标
            -- show_obj:SetLocalPosition(Vector3.New(0, -(max.y - min.y) / 2, -(max.x - min.x) / 2 / camera_tan))
            offset_z = -(max.x - min.x) / 2 / camera_tan
        elseif fill == ModelCameraFill.BasedOnHeight then
            -- local extent_y = (max.y - min.y) / 2
            -- local fov_y = math.abs(offset.z) * camera_tan
            -- local ratio = extent_y / fov_y
            -- offset.z = offset.z * ratio
            offset_z = -(max.y - min.y) / 2 / camera_tan
        --     show_obj:SetLocalPosition(Vector3.New(0, -(max.y - min.y) / 2, -(max.y - min.y) / 2 / camera_tan))
        -- else
        --     show_obj:SetLocalPosition(Vector3.New(0, -(max.y - min.y) / 2, 0))
        end

        local extent_y = (max.y - min.y) / 2

        -- if self.adapte_all then
        --     show_obj:SetLocalPosition(-(min + max) / 2)
        -- else
        show_obj:SetLocalPosition(Vector3.New(0, -extent_y, offset_z))
        -- end

        local fov_y = math.abs(offset.z + offset_z + max.z) * camera_tan

        local anchor = model_camera_data.anchor:ToInt()
        -- LogError(string.format("anchor====%s,fov_y=%s,extent_y=%s",tostring(anchor),tostring(fov_y),tostring(extent_y)))
        if anchor == ModelCameraAnchor.Center then
            -- local center_y = (max_y + min_y) / 2 - self.offset.y
            -- y = y - height / 2
        elseif anchor == ModelCameraAnchor.Top then
            -- y = y + fov_y - height
            offset.y = offset.y + (fov_y - extent_y)
        else
            -- y = y - fov_y
            offset.y = offset.y - (fov_y - extent_y)
            -- LogError(string.format("set offset y====%s,fov_y=%s,extent_y=%s",tostring(offset),tostring(fov_y),tostring(extent_y)))
        end
    -- show_obj:GetParent():SetLocalPosition(offset)
    -- LogError("height", height)
    -- LogError("y", y)
    -- else
    --     -- x = offset.x
    --     -- y = offset.y
    --     -- z = offset.z
    --     show_obj:SetLocalPosition(Vector3.zero)
    --     show_obj:GetParent():SetLocalPosition(offset)
        self.Camera.farClipPlane = -offset.z - offset_z + 10
        show_obj:GetParent():SetLocalPosition(offset)
        self:onShowObjParentPosChange(show_obj:GetParent())
        show_obj:GetParent():SetLocalEulerAngles(model_camera_data.rotationEularAngle)
        show_obj:SetLocalEulerAngles(model_camera_data:GetEularAngle(self.main_path))
    
    elseif adaption == ModelCameraAdaption.Never then
        show_obj:GetParent():SetLocalPosition(offset)
        self:onShowObjParentPosChange(show_obj:GetParent())
        --优化计算farClip要用到坐标转换，所以先把位置和旋转都设置好
        show_obj:GetParent():SetLocalEulerAngles(model_camera_data.rotationEularAngle)
        show_obj:SetLocalEulerAngles(model_camera_data:GetEularAngle(self.main_path))
        local setted_far_pl = false
        if show_obj.transform.childCount > 1 then       --根据模型的z轴深度决定相机farClip的偏移值
            local skin_tran = show_obj.transform:GetChild(1)
            local prop = skin_tran:GetComponent(typeof(Game.ModelCameraProperty))
            if prop ~= nil then
                local boundMax = transformReference(prop.boundMax, prop.transform, self.Camera.transform)
                local boundMin = transformReference(prop.boundMin, prop.transform, self.Camera.transform)              
                self.Camera.farClipPlane = math.max(0,math.max(boundMax.z,boundMin.z)) + 10
                setted_far_pl = true
            end
        end
        if setted_far_pl == false then  --没有框体的话给个plane默认值
            self.Camera.farClipPlane = 20
        end
    else
        if IS_EDITOR then
            Debuger.LogError("ModelCamera:Reposition Once类型已经不用了！请使用Always或Never!main_path=" ,self.main_path)
        end
        -- self.Camera.farClipPlane = -offset.z - offset_z + 10                

    end


    -- LogError(offset)

    -- show_obj:SetLocalRotation(Quaternion.Euler(eular.x, eular.y, eular.z))
    
    if self.show_shadow then
        self.Shadow:SetEulerAngles(show_obj:GetEulerAngles())
        self.Shadow:SetPosition(show_obj:GetPosition())
        self.ShadowCamera:SetLocalEulerAngles(self.shadow_dir)
    end
    if self.ShowTexture then
        local interactable = model_camera_data.interactable:ToInt()
        if interactable ~= ModelCameraInteractable.DontChange then
            if interactable == ModelCameraInteractable.ForceTrue then
                self.ShowTexture.raycastTarget = true
                local uies = self.ShowTexture:GetComponents(typeof(Game.LuaUIEvent))
                for i = 0, uies.Length - 1 do
                    uies[i].enabled = true
                end
            else
                self.ShowTexture.raycastTarget = false
            end
        end
    end

    return true
end

function ModelCamera:onShowObjParentPosChange(parent)
    if parent == self.Adaptive then
        self.adaptive_orginal_pos = parent:GetLocalPosition()
        if self.adaptive_offset_pos ~= Vector3.zero then
            parent:SetLocalPosition(self.adaptive_offset_pos + self.adaptive_offset_pos)
        end
    end
end

function ModelCamera:setAdaptiveOffset(off)
    self.adaptive_offset_pos = off
    self.Adaptive:SetLocalPosition(self.adaptive_orginal_pos + self.adaptive_offset_pos)
end

function ModelCamera:Release()
    if self.loaded_delay_timer ~= nil then
        TimeHelper:CancelTimer(self.loaded_delay_timer)
        self.loaded_delay_timer = nil
    end
    if self.rtq_config then
        self.rtq_config:CleanRendererMaterialProperty()
        self.rtq_config = nil
    end
    if self.Camera ~= nil and not self.Camera:Equals(nil) then
        self.Camera.targetTexture = nil
    end
    if self.render_texture ~= nil then
        RenderTexture.ReleaseTemporary(self.render_texture)
        self.render_texture = nil
    end
    if self.load_event_node then
        self.load_event_node:DeleteMe()
        self.load_event_node = nil
    end

    if self.reposition_runner then
        Runner.Instance:RemoveRunUntilTrue(self.reposition_runner)
        self.reposition_runner = nil
    end

    self.adapted = false

    self.CameraBrain:OnRelease()
end
