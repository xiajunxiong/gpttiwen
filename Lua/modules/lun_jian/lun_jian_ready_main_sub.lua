
LunJianReadyMainSub = LunJianReadyMainSub or DeclareMono("LunJianReadyMainSub", UIWFlushPanel)
--MyHead=========UIImageSpriteSet
--MyProf=========UIImageSpriteSet
--MyState========Text
--MyLv===========Text
--MyName=========Text
--OtherHead=========UIImageSpriteSet
--OtherProf=========UIImageSpriteSet
--OtherState========Text
--OtherLv===========Text
--OtherName=========Text

function LunJianReadyMainSub:LunJianReadyMainSub()
    self.data_cares = {
        {data = LunJianData.Instance:MyShowupInfo(), func = self.FlushMyShowupInfo},
        {data = LunJianData.Instance:OtherShowupInfo(), func = self.FlushOtherShowupInfo},
    }
end

function LunJianReadyMainSub:FlushMyShowupInfo()
    local my_showup_info = LunJianData.Instance:MyShowupInfo()
    self:flushShowupInfo(my_showup_info,"My")
end

function LunJianReadyMainSub:FlushOtherShowupInfo()
    local other_showup_info = LunJianData.Instance:OtherShowupInfo()
    self:flushShowupInfo(other_showup_info,"Other")
end

function LunJianReadyMainSub:flushShowupInfo(showup_info,name_pre)
    UH.SetAvatar(self[name_pre .. "Head"], showup_info.avatar_type)
    UH.SpriteName(self[name_pre .. "Prof"],RoleData.GetProfSp(showup_info.profession))
    UH.SetText(self[name_pre .. "State"],showup_info.is_show_up and 
        Language.LunJian.MainSub.InScene or Language.LunJian.MainSub.NoInScene)
    UH.SetText(self[name_pre .. "Lv"],tostring(showup_info.level))
    UH.SetText(self[name_pre .. "Name"],tostring(showup_info.name))
end

function LunJianReadyMainSub:OnClickExit()
    SceneCtrl.Instance:RequestLeaveFb()

    LogError("ONClick EXIT")
end