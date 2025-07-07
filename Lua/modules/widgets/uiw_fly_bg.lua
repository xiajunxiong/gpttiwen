UIWFlyBg = UIWFlyBg or DeclareMono("UIWFlyBg")
UIWFlyBg.RoleFlyQ = {[1] = "QianSeYiCiFeiSheng", [2] = "QianSeErCiFeiSheng"}
UIWFlyBg.RoleFlyS = {[1] = "ShenSeYiCiFeiSheng", [2] = "ShenSeErCiFeiSheng"}
--需要镜像图片时改Sub的scale x = -1
function UIWFlyBg:UIWFlyBg() end


--[[ function UIWFlyBg:Clean()
    self.Bg:SetActive(false)
end ]]
-- 功能1根据飞升次数设置图片
-- 图片需要调节透明度 Alpha
-- 图片需要调节大小 Scale
-- 深色还是浅色 Deep
function UIWFlyBg:SetData(data)
    local fly_flag = data.fly_flag or 0
    self.Bg:SetActive(fly_flag > 0)
    UH.SpriteName(self.BgSet, self.Deep and UIWFlyBg.RoleFlyS[fly_flag] or
                      UIWFlyBg.RoleFlyQ[fly_flag])
    UH.Alpha(self.BgImg, tonumber(self.Alpha))
    UH.LocalScale(self.BgRect, Vector3.New(tonumber(self.Scale),
                                           tonumber(self.Scale),
                                           tonumber(self.Scale)))
end
