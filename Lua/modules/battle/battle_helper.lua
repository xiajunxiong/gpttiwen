BattleHelper = BattleHelper or {}

--夺魂摄魄
BattleHelper.DuoHunShePo = 7060
--夺魂摄魄表现在 血量变化时做逻辑
BattleHelper.DuoHunShePoAsset = "DuoHunShePo"

--自身反噬的技能不显示被击
BattleHelper.HIT_SELF_NOSHOW = {
	["BuffWLHunLuanGongJi"] = true,
	["SheShenQuYi"] = true,
	["YiShenPoDi"] = true,
	["SheMingYiJi"] = true,
	["PuGongLianJiShouJi"] = true,
	["SheMingYiJi2"] = true,
}
--技能表现不在原地的不显示被击
BattleHelper.SKILL_NO_BEHIT = {
	["MFDanTiFengRenMoFaXuLi"] = true,
	["HuanJingZhiYi"] = true,
	["HunLuanZhiFeng"] = true,
	["ChenMoFengBao"] = true,
	["MFQunTiFengRenMoFaXuLi"] = true,
}
-- 溅射技能 被击触发对象，为被击表的第一个
BattleHelper.SKILL_JIANSHE = {
	["LuoYingLianFu"] = true,
	["TengBian"] = true,
}
--战斗播放队列限制 (某些播放剧情的战斗，要一个一个播放)
BattleHelper.LIMIT_PLAY_QUEUE = {
	[1014] = 1,
	[1002] = 1,
	[1001] = 1,
	[1009] = 1,
}
--技能伤害延迟显示
BattleHelper.DAMAGE_DELAY_SHOWTIME = {
	["WuYingQuan"] = 0.4,
	-- ["PuGongZhuRenShouJi"] = 0.1
}
-- 延迟血量飘字
BattleHelper.FABAO_DELAY_HP = {
    [112] = true,
}
--释放法宝需要被击表现的类型 effect
BattleHelper.FABAO_EFFECT =
{
    [112] = true,
    [115] = true
}
--战斧格挡有buff特效
BattleHelper.ZhanFuGeDangXuLi = "ZhanFuGeDangXuLi"
--仙灵祝福 加血时要额外特效
BattleHelper.XianLingZhuFu = "XianLingZhuFu"
--援护，偃月兔技能
BattleHelper.YuanHu = "YuanHu"
--金戈铁马 buff为目标添加特效
BattleHelper.JinGeTieMa = 306
BattleHelper.XiXueGongJiID = 132
--无敌buff -1显示其他名字
BattleHelper.WuDiBuff = 683
--播放buff表现 延迟表现的buff 护盾 石化 冰封 在事件的结束后播放
BattleHelper.BUFF_DELAY_PLAY = {
    -- [420] = true,
    [303] = true,
    [3] = true,
	[4] = true,
}
--buff不在结束后播放 石化 冰封
BattleHelper.BUFF_STOP_NOW = {
    [3] = true,
    [4] = true,
    [423] = true,
	[15] = true,
}
--buff正常删除
BattleHelper.BUFF_STOP_NORMAL = {
    [770] = true,
}

BattleHelper.BingFeng = {
	[4] = true,
	[9] = true,
	[423] = true,
	[466] = true,
}
BattleHelper.HuiYanBuff = {
	[16] = true,
	[711] = true,
}
BattleHelper.HuiYanEffect = {
	[298] = true,
	[297] = true,
	[296] = true,
}
BattleHelper.YinShengBuff = {
	[15] = true,
	[712] = true,
}
BattleHelper.YinShengEffect = {
	[2500] = true,
	[473] = true,
}
BattleHelper.AttrChange = 540
BattleHelper.YinSheng = 15
BattleHelper.YinSheng2 = 712
BattleHelper.HuiYan = 16
BattleHelper.HuiYan2 = 711
BattleHelper.YinSheng2 = 712
BattleHelper.HuiYanAsset = "HuiYan"
--女娲石复活特效
BattleHelper.NvWaShiFuHuo = "NvWaShiFuHuo"
--凤凰涅槃复活特效
BattleHelper.FengHuangNiePan = "FengHuangNiePan"
--风刃魔法 吹上天那种
BattleHelper.MFDanTiFengRenMoFaXuLi = "MFDanTiFengRenMoFaXuLi"
BattleHelper.FanJi = "FanJi"
BattleHelper.FanJiHouFaZhiRen = "FanJiHouFaZhiRen"
BattleHelper.FanJiAsset = {
    [315] = "FanJiHouFaZhiRen",
    [337] = "FanJiZhanFuGeDang"
}
--特殊心魔显示怪物组
BattleHelper.XinMoBattleGroup = 42
--BOSS主位
BattleHelper.BossPos = 18
--某些冲刺技能不需要额外的移动
BattleHelper.CHONGCHI_SKILL = {
	["Assassinate"] = true,
	["PoJiaChongQuan2"] = true,
	-- ["LiYuTouChui"] = true,
	["Charge"] = true,
	["MultipleCharge"] = true,
	["PoJiaChongQuan"] = true,
	["TieBiChongYa"] = true,
	["SheMingYiJi2"] = true,
}
--释放冲刺不需要前置移动技能释放完后要移动
BattleHelper.CHONGCHI_PRE_SKILL = {
	["LiYuTouChui"] = true,
	["SheMingYiJi"] = true,
}
--释放时不需要进行表现
BattleHelper.SKILL_SHOW_IGNORE = {
    [2] = true
}
--护卫技能（不移动 且所有目标需添加到演员列表(之前护卫对象有前置特效不知道现在还有没有)
BattleHelper.SKILL_GUARD = {
	["HuWeiXuLi"] = true,
	["QiShiZhiShiShouJi"] = true,
}
--双击 同一技能id 不同asset_id
BattleHelper.ShuangJiLiuXing = "ShuangJiLiuXing"
BattleHelper.ShuangJiHuiXing = "ShuangJiHuiXing"
--舍身取义 需要播放额外材质特效
BattleHelper.SheMingYiJi2 = "SheMingYiJi2"
--致命一击 给每个被击者加材质特效
BattleHelper.ZhiMingYiJi = "ZhiMingYiJi"
--化险为夷 服务端发的人实际给宠物加特效
BattleHelper.HuaXianWeiYi = "HuaXianWeiYi"
--舍命一击 牛牛两个动作
BattleHelper.SheMingYiji = "SheMingYiJi"
--护卫不需要短暂停顿
BattleHelper.HUWEI = {
	["HuWei"] = true
}
--技能失效 破空显示特效
BattleHelper.PoKong = 138
BattleHelper.TieSuoLianHuan = 1470

