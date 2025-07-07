--该模块废弃 不要再使用 用UIWAvatarCell
AvatarCell = AvatarCell or DeclareMono("AvatarCell", UIWFlushPanel)
--头像：方形头像 圆形头像
--IsCircle（暂方）
--背景款式自己换吧
--IsOther 是否使用他人数据

--主角 使用AvatarCell
--非头像 UH.SetAvatar

function AvatarCell:FlushMine()
    self.data = RoleData.Instance:GetBaseData()
    self:FlushAvatar()
end

-- 传入 带形象结构的vo即可
--data = {
--    appearance = {
--        avatar_type = [num]
--    }
--}
function AvatarCell:SetData(data) 
    self.data = data
    self:FlushAvatar()
end

function AvatarCell:Awake()
    UIWFlushPanel.Awake(self)
    self.data = nil
    if self.IsOther == false and self.appearance_care == nil then
        self.appearance_care = RoleData.Instance:GetBaseData():Care(BindTool.Bind(self.FlushMine, self), "appearance")
        self:FlushMine()
    end
end

--角色类型 头像id
function AvatarCell:SetAvatar(avatar_type,avatar_id,avatar_quality)
    self.data = {appearance = {avatar_type = avatar_type,avatar_id = avatar_id,avatar_quality = avatar_quality}}
    self:FlushAvatar()
end

function AvatarCell:FlushAvatar()
    UH.SetAvatar(self.Icon, self.data.appearance.avatar_type,self.data.appearance.avatar_id,self.data.appearance.avatar_quality)
end

function AvatarCell:OnDestroy()
    UIWFlushPanel.OnDestroy(self)
    if self.appearance_care then
        RoleData.Instance:GetBaseData():Uncare(self.appearance_care)
        self.appearance_care = nil
    end
end

function AvatarCell:Cancel()
    self.data = nil
    UH.SpriteName(self.Icon, nil)
    if self.appearance_care then
        RoleData.Instance:GetBaseData():Uncare(self.appearance_care)
        self.appearance_care = nil
    end
end


------------------------

local avatar_qua_anim_path =  "ui/widgets/fs_qua_anim.prefab"

PreloadCtrl.RegisterPreload(function()
    PreloadCtrl.RegisterLoading()
    GameObjPool:RegisterPool(avatar_qua_anim_path,function(error)
        if not StringIsEmpty(error) then
            Debuger.LogError("avatar fs_qua_anim register error:%s", error)
        end
        PreloadCtrl.UnregisterLoading()
    end,false)
end)

--该模块废弃 不要再使用 用UIWAvatarCell
AvatarEffect = AvatarEffect or DeclareMono("AvatarEffect")
--AnimTool======UISpriteAnimTool

function AvatarEffect.SetQuaAnimEffect(gameObject,quality,is_diamond)
    local effect = nil
    local anim_qua_obj = gameObject:FindChild("fs_qua_anim")
    if anim_qua_obj ~= nil then
        if quality == nil or quality == 0 then
            GameObjPool:Release(anim_qua_obj)
            -- anim_qua_obj:SetActive(false)
            return
        end
        -- anim_qua_obj:SetActive(true)
    end
    if quality == nil or quality == 0 then
        if anim_qua_obj ~= nil then
            GameObjPool:Release(anim_qua_obj)
        end
        return
    end
    if anim_qua_obj == nil then
        anim_qua_obj = GameObjPool:Get(avatar_qua_anim_path,gameObject)
    end
    if anim_qua_obj ~= nil then
        effect = anim_qua_obj:GetLuaComponent("AvatarEffect")
        if effect ~= nil then
            effect:SetAnimData(quality,is_diamond)
        end
    end
    return effect
end

function AvatarEffect:SetAnimData(quality,is_diamond)
    if is_diamond == nil then
        UIWPetHeadCell.EffAnimName(quality,self.AnimTool)
    else
        UIWPetHeadCell.EffAnimNameDiamond(quality,self.AnimTool)
    end
end

function AvatarEffect.RecieveQuaAnimObj(parent)
    local anim_qua_obj = parent:FindChild("fs_qua_anim")
    if anim_qua_obj ~= nil then
        GameObjPool:Release(anim_qua_obj)
    end
