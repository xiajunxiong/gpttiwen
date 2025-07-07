UH = UH or BaseClass()

UH.icon_load_index = {}

function UH.SetText(obj, str)
    if str == nil then str = "" end
    if obj and obj.text ~= str then 
        obj.text = str
    end
end

function UH.SpriteName(obj, name)
    if obj == nil then return end
    obj.SpriteName = name
end

function UH.LocalPos(obj, v3)
    obj.localPosition = v3
end

function UH.LocalPosG(obj, v3)
    obj.transform.localPosition = v3
end

function UH.LocalScale(obj, v3)
    obj.localScale = v3
end

function UH.LocalScaleG(obj, v3)
    obj.transform.localScale = v3
end

function UH.AnchoredPosition(obj, v2)
    obj.anchoredPosition = v2
end

function UH.Pos(obj, v3)
    obj.position = v3
end

function UH.Color(obj, color)
    obj.color = color
end

function UH.GetIconSprite(icon_id, type, func)
    return IconGeter.Instance:GetItemIcon(icon_id, type ,func)
end
-- obj === UI.Image
function UH.SetIcon(obj, icon_id, mtype)
    local is_sp_set = (obj:GetType() == typeof(Game.UIImageSpriteSet))
    local sp_name = is_sp_set and "Sprite" or "sprite"
    local load_index = UH.beginLoadIcon(obj)
    if icon_id and icon_id ~= 0 then
        if mtype == nil then
            if type(icon_id) == "string" and (string.sub(icon_id, 1, 1) == "s" and string.sub(icon_id, 1, 2) ~= "sp") then
                mtype = ICON_TYPE.SKILL
            elseif type(icon_id) == "string" and string.sub(icon_id, 1, 2) == "fb" then
                mtype = ICON_TYPE.BIGFABAO
            elseif type(icon_id) == "string" and string.sub(icon_id, 1, 1) == "f" and string.sub(icon_id, 1, 2) ~= "fy" then
                mtype = ICON_TYPE.FABAO
            elseif type(icon_id) == "string" and string.sub(icon_id, 1, 5) == "title" then
                mtype = ICON_TYPE.TITLE
            elseif type(icon_id) == "string" and (string.sub(icon_id, 1, 1) == "b" or string.sub(icon_id, 1, 2) == "sp" or string.sub(icon_id, 1, 1) == "t"
         or string.sub(icon_id, 1, 2) == "fy") then
                mtype = ICON_TYPE.BUFF
            end
        end
        obj:SetObjActive(false)
        UH.GetIconSprite(icon_id, mtype or ICON_TYPE.ITEM,function(sp)
            if UH.endLoadIcon(obj,load_index) == false then return end
            if not obj:Equals(nil) then
                obj[sp_name] = sp
                obj:SetObjActive(true)
            end
        end)
    else
        if UH.endLoadIcon(obj,load_index) == false then return end
        if is_sp_set then
            obj.SpriteName = nil
        else
            obj[sp_name] = nil
            obj:SetObjActive(false)
        end
    end
end

-- obj === UIImageSpriteSet
-- level可以为空
function UH.SkillIcon(obj, id, level)
    local cfg = SkillData.GetDecConfig(id, level)
    if cfg then
        obj.Sprite = nil
        IconGeter.Instance:GetItemIcon(cfg.icon_id, ICON_TYPE.SKILL,function(sp)
            if not obj:Equals(nil) then
                obj.Sprite = sp
            end
        end)
    end
end

-- obj === Image
-- level可以为空
function UH.SetSkillIconToImage(image, id, level)
    local cfg = SkillData.GetDecConfig(id, level)
    if cfg then
        UH.SetIconToImage(image, cfg.icon_id, ICON_TYPE.SKILL)
    end
end

-- obj === UGUI Image
function UH.SetIconToImage(obj, icon_id, type)
    obj.sprite = nil
    local load_index = UH.beginLoadIcon(obj)
    UH.GetIconSprite(icon_id, type, function (sp) 
        if UH.endLoadIcon(obj,load_index) == false then return end
        if not obj:Equals(nil) then
            obj.sprite = sp
            obj.enabled = true
        end
    end)
end

-- obj === UGUI RawImage
function UH.SetIconToRawImage(obj, icon_id, type)
    obj.texture = nil
    local load_index = UH.beginLoadIcon(obj)
    UH.GetIconSprite(icon_id, type, function(sp) 
        if UH.endLoadIcon(obj,load_index) == false then return end
        if sp ~= nil then
            if not obj:Equals(nil) then
                obj.texture = sp.texture
            end
        end   
    end)
