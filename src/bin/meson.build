inc = include_directories('.', '../..')
deps = [
    elm,
    edje,
    ecore,
    curl_dep,
    ecorex,
    depnls
]

src = [
  'enotes.h',
  'enotes_main.c',
  'enotes_systray.c',
  'enotes_caldav_check.c',
  'enotes_caldav.c',
  'enotes_caldav_get.c',
  'enotes_caldav_put.c',
  'enotes_caldav_del.c',
  'enotes_settings.c',
  'enotes_help.c'
]
executable('enotes', 
  src,
  include_directories: inc,
  dependencies       : deps,
  gui_app            : true,
  install_dir        : join_paths(dir_bin),
  install            : true)
