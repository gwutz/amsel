icon_sizes = [
  '24x24',
  '32x32',
  '48x48',
  '256x256',
  '512x512',
]

amsel_datadir = join_paths(get_option('prefix'), get_option('datadir'))

foreach icon_size: icon_sizes
  install_subdir(
    'hicolor/' + icon_size,
    install_dir: join_paths(amsel_datadir, 'icons', 'hicolor')
  )
endforeach