end

function UH.Alpha(obj, a)
    obj.color = Color.New(1, 1, 1, a)
end

function UH.FillAmount(obj, value)
    obj.fillAmount = value
end

--obj-----Transform类型 
--angle-------Quaternion类型
function UH.LocalRotate(obj, angle)
    obj.localRotation = angle
end

--obj-----Transform
--angle --Vector3
function UH.LocalEulerAngles(obj, angle)
    obj.localEulerAngles = angle
end

--@G货币使用类型封装
function UH.SetGold(obj,type,enum)
    enum = enum or HuoBi
    UH.SpriteName(obj,enum[type or 0])
end

-- 新购买活动专用，SetGold别用了
function UH.SetActGold(obj, type, cfg_or)
    if cfg_or == 1 then
        -- 人民币标识
        type = CurrencyType.RenMinBi
    end
    UH.SpriteName(obj, HuoBi[type or 0])
end

--设置角色头像 传入角色类型 最好使用角色头像控件 UIWAvatarCell
function UH.SetAvatar(obj, avatar_type, avatar_id,avatar_quality)
    if avatar_id and avatar_id ~= 0 and avatar_id > 1000 then
        UH.SetIcon(obj, string.format("head%d", avatar_id or 0),ICON_TYPE.HEAD)
    else
        UH.SpriteName(obj, string.format("head%d", avatar_type or 0))
    end
    AvatarEffect.SetQuaAnimEffect(obj.gameObject,avatar_quality)
end

--菱形头像
function UH.SetAvatarp(obj, avatar_type,avatar_id,avatar_quality)
    if avatar_id and avatar_id ~= 0 and avatar_id > 1000 then
        UH.SetIcon(obj, string.format("headp%d", avatar_id or 0),ICON_TYPE.HEAD)
    else
        UH.SpriteName(obj, string.format("headp%d", avatar_type or 0))
    end
    AvatarEffect.SetQuaAnimEffect(obj.gameObject,avatar_quality,true)
end

function UH.SizeDelta(obj, v2)
    if v2 then
        obj:GetComponent(typeof(UnityEngine.RectTransform)).sizeDelta = v2
    else
        return obj:GetComponent(typeof(UnityEngine.RectTransform)).sizeDelta
    end
end
function UH.SizeDelta2(obj, v2)
    if v2 then
        obj.sizeDelta = v2
    else
        return obj.sizeDelta
    end
end

function UH.InputAddListener(obj,func)
    obj.onValueChanged:AddListener(func)
end

function UH.InputRemoveListener(obj, func)
    obj.onValueChanged:RemoveListener(func)
end

function UH.onEndEdit(obj,func)
    obj.onEndEdit:AddListener(func)
end

function UH.removeEndEdit(obj,func)
    obj.onEndEdit:RemoveListener(func)
end

function UH.beginLoadIcon(obj)
    local obj_li = UH.icon_load_index[obj]
    if obj_li == nil then
        obj_li = 1
    else
        obj_li = obj_li + 1
    end
    UH.icon_load_index[obj] = obj_li
    return obj_li
end

function UH.endLoadIcon(obj,load_index)
    local obj_li = UH.icon_load_index[obj]
    if obj_li == nil then
        return false
    end
    if obj_li ~= load_index then
        return false
    else
        UH.icon_load_index[obj] = nil
        return true
    end
end
-- 将path_2的图片拼合到path_2的图片上,并返回对应的texture2D
function UH.PieceTexturePath(path_1,path_2,offset_x,offset_y)
    local tex_1 = ResourceManager.LoadStreamingTexture(path_1)
    local tex_2 = ResourceManager.LoadStreamingTexture(path_2)
    local piece_tex = UH.PieceTexture(tex_1,tex_2,offset_x,offset_y)
    return piece_tex
end

function UH.PieceTexture(tex_1,tex_2,offset_x,offset_y)
    for x= 0 ,tex_2.width do 
        for y = 0,tex_2.height do 
            local color_1 = UnityEngine.Texture2D.GetPixel(tex_2,x, y)
            local color_2 = UnityEngine.Texture2D.GetPixel(tex_1,offset_x+x, offset_y+y)
            local color_temp = {
                a = color_2.a,
                r = color_1.a * color_1.r + color_2.r * (1 - color_1.a),
                g = color_1.a * color_1.g + color_2.g * (1 - color_1.a),
                b = color_1.a * color_1.b + color_2.b * (1 - color_1.a),
            }
            UnityEngine.Texture2D.SetPixel(tex_1,offset_x+x, offset_y+y, color_temp)
        end 
    end 
    UnityEngine.Texture2D.Apply(tex_1)
    return tex_1
