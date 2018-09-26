gnome = import('gnome')
pkg = import('pkgconfig')

alb_conf = configuration_data()
alb_conf.set10('ALB_ENABLE_TRACING', get_option('enable_tracing'))
alb_debug_h = configure_file(
  input: 'alb-debug.h.in',
  output: 'alb-debug.h',
  configuration: alb_conf,
)

inc = include_directories(
  '.',
  'engine',
  'model',
  'utils',
  'logging',
)

amsel_public_headers = [
  'alb.h',

  'engine/alb-engine.h',
  'engine/alb-request.h',
  'engine/alb-validator.h',
  'engine/alb-parser.h',
  'engine/alb-cache.h',
  'engine/alb-database.h',
  'engine/alb-sqlite-database.h',
  'model/alb-channel.h',
  'model/alb-entry.h',
  'opml/alb-opml.h',
  'logging/alb-log.h',
]

install_headers(amsel_public_headers, subdir: 'alb')

amsel_sources = [
  'engine/alb-engine.c',
  'engine/alb-request.c',
  'engine/alb-validator.c',
  'engine/alb-validator-rss.c',
  'engine/alb-validator-atom.c',
  'engine/alb-parser.c',
  'engine/alb-parser-xml.c',
  'engine/alb-parser-rss.c',
  'engine/alb-parser-atom.c',
  'engine/alb-cache.c',
  'engine/alb-database.c',
  'engine/alb-sqlite-database.c',
  'model/alb-channel.c',
  'model/alb-entry.c',
  'utils/alb-date-parser.c',
  'utils/alb-sqlite-helper.c',
  'opml/alb-opml.c',
  'logging/alb-log.c',
]

amsel_dependencies = [
  dependency('glib-2.0'),
  dependency('gio-2.0'),
  dependency('gobject-2.0'),
  dependency('libxml-2.0'),
  dependency('sqlite3'),
]

alb_lib = library ('alb-1.0',
  amsel_sources + amsel_resources,
  dependencies: amsel_dependencies,
  include_directories: inc,
  c_args: ['-DALB_COMPILATION'],
  install: true,
)

libalb_dep = declare_dependency(
  dependencies: amsel_dependencies,
  link_with: alb_lib,
  include_directories: inc,
)

pkg.generate(
  name: 'alb-1.0',
  description: 'the AmseL Backend - a generic rss/atom engine',
  libraries: [alb_lib],
  subdirs: 'alb',
)

gnome.generate_gir(alb_lib,
  sources: amsel_public_headers + amsel_sources,
  nsversion: '1.0',
  dependencies: [amsel_dependencies],
  namespace: 'Alb',
  symbol_prefix: 'alb',
  identifier_prefix: 'Alb',
  includes: [ 'GLib-2.0', 'Gio-2.0' ],
  install: true,
  install_dir_gir: girdir,
  install_dir_typelib: typelibdir,
)

# libamsel_debug = configuration_data()
# libamsel_debug.set10('ENABLE_TRACING', get_option('enable_tracing'))
# libamsel_debug_h = configure_file(
#   input: 'amsel-debug.h.in',
#   output: 'amsel-debug.h',
#   configuration: libamsel_debug,
# )

# amsel_lib_headers = [
#   'amsel-feed-provider.h',
#   'amsel-feed-provider-manager.h',
#   'amsel-feed-persistence.h',
#   'amsel-engine.h',
#   'amsel-util.h',
#   'model/amsel-channel.h',
#   'model/amsel-entry.h',
#   'opml/amsel-opml.h',

#   libamsel_debug_h,
# ]

# install_headers(amsel_lib_headers)

# amsel_lib_sources = [
#   'amsel-feed-provider.c',
#   'amsel-feed-provider-manager.c',
#   'amsel-feed-persistence.c',
#   'amsel-engine.c',
#   'amsel-util.c',
#   'model/amsel-channel.c',
#   'model/amsel-entry.c',
#   'opml/amsel-opml.c',

#   'amsel-debug.c',
# ]

# amsel_lib_deps = [
#   dependency('glib-2.0', version: '>= 2.56'),
#   dependency('gio-2.0', version: '>= 2.50'),
#   dependency('gdk-pixbuf-2.0'),
#   dependency('gobject-introspection-1.0'),
#   dependency('libpeas-1.0'),
#   dependency('libsoup-2.4'),
#   dependency('libxml-2.0'),
# ]

# amsellib = library('amsel-1.0', amsel_lib_sources + amsel_resources,
#   dependencies: amsel_lib_deps,
#   install: true,
# )

# libamsel_dep = declare_dependency(
#          dependencies: amsel_lib_deps,
#             link_with: amsellib,
#   include_directories: include_directories('.'),
# )

# amsel_sources = [
#   'main.c',
#   'amsel-application.c',
#   'amsel-application-window.c',
#   'amsel-feed-popover.c',
#   'amsel-preferences.c',
#   'amsel-store.c',
#   'amsel-date.c',
# ]

# amsel_deps = [
#   dependency('gio-2.0', version: '>= 2.50'),
#   dependency('gtk+-3.0', version: '>= 3.22'),
#   dependency('webkit2gtk-4.0'),
#   dependency('libpeas-1.0'),
# ]

# executable('amsel', amsel_sources,
#   dependencies: amsel_deps,
#   link_with: [amsellib],
#   install: true,
# )

# gnome.generate_gir(amsellib,
#   sources: amsel_lib_headers + amsel_lib_sources + amsel_resources,
#   nsversion: '1.0',
#   dependencies: [amsel_lib_deps],
#   namespace: 'Amsel',
#   symbol_prefix: 'amsel',
#   identifier_prefix: 'Amsel',
#   includes: [ 'Peas-1.0', 'Gtk-3.0', 'Gio-2.0' ],
#   install: true,
#   install_dir_gir: pkggirdir,
#   install_dir_typelib: pkgtypelibdir,
# )