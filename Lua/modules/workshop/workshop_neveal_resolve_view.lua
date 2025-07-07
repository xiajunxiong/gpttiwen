WorkshopNevealResolveView = WorkshopNevealResolveView or DeclareView("WorkshopNevealResolveView", "workshop/workshop_neveal_resolve_view")
function WorkshopNevealResolveView:WorkshopNevealResolveView()
    -- body
end
function WorkshopNevealResolveView:LoadCallback()
    self.Board:SetData(self:GetType(), Language.Workshop.Neveal.Resolve)
end

function WorkshopNevealResolveView:OnClickTips()
    PublicPopupCtrl.Instance:OpenTipsPopup({tips_str = Config.language_cfg_auto.textdesc[184].desc})
end

WorkshopNevealResolvePanel = WorkshopNevealResolvePanel or DeclareMono("WorkshopNevealResolvePanel", UIWFlushPanel)
function WorkshopNevealResolvePanel:WorkshopNevealResolvePanel()
    self.data_cares = {
        {data = BagData.Instance.item_grid, func = self.onFlush, init_call = false}
    }

    self.EquipList:SetCompleteCallBack(function()
        if BagData.Instance:GetPreNevealResolveItem() ~= nil then 
            for k,v in pairs(self.EquipList.item_list) do 
                if BagData.Instance:CheckSameEquipByUniqueId(v:GetData(),BagData.Instance:GetPreNevealResolveItem()) then 
                    v:SetSelected(true)
                    v.Cell.MengBan:SetActive(true)

                    self:OnSelect()
                end 
            end 
        end 
    end)
end

function WorkshopNevealResolvePanel:onFlush()

    self.select_data = nil
    local data = BagData.Instance:GetCanResolveNeveal()
    self.EquipList:SetData(data)
    self.List:SetData({})
    self.Single.gameObject:SetActive(false)
