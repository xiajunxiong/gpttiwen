FILE_REQ("modules/workshop/workshop_forging")
FILE_REQ("modules/workshop/workshop_life")
FILE_REQ("modules/workshop/workshop_crystal")
FILE_REQ("modules/workshop/workshop_compound")
FILE_REQ("modules/workshop/workshop_inset")
FILE_REQ("modules/workshop/workshop_neocompose")
FILE_REQ("modules/workshop/workshop_jewelry")
FILE_REQ("modules/workshop/jewelry_recast")
FILE_REQ("modules/workshop/compose_deteat")
FILE_REQ("modules/workshop/compose_succ")
FILE_REQ("modules/workshop/workshop_smelt")
FILE_REQ("modules/workshop/workshop_stonesell")
Workshop = Workshop or DeclareView("Workshop", "workshop/workshop", Mod.Workshop.Forging)
VIEW_DECLARE_LEVEL(Workshop, ViewLevel.L1)
function Workshop:Workshop()
  -- 工坊强化，工坊熔炼，工坊镶嵌搬到装备系统上
    self.lang = Language.Workshop
    -- GuideManager.Instance:RegisterGetGuideUi("WorkshopTabInset", function ()
		-- return self.Board.Tabbar.item_list[4]
    -- end)

    GuideManager.Instance:RegisterGetGuideUi("WorkshopTabJewelry", function ()
		return self.Board.Tabbar.item_list[2]
    end)

    GuideManager.Instance:RegisterGetGuideUi("WorkshopTabNeoCompose", function ()
		return self.Board.Tabbar.item_list[4]
	  end)
    
    -- GuideManager.Instance:RegisterGetGuideUi("WorkshopTabUpgrade", function ()
    -- return self.Board.Tabbar.item_list[3]
    -- end)
end

function Workshop:LoadCallback(param_t)
  MarketCtrl.Instance:SendMarketReq()
  self.Board:SetData(self:GetType(), self.lang.Title1)
  self.Board:SetTabbarVisible(true)
  if ViewMgr:IsOpen(AnotherView) then
      self.Board.Tabbar:SetData({{name = self.lang.Tab3, tab_objs = {self.Tab3}, open = Mod.Workshop.NeoCompose}})
  else
    local param_t = {
      {name = self.lang.Tab1, tab_objs = {self.Tab1}, open = Mod.Workshop.Forging, mod = TabMod(Mod.Workshop.Forging)},
      {name = self.lang.Tab5, tab_objs = {self.Tab5}, open = Mod.Workshop.Jewelry, mod = TabMod(Mod.Workshop.Jewelry)},
      -- {name = self.lang.Tab2, tab_objs = {self.Tab2}, open = Mod.Workshop.Upgrade, mod = TabMod(Mod.Workshop.Upgrade)},
      -- {name = self.lang.Tab4, tab_objs = {self.Tab4}, open = Mod.Workshop.Inser, mod = TabMod(Mod.Workshop.Inser)},
      -- {name = self.lang.Tab6, tab_objs = {self.Tab6}, open = Mod.Workshop.Smelt, mod = TabMod(Mod.Workshop.Smelt)},
      -- {name = self.lang.Tab3, tab_objs = {self.Tab3}, open = Mod.Workshop.NeoCompose},
      {name = self.lang.Tab8, tab_objs = {self.Tab8}, open = Mod.Workshop.BlazeStar,mod = TabMod(Mod.Workshop.BlazeStar)},
      -- {name = self.lang.Tab3, tab_objs = {self.Tab3}, open = Mod.Workshop.Crystal},
      -- {name = self.lang.Tab4, tab_objs = {self.Tab4}, open = Mod.Workshop.Comp--[[ mod = TabMod(Mod.Workshop.Comp) ]]},
      -- {name = self.lang.Tab7, tab_objs = {self.Tab7}, open = Mod.Workshop.NeoCompose},
    }

    table.insert(param_t,{name = self.lang.Tab3, tab_objs = {self.Tab3}, open = Mod.Workshop.NeoCompose,mod = TabMod(Mod.Workshop.NeoCompose)})   
  
    self.Board.Tabbar:SetData(param_t)
  end
  self.Board.Tabbar:ClickItem(FunOpen.Instance:IWantSelect(Mod.Workshop, self.Board.Tabbar, param_t and param_t.open_param or nil))
  self.Currency:DefaultSet()

  BagCtrl.Instance:SendEquipmentOperaReq({
    oper_type = 5,
    param_1 = 0,})
end

function Workshop:CloseCallback()
  WorkshopData.Instance.complete_item.item = nil
  TaskData.Instance:TaskChainSubContinue()
end
