YouMingGaiZao = YouMingGaiZao or DeclareMono("YouMingGaiZao", UIWFlushPanel)
function YouMingGaiZao:YouMingGaiZao()
    self.data = YouMingData.Instance
    self.data_cares = {
        {data = self.data:GetSelectYouMing(), func = self.FlushBaseInfo}
    }
end
function YouMingGaiZao:FlushBaseInfo()
    self.youming = self.data:GetSelectYouMing()
    if self.youming.Aptitude == nil then
        return
    end
    self.Desc:SetActive(table.nums(self.youming:CurSkillList()) == 0)
    self.ZiZhi:SetData(self.youming:AptitudeList())
    self.List:SetData(self.youming:CurSkillList())
end

function YouMingGaiZao:OnClickGaiZao()
    ViewMgr:OpenView(YouMingRemodel)
end