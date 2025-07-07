require("modules/common/color_enum")

--0~255
local function _MC(color)
	if color then
		return string.format("%02x%02x%02x", color.r * 255, color.g * 255, color.b * 255)
	end
end

COLORSTR = {
	Transparent = "ffffff00", 
	White = _MC(_NC(255,255,255)),
	Black1 = _MC(COLORS.Black1),

	Gray1 = _MC(COLORS.Gray1),
	Gray2 = _MC(COLORS.Gray2),
	Gray3 = _MC(COLORS.Gray3),
	Gray4 = _MC(COLORS.Gray4),

	Yellow1 = _MC(COLORS.Yellow1),
	Yellow2 = _MC(COLORS.Yellow2),
	Yellow3 = _MC(COLORS.Yellow3),
	Yellow4 = _MC(COLORS.Yellow4),
	Yellow5 = _MC(COLORS.Yellow5),
	Yellow6 = _MC(COLORS.Yellow6),
	Yellow7 = _MC(COLORS.Yellow7),
	Yellow8 = _MC(COLORS.Yellow8),
	Yellow9 = _MC(COLORS.Yellow9),
	Yellow10 = _MC(COLORS.Yellow10),
	Yellow11 = _MC(COLORS.Yellow11),
	Yellow12 = _MC(COLORS.Yellow12),
	Yellow13 = _MC(COLORS.Yellow13),
	Yellow14 = _MC(COLORS.Yellow14),
	Yellow15 = _MC(COLORS.Yellow15),
	Yellow16 = _MC(COLORS.Yellow16),
	Yellow17 = _MC(COLORS.Yellow17),
	Yellow18 = _MC(COLORS.Yellow18),
	Yellow19 = _MC(COLORS.Yellow19),
	Yellow20 = _MC(COLORS.Yellow20),
	Yellow21 = _MC(COLORS.Yellow21),
	Yellow22 = _MC(COLORS.Yellow22),
	Yellow23 = _MC(COLORS.Yellow23),
	Yellow24 = _MC(COLORS.Yellow24),
	Yellow25 = _MC(COLORS.Yellow25),

	Purple1 = _MC(COLORS.Purple1),
	Purple2 = _MC(COLORS.Purple2),
	Purple3 = _MC(COLORS.Purple3),
	Purple4 = _MC(COLORS.Purple4),
	Purple5 = _MC(COLORS.Purple5),
	Purple6 = _MC(COLORS.Purple6),
	Purple7 = _MC(COLORS.Purple7),
	Purple8 = _MC(COLORS.Purple8),

	Blue1 = _MC(COLORS.Blue1),
	Blue2 = _MC(COLORS.Blue2),
	Blue3 = _MC(COLORS.Blue3),
	Blue4 = _MC(COLORS.Blue4),
	Blue5 = _MC(COLORS.Blue5),
	Blue6 = _MC(COLORS.Blue6),
	Blue7 = _MC(COLORS.Blue7),
	Blue8 = _MC(COLORS.Blue8),
	Blue9 = _MC(COLORS.Blue9),
	Blue10 = _MC(COLORS.Blue10),
	Blue11 = _MC(COLORS.Blue11),
	Blue12 = _MC(COLORS.Blue12),
	Blue13 = _MC(COLORS.Blue13),
	Blue14 = _MC(COLORS.Blue14),

	Green1 = _MC(COLORS.Green1),
	Green2 = _MC(COLORS.Green2),
	Green3 = _MC(COLORS.Green3),
	Green4 = _MC(COLORS.Green4),
	Green5 = _MC(COLORS.Green5),
	Green6 = _MC(COLORS.Green6),
	Green7 = _MC(COLORS.Green7),
	Green8 = _MC(COLORS.Green8),
	Green9 = _MC(COLORS.Green9),
	Green10 = _MC(COLORS.Green10),
	Green11 = _MC(COLORS.Green11),
	Green13 = _MC(COLORS.Green13),
	Green14 = _MC(COLORS.Green14),

	Red1 = _MC(COLORS.Red1),
	Red2 = _MC(COLORS.Red2),
	Red3 = _MC(COLORS.Red3),
	Red4 = _MC(COLORS.Red4),
	Red5 = _MC(COLORS.Red5),
	Red6 = _MC(COLORS.Red6),
	Red7 = _MC(COLORS.Red7),
	Red8 = _MC(COLORS.Red8),
	Red9 = _MC(COLORS.Red9),
	Red10 = _MC(COLORS.Red10),
	Red11 = _MC(COLORS.Red11),
	Red12 = _MC(COLORS.Red12),
	Red13 = _MC(COLORS.Red13),
	Red14 = _MC(COLORS.Red14),
	Red15 = _MC(COLORS.Red15),
	Red16 = _MC(COLORS.Red16),
	Red17 = _MC(COLORS.Red17),
	Red18 = _MC(COLORS.Red18),
	Red19 = _MC(COLORS.Red19),
	Red20 = _MC(COLORS.Red20),
	Red21 = _MC(COLORS.Red21),

	Orange1 = _MC(COLORS.Orange1),
	Orange2 = _MC(COLORS.Orange2),

	Pink1 = _MC(COLORS.Pink1),
	Pink2 = _MC(COLORS.Pink2),

	Gold1 = _MC(COLORS.Gold1),
	Gold2 = _MC(COLORS.Gold2),
}

