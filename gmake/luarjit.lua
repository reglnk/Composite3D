require "os"
require "io"
require "gmake/util"

local actions, targetOS, buildProfile, outDir = ...

local outFile = outDir .."/luarjit.cache"

-- == what can be done to obtain libs ==
-- 'clone' implies 'build'
-- 'build' will run cmake and build the library if needed, according to build profile
-- 'systemlib' will try to find systemwide installed library and use it
-- 'provided' will try to use prebuilt library by paths provided here
-- if nothing of above, only existing library will be used
local lact = actions:split(",")

-- where to locate or clone the library
local ldir = "external/luarjit2"

local linc = ldir .."/src"
local lbdir = ldir .."/src"

-- ================

shexecf('/bin/mkdir -p "%s"', outDir);

local function tryProvided()
	if not checkdir(ldir) then
		verbose("[luarjit.lua] not found at [".. ldir .."]")
		return
	end
	if not checkdir(linc) then
		verbose("[luarjit.lua] includes not found at [".. linc .."]")
		return
	end
	-- TODO find static library if luarjit's built so
	local ilib
	if targetOS ~= "Windows" then
		ilib = ldir .. "/src/libluarjit-5.1.so.2";
	else
		ilib = ldir .. "/src/libluarjit-5.1.dll.a";
	end
	if not checkfile(ilib) then
		verbose("[luarjit.lua] library not found at [".. ilib .."]");
		return
	end
	return {
		luarjit_DIR = ldir,
		luarjit_INC = linc,
		luarjit_BDIR = lbdir,
		luarjit_LIB = ilib
	}
end

local function trySystemUnix()
	local res = {luarjit_INC = "/usr/local/include/luarjit-2.1"};
	if not checkdir(res.luarjit_INC) then
		verbose("[luarjit.lua] includes not found at [".. res.luarjit_INC .."]")
		return
	end
	for ld in pairs {"/usr/lib", "/usr/local/lib", "/usr/lib64", "/usr/local/lib64"} do
		if checkfile(ld .."/libluarjit-5.1.so.2") then
			res.luarjit_LIB = ld .."/libluarjit-5.1.so.2";
		end
	end
	if not res.luarjit_LIB then
		return
	end
	return res
end

local function trySystemWindows()
	local res = {luarjit_DIR = "C:/Program Files (x86)/luarjit"};
	if not checkdir(res.luarjit_DIR) then
		verbose("[luarjit.lua] not found at [".. res.luarjit_DIR .."]")
		return
	end
	-- todo
	return res
end

local function tryBuild()
	if not checkdir(ldir) then
		verbose("[luarjit.lua] not found at [".. ldir .."]")
		return
	end
	if not checkdir(linc) then
		verbose("[luarjit.lua] includes not found at [".. linc .."]")
		return
	end
	shexecf('gmake -C "%s" -j4', lbdir);
	-- TODO find static library if luarjit's built so
	local ilib
	if targetOS ~= "Windows" then
		ilib = ldir .. "/src/libluarjit-5.1.so.2";
	else
		ilib = ldir .. "/src/libluarjit-5.1.dll.a";
	end
	if not checkfile(ilib) then
		verbose("[luarjit.lua] library not found at [".. ilib .."]");
		return
	end
	return {
		luarjit_DIR = ldir,
		luarjit_INC = linc,
		luarjit_BDIR = lbdir,
		luarjit_LIB = ilib
	}
end

local function tryClone()
	if not checkdir(ldir) then
		verbose("[luarjit.lua] not found at [".. ldir .."]. cloning...")
		shexecf('git clone https://github.com/reglnk/luarjit2 "%s"', ldir)
	end
	-- shexecf('git -C "%s" checkout 7b6aead9fb88b3623e3b3725ebb42670cbe4c579', ldir) -- 5.4.3
	return tryBuild()
end

local acttab = {
	["provided"] = tryProvided,
	["systemlib"] = trySystemUnix,
	["build"] = tryBuild,
	["clone"] = tryClone,
	["0"] = function() end
};

for i, v in ipairs(lact) do
	local res = acttab[v]()
	if res then
		local fp = io.open(outFile, "wb")
		if not fp then error("failed to open outFile") end
		for k, kv in pairs(res) do
			fp:write(k .. "=" .. kv .. "\n")
		end
		fp:flush()
		fp:close()
		os.execute("sync")
		break
	end
end

return 0