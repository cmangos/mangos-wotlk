realmd_sources = [
    'AuthSocket.cpp',
    'Main.cpp',
    'RealmList.cpp'
]

realmd_inc_dirs = []

if utfcpp_dep.found() == false
  realmd_inc_dirs = [realmd_inc_dirs, '../../dep/include/utf8cpp']
endif

if build_machine.system() == 'windows'
  realmd_inc_dirs = realmd_inc_dirs + ['../../dep/lib/include', '../../dep/lib/include/mysql']
endif

realmd_inc = include_directories(realmd_inc_dirs)

local_linker = _linker_args

if build_machine.system() == 'darwin'
  local_linker = [_linker_args, '-Wl,no_fixup_chains -framework Carbon']
endif

if get_option('BUILD_LOGIN_SERVER')
executable('realmd',
            realmd_sources,
            link_with: [sharedlib],
            include_directories: [realmd_inc, global_includes],
            dependencies: [boost_dep, dblib_dep, zlib_dep, openssl_dep, utfcpp_dep],
            link_args: local_linker,
            install: true
)
install_data('realmd.conf.dist.in', rename: 'realmd.conf.dist', install_dir: 'etc')
endif