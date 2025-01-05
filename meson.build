project('CMaNGOS WotLK', 'cpp', 'c',
  license: 'GPL-2.0-only',
  meson_version: '>=1.3.1',
  default_options : ['cpp_std=vc++17,c++17', 'buildtype=release', 'warning_level=0', 'default_library=static'])

if build_machine.system() == 'windows'
  add_project_arguments('/Zc:strictStrings-', language : 'cpp')
endif

_linker_args = ''

boost_dep = dependency('boost', version: '>=1.66.0', static: true, modules: ['thread', 'program_options', 'system', 'filesystem'], required: false)
if not boost_dep.found()
  message('Static Boost libraries could not be found. Trying dynamic libraries')
  boost_dep = dependency('boost', version: '>=1.66.0', static:false, modules: ['thread', 'program_options', 'system', 'filesystem'], required: true)
endif
dblib_dep = dependency('mariadb', 'libmariadb', 'mysql', 'libmysql', 'postgresql', required: false)
zlib_dep = dependency('zlib', version: '>=1.2.13', static: true, fallback: ['zlib', 'zlib_dep'], required: true)
openssl_dep = dependency('openssl', version: '>=3.0.14', static: true, fallback: ['openssl'], required: true)
utfcpp_dep = dependency('utf8cpp', 'utfcpp', static: true, required: false)
sqlite_dep = dependency('sqlite3', static: true, fallback: ['sqlite3'], required: get_option('SQLITE'))

global_includes = include_directories('src/game', 'dep/libmpq', 'src', 'src/framework', 'src/game/Vmap',
                                      'dep/json', 'dep/g3dlite', 'src/shared', 'dep/recastnavigation',
                                      'dep/recastnavigation/Recast/Include', 'dep/recastnavigation/Detour/Include',
                                      'src/game/MotionGenerators', 'dep/include/bzip2', 'dep/include/gsoap',
                                      'dep/include')

if not get_option('SQLITE')
  if not dblib_dep.found()
    if build_machine.system() == 'windows'
      mysql_dirs = [
        'C:\Program Files\MySQL\MySQL Server 8.0\lib',
        'C:\Program Files\MySQL\MySQL Server 5.7\lib',
        'C:\Program Files\MariaDB 11.0\lib',
        'C:\Program Files\MariaDB 11.1\lib',
        'C:\Program Files\MariaDB 11.2\lib',
        get_option('LIBMYSQL_PATH'),
      ]

      fs = import('fs')
      if fs.is_dir('C:\Program Files\MySQL\MySQL Server 8.0\lib')
        mysql_dll = 'C:\Program Files\MySQL\MySQL Server 8.0\lib\libmysql.dll'
      elif fs.is_dir('C:\Program Files\MySQL\MySQL Server 5.7\lib')
        mysql_dll = 'C:\Program Files\MySQL\MySQL Server 5.7\lib\libmysql.dll'
      elif fs.is_dir('C:\Program Files\MariaDB 11.2\lib')
        mysql_dll = 'C:\Program Files\MariaDB 11.2\lib\libmariadb.dll'
      elif fs.is_dir('C:\Program Files\MariaDB 11.1\lib')
        mysql_dll = 'C:\Program Files\MariaDB 11.1\liblibmariadb.dll'
      elif fs.is_dir('C:\Program Files\MariaDB 11.0\lib')
        mysql_dll = 'C:\Program Files\MariaDB 11.0\lib\libmariadb.dll'
      elif fs.is_dir(get_option('LIBMYSQL_PATH'))
        mysql_dll = get_option('LIBMYSQL_PATH') + '\libmysql.dll'
      else
        error('No local MySQL Server installation found')
      endif

      dblib_dep = meson.get_compiler('cpp').find_library('mysql', dirs: mysql_dirs, static: true, required: false)
      if not dblib_dep.found()
        dblib_dep = meson.get_compiler('cpp').find_library('mariadb', dirs: mysql_dirs, static: true, required: false)
      endif
      install_data(mysql_dll, install_dir: get_option('bindir'))
    else
      error('No local MySQL Server installation found')
    endif
  endif
else
  dblib_dep = sqlite_dep
endif

if not dblib_dep.found()
  error('No local SQL Server installation found')
endif

if get_option('POSTGRESQL')
  add_project_arguments('-DDO_POSTGRESQL', language: 'cpp')
endif

if get_option('SQLITE')
  add_project_arguments('-DDO_SQLITE=1', language: 'cpp')
endif

if get_option('DEBUG')
  add_project_arguments('-g3', language: 'cpp')
endif

if not get_option('WARNINGS')
  add_project_arguments('-w', language: 'cpp')
endif

if build_machine.system() == 'linux'
  add_project_arguments('-DSYSCONFDIR="../etc/"', language: 'cpp')
endif

add_project_arguments('-DBOOST_ALL_NO_LIB', language: 'cpp')

if get_option('BUILD_SCRIPTDEV')
  add_project_arguments('-DBUILD_SCRIPTDEV', language: 'cpp')
endif
if get_option('BUILD_PLAYERBOT')
  add_project_arguments('-DBUILD_PLAYERBOT', language: 'cpp')
endif
if get_option('BUILD_AHBOT')
  add_project_arguments('-DBUILD_AHBOT', language: 'cpp')
endif
if get_option('BUILD_METRICS')
  add_project_arguments('-DBUILD_METRICS', language: 'cpp')
endif

if get_option('BUILD_DOCS')
  subdir('doc')
endif

if get_option('BUILD_RECASTDEMOMOD')
  error('RECASTDEMOMOD currently unavailable')
  if build_machine.system() == 'windows'
    if not get_option('BUILD_EXTRACTORS')
      subdir('contrib/mmap')
    endif
    subdir('contrib/recastdemomod')
  else
    message('BUILD_RECASTDEMOMOD forced to OFF. Not supported on non windows system.')
  endif
endif

if get_option('BUILD_GIT_ID')
  subdir('contrib/git_id')
endif

subdir('dep')
if get_option('BUILD_GAME_SERVER') or get_option('BUILD_LOGIN_SERVER') or get_option('BUILD_EXTRACTORS')
  subdir('src')
endif

if get_option('BUILD_EXTRACTORS')
  extractor_script_file = files('contrib/extractor_scripts/ExtractResources.sh')
  movemapgen_script_file = files('contrib/extractor_scripts/MoveMapGen.sh')
  config_json_file = files('contrib/extractor_scripts/config.json')
  offmesh_file = files('contrib/extractor_scripts/offmesh.txt')
  subdir('contrib/extractor')
  subdir('contrib/vmap_extractor')
  subdir('contrib/vmap_assembler')
  subdir('contrib/mmap')
endif

