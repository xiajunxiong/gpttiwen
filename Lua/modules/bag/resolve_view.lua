ResolveView = ResolveView or DeclareView("ResolveView", "bag/resolve_view", Mod.Resolve.Main)
function ResolveView:ResolveView()
    -- body
end
function ResolveView:LoadCallback()
    self.Board:SetData(self:GetType(), Language.Resolve.Title)
end

function ResolveView:OnClickTips()
    PublicPopupCtrl.Instance:OpenTipsPopup({tips_str = Config.language_cfg_auto.textdesc[102].desc})
end

ResolvePanel = ResolvePanel or DeclareMono("ResolvePanel", UIWFlushPanel)
function ResolvePanel:ResolvePanel()
    self.data_cares = {
        {data = BagData.Instance.item_grid, func = self.onFlush, init_call = false}
    }

    self.EquipList:SetCompleteCallBack(function()
        if BagData.Instance:GetPreResolveItem() ~= nil then 
            for k,v in pairs(self.EquipList.item_list) do 
                if BagData.Instance:CheckSameEquipByUniqueId(v:GetData(),BagData.Instance:GetPreResolveItem()) then 
                    local with_score = BagData.Instance:CheckWithScore(v:GetData().item_id)
                    v:SetSelected(true)
                    v.Cell.MengBan:SetActive(true)
                    v.Cell.CellRankMenban:SetActive(with_score)

                    self:OnSelect()
                end 
            end 
        end 
    end)
end
function ResolvePanel:onFlush()
    self.select_data = nil
    local data = BagData.Instance:GetCanResolveItem()
    self.EquipList:SetData(data)
    self.List:SetData({})
    self.Single.gameObject:SetActive(false)
