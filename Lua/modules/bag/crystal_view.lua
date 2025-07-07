CrystalView = CrystalView or DeclareView("CrystalView", "bag/crystal")

function CrystalView:CrystalView()
    self.AudioOpen = CommonAudio.ViewOpenL2
    self.AudioClose = CommonAudio.ViewCloseL1
    self.Board:SetData(self:GetType(), Language.EleCrystal.Title, Vector2.New(426, 536))
end

function CrystalView:LoadCallback(param_t)
    self.crystal_func = param_t.crystal_func
end

function CrystalView:OnFlush()
    self:FlushCrystalView()
end

function CrystalView:FlushCrystalView()
    local data = self.crystal_func()
    -- self.None:SetActive(table.nums(data) == 0)
    self.CryList:SetDataList(data)
end

function CrystalView:OnClickUse()
    if MainViewData.Instance:IsBattle(true) then
        return
    end
    if self.item == nil then
        PublicPopupCtrl.Instance:Center(ErrorText[17])
        return
    end
    if ViewMgr:IsOpen(AnotherView) then
        if self.item.index == -1 then
            local param_t = AnotherEquipmentUnFixParam[ItemType.EleCrystal](self.item)
            local unfix_fuc = AnotherItemInfoUseFunc[ItemType.EleCrystal]
            if unfix_fuc then
                unfix_fuc(param_t)
            end
            ViewMgr:CloseView(CrystalView)
            return
        end
        if self.item:Type() == ItemType.EleCrystal then
            local param_t = AnotherItemUseParam[ItemType.EleCrystal](self.item)
            local put_fuc = AnotherItemInfoUseFunc[ItemType.EleCrystal]
            if put_fuc then
                put_fuc(param_t)
            end
        end
        self.item = nil
        ViewMgr:CloseView(CrystalView)
    else
        if self.item.index == -1 then
            local param_t = {}
            param_t.oper_type = EQUIP_OPERA_TYPE.OPERA_TYPE_TAKEOFF_CRYSTAL
            BagCtrl.Instance:SendEquipmentOperaReq(param_t)
            ViewMgr:CloseView(CrystalView)
            return
        end
        if self.item:Type() == ItemType.EleCrystal then
            local param_t = {
                column = self.item.column_type,
                index = self.item.index
            }
            BagCtrl.Instance:SendUseItem(param_t)
        end
        self.item = nil
        ViewMgr:CloseView(CrystalView)
    end
end

function CrystalView:OnClickLevelUp()
    ViewMgr:OpenViewByKey(Mod.Workshop.NeoCompose)
    ViewMgr:CloseView(CrystalView)
end

function CrystalView:OnClickItem(item)
    self.item = item:GetData()
    self.cell = item
    UH.SetText(self.BtnText, Language.Fabao.BtnState[self.item.index == -1 and 2 or 1])
end

function CrystalView:OnClickCompose()
    ViewMgr:OpenViewByKey(Mod.Workshop.NeoCompose)
    -- ViewMgr:CloseView(CrystalView)
end

function CrystalView:OnClickBuy()
    ViewMgr:OpenViewByKey(Mod.Mall.Shop)
    -- ViewMgr:CloseView(CrystalView)
end

CrystalItem = CrystalItem or DeclareMono("CrystalItem", UIWidgetBaseItem)

function CrystalItem:SetData(data)
    self.Cell:SetData(data)
    UH.SetText(self.Name, data:Name())
    self.EleAttr:SetData(data:Element())
    UIWidgetBaseItem.SetData(self, data)
    self.Equip:SetActive(data.index == -1)
    self.Label:SetActive(data:UsedTime() == 0)
    UH.SetText(self.UseNum, data:UsedTime() ~= 0 and string.format(Language.EleCrystal.UseNum, data:UsedTime()) or "")
end

function CrystalItem:Click()
    if self.Toggle then
        self.Toggle:Trigger()
    end
end