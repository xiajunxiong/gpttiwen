FabaoWash = FabaoWash or DeclareView("FabaoWash", "fabao/fabao_wash")
----Board=======UIWBoard
----Tabbar======UIWidgetList
function FabaoWash:FabaoWash()
    self.lang = Language.Fabao
    self.fabao_data = FabaoData.Instance
end
function FabaoWash:LoadCallback()
    self.Board:SetData(self:GetType(), self.lang.Title2, Vector2.New(672, 536))
    -- self.Board:SetTabbarVisible(true)
    --self.Currency:DefaultSet()
end

function FabaoWash:OnFlush()
    -- body
end
function FabaoWash:CloseCallback()
end

-- 获取当前选择法宝 判断阶数 当前属性
FabaoWashPanel = FabaoWashPanel or DeclareMono("FabaoWashPanel", UIWFlushPanel)
function FabaoWashPanel:FabaoWashPanel()
    self.fabao_data = FabaoData.Instance
    self.lang = Language.Fabao
    self.data_cares = {
        {data = self.fabao_data:ViewData(), func = self.FlushView, keys = {"flush"}}
        -- {data = self.fabao_data:GetFabaoList(), func = self.FlushView},
    }
end
function FabaoWashPanel:FlushView()
    self.data = self.fabao_data:ViewData().select_data
    self.fabao_attr = self.fabao_data:FabaoAttrInfo(self.data.id)
    self.jie_level = self.data.info.fabao_jie
    self.fabao_effect = self.data.info.fabao_effect
    for i = 1, self.JieList:Length() do
        self.JieList[i]:SetActive(self.fabao_effect[i] > -1)
        self.LockList[i]:SetActive(self.fabao_effect[i] == -1)
    end
    local item_id = self.data["param_" .. self.jie_level]
    self.cost_item = Item.Create({item_id = item_id})
    self.JieCell:SetData(self.cost_item)
    UH.SetText(self.CostName, self.cost_item:QuaName())
    UH.SetText(self.NextEffect, string.format(self.lang.NextEffect, self.fabao_attr.addition_desc))
    self:OnClickJie(self.select_jie or 1)
end

function FabaoWashPanel:OnClickJie(index)
    self.select_jie = index
    if self.fabao_effect[index] == -1 then
        return
    end
    for i = 1, self.SelectList:Length() do
        UH.SpriteName(self.SelectList[i], i == index and "fabao_di1" or "fabao_di2")
        if i < 4 then
            UH.SpriteName(self.PointList[i], i == index and "fabao_di1_1" or "fabao_di2_1")
            if self.PointList[index - 1] then
                UH.SpriteName(self.PointList[index - 1], "fabao_di1_1")
            end
        end
    end
    local jie_attr =
        string.format(
        self.fabao_attr.desc_2,
        self.data.type == 1 and self.data.info.rand_effect_param[index] / 1000 or
            self.data.info.rand_effect_param[index]
    )
    UH.SetText(self.CurEffect, string.format(self.lang.CurEffect, jie_attr))
    UH.SetText(
        self.NextEffect,
        index == 4 and self.lang.NoWash or string.format(self.lang.NextEffect, self.fabao_attr.addition_desc)
    )
    UH.SetText(self.CostNum, string.format("%d/%d", self.cost_item:Num(), self.data["param_num_" .. index]))
    UH.Color(self.CostNum, self.data["param_num_" .. index] > self.cost_item:Num() and COLORS.Red8 or COLORS.Green7)
    self.BtnInter.Interactable = index ~= 4
end

function FabaoWashPanel:OnClickWash()
    local data = self.fabao_data:ViewData().select_data
    local param_t = {}
    param_t.req_type = FABAO_REQ_TYPE.RAND
    param_t.param1 = data.id
    param_t.param2 = self.select_jie - 1
    FabaoCtrl.Instance:SendFabaoReq(param_t)
end