end
function ResolvePanel:OnSelect()
    local data = self.EquipList:GetSelectedData()
    --获取分解内容
    local resolve = WorkshopData.Instance:GetEquipResolveList(data)
    self.List:SetData(resolve)
    self.select_data = data

    self.Single.gameObject:SetActive(true)
    local single_data = Item.Create({item_id = CommonItemId.ImmortalCoin})
    self.Single:SetData(single_data)
    UH.SetText(self.SingleStr, single_data:QuaName().."："..(#resolve>0 and resolve[1]:Num() or 0))
    -- if #resolve>0 then 
        
    --     UH.SetText(self.SingleStr, resolve[1]:QuaName().."："..resolve[1]:Num())
    -- end 
end

function ResolvePanel:OnClickResolve()
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
                if value:HadStone() then self.with_stone = true end 
                if value:HasSpirit() then self.with_spirit = true end 
                value:UnStone()
                value:RemoveAllSpirit()
                table.insert(res, value.index)
            end
            local num = table.nums(res)
            WorkshopCtrl.Instance:CSWorkshopDecomposeReq({grid_num = num, decompose_grid = res})
            self.is_playing = false

            if self.with_stone then 
                PublicPopupCtrl.Instance:Center(Language.Resolve.StoneUnFix)
                self.with_stone = false
            end 
            if self.with_spirit then 
                PublicPopupCtrl.Instance:Center(Language.Resolve.SpiritUnFix)
                self.with_spirit = false
            end 
        end,
        2.2
    )
    self.block:SetActive(true)
end
function ResolvePanel:OnDestroy()
    UIWFlushPanel.OnDestroy(self)
    TimeHelper:CancelTimer(self.handle_delay)
    TimeHelper:CancelTimer(self.handle_delay1)
end
function ResolvePanel:OnEffectComplete()
    self.block:SetActive(false)
end

ResolveEquipItem = ResolveEquipItem or DeclareMono("ResolveEquipItem", UIWidgetBaseItem)
local rank_qua_sprite1 = {
    Rank3 = "cellRankS1",
    Rank2 = "cellRankA1",
    Rank1 = "cellRankB1",
    Rank0 = "cellRankC1"
}
function ResolveEquipItem:ResolveEquipItem()
    self.is_long_click = false
end
function ResolveEquipItem:OnRelease()
    UIWidgetBaseItem.OnRelease(self)
    TimeHelper:CancelTimer(self.time_func)
end
function ResolveEquipItem:SetData(data)
    TimeHelper:CancelTimer(self.handle_delay)
    TimeHelper:CancelTimer(self.time_func)
    UIWidgetBaseItem.SetData(self, data)
    UH.LocalScale(self.AnimRect, Vector3.one)
    self.Cell:SetData(data)
    self:SetSelected(false)
    UH.SpriteName(
        self.Cell.CellRankMenban:GetComponent(typeof(Game.UIImageSpriteSet)),
        rank_qua_sprite1["Rank" .. data.param.is_treasure]
    )
    self.Cell.MengBan.transform:SetAsLastSibling()
    self.Cell.CellRank.transform:SetAsLastSibling()
    self.Cell.CellRankMenban.transform:SetAsLastSibling()
    self:OnSelect(false)

    self.with_rank = BagData.Instance:CheckWithScore(data.item_id)
end
function ResolveEquipItem:OnSelect(selected)
    if not self.is_long_click then
        local total = self.List:GetSelectedData()
        if table.nums(total) > 20 then
            PublicPopupCtrl.Instance:Center(Language.Resolve.Limit)
            selected = false
        end
        if selected == true and WorkshopData.Instance:CheckEquipWithSmeltDone(self.data) then 
            local param_t = {
                content = Language.Resolve.SmeltLimit,
                confirm = {
                    func =  function()
                        ViewMgr:CloseView(DialogTipsView)
                        self:SetSelected(selected)
                        self.Cell.MengBan:SetActive(selected)
                        self.Cell.CellRankMenban:SetActive(selected and self.with_rank)

                        self.panel:OnSelect()
                    end 
                },
                cancel = {
                    func =  function()
                        ViewMgr:CloseView(DialogTipsView)
                        self:SetSelected(false)
                        self.Cell.MengBan:SetActive(false)
                        self.Cell.CellRankMenban:SetActive(false)

                        self.panel:OnSelect()
                    end 
                }
            }
    
            PublicPopupCtrl.Instance:DialogTips(param_t)
            return 
        elseif selected == true and self.data.IsNeveal~= nil and self.data:IsNeveal() then
            local param_t = {
                content = Language.Resolve.NevealLimit,
                confirm = {
                    func =  function()
                        ViewMgr:CloseView(DialogTipsView)
                        self:SetSelected(selected)
                        self.Cell.MengBan:SetActive(selected)
                        self.Cell.CellRankMenban:SetActive(selected and self.with_rank)

                        self.panel:OnSelect()
                    end 
                },
                cancel = {
                    func =  function()
                        ViewMgr:CloseView(DialogTipsView)
                        self:SetSelected(false)
                        self.Cell.MengBan:SetActive(false)
                        self.Cell.CellRankMenban:SetActive(false)

                        self.panel:OnSelect()
                    end 
                }
            }
    
            PublicPopupCtrl.Instance:DialogTips(param_t)
            return 
        end 

        self:SetSelected(selected)
        self.Cell.MengBan:SetActive(selected)
        self.Cell.CellRankMenban:SetActive(selected and self.with_rank)
    else
        -- CellClicks.BagGridNoButton(self.Cell)
        self:SetSelected(not selected)
    end
end
function ResolveEquipItem:OnPressUp()
    TimeHelper:CancelTimer(self.time_func)
    self.is_long_click = false
end

function ResolveEquipItem:PlayAnim()
    TimeHelper:CancelTimer(self.handle_delay)
    self.Anim:Play()
    self.handle_delay =
        TimeHelper:AddDelayTimer(
        function()
            if ViewMgr:IsOpen(ResolveView) then 
                self.EffectTool:Play(30000813)
            end 
        end,
        0.3336
    )
end
function ResolveEquipItem:OnDestroy()
    TimeHelper:CancelTimer(self.time_func)
end
function ResolveEquipItem:OnPressDown()
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
