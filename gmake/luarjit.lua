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

local message_verbose = function(...)
	print("=== [luarjit.lua] ".. table.concat({...}))
end

-- ================

shexecf('mkdir -p "%s"', outDir);

local function tryProvided()
	if not checkdir(ldir) then
		message_verbose("not found at [".. ldir .."]")
		return
	end
	if not checkdir(linc) then
		message_verbose("includes not found at [".. linc .."]")
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
		message_verbose("library not found at [".. ilib .."]");
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
		message_verbose("includes not found at [".. res.luarjit_INC .."]")
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
		message_verbose("not found at [".. res.luarjit_DIR .."]")
		return
	end
	-- todo
	return res
end

local function tryBuild()
	if not checkdir(ldir) then
		message_verbose("not found at [".. ldir .."]")
		return
	end
	if not checkdir(linc) then
		message_verbose("includes not found at [".. linc .."]")
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
		message_verbose("library not found at [".. ilib .."] - after a build attempt");
		return false
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
		message_verbose("not found at [".. ldir .."]. cloning...")
		shexecf('git clone https://github.com/reglnk/luarjit2 "%s"', ldir)
	end
	return tryBuild()
end

local acttab = {
	["provided"] = tryProvided,
	["systemlib"] = trySystemUnix,
	["build"] = tryBuild,
	["clone"] = tryClone,
	["0"] = function() end
};

local success = main_action(lact, acttab, outFile, message_verbose);
if success then
	return
end

message_error("no one of methods worked");
