-- 造化装备·背包界面
MarbasBagView = MarbasBagView or DeclareView("MarbasBagView", "marbas/marbas_bag",Mod.Marbas.Bag)
function MarbasBagView:MarbasBagView()
    -- self.board
end
function MarbasBagView:LoadCallback(param_t)
    self.Board:SetData(self:GetType(), Language.Marbas.BagView, Vector2.New(590, 673))

    MarbasCtrl.Instance:SendEquipOper(MarbasReqType.Order)
end

function MarbasBagView:OnCloseClick()
    ViewMgr:CloseView(MarbasBagView)
end

--------------------------MarbasBagPanel--------------------------
MarbasBagPanel = MarbasBagPanel or DeclareMono("MarbasBagPanel", UIWFlushPanel)
function MarbasBagPanel:MarbasBagPanel()
    self.data = MarbasData.Instance
    self.data_cares = {
        {data = self.data.item_grid, func = self.FlushBag,init_call = false},
    }

    self.fix_type = -1

    GuideManager.Instance:RegisterGetGuideUi("MarbasBagSelect", function ()
        if self.bag_list.index_item and self.bag_list.index_item[1] then
            return self.bag_list.index_item[1],function()
                self.bag_list.index_item[1]:OnClick()
            end
        end 
        return nil
    end)
end 

function MarbasBagPanel:onFlush()
    self:FlushBag()
end

function MarbasBagPanel:FlushBag()
    local list = self.data:GetMarbasBag(self.fix_type)
    self.bag_list:SetData(list)
end

function MarbasBagPanel:OnClickLevelSelect()
    local activeSelf = not self.BtnParent.activeSelf
    self.BtnParent:SetActive(activeSelf)
    self.SelectArrow:SetActive(not activeSelf)
    self.ProtectArea:SetActive(activeSelf)
    if activeSelf then
        self.BtnList:SetData(MarbasConfig.fix_select)
    end
    AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ClickGeneral2)
end 

function MarbasBagPanel:OnClickCloseShaiXuan()
    local activeSelf = not self.BtnParent.activeSelf
    self.BtnParent:SetActive(activeSelf)
    self.SelectArrow:SetActive(not activeSelf)
    self.ProtectArea:SetActive(activeSelf)
end 

function MarbasBagPanel:OnClickLevelBtn(data)
    self.SelectedText.text = data.name
    
    self.fix_type = data.qua
    self:FlushBag()
    self:OnClickLevelSelect()
end 

function MarbasBagPanel:OnClickResolve()
    ViewMgr:OpenView(MarbasResolveView)
end

function MarbasBagPanel:OnClickGetWay()
    ViewMgr:OpenViewByKey(Mod.HarmonyHeaven.Main)

    ViewMgr:CloseView(MarbasView)
    ViewMgr:CloseView(MarbasBagView)
end

MarbasBagBtnItem = DeclareMono("MarbasBagBtnItem", UIWidgetBaseItem)
function MarbasBagBtnItem:MarbasBagBtnItem() end
function MarbasBagBtnItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    self.Name.text = data.qua > 0 and string.format(data.name,QualityColorStr[data.qua]) or data.name
end
-- 