cmd = [ edje_cmd,
        '-id', join_paths(meson.source_root(), 'data' , 'themes', 'images'),
        '@INPUT@', '@OUTPUT@'
      ]
src = 'enotes'
custom_target(src + '.edj',
              input      : src + '.edc',
              output     : src + '.edj',
              command    : cmd,
              install_dir: join_paths(dir_data, proj, 'themes'),
              install    : true
             )
             
install_data('entry_theme.edj',
             install_dir: join_paths(dir_data, 'enotes/themes'))
meson.add_install_script('perms.sh')