end

function AvatarEffect.SetImageEnabled(obj,quality)
    UH.SetEnabled(obj,quality == nil or quality == 0)
end

function AvatarEffect:OnDestroy()
    UIWFlushPanel.OnDestroy(self)
    self.gameObject:SetActive(false)
end

--===================================================================================================
--头像背景类型
AvatarImageType = {
    Main = 1,
    Other = 2,
}
AvatarImageName = {
    [AvatarImageType.Main] = "TouXiangDiKuang",
    [AvatarImageType.Other] = "TouXiangKuang_1"
}
--角色头像控件
--=========================UIWAvatarCell=========================
UIWAvatarCell = UIWAvatarCell or DeclareMono("UIWAvatarCell")
--[[data =
    avatar_type   = 头像类型
    avatar_id     = 头像id 时装/进阶
    avatar_quality= 头像品质 飞升1 ~ 2
    top_level     = 巅峰头像框
    avatar_level  = 头像等级
    image_type    = 头像背景类型 可不传 
    role_id/uid   = 角色id 
]]
function UIWAvatarCell:SetData(data)
    data = CommonStruct.AvatarParam(data)
    if IS_AUDIT_VERSION then
        if not GLOBAL_CONFIG:PkgInfo().is_youyan then
            UH.SpriteName(self.AuditIconSp, "audithead_"..(AudutSeqIndex[GLOBAL_CONFIG:Urls().audit_seq] and GLOBAL_CONFIG:Urls().audit_seq or 1))
            self.IconObj:SetActive(false)
            self.AuditRawIconObj:SetActive(false)
            self.AuditIconObj:SetActive(true)
        else
            local path, streaming_has = ChannelAgentConsts.GetUIStremingAsset("Head"..data.avatar_type)
            if streaming_has then
                self.IconObj:SetActive(false)
                self.AuditRawIconObj:SetActive(true)
                self.AuditIconObj:SetActive(false)
                ChannelAgentConsts.SetRawImg(self.AuditRawIcon, path)
            else
                UH.SetAvatar(self.IconSp,data.avatar_type,data.avatar_id)
                self.IconObj:SetActive(true)
                self.AuditRawIconObj:SetActive(false)
                self.AuditIconObj:SetActive(false)
            end
        end
    else
        UH.SetAvatar(self.IconSp,data.avatar_type,data.avatar_id)
        self.IconObj:SetActive(true)
        self.AuditRawIconObj:SetActive(false)
        self.AuditIconObj:SetActive(false)
    end
    
    
    if data.avatar_level ~= 0 then
        UH.SetText(self.Level,data.top_level ~= 0 and ColorStr(data.top_level,COLORSTR.Red8) or data.avatar_level)
    end
    self.LevelBG:SetActive(data.avatar_level ~= 0)
    self.PeakLevel:SetActive(data.top_level ~= 0)
    if data.avatar_quality ~= 0 then
        UIWPetHeadCell.EffAnimName(data.avatar_quality,self.AnimTool)
    else
        UH.SetEnabled(self.PeakImg,true)
    end
    self.AnimTool:SetObjActive(data.avatar_quality ~= 0)
    UH.SetEnabled(self.PeakImg,data.avatar_quality == 0)
    if data.image_type ~= 0 then
        UH.SpriteName(self.IconImg,AvatarImageName[data.image_type])
    end
    self.role_id = data.role_id or data.uid 
end
function UIWAvatarCell:SetClickFunc(func)
    self.click_func = func
end
function UIWAvatarCell:OnClickItem()
    if self.click_func then
        self.click_func()
        return
    end
    if self.role_id then
        RoleInfoCtrl.Instance:Data():SetViewEquipUid(self.role_id)
        RoleInfoCtrl.Instance:SendReferRoleEquipment(self.role_id)
    end
end

function UIWAvatarCell:OnRelease()
    --UH.LocalScale(self.gameObject,Vector3.zero)
    UH.SpriteName(self.IconImg,AvatarImageName[AvatarImageType.Main])
    self.PeakLevel:SetActive(false)
    self.LevelBG:SetActive(false)
    self.AnimTool:SetObjActive(false)
    self.role_id = nil
    self.click_func = nil
end