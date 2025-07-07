-- 符石合成
FuShiFusionView = FuShiFusionView or DeclareView("FuShiFusionView", "sheng_qi/fu_shi_fusion_view", Mod.ShengQi.FuShiFusion)
function FuShiFusionView:FuShiFusionView()
    self.Board:SetData(self:GetType(),Language.ShengQi.Title2,Vector2.New(493, 680))
end


FuShiFusionPanel = FuShiFusionPanel or DeclareMono("FuShiFusionPanel", UIWFlushPanel)
function FuShiFusionPanel:FuShiFusionPanel()
    self.data = FuShiData.Instance
    self.data_cares = {
        {data = self.data.sel_fusion_fushi_list, func = self.FlushPanel, init_call = false},
        {data = self.data.fu_shi_list, func = self.FlushPanel, init_call = false}
    }
end

function FuShiFusionPanel:Awake()
    UIWFlushPanel.Awake(self)
    self:FlushPanel()
    self.move_effect_list = {}
    self.move_effect_start_post_list = {}
    for i = 1, 5 do
        self.move_effect_list[i] = ShengQiMoveEffect.New()
        self.move_effect_start_post_list[i] = UH.GetRootPosition(self.EffectList[i])
    end
end

function FuShiFusionPanel:OnDestroy()
    UIWFlushPanel.OnDestroy(self)
    self.data.sel_fusion_fushi_list:Set({list = {}})
    for i,v in pairs(self.move_effect_list) do
        v:Destroy()
    end
end

function FuShiFusionPanel:FlushPanel()
    local preview_fushi,coin = self.data:GetFusionPreview()
    UH.SetText(self.CoinNum, coin)
    self.Cell:SetData(preview_fushi)
    self.preview_fushi = preview_fushi
    self.Inter.Interactable = preview_fushi.item_id ~= 0
    self.LevelObj:SetActive(preview_fushi.item_id ~= 0)
    if preview_fushi.item_id ~= 0 then
        local fs = self.data.sel_fusion_fushi_list.list[1]
        UH.SetText(self.Level, string.format(Language.ShengQi.Level2,fs:Level() + 1))
    end

    local all_fushi = self.data:GetBagShowList()

    local fushi_list = {}
    for i,v in pairs(all_fushi) do
        if v:Color() <= 5 then
            table.insert(fushi_list, v)
        end
    end

    if #fushi_list < 15 then
        for i = #fushi_list + 1, 15 do
            table.insert(fushi_list,Item.Create({item_id = 0}))
        end
    end
    self.BagList:SetData(fushi_list)

    local stuff_list = {}
    for i = 1,5 do
        local item = self.data.sel_fusion_fushi_list.list[i] or FuShi.New({item_id = 0})
        table.insert(stuff_list, item)
    end
    self.stuff_list = stuff_list
    self.StuffList:SetData(stuff_list,item)
end

function FuShiFusionPanel:OnFusionClick()
    if self.effect_ing then
        return
    end
    if TableIsEmpty(self.preview_fushi) or self.preview_fushi.item_id == 0 then
        return
    end
    local t_level,t_num = self.data:FusionContion()
    local cfg = self.data:GetFusionCfg(t_level)
    if MallCtrl.IsNotCurrency(CurrencyType.CoinBind, cfg.cost) then
        local p1 = self.stuff_list[1].index
        local p2 = self.stuff_list[2].index
        local p3 = self.stuff_list[3].index
        local p4 = self.stuff_list[4].index
        local p5 = self.stuff_list[5].index

        self.effect_ing = true
        local end_pos = UH.GetRootPosition(self.Cell.gameObject)
        for i = 1, 5 do
            local pos_list = {}
            pos_list[1] = self.move_effect_start_post_list[i]
            pos_list[2] = end_pos
            UH.LocalPosG(self.EffectList[i], pos_list[1])
            self.EffectList[i]:SetActive(true)
            local v = self.move_effect_list[i]
            if i == 5 then
                v:Play(pos_list,self.EffectList[i],function ()
                    self.EffectList[i]:SetActive(false)
                    self.EffectTool:Play("3165168")
                    ShengQiCtrl.Instance:SendCSHolyEquipOpReq(ShengQiCtrl.OpType.OP_TYPE_FU_SHI_FUSION,t_level + 1,p1,p2,p3,p4,p5)
                    self.data.sel_fusion_fushi_list:Set({list = {}})  
                    self:FlushPanel()
                    self.effect_ing = false
                end,10)
            else
                v:Play(pos_list,self.EffectList[i],function ()
                    self.EffectList[i]:SetActive(false)
                end,10)
            end
        end
    end
end


FuShiFusionStuffItem = FuShiFusionStuffItem or DeclareMono("FuShiFusionStuffItem", UIWidgetBaseItem)
function FuShiFusionStuffItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    self.Cell:SetData(data)
    self.RemoveBtn:SetActive(data.item_id ~= 0)
end

function FuShiFusionStuffItem:OnRemoveClick()
    FuShiData.Instance:AddFusionStuff(self.data, -1)
end


FuShiFusionItem = FuShiFusionItem or DeclareMono("FuShiFusionItem", UIWidgetBaseItem)
function FuShiFusionItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    self.is_can = false
    self.Cell:SetData(data)
    self.Cell:MakeGrey(false)
    self.SelImg:SetActive(false)
    if data.item_id ~= 0 then
        self.is_can = FuShiData.Instance:IsCanFusion(data) == 1
        local is_sel = FuShiData.Instance:IsInFusion(data)
        self.Cell:MakeGrey(not self.is_can and not is_sel)
        self.SelImg:SetActive(is_sel)
        if is_sel then
            self.is_can = false
        end
    end
end

function FuShiFusionItem:OnItemClick()
    if self.is_can then
        FuShiData.Instance:AddFusionStuff(self.data, 1)
    end
end