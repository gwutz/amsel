aml_sources = [
  'main.c',
  'aml-application.c',
  'aml-application-window.c',
  'aml-add-feed-dialog.c',
  'downloader/aml-downloader.c',
  aml_resource,
]

gtk_dep = dependency('gtk+-3.0')
soup_dep = dependency('libsoup-2.4')
webkit_dep = dependency('webkit2gtk-4.0')

sharedlibpath = join_paths(get_option('prefix'), get_option('libdir'))

executable('amsel',
  aml_sources,
  dependencies: [libalb_dep, gtk_dep, soup_dep, webkit_dep],
  install_rpath: sharedlibpath,
  install: true,
)