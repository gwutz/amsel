# icondir = join_paths(get_option('datadir'), 'icons/hicolor')

gnome = import('gnome')

amsel_resources = gnome.compile_resources('amsel-resources',
  'amsel.gresource.xml',
  c_name: 'amsel'
)

# install_data ('org.gnome.Amsel.png', install_dir: join_paths(icondir, '256x256/apps'))