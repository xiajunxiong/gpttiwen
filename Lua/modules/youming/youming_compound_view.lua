-- 幽冥合成
YouMingCompoundView = YouMingCompoundView or DeclareView("YouMingCompoundView", "youming/youming_compound_view", Mod.YouMing.Compound)
function YouMingCompoundView:YouMingCompoundView()
    self.Board:SetData(self:GetType(),Language.YouMing.Title3,Vector2.New(1018, 568))
end


YouMingCompoundPanel = YouMingCompoundPanel or DeclareMono("YouMingCompoundPanel", UIWFlushPanel)
function YouMingCompoundPanel:YouMingCompoundPanel()
    self.data_cares = {
        {data = BagData.Instance.item_grid, func = self.FlushPanel, init_call = false}
    }
end

function YouMingCompoundPanel:Awake()
    UIWFlushPanel.Awake(self)
    self:FlushPanel()
end

function YouMingCompoundPanel:FlushPanel()
    local list_data = self:GetListData()
    self.List:SetData(list_data)
end

function YouMingCompoundPanel:GetListData()
    return Config.dim_pet_cfg_auto.compose
end

function YouMingCompoundPanel:OnAllCompoundBtnClick()
    local list_data = self:GetListData()
    local list = {}
    for i,v in pairs(list_data) do
        local has_num = Item.GetNum(v.compose_consume)
        if has_num >= v.compose_consume_num then
            --YouMingCtrl.Instance:SendReq(YouMingCfg.ReqType.COMPOSE, v.compose_consume)
            --table.insert(list, v.compose_consume)
            table.insert(list, v.id)
        end
    end

    YouMingCtrl.Instance:SendCompoundList(list)
end



YouMingCompoundItem = YouMingCompoundItem or DeclareMono("YouMingCompoundItem", UIWidgetBaseItem)
function YouMingCompoundItem:SetData(data)
    UIWidgetBaseItem.SetData(self,data)
    local item = Item.Create({item_id = data.compose_consume})
    self.ItemCell:SetData(item)
    local youming_cfg = YouMingData.Instance:GetYouMingCfg(data.id)
    UH.SetText(self.Name, youming_cfg.name)
    local has_num = Item.GetNum(data.compose_consume)
    UH.SetText(self.Count, DataHelper.ConsumeNum(has_num, data.compose_consume_num, true))
    UH.SetInter(self.Inter, has_num >= data.compose_consume_num)
end

function YouMingCompoundItem:OnCompoundClick()
    local has_num = Item.GetNum(self.data.compose_consume)
    if has_num < self.data.compose_consume_num then
        MainOtherCtrl.Instance:GetWayViewById(self.data.id)
    else
        YouMingCtrl.Instance:SendReq(YouMingCfg.ReqType.COMPOSE, self.data.id)
    end
end



YouMingGetShowView = YouMingGetShowView or DeclareView("YouMingGetShowView", "youming/youming_get_show_view")
function YouMingGetShowView:YouMingGetShowView()
    
end

function YouMingGetShowView:LoadCallback(param)
    local list = param.list or {}
    self.YouMingList:SetData(list)
end

function YouMingGetShowView:OnCloseClick()
    ViewMgr:CloseView(YouMingGetShowView)
end


-- 新版幽冥获得展示
YouMingShowView = YouMingShowView or DeclareView("YouMingShowView", "youming/youming_show_view")
VIEW_DECLARE_LEVEL(YouMingShowView, ViewLevel.L0)
--VIEW_DECLARE_MASK(YouMingShowView,ViewMask.BgBlockClose)
function YouMingShowView:YouMingShowView()
    if self.ui_obj == nil then
        self.ui_obj = UIObjDrawer.New()
        self.ModelShow:SetShowData({ui_obj = self.ui_obj,view = YouMingPanel})
    end
end

function YouMingShowView:OnCloseClick()
    ViewMgr:CloseView(YouMingShowView)
end

function YouMingShowView:LoadCallback(param_t)
    local youming = param_t.youming or self:GetShowYouMing(param_t.list)
    if not youming then
        return
    end

    UH.SetText(self.Name, youming:Name())
    self.ui_obj:SetData(youming:ResPath())
    self.TianZi:SetData(youming:AptitudeList())
    local aptitude, max_aptitude = youming:Aptitude()
    UH.SetText(self.TianZiText, string.format("%s/%s", aptitude, max_aptitude))
    UH.SetText(self.Level, "Lv." .. youming:Level())
    UH.SetText(self.Score, youming:Score())
    local awake_level = youming:AwakeLevel()
    UH.SpriteName(self.AwakeImg, "PetNum" .. awake_level)
    local skill_list = youming:SkillList()
    self.SkillList:SetData(skill_list)
end

-- 从一个列表提出最好的幽冥显示 排序条件：品质 id
function YouMingShowView:GetShowYouMing(list)
    if not list then
        return
    end
    table.sort(list, function (a,b)
        if a:Qua() == b:Qua() then
            return a.id > b.id
        end
        return a:Qua() > b:Qua()
    end)
    return list[1]
end

function YouMingShowView:CloseCallback()
    if self.ui_obj ~= nil then
        self.ui_obj:DeleteMe()
        self.ui_obj = nil
	end
    YouMingData.Instance:PopDelayPetShow()
end