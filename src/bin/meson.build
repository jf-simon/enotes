inc = include_directories('.', '../..')
deps = [
    elm,
    edje,
    ecore,
    ecorex
]
executable('enotes', [
    'enotes_main.c'
  ],
  include_directories: inc,
  dependencies       : deps,
  gui_app            : true,
  install_dir        : join_paths(dir_bin),
  install            : true)

#executable('ephoto_thumbnail', [
  #  'ephoto_ipc.c',
  #  'ephoto_thumbnailer.c',
  #  'ephoto_thumb.c'
  #],
 # include_directories: inc,
 # dependencies       : deps,
  #install_dir        : join_paths(dir_lib, proj),
 # install            : true)
