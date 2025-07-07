FILE_REQ("modules/star_treasure_map/star_treasure_map_data")
VIEW_REQ("modules/star_treasure_map/star_treasure_map_view")
VIEW_REQ("modules/star_treasure_map/star_treasure_map_roller")

StarTreasureMapCtrl = StarTreasureMapCtrl or BaseClass(BaseCtrl)

function StarTreasureMapCtrl:__init()
	if StarTreasureMapCtrl.Instance ~= nil then
		Debuger.LogError("[StarTreasureMapCtrl] attempt to create singleton twice!")
		return
	end
	StarTreasureMapCtrl.Instance = self
	self.data = StarTreasureMapData.New()
end

function StarTreasureMapCtrl:StartTouchEnded(click_enter_func)
	self:EndTouchEnded()
	self.touch_ended_handle = EventSys:Bind(GlobalEventKeys.TOUCH_ENDED,function()
		if MainPlayer then
        	MainPlayer:StopMove()
    	end 
		PublicPopupCtrl.Instance:DialogTips({
			content = Language.StarTreasureMap.EnterTip,
			cancel = {
				func = function()
					self:EndTouchEnded()
					ViewMgr:CloseView(DialogTipsView)
				end
			},
			confirm = {name = Language.StarTreasureMap.Continue,func = function()
				if click_enter_func ~= nil then
					click_enter_func()
					self:StartTouchEnded(click_enter_func)
				end
				ViewMgr:CloseView(DialogTipsView)
			end}
		})
	end)
end

function StarTreasureMapCtrl:Check()
	self:EndTouchEnded()
	local item = TreasureMapData.Instance:MapInfo()
	if item then
		local column = item.column_type
		local index = item.index
		--这里要再检查一遍
		local target, need_samp = SceneCtrl.Instance:CurScene():PosToClint(item.param.x, item.param.y)
		if (target.y == 0 or need_samp == true) and item.param.scene_id == SceneData.Instance:SceneId() then
			BagCtrl.Instance:SendActiveSpecialLogicItem(1,item.column_type, item.index)
			PublicPopupCtrl.Instance:DialogTips({
				content = Language.StarTreasureMap.ErrorPos,
				cancel = {
					func = function()
						TreasureMapData.Instance:ClearItem()
						ViewMgr:CloseView(DialogTipsView)
					end
				},
				confirm = {name = Language.StarTreasureMap.ReUse,func = function()
					item = BagData.Instance:GetItemByIndex(column,index)
					if item and item:IsStarTreasureMap() then
						TreasureMapData.Instance:SetMapInfo(item)
						ViewMgr:OpenView(item:IsActiveMap() and StarTreasureMapView or StarTreasureMapAtlaView)
					end
					ViewMgr:CloseView(DialogTipsView)
				end}
			})
		end
	end
end

function StarTreasureMapCtrl:EndTouchEnded()
	EventSys:UnBind(self.touch_ended_handle)
	self.touch_ended_handle = nil
end