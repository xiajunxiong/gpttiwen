
DuChuangZeiYingEffect = DuChuangZeiYingEffect or DeclareMono("DuChuangZeiYingEffect")

DuChuangZeiYingEffectRange = DuChuangZeiYingEffectRange or DeclareMono("DuChuangZeiYingEffectRange")

function DuChuangZeiYingEffectRange:SetData(data)
	self.data = data
end

function DuChuangZeiYingEffectRange:OnTriggerEnter(collider)
	if "MainRole" == collider.tag then
		BattleCtrl.BattleModeFightReq(
			{
				battle_mode = BATTLE_MODE.BATTLE_MODE_DU_CHUANG_ZEI_YING,
				param2 = self.data.id,
			}
		)
	end
end