# Composite3D
3D-only engine for games/simulations

## a bit of history
The main purpose of writing own engines is obviously having possibility to add anything relatively quickly when you want.
SOo that's the engine which I planned to be 500% more customizable than alternatives, and suitable for simulations, games, including ones like KSP.
By the way, back in 2019-2023 I had some weak laptop with lower characheristics than system req's of that thing, so that inspired me to at least try
to optimize the engine better than Unity3d, for that kind of things. \
I hope that describes great amount of design solutions here.

## current state
You can build and launch:
- some interactive 3D OpenGL example with loading models with textures, modify some shaders, \
and repeat all that in Termux in proot-distro environment, for which I did ImGui sliders
to turn 'nearly impossible to look around' into 'not-so-convenient'.

Supported OSes are Linux, Windows, and there's some initial support for FreeBSD.

## building
In the same day I added this README, I finished GNU make for core libs + one of examples. And changed layout a bit. \
So use GNU make from the root project directory, it should create the following directory (let's call this *THIS* build layout):
build/\<YourSystem\>/\<BuildProfile\>,
automatically clone dependencies if necessary, and build them using the same build directories, and continue building the engine.

###
To prevent cloning and/or building and/or searching for system libraries, override GNUmake variables `assimp_ACT`, `glfw_ACT`, `luarjit_ACT` and so on (read gmake/\*.mk). \
Or, for all of dependencies in bunch, do the same with `deps_ACT`. \
You can also pointlessly disable using provided libraries. \
For example, setting that to `provided,systemlib` will prevent just cloning & building dependencies. To disable everything, do `deps_ACT=0`. \
That stuff is described in gmake/*.lua modules. \
When using just `provided`, make sure to have ready-to-use libraries (or symbolic links to them) in the ./external directory, with *THIS* build layout,
or again, make symbolic links like that:
```sh
mkdir -p external/glfw/build/Linux && ln -s external/glfw/mycustombuild/linux_opensuse64 external/glfw/build/Linux/opt
```

## smooth transition from building to contribution guidelines

### supported compilers
Those are GCC and Clang, and maybe in future other compilers of C and C++.
If you want to support MSVC, put this idea aside. The effort enclosed into this would worth too few:
1. the compiler is proprietary
2. it will build on very limited amount of OSes (well, most probably you'll add only one)
3. limited amount of architectures (most probably it would distribute to 98% x86_64 and 2% aarch64)
4. even if your commit is accepted, the later ones would potentially break compatibility again

### CMake support
I left CMake in here (almost working) for 2 reasons
1. for anyone willing to use CMake to make a few patches and build with CMake if they want
2. and possibly see how it sucks and understand why I switched to GNU make

But if you do support CMake and send correct patches, that's totally okay to accept.
It does not interfere with the overall development.

### codestyle
```cpp
// 1. use tabs for the main blocks indentation
// 2. for preprocessor, indentation is 2 spaces (preferable)
// 3.
funcType someLongFunctionWithManyArguments(argType argument1, argType argument2,
                                           PyObject *foo, bool i_use_python_indent_everywhere = true,
                                           float foo2 = 3.13, bar: str | int = 0, whatsthis: bool = False,
                                           *args, *kwargs) -> your_contrib_status:
    return not_accepted

// 4. proper way to write pointers
int main() {
    void *ptr = &block;
    std::string const &ref = mystr;
    const void *const **const *so;
    void const *const **const *orSo;
    char **const *charptr = nullptr;
}

/* 5.
use this {
    ;
}
or this
{
    ;
}
or this
    callfunc();

there're no strict rules for braces, just keep code enough dense and do enough gaps to make it fastly readable
*/
funcType someShortFunction(type a, type b) {
    code;
    if (code) {
        code;
    }
    else if (code + 2 * (a * b + 0xfeed)) {
        code;
    }
    return code;
}

// 6. take a look at arguments indentation (highly preferred for long argument list)
template<typename T, typename K, typename Tk>
funcType someLongFunction (
    veryLongTypeName foo,
    otherVeryLongTypeName bar,
    anotherVeryLongTypeName baz,
    some few, some args, /* of */ some same, some types,
    std::function<retType(inputType1, inputType2)> func,
    T *something,
    K *somehing2,
    Tk probablySomethingFromTkinter
) /* const noexcept etc. for methods go here */ {
    code;
    if (code) {
        code;
    }
    else if (code + 2 * (a * b + 0xfeed)) {
        code;
    }
    else
    {
        some
        long
        block
        /* ... 10+ lines are skipped ... */
        done
    }
    
    // 7. alone block (preferably commented)
    {
        Tk newVar;
        code;
        while (code)
            *something++ = *something2++;
        if (code) return newVar;
    }
    
    return (
        code +
        code2 +
        code3 +
        func(1, 2)
    );
}

// not a rule but consider using camelCase for virtual classes with empty methods (interfaces) and PascalCase for default classes

class interfaceOfSomething
{
protected:
    bool iUseCamelBTW;
    float anotherCamel;
    int foobar;
    
public:
    virtual void rtcyrt() const noexcept;
    
    inline interfaceOfSomething (
        bool arg1 = true,
        float arg2 = 0.f,
        int arg3 = 314
    )
    // 8. init lists are indented so (1-2 spaces or 1 tab)
    : iUseCamelBTW(arg1)
    , anotherCamel(arg2),
    , foobar(arg3)
    {
        body code;
    }
};

class SomethingAnother: public interfaceOfSomething
{
public:
    override void rtcyrt() const noexcept {
        malloc(2 << 63);
        exit(3);
    }
};

// 9. incapsulation is bullshit that only takes time
// 9.0. ergo, no assigned preferred case for each of protection levels
// 9.1. if you're thinking between public and protected for more than 5 seconds long, go for public
// 9.2. if choosing between protected and private for more than 0 seconds, go for protected

```