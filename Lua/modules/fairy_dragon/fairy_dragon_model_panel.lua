-- 神龙模型信息1
FairyDragonModel = FairyDragonModel or DeclareMono("FairyDragonModel", UIWFlushPanel)
function FairyDragonModel:FairyDragonModel()
    self.data = FairyDragonData.Instance
    self.dragon = FairyDragon.Instance
    if self.ui_obj == nil then
		self.ui_obj = UIObjDrawer.New()
        self.ModelShow:SetShowData(
			{
				ui_obj = self.ui_obj,
                view = FairyDragonSkinPanel,
                show_shadow = true,
			}
		)
    end
    self.data_cares = {
        {data = self.dragon.base_data, func = self.FlushPanel, init_call = false},
        {data = self.dragon.show_skill_list, func = self.FlushSkill, init_call = false},
        {data = self.data.sel_tab_data,func = self.FlushShow}
    }
end

function FairyDragonModel:Awake()
    UIWFlushPanel.Awake(self)
    self:FlushPanel()
    self:FlushSkill()
end

function FairyDragonModel:OnDestroy()
    UIWFlushPanel.OnDestroy(self)
    if self.ui_obj ~= nil then
		self.ui_obj:DeleteMe()
	end
end

function FairyDragonModel:FlushPanel()
    self:FlushModel()
end

function FairyDragonModel:FlushShow()
    local tab = self.data:GetSelTab()
    self.HuanHuaBtn:SetActive(tab.open ~= Mod.FairyDragon.Star)
end

function FairyDragonModel:FlushModel()
    UH.SetText(self.Name, self.dragon:Name())
    local res_id = self.dragon:ResId()
    self.ui_obj:SetData(DrawerHelper.GetNpcPath(self.dragon:ResId()))
    self.Score:SetScore(self.dragon:Score())
    self.SkinOn:SetActive(self.dragon:IsSkin())
    self.SkinOff:SetActive(not self.dragon:IsSkin())
end

function FairyDragonModel:FlushSkill()
    local skill_list = self.dragon:GetShowSkillList()
    self.SkillList:SetData(skill_list or {})
end

function FairyDragonModel:OnSkinClick()
    if self.dragon:IsSkin() then
        FairyDragonCtrl.Instance:SendReq(FairyDragonCfg.REQ_TYPE.CANCEL_SKIN)
    else
        ViewMgr:OpenViewByKey(Mod.FairyDragon.Skin)
    end
end








-- 神龙模型信息2
FairyDragonModel2 = FairyDragonModel2 or DeclareMono("FairyDragonModel2", UIWFlushPanel)
function FairyDragonModel2:FairyDragonModel2()
    self.data = FairyDragonData.Instance
end