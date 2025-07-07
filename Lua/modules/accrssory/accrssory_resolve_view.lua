--配饰 天命 分解弹窗
AccrssoryResolveView = AccrssoryResolveView or DeclareView("AccrssoryResolveView", "accrssory/accrssory_resolve")
function AccrssoryResolveView:AccrssoryResolveView()
    -- self.board
end
function AccrssoryResolveView:LoadCallback(param_t)
    self.Board:SetData(self:GetType(), Language.Accrssory.ResolveView, Vector2.New(486, 641))
    self.panel.lock_data = param_t.item
end

function AccrssoryResolveView:OnHelpClick()
    PublicPopupCtrl.Instance:HelpTip(Config.language_cfg_auto.textdesc[141].desc)
end

function AccrssoryResolveView:OnCloseClick()
    ViewMgr:CloseView(AccrssoryResolveView)
end


--------------------------AccrssoryResolvePanel--------------------------
AccrssoryResolvePanel = AccrssoryResolvePanel or DeclareMono("AccrssoryResolvePanel", UIWFlushPanel)
function AccrssoryResolvePanel:AccrssoryResolvePanel()
    self.data = AccrssoryData.Instance
    self.language = Language.Accrssory
    self.data_cares = {
        {data = BagData.Instance:GetColumnGrids(ItemColumnType.Accrssory), func = self.FlushResolve},
        {data = BagData.Instance:GetColumnGrids(ItemColumnType.Material), func = self.FlushResolve},
    }

    self.EquipList:SetCompleteCallBack(function() 
        if self.lock_data ~= nil then 
            for k,v in pairs(self.EquipList.item_list) do 
                if v:GetData() ~= nil and self.lock_data ~= nil and BagData.Instance:CheckSameEquipByUniqueId(v:GetData(),self.lock_data) then 
                    self.EquipList:SetectItem(k,true)

                    self:OnSelect()
                    self.lock_data = nil
                    
                    break
                end
            end 
        end 
    end)

    self.resolve_tips.gameObject:SetActive(true)
    UH.SetText(self.resolve_tips,Language.Accrssory.ResolveInit)
end

function AccrssoryResolvePanel:OnDestroy()
    UIWFlushPanel.OnDestroy(self)

    AccrssoryData.Instance:ReleaseMarkResolve()

    if self.time_func then 
        TimeHelper:CancelTimer(self.time_func)
    end 
end

function AccrssoryResolvePanel:onFlush()
    self:FlushResolve()    
end

function AccrssoryResolvePanel:OnSelect()
    local data = self.EquipList:GetSelectedData()
    --获取分解内容
    local exp_plus = AccrssoryData.Instance:GetAccrssoryResolveExp(data)

    -- self.List:SetData(resolve)

    local data = self.data:GetTianMingInfo()
    local cur_cfg,next_cfg = self.data:GetTianMingLevelCfg()
    local max_exp = self.data:IsMaxTianMing() and cur_cfg.exp or next_cfg.exp

    local next_level,res_exp = self.data:GetResolveLevelPlus(data.exp + exp_plus)
    local plus_cfg = self.data:GetTianMingCfgByLevel(next_level)
    -- self.Progress.gameObject:SetActive(not is_change)
    -- self.levelup:SetActive(is_change)
    -- if not is_change or self.data:IsMaxTianMing() then 
    self.Progress:SetProgress((data.exp + exp_plus) / max_exp)
    self.Progress:SetText(data.exp + exp_plus .." / "..max_exp)
    -- else
    -- self.Progress:SetProgress(res_exp / plus_cfg.exp)
    -- LogError("fsada",exp_plus,data.exp ,res_exp)
    UH.SetText(self.cur_level,cur_cfg == nil and self.language.NoneTian  or cur_cfg.tianming_name)
    UH.SetText(self.next_level,plus_cfg == nil and self.language.NoneTian  or plus_cfg.tianming_name)
    -- end 

    -- self.Progress
    self.select_data = data

    local num = self.data:GetStoneNumByList(self.EquipList:GetSelectedData())
    self.resolve_tips.gameObject:SetActive(num == 0)
    self.stone_tips.gameObject:SetActive(num > 0)
    UH.SetText(self.stone_num,"X"..num)
end


function AccrssoryResolvePanel:FlushResolve()
    self.select_data = nil
    local data = self.data:GetCanResolveAccrssoryItem()
    self.EquipList:SetData(data)

    self:OnSelect()

    self.empty:SetActive(#data == 0)

    
end

function AccrssoryResolvePanel:OnResolveClick()
    if self.select_data == nil then
        return
    end

    local select_items = self.EquipList:GetSelectedItem()
    for k,v in pairs(select_items) do 
        AccrssoryCtrl.Instance:SendAccrssoryReq({
            req_type = AccrssoryReqType.Resolve,
            param_1 = v:GetData():GridColumn(),
            param_2 = v:GetData().index,
            param_3 = v:GetData().item_id == CommonItemId.DestinyItem and v:GetData():Num() or 1,
        })
    end 

    if self.data.tianming_eff_mark and #select_items > 0 then 
        self.EffectTool:Play(4165093)
    end 
end

AccrssoryResolveItem = AccrssoryResolveItem or DeclareMono("AccrssoryResolveItem", UIWidgetBaseItem)
function AccrssoryResolveItem:AccrssoryResolveItem()
    self.is_long_click = false
end
function AccrssoryResolveItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    self.Cell:SetData(data)
    self:SetSelected(false)

    self:OnSelect(false)
end 

function AccrssoryResolveItem:OnSelect(selected)
    if not self.is_long_click then
        self:SetSelected(selected)
    else 
        self:SetSelected(not selected)
    end 
end 

function AccrssoryResolveItem:OnPressUp()
    TimeHelper:CancelTimer(self.time_func)
    self.is_long_click = false
end
function AccrssoryResolveItem:OnRelease()
    -- TimeHelper:CancelTimer(self.handle_delay)
end
function AccrssoryResolveItem:OnPressDown()
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