--艺术字显示技能名称通用逻辑忽略
BattleHelper.SKILLNAME_BIG_SHOW = {
	["BaTianHuiXuan"] = true,
	["KuangNuJianTa"] = true,
}

--闪避忽略攻击者
BattleHelper.IGNORE_ATKER_DODGE = {
	["SheMingYiJi"] = true
}
--双击：一回合放这一回合放那
BattleHelper.SHUANG_JI =
{
	["ShuangJiLiuXing"] = true,
	["ShuangJiHuiXing"] = true
}
--材质buff
BattleHelper.SheShengQuYiBuff = "SheShengQuYi"

--5段被击间隔0.2秒
BattleHelper.MULTI_BEHIT5 =
{
	["XTXuanZhuanFeiFu"] = true,
	["XuanZhuanFeiFu2"] = true,
	["BaTianHuiXuan"] = true,
}
BattleHelper.IGNORE_ACTOR_LIMIT = {
	["XuanZhuanFeiFu2"] = true,
	["BaTianHuiXuan"] = true,
	["XTXuanZhuanFeiFu"] = true,
}
--4段被击间隔0.1秒
BattleHelper.MULTI_BEHIT4 = {
	["WuYingQuan"] = true,
}
--不做任何表现的特效 sp_id
BattleHelper.HIDE_SP_EFFECT = {
	[253] = true,
}
--特殊连击技能 服务端下发多次被击但客户端只需要一次，因为其他被击特效做在了Asset里头了 且不需要做闪避表现（飘字就行
--相同目标
BattleHelper.COMBO_SKILL_SP = {
	["FenShao"] = true,
	-- ["DongJie"] = true,
	["LianZhuJian"] = true,
	["SanQianLeiDong"] = true,
}
--普攻式连击
BattleHelper.COMBO_SKILL_ATTACK = {
	[1001] = true,
}
--连击不定目标 同一特效
BattleHelper.COMBO_TATGET_NOONE = {
	["BaiShouZhiNu"] = true,
}
--射手乱射
BattleHelper.PuGongLuanSheShouJi = "PuGongLuanSheShouJi"
--特殊自爆 (福牛烟花)
function BattleHelper.BATTLE_SELF_EFFECT(id)
	return (id >= 815 and id <= 840)
end

function BattleHelper.BATTLE_XINGTIAN_JUQING(id)
	return id == 1014
end
function BattleHelper.BATTLE_XINMO_JUQING(id)
	return id == 1006
end
function BattleHelper.BATTLE_NIUYAO_JUQING(id)
	return id == 1002
end
function BattleHelper.BATTLE_ORIGIN_JUQING(id)
	return id == 1001
end

--刑天放狠话
BattleHelper.XingTianTalk = 4996
--中毒攻击
BattleHelper.ZhongDuGongJiId = 148

BattleHelper.YanHuo = "YanHuo"
BattleHelper.ZiBao = "ZiBao"
BattleHelper.JueDiFanJi = "JueDiFanJi"

BattleHelper.JiangZiYaGroup = 1009

BattleHelper.BossAudioGroup = {
	[1006] = true,
	[1002] = true,
	[1001] = true,
	[1014] = true,
}

--忽略战斗连续播放队列的战斗模式
BattleHelper.BATTLE_IGNORE_SEQUENCE = {
	[BATTLE_MODE.BATTLE_MODE_INITIAL_FIGHT] = true,
}

BattleHelper.SKILL_SHIHUA = {
	["BuffMFShiHuaMoFa"] = true,
	["BuffWLShiHuaGongJiShouJi"] = true,
}

BattleHelper.ExtraLogic = {
	FenShen = "FenShen"
}
BattleHelper.YiChangJieChu = "yichangjiechu"