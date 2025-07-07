FILE_REQ("modules/monster/monster_data")

MonsterCtrl = MonsterCtrl or BaseClass(BaseCtrl)

function MonsterCtrl:__init()
	if MonsterCtrl.Instance ~= nil then
		Debuger.LogError("[MonsterCtrl] attempt to create singleton twice!")
		return
	end
	MonsterCtrl.Instance = self
	MonsterData.New()
end

function MonsterCtrl:__delete()
	MonsterCtrl.Instance = nil
end