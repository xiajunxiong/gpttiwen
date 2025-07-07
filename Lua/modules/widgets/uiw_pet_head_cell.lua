
UIWPetHeadCell = UIWPetHeadCell or DeclareMono("UIWPetHeadCell")
--Icon=======Image
--Mutant=====GameObject
--Quality====UIImageSpriteSet
--QuaAnim====UISpriteAnimTool
--StrLv======UIImageSpriteSet
--StrLvTran==RectTransform
--RootObj=====GameObject
--LvObj=======GameObject
--Lv==========Text

--不在预置体里的序列化字段，可由替身代为传入
--StrPos=======UIWPetHeadCell.StrPosType(int)   默认Left
--ShowLv=======bool                         默认true

--强化标的位置
UIWPetHeadCell.StrPosType = {
    None = 0,   --不显示
    Left = 1,   --左下[default]
    Right = 2,  --右下
}


function UIWPetHeadCell:UIWPetHeadCell()

end

function UIWPetHeadCell.EffAnimName(fs_lv,anim_com)
    if fs_lv and fs_lv <= PetFeiShengData.Lv.None then
        anim_com:SetObjActive(false)
        return
    end
    anim_com:SetObjActive(true)
    if fs_lv == PetFeiShengData.Lv.Two then
        anim_com.AnimName = "fs_qua2_"
        anim_com.PicNum = 24
        anim_com.transform.sizeDelta = Vector2.New(165,165)
        anim_com.Speed = 0.1
        UH.LocalPosG(anim_com.gameObject,Vector3.New(-1,2,0))
    else
        anim_com.AnimName = "fs_qua1_"
        anim_com.PicNum = 12
        anim_com.transform.sizeDelta = Vector2.New(140,140)
        anim_com.Speed = 0.1
        UH.LocalPosG(anim_com.gameObject,Vector3.zero)
    end
end

function UIWPetHeadCell.EffAnimNameDiamond(fs_lv,anim_com)
    if fs_lv <= PetFeiShengData.Lv.None then
        anim_com:SetObjActive(false)
        return
    end
    anim_com:SetObjActive(true)
    if fs_lv == PetFeiShengData.Lv.Two then
        anim_com.AnimName = "fs_quap2_"
        anim_com.PicNum = 24
        anim_com.transform.sizeDelta = Vector2.New(160,160)
        anim_com.Speed = 0.1
    else
        anim_com.AnimName = "fs_quap1_"
        anim_com.PicNum = 12
        anim_com.transform.sizeDelta = Vector2.New(145,145)
        anim_com.Speed = 0.1
    end
end


-- data = {
--     pet_id,     --宠物id
--     lv,         --等级
--     fs_lv,      --飞升等级
--     str_lv,     --强化等级
--     skin_id,    --皮肤id
--     str_pos,    --强化标的位置
-- }
function UIWPetHeadCell:SetData(data)
    self.RootObj:SetActive(true)
    local pet_cfg = PetData.Instance:GetPetCfg(data.pet_id)
    if pet_cfg == nil then
        if IS_EDITOR then
            LogError("pet_id",data.pet_id,"pet_cfg is nil")
        else
            BuglyReport("pet_id",data.pet_id,"pet_cfg is nil")
        end
    end
    if data.skin_id ~= nil and data.skin_id ~= 0 then
        local skin_co = Cfg.PetSkinAttr(data.skin_id)
        UH.SetIcon(self.Icon, skin_co.head_portrait or 0)
    else
        UH.SetIcon(self.Icon, pet_cfg and pet_cfg.icon_id or 0)
    end

    -- local anim_name = UIWPetHeadCell.EffAnimName(data.fs_lv or PetFeiShengData.Lv.None)
    -- if StringIsEmpty(anim_name) then
    --     self.QuaAnim:SetObjActive(false)
    -- else
    --     self.QuaAnim:SetObjActive(true)
    --     self.QuaAnim.AnimName = anim_name
    -- end

    UIWPetHeadCell.EffAnimName(data.fs_lv or PetFeiShengData.Lv.None,self.QuaAnim)
    
    UH.SpriteName(self.Quality, PetData.PinZhi[pet_cfg and pet_cfg.quality or 1])

    if data.lv ~= nil then
        if self:isLvShow() then
            self.LvObj:SetActive(true)
            UH.SetText(self.Lv,tostring(data.lv))
        else
            self.LvObj:SetActive(false)
        end
    else
        self.LvObj:SetActive(false)
    end
    PetData.SetStrLevel(nil, self.StrLv, self:strPos() == UIWPetHeadCell.StrPosType.None and 0 or data.str_lv )
    self.Mutant:SetActive(pet_cfg and pet_cfg.mutant_times and pet_cfg.mutant_times >= 1)
    self.LianYao:SetActive(data.lian_yao)
    self:flushStrPos()
end

function UIWPetHeadCell:flushStrPos()
    local str_pos = self:strPos()
    -- LogError("strpos===",str_pos,self.StrPos)
    if str_pos == UIWPetHeadCell.StrPosType.Right then
        self.StrLvTran.anchorMin = Vector2.New(1,0)
        self.StrLvTran.anchorMax = Vector2.New(1,0)
        self.StrLvTran.anchoredPosition = Vector2.New(-21.15,20)
    elseif str_pos == UIWPetHeadCell.StrPosType.Left then
        self.StrLvTran.anchorMin = Vector2.New(0,0)
        self.StrLvTran.anchorMax = Vector2.New(0,0)
        self.StrLvTran.anchoredPosition = Vector2.New(21.15,20)
    end
end

function UIWPetHeadCell:strPos()
    return self.StrPos or UIWPetHeadCell.StrPosType.Left
end

function UIWPetHeadCell:isLvShow()
    if self.ShowLv == nil then 
        return true
    else
        return self.ShowLv
    end
end


function UIWPetHeadCell:SetDataByPet(pet)
    self:SetData({
        pet_id = pet:ID(),
        lv = pet:Level(),
        fs_lv = pet:FeiShengTimes(),
        str_lv = pet:StrLevel(),
        skin_id = pet:SkinId(),
        lian_yao = pet:IsLianYao(),
    })
end

function UIWPetHeadCell:Clean()
    self.RootObj:SetActive(false)
end