end 
function WorkshopNevealResolvePanel:OnSelect()
    local data = self.EquipList:GetSelectedData()
    --获取分解内容
    local resolve = WorkshopData.Instance:GetNevealResolveList(data)
    self.List:SetData(resolve)
    self.select_data = data

    self.Single.gameObject:SetActive(true)
    local single_data = Item.Create({item_id = Config.equipment_dianhua_auto.other[1].fumo_id})
    self.Single:SetData(single_data)
    UH.SetText(self.SingleStr, single_data:QuaName().."："..(#resolve>0 and resolve[1]:Num() or 0))

end 

function WorkshopNevealResolvePanel:OnClickResolve()
    if self.select_data == nil then
        return
    end
    if self.is_playing == true then
        return
    end
    self.is_playing = true
    local select_items = self.EquipList:GetSelectedItem()
    --做个动画做完再发协议
    for i, v in ipairs(select_items) do
        v:PlayAnim()
    end
    self.handle_delay =
        TimeHelper:AddDelayTimer(
        function()
            self.EffectTool:Play(30000825)
        end,
        0.4
    )
    self.handle_delay1 =
        TimeHelper:AddDelayTimer(
        function()
            local res = {}
            for _, value in ipairs(self.select_data or {}) do
                -- flag, cfg = value:CanResolve()
                -- WorkshopCtrl.Instance:SendOperateReq(WORLSHOP_OP.WORKSHOP_OP_DECOMPOSE, cfg.seq, value.index)
                -- if value:HadStone() then self.with_stone = true end 
                -- if value:HasSpirit() then self.with_spirit = true end 
                -- value:UnStone()
                -- value:RemoveAllSpirit()
                -- table.insert(res, value.index)
                WorkshopCtrl.Instance:SendOperateReqWithParamt({
                    op_type = WORLSHOP_OP.WORKSHOP_OP_DISCOMPOSE,
                    param1 = value:GridColumn(),
                    param2 = value:GridIndex(),
                })
            end
            -- local num = table.nums(res)
            -- WorkshopCtrl.Instance:CSWorkshopDecomposeReq({grid_num = num, decompose_grid = res})
            self.is_playing = false

            -- if self.with_stone then 
            --     PublicPopupCtrl.Instance:Center(Language.Resolve.StoneUnFix)
            --     self.with_stone = false
            -- end 
            -- if self.with_spirit then 
            --     PublicPopupCtrl.Instance:Center(Language.Resolve.SpiritUnFix)
            --     self.with_spirit = false
            -- end 
        end,
        2.2
    )
end
function WorkshopNevealResolvePanel:OnDestroy()
    UIWFlushPanel.OnDestroy(self)
    TimeHelper:CancelTimer(self.handle_delay)
    TimeHelper:CancelTimer(self.handle_delay1)
end
ResolveNevealItem = ResolveNevealItem or DeclareMono("ResolveNevealItem", UIWidgetBaseItem)
function ResolveNevealItem:ResolveNevealItem()
    self.is_long_click = false
end
function ResolveNevealItem:OnRelease()
    UIWidgetBaseItem.OnRelease(self)
    TimeHelper:CancelTimer(self.time_func)
end
function ResolveNevealItem:SetData(data)
    TimeHelper:CancelTimer(self.handle_delay)
    TimeHelper:CancelTimer(self.time_func)
    UIWidgetBaseItem.SetData(self, data)
    UH.LocalScale(self.AnimRect, Vector3.one)
    self.Cell:SetData(data)
    self:SetSelected(false)
    -- UH.SpriteName(
    --     self.Cell.CellRankMenban:GetComponent(typeof(Game.UIImageSpriteSet)),
    --     rank_qua_sprite1["Rank" .. data.param.is_treasure]
    -- )
    self.Cell.MengBan.transform:SetAsLastSibling()
    -- self.Cell.CellRank.transform:SetAsLastSibling()
    -- self.Cell.CellRankMenban.transform:SetAsLastSibling()
    self:OnSelect(false)
end
function ResolveNevealItem:OnSelect(selected)
    if not self.is_long_click then
        local total = self.List:GetSelectedData()
        if table.nums(total) > 20 then
            PublicPopupCtrl.Instance:Center(Language.Resolve.Limit)
            selected = false
        end
        -- if selected == true and WorkshopData.Instance:CheckEquipWithSmeltDone(self.data) then 
        --     local param_t = {
        --         content = Language.Resolve.SmeltLimit,
        --         confirm = {
        --             func =  function()
        --                 ViewMgr:CloseView(DialogTipsView)
        --                 self:SetSelected(selected)
        --                 self.Cell.MengBan:SetActive(selected)
        --                 self.Cell.CellRankMenban:SetActive(selected)

        --                 self.panel:OnSelect()
        --             end 
        --         },
        --         cancel = {
        --             func =  function()
        --                 ViewMgr:CloseView(DialogTipsView)
        --                 self:SetSelected(false)
        --                 self.Cell.MengBan:SetActive(false)
        --                 self.Cell.CellRankMenban:SetActive(false)

        --                 self.panel:OnSelect()
        --             end 
        --         }
        --     }
    
        --     PublicPopupCtrl.Instance:DialogTips(param_t)
        --     return 
        -- end 

        self:SetSelected(selected)
        self.Cell.MengBan:SetActive(selected)
        -- self.Cell.CellRankMenban:SetActive(selected)
    else
        -- CellClicks.BagGridNoButton(self.Cell)
        self:SetSelected(not selected)
    end
end
function ResolveNevealItem:OnPressUp()
    TimeHelper:CancelTimer(self.time_func)
    self.is_long_click = false
end
function ResolveNevealItem:OnRelease()
    TimeHelper:CancelTimer(self.handle_delay)
end
function ResolveNevealItem:PlayAnim()
    TimeHelper:CancelTimer(self.handle_delay)
    self.Anim:Play()
    self.handle_delay =
        TimeHelper:AddDelayTimer(
        function()
            self.EffectTool:Play(30000813)
        end,
        0.3336
    )
end
function ResolveNevealItem:OnPressDown()
    self.is_long_click = false
    TimeHelper:CancelTimer(self.time_func)
    local time = 0
    self.time_func =
        TimeHelper:AddRunTimer(
        function()
            time = time + 0.2
            if time > 1 then
                self.is_long_click = true
                CellClicks.BagGridNoButton(self.Cell)
            end
        end,
        0.2
    )
end
