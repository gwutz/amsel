if get_option('with_gtk_doc')
gnome = import('gnome')

glib_prefix = dependency('glib-2.0').get_pkgconfig_variable('prefix')
glib_docpath = join_paths(glib_prefix, 'share', 'gtk-doc', 'html')
docpath = join_paths(get_option('datadir'), 'gtk-doc', 'html')

gnome.gtkdoc('libalb',
  main_xml: 'libalb-docs.sgml',
  src_dir: [
    join_paths(meson.source_root(), 'lib'),
    join_paths(meson.build_root(), 'lib')
  ],
  scan_args: [
    '--rebuild-types',
  ],
  dependencies: [
    libalb_dep,
  ],
  gobject_typesfile: 'libalb.types',
  fixxref_args: [
    '--html-dir=@0@'.format(docpath),
    '--extra-dir=@0@'.format(join_paths(glib_docpath, 'glib')),
    '--extra-dir=@0@'.format(join_paths(glib_docpath, 'gobject')),
    '--extra-dir=@0@'.format(join_paths(glib_docpath, 'gio')),
    '--extra-dir=@0@'.format(join_paths(glib_docpath, 'gi')),
    '--extra-dir=@0@'.format(join_paths(glib_docpath, 'gtk3')),
  ],
  install_dir: 'libalb',
  install: true,
)
endif