end

-- 区域截图
-- mark_obj选中区域
-- camera 相对相机
function UH.FeildShotScreen(mark_obj,camera)
    local screen_pos = camera:WorldToScreenPoint(mark_obj.transform.position)
    local rect = mark_obj.transform:GetComponent(typeof(UnityEngine.RectTransform)).rect
    local tex_temp = UnityEngine.Texture2D.New(rect.width,rect.height,UnityEngine.TextureFormat.RGBA32,false)
    rect.x = screen_pos.x + 1
    rect.y = screen_pos.y + 1
    UnityEngine.Texture2D.ReadPixels(tex_temp,rect,0,0)
    UnityEngine.Texture2D.Apply(tex_temp)
    return tex_temp
end

-- get texture2D from image 
function UH.CatchTexture2DFromImage(image)
    local rect = image.gameObject:GetComponent(typeof(UnityEngine.RectTransform)).rect
    local tex = UnityEngine.Texture2D.New(rect.width,rect.height,UnityEngine.TextureFormat.RGBA32,false)
    local pixels = UnityEngine.Texture2D.GetPixels(image.sprite.texture,rect.x,rect.y,rect.width,rect.height)
    UnityEngine.Texture2D.SetPixels(tex,pixels)
    UnityEngine.Texture2D.Apply(tex)
    return tex
end

-- 将texture2D存到设备中
-- 安卓/IOS 相册，pc,缓存位置
function UH.SavePhotoToPicture(texture,file_name,path)
    local bytes = UnityEngine.ImageConversion.EncodeToPNG(texture)
    local file_name = "/"..file_name..tostring(os.date("%Y%m%d-%H%M%S"))..".png"
    if path ~= nil then 
        System.IO.File.WriteAllBytes(path..file_name,bytes)
        return 
    end 
    if PLATFORM == PlatformType.Android and not IS_EDITOR then
        local origin_path = "/storage/emulated/0/DCIM/Screenshots"
        System.IO.File.WriteAllBytes(origin_path..file_name,bytes)
    elseif PLATFORM == PlatformType.IOS and not IS_EDITOR then
        System.IO.File.WriteAllBytes(UnityApp.persistentDataPath..file_name,bytes)
        -- ChannelAgent.ToPlatWithParam(UnityApp.persistentDataPath..file_name)
    else
        System.IO.File.WriteAllBytes(UnityApp.persistentDataPath..file_name,bytes)
    end
end

--@G计算屏幕边缘坐标 transform 超过并贴近边缘 中心坐标
--root根节点 存在缩放的或者需要子节点obj贴近父节点的范围
--设置位置的节点位置obj=transform root=transform
function UH.SetEdgePosition(obj,root)
    local pos = obj.localPosition
    local width = root.rect.width / 2 - obj.rect.width / 2
    local height = root.rect.height / 2 - obj.rect.height / 2
    if math.abs(pos.x) > width then
        UH.LocalPos(obj,Vector2.New(width > 0 and width * GameMath.Negative(pos.x) or width,obj.localPosition.y))
    end
    if math.abs(pos.y) > height then
        UH.LocalPos(obj,Vector2.New(obj.localPosition.x,height > 0 and height * GameMath.Negative(pos.y) or height))
    end
end

--@G是否超过屏幕边缘 中心坐标obj=transform root=transform
function UH.IsEdgePosition(obj,root)
    local pos = obj.localPosition
    if math.abs(pos.x) > (root.rect.width / 2 - obj.rect.width / 2) or math.abs(pos.y) > (root.rect.height / 2 - obj.rect.height / 2) then
        return true
    end
    return false
end

--obj=gameObject root=transform
--@G获取当前物体gameObject相对于根节点的坐标 root 父节点默认传空为根节点 中心坐标
function UH.GetRootPosition(obj,root)
    local world = obj.transform:InverseTransformPoint(root or Vector3.zero)
    return Vector2.New(-world.x,-world.y)
end

--@G屏幕点击坐标转点击当前的图片下的坐标[[rect:需要点击图标的RectTransform]]
function UH.ScreenPointToViewPoint(rect,mainCanvas)
    if mainCanvas == nil then
        local gameObject = UnityGameObj.FindGameObjectWithTag("MainCanvas")
        mainCanvas = gameObject:GetComponent(typeof(UnityEngine.Canvas))
    end
    local _,localPoint = UnityEngine.RectTransformUtility.ScreenPointToLocalPointInRectangle(rect,
    UnityEngine.Input.mousePosition,mainCanvas.worldCamera,nil)
    return localPoint or Vector2.zero