QualityColor = {
	[ItemColor.White] = COLORS.Yellow19,
	[ItemColor.Green] = COLORS.Green4,
	[ItemColor.Blue] = COLORS.Blue2,
	[ItemColor.Purple] = COLORS.Purple1,
	[ItemColor.Orange] = COLORS.Yellow6,
	[ItemColor.Red] = COLORS.Red8,
	[ItemColor.Star] = COLORS.Yellow6,
	[ItemColor.God] = COLORS.Red8,
	[ItemColor.Gold] = COLORS.Pink1,
	[ItemColor.Black] = COLORS.Gold1,
}
QualityDeepColor = {
	[ItemColor.White] = COLORS.Yellow24,
	[ItemColor.Green] = COLORS.Green9,
	[ItemColor.Blue] = COLORS.Blue6,
	[ItemColor.Purple] = COLORS.Purple4,
	[ItemColor.Orange] = COLORS.Yellow25,
	[ItemColor.Red] = COLORS.Red20,
	[ItemColor.Star] = COLORSTR.Yellow25,
	[ItemColor.God] = COLORSTR.Red20,
	[ItemColor.Gold] = COLORS.Pink2,
	[ItemColor.Black] = COLORS.Gold2,
}
--深色背景
QualityColorStr = {
	[ItemColor.None] = COLORSTR.Yellow23,
	[ItemColor.White] = COLORSTR.Yellow23,
	[ItemColor.Green] = COLORSTR.Green4,
	[ItemColor.Blue] = COLORSTR.Blue2,
	[ItemColor.Purple] = COLORSTR.Purple1,
	[ItemColor.Orange] = COLORSTR.Yellow6,
	[ItemColor.Red] = COLORSTR.Red8,
	[ItemColor.Star] = COLORSTR.Yellow6,
	[ItemColor.God] = COLORSTR.Red8,
	[ItemColor.Gold] = COLORSTR.Pink1,
	[ItemColor.Black] = COLORSTR.Gold1,
}


QualityColorShallow = {
	[ItemColor.None] = COLORS.Yellow24,
	[ItemColor.White] = COLORS.Yellow24,
	[ItemColor.Green] = COLORS.Green9,
	[ItemColor.Blue] = COLORS.Blue6,
	[ItemColor.Purple] = COLORS.Purple4,
	[ItemColor.Orange] = COLORS.Yellow25,
	[ItemColor.Red] = COLORS.Red20,
	[ItemColor.Star] = COLORS.Yellow25,
	[ItemColor.God] = COLORS.Red20,
	[ItemColor.Gold] = COLORS.Pink2,
	[ItemColor.Black] = COLORSTR.Gold2,
}

