project('WT anonymous chat', 'cpp')
WT_ROOT = get_option('wtroot')
c = meson.get_compiler('cpp')
src = meson.source_root()
dirs = [src + '/lib/x86', WT_ROOT + '/lib', WT_ROOT + '/lib/x86_64-linux-gnu', WT_ROOT + '/lib/i386-linux-gnu']
incs = include_directories(WT_ROOT + '/include')

wtlibs = ['wt', 'wthttp']
wtdeps = []

foreach l : wtlibs
  wtdeps += c.find_library(l, dirs : dirs)
endforeach

conf_data = configuration_data()
conf_data.set('MAX_MSG_COUNT', get_option('msgcount'))
conf_data.set('MAX_MSG_PERIOD', get_option('msgperiod'))
conf_data.set('MAX_MSG_LENGTH', get_option('msglen'))
conf_data.set('MAX_NAME_LENGTH', get_option('namelen'))
configure_file(input : 'src/config.h.in',
               output : 'config.h',
               configuration : conf_data)

wt = declare_dependency(dependencies : wtdeps, include_directories : incs)
deps = [wt]
src = [
  'src/main.cpp',
  'src/ChatApp.cpp',
  'src/ChatWidget.cpp',
  'src/State.cpp',
  'src/utils.cpp'
]

executable('wtchat', src, 
	build_rpath : '$ORIGIN/lib',
	dependencies : deps
)
