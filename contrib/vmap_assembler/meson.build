assembler_args = ''
if get_option('DEBUG')
  assembler_args = [assembler_args, '-DIOMAP_DEBUG']
endif

assembler_args = [assembler_args, '-DNO_CORE_FUNCS']

if build_machine.system() == 'linux'
  assembler_args = [assembler_args, '-ggdb']
endif

vmap_assembler_sources = [
  '../../src/game/Vmap/BIH.cpp',
  '../../src/game/Vmap/VMapManager2.cpp',
  '../../src/game/Vmap/MapTree.cpp',
  '../../src/game/Vmap/TileAssembler.cpp',
  '../../src/game/Vmap/WorldModel.cpp',
  '../../src/game/Vmap/ModelInstance.cpp',
  'vmap_assembler.cpp'
]

executable('vmap_assembler',
            vmap_assembler_sources,
            link_with: [sharedlib, g3dlib, frameworklib],
            include_directories: global_includes,
            dependencies: [boost_dep, zlib_dep],
            cpp_args: assembler_args,
            link_args: _linker_args,
            install_dir: 'bin/tools',
            install: true
)