--浅色背景
QualityColorStrShallow = {
	[ItemColor.None] = COLORSTR.Yellow24,
	[ItemColor.White] = COLORSTR.Yellow24,
	[ItemColor.Green] = COLORSTR.Green9,
	[ItemColor.Blue] = COLORSTR.Blue6,
	[ItemColor.Purple] = COLORSTR.Purple4,
	[ItemColor.Orange] = COLORSTR.Yellow25,
	[ItemColor.Red] = COLORSTR.Red20,
	[ItemColor.Star] = COLORSTR.Yellow25,
	[ItemColor.God] = COLORSTR.Red20,
	[ItemColor.Gold] = COLORSTR.Pink2,
	[ItemColor.Black] = COLORSTR.Gold2,
}
--熔炼界面专用
QualityColorStrSmelt = {
	[ItemColor.None] = COLORSTR.Yellow24,
	[ItemColor.White] = COLORSTR.Yellow24,
	[ItemColor.Green] = COLORSTR.Green9,
	[ItemColor.Blue] = COLORSTR.Blue6,
	[ItemColor.Purple] = COLORSTR.Purple4,
	[ItemColor.Orange] = COLORSTR.Yellow25,
	[ItemColor.Red] = COLORSTR.Red20,
	[ItemColor.Star] = COLORSTR.Yellow25,
	[ItemColor.God] = COLORSTR.Red20,
	[ItemColor.Gold] = COLORSTR.Pink1,
	[ItemColor.Black] = COLORSTR.Gold1,
}

--声望等级颜色
PRESTIGE_LEVEL_COLOR = {
    [0] = COLORS.Gray3,
    [1] = COLORS.Gray1,
    [2] = COLORS.Red15,
    [3] = COLORS.Blue6,
    [4] = COLORS.Green11,
    [5] = COLORS.Purple4,
    [6] = COLORS.Red1,
}

-- 勋章特效描边颜色
Medal_Color_Outline = {
    [0] = COLORS.Gray1,
    [1] = COLORS.Yellow19,
    [2] = COLORS.Green4,
    [3] = _NC(0,  48, 206),
    [4] = _NC(167,0,  161),
    [5] = _NC(212,65, 0  ),
    [6] = _NC(203,0,  0  ),
}

-- 勋章特效阴影颜色
Medal_Color_Shadow = {
    [0] = COLORS.Gray1, 
    [1] = COLORS.Yellow19,
    [2] = COLORS.Green4,
    [3] = _NC(0,  84, 255),
    [4] = _NC(204,0,  255),
    [5] = _NC(255,108,0  ),
    [6] = _NC(255,0,  0  ),
}

-- 伙伴品质色
PartnerQualityColor = {
	[ItemColor.White] = COLORSTR.Yellow19,
	[ItemColor.Green] = COLORSTR.Green4,
	[ItemColor.Blue] = COLORSTR.Blue2,
	[ItemColor.Purple] = COLORSTR.Purple1,
	[ItemColor.Orange] = COLORSTR.Yellow6,
	[ItemColor.Red] = COLORSTR.Red8,
	[ItemColor.Star] = COLORSTR.Pink1,
}
PartnerQualityDeepColor = {
	[ItemColor.White] = COLORSTR.Yellow24,
	[ItemColor.Green] = COLORSTR.Green9,
	[ItemColor.Blue] = COLORSTR.Blue6,
	[ItemColor.Purple] = COLORSTR.Purple4,
	[ItemColor.Orange] = COLORSTR.Yellow25,
	[ItemColor.Red] = COLORSTR.Red20,
	[ItemColor.Star] = COLORSTR.Pink2,
}

CrystalNameColor = {
	[40000] = QualityColorStr[ItemColor.Blue],
	[40001] = QualityColorStr[ItemColor.Red],
	[40002] = COLORSTR.Yellow16,
	[40003] = QualityColorStr[ItemColor.Green],
}