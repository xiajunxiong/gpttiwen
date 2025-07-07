YouMingBase = YouMingBase or DeclareMono("YouMingBase", UIWFlushPanel)
function YouMingBase:YouMingBase()
    self.data = YouMingData.Instance
    self.data_cares = {
        {data = self.data:GetSelectYouMing(), func = self.FlushBaseInfo}
    }
end
function YouMingBase:onFlush()
    
end
function YouMingBase:FlushBaseInfo()

    -- local config = Config.dim_pet_cfg_auto.dim_pet_base[62500]
    -- self.youming = YouMing.New(config)

    self.youming = self.data:GetSelectYouMing()
    if self.youming.Aptitude == nil then
        return
    end
    self.Cell:SetData(self.youming)
    UH.SetText(self.PosTex, Format(Language.YouMingPractice.Pos, self.youming:Pos()))
    UH.SetText(self.Name, self.youming:Name())
    UH.SetText(self.Level, Format(Language.YouMingPractice.Level, self.youming:Level()))
    local maxhp = self.youming:MaxHp()
    local maxmp = self.youming:MaxMp()
    UH.SetText(self.Hp, Format("%s/%s", Format(Language.Common.Xstr,COLORSTR.Blue12,self.youming:CurHp()), Format(Language.Common.Xstr,COLORSTR.Blue13,maxhp)))
    UH.SetText(self.Mp, Format("%s/%s", Format(Language.Common.Xstr,COLORSTR.Blue12,self.youming:CurMp()), Format(Language.Common.Xstr,COLORSTR.Blue13,maxmp)))
    if maxhp == 0 then
        self.HpBar:SetProgress(0)
        self.MpBar:SetProgress(0)
    else
        self.HpBar:SetProgress(self.youming:CurHp() / self.youming:MaxHp())
        self.MpBar:SetProgress(self.youming:CurMp() / self.youming:MaxMp())
    end
    self.ZiZhi:SetData(self.youming:AptitudeList())
    -- 显示基础属性
    local base_attr = self.youming:BaseAttrPoint()
    for i = 1, 5, 1 do
        UH.SetText(self.BaseAttrs[i], base_attr[i].attr_value)
    end
    self.SkillList:SetData(self.youming:SkillList())
    self.PetInfo:SetData(self.youming)
end
function YouMingBase:OnClickDetail()
    ViewMgr:OpenView(YouMingAttrDetail)
end
