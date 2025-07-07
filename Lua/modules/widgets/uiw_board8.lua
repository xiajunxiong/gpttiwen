------TitleImgSp====UIImageSpriteSet
UIWBoard8 = UIWBoard8 or DeclareMono("UIWBoard8")
UIWBoard8.TitleImg = {
    TiaoZhanFuBen = "teshudiban_3", -- 挑战副本
    TiaoZhanRenWu = "tiaozhanrenwu", -- 挑战任务
}
function UIWBoard8:UIWBoard8()

end

function UIWBoard8:OnClose()
    if self.view_type ~= nil then
        ViewMgr:CloseView(self.view_type)
    end
end

function UIWBoard8:SetData(view_type, title_img)
    self.view_type = view_type
    self.TitleImgSp.SpriteName = title_img
    self:SetDeco1(false)
end

function UIWBoard8:SetDeco1(value)
    self.Deco1Obj:SetActive(value)
end

function UIWBoard8:SetBg2(value)
    self.Bg2:SetObjActive(value)
end

function UIWBoard8:OnRelease()

end

