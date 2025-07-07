local Biter = Game.Biter
bit = {data8 = {},data16 = {},data32 = {},data64={}}

for i = 1, 8 do
	bit.data8[i] = 2 ^(8 - i)
end

for i = 1, 16 do
	bit.data16[i] = 2 ^(16 - i)
end

for i = 1, 32 do
	bit.data32[i] = 2 ^(32 - i)
end

for i=1,64 do 
	bit.data64[i]=2 ^(64 - i)
end 

bit.max = 16

-- number转table 1->64 高位->低位
function bit:d64b(arg)
	local tr = {}
	for i = 1, 64 do
		if arg >= self.data64[i] then
			tr[i] = 1
			arg = arg - self.data64[i]
		else
			tr[i] = 0
		end
	end
	return tr
end

function bit:b64d(arg)
	local nr = 0
	for i = 1, 64 do
		if arg[i] == 1 then
			nr = nr + 2 ^(64 - i)
		end
	end
	return nr
end

-- char转table 1->8 
function bit:d8b(arg)
	local tr = {}
	for i = 1, 8 do
		if arg >= self.data8[i] then
			tr[i] = 1
			arg = arg - self.data8[i]
		else
			tr[i] = 0
		end
	end
	return tr
end
-- table转char 1->8
function bit:b8d(arg)
	local nr = 0
	for i = 1, 8 do
		if arg[i] == 1 then
			nr = nr + 2 ^(8 - i)
		end
	end
	return nr
end

-- number转table 1->32 高位->低位
-- 接收int二进制常用
function bit:d2b(arg)
	local tr = {}
	for i = 1, 32 do
		if arg >= self.data32[i] then
			tr[i] = 1
			arg = arg - self.data32[i]
		else
			tr[i] = 0
		end
	end
	return tr
end

-- table转number
-- 请求int二进制使用
function bit:b2d(arg)
	local nr = 0
	for i = 1, 32 do
		if arg[i] == 1 then
			nr = nr + 2 ^(32 - i)
		end
	end
	return nr
end

--table转number
--低位到高位 16位
function bit:b16d(arg)
	local nr = 0
	for i = 1, 16 do
		if arg[i] == 1 then
			nr = nr + 2 ^ (i - 1)
		end
	end
	return nr
end

-- number转table 1->16 高位->低位
function bit:d16b(arg)
	local tr = {}
	for i = 1, 16 do
		if arg >= self.data16[i] then
			tr[i] = 1
			arg = arg - self.data16[i]
		else
			tr[i] = 0
		end
	end
	return tr
end

-- long long转table 1->64 高位->低位
-- @high 高32位
-- @low 低32位
function bit:ll2b(high, low)
	local high_t, low_t = bit:d2b(high), bit:d2b(low)
	
	for i = 1, 32 do
		high_t[i + 32] = low_t[i]
	end
	
	return high_t
end

-- table转long long
-- @return 高32位，低32位
function bit:b2ll(arg)
    local high_t, low_t = {}, {}
    for i = 1, 32 do
        high_t[i] = arg[i]
        low_t[i] = arg[i + 32]
    end
    return bit:b2d(high_t), bit:b2d(low_t)
end

-- 按位与【别用！危险！！！C#接口参数有问题别用！】
function bit:_and(a, b)
	return Biter._and(a, b)
end

-- 按位或【别用！危险！！！C#接口参数有问题别用！】
function bit:_or(a, b)
	return Biter._or(a, b)
end

-- 按位取反【别用！危险！！！C#接口参数有问题别用！】
function bit:_not(a)
	return Biter._not(a)
end

-- 按位异或【别用！危险！！！C#接口参数有问题别用！】
function bit:_xor(a, b)
	return Biter._xor(a, b)
end


--判断传入数字(a)的第几(n)个二进制位[从右到左]是否有值[为1]
--a  需要判断的二进制数
--n  判断第几位[从右到左]从1开始，1表示右数第一位
--return 返回该位上是否有值[为1]
function bit:_hasflag(a,n)
	local tmp = bit:_rshift(a,n-1)
	return (tmp % 2) ~= 0
end

-- 右移
function bit:_rshift(a, n)
	return math.floor(a / (2^n))
	--return Biter._rshift(a, n)
end

-- 左移
function bit:_lshift(a, n)
	return a * (2^n)
	--return Biter._rshift(a, n)
end

--长判位
function bit:_lland(a,b)
	local t_tab=bit:d64b(a)
	return t_tab[64-b]
end 

--短判位
function bit:_intand(a, b)
    local t_tab = bit:d2b(a)
    return t_tab[32 - b]
end

--连接二进制tab，int源
function bit:And(tab,start)
	local a = {}
	local index = start and 1 or 0
    for i = 1, #tab do
		local arge = bit:d2b(tab[i])
		for n = index, 31 + index do
			a[n + 32 * (i - 1)] = arge[32 + index - n]
		end
    end
    return a
end

--链接二进制short源类型表
function bit:SAnd(tab,start)
	local a = {}
	local index = start and 1 or 0
    for i = 1, #tab do
		local arge = bit:d16b(tab[i])
		for n = index, 15 + index do
			a[n + 16 * (i - 1)] = arge[16 + index - n]
		end
    end
    return a
end

--int 转两个 short
function bit:b2s(arg)
	local a,b = 0,0
	for i = 1, 32 do
		if arg[i] == 1 then
			if i <= 16 then
				a = a + 2 ^(32 - i)
			else
				b = b + 2 ^(32 - i)
			end
		end
	end
	return a,b
end

function bit:uint_and(a,b)
    local p,c=1,0
	b = b < 0 and -b or b
    while a>0 and b>0 do
        local ra,rb=a%2,b%2
        if ra+rb>1 then c=c+p end
        a,b,p=(a-ra)/2,(b-rb)/2,p*2
    end
    return c
end