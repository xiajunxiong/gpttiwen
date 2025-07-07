
FightMonsterItem = DeclareMono("FightMonsterItem", UIWidgetBaseItem)

function FightMonsterItem:SetData(info)
    UIWidgetBaseItem.SetData(self, info)
    UH.SetIcon(self.IconSp, info.icon_id, ICON_TYPE.ITEM)
    self.BossI:SetActive(info.tab_type ~= FightConfig.FightTabType.field and info.is_boss)
end

function FightMonsterItem:OnClickMonster()
    local data = FightData.Instance.cur_sel_dungeon
    if ViewMgr:IsOpen(FightFBView) then
        data = FightData.Instance.fight_fb_ymz.info
    elseif ViewMgr:IsOpen(FightEliteView) then
        data = FightData.Instance.fight_elite_dungeon.info
    elseif FightConfig.FightTabType.field == FightCtrl.Instance:Data().cur_sel_tabbar.val then
        data = FightData.Instance.cur_sel_field
    end
    MapData.Instance:SetSelectSceneId(data.scene_id)
    ViewMgr:OpenView(MapInfoView,{item = Monster.New{item_id = self.data.monster_id},root_pos = UH.GetRootPosition(self.gameObject),
    funcs = {{name = Language.Activity.RemindTips[1],enter_click = data.click_handle}}})
end