end

--@GViewPoint转为Texture2D对应坐标[[rect:需要点击图标的RectTransform]]
function UH.ViewPointToTexture2DPoint(rect,localPoint)
    local view_x = math.modf((rect.sizeDelta.x / 2 + localPoint.x))
    local view_y = math.modf((rect.sizeDelta.y / 2 + localPoint.y))
    return Vector2.New(view_x,view_y)
end

-- @G使物体obj移动到 目标位置target speed 移动速度 end_func 到达位置之后调用
-- 返回出去的handler需要自己手动销毁obj=gameObject
function UH.MoveToTarget(obj,target,speed,end_func)
    return RunUntilTrue(function()
        local local_pos = obj.transform.localPosition
        if GameMath.Equals(local_pos,target) then
            if end_func then Call(end_func) end
            return true
        end
        UH.LocalPosG(obj,Vector3.MoveTowards(local_pos,target,speed or 1))  
    end)
end

-- @G使物体obj缩放到 目标大小target speed 移动速度 end_func 到达位置之后调用
-- 返回出去的handler需要自己手动销毁obj=gameObject
function UH.ScaleToTarget(obj,target,speed,end_func)
    return RunUntilTrue(function()
        local local_scale = obj.transform.localScale
        if GameMath.Equals(local_scale,target) then
            if end_func then Call(end_func) end
            return true
        end
        UH.LocalScaleG(obj,Vector3.MoveTowards(local_scale,target,speed or 1))
    end)
end

-- @G使物体obj旋转到 目标角度target speed 移动速度 end_func 到达位置之后调用
-- 返回出去的handler需要自己手动销毁obj=gameObject
--target 必须是 Quaternion.Euler(x,y,z)
function UH.RotateToTarget(obj,target,speed,end_func)
    return RunUntilTrue(function()
        local local_rotation = obj.transform.localRotation
        if GameMath.Equals(local_rotation,target) then
            if end_func then Call(end_func) end
            return true
        end
        UH.LocalRotate(obj,Quaternion.RotateTowards(local_rotation,target,speed or 1))
    end)
end

--@G组件实例化重命名view=LuaMonoBehaviour
function UH.RenameGame(view,name)
    if view.gameObject == nil then
        view.name = name
    else
        view.gameObject.name = name
    end
end

--@G单独设置Rect位置tab[x,y]
function UH.SetRectPosition(obj,tab)
	local pos = obj.localPosition
	pos.x = tab.x or pos.x
	pos.y = tab.y or pos.y
	obj.localPosition = pos
end

--@GToggle设置
function UH.SetToggle(obj,is_on)
    if obj.isOn == nil then
        return
    end
    obj.isOn = is_on
end

--@GEnabled设置
function UH.SetEnabled(obj,enabled)
    if obj.enabled == nil then
        return 
    end
    obj.enabled = enabled
end

--@G置灰控件设置
function UH.SetInter(obj,inter)
    if obj == nil then return end
    if obj.Interactable == nil then
        return
    end
    obj.Interactable = inter
end

--@G销毁特效 不销毁会有特效残留obj=EffectTool
function UH.StopEffect(obj,effect_handle)
    if obj == nil then return end
    if effect_handle then
        obj:Stop(effect_handle)
        effect_handle = nil
    end
end

--@G播放特效 存在gameObject时 可以不用挂载特效obj=EffectTool
function UH.PlayEffect(obj,key,gameObject)
    if obj == nil then return end
    if not obj.isActiveAndEnabled then
        return
    end
    if gameObject then
        return obj:PlayByEffectId(key,gameObject)
    end
    return obj:Play(key)
end

--把场景中的pos（V3,场景obj.transform.position）转换为rectTran所在平面的屏幕世界坐标
--返回值为V3
function UH.GetWorldPosToScreen(pos, rectTran)
    return Game.UIManager.CalculateScreenPosition(pos, SceneManager:MainCamera(), Game.UIManager.Singleton():GetComponent(typeof(UnityEngine.Canvas)), rectTran)
end

--根据index设置ObjList哪个显示
function UH.SetListVisible(obj,index)
    if obj == nil then return end
    for i=1,obj:Length() do
        obj[i]:SetActive(i == index)
    end
end