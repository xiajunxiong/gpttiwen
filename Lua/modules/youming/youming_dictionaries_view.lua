YouMingDictionariesView = YouMingDictionariesView or DeclareView("YouMingDictionariesView", "youming/youming_dictionaries_view")
function YouMingDictionariesView:YouMingDictionariesView()
    self.Board:SetData(self:GetType(),Language.YouMing.Title4,Vector2.New(1262, 650))
end


-----------------------------------
YouMingDictionariesPanel = YouMingDictionariesPanel or DeclareMono("YouMingDictionariesPanel", UIWFlushPanel)
function YouMingDictionariesPanel:YouMingDictionariesPanel()
    if self.ui_obj == nil then
        self.ui_obj = UIObjDrawer.New()
        self.ModelShow:SetShowData({ui_obj = self.ui_obj,view = self})
    end
    self.data_cares = {
        {data = IllustrateData.Instance.included_data, func = self.FlushPanel, init_call = false}
    }
end

function YouMingDictionariesPanel:Awake()
    UIWFlushPanel.Awake(self)
    self:FlushPanel()
    self.List:SetectItem(1,true)
end

function YouMingDictionariesPanel:FlushPanel()
    local list_data = self:GetListData()
    self.List:SetData(list_data)
end

function YouMingDictionariesPanel:GetListData()
    local list = {}
    for i,v in pairs(Config.dim_pet_cfg_auto.dim_pet_base) do
        local youming = YouMing.New({id = v.id})
        table.insert(list, youming)
    end
    table.sort(list, function (a,b)
        return a.id < b.id
    end)
    return list
end

function YouMingDictionariesPanel:OnDestroy()
    UIWFlushPanel.OnDestroy(self)
    if self.ui_obj ~= nil then
        self.ui_obj:DeleteMe()
        self.ui_obj = nil
	end
end

function YouMingDictionariesPanel:FlushInfo()
    if not self.youming then
        return
    end
    local youming = self.youming
    local cfg = self.youming:Cfg()
    -- local tianzi_num, max_tianzi_num = youming:Aptitude()
    -- UH.SetText(self.TianZiNum, string.format("%s/%s", tianzi_num, max_tianzi_num))
    UH.SetText(self.TianZiNum, youming:MaxAptitude())
    UH.SetText(self.ScoreNum, youming:Score())
    self.NameInfo:SetData(youming)
    self.ui_obj:SetData(youming:ResPath())
    local skill_list = youming:SkillList()
    self.SkillList:SetData(skill_list)
    UH.SetText(self.Name, self.youming:Name())
    self.ZiZhi:SetData(self.youming:AptitudeList())
    self.SealBtn:SetActive(cfg.get_type == 1) 
    self.SealBtn2:SetActive(cfg.get_type == 2)
    self.SealTipObj:SetActive(cfg.hunt_level ~= 0)
    if cfg.hunt_level ~= 0 then
        UH.SetText(self.SealTip, string.format(Language.YouMing.SealTip, cfg.hunt_level))
    end
end

function YouMingDictionariesPanel:OnSelectClick(data)
    self.youming = data
    self:FlushInfo()
end

function YouMingDictionariesPanel:OnTianZiTipClick()
    PublicPopupCtrl.Instance:HelpTip(YouMingCfg.TIANZI_ID)
end

function YouMingDictionariesPanel:OnClickSeal()
    local id = self.youming and self.youming.id or nil
    local scene_id = YouMingData.Instance:GetSealSceneId(id)
    local cb = function ()
        ViewMgr:CloseView(YouMingDictionariesView)
        ViewMgr:CloseView(YouMingView)
    end

    YouMingCtrl.Instance:GoToSealScene(scene_id, cb)
end

function YouMingDictionariesPanel:OnClickSeal2()
    
    local get_way_info = {}
    local get_way_t = Split(self.youming:Cfg().catch_way,"|")
    for k,v in pairs(get_way_t) do
        table.insert(get_way_info, BagData.Instance:GetWayCfg(tonumber(v)))
    end
    ViewMgr:OpenView(PetGuideTipView, {list = get_way_info, tip = Language.YouMing.Text2})
end



YouMingDictionariesItem = YouMingDictionariesItem or DeclareMono("YouMingDictionariesItem", UIWidgetBaseItem)
-- data == youming
function YouMingDictionariesItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    self.ItemCell:SetData(data)
    self.ItemCell:MakeGrey(not YouMingData.Instance:IsActive(data.id))
end